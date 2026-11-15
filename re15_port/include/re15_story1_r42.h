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
/* Nur solange die Szene in 3000/3001 noch nicht lief (Pruefer B5: ein Altstand v0.8.30 jenseits von 3000 hat Bit 120 = 0 und kaeme
 * auf dem Rueckweg durch den Abschnitt in die Aufhol-Szene, obwohl der Begleiter oben schon bei den Polizisten war). Dieselbe
 * Bedingung, an der das Original-Skript den Begleiter in 11A0/11A1 setzt: sub00 @0x01188 `21 04 c6 00` = Ck(4,198) = 0, direkt vor
 * Sce_em_set @0x0118C. Gesetzt wird 4:198 nur in der Leichen-Szene: ROOM3000 sub03 @0x0166E `22 04 c6 01`, ROOM3001 sub03 @0x016A6
 * (Byte-Suche ueber alle RDTs: sonst nur Ck in 3000/3001 @0x01568 und 6031 @0x48B5A). */
#define RE15_S1_BANK_3000        4
#define RE15_S1_BIT_3000         198

/* ---- Inszenierung (PORT-WAHL, gemessen mit der Cut-3-Projektion RID @0x0C0, sd 208, und der SCA) --------------------------
 * Werkzeug: scratchpad story/s1/proj11a.py (Projektion), p2.py (Fuss/Kopf), gier.py (Richtungen). Figurenhoehe im Bild GEMESSEN
 * (Lauf s1/l3 Bild 4.0, Lineal mess_l3_4.png): Leon bei P Kopf 45 / Fuss 147, Marvin bei N Kopf 107 - die Projektion trifft das mit
 * 3000 Einheiten Kopfhoehe (Kopf 44 / 108); vorher mit 1750 angesetzt war der Start (4000,0,-2000) "unter dem Bildrand", im Bild
 * stand Marvin dort mit Kopf und Schultern unten in der Mitte (Lauf s1/l3 Bild 0.5). In Cut 3 liegt der Abschnitt nur als schmale
 * Oeffnung im Bild (alle Lagen x -1275..1875 / z -225..1700 bei Bildpunkt 174..195): zwei Figuren DARIN stuenden hintereinander.
 * Deshalb das Gespraech UEBER die Kante: der Spieler oben in der Oeffnung (P), der Begleiter unten im Gang (N, Band 0) - Kopf des
 * Begleiters auf Brusthoehe des Spielers.
 *   P (100,-1800,350)       Bildpunkt 172, Fuss 136, Tiefe 7354. NACHBESSERUNG 1 (Pruefer B3): der erste Platz (1100,500) lag auf
 *     der Sichtlinie der Pfeilerkante (Cut 3 Auge (5788,-3635) -> P schnitt z -225 bei x 1867, Ostwand @0x0824 ab x 1875: halb
 *     verdeckt). Jetzt schneidet die Linie z -225 bei x 921 (Mitte; Rand bei Radius 450: x 1306), 570 vor der Pfeilerkante.
 *     z 350 statt naeher an die Kante: Spieler-Radius 450 (@0x80073e94, re15_actor.h) haelt ihn von der Kante z -225 bei z >= 225;
 *     ein Plc_dest-Ziel davor kaeme nie an (Ankunft d < 100).
 *   N (-300,0,-1800)        Bildpunkt 112, Fuss 194, Kopf 108, Tiefe 7078 (links unten)
 *   Sherry S (900,0,-1250)  Bildpunkt 143, Fuss 208, zwischen John und der Kante. z <= -1250 wegen der NPC-KANTENSONDE (s. u.).
 *   Start Begleiter (6500,0,-900) Bildpunkt 451 = rechts ausserhalb; auf dem Weg nach N ist bei (5820,-810) der Kopf noch unter dem
 *     Kinobalken (224 > 216), ab (5140,-920) tritt er unten rechts ins Bild (Bildpunkt 294, Kopf 188) - aus der Richtung, aus der
 *     der Spieler kam (Osten). Start Sherry (7000,0,-1000) ebenso rechts ausserhalb.
 * NPC-KANTENSONDE (re15_npckante_r38.h): die Plc_dest-Geh-/Lauf-Subs 4/5 der NPCs 0x40..0x4d rufen nach jedem Schritt die Sonde
 *   FUN_8002d7d8 (@0x80051374 / @0x800516c4), 800 voraus auf dem GERASTETEN Kurs (rot+512)&~1023 @0x8002d82c-838. Die Kletterkante
 *   (Block @0x07E8, ab z -225) trifft sie, sobald ein NPC mit Kurs Nord bei z >= -1025 steht -> Kletter-Sub 0x12/0x16. Gemessen
 *   (Lauf s1/l2): der Aufstieg an DIESER Kante gelingt einem NPC nicht (Schub 1148 ab z -983 endet im Radius der Absprungzelle
 *   @0x0884, NPC-Schwerkraft FUN_8001bd60 holt ihn zurueck). Deshalb: jedes NPC-Ziel naeher an der Kante als z -1025 wird nur mit
 *   Kurs West/Ost angelaufen (|dx| > |dz|), und von dort weg erst nach einer Drehung (Sub 9 ruft die Sonde nicht).
 * UEBERGABE DER UHR (6b, Pruefer B4): Sherry geht unter Elza an H (100,0,-700) (von S aus Kurs West, 34 Grad), dreht sich zu ihr,
 *   Elza kniet sich an der Kante zu ihr hinunter und steht wieder auf - Form ROOM2040/2041 sub08 @0x0171E..@0x0173C:
 *   Work_set(1,0) `2e 01 00` / Plc_motion(1,11) `3f 01 0b 00` @0x01722 / Sleep 30 `09 0a 1e 00` @0x01726 ... Plc_motion(1,11)
 *   @0x01734 + Plc_flg(0,0x80) `43 00 80 00` @0x01738 (rueckwaerts = aufstehen) / Sleep 30 @0x0173C. Clip 11 aus PL00
 *   (op_plc_motion: pc[1] != 0 -> anim_use_pl00, ROOM1150 Knie-Clip). Die Uhr verlaesst das Inventar, wenn Elza kniet.
 * ABGANG (Konzept L3/E2 Punkt 4 "geht als Erster zur Leiter, Abblende"; NACHBESSERUNG 1, Pruefer B1/B2):
 *   1. letzte Zeile -> der Begleiter geht an den Fuss der Kante K (-300,0,-1200) (Kurs Nord, Ankunft d < 100 bei z ~ -1275 -> Sonde
 *      bis z -475, trifft nicht), Sherry folgt ihm und stellt sich hinter ihn an B (-400,0,-2200) ("right behind me"; Kurs West
 *      39 Grad). B liegt 940 hinter K: gemessen (Lauf s1r2/eu1, npckante.log F17380) blieb Sherry mit dem Ziel N (-300,-1800),
 *      528 hinter K, bei z -2020 stehen - 748 vor John, die NPC-Abstossung laesst sie nicht naeher; die Ankunft (d < 100) kam nie.
 *      Abgeblendet wird erst, wenn BEIDE angekommen sind (vorher drehte Sherry sich noch auf der Stelle, als es schwarz wurde).
 *   2. Abblende; im Schwarz verlassen sie den unteren Gang und stehen oben im Abschnitt (Sce_em_set Band 1, Kurs Nord 3072):
 *      Begleiter T (-600,-1801,1600) (Hoehe s. RE15_S1_OBEN_Y_NPC), Sherry TS (500,-1800,1300); die Kamera wechselt auf Cut 2 (RID @0x0A0, Auge (-338,-3666,19010)
 *      blickt durch den Abschnitt nach Sueden) - der Zeitsprung ist zugleich ein Schnitt.
 *   3. Einblende: man sieht sie in Cut 2 den Abschnitt hinauf auf die Kamera zu gehen, Richtung Leiter (Plc_dest Modus 4; der
 *      Gehbefehl kommt schon im Schwarz, beim Aufblenden sind sie unterwegs) - Begleiter bis Z (-600,-1801,10000), Sherry bis
 *      ZS (500,-1800,8500) hinter ihm; er wartet dort auf sie. Projektion Cut 2: Hoehe 34..43 px am Anfang, 59/70 px am Ziel.
 *      Erster Stand (Ziel z 6200/5400, Lauf s1r2/leon2, Pruefbild leon2_abgang.png) war zu kurz und zu klein: 1,7 s sichtbar,
 *      Marvin am Ende ~45 px und halb vor Leon. Gemessen: Marvin geht 88 je Bild (leon2 F788..F839), Sherry 50 (pruef_s1_r1).
 *   4. Beide angekommen -> Cut_chg 3; dort stehen sie hinter dem Pfeiler (Projektion Cut 3: Bildpunkt 294/301, die Sichtlinie
 *      schneidet z -225 bei x 4190/4302, Ostwand @0x0824 x 1875..5625) -> erst dann verlassen sie den Raum (re15_actor_free).
 *   Alle Lagen: PORT-WAHL (Inszenierung), gemessen mit proj11a.py (RID-Projektion, sd 208) gegen SCA/RID. */
