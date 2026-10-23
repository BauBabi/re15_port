# Runde 41 — Spur C "ventilkette" (STAGE2-Kette 2000 / 2030 / 20B0 / 2050 / 2080)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, Basis master 116e8b0f. Begonnen 2026-10-23 17:12.
Wortlaut des Nutzers: `AUFTRAG.md` ("Vorab vereinbart"); Zuteilung: `VERTRAG.md` (Bank 9 Bits 100-103, 106, 107;
Text-Ids 2000/20B0 40, 2030 40-49, 2050 40-44, 2080 40-41). Vorrunde: `befunde_runde40/UEBERGABE.md` "Befund STAGE2".
Modul: `re15_port/engine/src/ventilkette_r41.c` + `include/re15_ventilkette_r41.h` (alle Konstanten mit Beleg im Kopf),
Daten `engine/src/gen/ventilkette_r41_{texte,original,programme,wasser}.inc` aus
`tools/r41_ventilkette/gen_r41_ventilkette.py` + `programme.py`. Bilder: `C_belege/`.

## 0. Stand / Protokoll (neueste zuerst)

* 22:30 (Fortsetzung) Integrationsgruppe der beruehrten Raeume (16 Haken, seriell) gestartet; Ergebnis §5. Dossier
  §3.3/§4/§5/§6/§7 um die Laeufe der Fortsetzung ergaenzt. Orchestrator-Entscheide C-a..e (AUFTRAG.md 21:37) alle
  angenommen -> keine offene Entscheidungsfrage mehr; die Port-Texte (§7) bleiben Vorschlag bis zur Freigabe.
* 22:24 Ertrink-Blende: gemessen, dass "ABR 0 + Maske Blau" im PC-Renderer (render_pc.c:1282, nur ABR 1/2) als B-F
  gezeichnet wurde -> Blau WEG (Bildmitte F2740 (29,36,29) -> F2760 (30,37,0), gelbgruen). Behoben: ABR 2 (die
  ausgelieferte Form @0x174a) + Maske Rot+Gruen -> (0,0,30) = Blau bleibt (C_belege/flut_blende_vergleich.png).
  integration_r41_ventilkette A+B+C PASS (214 s); Commit 76fdeca4.
* 22:00 Die Flut am Spiel gemessen (exe, RE15_STATE_LOG, C_belege/flut_echt_sheet.png): Spinnen als Spielzustand
  "schon getoetet" ueber ihre Kill-Bits (kein Haken); Warnung zu F820 -> Flut F2621 (= 1800) -> Fuesse F2728 ->
  Spritzer ab F2733 -> Tod F2746 (= +125) -> YOU DIED. integration_r41_ventilkette: A neu getaktet (Debug-Sprung
  blickt auf die Ost-Tuer, nicht auf die Ventile; Ja erst beim 4. Druck, Warnung 1 Druck spaeter), B Leiter-
  Position -26000 (bei -25900 traf der Vorwaertspunkt das Rechteck nicht), C = Flut-Lauf neu.
* 21:50 Fortsetzung nach dem Sitzungslimit (Vorgaenger ~19:25). Bau 1745af8e + e19d755d: LOCAL-BUILD-OK. Gezielte
  Unit-Gruppe (37 Tests): 1 rot = unit_r41_ventilkette D2 (Riegel-Erwartung 324, gemessen 323: der erste der 324
  Schritte laeuft im Marker-Takt) -> Erwartung korrigiert, PASS; Commit 029c5460.
* 18:35 Alle sechs Punkte gebaut und gemessen. unit_r41_ventilkette PASS (227 Pruefungen, Leon + Elza); gezielte
  Unit-Gruppe 18/18. Integrationsgruppe (exe, seriell) laeuft; Ergebnis in §5.
* 17:44 Generator (Texte/Original-Scheiben/Programme/Wasser) + Modul + Haken; erster Bau ab 17:50 (Engine durfte
  re15_render_tile nicht rufen -> Zeichner als Abfrage, main.c zeichnet).
* 17:12 Baum sauber (status leer, HEAD 116e8b0f). AUFTRAG/VERTRAG/UEBERGABE gelesen. SCD-Dumps aller Kettenraeume
  (tools/scd_dump_room.py) + Texttabellen (tools/rdt_msgdump.py) gezogen, Belege unten in §1.

## 1. RE-Belege (Original, selbst aus den Bytes gelesen)

Alle Datei-Offsets beziehen sich auf `re15_port/shared_assets/PSX/STAGE2/ROOMxxxx.RDT` (Leon); die Elza-Varianten
ROOM2001/2031/2051/2081/20B1 sind in den hier zitierten Regionen byte-gleich (der Riegel vergleicht alle Scheiben
in beiden Fassungen).

### 1.1 ROOM2000 — die Leiter nach 2050 (Punkt 1)
* Tuersaetze main00 @0x0161E..@0x016BD (fuenf `3b`): Slot 0 @0x0161E -> ROOM1260 (hinauf), Slot 1 @0x0163E -> ROOM1260
  Cut 8, Slot 2 @0x0165E -> ROOM2010, Slot 3 @0x0167E (Rechteck (-20300,-8200,1000,4000)) -> ROOM2070 Cut 13,
  **Slot 4 @0x0169E = die Leiter nach ROOM2050**:
  `3b 04 02 31 01 00 30 94 48 26 e8 03 e8 03 8c 23 e8 ea 04 10 00 04 01 05 00 03 00 00 00 00 00 00`
  = sce 2, sat 0x31, Band 1, Rechteck x -27600 z 9800 1000x1000, Ziel (9100,-5400,4100) Richtung 1024,
  Stage 1 Raum 5 (= ROOM2050) Cut 0, nfloor 3.
* Die drei Wasserstrahlen an der Leiterwand (aus dem Gitterfenster ueber der Leiter, Bild C_belege/leiter2_vergleich.png
  unten rechts): main00 @0x0174C/@0x0175C/@0x0176C je `3a 00 0b 00 00 00 00 19 xx xx 4c eb c0 2b 00 04` (ESP 0x0b,
  x -23900/-23300/-22700, y -5300, z 11200) — IMMER an (kein Ck davor); je Satz 6 Stroeme (re15_esp.c:1006).
* Kein Skript in 2000 liest (3,235); kein Tuersatz in irgendeinem Raum fuehrt nach ROOM20B0 (Port-Tuertabellen
  gen/tuer_zuordnung.inc + gen/re15_tuer_eigen.inc: 0 Treffer "-> ROOM20B0"). Das Original schaltet 2000 -> 20B0
  also NICHT selbst; 20B0 ist nur per Debug-Menue erreichbar -> der Port lenkt um (§3.4).
