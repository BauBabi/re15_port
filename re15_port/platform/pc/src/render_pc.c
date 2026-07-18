/*
 * RE1.5 Rebuilt — PC render backend (Phase 4.2, 2026-05-18).
 *
 * SDL2-based 320x240 software framebuffer in a scaled window (4x = 1280x960).
 * RGB888 surface. Manual 6x8 bitmap font for debug text.
 *
 * Architecture:
 *   - SDL_Window  (host window, 1280x960)
 *   - SDL_Renderer + SDL_Texture (320x240, streaming, GPU-accelerated upscale)
 *   - software framebuffer = u32[320*240] in our RAM
 *   - re15_render_end_frame() uploads framebuffer to texture, presents
 *
 * Matches PSX semantics: 320x240 logical resolution, clear color per frame.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "re15_engine.h"
#include "re15_pri.h"           /* shared sprite.pri depth model (re15_pri_mask_camera_z) */
#include "re15_msg.h"           /* shared .msg text layout walk (re15_msg_layout) */
#include "re15_tim.h"           /* re15_tim_t — the YOU DIED game-over graphic */
#include "re15_fade.h"          /* the screen-fade channel engine (SCD 0x56/0x57, FUN_80021880) */
#include "re15_itps.h"          /* re15_itps_pixel — the item-get modal quad picture (ITPS.ITP, U11) */
#include "re15_item_prompt.h"   /* re15_item_prompt_walk — replay the prompt glyphs in the game font */
#include "shadow_blob_data.h"   /* RE1.5 char shadow blob, extracted from TEX.TIM */

#define WINDOW_SCALE 4

static SDL_Window   *s_window = NULL;
static SDL_Renderer *s_renderer = NULL;
static SDL_Texture  *s_texture = NULL;          /* 320x240 software framebuffer */
static uint32_t      s_framebuffer[SCREEN_XRES * SCREEN_YRES];
/* SUBTITLE OVERLAY (2026-06-08): subtitle glyphs go into THIS layer (not the
 * software framebuffer) so end_frame can blit them AFTER the 3D tris — else
 * Leon's feet (drawn on top of the framebuffer) cover the text. RGBA8888 with
 * alpha; cleared transparent each frame; text pixels forced opaque (alpha=0xFF). */
static uint32_t      s_text_overlay[SCREEN_XRES * SCREEN_YRES];
static SDL_Texture  *s_text_overlay_tex = NULL;
static int           s_text_overlay_used = 0;

/* ── RE1.5 character shadow (FUN_8001b064 / FUN_8001af5c) ──────────────────
 * The PSX shadow is a POLY_FT4 floor quad textured with a soft round blob
 * (extracted from TEX.TIM, shadow CLUT row), blended in ABR mode 2
 * (SUBTRACTIVE: dst = dst - src) and modulated by 0x808080 (neutral). We
 * reproduce it 1:1: an SDL texture of the blob + a custom REV_SUBTRACT blend
 * mode, drawn as a quad on the floor under each actor. Drawn AFTER the BG
 * (so it darkens the helipad floor) and BEFORE the character tris (so the
 * character occludes its own feet). */
static SDL_Texture *s_shadow_tex   = NULL;
static SDL_BlendMode s_shadow_blend = SDL_BLENDMODE_NONE;
typedef struct { SDL_Vertex v[6]; } shadow_quad_t;
#define SHADOW_QUAD_MAX 16
static shadow_quad_t s_shadow_quads[SHADOW_QUAD_MAX];
static int           s_shadow_quad_count = 0;

/* ── Item-get pickup MODAL overlay (byte-true FUN_8001db28) ─────────────────
 * The picked-up item's 112×72 picture zooming/spinning/flipping in, drawn as a
 * textured quad ON TOP of the frozen 3D scene (after the meshes in end_frame —
 * the game is paused underneath, NOT cleared). The 4 screen corners come from
 * the FSM (item_modal_common.c), which already did the byte-true scale/rotate/
 * flip; the texture is the byte-true per-item ITPS.ITP picture (re15_itps_pixel,
 * id×0x3000, 8-bit+CLUT) built once per item type. */
#define ITEM_MODAL_TW RE15_ITPS_W   /* 112 */
#define ITEM_MODAL_TH RE15_ITPS_H   /*  72 */
static int          s_item_modal_on = 0;
static int          s_item_modal_x[4], s_item_modal_y[4];
static SDL_Texture *s_item_modal_tex = NULL;
static int          s_item_modal_tex_type = -1;

/* BJ-round 2026-05-29: CINEMATIC LETTERBOX bars.
 * RE2/RE1.5 cutscenes (the intro etc.) draw black bars at the top & bottom of
 * the 320x240 frame — measured from the PSX ablauf at ~17px each. The bars hide
 * the topmost building/window row that's visible during normal gameplay (see
 * non-cutscene.png). This is NOT overscan/zoom: the scene underneath is the full
 * frame; the bars simply overlay it. The main loop sets the bar height each
 * frame (= cinematic active ? ~17 : 0) via re15_render_pc_set_letterbox(). */
static int           s_letterbox_h = 0;
static uint8_t       s_fade_alpha = 0;   /* BN-round: cinematic fade-in overlay (255=black .. 0=none) */
static uint8_t       s_title_fade = 0;   /* front-end fade: black OVER the title/menu (movie -> title) */
static uint8_t       s_white_alpha = 0;  /* YOU-DIED chain: ADDITIVE white overlay (FUN_80021880 ABR1) */
static int           s_black_bg = 0;     /* YOU-DIED chain: flat-black background (FUN_80021634(2,0)) */
static int           s_go_flyin = -1;    /* YOU DIED letter fly-in tick 0..50 (-1 = classic full draw) */
static SDL_Texture  *s_gameover_tex = NULL;   /* YOU DIED graphic (YOUDIED.TIM), converted once */
static int           s_gameover_w = 0, s_gameover_h = 0, s_gameover_show = 0;
static SDL_Texture  *s_title_tex = NULL;      /* TITLE screen (TITLEU.TIM 320x240 16bpp) */
static SDL_Texture  *s_config_tex = NULL;     /* OPTIONS/CONFIG screen (C_BACK2.TIM 320x240 16bpp) */
static int           s_config_show = 0, s_config_tab = 0;
/* OPTIONS sub-screen layers (byte-true FSM @0x8002dde4). pc_run_config repopulates these each frame;
 * end_frame composites: backdrop -> CONFIG.TIM tile overlay -> semi-transparent highlight rects -> text.
 * The tile overlay carries the picker "TYPE A B C"/"EDIT" and SOUND boxes (opaque, cover the baked tabs);
 * the highlight rects are the flat semi-transparent TILEs (blue 0x000080 cursor / yellow 0xc0c000 EDIT). */
static uint32_t      s_cfg_ov[SCREEN_XRES * SCREEN_YRES];
static SDL_Texture  *s_cfg_ov_tex = NULL;
static int           s_cfg_ov_used = 0;
#define CFG_RECT_MAX 8
static struct { int x, y, w, h; uint8_t r, g, b, a; } s_cfg_rects[CFG_RECT_MAX];
static int           s_cfg_rect_n = 0;
static int           s_title_show = 0;
static SDL_Texture  *s_select_tex = NULL;     /* PLAYER-SELECT bg (SELECTH.TIM 320x240 16bpp) */
static int           s_select_show = 0;
static int           s_select_sel = 0;        /* 0=Leon,1=Elza (scene+0x394) */
static SDL_Texture  *s_selecth3_tex = NULL;   /* PLAYER-SELECT name/profile text atlas (SELECTH3.TIM 256x256 8bpp) */
static int           s_pselect_text_show = 0;
/* CONFIRM-ZOOM overlays (TITLE.BIN sub3-7): sub4 fades the 2D layer (backdrop + idle text) to black;
 * sub5/6 slide the selected char's name+subtitle SPRT rows toward the target rect. Reset each frame in
 * re15_render_pc_player_select and re-set by the confirm FSM. */
static int           s_pselect_dim = 0;       /* 0=full, 255=black: black overlay over backdrop + colour-mod on idle text */
static int           s_slide_show = 0, s_slide_sel = 0, s_slide_nx, s_slide_ny, s_slide_sx, s_slide_sy;
static int           s_pselect_groupa = 1;    /* apply the Group-A half-screen dim (1=idle/sub4; 0 once the char pans) */
static int           s_select_pulse = 0x80;   /* highlight ramp counter scene+0x32e (0..0x80) */
static SDL_Texture  *s_fmv_tex = NULL;        /* STR movie frame (FE-3, updated per frame) */
static int           s_fmv_show = 0;
static int           s_fmv_w = 0, s_fmv_h = 0;
static SDL_Texture  *s_card_tex = NULL;       /* FE-4 memory-card BG (DATA/TYPE00.TIM 320x240) */
static int           s_card_show = 0;
static int           s_card_cur_x = 0, s_card_cur_y = 0, s_card_cur_show = 0;  /* card cursor (▶) */
static SDL_Texture  *s_tmoji[4][2];    /* FE-1 title-menu sprites [row 0-2 + copyright][0 white,1 blue] */
static int           s_tmoji_built = 0, s_tmoji_show = 0, s_tmoji_cursor = 0;
static int           s_tmoji_pulse_ctr = 0, s_tmoji_pulse_val = 0x80;   /* highlight pulse (FUN_801028ec) */
static uint32_t      rgb555_to_argb8888(uint16_t c);   /* fwd (defined with the TIM converters) */

/* Phase 4.5.5: textured-triangle layer.
 *
 * The 2D framebuffer (above) holds text/tile/sprite primitives that are
 * drawn pixel-by-pixel via put_pixel(). For textured 3D triangles we use
 * SDL_RenderGeometry directly against an SDL_Texture (host GPU sampling)
 * because doing software triangle rasterization in our own framebuffer
 * would be 100+ lines of careful scanline code for the same visual result.
 *
 * Order each frame:
 *   1. Clear framebuffer (begin_frame)
 *   2. Game draws into framebuffer (text, tiles, sprites)
 *   3. end_frame: UpdateTexture+RenderCopy framebuffer onto renderer
 *   4. end_frame: flush textured-tri queue via SDL_RenderGeometry
 *   5. end_frame: RenderPresent
 *
 * The queue is bounded to keep per-frame allocations off the heap. */
#define TEXTRI_QUEUE_MAX 8192   /* was 2048 ("~700 quads" = ONE actor) — ROOM1140 draws 7 actors
                                 * (Leon + W01 + 5 zombies + corpses) and saturated the queue, which
                                 * silently dropped everything queued after it (the effect sprites:
                                 * muzzle/smoke/shell + gore were never drawn). 8192 ≈ 4 rooms' worth. */

/* TIM slot pool — allows multiple characters/props to have their own
 * textures. Slot 0 = player default (Leon); other slots for NPCs/props. */
#define RE15_TIM_SLOT_MAX 26   /* 24/25 = weapon-in-hand model TIMs (W01 knife / W03 gun)
                                * 0=Leon 1=Elliot 2=heli-legacy 3=pilot-legacy
                                * 4..9 = ROOM1170 obj0..5 generic prop TIMs
                                * 10 = em21 (legacy); 11..18 = generic enemy banks
                                * (re15_enemy g_enemy[0..7], globalization 2026-06-13) */
typedef struct {
    SDL_Texture *tex;
    int          w, h, one_clut_h, n_cluts, clut_base_y;
    int          loaded;
} re15_tim_slot_t;
static re15_tim_slot_t s_tim_slots[RE15_TIM_SLOT_MAX];
static int             s_active_slot = 0;

/* Back-compat globals point at the active slot. Updated on bind. */
static SDL_Texture *s_tim_texture     = NULL;

/* AZ-round 2026-05-28: BG texture used for sprite.pri foreground-occluder
 * patch overdraw. Updated whenever a new BG cut is loaded (via the new
 * re15_render_pc_set_bg_image API). Per-frame, after character triangles
 * are flushed, we SDL_RenderCopy mask rectangles from this texture onto
 * the renderer surface so character pixels behind BG geometry are hidden.
 */
static SDL_Texture *s_bg_pri_texture = NULL;
/* Per-cut sprite.pri FOREGROUND ATLAS (256x256 RGBA, index0=transparent), decoded
 * from the room's BSS SLD block (byte-true game pixels). The overdraw samples THIS
 * (not the main BG — that was the AZ-round bug that produced ghost sky patches).  */
static SDL_Texture *s_pri_atlas_tex = NULL;
static int          s_pri_atlas_w = 0, s_pri_atlas_h = 0;
typedef struct { int src_x, src_y, dst_x, dst_y, w, h, depth; } re15_pri_rect_t;
#define RE15_PRI_RECTS_MAX 64
static re15_pri_rect_t s_pri_rects[RE15_PRI_RECTS_MAX];
static int             s_pri_rect_count = 0;
/* Player screen position + camera-space Z (set per frame). A mask is skipped ONLY if
 * it is BEHIND the player (mask camera-Z = depth<<4 >= player_z, since the sprite.pri
 * depth is the original otz>>4 = camera-Z>>4) AND it OVERLAPS the player on screen — so a
 * foreground piece the player is standing in front of stops covering him, WITHOUT
 * dropping foreground the player doesn't even overlap (that wrongly made the box's far
 * lower masks transparent). player_z=0 → no gate (draw all). */
static int             s_pri_player_z  = 0;
static int             s_pri_player_sx = 0, s_pri_player_sy = 0;
static int          s_tim_w           = 0;
static int          s_tim_h           = 0;
static int          s_tim_one_clut_h  = 0;
static int          s_tim_n_cluts     = 1;
static int          s_tim_clut_base_y = 0;

static void update_active_slot_globals(void) {
    re15_tim_slot_t *s = &s_tim_slots[s_active_slot];
    if (s->loaded) {
        s_tim_texture     = s->tex;
        s_tim_w           = s->w;
        s_tim_h           = s->h;
        s_tim_one_clut_h  = s->one_clut_h;
        s_tim_n_cluts     = s->n_cluts;
        s_tim_clut_base_y = s->clut_base_y;
    }
}

void re15_render_pc_bind_tim_slot(int slot) {
    if (slot < 0 || slot >= RE15_TIM_SLOT_MAX) return;
    if (!s_tim_slots[slot].loaded) {
        /* LOUD failure: caller expected this slot bound but it's empty.
         * Keep s_active_slot unchanged → subsequent tris use PREVIOUS
         * slot's texture (which is wrong but visible — better than zero). */
        static int s_warned[RE15_TIM_SLOT_MAX] = {0};
        if (!s_warned[slot]) {
            s_warned[slot] = 1;
            fprintf(stderr, "[render] bind_tim_slot(%d) FAILED — slot not loaded; tris will use slot %d (last bound)\n",
                    slot, s_active_slot);
        }
        return;
    }
    s_active_slot = slot;
    update_active_slot_globals();
}

int re15_render_pc_dbg_slot_loaded(int slot) {
    if (slot < 0 || slot >= RE15_TIM_SLOT_MAX) return 0;
    return s_tim_slots[slot].loaded;
}

/* Phase 4.5.7.7 (2026-05-19): per-tri depth field for back-to-front sort.
 *
 * IMPORTANT: SDL_RenderGeometry reads a FLAT SDL_Vertex array (3 verts/tri,
 * no per-tri metadata). We cannot embed depth inside textri_t and still
 * cast its address to (const SDL_Vertex *) — the per-tri padding would
 * desync SDL's stride. So we hold depth in a PARALLEL array and use it as
 * the sort key; after sorting, we copy the verts into a flat SDL_Vertex
 * array for the SDL call. */
typedef struct {
    SDL_Vertex v[3];   /* SDL2 vertex format: position + color + texcoord */
} textri_verts_t;

static textri_verts_t s_textri_queue[TEXTRI_QUEUE_MAX];
static float          s_textri_depth[TEXTRI_QUEUE_MAX];
static uint8_t        s_textri_slot [TEXTRI_QUEUE_MAX];  /* TIM slot per tri */
static int            s_textri_count = 0;

/* Temporary buffer used to emit the sorted-by-depth tri list to SDL.
 * Allocated once at TEXTRI_QUEUE_MAX size; lives in BSS so no per-frame
 * malloc. */
static SDL_Vertex     s_textri_flush_buf[TEXTRI_QUEUE_MAX * 3];

/* Minimal 6x8 bitmap font for ASCII 0x20..0x7F. Each glyph 6 columns, 8 rows.
 * Stored as 8 bytes per glyph, each byte = one row (LSB = leftmost pixel). */
