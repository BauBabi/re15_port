/*
 * RE1.5 Rebuilt — Runde 39 Spur H "elza": drei Elza-Befunde (Dossier analysis/befunde_runde39/H_elza.md).
 *
 * PUNKT 1 (ROOM1031 sub13, "Hand nur einmal heben"):
 *   Ein Plc_motion schreibt das Kommandowort des Work-Entitys +0x4 := 4 (Handler @0x80041b90: `sb v1(=4),4(v0)`
 *   @0x80041bb0). Der Spieler-Dispatcher FUN_80031c44 indiziert damit die Tabelle @0x80073f90 (`lbu v1,-13736(v1)`
 *   @0x80031c8c, `sll v1,v1,2` @0x80031c94, `addiu at,at,16272` @0x80031ca4, `lw v0,0(at)` @0x80031cac, `jalr v0`
 *   @0x80031cb4): [4] = 0x80030660 (Plc-Executor). Der cmd-0-Handler (Eintritts-Pose W-Bank Clip 1 @0x80031c10-c24) und
 *   die Fall-0-Uebergabe der Idle-FSM (@0x80032074-b4) laufen ab dann NICHT mehr. Der Port bildet cmd 0 / Fall 0 als
 *   zweistufige Latch (player_common.c s_entry_pose_step) nach; faellt der erste Spieler-Tick nach dem Raumwechsel wegen
 *   RE15_PAUSE_PLAYER (Dispatcher-Gate `lw a0,g_pauseflags` @0x80031c54) erst in das Bild des SCD-Plc_motion, ueberschreibt
 *   die Latch den Szenen-Clip (gemessen ROOM1031: Plc_motion(0,11) @0x029A4 bei F451, Latch 1 -> 210, F452 Latch 2 -> 200;
 *   Clip 11 der Raumbank = 2-Bilder-Haltepose "Arm oben", Clip 0 = 20 Bilder, rueckwaerts @0x029BE/@0x029C2 = senken).
 *   re15_elza_r39_latch_hinfaellig: Kommandowort 4 => die ausstehende Latch ist hinfaellig.
 *
 * PUNKT 1b (ROOM1031 sub13, zweiter Hub nach dem Senken — gemessen NACH dem Latch-Fix): nach Plc_motion(0,0)+0x80
 *   (senken, F491-F510) folgt Cut_chg(6) @0x029CA + Plc_dest(0,9,0x20,..) @0x029CC. Der Port liess den Spieler in der
 *   Align-Phase seines Walkers (walk_fsm 1) STEHEN und startete den Lauf-Clip erst beim Uebergang in Phase 2 — der
 *   Plc_dest-Handler nullt aber +0x1c4 (@0x80041c4c), und der globale Advancer lief Clip 0 damit VORWAERTS weiter
 *   (ANIM_TRACE F521-F528: clip 0 cur 30..37 slot 10..17 = Arm steigt erneut), erst F529 Clip 5. Im Original setzen die
 *   Spieler-Walk-Subs (Tabelle @0x80073e30) ihren Clip SOFORT in Phase 0:
 *     WALK Sub 4 @0x80030af0: +0x8c := 0x4b @0x80030b44, Phase := 1 @0x80030b4c, +0x94 := 5 @0x80030b58,
 *                             +0x95 := 0 @0x80030b64, +0x8f := 7 @0x80030b6c; Phase 1 (Align, arc_test 0x15e
 *                             @0x80030b84) posiert/advanced per anim_set @0x80030bf8 (a3 = 0x200) JEDES Bild
 *     RUN  Sub 5 @0x80030d28: +0x8c := 0xc8 @0x80030d7c, +0x94 := 5 @0x80030d90, +0x95 := 0 @0x80030d9c,
 *                             +0x8f := 7 @0x80030da4; Phase 2: +0x94 := 0 @0x80030dd8, +0x95 := 0 @0x80030de0
 *     TURN Sub 9 @0x80031360: Phase := 1 @0x80031398, +0x94 := 5 @0x800313a4, +0x95 := 0 @0x800313b0,
 *                             +0x8f := 7 @0x800313b8; arc_test 0x60 @0x800313d4, Slew 0x60 @0x80031440,
 *                             anim_set @0x80031494 jedes Bild; Ankunft: Sub := 6 @0x800313ec, Flag @0x800313f8
 *   +0x94 = 5 = PLW-Paar Clip 5 (Bank @0x800acbc4/0x800acbc8) = Port-Sentinel 105 (RE15_MOTION_WALK, re15_to_re2.c).
 *   re15_elza_r39_plc_walk_init: Spieler, Modus 4/5/9, am Walker-Init (walk_fsm 0) Clip 5 / Bild 0 / Blende 7.
 *
 * PUNKT 3 (ROOM1091/1090, "Elza klettert am Loescher aufs Autowrack"):
 *   Die Aktions-Sonde FUN_8002d474 fragt per FUN_8002d2c0 jedes installierte Objekt ab: `andi v0,v0,0x101` @0x8002d358
 *   (Bit 0 setzt der Installer immer, `ori v0,v0,0x1` @0x80040998), Band-Gate Objekt +0x82 == Spieler-Band 0x800acad6
 *   @0x8002d364-70, Reichweite FUN_8002d1e8 (563 voraus, Radius 900, box-unabhaengig, @0x8002d204/@0x8002d224) und
 *   Box-Treffer FUN_8002da4c(., ., -112, Band): `lhu v0,6(a2)` @0x8002da8c (Halbbreite VORZEICHENLOS) + Rand @0x8002da94,
 *   `sll v0,v0,1` @0x8002da9c, `sltu v0,v0,a1` @0x8002daa0 — bei Halbbreite 0 (Null-Box) und Rand -112 trifft das genau
 *   aus schraeger Lage (|dx| >= 112 UND |dz| >= 112). Ein Null-Box-Prop auf dem SPIELERBAND ist damit kletterbar.
 *   Das Original haelt seine Null-Box-Requisiten vom Spielerband fern: ROOM1091 obj 2 @0x02202 / ROOM1090 @0x0237E
 *   (`2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff ...`: Band 2, y -2653 = Spielerband 1 am Wagen ueber Boden -1800),
 *   ROOM1010 @0x00930 (First Aid Spray `2d 00 00 00 01 00 0a 00 00 00 c8 00 c0 f9 ...`: Band 1, y -1600 = Spielerband 0)
 *   — Band = Spielerband(eigene Hoehe y) + 1. Zensus ueber alle 240 RDTs
 *   (tests/unit/test_r39_elza_klettern.c zensus, gemessen): 558 Null-Box-Records, davon 291 auf Spielerband + 1
 *   (haeufigster Abstand), 51 AUF dem Spielerband (ROOM1040 obj 1-3, ROOM1190 obj 7-11, ROOM4050 obj 4/5, ...) — das
 *   Original ist also nicht durchgaengig; diese Records bildet climb_common.c unveraendert ab. Der Port-Loescher
 *   (Runde 37 Spur I) trug fest Band 1, in ROOM1090/1091 = Spielerband (y -1800 -> -(y/0x708) = 1, re15_collision.c:186).
 *   re15_elza_r39_prop_band_ueber_boden(y) = Spielerband der Hoehe y + 1 (PORT-WAHL in der Form der Original-Records).
 *
 * PUNKT 2 (Karte direkt nach Roys Tagebuch): tagebuch_r38.c (Zeitpunkt der Anforderung, NUTZER-VORGABE; Form = RE2
 *   Opcode 0x84 @0x800591C4, s. Dossier).
 */
