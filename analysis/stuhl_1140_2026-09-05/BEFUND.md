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
