/*
 * RE1.5 Rebuilt — Runde 41 Spur AN: Annette-Szene im hinteren Zugwagen ROOM5090 (Leon) und Zugstart im Fuehrerstand
 * ROOM6030 (Nachbesserung 1).
 *
 * Kopf, Herleitung und Abgrenzung: include/re15_annette_r41.h, Dossier analysis/befunde_runde41/AN_annette.md.
 *
 * ⛔ PORT-ERGAENZUNG nach Nutzerentscheid, KEIN Auslieferungscode. Vorbild der 5090-Szene ist die echte RE2-Szene
 * (Claire-Disc SLES-10972, PL1/RDT/ROOM6141.RDT MAIN sub02 @0x01024..@0x0118E, Emulator-Aufnahme mit Ton und 97
 * Savestates: analysis/befunde_runde41/AN_belege/re2_claire/zeitachse.md). Reihenfolge, Sleeps und Toene sind von dort
 * uebernommen (jede Zahl mit RE2-Adresse); Kameras, Lagen und Clips muessen auf RE1.5 5090 abgebildet werden und sind
 * PORT-WAHL (Begruendung je Konstante). Kein Asset-Patch: die RDTs bleiben byte-true, die Szenen sind Bytecode aus
 * ORIGINAL-Opcodes ueber die vorhandene VM.
 */
#include "re15_annette_r41.h"
#include "re15_story80_r41.h"   /* Integration A2: RE15_S80_BIT_NACHTRAG (Zugstart wartet auf den Sherry-Nachtrag) */
#include "re15_zws_r41.h"       /* Strang finale Stufe 2: RE15_ZUG600_MSG_FILM (Zugfilm-Zeile im Zugstart) */

#include <string.h>
#include <stdlib.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_damage.h"        /* re15_enemy_bone_world_pos */
#include "re15_esp.h"           /* re15_esp_fx_spawn_rows, re15_esp_room_bank, re15_esp_fx_count (Blut, RE2 FUN_80104C24) */
#include "re15_msg.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_math.h"         /* re15_squareroot0 */

#include "gen/annette_r41_texte.inc"   /* k_an_5090_msg40.. / k_an_6030_msg27.. (tools/r41_annette/texte_bauen.py) */

int re15_r38_birkin_anim(re15_actor_t *e);        /* enemy_ai_common.c: Clip-Vorschub mit der Bank des Typs */
int re15_r38_birkin_fc(const re15_actor_t *e);    /* enemy_ai_common.c: Clip-Laenge */
int re15_bb_bank_da(const re15_actor_t *e);       /* birkin_bahnsteig.c: 0x30 traegt die Riesenform */

/* ---- 5090: Lagen (PORT-WAHL, Dossier Nachbesserung 1 Abschnitt N1.2) -----------------------------------------------
 * RE2 ROOM6141: Annette geht auf die Stelle zu, an der William herabfaellt (@0x010A2/@0x010B8/@0x010DE), und bleibt
 * 4497 vor ihm stehen (Savestate e07: Annette (-11097,-15001), Birkin (-6600,-14800)); der Hieb kommt mit einem Vorstoss
 * von 1397 (e20..e22: -6600 -> -7997). Die RE1.5-Riesenform trifft mit dem Hieb links (Clip 4) nur im Trefferfenster
 * lokal x 3685..3864 (gemessen aus BAHNSTEIG_FORM.EMD, Dossier 8.2); ihr Clip 4 hat keinen Vorstoss (NB1/NB2: Abstand 3760).
 * NB3: der Vorstoss von RE2 laeuft im Takt (AN_VORSTOSS), Abstand vor dem Hieb 3760 + 1397.
 * Kameras (Projektion mit den RID-Daten 5090 @0x60+32*Cut, tools/form_kamera.py-Kette, Dossier N1.2):
 *   Cut 12 (Auge (18526,-2656,-24660), weit, Leon vorn)      = RE2 Cut 7 (beide im Bild, Krach, "William.")
 *   Cut 15 (Auge (46,-1236,-24302), Blick nach Osten)          = RE2 Cut 2 (sie kommt auf die Kamera zu)
 *   Cut 11 (Auge (7626,-3256,-25360), Blick nach Westen)       = RE2 Cut 0x0A und (NB3) 0x0B: Sturz, Bruellen, Satz, Hieb
 *   Cut 13 (Auge (11050,-3110,-24812), Blick nach Osten, Leon) = RE2 Cut 6 (weg von der Tat, Schrei aus dem Off)
 * Nachbesserung 3 (Abnahme 1 M1, Dossier N3.1): die Kasten-Projektion aus NB1 gab die echte Pose nicht wieder; mit L (500,-22600)
 * stand die Form 1760 vor dem Auge von Cut 15 und deckte beim Satz 71-97 % des Bildes (Annette 0-32 % sichtbar). Neu gemessen mit
 * dem Gerippe der Form (BAHNSTEIG_FORM.EMD + ANNETTE_4D.EMD, form_buehne-Kette, ID-Puffer, Kinobalken 24/216 abgezogen; Werkzeug
 * analysis/befunde_runde41/AN_belege/werkzeug/n3/szmass.py, gegen den Fensterlauf geprueft: F600 Cut 15 offline 74 %):
 *   Cut 15 sieht vom Wagen nur x 0..6000 (die Blickachse verlaesst den Gang bei x 3220). Suche ueber alle Lagen mit 3760 bzw.
 *   3760 + 1397 Abstand (suche3.py): keine einzige, bei der Annette >= 95 % frei ist und die Form <= 40 % deckt.
 *   Cut 11 (von Annettes Seite, wie das 80-%-Foto #145): Suche suche4.py, gewaehlt S (3700,-22500) / L (-1378,-23395):
 *   Satz (Form Clip 2) Form 9,8 % des Bildes, Annette 100 % frei und ganz zwischen den Balken (Bild-y 185..420 von 48..432),
 *   Drohen 8,5 %, Landung 9,8 %. Annette steht naeher am Auge (x > 4000) mit den Fuessen unter dem Balken.
 * Deshalb (PORT-WAHL): Sturz, Bruellen, Satz und Hieb in Cut 11; RE2 0x0B (hoch von Williams Seite auf Annette) hat in 5090 keine
 * Entsprechung, ohne dass die Form zwischen Auge und Annette steht.
 *   L  Landestelle (-1378,-23395): 5157 = 3760 + 1397 von S; die Form stoesst beim Hieb die 1397 vor (RE2 e20..e22, s. unten),
 *      danach steht die Hand im Trefferfenster (lokal x 3685..3864, Dossier 8.2).
 *   S  Haltepunkt (3700,-22500): 1303 vom naechsten Bodenfeuer main00 @0x0110E..@0x0120E ((2400,-22600)), Weg A0->S >= 1303.
 *   A0 Start (8000,-24250) wie NB1 (Cut 12 neben Leon).
 * Wegpunkte W1/W2 teilen A0->S wie RE2 seine drei Plc_dest-Strecken (8040/5016/4000 = 47 % / 76 %). */
#define AN_A0_X            8000
#define AN_A0_Z          (-24250)
#define AN_W1_X            5979
#define AN_W1_Z          (-23427)
#define AN_W2_X            4732
#define AN_W2_Z          (-22920)
#define AN_S_X             3700
#define AN_S_Z           (-22500)
#define AN_L_X           (-1378)
#define AN_L_Z           (-23395)
/* Vorstoss beim Hieb: RE2 Birkin x -6600 (e20) -> -7997 (e22) = 1397 auf Annette zu (zeitachse.md Abschnitt 0, RAM lauf3). */
#define AN_VORSTOSS        1397
/* Sturz der Riesenform vom Dach = ROOM5080 sub03 (Original): @0x00822 Pos_set y -5600 `20 ea`, @0x0082A Sleep 1,
 * @0x0082E Speed_set(1,400) `2f 01 90 01`, @0x00832 For 14 `0d 00 06 00 0e 00` { Add_speed `30`, Evt_next } -> y 0.
 * Im Takt statt im Programm: das Programm muss wie RE2 (gosub sub10 @0x010E6) waehrend des Sturzes auf Annettes Ankunft
 * warten, und deren Bit (5,0x21) ist ein Ein-Bild-Puls (Bank 5 Wort 1 wird jedes Bild gewischt, scd_vm.c FUN_8003ebf4);
 * eine blockierende For-Schleife verpasste ihn (gemessen: Puls F175, Schleife bis F176). */
#define AN_STURZ_Y       (-5600)
#define AN_STURZ_HALT        1
#define AN_STURZ_V          400
#define AN_STURZ_N           14
/* Liegen: bis Stufe 6 (Strang finale) lag die Leiche an der in RE2-Zerlegung abgeleiteten Stelle (RE2 @0x01162 setzt Annette
 * 1215 weg von William und 245 quer, NB3/NB4: Port (4856,-22748) Gier 1170, 700 nach Sueden wegen Regal SCA #28). Stufe 6 legt
 * sie dorthin, wo sie faellt (AN_LIEGEN_DREH unten): die alte Stelle liegt in Cut 11 ganz unter dem Kinobalken (Netz Bild-y
 * 216..266 von 24..216, scratchpad finale/s6r1/scan_lage.py) - mit Wegstoss dorthin verschwand sie im Entwurf neu1 aus dem Bild. */
/* Hieb-Takt (Nachbesserung 3, Abnahme 1 M7 "Ausholen bis Treffer Port 0,5 s, RE2 1,3 s"), RE2-Aufnahme Lauf 1
 * (re2_claire/zeitachse.md Abschnitt 2, Bildleiste re2c_12b_hieb_dicht.jpg): Schnitt 0x0A + Member_set 0x17=2 @0x0113C/@0x01142
 * bei 69,77 s, Birkin steht noch bei 70,05 s, erstes Ausholbild 70,22 s, Blut (Trefferbild) ab 71,09 s, Schnitt 6 bei 72,23 s.
 *   Vorlauf  69,77 -> 70,22 s = 0,45 s = 13 Port-Bilder: die Form steht nach dem Schnitt noch (Clip 2).
 *   Ausholen 70,22 -> 71,09 s = 0,87 s = 26 Port-Bilder; Clip 4 holt in 14 Bildern aus (Treffer Bild 14) -> je Bild 2 Takte = 28.
 *   Danach   71,09 -> 72,23 s = 1,14 s = 34 Port-Bilder bis zum Schnitt weg.
 * Der Vorstoss (AN_VORSTOSS) laeuft gleichmaessig ueber die 28 Ausholtakte (RE2 e20 -> e22). */
