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
 * Weitere Staende (Fortsetzung, Dossier §8.6 — MAIN01 von Raum zu Raum und der 1150-Latch am echten Weg):
 *   raus      Stand IN ROOM10F0 am Tuer-Spawn (8400,0,-350), Blick zur Tuer (Gierung 0 = +X; Vorwaerts-620-Punkt
 *             (9020,-350) liegt im Tuer-Rechteck Slot 0 @0x00F32 (8800,-1300,1000,2000)) -> Aktionstaste fuehrt
 *             zurueck nach ROOM10D0. Mit "gesehen" kombinieren.
 *   vor11c0   Stand in ROOM11B0 vor der Tuer zum Parkplatz (main00 @0x00FAA Door_aot_set Slot 1, Rechteck
 *             (-26730,-29940,2500,3500), Ziel-Byte 0x1c): Spieler (-25480,0,-26100) Gierung 1024 (= -Z), Vorwaerts-
 *             620-Punkt (-25480,-26720) im Rechteck. Flags (4,243)=1 Strom an (sonst macht sub01 @0x01216 die Tuer
 *             zum Text-Platz), (3,130)=1 Strom-Szene sub04 schon gesehen (sub01 @0x011AE). Mit "gesehen" kombinieren.
 *   in1150    Stand in ROOM1150 mit (3,94)=1 (erste Irons-Szene gesehen, main00 @0x00DE6) — der Raumaufbau am
 *             Lade-Weg setzt mit (9,71)=1 den Latch (9,72). Mit "gesehen" kombinieren.
 *
 * Nachbesserung 1 (Dossier §9):
 *   montage   (9,73) = 1  "Irons-Todesszene gesehen" (VERTRAG §1.1 Spur L) — mit "gesehen" ist damit das
 *             MAIN01-Fenster offen (re15_cut10f0.h RE15_CUT10F0_BGM_START_*).
 *   in11d0    Stand im Zwinger ROOM11D0 am Tuer-Spawn von ROOM1180 her (ROOM1180 main00 @0x00A12 Door_aot_set,
 *             Bytes 14..21 `d4 fe 00 00 08 bc 00 04` = (-300, 0, -17400)), Blick zur Tuer zurueck (Gierung 3072 =
 *             +Z; Vorwaerts-620-Punkt (-300,-16780) im Rechteck Slot 0 @0x011E2 (-1300,-17000,2000,1000), Ziel-
 *             Byte 0x18 solange (4,243)=0, main00 @0x011DE). Die fuenf Gegner des Raums tot: (7,209) (7,210)
 *             (7,221) (7,44) (7,45) = 1 -> sub01 @0x016E4-@0x0170C schickt im ersten Spielbild
 *             @0x01710 `54 00 02 00 00 00` (Sce_bgm_control Slot 0 op 2 = Stop). Mit "gesehen montage".
 *
 * Aufruf: probe_r35_cut10f0_karte <kartendatei> [gesehen] [montage] [in10f0|raus|vor11c0|in1150|in11d0]
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
    int gesehen = 0, in10f0 = 0, raus = 0, vor11c0 = 0, in1150 = 0, montage = 0, in11d0 = 0;
    for (int a = 2; a < argc; a++) {
        if (strcmp(argv[a], "gesehen") == 0)     gesehen = 1;
        else if (strcmp(argv[a], "in10f0") == 0) in10f0 = 1;   /* Stand IM Raum (alter Spielstand, Szene steht aus) */
        else if (strcmp(argv[a], "raus") == 0)    raus = 1;     /* IM Raum, Blick zur Tuer nach ROOM10D0 */
        else if (strcmp(argv[a], "vor11c0") == 0) vor11c0 = 1;  /* ROOM11B0 vor der Tuer zum Parkplatz */
        else if (strcmp(argv[a], "in1150") == 0)  in1150 = 1;   /* ROOM1150, erste Irons-Szene gesehen */
        else if (strcmp(argv[a], "montage") == 0) montage = 1;  /* (9,73) Irons-Todesszene gesehen (Spur L) */
        else if (strcmp(argv[a], "in11d0") == 0)  in11d0 = 1;   /* Zwinger ROOM11D0, alle fuenf Gegner tot */
        else { printf("FAIL: unbekanntes Argument '%s'\n", argv[a]); return 2; }
    }
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    const unsigned raum = (in10f0 || raus) ? RE15_CUT10F0_RAUM : vor11c0 ? 0x11B0u : in1150 ? 0x1150u
                        : in11d0 ? 0x11D0u : 0x10D0u;
    g_current_room_id = raum;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    if (raus)         { pl->x = RE15_CUT10F0_SPAWN_X; pl->y = 0; pl->z = RE15_CUT10F0_SPAWN_Z; pl->rot_y = 0; }
    else if (in10f0)  { pl->x = RE15_CUT10F0_SPAWN_X; pl->y = 0; pl->z = RE15_CUT10F0_SPAWN_Z; pl->rot_y = RE15_CUT10F0_SPAWN_DIR; }
    else if (vor11c0) { pl->x = -25480; pl->y = 0; pl->z = -26100; pl->rot_y = 1024; }
    else if (in1150)  { pl->x = -21000; pl->y = 0; pl->z = -20000; pl->rot_y = 0; }
    else if (in11d0)  { pl->x = -300; pl->y = 0; pl->z = -17400; pl->rot_y = 3072; }
    else              { pl->x = 1900; pl->y = 0; pl->z = -7000; pl->rot_y = 2048; }
    re15_game_flag_set(3, 50, 1);
    re15_game_flag_set(4, 247, 1);
    if (vor11c0) { re15_game_flag_set(4, 243, 1); re15_game_flag_set(3, 130, 1); }
    if (in1150)  re15_game_flag_set(3, 94, 1);
    if (gesehen) re15_game_flag_set(RE15_CUT10F0_GESEHEN_BANK, RE15_CUT10F0_GESEHEN_BIT, 1);
    if (montage) re15_game_flag_set(RE15_CUT10F0_BGM_START_BANK, RE15_CUT10F0_BGM_START_BIT, 1);
    if (in11d0) {
        static const uint8_t tot[] = { 209, 210, 221, 44, 45 };       /* ROOM11D0 sub01 @0x016E4..@0x01704 */
        for (unsigned i = 0; i < sizeof tot; i++) re15_game_flag_set(7, tot[i], 1);
    }

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
