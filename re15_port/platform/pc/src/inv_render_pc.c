/*
 * inv_render_pc.c — PC rasterizer for the byte-true RE1.5 status/inventory screen
 * (Wave 1; display list built engine-side in re15_inv_screen.c — see the spec chain
 * shots/inv_plan.md + shots/inv_wave1_blockers.md).
 *
 * Texture model (blocker Q3, authoritative VRAM map):
 *   TEX4  = 4bpp page VRAM (704,256) = DATA/TEX.TIM pixel columns 0-63hw (u 0-255).
 *   ICON8 = 8bpp page VRAM (640,256) = COMPOSED at runtime:
 *             v 0-119  : icon-cache grid, cells 0-9 dynamic (ITEMALL.PIX tile[id],
 *                        identity map; empty = tile 0 blank), cells 10/11 = ST_00
 *                        static art (factory preload rows 0-119);
 *             v 120-149: ST_00.TIM rows 120-149 (static band — the only region where
 *                        ST_00 uv's are trusted);
 *             v 150-252: STPIC_00.TIM pixel rows 0-102 at u 0-115 (ID card block,
 *                        uploaded at its TIM-header coords (640,406); Leon = index 0,
 *                        selector @0x800C01C4: file id 0x2C + character @0x800ACA5C);
 *             u 116-127 of that band + v 253-255: leftover ST_00 bytes.
 *   CLUTs: UI 0-7 = TEX.TIM CLUT rows 8-15 (VRAM (256,488..495), ids 0x7a10..0x7bd0);
 *          ST_00 CLUT row 0 (VRAM (0,484)); STPIC's own CLUT (VRAM (0,485)) (Q4).
 *
 * Pixel pipeline — calibrated against the savestate framebuffers of
 * stage_saves/mzd_inv_open.sav (both flip buffers, see shots/inv_plan.md wave-1 log):
 *   - 15-bit (5:5:5) framebuffer, cleared to black (the status screen covers it fully);
 *   - opaque textured: out5 = min(31, (t5*mod + 52) >> 7)
 *     (the rounding constant is pinned to [52,55] by the LED pixels at (25,108) under
 *      glow 0x18/0x1c and the Standard-Arms 0x3e-dimmed knife edge pixels);
 *   - CLUT value 0x0000 = transparent; texel STP bit + ABE would blend (no STP=1
 *     texels exist in these assets — every SPRT texel lands opaque);
 *   - untextured ABE LineF2 (ECG trace): out5 = (((d5<<3)|(d5>>2)) + f8) >> 4
 *     (50/50 abr0 blend with bit-replicated destination expansion — solved from the
 *      trace trail pixels of both flip buffers);
 *   - ops are rasterized in REVERSE list order (earliest AddPrim = topmost).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_tim.h"

#define INV_XRES 320
#define INV_YRES 240

extern uint8_t *re15_asset_read_file(const char *path, int *size);
extern void re15_pc_put_pixel(int x, int y, uint32_t rgba);

/* ---- decoded assets (lazy, once) ---- */
static int      s_ready = 0;        /* 0 = not tried, 1 = ok, -1 = failed */
static uint8_t  s_tex4[256][256];   /* 4bpp texel indices */
static uint8_t  s_font4[128][256];  /* message-FONT page: TEX.TIM texel cols 256-511,
                                     * rows 0-127 (glyph cells at v=32.., 16x16 —
                                     * the grid item-NAME print, wave 2) */
static uint8_t  s_icon8[256][256];  /* 8bpp texel indices (u 0-127 used) */
static uint16_t s_clut[11][256];    /* [0..7]=UI 16-entry, [8]=ST_00 row0, [9]=STPIC,
                                     * [10]=TEX.TIM CLUT row 0 (256,480) id 0x7810 —
                                     * the grid item-NAME font (wave 2) */
static uint8_t *s_itemall = NULL;   /* ITEMALL.PIX raw (72 x 1200B 40x30 tiles) */
static int      s_itemall_size = 0;
static uint8_t *s_itps = NULL;      /* ITEM/ITPS.ITP raw — wide-weapon 80x30 icons at
                                     * block(id*0x3000)+0x21A0 (wave 2)               */
static int      s_itps_size = 0;

static uint8_t *load_cd(const char *rel, int *size)
{
    const char *cdroot = getenv("RE15_CD_ROOT");
    char path[300];
#ifdef RE15_CD_ROOT_DEFAULT
    if (!cdroot || !cdroot[0]) cdroot = RE15_CD_ROOT_DEFAULT;
#endif
    if (!cdroot || !cdroot[0]) return NULL;
    snprintf(path, sizeof path, "%s/%s", cdroot, rel);
    return re15_asset_read_file(path, size);
}

