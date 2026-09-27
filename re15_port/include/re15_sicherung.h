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
 * @0x0F96-0x10B6) — und der passt, weil er im Auslieferungsstand keinen Zweck hat:
 * sub04 holt die geparkte Plattform mit Pos_set @0xFB4 auf y=-305 in den Raum, oeffnet
 * den zweiteiligen Deckel (24 Schritte a +-10), faehrt 91 Schritte a -10 hoch, wartet
 * bei Sleep 30 @0x101A — und faehrt wieder herunter, ohne dass etwas geschieht. Genau
 * in dieses Warten gehoert die Beute.
 *
 * ⛔ KEIN ASSET-PATCH: die ausgelieferten RDTs bleiben byte-true (dieselbe Linie wie
 * der uebrige schlafende Content, scd_room_setup.c:342). Das Welt-Modell kommt als
 * zusaetzliches Prop portseitig dazu — ROOM1150 hat nOmodel=4, der Pool fasst 17.
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

/* Sitz auf der Plattform, in deren MODELL-Koordinaten (die Elternmatrix rechnet sie in
 * die Welt um, pc_prop_world in main.c). Gemessen am MD1 von Prop 0:
 *   Deckflaeche des Unterbaus  y = -901  (62 Punkte auf dieser Hoehe)
 *   freier Streifen zwischen den Papierstapeln  z 698..870  ueber die volle Breite
 * Die Sicherung liegt QUER (sie ist 406 lang, der Streifen nur 172 tief) und mit dem
 * Zylinderradius 26 ueber der Deckflaeche: y = -901 - 26 = -927. */
#define RE15_SICHERUNG_POS_X  (-628)
#define RE15_SICHERUNG_POS_Y  (-927)
#define RE15_SICHERUNG_POS_Z  (784)

/* Legt das Prop beim Raumstart an — falls der Raum ROOM1150/1151 ist UND die Sicherung
 * noch nicht genommen wurde. Aus scd_room_setup.c nach dem Init-Tick gerufen (erst dann
 * stehen die Props aus main00 im Pool, an die sich dieses haengt). */
void re15_sicherung_install(uint16_t room_id);

/* Pro Gameplay-Bild: steht der Hebetisch oben, wird das Item-Modal aufgemacht. Der
 * Tiefpunkt der Fahrt ist rechnerisch y = -305 - 910 + 10 = -1205 (sub04: Pos_set auf
 * -305 @0xFB4, 91 Schritte a -10 @0xFF6, dann 10 Schritte a +1 @0x1010); die Schranke
 * liegt bei -1100 und wird nur oben unterschritten. Das Modal friert den Rest des
 * Spiels ein (g_pauseflags), sub04 laeuft danach weiter und faehrt die Plattform
 * herunter. Rueckgabe 1 = Modal wurde in diesem Bild aufgemacht. */
int  re15_sicherung_tick(void);

/* Die eingebackenen Engine-Bytes (gen/sicherung_prop.inc) fuer den Plattform-Lader,
 * der sie in den Prop-Slot RE15_SICHERUNG_OBJ_ID haengt. */
const uint8_t *re15_sicherung_md1_bytes(int *out_size);
const uint8_t *re15_sicherung_tim_bytes(int *out_size);

#endif /* RE15_SICHERUNG_H */
