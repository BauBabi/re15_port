/*
 * re15_panel_zeiger.h — der ROTE LEISTUNGS-ZEIGER des Boiler-Room-Bedienfelds (ROOM11F0).
 *
 * ================= WARUM ES DAS HIER UEBERHAUPT GIBT ==============================
 * ROOM11F0 hat KEIN Zeiger-Objekt. Selbst nachgezaehlt an
 * `re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT` (152588 B):
 *   - RDT-Kopf: nSprite = 0, nOmodel = 12.
 *   - Die 12 Props sind restlos zugeordnet: obj 0x00 = der Auswahl-Cursor
 *     (Obj_model_set @Datei 0x00E54, `2d 00 04 ...`, type 4, Pos (-19554,0,22684)),
 *     obj 0x01 = das Prop im begehbaren Raum @Datei 0x00D10, obj 0x02..0x0B = die zehn
 *     Kipphebel (@Datei 0x00E76 .. 0x00FA8).
 *   - Der ganze SCD (main00 + 20 Subs, 0xCC4..0x1810) enthaelt KEIN Se_on und nichts,
 *     das eine Skala bedient.
 * Die Skala 0..100 mit rot gedruckter 80 steckt vollstaendig im BSS-Hintergrund von
 * Cut 10 — im Port nachgemessen am 2D-Dump (RE15_FBDUMP, ROOM11F0 Cut 10, 320x240):
 * die Teilstrich-Saeule liegt auf x 278..280, die rote 80-Ziffer auf x 262..269 / y 74..83,
 * das rote 80->100-Band auf x 278..281 / y 54..79.
 *
 * RE1.5 ist an dieser Stelle also nachweislich UNFERTIG (40%-Beta) -> nach dem Projektziel
 * ist RE2-RETAIL massgeblich. Alles, was hier Mechanik ist, stammt aus RE2 ROOM2130
 * (dasselbe Bedienfeld, um 90 Grad gedreht) und traegt seine Datei-Offsets.
 *
 * ================= WAS AUS RE2 KOMMT (selbst aus den Bytes gelesen) ================
 * Datei `info/re2leon/PL0/RDT/ROOM2130.RDT`, SCD-MAIN sub04, Basis @Datei 0x01110:
 *   sub04+0x0028 (@0x01138)  24 05 00 00        save(var5 = 0)      LEISTUNGSWERT = 0
 *   sub04+0x0030 (@0x01140)  24 07 ac 9f        save(var7 = 0x9FAC) ZEIGER-Grundstellung
 *   sub04+0x00AA (@0x011BA)  26 00 00 10 24 00  var5 += 36   (Schalter EIN)
 *   sub04+0x00B8 (@0x011C8)  23 00 05 02 64 00  if (var5 >= 100) var5 = 100   DECKEL
 *   sub04+0x0122 (@0x01232)  26 00 01 10 0e 00  var5 -= 14   (Schalter AUS)
 *   sub04+0x0130 (@0x01240)  23 00 05 04 00 00  if (var5 <= 0)  var5 = 0      BODEN
 *   sub04+0x00E8 (@0x011F8)  25 10 07 00 26 00 00 10 09 00   var7 += 9  je Punkt
 *   sub04+0x0106 (@0x01216)  02                 evt_next  -> GENAU EIN Punkt je BILD
 *   sub04+0x0622 (@0x01732)  23 00 05 00 64 00  if (var5 == 100) "too high"
 *   sub04+0x0642 (@0x01752)  23 00 05 00 50 00  if (var5 ==  80) "OK"      <- die "80"
 *   sub04+0x064E (@0x0175E)  22 04 3c 01        Raetsel-geloest-Flag
 *   sub04+0x0652 (@0x01762)  36 02 0c 01 ...    se_on(Gruppe 2, 0x0C) BESTAETIGUNG
 *   sub04+0x06C6 (@0x017D6)  23 00 05 00 00 00  if (var5 ==   0) "too low"
 *   sub04+0x06D8 (@0x017E8)  23 00 05 01 50 00  if (var5 >   80) "too high" sonst "low"
 * => Zeigerlage = Grundstellung + SCHRITT * wert, ein Wertpunkt je Bild, Wert in [0,100],
 *    Ziel 80.
 *
 * ================= WAS AUS RE1.5 KOMMT (selbst gemessen) ==========================
 * (a) Die Skalen-Eichung, am 2D-Dump von Cut 10 abgelesen: Teilstriche auf
 *     y = 177,165,152,140,128,115,103,91 (dunkel bei x 278..280) und, im roten Band,
 *     y = 78,66,54. Das sind 10 Abstaende ueber 177-54 = 123 Pixel.
 *     => y(wert) = 177 - wert * 1.23. Gegenprobe: wert 80 -> y = 78.6, und genau dort
 *        stehen die rote 80-Ziffer (y 74..83, Mitte 78.5) und der Fuss des roten
 *        Bandes (y 79). wert 100 -> y = 54 = Bandkopf. wert 0 -> y = 177 = unterster
 *        Teilstrich.
 * (b) Die 90-Grad-Drehung: in RE2 (ROOM21306.bmp, selbst vermessen) liegt die Skala
 *     waagerecht (Grundlinie y 81..82, x 25..301, Ziffern darueber, Zeiger darunter);
 *     in RE1.5 liegt dieselbe Skala senkrecht (Saeule x 280, Ziffern LINKS davon).
 *     Die Drehung (a,b) -> (b,-a) (90 Grad gegen den Uhrzeigersinn) bildet RE2s
 *     "Zeiger auf der ziffern-abgewandten Seite" auf "rechts der Saeule" ab und
 *     RE2s "Wert waechst nach rechts" auf "Wert waechst nach oben".
 * (c) Der Loesungszustand: sub01 @Datei 0x012BE..0x012E2 prueft die zehn Schalterbits
 *     Bank 5 / 13..22 auf 1,0,1,0,1,0,1,0,1,0 und ruft dann Evt_exec(sub18) @0x012E6,
 *     danach Set(4,238,1) @0x012EA. Genau 5 der 10 Schalter sind im Ziel EIN.
 *
 * ================= WAS NUTZER-ENTSCHEIDUNG IST (KEIN Beleg, so gekennzeichnet) =====
 * DAS GEWICHT JE SCHALTER. RE2 hat 5 Schalter mit aufgedruckten 36/14 (im
 * Hintergrundbild ROOM21306.bmp lesbar) und einen WEGABHAENGIGEN Zaehler; RE1.5 hat
 * 10 Schalter, KEINE aufgedruckten Zahlen (im Cut-10-Bild nachgesehen: neben jedem
 * Hebel sitzt nur eine gruene Lampe) und eine rein ZUSTANDSabhaengige Loesungspruefung
 * (@0x012BE..0x012E2 liest nur Bits). Ein wegabhaengiger RE2-Zaehler wuerde der
 * RE1.5-Pruefung widersprechen: dieselbe Schalterstellung kaeme mal auf 80, mal nicht.
 * Deshalb bleibt die Wertbildung ZUSTANDSabhaengig: wert = Summe der Gewichte der
 * EIN-Schalter.
 *
 * ⛔ DIE ZEHN GEWICHTE SIND WOERTLICHE NUTZER-VORGABE vom 2026-09-26 — KEIN Original,
 *    kein RE-Beleg, und sie werden auch nicht als byte-true verkauft. Wortlaut:
 *
 *        Schalter :  1    2    3    4    5    6    7    8    9   10
 *        Wert     : +20  -20  -10  -30  +20  -40  +20  -50  +30  -60
 *
 *        Loesung: Schalter 1 + 3 + 5 + 7 + 9  ->  0 -> 20 -> 10 -> 30 -> 50 -> 80
 *        Anzeige: cursor = max(0, Summe der AKTIVEN Schalter)
 *        Die Schalterwerte bleiben erhalten, ein Schalter kann jederzeit wieder AUS.
 *
 *    Der Nutzer hat die Werte selbst gegen alle 1024 Kombinationen geprueft; die Sonde
 *    r27_panel_schalterwerte zaehlt sie NOCHMALS durch (genau eine Kombination erreicht
 *    80, und das ist 1+3+5+7+9 = Maske 0x155).
 *
 *    (VORHER stand hier eine gleichmaessige Gewichtung 16 je EIN-Schalter. Sie war
 *    ebenfalls Nutzer-Entscheidung und ist durch die Vorgabe oben ersetzt.)
 *
 * WICHTIG — ROHWERT vs. ANZEIGE: der ROHWERT darf negativ werden (Minimum −210 bei
 * allen zehn Schaltern EIN), nur die ANZEIGE wird bei 0 abgeschnitten. Deckel und Boden
 * sind weiterhin RE2 (sub04+0x00B8 @0x011C8 bzw. +0x0130 @0x01240); der Deckel 100
 * greift mit diesen Gewichten nie, weil die vier positiven Schalter zusammen nur 90
 * ergeben (20+20+20+30).
 *
 * DIE LOESUNGSPRUEFUNG BLEIBT RE1.5. Sie liest die zehn Bits @0x012BE..0x012E2 gegen
 * 1,0,1,0,1,0,1,0,1,0 auf Bank 5 / Bits 13..22 — also Schalter 1,3,5,7,9 EIN. Das ist
 * DIESELBE Menge, die die Nutzer-Gewichte auf 80 bringen; beide Seiten decken sich, es
 * gibt hier keinen Konflikt. Der Zeiger zeigt, das SCD entscheidet.
 */