static const uint8_t s_font6x8[96][8] = {
    /* 0x20 ' ' */ {0,0,0,0,0,0,0,0},
    /* 0x21 '!' */ {0x04,0x04,0x04,0x04,0x04,0,0x04,0},
    /* 0x22 '"' */ {0x0A,0x0A,0,0,0,0,0,0},
    /* 0x23 '#' */ {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0,0},
    /* 0x24 '$' */ {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04,0},
    /* 0x25 '%' */ {0x18,0x19,0x02,0x04,0x08,0x13,0x03,0},
    /* 0x26 '&' */ {0x08,0x14,0x14,0x08,0x15,0x12,0x0D,0},
    /* 0x27 ''' */ {0x04,0x04,0,0,0,0,0,0},
    /* 0x28 '(' */ {0x02,0x04,0x08,0x08,0x08,0x04,0x02,0},
    /* 0x29 ')' */ {0x08,0x04,0x02,0x02,0x02,0x04,0x08,0},
    /* 0x2A '*' */ {0,0x04,0x15,0x0E,0x15,0x04,0,0},
    /* 0x2B '+' */ {0,0x04,0x04,0x1F,0x04,0x04,0,0},
    /* 0x2C ',' */ {0,0,0,0,0,0x04,0x04,0x08},
    /* 0x2D '-' */ {0,0,0,0x1F,0,0,0,0},
    /* 0x2E '.' */ {0,0,0,0,0,0,0x04,0},
    /* 0x2F '/' */ {0,0x01,0x02,0x04,0x08,0x10,0,0},
    /* 0x30 '0' */ {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E,0},
    /* 0x31 '1' */ {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E,0},
    /* 0x32 '2' */ {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F,0},
    /* 0x33 '3' */ {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E,0},
    /* 0x34 '4' */ {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02,0},
    /* 0x35 '5' */ {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E,0},
    /* 0x36 '6' */ {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E,0},
    /* 0x37 '7' */ {0x1F,0x01,0x02,0x04,0x08,0x08,0x08,0},
    /* 0x38 '8' */ {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E,0},
    /* 0x39 '9' */ {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C,0},
    /* 0x3A ':' */ {0,0x04,0,0,0,0x04,0,0},
    /* 0x3B ';' */ {0,0x04,0,0,0,0x04,0x04,0x08},
    /* 0x3C '<' */ {0x02,0x04,0x08,0x10,0x08,0x04,0x02,0},
    /* 0x3D '=' */ {0,0,0x1F,0,0x1F,0,0,0},
    /* 0x3E '>' */ {0x08,0x04,0x02,0x01,0x02,0x04,0x08,0},
    /* 0x3F '?' */ {0x0E,0x11,0x01,0x02,0x04,0,0x04,0},
    /* 0x40 '@' */ {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E,0},
    /* 0x41 'A' */ {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11,0},
    /* 0x42 'B' */ {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E,0},
    /* 0x43 'C' */ {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E,0},
    /* 0x44 'D' */ {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C,0},
    /* 0x45 'E' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F,0},
    /* 0x46 'F' */ {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10,0},
    /* 0x47 'G' */ {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F,0},
    /* 0x48 'H' */ {0x11,0x11,0x11,0x1F,0x11,0x11,0x11,0},
    /* 0x49 'I' */ {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E,0},
    /* 0x4A 'J' */ {0x07,0x02,0x02,0x02,0x02,0x12,0x0C,0},
    /* 0x4B 'K' */ {0x11,0x12,0x14,0x18,0x14,0x12,0x11,0},
    /* 0x4C 'L' */ {0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0},
    /* 0x4D 'M' */ {0x11,0x1B,0x15,0x15,0x11,0x11,0x11,0},
    /* 0x4E 'N' */ {0x11,0x11,0x19,0x15,0x13,0x11,0x11,0},
    /* 0x4F 'O' */ {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E,0},
    /* 0x50 'P' */ {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10,0},
    /* 0x51 'Q' */ {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D,0},
    /* 0x52 'R' */ {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11,0},
    /* 0x53 'S' */ {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E,0},
    /* 0x54 'T' */ {0x1F,0x04,0x04,0x04,0x04,0x04,0x04,0},
    /* 0x55 'U' */ {0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0},
    /* 0x56 'V' */ {0x11,0x11,0x11,0x11,0x11,0x0A,0x04,0},
    /* 0x57 'W' */ {0x11,0x11,0x11,0x15,0x15,0x15,0x0A,0},
    /* 0x58 'X' */ {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11,0},
    /* 0x59 'Y' */ {0x11,0x11,0x11,0x0A,0x04,0x04,0x04,0},
    /* 0x5A 'Z' */ {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F,0},
    /* 0x5B '[' */ {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E,0},
    /* 0x5C '\' */ {0,0x10,0x08,0x04,0x02,0x01,0,0},
    /* 0x5D ']' */ {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E,0},
    /* 0x5E '^' */ {0x04,0x0A,0x11,0,0,0,0,0},
    /* 0x5F '_' */ {0,0,0,0,0,0,0,0x1F},
    /* 0x60 '`' */ {0x08,0x04,0x02,0,0,0,0,0},
    /* 0x61 'a' */ {0,0,0x0E,0x01,0x0F,0x11,0x0F,0},
    /* 0x62 'b' */ {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E,0},
    /* 0x63 'c' */ {0,0,0x0E,0x11,0x10,0x11,0x0E,0},
    /* 0x64 'd' */ {0x01,0x01,0x0F,0x11,0x11,0x11,0x0F,0},
    /* 0x65 'e' */ {0,0,0x0E,0x11,0x1F,0x10,0x0E,0},
    /* 0x66 'f' */ {0x06,0x09,0x08,0x1E,0x08,0x08,0x08,0},
    /* 0x67 'g' */ {0,0,0x0F,0x11,0x11,0x0F,0x01,0x0E},
    /* 0x68 'h' */ {0x10,0x10,0x1E,0x11,0x11,0x11,0x11,0},
    /* 0x69 'i' */ {0x04,0,0x04,0x04,0x04,0x04,0x0E,0},
    /* 0x6A 'j' */ {0x02,0,0x02,0x02,0x02,0x12,0x0C,0},
    /* 0x6B 'k' */ {0x10,0x10,0x12,0x14,0x18,0x14,0x12,0},
    /* 0x6C 'l' */ {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E,0},
    /* 0x6D 'm' */ {0,0,0x1A,0x15,0x15,0x15,0x15,0},
    /* 0x6E 'n' */ {0,0,0x1E,0x11,0x11,0x11,0x11,0},
    /* 0x6F 'o' */ {0,0,0x0E,0x11,0x11,0x11,0x0E,0},
    /* 0x70 'p' */ {0,0,0x1E,0x11,0x11,0x1E,0x10,0x10},
    /* 0x71 'q' */ {0,0,0x0F,0x11,0x11,0x0F,0x01,0x01},
    /* 0x72 'r' */ {0,0,0x16,0x19,0x10,0x10,0x10,0},
    /* 0x73 's' */ {0,0,0x0F,0x10,0x0E,0x01,0x1E,0},
    /* 0x74 't' */ {0x08,0x08,0x1E,0x08,0x08,0x09,0x06,0},
    /* 0x75 'u' */ {0,0,0x11,0x11,0x11,0x13,0x0D,0},
    /* 0x76 'v' */ {0,0,0x11,0x11,0x11,0x0A,0x04,0},
    /* 0x77 'w' */ {0,0,0x11,0x11,0x15,0x15,0x0A,0},
    /* 0x78 'x' */ {0,0,0x11,0x0A,0x04,0x0A,0x11,0},
    /* 0x79 'y' */ {0,0,0x11,0x11,0x11,0x0F,0x01,0x0E},
    /* 0x7A 'z' */ {0,0,0x1F,0x02,0x04,0x08,0x1F,0},
    /* 0x7B '{' */ {0x02,0x04,0x04,0x08,0x04,0x04,0x02,0},
    /* 0x7C '|' */ {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0},
    /* 0x7D '}' */ {0x08,0x04,0x04,0x02,0x04,0x04,0x08,0},
    /* 0x7E '~' */ {0x09,0x15,0x12,0,0,0,0,0},
    /* 0x7F     */ {0,0,0,0,0,0,0,0},
};

/* [#42] byte-true PSX clear colour. Texture is SDL_PIXELFORMAT_RGBA8888 = 0xRRGGBBAA,
 * so the PSX `setRGB0(draw_env, 8, 16, 48)` (psx/render.c:157) = R8 G16 B48, opaque
 * alpha → 0x081030FF. The old 0x00081830 was byte-shifted (R0 G8 B24 A0x30 = wrong
 * colour AND semi-transparent). Visible only on boot/error frames before the first
 * BG blit (the BG memcpy overwrites it otherwise). */
#define CLEAR_RGB  0x081030FFu

static void put_pixel(int x, int y, uint32_t rgba)
{
    if (x < 0 || x >= SCREEN_XRES) return;
    if (y < 0 || y >= SCREEN_YRES) return;
    s_framebuffer[y * SCREEN_XRES + x] = rgba;
}

/* Exposed to asset_pc.c for TIM blitting */
uint32_t *re15_pc_framebuffer(void) { return s_framebuffer; }
void re15_pc_put_pixel(int x, int y, uint32_t rgba) { put_pixel(x, y, rgba); }

/* Phase 4.5.10-F: debug getters — surface render state to HUD so we can
 * tell "no Leon = TIM upload failed" vs "no Leon = no tris queued" vs
 * "no Leon = all tris culled". Captured at flush time (before reset).
 *
 * Phase 4.5.10-G: BBX snapshot — getter must return latched values, NOT
 * live ones, because the HUD print happens BEFORE the bone loop fills
 * the bbox (and begin_frame already reset it to 0,0,0,0). */
static int s_dbg_last_textri_count = 0;
static int s_dbg_min_sx = 0, s_dbg_max_sx = 0;
static int s_dbg_min_sy = 0, s_dbg_max_sy = 0;
static int s_dbg_bbox_valid = 0;
static int s_dbg_last_min_sx = 0, s_dbg_last_max_sx = 0;
static int s_dbg_last_min_sy = 0, s_dbg_last_max_sy = 0;
int re15_render_pc_dbg_textri_count(void)    { return s_dbg_last_textri_count; }
/* Discard the queued textured tris (3D meshes) for this frame — used by a full-screen framebuffer
 * overlay (the inventory) so the meshes, which end_frame draws ON TOP of the framebuffer, don't
 * cover it. Call after the scene render + overlay draw, before re15_render_end_frame. */
void re15_render_pc_clear_textris(void)      { s_textri_count = 0; }
/* Per-tri vertex alpha for SUBSEQUENTLY queued tris (PSX ABE semi-transparency: the effect
 * sprites draw ABR0 = 0.5*back + 0.5*front -> alpha 128 with SDL BLEND). Reset to 255 after. */
static int s_tri_alpha = 255;
void re15_render_pc_set_tri_alpha(int a) { s_tri_alpha = (a < 0) ? 0 : (a > 255) ? 255 : a; }
int re15_render_pc_dbg_tim_loaded(void)      { return s_tim_texture != NULL ? 1 : 0; }
int re15_render_pc_dbg_min_sx(void)          { return s_dbg_last_min_sx; }
int re15_render_pc_dbg_max_sx(void)          { return s_dbg_last_max_sx; }
int re15_render_pc_dbg_min_sy(void)          { return s_dbg_last_min_sy; }
int re15_render_pc_dbg_max_sy(void)          { return s_dbg_last_max_sy; }

void re15_render_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    /* WINDOWED by default, at the largest integer scale that FITS the screen (<=90% of the usable
     * desktop) — so a big monitor gets scale 4 but a small one shrinks instead of covering the whole
     * display and reading as "fullscreen". RE15_WINDOW_SCALE=<1..8> forces a scale; RE15_FULLSCREEN
     * opts into real (desktop) fullscreen (Steam Deck / TV). The logical render size letterboxes the
     * 4:3 320x240 image to any panel. */
    int scale = WINDOW_SCALE;
    SDL_Rect ub;
    if (SDL_GetDisplayUsableBounds(0, &ub) == 0) {
        while (scale > 1 && (SCREEN_XRES * scale > ub.w * 9 / 10 || SCREEN_YRES * scale > ub.h * 9 / 10))
            scale--;
    }
    { const char *ws = getenv("RE15_WINDOW_SCALE"); if (ws && *ws) { int s = atoi(ws); if (s >= 1 && s <= 8) scale = s; } }
    Uint32 win_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (getenv("RE15_FULLSCREEN")) win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    fprintf(stderr, "[window] %s %dx%d\n",
            (win_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) ? "FULLSCREEN (RE15_FULLSCREEN set)" : "windowed",
            SCREEN_XRES * scale, SCREEN_YRES * scale);
    s_window = SDL_CreateWindow(
        "RE1.5 Rebuilt — PC",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_XRES * scale, SCREEN_YRES * scale,
        win_flags);
    if (!s_window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(1);
    }

    /* RE15_SOFTWARE_RENDER forces SDL's CPU rasteriser instead of the GPU. Needed to capture a
     * CORRECT framebuffer in a headless / disconnected-session context, where the accelerated
     * renderer has no real display surface and produces a black/garbled image — this is what
     * broke visual-verification screenshots. Slower, but pixel-correct, and no VSYNC stall. */
    Uint32 rflags = getenv("RE15_SOFTWARE_RENDER")
                        ? SDL_RENDERER_SOFTWARE
                        : (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    s_renderer = SDL_CreateRenderer(s_window, -1, rflags);
    if (!s_renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        exit(1);
    }

    s_texture = SDL_CreateTexture(s_renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_XRES, SCREEN_YRES);
    if (!s_texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        exit(1);
    }

    /* Subtitle overlay layer — alpha-blended, blitted on top of the 3D. */
    s_text_overlay_tex = SDL_CreateTexture(s_renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        SCREEN_XRES, SCREEN_YRES);
    if (s_text_overlay_tex)
        SDL_SetTextureBlendMode(s_text_overlay_tex, SDL_BLENDMODE_BLEND);

    /* Logical render size so SDL handles scale/letterbox for us */
    SDL_RenderSetLogicalSize(s_renderer, SCREEN_XRES, SCREEN_YRES);

    /* Clear framebuffer to dark blue */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) {
        s_framebuffer[i] = CLEAR_RGB;
    }

    /* RE1.5 shadow blob: upload the extracted TEX.TIM blob + build the
     * subtractive blend (ABR mode 2: result = dst*1 - src*1). The blob's
     * RGB IS the amount subtracted from the floor (center bright = darkest
     * shadow, edge black = transparent), matching the PSX GPU exactly. */
    s_shadow_tex = SDL_CreateTexture(s_renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC,
        RE15_SHADOW_BLOB_W, RE15_SHADOW_BLOB_H);
    if (s_shadow_tex) {
        SDL_UpdateTexture(s_shadow_tex, NULL, re15_shadow_blob_rgba,
                          RE15_SHADOW_BLOB_W * (int)sizeof(uint32_t));
        s_shadow_blend = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE,  SDL_BLENDFACTOR_ONE,  SDL_BLENDOPERATION_REV_SUBTRACT, /* RGB: dst - src */
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE,  SDL_BLENDOPERATION_ADD);          /* A: keep dst */
        SDL_SetTextureBlendMode(s_shadow_tex, s_shadow_blend);
    }
}

/* Toggle windowed <-> (desktop) fullscreen. Bound to F11 / Alt+Enter (event loop below) and to a
 * SELECT+START controller combo (input_pc.c). Desktop fullscreen + the logical render size scales
 * and letterboxes the 4:3 320x240 image to fill any panel (e.g. the Steam Deck's 1280x800). */
void re15_render_pc_toggle_fullscreen(void)
{
    if (!s_window) return;
    int is_fs = (SDL_GetWindowFlags(s_window) & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 1 : 0;
    SDL_SetWindowFullscreen(s_window, is_fs ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_ShowCursor(is_fs ? SDL_ENABLE : SDL_DISABLE);   /* hide the pointer while fullscreen */
}

/* Queue one shadow quad (4 projected floor corners in the FUN_8001af5c order:
 * 0=(-X,+Z)->uv(0,0), 1=(-X,-Z)->uv(1,0), 2=(+X,+Z)->uv(0,1), 3=(+X,-Z)->uv(1,1)).
 * Two tris cover the quad. Subtractive blend is set on the texture. */
/* Colored variant — the DEATH BLOOD POOL (LAB_8003694c: the shadow quad's color words flip to
 * 0x00ffff38 = R 0x38 G 0xff B 0xff on the SUBTRACTIVE blend -> all green/blue and ~78%% red are
 * subtracted under the quad = a dark-RED pool). */
void re15_render_shadow_quad_c(int x0, int y0, int x1, int y1,
                               int x2, int y2, int x3, int y3,
                               uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_shadow_tex || s_shadow_quad_count >= SHADOW_QUAD_MAX) return;
    const SDL_Color col = { r, g, b, 255 };
    SDL_FPoint p[4] = { {(float)x0,(float)y0}, {(float)x1,(float)y1},
                        {(float)x2,(float)y2}, {(float)x3,(float)y3} };
    SDL_FPoint t[4] = { {0.f,0.f}, {1.f,0.f}, {0.f,1.f}, {1.f,1.f} };
    const int idx[6] = { 0, 2, 3, 0, 3, 1 };
    shadow_quad_t *q = &s_shadow_quads[s_shadow_quad_count++];
    for (int i = 0; i < 6; i++) {
        q->v[i].position  = p[idx[i]];
        q->v[i].tex_coord = t[idx[i]];
        q->v[i].color     = col;
    }
}

void re15_render_shadow_quad(int x0, int y0, int x1, int y1,
                             int x2, int y2, int x3, int y3)
{
    if (!s_shadow_tex || s_shadow_quad_count >= SHADOW_QUAD_MAX) return;
    const SDL_Color white = { 255, 255, 255, 255 };  /* no extra modulation (0x808080 prim = neutral) */
    SDL_FPoint p[4] = { {(float)x0,(float)y0}, {(float)x1,(float)y1},
                        {(float)x2,(float)y2}, {(float)x3,(float)y3} };
    SDL_FPoint t[4] = { {0.f,0.f}, {1.f,0.f}, {0.f,1.f}, {1.f,1.f} };
    /* quad corners in space: 0(-X,+Z) 2(+X,+Z) 3(+X,-Z) 1(-X,-Z) → tris (0,2,3),(0,3,1) */
    const int idx[6] = { 0, 2, 3, 0, 3, 1 };
    shadow_quad_t *q = &s_shadow_quads[s_shadow_quad_count++];
    for (int i = 0; i < 6; i++) {
        q->v[i].position  = p[idx[i]];
        q->v[i].tex_coord = t[idx[i]];
        q->v[i].color     = white;
    }
}

void re15_render_begin_frame(void)
{
    /* Phase 4.5.10-F: reset debug bbox for this frame. */
    s_dbg_bbox_valid = 0;
    s_dbg_min_sx = 0; s_dbg_max_sx = 0;
    s_dbg_min_sy = 0; s_dbg_max_sy = 0;
    /* Pump events so the window stays responsive (close button, etc.) */
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        } else if (e.type == SDL_KEYDOWN &&
                   (e.key.keysym.sym == SDLK_F11 ||
                    (e.key.keysym.sym == SDLK_RETURN && (e.key.keysym.mod & KMOD_ALT)))) {
            re15_render_pc_toggle_fullscreen();     /* F11 / Alt+Enter */
        }
        /* CONTROLLERDEVICEADDED/REMOVED are pumped here; input_pc.c lazily (re)opens the pad. */
    }

    /* Clear framebuffer to background color (matches PSX setRGB0 / isbg=1) */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) {
        s_framebuffer[i] = CLEAR_RGB;
    }
    /* Clear the subtitle overlay (fully transparent) each frame. */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) s_text_overlay[i] = 0;
    s_text_overlay_used = 0;
}

