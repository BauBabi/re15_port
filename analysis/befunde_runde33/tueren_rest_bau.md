# Runde 33, Thema T, Stufe 2 — alle restlichen Tueren bauen

Nutzer-Auftrag (woertlich): "Ich möchte das du - für alle Türen die jetzt noch fehlen mit der
Türanimation die Türen baust und Animationen hinzufügst, das wir da komplett sind."

Arbeitsbaum: `.claude/worktrees/r33_tueren` (Zweig `r33/tueren`). Vorgaenger: `tueren_rest_plan.md`
(Plan, Stufe 1), `tueren_rest_pilot.md` (Pilot G1/G4/G6).

⛔ PORT-WAHL, KEINE Original-Adresse: RE1.5 waehlt kein Tuerarchiv (Payload+12/+13 = 0). Jede Textur,
jede Archivwahl und jede Tabellenzeile hier ist eine Bauentscheidung mit gemessener Herleitung.
Belegt sind die Rechenwege: gezeigt = Texel * c / 128 (psx-spx GPU:1438-1446), Blattfarbe c = 73
(BK 68 @0x800142e8, L @0x8009a470, LCM 1600 @0x8009a490, RGBC 0x808080 @0x80014b58; tor_helligkeit.md),
Texel = gemalt * 128 / c ("gezeigt = gemalt" wie beim Tor, Runde 32).

## 0. Ergebnis (Kurzfassung)

| | physische Tueren | Seiten |
|---|---|---|
| Runde 31 (RE2-Archiv gleich) | 83 | - |
| Tor ROOM1170 (eigene Sequenz, v0.8.16) | 1 | - |
| Runde 33 Port-Archive (G1..G11) | 53 | 105 |
| Runde 33 G12 Durchgang ohne Blatt: RE2-Archiv OHNE Objekt DOOR36 (Blende + Ton) | 4 | 5 |
| **mit Tuersequenz** | **141 von 144** | Runde-33-Tabelle: **110 von 110 geplanten Seiten** |
| nicht baubar: G14 ROOM1250 Slot 0/1/2 (sce 0, nie scharf, plan 2.4) | 3 | - |

Die drei einseitigen Tueren der Runde 31 sind jetzt von BEIDEN Seiten animiert: T048 (S159 DOOR0A R31,
S087 **P07M**), T078 (S146 DOOR1E R31, S154 **P1EL**), T161 (S314 DOOR2A R31, S322 **P1AZ**).

30 Port-Archive in `re15_port/shared_assets/RE15DOOR/` (Tabelle `engine/src/gen/re15_tuer_eigen.inc`, 220 Zeilen),
alle 110 Seiten im Kontaktbogen der echten exe angesehen, 6 Tueren im echten Spiel per Aktionstaste durchgegangen.

## 1. Stand beim Start (gelesen)

- 100 von 144 Tueren mit Sequenz (83 Runde 31 + 17 Pilot); Port-Archive P07G, P1DG, P16M.
- Paket-Gate `release/make_package.sh` prueft bereits JEDE Datei `shared_assets/RE15DOOR/*.DO2` (Schleife ueber den
  Quellbaum: vorhanden, nicht leer, `cmp` gleich) - die 27 neuen Archive sind damit ohne Aenderung gedeckt.
- `shared_assets/RE2/DOOR` hatte 24 RE2-Archive; neu kopiert (sha1-geprueft, `re2_tuer_tabelle.py --kopiere-nach`):
  **DOOR0C** (Basis P0CD), **DOOR14** (Basis P14A), **DOOR36** (G12, zur Laufzeit gelesen).

## 2. Werkzeuge / Laufzeit (neu oder erweitert)