#ifndef RE15_PANEL_ZEIGER_H
#define RE15_PANEL_ZEIGER_H

#include <stdint.h>

/* Der Raum, dessen Bedienfeld den Zeiger traegt, und der Kamera-Cut der Raetselbuehne
 * (Cut_chg 0x0A, ROOM11F0.RDT sub16 @Datei 0x015C0, Bytes `29 0a`). */
#define RE15_PANEL_RAUM        0x11F0u
#define RE15_PANEL_CUT         10

/* Die zehn Gewichte — ⛔ WOERTLICHE NUTZER-VORGABE 2026-09-26, siehe Kopf.
 * Reihenfolge = Schalter 1..10 = Bank 5 / Bits 13..22 (ROOM11F0.RDT @0x012BE..0x012E2,
 * `21 05 0d 01` .. `21 05 16 00`), also obj 0x02..0x0B. */
#define RE15_PANEL_SCHALTER    10
#define RE15_PANEL_GEWICHTE  { 20, -20, -10, -30, 20, -40, 20, -50, 30, -60 }
/* Die Maske, die die Gewichte oben auf genau 80 bringt (Schalter 1,3,5,7,9 = Bit 0,2,4,6,8)
 * UND die RE1.5 @0x012BE..0x012E2 als Loesung prueft. Nur Doku/Riegel, keine Spiellogik —
 * geloest wird weiterhin durch das SCD. */