* FORM fuer "Tuer wird zum Sperrtext" im selben Stage: ROOM2030 main00 @0x01B0A `06 00 1c 00` Ifel_ck, @0x01B0E
  `21 03 db 00` Ck(3,219)=0 -> @0x01B12 `2c 02 01 31 09 00 ba fa 6c d5 08 07 e8 03 00 00 ff ff 00 00` (Text-Platz
  Slot 2, msg 0 "It's locked from the other side.", GLEICHES Rechteck/Band wie die Tuer) / Else @0x01B2A Tuersatz
  Slot 2. Aot_reset-Form `46 ss 01 31 mm 00 ff ff 00 00` (ROOM2030 sub01 @0x01E18).

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
* ROOM20B0 hat KEINE Texttabelle (msg_sec 0). Port-Id 40 (Leiter-Satz) kommt per re15_msg_install_text.
* Kameras (RDT+0x60, 32 B je Cut, Tabellen 2000/20B0 selbst verglichen): 20B0 Cut 0..4 = 2000 Cut 0..4; 20B0 5/6/7/8/9
  = 2000 6/7/8/9/10; 20B0 10 = 2000 14; 2000 Cut 5/11/12/13/15 ohne Zwilling. Tueren nach 2000 tragen Cut 0
  (1260 @0x114A), 4 (1260 @0x11AA), 10 (2050 @0x1C4A), 13 (2070 @0xA3A/@0xA9A), 14 (2010 @0x746).

### 1.3 ROOM2030 — Bruecke, Lifte, Ventile (Punkte 2/3)
* main00 @0x01ACA Tuer Slot 0 (OST, Ring Band 3): `3b 00 02 31 03 00 c2 1a 62 f8 e8 03 ac 0d c0 12 00 00 64 e7 00 08
  01 07 00 00 ...` = Rechteck (6850,-1950,1000,3500), Ziel (4800,0,-6300) Richtung 2048, Stage 1 Raum 7 = ROOM2070
  Cut 0 (2070 = der Gang zwischen 2000 und 2030: Slots 0/3 @0xA3A/@0xA9A -> 2000, Slots 1/2 @0xA5A/@0xA7A -> 2030).
  Slot 1 @0x01AEA -> ROOM2080 (Band 9), Slot 2 @0x01B2A -> ROOM2020 (nur wenn (3,219)=1), Slot 3 @0x01B4C
  -> ROOM2060 (Band 9).
* **Bruecke = obj 0 @0x01B6C** `2d 00 00 00 00 00 0a 00 00 00 cd fb f8 f8 ed fe 00 00 00 00 ...` = Typ 0, Band 0,
  Lage (-1075,-1800,-275), rot 0; MD1 @0x0020CC 636 B = 18 Dreiecke, x -899..900, y 0, z -5685..5686 (die
  gelb-schwarz markierte Gitterplanke; tools/rdt_objekt_ansicht.py). Von keinem Skript bewegt (kein `2e 03 00`).
  Die Planke ist NICHT im BSS gemalt (Bild C_belege/bruecke_vergleich.png: hochgefahren liegt darunter nur das Becken).
* Lift A obj 1 (6725, -5400 | -10800, 12050) sub00 @0x01CFA/@0x01D48, Lift B obj 2 (-10675, -5400 | -16200, 4050)
  @0x01D9C/@0x01DEA. **FORM der Objekt-Fahrt (derselbe Raum!)**: sub03 @0x01ED2 `2e 03 01` Work_set(3,1),
  @0x01ED6 `2f 01 88 ff` Speed_set(1,-120), @0x01EDA `0d 00 06 00 2d 00` For 45 { `30` Add_speed, `02` `02`
  Evt_next x2 }, @0x01EE6 `34 01 d0 d5` Member_set(1,-10800). Rueckweg @0x01EF2 `2f 01 78 00` Speed_set(1,+120),
  For 45, @0x01F02 `34 01 e8 ea` Member_set(1,-5400). 45 x 120 = 5400 = die Hoehendifferenz. Lift B: For 90
  (@0x01FB4/@0x01FD0), 90 x 120 = 10800. => 120 Einheiten je Add_speed, Add_speed jeden zweiten Takt, Endlage per
  Member_set(1). Speed_set steht IMMER direkt vor dem Add_speed desselben Objekts (Speed ist je Faden, scd_vm.c
  op_speed_set LAB_80040f14 `sh thread+0x158`).
* Pad-Sperre fuer eine Objektfahrt: sub02 @0x01E30 `22 02 07 01` ... @0x01EC4 `22 02 07 00`; Cut_auto 0 @0x01E34,
  Cut_auto 1 @0x01EC2; Kamera der Fahrt `29 0a` @0x01E60 (Lift A oben) / `29 07` @0x01E8E (unten).
* Texte: msg 0 "It's locked from the other side." @0x2014, 1 "The elevator is not responding...Maybe it should be
  activated from somewhere." @0x2038, 2 "Will you take the Fuse? " @0x208B (Ja/Nein-Form `1b 03 02 01 00`), 3 "You've
  taken the Fuse." @0x20AC (`57 01 00`). Port-Ids 40..49 frei (VERTRAG).
* Ebenen (Runde 40, auswahl.json "ebene"): Ring Band 3 y -5400 (Cuts 0/1/2/7/8/15), Mittelsteg Band 1 y -1800
  (Cuts 3/5/6/14), Buero Band 6 y -10800 (10/11/12), Tuerebene Band 9 y -16200 (9/13). PSX-y: kleiner = hoeher.
* SCA (RDT+0x20, 5 Regionen x 76 Zellen, 12 B: w, d, x, z, type, u0, u1, floor): Zelle 12 (-2050,-5950,2000,11400)
  Band 1 u0 **00** = begehbarer Steg (in allen 5 Regionen gleich), Zelle 2 (-3050,-6900,1000,500) Band 1 = der Tank
  am Treppenfuss, Zelle 20 (-5900,-9000,11800,500) Band 3 = Nord-Gelaender, Zelle 24 (-9700,-12350,18950,2000) Band 3
  = Nordwand (Flaeche z -10350), Zellen 18/19 Ost-Wand/-Gelaender (x 7250 / 5400..5900). Sca_id_set-Form (Region,
  Zelle, u0): ROOM2050 sub04 @0x02346 `37 0r 32 ff` (Port op_sca_id_set -> sca_entry_at(region, index)->u0).
* Zone Slot 8 @0x01B8E `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00 02 00 00 00` sce 12 = TREPPE (Port
  RE15_AOT_TYPE_STAIR, p0 1) (-6050,-8400,3700,1400): vom Nordwest-Balkon des Rings hinab zum Steg-Landeplatz.
* Kameras: Cut 2 @0x000A0 (3883,-8101,-7091)->(-11568,-5157,-9778) = schaltschrank.png; Cut 4 @0x000E0
  (-4253,-9340,7013)->(-1841,-4606,2549) = Draufsicht auf Becken + Steg (Projektion des Stegs Bild x 100..210,
  y 73..221, tools/maske/geom.build_view) = Fahrt-Kamera; Cut 15 @0x00240 flag 1 = Schachtmitte oben auf die Ost-Tuer.
* Stelle Pult: Rueckprojektion des roten Markers in schaltschrank.png (Cut 2, Bild (45,128)) auf die Nordwand
  z = -10350 -> Welt (-89,-7079); Pultkanten Bild (30,60)/(20,180) -> x 125..706, y -8506..-6242 (Skript
  scratchpad proj2030.py, Kamera-Konvention gegen die Treppe geprueft: Zone NW (-6050,-5400,-8400) -> Bild (168,136),
  SE (-2350,-5400,-7000) -> (197,169), NW unten -> (168,202) = die Treppe im Bild).
