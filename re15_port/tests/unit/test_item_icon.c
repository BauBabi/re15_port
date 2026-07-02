/**
 * @file test_item_icon.c
 * @brief Phase 8.22 — byte-true inventory item ICON decode (item_icon_common.c).
 *
 * Loads the real DATA/ITEMALL.PIX + decodes the briefing items' icons. The opaque-pixel counts are
 * byte-true (the transparent mask = ITEMALL index 0/0xe3/0xe4/0xe6), and the tile↔id map + palettes
 * were framebuffer-derived (mzd_inv_open.sav, zero-mismatch), so a decoded opaque pixel is the exact
 * colour the game shows. See re15_item_icon.h + RE15_INVENTORY_SUBSYSTEM.md §3.
 */
#include "re15_item_icon.h"
#include <stdio.h>

static int count_opaque(unsigned char id)
{
    int n = 0;
    for (int v = 0; v < 30; v++)
        for (int u = 0; u < 40; u++) {
            unsigned char r, g, b;
            if (re15_item_icon_pixel(id, u, v, &r, &g, &b)) n++;
        }
    return n;
}

int main(void)
{
    int fail = 0;
    printf("=== item icon decode (Phase 8.22) ===\n");

    if (re15_itemall_load() != 0) {
        fprintf(stderr, "FAIL: cannot load ITEMALL.PIX\n"); return 1; }

    /* the briefing items have a byte-true tile + captured palette */
    if (!re15_item_icon_available(0x01) || !re15_item_icon_available(0x03) || !re15_item_icon_available(0x15)) {
        fprintf(stderr, "FAIL: knife/handgun/bullets must have a drawable icon\n"); fail = 1; }
    /* a key (id 0x22, clut_idx not captured) has no byte-true icon yet */
    if (re15_item_icon_available(0x22)) {
        fprintf(stderr, "FAIL: id 0x22 (key) has no captured palette -> not available\n"); fail = 1; }

    /* byte-true opaque-pixel counts (transparent = idx 0/0xe3/0xe4/0xe6; framebuffer-aligned) */
    int nk = count_opaque(0x01), nh = count_opaque(0x03), ns = count_opaque(0x15);
    if (nk != 269) { fprintf(stderr, "FAIL: COMBAT KNIFE opaque px 269, ist %d\n", nk); fail = 1; }
    if (nh != 538) { fprintf(stderr, "FAIL: BROWNING HP opaque px 538, ist %d\n", nh); fail = 1; }
    if (ns != 249) { fprintf(stderr, "FAIL: H.GUN BULLETS opaque px 249, ist %d\n", ns); fail = 1; }

    /* a transparent (background) corner pixel returns 0 */
    unsigned char r, g, b;
    if (re15_item_icon_pixel(0x01, 0, 0, &r, &g, &b)) {
        fprintf(stderr, "FAIL: knife (0,0) corner must be transparent\n"); fail = 1; }

    /* the knife blade is silver/gray: at least one opaque pixel is near-gray (|R-G|,|G-B| small, bright) */
    int gray = 0;
    for (int v = 0; v < 30 && !gray; v++)
        for (int u = 0; u < 40; u++) {
            if (re15_item_icon_pixel(0x01, u, v, &r, &g, &b)) {
                int mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
                int mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
                if (mx > 120 && (mx - mn) < 48) { gray = 1; break; }
            }
        }
    if (!gray) { fprintf(stderr, "FAIL: the knife must have a bright near-gray (blade) pixel\n"); fail = 1; }

    if (fail) { fprintf(stderr, "\nITEM-ICON TEST FAILED\n"); return 1; }
    printf("PASS: item icons (KNIFE 269px, BROWNING HP 538px, H.GUN BULLETS 249px opaque; "
           "byte-true tile+palette; blade is gray)\n");
    return 0;
}