#define RE15_PANEL_LOESUNGSMASKE 0x155u
/* Deckel/Boden: RE2 sub04+0x00B8 (@0x011C8) bzw. +0x0130 (@0x01240). */
#define RE15_PANEL_MAX         100
#define RE15_PANEL_MIN         0
/* Zielwert: RE2 sub04+0x0642 (@Datei 0x01752) `23 00 05 00 50 00` = cmp(var5,"==",0x50). */
#define RE15_PANEL_ZIEL        80

/* Skalen-Eichung, am Cut-10-Hintergrund selbst vermessen (s. Kopf (a)).
 * y = (ANKER_Y*100 - wert*SPANNE) / 100  mit ANKER_Y = 177 (wert 0) und
 * SPANNE = 123 Pixel fuer 100 Punkte -> 1.23 px je Punkt, ganzzahlig gerechnet. */
#define RE15_PANEL_ANKER_Y     177
#define RE15_PANEL_SPANNE_Y    123
/* Spitze des Zeigers: die Teilstrich-Saeule liegt auf x 278..280, die Platte endet bei
 * x 281 — der Zeiger sitzt rechts davon und zeigt nach links (90-Grad-Bild von RE2s
 * "unter der Skala, nach oben zeigend"). */
#define RE15_PANEL_SPITZE_X    281
/* Form: RE2s Zeiger-Modell 2 (room2130/obj/model02.md1, 124 B) ist ein Dreieck mit den
 * Ecken (0,-18,-7)/(0,-18,6)/(0,18,0) — 36 Einheiten quer zur Bewegung, 13 tief. Bei RE2s
 * 9 Einheiten je Punkt sind das 4,0 Punkte quer und 1,4 Punkte tief. Auf RE1.5s Eichung
 * (1,23 px je Punkt) sind 4,0 Punkte 4,9 px -> 5 px Hoehe; die Tiefe waechst analog auf
 * 2 px, wird hier aber auf 5 px gezogen, damit die Spitze bei 320x240 ueberhaupt als
 * Pfeil lesbar bleibt (⛔ NUTZER-ENTSCHEIDUNG, wie die Farbe). */
