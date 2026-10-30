/* re15_panel2030_r41.h — Runde 41 Spur N "nachtrag30": Panel-Raetsel ROOM2030/2031 (AUFTRAG.md Nachtrag 30/30b/37, Entwurf A "Druckspeicher").
 *
 * NUTZER-VORGABE Nachtrag 30: "in ROOM 2030 am Kasten, damit die Platform runter faehrt, erstelle ein Raetsel, das so aussieht wie das Panel in
 * Nahansicht, anstatt das komische - dreh an Valve 13." Nachtrag 30b: "die Pressure ... 80". Nachtrag 37 (Freigabe "A Druckspeicher"): drei Tanks fassen
 * 100/50/30, die Anzeige liest Tank A (Start 100/0/0); Cursor mit dem Steuerkreuz, Aktionstaste waehlt die Quelle, Aktionstaste auf dem zweiten Tank
 * fuellt um, bis die Quelle leer oder das Ziel voll ist; Loesung A nach B, B nach C, C nach A = 80, kein Fehlversuch; EXIT verlaesst das Panel; bei 80
 * folgt Port-Text 2030 Id 43 "The pressure gauge reads 80. / Lower the bridge?". Ventil 13 als Ausloeser entfaellt. Oberflaeche wie panel_entwurf_a.png.
 * Alle Raetselwerte = PORT-WAHL bzw. NUTZER-VORGABE, Dossier N_nachtrag30.md §2; die FORM ist belegt:
 *   Vorbild Pult:  ROOM2050 (Cursor, Steuerkreuz je Bild, SQUARE betaetigt, CROSS bricht ab; Kopf re15_ventilkette_r41.h "2050 Schaltpult").
 *   Cursor-Schritt: ROOM2050 sub05..08 @0x023C2.. `2f 02 c8 00` = +-200 Welt je Bild = 2,515 Bildpunkte unter Cut 15 (Kopf re15_ventilkette_r41.h).
 *   Zielwert 80, Ruhe 30 Bilder, Bestaetigungston 0x0C: RE2 ROOM2130 sub04 @0x01752 / @0x0171C / @0x01762 (re15_panel_zeiger.h).
 *   Klick 0x0A, Fehlerton 0x0B: RE2 ROOM2130 sub04 @0x01192 / @0x01800.
 *   Umfuellen in 16 Bildern: Form der Original-Hebel ROOM11F0 sub06 For 16 @0x01336.
 * Bild: gen/panel2030_r41.inc (tools/r41_nachtrag30/panel_bild.py), Vollbild-Ebene wie die Plakat-Nahaufnahme, der Text liegt darueber
 * (render_pc.c re15_render_pc_fmv_text, nur solange das Panel offen ist). Massen = Schirmpixel 320 x 240 wie im Generator.
 * Der Faden des Pults (Programm 2030_panel_ein) endet nach dem Ja; der C-Takt fuehrt das Panel, die Bits (2,0)/(2,2) gibt der C-Takt zurueck.
 */
#ifndef RE15_PANEL2030_R41_H
#define RE15_PANEL2030_R41_H

#include <stdint.h>

#define RE15_R41N_TANKS            3
#define RE15_R41N_KAPAZITAET       { 100, 50, 30 }   /* NUTZER-VORGABE Nachtrag 37 */
#define RE15_R41N_START            { 100, 0, 0 }     /* NUTZER-VORGABE Nachtrag 37 */
#define RE15_R41N_ZIEL             80                /* Nachtrag 30b, Text 43; RE2 ROOM2130 sub04 @0x01752 */
#define RE15_R41N_RUHE             30                /* RE2 @0x0171C sleep 0x1E */
#define RE15_R41N_UMFUELL_TAKTE    16                /* ROOM11F0 sub06 For 16 @0x01336 */
#define RE15_R41N_SCHRITT_Q8       644               /* 2,515 Bildpunkte je Bild in 24.8 (ROOM2050 sub05..08 @0x023C2.. +-200, unter Cut 15) */
#define RE15_R41N_CURSOR_X0        160               /* Startlage wie Cut 15: Cursor (-19554,-900,22684) -> Bild (160,119) */
#define RE15_R41N_CURSOR_Y0        119
#define RE15_R41N_X_MIN            14                /* Oeffnung des Rahmens x 12..303, y 20..219 (Generator) */
#define RE15_R41N_X_MAX            301
#define RE15_R41N_Y_MIN            22
#define RE15_R41N_Y_MAX            217
/* Trefferflaechen (Bildpunkte): Tankkoerper samt Fuss und Schild, EXIT-Feld x 240..268, y 180..209 (Generator: Feld 29 x 30 ab (240,180)). */
#define RE15_R41N_TANK_X0          { 50, 136, 170 }
#define RE15_R41N_TANK_X1          { 92, 159, 213 }
#define RE15_R41N_TANK_Y0          38
#define RE15_R41N_TANK_Y1          142
#define RE15_R41N_EXIT_RECT        240, 180, 269, 210
/* Fuellstand -> Zeile: Skala gleich je Tank, Wert 100 oben (Zeile GT) .. Wert 0 unten (Zeile GB). */
#define RE15_R41N_GT               66
#define RE15_R41N_GB               116
#define RE15_R41N_ZEIGER_X         124
#define RE15_R41N_ZEIGER_Y         87
#define RE15_R41N_ZEIGER_R         139               /* Zehntel Bildpunkte: Scheibe R 13,9 */
#define RE15_R41N_NACHRICHT_Y0     172               /* abgedunkeltes Band hinter dem Text (Text-Ursprung y 180) */
#define RE15_R41N_NACHRICHT_Y1     218

/* Takt-Ergebnis */
#define RE15_R41N_ERG_NICHTS       0
#define RE15_R41N_ERG_ZU           1                 /* EXIT / CROSS: Panel zu, keine 80 */
#define RE15_R41N_ERG_GELOEST      2                 /* 80 erreicht, Ruhe abgewartet: Bit 101, Panel zu, Ereignis 24 fragt Text 43 */

void re15_panel2030_r41_reset(void);
void re15_panel2030_r41_oeffnen(void);     /* Start 100/0/0, Cursor Mitte */
int  re15_panel2030_r41_offen(void);
/* Ein Bild; edge/held = virtuelle Pad-Worte (g_scd_pad_edge/held), nachricht = 1 solange eine Nachricht steht. */
int  re15_panel2030_r41_takt(uint16_t edge, uint16_t held, int nachricht);
/* Reine Regel (Riegel): Aktionstaste auf Tank t. Rueckgabe 0 nichts, 1 gewaehlt, 2 abgewaehlt, 3 umgefuellt, 4 leer, 5 voll. */
int  re15_panel2030_r41_aktion(int t);
/* Vollbild 320 x 240 RGBA8888 (0xRRGGBBAA) solange offen, sonst NULL (PC; PSX NULL). */
const uint32_t *re15_panel2030_r41_rgba(void);

/* Messhaken (Riegel). */
int  re15_panel2030_r41_stand(int t);       /* Zielstand je Tank (ohne die 16 Bilder Fahrt) */
int  re15_panel2030_r41_anzeige_zehntel(void); /* Anzeige in Zehnteln, laeuft mit der Fahrt */
int  re15_panel2030_r41_quelle(void);       /* -1 oder Tank */
void re15_panel2030_r41_cursor(int *x, int *y);
void re15_panel2030_r41_cursor_setzen(int x, int y);
int  re15_panel2030_r41_ziel_erreicht(void);

#endif /* RE15_PANEL2030_R41_H */
