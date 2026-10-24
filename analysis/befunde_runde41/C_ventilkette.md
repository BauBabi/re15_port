# Runde 41 — Spur C "ventilkette" (STAGE2-Kette 2000 / 2030 / 20B0 / 2050 / 2080)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, Basis master 116e8b0f. Begonnen 2026-10-23 17:12.
Wortlaut des Nutzers: `AUFTRAG.md` ("Vorab vereinbart"); Zuteilung: `VERTRAG.md` (Bank 9 Bits 100-103, 106, 107;
Text-Ids 2000/20B0 40, 2030 40-49, 2050 40-44, 2080 40-41). Vorrunde: `befunde_runde40/UEBERGABE.md` "Befund STAGE2".
Modul: `re15_port/engine/src/ventilkette_r41.c` + `include/re15_ventilkette_r41.h` (alle Konstanten mit Beleg im Kopf),
Daten `engine/src/gen/ventilkette_r41_{texte,original,programme,wasser}.inc` aus
`tools/r41_ventilkette/gen_r41_ventilkette.py` + `programme.py`. Bilder: `C_belege/`.

## 0. Stand / Protokoll (neueste zuerst)

* 2026-10-24 ab 05:40 (Nachbesserung 1 nach Abnahme 0, `C_abnahme_0.md`, Opus). Baum sauber (`status --porcelain`
  leer, HEAD 4af8fa3d). Arbeitsplan je Mangel M1-M9 + H1-H3 in §0b; Messordner scratchpad `nb1/`.
* 2026-10-24 02:51-03:10 (vierte Fortsetzung, Opus; Vorgaenger vom Sitzungslimit ~01:00 beendet). Baum sauber
  (`status --porcelain` leer, HEAD 271b05ea): der TEMPORAERE Nachbau des Spur-A-Hakens in aot_common.c war schon
  zurueckgenommen, die exe 00:56 aber MIT ihm gebaut -> `local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`
  (exe 02:51; Quelle = Bau 00:41, auf dem beide r41-Integrationsriegel gruen liefen -> nicht wiederholt).
  Gezielte Unit-Gruppe (Regex §5) 02:56: `100% tests passed, 0 tests failed out of 37`. Lauf kette_mit_a (mit dem
  Haken, 00:56-00:58) ausgewertet -> §6 Querbezug A (ohne den Haken haengt die 20B0-Szene, mit ihm laeuft sie bis
  zur Freigabe). NEUE Messung ring_nach_flut (Eintritt in das geflutete 2030 ueber die 20B0-Ost-Tuer, 03:03-03:05)
  -> §4, §6 OFFEN, §7 Frage (f). Dossier nachgezogen: §1.1 Cut der 2000-Tuer Slot 3 (3, nicht 13), §3.3/§4 Spritzer
  = EIN Ereignis (ddb65cfc), §5 Teil B + Gruppenlauf 2/2 + Unit 239 Pruefungen.
* 00:30-00:58 (Vorgaenger, aus Commits und Logs scratchpad r41c2/ uebernommen, NICHT wiederholt): e1cb3352/b486f182
  integration_r41_ventilkette_b (erste Fahrt 00:36-00:41 PASS, 291.85 s, exe 22:17); ddb65cfc Spritzer ohne
  erfundene Rate + Nachmessung flut_spritzer (exe 00:41); 271b05ea Riegel C prueft den EINEN Spritzer, beide
  r41-Riegel 00:46:38-00:55:43 auf der exe 00:41 gruen (2/2, 545.09 s); ab 00:56 temporaerer Spur-A-Haken (nicht
  committet, vom Orchestrator zurueckgenommen, Patch scratchpad r41c2/temp_haken_spur_a_aot_common.patch) + Lauf
  kette_mit_a (scratchpad r41c2/mess/kette_mit_a, Skript r41c2/mess_kette_a.sh).
* 2026-10-24 00:26-00:29 Luecken am Spiel geschlossen (neue Messlaeufe, keine Wiederholung; Skript scratchpad
  r41c2/mess_b.sh, exe-Kopie re15_pc_r41c_mess2.exe): Punkt 5 GANZ (Pult -> 80 V -> "Power supply OK." -> Hebel =
  Original sub04 -> Wasser steigt) und Hebel vorher mit der Aktionstaste gesperrt; Punkt 2 Pult ohne Ventil 13
  (Text 40, zwei Seiten); Punkt 4 Leiter 20B0 -> 2050 und 2050-Leiter -> 20B0; Punkt 6 2030 mit Bit 107 bitgleich
  zum Stand ohne Flut. Belegbilder pult2050_loesung_sheet / pult2030_ohne_ventil / raum2030_leer_vergleich (§4).
  Befund beim Lesen von integration A: ohne den Haken der Spur A haengt die 20B0-Szene (§6, Querbezug A).
* 2026-10-24 00:21 (dritte Fortsetzung, Opus; Vorgaenger vom Fable-Limit ~23:45 beendet). Baum sauber, HEAD 2088dd38.
  Bau `local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)`: die exe vom 23.10. 22:17 ist der
  Stand aller Commits (seit 76fdeca4 nur Dossier/Bilder/Auftrag; `git diff --stat 76fdeca4 HEAD -- re15_port` leer),
  also gelten die Laeufe des Vorgaengers. Aus dessen Protokollen (scratchpad r41c/) uebernommen, NICHT wiederholt:
  Integrationsgruppe 16/16 gruen (Ende 23:46:27, 4908 s), Unit-Gruppe 37/37 gruen (23:46:55), Messlaeufe Ventil 13 /
  Pult -> Bruecke / Rechner 2080 (23:47-23:51) mit den drei Belegbildern 55f511a8 — Logs gelesen, in §4/§5 eingeordnet.
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

## 0a. Wegweiser je Nutzer-Punkt (Messung vorher / Beleg / Umsetzung / Messung nachher / Riegel)

