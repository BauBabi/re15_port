/* probe_r37_feuerloescher_karte.c — MESS-WERKZEUG (kein add_test, Runde 37 Spur I): schreibt eine
 * PSX-Speicherkarte mit EINEM Spielstand an frei gewaehlter Stelle, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg dort startet (Muster probe_r36_schluessel_karte.c, dazu die Hoehe fuer ROOM1090/1091 Band 1).
 *
 * Aufruf: probe_r37_feuerloescher_karte <kartendatei> <raum-hex> <x> <z> <rot> [opt]...
 *   y:<n>              Spieler-Hoehe (ROOM1090/1091 Boden = -1800 = Band 1)
 *   flag:<bank>:<bit>  setzt ein Spielflag (bit dezimal oder 0x..), z.B. flag:3:0x81
 *   give:<id>:<n>      legt einen Gegenstand ins Inventar (re15_inv_grant), z.B. give:0x31:1
 *   cut:<n>            gespeicherter Kamera-Cut
 *   elza               Spielfigur Elza (g_gameflow.character = 4)
 * Inventar sonst = Einsatzausruestung (re15_inv_load_briefing).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_inventory.h"
#include "re15_gameflow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 6) {
        printf("Aufruf: %s <karte> <raum-hex> <x> <z> <rot> [y:n] [flag:b:n] [give:id:n] [cut:n] [elza]\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    re15_inv_load_briefing();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    pl->x = atoi(argv[3]); pl->z = atoi(argv[4]); pl->rot_y = (int16_t)atoi(argv[5]);
    for (int a = 6; a < argc; a++) {
        if (strncmp(argv[a], "y:", 2) == 0) {
            pl->y = atoi(argv[a] + 2);
        } else if (strncmp(argv[a], "flag:", 5) == 0) {
            char *e = NULL;
            long b = strtol(argv[a] + 5, &e, 0);
            long n = (e && *e == ':') ? strtol(e + 1, NULL, 0) : -1;
            if (b < 0 || b > 31 || n < 0 || n > 255) { printf("FAIL: '%s'\n", argv[a]); return 2; }
            re15_game_flag_set((uint8_t)b, (uint8_t)n, 1);
        } else if (strncmp(argv[a], "give:", 5) == 0) {
            char *e = NULL;
            long id = strtol(argv[a] + 5, &e, 0);
            long n = (e && *e == ':') ? strtol(e + 1, NULL, 0) : 1;
            if (re15_inv_grant((uint8_t)id, (uint8_t)n) != 0) { printf("FAIL: give 0x%02lX\n", id); return 2; }
        } else if (strncmp(argv[a], "cut:", 4) == 0) {
            g_scd.cam_id = (uint8_t)atoi(argv[a] + 4);
        } else if (strcmp(argv[a], "elza") == 0) {
            g_gameflow.character = 4;
        } else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  R37I") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    uint16_t rr = 0;
    if (re15_memcard_load(path, 0, &back) != 0 || re15_savedata_restore(&back, &rr) != 0) {
        printf("FAIL: Ruecklesen\n");
        return 1;
    }
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d,%d) rot %d, Figur %d, Cut %d\n", path, (unsigned)rr,
           (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y,
           (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y,
           (int)g_gameflow.character, (int)back.camera_cut);
    return rr == (uint16_t)room ? 0 : 1;
}