#define AN_HIEB_VORLAUF      13
#define AN_HIEB_TAKT          2
#define AN_HIEB_NACH         34
/* Clips der Riesenform (Stufe 3, BB_C_* in birkin_bahnsteig.c) */
#define AN_FC_DROHEN         10     /* Drohen; Bild 45-52 = Pose Bild 2 = 80 % #145 (STUFE3.md Abschnitt 3) */
#define AN_FC_STEHEN          2
#define AN_FC_HIEB            4     /* Hieb links, Teil 13 */
#define AN_HIEB_TREFFER      14     /* erstes Trefferbild, EDD-Bit 16 Bild 14-17 (G4-Clip 4, STUFE3.md Abschnitt 3) */
#define AN_HAND_LINKS        13
#define AN_TON_BIT   0x08000000u    /* Bild-Ton: Bit 27, Id = Wort >> 28 (FUN_80016028, birkin_bahnsteig.c bb_bild_ton) */
#define AN_TON_BANK          24     /* G4-Bank, wie bb_se */
/* Clips der Frau (0x4D-Bank, 24 Clips, Kontaktblatt AN_belege).
 * Strang finale Stufe 8 (Nutzer 19.11.2026 woertlich: "Beim zurückweichen fliegt Anette irgendwie komisch in der Luft."):
 * bis Stufe 7 spielte sie vom Hieb-Signal bis zum Trefferbild Clip 4 ihrer Aktionsbank ("Zurueckweichen mit erhobenen Armen",
 * eine Deutung des Kontaktblatts). Gemessen (Pose-Log s6r1/lauf/neu2, F1882..F1923): Becken -1695 -> -1800, tiefster Knochen
 * -101 -> -205..-253 (beide Fuesse 100..150 ueber der Standhoehe), ab F1901 Bild 19 eingefroren = 22 Bilder in der Luft.
 * Was Clip 4 im Original ist: die FLUGPHASE des Hinunterspringens. RE1.5 PSX.EXE, Executor-Sub 0x17/0x18 (Tabelle
 * @0x80076ca0[23] = [24] = 0x80052c14, Phasen +0x6 ueber @0x8001131c): Phase 0 Clip 3 (`ori v0,zero,0x3` @0x80052c6c /
 * `sb v0,148(v1)` @0x80052c70, Absprung bis Bild 13 @0x80052cc0), Phase 2 **Clip 4** (`ori v0,zero,0x4` @0x80052d08 /
 * `sb v0,148(v1)` @0x80052d0c, ohne Ueberblendung `sb zero,143(v0)` @0x80052d2c) mit Vorschub 0x320 (@0x80052d48/50) und
 * y += 500 (@0x80052d60-6c), Phase 3 je Bild y += 90 + 50 * Zaehler (@0x80052e28-5c) bis 800 ueber dem Boden (@0x80052e88-a4),
 * Phase 4 Clip 5 = Landung (`sb v1,148(v0)` @0x80052ec4, y = Boden @0x80052ef4-fc). Der Clip selbst hat keine Wurzelbewegung
 * (ANNETTE_4D.EMD Paar 1 Clip 4, 20 Bilder, Wurzel (0,-1800,0), v 0): den Fall rechnet der Code; ohne ihn haengt sie in der Luft.
 * Was RE2 an dieser Stelle zeigt: Claire ROOM6141 MAIN sub02 @0x01138 `3f 02 02 44` = Plc_motion(Modus 2, Clip 2, Flags 0x44).
 * Claire-EXE SLES_109.72: Handler @0x80052ef0 (+4 = (Byte1 << 8) | 4 @0x80052f10-18, Clip `sb a1,0x14c(v1)` @0x80052f1c, Flags
 * `sh a2,0x1cc(v1)` @0x80052f20); Routine 4 = 0x800a03a0[4] = 0x80061470 verteilt nach +5 ueber 0x800a0d78 (@0x80061530-4c):
 * [0] 0x800615b8 = +0x190/+0x194 (Raum-RBJ, gesetzt @0x80018868/78), [2] 0x80061600 = `lw a1,0x180(a0)` @0x80061608 /
 * `lw a2,0x184(a0)` @0x8006160c = das ZWEITE Clip-Paar ihres eigenen EMD (Lader dir[3] -> +0x184 @0x8001a740-4c, dir[4] ->
 * +0x180 @0x8001a750-5c). NICHT Clip 2 des Raum-RBJ: der hat 1 Bild (RBJ @0x2E70, EDD @0xA1D4), der RAM-Bildzaehler lief aber
 * 6 / 26 / 46 (Savestates lauf3 e20..e22 `r04020100 M02/..`, AN_belege/re2_claire/ram_reihe_voll.md, synchron zu Birkins M07
 * 5/25/45). EM049 Paar 2 Clip 2 (RE2/CDEMD0.EMS Sektor 4665, EDD @EMD+0x36F4, EMR @EMD+0x3A60, Keyframes 50..74) = 50 Bilder
 * STEHEN: Wurzel (0,-1722,0), Fuesse am Boden, Arme haengen - ihre Stand-Schleife. Flags: 0x40 = ohne Ueberblendung (`andi
 * v0,v0,0x40` @0x800621e4 -> `sb zero,0x14e(s0)` @0x800621f0), 0x04 = Schleife (`andi v0,v0,4` @0x80062228 -> +6 = 1 @0x80062238).
 * RE2 zeigt Annette in diesem Schnitt (0x0A) nicht; sie steht einfach, bis der Schnitt weg ist.
 * BAU: derselbe Befehl im Programm (k_prog, OP_MOTION(2, 2, 0x44)). Der RE1.5-Executor fuehrt ihn bytegleich aus: Handler
 * @0x80041b90 (+4 = 4 @0x80041bb0, +5 = Byte1 @0x80041bc4, Clip @0x80041ba8), Sub 2 = 0x80076ca0[2] = 0x80050f00 nimmt
 * `lw a0,368(v0)` @0x80050f88 / `lw a1,372(v0)` @0x80050f90 = +0x170/+0x174 = ebenfalls das zweite Clip-Paar des eigenen EMD
 * (Lader dir[3] -> +0x174 @0x800224ac-b8, dir[4] -> +0x170 @0x800224bc-c8), Flags 0x40 @0x80050f6c-78, 0x04 @0x80051004-10.
 * Clip 2 dieses Paars ist bei der 0x4D wie bei RE2 die Stand-Schleife (52 Bilder, Wurzel -1695; dieselbe, in der sie seit der
 * Ankunft an S steht: state 4 Sub 6 Phase 3, Clip 2 `sb v0,148(v1)` @0x800518c8). PORT-WAHL nur in einem Punkt: es laufen die Keyframes der RE1.5-Frau
 * (Regel "RE2 = nur Funktion, Modell bleibt RE1.5"), nicht die 25 RE2-Keyframes - als Clip 25 angehaengt hoeben die das Becken um
 * 27 (-1722 gegen -1695, gemessen scratchpad finale/s8r1/variante_a). Die Szene uebernimmt sie deshalb erst im Trefferbild.
 * Nachbesserung 3 (Abnahme 1 M4: nach dem Treffer stand sie 1 s aufrecht mit erhobenen Armen in Cut 11): im Trefferbild
 * sackt sie mit Clip 6 sofort zusammen (Bild 0-12, ab Bild 12 am Boden, gehalten); RE2 zeigt sie in diesem Moment nicht. */
#define AN_FA_STURZ           6     /* faellt zu Boden (Bild 6 = Tiefpunkt, danach richtet sie sich wieder auf) */
/* Strang finale Stufe 6 (Nutzer 18.11.2026 woertlich: "Wenn Anette am Ende von den 4 beinigen Birkin nieder geschlagen wird darf
 * sie in der Animation nicht noch mal aufstehen. Sondern mit den Schlag direkt liegen bleiben."). Gemessen (s6r1 alt1, Pose-Log):
 * Clip 6 lief ueber den Halt bei Bild 12 hinaus bis Bild 39 (der globale Advancer schob sie zusaetzlich vor, player_common.c), Kopf
 * (Knochen 8) F1930 -1003 -> F1957 -1410 = sie stand auf; danach Sprung an die Liegestelle (1105 weiter) beim Schnitt F1958.
 * Clip 6 ist "Sturz UND Aufstehen" (RE2/ANNETTE_4D.EMD = RE1.5 0x4D, Clips 0..23 bytegleich, CDEMD0.EMS 0x4D Keyframes 176..225
 * gleich): Wurzel-py je Bild (Bildliste @0x330.., Keyframes @0x3E5C + 80 * i, py = Wort +2) -1357 -1278 -1083 -835 -595 -427
 * **-387** (Bild 6, Keyframe 182 @0x403C, py @0x403E `7d fe`) -429 -464 .. -450 (Bild 12) .. -665 (Bild 31) .. -1720 (Bild 49);
 * Kopf ab Bild 6 (-878) wieder hoeher (Bild 12 -1004, Bild 30 -1163, Bild 49 -2343). Bild 6 ist der Tiefpunkt -> Bodenbild.
 * Ab dort geht sie in die RE2-Liegepose (Clip 24 Bild 0, s. AN_FA_LIEGEN) ueber, mit der Original-Ueberblendung einer Clip-
 * Umschaltung: +0x94 = Clip, +0x95 = 0, +0x8f = 7 (`ori v0,zero,0x6` / `sb v0,0x94(v1)` @0x80050018-1c, `sb zero,0x95(v0)`
 * @0x8005002c, `ori v0,zero,0x7` / `sb v0,0x8f(v1)` @0x80050038-3c); FUN_8001f3bc mischt mit Gewicht +0x8f * Rate
 * (`lbu s0,0x8f(v1)` @0x8001f408, `mult v0,a3` @0x8001f474) und zieht danach 1 ab (`addiu v0,v0,-0x1` / `sb v0,0x8f(v1)`
 * @0x8001f5b0-b4) -> 7 gemischte Bilder. */
#define AN_FA_STURZ_BODEN     6     /* Bild 6 = Tiefpunkt (Keyframe 182, py @0x403E = -387) */
#define AN_UEBERBLEND         7     /* +0x8f-Saat @0x80050038 `ori v0,zero,0x7` */
/* Liegestelle und -gier (PORT-WAHL nach NUTZERWUNSCH oben: "mit den Schlag direkt liegen bleiben", kein sichtbarer Lagesprung).
 * Sie bleibt, wo der Hieb sie trifft (kein Pos_set mehr; RE2 setzt die Lage @0x01162, waehrend die Kamera weg ist, und zeigt den
 * Sturz nicht). Clip 24 liegt mit den Fuessen nach vorn (Modell +x: Fuss 7 bei +1589, Kopf/Knochen 8 bei -615, Keyframe 358
 * @0x773C), Clip 6 faellt nach hinten (Bild 6: Knie vorn, Oberkoerper hinten) - mit der Gier des Trefferbilds (Blick auf die
 * Form, S -> L 1934) ist das dieselbe Richtung, die Fuesse laegen dann aber 35..79 vor dem Bodenfeuer main00 (2400,-22600).
 * AN_LIEGEN_DREH = kleinste Drehung im 16er-Raster ab der Blickrichtung S -> L, bei der das Netz der Liegepose >= 100 von jeder
 * soliden SCA-Zelle und >= 403 (= Abstand der abgenommenen NB4-Liegestelle) von jedem Bodenfeuer liegt (scratchpad finale/s6r1/
 * scan3.py: an S -240 -> Gier 1694, Wand 288, Feuer 418, in Cut 11 1744 von 1950 Netzpixeln im Kinoband; an der Trefferlage
 * (3773,-22529) Wand 317, Feuer 440). Die Drehung laeuft gleichmaessig ueber die Sturzbilder und die Ueberblendung
 * (AN_STURZ_BILDER), der Rest der Lage bleibt. Beim Wiedereintritt liegt sie an S mit derselben Gier. */
#define AN_LIEGEN_DREH     (-240)
#define AN_STURZ_BILDER     (AN_FA_STURZ_BODEN + AN_UEBERBLEND)
/* Nachbesserung 4 (Abnahme 2 N2 "Leiche wirkt sitzend, Knie hoch, Kopf aufrecht"): liegen wie RE2 statt Clip 6 Bild 12 um
 * 60 Grad gekippt. RE2 Claire ROOM6141 MAIN sub02, waehrend die Kamera weg ist: Pos_set @0x01162 `32 00 f4 cf 84 03 3c c4` =
 * (-12300, 900, -15300), Dir_set @0x0116A `33 00 00 00 e4 0c` = 3300, Plc_motion @0x01172 `3f 00 18 04` = Clip 0x18 des
 * Raum-RBJ; beim spaeteren Besuch dieselbe Lage und Clip (Sce_em_set @0x00D16 `... f4 cf 84 03 3c c4 e4 0c 18 01 ..`).
 * RAM lauf3 e23..e30 (AN_belege/re2_claire): +0x14C = 0x18, Teile y -433..-24 = flach am Boden, +0x156 (hp) = -1.
 * Der Clip liegt als Clip 24 in Paar 1 von ANNETTE_4D*.EMD (tools/r41_annette/annette_modell.py, RBJ ROOM6141 @0x2E70
 * Record 0 Clip 24 = 40 Bilder). RE2 laesst ihn als Schleife laufen (Bildzaehler e23..e30 6/26/6/22): dort atmet die
 * Sterbende noch und spricht spaeter mit Claire (sub03). Im Port ist sie tot -> Bild 0 gehalten (das Startbild, das
 * Plc_motion setzt: RE2-Handler 0x80056b8c = Opcode-Tabelle 0x800a74c8[0x3F] @0x800a75c4, darin `a060014d sb zero,333(v1)`
 * @0x80056ba0, v1 = Arbeits-Entity `lw v1,340(a0)` @0x80056b90; Clip `sb a1,332(v1)` @0x80056bb8; Leon PSX.EXE und SLUS_007.48
 * gleich - Nachbesserung 5, Abnahme 3 A1: vorher stand hier 0x80052ef0, dort steht `subu v0,v0,v1`).
 * PORT-WAHL: Halten statt Schleife. */
#define AN_FA_LIEGEN         24
#define AN_LIEGEN_Y         900     /* @0x01162 `84 03` */
/* Blut (Nachbesserung 2): der Emitter der RE2-Szene selbst. Claire-Disc SLES-10972, Birkin-Overlay (Typ 0x33, im RAM
 * @0x80100000, Savestate e22 aus analysis/befunde_runde41/AN_belege/re2_claire/), Routinen-Tabelle @0x80106584: [2] =
 * FUN_80104C24 (Member_set 0x17=2 @0x01142 -> Routine 2 @0x80104BE4-F8). Dort, NACH dem Bildvorschub @0x80104C9C-A8:
 *   @0x80104CD4-E8  Bildwort & 0x10000 (EDD-Trefferbit): `lw v0,0x178(s0)` `lw v0,0(v0)` `lui v1,1` `and v0,v0,v1`
 *   @0x80104CF0-F8  nur einmal (+7): `lbu v0,7(s0)` / `bnez`; @0x80104D04-0C `sb v0,7(s0)`, `jal 0x80057690` (a0 = 3)
 *   @0x80104D10-14  `addiu v0,zero,0xf` / `sb v0,0x16b(s0)`             -> 15 Bilder Blut, ab diesem Bild
 *   @0x80104D18-60  solange +0x16B: `jal 0x801058bc` (e, [0x80106330]+1, 1) @0x80104D38 und (e, [0x80106330]+3, 1)
 *                   @0x80104D4C, dann +0x16B - 1; [0x80106330] = 13 (RAM e22) -> Teile 14 und 16
 *   FUN_801058BC    Beschreiber 0x801065B8 + 6 * 1 = `00 01 00 00 00 20` (@0x801058D0-E4, Packung @0x80105994-B4) =
 *                   Effekt 0, sub 1, Skala 0x2000; Bit 0x80 = 0 -> Anker Einheitsmatrix 0x80095F68 (T = 0), a3 = Weltlage
 *                   des Teils (+0x5C/+0x60/+0x64 @0x80105950-78) = Blut bleibt, wo die Klaue in DIESEM Bild war;
 *                   Winkel a1 = FUN_80014E4C(e.x, e.z, Spieler.x, Spieler.z) @0x801058F0-908 (= Leon FUN_800154AC)
 *   FUN_8001B910    (= Leon FUN_8001BF10, Pool 96 @0x8001B910 `addiu t2,zero,0x60`): 3 Zeilen je Aufruf.
 * Gemessen (RAM e22, 71,54 s): 60 Plaetze 0/1/0x2000 in 10 Gruppen zu 6 (2 Teile x 3 Zeilen), Gruppen eines Bildes
 * wandern mit der Klaue; e23 (6 Spielbilder spaeter) noch 4 Gruppen. RAM e22: Teile 14/15/16 in einer Reihe (je ~195
 * auseinander, je ~600 von Teil 13) = die drei Klauen der Hand 13, 14 und 16 die aeusseren.
 * Port: dieselbe Kette auf dem vorhandenen RE2->RE1.5-Dekoder (enemy_ai_re2_zombie.c re2z_gore_fx_core: RE2-Effekt 0 ->
 * RE1.5-Blut 0, sub und Skala durchgereicht, re15_esp_fx_spawn_rows). Teile -> Knochen der Riesenform: die linke Hand 13
 * (Hieb links, AN_HAND_LINKS) traegt in BAHNSTEIG_FORM.EMD die Klauen 14 (729,30,-228), 15 (782,30,83), 28 (684,30,258)
 * und den Daumen 30 -> die aeusseren 14 und 28 (PORT-WAHL der Zuordnung, Form ist kein RE2-Modell). Die 8 Spritzer
 * vorher (4 an Annette im Trefferbild, je 2 nach 6/12 Bildern) hatten fuer Lage und Zeitpunkt keinen Beleg: entfallen. */
