# Abnahme 4 der Spur G5A (G5-Auftritt im Zug, ROOM5090 Leon / 5091 Elza) - unabhaengig

Zweig `r41/g5auftritt`, Kopf `6cce9fcf2` (Code-Stand `d6e0e919d` = Nachbesserung 4, danach nur Dossier/Belege). Pruefer ist nicht der Autor.
Massstab laut Nutzer: "nach dem raus schiessen von Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei Resident Evil 2".
Belege: `G5_belege/abnahme4/` (Leisten, `messung_*.txt`, Werkzeuge `abnahme4/werkzeug/`). Alle Zahlen unten selbst gemessen, mit eigenem
RE2-RAM-Leser (`re2ram4.py`, zstd ab der Magie, RAM-Basis ueber SLUS_007.48[0x800:0x840]) und eigenem MIPS-Disassembler (`mipsdis4.py`,
`exedis4.py`, capstone). Dossier Abschnitt 10 und Abnahmen 1-3 nur als Hinweis gelesen; die Riegel-Tabelle des Autors wurde nicht benutzt.

## Urteil: BESTANDEN

N1 (Arme in [T7] nicht eingezogen) und N2 (Armstummel Bild 313/320) der Abnahme 3 sind behoben - im Motor (Armzustaende bitgleich zum
RE2-RAM an allen 18 Savestates s03..s20, an s21..s26 bis auf den Zaehler +0x158) und im Bild (echte exe, Framedump, Leon und Elza gleich).
Die fuenf Durchfaelle sind gegen das RE2-Overlay selbst nachdisassembliert und richtig. Bild fuer Bild gegen die echte RE2-Aufnahme: keine neue
sichtbare Abweichung; wo NB4 das Bild aendert (nur Arm-Bilder), naehert es sich RE2. Kampf fair und gewinnbar (Pistolentest Leon/Elza: G5 tot,
Spieler-HP 45). Die Testanpassung in `unit_r41_birkin_wand_g5` laesst den Zweck des Riegels unberuehrt. M4 bleibt offen, ist aber kein
Blocker dieser Spur (Begruendung 5). Nichts davon ist "fertig wie RE2" im Sinne des ganzen Raums: M3/M4/M5/M6 bleiben offene, benannte
Abweichungen (Tabelle 7).

## 1. Bau, Laeufe

- Bau: `bash re15_port/tools/local_build.sh configure` + `build` im Worktree, Baum `re15_port/build`: LOCAL-BUILD-OK, "ninja: no work to do";
  keine Quelle unter engine/platform/include/tests neuer als `re15_pc.exe` (04:04:30; juengste Quelle enemy_ai_tentakel_g5.c 03:54:08),
  `git status re15_port` sauber, Symbol `re15_g5_tentakel_kette_strecken` in der exe - also Stand `d6e0e919d`.
- Fensterlaeufe (`werkzeug/lauf4.sh`): eigene exe-Kopie `re15_g5abn4_<name>.exe`, kein AUTOSHOT, kein SOFTWARE_RENDER, Ton an, Eingabe bis
  zur Szene wie `integration_r41_birkin_pistole` (Debug-Sprung, Pistole, Zielen + Feuer gehalten).
  - `fd_leon` / `fd_elza`: **RE15_FRAMEDUMP 1540-2900/1** (fertig komponierter Rueckpuffer vor dem Present, 960x720), je 1361 Bilder.
    Protokoll beider: `F1558 Vorlauf, Schnitt 16`, `F1613 Armierung nach 55 Vorlaufbildern`, `F2775 Szenenende Bild 1162: G5 Sub 1 Ph 1,
    Spieler x 13873, Cut 12` - Szenenbild n = Spielbild F1613 + n.
  - `steh_leon` / `steh_elza`: Zielen + Feuer gehalten bis F4400 (Kampf). `schub_leon`: Framedump F2996-3024 (Kampf, 4.2).
  - `pw_leon` / `pw_elza`: das Fenster der eigenen exe-Kopie per PrintWindow abgegriffen (Abschnitt 3.6).
- **Ablauftest = echte exe:** Messschiene `RE15_G5A_SPUR` des Ablauftests `unit_r41_g5auftritt` (Engine-Diagnosen je Bild) gegen das
  `state.log` der echten exe (`exe_spur4.py`, `messung_exe_gegen_spur.txt`): G5-Lage und Arm-Gier in **allen 1163 Szenenbildern gleich**,
  5090 und 5091. Die Armzustaende aus der Schiene gelten damit fuer den Fensterlauf.