* Stelle Ventil 13: der Tank rechts der Treppe = SCA Zelle 2 (Band 1), Landeplatz x -2550..-50, z -8500..-6900.
* Stelle Ventile (Platzhalter): Ost-Streifen x 5900..7250 an der Ost-Tuer; Cut 15 zeigt dort die Schilder 04/05/06.
* RVD (RDT+0x28, 20 B je Zone, from/to): 1->2 bei x -1500..-500 z -11000..-8500 (Nordstreifen), 2->3 bei x -4000..-3000
  z -9000..-6500 (Treppe), 3->4 bei z -5500..-4500 (Steg), 4->5 bei z 5000..6000.
* Sicherung: (3,108)=0 -> Slot 9 sce 3 Ereignis 6 @0x01BAA (-5900,6650,800,1000) **Band 1 = Suedlandeplatz des
  Stegs**; sub06 @0x01FE4 Frage msg 2, @0x01FF2 `22 03 6c 01`. => Die Bruecke oben sperrt den Weg zur Sicherung.
* Bank 5 in 2030: 0 Treffer `21 05`/`22 05` (frei fuer Marker). Bank-5-Wort 1 (Bits 0x20..0x3F) wird am Ende JEDES
  VM-Laufs geloescht (FUN_8003ebf4 @0x8003ec1c, scd_vm.c:862), Wort 0 beim Raumladen (FUN_8003ecec @0x8003ed74).

### 1.4 ROOM2050 — Hebel, Wasser, Schaltpult (Punkt 5)
* Hebel: sub00 (3,109)=0-Zweig: (3,104)=0 -> @0x01E28 `2c 06 03 31 03 00 5c 12 e2 d2 20 03 20 03 ff 00 18 04 00 00`
  Slot 6 sce 3 **Ereignis 4 = Hebel** (Rechteck (4700,-11550,800,800), Band 3), Slot 7 sce 0.
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
  @0x0227C `2f 01 ec ff` Speed_set(1,-20) + @0x02280 For 180 = Wasser steigt 180 Takte um 20 je Takt = 3600 -> y -3600
  (1800 unter dem Steg -5400). **= die FORM "Wasser steigt" (Objekt-Fahrt der Wasserflaeche).**
* Schaltpult: main00 @0x01D30 `06 00 3c 00` Ifel_ck + @0x01D34 `21 05 07 01` Ck(5,7)=1 -> @0x01D38 `2c 08 03 31
  03 00 a0 0f fe de 08 07 20 03 ff 00 18 09 00 00` Slot 8 sce 3 Ereignis 9 (Rechteck (4000,-8450,1800,800), Band 3)
  + @0x01D4C Cursor obj 8 `2d 08 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 ...` Typ 4 (-19554,0,22684) (= der Cursor
  der Cursor-Raetsel, re15_hebetisch_cursor.h). (5,7) wird in 2050 NIRGENDS gesetzt (grep `22 05 07`: 0) -> tot.
  sub09 @0x023F2 `29 0f` Cut 15 (2D-Pult), `46 08 00..` Slot 8 aus, `22 05 04 01` `22 05 05 01` `22 05 06 01`.
  sub01 @0x02182..@0x021E0: (5,4)=1 & Sce_key_ck 0x01/0x04/0x02/0x08 -> sub05..08 (Cursor obj 8 `2f 02 c8 00` /
  `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` + Add_speed); @0x021E2 (5,5)=1 & Sce_key_ck 0x40 -> sub10 (Ausstieg
  @0x0240C: Cursor parken @0x02414, Slot 8 wieder sce 3 Ereignis 9 @0x0241C, (5,4..6)=0, (2,0)/(2,2)=0, Cut_old,
  Cut_auto 1); @0x021FA (5,6)=1 -> Set(2,0)=1 Set(2,2)=1. KEINE Schalterzonen, keine Spannung, kein Ergebnis.
* Vorbild Schalterlogik ROOM2060 (Generator): Zonen `2c 03 05 44 ...` Slots 3..6 (sce 5, sat 0x44, 2050x2050)
  @0x00F34..@0x00F70; sub01: `21 05 01 01` + `2e 03 03` Cursor + `3e 00 0f 00 03 00` Member_cmp(0x0F == Zone 3) +
  `51 01 40 00` Sce_key_ck 0x40 (= SQUARE, pad_common.c vtbl Bit 6) -> Evt_exec sub08 @0x011D6; sub08 @0x014BA
  (5,1)=0, Hebel obj 5 `2f 03 c0 ff` For 16 / Else `2f 03 40 00`, Bit (5,6) kippt, (5,1)=1. Loesung @0x01296..
  @0x012AA `21 03 7e 00` `21 05 06 01` `21 05 07 00` `21 05 08 01` `21 05 09 00` -> Evt_exec sub13 + @0x012AE
  `22 03 7e 01`. Einstieg sub12 @0x015BA..@0x015FE (Frage msg 0, Ja, Slot 8 aus, `29 07`, (5,0..5),(5,12)=1).
* Vorbild Zielwert 80: ROOM11F0-Pult (include/re15_panel_zeiger.h; RE2 ROOM2130 sub04 @0x01752 `23 00 05 00 50 00`
  cmp(var5 == 80), Ruhe 30 Bilder @0x0171C, Deckel 100 @0x011C8, Bestaetigung se 0x0C @0x01762, Klick se 0x0A
  @0x01192). Texte ROOM11F0 msg 1 @0x18D8 "Will you push them? " (25 B), msg 2 @0x18F1 "Power supply OK." (24 B).
* Cut 15 @0x00240: (-19628,-17442,22616) -> (-19628,15928,22617) (Nadir, wie 11F0 Cut 10). Projektion (geom.build_view):
  Cursor (-19554,-900,22684) -> Bild (160.9,119.1); +200 x -> +2.515 px; +200 z -> -2.515 px (H 208, vz 16542).
  BSS-Bild ROOM20515.ppm (probe_bg_dump): drei grosse Hebel Mitten x 88/133/178, kleiner x 238, Koerper y 85..165;
  Skala Ziffernspalten (dunkle Pixel y 62..67): "0" 165.5, "90" 248.5, "100" 256.5 -> 0.91 px je Punkt; rotes Band
  (r > g+40, r > b+40) y 74 x 239..256 = 80..100; Laufschiene hell y 70..73, Grundlinie dunkel y 75.

### 1.5 ROOM2080 — der Rechner (Punkt 6)
* sub00 @0x00C02 `21 03 f5 00` Ck(3,245)=0: Slot 3 sce 1 msg 1, Slot 4 sce 3 Ereignis 2 @0x00C1A `2c 04 03 31 08 00
  ee d0 ae e3 e8 03 e8 03 ff 00 18 02 00 00` (Rechner, (-12050,-7250,1000,1000), **Band 8**), Wasser obj 0 @0x00C2E
  `2d 00 03 02 09 00 0a 00 1a 00 d8 dc d0 d5 e8 ea 00 00 00 04 ...` (Typ 3, Band 9, (-9000,-10800,-5400), rot_y 1024,
  Flags 0x000a|1, Attribut 0x001a), Tuer Slot 1 aus @0x00C50. Else @0x00C5E obj 0 bei y -3420 (Wiedereintritt).