/* Build (or rebuild on type change) the 112×72 ABGR item-picture texture (ITPS.ITP) for the modal quad. */
static void item_modal_build_tex(uint8_t type)
{
    if (s_item_modal_tex && s_item_modal_tex_type == (int)type) return;
    if (!s_item_modal_tex) {
        s_item_modal_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ABGR8888,
                                             SDL_TEXTUREACCESS_STATIC, ITEM_MODAL_TW, ITEM_MODAL_TH);
        if (s_item_modal_tex) SDL_SetTextureBlendMode(s_item_modal_tex, SDL_BLENDMODE_BLEND);
    }
    if (!s_item_modal_tex) return;
    uint32_t px[ITEM_MODAL_TW * ITEM_MODAL_TH];
    int have = re15_itps_available(type);
    for (int v = 0; v < ITEM_MODAL_TH; v++) {
        for (int u = 0; u < ITEM_MODAL_TW; u++) {
            uint8_t r = 0, g = 0, b = 0;
            uint32_t c;
            if (have && re15_itps_pixel(type, u, v, &r, &g, &b))
                c = 0xFF000000u | ((uint32_t)b << 16) | ((uint32_t)g << 8) | r;  /* ABGR opaque */
            else if (!have)
                c = 0xFF604840u;   /* flat card if ITPS.ITP is missing (asset guard) */
            else
                c = 0x00000000u;   /* the picture's own transparent pixel (CLUT entry 0) */
            px[v * ITEM_MODAL_TW + u] = c;
        }
    }
    SDL_UpdateTexture(s_item_modal_tex, NULL, px, ITEM_MODAL_TW * (int)sizeof(uint32_t));
    s_item_modal_tex_type = (int)type;
}

/* Main-loop hook: set the current-frame item-modal quad (4 screen corners TL,TR,BL,BR), or clear it
 * (on=0). Drawn in end_frame after the 3D meshes. `face` is ignored — front/back share the picture
 * (the reverse-side texture row has no port equivalent; faithful-line). */
void re15_render_pc_item_modal(int on, const int *cx, const int *cy, uint8_t type, int face)
{
    (void)face;
    s_item_modal_on = on;
    if (!on) return;
    for (int i = 0; i < 4; i++) { s_item_modal_x[i] = cx[i]; s_item_modal_y[i] = cy[i]; }
    item_modal_build_tex(type);
}