| Punkt (AUFTRAG.md) | vorher | Beleg | Umsetzung | nachher (exe) | Riegel |
|---|---|---|---|---|---|
| 1 Leiter 2000 gesperrt + Text, bis die Flut lief | §2 (Slot 4 = Tuer nach 2050) | §1.1 @0x0169E, Strahlen @0x0174C.., Form @0x01B12 | §3.1 | §4 "Leiter" (C_belege/leiter2_vergleich.png) | unit B; integration_r41_ventilkette Lauf b; _b Lauf K (Elza 2001) |
| 2 Bruecke oben, Pult -> Ventil 13 -> "Lower the bridge?" -> Fahrt | §2 (Planke y -1800) | §1.3 obj 0 @0x01B6C, Lift-Fahrt @0x01ED2.., Pult/Tank-Lage | §3.2 | §4 "Bruecke", "Punkt 2 am Spiel", "P2 Pult ohne Ventil 13" | unit C; _b Lauf F |
| 3 Ventile -> Warnung -> 60 s -> Flut ueber den Ring -> Tod mit Ertrinken; Ost-Tuer -> 20B0 | §2 (keine Stelle, Cut 14/15 nie) | §1.2 @0x019C2/sub02/sub05, §1.5 Rate @0x00CC8, Blende @0x174a, Spritzer @0x800456b8 | §3.3 | §4 "Flut am Spiel", "Spritzer nachgemessen", "Blende"; Szene 20B0 §6 Querbezug A | unit D/D2; integration_r41_ventilkette Laeufe a/c |
| 4 danach 20B0 statt 2000, Leiter frei (kein Wasser) | §2 (20B0 nur per Sprung) | §1.1/§1.2 (kein Tuersatz -> 20B0, Kamera-Zwillinge, Strahlen @0x01E96..) | §3.4 | §4 "Leiter", "P4 Leiter 20B0 -> 2050", "P4 2050-Leiter -> 20B0" | unit E; _b Laeufe G/H |
| 5 2050: Hebel erst nach dem Pult-Raetsel (80 V) | §2 (Hebel immer bedienbar) | §1.4 sub04 @0x022E6, Slot 8 @0x01D38, Cursor @0x01D4C, 2060/11F0/RE2 ROOM2130 | §3.5 | §4 "Pult 2050", "P5 Pult -> Hebel", "P5 Hebel vorher" | unit F; _b Laeufe D/E/J (Elza 2051) |
| 6 2080-Rechner leert auch 2030 | §2 ((3,245) Raum-Merkflag) | §1.5 sub02 @0x00C86..@0x00CF1 | §3.6 | §4 "Punkt 6 am Spiel", "P6 2030 nach dem Rechner" | unit G; _b Lauf I |

unit = unit_r41_ventilkette (Abschnitte A-G, D2), _b = integration_r41_ventilkette_b. Offen nach Messung: §6
(Querbezug A; Ring-Eintritt in das geflutete 2030 = Frage §7 f).

## 0b. Nachbesserung 1 (Abnahme 0 vom 2026-10-24, `C_abnahme_0.md` §4) — je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher

Messordner: scratchpad `nb1/` (`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb1`).
Die Messungen der Abnahme (`abn0/mess/*`) werden NICHT wiederholt, sondern als "vorher" zitiert.

### M1 (hoch, Punkt 5) — Kisten im Becken: Hebel = Ereignis 2 hebt das Wasser ohne das Pult
(in Arbeit)

### M2 (hoch, Punkte 3/4) — 20B0-Auswasch-Szene braucht den AUTO-Haken der Spur A
(in Arbeit)

### M3 (mittel) — Installer loeschte (5,0x1D)/(5,0x1E) in jedem Raum
(in Arbeit)

### M4 (mittel) — Riegel C belegte die Farbe der Ertrink-Blende nicht
(in Arbeit)

### M5 (niedrig, Entscheidung) — geflutetes 2030 ueber den Ring betretbar (Frage f)
(in Arbeit)

### M6 (formal) — Haken in main.c / game_step_common.c, zwei fremde Riegel
(in Arbeit)

### M7 (niedrig, Dossier) — 2070 nach der Flut nur ueber 2040
(in Arbeit)

### M8 (niedrig, Text) — Ventile melden nach dem Entleeren weiter "flooded"
(in Arbeit)

### M9 (niedrig, Doku) — Kopf des Unit-Riegels nannte (5,0x3D)/(5,0x3E)
(in Arbeit)

### H1-H3 (Hinweise)
(in Arbeit)

## 1. RE-Belege (Original, selbst aus den Bytes gelesen)

Alle Datei-Offsets beziehen sich auf `re15_port/shared_assets/PSX/STAGE2/ROOMxxxx.RDT` (Leon); die Elza-Varianten
ROOM2001/2031/2051/2081/20B1 sind in den hier zitierten Regionen byte-gleich (der Riegel vergleicht alle Scheiben
in beiden Fassungen).

