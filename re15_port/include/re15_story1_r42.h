/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE1: Szene vor der Leiter in 11A0 (Leon, Marvin) und 11A1 (Elza, John,
 * Sherry, Rueckgabe der Taschenuhr). Code: engine/src/story1_r42.c, Texte: tools/r42_story/texte_stage1.py ->
 * engine/src/gen/story1_r42_texte.inc.
 *
 * PORT-STORY (Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, vom Nutzer am 08.11. freigegeben;
 * Korrekturen K-01..K-08 Abschnitt 0 bindend): 3.2 L3 + K-07 (Marvin steht nicht mehr in 11A0; beim Betreten des schmalen
 * Abschnitts vor der Leiter kommt er angelaufen, vier Zeilen, geht als Erster zur Leiter, Abblende, nur einmal), 4.2 E2 + K-07
 * (dasselbe mit John und Sherry in 11A1), 6b + K-08 + F-26 (Rueckgabe der Uhr in 11A1, nur wenn Elza die Uhr hat). 3000/3001
 * bleiben unveraendert (L2/K-07). Bits und Nachrichten-Ids NUR aus dem VERTRAG analysis/befunde_runde42/VERTRAG.md (Werte in
 * re15_story_r42.h): Bit 120 RE15_R42_BIT_LEITER, 11A0 msg 3..6, 11A1 msg 3..10 (6..9 nur mit Uhr).
 *
 * ---- MESSUNGEN (Belege; ROOM11A0/11A1 main00 bytegleich, scd_dump_room.py; SCA/RID/RVD aus dem RDT-Kopf +0x20/+0x24/+0x28) -------
 * Leiter: main00 Door_aot_set Slot 1 @0x00F7C `3b 01 02 31 01 00 76 fd b4 5f 14 05 84 03 d6 a4 00 00 c9 c6 00 00 02 00 ..` =
 *   Rechteck x -650..650 z 24500..25400 Band 1 -> Stage-Byte 2 Raum 00 = ROOM3000/3001 (STAGE3). Einziger Weg nach 3000.
 * Der schmale Abschnitt (SCA ROOM11A0 @0x77C, 5 gleiche Gruppen; Band = floor >> 4):
 *   Westwand @0x0830 Band 1 x -6875..-1275 z -225..16325, Ostwand @0x0824 Band 1 x 1875..5625 z -225..16350 -> Gang x -1275..1875
 *   (3150 breit), danach bei z 16350..24950 breiter bis zur Leiter (Waende @0x0800/@0x080C/@0x0818).
 *   Sein Boden liegt eine Ebene hoeher: Block @0x07E8 Band 0 `u0 ff u1 01 floor 01` x -2925..3325 z -225..25000 (Kletterkante,
 *   der Spieler steigt mit der Aktionstaste hinauf; Lauf s1/x2: Aktion bei (321,-810) -> (321,338) auf Band 1), davor die
 *   Absprungzelle @0x0884 Band 1 `u0 01 u1 02` x -1275..1875 z -1025..-225.
 *   Kameras (RID @0x60, 32 B je Cut, sd = fov >> 7 = 208): Cut 3 @0x0C0 Auge (5788,-4317,-3635) -> Ziel (-128,-2387,6) blickt vom
 *   unteren Gang auf die Oeffnung des Abschnitts; Cut 2 @0x0A0 blickt von z 19010 durch den Abschnitt nach Sueden. RVD @0x02D4
 *   "Cut 3 -> 2" x -2014..2713 z 1712..2736: ab z 1712 zeigt Cut 2 den Abschnitt.
 *   -> AUSLOESEZONE = der Anfang des Abschnitts, solange Cut 3 ihn zeigt: Band 1, x -1275..1875 (@0x0830/@0x0824), z -225 (Kante
 *   @0x07E8/@0x0884) .. 1712 (RVD @0x02D4). Wer hinaufsteigt, landet darin (Lauf s1/x2 z 338). Ausgeloest wird erst nach dem
 *   Klettern: Band 1 (re15_collision_debug_band()), Kletter-Sub aus (re15_climb_active() = 0) UND die Kletterbewegung zu Ende -
 *   gemessen (Lauf s1/l3, state.log): Landung F250 bei z 338, re15_climb_active() schon 0, die Bewegung 231 (Hochsteigen, PL00 Clip 2
 *   @0x80038134, re15_climb.h) laeuft bis F273; mit dem Riegel mittendrin blieb sie stehen und wiederholte sich (Bild 0.5/1.5).
 * Woher man kommt: aus dem unteren Gang von Osten - Tuer nach 2070 (Slot 0 @0x00F5A, Ankunft (6500,0,-3200)) bzw. vom Steg
 *   (Ankunft aus 20A0 (28150,-1800,1250), Absprung an der Stegkante @0x0860 z -1025..-225). Der Begleiter kommt deshalb im unteren
 *   Gang von Osten (Band 0) und steigt wie der Spieler die Kante hinauf (NPC-Kantensonde FUN_8002d7d8, Runde 38 npckante_r38.c:
 *   Kletter-Sub 0x12/0x16 @0x80052508, y -= 1800, Band + 1).
 * Mod-Inhalt: ROOM11A0 sub00 @0x0118C `44 00 40 40 01 ..` (Marvin, (-1600,-1800,23450)), ROOM11A1 sub00 @0x0118C `44 00 49 40 ..`
 *   (John) und @0x011A0 `44 01 4b 40 ..` (Sherry): auf der Disc und im 80-%-Material nicht belegt (Konzept Abschnitt 2); K-07 "In 11A0
 *   und 11A1 sind die Begleiter weiterhin nicht da" -> verworfen wie am Kill-Flag-Tor @0x80042120-38 (story2_r42.c spawn_weg).
 */
