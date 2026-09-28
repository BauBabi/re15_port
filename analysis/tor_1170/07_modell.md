# Tor ROOM1170 - Dossier 07: das Tuermodell

Stand 2026-09-28. Auftrag des Nutzers: *"Ich habe dir unter gate_01, gate_02, gate_03 Bilder
von den Tor von Room 1170 gemacht. Ich moechte ein Tuermodel davon erstellen, so wie die DOOR
sachen von Resident Evil 1.5/2 um danach eine Tuer auf Tuer zu Animation zu erstellen."*

Werkzeug: `re15_port/tools/tor/tor_modell.py` (baut alles und fuehrt die Abnahme aus, rund
30 s). Ausgabe nach `build/tor_1170/modell/`. Der Port ist NICHT beruehrt: das Modell wird
erst mit der Sequenz eingebaut.

## Die Dossiers dieser Reihe

| Nr | Inhalt | Gegenpruefung |
|---|---|---|
| 01 | Vermessung des Tors ueber die Raumkameras (`tor_vermessung.py`) | ausgefallen (Guthaben); ersetzt durch die Abnahme unten |
| 02 | Tuerformat DO2/MD1/TIM, Leser/Schreiber (`do2_format.py`) | `02_tuerformat.skeptiker.md`: 33 bestaetigt, 2 in Zahlen berichtigt |
| 03 | Tuersequenz RE2 und was RE1.5 davon hat | `03_tuersequenz.skeptiker.md`: 36 bestaetigt, 1 in Zahlen berichtigt |
| 04 | Katalog der 55 RE2-Tueren, Vorbild DOOR2E (`tuerkatalog.py`, `tuerskript_dump.py`) | `04_tuerkatalog.skeptiker.md`: 29 bestaetigt, 2 widerlegt |
| 05 | Anschlussstellen im Port | ohne (Plan, keine Messwerte) |
| 06 | Massstab Raumwelt -> Tuerszene (`tor_massstab.py`) | `06_massstab.skeptiker.md`: k 1,86 -> **1,90** berichtigt |
| 07 | dieses Dossier: das Modell | Abnahme unten, unabhaengiger Lader-Nachbau |

## 0. Ergebnis

| | Wert | Herkunft |
|---|---|---|
| Meshes | 2: Fluegel (Mesh 0), Pfosten (Mesh 1) | Aufbau wie RE2: Fluegel = Wurzelobjekt, feste Teile eigene Wurzel (04 K30) |
| Fluegel | 92 Vertices, 164 Dreiecke, 0 Vierecke | Tuer-Renderer zeichnet nur Dreiecke (02 "nur-dreiecke") |
| Pfosten | 12 Vertices, 16 Dreiecke | |
| mit zwei Pfosten aufgestellt | 196 Dreiecke | RE1.5-Primitivpuffer reicht rechnerisch fuer 307 (02, abgeleitet) |
| Ausdehnung Fluegel | x -125..125, y -2933..-479, z -3610..144 | Tuerszenen-Einheiten |
| MD1 | 5876 B | Rundlauf lesen -> schreiben byte-gleich |
| TIM | 128 x 256, 8 bit, 1 CLUT, 33312 B, 212 Farben verlustfrei | wie alle 56 Tuertexturen (02 "tim-einheitlich") |
| DO2 | 60448 B, RE1.5-Container, Skript = `Evt_end`, Ton von DOOR00 | wie DOOR00: laedt, zeigt nichts |

Masse im Raum (Torebene, u ab Achse hi-Rohr, v ueber Boden):

| Teil | u | v | Quelle |
|---|---|---|---|
| Rohrrahmen, Mittellinie | 0 .. 1823,9 | 327,9 .. 1467,5 | M1, M2 (lo: Innenkante 1747,87 + R) |
| Rohr | D 151,97, Sechskant | | M3 (Rohr oben, beide Kanten gegen Himmel) |
| Eckbogen | aussen R 307,85, Mittellinie 231,87, 2 Abschnitte | | M4; 2 Abschnitte wie DOOR2E (04 K19) |
| Schild | 293,66 .. 1536,87 | 470,55 .. 1229,34 | M5 |
| Laschen | 369,92..554,96 / 1281,56..1429,18 | 1229,34 .. Rohrmitte | M8 |
| Fuesse | 383,21..551,17 / 1329,60..1459,72 | Rohrmitte .. 470,55 | M8 |
| Pfosten | Achse -207 / -213 / -192 (Cut 0/11/12), D 151,97, H 1802 | 0 .. 1802 | M9, P1 |

