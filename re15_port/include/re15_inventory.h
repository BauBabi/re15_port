/*
 * RE1.5 Rebuilt — Inventory (Phase 4.4.7, 2026-05-19).
 *
 * Minimal item-storage backend for the demo. RE2's real inventory is a
 * fixed 8-slot grid + 1 quest-item slot with per-item metadata (icon,
 * uses, ammo type). For demo we just track (type, amount) pairs and let
 * the HUD echo show "+0xNN" briefly on pickup.
 *
 * Phase 4.8+ will replace this with the proper RE2 inventory FSM
 * (equip, combine, examine, drop).
 */
#ifndef RE15_INVENTORY_H
#define RE15_INVENTORY_H

#include <stdint.h>

/* Byte-true: 11 slots (DAT_800d46ac latches the live count between 8 and 0xB).
 * (Was 16; room1170 uses no inventory so this is latent fidelity for later rooms.) */
#define RE15_INV_MAX_SLOTS 11

/* Byte-true slot = 4-byte stride at DAT_800d4a3c, written raw by FUN_8006947c:
 *   +0 id (0 = empty), +1 qty, +2 flags, +3 pad. No 255 clamp in the write path. */
typedef struct {
    uint8_t id;       /* item id; 0 = empty slot (+0 DAT_800d4a3c) */
    uint8_t qty;      /* quantity                (+1 DAT_800d4a3d) */
    uint8_t flags;    /* per-item flags          (+2 DAT_800d4a3e) */
    uint8_t pad;      /* +3 — keeps the 4-byte stride */
} re15_inv_slot_t;

typedef struct {
    re15_inv_slot_t  slots[RE15_INV_MAX_SLOTS];
    /* HUD pickup-echo: type + amount of the most-recent pickup, with a
     * frames-remaining countdown so the HUD shows it briefly. */
    uint8_t   last_pickup_type;
    uint8_t   last_pickup_amount;
    int       last_pickup_display_frames;
} re15_inventory_t;

extern re15_inventory_t g_inv;

void re15_inv_init(void);

/* Add `amount` of `type`. If a matching slot exists, increments it;
 * otherwise occupies the first empty slot. Returns 0 on success, -1 if
 * the inventory is full. Sets the HUD pickup echo on success. */
int  re15_inv_grant(uint8_t type, uint8_t amount);

#endif /* RE15_INVENTORY_H */
