/* probe_r36_ziel1190_karte.c — MESS-WERKZEUG (kein add_test) fuer den exe-Riegel der Scheiben-Texte
 * (Runde 36 Spur H, analysis/befunde_runde36/H_ziel1190.md).
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand im Schiessstand, damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg (main.c Boot-/CONTINUE-Installer) als Leon in ROOM1190 bzw. als Elza in ROOM1191 startet —
 * Elza ueber den Charakter des Spielstands (re15_savedata.c `out->character` / Restore), also mit der
 * Elza-RDT wie im Spiel (Dateiindex + Elza-Bit @0x800397ec), nicht ueber einen Raumsprung.
 *
 * Aufruf: probe_r36_ziel1190_karte <kartendatei> <1190|1191> [strom]
 *   Spielerlage (-2800,0,-10900) Blick -x: Stand-Seite des Durchgangs am Tresenende (SCA r0[0] @0x01C1C
 *   endet bei z=-11504, Suedwand r0[1] @0x01C28 bei z=-10280) — von dort laeuft der Riegel zu den Scheiben.
 *   strom = Flag (4,243) gesetzt (sub01 @0x0237C: Knoepfe sce 5 statt sce 3).
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
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    unsigned room = (argc > 2) ? (unsigned)strtoul(argv[2], NULL, 16) : 0x1190u;
    int strom = 0;
    for (int a = 3; a < argc; a++) {
        if (strcmp(argv[a], "strom") == 0) strom = 1;
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    if (room != 0x1190u && room != 0x1191u) {
        printf("FAIL: Raum 0x%04X ist nicht der Schiessstand (1190/1191)\n", room);
        return 2;
    }

    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();
    g_current_room_id = (int)room;
    /* Charakter-Byte wie re15_gameflow_new_game: Cursor << 2 (@0x801016a4) — Elza = 4. */
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -2800; pl->y = 0; pl->z = -10900; pl->rot_y = 2048;
    if (strom) re15_game_flag_set(4, 243, 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, (room & 1u) ? "ELZA  SCHIESS" : "LEON  SCHIESS") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path);
        return 1;
    }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    g_gameflow.character = 0;
    re15_game_flag_set(4, 243, 0);
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Charakter %d, Flag(4,243)=%d\n", path, (unsigned)rr,
           (int)g_gameflow.character, re15_game_flag_get(4, 243) ? 1 : 0);
    if (rr != (uint16_t)room || (int)g_gameflow.character != ((room & 1u) ? 4 : 0) ||
        (re15_game_flag_get(4, 243) ? 1 : 0) != strom) {
        printf("FAIL: Ruecklesen\n");
        return 1;
    }
    return 0;
}
