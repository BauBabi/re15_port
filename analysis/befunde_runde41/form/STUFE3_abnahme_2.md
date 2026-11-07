# Stufe 3 Bahnsteig-Form - Unabhaengige Abnahme 2 (Delta nach Abnahme 1)

Pruefer: unabhaengig (nicht der Autor), 2026-11-07. Gegenstand: alle Code-/Asset-Commits auf r41/form nach Abnahme 1
(d8eab158b, 2026-11-02 17:26): a452893f1, fbef4bcc7 (Nachbesserung 1), 11427ea54, 67408cfcb (Nachbesserung 2), 20998731f,
8ae5263fb, 56050f737 (Nachbesserung 3). Arbeitsbaum .claude/worktrees/r41_form, Belege abn2/ (Bilder abn2/bilder/, Zeitleisten,
Helligkeit, Ton, Disassemblierung, Pins, Werkzeug, Umgebung je Lauf).
Bindend: NUTZER_ENTSCHEIDE.md E1-E28; Nutzerentscheid E23 (2026-11-03, "passt erst einmal") und 2026-11-07 "K4 bleibt fuer
v0.8.30": das Aussehen von K4 ist NICHT Gegenstand dieser Abnahme, nur dass das Spiel-Asset das abgenommene K4/NB2 ist und nichts
kaputt rendert.

## 0. Urteil

**Urteil: NICHT BESTANDEN - ein blockierender Mangel (M-A).** Die Rueckstoss-Szene der Riesenform in 5090/5091 schaltet fest auf
Cut 12 (birkin_formen.c:484 und :541, bis die Form bei x <= 3000 ist) und prueft dabei nicht, ob Form oder Spieler direkt vor dem
Auge von Cut 12 (x 18526) stehen. Im echten Wagenweg (erst durch Nachbesserung 3 kampffaehig) fuellt dadurch zu Szenenbeginn
entweder die Form 16 Bilder lang fast das ganze Bild (Leon und Elza, Standardweg) oder Ruecken und Kopf des Spielers 7,5 s lang
40-70 % des Bildes, waehrend das Taumeln dahinter verschwindet (Leon und Elza, nach 2 s Zurueckweichen). Das ist der Mangel M4
(Kamera-Nahsicht) der Abnahme 1 in der von Nachbesserung 1 neu gebauten Szene: beauftragter Punkt im echten Spielweg kaputt.

Alles uebrige des Deltas ist gemessen bestaetigt (Abschnitt 2): Spiel-Asset = K4/NB2 (Pins stimmen), HP in Wagen C 150 fuer Leon
und Elza im echten Weg, START im Griff verworfen (Leon und Elza; Gegenprobe: START nach dem Loslassen oeffnet den Schirm), Sturz
sichtbar, Taumeln statt Gleiten, Kampfstart in Cut 11 ohne Cut-15-Bild, Gebruell hoerbar (Raum-Ton 10), Biss am Kiefer, Wandklemme
19 Bilder statt 8 s, 5080/5081 mit Flucht und Raumtext; kein Absturz, kein Haenger; alle zitierten Adressen selbst disassembliert.
Riegel: 5/5 Pflicht, 25/25 Birkin/G5/Plattform, 86/86 Menue/Inventar/START gruen.

| Schritt | Stand |
|---|---|
| Delta, Zweigkopf, exe | 7 Code-Commits wie beauftragt; nach 56050f737 nur analysis/ (Abschnitt 1) |
| exe | re15_pc.exe 2026-11-03 06:19:27, sha256 f4deb1a3...; ninja -n "no work to do" = Zweigkopf fuer re15_port; Messkopie scratchpad w2abn/exe |
| Belege (Adressen, E-Nummern, Pins) | erledigt (Abschnitt 2, abn2/disasm_pruefung.txt, abn2/pins.txt); drei Doku-Hinweise (M-D) |
| Spiel 5090/5091 Wagenweg, Leon UND Elza | erledigt, 5 Laeufe + 1 bildgenauer Lauf (Abschnitt 3.1) |
| Spiel 5090/5091 Angriffe/Griff/START, Leon UND Elza | erledigt, 6 Laeufe (Abschnitt 3.2) |
| Spiel 5080/5081, Leon UND Elza | erledigt, 3 Laeufe (Abschnitt 3.3) |
| Riegel | Pflicht 5/5, Zusatz 25/25 und 86/86 gruen (Abschnitt 4) |