#ifndef RE15_STORY1_R42_H
#define RE15_STORY1_R42_H

#include <stdint.h>
#include "re15_story_r42.h"

#define RE15_S1_RAUM_11A0        0x11A0u   /* Raum-Basis & 0xFFFE; Elza = 11A1 (@0x800397ec Elza-Variante) */

/* ---- Ausloesezone (Belege oben) --------------------------------------------------------------------------------------- */
#define RE15_S1_ZONE_X0          (-1275)   /* SCA @0x0830 Westwand Band 1 (Ostkante)                                        */
#define RE15_S1_ZONE_X1          1875      /* SCA @0x0824 Ostwand Band 1 (Westkante)                                        */
#define RE15_S1_ZONE_Z0          (-225)    /* Kante: Block @0x07E8 / Absprungzelle @0x0884                                  */
#define RE15_S1_ZONE_Z1          1712      /* RVD @0x02D4 Cut 3 -> 2 beginnt bei z 1712                                     */
#define RE15_S1_ZONE_BAND        1

/* ---- Inszenierung (PORT-WAHL, gemessen mit der Cut-3-Projektion RID @0x0C0, sd 208, und der SCA) --------------------------
 * Werkzeug: scratchpad story/s1/proj11a.py (Projektion), p2.py (Fuss/Kopf), gier.py (Richtungen). Figurenhoehe im Bild GEMESSEN
 * (Lauf s1/l3 Bild 4.0, Lineal mess_l3_4.png): Leon bei P Kopf 45 / Fuss 147, Marvin bei N Kopf 107 - die Projektion trifft das mit
 * 3000 Einheiten Kopfhoehe (Kopf 44 / 108); vorher mit 1750 angesetzt war der Start (4000,0,-2000) "unter dem Bildrand", im Bild
 * stand Marvin dort mit Kopf und Schultern unten in der Mitte (Lauf s1/l3 Bild 0.5). In Cut 3 liegt der Abschnitt nur als schmale
 * Oeffnung im Bild (alle Lagen x -1275..1875 / z -225..1700 bei Bildpunkt 174..195): zwei Figuren DARIN stuenden hintereinander.
 * Deshalb das Gespraech UEBER die Kante: der Spieler oben in der Oeffnung (P, Ostseite, damit die Kletterstelle des Begleiters frei
 * bleibt), der Begleiter unten im Gang (N, Band 0) - Kopf des Begleiters auf Brusthoehe des Spielers.
 *   P (1100,-1800,500)      Bildpunkt 193, Fuss 144, Kopf 44, Tiefe 6609
 *   N (-300,0,-1800)        Bildpunkt 112, Fuss 194, Kopf 108, Tiefe 7078  (81 links, tiefer)
 *   Sherry S (800,0,-1300)  Bildpunkt 140, Fuss 207, zwischen John und der Kante
 *   Start Begleiter (6500,0,-900) Bildpunkt 451 = rechts ausserhalb; auf dem Weg nach N ist bei (5820,-810) der Kopf noch unter dem
 *     Kinobalken (224 > 216), ab (5140,-920) tritt er unten rechts ins Bild (Bildpunkt 294, Kopf 188) - aus der Richtung, aus der
 *     der Spieler kam (Osten). Start Sherry (7000,0,-1000) ebenso rechts ausserhalb.
 *   Abgang: von N gerade nach Norden zur Kletterkante (Richtung Leiter, Ziel L hinter der Kante); Sherry geht an Johns Platz N
 *     ("right behind me"). ABGANG GEMESSEN (Lauf s1/l2, RE15_NPCKANTE_LOG): Marvin geht von N nach Norden, die Sonde trifft bei
 *     z -983 (Sub 0x12), der Aufstieg setzt ihn bei Bild 0x1d auf Band 1 (z 165, y -1800) - im naechsten Bild greift die
 *     NPC-Schwerkraft FUN_8001bd60 (trage_1200.c): die Absprungzelle @0x0884 Band 1 (z -1025..-225, Typ 2) liegt mit dem
 *     Radius-Rand noch unter ihm -> Band 0, Fall, Wand-Klemme zurueck an die Kante (z -693). Der Aufstieg an DIESER Kante gelingt
 *     einem NPC nicht (Schub 1148 ab z -983 endet innerhalb Radius der Zelle). Die Szene blendet deshalb ab, sobald der
 *     Begleiter den Kletter-Sub beginnt (Konzept L3/E2 Punkt 4 "geht als Erster zur Leiter, Abblende"); im Schwarz verlaesst er
 *     den Raum. */
