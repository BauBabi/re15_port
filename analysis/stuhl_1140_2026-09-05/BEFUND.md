# ROOM1140 Cut 0, der Stuhl — was gemessen ist und wo es reisst

Nutzer-Befund 2026-09-05 (`fehler/error06.png`): "der Stuhl ueberdeckt mich immernoch
nicht." Vorgeschichte: im Durchgang davor "when I get closer to the chair, it is not in
front of me anymore"; darauf hatte ich `ebene: -700` gesetzt (v0.6.2).

## Was NICHT die Ursache ist — jeweils gemessen

**Die Lage der Freistellung.** `pri/1140/00_02.png` (23x28) passt an GENAU EINER Stelle
zum Hintergrund: x=210, y=113 mit 99,2 % Uebereinstimmung; keine zweite Stelle kommt
ueber 95 %. Ein Kasten aus fast einfarbiger Wand haette viele Treffer — dieser nicht.

**Der Ladeweg.** Die Sonde `RE15_PRI_LOG` meldet in diesem Cut durchgaengig
`Atlas 1 Sektion 86 | 86 Masken`. Atlas vorhanden, Sektion geparst, alle 86 Rechtecke
gezeichnet.

**Der Atlas.** Punktgenau geprueft: an ALLEN 53 Bildpunkten (320x240-Raster), an denen
Leon durch die Stuhlflaeche scheint, traegt `ROOM1140_PRI00.TIM` einen zeichenbaren
Index (0 waere durchsichtig). Kein einziger Punkt faellt aus.

**Die Verdeckung im Spiel.** In der Naehe des Stuhls meldet die Sonde
`verdeckend 86 von 86` — die Maske wirkt dort.

## Was der Nutzer sieht — und dass es echt ist

Differenz seines Bildes gegen einen maskenlosen Referenzrender desselben Cuts:

    Punkte "Leon" auf der Stuhl-Silhouette : 361 (3x-Raster)
      Differenzstaerke dort                : Median 191 (10..90 %: 73..396)
      Leon frei stehend                    : Median 151
      Stuhlflaeche OHNE Leon (reine 8-Bit-Quantisierung des Atlas) : Median 10

⛔ Der letzte Wert ist die Kontrolle gegen meinen eigenen Messfehler: der Referenzrender
lief OHNE Masken, die Maske malt aber aus einem quantisierten Atlas, erzeugt also selbst
Differenzen. Die sind mit Median 10 eine Groessenordnung kleiner. Was dort steht, ist
wirklich Leon.

## Wo meine Rekonstruktion reisst

Um zu entscheiden, ob die Maske zu FERN liegt, brauche ich Leons Tiefe. Aus dem Bild
allein bekomme ich sie nicht widerspruchsfrei:

* Sein Kopf steht auf Bildzeile ~68, seine sichtbare Unterkante auf 121.
* Steht er mit den Fuessen auf Bildzeile r, hat er die Tiefe `vz(r)` und ist
  `H*1500/vz(r)` Zeilen hoch (H = 208 in diesem Cut). Fuer KEIN r zwischen 122 und 218
  faellt der Kopf auf 68 — der naechste Wert ist 96 bei r=122, und mit wachsendem r
  wird es schlechter.
* Aus der Bildhoehe allein (>= 53 Zeilen) folgt vz <= 5887; aus der Fusszeile 122 folgt
  vz 12198. Beides zusammen geht nicht auf.

Eine der Annahmen ist also falsch (Spielerhoehe 1500, Fusspunkt, oder meine Zuordnung
der Differenz-Komponente zu Leon). Solange das nicht steht, waere jede Aenderung an der
Stuhltiefe geraten.

## Die Tiefen, um die es geht

    Konferenztisch  Kamera-z 3328..5824
    Stuhl           Kamera-z 7232..7488   <- ebene -700 (Tischplatte)
    Karaffe         Kamera-z 6912..9536

Zum Vergleich: mit der BODENebene statt -700 bekaeme der Stuhl 8704 — also noch WEITER
und damit noch weniger Verdeckung. Die jetzige Einstellung ist bereits die naehere.

## Was ich zum Abschluss brauche

Die Stelle, an der es auftritt — am besten zwei Bilder: eines dort, wo der Stuhl NICHT
deckt, und eines einen Schritt weiter Richtung Tuer. Daraus laesst sich die Tiefe
eindeutig bestimmen. Testbare Aussage bis dahin: der Stuhl deckt, sobald man WEITER von
der Kamera weg steht als er — also zwischen Stuhl und Doppeltuer. Deckt er dort nicht,
ist es ein Fehler; deckt er nur neben/vor sich nicht, ist es richtig.

## Warum ich es nicht selbst nachstellen konnte

