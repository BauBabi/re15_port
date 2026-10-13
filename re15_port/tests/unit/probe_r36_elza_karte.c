/* probe_r36_elza_karte.c — MESS-WERKZEUG (kein add_test), Spur L Runde 36.
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand (Elza: g_gameflow.character = 4 fuer xxx1-Raeume, das Charakter-Byte, aus dem
 * TITLE.BIN das Elza-Bit DAT_800ACA3C Bit 31 ableitet: @0x801024a8/@0x801024e4) in einem Raum an einer Stelle, damit
 * die echte exe ueber RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 am LADE-Weg startet (Muster
 * probe_r35_cut1150_karte.c). Die Flags kommen dann aus dem GELADENEN Spielstand.
 * Aufruf: probe_r36_elza_karte <kartendatei> <raum-hex> <x> <z> <gier> [bank:bit ...] [give:id]
 * give:id (Runde 37 Spur Q): Gegenstand ins INVENTAR — seit der Kistenregel (kistenregel_r37.c) gilt ein Aufnahme-Bit
 * wie (4,0xEC) nur mit dem Gegenstand im Inventar (RE2 Keep_Item_ck @0x800584F0 -> FUN_800696CC).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_gameflow.h"
#include "re15_inventory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 6) { printf("Aufruf: karte raum-hex x z gier [bank:bit ...]\n"); return 2; }
    const char *path = argv[1];
    unsigned room = (unsigned)strtoul(argv[2], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    g_current_room_id = (int)room;
    /* Figur aus der RDT-Variante (niedrigste Hex-Ziffer, FUN_800396fc @0x800397ec): xxx1 = Elza (4), sonst Leon (0) —
     * so liefert das Werkzeug auch den Leon-Gegenlauf (z.B. ROOM1150) an derselben Stelle. */
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[3]); pl->z = atoi(argv[4]); pl->rot_y = (int16_t)atoi(argv[5]); pl->y = 0;
    for (int a = 6; a < argc; a++) {
        if (strncmp(argv[a], "give:", 5) == 0) {           /* Runde 37 Spur Q */
            long id = strtol(argv[a] + 5, NULL, 0);
            if (id <= 0 || id > 255 || re15_inv_grant((uint8_t)id, 1) != 0) { printf("FAIL: '%s'\n", argv[a]); return 2; }
            continue;
        }
        char *c = strchr(argv[a], ':'); if (!c) { printf("FAIL: '%s'\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)strtol(argv[a], NULL, 0), (uint8_t)strtol(c + 1, NULL, 0), 1);
    }
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "ELZA  R36   ") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d (Elza=4), Lage (%d,%d)\n", path, (unsigned)rr, (int)g_gameflow.character,
           (int)pl->x, (int)pl->z);
    return (rr == (uint16_t)room && g_gameflow.character == ((room & 1u) ? 4 : 0)) ? 0 : 1;
}