static int inv_assets_init(void)
{
    uint8_t *buf; int sz;
    re15_tim_t tim;
    if (s_ready) return s_ready;
    s_ready = -1;

    /* DATA/TEX.TIM: 4bpp, pixel block 320hw x 256 at (704,256); CLUT (256,480) 32x24.
     * The 4bpp page uses only the first 64hw (= 128 bytes) of each row. */
    buf = load_cd("DATA/TEX.TIM", &sz);
    if (!buf) { fprintf(stderr, "[inv] TEX.TIM missing\n"); return s_ready; }
    if (re15_tim_parse(buf, sz, &tim) != 0 || tim.bpp != 4 || !tim.has_clut ||
        tim.clut_entries < 24 * 32) { free(buf); return s_ready; }
    {
        const uint8_t *px = (const uint8_t *)tim.pixels;
        int pitch = tim.width / 2;   /* 4bpp: 2 texels per byte */
        int y, u, r;
        for (y = 0; y < 256 && y < tim.height; y++)
            for (u = 0; u < 256; u++) {
                uint8_t bb = px[y * pitch + (u >> 1)];
                s_tex4[y][u] = (u & 1) ? (bb >> 4) : (bb & 0xF);
            }
        /* message-FONT page = TEX.TIM texel columns 256-511 (font_atlas_psx.h region;
         * glyph cells at v=32.. — the name print's byte-true uv lands here) */
        memset(s_font4, 0, sizeof s_font4);
        for (y = 0; y < 128 && y < tim.height; y++)
            for (u = 0; u < 256 && 256 + u < tim.width; u++) {
                int vx = 256 + u;
                uint8_t bb = px[y * pitch + (vx >> 1)];
                s_font4[y][u] = (vx & 1) ? (bb >> 4) : (bb & 0xF);
            }
        /* UI cluts = CLUT rows 8..15, entries 0..15 of each 32-entry row (Q4 F) */
        for (r = 0; r < 8; r++)
            memcpy(s_clut[r], tim.clut + (8 + r) * 32, 16 * sizeof(uint16_t));
        /* TEX CLUT row 0 (VRAM (256,480) = clut id 0x7810) — the item-NAME glyph
         * palette (FUN_80028c1c @0x80028cb8-c0: ((flags&0x30)<<3)|0x7810, flags=1). */
        memcpy(s_clut[10], tim.clut, 16 * sizeof(uint16_t));
    }
    free(buf);

    /* DATA/ST_00.TIM: 8bpp 64hw x 278 -> (640,256); CLUT 256x2 -> (0,484/485).
     * Rows 0-255 are the factory preload of the icon page (cells 0-9 rewritten below). */
    buf = load_cd("DATA/ST_00.TIM", &sz);
    if (!buf) { fprintf(stderr, "[inv] ST_00.TIM missing\n"); return s_ready; }
    if (re15_tim_parse(buf, sz, &tim) != 0 || tim.bpp != 8 || !tim.has_clut) {
        free(buf); return s_ready;
    }
    {
        const uint8_t *px = (const uint8_t *)tim.pixels;
        int y;
        memset(s_icon8, 0, sizeof s_icon8);
        for (y = 0; y < 256 && y < tim.height; y++)
            memcpy(s_icon8[y], px + y * tim.width, (tim.width < 128) ? tim.width : 128);
        memcpy(s_clut[8], tim.clut, 256 * sizeof(uint16_t));   /* row 0 = (0,484) */
    }
    free(buf);

    /* ITEM/STPIC_00.TIM: 8bpp 58hw x 103 -> (640,406) = page v150+, CLUT -> (0,485).
     * Leon (character @0x800ACA5C = 0) -> STPIC_00 (selector @0x800C01C4, blocker Q2). */
    buf = load_cd("ITEM/STPIC_00.TIM", &sz);
    if (!buf) { fprintf(stderr, "[inv] STPIC_00.TIM missing\n"); return s_ready; }
    if (re15_tim_parse(buf, sz, &tim) != 0 || tim.bpp != 8 || !tim.has_clut) {
        free(buf); return s_ready;
    }
    {
        const uint8_t *px = (const uint8_t *)tim.pixels;
        int y;
        for (y = 0; y < tim.height && 150 + y < 256; y++)
            memcpy(s_icon8[150 + y], px + y * tim.width,
                   (tim.width < 116) ? tim.width : 116);
        memcpy(s_clut[9], tim.clut, 256 * sizeof(uint16_t));   /* STPIC CLUT (0,485) */
    }
    free(buf);

    /* DATA/ITEMALL.PIX: headerless 72 x 1200B (40x30 8bpp) icon tiles. */
    s_itemall = load_cd("DATA/ITEMALL.PIX", &s_itemall_size);
    if (!s_itemall) { fprintf(stderr, "[inv] ITEMALL.PIX missing\n"); return s_ready; }

    /* ITEM/ITPS.ITP: 72 x 0x3000 blocks; the wide-weapon (ids 0x0e-0x13) 80x30 8bpp
     * icon sits at block+0x21A0 (0x960 bytes, row-major) — the pickup grant passes
     * a2 = *(0x800ac77c)+0x21A0 into FUN_8004dc4c @0x8001e0b8-c8, uploaded once via
     * FUN_800492b8 mode 1 (w=0x28hw=80px, h=0x1e @0x80049318-6c). Optional: only
     * needed once a wide weapon is carried. */
    s_itps = load_cd("ITEM/ITPS.ITP", &s_itps_size);

    s_ready = 1;
    return s_ready;
}

