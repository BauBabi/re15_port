/*
 * RE1.5 Rebuilt — Background-frame subsystem, PC backend
 * (Phase 4.5.6.4, 2026-05-18).
 *
 * Software MDEC path (PSX uses libpsxpress hardware MDEC chip). The
 * decode pipeline matches the PSX backend conceptually:
 *
 *   1. re15_bg_init()                 — no-op on PC (no chip to reset)
 *   2. re15_bg_load_from_bss()        — VLC + software IDCT + YUV→RGB
 *                                       → 320×240 RGBA cache in main RAM
 *   3. re15_bg_blit(x, y)             — memcpy the cache into the PC
 *                                       software framebuffer
 *
 * Asset path differs: PC reads from disk (../assets/test.bss relative
 * to the executable), while PSX uses an incbin'd buffer in EXE rodata.
 *
 * Visual parity vs PSX: the algorithm matches the Java extractor's
 * MdecDecoder which has produced the reference BMPs we've been working
 * from. Acceptable color drift vs the PSX hardware MDEC exists due to
 * the revengi Cr/Cb swap quirk (documented in bss_mdec.c). For Phase
 * 4.5 demo this is good enough; #PC-N improvements can later swap to
 * the standard MDEC interpretation if needed.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>      /* snprintf for per-cut path build */
#include <stdlib.h>
#include <string.h>
#include "re15_engine.h"
#include "re15_bss.h"
#include "re15_room.h"   /* g_current_room_id — room-aware BG selection */
#include "re15_bg.h"
#include "re15_tim.h"    /* re15_tim_parse — per-cut foreground atlas decode */

/* PC asset helpers — declared in asset_pc.c */
extern uint8_t  *re15_asset_read_file(const char *path, int *out_size);
extern uint32_t *re15_pc_framebuffer(void);

/* AZ-round 2026-05-28: sprite.pri overdraw — re-blit BG patches over
 * character pixels via SDL_RenderCopy in render_pc.c (characters use
 * SDL_RenderGeometry, which sits ABOVE our software framebuffer).
 * Declared in render_pc.c. */
extern void re15_render_pc_set_bg_image(const uint32_t *bg_rgba_320x240);
extern void re15_render_pc_set_pri_atlas(const uint32_t *rgba, int w, int h);

/* sprite.pri FOREGROUND-ATLAS loader (2026-06-09): load the per-cut atlas TIM
 * (room1170_priNN.tim — our BSS-SLD decode, byte-true game pixels), convert 8-bit
 * +CLUT → RGBA with index0 keyed transparent, hand it to the renderer's overdraw.
 * Returns 1 if the cut has a foreground atlas, 0 otherwise (overdraw stays off). */
static const char *re15_bg_room_prefix(unsigned room_id);   /* fwd (defined below) */
static uint32_t s_pri_atlas_rgba[256 * 256];

static const char *const s_pc_bg_roots[] = {
#ifdef RE15_ASSET_ROOT_DEFAULT
    /* Verschiebung nach re15_port: absolute Default-Wurzel (psx_dev/assets_shared)
     * als Erstes — cwd-unabhängig. Gleicher Mechanismus wie pc_read_shared in main.c. */
    RE15_ASSET_ROOT_DEFAULT "/",
#endif
    "../../assets_shared/", "../assets_shared/", "../../../assets_shared/",
    "psx_dev/assets_shared/", "assets_shared/",
    "../re15_reborn/", "../../re15_reborn/", "../../../re15_reborn/",
    "../re15_reborn/assets/", "../../re15_reborn/assets/", "../../../re15_reborn/assets/",
    "psx_dev/re15_reborn/", "psx_dev/re15_reborn/assets/",
    "re15_reborn/", "re15_reborn/assets/",
    NULL
};

