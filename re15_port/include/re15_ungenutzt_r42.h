/*
 * RE1.5 Rebuilt — Runde 42 Strang "rest", Stufe 3 "Ungenutzte Inhalte STAGE2-6": Capcom-/80-%-Inhalte, die der Port nie zeigte.
 * Code engine/src/ungenutzt_r42.c, Texte tools/r42_story/texte_ungenutzt.py -> engine/src/gen/ungenutzt_r42_texte.inc.
 * U-07 (Sherry 2011) steht in story2_r42.c k_prog_elza, U-03 (6031 Blick auf Marvin) in tools/r42_story/szenen_stage6.py.
 *
 * AUFTRAG (Orchestrator Strang rest, Stufe 3/3 nach scratchpad ungenutzt/VORSCHLAEGE.md mit PRUEFUNG.md, die Pruefung hat Vorrang):
 * Reihenfolge U-07, U-05, U-01, U-02, U-03, U-11, U-12, U-04, U-13, U-18. "Disc" = Version_40%/PSX (echte Capcom-Disc), "Mod" =
 * shared_assets/PSX (den spielt der Port). Kein Asset-Patch: die RDTs bleiben byte-true, die Programme sind Original-Befehle.
 *
 * ---- U-05 (501 Rechner) ----------------------------------------------------------------------------------------------------
 * Disc ROOM5010/5011 main00 @0x372 `2c 04 01 31 .. 28 00 ff ff` = Aot_set Slot 4 sce 1 Text 0x28 = #40 「少し前まで使われていたようだ」,
 *   kein Aot_reset in beiden Disc-Dateien (Pruefung U-05) -> bei JEDEM Untersuchen.
 * Mod 5010 @0x3E2 `2c 03 00 31` (sce 0 = tot, Text 2 "It looks like this was used a little while ago."), Mod 5011 @0x3F6
 *   `2c 04 00 31` (tot, Text 40, derselbe Satz). Im selben Rechteck (-20327,-21250,1600,2700) liegt der Speicherrechner der Mod:
 *   5010 @0x45E `2c 09 03 31 .. ff 00 18 03` -> sub03 @0x4F6 `29 02 | 2b 06 ff ff | 02 00 | 2a 00 | 3c 01 | 01 00`,
 *   5011 @0x3BA `2c 01 03 31 .. ff 00 18 08` -> sub08 @0xE34 `29 02 | 2b 2d ff ff | 02 00 | 2a 00 | 3c 01 | 01 00`
 *   (msg 6 / 45 = Speicher-Text, re15_savepoint.c {0x5010,0x06}/{0x5011,0x2D}).
 * BAU: das Rechner-Ereignis bekommt den Disc-Satz VOR dem Speicher-Text, im Nahbild Cut 2 des Rechners: sub03/sub08 Byte fuer
 *   Byte, nach dem Cut_chg 2 zusaetzlich `2b mm ff ff 02 00` (Message_on + Evt_next in der Form desselben Subs @0x4F8/@0xE36;
 *   Maske ffff wie der Disc-Satz @0x372 pc[16..17]). PORT-WAHL: Nahbild statt Raumkamera (Disc sce 1 schaltet keine Kamera) -
 *   der Satz spricht vom Rechner, den Cut 2 zeigt; der Speicher-Ablauf danach ist unveraendert.
 *
 * ---- U-01 (3091 Geraeusch an der Hoftuer, D309 #21/#24) -------------------------------------------------------------------------
 * Disc ROOM3091 sub07: viermal `36 02 0d 01 00 00 46 a0 00 00 dc 05` @0x1778/@0x1788/@0x1798/@0x17A8 = Ton 0x0D am Weltort
 *   (-24506, 0, 1500) (Se_on-Handler LAB_80041624, Modus 0 -> Ursprung (0,0,0) @0x80041678..@0x80041684, Pruefung U-01); der Ort
 *   liegt in der Tuer Slot 1 @0x133C nach 306 (Hof). Danach #21 John (@0x1B4E) und #24 Elza (@0x1BD6), im Port nie gezeigt.
 * Mod ROOM3091 sub07 [@0x16CA, @0x1DB2) spielt dieselben Schlaege @0x1A62..@0x1A92, dreht aber danach beide Figuren zur Lobby-Tuer
 *   Slot 0 (-28481,-155): Plc_neck @0x1AA6 / Plc_dest @0x1AB0 (Elza, Work 2,1), Plc_neck @0x1AC0 / Plc_dest @0x1ACA (John, Work 2,0).
 * BAU (Pruefung U-01 "Mod-Blickrichtung @0x1AA6-@0x1ACA mit aendern"): Kopie von sub07 mit
 *   (a) Blick-/Drehziel dieser vier Saetze = Ort der Schlaege (-24506,1500) (Disc @0x1778 `46 a0 .. dc 05`);
 *   (b) John msg 33 (= D309 #21) VOR Elzas msg 25 @0x1BC2 "Let me deal with it..." (Johns Drang zum Aufbruch, Elza antwortet mit
 *       der Aufteilung - wie auf der Disc #21 -> #22 Elza "teilen wir uns auf"), Gestenblock = Johns Block nach msg 23
 *       [@0x1B2A, @0x1B5A) (Bewegung 4/4/0/0/8);
 *   (c) Elza msg 34 (= D309 #24) NACH Sherrys msg 28 "Yes sir." vor @0x1C64 (Disc: #23 John "Sherry, du kommst mit mir" -> #24 Elza;
 *       Mod #27 John "You stay with me now" / #28 Sherry), Gestenblock = Elzas Block nach msg 25 [@0x1BCA, @0x1BEE) (5/5/2/9).
 *   (d) Runde 2 (Pruefung s3_r1 G2): John msg 36 (= D309 #25 @0x1BEF 「任せとけ！」, auf der Disc die Antwort auf #24) direkt nach
 *       msg 34, Gestenblock = Johns Block nach msg 26 "Will do!" [@0x1BF6, @0x1C0A) (3/3). Englisch PORT-WAHL "Leave it to me!".
 *   NICHT geaendert (Pruefung s3_r1 G1, offen): der Schluss @0x1D5E `40 00 09 20 bf 90 65 ff` dreht Elza zur Lobby-Tuer, und
 *   @0x1DAE `47 00` (Aot_on Slot 0) feuert genau diese Tuer (S198 -> 30C1, Marvin in der Lobby, ROOM30C1 sub03). Elzas weiterer
 *   Weg fuehrt in der Mod zwingend durch die Lobby; sie zur Hoftuer zu drehen und dann in die Lobby zu schicken waere schlechter.
 *   Jede neue Zeile wartet danach, bis Zeile UND Stimme zu Ende sind (Form story3b_r42.c OP_ZEILE: Signal Bank 5 0x14, RE2 Wsleep
 *   ROOM7020 sub12 @0x079A); 3091 nutzt in Bank 5 nur 0x20..0x22 (sub09..sub11 @0x1DCA/@0x1DD8/@0x1DE6).
 *   Work_set (2,0) = John (sub00 @0x014E0 Sce_em_set Slot 0 Typ 0x49), (2,1) = Elza (@0x014F4 Slot 1).
 *
 * ---- U-02 (309 "There's a power supply.", 80 % T25) -------------------------------------------------------------------------
 * 80 %: KATALOG_80.md K071/T25, Video A 0:40.84 = Foto A#051: Sherry allein in 309 "an der Kiste beim Kasten", 「電げんがある」.
 * Stelle am Bild (Messbild scratchpad rest/s3/bild/MB_u02_a051_3091*.png): das Foto aehnelt der warm beleuchteten hinteren Ecke
 *   (Cut 6/14: Kisten links, Tuer hinten, Wandgeraet rechts). Berichtigt (Pruefung s3_r1 G7): die Schiebekiste obj 0 (sub00
 *   @0x01448, z ~ -8024) und der Schalter-Platz Slot 5 (main00 @0x013EC, Rechteck (-22900,-4750,500,1000), im gruenen Gang Cut 1/2)
 *   liegen rund 3800 auseinander. PORT-WAHL Stelle: der Satz kommt am Schalter-Platz Slot 5, wo "I can't reach..." steht.
 * Mod-Ablauf: Slot 5 sce 5 -> V[0] = 5 -> sub01 @0x01542 Cmp V[0]==5 -> Evt_exec sub04 @0x015CA: Strom an (Ck(4,8)=1) msg 2, sonst
 *   msg 3 "I can't reach the breaker switch..." (Sherry steht unten; auf der Kiste ist es Slot 6 floor 1 -> sub05 Schalter).
 * BAU: sub04 als zwei Ifel-Bloecke (Form ROOM3091 sub06 @0x016B4 `06 00 10 00`): Ck(4,8)=1 -> msg 2 wie bisher; Ck(4,8)=0 -> erst
 *   msg 35 "There's a power supply." (System-Text, Form msg 3), dann msg 3. Nur ohne Strom, also nur Sherry allein (der Strom geht
 *   in sub05 @0x016AA an, bevor Elza und John kommen).
 *
 * ---- U-11 (402 Wandplan -> Kartenblatt 8) -----------------------------------------------------------------------------------
 * Mod ROOM4020/4021 main00 @0x073C Aot_set Slot 8 sce 3 -> Ereignis 11 = sub11 @0x0B06 `29 03 | 2b 00 ff ff | 02 00 | 2a 00 | 3c 01
 *   | 01 00` (Kamera 3, msg 0 "There's a map of the entire area."), ohne Wirkung. Blatt 8 ("Laboratory B1", menu_common.c ORDER)
 *   setzt EXE @0x8004BDB4 `ori v1,zero,0x8`; der Port vergibt es nirgends (re15_map_owned.c: nur 5030 -> 9..11).
 * BAU: sub11 + nach dem Text einmalig Set Bank-9-Bit 36 (RE15_R42_BIT_PLAN_FORSCHUNG, VERTRAG "Wandplan 4010/4011" - Ort jetzt
 *   402, VORSCHLAEGE U-11) und msg 2 = Bytes von ROOM5030 msg 1 "You've taken the laboratory map." - Form des einzigen Kartenfunds
 *   ROOM5030 sub00 @0x1086 `21 03 73 00` (nur ohne Bit) / sub02 @0x10C8 `22 03 73 01` Set + @0x10CC `2b 01 ff ff` Message_on.
 *   PORT-WAHL: ohne Ja/Nein-Frage (der 402-Text fragt nicht).
 */
