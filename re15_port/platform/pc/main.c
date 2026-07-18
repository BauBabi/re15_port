/*
 * RE1.5 Rebuilt — PC port main (Phase 4.2..4.5.4, 2026-05-18).
 *
 * Cross-platform: builds on Windows (MSVC, gcc-mingw) and Linux (gcc, clang).
 * Renders into a 320x240 SDL2 window scaled 4x (1280x960). VSync drives the
 * main loop at 60 FPS. Same splash + demos as the PSX target.
 *
 * Portability notes:
 *   - All output uses snprintf (no MSVC-only sprintf_s / no Linux-only asprintf)
 *   - SDL2 is the only platform-API dependency, statically linked via FetchContent
 *   - File I/O via plain fopen/fread (POSIX + Windows CRT)
 *   - No <windows.h>, no <unistd.h>, no Sleep()/usleep() — SDL handles timing
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <SDL_timer.h>    /* SDL_GetTicks/SDL_Delay — Frame-Timing (ohne main-Umleitung) */
#include "re15_engine.h"
#include "re15_tim.h"
#include "re15_scd.h"
#include "re15_fade.h"      /* fade channels + the letterbox counter (FUN_80021a0c) */
#include "re15_md1.h"

re15_engine_state_t g_engine;

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);
extern void re15_render_pc_upload_tim(const re15_tim_t *tim);

/* M-round (2026-05-25): round-half-away-from-zero float→int helper.
 * Plain `(int)f` truncates toward zero, which asymmetrically biases
 * negative screen coords up and positive down — a major contributor to
 * the user-reported "wiederkehrende Pixel Verschiebungen". The PSX GPU
 * receives already-integer GTE output, so the PC port must round the
 * float-projected coords the same way to match pixel placement. */
static inline int RNDI(float f) {
    return (int)(f >= 0.0f ? f + 0.5f : f - 0.5f);
}

#include "re15_bg.h"
#include "re15_audio.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_math.h"      /* re15_gte_divide — byte-true GTE RTPS perspective divide */
#include "re15_light.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_to_re2.h"
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_pri.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_game_step.h"   /* SHARED per-frame interpreter step (PSX+PC) */
#include "re15_menu.h"        /* re15_menu_* — inventory/weapon-select overlay (8.20) */
#include "re15_item_icon.h"   /* re15_item_icon_* — byte-true ITEMALL grid icons (8.22) */
#include "re15_item_modal.h"  /* re15_item_modal_* — item-get zoom/flip pickup presentation (U11) */
#include "re15_itps.h"        /* re15_itps_set_data — the per-item modal picture sheet (ITPS.ITP, U11) */
#include "re15_item_use.h"    /* re15_item_use_* — the inventory heal-USE flow ("Will you use the X?") */
#include "re15_damage.h"      /* re15_player_equipped_weapon (ARMS CONTROL panel, 8.23) */

/* Draw a byte-true 40×30 ITEMALL icon at (x,y) over the paused inventory (pixel-wise; transparent
 * skipped). Returns 1 if drawn, 0 if the item has no captured palette (caller can text-fallback). */
static int re15_pc_draw_item_icon(int x, int y, uint8_t id)
{
    if (!re15_item_icon_available(id)) return 0;
    for (int iv = 0; iv < 30; iv++)
        for (int iu = 0; iu < 40; iu++) {
            uint8_t pr, pg, pb;
            if (re15_item_icon_pixel(id, iu, iv, &pr, &pg, &pb))
                re15_render_tile(x + iu, y + iv, 1, 1, 0, pr, pg, pb);
        }
    return 1;
}

/* FAITHFUL-LINE inventory chrome (the byte-true window-frame builder FUN_800467a8 is deferred): a
 * beveled blue panel — dark fill + a lighter top/left edge + darker bottom/right edge (the RE1.5
 * status-screen panel look). z=4 (behind the content). */
static void re15_pc_panel(int x, int y, int w, int h)
{
    re15_render_tile(x, y, w, h, 4, 32, 48, 96);              /* fill   */
    re15_render_tile(x, y, w, 1, 4, 96, 128, 200);            /* top    */
    re15_render_tile(x, y, 1, h, 4, 96, 128, 200);            /* left   */
    re15_render_tile(x, y + h - 1, w, 1, 4, 12, 20, 48);      /* bottom */
    re15_render_tile(x + w - 1, y, 1, h, 4, 12, 20, 48);      /* right  */
}

/* FAITHFUL-LINE ECG heart-rate trace (the byte-true waveform generator is deferred): a green scrolling
 * heartbeat across the CONDITION panel. `hp` picks the beat shape (Fine = steady, Danger = erratic);
 * `phase` scrolls it. Draws into a wxh box at (x,y). */
static void re15_pc_ecg(int x, int y, int w, int h, int hp, unsigned phase)
{
    int mid = y + h / 2;
    for (int c = 0; c < w; c++) {
        int t = (c + (int)phase) % 40;         /* one beat per 40 px */
        int dy = 0;
        if      (t == 8)  dy = -1;
        else if (t == 9)  dy = (hp >= 50 ? -h / 3 : -h / 2);   /* R spike (taller when injured) */
        else if (t == 10) dy = (hp >= 30 ? h / 4 : h / 2);     /* S dip */
        else if (t == 11) dy = -1;
        else if (hp < 30 && (t == 20 || t == 28)) dy = -(h / 4) + (c & 1);  /* erratic extra beats */
        re15_render_tile(x + c, mid + dy, 1, 1, 0, 64, 224, 96);
    }
}
#include "re15_room.h"        /* SHARED cross-room transition (re15_room_apply_pending) */
#include "re15_enemy.h"       /* generic enemy-model registry (re15_enemy_find/alloc/reset) */
#include "re15_enemy_ai.h"    /* re15_player_victim_state/type — Leon's grab-victim render override */
#include "re15_ems.h"         /* enemy-model archive index (load EMDs out of CDEMD*.EMS) */
#include "re15_room_list.h"   /* GENERATED room-id list for the [ / ] debug room-browser */
#include "re15_room_spawns.h" /* GENERATED per-room entry spawn (inbound-door landing spot) */
#include "re15_anim_select.h"  /* SHARED actor bank/clip selection view-model */
#include "re15_esp.h"          /* Phase ESP-C: op-0x3a effect-sprite bank + particle pool */
#include "re15_gameflow.h"     /* FE-0.2 top-level mode machine (BOOT/TITLE/INGAME/GAMEOVER) */
#include "re15_str.h"          /* FE-3 STR/MDEC opening-movie demux+decode (CAPCOM.STR) */
#include "re15_xa.h"           /* FE-3 CD-XA ADPCM opening-movie audio decode */
#include "re15_savedata.h"     /* FE-4 game-state save block */
#include "re15_memcard.h"      /* FE-4 byte-true PSX .mcr backend */
#include "re15_savepoint.h"    /* FE-4 phone save-point pending signal */

#define RE15_TIM_SLOT_EFFECT 19   /* effect-sprite TIM render slot (0..18 used by chars/props) */
#define RE15_TIM_SLOT_EFFECT_GLOBAL 20 /* GLOBAL effect bank (CORE00.ESP) sprite sheet — effect-id 0
                                        * hit/blood. Its texture is NOT in any RDT; it lives in VRAM
                                        * (tpage 0x001f -> VRAM(960,256) 4-bit, clut 0x7951). Extracted
                                        * byte-true from the live ShowVRAM ground truth into a TIM
                                        * (tools/vram_png_to_tim.py) shipped at extracted_fx/effect0_blood.tim. */
#define RE15_TIM_SLOT_FX_MUZZLE 21 /* global effect-id 2 — muzzle flash/sparks (handgun discharge
                                    * 0x02000800 @0x800337bc); ShowVRAM-extracted sheet. */
#define RE15_TIM_SLOT_FX_SMOKE  22 /* global effect-id 3 — gun smoke (0x03000c00) */
#define RE15_TIM_SLOT_FX_SHELL  23 /* global effect-id 4 — shell eject/debris (0x04000800) */
#define RE15_TIM_SLOT_WPN_MELEE 24 /* PL00W01.PLW dir[3] — the knife-class in-hand model TIM */
#define RE15_TIM_SLOT_WPN_GUN   25 /* PL00W03.PLW dir[3] — the handgun-class in-hand model TIM */

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

/* Phase ESP-C: the current room's parsed effect-sprite bank (borrows the resident RDT buf;
 * bound via re15_esp_set_room_bank so op_sce_espr_on can resolve effect ids to anim records). */
static re15_esp_t s_room_esp;
static re15_esp_t s_global_esp;        /* the GLOBAL effect bank CORE00.ESP (effect-id 0 = hit fx) */
static uint8_t   *s_global_esp_buf = NULL;

/* Parse `room_id`'s ESP section from its RDT and bind it as the active effect bank. */
static void pc_load_room_esp(const uint8_t *rdt_buf, int rdt_size, unsigned room_id)
{
    re15_esp_fx_reset();
    re15_esp_set_room_bank(NULL);
    if (!rdt_buf || rdt_size < 0x5C) return;
    #define U32LE(o) ((uint32_t)rdt_buf[o] | ((uint32_t)rdt_buf[(o)+1]<<8) | \
                      ((uint32_t)rdt_buf[(o)+2]<<16) | ((uint32_t)rdt_buf[(o)+3]<<24))
    uint32_t idh = U32LE(0x4C), pe = U32LE(0x50), tb = U32LE(0x54), te = U32LE(0x58);
    #undef U32LE
    int rc = re15_esp_parse(rdt_buf, (size_t)rdt_size, idh, pe, tb, te, &s_room_esp);
    if (rc == 0) {
        re15_esp_set_room_bank(&s_room_esp);
        fprintf(stderr, "[esp] room %04X: %d effect bank(s) parsed (e.g. id 0x%02x: %u anim/%u cells)\n",
                room_id, s_room_esp.id_count,
                s_room_esp.id_count ? s_room_esp.eff[0].effect_id : 0,
                s_room_esp.id_count ? s_room_esp.eff[0].count_a : 0,
                s_room_esp.id_count ? s_room_esp.eff[0].count_b : 0);
        /* Upload the effect TIM (eff[0].tim_off) to the dedicated effect render slot. */
        if (s_room_esp.id_count && s_room_esp.eff[0].tim_off &&
            (uint32_t)rdt_size > s_room_esp.eff[0].tim_off) {
            re15_tim_t tim;
            if (re15_tim_parse(rdt_buf + s_room_esp.eff[0].tim_off,
                               (size_t)rdt_size - s_room_esp.eff[0].tim_off, &tim) == 0) {
                re15_render_pc_upload_tim_slot(&tim, RE15_TIM_SLOT_EFFECT);
                fprintf(stderr, "[esp] effect TIM (id 0x%02x) -> slot %d: %dx%d\n",
                        s_room_esp.eff[0].effect_id, RE15_TIM_SLOT_EFFECT, tim.width, tim.height);
            }
        }
    } else {
        fprintf(stderr, "[esp] room %04X: no effect bank (rc=%d)\n", room_id, rc);
    }
}

/* Phase ESP-C draw: project each live effect particle (byte-true owner+offset world pos, same
 * camera transform as the player) and emit a billboard quad textured from the effect TIM.
 * The texture is byte-true: room fx sample the RDT effect TIM (slot 19); GLOBAL-bank fx (effect-id
 * 0 hit/blood from CORE00.ESP, whose sheet is VRAM-only) sample the byte-true extracted TIM (slot 20).
 * FAITHFUL-LINE (flagged): the exact anim-record -> coord-cell mapping + the cell's screen geometry
 * are the data-driven bit (C3_RENDER_DESIGN.md §2c, TBD via live capture / the PTR_LAB_80071d40 draw
 * dispatch); here the cell index = frame % count_b, a fixed 24px UV span at the cell's byte-true (u,v)
 * origin (the coord record's w/h = signed pivot offsets, deferred with the draw routine), and a
 * ~world-600 half-size billboard. Position, effect TIM, and cell origin are byte-true. */
FILE *pc_fx_log_handle(void)
{
    static FILE *s_fxlog = NULL; static int s_fxlog_init = 0;
    if (!s_fxlog_init) { s_fxlog_init = 1;
        const char *fl = getenv("RE15_FX_LOG");
        if (fl && *fl) s_fxlog = fopen(fl, "w"); }
    return s_fxlog;
}

static void pc_draw_effects(const re15_camera_view_t *cam, int cx, int cy)
{
    extern int  re15_render_pc_dbg_slot_loaded(int slot);
    extern void re15_render_pc_bind_tim_slot(int slot);
    const re15_esp_t *global_bank = re15_esp_global_bank();
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        if (!re15_esp_fx_visible(f)) continue;   /* byte-true flags bit1 gate (frozen/staggered = hidden) */
        /* Each particle animates from ITS OWN resolved bank: the room ESP (RDT-TIM slot 19) or the
         * GLOBAL bank CORE00.ESP, whose sheets live only in VRAM -> the byte-true extracted TIMs.
         * Global ids have per-id sheets (0 blood / 2 muzzle / 3 smoke / 4 shell) in slots 20-23. */
        const re15_esp_t *bank = f->bank;
        int slot = RE15_TIM_SLOT_EFFECT;
        if (bank && bank == global_bank) {
            switch (f->effect_id) {
                case 2:  slot = RE15_TIM_SLOT_FX_MUZZLE;     break;
                case 3:  slot = RE15_TIM_SLOT_FX_SMOKE;      break;
                case 4:  slot = RE15_TIM_SLOT_FX_SHELL;      break;
                default: slot = RE15_TIM_SLOT_EFFECT_GLOBAL; break;
            }
        }
        if (!re15_render_pc_dbg_slot_loaded(slot)) continue;   /* that bank's texture not loaded */
        re15_render_pc_bind_tim_slot(slot);
        /* SPLATTER physics offset (byte-true xlat): a physics particle draws at anchor + xlat. */
        /* byte-true integer GTE RTPS (same path as the character mesh + shadows):
         * view = (rot·world)>>12 + trans, no float. rot is int32 Q12, trans int32. */
        int32_t wx = f->x + f->xlat_x;
        int32_t wy = f->y + f->xlat_y;
        int32_t wz = f->z + f->xlat_z;
        int32_t vx = (int32_t)(((int64_t)cam->rot[0]*wx + (int64_t)cam->rot[1]*wy + (int64_t)cam->rot[2]*wz) >> 12) + cam->trans[0];
        int32_t vy = (int32_t)(((int64_t)cam->rot[3]*wx + (int64_t)cam->rot[4]*wy + (int64_t)cam->rot[5]*wz) >> 12) + cam->trans[1];
        int32_t vz = (int32_t)(((int64_t)cam->rot[6]*wx + (int64_t)cam->rot[7]*wy + (int64_t)cam->rot[8]*wz) >> 12) + cam->trans[2];
        /* RE15_FX_LOG: per-particle draw decision trace (debug harness) */
        {
            FILE *fl = pc_fx_log_handle();
            if (fl) {
                extern int re15_render_pc_dbg_textri_count(void);
                fprintf(fl, "id=%d sub=%d eidx=%d frame=%d w(%d,%d,%d) phys=%d xlat=(%d,%d,%d) drift=(%d,%d,%d) slot=%d q=%d\n",
                        f->effect_id, f->sub_index, f->eff_idx, f->frame,
                        f->x, f->y, f->z, f->phys, f->xlat_x, f->xlat_y, f->xlat_z,
                        f->drift_x, f->drift_y, f->drift_z, slot,
                        re15_render_pc_dbg_textri_count());
                fflush(fl);
            }
        }
        if (vz < 64) continue;                        /* H28 near-clip (also guards gte_divide) */
        /* byte-true GTE RTPS centre: IR1/IR2 sat s16, SZ3 sat u16, UNR reciprocal, IR·n>>16
         * (truncation, not RNDI). Matches the mesh/shadow projection exactly. */
        int32_t _ir1 = vx > 0x7FFF ? 0x7FFF : (vx < -0x8000 ? -0x8000 : vx);
        int32_t _ir2 = vy > 0x7FFF ? 0x7FFF : (vy < -0x8000 ? -0x8000 : vy);
        uint32_t _sz3 = (uint32_t)(vz > 0xFFFF ? 0xFFFF : vz);
        uint32_t _n = re15_gte_divide((uint32_t)cam->fov_screen_dist, _sz3);
        int sx = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
        int sy = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
        /* ===== BYTE-TRUE SPRITE BUILD (FUN_800534c4, arbitrated wf_efa45868-e53) =============
         * anim record = {u8 coord_start, u8 n_sprites, u8 duration, u8 S}: byte0 = the FIRST
         * coord-cell index (NOT frame%count!), byte1 = quads this frame, byte3 = S = the SxS UV
         * cell size AND the scale numerator. Screen size (16.16):
         *   step16 = (S * scale16 * camf) / (sz<<4)      sz = view z; camf = RDT cam fov>>7
         *   w16    = defW(Q12, 0x1000 for the weapon fx) * step16;  h16 analog
         * corner  = projected center + s8(coord dx/dy) * (w16/S)    (dx=dy=-S/2 -> centered)
         * UV      = (u,v)..(u+S,v+S), minus 1 texel per axis when its step > 0x1ffff
         * blend   = ABE (def status bit4; muzzle 0x93 / smoke 0x13 -> ON) = PSX ABR0 50/50. */
        int S = 24, nspr = 1, cstart = f->frame;      /* legacy fallback when no record resolves */
        if (bank && f->eff_idx >= 0) {
            re15_esp_anim_t a;
            if (re15_esp_anim(bank, f->eff_idx, f->frame, &a) == 0) {
                cstart = a.desc & 0xff;
                nspr   = (a.desc >> 8) & 0xff;
                S      = (a.param >> 8) & 0xff;
                if (nspr < 1) nspr = 1;
                if (S < 1) S = 24;
            }
        }
        int camf = 208;                               /* ROOM1140 fallback (26684>>7) */
        {
            extern int pc_fx_camf(void);              /* RDT cam record fov>>7 (below) */
            int cf = pc_fx_camf();
            if (cf > 0) camf = cf;
        }
        int sz = (int)vz; if (sz < 1) sz = 1;
        int64_t step16 = ((int64_t)S * (int64_t)f->scale16 * (int64_t)camf) / ((int64_t)sz << 4);
        int64_t w16 = 0x1000 * step16;                /* w16 = defW * step16 (RAW mult, byte-true —
                                                       * defW u16 0x1000; the 16.16 result absorbs
                                                       * the Q12: px = w16>>16 @0x800535f0) */
        int64_t h16 = w16;                            /* defH identical for the weapon fx */
        int64_t stepX = (S > 0) ? w16 / S : 0;
        int64_t stepY = (S > 0) ? h16 / S : 0;
        int ute = (stepX > 0x1ffff) ? 1 : 0;          /* >=2x magnification edge trim */
        int vte = (stepY > 0x1ffff) ? 1 : 0;
        int abe = (bank == global_bank && (f->effect_id == 2 || f->effect_id == 3));
        int z = (int)vz >> 4;
        {
            extern FILE *pc_fx_log_handle(void);
            FILE *fl = pc_fx_log_handle();
            if (fl) fprintf(fl, "   -> sx=%d sy=%d S=%d n=%d c0=%d w16=%lld camf=%d\n",
                            sx, sy, S, nspr, cstart, (long long)w16, camf);
        }
        extern void re15_render_pc_set_tri_alpha(int a);
        if (abe) re15_render_pc_set_tri_alpha(128);   /* ABR0 = 0.5*back + 0.5*front */
        for (int q = 0; q < nspr; q++) {
            re15_esp_coord_t c;
            if (!bank || f->eff_idx < 0 ||
                re15_esp_coord(bank, f->eff_idx, cstart + q, &c) != 0) break;
            int x0 = sx + (int)(((int64_t)(int8_t)c.w * stepX) >> 16);
            int y0 = sy + (int)(((int64_t)(int8_t)c.h * stepY) >> 16);
            int x1 = sx + (int)((((int64_t)(int8_t)c.w * stepX) + w16) >> 16);
            int y1 = sy + (int)((((int64_t)(int8_t)c.h * stepY) + h16) >> 16);
            if (x1 <= x0 || y1 <= y0) continue;
            int u0 = c.u, v0 = c.v, u1 = c.u + S - ute, v1 = c.v + S - vte;
            re15_render_textured_tri(x0, y0, u0, v0,  x1, y0, u1, v0,
                                     x0, y1, u0, v1,  0, 0, z, 128, 128, 128);
            re15_render_textured_tri(x1, y0, u1, v0,  x1, y1, u1, v1,
                                     x0, y1, u0, v1,  0, 0, z, 128, 128, 128);
        }
        if (abe) re15_render_pc_set_tri_alpha(255);
    }
}

/* The active camera's fx scale field: u16 @ RDT + cut*0x20 + 0x62, >>7 (byte-true
 * FUN_800534c4 camf; ROOM1140 = 26684>>7 = 208). The main loop publishes it per frame. */
static int s_fx_camf = 0;
int  pc_fx_camf(void) { return s_fx_camf; }
static void pc_fx_set_camf(const uint8_t *rdt_raw, size_t rdt_size, int cut)
{
    size_t off = (size_t)cut * 0x20 + 0x62;
    if (rdt_raw && off + 2 <= rdt_size) {
        uint16_t v = (uint16_t)(rdt_raw[off] | (rdt_raw[off + 1] << 8));
        s_fx_camf = v >> 7;
    }
}

/* AZ-round 2026-05-28: sprite.pri overdraw — push parsed mask list to
 * the renderer's overdraw layer (declared in render_pc.c). */
extern void re15_render_pc_set_pri_rects(const int *src_x, const int *src_y,
                                         const int *dst_x, const int *dst_y,
                                         const int *w, const int *h,
                                         const int *depth, int count);
extern void re15_render_pc_set_pri_player(int sx, int sy, int z);

/* PC data-driven per-room prop set (parity with PSX re15_load_room_props): loads
 * room_id's Obj_model_set prop MD1s + TIMs into md1[0..5]/ok[0..5] (TIMs → render
 * slots 4..9). Resets all slots first. room1140 = obj00 only; room1170 = obj00-05.
 * Called inline at boot AND in the cross-room consume (the s_room_prop_* arrays are
 * local to main, so PC keeps rc.load_props=NULL and reloads inline rather than via
 * the PSX-style fn-ptr callback). */
extern uint8_t *re15_asset_read_file(const char *path, int *size);
extern void     re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);

/* Asset-Pfad-Konsolidierung (2026-07-02): read a global asset from the EINEN Asset-Wurzel
 * shared_assets/PSX/<rel>. Dieser Baum enthält jetzt ALLES — den rohen CD-Baum (RDT unter
 * STAGE{N}/, DATA, gepackte Container) PLUS die vor-extrahierten Assets (PLD-Split, RBJ/,
 * per-cut BSS/). Die alte psx_dev/assets_shared- + re15_reborn-Abhängigkeit ist entfernt.
 * Auflösungsreihenfolge: env RE15_ASSET_ROOT → RE15_ASSET_ROOT_DEFAULT → env RE15_CD_ROOT →
 * RE15_CD_ROOT_DEFAULT (alle = shared_assets/PSX; die Defaults sind identisch) → cwd-relative
 * Fallbacks. Returns a malloc'd buffer (caller frees/keeps); *size set; NULL if not found. */
static uint8_t *pc_read_shared(const char *rel, int *size)
{
    /* Höchste Priorität: expliziter Asset-Wurzel-Pfad aus der Umgebung (RE15_ASSET_ROOT),
     * macht den Build cwd-unabhängig — er findet shared_assets/PSX unabhängig davon, von wo
     * aus die .exe gestartet wird. */
    {
        const char *envroot = getenv("RE15_ASSET_ROOT");
        if (envroot && envroot[0]) {
            char epath[256];
            size_t L = strlen(envroot);
            int has_sep = (L > 0 && (envroot[L-1] == '/' || envroot[L-1] == '\\'));
            snprintf(epath, sizeof epath, "%s%s%s", envroot, has_sep ? "" : "/", rel);
            uint8_t *eb = re15_asset_read_file(epath, size);
            if (eb) return eb;
        }
    }
#ifdef RE15_ASSET_ROOT_DEFAULT
    {
        char dpath[256];
        snprintf(dpath, sizeof dpath, "%s/%s", RE15_ASSET_ROOT_DEFAULT, rel);
        uint8_t *db = re15_asset_read_file(dpath, size);
        if (db) return db;
    }
#endif
    /* Dieselbe Wurzel über RE15_CD_ROOT (env) bzw. RE15_CD_ROOT_DEFAULT — deckt zusätzlich die
     * Geschwister-Texturen unter extracted_fx/ ab (…/PSX/../extracted_fx/). Der Default ist mit
     * RE15_ASSET_ROOT_DEFAULT identisch (beide = shared_assets/PSX); getrennt gehalten nur wegen
     * der env-Override-Semantik und des ../-Geschwister-Pfads. */
    {
        const char *cdroot = getenv("RE15_CD_ROOT");
#ifdef RE15_CD_ROOT_DEFAULT
        if (!cdroot || !cdroot[0]) cdroot = RE15_CD_ROOT_DEFAULT;
#endif
        if (cdroot && cdroot[0]) {
            char cpath[300];
            snprintf(cpath, sizeof cpath, "%s/%s", cdroot, rel);        /* CD-Datei: DATA/... */
            uint8_t *cb = re15_asset_read_file(cpath, size);
            if (cb) return cb;
            snprintf(cpath, sizeof cpath, "%s/../%s", cdroot, rel);     /* Geschwister: extracted_fx/... */
            cb = re15_asset_read_file(cpath, size);
            if (cb) return cb;
        }
    }
    /* Asset-Pfad-Konsolidierung (2026-07-02): die alten assets_shared- + re15_reborn-Ketten
     * und Legacy-Namens-Aliase sind ENTFERNT. Alles kommt aus der EINEN Wurzel shared_assets/PSX
     * (oben via env RE15_ASSET_ROOT/RE15_CD_ROOT bzw. den Compile-Default abgedeckt). Diese
     * cwd-relativen Einträge sind nur der Fallback für CTest/headless aus wechselnden
     * Arbeitsverzeichnissen. */
    {
        static const char *rel_roots[] = { "shared_assets/PSX/", "../shared_assets/PSX/",
                                           "../../shared_assets/PSX/", "../../../shared_assets/PSX/", NULL };
        char path[300];
        for (int i = 0; rel_roots[i]; i++) {
            snprintf(path, sizeof path, "%s%s", rel_roots[i], rel);
            uint8_t *b = re15_asset_read_file(path, size);
            if (b) return b;
        }
    }
    return NULL;
}

/* Scratch for re15_apply_room_cinematic (the shared overlay parses into this before copying
 * to the real destination). File-scope static so it's NOT a ~7 KB stack local; PC RAM is
 * unconstrained. (The PSX caller instead reuses its CD staging buffer — see asset_psx.c.) */
static re15_emd_skeleton_t  s_cine_scratch_skel;
static re15_emd_animation_t s_cine_scratch_anim;

/* Generic enemy loader (globalization 2026-06-13): load EM<type>.EMD from the shared
 * tree into a registry bank + upload its texture to render slot 11+bank. Lazy: called
 * from the NPC render loop the first time an actor of `type` appears. Replaces the
 * em21-only special case for every OTHER enemy type. */
/* Cached CDEMD0.EMS — the enemy-model archive every generic enemy loads from
 * (the disc has no per-type EM<NN>.EMD; ~4.7 MB, read once, kept resident). */
static const uint8_t *pc_cdemd(size_t *out_sz)
{
    static uint8_t *s_ems = NULL; static int s_sz = 0; static int s_tried = 0;
    if (!s_tried) { s_tried = 1; s_ems = pc_read_shared("EMD/CDEMD0.EMS", &s_sz); }
    if (out_sz) *out_sz = (size_t)s_sz;
    return s_ems;
}

static void pc_enemy_load(uint8_t type)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    if (type == 0 || re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;                                   /* registry full */

    /* Enemy models live inside CDEMD0.EMS (no per-type EM<NN>.EMD on the disc).
     * Try a standalone split file first (back-compat / future), else extract the
     * type's EMD blob out of the archive (re15_ems, the byte-true port of the
     * Java EMS splitter). Either way `buf` ends up a private, bank-owned copy. */
    uint8_t *buf = NULL; size_t buflen = 0;
    char rel[32]; int sz = 0;
    snprintf(rel, sizeof rel, "EMD/EM%02X.EMD", type);
    buf = pc_read_shared(rel, &sz);                    /* malloc'd if it exists */
    if (buf) {
        buflen = (size_t)sz;
    } else {
        int idx = re15_ems_index_for_type(type);
        size_t ems_sz = 0; const uint8_t *ems = (idx >= 0) ? pc_cdemd(&ems_sz) : NULL;
        size_t off = 0, len = 0;
        if (ems && re15_ems_get_entry(ems, ems_sz, idx, &off, &len) == 0) {
            buf = (uint8_t *)malloc(len);             /* private copy: banks free their buf */
            if (buf) { memcpy(buf, ems + off, len); buflen = len; }
        }
    }
    if (!buf) {
        eb->type = 0;
        fprintf(stderr, "[enemy] EM%02X model not found (no split file, not in CDEMD0.EMS)\n", type);
        return;
    }

    re15_tim_t tim = {0};
    if (re15_emd_parse_container(buf, buflen, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        free(buf); eb->type = 0;
        fprintf(stderr, "[enemy] EM%02X EMD parse FAILED\n", type);
        return;
    }
    eb->buf = buf;                                     /* md1/skel/anim point into buf */
    /* Load the LOCOMOTION bank (bank 0) too — the STAGE1 zombie APPROACH state (+0x5=0x13) plays it
     * (the 6-clip loco set with the 0x2000 foot-lock frame flags), which parse_container skips in
     * favour of the 43-clip action bank. Aliases into buf (no extra alloc). */
    eb->loco_ok = (re15_emd_parse_loco_bank(buf, buflen, &eb->skel_loco, &eb->anim_loco) == 0);
    /* Load the GRAB-VICTIM bank (bank 2) — the animation the grab plays on LEON (struggle clips 0-5 /
     * collapse 6/7); PL00-compatible. Used by the player-victim FSM when a zombie grabs the player. */
    eb->victim_ok = (re15_emd_parse_victim_bank(buf, buflen, &eb->skel_victim, &eb->anim_victim) == 0);
    int slot = 11 + (int)(eb - g_enemy);
    if (tim.width > 0 && tim.height > 0 && slot < 24) {
        re15_render_pc_upload_tim_slot(&tim, slot);
        eb->pc_tex_slot = slot;
    }
    eb->ok = 1;
    fprintf(stderr, "[enemy] EM%02X loaded: %d meshes, %d bones, %d clips -> slot %d\n",
            type, eb->md1.mesh_count, eb->skel.bone_count, eb->anim.clip_count, eb->pc_tex_slot);
}

static void pc_load_room_prop_set(const re15_rdt_t *rdt, re15_md1_t md1[6], int ok[6])
{
    /* Globalization Phase 3-A (2026-06-13): the room's Obj_model_set props (MD1+TIM)
     * are SLICED from the parsed RDT (rdt->prop_md1/tim[], the 0x30 model-pointer
     * table) — was per-room room####_obj##.{md1,tim} files. Pointers alias the resident
     * RDT buffer (byte-true == the old files). prop[op] → md1[op] + render slot 4+op.
     * Parity with PSX re15_load_room_props. Caller passes the right RDT: &rdt at boot,
     * &g_room_rdt on the cross-room reload. */
    for (int op = 0; op < 6; op++) ok[op] = 0;
    if (!rdt) return;
    int nprops = rdt->prop_count < 6 ? rdt->prop_count : 6;
    for (int op = 0; op < nprops; op++) {
        const uint8_t *mb = rdt->prop_md1[op]; int msz = rdt->prop_md1_size[op];
        const uint8_t *tb = rdt->prop_tim[op]; int tsz = rdt->prop_tim_size[op];
        if (mb && re15_md1_parse(mb, (size_t)msz, &md1[op]) == 0) ok[op] = 1;
        if (tb) { re15_tim_t tt; if (re15_tim_parse(tb, tsz, &tt) == 0)
                      re15_render_pc_upload_tim_slot(&tt, 4 + op); }
    }
}

/* ── FE-4 memory-card save/load ────────────────────────────────────────────── */
#define RE15_CARD_PATH "re15_card.mcr"      /* the PSX card image, next to the CWD */

/* Deferred resume: CONTINUE loads a save block here + arms the flag; the game loop
 * applies it AFTER the room + player have been set up, so the saved state wins over
 * the new-game briefing defaults. */
static re15_savedata_t s_resume_sd;
static int             s_resume_pending = 0;
/* Persistent save number (RE1.5 DAT_800b0fbd): restored from the loaded save on CONTINUE (case 9
 * copies it inside the save region), then carried forward per save — so a load-then-save continues
 * the counter from the loaded save, not max-across-slots+1. 0 = "not yet seeded from a load". */
static uint16_t        s_save_counter = 0;

/* The memory-card slot screen — a blocking modal (like the title). save_mode:
 * 1 = save *sd to the chosen slot; 0 = load the chosen slot into s_resume_sd and
 * return its resume room in *out_room (NOT restored yet — the game loop defers it).
 * Returns the chosen slot (0..4), or -1 on cancel/failure. */
/* Write `sd` to `slot` with a byte-true RE1.5 directory title ("BIO HAZARD <char> /NN/",
 * DAT_800107cc/f8 templates: Elza/Leon + the 2-digit save counter). Returns 1 on success. */
static int pc_do_save(int slot, const re15_savedata_t *sd)
{
    char title[RE15_MC_TITLE_LEN];
    /* On-card BIOS title = DAT_800107cc/f8 template "BIO HAZARD <Leon|Elza> /NN/" (what a PSX card
     * manager shows). The in-game slot list instead reconstructs the sysmes "<Leon|Elza> /NN/". */
    snprintf(title, sizeof title, "BIO HAZARD %s /%02d/",
             sd->character ? "Elza" : "Leon", sd->save_count % 100);
    return re15_memcard_save(RE15_CARD_PATH, slot, sd, title) == 0;
}

/* Build the byte-true RE1.5 slot title as raw sysmes atlas codes. Template DEBUG.BIN @0x617f
 * (verified verbatim): 05 07 "Leon" 05 00 38 0c 0c 38 ...  /  05 06 "Elza" 05 00 38 0c 0c 38 ...
 * "Irons' Office" (apostrophe = atlas 0x3A; room stubbed, FUN_80026e4c returns 0). The 0x05 op is
 * the colour selector (`attr = next_byte & 7`, cf. msg_common.c:537): Leon = 07 (attr 7, blue
 * 32,80,232), Elza = 06 (attr 6, red 152,0,72), 05 00 resets the counter/room to the default
 * palette. re15_render_pc_game_codes interprets the embedded 0x05 ops. */
static int pc_slot_title_codes(uint8_t *tc, int character, int count)
{
    static const uint8_t leon[] = { 0x28, 0x41, 0x4b, 0x4a };                       /* "Leon" */
    static const uint8_t elza[] = { 0x21, 0x48, 0x56, 0x3d };                       /* "Elza" */
    static const uint8_t room[] = { 0x25,0x4e,0x4b,0x4a,0x4f, 0x3a, 0x00, 0x2b,0x42,0x42,0x45,0x3f,0x41 }; /* "Irons' Office" */
    int n = 0;
    const uint8_t *nm = character ? elza : leon;
    tc[n++] = 0x05; tc[n++] = character ? 0x06 : 0x07; /* name colour: Elza=06 (red), Leon=07 (blue) */
    for (int k = 0; k < 4; k++)  tc[n++] = nm[k];
    tc[n++] = 0x05; tc[n++] = 0x00;                    /* reset to default palette */
    tc[n++] = 0x00;                                    /* space */
    tc[n++] = 0x38;                                    /* counter delimiter glyph */
    tc[n++] = (uint8_t)(0x0c + (count / 10) % 10);     /* tens digit ('0' = atlas 0x0c) */
    tc[n++] = (uint8_t)(0x0c + count % 10);            /* ones digit */
    tc[n++] = 0x38;                                    /* counter delimiter glyph */
    tc[n++] = 0x00; tc[n++] = 0x00;                    /* gap */
    for (int k = 0; k < 13; k++) tc[n++] = room[k];
    return n;
}

/* The byte-true RE1.5 memory-card SAVE/LOAD screen (FUN_80025c00 / FUN_80026658), adapted for
 * the PC single-card backend: the "MEMORY CARD BG" (DATA/TYPE00.TIM) full-screen, the dim panel
 * (drawn in render_pc), a header, the 5-slot list + an EXIT row, the SAVE/LOAD label, a blinking
 * cursor, and the OVERWRITE-confirm + result sub-screens. Coords are the RE'd 320x240 positions.
 * save_mode: 0 = SAVE (*sd), 1 = LOAD (into s_resume_sd, resume room -> *out_room). Returns the
 * chosen slot (0..4) or -1 on cancel/exit. */
static int pc_run_memcard_screen(int save_mode, const re15_savedata_t *sd, uint16_t *out_room)
{
    extern int  re15_render_pc_game_text(int x, int y, const char *str, int attr);   /* RE1.5 game font */
    extern int  re15_render_pc_game_text_width(const char *str);
    extern int  re15_render_pc_game_codes(int x, int y, const uint8_t *codes, int n, int attr);
    extern void re15_render_pc_show_cardbg(const re15_tim_t *tim);
    extern void re15_render_pc_hide_cardbg(void);
    extern void re15_render_pc_card_cursor(int x, int y, int show);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_screenshot(const char *path);
    re15_render_pc_hide_title();        /* CONTINUE opens this from the title loop — the card BG must win */
    re15_render_pc_hide_title_menu();   /* also hide the TMOJI menu sprites (NEW GAME/LOAD GAME/OPTION) */

    /* "MEMORY CARD BG" = DATA/TYPE00.TIM (CD file 0x23): a pre-rendered PC/CRT terminal desk
     * (RE1.5's save point is a computer). Full-screen 320x240; loaded once. */
    static re15_tim_t s_card_bg = {0};
    static int s_card_bg_tried = 0;
    if (!s_card_bg_tried) { s_card_bg_tried = 1; int bsz = 0;
        uint8_t *bb = pc_read_shared("DATA/TYPE00.TIM", &bsz);
        if (bb) re15_tim_parse(bb, bsz, &s_card_bg); }

    int  used[RE15_SAVE_SLOTS];
    char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN];
    re15_memcard_list(RE15_CARD_PATH, used, titles);
    /* In-game slot label = byte-true RE1.5 sysmes idx 0x17 template (FUN_80026658), reconstructed
     * from each slot's savedata (character + save counter) — NOT the on-card BIOS title. */
    int slot_char[RE15_SAVE_SLOTS], slot_cnt[RE15_SAVE_SLOTS];
    for (int i = 0; i < RE15_SAVE_SLOTS; i++) {
        slot_char[i] = 0; slot_cnt[i] = 0;
        if (used[i]) {
            re15_savedata_t sl;
            if (re15_memcard_load(RE15_CARD_PATH, i, &sl) == 0) { slot_char[i] = sl.character; slot_cnt[i] = sl.save_count % 100; }
            else used[i] = 0;
        }
    }

    enum { ST_LIST, ST_OVERWRITE, ST_RESULT } st = ST_LIST;
    int cursor = 0, ow = 0 /* overwrite cursor defaults to Yes/top (FUN_80025c00 case7: uVar8=0) */, result = -1, running = 1;
    const char *msg = 0;
    unsigned blink = 0;
    int nav_delay = 0;   /* FUN_80025c00 auto-repeat: 0x13 initial, 0x05 repeat (local_40) */
    int auto_drive = getenv("RE15_CARD_AUTO") != NULL;
    const char *card_shot = getenv("RE15_CARD_SHOT");
    unsigned af = 0;

    while (running) {
        re15_render_begin_frame();
        re15_input_tick();
        if (s_card_bg.pixels) re15_render_pc_show_cardbg(&s_card_bg);
        else re15_render_background_gradient(0, 0, 24, 0, 0, 0);

        /* All text in the RE1.5 GAME font (TEX.TIM), attr 0; centered via glyph width. Strings are
         * the byte-true RE1.5 sysmes (DEBUG.BIN @0x5fd9): idx2 "Memory Card 1" header, idx0/1
         * "Do not save/load" exit, idx0x10 "NO DATA", idx8 "Overwrite?", idx9/0xa Yes/No,
         * idx0x11 "Access error", idx0x15 "Fail in save". */
        re15_render_pc_game_text(160 - re15_render_pc_game_text_width("Memory Card 1") / 2, 24, "Memory Card 1", 0);
        for (int i = 0; i < RE15_SAVE_SLOTS; i++) {                        /* 5 slots x=41 y=56+20i */
            if (used[i]) { uint8_t tc[48]; int tn = pc_slot_title_codes(tc, slot_char[i], slot_cnt[i]);
                           re15_render_pc_game_codes(41, 56 + i * 20, tc, tn, 0); }
            else re15_render_pc_game_text(41, 56 + i * 20, "NO DATA", 0);  /* sysmes idx 0x0f */
        }
        re15_render_pc_game_text(41, 156, save_mode ? "Do not save" : "Do not load", 0);  /* row 5 = exit */

        if (st == ST_OVERWRITE) {
            re15_render_pc_game_text(160 - re15_render_pc_game_text_width("Overwrite?") / 2, 178, "Overwrite?", 0);
            re15_render_pc_game_text(139, 196, "Yes", 0);
            re15_render_pc_game_text(139, 214, "No", 0);
        } else if (st == ST_RESULT && msg) {
            re15_render_pc_game_text(160 - re15_render_pc_game_text_width(msg) / 2, 196, msg, 0);
        }

        /* the ▶ selection cursor (render_pc draws a filled white triangle at this position) */
        if (st == ST_LIST)           re15_render_pc_card_cursor(25, 56 + cursor * 20, !(blink & 0x10));  /* FUN_80026be8(0x19,row*0x14+0x38) */
        else if (st == ST_OVERWRITE) re15_render_pc_card_cursor(123, 196 + ow * 18, !(blink & 0x10));
        else                         re15_render_pc_card_cursor(0, 0, 0);
        re15_render_end_frame();
        blink++;

        uint16_t pp = g_engine.pad_pressed;
        if (auto_drive) {
            { const char *sa = getenv("RE15_CARD_SHOT_AF"); unsigned saf = sa ? (unsigned)atoi(sa) : 3;
              if (card_shot && af == saf) re15_render_pc_screenshot(card_shot); }
            { const char *hd = getenv("RE15_CARD_HOLD"); unsigned hold = hd ? (unsigned)atoi(hd) : 6;
              if (af == hold || af == hold + 20) pp |= RE15_PAD_BIT_SQUARE; }   /* open, then confirm/dismiss */
            af++;
        }
        /* Byte-true confirm/cancel (FUN_80025c00 cases 1/2/6 + k_pad_remap[0]): the FSM checks the
         * config-remapped action bit 14 = SQUARE (0x8000) and START to PROCEED/confirm, and action
         * bit 15 = CROSS (0x4000) to go BACK/cancel (CROSS → iVar6=0). RE1.5 uses the Japanese menu
         * convention here (○/□ family confirm, ✕ cancel) — the inverse of the western dialog/FF path. */
        uint16_t ok     = pp & (RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_START);
        uint16_t cancel = pp & RE15_PAD_BIT_CROSS;

        /* Byte-true cursor navigation (FUN_80025c00): read the HELD pad (DAT_800ac760, raw
         * PadRead layout) with a 19/5-frame auto-repeat — up = UP, down = DOWN | SELECT
         * (0x4100 = PADLdown | PADselect, so SELECT also steps down). A fresh press steps
         * immediately + arms the 0x13 initial delay; while held, repeat every 0x05 frames. */
        uint16_t held    = g_engine.pad_current;
        uint16_t nav_msk = RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN | RE15_PAD_BIT_SELECT;
        int nav_step = 0;
        if ((pp & nav_msk) == 0 && nav_delay != 0) { nav_delay--; }
        else { nav_delay = (pp & nav_msk) ? 0x13 : 0x05; nav_step = 1; }
        int nav_up   = nav_step && (held & RE15_PAD_BIT_UP);
        int nav_down = nav_step && (held & (RE15_PAD_BIT_DOWN | RE15_PAD_BIT_SELECT));

        if (st == ST_LIST) {
            if (nav_up)   { cursor = (cursor + RE15_SAVE_SLOTS) % (RE15_SAVE_SLOTS + 1); blink = 0; }
            if (nav_down) { cursor = (cursor + 1) % (RE15_SAVE_SLOTS + 1); blink = 0; }
            if (cancel) running = 0;
            else if (ok) {
                if (cursor == RE15_SAVE_SLOTS) running = 0;               /* EXIT row */
                else if (save_mode) {                                    /* SAVE */
                    if (used[cursor]) { st = ST_OVERWRITE; ow = 0; blink = 0; }  /* default Yes (FUN_80025c00) */
                    else if (pc_do_save(cursor, sd)) { result = cursor; running = 0; }  /* no completion msg (byte-true) */
                    else { msg = "Fail in save"; st = ST_RESULT; }       /* sysmes idx 0x15 */
                } else {                                                 /* LOAD */
                    /* Empty slot -> sysmes 0x14 "There are no files" (FUN_80025c00 case7: empty
                     * marker 5 -> unaff_s1=3 -> state 0xc -> idx unaff_s1+0x11=0x14); the LIST already
                     * labels empty slots "NO DATA" (0x10). Load I/O failure -> sysmes 0x17 "Fail in
                     * load" (case9: unaff_s1=6 -> idx 0x17), not "Access error" (0x12, card-level). */
                    if (!used[cursor]) { msg = "There are no files"; st = ST_RESULT; }
                    else if (re15_memcard_load(RE15_CARD_PATH, cursor, &s_resume_sd) == 0) {
                        if (out_room) *out_room = s_resume_sd.room; result = cursor; running = 0;
                    } else { msg = "Fail in load"; st = ST_RESULT; }
                }
            }
        } else if (st == ST_OVERWRITE) {
            if (nav_up || nav_down) { ow ^= 1; blink = 0; }
            if (cancel) { st = ST_LIST; blink = 0; }
            else if (ok) {
                if (ow == 0) {                                           /* Yes -> overwrite */
                    if (pc_do_save(cursor, sd)) { result = cursor; running = 0; }  /* byte-true: no completion msg */
                    else { msg = "Fail in save"; st = ST_RESULT; }
                } else { st = ST_LIST; blink = 0; }                      /* No */
            }
        } else {                                                         /* ST_RESULT */
            if (ok || cancel) {
                if (result >= 0) running = 0;                            /* saved/loaded -> done */
                else { st = ST_LIST; msg = 0; }                          /* error -> back to list */
            }
        }
    }
    re15_render_pc_hide_cardbg();
    return result;
}

/* PLAYER-SELECT 3D MODELS (Leon PL00 left / Elza PL01 right) — byte-true camera + transform
 * (RE15_PLAYER_SELECT_DRAW.md, workflow wf_0aab308c, savestate-verified). Self-contained render:
 * reuses the port's pose (re15_compute_actor_kf/re15_skel_compute_pose) + camera-compose + textri
 * building blocks, with NO game-state coupling. Camera (wide idle shot): projection H=1000, screen
 * center (160,120); global view R=diag(4104,4096,4104), TR=(0,0,20039). Models at world POS
 * (Leon -1568 / Elza +1568, Y=2036, Z=0), rot_y 0x404, idle clip 2. Both drawn every frame. */
typedef struct { re15_md1_t md1; re15_emd_skeleton_t skel; re15_emd_animation_t anim; int tim_slot; int ok;
                 re15_md1_t wpn_md1; int wpn_ok; int wpn_tim_slot; } pselect_model_t;

static void pselect_load_model(pselect_model_t *m, const char *md1p, const char *eddp,
                               const char *base_emrp, const char *kf_emrp, const char *timp, int tim_slot,
                               const char *wpn_plw, int wpn_tim_slot)
{
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    memset(m, 0, sizeof *m); m->tim_slot = tim_slot; m->wpn_tim_slot = wpn_tim_slot;
    int sz = 0;
    uint8_t *mb = pc_read_shared(md1p, &sz); if (!mb || re15_md1_parse(mb, sz, &m->md1) != 0) return;
    uint8_t *eb = pc_read_shared(eddp, &sz); if (!eb || re15_emd_parse_animation(eb, sz, &m->anim) != 0) return;
    /* The skeleton HIERARCHY + bind pose come from the BASE PLD EMR (a full EMR: header @0x64
     * bones-table / @0xb0 keyframes). The weapon PLW EMR is KEYFRAME-ONLY (8-byte header + N*80
     * keyframes, NO bone table) and shares the base skeleton — byte-true to the original, which
     * keeps Leon's resident PL00.PLD skeleton (@0x801bd814) and points model+0x170 at the weapon
     * keyframes. Parsing the keyframe-only PLW EMR as a full EMR read the hierarchy out of its
     * 8-byte header → garbage parents → the folded/flying skeleton. So: parse base for the
     * skeleton, then OVERRIDE the keyframe block with the weapon EMR's keyframes. */
    uint8_t *rb = pc_read_shared(base_emrp, &sz); if (!rb || re15_emd_parse_skeleton(rb, sz, &m->skel) != 0) return;
    if (kf_emrp) {
        int ksz = 0; uint8_t *kb = pc_read_shared(kf_emrp, &ksz);
        if (kb && ksz > 8) {
            int koff = kb[2] | (kb[3] << 8);   /* weapon EMR header @2 = keyframes_offset (= 8) */
            int kfs  = m->skel.keyframe_size_bytes > 0 ? m->skel.keyframe_size_bytes : 80;
            m->skel.keyframe_data      = kb + koff;
            m->skel.keyframe_data_size = (size_t)(ksz - koff);
            m->skel.keyframe_count     = (ksz - koff) / kfs;
        }
    }
    re15_tim_t tim = {0}; int tsz = 0; uint8_t *tb = pc_read_shared(timp, &tsz);
    if (tb && re15_tim_parse(tb, tsz, &tim) == 0) re15_render_pc_upload_tim_slot(&tim, tim_slot);
    /* EQUIPPED WEAPON (W03 handgun): the hand+gun mesh is PLW section [2] (de[2]..de[3]), drawn on
     * bone 11 at render time. It is textured from the character's BODY skin TIM (all its tris read
     * page 0x81 / clut 0x7840 = the same tpage as the body's hand mesh) — the PLW's own dir[3] TIM
     * is NOT referenced by this mesh, so we don't upload it. (void)wpn_tim_slot. */
    (void)wpn_tim_slot;
    if (wpn_plw) {
        int psz = 0; uint8_t *plw = pc_read_shared(wpn_plw, &psz);   /* stays resident (MD1 borrows) */
        if (plw && psz >= 16) {
            uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
            uint32_t de[4] = {0};
            if (diroff + 16 <= (uint32_t)psz)
                for (int k = 0; k < 4; k++)
                    de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                                       (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
            if (de[3] > de[2] && re15_md1_parse(plw + de[2], (int)(de[3] - de[2]), &m->wpn_md1) == 0)
                m->wpn_ok = 1;
        }
    }
    m->ok = 1;
    if (getenv("RE15_PSELECT_DIAG")) {
        fprintf(stderr, "PSELECT LOAD base=%s kf=%s: md1.mesh=%d edd.clip=%d emr.bone=%d emr.kf=%d\n",
                base_emrp, kf_emrp ? kf_emrp : "(none)", m->md1.mesh_count, m->anim.clip_count,
                m->skel.bone_count, m->skel.keyframe_count);
        if (m->wpn_ok && m->wpn_md1.mesh_count > 0) {
            const re15_md1_mesh_t *wm = &m->wpn_md1.meshes[0];
            fprintf(stderr, "  WPN mesh: tris=%d quads=%d tvtx=%d page=0x%x (body page 0x%x)\n",
                    wm->triangle_count, wm->quad_count, wm->tri_vertex_count,
                    wm->triangle_count>0 ? wm->triangle_uvs[0].page : 0,
                    m->md1.mesh_count>11 && m->md1.meshes[11].triangle_count>0 ? m->md1.meshes[11].triangle_uvs[0].page : 0);
        }
    }
}

static void pselect_render_model(const pselect_model_t *m, int32_t px, int32_t py, int32_t pz,
                                 int16_t rot_y, uint32_t frame, const re15_camera_view_t *cam)
{
    extern void re15_render_pc_bind_tim_slot(int slot);
    if (!m->ok || m->anim.clip_count <= 0 || m->md1.mesh_count <= 0) return;
    re15_actor_t a; memset(&a, 0, sizeof a);
    /* BYTE-TRUE idle clip = model+0x94 = 2. Verified against the LIVE player-select savestate
     * (mzd_after_flow.sav): both model structs read model->0x94 = 2, ->0x95 (anim_frame) = 37,
     * ->0x6a rot = 1028. FUN_8001f314 (@0x8001f324 `lbu v0,148(t0)`) reads the clip from
     * g_entity->0x94; TITLE.BIN never writes +0x94 (the EXE model spawn seeds it to 2). The
     * original renders clip 2 as an upright arms-down idle — the port's clip-2 contortion is a
     * POSE-computation bug, NOT a wrong clip. */
    int clip = 2;   /* RE15_PSELECT_CLIP overrides for the clip sweep */
    { const char *cc = getenv("RE15_PSELECT_CLIP"); if (cc) clip = atoi(cc); }
    a.motion = (int16_t)clip; a.anim_frame = (int32_t)frame; a.rot_y = rot_y;
    /* QUERY mode: re15_skel_compute_pose reads g_anim_pose_actor for the FRAC crossfade + the
     * player head-look; the player-select is a standalone render (no game actor), so force NULL =
     * raw keyframe pose, no crossfade, no head-look (same as the enemy_ai/damage pose queries).
     * Leaving it at a stale in-game pointer blended the idle against a foreign skeleton. */
    extern void *g_anim_pose_actor; void *save_gpa = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int kf = re15_compute_actor_kf(&m->anim, &m->skel, &a, clip, frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    if (re15_skel_compute_pose(&m->skel, kf, poses) != 0) { g_anim_pose_actor = save_gpa; return; }
    g_anim_pose_actor = save_gpa;
    if (getenv("RE15_PSELECT_DIAG") && frame == 37) {
        fprintf(stderr, "PSELECT POSE tim%d kf=%d bones=%d root.trans=(%d,%d,%d) b8.trans=(%d,%d,%d) b6=(%d,%d,%d)\n",
                m->tim_slot, kf, m->skel.bone_count,
                (int)poses[0].trans[0],(int)poses[0].trans[1],(int)poses[0].trans[2],
                (int)poses[8].trans[0],(int)poses[8].trans[1],(int)poses[8].trans[2],
                (int)poses[6].trans[0],(int)poses[6].trans[1],(int)poses[6].trans[2]); }
    int32_t yaw[9]; re15_camera_yaw_matrix_angle(rot_y, yaw);
    int nb = m->skel.bone_count; if (nb > m->md1.mesh_count) nb = m->md1.mesh_count;
    const int cx = 160, cy = 120;    /* OFX/OFY */
    const uint8_t C = 0x80;          /* neutral fallback tint when a mesh part lacks per-vertex normals */
    /* NCCT PER-VERTEX LIGHTING — BYTE-TRUE, verified end-to-end against the live original
     * (mzd_after_flow.sav, 2026-07-14). The player-select models are GTE NCCT-lit (front-end draw
     * FUN_8001e8c8 → FUN_800254a0, GTE cmd 0x4b18043f), NOT flat. The cut below is read
     * FIELD-FOR-FIELD from the scene light struct @0x80190088 (NOT guessed/calibrated):
     *   global_scale=1, type_flags=(1,1,1) directional, colors all (128,128,128),
     *   ambient/BackColor (76,76,86), positions L0(-4600,-4000,-23200)/L1(-99,-2000,-29200)/
     *   L2(2000,2000,2000), ranges 20000/10000/20000.
     * Cross-checked against the FINAL GTE matrices in RAM: LLM @0x80076d14 =
     *   {786,684,3964 / 14,281,4089 / -2365,-2365,-2365}  — which is exactly
     *   -normalize(position) per light, i.e. re15_light_setup_actor reproduces the GTE LLM to ±1;
     *   LCM @0x80076d34 = 2048 = colour 128<<4 for all 3 lights. So light DATA + NCCT MATH are byte-true.
     * (A one-time WRONG Z-negation experiment was reverted: the positions above are the byte-true ones.)
     * Verified visually at native 320×240 vs the PSX framebuffer: Leon clearly lit, Elza near-black.
     * The only remaining brightness delta vs a DuckStation *capture* is a GLOBAL ~7/255 framebuffer-
     * precision offset (settings DitheringMode=TrueColor round-to-nearest vs the port's <<3 truncate)
     * that shifts the un-lit static backdrop by the SAME amount as the lit model — an engine-wide
     * texture-precision matter, not a player-select lighting difference. Same pipeline for both
     * characters; only the mirrored world-X (Leon -1568 / Elza +1568) changes the light direction. */
    static const re15_light_cut_t s_ps_cut = {
        1, {1,1,1}, {{128,128,128},{128,128,128},{128,128,128}}, {76,76,86},
        {{-4600,-4000,-23200},{-99,-2000,-29200},{2000,2000,2000}}, {20000,10000,20000} };
    re15_actor_lightctx_t lctx_world;
    { int32_t apos[3] = { px, py, pz }; re15_light_setup_actor(&s_ps_cut, apos, NULL, &lctx_world); }
    /* NCCT ON by default (disable for A/B with RE15_PSELECT_NO_NCCT). Byte-true: the models ARE GTE
     * NCCT-lit; the selected char is clearly lit and the UNSELECTED char is near-black — matching the
     * PSX framebuffer (verified at native 320×240: Leon lit, Elza near-black; the earlier "too bright"
     * apparent gap was a mis-aligned crop between the 1024-wide capture and the 640-wide render). */
    int ncct_on = getenv("RE15_PSELECT_NO_NCCT") == NULL;
    /* EQUIPPED WEAPON (byte-true, workflow wf_8837a549 + user-corrected): the player-select draws the
     * model through ONE shared skeleton-part pass (FUN_8001e8c8 walks model+0x188 parts); the
     * equipped weapon is part[11] = a HAND+GUN mesh (35 verts) that REPLACES the plain right-hand
     * mesh (23 verts) — so the character's hand is drawn INCLUDING the weapon. BOTH characters carry
     * it (user: "sowohl Leon als auch Elza haben ihre Hand inklusive Waffe" — the savestate that
     * showed only Leon armed was a transient load state). So at bone 11, draw the weapon MD1
     * (PLW dir[2]) with the weapon TIM (PLW dir[3]) using bone 11's own composed matrix, each
     * character its own PLW (Leon PL00W03 / Elza PL04W03) — same slot-per-tri queue as in-game. */
    for (int bi = 0; bi < nb; bi++) {
        const re15_skel_pose_t *p = &poses[bi];
        int32_t yr[9], yt[3];
        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) {
            int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)p->rot[k*3+c];
            yr[r*3+c] = (int32_t)(s >> 12); }
        for (int r = 0; r < 3; r++) {
            int64_t s = 0; for (int k = 0; k < 3; k++) s += (int64_t)yaw[r*3+k] * (int64_t)p->trans[k];
            yt[r] = (int32_t)(s >> 12); }
        int32_t bwt[3] = { yt[0] + px, yt[1] + py, yt[2] + pz };
        int32_t bm[9], bt[3];
        re15_camera_compose_view_bone(cam, yr, bwt, bm, bt);
        /* rotate the world light context into THIS bone's frame (yr = R_y(yaw)×pose.rot = the bone's
         * world rotation), so a RAW bone-local vertex normal shades as L_world·N_world (byte-true
         * per-bone SetLightMatrix, FUN_8001e9ec). */
        re15_actor_lightctx_t lctx; re15_light_ctx_rotate_for_bone(&lctx_world, yr, &lctx);
        /* EQUIPPED WEAPON (byte-true, verified against the live original savestate): at bone 11 the
         * hand+gun mesh REPLACES the plain hand. Attach = bone 11 — the port's bone-11 world matrix
         * is byte-identical to the original's part[11] matrix (@+0x40 rot [2 204 4088]/[523 4048
         * -205]/[-4060 522 -18], trans (-2118,385,-66)). Mesh = PL04W03.PLW dir[2] (verts match the
         * savestate 6/6; a shared weapon buffer, so BOTH characters use PL04W03). The mesh is
         * textured entirely from the character's BODY skin TIM — every tri/quad reads page 0x81 /
         * clut 0x7840, the SAME tpage as body mesh 11 — so it binds m->tim_slot (NOT the separate
         * PLW dir[3] gun TIM, which this mesh does not reference). Both characters carry it. */
        int wpn_here = (bi == 11 && m->wpn_ok && m->wpn_md1.mesh_count > 0);
        const re15_md1_mesh_t *mesh = wpn_here ? &m->wpn_md1.meshes[0] : &m->md1.meshes[bi];
        /* The shared PL04W03 hand+gun mesh is textured from PL04's skin TIM (the gun art lives in
         * PL04.TIM, slot 21) for BOTH characters — Leon's own body TIM (slot 20) has no gun there. */
        re15_render_pc_bind_tim_slot(wpn_here ? 21 : m->tim_slot);
#define PSV(vp, ox, oy, owz) do { \
        int32_t _x=(vp)->x,_y=(vp)->y,_z=(vp)->z; \
        int32_t _vx=(int32_t)(((int64_t)_x*bm[0]+(int64_t)_y*bm[1]+(int64_t)_z*bm[2])>>12)+bt[0]; \
        int32_t _vy=(int32_t)(((int64_t)_x*bm[3]+(int64_t)_y*bm[4]+(int64_t)_z*bm[5])>>12)+bt[1]; \
        int32_t _vz=(int32_t)(((int64_t)_x*bm[6]+(int64_t)_y*bm[7]+(int64_t)_z*bm[8])>>12)+bt[2]; \
        if(_vz<64){(owz)=-1.0f;(ox)=0;(oy)=0;} else { \
            int32_t _i1=_vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx); \
            int32_t _i2=_vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy); \
            uint32_t _s3=_vz>0xFFFF?0xFFFFu:(uint32_t)_vz; \
            uint32_t _n=re15_gte_divide((uint32_t)cam->fov_screen_dist,_s3); \
            (ox)=cx+(int)(((int64_t)_i1*(int64_t)_n)>>16); \
            (oy)=cy+(int)(((int64_t)_i2*(int64_t)_n)>>16); \
            (owz)=(float)_vz; } } while(0)
#define PSBF(ax,ay,bx,by,ccx,ccy) (((long long)((bx)-(ax))*((ccy)-(ay)) - (long long)((by)-(ay))*((ccx)-(ax))) <= 0)
        for (int ti = 0; ti < mesh->triangle_count; ti++) {
            const re15_md1_triangle_t *t = &mesh->triangles[ti];
            if (t->v0 >= (uint32_t)mesh->tri_vertex_count || t->v1 >= (uint32_t)mesh->tri_vertex_count ||
                t->v2 >= (uint32_t)mesh->tri_vertex_count) continue;
            int sx0,sy0,sx1,sy1,sx2,sy2; float wz0,wz1,wz2;
            PSV(&mesh->tri_vertices[t->v0],sx0,sy0,wz0);
            PSV(&mesh->tri_vertices[t->v1],sx1,sy1,wz1);
            PSV(&mesh->tri_vertices[t->v2],sx2,sy2,wz2);
            if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
            if (PSBF(sx0,sy0,sx1,sy1,sx2,sy2)) continue;
            const re15_md1_tri_uv_t *uv = &mesh->triangle_uvs[ti];
            int pxo = (int)((uv->page & 0xF) * 128);
            int avgz = (int)((wz0 + wz1 + wz2) * (1.0f/3.0f));
            uint8_t r0,g0,b0,r1,g1,b1,r2,g2,b2;
            if (ncct_on && mesh->tri_normals && t->n0 < (uint32_t)mesh->tri_normal_count &&
                t->n1 < (uint32_t)mesh->tri_normal_count && t->n2 < (uint32_t)mesh->tri_normal_count) {
                const re15_md1_vertex_t *nn0=&mesh->tri_normals[t->n0], *nn1=&mesh->tri_normals[t->n1], *nn2=&mesh->tri_normals[t->n2];
                re15_light_shade_vertex(&lctx, nn0->x,nn0->y,nn0->z, &r0,&g0,&b0);
                re15_light_shade_vertex(&lctx, nn1->x,nn1->y,nn1->z, &r1,&g1,&b1);
                re15_light_shade_vertex(&lctx, nn2->x,nn2->y,nn2->z, &r2,&g2,&b2);
            } else { r0=r1=r2=C; g0=g1=g2=C; b0=b1=b2=C; }
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx1,sy1,(int)uv->u1+pxo,(int)uv->v1,
                sx2,sy2,(int)uv->u2+pxo,(int)uv->v2, 0,(int)uv->clut,avgz, r0,g0,b0,r1,g1,b1,r2,g2,b2);
        }
        for (int qi = 0; qi < mesh->quad_count; qi++) {
            const re15_md1_quad_t *q = &mesh->quads[qi];
            if (q->v0 >= (uint32_t)mesh->quad_vertex_count || q->v1 >= (uint32_t)mesh->quad_vertex_count ||
                q->v2 >= (uint32_t)mesh->quad_vertex_count || q->v3 >= (uint32_t)mesh->quad_vertex_count) continue;
            int sx0,sy0,sx1,sy1,sx2,sy2,sx3,sy3; float wz0,wz1,wz2,wz3;
            PSV(&mesh->quad_vertices[q->v0],sx0,sy0,wz0);
            PSV(&mesh->quad_vertices[q->v1],sx1,sy1,wz1);
            PSV(&mesh->quad_vertices[q->v2],sx2,sy2,wz2);
            PSV(&mesh->quad_vertices[q->v3],sx3,sy3,wz3);
            if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
            if (PSBF(sx0,sy0,sx1,sy1,sx2,sy2)) continue;
            const re15_md1_quad_uv_t *uv = &mesh->quad_uvs[qi];
            int pxo = (int)((uv->page & 0xF) * 128);
            int az1 = (int)((wz0+wz1+wz3)*(1.0f/3.0f)), az2 = (int)((wz0+wz3+wz2)*(1.0f/3.0f));
            uint8_t qr0,qg0,qb0,qr1,qg1,qb1,qr2,qg2,qb2,qr3,qg3,qb3;
            if (ncct_on && mesh->quad_normals && q->n0 < (uint32_t)mesh->quad_normal_count && q->n1 < (uint32_t)mesh->quad_normal_count &&
                q->n2 < (uint32_t)mesh->quad_normal_count && q->n3 < (uint32_t)mesh->quad_normal_count) {
                const re15_md1_vertex_t *m0=&mesh->quad_normals[q->n0], *m1=&mesh->quad_normals[q->n1],
                                        *m2=&mesh->quad_normals[q->n2], *m3=&mesh->quad_normals[q->n3];
                re15_light_shade_vertex(&lctx, m0->x,m0->y,m0->z, &qr0,&qg0,&qb0);
                re15_light_shade_vertex(&lctx, m1->x,m1->y,m1->z, &qr1,&qg1,&qb1);
                re15_light_shade_vertex(&lctx, m2->x,m2->y,m2->z, &qr2,&qg2,&qb2);
                re15_light_shade_vertex(&lctx, m3->x,m3->y,m3->z, &qr3,&qg3,&qb3);
            } else { qr0=qr1=qr2=qr3=C; qg0=qg1=qg2=qg3=C; qb0=qb1=qb2=qb3=C; }
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx1,sy1,(int)uv->u1+pxo,(int)uv->v1,
                sx3,sy3,(int)uv->u3+pxo,(int)uv->v3, 0,(int)uv->clut,az1, qr0,qg0,qb0,qr1,qg1,qb1,qr3,qg3,qb3);
            re15_render_textured_tri_lit(sx0,sy0,(int)uv->u0+pxo,(int)uv->v0, sx3,sy3,(int)uv->u3+pxo,(int)uv->v3,
                sx2,sy2,(int)uv->u2+pxo,(int)uv->v2, 0,(int)uv->clut,az2, qr0,qg0,qb0,qr3,qg3,qb3,qr2,qg2,qb2);
        }
#undef PSV
#undef PSBF
    }
}

/* PLAYER-SELECT scene — "PLEASE SELECT MAIN CAST" (byte-true TITLE.BIN task @0x80101094; full spec
 * in RE15_PLAYER_SELECT_DRAW.md). Runs after NEW-GAME confirm; returns the chosen character
 * (0=Leon, 1=Elza) = DAT_800aca5c>>2. This FOUNDATION does the byte-true state machine, input and
 * highlight crossfade over the SELECTH.TIM backdrop; the live PL00/PL01 3D models, the SELECTH3
 * name/profile SPRT overlays and the idle->zoom camera lerp are layered on in the next increments.
 *  - INPUT (sub1 @0x80101258): TOGGLE Leon<->Elza = D-pad L/R (mask 0xa000), GATED on the pulse
 *    scene+0x32e>=0x80 (16-frame debounce, @0x801012ac); CONFIRM = any face button or Start
 *    (mask 0x8f0, @0x80101268). Pad word @0x800ac762 is byte-swapped in the original; the port pad
 *    is already normalised so LEFT/RIGHT + face/START map 1:1.
 *  - PULSE (@0x8010080c): scene+0x32e init 0x80, +8/frame clamp 0x80, reset 0 on toggle. */
/* CONFIRM-ZOOM byte-true helpers (TITLE.BIN sub3-7). idiv40 = the exact trunc-toward-zero /40 used by
 * both the SPRT animator (@0x80100ac4) and the pan setup (magic 0x66666667, sra 4). */
static inline int32_t re15_idiv40(int32_t x)
{
    int32_t q = (int32_t)(((int64_t)x * (int64_t)0x66666667) >> 32);
    return (q >> 4) - (x >> 31);
}
/* Fade driver 0x8010026c (keyed on cz_c7): phase 0 → arm busy (c4=1); phase 1 → ramp c6 += 4 until it
 * wraps 0 after 64 ticks then latch 0xff; phase 2+ → release (c4=0). c6 = the sub4 fade level. */
static void re15_cz_fade_tick(int *c4, int *c6, int *c7)
{
    if (*c7 == 0)      { *c4 = 1; *c7 = 1; }
    else if (*c7 == 1) { *c6 = (*c6 + 4) & 0xff; if (*c6 == 0) { *c6 = 0xff; *c7 = 2; } }
    else               { *c4 = 0; }
}

static int pc_run_player_select(void)
{
    extern void re15_render_pc_player_select(const re15_tim_t *bg, int sel, int pulse);
    extern void re15_render_pc_pselect_dim(int level);
    extern void re15_render_pc_pselect_slide(int sel, int nx, int ny, int sx, int sy);
    extern void re15_render_pc_pselect_groupa(int on);
    extern void re15_render_pc_hide_player_select(void);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_set_title_fade(int a);
    extern void re15_render_pc_screenshot(const char *path);
    /* The menu confirm REPLACES the title task with this one (FUN_80029ba4 @0x80102c9c), so the
     * NEW GAME / LOAD GAME / OPTION menu + the title art STOP being drawn entirely. */
    re15_render_pc_hide_title_menu();
    re15_render_pc_hide_title();
    extern void re15_render_pc_pselect_text(const re15_tim_t *atlas, int sel);
    static re15_tim_t s_sel_bg = {0};
    if (!s_sel_bg.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/SELECTH.TIM", &sz);
                            if (b) re15_tim_parse(b, sz, &s_sel_bg); }   /* one-time; buffer kept */
    /* SELECTH3.TIM = the byte-true name/profile TEXT atlas (8bpp 256x256, colours baked in). */
    static re15_tim_t s_sel_txt = {0};
    if (!s_sel_txt.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/SELECTH3.TIM", &sz);
                             if (b) re15_tim_parse(b, sz, &s_sel_txt); }
    /* Load Leon(PL00) + Elza(PL08) once (TIM slots 20/21, free during the front-end).
     * BYTE-TRUE model ids (loader FUN_80101720 + CD dir @0x8006f43c): Leon = the resident PL00
     * (buffer 0x801bd814); Elza = CD file-id 0x44 whose dir-entry size (182928) is EXACTLY PL08.PLD.
     * (PL01 is Leon in a red POLICE costume — the wrong "different-costume Leon" I mis-loaded before.) */
    static pselect_model_t s_leon = {0}, s_elza = {0}; static int s_models_loaded = 0;
    if (!s_models_loaded) { s_models_loaded = 1;
        /* BYTE-TRUE anim source: the resident Leon in the player-select uses the WEAPON-variant
         * animation, NOT PL00.PLD's base EDD. Verified against the live savestate: the model's
         * anim table @0x801d7708 == PL00W03.EDD byte-for-byte (14 clips, clip2 = 52 frames ping-
         * ponging keyframes 38-51) and its skeleton @0x801d7bc0 == PL00W03.EMR. The base PL00.EDD
         * (24 clips, clip2 = keyframes 68+) is a DIFFERENT animation set → posed the wrong keyframe
         * (46 vs 105) → the folded/lying model. Body mesh + texture stay PL00 (PLW carries only the
         * weapon mesh). W03 = the handgun stance (Leon holds the pistol in the idle, as in the original). */
        /* WEAPON MESH = PL04W03.PLW dir[2] for BOTH characters (byte-true: the live savestate's Leon
         * part[11] weapon vertices match PL04W03 6/6, NOT PL00W03 — the player-select shares one weapon
         * buffer and PL04 is loaded last, so both models' hand+gun mesh is PL04W03's). Leon's body/anim
         * stays PL00/PL00W03; only the displayed weapon geometry is PL04W03. */
        pselect_load_model(&s_leon, "PLD/PL00.MD1", "PLD/PL00W03.EDD", "PLD/PL00.EMR", "PLD/PL00W03.EMR", "PLD/PL00.TIM", 20, "PLD/PL04W03.PLW", 22);
        /* BYTE-TRUE right character = ELZA = PL04 (blonde, red "RACCOON" biker suit), NOT PL08/Jill.
         * The live savestate's right model (g_entity 0x800ace20) reads: anim EDD @0x801460f4 ==
         * PL04W03.EDD and skeleton EMR @0x80146688 == PL04W03.EMR (both byte-verified). The MZD disc's
         * player-select CD id 0x44 loads PL04, not the prototype's PL08 — that is why my earlier
         * "right = PL08" (proto-disc directory) rendered Jill. Elza also holds the W03 handgun idle. */
        pselect_load_model(&s_elza, "PLD/PL04.MD1", "PLD/PL04W03.EDD", "PLD/PL04.EMR", "PLD/PL04W03.EMR", "PLD/PL04.TIM", 21, "PLD/PL04W03.PLW", 23); }
    /* Byte-true camera (wf_0aab308c): global view R=diag(4104,4096,4104), TR=(0,0,20039), H=1000. */
    re15_camera_view_t cam = {0};
    cam.rot[0] = 4104; cam.rot[4] = 4096; cam.rot[8] = 4104;
    cam.trans[0] = 0; cam.trans[1] = 0; cam.trans[2] = 20039;
    cam.fov_screen_dist = 1000;
    int sel = 0;        /* scene+0x394: Leon default (@0x801011f8) */
    int pulse = 0x80;   /* scene+0x32e: init 0x80 (@0x80100548) -> first toggle is instant */
    /* FADE-IN: sub0 kicks fade ch0 abr2-subtractive step -0x400 with level 0x7fff (FUN_800217b0
     * @0x8010121c + FUN_800216ec @0x80101240) -> brightness (level>>7) ramps 0xff->0 over 32 frames. */
    int fade_level = 0x7fff;   /* brightness = fade_level>>7 */
    /* CONFIRM-ZOOM FSM (TITLE.BIN LEVEL-2 scene+0x392, sub3-7). cz = -1 idle, 0..5 the FSM state.
     * sub3(1f init) -> sub4(65f 2D-layer fade via the c4/c6/c7 driver) -> sub5(1f pan/dolly setup) ->
     * sub6(41f: pan selected char world-X->0, spin rot_y+=8, camera dolly pos.z += quadratic accum,
     * rebuild LookAt) -> sub7(60f hold + 32f fade-to-black) -> handoff. */
    int cz = -1;                          /* -1 = idle input phase; 0..5 = confirm LEVEL-2 state */
    int cz_c4 = 0, cz_c6 = 0, cz_c7 = 0;  /* fade driver 0x8010026c (busy / ramp level / phase) */
    int cz_396 = 0;                       /* shared counter scene+0x396 */
    int32_t cz_panvel = 0, cz_accel = 0, cz_accum = 0;  /* model pan vel (scene+0), accel04 (+4), cam-z accum (+0x74) */
    int32_t pan_x[2]  = { -1568, 1568 };  /* selected char world-X (field+0x34); pans toward 0 */
    int32_t pan_ry[2] = { 0x404, 0x404 }; /* selected char rot_y (field+0x6a); += 8/frame */
    int32_t blk_x[4], blk_y[4], blk_dx[4] = {0}, blk_dy[4] = {0};  /* SPRT name/sub blocks, 16.16 */
    int elza_alive = 1;                   /* the unselected char is disabled at the sub4->sub5 edge */
    int cz_black = 0;                      /* sub7 fade-to-black brightness (0..255, +8/frame) */
    /* seed SPRT blocks from the byte-true idle positions (@0x80102624): blk0/2 = names, blk1/3 = subs */
    { static const int bx[4] = {16,24,168,176}, by[4] = {48,216,48,216};
      for (int i=0;i<4;i++){ blk_x[i]=bx[i]<<16; blk_y[i]=by[i]<<16; } }
    static const int32_t cz_target[4][2] = { {11<<16,20<<16},{176<<16,208<<16},{11<<16,20<<16},{176<<16,208<<16} };
    unsigned frames = 0;
    const char *ps_shot = getenv("RE15_PSELECT_SHOT");
    int auto_drive = getenv("RE15_PSELECT_AUTO") != NULL;
    /* 30fps cap: the idle clip 2 advances +0x95 by 1 PER FRAME (FUN_8001f314 POST-inc), so the
     * scene MUST run at the PSX 30Hz cadence or the animation plays far too fast (the game loop
     * caps the same way). Without this the models twitch / cycle wrong. */
    uint32_t ps_last = SDL_GetTicks();
    while (re15_gameflow_mode() == RE15_MODE_TITLE) {
        if (pulse < 0x80) { pulse += 8; if (pulse > 0x80) pulse = 0x80; }
        /* ---- CONFIRM-ZOOM FSM step (byte-true LEVEL-2 sub3-7; runs BEFORE the render like the original
         * dispatch order sub2 -> renderers). Advances the state + counters + pan/dolly/slide. ---- */
        if (cz == 0) {                               /* sub3 START-ZOOM (once): arm the fade driver */
            cz_c6 = 0; re15_cz_fade_tick(&cz_c4, &cz_c6, &cz_c7); cz = 1;
        } else if (cz == 1) {                        /* sub4 ZOOM-LOOP: fade the 2D layer over 64f */
            re15_cz_fade_tick(&cz_c4, &cz_c6, &cz_c7);
            if (cz_c4 == 0) { elza_alive = 0; cz_396 = 0; cz = 2; }   /* fade done: disable other char, arm pan/anim */
        } else if (cz == 2) {                        /* sub5 PAN-SETUP + animator SETUP (once) */
            cz_panvel = re15_idiv40(pan_x[sel]); cz_accel = 0x14; cz_accum = 0; cz_396 = 40;
            { int b0 = sel ? 2 : 0; for (int k = 0; k < 2; k++) { int b = b0 + k;
                blk_dx[b] = re15_idiv40(cz_target[b][0] - blk_x[b]);
                blk_dy[b] = re15_idiv40(cz_target[b][1] - blk_y[b]); } }
            cz = 3;
        } else if (cz == 3) {                        /* sub6 PAN (41f): model to centre + spin + camera dolly + slide */
            pan_x[sel] -= cz_panvel; pan_ry[sel] += 8;
            cz_accum += cz_accel; cz_accel += 0xA;
            { int b0 = sel ? 2 : 0; for (int k = 0; k < 2; k++) { int b = b0 + k; blk_x[b] += blk_dx[b]; blk_y[b] += blk_dy[b]; } }
            { int c = cz_396; cz_396--; if (c == 0) { cz_396 = 60; cz = 4; } }
        } else if (cz == 4) {                        /* sub7 FADE: 60f hold, then fade-to-black */
            int c = cz_396; cz_396--; if (c == 0) cz = 5;
        } else if (cz == 5) {                        /* handoff: ramp the fade-to-black (32f @ +8/frame) */
            cz_black += 8; if (cz_black > 255) cz_black = 255;
        }
        /* camera DOLLY (sub6+): pos.z = -20000 + accum; view (LookAt) keeps R constant, TR.z = R[8]*(-pos.z)/4096
         * (FUN_80053ca4). accum grows quadratically -> ease-in zoom 20039 -> ~11000 (~1.82x). */
        if (cz >= 3) cam.trans[2] = (int32_t)(((int64_t)4104 * (20000 - cz_accum)) >> 12);
        int cz_dim = (cz == 1) ? cz_c6 : (cz >= 2 ? 255 : 0);   /* sub4 ramp; then backdrop fully black */

        re15_render_begin_frame();
        re15_input_tick();
        re15_render_background_gradient(0, 0, 0, 0, 0, 0);
        re15_render_pc_player_select(&s_sel_bg, sel, pulse);
        re15_render_pc_pselect_text(&s_sel_txt, sel);   /* name/profile text overlays (groups B+C) */
        if (cz_dim) re15_render_pc_pselect_dim(cz_dim);
        if (cz >= 2) re15_render_pc_pselect_groupa(0);   /* stop the half-screen dim once the char pans to centre */
        if (cz >= 3) { int b0 = sel ? 2 : 0;             /* sub6: the sliding name+subtitle rows */
            re15_render_pc_pselect_slide(sel, blk_x[b0] >> 16, blk_y[b0] >> 16, blk_x[b0+1] >> 16, blk_y[b0+1] >> 16); }
        /* the live 3D models: the SELECTED char pans (pan_x/pan_ry); the unselected is drawn until sub4 disables it. */
        { const char *cv=getenv("RE15_PSELECT_CUR"); uint32_t acur = cv ? (uint32_t)atoi(cv) : frames;
          if (sel == 0 || elza_alive) pselect_render_model(&s_leon, pan_x[0], 2036, 0, (int16_t)pan_ry[0], acur, &cam);
          if (sel == 1 || elza_alive) pselect_render_model(&s_elza, pan_x[1], 2036, 0, (int16_t)pan_ry[1], acur, &cam); }
        { int br = fade_level >> 7; if (br > 255) br = 255; if (br < 0) br = 0;
          if (cz_black > br) br = cz_black;   /* sub7 fade-to-black wins over the initial fade-in */
          re15_render_pc_set_title_fade(br); }
        re15_render_end_frame();
        { const char *af = getenv("RE15_PSELECT_SHOT_AF"); unsigned sf = af ? (unsigned)atoi(af) : 40;
          if (ps_shot && frames == sf) re15_render_pc_screenshot(ps_shot); }
        { const char *ser = getenv("RE15_PSELECT_SERIES");   /* frame-series dump for the video compare */
          if (ser) { unsigned lim = getenv("RE15_PSELECT_SERIES_N") ? (unsigned)atoi(getenv("RE15_PSELECT_SERIES_N")) : 60;
                     if (frames < lim) { char pb[512]; snprintf(pb, sizeof pb, "%s/f%03u.png", ser, frames);
                                         re15_render_pc_screenshot(pb); } else break; } }

        if (cz < 0) { fade_level -= 0x400; if (fade_level < 0) fade_level = 0; }   /* fade-in (idle only) */
        if (cz == 5 && cz_black >= 255) break;   /* confirm fade-to-black complete -> hand off to the game */

        uint16_t pp = g_engine.pad_pressed;
        if (auto_drive) { if (frames == 40 && getenv("RE15_PSELECT_AUTO_SWITCH")) pp |= RE15_PAD_BIT_RIGHT;  /* -> Elza */
                          if (frames == 70) pp |= RE15_PAD_BIT_CROSS; }    /* confirm (default = Leon) */
        if (cz < 0 && fade_level == 0) {   /* input active once faded in (sub1 idle) */
            if ((pp & (RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) && pulse >= 0x80) { sel ^= 1; pulse = 0; }
            if (pp & (RE15_PAD_BIT_CROSS | RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_TRIANGLE |
                      RE15_PAD_BIT_CIRCLE | RE15_PAD_BIT_START))
                cz = 0;   /* CONFIRM -> start the confirm-zoom FSM (sub3) */
        }
        frames++;
        /* 30fps cap (byte-true PSX 30Hz cadence) so the idle clip advances 1 frame/tick, not
         * uncapped -> the models animated far too fast without this (game loop caps @4431). */
        { uint32_t now = SDL_GetTicks(); uint32_t el = now - ps_last;
          if (el < 33) SDL_Delay(33 - el); ps_last = SDL_GetTicks(); }
    }
    re15_render_pc_set_title_fade(0);
    re15_render_pc_hide_player_select();
    return sel;
}

/* OPTIONS / controller-CONFIG screen (byte-true EXE task @0x8002dde4, RE workflow wf_ff4bebb2 +
 * RE_15_Quellcode_V2/FUN_8002dfb0). Entered from the title menu OPTION item (TITLE.BIN FUN_801025f0 →
 * FUN_80029a98(1,0x8002dde4)). This FOUNDATION renders the byte-true C_BACK2.TIM backdrop (the OPTIONS
 * title, the CONFIG/SOUND/EXIT tabs, the DualShock graphic + the label boxes are baked in), the
 * fade-in/out, and the tab cursor; the per-button action LABELS (game-font strings drawn by the
 * printf-style FUN_800279c8 at the DAT_80073d2c slot positions) and the functional A/B/C/EDIT remap
 * (tables @0x80073dbc/ddc/dfc verified from RAM) are the next increments. Returns when EXIT/Cancel. */
/* Controller-config preset (OPTIONS TYPE A/B/C), byte-true (workflow wf_df9c864a + FUN_80030444, tables
 * @0x80073dbc/ddc/dfc). The runtime remap BUILDS a 16-bit logical action word from the raw pad
 * (logical bit i set iff REMAP[type][i] & raw), which the game reads. The port's game readers already
 * encode TYPE A's physical layout, so we build the logical word then re-express it into TYPE A's
 * physical bits (out |= REMAP[A][i] for each set logical bit), keeping the non-config buttons
 * (dialog Circle, menu Start, …) as pass-through. TYPE A is therefore IDENTITY (no regression).
 *   A vs B: bits 6,7,14 (ACTION/ATTACK) SQUARE→CIRCLE — the attack button moves to Circle (NOT a turn swap).
 *   A vs C: + tank-turn onto the shoulders (i1 Right→R1, i3 Left→L1) + Aim/Ready R1→R2 (i8).
 * Tables are in the PORT's SIO/RE15_PAD_BIT order (the EXE's PsyQ-order hex is byte-swapped). INGAME only. */
static int s_config_type = 0;
static const uint16_t k_pad_remap[3][16] = {
    /* A */ { 0x0010,0x0020,0x0040,0x0080, 0x0010,0x0040, 0x8000,0x8000, 0x0800,0x4000,0x0800,0x0400, 0x0080,0x0020,0x8000,0x4000 },
    /* B */ { 0x0010,0x0020,0x0040,0x0080, 0x0010,0x0040, 0x2000,0x2000, 0x0800,0x4000,0x0800,0x0400, 0x0080,0x0020,0x2000,0x4000 },
    /* C */ { 0x0010,0x0800,0x0040,0x0400, 0x0010,0x0040, 0x2000,0x2000, 0x0200,0x4000,0x0800,0x0400, 0x0080,0x0020,0x2000,0x4000 },
};
static void pc_edit_build_remap(uint16_t remap[16]);   /* fwd — type 3 (EDIT) builds its remap from d08 */
static uint16_t pc_pad_config(uint16_t p)
{
    if (s_config_type <= 0) return p;   /* TYPE A = identity */
    const uint16_t *TA = k_pad_remap[0];
    const uint16_t *TT;
    uint16_t edit_remap[16];
    if (s_config_type <= 2) TT = k_pad_remap[s_config_type];
    else { pc_edit_build_remap(edit_remap); TT = edit_remap; }     /* TYPE 3 = EDIT custom map */
    uint16_t cfg = 0; for (int i = 0; i < 16; i++) cfg |= TA[i];   /* buttons the config owns */
    uint16_t out = (uint16_t)(p & ~cfg);                          /* non-config buttons pass through */
    for (int i = 0; i < 16; i++) if (TT[i] & p) out |= TA[i];     /* fire TYPE-A physical bit i when TYPE-T's button is held */
    return out;
}

/* ASCII -> TEX.TIM game-font glyph code (decoded from the font grid, shots/texfont_grid.png):
 * A-Z @0x1D, a-z @0x3D, 0-9 @0x0C (contiguous; 0x0b=down-arrow), space=0, + config-label punctuation. */
static int pc_font_code(char c)
{
    if (c >= 'A' && c <= 'Z') return 0x1D + (c - 'A');
    if (c >= 'a' && c <= 'z') return 0x3D + (c - 'a');
    if (c >= '0' && c <= '9') return 0x0C + (c - '0');
    switch (c) { case ' ': return 0x00; case '.': return 0x57; case ',': return 0x18;
                 case '/': return 0x38; case '(': return 0x37; case ')': return 0x39;
                 case '\'': return 0x3A; case '-': return 0x3B; case ':': return 0x16;
                 case ';': return 0x17; case '!': return 0x1A; case '?': return 0x1B; }
    return 0x00;
}

/* Live per-box action-code array (byte-true DAT_80073d08): the active preset A/B/C (copied in by the
 * picker) or the EDIT custom map. pc_config_draw_labels + the ingame remap read this. */
static unsigned char s_config_d08[16] = {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x04,0x07,0x05,0x0e,0x0f,0x15,0x17};
/* The 3 preset code tables (byte-true DAT_80073cd8, copied into d08 on TYPE A/B/C confirm). */
static const unsigned char s_config_preset[3][16] = {
    {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x04,0x07,0x05,0x0e,0x0f,0x15,0x17},  /* A */
    {0x07,0x07,0x00,0x03,0x02,0x01,0x07,0x06,0x07,0x07,0x04,0x05,0x0e,0x0f,0x16,0x17},  /* B */
    {0x07,0x03,0x00,0x07,0x07,0x01,0x06,0x02,0x07,0x07,0x04,0x05,0x0e,0x0f,0x16,0x17},  /* C */
};
/* SOUND setting: 1 = stereo, 0 = mono (byte-true scene S[7] / persisted DAT_800b0fce; default stereo). */
static int s_config_sound = 1;

/* the 16 grid-slot / action-label box positions (DAT_80073d2c) — shared by the labels + the EDIT slot cursor. */
static const struct { int x, y; } k_cfg_box[16] = {
    {31,52},{31,71},{46,91},{9,111},{84,111},{46,131},{217,52},{217,71},
    {203,91},{164,111},{239,111},{203,131},{137,182},{137,200},{281,182},{281,200} };

/* CONFIG action labels — byte-true (grid FUN_8002ffb8). Per box i, draw the string/glyph for
 * id = DAT_80073d08[i] (s_config_d08) at box position DAT_80073d2c[i] (+ per-id x-nudge, +1 y), rendered
 * with the game font (FUN_80028ec4) using CLUT 0x7810 (normal, attr 0) except id 7 = "Not set" which
 * uses CLUT 0x7990 (dim, attr 3). id<0x12 = CONFIG.TIM icon (msg-font approximation); id>=0x12 = font
 * string. Plus 4 fixed labels (Upper/Lower Attack, Confirm, Cancel). */
static void pc_config_draw_labels(void)
{
    extern int re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    static const char *STR[0x0c] = {
        "Forward", "Backward", "L. Turn", "R. Turn", "OK/Attack", "Run", "Aim", "Not set",
        "Upper Attack", "Lower Attack", "Confirm", "Cancel" };
    /* id 0x12-0x19 = button-legend symbols. The EXE draws them via FUN_800279c8's own symbol atlas;
     * mapped here to the TEX.TIM msg-font button glyphs (△0x06 ○0x07 ✕0x08 □0x09). The confirm/cancel
     * legend (ids 0x15/0x16 = □, 0x17 = ✕) is what actually shows for TYPE A/B/C. */
    static const unsigned char SPR[8] = { 0x06,0x07,0x06,0x09,0x09,0x08,0x07,0x08 };   /* id 0x12..0x19 */
    static const int NUDGE[0x12] = { 0x16,0x16,0x0f,0x0f,0x02,0x09,0x0f,0x0f, 0,0,0,0,0,0,0,0,0,0 };
    unsigned char buf[24];
    for (int i = 0; i < 16; i++) {
        int id = s_config_d08[i];
        int x = k_cfg_box[i].x + (id < 0x12 ? NUDGE[id] : 0), y = k_cfg_box[i].y + 1, n = 0;
        int attr = (id == 7) ? 3 : 0;   /* "Not set" -> dim CLUT 0x7990 (attr 3), else normal 0x7810 (attr 0) */
        if (id < 0x0c)       { for (const char *p = STR[id]; *p && n < 24; p++) buf[n++] = (unsigned char)pc_font_code(*p); }
        else if (id < 0x12)  { buf[n++] = (unsigned char)(0x98 - (id - 0x0e)); }   /* 0x0e→0x98 .. 0x11→0x95 */
        else if (id <= 0x19) { buf[n++] = SPR[id - 0x12]; }
        if (n) re15_render_pc_config_text(x, y, buf, n, attr);
    }
    static const struct { int x, y, id; } FIX[4] = { {30,183,8},{30,201,9},{174,183,10},{174,201,11} };
    for (int k = 0; k < 4; k++) {
        int n = 0; for (const char *p = STR[FIX[k].id]; *p && n < 24; p++) buf[n++] = (unsigned char)pc_font_code(*p);
        re15_render_pc_config_text(FIX[k].x, FIX[k].y, buf, n, 0);
    }
}

/* OPTIONS screen state machine — byte-true (EXE task @0x8002dde4). Screens:
 *   TOP    : CONFIG(0)/SOUND(1)/EXIT(2)            LEFT/RIGHT wrap 0..2      (FUN_8002e774)
 *   PICKER : TYPE A(0)/B(1)/C(2)/EDIT(3)/EXIT(4)   LEFT/RIGHT wrap 0..4      (FUN_8002e980)
 *   SOUND  : Stereo/Mono                           UP|DOWN toggles           (FUN_8002f0d4)
 *   EDIT   : 16-slot grid + panel A/B                                        (FUN_8002ebbc)
 * confirm = Square|Circle (raw 0xa0), cancel = Cross (raw 0x40) — RE1.5 config convention. Positions:
 * DAT_80073d6c (top) / DAT_80073d78 (picker), y=24; blue cursor TILE RGB(0,0,0x80) @50% ABE, 48x16
 * (top/EDIT/EXIT) or 16x16 (TYPE A/B/C). */
enum { CFG_TOP = 0, CFG_PICKER, CFG_SOUND, CFG_EDIT };
static const int k_cfg_top_x[3]  = { 147, 198, 248 };            /* DAT_80073d6c.x (y=24) */
static const int k_cfg_pick_x[5] = { 147, 164, 181, 198, 248 }; /* DAT_80073d78.x (y=24) */

/* EDIT custom-remap screen (FUN_8002ebbc). s_edit_phase: 0=slot-select, 1=panel A (basic action), 2=panel B
 * (special code). Slot-grid nav = byte-true jump-table transitions (0x80010a30/a70/aa8/ae8); panels + assign
 * dedup + remap rebuild all byte-true (workflow wf_abeec930). Panel GLYPHS are the game-font approximation
 * (action names / button symbols) — the exact CONFIG.TIM streams need a dynamic ligature-table dump. */
static int s_edit_phase = 0, s_edit_slot = 0, s_edit_code = 0;   /* scene S[3]/S[5]/S[6] */
static const uint8_t k_edit_up[16]    = {13,0,1,2,2,3,15,6,7,8,8,9,5,12,11,14};
static const uint8_t k_edit_down[16]  = {1,2,3,5,5,12,7,8,9,11,11,14,13,0,15,6};
static const uint8_t k_edit_right[16] = {6,7,8,10,3,11,0,1,2,4,9,5,14,15,12,13};
static const uint8_t k_edit_left[16]  = {6,7,8,4,9,11,0,1,2,10,3,5,14,15,12,13};
static const uint8_t k_editB_up[12]   = {5,0,1,2,3,4,11,6,7,8,9,10};
static const uint8_t k_editB_down[12] = {1,2,3,4,5,0,7,8,9,10,11,6};
static const uint8_t k_editB_lr[12]   = {6,7,8,9,10,11,0,1,2,3,4,5};
/* per-grid-slot physical mask (DAT_80073c88) + special-code masks (0x80073c84 + code*2, code 14..25),
 * both in the EXE's standard PSX-SIO order; converted to the port's RE15_PAD_BIT order at remap-build. */
static const uint16_t k_box_mask[16]     = {0x0001,0x0004,0x1000,0x8000,0x2000,0x4000,0x0002,0x0008,0x0010,0x0080,0x0020,0x0040,0x1000,0x4000,0x2000,0x8000};
static const uint16_t k_special_mask[12] = {0x1000,0x4000,0x2000,0x8000,0x0004,0x0001,0x0010,0x0080,0x0020,0x0040,0x0008,0x0002}; /* code 14..25 */

/* EDIT assign — panel A (basic codes 0..7, swap-dedup over grid slots 0..11; code 7 = "Not set", no
 * dedup / duplicates allowed). Byte-true FUN_8002f714. */
static void pc_edit_assign_a(void)
{
    int newcode = s_edit_code;                          /* 0..7 */
    if (newcode == 7) { s_config_d08[s_edit_slot] = 7; return; }
    int old = s_config_d08[s_edit_slot];
    for (int i = 11; i >= 0; --i) if (s_config_d08[i] == newcode) s_config_d08[i] = (unsigned char)old;
    s_config_d08[s_edit_slot] = (unsigned char)newcode;
}
/* EDIT assign — panel B (special code = cursor+14, swap-dedup over slots 12..15, first match). FUN_8002f7e8. */
static void pc_edit_assign_b(void)
{
    int newcode = s_edit_code + 14;                     /* 14..25 */
    int old = s_config_d08[s_edit_slot];
    for (int i = 12; i < 16; ++i) if (s_config_d08[i] == newcode) { s_config_d08[i] = (unsigned char)old; break; }
    s_config_d08[s_edit_slot] = (unsigned char)newcode;
}
/* Build the EDIT custom remap[16] (logical action -> physical port-pad mask) from s_config_d08, byte-true
 * rebuild loop @0x8002e5d0 (code->action LUT 0x80010a10). The box/special masks are the EXE's standard
 * PSX-SIO bits; byte-swapped to the port's RE15_PAD_BIT order (matches k_pad_remap exactly for A). The
 * fixed actions {10..13} are seeded from TYPE A (the byte-true rebuild leaves them 0 — they are engine
 * defaults not exposed by the editor). */
static void pc_edit_build_remap(uint16_t remap[16])
{
    for (int i = 0; i < 16; i++) remap[i] = k_pad_remap[0][i];   /* seed non-editable actions from TYPE A */
    for (int slot = 0; slot < 16; slot++) {
        int c = s_config_d08[slot];
        if (c >= 7) continue;
        uint16_t m = (uint16_t)(((k_box_mask[slot] & 0xFF) << 8) | (k_box_mask[slot] >> 8));
        switch (c) {
            case 0: remap[0] = m; break;
            case 1: remap[2] = m; break;
            case 2: remap[1] = m; break;
            case 3: remap[3] = m; break;
            case 4: remap[7] = m; remap[6] = m; break;
            case 5: remap[9] = m; break;
            case 6: remap[8] = m; break;
        }
    }
    static const int spdst[4] = { 4, 5, 14, 15 };
    for (int k = 0; k < 4; k++) {
        int code = s_config_d08[12 + k];
        if (code >= 14 && code <= 25) {
            uint16_t sm = k_special_mask[code - 14];
            remap[spdst[k]] = (uint16_t)(((sm & 0xFF) << 8) | (sm >> 8));
        }
    }
}

/* Draw the current sub-screen's CONFIG.TIM tiles + selected-item highlight + text over the backdrop
 * (called after re15_render_pc_config_clear + the backdrop + the 16 action labels). */
static void pc_config_draw_overlay(const re15_tim_t *tim, int screen, int cur)
{
    extern void re15_render_pc_config_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern void re15_render_pc_config_tile_ov(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern void re15_render_pc_config_rect_ov(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern int  re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    if (screen == CFG_TOP) {
        re15_render_pc_config_rect(k_cfg_top_x[cur], 24, 48, 16, 0, 0, 0x80, 128);   /* blue tab cursor @50% */
    } else if (screen == CFG_PICKER) {
        /* opaque CONFIG.TIM tiles cover the baked CONFIG/SOUND: "TYPE A B C" then "EDIT" ("EXIT" stays baked). */
        re15_render_pc_config_tile(tim, 0, 192, 50, 24, 147, 16);
        re15_render_pc_config_tile(tim, 0, 216, 48, 16, 198, 24);
        re15_render_pc_config_rect(k_cfg_pick_x[cur], 24, (cur < 3) ? 16 : 48, 16, 0, 0, 0x80, 128);
    } else if (screen == CFG_SOUND) {
        /* SOUND label boxes (CONFIG.TIM uv 0,232 88x18) + STEREO/MONO text; current setting -> dim CLUT
         * 0x7990 (attr 3), other -> normal 0x7810 (attr 0). Tab cursor stays on SOUND (198). */
        re15_render_pc_config_tile(tim, 0, 232, 88, 18, 116, 51);
        re15_render_pc_config_tile(tim, 0, 232, 88, 18, 116, 70);
        re15_render_pc_config_rect(k_cfg_top_x[1], 24, 48, 16, 0, 0, 0x80, 128);
        /* Current setting -> BRIGHT (attr 0 / CLUT 0x7810), the other -> DIM (attr 3 / 0x7990). Verified
         * against the PSX SOUND screen (STEREO current = bright 177, MONO = dim 132). */
        { unsigned char b[8]; int n = 0; const char *s = "STEREO"; for (const char *p = s; *p; p++) b[n++] = (unsigned char)pc_font_code(*p);
          re15_render_pc_config_text(119, 53, b, n, s_config_sound ? 0 : 3); }
        { unsigned char b[8]; int n = 0; const char *s = "MONO";   for (const char *p = s; *p; p++) b[n++] = (unsigned char)pc_font_code(*p);
          re15_render_pc_config_text(131, 71, b, n, s_config_sound ? 3 : 0); }
    } else if (screen == CFG_EDIT) {
        /* EDIT is a sub-state of the KEY/picker screen, so the picker bar stays visible with EDIT
         * highlighted (the "TYPE A B C" + "EDIT" tiles over the baked tabs, blue cursor on EDIT). */
        re15_render_pc_config_tile(tim, 0, 192, 50, 24, 147, 16);
        re15_render_pc_config_tile(tim, 0, 216, 48, 16, 198, 24);
        re15_render_pc_config_rect(k_cfg_pick_x[3], 24, 48, 16, 0, 0, 0x80, 128);
        /* slot-select YELLOW cursor (0xc0c000 @50%) at the selected grid box: w=72 for the 12 basic slots,
         * 16 for the 4 special slots (FUN_8002f518). */
        re15_render_pc_config_rect(k_cfg_box[s_edit_slot].x, k_cfg_box[s_edit_slot].y,
                                   (s_edit_slot < 12) ? 72 : 16, 16, 0xc0, 0xc0, 0x00, 128);
        if (s_edit_phase == 1) {
            /* Panel A = the "ACT" box (CONFIG.TIM uv 0,48) with 8 empty rows + a BLUE row cursor (this
             * prototype does not render row glyphs — the action is the row position). The panel sits on the
             * side opposite the selected box: right (s2=187) for slots 0..5, left (37) for 6..11. Byte-true
             * cursor = flat blue TILE 72x16 @ (s2+12, code*16+54). */
            int s2 = (s_edit_slot < 6) ? 0xbb : 0x25;
            /* ACT box = 2 CONFIG.TIM tiles (byte-true, scene prim scan mzd_edit_panelA.sav): top uv(0,48)
             * 96x48 @(s2,35) = "ACT" header + first rows; bottom uv(0,80) 96x112 @(s2,83) = lower rows. */
            re15_render_pc_config_tile_ov(tim, 0, 48, 96, 48, s2, 35);
            re15_render_pc_config_tile_ov(tim, 0, 80, 96, 112, s2, 83);
            /* row cursor = the BRIGHT-blue TILE RGB(0,0,0xc0) @50% (SETUP tile 3), 72x16 @ (s2+12,code*16+54). */
            re15_render_pc_config_rect_ov(s2 + 12, s_edit_code * 16 + 54, 72, 16, 0, 0, 0xc0, 128);
        } else if (s_edit_phase == 2) {
            /* Panel B — the "KEY" box (CONFIG.TIM uv 88,0) + 8 assignable special-button glyphs at the
             * byte-true screen positions (prim-buffer scan of FUN_8002fb94's SPRTs: all clut 0x7810, 16x16,
             * V=32, U=48..160 = msg-font glyph codes 3..10 = L2/R2/L1/R1/△/○/✕/□). + BLUE cursor. */
            static const struct { int x, y; unsigned char g; } BICON[8] = {
                {216, 55, 7}, {216, 71,10}, {216, 87, 8}, {216,103, 9},   /* △ □ ○ ✕ */
                {216,119, 6}, {216,135, 4},                                 /* R1 R2 */
                {200,119, 5}, {200,135, 3} };                               /* L1 L2 */
            /* KEY box = 2 CONFIG.TIM SPRTs (byte-true, scene prim scan @0x80190148): left half
             * uv(96,0) 32x128 @(187,35) = "KE" + left cell column; right half uv(96,128) 24x128 @(219,35)
             * = "Y" + right cell column. */
            re15_render_pc_config_tile_ov(tim, 96,   0, 32, 128, 187, 35);
            re15_render_pc_config_tile_ov(tim, 96, 128, 24, 128, 219, 35);
            for (int i = 0; i < 8; i++)
                re15_render_pc_config_text(BICON[i].x, BICON[i].y, &BICON[i].g, 1, 0);
            /* Byte-true extra glyphs from the overlay prim buffer (mzd_edit_panelB.sav, 0x800c7000 code-0x7c
             * SPRTs, uv->msg-font code): the "R1 O8!?KQW" debug string overlaid on the two "Not set" boxes
             * (rows y55/y87) + the header letters D@(200,87) / E@(200,119). Glyphs: R1=0x06, O=0x0c, 8=0x14,
             * !?=0x1c, K=0x27, Q=0x2d, W=0x33, D=0x20, E=0x21. */
            static const struct { short x, y; unsigned char g; } PBX[] = {
                {219,55,0x06},{232,55,0x0c},{244,55,0x14},{256,55,0x1c},{268,55,0x27},{280,55,0x2d},{292,55,0x33},{306,55,0x3b},{317,55,0x44},{329,55,0x4d},
                {200,87,0x20},{219,87,0x06},{232,87,0x0c},{244,87,0x14},{256,87,0x1c},{268,87,0x27},{280,87,0x2d},{292,87,0x33},{306,87,0x3b},{317,87,0x44},{329,87,0x4d},
                {200,119,0x21} };
            for (int i = 0; i < (int)(sizeof(PBX)/sizeof(PBX[0])); i++)
                re15_render_pc_config_text(PBX[i].x, PBX[i].y, &PBX[i].g, 1, 0);
            /* cell cursor = BRIGHT-blue TILE RGB(0,0,0xc0) @50% (SETUP tile 3), 16x16 @ (199/215, code*16+54). */
            int bx = (s_edit_code < 6) ? 199 : 215, by = ((s_edit_code < 6) ? s_edit_code : (s_edit_code - 6)) * 16 + 54;
            re15_render_pc_config_rect_ov(bx, by, 16, 16, 0, 0, 0xc0, 128);
        }
    }
}

static void pc_run_config(void)
{
    extern void re15_render_pc_config(const re15_tim_t *bg, int tab);
    extern void re15_render_pc_hide_config(void);
    extern void re15_render_pc_hide_title(void);
    extern void re15_render_pc_hide_title_menu(void);
    extern void re15_render_pc_set_title_fade(int a);
    extern void re15_render_pc_config_clear(void);
    extern void re15_render_pc_config_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    extern void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
    extern int  re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
    extern void re15_audio_set_mono(int mono);
    re15_render_pc_hide_title_menu();
    re15_render_pc_hide_title();
    static re15_tim_t s_cfg_bg = {0};      /* C_BACK2.TIM backdrop (OPTIONS title + tabs + controller) */
    if (!s_cfg_bg.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/C_BACK2.TIM", &sz);
                            if (b) re15_tim_parse(b, sz, &s_cfg_bg); }
    static re15_tim_t s_cfg_tim = {0};     /* CONFIG.TIM 8bpp icon/tile sheet (picker + SOUND tiles) */
    if (!s_cfg_tim.pixels) { int sz = 0; uint8_t *b = pc_read_shared("DATA/CONFIG.TIM", &sz);
                             if (b) re15_tim_parse(b, sz, &s_cfg_tim); }

    /* keep the live label array in sync with the active preset A/B/C on entry (EDIT keeps its custom map). */
    if (s_config_type >= 0 && s_config_type <= 2) memcpy(s_config_d08, s_config_preset[s_config_type], 16);

    int screen = CFG_TOP, cur = 0;
    int fade_level = 0x7fff;   /* brightness = fade_level>>7; fade IN from black */
    int fading_out = 0;
    uint32_t last = SDL_GetTicks();
    while (re15_gameflow_mode() == RE15_MODE_TITLE) {
        re15_render_begin_frame();
        re15_input_tick();
        re15_render_background_gradient(0, 0, 0, 0, 0, 0);
        re15_render_pc_config_clear();
        re15_render_pc_config(&s_cfg_bg, 0);   /* backdrop */
        pc_config_draw_labels();               /* 16 action labels (+ "Not set" dim) */
        pc_config_draw_overlay(&s_cfg_tim, screen, cur);

        { int br = fade_level >> 7; re15_render_pc_set_title_fade(br > 255 ? 255 : (br < 0 ? 0 : br)); }
        re15_render_end_frame();

        if (!fading_out) { fade_level -= 0x400; if (fade_level < 0) fade_level = 0; }
        else { fade_level += 0x400; if (fade_level >= 0x7fff) break; }

        if (!fading_out && fade_level == 0) {
            uint16_t pp = g_engine.pad_pressed;
            int confirm = (pp & (RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_CIRCLE)) != 0;   /* raw 0xa0 */
            int cancel  = (pp & RE15_PAD_BIT_CROSS) != 0;                            /* raw 0x40 */
            if (screen == CFG_TOP) {
                if (pp & RE15_PAD_BIT_LEFT)  { if (--cur < 0) cur = 2; }
                if (pp & RE15_PAD_BIT_RIGHT) { if (++cur > 2) cur = 0; }
                if (confirm) {
                    if (cur == 0)      { screen = CFG_PICKER; cur = 0; }   /* CONFIG -> preset picker (cursor 0) */
                    else if (cur == 1) { screen = CFG_SOUND; }             /* SOUND  -> sound screen */
                    else               { fading_out = 1; }                 /* EXIT   -> leave config */
                }
            } else if (screen == CFG_PICKER) {
                if (pp & RE15_PAD_BIT_LEFT)  { if (--cur < 0) cur = 4; }
                if (pp & RE15_PAD_BIT_RIGHT) { if (++cur > 4) cur = 0; }
                if (cancel) { screen = CFG_TOP; cur = 0; }                 /* Cross -> back to top (CONFIG) */
                else if (confirm) {
                    if (cur <= 2)      { s_config_type = cur; memcpy(s_config_d08, s_config_preset[cur], 16); }  /* apply preset + relabel */
                    else if (cur == 3) { screen = CFG_EDIT; s_edit_phase = 0; s_edit_slot = 0; s_config_type = 3; }  /* EDIT custom */
                    else               { screen = CFG_TOP; cur = 0; }      /* EXIT -> back to top */
                }
            } else if (screen == CFG_SOUND) {
                if (pp & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN)) s_config_sound ^= 1;   /* raw 0x5000 toggle */
                if (confirm)     { re15_audio_set_mono(!s_config_sound); screen = CFG_TOP; cur = 1; }  /* save+apply */
                else if (cancel) { screen = CFG_TOP; cur = 1; }           /* back to top (SOUND), no save */
            } else if (screen == CFG_EDIT) {
                if (s_edit_phase == 0) {   /* slot-select: byte-true grid-jump LUTs, confirm -> panel, cancel -> picker */
                    if (pp & RE15_PAD_BIT_UP)    s_edit_slot = k_edit_up[s_edit_slot];
                    if (pp & RE15_PAD_BIT_DOWN)  s_edit_slot = k_edit_down[s_edit_slot];
                    if (pp & RE15_PAD_BIT_LEFT)  s_edit_slot = k_edit_left[s_edit_slot];
                    if (pp & RE15_PAD_BIT_RIGHT) s_edit_slot = k_edit_right[s_edit_slot];
                    if (confirm) { s_edit_code = 0; s_edit_phase = (s_edit_slot < 12) ? 1 : 2; }
                    else if (cancel) { screen = CFG_PICKER; cur = 3; }
                } else if (s_edit_phase == 1) {   /* panel A: 8 basic actions, UP/DOWN wrap [0,7] */
                    if (pp & RE15_PAD_BIT_UP)   s_edit_code = (s_edit_code + 7) & 7;
                    if (pp & RE15_PAD_BIT_DOWN) s_edit_code = (s_edit_code + 1) & 7;
                    if (confirm) { pc_edit_assign_a(); s_edit_phase = 0; }
                    else if (cancel) { s_edit_phase = 0; }
                } else {   /* panel B: 12 special codes, 2col x 6row */
                    if (pp & RE15_PAD_BIT_UP)    s_edit_code = k_editB_up[s_edit_code];
                    if (pp & RE15_PAD_BIT_DOWN)  s_edit_code = k_editB_down[s_edit_code];
                    if (pp & (RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) s_edit_code = k_editB_lr[s_edit_code];
                    if (confirm) { pc_edit_assign_b(); s_edit_phase = 0; }
                    else if (cancel) { s_edit_phase = 0; }
                }
            }
        }
        { uint32_t now = SDL_GetTicks(); uint32_t el = now - last; if (el < 33) SDL_Delay(33 - el); last = SDL_GetTicks(); }
    }
    re15_render_pc_set_title_fade(0);
    re15_render_pc_hide_config();
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    /* Phase 4.5.10-J: redirect stderr to debug.log so user can read
     * exact numerical state. */
    freopen("debug.log", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);   /* unbuffered for live tail */

    re15_render_init();
    re15_input_init();
    re15_audio_init();   /* FE-3: open the audio device early so the opening movie has sound
                          * (idempotent — the later call in the game-init path is a no-op). */

    /* FE-0.3 / FE-1.3 — top-level mode machine: de-hardcode the boot-into-a-room.
     * RE15_START_ROOM=<hex> keeps the debug fast-path (straight into that room, INGAME —
     * every parity/room-probe harness preserved); otherwise boot to the TITLE screen. */
    {
        const char *sr_env = getenv("RE15_START_ROOM");
        re15_gameflow_init((sr_env && *sr_env) ? (int)strtoul(sr_env, 0, 16) : -1);
    }
    /* FE-3 — the CAPCOM.STR opening (re15_str.c video + re15_xa.c audio, byte-true). It is the WHOLE
     * boot intro: the "This game contains scenes of explicit violence and gore." disclaimer over a
     * lab scene (~frame 80-300), then the CAPCOM logo (~frame 320-500) — CAPTURE-VERIFIED against the
     * real MZD disc (shots/boot_seq.png = BIOS -> disclaimer -> CAPCOM -> title; shots/capcom_frames.png
     * = the decoded movie). The EXE main-loop plays it via gate DAT_800aca38 bit 0x8000 @main.c:20
     * (one-shot, cleared @0x80020c48). My earlier "dormant" was for the un-modded prototype EXE where
     * nothing sets that bit — but the MZD build the port targets sets it on boot and DOES play the
     * intro. So it plays on every title boot (default ON); RE15_NO_INTRO skips it for quick iteration,
     * RE15_FMV_SHOT="<frame>:<path.bmp>" dumps one decoded frame then exits. */
    g_gameflow.boot_movie = getenv("RE15_NO_INTRO") ? 0 : 1;
    if (re15_gameflow_mode() == RE15_MODE_TITLE && g_gameflow.boot_movie) {
        g_gameflow.boot_movie = 0;                                    /* one-shot clear (byte-true) */
        extern void re15_render_pc_show_fmv(const uint32_t *rgba, int w, int h);
        extern void re15_render_pc_hide_fmv(void);
        extern void re15_render_pc_screenshot(const char *path);
        int msz = 0;
        uint8_t *mbuf = pc_read_shared("MOVIE/CAPCOM.STR", &msz);
        re15_str_t movie = {0};
        if (mbuf && re15_str_open(&movie, mbuf, (size_t) msz) == 0) {
            uint32_t *frame_rgba = (uint32_t *) malloc((size_t) movie.width * movie.height * 4);
            extern void   re15_fmv_audio_start(const int16_t *src, int src_frames, int src_rate, int cd_vol);
            extern void   re15_fmv_audio_stop(void);
            extern double re15_fmv_audio_time(void);
            const char *fmv_shot = getenv("RE15_FMV_SHOT");
            int  shot_frame = -1; char shot_path[256] = {0};
            if (fmv_shot) sscanf(fmv_shot, "%d:%255s", &shot_frame, shot_path);

            /* Decode the movie's CD-XA soundtrack up-front (the whole STR is in RAM) and start it.
             * The PSX has the CD controller auto-decode XA into the SPU CD-input at CD_initvol
             * (0x3FFF); the port software-decodes (re15_xa.c) and mixes at the same gate. */
            int16_t *apcm = NULL; int aframes = 0, arate = 0, achans = 0;
            int have_audio = 0;
            if (!fmv_shot && re15_xa_decode_movie(mbuf, (size_t) msz, &apcm, &aframes, &arate, &achans) == 0) {
                /* byte-true movie CD->SPU gain = 100/128 (CdlATV {0x64,0,0x64,0}, FUN_8002acac
                 * stereo branch fed by FUN_8002ac84(100)); the fades are instant gates. */
                re15_fmv_audio_start(apcm, aframes, arate, 100);
                /* Audio is the master clock ONLY when the SDL audio device is actually running; with
                 * RE15_NOAUDIO (or a session with no audio endpoint) the mixer callback never fires, so
                 * the clock would stay 0 and the frame-wait below would hang forever — fall back to the
                 * fixed 30fps timer in that case. */
                have_audio = g_audio.initialized && (re15_fmv_audio_time() >= 0.0);
            }

            /* Cadence (byte-true): on PSX the CD streams audio continuously and video frames
             * present as they arrive (FUN_8002a630) — AUDIO is the master clock. So we present
             * video frame f when the audio playback position reaches f/30 s (the authored ~30fps:
             * 525 frames / 17.55s = 29.92fps). If audio is unavailable, fall back to a fixed 30fps
             * SDL_Delay timer (same as the main loop). Skip = raw START (FUN_80029cd8:24, bit 0x800). */
            const uint32_t fmv_budget_ms = 1000u / 30u;
            uint32_t fmv_last = 0;
            int skipped = 0;
            for (int f = 0; f < movie.num_frames && frame_rgba && !skipped; f++) {
                if (re15_str_decode(&movie, f, frame_rgba) != 0) continue;
                if (have_audio) {
                    /* wait until the audio clock reaches this frame (audio = master) */
                    double frame_time = (double) f / 30.0;
                    for (;;) {
                        double t = re15_fmv_audio_time();
                        if (t < 0.0 || t >= frame_time) break;   /* audio ended, or reached frame */
                        if (g_engine.pad_pressed & RE15_PAD_BIT_START) { skipped = 1; break; }
                        re15_input_tick();
                        SDL_Delay(2);
                    }
                    if (skipped) break;
                }
                re15_render_begin_frame();
                re15_input_tick();
                re15_render_pc_show_fmv(frame_rgba, movie.width, movie.height);
                re15_render_end_frame();
                if (fmv_shot && f == shot_frame) {
                    re15_render_pc_screenshot(shot_path);
                    exit(0);                                /* one-shot verification probe */
                }
                if (g_engine.pad_pressed & RE15_PAD_BIT_START) skipped = 1;
                if (have_audio) {
                    if (re15_fmv_audio_time() < 0.0) break;  /* audio finished -> end the movie */
                } else {
                    uint32_t now = SDL_GetTicks();           /* no audio: fixed 30fps timer */
                    if (fmv_last != 0 && now - fmv_last < fmv_budget_ms)
                        SDL_Delay(fmv_budget_ms - (now - fmv_last));
                    fmv_last = SDL_GetTicks();
                }
            }
            re15_fmv_audio_stop();
            if (apcm) free(apcm);
            free(frame_rgba);
            re15_render_pc_hide_fmv();
        }
        free(mbuf);
    }

    /* FE-5.3 mode-cycle: on death the game loop sets mode=TITLE and exits; control jumps back
     * HERE (after boot/FMV, before the title menu) so the REAL title menu runs — its CONTINUE
     * reloads a card save via the FE-4.3 path, NEW GAME restarts. Reuses byte-true code rather
     * than the old inline "reload-the-current-room" death stand-in. */
re_title:;
    if (re15_gameflow_mode() == RE15_MODE_TITLE) {
        /* BOOT TITLE (FE-1.3): DATA/TITLEU.TIM + blinking PRESS START, until START = NEW GAME.
         * NEW GAME falls through to the normal boot of the intro (ROOM1240 = 0x1240 = start_room). */
        extern void re15_render_pc_show_title(const re15_tim_t *tim);
        extern void re15_render_pc_hide_title(void);
        extern void re15_render_pc_screenshot(const char *path);
        extern void re15_render_pc_title_menu(const re15_tim_t *tmoji, int cursor);   /* byte-true TMOJI sprites */
        extern void re15_render_pc_hide_title_menu(void);
        extern void re15_render_pc_set_title_fade(int a);   /* front-end fade (menu->player-select) */
        re15_tim_t s_boot_title = {0}, s_tmoji = {0};
        { int tsz = 0; uint8_t *tb = pc_read_shared("DATA/TITLEU.TIM", &tsz);
          if (tb) re15_tim_parse(tb, tsz, &s_boot_title); }
        { int msz = 0; uint8_t *mb = pc_read_shared("DATA/TMOJI.TIM", &msz);   /* the menu-text sprite sheet */
          if (mb) re15_tim_parse(mb, msz, &s_tmoji); }
        const char *t_shot = getenv("RE15_TITLE_SHOT");   /* debug: dump the title/menu frame + auto-advance */
        unsigned tblink = 0;
        int cursor = 0;   /* 0=NEW GAME 1=LOAD GAME 2=OPTION — byte-true 3-item menu (TITLE.BIN FUN_80102b00) */
        if (getenv("RE15_PSELECT_TEST")) {   /* debug: jump straight into the LEON/ELZA player-select */
            int ch = pc_run_player_select();
            fprintf(stderr, "[pselect] returned character %d (%s)\n", ch, ch ? "Elza" : "Leon");
            exit(0);
        }
        if (getenv("RE15_CONFIG_TEST")) {    /* debug: jump straight into the OPTIONS/config screen */
            const char *cs = getenv("RE15_CONFIG_SHOT");
            if (cs) { extern void re15_render_pc_config(const re15_tim_t *bg, int tab);
                      extern void re15_render_pc_config_clear(void);
                      extern void re15_render_pc_screenshot(const char *path);
                      static re15_tim_t bg = {0}; int sz=0; uint8_t *b = pc_read_shared("DATA/C_BACK2.TIM",&sz);
                      if (b) re15_tim_parse(b,sz,&bg);
                      static re15_tim_t tim = {0}; int tsz=0; uint8_t *tb = pc_read_shared("DATA/CONFIG.TIM",&tsz);
                      if (tb) re15_tim_parse(tb,tsz,&tim);
                      (void)sz; (void)tsz;
                      /* RE15_CONFIG_TAB selects the screen for the shot: 0/1/2 = top menu cursor CONFIG/SOUND/EXIT,
                       * 3 = preset picker, 4 = SOUND, 5 = EDIT slot-select, 6 = EDIT panel A, 7 = EDIT panel B.
                       * RE15_CONFIG_CUR = picker cursor / EDIT slot; RE15_CONFIG_CODE = EDIT panel cursor. */
                      int t = atoi(getenv("RE15_CONFIG_TAB")?getenv("RE15_CONFIG_TAB"):"0");
                      int screen = (t <= 2) ? CFG_TOP : (t == 3 ? CFG_PICKER : (t == 4 ? CFG_SOUND : CFG_EDIT));
                      int cur = (t <= 2) ? t : atoi(getenv("RE15_CONFIG_CUR")?getenv("RE15_CONFIG_CUR"):"0");
                      if (t >= 5) { s_config_type = 3; s_edit_phase = t - 5; s_edit_slot = cur;
                                    s_edit_code = atoi(getenv("RE15_CONFIG_CODE")?getenv("RE15_CONFIG_CODE"):"0"); }
                      for (int f=0; f<4; f++) {   /* render a few frames so the shot reads a presented buffer, not stale black */
                          re15_render_begin_frame(); re15_render_background_gradient(0,0,0,0,0,0);
                          re15_render_pc_config_clear();
                          re15_render_pc_config(&bg, 0); pc_config_draw_labels();
                          pc_config_draw_overlay(&tim, screen, cur); re15_render_end_frame(); }
                      re15_render_pc_screenshot(cs); exit(0); }
            pc_run_config(); exit(0);
        }
        /* The real RE1.5 title menu, VISUALLY verified against the PSX (shots/psx_title_a.png): the 3 rows
         * NEW GAME / LOAD GAME / OPTION + the CAPCOM copyright are SPRITES from DATA/TMOJI.TIM, drawn at
         * x=0x20, y=0x85/0x99/0xad (draw code FUN_801027a0, descriptor 0x801028ac); the active row uses the
         * white CLUT sub-palette, the others blue. Shown immediately (init skips "press any button").
         * (My earlier "LOAD DATA"/"CONFIG" was wrong — those ASCII strings @0x80 are the vestigial dev
         * menu; the real sprite labels read LOAD GAME / OPTION.) */
        while (re15_gameflow_mode() == RE15_MODE_TITLE) {
            re15_render_begin_frame();
            re15_input_tick();                       /* SDL_QUIT -> exit(0) inside; refreshes pad */
            re15_render_background_gradient(8, 8, 16, 0, 0, 0);
            if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);
            uint16_t pp = g_engine.pad_pressed;
            if (getenv("RE15_CONTINUE_TEST")) {              /* debug: auto-drive title -> LOAD GAME */
                if (tblink == 8)  cursor = 1;
                if (tblink == 16) pp |= RE15_PAD_BIT_CROSS;  /* confirm LOAD GAME -> load screen */
            }
            /* Draw the byte-true NEW GAME / LOAD GAME / OPTION + copyright sprites from TMOJI.TIM
             * (active row white, others blue) at x=0x20, y=0x85/0x99/0xad. */
            re15_render_pc_title_menu(&s_tmoji, cursor);

            if (pp & RE15_PAD_BIT_UP)    cursor = (cursor + 2) % 3;
            if (pp & RE15_PAD_BIT_DOWN)  cursor = (cursor + 1) % 3;
            /* confirm = any of the four face buttons OR Start (byte-true mask 0x8f0 @0x800bc762) */
            uint16_t confirm = pp & (RE15_PAD_BIT_CROSS | RE15_PAD_BIT_SQUARE |
                                     RE15_PAD_BIT_TRIANGLE | RE15_PAD_BIT_CIRCLE | RE15_PAD_BIT_START);
            if (confirm) {
                if (cursor == 0) {                            /* NEW GAME -> fade out menu -> player-select */
                    /* byte-true: the menu confirm fades to black (0x80102ccc) then REPLACES the task
                     * with the player-select (FUN_80029ba4 @0x80102c9c). Fade the menu out (32f, rate
                     * 8/frame) drawing the title + menu each frame, then hand off. */
                    for (int f = 8; f <= 256; f += 8) {
                        re15_render_begin_frame();
                        re15_input_tick();
                        re15_render_background_gradient(8, 8, 16, 0, 0, 0);
                        if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);
                        re15_render_pc_title_menu(&s_tmoji, cursor);
                        re15_render_pc_set_title_fade(f > 255 ? 255 : f);
                        re15_render_end_frame();
                    }
                    int ch = pc_run_player_select();          /* "PLEASE SELECT MAIN CAST" (@0x80101094) */
                    re15_gameflow_new_game(ch);               /* ch = DAT_800aca5c>>2 (0=Leon,1=Elza) */
                }
                else if (cursor == 1) {                       /* LOAD GAME -> FE-4 memory-card load screen */
                    uint16_t resume_room = 0;
                    if (pc_run_memcard_screen(0, NULL, &resume_room) >= 0) {
                        /* loaded into s_resume_sd; enter INGAME at the saved room, then the
                         * game loop applies s_resume_sd after the room + player are set up. */
                        s_resume_pending        = 1;
                        g_gameflow.character     = s_resume_sd.character;
                        g_gameflow.start_room    = resume_room;
                        g_gameflow.enter_ingame  = 1;
                        g_gameflow.mode          = RE15_MODE_INGAME;   /* exits the title loop */
                    }
                }
                else if (cursor == 2) {                       /* OPTION -> controller-CONFIG screen */
                    pc_run_config();                          /* byte-true EXE task @0x8002dde4 (foundation) */
                    if (s_boot_title.pixels) re15_render_pc_show_title(&s_boot_title);   /* restore title art */
                }
            }
            { extern void re15_render_pc_set_title_fade(int a);   /* fade in from the CAPCOM intro */
              re15_render_pc_set_title_fade(tblink < 20 ? 255 - (int)tblink * 13 : 0); }
            re15_render_end_frame();
            re15_render_pc_hide_title_menu();   /* stop drawing the menu sprites once the title yields */
            { unsigned t_af = 22; const char *afe = getenv("RE15_TITLE_SHOT_AF"); if (afe) t_af = (unsigned)atoi(afe);
              if (t_shot && tblink == t_af) { re15_render_pc_screenshot(t_shot); re15_gameflow_new_game(0); } }
            tblink++;
        }
        re15_render_pc_hide_title();
    }

    /* RNG SESSION ENTROPY (interactive runs only): the original FUN_8001af20 state evolves from
     * boot across ALL callers (idle timers/SEs/other rooms) -> each playthrough sees different
     * behavior rolls. The port's fixed-seed xorshift + a fixed draw count made every roll IDENTICAL
     * per run (e.g. the chair zombie ALWAYS rolled the slow 0x13 shamble). Seed from the wall clock
     * unless a deterministic parity run is scripted (RE15_INPUT_SCRIPT / RE15_POSE_DUMP). */
    {
        extern void re15_damage_seed_rng(uint32_t seed);
        if (!getenv("RE15_INPUT_SCRIPT") && !getenv("RE15_POSE_DUMP"))
            re15_damage_seed_rng((uint32_t)SDL_GetTicks() ^ 0xA5F15A3Du ^ (uint32_t)time(NULL));
    }

    /* BE-round REVERTED (2026-05-28): tried 60fps default + 60Hz SCD as
     * PSX-canonical, but our cinematic was tuned for 30Hz SCD so all
     * Sleep gates now elapse 2× faster, breaking cut-chain timing.
     * Keep 30fps default; rotor stroboscopic strobe remains a known
     * artifact. RE15_FPS=60 still available for future re-tuning round. */
    int target_fps = 30;
    {
        const char *fps_env = getenv("RE15_FPS");
        if (fps_env) {
            int parsed = atoi(fps_env);
            if (parsed >= 15 && parsed <= 240) target_fps = parsed;
        }
    }
    const uint32_t frame_budget_ms = (uint32_t)(1000 / target_fps);
    fprintf(stderr, "[fps] target=%d FPS, frame_budget=%ums (RE15_FPS env to override)\n",
            target_fps, frame_budget_ms);

    /* AI-round (2026-05-26): scale frame-count-based gates by target_fps.
     * All historic frame_count constants (1800 sub00 spawn, 3390 Leon visible,
     * shot_frames[] autoshot targets) were tuned for 60fps render. At 30fps
     * each frame = 2x wall-time so these gates would fire at half the rate
     * → Leon wouldn't be visible until 113s instead of 56s.
     * Use FRAME_AT_60(n) = n at 60fps target, n/2 at 30fps target, etc. */
#define FRAME_AT_60(n)  (((n) * target_fps) / 60)

    /* Phase 4.5.6.4: software MDEC + decode bundled BG. Mirror the PSX
     * flow (re15_bg_init + re15_bg_load_test_asset at boot) so both
     * targets show the same room background. PC decodes the BSS chunk
     * in software (IDCT + YUV→RGB) into a 320×240 RGBA cache. */
    re15_bg_init();
    /* Boot BG preload: re15_bg_load_cut(0) is room-aware via g_current_room_id,
     * which is still the room_common.c default (0x1170) HERE — g_current_room_id
     * isn't set to boot_room until ~L457 below. So this caches ROOM1170's cut 0 as
     * a transient placeholder; it is immediately overwritten once the boot room's
     * SCD runs and its first Cut_chg fires the real per-cut load (~L1274). For the
     * ROOM1240 boot this is the pre-intro: sub[2] Cut_chg(0..8) cycles ROOM1240's OWN
     * BG00..BG08 (black->zombie->T-Virus->STARS->heli->Umbrella), verified byte-true
     * against the original — the crates the placeholder shows never reach the screen
     * (frame 0 is cut 0 = black). Fall back to the bundled test asset if the file is
     * missing, so dev builds without room BSS still show something. */
    if (re15_bg_load_cut(0) != 0) {
        re15_bg_load_test_asset();
    }

    /* Phase 4.5.9 / globalization Phase 3-A (2026-06-13): load + parse the room RDT
     * EARLY — before audio + props — so the footstep VAB, the Obj_model_set props,
     * and the heli/pilot can be SLICED from it (g_room_rdt prop + snd banks) exactly
     * like the PSX engine, instead of separate room####_obj## / snd0 files. The local
     * `rdt`/`rdt_ok`/`rdt_buf` keep driving the ~32 main-loop refs (cuts/zones/scd/
     * cross-room); we ALSO publish to g_room_rdt + g_current_room_id so the shared
     * footstep loader (audio_pc) and the cross-room machinery see the boot room.
     * (Was loaded later, after props — moved up; RDT parse only needs file I/O.) */
    int rdt_size = 0;
    /* RE15_START_ROOM=1150 boots a different room's RDT (debug: render the room1150 Irons
     * kneel cutscene to compare frame-exact vs ablauf4). Default = the ROOM1170 intro. */
    const char *start_room = getenv("RE15_START_ROOM");
    /* FE-4: default boot room = g_gameflow.start_room (0x1240 new-game, or the CONTINUE
     * resume room) instead of a hardcoded 0x1240; RE15_START_ROOM still overrides for debug. */
    unsigned boot_room = (start_room && *start_room) ? (unsigned)strtoul(start_room, 0, 16)
                                                     : (unsigned)g_gameflow.start_room;
    /* Asset-Pfad-Konsolidierung (2026-07-02): der Boot-Room-RDT kommt aus dem CD-Layout
     * STAGE{N}/ROOM%04X.RDT (N = room_id>>12), NICHT mehr aus der entfernten flachen RDT/-Struktur.
     * Identische Auflösung wie re15_room_load (room_pc.c) für die Cross-Room-Transitions. */
    char rdt_path[32]; snprintf(rdt_path, sizeof rdt_path, "STAGE%u/ROOM%04X.RDT",
                                (boot_room >> 12) & 0xFu, boot_room);
    uint8_t *rdt_buf = pc_read_shared(rdt_path, &rdt_size);
    fprintf(stderr, "[boot] room RDT: %s (%d bytes)\n", rdt_path, rdt_size);
    re15_rdt_t rdt = {0};
    int rdt_ok = 0;
    if (rdt_buf && re15_rdt_parse(rdt_buf, (size_t)rdt_size, &rdt) == 0) {
        rdt_ok = 1;
        g_room_rdt        = rdt;        /* publish for the shared footstep loader + parity */
        g_room_rdt_ok     = 1;
        g_current_room_id = boot_room;
        fprintf(stderr, "[rdt] nCut=%d nDoor=%d nItem=%d zones=%d main_scd=%zuB sub=%d props=%d\n",
                rdt.nCut, rdt.nDoor, rdt.nItem, rdt.zone_count,
                rdt.main_scd_size, rdt.sub_scd_count, rdt.prop_count);
    }
    /* Phase ESP-C: parse + bind this room's effect-sprite bank for op-0x3a spawns. */
    pc_load_room_esp(rdt_buf, rdt_size, boot_room);

    /* Phase ESP-D: load the GLOBAL effect bank CORE00.ESP once at game-init (FUN_8001923c) so the
     * universal hit effects (effect-id 0, used by the master-table hit handlers) resolve. */
    {
        int gsz = 0;
        s_global_esp_buf = pc_read_shared("DATA/CORE00.ESP", &gsz);
        if (s_global_esp_buf && re15_esp_parse_global(s_global_esp_buf, (size_t)gsz, &s_global_esp) == 0) {
            re15_esp_set_global_bank(&s_global_esp);
            fprintf(stderr, "[esp] global bank CORE00.ESP: %d effects (incl effect-id 0 hit fx)\n",
                    s_global_esp.id_count);
        } else {
            fprintf(stderr, "[esp] global bank CORE00.ESP NOT loaded\n");
        }
        /* The GLOBAL effect textures live only in VRAM (no RDT TIM). All four sheets were
         * extracted byte-true from the live ShowVRAM ground truth (re15_vram_extract.py):
         * id 0 = hit/blood, id 2 = muzzle flash/sparks, id 3 = smoke, id 4 = shell/debris —
         * the ids the handgun discharge spawns (@0x800337bc: 0x02000800/0x03000c00/0x04000800).
         * Upload each to its own slot so pc_draw_effects binds per effect id. */
        static const struct { const char *file; int slot; const char *tag; } k_gfx[] = {
            { "extracted_fx/effect0_blood.tim",  RE15_TIM_SLOT_EFFECT_GLOBAL, "0 blood"  },
            { "extracted_fx/effect2_muzzle.tim", RE15_TIM_SLOT_FX_MUZZLE,     "2 muzzle" },
            { "extracted_fx/effect3_smoke.tim",  RE15_TIM_SLOT_FX_SMOKE,      "3 smoke"  },
            { "extracted_fx/effect4_shell.tim",  RE15_TIM_SLOT_FX_SHELL,      "4 shell"  },
        };
        for (size_t gi = 0; gi < sizeof(k_gfx)/sizeof(k_gfx[0]); gi++) {
            int bsz = 0;
            uint8_t *gtim = pc_read_shared(k_gfx[gi].file, &bsz);
            if (gtim) {
                re15_tim_t btim;
                if (re15_tim_parse(gtim, bsz, &btim) == 0) {
                    re15_render_pc_upload_tim_slot(&btim, k_gfx[gi].slot);
                    fprintf(stderr, "[esp] global effect-%s TIM -> slot %d: %dx%d %dbpp\n",
                            k_gfx[gi].tag, k_gfx[gi].slot, btim.width, btim.height, btim.bpp);
                } else {
                    fprintf(stderr, "[esp] %s parse FAILED\n", k_gfx[gi].file);
                }
                free(gtim);
            } else {
                fprintf(stderr, "[esp] %s NOT found\n", k_gfx[gi].file);
            }
        }
    }

    /* Phase 4.6.1: SDL audio device + SCD audio queue consumer. Silent
     * playback callback until 4.6.3 wires the ADPCM mixer in. */
    re15_audio_init();

    /* Load + parse test asset. Try several relative paths so it works whether
     * run from build/Release/, from project root, or installed bin/. */
    int tim_size = 0;
    uint8_t *tim_buf = pc_read_shared("PLD/PL00.TIM", &tim_size);
    re15_tim_t tim;
    int tim_ok = 0;
    if (tim_buf && re15_tim_parse(tim_buf, tim_size, &tim) == 0) {
        tim_ok = 1;
        fprintf(stderr, "[tim] loaded test.tim: %dx%d bpp=%d clut=%d\n",
                tim.width, tim.height, tim.bpp, tim.has_clut);
        /* Phase 4.5.5: upload to GPU as SDL_Texture so SDL_RenderGeometry
         * can sample it for textured 3D triangles. */
        re15_render_pc_upload_tim(&tim);
    } else {
        fprintf(stderr, "[tim] FAILED to load test.tim\n");
    }

    /* Phase 4.5: load MD1 mesh — same path-search pattern */
    int md1_size = 0;
    uint8_t *md1_buf = pc_read_shared("PLD/PL00.MD1", &md1_size);
    /* Zero-init avoids "possibly uninitialised" warnings on toolchains that
     * can't see the md1_ok guard implies md1 is populated. The fields the
     * renderer reads later (mesh_count, meshes[]) are then well-defined. */
    re15_md1_t md1 = {0};
    int md1_ok = (md1_buf && re15_md1_parse(md1_buf, md1_size, &md1) == 0);
    if (md1_ok) {
        fprintf(stderr, "[md1] loaded test.md1: %d meshes\n", md1.mesh_count);
    }

    /* Forward-declare render helpers used by the per-prop loader below. */
    extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
    extern void re15_render_pc_bind_tim_slot(int slot);

    /* Generic per-prop loader for all 6 ROOM1170 obj models (obj_id 0..5).
     * Each gets its own MD1 + TIM in slots 4..9 so the prop renderer can
     * bind the correct texture per prop. (Slot 0=Leon, 1=Elliot, 2=heli
     * body legacy, 3=pilot legacy, 4..9 = obj_id 0..5 generic.)            */
    re15_md1_t s_room_prop_md1[6] = {0};
    int        s_room_prop_ok [6] = {0};
    /* Data-driven per-room prop set (parity with PSX). Boots ROOM1170; the cross-
     * room consume reloads for the destination room (room1140 etc.). */
    pc_load_room_prop_set(&rdt, s_room_prop_md1, s_room_prop_ok);   /* boot room RDT (room1170) */

    /* Helicopter body (obj 0x02) + Pilot (obj 0x05) — Phase 3-A (2026-06-13): both are
     * SLICED from the RDT prop table (prop[2] heli, prop[5] pilot), TIMs → render slots
     * 2/3. Were room1170_obj0[25].{md1,tim} files; pointers alias the resident RDT
     * buffer (byte-true == those files). (slot 0=Leon, 1=Elliot, 2=heli, 3=pilot.) */
    re15_md1_t heli_md1 = {0};
    int heli_ok = (rdt_ok && rdt.prop_md1[2] &&
                   re15_md1_parse(rdt.prop_md1[2], (size_t)rdt.prop_md1_size[2], &heli_md1) == 0);
    if (heli_ok) fprintf(stderr, "[md1] heli body (RDT prop2): %d meshes\n", heli_md1.mesh_count);
    if (rdt_ok && rdt.prop_tim[2]) {
        re15_tim_t heli_tim;
        if (re15_tim_parse(rdt.prop_tim[2], rdt.prop_tim_size[2], &heli_tim) == 0) {
            re15_render_pc_upload_tim_slot(&heli_tim, 2);
            fprintf(stderr, "[tim] heli TIM (RDT prop2) in slot 2: %dx%d\n", heli_tim.width, heli_tim.height);
        } else {
            fprintf(stderr, "[tim] heli TIM parse FAILED — heli will render with Leon's TIM\n");
        }
    }

    re15_md1_t pilot_md1 = {0};
    int pilot_ok = (rdt_ok && rdt.prop_md1[5] &&
                    re15_md1_parse(rdt.prop_md1[5], (size_t)rdt.prop_md1_size[5], &pilot_md1) == 0);
    if (pilot_ok) fprintf(stderr, "[md1] pilot (RDT prop5): %d meshes\n", pilot_md1.mesh_count);
    if (rdt_ok && rdt.prop_tim[5]) {
        re15_tim_t pilot_tim;
        if (re15_tim_parse(rdt.prop_tim[5], rdt.prop_tim_size[5], &pilot_tim) == 0) {
            re15_render_pc_upload_tim_slot(&pilot_tim, 3);
            fprintf(stderr, "[tim] pilot TIM (RDT prop5) in slot 3: %dx%d\n", pilot_tim.width, pilot_tim.height);
        } else {
            fprintf(stderr, "[tim] pilot TIM parse FAILED — pilot will render with Leon's TIM\n");
        }
    }

    /* Phase 4.5.13-R23: Elliot's actual model is PL05.PLD (not em47 EMD).
     * Load his MD1 mesh + EDD/EMR for skeletal animation so NPC[1] (type
     * 0x47) renders as Elliot instead of a Leon-clone. */
    int elliot_md1_size = 0;
    uint8_t *elliot_md1_buf = pc_read_shared("PLD/ELLIOT.MD1", &elliot_md1_size);
    re15_md1_t elliot_md1 = {0};
    int elliot_ok = (elliot_md1_buf && re15_md1_parse(elliot_md1_buf, elliot_md1_size, &elliot_md1) == 0);
    int elliot_edd_size = 0, elliot_emr_size = 0;
    uint8_t *elliot_edd_buf = pc_read_shared("PLD/ELLIOT.EDD", &elliot_edd_size);
    uint8_t *elliot_emr_buf = pc_read_shared("PLD/ELLIOT.EMR", &elliot_emr_size);
    re15_emd_animation_t elliot_anim = {0};
    re15_emd_skeleton_t  elliot_skel = {0};
    int elliot_skel_ok = 0;
    if (elliot_edd_buf && elliot_emr_buf) {
        if (re15_emd_parse_animation(elliot_edd_buf, elliot_edd_size, &elliot_anim) == 0 &&
            re15_emd_parse_skeleton (elliot_emr_buf, elliot_emr_size, &elliot_skel) == 0) {
            elliot_skel_ok = 1;
            fprintf(stderr, "[elliot] PL05 loaded: %d meshes, %d bones, %d clips\n",
                    elliot_md1.mesh_count, elliot_skel.bone_count, elliot_anim.clip_count);
        }
    }

    /* AD-round (2026-05-26): load PL00W01 (handgun weapon track) for the
     * RUN/WALK_FORWARD animation. PL00.EDD only has Walk_Backward / Damage
     * / Climb / Stairs. The real forward-walk + run live in the weapon
     * track EDDs (PL00W*.PLW) per RE2-Nov96 architecture. Per bio-lib enum
     * weapon track: clip 0 = Run, clip 5 = Walk_Forward.
     * PL00W01.edd verified: 14 clips, clip 5 = 30 frames (Walk_Forward),
     * clip 0 = 22 frames (Run). */
    int w01_edd_size = 0, w01_emr_size = 0;
    uint8_t *w01_edd_buf = pc_read_shared("PLD/PL00W01.EDD", &w01_edd_size);
    uint8_t *w01_emr_buf = pc_read_shared("PLD/PL00W01.EMR", &w01_emr_size);
    re15_emd_animation_t w01_anim = {0};
    re15_emd_skeleton_t  w01_skel_raw = {0};   /* W01-owned bind+keyframes */
    re15_emd_skeleton_t  w01_skel = {0};        /* composite used by renderer */
    int w01_ok = 0;
    if (w01_edd_buf && w01_emr_buf) {
        if (re15_emd_parse_animation(w01_edd_buf, w01_edd_size, &w01_anim) == 0 &&
            re15_emd_parse_skeleton (w01_emr_buf, w01_emr_size, &w01_skel_raw) == 0) {
            w01_ok = 1;
            fprintf(stderr, "[w01] PL00W01 weapon-track: %d bones, %d clips, %d kf\n",
                    w01_skel_raw.bone_count, w01_anim.clip_count, w01_skel_raw.keyframe_count);
        }
    }
    /* PL00W03 = the GUN carry set (byte-true item->bank map: the equip loader @0x80036b80 loads
     * CD file DAT_800741e8[char]=76 + item — melee trio W00≡W01≡W02 (items 0-2, md5-identical),
     * gun pair W03≡W04 (items 3/4). Same 14-clip layout as W01 (raise 6=10f, holds 8/10/12=1f,
     * recoils 7/9/11=23/24/24f, reload 0xD=32f). W03.EDD/.EMR = byte-true slices of PL00W03.PLW
     * (dir[0]/dir[1]), extracted the same way as the vendored W01 pair (slice==vendored verified). */
    int w03_edd_size = 0, w03_emr_size = 0;
    uint8_t *w03_edd_buf = pc_read_shared("PLD/PL00W03.EDD", &w03_edd_size);
    uint8_t *w03_emr_buf = pc_read_shared("PLD/PL00W03.EMR", &w03_emr_size);
    re15_emd_animation_t w03_anim = {0};
    re15_emd_skeleton_t  w03_skel_raw = {0};
    re15_emd_skeleton_t  w03_skel = {0};
    int w03_ok = 0;
    if (w03_edd_buf && w03_emr_buf) {
        if (re15_emd_parse_animation(w03_edd_buf, w03_edd_size, &w03_anim) == 0 &&
            re15_emd_parse_skeleton (w03_emr_buf, w03_emr_size, &w03_skel_raw) == 0) {
            w03_ok = 1;
            fprintf(stderr, "[w03] PL00W03 gun-track: %d bones, %d clips, %d kf\n",
                    w03_skel_raw.bone_count, w03_anim.clip_count, w03_skel_raw.keyframe_count);
        }
    }
    /* WEAPON-IN-HAND MODELS (room-fix #3, byte-true mechanism: the melee DRAW's anim-event
     * attaches the equipped weapon's model pointers @0x800356f0-70 into kine+1900..1912 —
     * drawn as extra parts on the weapon bone (bone 11). The MESHES live in the PLW
     * containers: dir[2] = MD1 (1 mesh), dir[3] = TIM; PL00.MD1's meshes 15/16 are only the
     * empty attach slots). Slice + parse both class models; TIMs -> slots 24/25. */
    re15_md1_t wpn_md1[2]; int wpn_md1_ok[2] = {0, 0};
    for (int wi = 0; wi < 2; wi++) {
        const char *plw_name = wi ? "PLD/PL00W03.PLW" : "PLD/PL00W01.PLW";
        int psz = 0;
        uint8_t *plw = pc_read_shared(plw_name, &psz);   /* stays resident (MD1 borrows) */
        if (!plw || psz < 16) continue;
        uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
        if (diroff + 16 > (uint32_t)psz) continue;
        uint32_t de[4];
        for (int k = 0; k < 4; k++)
            de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                               (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
        if (de[2] >= de[3] || de[3] > (uint32_t)psz) continue;
        if (re15_md1_parse(plw + de[2], (int)(de[3] - de[2]), &wpn_md1[wi]) == 0) {
            re15_tim_t wtim;
            if (re15_tim_parse(plw + de[3], (int)(diroff - de[3]), &wtim) == 0) {
                re15_render_pc_upload_tim_slot(&wtim, wi ? RE15_TIM_SLOT_WPN_GUN
                                                         : RE15_TIM_SLOT_WPN_MELEE);
                wpn_md1_ok[wi] = 1;
                fprintf(stderr, "[wpn] %s: %d mesh(es), TIM %dx%d -> slot %d\n", plw_name,
                        wpn_md1[wi].mesh_count, wtim.width, wtim.height,
                        wi ? RE15_TIM_SLOT_WPN_GUN : RE15_TIM_SLOT_WPN_MELEE);
            }
        }
    }

    /* Elliot TIM into slot 1. */
    int elliot_tim_size = 0;
    uint8_t *elliot_tim_buf = pc_read_shared("PLD/ELLIOT.TIM", &elliot_tim_size);
    re15_tim_t elliot_tim;
    if (elliot_tim_buf && re15_tim_parse(elliot_tim_buf, elliot_tim_size, &elliot_tim) == 0) {
        re15_render_pc_upload_tim_slot(&elliot_tim, 1);
        fprintf(stderr, "[tim] elliot TIM in slot 1: %dx%d\n", elliot_tim.width, elliot_tim.height);
    } else {
        fprintf(stderr, "[tim] elliot TIM FAILED to load — NPC type 0x47 will use Leon's TIM\n");
    }
    if (elliot_ok) {
        fprintf(stderr, "[md1] loaded elliot mesh: %d meshes\n", elliot_md1.mesh_count);
    } else {
        fprintf(stderr, "[md1] elliot MD1 FAILED to load — NPC type 0x47 will use Leon's mesh\n");
    }

    /* Phase 4.5.7.3: load EDD (animation) + EMR (skeleton) for the
     * skeletal renderer. The EMR pointer is held by skel.keyframe_data
     * — keep emr_buf alive for the program's lifetime. */
    int edd_size = 0, emr_size = 0;
    uint8_t *edd_buf = pc_read_shared("PLD/PL00.EDD", &edd_size);
    uint8_t *emr_buf = pc_read_shared("PLD/PL00.EMR", &emr_size);

    re15_emd_animation_t anim = {0};
    re15_emd_skeleton_t  skel = {0};
    int skel_ok = 0;
    if (edd_buf && emr_buf
        && re15_emd_parse_animation(edd_buf, (size_t)edd_size, &anim) == 0
        && re15_emd_parse_skeleton (emr_buf, (size_t)emr_size, &skel) == 0) {
        skel_ok = 1;
        fprintf(stderr, "[skel] PL00: %d bones, %d clips, %d keyframes\n",
                skel.bone_count, anim.clip_count, skel.keyframe_count);
    }

    /* Phase 4.5.13-B2: try to load room-local animation.rbj (cinematic
     * anim source). If present + parsable, overlay rbj's keyframes + EDD
     * on PL00's skeleton. SCD Plc_motion(0, N, 0) will then index into
     * rbj's clip table (which has the actual cinematic animations like
     * the "Hey!" gesture at clip 15). */
    int rbj_size = 0;
    /* RE15_RBJ lets a debug run point the player's cinematic bank at a different room's
     * RBJ (e.g. RBJ/ROOM1150.RBJ for the Irons kneel) so RE15_CLIP_TEST can play those
     * clips. Default = the ROOM1170 intro bank. */
    const char *rbj_path = getenv("RE15_RBJ");
    char rbj_default[64];
    if (!rbj_path || !*rbj_path) {
        /* Room-aware default (2026-06-17): the cinematic bank must match the BOOT
         * room (was hardcoded ROOM1170 → booting room1150 used the wrong kneel clips). */
        snprintf(rbj_default, sizeof rbj_default, "RBJ/ROOM%04X.RBJ", boot_room);
        rbj_path = rbj_default;
    }
    uint8_t *rbj_buf = pc_read_shared(rbj_path, &rbj_size);
    /* RE1.5 ships NO standalone RBJ files — every room's cinematic anim lives INSIDE its
     * RDT (@0x5C, now parsed into rdt.animation). Fall back to that byte-true slice when
     * the (optional/debug) standalone file is absent — this is what makes room1150's Irons
     * kneel + any other room's cinematics actually load. `rbj_borrowed` aliases the resident
     * RDT buffer (must NOT be freed). The RE15_RBJ env override still wins for debug runs. */
    int rbj_borrowed = 0;
    if (!rbj_buf && rdt_ok && rdt.animation && rdt.animation_size > 0) {
        rbj_buf = (uint8_t *)rdt.animation;     /* alias into resident RDT — do not free */
        rbj_size = rdt.animation_size;
        rbj_borrowed = 1;
    }
    fprintf(stderr, "[rbj] loading cinematic bank: %s (%d bytes%s)\n",
            rbj_path, rbj_size, rbj_borrowed ? ", from RDT@0x5C" : "");
    /* X-round (2026-05-25): rbj overlay DISABLED. Deep RE of rbj keyframes
     * proved the overlay clips contain NO walk cycle — all "moving" clips
     * have correlated shoulders (both arms doing same motion = gestures)
     * and single-leg knee-lifts (no alternating thigh stride). The original
     * cinematic uses rbj for POSED moments (Speed-baked single-step toward
     * camera) — NOT for visible walking. PL00.edd / em47.edd contain the
     * REAL walk cycle at clip 4 (anti-correlated thighs, ~30° per leg).
     * Reverting to native EDDs gives proper walk animation for both
     * Leon and Elliot. Cost: Plc_motion(0, N) gestures during dialog now
     * use PL00/em47 native clips at N (which DO have multi-frame data
     * per agent table — e.g. PL00 clip 15=24f, 16=16f, 20=42f). */
    /* AC-round (2026-05-25): rbj overlay on BOTH Leon and Elliot for the
     * cinematic. Per bio-lib enum (Megan Grass) PL00.EDD's clip indices
     * are: 0=Walk_Backward, 4=Falling, 12=Damage_Laying, 19=Stairs_Begin,
     * 20=Stairs_Ascend, 22/23=Idle_Caution/Danger. **PL00.EDD has NO
     * forward walk** — Walk_Forward only exists in PL00W*.EDD (weapon
     * track) which RE1.5 may not load. So Leon's walks during cinematic
     * have to come from rbj overlay's clip 15 (cinematic walking pose
     * with pointing arms, baked Speed — perfect for sub02 "Hey wait!"
     * narrative beat per memory phase4_5_13_rbj_byte_layout). */
    /* AE-round (2026-05-26): build composite W01 skel = PL00 bind + W01
     * keyframes. Direct use of W01.EMR caused "scrambled" rendering because
     * W01.EMR has a DIFFERENT bind pose (gun-holding stance with arm raised)
     * than the PL00.MD1 mesh expects. Solution: keep PL00 bind, use W01's
     * keyframe pool for Walk_Forward + Run animation. Build composite ONCE
     * before any rbj overlay (rbj only changes keyframe_data, bind stays). */
    if (w01_ok && skel_ok) {
        w01_skel = skel;   /* copy bone hierarchy + bind pose from PL00 */
        w01_skel.keyframe_data       = w01_skel_raw.keyframe_data;
        w01_skel.keyframe_data_size  = w01_skel_raw.keyframe_data_size;
        w01_skel.keyframe_count      = w01_skel_raw.keyframe_count;
        w01_skel.keyframe_size_bytes = w01_skel_raw.keyframe_size_bytes;
        fprintf(stderr, "[w01-composite] PL00 bind + W01 keyframes: %d bones, %d kf\n",
                w01_skel.bone_count, w01_skel.keyframe_count);
    }
    if (w03_ok && skel_ok) {
        w03_skel = skel;   /* same composite pattern: PL00 bind + W03 keyframe pool */
        w03_skel.keyframe_data       = w03_skel_raw.keyframe_data;
        w03_skel.keyframe_data_size  = w03_skel_raw.keyframe_data_size;
        w03_skel.keyframe_count      = w03_skel_raw.keyframe_count;
        w03_skel.keyframe_size_bytes = w03_skel_raw.keyframe_size_bytes;
        fprintf(stderr, "[w03-composite] PL00 bind + W03 keyframes: %d bones, %d kf\n",
                w03_skel.bone_count, w03_skel.keyframe_count);
    }
    /* Seed the aim-FSM per-clip lengths with the START bank (byte-true default equip = the
     * KNIFE, item 1 -> melee bank W01). The per-frame equip watcher below re-feeds on switch. */
    {
        extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
        uint16_t fcs[14]; int n = (w01_anim.clip_count < 14) ? w01_anim.clip_count : 14;
        for (int i = 0; i < n; i++) fcs[i] = (uint16_t)w01_anim.clips[i].frame_count;
        if (w01_ok) re15_player_set_aim_clip_lens(fcs, n);
    }
    /* The ACTIVE player W bank (equip-dependent; watcher in the main loop switches these). */
    re15_emd_skeleton_t  *wact_skel = &w01_skel;
    re15_emd_animation_t *wact_anim = &w01_anim;
    int                   wact_ok   = w01_ok;

    /* Unify 2026-06-06: keep the PL00 BASE track (PL00.edd clips, incl. clip 22/23
     * injured idle) BEFORE the rbj overlay below overwrites anim/skel. Mirrors PSX
     * asset_psx.c:329-331 (re15_pl00_*). The shared anim selector uses it for the
     * HP-gated injured idle (clip 22 when HP<50 / clip 23 when HP<30). */
    re15_emd_animation_t pl00_anim = {0};
    re15_emd_skeleton_t  pl00_skel = {0};
    int pl00_ok = 0;
    if (skel_ok) { pl00_anim = anim; pl00_skel = skel; pl00_ok = 1; }

    /* Save the BASE (pre-overlay) Elliot skeleton so a cross-room reload re-overlays from
     * base instead of stacking overlays (parity with the pl00_skel base saved above). */
    re15_emd_skeleton_t elliot_base_skel = elliot_skel;
    /* SHARED room-cinematic overlay (enemy_common.c) — the SAME single source of truth the
     * PSX port (re15_load_room_cinematic) and the cross-room reload below use: Leon (overlaid
     * from the clean pl00 base) + Elliot (from his base) + per-room RBJ→enemy rebind
     * (pc_enemy_load force-loads the rebind target, e.g. ROOM1150 prone Irons onto em45). */
    if (rbj_buf) {
        re15_apply_room_cinematic(rbj_buf, (size_t)rbj_size, boot_room,
                pl00_ok ? &pl00_skel : NULL, &skel, &anim,
                &elliot_base_skel, elliot_skel_ok, &elliot_skel, &elliot_anim,
                &s_cine_scratch_skel, &s_cine_scratch_anim, pc_enemy_load);
        fprintf(stderr, "[rbj] boot room %04X cinematic overlay: %d clips, %d kf\n",
                boot_room, anim.clip_count, skel.keyframe_count);
    }

    /* (em21 special load removed 2026-06-14f — type 0x21 crows now load via the GENERIC
     * enemy loader pc_enemy_load(0x21) → EMD/EM21.EMD into their own render slot, exactly
     * like every other enemy. No per-type special case.) */

    /* RDT (room container) was loaded + parsed EARLY above (before audio/props) so
     * props/footstep/heli/pilot can be sliced from it — globalization Phase 3-A. The
     * `rdt`/`rdt_ok`/`rdt_buf` locals from there drive everything below.
     * (RVD CAM_SWITCH zones are installed in scd_register_room_events() at slot_offset
     * 16 — SCD main00's Door_aot_set(0..6)+Aot_set(7..10) overwrite slots 0-10.) */

    /* Phase 4.4: SCD VM init + start demo thread */
    scd_vm_init();
    /* Byte-true STAGE1 briefing loadout into g_inv (handgun + 2 stacks; savestate-confirmed).
     * scd_vm_init just cleared it; populate the game-start inventory here. (Per-room persistence
     * across a room_unload -> scd_vm_init is a separate concern; the briefing/combat room boots
     * with this. Phase 2b: the full inventory screen renders g_inv + the item classification.) */
    re15_inv_load_briefing();
    /* RE15_EQUIP=<item> (debug harness): equip an item at boot without the menu — input scripts
     * have no START/menu tokens, so deterministic gun probes (ammo chain, discharge fx) need this.
     * The byte-true default stays the briefing knife (aca5d=1, slot 0). */
    {
        const char *eqe = getenv("RE15_EQUIP");
        if (eqe && *eqe) {
            extern void re15_player_set_equipped_weapon(int weapon_id);
            re15_player_set_equipped_weapon((int)strtol(eqe, NULL, 0));
            fprintf(stderr, "[equip] RE15_EQUIP -> item %ld (slot %d)\n",
                    strtol(eqe, NULL, 0), re15_inv_equipped_slot());
        }
    }
    /* Load the item-icon sheet via the cwd-independent asset root (pc_read_shared) and hand it to the
     * engine — re15_asset_read_file only fopen's a raw relative path, which fails when the .exe runs
     * from the build dir (that made the icons blank AND re-tried the open every pixel = dog-slow). */
    {
        int isz = 0;
        uint8_t *ipix = pc_read_shared("DATA/ITEMALL.PIX", &isz);
        if (ipix) re15_itemall_set_pix(ipix, isz);   /* buffer intentionally kept for the program's life */
    }
    /* Item-get modal per-item PICTURE sheet (ITEM/ITPS.ITP, U11) — same cwd-independent root as the
     * icons. The modal draws the 112×72 TIM at id×0x3000 (re15_itps_pixel). */
    {
        int psz = 0;
        uint8_t *ipic = pc_read_shared("ITEM/ITPS.ITP", &psz);
        if (ipic) re15_itps_set_data(ipic, psz);     /* kept for the program's life */
    }
    scd_register_room_events(rdt_ok ? &rdt : NULL);

    /* AW-round 2026-05-28: pre-parse ROOM1170 .msg files for canonical
     * dialog display durations (PSX dialog FSM at 0x80028134 reads embedded
     * text codes 0x01 / 0x04 to set per-message timer). Without this, all
     * subtitles flash for 90 frames (1.5s); ablauf shows 7-15s per message.
     * Lookup is registered with the SCD VM via the msg-duration callback. */
    /* Per-room subtitle text + display durations come from the RDT MESSAGE block
     * (g_room_rdt.messages @ RDT+0x3c) via the shared loader — globalization Phase 3-B
     * (2026-06-13), parity with the PSX engine (asset_psx.c). Replaces the old loop that
     * read 17 separate room1170_msg##.msg files: re15_msg_load_room_block installs the
     * durations + decoded text + the SCD-VM provider for every message in the block
     * (same byte-true .msg bodies, same FUN_80028134 30Hz duration logic). */
    if (g_room_rdt_ok && g_room_rdt.messages)
        re15_msg_load_room_block(g_room_rdt.messages, g_room_rdt.messages_size);

    /* BE-round: load per-cut lighting data (light.lit) so the renderer
     * can modulate vertex tints atmospherically. Without this every cut
     * renders at full (255,255,255) — heli looks bleached, sky cuts
     * lack their dim mood, interior cuts lose the red ambient. */
    /* Per-cut lighting (NCCT) comes from the RDT LIGHT block (g_room_rdt.lights @
     * RDT+0x2c) — globalization Phase 3-B (2026-06-13), parity with PSX (asset_psx.c).
     * Replaces reading a separate room1170_light.lit file (same byte-true bytes). */
    if (g_room_rdt_ok && g_room_rdt.lights &&
        re15_light_parse(g_room_rdt.lights, (size_t)g_room_rdt.lights_size, &g_re15_room_lights) == 0) {
        g_re15_room_lights_ok = 1;
        re15_light_apply_cut(&g_re15_room_lights, 0);
        fprintf(stderr, "[light] RDT: %d cuts, cut 0 tint=(%u,%u,%u)\n",
                g_re15_room_lights.cut_count,
                g_re15_light_tint[0], g_re15_light_tint[1], g_re15_light_tint[2]);
    } else {
        fprintf(stderr, "[light] RDT light block missing — neutral tint\n");
    }

    /* DATA-DRIVEN intro (2026-06-04, keystone parity with the PSX build): set ONLY
     * (3,193,1) — the flag sub03 would set on the (not-yet-implemented multi-room)
     * PRIOR visit. With (3,193)=1 AND (3,125)=0, room1170 main00 itself fires
     * Evt_exec(0x180B) → sub11 (the narrator) through the faithful op_evt_exec path;
     * sub11 then sets (3,125)/(4,242)/(2,7) + Cut_chg(7) + its 4 narrator messages
     * from its OWN bytecode. We no longer force (3,125)/(4,242) nor manually start
     * the narrator. */
    re15_game_flag_set(3, 193, 1);
    /* Außenbereich door hub (2026-06-06): ROOM1170 main00 gates the whole outdoor
     * door set behind `else` of `if(Ck(4,195,0))` — door 6 (return from the outdoor
     * area to the helipad) + door 5 (→ROOM1140) + examine AOTs only register when
     * (4,195)==1. Door 0 (helipad→outdoor) is always on, so (4,195)==0 makes the
     * outdoor area a DEAD END. The original sets (4,195) in ROOM1140/sub02; we stage
     * it (same pattern as (3,193)) so the return door 6 + on-foot door5→1140 exist. */
    re15_game_flag_set(4, 195, 1);

    /* Phase 4.5.12-H: refined position from agent F1's precise reverse-
     * projection. F1 measured Leon as 65 native px (head-to-feet, not
     * head-to-wrist) and solved the floor-constrained projection for
     * cut 0 → world (+1272, -7965, +10898). vz=8166. This position is
     * along sub02's Plc_dest walk path (intermediate waypoint during
     * the helicopter intro), driven by the C-level walk solver, not a
     * static Pos_set/Member_set.
     *
     * AM-round (2026-05-26): Y corrected from -7965 to -7200. Per 30-agent
     * RE: -7965 was misappropriated from sub14's railing-area Pos_set
     * (post-cinematic monologue), but helipad floor (cinematic surface) is
     * Y=-7200 per sub15 NPC spawns (Elliot, pilot, heli all at -7200).
     * Leon was floating 765 units above floor pre-fix. */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x      = 1272;
    g_actors[RE15_ACTOR_SLOT_PLAYER].y      = -7200;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z      = 10898;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
    /* DEBUG room boot (RE15_START_ROOM != 1170): the hardcoded spawn above is the ROOM1170
     * helipad (Y=-7200). For another room use ITS inbound-door spawn (re15_room_spawns) so
     * Leon stands on that room's floor — room1150 floor is Y=0, so Leon was 7200 units below
     * the cut and off-screen (the "missing Leon model"). */
    if (boot_room != 0x1170) {
        int bidx = 0;
        for (int i = 0; i < RE15_ROOM_COUNT; i++)
            if (re15_room_ids[i] == boot_room) { bidx = i; break; }
        const re15_room_spawn_t *bsp = &re15_room_spawns[bidx];
        g_actors[RE15_ACTOR_SLOT_PLAYER].x     = bsp->x;
        g_actors[RE15_ACTOR_SLOT_PLAYER].y     = bsp->y;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z     = bsp->z;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = bsp->yaw;
        fprintf(stderr, "[boot] player spawn for ROOM%04X = (%d,%d,%d) yaw=%d\n",
                boot_room, bsp->x, bsp->y, bsp->z, bsp->yaw);
    }
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp     = 100;   /* RE1.5 max HP (DAT_800acaee
        init 0x64), matching the PSX build — drives the HP-gated injured idle
        (clip22 <50 / clip23 <30). Was 1024 (pre-unify), which never injured. */
    /* Collision floor band from the spawn Y (band = -(Y/0x708); ROOM1170 = 4).
     * RESTORED 2026-06-07 to push-out band 4 (the band-4-FREE complement is walkable;
     * band-4 cells are walls). */
    re15_collision_set_band(re15_collision_band_from_y(g_actors[RE15_ACTOR_SLOT_PLAYER].y));
    /* PARITY STATE-INJECT (RE15_PLAYER_POS="x,z,rot"): teleport the player after spawn so a port
     * run can START from the SAME player pose as a DuckStation savestate — isolates the zombie-AI
     * parity from the (fuzzy vgamepad) input-replay. Reusable parity tool (memory: parity-oracle). */
    {
        const char *pp = getenv("RE15_PLAYER_POS");
        int px = 0, pz = 0, prot = 0;
        if (pp && *pp && sscanf(pp, "%d,%d,%d", &px, &pz, &prot) >= 2) {
            g_actors[RE15_ACTOR_SLOT_PLAYER].x     = px;
            g_actors[RE15_ACTOR_SLOT_PLAYER].z     = pz;
            g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = (int16_t)prot;
            fprintf(stderr, "[parity] player teleported to (%d,%d) rot=%d\n", px, pz, prot);
        }
    }
    /* Phase 4.5.13-A6: user-verified ground truth:
     *   0=walk, 12=fall, 15=climb, 20=march, 22=hurt1, 23=hurt2
     * Try clip 7 (113 frames, wrist at head level per J2 — 2nd-most-
     * likely wave candidate after clip 12 which user confirmed FALL).
     * Press SPACE/BACKSPACE in-game to cycle clips. */
    /* Phase 4.5.13-B1 quick-test: test.emr/test.edd now contain Leon's
     * PL00 skeleton + ROOM1170's animation.rbj cinematic keyframes (671)
     * + rbj's 25-clip EDD. sub02 uses Plc_motion(0, 15, 0) at "Hey!" so
     * motion=15 should now show the ACTUAL dialog gesture (20-frame
     * loop), not PL00's stand-up-from-prone climb. */
    /* Start in IDLE (200 → W01 clip 3, arms-down), NOT bare clip 0: motion 0 is not a
     * locomotion sentinel and would render the DEFAULT bank clip 0 = the boot room's RBJ
     * overlay clip 0 (a gesture, e.g. ROOM1150). 200 is the bank-independent idle. */
    re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER], 200);
    /* Cut 0 = the wide overhead helipad shot used in original.png +
     * sub02's "Hey! Hey wait!" line. Per agent B4's BMP audit cut 0
     * is the wide R-logo helipad with city skyline + neon signs. */
    g_scd.cam_id              = 0;
    g_scd.cam_change_pending  = 1;

    const uint8_t *main_pc = (rdt_ok && rdt.main_scd)
        ? rdt.main_scd
        : scd_fallback_bytecode();
    scd_thread_start(0, main_pc);

    /* Phase 4.5.13-D: ROOM1170 authentic cinematic chain wire-up.
     *
     * The "Hey! Hey wait!" intro (sub02) is gated through this
     * RE2-faithful chain (decoded from ROOM1170 scd/*.c):
     *
     *   main00         registers doors + branches on flags (3,193)+(3,125)
     *   sub00 case 0   goto sub15           (locals[10]==0 at boot ✓)
     *   sub15 Ck(4,242,1)  Evt_exec(0x1802) → sub02 ✓ cinematic
     *
     * Flag (4,242,1) is normally set by sub11 on the player's SECOND
     * room visit (after sub03 sets (3,193,1) on first visit). To play
     * the cinematic on FIRST entry without forcing a multi-room loop,
     * pre-stage the chain flags here — equivalent to "the player has
     * already completed sub03+sub11 on prior visits". The real
     * sub00→sub15→sub02 chain then fires through the unmodified
     * disassembled bytecode (no opcode hacks, no script edits). */
    re15_game_flag_set(3, 193, 1);   /* simulate sub03 already ran (prior visit) */
    /* (3,125) and (4,242) are NO LONGER forced — sub11 sets them itself once main00
     * Evt_execs it; sub00→sub15 then sees (4,242) and spawns the helipad cinematic
     * (sub02). This is the keystone: the SCD data drives the progression. */

    /* sub11 (narrator) is NOT spawned here — main00 itself fires Evt_exec(0x180B)
     * → sub11 (data-driven). sub00 (→sub15→sub02 helipad) is deferred to the main
     * loop until sub11 ends (mirrors the PSX keystone; the door-3 self-reentry the
     * original uses for that handoff is the not-yet-built multi-room mechanism). */

    /* FE-4 CONTINUE resume — apply the loaded save NOW, BEFORE main00 ticks. main00
     * registers doors/AOTs, spawns Sce_em enemies and branches on story flags (Ck),
     * so the saved g_game.flags + character MUST be in place before scd_vm_tick or the
     * room initialises against new-game defaults (wrong doors/enemies/events/camera).
     * The restore overwrites the new-game scaffolding (forced intro flags, default
     * spawn/hp/band set above) with the saved state; the AOT-settle below then primes
     * edge-state from the SAVED player position. (New game: s_resume_pending == 0 → no
     * change to the intro path.) */
    if (s_resume_pending) {
        uint16_t rr = 0;
        re15_savedata_restore(&s_resume_sd, &rr);
        s_resume_pending = 0;
        s_save_counter = s_resume_sd.save_count;   /* DAT_800b0fbd restored from the loaded save */
        /* Re-prime the ARMS SE bank to the restored weapon: the room-init primed the default
         * (bank1), and re15_player_set_equipped_weapon (in restore) sets the weapon id but not the
         * cached SE bank — so a save with a gun equipped would fire the wrong SE without this. */
        { extern void re15_audio_prime_weapon(int weapon_id);
          extern int  re15_player_equipped_weapon(void);
          int wid = re15_player_equipped_weapon();
          if (wid >= 1) re15_audio_prime_weapon(wid); }
        fprintf(stderr, "[save] CONTINUE: resumed in room %04x (hp=%d)\n", rr, g_actors[0].hp);
    }

    /* Phase 4.5.12: prime AOT edge-state from spawn pos so door zones
     * the player materializes inside don't auto-trigger on frame 1.
     * (Tick the SCD VM once first so main00's Door_aot_set / Aot_set
     * registrations are present — and so main00's Evt_exec(0x180B) spawns sub11.) */
    scd_vm_tick();
    re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                       g_actors[RE15_ACTOR_SLOT_PLAYER].z);

    /* main00's boot tick fired Evt_exec(0x180B) → sub11 into an event slot. Detect
     * it: a running narrator means we're in the pre-intro and sub00 (helipad) is
     * held until it ends. If none spawned (flags already advanced), run sub00 now. */
    int s_preintro = 0, s_sub11_slot = -1, s_sub00_spawned = 0;
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
        if (g_scd.threads[s].active) { s_sub11_slot = s; s_preintro = 1; break; }
    }
    if (!s_preintro && rdt_ok && rdt.sub_scd[0]) {
        scd_thread_start(1, rdt.sub_scd[0]);
        s_sub00_spawned = 1;
        re15_audio_start_room_bgm(0, 0x17);
    }

    g_engine.frame_count = 0;


    int sx = 32, sy = 32, sdx = 1, sdy = 1;

    /* `running` is never set to 0 — SDL_QUIT triggers exit(0) inside
     * re15_render_begin_frame — but having a real loop variable avoids
     * the "for(;;) → unreachable return" + "no return from non-void"
     * compiler warning split between MSVC and gcc. */
    volatile int running = 1;
    /* (FE-4 CONTINUE resume is applied above, before scd_vm_tick, so main00 initialises
     * the room from the saved story flags — not new-game defaults.) */

    /* debug: RE15_GIVE_CARD drops a MEMORY CARD in inventory; RE15_SAVE_TEST also fires a
     * save-point this frame (exercise the save flow without navigating to a phone). GIVE_CARD
     * alone lets a REAL phone examine be tested (walk onto the phone AOT + press SQUARE). */
    if (getenv("RE15_GIVE_CARD")) {   /* debug: drop a MEMORY CARD so the consume-per-save path is exercised */
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
            if (g_inv.slots[i].id == 0) { g_inv.slots[i].id = 0x21; g_inv.slots[i].qty = 2; break; }
    }
    if (getenv("RE15_SAVE_TEST")) re15_savepoint_set_pending(1);   /* fire a save-point this frame (card not required) */

    while (running) {
        /* FE-4 phone SAVE (outside the game frame): a save-point phone was examined?
         * gate on the MEMORY CARD item (0x21, RE1.5's ink-ribbon equivalent) — if held,
         * open the save screen and consume one card on a successful save. */
        if (re15_savepoint_pending()) {
            re15_savepoint_set_pending(0);
            /* Examining a save-point phone ALWAYS opens the save screen (replacing the dormant RE1.5
             * "you can save your progress with this — save is not available in this preview" flavor
             * message). The MEMORY CARD (0x21, RE1.5's ink-ribbon equivalent) is consumed per save WHEN
             * held; but since the accessible RE1.5 content has no card pickup, saving is not hard-gated
             * on it (otherwise the whole feature is unreachable). */
            g_scd.message_active = 0;                 /* suppress the flavor message, open the menu */
            int mc = -1;
            for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
                if (g_inv.slots[i].id == 0x21 && g_inv.slots[i].qty > 0) { mc = i; break; }
            re15_savedata_t sd;
            /* Next save number = persistent counter + 1 (DAT_800b0fbd++). Seed from the card's
             * max on the first save of a fresh (non-CONTINUE) session; a CONTINUE already seeded it
             * from the loaded save above, so a load-then-save continues that counter. */
            if (s_save_counter == 0) s_save_counter = (uint16_t)re15_memcard_max_save_count(RE15_CARD_PATH);
            int scount = (int)s_save_counter + 1;
            re15_savedata_capture(&sd, g_engine.frame_count, (uint16_t)scount);
            /* Byte-true: the card (RE1.5's ink-ribbon) consumed by THIS save is part of the saved
             * state — reflect the decrement in the CAPTURED block so reloading doesn't hand it back
             * (the live inventory is decremented on success below). Recompute the checksum. */
            if (mc >= 0) {
                if (sd.inv[mc].qty > 0) sd.inv[mc].qty--;
                if (sd.inv[mc].qty == 0) { sd.inv[mc].id = 0; sd.inv[mc].flags = 0; }
                sd.checksum = re15_savedata_checksum(&sd);
            }
            if (pc_run_memcard_screen(1, &sd, 0) >= 0) {
                s_save_counter = (uint16_t)scount;   /* persist for the next save */
                if (mc >= 0 && --g_inv.slots[mc].qty == 0) { g_inv.slots[mc].id = 0; g_inv.slots[mc].flags = 0; }
                fprintf(stderr, "[save] saved (room %04x); card=%s\n", g_current_room_id, mc >= 0 ? "consumed" : "none");
            }
        }

        re15_render_begin_frame();
        re15_input_tick();

        /* DEBUG: RE15_KILL_AT=<frame> drops the player's HP to 0 at that frame to exercise the
         * death FSM + FE-5.3 death->TITLE mode-cycle deterministically (no combat-path tuning). */
        { static int s_kill_at = -2;
          if (s_kill_at == -2) { const char *k = getenv("RE15_KILL_AT"); s_kill_at = k ? atoi(k) : -1; }
          if (s_kill_at >= 0 && (int)g_engine.frame_count == s_kill_at) g_actors[RE15_ACTOR_SLOT_PLAYER].hp = -1; }  /* dead = hp<0 */

        /* FE-5.1/5.2: track the START-menu pause/inventory in the FE-0 mode machine. The status/
         * inventory screen (re15_menu_toggle) freezes the world — byte-true inline behavior in
         * re15_game_step is unchanged; the mode now reflects it (INVENTORY while up, INGAME while
         * playing). The death block below sets TITLE + exits, which supersedes this. */
        { extern int re15_menu_is_open(void);
          if (re15_gameflow_mode() != RE15_MODE_TITLE)
              g_gameflow.mode = re15_menu_is_open() ? RE15_MODE_INVENTORY : RE15_MODE_INGAME; }

        /* Phase 4.5.6.4: paint cached MDEC BG into the software
         * framebuffer (replaces the gradient when an asset loaded).
         * Match PSX flow: BG first, meshes/HUD layer on top. */
        if (re15_bg_is_loaded()) {
            re15_bg_blit(0, 0);
        } else {
            re15_render_background_gradient(60, 80, 140, 20, 30, 60);
        }

        /* PRE-INTRO → HELIPAD handoff: once main00's narrator (sub11, in event slot
         * s_sub11_slot) ends, fire the FAITHFUL door-3 self-reentry (sub11's Aot_on(3)
         * → door 3, dest = THIS room): re-run the room's init(main00)+main(sub00) with
         * sub11's flags persisting → main00 Ck(3,125,1)→BGM, sub00→sub15 Ck(4,242,1) →
         * Evt_exec(0x1802) helipad cinematic. (Replaces the hand-deferred sub00 spawn.)
         * Start the BGM bank first, then re-enter. Frame cap = safety. */
        if (s_preintro && !s_sub00_spawned &&
            (!g_scd.threads[s_sub11_slot].active ||
             g_engine.frame_count >= (uint32_t)FRAME_AT_60(1800)) &&
            rdt_ok) {
            s_sub00_spawned = 1;
            re15_audio_start_room_bgm(0, 0x17);   /* no BGM under the narrator */
            scd_room_reenter(&rdt,
                             g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                             g_actors[RE15_ACTOR_SLOT_PLAYER].z, 0);  /* helipad scenario */
            fprintf(stderr, "[scd] door-3 self-reentry: re-ran main00+sub00 at frame %d\n",
                    g_engine.frame_count);
        }

        /* (Same-room SCENARIO re-entry consume MOVED into the shared
         * re15_game_step() — it now fires same-frame right after the AOT scan,
         * matching PSX. The old early-consume here ran a frame ahead of the
         * scan, which was the PC/PSX drift.) */

        /* SCD VM ticks at the AH-round 30Hz (pre-BE: tuned timing). At
         * 30fps target SCD ticks every frame. At 60fps target (env override),
         * SCD ticks every 2nd frame so SCD remains 30Hz. */
        if ((target_fps == 30 || (g_engine.frame_count & 1) == 0) && re15_item_modal_active()) {
            /* ITEM-GET MODAL: while the pickup zoom/flip presentation runs, the byte-true freeze
             * (g_pauseflags|=0xff000000) halts the SCD subsystem + walkers + fx here (and game_step
             * early-returns for player/enemy/anim). Advance ONLY the modal FSM at 30 Hz; rendering
             * (incl. the modal quad) keeps running. The pad EDGE drives the state-6 Yes/No prompt. */
            re15_item_modal_tick((uint16_t)g_engine.pad_pressed);
        } else if (target_fps == 30 || (g_engine.frame_count & 1) == 0) {
            scd_vm_tick();
            /* RE15_ITEM_MODAL_TEST: debug — force-start the item-get pickup MODAL once (frame 40) to
             * visually verify the zoom/spin/flip presentation. Overlays don't show in the autoshot BMP
             * (SDL_RenderReadPixels captures only RenderCopy'd textures) — verify via an ffmpeg video. */
            if (getenv("RE15_ITEM_MODAL_TEST") && g_engine.frame_count == 40)
                re15_item_modal_start(0x15, 50, 0, -1);   /* H.GUN BULLETS (ITPS picture id 0x15) */
            /* RE15_ITEM_USE_TEST: debug — seed a Green Medicine as the only item + open the inventory
             * at frame 40 to visually verify the heal-USE prompt. Then SQUARE (start USE) + CROSS (Yes). */
            if (getenv("RE15_ITEM_USE_TEST") && g_engine.frame_count == 40) {
                re15_inv_init();
                g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* Green Medicine, only item */
                re15_menu_toggle();                                  /* open (cursor snaps to slot 0) */
            }
            /* RE15_FORCE_SPLAT: debug — throw one byte-true blood splatter burst at the player at
             * F30 to visually verify the physics (gravity + RNG spread + floor bounce). */
            if (getenv("RE15_FORCE_SPLAT") && g_engine.frame_count == 30) {
                extern void re15_esp_fx_splatter(const re15_esp_t *, uint8_t, int,
                                                 int32_t, int32_t, int32_t, int32_t);
                re15_actor_t *pp = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                re15_esp_fx_splatter(re15_esp_global_bank(), 0, 12,
                                     pp->x, pp->y - 1400, pp->z, pp->y);
            }
            re15_esp_fx_tick(re15_esp_room_bank());   /* Phase ESP-C: advance effect particles (30Hz) */
            /* Walker steps once per 30 Hz SCD tick. (A 2026-06-01 disasm trace
             * suggested the PSX walker runs at 60 Hz → tried 2× stepping, but the
             * USER confirmed 2× FEELS TOO FAST vs PSX — so the PSX position-advance
             * is effectively 30 Hz-content-paced like the Sleeps. Keep 1× = correct
             * wall-clock walk speed.) */
            re15_actor_step_all_walkers();

            /* BL-round 2026-05-29: canonical player-mode + letterbox FSMs (30Hz).
             * ENTER (scripted): while sub02 scripts Leon (its guard flags 1,27/2,7),
             * the player is SCRIPTED (PAD ignored, SCD owns the actor) and the
             * letterbox is held on. PSX enters via Plc_call→mode2; our SCD drives
             * Leon via Plc_motion/Plc_dest, so we enter on these scripting guards. */
            /* EXIT (2026-06-03 audit fix): the handoff is now FLAG-DRIVEN, matching
             * the PSX build and the reduced op_plc_ret (which now only sets
             * player_mode + idle + pc, NOT the letterbox/NPC). sub02 itself does
             * Set(2,7,0)/Set(1,27,0) right before Plc_ret, so the cinematic ends when
             * those guard flags clear; on that set→clear edge we start the orthogonal
             * letterbox close ramp (FUN_80021a0c, 15 frames). Elliot/heli are hidden
             * by camera framing (cut 0x03), not a despawn — see the audit. */
            static int s_cine_was_active = 0;
            int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
            /* Byte-true LETTERBOX counter (FUN_80021a0c @0x80020f34, once per frame): ramps
             * ±0x10 within [0, 0xF0] on the LIVE flag(1,27) bit — the bars' subtractive gray
             * level the renderer draws (bg - level). Replaces the binary 24/0 bar toggle;
             * the countdown below stays as the player_mode HANDOFF timer (its 15 decrements
             * == the 15 level steps). Trace wf_bba41002 (#21). */
            re15_letterbox_tick(re15_game_flag_get(1, 27));
            if (cine_active) {
                g_scd.player_mode         = 2;    /* scripted */
                g_scd.letterbox_countdown = -1;   /* bars held while scripted */
            } else if (s_cine_was_active) {
                g_scd.letterbox_countdown = 15;   /* canonical FUN_80021a0c 0xF0→0 */
            }
            s_cine_was_active = cine_active;
            if (g_scd.letterbox_countdown > 0 &&
                --g_scd.letterbox_countdown == 0) {
                g_scd.player_mode            = 0;
                /* Do NOT zero message_display_frames here. The original's
                 * cinematic-end path (Plc_ret 0x80041f88, Cut_chg, Cut_auto)
                 * never touches the dialog FSM — a subtitle is dismissed SOLELY
                 * by its own end-wait countdown (DAT_800b8525 in FUN_80028134).
                 * Zeroing it on letterbox-close prematurely killed the LAST
                 * message of a cinematic (e.g. sub14's main16), which — unlike
                 * the earlier lines — has no successor Message_on to re-arm the
                 * display. The message's own duration now governs its lifetime,
                 * exactly like the PSX. */
                re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                                   g_actors[RE15_ACTOR_SLOT_PLAYER].z);
                fprintf(stderr, "[scd F%u] letterbox closed -> gameplay (player_mode=0)\n",
                        g_engine.frame_count);
            }
        }

        /* Phase 4.6.1: drain SCD audio events into the SDL audio backend. */
        re15_audio_tick();

        /* Phase 4.3 legacy: TIM atlas blit (top-right corner) — used to
         * be the "we loaded a texture" debug indicator. Now that the
         * skeletal Leon renders fully via SDL_RenderGeometry sampling the
         * same TIM, the atlas-preview overlap was confusing — disabled
         * to mirror the PSX side (see psx_dev/re15_reborn/main.c:77). */
        (void) tim_ok;
        /* if (tim_ok) re15_tim_blit_pc(&tim, SCREEN_XRES - 128, 8); */

        /* Phase 4.2 demo: bouncing square */
        if (sx < 0 || sx > (SCREEN_XRES - 24)) sdx = -sdx;
        if (sy < 0 || sy > (SCREEN_YRES - 24)) sdy = -sdy;
        sx += sdx;
        sy += sdy;
        /* I-round disable: bouncing orange square — leftover demo cruft. */
        /* re15_render_tile(sx, sy, 24, 24, 1, 255, 128, 0); */
        (void)sx; (void)sy;

        /* Title silenced too for clean cinematic view (was over UI). */
        /* re15_debug_text(8, 16, 0, "RE1.5 Rebuilt"); */

        /* Subtitle text (the one HUD line we actually want during cinematic).
         * Lifetime + dismiss are driven by the SHARED re15_msg_tick (msg_common.c) so
         * PSX and PC behave identically — a message stays up for its own duration and
         * is dismissed only when that expires (sub14's last line main16 no longer
         * gets cut short). The only per-port code is the glyph draw below. */
        {
            extern int re15_render_pc_msg_text(int, int, const unsigned char *, int);
            const unsigned char *raw = 0;
            int raw_len = 0, msg_id = 0;
            int live = re15_msg_tick(&raw, &raw_len, &msg_id);
            if (live) {
                /* Render the raw .msg body with the real TEX.TIM glyph font in the
                 * per-speaker colour (0x05 code → CLUT row), at the RE'd box origin
                 * (34,180) for the 0x300 dynamic Message_on. Fall back to the decoded
                 * ASCII string + 6x8 debug font only if the font/raw is unavailable. */
                int drawn = (raw && raw_len > 0) &&
                            re15_render_pc_msg_text(34, 180, raw, raw_len);
                if (!drawn) {
                    const char *subtitle = re15_msg_get_text(msg_id);
                    if (!subtitle) subtitle = re15_to_re2_room1170_subtitle(msg_id);
                    if (subtitle) re15_debug_text(8, SCREEN_YRES - 32, 0, subtitle);
                }
                /* PAGE BREAK (FSM state 1): blinking down-arrow = press action for next page. */
                if (g_scd.message_fsm == 1 && (g_scd.message_blink & 0x18)) {
                    extern void re15_render_pc_down_arrow(int x, int y);
                    re15_render_pc_down_arrow(160, 222);
                }
            }
            /* YES/NO selection prompt + blinking '>' cursor (mirrors the PSX path;
             * byte-true layout from FUN_80028134 state 4: Yes col 160, No col 230, row
             * 196). Option glyph codes are the .msg charset: "Yes"={0x35,0x41,0x4F},
             * "No"={0x2A,0x4B}. The cursor is a '>' triangle (the original's menu-font
             * glyph 0x26 = an 8×8 TEX.TIM sub-region we don't load) and blinks per the
             * state-4 timer (visible when message_blink & 0x18). */
            if (g_scd.message_select) {
                /* Byte-true positions (FUN_80028134 state 4): cursor cell X =
                 * choice*0x46 + 0xa0 = 160 (Yes) / 230 (No); options at 0xae=174 / 244;
                 * row 196. Each option sits 14px right of its cursor. */
                extern void re15_render_pc_cursor(int x, int y);
                static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4F };
                static const unsigned char no_g[2]  = { 0x2A, 0x4B };
                re15_render_pc_msg_text(174, 196, yes_g, 3);
                re15_render_pc_msg_text(244, 196, no_g,  2);
                if (g_scd.message_blink & 0x18)
                    re15_render_pc_cursor((g_scd.message_choice ? 230 : 160), 196);
            }
        }

        /* INVENTORY / STATUS SCREEN (Phase 8.23). The real RE1.5 inventory is a 3-column status screen
         * (framebuffer ground truth = stage_saves/mzd_inv_open.sav): LEFT = ID card + CONDITION/ECG +
         * ITEM/MAP/FILE/EXIT tabs; CENTER = ARMS CONTROL (equipped weapon); RIGHT = ITEM LIST (the 2-col
         * item grid). Chrome = faithful-line panels (the byte-true window-frame builder FUN_800467a8 is
         * deferred); the ITEM-LIST + equipped-weapon ICONS + the grid POSITIONS (cells @217/257,58+row*30)
         * + the CONDITION text are byte-true. See RE15_INVENTORY_SUBSYSTEM.md §3. */
        if (re15_menu_is_open()) {
            static unsigned s_inv_frame = 0; s_inv_frame++;   /* ECG scroll phase */
            int n = re15_menu_count(), cur = re15_menu_cursor();
            re15_actor_t *plr = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            uint8_t eqw = (uint8_t)re15_player_equipped_weapon();
            re15_render_tile(0, 0, SCREEN_XRES, SCREEN_YRES, 5, 20, 28, 64);   /* screen background */

            /* ---- LEFT column: ID card + CONDITION + tabs ---- */
            re15_pc_panel(8, 6, 144, 84);
            re15_render_tile(14, 22, 40, 54, 3, 12, 12, 32);                   /* photo (placeholder) */
            re15_debug_text(64, 12, 0, "POLICE");
            re15_debug_text(60, 26, 0, "Leon.S.kennedy");
            re15_pc_panel(8, 94, 144, 66);
            re15_debug_text(14, 98, 0, "CONDITION");
            re15_debug_text(18, 118, 0, "ECG");
            re15_pc_ecg(46, 112, 100, 22, plr->hp, s_inv_frame / 2);          /* FL heart-rate trace */
            {   /* byte-true condition thresholds (idle-FSM @0x8003206c: <0x32 caution, <0x1e danger) */
                const char *cond = plr->hp >= 50 ? "FINE" : plr->hp >= 30 ? "CAUTION" : "DANGER";
                re15_debug_text(48, 138, 0, cond);
            }
            {   static const char *tabs[4] = { "ITEM", "MAP", "FILE", "EXIT" };
                for (int i = 0; i < 4; i++) {
                    int tx = 66 + (i & 1) * 48, ty = 168 + (i >> 1) * 18;
                    if (i == 0) re15_render_tile(tx - 4, ty - 2, 44, 12, 3, 48, 72, 140);  /* ITEM = active tab */
                    re15_debug_text(tx, ty, 0, tabs[i]);
                } }

            /* ---- CENTER: ARMS CONTROL ---- Equip Arms = the equipped weapon's 40×30 grid icon, native
             * (BYTE-TRUE, inventory-composite-icon-re workflow: ITEMALL tile==id reproduces the Equip box
             * with ZERO mismatch, same tile+CLUT as ITEM LIST — the equip icon is NOT the 88×136 composite).
             * "Standard Arms" IS the larger composite (descriptor @0x80074a8c, a separate 4-bit-atlas
             * subsystem) — shown as the grid icon = FAITHFUL-LINE (its atlas was unresolvable from the
             * capture; deferred). */
            re15_pc_panel(156, 6, 52, 152);
            re15_debug_text(158, 8, 0, "ARMS");
            re15_debug_text(160, 24, 0, "EQUIP");
            if (!re15_pc_draw_item_icon(160, 44, eqw))
                re15_debug_text(168, 52, 0, "--");
            re15_debug_text(158, 96, 0, "STD ARMS");   /* FL: real Standard-Arms art is the 88×136 composite */
            re15_pc_draw_item_icon(160, 112, eqw);

            /* ---- RIGHT: ITEM LIST (byte-true 2-col grid, cells @217/257,58+row*30) ---- */
            re15_pc_panel(212, 6, 104, 152);
            re15_debug_text(214, 8, 0, "ITEM LIST");
            for (int c = 0; c < 10; c++) {
                int cx = 217 + ((c & 1) ? 40 : 0);
                int cy = 58 + (c >> 1) * 30;
                if (c >= n) continue;
                if (c == cur) re15_render_tile(cx - 1, cy - 1, 42, 32, 3, 40, 40, 120);  /* cursor */
                uint8_t id = re15_menu_disp_id(c);
                if (id == 0) continue;                     /* EMPTY grid cell: cursor drawn, no icon/name */
                if (!re15_pc_draw_item_icon(cx, cy, id)) {
                    char t[8]; snprintf(t, sizeof t, "%02X", id);
                    re15_debug_text(cx + 2, cy + 2, 0, t);
                }
                uint8_t q = re15_menu_disp_qty(c);
                if (q > 0) { char t[8]; snprintf(t, sizeof t, "%d", q); re15_debug_text(cx + 22, cy + 20, 0, t); }
            }

            /* (No selected-item name/control-hint line: the RE1.5 status screen has no such text —
             * it is a pre-rendered background TIM with baked labels, and the PC-keyboard hints the
             * port used to print here ("Enter=equip / Shift=close") were both invented AND wrong
             * (equip is SQUARE, close is START/cancel — menu_common.c). Removed, audit F3.) */

            /* Item-USE prompt ("Will you use the X?" Yes/No, then "You have used the X") over the grid —
             * byte-true glyph replay in the game font (script 4/5), same layer as the pickup modal. */
            {
                extern void re15_render_pc_item_prompt(int x, int y, int prompt_type, uint8_t item_id, int reveal);
                extern void re15_render_pc_cursor(int x, int y);
                extern int  re15_render_pc_msg_text(int x, int y, const unsigned char *raw, int len);
                uint8_t uid = 0; int uch = 0;
                int up = re15_item_use_prompt(&uid, &uch);
                if (up) {
                    re15_render_pc_item_prompt(34, 180, up, uid, re15_item_use_reveal());   /* 4=use, 5=used */
                    if (up == 4 && re15_item_use_prompt_ready()) {
                        static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4f };  /* "Yes" */
                        static const unsigned char no_g[2]  = { 0x2a, 0x4b };         /* "No"  */
                        re15_render_pc_msg_text(190, 202, yes_g, 3);
                        re15_render_pc_msg_text(234, 202, no_g,  2);
                        re15_render_pc_cursor(uch ? 224 : 180, 203);
                    }
                }
            }
        }

        /* Phase 4.4.3: drain audio queue (consumes events without HUD). */
        scd_audio_event_t evt;
        while (scd_audio_queue_pop(&evt)) (void)evt;

        /* Phase 4.5.10-I: minimal but clear diagnostic — drop the magenta
         * marker (was making bone 0's marker invisible) and trust the
         * per-bone tiles + bbox in the actual mesh-render path. */
        {
            extern int re15_render_pc_dbg_textri_count(void);
            extern int re15_render_pc_dbg_tim_loaded(void);
            extern int re15_render_pc_dbg_min_sx(void);
            extern int re15_render_pc_dbg_max_sx(void);
            extern int re15_render_pc_dbg_min_sy(void);
            extern int re15_render_pc_dbg_max_sy(void);
            (void)re15_render_pc_dbg_textri_count;
        }

        /* Phase 4.5: rotating MD1 wireframe (orthographic projection).
         * Sums all meshes' triangle wires. Phase 4.5.2 will add proper 3D. */
        if (md1_ok) {
            int total_verts = 0, total_tris = 0;
            for (int i = 0; i < md1.mesh_count; i++) {
                total_verts += md1.meshes[i].tri_vertex_count + md1.meshes[i].quad_vertex_count;
                total_tris  += md1.meshes[i].triangle_count;
            }
            (void)total_verts; (void)total_tris;

            /* Phase 4.5.7.3: skeletal multi-mesh render. PC mirror of
             * mesh_psx_render_skeletal — computes per-bone world poses
             * from the EMR keyframe, then renders each MD1 mesh with
             * its bone's matrix applied to vertices before the
             * perspective project step.
             *
             * Wireframe-only initially to match PSX-side checkpoint
             * (textured per-bone CLUT routing comes later). */
            int   cx = SCREEN_XRES / 2;
            int   cy = SCREEN_YRES / 2;

            /* Phase 4.5.9: prefer RDT-parsed cuts; fall back to static. */
            const re15_camera_cut_t *active_cuts = rdt_ok
                ? rdt.cuts : re15_camera_room1100_cuts;
            int active_cut_count = rdt_ok
                ? rdt.cut_count : re15_camera_room1100_cut_count;

            /* RE2-pure cut handling: cuts change ONLY via SCD Cut_chg.
             *
             * AN-round (2026-05-26, post-user-feedback): REVERTED all
             * animator hacks. Per agent-confirmed PSX behavior: cut framing
             * is STATIC from cuts[N] per cut, no per-frame interpolation.
             * The previous snap-to-focus, work_slot tracking, and lerp
             * were symptom-fixing hacks. Box positions appearing to "jump"
             * between cuts was the smoking-gun that proper Member_set /
             * Obj_model_set semantics are wrong somewhere — fixing the
             * camera animator can't compensate for wrong actor positions. */
            static int s_last_cut_idx = -1;
            static int s_fade_frames  = 0;   /* BN-round: cinematic fade-in countdown */
            static int s_intro_faded  = 0;   /* one-shot: fade only on the first room open */
            /* BO-round (Tier-3): active cut's region quad (anchor zone), for the
             * per-object region cull (PSX FUN_8002c18c → FUN_80014368). Refreshed
             * on cut change; props/NPCs outside it are not drawn. */
            static int16_t cam_region_xs[4] = {0}, cam_region_zs[4] = {0};
            static int     cam_has_region   = 0;
            int active_cut_idx = (s_last_cut_idx < 0) ? 0 : s_last_cut_idx;
            { const char *fc = getenv("RE15_FORCE_CUT");   /* TEMP: pin the camera to a cut to see the kneel on-camera */
              if (fc && *fc) { g_scd.cam_id = (uint8_t)atoi(fc); g_scd.cam_change_pending = 1; } }
            if (g_scd.cam_change_pending) {
                /* Apply the cut INSTANTLY. The PSX holds the old cut ~6 frames while it
                 * CD-seeks+reads the new camera-angle BG (FUN_80013c50) — but that is
                 * pure HARDWARE LOAD TIME, not game logic, so the PC legitimately
                 * switches faster (no per-cut load delay). */
                active_cut_idx = (int)g_scd.cam_id;
                if (active_cut_idx >= active_cut_count)
                    active_cut_idx = active_cut_count - 1;
                g_scd.cam_change_pending = 0;
            }
            if (active_cut_idx != s_last_cut_idx) {
                /* BN-round 2026-05-29: on the first helipad cut (room/cinematic
                 * open), trigger the PSX-style fade-in and clear any lingering
                 * pre-intro narrator subtitle (#2 "To escape", timer runs into here).
                 * BO-round (Tier-3 #4): 15 frames = the canonical FUN_80021a0c ramp
                 * (DAT_800b5568 0x00→0xF0 at +0x10/frame = 15 steps), NOT the old
                 * 6-frame ablauf measurement. */
                if (active_cut_idx == 0 && s_last_cut_idx != 0 && s_fade_frames == 0
                    && !s_intro_faded) {
                    s_intro_faded = 1;
                    s_fade_frames = 15;
                    g_scd.message_display_frames = 0;
                }
                s_last_cut_idx = active_cut_idx;
                /* BO-round (Tier-3): refresh the active cut's region quad. */
                cam_has_region = rdt_ok
                    ? re15_rdt_get_region_quad(&rdt, active_cut_idx,
                                               cam_region_xs, cam_region_zs)
                    : 0;
                re15_bg_load_cut(active_cut_idx);

                /* BE-round: switch lighting tint to the new cut. */
                if (g_re15_room_lights_ok) {
                    re15_light_apply_cut(&g_re15_room_lights, active_cut_idx);
                    fprintf(stderr,
                            "[light] cut %d tint=(%u,%u,%u)\n",
                            active_cut_idx,
                            g_re15_light_tint[0], g_re15_light_tint[1],
                            g_re15_light_tint[2]);
                }

                /* AZ-round 2026-05-28: parse sprite.pri for this cut and
                 * push the mask list to the renderer's BG-overdraw layer.
                 * NULL section (pri_offset bytes 0xFFFFFFFF) → no masks,
                 * overdraw disabled for this cut. */
                re15_pri_cut_t pri = {0};
                int pri_n = 0;
                if (rdt_ok && rdt_buf && active_cut_idx >= 0
                    && active_cut_idx < active_cut_count) {
                    pri_n = re15_pri_parse_section(rdt_buf, (size_t)rdt_size,
                                                   active_cuts[active_cut_idx].pri_offset,
                                                   &pri);
                }
                /* sprite.pri FOREGROUND OCCLUSION (2026-06-09): the AZ-round bug
                 * (sampling the main BG cache → ghost sky patches) is fixed — we
                 * now load the cut's dedicated foreground ATLAS (decoded byte-true
                 * from the room's BSS SLD block, VRAM-verified) and feed the real
                 * masks. The overdraw re-blits atlas[srcX,srcY,w,h]→screen[dstX,dstY]
                 * after the actors so railings/boxes occlude them (byte-true to PSX
                 * FUN_800392d4/FUN_80039590). Cuts with no foreground → atlas absent
                 * → overdraw off. */
                extern int re15_pri_load_cut_atlas(int cut_idx);
                int has_fg = re15_pri_load_cut_atlas(active_cut_idx);
                if (has_fg && pri_n > 0) {
                    int sx[RE15_PRI_MAX_MASKS_PER_CUT], sy[RE15_PRI_MAX_MASKS_PER_CUT];
                    int dx[RE15_PRI_MAX_MASKS_PER_CUT], dy[RE15_PRI_MAX_MASKS_PER_CUT];
                    int pw[RE15_PRI_MAX_MASKS_PER_CUT], ph[RE15_PRI_MAX_MASKS_PER_CUT];
                    int pz[RE15_PRI_MAX_MASKS_PER_CUT];
                    int n = pri.mask_count;
                    if (n > RE15_PRI_MAX_MASKS_PER_CUT) n = RE15_PRI_MAX_MASKS_PER_CUT;
                    for (int m = 0; m < n; m++) {
                        sx[m] = pri.masks[m].srcX;          sy[m] = pri.masks[m].srcY;
                        dx[m] = (int16_t)pri.masks[m].dstX; dy[m] = (int16_t)pri.masks[m].dstY;
                        pw[m] = pri.masks[m].width;         ph[m] = pri.masks[m].height;
                        pz[m] = pri.masks[m].depth;
                    }
                    re15_render_pc_set_pri_rects(sx, sy, dx, dy, pw, ph, pz, n);
                } else {
                    re15_render_pc_set_pri_rects(NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0);
                }
                fprintf(stderr, "[pri] cut=%d pri_offset=0x%X masks=%d fg_atlas=%d\n",
                        active_cut_idx,
                        (unsigned)active_cuts[active_cut_idx].pri_offset, pri_n, has_fg);
            }
            re15_camera_view_t cam_view;
            /* BYTE-TRUE DEATH-CAMERA ORBIT (per-frame FUN_80015850 + init FUN_80015284, RE'd
             * wf_32d97217). On death the camera CUTS to a corpse-orbiting pose and slowly circles it:
             *   cam.x = (rcos(yaw)*radius)>>12 + corpse.x ;  cam.z = (rsin(yaw)*radius)>>12 + corpse.z
             *   yaw = (yaw - yaw_step) & 0xfff ;  radius -= dist_step ;  cam.y -= y_step   (PSX-down: cranes UP)
             * with the look-at easing toward the (live) corpse: XZ /60, Y toward corpse.y-400 /20.
             * radius seeds to the room-cam→corpse distance and dollies to 3300 over 3 frames
             * (dist_step=(r-3300)/3, then frozen). Uses the BIOS rcos/rsin/SquareRoot0 the ROM uses
             * (0x80068348/0x800683e8/0x80065f60). The STEADY orbit (yaw=0x80, yaw_step=0xc=12,
             * radius=3300, dist_step=0, y_step=100) is byte-true VERIFIED against the death savestates:
             * yaw=(128-12*frame)&0xfff → frame 108 = 2928, matching RAM (0x800b525c) at counter 109.
             * FLY-IN (FLAG B RESOLVED, savestate-derived — implemented in the INIT block below, s_dc_yaw /
             * s_dc_yaw_step): the ~3-frame fly-in SPIN (yaw_step=0x384=900 + a yaw seeded from the corpse
             * model's facing point P+0x5b4/0x5bc via ratan2-0xd2c) IS reproduced — the seed reduces to
             * (-rot_y - 0xd2c) & 0xfff (=710·forward(rot_y), verified 3/3 death saves) and swings at
             * 900/frame for 3 frames before settling, so the cut + fly-in spin + orbit are all byte-true. Replaces the old fabricated {0x1f4,0xbb8,0x1f4}+drift glide. */
            re15_camera_cut_t death_cut;
            const re15_camera_cut_t *view_cut = &active_cuts[active_cut_idx];
            {
                static int s_dc_on = 0, s_dc_updates = 0;
                static re15_camera_cut_t s_dc;
                static int32_t s_dc_radius, s_dc_yaw, s_dc_yaw_step, s_dc_dist_step, s_dc_y_step;
                if (g_death_cam) {
                    re15_actor_t *dcp = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    if (!s_dc_on) {                                    /* INIT (path A), once */
                        s_dc = *view_cut;                             /* seed cam.xyz + fov from the room cut */
                        int32_t rdx = s_dc.pos_x - dcp->x, rdz = s_dc.pos_z - dcp->z;
                        s_dc_radius    = (int32_t)re15_squareroot0((uint32_t)(rdx*rdx + rdz*rdz));
                        s_dc_dist_step = (s_dc_radius - 3300) / 3;    /* PHASE A: dolly radius to 3300 in 3 frames */
                        s_dc_y_step    = (s_dc.pos_y - (dcp->y - 400)) / 3;  /* PHASE A: dolly cam.y to corpse.y-400 */
                        /* FLY-IN yaw SEED (FLAG B RESOLVED, savestate-derived): RE1.5 seeds
                         * yaw = ratan2(dz,dx) - 0xd2c where (dx,dz) = the corpse model's facing
                         * endpoint (P+0x5b4/0x5bc) minus the corpse = 710·(cos(rot_y),-sin(rot_y)),
                         * i.e. the player's own rot_y at death (confirmed: 3/3 death saves match
                         * 710·forward(rot_y) to ±1u). ratan2 of that forward = -rot_y, so the seed
                         * is (-rot_y - 0xd2c) & 0xfff. Verified: combat_death rot_y=1199 → seed 3621,
                         * minus 3×900 fly-in = 921 = the RAM yaw (0x800b525c) at phase 5. */
                        s_dc_yaw       = (-(int32_t)dcp->rot_y - 0xd2c) & 0xfff;
                        s_dc_yaw_step  = 0x384;                       /* 900/frame fast fly-in swing (phase A) */
                        s_dc.target_x  = dcp->x;                      /* look-at seed = corpse (no offset) */
                        s_dc.target_z  = dcp->z;
                        s_dc.target_y  = dcp->y - 400;
                        s_dc_updates   = 0;
                        s_dc_on = 1;
                    }
                    /* PER-FRAME UPDATE (FUN_80015850) */
                    s_dc_radius -= s_dc_dist_step;
                    s_dc.pos_y  -= s_dc_y_step;
                    s_dc.pos_x   = ((re15_rcos(s_dc_yaw) * s_dc_radius) >> 12) + dcp->x;
                    s_dc.pos_z   = ((re15_rsin(s_dc_yaw) * s_dc_radius) >> 12) + dcp->z;
                    s_dc_yaw     = (s_dc_yaw - s_dc_yaw_step) & 0xfff;
                    s_dc.target_x += (dcp->x - s_dc.target_x) / 60;   /* look-at ease /60 XZ */
                    s_dc.target_z += (dcp->z - s_dc.target_z) / 60;
                    s_dc.target_y += (dcp->y - (s_dc.target_y + 400)) / 20;  /* /20 Y, +400 head */
                    s_dc_updates++;
                    if (s_dc_updates == 3) {                          /* PHASE A -> B (settle), ONCE after the
                                                                       * 3-frame dolly + fly-in swing (block @0x8001572c) */
                        s_dc_dist_step = 0;                           /* freeze radius at ~3300 */
                        s_dc_y_step    = 0x64;                        /* 100/frame steady crane-up */
                        s_dc_yaw       = 0x80;                        /* 128 constant reset (ends the fly-in) */
                        s_dc_yaw_step  = 0xc;                         /* 12/frame steady orbit */
                    }
                    death_cut = s_dc;
                    view_cut  = &death_cut;
                } else if (s_dc_on) {
                    s_dc_on = 0; s_dc_updates = 0;                    /* revive/reload -> normal cam */
                }
            }
            re15_camera_build_view(view_cut, &cam_view);

            /* sprite.pri depth gate: project the player into camera + screen space
             * (same scale as the per-tri avg_z / mask depth<<3) so a mask BEHIND him
             * that he OVERLAPS on screen is skipped — he stops being occluded by
             * foreground he stands in front of, without dropping foreground elsewhere. */
            {
                re15_actor_t *plz = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                long pvx = ((long)plz->x * cam_view.rot[0] + (long)plz->y * cam_view.rot[1]
                          + (long)plz->z * cam_view.rot[2]) / 4096 + cam_view.trans[0];
                long pvy = ((long)plz->x * cam_view.rot[3] + (long)plz->y * cam_view.rot[4]
                          + (long)plz->z * cam_view.rot[5]) / 4096 + cam_view.trans[1];
                long pvz = ((long)plz->x * cam_view.rot[6] + (long)plz->y * cam_view.rot[7]
                          + (long)plz->z * cam_view.rot[8]) / 4096 + cam_view.trans[2];
                int psx = 160, psy = 120;
                if (pvz > 64) {
                    psx = 160 + (int)(pvx * cam_view.fov_screen_dist / pvz);
                    psy = 120 + (int)(pvy * cam_view.fov_screen_dist / pvz);
                }
                re15_render_pc_set_pri_player(psx, psy, (int)pvz);
            }

            /* Helicopter rotor (BGM SUB layer = SsSeq slot 1): on/off is driven by the
             * SCD's Sce_bgm_control (0x54) opcode (SsSeqPlay at the heli/sky cuts,
             * SsSeqStop during Leon's close-ups). The PER-FRAME distance+azimuth
             * ATTENUATION of the playing layer (FUN_80045a64) is now driven by the SHARED
             * re15_rotor_drive — IDENTICAL to the PSX (this port previously had
             * re15_audio_rotor_update implemented but never called it, so the rotor never
             * faded by distance). Safety: once the cinematic ends (player_mode != 2; heli
             * gone in gameplay), force the SUB layer silent in case the final SsSeqStop
             * was missed by our SCD execution. */
            re15_rotor_drive(&active_cuts[active_cut_idx]);
            if (g_scd.player_mode != 2) re15_audio_rotor_silence();

            /* Shadow orientation is built PER-ACTOR at each shadow draw below via
             * re15_camera_yaw_matrix_angle(actor.rot_y) — byte-true RotMatrixY(actor rot_y), the
             * mechanism RE1.5 FUN_8001b064 actually uses (RE'd wf_13911cba). The former camera-yaw
             * form (built here from the cut forward) was a misread of DAT_800ac784=actor, now gone. */

            /* BJ-round 2026-05-29: cinematic letterbox. PSX cutscenes (the intro)
             * draw ~17px black bars top+bottom (measured from ablauf 217030:
             * content rows 77..1003 of 1080 → ~17px in 240-space), hiding the
             * topmost building/window row that's visible during gameplay
             * (non-cutscene.png). Gate on the cinematic flags sub02 sets. */
            {
                /* Letterbox: now the byte-true FUN_80021a0c counter model — the renderer
                 * reads g_letterbox_level directly (ticked above on the live flag(1,27));
                 * the old binary set_letterbox(24/0) toggle is superseded (#21). */

                /* BO-round: ramp the cinematic fade-in (alpha 255→0 over 15 frames =
                 * canonical FUN_80021a0c ±0x10/frame). PSX fades in from black. */
                {
                    extern void re15_render_pc_set_fade(int a);
                    if (s_fade_frames > 0) {
                        re15_render_pc_set_fade((s_fade_frames * 255) / 15);
                        s_fade_frames--;
                    } else {
                        re15_render_pc_set_fade(0);
                    }
                }
            }
            /* Action-button press edge (Square = Enter on PC). */
            /* --- SHARED interpreter step (commons re15_game_step) ---
             * The canonical PSX-order: action latch -> stair traversal OR
             * (player move + SCA/object collision) -> door/stair AOT scan ->
             * same-room scenario re-entry consume -> fired-event dispatch.
             * IDENTICAL on PSX and PC (game_step_common.c) so the interpreter
             * cannot drift between the ports; only render/input/audio/asset and
             * the SCD-tick cadence stay per-port. */
            {
                re15_game_ctx_t gctx;
                gctx.rdt         = &rdt;
                gctx.rdt_ok      = rdt_ok;
                gctx.pl00_skel   = &pl00_skel;
                gctx.pl00_anim   = &pl00_anim;
                gctx.w01_anim    = &w01_anim;   /* walk-source = footstep flags */
                gctx.cam_view    = &cam_view;
                gctx.active_cut  = active_cut_idx;
                /* apply the OPTIONS controller preset (TYPE A = identity → byte-true default). */
                gctx.pad_current = pc_pad_config((uint16_t)g_engine.pad_current);
                gctx.pad_pressed = pc_pad_config((uint16_t)g_engine.pad_pressed);
                re15_game_step(&gctx);
            }
            /* PARITY STATE-LOG (RE15_STATE_LOG=path): append per-tick player pose + each live
             * enemy's AI state so the port run can be diffed NUMERICALLY against the DuckStation
             * savestate (re15_enemy_state.py). One line/tick: F<frame> pad=<hex> PL(x,z,rot,hp)
             * then per zombie [slot type st ss1 dist]. This is the parity oracle's measuring tape. */
            {
                static FILE *s_state_log = NULL; static int s_slog_init = 0;
                if (!s_slog_init) { s_slog_init = 1;
                    const char *lp = getenv("RE15_STATE_LOG");
                    if (lp && *lp) s_state_log = fopen(lp, "w"); }
                if (s_state_log) {
                    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                    extern int re15_player_aim_clip(void);
                    extern int re15_render_pc_dbg_slot_loaded(int slot);
                    {
                        int eqs = re15_inv_equipped_slot();
                        int mag = (eqs >= 0 && eqs < RE15_INV_MAX_SLOTS) ? g_inv.slots[eqs].qty : -1;
                        int bxs = re15_inv_find_item(0x15);
                        int box = (bxs >= 0) ? g_inv.slots[bxs].qty : -1;
                        fprintf(s_state_log,
                                "F%u pad=%04x PL(%d,%d,rot=%d,hp=%d) pst=%d ps1=%d ps2=%d mo=%d ac=%d fx=%d mg=%d bx=%d sl=%d%d%d%d",
                                g_engine.frame_count, g_engine.pad_current,
                                pl->x, pl->z, pl->rot_y, pl->hp,
                                pl->state, pl->sub_state_1, pl->sub_state_2,
                                pl->motion, re15_player_aim_clip(), re15_esp_fx_count(), mag, box,
                                re15_render_pc_dbg_slot_loaded(20), re15_render_pc_dbg_slot_loaded(21),
                                re15_render_pc_dbg_slot_loaded(22), re15_render_pc_dbg_slot_loaded(23));
                    }
                    for (int si = 1; si < RE15_ACTOR_MAX; si++) {
                        re15_actor_t *e = &g_actors[si];
                        if (!e->active || e->type == 0) continue;
                        fprintf(s_state_log, " [%d t=%02x st=%d ss1=%d g=%02x mo=%d af=%d d=%u @(%d,%d,r%d)]",
                                si, e->type, e->state, e->sub_state_1, e->grid_id, e->motion,
                                e->anim_frame, e->ai_dist, e->x, e->z, e->rot_y);
                    }
                    fputc('\n', s_state_log); fflush(s_state_log);
                }
            }
            /* DEATH PRESENTATION (byte-true FUN_8003694c): fade the scene to black over the 0x78 death
             * timer (g_death_fade), then show the YOU DIED graphic (YOUDIED.TIM) for ~2s before the RE
             * continue = reload the current room. (Original: YOU DIED -> title; the port has no title
             * screen, so the continue-reload is the tail — a healthy room clears g_gameover_active.) */
            {
                extern void re15_render_pc_set_fade(int a);
                extern void re15_render_pc_set_white_fade(int a);
                extern void re15_render_pc_set_black_bg(int on);
                extern void re15_render_pc_set_gameover_flyin(int t);
                extern void re15_render_pc_show_gameover(const re15_tim_t *tim);
                extern void re15_render_pc_hide_gameover(void);
                extern void re15_render_pc_show_title(const re15_tim_t *tim);
                extern void re15_render_pc_hide_title(void);
                static re15_tim_t s_youdied = {0};
                static int s_yd_tried = 0;
                /* THE BYTE-TRUE YOU-DIED CHAIN (game_step FSM drives; this block only renders):
                 * white additive flash + heartbeat pulses (g_death_white), flat-black background
                 * (g_death_blackbg), YOU DIED letters flying in over 50 frames (g_death_flyin),
                 * the sub-6 exit fade-to-black (g_death_fade), then the mode-cycle to TITLE. */
                if (re15_player_is_dead() || g_gameover_active) {
                    re15_render_pc_set_fade(g_death_fade);
                    re15_render_pc_set_white_fade(g_death_white);
                    re15_render_pc_set_black_bg(g_death_blackbg);
                    if (g_death_flyin >= 0) {
                        if (!s_yd_tried) { s_yd_tried = 1; int ysz = 0;
                            uint8_t *yb = pc_read_shared("DATA/YOUDIED.TIM", &ysz);
                            if (yb) re15_tim_parse(yb, ysz, &s_youdied); }
                        if (s_youdied.pixels) {
                            re15_render_pc_show_gameover(&s_youdied);
                            re15_render_pc_set_gameover_flyin(g_death_flyin);
                        }
                    }
                }
                if (g_gameover_active) {
                    /* FE-5.3: the YOU-DIED presentation completed (sub-6 ctr 0x6d). The original stops
                     * all audio and leaves the in-game module for the attract/title flow (@0x80015810-38,
                     * gate @0x8001d1e8). Byte-true: hand off to the TITLE MENU via the mode-cycle — set
                     * mode=TITLE and break the game loop; main() jumps back to re_title, where CONTINUE
                     * reloads the last card save (FE-4.3) and NEW GAME restarts. (Replaces the old inline
                     * "reload-the-current-room" stand-in.) */
                    re15_audio_seq_ctl(0, 2);    /* SsSeqStop the 3 BGM/SEQ layers (FUN_80043958) */
                    re15_audio_seq_ctl(1, 2);
                    re15_audio_seq_ctl(2, 2);
                    re15_render_pc_hide_gameover();
                    re15_render_pc_set_gameover_flyin(-1);
                    re15_render_pc_set_black_bg(0);
                    re15_render_pc_set_white_fade(0);
                    re15_render_pc_set_fade(0);
                    /* Reset the world to a fresh-boot base: NEW GAME (re15_gameflow_new_game doesn't
                     * clear these) starts clean; CONTINUE applies its save over the clean base. The
                     * death FSM self-clears when the re-entered game revives the player (game_step:142). */
                    memset(&g_inv,  0, sizeof g_inv);
                    memset(&g_game, 0, sizeof g_game);
                    g_gameflow.mode = RE15_MODE_TITLE;
                    running = 0;
                }
            }
            /* DEBUG: per-tick kneel trace — find the exact frames + camera cut where Leon
             * kneels (motion 10-12), so the autoshot can target the kneel-down vs ablauf4. */
            {
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                if (pl->motion >= 10 && pl->motion <= 12)
                    fprintf(stderr, "[knee] F%u cut=%d mo=%d af=%d frac=%d | pl pos=(%d,%d,%d) active=%d rot=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx,
                            (int)pl->motion, (int)pl->anim_frame, (int)pl->anim_frac,
                            (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->active, (int)pl->rot_y);
                if ((g_engine.frame_count % 30u) == 0u)
                    fprintf(stderr, "[walk] F%u cut=%d mo=%d | pl pos=(%d,%d,%d) rot=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx, (int)pl->motion,
                            (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y);
                /* RE15_MOTRACE: log player motion/af/flags/player_mode EVERY frame (find which
                 * clip drives the standing arm-gesture before the kneel). */
                if (getenv("RE15_MOTRACE"))
                    fprintf(stderr, "[mot] F%u cut=%d mo=%d af=%d flg=0x%02X frac=%d pmode=%d\n",
                            (unsigned)g_engine.frame_count, active_cut_idx, (int)pl->motion,
                            (int)pl->anim_frame, (unsigned)pl->anim_flags, (int)pl->anim_frac,
                            (int)g_scd.player_mode);
            }
            /* DEBUG: force-fire a room SCD event so an AOT-triggered cutscene runs headless
             * for frame-exact comparison vs ablauf4 (RE15_FORCE_EVENT=8 = room1150 sub08
             * Irons-kneel, normally fired by AOT 6 when Leon walks to Irons). Fires once,
             * after the room SCD (main00) has installed AOTs + actors. */
            {
                static int s_fe_init = 0, s_fe_id = -1, s_fe_done = 0;
                if (!s_fe_init) { const char *fe = getenv("RE15_FORCE_EVENT");
                                  if (fe && *fe) s_fe_id = atoi(fe); s_fe_init = 1; }
                /* If RE15_GOTO_ROOM is used, wait until we are actually IN that room
                 * (g_current_room_id) before firing — so the kneel fires AFTER the
                 * cross-room cinematic-bank reload, testing the real door path. */
                const char *fe_goto = getenv("RE15_GOTO_ROOM");
                unsigned fe_target = (fe_goto && *fe_goto) ? (unsigned)strtol(fe_goto, NULL, 16) : 0;
                int fe_room_ok = (fe_target == 0) || (g_current_room_id == fe_target);
                if (s_fe_id >= 0 && !s_fe_done && g_engine.frame_count >= 20 && fe_room_ok) {
                    extern int scd_event_fire(uint8_t);
                    fprintf(stderr, "[force-event] scd_event_fire(%d) at F%u\n",
                            s_fe_id, (unsigned)g_engine.frame_count);
                    scd_event_fire((uint8_t)s_fe_id);
                    s_fe_done = 1;
                }
            }
            /* RE15_GOTO_ROOM=<hex>: debug — auto-queue ONE cross-room change to that
             * room at F30, to test the cross-room cinematic-bank reload headless
             * (e.g. boot 1170 → goto 1150, confirm ROOM1150.RBJ re-overlays). */
            {
                static int s_goto_done = 0, s_goto_id = -1, s_goto_init = 0;
                if (!s_goto_init) { const char *gr = getenv("RE15_GOTO_ROOM");
                    if (gr && *gr) s_goto_id = (int)strtol(gr, NULL, 16); s_goto_init = 1; }
                if (s_goto_id >= 0 && !s_goto_done && g_engine.frame_count >= 30 && !g_room_change.pending) {
                    int idx = 0;
                    for (int i = 0; i < RE15_ROOM_COUNT; i++)
                        if (re15_room_ids[i] == (unsigned)s_goto_id) { idx = i; break; }
                    const re15_room_spawn_t *sp = &re15_room_spawns[idx];
                    g_room_change.pending    = 1;
                    g_room_change.room_id    = re15_room_ids[idx];
                    g_room_change.x = sp->x; g_room_change.y = sp->y; g_room_change.z = sp->z;
                    g_room_change.yaw_4096   = sp->yaw;
                    g_room_change.target_cut = sp->cut;
                    s_goto_done = 1;
                    fprintf(stderr, "[goto] queued room change -> ROOM%04X at F%u\n",
                            (unsigned)s_goto_id, (unsigned)g_engine.frame_count);
                }
            }
            /* DEV ROOM-BROWSER (globalization 2026-06-13): F1 / F2 cycle through every
             * room in the shared tree (re15_room_list.h) by queuing a g_room_change —
             * the SAME consume path below then loads it (RDT + render reset + props +
             * scd_room_reenter + BG + light + msg, all from the RDT). Lets us load ANY
             * of the 240 rooms and see how far it gets. Guarded so it never overrides a
             * door-queued change. Spawn (0,0,0)/cut 0 — may land in a wall, fine for a
             * browse. PC-only, on the FUNCTION keys so it never touches the game keys. */
            {
                extern int re15_input_debug_fkey(int n);
                int rsel = 0;
                if      (re15_input_debug_fkey(2)) rsel = +1;   /* F2 = next room */
                else if (re15_input_debug_fkey(1)) rsel = -1;   /* F1 = prev room */
                if (rsel && !g_room_change.pending) {
                    int idx = 0;
                    for (int i = 0; i < RE15_ROOM_COUNT; i++)
                        if (re15_room_ids[i] == g_current_room_id) { idx = i; break; }
                    idx = (idx + rsel + RE15_ROOM_COUNT) % RE15_ROOM_COUNT;
                    const re15_room_spawn_t *sp = &re15_room_spawns[idx];
                    g_room_change.pending    = 1;
                    g_room_change.room_id    = re15_room_ids[idx];
                    /* Land at the room's authored inbound-door spawn (walkable, correct
                     * band) so you can walk to its doors; (0,0,0) if no inbound door. */
                    g_room_change.x = sp->x; g_room_change.y = sp->y; g_room_change.z = sp->z;
                    g_room_change.yaw_4096   = sp->yaw;
                    g_room_change.target_cut = sp->cut;
                    fprintf(stderr, "[roomsel] -> ROOM%04X (%d/%d) spawn(%d,%d,%d) cut%d\n",
                            g_room_change.room_id, idx + 1, RE15_ROOM_COUNT,
                            sp->x, sp->y, sp->z, sp->cut);
                }
            }
            /* Cross-room DOOR transition (SHARED re15_room_apply_pending,
             * room_common.c): a door to a DIFFERENT room queued g_room_change in
             * the scan above. Identical LOGIC to PSX; the PC ARCH callbacks are
             * a FILE RDT loader + a no-op render reset + the BG decode. (The BG
             * cut names are now room-aware (bg_pc.c re15_bg_room_prefix builds "room%04x" from
             * g_current_room_id; re15_bg_load_cut bg_pc.c:262-264), and per-room props
             * (pc_load_room_prop_set) + the cinematic/enemy model bank (per-dest_room RBJ reload +
             * re15_apply_room_cinematic below) reload too; the destination RDT,
             * collision, SCD + spawn already switch correctly.) */
            {
                extern int  re15_room_load(unsigned);
                extern void re15_room_reset_render_pc(void);
                extern int  re15_bg_load_cut(int);
                unsigned dest_room = g_room_change.room_id;  /* capture before apply consumes it */
                re15_room_apply_ctx_t rc;
                rc.rdt              = &rdt;
                rc.rdt_ok           = &rdt_ok;
                rc.active_cuts      = &active_cuts;
                rc.active_cut_count = &active_cut_count;
                rc.cam_active_cut   = &active_cut_idx;
                rc.cam_view         = &cam_view;
                rc.load_rdt         = re15_room_load;
                rc.reset_render     = re15_room_reset_render_pc;
                rc.load_bg_cut      = re15_bg_load_cut;
                rc.load_props       = 0;   /* PC reloads props INLINE below (the s_room_prop_*
                                            * arrays are local to main → no fn-ptr closure; the
                                            * callback stays NULL, behaviour is parity w/ PSX). */
                rc.load_cinematic   = 0;   /* PC keeps its boot-loaded Elliot/rbj resident (not
                                            * RAM-constrained) → no per-room cinematic reload. */
                if (re15_room_apply_pending(&rc)) {
                    /* BYTE-TRUE DOOR TRANSITION FADE-IN (RE1.5 transition FSM FUN_8001c958 state-1,
                     * RE'd wf_8e6a4d88): a PLAIN door CUTS to black on the warp frame (there is NO
                     * gradual fade-OUT — that path is exclusively the montage/cut flag aca3c&0x8000),
                     * then FADES IN over 6 frames. Kick the byte-true fade channel with the exact
                     * transition args (FUN_800217b0(0x200,-6144,7,0) → ch0, abr2 subtractive, rgb_mask7,
                     * step -0x1800; FUN_800216ec → level 0x7fff): level += -0x1800/frame, overlay
                     * brightness = level>>7 = 0xFF→0xCF→0x9F→0x6F→0x3F→0x0F→done, drawn by render_pc's
                     * re15_fade_tick, so the new room emerges from black. RE1.5 has NO door-model
                     * animation and NO door SFX in this path (the transition STOPS sound, plays none;
                     * door-open audio is a room-SCD Se_on — see aot_common.c). */
                    /* Kick the exact PSX fade tween (FUN_800217b0(0x200,-6144,7,0) + FUN_800216ec):
                     * ch0, abr2 subtractive, rgb_mask7, step -0x1800 → level 0x7FFF; render_pc's
                     * re15_fade_tick ramps darkness = level>>7 (0xFF→0xCF→…→0x0F→done) over 6 frames so
                     * the new room emerges from black. VERIFIED byte-true + RENDERS CORRECTLY on the PC
                     * screen (a gdigrab screen-recording of the real window shows the door cut to black
                     * then the destination room fade in over exactly 6 game frames). NB: the RE15_AUTOSHOT
                     * BMP tool (SDL_RenderReadPixels) does NOT capture these blend/overlay draws — verify
                     * fades/letterbox by recording the window with ffmpeg, not by autoshot brightness. */
                    re15_fade_config(0, 2, 7, (int16_t)-0x1800, 0);
                    re15_fade_kick(0, 0);

                    /* The shared transition set the door's ENTRY cut in the
                     * frame-local active_cut_idx (via rc.cam_active_cut). The PC
                     * loop re-seeds active_cut_idx from the persistent
                     * s_last_cut_idx at the TOP of every frame, so without
                     * persisting it here the NEXT frame reverts to the previous
                     * room's last cut — leaving the player framed by a camera
                     * that faces away from the new spawn (root behind the
                     * near-plane -> whole mesh culled -> "Leon not loaded").
                     * The PSX loop carries cam_active_cut across frames in the
                     * loop scope, so it never had this bug; here we sync the
                     * static to match. (2026-06-08 cross-room player-missing fix.) */
                    s_last_cut_idx = active_cut_idx;

                    /* PRE-INTRO-Kette ÜBER Räume (Pre-Intro 1240 → Intro 1170):
                     * Nach dem Wechsel in den Intro-Raum hat scd_room_reenter dessen
                     * main00 ausgeführt → Evt_exec(0x180B)=sub11 (Narrator) liegt jetzt
                     * in einem Event-Slot. Den s_preintro-Handoff (Narrator-Ende →
                     * Helipad-Self-Reentry, siehe oben) hier NEU scharfmachen — er war
                     * sonst nur beim Boot aktiv, weshalb das via-Tür eingetretene 1170
                     * zwar den Narrator zeigte, aber nie das Elliot/Pilot-Helipad-
                     * Cinematic startete. `rdt` ist hier bereits der neue Raum (1170). */
                    s_preintro = 0; s_sub11_slot = -1; s_sub00_spawned = 0;
                    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
                        if (g_scd.threads[s].active) {
                            s_sub11_slot = s; s_preintro = 1; break;
                        }
                    }
                    g_engine.frame_count = 0;   /* Frame-Cap des Handoffs relativ zum Eintritt */
                    /* Refresh the per-cut region-quad cull for the NEW room/cut (same
                     * as the PSX fix): next frame the cut-change block sees no change
                     * (entry cut == last) so it won't refresh, leaving the player +
                     * props culled against the PREVIOUS room's stale region quad
                     * ("Leon invisible until you walk to a camera change"). */
                    cam_has_region = rdt_ok
                        ? re15_rdt_get_region_quad(&rdt, active_cut_idx,
                                                   cam_region_xs, cam_region_zs)
                        : 0;
                    /* Data-driven per-room props (parity with PSX re15_load_room_props):
                     * reload the destination room's Obj_model_set prop set so room1140
                     * shows ITS prop, not room1170's box. */
                    pc_load_room_prop_set(&rdt, s_room_prop_md1, s_room_prop_ok);   /* &rdt is synced to the dest room here */

                    /* 2026-06-17 FIX — ROOM-AWARE CINEMATIC BANK. The boot RBJ overlay
                     * (ROOM1170 by default) was kept RESIDENT across rooms (old
                     * rc.load_cinematic=0), so room1150's Plc_motion(0,10/11) indexed
                     * ROOM1170's clips → Leon did a helipad point/reach instead of the
                     * Irons kneel ("streckt die Hand aus, bevor er sich beugt"). Each
                     * room's cinematics live in ITS RBJ → reload + re-overlay Leon
                     * (and Elliot) from the PRESERVED base PL00 on every room change. */
                    {
                        static uint8_t *s_room_rbj = NULL;   /* keep alive: parse_rbj refs it */
                        static unsigned s_rbj_room  = 0xFFFFFFFFu;
                        if (dest_room != s_rbj_room) {
                            char rpath[64];
                            snprintf(rpath, sizeof rpath, "RBJ/ROOM%04X.RBJ", dest_room);
                            int rsz = 0;
                            uint8_t *rbuf = pc_read_shared(rpath, &rsz);
                            /* RE1.5 has no standalone RBJ — slice the dest room's cinematic anim
                             * from its now-resident RDT (@0x5C, rdt.animation; apply_pending above
                             * updated the local `rdt` in-place to dest_room via rc.rdt = &rdt).
                             * Borrowed alias into the resident RDT buffer → keep s_room_rbj=NULL so
                             * it is never freed. */
                            int rbj_borrowed = 0;
                            if ((!rbuf || rsz <= 0) && rdt_ok &&
                                rdt.animation && rdt.animation_size > 0) {
                                rbuf = (uint8_t *)rdt.animation;
                                rsz  = rdt.animation_size;
                                rbj_borrowed = 1;
                            }
                            if (rbuf && rsz > 0) {
                                if (s_room_rbj) free(s_room_rbj);
                                s_room_rbj = rbj_borrowed ? NULL : rbuf;
                                s_rbj_room = dest_room;
                                /* SHARED overlay (enemy_common.c) — identical math to the PSX
                                 * re15_load_room_cinematic: Leon (from pl00 base) + Elliot (from
                                 * his base) + per-room enemy rebind (pc_enemy_load force-loads). */
                                re15_apply_room_cinematic(rbuf, (size_t)rsz, dest_room,
                                        pl00_ok ? &pl00_skel : NULL, &skel, &anim,
                                        &elliot_base_skel, elliot_skel_ok, &elliot_skel, &elliot_anim,
                                        &s_cine_scratch_skel, &s_cine_scratch_anim, pc_enemy_load);
                                fprintf(stderr, "[rbj] room %04X cinematic overlay: %d clips, %d kf\n",
                                        dest_room, anim.clip_count, skel.keyframe_count);
                            } else {
                                fprintf(stderr, "[rbj] room %04X has no RBJ (%s) — keeping current bank\n",
                                        dest_room, rpath);
                            }
                        }
                    }
                }
            }
            /* PC-ONLY DEV TOOL (NOT gameplay; architecture-irrelevant): the motion-lock + PL00 clip
             * cycler. Applied AFTER the shared step so it overrides SCD/Plc_motion for visual clip
             * identification. GATED behind RE15_MOTION_DEBUG — it reads SELECT (0x0001) and the
             * TRIANGLE/CIRCLE bits (0x1000/0x2000), which are now the real Select and △/○ face
             * buttons, so left ungated it would hijack them during normal play (△/○ would cycle the
             * player animation). */
            {
                extern int re15_input_debug_fkey(int n);
                static int  s_motion_dev  = -1;
                if (s_motion_dev < 0) s_motion_dev = getenv("RE15_MOTION_DEBUG") ? 1 : 0;
                static int  s_motion_lock = 0;
                static int  s_locked_clip = 0;
                if (s_motion_dev && re15_input_debug_fkey(3)) {   /* F3 = lock toggle */
                    s_motion_lock = !s_motion_lock;
                    s_locked_clip = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].motion;
                }
                int cyc = 0;
                if      (s_motion_dev && re15_input_debug_fkey(5)) cyc = +1;   /* F5 = clip + */
                else if (s_motion_dev && re15_input_debug_fkey(4)) cyc = -1;   /* F4 = clip - */
                if (cyc && anim.clip_count > 0) {
                    if (s_motion_lock) {
                        s_locked_clip += cyc;
                        while (s_locked_clip < 0) s_locked_clip += anim.clip_count;
                        while (s_locked_clip >= anim.clip_count) s_locked_clip -= anim.clip_count;
                        re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                              (int16_t)s_locked_clip);
                    } else {
                        re15_player_cycle_motion(cyc, anim.clip_count);
                    }
                }
                if (s_motion_lock && anim.clip_count > 0) {
                    re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                          (int16_t)s_locked_clip);
                }
            }
            /* AOT/INV/NPC HUD lines silenced for cleaner cinematic view. */
            if (g_inv.last_pickup_display_frames > 0) {
                g_inv.last_pickup_display_frames--;
            }
            /* CLIP-TEST mode (2026-05-25): RE15_CLIP_TEST=N forces Leon's
             * motion = N every frame, makes him visible from frame 0, and
             * places him at a known visible position so autoshot captures
             * isolated clip poses for empirical walk-cycle identification.
             * Bypasses SCD's Plc_motion writes. Use together with
             * RE15_AUTOSHOT=1 to dump pose snapshots, or with
             * RE15_AUTOSHOT_SERIES for per-frame inspection.
             *
             * RE15_CLIP_TEST=ALL sweeps clips 0..23 in one run: motion
             * cycles every 100 frames, autoshot fires at +5,+15,+25,+35,+45
             * within each 100-frame window — produces 120 BMPs labelled
             * clip_NN_kfXX.bmp. */
            static int  s_clip_test_inited = 0;
            static int  s_clip_test_id     = -1;
            static int  s_clip_test_all    = 0;
            if (!s_clip_test_inited) {
                const char *ct = getenv("RE15_CLIP_TEST");
                if (ct && *ct) {
                    if (ct[0] == 'A' || ct[0] == 'a') {
                        s_clip_test_all = 1;
                        s_clip_test_id  = 0;  /* start at clip 0 */
                        fprintf(stderr, "[clip-test] RE15_CLIP_TEST=ALL — "
                                "sweeping clips 0..23, 100f each, 5 shots/clip\n");
                    } else {
                        s_clip_test_id = atoi(ct);
                        fprintf(stderr, "[clip-test] RE15_CLIP_TEST=%d active — "
                                "forcing motion=%d, Leon visible from F0\n",
                                s_clip_test_id, s_clip_test_id);
                    }
                }
                s_clip_test_inited = 1;
            }
            if (s_clip_test_id >= 0 && anim.clip_count > 0) {
                int cid;
                if (s_clip_test_all) {
                    /* Cycle clip every 100 frames. set_motion resets
                     * anim_frame to 0 on motion-change, so each clip's
                     * 100-frame window starts fresh. */
                    cid = ((int)g_engine.frame_count / 100) % 24;
                } else {
                    cid = s_clip_test_id;
                }
                if (cid >= anim.clip_count) cid = cid % anim.clip_count;
                /* DEBUG: play a PREV clip (RE15_PREV_CLIP, default 0=idle) for the first 40
                 * frames so the player accumulates a real prev-pose, THEN switch to the test
                 * clip — reproduces the cutscene's <prev>→kneel crossfade (not a 0-prev snap).
                 * Set RE15_PREV_CLIP=4 (walk) or 9 (turn) to test the auto-walk approach. */
                if (!s_clip_test_all && g_engine.frame_count < 40) {
                    const char *pc = getenv("RE15_PREV_CLIP");
                    cid = (pc && *pc) ? atoi(pc) : 0;
                }
                /* Re-assert each frame so SCD/Plc_motion cannot override. */
                re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                      (int16_t)cid);
                /* Park Leon at a well-framed spot for the helipad cut 0
                 * wide shot. AM-round: Y -7965→-7200 (helipad floor). */
                g_actors[RE15_ACTOR_SLOT_PLAYER].x = 1272;
                g_actors[RE15_ACTOR_SLOT_PLAYER].y = -7200;
                g_actors[RE15_ACTOR_SLOT_PLAYER].z = 10898;
                /* Face camera-front-right so limb motion is broadside. */
                g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 1024; /* ~90° */
                g_actors[RE15_ACTOR_SLOT_PLAYER].walk_active = 0;
                /* Force camera back to cut 0 every frame — sub11 fires
                 * Cut_chg(7) at F0 which switches to a black/missing BG
                 * and an off-Leon framing. Pinning to cut 0 keeps the
                 * wide helipad shot Leon is positioned for. Rebuild the
                 * current frame's cam_view so the capture this tick is
                 * already from cut 0 (not next tick). */
                if (active_cut_idx != 0) {
                    g_scd.cam_id             = 0;
                    g_scd.cam_change_pending = 0;  /* consumed inline */
                    s_last_cut_idx           = 0;
                    active_cut_idx           = 0;
                    re15_bg_load_cut(0);
                    re15_camera_build_view(&active_cuts[0], &cam_view);
                }
            }

            /* FORCE_MOTION mode (2026-05-25): RE15_FORCE_MOTION=N is the
             * minimal-impact variant of RE15_CLIP_TEST. It ONLY overrides
             * the player's motion every frame — no teleport, no rot_y
             * change, no visibility override. The cinematic still plays
             * normally; we just pin Leon's clip so we can observe the
             * forced pose during the windows where SCD makes him visible. */
            static int s_force_motion_inited = 0;
            static int s_force_motion_id     = -1;
            if (!s_force_motion_inited) {
                const char *fm = getenv("RE15_FORCE_MOTION");
                if (fm && *fm) {
                    s_force_motion_id = atoi(fm);
                    fprintf(stderr, "[force-motion] RE15_FORCE_MOTION=%d active — "
                            "pinning player motion every frame\n",
                            s_force_motion_id);
                }
                s_force_motion_inited = 1;
            }
            if (s_force_motion_id >= 0 && anim.clip_count > 0) {
                int fid = s_force_motion_id;
                if (fid >= anim.clip_count) fid = fid % anim.clip_count;
                re15_actor_set_motion(&g_actors[RE15_ACTOR_SLOT_PLAYER],
                                      (int16_t)fid);
            }

            /* Phase 4.5.9-D: player state from g_actors[0]. */
            const re15_actor_t *player_ref = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            int32_t model_pos_x = player_ref->x;
            int32_t model_pos_y = player_ref->y;
            int32_t model_pos_z = player_ref->z;
            float   screen_dist = (float)cam_view.fov_screen_dist;

            /* AO9-round (2026-05-26): position-based visibility gate.
             * Previously a hardcoded `frame_count >= FRAME_AT_60(3390)`
             * gate hid Leon until ~F1695 (30fps), causing him to "pop
             * into" the scene 1+ frames AFTER sub02's Cut_chg(0x00) +
             * Member_set teleport (lines 67-73). In the original PSX,
             * the cut switch and position teleport happen on the SAME
             * tick, so Leon is already at his cinematic position when
             * the camera reveals him (ablauf intro00218628 shows him
             * present from frame 1 of the new cut).
             *
             * The frame-window heuristic was fragile because sub02's
             * absolute timing depends on FPS scaling. Position-based
             * gate is robust: Leon's spawn = (1272, ...), sub02 line
             * 10-12 teleports him FAR OFF (-31000, 31000) for dialog
             * cuts, line 71-73 brings him back (2300, ?, 14365) for
             * his appearance. |x| < 25000 captures all on-stage
             * positions; the -31000 hidden state is rejected.
             *
             * I2-round (2026-05-24): per ablauf 001-013, Leon is HIDDEN
             * during sub11 narrator (cut 7) and during sub02's
             * Elliot+Pilot+Heli dialog phase. He only becomes visible
             * when sub02 fires the second Cut_chg(0) (Leon-arrival).
             *
             * Without this gate, Leon stands as a R.P.D. statue in
             * cuts 1/2 where the original game frames him out of view.
             * The real engine relied on positional framing (Leon spawn
             * outside cam frustum) — our coord system doesn't reproduce
             * that exactly, so we use a frame-window heuristic.
             *
             * CLIP_TEST override: force-visible from frame 0 so we can
             * see the pose immediately without waiting 56s of intro. */
            int player_visible;
            if (s_clip_test_id >= 0) {
                player_visible = 1;
            } else {
                /* Per-cut region-quad cull (byte-true FUN_80039ca0→FUN_80014368 vs the
                 * active cut's quad — the SAME test the props use), replacing the eyeballed
                 * `x > -30000` off-stage gate (#18 2026-06-09). Leon is drawn unless his
                 * world XZ is outside the active cut's region quad: the SCD off-stage hide
                 * (Member_set(0,-31000)) lands outside → hidden; the whole on-stage outdoor
                 * area (incl. the staircase x≈-27410) is inside its cut's quad → shown. */
                player_visible = !(cam_has_region &&
                    !re15_aot_point_in_quad(player_ref->x, player_ref->z,
                                            cam_region_xs, cam_region_zs));
            }
            /* No far-clip here. PSX GTE has no upper-bound OTZ check —
             * FUN_80016b54 @0x80016d24 does `bltz v0, skip` (OTZ<0 only).
             * The |x|<25000 proxy above already hides any teleport-hidden
             * actor (sub02 sets x=-31000 before cut 6). The per-vertex
             * _vz<64 near-clip in the projection macro guards div-by-zero.
             * BB-round's 32000 far-clip was based on a false premise and
             * has been removed (BM-round already removed it from props for
             * the same reason). */

            /* SHARED anim selection (unify 2026-06-06): the bank/clip view-model is
             * game logic, so it lives in anim_select_common.c and is IDENTICAL to
             * the PSX build — locomotion 105/100 -> PL00W01 walk/run, idle-fidget
             * 200/210/211/212 -> PL00W01, HP-gated injured idle 213/214 -> PL00
             * base, else the actor's own cinematic bank (motion = direct clip
             * index). Fill the bank table from this build's locals. */
            /* EQUIP WATCHER — the byte-true W-bank switch (loader @0x80036b80: CD file 76+item):
             * items 0-2 -> melee bank (W01 trio), items 3+ -> gun bank (W03 pair). The WHOLE
             * carry set switches (locomotion + aim clips share the 14-clip layout), and the aim
             * FSM gets the new bank's per-clip frame counts (recoil 23f vs draw 15f cadence). */
            {
                extern int  re15_player_equipped_weapon(void);
                extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
                static int s_wgun = -1;
                int want_gun = (re15_player_equipped_weapon() >= 3) && w03_ok;
                if (want_gun != s_wgun) {
                    s_wgun = want_gun;
                    wact_skel = want_gun ? &w03_skel : &w01_skel;
                    wact_anim = want_gun ? &w03_anim : &w01_anim;
                    wact_ok   = want_gun ? w03_ok    : w01_ok;
                    uint16_t fcs[14];
                    int n = (wact_anim->clip_count < 14) ? wact_anim->clip_count : 14;
                    for (int i = 0; i < n; i++) fcs[i] = (uint16_t)wact_anim->clips[i].frame_count;
                    re15_player_set_aim_clip_lens(fcs, n);
                }
            }
            re15_anim_banks_t banks = {
                .def_mesh = &md1, .def_skel = &skel, .def_anim = &anim,
                .w01_skel = wact_skel, .w01_anim = wact_anim, .w01_ok = wact_ok,
                .pl00_skel = &pl00_skel, .pl00_anim = &pl00_anim, .pl00_ok = pl00_ok,
                .elliot_mesh = &elliot_md1, .elliot_skel = &elliot_skel,
                .elliot_anim = &elliot_anim, .elliot_ok = (elliot_ok && elliot_skel_ok),
            };
            re15_anim_view_t av;
            re15_actor_anim_select(player_ref, 1, &banks, &av);
            const re15_emd_skeleton_t  *p_skel = av.skel;
            const re15_emd_animation_t *p_anim = av.anim;
            int p_clip_override = av.clip_override;
            /* LEON GRAB-VICTIM render override (state 5 struggle / state 6 collapse): while a zombie
             * has Leon grabbed (or the grab killed him), pose Leon from the grabbing zombie's EMD
             * BANK 2 (the grab-victim set) at player->motion (the struggle/collapse clip the victim
             * FSM set). Byte-true: @0x8010a28c/@0x8010a6f8 animate the PLAYER from DAT_800acbcc/acbd0
             * = *(zombie+0x178)/+0x17c (bank 2), NOT his own PL00 set. This is what stops Leon from
             * freezing during the grab/death (the "no Leon reactions / death finish missing"). */
            if (re15_player_victim_state() != 0 && pl00_ok) {
                re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
                if (vb && vb->victim_ok && vb->anim_victim.clip_count > 0) {
                    /* Pose Leon with HIS OWN structure (PL00 bone hierarchy + bind = his proportions)
                     * but bank 2's keyframe POSES + clip table. The original renders the player with
                     * the player entity's skeleton; the victim EDD/EMR only supply the clips+poses. */
                    static re15_emd_skeleton_t s_victim_skel;
                    s_victim_skel = pl00_skel;                              /* Leon's bones + bind pose */
                    s_victim_skel.keyframe_data       = vb->skel_victim.keyframe_data;   /* bank2 poses */
                    s_victim_skel.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
                    s_victim_skel.keyframe_count      = vb->skel_victim.keyframe_count;
                    s_victim_skel.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
                    p_skel = &s_victim_skel;
                    p_anim = &vb->anim_victim;
                    /* player->motion IS the bank-2 clip the victim FSM set (struggle 0-5 / collapse
                     * 6-7). re15_compute_actor_kf takes clip_override>=0 as the LITERAL clip index and
                     * loops slot = anim_frame % frame_count — so pass motion, NOT a bare 1 (that pinned
                     * Leon to clip 1: the collapse never rendered + behind-grab showed the wrong pose;
                     * caught by the byte-true render-path verify, invisible to the motion-value test). */
                    p_clip_override = (int)player_ref->motion;
                }
            }
            /* AIM render override: while aiming, Leon poses from the EQUIPPED W bank — his own
             * bones + the weapon pool, same pattern as the grab-victim override. Melee (items
             * 0-2, bank W01): draw clip 0xD (FUN_80035538); gun (items 3+, bank W03): the gun-FSM
             * clip (6 raise / 8|10|12 hold / 7|9|11 recoil). */
            {
                extern int re15_player_aim_active(void);
                if (re15_player_aim_active() && wact_ok &&
                    re15_player_victim_state() == 0) {
                    extern int re15_player_aim_clip(void);
                    p_skel = wact_skel;              /* composite: PL00 bones + active W pool */
                    p_anim = wact_anim;
                    p_clip_override = re15_player_aim_clip();
                }
            }
            int kf_idx = 0;
            if (player_visible && skel_ok && p_anim->clip_count > 0) {
                /* The platform owns the fps policy: at 30fps target anim_frame is
                 * already 30Hz; at 60fps halve to PSX-canonical 30Hz. */
                uint32_t cur = (target_fps == 30)
                    ? (uint32_t)player_ref->anim_frame
                    : ((uint32_t)player_ref->anim_frame >> 1);
                kf_idx = re15_compute_actor_kf(p_anim, p_skel, player_ref,
                                               p_clip_override, cur);
            }
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            int pose_ok = 0;
            if (player_visible && skel_ok) {
                g_anim_pose_actor = player_ref;   /* FRAC crossfade for the player body */
                pose_ok = (re15_skel_compute_pose(p_skel, kf_idx, poses) == 0);
                /* RE15_POSE_DUMP: LEON render-level pose (grab-start / release investigations) */
                {
                    static FILE *s_lp = NULL; static int s_lp_tried = 0;
                    if (!s_lp_tried) { s_lp_tried = 1;
                        const char *pd = getenv("RE15_POSE_DUMP");
                        if (pd && *pd) { char pb[512]; snprintf(pb, sizeof pb, "%s.leon", pd); s_lp = fopen(pb, "w"); }
                    }
                    if (s_lp && pose_ok) {
                        fprintf(s_lp, "[leon] F%u vs=%d mo=%d af=%d frac=%d kf=%d ovr=%d rot=%d "
                                "b9(%d,%d,%d) b13(%d,%d,%d)\n",
                                (unsigned)g_engine.frame_count, re15_player_victim_state(),
                                player_ref->motion, player_ref->anim_frame, player_ref->anim_frac,
                                kf_idx, p_clip_override, (int)player_ref->rot_y,
                                (int)poses[9].trans[0], (int)poses[9].trans[1], (int)poses[9].trans[2],
                                (int)poses[13].trans[0], (int)poses[13].trans[1], (int)poses[13].trans[2]);
                        fflush(s_lp);
                    }
                }
            }
            /* Per-bone composed matrix `view × bone_world` (Q12 rotation
             * + world-unit translation, float copies for in-macro math).
             * Set inside the mesh loop below by re15_camera_compose_
             * view_bone. */
            int32_t bone_m[9] = {4096,0,0,  0,4096,0,  0,0,4096};   /* Q12 view×bone (was float; byte-true RTPS) */
            int32_t bone_t[3] = {0, 0, 0};
            /* bone-11 composed matrix capture (the weapon-in-hand attach bone) */
            int32_t wpn_bone_m[9], wpn_bone_t[3];
            int32_t wpn_yawed[9];
            int     wpn_bone_valid = 0;

/* Phase 4.5.8.2: bone_m / bone_t now hold view × bone_world. Vertex maps
 * directly into camera space — no extra MESH_POS_Z hack. Perspective
 * divide uses screen_dist from the active cut. out_wz emits the
 * camera-space z for back-to-front sort (Phase 4.5.7.7). */
/* H28 fix (2026-05-24): set out_wz to a negative sentinel for behind-near
 * plane verts so callers can skip the tri (PSX gte_nclip equivalent). Was
 * clamping _vz to 1.0 which projected behind-camera verts to ±million
 * pixels, producing exploded triangles + wraparound + CLUT smear. Near
 * plane bumped from 1 to 64 to also kill the GTE-overflow case noted in
 * bugfix_psx_canonical_pipeline_2026_05_18. */
/* Byte-true GTE RTPS (integer, replaces the old float divide+transform — the last pixel-shift
 * source): view = (bone_m·v)>>12 + bone_t; IR1/IR2 sat s16, SZ3 sat u16; sx = OFX + (IR·n)>>16
 * with n = the GTE UNR divide (re15_gte_divide). H = fov_screen_dist. */
#define PROJECT_VERT(vp, out_sx, out_sy, out_wz) do { \
                int32_t _x = (vp)->x, _y = (vp)->y, _z = (vp)->z; \
                int32_t _vx = (int32_t)(((int64_t)_x*bone_m[0] + (int64_t)_y*bone_m[1] + (int64_t)_z*bone_m[2]) >> 12) + bone_t[0]; \
                int32_t _vy = (int32_t)(((int64_t)_x*bone_m[3] + (int64_t)_y*bone_m[4] + (int64_t)_z*bone_m[5]) >> 12) + bone_t[1]; \
                int32_t _vz = (int32_t)(((int64_t)_x*bone_m[6] + (int64_t)_y*bone_m[7] + (int64_t)_z*bone_m[8]) >> 12) + bone_t[2]; \
                if (_vz < 64) { (out_wz) = -1.0f; (out_sx) = 0; (out_sy) = 0; } \
                else { \
                    int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx); \
                    int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy); \
                    uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz; \
                    uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3); \
                    (out_sx) = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16); \
                    (out_sy) = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16); \
                    (out_wz) = (float)_vz; \
                } \
            } while (0)

#define IS_BACKFACE(ax, ay, bx, by, cx_, cy_) \
                (((long long)((bx) - (ax)) * ((cy_) - (ay)) \
                - (long long)((by) - (ay)) * ((cx_) - (ax))) <= 0)
            const int backface_cull = 1;
            (void)backface_cull;  /* wireframe doesn't cull yet — symmetric to PSX wireframe path */

            /* Phase 4.5.9-D: model Y-rotation derived from player rot_y. */
            int32_t face_s = re15_sin_q12((int)player_ref->rot_y);
            int32_t face_c = re15_cos_q12((int)player_ref->rot_y);
            int32_t yaw_rot_q12[9] = {
                 face_c, 0,  face_s,
                 0,      0x1000, 0,
                -face_s, 0,  face_c
            };

            /* RE1.5 CHARACTER SHADOW (FUN_8001b064 + FUN_8001af5c, 2026-05-29).
             * A subtractive textured floor quad under the actor: 4 corners at
             * (±500, 0, ±600) world units (a2=0x1f4 half-width, a3=0x258
             * half-depth), centered on the actor (offset a0=a1=0), at the floor
             * Y, yaw-rotated by the actor facing (RotMatrixY). The PSX builds
             * the corners in the actor's rotated+translated frame and projects
             * via RotAverage4 — we mirror that with re15_camera_compose_view_bone
             * (view × RotY(facing) @ actor world pos) + the standard projection.
             * Floor Y = the actor's own Y (= floorIdx·−0x708 on the helipad,
             * where every actor stands on the −7200 floor). Player-only, matching
             * the PSX caller (player update FUN_80031c44). */
            if (player_visible) {
                /* Half-extents = descriptor +0xc/+0xe (= player entity+0xbc/+0xbe). RAM-CONFIRMED
                 * 500/600 across every alive-player savestate (they grow ONLY at death = the blood
                 * pool below); a workflow finder's "square ~400" claim was wrong — the savestate is
                 * the arbiter. NOT a square. */
                int32_t SH_HALF_X = 500;                /* entity+0xbc, RAM-confirmed */
                int32_t SH_HALF_Z = 600;                /* entity+0xbe, RAM-confirmed */
                /* DEATH BLOOD POOL (LAB_8003694c @0x800369bc-d8): the quad half-extents grow
                 * +0xc per death tick (live terminal 0x7ac/0x810 after ~122 ticks). */
                if (re15_player_is_dead() && g_death_pool > 0) {
                    SH_HALF_X += 12 * g_death_pool;
                    SH_HALF_Z += 12 * g_death_pool;
                }
                int32_t sh_corner[4][3] = {
                    { -SH_HALF_X, 0,  SH_HALF_Z },   /* ecke1 (-X,+Z) → uv(0,0) */
                    { -SH_HALF_X, 0, -SH_HALF_Z },   /* ecke2 (-X,-Z) → uv(1,0) */
                    {  SH_HALF_X, 0,  SH_HALF_Z },   /* ecke3 (+X,+Z) → uv(0,1) */
                    {  SH_HALF_X, 0, -SH_HALF_Z },   /* ecke4 (+X,-Z) → uv(1,1) */
                };
                /* Center on the ACTOR position. FUN_8001b064 centers on the
                 * camera focus cam+0x34/+0x3c, but FUN_800369f8 shows that focus
                 * is the LIVE soft-tracked look-at that lerps toward the character
                 * — so the actor position is its faithful analog. (The STATIC cut
                 * target is the framing point ~4000u off Leon → put the quad above
                 * his feet = shadow "gone"; verified via the projection log.)
                 * Rotation = CAMERA yaw (sh_cam_yaw_rot, = RotMatrixY(cam+0x6a)) —
                 * a camera-aligned quad covers both feet; actor-yaw swung it off
                 * one foot. Floor Y = model_pos_y (the −7200 helipad floor). */
                int32_t sh_world[3] = { model_pos_x, model_pos_y, model_pos_z };
                int32_t sh_rot[9], sh_trans[3];
                /* byte-true: RotMatrixY(PLAYER rot_y) via the trig LUT — FUN_8001b064 @0x8001b0e4
                 * builds the shadow orientation from the ACTOR's entity+0x6a, not the camera yaw
                 * (RE'd wf_13911cba; the "camera yaw" note was a misread of DAT_800ac784=actor). */
                int32_t sh_actor_yaw[9];
                re15_camera_yaw_matrix_angle(g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y, sh_actor_yaw);
                re15_camera_compose_view_bone(&cam_view, sh_actor_yaw, sh_world,
                                              sh_rot, sh_trans);
                int32_t sbm[9], sbt[3];   /* Q12 (byte-true integer RTPS) */
                for (int i = 0; i < 9; i++) sbm[i] = sh_rot[i];
                for (int i = 0; i < 3; i++) sbt[i] = sh_trans[i];
                int sx[4], sy[4], sok = 1;
                for (int v = 0; v < 4; v++) {
                    int32_t _x = sh_corner[v][0], _y = sh_corner[v][1], _z = sh_corner[v][2];
                    int32_t _vx = (int32_t)(((int64_t)_x*sbm[0] + (int64_t)_y*sbm[1] + (int64_t)_z*sbm[2]) >> 12) + sbt[0];
                    int32_t _vy = (int32_t)(((int64_t)_x*sbm[3] + (int64_t)_y*sbm[4] + (int64_t)_z*sbm[5]) >> 12) + sbt[1];
                    int32_t _vz = (int32_t)(((int64_t)_x*sbm[6] + (int64_t)_y*sbm[7] + (int64_t)_z*sbm[8]) >> 12) + sbt[2];
                    if (_vz < 64) { sok = 0; break; }   /* H28 near-clip */
                    int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                    int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                    uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                    uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                    sx[v] = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
                    sy[v] = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
                }
                if (sok) {
                    if (re15_player_is_dead() && g_death_pool > 0)
                        /* BLOOD POOL (byte-true LAB_8003694c): the shadow quad turns dark-red
                         * (subtractive color 0x38/0xff/0xff @0x8003699c-b8) — the SIZE growth
                         * (+0xc/frame) is applied to sh_corner above via g_death_pool. */
                        re15_render_shadow_quad_c(sx[0], sy[0], sx[1], sy[1],
                                                  sx[2], sy[2], sx[3], sy[3],
                                                  0x38, 0xff, 0xff);
                    else
                        re15_render_shadow_quad(sx[0], sy[0], sx[1], sy[1],
                                                sx[2], sy[2], sx[3], sy[3]);
                }
            }

            /* CANONICAL per-bone NCCT lighting (2026-06-02, mirrors the PSX-native
             * mesh_psx.c + FUN_8001e9ec). Build the WORLD-space context ONCE here
             * (actor_rot = NULL → ctx->L stays world-space); the per-bone fold
             * (L_bone = bone_rot^T × L_world) happens inside the bone loop so a
             * posed limb is lit by L_world · N_world, not the body yaw. The old
             * body-only path (actor_rot = yaw_rot_q12, single ctx) was an
             * approximation that mis-shaded articulated bones. */
            re15_actor_lightctx_t lctx_player, lctx_player_world;
            int player_lit = (g_re15_room_lights_ok &&
                              g_re15_active_cut >= 0 &&
                              g_re15_active_cut < g_re15_room_lights.cut_count);
            if (player_lit) {
                /* Canonical (2026-05-29): eval_pos = the actor's OWN world
                 * position, matching PSX FUN_80053fc0(param_1 = actor pos) —
                 * its caller FUN_80039ca0 passes the actor record's position
                 * field (puVar3+0x14), confirmed in RE_15_Quellcode_V2/.
                 * Lighting only runs inside the player_visible gate (|x|<25000),
                 * so a teleport-hidden Leon is never lit — as PSX culls hidden
                 * actors before shading them. */
                int32_t actor_pos_w[3] = {
                    (int32_t)player_ref->x,
                    (int32_t)player_ref->y,
                    (int32_t)player_ref->z
                };
                re15_light_setup_actor(
                    &g_re15_room_lights.cuts[g_re15_active_cut],
                    actor_pos_w, NULL, &lctx_player_world);
            } else {
                memset(&lctx_player, 0, sizeof(lctx_player));
            }

            /* Bind Leon's TIM (slot 0) for player render. */
            re15_render_pc_bind_tim_slot(0);

            /* 1:1 mesh_idx == bone_idx (PL00 convention; meshes 15..16
             * are weapon slots). See mesh_psx.c for the rationale. */
            int n_bones = pose_ok ? skel.bone_count : 0;
            if (n_bones > md1.mesh_count) n_bones = md1.mesh_count;
            for (int bi = 0; bi < n_bones; bi++) {
                /* Phase 4.5.10-L: revert to 1:1 mesh-bone mapping.
                 * Phase 4.5.10-H tried bone_mesh_index[] but that field's
                 * semantics weren't what we thought (it's the EMR child
                 * traversal table, NOT a bone→mesh permutation). Memory
                 * note phase4_5_7_5_textured_skeletal confirms 1:1 is
                 * correct per revengi/BioModels/lib_bio (3 independent
                 * RE2 reimplementations). */
                int mi = bi;

                /* Phase 4.5.8.2 + 4.4.5.1: pre-rotate pose by R_y(yaw),
                 * then compose view × (R_y × bone) per bone. */
                const re15_skel_pose_t *p = &poses[bi];
                int32_t yawed_rot[9];
                for (int r = 0; r < 3; r++) {
                    for (int c = 0; c < 3; c++) {
                        int64_t s = 0;
                        for (int k = 0; k < 3; k++) {
                            s += (int64_t)yaw_rot_q12[r*3 + k]
                                 * (int64_t)p->rot[k*3 + c];
                        }
                        yawed_rot[r*3 + c] = (int32_t)(s >> 12);
                    }
                }
                int32_t yawed_trans[3];
                for (int r = 0; r < 3; r++) {
                    int64_t s = 0;
                    for (int k = 0; k < 3; k++) {
                        s += (int64_t)yaw_rot_q12[r*3 + k] * (int64_t)p->trans[k];
                    }
                    yawed_trans[r] = (int32_t)(s >> 12);
                }
                int32_t bone_world_trans[3] = {
                    yawed_trans[0] + model_pos_x,
                    yawed_trans[1] + model_pos_y,
                    yawed_trans[2] + model_pos_z,
                };
                /* BLADE/HAND WORLD POINT capture (byte-true kine layout: the melee SLASH reads
                 * *(0x800acbdc)+0x7b8 = bone-11 matrix translation — per-bone stride 0xAC, matrix
                 * @+0x40, translation @+0x14 -> 11*0xAC+0x40+0x14 = 0x7b8; resolver LAB_80040b88).
                 * Hand the player's bone-11 world origin to the damage resolver each rendered
                 * frame (1-frame-stale vs the PSX in-frame pose pass — faithful-line). */
                if (bi == 11) {
                    extern void re15_player_set_hand_world(int32_t x, int32_t y, int32_t z);
                    re15_player_set_hand_world(bone_world_trans[0], bone_world_trans[1],
                                               bone_world_trans[2]);
                }
                /* CANONICAL per-bone light fold (2026-06-02): rotate the world
                 * light dirs into THIS bone's frame so the raw bone-local normals
                 * shade as L_world · N_world. yawed_rot = R_y(yaw) × pose.rot is the
                 * bone's world rotation (same matrix used for the vertex transform).
                 * Mirrors PSX SetLightMatrix(LLM_world × bone_rot)/bone. */
                if (player_lit)
                    re15_light_ctx_rotate_for_bone(&lctx_player_world, yawed_rot,
                                                   &lctx_player);
                /* Phase 4.5.12-G diag: dump each bone's WORLD y so we can
                 * see Leon's actual span at runtime (vs E3 agent's predicted
                 * 829 units after clip 0 frame 0 YXZ Euler). One-shot on
                 * frame 60. */
                if (g_engine.frame_count == 60) {
                    fprintf(stderr, "[bone] %2d: rel=(%d,%d,%d) world=(%d,%d,%d)\n",
                            bi, (int)p->trans[0], (int)p->trans[1], (int)p->trans[2],
                            (int)bone_world_trans[0], (int)bone_world_trans[1],
                            (int)bone_world_trans[2]);
                }
                /* AL-round (2026-05-26): dump Leon's root bone + view+screen
                 * pos right after the visibility gate fires so we can see
                 * whether Q12 fix put him off-cut. */
                if (bi == 0 && (g_engine.frame_count == 1700 ||
                                g_engine.frame_count == 1750 ||
                                g_engine.frame_count == 1800)) {
                    int32_t cr[9], ct[3];
                    re15_camera_compose_view_bone(&cam_view, yawed_rot, bone_world_trans,
                                                  cr, ct);
                    float _proj = (cam_view.fov_screen_dist > 0 && ct[2] > 1)
                        ? (float)cam_view.fov_screen_dist / (float)ct[2]
                        : 0.0f;
                    fprintf(stderr,
                        "[F%u-leon-root] world=(%d,%d,%d) view=(%d,%d,%d) "
                        "sd=%d screen=(%.1f,%.1f) cam_pos=(%d,%d,%d) cam_tgt=(%d,%d,%d) cut=%d visible=%d\n",
                        g_engine.frame_count,
                        (int)player_ref->x, (int)player_ref->y, (int)player_ref->z,
                        (int)ct[0], (int)ct[1], (int)ct[2],
                        (int)cam_view.fov_screen_dist,
                        (float)cx + ct[0] * _proj, (float)cy + ct[1] * _proj,
                        (int)active_cuts[active_cut_idx].pos_x,
                        (int)active_cuts[active_cut_idx].pos_y,
                        (int)active_cuts[active_cut_idx].pos_z,
                        (int)active_cuts[active_cut_idx].target_x,
                        (int)active_cuts[active_cut_idx].target_y,
                        (int)active_cuts[active_cut_idx].target_z,
                        active_cut_idx, player_visible);
                    fflush(stderr);
                }
                int32_t combined_rot[9];
                int32_t combined_trans[3];
                re15_camera_compose_view_bone(&cam_view, yawed_rot, bone_world_trans,
                                               combined_rot, combined_trans);
                for (int k = 0; k < 9; k++) bone_m[k] = combined_rot[k];
                bone_t[0] = combined_trans[0];
                bone_t[1] = combined_trans[1];
                bone_t[2] = combined_trans[2];
                /* save bone 11's COMPOSED matrix for the weapon-in-hand draw below */
                if (bi == 11) {
                    for (int k = 0; k < 9; k++) { wpn_bone_m[k] = bone_m[k]; wpn_yawed[k] = yawed_rot[k]; }
                    wpn_bone_t[0] = bone_t[0]; wpn_bone_t[1] = bone_t[1]; wpn_bone_t[2] = bone_t[2];
                    wpn_bone_valid = 1;
                }

                /* Phase 4.5.10-I DEBUG: HD per-bone marker. 8×8 high-contrast
                 * tile per bone. If projected position is OFF-screen, render
                 * a CLAMPED edge marker so we still see it. Plus dump bone
                 * 0 details (camera-space coords + screen) on a clear HUD
                 * line. */
                {
                    float _vx = bone_t[0];
                    float _vy = bone_t[1];
                    float _vz = bone_t[2];
                    if (_vz < 1.0f) _vz = 1.0f;
                    float _proj = screen_dist / _vz;
                    int sx = cx + RNDI(_vx * _proj);
                    int sy = cy + RNDI(_vy * _proj);
                    /* Clamp off-screen markers to edge so user SEES the
                     * direction Leon's bones flew off-screen to. */
                    int sx_clamp = sx;
                    int sy_clamp = sy;
                    int off_screen = 0;
                    if (sx_clamp < 4)   { sx_clamp = 4;   off_screen = 1; }
                    if (sx_clamp > SCREEN_XRES - 12) { sx_clamp = SCREEN_XRES - 12; off_screen = 1; }
                    if (sy_clamp < 4)   { sy_clamp = 4;   off_screen = 1; }
                    if (sy_clamp > SCREEN_YRES - 12) { sy_clamp = SCREEN_YRES - 12; off_screen = 1; }
                    static const uint8_t s_bone_colors[16][3] = {
                        {255, 255, 0},   /* yellow  bone 0 */
                        {0,   255, 0},   /* green   */
                        {255, 0,   0},   /* red     */
                        {0,   255, 255}, /* cyan    */
                        {255, 0,   255}, /* magenta */
                        {255, 128, 0},   /* orange  */
                        {128, 255, 128},
                        {255, 255, 255},
                        {128, 128, 255},
                        {255, 128, 128},
                        {128, 255, 0},
                        {0,   128, 255},
                        {200, 200, 0},
                        {0,   200, 200},
                        {200, 0,   200},
                        {255, 192, 64},
                    };
                    int ci = bi & 0xF;
                    (void)ci; (void)s_bone_colors; (void)sx_clamp; (void)sy_clamp;
                    /* I-round disable (2026-05-24): debug bone markers were
                     * the source of user-reported "multi-colored cubes"
                     * overlaying Leon — 15×8×8 colored tiles drawn over
                     * his mesh. Disabled now that bone math is correct. */
                    /* re15_render_tile(sx_clamp, sy_clamp, 8, 8, 0,
                                     s_bone_colors[ci][0],
                                     s_bone_colors[ci][1],
                                     s_bone_colors[ci][2]); */
                    (void)off_screen;
                    if (bi == 0 && g_engine.frame_count == 60) {
                        /* Phase 4.5.10-J: one-shot dump to debug.log at
                         * frame 60 so user can paste exact numbers. */
                        fprintf(stderr,
                            "[F60] cut=%d sd=%d\n"
                            "  cut.pos=(%d,%d,%d)\n"
                            "  cut.target=(%d,%d,%d)\n"
                            "  cut.fov=%u\n"
                            "  VT=(%d,%d,%d)\n"
                            "  VR0=(%d,%d,%d) VR1=(%d,%d,%d) VR2=(%d,%d,%d)\n"
                            "  Player=(%ld,%ld,%ld) rot_y=%d\n"
                            "  pose[0].trans=(%ld,%ld,%ld)\n"
                            "  bone_world=(%ld,%ld,%ld)\n"
                            "  combined_trans=(%d,%d,%d)\n"
                            "  combined_rot[0..2]=(%d,%d,%d)\n"
                            "  proj sx=%d sy=%d\n",
                            active_cut_idx, (int)screen_dist,
                            (int)active_cuts[active_cut_idx].pos_x,
                            (int)active_cuts[active_cut_idx].pos_y,
                            (int)active_cuts[active_cut_idx].pos_z,
                            (int)active_cuts[active_cut_idx].target_x,
                            (int)active_cuts[active_cut_idx].target_y,
                            (int)active_cuts[active_cut_idx].target_z,
                            (unsigned)active_cuts[active_cut_idx].fov,
                            (int)cam_view.trans[0], (int)cam_view.trans[1], (int)cam_view.trans[2],
                            (int)cam_view.rot[0], (int)cam_view.rot[1], (int)cam_view.rot[2],
                            (int)cam_view.rot[3], (int)cam_view.rot[4], (int)cam_view.rot[5],
                            (int)cam_view.rot[6], (int)cam_view.rot[7], (int)cam_view.rot[8],
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].y,
                            (long)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
                            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y,
                            (long)poses[0].trans[0], (long)poses[0].trans[1], (long)poses[0].trans[2],
                            (long)bone_world_trans[0], (long)bone_world_trans[1], (long)bone_world_trans[2],
                            (int)combined_trans[0], (int)combined_trans[1], (int)combined_trans[2],
                            (int)combined_rot[0], (int)combined_rot[1], (int)combined_rot[2],
                            sx, sy);
                    }
                    /* B0/VT/VR HUD silenced for cleaner cinematic view. */
                }

                const re15_md1_mesh_t *m = &md1.meshes[mi];

                /* Phase 4.5.7.5: textured triangles. The MD1's per-tri
                 * `page` selects which 128-pixel-wide sub-page of the TIM
                 * the UVs sample from. PC's SDL_Texture is the WHOLE TIM
                 * as one image, so we add page_x_offset to each UV.x to
                 * map page-local 0..127 → full-TIM pixel coord.
                 *
                 * page bits 0..3 = VRAM_x / 64. For 8bpp, each VRAM column
                 * holds 2 pixels, so page_x_offset = (page & 0xF) * 128.
                 *
                 * Caveat (#PC-2): only the first CLUT is decoded into the
                 * SDL_Texture, so body/limb colors will be off until the
                 * PC backend learns to switch CLUTs per triangle. */
                for (int ti = 0; ti < m->triangle_count; ti++) {
                    const re15_md1_triangle_t *t = &m->triangles[ti];
                    if (t->v0 >= (uint32_t)m->tri_vertex_count) continue;
                    if (t->v1 >= (uint32_t)m->tri_vertex_count) continue;
                    if (t->v2 >= (uint32_t)m->tri_vertex_count) continue;
                    int sx0, sy0, sx1, sy1, sx2, sy2;
                    float wz0, wz1, wz2;
                    PROJECT_VERT(&m->tri_vertices[t->v0], sx0, sy0, wz0);
                    PROJECT_VERT(&m->tri_vertices[t->v1], sx1, sy1, wz1);
                    PROJECT_VERT(&m->tri_vertices[t->v2], sx2, sy2, wz2);
                    /* H28: skip tris where any vert is behind near-plane
                     * (was clamping to vz=1 → exploded projections). */
                    if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
                    /* Backface cull — REQUIRED. PSX's gte_nclip drops these
                     * implicitly on the hardware side; PC needs the explicit
                     * check or back-facing tris render too, producing the
                     * "duplicate arms / atlas-on-the-right" pattern the user
                     * saw (each mesh drawn twice with its back-face UVs). */
                    if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                    const re15_md1_tri_uv_t *uv = &m->triangle_uvs[ti];
                    int page_x_offset = (int)((uv->page & 0x000F) * 128);
                    /* Phase 4.5.7.7: avg world-Z for back-to-front sort. */
                    int avg_z = (int)((wz0 + wz1 + wz2) * (1.0f / 3.0f));
                    /* BF-round: per-vertex NCCT shading. Lookup normals by
                     * tri.n0/n1/n2 indices; fall back to tint if missing. */
                    uint8_t r0t, g0t, b0t, r1t, g1t, b1t, r2t, g2t, b2t;
                    if (m->tri_normals &&
                        t->n0 < (uint32_t)m->tri_normal_count &&
                        t->n1 < (uint32_t)m->tri_normal_count &&
                        t->n2 < (uint32_t)m->tri_normal_count) {
                        const re15_md1_vertex_t *n0 = &m->tri_normals[t->n0];
                        const re15_md1_vertex_t *n1 = &m->tri_normals[t->n1];
                        const re15_md1_vertex_t *n2 = &m->tri_normals[t->n2];
                        re15_light_shade_vertex(&lctx_player, n0->x, n0->y, n0->z, &r0t, &g0t, &b0t);
                        re15_light_shade_vertex(&lctx_player, n1->x, n1->y, n1->z, &r1t, &g1t, &b1t);
                        re15_light_shade_vertex(&lctx_player, n2->x, n2->y, n2->z, &r2t, &g2t, &b2t);
                    } else {
                        r0t = r1t = r2t = g_re15_light_tint[0];
                        g0t = g1t = g2t = g_re15_light_tint[1];
                        b0t = b1t = b2t = g_re15_light_tint[2];
                    }
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx1, sy1, (int)uv->u1 + page_x_offset, (int)uv->v1,
                        sx2, sy2, (int)uv->u2 + page_x_offset, (int)uv->v2,
                        0, (int)uv->clut, avg_z,
                        r0t, g0t, b0t, r1t, g1t, b1t, r2t, g2t, b2t);
                }

                /* Quads: 4-edge wireframe loop in (v0,v1,v3,v2) winding. */
                for (int qi = 0; qi < m->quad_count; qi++) {
                    const re15_md1_quad_t *q = &m->quads[qi];
                    if (q->v0 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v1 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v2 >= (uint32_t)m->quad_vertex_count) continue;
                    if (q->v3 >= (uint32_t)m->quad_vertex_count) continue;
                    int sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3;
                    float wz0, wz1, wz2, wz3;
                    PROJECT_VERT(&m->quad_vertices[q->v0], sx0, sy0, wz0);
                    PROJECT_VERT(&m->quad_vertices[q->v1], sx1, sy1, wz1);
                    PROJECT_VERT(&m->quad_vertices[q->v2], sx2, sy2, wz2);
                    PROJECT_VERT(&m->quad_vertices[q->v3], sx3, sy3, wz3);
                    /* H28: skip quads where any vert is behind near-plane. */
                    if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
                    /* Backface cull — same reason as the triangle path. */
                    if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                    const re15_md1_quad_uv_t *uv = &m->quad_uvs[qi];
                    int page_x_offset = (int)((uv->page & 0x000F) * 128);
                    /* Phase 4.5.7.7: avg world-Z for back-to-front sort.
                     * Each split tri gets its OWN 3-vertex average so the
                     * two halves of a quad sort independently — matches
                     * PSX's gte_avsz3 per-poly OTZ rather than the quad-
                     * wide gte_avsz4 (we lose 4-vert averaging but gain
                     * finer self-sort on twisted quads). */
                    int avg_z1 = (int)((wz0 + wz1 + wz3) * (1.0f / 3.0f));
                    int avg_z2 = (int)((wz0 + wz3 + wz2) * (1.0f / 3.0f));
                    /* BF-round: per-vertex shading for the 4 quad corners. */
                    uint8_t qr0, qg0, qb0, qr1, qg1, qb1, qr2, qg2, qb2, qr3, qg3, qb3;
                    if (m->quad_normals &&
                        q->n0 < (uint32_t)m->quad_normal_count &&
                        q->n1 < (uint32_t)m->quad_normal_count &&
                        q->n2 < (uint32_t)m->quad_normal_count &&
                        q->n3 < (uint32_t)m->quad_normal_count) {
                        const re15_md1_vertex_t *qn0 = &m->quad_normals[q->n0];
                        const re15_md1_vertex_t *qn1 = &m->quad_normals[q->n1];
                        const re15_md1_vertex_t *qn2 = &m->quad_normals[q->n2];
                        const re15_md1_vertex_t *qn3 = &m->quad_normals[q->n3];
                        re15_light_shade_vertex(&lctx_player, qn0->x, qn0->y, qn0->z, &qr0, &qg0, &qb0);
                        re15_light_shade_vertex(&lctx_player, qn1->x, qn1->y, qn1->z, &qr1, &qg1, &qb1);
                        re15_light_shade_vertex(&lctx_player, qn2->x, qn2->y, qn2->z, &qr2, &qg2, &qb2);
                        re15_light_shade_vertex(&lctx_player, qn3->x, qn3->y, qn3->z, &qr3, &qg3, &qb3);
                    } else {
                        qr0 = qr1 = qr2 = qr3 = g_re15_light_tint[0];
                        qg0 = qg1 = qg2 = qg3 = g_re15_light_tint[1];
                        qb0 = qb1 = qb2 = qb3 = g_re15_light_tint[2];
                    }
                    /* Quad → 2 tris with (v0,v1,v3,v2) winding (matches PSX). */
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx1, sy1, (int)uv->u1 + page_x_offset, (int)uv->v1,
                        sx3, sy3, (int)uv->u3 + page_x_offset, (int)uv->v3,
                        0, (int)uv->clut, avg_z1,
                        qr0, qg0, qb0, qr1, qg1, qb1, qr3, qg3, qb3);
                    re15_render_textured_tri_lit(
                        sx0, sy0, (int)uv->u0 + page_x_offset, (int)uv->v0,
                        sx3, sy3, (int)uv->u3 + page_x_offset, (int)uv->v3,
                        sx2, sy2, (int)uv->u2 + page_x_offset, (int)uv->v2,
                        0, (int)uv->clut, avg_z2,
                        qr0, qg0, qb0, qr3, qg3, qb3, qr2, qg2, qb2);
                }
            }

            /* WEAPON-IN-HAND (room-fix #3, byte-true mechanism @0x800356f0-70: the DRAW
             * anim-event attaches the equipped weapon's model as extra parts on the weapon
             * bone; the port draws the equipped PLW's MD1 mesh with bone-11's composed
             * matrix). Visible: melee once the knife is IN-HAND (the 0x4000 flag, persists
             * across lower/raise); gun once equipped (faithful-line — the gun FSM's attach
             * event is un-RE'd; classic-RE carry look). */
            {
                extern int re15_player_equipped_weapon(void);
                extern int re15_player_knife_in_hand(void);
                int eq = re15_player_equipped_weapon();
                int wi = (eq >= 3) ? 1 : 0;
                int vis = (eq >= 3) ? 1 : re15_player_knife_in_hand();
                if (vis && wpn_bone_valid && wpn_md1_ok[wi] && player_visible &&
                    re15_player_victim_state() == 0) {
                    re15_render_pc_bind_tim_slot(wi ? RE15_TIM_SLOT_WPN_GUN
                                                    : RE15_TIM_SLOT_WPN_MELEE);
                    for (int k = 0; k < 9; k++) bone_m[k] = wpn_bone_m[k];
                    bone_t[0] = wpn_bone_t[0]; bone_t[1] = wpn_bone_t[1]; bone_t[2] = wpn_bone_t[2];
                    if (player_lit)
                        re15_light_ctx_rotate_for_bone(&lctx_player_world, wpn_yawed, &lctx_player);
                    const re15_md1_mesh_t *m = &wpn_md1[wi].meshes[0];
                    for (int ti = 0; ti < m->triangle_count; ti++) {
                        const re15_md1_triangle_t *t = &m->triangles[ti];
                        if (t->v0 >= (uint32_t)m->tri_vertex_count ||
                            t->v1 >= (uint32_t)m->tri_vertex_count ||
                            t->v2 >= (uint32_t)m->tri_vertex_count) continue;
                        int sx0, sy0, sx1, sy1, sx2, sy2;
                        float wz0, wz1, wz2;
                        PROJECT_VERT(&m->tri_vertices[t->v0], sx0, sy0, wz0);
                        PROJECT_VERT(&m->tri_vertices[t->v1], sx1, sy1, wz1);
                        PROJECT_VERT(&m->tri_vertices[t->v2], sx2, sy2, wz2);
                        if (wz0 < 0 || wz1 < 0 || wz2 < 0) continue;
                        if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                        const re15_md1_tri_uv_t *uv = &m->triangle_uvs[ti];
                        int pxo = (int)((uv->page & 0x000F) * 128);
                        int avg_z = (int)((wz0 + wz1 + wz2) * (1.0f / 3.0f));
                        uint8_t r0t,g0t,b0t,r1t,g1t,b1t,r2t,g2t,b2t;
                        if (m->tri_normals && t->n0 < (uint32_t)m->tri_normal_count &&
                            t->n1 < (uint32_t)m->tri_normal_count &&
                            t->n2 < (uint32_t)m->tri_normal_count) {
                            const re15_md1_vertex_t *n0 = &m->tri_normals[t->n0];
                            const re15_md1_vertex_t *n1 = &m->tri_normals[t->n1];
                            const re15_md1_vertex_t *n2 = &m->tri_normals[t->n2];
                            re15_light_shade_vertex(&lctx_player, n0->x,n0->y,n0->z, &r0t,&g0t,&b0t);
                            re15_light_shade_vertex(&lctx_player, n1->x,n1->y,n1->z, &r1t,&g1t,&b1t);
                            re15_light_shade_vertex(&lctx_player, n2->x,n2->y,n2->z, &r2t,&g2t,&b2t);
                        } else {
                            r0t=r1t=r2t=g_re15_light_tint[0];
                            g0t=g1t=g2t=g_re15_light_tint[1];
                            b0t=b1t=b2t=g_re15_light_tint[2];
                        }
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx1, sy1, (int)uv->u1 + pxo, (int)uv->v1,
                            sx2, sy2, (int)uv->u2 + pxo, (int)uv->v2,
                            0, (int)uv->clut, avg_z,
                            r0t,g0t,b0t, r1t,g1t,b1t, r2t,g2t,b2t);
                    }
                    for (int qi = 0; qi < m->quad_count; qi++) {
                        const re15_md1_quad_t *q = &m->quads[qi];
                        if (q->v0 >= (uint32_t)m->quad_vertex_count ||
                            q->v1 >= (uint32_t)m->quad_vertex_count ||
                            q->v2 >= (uint32_t)m->quad_vertex_count ||
                            q->v3 >= (uint32_t)m->quad_vertex_count) continue;
                        int sx0,sy0,sx1,sy1,sx2,sy2,sx3,sy3;
                        float wz0,wz1,wz2,wz3;
                        PROJECT_VERT(&m->quad_vertices[q->v0], sx0, sy0, wz0);
                        PROJECT_VERT(&m->quad_vertices[q->v1], sx1, sy1, wz1);
                        PROJECT_VERT(&m->quad_vertices[q->v2], sx2, sy2, wz2);
                        PROJECT_VERT(&m->quad_vertices[q->v3], sx3, sy3, wz3);
                        if (wz0 < 0 || wz1 < 0 || wz2 < 0 || wz3 < 0) continue;
                        if (IS_BACKFACE(sx0, sy0, sx1, sy1, sx2, sy2)) continue;
                        const re15_md1_quad_uv_t *uv = &m->quad_uvs[qi];
                        int pxo = (int)((uv->page & 0x000F) * 128);
                        int avg_z1 = (int)((wz0 + wz1 + wz3) * (1.0f / 3.0f));
                        int avg_z2 = (int)((wz0 + wz3 + wz2) * (1.0f / 3.0f));
                        uint8_t c0[3], c1[3], c2[3], c3[3];
                        c0[0]=c1[0]=c2[0]=c3[0]=g_re15_light_tint[0];
                        c0[1]=c1[1]=c2[1]=c3[1]=g_re15_light_tint[1];
                        c0[2]=c1[2]=c2[2]=c3[2]=g_re15_light_tint[2];
                        if (m->quad_normals && q->n0 < (uint32_t)m->quad_normal_count &&
                            q->n1 < (uint32_t)m->quad_normal_count &&
                            q->n2 < (uint32_t)m->quad_normal_count &&
                            q->n3 < (uint32_t)m->quad_normal_count) {
                            const re15_md1_vertex_t *n;
                            n=&m->quad_normals[q->n0]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c0[0],&c0[1],&c0[2]);
                            n=&m->quad_normals[q->n1]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c1[0],&c1[1],&c1[2]);
                            n=&m->quad_normals[q->n2]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c2[0],&c2[1],&c2[2]);
                            n=&m->quad_normals[q->n3]; re15_light_shade_vertex(&lctx_player,n->x,n->y,n->z,&c3[0],&c3[1],&c3[2]);
                        }
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx1, sy1, (int)uv->u1 + pxo, (int)uv->v1,
                            sx3, sy3, (int)uv->u3 + pxo, (int)uv->v3,
                            0, (int)uv->clut, avg_z1,
                            c0[0],c0[1],c0[2], c1[0],c1[1],c1[2], c3[0],c3[1],c3[2]);
                        re15_render_textured_tri_lit(
                            sx0, sy0, (int)uv->u0 + pxo, (int)uv->v0,
                            sx3, sy3, (int)uv->u3 + pxo, (int)uv->v3,
                            sx2, sy2, (int)uv->u2 + pxo, (int)uv->v2,
                            0, (int)uv->clut, avg_z2,
                            c0[0],c0[1],c0[2], c3[0],c3[1],c3[2], c2[0],c2[1],c2[2]);
                    }
                }
            }
#undef PROJECT_VERT
#undef IS_BACKFACE

            /* NPC skeletal render: same MD1 mesh as Leon (we don't have
             * Elliot's PLD parsed yet), positioned at the NPC's world
             * pos using its motion/anim_frame for pose. Visually appears
             * as a second Leon-shaped character — communicates presence
             * even though wrong model. Bone-by-bone projection mirrors
             * the player loop above. */
            for (int npc_i = 1; npc_i < RE15_ACTOR_MAX; npc_i++) {
                const re15_actor_t *npc = &g_actors[npc_i];
                if (!npc->active) continue;

                /* Load the enemy model the moment the actor is ACTIVE — BEFORE the region-quad cull
                 * below, so an OFF-SCREEN actor still gets its EMD into the shared registry. The AI's
                 * root-motion (re15_clip_root_motion*) reads that bank regardless of visibility, and the
                 * PSX loads the model at Sce_em_set SPAWN (not lazily on first draw) — so a walking actor
                 * outside the current camera cut must still have its keyframes. (Was gated after the cull
                 * at the old site below; a culled off-screen enemy then had bank=NULL and froze.) */
                if (npc->type && npc->type != 0x47 && !re15_enemy_find(npc->type))
                    pc_enemy_load(npc->type);

                /* BO-round (Tier-3): canonical per-cut REGION-QUAD cull, same as
                 * the prop path (PSX FUN_8002c18c → FUN_80014368). Replaces the
                 * BH-round |x|<25000 teleport-hide proxy: Elliot at the off-stage
                 * (-31000,…) hide corner is outside every cut-0..6 quad → not
                 * drawn (nor lit), exactly as PSX. On-stage NPCs are inside →
                 * drawn. Falls back to nothing-culled if the cut has no region. */
                if (cam_has_region &&
                    !re15_aot_point_in_quad(npc->x, npc->z,
                                            cam_region_xs, cam_region_zs)) {
                    continue;
                }

                /* RE1.5 character shadow for this NPC — FUN_8001b064 is called
                 * PER-ENTITY (corners from param_1+0xc/+0xe), so each on-stage
                 * actor gets one. Actor-position center + CAMERA-yaw rotation
                 * (same corrected scheme as the player). */
                {
                    /* byte-true actor-yaw shadow matrix = RotMatrixY(npc->rot_y) via the trig LUT. */
                    int32_t nyaw[9];
                    re15_camera_yaw_matrix_angle(npc->rot_y, nyaw);
                    /* CORPSE BLOOD POOL (root-state-7 sub0/1, FUN_80109554 @0x801095e8-614 +
                     * @0x80109710-24): the shadow recolors 0x00ffff38 (subtractive dark red) and
                     * its half-extents grow +8/frame for the 0x5a pool budget (600/700 base ->
                     * ~1328/1428). Pool progress = 0x5a - +0x9e while settling (sub1), full after. */
                    int32_t nhx = 500, nhz = 600;
                    int corpse_pool = 0;
                    /* The growing dark-red blood pool is the ZOMBIE root's corpse-settle (FUN_80109554):
                     * only the live-step zombie types (0x10/0x11/0x12/0x16/0x18) actually run
                     * re15_enemy_corpse_settle (the writer of grab_kill_ctr=0x5a). Every other enemy
                     * (dog/crow/spider/maggot/cockroach/alligator/tyrant/ivy/birkin/zgirl) reaches state 7
                     * WITHOUT it, so its grab_kill_ctr/sub_state_1 hold unrelated values -> gating the pool
                     * on state==7 alone drew a spurious full-size zombie blood pool under those corpses
                     * (audit wf_246147e3). Gate on the zombie type: those others keep the normal shadow. */
                    int nis_zombie = (npc->type == 0x10 || npc->type == 0x11 || npc->type == 0x12 ||
                                      npc->type == 0x16 || npc->type == 0x18);
                    if (npc->state == RE15_AI_STATE_CORPSE && nis_zombie) {
                        int grow = (npc->sub_state_1 <= 1)
                                     ? (0x5a - (npc->grab_kill_ctr > 0 ? npc->grab_kill_ctr : 0))
                                     : 0x5a;
                        if (grow < 0) grow = 0;
                        nhx = 600 + 8 * grow;                 /* af5c base for zombies = 600/700 */
                        nhz = 700 + 8 * grow;
                        corpse_pool = 1;
                    }
                    int32_t nsh_c[4][3] = {
                        { -nhx, 0,  nhz }, { -nhx, 0, -nhz },
                        {  nhx, 0,  nhz }, {  nhx, 0, -nhz },
                    };
                    int32_t nsh_world[3] = { npc->x, npc->y, npc->z };
                    int32_t nsh_rot[9], nsh_trans[3];
                    /* byte-true: RotMatrixY(ACTOR rot_y) via the trig LUT (FUN_8001b064 @0x8001b0e4
                     * uses entity+0x6a, NOT the camera yaw — RE'd wf_13911cba). nyaw above IS that
                     * matrix; the old camera-yaw was the port author's misread of DAT_800ac784. */
                    re15_camera_compose_view_bone(&cam_view, nyaw, nsh_world,
                                                  nsh_rot, nsh_trans);
                    int32_t nm[9], nt[3];   /* Q12 (byte-true integer RTPS) */
                    for (int i = 0; i < 9; i++) nm[i] = nsh_rot[i];
                    for (int i = 0; i < 3; i++) nt[i] = nsh_trans[i];
                    int nsx[4], nsy[4], nok = 1;
                    for (int v = 0; v < 4; v++) {
                        int32_t _x = nsh_c[v][0], _y = nsh_c[v][1], _z = nsh_c[v][2];
                        int32_t _vx = (int32_t)(((int64_t)_x*nm[0] + (int64_t)_y*nm[1] + (int64_t)_z*nm[2]) >> 12) + nt[0];
                        int32_t _vy = (int32_t)(((int64_t)_x*nm[3] + (int64_t)_y*nm[4] + (int64_t)_z*nm[5]) >> 12) + nt[1];
                        int32_t _vz = (int32_t)(((int64_t)_x*nm[6] + (int64_t)_y*nm[7] + (int64_t)_z*nm[8]) >> 12) + nt[2];
                        if (_vz < 64) { nok = 0; break; }
                        int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                        int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                        uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                        uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                        nsx[v] = cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16);
                        nsy[v] = cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16);
                    }
                    if (nok) {
                        if (corpse_pool)
                            re15_render_shadow_quad_c(nsx[0], nsy[0], nsx[1], nsy[1],
                                                      nsx[2], nsy[2], nsx[3], nsy[3],
                                                      0x38, 0xff, 0xff);   /* the dark-red pool */
                        else
                            re15_render_shadow_quad(nsx[0], nsy[0], nsx[1], nsy[1],
                                                    nsx[2], nsy[2], nsx[3], nsy[3]);
                    }
                }

                /* (enemy-model load moved ABOVE the region cull — see the pc_enemy_load call right
                 * after the !npc->active guard; visibility-independent so off-screen AI has its bank.) */

                /* SHARED anim selection (unify 2026-06-06) — same view-model as the
                 * player + the PSX build (anim_select_common.c). is_player=0 enables
                 * the Elliot (type 0x47) branch: his own PL05 bank + mesh; everyone
                 * else falls back to Leon's PL00. Locomotion/idle sentinels route to
                 * PL00W01 / PL00-base exactly as for the player. (em21_* banks fields are
                 * no longer set — type 0x21 goes through the generic registry now.) */
                re15_anim_banks_t banks = {
                    .def_mesh = &md1, .def_skel = &skel, .def_anim = &anim,
                    .w01_skel = &w01_skel, .w01_anim = &w01_anim, .w01_ok = w01_ok,
                    .pl00_skel = &pl00_skel, .pl00_anim = &pl00_anim, .pl00_ok = pl00_ok,
                    .elliot_mesh = &elliot_md1, .elliot_skel = &elliot_skel,
                    .elliot_anim = &elliot_anim, .elliot_ok = (elliot_ok && elliot_skel_ok),
                };
                re15_anim_view_t av;
                re15_actor_anim_select(npc, 0, &banks, &av);
                int is_elliot = (av.mesh == &elliot_md1);
                const re15_md1_t           *npc_md1  = av.mesh;
                const re15_emd_skeleton_t  *npc_skel = av.skel;
                const re15_emd_animation_t *npc_anim = av.anim;

                /* LOCO-BANK render: the STAGE1 zombie WALKING states pose the locomotion bank (bank0,
                 * entity+0x84), NOT the 43-clip action bank. W1 disasm 2026-07-03: the ENGAGE (+0x5=2)
                 * IS the aware walk (bank0 clip +0x1d4 in {2..5} — the arms-out per-zombie walks),
                 * the 0x13 lurch plays bank0 clip 1, and the TURN (+0x5=7) pivots on the same +0x1d4
                 * clip. Gate on state ACTIVE (a HURT zombie has +0x5 = the weapon id, which collides
                 * with 2 — its stagger renders bank1 as before). motion carries the clip index. */
                if (npc->state == 1 &&
                    (npc->sub_state_1 == 0x13 || npc->sub_state_1 == 2 || npc->sub_state_1 == 7)) {
                    re15_enemy_bank_t *lb = re15_enemy_find(npc->type);
                    if (lb && lb->loco_ok && (int)npc->motion < lb->anim_loco.clip_count) {
                        npc_skel = &lb->skel_loco;
                        npc_anim = &lb->anim_loco;
                        av.clip_override = (int)npc->motion;   /* bank0: 1 = lurch, 2..5 = engage walks */
                    }
                }

                /* DIAG (RE15_ENEMY_DIAG=1): one line per actor on first render — proves the
                 * generic enemy uses its OWN model (mesh != Leon's def_mesh) vs a Leon fallback,
                 * and shows motion/anim_frame so a "static Leon" report can be diagnosed headless. */
                if (getenv("RE15_ENEMY_DIAG")) {
                    static uint32_t s_diag_seen = 0;   /* bitmask by actor index, one-shot */
                    int ai = (int)(npc - g_actors);
                    if (ai >= 0 && ai < 32 && !(s_diag_seen & (1u << ai))) {
                        s_diag_seen |= (1u << ai);
                        const char *which = (av.mesh == &md1) ? "LEON-FALLBACK"
                                          : is_elliot ? "elliot"
                                          : (av.pc_tex_slot >= 0) ? "enemy-bank" : "other";
                        fprintf(stderr, "[enemy-diag] actor%d type=0x%02x model=%s slot=%d "
                                "motion=%u anim_frame=%u clips=%d\n",
                                ai, npc->type, which, av.pc_tex_slot,
                                (unsigned)npc->motion, (unsigned)npc->anim_frame,
                                npc_anim->clip_count);
                    }
                }

                /* Bind the NPC's TIM: a generic enemy bank's own slot (av.pc_tex_slot,
                 * which now covers the crows too), else Elliot (1) / Leon (0). */
                re15_render_pc_bind_tim_slot(av.pc_tex_slot >= 0 ? av.pc_tex_slot
                                             : (is_elliot ? 1 : 0));

                if (npc_anim->clip_count <= 0 || npc_skel->keyframe_count <= 0) continue;
                /* Platform fps policy: 30fps target = raw anim_frame; 60fps halves. */
                uint32_t ncur = (target_fps == 30)
                    ? (uint32_t)npc->anim_frame
                    : ((uint32_t)npc->anim_frame >> 1);
                int npc_kf = re15_compute_actor_kf(npc_anim, npc_skel, npc,
                                                   av.clip_override, ncur);

                re15_skel_pose_t npc_poses[RE15_EMD_MAX_BONES];
                g_anim_pose_actor = npc;   /* FRAC crossfade for this NPC/enemy body */
                if (re15_skel_compute_pose(npc_skel, npc_kf, npc_poses) != 0) continue;
                /* RE15_POSE_DUMP=1: per-frame RENDER-LEVEL pose log (verify the posed keyframes reach
                 * the screen — the walk-look investigations). b13 = the em10 reach forearm. */
                {
                    static FILE *s_pose_fp = NULL; static int s_pose_tried = 0;
                    if (!s_pose_tried) { s_pose_tried = 1;
                        const char *pd = getenv("RE15_POSE_DUMP");
                        if (pd && *pd) s_pose_fp = fopen(pd, "w");
                    }
                    if (s_pose_fp) {
                        fprintf(s_pose_fp, "[pose] F%u slot%d ss1=%d mo=%d af=%d frac=%d kf=%d "
                                "b9(%d,%d,%d) b13(%d,%d,%d)\n",
                                (unsigned)g_engine.frame_count, (int)(npc - g_actors),
                                npc->sub_state_1, npc->motion, npc->anim_frame, npc->anim_frac, npc_kf,
                                (int)npc_poses[9].trans[0], (int)npc_poses[9].trans[1], (int)npc_poses[9].trans[2],
                                (int)npc_poses[13].trans[0], (int)npc_poses[13].trans[1], (int)npc_poses[13].trans[2]);
                        fflush(s_pose_fp);
                    }
                }

                int32_t nfs = re15_sin_q12((int)npc->rot_y);
                int32_t nfc = re15_cos_q12((int)npc->rot_y);
                int32_t nyaw[9] = { nfc, 0, nfs, 0, 0x1000, 0, -nfs, 0, nfc };

                /* Canonical (2026-05-29): eval_pos = the NPC's own world
                 * position, matching PSX FUN_80053fc0 / FUN_80039ca0
                 * (puVar3+0x14). Safe: the teleport-hide cull above already
                 * dropped any off-stage NPC, so we only light visible actors. */
                /* CANONICAL per-bone (2026-06-02): world-space ctx built once
                 * (rot=NULL); per-bone fold below. Was body-only (nyaw, approx). */
                re15_actor_lightctx_t lctx_npc, lctx_npc_world;
                int npc_lit = (g_re15_room_lights_ok &&
                               g_re15_active_cut >= 0 &&
                               g_re15_active_cut < g_re15_room_lights.cut_count);
                if (npc_lit) {
                    int32_t npc_pos_w[3] = {
                        (int32_t)npc->x,
                        (int32_t)npc->y,
                        (int32_t)npc->z
                    };
                    re15_light_setup_actor(
                        &g_re15_room_lights.cuts[g_re15_active_cut],
                        npc_pos_w, NULL, &lctx_npc_world);
                } else {
                    memset(&lctx_npc, 0, sizeof(lctx_npc));
                }

                int npc_bones = npc_skel->bone_count;
                if (npc_bones > npc_md1->mesh_count) npc_bones = npc_md1->mesh_count;
                for (int nbi = 0; nbi < npc_bones; nbi++) {
                    const re15_skel_pose_t *np = &npc_poses[nbi];
                    int32_t nyawed_rot[9];
                    for (int r = 0; r < 3; r++) {
                        for (int c = 0; c < 3; c++) {
                            int64_t s = 0;
                            for (int k = 0; k < 3; k++)
                                s += (int64_t)nyaw[r*3+k] * (int64_t)np->rot[k*3+c];
                            nyawed_rot[r*3+c] = (int32_t)(s >> 12);
                        }
                    }
                    int32_t nyawed_trans[3];
                    for (int r = 0; r < 3; r++) {
                        int64_t s = 0;
                        for (int k = 0; k < 3; k++)
                            s += (int64_t)nyaw[r*3+k] * (int64_t)np->trans[k];
                        nyawed_trans[r] = (int32_t)(s >> 12);
                    }
                    int32_t nbone_world_trans[3] = {
                        nyawed_trans[0] + npc->x,
                        nyawed_trans[1] + npc->y,
                        nyawed_trans[2] + npc->z,
                    };
                    /* CANONICAL per-bone light fold (2026-06-02). */
                    if (npc_lit)
                        re15_light_ctx_rotate_for_bone(&lctx_npc_world, nyawed_rot,
                                                       &lctx_npc);
                    int32_t ncomb_rot[9];
                    int32_t ncomb_trans[3];
                    re15_camera_compose_view_bone(&cam_view, nyawed_rot, nbone_world_trans,
                                                   ncomb_rot, ncomb_trans);

                    int32_t nbone_m[9], nbone_t[3];   /* Q12 view×bone (byte-true integer RTPS) */
                    for (int i = 0; i < 9; i++) nbone_m[i] = ncomb_rot[i];
                    for (int i = 0; i < 3; i++) nbone_t[i] = ncomb_trans[i];

                    /* AK-round F950 dump (2026-05-26): Elliot's root bone view-
                     * space + projected screen for cut 2 framing diagnosis. */
                    if (is_elliot && nbi == 0 &&
                        g_engine.frame_count == 950) {
                        float _proj0 = (cam_view.fov_screen_dist > 0 && nbone_t[2] > 1.0f)
                            ? (float)cam_view.fov_screen_dist / nbone_t[2]
                            : 0.0f;
                        fprintf(stderr,
                            "[F950-elliot-root] world=(%d,%d,%d) view=(%d,%d,%d) "
                            "sd=%d screen=(%.1f,%.1f) cam_pos=(%d,%d,%d) cam_tgt=(%d,%d,%d) fov=%u cut=%d\n",
                            (int)npc->x, (int)npc->y, (int)npc->z,
                            (int)nbone_t[0], (int)nbone_t[1], (int)nbone_t[2],
                            (int)cam_view.fov_screen_dist,
                            (float)cx + nbone_t[0] * _proj0,
                            (float)cy + nbone_t[1] * _proj0,
                            (int)active_cuts[active_cut_idx].pos_x,
                            (int)active_cuts[active_cut_idx].pos_y,
                            (int)active_cuts[active_cut_idx].pos_z,
                            (int)active_cuts[active_cut_idx].target_x,
                            (int)active_cuts[active_cut_idx].target_y,
                            (int)active_cuts[active_cut_idx].target_z,
                            (unsigned)active_cuts[active_cut_idx].fov,
                            active_cut_idx);
                    }

                    const re15_md1_mesh_t *nm = &npc_md1->meshes[nbi];
                    for (int ti = 0; ti < nm->triangle_count; ti++) {
                        const re15_md1_triangle_t *tri = &nm->triangles[ti];
                        if (tri->v0 >= (uint32_t)nm->tri_vertex_count) continue;
                        if (tri->v1 >= (uint32_t)nm->tri_vertex_count) continue;
                        if (tri->v2 >= (uint32_t)nm->tri_vertex_count) continue;
                        float ax[3], ay[3], wz[3];
                        const re15_md1_vertex_t *vp[3] = {
                            &nm->tri_vertices[tri->v0],
                            &nm->tri_vertices[tri->v1],
                            &nm->tri_vertices[tri->v2],
                        };
                        int ok = 1;
                        for (int v = 0; v < 3; v++) {
                            int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                            int32_t _vx = (int32_t)(((int64_t)_x*nbone_m[0] + (int64_t)_y*nbone_m[1] + (int64_t)_z*nbone_m[2]) >> 12) + nbone_t[0];
                            int32_t _vy = (int32_t)(((int64_t)_x*nbone_m[3] + (int64_t)_y*nbone_m[4] + (int64_t)_z*nbone_m[5]) >> 12) + nbone_t[1];
                            int32_t _vz = (int32_t)(((int64_t)_x*nbone_m[6] + (int64_t)_y*nbone_m[7] + (int64_t)_z*nbone_m[8]) >> 12) + nbone_t[2];
                            if (_vz < 64) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                            int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                            uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                            uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                            ax[v] = (float)(cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16));
                            ay[v] = (float)(cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16));
                            wz[v] = (float)_vz;
                        }
                        if (!ok) continue;
                        const re15_md1_tri_uv_t *uv = &nm->triangle_uvs[ti];
                        int page_off = (int)((uv->page & 0x000F) * 128);
                        /* G-round (2026-05-24): use real avg_z for depth sort
                         * (was hardcoded 1 = always-on-top which broke heli/
                         * NPC overlap — Leon-cubes always drew on top of heli
                         * even when behind it). */
                        int npc_avg_z = (int)((wz[0] + wz[1] + wz[2]) * (1.0f / 3.0f));
                        /* BF-round: per-vertex NCCT shading for NPC. */
                        uint8_t nr0, ng0, nb0, nr1, ng1, nb1, nr2, ng2, nb2;
                        if (nm->tri_normals &&
                            tri->n0 < (uint32_t)nm->tri_normal_count &&
                            tri->n1 < (uint32_t)nm->tri_normal_count &&
                            tri->n2 < (uint32_t)nm->tri_normal_count) {
                            const re15_md1_vertex_t *nn0 = &nm->tri_normals[tri->n0];
                            const re15_md1_vertex_t *nn1 = &nm->tri_normals[tri->n1];
                            const re15_md1_vertex_t *nn2 = &nm->tri_normals[tri->n2];
                            re15_light_shade_vertex(&lctx_npc, nn0->x, nn0->y, nn0->z, &nr0, &ng0, &nb0);
                            re15_light_shade_vertex(&lctx_npc, nn1->x, nn1->y, nn1->z, &nr1, &ng1, &nb1);
                            re15_light_shade_vertex(&lctx_npc, nn2->x, nn2->y, nn2->z, &nr2, &ng2, &nb2);
                        } else {
                            nr0 = nr1 = nr2 = g_re15_light_tint[0];
                            ng0 = ng1 = ng2 = g_re15_light_tint[1];
                            nb0 = nb1 = nb2 = g_re15_light_tint[2];
                        }
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[1], (int)ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                            (int)ax[2], (int)ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                            0, (int)uv->clut, npc_avg_z,
                            nr0, ng0, nb0, nr1, ng1, nb1, nr2, ng2, nb2);
                    }
                    /* quads similarly */
                    for (int qi = 0; qi < nm->quad_count; qi++) {
                        const re15_md1_quad_t *qd = &nm->quads[qi];
                        if (qd->v0 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v1 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v2 >= (uint32_t)nm->quad_vertex_count) continue;
                        if (qd->v3 >= (uint32_t)nm->quad_vertex_count) continue;
                        float ax[4], ay[4], wz[4];
                        const re15_md1_vertex_t *vp[4] = {
                            &nm->quad_vertices[qd->v0],
                            &nm->quad_vertices[qd->v1],
                            &nm->quad_vertices[qd->v2],
                            &nm->quad_vertices[qd->v3],
                        };
                        int ok = 1;
                        for (int v = 0; v < 4; v++) {
                            int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                            int32_t _vx = (int32_t)(((int64_t)_x*nbone_m[0] + (int64_t)_y*nbone_m[1] + (int64_t)_z*nbone_m[2]) >> 12) + nbone_t[0];
                            int32_t _vy = (int32_t)(((int64_t)_x*nbone_m[3] + (int64_t)_y*nbone_m[4] + (int64_t)_z*nbone_m[5]) >> 12) + nbone_t[1];
                            int32_t _vz = (int32_t)(((int64_t)_x*nbone_m[6] + (int64_t)_y*nbone_m[7] + (int64_t)_z*nbone_m[8]) >> 12) + nbone_t[2];
                            if (_vz < 64) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            int32_t _ir1 = _vx > 0x7FFF ? 0x7FFF : (_vx < -0x8000 ? -0x8000 : _vx);
                            int32_t _ir2 = _vy > 0x7FFF ? 0x7FFF : (_vy < -0x8000 ? -0x8000 : _vy);
                            uint32_t _sz3 = _vz > 0xFFFF ? 0xFFFFu : (uint32_t)_vz;
                            uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                            ax[v] = (float)(cx + (int)(((int64_t)_ir1 * (int64_t)_n) >> 16));
                            ay[v] = (float)(cy + (int)(((int64_t)_ir2 * (int64_t)_n) >> 16));
                            wz[v] = (float)_vz;
                        }
                        if (!ok) continue;
                        const re15_md1_quad_uv_t *uv = &nm->quad_uvs[qi];
                        int page_off = (int)((uv->page & 0x000F) * 128);
                        /* G-round: real depth for both quad-split triangles. */
                        int npc_qz1 = (int)((wz[0] + wz[1] + wz[3]) * (1.0f / 3.0f));
                        int npc_qz2 = (int)((wz[0] + wz[3] + wz[2]) * (1.0f / 3.0f));
                        /* BF-round: per-vertex shading for NPC quad. */
                        uint8_t nqr0, nqg0, nqb0, nqr1, nqg1, nqb1;
                        uint8_t nqr2, nqg2, nqb2, nqr3, nqg3, nqb3;
                        if (nm->quad_normals &&
                            qd->n0 < (uint32_t)nm->quad_normal_count &&
                            qd->n1 < (uint32_t)nm->quad_normal_count &&
                            qd->n2 < (uint32_t)nm->quad_normal_count &&
                            qd->n3 < (uint32_t)nm->quad_normal_count) {
                            const re15_md1_vertex_t *qn0 = &nm->quad_normals[qd->n0];
                            const re15_md1_vertex_t *qn1 = &nm->quad_normals[qd->n1];
                            const re15_md1_vertex_t *qn2 = &nm->quad_normals[qd->n2];
                            const re15_md1_vertex_t *qn3 = &nm->quad_normals[qd->n3];
                            re15_light_shade_vertex(&lctx_npc, qn0->x, qn0->y, qn0->z, &nqr0, &nqg0, &nqb0);
                            re15_light_shade_vertex(&lctx_npc, qn1->x, qn1->y, qn1->z, &nqr1, &nqg1, &nqb1);
                            re15_light_shade_vertex(&lctx_npc, qn2->x, qn2->y, qn2->z, &nqr2, &nqg2, &nqb2);
                            re15_light_shade_vertex(&lctx_npc, qn3->x, qn3->y, qn3->z, &nqr3, &nqg3, &nqb3);
                        } else {
                            nqr0=nqr1=nqr2=nqr3=g_re15_light_tint[0];
                            nqg0=nqg1=nqg2=nqg3=g_re15_light_tint[1];
                            nqb0=nqb1=nqb2=nqb3=g_re15_light_tint[2];
                        }
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[1], (int)ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                            (int)ax[3], (int)ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                            0, (int)uv->clut, npc_qz1,
                            nqr0, nqg0, nqb0, nqr1, nqg1, nqb1, nqr3, nqg3, nqb3);
                        re15_render_textured_tri_lit(
                            (int)ax[0], (int)ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                            (int)ax[3], (int)ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                            (int)ax[2], (int)ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                            0, (int)uv->clut, npc_qz2,
                            nqr0, nqg0, nqb0, nqr3, nqg3, nqb3, nqr2, nqg2, nqb2);
                    }
                }
            }

            /* I-round disable (2026-05-24): NPC name-label overlay
             * removed — was drawing "Elliot(x,z)" text on top of him. */

            /* Obj_model_set prop rendering: render each placed prop with its
             * OWN MD1 + TIM (slots 4..9 for obj_id 0..5). Helicopter body
             * (0x02), rotor (0x03), light disc (0x04), pilot (0x05), and
             * the helipad fixtures (0x00, 0x01) all get authentic textures.   */
            for (int pi = 0; pi < g_scd.prop_count && pi < 16; pi++) {
                if (!g_scd.props[pi].active) continue;
                /* Byte-true per-prop cull, SHARED with the PSX port (re15_prop_culled
                 * in re15_aot.h). Every room1170 prop is type 0 (real type byte pc[2],
                 * not the old pc[4]=BAND read) → whole-object FUN_80014368 region-quad
                 * cull, with a sink-gate (x<-25000) fallback for region-less cinematic
                 * cuts. This PC loop already renders all active props (no type gate),
                 * so only the cull rule changed; both ports now cull identically. */
                if (re15_prop_culled((int)g_scd.props[pi].obj_type,
                                     g_scd.props[pi].x, g_scd.props[pi].z,
                                     cam_has_region, cam_region_xs, cam_region_zs))
                    continue;
                int oid = (int)g_scd.props[pi].obj_id;
                const re15_md1_t *prop_md1 = NULL;
                int prop_md1_ok = 0;
                if (oid >= 0 && oid < 6 && s_room_prop_ok[oid]) {
                    prop_md1 = &s_room_prop_md1[oid];
                    prop_md1_ok = 1;
                    re15_render_pc_bind_tim_slot(4 + oid);
                }

                /* Render prop's MD1 mesh as TEXTURED triangles. */
                if (prop_md1_ok) {
                    int32_t prop_x = g_scd.props[pi].x;
                    int32_t prop_y = g_scd.props[pi].y;
                    int32_t prop_z = g_scd.props[pi].z;
                    int16_t prop_rx = g_scd.props[pi].rot_x;
                    int16_t prop_ry = g_scd.props[pi].rot_y;
                    int16_t prop_rz = g_scd.props[pi].rot_z;

                    /* BM-round 2026-05-29: REMOVED the BA-round whole-prop
                     * view_z>32000 far-clip. It was added on a FALSE premise —
                     * PSX has NO far-clip in its render loop (FUN_80016b54 only
                     * does the behind-camera near-clip OTZ<0). That cull made the
                     * cut-6 sky-view helicopter (props 2/3, flown far by sub12/13)
                     * VANISH mid-scene once its view_z crossed 32000, whereas PSX
                     * keeps it visible the whole scene. The per-vertex near-clip
                     * (_vz<64) below already guards div-by-zero; distant props
                     * shrink to sub-pixel naturally, matching PSX (the teleport-
                     * hidden fixtures become a ~2-3px dot, as on PSX). */
                    /* I3-round diag: log each prop's render pos once. */
                    static int s_prop_logged[16] = {0};
                    if (!s_prop_logged[pi]) {
                        s_prop_logged[pi] = 1;
                        fprintf(stderr, "[prop-render] pi=%d oid=0x%02X pos=(%d,%d,%d) rot=(%d,%d,%d) meshes=%d\n",
                                pi, oid, prop_x, prop_y, prop_z,
                                prop_rx, prop_ry, prop_rz, prop_md1->mesh_count);
                    }
                    /* Build full Euler rotation matrix (RE2 stock RotMatrix
                     * convention = Ry * Rx * Rz, per skeleton_common.c
                     * mat3_from_euler). Without rot_x the tail rotor (Obj
                     * 0x04, rot_x=1024 = 90°) would render flat-horizontal
                     * instead of vertical-on-the-side. Local names rsx/rcx/
                     * etc. avoid shadowing the outer-scope `cx,cy` which
                     * are the screen center used by projection below. */
                    int32_t rsx = re15_sin_q12((int)prop_rx), rcx = re15_cos_q12((int)prop_rx);
                    int32_t rsy = re15_sin_q12((int)prop_ry), rcy = re15_cos_q12((int)prop_ry);
                    int32_t rsz = re15_sin_q12((int)prop_rz), rcz = re15_cos_q12((int)prop_rz);
                    #define Q12_MUL(a,b)   ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12))
                    /* M = Ry * Rx * Rz (PSX YXZ Euler). Direct expansion: */
                    int32_t prot_q12[9];
                    /* Row 0 */
                    prot_q12[0] = Q12_MUL(rcz, rcy);
                    prot_q12[1] = -Q12_MUL(rsz, rcy);
                    prot_q12[2] = rsy;
                    /* Row 1 — 2-term int64-accumulated to mirror M-round Q12 fix. */
                    {
                        int64_t t1 = ((int64_t)rsz * (int64_t)rcx) << 12;
                        int64_t t2 = (int64_t)rcz * (int64_t)rsy * (int64_t)rsx;
                        prot_q12[3] = (int32_t)((t1 + t2) >> 24);
                    }
                    {
                        int64_t t1 = ((int64_t)rcz * (int64_t)rcx) << 12;
                        int64_t t2 = (int64_t)rsz * (int64_t)rsy * (int64_t)rsx;
                        prot_q12[4] = (int32_t)((t1 - t2) >> 24);
                    }
                    prot_q12[5] = -Q12_MUL(rcy, rsx);
                    /* Row 2 */
                    {
                        int64_t t1 = ((int64_t)rsz * (int64_t)rsx) << 12;
                        int64_t t2 = (int64_t)rcz * (int64_t)rsy * (int64_t)rcx;
                        prot_q12[6] = (int32_t)((t1 - t2) >> 24);
                    }
                    {
                        int64_t t1 = ((int64_t)rcz * (int64_t)rsx) << 12;
                        int64_t t2 = (int64_t)rsz * (int64_t)rsy * (int64_t)rcx;
                        prot_q12[7] = (int32_t)((t1 + t2) >> 24);
                    }
                    prot_q12[8] = Q12_MUL(rcy, rcx);
                    #undef Q12_MUL

                    /* CANONICAL per-bone (2026-06-02): a rigid prop is ONE "bone"
                     * with world rotation prot_q12. Build the world-space ctx
                     * (rot=NULL) then fold prot_q12 in once (L_world · N_world).
                     * Was body-only (prot_q12 to setup_actor) — for a single rigid
                     * rotation that was already equivalent, but we now route it
                     * through the same canonical helper for consistency. */
                    re15_actor_lightctx_t lctx_prop, lctx_prop_world;
                    if (g_re15_room_lights_ok &&
                        g_re15_active_cut >= 0 &&
                        g_re15_active_cut < g_re15_room_lights.cut_count) {
                        int32_t prop_pos_w[3] = { prop_x, prop_y, prop_z };
                        re15_light_setup_actor(
                            &g_re15_room_lights.cuts[g_re15_active_cut],
                            prop_pos_w, NULL, &lctx_prop_world);
                        re15_light_ctx_rotate_for_bone(&lctx_prop_world, prot_q12,
                                                       &lctx_prop);
                    } else {
                        memset(&lctx_prop, 0, sizeof(lctx_prop));
                    }

                    for (int hbi = 0; hbi < prop_md1->mesh_count; hbi++) {
                        const re15_md1_mesh_t *hm = &prop_md1->meshes[hbi];
                        int32_t world_trans[3] = { prop_x, prop_y, prop_z };
                        int32_t comb_rot[9];
                        int32_t comb_trans[3];
                        re15_camera_compose_view_bone(&cam_view, prot_q12, world_trans,
                                                       comb_rot, comb_trans);
                        int32_t bm[9], bt[3];   /* Q12 view×bone — byte-true integer RTPS (was float) */
                        for (int i = 0; i < 9; i++) bm[i] = comb_rot[i];
                        for (int i = 0; i < 3; i++) bt[i] = comb_trans[i];

                        /* Textured rendering using bound prop TIM slot. */
                        for (int ti = 0; ti < hm->triangle_count; ti++) {
                            const re15_md1_triangle_t *tr = &hm->triangles[ti];
                            if (tr->v0 >= (uint32_t)hm->tri_vertex_count) continue;
                            if (tr->v1 >= (uint32_t)hm->tri_vertex_count) continue;
                            if (tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
                            int ax[3], ay[3], wz_avg = 0;
                            int ok = 1;
                            const re15_md1_vertex_t *vp[3] = {
                                &hm->tri_vertices[tr->v0],
                                &hm->tri_vertices[tr->v1],
                                &hm->tri_vertices[tr->v2],
                            };
                            for (int v = 0; v < 3; v++) {
                                int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                                int32_t _vx = (int32_t)(((int64_t)_x*bm[0] + (int64_t)_y*bm[1] + (int64_t)_z*bm[2]) >> 12) + bt[0];
                                int32_t _vy = (int32_t)(((int64_t)_x*bm[3] + (int64_t)_y*bm[4] + (int64_t)_z*bm[5]) >> 12) + bt[1];
                                int32_t _vz = (int32_t)(((int64_t)_x*bm[6] + (int64_t)_y*bm[7] + (int64_t)_z*bm[8]) >> 12) + bt[2];
                                if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */
                                int32_t _ir1 = _vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx);
                                int32_t _ir2 = _vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy);
                                uint32_t _sz3 = (uint32_t)(_vz>0xFFFF?0xFFFF:_vz);
                                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                                ax[v] = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                                ay[v] = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                                wz_avg += (int)_vz;
                            }
                            if (!ok) continue;
                            wz_avg /= 3;
                            int wz_for_sort = wz_avg;
                            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
                            int page_off = (int)((uv->page & 0x000F) * 128);
                            /* BF-round: per-vertex shading for prop tri. */
                            uint8_t pr0, pg0, pb0, pr1, pg1, pb1, pr2, pg2, pb2;
                            if (hm->tri_normals &&
                                tr->n0 < (uint32_t)hm->tri_normal_count &&
                                tr->n1 < (uint32_t)hm->tri_normal_count &&
                                tr->n2 < (uint32_t)hm->tri_normal_count) {
                                const re15_md1_vertex_t *pn0 = &hm->tri_normals[tr->n0];
                                const re15_md1_vertex_t *pn1 = &hm->tri_normals[tr->n1];
                                const re15_md1_vertex_t *pn2 = &hm->tri_normals[tr->n2];
                                re15_light_shade_vertex(&lctx_prop, pn0->x, pn0->y, pn0->z, &pr0, &pg0, &pb0);
                                re15_light_shade_vertex(&lctx_prop, pn1->x, pn1->y, pn1->z, &pr1, &pg1, &pb1);
                                re15_light_shade_vertex(&lctx_prop, pn2->x, pn2->y, pn2->z, &pr2, &pg2, &pb2);
                            } else {
                                pr0 = pr1 = pr2 = g_re15_light_tint[0];
                                pg0 = pg1 = pg2 = g_re15_light_tint[1];
                                pb0 = pb1 = pb2 = g_re15_light_tint[2];
                            }
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[1], ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                                ax[2], ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                                0, (int)uv->clut, wz_for_sort,
                                pr0, pg0, pb0, pr1, pg1, pb1, pr2, pg2, pb2);
                        }
                        for (int qi = 0; qi < hm->quad_count; qi++) {
                            const re15_md1_quad_t *qd = &hm->quads[qi];
                            if (qd->v0 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v1 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v2 >= (uint32_t)hm->quad_vertex_count) continue;
                            if (qd->v3 >= (uint32_t)hm->quad_vertex_count) continue;
                            int ax[4], ay[4], wz_avg = 0;
                            const re15_md1_vertex_t *vp[4] = {
                                &hm->quad_vertices[qd->v0],
                                &hm->quad_vertices[qd->v1],
                                &hm->quad_vertices[qd->v2],
                                &hm->quad_vertices[qd->v3],
                            };
                            int ok = 1;
                            for (int v = 0; v < 4; v++) {
                                int32_t _x = vp[v]->x, _y = vp[v]->y, _z = vp[v]->z;
                                int32_t _vx = (int32_t)(((int64_t)_x*bm[0] + (int64_t)_y*bm[1] + (int64_t)_z*bm[2]) >> 12) + bt[0];
                                int32_t _vy = (int32_t)(((int64_t)_x*bm[3] + (int64_t)_y*bm[4] + (int64_t)_z*bm[5]) >> 12) + bt[1];
                                int32_t _vz = (int32_t)(((int64_t)_x*bm[6] + (int64_t)_y*bm[7] + (int64_t)_z*bm[8]) >> 12) + bt[2];
                                if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */
                                int32_t _ir1 = _vx>0x7FFF?0x7FFF:(_vx<-0x8000?-0x8000:_vx);
                                int32_t _ir2 = _vy>0x7FFF?0x7FFF:(_vy<-0x8000?-0x8000:_vy);
                                uint32_t _sz3 = (uint32_t)(_vz>0xFFFF?0xFFFF:_vz);
                                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                                ax[v] = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                                ay[v] = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                                wz_avg += (int)_vz;
                            }
                            if (!ok) continue;
                            wz_avg /= 4;
                            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];
                            int page_off = (int)((uv->page & 0x000F) * 128);
                            /* BF-round: per-vertex shading for prop quad. */
                            uint8_t pqr0, pqg0, pqb0, pqr1, pqg1, pqb1;
                            uint8_t pqr2, pqg2, pqb2, pqr3, pqg3, pqb3;
                            if (hm->quad_normals &&
                                qd->n0 < (uint32_t)hm->quad_normal_count &&
                                qd->n1 < (uint32_t)hm->quad_normal_count &&
                                qd->n2 < (uint32_t)hm->quad_normal_count &&
                                qd->n3 < (uint32_t)hm->quad_normal_count) {
                                const re15_md1_vertex_t *qn0 = &hm->quad_normals[qd->n0];
                                const re15_md1_vertex_t *qn1 = &hm->quad_normals[qd->n1];
                                const re15_md1_vertex_t *qn2 = &hm->quad_normals[qd->n2];
                                const re15_md1_vertex_t *qn3 = &hm->quad_normals[qd->n3];
                                re15_light_shade_vertex(&lctx_prop, qn0->x, qn0->y, qn0->z, &pqr0, &pqg0, &pqb0);
                                re15_light_shade_vertex(&lctx_prop, qn1->x, qn1->y, qn1->z, &pqr1, &pqg1, &pqb1);
                                re15_light_shade_vertex(&lctx_prop, qn2->x, qn2->y, qn2->z, &pqr2, &pqg2, &pqb2);
                                re15_light_shade_vertex(&lctx_prop, qn3->x, qn3->y, qn3->z, &pqr3, &pqg3, &pqb3);
                            } else {
                                pqr0=pqr1=pqr2=pqr3=g_re15_light_tint[0];
                                pqg0=pqg1=pqg2=pqg3=g_re15_light_tint[1];
                                pqb0=pqb1=pqb2=pqb3=g_re15_light_tint[2];
                            }
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[1], ay[1], (int)uv->u1 + page_off, (int)uv->v1,
                                ax[3], ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                                0, (int)uv->clut, wz_avg,
                                pqr0, pqg0, pqb0, pqr1, pqg1, pqb1, pqr3, pqg3, pqb3);
                            re15_render_textured_tri_lit(
                                ax[0], ay[0], (int)uv->u0 + page_off, (int)uv->v0,
                                ax[3], ay[3], (int)uv->u3 + page_off, (int)uv->v3,
                                ax[2], ay[2], (int)uv->u2 + page_off, (int)uv->v2,
                                0, (int)uv->clut, wz_avg,
                                pqr0, pqg0, pqb0, pqr3, pqg3, pqb3, pqr2, pqg2, pqb2);
                        }
                    }
                    continue;
                }

                /* Other props (helicopter parts, lights) — small markers. NOTE: the mustard tile
                 * below is a PORT PLACEHOLDER for props with no MD1 mesh, not a byte-true render;
                 * only its centre projection is made uniform with the integer GTE RTPS. */
                int32_t fx = g_scd.props[pi].x, fy = g_scd.props[pi].y, fz = g_scd.props[pi].z;
                int32_t vx = (int32_t)(((int64_t)cam_view.rot[0]*fx + (int64_t)cam_view.rot[1]*fy + (int64_t)cam_view.rot[2]*fz) >> 12) + cam_view.trans[0];
                int32_t vy = (int32_t)(((int64_t)cam_view.rot[3]*fx + (int64_t)cam_view.rot[4]*fy + (int64_t)cam_view.rot[5]*fz) >> 12) + cam_view.trans[1];
                int32_t vz = (int32_t)(((int64_t)cam_view.rot[6]*fx + (int64_t)cam_view.rot[7]*fy + (int64_t)cam_view.rot[8]*fz) >> 12) + cam_view.trans[2];
                if (vz < 64) continue;
                int32_t _ir1 = vx>0x7FFF?0x7FFF:(vx<-0x8000?-0x8000:vx);
                int32_t _ir2 = vy>0x7FFF?0x7FFF:(vy<-0x8000?-0x8000:vy);
                uint32_t _sz3 = (uint32_t)(vz>0xFFFF?0xFFFF:vz);
                uint32_t _n = re15_gte_divide((uint32_t)cam_view.fov_screen_dist, _sz3);
                int sx_p = cx + (int)(((int64_t)_ir1*(int64_t)_n)>>16);
                int sy_p = cy + (int)(((int64_t)_ir2*(int64_t)_n)>>16);
                int sz = (int)(((int64_t)2000 * _n) >> 16);
                if (sz < 8) sz = 8;
                if (sz > 80) sz = 80;
                re15_render_tile(sx_p - sz/8, sy_p - sz/8, sz/4, sz/4, 0, 200, 180, 50);
                (void)pi;
            }

            /* Cut/player HUD + projection marker silenced. */
            (void)active_cut_count;

            /* Phase ESP-C: draw the op-0x3a effect particles (after actors, in cam_view scope). */
            pc_fx_set_camf(rdt_buf, (size_t)rdt_size, (int)g_scd.cam_id);
            pc_draw_effects(&cam_view, cx, cy);
        }

        /* INVENTORY on top (Phase 8.26): the overlay above is drawn into the framebuffer, but end_frame
         * draws the queued 3D meshes (textured-tri queue) ON TOP of the framebuffer — which would cover
         * the inventory. When the menu is open, discard those meshes so the full-screen inventory shows. */
        if (re15_menu_is_open()) {
            extern void re15_render_pc_clear_textris(void);
            re15_render_pc_clear_textris();
        }

        /* ITEM-GET MODAL overlay: hand the current-frame item quad to the renderer. It is drawn
         * AFTER the 3D meshes in end_frame (see re15_render_pc_item_modal) so the zooming/flipping
         * item box floats ABOVE the frozen scene (the game is paused underneath — NOT cleared, unlike
         * the full-screen inventory). Byte-true FUN_8001db28; the box art is the per-item 112×72
         * picture from ITEM/ITPS.ITP (re15_itps_pixel, id×0x3000). */
        {
            extern void re15_render_pc_item_modal(int on, const int *cx, const int *cy,
                                                  uint8_t type, int face);
            int mqx[4], mqy[4], mface = 0; uint8_t mtype = 0;
            int mdraw = re15_item_modal_quad(mqx, mqy, &mtype, &mface);
            if (mdraw) re15_render_pc_item_modal(1, mqx, mqy, mtype, mface);
            else       re15_render_pc_item_modal(0, NULL, NULL, 0, 0);
            /* RE15_MODAL_LOG: FILE trace of the live modal FSM (stderr goes to the void for the SDL
             * exe) — proves the presentation ticks in the running game with the right progression. */
            {
                static FILE *ml = NULL; static int mli = 0;
                if (!mli) { mli = 1; const char *p = getenv("RE15_MODAL_LOG"); if (p && *p) ml = fopen(p, "w"); }
                if (ml && re15_item_modal_active()) {
                    fprintf(ml, "F%u state=%u f630=%d draw=%d type=0x%02x face=%d",
                            g_engine.frame_count, re15_item_modal_state(), re15_item_modal_frame(),
                            mdraw, mtype, mface);
                    if (mdraw) fprintf(ml, " quad TL(%d,%d) TR(%d,%d) BL(%d,%d) BR(%d,%d)",
                                       mqx[0],mqy[0], mqx[1],mqy[1], mqx[2],mqy[2], mqx[3],mqy[3]);
                    fprintf(ml, "\n"); fflush(ml);
                }
            }
        }

        /* ITEM-GET MODAL message box (states 5/6, byte-true FUN_80027e68 a1=0x100 → the message VM):
         * "WILL YOU TAKE THE <item>." with a Yes/No cursor when there's room, or "YOU CAN'T CARRY ANY
         * MORE ITEMS" when full. Box origin (34,180) is byte-true (DAT_800b8534=0x22 / 8536=0xb4); the
         * text glyphs + item name render in the real TEX.TIM message font via byte-true glyph replay
         * (re15_render_pc_item_prompt, render_pc.c:1681, using re15_msgfont_glyph; the prompt's own
         * glyph bytes come from the BSS scripts @0x800c4fc6 + name blob) — byte-true end-to-end, no
         * longer a 6×8-overlay faithful-line. */
        {
            extern void re15_render_pc_text_overlay(int x, int y, const char *text);
            extern int  re15_render_pc_text_overlay_n(int x, int y, const char *text, int n);
            extern void re15_render_pc_cursor(int x, int y);
            uint8_t ptype = 0; int pchoice = 0;
            int prompt = re15_item_modal_prompt(&ptype, &pchoice);
            int reveal = re15_item_modal_reveal();   /* typewriter budget (2 frames/glyph) */
            if (prompt) {
                /* Byte-true GAME-FONT render: replay the prompt's own glyph bytes (BSS scripts + name
                 * blob) through the TEX.TIM message font — "Will you take the <Item>." / "You can't
                 * carry any more items", exact font + Title-Case, typewritered to `reveal` glyphs. */
                extern void re15_render_pc_item_prompt(int x, int y, int prompt_type, uint8_t item_id, int reveal);
                re15_render_pc_item_prompt(34, 180, prompt, ptype, reveal);
                if (prompt == 1 && re15_item_modal_prompt_ready()) {  /* Yes/No only after the text types out */
                    static const unsigned char yes_g[3] = { 0x35, 0x41, 0x4f };  /* "Yes" (game glyphs) */
                    static const unsigned char no_g[2]  = { 0x2a, 0x4b };         /* "No"                */
                    extern int re15_render_pc_msg_text(int x, int y, const unsigned char *raw, int len);
                    re15_render_pc_msg_text(190, 202, yes_g, 3);
                    re15_render_pc_msg_text(234, 202, no_g,  2);
                    re15_render_pc_cursor(pchoice ? 224 : 180, 203);  /* ▶ on the current choice */
                }
            }
        }

        re15_render_end_frame();

        /* AH-round: 30 FPS cap. SDL2 VSync runs at the monitor refresh
         * (60/144/etc) which makes our engine run at non-PSX speeds.
         * SDL_Delay-based cap is portable + matches PSX 30Hz cinematic
         * pacing. Override via RE15_FPS env var (option-menu hook). */
        {
            static uint32_t last_tick = 0;
            uint32_t now = SDL_GetTicks();
            uint32_t elapsed = now - last_tick;
            if (elapsed < frame_budget_ms && last_tick != 0) {
                SDL_Delay(frame_budget_ms - elapsed);
            }
            last_tick = SDL_GetTicks();
        }
        /* AUTO-SCREENSHOT for ablauf diff (2026-05-24). When env
         * RE15_AUTOSHOT=1, snap frames at preset intervals from sub11
         * start through sub02 finish, dump as shots/NN.bmp. */
        {
            static const char *autoshot = NULL;
            static int s_autoshot_inited = 0;
            if (!s_autoshot_inited) {
                autoshot = getenv("RE15_AUTOSHOT");
                s_autoshot_inited = 1;
            }
            if (autoshot && *autoshot == '1') {
                extern void re15_render_pc_screenshot(const char *path);
                /* RE15_AUTOSHOT_SERIES=START,END,STEP captures every STEP-th
                 * frame in [START,END] for per-frame motion / jitter analysis.
                 * Bypasses the fixed shot_frames list. */
                static const char *series_env = NULL;
                static int s_series_init = 0;
                static int series_start = -1, series_end = -1, series_step = 1;
                static int next_series_capture = -1;
                /* CLIP_TEST=ALL bookkeeping: capture 5 shots per clip at
                 * fixed offsets within each 100-frame window. */
                static int s_clip_all_mode = 0;
                if (!s_series_init) {
                    series_env = getenv("RE15_AUTOSHOT_SERIES");
                    const char *ct = getenv("RE15_CLIP_TEST");
                    /* RE15_CLIP_TEST=ALL overrides series with a sweep. */
                    if (ct && (ct[0] == 'A' || ct[0] == 'a')) {
                        s_clip_all_mode = 1;
                        fprintf(stderr, "[autoshot] CLIP_TEST=ALL sweep — "
                                "capturing clips 0..23 at kf 0..4 each\n");
                    } else if (series_env) {
                        int a = -1, b = -1, c = 1;
                        if (sscanf(series_env, "%d,%d,%d", &a, &b, &c) >= 2) {
                            series_start = a;
                            series_end   = b;
                            series_step  = (c > 0 ? c : 1);
                            next_series_capture = a;
                        }
                    } else if (ct && *ct) {
                        /* Sensible default for single-clip mode so user
                         * doesn't have to remember the series env. Captures
                         * 5 anim-frame offsets: 5,15,25,35,45 → covers up
                         * to ~22 clip-frames at /2 ratio, enough for one
                         * full cycle of the typical 20-30f gait clip. */
                        series_start = 5;
                        series_end   = 45;
                        series_step  = 10;
                        next_series_capture = 5;
                        fprintf(stderr, "[autoshot] CLIP_TEST=%s default series 5..45 step 10\n", ct);
                    }
                    s_series_init = 1;
                }
                /* CLIP_TEST=ALL sweep: 5 captures per 100-frame clip window
                 * at offsets +5/+15/+25/+35/+45. Stops after clip 23 (frame
                 * 2400). */
                if (s_clip_all_mode) {
                    int f      = (int)g_engine.frame_count;
                    int clip_i = f / 100;
                    int off    = f - clip_i * 100;
                    int kf_idx_out = -1;
                    if      (off == 5)  kf_idx_out = 0;
                    else if (off == 15) kf_idx_out = 1;
                    else if (off == 25) kf_idx_out = 2;
                    else if (off == 35) kf_idx_out = 3;
                    else if (off == 45) kf_idx_out = 4;
                    if (clip_i < 24 && kf_idx_out >= 0) {
                        char path[96];
                        snprintf(path, sizeof path,
                                 "shots/clip_%02d_kf%02d.bmp",
                                 clip_i, kf_idx_out);
                        re15_render_pc_screenshot(path);
                    }
                    if (f >= 2400) {
                        fprintf(stderr, "[autoshot] CLIP_TEST=ALL done (clip 23 captured)\n");
                        running = 0;
                    }
                }
                else if (next_series_capture >= 0 &&
                    g_engine.frame_count == (uint32_t)next_series_capture) {
                    char path[96];
                    const char *ct = getenv("RE15_CLIP_TEST");
                    if (ct && *ct) {
                        /* CLIP_TEST mode: name shots by clip + keyframe-
                         * within-series so a sweep of clips produces
                         * comparable filenames. */
                        int kf_in_series =
                            (next_series_capture - series_start) / series_step;
                        snprintf(path, sizeof path,
                                 "shots/clip_%02d_kf%02d.bmp",
                                 atoi(ct), kf_in_series);
                    } else {
                        snprintf(path, sizeof path, "shots/series_f%05d.bmp",
                                 next_series_capture);
                    }
                    re15_render_pc_screenshot(path);
                    next_series_capture += series_step;
                    if (next_series_capture > series_end) {
                        fprintf(stderr, "[autoshot] series done\n");
                        running = 0;
                    }
                }
                /* Frames to capture (covers narrator + helipad cinematic +
                 * post-cinematic gameplay where Elliot should be despawned
                 * and rotor should be off-screen). */
                /* AI-round: scale shot_frames by target_fps. Original values
                 * tuned at 60fps; at 30fps each frame = 2x wall-time so we
                 * need to halve to capture the same cinematic moments. */
                static int shot_frames[14];
                static int shot_frames_inited = 0;
                if (!shot_frames_inited) {
                    int base[] = {600, 1850, 2100, 2904, 3022, 3500, 3700,
                                  3900, 4200, 4400, 4600, 5000, 5500, 6000};
                    for (int i = 0; i < 14; i++)
                        shot_frames[i] = FRAME_AT_60(base[i]);
                    shot_frames_inited = 1;
                }
                static int next_shot = 0;
                int N = (int)(sizeof(shot_frames)/sizeof(shot_frames[0]));
                if (series_env || s_clip_all_mode || getenv("RE15_CLIP_TEST")) {
                    /* series / clip-test mode active — skip canonical shots */
                }
                else if (next_shot < N && g_engine.frame_count >= (uint32_t)shot_frames[next_shot]) {
                    char path[64];
                    snprintf(path, sizeof path, "shots/%02d_f%d.bmp",
                             next_shot, shot_frames[next_shot]);
                    re15_render_pc_screenshot(path);
                    next_shot++;
                    if (next_shot >= N) {
                        /* Also dump TIM slots so we can compare with raw textures. */
                        extern void re15_render_pc_dump_slot(int slot, const char *path);
                        re15_render_pc_dump_slot(0, "shots/slot0_leon_tim.bmp");
                        re15_render_pc_dump_slot(1, "shots/slot1_elliot_tim.bmp");
                        re15_render_pc_dump_slot(4, "shots/slot4_obj0_tim.bmp");
                        re15_render_pc_dump_slot(5, "shots/slot5_obj1_tim.bmp");
                        re15_render_pc_dump_slot(6, "shots/slot6_obj2_heli_tim.bmp");
                        re15_render_pc_dump_slot(7, "shots/slot7_obj3_rotor_tim.bmp");
                        re15_render_pc_dump_slot(8, "shots/slot8_obj4_light_tim.bmp");
                        re15_render_pc_dump_slot(9, "shots/slot9_obj5_pilot_tim.bmp");
                        fprintf(stderr, "[autoshot] all %d shots captured, exiting\n", N);
                        running = 0;
                    }
                }
            }
        }
        g_engine.frame_count++;
    }
    /* FE-5.3: the death FSM set mode=TITLE and broke the game loop — go back to the title menu
     * (YOU DIED -> TITLE). CONTINUE there reloads the last card save; NEW GAME restarts. Any other
     * exit (SDL_QUIT calls exit() directly) falls through to a normal return. */
    if (re15_gameflow_mode() == RE15_MODE_TITLE) goto re_title;
    return 0;
}