Umrechnung in die Tuerszene: `(X, Y, Z) = (-w, -v, -u) * 1,90`. Das ist eine Drehung um y
(Determinante +1), keine Spiegelung. Folge, im Bild geprueft:

- Drehung 0 zeigt der Tuerkamera die **Landeplatz-Seite mit der Angel rechts** - wie Cut 0.
- Drehung 2048 zeigt die **Laufsteg-Seite mit der Angel links** - wie Cut 12.
- Blatt nach -z, oben = -y, Ursprung auf der Angelachse in Bodenhoehe - Konvention des
  Standardblatts (02 "angel", 04 K20).

## 1. Was Beleg ist und was Nachbau

**Beleg.** Jede Masszahl stammt aus Dossier 01 (Kennungen M1..M9, P1). Jeder Texel ist ein
Originalpixel aus ROOM1170 Cut 12, NEAREST ueber die im Bild gemessene Ebenen-Abbildung
(Fluchtpunkte `vx (1231,83 / 190,41)`, `vy (173,50 / -1202,74)`, s = 17,49). Herkunft je
Texturbereich:

| Bereich | Texel | Bildpunkte Cut 12 | verschiedene Bildpunkte |
|---|---|---|---|
| Schild | 128 x 78 | x 132..203, y 134..179 | 3120 |
| Rohr oben | 128 x 16 | x 130..205, y 116..128 | 663 |
| Rohr hi | 128 x 16 | x 109..119, y 132..173 | 413 |
| Rohr unten | 128 x 16 | x 127..206, y 183..192 | 793 |
| Laschen, Fuesse | je 32 x 16 | an ihren Bildrechtecken | 72..155 |

Die Aussenflanke des hi-Rohrs ist im Original tiefschwarz (Zeile 155: x 107..114 = 0..1),
nur die Innenflanke ist beleuchtet (x 115..119 = 57..146). Das Modell traegt das so.

**Nachbau** (steht auch im Kopf des Werkzeugs):

1. Ein Rohrdurchmesser fuer den ganzen Rahmen: ein gebogenes Rohr hat einen. Gewaehlt ist
   151,97, das einzige mit beiden Kanten gegen den Himmel. hi misst 170,4, unten 118,1
   (dunkel vor dunkel); die Abweichung ist 0,5 bzw. 1,9 Bildpunkte.
2. lo-Aussenkante: im Original schwarz vor schwarz. Innenkante gemessen, Rohr angesetzt.
3. Schild, Laschen, Fuesse sind Flaechen ohne Dicke, beidseitig angelegt (Gegenflaechen wie
   in DOOR04/DOOR10/DOOR1F, 02 "doppelseitig"). Die Dicke ist in keinem Cut messbar.
4. Die Landeplatz-Seite traegt denselben Druck wie die Laufsteg-Seite, lesbar. Cut 0 zeigt
   dort ebenfalls Schraffur und helle Tafel, loest die Schrift aber nicht auf (01 D1).
5. Die Rohrrueckseite traegt das gespiegelte Querprofil; das lo-Rohr das Profil des
   hi-Rohrs. Das passt zum Original: auch dort ist lo aussen schwarz, innen hell.
6. Pfosten: ein Rohr desselben Durchmessers (Cut 12 misst 147), Hoehe = oberer Holm 1802.

## 2. Abnahme

### 2.1 Format - unabhaengig gelesen

Der Lader-Nachbau des Format-Skeptikers (`build/tor_1170/skeptiker_tuerformat/sk_lader_sim.py`,
jeder Schritt mit selbst disassemblierter Instruktion, **kein Import aus do2_format.py**) stellt
aus `TOR1170.DO2` auf:

- Fluegel 164 / 164, Pfosten 16 / 16 Dreiecke, **0 abweichend** in Lage, UV, CLUT 0x7800,
  tpage 0x80; Primitiv-Code 0x34 (POLY_GT3).
- MD1 bei 0x18, SCD bei 0x170C (4 B), TIM bei 0x1710; MD1-Ende muss unter 0x9FFC liegen.
- Primitivpuffer 0x801ab000 -> 0x801ae840 = 180 x 80 B.

Dazu `do2_format.lader_pruefung`: lesbar, keine Fehler.

### 2.2 Cut 12 - Einsetzen ins Original