#define RE15_S1_SPIELER_X        1100
#define RE15_S1_SPIELER_Z        500
#define RE15_S1_NPC_START_X      6500
#define RE15_S1_NPC_START_Z      (-900)
#define RE15_S1_NPC_X            (-300)
#define RE15_S1_NPC_Z            (-1800)
#define RE15_S1_SHERRY_START_X   7000
#define RE15_S1_SHERRY_START_Z   (-1000)
#define RE15_S1_SHERRY_X         800
#define RE15_S1_SHERRY_Z         (-1300)
#define RE15_S1_LEITER_X         (-300)    /* Ziel L hinter der Kante (gerade nach Norden ab N, Kurs 3072)                    */
#define RE15_S1_LEITER_Z         3600
#define RE15_S1_BAND_UNTEN       0         /* Sce_em_set pc[4] (Band), y = -Band * 1800 (Sce_em_set +0x1ba @0x800421d4)       */
/* Gier beim Erscheinen = Richtung zum Ziel, Konvention dir(Gier) = (cos, -sin) (0 = +X, 1024 = -Z, 3072 = +Z; dachkiste_r36.c
 * wp_yaw_zu): round(atan2(-dz, dx) / 2pi * 4096) & 4095 (gier.py). */
#define RE15_S1_GIER_NPC_START   1962      /* Start -> N */
#define RE15_S1_GIER_SHERRY_START 2016     /* Start Sherry -> S */

#define RE15_S1_CUT_SZENE        3         /* RID @0x0C0 (s. o.)                                                               */
#define RE15_S1_NPC_SLOT         8         /* Skript-Slot wie RE15_S2_NPC_SLOT (Aktor 9); der Mod-Satz nutzt 0/1 */
#define RE15_S1_SHERRY_SLOT      9
#define RE15_S1_TYP_MARVIN       0x40      /* ROOM2000 @0x017B4 `44 01 40 40 ..` = RE15_S2_TYP_MARVIN                          */
#define RE15_S1_TYP_JOHN         0x49      /* ROOM2001 @0x0179E                                                                */
#define RE15_S1_TYP_SHERRY       0x4B      /* ROOM2001 @0x0180A                                                                */
#define RE15_S1_NPC_GRID         0x40      /* stehender NPC (ROOM2000/2001 pc[3] = 0x40; ROOM11A0 @0x0118C pc[3] = 0x40)      */

/* ---- Uhr (6b, K-08, F-26) --------------------------------------------------------------------------------------------- */
#define RE15_S1_UHR_ITEM         0x3F      /* ROOM2090 @0x00A12 pc[14] = RE15_S2_UHR_ITEM                                      */

