/* probe_r17_munition.c — MESSSONDE + PIN zur NUTZER-ENTSCHEIDUNG "Munition halbieren".
 *
 * Nutzer-Auftrag 2026-09-20: "dann ist die Munition die man findet viel zu viel. ich will
 * Das du die Munition auf maximal die haelfte begrenzt die man findet."
 *
 * ⛔ Das ist KEINE byte-true Frage. Gehalbiert wird auf Wunsch, nicht weil das Original es
 * taete. Byte-true ist nur die SCHRANKE, auf die sich die Halbierung stuetzt:
 *   re15_item_is_ammo -> Id 0x15..0x21  (`sltiu id,0x15` @0x80047d54 / `sltiu id,0x22`
 *   @0x80049124, gestuetzt auf die ARMS-Kopftabelle @0x8007492c).
 *
 * Was diese Sonde festhaelt:
 *   1) MUNITION wird beim AUFSAMMELN halbiert (Abrundung, Mindestmenge 1) — und zwar
 *      END-TO-END ueber den echten Modal-Pfad (re15_item_modal_start -> Zustand 7 ->
 *      re15_inv_grant), nicht nur in der Hilfsfunktion.
 *   2) NICHT-Munition (Waffen mit geladenem Magazin, Kraeuter, Schluessel) bleibt exakt.
 *   3) Das NACHLADEN aus der Reserve (re15_ammo_reload_exec, FUN_8004ebdc @0x8004ebdc) ist
 *      NICHT halbiert — sonst verschwaende jedes Nachladen die Haelfte des Magazins.
 *   4) Die Startausruestung (re15_inv_load_briefing) ist unberuehrt.
 *
 * Aufruf: probe_r17_munition [pin]
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_inventory.h"
#include "re15_item_modal.h"

extern re15_inventory_t g_inv;

static int g_fail = 0;
#define CHECK(name, cond) do {                                             \
        int _c = (cond);                                                   \
        printf("  [%s] %s\n", _c ? "OK  " : "FAIL", (name));               \
        if (!_c) g_fail++;                                                 \
    } while (0)

/* Modal bis zum Ende durchtreiben: der Grant passiert in Zustand 7, die Abfrage
 * ("WILL YOU TAKE THE ...") wird mit der Bestaetigungs-Taste beantwortet. */
#define PAD_CONFIRM 0x4000u   /* VIRTUELLES Bestaetigen-Bit (<- roh SQUARE, @0x80073dbc[14]) */

static void modal_durchlaufen(uint8_t id, uint8_t menge)
{
    re15_item_modal_start(id, menge, 0, -1, 0xFF);
    int wache = 0;
    /* erst die Schreibmaschine auslaufen lassen (Bestaetigen waehrend des Tippens wird
     * byte-true ignoriert), dann bestaetigen. */
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && wache++ < 800)
        re15_item_modal_tick(0, 0);
    wache = 0;
    while (re15_item_modal_active() && wache++ < 800)
        re15_item_modal_tick((uint16_t)PAD_CONFIRM, 0);
}

static int menge_im_beutel(uint8_t id)
{
    int s = re15_inv_find_item(id);
    return (s < 0) ? -1 : (int)g_inv.slots[s].qty;
}