Modell ueber Ebene12 in den Hintergrund gerastert (4-fach, NCLIP >= 0 wie der Tuer-Renderer),
Helligkeits-Korrelation im Ausschnitt, Nullmodell 245 Varianten (Streckung laengs 0,92..1,08,
Versatz +-3 px): **Nulllage auf Rang 1, NCC 0,905.** Selbstbestaetigend fuer die Textur (sie
kommt aus Cut 12); was prueft, sind die Rohrkanten gegen den Himmel - Bild `abnahme_cut12.png`.

### 2.3 Cut 0 und Cut 11 - Abweichung in Bildpunkten

Hier traegt die Textur das Licht von Cut 12, eine Helligkeits-Korrelation misst den
Lichtunterschied mit. Erster Lauf: das Optimum lag in beiden Cuts am Rand des Suchraums
(su 0,92, dy -3), ein Kantenmass streute ohne Optimum (Nulllage Rang 82 bzw. 97 von 405).
**Beides taugt dort nicht als Abnahme.** Stattdessen: das Modell gegen die Linien, die die
Vermessung in genau diesen Cuts gemessen hat, beide ueber die RDT-Kamera projiziert:

| Cut 0 | gemessen | Modell | px |
|---|---|---|---|
| v Rohr oben / unten | 1506,7 / 340,2 | 1467,5 / 327,9 | +0,68 / +0,20 |
| v Rohr oben Ober-/Unterkante | 1570,7 / 1442,5 | 1543,5 / 1391,5 | +0,47 / +0,88 |
| v Rohr unten Ober-/Unterkante | 431,4 / 248,6 | 403,9 / 251,9 | +0,44 / -0,05 |
| v Schild oben / unten | 1299,0 / 471,3 | 1229,3 / 470,6 | +1,19 / +0,01 |
| u Schild lo | 1399,4 | 1536,9 | **-2,34** |
| u Tafel hi / lo | 235,1 / 1273,7 | 444,8 / 1353,7 | **-3,35** / -1,35 |

| Cut 11 | gemessen | Modell | px |
|---|---|---|---|
| v Rohr oben / unten | 1422,5 / 364,9 | 1467,5 / 327,9 | -1,17 / +0,82 |
| v Rohr oben Ober-/Unterkante | 1475,4 / 1369,1 | 1543,5 / 1391,5 | -1,78 / -0,58 |
| v Rohr unten Ober-/Unterkante | 419,9 / 309,5 | 403,9 / 251,9 | +0,36 / +1,26 |
| u Schild hi / lo | 207,4 / 1529,6 | 293,7 / 1536,9 | +1,97 / +0,18 |
| u lo-Rohr (dunkles Band) | 1819,4 | 1823,9 | +0,11 |

Hoehen: hoechstens 1,8 px in beiden Cuts. Breiten: Cut 11 hoechstens 2,0 px. Cut 0 bis 3,4 px
in u - das ist die offene Unstimmigkeit G1 aus Dossier 01 (die waagrechte Skala von Cut 0 ist
8 % kuerzer als die von Cut 11/12, Ursache nicht gefunden); die Tafel ist in Cut 0 2..3 px
breit und an der Aufloesungsgrenze. Das Modell folgt Cut 12, dem einzigen Cut mit genug
Bildpunkten (111 x 76).

Bilder: `abnahme_cut00.png`, `abnahme_cut11.png` (Original | Modell eingesetzt |
Dreieckskanten auf dem Original).

### 2.4 Tuerszene

`vorschau_tuerszene.png`: RE1.5-Tuerkamera (Auge 30000, Ziel 22000, H 1000; 03 K32),
RotMatrix mit m[0][2] = +sin y (03 K22). Vier Ansichten, je 180 Dreiecke, davon 88 gezeichnet
(NCLIP >= 0) - die zugewandte Haelfte der geschlossenen Rohre plus eine Seite der Platten.
Schrift von beiden Seiten lesbar, nicht gespiegelt. Die Aufstellung (12000, 1700, +-1800) gilt
**nur fuer das Bild**, sie ist keine Festlegung fuer die Sequenz.

## 3. Was die Gegenpruefungen fuer das Modell geaendert haben

- **u an der Angel**: das Probemodell aus 02 legte u = 0 an die Angel, die Originale u = 126;
  die Texttafel haette spiegelverkehrt gestanden. Das Modell legt die Textur je Seite so, dass
  sie von dort lesbar ist - im Bild geprueft, nicht gerechnet.
- **k = 1,90 statt 1,86** (06-Skeptiker: zwei Artefakte des Bodenankers).
- **Schwarz = 0x8000**, nicht 0x0000 (05 D7 / 02 "transparenz"): sonst waeren 1865 der 8436
  Bildpunkte von gate_03 Loecher geworden. `tim_aus_bild` ersetzt Schwarz durch 0x8000.
