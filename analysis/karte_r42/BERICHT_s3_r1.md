# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 1

Zweig r42/karte (Stand 63d51569d + dieser Commit). Kein Push.

## Kurz und in Alltagssprache

- **Ich bin die Karte mit beiden Figuren komplett durchgegangen.** Leon und Elza, alle Bereiche von der
  Polizeistation (STAGE1) bis zur U-Bahn (STAGE6). In jedem Raum habe ich die Karte im echten Spiel geoeffnet
  und ein Bild davon gemacht, meist an zwei bis drei Stellen im Raum. Das sind 464 Bilder.
- **Wie bei einem echten Durchlauf** sind die Raeume nacheinander betreten. Auf jedem Bild sind die schon
  besuchten Raeume gruen aufgedeckt, der aktuelle Raum ist rot, der Rest bleibt verdeckt.
- **Ergebnis:** In 429 von 464 Bildern sitzt der Leon- bzw. Elza-Punkt auf den Bildpunkt genau dort, wo er
  hingehoert. Das Blatt stimmt, und der Raum ist rot. Leon und Elza zeigen an derselben Stelle denselben Punkt
  (99 von 99 Paaren).
- **STAGE2 bis STAGE6 im Einzelnen:** Jeder Raum, in dem man die Karte aufrufen kann, ist richtig, bis auf
  einen: im Zug 5090 leuchtet der Raum nicht rot, und der Punkt steht neben der gemalten Flaeche. Den Zug baut
  gerade der Zweig r42/rest um (Punkt U-12). Ich habe ihn deshalb nicht angefasst und die Messung dorthin
  weitergegeben.
- **Was nicht geht, und warum:** In fuenf Raeumen laesst sich die Karte im Spiel gar nicht oeffnen. Dort laeuft
  durchgehend eine Szene: der Fahrstuhl 3080 (die Fahrt bringt Leon direkt in Raum 4000), die Szenenraeume
  6040 und 1240, Raum 20B0 (kein Weg fuehrt hinein) und bei Elza die Eintrittsszenen in 4001 und 4031.
- **Vorher und nachher:** Fuer STAGE2 bis 6 habe ich jeden Raum zusaetzlich mit dem alten Stand (v0.8.30)
  abgebildet, an derselben Stelle. Vorher fehlte der Punkt in 4 Raeumen (2020, 20A0, 5070, 5130), jetzt in
  keinem. Vorher stand der Punkt in 13 weiteren Raeumen neben seinem roten Raum, jetzt nicht mehr.
- **Kontrolle am echten Fenster:** Fuenf Raeume habe ich zusaetzlich direkt vom Bildschirm abgegriffen
  (normale Grafik, kein Testmodus). Diese Bilder sind mit den Kartenbildern bis auf den blinkenden Punkt gleich.
- **Code:** Diese Runde aendert am Spiel nichts. Neu ist nur eine Messfunktion in der Pruef-Sonde. Ich habe
  keinen Restfehler gefunden, den ich in meinem Bereich mit Beleg haette beheben koennen.

Fachwoerter: "Blatt" = Kartenseite (z. B. "Factory" oder "Laboratory B2"). "Kachel" = ein gemalter Raum darauf.
"Abzug" = Bild des Kartenschirms direkt aus dem Spielspeicher (FB_SHOT). "Riegel" = automatischer Test.
"Schleier" = unbesuchte Raeume bleiben verdeckt.

## Die Bildtafeln (fuer den Nutzer)

Ordner `analysis/karte_r42/s3/tafeln/` (Kopie unter `scratchpad/karte/s3/tafeln/`):

- `tafel_<blatt>_leon.png`, `tafel_<blatt>_elza.png` - je Blatt 0-12 und Figur eine Tafel, 26 Stueck.
  Jedes Feld: ein Abzug aus dem echten Spiel, beschriftet mit Bereich (STAGE + Blattname), Raum und Figur.
  Raeume in Durchlauf-Reihenfolge. Ein pinker Kreis zeigt, wo der Punkt ist (nur Suchhilfe, nicht im Spiel).
  Abweichungen stehen rot in der Beschriftung (z. B. "FEHLT: Raum nicht rot" bei 5090).
- `vergleich_<blatt>_leon.png` - STAGE2-6 (Blatt 6-12), je Raum links vorher (v0.8.30-Stand aaea5f8c2), rechts
  nachher, gleiche Stelle, nur dieser Raum betreten.