#define RE15_PANEL_HALB_H      2      /* 2*2+1 = 5 px hoch */
#define RE15_PANEL_TIEFE       5      /* 5 px lang */

/* Die Farbe. RE2s Zeiger-Modell traegt KEIN Farbfeld (room2130/obj/model02.md1, 124 B:
 * nur Vertex-, Normal- und Flaechenindizes) — die Farbe DES RE2-ZEIGERS ist damit NICHT
 * belegt, und ich erfinde sie auch nicht. Belegt ist das ROT DES RAUMS: ROOM2130 hat genau
 * ein ESP (Nr. 0x16), dessen CLUT-Block bei `esp16.tim` @Datei 0x08 liegt (VRAM (0,480),
 * 16x4 = vier Paletten). Palette 0 ist die rote; ihr gesaettigter Stuetzpunkt steht
 * @Datei 0x22 als 0x947A = R 208 / G 24 / B 40 (selbst ausgelesen).
 * RE1.5s eigene 80-Ziffer taugt als Quelle NICHT: im MDEC-Hintergrund ist sie bis auf
 * (159,95,95) entsaettigt (staerkstes Rot im ganzen Skalenkasten, selbst gemessen).
 * ⛔ Die Zuordnung "Raum-Rot = Zeiger-Rot" ist eine NUTZER-ENTSCHEIDUNG. */
#define RE15_PANEL_ROT_R       208    /* esp16.tim @0x22, CLUT 0 */
#define RE15_PANEL_ROT_G       24
#define RE15_PANEL_ROT_B       40

/* Ein Bild weiterdrehen. Gehoert in den Spiel-Tick (game_step_common.c), NACH dem
 * SCD-Tick, damit die Schalterbits des laufenden Bildes schon stehen. */
void re15_panel_zeiger_tick(void);

/* Zustand zuruecksetzen (Raumwechsel / Testaufbau). */
void re15_panel_zeiger_reset(void);

/* Sichtbarkeit + Bildschirmlage fuer den Zeichner.
 * Rueckgabe 1 = zeichnen, 0 = nichts. `*sx` = Spitze x, `*sy` = Spitze y. */
int  re15_panel_zeiger_sicht(int *sx, int *sy);

/* Gewicht eines einzelnen Schalters, `nr` = 1..10 (0 ausserhalb). */
int  re15_panel_zeiger_gewicht(int nr);

/* ROHWERT aus der Schaltermaske (Bit 0 = Schalter 1 .. Bit 9 = Schalter 10).
 * DARF NEGATIV WERDEN — hier wird nichts geklemmt. */
int  re15_panel_zeiger_roh_aus_maske(unsigned maske);

/* ANZEIGEWERT aus derselben Maske: max(0, Rohwert), oben bei RE15_PANEL_MAX gedeckelt. */
int  re15_panel_zeiger_ziel_aus_maske(unsigned maske);

/* Messhaken (Sonden): der ANIMIERTE, der ANGESTREBTE und der ROHE Wert. */
int  re15_panel_zeiger_wert(void);
int  re15_panel_zeiger_ziel(void);
int  re15_panel_zeiger_roh(void);

#endif /* RE15_PANEL_ZEIGER_H */
