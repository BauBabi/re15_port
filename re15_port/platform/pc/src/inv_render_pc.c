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
 * Pixel pipeline — GPU-exact against the CURRENT ground-truth renderer (DuckStation
 * software rasterizer, gpu_sw_rasterizer.inl; wave-6 fix pass — the earlier "+52"
 * modulation rounding matched only the pre-drift wave-1 idle capture, see mod5()):
 *   - 15-bit (5:5:5) framebuffer, cleared to black (the status screen covers it fully);
 *   - opaque textured: out5 = (min(255, (t5*mod8) >> 4)) >> 3  (truncating — ShadePixel
 *     dither-LUT index (t5*m)>>4 with the dither-off LUT cell DM[2][3]=0; verified
 *     1170/1170 vs the fresh grid/cmd savestate fbs on the 0x3e-modulated box);
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
#include "re15_inv_ui.h"     /* MAP wave: CD file-id table u16 @0x80074c4c[page] (region-1
                              * blob; the loader 0x8004c328 @0x8004c344-50) */
#include "re15_inventory.h"
#include "re15_tim.h"
#include "asset_root_pc.h"   /* gemeinsame Asset-Wurzel-Aufloesung (exe-relativ) */

#define INV_XRES 320
#define INV_YRES 240

extern uint8_t *re15_asset_read_file(const char *path, int *size);
extern void re15_pc_put_pixel(int x, int y, uint32_t rgba);
#include "re15_re2doc.h"   /* die Bild-Ebene des FILE-Schirms */

/* ---- decoded assets (lazy, once) ---- */
static int      s_ready = 0;        /* 0 = not tried, 1 = ok, -1 = failed */
static uint8_t  s_tex4[256][256];   /* 4bpp texel indices */
static uint8_t  s_font4[128][256];  /* message-FONT page: TEX.TIM texel cols 256-511,
                                     * rows 0-127 (glyph cells at v=32.., 16x16 —
                                     * the grid item-NAME print, wave 2) */
static uint8_t  s_exam4[256][256];  /* wave 4: 4bpp texpage 0x1d = VRAM (832,256) =
                                     * TEX.TIM texel cols 512-767 (halfword cols 128+)
                                     * — the CHECK panel chrome (elements @0x800c6b64
                                     * sample v=0x48..0x87, savestate-VRAM verified
                                     * resident at (832,328+) while the menu is open) */
static uint8_t  s_icon8[256][256];  /* 8bpp texel indices (u 0-127 used) */
static uint8_t  s_map4[256][256];   /* MAP wave: 4bpp texpage 0x17 = VRAM (448,256) —
                                     * the MAP0x.PIX floor plan (raw 32768 B = 64hw x
                                     * 256 rows, upload rect @0x8004c1a0-b0; loaded
                                     * synchronously off g_inv_screen.map_page)       */
static int      s_map_loaded = -1;  /* CD file id resident in s_map4 (-1 = none)      */
static uint16_t s_clut[17][256];    /* [0..7]=UI 16-entry, [8]=ST_00 row0, [9]=STPIC,
                                     * [10]=TEX.TIM CLUT row 0 (256,480) id 0x7810 —
                                     * the grid item-NAME font (wave 2);
                                     * [11]=TEX.TIM CLUT row 16 (256,496) id 0x7c10 —
                                     * the CHECK chrome (wave 4);
                                     * [12]=the ITPS photo CLUT (0,489) — DYNAMIC,
                                     * zero until the first CHECK/pickup upload
                                     * (savestate: row (0,489) all-0 on the idle
                                     * screen; CLUT 0 = transparent -> the photo
                                     * window is invisible before a valid upload);
                                     * [13]=TEX.TIM CLUT row 21 (256,501) id 0x7d50 —
                                     * the MAP page palette (GetClut(0x100,0x1f5)
                                     * @0x80046fdc-fe8, MAP wave);
                                     * [14..16]=TEX.TIM CLUT rows 2/4/6 (256,482/484/
                                     * 486) ids 0x7890/0x7910/0x7990 — the FUN_80028ec4
                                     * text palettes (clut = 0x7810|(a3&0x30)<<3
                                     * @0x80028f5c-64: title 0x10 / footer 0x20 /
                                     * dimmed underscores 0x30; FILE wave) */
/* wave 4: the photo VRAM window = halfwords (832..887, 256..327) as 8bpp 112x72 —
 * texpage 0x9d of the photo prim @0x800c6944-84. Initial content = the TEX.TIM data
 * that the menu-open upload put there (byte cols 256..367 of each row); the CHECK
 * loader 0x800c6878 -> 0x800c0258 overwrites it with the ITPS block's pixel data IF
 * the block's embedded prect is (832,256) — the id-0x24 block ships prect (0,0) /
 * crect (0,480), so window AND clut keep their stale content (byte-true quirk). */
static uint8_t  s_photo_px[72][112];
static uint8_t  s_photo_seq = 0;    /* last consumed g_inv_screen.exam_upload_seq */
static uint8_t *s_itemall = NULL;   /* ITEMALL.PIX raw (72 x 1200B 40x30 tiles) */
static int      s_itemall_size = 0;
static uint8_t *s_mixitem = NULL;   /* wave 5: MIXITEM.PIX raw — 16800 B = exactly
                                     * 14 x 1200B (40x30 8bpp) tiles = pic ids 1-14
                                     * (CD file id 0x19: size table @0x8006f43c +
                                     * 0x19*8 = 0x41a0; loaded by the EXCHANGE
                                     * executor jal 0x80013b60(0x19, 0x801a0000, 0)
                                     * @0x8004e174; result tile at (pic-1)*1200
                                     * @0x8004e224-38). Same ST_00-row-0 CLUT as the
                                     * other cells (the cell SPRT's clut).           */