void re15_render_end_frame(void)
{
    /* Step 1: blit the software framebuffer (2D primitives) onto the renderer.
     * FLAT-BLACK BG MODE (YOU-DIED chain, byte-true FUN_80021634(2,0)): the pre-rendered room
     * backdrop is replaced by black — the 3D scene (corpse/zombies) still draws on top. */
    SDL_UpdateTexture(s_texture, NULL, s_framebuffer, SCREEN_XRES * sizeof(uint32_t));
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
    SDL_RenderClear(s_renderer);
    if (!s_black_bg) {
        SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);
    } else if (s_gameover_tex) {
        /* THE SPOTLIGHT BACKDROP (YOU-DIED chain): the glow quadrant of YOUDIED.TIM mirrored 2x2
         * around the screen centre, drawn as the BACKGROUND (the 3D corpse renders OVER it as a
         * dark shape — the original's 4 fullscreen GT4 quads are backdrop prims, drawing it after
         * the 3D washed the corpse out). Two additive passes saturate the core like the original. */
        extern int g_death_glow;
        int gb = g_death_glow + ((int)s_white_alpha * 5) / 8;
        if (gb > 255) gb = 255;
        SDL_Rect gsrc = { 0, 64, 148, 88 };
        int hx = SCREEN_XRES / 2, hy = SCREEN_YRES / 2;
        SDL_Rect q0 = { 0, 0, hx, hy }, q1 = { hx, 0, hx, hy };
        SDL_Rect q2 = { 0, hy, hx, hy }, q3 = { hx, hy, hx, hy };
        SDL_SetTextureBlendMode(s_gameover_tex, SDL_BLENDMODE_ADD);
        for (int pass = 0; pass < 2; pass++) {
            int m = pass ? gb / 2 : gb;                    /* core ~1.5x -> blown-out white */
            SDL_SetTextureColorMod(s_gameover_tex, (Uint8)m, (Uint8)m, (Uint8)m);
            SDL_RenderCopyEx(s_renderer, s_gameover_tex, &gsrc, &q0, 0, NULL, SDL_FLIP_NONE);
            SDL_RenderCopyEx(s_renderer, s_gameover_tex, &gsrc, &q1, 0, NULL, SDL_FLIP_HORIZONTAL);
            SDL_RenderCopyEx(s_renderer, s_gameover_tex, &gsrc, &q2, 0, NULL, SDL_FLIP_VERTICAL);
            SDL_RenderCopyEx(s_renderer, s_gameover_tex, &gsrc, &q3, 0, NULL,
                             (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));
        }
        SDL_SetTextureColorMod(s_gameover_tex, 255, 255, 255);
        SDL_SetTextureBlendMode(s_gameover_tex, SDL_BLENDMODE_BLEND);
    }

    /* Step 1.5: RE1.5 character shadows — subtractive blob quads on the floor.
     * Drawn after the BG (darken the helipad) and before the character tris
     * (so the actor occludes its own feet). Matches PSX FUN_8001b064. */
    if (s_shadow_tex && s_shadow_quad_count > 0) {
        for (int i = 0; i < s_shadow_quad_count; i++)
            SDL_RenderGeometry(s_renderer, s_shadow_tex,
                               s_shadow_quads[i].v, 6, NULL, 0);
    }
    s_shadow_quad_count = 0;

    /* PLAYER-SELECT backdrop (SELECTH.TIM) — drawn as the BACKGROUND here, BEFORE the 3D models
     * (textri, Step 2 below) so Leon(PL00)/Elza(PL01) render ON TOP of the rooftop/debris scene.
     * The half-screen highlight TILEs + the transition fade draw LATER (over the models). */
    if (s_select_show && s_select_tex) {
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderCopy(s_renderer, s_select_tex, NULL, &full);
        /* CONFIRM sub4: fade the BACKDROP to black here (BEFORE the models) so the selected char still
         * renders full-bright over it. Byte-true ABR2 SUBTRACTIVE (backdrop - level) like the RE1.5 fade
         * engine (FUN_800217b0/21880) — clips to black faster + non-uniformly than an alpha blend, which
         * matches the original's ~40-frame backdrop fade (a linear alpha blend was visibly too slow). */
        if (s_pselect_dim > 0) {
            SDL_BlendMode sub = SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
            if (SDL_SetRenderDrawBlendMode(s_renderer, sub) != 0)
                SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);   /* SW fallback = alpha blend */
            SDL_SetRenderDrawColor(s_renderer, (Uint8) s_pselect_dim, (Uint8) s_pselect_dim, (Uint8) s_pselect_dim, 255);
            SDL_RenderFillRect(s_renderer, &full);
        }
    }

    /* Step 2: flush textured triangles ON TOP of the framebuffer copy.
     * SDL_RenderGeometry takes an interleaved vertex array (no indices = each
     * group of 3 vertices is one triangle). We pass our queue as a flat array
     * since SDL_Vertex is the exact memory layout it expects. */
    /* Phase 4.5.7.7 + 2026-06-09 unify: back-to-front Z sort of the textured tris
     * AND the sprite.pri foreground masks in ONE depth order — the PC mirror of the
     * PSX OT depth-sort. SDL_RenderGeometry has no depth buffer, so painter's draw
     * order decides which primitive wins at each pixel. Each mask sits at camera-Z =
     * re15_pri_mask_camera_z(depth) (the SHARED model in re15_pri.h — same value the
     * PSX uses for its OT bucket), so a fixture occludes only an actor genuinely
     * behind it. This REPLACES the old "draw all tris, then blit masks on top with a
     * player-bbox gate" path, which diverged from PSX and left the lamp occluding
     * Leon when he stood in front of it. */
    {
        /* Sort tris by depth descending (far first → painted under nearer tris). */
        int order[TEXTRI_QUEUE_MAX];
        for (int i = 0; i < s_textri_count; i++) order[i] = i;
        for (int i = 1; i < s_textri_count; i++) {
            int   k    = order[i];
            float kdep = s_textri_depth[k];
            int   j    = i - 1;
            while (j >= 0 && s_textri_depth[order[j]] < kdep) {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = k;
        }
        /* Sort masks by camera-Z descending — SAME scale as the tri depth key. */
        int mask_n = (s_pri_atlas_tex && s_pri_rect_count > 0) ? s_pri_rect_count : 0;
        int mask_order[RE15_PRI_RECTS_MAX];
        for (int i = 0; i < mask_n; i++) mask_order[i] = i;
        for (int i = 1; i < mask_n; i++) {
            int k  = mask_order[i];
            int kd = re15_pri_mask_camera_z(s_pri_rects[k].depth);
            int j  = i - 1;
            while (j >= 0 && re15_pri_mask_camera_z(s_pri_rects[mask_order[j]].depth) < kd) {
                mask_order[j + 1] = mask_order[j];
                j--;
            }
            mask_order[j + 1] = k;
        }
        /* Merge-walk: emit consecutive same-slot tri batches; whenever the next mask
         * is farther than (or equal to) the next tri, flush the pending batch and blit
         * that mask first (it belongs UNDER the nearer tri). I5-round GLOBAL z-sort is
         * preserved (batches by slot within the global depth order). */
        int mi = 0;
        if (s_textri_count > 0 && s_tim_texture) {
            uint8_t cur_slot = s_textri_slot[order[0]];
            int batch_start = 0;
            for (int i = 0; i <= s_textri_count; i++) {
                float tri_depth = (i < s_textri_count) ? s_textri_depth[order[i]] : -1e30f;
                uint8_t s = (i < s_textri_count) ? s_textri_slot[order[i]] : 0xFF;
                int mask_due = (mi < mask_n) &&
                    ((float) re15_pri_mask_camera_z(s_pri_rects[mask_order[mi]].depth) >= tri_depth);
                if (i == s_textri_count || s != cur_slot || mask_due) {
                    int n = i - batch_start;
                    if (n > 0 && s_tim_slots[cur_slot].loaded) {
                        for (int j = 0; j < n; j++) {
                            const textri_verts_t *src = &s_textri_queue[order[batch_start + j]];
                            s_textri_flush_buf[j * 3 + 0] = src->v[0];
                            s_textri_flush_buf[j * 3 + 1] = src->v[1];
                            s_textri_flush_buf[j * 3 + 2] = src->v[2];
                        }
                        SDL_RenderGeometry(s_renderer, s_tim_slots[cur_slot].tex,
                                            s_textri_flush_buf, n * 3, NULL, 0);
                    }
                    batch_start = i;
                    cur_slot = s;
                    while (mi < mask_n &&
                           (float) re15_pri_mask_camera_z(s_pri_rects[mask_order[mi]].depth) >= tri_depth) {
                        const re15_pri_rect_t *r = &s_pri_rects[mask_order[mi++]];
                        SDL_Rect src = { r->src_x, r->src_y, r->w, r->h };
                        SDL_Rect dst = { r->dst_x, r->dst_y, r->w, r->h };
                        SDL_RenderCopy(s_renderer, s_pri_atlas_tex, &src, &dst);
                    }
                }
            }
        }
        /* Remaining masks (nearest, or the no-tris case) — blit on top. */
        while (mi < mask_n) {
            const re15_pri_rect_t *r = &s_pri_rects[mask_order[mi++]];
            SDL_Rect src = { r->src_x, r->src_y, r->w, r->h };
            SDL_Rect dst = { r->dst_x, r->dst_y, r->w, r->h };
            SDL_RenderCopy(s_renderer, s_pri_atlas_tex, &src, &dst);
        }
    }

    s_dbg_last_textri_count = s_textri_count;
    s_dbg_last_min_sx       = s_dbg_min_sx;
    s_dbg_last_max_sx       = s_dbg_max_sx;
    s_dbg_last_min_sy       = s_dbg_min_sy;
    s_dbg_last_max_sy       = s_dbg_max_sy;
    s_textri_count = 0;  /* reset queue for next frame */

    /* Step 2.5: ITEM-GET pickup MODAL — the zooming/spinning/flipping item quad, ON TOP of the frozen
     * 3D scene (byte-true FUN_8001db28; gameplay is paused underneath). 2 triangles from the 4 screen
     * corners (TL,TR,BL,BR) with UV 0..1, sampled from the item-icon texture. The corners already carry
     * the byte-true scale/rotate/flip (item_modal_common.c) — SDL maps the texture affinely, so the
     * spin (state 2) and the coin-flip Y-inversion (state 4) render for free. */
    if (s_item_modal_on && s_item_modal_tex) {
        static const int   idx[6] = { 0, 1, 2, 1, 3, 2 };   /* TL,TR,BL + TR,BR,BL */
        static const float uvx[4] = { 0.f, 1.f, 0.f, 1.f };
        static const float uvy[4] = { 0.f, 0.f, 1.f, 1.f };
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Vertex mv[6];
        for (int k = 0; k < 6; k++) {
            int c = idx[k];
            mv[k].position.x  = (float)s_item_modal_x[c];
            mv[k].position.y  = (float)s_item_modal_y[c];
            mv[k].color       = white;
            mv[k].tex_coord.x = uvx[c];
            mv[k].tex_coord.y = uvy[c];
        }
        SDL_RenderGeometry(s_renderer, s_item_modal_tex, mv, 6, NULL, 0);
    }

    /* SCREEN-FADE channels (byte-true FUN_80021880, trace wf_2c73ab52): tick once per rendered
     * frame (the PSX calls it after the game tick, before present @0x80020f44), then draw the
     * active channels' full-screen semi-transparent tiles. OT placement: channels 0-2 target
     * the top OT drawn LAST = over BG+3D, and the SCD bucket 7 sits UNDER the letterbox
     * (bucket 4) + UI -> draw here, after the scene and before the bars. Channel 3 targets the
     * BACKGROUND OT (behind the scene) — no shipped SCD user; skipped (documented). ABR blend:
     * 2 = B - F subtractive (the shipped ROOM11F0/11F1/3080 flicker), 1 = B + F additive;
     * modes 0/3 have no shipped SCD user and fall back to subtractive-darken semantics. */
    re15_fade_tick();
    for (int fch = 0; fch < 3; fch++) {
        const re15_fade_ch_t *fc = &g_fade_ch[fch];
        if (!fc->drawn || (fc->out_r == 0 && fc->out_g == 0 && fc->out_b == 0)) continue;
        SDL_BlendMode bm;
        if (fc->abr == 1) bm = SDL_BLENDMODE_ADD;
        else bm = SDL_ComposeCustomBlendMode(   /* ABR2: dst - src per channel */
                 SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                 SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
        SDL_SetRenderDrawBlendMode(s_renderer, bm);
        SDL_SetRenderDrawColor(s_renderer, fc->out_r, fc->out_g, fc->out_b, 255);
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderFillRect(s_renderer, &full);
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    }

    /* Cinematic LETTERBOX — byte-true FUN_80021a0c (#21, trace wf_bba41002): two static
     * POLY_F4 strips (0,0)-(320,24) + (0,216)-(320,240) whose RGB = the ±0x10/frame counter
     * (g_letterbox_level), code 0x2A semi-trans + DR_MODE ABR=2 SUBTRACTIVE -> pixel =
     * background - level: the bars RAMP smoothly to black over 15 frames instead of popping
     * opaque. OT bucket 4: over the SCD fade (bucket 7, drawn above) and UNDER the subtitle
     * overlay (drawn after) — text sits ON the bars, byte-true. Bars emit only when the
     * counter != 0 (@0x80021b20). The old binary s_letterbox_h path is superseded (the
     * legacy setter remains a no-op shim for the PSX-side main). */
    if (g_letterbox_level != 0) {
        SDL_BlendMode bm = SDL_ComposeCustomBlendMode(   /* ABR2: dst - src per channel */
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
        SDL_SetRenderDrawBlendMode(s_renderer, bm);
        SDL_SetRenderDrawColor(s_renderer, g_letterbox_level, g_letterbox_level,
                               g_letterbox_level, 255);
        SDL_Rect top = { 0, 0, SCREEN_XRES, 24 };
        SDL_Rect bot = { 0, SCREEN_YRES - 24, SCREEN_XRES, 24 };
        SDL_RenderFillRect(s_renderer, &top);
        SDL_RenderFillRect(s_renderer, &bot);
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    }

    /* Subtitle overlay ON TOP of the 3D + foreground AND the letterbox bars (drawn after
     * the character tris + sprite.pri overdraw so the actors' feet no longer cover the text,
     * and after the bars so the cinematic subtitle reads on the black bar — #1B). */
    if (s_text_overlay_used && s_text_overlay_tex) {
        SDL_UpdateTexture(s_text_overlay_tex, NULL, s_text_overlay,
                          SCREEN_XRES * sizeof(uint32_t));
        SDL_RenderCopy(s_renderer, s_text_overlay_tex, NULL, NULL);
    }

    /* BN-round 2026-05-29: cinematic FADE-IN overlay (PSX engine-driven fade,
     * RE2 FUN_8002bda8; measured ablauf ramp 0→full over ~6 engine frames at
     * room entry). Full-screen black with decreasing alpha, drawn LAST (over
     * everything incl. letterbox), so the helipad fades in from black like PSX
     * (and covers our 1-frame stale-BG on the Cut_chg(0) frame). */
    /* ADDITIVE WHITE overlay (YOU-DIED chain: FUN_800217b0 mode1 = ABR1 additive, brightness =
     * level>>7; the white death-flash + the 3 heartbeat pulses). Drawn UNDER the black fade. */
    if (s_white_alpha > 0) {
        /* while the glow backdrop is active, the flat prim only adds the PULSE crest (the glow
         * quads already carry the level) — full flat white only for the pre-glow flash. */
        uint8_t wf = s_black_bg ? (uint8_t)(s_white_alpha / 3) : s_white_alpha;
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(s_renderer, wf, wf, wf, 255);
        SDL_Rect fullw = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderFillRect(s_renderer, &fullw);
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    }
    if (s_fade_alpha > 0) {
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, s_fade_alpha);
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderFillRect(s_renderer, &full);
    }

    /* STR MOVIE frame (FE-3) — full-screen, OVER everything. The opening CAPCOM.STR plays before the
     * title; each decoded frame is uploaded to s_fmv_tex and drawn 1:1 to the 320x240 screen. */
    if (s_fmv_show && s_fmv_tex) {
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderCopy(s_renderer, s_fmv_tex, NULL, &full);
    }

    /* FE-4 MEMORY-CARD screen: full-screen BG (DATA/TYPE00.TIM), the byte-true dim panel behind
     * the slot list (FUN_80026b30: GP0(0x62) 50%% mono rect RGB(0x20) at (18,52) 286x102), then the
     * slot/label text overlay ON TOP. */
    if (s_card_show && s_card_tex) {
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderCopy(s_renderer, s_card_tex, NULL, &full);
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(s_renderer, 0x20, 0x20, 0x20, 128);
        SDL_Rect panel = { 18, 52, 286, 102 };
        SDL_RenderFillRect(s_renderer, &panel);
        if (s_text_overlay_used && s_text_overlay_tex)
            SDL_RenderCopy(s_renderer, s_text_overlay_tex, NULL, NULL);
    }

    /* TITLE screen (TITLEU.TIM) — full-screen, OVER everything. Byte-true tail of the death sequence
     * (YOU DIED -> title) and the boot front-end. Drawn before YOU DIED so a mid-death title wins. */
    if (s_title_show && s_title_tex) {
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderCopy(s_renderer, s_title_tex, NULL, &full);
        /* FE-1.4: re-composite the text overlay ON TOP of the title art (used by the death->title
         * tail; the front-end menu is now TMOJI sprites below, not overlay text). */
        if (s_text_overlay_used && s_text_overlay_tex)
            SDL_RenderCopy(s_renderer, s_text_overlay_tex, NULL, NULL);
    }

    /* OPTIONS/CONFIG screen: backdrop -> CONFIG.TIM tile overlay -> semi-transparent highlight rects ->
     * text overlay (byte-true layering of the config task @0x8002dde4). */
    if (s_config_show && s_config_tex) {
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderCopy(s_renderer, s_config_tex, NULL, &full);
        /* CONFIG.TIM tile overlay (picker "TYPE A B C"/"EDIT", SOUND boxes) — opaque, covers baked tabs. */
        if (s_cfg_ov_used) {
            if (!s_cfg_ov_tex) {
                s_cfg_ov_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888,
                                                 SDL_TEXTUREACCESS_STREAMING, SCREEN_XRES, SCREEN_YRES);
                if (s_cfg_ov_tex) SDL_SetTextureBlendMode(s_cfg_ov_tex, SDL_BLENDMODE_BLEND);
            }
            if (s_cfg_ov_tex) {
                SDL_UpdateTexture(s_cfg_ov_tex, NULL, s_cfg_ov, SCREEN_XRES * 4);
                SDL_RenderCopy(s_renderer, s_cfg_ov_tex, NULL, NULL);
            }
        }
        /* Selected-item highlight: the flat semi-transparent TILE (blue 0x000080 / yellow 0xc0c000),
         * GPU ABE mode 0 = 50/50 blend -> SDL BLEND with the stored alpha. */
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < s_cfg_rect_n; i++) {
            SDL_Rect rr = { s_cfg_rects[i].x, s_cfg_rects[i].y, s_cfg_rects[i].w, s_cfg_rects[i].h };
            SDL_SetRenderDrawColor(s_renderer, s_cfg_rects[i].r, s_cfg_rects[i].g, s_cfg_rects[i].b, s_cfg_rects[i].a);
            SDL_RenderFillRect(s_renderer, &rr);
        }
        /* the action-label / STEREO-MONO text (game font) is written to the text overlay by pc_run_config;
         * re-blit it HERE, over the backdrop (the earlier overlay blit above is under this backdrop). */
        if (s_text_overlay_used && s_text_overlay_tex)
            SDL_RenderCopy(s_renderer, s_text_overlay_tex, NULL, NULL);
    }

    /* PLAYER-SELECT name/profile TEXT (SELECTH3.TIM atlas, groups B+C) — drawn OVER the models and
     * BEFORE the dim TILEs so the unselected side's text dims with its model (byte-true, §render_pc
     * re15_render_pc_pselect_text). Rects verbatim from TITLE.BIN @0x80102624 / @0x8010265c. Logical
     * render size is 320x240 so the coords are used directly. */
    if (s_pselect_text_show && s_selecth3_tex) {
        /* IDLE state draws only the 2 NAMES + 2 PROFILE blocks (verified byte-true against the PSX
         * framebuffer). The 2 subtitle sprites (Group B rows 1/3, "from police department" /
         * "from college dormitory" at screen y=216) are set up by the same @0x80102624 loop but are
         * CONFIRM-ONLY: their blocks are moved on-screen by the zoom animator 0x80100a50, which is
         * gated on scene+0x15c state 2/3 (the confirm-zoom) — in the idle state they are not shown
         * (native has nothing at y=216 there). They render with the zoom (next increment). */
        static const struct { int x, y, w, h, u, v; } ps_text[4] = {
            {  16,  48, 136, 24,  0,   0 },   /* B0 Leon name   UV(0,0)   136x24 */
            { 168,  48, 136, 24,  0,  48 },   /* B2 Elza name   UV(0,48)  136x24 */
            {  24, 168, 120, 48,  0,  88 },   /* C0 Leon PROFILE screen(24,168)  UV(0,88)  120x48 */
            { 176, 168, 120, 48,  0, 136 },   /* C1 Elza PROFILE screen(176,168) UV(0,136) 120x48 */
        };
        /* CONFIRM sub4: fade the idle text ~2x faster than the backdrop (the profile-card colour fade is
         * scene+0x26a over 32 frames vs the backdrop's ~64) — colour-mod toward black. */
        int td = s_pselect_dim * 2; if (td > 255) td = 255;
        Uint8 tm = (Uint8)(255 - td);
        SDL_SetTextureColorMod(s_selecth3_tex, tm, tm, tm);
        if (s_pselect_dim < 255) for (int i = 0; i < 4; i++) {
            SDL_Rect src = { ps_text[i].u, ps_text[i].v, ps_text[i].w, ps_text[i].h };
            SDL_Rect dst = { ps_text[i].x, ps_text[i].y, ps_text[i].w, ps_text[i].h };
            SDL_RenderCopy(s_renderer, s_selecth3_tex, &src, &dst);
        }
        /* CONFIRM sub5/6: the sliding NAME row (full bright, over the faded idle text). The subtitle row
         * (blk1/blk3) is NOT drawn — the original shows only the name during the confirm-zoom (same as
         * the idle, where the subtitle is never visible either; its block slides but is not rendered). */
        if (s_slide_show) {
            SDL_SetTextureColorMod(s_selecth3_tex, 255, 255, 255);
            int nv = s_slide_sel ? 48 : 0;
            SDL_Rect nsrc = { 0, nv, 136, 24 }, ndst = { s_slide_nx, s_slide_ny, 136, 24 };
            SDL_RenderCopy(s_renderer, s_selecth3_tex, &nsrc, &ndst);
            (void) s_slide_sx; (void) s_slide_sy;
        }
        SDL_SetTextureColorMod(s_selecth3_tex, 255, 255, 255);
    }

    /* PLAYER-SELECT scene (TITLE.BIN task @0x80101094): SELECTH.TIM bg re-blit full-screen + the two
     * half-screen highlight TILEs. Leon = left half, Elza = right half, tile rect (x, y=40, w=160,
     * h=200) @0x801004f8. The pulse counter scene+0x32e drives a byte-true grayscale crossfade
     * (@0x80100860/0x80100898): SELECTED dim = (-0x80-counter)&0xff (0x80->0x00 = brightens), OTHER
     * = counter (0x00->0x80 = dims). Drawn here as a semi-transparent black dim overlay (the exact
     * PSX ABR blend rate still to be confirmed from VRAM; the brightness FORMULA is byte-true). */
    if (s_select_show && s_select_tex && s_pselect_groupa) {
        /* (the SELECTH backdrop is drawn earlier, before the 3D models; here = the highlight tiles.) */
        int c = s_select_pulse & 0xff;
        int dim_sel = (-0x80 - c) & 0xff;
        int dim_oth = c & 0xff;
        int leon_dim = (s_select_sel == 0) ? dim_sel : dim_oth;
        int elza_dim = (s_select_sel == 1) ? dim_sel : dim_oth;
        int hx = SCREEN_XRES / 2, ty = SCREEN_YRES * 40 / 240;
        SDL_Rect lr = { 0, ty, hx, SCREEN_YRES - ty };
        SDL_Rect rr = { hx, ty, SCREEN_XRES - hx, SCREEN_YRES - ty };
        /* byte-true ABR2 SUBTRACTIVE (dst - grayscale): SUBTRACT the tile value from the framebuffer
         * (bg + model), so the non-selected half's dark room + model go to BLACK (the PSX semi-trans
         * TILE @0x801004f8). The SDL software renderer doesn't support custom blend -> fall back to a
         * black BLEND overlay (proportional dim) for headless captures; the accelerated renderer (the
         * normal run) does the exact subtractive. */
        SDL_BlendMode sub = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
        if (SDL_SetRenderDrawBlendMode(s_renderer, sub) == 0) {
            if (getenv("RE15_PSELECT_DIAG")) { static int once=0; if(!once){once=1; fprintf(stderr,"PSELECT DIM: SUBTRACT path (leon_dim=%d elza_dim=%d)\n",leon_dim,elza_dim);} }
            SDL_SetRenderDrawColor(s_renderer, (Uint8) leon_dim, (Uint8) leon_dim, (Uint8) leon_dim, 255);
            SDL_RenderFillRect(s_renderer, &lr);
            SDL_SetRenderDrawColor(s_renderer, (Uint8) elza_dim, (Uint8) elza_dim, (Uint8) elza_dim, 255);
            SDL_RenderFillRect(s_renderer, &rr);
        } else {
            if (getenv("RE15_PSELECT_DIAG")) { static int once=0; if(!once){once=1; fprintf(stderr,"PSELECT DIM: FALLBACK blend path (elza_dim=%d)\n",elza_dim);} }
            SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, (Uint8) leon_dim); SDL_RenderFillRect(s_renderer, &lr);
            SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, (Uint8) elza_dim); SDL_RenderFillRect(s_renderer, &rr);
        }
    }

    /* byte-true title MENU: the NEW GAME / LOAD GAME / OPTION sprites + copyright, from TMOJI.TIM.
     * The original (FUN_801027a0) draws each sprite as a DOUBLE EXPOSURE: a subtractive copy (abr=2,
     * E1=0xe10000d5) at Y+1 = a soft shadow, then an ADDITIVE copy (abr=1, E1=0xe10000b5) at Y = the
     * glow — so the letters blend with the eye behind them (semi-transparent), not opaque. We match it:
     * subtractive shadow first, additive glow over. (Index 0 decoded to RGBA 0 → no-op in both blends.) */
    if (s_tmoji_show && s_tmoji_built) {
        static const int ITEM_Y[4] = { 0x85, 0x99, 0xad, 0xc8 };   /* 3 rows @133/153/173 + copyright @200 */
        for (int i = 0; i < 4; i++) {
            int active = (i < 3 && i == s_tmoji_cursor);
            SDL_Texture *tex = (i < 3) ? s_tmoji[i][active ? 0 : 1] : s_tmoji[3][0];
            if (!tex) continue;
            int h = (i < 3) ? 16 : 20;
            /* active row: modulate by the pulse (0x80..0xBE -> 200..255; the white CLUT already keeps
             * it brighter than the blue inactive rows — SDL colour-mod can't exceed 1.0x additive). */
            int mod = active ? (200 + (s_tmoji_pulse_val - 0x80) * 55 / 0x3e) : 255;
            if (mod > 255) mod = 255;
            SDL_SetTextureColorMod(tex, (Uint8) mod, (Uint8) mod, (Uint8) mod);
            SDL_Rect shadow = { 0x20, ITEM_Y[i] + 1, 256, h };
            SDL_Rect glow   = { 0x20, ITEM_Y[i],     256, h };
            SDL_SetTextureBlendMode(tex, s_shadow_blend);   SDL_RenderCopy(s_renderer, tex, NULL, &shadow); /* abr2 */
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD); SDL_RenderCopy(s_renderer, tex, NULL, &glow);   /* abr1 */
        }
    }

    /* front-end fade OVER the title + menu (the CAPCOM intro -> title fade-in). Drawn last so it
     * covers the title art, the logo and the sprites. */
    if (s_title_fade > 0) {
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, s_title_fade);
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderFillRect(s_renderer, &full);
    }

    /* Selection cursor (▶): a small filled white triangle shared by the card screen AND the title
     * menu (the RE1.5 originals draw a cursor sprite; a filled ▶ is the byte-true equivalent marker).
     * Drawn after the card/title composite so it sits on top of whichever screen positioned it. */
    if (s_card_cur_show) {
        SDL_Vertex cv[3];
        for (int k = 0; k < 3; k++) { cv[k].color.r = cv[k].color.g = cv[k].color.b = cv[k].color.a = 255;
                                      cv[k].tex_coord.x = cv[k].tex_coord.y = 0; }
        cv[0].position.x = (float)s_card_cur_x;        cv[0].position.y = (float)(s_card_cur_y + 1);
        cv[1].position.x = (float)s_card_cur_x;        cv[1].position.y = (float)(s_card_cur_y + 11);
        cv[2].position.x = (float)(s_card_cur_x + 8);  cv[2].position.y = (float)(s_card_cur_y + 6);
        SDL_RenderGeometry(s_renderer, NULL, cv, 3, NULL, 0);
    }

    /* GAME-OVER (YOU DIED) — drawn OVER the death fade. The graphic (YOUDIED.TIM) is centred on the
     * faded-black scene. Byte-true death FSM (FUN_8003694c) fades then shows this before the title. */
    if (s_gameover_show && s_gameover_tex) {
        /* YOUDIED.TIM layout (decoded 2026-07-05, shots/youdied_tim_content.png): rows 0..~56 =
         * the red dripping "YOU DIED" letters (256 wide); below = the radial GLOW quadrant
         * (~192x144) — the original draws it as 4 MIRRORED fullscreen GT4 quads = the spotlight
         * backdrop (FUN_80015a80's 4 fullscreen quads), with the flat additive fade prim pulsing
         * ON TOP (the heartbeat). */
        /* the YOU DIED letters (original s17/s19: HUGE — near-full width, ~40%% screen height
         * with a ~2x vertical stretch, vertically centred around ~58%%). Src = the TIM text rows. */
        SDL_Rect tsrc = { 0, 0, 256, 56 };
        int tw = (SCREEN_XRES * 97) / 100;
        int th = (SCREEN_YRES * 42) / 100;
        SDL_Rect tdst = { (SCREEN_XRES - tw) / 2, (SCREEN_YRES * 38) / 100, tw, th };
        if (s_go_flyin >= 0 && s_go_flyin < 50) {
            /* LETTER FLY-IN (byte-true FUN_80015a80: 4 quads glide (target-start)/0x32 over 50
             * frames) — FL: 4 vertical slices of the text strip slide in from alternating sides. */
            int sw = 256 / 4;
            for (int i = 0; i < 4; i++) {
                int rem  = 50 - s_go_flyin;
                int off  = (SCREEN_XRES * rem) / 50;
                int offx = (i & 1) ? off : -off;
                SDL_Rect src = { i * sw, 0, sw, 56 };
                SDL_Rect d2  = { tdst.x + i * (tw / 4) + offx, tdst.y, tw / 4, th };
                SDL_RenderCopy(s_renderer, s_gameover_tex, &src, &d2);
            }
        } else {
            SDL_RenderCopy(s_renderer, s_gameover_tex, &tsrc, &tdst);
            SDL_SetTextureBlendMode(s_gameover_tex, SDL_BLENDMODE_ADD);
            SDL_RenderCopy(s_renderer, s_gameover_tex, &tsrc, &tdst);   /* PSX GT4 0xFF gouraud =
                                                                          * 2x texture brightness */
            SDL_SetTextureBlendMode(s_gameover_tex, SDL_BLENDMODE_BLEND);
        }
    }

    SDL_RenderPresent(s_renderer);
}

