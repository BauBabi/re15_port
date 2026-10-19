/*
 * RE1.5 Rebuilt — Runde 39 Spur R "hund1121": in Elzas Panikphase springt in ROOM1121 ein Hund durch das Fenster, durch das
 * bei Leon in ROOM1120 die Kraehe hereinfliegt.
 *
 * NUTZER-VORGABE (woertlich, analysis/befunde_runde39/AUFTRAG.md, Nachtrag 10):
 *   "Und dann möchte ich, das in ROOM 1121, dort wo bei Leon die Krähe durch das Fenster fliegt, beim Durchlauf ein Hunde
 *    durch das Fenster rein springt."
 * Lesart des Orchestrators (verbindlich): einmal, nur in der Panikphase (Bank-9-Bit 97, gesetzt und geloescht von Spur P),
 * am Fenster von Leons Kraehen-Ereignis, danach ein normaler Hunde-Gegner; Leon unveraendert.
 * Dossier mit Messungen und Belegen: analysis/befunde_runde39/R_hund1121.md.
 *
 * ⛔ PORT-EREIGNIS AUF NUTZERWUNSCH. ROOM1121 hat im Auslieferungsstand kein Fenster-Ereignis (ROOM1121.RDT == ROOM1120.RDT
 * bytegleich, main00 nur Tueren/Texte + drei Kraehen @0x00D0C/@0x00D20/@0x00D34; Runde 35 M §1). Die FORM jedes Teils ist
 * Original:
 *   Ausloeser   = Leons Band der Runde 35 (re15_fenster1120.h, AOT sce 3 sat 0x41, Satzform ROOM1050 @0x0C22).
 *   Hund        = RE2 Leon ROOM21A0 sub07 (der einzige Hunde-Sprung per Skript im RE2-Leon-Datenbestand, Zensus aller 495
 *                 RDTs): @0x020FE..0x0210A `34 02 01 00 / 34 03 0e 00 / 34 04 00 00 / 34 05 00 00` (Member_set +4=1,
 *                 +5=14, +6=0, +7=0), @0x0211E `02` (Evt_next), @0x02122 `34 05 01 00` (Member_set +7=1 = Freigabe).
 *                 Sub 14 des RE2-Hundemoduls EMD0G_MOD0.BIN (@0x80102C28, Phasen @0x80105514): Phase 0 WARTET
 *                 (@0x80102CD4-F4, kein +7-Schreiber), Phase 1 Anlauf 0x80101380(65,-320), 8 Bilder (@0x80101398),
 *                 Absprung 280 + SE 3 (@0x80102D68/@0x80102D74), Flug mit +40/Bild (@0x801014B8-C8) ohne Biss (+0x22C
 *                 @0x80102DC0), Landung SE 10 (@0x801015FC), dann Sub 2 = RUN (@0x80102E2C) = normaler Gegner.
 *                 Im reinen RE1.5-KI-Modus (kein RE2-Hirn fuer 0x20) dieselbe Rolle in RE1.5-Form: Spawn grid 0x40
 *                 (INIT @0x8010db88-98 -> Zustand 4 Sub 0 = warten), Freigabe grid 0x43 (Tor @0x801113e4-ec; ROOM1190
 *                 sub10 @0x027E6 `34 0c 43 00`), Sprung 0x80111398, Ausgang 0x201 @0x8011162c.
 *   Glas        = RE2 room1090 sub15 wie Leons Ereignis: Rumble @0x003A/@0x0040/@0x0046 + 13 Splitter (@0x0054..0x01D4,
 *                 Fenster 1), Knall Raumbank-Satz 0x21 (@0x0208/@0x0218); Abstaende Splitter -> Knall 1 -> Knall 2 =
 *                 Sleep 3 (@0x0204 `0a 03 00`) und Sleep 5 (@0x0214 `0a 05 00`).
 * PORT-WAHL (Dossier §3): die Lage des Hundes (Anflugslinie der Kraehe, Hoehe = Rahmen-Unterkante), der Augenblick des
 * Glasbruchs (der Hund erreicht die Glasebene, wie die Kraehe in room1090 und das Hindernis in ROOM21A0 im Moment des
 * Durchstossens brechen) und der Schatten erst diesseits der Wand.
 *
 * ABLAUF: Raumaufbau (scd_room_setup.c + platform/pc/main.c Boot-/CONTINUE-Weg) -> re15_hund1121_r39_install: ROOM1121,
 * (9,97) = 1, Zone-7-Bit 0xAE = 0 -> AOT-Slot 4 als AUTO-Ereignis 30 auf Leons Band. Spieler im Band -> AOT-Scan ->
 * scd_event_fire(30) -> Port-Programm: 5 x Sca_id_set (Band-1-Rueckwand frei) + Sce_em_set (Hund) + Set(7,0xAE,1) +
 * Aot_reset(4) + Evt_end. re15_hund1121_r39_tick
 * (game_step_common.c, vor der KI) faehrt die Skript-Schritte von ROOM21A0 sub07 und den Glasbruch.
 */
