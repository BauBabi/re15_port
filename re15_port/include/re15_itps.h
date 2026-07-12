/*
 * RE1.5 Rebuilt — ITPS item-picture decode (item-get modal, U11 gap 1).
 *
 * The item-get modal (FUN_8001db28 case4) draws a DISTINCT per-item 112×72 picture — NOT the 40×30
 * ITEMALL inventory icon. RE'd byte-true (workflow wf_9e42f4cb): the modal's load callback
 * (LAB_8001e404 @0x8001e404) reads the picked item id and streams file 0x2a (= ITEM/ITPS.ITP, CD LBA
 * 0x1740 + id×6 sectors) into a scratch buffer, then FUN_8004ee78 OpenTIMs it. Each item's TIM sits at
 * byte offset `id × 0x3000` inside ITPS.ITP (72 TIMs, verified on disk). TIM layout (flag 0x09 =
 * 8-bit + CLUT): magic 0x10 @+0x00, CLUT data @+0x14 (256 × BGR555, VRAM y489), image data @+0x220
 * (112×72, 1 byte/pixel). Pixel = CLUT16[img8[v*112+u]].
 */
#ifndef RE15_ITPS_H_INCLUDED
#define RE15_ITPS_H_INCLUDED

#include <stdint.h>

#define RE15_ITPS_W 112
#define RE15_ITPS_H 72

/* Hand the engine the already-loaded ITPS.ITP bytes (the PC backend resolves the asset root). Call
 * once at boot; supersedes the lazy load. `data` must remain valid for the program's lifetime. */
void re15_itps_set_data(const uint8_t *data, int size);

/* Load ITEM/ITPS.ITP once via re15_asset_read_file (lazy, fail-once — a missing asset must not fopen-
 * storm the per-pixel draw). Returns 0 on success, -1 on failure. */
int  re15_itps_load(void);

/* Does the per-item 112×72 picture for item `id` exist (id < 72 and the file loaded)? */
int  re15_itps_available(uint8_t id);

/* Decode one modal-picture pixel for item `id` at (u,v) in [0,112)×[0,72). On an opaque pixel writes
 * the byte-true RGB (from the item's 256-color CLUT) and returns 1; on a transparent pixel (CLUT entry
 * 0 = black, or out of range / not loaded) returns 0 and leaves *r/*g/*b untouched. */
int  re15_itps_pixel(uint8_t id, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* RE15_ITPS_H_INCLUDED */
