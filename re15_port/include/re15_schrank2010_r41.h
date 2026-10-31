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
 * Die Fragen und Bit 125 gehoeren der Spur O / Spur C: die Programme 2010_kasten_* laufen hinter dem Aufklappen weiter.
 *
 * EINSETZEN wie ROOM2060 (Nachbesserung 1, Mangel 1): im Ja-Zweig zeigt die Vollbild-Ebene Cut 8 mit dem Cursor obj 4 (sub18 @0x0168C `29 08`, @0x0168E (5,10), @0x01692 (5,11)), ohne
 * Zeitgrenze und ohne Abbruch wie das Original. Steuerkreuz GEHALTEN je Bild +-200 Welt: oben sub14 @0x0164C z+, unten sub15 @0x01658 z-, rechts sub16 @0x01664 x+, links sub17 @0x01670 x-
 * (Polls sub01 @0x012C0/@0x012D8/@0x012F0/@0x01308, Sce_key_ck liest den gehaltenen Pad, op_sce_key_ck). Rahmen = Ausschiebung der Zellen (Original gemessen, G2_belege/orig_zensus/log.txt,
 * Riegel unit_r36_g2_cursor_rahmen). Bestaetigung sub01 @0x01314..@0x01332: Member_cmp(15 == 7) @0x01324 UND SQUARE gehalten @0x0132E -> sub19: Set(3,144) @0x0169E, Cut 9 @0x016A2, Text 5 @0x016C8.
 * Der Stempel des Objekts: gen/schrank2010_cursor_r41.inc (tools/r41_nachtrag30/schrank_cursor.py, Spielbild gegen BG08). Spieler und KI stehen ueber (2,0)/(2,2) wie sub01 @0x01288/@0x0128C,
 * die Pad-Sperre (2,7) wuerde die SCD-Pad-Woerter maskieren (game_step_common.c). Der Text liegt wie im Original auf Cut 9 (render_pc.c re15_render_pc_fmv_text); der Klick auf der Zone ist die
 * Port-Regel op_sce_key_ck (Nutzer 2026-09-26, RE2 ROOM2130 @0x01192).
 *
 * NUR CUT 3 (Nachbesserung 1, Mangel 4): die drei Texturen sind aus BG03/BG06 auf die Flaechen gerechnet und passen nur unter Cut 3 @0xC0. Der Fluegel steht in Cut 4 direkt vor der Kamera
 * (Lauf s_geh2: Spalten 0..86), die Hintergruende aller anderen Cuts zeigen den Schrank geschlossen: dort werden die Modelle nicht gezeichnet (main.c, Haken im Prop-Durchlauf). PORT-WAHL.
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
#define RE15_R41N_NAH_RAUM           0x2060u
#define RE15_R41N_NAH_LEER           8        /* ROOM2060 Cut 8 */
#define RE15_R41N_NAH_DRIN           9        /* ROOM2060 Cut 9 */
#define RE15_R41N_SCHRANK_CUT        3        /* Cut 3 @0xC0: fuer diesen Blick sind die Texturen gerechnet */
#define RE15_R41N_CUR_X0             19571    /* Obj_model_set obj 4 @0x00F12 */
#define RE15_R41N_CUR_Z0             (-22542)
#define RE15_R41N_CUR_SCHRITT        200      /* sub14..17 @0x0164C/@0x01658/@0x01664/@0x01670 */
#define RE15_R41N_CUR_X_MIN          9418     /* Rahmen obj 4 (Original): G2_belege/orig_zensus/log.txt */
#define RE15_R41N_CUR_X_MAX          29532
#define RE15_R41N_CUR_Z_MIN          (-29132)
#define RE15_R41N_CUR_Z_MAX          (-15968)
#define RE15_R41N_ZONE_X             20725    /* Aot_set Slot 7 @0x0100C rect (19700,-24300,2050,2050): Mitte + halbe Weite wie op_aot_set */
#define RE15_R41N_ZONE_Z             (-23275)
#define RE15_R41N_ZONE_HALB          1025
#define RE15_R41N_CUR_BILD_X         159      /* Bild der Startlage unter Cut 8, gemessen (Lauf c_fein): Rahmen x 147..171, y 108..130 */
#define RE15_R41N_CUR_BILD_Y         119
#define RE15_R41N_CUR_BILD_ZAEHLER   2        /* 200 Welt = 8/3 Bildpunkte, gemessen F586..F600 (15 Bilder = 40 Bildpunkte): Nadir-Kamera Cut 8 @0x160 */
#define RE15_R41N_CUR_BILD_NENNER    150
/* Virtuelle Pad-Bits (Sce_key_ck, ROOM2060 sub01): */
#define RE15_R41N_TASTE_HOCH         0x0001u  /* @0x012C0 */
#define RE15_R41N_TASTE_RUNTER       0x0004u  /* @0x012D8 */
#define RE15_R41N_TASTE_RECHTS       0x0002u  /* @0x012F0 */
#define RE15_R41N_TASTE_LINKS        0x0008u  /* @0x01308 */
#define RE15_R41N_TASTE_SETZEN       0x0040u  /* @0x0132E */

void re15_schrank2010_r41_reset(void);
/* Je Spielbild in 2010/2011 (aus kasten_tick): Marker (5,0x1E) = aufklappen, (5,0x1D) = Einsetz-Ansicht. */
void re15_schrank2010_r41_tick(void);
/* 1 = Vollbild-Ebene zeigt gerade eine Nahansicht; *cut = ROOM2060 Cut 8/9 (cut darf NULL sein). */
int  re15_schrank2010_r41_nahbild(int *cut);
/* Vollbild 320 x 240 RGBA8888 mit Cursor auf dem Grund `grund`, solange der Cursor-Schritt laeuft, sonst NULL (PC; PSX NULL). */
const uint32_t *re15_schrank2010_r41_cursor_rgba(const uint32_t *grund);
/* 1 = obj darf im aktuellen Cut nicht gezeichnet werden (Kopf "NUR CUT 3"). */
int  re15_schrank2010_r41_verdeckt(uint16_t raum, int cut, int obj);

/* Messhaken (Riegel). */
int  re15_schrank2010_r41_offen(void);      /* 1 = Tuer ganz auf */
int  re15_schrank2010_r41_winkel(void);     /* rot_y der Tuer 0..1024 */
int  re15_schrank2010_r41_prop(int obj);    /* Pool-Index oder -1 */
void re15_schrank2010_r41_cursor(int32_t *x, int32_t *z);   /* Weltlage des Cursors */
void re15_schrank2010_r41_cursor_bild(int *x, int *y);      /* Mitte des Stempels im Bild */
int  re15_schrank2010_r41_auf_zone(void);   /* 1 = Cursor steht in Zone 7 */

#endif /* RE15_SCHRANK2010_R41_H */
