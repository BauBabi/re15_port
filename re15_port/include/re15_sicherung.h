/*
 * RE1.5 Rebuilt — die SICHERUNG (Item 0x40 "Fuse") im Hebetisch von Irons' Buero.
 *
 * ⛔ WARUM ES DIESE DATEI GIBT (Befund 2026-09-27, Nutzer-Auftrag):
 * Das Sicherungs-Raetsel in ROOM1050 ist im Auslieferungsstand herausgeschnitten, die
 * KUNST dafuer aber vollstaendig vorhanden. Belege, alle nachmessbar:
 *   - ROOM1050.RDT Kameratabelle @0x60: Cut 7 (+7*32) und Cut 8 (+8*32) tragen die
 *     IDENTISCHE Kamera (flag=0 fov=26684 pos=(16171,-2733,-8185)
 *     tgt=(18240,-1974,-8706)) und unterscheiden sich nur im pri_offset (0x518/0x51C)
 *     = zwei Zustaende EINES Blicks.
 *   - Die Hintergruende unterscheiden sich in 2317 Pixeln, alle im Sicherungskasten:
 *     Cut 7 rechter Sockel leer + rote Leuchte, Cut 8 Sicherung drin + beide gruen.
 *   - msg 2 "I need a fuse to run the shutter." (Block @0xE44) wird NIE aufgerufen;
 *     im ganzen SCD stehen nur Message_on 0 @0xCAC und Message_on 8 @0xDD6.
 *   - Kein Cut_chg 7/8 im SCD — beide Blicke sind unerreichbar.
 *   - Item 0x40 hat Name (DEBUG.BIN @0x800C495C), Inventar-Icon (ITEMALL.PIX Tile
 *     0x40) und Item-Bild (ITPS.ITP @0x40*0x3000) — aber NULL Item_aot_set im
 *     gesamten Spiel (Zensus ueber alle 240 RDTs).
 *
 * Die FUNDSTELLE ist der Hebetisch in Irons' Buero (ROOM1150/1151 Prop 0, sub04
 * @Datei 0x0F96-0x10B6 in ROOM1150.RDT; ROOM1151.RDT traegt dieselben Records um 0x22
 * frueher, sub04 dort @0x0F74) — und der passt, weil er im Auslieferungsstand keinen
 * Zweck hat. Der Ablauf, Record fuer Record (Datei-Offsets ROOM1150.RDT):
 *   Pos_set    @0x0FB4  32 00 24 af cf fe cc bb   Plattform aus der Parklage auf y = -305
 *   For        @0x0FC0  0d 00 18 00 0f 00         15 Durchlaeufe (0x18 ist die BLOCKLAENGE,
 *                                                 der Zaehler ist das dritte Feld 0x0F —
 *                                                 For-Handler @0x8003f540: `lh t1,2(t0)`
 *                                                 @0x8003f564 = Laenge, `lhu a1,4(t0)`
 *                                                 @0x8003f568 = Zaehler)
 *   Speed_set  @0x0FCA  2f 02 0a 00 / @0x0FD4 2f 02 f6 ff    Deckelhaelften je +-10
 *                                                 -> Deckelweg +-150 (NICHT 240)
 *   Sleep 30   @0x0FDE
 *   Speed_set  @0x0FF2  2f 01 f6 ff, For @0x0FF6 0d 00 04 00 5b 00   91 Schritte a -10
 *                                                 -> Hochpunkt y = -305 - 910 = -1215
 *   Speed_set  @0x100C  2f 01 01 00, For @0x1010 0d 00 04 00 0a 00   10 Schritte a +1
 *                                                 -> y = -1205, dort steht sie
 *   Sleep 30   @0x101A, Sleep 10 @0x102E          — und faehrt wieder herunter, ohne dass
 *                                                 etwas geschieht
 *   Pos_set    @0x109E  32 00 24 af 00 b1 cc bb   Parklage NACH der Szene y = -20224
 *                                                 (vor der Szene parkt main00 sie auf -20324)
 * Genau in diese Fahrt gehoert die Beute.
 *
 * ⛔ KEIN ASSET-PATCH: die ausgelieferten RDTs bleiben byte-true (dieselbe Linie wie
 * der uebrige schlafende Content, scd_room_setup.c:342). Das Welt-Modell kommt als
 * zusaetzliches Prop portseitig dazu — ROOM1150 hat nOmodel=4, der Pool fasst 17.
 * Dasselbe gilt fuer Item-Bild und Icon: sie werden im GELADENEN Puffer eingesetzt
 * (re15_sicherung_bild_einsetzen / _icon_einsetzen), ITPS.ITP und ITEMALL.PIX auf der
 * Platte bleiben unberuehrt.
 */
#ifndef RE15_SICHERUNG_H
#define RE15_SICHERUNG_H

#include <stdint.h>

