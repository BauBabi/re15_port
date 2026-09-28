/* probe_r30_granate_karte.c — MESS-WERKZEUG (kein add_test) fuer den Lade-Riegel der Granate.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero (wie
 * probe_r30_sicherung_karte.c), damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg in ROOM1150/1151 startet.
 *
 * Aufruf: probe_r30_granate_karte <kartendatei> <raum-hex> [sicherung] [genommen] [fach0]
 *   sicherung   Flag (9,53): die Sicherung ist schon genommen -> in der Fahrt geht direkt das
 *               Granaten-Modal auf (sonst wartet es hinter dem Sicherungs-Modal)
 *   genommen    Flag (9,56): die Granate ist schon genommen = Gegenprobe, der Lade-Weg darf
 *               sie NICHT anlegen
 *   fach0       legt die Granate (Item 0x09, RE15_GRANATE_MENGE Stueck) in Inventarplatz 0 —
 *               die CHECK-Haken des Ports (RE15_INV_CHECK_SHOT) arbeiten fest auf Platz 0
 * Spielerlage (-22250,0,-18500): ausserhalb des Ausloeser-Rechtecks von sub04 (Aot_set slot 1
 * @Datei 0x0D7E, Ecke (-21800,-20000), 1500x3300), dieselbe wie im Sicherungs-Werkzeug.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1150u;
    int sicherung = 0, genommen = 0, fach0 = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "sicherung") == 0)     sicherung = 1;
        else if (strcmp(argv[a], "genommen") == 0) genommen = 1;
        else if (strcmp(argv[a], "fach0") == 0)    fach0 = 1;
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

    if (sicherung) re15_game_flag_set(9, RE15_SICHERUNG_TAKEN_BIT, 1);
    if (genommen)  re15_game_flag_set(9, RE15_GRANATE_TAKEN_BIT, 1);
    if (fach0) { g_inv.slots[0].id = RE15_GRANATE_ITEM; g_inv.slots[0].qty = RE15_GRANATE_MENGE; }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    /* Gegenprobe: zurueckgelesen muessen Raum, beide Flags und Platz 0 stimmen. */
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(9, RE15_SICHERUNG_TAKEN_BIT, 0);
    re15_game_flag_set(9, RE15_GRANATE_TAKEN_BIT, 0);
    g_inv.slots[0].id = 0; g_inv.slots[0].qty = 0;
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int fs = re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) ? 1 : 0;
    int fg = re15_game_flag_get(9, RE15_GRANATE_TAKEN_BIT) ? 1 : 0;
    printf("Karte %s: Raum 0x%04X, Flag(9,%d)=%d, Flag(9,%d)=%d, Platz 0 = Item 0x%02X x%u\n",
           path, (unsigned)rr, RE15_SICHERUNG_TAKEN_BIT, fs, RE15_GRANATE_TAKEN_BIT, fg,
           (unsigned)g_inv.slots[0].id, (unsigned)g_inv.slots[0].qty);
    if (rr != (uint16_t)room || fs != sicherung || fg != genommen) { printf("FAIL: Ruecklesen\n"); return 1; }
    if (fach0 && (g_inv.slots[0].id != RE15_GRANATE_ITEM || g_inv.slots[0].qty != RE15_GRANATE_MENGE)) {
        printf("FAIL: Platz 0 traegt nach dem Ruecklesen nicht die Granate\n"); return 1;
    }
    return 0;
}
