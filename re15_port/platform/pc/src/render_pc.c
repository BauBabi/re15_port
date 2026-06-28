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

/* BJ-round 2026-05-29: CINEMATIC LETTERBOX bars.
 * RE2/RE1.5 cutscenes (the intro etc.) draw black bars at the top & bottom of
 * the 320x240 frame — measured from the PSX ablauf at ~17px each. The bars hide
 * the topmost building/window row that's visible during normal gameplay (see
 * non-cutscene.png). This is NOT overscan/zoom: the scene underneath is the full
 * frame; the bars simply overlay it. The main loop sets the bar height each
 * frame (= cinematic active ? ~17 : 0) via re15_render_pc_set_letterbox(). */
static int           s_letterbox_h = 0;
static uint8_t       s_fade_alpha = 0;   /* BN-round: cinematic fade-in overlay (255=black .. 0=none) */

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
#define TEXTRI_QUEUE_MAX 2048   /* enough for a mesh of ~700 quads */

/* TIM slot pool — allows multiple characters/props to have their own
 * textures. Slot 0 = player default (Leon); other slots for NPCs/props. */
#define RE15_TIM_SLOT_MAX 24   /* 0=Leon 1=Elliot 2=heli-legacy 3=pilot-legacy
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

#define CLEAR_RGB  0x00081830u   /* dark blue, matches PSX target (8,16,48) */

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
int re15_render_pc_dbg_tim_loaded(void)      { return s_tim_texture != NULL ? 1 : 0; }
int re15_render_pc_dbg_min_sx(void)          { return s_dbg_last_min_sx; }
int re15_render_pc_dbg_max_sx(void)          { return s_dbg_last_max_sx; }
int re15_render_pc_dbg_min_sy(void)          { return s_dbg_last_min_sy; }
int re15_render_pc_dbg_max_sy(void)          { return s_dbg_last_max_sy; }

void re15_render_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    s_window = SDL_CreateWindow(
        "RE1.5 Rebuilt — PC",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_XRES * WINDOW_SCALE, SCREEN_YRES * WINDOW_SCALE,
        SDL_WINDOW_SHOWN);
    if (!s_window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(1);
    }

    s_renderer = SDL_CreateRenderer(s_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

/* Queue one shadow quad (4 projected floor corners in the FUN_8001af5c order:
 * 0=(-X,+Z)->uv(0,0), 1=(-X,-Z)->uv(1,0), 2=(+X,+Z)->uv(0,1), 3=(+X,-Z)->uv(1,1)).
 * Two tris cover the quad. Subtractive blend is set on the texture. */
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
        }
    }

    /* Clear framebuffer to background color (matches PSX setRGB0 / isbg=1) */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) {
        s_framebuffer[i] = CLEAR_RGB;
    }
    /* Clear the subtitle overlay (fully transparent) each frame. */
    for (int i = 0; i < SCREEN_XRES * SCREEN_YRES; i++) s_text_overlay[i] = 0;
    s_text_overlay_used = 0;
}