/* Item-Id "Fuse" aus der Namenstabelle (DEBUG.BIN Offsettabelle @0x800C495C, Eintrag
 * 0x40; Gegenprobe ueber die Nachbarn 0x3F "Pocket Watch" / 0x41 "Spark Plug"). */
#define RE15_SICHERUNG_ITEM      0x40

/* Zone-9-Bit "Sicherung genommen". NICHT gewaehlt, sondern aus einem Zensus ueber alle
 * 240 RDTs bestimmt: 85 der 256 Bits sind belegt (164 Item_aot_set-Nutzlasten plus alle
 * Ck/Set mit bank=9). Der groesste zusammenhaengende freie Block ist 53..84; 53 ist
 * dessen Anfang und liegt weit von jedem belegten Cluster. */
#define RE15_SICHERUNG_TAKEN_BIT 53

/* obj_id des zusaetzlichen Props. ROOM1150/1151 belegen 0..3 (nOmodel=4), 4 ist frei —
 * und RE15_TIM_SLOT_PROP(4) = 8 ist ebenfalls unbenutzt. */
#define RE15_SICHERUNG_OBJ_ID    4

/* SITZ UND DREHUNG — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE.
 * Das Original hat im Hebetisch keinen Gegenstand; fuer Sitz und Drehung gibt es deshalb
 * keine Instruktion, die man zitieren koennte. Jede der vier Zahlen ist aus der
 * AUSGELIEFERTEN GEOMETRIE des Tisches abgeleitet (Runde 30, Dossier
 * analysis/befunde_runde30/sicherung.md §3.6/§5.3), nicht geschaetzt:
 *
 * Der Tisch (Prop 0, MD1 @Datei 0x11E40 in ROOM1150.RDT, Punktliste @0x11E84) traegt
 * unter der zweiteiligen Kuppel ein Fach. Sein Boden ist ein Achteck aus den Vierecken
 * 79/80/81 (Face-Records @0x12E40/@0x12E50/@0x12E60), alle acht Punkte auf y = -1036:
 *     Punkt 101 @0x121AC ( -74,-1036,1260)     Punkt 109 @0x121EC (-485,-1036,1260)
 *     Punkt 103 @0x121BC (-128,-1036,1562)     Punkt 111 @0x121FC (-432,-1036, 958)
 *     Punkt 105 @0x121CC (-280,-1036,1645)     Punkt 113 @0x1220C (-280,-1036, 875)
 *     Punkt 107 @0x121DC (-432,-1036,1562)     Punkt 115 @0x1221C (-128,-1036, 958)
 * (ROOM1151.RDT: dieselben Werte, Punktliste @0x13EFC, die acht Punkte @0x14224..@0x14294.)
 * Die Deckelhaelften (Prop 1 MD1 @0x138D4 z[1260..1645], Prop 2 MD1 @0x13B88
 * z[875..1260]) stossen bei z = 1260 aneinander.
 *
 *   POS_X = -280   Mitte des Fachbodens: Punkte 105 und 113 tragen x = -280
 *                  (Rand -485 @0x121EC / -74 @0x121AC)
 *   POS_Y = -1062  Fachboden y = -1036 minus Rohrradius 26 (MD1 der Sicherung,
 *                  gen/sicherung_prop.inc, y[-26..26]) — sie liegt AUF dem Boden
 *   POS_Z = 1260   Naht der Deckelhaelften = Mitte des Fachbodens: Punkte 101 und 109
 *                  tragen z = 1260
 *   ROT_Y = 1024   Viertelkreis (4096 = 360 Grad): die Laengsachse X des Modells liegt
 *                  damit entlang Z der Plattform = entlang der LANGEN Seite des Fachs
 *                  (770 in z gegen 411 in x) und quer zur Blickrichtung von Cut 4
 *
 * GEMESSEN im echten Spiel (Framedump, Differenz MIT/OHNE Prop, Punkte bei 320x240):
 * dieser Sitz traegt in den Stichbildern F110..F220 211..439 Punkte und 0 bei F100
 * (Deckel zu); der fruehere Sitz (-628,-927,784) mit Drehung 0 lag AUSSERHALB des Fachs
 * hinter der Kuppel, Laengsachse auf die Kamera gerichtet, und trug 12..124 Punkte.
 * Eingefroren von unit_r30_sicherung_sitz (liest den Fachboden aus beiden RDTs). */
#define RE15_SICHERUNG_POS_X  (-280)
#define RE15_SICHERUNG_POS_Y  (-1062)
#define RE15_SICHERUNG_POS_Z  (1260)
#define RE15_SICHERUNG_ROT_Y  (1024)