#define AN_BLUT_BILDER       15     /* @0x80104D10 `addiu v0,zero,0xf` */
#define AN_BLUT_KLAUE_A      14     /* RE2 Teil [0x80106330]+1 = 14 (@0x80104D2C `addiu a1,s1,1`) */
#define AN_BLUT_KLAUE_B      28     /* RE2 Teil [0x80106330]+3 = 16 (@0x80104D44 `addiu a1,s1,3`) */
#define AN_BLUT_EFFEKT        0     /* 0x801065BE `00` (Bit 0x80 = 0: Einheitsanker) */
#define AN_BLUT_SUB           1     /* 0x801065BF `01` + u16 0x801065C0 `00 00` */
#define AN_BLUT_SKALA    0x2000     /* u16 0x801065C2 `00 20` */
/* Nachbesserung 3 (Abnahme 1 M5 "kleiner Fleck am Boden, RE2 grosse Wolke vor der Kamera"): Bildgroesse wie RE2. RE2 zeigt das
 * Blut durch Schnitt 0x0A, ROOM6141 RID @0x64 + 0x0A * 32: fov 32946 (H = fov >> 7 = 257), Auge (-10656,-684,-16056); erster
 * Platz RAM e22 Slot 6 +0x2C/+0x30 = (-8064,-1865,-16471) -> 2878 vom Auge. Ein Zeilenbild hat auf dem Schirm die Groesse
 * Skala * H / Abstand; gleiche Groesse im Port: Skala = 0x2000 * (257 / H_Port) * (Abstand_Port / 2878), mit H und Auge des
 * Port-Schnitts im Trefferbild (g_room_rdt.cuts[Cut]) und dem Abstand zur Klaue 14 (PORT-WAHL: RE2-Bildwirkung als Massstab,
 * Effekt/sub/Zahl/Dauer bleiben RE2). */
#define AN_BLUT_RE2_H       257
#define AN_BLUT_RE2_ABSTAND 2878

/* ---- 5090: Programm --------------------------------------------------------------------------------------------------
 * Opcode-Formen mit Vorbild (Laengen = s_opcode_sizes, scd_vm.c):
 *   Set `22 bb ii vv`               ROOM5090 sub04 @0x012F2/@0x012F6 (Sperre (2,7), Balken (1,27))
 *   Cut_chg `29 cc`                 ROOM5090 sub04 @0x0130E
 *   Work_set `2e kk ii` + Nop       ROOM5090 sub04 @0x0131C
 *   Plc_dest `40 00 09 20 x z`      ROOM5090 sub04 @0x01320 (Modus 9 = drehen, Ankunftsbit 0x20)
 *   Plc_dest `40 00 09 21 x z`      ROOM6030 sub08 @0x01160 (Annette dreht sich, Ankunftsbit 0x21)
 *   Plc_dest `40 00 04 21 x z`      ROOM6000 sub02 @0x00FB0 (Annette geht, Modus 4) mit dem Bit 0x21 von oben
 *   Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`   ROOM5090 sub04 @0x01328..@0x01330 / ROOM6030 @0x0128A
 *   Sleep `09 0a nn nn`             ROOM5090 sub04 @0x0133E
 *   Message_on `2b id 00 00`        ROOM6030 sub02 @0x00DC2 (Maske 0 = Untertitel)
 *   Plc_motion `3f mm cc ff`        ROOM1021 sub02 @0x020C6 `3f 02 06 00` (Modus 2 = eigenes zweites Clip-Paar, @0x80050f00)
 *   Evt_exec `04 ff 18 ss`          ROOM5090 sub01 @0x01296
 * Bildzahlen = RE2-Skript (ROOM6141 sub02), XA-Laengen aus der Claire-STAGE6.BIN-Tabelle @0x4D4 (Laenge in
 * Kanalsektoren * 4032 / 37800 s, je 30 Bilder/s): Xa 0x30 11 Sektoren = 1,173 s = 35 Bilder; Xa 0x31 37 = 3,947 s =
 * 118 Bilder; Xa 0x32 13 = 1,387 s = 42 Bilder. RE2 wartet nach jedem Xa mit Wsleep bis zum Ende der Stimme.
 * Nachbesserung 3 (Abnahme 1 M7 "Port 17,8 s, RE2 24,4 s"): die Sleeps des RE2-Skripts zaehlen Spielbilder der PAL-Fassung,
 * 25 je Sekunde (VSync(2) @0x8002ade4-ec `90847662` lbu VSync-Teiler, zeitachse.md Abschnitt 0/3); NB1/NB2 setzten sie 1:1 als
 * Port-Bilder (30/s) und waren damit um 1/6 zu kurz. Jetzt PAL(n) = n * 30 / 25 (gerundet), also dieselbe Wanduhrzeit wie RE2.
 * Die Xa-Laengen sind schon Sekunden (oben). */
#define PAL(n)           (((n) * 6 + 2) / 5)
#define LE16(v)          (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)    0x22, (b), (i), (v)
#define OP_CUT(c)        0x29, (c)
#define OP_WORK(k,i)     0x2e, (k), (i), 0x00
#define OP_DREHEN(x,z)   0x40, 0x00, 0x09, 0x20, LE16(x), LE16(z)
#define OP_DREHEN_AN(x,z) 0x40, 0x00, 0x09, 0x21, LE16(x), LE16(z)
#define OP_GEHEN_AN(x,z) 0x40, 0x00, 0x04, 0x21, LE16(x), LE16(z)
#define OP_WARTE(bit)    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)      0x09, 0x0a, LE16(n)
#define OP_MSG(id)       0x2b, (id), 0x00, 0x00
#define OP_MOTION(m,c,f) 0x3f, (m), (c), (f)          /* Plc_motion, Form mit Modus 2: ROOM1021 sub02 @0x020C6 `3f 02 06 00` */
#define OP_KAMPF         0x04, 0xff, 0x18, RE15_AN_SUB_KAMPF
#define OP_END           0x01, 0x00

static const uint8_t k_prog[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),          /* Sperre + Balken (5090 sub04 @0x012F2/@0x012F6 = RE2 @0x00E32/@0x00E36) */
                                                /* Cut 12 bleibt (RE2 Cut 7 @0x01024); Krach im Haken (RE2 @0x01040) */
    OP_SLEEP(PAL(25)),                          /* RE2 @0x0104C `0a 19 00` */
    OP_WORK(1, 0), OP_DREHEN(AN_L_X, AN_L_Z), OP_WARTE(0x20),        /* beide sehen hin (RE2 Plc_neck @0x0105A/@0x01068) */
    OP_WORK(2, 0), OP_DREHEN_AN(AN_L_X, AN_L_Z), OP_WARTE(0x21),
    OP_SLEEP(PAL(36)), OP_SLEEP(PAL(1)),        /* RE2 @0x01072 `0a 24 00`, @0x01076 `0a 01 00` */
    OP_MSG(RE15_AN_MSG_WILLIAM),                /* "William." RE2 Xa 0x30 @0x0107A */
    OP_SLEEP(35),                               /* RE2 Wsleep @0x01084 bis Xa-Ende (11 Sektoren) */
    OP_WORK(2, 0), OP_GEHEN_AN(AN_W1_X, AN_W1_Z), OP_WARTE(0x21),   /* RE2 Strecke 1 @0x010A2 + gosub sub10 @0x010AE */
    OP_CUT(15),                                 /* RE2 Cut 2 @0x010B2: sie kommt auf die Kamera zu */
    OP_GEHEN_AN(AN_W2_X, AN_W2_Z), OP_WARTE(0x21),                  /* RE2 Strecke 2 @0x010B8 + gosub @0x010C4 */
    /* RE2 @0x010CA Member_set 0x17=1 (Birkin faellt), @0x010CE Se_on(2,0x0A). Nachbesserung 3 (M1b, PORT-WAHL): Schnitt auf
     * Cut 11 mit dem Sturz - die Landestelle L liegt hinter/links neben dem Auge von Cut 15 (dort 70 % dunkle Flaeche), Cut 11
     * zeigt den Sturz ganz vor der Tuer (Form 9,8 % des Bildes, Annette von hinten auf dem Weg); RE2 schneidet erst danach auf
     * 0x0A (@0x01102). */
    OP_CUT(11), OP_SET(5, RE15_AN_SIG_STURZ, 1),
    OP_GEHEN_AN(AN_S_X, AN_S_Z),                /* RE2 Strecke 3 @0x010DE; der Sturz (Takt, 5080 sub03) laeuft dabei (RE2 e06) */
    OP_WARTE(0x21),                             /* RE2 gosub sub10 @0x010E6 */
    /* RE2 Plc_motion 0x0C @0x010F9 (sie bleibt stehen, Waffe auf William). Port: sie dreht sich zur Form (Modus 9 wie oben,
     * Form 6030 sub08 @0x01160) - der Weg W2 -> S zeigt 41 Grad an L vorbei. */
    OP_DREHEN_AN(AN_L_X, AN_L_Z), OP_WARTE(0x21),
    OP_SLEEP(PAL(30)),                          /* RE2 @0x010FE `0a 1e 00` */
    OP_SET(5, RE15_AN_SIG_BRUELL, 1),           /* RE2 Cut 0x0A @0x01102 (Port: Cut 11 steht schon) + Se_on(2,0x0C) @0x01104 */
    OP_SLEEP(PAL(60)),                          /* RE2 @0x01110 `0a 3c 00` */
    /* RE2 Cut 0x0B @0x01114 (Annette von Williams Seite). NB3 (M1, PORT-WAHL, Lagen oben): kein 5090-Schnitt zeigt sie von dort,
     * ohne dass die Form zwischen Auge und Annette steht (Cut 15: 70 % Form, Annette 25 % frei) - der Satz bleibt in Cut 11. */
    OP_SLEEP(PAL(1)),                           /* RE2 @0x01116 `0a 01 00` */
    OP_MSG(RE15_AN_MSG_SATZ),                   /* "W-William! You're alive!" RE2 Xa 0x31 @0x0111A */
    OP_SLEEP(118),                              /* RE2 Sleep 30 @0x01132 + Wsleep @0x01136 bis Xa-Ende (37 Sektoren) */
    OP_MOTION(2, 2, 0x44),                      /* RE2 @0x01138 `3f 02 02 44`: Annette (Work steht seit OP_WORK(2,0) oben) spielt
                                                 * ihre Stand-Schleife, Clip 2 des zweiten Paars, ohne Ueberblendung, Schleife
                                                 * (Stufe 8, Herleitung bei AN_FA_STURZ) */
    OP_SET(5, RE15_AN_SIG_SCHLAG, 1),           /* RE2 @0x01142 Member_set 0x17=2 (Hieb); RE2 Cut 0x0A @0x0113C = Port Cut 11,
                                                 * steht schon */
    /* RE2 @0x01146 `0a 2d 00` = 45 Spielbilder; gemessen Schnitt 0x0A -> Schnitt 6 = 69,77 -> 72,23 s (mit Ladezeit). Port:
     * Vorlauf + Ausholen bis zum Trefferbild + RE2-Rest nach dem Treffer (AN_HIEB_*, oben) = 13 + 28 + 34 = 75 Bilder = 2,5 s. */
    OP_SLEEP(AN_HIEB_VORLAUF + AN_HIEB_TREFFER * AN_HIEB_TAKT + AN_HIEB_NACH),
    OP_CUT(13), OP_SLEEP(PAL(1)),               /* RE2 Cut 6 @0x0114A (weg von der Tat), @0x0114C `0a 01 00` */
    OP_SET(5, RE15_AN_SIG_LIEGT, 1),            /* RE2 Xa 0x32 @0x01150 (Schrei, nur Ton) + Pos_set liegend @0x01162 (Stufe 6:
                                                 * sie liegt schon, wo sie fiel - der Takt setzt nur noch Gier/Pose fest) */
    OP_SLEEP(42),                               /* RE2 Wsleep @0x01184 bis Xa-Ende (13 Sektoren) */
    OP_MSG(RE15_AN_MSG_LEON),                   /* "Leon: Annette!" (PORT-WAHL, RE2 gibt hier die Steuerung zurueck) */
    OP_SLEEP(60),
    OP_KAMPF,                                   /* Kampfstart wie bisher (sub04) */
    OP_END
};

/* ---- 6030: Zugstart nach dem Tor (Nutzer-Idee, Wortlaut PORT-WAHL) -----------------------------------------------
 * Das Programm besteht aus ausgelieferten Byte-Bereichen von ROOM6030 (rdt != 0, der Riegel unit_r41_annette vergleicht
 * jeden gegen die RDT) und wenigen Port-Befehlen in Original-Form (rdt == 0, Vorbild im Kommentar). Gesten der neuen Zeilen
 * = Gestenbloecke desselben Sprechers aus sub08 (wie Spur ST80-T, analysis/story80/TEXTE_UMSETZUNG.md Abschnitt 4). */
/* Abgang durch die Wagentuer Slot 0 (main00 @0x00C32, rect x -5100..-4200, z -24200..-22500). Die Tuer liegt in der Wand
 * SCA #13 (Rechteck x -5941..-4341, z -27900..-19610, ROOM6030 SCA @0x708): erreichbar ist die Wandflaeche x -3873 = Ziel
 * der Tuer Slot 1 (@0x00C52 `df f0`, Leons Eintritt). z -22550 = Nordteil des Tuerrechtecks, 937 von Leons Eintritt
 * (-3873,-23487), also ausserhalb der zwei Koerperradien (Fensterlauf: Stopp bei Abstand 902). PORT-WAHL. */