- RE2-Referenz: Mitschnitt Lauf 1 der Abnahme 1 (640x480, 59,817 B/s) und Savestates s01..s26 (Lauf 2). Zeitachse selbst nachgemessen
  (`re2_kanten4.py`): CD-Standbilder 67,809..68,261 / 78,545..79,649 / 92,040..92,776 / 104,599..104,883 s (= Abnahme 3). Segmentanker wie
  Abnahme 3 (Bild 0 = 63,47 s). Restversatz der RE2-Zeitachse selbst bestimmt: Kreuzkorrelation Armsignal Schnitt 17 (Bild 440..806)
  **-2,0 Spielbilder** (r 0,897, `arme_kurve.txt`), Schnitt 16 Segment 1 -1,5 (r 0,997, `arme_kurve_s1.txt`), Standbildbeginn 67,809 s
  gegen Bild 132 -3,3. Ein RE2-Bild "n" zeigt also RE2-Szenenbild n+1,5..n+3,3; Vergleiche auf +-2 Bilder sind innerhalb der Messgenauigkeit.

## 2. N1 / N2 - eigene Messung

### 2.1 Disassembly (selbst, `build/extracted/re2_ems/CDEMD0_EM3x_ai0.BIN`, Datei-Offset = Adresse - 0x80100000)

- EM36 [T7] @0x801015b8-0x80101608: `sh zero,0x10(v0)` @0x801015c8 (Gewicht 1 = 0), `sw v1,0x14c(s0)` @0x801015d0 (0x1F0002), `sb v0,6(s0)`
  @0x801015d4 (Ph 8), **`lw a2,0x3c38(s3)` @0x801015d8, `addiu v0,zero,9` @0x801015dc, `sb v0,6(a2)` @0x801015e0 / @0x801015ec /
  @0x801015f8 / @0x80101608** (Verzoegerungsplatz von `j 0x8010e734` @0x80101604). Nur das Phasenbyte; Port `g5_tentakel_phase_alle(9)` =
  `re15_g5_tentakel_phase` (nur `ph`) - gleich.
- EM37 sub9: Phasenbyte `lbu v1,6(s1)` @0x8010206c, `sltiu v0,v1,0xa` @0x80102074 (Ph >= 10 -> Epilog, Phase 10 tut nichts), Tabelle
  `lw v0,-0x2f8c(at)` @0x8010208c = Datei 0x74 (EM37 im RAM bei 0x8010D000). Ph7 Ende: `addiu v0,zero,0xb4` @0x8010266c -> `sb 8,6(s1)`
  @0x80102678, Zaehler +1 @0x80102684. **Ph9 @0x80102690:** `jal 0x8002959c` a3 0x40, `lhu 0x8c(s2)` - `lhu 0x158(s1)` -> `sh 0x8c(s2)`
  @0x801026a4-b4, +0x158 += 4 / rot_z(+0x78) += 8 @0x801026b8-cc, unter 0 -> `sh zero,0x8c(s2)` / `sb 0xa,6(a1)` @0x801026e4-f0.
- **Fuenf Durchfaelle** (Phase endet ohne Sprung, naechste Phase im selben Takt) - alle bestaetigt:
  sub8 ph1 @0x80101d90-0x80101da8 (`sb v0=1,0x219(s1)` im Verzoegerungsplatz des `jal 0x8005bd6c`) -> ph2-Koerper 0x80101dac (`jal 0x8002959c`
  a3 0x100); sub9 ph0 `sw v0,0x1e8(s1)` @0x80102140 -> ph1 0x80102144; sub9 ph4 `sb v0=7,0x222(s1)` @0x80102378 -> ph5 0x8010237c; sub10 ph0
  `sh v0,0x78(s1)` @0x801027fc -> ph1 0x80102800; sub6 ph0 `sb v0=1,6(s0)` @0x80101bc0 -> 0x80101bc4 (ph1 `jal 0x8002959c` @0x80101bc8,
  `addiu -0x40` @0x80101bd8, unter 0 -> `sw v0=1,4(s0)` @0x80101bf0 = Sub 0 Ph 0 wie Port).