/* Re-compose the dynamic icon-cache cells 0-9 (mirrors FUN_800492b8 uploads at grant/
 * compaction: cell k at u=(k%3)*40, v=(k/3)*30; source = ITEMALL tile[id], identity;
 * empty = tile 0 blank). Cells 10/11 keep the ST_00 factory art (never allocated —
 * capacity 10, Q3). */
static void inv_compose_cells(void)
{
    int k;
    for (k = 0; k < 10; k++) {
        int tile = re15_inv_screen_cache_tile(k);
        int u0 = (k % 3) * 40, v0 = (k / 3) * 30, r;
        if (tile == -2) {
            /* wide weapon (kind 1/2): the 80x30 icon from the item's ITPS block+0x21A0
             * (FUN_800492b8 mode 1, single upload at the HEAD cell covering cells k and
             * k+1 — LoadImage w=0x28hw @0x80049318-6c; the two ordinary 40x30 cell SPRTs
             * then show the halves seamlessly). kind==2 (tail) is covered by the head's
             * upload -> skip. */
            uint8_t kind = g_inv.slots[k].flags;
            uint8_t id = g_inv.slots[k].id;
            if (kind == 1 && s_itps &&
                ((int)id * 0x3000 + 0x21A0 + 80 * 30) <= s_itps_size) {
                const uint8_t *src = s_itps + (int)id * 0x3000 + 0x21A0;
                for (r = 0; r < 30; r++)
                    memcpy(&s_icon8[v0 + r][u0], src + r * 80, 80);
            }
            continue;
        }
        if (tile < 0 || (tile + 1) * 1200 > s_itemall_size) tile = 0;
        for (r = 0; r < 30; r++)
            memcpy(&s_icon8[v0 + r][u0], s_itemall + tile * 1200 + r * 40, 40);
    }
}

/* ---- rasterizer (pipeline calibrated against the savestate fb — header comment) ---- */
static uint16_t s_fb5[INV_YRES][INV_XRES];   /* 5:5:5 packed r|g<<5|b<<10 */

static void px_opaque(int x, int y, int r5, int g5, int b5)
{
    if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) return;
    s_fb5[y][x] = (uint16_t)(r5 | (g5 << 5) | (b5 << 10));
}
static int mod5(int t5, int m)
{
    int v = (t5 * m + 52) >> 7;      /* rounding constant pinned to [52,55] */
    return v > 31 ? 31 : v;
}
static int blend_ch(int d5, int f8)  /* abr0 with replicated dst expansion */
{
    int v = (((d5 << 3) | (d5 >> 2)) + f8) >> 4;
    return v > 31 ? 31 : v;
}