| Datei | Zweck |
|---|---|
| `tools/tueren/tuer_rezepte.py` (neu) | Textur-Rezepte aller Stufe-2-Archive + Hilfen (Farbe messen `farbe_re15`/`farbe_bild` auf dem ROH-Hintergrund `extracted/PSX/.../ROOMrrrcc.bmp`, Grundformen mit Kantenglaettung, Umfaerben mit RE2-Zeichnung, `ausbessern` = Flicken mit Helligkeitsausgleich), MD1-Ergaenzung P1EL |
| `tools/tueren/tuer_archiv_bauen.py` | laedt die Stufe-2-Rezepte; Archiv je Seite (`SEITE_ARCHIV`), G12-Zeilen (eigen 0, DOOR36), Selbst-Tausch, Versatz am Spender-Anhaengepunkt, Schluessel `fuer_eigen`, `md1_eigen`, Liste der geplanten Seiten `re15_tuer_geplant[]` (bricht ab, wenn eine geplante Seite keine Zeile hat) |
| `tools/tueren/tuer_rest_plan.py` | P07T, P1BD, P1DL, P27K, P27O; G4c; G12 = DOOR36; Tausch P1DG/P1DK/P1DL = Selbst-Tausch, P1B3/P1BD = Versatz |
| `tools/tueren/tuer_g12_ton.py` (neu) | dekodiert die Tonteile der objektlosen RE2-Archive 20/21/32/34/36, Huellkurve + Einsaetze (Wahl G12, Abschnitt 5) |
| `tools/tueren/tuer_uv_sonde.py` (neu) | rastert ein Archiv mit Koordinaten-Textur (Katalog-Simulator + Rasterer tor_helligkeit): welcher Teil liest welche Texel (DOOR27, DOOR26, DOOR1E) |
| `tools/tueren/tuer_echtlauf_taste.sh` (neu) | Echtlauf per AKTIONSTASTE: `RE15_DEBUG_JUMP`, Standplatz `RE15_PLAYER_POS="x,z,rot,band"`, `RE15_PRESS=square@300`, `RE15_FRAMEDUMP`, `RE15_TUER_SERIE`, `RE15_SE_DEBUG`; eigene exe-Kopie `re15_pc_r33e.exe` |
| `tools/tueren/tuer_rest_bogen.py` | Archiv je Seite + G12, `--praefix`, `--ohne` |
| `tools/tueren/tuer_maschine_referenz.py --r33` | Simulator-Referenz fuer die NUR in Runde 33 benutzten Paare -> `tests/unit/gen/r33_tuer_referenz.inc` |
| `platform/pc/main.c` | Messhaken `RE15_PLAYER_POS` beim JUMP-Spawn: optional 4. Wert = Band, Y = -Band*0x708 wie der JUMP-Executor (@0x8001d7b8-d4). Env-gegated, kein Spielverhalten |
| `include/re15_door_seq.h`, `engine/src/door_seq_zuordnung.c`, `platform/pc/src/door_scene_pc.c` | `re15_griff_tausch_t`: `versatz_vorn/hinten`, `fuer_eigen`; `re15_tuer_eigen_t.md1_eigen`; `re15_door_seq_griff_tausch_fuer(archiv, spender, eigen)`; Spender-Archiv mit SEINER Basis lesen (Selbst-Tausch); `re15_door_seq_zeilen_runde31()`, `re15_door_seq_geplant()` |

Griff-Tausch, neu (PORT-WAHL; RE2 tauscht Griffe nur archivintern per Bit 7):
- **Selbst-Tausch** (P1DG, P1DK, P1DL): nur die Grund-Drehung kommt vom Spender DOOR07 (waagerechter Druecker statt
  des um x -780 gekippten 1D-Drueckers), Mesh + Textur aus dem EIGENEN Archiv - zulaessig, weil DOOR1D.m1 = DOOR07.m1
  bytegleich ist (Generator prueft Ecken + UV). Damit hat jedes 1D-Archiv seinen eigenen Druecker (Pilot-Punkt b).