/* BN-round 2026-05-29: cinematic fade-in overlay alpha (0=none, 255=black).
 * Main loop ramps it 255→0 over the room-entry fade window. */
void re15_render_pc_set_title_fade(int a) { s_title_fade = (a < 0) ? 0 : (a > 255) ? 255 : (uint8_t)a; }
void re15_render_pc_set_fade(int a)
{
    if (a < 0) a = 0;
    if (a > 255) a = 255;
    s_fade_alpha = (uint8_t)a;
}

/* ADDITIVE white overlay (byte-true FUN_800217b0 mode 1 = ABR 1 + FUN_80021880 fullscreen quad,
 * brightness = level>>7) — the YOU-DIED white flash / heartbeat pulses. */
void re15_render_pc_set_white_fade(int a)
{
    if (a < 0) a = 0;
    if (a > 255) a = 255;
    s_white_alpha = (uint8_t)a;
}

/* Flat-BLACK background mode (byte-true FUN_80021634(2,0)): the pre-rendered room backdrop is
 * replaced by black; the 3D scene (the corpse + zombies) keeps rendering on top. */
void re15_render_pc_set_black_bg(int on) { s_black_bg = on ? 1 : 0; }

/* YOU DIED letter fly-in tick (byte-true FUN_80015a80: 4 letter quads glide (target-start)/0x32
 * over 50 frames). The port draws the YOUDIED.TIM in 4 horizontal strips gliding in from
 * alternating screen sides (faithful-line of the 4 authored letter quads). -1 = classic full. */
void re15_render_pc_set_gameover_flyin(int t) { s_go_flyin = t; }

/* GAME-OVER graphic (YOUDIED.TIM). Converts the indexed TIM to RGBA once, then flags it to draw
 * full-screen (centred) over the death fade each end_frame. re15_render_pc_hide_gameover clears it. */
void re15_render_pc_show_gameover(const re15_tim_t *tim)
{
    if (!s_renderer || !tim || !tim->pixels) return;
    if (!s_gameover_tex) {
        int n = tim->width * tim->height;
        if (n <= 0) return;
        uint32_t *rgba = (uint32_t *) malloc((size_t)n * 4);
        if (!rgba) return;
        if (tim->bpp == 8 && tim->has_clut && tim->clut) {
            const uint8_t *src = (const uint8_t *) tim->pixels;
            for (int i = 0; i < n; i++) {
                uint16_t c = tim->clut[src[i]];
                rgba[i] = (c & 0x7fff) ? rgb555_to_argb8888(c) : 0;   /* RGB-black (incl. STP-black
                                                            * 0x8000) = TRANSPARENT — the letter key */
            }
        } else if (tim->bpp == 16) {
            for (int i = 0; i < n; i++) {
                uint16_t c = tim->pixels[i];
                rgba[i] = (c & 0x7fff) ? rgb555_to_argb8888(c) : 0;
            }
        } else { free(rgba); return; }
        s_gameover_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STATIC, tim->width, tim->height);
        if (s_gameover_tex) {
            SDL_SetTextureBlendMode(s_gameover_tex, SDL_BLENDMODE_BLEND);
            SDL_UpdateTexture(s_gameover_tex, NULL, rgba, tim->width * 4);
            s_gameover_w = tim->width; s_gameover_h = tim->height;
        }
        free(rgba);
    }
    s_gameover_show = 1;
}

void re15_render_pc_hide_gameover(void) { s_gameover_show = 0; }

/* TITLE screen (TITLEU.TIM, 320x240 16bpp RGB555). Converts once, then draws full-screen each
 * end_frame while shown. The byte-true YOU DIED -> title tail (and the boot front-end). */
void re15_render_pc_show_title(const re15_tim_t *tim)
{
    if (!s_renderer || !tim || !tim->pixels) return;
    if (!s_title_tex) {
        int n = tim->width * tim->height;
        if (n <= 0 || tim->bpp != 16) return;
        uint32_t *rgba = (uint32_t *) malloc((size_t)n * 4);
        if (!rgba) return;
        for (int i = 0; i < n; i++) rgba[i] = 0xff000000u | (rgb555_to_argb8888(tim->pixels[i]) & 0xffffffu);
        s_title_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STATIC, tim->width, tim->height);
        if (s_title_tex) SDL_UpdateTexture(s_title_tex, NULL, rgba, tim->width * 4);
        free(rgba);
    }
    s_title_show = 1;
}

void re15_render_pc_hide_title(void) { s_title_show = 0; }

/* OPTIONS/CONFIG screen (EXE task @0x8002dde4). bg = C_BACK2.TIM (16bpp 320x240 — the OPTIONS title,
 * the CONFIG/SOUND/EXIT tabs, the controller graphic + the label boxes are all baked in). tab = the
 * selected top tab (0=CONFIG 1=SOUND 2=EXIT); a highlight bar is drawn over it. */
void re15_render_pc_config(const re15_tim_t *bg, int tab)
{
    if (s_renderer && bg && bg->pixels && bg->bpp == 16 && !s_config_tex) {
        int n = bg->width * bg->height;
        if (n > 0) {
            uint32_t *rgba = (uint32_t *) malloc((size_t) n * 4);
            if (rgba) {
                for (int i = 0; i < n; i++)
                    rgba[i] = 0xff000000u | (rgb555_to_argb8888(bg->pixels[i]) & 0xffffffu);
                s_config_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_STATIC, bg->width, bg->height);
                if (s_config_tex) SDL_UpdateTexture(s_config_tex, NULL, rgba, bg->width * 4);
                free(rgba);
            }
        }
    }
    s_config_show = 1; s_config_tab = tab;
}
void re15_render_pc_hide_config(void)
{
    s_config_show = 0;
    if (s_config_tex) { SDL_DestroyTexture(s_config_tex); s_config_tex = NULL; }
}

/* Clear the OPTIONS sub-screen tile + highlight layers — call once at the top of each config frame. */
void re15_render_pc_config_clear(void)
{
    s_cfg_rect_n = 0;
    if (s_cfg_ov_used) { memset(s_cfg_ov, 0, sizeof(s_cfg_ov)); s_cfg_ov_used = 0; }
}

/* Queue a semi-transparent filled highlight/cover rect (320-space logical coords, a = alpha 0..255). */
void re15_render_pc_config_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (s_cfg_rect_n >= CFG_RECT_MAX) return;
    s_cfg_rects[s_cfg_rect_n].x = x; s_cfg_rects[s_cfg_rect_n].y = y;
    s_cfg_rects[s_cfg_rect_n].w = w; s_cfg_rects[s_cfg_rect_n].h = h;
    s_cfg_rects[s_cfg_rect_n].r = r; s_cfg_rects[s_cfg_rect_n].g = g;
    s_cfg_rects[s_cfg_rect_n].b = b; s_cfg_rects[s_cfg_rect_n].a = a;
    s_cfg_rect_n++;
}

/* Blit an 8bpp CONFIG.TIM sub-region (su,sv,w,h) into the config tile overlay at (dx,dy). idx 0 =
 * transparent. Byte-true source for the picker "TYPE A B C" (uv 0,192 50x24) / "EDIT" (uv 0,216 48x16)
 * and the SOUND label boxes (uv 0,232 88x18) — the opaque tiles that cover the baked CONFIG/SOUND tabs. */
void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy)
{
    if (!t || t->bpp != 8 || !t->has_clut || !t->pixels) return;
    const uint8_t *src = (const uint8_t *) t->pixels;
    for (int y = 0; y < h; y++) {
        int sy = sv + y, oy = dy + y;
        if (sy < 0 || sy >= t->height || (unsigned) oy >= (unsigned) SCREEN_YRES) continue;
        for (int x = 0; x < w; x++) {
            int sx = su + x, ox = dx + x;
            if (sx < 0 || sx >= t->width || (unsigned) ox >= (unsigned) SCREEN_XRES) continue;
            uint8_t idx = src[sy * t->width + sx];
            if (!idx) continue;   /* index 0 = transparent (STP) */
            uint16_t c = t->clut[idx];   /* RGB555 -> RGBA8888 (R high, opaque), matching the overlay texture */
            uint32_t r = ((c >> 0) & 0x1F) << 3, g = ((c >> 5) & 0x1F) << 3, b = ((c >> 10) & 0x1F) << 3;
            s_cfg_ov[oy * SCREEN_XRES + ox] = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
            s_cfg_ov_used = 1;
        }
    }
}

/* EDIT-panel variants that write into the TOP text-overlay layer (drawn last, OVER the 16 config labels) —
 * the opaque "ACT" box occludes the labels behind it, and the blue row cursor blends over the box. */
void re15_render_pc_config_tile_ov(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy)
{
    if (!t || t->bpp != 8 || !t->has_clut || !t->pixels) return;
    const uint8_t *src = (const uint8_t *) t->pixels;
    for (int y = 0; y < h; y++) {
        int sy = sv + y, oy = dy + y;
        if (sy < 0 || sy >= t->height || (unsigned) oy >= (unsigned) SCREEN_YRES) continue;
        for (int x = 0; x < w; x++) {
            int sx = su + x, ox = dx + x;
            if (sx < 0 || sx >= t->width || (unsigned) ox >= (unsigned) SCREEN_XRES) continue;
            uint8_t idx = src[sy * t->width + sx];
            if (!idx) continue;
            uint16_t c = t->clut[idx];
            uint32_t r = ((c >> 0) & 0x1F) << 3, g = ((c >> 5) & 0x1F) << 3, b = ((c >> 10) & 0x1F) << 3;
            s_text_overlay[oy * SCREEN_XRES + ox] = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
            s_text_overlay_used = 1;
        }
    }
}
void re15_render_pc_config_rect_ov(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    for (int py = 0; py < h; py++) {
        int oy = y + py; if ((unsigned) oy >= (unsigned) SCREEN_YRES) continue;
        for (int px = 0; px < w; px++) {
            int ox = x + px; if ((unsigned) ox >= (unsigned) SCREEN_XRES) continue;
            uint32_t e = s_text_overlay[oy * SCREEN_XRES + ox];
            int er = (e >> 24) & 0xFF, eg = (e >> 16) & 0xFF, eb = (e >> 8) & 0xFF;
            int nr = (r * a + er * (255 - a)) / 255, ng = (g * a + eg * (255 - a)) / 255, nb = (b * a + eb * (255 - a)) / 255;
            s_text_overlay[oy * SCREEN_XRES + ox] = ((uint32_t) nr << 24) | ((uint32_t) ng << 16) | ((uint32_t) nb << 8) | 0xFFu;
            s_text_overlay_used = 1;
        }
    }
}

/* PLAYER-SELECT scene bg + highlight state (TITLE.BIN task @0x80101094). bg = SELECTH.TIM (16bpp
 * 320x240, "PLEASE SELECT MAIN CAST" header + rooftop(Leon)/debris-room(Elza) baked in; id 0x1d,
 * re-blit full-screen each frame @FUN_80043870). sel = scene+0x394 (0=Leon,1=Elza); pulse =
 * scene+0x32e (0..0x80) drives the half-screen TILE crossfade drawn in end_frame. */
void re15_render_pc_player_select(const re15_tim_t *bg, int sel, int pulse_counter)
{
    if (s_renderer && bg && bg->pixels && bg->bpp == 16 && !s_select_tex) {
        int n = bg->width * bg->height;
        if (n > 0) {
            uint32_t *rgba = (uint32_t *) malloc((size_t) n * 4);
            if (rgba) {
                for (int i = 0; i < n; i++)
                    rgba[i] = 0xff000000u | (rgb555_to_argb8888(bg->pixels[i]) & 0xffffffu);
                s_select_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_STATIC, bg->width, bg->height);
                if (s_select_tex) SDL_UpdateTexture(s_select_tex, NULL, rgba, bg->width * 4);
                free(rgba);
            }
        }
    }
    s_select_sel = sel; s_select_pulse = pulse_counter; s_select_show = 1;
    s_pselect_dim = 0; s_slide_show = 0; s_pselect_groupa = 1;   /* reset confirm overlays; the FSM re-sets them */
}
/* CONFIRM-ZOOM: sub4 backdrop/idle-text fade level (0..255). */
void re15_render_pc_pselect_dim(int level) { s_pselect_dim = level < 0 ? 0 : level > 255 ? 255 : level; }
/* CONFIRM-ZOOM: turn the idle Group-A half-screen dim off once the selected char starts panning to centre
 * (else the right-half dim would darken the char as it crosses screen centre). */
void re15_render_pc_pselect_groupa(int on) { s_pselect_groupa = on; }
/* CONFIRM-ZOOM: sub5/6 name+subtitle slide — draw the SELECTED char's two SPRT rows at (nx,ny)/(sx,sy).
 * Leon rows = atlas UV(0,0)/(8,26); Elza rows = UV(0,48)/(8,74). */
void re15_render_pc_pselect_slide(int sel, int nx, int ny, int sx, int sy)
{
    s_slide_show = 1; s_slide_sel = sel;
    s_slide_nx = nx; s_slide_ny = ny; s_slide_sx = sx; s_slide_sy = sy;
}
void re15_render_pc_hide_player_select(void)
{
    s_select_show = 0;
    s_pselect_text_show = 0;
    if (s_select_tex) { SDL_DestroyTexture(s_select_tex); s_select_tex = NULL; }
    if (s_selecth3_tex) { SDL_DestroyTexture(s_selecth3_tex); s_selecth3_tex = NULL; }
}

/* PLAYER-SELECT name/profile TEXT overlays (byte-true TITLE.BIN task @0x80101094, RE15_PLAYER_SELECT_DRAW.md
 * groups B+C). The text is a PRE-RENDERED atlas: SELECTH3.TIM (id 0x1f, 8bpp 256x256 + 256-CLUT) with the
 * name colours (Leon "L" blue / Elza "E" red) and the green sub-labels BAKED IN — no runtime font draw,
 * no CLUT swap (verified: SELECT/SELECTH2 are unused, my earlier "3 variants" note was wrong). Index 0 =
 * transparent. The 6 sprites' src UV + screen XY are read verbatim from TITLE.BIN:
 *   Group B (names+subtitles) @0x80102624, 6B/entry [x,y,w,h,u,v]
 *   Group C (PROFILE blocks)  @0x8010265c, [x,y,u,v] u16 + const 120x48
 * Drawn OVER the 3D models but BEFORE the half-screen dim TILEs, so the unselected side's text dims with
 * the model (byte-true: the dim tile is a semi-transparent subtractive rect covering the whole half). */
void re15_render_pc_pselect_text(const re15_tim_t *atlas, int sel)
{
    (void)sel;   /* which side is selected is handled by the dim overlay, not the text */
    if (s_renderer && atlas && atlas->pixels && atlas->bpp == 8 && !s_selecth3_tex) {
        int W = atlas->width, H = atlas->height;   /* 256x256 */
        if (W > 0 && H > 0) {
            uint32_t *rgba = (uint32_t *) malloc((size_t) W * H * 4);
            if (rgba) {
                const uint8_t *idx = (const uint8_t *) atlas->pixels;
                for (int i = 0; i < W * H; i++) {
                    uint8_t p = idx[i];
                    if (p == 0) { rgba[i] = 0; continue; }   /* index 0 = transparent */
                    uint16_t c = (p < atlas->clut_entries) ? atlas->clut[p] : 0;
                    uint32_t r = ((c) & 31) << 3, g = ((c >> 5) & 31) << 3, b = ((c >> 10) & 31) << 3;
                    rgba[i] = 0xFF000000u | (r << 16) | (g << 8) | b;
                }
                s_selecth3_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                                   SDL_TEXTUREACCESS_STATIC, W, H);
                if (s_selecth3_tex) {
                    SDL_UpdateTexture(s_selecth3_tex, NULL, rgba, W * 4);
                    SDL_SetTextureBlendMode(s_selecth3_tex, SDL_BLENDMODE_BLEND);
                }
                free(rgba);
            }
        }
    }
    s_pselect_text_show = 1;
}