static int      s_mixitem_size = 0;
static uint8_t *s_itps = NULL;      /* ITEM/ITPS.ITP raw — wide-weapon 80x30 icons at
                                     * block(id*0x3000)+0x21A0 (wave 2)               */
static int      s_itps_size = 0;

/* 2026-08-24: dieser Lader kannte GENAU EINE Wurzel (env RE15_CD_ROOT bzw. den einkompilierten
 * Default) und gab danach NULL zurueck — kein einziger Fallback. Im ausgelieferten Paket zeigt
 * der Default ins Leere, damit scheiterte schon DATA/TEX.TIM, und inv_assets_init() bricht beim
 * ersten Fehlschlag ab: der KOMPLETTE Status-/Inventar-Bildschirm (ST_00, STPIC_00, ITEMALL.PIX,
 * MIXITEM.PIX, ITPS.ITP, MAP%02X.PIX) fiel mit aus. Jetzt ueber die gemeinsame CD-Wurzelliste. */
static uint8_t *load_cd(const char *rel, int *size)
{
    return re15_pc_read_cd(rel, size);
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
        /* wave 4: examine-chrome page = TEX.TIM texel columns 512-767 (= the VRAM
         * texpage 0x1d region (832,256), upload target (704,256) + 128hw). */
        memset(s_exam4, 0, sizeof s_exam4);
        for (y = 0; y < 256 && y < tim.height; y++)
            for (u = 0; u < 256 && 512 + u < tim.width; u++) {
                int vx = 512 + u;
                uint8_t bb = px[y * pitch + (vx >> 1)];
                s_exam4[y][u] = (vx & 1) ? (bb >> 4) : (bb & 0xF);
            }
        /* wave 4: photo window initial content = the same VRAM halfwords (832..887)
         * read as 8bpp = TEX.TIM byte cols 256..367, rows 0..71. */
        memset(s_photo_px, 0, sizeof s_photo_px);
        for (y = 0; y < 72 && y < tim.height; y++)
            for (u = 0; u < 112 && 256 + u < pitch; u++)
                s_photo_px[y][u] = px[y * pitch + 256 + u];
        /* UI cluts = CLUT rows 8..15, entries 0..15 of each 32-entry row (Q4 F) */
        for (r = 0; r < 8; r++)
            memcpy(s_clut[r], tim.clut + (8 + r) * 32, 16 * sizeof(uint16_t));
        /* TEX CLUT row 0 (VRAM (256,480) = clut id 0x7810) — the item-NAME glyph
         * palette (FUN_80028c1c @0x80028cb8-c0: ((flags&0x30)<<3)|0x7810, flags=1). */
        memcpy(s_clut[10], tim.clut, 16 * sizeof(uint16_t));
        /* wave 4: TEX CLUT row 16 (VRAM (256,496) = clut id 0x7c10) — the CHECK
         * chrome palette (element table @0x800c6b64; TEX.TIM CLUT block (256,480)
         * 32x24 covers rows 480..503). */
        if (tim.clut_entries >= 17 * 32)
            memcpy(s_clut[11], tim.clut + 16 * 32, 16 * sizeof(uint16_t));
        /* MAP wave: TEX CLUT row 21 (VRAM (256,501) = clut id 0x7d50 =
         * GetClut(0x100,0x1f5) @0x80046fdc-fe8) — the palette of the map fixed
         * sprites and room-rect SPRTs (same 32x24 CLUT block, row 501-480=21). */
        if (tim.clut_entries >= 22 * 32)
            memcpy(s_clut[13], tim.clut + 21 * 32, 16 * sizeof(uint16_t));
        /* FILE wave: TEX CLUT rows 2/4/6 (ids 0x7890/0x7910/0x7990) — the FUN_80028ec4
         * a3-flag text palettes (0x10 title / 0x20 footer / 0x30 underscores). */
        memcpy(s_clut[14], tim.clut + 2 * 32, 16 * sizeof(uint16_t));
        memcpy(s_clut[15], tim.clut + 4 * 32, 16 * sizeof(uint16_t));
        memcpy(s_clut[16], tim.clut + 6 * 32, 16 * sizeof(uint16_t));
        /* [12] photo CLUT starts ZERO (savestate (0,489) idle census). */
        memset(s_clut[12], 0, 256 * sizeof(uint16_t));
        s_photo_seq = 0;
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

    /* DATA/MIXITEM.PIX (wave 5): headerless 14 x 1200B (40x30 8bpp) combine-result
     * tiles, pic ids 1-14 (see the s_mixitem note above). */
    s_mixitem = load_cd("DATA/MIXITEM.PIX", &s_mixitem_size);
    if (!s_mixitem) fprintf(stderr, "[inv] MIXITEM.PIX missing\n");

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
        {   /* wave 5: MIXITEM override — the EXCHANGE executor uploaded the combine
             * result tile (pic-1)*1200 into this cell (FUN_800492b8 @0x8004e240 et al.;
             * engine-side event state re15_inv_screen_cache_mix_pic). Decoded with the
             * same ST_00 row-0 CLUT the cell SPRT selects. */
            int pic = re15_inv_screen_cache_mix_pic(k);
            if (pic > 0 && s_mixitem && pic * 1200 <= s_mixitem_size) {
                const uint8_t *src = s_mixitem + (pic - 1) * 1200;
                for (r = 0; r < 30; r++)
                    memcpy(&s_icon8[v0 + r][u0], src + r * 40, 40);
                continue;
            }
        }
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

/* ---- wave 4: CHECK photo upload (0x800c6878 -> 0x800c0258) ------------------------
 * Consumes g_inv_screen.exam_upload_seq: parses the ITPS block (id*0x3000) as a TIM
 * exactly like 0x800c0258 (OpenTIM/ReadTIM + LoadImage of the EMBEDDED crect + prect
 * @0x800c0270-2ac) and applies the writes to the two VRAM windows this rasterizer
 * models: the photo pixels (832,256) 56hw x 72 and the photo CLUT row (0,489).
 * Blocks whose embedded rects point elsewhere (id 0x24 ships crect (0,480) / prect
 * (0,0)!) leave the windows STALE — byte-true for the modeled windows. (The original
 * would additionally scribble the (0,0) block onto the framebuffer area; that
 * out-of-window write is NOT modeled — documented open item.) */
static void photo_upload_check(void)
{
    const uint8_t *b;
    uint32_t flags, clen, plen;
    int cx, cy, pxr, pyr, pw, ph, y;
    if (s_photo_seq == g_inv_screen.exam_upload_seq) return;
    s_photo_seq = g_inv_screen.exam_upload_seq;
    if (!s_itps || (int)g_inv_screen.exam_item * 0x3000 + 0x3000 > s_itps_size) return;
    b = s_itps + (int)g_inv_screen.exam_item * 0x3000;
    if ((b[0] | (b[1] << 8)) != 0x10) return;                 /* TIM magic */
    flags = (uint32_t)b[4] | ((uint32_t)b[5] << 8);
    if (!(flags & 8)) return;                                 /* all blocks ship a CLUT */
    clen = (uint32_t)b[8] | ((uint32_t)b[9] << 8) | ((uint32_t)b[10] << 16);
    cx = b[12] | (b[13] << 8); cy = b[14] | (b[15] << 8);
    if (cx == 0 && cy == 489) {                               /* crect (0,489) 256x1 */
        /* CLUT DATA starts at +20 (TIM clut block: len u32 @+8 [self-inclusive],
         * rect 4xu16 @+12..19, entries @+20). The wave-4 code read from +16 = the
         * rect's w/h halfwords -> every entry shifted by 2 (wave-6 finding 2, the
         * noisy photo). Ground truth: knife block (ITPS id 1) crect (0,489) 256x1,
         * entry0 = 0x8000 (black, STP=1, 256/256 STP set, 0 zero entries); the
         * check savestate VRAM row (0,489) == block bytes from +20 (256/256) and
         * the fb photo == clut-direct opaque draw (8064/8064 px, prim 0x64808080
         * @0x800c6958, uv(0,0) clut 0x7a40 @0x800c6970-78, rgb 0x80 = identity). */
        int i;
        for (i = 0; i < 256; i++)
            s_clut[12][i] = (uint16_t)(b[20 + i * 2] | (b[21 + i * 2] << 8));
    }
    b += 8 + clen;
    plen = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
    (void)plen;
    pxr = b[4] | (b[5] << 8); pyr = b[6] | (b[7] << 8);
    pw = b[8] | (b[9] << 8); ph = b[10] | (b[11] << 8);
    if (pxr == 832 && pyr == 256 && pw == 56 && ph == 72)     /* prect (832,256) 56hw */
        for (y = 0; y < 72; y++)
            memcpy(s_photo_px[y], b + 12 + y * 112, 112);
}

/* ---- MAP wave: keep the MAP page in sync with g_inv_screen.map_page ----------------
 * Original flow: the entry-spawned task 0x8004c328 CD-loads file id u16 @0x80074c4c
 * [DAT_800b260e] to 0x801a8000 (jal 0x80013b60 @0x8004c354), and FUN_8004c058 uploads
 * it into the VRAM rect (448,256,64,256) after the slide (@0x8004c198-b0). Ids 12..24
 * name MAP01.PIX..MAP0D.PIX (13 files, DATA/); page 0xd's id read overruns the table
 * -> u16[13] = 0 (bytes @0x80074c66) = CD file 0 (identity unresolved — OPEN; the
 * port renders a black page for non-MAP ids). A MAP0x.PIX is headerless raw VRAM data:
 * 32768 B = 64 halfwords x 256 rows = 256x256 4bpp texels. */
static void map_page_check(void)
{
    const unsigned char *p;
    int id, y, u;
    uint8_t *buf; int sz;
    char rel[32];
    if (!(g_inv_screen.substate == 1 && g_inv_screen.item_state == 1)) return;
    p = RE15_INV_PTR(0x80074c4cu + (uint32_t)g_inv_screen.map_page * 2u);
    id = p[0] | (p[1] << 8);
    if (id == s_map_loaded) return;
    s_map_loaded = id;
    memset(s_map4, 0, sizeof s_map4);
    if (id < 12 || id > 24) {
        fprintf(stderr, "[inv] map page %d -> CD file id %d (not a MAP PIX; black page)\n",
                g_inv_screen.map_page, id);
        return;
    }
    snprintf(rel, sizeof rel, "DATA/MAP%02X.PIX", id - 11);   /* id 12 -> MAP01 .. 24 -> MAP0D */
    buf = load_cd(rel, &sz);
    if (!buf) { fprintf(stderr, "[inv] %s missing\n", rel); return; }
    for (y = 0; y < 256 && y * 128 + 127 < sz; y++)
        for (u = 0; u < 256; u++) {
            uint8_t bb = buf[y * 128 + (u >> 1)];
            s_map4[y][u] = (u & 1) ? (bb >> 4) : (bb & 0xF);
        }
    free(buf);
}

/* ---- rasterizer (pipeline calibrated against the savestate fb — header comment) ---- */
static uint16_t s_fb5[INV_YRES][INV_XRES];   /* 5:5:5 packed r|g<<5|b<<10 */

static void px_opaque(int x, int y, int r5, int g5, int b5)
{
    if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) return;
    s_fb5[y][x] = (uint16_t)(r5 | (g5 << 5) | (b5 << 10));
}