### 1.1 ROOM2000 — die Leiter nach 2050 (Punkt 1)
* Tuersaetze main00 @0x0161E..@0x016BD (fuenf `3b`): Slot 0 @0x0161E -> ROOM1260 (hinauf), Slot 1 @0x0163E -> ROOM1260
  Cut 8, Slot 2 @0x0165E -> ROOM2010, Slot 3 @0x0167E (Rechteck (-20300,-8200,1000,4000)) -> ROOM2070 Cut 3 (pc[22..25] `01 07 03 00`; Cut 13 tragen
  umgekehrt die 2070-Tueren nach 2000, §1.2),
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
  + EIN Spritzer am Spieler, wenn die steigende Flaeche seine Fuesse erreicht (`wy <= pl->y`, einmal je Flut;
  re15_esp_fx_spawn_rows mit dem Wasser-Schritt-Effekt 0x06050D00 = `lui a0,0x605` / `ori a0,a0,0xd00`
  @0x800456b8/bc in FUN_80045630, Gier `lh a1,106(a1)` @0x800456d0, `jal 0x80019700` @0x80045710). Das Original
  setzt diesen Effekt nur je Fuss-Aufsetzer (Bildwort-Bit 0x4000 `andi v0,v1,0x4000` @0x8003253c -> Fuss 7 - 3*Bit12
  @0x80032548-60 -> `jal 0x80045630` @0x8003255c, Wasserboden `andi v0,s0,0x80` @0x800456ac); waehrend der Flut steht
  der Spieler unter der Pad-Sperre, es gibt keinen Aufsetzer -> PORT-WAHL "ein Ereignis, keine Rate". Die erste
  Fassung "alle 6 Takte" (RE15_R41C_SPRITZER_TAKT 6) hatte keine Adresse und ist entfernt (ddb65cfc).
  Jeder Tod in 2030 (Ertrinken oder Spinnen: tick `if (hp < 0) s_timer = 0`) loescht den Zaehler, also setzt der
  naechste Raumstart (Titel -> LOAD GAME) keine Flut-Bits; der Raumstart loescht Marker, Blende, Zustand.
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
    [Stand 22:00, alte Fassung: Spritzer alle 6 Takte ab F2733 (Zustandslog fx 1/2/3 bei F2733/2739/2745) — seit
    ddb65cfc EIN Spritzer bei F2728, s. "Spritzer nachgemessen"], **F2746 Tod** (`ERTRUNKEN: Wasser y
    -6300, Spieler y -5400 -> hp -1`, = 125 Bilder nach dem Flutbeginn, 126 Schritte x 50 = 6300 mit dem ersten im
    Marker-Takt), Blende (`config ch=0 abr=2 mask=6`, `kick ch=0 value=0x5000`), Todeskette (weisser Blitz F2820,
    pst 7 bei F2859, YOU DIED F2880..F2960), Ende F2960#2030. Ein spaeteres Ende (3500 ohne Raumfilter) lief in den
    Neustart nach dem Tod, dessen Bildzaehler die Framedumps 2400..3400 mit ROOM1170-Bildern ueberschrieb (erster
    Lauf, verworfen).
  - Blende (22:05): Bildmitte 100x60 F2740 (29,36,29) -> F2760 mit "ABR 0 + Maske Blau": (30,37,0) = Blau WEG
    (render_pc.c:1282 kennt nur ABR 1 additiv / 2 subtraktiv, 0/3 fallen auf subtraktiv zurueck -> "minus Blau");
    mit ABR 2 + Maske Rot+Gruen: (0,0,30) = Blau bleibt (C_belege/flut_blende_vergleich.png). Die Form ist auf der PSX
    dieselbe (GPU B-F je Kanal, Kachel-RGB = Helligkeit & Maske @0x800218d0).
  - **Spritzer nachgemessen** (Vorgaenger 00:43-00:45, exe 00:41 = Stand ddb65cfc, gleicher Aufbau wie oben,
    Skript scratchpad r41c2/mess_flut.sh, Log r41c2/mess/flut_spritzer/debug.log + state.txt, von mir gelesen):
    `FLUT: Wasser steigt` (F2621) -> F2728 `[ventilkette41] SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y
    -5400), Effekt 0x06050D00` GENAU EINMAL (Zustandslog fx=1 F2728..F2740, ab F2741 fx=0) -> `config ch=0 abr=2
    mask=6 step=0 bucket=7` + `kick ch=0 value=0x5000` -> F2746 `ERTRUNKEN: Wasser y -6300, Spieler y -5400 -> hp
    -1, blaue Blende` (Zustandslog hp 100 -> -1 bei F2746). Bildreihe **C_belege/flut_spritzer_sheet.png**.
* Echter Weg 2030 -> Ost-Tuer -> 20B0: integration_r41_ventilkette Lauf A (§5). Die Original-Szene in 20B0 (sub02 ->
  Zone Slot 7 -> sub05 -> Set(3,235)) laeuft im Port erst mit dem Haken der Spur A: §6 Querbezug A (Lauf kette_mit_a,
  Bild C_belege/querbezug_a_20b0_szene_sheet.png).
* **Eintritt in das geflutete 2030 nach der Flut** (NEU 03:03-03:05, exe 02:51, Kopie re15_pc_r41c3_mess.exe, Skript
  scratchpad r41c3/mess_ring.sh, Log r41c3/mess/ring_nach_flut/debug.log + state.txt): Zustand nach der Szene
  `RE15_SET_FLAG=3:235,9:103,9:100` (ohne 107), Sprung 20B0, `RE15_FIRE_AOT=3@200#20B0` = die Original-Ost-Tuer
  Slot 3 @0x019C2 (`[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)`)
  -> `[room] PC loaded room2030.rdt` -> `[ventilkette41] ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1`
  + `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)`. Der Spieler steht auf dem Ring (y -5400), die
  Wasserflaeche 1800 ueber seinen Fuessen, und laeuft (Eingabe U2 ab F330: x 6782 -> 6368, mo 105); kein Ertrinken
  (der Ertrink-Takt laeuft nur waehrend der Flut, s_flut), eine Spinne greift im Wasser an (hp 100 -> 77 bis F460).
  Bild **C_belege/ring_nach_flut_sheet.png** (F60..F480, Cut 0: die Flaeche als Band in Brusthoehe vor Leon).
  Dieselbe Landung haben 2070 Slots 1/2 @0x00A5A/@0x00A7A (unbedingt in main00, Ziel ROOM2030 Cut 0, spawn
  (6550,-5400,-100) = dieselben Bytes wie 20B0 @0x019C2 ab pc[14]; Tuersaetze selbst dekodiert). Nicht gemessen: ob
  2070 nach der Flut erreichbar ist (2070 Slot 4 -> ROOM11A0, Slot 5 -> 2040; Nutzer: 2040 braucht den Aufzug mit
  Strom). Der Weg zum Rechner 2080 braucht den Ring NICHT (aus den Tuersaetzen, nicht gefahren): 2050 Slot 1 @0x01C6A
  -> 2020, 2020 Slot 1 (@0x0088A, bei (3,219) = 0 erst der Ereignis-Platz Slot 4 @0x00852 + Tuer sce 0 @0x00866) ->
  2030 Tuerebene (spawn y -16200, ueber dem Wasser) -> 2030 Slot 1 @0x01AEA -> 2080. -> §6 OFFEN, §7 Frage (f).