void re15_render_end_frame(void)
{
    /* Step 1: blit the software framebuffer (2D primitives) onto the renderer. */
    SDL_UpdateTexture(s_texture, NULL, s_framebuffer, SCREEN_XRES * sizeof(uint32_t));
    SDL_RenderClear(s_renderer);
    SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);

    /* Step 1.5: RE1.5 character shadows — subtractive blob quads on the floor.
     * Drawn after the BG (darken the helipad) and before the character tris
     * (so the actor occludes its own feet). Matches PSX FUN_8001b064. */
    if (s_shadow_tex && s_shadow_quad_count > 0) {
        for (int i = 0; i < s_shadow_quad_count; i++)
            SDL_RenderGeometry(s_renderer, s_shadow_tex,
                               s_shadow_quads[i].v, 6, NULL, 0);
    }
    s_shadow_quad_count = 0;

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

    /* Subtitle overlay ON TOP of the 3D + foreground (drawn after the character
     * tris + sprite.pri overdraw so the actors' feet no longer cover the text),
     * but BEFORE the letterbox bars (so a bar can still clip text that strays
     * into the bar region). This is the fix for "Leon's feet cover the text". */
    if (s_text_overlay_used && s_text_overlay_tex) {
        SDL_UpdateTexture(s_text_overlay_tex, NULL, s_text_overlay,
                          SCREEN_XRES * sizeof(uint32_t));
        SDL_RenderCopy(s_renderer, s_text_overlay_tex, NULL, NULL);
    }

    s_dbg_last_textri_count = s_textri_count;
    s_dbg_last_min_sx       = s_dbg_min_sx;
    s_dbg_last_max_sx       = s_dbg_max_sx;
    s_dbg_last_min_sy       = s_dbg_min_sy;
    s_dbg_last_max_sy       = s_dbg_max_sy;
    s_textri_count = 0;  /* reset queue for next frame */

    /* BJ-round: cinematic letterbox — draw black bars over the top & bottom of
     * the full composited frame during cutscenes (matches the PSX intro, which
     * has ~17px bars hiding the topmost window row). Drawn LAST so it occludes
     * BG, 3D and HUD alike. s_letterbox_h=0 during gameplay (no bars). */
    if (s_letterbox_h > 0) {
        SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
        SDL_Rect top = { 0, 0, SCREEN_XRES, s_letterbox_h };
        SDL_Rect bot = { 0, SCREEN_YRES - s_letterbox_h, SCREEN_XRES, s_letterbox_h };
        SDL_RenderFillRect(s_renderer, &top);
        SDL_RenderFillRect(s_renderer, &bot);
    }

    /* BN-round 2026-05-29: cinematic FADE-IN overlay (PSX engine-driven fade,
     * RE2 FUN_8002bda8; measured ablauf ramp 0→full over ~6 engine frames at
     * room entry). Full-screen black with decreasing alpha, drawn LAST (over
     * everything incl. letterbox), so the helipad fades in from black like PSX
     * (and covers our 1-frame stale-BG on the Cut_chg(0) frame). */
    if (s_fade_alpha > 0) {
        SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, s_fade_alpha);
        SDL_Rect full = { 0, 0, SCREEN_XRES, SCREEN_YRES };
        SDL_RenderFillRect(s_renderer, &full);
    }

    SDL_RenderPresent(s_renderer);
}

/* BN-round 2026-05-29: cinematic fade-in overlay alpha (0=none, 255=black).
 * Main loop ramps it 255→0 over the room-entry fade window. */
void re15_render_pc_set_fade(int a)
{
    if (a < 0) a = 0;
    if (a > 255) a = 255;
    s_fade_alpha = (uint8_t)a;
}

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

    /* Heuristic: for indexed TIMs treat index 0 as OPAQUE (CLUT[0] colour)
     * rather than transparent. RE1.5 MD1 textures need this to look right —
     * fully-black trim pixels (door handle, weapon details) are stored as
     * palette index 0 with CLUT[0] = 0x0000 (black). Treating them as
     * transparent produces the "triangle-shaped holes" the user reported. */
    const int treat_index_0_opaque = 1;

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

    SDL_Color tint = { r, g, b, 0xFF };

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
    static const char *cand[5] = {
        "../../../../extracted/PSX/DATA/TEX.TIM",
        "../../../extracted/PSX/DATA/TEX.TIM",
        "extracted/PSX/DATA/TEX.TIM",
        "../../re15_reborn/assets/TEX.TIM",
        "psx_dev/re15_reborn/assets/TEX.TIM",
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
    for (int i = 0; i < 5 && !buf; i++) buf = re15_asset_read_file(cand[i], &sz);
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
        static const char *dcand[5] = {
            "../../../../extracted/PSX/BIN/DEBUG.BIN",
            "../../../extracted/PSX/BIN/DEBUG.BIN",
            "extracted/PSX/BIN/DEBUG.BIN",
            "../../re15_reborn/assets/DEBUG.BIN",
            "psx_dev/re15_reborn/assets/DEBUG.BIN",
        };
        uint8_t *dbg = NULL; int dsz = 0;
        /* Original-CD-Baum zuerst: DEBUG.BIN liegt dort unter BIN/. */
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
        for (int i = 0; i < 5 && !dbg; i++) dbg = re15_asset_read_file(dcand[i], &dsz);
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
