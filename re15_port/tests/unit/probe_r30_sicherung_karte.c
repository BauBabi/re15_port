/* probe_r30_sicherung_karte.c — MESS-WERKZEUG (kein Pin, kein add_test).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero, damit die echte
 * exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg in ROOM1150/1151 startet — genau der Weg, an dem die Sicherung im
 * Hebetisch bis Runde 30 fehlte (Dossier analysis/befunde_runde30/sicherung.md §2.4).
 *
 * Aufruf: probe_r30_sicherung_karte <kartendatei> [<raum-hex>] [genommen]
 *   <raum-hex>   1150 (Standard) oder 1151
 *   genommen     setzt das Genommen-Flag (9,53) im Spielstand = Gegenprobe: dann darf
 *                der Lade-Weg die Sicherung NICHT anlegen
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1150u;
    int genommen = (argc > 3 && strcmp(argv[3], "genommen") == 0);

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
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int flag = re15_game_flag_get(9, RE15_SICHERUNG_TAKEN_BIT) ? 1 : 0;
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, Flag(9,%d)=%d\n",
           path, (unsigned)rr, RE15_SICHERUNG_TAKEN_BIT, flag);
    if (rr != (uint16_t)room)  { printf("FAIL: Raum nach dem Ruecklesen 0x%04X\n", rr); return 1; }
    if (flag != genommen)      { printf("FAIL: Flag nach dem Ruecklesen %d\n", flag);   return 1; }
    return 0;
}
