/* probe_r30_irons_tisch_karte.c — MESS-WERKZEUG (kein add_test) fuer den Lade-Riegel
 * integration_r30_irons_tisch_laden (Runde 30, Thema irons-diary-welt).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero, damit die echte exe
 * ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 am LADE-Weg in ROOM1150/1151
 * startet — dem Weg, der nicht durch scd_room_reenter geht (Dossier sicherung.md §2.4).
 * Vorlage: probe_r30_sicherung_karte.c.
 *
 * Aufruf: probe_r30_irons_tisch_karte <kartendatei> [<raum-hex>] [genommen]
 *   <raum-hex>  1150 (Standard) oder 1151
 *   genommen    setzt die Genommen-Bits (9,54) und (9,55) im Spielstand — dann darf der
 *               Lade-Weg weder Buch noch Karte anlegen
 *   pos=x,z,rot Spielerlage im Spielstand (Standard -22250,-18500,0) — fuer die Abnahme-
 *               Laeufe "nach dem Laden aufheben" (Stand an der Tischkante, Blick -X)
 *
 * Spielerlage (-22250,0,-18500): Westseite des Raums vor dem Schreibtisch-Gang, ausserhalb
 * der beiden Aufhebe-Rechtecke (x -24000..-23000) — dieselbe Lage wie der Sicherungs-Riegel.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_irons_tisch.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1150u;
    int genommen = 0, px = -22250, pz = -18500, prot = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "genommen") == 0) genommen = 1;
        else if (strncmp(argv[a], "pos=", 4) == 0 &&
                 sscanf(argv[a] + 4, "%d,%d,%d", &px, &pz, &prot) == 3) { }
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
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = (int16_t)prot;
    if (genommen) {
        re15_game_flag_set(9, RE15_IRONS_DIARY_TAKEN_BIT, 1);
        re15_game_flag_set(9, RE15_IRONS_KARTE_TAKEN_BIT, 1);
    }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_flag_set(9, RE15_IRONS_DIARY_TAKEN_BIT, 0);
    re15_game_flag_set(9, RE15_IRONS_KARTE_TAKEN_BIT, 0);
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int f54 = re15_game_flag_get(9, RE15_IRONS_DIARY_TAKEN_BIT) ? 1 : 0;
    int f55 = re15_game_flag_get(9, RE15_IRONS_KARTE_TAKEN_BIT) ? 1 : 0;
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, Flag(9,54)=%d Flag(9,55)=%d\n",
           path, (unsigned)rr, f54, f55);
    if (rr != (uint16_t)room)             { printf("FAIL: Raum 0x%04X\n", rr); return 1; }
    if (f54 != genommen || f55 != genommen) { printf("FAIL: Flags nach dem Ruecklesen\n"); return 1; }
    return 0;
}
