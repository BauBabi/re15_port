/* probe_r35_cut10f0_karte.c — MESS-WERKZEUG (kein add_test), Spur K Runde 35.
 *
 * Schreibt eine PSX-Speicherkarte mit EINEM Spielstand im Flur ROOM10D0 direkt vor der Tuer zum
 * Communication Room (ROOM10D0 main00 @0x01174 Door_aot_set Slot 0, Rechteck (300,-8050,1000,2000);
 * der Spieler steht oestlich davor, Blick nach Westen = Gierung 2048, Vorwaerts-620-Punkt (1280,-7000)
 * liegt im Rechteck), damit die echte exe ueber
 *     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
 * am LADE-Weg startet und per Aktionstaste DURCH DIE TUER nach ROOM10F0 geht — der Abnahmeweg des
 * Auftrags (Spielstand + CONTINUE, echter Eintritt). Muster probe_r34n_d_karte.c.
 *
 * Flags im Stand:
 *   immer     (3,50) = 1  Kartenleser-Schloss der Tuer geoeffnet (ROOM10D0 main00 @0x01026 Ck(3,50)==0
 *                         legt sonst den Text-Platz "It's electronically locked" + Tastenfeld an;
 *                         gesetzt von sub01 @0x0152A `22 03 32 01` nach dem Code)
 *             (4,247) = 1 Marvins "Freeze!"-Szene in ROOM10D0 schon gesehen (sub21 @0x019EC) — der
 *                         Flur spawnt dann keinen Marvin mehr (main00 Else-Zweig @0x012A4)
 *   gesehen   (9,71) = 1  10F0-Szene schon gesehen (re15_cut10f0.h) — Gegenprobe "genau einmal"
 *
 * Aufruf: probe_r35_cut10f0_karte <kartendatei> [gesehen]
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_aot.h"
#include "re15_cut10f0.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : "re15_card.mcr";
    int gesehen = 0, in10f0 = 0;
    for (int a = 2; a < argc; a++) {
        if (strcmp(argv[a], "gesehen") == 0)     gesehen = 1;
        else if (strcmp(argv[a], "in10f0") == 0) in10f0 = 1;   /* Stand IM Raum (alter Spielstand, Szene steht aus) */
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    const unsigned raum = in10f0 ? RE15_CUT10F0_RAUM : 0x10D0u;
    g_current_room_id = raum;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    if (in10f0) { pl->x = RE15_CUT10F0_SPAWN_X; pl->y = 0; pl->z = RE15_CUT10F0_SPAWN_Z; pl->rot_y = RE15_CUT10F0_SPAWN_DIR; }
    else        { pl->x = 1900; pl->y = 0; pl->z = -7000; pl->rot_y = 2048; }
    re15_game_flag_set(3, 50, 1);
    re15_game_flag_set(4, 247, 1);
    if (gesehen) re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);

    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  CUT10F0") != 0) { printf("FAIL: Karte %s\n", path); return 1; }
    re15_savedata_t back;
    if (re15_memcard_load(path, 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    uint16_t rr = 0;
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    int g = re15_game_flag_get(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT);
    printf("Karte %s: Raum 0x%04X, (3,50)=%d, (4,247)=%d, (9,71)=%d\n", path, (unsigned)rr,
           re15_game_flag_get(3, 50), re15_game_flag_get(4, 247), g);
    return (rr == raum && re15_game_flag_get(3, 50) == 1 && g == gesehen) ? 0 : 1;
}