## 1. Delta, Zweig, exe

- Letzte Abnahme: STUFE3_abnahme_1.md, d8eab158b 2026-11-02 17:26:37. Code/Asset danach (git log d8eab158b..HEAD -- re15_port release):
  a452893f1 (20:34, K3-Asset, Clips 12/13), fbef4bcc7 (20:37, Tick/Rueckstoss/Kamera/Ton/Koerper/Biss), 11427ea54 (01:27, START im Griff),
  67408cfcb (03:20, Asset K4 + Textur NB2, Biss-Probe), 20998731f (06:00, Eintritt bei jeder Anlage, HP 150), 8ae5263fb (06:00, Riegel
  Wagenweg), 56050f737 (06:22, Riegel Wagenweg fuer hoehere HP). Das ist genau die beauftragte Liste.
- Danach bis zum Pruefende 42 Commits (Kopf ccdecce8f), keine Merges, alle nur unter analysis/ (Farbvarianten K5):
  git diff 56050f737..HEAD -- re15_port release ist leer, git diff --name-only 56050f737..HEAD ohne Pfad ausserhalb analysis/,
  git status re15_port release leer. Die Farbvarianten haben Spiel-Asset und Pins nicht angefasst.
- exe: re15_port/build/platform/pc/re15_pc.exe 2026-11-03 06:19:27 (Objekte 06:19:24-26, Kopf re15_birkin_formen.h 06:19:23 = Ruecknahme
  des 1000-HP-Probelaufs auf 150, Dossier NB3-5). 56050f737 (06:22:40) aendert nur tests/integration/test_r41_form_wagenweg.cmake, das ctest
  zur Laufzeit liest. ninja -n -d explain im Bauverzeichnis (Trockenlauf): "no work to do", exe und alle Testprogramme sind aktuell zum
  Baum (= HEAD). **exe = Stand des Zweigkopfs fuer re15_port.** Kopie: scratchpad w2abn/exe/re15_pc_w2abn.exe, sha256 f4deb1a33feb4d59...;
  jeder Lauf protokolliert die sha256 seiner Kopie (alle f4deb1a3). Bau-exe vor und nach den ctest-Laeufen unveraendert. Nicht gebaut.

## 2. Belege je Punkt