/* ---- Abblende (Konzept L3 Punkt 4 / E2 Punkt 4) - die Blende des Raumwechsels, unveraendert:
 *   aus:    FUN_800217b0(0x200, +0x1800, 7, 0) + FUN_800216ec(0, 0) @0x8001ca64-88 (Uebergangs-Stufe 1 DAT_800b5359 @0x8001c9c8,
 *           menu_common.c re15_menu_fsm_tick)
 *   halten: FUN_800217b0(0x200, 0, 7, 0) + FUN_800216ec(0, 0x7fff), 2 Bilder @0x80046620-50 (menu_common.c close_phase)
 *   ein:    FUN_800217b0(0x200, -0x1800, 7, 0) + FUN_800216ec(0, 0) @0x8001cbb8-cc18 (room_common.c Zustand 3)
 *   Ausgeloest vom Takt, sobald der vorangehende Begleiter an der Kante den Kletter-Sub 0x12/0x16 beginnt (s. "Abgang").
 *   Waehrend Schwarz verlassen die Begleiter den Raum (re15_actor_free), dann Bit 120. Dauer aus den Bytes: 6 Bilder aus,
 *   2 halten, 6 ein (room_common.c: 0x7fff..0x07ff = 6 gezeichnete Bilder). */
#define RE15_S1_BLENDE_KANAL     0         /* a0 = 0x200: Kanal 0 | abr 2 << 8                                                 */
#define RE15_S1_BLENDE_ABR       2
#define RE15_S1_BLENDE_MASKE     7
#define RE15_S1_BLENDE_AUS       0x1800
#define RE15_S1_BLENDE_EIN       (-0x1800)
#define RE15_S1_BLENDE_SCHWARZ   0x7fff
#define RE15_S1_BLENDE_HALTEN    2

/* ---- Signale Bank 5 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74). ROOM11A0/11A1-Skripte nutzen nur 5:0..7
 * (sub01 @0x011A8..@0x012C4, main00 @0x01040/@0x010A6). Wort 0 bleibt stehen, Wort 1 (0x20..0x3F, Plc_dest-Ankunft) loescht der
 * Takt JEDES Bild (scd_vm.c `g_game.flags[5][1] = 0`, FUN_8003ebf4): der Takt dieses Moduls (vor den Faeden) haelt jede Ankunft
 * deshalb in Wort 0 fest, damit zwei Figuren gleichzeitig laufen koennen. */
#define RE15_S1_SIG_FREI         0x14      /* Takt -> Programm: keine Zeile, keine Stimme (Form RE15_S2_SIG_FREI)            */
#define RE15_S1_SIG_ABBLENDE     0x15      /* Programm -> Takt: abblenden, sobald der Begleiter an der Kante ist; dann weg     */
#define RE15_S1_SIG_HELL         0x16      /* Takt -> Programm: Blende fertig                                                  */
#define RE15_S1_SIG_UHR          0x17      /* Programm -> Takt: Uhr an Sherry (aus dem Inventar)                               */
#define RE15_S1_HALT_SPIELER     0x18      /* festgehaltene Ankunft Spieler / Begleiter / Sherry                                */
#define RE15_S1_HALT_NPC         0x19
#define RE15_S1_HALT_SHERRY      0x1A
#define RE15_S1_AN_SPIELER       0x20      /* Plc_dest-Ankunftsbit (Form ROOM2040 sub02 @0x01648 `40 00 09 20`)                */
#define RE15_S1_AN_NPC           0x21      /* (Form ROOM6030 sub08 @0x01160 `40 00 09 21`)                                     */
#define RE15_S1_AN_SHERRY        0x22      /* (Form ROOM1021 sub02 @0x020B2 `40 00 04 22`)                                     */

/* ---- Haken ------------------------------------------------------------------------------------------------------------ */
void re15_story1_r42_install(uint16_t room_id);   /* scd_room_setup.c, nach re15_story2_r42_install                            */
void re15_story1_r42_tick(void);                  /* scd_vm.c scd_vm_tick, nach re15_story2_r42_tick                            */
int  re15_story1_r42_szene_laeuft(void);          /* 1 = Programm laeuft (story2_r42.c spawn_weg laesst DESSEN Spawns durch)   */

/* Pruefhaken / Messzeilen */
const uint8_t *re15_story1_r42_text(uint16_t room_id, uint8_t id, int *out_len);
const uint8_t *re15_story1_r42_programm(int fassung, int *out_len);   /* 0 Leon, 1 Elza ohne Uhr, 2 Elza mit Uhr */
extern unsigned g_re15_s1_zaehler[4];  /* 0 Szenen-Start, 1 Uhr uebergeben, 2 Abblende, 3 Begleiter entfernt */

#endif /* RE15_STORY1_R42_H */
