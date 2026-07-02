/*
 * RE1.5 Rebuilt — Inventory impl (Phase 4.4.7, 2026-05-19).
 *
 * Target-agnostic. Minimal grant/decrement logic for the demo.
 */
#include <string.h>
#include "re15_inventory.h"

#define HUD_PICKUP_DISPLAY_FRAMES 90   /* 1.5 s at 60 fps */

re15_inventory_t g_inv;

void re15_inv_init(void)
{
    memset(&g_inv, 0, sizeof(g_inv));
}

int re15_inv_grant(uint8_t type, uint8_t amount)
{
    if (type == 0 || amount == 0) return -1;

    /* Pass 1: stack onto an existing same-id slot (RE2 ammo stacks). No 255 clamp
     * — the byte-true slot write is a raw qty write; a single pickup-merge never exceeds a
     * uint8 in practice. (The grant-level merge wrapper was not disasm-pinned. NOTE: the earlier
     * "FUN_8006947c" citation was WRONG — that is sys.c GPU code, not an inventory writer; the real
     * writers are FUN_8004a1f0/FUN_8004dc4c/FUN_8004e214 on DAT_800b10ac. See RE15_INVENTORY_SUBSYSTEM.md §2.4.) */
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) {
        if (g_inv.slots[i].id == type) {
            g_inv.slots[i].qty               = (uint8_t)(g_inv.slots[i].qty + amount);
            g_inv.last_pickup_type           = type;
            g_inv.last_pickup_amount         = amount;
            g_inv.last_pickup_display_frames = HUD_PICKUP_DISPLAY_FRAMES;
            return 0;
        }
    }
    /* Pass 2: place in first free slot (raw slot write, DAT_800b10ac 4-byte stride). */
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) {
        if (g_inv.slots[i].id == 0) {
            g_inv.slots[i].id     = type;
            g_inv.slots[i].qty    = amount;
            g_inv.last_pickup_type           = type;
            g_inv.last_pickup_amount         = amount;
            g_inv.last_pickup_display_frames = HUD_PICKUP_DISPLAY_FRAMES;
            return 0;
        }
    }
    return -1;   /* inventory full */
}