/* ---- FE-1: byte-true title MENU sprites (DATA/TMOJI.TIM) ------------------------------------
 * The 3 menu rows + the copyright are GPU SPRITES, not font text (RE'd from TITLE.BIN: menu
 * handler FUN_80102b00, sprite draw FUN_801027a0, descriptor table 0x801028ac). TMOJI.TIM is an
 * 8-bit 256x256 sheet (image VRAM 320,256; 256-entry CLUT VRAM 0,511). Rows are 256x16 strips:
 * v=16 "NEW GAME", v=32 "LOAD GAME", v=48 "OPTION"; the copyright is 256x20 at v=82. The ACTIVE
 * row is sampled through the WHITE sub-palette (CLUT base 0, orig clut 0x7fc0); the others through
 * BLUE (CLUT base 192, orig clut 0x7fcc) — the exact CLUT-swap the original does. Screen positions
 * from the draw code: x=0x20, y=0x85/0x99/0xad; copyright (0x20,0xc8). (The original's semi-
 * transparent double-exposure glow + the 60-frame highlight pulse are not yet reproduced — the
 * strips are drawn opaque; a follow-up.) */
static const struct { int v, h; } s_tmoji_src[4] = { {16,16}, {32,16}, {48,16}, {82,20} };

static SDL_Texture *tmoji_strip(const re15_tim_t *t, int v, int h, int clut_base)
{
    const int W = 256;
    uint32_t *rgba = (uint32_t *) malloc((size_t) W * h * 4);
    if (!rgba) return NULL;
    const uint8_t *idx = (const uint8_t *) t->pixels;   /* 8-bit indices */
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < W; x++) {
            uint8_t p = (v + y < t->height && x < t->width) ? idx[(v + y) * t->width + x] : 0;
            if (p == 0) { rgba[y * W + x] = 0; continue; }              /* index 0 = transparent */
            int ci = clut_base + p;
            uint16_t c = (ci < t->clut_entries) ? t->clut[ci] : 0;
            uint32_t r = ((c) & 31) << 3, g = ((c >> 5) & 31) << 3, b = ((c >> 10) & 31) << 3;
            rgba[y * W + x] = 0xFF000000u | (r << 16) | (g << 8) | b;   /* ARGB8888 */
        }
    }
    SDL_Texture *tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, W, h);
    if (tex) { SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND); SDL_UpdateTexture(tex, NULL, rgba, W * 4); }
    free(rgba);
    return tex;
}

void re15_render_pc_title_menu(const re15_tim_t *tmoji, int cursor)
{
    if (!s_tmoji_built && tmoji && tmoji->pixels && tmoji->clut && tmoji->bpp == 8) {
        s_tmoji_built = 1;
        for (int i = 0; i < 4; i++) {
            s_tmoji[i][0] = tmoji_strip(tmoji, s_tmoji_src[i].v, s_tmoji_src[i].h, 0);     /* white */
            s_tmoji[i][1] = tmoji_strip(tmoji, s_tmoji_src[i].v, s_tmoji_src[i].h, 192);   /* blue  */
        }
    }
    s_tmoji_cursor = cursor;
    s_tmoji_show   = 1;
    /* advance the active-row brightness pulse — byte-true triangle wave (FUN_801028ec @0x80102944):
     * base 0x80, +2 for the first 0x1f frames then -2, reset to 0x80 every 0x3c (60) frames. */
    if (s_tmoji_pulse_ctr < 0x1f) s_tmoji_pulse_val += 2; else s_tmoji_pulse_val -= 2;
    if (++s_tmoji_pulse_ctr >= 0x3c) { s_tmoji_pulse_ctr = 0; s_tmoji_pulse_val = 0x80; }
}
void re15_render_pc_hide_title_menu(void) { s_tmoji_show = 0; }

/* STR MOVIE (FE-3): upload one decoded 320x240 RGBA8888 frame and mark it shown.
 * Unlike the title (cached once), the texture is (re)created on size change and
 * UPDATED every call so successive movie frames present. rgba layout matches the
 * framebuffer: 0xRRGGBBAA. Call re15_render_pc_hide_fmv() when the movie ends. */
void re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h)
{
    if (!s_renderer || !rgba || w <= 0 || h <= 0) return;
    if (s_fmv_tex && (w != s_fmv_w || h != s_fmv_h)) {
        SDL_DestroyTexture(s_fmv_tex);
        s_fmv_tex = NULL;
    }
    if (!s_fmv_tex) {
        s_fmv_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_STATIC, w, h);
        s_fmv_w = w; s_fmv_h = h;
    }
    if (s_fmv_tex) {
        SDL_UpdateTexture(s_fmv_tex, NULL, rgba, w * (int)sizeof(uint32_t));
        s_fmv_show = 1;
    }
}

void re15_render_pc_hide_fmv(void) { s_fmv_show = 0; }

/* FE-4 MEMORY-CARD screen background (DATA/TYPE00.TIM, 320x240 16bpp RGB555 — the byte-true
 * "MEMORY CARD BG", CD file 0x23). Cached once (static art), drawn full-screen while shown. */
void re15_render_pc_show_cardbg(const re15_tim_t *tim)
{
    if (!s_renderer || !tim || !tim->pixels || tim->bpp != 16) return;
    if (!s_card_tex) {
        int n = tim->width * tim->height;
        if (n <= 0) return;
        uint32_t *rgba = (uint32_t *) malloc((size_t)n * 4);
        if (!rgba) return;
        for (int i = 0; i < n; i++)
            rgba[i] = 0xff000000u | (rgb555_to_argb8888(tim->pixels[i]) & 0xffffffu);
        s_card_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STATIC, tim->width, tim->height);
        if (s_card_tex) SDL_UpdateTexture(s_card_tex, NULL, rgba, tim->width * 4);
        free(rgba);
    }
    s_card_show = 1;
}
void re15_render_pc_hide_cardbg(void) { s_card_show = 0; s_card_cur_show = 0; }

/* Position the card-screen selection cursor (▶). show=0 hides it (e.g. blink-off). */
void re15_render_pc_card_cursor(int x, int y, int show) { s_card_cur_x = x; s_card_cur_y = y; s_card_cur_show = show; }

/* BJ-round 2026-05-29: set cinematic letterbox bar height (px, in 320x240
 * space). 0 = no bars (gameplay). The main loop sets this each frame based on
 * cinematic state. PSX intro measured ~17px top & bottom. */
void re15_render_pc_set_letterbox(int h)
{
    if (h < 0) h = 0;
    if (h > 120) h = 120;
    s_letterbox_h = h;
}

/* AZ-round 2026-05-28: upload BG image to a texture used for sprite.pri
 * overdraw. Called by bg_pc.c whenever BG cache is refreshed (per cut).
 * Image is 320x240 RGBA8888. */
void re15_render_pc_set_bg_image(const uint32_t *bg_rgba_320x240)
{
    if (!s_renderer || !bg_rgba_320x240) return;
    if (!s_bg_pri_texture) {
        s_bg_pri_texture = SDL_CreateTexture(s_renderer,
                                             SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_STATIC,
                                             SCREEN_XRES, SCREEN_YRES);
    }
    if (s_bg_pri_texture) {
        SDL_UpdateTexture(s_bg_pri_texture, NULL, bg_rgba_320x240,
                          SCREEN_XRES * sizeof(uint32_t));
    }
}

/* Upload the per-cut sprite.pri foreground atlas (RGBA8888, index0 already keyed
 * to alpha 0) used as the overdraw source. w/h are the atlas pixel dims (256x256).
 * Pass NULL to clear (cut with no foreground). Recreated when the size changes. */
void re15_render_pc_set_pri_atlas(const uint32_t *rgba, int w, int h)
{
    if (!s_renderer || !rgba || w <= 0 || h <= 0) {
        s_pri_atlas_w = s_pri_atlas_h = 0;   /* disable overdraw until next set */
        return;
    }
    if (s_pri_atlas_tex && (w != s_pri_atlas_w || h != s_pri_atlas_h)) {
        SDL_DestroyTexture(s_pri_atlas_tex);
        s_pri_atlas_tex = NULL;
    }
    if (!s_pri_atlas_tex) {
        s_pri_atlas_tex = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_STATIC, w, h);
        if (s_pri_atlas_tex)
            SDL_SetTextureBlendMode(s_pri_atlas_tex, SDL_BLENDMODE_BLEND);
        s_pri_atlas_w = w; s_pri_atlas_h = h;
    }
    if (s_pri_atlas_tex)
        SDL_UpdateTexture(s_pri_atlas_tex, NULL, rgba, w * sizeof(uint32_t));
}

/* AZ-round 2026-05-28: set the active pri mask list for the current cut.
 * Called once per cut change. Empty list (count=0) disables overdraw. */
void re15_render_pc_set_pri_rects(const int *src_x, const int *src_y,
                                  const int *dst_x, const int *dst_y,
                                  const int *w, const int *h,
                                  const int *depth, int count)
{
    if (count > RE15_PRI_RECTS_MAX) count = RE15_PRI_RECTS_MAX;
    s_pri_rect_count = count;
    for (int i = 0; i < count; i++) {
        s_pri_rects[i].src_x = src_x[i];
        s_pri_rects[i].src_y = src_y[i];
        s_pri_rects[i].dst_x = dst_x[i];
        s_pri_rects[i].dst_y = dst_y[i];
        s_pri_rects[i].w     = w[i];
        s_pri_rects[i].h     = h[i];
        s_pri_rects[i].depth = depth ? depth[i] : 0;
    }
}

/* Set the player's on-screen position + camera-space Z for the sprite.pri depth gate
 * (main.c, per frame, from the active cut's view × player position). */
void re15_render_pc_set_pri_player(int sx, int sy, int z)
{
    s_pri_player_sx = sx; s_pri_player_sy = sy; s_pri_player_z = z;
}

/* Dump a TIM slot's SDL_Texture to BMP for inspection. */
void re15_render_pc_dump_slot(int slot, const char *path)
{
    if (slot < 0 || slot >= RE15_TIM_SLOT_MAX) return;
    re15_tim_slot_t *st = &s_tim_slots[slot];
    if (!st->loaded || !st->tex || !s_renderer) return;
    /* Render slot texture full-size to back-buffer, snapshot, then refresh. */
    SDL_Texture *prev_target = SDL_GetRenderTarget(s_renderer);
    SDL_Texture *target = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGB888,
                                             SDL_TEXTUREACCESS_TARGET, st->w, st->h);
    if (!target) return;
    SDL_SetRenderTarget(s_renderer, target);
    SDL_RenderCopy(s_renderer, st->tex, NULL, NULL);
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, st->w, st->h, 32, SDL_PIXELFORMAT_RGB888);
    if (surf) {
        SDL_RenderReadPixels(s_renderer, NULL, SDL_PIXELFORMAT_RGB888,
                              surf->pixels, surf->pitch);
        SDL_SaveBMP(surf, path);
        fprintf(stderr, "[dump] slot %d → %s (%dx%d)\n", slot, path, st->w, st->h);
        SDL_FreeSurface(surf);
    }
    SDL_SetRenderTarget(s_renderer, prev_target);
    SDL_DestroyTexture(target);
}

/* Save current renderer output to a BMP file (for ablauf visual diff).
 * BC-round 2026-05-28: optionally downscale to SCREEN_XRES×SCREEN_YRES
 * (320×240) for high-volume frame-by-frame capture — controlled by env
 * RE15_AUTOSHOT_SMALL=1. Full-resolution BMP at 3.5MB × 2800 frames is
 * too large; downscaled BMP (~230KB) makes per-frame capture feasible. */
void re15_render_pc_screenshot(const char *path)
{
    if (!s_renderer) return;
    int w, h;
    SDL_GetRendererOutputSize(s_renderer, &w, &h);
    SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGB888);
    if (!surf) return;
    if (SDL_RenderReadPixels(s_renderer, NULL, SDL_PIXELFORMAT_RGB888,
                              surf->pixels, surf->pitch) == 0) {
        static int s_small_inited = 0;
        static int s_use_small = 0;
        if (!s_small_inited) {
            const char *e = getenv("RE15_AUTOSHOT_SMALL");
            s_use_small = (e && *e == '1') ? 1 : 0;
            s_small_inited = 1;
        }
        SDL_Surface *out = surf;
        SDL_Surface *small = NULL;
        if (s_use_small && (w != SCREEN_XRES || h != SCREEN_YRES)) {
            small = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_XRES, SCREEN_YRES,
                                                    32, SDL_PIXELFORMAT_RGB888);
            if (small) {
                SDL_Rect dst = {0, 0, SCREEN_XRES, SCREEN_YRES};
                SDL_BlitScaled(surf, NULL, small, &dst);
                out = small;
            }
        }
        if (SDL_SaveBMP(out, path) != 0) {
            fprintf(stderr, "[screenshot] SaveBMP failed: %s\n", SDL_GetError());
        }
        if (small) SDL_FreeSurface(small);
    }
    SDL_FreeSurface(surf);
}

/* Phase 4.5.5: convert a parsed TIM into an SDL_Texture for fast textured-
 * triangle sampling via SDL_RenderGeometry. Called by asset_pc.c after
 * TIM parsing succeeds. Supports 4bpp+CLUT, 8bpp+CLUT, and 16bpp direct.
 *
 * Declared as `extern` in main.c via re15_engine.h won't pick this up since
 * it's PC-specific. asset_pc.c just declares it locally before calling. */
#include "re15_tim.h"

/* Convert RE1.5 16-bit RGB555 (PSX colour format) to SDL ARGB8888.
 *
 * PSX hardware rule: a 16-bit value of exactly 0x0000 in a 16bpp texture
 * means "transparent pixel". For 4bpp / 8bpp the palette-index 0 maps to
 * whatever CLUT[0] holds — almost always 0x0000, so the same rule applies.
 *
 * We honour this for 16bpp textures (UI / icons that genuinely need
 * cutout transparency). For 4bpp / 8bpp we leave the decision to the
 * caller via the `cutout_index_0` flag — RE1.5 model textures often need
 * index 0 to render as opaque black (otherwise the door handle and other
 * dark trim pixels show as holes onto the framebuffer). */
static uint32_t rgb555_to_argb8888(uint16_t c)
{
    int r = ((c >>  0) & 0x1F) << 3;
    int g = ((c >>  5) & 0x1F) << 3;
    int b = ((c >> 10) & 0x1F) << 3;
    return ((uint32_t)0xFF << 24)
         | ((uint32_t)r    << 16)
         | ((uint32_t)g    <<  8)
         |  (uint32_t)b;
}

/* Upload a TIM into a specific slot. slot=0 = default (player). */
void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

void re15_render_pc_upload_tim(const re15_tim_t *tim) {
    re15_render_pc_upload_tim_slot(tim, 0);
}

void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot)
{
    if (!tim || !s_renderer) return;
    if (slot < 0 || slot >= RE15_TIM_SLOT_MAX) return;

    /* Heuristic: for indexed MODEL/PROP TIMs treat index 0 as OPAQUE (CLUT[0] colour)
     * rather than transparent. RE1.5 MD1 textures need this to look right —
     * fully-black trim pixels (door handle, weapon details) are stored as
     * palette index 0 with CLUT[0] = 0x0000 (black). Treating them as
     * transparent produces the "triangle-shaped holes" the user reported.
     *
     * EXCEPTION — the effect-sprite slots (19 = room RDT effect TIM, 20 = GLOBAL effect
     * bank / effect-id 0 hit-blood, see main.c RE15_TIM_SLOT_EFFECT[_GLOBAL]): these are
     * PSX textured SPRITES, where index 0 / CLUT[0]=0x0000 is TRANSPARENT (byte-true GPU
     * sprite semantics). Without this, the 4bpp blood sheet would render as a black box
     * around each splatter (the ~86% index-0 background becomes opaque black). */
    const int treat_index_0_opaque = (slot != 19 && slot != 20);

    /* Phase 4.5.7.5 #PC-2: support multi-CLUT TIMs by decoding each CLUT
     * into its own copy of the TIM, stacked vertically. The texture
     * height becomes `tim->height × n_cluts`. At render time we extract
     * the CLUT index from the per-triangle PSX `clut` word and add
     * `clut_index × one_clut_h` to UV.y. */
    int row_width = (tim->bpp == 4) ? 16 : 256;
    int n_cluts = (tim->has_clut && row_width > 0)
                    ? (tim->clut_entries / row_width)
                    : 1;
    if (n_cluts < 1) n_cluts = 1;

    int pixels_per_clut = tim->width * tim->height;
    int tex_w           = tim->width;
    int tex_h           = tim->height * n_cluts;
    int n_pixels        = tex_w * tex_h;
    uint32_t *rgba      = (uint32_t *) malloc((size_t)n_pixels * 4);
    if (!rgba) return;

    if (tim->bpp == 8 && tim->has_clut) {
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int c = 0; c < n_cluts; c++) {
            const uint16_t *clut_row = &tim->clut[c * 256];
            uint32_t       *out      = &rgba[c * pixels_per_clut];
            for (int i = 0; i < pixels_per_clut; i++) {
                uint8_t idx = src[i];
                if (idx == 0 && !treat_index_0_opaque) {
                    out[i] = 0u;
                } else {
                    out[i] = rgb555_to_argb8888(clut_row[idx]);
                }
            }
        }
    } else if (tim->bpp == 4 && tim->has_clut) {
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int c = 0; c < n_cluts; c++) {
            const uint16_t *clut_row = &tim->clut[c * 16];
            uint32_t       *out      = &rgba[c * pixels_per_clut];
            for (int i = 0; i < pixels_per_clut; i++) {
                int byte_idx = i / 2;
                int nibble = (i & 1) ? (src[byte_idx] >> 4) : (src[byte_idx] & 0xF);
                if (nibble == 0 && !treat_index_0_opaque) {
                    out[i] = 0u;
                } else {
                    out[i] = rgb555_to_argb8888(clut_row[nibble]);
                }
            }
        }
    } else if (tim->bpp == 16) {
        /* 16bpp: keep PSX transparency for genuinely 0x0000 pixels — these
         * textures are typically UI / cutout sprites where it's intentional. */
        for (int i = 0; i < n_pixels; i++) {
            uint16_t c = tim->pixels[i];
            rgba[i] = (c == 0) ? 0u : rgb555_to_argb8888(c);
        }
    } else {
        free(rgba);
        return;
    }

    /* Upload to the requested slot, then bind slot 0 globally so legacy
     * code still sees default texture (Leon). */
    re15_tim_slot_t *st = &s_tim_slots[slot];
    if (st->tex) {
        SDL_DestroyTexture(st->tex);
        st->tex = NULL;
    }
    st->tex = SDL_CreateTexture(s_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC,
        tex_w, tex_h);
    if (st->tex) {
        SDL_UpdateTexture(st->tex, NULL, rgba, tex_w * 4);
        SDL_SetTextureBlendMode(st->tex, SDL_BLENDMODE_BLEND);
        st->w           = tex_w;
        st->h           = tex_h;
        st->one_clut_h  = tim->height;
        st->n_cluts     = n_cluts;
        st->clut_base_y = tim->clut_y;
        st->loaded      = 1;
    }
    free(rgba);
    /* Re-bind active slot so globals stay current (no-op if same slot). */
    update_active_slot_globals();
}

