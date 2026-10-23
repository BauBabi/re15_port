# Runde 41 — Spur C "ventilkette" (STAGE2-Kette 2000 / 2030 / 20B0 / 2050 / 2080)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, Basis master 116e8b0f. Begonnen 2026-10-23 17:12.
Wortlaut des Nutzers: `AUFTRAG.md` ("Vorab vereinbart"); Zuteilung: `VERTRAG.md` (Bank 9 Bits 100-103, 106, 107;
Text-Ids 2000/20B0 40, 2030 40-49, 2050 40-44, 2080 40-41). Vorrunde: `befunde_runde40/UEBERGABE.md` "Befund STAGE2".

## 0. Stand / Protokoll (neueste zuerst)

* 17:12 Baum sauber (status leer, HEAD 116e8b0f). AUFTRAG/VERTRAG/UEBERGABE gelesen. SCD-Dumps aller Kettenraeume
  (tools/scd_dump_room.py) + Texttabellen (tools/rdt_msgdump.py) gezogen, Belege unten in §1. Noch kein Code.

## 1. RE-Belege (Original, selbst aus den Bytes gelesen)

Alle Datei-Offsets beziehen sich auf `re15_port/shared_assets/PSX/STAGE2/ROOMxxxx.RDT` (Leon); die Elza-Varianten
ROOM2001/2031/2051/2081/20B1 sind in den hier zitierten Regionen byte-gleich (Dumps identisch; 2001 hat eine andere
Szenen-Sub, die Tuer-/Objekt-Saetze von main00 sind gleich).

### 1.1 ROOM2000 — die Leiter nach 2050 (Punkt 1)
* Tuersaetze main00 @0x0161E..@0x016BD (fuenf `3b`): Slot 0 @0x0161E -> ROOM1260 (hinauf), Slot 1 @0x0163E -> ROOM1260
  Cut 8, Slot 2 @0x0165E -> ROOM2010, Slot 3 @0x0167E (Rechteck (-20300,-8200,1000,4000)) -> ROOM2070 Cut 3,
  **Slot 4 @0x0169E = die Leiter nach ROOM2050**:
  `3b 04 02 31 01 00 30 94 48 26 e8 03 e8 03 8c 23 e8 ea 04 10 00 04 01 05 00 03 00 00 00 00 00 00`
  = sce 2, sat 0x31, Band 1, Rechteck x -27600 z 9800 1000x1000, Ziel (9100,-5400,4100) Richtung 1024,
  Stage 1 Raum 5 (= ROOM2050) Cut 0, nfloor 3. (Port-Tuertabelle gen/tuer_zuordnung.inc S150/T070 ist der
  Gegenweg 2050 Slot 0 @0x1C4A "DOOR16 V5 hinab".)
* Die drei Wasserstrahlen an der Leiterwand: main00 @0x0174C/@0x0175C/@0x0176C je `3a 00 0b 00 00 00 00 19 xx xx 4c eb
  c0 2b 00 04` (ESP 0x0b, x -23900/-23300/-22700, y -5300, z 11200) — IMMER an (kein Ck davor).
* Kein Skript in 2000 liest (3,235); kein Tuersatz in irgendeinem Raum fuehrt nach ROOM20B0 (Port-Tabellen
  gen/tuer_zuordnung.inc + gen/re15_tuer_eigen.inc: 0 Treffer "-> ROOM20B0"). Das Original schaltet 2000 -> 20B0
  also NICHT selbst; 20B0 ist nur per Debug-Menue erreichbar.
* FORM fuer "Tuer wird zum Sperrtext" im selben Stage: ROOM2030 main00 @0x01B0A `06 00 1c 00` Ifel_ck, @0x01B0E
  `21 03 db 00` Ck(3,219)=0 -> @0x01B12 `2c 02 01 31 09 00 ba fa 6c d5 08 07 e8 03 00 00 ff ff 00 00` (Text-Platz
  Slot 2, msg 0 "It's locked from the other side.", GLEICHES Rechteck/Band wie die Tuer) / Else @0x01B2A Tuersatz
  Slot 2. Der Port legt die Leiter-Sperre in genau dieser Form an (Slot 4 -> sce 1, msg 40, gleiches Rechteck).

