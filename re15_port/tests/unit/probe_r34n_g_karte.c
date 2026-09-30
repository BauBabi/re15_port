/* probe_r34n_g_karte.c — Spur G2 (Runde 34 Nacht), MESS-WERKZEUG (kein Pin, kein add_test).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero, dessen Kamera-Cut
 * (Savedata v3 camera_cut = g_scd.cam_id beim Speichern, re15_savedata.c) auf einen waehlbaren
 * Cut steht — Standard Cut 2 (Blick auf Irons' Schreibtisch mit der Leuchtschrift "HEAVEN").
 * Damit startet die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg (Raumlader wie im Spiel, NICHT der Debug-Sprung) direkt in Cut 2.
 *
 * Spielerlage (-22250,0,-18500) wie probe_r30_sicherung_karte: liegt im Regionsviereck von Cut 2
 * (RVD-Satz 8 von ROOM1150 @RDT+0x1A0+8*20, rec[2] = 2: (-27600,-24800) (-27635,-9963)
 * (-18400,-16000) (-19300,-22100)) und ausserhalb des Hebetisch-Ausloesers (Aot_set slot 1
 * @Datei 0x0D7E). Kein Zonen-Uebergang aus Cut 2 heraus, solange die Figur steht.
 *
 * Aufruf: probe_r34n_g_karte <kartendatei> [<raum-hex> 1150|1151] [<cut>]
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1150u;
    int cut = (argc > 3) ? atoi(argv[3]) : 2;
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
    g_scd.cam_id = (uint8_t)cut;

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  BUERO") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    printf("Karte %s geschrieben: Slot 0, Raum 0x%04X, camera_cut %u (Ruecklesen %u)\n",
           path, room, (unsigned)sd.camera_cut, (unsigned)back.camera_cut);
    return (back.camera_cut == (uint8_t)cut) ? 0 : 1;
}