int main(int argc, char **argv)
{
    int pin = (argc > 1 && strcmp(argv[1], "pin") == 0);

    printf("=== A: Hilfsfunktion re15_pickup_menge_nutzer (Id-Fenster + Abrundung) ===\n");
    /* Alle im Auslieferungsstand vorkommenden Munitions-Packungen (Zensus
     * tools/aot_sce_census.py ueber 240 RDTs): 6, 7, 12, 14, 15, 30. */
    static const struct { uint8_t menge, soll; } pk[] = {
        { 6, 3 }, { 7, 3 }, { 12, 6 }, { 14, 7 }, { 15, 7 }, { 30, 15 },
    };
    for (unsigned i = 0; i < sizeof pk / sizeof pk[0]; i++) {
        uint8_t got = re15_pickup_menge_nutzer(0x15, pk[i].menge);
        char nm[64];
        snprintf(nm, sizeof nm, "H.GUN BULLETS x%u -> %u (soll %u)",
                 pk[i].menge, got, pk[i].soll);
        CHECK(nm, got == pk[i].soll);
    }
    CHECK("Mindestmenge: Munition x1 bleibt 1", re15_pickup_menge_nutzer(0x15, 1) == 1);
    CHECK("Munition x0 bleibt 0 (Grant weist es ohnehin ab)",
          re15_pickup_menge_nutzer(0x15, 0) == 0);
    CHECK("Id 0x21 (letzte Munition) wird halbiert", re15_pickup_menge_nutzer(0x21, 30) == 15);
    CHECK("Id 0x22 (erster Schluessel/Heil) bleibt", re15_pickup_menge_nutzer(0x22, 30) == 30);
    CHECK("Id 0x14 (letzte Waffe) bleibt",           re15_pickup_menge_nutzer(0x14, 30) == 30);
    CHECK("Id 0x0c INGRAM M10 x100 (Waffe mit Magazin) bleibt",
          re15_pickup_menge_nutzer(0x0c, 100) == 100);
    CHECK("Id 0x24 GREEN MEDICINE x1 bleibt", re15_pickup_menge_nutzer(0x24, 1) == 1);

    printf("\n=== B: ECHTER Aufnahme-Pfad (Modal -> Zustand 7 -> re15_inv_grant) ===\n");
    re15_inv_init();
    modal_durchlaufen(0x15, 30);
    int m = menge_im_beutel(0x15);
    printf("  H.GUN BULLETS x30 aufgesammelt -> im Beutel: %d\n", m);
    CHECK("Aufnahme 30 -> 15 im Beutel", m == 15);

    re15_inv_init();
    modal_durchlaufen(0x16, 7);
    m = menge_im_beutel(0x16);
    printf("  SHOTGUN SHELLS x7 aufgesammelt -> im Beutel: %d\n", m);
    CHECK("Aufnahme 7 -> 3 im Beutel", m == 3);

    re15_inv_init();
    modal_durchlaufen(0x24, 1);
    m = menge_im_beutel(0x24);
    printf("  GREEN MEDICINE x1 aufgesammelt -> im Beutel: %d\n", m);
    CHECK("Kraut x1 bleibt 1", m == 1);

    printf("\n=== C: NACHLADEN ist NICHT halbiert (FUN_8004ebdc @0x8004ebdc) ===\n");
    /* Startausruestung: Slot0 Messer (equip), Slot1 BROWNING HP x15, Slot2 H.GUN BULLETS x50. */
    re15_inv_init();
    re15_inv_load_briefing();
    int sl_w = re15_inv_find_item(0x03);       /* BROWNING HP */
    int sl_a = re15_inv_find_item(0x15);       /* Reserve */
    printf("  Startausruestung: Waffe slot=%d qty=%d, Reserve slot=%d qty=%d\n",
           sl_w, sl_w >= 0 ? g_inv.slots[sl_w].qty : -1,
           sl_a, sl_a >= 0 ? g_inv.slots[sl_a].qty : -1);
    CHECK("Startausruestung unberuehrt: Reserve x50", sl_a >= 0 && g_inv.slots[sl_a].qty == 50);
    CHECK("Startausruestung unberuehrt: Magazin x15", sl_w >= 0 && g_inv.slots[sl_w].qty == 15);

    /* Magazin leerschiessen, dann nachladen: Chunk 15 (props[3].chunk @0x80074da8). */
    re15_inv_set_equipped_slot(sl_w);
    while (re15_ammo_consume()) { }
    CHECK("Magazin leer", g_inv.slots[sl_w].qty == 0);
    re15_ammo_reload_exec();
    printf("  nach Nachladen: Magazin=%d Reserve=%d\n",
           g_inv.slots[sl_w].qty, g_inv.slots[sl_a].qty);
    CHECK("Nachladen gibt VOLLE 15 (nicht 7)", g_inv.slots[sl_w].qty == 15);
    CHECK("Reserve faellt um VOLLE 15 (50 -> 35)", g_inv.slots[sl_a].qty == 35);

    printf("\n%s (%d Fehler)\n", g_fail ? "FEHLER" : "ALLES GRUEN", g_fail);
    if (!pin) return 0;
    return g_fail ? 1 : 0;
}