* sub02 @0x00C86..@0x00CF1 (108 B, Ereignis 2): msg 0, Ja -> @0x00C94 `22 03 f5 01` Set(3,245)=1, Slot 4/3 aus,
  Tuer Slot 1 an @0x00CAC, Pad-Sperre, Sleep 20, `29 07` Cut 7, @0x00CC4 `2e 03 00` Wasser, @0x00CC8 `2f 01 32 00`
  Speed_set(1,+50), @0x00CCC `0d 00 06 00 94 00` For 148 { Add_speed, Evt_next x2 } (148 x 50 = 7400 -> y -3400),
  Sleep 20, Cut 3, Cut_auto 1, Freigabe, Plc_ret @0x00CEC, Endif @0x00CEE, Evt_end @0x00CF0. Ifel-Laenge @0x00C8E 0x60.
* Wasserflaeche obj 0: MD1 @0x000D98 4572 B (108 Vierecke, x 0..21600, y 0, z 0..16200, Ursprung in der Ecke),
  TIM @0x026408 16928 B.

### 1.6 Zuteilung Bank 9 (VERTRAG §1.1) — Benennung (k_port[] in test_r36_kartenbits.c eingetragen)
| Bit | Name | Bedeutung |
|---|---|---|
| 100 | RE15_R41C_BIT_LEITER_FREI | 2000/20B0: Leiter nach 2050 frei (Nutzer: "nach dem Steigen des Wassers") |
| 101 | RE15_R41C_BIT_VENTIL13 | 2030: Zulaufventil 13 geoeffnet |
| 102 | RE15_R41C_BIT_BRUECKE_UNTEN | 2030: Bruecke gesenkt |
| 103 | RE15_R41C_BIT_FLUT_GELAUFEN | 2030-Ventile geoeffnet und Raum verlassen -> 2000 wird 20B0 |
| 106 | RE15_R41C_BIT_PULT2050 | 2050: Schaltpult auf 80 V |
| 107 | RE15_R41C_BIT_2030_LEER | 2080-Rechner gelaufen -> 2030 wieder ohne Wasser |

## 2. Messung vorher (gebauter Stand master 116e8b0f, exe-Laeufe + Dumps)
* ROOM2030 per Debug-Sprung (Ost-Tuer, Cut 0): die Bruecke obj 0 steht bei y -1800 (Mittelsteg), begehbar; drei
  Spinnen greifen binnen Sekunden an (main00 Switch auf den Eintritts-Cut, Case 0 @0x01C12). Kein Pult-Platz, kein
  Ventil-Platz (AOT-Slots 12..14 frei), Cut 14/15 nie geschaltet (Befund Runde 40 bestaetigt).
* ROOM2000: die Leiter Slot 4 ist eine Tuer nach 2050; die drei Strahlen laufen immer (C_belege/leiter2_vergleich.png
  unten rechts zeigt die Wand mit Strahlen; vorher ohne Text).
* ROOM20B0 nur per Debug-Sprung erreichbar; (3,235)=1 zeigt die Strahlen (18 Effektplaetze, Riegel B).
* ROOM2050: Hebel sub04 immer bedienbar ((3,104) kippt); Pult-Zone Slot 8 nie angelegt ((5,7)=0), Cursor obj 8 nicht
  im Pool; Cut 15 nie geschaltet.
* ROOM2080: Rechner senkt nur das eigene Wasser; (3,245) ist ein Raum-Merkflag.
Bilder der BSS-Cuts (probe_bg_dump): scratchpad r41c/bg/sheet_203.png, sheet_205.png, sheet_200.png, sheet_20B.png.

## 3. Umsetzung (Dateien, Konstanten — jede mit @0x…/Byte-Offset im Kopf von re15_ventilkette_r41.h)

### 3.1 Punkt 1 — Leiter 2000 (`install_2000`)
Slot 4 (@0x0169E) wird bei Bit 100 = 0 zum Sperrtext 40 (re15_aot_retype sce 1, sat 0x31, Maske ffff; Slot-Band aus
door_params wie adaruf_1050.c:231, Form ROOM2030 @0x01B12). Text 40 = NUTZER-VORGABE woertlich
"I cannot climb up the ladder because of the water pressure." (gen/ventilkette_r41_texte.inc, Glyphen msg_common.c).
Mit Bit 100 bleibt Slot 4 die Tuer. Dasselbe in 20B0/20B1 (dort nur bei Debug-Sprung ohne Bit 100).

### 3.2 Punkt 2 — Bruecke oben, Pult, Ventil 13 (`install_2030`, Programme 2030_pult_*, 2030_v13_*)
* Bruecke: solange Bit 102 = 0 steht obj 0 bei y -5400 (PORT-WAHL: die Ringebene = die naechste Ebene ueber Band 1,
  Lift-Endlage @0x01F02 `34 01 e8 ea`), und die Steg-Zelle 12 ist in allen 5 Regionen u0 := ff gesperrt (Form Sca_id_set
  `37 0r 0c ff`, ROOM2050 @0x02346). Die Sicherung (Suedlandeplatz, Band 1) ist damit erst nach dem Senken erreichbar.
* Plaetze (Ereignis-Saetze in der Form ROOM2050 @0x01D38 `2c ss 03 31 bb 00 … ff 00 18 ee 00 00`, Port-Ereignisse
  24/25/26, Slots 12/13/14): Pult (-300,-10850,1300,1000) Band 3 (Nordwand, §1.3), Ventil 13 (-3050,-6900,1300,1000)
  Band 1 (Tank = Zelle 2, vom Landeplatz nach Westen), Ventile (4900,-2000,1400,3500) Band 3 (Ost-Streifen, Schilder
  04/05/06). Vorwaertspunkt 620 (FUN_80042bac) in den Riegeln mit den gemessenen Spielerlagen belegt.
* Programme (gen/ventilkette_r41_programme.inc, jede Zeile mit Vorbild): Pult ohne Bit 101 -> msg 40 ("gauge reads
  zero … Intake valve 13 is closed."); Ventil 13: Frage msg 41 -> Ja -> Set(9,101) + msg 42, danach Zustand msg 47
  (Klick = RE2-Panel-se 0x0A beim Kippen, Flanke von Bit 101 im Takt); Pult mit Bit 101: Frage msg 43 "… Lower the
  bridge?" (NUTZER-VORGABE) -> Ja -> `22 02 07 01` `3c 00` `29 04` (Cut 4 Draufsicht, PORT-WAHL als Fahrt-Kamera wie
  `29 0a` @0x01E60) `2e 03 00` `2f 01 78 00` For 30 { `30 02 02 00` } `34 01 f8 f8` (30 x 120 = 3600 in der Lift-Form
  sub03) + Sca_id_set Zelle 12 := 00 x5 + Set(9,102) + Sleep 20 + Cut_old + Cut_auto 1 + `22 02 07 00`; danach msg 44.
  Nein-Zweig = nichts (Ck(12,31) wie sub04 @0x022F0).