- `fenster_echt_<raum>_leon.png` - fuenf Abgriffe des echten Fensters (gdigrab, ohne Software-Renderer):
  2030, 30A0, 4080, 6010, 1050.

Blattnamen (von den Titelkacheln abgelesen): 0 Police Station B1, 1 B2, 2 1F, 3 2F, 4 3F, 5 Roof, 6 Drains B2,
7 Factory, 8 Laboratory B1, 9 Laboratory B2, 10 Laboratory B3, 11 Laboratory B4, 12 Subway.

## Was gegenueber vorher besser ist (STAGE2-6, gemessen an den Vorher/Nachher-Abzuegen, 61 Raeume)

1. **Punkt ueberall da:** vorher 4 Raeume ohne Punkt (2020, 20A0, 5070, 5130), jetzt 0.
2. **Punkt auf dem eigenen roten Raum:** vorher 42 von 61, jetzt 59 von 61 (Rest: 5090 Zug, 6000 Gang zu 50B0).
   Deutlich besser u. a. in 2040, 2050, 2080, 3070, 30A0, 30B0, 30E0, 4040, 4050, 5020, 5050, 50A0, 5140.
3. **Jeder Raum auf seiner eigenen Kachel:** vorher teilten sich in Drains B2 viele Raeume das L-foermige Feld
   von 2000 (2010, 2030, 2060, 2070, 2080); jetzt hat jeder Raum sein Feld (Tafel `vergleich_06_leon.png`).
4. **Gedrehte Raeume laufen richtig mit:** 3050, 30A0 und 20A0 (Achsentausch, Stufe 1).
5. **Der T-Gang 5070/5130 ist als Raum sichtbar** (vorher nur Punkt auf leerem Blau, Stufe 2).
6. **20A0 zeigt das richtige Blatt** (Drains B2) statt eines leeren Blatts ohne Titel.
7. **Treppen- und Tuerzeichen sitzen auf den gemalten Symbolen** (Stufen 1 und 2, hier in den Tafeln sichtbar).

Diese Zaehlung ist ein grobes Mass (Rot im 2-px-Rand um den Punkt). Der Beleg fuer "richtige Stelle" ist die
Auswertung unten (Punkt = Engine-Rechnung auf den Bildpunkt) und die Riegel der Stufen 1 und 2.

## Messweg (zum Nachpruefen)

- Liste: `probe_r42_karte_bestand` liefert je Kamera eine Lage (Mitte der Kamera-Region,
  `re15_rdt_get_region_quad`); `gen.py` waehlt je Raum bis zu 3 (Leon) bzw. 2 (Elza) verschiedene Lagen.
  Je Stage aufsteigende Raumnummer; alle vorher betretenen Raeume der Stage werden mit `besucht:` gesetzt.
- Lauf: `probe_r36_karte karte` schreibt den Spielstand, das echte Spiel laedt ihn (CONTINUE), oeffnet die Karte
  (`RE15_INV_OPEN_AT=40#raum`) und zieht `RE15_INV_FB_SHOT` bei Bild 96 ab.
- Erwartung: NEU `RE15_BESTAND_PUNKTE=<datei>` in `probe_r42_karte_bestand.c` - dieselbe Kette wie die Sonde
  (Raum laden, Zone, `re15_inv_map_page_shown`, `re15_inv_map_marker`), aber an der TATSAECHLICHEN Lage aus dem
  Spiel-Log (letzter `[walk]`-Eintrag bis Bild 96). Die Lage weicht oft ab (Wand-Abstoss beim Laden).
- Pruefung je Abzug: Punkt (Ringsuche) = Erwartung auf 1 px (alle 429: 0 px), Blatt = bestes Sweep-Blatt
  (Wandpixel-Vergleich, `RE15_MAP_SHOT_SWEEP`), Rot vorhanden.
- Szenen vor dem Kartenaufruf ueber Merker abgeschaltet (alle aus dem SCD des Raums gelesen):
  10F0 Port-Szene `[cut10f0]` Flag (9,71); 1170/1031 `Ck 3:193` @ROOM1170.RDT 0x01298 und `Ck 3:125` @0x012C8;
  11C0 `Ck 4:64` @ROOM11C0.RDT 0x01820; 3000/3001 `Ck 4:198` @ROOM3000.RDT 0x01568; 1011 `Ck 4:241`
  @ROOM1011.RDT 0x009E8; dazu die Merker der Vorrunde (4000, 4010, 6000, 6030). Fuenf Laeufe brauchten nur mehr
  Zeit (1010, 1180, 1181, 1250, 5060: Spieler noch in einer Bewegung, Inventar gesperrt).