#ifndef RE15_HUND1121_R39_H
#define RE15_HUND1121_R39_H

#include <stdint.h>
#include "re15_actor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Elzas Variante von ROOM1120 (niedrigste Hex-Ziffer = Spielervariante, Raumlader FUN_800396fc `srl a0,a0,31` @0x800397e4 +
 * `addu` @0x800397ec). Leons ROOM1120 bleibt unberuehrt. */
#define RE15_R39R_RAUM            0x1121

/* Panikphase: VERTRAG Runde 39 §1.1 "P = 97 (Elza-Panikphase: gesetzt beim 1041-Ruf, geloescht bei Johns Rettung; Q und R
 * lesen es)". Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9]). Spur R liest das Bit nur. */
#define RE15_R39R_PANIK_BANK      9
#define RE15_R39R_PANIK_BIT       97

/* Port-Ereignis: < RE15_RDT_MAX_SUB_SCD (32, Schranke in scd_event_fire); in ROOM1121 benutzt nur 26 (Elza-Kraehe, Runde 36
 * L), game-weit 2..6, 13, 20..29 (grep RE15_*EREIGNIS in include/); ROOM1121 hat sub00/sub01. */
#define RE15_R39R_EREIGNIS        30

/* AOT-Slot 4 = derselbe Platz wie Leons Band in ROOM1120 (main00 belegt 0..3, Kamerazonen 48..63; die Elza-Kraehe der
 * Runde 36 braucht keinen Slot). Band = re15_fenster1120.h RE15_FENSTER_ZONE_* (x 3500..6650, z 4300..6400). */
#define RE15_R39R_SLOT            4

/* Gegner-Slot 4 (= Aktor 5): main00 spawnt 0..2 (@0x00D0C/@0x00D20/@0x00D34), die Elza-Kraehe Slot 3. */
#define RE15_R39R_EM              4

/* Tot-Bit des Hundes UND Einmal-Riegel, Zone 7 (= re15_em_status_zone() fuer Stage 1-3, @0x800420ec). Zensus ueber alle 144
 * RDTs STAGE1-3 (Sce_em_set pc[7] + Ck/Set Bank 7, scd_dump_room.py, 0 Abbrueche): 0xAE ist im Original frei; im Port
 * belegt sind 0x28-0x2B, 0x2E-0x31, 0x4B-0x4F, 0x53, 0x54, 0x5A, 0x5B, 0x5F (Montage, re15_irons_tod.h) und 0xAC
 * (Elza-Kraehe, re15_fenster1120.h). Das Programm setzt das Bit NACH dem Sce_em_set (Spawn-Tor @0x80042128-38), der Tod
 * setzt es noch einmal (Tod-Commit +0x1C6 @0x80042170) — in beiden Faellen gibt es kein zweites Ereignis. */
#define RE15_R39R_KILL            0xAE

/* Lage des Hundes (PORT-WAHL, Dossier §3): x/z/Richtung = Start der Fensterkraehe (re15_fenster1120.h: RE2 room1090 sub09
 * Kraehe 0 abgebildet -> (5400,-2500,12100) dir 0x0418), die Hoehe = Unterkante des Fensterrahmens aus der Rueckprojektion
 * von Cut 1 (Runde 35 M §1: Rahmen y -3942..-1827, Scheiben y -3049..-1953) — der Hund steht draussen auf der Bruestung.
 * pc[4] = 0 -> Bodenbezug +0x1BA = 0 (FUN_800420a0 @0x800421f8-0x80042210) = der Raumboden, auf dem er landet. */
#define RE15_R39R_X               5400
#define RE15_R39R_Y               (-1827)
#define RE15_R39R_Z               12100
#define RE15_R39R_DIR             0x0418

/* Glasebene = Rueckwand z 11200 (SCA-Zelle (-2700,11200,11350,2000), Runde 35 M §1). */
#define RE15_R39R_GLAS_Z          11200