### 3.3 Punkt 3 — Ventilraetsel-Platzhalter, 60 s, Flut, Ertrinken (Programm 2030_ventile, C-Takt `flut_tick`)
* Platz an den Ventilen: Frage msg 45 (zwei Seiten, PORT-WAHL-Wortlaut) -> Ja -> Warnung msg 46 ("WARNING: The water
  will rise in one minute. Leave the room!", NUTZER-VORGABE sinngemaess) -> `22 05 1d 01` Marker -> **Sleep 1800**
  (`09 0a 08 07` = 60 s bei 30 SCD-Takten; Hintergrundfaden wie ROOM2000 sub07 @0x01B36, Spieler frei) -> Pad-Sperre
  `22 02 07 01` + `22 02 02 01` + `3c 00` + `29 04` -> zwei Wasserflaechen (obj 4/5 = ROOM2080 Prop 0, §1.5) per
  Pos_set auf den Beckengrund y 0 (x -10800, z -12400 / 3800; decken x -10800..10800, z -12400..20000) -> `22 05 1e 01`
  -> For 324 { Work_set(3,4) Speed_set(1,-50) Add_speed, Work_set(3,5) Speed_set(1,-50) Add_speed, Evt_next } (Rate
  des Rechners @0x00CC8, 324 x 50 = 16200 = bis zur Tuerebene) -> Sleep 60.
* C-Takt (game_step_common.c-Haken): Marker (5,0x1D) -> Zaehler-Latch je Raumstart, Ost-Tuer Slot 0 -> ROOM20B0
  Cut 11 (Ziel der 20B0-Szene @0x01F00/@0x01F08; Gegenweg der 20B0-Tuer @0x019C2), Wasserflaechen angelegt und bei
  z 60000 geparkt. Marker (5,0x1E) -> Flut: steht die Flaeche 900 ueber den Fuessen (RE15_R41C_ERTRINK_TIEFE, halbe
  Koerperhoehe = Typ-4-Anhebung -900 @0x8002c24c), dann hp := -1 (= RE15_KILL_AT main.c, Todeskette game_step_common.c
  :1413) + blaue Blende Kanal 0 (Form ROOM11F0 @0x174a `56 00 02 07 00 00` + `57 00 <level>`: ABR 2 = subtraktiv, der
  einzige Blendmodus der ausgelieferten Skripte (Zensus aller RDTs: nur 11F0/11F1/3080, immer `56 00 02 07`, Pulse
  `57 00 00 08`); hier Maske 0x6 = Rot+Gruen (FUN_800217b0 Config-Bits 2+1, fade_common.c), d. h. die Kachel zieht
  Rot und Gruen ab und Blau bleibt stehen = blauer Schleier; Level 0x5000 = Helligkeit 160 (level >> 7 @0x800218d0),
  PORT-WAHL. Erste Fassung "ABR 0 + Maske Blau" war ein Defekt, s. §4 "Blende")
  + Spritzer am Spieler alle 6 Takte (Wasser-Schritt-Effekt FUN_80045630 Code 0x06050D00, re15_esp_fx_spawn_rows).
  Tod loescht den Zaehler (keine Flut-Bits beim CONTINUE); der Raumstart loescht Marker, Blende, Zustand.
* Verlassen waehrend des Zaehlers (jede Tuer): der Installer des naechsten Raums setzt Bits 103 + 100 (Nutzer:
  "Verlaesst man den Raum fuehrt das zu ROOM 20B0"); ueber die Ost-Tuer landet man in 20B0 und dessen Original-Szene
  sub02/sub05 laeuft (Cut 11, Auswasch, Set(3,235)).

### 3.4 Punkt 4 — 20B0 statt 2000, Leiter frei (`tueren_umlenken`, `espr_unterdrueckt`)
Bei Bit 103 bekommt jede Tuer mit Ziel Stage 1 Raum 0 (= ROOM2000; 1260 Slots 1/4, 2010 Slot 0, 2050 Slot 0, 2070
Slots 0/3) dest_room 0x0B (ROOM20B0, Szenario-Nibble bleibt, aot_common.c dest_id) und den Zwillings-Cut
(re15_ventilkette_r41_cut_20b0: 0->0, 4->4, 10->9, 14->10, 13->7; Tabellen §1.2); 2030 Slot 0 -> 20B0 Cut 11 dauerhaft.
In 20B0/20B1 ueberspringt op_sce_espr_on bei Bit 100 genau die drei Sce_espr_on @0x01E96/@0x01EA6/@0x01EB6 (PC-Offset
gegen den RDT-Puffer; Haken scd_vm.c) -> "kein Wasser mehr runter laufen" (Bild C_belege/leiter2_vergleich.png).

### 3.5 Punkt 5 — Schaltpult 2050 sperrt den Hebel (`install_2050`, `panel_tick`, Programme 2050_*)
* Hebel (Ereignis 4) bei Bit 106 = 0 -> Sperrsatz msg 40 (Form ROOM2060 sub12 @0x015D0); mit Bit 106 das Original sub04.
* Pult: Slot 8 + Cursor obj 8 werden aus den Original-Saetzen @0x01D38/@0x01D4C angelegt (ohne (5,7)); Ereignis 9 ->
  Beschreibung msg 43 + "Will you push them? " (ROOM11F0 msg 1 Bytes) -> Ja -> sub09-Bytes `29 0f` `46 08 00…`
  `22 05 04 01` `22 05 06 01` OHNE `22 05 05 01` (sonst beendet SQUARE das Pult, sub01 @0x021E2). Die Original-Subs
  sub05..08 bewegen den Cursor (Riegel: RIGHT = sub07 @0x023DA).
* C-Takt bei (5,4)=1: Cursorlage gegen vier Hebelzonen (2400 x 4400 um die Hebelmitten, aus den Bildpixeln §1.4
  rueckgerechnet: x -26550/-22972/-19393/-14622, z 19800..24200); SQUARE-Flanke (virtuell 0x40) auf einer Zone kippt
  den Hebel + Klick se 0x0A; Gewichte **30/20/40 (gross) + 10 (klein) = PORT-WAHL**, einzige Loesung 30+40+10 = 80
  (Riegel zaehlt alle 16 Masken); 30 Ruhebilder (RE2 @0x0171C) -> Bit 106 + se 0x0C + "Power supply OK." (11F0 msg 2)
  -> Ausstieg = Original sub10 (Cursor parken, Slot 8 wieder Ereignis 9, Cut_old). CROSS (0x8000) = Abbruch (sub10).
  Danach ist Slot 8 ein Zustandssatz msg 44.
* Zeichner (main.c, neben dem 11F0-Zeiger): roter Laeufer x = 165.5 + 0.91 * Wert auf der Schiene y 70..73 (RE2
  ROOM2130: Zeiger unter der waagerechten Skala; Rot wie der 11F0-Zeiger) + gruene Marke 8x3 unter jedem EIN-Hebel
  (PORT-WAHL, Farbe der 11F0-Lampe). Bild C_belege/pult2050_sheet.png: F560 Hebel 3 EIN -> Zeiger auf 40, F620 Hebel 1
  dazu -> 70.

### 3.6 Punkt 6 — Rechner 2080 entleert 2030 (`install_2080`, Programme 2080_rechner_*)
Bei Bit 103 und nicht 107: Ereignis 2 = Original sub02 Byte fuer Byte (@0x00C86..@0x00CED) + im Ja-Zweig `22 09 6b 01`
Set(9,107) + msg 40 "The treatment pool is being drained as well." (Ifel-Laenge 0x60 -> 0x6A). War der Rechner
schon gelaufen ((3,245)=1), legt der Installer Slot 4 (@0x00C1A) erneut an; dann nur Frage msg 0 + Slot aus + Set +
Satz (kein zweites Senken). In 2030 gilt: Bit 103 ohne 107 = zwei Wasserflaechen bei y -7200 (1800 ueber dem Ring,
Abstand wie ROOM2050 Wasser -3600 zum Steg -5400 @0x01F88) und die oberen Lift-Rufzonen (Slots 4/6, sub00 @0x01D20/
@0x01DC2) = Original-Satz msg 1 "The elevator is not responding…" (Form sub01 @0x01E18); mit Bit 107 alles Original.

### 3.7 Gemeinsame Dateien (Haken, je 1-6 Zeilen, "Runde 41 Spur C")
scd_room_setup.c (Installer-Zeile am Blockende + include), scd_vm.c (Ereignis-Kette hinter nahaufnahmen_r40 +
op_sce_espr_on-Sprung + include), game_step_common.c (Takt hinter johnruf_r39), platform/pc/main.c (Boot-/CONTINUE-
Installer, Prop-Lader fuer obj 4/5, Zeichner-Block, include), tests/unit/test_r36_kartenbits.c (6 Zeilen k_port[]).
Neue Dateien: engine/src/ventilkette_r41.c, include/re15_ventilkette_r41.h, engine/src/gen/ventilkette_r41_*.inc,
tools/r41_ventilkette/{gen_r41_ventilkette.py,programme.py}, tests/unit/test_r41_ventilkette.c,
tests/unit/probes/r41_ventilkette.cmake, tests/integration/test_r41_ventilkette.cmake. Keine Aenderung an
shared_assets/PSX, pri/, synchro/, release/.

## 4. Messung nachher (exe, RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP; Logs im scratchpad r41c/mess/*)
* Bruecke (C_belege/bruecke_vergleich.png, Cut 4 erzwungen, F300): frisch = Planke bei y -5400 ueber dem Becken (log
  "[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt"); mit RE15_SET_FLAG=9:101,9:102 = Planke
  in der Original-Lage -1800 (im BSS ist dort nur das Becken gemalt -> die Fahrt ist sichtbar).
* Flut-Zustand (C_belege/flut_vergleich.png, Bits 101/102/103/100): Cut 4 = Wasserflaeche 2140 unter der Kamera
  (Differenz zum BSS 35710 px), Cut 13 (Tuerebene) = Flaeche 9000 tiefer (Differenz 1406 px unten rechts).
* Leiter (C_belege/leiter2_vergleich.png, Cut 9/10 erzwungen, RE15_PLAYER_POS -24500,10300): 20B0 (3,235)=1 ohne
  Bit 100 = Strahlen aus dem Gitterfenster (Original), mit Bit 100 = keine Strahlen (Differenz 3083 px, nur die
  Strahlen); 2000 Aktion an der Leiter = Text 40 im Bild (F340, Schreibmaschine "I cannot climb up the la").
* Pult 2050 (C_belege/pult2050_sheet.png, Eingabeskript A/W/R/L): F300 Aktion -> msg 43, F423 -> msg 41 "Will you push
  them?", F501 Ja -> Cut 15 + Cursor; RIGHT 7 Bilder -> Hebel 3, SQUARE F550 -> log "Hebel 3 -> EIN, Schalter 0x4,
  40 V" (Zeiger auf 40, gruene Marke); LEFT 36 Bilder -> Hebel 1, SQUARE F616 -> "Hebel 1 -> EIN, Schalter 0x5, 70 V".
  Lehre fuer die Eingabeskripte: ein Druck waehrend der Schreibmaschine spult nur vor; je Text ~3-4 s warten.
* Zaehler/Flut/Tod (Unit-Riegel, Takt fuer Takt, Spieler auf dem Ring y -5400): Warnung zu -> Marker -> Zaehler
  (Ost-Tuer Ziel 0x0B Cut 11, Wasser geparkt) -> Flut 1800 Takte nach dem Schliessen der Warnung (gemessen 1504 +
  600 Takte nach der Aktion, davon ~300 Takte Texte) -> Wasser 50/Takt -> Tod bei Wasser y -6300 (= -5400 - 900),
  125 Takte nach Flutbeginn -> Blende Kanal 0 (level 0x5000, ABR 2, Maske Rot+Gruen) -> Wasser endet bei -16200.
  D2 (Tuerebene y -16200): Tod mit der Endlage -16200 im Takt flut_bei + 323 (der erste der 324 Schritte laeuft im
  Marker-Takt: der For-Koerper laeuft bis zum ersten Evt_next, derselbe Takt sieht s_flut = 1).
* **Flut am Spiel** (Fortsetzung 22:00, exe-Kopie re15_pc_r41c_mess.exe, RE15_STATE_LOG, Bildnummern = Spielbilder ab
  dem Sprung; Reihe C_belege/flut_echt_sheet.png = F2600/2620/2640/2680/2720/2740/2760/2780/2820/2880/2940/2960):
  - Messmittel (kein Spielverhalten): die drei Spinnen des Eintritts-Cuts als Spielzustand "schon getoetet" ueber ihre
    Kill-Bits `RE15_SET_FLAG=7:155,7:156,7:157` = Sce_em_set pc[7] 0x9b/0x9c/0x9d (main00 @0x01C18/@0x01C2C/@0x01C40),
    EM-Status-Gate FUN_800420a0 @0x80042120-38 (`persist != 0xFF && flag(zone, persist)` -> kein Spawn), Zone 7 =
    STAGE2 (re15_em_status_zone, enemy_ai_common.c:14208); Zustandslog: 0 Gegner-Eintraege in 2030. Spieler per
    RE15_PLAYER_POS (6550,-100) Blick +z (3072; der Vorwaertspunkt 620 trifft keinen Platz), der Ventil-Platz per
    RE15_FIRE_AOT=14@330 (= derselbe Pfad wie die Aktionstaste, re15_aot_fire_slot -> scd_event_fire 26), Druecke alle
    4 s ab F450 (ein Druck nach dem Schliessen der Warnung trifft so keinen Platz mehr).
  - Vorbefund (21:52): der Debug-Sprung stellt den Spieler mit Blick +x = auf die Ost-Tuer; die Aktionstaste bei F330
    feuerte Tuer Slot 0 nach ROOM2070 (`[aot] DOOR FIRE slot=0 rect=(7350,-200,...)`), dort toeteten zwei Spinnen den
    stehenden Spieler (hp 100 -> -20 bei F605). Darum tragen integration A und der Flut-Lauf jetzt RE15_PLAYER_POS
    mit Blickrichtung.
  - Ablauf: F330 Ereignis 26 -> Frage 45 oeffnet F331 (Pause-Maske FFFF0007); Druecke F450/F573 spulen und blaettern,
    F696 = Ja -> 45 zu (F697), Warnung 46 auf (F699); F819 schliesst -> Marker (5,0x1D) -> `[ventilkette41] Zaehler
    laeuft` + Ost-Tuer -> 20B0 Cut 11 (F820) -> Sleep 1800 -> **F2621 Flut** (`FLUT: Wasser steigt`, Cut 0 -> 4,
    Pause-Maske 21000007 = Pad-Sperre) = 1800 Bilder nach dem Schliessen -> Wasser 50 je Takt ab y 0: Flaeche sichtbar
    unter der Bruecke (F2640 y -1000, F2680 -3000), Bruecke taucht unter (F2720 -5000), Fuesse (-5400) bei F2728,
    Spritzer alle 6 Takte ab F2733 (Zustandslog fx 1/2/3 bei F2733/2739/2745), **F2746 Tod** (`ERTRUNKEN: Wasser y
    -6300, Spieler y -5400 -> hp -1`, = 125 Bilder nach dem Flutbeginn, 126 Schritte x 50 = 6300 mit dem ersten im
    Marker-Takt), Blende (`config ch=0 abr=2 mask=6`, `kick ch=0 value=0x5000`), Todeskette (weisser Blitz F2820,
    pst 7 bei F2859, YOU DIED F2880..F2960), Ende F2960#2030. Ein spaeteres Ende (3500 ohne Raumfilter) lief in den
    Neustart nach dem Tod, dessen Bildzaehler die Framedumps 2400..3400 mit ROOM1170-Bildern ueberschrieb (erster
    Lauf, verworfen).
  - Blende (22:05): Bildmitte 100x60 F2740 (29,36,29) -> F2760 mit "ABR 0 + Maske Blau": (30,37,0) = Blau WEG
    (render_pc.c:1282 kennt nur ABR 1 additiv / 2 subtraktiv, 0/3 fallen auf subtraktiv zurueck -> "minus Blau");
    mit ABR 2 + Maske Rot+Gruen: (0,0,30) = Blau bleibt (C_belege/flut_blende_vergleich.png). Die Form ist auf der PSX
    dieselbe (GPU B-F je Kanal, Kachel-RGB = Helligkeit & Maske @0x800218d0).
* Echter Weg 2030 -> Ost-Tuer -> 20B0: integration_r41_ventilkette Lauf A (§5).

## 5. Tests
* `unit_r41_ventilkette` (tests/unit/test_r41_ventilkette.c, registriert in probes/r41_ventilkette.cmake): A Scheiben
  Byte fuer Byte gegen 9 RDTs (Leon+Elza) + 2080-Programm = sub02 + Schwanz + Texte dekodiert + Gewichte (1 von 16
  Masken) + Cut-Zuordnung; B Leiter (Text, Aktion, Bit 100 = Tuer, 20B0 ohne Bit 100 = 18 Strahlen-Plaetze); C Bruecke
  oben/Zelle 12 zu, Plaetze 12/13/14, Pult msg 40, Ventil 13 Nein/Ja/Zustand + Bit 101, Pult Nein/Ja (Fahrt unter
  Cut 4, y -1800, Bit 102, Zelle offen, Rueckkehr, 60..200 Takte), Neustart mit 102; D Ventile Nein/Ja, Zaehler,
  Ost-Tuer, Wasser geparkt, Flut nach 1800 Takten, 50/Takt, Pad-Sperre, Cut 4, Tod bei 900 ueber den Fuessen, Blende,
  Wasser bis -16200, Neustart ohne Blende; E Verlassen -> Bits 103+100, 20B0 Leiter = Tuer + 0 Strahlen, Tueren
  2010/2050/2070/1260 -> 20B0 mit Cut, 2030 unter Wasser + Rufzone msg 1 + Zustandssatz, mit 107 leer; F 2050 Hebel
  ohne Strom, Pult Nein/Ja, Cursorfahrt sub07, Zonen, 30+40+10 = 80, Zeiger x(80)=238/x(0)=166, Marken, Bit 106 nach
  <= 32 Ruhebildern, Ausstieg sub10, Cursor geparkt, Hebel = Original (3,104)=1, Zustandssatz, CROSS = Abbruch;
  G 2080 Original / mit Flut (msg 0 + 40, Bit 107, Wasser -3400) / Rechner schon gelaufen / mit 107.
  D2 Tuerebene (Tod mit der Endlage -16200, flut_bei + 323). **Ergebnis: PASS (233 Pruefungen), Leon + Elza**
  (Fortsetzung: Blenden-Riegel = abr 2, Masken r/g 0xff, b 0, Helligkeit 160).
* `integration_r41_ventilkette` (tests/integration/test_r41_ventilkette.cmake): A echter Weg 2030 Ventile (Aktionstaste,
  Blick 2048) -> Frage -> Ja (4. Druck) -> Warnung -> zu (F777) -> Zaehler -> Ost-Tuer feuert (RE15_FIRE_AOT=0@900) ->
  ROOM20B0 geladen -> Bits 103 + 100, kein 2070; B 2000 Leiter (-26000) = Sperrtext `[msgtext] room=2000 id=40`, kein
  Raumwechsel nach 2050; C Flut am Spiel (Kill-Bits, FIRE_AOT 14, Zustandslog): Zaehler 1800 +- 3 Bilder, Flut unter
  Cut 4, Tod 125 +- 2 Bilder danach bei Wasser -6300 / Spieler -5400, Blende kick 0x5000, Framedump F2750, Ende
  F2960#2030. **`ctest --timeout 900 -R "^integration_r41_ventilkette$"` -> `100% tests passed, 0 tests failed out
  of 1` (213.97 s)** — davor zwei rote Laeufe: A/B mit den alten Takten (Tuer statt Ventile; Leiter nicht getroffen).
* Gezielte Gruppe (ctest, Regex woertlich):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie)$"`
  -> `100% tests passed, 0 tests failed out of 18` (18:35, vor dem Riegel D2).
  Fortsetzung, erweitert um die Gruppen der beruehrten Dateien (scd_room_setup.c, scd_vm.c, main.c, kartenbits):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen)$"`
  -> 21:50 `97% tests passed, 1 tests failed out of 37` (unit_r41_ventilkette D2, Erwartung 324 statt 323) ->
  nach dem Riegel-Fix und dem Blenden-Fix: UNIT_GRUPPE_ENDE.
