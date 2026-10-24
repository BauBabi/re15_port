/* probe_r41_zombiefresser_karte.c — MESS-WERKZEUG (kein add_test) fuer integration_r41_zombiefresser.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in einem Raum mit 0x84/0x85-Zombies, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet (Muster probe_r38_wasserzombie_karte.c + probe_r36_gegnerzahl_karte.c: Spielfigur, Band und
 * Story-Bits zusammen, die keines der beiden Werkzeuge allein kann).
 *
 * Aufruf: probe_r41_zombiefresser_karte <kartendatei> <raum-hex> <x> <z> <gierung> <band> <leon|elza> [bank:bit ...] [cam=<n>]
 *   z.B.  ... re15_card.mcr 2001 -21300 -10100 2048 1 elza 3:0x45     (Elza auf dem Oststeg, Spawn-Bit 3:0x45)
 *         ... re15_card.mcr 3000 -8700 -14800 2048 0 leon 4:9 4:0xC6  (Spawn-Bit 4:9, Marvin-Szene aus)
 * Spieler-Y = -Band * 1800 (Bandhoehe 0x708, @0x8001d7b8-d4).
 * cam=<n> (Nachbesserung 1): Kamera-Schnitt des Spielstands (re15_savedata_capture: camera_cut = g_scd.cam_id) — ein echter
 * Spielstand traegt die Kamera des Speichermoments, CONTINUE stellt sie wieder her (main.c s_resume_cut). Ohne cam= Kamera 0.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 8) {
        printf("Aufruf: %s <kartendatei> <raum-hex> <x> <z> <gierung> <band> <leon|elza> [bank:bit ...]\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    int band = (int)strtol(argv[6], NULL, 0);
    int elza = (strcmp(argv[7], "elza") == 0);
    if (elza != ((room & 1u) != 0)) { printf("FAIL: Elza <-> ROOMxxx1 passt nicht\n"); return 2; }
    if (band < 0 || band > 3) { printf("FAIL: Band %d\n", band); return 2; }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    g_gameflow.character = elza ? 4 : 0;          /* PLD-Index: Leon 0, Elza 4 (re15_gameflow.h) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = (int32_t)strtol(argv[3], NULL, 0);
    pl->z = (int32_t)strtol(argv[4], NULL, 0);
    pl->rot_y = (int16_t)strtol(argv[5], NULL, 0);
    pl->y = -1800 * band; pl->floor = (uint8_t)band;
    int gesetzt = 0;
    for (int a = 8; a < argc; a++) {
        char *p = argv[a];
        if (strncmp(p, "cam=", 4) == 0) { g_scd.cam_id = (uint8_t)strtol(p + 4, NULL, 0); continue; }
        long bank = strtol(p, &p, 0);
        if (*p != ':') { printf("FAIL: '%s' ist kein bank:bit\n", argv[a]); return 2; }
        long bit = strtol(p + 1, &p, 0);
        if (bank < 0 || bank > 31 || bit < 0 || bit > 255) { printf("FAIL: '%s' ausserhalb\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)bank, (uint8_t)bit, 1);
        gesetzt++;
    }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, elza ? "ELZA  FRESSER" : "LEON  FRESSER") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int zurueck = 0;
    for (int a = 8; a < argc; a++) {
        char *p = argv[a];
        if (strncmp(p, "cam=", 4) == 0) continue;
        long bank = strtol(p, &p, 0);
        long bit = strtol(p + 1, &p, 0);
        zurueck += re15_game_flag_get((uint8_t)bank, (uint8_t)bit) ? 1 : 0;
    }
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d,%d) Band %d Blick %d Figur %d, %d Bits gesetzt, %d nach dem Ruecklesen, Kamera %d\n",
           path, (unsigned)rr, (int)g_actors[0].x, (int)g_actors[0].y, (int)g_actors[0].z, (int)g_actors[0].floor,
           (int)g_actors[0].rot_y, (int)g_gameflow.character, gesetzt, zurueck, (int)back.camera_cut);
    return (rr == (uint16_t)room && zurueck == gesetzt) ? 0 : 1;
}