#define Z_TUER_X          (-3873)
#define Z_TUER_Z          (-22550)

typedef struct { const uint8_t *b; uint16_t n; uint16_t rdt; } an_seg_t;

static const uint8_t z_bit[]      = { 0x22, RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT, 0x01 };       /* Set-Form sub08 @0x0113A */
static const uint8_t z_sperre[]   = { 0x22, 0x02, 0x07, 0x01, 0x22, 0x01, 0x1b, 0x01 };     /* @0x0113E */
static const uint8_t z_emset[]    = { 0x44, 0x00, 0x4d, 0x40, 0x00, 0x00, 0x00, 0xff, 0x02, 0xf9,
                                      0x00, 0x00, 0x00, 0xa9, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00 }; /* sub07 @0x010F8 */
static const uint8_t z_kopf[]     = { 0x09, 0x0a, 0x01, 0x00, 0x29, 0x01, 0x2e, 0x01, 0x00, 0x00, 0x40, 0x00, 0x09, 0x20,
                                      0x02, 0xf9, 0x00, 0xa9, 0x09, 0x0a, 0x0a, 0x00 };   /* @0x01146: Sleep 1, Cut 1, Leon dreht sich */
static const uint8_t z_work_an[]  = { 0x2e, 0x02, 0x00, 0x00 };                             /* @0x011CC Work_set(2,0) */
static const uint8_t z_pult[]     = { 0x29, 0x03, 0x3f, 0x00, 0x12, 0x00, 0x09, 0x0a, 0x50, 0x00 };   /* @0x011F6 Cut 3, Pult, Sleep 80 */
/* Runde 42 Strang finale, Stufe 2 (Zugfilm): der fruehere Port-Befehl z_sig_an `22 05 10 01` (Signal -> ANNETTE_SE Satz 3 =
 * RE2 Leon ROOM7020 snd0 Satz 0x0C, EDT @0x5C90, Satzbytes `00 00 93 00` @0x5CC0, Aufruf MAIN sub04 @0x014AA `36 02 0c 00`) ist
 * entfallen: der Zugfilm des Nutzers spielt GENAU diesen Satz selbst bei Film-Bild 18 (Ton-Beleg des Films
 * zug600/film/r1/ton_belege.md: "hochfahren" = ROOM7020 snd0 Satz 0x0C @0x5CC0, VAG 14, 12 736 B), 1 s nach dem Pultlicht.
 * Beide zusammen = derselbe Ton zweimal (Pruefberichte des Films R1/R2 Punkt "Einbau-Hinweis"). Ohne den Port-Befehl ist der
 * Pult-Abschnitt wieder bytegleich zum Original (sub08 @0x011F6..@0x01223 hat keinen Ton). */
/* Zugfilm (Nutzer 2026-11-17 "Ja, die Zugfahrt ist grosse Klasse!", Einbau nach dem Fahrpult, vor "There! The train's moving."):
 * Port-Zeile ohne Text `2b 3c 00 00` = Message_on 60 in der Form der Pult-Zeile sub08 @0x01200 `2b 0e 00 00` (die das Port-
 * Programm wie bisher auslaesst). Haken re15_zws_r41_message_on (Tabelle k_filme "zug600"): Film R600ZUG.STR, das Skript parkt,
 * danach 15 Bilder Einblenden aus Schwarz wie RE2 (re15_zws_r41.h). Id 60: ROOM6030 belegt 0..20 (RDT), 21..26, 27..30, 31, 40
 * (VERTRAG Runde 42 Abschnitt 2.2) - 60 ist frei. */
static const uint8_t z_film[]     = { 0x2b, RE15_ZUG600_MSG_FILM, 0x00, 0x00 };             /* Form sub08 @0x01200 */
/* Musik unter dem Zugfilm (Strang finale Stufe 2 Runde 2, Pruefbericht s2r1 Rot 1: die Raummusik MAIN2A lief unter dem Film
 * weiter, die Abgabe ist in Bild 1-17 stumm). Vorbild RE2 Leon, Film mitten in einer laufenden Szene mit Musik:
 *   ROOM1080 sub14 @0x02524 `51 00 04 00 01 41` (MAIN anhalten), @0x02530 `6f 09` Movie_on, @0x02532 Sleep 1 (das Bild, in dem
 *     FUN_8002b968 @0x8002ba28 den Film spielt), @0x02536 `51 00 03 00 80 41` (MAIN weiter);
 *   ROOM4090 @0x027FA `51 02 04 00 64 40` (SUB anhalten), @0x02804 `51 00 04 01 33 41` (MAIN anhalten), @0x02818 `6f 0b`,
 *     @0x02824 `51 02 03 00 64 40` (SUB weiter), @0x0282B `51 00 03 01 33 41` (MAIN weiter) = genau die Spuren, die laufen.
 *   RE2-Handler @0x80057e20 -> FUN_8005b6f0: Fall 4 `jal SsSeqPause` @0x8005b938, Fall 3 `jal SsSeqReplay` @0x8005b900.
 *   RE1.5 hat denselben Befehl als 0x54 (Handler -> FUN_80044da4): Fall 4 `jal SsSeqPause` @0x80044efc, Fall 3 SsSeqSetVol +
 *   `jal SsSeqReplay` @0x80044ec4. Der RE2-Filmspieler FUN_80032340 fasst den Sequenzer nicht an (nur DsMix FUN_800333c4).
 * Beim Zugstart ((3,89)=1) laeuft in ROOM6030 NUR MAIN: sub06 @0x0108E Ck(3,89)=1 -> @0x01092 `54 00 01 00 00 00`, sub07 @0x01128
 * -> @0x0112C; den SUB (Slot 1) spielen nur die (3,89)=0-Zweige @0x01060 -> @0x01064 und @0x01118 -> @0x0111C (Strom aus), und
 * der Raumwechsel ueber den Bahnsteig (ROOM6000 Eintrag FF65 = kein SUB) hat ihn beendet. Ein Op 3 auf Slot 1 wuerde ihn erst
 * einschalten (gemessen Runde 2 Lauf 1: nach dem Film -20 dB statt -32 dB) -> nur Slot 0. SUB SEQ#2 von SUB_16.BGM hat keine Note.
 * Form = Original-Zeile @0x01092 mit Op 4 / Op 3 statt 1. Lautstaerke-/Pan-Byte 0 = keine Schreibung (RE2 @0x8005b978 /
 * @0x8005b9a4 `beq` bei 0), PORT-WAHL: die RE2-Werte (0 / 0x7F) wuerden die Raum-Lautstaerke von MAIN2A ueberschreiben. Kein Sleep
 * zwischen Film und Weiter-Befehl: die Port-Zeile 60 parkt das Skript bis zum Filmende, das ersetzt RE2s Sleep 1 @0x02532. */
static const uint8_t z_musik_halt[]  = { 0x54, 0x00, 0x04, 0x00, 0x00, 0x00 };               /* Form sub06 @0x01092, Op 4 */
static const uint8_t z_musik_weiter[]= { 0x54, 0x00, 0x03, 0x00, 0x00, 0x00 };               /* Form sub06 @0x01092, Op 3 */
/* Pult: sub08 @0x01210..@0x01223 wechselt Cut 4 (Pult dunkel) und Cut 5 (rotes WARNING) = der gescheiterte Start "Not enough
 * power!". Fuer den gelungenen Start nur Cut 4 (PORT-WAHL, Fensterlauf: Cut 5 zeigt WARNING): Set(1,28,1) + Cut 4 + Sleep 15
 * @0x01210, Sleep 15 @0x01216, Set(1,28,0) @0x01220 - Original-Bytes ohne `29 05`. */
static const uint8_t z_licht[]    = { 0x22, 0x01, 0x1c, 0x01, 0x29, 0x04, 0x09, 0x0a, 0x0f, 0x00 };   /* @0x01210 */
static const uint8_t z_schlaf15[] = { 0x09, 0x0a, 0x0f, 0x00 };                                       /* @0x01216 */
static const uint8_t z_licht_aus[]= { 0x22, 0x01, 0x1c, 0x00 };                                       /* @0x01220 */
static const uint8_t z_cut1[]     = { 0x29, 0x01, 0x09, 0x0a, 0x3c, 0x00 };                 /* @0x01254 Cut 1, Sleep 60 */
static const uint8_t z_fahrt[]    = { 0x04, 0xff, 0x18, 0x05 };                             /* sub07 @0x01132 Licht-Schleife */
static const uint8_t z_m27[]      = { 0x2b, RE15_AN_MSG_FAEHRT, 0x00, 0x00 };              /* Form @0x01296 */
static const uint8_t z_geste16[]  = { 0x3f, 0x00, 0x09, 0x00, 0x09, 0x0a, 0x19, 0x00, 0x3f, 0x00, 0x09, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x19, 0x00, 0x3f, 0x00, 0x0b, 0x00, 0x09, 0x0a, 0x32, 0x00,
                                      0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 };   /* @0x0129A Annette nach msg 16 */
static const uint8_t z_sig_krach[]= { 0x22, 0x05, RE15_AN_SIG_KRACH, 0x01 };               /* Port: Krach (RE2 @0x01040) */
/* WARNING beim Krachen (Strang finale Stufe 2, Vorschlag A7 "Fuehrerstand 603: WARNING beim Krachen", Nutzer 2026-11-17 zum
 * Loch "Loch ist einfach da": beim Anfahren kracht es, im Fuehrerstand blinkt WARNING; das Loch selbst ist seit Stufe 4 wieder
 * entfernt, Nutzerentscheid 18.11. "Finden wir keinen Weg, muss es halt raus", re15_zug_r42 geloescht). Original-Bytes des gescheiterten Starts
 * ROOM6030 sub08 @0x01224..@0x0123F unveraendert: Set(1,29,1) @0x01224, For 3 @0x01228 { Cut 4 @0x0122E (Pult dunkel),
 * Sleep 15 @0x01230, Cut 5 @0x01234 (rotes WARNING, BSS ROOM603 Bild 5; Kamera 5 @0x0100 = Kamera 4 @0x00E0), Sleep 15
 * @0x01236 } Next @0x0123A, Set(1,29,0) @0x0123C = drei Wechsel 4/5 in 90 Bildern. Danach zurueck auf Cut 1 = sub08 @0x01254
 * `29 01` (die Kamera, in der die Szene weiterlaeuft). Bank 1 Bit 29 ist im Port ungebunden (game_state.c, Zone 1 = DAT_800aca3c). */
static const uint8_t z_warnung[]  = { 0x22, 0x01, 0x1d, 0x01, 0x0d, 0x00, 0x1a, 0x00, 0x03, 0x00, 0x29, 0x04, 0x09, 0x0a,
                                      0x0f, 0x00, 0x29, 0x05, 0x09, 0x0a, 0x0f, 0x00, 0x0e, 0x00, 0x22, 0x01, 0x1d, 0x00 }; /* @0x01224 */
