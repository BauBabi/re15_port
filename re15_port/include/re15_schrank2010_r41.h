/* re15_schrank2010_r41.h — Runde 41 Spur N "nachtrag30": der Ventil-Sicherungskasten ROOM2010/2011 klappt auf (AUFTRAG.md Nachtrag 30).
 *
 * NUTZER-VORGABE: "In ROOM 2010, beim Anklicken des Schranks, mache das er aufklappt und dann so aussieht wie in 2060 der Fuse Kasten - siehe fuse_box.bmp. Ab da
 * soll auch der Mechanismus gleich - das Einsetzen der Sicherung etc."
 *
 * BELEG BILD: fuse_box.bmp (320 x 240) = ROOM2060 Cut 6 (mittlere Abweichung 1,04 je Kanal bei Verschiebung 0; Cut 10 = 1,15, alle anderen Cuts >= 21,3): der offene
 * Schrank links im Raumbild. Cut 10 ist derselbe Blick NACH dem Einsetzen (sub19 @0x016C5 `4b 06 0a` Cut_replace 6 <-> 10), Cut 8/9 sind die Nahansichten des
 * Sicherungsfelds: Cut 8 = eine Sicherung fehlt, rote Lampe (sub18 @0x0168C `29 08`), Cut 9 = eingesetzt, gruene Lampe (sub19 @0x016A2 `29 09`).
 *
 * FORM "aufklappen": ein Tuerfluegel als Port-Modell um seine Angel, Drehung wie die Original-Hebel ROOM11F0 sub06 (`2f 05 40 00` Speed_set(5,+0x40) @0x01332, For 16
 * `0d 00 04 00 10 00` @0x01336: 16 Bilder, 64 je Bild = 1024 = 90 Grad); Modelle im Format des Aushangs (tools/r41_ventilkette/plakat.py, tools/r41_nachtrag30/schrank_bild.py).
 * Lage gemessen (Cut 3 @0xC0, Projektion H 208): Schrankfront an der Ostwand x 5400, z -3300..-2000, y -3650..-50; der kleine Tastenblock aus Nachbesserung 5 sitzt auf
 * der Tuer. Angel am fernen Rand z -2000: der Fluegel oeffnet zur Kamera hin nach links, wie in fuse_box.bmp. Geschlossen deckt sich die Aussenseite mit dem Hintergrund
 * (Textur = BG03 auf die Tuerebene zurueckprojiziert), deshalb liegt das Modell erst mit dem Aufklappen im Raum.
 * Das Bit bleibt (9,125); der offene Zustand ist Raum-Scratch und faellt beim Raumwechsel zu (die Tuer geht beim naechsten Besuch wieder auf).
 * Die Logik der Sicherung (Fragen, Bit 125, Inventar) gehoert der Spur O / Spur C: die Programme 2010_kasten_* laufen unveraendert hinter dem Aufklappen weiter. In ihrem Ja-Zweig
 * zeigt die Vollbild-Ebene die zwei Nahansichten Cut 8 -> Cut 9 in der Reihenfolge von sub18/sub19; die Dauer je Bild = Sleep 20 (ROOM2080 sub02 @0x00CD8) + 10.
 */
#ifndef RE15_SCHRANK2010_R41_H
#define RE15_SCHRANK2010_R41_H

#include <stdint.h>

#define RE15_R41N_SCHRANK_OBJ_INNEN  5        /* ROOM2010 hat obj 0..4 (nOmodel 5) */
#define RE15_R41N_SCHRANK_OBJ_AUSSEN 6
#define RE15_R41N_SCHRANK_OBJ_TUER   7
#define RE15_R41N_SCHRANK_BAND       2        /* weder Spielerband 0 noch darunter (Kopf re15_ventilkette_r41.h RE15_R41C_BLATT_BAND) */
#define RE15_R41N_ANGEL_X            5400     /* Ostwand = Innenkante SCA Zelle 3 */
#define RE15_R41N_ANGEL_Z            (-2000)  /* gemessen Cut 3 */
#define RE15_R41N_AUF_TAKTE          16       /* ROOM11F0 sub06 For 16 @0x01336 */
#define RE15_R41N_AUF_SCHRITT        64       /* ROOM11F0 sub06 @0x01332 +0x40  */
#define RE15_R41N_NAHBILD_TAKTE      30       /* je Nahansicht: Sleep 20 @0x00CD8 + 10 fuer das Lesen; PORT-WAHL */
#define RE15_R41N_NAH_RAUM           0x2060u
#define RE15_R41N_NAH_LEER           8        /* ROOM2060 Cut 8 */
#define RE15_R41N_NAH_DRIN           9        /* ROOM2060 Cut 9 */

void re15_schrank2010_r41_reset(void);
/* Je Spielbild in 2010/2011 (aus kasten_tick): Marker (5,0x1E) = aufklappen, (5,0x1D) = Nahansichten. */
void re15_schrank2010_r41_tick(void);
/* 1 = Vollbild-Ebene zeigt gerade eine Nahansicht; *cut = ROOM2060 Cut 8/9. */
int  re15_schrank2010_r41_nahbild(int *cut);

/* Messhaken (Riegel). */
int  re15_schrank2010_r41_offen(void);      /* 1 = Tuer ganz auf */
int  re15_schrank2010_r41_winkel(void);     /* rot_y der Tuer 0..1024 */
int  re15_schrank2010_r41_prop(int obj);    /* Pool-Index oder -1 */

#endif /* RE15_SCHRANK2010_R41_H */