/* Die Rueckwand in Fensterhoehe: ROOM1121.RDT SCA @0x2FC (Kopf `50 da 0e cb`, 5 Regionen zu je 39 Zellen), Region r
 * Index 15 = `56 2c d0 07 74 f5 c0 2b 01 ff 00 13` (x -2700 w 11350, z 11200 d 2000, u0 0xFF, floor 0x13 = Band 1) @0x003C8 /
 * @0x0059C / @0x00770 / @0x00944 / @0x00B18. Der Hund steht mit y -1827 in Band 1 (re15_collision_band_from_y =
 * -(y/0x708)) — ohne Freigabe schiebt ihn die Wandklemme FUN_8003b0a4 im Spawn-Bild auf z 11200 - Radius (gemessen 10682).
 * FORM der Freigabe: RE2 ROOM21A0 sub07 @0x020F6 `37 11 00 00` (Sca_id_set auf das Hindernis VOR dem Sprung) in der
 * RE1.5-Satzform von ROOM1130 sub02 @0x00AC8..@0x00AD8 `37 0r 0a 00` (je Region ein Satz, Handler LAB_8004175c: u0 :=
 * Wert, Bit 0 = fest). PORT-WAHL: wieder zu (u0 = Originalwert), sobald der Hund mit seinem Radius diesseits der Wand
 * ist — sonst stuende die ganze Rueckwand in Band 1 offen (x -2700..8650, nicht nur das Fenster). */
#define RE15_R39R_WAND_INDEX      15
#define RE15_R39R_WAND_REGIONEN   5

/* RE2-Skriptform ROOM21A0 sub07 / Hundemodul (s. Kopf). */
#define RE15_R39R_RE2_STATE       1      /* @0x020FE `34 02 01 00` (+4)                                         */
#define RE15_R39R_RE2_SUB         14     /* @0x02102 `34 03 0e 00` (+5); Spawn-Tabelle @0x80105458[6] = 0x0e    */
#define RE15_R39R_RE2_LOS         1      /* @0x02122 `34 05 01 00` (+7)                                         */
/* RE1.5-Form im reinen RE1.5-KI-Modus (s. Kopf). */
#define RE15_R39R_RE15_GRID       0x40   /* INIT @0x8010db88-98: grid 0x40 -> +0x4 = 4                         */
#define RE15_R39R_RE15_LOS        0x43   /* Tor @0x801113e4-ec; ROOM1190 sub10 @0x027E6 `34 0c 43 00`          */

/* Glas-Zeitlinie ab dem Bruch (RE2 room1090 sub15). */
#define RE15_R39R_T_KNALL1        3      /* @0x0204 `0a 03 00`                                                  */
#define RE15_R39R_T_KNALL2        8      /* + @0x0214 `0a 05 00`                                                */

/* Phasen (Pruefhaken). */
#define RE15_R39R_AUS             0      /* falscher Raum / keine Panik / schon gesprungen                      */
#define RE15_R39R_SCHARF          1      /* Slot 4 scharf, noch kein Hund                                       */
#define RE15_R39R_GESPAWNT        2      /* Programm hat den Hund gesetzt (INIT laeuft in diesem Bild)          */
#define RE15_R39R_WARTET          3      /* RE2 Sub 14 Phase 0 / RE1.5 Zustand 4 Sub 0                          */
#define RE15_R39R_SPRUNG          4      /* freigegeben                                                         */
#define RE15_R39R_FERTIG          5      /* Knall 2 vorbei und der Hund auf dem Boden                           */

/* HAKEN scd_room_setup.c (Ende des Installer-Blocks) und platform/pc/main.c (Boot-/CONTINUE-Weg). */
void re15_hund1121_r39_install(uint16_t room_id);
/* HAKEN scd_vm.c scd_event_fire: das Programm fuer (ROOM1121, Ereignis 30), solange scharf; sonst NULL. */
const uint8_t *re15_hund1121_r39_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN game_step_common.c (vor re15_enemy_ai_run_all, neben re15_fenster1120_tick). */
void re15_hund1121_r39_tick(void);
/* HAKEN platform/pc/main.c (Schattenschleife): 1 = kein Schatten (der Hund steht noch jenseits der Glasebene). */
int  re15_hund1121_r39_schatten_aus(const re15_actor_t *e);

/* Pruefhaken (Riegel). */
int  re15_hund1121_r39_phase(void);
int  re15_hund1121_r39_slot(void);              /* Aktor-Slot des Hundes, -1 = keiner                          */
int  re15_hund1121_r39_takt(void);              /* Bilder seit dem Spawn (-1 davor)                            */
int  re15_hund1121_r39_glas_takt(void);         /* Bild des Glasbruchs relativ zum Spawn (-1 = noch nicht)     */
int  re15_hund1121_r39_splitter(void);
int  re15_hund1121_r39_knalle(void);
int  re15_hund1121_r39_re2(void);               /* 1 = RE2-Form, 0 = RE1.5-Form (bei der Freigabe gewaehlt)    */
const uint8_t *re15_hund1121_r39_programm(int *out_len);

#ifdef __cplusplus
}
#endif

#endif /* RE15_HUND1121_R39_H */