* Integrationsgruppe der beruehrten Raeume (seriell, exe): Regex
  `^(integration_r41_ventilkette|integration_r37_fahrstuhl_n1|integration_r37_itemband|integration_r37_itemband_lift|integration_r37_kiste|integration_r37_kiste2feld|integration_r37_kistenregel|integration_r36_gegnerzahl|integration_r36_dachkiste|integration_r38_spinnen_echtlauf|integration_r38_wasser|integration_r38_wasserzombie|integration_map_uebergang|integration_cross_stage|integration_item_name_census|integration_save_counter_pin)$`
  -> 19:03 (Vorgaenger, alte exe, abgebrochen vom Sitzungslimit nach 4 von 16): dachkiste rot (echt2050 Leiter — der
  Querbezug unten), gegnerzahl / fahrstuhl_n1 / itemband gruen (103 / 155 / 829 s).
  Fortsetzung (exe nach den Fixes, `--timeout 1800`, weil dachkiste allein ~1100 s braucht): INTEG_GRUPPE_ENDE.

* Querbezuege (Memory "neue Regeln widerlegen alte Riegel", beim ersten Integrationslauf gefunden):
  - `integration_r36_dachkiste` echt2050/elza2051 fahren 2050 -> 2000 -> Leiter slot 4 @0x0169E -> 2050; die Leiter ist
    jetzt bis zur Flut gesperrt -> die zwei Laeufe setzen `RE15_SET_FLAG=9:100` (Leiter frei; Gegenstand des Riegels
    ist der Kisten-Record, nicht die Leiter). Lauf 1 der Gruppe fiel genau an "DOOR FIRE slot=4 rect=(-27100,10300"
    durch (1115 s unter Last).
  - `integration_r37_itemband` schrot2030 geht ueber den Steg (GEHE -1050 -7700 -> -1050 7100 = Zelle 12) zu den
    Schrotpatronen; der Steg ist bis zum Pult-Raetsel gesperrt -> Lauf setzt `RE15_SET_FLAG=9:101,9:102`.
  Beide Aenderungen sind Kommentar-belegt in den cmake-Dateien.