/* ---- wave-6 fix (finding 3): GPU-exact shaded (Gouraud) triangle + 4x4 dither -------
 * The CHECK panel background is NOT a smooth bilinear gradient: the PSX draws the two
 * POLY_G4 boxes (code 0x38 @0x800c6bd0 byte3; DR_MODE word 0xE1000260 @0x800c6bc4-cc
 * = texpage attr with DITHER bit9 SET) through the GPU's per-triangle DDA + 4x4
 * ordered dither -- the "dotted" pattern of zoom_checkbg_psx.png IS the dither.
 * Model = DuckStation's software rasterizer (the ground-truth captures' GPU),
 * gpu_sw_rasterizer.inl: ATTRIB_SHIFT=12/ATTRIB_POST_SHIFT=12 color steppers with
 * +half init, s64 edge steppers makefp/makestep, quad split (v0,v1,v2)+(v2,v1,v3)
 * (gpu_sw.cpp DrawPolygon), dither = clamp8(c8 + DM[y&3][x&3]) >> 3.
 * VERIFIED byte-exact vs stage_saves/inv_states/psx_inv_check.sav framebuffer:
 * 12686/12686 clean gbox px (both boxes, both parities' fb) before porting. */
static const int8_t k_dither[4][4] = {
    { -4,  0, -3,  1 }, {  2, -2,  3, -1 }, { -3,  1, -4,  0 }, {  3, -1,  2, -2 }
};