| Punkt (Commit) | Soll | Ist (gemessen) | Bewertung |
|---|---|---|---|
| Spiel-Asset K4/NB2 (67408cfcb) | RE2/BAHNSTEIG_FORM.EMD/.TIM = abgenommenes K4 mit Textur NB2 | Blobs 1fba8a34 / 4e7db099 = HEAD = 917195879 (Stand der Nutzerzusage E23) = stufe3/asset; MD1 und TIM bytegleich stufe3/basis_n2. sha256 EMD 16eb6ea8..., TIM ec280802... | bestaetigt |
| Pins (tests/unit/form_j_posen.h) | MD1 167644 Byte CRC32 0xc0b6c82a, TIM 362528 Byte 0x76775364 | selbst nachgerechnet aus der Datei: 167644 / 0xc0b6c82a, 362528 / 0x76775364 | bestaetigt |
| Release-Pin | Paket-Gate | release/make_package.sh:294-297 prueft vorhanden, nicht leer, bytegleich Quellbaum/Paket; einen sha256-Pin der Datei gibt es unter release/ nicht (git grep der sha256 ohne Treffer) | in Ordnung (Gate wie Abnahme 1) |
| NB3 Eintritt bei neuer Anlage (20998731f) | HP in Wagen C = 150 im echten Weg | eigene Laeufe ohne Messhaken, Leon und Elza: A 300 (EM30-INIT), B F498 150, C F946 150; Kampf 10 Treffer F1237-F1464, Rueckstoss F1486 | bestaetigt |
| NB3 Adressen | Sce_em_set +0x4 = 0, G4-Ctor-Verteilung, HP 1000 | sw zero,4(s0) @0x800421e0 (Disc und Mod); lbu v0,4(s5) @0x80100314, lw v0,-0x50a0(at) @0x80100328, jalr @0x80100330, Tabelle @0x8010af60 = 0x8010062c/0x80100bbc/0x80106d3c/0x801076ec; addiu v0,zero,1 @0x80100638 + sw v0,4(s1) @0x80100654; addiu v0,zero,0x3e8 @0x80100678 + sh v0,0x156(s1) @0x8010067c; beq @0x8001d968, jal 0x800396fc @0x8001d988, sw zero,(at) @0x8001a4e8; Mutation ori v0,zero,0x96 / sh v0,0x9a(v1) @0x80118094-98 STAGE3 | alle Bytes wie zitiert |
| NB2 START im Griff (11427ea54) | START im Griff verworfen, keine Vormerkung | Leon (RE15_PAD_AT 726/735/745/755) und Elza (719/728/738/748), alle im Griff (hk=6): ms=0 in jedem Bild, Zeitleiste bildgleich zum Lauf ohne START. Gegenprobe START F770 nach dem Loslassen (F759): Schirm offen (ms=2) | bestaetigt |
| NB2 Adressen | aca3c & 0x40 springt ueber den START-Block; Haken setzt/loescht das Bit | lui a1,0x800b / addiu a1,-0x35c4 (= 0x800aca3c) @0x8001ccf4-f8, andi v0,a0,0x40 @0x8001cd04, bnez -> 0x8001cdec @0x8001cd08 (Disc = Mod); STAGE5 ori 0x40 / sw @0x8011b0b4/c0, @0x8011b4a0/ac, @0x8011b770/7c, addiu v1,zero,-0x41 / sw @0x8011b3e0/ec, @0x8011b6b8/c4, @0x8011b95c/68 (Disc); RE2 s3 = 0x800cc1e8 @0x800257cc-d0, lbu v0,0x3be3(s3) = 0x800cfdcb @0x80026438, bnez -> 0x80026540 @0x80026440 | alle Bytes wie zitiert |
| NB1 M1 Sturz sichtbar (fbef4bcc7) | Sturz beginnt im Wagen, in Cut 11 zu sehen | Sturz Clip 12 ab F1749 bei x -2538 (Leon) / -2521 (Elza); Cut 11 zeigt das Kippen durch die Tuer F1750-F1774 (bilder/w5090_leon_rueckstoss.png, w5091_elza_ablauf.png) | bestaetigt |
| NB1 M5 Taumeln | Clip 13, Vortrieb aus der Spur | Clip 13 ab F1486, x 17290 -> -2538 in 263 Bildern (rund 75 je Bild), Fuesse laufen mit | bestaetigt |
| NB1 M4a Kampfstart | Auftritt in Cut 11 statt Cut 15 | state.log meldet Cut 15 nur in F1169; bildgenau (jedes Bild): F1168-F1170 zeigen das gehaltene Cut-12-Bild (Mittelwert 56,2 identisch), F1171 Cut 11, kein Cut-15-Bild sichtbar (bilder/w5090_leon_schnitt_f1166.png) | bestaetigt |
| NB1 M4b Nahsicht-Wache | Gegenkamera, wenn die Form Cut 12/13 fuellt | Wache schaltet (F845 Cut 12, Leon vorn, Form dahinter); Rest Cut 13 wie im Dossier N11 offen (M-B) | teilweise, Rest bekannt |
| NB1 M4/M5 Kamera der Rueckstoss-Szene | Szene zeigt das Taumeln | **nein**, M-A | **Mangel** |
| NB1 M3 Gebruell | hoerbare Ids, Raum-Ton 10 | Raum-Ton 10 gespielt: 5090 3 bis 5, 5080 6; re2se SILENT nur se=11 (Gang-Clipende @0x80102a40, Bank 24 Id 11 = FFFFFFFF, auch in RE2 stumm); Bank 24 Ids 9-15 = FFFFFFFF selbst gelesen (ENEMSE.VBS 0x1d8000); unter Dauerfeuer verwirft das SE-Tor FUN_80045a18 einzelne Gebruelle (M-C) | bestaetigt, Hinweis |
| NB1 M6 Wand | kein Laufen auf der Stelle | 5080/5081: laengste Standphase im Lauf-Clip 5 = 19 Bilder (F1053-F1072), Abnahme 1: 8 s | bestaetigt |
| NB1 M7 Koerper | Spieler nicht in der Form | kleinster Abstand Spieler-Wurzel ohne Griff 1231 (5080 F1063, Weichen; Leon steht neben dem Hals, bilder/b5080_leon_nah_f1048.png), 5090 1417; kein Unter-dem-Bauch-Bild mehr | bestaetigt |
| NB1 D Biss am Kiefer | Treffer nur mit Kieferprobe am Spieler | unit_birkin_bahnsteig D: 2600-3000 kein Treffer (Kiefer 1384-1390, alter Brustpunkt 54-60), 3200-6000 Treffer (Kiefer <= 1390); Spiel 5080: Blut am Kopf beim Biss (bilder/b5080_leon_ablauf.png F1186) | bestaetigt |
| NB1 M8a Blut | Anker Kopf | Spiel 5080/5081: Blutspritzer am Kopf/Maul beim Biss (F1078, F1186) | bestaetigt |
| NB1 Adressen | Segmente, Schatten, Biss, Lenken, Ton, Flag | sw v0,0x1e8(s1) mit 2 @0x80100708/14; x 0x190 @0x80100718/1c; 0x44c @0x8010076c, 0x7d0 @0x80100770/@0x801007a4, 0x3e8 @0x80100780/84, x 0 @0x80100798; Schatten 0x8fc @0x801002cc, 0x514 @0x80100300; jal 0x80034d0c @0x8010052c; Biss sltiu 0xce4 @0x8010243c, sw 0xe01 @0x80102464; Probe 0x5dc / 0x578 / +0x700 @0x80104f88-a8; Kegel 0x338 + Rate 0x20 @0x801018ec-908; Rate 0x20 @0x80104204; a3 0x10080 @0x8010427c-84; Raum-Ton jal 0x800453d0 / ori a0,zero,0xa @0x8011af58-5c bei Bild 0x3c; Flag a0 0x800b1028 a1 0x1c @0x8011aea4-cc; 0x30-Kasten {0,-1440,0,1000,1440,1000} @0x8011ee64 STAGE3 | alle Bytes wie zitiert |
| Nutzerentscheide | E-Nummern woertlich | E19-E22 in NUTZER_ENTSCHEIDE.md woertlich wie im Memory; E23 und "K4 bleibt fuer v0.8.30" fehlen dort (M-D) | Hinweis |
| PORT-WAHL-Werte | jede neue Konstante mit @0x oder als Gestaltung markiert | alle neuen Werte im Delta tragen @0x oder stehen in einem als PORT-WAHL/Gestaltung markierten Block | in Ordnung |