- 3080: `sub00` @ROOM3080.RDT 0x007A8 startet mit und ohne `Ck 4:96` eine Szene (sub02 bzw. sub09); im Lauf mit
  1500 Bildern faehrt der Fahrstuhl und laedt ROOM4000 - die Karte ist dort nie aufrufbar.

## Pruefpunkte und Restbefunde

| Befund | Wo | Stand |
|---|---|---|
| Zug 5090/5091: Raum nicht rot an 2 von 3 Lagen, Punkt (202,98)/(199,153) auf ungemalter Flaeche | Blatt 9 r7 | NICHT angefasst (U-12, r42/rest). Ursache gemessen: die fuenf Teilbereiche (`s_map_teile` zid 84) decken Weltlagen wie (13562,5087) und (11231,-23368) nicht ab |
| 6000 Gang zu 50B0: Punkt (219,78) ueber der gemalten Flaeche | Blatt 12 r5 | offen wie in Stufe 2 (kein Beleg fuer eine andere Lage) |
| 10B0/10B1 ohne Punkt | Blatt 2 (STAGE1) | vorbestehend (Messlatte, rect 255), nicht geaendert |
| 1260/1261 ohne Punkt (keine Zone, Raumindex 38 >= 0x26 bekommt vom Stage-Init keine Seite) | Blatt 0 (STAGE1) | vorbestehend, nicht geaendert |
| 1110 kleiner Raum (Teilbereich) an einer Lage nicht rot | Blatt 3 (STAGE1) | vorbestehend (Teilbereiche) |
| Karte nicht aufrufbar | 3080/3081, 6040/6041, 1240/1241, 20B0/20B1, Elza 4001/4031 | Szenenraeume bzw. unerreichbar |

STAGE1 habe ich bewusst nicht veraendert: Es ist die Messlatte, und die drei Punkte oben standen schon in der
Bestandsliste von Stufe 1.

## Tests

- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan)" -j 6`: 52/52 gruen.
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan)" -j 4`: 13/13 gruen (u. a. integration_map_raum_live).
- Kein neuer Riegel (keine Verhaltensaenderung). Die Sonden-Erweiterung ist ein Messwerkzeug ohne add_test.
- Volle Suite nicht gelaufen (Auftrag).

## Kollisionen

`git merge-tree --write-tree` r42/karte gegen r42/finale 0d2551909, r42/gegenstaende 5515f2601, r42/gore
be62608c0, r42/rest c5ac41ca7, r42/story 5849c8996, r42/lageplaene 10287b077, r42/original a6260d207,
r42/birkinfilm d026e5f95: **kein Konflikt**. Inhaltlich beruehren nur r42/rest (+4 Zeilen re15_map_owned.c, U-11)
und r42/gegenstaende (+4 Zeilen re15_inv_screen.c) die Kartendateien, an anderen Stellen.

**Vorwarnung U-12 (r42/rest):** 5090/5091 ist der einzige Raum STAGE2-6 mit sichtbarem Fehler (nicht rot, Punkt
neben der Kunst). Wer den Zug auf die Wagenkacheln von Blatt 12 umzieht, sollte die fuenf Teile und Innenwaende
mit zid 84 auf Blatt 9 r7 mitnehmen (siehe Bericht Stufe 2 Runde 2); `unit_r42_karte_sichtbar` [K] prueft 5090
auf r7 und muss dann mit umziehen. Die Abzuege `L5090_*`/`E5091_*` in `analysis/karte_r42/s3/abzuege_s3.tsv`
sind die Messlage dafuer.

## Dateien

- Code: `re15_port/tests/unit/probe_r42_karte_bestand.c` (Messmodus `RE15_BESTAND_PUNKTE`).
- Daten: `analysis/karte_r42/s3/abzuege_s3.tsv` (alle 464 Laeufe), `auswertung2.txt`, `vergleich.txt`,
  `fenster_abgleich.txt`, `kampagne.txt`.
- Werkzeuge: `analysis/karte_r42/s3/werkzeuge/` (gen.py, schuss1.sh, kampagne.sh, auswerten.py, tafel.py,
  tafel_vgl.py, vergleich.py, fenster.sh, vergleich_fenster.py).
- Tafeln: `analysis/karte_r42/s3/tafeln/` (26 Durchlauf-, 7 Vergleichs-, 5 Fensterbilder).
- Arbeitsordner: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3/`.