typedef struct { int32_t x, y, r, g, b; } gvert_t;

static void px_dither8(int x, int y, int r8, int g8, int b8)
{
    int d = k_dither[y & 3][x & 3];
    int r = r8 + d, g = g8 + d, b = b8 + d;
    if (r < 0) r = 0; else if (r > 255) r = 255;
    if (g < 0) g = 0; else if (g > 255) g = 255;
    if (b < 0) b = 0; else if (b > 255) b = 255;
    px_opaque(x, y, r >> 3, g >> 3, b >> 3);
}

static int64_t g_cdiv(int64_t a, int64_t b)   /* C-style trunc-toward-zero division */
{
    int64_t q = (a < 0 ? -a : a) / (b < 0 ? -b : b);
    return ((a < 0) == (b < 0)) ? q : -q;
}
static int64_t g_makefp(int32_t x) { return ((int64_t)x << 32) + ((1LL << 32) - (1 << 11)); }
static int64_t g_makestep(int32_t dx, int32_t dy)
{
    int64_t num = ((int64_t)dx << 32) + ((dx < 0) ? -(dy - 1) : ((dx > 0) ? (dy - 1) : 0));
    return g_cdiv(num, dy);
}

/* One shaded triangle, scalar port of gpu_sw_rasterizer.inl DrawTriangle/DrawSpan
 * (shading on, texture off, transparency off). */
static void gtri_shaded(gvert_t v0, gvert_t v1, gvert_t v2)
{
    gvert_t *v[3];
    unsigned tl;
    int32_t det;
    uint32_t drdx, dgdx, dbdx, drdy, dgdy, dbdy, baser, baseg, baseb;
    int64_t base_coord, base_step, bcu, bcl;
    int rfi, ofi, right_facing;
    unsigned vo, vp;
    struct { int64_t sx[2], stx[2]; int32_t sy, ey; int ud; } tp[2];
    int i;
    gvert_t t;

    v[0] = &v0; v[1] = &v1; v[2] = &v2;
    if (v[1]->x <= v[0]->x) tl = (v[2]->x <= v[1]->x) ? 4 : 2;
    else if (v[2]->x < v[0]->x) tl = 4;
    else tl = 1;
    if (v[2]->y < v[1]->y) { t = *v[2]; *v[2] = *v[1]; *v[1] = t;
        tl = ((tl >> 1) & 0x2) | ((tl << 1) & 0x4) | (tl & 0x1); }
    if (v[1]->y < v[0]->y) { t = *v[1]; *v[1] = *v[0]; *v[0] = t;
        tl = ((tl >> 1) & 0x1) | ((tl << 1) & 0x2) | (tl & 0x4); }
    if (v[2]->y < v[1]->y) { t = *v[2]; *v[2] = *v[1]; *v[1] = t;
        tl = ((tl >> 1) & 0x2) | ((tl << 1) & 0x4) | (tl & 0x1); }
    tl >>= 1;
    if (v[0]->y == v[2]->y) return;

    base_coord = g_makefp(v[0]->x);
    base_step  = g_makestep(v[2]->x - v[0]->x, v[2]->y - v[0]->y);
    bcu = (v[1]->y == v[0]->y) ? 0 : g_makestep(v[1]->x - v[0]->x, v[1]->y - v[0]->y);
    bcl = (v[2]->y == v[1]->y) ? 0 : g_makestep(v[2]->x - v[1]->x, v[2]->y - v[1]->y);
    vo = (tl != 0) ? 1 : 0;
    vp = (tl == 2) ? 3 : 0;
    right_facing = (v[1]->y == v[0]->y) ? (v[1]->x > v[0]->x) : (bcu > base_step);
    rfi = right_facing ? 1 : 0;
    ofi = right_facing ? 0 : 1;

    tp[vo].sy = v[0 ^ vo]->y;      tp[vo].ey = v[1 ^ vo]->y;
    tp[vo ^ 1].sy = v[1 ^ vp]->y;  tp[vo ^ 1].ey = v[2 ^ vp]->y;
    tp[vo].sx[rfi] = g_makefp(v[0 ^ vo]->x);
    tp[vo].stx[rfi] = bcu;
    tp[vo].sx[ofi] = base_coord + ((int64_t)(v[vo]->y - v[0]->y) * base_step);
    tp[vo].stx[ofi] = base_step;
    tp[vo].ud = (vo != 0);
    tp[vo ^ 1].sx[rfi] = g_makefp(v[1 ^ vp]->x);
    tp[vo ^ 1].stx[rfi] = bcl;
    tp[vo ^ 1].sx[ofi] = base_coord + ((int64_t)(v[1 ^ vp]->y - v[0]->y) * base_step);
    tp[vo ^ 1].stx[ofi] = base_step;
    tp[vo ^ 1].ud = (vp != 0);

#define GDET(A, B) (((int64_t)(v[1]->A - v[0]->A) * (v[2]->B - v[1]->B)) \
                  - ((int64_t)(v[2]->A - v[1]->A) * (v[1]->B - v[0]->B)))
    det = (int32_t)GDET(x, y);
    if (det == 0) return;
    drdx = (uint32_t)g_cdiv(GDET(r, y) * 4096, det) << 12;
    dgdx = (uint32_t)g_cdiv(GDET(g, y) * 4096, det) << 12;
    dbdx = (uint32_t)g_cdiv(GDET(b, y) * 4096, det) << 12;
    drdy = (uint32_t)g_cdiv(GDET(x, r) * 4096, det) << 12;
    dgdy = (uint32_t)g_cdiv(GDET(x, g) * 4096, det) << 12;
    dbdy = (uint32_t)g_cdiv(GDET(x, b) * 4096, det) << 12;