## 3. Spiel (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT/SOFTWARE_RENDER, Bilder per RE15_FRAMEDUMP-Serie)

### 3.1 Zug 5090/5091, echter Wagenweg A -> B -> C

Eingabe: Raumladung 5090/5091 am 6030-Eintrittspunkt (27200,4900), Flag (3,89), Tastenskript w8 der AN-Abnahme 2 (wie
integration_r41_form_wagenweg), danach Zielen + Feuer gehalten; KEIN Messhaken (echte Spieler-HP). Zeitleisten abn2/zeitleisten/.

| Lauf | Ergebnis |
|---|---|
| w5090_leon (bis F2700) | HP A/B/C 300/150/150; Kampfstart F1169; Rennen trifft Leon F1322 (100 -> 95); Biss F1377/F1446 verfehlt; 10 Treffer; Rueckstoss F1486 (Form x 17290); Cut 11 ab F1679; Sturz F1749; G5 F1850, G5 nimmt Schaden (600 -> 180 bis F2700); Ende per EXIT_AT, exit 0 |
| w5091_elza (bis F2700) | bildgleich (Rueckstoss F1486 Form x 17313, Sturz F1749, G5 F1850) |
| w5090_leon_dicht (bis F1530, jedes Bild) | Kampfstart ohne Cut-15-Bild (s. 2); **F1487-F1502: 16 Bilder mit 77-100 % Bildpunkten < 24, Mittel 14-22** (Form rund 1240 vor dem Auge von Cut 12); Leon wieder sichtbar ab F1503 |
| v5090_leon (Leon weicht nach dem Kampfstart 2 s zurueck, dann Feuer) | Rueckstoss F1544, Form x 20124 (hinter dem Auge von Cut 12), Leon x 17815 = 711 vor dem Auge: **F1546-F1771 (7,5 s) fuellt Leons Ruecken 40-70 % des Bildes, das Taumeln ist dahinter kaum zu sehen**; Cut 11 erst ab F1772 (x <= 3000); Sturz F1841, G5 F1942 |
| v5091_elza (dasselbe) | Rueckstoss F1544, Elza x 17665: Elzas Kopf und Ruecken 7,5 s gross im Bild, Form dahinter; Sturz F1843, G5 F1944 |
| k5090_leon (AN-Weg mit 24 s Warten, ohne Messhaken) | Form greift den stehenden Leon an (Rennen, Biss, Griff F1466 95 -> 50), Leon stirbt F1932, als das Feuer spaet beginnt: Spielerverhalten, kein Mangel; Bilder nach dem Tod vom Neustart ueberschrieben, nicht verwendet |

