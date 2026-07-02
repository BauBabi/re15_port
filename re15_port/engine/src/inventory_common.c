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

/* Byte-true STAGE1 briefing loadout (mzd_stage1_briefing.sav, DAT_800b10ac 4-byte slots). */
void re15_inv_load_briefing(void)
{
    re15_inv_init();                                    /* clear first */
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;   /* COMBAT KNIFE (the equipped weapon, DAT_800aca5d==1) */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;  /* BROWNING HP (handgun, 15-round clip)                */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;  /* H. GUN BULLETS (ammo)                               */
}

/* Item CLASSIFICATION — BYTE-TRUE id-range gate (@0x80047d54 `sltiu id,0x15`; @0x80049124 `sltiu id,0x22`;
 * backed by the ARMS-head table @0x8007492c being non-zero exactly for ids 0x00..0x14). */
int re15_item_is_weapon(uint8_t id) { return id < 0x15; }              /* 0x00..0x14 (id 0 = base weapon slot) */
int re15_item_is_ammo(uint8_t id)   { return id >= 0x15 && id < 0x22; }/* 0x15..0x21                          */
int re15_item_is_key(uint8_t id)    { return id >= 0x22; }             /* max_stack 1, outside the icon bound  */

/* Byte-true item name catalog 0x00..0x21 (DAT_800c4a28 string blob, decoded). */
static const char *const s_item_names[] = {
    "",               "COMBAT KNIFE",   "PIPE",           "BROWNING HP",      /* 0x00..0x03 */
    "SIG P228",       "BERETTA M93R",   "GLOCK 18",       "SUPER REDHAWK",    /* 0x04..0x07 */
    "REMINGTON M870", "HAND GRENADE",   "ACID GRENADE",   "INCEND. GRENADE",  /* 0x08..0x0b */
    "INGRAM M10",     "SPAS-12",        "FLAMETHROWER",   "GRENADE LAUNCHER", /* 0x0c..0x0f */
    "GRENADE LAUNCHER","GRENADE LAUNCHER","ROCKET LAUNCHER","H&K MC51",       /* 0x10..0x13 */
    "COLT PYTHON",    "H. GUN BULLETS", "SHOTGUN SHELLS", "MAGNUM BULLETS",   /* 0x14..0x17 */
    "FLAME FUEL",     "EXPLOSIVE RND",  "ACID ROUNDS",    "INCEND. ROUNDS",   /* 0x18..0x1b */
    "REMOTE DETON.",  "EMPTY SHELLS",   "NITRO CAPSULE",  "ACID CAPSULE",     /* 0x1c..0x1f */
    "INCEND. CAPSULE","MEMORY CARD",                                          /* 0x20..0x21 */
};
const char *re15_item_name(uint8_t id)
{
    if (id >= (uint8_t)(sizeof s_item_names / sizeof s_item_names[0])) return "";
    return s_item_names[id];
}