- Sender 0x80104E9C (EM36): `lw v0,-0x1e0(at)` / `jr ra` / **`sw a1,4(v0)` @0x80104eb0** - schreibt nur das Wort +4. Der Port nullt im
  Kommando zusaetzlich +0x158 (bekannt, Dossier 10.6) - siehe 2.2 s21..s26.
- Gestreckte Kette: FUN_80019CD0 (RE2 EXE) `andi v0,v0,0x400` @0x80019d68, kopiert die Teil-Matrix +0x48..+0x67, `jal 0x8008dc04` @0x80019dd4
  mit (+0x8c,+0x8e,+0x90), `jal 0x8008e1b4` @0x80019de4 - die Streckung um Teil 0 (b). Die Kettenstreckung (a) selbst im RAM nachgemessen
  (`kette4.py`, `messung_kette_ram.txt`): Teil-Flags 0x4401 / 0x4801 / 0x4801 / 0x4801, Gelenkabstaende |p_i - p_(i-1)| = lokales X
  (3750/3003/1508) x Streckung/4096 an s03, s09, s11, s21, s26 fuer alle Arme auf +-3 (Streckung des Vorbilds, z.B. s03 Arm 2 233/187/93 =
  x 256/4096, s26 1406/1125/565 = x 1536/4096). Arm mit Streckung 0 (s03/s09 Arm 1): alle Gelenke auf p0, |p3 - p0| = 0.

### 2.2 Armzustaende gegen RE2-RAM, alle Savestates (`arme_abgleich4.py`, `messung_arme_ram.txt`)

Zuordnung Savestate -> Szenenbild selbst ueber den G5-Zustand (Phase + Zaehler +0x158, in Clip-Phasen 2/6/8/12 das Clipbild +0x14D): jeder
Savestate hat genau einen Kandidaten, und die G5-x-Lage stimmt dort jeweils mit dem RAM ueberein. Felder je Arm (Index +0x218 = Port-Arm):
Sub, Phase, Streckung `*(+0x198)+0x8c`, +0x158, Drehung +0x74/+0x76/+0x78.

| Savestates | Szenenbilder | Ergebnis Leon 5090 = Elza 5091 |
|---|---|---|
| s01, s02 | Vorlauf (G5 Ph 0) | nicht zugeordnet (RE2 Arme Sub 8 Ph 0 Streckung 0, Port Bild 0 ebenso) |
| s03..s20 (18) | 31, 80, 129, 165, 215, 264, 313, 363, 412, 441, 479, 529, 578, 628, 677, 727, 776, 812 | **72/72 Arme in allen 7 Feldern gleich** |
| s21..s26 (6) | 854, 904, 953, 1002, 1052, 1101 | Sub/Phase/Streckung/Drehung **24/24 gleich**, +0x158 abweichend |

- Damit auch s22..s25 (der Riegel prueft sie nicht): z.B. s22 Arme 0/2 Ph 3, 1/3 Ph 2 mit 3456; s23/s24 Arme 0/2 Ph 5, 1/3 Ph 3; s25 alle Ph 3 -
  wie RE2.
- +0x158 ab s21: RE2 257 (aus [T7]) bzw. Zufallswerte, Port 0 bzw. andere Zufallswerte (Leon und Elza dort unter sich verschieden, z.B. s22 Arm 0
  23 / 18, RE2 12). Ursache das Nullen im Port-Kommando (2.1). An keinem der sechs Punkte haengt Sub, Phase, Streckung oder Drehung daran.
- N1 im Einzelnen: s16 (Bild 628) alle vier Sub 9 Ph 7 Zaehler 159; s17 (677) alle **Ph 10, Streckung 0, Zaehler 257, rot_z +152** - Port gleich.
- N2: s09 (313) Arme 0/2/3 Sub 8 Ph 8 Streckung 3264, Arm 1 Ph 9 Streckung 0; s10 alle Ph 9 Streckung 0 - Port gleich.

### 2.3 Gezeichnete Arme (Framedump der echten exe; RE2 echt | Port vorher = Framedump Abnahme 3 | Leon NB4 | Elza NB4)

- **N2, Bild 296..336** (`a4_n2_bild296_bis_336.jpg`, Kopfausschnitt `a4_n2_kopf_bild296_bis_336.jpg`): Bild 313/316/320 **kein Armstummel
  mehr** rechts vom Kopf (vorher deutlich sichtbar), wie RE2. Bild 305/309: die einziehenden Arme enden wie in RE2 (oberer Arm Spitze ~200 px,
  unterer ~240 px vom linken Rand des Ausschnitts, RE2 ~205 / ~240); vorher waren sie laenger.