### 1.2 ROOM20B0 — die geflutete 2000-Variante (Punkte 3/4)
* main00 @0x01962..@0x01A90: dieselben fuenf Tueren wie 2000, aber Slot 3 @0x019C2 ist die OST-Tuer
  `3b 03 02 31 01 00 4a 15 24 e1 e8 03 b8 0b 96 19 e8 ea 9c ff 00 08 01 03 00 03 ...` = Rechteck (5450,-7900,
  1000,3000), Ziel (6550,-5400,-100) Richtung 2048, Stage 1 Raum 3 = ROOM2030 Cut 0, nfloor 3 (der Ring, Band 3).
  In 2000 fuehrt derselbe Slot 3 nach 2070 — die Entwickler haben in 20B0 den Umweg 2070 weggelassen.
* sub00 @0x01AA6 `06 00 64 03` Ifel_ck + @0x01AAA `21 03 eb 00` **Ck(3,235)=0 = Flut noch nicht gelaufen**:
  35 Emitter EM024 (`44 nn 24 05 01 ...` @0x01AAE..@0x01D56, x 7200, y -1800, z -8150..-4750 = die Wasserwand
  vor der Ost-Tuer; Spur A repariert den Effekt), Tuerfluegel obj 3/4 (West) @0x01D6A/@0x01D8C, obj 5/6 (Ost,
  x 5850) @0x01DAE/@0x01DD0, Zone Slot 7 sce 3 Ereignis 5 @0x01DF2 (Rechteck (-17400,-8700,2250,4600), Band 1),
  **@0x01E06 `04 ff 18 02` Evt_exec sub02 = Eintrittsszene**.
  Else @0x01E0A: obj 3 liegt am Boden (-23450,0,-4300) rot (0,-2880,-1024) @0x01E0E, obj 4 angelehnt rot 256,
  die drei Strahlen @0x01E96/@0x01EA6/@0x01EB6 (gleiche Bytes wie 2000 @0x0174C ff.).
* sub02 @0x01EF8: `22 02 07 01` Pad-Sperre, `22 01 1c 01`, `29 0b` Cut 11, `3c 01`, Work_set(1,0) Spieler,
  @0x01F08 `32 00 4c 09 f8 f8 2d e7` Pos_set (2380,-1800,-6355), @0x01F10 `34 04 00 08` Richtung 2048,
  Evt_exec sub03/sub04 (Ost-Fluegel obj 5/6 schwingen: `2f 04 80 00` / `2f 04 80 ff`, For 8), @0x01F24
  `40 00 05 00 d0 c1 2d e7` Plc_dest Modus 5 nach (-15920,-6355), Gosub 8 (wartet auf (5,32)=0).
* sub05 @0x01F5C (Zone Slot 7 = Auswasch-Szene): Cut 7, Pos_set (-15400,-1800,-7305), Plc_dest..., sub06/sub07
  (West-Tuer fliegt: obj 4 `2f 04 40 fe` For 4; obj 3 Keyframes @0x02038..@0x020C6), Cut 15, Sleep 60, Cut 6,
  Plc_ret, Cut_auto 1, **@0x01FBC `22 03 eb 01` Set(3,235)=1**, Sca_id_set 0x13/0x14 -> ff.
* ROOM20B0 hat KEINE Texttabelle (msg_sec 0). Port-Id 40 (Leiter-Satz) muss per re15_msg_install_text kommen.

### 1.3 ROOM2030 — Bruecke, Lifte, Ventile (Punkte 2/3)
* main00 @0x01ACA Tuer Slot 0 (OST, Ring Band 3): `3b 00 02 31 03 00 c2 1a 62 f8 e8 03 ac 0d c0 12 00 00 64 e7 00 08
  01 07 00 00 ...` = Rechteck (6850,-1950,1000,3500), Ziel (4800,0,-6300) Richtung 2048, Stage 1 Raum 7 = ROOM2070
  Cut 0. Slot 1 @0x01AEA -> ROOM2080 (Band 9), Slot 2 @0x01B2A -> ROOM2020 (nur wenn (3,219)=1), Slot 3 @0x01B4C
  -> ROOM2060 (Band 9).