#define RE15_S1_SPIELER_X        100
#define RE15_S1_SPIELER_Z        350
#define RE15_S1_NPC_START_X      6500
#define RE15_S1_NPC_START_Z      (-900)
#define RE15_S1_NPC_X            (-300)
#define RE15_S1_NPC_Z            (-1800)
#define RE15_S1_SHERRY_START_X   7000
#define RE15_S1_SHERRY_START_Z   (-1000)
#define RE15_S1_SHERRY_X         900
#define RE15_S1_SHERRY_Z         (-1250)
#define RE15_S1_UEBERGABE_X      100       /* H: Sherry unter Elza                                                             */
#define RE15_S1_UEBERGABE_Z      (-700)
#define RE15_S1_KANTE_X          (-300)    /* K: Fuss der Kante, Ziel des Begleiters vor der Abblende                            */
#define RE15_S1_KANTE_Z          (-1200)
#define RE15_S1_HINTER_X         (-400)    /* B: Sherry hinter dem Begleiter (940 hinter K)                                     */
#define RE15_S1_HINTER_Z         (-2200)
#define RE15_S1_OBEN_X           (-600)    /* T: Begleiter oben im Abschnitt nach dem Zeitsprung                                 */
#define RE15_S1_OBEN_Z           1600
#define RE15_S1_OBEN_SHERRY_X    500       /* TS (976 vom Spieler bei P: Abstossung Spieler/NPC aec4 greift nicht)              */
#define RE15_S1_OBEN_SHERRY_Z    1300
#define RE15_S1_ZIEL_X           (-600)    /* Z: Richtung Leiter (Door_aot_set Slot 1 @0x00F7C bei z 24500..25400)               */
#define RE15_S1_ZIEL_Z           10000
#define RE15_S1_ZIEL_SHERRY_X    500       /* ZS hinter ihm                                                                      */
#define RE15_S1_ZIEL_SHERRY_Z    8500
#define RE15_S1_BLICK_X          100       /* Blickpunkt des Spielers nach dem Zeitsprung: gerade nach Norden den Abschnitt hinauf */
#define RE15_S1_BLICK_Z          10000
#define RE15_S1_BAND_UNTEN       0         /* Sce_em_set pc[4] (Band), y = -Band * 1800 (Sce_em_set +0x1ba @0x800421d4)       */
#define RE15_S1_BAND_OBEN        1         /* Band des Abschnitts (Waende @0x0830/@0x0824 Band 1)                              */
/* Hoehe des Begleiters (Marvin/John) oben: -1801 statt -(Band * 1800) - PORT-WAHL gegen eine Eigenheit der Original-Schwerkraft.
 * GEMESSEN (Lauf s1r2/eo1 + unit szene_elza mit Messzeilen, 2026-11-15): John (Radius 450) erschien bei (-600,-1800,1600) Band 1
 * und fiel im ersten Bild: FUN_8001bd60 (trage_1200.c) fragt den Boden mit FUN_8003b7f0 und Rand -Radius ab; der Vergleich ist
 * vorzeichenlos (@0x8003b8b8-f8, re15_collision_floor_typeword): bei Zellen schmaler/flacher als 2 * 450 laeuft (Breite - 900) ueber,
 * und die Zelle trifft in dieser Achse UEBERALL. Im Abschnitt sind das die Absprungzellen @0x0884 (800 tief: trifft fuer
 * x -825..1425 = der ganze Gang abzueglich Radius) und @0x0878 (825 breit: trifft fuer z 250..5600). Wort & 2 -> Fall auf Band 0,
 * die Klemme setzt ihn an die Kante (z -693). Ein NPC mit Radius 450 kann im schmalen Abschnitt also nicht stehen (Sherry, kleiner,
 * schon: gemessen bis z 5330 auf Band 1). Der Fall BEGINNT nur bei y == -(Band * 1800) (@0x8001bdc8-ec); eine Einheit darueber
 * bleibt er aus, die Figur steht unsichtbar 1 Einheit hoeher. */