- **N1, Bild 640..806** (`a4_n1_bild640_bis_806.jpg`, obere Haelfte `a4_n1_obere_haelfte_bild646_bis_680.jpg`): ab 658 keine Arme mehr ueber
  der Masse, 660..806 nur Masse und Maul wie RE2 (vorher zwei rote Stangen bis 806). Armsignal (Rotanteil x 180..560, y 40..170;
  `arme_kurve.txt`): RE2 652 -> 654 **4,77 -> 1,74 %**, Port 654 -> 656 **4,72 -> 1,68 %**, beide mit derselben Spitze im Bild davor (erstes
  Ph-9-Bild, rot_z-Sprung). Mit dem gemessenen Zeitachsenversatz -2,0 liegen die Kanten auf demselben Bild. Grundversatz bleibt (RE2 vor der
  Kante 3,5..4,8 / Port 2,6..2,9 %, danach 1,7..2,3 / 1,3..1,6 %) - Farbe, bestand schon in Abnahme 3.
- **Cut 12 / Schnitt 5, Bild 812..950** (`a4_cut12_bild812_bis_950.jpg`): Port Leon und Elza 812..860 ohne Arme, ~870 der erste aus der Masse,
  880/890 weitere, 910/930 mehrere, 950 voll; RE2 Schnitt 5: bis 860 keine, 870 der erste, 880..910 wachsen, 930/950 voll - gleiche Zeitfolge
  (Kamera ist PORT-WAHL M3). Vorher: 850/860 schon voll draussen.
- Szenenende (`a4_ende_bild1090_bis_1162.jpg`): RE2 Arme 1090..1145 eingezogen, 1155 wieder draussen; Port Leon/Elza ebenso (Port-Zustand Sub 6
  Ph 1 bis 1120, ab 1125 Sub 11, 1150 Streckung 3248/1224/1680/624, 1162 3992/3480/3992/2400).

## 3. Gesamte Szene Bild fuer Bild gegen RE2 - Folgen der fuenf Durchfaelle und der Kettenstreckung

### 3.1 Wo NB4 das Bild aendert (`vorher_nachher4.py`, `messung_vorher_nachher.txt`)

Framedump Abnahme 3 (Kopf 15b2fd419) gegen NB4, Leon, je Spielbild (960x720): **Vorlauf bis Bild 18 pixelgleich** (92 Bilder, vor dem
ersten sichtbaren Arm). Danach Unterschiede (> 0,2 % Pixel mit |d| > 30, 160x120) nur in Arm-Bildern: 21..68 (Austritt, sub8-Durchfall + Kette),
112..262 (gehaltene Arme, s. 3.2), 301..312 (N2), 408..457 (Speer, sub9-Durchfaelle), 510..641 (Arme ueber der Masse), 648..1079 (N1, Cut 12),
1152..1287 (Ausfahren zum Kampf). Die Differenzbilder (`a4_diff_vorher_nb4_bild120_200.jpg`, `a4_diff_vorher_nb4_bild600_700.jpg`) zeigen die
Abweichungen ausschliesslich auf den Armen (600: Schattierung der gestreckten Arme leicht anders; 700: die vorher stehenden Arme sind weg);
Masse, Licht, Pfosten, Hintergrund, Figur unveraendert (Leisten `a4_szene1_bild-50_bis_100.jpg`, `a4_szene2_bild180_bis_479.jpg`,
`a4_szene3_bild520_bis_1200.jpg`).

### 3.2 Bewertung der geaenderten Stellen gegen RE2

