/* re15_wasserlauf_r41.h — Runde 41 Spur N "nachtrag30": Wasserlauf an der Leiter von ROOM2000/2001.
 *
 * NUTZER-VORGABE AUFTRAG.md Nachtrag 30e (Wortlaut): "in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die
 * Leiter runter kommen und ueber den Weg bis zum Wasser Pool in der Mitte laufen." Nutzer-Wahl: "Bis zur Flut in 2030 (Empfohlen)".
 *
 * ⛔ RAUM: ROOM2010/2011 ist das Buero (Schreibmaschine, Computer, Bilder BG00..BG05) und hat weder Leiter noch Becken noch Wasser:
 * nur die Tuer Slot 0 @0x00746 nach 2000, keine Band-Stufe, kein Kletter-Platz, kein Sce_espr_on, kein Wasser-Objekt. Die Beschreibung trifft
 * Wort fuer Wort ROOM2000/2001, die Nachbartuer: Leiter nach 2050 (Tuer Slot 4 @0x0169E) auf dem West-Steg, Weg = der Steg (Band 1,
 * y -1800, x -27000..-25200 zwischen SCA Zelle 9 und 13), Pool in der Mitte = der Kanal x -25200..-21650 (Band 0, Wasser-Objekte obj 0..2
 * @0x016E6/@0x01708/@0x0172A Typ 3, y -900). Der Port baut den Wasserlauf deshalb dort; die Rueckfrage steht im Dossier
 * N_nachtrag30.md §7 und in fragen_an_nutzer.
 *
 * FORM (Original): der Leiterstrom ist derselbe Effekt wie die drei Strahlen am Gitter derselben Wand, ROOM2000 main00 @0x0174C/@0x0175C/
 * @0x0176C `3a 00 0b 00 00 00 00 19 x y z 00 04` = Sce_espr_on Effekt 0x0B, Sub 0, Skala 0x1900, Parameter 0x400 (FUN_80041864 ->
 * FUN_80019700), absolut, einmal gesetzt und dauerhaft (gemessen: am Gitter Oberkante y -5300, Unterkante y -784, die Teilchen fallen in Welt-y).
 * Der Einschlag im Kanal ist der Wasser-Effekt des Originals: Code 0x06050D00 = Effekt 6, Sub 5, Skala 0x0D00 `lui a0,0x605` /
 * `ori a0,a0,0xd00` @0x800456b8/bc (Wasserboden FUN_80045630).
 *
 * LAUF UEBER DEN WEG (Nachbesserung 1, Mangel 2; eine stehende Flaeche laeuft nicht, die nasse Spur der ersten Fassung ist entfallen): laufendes Wasser am Boden ist im Original die Flut-Welle
 * ROOM20B0: Emitter EM024 (0x8010ee9c) setzen Effekt 0x0F der Raum-Bank (`lui v0,0xf00` @0x801109b8, Dossier A §1.2). ROOM2000 traegt denselben Effekt in der eigenen Bank (RDT+0x4C @0x14608
 * ids 05 06 07 0b 0c 0d 0f, EFF 0x0F @0x15D64). Anim-Saetze Index 4..16 = 13 Bilder je 1 Bild Dauer, Sub-Zeile 0 `A 10` startet bei Index 4 (gate 4), Satz 17 = Ende: jeder Spritzer lebt 13 Bilder.
 * Sub 3: Zeile 1 vel (100,0,0), acc 0 = flach in Blickrichtung +x (Gier 0): 13 Bilder = 1300, von der Leiter bis vor die Kante. Sub 6: vel (42,-18,0), acc (0,10,0) = faellt ueber die Kante.
 * PORT-WAHLEN: Punkte, Skala (Flut 0x2000..0x4000 fuer einen Gang, hier ein Rinnsal so breit wie die Leiter), Abstaende.
 *
 * LAGE (gemessen am Hintergrund ROOM2000 Cut 10 @0x1A0, Projektion H 208, Dossier §7): Leiterholme am Fuss (-26748,-1800,9034) und
 * (-26715,-1800,9945), oben im Schacht y -7000 (Cut 10 Bild (3,45)/(23,43)). Weg: von der Leiter x -26750 quer ueber den Steg bis zur
 * Kante x -25200 (SCA Zelle 13), so breit wie die Leiter (z 9050..9950).
 *
 * ENDE: dieselbe Bedingung wie die Sperre der Leiter (ventilkette_r41.c install_2000): solange Bit (9,100) "Leiter frei" = 0 und Bit (9,103) "Flut
 * gelaufen" = 0. Nach der Flut fuehren alle Tueren nach 2000 ohnehin nach 20B0 (dort kein Wasserlauf).
 *
 * PORT-WAHLEN: Skala der Leiter-Strahlen, Punkte und Takt der Einschlaege im Kanal.
 */