#ifndef RE15_ELZA_R39_H
#define RE15_ELZA_R39_H

#include <stdint.h>
#include "re15_actor.h"   /* re15_actor_t (Punkt 1b) */

/* Kommandowort des Spielers (+0x4 = DAT_800aca58), wie Plc_motion @0x80041bb0 es schreibt. */
#define RE15_R39H_KOMMANDOWORT_PLC 4

/* Punkt 1: 1 = die zweistufige Eintritts-Latch (cmd 0 / Fall 0) ist hinfaellig, weil das Kommandowort schon 4 ist. */
int re15_elza_r39_latch_hinfaellig(int state);

/* Punkt 1b: Sub-Init der Spieler-Walk-Subs 4/5/9 — Clip 5 der PLW-Bank (Sentinel 105) sofort, Bild 0, Blende 7
 * (@0x80030b58-6c / @0x80030d90-a4 / @0x800313a4-b8). Rueckgabe 1 = gesetzt (Spieler + Modus 4/5/9), sonst 0. */
int re15_elza_r39_plc_walk_init(re15_actor_t *a);

/* Punkt 3: Band eines Port-Props ohne Kollisionsbox an der Hoehe y: Spielerband(y) + 1
 * (Spielerband = -(y / 0x708), re15_collision_band_from_y; Original-Form ROOM1091 @0x02202 pc[4] = 2, ROOM1010 @0x00930
 * pc[4] = 1). */
int re15_elza_r39_prop_band_ueber_boden(int32_t y);

#endif /* RE15_ELZA_R39_H */