* **Bruecke = obj 0 @0x01B6C** `2d 00 00 00 00 00 0a 00 00 00 cd fb f8 f8 ed fe 00 00 00 00 ...` = Typ 0, Band 0,
  Lage (-1075,-1800,-275), rot 0. Von keinem Skript bewegt (kein `2e 03 00` in 2030).
* Lift A obj 1 (6725, -5400 | -10800, 12050) sub00 @0x01CFA/@0x01D48, Lift B obj 2 (-10675, -5400 | -16200, 4050)
  @0x01D9C/@0x01DEA. **FORM der Objekt-Fahrt (derselbe Raum!)**: sub03 @0x01ED2 `2e 03 01` Work_set(3,1),
  @0x01ED6 `2f 01 88 ff` Speed_set(1,-120), @0x01EDA `0d 00 06 00 2d 00` For 45 { `30` Add_speed, `02` `02`
  Evt_next x2 }, @0x01EE6 `34 01 d0 d5` Member_set(1,-10800). Rueckweg @0x01EF2 `2f 01 78 00` Speed_set(1,+120),
  For 45, @0x01F02 `34 01 e8 ea` Member_set(1,-5400). 45 x 120 = 5400 = die Hoehendifferenz. Lift B: For 90
  (@0x01FB4/@0x01FD0), 90 x 120 = 10800. => 120 Einheiten je Add_speed, Add_speed jeden zweiten Takt (zwei
  Evt_next), Endlage per Member_set(1).
* Spieler-Mitfahrt sub02 @0x01E4C `2e 01 00` + `2f 01 88 ff` + For 45 (gleiche Form), danach @0x01E62 `34 01 d0 d5`
  Member_set(1,-10800), @0x01E66/@0x01E6A `34 12 06 00`/`34 13 06 00` Member_set(0x12/0x13, Band 6).
* Pad-Sperre fuer eine Objektfahrt: sub02 @0x01E30 `22 02 07 01` ... @0x01EC4 `22 02 07 00`; Cut_auto 0 @0x01E34,
  Cut_auto 1 @0x01EC2; Kamera der Fahrt `29 0a` @0x01E60 (Lift A oben) / `29 07` @0x01E8E (unten).
* Texte: msg 0 "It's locked from the other side." @0x2014, 1 "The elevator is not responding...Maybe it should be
  activated from somewhere." @0x2038, 2 "Will you take the Fuse? " @0x208B (Ja/Nein-Form `03`), 3 "You've taken
  the Fuse." @0x20AC. Port-Ids 40..49 sind frei (VERTRAG).
* Ebenen (Runde 40, auswahl.json "ebene"): Ring Band 3 y -5400 (Cuts 0/1/2/7/8/15), Mittelsteg Band 1 y -1800
  (Cuts 3/5/6/14), Buero Band 6 y -10800 (10/11/12), Tuerebene Band 9 y -16200 (9/13). PSX-y: kleiner = hoeher.
* Zone Slot 8 @0x01B8E `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00 02 00 00 00` sce 12 (Rechteck
  (-6050,-8400,3700,1400), Band 0x80) — noch zu klaeren (Kurbel-Stelle?).
* Sicherung: (3,108)=0 -> Slot 9 sce 3 Ereignis 6 @0x01BAA; sub06 @0x01FE4 Frage msg 2, @0x01FF2 `22 03 6c 01`.
* Nie geschaltete Kameras: Cut 14 (pixelgleich Cut 3), Cut 15 (Schachtmitte oben, Blick auf die Doppeltuer Band 3).

### 1.4 ROOM2050 — Hebel, Wasser, Schaltpult (Punkt 5)
* Hebel: main00 setzt nichts; sub00 (3,109)=0-Zweig: (3,104)=0 -> @0x01E28 `2c 06 03 31 03 00 5c 12 e2 d2 20 03 20 03
  ff 00 18 04 00 00` Slot 6 sce 3 **Ereignis 4 = Hebel** (Rechteck (4700,-11550,800,800), Band 3), Slot 7 sce 0.
  (3,104)=1 -> Slot 7 sce 1 msg 1 "The water is blocking the entrance." @0x01EC6.