- **Versatz** (P1B3, P1BD): die DOOR23-Riegelstange sitzt am DOOR23-Anhaengepunkt statt am 1B-Druckstangenpunkt,
  passend zur Ausbuchtung der DOOR23-Textur: vorn (0,-584,+458), hinten (0,-1064,+418) = Anker DOOR23 V0/V1
  (130,-3524,-2952)/(-130,-3524,-2952) minus Anker DOOR1B (130,-2940,-3410)/(-130,-2460,-3370) [SIM tuerkatalog.VM].
- `fuer_eigen`: drei Port-Archive haben dasselbe Paar DOOR1D <- DOOR07 - der Tausch wird je Port-Archiv gesucht.

## 3. Archive (Rezept, Messung, Befund im Bogen)

Kontaktboegen (echte exe, RE15_TUER_SEITE + RE15_TUER_BOGEN, beschleunigter Renderer, Rueckleser vor dem
Present): `tueren_rest_belege/bau_kontaktbogen_01..09.jpg` (je Seite RE1.5-Ausschnitt | Sequenz Anfang | Mitte).
Alle 110 Seiten angesehen; debug.log: 110 x "Sequenz fertig", alle Notizen 0x0, kein "fehlt/passt nicht/NICHT lesbar".

| Kennung | Basis | Tueren (Seiten) | Rezept (gemessen) | im Bogen angesehen |
|---|---|---|---|---|
| P07G | DOOR07 | G1: 12 (23 Seiten) | Pilot + **silbernes Rechteckschild** unter dem Druecker (Pilot-Punkt a, s. 4) | Druecker auf Schild, Griffseite je Seite wie gemalt |
| P07T | DOOR07 | S023 S024 S025 (Treppenhaus ROOM1060) | wie P07G, Farbe = Median dieser drei Seiten (72/82/67) | hell graugruen wie gemalt (Pilot-Punkt c) |
| P06F | DOOR06 | G2: T094 T098 T103 T105 T106 T107 (16) | Randnut gerundet u 15..112 v 9..211, zwei vertiefte Felder mit breitem dunklem Rahmen u 28..100 v 28..94 / 111..189 (S189 c08 Profil), Feld 59/63/54 (Median 8 Ausschnitte), Korn DOOR07-Blech, DOOR06-Griffplatte behalten | Buegelgriff auf Platte, Felder + Nut wie S189/S213 |
| P06U | DOOR06 | G2b: T096 (2) | erhabene Felder u 22..106 v 16..100 / u 30..98 v 112..190 mit Fase + Fuge (S188 c00) | wie S188 |
| P1B3 | DOOR1B | T071 S136, T076 S141 | DOOR23-Blatt je Fluegel (Ausbuchtung an der Fuge = u klein, UV-Sonde), braun/oliv (Median S136/S141) + Riegel DOOR23 (Versatz) | wie S141: Achteckprofil, Riegel mit Rosette an der Fuge |
| P1BD | DOOR1B | S155 S156 S157 S158 (ROOM2070) | wie P1B3, Farbe der ROOM2070-Seiten (dunkel, kaltes Licht) | dunkel wie gemalt |
| P1DG | DOOR1D | G4: T026 T054 (4) | Pilot + Druecker in der gemalten G4-Farbe (Selbst-Tausch, kalibriert) + Schild | Druecker gezeigt = gemalt (4) |
| P1DK | DOOR1D | T014 (2) | DOOR1A-Stahlrahmen, 3 Felder, Kartenleser weg (gespiegelte Feldseite, helligkeitsangeglichen), dunkelgrau-teal | 3 Felder je Fluegel wie S030, Druecker + Schild an der Fuge |
| P1DL | DOOR1D | T045 (4) | wie P1DK, 2 Felder (oberer Querriegel weg) - S080 "hohes Feld + Querriegel" | 2 Felder, Druecker an der Fuge |
| P04B | DOOR04 | T035 (2) | DOOR04 blau -> braunes Holz (S063 c00: 33/25/6), Messingschilder behalten | Kassetten + Messingstangen wie S063 |
| P0CD | DOOR0C | T025 (2) | helles Holz (82/69/42 aus ROOM10C03.bmp), hohes schmales Drahtglasfenster u 26..57 v 17..137, Panikstange v 147..153 (S041 Vollbild: 20..45 % ab Fuge, 8..63 %) | wie S041 |
| P16R | DOOR16 | T047 (2) | Leiter: Helligkeit RE2, Farbton der gemalten Leiter ROOM11A0 (0,97/1,08/0,68 - im gruenen Kanallicht oliv-braun gemalt) | Leiter oliv-braun |
| P1EL | DOOR1E | T029 S048, T078 S154 | waagerechte Lamellen (Blech 80. / Spalt 20. Perzentil der Lueftung) **+ MD1: Lamellenplatte** (s. 6) | Lamellen wie S154/S048 |
| P1EU | DOOR1E | T080 (2) | Staebe nur unten (Stabteile oben Texel 0), dunkel | Oeffnung oben, Staebe unten wie S145/S152 |
| P25G | DOOR25 | T011 T017 T018 (3) | Schild, Aufkleber, Griffmulde, Rippen, Pfeil weg (ausbessern), grau-teal (Median S013/S040/S057) | glatte Aufzugtuer |
| P14A | DOOR14 | T102 (1) | Rahmen + Mittelriegel hellgrau (Pfosten ROOM30704.bmp x 42..54: 80/82/76) | hellgrauer Rahmen, Rauten |
| P2DS | DOOR2D | T112 (1) | Bedientafel + Lampe Texel 0 (nicht gezeichnet), Buehne + Warnrand bleiben | Buehne kommt, ohne Tafel |
| P07R | DOOR07 | T049 (2) | DOOR22-Rost ohne Nietrand (Inneres gestreckt) + ohne D-Riegel, braun (S089 c09/S090 c12), duenner dunkler Druecker | braune schlichte Tuer |
| P1AP | DOOR1A | T050 T072 T073 (6) | DOOR23-Profil, Ausbuchtung durch gerade Profilzeilen ersetzt, braun, dunkle Platte unter dem Druecker | Profil ohne Ausbuchtung wie S151 |
| P1DO | DOOR1D | T074 (2) | orange (64/34/12), Gitter weg, schmales dunkles Schild u 42..90 v 45..52, gelbes Warndreieck (S153) | wie S153/S144 |
| P26W | DOOR26 | T084 (2) | einteiliges Schott (V2/V3 = Mesh 0, UV u 57..4 gespiegelt, UV-Sonde) dunkel, zwei Warnstreifen v 50..60 / 104..114 (Gelb gemessen 34/36/19 - gemalt dunkel), Rad rot (41/24/19) | dunkles Schott, Streifen, rotes Rad |
| P24B | DOOR24 | T095 (2) | Aushang weg, Fenster flach u 40..96 v 44..62, dunkler Griffkasten, Trittblech dunkel, hell beige | hell, Fenster oben, Kasten an der Griffseite |
| P07D | DOOR07 | T123 (2) | Damentoilette: Feld mit abgeschnittener Ecke, Piktogramm-Schild orange, blaues Schild (S236 c00) | wie S236 |
| P07H | DOOR07 | T126 (2) | Herrentoilette: wie P07D, Piktogramm blau (S232 c02) | wie S232 |
| P27S | DOOR27 | S273 S274 (Laborseite) | gelbes Strahlenschild (80/102/19) im linken Teil, rotes Dreieck + gelb-schwarzer Aufkleber rechts, blaugrau | ein Schild (s. 6), Dreieck + Aufkleber |
| P27K | DOOR27 | S269 (Gang ROOM5040) | wie P27S ohne Strahlenschild, blaugrau | blaugrau |
| P27O | DOOR27 | S305 (Gang ROOM5120) | wie P27K, orange gemalt (Raumlicht) | orange wie gemalt |
| P1AZ | DOOR1A | T161 S322 | zwei Felder (oberes v 0..106 hell, Leiste 106..120, unteres), senkrechte Rahmenleiste u 20..28 an der Griffseite, Licht als Verlauf | wie S322 |
| P07M | DOOR07 | T048 S087 | DOOR14-Maschendraht (Loecher Texel 0) auf das Standardblatt, Rahmen braun (S087) | Gittertuer mit Mittelriegel wie S087 |
| P16M | DOOR16 | G6: T023 T052 T053 (Pilot) | unveraendert | - |