/* Legt das Prop beim Raumstart an — falls der Raum ROOM1150/1151 ist UND die Sicherung
 * noch nicht genommen wurde. Gerufen NACH dem Init-Lauf (erst dann stehen die Props aus
 * main00 im Pool, an die sich dieses haengt), und zwar an BEIDEN Raumstart-Wegen des
 * Ports: Tuer (scd_room_setup.c, scd_room_reenter) und Boot/CONTINUE (platform/pc/main.c).
 * Im Original gibt es diese Zweiteilung nicht — der Raumlader FUN_800396fc hat genau
 * zwei Aufrufer, `jal 0x800396fc` @0x8001d5ac (Session-Start/LOAD) und @0x8001d988
 * (Tuer), und ruft selbst @0x80039a00 die SCD-Raum-Init FUN_8003ef6c. */
void re15_sicherung_install(uint16_t room_id);

/* Pro Gameplay-Bild: steht der Hebetisch oben, wird das Item-Modal aufgemacht. Der
 * Hochpunkt der Fahrt ist y = -305 - 910 = -1215, danach steht die Plattform auf -1205
 * (s. Kopf). Das Fenster (-5000 .. -1100] ist eine ⛔ PORT-WAHL OHNE ORIGINAL-ADRESSE
 * (das Original oeffnet hier kein Modal): -1100 wird nur in der Aufwaertsfahrt
 * unterschritten, -5000 haelt die Parklagen -20324/-20224 draussen. Das Modal friert
 * den Rest des Spiels ein (g_pauseflags), sub04 laeuft danach weiter und faehrt die
 * Plattform herunter. Rueckgabe 1 = Modal wurde in diesem Bild aufgemacht. */
int  re15_sicherung_tick(void);

/* Die eingebackenen Engine-Bytes (gen/sicherung_prop.inc) fuer den Plattform-Lader,
 * der sie in den Prop-Slot RE15_SICHERUNG_OBJ_ID haengt. */
const uint8_t *re15_sicherung_md1_bytes(int *out_size);
const uint8_t *re15_sicherung_tim_bytes(int *out_size);

/* ITEM-BILD UND ICON AUS DEM MODELL (Runde 30, Thema H "andere Sicherung").
 *
 * Das ausgelieferte Bild/Icon von Item 0x40 zeigt einen ANDEREN Gegenstand als das
 * Welt-Modell: ITPS-Block 0x40 @Datei 0xC0000 ist bytegleich RE2-Retail Block 77
 * (@0xE7000 in COMMON/DATA/ITPS.ITP, Item 0x4D "Fuse Case"), Tile 0x40 @0x12C00 in
 * ITEMALL.PIX bytegleich RE2-Tile 77 (@0x168F0). Dazu traegt der Block die Rechtecke
 * eines anderen Statusschirms (crect (0,480) / prect (0,0)), weshalb das CHECK-Foto
 * leer bleibt: der Foto-Lader @0x800c0258 (DEBUG.BIN) laedt an die EINGEBETTETEN
 * Rechtecke (`jal 0x8006bbbc` OpenTIM @0x800c0260, `jal 0x8006bbcc` ReadTIM @0x800c0268,
 * `jal 0x80068c88` LoadImage @0x800c0280 mit crect und @0x800c02a0 mit prect), das
 * Fotofenster des RE1.5-Schirms liegt aber bei (832,256) mit CLUT-Zeile (0,489).
 *
 * Die beiden Funktionen ueberschreiben im GELADENEN Puffer genau den Block bzw. das
 * Tile von Item 0x40 mit den aus dem Welt-Modell gerenderten Bytes
 * (gen/sicherung_itembild.inc, Werkzeug tools/sicherung_itembild.py):
 *   ITPS.ITP    Block 0x40 = Bytes 0xC0000..0xC2FFF   (Blockgroesse 0x3000: Lader
 *               LAB_8001e404, `ori v0,zero,0x3000` @0x8001e414 und id*6 Sektoren
 *               `sll v0,a0,1` / `addu v0,v0,a0` / `sll v0,v0,1` @0x8001e450-58)
 *   ITEMALL.PIX Tile 0x40  = Bytes 0x12C00..0x130AF   (1200 B je Tile, Tile = Id)
 * Alles ausserhalb dieser Fenster bleibt unberuehrt. Ein Puffer, der zu kurz ist,
 * bleibt ganz unberuehrt. Mehrfacher Aufruf ist unschaedlich (dieselben Bytes).
 * ⛔ Blickneigung und Licht des gerenderten Bildes sind PORT-WAHL (s. Kopf der .inc). */
void re15_sicherung_bild_einsetzen(uint8_t *itps, int size);
void re15_sicherung_icon_einsetzen(uint8_t *itemall, int size);

/* Die eingebackenen Bild-Bytes selbst (fuer Riegel und Werkzeuge). */
const uint8_t *re15_sicherung_itps_block_bytes(int *out_size);
const uint8_t *re15_sicherung_icon_tile_bytes(int *out_size);

#endif /* RE15_SICHERUNG_H */