int re15_pri_load_cut_atlas(int cut_idx)
{
    /* Globalization Phase 3-B (2026-06-13): read from the shared tree's disc layout
     * (assets_shared/BSS/ROOM%04X/PRI%02d.TIM) instead of the old flat
     * re15_reborn/assets/room####_pri##.tim. */
    char rel[96];
    uint8_t *buf = NULL; int sz = 0;
    for (int i = 0; s_pc_bg_roots[i] && !buf; i++) {
        snprintf(rel, sizeof rel, "%sBSS/ROOM%04X/PRI%02d.TIM", s_pc_bg_roots[i], g_current_room_id, cut_idx);
        buf = re15_asset_read_file(rel, &sz);
    }
    for (int i = 0; s_pc_bg_roots[i] && !buf; i++) {
        snprintf(rel, sizeof rel, "%sroom%04x_pri%02d.tim", s_pc_bg_roots[i], g_current_room_id, cut_idx);
        buf = re15_asset_read_file(rel, &sz);
    }
    if (!buf) { re15_render_pc_set_pri_atlas(NULL, 0, 0); return 0; }
    re15_tim_t tim;
    if (re15_tim_parse(buf, sz, &tim) != 0 || tim.bpp != 8 || !tim.has_clut ||
        tim.width * tim.height > (int)(sizeof s_pri_atlas_rgba / sizeof s_pri_atlas_rgba[0])) {
        free(buf); re15_render_pc_set_pri_atlas(NULL, 0, 0); return 0;
    }
    const uint8_t *idx = (const uint8_t *)tim.pixels;
    for (int i = 0; i < tim.width * tim.height; i++) {
        uint8_t ix = idx[i];
        if (ix == 0) { s_pri_atlas_rgba[i] = 0; continue; }   /* index 0 = transparent */
        uint16_t c = tim.clut[ix];
        uint32_t r = (uint32_t)((c & 0x1f) << 3), g = (uint32_t)(((c >> 5) & 0x1f) << 3),
                 b = (uint32_t)(((c >> 10) & 0x1f) << 3);
        s_pri_atlas_rgba[i] = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
    }
    int w = tim.width, h = tim.height;
    free(buf);
    re15_render_pc_set_pri_atlas(s_pri_atlas_rgba, w, h);
    return 1;
}

#define BG_WIDTH   320
#define BG_HEIGHT  240
#define BG_PIXELS  (BG_WIDTH * BG_HEIGHT)

/* Cached BG frame, RGBA8888 layout (matches the PC framebuffer). */
static uint32_t s_bg_cache[BG_PIXELS];
static int      s_bg_loaded = 0;

void re15_bg_init(void)
{
    /* Nothing to do — software decode has no per-process setup. */
}

int re15_bg_load_from_bss(const uint8_t *bss_chunk, size_t size)
{
    /* 1. Parse header. */
    re15_bss_chunk_t chunk;
    if (!re15_bss_parse_chunk(bss_chunk, size, &chunk)) return -1;
    if (!re15_bss_chunk_has_video(&chunk))              return -2;

    /* 2. VLC decode — output sized to the worst-case formula
     * documented in re15_bss_vlc_decode. malloc because 80 KB +
     * 230 KB is too big for the C stack on Windows MSVC defaults. */
    size_t vlc_cap = ((size_t)chunk.run_length_words + 2) * 4;
    int16_t *coeffs = (int16_t *) malloc(vlc_cap * sizeof(int16_t));
    if (!coeffs) return -3;

    int vlc_written = re15_bss_vlc_decode(chunk.vlc_payload,
                                          chunk.vlc_payload_size,
                                          chunk.run_length_words,
                                          chunk.quant, chunk.version,
                                          coeffs, vlc_cap);
    if (vlc_written < 0) {
        free(coeffs);
        return -4;
    }

    /* 3. Software MDEC: IDCT + YUV→RGB. Output is plain 24bpp R8G8B8. */
    uint8_t *rgb_bytes = (uint8_t *) malloc((size_t)BG_PIXELS * 3);
    if (!rgb_bytes) {
        free(coeffs);
        return -5;
    }

    int mdec_rv = re15_bss_mdec_decode(coeffs, (size_t)vlc_written,
                                        BG_WIDTH, BG_HEIGHT, rgb_bytes);
    free(coeffs);
    if (mdec_rv != 0) {
        free(rgb_bytes);
        return -6;
    }

    /* 4. Pack into RGBA8888 to match the PC framebuffer format
     * (R << 24) | (G << 16) | (B << 8) | 0xFF — see asset_pc.c
     * rgb555_to_rgba8888 for the canonical layout. */
    for (int i = 0; i < BG_PIXELS; i++) {
        uint8_t r = rgb_bytes[i * 3 + 0];
        uint8_t g = rgb_bytes[i * 3 + 1];
        uint8_t b = rgb_bytes[i * 3 + 2];
        s_bg_cache[i] = ((uint32_t)r << 24) | ((uint32_t)g << 16)
                      | ((uint32_t)b <<  8) | 0xFFu;
    }
    free(rgb_bytes);

    s_bg_loaded = 1;

    /* AZ-round: hand the freshly decoded BG to the renderer so its
     * sprite.pri overdraw layer can sample from the matching image. */
    re15_render_pc_set_bg_image(s_bg_cache);
    return 0;
}