* **Punkt 2 am Spiel** (Vorgaenger 23:47-23:49, exe-Kopie re15_pc_r41c_mess.exe, Skript scratchpad r41c/mess_punkte.sh,
  Spinnen per Kill-Bits wie im Flut-Lauf; die Kamera bleibt beim Sprung-Cut 0, weil der Spieler nicht laeuft;
  Logs scratchpad r41c/mess/{v13,pult_bruecke}/debug.log, von mir gelesen):
  - Ventil 13 (`RE15_PLAYER_POS=-1500,-6400,2048,1`, Log `pl pos=(-1500,-1800,-6418) rot=2048` = Landeplatz Band 1
    am Tank): F330 Aktion -> `[ventilkette41] ROOM2030 Ereignis 25 -> Port-Programm 2030_v13_frage` ->
    `[msgtext] room=2030 id=41 "Intake valve 13. Will you open it?"` (Bild **C_belege/ventil13_frage.png** = F360,
    Schreibmaschine "Intake valve 13. / Wi"); F453 Ja -> `[msgtext] room=2030 id=42 "You've opened valve 13. The
    pressure is rising."` (Bit 101); F699 Aktion -> `Port-Programm 2030_v13_zustand` -> id 47 "Intake valve 13 is open."
    (die Programmwahl selbst belegt Bit 101 = 1, ventilkette_r41.c re15_ventilkette_r41_ereignis).
  - Pult mit offenem Ventil 13 (`RE15_SET_FLAG=...,9:101`, `RE15_PLAYER_POS=350,-9500,1024,3`, Log `pl pos=(350,-5400,
    -9500)` = Ring Band 3 vor der Nordwand): F330 Aktion -> `Ereignis 24 -> Port-Programm 2030_pult_senken` ->
    id 43 "The pressure gauge reads 80. Lower the bridge?"; F453 Ja -> `[scd F456] Cut_chg(4)` + `[szenenschutz] F456 an`
    -> `[prop-member F516] prop[0] obj=0x00 m=1 → val=-1800` (Endlage Member_set(1,-1800) nach 30 Schritten je 120
    jeden zweiten Takt: 456 + 60 = 516) -> Kamera zurueck (Cut_old, cut 0) -> `[scd F550] letterbox closed -> gameplay`;
    F576 Aktion -> `2030_pult_unten` -> id 44 "The bridge is down.". Bilderreihe **C_belege/bruecke_fahrt_sheet.png**:
    F345 Frage 43 (Schreibmaschine), F450 Frage komplett mit Yes/No, F465 Cut 4 (Draufsicht, Fahrt laeuft), F480/F495/
    F510 die Planke sinkt ueber dem Becken, F525 unten (Steg-Ebene), F600 zurueck an der Nordwand mit Text 44.
* **Punkt 6 am Spiel** (Vorgaenger 23:49-23:51, Bits 103 + 100 vorgesetzt, Log scratchpad r41c/mess/rechner2080b/):
  erster Versuch `RE15_PLAYER_POS=-11550,-6000,1024,8` verworfen (die Kollision schob den Spieler nach (-11185,-7206),
  Blick 1024 = -z, der Vorwaertspunkt traf das Rechner-Rechteck (-12050,-7250,1000,1000) nicht -> kein Ereignis);
  zweiter Lauf `(-11185,-7206)` Blick 3072 (+z, Vorwaertspunkt z -6586 im Rechteck), Log `[parity] JUMP-Spawn ersetzt
  durch RE15_PLAYER_POS (-11185,-7206) rot=3072 band=8`: F330 Aktion -> `ROOM2080 Ereignis 2 -> Port-Programm
  2080_rechner_erw` -> `[msgtext] room=2080 id=0 "From here maybe I can control the water level.Will you operate the
  computer?"` (Original-Text) -> F453 spult, F576 Ja -> `[scd F599] Cut_chg(7)` -> Wasser obj 0 sinkt (Original For 148
  @0x00CCC, 2 Evt_next je Schritt: 599 + 296 + Sleep 20 = 915) -> `[scd F915] Cut_chg(3)` -> `[msgtext] room=2080 id=40 "The treatment pool is being drained as well."`
  (Bit 107 steht im selben Ja-Zweig `22 09 6b 01`; das Zustandslog fuehrt Bank 9 nicht, das Bit misst Riegel G).
  Bilderreihe **C_belege/rechner2080_sheet.png**: F360 Frage (Sprung-Kamera), F600 Cut 7 Wasser sinkt, F750, F900
  (Becken leer bis auf den Grund), F930 Cut 3 Text 40 (Schreibmaschine), F1200 Text 40 komplett.
