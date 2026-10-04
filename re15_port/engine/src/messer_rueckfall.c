/*
 * RE1.5 Rebuilt — Kampfmesser als Rueckfall statt Inventar-Gegenstand (Runde 35, Spur E).
 * Herleitung und Belege: include/re15_messer.h und analysis/befunde_runde35/E_inventar1050.md.
 */
#include "re15_messer.h"

#include "re15_damage.h"      /* re15_player_set_equipped_weapon (DAT_800aca5d) */
#include "re15_gameflow.h"    /* g_gameflow.character (DAT_800aca5c)             */
#include "re15_inventory.h"
#include "re15_itembox.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* Starttabellen des Originals (PSX.EXE, re15_disasm.py bytes 0x80074bb8 48):
 *   80074bb8: 01 03 15 00 00 00   Leon-Ids   (Lesestelle @0x80045e44 / @0x80045e74)
 *   80074bc4: 01 00 00 00 00 00   Elza-Ids   (Lesestelle @0x80045ef0 / @0x80045f20)
 *   80074bd0: 00 0f 32 00 00 00   Mengen     (Lesestelle @0x80045e98 / @0x80045f44, beide Tabellen)
 * Sechs Eintraege je Charakter (Schleifenende `sltiu v0,v0,0x6` @0x80045ecc / @0x80045f78). */
static const uint8_t k_ids_leon[6] = { 0x01, 0x03, 0x15, 0x00, 0x00, 0x00 };
static const uint8_t k_ids_elza[6] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t k_menge[6]    = { 0x00, 0x0f, 0x32, 0x00, 0x00, 0x00 };

void re15_messer_startinventar(void)
{
    re15_inv_init();
    /* Charakterweiche @0x80045e20 `lbu v0,-13732(v0)` (0x800aca5c) / @0x80045e28 `sltiu v0,v0,0x4`:
     * < 4 = Leon-Tabelle, sonst Elza-Tabelle. */
    const uint8_t *ids = ((unsigned)g_gameflow.character < 4u) ? k_ids_leon : k_ids_elza;
    int n = 0;
    for (int i = 0; i < 6; i++) {
        if (ids[i] == 0x00) continue;                     /* leerer Eintrag                     */
        if (ids[i] == RE15_MESSER_ID) continue;           /* NUTZER-VORGABE: Messer nicht hinein */
        g_inv.slots[n].id    = ids[i];
        g_inv.slots[n].qty   = k_menge[i];
        g_inv.slots[n].flags = 0;
        n++;
    }
    /* 25c9 := 0x80 wie @0x80045fe0; 25c8 := 0x80 statt 0 (@0x80045fec zeigte auf das Messer in
     * Platz 0, das es nicht mehr gibt) — der Wert, den das Original selbst fuer "nichts" fuehrt. */
    re15_inv_set_prev_equip_slot(RE15_MESSER_NICHTS);
    /* Waffen-Id nach der Commit-Regel @0x80046654-88 (0x80 -> 1). re15_player_set_equipped_weapon
     * leitet den Platz per Suche ab (kein Messer im Inventar -> 0x80); danach explizit festnageln. */
    re15_player_set_equipped_weapon(RE15_MESSER_ID);
    re15_inv_set_equipped_slot(RE15_MESSER_NICHTS);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[messer] Startinventar Charakter %d:", g_gameflow.character);
    for (int i = 0; i < n; i++)
        fprintf(stderr, " %02x x%u", (unsigned)g_inv.slots[i].id, (unsigned)g_inv.slots[i].qty);
    fprintf(stderr, " | Ausruest-Platz 0x%02x, Waffe %d\n", (unsigned)re15_inv_equipped_slot(),
            re15_player_equipped_weapon());
#endif
}

int re15_messer_aus_inventar(void)
{
    int weg = 0;
    /* Inventar: ein ausgeruestetes Messer erst ablegen (wie UNEQUIP, 25c8 := 0x80), dann den Platz
     * leeren + verdichten (re15_inv_remove_slot = Verbrauch @0x8004aef0 + FUN_8004dadc, das den
     * Ausruest-Platz mitzieht @0x8004dbac-c8). */
    for (int i = 0; i < RE15_INV_MAX_SLOTS; ) {
        if (g_inv.slots[i].id != RE15_MESSER_ID) { i++; continue; }
        if (re15_inv_equipped_slot() == i) re15_inv_set_equipped_slot(RE15_MESSER_NICHTS);
        re15_inv_remove_slot(i);                          /* Platz i ist danach der Nachfolger */
        weg++;
    }
    if (re15_inv_equipped_slot() == RE15_MESSER_NICHTS) {
        re15_player_set_equipped_weapon(RE15_MESSER_ID);  /* Commit-Regel 0x80 -> 1 @0x8004666c */
        re15_inv_set_equipped_slot(RE15_MESSER_NICHTS);
    }
    /* Kiste (Port-Zusatz, RE2-Form): ein dort abgelegtes Messer verschwindet ebenfalls, sonst kaeme
     * es beim Herausnehmen zurueck ins Inventar. Kistenplaetze werden nicht verdichtet (leer = Id 0). */
    {
        re15_inv_slot_t box[RE15_BOX_SLOTS];
        int kiste = 0;
        re15_itembox_export(box);
        for (int i = 0; i < RE15_BOX_SLOTS; i++)
            if (box[i].id == RE15_MESSER_ID) {
                box[i].id = 0; box[i].qty = 0; box[i].flags = 0; box[i].pad = 0;
                kiste++;
            }
        if (kiste) re15_itembox_import(box);
        weg += kiste;
    }
#ifdef RE15_PLATFORM_PC
    if (weg)
        fprintf(stderr, "[messer] alter Spielstand: %d Messer entfernt, Ausruest-Platz 0x%02x, Waffe %d\n",
                weg, (unsigned)re15_inv_equipped_slot(), re15_player_equipped_weapon());
#endif
    return weg;
}
