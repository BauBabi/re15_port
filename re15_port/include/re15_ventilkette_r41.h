/*
 * RE1.5 Rebuilt — Runde 41 Spur C "ventilkette": die STAGE2-Kette Leiter 2000 / Bruecke + Ventile 2030 /
 * Flut -> 20B0 / Schaltpult 2050 / Rechner 2080.
 *
 * NUTZER-VORGABE (AUFTRAG.md "Vorab vereinbart", 2026-10-23, woertlich):
 *   "in ROOM 2000 die Leiter zu ROOM 2050 mit herunterlaufenden Wasserstrom zu blockieren mit einer Message wie
 *    'I cannot climb up the ladder because of the water pressure'. Dann wollte ich das man das Valve Raetsel in
 *    2030 loest. Damit sollte dann eine Warnung kommen, das das Wasser in einer Minute steigt, und man den Raum
 *    verlassen soll. Verlaesst man den Raum fuehrt das zu ROOM 20B0. spaeter sollte der Raum wieder frei werden
 *    ohne Wasser. Nach dem Steigen des Wassers dort, sollte die Leiter frei sein, also kein Wasser mehr runter
 *    laufen. ... das man das Wasserlevel vielleicht im Raum [2050] nicht steigen lassen kann, bevor man das
 *    Raetsel loest. Und die Herunterfahrbare Plattform, bei der ich auch toll finde, wenn die vorher oben waere
 *    und man etwas machen muss, damit sie runter faehrt ... eher das Pult auf der linken Seite ...
 *    Bruecken-Pult: Irgendein kleines Raetsel waere gut ... Wasser in 2030: Ueber die Ringebene ...
 *    Ventilraetsel: einverstanden. Zeit abgelaufen und noch im Raum: Tot mit ertrink Effekt. Ventil 13 als
 *    Bruecken-Raetsel: Einverstanden."
 *   Punkt 6 (VERTRAG): "ROOM 2080: der Rechner, der das Wasser in 2080 senkt, entleert danach auch 2030".
 *
 * Dossier: analysis/befunde_runde41/C_ventilkette.md (RE-Belege §1, Umsetzung §3). Kein Asset-Patch: die RDTs
 * bleiben byte-true; alles hier wird nach dem Init-Lauf des Raums angelegt (scd_room_setup.c / main.c Boot-Weg).
 *
 * ============================================================================================================
 * ORIGINAL-BELEGE (Bytes selbst gelesen, tools/scd_dump_room.py; Datei-Offsets in shared_assets/PSX/STAGE2/)
 * ============================================================================================================
 * ROOM2000 Leiter = main00 @0x0169E Door_aot_set Slot 4 `3b 04 02 31 01 00 30 94 48 26 e8 03 e8 03 8c 23 e8 ea 04 10
 *   00 04 01 05 00 03 ...` (Rechteck (-27600,9800,1000,1000) Band 1 -> ROOM2050 Cut 0). Die Wasserstrahlen an der
 *   Leiterwand: main00 @0x0174C/@0x0175C/@0x0176C `3a 00 0b ...` (immer an). Kein Skript liest (3,235); keine Tuer
 *   fuehrt nach ROOM20B0 (Tuertabellen gen/tuer_zuordnung.inc + gen/re15_tuer_eigen.inc: 0 Treffer).
 * FORM "Tuer wird Sperrtext": ROOM2030 main00 @0x01B0A Ifel_ck / @0x01B0E Ck(3,219)=0 -> @0x01B12 Aot_set Slot 2
 *   sce 1 msg 0 im Tuer-Rechteck, Else @0x01B2A Door_aot_set. (= Aot_reset-Form `46 ss 01 31 mm 00 ff ff 00 00`,
 *   ROOM2030 sub01 @0x01E18.)
 * ROOM20B0 = geflutete 2000-Variante: sub00 @0x01AAA Ck(3,235)=0 -> 35 Emitter EM024 @0x01AAE.. (Wasserwand
 *   x 7200 vor der Ost-Tuer), Zone Slot 7 @0x01DF2 -> sub05 Auswasch-Szene, @0x01E06 Evt_exec sub02 (Eintritt:
 *   Pos_set (2380,-1800,-6355) @0x01F08, Cut 11 @0x01F00); sub05 setzt @0x01FBC `22 03 eb 01` Set(3,235)=1.
 *   Else-Zweig @0x01E0A: die drei Strahlen @0x01E96/@0x01EA6/@0x01EB6 (gleiche Bytes wie 2000). Tuer Slot 3
 *   @0x019C2 fuehrt direkt nach ROOM2030 Cut 0 (6550,-5400,-100) Richtung 2048 — in 2000 fuehrt Slot 3 nach 2070.
 *   Kameras: 20B0 Cut 0..4 = 2000 Cut 0..4, 20B0 5/6/7/8/9 = 2000 6/7/8/9/10, 20B0 10 = 2000 14 (Tabellen
 *   RDT+0x60, 32 B je Cut, selbst verglichen). Tuern nach 2000 tragen Cut 0 (1260 @0x114A), 4 (1260 @0x11AA),
 *   10 (2050 @0x1C4A), 13 (2070 @0xA3A/@0xA9A), 14 (2010 @0x746).
 * ROOM2030 Bruecke = main00 @0x01B6C Obj_model_set obj 0 `2d 00 00 00 00 00 0a 00 00 00 cd fb f8 f8 ed fe ...`
 *   (Typ 0, Band 0, Lage (-1075,-1800,-275); MD1 @0x0020CC 636 B: 18 Dreiecke, x -899..900, z -5685..5686, y 0).
 *   Von keinem Skript bewegt. Ebenen: Ring Band 3 y -5400, Mittelsteg Band 1 y -1800 (PSX-y: kleiner = hoeher).
 *   Begehbare Steg-Zelle: SCA Region 0..4 Zelle 12 (-2050,-5950,2000,11400) u0 00 Band 1 (in allen 5 Regionen
 *   gleich); Sperren per Sca_id_set-Form ROOM2050 sub04 @0x02346 `37 0r 32 ff` (Region r, Zelle, u0 := ff).
 * FORM Objekt-Fahrt (derselbe Raum): sub03 @0x01ED2 `2e 03 01` Work_set, @0x01ED6 `2f 01 88 ff` Speed_set(1,-120),
 *   @0x01EDA `0d 00 06 00 2d 00` For 45 { `30` Add_speed `02` `02` Evt_next x2 `00` }, @0x01EE6 `34 01 d0 d5`
 *   Member_set(1,Endlage); 45 x 120 = 5400 = Hoehendifferenz Ring/Buero. Pad-Sperre `22 02 07 01` @0x01E30,
 *   Cut_auto 0 @0x01E34, Fahrt-Kamera `29 0a` @0x01E60, Rueckweg `3c 01` @0x01EC2, `22 02 07 00` @0x01EC4.
 * ROOM2030 Stelle Pult: Nordwand z -10350 (SCA Zelle 24 Band 3 z -12350..-10350), Rueckprojektion des roten
 *   Markers in schaltschrank.png (Cut 2, Bild (45,128)) auf z = -10350 -> Welt (-89,-7079); Pultkanten Bild
 *   (30,60)/(20,180) -> x 125..706, y -8506..-6242. Nordstreifen des Rings z -10350..-9000 (Gelaender Zelle 20).
 * ROOM2030 Stelle Ventil 13: der Tank rechts der Treppe (schaltschrank.png) = SCA Zelle 2 Band 1 (-3050,-6900,
 *   1000,500) am Fuss der Treppe (Zone Slot 8 sce 12 @0x01B8E (-6050,-8400,3700,1400)); Landeplatz Band 1
 *   x -2550..-50, z -8500..-6900 (Zellen 1/6/11 ringsum).
 * ROOM2030 Stelle Ventile (Platzhalter): Ost-Streifen des Rings x 5900..7250 (Zellen 18/19) an der Ost-Tuer
 *   (Slot 0 @0x01ACA Rechteck (6850,-1950,1000,3500)); Cut 15 (@0x00240, flag 1) zeigt dort die Schilder 04/05/06.
 * ROOM2030 Kameras: Cut 4 @0x000E0 (-4253,-9340,7013)->(-1841,-4606,2549) = Draufsicht auf Becken + Steg
 *   (Projektion des Stegs: Bild x 100..210, y 73..221) = Fahrt-Kamera der Bruecke und der Flut (PORT-WAHL).
 * ROOM2050 Hebel = Slot 6 sce 3 Ereignis 4 (sub00 @0x01E28), sub04 @0x022E6 (Frage msg 0, Set(3,104),
 *   Speed_set(1,-20)/For 180). Schaltpult = main00 @0x01D30 Ifel_ck/@0x01D34 Ck(5,7)=1 -> @0x01D38 Slot 8 sce 3
 *   Ereignis 9 (4000,-8450,1800,800) Band 3 + @0x01D4C Cursor obj 8 (Typ 4, (-19554,0,22684)); (5,7) wird nirgends
 *   gesetzt (tot). sub09 @0x023F2 `29 0f` Cut 15, Slot 8 aus, `22 05 04 01` `22 05 05 01` `22 05 06 01`; sub01
 *   @0x02182..@0x021E0 (5,4)=1 & Sce_key_ck 0x01/0x04/0x02/0x08 -> sub05..08 (Cursor +-200 z/x), @0x021E2..@0x021F8
 *   (5,5)=1 & 0x40 -> sub10 (Ausstieg @0x0240C: Cursor parken, Slot 8 wieder sce 3 Ereignis 9, (5,4..6)=0,
 *   (2,0)/(2,2)=0, Cut_old, Cut_auto 1), @0x021FA (5,6)=1 -> Set(2,0)=1 Set(2,2)=1. KEINE Schalterzonen.
 *   Vorbild Schalterlogik ROOM2060 sub01 @0x011BC.. (Cursor-Zone Member_cmp 0x0F + Sce_key_ck 0x40 -> Kippen),
 *   Loesung @0x01296..@0x012AE (Bits -> Set(3,126)). Vorbild Zielwert 80: include/re15_panel_zeiger.h (RE2
 *   ROOM2130 sub04 @0x01752 cmp(var5 == 80), Ruhe 30 Bilder @0x0171C, Bestaetigung se 0x0C @0x01762).
 *   Cut 15 (@0x00240, (-19628,-17442,22616) Nadir): Cursor (-19554,-900,22684) -> Bild (160.9,119.1), 200 Einheiten
 *   = 2.515 px (H 208, vz 16542). Hebel im BSS-Bild ROOM20515: Mitten x 88/133/178 (gross), 238 (klein), y 125;
 *   Skala "0..100": Ziffern-Spalten 0 @165.5 ... 90 @248.5, 100 @256.5 (0.91 px je Punkt), rotes Band 80..100
 *   x 239..256 (y 74), Laufschiene y 70..73 (Pixel selbst gemessen, Dossier §1.4).
 * ROOM2080 Rechner = sub00 @0x00C1A Slot 4 sce 3 Ereignis 2 (nur (3,245)=0), sub02 @0x00C86..@0x00CF1:
 *   Frage msg 0, Ja -> @0x00C94 Set(3,245)=1, Slot 4/3 aus, Tuer 1 an, Pad-Sperre, Sleep 20, Cut 7,
 *   @0x00CC4 Work_set(3,0) Wasser, @0x00CC8 `2f 01 32 00` Speed_set(1,+50), @0x00CCC For 148 (148 x 50 = 7400),
 *   Sleep 20, Cut 3, Cut_auto 1, Freigabe, Plc_ret, Endif @0x00CEE, Evt_end @0x00CF0. Ifel-Laenge @0x00C8E 0x60.
 * Wasserflaeche (PORT-PROP fuer 2030, das keine hat): ROOM2080 Prop 0 (MD1 @0x000D98 4572 B, 108 Vierecke
 *   x 0..21600 z 0..16200, TIM @0x026408 16928 B; Obj_model_set @0x00C2E Typ 3, Flags 0x000a|1, Attribut 0x001a),
 *   eingebacken in gen/ventilkette_r41_wasser.inc (tools/r41_ventilkette/gen_r41_ventilkette.py). Zwei Flaechen
 *   decken den Schacht (Aussenwaende x -9700..9250, z -12350..12000; Buero bis z 20000).
 *
 * ============================================================================================================
 * PORT-WAHLEN (benannt; Form jeweils belegt)
 * ============================================================================================================
 * - Bruecke OBEN = Ringebene y -5400 (die naechste Ebene ueber Band 1 in diesem Raum; Fahrt 30 x 120 = 3600 in der
 *   Lift-Form sub03). Solange oben: Steg-Zelle 12 gesperrt (Sca_id_set-Form), Pult meldet msg 40 (Druck null,
 *   Ventil 13 zu); Ventil 13 (am Tank) oeffnen -> Pult fragt "Lower the bridge?" -> Fahrt unter Cut 4.
 * - Ventilraetsel-Platzhalter: EIN Platz an den Ventilen 04/05/06 (Ost-Streifen), Frage -> Ja -> Warnung ->
 *   Sleep 1800 (= 60 s bei 30 Takten) im selben Faden (Form ROOM2000 sub07 @0x01B36: For/Evt_next-Schleife eines
 *   Hintergrundfadens) -> Pad-Sperre, Cut 4, Wasser steigt 324 x 50 (Rate des Rechners @0x00CC8) vom Beckengrund
 *   y 0 bis zur Tuerebene y -16200; der C-Takt ertraenkt den Spieler, sobald die Flaeche 900 ueber seinen Fuessen
 *   steht (hp := -1 wie RE15_KILL_AT, main.c; blaue Blende Kanal 0 in der Form ROOM11F0 @0x174a `56 00 02 07 00 00`
 *   + `57`, hier ABR 0 + Maske Blau; Spritzer = Wasser-Schritt-Effekt FUN_80045630 Code 0x06050D00).
 * - Verlassen waehrend des Zaehlers (jede Tuer) = Flut gelaufen: Bits 103 + 100. Ost-Tuer Slot 0 (-> 2070) wird
 *   dabei auf ROOM20B0 Cut 11 umgelenkt (Ziel der Szene @0x01F00/@0x01F08) — der Gegenweg der 20B0-Tuer Slot 3
 *   @0x019C2, die 2070 ebenfalls ueberspringt. Danach dauerhaft (Bit 103).
 * - Nach der Flut (103, nicht 107): 2030 steht bis 1800 ueber dem Ring unter Wasser (y -7200; Abstand wie
 *   ROOM2050 Wasser -3600 zum Steg -5400 @0x01F88), Lift-Rufzonen oben (Slots 4/6) = Text msg 1 (Original-Satz
 *   "The elevator is not responding...", Form sub01 @0x01E18). 2000 -> 20B0 fuer jede Tuer nach 2000 (Cut-
 *   Zuordnung oben), in 20B0 die drei Strahlen @0x01E96.. unterdrueckt (Nutzer: "kein Wasser mehr runter laufen").
 * - 2050: Hebel (Ereignis 4) vor der Loesung = Sperrsatz msg 40 (Form ROOM2060 sub12 @0x015D0). Pult: Slot 8 +
 *   Cursor wie @0x01D38/@0x01D4C angelegt, Einstieg = Beschreibung + "Will you push them? " (ROOM11F0 msg 1
 *   Bytes) + sub09-Bytes ohne (5,5) (sonst beendet SQUARE das Pult, sub01 @0x021E2); Kippen = SQUARE auf einer
 *   Hebelzone (2060-Form), Abbruch = CROSS (Ausstieg sub10 @0x0240C), Gewichte 30/20/40 (gross) + 10 (klein),
 *   einzige Loesung 30+40+10 = 80 (alle 16 Kombinationen gezaehlt, Riegel). Loesung -> 30 Ruhebilder (RE2
 *   @0x0171C) -> Bit 106, se 0x0C (RE2 @0x01762), "Power supply OK." (ROOM11F0 msg 2 Bytes), Ausstieg sub10.
 *   Zeiger = roter Laeufer auf der Schiene y 70..73 (RE2 ROOM2130: Zeiger unter der waagerechten Skala).
 * - 2080: Rechner (Ereignis 2) nach der Flut = Original sub02 + Set(9,107) + msg 40 im Ja-Zweig; war der Rechner
 *   schon gelaufen ((3,245)=1), bleibt die Stelle fuer 2030 bedienbar (nur Frage + Set + Satz).
 */
