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
 * keine Instruktion, die man zitieren koennte. Die Zahlen sind aus der AUSGELIEFERTEN
 * GEOMETRIE des Tisches abgeleitet, nicht geschaetzt.
 *
 * Runde 32 (Nutzer, 2026-09-29): "Die Granate und die Sicherung ... liegt aktuell oben drauf.
 * Aber sie sollen unten, in den hochfahrenden Fach liegen - ein item links ein item rechts."
 * Dossier analysis/befunde_runde32/hebetisch_faecher.md, Werkzeuge
 * analysis/befunde_runde32/hebetisch_faecher_werkzeug/ (faecher.py, sitz_fach.py).
 * Prop 0 (ROOM1150.RDT MD1 @0x11E40) traegt UNTER der Tischplatte (y=-901) zwei durchgehende
 * Faecher, vorn (x=2) offen, hinten (x=-1258) von einer vollen Tafel geschlossen, Trennwand
 * z 861..950. Die Sicherung liegt im RECHTEN Fach B (Plattform-Koordinaten, +Y nach unten):
 *   Boden   y = -91  Viereck 93, Face-Record @0x12F20 `6f00 8e00 6f00 8f00 6f00 8200 6f00 8300`
 *                    = Punkte 142 @0x122F4 (2,-91,1715), 143 @0x122FC (2,-91,950),
 *                      130 @0x12294 (-1258,-91,1715), 131 @0x1229C (-1258,-91,950)
 *   Waende  z = 950  Viereck 90 @0x12EF0,  z = 1715  Viereck 92 @0x12F10
 *   Decke   y = -811 Viereck 91 @0x12F00;  Rueckwand x = -1258 Viereck 82 @0x12E70
 *   (ROOM1151.RDT Prop 0 @0x13EB8: dieselben Werte, Boden Viereck 93 @0x14F98.)
 *   Kamera  Cut 4 (@Datei 0xE0) steht in Plattform-Koordinaten bei x=+1242, z=+918 (vor der
 *           Trennwand), +z = Schirm RECHTS -> Fach B (z 950..1715) ist das rechte Fach.
 *
 *   POS_X = -628   Fach-Mitte in x: (vorn 2 + hinten -1258) / 2
 *   POS_Y = -117   Fachboden -91 minus Rohrradius 26 (gen/sicherung_prop.inc y[-26..26]) —
 *                  liegt AUF dem Boden (tiefster Punkt genau -91)
 *   POS_Z = 1332   Fach-Mitte in z: (950 + 1715) / 2 = 1332,5, abgerundet
 *   ROT_Y = 1024   Laengsachse X des Modells auf Plattform-z = parallel zur Oeffnung, quer im
 *                  Bild (groesste Ansicht von der Kamera aus)
 * Ergebnis (sitz_fach.py, ROOM1150 = ROOM1151): Huelle x -654..-602, y -143..-91, z 1129..1535;
 * Abstand vorn 604, hinten 604, Wand 179 / 180, Decke 668 -> ganz im Fach, kein Durchstoss.
 * Links/rechts und Sichtbarkeit GEMESSEN im Framedump (Dossier §4).
 * Eingefroren von unit_r32_hebetisch_faecher, unit_r30_sicherung_sitz.
 *
 * Vorher: Runde 30 mittig in der Kuppel (-280,-1062,1260) rot_y 1024; Runde 31 schraeg in der
 * Kuppel (-280,-1062,1280) rot_y 1440 — beides "oben drauf" (Kuppelpodest y=-1036). */
#define RE15_SICHERUNG_POS_X  (-628)
#define RE15_SICHERUNG_POS_Y  (-117)
#define RE15_SICHERUNG_POS_Z  (1332)
#define RE15_SICHERUNG_ROT_Y  (1024)

/* Legt das Prop beim Raumstart an — falls der Raum ROOM1150/1151 ist UND die Sicherung
 * noch nicht genommen wurde. Gerufen NACH dem Init-Lauf (erst dann stehen die Props aus
 * main00 im Pool, an die sich dieses haengt), und zwar an BEIDEN Raumstart-Wegen des
 * Ports: Tuer (scd_room_setup.c, scd_room_reenter) und Boot/CONTINUE (platform/pc/main.c).
 * Im Original gibt es diese Zweiteilung nicht — der Raumlader FUN_800396fc hat genau
 * zwei Aufrufer, `jal 0x800396fc` @0x8001d5ac (Session-Start/LOAD) und @0x8001d988
 * (Tuer), und ruft selbst @0x80039a00 die SCD-Raum-Init FUN_8003ef6c. */
void re15_sicherung_install(uint16_t room_id);