Helligkeit (nicht doppelt abgedunkelt), `tuer_rest_helligkeit.py` auf den Serienbildern der Echtlaeufe (Bild 60):
P0CD F/T 0,567, P25G 0,567, P1B3 0,569, P1EL 0,587 (c 75,7), P16R 0,493 (c 63 = Leiterlicht) - also F/T = c/128: das
RE2-Tuerlicht wirkt EINMAL, und weil Texel = gemalt*128/c, ist gezeigt = gemalt an den Texelstellen (F/P = 1),
ausser wo gekappt: P24B (gemalt L 142, zeigbar hoechstens 145 bei c = 73 ohne Flag 0x1000 -> 9 897 Texel mit
Farbton erhalten gekappt), P27O 3 559 (orange Spitzlichter), P14A 1 079 (heller Rahmen), P1AZ 864, P06U 351.
Flag 0x1000 (BK 136) waere eine SCD-Aenderung - nicht gemacht (SCD bleibt bytegleich, Riegel "archive").

## 4. Pilot-Punkte

**a) Griff-Groesse G1.** Gemessen in den 19 G1-Ausschnitten (Griffbox, Punkte > Blatt + 30): Hebel laengster Lauf
Median 22 Texel (17 % der Blattbreite), mit Schild u 4..32 = 28 Texel (22 %); gezoomt (`build/r33_tueren/ueber/
g1_griff_zoom.png`): senkrechtes Rechteckschild ~8 x 16 Texel + waagerechter Hebel ~18 Texel. In der Sequenz
(S001 Anfang, 960 px): Blatt 399 px, Druecker 57 px = 14,3 % - der DOOR07-Hebel (509 von 3599 = 14,1 %) ist
also fast so LANG wie gemalt; was fehlte, ist das Schild (der Pilot hatte den Pfeilschild-Umriss des DOOR07
entfernt). Entscheidung: kein Formwechsel (DOOR05-Druecker mit Rosette braeuchte einen silbernen Port-Spender),
sondern das gemessene Rechteckschild silbern auf das Blatt, 8 x 16 Texel an der Lage des 3D-Hebels (Anker z -3404
-> u 6,9; y -3352 -> v 111): u 3..11 v 103..119. Ebenso fuer alle DOOR1D-Druecker (u 3..11 v 120..136). Beleg
`tueren_rest_belege/bau_pilotpunkte_abc.jpg`. Rest: gemalt sitzt der Druecker ~16 Texel tiefer (v ~127) als der
DOOR07-Anker; die Lage des Meshes bleibt (MD1/SCD bytegleich).