/* Phase 4.5.5: enqueue one textured triangle. Flushed in re15_render_end_frame
 * via SDL_RenderGeometry. tpage/clut are ignored on PC because the SDL texture
 * already holds decoded pixels. UVs are interpreted as pixel offsets within
 * the texture page; we divide by texture dimensions to get the [0..1] range
 * SDL_Vertex.tex_coord expects. */
void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                              int x1, int y1, int u1, int v1,
                              int x2, int y2, int u2, int v2,
                              int tpage, int clut, int z,
                              uint8_t r, uint8_t g, uint8_t b)
{
    (void) tpage;
    if (!s_tim_texture || s_textri_count >= TEXTRI_QUEUE_MAX) return;
    if (s_tim_w == 0 || s_tim_h == 0) return;

    /* Phase 4.5.7.7: `z` is the sort key for back-to-front rendering.
     * Higher z = farther from camera = drawn first (painted UNDER closer
     * tris). PSX-side already passes world-Z via mesh_psx.c per-poly
     * OTZ; PC-side computes the same in main.c (average of the three
     * vertices' projected world-Z before perspective divide).
     *
     * Older sites that passed a constant (e.g. z=3 for HUD-ish overlays)
     * will all land in the same depth band and still draw in insertion
     * order — back-compat with anything that wasn't depth-aware. */
    s_textri_depth[s_textri_count] = (float)z;
    s_textri_slot[s_textri_count]  = (uint8_t)s_active_slot;

    /* Phase 4.5.7.5 #PC-2: route to the correct CLUT slab. The PSX clut
     * word's bits 6..14 = CLUT VRAM y; subtracting the TIM's base y
     * gives the CLUT index. v_offset_pixels = clut_index × one_clut_h
     * shifts UV.y into the right vertical stripe of the stacked texture. */
    int clut_y = (clut >> 6) & 0x1FF;
    int clut_idx = clut_y - s_tim_clut_base_y;
    if (clut_idx < 0 || clut_idx >= s_tim_n_cluts) clut_idx = 0;
    int v_offset = clut_idx * s_tim_one_clut_h;

    /* Phase 4.5.10-F: track bbox of all enqueued vertices. */
    if (!s_dbg_bbox_valid) {
        s_dbg_min_sx = s_dbg_max_sx = x0;
        s_dbg_min_sy = s_dbg_max_sy = y0;
        s_dbg_bbox_valid = 1;
    }
    if (x0 < s_dbg_min_sx) s_dbg_min_sx = x0;
    if (x1 < s_dbg_min_sx) s_dbg_min_sx = x1;
    if (x2 < s_dbg_min_sx) s_dbg_min_sx = x2;
    if (x0 > s_dbg_max_sx) s_dbg_max_sx = x0;
    if (x1 > s_dbg_max_sx) s_dbg_max_sx = x1;
    if (x2 > s_dbg_max_sx) s_dbg_max_sx = x2;
    if (y0 < s_dbg_min_sy) s_dbg_min_sy = y0;
    if (y1 < s_dbg_min_sy) s_dbg_min_sy = y1;
    if (y2 < s_dbg_min_sy) s_dbg_min_sy = y2;
    if (y0 > s_dbg_max_sy) s_dbg_max_sy = y0;
    if (y1 > s_dbg_max_sy) s_dbg_max_sy = y1;
    if (y2 > s_dbg_max_sy) s_dbg_max_sy = y2;

    textri_verts_t *t = &s_textri_queue[s_textri_count++];

    /* SDL_Vertex tex_coord is [0..1]. UVs from PSX are pixel offsets;
     * we divide by texture dims to map to [0..1]. */
    const float inv_w = 1.0f / (float)s_tim_w;
    const float inv_h = 1.0f / (float)s_tim_h;

    SDL_Color tint = { r, g, b, (Uint8)s_tri_alpha };

    t->v[0].position.x = (float)x0;
    t->v[0].position.y = (float)y0;
    t->v[0].color      = tint;
    t->v[0].tex_coord.x =  (float)u0                 * inv_w;
    t->v[0].tex_coord.y = ((float)v0 + (float)v_offset) * inv_h;

    t->v[1].position.x = (float)x1;
    t->v[1].position.y = (float)y1;
    t->v[1].color      = tint;
    t->v[1].tex_coord.x =  (float)u1                 * inv_w;
    t->v[1].tex_coord.y = ((float)v1 + (float)v_offset) * inv_h;

    t->v[2].position.x = (float)x2;
    t->v[2].position.y = (float)y2;
    t->v[2].color      = tint;
    t->v[2].tex_coord.x =  (float)u2                 * inv_w;
    t->v[2].tex_coord.y = ((float)v2 + (float)v_offset) * inv_h;
}

/* PSX GPU texture-blend modulation: final = (tex × prim) / 0x80, sat 0..0xFF.
 * SDL's modulate path uses /0xFF. To match PSX, we scale the primitive color
 * by 0xFF/0x80 ≈ 1.992 before handing to SDL — saturating at 0xFF. This means:
 *   - PSX prim 0x80 (NCCT neutral)        → SDL vert 0xFF → SDL out = tex × 1.0  ✓
 *   - PSX prim 0xFF (NCCT max headroom)   → SDL vert 0xFF → SDL out = tex × 1.0
 *     (PSX would give tex × 1.992; SDL caps at 1.0 — minor loss of overbright)
 *   - PSX prim 0x40 (half-lit)            → SDL vert 0x7F → SDL out = tex × 0.5  ✓
 * Source: nocash psx-spx GPU §"Texture Color" — primitive_color/128 modulation. */
static inline uint8_t psx_prim_to_sdl_vert(uint8_t v) {
    int s = ((int)v * 0xFF + 0x40) / 0x80;   /* round-to-nearest */
    return s > 0xFF ? 0xFF : (uint8_t)s;
}

/* BF-round: per-vertex tinted variant (NCCT Gouraud shading). Same body as
 * re15_render_textured_tri except each vertex carries its own SDL_Color.
 * NCCT output is in PSX primitive_color space (neutral = 0x80); we scale to
 * SDL vertex_color space (neutral = 0xFF) before submitting. */
void re15_render_textured_tri_lit(int x0, int y0, int u0, int v0,
                                  int x1, int y1, int u1, int v1,
                                  int x2, int y2, int u2, int v2,
                                  int tpage, int clut, int z,
                                  uint8_t r0, uint8_t g0, uint8_t b0,
                                  uint8_t r1, uint8_t g1, uint8_t b1,
                                  uint8_t r2, uint8_t g2, uint8_t b2)
{
    (void) tpage;
    if (!s_tim_texture || s_textri_count >= TEXTRI_QUEUE_MAX) return;
    if (s_tim_w == 0 || s_tim_h == 0) return;

    s_textri_depth[s_textri_count] = (float)z;
    s_textri_slot[s_textri_count]  = (uint8_t)s_active_slot;

    int clut_y = (clut >> 6) & 0x1FF;
    int clut_idx = clut_y - s_tim_clut_base_y;
    if (clut_idx < 0 || clut_idx >= s_tim_n_cluts) clut_idx = 0;
    int v_offset = clut_idx * s_tim_one_clut_h;

    if (!s_dbg_bbox_valid) {
        s_dbg_min_sx = s_dbg_max_sx = x0;
        s_dbg_min_sy = s_dbg_max_sy = y0;
        s_dbg_bbox_valid = 1;
    }
    if (x0 < s_dbg_min_sx) s_dbg_min_sx = x0;
    if (x1 < s_dbg_min_sx) s_dbg_min_sx = x1;
    if (x2 < s_dbg_min_sx) s_dbg_min_sx = x2;
    if (x0 > s_dbg_max_sx) s_dbg_max_sx = x0;
    if (x1 > s_dbg_max_sx) s_dbg_max_sx = x1;
    if (x2 > s_dbg_max_sx) s_dbg_max_sx = x2;
    if (y0 < s_dbg_min_sy) s_dbg_min_sy = y0;
    if (y1 < s_dbg_min_sy) s_dbg_min_sy = y1;
    if (y2 < s_dbg_min_sy) s_dbg_min_sy = y2;
    if (y0 > s_dbg_max_sy) s_dbg_max_sy = y0;
    if (y1 > s_dbg_max_sy) s_dbg_max_sy = y1;
    if (y2 > s_dbg_max_sy) s_dbg_max_sy = y2;

    textri_verts_t *t = &s_textri_queue[s_textri_count++];

    const float inv_w = 1.0f / (float)s_tim_w;
    const float inv_h = 1.0f / (float)s_tim_h;

    t->v[0].position.x = (float)x0;
    t->v[0].position.y = (float)y0;
    t->v[0].color      = (SDL_Color){psx_prim_to_sdl_vert(r0),
                                     psx_prim_to_sdl_vert(g0),
                                     psx_prim_to_sdl_vert(b0), 0xFF};
    t->v[0].tex_coord.x =  (float)u0                 * inv_w;
    t->v[0].tex_coord.y = ((float)v0 + (float)v_offset) * inv_h;

    t->v[1].position.x = (float)x1;
    t->v[1].position.y = (float)y1;
    t->v[1].color      = (SDL_Color){psx_prim_to_sdl_vert(r1),
                                     psx_prim_to_sdl_vert(g1),
                                     psx_prim_to_sdl_vert(b1), 0xFF};
    t->v[1].tex_coord.x =  (float)u1                 * inv_w;
    t->v[1].tex_coord.y = ((float)v1 + (float)v_offset) * inv_h;

    t->v[2].position.x = (float)x2;
    t->v[2].position.y = (float)y2;
    t->v[2].color      = (SDL_Color){psx_prim_to_sdl_vert(r2),
                                     psx_prim_to_sdl_vert(g2),
                                     psx_prim_to_sdl_vert(b2), 0xFF};
    t->v[2].tex_coord.x =  (float)u2                 * inv_w;
    t->v[2].tex_coord.y = ((float)v2 + (float)v_offset) * inv_h;
}

void re15_render_tile(int x, int y, int w, int h, int z,
                      uint8_t r, uint8_t g, uint8_t b)
{
    (void) z;
    uint32_t color = (uint32_t)r << 24 | (uint32_t)g << 16 | (uint32_t)b << 8 | 0xFFu;
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            put_pixel(x + dx, y + dy, color);
        }
    }
}

/* Phase 4.6.0: full-screen vertical gradient written directly into the
 * software framebuffer. Same per-row interpolation as the PSX TILE strips
 * but at per-pixel resolution since CPU can do it cheaply. */
void re15_render_background_gradient(uint8_t r_top,    uint8_t g_top,    uint8_t b_top,
                                     uint8_t r_bottom, uint8_t g_bottom, uint8_t b_bottom)
{
    for (int y = 0; y < SCREEN_YRES; y++) {
        int t = y * 255 / (SCREEN_YRES - 1);    /* 0..255 across the screen */
        uint8_t r = (uint8_t)((r_top * (255 - t) + r_bottom * t) / 255);
        uint8_t g = (uint8_t)((g_top * (255 - t) + g_bottom * t) / 255);
        uint8_t b = (uint8_t)((b_top * (255 - t) + b_bottom * t) / 255);
        uint32_t color = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFFu;
        for (int x = 0; x < SCREEN_XRES; x++) {
            s_framebuffer[y * SCREEN_XRES + x] = color;
        }
    }
}

