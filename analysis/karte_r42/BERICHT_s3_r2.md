# Bericht Strang "karte", Stufe 3 (Durchlauf und Bildtafeln), Runde 2

Zweig r42/karte, Code-Commits d637ad25a, 12930b25b, 821a8b120, danach ein Doku-Commit mit diesem Bericht. Kein Push.

## Kurz und in Alltagssprache

- **Die Pruefung der Vorrunde hatte recht.** Ich hatte nur geprueft, ob der Punkt dort steht, wo das Spiel ihn
  selbst ausrechnet. Und viele Messstellen waren Orte, an denen Leon gar nicht stehen kann.
- **Neu gemessen, nur an echten Stellen.** Jede Messstelle ist jetzt eine Stelle, an die das Spiel die Figur
  wirklich hinstellt: direkt hinter einer Tuer, nachdem man hindurchgegangen ist. Dazu kommen Stellen, an die Leon
  von dort aus 2 Sekunden gelaufen ist. Insgesamt sind es 611 Bilder aus dem echten Spiel, Leon und Elza.
- **Neu ist auch, wie ich pruefe.** Ich schaue nicht mehr, ob das Bild zur Rechnung passt. Ich schaue im Bild
  selbst, ob der Punkt auf dem roten Raum steht. Dafuer nehme ich genau das Mass des Pruefers: Rund um den Punkt
  muessen mindestens 30 % rot sein.
- **Ergebnis STAGE2 bis 6:** In 334 von 377 Bildern steht der Punkt auf dem roten Raum. Der Rest ist unten
  einzeln aufgefuehrt: der Zug 5090 (12 Bilder), 15 Grenzfaelle an Tueren, 16 Bilder, die das Spiel gar nicht
  zeigen kann.
- **Vorher und nachher, an denselben 144 Stellen (Leon, STAGE2-6):** Mit dem alten Stand v0.8.30 stand der Punkt
  in 85 Bildern auf dem roten Raum, jetzt in 132. In 51 statt 28 von 58 Raeumen stimmt jetzt jede Stelle.
- **Was ich repariert habe:**
  1. Der Punkt kann nicht mehr neben dem gemalten Raum stehen. Bisher wurde er nur in das Rechteck um den Raum
     gezwungen. Viele Raeume sind aber L- oder U-foermig. Jetzt kommt er auf die naechste Stelle, die wirklich
     zum Raum gehoert, nicht auf die Wandlinie. Betroffen waren zum Beispiel 6000 (Ankunft aus der Fabrik),
     4040, 4050, 4080 und 11A0.
  2. Raum 4080 leuchtet jetzt ganz. Er ist im Original auf zwei Kacheln gemalt. Kam man aus 4030, leuchtete nur
     ein duenner Strich. Jetzt leuchten beide Teile. Das gilt genauso fuer 4040 und 5140.
- **Was ich nicht repariert habe, mit Grund:** 10B0 und 1260 haben im Original weder ein gemaltes Feld noch eine
  Kartenseite. Dafuer muesste ich etwas Neues zeichnen (Frage unten). Der Zug 5090 gehoert zum Zweig r42/rest.
- **Tafeln fuer dich:** Je Kartenseite und Figur zeigt eine Tafel jetzt **alle** Bilder, nicht nur das erste
  gute. Darunter steht der Bereich, der Raum, die Figur und woher die Figur kam. Abweichungen stehen rot darunter.

Fachwoerter: "Blatt" = Kartenseite. "Kachel" = ein gemalter Raum auf dem Blatt. "Abzug" = Bild des
Kartenschirms direkt aus dem laufenden Spiel (FB_SHOT). "Riegel" = automatischer Test. "Gegenprobe" = derselbe
Test auf dem alten Stand muss fehlschlagen. "Ankunft" = die Stelle hinter einer Tuer, an die das Spiel die Figur
stellt.

## Die roten Punkte der Vorrunde

