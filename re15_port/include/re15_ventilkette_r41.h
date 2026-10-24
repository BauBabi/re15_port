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
 *   + `57`: ABR 2 (subtraktiv, der einzige Blendmodus der ausgelieferten Skripte) mit Maske Rot+Gruen (Bits 2+1) -
 *   die Kachel zieht Rot und Gruen ab, Blau bleibt = blauer Schleier; Spritzer = Wasser-Schritt-Effekt FUN_80045630
 *   Code 0x06050D00). Gemessen 2026-10-23 22:05 (Flut-Lauf, Bildmitte): die erste Fassung "ABR 0 + Maske Blau"
 *   zeichnete der PC-Renderer als B-F (render_pc.c kennt nur ABR 1/2) -> (29,36,29) wurde (30,37,0) = Blau WEG.
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
#define RE15_R41C_MSG_VENTILE_LEER    49   /* Nachbesserung 1 (M8): Zustandssatz nach dem Entleeren (Bit 107) */
#define RE15_R41C_MSG_HEBEL_OHNE      40   /* 2050 */
#define RE15_R41C_MSG_PULT_FRAGE      41   /* = ROOM11F0 msg 1 "Will you push them? " @0x18D8 */
#define RE15_R41C_MSG_PULT_OK         42   /* = ROOM11F0 msg 2 "Power supply OK." @0x18F1     */
#define RE15_R41C_MSG_PULT_BESCHR     43
#define RE15_R41C_MSG_PULT_ZUSTAND    44
#define RE15_R41C_MSG_RECHNER_2030    40   /* 2080 */
/* Orchestrator-Entscheid C-E2 (AUFTRAG.md 2026-10-24 05:25, Abnahme 0 M5): bis Bit 107 sind die Zugaenge in den
 * gefluteten Ring von 2030 gesperrt (Aktionstaste -> Port-Text, kein Raumwechsel). Ids laut VERTRAG §1.2 (ergaenzt). */
#define RE15_R41C_MSG_RING_20B0       41   /* 20B0/20B1 */
#define RE15_R41C_MSG_RING_2070       40   /* 2070/2071 */
#define RE15_R41C_MSG_LIFT_ORIGINAL    1   /* 2030 msg 1 "The elevator is not responding..." @0x2038 */

/* ---- Ereignisse (Port-Ids < RE15_RDT_MAX_SUB_SCD, ausserhalb der Raum-Subs: 2030 hat sub00..06) ------------- */
#define RE15_R41C_EV_PULT      24
#define RE15_R41C_EV_VENTIL13  25
#define RE15_R41C_EV_VENTILE   26
#define RE15_R41C_EV_2050_HEBEL 4   /* Original-Ereignis des Hebels (sub00 @0x01E28)  */
/* Nachbesserung 1 (Abnahme 0, M1): DERSELBE Hebel-Platz Slot 6 fuehrt Ereignis 2, sobald die drei Kisten im Becken
 * liegen: ROOM2050/2051 sub01 @0x02006 `06 00 1c 00` Ifel_ck, @0x0200A `21 03 6d 00` Ck(3,109)=0, @0x0200E/@0x02012/
 * @0x02016 `21 05 00 01`/`21 05 01 01`/`21 05 02 01` Ck(5,0..2)=1 (gesetzt @0x0203C ff. bei Member_cmp 0x0F == 3/4/5),
 * @0x0201A `46 06 03 31 ff 00 18 02 00 00` Aot_reset Slot 6 sce 3 sat 0x31 -> Ereignis 2 = sub02 @0x0220E: Frage msg 0
 * `2b 00 ff ff`, Ja `21 0c 1f 00` @0x02218, Set(3,109) `22 03 6d 01` @0x0221C, Wasser obj 0 y 0 `34 01 00 00` @0x02268,
 * Cut 4 `29 04` @0x02270, Speed_set(1,-20) `2f 01 ec ff` @0x0227C + For 180 `0d 00 06 00 b4 00` @0x02280 = das Wasser
 * STEIGT. Einzige Ereignis-2-Stelle in 2050/2051 (scd_dump_room.py: Aot_set/Aot_reset mit `18 02` nur @0x0201A). */
