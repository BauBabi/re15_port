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
 * Aufruf: probe_r34n_g_karte <kartendatei> [<raum-hex> 1150|1151] [<cut>] [p=<x>,<z>] [f=<b>:<bit>...]
 *
 * BAU G2 (Abnahme 6.6, andere Raeume mit Opcode 0x45): JEDER Raum; optional
 *     p=<x>,<z>        Spielerlage (Standard (-22250,-18500) wie oben)
 *     f=<bank>:<bit>   Flag im Spielstand (z.B. f=4:9 = ROOM3000-Zombie-Variante: main00 @0x14B6
 *                      setzt dann (5,0) := 0). Bank 5 ist raumlokal und wird beim Raumaufbau
 *                      geloescht - dafuer RE15_SET_FLAG_AT an der exe.
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
    if (room < 0x1000u || room > 0x7FFFu) {
        printf("FAIL: Raum 0x%04X unbekannt\n", room);
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
    for (int i = 4; i < argc; i++) {       /* BAU G2: p=<x>,<z> und f=<bank>:<bit> (Kopf) */
        int a = 0, b = 0;
        if (sscanf(argv[i], "p=%d,%d", &a, &b) == 2) { pl->x = a; pl->z = b; }
        else if (sscanf(argv[i], "f=%d:%d", &a, &b) == 2 && a >= 0 && a < 32 && b >= 0 && b < 256) {
            re15_game_flag_set((uint8_t)a, (uint8_t)b, 1);
            printf("Flag (%d,%d) = 1 im Spielstand\n", a, b);
        }
    }

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