static const uint8_t z_cut1_w[]   = { 0x29, 0x01 };                                         /* sub08 @0x01254 */
static const uint8_t z_schlaf25[] = { 0x09, 0x0a, 0x19, 0x00 };                             /* RE2 @0x0104C `0a 19 00` */
static const uint8_t z_work_leon[]= { 0x2e, 0x01, 0x00, 0x00 };                             /* @0x0125A Work_set(1,0) */
static const uint8_t z_dreh_leon[]= { 0x40, 0x00, 0x09, 0x20, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form @0x01150 */
static const uint8_t z_warte20[]  = { 0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x20, 0x00 }; /* @0x01358 */
static const uint8_t z_dreh_an[]  = { 0x40, 0x00, 0x09, 0x21, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form @0x01160 */
static const uint8_t z_warte21[]  = { 0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x21, 0x00 }; /* @0x0128A */
static const uint8_t z_schlaf36[] = { 0x09, 0x0a, 0x24, 0x00 };                             /* RE2 @0x01072 `0a 24 00` */
static const uint8_t z_m28[]      = { 0x2b, RE15_AN_MSG_WAS, 0x00, 0x00 };
static const uint8_t z_geste11[]  = { 0x3f, 0x00, 0x0d, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x0d, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x01178 */
static const uint8_t z_m29[]      = { 0x2b, RE15_AN_MSG_PRUEFE, 0x00, 0x00 };
static const uint8_t z_geste15[]  = { 0x3f, 0x00, 0x04, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x04, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x07, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x01262 Leon */
static const uint8_t z_work_an2[] = { 0x2e, 0x02, 0x00, 0x00 };                             /* @0x012E2 Work_set(2,0) */
static const uint8_t z_m30[]      = { 0x2b, RE15_AN_MSG_BLEIB, 0x00, 0x00 };
static const uint8_t z_geste18[]  = { 0x3f, 0x00, 0x0e, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x0e, 0x00, 0x43, 0x00,
                                      0x80, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x10, 0x00, 0x09, 0x0a, 0x14, 0x00 }; /* @0x012EA */
static const uint8_t z_gehen[]    = { 0x40, 0x00, 0x04, 0x21, LE16(Z_TUER_X), LE16(Z_TUER_Z) };   /* Form 6000 sub02 @0x00FFA */
static const uint8_t z_weg[]      = { 0x32, 0x00, 0xf5, 0xd6, 0xf0, 0xf1, 0xc6, 0x21 };     /* 6000 sub02 @0x0100A (versteckt) */
/* Nachbesserung 3 (Abnahme 1 M2: sie ging in Cut 1 auf die Kamera zu und verschwand gross im Vordergrund). Das Auge von Cut 1
 * steht in der Tuer selbst (RID 6030 Cut 1 (-5886,-3132,-23058), Tuerrechteck x -5100..-4200), wer zur Tuer geht, laeuft also
 * in die Kamera; am Tuerziel waren Kopf und Rumpf noch im Bild (Projektion Bild-x 94..105, Fuesse unter dem Rand). PORT-WAHL:
 * fuer den Abgang Cut 0 (Auge (-3510,-4356,-19512) im Norden, die Tuer rechts im Bild, Eintrittskamera von Slot 1), sie geht zur
 * Tuer; im Ankunftsbild Schnitt zurueck auf Cut 1 und im selben Bild versteckt (Cut 1 schaut aus der Tuer in den Raum, sie
 * ist hinter der Kamera hinaus). Original-Bytes: Cut 0 = sub08 @0x01336 `29 00`, Cut 1 = sub08 @0x0114A `29 01`. */
static const uint8_t z_cut0[]     = { 0x29, 0x00 };                                         /* sub08 @0x01336 */
static const uint8_t z_cut1_ab[]  = { 0x29, 0x01 };                                         /* sub08 @0x0114A */
/* Nachbesserung 4 (Abnahme 2 N4: "sie bleibt vor Leon stehen, die Tuer bewegt sich nicht, kein Tuerton").
 * (a) Leon tritt zur Seite: er steht an der Tuer (Eintritt Slot 1 (-3873,-23487)); am Tuerziel stand Annette zwischen Auge von
 *     Cut 0 und ihm (ID-Puffer Cut 0: Leon 46 % frei, gemessen = Abnahme F1490). PORT-WAHL Z_LEON (-3000,-23700): 898 Schritt,
 *     Leon und Annette in Cut 0 je 100 % frei, 1444 von ihrem Tuerziel, >= 1100 von ihrem Weg (zwei Koerperradien 902;
 *     Werkzeug AN_belege/werkzeug/n4/zug6030.py). Form: Work_set(1,0) + Plc_dest Modus 4 mit Ankunftsbit 0x20 = ROOM6031
 *     @0x00D86 `2e 01 00 00 40 00 04 20`; Warten auf seine Ankunft (Form @0x01358), dann dreht er sich zur Tuer (z_dreh_leon,
 *     Form @0x01150, ohne Warten) und erst danach geht sie los - nacheinander, weil die Ankunftsbits (5,0x20)/(5,0x21)
 *     Ein-Bild-Pulse sind (Bank 5 Wort 1 je Bild gewischt, FUN_8003ebf4) und ein zweites Warten einen frueheren Puls
 *     verpassen wuerde.
 * (b) Tuerton: kein RE1.5-Raum laesst ein NPC mit Ton oder Tuerbewegung durch eine Tuer gehen (Zensus aller 240 RDTs: die fuenf
 *     Abgaenge 10D0 sub21, 2000 sub02/03, 4010 sub02, 6000 sub02 sind stumm; ROOM4010 sub02 @0x00D1A..@0x00D2A = gehen,
 *     warten, Cut 1, Sleep 20, Pos_set weg). RE2 hat genau einen: Claire ROOM4081 MAIN sub05 @0x03624..@0x03658 (Annette
 *     laeuft in die Tuer): Se_on Tuerbank Satz 0 (oeffnen) `36 00 00 01 03 00 ..` @0x03634, Sleep 30 @0x03640, Satz 1
 *     (zufallen) @0x03644, Pos_set weg. Port: Tuerbank = Port-Bank 0x0E (RE15_CUT10F0_SE_BANK, audio_pc.c ->
 *     re15_audio_re2_tuer_se), Ursprung RE1.5-Modus 2 = Entity-Pool Index 0 (Handler @0x80041624, @0x80041698-b0; Annette ist
 *     Sce_em_set-Slot 0, z_emset) statt RE2-Modus 3. Die Tuerbank ist DOOR2A (alle Zugtueren 5090/6030, gen/tuer_zuordnung.inc
 *     S320/S321), geladen beim Szenenstart (re15_annette_r41_tuerbank_hook).
 * (c) Tuerbewegung: ROOM6030 hat kein Tuermodell (nOmodel 2: Objekt 0 main00 @0x00CEE bei (-17800,-25900), Objekt 1 sub06
 *     @0x010A2 bei (-26230,-20148)); das Tuernetz gibt es nur in der Tuersequenz (DOOR2A.DO2). RE1.5 und RE2 bewegen bei einem
 *     NPC-Abgang nie eine Tuer im Raum -> keine Tuerbewegung (Beleg statt Erfindung). Sie geht durch die Tuer, waehrend der
 *     Schnitt auf Cut 1 (Auge in der Tuer) fuehrt: Satz 0 im Ankunftsbild, Satz 1 nach Sleep 30 wie RE2. */
#define Z_LEON_X          (-3000)
#define Z_LEON_Z          (-23700)
static const uint8_t z_leon_seite[]= { 0x2e, 0x01, 0x00, 0x00, 0x40, 0x00, 0x04, 0x20, LE16(Z_LEON_X), LE16(Z_LEON_Z) };   /* Form 6031 @0x00D86 */
static const uint8_t z_tuer_auf[] = { 0x36, 0x0e, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };   /* RE2 4081 @0x03634 */
static const uint8_t z_tuer_zu[]  = { 0x36, 0x0e, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };   /* RE2 4081 @0x03644 */
static const uint8_t z_schlaf30[] = { 0x09, 0x0a, 0x1e, 0x00 };                             /* @0x01364 (= RE2 4081 @0x03640) */
static const uint8_t z_schluss[]  = { 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x3c, 0x01,
                                      0x42, 0x00 };                                       /* @0x01368 Sperren auf, Cut_auto, Plc_ret */
static const uint8_t z_ende[]     = { 0x01, 0x00 };                                         /* @0x01374 Evt_end */

#define SEG(a, off) { a, (uint16_t)sizeof a, (uint16_t)(off) }
static const an_seg_t k_zug_seg[] = {
    SEG(z_bit, 0),          SEG(z_sperre, 0x0113E), SEG(z_emset, 0x010F8),  SEG(z_kopf, 0x01146),
    SEG(z_work_an, 0x011CC), SEG(z_pult, 0x011F6),   SEG(z_licht, 0x01210),
    SEG(z_schlaf15, 0x01216), SEG(z_licht_aus, 0x01220), SEG(z_musik_halt, 0), SEG(z_film, 0), SEG(z_musik_weiter, 0),
    SEG(z_cut1, 0x01254),   SEG(z_fahrt, 0x01132),  SEG(z_work_an2, 0x012E2), SEG(z_m27, 0),
    SEG(z_geste16, 0x0129A), SEG(z_sig_krach, 0),    SEG(z_warnung, 0x01224), SEG(z_cut1_w, 0x01254), SEG(z_schlaf25, 0),
    SEG(z_work_leon, 0x0125A), SEG(z_dreh_leon, 0),  SEG(z_warte20, 0x01358),
    SEG(z_work_an2, 0x012E2), SEG(z_dreh_an, 0),     SEG(z_warte21, 0x0128A), SEG(z_schlaf36, 0),
    SEG(z_m28, 0),           SEG(z_geste11, 0x01178),
    SEG(z_work_leon, 0x0125A), SEG(z_m29, 0),        SEG(z_geste15, 0x01262),
    SEG(z_work_an2, 0x012E2), SEG(z_m30, 0),         SEG(z_geste18, 0x012EA),
    SEG(z_cut0, 0x01336),    SEG(z_leon_seite, 0),   SEG(z_warte20, 0x01358),
    SEG(z_dreh_leon, 0),
    SEG(z_work_an2, 0x012E2), SEG(z_gehen, 0),        SEG(z_warte21, 0x0128A),
    SEG(z_tuer_auf, 0),      SEG(z_cut1_ab, 0x0114A), SEG(z_weg, 0),
    SEG(z_schlaf30, 0x01364), SEG(z_tuer_zu, 0),
    SEG(z_work_leon, 0x0125A), SEG(z_schluss, 0x01368), SEG(z_ende, 0x01374),
};
#define AN_N_ZUG_SEG ((int)(sizeof k_zug_seg / sizeof k_zug_seg[0]))

/* Wagen-Eintritt (sub06 @0x010C4 Evt_exec sub05) solange der Zugstart nicht lief = der Zug steht noch. Beide Wagentueren
 * wie im Original vor dem Start, also wie im (3,89)=0-Zweig von sub06:
 *   Slot 2 (Ausgang zur Bahnsteighalle ROOM6000) ist wieder die Tuer aus main00 @0x00C72 (Door_aot_set, 32 Byte). Der
 *     (3,89)=1-Zweig hat sie davor mit @0x01098 `46 02 01 31 14 00` auf msg 20 "You can't get off the train right now."
 *     gestellt; der (3,89)=0-Zweig (@0x0105C..@0x01088) fasst Slot 2 nicht an. Nachbesserung 2 (Orchestrator-Entscheid):
 *     msg 20 erst, wenn der Zug faehrt - nach dem Zugstart laeuft sub06 unveraendert und @0x01098 gilt.
 *   Slot 3 (nach 5090) zeigt den Original-Text msg 10 "I have to get this thing moving first!" = sub06 @0x0106A.
 * Die Licht-Schleife sub05 ("Fahrt") startet erst nach dem Zugstart. */
static const uint8_t k_tuer[] = { 0x3b, 0x02, 0x02, 0x31, 0x00, 0x00, 0xf4, 0xb6, 0xc0, 0xae, 0xdc, 0x05, 0xb0, 0x04,
                                  0x9d, 0x06, 0xf0, 0xf1, 0xba, 0xd8, 0x00, 0x0c, 0x05, 0x00, 0x03, 0x02, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00,                                      /* main00 @0x00C72 */
                                  0x46, 0x03, 0x01, 0x31, 0x0a, 0x00, 0xff, 0xff, 0x00, 0x00,   /* sub06 @0x0106A */
                                  0x01, 0x00 };                                                /* sub06 @0x010CA */

static uint8_t s_zug_prog[512];
static int     s_zug_len = 0;

static void zug_bauen(void)
{
    if (s_zug_len) return;
    int n = 0;
    for (int i = 0; i < AN_N_ZUG_SEG; i++) {
        if (n + k_zug_seg[i].n > (int)sizeof s_zug_prog) { n = 0; break; }
        memcpy(s_zug_prog + n, k_zug_seg[i].b, k_zug_seg[i].n);
        n += k_zug_seg[i].n;
    }
    s_zug_len = n;
}

/* ---- Nachrichten -------------------------------------------------------------------------------------------------- */
static const struct { uint16_t raum; uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { 0x5090, RE15_AN_MSG_SATZ,    k_an_5090_msg40, (uint8_t)sizeof k_an_5090_msg40 },
    { 0x5090, RE15_AN_MSG_LEON,    k_an_5090_msg42, (uint8_t)sizeof k_an_5090_msg42 },
    { 0x5090, RE15_AN_MSG_WILLIAM, k_an_5090_msg43, (uint8_t)sizeof k_an_5090_msg43 },
    { 0x6030, RE15_AN_MSG_FAEHRT,  k_an_6030_msg27, (uint8_t)sizeof k_an_6030_msg27 },
    { 0x6030, RE15_AN_MSG_WAS,     k_an_6030_msg28, (uint8_t)sizeof k_an_6030_msg28 },
    { 0x6030, RE15_AN_MSG_PRUEFE,  k_an_6030_msg29, (uint8_t)sizeof k_an_6030_msg29 },
    { 0x6030, RE15_AN_MSG_BLEIB,   k_an_6030_msg30, (uint8_t)sizeof k_an_6030_msg30 },
};
#define AN_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

/* ---- Zustand ------------------------------------------------------------------------------------------------------ */
static struct {
    int phase, t;
    int form, frau;               /* Slots, -1 = keiner */
    int luft;                     /* Add_speed-Schritte des Sturzes (5080 sub03 For 14) */
    int blut;                     /* Blut-Aufrufe FUN_801058BC (Testzaehler) */
    int blut_plaetze;             /* davon belegte ESP-Plaetze (Zeilen) */
    int blut_rest;                /* +0x16B: Blut-Bilder uebrig (@0x80104D10..60) */
    int blut_textur;              /* Textur mit Blut geladen */
    int treffer;                  /* Hieb hat getroffen */
    int zug;                      /* Zugstart-Programme seit Raumeintritt */
    int toene[RE15_AN_SE_N];
    int schrei;
    unsigned raum;
    int32_t vx0, vz0, vx1, vz1;   /* Vorstoss beim Hieb: Wurzel vorher / nachher (AN_VORSTOSS) */
    int blut_skala;               /* Skala der Blutzeilen (AN_BLUT_RE2_*), im Trefferbild bestimmt */
    int16_t wg0;                  /* Stufe 6: Gier im Trefferbild */
    int wt;                       /* Stufe 6: Takte seit dem Treffer (Sturz), -1 = kein Sturz im Bild */
} s_an = { RE15_AN_AUS, 0, -1, -1, 0, 0, 0, 0, 0, 0, 0, { 0 }, 0, 0, 0, 0, 0, 0, AN_BLUT_SKALA, 0, -1 };

static void (*s_modell_fn)(uint8_t) = 0;
static void (*s_se_fn)(int, int) = 0;
static void (*s_bank_fn)(int) = 0;
static void (*s_satz_fn)(int) = 0;
static void (*s_stimme_fn)(uint16_t, int) = 0;
void re15_annette_r41_modell_hook(void (*fn)(uint8_t type)) { s_modell_fn = fn; }
void re15_annette_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int)) { s_se_fn = se_fn; s_bank_fn = bank_fn; }
void re15_annette_r41_se_hook(void (*fn)(int satz)) { s_satz_fn = fn; }
void re15_annette_r41_stimme_hook(void (*fn)(uint16_t room, int voice_id)) { s_stimme_fn = fn; }
static int (*s_tuerbank_fn)(void) = 0;
void re15_annette_r41_tuerbank_hook(int (*fn)(void)) { s_tuerbank_fn = fn; }

static void ton(int satz)
{
    if (satz < 0 || satz >= RE15_AN_SE_N) return;
    s_an.toene[satz]++;
    if (s_satz_fn) s_satz_fn(satz);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[annette] F%u Ton %d (ANNETTE_SE.VBS)\n", (unsigned)g_engine.frame_count, satz);
#endif
}

/* Runde 42 Story STAGE6 (E9, re15_story6_r42.h): Elzas Zugstart in 6031 spielt dieselben Saetze wie Leons Zugstart. */
void re15_annette_r41_ton_spielen(int satz) { ton(satz); }

void re15_annette_r41_reset(void)
{
    s_an.phase = RE15_AN_AUS; s_an.t = 0; s_an.form = s_an.frau = -1;
    s_an.luft = 0; s_an.blut = 0; s_an.blut_plaetze = 0; s_an.blut_rest = 0; s_an.blut_textur = 0; s_an.treffer = 0;
    s_an.zug = 0; memset(s_an.toene, 0, sizeof s_an.toene); s_an.schrei = 0;
    s_an.vx0 = s_an.vz0 = s_an.vx1 = s_an.vz1 = 0; s_an.blut_skala = AN_BLUT_SKALA;
    s_an.wg0 = 0; s_an.wt = -1;
}

int re15_annette_r41_phase(void) { return s_an.phase; }
int re15_annette_r41_slot(int form) { return form ? s_an.form : s_an.frau; }
int re15_annette_r41_blut(void) { return s_an.blut; }
int re15_annette_r41_blut_plaetze(void) { return s_an.blut_plaetze; }
int re15_annette_r41_blut_skala(void) { return s_an.blut_skala; }
int re15_annette_r41_zug_szene(void) { return s_an.zug; }
int re15_annette_r41_ton(int satz) { return (satz >= 0 && satz < RE15_AN_SE_N) ? s_an.toene[satz] : 0; }
int re15_annette_r41_schrei(void) { return s_an.schrei; }

const uint8_t *re15_annette_r41_programm(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_prog;
    return k_prog;
}

const uint8_t *re15_annette_r41_programm_zug(int *out_len)
{
    zug_bauen();
    if (out_len) *out_len = s_zug_len;
    return s_zug_len ? s_zug_prog : NULL;
}

const uint8_t *re15_annette_r41_programm_tuer(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_tuer;
    return k_tuer;
}

/* Fuer den Riegel: Segment i des Zugstart-Programms (rdt = Datei-Offset in ROOM6030.RDT, 0 = Port-Befehl). */
int re15_annette_r41_zug_segment(int i, const uint8_t **b, int *n, int *rdt);
int re15_annette_r41_zug_segment(int i, const uint8_t **b, int *n, int *rdt)
{
    if (i < 0 || i >= AN_N_ZUG_SEG) return 0;
    if (b) *b = k_zug_seg[i].b;
    if (n) *n = k_zug_seg[i].n;
    if (rdt) *rdt = k_zug_seg[i].rdt;
    return 1;
}

const uint8_t *re15_annette_r41_meldung_raum(uint16_t room_id, int msg_id, int *out_len)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++)
        if (k_meldungen[i].raum == room_id && (int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

const uint8_t *re15_annette_r41_meldung(int msg_id, int *out_len)
{
    return re15_annette_r41_meldung_raum(RE15_AN_RAUM, msg_id, out_len);
}

static void protokoll(const char *was)
{
#ifdef RE15_PLATFORM_PC
    const re15_actor_t *f = s_an.form >= 0 ? &g_actors[s_an.form] : NULL;
    const re15_actor_t *a = s_an.frau >= 0 ? &g_actors[s_an.frau] : NULL;
    fprintf(stderr, "[annette] F%u %s: Phase %d, Form %s(%d,%d,%d) Gier %d Clip %d/%d, Annette %s(%d,%d,%d) Clip %d/%d, Cut %d\n",
            (unsigned)g_engine.frame_count, was, s_an.phase,
            f ? "" : "-", f ? (int)f->x : 0, f ? (int)f->y : 0, f ? (int)f->z : 0, f ? (int)f->rot_y : 0,
            f ? (int)f->motion : -1, f ? (int)f->anim_frame : -1,
            a ? "" : "-", a ? (int)a->x : 0, a ? (int)a->y : 0, a ? (int)a->z : 0, a ? (int)a->motion : -1,
            a ? (int)a->anim_frame : -1, (int)g_scd.work_vars[0x0A]);
#else
    (void)was;
#endif
}

/* ---- Bedingungen ---------------------------------------------------------------------------------------------------- */
static int raum_ist(uint16_t room_id) { return room_id == RE15_AN_RAUM; }

static int zug_gesehen(void) { return re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT) != 0; }

static int annette_steht_da(void)
{
    /* PORT-WAHL nach Nutzerentscheid: im Wagen erst nach dem Tor (3,89) UND nach der Zugstart-Szene im Fuehrerstand
     * (9,133); nach dem Kampfstart (3,42) nicht mehr (sie ist dann tot). */
    return re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) && zug_gesehen() &&
           !re15_game_flag_get(RE15_AN_KAMPF_BANK, RE15_AN_KAMPF_BIT);
}

/* Nachbesserung 3 (Abnahme 1 M8): RE2 legt Annette bei spaeteren Besuchen liegend hin (Claire ROOM6141 MAIN sub00 else-Zweig
 * @0x00D16 Sce_em_set Typ 0x49 auf (-12300,900,-15300), Richtung 0x0CE4, Bewegung 0x18 = dieselbe Lage wie Pos_set @0x01162).
 * Kein neues Bit: (3,42) schreiben nur ROOM5090 sub01 @0x0129A `22 03 2a 01` (Leon) und ROOM5091 @0x0127E (Elza) - Byte-Suche
 * ueber alle 240 RDTs. In 5090 steht Annette beim Ausloeser genau dann im Wagen, wenn (3,89) und (9,133) gelten und (3,42)
 * noch 0 ist (annette_steht_da), und Evt_exec sub04 @0x01296 laeuft dann in ihre Szene, bevor @0x0129A (3,42) setzt. Also:
 * (3,89) und (9,133) und (3,42) in 5090 <=> die Szene ist gelaufen und Annette ist tot. */
static int leiche_da(void)
{
    return re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) && zug_gesehen() &&
           re15_game_flag_get(RE15_AN_KAMPF_BANK, RE15_AN_KAMPF_BIT);
}