| Bild | RE2 echt | Port NB4 (Leon = Elza) | vorher | Bewertung |
|---|---|---|---|---|
| 19..24 erster Arm (`a4_erster_arm_bild12_bis_30.jpg`, `a4_arm0_ausfahren_bild16_bis_28.jpg`) | rechtester Armpixel x 265 bei Bild 21 (re2_t) | erreicht x 265 bei ~23,0 (190 @22, 266 @23) | ~22,4 | NB4 2,0 Bilder hinter der RE2-Zeitachse = deren gemessener Versatz (-1,5..-3,3); kein Befund |
| 40 (`a4_arme_gehalten_bild40_bis_250.jpg`) | unterer Arm (Arm 2, Streckung im Aufbau) endet ~145 px | ~145 px | ~200 px | NB4 = RE2, vorher zu lang |
| 120..258 gehaltene Arme (Streckung 4096) | | Kanten 2..4 px gegen vorher verschoben (`a4_diff_vorher_nb4_bild120_200.jpg`) | | NB4-Gelenke = RE2-RAM s04 (Riegel M2 max Abweichung 0, eigener Lauf); **das Dossier 10.2 "bei 4096 bitgleich zur alten Kette" stimmt fuer das Bild nicht** - die Abweichung geht aber zu RE2 hin (Dossier-Ungenauigkeit, gering) |
| 305/309, 313..320 | s. 2.3 | wie RE2 | Stummel | N2 behoben |
| 412..441 Speer | ein Arm waagrecht bis ~180/250 px (412), zwei Speere 420/430 | gleich (412 ~185 px) | 412 bis zum Rand | NB4 = RE2 (s11/s12 bitgleich) |
| 652..806, 812..950, 1090..1162 | s. 2.3 | wie RE2 | | N1 behoben |

### 3.3 Uebrige Szene (unveraendert, nachgeprueft)

- Spruenge (`spruenge4.py`, alle 1361 Framedump-Bilder, 160x120): Median 0,96 / 0,97, 99-%-Quantil 6,8 / 6,9; ueber 7 nur die Schnittkanten F1559
  (38,7), F2051 = Bild 438 (16,3 / 16,6), F2421 = Bild 808 (30,4 / 30,9) und das Feuer der Rueckstoss-Szene F1542..F1557. Szenenende und
  Uebergang in den Kampf ohne Sprung. `messung_spruenge.txt`.
- Ton (`xa_ton.py` der Abnahme 2 auf meinen Mitschnitten `RE15_AUDIO_CAP_SYNC`): Xa 0x0B Takt 2171 (Korrelation 0,838), 0x0C Takt 2477
  (0,663), Abstand **306 = 438 - 132** wie RE2; Leon und Elza gleich. `messung_xa.txt`.
- Laenge: Vorlauf 55 Bilder (F1558..F1613), Szene 1162 Bilder bis F2775 - wie Abnahme 3.
- Masse/Morph, Licht, Pfosten: keine Code-Aenderung, in den Differenzbildern Bild 120/200/600/700 (3.1) ausserhalb der Arme kein geaendertes Pixel; Leisten
  180/240/265/360/479/520/600 wie RE2 (Abnahme 3 hat Gewichte und Umriss bitgleich gemessen; der Ablauftest meldet w0/w1 an acht RE2-Bildern
  gleich, Teil-2-Vertices max 2).
- Figur: Leon laeuft in RE2 ab Bild ~1, im Port ab 12; Vorlauf 55 statt ~70 Bilder (RE2 bei -50 schon in der Tuer) - bekannt seit Abnahme 2/3, gering.

### 3.4 M2-Nebenwirkung

Ablauftest (eigener Lauf, `messung_tests.txt`): "M2 Armvergleiche 123, davon <= 250 123, max Abweichung 0"; "M5 Speer Arm 0 |p3-p0| 8159 (RE2 s13
8170)" - die Kette sitzt an den RE2-RAM-Gelenken.

### 3.5 Kind-Kollision

Unveraendert (`tent_kind_kollision` nimmt `tent_gestreckt` = Streckung um den Teil-0-Rahmen der ungestreckten Kette, @0x80104f94-5074 laut
Kommentar). Fuer gerade Arme gleich der Zeichenkette, fuer gekruemmte bei Streckung < 4096 nicht - der Autor fuehrt das offen (Dossier 10.6).

### 3.6 Echtes Fenster

gdigrab auf das hwnd scheiterte in dieser Sitzung ("Failed to capture image (error 8)", Fenster verdeckt); ein erster Versuch mit
falsch ermitteltem hwnd 0 nahm den Desktop auf - diese Aufnahmen habe ich sofort geloescht, sie sind nirgends abgelegt. Stattdessen
`printwindow4.py`: PrintWindow(hwnd, PW_CLIENTONLY | PW_RENDERFULLCONTENT) nur auf das Fenster der eigenen exe-Kopie (Client 960x720,
~10 Bilder/s, 821 / 833 Bilder, 0 Fehlschlaege). Je Szenenbild das aehnlichste Fensterbild gesucht (`fenster4.py`, `messung_fenster.txt`,
`a4_fenster_leon.jpg`, `a4_fenster_elza.jpg`):

