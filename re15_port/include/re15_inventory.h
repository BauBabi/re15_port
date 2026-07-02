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

/* Byte-true slot = 4-byte stride {+0 id (0=empty), +1 qty, +2 flags/mode, +3 pad}. The LIVE carried
 * inventory is DAT_800b10ac (53 xrefs; 4-byte stride proven @0x8004ded4 `sll v0,v0,0x2`), compacted/
 * written by FUN_8004dc4c (`sb → DAT_800b10ac`). Layout independently confirmed = RE2's ITEM_WORK
 * {u8 Id; Num; Size; dummy} × 11 (BioRdt-master/global.h:783-788,1716).
 * (Corrected by the inventory-weapon-select RE — RE15_INVENTORY_SUBSYSTEM.md §2.4: the earlier
 *  "DAT_800d4a3c / FUN_8006947c" annotations were WRONG — DAT_800d4a3c is uninitialised 0xFF, and
 *  FUN_8006947c is a sys.c GPU beq, not an inventory writer. The 11-slot count + layout stay correct.) */
typedef struct {
    uint8_t id;       /* item id; 0 = empty slot (+0 DAT_800b10ac) */
    uint8_t qty;      /* quantity                (+1)              */
    uint8_t flags;    /* per-item flags / display mode (+2)        */
    uint8_t pad;      /* +3 — keeps the 4-byte stride              */
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

/* Load the byte-true STAGE1 briefing loadout into g_inv (clears first). Savestate-confirmed
 * (mzd_stage1_briefing.sav / mzd_stage1_engage_live.sav, DAT_800b10ac): slot0 = item 0x01 qty 0
 * (the HANDGUN — the equipped weapon, DAT_800aca5d==1), slot1 = item 0x03 qty 15, slot2 = item
 * 0x15 qty 50 (the two non-weapon stacks). This is the game-start loadout; a full new-game-init
 * table + the per-item weapon/ammo/key CLASSIFICATION (the item-attribute table in the DEBUG.BIN
 * item module @0x800c0000) are Phase 2b. See RE15_INVENTORY_SUBSYSTEM.md §2.4. */
void re15_inv_load_briefing(void);

/* Is item id a WEAPON (equippable -> DAT_800aca5d)? BYTE-TRUE: id 0x01 = the handgun (weapon 1,
 * the briefing weapon, savestate-confirmed; item-id == weapon-id in the equip-commit @0x80046688).
 * The general item-attribute table (all weapons/ammo/keys) lives in the DEBUG.BIN item module
 * @0x800c0000 (resident, capstone-disassemblable; the char-default table @0x800c00d4 is decoded,
 * the type table is not yet located) -> Phase 2b. The briefing owns only the handgun as a weapon. */
int  re15_item_is_weapon(uint8_t id);

#endif /* RE15_INVENTORY_H */