| Pruefpunkt | Stand jetzt |
|---|---|
| 1. 25 Bilder STAGE2-6 mit Punkt neben dem Raum | Neu gemessen an echten Stellen. Die alten Stellen (31500,31500), (-5000,-6500) usw. kommen nicht mehr vor. Uebrig: 15 Grenzfaelle, Liste unten. |
| 2. Messung bestaetigt sich selbst | Ersetzt. Urteil nur noch aus dem Bild (roter Rand um den Punkt, Mass des Pruefers aus `pruef_s3_r1/py/eigen.py`). Zusaetzlich fuer Raeume mit Original-Zeile: Abstand zur Original-Formel, Zeile direkt aus PSX.EXE gelesen. Der Riegel prueft gegen die Texel aus DATA/MAP0x.PIX. |
| 3. Kopf widerspricht der Liste | Kopf und Liste "offen" nennen dieselben Faelle (unten). |
| 4. Tafeln zeigen nur das erste gute Bild | Jetzt alle 611 Bilder auf 48 Tafeln, jede Abweichung rot beschriftet. Bereich kommt aus der Raumnummer (5080 = STAGE5). |
| 5. Nichts behoben | Drei Code-Commits, ein neuer Riegel mit drei Teilen, Gegenprobe rot auf aaea5f8c2 und 6e0f3db61. |
| 6000 | behoben (Punkt an der Tuer nach 50B0 im Raum, nicht mehr 8 px darueber). Grenzfall bleibt: 29 % rot, weil die Stelle direkt an der Wand liegt. |
| 6020 | kein Fehler an echten Stellen: alle Ankuenfte und Laeufe ok (4 Bilder). Der alte Fehlschuss lag bei (31500,31500) = keine Stelle im Raum. |
| 4080 | behoben (ganzer Raum rot, siehe 2. oben). Zwei Grenzfaelle mit 29 % (Punkt im schmalen Diagonalstueck bzw. an der Tuer zu 40B0). |
| 10B0 / 1260 | begruendet, nicht gebaut (siehe unten), Frage an den Nutzer. |

## Was geaendert ist (PORT-WAHL, jede Konstante mit Beleg)

1. **Punkt auf den Raumkoerper der eigenen Kachel** (`engine/src/karte_r42.c`, `include/re15_karte_r42.h`, Haken in
   `re15_inv_map_marker`). Nach der Rechteck-Klemmung wird die sichtbare Ring-Mitte des Punkts auf den naechsten
   Texel des Raumkoerpers gesetzt.
   - Raumkoerper = Index != 0 und != 4 in DATA/MAP0x.PIX an der Kachel-uv der Seitentabelle @0x80076840. Index 4 ist
     die Wandlinie (CLUT 0x5ad6, Beleg Tuer-Nische an DATA/MAP05.PIX in `re15_inv_screen.c`).
   - Ring-Mitte = (mx-1, my-1): TEX.TIM @0x14910, dieselbe Konstante wie `KF_GLYPH_MITTE`. In allen 611 Abzuegen
     liegt die Ring-Mitte genau dort.
   - Daten erzeugt `tools/gen_map_kunst.py` (-> `engine/src/re15_map_kunst.h`, 105 Kacheln, 4305 Spannen). Es gibt
     keine Schwelle und keine freie Zahl.
   - Nur auf Blatt 6-12. STAGE1 ist die Messlatte, und dort zeichnet der Port die Kacheln nach den Runde-39-Vorlagen
     um, die PIX-Maske ist dort also nicht das gezeigte Bild.
   - Nicht fuer Zonen mit der AUSGELIEFERTEN Zeile (@0x800768b0 + 8*(Basis+Raum), Basen wie FUN_8004b568). Dort
     bleibt die Original-Formel byte-true.
   - Nicht fuer Zonen mit eigenen Teilbereichen (Zug 5090, U-12).
2. **Ein Ort auf zwei Kacheln ist ganz rot** (`re15_map_zones.c rect_state_impl`, Blatt 6-12). Zonen desselben Raums
   mit derselben Zeile auf demselben Blatt sind ein Ort: 4040 r4+r12, 4080 r8+r13 und 5140 r10+r12 (AUSGELIEFERTE
   Zeile @0x80076BB8). Steht die Figur in einer, sind beide Kacheln aktuell.