#define RE15_S1_OBEN_Y_NPC       (-1801)
/* Gier beim Erscheinen = Richtung zum Ziel, Konvention dir(Gier) = (cos, -sin) (0 = +X, 1024 = -Z, 3072 = +Z; dachkiste_r36.c
 * wp_yaw_zu): round(atan2(-dz, dx) / 2pi * 4096) & 4095 (gier.py). */
#define RE15_S1_GIER_NPC_START   1962      /* Start -> N */
#define RE15_S1_GIER_SHERRY_START 2021     /* Start Sherry -> S */
#define RE15_S1_GIER_NORD        3072      /* +Z: oben im Abschnitt Richtung Leiter */

#define RE15_S1_CUT_SZENE        3         /* RID @0x0C0 (s. o.)                                                               */
#define RE15_S1_CUT_ABGANG       2         /* RID @0x0A0: blickt von z 19010 durch den Abschnitt nach Sueden (s. o. ABGANG)   */
#define RE15_S1_NPC_SLOT         8         /* Skript-Slot wie RE15_S2_NPC_SLOT (Aktor 9); der Mod-Satz nutzt 0/1 */
#define RE15_S1_SHERRY_SLOT      9
#define RE15_S1_TYP_MARVIN       0x40      /* ROOM2000 @0x017B4 `44 01 40 40 ..` = RE15_S2_TYP_MARVIN                          */
#define RE15_S1_TYP_JOHN         0x49      /* ROOM2001 @0x0179E                                                                */
#define RE15_S1_TYP_SHERRY       0x4B      /* ROOM2001 @0x0180A                                                                */
#define RE15_S1_NPC_GRID         0x40      /* stehender NPC (ROOM2000/2001 pc[3] = 0x40; ROOM11A0 @0x0118C pc[3] = 0x40)      */