Bilder: w5090_leon_kampf.png, w5090_leon_rueckstoss.png, w5090_leon_rs_start_dicht.png, w5090_leon_schnitt_f1166.png,
w5090_leon_g5.png, w5091_elza_ablauf.png, v5090_leon_rs_cut12.png, v5091_elza_rs_cut12.png. Helligkeit je Bild: abn2/hell/.

### 3.2 Zug 5090/5091 ab Wagen B (Debug-Sprung), Angriffe ohne Feuer, Griff, START

| Lauf | Ergebnis |
|---|---|
| a5090_leon | Auftritt Cut 11 F282-F340 (Krokodilkopf, Feuer); Biss aus 5032 Wurzelabstand F504 (100 -> 90), drei weitere Bisse; Griff F718-F759 (60 -> 25, Wurf nach x 14932); Wache Cut 12 F845; Leon stirbt F1132 (stehender Spieler, erwartet); Cut 13 F1051-F1174: Form deckt 54-70 % (M-B) |
| a5091_elza | bildgleich (Griff F711-F751, Tod F1126) |
| s5090_leon / s5091_elza | wie a5090/a5091 + START viermal im Griff: ms=0 in jedem Bild, Zeitleiste bildgleich ohne START |
| g5090_leon | START F770 nach dem Loslassen: Statusschirm ms=2 ab F770 (Gegenprobe) |
| a5090_leon_devicelost | erster Versuch: SDL-D3D DEVICELOST ab Laufbeginn (Umgebung, kein Spielfehler; Zeitleiste bildgleich mit dem Wiederholungslauf); nach Leons Tod startet das Spiel neu, EXIT_AT im Raum 5090 nie erreicht, eigenes timeout beendete den eigenen Kindprozess |

Bilder: a5090_leon_auftritt_biss.png, a5090_leon_griff_wache.png, s5090_leon_start_im_griff.png, g5090_leon_start_nach_griff.png.

### 3.3 Bahnsteig 5080/5081 (Debug-Sprung 5080/5081@120, Eingabe wie N1 l5080_leon/e5081, Feuer)

| Lauf | Ergebnis |
|---|---|
| b5080_leon | Fall F836, Auftritt F851 HP 150, Kampf Cut 4 mit Krokodilkopf; Rennen trifft (95), Bisse mit Blut am Kopf, Zucken; Griff F1246-F1338 (95 -> 50); HP 0 F1348 -> Flucht F1370 (zurueck, Drehen, Clip 11 ab F1465), Form weg F1521; Raumtext msg 3 "The power already has been supplied to the train!" ab F1522; exit 0 |
| b5081_elza | bildgleich bis auf Elzas Laufweg (Flucht F1370, msg 3) |
| t5080_leon | wie b5080 mit SDL_AUDIODRIVER=dummy (WASAPI-Endpunkt ab ~15:00 weg; Ablauf bildgleich): Raum-Ton 10 im Auftritt 4x, im Griff 2x, Flucht-Gebruell F~1470 vom SE-Tor verworfen (Leon feuert) |