int re15_bg_load_test_asset(void)
{
    /* Try several plausible paths so the binary works from CTest, the
     * IDE, and `cmake --build && cd build/Release && .\re15_reborn_pc.exe`.
     * The bundled BSS lives at psx_dev/re15_reborn/assets/test.bss. */
    static const char *candidates[] = {
        "../../re15_reborn/assets/test.bss",            /* from build/Release */
        "../re15_reborn/assets/test.bss",               /* from build */
        "../../../re15_reborn/assets/test.bss",         /* from build/Release/subdir */
        "psx_dev/re15_reborn/assets/test.bss",          /* from repo root */
        NULL
    };

    for (int i = 0; candidates[i]; i++) {
        int sz = 0;
        uint8_t *buf = re15_asset_read_file(candidates[i], &sz);
        if (buf) {
            int rv = re15_bg_load_from_bss(buf, (size_t)sz);
            free(buf);
            if (rv == 0) return 0;
        }
    }
    return -100;  /* couldn't find asset */
}

/* Phase 4.5.10 / 4.5.12: load per-cut BG for the active room.
 * room_prefix is the basename ("room1100" or "room1170") used in the
 * file pattern <prefix>_bgNN.bss. Old API kept as a thin wrapper. */
int re15_bg_load_room_cut(const char *room_prefix, int cut_idx)
{
    if (!room_prefix) return -11;
    /* Cut cap raised 13→64 (globalization 2026-06-13): room1170 has 13 cuts but other
     * rooms have many more (ROOM5011=46, ROOM4010=44). The real bound is "does the
     * per-cut file exist" — a missing cut just fails the open below and the caller
     * falls back. 64 covers every RE1.5 room with margin. */
    if (cut_idx < 0 || cut_idx >= 64) return -10;
    char rel[96];
    /* Globalization Phase 3-B (2026-06-13): read per-cut BG from the shared tree's
     * disc layout (assets_shared/BSS/<ROOM>/BG%02d.BSS) instead of the old flat
     * re15_reborn/assets/<room>_bg##.bss. room_prefix ("room%04x") names the subdir;
     * Windows file lookup is case-insensitive so it matches the tree's ROOM%04X. */
    for (int i = 0; s_pc_bg_roots[i]; i++) {
        snprintf(rel, sizeof rel, "%sBSS/%s/BG%02d.BSS", s_pc_bg_roots[i], room_prefix, cut_idx);
        int sz = 0;
        uint8_t *buf = re15_asset_read_file(rel, &sz);
        if (buf) {
            int rv = re15_bg_load_from_bss(buf, (size_t)sz);
            free(buf);
            if (rv == 0) return 0;
            return rv;
        }
    }
    for (int i = 0; s_pc_bg_roots[i]; i++) {
        snprintf(rel, sizeof rel, "%s%s_bg%02d.bss", s_pc_bg_roots[i], room_prefix, cut_idx);
        int sz = 0;
        uint8_t *buf = re15_asset_read_file(rel, &sz);
        if (buf) {
            int rv = re15_bg_load_from_bss(buf, (size_t)sz);
            free(buf);
            if (rv == 0) return 0;
            return rv;
        }
    }
    return -100;
}

/* Map the resident room id to its BG-asset basename, GENERICALLY from the room
 * id (byte-true: FUN_800396fc picks BG assets via a stage/room table, no per-room
 * hardcoding; the PSX backend bg_psx.c already builds "\BSS\ROOM%04X\" straight
 * from g_current_room_id). Was an explicit 3-room whitelist with a
 * `default: return "room1170"` fallback that silently mis-loaded room1170's BGs
 * for any other room — removed. room1170 (0x1170) → "room1170" exactly as before
 * (its old explicit case echoed room%04x); unbundled rooms now fail cleanly at the
 * file open (callers guard the missing asset) instead of loading the wrong BG. */
