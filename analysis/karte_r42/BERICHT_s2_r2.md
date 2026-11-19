# Bericht Strang "karte", Stufe 2 (STAGE4 bis STAGE6), Runde 2

Zweig r42/karte, Code-Commit **94b132b32**, danach ein Doku-Commit mit diesem Bericht (Id im Schema). Kein Push.

## Kurz und in Alltagssprache

- **Der rote Punkt aus der Pruefung ist behoben.** Im T-foermigen Gang der Fabrik (Raum 5070, spaeter 5130) zeigt
  die Karte jetzt den Raum. Vorher sah man dort nur den Leon-Punkt und drei Tuerstriche auf leerem Blau.
- **Warum es fehlte:** Auf demselben gemalten Feld liegt auch der Zug 5090. Fuer den Zug ist das Feld in fuenf
  Stuecke zerteilt, damit nur das Stueck aufleuchtet, in dem man steht. Diese Stuecke kannten aber nur den Zug.
  Stand man im Gang, fuehlte sich keines zustaendig. Also wurde gar nichts gezeichnet.
- **Was jetzt gilt:** Steht man in einem anderen Raum auf so einem Feld, leuchtet das ganze Feld. Die Zug-Stuecke
  bleiben fuer den Zug wie bisher. Den Zug selbst habe ich nicht angefasst, den baut r42/rest um (U-12).
- **Neuer automatischer Test (Riegel):** Er prueft fuer jeden Raum auf allen Blaettern, ob der Raum, in dem man
  steht, wirklich rot auf dem Bild erscheint und der Punkt darauf sitzt. Auf dem alten Stand schlaegt er an (beim
  Gang und, noch frueher, bei Raum 5020), auf dem neuen nicht.
- **Der Live-Test ist strenger geworden.** Er liess bisher jeden siebten Raum ohne Rot durchgehen. Jetzt muss jeder
  betretene Raum rot sein: 98 von 98.
- **Im echten Spiel nachgemessen:** 51 Abzuege des Kartenschirms. In allen 51 sitzt der Punkt auf den Bildpunkt genau
  dort, wo die Rechnung ihn hinsetzt. Neu dabei: der Gang, die Raeume 4000, 4010, 6000 und 6030 (dort lief beim
  Betreten eine Szene; ich habe sie ueber die Spielmerker abgeschaltet) und Elza in 32 von 34 Raeumen.
- **Zwei Elza-Raeume ohne Abzug:** 4001 und 4031. Dort laeuft beim Betreten eine Szene, und der Kartenschirm geht
  nicht auf, auch nicht spaeter. Der Unit-Riegel prueft beide Raeume trotzdem.
- **Ehrlich offen:** In Raum 6000 steht der Punkt an zwei Stellen neben der gemalten Flaeche. Dort liegt ein Gang
  zur Fabrik (Tuer nach 50B0), den das Originalbild nicht zeigt. Ich habe nichts geaendert, weil mir ein Beleg fuer
  eine andere Lage fehlt.

Fachwoerter: "Blatt" = Kartenseite. "Feld/Kachel" = ein gemalter Raum darauf. "Teilbereich" = ein Stueck eines Feldes,
das eigens aufleuchtet. "Zeile" = Umrechnung Spielposition -> Kartenpunkt. "Riegel" = automatischer Test.
"Gegenprobe" = derselbe Test auf dem alten Stand muss rot sein. "FB_SHOT" = Abzug des Kartenschirms aus dem echten
Spiel.

## Die Pruefpunkte der Vorrunde

### ROT 1: 5070/5130 ohne Grundriss -> behoben

- Messung: `unit_r42_karte_sichtbar` baut die echte Op-Liste des Kartenschirms (`re15_inv_screen_build`) fuer alle
  210 Zonen mit Original-Rechteck. Auf cf95284eb fallen **genau** 5070/5071/5130/5131 heraus (0 von 81 Rasterpunkten
  auf Rot, 0 von 1145 gemalten Texeln rot). Kein anderer Raum auf irgendeinem Blatt.
- Mechanismus: Blatt 9 r7 hat in `s_map_teile` fuenf Teile mit zid 84 (5090). Der Zeichner malt ein Rechteck mit
  Teilen nur ueber die Teile; `teil_zustand` kennt nur zid 84 -> fuer zid 115/116 UNBESUCHT -> uebersprungen.
  Die Teile decken 1109 der 1145 gemalten Texel (die restlichen 36 = vier gemalte Querlinien).
- Abhilfe (PORT-WAHL, keine Verhaltens-Konstante): `re15_map_rect_state_fremd` (Zustand nur aus Zonen ohne Teile auf
  dem Rechteck), `teil_zustand` nimmt den staerkeren Zustand, der Zeichner malt bei einer fremden Zone das ganze
  Rechteck (gleiche Gatter wie ohne Teile, RE2 `beq` @0x8006E744), staerkere Teile liegen oben.
  Die Teilbereiche selbst sind schon eine Port-Ergaenzung (Original: zwei SPRTs je Rechteck ohne Zustand,
  FUN_80046fd8 Schleife @0x800472fc-0x800473e0).