/* ---- Uhr (6b, K-08, F-26) --------------------------------------------------------------------------------------------- */
#define RE15_S1_UHR_ITEM         0x3F      /* ROOM2090 @0x00A12 pc[14] = RE15_S2_UHR_ITEM                                      */

/* ---- Abblende = Zeitsprung (Konzept L3 Punkt 4 / E2 Punkt 4). PORT-WAHL (Pruefer B6): die ZUSAMMENSETZUNG ist eine Wahl des Ports,
 * kein Original-Ablauf - die Disc hat keine Zeitsprung-Blende im Skript (alle SCD-Blenden 0x56/0x57 sind statische Pulse,
 * scd_vm.c op_fade_adjust; z. B. ROOM3080 @0x0081E..). Belegt sind nur die BAUSTEINE, jeweils unveraendert:
 *   aus:    FUN_800217b0(0x200, +0x1800, 7, 0) + FUN_800216ec(0, 0) @0x8001ca64-88 (die Abblende des Raumwechsels, Uebergangs-Stufe
 *           1 DAT_800b5359 @0x8001c9c8, menu_common.c re15_menu_fsm_tick): 6 gezeichnete Bilder bis Schwarz
 *   schwarz: FUN_800217b0(0x200, 0, 7, 0) + FUN_800216ec(0, 0x7fff) (statisches Schwarz wie beim Schliessen des Menues
 *           @0x80046620-50, menu_common.c close_phase - dort 2 Bilder; im Raumwechsel bleibt es schwarz, solange geladen wird)
 *   ein:    FUN_800217b0(0x200, -0x1800, 7, 0) + FUN_800216ec(0, 0) @0x8001cbb8-cc18 (Einblende nach dem Raumwechsel,
 *           room_common.c Zustand 3): 6 Bilder
 * Haltezeit des Schwarz: PORT-WAHL RE15_S1_BLENDE_HALTEN = 30 Spielbilder (~1 s; das Spiel laeuft mit 30 Bildern/s, gemessen vom
 * Pruefer: 6 + 2 + 6 Bilder = 0,47 s). 2 Bilder lasen sich als "Blinzeln" (Pruefer B1), der Zeitsprung muss als solcher lesbar
 * sein. Im Schwarz wechselt das Programm die Kamera und setzt die Begleiter oben in den Abschnitt (s. ABGANG); eingeblendet wird
 * erst nach der Haltezeit UND wenn das Programm fertig ist (Signal EIN). */
