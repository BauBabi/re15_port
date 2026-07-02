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
#include <math.h>
#include <string.h>
#include <SDL_timer.h>    /* SDL_GetTicks/SDL_Delay — Frame-Timing (ohne main-Umleitung) */
#include "re15_engine.h"
#include "re15_tim.h"
#include "re15_scd.h"
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
#include "re15_ems.h"         /* enemy-model archive index (load EMDs out of CDEMD*.EMS) */
#include "re15_room_list.h"   /* GENERATED room-id list for the [ / ] debug room-browser */
#include "re15_room_spawns.h" /* GENERATED per-room entry spawn (inbound-door landing spot) */
#include "re15_anim_select.h"  /* SHARED actor bank/clip selection view-model */
#include "re15_esp.h"          /* Phase ESP-C: op-0x3a effect-sprite bank + particle pool */

#define RE15_TIM_SLOT_EFFECT 19   /* effect-sprite TIM render slot (0..18 used by chars/props) */
#define RE15_TIM_SLOT_EFFECT_GLOBAL 20 /* GLOBAL effect bank (CORE00.ESP) sprite sheet — effect-id 0
                                        * hit/blood. Its texture is NOT in any RDT; it lives in VRAM
                                        * (tpage 0x001f -> VRAM(960,256) 4-bit, clut 0x7951). Extracted
                                        * byte-true from the live ShowVRAM ground truth into a TIM
                                        * (tools/vram_png_to_tim.py) shipped at extracted_fx/effect0_blood.tim. */

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
static void pc_draw_effects(const re15_camera_view_t *cam, int cx, int cy)
{
    extern int  re15_render_pc_dbg_slot_loaded(int slot);
    extern void re15_render_pc_bind_tim_slot(int slot);
    const re15_esp_t *global_bank = re15_esp_global_bank();
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        /* Each particle animates from ITS OWN resolved bank: the room ESP (RDT-TIM slot 19) or the
         * GLOBAL bank CORE00.ESP (effect-id 0 hit/blood, whose sheet lives only in VRAM -> the
         * byte-true extracted TIM in slot 20). Pick the matching texture slot per particle. */
        const re15_esp_t *bank = f->bank;
        int slot = (bank && bank == global_bank) ? RE15_TIM_SLOT_EFFECT_GLOBAL
                                                  : RE15_TIM_SLOT_EFFECT;
        if (!re15_render_pc_dbg_slot_loaded(slot)) continue;   /* that bank's texture not loaded */
        re15_render_pc_bind_tim_slot(slot);
        float wx = (float)f->x, wy = (float)f->y, wz = (float)f->z;
        float vx = (cam->rot[0]*wx + cam->rot[1]*wy + cam->rot[2]*wz) / 4096.0f + cam->trans[0];
        float vy = (cam->rot[3]*wx + cam->rot[4]*wy + cam->rot[5]*wz) / 4096.0f + cam->trans[1];
        float vz = (cam->rot[6]*wx + cam->rot[7]*wy + cam->rot[8]*wz) / 4096.0f + cam->trans[2];
        if (vz < 1.0f) continue;                      /* behind / on the camera plane */
        float proj = (float)cam->fov_screen_dist / vz;
        int sx = cx + RNDI(vx * proj), sy = cy + RNDI(vy * proj);
        re15_esp_coord_t c = {0, 0, 0, 0};            /* byte-true cell origin (u,v) */
        if (bank && f->eff_idx >= 0) {
            int nb = bank->eff[f->eff_idx].count_b;
            if (nb > 0) re15_esp_coord(bank, f->eff_idx, f->frame % nb, &c);
        }
        int half = (int)(600.0f * proj); if (half < 3) half = 3; if (half > 120) half = 120;
        int u0 = c.u, v0 = c.v, u1 = c.u + 24, v1 = c.v + 24;   /* faithful 24px UV cell */
        int z = (int)vz >> 4;
        re15_render_textured_tri(sx-half, sy-half, u0, v0,  sx+half, sy-half, u1, v0,
                                 sx-half, sy+half, u0, v1,  0, 0, z, 128, 128, 128);
        re15_render_textured_tri(sx+half, sy-half, u1, v0,  sx+half, sy+half, u1, v1,
                                 sx-half, sy+half, u0, v1,  0, 0, z, 128, 128, 128);
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

/* Globalization Phase 3-B (2026-06-13): read a global asset from the SHARED tree
 * (psx_dev/assets_shared/<rel>), trying the run-dir-relative roots (build/Debug,
 * build/Release, project root, …). Replaces the per-asset 4-5 candidate
 * re15_reborn/assets/ path chains so the PC engine sources from assets_shared like
 * the PSX disc does. Returns a malloc'd buffer (caller frees/keeps); *size set; NULL
 * if not found in any root. */
static uint8_t *pc_read_shared(const char *rel, int *size)
{
    /* Höchste Priorität: expliziter Asset-Wurzel-Pfad aus der Umgebung
     * (RE15_ASSET_ROOT). Macht den nach re15_port verschobenen Build
     * cwd-unabhängig — er findet den extrahierten assets_shared-Baum unabhängig
     * davon, von wo aus die .exe gestartet wird. */
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
    /* Original-CD-Baum (shared_assets/PSX) für Assets, die NICHT im extrahierten
     * assets_shared liegen: DATA/CORE00.ESP (globale Effekt-Bank) sowie die daraus/aus
     * VRAM abgeleiteten Effekt-Texturen unter extracted_fx/ (Geschwister von PSX). Gleiche
     * Konvention wie render_pc.c (RE15_CD_ROOT env übersteuert den Compile-Default). */
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
    static const char *roots[] = {
        "../../assets_shared/", "../assets_shared/", "../../../assets_shared/",
        "psx_dev/assets_shared/", "assets_shared/",
        "../re15_reborn/", "../../re15_reborn/", "../../../re15_reborn/",
        /* Fallback for repositories that still store runtime content here. */
        "../re15_reborn/assets/", "../../re15_reborn/assets/", "../../../re15_reborn/assets/",
        "psx_dev/re15_reborn/", "psx_dev/re15_reborn/assets/",
        "re15_reborn/", "re15_reborn/assets/",
        NULL
    };
    static const struct {
        const char *rel;
        const char *legacy[4];
    } aliases[] = {
        { "PLD/PL00.TIM",   { "TEST.TIM",     "test.tim",     NULL } },
        { "PLD/PL00.MD1",   { "test.md1",     "pl00_model.bin", NULL } },
        { "PLD/ELLIOT.TIM", { "ELLIOT.TIM",   "elliot.tim",   NULL } },
        { "PLD/ELLIOT.MD1", { "ELLIOT.MD1",   "elliot.md1",   NULL } },
        { "PLD/ELLIOT.EDD", { "elliot.edd",   NULL } },
        { "PLD/ELLIOT.EMR", { "elliot.emr",   NULL } },
        { "PLD/PL00W01.EDD", { "pl00w01.edd", NULL } },
        { "PLD/PL00W01.EMR", { "pl00w01.emr", NULL } },
        { NULL, { NULL } }
    };
    char path[192];
    for (int i = 0; roots[i]; i++) {
        snprintf(path, sizeof path, "%s%s", roots[i], rel);
        uint8_t *b = re15_asset_read_file(path, size);
        if (b) return b;
    }
    {
        const char *leaf = strrchr(rel, '/');
        if (leaf && leaf[1]) {
            char lower_leaf[64];
            leaf++;
            for (int i = 0; roots[i]; i++) {
                snprintf(path, sizeof path, "%s%s", roots[i], leaf);
                uint8_t *b = re15_asset_read_file(path, size);
                if (b) return b;
            }
            size_t n = strlen(leaf);
            if (n >= sizeof lower_leaf) n = sizeof lower_leaf - 1;
            for (size_t i = 0; i < n; i++) {
                char ch = leaf[i];
                lower_leaf[i] = (ch >= 'A' && ch <= 'Z') ? (char)(ch - 'A' + 'a') : ch;
            }
            lower_leaf[n] = '\0';
            for (int i = 0; roots[i]; i++) {
                snprintf(path, sizeof path, "%s%s", roots[i], lower_leaf);
                uint8_t *b = re15_asset_read_file(path, size);
                if (b) return b;
            }
        }
    }
    for (int i = 0; aliases[i].rel; i++) {
        if (strcmp(rel, aliases[i].rel) != 0) continue;
        for (int j = 0; aliases[i].legacy[j]; j++) {
            for (int k = 0; roots[k]; k++) {
                snprintf(path, sizeof path, "%s%s", roots[k], aliases[i].legacy[j]);
                uint8_t *b = re15_asset_read_file(path, size);
                if (b) return b;
            }
        }
        break;
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

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    /* Phase 4.5.10-J: redirect stderr to debug.log so user can read
     * exact numerical state. */
    freopen("debug.log", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);   /* unbuffered for live tail */

    re15_render_init();
    re15_input_init();

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
    /* Phase 4.5.12: boot directly into ROOM1170 cut 0 BG (intro view).
     * Fall back to the bundled test asset only if the per-cut file is
     * missing, so dev builds without ROOM1170 BSS still show something. */
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
    unsigned boot_room = (start_room && *start_room) ? (unsigned)strtoul(start_room, 0, 16) : 0x1240;
    char rdt_path[32]; snprintf(rdt_path, sizeof rdt_path, "RDT/ROOM%04X.RDT", boot_room);
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
        /* The GLOBAL effect textures live only in VRAM (no RDT TIM). effect-id 0 (hit/blood) was
         * extracted byte-true from the live ShowVRAM ground truth into a TIM; upload it to the
         * dedicated global-effect slot so pc_draw_effects can bind it for global-bank particles. */
        int bsz = 0;
        uint8_t *bloodtim = pc_read_shared("extracted_fx/effect0_blood.tim", &bsz);
        if (bloodtim) {
            re15_tim_t btim;
            if (re15_tim_parse(bloodtim, bsz, &btim) == 0) {
                re15_render_pc_upload_tim_slot(&btim, RE15_TIM_SLOT_EFFECT_GLOBAL);
                fprintf(stderr, "[esp] global effect-0 blood TIM -> slot %d: %dx%d %dbpp\n",
                        RE15_TIM_SLOT_EFFECT_GLOBAL, btim.width, btim.height, btim.bpp);
            } else {
                fprintf(stderr, "[esp] effect0_blood.tim parse FAILED\n");
            }
            free(bloodtim);
        } else {
            fprintf(stderr, "[esp] effect0_blood.tim NOT found\n");
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
    while (running) {
        re15_render_begin_frame();
        re15_input_tick();

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
        if (target_fps == 30 || (g_engine.frame_count & 1) == 0) {
            scd_vm_tick();
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
                if (!re15_pc_draw_item_icon(cx, cy, id)) {
                    char t[8]; snprintf(t, sizeof t, "%02X", id);
                    re15_debug_text(cx + 2, cy + 2, 0, t);
                }
                uint8_t q = re15_menu_disp_qty(c);
                if (q > 0) { char t[8]; snprintf(t, sizeof t, "%d", q); re15_debug_text(cx + 22, cy + 20, 0, t); }
            }

            /* selected item name + controls */
            if (n > 0) {
                uint8_t id = re15_menu_disp_id(cur);
                const char *ty = re15_item_is_weapon(id) ? "WEAPON"
                               : re15_item_is_ammo(id)   ? "AMMO" : "ITEM";
                char line[64];
                snprintf(line, sizeof line, "%s [%s]  Enter=equip Shift=close", re15_item_name(id), ty);
                re15_debug_text(158, 224, 0, line);
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
            re15_camera_build_view(&active_cuts[active_cut_idx], &cam_view);

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

            /* Shadow orientation = the CAMERA Y-angle (RE1.5 FUN_8001b064 uses
             * RotMatrixY(cam+0x6a), NOT the actor facing). A 500×600 floor quad
             * rotated by the actor's yaw swings off-screen-axis so one foot falls
             * outside it; rotating by the camera yaw keeps it screen-aligned over
             * the figure. Build RotY(cam_yaw) from the cut forward vector. */
            /* Camera-yaw RotY from the cut forward — SHARED with the PSX shadow via
             * re15_camera_yaw_matrix (integer isqrt; was a double sqrt here). */
            int32_t sh_cam_yaw_rot[9];
            re15_camera_yaw_matrix(
                active_cuts[active_cut_idx].target_x - active_cuts[active_cut_idx].pos_x,
                active_cuts[active_cut_idx].target_z - active_cuts[active_cut_idx].pos_z,
                sh_cam_yaw_rot);

            /* BJ-round 2026-05-29: cinematic letterbox. PSX cutscenes (the intro)
             * draw ~17px black bars top+bottom (measured from ablauf 217030:
             * content rows 77..1003 of 1080 → ~17px in 240-space), hiding the
             * topmost building/window row that's visible during gameplay
             * (non-cutscene.png). Gate on the cinematic flags sub02 sets. */
            {
                extern void re15_render_pc_set_letterbox(int h);
                /* Letterbox is its OWN render FSM (PSX FUN_80020f8c PolyF4 quads +
                 * FUN_80021a0c driver), orthogonal to player_mode. On while scripted
                 * (countdown=-1) and during the close after Plc_ret; off at 0.
                 * BO-round (Tier-3 #3): 24px = canonical PSX bar height (POLY_F4
                 * y2 = 0x18; top 0..24, bottom 216..240), NOT the old 17px ablauf
                 * measurement. */
                re15_render_pc_set_letterbox(g_scd.letterbox_countdown != 0 ? 24 : 0);

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
                gctx.pad_current = (uint16_t)g_engine.pad_current;
                gctx.pad_pressed = (uint16_t)g_engine.pad_pressed;
                re15_game_step(&gctx);
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
            /* DEV ROOM-BROWSER (globalization 2026-06-13): [ / ] cycle through every
             * room in the shared tree (re15_room_list.h) by queuing a g_room_change —
             * the SAME consume path below then loads it (RDT + render reset + props +
             * scd_room_reenter + BG + light + msg, all from the RDT). Lets us load ANY
             * of the 240 rooms and see how far it gets. Guarded so it never overrides a
             * door-queued change. Spawn (0,0,0)/cut 0 — may land in a wall, fine for a
             * browse. PC-only (PSX would need every room on the disc). */
            {
                int rsel = 0;
                if      (g_engine.pad_pressed & 0x0200) rsel = +1;   /* R2 = ] next room */
                else if (g_engine.pad_pressed & 0x0100) rsel = -1;   /* L2 = [ prev room */
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
             * cut names are still room1170-hardcoded — room-aware BG/model asset
             * loading is the separate multi-room TODO; the destination RDT,
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
            /* PC-ONLY DEV TOOL (NOT gameplay; architecture-irrelevant): the
             * motion-lock + PL00 clip cycler. Applied AFTER the shared step so
             * it overrides SCD/Plc_motion for visual clip identification.
             * Tab=lock, PageUp/PageDown=cycle. */
            {
                static int  s_motion_lock = 0;
                static int  s_locked_clip = 0;
                if (g_engine.pad_pressed & 0x0001 /* SELECT/Tab */) {
                    s_motion_lock = !s_motion_lock;
                    s_locked_clip = (int)g_actors[RE15_ACTOR_SLOT_PLAYER].motion;
                }
                int cyc = 0;
                if      (g_engine.pad_pressed & 0x1000 /* PageUp   */) cyc = +1;
                else if (g_engine.pad_pressed & 0x2000 /* PageDown */) cyc = -1;
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
            re15_anim_banks_t banks = {
                .def_mesh = &md1, .def_skel = &skel, .def_anim = &anim,
                .w01_skel = &w01_skel, .w01_anim = &w01_anim, .w01_ok = w01_ok,
                .pl00_skel = &pl00_skel, .pl00_anim = &pl00_anim, .pl00_ok = pl00_ok,
                .elliot_mesh = &elliot_md1, .elliot_skel = &elliot_skel,
                .elliot_anim = &elliot_anim, .elliot_ok = (elliot_ok && elliot_skel_ok),
            };
            re15_anim_view_t av;
            re15_actor_anim_select(player_ref, 1, &banks, &av);
            const re15_emd_skeleton_t  *p_skel = av.skel;
            const re15_emd_animation_t *p_anim = av.anim;
            int kf_idx = 0;
            if (player_visible && skel_ok && p_anim->clip_count > 0) {
                /* The platform owns the fps policy: at 30fps target anim_frame is
                 * already 30Hz; at 60fps halve to PSX-canonical 30Hz. */
                uint32_t cur = (target_fps == 30)
                    ? (uint32_t)player_ref->anim_frame
                    : ((uint32_t)player_ref->anim_frame >> 1);
                kf_idx = re15_compute_actor_kf(p_anim, p_skel, player_ref,
                                               av.clip_override, cur);
            }
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            int pose_ok = 0;
            if (player_visible && skel_ok) {
                g_anim_pose_actor = player_ref;   /* FRAC crossfade for the player body */
                pose_ok = (re15_skel_compute_pose(p_skel, kf_idx, poses) == 0);
            }
            /* Per-bone composed matrix `view × bone_world` (Q12 rotation
             * + world-unit translation, float copies for in-macro math).
             * Set inside the mesh loop below by re15_camera_compose_
             * view_bone. */
            float bone_m[9] = {4096,0,0,  0,4096,0,  0,0,4096};
            float bone_t[3] = {0, 0, 0};

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
#define PROJECT_VERT(vp, out_sx, out_sy, out_wz) do { \
                float _x = (float)(vp)->x; \
                float _y = (float)(vp)->y; \
                float _z = (float)(vp)->z; \
                float _vx = (_x*bone_m[0] + _y*bone_m[1] + _z*bone_m[2]) / 4096.0f + bone_t[0]; \
                float _vy = (_x*bone_m[3] + _y*bone_m[4] + _z*bone_m[5]) / 4096.0f + bone_t[1]; \
                float _vz = (_x*bone_m[6] + _y*bone_m[7] + _z*bone_m[8]) / 4096.0f + bone_t[2]; \
                if (_vz < 64.0f) { (out_wz) = -1.0f; (out_sx) = 0; (out_sy) = 0; } \
                else { \
                    float _proj = screen_dist / _vz; \
                    (out_sx) = cx + RNDI(_vx * _proj); \
                    (out_sy) = cy + RNDI(_vy * _proj); \
                    (out_wz) = _vz; \
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
                static const int32_t SH_HALF_X = 500;   /* a2 = 0x1F4 */
                static const int32_t SH_HALF_Z = 600;   /* a3 = 0x258 */
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
                re15_camera_compose_view_bone(&cam_view, sh_cam_yaw_rot, sh_world,
                                              sh_rot, sh_trans);
                float sbm[9], sbt[3];
                for (int i = 0; i < 9; i++) sbm[i] = (float)sh_rot[i];
                for (int i = 0; i < 3; i++) sbt[i] = (float)sh_trans[i];
                int sx[4], sy[4], sok = 1;
                for (int v = 0; v < 4; v++) {
                    float _x = (float)sh_corner[v][0];
                    float _y = (float)sh_corner[v][1];
                    float _z = (float)sh_corner[v][2];
                    float _vx = (_x*sbm[0] + _y*sbm[1] + _z*sbm[2]) / 4096.0f + sbt[0];
                    float _vy = (_x*sbm[3] + _y*sbm[4] + _z*sbm[5]) / 4096.0f + sbt[1];
                    float _vz = (_x*sbm[6] + _y*sbm[7] + _z*sbm[8]) / 4096.0f + sbt[2];
                    if (_vz < 64.0f) { sok = 0; break; }   /* H28 near-clip */
                    float _proj = screen_dist / _vz;
                    sx[v] = cx + RNDI(_vx * _proj);
                    sy[v] = cy + RNDI(_vy * _proj);
                }
                if (sok)
                    re15_render_shadow_quad(sx[0], sy[0], sx[1], sy[1],
                                            sx[2], sy[2], sx[3], sy[3]);
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
                for (int k = 0; k < 9; k++) bone_m[k] = (float)combined_rot[k];
                bone_t[0] = (float)combined_trans[0];
                bone_t[1] = (float)combined_trans[1];
                bone_t[2] = (float)combined_trans[2];

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
                    int32_t nfs = re15_sin_q12((int)npc->rot_y);
                    int32_t nfc = re15_cos_q12((int)npc->rot_y);
                    int32_t nyaw[9] = { nfc, 0, nfs,  0, 0x1000, 0,  -nfs, 0, nfc };
                    int32_t nsh_c[4][3] = {
                        { -500, 0,  600 }, { -500, 0, -600 },
                        {  500, 0,  600 }, {  500, 0, -600 },
                    };
                    int32_t nsh_world[3] = { npc->x, npc->y, npc->z };
                    int32_t nsh_rot[9], nsh_trans[3];
                    (void)nyaw;   /* shadow uses CAMERA yaw, not actor facing (FUN_8001b064) */
                    re15_camera_compose_view_bone(&cam_view, sh_cam_yaw_rot, nsh_world,
                                                  nsh_rot, nsh_trans);
                    float nm[9], nt[3];
                    for (int i = 0; i < 9; i++) nm[i] = (float)nsh_rot[i];
                    for (int i = 0; i < 3; i++) nt[i] = (float)nsh_trans[i];
                    int nsx[4], nsy[4], nok = 1;
                    for (int v = 0; v < 4; v++) {
                        float _x = (float)nsh_c[v][0], _y = (float)nsh_c[v][1], _z = (float)nsh_c[v][2];
                        float _vx = (_x*nm[0] + _y*nm[1] + _z*nm[2]) / 4096.0f + nt[0];
                        float _vy = (_x*nm[3] + _y*nm[4] + _z*nm[5]) / 4096.0f + nt[1];
                        float _vz = (_x*nm[6] + _y*nm[7] + _z*nm[8]) / 4096.0f + nt[2];
                        if (_vz < 64.0f) { nok = 0; break; }
                        float _proj = (float)cam_view.fov_screen_dist / _vz;
                        nsx[v] = cx + RNDI(_vx * _proj);
                        nsy[v] = cy + RNDI(_vy * _proj);
                    }
                    if (nok)
                        re15_render_shadow_quad(nsx[0], nsy[0], nsx[1], nsy[1],
                                                nsx[2], nsy[2], nsx[3], nsy[3]);
                }

                /* GENERIC ENEMY (globalization 2026-06-13; em21 folded 2026-06-14f): first
                 * time an actor of a non-player, non-Elliot type appears (INCLUDING the
                 * type-0x21 crows), lazy-load its EM<NN>.EMD into the shared registry; the
                 * selector then routes it to its own model (av.pc_tex_slot = its slot). */
                if (npc->active && npc->type && npc->type != 0x47
                    && !re15_enemy_find(npc->type))
                    pc_enemy_load(npc->type);

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

                    float nbone_m[9], nbone_t[3];
                    for (int i = 0; i < 9; i++) nbone_m[i] = (float)ncomb_rot[i];
                    for (int i = 0; i < 3; i++) nbone_t[i] = (float)ncomb_trans[i];

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
                            float _x = (float)vp[v]->x, _y = (float)vp[v]->y, _z = (float)vp[v]->z;
                            float _vx = (_x*nbone_m[0] + _y*nbone_m[1] + _z*nbone_m[2]) / 4096.0f + nbone_t[0];
                            float _vy = (_x*nbone_m[3] + _y*nbone_m[4] + _z*nbone_m[5]) / 4096.0f + nbone_t[1];
                            float _vz = (_x*nbone_m[6] + _y*nbone_m[7] + _z*nbone_m[8]) / 4096.0f + nbone_t[2];
                            if (_vz < 64.0f) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            float _proj = (float)cam_view.fov_screen_dist / _vz;
                            ax[v] = (float)(cx + RNDI(_vx * _proj));
                            ay[v] = (float)(cy + RNDI(_vy * _proj));
                            wz[v] = _vz;
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
                            float _x = (float)vp[v]->x, _y = (float)vp[v]->y, _z = (float)vp[v]->z;
                            float _vx = (_x*nbone_m[0] + _y*nbone_m[1] + _z*nbone_m[2]) / 4096.0f + nbone_t[0];
                            float _vy = (_x*nbone_m[3] + _y*nbone_m[4] + _z*nbone_m[5]) / 4096.0f + nbone_t[1];
                            float _vz = (_x*nbone_m[6] + _y*nbone_m[7] + _z*nbone_m[8]) / 4096.0f + nbone_t[2];
                            if (_vz < 64.0f) { ok = 0; break; } /* BO: H28 near-plane (was 1.0) */
                            float _proj = (float)cam_view.fov_screen_dist / _vz;
                            ax[v] = (float)(cx + RNDI(_vx * _proj));
                            ay[v] = (float)(cy + RNDI(_vy * _proj));
                            wz[v] = _vz;
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
                        float bm[9], bt[3];
                        for (int i = 0; i < 9; i++) bm[i] = (float)comb_rot[i];
                        for (int i = 0; i < 3; i++) bt[i] = (float)comb_trans[i];

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
                                float _x = (float)vp[v]->x, _y = (float)vp[v]->y, _z = (float)vp[v]->z;
                                float _vx = (_x*bm[0] + _y*bm[1] + _z*bm[2]) / 4096.0f + bt[0];
                                float _vy = (_x*bm[3] + _y*bm[4] + _z*bm[5]) / 4096.0f + bt[1];
                                float _vz = (_x*bm[6] + _y*bm[7] + _z*bm[8]) / 4096.0f + bt[2];
                                if (_vz < 64.0f) { ok = 0; break; }  /* H28 near-clip */
                                float _proj = (float)cam_view.fov_screen_dist / _vz;
                                ax[v] = cx + RNDI(_vx * _proj);
                                ay[v] = cy + RNDI(_vy * _proj);
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
                                float _x = (float)vp[v]->x, _y = (float)vp[v]->y, _z = (float)vp[v]->z;
                                float _vx = (_x*bm[0] + _y*bm[1] + _z*bm[2]) / 4096.0f + bt[0];
                                float _vy = (_x*bm[3] + _y*bm[4] + _z*bm[5]) / 4096.0f + bt[1];
                                float _vz = (_x*bm[6] + _y*bm[7] + _z*bm[8]) / 4096.0f + bt[2];
                                if (_vz < 64.0f) { ok = 0; break; }  /* H28 near-clip */
                                float _proj = (float)cam_view.fov_screen_dist / _vz;
                                ax[v] = cx + RNDI(_vx * _proj);
                                ay[v] = cy + RNDI(_vy * _proj);
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

                /* Other props (helicopter parts, lights) — small markers. */
                float fx = (float)g_scd.props[pi].x;
                float fy = (float)g_scd.props[pi].y;
                float fz = (float)g_scd.props[pi].z;
                float vx = (cam_view.rot[0]*fx + cam_view.rot[1]*fy + cam_view.rot[2]*fz) / 4096.0f + cam_view.trans[0];
                float vy = (cam_view.rot[3]*fx + cam_view.rot[4]*fy + cam_view.rot[5]*fz) / 4096.0f + cam_view.trans[1];
                float vz = (cam_view.rot[6]*fx + cam_view.rot[7]*fy + cam_view.rot[8]*fz) / 4096.0f + cam_view.trans[2];
                if (vz < 1.0f) continue;
                float proj = (float)cam_view.fov_screen_dist / vz;
                int sx_p = cx + RNDI(vx * proj);
                int sy_p = cy + RNDI(vy * proj);
                int sz = (int)(2000.0f * proj);
                if (sz < 8) sz = 8;
                if (sz > 80) sz = 80;
                re15_render_tile(sx_p - sz/8, sy_p - sz/8, sz/4, sz/4, 0, 200, 180, 50);
                (void)pi;
            }

            /* Cut/player HUD + projection marker silenced. */
            (void)active_cut_count;

            /* Phase ESP-C: draw the op-0x3a effect particles (after actors, in cam_view scope). */
            pc_draw_effects(&cam_view, cx, cy);
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
    return 0;
}