**b) G4-Druecker 22 % zu dunkel.** Behoben durch Selbst-Tausch (eigene Textur je 1D-Archiv) + Kalibrierung an
der Messung (hellste Griffpunkte > Blatt + 25, Median - dieselbe Messart wie die gemalte Grifffarbe):
Pilot 0,77 -> Stufe 2 erst 1,19 (Kontrastgrenze macht die Antwort nichtlinear), Faktor 99,4/117,9, 2. Messung 1,07/
1,10, nochmals x 99,4/108,3 -> **S045 1,00, S100 1,00, S049 1,03, S104 1,03**. G1 mit derselben Messung 0,99.

**c) Treppenhausseiten.** S023/S024/S025 (ROOM1060) sind hell graugruen GEMALT, ihre Gegenseiten S056/S039/S015
dunkel (`g1_re15_entz.jpg`) -> eigenes Archiv **P07T** (Farbe = Median der drei Seiten) - Bogen 02: wie gemalt.
Nach derselben Regel ("je Seite ein Archiv, wenn verschieden gemalt"): P1BD (ROOM2070-Seiten der Panzer-
Doppeltueren, dunkel), P27K/P27O (Gangseiten der P-4-Labortuer, blau bzw. orange gemalt, ohne Strahlenschild).

## 5. G12 (Durchgang ohne Tuerblatt): DOOR36

