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

    /* World-item pickup = the byte-true INSERT FUN_8004dc4c (@0x8004dc4c, disasm-verified
     * wf_6eea7fa1). It does NOT stack onto an existing same-id slot and does NOT read/clamp the
     * max_stack table (DAT_80074da8) — same-id MERGING and max_stack clamping exist ONLY in the menu
     * combine/reload paths (FUN_8004a0cc/FUN_8004e054/FUN_8004ebdc), never on world pickup. Inventory-
     * full is gated upstream (FUN_8001db28 case5: FUN_8004df2c()==0xff refuses the pickup). */
    int free_slot = -1;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { free_slot = i; break; }
    if (free_slot < 0) return -1;                          /* full -> the pickup is refused upstream */

    if ((uint8_t)(type - 0x0e) < 6) {
        /* WIDE weapon (0x0e..0x13 = flamethrower / 3× grenade-launcher / rocket / MC51): 2-cell
         * front-shift. Slots 0..7 move to 2..9 (old 8/9 dropped, slot 10 stays), then slots 0 and 1
         * take the weapon with the L/R display flags 1/2, and the equipped-slot index bumps by 2 so
         * it keeps tracking its weapon (@0x8004dc88, FUN_8004ea6c). */
        for (int i = 7; i >= 0; i--) g_inv.slots[i + 2] = g_inv.slots[i];
        g_inv.slots[0].id = type; g_inv.slots[0].qty = amount; g_inv.slots[0].flags = 1;
        g_inv.slots[1].id = type; g_inv.slots[1].qty = amount; g_inv.slots[1].flags = 2;
        re15_inv_set_equipped_slot(re15_inv_equipped_slot() + 2);
    } else {
        /* everything else = 1 cell in the first FREE slot (raw qty, flags 0). */
        g_inv.slots[free_slot].id    = type;
        g_inv.slots[free_slot].qty   = amount;
        g_inv.slots[free_slot].flags = 0;
    }
    g_inv.last_pickup_type           = type;
    g_inv.last_pickup_amount         = amount;
    g_inv.last_pickup_display_frames = HUD_PICKUP_DISPLAY_FRAMES;
    return 0;
}

/* Clear an inventory slot (byte-true item-USE consume @0x8004aef0-af28: DAT_800b10ac[slot] id/qty/flag
 * = 0). Used by the heal-USE FSM (item_use_common.c). The follow-on compaction FUN_8004dadc is
 * faithful-line-deferred — the menu display re-compacts occupied slots on the next rebuild. */
void re15_inv_remove_slot(int slot)
{
    if (slot >= 0 && slot < RE15_INV_MAX_SLOTS) {
        g_inv.slots[slot].id    = 0;
        g_inv.slots[slot].qty   = 0;
        g_inv.slots[slot].flags = 0;
    }
}

/* Byte-true STAGE1 briefing loadout (mzd_stage1_briefing.sav, DAT_800b10ac 4-byte slots). */
void re15_inv_load_briefing(void)
{
    re15_inv_init();                                    /* clear first */
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;   /* COMBAT KNIFE (the equipped weapon, DAT_800aca5d==1) */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;  /* BROWNING HP (handgun, 15-round clip)                */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;  /* H. GUN BULLETS (ammo)                               */
}

/* ====================================================================== *
 *  AMMO / MAGAZINE / RELOAD — byte-true FUN_8004ea6c / eae4 / dfec /      *
 *  eb70 / ebdc (PSX.EXE inventory subsystem; RE'd + arbitrated           *
 *  2026-07-05, workflow wf_7fc66a06-55e — every claim disasm-cited).     *
 * ====================================================================== */

/* The equipped inventory SLOT index (DAT_800b25c8; 0x80 = nothing equipped). The briefing
 * loadout equips the KNIFE in slot 0 (equip_test.sav: equipping the handgun sets slot=1). */
#define RE15_INV_SLOT_NONE 0x80
static uint8_t s_equipped_slot = 0;   /* byte-true start: knife = slot 0 */
int  re15_inv_equipped_slot(void)      { return s_equipped_slot; }
void re15_inv_set_equipped_slot(int s) { s_equipped_slot = (uint8_t)s; }

/* FUN_8004dfec @0x8004dfec — find-inventory-slot-by-item-id: linear scan comparing the id byte
 * @0x800b10ac+i*4, return slot or -1. (Its Ghidra decompile "return 0xffffffff" is WRONG —
 * the real bytes are the search loop @0x8004e00c-40.) The original bounds the scan with the
 * live count byte @0x800b0fbc (=10); the port scans its fixed 11 slots — result-identical,
 * since empty slots (id 0) never match a real item id. */
int re15_inv_find_item(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == id) return i;
    return -1;
}

/* Equipped-slot resolution shared by ea6c/eae4: flag byte ==2 marks the SECOND half of a
 * 2-slot item -> use slot-1 (@0x8004ea8c-a4 `ori v0,2; bne; addiu v0,-1`). */
