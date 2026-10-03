/* probe_r35_cut1150_karte.c — MESS-WERKZEUG (kein add_test), Spur L Runde 35.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in ROOM1130 (Flur vor Irons' Buero) bzw.
 * ROOM1060 (Treppenhaus, Etage 0), damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet (Muster probe_r34n_d_karte.c). Der Spieler steht vor der Tuer (Vorwaerts-620-
 * Punkt im Tuer-Rechteck): ROOM1130 Slot 2 @0x008CE Rechteck (-7150,15350,1000,2000) -> Standplatz
 * (-5900,16350) Blick 2048 (-x); ROOM1060 Slot 2 @0x00D52 Rechteck (26600,24300,1000,2200) ->
 * Standplatz (26200,25400) Blick 0 (+x, die Tuer liegt oestlich des 1040-Spawns (26000,25300)). Die Flags kommen dann aus dem GELADENEN Spielstand.
 *
 * Aufruf: probe_r35_cut1150_karte <kartendatei> <raum-hex> [nach10f0] [ersteszene] [tot1140] [tot1070] [gesehen]
 *   nach10f0    (9,71)  10F0-Szene gesehen (Spur K)
 *   ersteszene  (3,94)  erste Irons-Szene (ROOM1150 sub08 @0x01110)
 *   tot1140     Zone-7-Bits 0xd3..0xd7 (alle 1140-Zombies tot)
 *   tot1070     Zone-7-Bits 0xc6..0xca (alle 1070-Zombies tot)
 *   gesehen     (9,73)  Todesszene gesehen
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
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1130u;
    int k = 0, e = 0, t1140 = 0, t1070 = 0, g = 0, tor = 0;
    for (int a = 3; a < argc; a++) {
        if (!strcmp(argv[a], "nach10f0"))        k = 1;
        else if (!strcmp(argv[a], "ersteszene")) e = 1;
        else if (!strcmp(argv[a], "tot1140"))    t1140 = 1;
        else if (!strcmp(argv[a], "tot1070"))    t1070 = 1;
        else if (!strcmp(argv[a], "gesehen"))    g = 1;
        else if (!strcmp(argv[a], "tor1040offen")) tor = 1;   /* Rolltor ROOM1040 schon offen: (4,5)=(4,4)=1 wie sub01 @0x0157A/@0x01586 */
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = (int)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Blick 2048 = -x (Vorwaerts-620-Punkt des Scans: fx = x + 620*cos(yaw), fz = z - 620*sin(yaw),
     * aot_common.c / test_r34n_d_adaruf.c vorwaerts_trifft; Gierung 0 = +x, 1024 = -z, 2048 = -x, 3072 = +z). */
    /* ROOM1060: der Tuer-Spawn aus ROOM1040 (Slot 2 @0x010D8) liegt bei (26000,25300) Blick 2048 = die Tuer ist
     * OESTLICH (+x); (27720,25400) lag in der Wand (CONTINUE-Lauf D: Kollision schob auf x=26432). Standplatz
     * 200 vor dem Spawn, Blick 0 = +x -> Vorwaertspunkt (26820,25400) im Rechteck (26600..27600, 24300..26500). */
    if (room == 0x1060u) { pl->x = 26200; pl->z = 25400; pl->rot_y = 0; }
    else                 { pl->x = -5900; pl->z = 16350; pl->rot_y = 2048; }
    pl->y = 0;
    if (k) re15_game_flag_set(9, 71, 1);
    if (e) re15_game_flag_set(3, 94, 1);
    if (g) re15_game_flag_set(9, 73, 1);
    if (t1140) for (int i = 0; i < 5; i++) re15_game_flag_set(7, (uint8_t)(0xd3 + i), 1);
    if (t1070) for (int i = 0; i < 5; i++) re15_game_flag_set(7, (uint8_t)(0xc6 + i), 1);
    if (tor) { re15_game_flag_set(4, 5, 1); re15_game_flag_set(4, 4, 1); }

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  IRONS ") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, (9,71)=%d, (3,94)=%d, (9,73)=%d, 1140-tot=%d, 1070-tot=%d\n", path, (unsigned)rr,
           re15_game_flag_get(9, 71), re15_game_flag_get(3, 94), re15_game_flag_get(9, 73),
           re15_game_flag_get(7, 0xd3), re15_game_flag_get(7, 0xc6));
    return (rr == (uint16_t)room && re15_game_flag_get(9, 71) == k && re15_game_flag_get(3, 94) == e) ? 0 : 1;
}