`tools/tueren/tuer_g12_ton.py` (Beleg `tueren_rest_belege/bau_g12_huellkurven.jpg`, `build/r33_tueren/g12/g12_ton.json`):

| Archiv | Ton 0 | Einsaetze (Huellkurve 20 ms) | Se_on (Bild) | RE2-Einsatz |
|---|---|---|---|---|
| DOOR20 | 1,84 s, 19 423 Hz | 1 - ein ~0,7 s langes Dauergeraeusch (Schleifen/Gleiten) | 80 | ROOM4100 -> 4040 |
| DOOR21 | 4,68 s | 11 unregelmaessige | 80 | in RE2 Leon nicht benutzt |
| DOOR32 | 4,43 s | 5 + Dauergeraeusch ab 2,9 s | 20 | ROOM2080 -> 2080 (Selbst-Uebergang, Skript) |
| DOOR34 | 4,51 s | 6, lauter werdend | 20 | ROOM2140 -> 2140 (Selbst-Uebergang, Skript) |
| DOOR36 | 0,96 s, 32 795 Hz | 1 scharfer Einsatz mit Abklingen, **zweimal gespielt** | 80, 140 | ROOM4100 -> 4040 (Raum-zu-Raum) |

Wahl **DOOR36 V0** fuer alle vier G12-Tueren: (1) RE2 zeigt einen Raum-zu-Raum-Uebergang ohne Tuerobjekt mit 20 oder 36
(ROOM4100 -> 4040); 32/34 sind Skript-Selbst-Uebergaenge, 21 kommt nicht vor; (2) unter 20/36: DOOR20 ist ein
anhaltendes Gleit-/Schleifgeraeusch (etwas, das sich bewegt), DOOR36 zwei kurze Einsaetze im Abstand von 1 s (Bild 80/
140) - ohne Tuerblatt bewegt sich nichts, zwei kurze Einsaetze passen zum Durchschreiten. Maschine: DOOR36 V0 281 Bilder,
kein Objekt je an, 2 Se_on, kein Schliesston (probe_r33 "maschine" gegen den Simulator). Skript 0 hat keinen Switch
auf var 0x0C (nur V0). T085 (Hochklettern in eine Nische, Selbst-Uebergang) bekommt dasselbe Archiv - RE2 hat fuer
Klettern kein objektloses Archiv; offen, ob der Nutzer dort lieber Stille/anderes will. T019 (Dach, Band 6): Zeile
vorhanden, Erreichbarkeit im Spiel nicht geprueft.

## 6. Grenzen der Textur (gemessen) und was getan wurde

- **P1EL Lamellen (G7):** die RE2-Lueftungsgitter 1E/33/35 sind 3D-Staebe mit ECHTEN Luecken (`tuer_uv_sonde.py 1E 0`:
  schwarz zwischen den Staeben = kein Dreieck; Mesh 0: 6 achteckige Staebe z 163..1603, x 100..200, y 81..1116).
  Die reine Textur ergab ein Karomuster (Bogen, erster Lauf). RE2 hat keine Lamellenklappe. Darum (PORT-WAHL) eine
  **Lamellenplatte**: +4 Ecken, +4 Dreiecke in Mesh 0 (x 150, z 18..1782, y 15..1185, vorn + hinten), UV wie die
  Stabflaechen (v = 5 + (y-81)*68/1035, u = 124,7 - 0,0606 z), Umlauf wie die Stab-Dreiecke mit Normale +x. Tonteil +
  SCD bytegleich, die Basis-Meshes sind der Anfang jedes Meshes (Riegel "archive": `md1_eigen`). Bogen + Echtlauf:
  waagerechte Lamellen wie S154/S048.