#define RE15_R41C_EV_2050_KISTEN 2  /* sub01 @0x0201A -> sub02 @0x0220E (Hebel mit den Kisten im Becken) */
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
/* Zugaenge in den Ring von 2030 (Tuer-Zensus aller ROOM*.RDT, Ziel Stage 1 Raum 3 mit Ankunft auf dem Ring
 * (6550,-5400,-100) nfloor 3): 20B0/20B1 Slot 3 @0x019C2 `3b 03 02 31 01 00 4a 15 24 e1 ... 00 08 01 03 00 03`,
 * 2070/2071 Slot 1 @0x00A5A `3b 01 02 31 00 00 88 13 24 e1 ... 00 08 01 03 00 03` und Slot 2 @0x00A7A
 * `3b 02 02 31 00 00 12 b2 24 13 ... 00 08 01 03 00 03`. Alle uebrigen Tueren nach 2030 (2020/2021 Slot 1 @0x00866/
 * @0x0088A, 2060/2061 Slot 0 @0x00D9A, 2080/2081 Slot 0 @0x00BA2) kommen auf der Tuerebene y -16200 (nfloor 9) an,
 * also ueber dem Wasser y -7200 -> bleiben offen (Weg zum Rechner 2080). */
#define RE15_R41C_SLOT_20B0_RING  3
#define RE15_R41C_SLOT_2070_RING_A 1
#define RE15_R41C_SLOT_2070_RING_B 2
#define RE15_R41C_RAUM_2030_ID    0x03   /* Door_aot_set pc[23] (Stage 1 = STAGE2) */
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
#define RE15_R41C_WASSER_PARK_Z    60000     /* ausserhalb jeder Kamera, bis die Flut laeuft: PORT-WAHL des Parkplatzes, */
#define RE15_R41C_WASSER_PARK_Y    1800      /* Form = ROOM2050 Wasser obj 0 "ausser Sicht" bei y -16200 @0x01D92       */
#define RE15_R41C_WASSER_Y_BODEN   0         /* Beckengrund Band 0                              */
#define RE15_R41C_WASSER_Y_RUHE    (-7200)   /* nach der Flut: 1800 ueber dem Ring (-5400)       */
#define RE15_R41C_WASSER_Y_ENDE    (-16200)  /* Tuerebene Band 9                                 */
#define RE15_R41C_WASSER_SCHRITT   50        /* ROOM2080 sub02 @0x00CC8 `2f 01 32 00`           */
#define RE15_R41C_WASSER_SCHRITTE  324       /* 324 x 50 = 16200                                 */
#define RE15_R41C_WASSER_TYP       3         /* @0x00C2E pc[2]                                   */
#define RE15_R41C_WASSER_FLAGS     0x000Bu   /* @0x00C2E pc[6..7] 0x000a | 1 (op_obj_model_set)  */
#define RE15_R41C_WASSER_ATTR      0x001Au   /* @0x00C2E pc[8..9]                                */
#define RE15_R41C_ERTRINK_TIEFE    900       /* PORT-WAHL (Ertrinken gibt es im Original nicht): Wasser 900 ueber
                                              * den Fuessen. ENTLIEHENES Mass, KEINE Koerperhoehe: @0x8002c24c
                                              * `addiu v0,v0,-900` hebt in der Objekt-Schleife (Schritt 148
                                              * `addiu s6,s6,148` @0x8002c1f8) die y-Lage (+0x60) von Typ-4-Objekten
                                              * an (`lbu v1,8(s1)` / `ori v0,zero,0x4` / `bne` @0x8002c234-3c). */
