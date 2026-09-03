# Nutzerbefunde an v0.4.2 (error01/02/03) — Ursache, Behebung, Methodenaenderung

Datum: 2026-09-03 · Belege: `error01.png`, `error02.png`, `error03.png` (Repo-Wurzel,
aus dem Test von v0.4.2). Alle drei liegen in ROOM1140.

## 0. Was der Nutzer gesehen hat

| Bild | Cut | Symptom |
|---|---|---|
| error01 | 2 | Ein Streifen **Wandtextur** liegt ueber Leons Schulter und Ruecken, waehrend er vor der Sitzbank steht |
| error02 | 1 | Ein **schwarzer Kasten** (Stuhl-/Teppichpixel) liegt ueber Leons Oberkoerper und Oberschenkel im Gang |
| error03 | 5 | Leon wird **UEBER die Anschlagtafel** gezeichnet, obwohl er dahinter steht |

## 1. Ursachen (jede am Bild nachgemessen, nicht vermutet)

**error01 — Wand in derselben Superpixel-Flaeche wie das Objekt.**
Das Auswahlblatt lief mit 90 Flaechen. Bei dieser Groebe lagen Wandbild, Wand,
Sitzbank und Kamerastativ in EINER Flaeche (Nr. 30). Sie war nur ganz oder gar nicht
waehlbar — gewaehlt hiess: die Wand verdeckt. Dazu kommt, dass die Tiefe je
Bildspalte aus dem UNTERSTEN Punkt der Zusammenhangskomponente kommt: die Wandpixel
erbten damit die nahe Tiefe des Stativfusses (69 statt der Wandtiefe), und ein
Wandstreifen mit Tiefe 69 verdeckt jede Figur ab Kamera-Z 4416.

**error02 — die Spaltenfuellung ueberbrueckte zwei Objekte.**
Die Nachbearbeitung fuellte je BILDSPALTE alles zwischen oberstem und unterstem
markierten Pixel. In einer Spalte liegen aber Stuhl (oben, fern), Teppich, Stuhl
(unten, nah). Der Teppich dazwischen wurde markiert und bekam die nahe Tiefe der
unteren Kante — genau der Gang, in dem Leon geht.

**error03 — das Objekt fehlte schlicht.**
Cut 5 hatte nur den Konferenztisch. Anschlagtafel, Schrank darunter und der
Schreibtisch dahinter waren nicht ausgewaehlt.

## 2. Warum es durch die Abnahme kam

Geprueft wurde die **Auswahl** (`pruefblatt.py`, Superpixelnummern ueber dem Bild),
nicht das **Ergebnis**. Alle drei Fehler entstehen erst NACH der Auswahl (Aufweitung,
Spaltenfuellung, Rechteckzerlegung, Tiefe je Komponente) oder sind eine Auslassung —
im Ergebnisbild sind alle drei auf den ersten Blick sichtbar.

## 3. Was geaendert wurde

* `maskenbild.py` (neu) — zeichnet, was der Renderer TATSAECHLICH verdeckt: die
  Atlas-Pixel an ihrer Zielstelle, je Rechteck mit seiner Tiefe, plus Koordinatengitter.
  Das ist ab jetzt die Abnahme je Cut.
* `raum.py` (neu) — erzeugt einen ganzen Raum aus der Auswahldatei UND rendert dabei
  die Ergebnisbilder. Auswahl und Abnahme koennen nicht mehr auseinanderlaufen.
* `original.py` (neu) — zeigt die KUENSTLER-Masken (Rechtecke aus dem RDT, Pixel aus
  dem SLD-Atlas des BSS-Chunks). Der Massstab, an dem sich Nachgezeichnetes misst.
  Befund daraus: die Kuenstler benutzen viele kleine Rechtecke mit **je eigener Tiefe**
  (ROOM1150 Cut 2: 54 Rechtecke, 22 verschiedene Tiefen).
* **Spaltenfuellung jetzt je Zusammenhangskomponente** statt global. Gemessen gegen die
  Kuenstler-Silhouetten (`kalib.py`, ROOM1150 Cut 1/2/3, Auswahl als perfekt angenommen,
  seg 220, oben 4, Saum 1): global 65.9 % Praezision / 93.0 % Ausbeute, Komponente
  70.6 % / 89.8 %, aus 72.1 % / 88.2 %.
* **Feinheit 220 statt 90** als Standard (gemessen: 67.2 %/89.0 % gegen 70.6 %/89.8 %),
  und `--minus`/`--plus`-Kaesten, um Wand- oder Bodenanteile aus einer zu groben Flaeche
  herauszuschneiden, ohne sie ganz aufzugeben.
* Auswahldatei traegt jetzt die Feinheit MIT ("gruppen"): eine Auswahl ist nur zusammen
  mit ihrer Segmentierung gueltig. Die alten `.npy` liessen sich nicht reproduzieren,
  weil die benutzte Flaechenzahl nirgends stand (sie war 90, der Werkzeug-Standard 80).
* `tiefenkalib.py` (neu) — misst den Tiefenfaktor DIREKT gegen 5151 Kuenstler-Tiefen
  statt gegen ein Ersatzmass. Ergebnis: 0.90 ist das Optimum (Medianfehler +2 von rund
  80). Die frueher hier stehende Begruendung war in sich widerspruechlich und wurde
  ersetzt.

## 4. Stand ROOM1140 danach

| Cut | Inhalt der Maske | Aenderung |
|---|---|---|
| 0 | Konferenztisch + Stuehle | unveraendert |
| 1 | Tisch + Stuhlreihe | Spaltenfuellung je Komponente — Gang frei |
| 2 | Tisch, linker Schrank + Stativ, Bank + rechtes Stativ | Wandband ueber der Bank ab y=104 abgezogen |
| 3 | Pult, Fahnen, Seitenschraenke | unveraendert |
| 4 | Wandtafel, Seitentisch, Konferenztisch | Wandband und Gangteppich abgezogen |
| 5 | Tisch **+ Anschlagtafel + Schrank + Schreibtisch** | Objekte ergaenzt (Feinheit 220) |
| 6 | — | absichtlich leer (Tuer-Nahaufnahme) |
| 7 | Tisch + Stuhl | Fensterwand abgezogen |
| 8 | — | absichtlich leer |
| 9 | Schreibtisch, Rechner, Stuhl | Wand ueber dem Bildschirm und Wandecke abgezogen |

## 5. Verifikation

* Deckungsbild je Cut angesehen (`build/maskenbild/ROOM1140_*_maske.png`).
* Im laufenden Spiel per gdigrab (echtes Fenster, kein Autoshot, kein Software-Render):
  Save laden -> Debug-Sprung ROOM1140 -> Gang ablaufen. In Cut 1 liegt kein Kasten mehr
  ueber Leon oder den Zombies (`shots/pri/n_*.png`).
* `ctest`: 267/267.