#undef GDET
    baser = ((((uint32_t)v[tl]->r << 12) + (1u << 11)) << 12)
            - drdx * (uint32_t)v[tl]->x - drdy * (uint32_t)v[tl]->y;
    baseg = ((((uint32_t)v[tl]->g << 12) + (1u << 11)) << 12)
            - dgdx * (uint32_t)v[tl]->x - dgdy * (uint32_t)v[tl]->y;
    baseb = ((((uint32_t)v[tl]->b << 12) + (1u << 11)) << 12)
            - dbdx * (uint32_t)v[tl]->x - dbdy * (uint32_t)v[tl]->y;

    for (i = 0; i < 2; i++) {
        int64_t lx = tp[i].sx[0], rx = tp[i].sx[1];
        int32_t cy = tp[i].sy;
        int32_t ey = tp[i].ey;
        int32_t xs, xb, x;
#define GSPAN() do { \
            xs = (int32_t)((uint64_t)lx >> 32); \
            xb = (int32_t)((uint64_t)rx >> 32); \
            for (x = xs; x < xb; x++) \
                px_dither8(x, cy, \
                    (int)((uint32_t)(baser + drdx * (uint32_t)x + drdy * (uint32_t)cy) >> 24), \
                    (int)((uint32_t)(baseg + dgdx * (uint32_t)x + dgdy * (uint32_t)cy) >> 24), \
                    (int)((uint32_t)(baseb + dbdx * (uint32_t)x + dbdy * (uint32_t)cy) >> 24)); \
        } while (0)
        if (tp[i].ud) {
            while (cy > ey) {
                cy--; lx -= tp[i].stx[0]; rx -= tp[i].stx[1];
                GSPAN();
            }
        } else {
            while (cy < ey) {
                GSPAN();
                cy++; lx += tp[i].stx[0]; rx += tp[i].stx[1];
            }
        }