#ifndef RE15_WASSERLAUF_R41_H
#define RE15_WASSERLAUF_R41_H

#include <stdint.h>

#define RE15_R41N_LAUF_Y          (-1800)  /* Steg = Band 1 (Spawn-Lage 2000, Tuer Slot 2 @0x0165E)                         */
#define RE15_R41N_LAUF_KANTE      (-25200) /* Kante des Stegs: SCA Zelle 13 (Kopf "LAGE")                                  */
#define RE15_R41N_KANAL_Y         (-900)   /* Wasser-Objekte @0x016E6 Lage y                                                */
#define RE15_R41N_ESP_LAUF        0x0F     /* Effekt der Flut-Welle (@0x801109b8), EFF ROOM2000 @0x15D64                    */
#define RE15_R41N_LAUF_SUB_FLACH  3        /* EFF 0x0F Sub 3: vel (100,0,0), acc 0                                          */
#define RE15_R41N_LAUF_SUB_FALL   6        /* EFF 0x0F Sub 6: vel (42,-18,0), acc (0,10,0)                                  */
#define RE15_R41N_LAUF_VEL        100      /* Zeile 1 Sub 3                                                                  */
#define RE15_R41N_LAUF_BILDER     13       /* Anim-Saetze 4..16 je 1 Bild                                                    */
#define RE15_R41N_LAUF_SKALA      0x1000   /* PORT-WAHL (Kopf "LAUF UEBER DEN WEG")                                          */
#define RE15_R41N_LAUF_HOEHE      (-1860)  /* PORT-WAHL: 60 ueber dem Steg, der Spritzer sitzt auf dem Boden                 */
#define RE15_R41N_ESP_STRAHL      0x0B     /* @0x0174C pc[2]                                                                */
#define RE15_R41N_ESP_PARAM       0x400    /* @0x0174C pc[14..15]                                                           */
#define RE15_R41N_ESP_WASSER      6        /* Effekt 6 @0x800456b8                                                          */
#define RE15_R41N_ESP_RING_SUB    5        /* Sub 5 @0x800456b8                                                             */
#define RE15_R41N_ESP_RING_SKALA  0x0D00   /* @0x800456bc                                                                   */
#define RE15_R41N_TAKT_RING       11       /* PORT-WAHL Einschlag-Takt                                                      */

/* 1 = dieser Raum zeigt den Wasserlauf (Kopf "ENDE"). */
int  re15_wasserlauf_r41_soll(uint16_t room_id);
/* Aus install_2000 (ventilkette_r41.c), auch fuer ROOM2001: Strahlen anlegen oder (Leiter frei) nichts tun. */
void re15_wasserlauf_r41_install(uint16_t room_id);
/* Aus re15_ventilkette_r41_tick: Lauf ueber den Weg, Einschlaege im Kanal. Steht mit den Effekten (RE15_PAUSE_ACTION @0x80019e28). */
void re15_wasserlauf_r41_tick(void);

/* Messhaken (Riegel). */
int      re15_wasserlauf_r41_aktiv(void);           /* 1 = Wasserlauf laeuft im aktuellen Raum                        */
int      re15_wasserlauf_r41_strahlen(void);        /* Zahl der angelegten Dauer-Strahlen                              */
unsigned re15_wasserlauf_r41_spritzer(void);        /* Einschlaege seit dem Raumstart                                  */
unsigned re15_wasserlauf_r41_laeufer(void);         /* Spawns des Laufs seit dem Raumstart                             */
const int32_t *re15_wasserlauf_r41_strahl(int i);   /* i-ter Strahl: { x, y, z, Skala } oder NULL                      */
void     re15_wasserlauf_r41_reset(void);

#endif /* RE15_WASSERLAUF_R41_H */
