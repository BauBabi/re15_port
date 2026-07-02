/*
 * item_icon_common.c — Inventory item ICON decode (Phase 8.22, 2026-07-02).
 *
 * Byte-true ITEMALL.PIX icon decode (see re15_item_icon.h + RE15_INVENTORY_SUBSYSTEM.md §3). An item id
 * -> an ITEMALL 40×30 8-bit tile + a per-item CLUT; the pixel = CLUT[tile[v*40+u]] (native, no scale).
 * The tile↔id map + the CLUTs are framebuffer-derived (mzd_inv_open.sav), byte-identical to the game.
 */
#include "re15_item_icon.h"
#include <stddef.h>

/* Platform asset reader (re15_asset_read_file lives in the PC/PSX backend; test_support stubs it). */
extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

#define ITEMALL_TILE_BYTES  1200   /* 40 × 30 × 1 byte */
#define ITEMALL_TILES         72
#define ITEMALL_SIZE        (ITEMALL_TILE_BYTES * ITEMALL_TILES)   /* 86400 */

static const uint8_t *s_pix = NULL;   /* the raw ITEMALL.PIX */
static int            s_loaded = 0;
static int            s_tried  = 0;   /* fail-once: don't retry a failed lazy load every pixel */

/* Byte-true grid-icon palettes (framebuffer-derived, mzd_inv_open.sav, zero-mismatch align).
 * s_pal_clut2 = items with descriptor clut_idx 2 (knife, handgun, …); s_pal_clut6 = clut_idx 6 (shells, …).
 * Unknown entries = 0 (the tiles only reference the indices they use). */
#include "gen/itemall_palettes.inc"

/* Per-item descriptor clut_idx (DAT_80074a8c[id]+0), ids 0x00..0x18 (byte-true, extracted). */
static const uint8_t s_item_clut_idx[0x19] = {
    0,2,2,2, 2,6,7,7, 6,6,2,4, 0,2,6,6, 2,6,6,0, 2,6,2,6, 6
};

/* Item id -> ITEMALL tile index. Weapons (id<0x15) map 1:1 (tile==id, proven for 1/3). Ammo+ map via
 * the runtime upload order — captured per item from the framebuffer. Unknowns fall back to id. */
static uint8_t item_tile(uint8_t id)
{
    switch (id) {
        case 0x15: return 37;   /* H. GUN BULLETS (framebuffer-confirmed) */
        default:   return id;   /* weapons 0x00..0x14: tile == id (byte-true for the briefing) */
    }
}

/* The captured palette for an item, or NULL if not available yet. */
static const uint16_t *item_palette(uint8_t id)
{
    if (id >= 0x19) return NULL;
    switch (s_item_clut_idx[id]) {
        case 2: return s_pal_clut2;
        case 6: return s_pal_clut6;
        default: return NULL;   /* clut_idx 0/4/7 palettes not captured yet (Phase 8.23) */
    }
}

void re15_itemall_set_pix(const uint8_t *pix, int size)
{
    if (pix && size >= ITEMALL_SIZE) { s_pix = pix; s_loaded = 1; s_tried = 1; }
}

int re15_itemall_load(void)
{
    if (s_loaded) return 0;
    if (s_tried)  return -1;   /* fail-once — a missing asset must not fopen-storm the per-pixel draw */
    s_tried = 1;
    static const char *dirs[] = { "shared_assets/PSX/DATA/", "DATA/", "PSX/DATA/",
                                  "../shared_assets/PSX/DATA/", NULL };
    char path[256];
    uint8_t *pix = NULL; int sz = 0;
    for (int i = 0; dirs[i] && !pix; i++) {
        /* build the path without <stdio.h> snprintf dependency in the engine tier */
        int n = 0; const char *d = dirs[i];
        while (d[n] && n < 200) { path[n] = d[n]; n++; }
        const char *f = "ITEMALL.PIX"; int k = 0;
        while (f[k] && n < 250) { path[n++] = f[k++]; }
        path[n] = 0;
        pix = re15_asset_read_file(path, &sz);
    }
    if (!pix || sz < ITEMALL_SIZE) return -1;
    s_pix = pix;
    s_loaded = 1;
    return 0;
}

int re15_item_icon_available(uint8_t id)
{
    if (!s_loaded && re15_itemall_load() != 0) return 0;   /* no PIX -> text fallback (not a blank cell) */
    return item_palette(id) != NULL;
}

int re15_item_icon_pixel(uint8_t id, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (u < 0 || u >= 40 || v < 0 || v >= 30) return 0;
    if (!s_loaded && re15_itemall_load() != 0) return 0;
    const uint16_t *pal = item_palette(id);
    if (!pal) return 0;
    uint8_t tile = item_tile(id);
    if (tile >= ITEMALL_TILES) return 0;
    uint8_t idx = s_pix[(size_t)tile * ITEMALL_TILE_BYTES + (size_t)v * 40 + u];
    if (idx == 0 || idx == 0xe3 || idx == 0xe4 || idx == 0xe6) return 0;   /* transparent */
    uint16_t c = pal[idx];
    if (c == 0) return 0;                                                   /* uncaptured index */
    if (r) *r = (uint8_t)((c & 0x1f) << 3);                                 /* BGR555 -> RGB888 */
    if (g) *g = (uint8_t)(((c >> 5) & 0x1f) << 3);
    if (b) *b = (uint8_t)(((c >> 10) & 0x1f) << 3);
    return 1;
}