#ifndef RE15_VENTILKETTE_R41_H
#define RE15_VENTILKETTE_R41_H

#include <stdint.h>
#include "re15_rdt.h"

/* ---- Bank-9-Bits (VERTRAG §1.1: C = 100-103, 106, 107; Zensus unit_r36_kartenbits) ------------------------- */
#define RE15_R41C_BANK               9
#define RE15_R41C_BIT_LEITER_FREI    100   /* 2000/20B0: Leiter nach 2050 frei (nach der Flut)              */
#define RE15_R41C_BIT_VENTIL13       101   /* 2030: Zulaufventil 13 geoeffnet                               */
#define RE15_R41C_BIT_BRUECKE_UNTEN  102   /* 2030: Bruecke gesenkt (y -1800, Zelle 12 offen)               */
#define RE15_R41C_BIT_FLUT_GELAUFEN  103   /* 2030: Ventile geoeffnet und Raum verlassen -> 2000 wird 20B0  */
#define RE15_R41C_BIT_PULT2050       106   /* 2050: Schaltpult auf 80 V                                     */
#define RE15_R41C_BIT_2030_LEER      107   /* 2080-Rechner gelaufen -> 2030 wieder ohne Wasser              */

/* ---- Raeume (Leon; Elza = |1) -------------------------------------------------------------------------------- */
#define RE15_R41C_RAUM_2000   0x2000u
#define RE15_R41C_RAUM_2010   0x2010u
#define RE15_R41C_RAUM_2030   0x2030u
#define RE15_R41C_RAUM_2050   0x2050u
#define RE15_R41C_RAUM_2070   0x2070u
#define RE15_R41C_RAUM_2080   0x2080u
#define RE15_R41C_RAUM_20B0   0x20B0u
#define RE15_R41C_RAUM_1260   0x1260u