* sub04 @0x022E6 (Ereignis 4): `2b 00 ff ff` msg 0 "A lever to fill and drain water. Will you move it? ", `02`,
  @0x022EC Ifel_ck, @0x022F0 `21 0c 1f 00` Ja, Pad-Sperre `22 02 07 01` `22 01 1b 01` `22 02 02 01`, Sleep 1;
  @0x02304 Ifel_ck (3,104)=0: @0x0230C `22 03 68 01` Set(3,104)=1, obj 4 y -1400, Sleep 20, `29 04` Cut 4, BGM,
  @0x02324 `2f 01 ec ff` Speed_set(1,-20), @0x02328 `0d 00 06 00 b4 00` For 180 { Add_speed, Nop, Evt_next, Nop },
  obj 5 y -5000, Slot 7 -> sce 1 msg 1, Sca_id_set 0..4 0x32 ff; Else @0x0235A: Sleep 20, Cut 4, `22 03 68 00`,
  obj 4 `2f 01 14 00` Speed_set(1,+20) For 180, Member_set(1,-16200) obj 4/5, Slot 7 sce 0, Sca 0x32 00;
  @0x023AE Freigabe, `29 07` Cut 7, `3c 01`.
* Wasserobjekt obj 0 (Typ 0, Band 5): (3,109)=0 -> y -16200 @0x01D92 (ausser Sicht), (3,109)=1 -> y -3600
  @0x01F88. Kistenraetsel-Ende sub02 @0x02264 `2e 03 00` + @0x02268 `34 01 00 00` (y 0 = Beckengrund), dann
  @0x0227C `2f 01 ec ff` Speed_set(1,-20) + @0x02280 For 180 (Add_speed, Nop, Evt_next, Nop) = Wasser steigt
  180 Takte um 20 je Takt = 3600 -> y -3600. **= die FORM "Wasser steigt" (Objekt-Fahrt der Wasserflaeche).**
* Schaltpult: main00 @0x01D30 `06 00 3c 00` Ifel_ck + @0x01D34 `21 05 07 01` Ck(5,7)=1 -> @0x01D38 `2c 08 03 31
  03 00 a0 0f fe de 08 07 20 03 ff 00 18 09 00 00` Slot 8 sce 3 Ereignis 9 (Rechteck (4000,-8450,1800,800), Band 3)
  + @0x01D4C Cursor obj 8 Typ 4 (-19554,0,22684) (= der Cursor der Cursor-Raetsel, re15_hebetisch_cursor.h).
  (5,7) wird in 2050 NIRGENDS gesetzt (grep `22 05 07`: 0 Treffer) -> im Auslieferungsstand tot.
  sub09 @0x023F2 `29 0f` Cut 15 (2D-Pult), Slot 8 aus, `22 05 04 01` `22 05 05 01` `22 05 06 01`.
  sub01 @0x02182..@0x0220A: (5,4)=1 & Sce_key_ck 0x01/0x04/0x02/0x08 -> sub05..08 (Cursor obj 8 `2f 02 c8 00` /
  `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` + Add_speed); (5,5)=1 & Sce_key_ck 0x40 -> sub10 (Ausstieg:
  Cursor parken @0x02414, Slot 8 wieder sce 3, (5,4..6)=0, (2,0)/(2,2)=0, Cut_old, Cut_auto 1); (5,6)=1 ->
  Set(2,0)=1 Set(2,2)=1. KEINE Schalterzonen, keine Spannung, kein Ergebnis.