void re15_render_line(int x0, int y0, int x1, int y1, int z,
                      uint8_t r, uint8_t g, uint8_t b)
{
    (void) z;
    uint32_t color = (uint32_t)r << 24 | (uint32_t)g << 16 | (uint32_t)b << 8 | 0xFFu;

    /* Bresenham's line algorithm */
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int x = x0, y = y0;
    for (;;) {
        put_pixel(x, y, color);
        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
}

/*=========================================================================
 * RE1.5 in-game subtitle FONT (TEX.TIM glyph page) — RE 2026-05-31.
 *
 * The real message font is the sub-region of TEX.TIM at VRAM (256,0), 4bpp,
 * 16-col grid of 16×16 cells starting at page y=32: for a .msg glyph code,
 * cell = (col=code&0x0F, row=code>>4) at atlas pixel (col*16, row*16+32)
 * (PSX glyph emitter FUN_80028ec4 / subtitle renderer FUN_80028868). NOT
 * TMOJI.TIM (that is the title-screen bitmap).
 *
 * COLOUR is data-driven, NOT a fixed cyan: the .msg `0x05 N` code selects a
 * TEX CLUT *row* (the SPRT prim colour is the neutral 0x808080). Verified
 * arithmetic (FUN_80028868): clut_row = (N&3)*2 + (N>>2); N=0 default = white,
 * N=1 (Leon) = green, N=2 (Pilot) = orange, N=7 (Elliot) = blue. We decode the
 * glyph page once to 4-bit PALETTE INDICES and build one 16-colour palette per
 * attribute from the matching CLUT row, then render the raw .msg bytes directly
 * so the colour can change mid-line (speaker name vs dialogue). Box origin for
 * the 0x300 (dynamic) Message_on = (X=0x22=34, Y=0xB4=180). Per-glyph advance
 * derived from the glyph's own non-transparent extent (the PSX proportional
 * table DAT_800c4416 is runtime-only). Framebuffer is native 320×240.
 *=========================================================================*/
#define MSGFONT_W 256
#define MSGFONT_H 256
static uint8_t  s_msgfont_idx[MSGFONT_W * MSGFONT_H]; /* 4-bit palette index (0 = transparent) */
static uint32_t s_msgfont_pal[8][16];                 /* ARGB palette per 0x05 attribute 0..7 */
static uint8_t  s_msgfont_w[256];                     /* per-glyph-code advance (px, 320-space) */
static int      s_msgfont_ready = 0, s_msgfont_ok = 0;

/* 0x05 attribute N → TEX CLUT block row (FUN_80028868 colour arithmetic). */
static int re15_msgfont_attr_row(int n) { return ((n & 3) * 2) + ((n >> 2) & 1); }

static void re15_msgfont_ensure(void)
{
    if (s_msgfont_ready) return;
    s_msgfont_ready = 1;   /* attempt once; on failure callers fall back to 6x8 */

    extern uint8_t *re15_asset_read_file(const char *, int *);
    /* Asset-Pfad-Konsolidierung (2026-07-02): TEX.TIM kommt aus der EINEN Wurzel
     * shared_assets/PSX (CD-Baum, unten CD-root-first). Die alten re15_reborn-Fallbacks
     * sind entfernt; die extracted/PSX-Relativen bleiben nur als cwd-Fallback. */
    static const char *cand[3] = {
        "../../../../extracted/PSX/DATA/TEX.TIM",
        "../../../extracted/PSX/DATA/TEX.TIM",
        "extracted/PSX/DATA/TEX.TIM",
    };
    uint8_t *buf = NULL; int sz = 0;
    /* Original-CD-Baum zuerst: TEX.TIM liegt dort unter DATA/ (re15_port
     * shared_assets/PSX). RE15_CD_ROOT (env) übersteuert den Compile-Default. */
    {
        const char *cdroot = getenv("RE15_CD_ROOT");
#ifdef RE15_CD_ROOT_DEFAULT
        if (!cdroot || !cdroot[0]) cdroot = RE15_CD_ROOT_DEFAULT;
#endif
        if (cdroot && cdroot[0]) {
            char p[256];
            snprintf(p, sizeof p, "%s/DATA/TEX.TIM", cdroot);
            buf = re15_asset_read_file(p, &sz);
        }
    }
    for (int i = 0; i < 3 && !buf; i++) buf = re15_asset_read_file(cand[i], &sz);
    if (!buf) return;

    re15_tim_t tim;
    if (re15_tim_parse(buf, sz, &tim) != 0 || tim.bpp != 4 || !tim.has_clut) { free(buf); return; }

    /* Font page = TEX image x∈[256,512). 4bpp: row pitch = tim.width/2 bytes.
     * Store the raw 4-bit index (0 = transparent); colour comes from the CLUT. */
    const uint8_t *src = (const uint8_t *)tim.pixels;
    int pitch = tim.width / 2;
    for (int ay = 0; ay < MSGFONT_H && ay < tim.height; ay++) {
        for (int ax = 0; ax < MSGFONT_W; ax++) {
            int vx = 256 + ax;
            if (vx >= tim.width) break;
            int byte = src[ay * pitch + (vx >> 1)];
            s_msgfont_idx[ay * MSGFONT_W + ax] = (uint8_t)((vx & 1) ? (byte >> 4) : (byte & 0xF));
        }
    }

    /* Build one palette per 0x05 attribute from the matching CLUT row. TEX's
     * CLUT block is 32 entries wide (2 sub-CLUTs of 16); the renderer uses
     * sub-CLUT A (entries 0..15) of the selected row. index 0 = transparent. */
    int clut_stride = (tim.clut_entries >= 32) ? 32 : 16;
    for (int n = 0; n < 8; n++) {
        int row = re15_msgfont_attr_row(n);
        for (int e = 0; e < 16; e++) {
            int ci = row * clut_stride + e;
            if (e == 0 || ci >= tim.clut_entries) { s_msgfont_pal[n][e] = 0u; continue; }
            uint16_t c = tim.clut[ci];
            uint32_t r = ((c >> 0) & 0x1F) << 3, g = ((c >> 5) & 0x1F) << 3, b = ((c >> 10) & 0x1F) << 3;
            /* The 2D framebuffer texture is SDL_PIXELFORMAT_RGBA8888 (R in the high
             * byte, A in the low byte) — NOT ARGB. Encode to match, opaque alpha. */
            s_msgfont_pal[n][e] = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
        }
    }
    free(buf);

    /* Per-glyph advance width = the CANONICAL PSX table, not a measured proxy.
     * The dialog/HUD renderers (FUN_80028868:55, FUN_80028ec4:110) advance the
     * pen by `(&DAT_800c4416)[code]`. DAT_800c4416 is BSS — it is the DEBUG.BIN
     * image loaded to RAM 0x800c0000, so width[code] = DEBUG.BIN[0x4416 + code]
     * (proven byte-exact: DEBUG.BIN[0x44b6]==0xFFFF == FUN_80013160's SJIS-latch
     * init value). avg 8.2px vs our old maxc+2 ~10px → fixes the ~17% over-wide
     * text. We load the real bytes here; maxc+2 only as a last-resort fallback. */
    {
        static const char *dcand[3] = {
            "../../../../extracted/PSX/BIN/DEBUG.BIN",
            "../../../extracted/PSX/BIN/DEBUG.BIN",
            "extracted/PSX/BIN/DEBUG.BIN",
        };
        uint8_t *dbg = NULL; int dsz = 0;
        /* Original-CD-Baum zuerst: DEBUG.BIN liegt dort unter BIN/ (shared_assets/PSX). */
        {
            const char *cdroot = getenv("RE15_CD_ROOT");
#ifdef RE15_CD_ROOT_DEFAULT
            if (!cdroot || !cdroot[0]) cdroot = RE15_CD_ROOT_DEFAULT;
#endif
            if (cdroot && cdroot[0]) {
                char p[256];
                snprintf(p, sizeof p, "%s/BIN/DEBUG.BIN", cdroot);
                dbg = re15_asset_read_file(p, &dsz);
            }
        }
        for (int i = 0; i < 3 && !dbg; i++) dbg = re15_asset_read_file(dcand[i], &dsz);
        if (dbg && dsz >= 0x4416 + 256) {
            for (int code = 0; code < 256; code++)
                s_msgfont_w[code] = dbg[0x4416 + code];   /* RAM 0x800c4416[code] */
            free(dbg);
        } else {
            if (dbg) free(dbg);
            for (int code = 0; code < 256; code++) {      /* fallback only */
                int col = (code & 0x0F) * 16, row = (code >> 4) * 16 + 32, maxc = -1;
                for (int gy = 0; gy < 16; gy++) {
                    int py = row + gy; if (py >= MSGFONT_H) break;
                    for (int gx = 0; gx < 16; gx++)
                        if (s_msgfont_idx[py * MSGFONT_W + (col + gx)]) { if (gx > maxc) maxc = gx; }
                }
                s_msgfont_w[code] = (uint8_t)(maxc < 0 ? 6 : maxc + 2);
            }
        }
    }
    s_msgfont_ok = 1;

    if (getenv("RE15_FONT_DUMP")) {
        FILE *lf = fopen("font_dump.txt", "w");
        if (lf) {
            fprintf(lf, "clut_entries=%d stride=%d width=%d height=%d\n",
                    tim.clut_entries, clut_stride, tim.width, tim.height);
            for (int n = 0; n < 8; n++) {
                fprintf(lf, "attr %d (row %d):", n, re15_msgfont_attr_row(n));
                for (int e = 1; e <= 8; e++) {
                    uint32_t c = s_msgfont_pal[n][e];
                    fprintf(lf, " (%d,%d,%d)", (c>>16)&0xFF, (c>>8)&0xFF, c&0xFF);
                }
                fprintf(lf, "\n");
            }
            /* index histogram of glyph 'L' (0x28) cell */
            int col=(0x28&0xF)*16, row=(0x28>>4)*16+32, hist[16]={0};
            for (int gy=0;gy<16;gy++) for(int gx=0;gx<16;gx++)
                hist[s_msgfont_idx[(row+gy)*MSGFONT_W+(col+gx)]&0xF]++;
            fprintf(lf, "glyph 'L' idx hist:");
            for (int e=0;e<16;e++) if(hist[e]) fprintf(lf," %d:%d",e,hist[e]);
            fprintf(lf, "\n"); fclose(lf);
        }
    }
}

/* Blit glyph code at pen in palette `attr`. */
static void re15_msgfont_glyph(int penx, int peny, int code, int attr)
{
    int col = (code & 0x0F) * 16, row = (code >> 4) * 16 + 32;
    const uint32_t *pal = s_msgfont_pal[attr & 7];
    for (int gy = 0; gy < 16; gy++) {
        int py = row + gy; if (py >= MSGFONT_H) break;
        for (int gx = 0; gx < 16; gx++) {
            uint8_t idx = s_msgfont_idx[py * MSGFONT_W + (col + gx)];
            if (!idx) continue;
            uint32_t px = pal[idx];           /* RGBA8888 (matches the framebuffer texture) */
            if (px) {
                /* Write the subtitle into the OVERLAY layer (blitted after the
                 * 3D tris), not the framebuffer. Force alpha=0xFF (opaque) so
                 * the alpha-blended overlay shows it; cleared pixels (alpha 0)
                 * stay transparent. */
                int ox = penx + gx, oy = peny + gy;
                if ((unsigned)ox < (unsigned)SCREEN_XRES &&
                    (unsigned)oy < (unsigned)SCREEN_YRES) {
                    s_text_overlay[oy * SCREEN_XRES + ox] = (px & 0xFFFFFF00u) | 0xFFu;
                    s_text_overlay_used = 1;
                }
            }
        }
    }
}

/* Render a raw .msg body with the real TEX.TIM font, honouring the embedded
 * control codes (0x05 colour, 0x08 newline) — the canonical PSX subtitle path
 * (FUN_80028868). Returns 1 if drawn, 0 to fall back to the 6x8 debug font. */
/* Per-glyph callbacks for the shared re15_msg_layout walk (PC software-blit path). */
static void pc_msg_glyph_cb(int penx, int peny, unsigned char b, int attr, void *ctx)
{
    (void) ctx;
    re15_msgfont_glyph(penx, peny, b, attr);
}
static int pc_msg_width_cb(unsigned char b, void *ctx) { (void) ctx; return s_msgfont_w[b]; }

int re15_render_pc_msg_text(int x, int y, const unsigned char *raw, int len)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok || !raw) return 0;
    re15_msg_layout(x, y, raw, len, pc_msg_glyph_cb, pc_msg_width_cb, NULL);
    return 1;
}

/* OPTIONS config text: draw literal game-font glyph CODES (not .msg control-coded bytes) at (x,y) with a
 * fixed colour attribute. The config screen (FUN_8002ffb8 / FUN_80028ec4) draws each label glyph directly
 * with a per-glyph CLUT, so button symbols like ✕ (code 0x08) or □ (0x09) must NOT be re-interpreted as
 * .msg newline/control codes. attr = the TEX CLUT colour row: 0 = normal (0x7810), 3 = dim (0x7990,
 * the byte-true "Not set" dim). Returns the pen x after the last glyph. */
int re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok || !codes) return x;
    int penx = x;
    for (int i = 0; i < len; i++) {
        unsigned char c = codes[i];
        if (c != 0x00) re15_msgfont_glyph(penx, y, c, attr);   /* 0x00 = blank space */
        int w = s_msgfont_w[c];
        penx += (w > 0) ? w : 6;
    }
    s_text_overlay_used = 1;
    return penx;
}

/* YES/NO selection cursor — the REAL filled right-pointing triangle the original draws
 * ("Will you push it?  ▶ Yes   No", verified vs screenshot). It is a 16×16 glyph in the
 * SAME TEX.TIM font we already load (font code 0x02 = atlas cell (2,0), TEX.TIM (288,32),
 * a clean ▶). We render it via the real glyph blitter into the text-overlay layer — NOT a
 * hand-drawn shape — so it matches the option text 1:1. (.msg layout reserves 0x02 as a
 * control code, so we call the glyph blitter directly.) Caller gates it on the blink. */
void re15_render_pc_cursor(int x, int y)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok) return;
    re15_msgfont_glyph(x, y, 0x02, 0);
    s_text_overlay_used = 1;
}

/* ASCII text into the TOP text-overlay layer (Layer 5, above the 3D scene + the modal quad) — the
 * 6×8 debug font but composited ON TOP, for the item-get modal's "WILL YOU TAKE THE X." prompt
 * (re15_debug_text goes to the framebuffer = under the meshes, which the frozen scene would cover).
 * `_n` draws at most `n` characters (the typewriter budget) and returns how many it drew (so a caller
 * can carry the remaining budget to the next line). n < 0 draws nothing. */
int re15_render_pc_text_overlay_n(int x, int y, const char *text, int n)
{
    if (!text || n <= 0) return 0;
    int cx = x, cy = y, drawn = 0;
    while (*text && drawn < n) {
        unsigned char c = (unsigned char) *text++;
        drawn++;
        if (c == '\n') { cx = x; cy += 9; continue; }
        if (c < 0x20 || c >= 0x80) c = '?';
        const uint8_t *glyph = s_font6x8[c - 0x20];
        for (int row = 0; row < 8; row++) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 5; col++) {
                int ox = cx + col, oy = cy + row;
                if ((bits & (1 << (4 - col))) && ox >= 0 && ox < SCREEN_XRES && oy >= 0 && oy < SCREEN_YRES)
                    s_text_overlay[oy * SCREEN_XRES + ox] = 0xFFFFFFFFu;   /* white opaque (RGBA) */
            }
        }
        cx += 6;
        if (cx + 6 > SCREEN_XRES) { cx = x; cy += 9; }
    }
    s_text_overlay_used = 1;
    return drawn;
}

void re15_render_pc_text_overlay(int x, int y, const char *text)
{
    re15_render_pc_text_overlay_n(x, y, text, text ? (int)strlen(text) : 0);
}

/* Item-get prompt in the GAME TEX.TIM font — byte-true glyph replay. The engine walk
 * (re15_item_prompt_walk) yields the prompt's own glyph bytes (from the BSS scripts + name blob) in
 * stream order; we draw the first `reveal` of them (typewriter) via the real message-font blitter at
 * (x,y). The Yes/No + cursor are drawn by the caller once the text is fully revealed. */
typedef struct { int x, penx, peny; } pc_prompt_pen_t;
static void pc_prompt_glyph_cb(void *v, unsigned char code, int attr, int newline)
{
    pc_prompt_pen_t *p = (pc_prompt_pen_t *)v;
    if (newline) { p->penx = p->x; p->peny += 13; return; }   /* drop a line */
    if (code != 0x00) re15_msgfont_glyph(p->penx, p->peny, code, attr);   /* 0x00 = blank space */
    int w = s_msgfont_w[code];
    p->penx += (w > 0) ? w : 6;                               /* space / unmeasured -> fixed advance */
}
void re15_render_pc_item_prompt(int x, int y, int prompt_type, uint8_t item_id, int reveal)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok) return;
    pc_prompt_pen_t p = { x, x, y };
    re15_item_prompt_walk(prompt_type, item_id, reveal, pc_prompt_glyph_cb, &p);
}

/* FE-4: draw an ASCII string in the RE1.5 GAME font (TEX.TIM msgfont — code = char byte,
 * the same font/glyph path as the item prompt), attr = colour palette 0..7. Writes to the
 * text overlay. Returns the pixel advance (for centering). Space (0x20) advances only. */
/* ASCII -> RE1.5 TEX.TIM game-font atlas glyph code (decoded from the font grid, shots/texfont_grid.png).
 * The atlas is NOT a flat ASCII-0x24 shift: letters are (A-Z@0x1D, a-z@0x3D), but the digits split
 * around a gap (0-3 @0x0B..0x0E, then 4-9 @0x10..0x15) and the punctuation lives in its own cells
 * (.=0x57 ,=0x18 /=0x38 …). The old flat "-0x24" mangled digits 0-3 (off by one) and every symbol
 * (e.g. '/' -> 0x0B = a ▼ down-arrow). Returns -1 for space/unmapped (advance only). */
static int re15_msgfont_code(unsigned char c)
{
    if (c >= 'A' && c <= 'Z') return 0x1D + (c - 'A');
    if (c >= 'a' && c <= 'z') return 0x3D + (c - 'a');
    if (c >= '0' && c <= '9') return 0x0C + (c - '0');   /* digits contiguous 0x0c..0x15 (0x0b=down-arrow) */
    switch (c) {
        case '.': return 0x57; case ',': return 0x18; case '/': return 0x38;
        case '(': return 0x37; case ')': return 0x39; case '\'': return 0x3A;
        case '-': return 0x3B; case ':': return 0x16; case ';': return 0x17;
        case '!': return 0x1A; case '?': return 0x1B;
    }
    return -1;   /* space / unmapped -> advance only */
}

int re15_render_pc_game_text(int x, int y, const char *str, int attr)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok || !str) return 0;
    int penx = x;
    for (const unsigned char *pp = (const unsigned char *)str; *pp; pp++) {
        int code = re15_msgfont_code(*pp);
        if (code < 0) { penx += 8; continue; }               /* space / unmapped -> advance only */
        re15_msgfont_glyph(penx, y, code, attr);
        int w = s_msgfont_w[code];
        penx += (w > 0) ? w : 12;
    }
    return penx - x;
}

/* FE-4: draw a run of RAW RE1.5 sysmes atlas codes (byte 0x00 = space advance, else the glyph
 * code directly — the byte-true encoding used by the save-slot title idx 0x18/0x19, whose counter
 * delimiter (0x38) and apostrophe (0x3A) are not reachable through the ASCII path). Returns width. */
int re15_render_pc_game_codes(int x, int y, const uint8_t *codes, int n, int attr)
{
    re15_msgfont_ensure();
    if (!s_msgfont_ok || !codes) return 0;
    int penx = x;
    for (int k = 0; k < n; k++) {
        int code = codes[k];
        if (code == 0x05 && k + 1 < n) { attr = codes[++k] & 7; continue; }  /* colour op: next byte & 7 = palette (byte-true, cf. msg_common.c:537) */
        if (code == 0x00) { penx += 8; continue; }           /* space -> advance only */
        re15_msgfont_glyph(penx, y, code, attr);
        int w = s_msgfont_w[code];
        penx += (w > 0) ? w : 12;
    }
    return penx - x;
}

/* Width of an ASCII string in the game font (for centering) without drawing. */
int re15_render_pc_game_text_width(const char *str)
{
    re15_msgfont_ensure();
    if (!str) return 0;
    int w = 0;
    for (const unsigned char *pp = (const unsigned char *)str; *pp; pp++) {
        int code = re15_msgfont_code(*pp);
        if (code < 0) { w += 8; continue; }
        w += (s_msgfont_ok && s_msgfont_w[code] > 0) ? s_msgfont_w[code] : 12;
    }
    return w;
}

/* Dialog page-break indicator: a small DOWN-pointing triangle (byte-true FUN_80028134
 * state 2 = "press action for the next page"), drawn into the alpha text overlay. */
void re15_render_pc_down_arrow(int x, int y)
{
    for (int row = 0; row < 7; row++) {
        int half = (6 - row) * 5 / 6;          /* wide at the top, point at the bottom */
        for (int col = -half; col <= half; col++) {
            int ox = x + col, oy = y + row;
            if (ox >= 0 && ox < SCREEN_XRES && oy >= 0 && oy < SCREEN_YRES)
                s_text_overlay[oy * SCREEN_XRES + ox] = 0xC8C8C8FFu;  /* light grey, opaque */
        }
    }
    s_text_overlay_used = 1;
}

void re15_debug_text(int x, int y, int z, const char *text)
{
    (void) z;
    int cx = x;
    int cy = y;
    while (*text) {
        unsigned char c = (unsigned char) *text++;
        if (c == '\n') {
            cx = x;
            cy += 9;
            continue;
        }
        if (c < 0x20 || c >= 0x80) c = '?';

        const uint8_t *glyph = s_font6x8[c - 0x20];
        for (int row = 0; row < 8; row++) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 5; col++) {
                if (bits & (1 << (4 - col))) {
                    put_pixel(cx + col, cy + row, 0x00FFFFFFu);
                }
            }
        }
        cx += 6;
        if (cx + 6 > SCREEN_XRES) {
            cx = x;
            cy += 9;
        }
    }
}