/* ---- Text-Ids (VERTRAG §1.2) ---------------------------------------------------------------------------------- */
#define RE15_R41C_MSG_LEITER          40   /* 2000/20B0 */
#define RE15_R41C_MSG_PULT_OHNE       40   /* 2030 */
#define RE15_R41C_MSG_V13_FRAGE       41
#define RE15_R41C_MSG_V13_OFFEN       42
#define RE15_R41C_MSG_BRUECKE_FRAGE   43
#define RE15_R41C_MSG_BRUECKE_UNTEN   44
#define RE15_R41C_MSG_VENTILE_FRAGE   45
#define RE15_R41C_MSG_WARNUNG         46
#define RE15_R41C_MSG_V13_ZUSTAND     47
#define RE15_R41C_MSG_VENTILE_ZUSTAND 48
#define RE15_R41C_MSG_HEBEL_OHNE      40   /* 2050 */
#define RE15_R41C_MSG_PULT_FRAGE      41   /* = ROOM11F0 msg 1 "Will you push them? " @0x18D8 */
#define RE15_R41C_MSG_PULT_OK         42   /* = ROOM11F0 msg 2 "Power supply OK." @0x18F1     */
#define RE15_R41C_MSG_PULT_BESCHR     43
#define RE15_R41C_MSG_PULT_ZUSTAND    44
#define RE15_R41C_MSG_RECHNER_2030    40   /* 2080 */
#define RE15_R41C_MSG_LIFT_ORIGINAL    1   /* 2030 msg 1 "The elevator is not responding..." @0x2038 */