static void raster_op(const re15_inv_op_t *o)
{
    if (o->kind == RE15_INV_OP_LINE) {
        /* axis-aligned ABE LineF2: (x,y)->(w,h). Vertical (ECG trace / wipe mode 1)
         * or horizontal (wipe mode 2, wave 2). Endpoints inclusive (fb-verified). */
        if (o->y != o->h) {
            int y0 = o->y, y1 = o->h, x = o->x, y;
            if (y1 < y0) { int t = y0; y0 = y1; y1 = t; }
            for (y = y0; y <= y1; y++) {
                uint16_t d;
                if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                d = s_fb5[y][x];
                s_fb5[y][x] = (uint16_t)(blend_ch(d & 31, o->r)
                                         | (blend_ch((d >> 5) & 31, o->g) << 5)
                                         | (blend_ch((d >> 10) & 31, o->b) << 10));
            }
        } else {
            int x0 = o->x, x1 = o->w, y = o->y, x;
            if (x1 < x0) { int t = x0; x0 = x1; x1 = t; }
            for (x = x0; x <= x1; x++) {
                uint16_t d;
                if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                d = s_fb5[y][x];
                s_fb5[y][x] = (uint16_t)(blend_ch(d & 31, o->r)
                                         | (blend_ch((d >> 5) & 31, o->g) << 5)
                                         | (blend_ch((d >> 10) & 31, o->b) << 10));
            }
        }
    } else if (o->kind == RE15_INV_OP_SPRT) {
        const uint16_t *clut = s_clut[o->clut];
        int py, pxx;
        for (py = 0; py < o->h; py++) {
            int v = (o->v + py) & 255;
            for (pxx = 0; pxx < o->w; pxx++) {
                int u = (o->u + pxx) & 255;
                uint8_t t = (o->page == RE15_INV_PAGE_TEX4)  ? s_tex4[v][u]
                          : (o->page == RE15_INV_PAGE_FONT4) ? s_font4[v & 127][u]
                          :                                    s_icon8[v][u];
                uint16_t c = clut[t];
                int t5r, t5g, t5b;
                if (c == 0) continue;                 /* CLUT value 0 = transparent */
                t5r = c & 31; t5g = (c >> 5) & 31; t5b = (c >> 10) & 31;
                if (o->abe && (c & 0x8000)) {
                    /* semi-trans texel (STP set): abr0 blend — no such texels exist
                     * in these assets (STP census = 0), kept for completeness */
                    int x = o->x + pxx, y = o->y + py;
                    uint16_t d;
                    if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                    d = s_fb5[y][x];
                    s_fb5[y][x] = (uint16_t)(
                        blend_ch(d & 31, mod5(t5r, o->r) << 3)
                        | (blend_ch((d >> 5) & 31, mod5(t5g, o->g) << 3) << 5)
                        | (blend_ch((d >> 10) & 31, mod5(t5b, o->b) << 3) << 10));
                } else {
                    px_opaque(o->x + pxx, o->y + py,
                              mod5(t5r, o->r), mod5(t5g, o->g), mod5(t5b, o->b));
                }
            }
        }
    }
    /* RE15_INV_OP_TILE: reserved, not emitted in wave 1 */
}

/* Draw one frame of the status screen into the software framebuffer.
 * Returns 0 on success, -1 if assets could not be loaded. */
int re15_inv_render_pc_draw(const re15_inv_op_t *ops, int n)
{
    int i, x, y;
    if (inv_assets_init() != 1) return -1;
    {   /* debug: RE15_INV_DUMP=<path> writes the display list once (op-diff harness) */
        static int s_dumped = 0;
        const char *dp = s_dumped ? NULL : getenv("RE15_INV_DUMP");
        if (dp && *dp) {
            FILE *f = fopen(dp, "w");
            s_dumped = 1;
            if (f) {
                for (i = 0; i < n; i++)
                    fprintf(f, "%d k%d p%d c%d a%d xy(%d,%d) wh(%d,%d) uv(%d,%d) rgb(%d,%d,%d)\n",
                            i, ops[i].kind, ops[i].page, ops[i].clut, ops[i].abe,
                            ops[i].x, ops[i].y, ops[i].w, ops[i].h, ops[i].u, ops[i].v,
                            ops[i].r, ops[i].g, ops[i].b);
                fclose(f);
            }
            {   /* raw page/clut dump next to the op dump */
                char pp[320]; FILE *g;
                snprintf(pp, sizeof pp, "%s.pages", dp);
                g = fopen(pp, "wb");
                if (g) {
                    inv_compose_cells();
                    fwrite(s_tex4, 1, sizeof s_tex4, g);
                    fwrite(s_icon8, 1, sizeof s_icon8, g);
                    fwrite(s_clut, 1, sizeof s_clut, g);
                    fclose(g);
                }
            }
        }
    }
    inv_compose_cells();
    memset(s_fb5, 0, sizeof s_fb5);   /* cleared draw buffer (screen fully covered) */
    for (i = n - 1; i >= 0; i--) raster_op(&ops[i]);   /* back-to-front */
    for (y = 0; y < INV_YRES; y++)
        for (x = 0; x < INV_XRES; x++) {
            uint16_t c = s_fb5[y][x];
            uint32_t r = (uint32_t)(c & 31) << 3,
                     g = (uint32_t)((c >> 5) & 31) << 3,
                     b = (uint32_t)((c >> 10) & 31) << 3;
            re15_pc_put_pixel(x, y, (r << 24) | (g << 16) | (b << 8) | 0xFFu);
        }
    return 0;
}