#define RE15_S1_BLENDE_KANAL     0         /* a0 = 0x200: Kanal 0 | abr 2 << 8                                                 */
#define RE15_S1_BLENDE_ABR       2
#define RE15_S1_BLENDE_MASKE     7
#define RE15_S1_BLENDE_AUS       0x1800
#define RE15_S1_BLENDE_EIN       (-0x1800)
#define RE15_S1_BLENDE_SCHWARZ   0x7fff
#define RE15_S1_BLENDE_HALTEN    30        /* PORT-WAHL (s. o.)                                                                */

/* ---- Signale Bank 5 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74). ROOM11A0/11A1-Skripte nutzen nur 5:0..7
 * (sub01 @0x011A8..@0x012C4, main00 @0x01040/@0x010A6). Wort 0 bleibt stehen, Wort 1 (0x20..0x3F, Plc_dest-Ankunft) loescht der
 * Takt JEDES Bild (scd_vm.c `g_game.flags[5][1] = 0`, FUN_8003ebf4): der Takt dieses Moduls (vor den Faeden) haelt jede Ankunft
 * deshalb in Wort 0 fest, damit zwei Figuren gleichzeitig laufen koennen. */
#define RE15_S1_SIG_FREI         0x14      /* Takt -> Programm: keine Zeile, keine Stimme (Form RE15_S2_SIG_FREI)            */
#define RE15_S1_SIG_ABBLENDE     0x15      /* Programm -> Takt: abblenden                                                      */
#define RE15_S1_SIG_HELL         0x16      /* Takt -> Programm: Einblende fertig                                               */
#define RE15_S1_SIG_UHR          0x17      /* Programm -> Takt: Uhr an Sherry (aus dem Inventar)                               */
#define RE15_S1_HALT_SPIELER     0x18      /* festgehaltene Ankunft Spieler / Begleiter / Sherry                                */
#define RE15_S1_HALT_NPC         0x19
#define RE15_S1_HALT_SHERRY      0x1A
#define RE15_S1_SIG_SCHWARZ      0x1B      /* Takt -> Programm: schwarz, Begleiter unten entfernt                              */
#define RE15_S1_SIG_EIN          0x1C      /* Programm -> Takt: oben gesetzt, Kamera gewechselt - einblenden                   */
#define RE15_S1_SIG_WEG          0x1D      /* Programm -> Takt: Begleiter (hinter dem Pfeiler in Cut 3) verlassen den Raum     */
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