/* Pro Gameplay-Bild: RUHT der Hebetisch oben, wird das Item-Modal aufgemacht. Runde 31: "oben"
 * heisst der Skript-Zustand von sub04 — ein SCD-Thread im Fenster [Sleep 30 @0x101A,
 * For-Abfahrt @0x1042), nach dem Setzen-For @0x1010, Plattform auf -1205
 * (re15_hebetisch_ruht_oben, include/re15_hebetisch.h). Bis Runde 30 war es die y-Schranke
 * -1100 mitten im Hub. ⛔ PORT-WAHL OHNE ORIGINAL-ADRESSE (das Original oeffnet hier kein
 * Modal). Das Modal friert das Skript ein (FUN_8001db28 @0x8001dbc8 g_pauseflags |=
 * 0xFF000000, SCD-Laeufer @0x8003f04c; Port main.c Zweig re15_item_modal_active), sub04
 * laeuft erst danach weiter und faehrt die Plattform herunter. Rueckgabe 1 = Modal wurde in
 * diesem Bild aufgemacht.
 * SPERRE JE FAHRT (Runde 30, Nachschliff "sicherung-nein"): hoechstens ein Modal je Fahrt;
 * nach "No" bietet die NAECHSTE Fahrt die Sicherung wieder an — wie das Original eine
 * abgelehnte Aufnahme scharf laesst (RE1.5 nur der Ja-Zweig nullt die Zone @0x8001e090,
 * "No" @0x8001e0ec laesst sie stehen; RE2 dasselbe @0x800720cc / @0x80072298). Wieder
 * scharf in der Parklage (Pos_set @0x109E y = -20224) — ⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE,
 * Messung und Herleitung bei s_modal_ausgeloest in sicherung_1150.c. */
int  re15_sicherung_tick(void);

/* Runde 30, Nachtrag K: 1 = die Sicherung kann in DIESER Fahrt noch ein Modal aufmachen
 * (angelegt, sichtbar, nicht genommen, Sperre dieser Fahrt noch frei). Die Granate derselben
 * Fahrt (re15_granate_tick) wartet so lange. ⛔ Port-Wahl, keine Original-Adresse. */
int  re15_sicherung_fahrt_offen(void);

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

/* PRUEFUNG AN DEN LADESTELLEN (Runde 30, Nachbesserung nach dem Gegenpruefer).
 *
 * ⛔ WARUM: Das PC-Spiel liest Bild und Icon NIE ueber die faulen Lader der Engine, sondern
 * ueber vier Stellen im Plattformcode (main.c zweimal vor re15_itemall_set_pix /
 * re15_itps_set_data, inv_render_pc.c zweimal in den eigenen Puffern des Statusschirms).
 * Ohne den Einsetz-Aufruf an diesen vier Stellen blieben die Riegel gruen, obwohl das
 * Aufnahme-Modal wieder den "Fuse Case" zeigte (Mutationsprobe M2 des Gegenpruefers:
 * Modal F240 24583 Punkte vom Bau verschieden, 4158 vom Bestand).
 *
 * Die drei Funktionen zaehlen, wie weit das, was GEZEICHNET wird, vom eingebackenen Rohr
 * abweicht:
 *   _bild_abweichung(puffer)  Bytes in Block 0x40 (0xC0000..0xC2FFF) ungleich dem Rohr-Block
 *   _icon_abweichung(puffer)  Bytes in Tile 0x40 (0x12C00..0x130AF) ungleich dem Rohr-Icon
 *   _modal_bild_abweichung()  Bildpunkte, in denen der Modal-Leser re15_itps_pixel(0x40,u,v)
 *                             etwas anderes liefert als das Rohr-Bild (112 x 72 = 8064)
 *   _icon_leser_abweichung()  Bytes, in denen re15_itemall_tile_raw(0x40) vom Rohr-Icon
 *                             abweicht (der Puffer aus re15_itemall_set_pix)
 * 0 = eingesetzt. Der AUSGELIEFERTE Stand weicht ab (Block 8904 Bytes, Tile 389 Bytes —
 * gemessen, unit_r30_sicherung_bild Abschnitt 7). -1 = kein Puffer / zu kurz / kein Leser.
 * Die PC-Ladestellen schreiben das Ergebnis ins debug.log, integration_r30_sicherung_bild
 * verlangt dort die Null. */
int re15_sicherung_bild_abweichung(const uint8_t *itps, int size);
int re15_sicherung_icon_abweichung(const uint8_t *itemall, int size);
int re15_sicherung_modal_bild_abweichung(void);
int re15_sicherung_icon_leser_abweichung(void);

#endif /* RE15_SICHERUNG_H */