3. **Sonde** `probe_r42_karte_bestand`: neue EIN-Zeilen (Ankunftslage und Blickrichtung je Tuer-Datensatz).

## Riegel und Tests

- NEU `unit_r42_karte_ankunft` (`tests/unit/test_r42_karte_ankunft.c`). Er nimmt 399 Ankunftslagen aus allen
  Tuer-Datensaetzen STAGE1-6, Leon und Elza.
  - [A] Hergeleitete Zeile, Blatt 6-12: Die sichtbare Punktmitte liegt auf dem Raumkoerper der eigenen Kachel.
    Die Texel werden direkt aus MAP0x.PIX gelesen. Neu **124/124**.
  - [B] Ausgelieferte Zeile: nur gemeldet, 93/109. Die Abweichungen liegen an Tueren bei 1-2 px oder sind
    vorbestehend (6030-Zeile ausserhalb r7, siehe st456-Ausnahme).
  - [C] Ein Ort auf zwei Kacheln: Beide Kacheln sind rot. Neu **12/12**.
  - GEGENPROBE ROT: aaea5f8c2 [A] 54/114, [C] 0/4. 6e0f3db61 [A] 106/124, [C] 0/12. Dateien:
    `analysis/karte_r42/s3r2/gegenprobe_ankunft_*.txt`. Der Baum fuer aaea5f8c2 kommt aus `git archive`. Fuer
    6e0f3db61 habe ich die beiden Engine-Dateien des alten Stands eingesetzt und danach wiederhergestellt.
- `unit_r42_karte_st456`:
  - [B]-Ausnahmen 4040/4080/5070/5130/5080/6000 sind gestrichen. Sie bestehen jetzt, der Test ist also strenger.
  - Zwei neue [C]-Ausnahmen mit Messwert:
    - 4080 Zone 1 (Diagonalstreifen r13): 13 von 28 Rasterpunkten an verschiedenen Stellen statt 14.
    - 4020 Fahrstuhlkabine: Gemalt ist nur 12x12 der 16x16-Kachel (MAP09.PIX Blatt 8 r2). Ergebnis 20/49.
  - Beide Male liegen Rasterpunkte ausserhalb der gemalten Flaeche. Sie landen jetzt am Innenrand statt auf
    leerem Blau.
- `ctest -R "unit_.*(map|karte|kartenbits|inv_screen|lageplan)" -j 6`: **53/53** auf 821a8b120.
- `ctest -R "integration_.*(map|karte|kartenbits|lageplan)" -j 4`: **13/13** auf d637ad25a und auf dem Endstand
  821a8b120, darunter integration_map_raum_live.
- Die volle Suite ist nicht gelaufen (so im Auftrag).

## Kartenlauf (alle Bilder aus dem echten Spiel)

So bin ich vorgegangen:
- Die Liste erzeugt `gen2.py` aus den EIN-Zeilen. Pro Raum gibt es jede Ankunft (`tN`). Fuer Leon kommen bis zu
  zwei Laeufe dazu (`wN`): von der Ankunft aus 2 s vorwaerts rennen, mit echter Kollision.
- Je Stage gehe ich die Raeume aufsteigend durch. Vorher betretene Raeume sind als besucht markiert.
- Szenen sind ueber Merker abgeschaltet, wie in Runde 1.

Ergebnis (`analysis/karte_r42/s3r2/auswertung.txt`, `summe.txt`, `abzuege_s3r2.tsv`):

| | ok | Punkt neben Rot | Raum nicht rot | kein Punkt | kein Abzug |
|---|---|---|---|---|---|
| STAGE1 (234) | 211 | 3 | 0 | 14 (10B0, 1260) | 6 |
| STAGE2-6 (377) | 334 | 15 | 12 (Zug 5090) | 0 | 16 |

