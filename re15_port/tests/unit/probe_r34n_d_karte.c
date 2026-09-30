/* probe_r34n_d_karte.c — MESS-WERKZEUG (kein add_test), Spur D Runde 34 Nacht, Gegenpruefung Auflage 8a.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand in Irons' Buero ROOM1150 (einer echten
 * Speicherstelle, re15_savepoint.c:47), damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet — Muster probe_r30_granate_karte.c. Von dort geht der Messlauf per
 * RE15_DEBUG_JUMP in die Umkleide ROOM1000 und durch die Tuer in den 1050-Sued: die Flags
 * kommen dann aus dem GELADENEN Spielstand, nicht aus RE15_SET_FLAG.
 *
 * Aufruf: probe_r34n_d_karte <kartendatei> [gesehen] [gerettet]
 *   immer     Flag (3,121) Rolltor offen (ROOM1050 sub02 @0x0CBA `22 03 79 01`)
 *   gesehen   Flag (9,65)  Ada-Ruf-Szene gesehen (re15_adaruf.h)
 *   gerettet  Flag (3,187) Ada gerettet (ROOM1090 sub03 @0x024D2 `22 03 bb 01`)
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_adaruf.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    int gesehen = 0, gerettet = 0;
    for (int a = 2; a < argc; a++) {
        if (strcmp(argv[a], "gesehen") == 0)       gesehen = 1;
        else if (strcmp(argv[a], "gerettet") == 0) gerettet = 1;
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = 0x1150;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22250; pl->y = 0; pl->z = -18500; pl->rot_y = 0;   /* Lage wie probe_r30_granate_karte */
    re15_game_flag_set(3, 121, 1);
    if (gesehen)  re15_game_flag_set(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT, 1);
    if (gerettet) re15_game_flag_set(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT, 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  ADARUF") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    /* Gegenprobe: zurueckgelesen muessen Raum und Flags stimmen. */
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int g = re15_game_flag_get(RE15_ADARUF_GESEHEN_BANK, RE15_ADARUF_GESEHEN_BIT);
    int f = re15_game_flag_get(RE15_ADARUF_FREI_BANK, RE15_ADARUF_FREI_BIT);
    printf("Karte %s: Raum 0x%04X, (3,121)=%d, (9,65)=%d, (3,187)=%d\n", path, (unsigned)rr,
           re15_game_flag_get(3, 121), g, f);
    return (rr == 0x1150 && g == gesehen && f == gerettet) ? 0 : 1;
}