- **P27S Strahlenschild:** DOOR27 liest fuer den Kasten unten links dieselben Texel (u 0..80 v 7..70) wie fuer die obere
  Flaeche des linken Teils (UV-Sonde) - das Schild stand zweimal im Bild. Jetzt in v 74..98 (v 72/73 liest zusaetzlich die Kastenunterkante): einmal, aber auf ~55 %
  der Hoehe statt gemalt ~15..35 %.
- **P26W Handrad:** Rad-Mesh bleibt in Blattmitte (RE2); gemalt ist ein kleines Rad unten an der Kante.
- **P07D/P07H Rueckseite (V1: S239/S240):** das Blatt zeigt beim Aufziehen seine Rueckseite mit waagerecht gespiegelter
  Textur (Griffkante bleibt u klein) - Piktogramm und Feldecke erscheinen dort gespiegelt; RE1.5 zeigt die Innenseite
  nur steil/dunkel.
- **P14A** "groebere Rauten": nicht nachgebildet (Teilung im Ausschnitt nicht messbar) - RE2-Maschendraht bleibt.
- **P24B:** heller als zeigbar (s. 3, gekappt).

## 7. Echtlauf im Spiel per Aktionstaste

`tools/tueren/tuer_echtlauf_taste.sh` (Standplatz aus `probe_r33_tueren standplatz <raum> <ziel> [bank:bit]`), echte exe
(Kopie `re15_pc_r33e.exe`), beschleunigter Renderer, RE15_FRAMEDUMP + RE15_TUER_SERIE + RE15_SE_DEBUG. Ablaufboegen
`tueren_rest_belege/bau_echtlauf_1.jpg` / `_2.jpg` (alter Raum | Abdunkeln 8/24 | Tuer 20 / Mitte / Ende | Schwarz |
neuer Raum F0/F4/F8/F40):

| Lauf | Art | Seite | Standplatz (x,z,rot,band) | debug.log | Ton (RE15_SE_DEBUG) |
|---|---|---|---|---|---|
| g5b_10c0 | Doppeltuer (Holz, beide Fluegel) | S041 T025 P0CD V1 | -7027,-2014,1792,0 | 307 Bilder + 1, Notizen 0 -> ROOM10D0 | Se_on Bild 90 + Schliesston (2 Stimmen) |
| g3_2030 | Doppeltuer (Panzer, Riegel-Tausch) | S141 T076 P1B3 V3 | 6730,-200,0,3 | 307 + 1 -> ROOM2070, "Griff-Tausch ... geladen" | Se_on 90 + Schliesston |
| g6b_11a0 | Leiter (hinauf) | S088 T047 P16R V4 | 237,24377,2816,1 | 331 + 1 -> ROOM3000 | 4 Sprossen (Bild 110/165/220/275), kein Schliesston |
| g7_10f0 | Lueftung (Lamellen) | S048 T029 P1EL V0 | -5187,-1312,512,1 | 301 + 1 -> ROOM1090 | Se_on 70 + Schliesston |
| g12_4080 | Durchgang ohne Blatt | S248 T117 DOOR36 V0 | -3630,2550,2048,0 | 281 + 0 -> ROOM4030 | 2 x Stimme 32 795 Hz (Bild 80/140) |
| g8_1120 | Aufzugtuer | S057 T018 P25G V0 | 1064,6972,768,0 + RE15_SET_FLAG=4:243 | 286 + 1 -> ROOM1080 | Se_on 110 + Schliesston |

