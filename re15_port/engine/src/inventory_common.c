/*
 * RE1.5 Rebuilt — Inventory impl (Phase 4.4.7, 2026-05-19).
 *
 * Target-agnostic. Minimal grant/decrement logic for the demo.
 */
#include <string.h>
#include "re15_inventory.h"
#include "re15_inv_ui.h"       /* die VERBATIM ausgelieferten EXE-Bytes [0x80074a8c,0x800762a0):
                               * hier gebraucht fuer die Eigenschaftstabelle @0x80074da8 */
#include "re15_inv_screen.h"   /* wave 5: icon-cache cell art maintenance (the original's
                                * FUN_80049390 copies / FUN_8004947c blanks are part of the
                                * slot ops FUN_8004dadc/FUN_8004dc4c — see the disasm cites
                                * at the call sites below) */

#define HUD_PICKUP_DISPLAY_FRAMES 90   /* 1.5 s at 60 fps */

re15_inventory_t g_inv;

void re15_inv_init(void)
{
    memset(&g_inv, 0, sizeof(g_inv));
    re15_inv_icon_reset();
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
         * it keeps tracking its weapon (@0x8004dc88, FUN_8004ea6c). The original moves the icon
         * cells along (FUN_8004dc4c: 8x jal 0x80049390 @0x8004dc98-de3c) and uploads the wide icon
         * over cells 0/1 (jal 0x800492b8 @0x8004df08) — mirrored on the override layer (wave 5). */
        for (int i = 7; i >= 0; i--) {
            g_inv.slots[i + 2] = g_inv.slots[i];
            re15_inv_icon_copy(i, i + 2);
        }
        g_inv.slots[0].id = type; g_inv.slots[0].qty = amount; g_inv.slots[0].flags = 1;
        g_inv.slots[1].id = type; g_inv.slots[1].qty = amount; g_inv.slots[1].flags = 2;
        re15_inv_icon_blank(0); re15_inv_icon_blank(1);   /* fresh upload = identity art */
        re15_inv_set_equipped_slot(re15_inv_equipped_slot() + 2);
    } else {
        /* everything else = 1 cell in the first FREE slot (raw qty, flags 0). The insert
         * uploads the item's icon (FUN_800492b8 @0x8004df08) = identity art. */
        g_inv.slots[free_slot].id    = type;
        g_inv.slots[free_slot].qty   = amount;
        g_inv.slots[free_slot].flags = 0;
        re15_inv_icon_blank(free_slot);
    }
    g_inv.last_pickup_type           = type;
    g_inv.last_pickup_amount         = amount;
    g_inv.last_pickup_display_frames = HUD_PICKUP_DISPLAY_FRAMES;
    return 0;
}

/* FUN_8004dadc @0x8004dadc — post-USE slot compaction: FUN_8004df2c finds the first FREE (id==0)
 * slot, then every slot after it shifts DOWN one to close the hole (@0x8004db3c-dba8 copies
 * slot[s0] -> slot[s0-1]), and the equipped-slot index decrements if it pointed past the hole
 * (@0x8004dbac-dbc8: `bne equipped,s0` else `equipped--`). The original bounds the shift with the
 * live count byte @0x800b0fbc; the port scans its fixed slots and clears the vacated tail. */
void re15_inv_compact(void)
{
    int f = -1;
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == 0) { f = i; break; }
    if (f < 0 || f >= RE15_INV_MAX_SLOTS - 1) return;      /* no hole (or the hole is already last) */
    for (int i = f; i < RE15_INV_MAX_SLOTS - 1; i++) {
        re15_inv_icon_copy(i + 1, i);                      /* icon cell s0 -> s0-1 per shifted slot
                                                            * (jal 0x80049390 @0x8004db30, a0=s0
                                                            * a1=s0-1 @0x8004db24-2c) — wave 5      */
        g_inv.slots[i] = g_inv.slots[i + 1];               /* shift the tail down to close the hole */
    }
    g_inv.slots[RE15_INV_MAX_SLOTS - 1].id    = 0;         /* clear the now-duplicated last slot    */
    g_inv.slots[RE15_INV_MAX_SLOTS - 1].qty   = 0;
    g_inv.slots[RE15_INV_MAX_SLOTS - 1].flags = 0;
    re15_inv_icon_blank(9);                                /* final blank of the vacated last cell
                                                            * (jal 0x8004947c @0x8004dbec, a0 =
                                                            * count-1 with count @0x800b0fbc = 10)  */
    { int eq = re15_inv_equipped_slot();                   /* equipped-- iff it pointed past the hole */
      if (eq != 0x80 && eq > f) re15_inv_set_equipped_slot(eq - 1); }
}