| Szenenbild | 309 | 313 | 320 | 660 | 700 | 870 | 950 |
|---|---|---|---|---|---|---|---|
| mittlere Differenz Leon (Median aller Fensterbilder) | 0,23 (31) | 0,23 | 0,45 | 0,25 | 1,23 | 0,26 | 0,30 (20) |
| Elza | 0,67 | 0,45 | 0,46 | 2,07 | 1,13 | 2,88 | 1,82 |

Das echte Fenster zeigt dasselbe wie der Framedump (Rest = ~10 B/s Abgriff, naechstes Fensterbild liegt bis 1 Spielbild daneben): Bild 313/320
ohne Stummel, 660/700 ohne Arme ueber der Masse, 870 ohne / 950 mit ausgewachsenen Armen - Leon und Elza.

## 4. Kampf

### 4.1 Pistolentest (ctest `integration_r41_birkin_pistole`, eigener Lauf)

`r41_birkin_pistole leon: Mutationen 1, Form-HP 150 bis 0, Rueckstoss 1, G5 1, G5 tot 1, Spieler-HP dabei 45`, Elza ebenso - **gewinnbar,
Spieler-HP 45** wie vom Autor angegeben (Abnahme 3: 38).

### 4.2 Stehen + Feuer gehalten (`kampf4.py`, `messung_kampf_steh.txt`, Leon und Elza bitgleich)

| | vorher (Abnahme-3-Lauf `steh_leon`) | NB4 |
|---|---|---|
| +7,5..8,1 s | ein Schub 260 | **ein Arm schiebt den Spieler in 11 Bildern um 2530 nach Osten** (F3008..F3018, 220..587 je Bild, kein Treffer, gr 0; Kamera wechselt bei F3012) |
| erster Treffer | +11,2 s Biss 45 -> 25 | +25,7 s Griff 45 -> 38 |
| weiter | +15,5 s -> 5, +19,4 s -> 0 | +27,1 s -> 18 |
| Ende | gefressen +20,1 s, G5-HP 240 | **G5 tot +29,6 s**, Spieler-HP 18 |

- Der Kampf verlaeuft anders, weil die Arme mit den RE2-genauen Durchfaellen an anderen Bildern stehen (Zeitfolge und damit Zufallsfolge
  verschoben); beide Verlaeufe sind faire Kaempfe (>= 7 s ohne Schaden, gewinnbar). Fair und gewinnbar: **ja**.
- Beobachtung B1 (kein Mangel dieser Abnahme): der Schub 2530 in 11 Bildern (`a4_kampf_schub_F2996_bis_F3018.jpg`) ist sichtbar - Leon gleitet
  in Zielhaltung nach hinten. Mechanismus = Kind-Kollision (radiales Herausdruecken, kein Tempo-Deckel; schon vorher gab es 742 je Bild nach dem
  Biss). Ob RE2 im Kampf genauso schiebt, belegt kein RE2-Kampf-RAM; gehoert zur offenen Kind-Kollision-Kette (3.5 / Dossier 10.6).

## 5. Testanpassung `unit_r41_birkin_wand_g5` (Teilfall N2)

- Aenderung: `if (kontakt && ++geduld >= 5)` statt `>= 10` - wann die Laufprobe startet (Zahl der Kontaktbilder vor der Probe), kein
  Spielwert. Die Pruefungen sind unveraendert: `n_kontakt > 0` (misst ueberhaupt etwas), **`n_wand == 0` an jedem Bildende** (der eigentliche
  Zweck: der Spieler steht nie in fester Kollision) und `lauf_try > 0 && lauf_ok == lauf_try` (Eingabe wirkt, waehrend ein Arm anliegt).
- Die Wandpruefung haengt nicht an der Schwelle - sie laeuft ueber alle Bilder. Die Schwelle 10 liess die Probe nie laufen (0/0) und haette den
  Riegel mit "Eingabe wirkt nicht" rot gemacht, ohne dass die Klemme sich geaendert hat; 5 stellt die Laufprobe wieder her. `kontakt` wird im
  Startbild selbst geprueft, die Probe beginnt also weiter nur, waehrend ein Arm am Spieler liegt.