* **Luecken am Spiel (dritte Fortsetzung, 2026-10-24 00:26-00:29, scratchpad r41c2/mess/<lauf>/debug.log)** — was bis
  dahin nur der Unit-Riegel zeigte, jetzt mit der exe (Eingabeskript = Aktionstaste/Steuerkreuz, Bildnummern = Spielbilder):
  - **P5 Pult -> Hebel, ein Lauf** (`p5a_pult_loesung`, `RE15_PLAYER_POS=4900,-7100,1024` vor dem Pult wie pult2050c):
    F330 Aktion -> `ROOM2050 Ereignis 9 -> Port-Programm 2050_pult_ein` -> id 43 "A high voltage switchboard. The pump
    needs 80 volts." -> F453 -> id 41 "Will you push them?" -> F531 Ja -> `[scd F534] Cut_chg(15)`; F564 RIGHT (8 Bilder),
    F581 SQUARE -> `Pult 2050: Hebel 3 -> EIN, Schalter 0x4, 40 V`; F605 LEFT (36 Bilder), F650 SQUARE -> `Hebel 1 -> EIN,
    Schalter 0x5, 70 V`; F674 RIGHT (60 Bilder = 12000 bei 200 je Bild `2f 00 c8 00` sub07), F743 SQUARE -> `Hebel 4 ->
    EIN, Schalter 0xD, 80 V` -> nach den 30 Ruhebildern (RE2 @0x0171C) `Pult 2050 GELOEST: 80 V -> Bit 106, "Power supply
    OK."` + id 42 (= 11F0 msg 2) -> F836 zu -> `Pult 2050 verlassen (Schalter 0xD, 80 V)` (sub10 @0x0240C). Danach der
    Hebel-Platz Slot 6 (Ereignis 4) ueber `RE15_FIRE_AOT=6@900` (= re15_aot_fire_slot, derselbe Pfad wie die Aktionstaste;
    das Pult steht an der Saeule, der Hebel in der Nische dahinter — ein offenes Skript um die Wand herum waere geraten)
    -> KEIN Port-Programm mehr, das Original sub04: id 0 "A lever to fill and drain water. Will you move it?" -> F1022 Ja
    -> `[prop-member F1026] prop[5] obj=0x04 m=1 → val=-1400` (@0x0230C ff.) -> `[scd F1046] Cut_chg(4)` (Sleep 20) ->
    For 180 x Speed -20 (@0x02324/@0x02328) -> `[prop-member F1226] prop[6] obj=0x05 m=1 → val=-5000` + `Cut_chg(7)` ->
    `[scd F1240] letterbox closed -> gameplay`. Bildvergleich Cut 4: F1050..F1155 0 Pixel Unterschied, F1200 736, F1215
    1283 Pixel (Rahmen x 52..233, y 188..215 = die Wasserflaeche taucht unter dem Gitter ins Bild).
  - **P5 Hebel vorher, Aktionstaste** (`p5b_hebel_gesperrt`, `RE15_PLAYER_POS=4300,-11150,0,3` = Nische westlich des
    Hebels, Blick +x, Vorwaertspunkt x 4920 im Rechteck (4700,-11550,800,800) @0x01E28): F330 Aktion -> `ROOM2050
    Ereignis 4 -> Port-Programm 2050_hebel_sperre` -> id 40 "The pump has no power. The switchboard must be set first.";
    kein Cut_chg, kein Member_set. Bilderreihe **C_belege/pult2050_loesung_sheet.png** (F420 Hebel vorher, F540 Cut 15,
    F585 40 V, F660 70 V, F750 80 V mit drei gruenen Marken, F795 "Power supply OK.", F990 Original-Frage am Hebel,
    F1155 Cut 4, F1215 Wasser im Bild).
  - **P2 Pult ohne Ventil 13** (`p2a_pult_ohne`, Ring vor der Nordwand wie pult_bruecke, ohne Bit 101): F330 Aktion ->
    `Ereignis 24 -> Port-Programm 2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the bridge. The
    pressure gauge reads zero.Intake valve 13 is closed."` (zwei Seiten, F453 blaettert); kein Cut_chg(4), keine
    Fahrt. Bild **C_belege/pult2030_ohne_ventil.png** (F440 Seite 1, F560 Seite 2).
  - **P4 Leiter 20B0 -> 2050** (`p4a_leiter20b0`, Zustand nach der Szene: `RE15_SET_FLAG=3:235,9:103,9:100`, Spieler
    vor der Leiter wie integration B): F330 Aktion -> `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500)
    target_cut=0 spawn=(9100,-5400,4100)` -> `[room] PC loaded room2050.rdt` (kein Sperrtext) -> in 2050 sofort
    `[ventilkette41] ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel ROOM2000)`.
  - **P4 2050-Leiter hinab -> 20B0** (`p4b_leiter2050`, gleicher Zustand, `RE15_PLAYER_POS=9100,4300,3072` wie
    integration_r36_dachkiste echt2050): F330 Aktion -> `[aot] DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000)
    target_cut=9 spawn=(-26450,-1800,10250)` -> `[room] PC loaded room20b0.rdt` (Original-Ziel ROOM2000 Cut 10 @0x01C4A,
    Zwilling Cut 9 laut §1.2).
  - **P6 2030 nach dem Rechner** (`p6b_leer_cut4`, Cut 4 erzwungen, Bits 101/102/103/100 + 107): kein `unter Wasser`-
    Satz im Log (nur `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)`); das Bild ist **bitgleich** (0 Pixel
    > 24) zum Stand ohne Flut (r41c/mess/bruecke_unten, nur 101+102) und unterscheidet sich vom Flut-Stand ohne 107
    (r41c/mess/flut_cut4) in 39271 Pixeln. Bild **C_belege/raum2030_leer_vergleich.png**.

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
  D2 Tuerebene (Tod mit der Endlage -16200, flut_bei + 323). Ergebnis bis 00:41: PASS (233 Pruefungen), Leon + Elza
  (Fortsetzung: Blenden-Riegel = abr 2, Masken r/g 0xff, b 0, Helligkeit 160). Seit ddb65cfc zusaetzlich D: genau EIN
  Spritzer bei Wasser y -5400 (= Fuesse) vor dem Tod, D2: einer im Todestakt auf der Tuerebene.
  **Lauf 2026-10-24 02:56 (exe/Test-exe 02:51 = Stand 271b05ea): `PASS (239 Pruefungen)`.**