## 6. OFFEN
* (erledigt 22:00) Die Flut als exe-Messung: gemessen mit den Spinnen-Kill-Bits als Spielzustand (§4 "Flut am Spiel",
  Riegel integration C). Offen bleibt nur die ORIGINAL-Seite: RE1.5/RE2 kennen kein Ertrinken, Form = PORT-WAHL.
* Der Level 0x5000 (Helligkeit 160) der Ertrink-Blende ist PORT-WAHL: die ausgelieferten Skripte nutzen die Blende nur
  als Lampenflackern (11F0/11F1/3080, `57 00 00 08` = Helligkeit 16); ein ausgelieferter Tint-Wert existiert nicht.
* Verlaesst der Spieler 2030 waehrend des Zaehlers NICHT ueber die Ost-Tuer (sondern per Lift nach 2080/2060/2020),
  gilt die Flut ebenfalls als gelaufen (Bits 103 + 100); die 20B0-Szene laeuft dann beim naechsten Eintritt in 2000
  (= 20B0), gleich von welcher Tuer (die Original-Szene sub02 setzt den Spieler an die Ost-Tuer). PORT-WAHL, nicht
  vom Nutzer entschieden.
* 2070 (der Gang) bleibt nach der Flut nur von 2030/2040 aus erreichbar; von 20B0 gibt es (wie im Original-20B0)
  keine Tuer dorthin. 2070 -> 2000 landet in 20B0 am Westtor (die Szene erwartet den Ost-Eintritt).
