# Unverdrahtete Untersuchungstexte ROOM1070/1071 und ROOM1120/1121 — wo sie hingehören

Stand 2026-10-23. Ausgangspunkt: Zensus STAGE1/2. 1070 hat 10, 1120 hat 7 Texte, die kein Skript und keine Zone zeigt.
Die Bilder `ROOM*_C*_vorschlag.png` zeigen die vorhandenen Examine-Zonen (grün, `mNN`) und die
Vorschläge (magenta, Nummer = msg). Erzeugt mit `vorschlag_bild.py`; die Wandpositionen kommen aus der
Rückprojektion der Kamerabilder (`rueck.py`, Kamera = RDT-Kamerasatz, `tools/maske/geom.py`).

## Was das Original hergibt

* Im ausgelieferten Skript gibt es für diese Texte **keine** Zone, auch nicht als Restbytes: Die Rohsuche nach
  `2c ?? 01 … <msg> ff ff` außerhalb der gewalkten Befehle findet in STAGE1 nichts.
* 1070 und 1071 sind skriptgleich (main00/sub00/sub01/sub02 byte-identisch), 1120 und 1121 ebenso.
  Die Texttabellen sind gleich.
* ROOM1140 msg 3 ist eine Byte-Doublette von msg 4, und msg 4 ist verdrahtet (main00 Platz 4 @0x0AE8).
  Dort fehlt nichts.
* Reichweite: Examine-Zonen mit Flags 0x31 prüfen einen Punkt 620 vor dem Spieler (FUN_80042bac,
  `aot_common.c` ab Zeile 1262). Wandgegenstände brauchen daher eine Zone, die über die Wand reicht
  (wie m4 Spinde 20616..21624 über die Ostwand 20968).

## ROOM1070/1071 (Großraumbüro)

Die verdrahteten Zonen main00 @0x0147E..@0x0155A (Plätze 1–12) laufen in Textreihenfolge durch den Raum:
m0 Klappstühle NO → m2/m3/m4 Ostwand nach Süden → m7 Pinnwand Südwand → m8 → m9/m11 Westreihe Nordende → m12 →
m18 Brüstung → m20 Telefon. Kollision (SCA-Zellen): drei Schreibtischreihen West x 2320..5488
(Zellen 10/25), Mitte x 9020..12500 (3/9/17/24) und Ost x 15860..19154 (2/16).

| msg | Text | Vorschlag (x, z, w, d) | Beleg | Sicherheit |
|---|---|---|---|---|
| 19 | A police officer. He's dead... | (344, 13624, 1000, 1000) um Leiche em4; nur solange (4,0xC5)=0, beim Aufstehen weg | siehe unten | **hoch** |
| 6 (Leon, 1070) / 5 (Elza, 1071) | Dienstplan | (13820, 14401, 1430, 1542), Südwand rechts der Pinnwand | Lücke 5/6 liegt zwischen m4 (Ostwand) und m7 (Südwand); Zettel rückprojiziert x 13675..15245 (C3/C4/C7); Wortlaut: "my week" = Polizist Leon, "of some sort" = Außenstehende Elza | mittel–hoch |
| 15 | These desks all appear quite messy | Reihe Ost, beide Längsseiten | 3 Reihen ↔ 3 Reihentexte; Ost: Bücher verstreut (C3) | Reihe sicher, Zuordnung mittel |
| 16 | The desks on this row … paperwork | Reihe Mitte, freie Seiten | Mitte: voll mit Ordnern/Ablagen (C1) | dito |
| 17 | These desks look strangely empty | Reihe West, freie Seiten | West: am leersten (C5) | dito |
| 13 | Going by the shelf … privacy | (4988, 9600, 1000, 2000) | Tisch mit hohem Regal, Westreihe Ostseite Süd (C5) | mittel–niedrig |
| 14 | Like the other, this desk too … | (4988, 11600, 1000, 2028), Nachbartisch | nur Nachbarschaft zu 13 | niedrig |
| 10 | … paperwork at a desk like this | (2638, 2604, 1579, 1000), Nordende Westreihe | Id liegt zwischen m9 (Westseite) und m11 (Ostseite) | niedrig |
| 1 | The seats contra chairs … musical chairs … | – | zweiter Satz zum Stuhl-Thema von msg 0 (m0), kein eigener Gegenstand | nicht verdrahten |

**Leiche (msg 19), Beleg:** sub00 @0x015AE `06 00 80 00` / `21 04 c5 00` setzt fünf Körper mit Deskriptor
0x87/0x88 ("passiver Liegender": in beiden Originalen nicht treffbar, kein Selbst-Wecker, siehe
`enemy_ai_re2_zombie.c`, @0x8011F80C → @0x801039F4 `jr ra`). Der Auslöse-Streifen Platz 14 @0x015B6
(10000, -1000, 1400, 21000) startet sub02 @0x0169C. Das setzt (4,0xC5) und weckt alle fünf
(`34 0c 8a/89` @0x016AE..@0x016CE). Hinter dem Streifen (x < 10000) liegt nur em4 @0x0161A
`44 04 11 87 … ca 4c 03 00 00 2c 37` = Typ 0x11 bei (844, 14124). Typ 0x11 ist das Modell des toten
Polizisten in ROOM1110: em0 @0x00BF8 `44 00 11 88 …` bei (11300, 2675), dazu die Zone Platz 5 @0x00AEE
(10700, 2150, 1000, 1000), die sub02 mit "It's a police officer, he's dead…" startet. Die Zone liegt dort um
den Körper. Der Text ist also für em4 geschrieben, solange er noch "tot" daliegt.

## ROOM1120/1121 (Büro mit Aufzug)

| msg | Text | Vorschlag (x, z, w, d) | Beleg | Sicherheit |
|---|---|---|---|---|
| 1 | A painting, titled: The Music Lesson | (-197, 1150, 1140, 1000) | Bild A Südwand x -197..943 (C2): bunte Figurengruppe in Rot/Grün = Matisse "The Music Lesson" (1917) | mittel |
| 2 | A painting of 5 women's | (-5896, 450, 2271, 1700), über das Sofa (Zelle 9) | Bild B Südwand x -5896..-3625 (C3): hohe dunkle Figuren = Kirchner "Fünf Frauen auf der Straße" | mittel |
| 3 | Only the lights of the buildings are visible | (6150, -1037, 1000, 4876) | Jalousiefenster Ostwand z -1037..3839 (C2), zwischen den Lamellen beleuchtete Fenster | hoch |
| 4 | Various reports and case information | (4950, -5800, 1000, 1850) | Schrank mit Ordnern, Zelle 10 (C4) | mittel–hoch |
| 8 | An advertisement for an arm wrestling contest | (1800, -6300, 3600, 1000)? | Pinnwand Nordwand (C2/C4); Alternative: Zeitschriftenständer Zelle 8 | niedrig |
| 5 | A strange Christmas toy | – | in keinem der fünf Hintergründe zu finden | nicht platzierbar |
| 7 | ... | – | kein Gegenstand zuzuordnen | nicht platzierbar |

Bild C (Westwand z ≈ 336, C3, grün) hat keinen Text.

Alles hier ist Port-Inhalt: Das Original hat diese Zonen nie gehabt. Gestützt sind die Vorschläge auf Daten
(Körper, Kollisionszellen, Textreihenfolge) und auf die Hintergrundbilder.