/* ---- Ereignisse (Port-Ids < RE15_RDT_MAX_SUB_SCD, ausserhalb der Raum-Subs: 2030 hat sub00..06) ------------- */
#define RE15_R41C_EV_PULT      24
#define RE15_R41C_EV_VENTIL13  25
#define RE15_R41C_EV_VENTILE   26
#define RE15_R41C_EV_2050_HEBEL 4   /* Original-Ereignis des Hebels (sub00 @0x01E28)  */
#define RE15_R41C_EV_2050_PULT  9   /* Original-Ereignis des Pults (main00 @0x01D38)  */
#define RE15_R41C_EV_2050_AUS  10   /* Original sub10 = Ausstieg aus dem Pult          */
#define RE15_R41C_EV_2080_RECHNER 2 /* Original-Ereignis des Rechners (sub00 @0x00C1A) */

/* ---- Plaetze 2030 (frei: 0-3 Tueren, 4-7 Lifte, 8 Treppe, 9 Sicherung, 10/11 Items, 51-63 RVD) --------------- */
#define RE15_R41C_SLOT_PULT      12
#define RE15_R41C_SLOT_VENTIL13  13
#define RE15_R41C_SLOT_VENTILE   14
#define RE15_R41C_SLOT_2000_LEITER 4   /* main00 @0x0169E */
#define RE15_R41C_SLOT_2030_OSTTUER 0  /* main00 @0x01ACA */
#define RE15_R41C_SLOT_2050_PULT  8    /* main00 @0x01D38 */
#define RE15_R41C_SLOT_2080_RECHNER 4  /* sub00  @0x00C1A */
#define RE15_R41C_SAT            0x31  /* Vorwaertspunkt-Test wie alle Original-Plaetze dieser Raeume */
#define RE15_R41C_PAUSE          0xFFFFu
/* Rechtecke (x, z, Breite, Tiefe) + Band — Herleitung im Kopf. */
#define RE15_R41C_PULT_RECT      -300, -10850, 1300, 1000
#define RE15_R41C_PULT_BAND      3
#define RE15_R41C_VENTIL13_RECT  -3050, -6900, 1300, 1000
#define RE15_R41C_VENTIL13_BAND  1
#define RE15_R41C_VENTILE_RECT   4900, -2000, 1400, 3500
#define RE15_R41C_VENTILE_BAND   3