- ROOM1110 (Blatt 3 r5, nur zid 17) ist unveraendert: `unit_map_teilbereich` gruen.
- Live-Test: `ROT FEHLT` 0 (vorher 5070, 5130); `KEIN ROT` fuer 5030/5110/6020 <-> 5070/5130 alle weg. Uebrig bleibt
  nur `KEIN ROT: ROOM1110(0) -> ROOM1100(1)`, das stand schon auf aaea5f8c2 so.
- Der blinde Fleck aus der Pruefung ist geschlossen: [J] prueft, dass das Feld GEZEICHNET wird, nicht nur, dass der
  Marker auf Texeln liegt; der Live-Pin verlangt jetzt jeden Raum.

### GELB

- 4000, 4010, 6000, 6030 jetzt im echten Spiel abgezogen (je 3 Lagen), alle genau. Szenen ueber Merker aus:
  4000 `Ck bank=4 bit=38` @ROOM4000.RDT 0x0129E, 4010 `bit 4:34` @0x006BE, 6000 `bit 3:100` @0x00F40,
  6030 `bit 4:23` @0x010D0.
- Elza: 32 von 34 Raeumen abgezogen, alle genau; 4001/4031 ohne Abzug (s.o.).
- 6000: Befund jetzt genauer. Der Punkt (219,78) und (235,85) liegt im Gang zur Tuer nach 50B0 (Tuer-Record 6000
  Slot 0, Welt (2800,2750) -> Karte (210,74)); das Bild r5 malt diesen Teil nicht. OFFEN.
- Unveraendert offen: 4050-Seitenkammern (Besucht-Bits fehlen), Tuermarken-Luecken 50D0<->50E0/50F0 und 5080<->6010,
  fuenf Drehungen PORT-WAHL mit nur einer Tuer, PORT-WAHL Blatt fuer 5080/6020.

## Tests

- `unit_r42_karte_sichtbar` [I] [J] [K] gruen. GEGENPROBE: aaea5f8c2 ROT ([I]/[J] 5020/5021, [K]), cf95284eb ROT
  ([I]/[J] 5070/5071/5130/5131, [K]). Eigene Baeume aus `git archive` unter
  `scratchpad/karte/s2r2/gegen_<commit>`.
- `integration_map_raum_live` verschaerft (jeder Raum rot): neu 98/98 gruen; GEGENPROBE aaea5f8c2 ROT (93/94, 5020),
  cf95284eb ROT (96/98).
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan)" -j 6`: 52/52 gruen.
- `ctest -R "integration_.*(map|karte|kartenbits|inv_screen|lageplan)" -j 6`: 13/13 gruen (vor dem Verschaerfen);
  danach `integration_map_raum_live` einzeln gruen.
- Volle Suite nicht gelaufen (Auftrag).

## Bilder (Kopfzeile "Messbild - kein Ergebnis", Abzuege aus dem echten Spiel)

- `analysis/karte_r42/messbild_tgang_5070_5130_s2r2.png` - der Gang jetzt sichtbar, Leon und Elza.
- `analysis/karte_r42/messbild_blatt8_s2r2.png`, `messbild_blatt9_11_s2r2.png`, `messbild_blatt12_s2r2.png`.
- Zahlen je Abzug: `analysis/karte_r42/abzuege_s2r2.txt`.
- Kein gdigrab-Video; die Abzuege sind FB_SHOT des echten Spielfensters (deterministisch).

## Kollisionen

`git merge-tree --write-tree` von r42/karte (94b132b32) gegen r42/finale 0d2551909, r42/gegenstaende 5515f2601,
r42/gore be62608c0, r42/rest 3beddd8af, r42/story 5849c8996, r42/lageplaene bce9c91a8, r42/original ee407d935:
**kein Konflikt**. Inhaltliche Beruehrung: r42/gegenstaende +4 Zeilen in re15_inv_screen.c, r42/rest +4 Zeilen in
re15_map_owned.c (U-11) - andere Stellen.

**Vorwarnung U-12 (r42/rest):** Wer 5090/5091 auf die Wagenkacheln von Blatt 12 umzieht, sollte die fuenf Teile und
fuenf Innenwaende mit zid 84 von Blatt 9 r7 mitnehmen. Dann hat r7 keine Teile mehr, und der neue Zweig in
`re15_inv_screen.c` laeuft dort nicht mehr. Bleiben die Teile stehen, sorgt er dafuer, dass der Gang trotzdem
gezeichnet wird. `unit_r42_karte_sichtbar` [K] prueft 5090 auf r7 und muesste dann mit umziehen.

## Dateien

- Code: `re15_port/engine/src/re15_map_zones.c`, `re15_inv_screen.c`, `include/re15_room.h`.
- Riegel: `re15_port/tests/unit/test_r42_karte_sichtbar.c` (+ `tests/unit/probes/r42_karte.cmake`),
  `re15_port/tests/integration/test_map_raum_live.c`.
- Bestand: `analysis/karte_r42/BESTAND_s2_r2.md`.
- Arbeitsdateien: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s2r2/`.