int re15_annette_r41_spawn_weg(uint16_t room_id, uint8_t type)
{
    if (!raum_ist(room_id) || type != RE15_AN_TYP) return 0;
    const int weg = !annette_steht_da() && !leiche_da();
#ifdef RE15_PLATFORM_PC
    if (weg) fprintf(stderr, "[annette] ROOM%04X: Frau im Kittel (0x4D, @0x0122A) verworfen: (3,89)=%d (9,133)=%d (3,42)=%d\n",
                     room_id, re15_game_flag_get(3, 89), re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT),
                     re15_game_flag_get(3, 42));
#endif
    return weg;
}

static int slot_vom_typ(uint8_t type)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == type) return i;
    return -1;
}

static int16_t blick(const re15_actor_t *e, int32_t tx, int32_t tz)
{
    /* dieselbe Peilung wie re15_enemy_steer_point / birkin_formen.c bf_blick */
    return (int16_t)(((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff);
}

static void clip(re15_actor_t *e, int c)
{
    e->motion = (int16_t)c; e->anim_frame = 0; e->anim_frac = 7;
    e->anim_flags &= (uint16_t)~0x80u;
}

/* Bild vorruecken, am Ende das letzte Bild halten; Bildton der Form wie bb_bild_ton. Rueckgabe 1 am Clip-Ende. */
static int vor(re15_actor_t *e, int halten)
{
    if (e->type == RE15_AN_FORM_TYP) {
        const re15_enemy_bank_t *b = re15_enemy_find(e->type);
        if (b && b->ok) {
            const uint32_t w = re15_emd_get_frame_entry(&b->anim, (int)e->motion, (int)e->anim_frame);
            if ((w & AN_TON_BIT) && s_se_fn) { if (s_bank_fn) s_bank_fn(AN_TON_BANK); s_se_fn((int)(w >> 28), 0); }
        }
    }
    if (halten && e->anim_frame + 1 >= re15_r38_birkin_fc(e)) return 1;
    return re15_r38_birkin_anim(e);
}

/* Ein Bild Blut von FUN_80104C24 (@0x80104D18-60): je ein FUN_801058BC-Aufruf an den beiden aeusseren Klauen, Lage = Welt-
 * lage des Knochens in diesem Bild (Einheitsanker 0x80095F68 + a3 = Teil-T @0x80105950-78), Winkel = Peilung Form -> Spieler
 * (FUN_80014E4C @0x80105908 = Leon FUN_800154AC; Port-Zwilling wie re2z_bearing_to), Boden = Aktor-y wie
 * re2z_gore_fx_core. Rueckgabe der Zeilen-Spawns = belegte Plaetze (Pool voll: das Original verliert den Aufruf,
 * @0x8001B98C-98 `j 0x8001bc1c` / `addiu v0,zero,0xff`). */
static void blut_bild(const re15_actor_t *f)
{
    static const int k_klaue[2] = { AN_BLUT_KLAUE_A, AN_BLUT_KLAUE_B };   /* Reihenfolge @0x80104D38, @0x80104D4C */
    if (s_an.blut_rest <= 0) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int16_t w = blick(f, pl->x, pl->z);
    for (int i = 0; i < 2; i++) {
        int32_t p[3];
        re15_enemy_bone_world_pos(f, k_klaue[i], p);
        s_an.blut++;
        s_an.blut_plaetze += re15_esp_fx_spawn_rows(re15_esp_room_bank(), AN_BLUT_EFFEKT, AN_BLUT_SUB, (uint16_t)s_an.blut_skala,
                                                    p[0], p[1], p[2], f->y, w);
    }
    s_an.blut_rest--;                                    /* @0x80104D54-60 `addiu v0,v0,-1` / `sb v0,0x16b(s0)` */
#ifdef RE15_PLATFORM_PC
    if (s_an.blut_rest == AN_BLUT_BILDER - 1 || s_an.blut_rest == 0)
        fprintf(stderr, "[annette] F%u Blut %d/%d: %d Aufrufe, %d Plaetze, Pool %d/%d\n", (unsigned)g_engine.frame_count,
                AN_BLUT_BILDER - s_an.blut_rest, AN_BLUT_BILDER, s_an.blut, s_an.blut_plaetze, re15_esp_fx_count(),
                RE15_ESP_FX_MAX);
#endif
}

static void meldungen_einsetzen(uint16_t room_id)
{
    for (int i = 0; i < AN_N_MELDUNGEN; i++) {
        if (k_meldungen[i].raum != room_id) continue;
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

void re15_annette_r41_install(uint16_t room_id)
{
    /* NB3 M8: auch beim Neuaufbau desselben Raums (Laden) nach dem Szenenende neu aufsetzen, damit die Leiche gelegt wird */
    if (s_an.raum != room_id || s_an.phase >= RE15_AN_FREI) { re15_annette_r41_reset(); s_an.raum = room_id; }
    if (raum_ist(room_id) || room_id == RE15_AN_ZUG_RAUM) meldungen_einsetzen(room_id);
}

/* ---- 6030 ----------------------------------------------------------------------------------------------------------- */
static const uint8_t *zug_evt_exec(uint8_t sub_id, const uint8_t *target)
{
    if (sub_id != RE15_AN_ZUG_SUB) return target;
    if (!re15_game_flag_get(RE15_AN_TOR_BANK, RE15_AN_TOR_BIT) || zug_gesehen()) return target;
    /* sub00 @0x00D28 Switch(Cut) Case 0 -> sub07 (Fuehrerstand), Case 6/8/12 -> sub06 (Wagen). */
    if (g_scd.work_vars[0x0A] != 0) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] ROOM6030 Wagen (Cut %d): Zugstart nicht gelaufen -> Ausgang = Tuer (main00 @0x00C72), "
                "Tuer nach 5090 = msg 10 (sub06 @0x0106A)\n",
                (int)g_scd.work_vars[0x0A]);
#endif
        return k_tuer;
    }
    /* Erster Besuch erst nach dem Tor: sub07 @0x010D4 hat sie schon gesetzt und sub08 laeuft -> Zugstart beim naechsten
     * Eintritt (sub07 spawnt sie ab (4,23)=1 und (3,89)=1 nicht mehr, @0x010F4). */
    if (slot_vom_typ(RE15_AN_TYP) >= 0) return target;
    /* Integration A2 (Weg F gemessen: vorne zuerst -> Wagen -> ohne Rueckkehr -> Tor): solange der Sherry-Nachtrag der Spur
     * ST80 offen ist ((9,128)=1, gesetzt k_s80_6030_sub08_vorne), weiss Annette nichts von Sherry - msg 28 "...Sherry is back
     * there!" setzt msg 25 voraus. Der Haken story80 (vor diesem in op_evt_exec) leitet dann dieselbe Stelle sub07 @0x01132 auf
     * den Nachtrag um (k_s80_6030_sub05_nachtrag_tor); der Zugstart folgt beim naechsten Betreten, wie nach sub08 oben. */
    if (re15_game_flag_get(RE15_S80_BIT_BANK, RE15_S80_BIT_NACHTRAG)) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] ROOM6030 Zugstart wartet: Sherry-Nachtrag offen (9,%d)=1\n", RE15_S80_BIT_NACHTRAG);
#endif
        return target;
    }
    zug_bauen();
    if (!s_zug_len) return target;
    s_an.zug++;
    /* NB4 N4: Tuerbank DOOR2A fuer den Abgang (z_tuer_auf/z_tuer_zu). Nach dem Weg durch die Wagentuer liegt sie schon in der
     * Bank (Tuersequenz DOOR2A, gleiche Bytes -> kein Neuladen, audio_pc.c memcmp); beim Debug-Sprung laedt sie diese Zeile. */
    const int tb = s_tuerbank_fn ? s_tuerbank_fn() : 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[annette] F%u ROOM6030 Zugstart-Szene statt sub05 (sub07 @0x01132), %d Byte, Tuerbank DOOR2A %s\n",
            (unsigned)g_engine.frame_count, s_zug_len, tb ? "geladen" : "NICHT geladen");
#else
    (void)tb;
