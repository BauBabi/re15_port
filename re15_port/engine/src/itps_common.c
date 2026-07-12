/*
 * itps_common.c — ITPS item-picture decode (item-get modal picture, U11 gap 1).
 *
 * Byte-true per-item 112×72 picture the item-get modal (FUN_8001db28) zooms/flips in, from
 * ITEM/ITPS.ITP (file id 0x2a). RE'd wf_9e42f4cb: the modal's load callback LAB_8001e404 streams the
 * TIM at CD LBA 0x1740 + id×6 sectors = byte offset `id × 0x3000` in ITPS.ITP; FUN_8004ee78 OpenTIMs
 * it (8-bit + 256-color CLUT, CLUT→VRAM y489, image 112×72). See re15_itps.h. NOT the ITEMALL 40×30
 * icon (item_icon_common.c) — a different file / bit-depth / code path.
 */
#include "re15_itps.h"
#include <stddef.h>

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

#define ITPS_STRIDE   0x3000            /* one item TIM every 6 sectors (id×0x3000) */
#define ITPS_TILES    72               /* 884736 / 0x3000 = 72 TIMs (ids 0x00..0x47) */
#define ITPS_CLUT_OFF 0x14             /* CLUT data: after magic(4)+flag(4)+clut_len(4)+rect(8) */
#define ITPS_IMG_OFF  0x220            /* image data: after CLUT(0x200)+img_block_header(0xc) */
#define ITPS_SIZE     ((size_t)ITPS_STRIDE * ITPS_TILES)   /* 884736 */

static const uint8_t *s_itps  = NULL;
static int            s_loaded = 0;
static int            s_tried  = 0;

void re15_itps_set_data(const uint8_t *data, int size)
{
    if (data && (size_t)size >= ITPS_SIZE) { s_itps = data; s_loaded = 1; s_tried = 1; }
}

int re15_itps_load(void)
{
    if (s_loaded) return 0;
    if (s_tried)  return -1;   /* fail-once — a missing asset must not fopen-storm the per-pixel draw */
    s_tried = 1;
    static const char *dirs[] = { "shared_assets/PSX/ITEM/", "ITEM/", "PSX/ITEM/",
                                  "../shared_assets/PSX/ITEM/", NULL };
    char path[256];
    uint8_t *data = NULL; int sz = 0;
    for (int i = 0; dirs[i] && !data; i++) {
        int n = 0; const char *d = dirs[i];
        while (d[n] && n < 200) { path[n] = d[n]; n++; }
        const char *f = "ITPS.ITP"; int k = 0;
        while (f[k] && n < 250) { path[n++] = f[k++]; }
        path[n] = 0;
        data = re15_asset_read_file(path, &sz);
    }
    if (!data || (size_t)sz < ITPS_SIZE) return -1;
    s_itps  = data;
    s_loaded = 1;
    return 0;
}

int re15_itps_available(uint8_t id)
{
    if (id >= ITPS_TILES) return 0;
    if (!s_loaded && re15_itps_load() != 0) return 0;
    return s_itps != NULL;
}

int re15_itps_pixel(uint8_t id, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (u < 0 || u >= RE15_ITPS_W || v < 0 || v >= RE15_ITPS_H) return 0;
    if (id >= ITPS_TILES) return 0;
    if (!s_loaded && re15_itps_load() != 0) return 0;
    size_t base = (size_t)id * ITPS_STRIDE;
    uint8_t  idx = s_itps[base + ITPS_IMG_OFF + (size_t)v * RE15_ITPS_W + u];
    const uint8_t *clut = s_itps + base + ITPS_CLUT_OFF;
    uint16_t c = (uint16_t)(clut[idx * 2] | (clut[idx * 2 + 1] << 8));   /* BGR555 (little-endian) */
    if (c == 0) return 0;                                               /* CLUT entry 0 = transparent */
    if (r) *r = (uint8_t)((c & 0x1f) << 3);
    if (g) *g = (uint8_t)(((c >> 5) & 0x1f) << 3);
    if (b) *b = (uint8_t)(((c >> 10) & 0x1f) << 3);
    return 1;
}