Der Debug-Sprung nach ROOM1140 landet in der **Irons-Zwischensequenz**; der Spieler ist
dort minutenlang nicht steuerbar. Nach ihrem Ende steht er in Cut 2. Der Autopilot
(`RE15_AUTOPILOT="xz:..."`) bringt ihn in die Naehe, aber nicht auf die Stelle des
Screenshots; drei Anlaeufe mit 45-115 s Laufzeit.

---

# Zweiter Durchgang, 2026-09-06 — sechs neue Nutzer-Bilder

## Das Wichtigste zuerst: es gibt hier kein `@0x`

Der Port liest die nachgezeichnete Maskendatei **nur dort, wo die RDT-Sektion
`FFFFFFFF` fuehrt** (`platform/pc/main.c`, "NUR wenn das Original eine NULL-Sektion
fuehrt"). Fuer ROOM1140 Cut 0 ist genau das der Fall — im Container
`MASKS/ROOM1140.MSK` traegt Cut 0 die 86 Rechtecke, das Original traegt nichts.

**Der Stuhl ist also meine Rekonstruktion.** Seine Tiefe hat keine Original-Adresse;
sie stammt aus `auswahl.json` (`ebene: -700`, Tischplattenebene). Das gehoert offen
gesagt, statt eine Herleitung wie einen Beleg klingen zu lassen.

Maskensatz des Raums (aus dem Container gelesen):

| Cut | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|-----|---|---|---|---|---|---|---|---|---|---|
| Masken | 86 | – | 84 | 71 | – | 89 | – | – | – | – |

Sechs der zehn Kamerawinkel haben ueberhaupt keine Maske. Eigene Luecke.

## Die Maschinerie arbeitet — gemessen, nicht angenommen

* `[pri] cut=0 pri_offset=0x51C masks=86 fg_atlas=1` — alle 86 geladen; der Deckel
  `RE15_PRI_MAX_MASKS_PER_CUT` ist 105, es faellt nichts weg.
* Neue Sonde (`RE15_POCC`, um Huefte und Kopf erweitert): am Spawn
  `pl=(-7600,0,-17600) vz=11034 vzHuft=10800 vzKopf=10565`. Die Figur spannt also nur
  **469 Einheiten** in der Tiefe — eine Maske kann sie nicht „waagerecht durchschneiden",
  solange sie nicht genau in diesem Band liegt.
* Nachgestellt bei `scr=(222,142) vz=9460`: der Stuhlrahmen laeuft im Bild
  **ungebrochen** ueber ihre Beine (Frame-Abzug `m9/F2000_stuhl.png`).

## Die Stuhl-Rechtecke

    x=210..226  y=113..141   depth=117  -> Kamera-z 7488
    x=226..233  y=113..141   depth=113  -> Kamera-z 7232

## Wo die Grenze wirklich liegt

Aus dem Bodengitter (`RE15_POCC_SCAN`, 539 begehbare Punkte im Maskenbereich): von den
**153** Standplaetzen, deren Figur den Stuhl auf dem Bild ueberlappt, liegen

* **84 dahinter** (vz > 7488) — der Stuhl deckt, richtig;
* **69 davor**   (vz < 7488) — der Stuhl deckt nicht, ebenfalls richtig.

Ein Schritt ueber diese Linie schaltet die Verdeckung um. Das erklaert, warum in
`233105/233113/233123` gedeckt wird und in `233130/233139` nicht — **ohne** dass ein
Fehler vorliegen muss. Ob 7488 die richtige Linie ist, ist die eigentliche Frage.

## Was NICHT die Ursache ist (jeweils ausgeschlossen)

* **Falscher Cut.** Hintergrundabgleich der Nutzerbilder gegen alle zehn Winkel:
  Cut 0 mit Abweichung 8.3, naechstbester Cut 8 mit 26.3.
* **Fehlende Dateien im Paket.** `re15_port_v0.6.5_win64.zip` enthaelt
  `MASKS/ROOM1140.MSK` und `ROOM1140_PRI00.TIM`.
* **Das Inventar.** Mit dem v0.6.4-Riegel bleibt die Liste stehen: 137 Sonden-Zeilen
  im selben Raumbesuch, durchgehend „86 Masken".

## Was offen bleibt

1. ⛔ **Meine Belege sind Frame-Abzuege.** In diesem Projekt ist dokumentiert, dass
   Abzuege Fehler verdecken koennen (Skill `re15-port-visual-verify`). Der Mitschnitt
   des ECHTEN Fensters ist noch nicht gelungen: im Live-Fenster loeste der Debug-Sprung
   nach ROOM1140 nicht aus (F20889 in ROOM1170), und ohne ihn kommt der Messlauf nicht
   in den Raum.
2. **Die Weltlage des Nutzers.** Aus dem Bild allein nicht bestimmbar; der Abgleich
   gegen den Hintergrund traegt nicht weit genug (Ausrichtungs-Rest 5-9 gegen ~0 bei
   einem eigenen Abzug). `RE15_POS_HUD=1` liefert sie in einer Zeile.