- **Punkt laeuft mit:** Es gibt 127 Laeufe mit mindestens 1000 Einheiten Weg. In 122 davon bewegt sich der
  Punkt um 2 px oder mehr.
  - Er bleibt fast stehen bei: 20A0_w0 (1 px), 4040_w1 (1 px), 4070_w0 (1 px), 4080_w1 (0 px), 6000_w0 (0 px).
  - Bei 6000 ist das gewollt: Der Gang nach 50B0 ist nicht gemalt, der Punkt bleibt an der Tuer.
  - Bei den uebrigen vier lief Leon im Raum fast nur quer zur kurzen Achse der Kachel. Das ist nicht
    nachgemessen, also offen.
- **Original-Formel** (Raeume mit ausgelieferter Zeile, 237 Abzuege): 158 genau, 23 mit 1 px, 13 mit 2 px.
  Groessere Abstaende haben vorbestehende Gruende:
  - 30E0 Zone 0 rechnet bewusst mit eigener Zeile (Bestand Stufe 1).
  - 1170, 1190, 11A0 und 11B0: andere Zone bzw. RE2-Massstab B1 (STAGE1-Messlatte).
  - 1030 ist eine PORT-KORREKTUR.
  - 6030: Die Zeile @0x80076BD8 projiziert neben r7, deshalb klemmt der Port (st456-Ausnahme).
  - 4000: Gang r14 ungemalt.
- **Kein Abzug** (Karte im Spiel nicht aufrufbar):
  - Szenenraeume: 6040/6041, 1240/1241, Elza 4001 und 4031.
  - Laeufe, in denen Leon in eine Bewegung geriet (letzte Bewegung mo=4/8/9/210, Inventar gesperrt): 50B0_w0/w1,
    6030_w0/w1, 1010_w1, 1220_w0, 6000_w1, 40A0_w0.
  - E1041_t3.

## Was noch offen ist (dieselbe Liste wie im Kopf)

1. **15 Grenzfaelle STAGE2-6** (weniger als 30 % Rot um den Punkt, alle 20-29 %). Der Punkt steht auf der Kachel,
   aber an deren Rand:
   - 6000 aus 50B0, Leon und Elza (Ankunft im ungemalten Gang, Punkt an der Tuer, 29 %).
   - 4080_t0 (schmaler Diagonalstreifen r13, 29 %) und 4080_t3 (Tuer zu 40B0, 29 %), je Leon und Elza.
   - 4070_t0/w0 und Elza (Treppenhaus-Kachel, Punkt auf dem gemalten Treppengitter, 20-25 %). Vorher waren es
     45 %. Der Punkt steht an derselben Stelle, das Rot drumherum ist weniger geworden, weil die Stufe-2-
     Treppenmarke dort liegt. Nicht weiter untersucht.
   - Mit ausgelieferter Zeile, byte-true und unveraendert: 2000 aus 1260 (20 %), 3090_w1 (29 %), 6030 aus 6000 (29 %).
2. **Zug 5090/5091** (12 Bilder): Der Punkt steht auf dem Zug. Der Zug leuchtet aber im Farbton der Teilbereiche
   (braunrot), den mein Rot-Filter nicht als Raumfarbe zaehlt. Auf aaea5f8c2 waren 4 der 5 Leon-Stellen rot.
   Das ist U-12 in r42/rest, ich habe es nicht angefasst. Vorwarnung unten.
3. **10B0/10B1 und 1260/1261 ohne Punkt** (STAGE1). Begruendung mit Adresse:
   - 10B0: Zeile Slot 0x0B @0x80076908 = `00 00 00 00 01 00 01 00` = Stub {0,0,1,1}. Das Original zeigt Blatt 2
     (1F) ohne Punkt, und der Port tut dasselbe.
   - 1260: Raumindex 0x26 liegt hinter der Grenze des Seiten-Setzers (`sltiu v0,v1,0x26` @0x8004b574 =
     `26 00 62 2c`, `beq` @0x8004b578). Das Original setzt dort keine Seite. Sein Zeilenindex 38 ist die Zeile
     von ROOM2000 (@0x800769E0).
   - Keines der beiden Raeume hat eine gemalte Kachel. Unbenutzte Kacheln gibt es nur auf Blatt 0 (die 7
     Original-B1-Kacheln, ersetzt durch die Runde-39-Schemazeichnung), Blatt 3 r2, Blatt 8 r14 und die
     U-Bahn-Wagen auf Blatt 12.
   - 1260 ist ein grosser Ringgang (124x97 px im Stage-Massstab). Seine Tueren nach 11B0 und 11D0 passen nicht zu
     deren Lage auf dem B1-Schema (74/103 px gegen 13/39 px Abstand). Eine Zeichnung waere also frei erfunden.