Bilder: b5080_leon_ablauf.png, b5080_leon_flucht.png, b5081_elza_ablauf.png, b5080_leon_nah_f1048.png.
Gesehen: keine kaputten Texturen, keine Loecher, kein Haenger, kein Absturz in 15 Laeufen.

## 4. Riegel (ctest aus Git-Bash, PATH mit msys64 zuerst, SDL_ASSERT=always_ignore, --test-dir r41_form/re15_port/build)

- Pflicht, -j 3: **5/5 gruen** - integration_r41_form_wagenweg (95,5 s: Soll 150, B 150, C 150 F946, 10 Treffer, Rueckstoss F1486,
  Leon 1x getroffen), unit_r41_form_emd, unit_birkin_bahnsteig (u. a. D, Flucht 5080/5081 Drehen 56 Bilder max 32, Schwanz >= 2721),
  integration_r34_plattform_takt (23 s), integration_r34_plattform_esp_eintritt (9,7 s).
- Zusatz Birkin/G5/Plattform, -j 3: **25/25 gruen** (440 s), darunter integration_r41_birkin_pistole (243 s), integration_r41_g5_tod,
  integration_r41_g5_westen, integration_r35_zgirl_5090, integration_r37_elzaems_leon5080, unit_n2_griff_start (5 Faelle laufen echt),
  unit_r41_birkin_wand_griff/_g5/_rein, unit_birkin_szenen/_formen/_opfer/_ai, unit_g5_*, Birkin-Pins r17/r18/r20/r30.
- Zusatz Menue/Inventar/START (menu_common.c gehoert zum Delta), -R "inv_|inventar|menu|kleinkram|hinweis|r39_status|r38_griff|opfer|
  r30_android|statusschirm|start", -j 3: **86/86 gruen**.
- Kein roter Riegel, nichts nachzufahren. Logs: abn2/ctest_set_a.txt, ctest_set_b.txt, ctest_set_c.txt.

## 5. Maengel und Hinweise