/* ---- Bruecke (obj 0 @0x01B6C) ------------------------------------------------------------------------------- */
#define RE15_R41C_BRUECKE_OBJ      0
#define RE15_R41C_BRUECKE_Y_UNTEN  (-1800)   /* @0x01B6C Lage y                                 */
#define RE15_R41C_BRUECKE_Y_OBEN   (-5400)   /* Ringebene (Lift-Endlage @0x01F02 `34 01 e8 ea`) */
#define RE15_R41C_BRUECKE_SCHRITT  120       /* @0x01EF2 `2f 01 78 00`                           */
#define RE15_R41C_BRUECKE_SCHRITTE 30        /* 30 x 120 = 3600                                  */
#define RE15_R41C_STEG_ZELLE       12        /* SCA Zelle 12 je Region (u0 00 = begehbar)        */
#define RE15_R41C_FAHRT_CUT        4         /* Draufsicht @0x000E0                              */

/* ---- Wasser (Port-Props obj 4/5 in 2030, ROOM2080 Prop 0) --------------------------------------------------- */
#define RE15_R41C_WASSER_OBJ_A     4
#define RE15_R41C_WASSER_OBJ_B     5
#define RE15_R41C_WASSER_X         (-10800)  /* Flaeche 0..21600 -> x -10800..10800            */
#define RE15_R41C_WASSER_ZA        (-12400)  /* Flaeche A 0..16200 -> z -12400..3800            */
#define RE15_R41C_WASSER_ZB        3800      /* Flaeche B -> z 3800..20000                      */
#define RE15_R41C_WASSER_PARK_Z    60000     /* ausserhalb jeder Kamera, bis der Zaehler laeuft */
#define RE15_R41C_WASSER_PARK_Y    1800
#define RE15_R41C_WASSER_Y_BODEN   0         /* Beckengrund Band 0                              */
#define RE15_R41C_WASSER_Y_RUHE    (-7200)   /* nach der Flut: 1800 ueber dem Ring (-5400)       */
#define RE15_R41C_WASSER_Y_ENDE    (-16200)  /* Tuerebene Band 9                                 */
#define RE15_R41C_WASSER_SCHRITT   50        /* ROOM2080 sub02 @0x00CC8 `2f 01 32 00`           */
#define RE15_R41C_WASSER_SCHRITTE  324       /* 324 x 50 = 16200                                 */
#define RE15_R41C_WASSER_TYP       3         /* @0x00C2E pc[2]                                   */
#define RE15_R41C_WASSER_FLAGS     0x000Bu   /* @0x00C2E pc[6..7] 0x000a | 1 (op_obj_model_set)  */
#define RE15_R41C_WASSER_ATTR      0x001Au   /* @0x00C2E pc[8..9]                                */
#define RE15_R41C_ERTRINK_TIEFE    900       /* Wasser 900 ueber den Fuessen = halbe Koerperhoehe (Typ-4-Anhebung
                                              * -900 @0x8002c24c als Mass einer Koerperhaelfte)   */