4. **STAGE1-Restfaelle**:
   - 11B0 Ankunft aus 1180 und ein Lauf (Leon und Elza, 25 %, B1-Schemazeichnung).
   - 1090 Ankunft aus 10F0: Punkt 5 px auf der 1050-Kachel. Das hat die Vorpruefung gefunden. Im Bild ist es noch
     ok (Rot vorhanden), aber es liegt auf der Nachbarkachel.
   - Ich habe STAGE1 nicht geaendert, es ist die Messlatte.
5. **Mitlaufen fast still** bei 20A0_w0, 4040_w1, 4070_w0 und 4080_w1 (je 0-1 px bei 1182-3191 Einheiten Weg).
   Nicht untersucht.

## Bilder

Alle Bilder liegen in `analysis/karte_r42/s3r2/tafeln/` (Kopie `scratchpad/karte/s3r2/tafeln/`):
- `tafel_<blatt>_<figur>[_<teil>].png`: 48 Tafeln, alle 611 Abzuege. Jedes Feld traegt diese Beschriftung:
  - STAGE (aus der Raumnummer), Blattname, Raum und Figur;
  - "Ankunft aus XXXX" oder "gelaufen ab Tuer aus XXXX" mit der Lage;
  - Abweichungen in roter Schrift. Der pinke Kreis ist nur eine Suchhilfe.
- `fenster_echt.png`: vier Abgriffe des echten Spielfensters (gdigrab, normale Grafik): 6000, 4080, 4040, 6020. Sie
  sind mit dem FB_SHOT desselben Laufs bis auf den blinkenden Punkt gleich (99,98 %, 52 px).
- Messbild `analysis/karte_r42/s3r2/messbild_vorher_nachher_rest.png`: vorher/nachher der Restfaelle.

## Kollisionen

`git merge-tree --write-tree` r42/karte gegen r42/rest f7392eb72, r42/gegenstaende 5515f2601, r42/finale 0cc6a248e,
r42/gore b8b879d54, r42/story 5849c8996, r42/lageplaene 10287b077, r42/original a6260d207 und r42/birkinfilm
d026e5f95: **kein Konflikt**. Inhaltlich beruehren nur zwei Zweige die Kartendateien, beide an anderen Stellen:
r42/rest (+4 Zeilen re15_map_owned.c) und r42/gegenstaende (+4 Zeilen re15_inv_screen.c).

**Vorwarnung U-12 (r42/rest):**
- Die neue Klemme laesst Zonen mit eigenen Teilbereichen bewusst aus, also den Zug 5090 auf Blatt 9 r7.
- Zieht r42/rest den Zug auf die Wagenkacheln von Blatt 12 um (r1-r4, bisher ohne Zone), greift dort die Klemme,
  sobald die Wagen eine hergeleitete Zeile tragen. Dann muss `tools/gen_map_kunst.py` neu laufen, sonst fehlen die
  Wagenkacheln in `re15_map_kunst.h` und die Klemme tut nichts.
- `unit_r42_karte_ankunft` laesst 5090/5091 aus.

## Dateien

- Code:
  - `re15_port/engine/src/karte_r42.c`, `re15_port/engine/src/re15_map_kunst.h` (erzeugt),
    `re15_port/include/re15_karte_r42.h`, `re15_port/tools/gen_map_kunst.py`;
  - Haken in `re15_inv_screen.c` und `re15_map_zones.c`, `include/re15_room.h`.
- Riegel: `re15_port/tests/unit/test_r42_karte_ankunft.c` (+ `tests/unit/probes/r42_karte.cmake`),
  `test_r42_karte_st456.c`.
- Daten und Werkzeuge: `analysis/karte_r42/s3r2/` (Werkzeuge in `werkzeuge/`).
- Arbeitsordner: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r2/`.