#ifndef RE15_UNGENUTZT_R42_H
#define RE15_UNGENUTZT_R42_H

#include <stdint.h>
#include "re15_story_r42.h"

/* ---- U-05 --------------------------------------------------------------------------------------------------------------- */
#define RE15_U42_RAUM_5010        0x5010u
#define RE15_U42_RAUM_5011        0x5011u
#define RE15_U42_5010_EREIGNIS    3        /* Slot 9 @0x45E `ff 00 18 03` -> sub03 @0x4F6                                   */
#define RE15_U42_5011_EREIGNIS    8        /* Slot 1 @0x3BA `ff 00 18 08` -> sub08 @0xE34                                   */
#define RE15_U42_5010_SUB_OFF     0x04F6
#define RE15_U42_5011_SUB_OFF     0x0E34
#define RE15_U42_5010_SATZ        2        /* Mod 5010 msg 2 (Slot 3 @0x3E2, tot) = Disc #40                                 */
#define RE15_U42_5011_SATZ        40       /* Mod 5011 msg 40 (Slot 4 @0x3F6, tot) = Disc #40 @0x372                          */
#define RE15_U42_5010_SPEICHER    6        /* sub03 @0x4F8 `2b 06 ff ff`                                                     */
#define RE15_U42_5011_SPEICHER    0x2D     /* sub08 @0xE36 `2b 2d ff ff`                                                     */