/* Clear an inventory slot (byte-true item-USE consume @0x8004aef0-af28: DAT_800b10ac[slot] id/qty/flag
 * = 0), then run the post-USE compaction FUN_8004dadc (@0x8004aef0 zeroes, then jal FUN_8004dadc).
 * Used by the heal-USE FSM (item_use_common.c). */
void re15_inv_remove_slot(int slot)
{
    if (slot >= 0 && slot < RE15_INV_MAX_SLOTS) {
        g_inv.slots[slot].id    = 0;
        g_inv.slots[slot].qty   = 0;
        g_inv.slots[slot].flags = 0;
        re15_inv_compact();                                /* close the hole (byte-true FUN_8004dadc) */
    }
}

/* Byte-true STAGE1 briefing loadout (mzd_stage1_briefing.sav, DAT_800b10ac 4-byte slots). */
void re15_inv_load_briefing(void)
{
    re15_inv_init();                                    /* clear first */
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;   /* COMBAT KNIFE (the equipped weapon, DAT_800aca5d==1) */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;  /* BROWNING HP (handgun, 15-round clip)                */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;  /* H. GUN BULLETS (ammo)                               */
    /* game-start equip-history init: DAT_800b25c9 := 0x80 @0x80045fe0 (the same init that
     * writes 25c8:=0 @0x80045fec; savestate mzd_inv_open.sav confirms 25c9==0x80 live). */
    re15_inv_set_prev_equip_slot(0x80);
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

/* DAT_800b25c9 — previous equip slot (wave 3; see re15_inventory.h). BSS initial 0; the
 * game-start init writes 0x80 (@0x80045fe0, hooked into re15_inv_load_briefing); the only
 * other writers are the equip step-5 commit (@0x8004abe0) and the swap step-0 commit
 * (@0x8004b0e8) in menu_common.c. */
static uint8_t s_prev_equip_slot = 0;
int  re15_inv_prev_equip_slot(void)      { return s_prev_equip_slot; }
void re15_inv_set_prev_equip_slot(int s) { s_prev_equip_slot = (uint8_t)s; }

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

/* ⛔ NUTZER-ENTSCHEIDUNG, NICHT BYTE-TRUE — Herleitung + Gueltigkeitsbereich stehen ueber der
 * Deklaration in re15_inventory.h. Halbiert die Stueckzahl EINER aufgesammelten Munitions-
 * Packung; alles andere gibt die Menge unveraendert zurueck.
 *
 * Die Id-Schranke selbst ist byte-true (re15_item_is_ammo: `sltiu id,0x15` @0x80047d54 /
 * `sltiu id,0x22` @0x80049124) — GEHALBIERT wird auf Nutzer-Wunsch, nicht weil das Original
 * das taete. Abrundung + Mindestmenge 1: eine 1er-Packung (die es im Auslieferungsstand bei
 * Munition nicht gibt — kleinste gemessene Packung ist 6) darf nicht auf 0 fallen, weil
 * re15_inv_grant amount==0 zurueckweist (test_inv_grant "reject amount 0") und der Gegenstand
 * dann spurlos verschwaende. */
uint8_t re15_pickup_menge_nutzer(uint8_t item_id, uint8_t menge)
{
    if (!re15_item_is_ammo(item_id)) return menge;   /* Waffen/Kraeuter/Schluessel unveraendert */
    if (menge <= 1)                  return menge;   /* 0 bleibt 0 (Grant weist es ohnehin ab) */
    uint8_t h = (uint8_t)(menge / 2u);
    return h ? h : (uint8_t)1;
}

/* ================= NUTZER-ENTSCHEIDUNG: Munition gleicher Sorte beim Aufnehmen stapeln ========
 * Nutzer-Auftrag 2026-09-26: "Munition gleicher Sorte kannst du bitte gleich Stapeln, wenn du
 * sie aufnimmst." — bestaetigt als BEWUSSTE Abweichung ("stapeln und sound ist fertiges
 * resident evil 2 und will ich bewusst haben").
 *
 * ⛔ GEMESSEN UND UNABHAENGIG NACHGEPRUEFT: WEDER RE1.5 NOCH RE2 stapelt beim Aufnehmen.
 *   RE1.5 hat genau EINEN Einfuege-Pfad fuer Welt-Items, FUN_8004dc4c @0x8004dc4c, mit genau
 *   EINEM Aufrufer (@0x8001e0c4, Item-Get-FSM Zustand 7) und keinem Datenwort-Zeiger; er
 *   sucht den ersten FREIEN Platz (jal 0x8004df2c @0x8004debc) und SCHREIBT die Menge roh:
 *     8004dee4: sb s0,0(at)   ; slot.id
 *     8004def4: sb s1,0(at)   ; slot.qty  <- ZUWEISUNG, keine Addition
 *     8004df04: sb zero,0(at) ; slot.flag
 *   Die Obergrenzen-Tabelle @0x80074da8 wird im ganzen Insert-Bereich
 *   0x8004dc4c..0x8004df28 nicht gelesen. RE2 ebenso (Insert FUN_80069adc, Aufrufer
 *   jal 0x80069adc @0x80058864 / @0x80072320, kein Gleiche-Id-Sucher).
 * Also: NUTZER-ENTSCHEIDUNG, hier klar gekennzeichnet und NIE als byte-true ausgegeben.
 *
 * ERFUNDEN wird trotzdem nichts: die Arithmetik ist woertlich aus der einzigen Stelle des
 * Originals geborgt, die ueberhaupt stapelt — der MENUE-Zusammenlegung FUN_8004e054
 * (manuelle Aktion, Aufrufer @0x8004b3b8 hinter `andi v0,v1,0x4000` = Bestaetigen):
 *     8004e13c: lbu a0,0(at)          ; qty[Ziel]
 *     8004e14c: lbu v0,0(at)          ; qty[Quelle]
 *     8004e160: addu s0,a0,v0         ; SUMME (32 Bit, kein Byte-Ueberlauf)
 *     8004e330: addiu at,at,19880     ; 0x80074da8
 *     8004e338: lbu  v1,0(at)         ; OBERGRENZE(Id) als u8
 *     8004e340: sltu v0,v1,s0         ; Obergrenze < Summe ?
 *   ja  -> 8004e3f4: sb v1,0(at)      ; Ziel = Obergrenze
 *          8004e410: subu v0,s0,v0    ; Rest = Summe - Obergrenze
 *   nein-> 8004e398: addu v0,v0,a2 / 8004e3a8: sb v0,0(at)   ; Ziel = Summe
 * Die Zahlen 250/100 stehen NICHT im Code — sie werden aus der ausgelieferten Tabelle
 * gelesen (re15_inv_ui_blob deckt [0x80074a8c,0x800762a0), also auch 0x80074da8). */

#define INV_PROP_TBL   0x80074DA8u   /* Eigenschaftstabelle, Schrittweite 12:
                                      * {+0 u32 Obergrenze, +4 u32 Kombinations-Satz,
                                      *  +8 u8 Art, +9 u8 Anzahl} — Leser des Originals
                                      * @0x8004e330-38 (+0) und @0x8004e9d8/@0x8004e9e8 (+4) */
#define INV_PROP_NULL  0x80074C88u   /* der ALL-NULL-Satz `00 00 00 00` @0x80074c88, auf den
                                      * alle Posten OHNE eigenen Kombinations-Satz zeigen */

static uint32_t inv_prop_u32(uint32_t addr)
{
    const unsigned char *p = RE15_INV_PTR(addr);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Obergrenze wie das Original sie liest: als EIN BYTE (`lbu v1,0(at)` @0x8004e338),
 * nicht als u32 — deshalb hier ebenfalls u8. */
static int inv_obergrenze(uint8_t id)
{
    return (int)*RE15_INV_PTR(INV_PROP_TBL + (uint32_t)id * 12u);
}

/* Gilt die Nutzer-Stapelregel fuer diese Id?
 * (a) byte-true Id-Fenster 0x15..0x21 (re15_item_is_ammo: `sltiu id,0x15` @0x80047d54 /
 *     `sltiu id,0x22` @0x80049124) — das ist aber nur eine KLASSE, kein Munitionsbeweis,
 * (b) GESCHNITTEN mit "hat einen EIGENEN Kombinations-Satz", Feld +4 der ausgelieferten
 *     Tabelle @0x80074da8 (dasselbe Feld liest das Original @0x8004e9d8).
 * GEMESSEN ueber das ganze Fenster: 0x15..0x20 tragen eigene Saetze (0x80074ccc, 0x80074ce8,
 * 0x80074cf4, 0x80074cfc, 0x80074d04, 0x80074d08, 0x80074d0c, 0x80074d10, 0x80074d20,
 * 0x80074d30, 0x80074d3c, 0x80074d48) — NUR 0x21 zeigt auf den Null-Satz @0x80074c88.
 * 0x21 ist im ausgelieferten Namens-Blob "Memory Card" (@0x800c4a28 + Offset[0x21]), also
 * keine Munition. Genau diese eine Id faellt damit datengetrieben heraus, ohne Id-Liste.
 * (re15_item_is_ammo selbst bleibt unangetastet — sie ist byte-true und wird anderswo
 *  gebraucht.) */
int re15_pickup_stapelt_nutzer(uint8_t item_id)
{
    if (!re15_item_is_ammo(item_id)) return 0;
    return inv_prop_u32(INV_PROP_TBL + (uint32_t)item_id * 12u + 4u) != INV_PROP_NULL;
}

/* Passt die GANZE Menge auf einen schon vorhandenen Platz, ohne dass ein freier Platz
 * gebraucht wird? Nur dann darf die Aufnahme bei VOLLEM Inventar weitergehen
 * (item_modal_common.c Zustand 5). Sonst bleibt der byte-true "kann ich nicht tragen"-Zweig
 * (@0x8001df14 Vorpruefung + @0x8001e054 bltz) unveraendert. */
int re15_pickup_passt_nutzer(uint8_t item_id, uint8_t menge)
{
    if (!re15_pickup_stapelt_nutzer(item_id) || menge == 0) return 0;
    int ziel = re15_inv_find_item(item_id);            /* FUN_8004dfec @0x8004dfec */
    if (ziel < 0) return 0;
    return ((int)g_inv.slots[ziel].qty + (int)menge) <= inv_obergrenze(item_id);
}

/* Gutschrift MIT Nutzer-Stapelung. Rueckgabe wie re15_inv_grant: 0 = die GANZE Menge ist
 * gutgeschrieben, -1 = NICHTS ist gutgeschrieben.
 *
 * ⛔ DIE MENGE WIRD IN JEDEM ZWEIG GENAU EINMAL GUTGESCHRIEBEN, und jeder Zweig ist
 * ATOMAR: der Teiluebertrag ruft ERST re15_inv_grant fuer den Rest und schreibt die
 * Obergrenze in den Zielplatz erst danach. Scheitert der Grant (kein freier Platz), ist
 * kein Byte des Inventars veraendert und die Aufnahme faellt sauber auf den byte-true
 * Voll-Zweig zurueck — sonst laege die Packung weiter in der Welt, waehrend ein Teil schon
 * im Beutel waere (unbegrenzte Munition durch wiederholtes Ansprechen).
 * re15_inv_grant kann den Zielplatz dabei nicht verschieben: der 2-Zell-Schub gilt nur fuer
 * Ids 0x0e..0x13 (`sltiu v0,v0,0x6` @0x8004dc78), Munition liegt darueber. */
int re15_inv_grant_stapeln_nutzer(uint8_t item_id, uint8_t menge)
{
    if (item_id == 0 || menge == 0) return -1;              /* wie re15_inv_grant */
    if (!re15_pickup_stapelt_nutzer(item_id))
        return re15_inv_grant(item_id, menge);              /* byte-true Insert, unveraendert */

    int ziel = re15_inv_find_item(item_id);                 /* FUN_8004dfec @0x8004dfec */
    if (ziel < 0)
        return re15_inv_grant(item_id, menge);              /* nichts zum Stapeln da */

    int cap   = inv_obergrenze(item_id);                    /* lbu @0x8004e338 */
    int summe = (int)g_inv.slots[ziel].qty + (int)menge;    /* addu @0x8004e160 */

    if (cap < summe) {                                      /* sltu @0x8004e340 */
        int rest = summe - cap;                             /* subu @0x8004e410 */
        if (rest > 255) return -1;                          /* passt nicht in ein qty-Byte;
                                                             * im Auslieferungsstand
                                                             * unerreichbar (groesste Packung
                                                             * 30, davor halbiert) */
        if (re15_inv_grant(item_id, (uint8_t)rest) != 0) return -1;  /* ATOMAR: nichts getan */
        g_inv.slots[ziel].qty = (uint8_t)cap;               /* sb @0x8004e3f4 */
    } else {
        g_inv.slots[ziel].qty = (uint8_t)summe;             /* addu/sb @0x8004e398-3a8 */
    }
    g_inv.last_pickup_type           = item_id;
    g_inv.last_pickup_amount         = menge;               /* die GANZE aufgenommene Menge */
    g_inv.last_pickup_display_frames = HUD_PICKUP_DISPLAY_FRAMES;
    return 0;
}

/* Byte-true item name catalog 0x00..0x2f (DAT_800c4a28 glyph blob, decoded via the item-prompt font
 * map). The HUD uses this ASCII form (port convention: caps for the 6x8 debug font); the byte-true
 * Title-Case glyphs the GAME draws come from the blob directly in the prompt (item_prompt_common.c).
 * Heal items 0x22..0x2e are the First Aid / medicine set the item-USE flow consumes (see re15_item_use). */
static const char *const s_item_names[] = {
    "",               "COMBAT KNIFE",   "PIPE",           "BROWNING HP",      /* 0x00..0x03 */
    "SIG P228",       "BERETTA M93R",   "GLOCK 18",       "SUPER REDHAWK",    /* 0x04..0x07 */
    "REMINGTON M870", "HAND GRENADE",   "ACID GRENADE",   "INCEND. GRENADE",  /* 0x08..0x0b */
    "INGRAM M10",     "SPAS-12",        "FLAMETHROWER",   "GRENADE LAUNCHER", /* 0x0c..0x0f */
    "GRENADE LAUNCHER","GRENADE LAUNCHER","ROCKET LAUNCHER","H&K MC51",       /* 0x10..0x13 */
    "COLT PYTHON",    "H. GUN BULLETS", "SHOTGUN SHELLS", "MAGNUM BULLETS",   /* 0x14..0x17 */
    "FLAME FUEL",     "EXPLOSIVE RND",  "ACID ROUNDS",    "INCEND. ROUNDS",   /* 0x18..0x1b */
    "REMOTE DETON.",  "EMPTY SHELLS",   "NITRO CAPSULE",  "ACID CAPSULE",     /* 0x1c..0x1f */
    "INCEND. CAPSULE","MEMORY CARD",    "FIRST AID SPRAY","ANTIDOTE SPRAY",   /* 0x20..0x23 */
    "GREEN MEDICINE", "RED MEDICINE",   "BLUE MEDICINE",  "G.R MEDICINE MIX", /* 0x24..0x27 */
    "G.G MEDICINE MIX","G.B MEDICINE MIX","G.G.R MED. MIX","G.G.G MED. MIX",  /* 0x28..0x2b */
    "G.G.B MED. MIX", "G.R.R MED. MIX", "G.R.B MED. MIX", "NUT",              /* 0x2c..0x2f */
};
const char *re15_item_name(uint8_t id)
{
    if (id >= (uint8_t)(sizeof s_item_names / sizeof s_item_names[0])) return "";
    return s_item_names[id];
}