| Nr | Was | Beleg | Schwere | Vorschlag |
|---|---|---|---|---|
| M-A | **Kamera der Rueckstoss-Szene im echten Wagenweg.** re15_bf_rueckstoss_start setzt Cut 12 (birkin_formen.c:484), bf_rs_riesenform_phase1 haelt Cut 12 bis x <= 3000 (:541); die Nahsicht-Wache ist in der Szene aus (s_rs.slot < 0, :417). Cut 12 hat sein Auge bei x 18526 (Blick -x). (1) Standardweg w8 + Dauerfeuer: Form bei Szenenbeginn x 17290 (Leon) / 17313 (Elza), rund 1240 vor dem Auge -> 16 Bilder F1487-F1502 fast schwarz (77-100 % Bildpunkte < 24). (2) Spieler weicht vorher 2 s zurueck: Form x 20124 hinter dem Auge, Leon x 17815 (711 vor dem Auge) bzw. Elza x 17665 -> 7,5 s (F1546-F1771) nur Ruecken/Kopf des Spielers gross, das Taumeln dahinter verdeckt. Die Messungen der Nachbesserung 1 liefen nur im Debug-Sprung-Weg (Form bei Szenenbeginn x 5929, weit vor dem Auge). | bilder/w5090_leon_rs_start_dicht.png, w5091_elza_ablauf.png F1492/F1498, v5090_leon_rs_cut12.png, v5091_elza_rs_cut12.png; hell/w5090_leon_dicht.txt; zeitleisten w5090/w5091/v5090/v5091 | **hoch (blockierend)**: Hoehepunkt der Szene im normalen Spielweg nicht zu sehen; M4 im neu gebauten Teil | Szenenkamera nach Lage waehlen wie bf_nahsicht_wache (Form und Spieler je Gangkamera gegen das Auge pruefen; in beiden gemessenen Faellen stuenden sie in Cut 13, Auge x 11050 Blick +x, weit genug vor dem Auge: Spieler 3948 / 6765, Form 6240 / 9074), Cut 11 ab x <= 3000 bleibt; Riegel im echten Wagenweg mit beiden Lagen (Standardweg, 2 s zurueck), Leon und Elza, Pruefmass Bildanteil der Naehe (kein Szenenbild mit > 60 % Bildpunkten < 24, kein Spielerkoerper ueber einem Drittel des Bildes) |
| M-B | Cut 13 Nahsicht-Rest (Dossier N11, bekannt): steht der Spieler nach dem Wurf bei x 17218-17714 (1300 / 800 vor dem Auge von Cut 12), schaltet die Wache nicht; Cut 13 zeigt die Form 4 s lang mit 54-70 % dunklem Bildanteil, Leon zeitweise verdeckt | a5090_leon F1051-F1174 (hell/a5090_leon.txt), bilder/a5090_leon_griff_wache.png | mittel (nicht blockierend: offen dokumentiert, Abnahme 1 sah 8 s mit Leon verdeckt) | mit M-A zusammen loesen (dieselbe Lagepruefung) |
| M-C | Gebruell faellt unter Dauerfeuer dem SE-Tor FUN_80045a18 zum Opfer (Pistole prio 3 belegt SE-Stimme 6, Raum-Ton 10 prio 2): 5090 Leon/Elza 2 von 5 (Auftritt Bild ~81, Sturz F~1751), 5080 das Flucht-Gebruell; ohne gleichzeitigen Schuss alle 5 (v5090_leon) | abn2/ton.txt | niedrig (Hinweis; das Tor ist RE1.5-Verhalten und folgt aus der PORT-WAHL Raum-Ton 10) | dem Nutzer vorspielen; falls stoerend, den Ton der Szene auf eine eigene Stimme legen |
| M-D | Doku: (a) E23 ("passt erst einmal", Pose Bild 2 nicht drehen) und der Entscheid 2026-11-07 "K4 bleibt fuer v0.8.30" fehlen in NUTZER_ENTSCHEIDE.md (die Datei ist laut Dossier bindend); (b) enemy_ai_common.c:4552 schreibt "drei Segmente ... K3", der Code hat BB_SEG_N 2 (birkin_bahnsteig.c:732) und das Asset K4; (c) Spieler-Ctor @0x8003bdc0-ec (BB_PL_R/Y/H, 450/-1530/1530) sind Adressen der RE2-EXE (dort addiu 0x1c2 / -0x5fa / 0x5fa), der Kommentar nennt die EXE nicht; in der RE1.5-EXE steht dort anderer Code | NUTZER_ENTSCHEIDE.md, Quelltext, abn2/disasm_pruefung.txt | niedrig | nachtragen |

Nicht als Mangel gewertet: Aussehen K4 (Nutzerentscheid); se=11 am Ende des Gang-Clips stumm (G4 @0x80102a40, Bank 24 Id 11 = FFFFFFFF,
auch in RE2 stumm); Leons Tod im AN-Weg mit 24 s Stillstand; Umgebungsfehler DEVICELOST/WASAPI (Wiederholung bildgleich).

## 6. Messweg und Umgebung

- Eigene exe-Kopie je Lauf (scratchpad w2abn/exe, Name re15_pc_w2abn_<lauf>_<pid>), eigenes Laufverzeichnis, Fenster 640x480
  (RE15_WINDOW_SCALE=2), RE15_FRAMEDUMP-Serie (Bild vor dem Present zurueckgelesen), Ende per RE15_EXIT_AT, Notbremse timeout nur auf
  den eigenen Kindprozess; nie taskkill /IM. RE15_RE2_ASSET_ROOT zeigte auf eine Kopie der HEAD-Blobs von BAHNSTEIG_FORM.EMD/.TIM
  (gleiche sha256 wie der Baum), damit ein Tausch im Arbeitsbaum durch den parallelen Agenten die Laeufe nicht beruehrt.
- Werkzeug abn2/werkzeug/: lauf.sh, zustand.py (Zeitleiste), blatt.py (Kontaktblatt), hell.py (Bildhelligkeit), klemme.py (M6/M7),
  mipsabn.py (Disassembler), ausschnitt.py, ctest_*.sh. Umgebung je Lauf abn2/env/. Die Bilder unter abn2/bilder/ sind trotz der
  Ordnerregel *.png per git add -f versioniert (nur diese Belege).