* `integration_r41_ventilkette` (tests/integration/test_r41_ventilkette.cmake): A echter Weg 2030 Ventile (Aktionstaste,
  Blick 2048) -> Frage -> Ja (4. Druck) -> Warnung -> zu (F777) -> Zaehler -> Ost-Tuer feuert (RE15_FIRE_AOT=0@900) ->
  ROOM20B0 geladen -> Bits 103 + 100, kein 2070; B 2000 Leiter (-26000) = Sperrtext `[msgtext] room=2000 id=40`, kein
  Raumwechsel nach 2050; C Flut am Spiel (Kill-Bits, FIRE_AOT 14, Zustandslog): Zaehler 1800 +- 3 Bilder, Flut unter
  Cut 4, Tod 125 +- 2 Bilder danach bei Wasser -6300 / Spieler -5400, Blende kick 0x5000, Framedump F2750, Ende
  F2960#2030. **`ctest --timeout 900 -R "^integration_r41_ventilkette$"` -> `100% tests passed, 0 tests failed out
  of 1` (213.97 s)** — davor zwei rote Laeufe: A/B mit den alten Takten (Tuer statt Ventile; Leiter nicht getroffen).
  Seit 271b05ea prueft C zusaetzlich `SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400)` VOR `ERTRUNKEN:`
  und genau EIN `SPRITZER:`-Satz (Ereignis, keine Rate). A prueft den Szenenverlauf in 20B0 NICHT (der haengt ohne den
  Haken der Spur A, §6) — der Riegel bleibt also mit und ohne diesen Haken gueltig.
* `integration_r41_ventilkette_b` (tests/integration/test_r41_ventilkette_b.cmake, Kopf-Praefix re15_pc_r41cb_): die
  Glieder, die A nicht faehrt — D Punkt 5 in einem Lauf (Pult 2050 -> Text 43 -> "Will you push them?" -> Cut 15 ->
  Cursor RIGHT 8 / LEFT 36 / RIGHT 60 -> 40/70/80 V -> 30 Ruhebilder -> Bit 106 + "Power supply OK." -> sub10 -> Hebel
  Slot 6 per FIRE_AOT = Original sub04 -> obj 4 y -1400 -> Cut 4 -> obj 5 y -5000); E Hebel vorher = 2050_hebel_sperre
  (Text 40, kein Cut 4, kein Member_set); F Bruecken-Pult ohne Ventil 13 = 2030_pult_ohne (Text 40, keine Fahrt);
  G 20B0 nach der Szene mit 103 + 100: Leiter Slot 4 = Tuer nach 2050, dort Slot 0 -> 20B0 Cut 9; H 2050-Leiter hinab
  -> 20B0 Cut 9; I 2030 mit 103 ohne 107 = Wasser y -7200 (Gegenprobe), mit 107 kein Wasser; J Elza 2051 = D bis
  "Power supply OK." + Original-Hebel; K Elza 2001 Leiter = Sperrtext 40 woertlich. Erste Fahrt (exe 22:17, 00:36-00:41,
  Protokoll scratchpad r41c2/ctest_b2.txt): `100% tests passed, 0 tests failed out of 1` (291.85 s).
* Beide r41-Integrationsriegel auf der exe 00:41 (= Quellstand ddb65cfc/271b05ea; 00:46:38-00:55:43, Protokoll
  scratchpad r41c2/ctest_integ_r41.txt, `exit=0`):
  `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b)?$" --timeout 1200`
  -> **`100% tests passed, 0 tests failed out of 2`** (`Total Test time (real) = 545.09 sec`; A/B/C 216.23 s,
  Teil B 328.81 s). Die exe 02:51 ist aus derselben Quelle gebaut (nur der temporaere Spur-A-Haken war dazwischen
  drin und wieder heraus) -> nicht wiederholt.
* Gezielte Gruppe (ctest, Regex woertlich):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie)$"`
  -> `100% tests passed, 0 tests failed out of 18` (18:35, vor dem Riegel D2).
  Fortsetzung, erweitert um die Gruppen der beruehrten Dateien (scd_room_setup.c, scd_vm.c, main.c, kartenbits):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen)$"`
  -> 21:50 `97% tests passed, 1 tests failed out of 37` (unit_r41_ventilkette D2, Erwartung 324 statt 323) ->
  nach dem Riegel-Fix und dem Blenden-Fix (Lauf 23:46:55 auf der exe/den Test-exe vom 22:17, Protokoll scratchpad
  r41c/ctest_unit_r41c_final.txt): **`100% tests passed, 0 tests failed out of 37`** (`Total Test time (real) =
  6.24 sec`).
  **Abschluss-Lauf 2026-10-24 02:56** (gleicher Regex, Bau 02:51 = Stand 271b05ea ohne den temporaeren Haken,
  `--output-on-failure`, Protokoll scratchpad r41c3/ctest_unit.txt, `EXIT=0`):
  `ctest --test-dir re15_port/build -j 6 --timeout 600 --output-on-failure -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen)$"`
  -> **`100% tests passed, 0 tests failed out of 37`** (`Total Test time (real) =   8.32 sec`).
* Integrationsgruppe der beruehrten Raeume (seriell, exe): Regex
  `^(integration_r41_ventilkette|integration_r37_fahrstuhl_n1|integration_r37_itemband|integration_r37_itemband_lift|integration_r37_kiste|integration_r37_kiste2feld|integration_r37_kistenregel|integration_r36_gegnerzahl|integration_r36_dachkiste|integration_r38_spinnen_echtlauf|integration_r38_wasser|integration_r38_wasserzombie|integration_map_uebergang|integration_cross_stage|integration_item_name_census|integration_save_counter_pin)$`
  -> 19:03 (Vorgaenger, alte exe, abgebrochen vom Sitzungslimit nach 4 von 16): dachkiste rot (echt2050 Leiter — der
  Querbezug unten), gegnerzahl / fahrstuhl_n1 / itemband gruen (103 / 155 / 829 s).
  Fortsetzung (exe vom 22:17 = Stand aller Commits, `--timeout 1800`, weil dachkiste allein >1100 s braucht; Lauf
  ~22:24:39-23:46:27, Protokoll scratchpad r41c/ctest_integ_gruppe.txt, `exit=0`):
  **`100% tests passed, 0 tests failed out of 16`** (`Total Test time (real) = 4908.47 sec`; dachkiste 1425.21 s,
  itemband_lift 1379.32 s, itemband 824.22 s, kistenregel 284.60 s, r41_ventilkette 214.59 s, wasser 179.11 s,
  fahrstuhl_n1 132.01 s, wasserzombie 122.24 s, spinnen_echtlauf 115.02 s, gegnerzahl 90.71 s, kiste 63.03 s,
  kiste2feld 57.60 s, save_counter_pin 19.68 s, map_uebergang 0.69 s, item_name_census 0.31 s, cross_stage 0.02 s).
  Kein roter Fenster-Haken -> nichts einzeln nachzufahren. Seit diesem Lauf hat sich im Engine-Code nur flut_tick
  (Spritzer, ddb65cfc) geaendert; der Takt laeuft nur waehrend der Flut in 2030 (s_flut), die keine der 15 fremden
  Gruppen faehrt — die r41-Riegel selbst liefen auf der neuen exe (2/2 oben).