* Vorbild Schalterlogik ROOM2060 (Generator, derselbe Stage): Zonen `2c 03 05 44 ...` Slots 3..6 (sce 5, sat 0x44,
  2050x2050) @0x00F34..@0x00F70; sub01: `21 05 01 01` + `2e 03 03` Cursor + `3e 00 0f 00 03 00` Member_cmp(0x0F ==
  Zone 3) + `51 01 40 00` Sce_key_ck 0x40 (SQUARE) -> Evt_exec sub08 @0x011D6; sub08 @0x014BA: (5,1)=0, Hebel
  obj 5 `2f 03 c0 ff` Speed_set(3,-64) For 16 / Else `2f 03 40 00`, Bit (5,6) kippt, (5,1)=1. Loesung @0x01296..
  @0x012AA `21 03 7e 00` `21 05 06 01` `21 05 07 00` `21 05 08 01` `21 05 09 00` -> Evt_exec sub13 + @0x012AE
  `22 03 7e 01`. Einstieg sub12 @0x015DA Slot 8 aus, `29 07`, (5,0..5,5),(5,12)=1; Ausstieg sub20 @0x016D4.
  Texte: 2050 msg 0..2 vorhanden, Port-Ids 40..44 frei.
* Vorbild Zielwert 80: ROOM11F0-Pult (include/re15_panel_zeiger.h; RE2 ROOM2130 sub04 @0x01752 `23 00 05 00 50 00`
  cmp(var5 == 80), Deckel 100 @0x011C8, Boden 0 @0x01240, Bestaetigung se 0x0C @0x01762).

### 1.5 ROOM2080 — der Rechner (Punkt 6)
* sub00 @0x00C02 `21 03 f5 00` Ck(3,245)=0: Slot 3 sce 1 msg 1 (Tuer 1 gesperrt: "The water level is very high...
  I can't get down to the floor below."), Slot 4 sce 3 Ereignis 2 @0x00C1A (Rechner, Rechteck (-12050,-7250,
  1000,1000), Band 8), Wasser obj 0 @0x00C2E (-9000,-10800,-5400) Band 9, Tuer Slot 1 aus @0x00C50.
  Else @0x00C5E obj 0 bei y -3420 (abgesenkt).
* sub02 @0x00C86 (Ereignis 2): msg 0 "From here maybe I can control the water level.Will you operate the computer? ",
  Ja -> @0x00C94 `22 03 f5 01` Set(3,245)=1, Slot 4/3 aus, Tuer Slot 1 an @0x00CAC, Pad-Sperre, Sleep 20,
  `29 07` Cut 7, @0x00CC4 `2e 03 00` Wasser, @0x00CC8 `2f 01 32 00` Speed_set(1,+50), @0x00CCC `0d 00 06 00 94 00`
  For 148 { Add_speed, Evt_next x2 } (148 x 50 = 7400 -> -10800+7380 = -3420), Sleep 20, Cut 3, Cut_auto 1,
  Freigabe, Plc_ret. **= Anker fuer "Rechner gelaufen" = (3,245) (Original-Flag, Raum-Merkflag).**

### 1.6 Zuteilung Bank 9 (VERTRAG §1.1) — Benennung
| Bit | Name | Bedeutung |
|---|---|---|
| 100 | RE15_R41C_BIT_LEITER_FREI | 2000/20B0: Leiter nach 2050 frei (Nutzer: "nach dem Steigen des Wassers") |
| 101 | RE15_R41C_BIT_VENTIL13 | 2030: Zulaufventil 13 geoeffnet |
| 102 | RE15_R41C_BIT_BRUECKE_UNTEN | 2030: Bruecke gesenkt |
| 103 | RE15_R41C_BIT_FLUT_GELAUFEN | 2030-Ventilraetsel geloest, Wasser gestiegen -> 2000 wird 20B0 |
| 106 | RE15_R41C_BIT_PULT2050 | 2050: Schaltpult auf 80 V |
| 107 | RE15_R41C_BIT_2030_LEER | 2080-Rechner gelaufen -> 2030 wieder ohne Wasser |

## 2. Messung vorher
(folgt nach dem ersten Bau)

## 3. Umsetzung
(folgt)

## 4. Messung nachher
(folgt)

## 5. Tests
(folgt)

## 6. OFFEN
* Zone Slot 8 sce 12 in 2030 @0x01B8E: Bedeutung von sce 12 im Port pruefen (aot_common.c).

## 7. Fuer den Nutzer
(folgt: Port-Texte zur Freigabe, Bedienung, Entscheidungsfragen)