#define RE15_R41C_SPRITZER_TAKT    6
#define RE15_R41C_ZAEHLER_BILDER   1800      /* 60 s bei 30 SCD-Takten (Sleep `09 0a 08 07`)     */
#define RE15_R41C_FLAG_TIMER       0x3D      /* Bank 5 Wort 1 (geloescht beim Raum-Init @0x8003ec1c) */
#define RE15_R41C_FLAG_FLUT        0x3E
/* Blaue Blende: Kanal 0, ABR 0 (B/2+F/2), Maske Blau (Bit 0), statisch (Form ROOM11F0 @0x174a `56 00 02 07 00 00`
 * + `57 00 <level>`; Helligkeit = level >> 7, re15_fade.h). */
#define RE15_R41C_FADE_KANAL   0
#define RE15_R41C_FADE_ABR     0
#define RE15_R41C_FADE_MASKE   0x1
#define RE15_R41C_FADE_LEVEL   0x5000u      /* -> 160 */
#define RE15_R41C_FADE_BUCKET  7

/* ---- 2050 Schaltpult ----------------------------------------------------------------------------------------- */
#define RE15_R41C_PANEL_CUT        15
#define RE15_R41C_PANEL_N          4
#define RE15_R41C_PANEL_GEWICHTE   { 30, 20, 40, 10 }     /* PORT-WAHL, Loesung 30+40+10 = Maske 0xD */
#define RE15_R41C_PANEL_LOESUNG    0xDu
#define RE15_R41C_PANEL_ZIEL       80                     /* RE2 ROOM2130 sub04 @0x01752 */
#define RE15_R41C_PANEL_RUHE       30                     /* RE2 @0x0171C sleep 0x1E      */
/* Cursor-Welt <-> Bild unter Cut 15: Bild = (160.9 + (x+19554)*2.515/200, 119.1 - (z-22684)*2.515/200).
 * Hebelmitten im Bild x 88/133/178/238, y 125 -> Welt x -25350 / -21772 / -18193 / -13422, z 22215.
 * Zonen 2400 x 4400 (Hebelkoerper y 85..165 = z 19000..25400), Form der 2060-Zonen 2050 x 2050 @0x00F34. */