- **Keine Aufhellung** (05 D9): RE1.5 zeichnet die Tuer mit fester Farbe 0x808080 und liest
  keine Normalen (02 "normalen"), die Textur erscheint also so, wie sie ist.

## 4. Nebenbefunde (nicht Teil des Modells)

- **Die Projektnotiz "RE1.5 laedt kein DO2" ist falsch.** Die Tuermaschine laeuft bei jedem
  Tuerwechsel (FUN_8001d600 @0x8001d838/48), 1 Bild lang, weil das einzige Skript `Evt_end` ist.
  Der Skeptiker fand die DOOR00-Textur byte-gleich im VRAM bei (320,256) samt CLUT (0,511) in
  vier sauberen Savestates (03-Skeptiker K31).
- **RE1.5 zeigt die Tuertextur nicht**: der Lader laedt die TIM nach Seite 0x15 / CLUT-Zeile 511
  (@0x80016344, @0x8004eea0..ef3c), laesst die Primitive aber auf 0x0080 / 0x7800
  (a2 = a3 = 0 @0x80016370 / @0x8001639c). Die Zuschlaege stehen nur in RE2 (@0x80013d88 /
  @0x80013d90). Fuer den Port gilt RE2 (Beta -> Retail).
- **Cut 12 von ROOM1170**: der Kamerasatz beschreibt das Bild nicht (Senkrechte laufen halb so
  stark zusammen, das Tor erscheint 1,20-mal zu gross; 01 K3). Ungeprueft durch einen
  Skeptiker. Falls es stimmt, stehen 3D-Figuren in diesem Cut im Port wie im Original falsch.

## 5. OFFEN

1. **Lage des lo-Pfostens**: Cut 0 (u 1738) und Cut 11 (u 1932 hell / 1819 dunkel) widersprechen
   sich (01 M9). Das Modell liefert den Pfosten als Mesh; wo der zweite steht, entscheidet die
   Sequenz - erst nach Klaerung.
2. **Angelseite**: hi-Seite nur durch Indizien gestuetzt (01 A1), keine Scharniere aufgeloest.
3. **Waagrechte 8 % in Cut 0** (01 G1) - ungeklaert.
4. **Textursaetze in RE1.5** werden nach 0x8018fff0 - 12 x Dreieckszahl kopiert, hier ab
   0x8018F780 (2160 B). Belegt ist dort nur, was DOOR00 braucht (540 B ab 0x8018fdd4); was
   darunter liegt, ist nicht bestimmt (02-Skeptiker). Fuer einen PSX-Lauf zu klaeren.
5. Die Gegenpruefung der Vermessung (01) steht aus.

## 6. Naechster Schritt: die Sequenz "Tuer auf, Tuer zu"

Vorbild DOOR2E (04; Gegenpruefung bestaetigt alle Schwerpunkt-Aussagen): 291 Bilder; Bild 0
Aufbau und Einblenden, Bild 130..209 Fluegel +570 Einheiten = 50,1 Grad, ab Bild 200 Fahrt des
Wurzelobjekts durch die Kamera, Bild 260 Ausblenden. Zwei Varianten: aufdruecken (Angel rechts,
freie Kante von der Kamera weg) und aufziehen (Angel links, Fluegel um 180 Grad gedreht).
Fuer ROOM1170 heisst das: Slot 0 (Landeplatz -> Laufsteg) und Slot 6 (zurueck) bekommen je eine
Variante.

Was dafuer zu bauen ist (Dossier 05): die Tuerszene im Port (eigene Kamera, Skriptmaschine mit
den Bewegungs-Opcodes), eine Zuordnung Tuer -> Modell/Variante (die RE1.5-Raumdaten tragen
keine: 649 von 653 Door_aot_set haben Payload+12/+13 = 0), und das Modell eingebacken wie die
Sicherung. RE2-Skriptbytes laufen auf der RE1.5-Maschine nicht unveraendert (Opcode 0x4D gegen
0x4F, Eltern-Bit 0x10 gegen 0x08; 03 K35). Die Aufstellung eines hueft-hohen Tors hat in RE2
kein Vorbild (04 K14) - sie ist dann eine Gestaltungsentscheidung, die ich vorher vorlege.

## 7. Nachmessen

```bash
python re15_port/tools/tor/tor_vermessung.py alles      # Masse -> build/tor_1170/vermessung.json
python re15_port/tools/tor/tor_modell.py                # Modell + Abnahme -> build/tor_1170/modell/
```