static int inv_resolve_slot(void)
{
    int s = s_equipped_slot;
    if (s == RE15_INV_SLOT_NONE || s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    if (g_inv.slots[s].flags == 2 && s > 0) s--;
    return s;
}

/* FUN_8004ea6c @0x8004ea6c — "magazine > 0?": strict bool of the equipped weapon slot's
 * quantity byte (@0x8004eacc lbu + @0x8004ead4 `sltu v0,zero,v0`); slot 0x80 -> 0. */
int re15_ammo_mag_nonzero(void)
{
    int s = inv_resolve_slot();
    return (s >= 0) && (g_inv.slots[s].qty != 0);
}

/* FUN_8004eae4 @0x8004eae4 — consume 1 round: qty==0 -> return 0 with no write (underflow-
 * safe @0x8004eb44-4c); else qty-- (@0x8004eb60 sb) and return 1 (pre-decrement had-ammo
 * bool — the FSM-B auto-fire gates FUN_80011f50 on it @0x80034c00). The original has NO
 * 0x80 guard (unreachable: only discharge handlers call it, which require an equip); the
 * port's bounds check stands in for the same unreachable-state safety. */
int re15_ammo_consume(void)
{
    int s = inv_resolve_slot();
    if (s < 0 || g_inv.slots[s].qty == 0) return 0;
    g_inv.slots[s].qty--;
    return 1;
}

/* Weapon property table @0x80074da8 (stride 0xc, indexed by ITEM id): byte+0 = reload chunk
 * (= de-facto magazine size, since reload only fires at mag==0 and ebdc never clamps),
 * ptr+4 -> byte[0] = the weapon's AMMO item id. Byte dump (PSX.EXE): ids 3-6 = 15 rounds,
 * id 7 = 6, id 8 = 7, ids 9/10/11 = 250 (decrement-only stubs), id 12 = 100, id 13 = 12,
 * id 14 = 100, id 15 = 6; ammo ids: 3/4/5/6/12 -> 0x15, 8/13 -> 0x16, 7 -> 0x17, 14 -> 0x18,
 * 15 -> 0x19. Only ids < 9 can reload (the HOLD gate `sltiu aca5d,9` @0x80033368), so the
 * 9..15 rows are carried for the table's byte-truth, not reachability. */
typedef struct { uint8_t chunk; uint8_t ammo_id; } re15_wpn_prop_t;
static const re15_wpn_prop_t s_wpn_props[16] = {
    {0,0}, {0,0}, {0,0},                       /* 0-2 melee: no ammo               */
    {15,0x15}, {15,0x15}, {15,0x15}, {15,0x15},/* 3-6 handgun class -> H.GUN BULLETS */
    {6,0x17},  {7,0x16},                       /* 7 REDHAWK -> MAGNUM; 8 M870 -> SHELLS */
    {250,0},   {250,0},  {250,0},              /* 9-11 grenades (no reload: gate <9) */
    {100,0x15},{12,0x16},{100,0x18},{6,0},     /* 12 M10; 13 SPAS; 14 FLAME; 15 GL (ammo_id 0: the
                                                * grenade-launcher row's ammo ptr @0x80074E5C[1]=
                                                * 0x80074C88 dereferences to byte 0, like the 9-11
                                                * grenades; the port had 0x19 = FLAME ammo by mistake) */
};

/* FUN_8004eb70 @0x8004eb70 — reserve-ammo-present: property-table lookup of the equipped
 * weapon's ammo item id -> FUN_8004dfec(ammo_id) -> **slot > 0** (@0x8004ebc4-c8 `slt
 * zero,slot` — the byte-true QUIRK: ammo sitting in inventory slot 0 is NOT recognized).
 * Returns the ammo slot (>0) or 0. */
int re15_ammo_reserve_slot(void)
{
    int ws = inv_resolve_slot();
    if (ws < 0) return 0;
    uint8_t wid = g_inv.slots[ws].id;
    if (wid >= 16 || s_wpn_props[wid].ammo_id == 0) return 0;
    int as = re15_inv_find_item(s_wpn_props[wid].ammo_id);
    return (as > 0) ? as : 0;                  /* slot-0 quirk byte-true (slt zero,slot) */
}

/* FUN_8004ebdc @0x8004ebdc — reload execute: chunk = props[weapon_id].chunk; if chunk <
 * box_qty: mag += chunk, box -= chunk; else mag += box_qty and the emptied box slot's
 * id/qty/flags are zeroed (FUN_8004947c slot removal). NO capacity clamp (@decompile
 * FUN_8004ebdc.c:13-24). Called at reload-clip COMPLETION (@0x80033ec0), not entry. */
void re15_ammo_reload_exec(void)
{
    int ws = inv_resolve_slot();
    int as = re15_ammo_reserve_slot();
    if (ws < 0 || as <= 0) return;
    uint8_t wid   = g_inv.slots[ws].id;
    uint8_t chunk = (wid < 16) ? s_wpn_props[wid].chunk : 0;
    uint8_t box   = g_inv.slots[as].qty;
    if (chunk < box) {
        g_inv.slots[ws].qty = (uint8_t)(g_inv.slots[ws].qty + chunk);
        g_inv.slots[as].qty = (uint8_t)(box - chunk);
    } else {
        g_inv.slots[ws].qty = (uint8_t)(g_inv.slots[ws].qty + box);
        g_inv.slots[as].id = 0; g_inv.slots[as].qty = 0; g_inv.slots[as].flags = 0;
    }
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