#define RE15_R41C_PANEL_ZONE_W     2400
#define RE15_R41C_PANEL_ZONE_D     4400
#define RE15_R41C_PANEL_ZONE_Z0    19800
#define RE15_R41C_PANEL_X0         { -26550, -22972, -19393, -14622 }
#define RE15_R41C_CURSOR_OBJ       8
/* Zeiger auf der Skala (320x240): x = 165.5 + 0.91 * Wert, Schiene y 70..73, rotes Band 239..256. */
#define RE15_R41C_SKALA_X0_100     1655     /* x 0 in Zehntelpixeln */
#define RE15_R41C_SKALA_PX_100     91       /* Zehntelpixel je Punkt */
#define RE15_R41C_SKALA_Y          70
#define RE15_R41C_SKALA_H          4
#define RE15_R41C_LAMPE_Y          168      /* gruene Marke unter einem EIN-Hebel (PORT-WAHL) */

/* ---- API ------------------------------------------------------------------------------------------------------ */
/* Nach dem Init-Lauf (scd_room_setup.c Installer-Block; main.c Boot-/CONTINUE-Weg). rdt darf NULL sein. */
void re15_ventilkette_r41_install(uint16_t room_id, const re15_rdt_t *rdt);
/* scd_event_fire-Kette: Port-Programm fuer (Raum, Ereignis) oder NULL (= Original). */
const uint8_t *re15_ventilkette_r41_ereignis(uint16_t room_id, uint8_t event_id);
/* op_sce_espr_on: 1 = diesen Sce_espr_on ueberspringen (20B0/20B1 Strahlen @0x01E96/@0x01EA6/@0x01EB6 bei Bit 100). */
int re15_ventilkette_r41_espr_unterdrueckt(uint16_t room_id, const uint8_t *pc, const uint8_t *raw);
/* Spiel-Takt (game_step_common.c, hinter den anderen Runden-Ticks). */
void re15_ventilkette_r41_tick(void);
/* Zuruecksetzen (Testaufbau). */
void re15_ventilkette_r41_reset(void);
/* Eingebackene Wasserflaeche fuer main.c: obj_id fuer k = 0/1 im Raum (sonst -1), MD1/TIM-Bytes. */
int re15_ventilkette_r41_obj_id(uint16_t room_id, int k);
const uint8_t *re15_ventilkette_r41_md1_bytes(int *out_size);
const uint8_t *re15_ventilkette_r41_tim_bytes(int *out_size);
/* Zeichner (main.c, nach dem 11F0-Zeiger): Zeiger + Hebelmarken des 2050-Pults, nur solange es aktiv ist. */
void re15_ventilkette_r41_zeichnen(void);