/* Spritzer: KEINE Rate — ein Ereignis, wenn die Flaeche die Fuesse erreicht (ventilkette_r41.c flut_tick; Original
 * nur je Fuss-Aufsetzer @0x8003253c -> FUN_80045630 @0x800456b8). Die fruehere Rate "alle 6 Takte" war ohne Beleg. */
#define RE15_R41C_ZAEHLER_BILDER   1800      /* 60 s bei 30 SCD-Takten (Sleep `09 0a 08 07`)     */
/* Marker des Programms fuer den C-Takt: Bank 5 WORT 0 (Raum-Scratch, beim Raumladen gewischt FUN_8003ecec
 * @0x8003ed74; Wort 1 taugt NICHT — es ist der Ein-Bild-Handshake, den FUN_8003ebf4 @0x8003ec1c am Ende JEDES
 * VM-Laufs loescht). 2030 benutzt Bank 5 nicht (0 Treffer `21 05`/`22 05` in ROOM2030.RDT); der Installer loescht
 * beide Bits NUR in 2030/2031 (Nachbesserung 1, M3: in fremden Raeumen sind es Original-Bits, z. B. ROOM1171 sub02
 * @0x013B2 `22 05 1e 01` im Init-Takt = Kraehen-Flag DIVE-ARM @0x801155e8; das Raumladen wischt Wort 0 ohnehin,
 * scd_room_setup.c @0x8003ed74). */
#define RE15_R41C_FLAG_TIMER       0x1D
#define RE15_R41C_FLAG_FLUT        0x1E
/* Blaue Blende: Kanal 0, ABR 2 (B-F subtraktiv wie @0x174a), Maske Rot+Gruen (Config-Bits 2+1: Blau bleibt stehen),
 * statisch (Form ROOM11F0 @0x174a `56 00 02 07 00 00`
 * + `57 00 <level>`; Helligkeit = level >> 7, re15_fade.h). */
#define RE15_R41C_FADE_KANAL   0
#define RE15_R41C_FADE_ABR     2            /* @0x174a `56 00 02 ..` (FUN_800217b0: abr = Byte 2)                */
#define RE15_R41C_FADE_MASKE   0x6          /* Rot (Bit 2) + Gruen (Bit 1) abziehen, Blau (Bit 0) bleibt          */
#define RE15_R41C_FADE_LEVEL   0x5000u      /* -> Helligkeit 160 (level >> 7 @0x800218d0); PORT-WAHL: tiefer Schleier */
#define RE15_R41C_FADE_BUCKET  7            /* OT-Fach der SCD-Blende: Handler 0x56 @0x80042a58 `ori a3,7` (scd_vm.c op_fade_config) */

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
/* Zeiger auf der Skala (320x240): x = 165.5 + 0.91 * Wert (Ziffern 0 @165.5 .. 100 @256.5 = 91 px je 100 Punkte),
 * Schiene y 70..73, rotes Band 239..256. Ganzzahlig: x = (16550 + 91 * Wert + 50) / 100. */
#define RE15_R41C_SKALA_X0_100     16550    /* x 0 in Hundertstelpixeln */
#define RE15_R41C_SKALA_PX_100     91       /* Hundertstelpixel je Punkt (= 91 px je 100 Punkte) */
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
/* Zeichner-Abfrage (main.c zeichnet, nach dem 11F0-Zeiger): nr 0 = Zeiger, nr 1..4 = gruene Marke unter dem
 * EIN-Hebel nr; 1 = zeichnen, Rechteck in *x0/*y0/*w/*h (320x240). Nur solange das 2050-Pult aktiv ist. */
int re15_ventilkette_r41_sicht(int nr, int *x0, int *y0, int *w, int *h);

/* ---- Messhaken (Riegel) --------------------------------------------------------------------------------------- */
int      re15_ventilkette_r41_timer_laeuft(void);
int      re15_ventilkette_r41_flut_laeuft(void);
int      re15_ventilkette_r41_ertrunken(void);
int      re15_ventilkette_r41_spritzer(void);     /* Spritzer-Ereignisse seit dem Raumstart (Riegel) */
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