- Eigener Lauf (`messung_tests.txt`): `N2: Arm-Kontakt 15 Bilder, Bildende in der Wand 0, Laufproben 3/3`; der Fuzz desselben Tests `g5: 424
  Faelle ... Bildenden in fester Zelle 0, eingekeilt 0` (5090 und 5091); `g5saat` 200 Saaten x 5 Lagen je Raum, 38 660 / 38 718 Griff-Bilder,
  0 in fester Zelle. **Der Test misst seinen Zweck weiter.** (Eine Mutationsprobe der Klemme habe ich nicht gemacht - Code nicht geaendert.)

## 6. M4 (Tunnellampen / Truemmer im Tuerloch) - Bewertung

- RE2 (`a4_m4_tuerloch_bild-54_bis_12.jpg`, `a4_m4_tuerloch_bild18_bis_72.jpg`): im dunklen Tuerloch zieht ein kleines helles Lampenrechteck
  (~15 x 10 px bei 640x480) vorbei, gesehen bei Bild -42, -12, 0, 18, 30, 48, 60 (alle ~12..18 Bilder), vom Vorlauf bis die Masse das Loch
  zudeckt (~100); dazu Truemmer. Port: Loch schwarz.
- Bewertung: **kein Blocker fuer die Abnahme dieser Spur, aber eine sichtbare Abweichung vom Massstab "GENAUSO" und darum offen zu fuehren und dem
  Nutzer ausdruecklich zu nennen.** Gruende: (1) Gegenstand der Spur ist der G5-Auftritt (Arme, Masse, Maul, Hereinziehen, Uebergang) - der ist
  jetzt in Zustand und Bild RE2-gleich; die Lampe ist Raum-Atmosphaere im Hintergrund, klein, ~5 s. (2) Der Mechanismus ist belegt, aber nicht
  gebaut: RE2-Raumobjekte ROOM7040 Modell 3/4 als Asset, sub09..sub12 bytegenau (0x32/0x26/0x3e), Zeichnen hinter R704_CUT09.PRI, Truemmer ueber
  die RE2-Raum-ESP - ein eigener Arbeitsauftrag. (3) Eine "einfache Lichtflaeche" ohne diese Belege waere geraten (RE-Gate) - dann lieber offen.
  Der Nutzer entscheidet, ob er sie nachgebaut haben will.

## 7. Maengel und offene Punkte

| Nr | Was | Beleg | Schwere / Bewertung |
|---|---|---|---|
| - | **kein neuer Mangel** | 2, 3 | - |
| D1 | Dossier 10.2: "bei Streckung 4096 bitgleich zur alten Kette" trifft fuers Bild nicht zu (gehaltene Arme 2..4 px verschoben); die Verschiebung fuehrt auf die RE2-RAM-Gelenke (M2 0) | `a4_diff_vorher_nb4_bild120_200.jpg` | gering, nur Text |
| B1 | Kampf: Arm schiebt Leon in 11 Bildern 2530 nach hinten (Kind-Kollision); Kampfverlauf anders als in Abnahme 3, fair und gewinnbar | 4.2 | Beobachtung, offen mit Dossier 10.6 |
| Z1 | +0x158 der Arme ab [T10] verschieden (Port-Kommando nullt, RE2-Sender schreibt nur +4 @0x80104eb0); an s21..s26 ohne Wirkung auf Sub/Phase/Streckung/Drehung | 2.2 | gering, offen (Dossier 10.6) |
| M4 | Tunnellampen/Truemmer im Tuerloch fehlen | 6 | offen, kein Blocker, dem Nutzer nennen |
| M3, M5, M6 | Cut 12 statt RE2-Schnitt 5 / Musik / PSX | Abnahme 2/3 | PORT-WAHL bzw. PSX-Luecke, unveraendert |

Behoben gegen Abnahme 3 (selbst nachgemessen): N1 (Einziehen ab [T7], Cut-12-Auswachsen), N2 (Stummel 313/320), dazu die Laengen der
ein-/ausfahrenden Arme (Bild 40, 305/309, 412).

## 8. Tests (re15_port/build, ctest, eigene Laeufe; `messung_tests.txt`)

