/* probe_r30_sicherung_karte.c — MESS-WERKZEUG (kein Pin, kein add_test).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero, damit die echte
 * exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg in ROOM1150/1151 startet — genau der Weg, an dem die Sicherung im
 * Hebetisch bis Runde 30 fehlte (Dossier analysis/befunde_runde30/sicherung.md §2.4).
 *
 * Aufruf: probe_r30_sicherung_karte <kartendatei> [<raum-hex>] [genommen] [fach0]
 *   <raum-hex>   1150 (Standard) oder 1151
 *   genommen     setzt das Genommen-Flag (9,53) im Spielstand = Gegenprobe: dann darf
 *                der Lade-Weg die Sicherung NICHT anlegen
 *   fach0        legt die Sicherung (Item 0x40, 1 Stueck) in Inventarplatz 0 — die
 *                Inventar-Abnahmehaken des Ports (RE15_INV_CHECK_SHOT, main.c) arbeiten
 *                fest auf Platz 0. Gebraucht von integration_r30_sicherung_bild.
 *
 * Spielerlage (-22250,0,-18500): Westseite des Raums, ausserhalb des Ausloeser-Rechtecks
 * von sub04 (Aot_set slot 1 @Datei 0x0D7E: Ecke (-21800,-20000), Groesse 1500x3300) —
 * dieselbe Lage, die der Messlauf lauf_laden.sh fuer seinen Spielstand benutzt.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_sicherung.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1150u;
    int genommen = 0, fach0 = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "genommen") == 0)   genommen = 1;
        else if (strcmp(argv[a], "fach0") == 0) fach0 = 1;
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }

    if (room != 0x1150u && room != 0x1151u) {
        printf("FAIL: Raum 0x%04X ist nicht Irons' Buero (1150/1151)\n", room);
        return 2;
    }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();

    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22250; pl->y = 0; pl->z = -18500; pl->rot_y = 0;

    if (genommen) re15_game_flag_set(9, RE15_SICHERUNG_TAKEN_BIT, 1);
    if (fach0) { g_inv.slots[0].id = RE15_SICHERUNG_ITEM; g_inv.slots[0].qty = 1; }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }

    /* Gegenprobe: zurueckgelesen muessen Raum und Flag stimmen. */
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(9, RE15_SICHERUNG_TAKEN_BIT, 0);
    g_inv.slots[0].id = 0; g_inv.slots[0].qty = 0;
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int flag = re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) ? 1 : 0;
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, Flag(9,%d)=%d\n",
           path, (unsigned)rr, RE15_SICHERUNG_TAKEN_BIT, flag);
    if (rr != (uint16_t)room)  { printf("FAIL: Raum nach dem Ruecklesen 0x%04X\n", rr); return 1; }
    if (flag != genommen)      { printf("FAIL: Flag nach dem Ruecklesen %d\n", flag);   return 1; }
    printf("Inventarplatz 0 nach dem Ruecklesen: Item 0x%02X x%u\n",
           (unsigned)g_inv.slots[0].id, (unsigned)g_inv.slots[0].qty);
    if (fach0 && g_inv.slots[0].id != RE15_SICHERUNG_ITEM) {
        printf("FAIL: Inventarplatz 0 traegt nach dem Ruecklesen nicht Item 0x40\n"); return 1;
    }
    return 0;
}