* Die Wasserflaeche in 2030 ist das 2080-Modell (Ecke als Ursprung, zwei Flaechen); unter Cut 4 (nur ueber den Steg
  erreichbar, der unter Wasser steht) zeigt sich ein Rand der Flaeche (C_belege/flut_vergleich.png links).
* Die Hebel des 2050-Pults sind gemalt (keine Modelle wie in 2060/11F0): Zustand = gruene Marke + Zeiger.
* RE2-Ton fuer ein Ventilrad gibt es in RE2_SOUND_LISTE.md nicht (gesucht: valve/wheel/pipe/turn): Ventil 13 nutzt den
  RE2-Panel-Klick 0x0A wie lift2030_r37/hebetisch.

## 7. Fuer den Nutzer
* Keine neuen Assets im Paket-Gate (alles eingebacken in gen/*.inc aus den vorhandenen RDTs); keine Sprachdateien.
* Bedienung: 2030 Ring-Nordwand (Cut 1/2) = Pult; Treppe hinab, am Tank rechts (Zelle 2) = Ventil 13; Ring Ost-Streifen
  an der Tuer (Schilder 04/05/06) = die Ventile (Platzhalter des spaeteren Raetsels). 2050: Pult an der Saeule
  (Slot 8), Cursor mit dem Steuerkreuz, SQUARE kippt einen Hebel, CROSS verlaesst das Pult; Loesung 30 + 40 + 10 V.
* Port-Texte zur Freigabe (englisch wie das Spiel; NUTZER-VORGABE = Wortlaut aus dem Auftrag):
  2000/20B0 40 "I cannot climb up the ladder / because of the water pressure." (VORGABE)
  2030 40 "A control panel for the bridge. / The pressure gauge reads zero. | Intake valve 13 is closed."
  2030 41 "Intake valve 13. / Will you open it?" (Ja/Nein) · 42 "You've opened valve 13. / The pressure is rising."
  2030 43 "The pressure gauge reads 80. / Lower the bridge?" (VORGABE "Lower the bridge?") · 44 "The bridge is down."
  2030 45 "Numbered valves. They control the / flood gates of the treatment pool. | Will you turn them?"
  2030 46 "WARNING: The water will rise / in one minute. Leave the room!" (VORGABE sinngemaess)
  2030 47 "Intake valve 13 is open." · 48 "The valves are open. / The pool has been flooded."
  2050 40 "The pump has no power. / The switchboard must be set first." · 41 = 11F0 "Will you push them? " ·
  42 = 11F0 "Power supply OK." · 43 "A high voltage switchboard. / The pump needs 80 volts." ·
  44 "The switchboard is set to 80 volts. / The pump is powered."
  2080 40 "The treatment pool is / being drained as well."
* Entscheidungsfragen: (a) Hebelgewichte 30/20/40/10 (Loesung Hebel 1 + 3 + klein) — anderes Muster gewuenscht?
  (b) Bruecke "oben" = Ringebene y -5400 — hoeher gewuenscht? (c) Flut auch beim Verlassen ueber die oberen Tueren
  (jetzt: ja, siehe §6). (d) Lift-Rufzonen oben nutzen nach der Flut den Original-Satz 1 ("not responding") —
  eigener Satz ("The elevator is flooded.") gewuenscht? (e) Ertrink-Effekt = Tod + blaue Blende (subtraktiv Rot+Gruen,
  Blau bleibt) + Spritzer; ein eigener Ton fehlt (RE2-Liste ohne Ertrinken).
  -> Orchestrator-Entscheide 21:37 (AUFTRAG.md): a-e alle angenommen; die Texte bleiben Vorschlag bis zur Freigabe.
* Sichtprobe fuer den Nutzer: 2030 am Ring vor den Ventilen die Aktionstaste, Ja, Warnung wegdruecken, 60 s stehen
  bleiben -> Draufsicht, Wasser steigt ueber die Bruecke und den Ring, Spritzer, Tod mit blauem Schleier
  (C_belege/flut_echt_sheet.png). Mit den drei Spinnen im Raum (ohne Kill-Bits) stirbt man vorher an den Spinnen;
  im Spiel also erst die Spinnen erledigen oder den Raum durch die Ost-Tuer verlassen (-> 20B0).