/* ---- Messhaken (Riegel) --------------------------------------------------------------------------------------- */
int      re15_ventilkette_r41_timer_laeuft(void);
int      re15_ventilkette_r41_flut_laeuft(void);
int      re15_ventilkette_r41_ertrunken(void);
unsigned re15_ventilkette_r41_schalter(void);      /* Maske der EIN-Hebel (Bit 0..3) */
int      re15_ventilkette_r41_spannung(void);      /* Summe der Gewichte der EIN-Hebel */
int      re15_ventilkette_r41_spannung_aus_maske(unsigned maske);
int      re15_ventilkette_r41_panel_aktiv(void);
int      re15_ventilkette_r41_panel_zone(int32_t x, int32_t z);   /* -1 oder Hebel 0..3 */
int      re15_ventilkette_r41_zeiger_x(int wert);                 /* Bild-x des Zeigers */
int      re15_ventilkette_r41_cut_20b0(int cut_2000);             /* Cut-Zuordnung 2000 -> 20B0 */
const uint8_t *re15_ventilkette_r41_programm(const char *name, int *out_len);
const uint8_t *re15_ventilkette_r41_text(uint16_t room_id, uint8_t id, int *out_len);
int      re15_ventilkette_r41_wasser_prop(int k);                 /* Pool-Index der Flaeche k oder -1 */

#endif /* RE15_VENTILKETTE_R41_H */
