/* probe_r36_gegnerzahl_karte.c — MESS-WERKZEUG (kein add_test), Runde 36 Spur E.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet (Muster probe_r35_cut1150_karte.c) und der Spieler durch eine ECHTE Tuer in den
 * gemessenen Raum geht. Die Tot-Bits der Raum-Records kommen aus dem GELADENEN Spielstand.
 *
 * Aufruf: probe_r36_gegnerzahl_karte <kartendatei> <raum-hex> <x> <z> <gierung> [bank:bit[-bit] ...]
 *   z.B.  probe_r36_gegnerzahl_karte re15_card.mcr 1030 -26214 -3861 2048 7:0-19 7:20-34
 *         (ROOM1030 vor der Tuer Slot 2 @0x01CAA nach ROOM1040; 1030 leer; in 1040 Records 0..14 tot)
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
    if (argc < 6) {
        printf("Aufruf: %s <kartendatei> <raum-hex> <x> <z> <gierung> [bank:bit[-bit] ...]\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = (int32_t)strtol(argv[3], NULL, 0);
    pl->z = (int32_t)strtol(argv[4], NULL, 0);
    pl->rot_y = (int16_t)strtol(argv[5], NULL, 0);
    pl->y = 0;
    int gesetzt = 0;
    for (int a = 6; a < argc; a++) {
        char *p = argv[a];
        long bank = strtol(p, &p, 0);
        if (*p != ':') { printf("FAIL: '%s' ist kein bank:bit\n", argv[a]); return 2; }
        long b0 = strtol(p + 1, &p, 0), b1 = b0;
        if (*p == '-') b1 = strtol(p + 1, &p, 0);
        if (bank < 0 || bank > 31 || b0 < 0 || b1 > 255 || b1 < b0) { printf("FAIL: '%s' ausserhalb\n", argv[a]); return 2; }
        for (long b = b0; b <= b1; b++) { re15_game_flag_set((uint8_t)bank, (uint8_t)b, 1); gesetzt++; }
    }
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  GEGNER") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int zurueck = 0;
    for (int a = 6; a < argc; a++) {
        char *p = argv[a];
        long bank = strtol(p, &p, 0);
        long b0 = strtol(p + 1, &p, 0), b1 = b0;
        if (*p == '-') b1 = strtol(p + 1, &p, 0);
        for (long b = b0; b <= b1; b++) zurueck += re15_game_flag_get((uint8_t)bank, (uint8_t)b) ? 1 : 0;
    }
    printf("Karte %s: Raum 0x%04X, Spieler (%d,%d) Blick %d, %d Bits gesetzt, %d nach dem Ruecklesen\n", path,
           (unsigned)rr, (int)g_actors[0].x, (int)g_actors[0].z, (int)g_actors[0].rot_y, gesetzt, zurueck);
    return (rr == (uint16_t)room && zurueck == gesetzt) ? 0 : 1;
}