/* ---- U-01 / U-02 (ROOM3091) ------------------------------------------------------------------------------------------- */
#define RE15_U42_RAUM_3091        0x3091u
#define RE15_U42_3091_SUB_SZENE   7        /* sub02 @0x015A4 Evt_exec 7                                                      */
#define RE15_U42_3091_SUB_SCHALTER 4       /* sub01 @0x01548 Evt_exec 4                                                      */
#define RE15_U42_SUB07_OFF        0x16CA
#define RE15_U42_SUB07_ENDE       0x1DB2
#define RE15_U42_SUB04_OFF        0x15CA
#define RE15_U42_SUB04_ENDE       0x15E6
#define RE15_U42_HOF_X            (-24506) /* Disc ROOM3091 Se_on @0x1778 `46 a0`                                             */
#define RE15_U42_HOF_Z            1500     /* Disc ROOM3091 Se_on @0x1778 `dc 05`                                             */
#define RE15_U42_LOBBY_X          (-28481) /* Mod @0x1AA6 `bf 90` (Lobby-Tuer Slot 0)                                         */
#define RE15_U42_LOBBY_Z          (-155)   /* Mod @0x1AA6 `65 ff`                                                              */
#define RE15_U42_MSG_JOHN         33       /* D309 #21 */
#define RE15_U42_MSG_ELZA         34       /* D309 #24 */
#define RE15_U42_MSG_JOHN2        36       /* D309 #25 @0x1BEF 「任せとけ！」 (Runde 2, G2)                                   */
#define RE15_U42_MSG_STROM        35       /* 80 % T25; Uebersetzung = englische Spalte KATALOG_80.md:215 (PORT-WAHL)            */
#define RE15_U42_SIG_FREI         0x14     /* Bank 5, Takt -> Programm: keine Zeile, keine Stimme (wie RE15_S3B_SIG_FREI)       */

