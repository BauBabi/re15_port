/*
 * RE1.5 Rebuilt — Inventory item ICONS (Phase 8.22, 2026-07-02).
 *
 * Byte-true decode of the RE1.5 inventory item icons from DATA/ITEMALL.PIX. The icon is a 40×30
 * 8-bit-indexed tile drawn native-size (SPRT, no scaling); an item id maps to an ITEMALL tile index
 * and a per-item CLUT (descriptor DAT_80074a8c[id].clut_idx). See RE15_INVENTORY_SUBSYSTEM.md §3.
 *
 * GROUND TRUTH: the palettes + the tile↔id mapping were reverse-engineered from the mzd_inv_open.sav
 * FRAMEBUFFER (the rendered inventory) by aligning each ITEMALL tile against its on-screen icon with a
 * ZERO-mismatch fit — so the decoded RGBA is byte-identical to what the game displays. Confirmed for the
 * briefing items (COMBAT KNIFE id 1 = tile 1, BROWNING HP id 3 = tile 3, H.GUN BULLETS id 0x15 = tile 37).
 * Transparent = ITEMALL index 0 or the background indices 0xe3/0xe4/0xe6.
 */
#ifndef RE15_ITEM_ICON_H
#define RE15_ITEM_ICON_H

#include <stdint.h>

/* Load DATA/ITEMALL.PIX once (86400 B = 72×40×30 8-bit tiles). Returns 0 on success, -1 on failure.
 * Idempotent. Call before re15_item_icon_pixel (or it lazy-loads on first use). */
int  re15_itemall_load(void);

/* Decode one icon pixel for item `id` at (u,v) in [0,40)×[0,30). On an OPAQUE pixel: writes the
 * byte-true 8-bit RGB (0..255 each) and returns 1. On a TRANSPARENT pixel (or if the id has no
 * byte-true palette captured yet): returns 0 and leaves *r/*g/*b untouched. */
int  re15_item_icon_pixel(uint8_t id, int u, int v, uint8_t *r, uint8_t *g, uint8_t *b);

/* Does item `id` have a byte-true icon (tile + captured palette) the port can draw? (The briefing
 * items do; other ids need their palette captured from a menu-open savestate — Phase 8.23.) */
int  re15_item_icon_available(uint8_t id);

#endif /* RE15_ITEM_ICON_H */