| Lauf | Ergebnis |
|---|---|
| `unit_r41_g5auftritt` -V (mit RE15_G5A_SPUR) | gruen, 5090 und 5091: "NB4 Arme gegen RE2-RAM 80 von 80 gleich ... Reichweite Streckung 0 22 von 22 innerhalb 16 (max 4); Einzug [T7] fertig Bild 669; Bild 670..806 Arm vor der Masse in 0 von 137 Bildern (max Reichweite 572); Bild 302..336 Arm-1-Stummel in 0 von 35 (max 568)" |
| `unit_r41_birkin_wand_rein/_g5/_griff/_g5saat` -V (-j 4) | **4/4 gruen** (griff 160 s, g5 42 s, g5saat 33 s) |
| Alle Nicht-Integrationstests ohne die 7 df-haengenden Skripttests (-j 6), enthaelt die 46 gezielten der Abnahme 3 (Birkin/G5/Griff/Klemme) | **1068/1068 gruen**, 334,8 s |
| Integration (-j 3): r41_birkin_pistole, r41_g5_tod, r41_g5_westen, r35_zgirl_5090, r37_bissarm_echtlauf, r37_elzaems_elza1181griff, r37_re2zombie, r38_wasserzombie, r41_zombiefresser_tuer2000, pri_masken, r36_pri_schatten | **11/11 gruen**, 530 s (pistole Leon/Elza G5 tot, HP 45; g5_westen 0 in fester Zelle; g5_tod Tod bei HP 0) |
| unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd}, unit_r39_titelversion_ablauf, unit_r35_android_bash_mutanten | nicht gelaufen (Umgebung: df.exe haengt maschinenweit - im Lauf selbst bestaetigt, mein `df` hing und wurde per Task-Stopp beendet) |

Kein Test rot, keiner wiederholt. Nur eigene Prozesse (eigene exe-Kopien) liefen; keiner musste beendet werden.

## 9. Belege (`G5_belege/abnahme4/`)

- N1/N2: `a4_n2_bild296_bis_336.jpg`, `a4_n2_kopf_bild296_bis_336.jpg`, `a4_n1_bild640_bis_806.jpg`, `a4_n1_obere_haelfte_bild646_bis_680.jpg`,
  `a4_cut12_bild812_bis_950.jpg`, `a4_ende_bild1090_bis_1162.jpg`; `messung_arme_ram.txt` (+ `re2_arme.json`), `messung_kette_ram.txt`,
  `messung_arme_kurve.txt`, `messung_exe_gegen_spur.txt`.
- Szene: `a4_szene1_bild-50_bis_100.jpg`, `a4_szene2_bild180_bis_479.jpg`, `a4_szene3_bild520_bis_1200.jpg`, `a4_erster_arm_bild12_bis_30.jpg`,
  `a4_arm0_ausfahren_bild16_bis_28.jpg`, `a4_arme_gehalten_bild40_bis_250.jpg`, `a4_diff_vorher_nb4_bild120_200.jpg`; `messung_vorher_nachher.txt`,
  `messung_armspitze_s1.txt`, `messung_arme_kurve_s1.txt`, `messung_spruenge.txt`, `messung_xa.txt`.
- Fenster: `a4_fenster_leon.jpg`, `a4_fenster_elza.jpg`, `messung_fenster.txt`; Differenzbilder `a4_diff_vorher_nb4_bild600_700.jpg`; Tests
  `messung_tests.txt`.
- Kampf: `a4_kampf_schub_F2996_bis_F3018.jpg`, `messung_kampf_steh.txt`. M4: `a4_m4_tuerloch_bild-54_bis_12.jpg`, `a4_m4_tuerloch_bild18_bis_72.jpg`.
- Werkzeuge `werkzeug/` (Pfade auf den Scratch-Ordner dieser Sitzung): `lauf4.sh`, `re2ram4.py`, `mipsdis4.py`, `exedis4.py`, `arme_abgleich4.py`,
  `exe_spur4.py`, `kette4.py`, `re2zeit4.py`, `re2_kanten4.py`, `re2_bilder4a.py`, `leiste4a.py`, `serie4.py`, `arme_kurve4.py`, `arme_kurve4s1.py`,
  `armspitze4.py`, `vorher_nachher4.py`, `spruenge4.py`, `kampf4.py`, `lauf4p.sh` + `printwindow4.py` (Fensterabgriff), `fenster4.py`,
  `zeitabgleich4.py` (Bildabgleich, zu unscharf, verworfen),
  `m4_lampen4.py` (Helligkeitszaehlung verworfen, nur der Kontaktbogen zaehlt). `xa_ton.py` unveraendert aus `abnahme2/werkzeug/`.