#undef GSPAN
    }
}
static int mod5(int t5, int m)
{
    /* WAVE-6 FINDING-1 RESOLUTION: the "+52 rounding constant" (wave 1) was calibrated
     * against the OLD mzd_inv_open.sav framebuffer — which a since-changed DuckStation
     * build had rendered with rounded modulation. The CURRENT ground-truth renderer
     * (gpu_sw_rasterizer.inl ShadePixel: `(u16(texel5) * u16(color8)) >> 4`, dither-LUT
     * index with dither off = matrix[2][3] = 0, then >>3) TRUNCATES. Proven from the
     * savestates: the fresh grid/cmd fbs match trunc on 1170/1170 Standard-Arms texels
     * while the old idle fb matches +52 — with byte-identical prim (rgb 0x3e), CLUT
     * (0,484) and art in ALL saves, and DAT_800b25cd's complete writer set only ever
     * producing {0x3e, 0x80, ±6 steps} (@0x800495e8-618, @0x8004a624-34, @0x8004a8b0,
     * @0x8004ab14/30, @0x8004ad58-68, @0x8004ac90-a4, @0x8004b1fc, DEBUG.BIN
     * @0x800c6510-20). There is NO in-game dim mechanism — the wave-6 "finding 1"
     * 649-px class was capture-session drift. Only m=0x3e texels are affected
     * (m=0x80/0x40 are rounding-invariant), so MAP/FILE/CHECK stay pixel-exact. */
    int v = (t5 * m) >> 4;           /* 8-bit modulated color, truncating */
    if (v > 255) v = 255;
    return v >> 3;
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
    } else if (o->kind == RE15_INV_OP_PANEL) {
        /* Das originale RE2-Box-Panel (214x156 RGB555) 1:1 blitten. */
        extern const uint16_t *re15_inv_boxpanel(int which, int *w, int *h);
        int pw = 0, ph = 0;
        const uint16_t *img = re15_inv_boxpanel(o->v, &pw, &ph);
        if (img) {
            for (int py = 0; py < ph; py++)
                for (int pxx = 0; pxx < pw; pxx++) {
                    int x = o->x + pxx, y = o->y + py;
                    if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                    s_fb5[y][x] = img[py * pw + pxx];
                }
        }
    } else if (o->kind == RE15_INV_OP_FILL) {
        /* Deckendes Rechteck in exakter Farbe (kein ABE): fuer die aus dem
         * laufenden RE2 gemessenen Panel-Flaechen. RGB888 -> RGB555. */
        int py, pxx;
        int r5 = o->r >> 3, g5 = o->g >> 3, b5 = o->b >> 3;
        uint16_t src = (uint16_t)(r5 | (g5 << 5) | (b5 << 10));
        for (py = 0; py < o->h; py++)
            for (pxx = 0; pxx < o->w; pxx++) {
                int x = o->x + pxx, y = o->y + py;
                if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                s_fb5[y][x] = src;
            }
    } else if (o->kind == RE15_INV_OP_GBOX) {
        /* wave 4 + wave-6 fix (finding 3): POLY_G4 gouraud quad (DEBUG.BIN builder
         * 0x800c6b84: tag len 9 @0x800c6bbc-c0, embedded DR_MODE word 0xE1000260
         * @0x800c6bc4-cc = DITHER bit9 SET, code 0x38 opaque @0x800c6bd0 byte3).
         * Byte-true corner colors:
         *   c0 TL (0x00,0x10,0x7b) @0x800c6bd0 (lui 0x387b + addiu 0x1000)
         *   c1 TR (0x08,0x00,0x4a) @0x800c6bdc (lui 0x4a + addiu 8)
         *   c2 BL (0x08,0x00,0x52) @0x800c6be8 (lui 0x52 + addiu 8)
         *   c3 BR (0x00,0x10,0x5a) @0x800c6bf4 (lui 0x5a + addiu 0x1000)
         * Vertices @0x800c6c00-24: (x,y),(x+w,y),(x,y+h),(x+w,y+h). Rendered via the
         * GPU-exact triangle DDA + dither (gtri_shaded above; quad split (v0,v1,v2)
         * + (v2,v1,v3) per gpu_sw.cpp; 12686/12686 px savestate-verified). */
        gvert_t v0, v1, v2, v3;
        v0.x = o->x;        v0.y = o->y;        v0.r = 0x00; v0.g = 0x10; v0.b = 0x7b;
        v1.x = o->x + o->w; v1.y = o->y;        v1.r = 0x08; v1.g = 0x00; v1.b = 0x4a;
        v2.x = o->x;        v2.y = o->y + o->h; v2.r = 0x08; v2.g = 0x00; v2.b = 0x52;
        v3.x = o->x + o->w; v3.y = o->y + o->h; v3.r = 0x00; v3.g = 0x10; v3.b = 0x5a;
        gtri_shaded(v0, v1, v2);
        gtri_shaded(v2, v1, v3);
    } else if (o->kind == RE15_INV_OP_TILE) {
        /* FILE wave: untextured TILE 0x62 under DR_MODE 0xe1000440 = ABR 2 SUBTRACTIVE
         * (the selection highlight, DEBUG.BIN 0x800c742c: words 0x04000000/0xe1000440/
         * 0x62202020) — per channel out5 = max(0, dst5 - src5), src = rgb>>3. */
        int py, pxx;
        for (py = 0; py < o->h; py++)
            for (pxx = 0; pxx < o->w; pxx++) {
                int x = o->x + pxx, y = o->y + py;
                uint16_t d;
                int r5, g5, b5;
                if ((unsigned)x >= INV_XRES || (unsigned)y >= INV_YRES) continue;
                d = s_fb5[y][x];
                r5 = (d & 31) - (o->r >> 3);
                g5 = ((d >> 5) & 31) - (o->g >> 3);
                b5 = ((d >> 10) & 31) - (o->b >> 3);
                if (r5 < 0) r5 = 0;
                if (g5 < 0) g5 = 0;
                if (b5 < 0) b5 = 0;
                s_fb5[y][x] = (uint16_t)(r5 | (g5 << 5) | (b5 << 10));
            }
    } else if (o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_ITEMTILE) {
        /* ITEM BOX cell (RE15_INV_PAGE_ITEMTILE, [DESIGN] — see the enum note):
         * o->v = the tile id; w<=40 samples ITEMALL.PIX tile[v] (the byte-true
         * 40x30 art of the identity icon map), w==80 samples the wide-weapon
         * 80x30 icon at ITPS block v*0x3000+0x21A0 (FUN_800492b8 mode-1 source).
         * Same ST_00-row-0 CLUT + opaque modulated pipeline as the icon cells. */
        const uint16_t *clut = s_clut[o->clut];
        int tile = o->v, py, pxx;
        for (py = 0; py < o->h; py++)
            for (pxx = 0; pxx < o->w; pxx++) {
                uint8_t t;
                uint16_t c;
                if (o->w == 80) {
                    if (!s_itps || tile * 0x3000 + 0x21A0 + 80 * 30 > s_itps_size)
                        continue;
                    t = s_itps[tile * 0x3000 + 0x21A0 + py * 80 + pxx];
                } else {
                    if (!s_itemall || (tile + 1) * 1200 > s_itemall_size)
                        continue;
                    {   /* SKALIEREN statt beschneiden: die Quellkachel ist 40x30,
                         * das Ziel kann kleiner sein (RE2s Zeilensymbol ist 25x19).
                         * Vorher wurde nur die linke obere Ecke gezeigt — daher
                         * "die Item Bilder ragen heraus/sind abgeschnitten"
                         * (Nutzer 2026-08-31). */
                        int sxp = (o->w >= 40) ? pxx : (pxx * 40) / o->w;
                        int syp = (o->h >= 30) ? py  : (py  * 30) / o->h;
                        if (sxp > 39) sxp = 39;
                        if (syp > 29) syp = 29;
                        t = s_itemall[tile * 1200 + syp * 40 + sxp];
                    }
                }
                c = clut[t];
                if (c == 0) continue;                 /* CLUT 0 = transparent */
                px_opaque(o->x + pxx, o->y + py,
                          mod5(c & 31, o->r), mod5((c >> 5) & 31, o->g),
                          mod5((c >> 10) & 31, o->b));
            }
    } else if (o->kind == RE15_INV_OP_SPRT) {
        const uint16_t *clut = s_clut[o->clut];
        int py, pxx;
        for (py = 0; py < o->h; py++) {
            int v = (o->v + py) & 255;
            for (pxx = 0; pxx < o->w; pxx++) {
                int u = (o->u + pxx) & 255;
                uint8_t t = (o->page == RE15_INV_PAGE_TEX4)   ? s_tex4[v][u]
                          : (o->page == RE15_INV_PAGE_FONT4)  ? s_font4[v & 127][u]
                          : (o->page == RE15_INV_PAGE_EXAM4)  ? s_exam4[v][u]
                          : (o->page == RE15_INV_PAGE_MAP4)   ? s_map4[v][u]
                          : (o->page == RE15_INV_PAGE_PHOTO8) ?
                                ((v < 72 && u < 112) ? s_photo_px[v][u] : 0)
                          :                                     s_icon8[v][u];
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
}

/* debug: RE15_INV_FB_SHOT=<path.bmp> + RE15_INV_FB_SHOT_AT=<inv-frame> — dump the pure
 * SOFTWARE framebuffer s_fb5 as a 24-bit BMP, bypassing the SDL present/readback path
 * entirely (a detached display session garbles SDL_RenderReadPixels — memory note
 * reai-v2-frontend-cleanup). Channel expansion identical to the present loop (c5<<3). */
static void fb_dump_bmp(const char *path)
{
    FILE *f = fopen(path, "wb");
    int x, y;
    unsigned char hdr[54];
    unsigned sz = 54u + INV_XRES * INV_YRES * 3u;
    if (!f) return;
    memset(hdr, 0, sizeof hdr);
    hdr[0] = 'B'; hdr[1] = 'M';
    hdr[2] = (unsigned char)sz; hdr[3] = (unsigned char)(sz >> 8);
    hdr[4] = (unsigned char)(sz >> 16); hdr[5] = (unsigned char)(sz >> 24);
    hdr[10] = 54; hdr[14] = 40;
    hdr[18] = INV_XRES & 0xff; hdr[19] = INV_XRES >> 8;
    hdr[22] = INV_YRES & 0xff; hdr[23] = INV_YRES >> 8;
    hdr[26] = 1; hdr[28] = 24;
    fwrite(hdr, 1, 54, f);
    for (y = INV_YRES - 1; y >= 0; y--)          /* BMP bottom-up; row 320*3 = pad-free */
        for (x = 0; x < INV_XRES; x++) {
            uint16_t c = s_fb5[y][x];
            unsigned char px[3];
            px[0] = (unsigned char)(((c >> 10) & 31) << 3);   /* B */
            px[1] = (unsigned char)(((c >> 5) & 31) << 3);    /* G */
            px[2] = (unsigned char)((c & 31) << 3);           /* R */
            fwrite(px, 1, 3, f);
        }
    fclose(f);
}

/* ---- BILD-EBENE DES FILE-SCHIRMS (RE2-Dokumentseiten) ---------------------------------
 * RE1.5s FILE-Leser ist reiner Text (Blob @0x800ccd34 in DEBUG.BIN); eine Bild-Ebene gibt
 * es dort nicht. RE2 zeichnet je Seite ZWEI Sprites, und genau die zeichnet diese Funktion.
 * BYTE-BELEGT aus dem RE2-Leser FUN_80075fd0 (selbst disassembliert):
 *     8007603c  lhu   s3,-24250(at)   ; y_off aus dem Dokument-Record @0x800AA144
 *     80076040  addiu s1,zero,256
 *     80076044  subu  s5,s1,s3        ; H = 256 - y_off
 *   TEXTSEITE (4bpp, CLUT 0,490 @0x8007604c):
 *     80076068  sb zero,-2(s0)  u = 0
 *     8007606c  sb zero,-1(s0)  v = 0
 *     80076070  sh s1,2(s0)     w = 256
 *     80076078  sh s5,4(s0)     h = H
 *   ILLUSTRATION (8bpp, CLUT 0,489 @0x800760ac):
 *     800760b8  subu v0,zero,s3 ; v = (-y_off) & 0xFF = H
 *     800760d4  sh s1,2(s0)     w = 128
 *     800760dc  sh s3,4(s0)     h = y_off = 256 - H
 * Also: Text oben (volle Breite 256, Zeilen 0..H-1), darunter die 128 breite
 * Illustration (Zeilen H..255), zusammen eine 256x256-Flaeche.
 *
 * ⛔ PORT-ERWEITERUNG, klar benannt: RE1.5 kennt diese Ebene nicht. Sie zeichnet NUR,
 * wenn ein Dokument gewaehlt ist (re15_re2doc_select); ohne Auswahl bleibt der FILE-Schirm
 * unveraendert der byte-true Textleser. Die Bildlage im Schirm ist eine Port-Entscheidung -
 * RE2s eigene Sprite-Position gehoert zu RE2s Schirm-Layout, nicht zu RE1.5s.
 *
 * Die Masse kommen aus den TIM-Koepfen, nicht aus einer Tabelle - so kann ein selbst
 * gebautes Dokument eine eigene Seitenhoehe haben, ohne dass hier etwas nachgezogen wird. */
void re15_inv_render_pc_file_image(int doc, int page, int ox, int oy)
{
    if (doc < 0) return;
    int tw = 0, th = 0;
    if (!re15_re2doc_size(doc, page, RE15_RE2DOC_PAGE, &tw, &th)) return;
    const int H = th;

    uint8_t r, g, b;
    /* Sprite 1: Textseite, Zeilen 0..H-1 */
    for (int v = 0; v < H; v++)
        for (int u = 0; u < tw; u++)
            if (re15_re2doc_pixel(doc, page, RE15_RE2DOC_PAGE, u, v, &r, &g, &b))
                re15_pc_put_pixel(ox + u, oy + v,
                                  ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFFu);

    /* Sprite 2: Illustration, abgetastet ab Zeile H, 128 breit, Hoehe 256-H */
    int pw = 0, ph = 0;
    if (!re15_re2doc_size(doc, page, RE15_RE2DOC_PAPER, &pw, &ph)) return;
    const int hh = ph - H;                     /* = y_off, `sh s3,4(s0)` @0x800760dc */
    for (int v = 0; v < hh; v++)
        for (int u = 0; u < pw; u++)
            if (re15_re2doc_pixel(doc, page, RE15_RE2DOC_PAPER, u, H + v, &r, &g, &b))
                re15_pc_put_pixel(ox + u, oy + H + v,
                                  ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFFu);
}

/* Draw one frame of the status screen into the software framebuffer.
 * Returns 0 on success, -1 if assets could not be loaded. */
int re15_inv_render_pc_draw(const re15_inv_op_t *ops, int n)
{
    int i, x, y;
    static int s_inv_frame = -1;      /* n-th inv-rendered frame (debug hooks below) */
    if (inv_assets_init() != 1) return -1;
    s_inv_frame++;
    {   /* debug: RE15_INV_DUMP=<path> writes the display list once (op-diff harness);
         * RE15_INV_DUMP_AT=<n> delays the dump to the n-th inv-rendered frame (wave 4:
         * inspect the CHECK/examine list mid-flow). */
        static int s_dumped = 0;
        const char *dp = s_dumped ? NULL : getenv("RE15_INV_DUMP");
        if (dp && *dp) {
            const char *at = getenv("RE15_INV_DUMP_AT");
            if (at && s_inv_frame < atoi(at)) dp = NULL;
        }
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
    photo_upload_check();             /* wave 4: consume a pending CHECK photo upload */
    map_page_check();                 /* MAP wave: sync the MAP PIX page (0x8004c328) */
    memset(s_fb5, 0, sizeof s_fb5);   /* cleared draw buffer (screen fully covered) */
    for (i = n - 1; i >= 0; i--) raster_op(&ops[i]);   /* back-to-front */
    {   /* software-framebuffer BMP dump (display-session independent) */
        static int s_fb_shot_done = 0;
        const char *fp = s_fb_shot_done ? NULL : getenv("RE15_INV_FB_SHOT");
        if (fp && *fp) {
            const char *at = getenv("RE15_INV_FB_SHOT_AT");
            if (!at || s_inv_frame >= atoi(at)) {
                fb_dump_bmp(fp);
                fprintf(stderr, "[inv] fb shot (inv frame %d) -> %s\n", s_inv_frame, fp);
                s_fb_shot_done = 1;
            }
        }
    }
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

/* ---- Originales RE2-Box-Panel -------------------------------------------------
 * 214x156 RGB555, aus dem laufenden Spiel geschnitten (Speicherabzug -> Bildspeicher,
 * Panel-Region x 6..219 / y 13..202 — MIT dem unteren Textrahmen; Listenfeld, der
 * bewegliche Scrollgriff und das Textfeld sind darin geleert, weil unser eigener
 * Inhalt darueber kommt).
 * Datei: shared_assets/RE2/BOXPANEL.BIN — dieselbe Wurzel wie die uebrigen RE2-Assets. */
#define RE15_BOXPANEL_W 214
#define RE15_BOXLIST_H  156     /* Listen-Panel (Original y 13..168)   */
#define RE15_BOXTEXT_H   34     /* Textbox darunter (Original y 169..202) */
static uint16_t s_boxlist[RE15_BOXPANEL_W * RE15_BOXLIST_H];
static uint16_t s_boxtext[RE15_BOXPANEL_W * RE15_BOXTEXT_H];
static int      s_boxlist_state = 0, s_boxtext_state = 0;

static int load_panel_part(const char *rel, uint16_t *dst, size_t bytes)
{
    int sz = 0;
    uint8_t *buf = re15_pc_read_re2(rel, &sz);
    int ok = 0;
    if (buf) {
        if (sz >= (int)bytes) { memcpy(dst, buf, bytes); ok = 1; }
        free(buf);
    }
    return ok ? 1 : -1;
}

/* which: 0 = Listen-Panel, 1 = Textbox darunter. */
const uint16_t *re15_inv_boxpanel(int which, int *w, int *h)
{
    if (which == 1) {
        if (s_boxtext_state == 0)
            s_boxtext_state = load_panel_part("BOXTEXT.BIN", s_boxtext, sizeof s_boxtext);
        if (s_boxtext_state != 1) return NULL;
        if (w) *w = RE15_BOXPANEL_W;
        if (h) *h = RE15_BOXTEXT_H;
        return s_boxtext;
    }
    if (s_boxlist_state == 0)
        s_boxlist_state = load_panel_part("BOXPANEL.BIN", s_boxlist, sizeof s_boxlist);
    if (s_boxlist_state != 1) return NULL;
    if (w) *w = RE15_BOXPANEL_W;
    if (h) *h = RE15_BOXLIST_H;
    return s_boxlist;
}