/* ---- U-11 ------------------------------------------------------------------------------------------------------------- */
#define RE15_U42_RAUM_4020        0x4020u
#define RE15_U42_PLAN_EREIGNIS    11       /* Slot 8 @0x073C `ff 00 18 0b`                                                   */
#define RE15_U42_PLAN_SUB_OFF     0x0B06
#define RE15_U42_PLAN_MSG         2
#define RE15_U42_PLAN_BLATT       8        /* EXE @0x8004BDB4 `ori v1,zero,0x8`; "Laboratory B1"                              */

/* ---- Haken ------------------------------------------------------------------------------------------------------------ */
const uint8_t *re15_ungenutzt_r42_ereignis(uint16_t room_id, uint8_t event_id);              /* scd_vm.c scd_event_fire */
const uint8_t *re15_ungenutzt_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc); /* scd_vm.c op_evt_exec  */
void re15_ungenutzt_r42_texte(uint16_t room_id);                                             /* msg_common.c          */
void re15_ungenutzt_r42_tick(void);                                                          /* scd_vm.c scd_vm_tick  */
int  re15_ungenutzt_r42_blatt(unsigned page);                                                /* re15_map_owned.c      */

/* Pruefhaken */
const uint8_t *re15_ungenutzt_r42_programm(uint16_t room_id, int welches, int *out_len);   /* 0 Hauptprogramm des Raums */
const uint8_t *re15_ungenutzt_r42_text(uint16_t room_id, uint8_t id, int *out_len);
extern unsigned g_re15_u42_zaehler[6];   /* 0 Rechner 501, 1 Szene 3091, 2 Schalter 3091, 3 Plan 402 genommen, 4 Bytes anders */

#endif /* RE15_UNGENUTZT_R42_H */
