/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE6: Zug 6030 (Leon) / 6031 (Elza).
 *
 * PORT-STORY nach dem freigegebenen Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md
 * (3.2 L21, 4.2 E6/E9, Abschnitt 5 T-05; Entscheid F-10 "Tiefe aendern"). Ids/Bit = VERTRAG analysis/befunde_runde42/VERTRAG.md
 * (2.3: 6030 msg 2/3/4/5/21/22 Neutext, 6031 msg 1/2 Neutext, 12..17 neu; 1.2: Bit 130 = RE15_R42_BIT_ZUG_ELZA).
 *
 * ---- L21 (6030, Leon) ---------------------------------------------------------------------------------------------------------
 * Wagen-Programm k_s80_6030_sub02 (tools/r41_story80/szenen_bauen.py, Folge 0,1,21,2,3,22,4,5,6,7,23,24,8): Zeilen 21/2/3/22/4/5
 * mit neuem Wortlaut; Sprecherwechsel msg 2 Sherry -> Leon (Leon-Gestenblock @0x00ECA, Blick bleibt bei John), msg 22 John -> Sherry
 * (Blick zu Sherry @0x00DEC, Sleep 100 @0x00E84). "She's out there fighting the monsters" (RDT msg 2 @0x1416) faellt weg.
 *
 * ---- E6 (6031, Elza) ----------------------------------------------------------------------------------------------------------
 * ROOM6031 sub01 @0x00D6E `04 ff 18 02` -> k_s6_6031_wagen = sub02 [@0x00D78,@0x00DFA) (msg 0 story_r36, 1, 2 mit Neutext) + msg 12
 * (Form msg 1 @0x00DD2, Sleep 100 @0x00DD6) + msg 13 (Form msg 2 @0x00DDA, Elza-Gesten @0x00DDE) + Schluss [@0x00DFA,@0x00E0E).
 *
 * ---- E9 (6031, Elza): Zugstart ------------------------------------------------------------------------------------------------
 * Original: nach dem Tor ((3,89), ROOM6001 sub02 @0x00F26 `22 03 59 01`) setzt sub07 Sherry nicht mehr ans Pult (@0x00F1A Ck(3,89)=0)
 * und ruft @0x00F54 nur die Licht-Schleife sub05; niemand startet den Zug [Konzept B31]. Port: ROOM6031 sub07 @0x00F54 `04 ff 18 05`
 * -> k_s6_6031_zug, einmalig ueber (9,130), nur im Fuehrerstand (Cut 0: sub00 @0x00D2A Case 0 -> Gosub sub07). Ablauf nach Leons
 * Zugstart (annette_r41.c k_zug_seg): Sherry am Pult (Satz sub07 @0x00F1E), Pult-Gesten (sub08 @0x00FC8), Pultlicht
 * (sub08 @0x00FDC..@0x00FEC ohne Cut 5), Zugfilm R600ZUG.STR (Port-Zeile 60, re15_zws_r41.h "Zugfilm Raum 600"; der Film traegt den
 * Zug-an-Ton, Runde 42 Strang finale), Cut 1 + Licht-Schleife = Fahrt (@0x01020, @0x00F54), msg 14, Krachen + WARNING (sub08
 * @0x00FF0..@0x0100B, Vorschlag A7), Cut 1, alle drehen sich zur Wagentuer, msg 15 (John), 16 (Sherry), 17 (Elza). John steht im Fuehrerstand (Satz sub06 @0x00E56 Slot 1, Lage PORT-STORY, Werkzeug
 * tools/r42_story/szenen_stage6.py). Danach wie heute: Wagen nach (3,89) = sub06 (msg 11, Licht-Schleife, Tuer nach 5091 offen).
 * "Ruck": wie bei Leon = Zug-an-Ton + Pultlicht + Licht-Schleife.
 *
 * ---- E9 Ruckeln (Runde 42 rest b) ----------------------------------------------------------------------------------------------
 * RE1.5 hat kein Bildwackeln, das der Zug nutzen koennte: Opcode 0x5C ist in RE1.5 KEIN Sce_shake_on. Tabelle @0x800744a8,
 * Eintrag 0x5C @0x80074618 -> 0x80042a10: `lhu a0,0(pc)` @0x80042a28, `jal 0x80044fec` @0x80042a2c, pc += 4 @0x80042a3c;
 * FUN_80044fec schreibt das Halbwort in die Tabelle 0x80074828 (Basis je Stufe 0x800748fc = 0/38/50/65/77/98); FUN_80044210 liest
 * sie mit dem Index @0x800b0fe0/@0x800b0fe2 und vergleicht die unteren 6 Bit mit der laufenden BGM 0x800b2b44 (Raum-BGM-Tabelle).
 * Keine ausgelieferte RDT benutzt 0x5C (Zensus tools/scd_dump_room.py ueber alle STAGE1-6-RDTs: 0 Treffer).
 * RE2 (Beta -> Retail: wo RE1.5 unfertig ist, gilt RE2) faehrt den Zug in ROOM7030: sub08 @0x06F8/@0x06FC, sub12 @0x0A48/@0x0A4C
 * und sub13 @0x0AE0/@0x0AE4 starten jedesmal ZUSAMMEN Evt_exec(7, sub15) = vorbeiziehende Lampen und Evt_exec(3, sub14) = Ruckeln.
 * sub14 @0x0B12..@0x0BF4 ist eine Endlosschleife aus Vib_set1 (0x8B, @0x80059394) und Sce_shake_on (0x5C, @0x80058164; RE2-Laenge 3,
 * pc[1] = dx, pc[2] = dy): Tabelle k_ruck in story6_r42.c, 64 Bilder je Durchlauf, jeder zweite ab dem dritten ein Bild laenger
 * (var9 >= 2 -> Evt_next @0x0BDC; var9 = 0 @0x0640 in sub08). FUN_8002ac94 kopiert bei pc[1] < 0 nichts (`bgtz a1` @0x8002acb8):
 * sichtbar sind nur die Bilder mit pc[1] = 1 (1 Bildpunkt nach links, mit pc[2] = 1 auch eine Zeile nach oben).
 * Port (PORT-WAHL): ROOM6031 nach dem Zugstart ((9,130) = 1) - solange die Licht-Schleife sub05 laeuft (= RE2 sub15, die Fahrt),
 * laeuft k_ruck mit; Beginn im ersten Takt, in dem der sub05-Faden aktiv ist (RE2 startet beide Faeden im selben Takt @0x0AE0/
 * @0x0AE4; die Fadenreihenfolge des Ports kann das um 1 Bild verschieben). Gezeichnet in render_pc.c ueber ein Zwischenziel des
 * Szenenteils (negativer Viewport-Ursprung zeichnet im direct3d-Renderer nichts), Balken und Text unverschoben.
 * Leon 6030 (Runde 42 rest Stufe 4, NUTZERWUNSCH 18.11.2026 woertlich: "Ja, na klar, wenn du die ruckeln lassen kannst waehrend der
 * Zug faehrt lass sie auch ruckeln."): dieselbe Regel mit Leons Zugstart-Bit (9,133) RE15_AN_ZUG_BIT und seiner Licht-Schleife
 * ROOM6030 sub05 @0x01004..@0x0101C (24 Byte, bytegleich mit ROOM6031 @0x00E2A). Der Haken in re15_story6_r42_evt_exec merkt sich
 * in 6030 nur den Anfang dieses sub05 und gibt das Ziel unveraendert zurueck; Leons Zugstart-Programm (annette_r41.c k_zug_seg,
 * abgenommen), seine Texte, Kamera und Toene (annette_r41.c zug_tick) bleiben unberuehrt. Das Ruckeln beginnt also am Zugstart mit
 * z_fahrt (Kopie von sub07 @0x01132) und laeuft bei jedem spaeteren Betreten des fahrenden Zugs (sub07 @0x01132, sub06 @0x010C4).
 * Opcode 0x5C selbst bleibt fuer RDT-Bytecode RE1.5 (scd_vm.c op_sce_shake_on).
 * Wagen vor dem Zugstart ((3,89)=1, (9,130)=0, Cut 6/8/12: sub06 @0x00EEA `04 ff 18 05`) -> k_s6_6031_tuer: Ausgang Slot 2 wieder die
 * Tuer (main00 @0x00C72), Tuer nach 5091 zeigt msg 4 (sub06 @0x00E90) - Form Leons k_tuer.
 *
 * ---- Toene ---------------------------------------------------------------------------------------------------------------------
 * Signale Bank 5 (Raum-Scratch; die 6031-Skripte nutzen nur (5,0x20)/(5,0x21)): 0x10 Zug-an-Ton (ANNETTE_SE Satz 3 = RE2 ROOM7020 sub04
 * @0x014AA Se_on(2,0x0C)), 0x11 Krachen (Satz 0 = RE2 Claire ROOM6141 sub02 @0x01040 Se_on(3,0x0B)) - dieselben Saetze wie Leons
 * Zugstart (re15_annette_r41.h RE15_AN_SE_*), gespielt ueber re15_annette_r41_ton_spielen.
 *
 * ---- T-05 (beide) --------------------------------------------------------------------------------------------------------------
 * Tiefe der Port-Masken ueber der Liegeflaeche in Cut 9 (MASKS/ROOM6030.MSK == ROOM6031.MSK): tools/maske/liege_tiefe_r42.py.
 */
#ifndef RE15_STORY6_R42_H
#define RE15_STORY6_R42_H

#include <stdint.h>

#define RE15_S6_RAUM_LEON         0x6030u
#define RE15_S6_RAUM_ELZA         0x6031u
#define RE15_S6_SUB_WAGEN         2         /* sub01 @0x00D6E `04 ff 18 02`                                                     */
#define RE15_S6_SUB_LICHT         5         /* sub07 @0x00F54 / sub06 @0x00EEA `04 ff 18 05`                                    */
#define RE15_S6_TOR_BANK          3         /* (3,89): ROOM6001 sub02 @0x00F26 `22 03 59 01`; Leser sub06 @0x00EB4, sub07 @0x00F4A */
#define RE15_S6_TOR_BIT           89
#define RE15_S6_SIG_ZUG_AN        0x10      /* Bank 5, Programm -> Takt (Form annette_r41.c RE15_AN_SIG_ZUG_AN)                  */
#define RE15_S6_SIG_KRACH         0x11

enum { RE15_S6_P_WAGEN = 0, RE15_S6_P_ZUG = 1, RE15_S6_P_TUER = 2, RE15_S6_P_N = 3 };

extern unsigned g_re15_s6_zaehler[RE15_S6_P_N + 2];   /* [P] Umleitungen je Programm, [3] Zug-an-Ton, [4] Krachen */

void           re15_story6_r42_texte(uint16_t room_id);                                        /* msg_common.c, nach den RDT-Texten */
const uint8_t *re15_story6_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc);     /* scd_vm.c op_evt_exec               */
void           re15_story6_r42_tick(void);                                                     /* scd_vm.c je Bild                   */
const uint8_t *re15_story6_r42_text(uint16_t room_id, uint8_t id, int *out_len);               /* Tests                              */
const uint8_t *re15_story6_r42_programm(int p, int *out_len);                                  /* Tests                              */
const uint8_t *re15_story6_r42_folge(int p, int *out_n);                                       /* Tests                              */
int            re15_story6_r42_ruck(int *dx, int *dy);                                         /* render_pc.c je Bild (rest b)       */
extern unsigned g_re15_s6_ruck_bilder;                                                         /* Messhaken (Tests)                  */

#endif