#endif
    return s_zug_prog;
}

static void zug_tick(void)
{
    if (re15_game_flag_get(5, RE15_AN_SIG_ZUG_AN)) { re15_game_flag_set(5, RE15_AN_SIG_ZUG_AN, 0); ton(RE15_AN_SE_ZUG_AN); }
    if (re15_game_flag_get(5, RE15_AN_SIG_KRACH))  { re15_game_flag_set(5, RE15_AN_SIG_KRACH, 0);  ton(RE15_AN_SE_KRACH); }
}

/* ---- 5090 ----------------------------------------------------------------------------------------------------------- */
/* Nachbesserung 6 (Abnahme 4 M1, Entscheid Orchestrator nach "Original oder nicht"): Koerper der 0x4D je Raum/Phase.
 *   - Bis zum Start der RE2-Szene ist sie RE1.5-Inhalt und traegt ihren RE1.5-Kasten {0,-1530,0,450,1530,450}, den
 *     re15_enemy_apply_hitbox beim Sce_em_set setzt (STAGE5 INIT `lw v0,2012(v0)` @0x8011d88c -> 0x801207d0 / `sw v0,120(v1)`
 *     @0x8011d894; STAGE6 @0x80101958 -> 0x80102784 / @0x80101960). Das gilt in 6000, 6030 (auch in der Port-Zugstart-Szene
 *     NB1, die in einem RE1.5-Raum mit RE1.5-Skriptsegmenten laeuft) und in 5090 vor dem Ausloeser.
 *   - Ab dem Start der RE2-Szene (re15_annette_r41_evt_exec: Evt_exec sub04 @0x01296 laeuft in das Programm nach Claire
 *     ROOM6141 MAIN sub02 ab @0x01024) und als Leiche (Wiedereintritt, ruhe_setzen = RE2 MAIN sub00 @0x00D16) ist sie die
 *     RE2-Annette mit dem RE2-Koerpersegment: RE2-Schub FUN_80034D0C (info/re2leon/PSX.EXE; Claire 0x80032904) liest die
 *     Segmentzahl `lw v0,488(s6)` (+0x1E8) @0x80034d74, die Segmente ab `addiu s5,s6,132` (+0x84) @0x80034d78, den Radius
 *     `lhu s7,22(s5)` (+0x16) @0x80034dac und die Halbhoehe `lhu v0,26(s5)` (+0x1A) @0x80034e5c. Claire-RAM ROOM6141 lauf3,
 *     Annette @0x80125d4c (em[0] @0x800c79b0): schon im ersten Abzug der Szene e01 (Skript @0x010B0 in sub02) und bis e30
 *     lebend wie liegend +0x1E8 = 1, +0x14 = -1350, +0x16 = 350, +0x1A = 1350 (AN_belege/abn4/claire_annette_ram.txt,
 *     re2_claire/ram_reihe_voll.md Zeile e01) -> r 350, h 1350, Versatz -1350. */
#define AN_RE2_SEG_R   350      /* +0x16 @0x80034dac, Claire-RAM lauf3 e01..e30 */
#define AN_RE2_SEG_HH  1350     /* +0x1A @0x80034e5c, Claire-RAM lauf3 e01..e30 */
#define AN_RE2_SEG_DY  (-1350)  /* +0x14 Segmentmitte = Lage - 1350, Claire-RAM lauf3 e01..e22 (y 0: Mitte -1350) */
static void re2_segment(re15_actor_t *a)
{
    a->hit_radius_min = a->hit_radius_max = AN_RE2_SEG_R;
    a->hit_height = AN_RE2_SEG_HH;
    a->hit_offset_x = 0; a->hit_offset_y = AN_RE2_SEG_DY; a->hit_offset_z = 0;
}

const uint8_t *re15_annette_r41_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target)
{
    if (room_id == RE15_AN_ZUG_RAUM) return zug_evt_exec(sub_id, target);
    if (!raum_ist(room_id) || sub_id != RE15_AN_SUB_KAMPF) return target;
    if (s_an.phase == RE15_AN_LEICHE) {                  /* das eigene Programm ruft den Kampfstart: Form frei */
        s_an.phase = RE15_AN_FREI;
        protokoll("Kampfstart (sub04), Form frei");
        return target;
    }
    if (s_an.phase != RE15_AN_AUS || !annette_steht_da()) return target;
    s_an.form = slot_vom_typ(RE15_AN_FORM_TYP);
    s_an.frau = slot_vom_typ(RE15_AN_TYP);
    if (s_an.form < 0 || s_an.frau < 0 || !re15_bb_bank_da(&g_actors[s_an.form])) {
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[annette] keine Szene: Form-Slot %d (Riesenform %d), Annette-Slot %d\n", s_an.form,
                s_an.form >= 0 ? re15_bb_bank_da(&g_actors[s_an.form]) : 0, s_an.frau);
#endif
        s_an.form = s_an.frau = -1;
        return target;
    }
    for (unsigned b = RE15_AN_SIG_SCHLAG; b <= RE15_AN_SIG_BRUELL; b++) re15_game_flag_set(5, b, 0);
    s_an.phase = RE15_AN_WARTEN; s_an.t = 0; s_an.blut = 0; s_an.blut_plaetze = 0; s_an.blut_rest = 0; s_an.luft = 0;
    s_an.treffer = 0;
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    f->no_draw = 1;                                      /* bis zum Sturz unsichtbar (RE2: geparkt auf y -9000, @0x00C82) */
    a->x = AN_A0_X; a->y = 0; a->z = AN_A0_Z; a->rot_x = a->rot_z = 0;   /* PORT-WAHL A0 (s. oben); vorher in Cut 13 nicht im Bild */
    a->rot_y = blick(a, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
    re2_segment(a);                                      /* NB6: ab hier RE2-Annette (Claire-RAM lauf3 e01 @0x010B0 sub02) */
    ton(RE15_AN_SE_KRACH);                               /* RE2 @0x01040 Se_on(3,0x0B) */
    protokoll("Szenenstart statt sub04 (Krach)");
    return k_prog;
}

int re15_annette_r41_besitzt(int slot)
{
    if (s_an.phase == RE15_AN_AUS || slot < 0) return 0;
    /* Stufe 8: bis zum Trefferbild fuehrt sie der Executor (Plc_dest, dann Plc_motion @0x01138 = Sub 2 @0x80050f00, der einzige
     * Stepper @0x80050f94); die Szene uebernimmt sie in treffer(). RUHE = Leiche. */
    if (slot == s_an.frau) return s_an.phase >= RE15_AN_REAKTION;
    return slot == s_an.form && s_an.phase < RE15_AN_FREI;
}

/* Nachbesserung 4 (Abnahme 2 N3, state.log F2751: Annette hp -1 -> -6, st 0 -> 3, ss1 3 = Waffe 3, ss2 1 = EBEN): der
 * Waffen-Resolver re15_player_weapon_fire waehlte die Leiche als naechstes Ziel im Schusskeil (Band LEVEL wie jedes NPC,
 * RE1.5-RAM mzd_stage1_npc.sav: NPC 0x40/0x42/0x4B Wort 0 = 0x40000001) vor der G5 dahinter. RE2 verwirft Kandidaten mit
 * hp < 0, fuer die Pistole in FUN_800410CC (`lh v0,342(s2)` @0x80041280, `bltz v0,0x80041300` @0x80041288), fuer Messer und
 * Bolzen in FUN_800470C0 (`lh v0,0x156(s0)` @0x80047148, `bltz v0,0x8004740c` @0x80047150; info/re2leon/PSX.EXE und SLUS_007.48
 * gleich; NB5 nach Abnahme 3 H1), und die liegende
 * RE2-Annette traegt hp -1 (RAM lauf3 e24: +0x156 = -1). Die Leiche ist RE2-Inhalt (RE1.5 hat keine): RE2-Regel.
 * NB5 (Nutzerentscheid 2026-11-03 "Mache die RE 2 Variante, das der Schuss durch begleiter durch geht."): dieselbe RE2-Regel
 * gilt jetzt im Resolver fuer jeden Begleiter 0x40..0x4D mit hp < 0, auch fuer die lebende Annette; dieser Haken ist damit
 * ein Teilfall und bleibt als Zustandsabfrage (Leiche gelegt) fuer die Szene und test_r41_annette. */
int re15_annette_r41_kein_ziel(const void *actor)
{
    if (s_an.frau < 0 || actor != (const void *)&g_actors[s_an.frau]) return 0;
    return (s_an.treffer || s_an.phase == RE15_AN_RUHE) && g_actors[s_an.frau].hp < 0;
}

int re15_annette_r41_volle_drehung(const void *actor)
{
    return s_an.frau >= 0 && actor == (const void *)&g_actors[s_an.frau] && s_an.phase >= RE15_AN_LEICHE;
}

const char *re15_annette_r41_emd_datei(uint8_t type)
{
    if (type != RE15_AN_TYP) return NULL;                /* 0x4D steht nur in 5090/6000/6030 (Leon) */
    /* NB3 M8: beim Wiedereintritt (Szene gelaufen) mit dem blutigen Kittel wie nach dem Treffer */
    const int tot = raum_ist((uint16_t)g_current_room_id) && s_an.phase == RE15_AN_AUS && leiche_da();
    return (s_an.blut_textur || tot) ? "ANNETTE_4D_BLUT.EMD" : "ANNETTE_4D.EMD";
}

/* Hieb ausrichten: die Gier waehlen, bei der die linke Hand im Trefferbild ihr am naechsten kommt (gemessen am Gerippe). */
static void hieb_ausrichten(re15_actor_t *f, const re15_actor_t *a)
{
    const int16_t basis = blick(f, a->x, a->z);
    const int16_t m0 = f->motion; const int32_t b0 = f->anim_frame;
    int16_t beste = basis; int64_t best = -1;
    f->motion = AN_FC_HIEB; f->anim_frame = AN_HIEB_TREFFER + 1;
    for (int d = -0x100; d <= 0x100; d += 0x10) {
        int32_t h[3];
        f->rot_y = (int16_t)((basis + d) & 0x0fff);
        re15_enemy_bone_world_pos(f, AN_HAND_LINKS, h);
        const int64_t dx = h[0] - a->x, dz = h[2] - a->z, q = dx * dx + dz * dz;
        if (best < 0 || q < best) { best = q; beste = f->rot_y; }
    }
    f->rot_y = beste; f->motion = m0; f->anim_frame = b0;
}

/* NB3 M5: Skala der Blutzeilen fuer RE2-Bildgroesse (AN_BLUT_RE2_*): Auge und H des angezeigten Schnitts, Abstand zur Klaue 14
 * im Trefferbild. Ohne Raumdaten (Unit-Test ohne RID) bleibt die RE2-Skala. */
static int blut_skala(const re15_actor_t *f)
{
    const int cut = (int)g_scd.work_vars[0x0A];
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return AN_BLUT_SKALA;
    const re15_camera_cut_t *c = &g_room_rdt.cuts[cut];
    const int h = (int)(c->fov >> 7);
    int32_t k[3];
    re15_enemy_bone_world_pos(f, AN_BLUT_KLAUE_A, k);
    const int64_t dx = (int64_t)k[0] - c->pos_x, dy = (int64_t)k[1] - c->pos_y, dz = (int64_t)k[2] - c->pos_z;
    const int64_t q = dx * dx + dy * dy + dz * dz;
    const int64_t d = (int64_t)re15_squareroot0((uint32_t)(q > 0xffffffffLL ? 0xffffffffLL : q));
    if (h <= 0 || d <= 0) return AN_BLUT_SKALA;
    int64_t sk = (int64_t)AN_BLUT_SKALA * AN_BLUT_RE2_H * d / ((int64_t)h * AN_BLUT_RE2_ABSTAND);
    if (sk < 1) sk = 1;
    if (sk > 0xffff) sk = 0xffff;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[annette] F%u Blut-Skala 0x%04X: Cut %d H %d, Klaue 14 (%d,%d,%d) %d vom Auge (RE2 0x2000 bei H 257 / 2878)\n",
            (unsigned)g_engine.frame_count, (unsigned)sk, cut, h, (int)k[0], (int)k[1], (int)k[2], (int)d);
#endif
    return (int)sk;
}

static void treffer(re15_actor_t *f, re15_actor_t *a)
{
    s_an.blut_skala = blut_skala(f);
    s_an.blut_rest = AN_BLUT_BILDER;                     /* @0x80104D10-14 (Blut ab diesem Bild, blut_bild im Takt) */
    /* Stufe 8: erst hier uebernimmt der Takt (bis hierher Executor-Sub 2, Stand-Schleife); die Flags 0x44 des Plc_motion gehen
     * mit dem Executor (vor Stufe 8 standen sie auf 0: Plc_dest `sh zero,452(a1)` @0x80041c4c). */
    a->state = 0; a->sub_state_1 = 0; a->sub_state_2 = 0; a->walk_active = 0; a->anim_flags = 0;
    clip(a, AN_FA_STURZ);                                /* NB3 M4: sackt sofort zusammen (Clip 6 bis zum Bodenbild 6, Stufe 6) */
    s_an.wg0 = a->rot_y; s_an.wt = 0;                    /* Stufe 6: Sturz ab hier (sturz_schritt, AN_STURZ_BILDER) */
    a->hp = -1;                                          /* NPC-Familie: unverwundbar, kein Treffer-Kandidat (re15_damage.c) */
    s_an.blut_textur = 1;
    if (s_modell_fn) s_modell_fn(RE15_AN_TYP);           /* Kittel mit Blut (ANNETTE_4D_BLUT.EMD = RE2 EM049-Textur) */
    s_an.treffer = 1;
    protokoll("Treffer");
}

/* Peilung von (x0,z0) nach (x1,z1) als Gier (0 = +X) - dieselbe Formel wie blick() (re15_actor.h re15_atan2_q12). */
static int peilung(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    return ((int)re15_atan2_q12(z1 - z0, x1 - x0) - 0x400) & 0x0fff;
}