* Querbezuege (Memory "neue Regeln widerlegen alte Riegel", beim ersten Integrationslauf gefunden):
  - `integration_r36_dachkiste` echt2050/elza2051 fahren 2050 -> 2000 -> Leiter slot 4 @0x0169E -> 2050; die Leiter ist
    jetzt bis zur Flut gesperrt -> die zwei Laeufe setzen `RE15_SET_FLAG=9:100` (Leiter frei; Gegenstand des Riegels
    ist der Kisten-Record, nicht die Leiter). Lauf 1 der Gruppe fiel genau an "DOOR FIRE slot=4 rect=(-27100,10300"
    durch (1115 s unter Last).
  - `integration_r37_itemband` schrot2030 geht ueber den Steg (GEHE -1050 -7700 -> -1050 7100 = Zelle 12) zu den
    Schrotpatronen; der Steg ist bis zum Pult-Raetsel gesperrt -> Lauf setzt `RE15_SET_FLAG=9:101,9:102`.
  Beide Aenderungen sind Kommentar-belegt in den cmake-Dateien.

## 6. OFFEN
* **Querbezug A — die 20B0-Szene haengt ohne den Haken der Spur A, mit ihm laeuft sie** (ausgewertet 2026-10-24 02:52):
  - OHNE Haken (integration A, Lauf build/tests/integration/r41_ventilkette/lauf_912f1ca0_a/debug.log, 00:47, exe
    00:41 = Quellstand 271b05ea): `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11
    spawn=(5950,-1800,-6400)` -> `[room] PC loaded room20b0.rdt` -> sub00 Ck(3,235)=0 @0x01AAA -> Evt_exec sub02 @0x01E06
    -> `[scd F900] Cut_chg(11)` (@0x01F00) -> `Plc_dest(slot=0 mode=0x05 dest=(-15920,-6355) flag=0)` (@0x01F24) ->
    Lauf nach Westen (Cut 12 F30, Cut 13 F60/F90) -> ab F120 steht der Spieler bei (-15820,-1800,-6355) in Cut 14, mo 2,
    bis zum Ende F1040: im Rechteck der Zone Slot 7 (@0x01DF2, (-17400,-8700,2250,4600), Band 1, sce 3 Ereignis 5),
    aber sub05 startet nie (kein `thread start ... first_op=0x46` = Aot_reset @0x01F5C, kein Cut_chg(7), kein
    `letterbox closed`). Ursache im Port: re15_aot_scan ueberspringt waehrend einer Szene jeden Platz ausser CAM_SWITCH
    (aot_common.c:1169 `if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;`; in_cinematic @1077 =
    player_mode 2 oder Balken schliessen noch, player_mode 2 setzt main.c:5795 solange re15_cine_active() = (1,27) ||
    (2,7), game_state.c:256), und sub02 setzt (2,7)=1 @0x01EF8 / (1,27)=1 @0x01EFC, ohne sie zu loeschen — das tut erst sub05
    @0x01FC0/@0x01FC4. Ohne den Haken bleibt der Spieler in 20B0 unter der Pad-Sperre stehen.
  - MIT Haken (temporaerer Nachbau, NICHT committet: AUTO_EVENT in 20B0/20B1 trotz in_cinematic, Patch scratchpad
    r41c2/temp_haken_spur_a_aot_common.patch; Lauf scratchpad r41c2/mess/kette_mit_a, 00:56-00:58, Aufbau wie
    integration A ohne Kill-Bits, Ende F700#20B0, Bild C_belege/querbezug_a_20b0_szene_sheet.png): 2030 F900 Ost-Tuer
    -> 20B0 F0 Cut 11 + Pos_set (2380,-1800,-6355) @0x01F08 (Zustandslog F1 `PL(2380,-6355)`) -> Plc_dest wie oben ->
    beim Eintritt in Zone Slot 7 `thread start slot=11 first_op=0x46` = sub05 @0x01F5C (zwischen F100 und F105; mit 200
    je Bild ab F90 x -12420 erreicht der Spieler die Zonenkante x -15150 in F104) -> F105
    Cut_chg(7) @0x01F6A -> Plc_dest (-20370,-7305) @0x01F80 / (-21020,-9355) @0x01F8E (Bild F120: Cut 7, die Tuer
    fliegt auf) -> F146 Cut_chg(15) @0x01F9C -> Plc_dest Modus 9 (-22820,-7555) @0x01F9E / Modus 7 (-20620,-9705)
    @0x01FA8 -> Sleep 60 @0x01FB2 -> F228 Cut_chg(6) @0x01FB6 + Plc_ret @0x01FB8 (Zustandslog pm 2 -> 1) -> Cut_auto 1
    -> **Set(3,235)=1 @0x01FBC** -> Set(2,7)=0 @0x01FC0 -> Set(1,27)=0 @0x01FC4 -> Sca_id_set x10 -> Evt_end @0x01FF0.
    Set(3,235) wird nicht eigens geloggt: sub05 ist ohne Verzweigung (scd_dump_room.py), die Flanke von (1,27)/(2,7)
    meldet main.c:5797-5816 nach den 15 Abblend-Takten als `[scd F242] letterbox closed -> gameplay (player_mode=0)`,
    also liefen Set(3,235) und die beiden Freigaben im VM-Lauf von F228. **Spieler frei**: pm 0 ab F242 (Zustandslog),
    danach die freien Steh-Varianten mo 200 -> 210 (F343) -> 211 (F359) -> 212 (F448) wie im freien 2030 (F119/F135/
    F303), Bilder F260/F700 Cut 6 ohne Balken. (Die Wasserwand EM024 ist darin noch die Port-Fassung — Spur A.)
  - Der echte Haken kommt mit Spur A (Zweig r41/wasser20b0: aot_common.c `&& !re15_wasser20b0_r41_auto_aot(
    g_current_room_id, a->type)`, AUTO-Pass FUN_800436a8 @0x8001ce1c ohne Szenen-Gate) bei der Integration; in diesem
    Baum NICHT uebernommen. integration_r41_ventilkette A prueft den Szenenverlauf nicht und bleibt mit dem Haken
    gueltig; die Bits 103 + 100 setzt schon der Raumwechsel (Installer), nicht die Szene.
* **Geflutetes 2030 ueber den Ring betretbar** (NEU 03:03, §4 "Eintritt in das geflutete 2030"): nach der Flut (103
  ohne 107) fuehren 20B0 Slot 3 @0x019C2 und 2070 Slots 1/2 @0x00A5A/@0x00A7A auf den Ring (y -5400); die Flaeche steht
  1800 darueber (y -7200, PORT-WAHL §3.6), der Spieler steht und laeuft im Wasser, die Spinnen greifen an; ertrunken
  wird nur waehrend der Flut (900 ueber den Fuessen, s_flut). Der Wortlaut ("Wasser in 2030: Ueber die Ringebene",
  "spaeter wieder frei ohne Wasser") ist erfuellt, er legt aber nicht fest, was der Ring-Eintritt in den gefluteten
  Raum tut. Innerhalb der Zuteilung versucht: (1) Original-Text als Sperrsatz — 20B0 hat keine Texttabelle (msg_sec 0),
  2070 nur msg 0 "You've opened the lock." (rdt_msgdump.py); (2) ein Tuer-Riegel ohne eigenen Text —
  re15_aot_door_params_t hat kein Schloss-Feld, die Port-Sperren (tuersperre_r39.c, sperre1050_r37.c) nutzen je Raum
  eine Port-Id; (3) Ertrinken auch nach der Flut — ein Sofort-Tod hinter einer Tuer ohne Warnung. Keins davon ist
  belegt oder vertraglich gedeckt -> nicht gebaut, Entscheidungsfrage §7 (f) mit Vorschlag.
* PSX-Ziel: die Wasserflaechen-Daten (gen/ventilkette_r41_wasser.inc = ROOM2080 MD1 @0x000D98 4572 B + TIM @0x026408
  16928 B) sind nur unter RE15_PLATFORM_PC eingebunden (ventilkette_r41.c, PSX-Laufzeitbudget); Bits, Zaehler, Flut-Takt
  und Tod sind plattformneutral. Der PSX-Bau ist in diesem Baum weder gebaut noch gemessen (keine PSn00bSDK-Kette).
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
  **(f) NEU (2026-10-24, §6):** Nach der Flut fuehren die 20B0-Ost-Tuer (Slot 3) und 2070 (Slots 1/2) auf den
  gefluteten Ring von 2030; Leon steht dort bis zur Brust im Wasser (Bild C_belege/ring_nach_flut_sheet.png). Vorschlag:
  diese Tueren sperren, bis der Rechner 2080 2030 entleert hat (Bit 107), mit einem Satz in der Form der Leiter-Vorgabe,
  z. B. "I cannot open the door because of the water pressure." (Mechanik wie die Leiter: Sperrtext-Satz sce 1, Form
  ROOM2030 @0x01B12). Braucht Port-Id 41 in 20B0/20B1 und Id 40 in 2070/2071 — VERTRAG §1.2 teilt C dort nur 20B0 Id 40
  zu, deshalb nicht gebaut. Der Weg zum Rechner bleibt offen (2050 -> 2020 -> 2030 Tuerebene -> 2080, ueber dem Wasser).
  Alternative: so lassen (Waten in Brusthoehe).
* Sichtprobe fuer den Nutzer: 2030 am Ring vor den Ventilen die Aktionstaste, Ja, Warnung wegdruecken, 60 s stehen
  bleiben -> Draufsicht, Wasser steigt ueber die Bruecke und den Ring, Spritzer, Tod mit blauem Schleier
  (C_belege/flut_echt_sheet.png). Mit den drei Spinnen im Raum (ohne Kill-Bits) stirbt man vorher an den Spinnen;
  im Spiel also erst die Spinnen erledigen oder den Raum durch die Ost-Tuer verlassen (-> 20B0).
* Nach der Flut: in 20B0 laeuft die Auswasch-Szene des Originals erst mit dem Haken der Spur A bis zur Freigabe (§6
  Querbezug A); ohne ihn bleibt Leon im Westgang (Cut 14) stehen. Zum Rechner 2080 geht es ueber 2050 -> 2020 -> die
  Tuerebene von 2030 (ueber dem Wasser) -> 2080; der Rechner senkt 2080 und leert 2030 (Bild C_belege/rechner2080_sheet.png,
  raum2030_leer_vergleich.png).
* Neue Belegbilder dieser Fortsetzung: C_belege/querbezug_a_20b0_szene_sheet.png (Szene mit dem Haken der Spur A),
  C_belege/ring_nach_flut_sheet.png (Frage f).