static const char *re15_bg_room_prefix(unsigned room_id)
{
    static char buf[16];
    snprintf(buf, sizeof buf, "room%04x", room_id);
    return buf;
}

/* ROOM-AWARE per-cut BG load — selects the basename from g_current_room_id, so after a
 * cross-room transition the destination room's BG renders (parity with the PSX loader). */
int re15_bg_load_cut(int cut_idx)
{
    return re15_bg_load_room_cut(re15_bg_room_prefix(g_current_room_id), cut_idx);
}

void re15_bg_blit(int dst_x, int dst_y)
{
    if (!s_bg_loaded) return;

    /* Paint cached BG into the PC software framebuffer. The framebuffer
     * is SCREEN_XRES × SCREEN_YRES uint32 RGBA, owned by render_pc.c.
     * For the typical (dst_x, dst_y) = (0, 0) full-screen case this is
     * one memcpy. Generalised to handle partial blits in case render
     * pipeline ever needs sub-region updates. */
    uint32_t *fb = re15_pc_framebuffer();
    if (!fb) return;

    int x0 = dst_x < 0 ? 0 : dst_x;
    int y0 = dst_y < 0 ? 0 : dst_y;
    int x1 = dst_x + BG_WIDTH;  if (x1 > SCREEN_XRES) x1 = SCREEN_XRES;
    int y1 = dst_y + BG_HEIGHT; if (y1 > SCREEN_YRES) y1 = SCREEN_YRES;
    if (x1 <= x0 || y1 <= y0) return;

    int copy_w = x1 - x0;
    for (int y = y0; y < y1; y++) {
        const uint32_t *src = &s_bg_cache[(y - dst_y) * BG_WIDTH + (x0 - dst_x)];
        uint32_t       *dst = &fb[y * SCREEN_XRES + x0];
        memcpy(dst, src, (size_t)copy_w * sizeof(uint32_t));
    }
}

int re15_bg_is_loaded(void)
{
    return s_bg_loaded;
}

/* AZ-round 2026-05-28: BG foreground occlusion patch — re-blit a rect of
 * the cached BG image (from sprite.pri mask) onto the PC framebuffer
 * AFTER characters have rendered. This overdraws character pixels where
 * the BG has foreground geometry (railings, pillars, boxes etc.) the
 * character should be hidden behind.
 *
 * PSX uses OT-based depth sort: BG foreground sprites are inserted into
 * OT at their pri.depth value; characters at their avgZ; the GPU draws
 * back-to-front so foreground sprites overdraw deeper characters. Our
 * software renderer simplifies this: always overdraw masked regions
 * after character render (loses fidelity if character is in front of
 * mask, but for cinematic shots characters are nearly always BEHIND
 * the masked foreground geometry).
 *
 * Per re15_pri.h Mask: srcX/Y is sample position in 256-wide BG TIM,
 * dstX/Y is screen pixel position, width/height is rect dims.
 */
void re15_bg_blit_pri_patch(int src_x, int src_y,
                            int dst_x, int dst_y,
                            int w, int h)
{
    if (!s_bg_loaded) return;
    uint32_t *fb = re15_pc_framebuffer();
    if (!fb) return;

    /* Clamp src against BG image bounds, dst against framebuffer. */
    int sx0 = src_x, sy0 = src_y;
    int dx0 = dst_x, dy0 = dst_y;
    if (dx0 < 0) { sx0 -= dx0; w += dx0; dx0 = 0; }
    if (dy0 < 0) { sy0 -= dy0; h += dy0; dy0 = 0; }
    if (sx0 < 0 || sy0 < 0) return;
    if (sx0 + w > BG_WIDTH)  w = BG_WIDTH  - sx0;
    if (sy0 + h > BG_HEIGHT) h = BG_HEIGHT - sy0;
    if (dx0 + w > SCREEN_XRES) w = SCREEN_XRES - dx0;
    if (dy0 + h > SCREEN_YRES) h = SCREEN_YRES - dy0;
    if (w <= 0 || h <= 0) return;

    for (int yy = 0; yy < h; yy++) {
        const uint32_t *src = &s_bg_cache[(sy0 + yy) * BG_WIDTH + sx0];
        uint32_t       *dst = &fb[(dy0 + yy) * SCREEN_XRES + dx0];
        memcpy(dst, src, (size_t)w * sizeof(uint32_t));
    }
}