/* Stufe 6: Gier der Liegenden = Blick vom Haltepunkt S auf die Landestelle L (die Gier im Trefferbild, blick() beim Hieb-Signal)
 * plus AN_LIEGEN_DREH (oben). Fest, damit Szene und Wiedereintritt dieselbe Gier haben. */
static int16_t liegen_gier(void)
{
    return (int16_t)((peilung(AN_S_X, AN_S_Z, AN_L_X, AN_L_Z) + AN_LIEGEN_DREH) & 0x0fff);
}

/* Liegepose halten: RE2-Clip 0x18 (Clip 24) Bild 0, y +900, ungekippt. Lage und Gier bleiben. */
static void leiche_halten(re15_actor_t *a)
{
    a->motion = AN_FA_LIEGEN; a->anim_frame = 0; a->anim_frac = 0;
    a->anim_flags &= (uint16_t)~0x80u;
    a->rot_x = 0; a->rot_z = 0;
    a->y = AN_LIEGEN_Y;
}

/* Liegen ohne Sturz im Bild (Schnitt weg vor dem Trefferbild, RE2-Fall @0x01162 bei weggeschnittener Kamera): an Ort und Stelle,
 * Gier liegen_gier(). Die Lage bleibt (Stufe 6: kein Pos_set mehr). */
static void liegen(re15_actor_t *a)
{
    a->rot_y = liegen_gier();
    leiche_halten(a);
}

/* Stufe 6: ein Takt des Sturzes nach dem Treffer (Kopf von AN_FA_STURZ_BODEN). Clip 6 laeuft bis zum Bodenbild (ein Bild je
 * Takt, die Szene ist der einzige Stepper, player_common.c), das Bodenbild steht einen Takt; dann Clip 24 Bild 0 (RE2-Liegepose,
 * Plc_motion @0x01172 `3f 00 18 04`) mit der Original-Ueberblendung +0x8f = 7 (@0x80050038). RE2 legt sie dazu auf y 900
 * (@0x01162 `84 03`); damit die gezeichnete Wurzel dabei nicht springt, wird die gemerkte Wurzel der Ueberblendung (prev_root,
 * Modellraum, skeleton_common.c) um denselben Betrag zurueckgenommen (PORT-WAHL: Welt-y = Aktor-y + Wurzel bleibt stetig).
 * Danach haelt sie Bild 0 und die Ueberblendung laeuft aus (je Takt 1 ab wie @0x8001f5b0-b4). Gier: gleichmaessig auf dem kurzen
 * Bogen von der Gier im Trefferbild zu liegen_gier() ueber AN_STURZ_BILDER Takte; die Lage bleibt. */
static void sturz_schritt(re15_actor_t *a)
{
    if (s_an.wt < 0) return;
    if (a->motion == AN_FA_STURZ) {
        if ((int)a->anim_frame < AN_FA_STURZ_BODEN) vor(a, 0);
        else {
            a->motion = AN_FA_LIEGEN; a->anim_frame = 0; a->anim_frac = AN_UEBERBLEND;
            a->anim_flags &= (uint16_t)~0x80u;
            a->rot_x = 0; a->rot_z = 0;
            a->prev_root[1] -= (AN_LIEGEN_Y - a->y);
            a->y = AN_LIEGEN_Y;
            protokoll("Bodenbild, Liegepose (Clip 24, Ueberblendung 7)");
        }
    } else if (a->motion == AN_FA_LIEGEN) {
        a->anim_frame = 0;
        if (a->anim_frac > 0) a->anim_frac--;            /* @0x8001f5b0-b4: je gezeichnetem Bild 1 ab */
    }
    if (s_an.wt < AN_STURZ_BILDER) {
        s_an.wt++;
        const int dg = ((((int)liegen_gier() - (int)s_an.wg0) + 2048) & 0x0fff) - 2048;
        a->rot_y = (int16_t)(((int)s_an.wg0 + dg * s_an.wt / AN_STURZ_BILDER) & 0x0fff);
        if (s_an.wt == AN_STURZ_BILDER) protokoll("Sturz fertig, liegt");
    }
}

/* NB3 M8: Wiedereintritt nach der Szene - die 0x4D (sub00 @0x0122A) liegt als Leiche dort, wo die Szene sie hingelegt hat (RE2
 * MAIN sub00 @0x00D16 setzt dieselbe Lage und denselben Clip 0x18 wie der Pos_set/Plc_motion der Szene; Stufe 6: an S mit
 * liegen_gier(), die Szene laesst sie an der Trefferlage = Ankunft an S liegen). */
static void ruhe_setzen(void)
{
    const int as = slot_vom_typ(RE15_AN_TYP);
    if (as < 0) return;
    re15_actor_t *a = &g_actors[as];
    a->state = 0; a->sub_state_1 = 0; a->sub_state_2 = 0; a->walk_active = 0;
    a->hp = -1;                                          /* wie nach dem Treffer: kein Treffer-Kandidat (re15_annette_r41_kein_ziel) */
    re2_segment(a);                                      /* NB6: Leiche = RE2-Segment (Claire-RAM lauf3 e23..e30) */
    a->x = AN_S_X; a->z = AN_S_Z;                        /* Stufe 6: wo sie gefallen ist - Haltepunkt S (AN_LIEGEN_DREH) */
    liegen(a);
    s_an.frau = as; s_an.form = -1; s_an.blut_textur = 1;
    s_an.phase = RE15_AN_RUHE;
    protokoll("Wiedereintritt nach der Szene: Leiche liegt (RE2 sub00 @0x00D16)");
}

void re15_annette_r41_tick(void)
{
    if ((uint16_t)g_current_room_id == RE15_AN_ZUG_RAUM) { zug_tick(); return; }
    if (s_an.phase == RE15_AN_AUS) {
        if (raum_ist((uint16_t)g_current_room_id) && leiche_da()) ruhe_setzen();
        return;
    }
    if (!raum_ist((uint16_t)g_current_room_id)) { re15_annette_r41_reset(); return; }
    if (s_an.phase == RE15_AN_RUHE) {
        if (s_an.frau >= 0 && g_actors[s_an.frau].active) leiche_halten(&g_actors[s_an.frau]);
        return;
    }
    if (s_an.form < 0 || s_an.frau < 0) return;
    re15_actor_t *f = &g_actors[s_an.form], *a = &g_actors[s_an.frau];
    if (!f->active || !a->active) {
        s_an.phase = RE15_AN_FREI;
        /* Runde 42 Strang finale: nach dem Abschuss ist die Form fort (finale_r42.c), die Leiche bleibt liegen wie in RE15_AN_FREI
         * (Clip 24 Bild 0 gehalten, PORT-WAHL "Halten statt Schleife" oben). Vorher lief hier ab dem Kill die Liegeschleife an
         * (gemessen integration_r41_annette_g5: F1315 af=1, F1316 af=2). */
        if (a->active && s_an.frau >= 0) leiche_halten(a);
        return;
    }
    s_an.t++;
    if (s_an.phase < RE15_AN_FREI && re15_game_flag_get(5, RE15_AN_SIG_BRUELL)) {
        re15_game_flag_set(5, RE15_AN_SIG_BRUELL, 0); ton(RE15_AN_SE_BRUELL);
    }
    switch (s_an.phase) {
    case RE15_AN_WARTEN:
        f->no_draw = 1;
        if (re15_game_flag_get(5, RE15_AN_SIG_STURZ)) {
            re15_game_flag_set(5, RE15_AN_SIG_STURZ, 0);
            f->no_draw = 0; f->rot_x = f->rot_z = 0;
            f->x = AN_L_X; f->y = AN_STURZ_Y; f->z = AN_L_Z;          /* 5080 sub03 @0x00822 Pos_set */
            f->rot_y = (int16_t)(((int)re15_atan2_q12(AN_S_Z - AN_L_Z, AN_S_X - AN_L_X) - 0x400) & 0x0fff);
            clip(f, AN_FC_STEHEN);                       /* 5080: faellt im Stehen-Clip vom Dach (re15_bb_wartet) */
            ton(RE15_AN_SE_STURZ);                       /* RE2 @0x010CE Se_on(2,0x0A) */
            s_an.phase = RE15_AN_STURZ; s_an.t = 0; s_an.luft = 0;
            protokoll("Sturz");
        }
        break;
    case RE15_AN_STURZ:
        vor(f, 0);
        if (s_an.t > AN_STURZ_HALT && s_an.luft < AN_STURZ_N) {   /* @0x0082A Sleep 1, dann je Bild Add_speed (+400) */
            f->y += AN_STURZ_V; s_an.luft++;
        }
        if (s_an.luft >= AN_STURZ_N) {                   /* 14 x 400 = gelandet (y -5600 -> 0) */
            f->y = 0;
            clip(f, AN_FC_DROHEN);
            s_an.phase = RE15_AN_DROHEN; s_an.t = 0;
            protokoll("gelandet, Drohen");
        }
        break;
    case RE15_AN_DROHEN:
        if (f->motion == AN_FC_DROHEN) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); }
        else vor(f, 0);
        if (re15_game_flag_get(5, RE15_AN_SIG_SCHLAG)) {
            re15_game_flag_set(5, RE15_AN_SIG_SCHLAG, 0);
            /* Stufe 8: Annette bleibt beim Executor (Plc_motion @0x01138 im selben Skriptbild: state 4 Sub 2, Stand-Schleife ab
             * Bild 0); vorher uebernahm der Takt hier und setzte Clip 4 = die Flugphase des Sprungs (@0x80052d0c). */
            a->rot_y = blick(a, f->x, f->z);
            {   /* NB3: Vorstoss AN_VORSTOSS auf Annette zu (RE2 e20..e22); die Gier des Hiebs gilt fuer die Lage danach */
                int32_t ux = a->x - f->x, uz = a->z - f->z;
                int32_t l = (int32_t)re15_squareroot0((uint32_t)(ux * ux + uz * uz)); if (l < 1) l = 1;
                s_an.vx0 = f->x; s_an.vz0 = f->z;
                s_an.vx1 = f->x + (int32_t)((int64_t)ux * AN_VORSTOSS / l);
                s_an.vz1 = f->z + (int32_t)((int64_t)uz * AN_VORSTOSS / l);
                f->x = s_an.vx1; f->z = s_an.vz1;
                hieb_ausrichten(f, a);
                f->x = s_an.vx0; f->z = s_an.vz0;
            }
            s_an.phase = RE15_AN_HIEB; s_an.t = 0;
            protokoll("Hieb (RE2 @0x01142)");
        }
        break;
    case RE15_AN_HIEB:
        /* Stufe 8: kein vor(a) - Annettes Bildzaehler fuehrt bis zum Treffer der Executor (re15_npc_sub_motion, Schleife) */
        if (s_an.t <= AN_HIEB_VORLAUF) {                 /* Vorlauf: die Form steht noch (RE2 69,77 -> 70,22 s) */
            if (f->motion == AN_FC_DROHEN) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
            if (s_an.t == AN_HIEB_VORLAUF) { clip(f, AN_FC_HIEB); protokoll("Ausholen (RE2 70,22 s)"); }
        } else {
            const int k = s_an.t - AN_HIEB_VORLAUF, n = AN_HIEB_TREFFER * AN_HIEB_TAKT;
            if (!s_an.treffer && f->motion == AN_FC_HIEB && (int)f->anim_frame == AN_HIEB_TREFFER) {
                treffer(f, a); s_an.phase = RE15_AN_REAKTION; s_an.t = 0;
            }
            if ((int)f->anim_frame < AN_HIEB_TREFFER) { if (k % AN_HIEB_TAKT == 0) vor(f, 1); }   /* Ausholen, 2 Takte je Bild */
            else vor(f, 1);
            if (k <= n) {                                /* Vorstoss gleichmaessig ueber die Ausholtakte */
                f->x = s_an.vx0 + (int32_t)((int64_t)(s_an.vx1 - s_an.vx0) * k / n);
                f->z = s_an.vz0 + (int32_t)((int64_t)(s_an.vz1 - s_an.vz0) * k / n);
            }
        }
        if (re15_game_flag_get(5, RE15_AN_SIG_LIEGT)) {   /* Schnitt weg vor dem Trefferbild: Treffer nachholen */
            if (!s_an.treffer) treffer(f, a);
            s_an.phase = RE15_AN_REAKTION; s_an.t = 0;
        }
        break;
    case RE15_AN_REAKTION:
        if (s_an.wt >= 0) sturz_schritt(a);              /* Stufe 6: zu Boden und liegen bleiben (AN_FA_STURZ_BODEN) */
        else vor(a, 1);
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        if (re15_game_flag_get(5, RE15_AN_SIG_LIEGT)) {
            re15_game_flag_set(5, RE15_AN_SIG_LIEGT, 0);
            liegen(a);                                   /* Stufe 6: an Ort und Stelle (nach dem Sturz schon so) */
            s_an.schrei++;
            if (s_stimme_fn) s_stimme_fn(RE15_AN_RAUM, RE15_AN_MSG_SCHREI);   /* nur Ton, kein Untertitel (Nutzer F9) */
            s_an.phase = RE15_AN_LEICHE; s_an.t = 0;
            protokoll("Schnitt weg, liegt, Schrei (RE2 @0x01150/@0x01162)");
        }
        break;
    case RE15_AN_LEICHE:
        if (f->motion == AN_FC_HIEB) { if (vor(f, 1)) clip(f, AN_FC_STEHEN); } else vor(f, 0);
        f->rot_y = blick(f, a->x, a->z);
        /* fallthrough: Leiche halten */
    case RE15_AN_FREI:
    default:
        leiche_halten(a);
        break;
    }
    /* Blut wie FUN_80104C24: nach dem Bildvorschub (@0x80104C9C-A8) und dem Trefferbild (@0x80104CD4-14) je Bild an den
     * Klauen, 15 Bilder lang, unabhaengig vom Schnitt (Routine der Form, nicht des Skripts). */
    if (s_an.phase >= RE15_AN_HIEB) blut_bild(f);
}