Angesehen: in allen sechs dunkelt das stehende Bild des alten Raums ab (Abdunkeln 8 -> 24), die Sequenz laeuft mit dem
Port-Archiv (bzw. schwarz mit Ton bei G12), danach Schwarz und Einblendung des neuen Raums (F4 dunkel -> F8 hell).
Fallen: (1) ROOM2060 (Lueftung S154): der Spieler wurde vor dem Tastendruck angegriffen und starb (debug.log mo 7/8/9,
danach Neustart ROOM1240) -> Lueftung ueber ROOM10F0 gelaufen; (2) die Aufzugtueren setzen ihren Door_aot_set nur bei
Flag (4,243): ROOM1120 main00 @0xC9A `Ck 21 04 f3 00` -> sonst Aot_set (Meldung) @0xC9E, Door_aot_set @0xCB6.

## 8. Riegel / Suite

- `probe_r33_tueren archive`: 30 Archive, 46 Archiv-Varianten Bild fuer Bild gleich dem Basis-Archiv; P1EL: MD1 = Basis +
  4 Dreiecke.
- `probe_r33_tueren zuordnung`: 220 Zeilen (10 G12), alle treffen ihren Door_aot_set, 110 Seiten, 57 Tueren; Plan-Deckung
  (110 geplante Seiten, alle in der Tabelle); Spielschritt-Durchgaenge ROOM1000 -> 1050 (P07G), ROOM3050 -> 30E0 (P06F),
  ROOM4080 -> 4030 (DOOR36, eigen 0).
- **neu** `probe_r33_tueren maschine` (unit_r33_tueren_maschine): 9 Paare, die nur Runde 33 benutzt (DOOR04 V2, DOOR07
  V0/V1, DOOR0C V0/V1, DOOR14 V0, DOOR1D V0/V1, DOOR36 V0) gegen den Katalog-Simulator: 9 von 9 gleich.
- `probe_r31_tueren zuordnung` angepasst: nur die Runde-31-Zeilen; S226 jetzt G12 -> DOOR36 V0; ROOM1050 -> ROOM1090
  (T014) stellt jetzt die Anfrage mit P1DK (vorher "nicht abgedeckt" - seit Stufe 2 gibt es keine begehbare Tuer
  ohne Sequenz mehr).
- RE15_MIN_TESTS 418 -> 419 (+ unit_r33_tueren_maschine).
- Suite: 419/419 (Abschnitt 10).

## 9. Offen

1. G12-Tonwahl (DOOR36) fuer T085 (Klettern) ist eine Naeherung; T019 (Dach) im Spiel nicht angelaufen.
2. P27S Schild tiefer als gemalt (geteilte UV), P26W Rad in der Mitte, P07D/H Rueckseite gespiegelt, P14A Rauten RE2,
   P24B/P27O gekappt (Abschnitt 6/3).
3. Ton nicht abgehoert (Stimmen + Tonhoehe im debug.log, Tonteile bytegleich den Basis-Archiven).

## 10. Suite und Commits

- `bash re15_port/tools/local_build.sh all`: **`=== LOCAL-BUILD-OK (all) — Tests 419/419`** (zweimal gelaufen, der
  zweite Lauf nach der letzten Archiv-Aenderung P27S; kein GUI-Flattern).
- Commits auf `r33/tueren`: `d094cea1` wip (Rezepte, G12, Selbst-Tausch/Versatz) und der Abschluss-Commit dieser Stufe
  (feat, Archive + Riegel + Dossier + Belege).
- Belege (verkleinert): `tueren_rest_belege/bau_kontaktbogen_01..09.jpg` (110 Seiten), `bau_pilotpunkte_abc.jpg`,
  `bau_echtlauf_1.jpg` / `_2.jpg` (6 Echtlaeufe per Aktionstaste), `bau_g12_huellkurven.jpg`.
