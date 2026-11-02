# Rohbau B (Panzer)

Stand 2026-11-01, Arbeitsbaum r41_form, Ordner `rohbau_b/`, kein Commit. Bauweise: Ringe entlang der Knochen, starre Teile, Oberflaeche Panzer. Kein Metaball, kein Voxel, kein Fremdmodell als Form.

## 1. Ergebnis

Rahmen eingehalten: 1876 Flaechen, 32 Knochen, Textur 384x256 8 bpp, 3 Clips, tiefster Punkt je Pose auf y=0. Zahlen aus `form_pruefung.py pruefe form.json`, Auszug `ZAHLEN.txt`, voll `bericht.txt`.

| Teil | Bild | IoU | Deckung | Innen |
|---|---|---|---|---|
| tail | 1 | 0,89 | 0,98 | 0,91 |
| right_leg | 1 | 0,90 | 0,96 | 0,93 |
| left_leg | 1 | 0,78 | 0,95 | 0,82 |
| part_of_Body | 1 | 0,78 | 0,93 | 0,83 |
| right_arm | 1 | 0,78 | 0,89 | 0,87 |
| face_and_upper_body | 2 | 0,79 | 0,84 | 0,94 |
| left_arm_front | 2 | 0,69 | 0,88 | 0,76 |
| right_arm_front | 2 | 0,74 | 0,86 | 0,85 |

Ziel 0,80: Deckung bei allen acht erreicht, kleinster Wert 0,84. IoU nur bei zwei von acht, kleinster Wert 0,69 (left_arm_front).
Bild 1: Punkte 22/31, Boden 1/3, Zeichnung 8/9. Bild 2: Punkte 7/7, Flaechen 8/9, Zeichnung 6/7.

Am Bild beurteilt (`vergleich_bild1_nah.png`, `vergleich_bild2_nah.png`): Bild 2 trifft Dome, Zug Q, Unterarmstreifen und Stimmung nah. Bild 1 trifft Haltung und Umriss. Der Eindruck ist gut, aber die Figur ist nicht gleich: siehe Abschnitt 6.

## 2. Bauweise

- `ringe.py`: Teil = Folge von Ringen, je Ring Mitte, Ausrichtung, n Ecken mit eigenem Radius. Rumpf und Kopf 10 Ecken, Glieder 8, Finger 6, Krallen und Spitzen 3 und 4. Vierecke zwischen den Ringen, Enden mit Pol oder offen, wo sie im Elternteil stecken.
- Plattenkante: zwei Ringe dicht hintereinander mit Radiussprung, harte Kante ueber den Knickwinkel der Normalen (46 Grad). Rumpf: vordere Platte liegt auf der hinteren. Schwanz: je Abschnitt ein Rand ueber dem naechsten. Glieder: Schulterplatte, Knieplatte, Wadenplatten, Knoechelring, Stulpe.
- Querschnitte sind kantig: Dachform an der Huefte, Grat und Seitenkante am Schwanz, flacher Unterarm, Kiel unter der Brust, zwei Eckbuckel am Kopfrand.
- `figur.py` enthaelt alle Masse von Hand. `anpassen.py` skaliert und schiebt danach nur ganze Ringe und zieht Gelenkwinkel nach, Koordinatensuche gegen die Nutzer-Teile. Mittelteile bleiben symmetrisch, links gleich rechts.
- Gelenke ueberlappen: Brust hat einen langen hinteren Kragen ueber dem Wirbel, Wirbel laeuft innen weiter. `biege.py`, Blatt `lauf/biege.png`: Rumpf +-25 Grad, Ellbogen und Knie 90 Grad ohne sichtbares Loch im Lehmblatt. Nur Lehmblatt, im Spiel nicht geprueft.
- Atlas: je Teil ein Streifen, U Umfang, V Laenge. Finger, Zehen, Zaehne, Schuppen und Wangenspitzen teilen sich je eine Karte. Lage der Karten in `zonen.json`, Vorlage `textur.png`.

## 3. Aenderungen am Gerippe

| Stelle | Aenderung | Grund |
|---|---|---|
| Knochen | 30 und 31 neu: `klaue_l4`, `klaue_r4` | Beide Haende zeigen im Umriss vier Spitzen: rechts (274,443), (222,475), (197,476), (147,350), links (410,392), (437,418), (468,417), (460,392) |
| Hand | kuerzer, Finger neu versetzt, Tabelle `FINGER` in `figur.py` | Handgelenk zu Spitzen im Raum 1000-1300, das Gerippe hatte 1300-1500 |
| Hand | Handflaeche halb so breit wie zuerst gebaut, Finger dick | Keil der linken Hand ist nur rund 60 px breit, die Breite rechts kommt von den gespreizten Fingern |
| Unterarm | flach, vorn-hinten rund 225-265, seitlich 295-325 | Bild 1 und rechter Arm Bild 2 sehen die Breite, linker Arm Bild 2 die Schmalseite |
| Linker Unterarm Bild 2 | 45 Grad um die eigene Achse gedreht, Hand gegengedreht | wie oben |
| Pose 2 | Huefte steiler und tiefer, Beine neue Hocke, Schwanz liegt auf | Maske oben links frei, unten links voll |
| Pose 1 | Linker Arm gehoben, hinter Rumpf und rechtem Bein | Boden in L1 und L2 ist frei, die Hand kann dort nicht stehen |
| Beide | Aeusserer Finger der linken Hand in die Handflaeche geschlagen | Der lange Finger passt links nicht in den Keil |
| Kopf Bild 2 | Hals und Kopf nachgestellt, Rand-Mitte (327,245) | Zug Q |

Alle Winkel stehen in `posen.json`, Ringfaktoren in `ringfit.json`.

## 4. Frei gesetzt

Kein Bild belegt: Schnauze, Kiefer, Zaehne, Brauen, Wangenspitzen (nur die Spitze ist an P gebunden), Fingerlaengen, Fussform, Zehen, Plattenteilung, Bauch, Unterarm-Schuppen in ihrer Form, linker Arm Pose 1, Beine und Schwanz Pose 2, Ruhepose Clip 2.
Grundhaut (36,22,32) liegt im Fehlerband der Palette, gewaehlt nach dem Filtervergleich. Hoehenanteil der Normalen gestaucht (`NORM_U0`, `NORM_G` in `bau.py`), weil das Disc-Licht von ROOM5080 Bild 1 sonst zu hell macht. Wirkt nicht in ROOM5090.
Der Haken H ist nicht gebaut: Der Nutzer-Umriss laeuft um ihn herum.

## 5. Durchgaenge

1. Roehren nach `gerippe.json`, 1300 Flaechen. Tier sofort lesbar. Huefte in Bild 1 zu breit, Hand zu klein.
2. Dachform Huefte, Stufen an den Gliedenden, Kopfrand, Klauen.
3. Platten, Zaehne, vier Finger je Hand, 32 Knochen. 2106 Flaechen, zu viel.
4. Offene Enden, Zaehne sparsam. Bodenlage: Klauen standen 140 unter dem Boden, Haende flach nachgestellt.
5. Pose 2: Huefte tief. Umriss oben links passt, unten links fuellt die Hocke.
6. Ring- und Winkelanpassung. Dabei wanderte der Kopf aus Zug Q heraus, Kopf festgesetzt und zurueck auf den Bogen.
7. Unterarm flach, linker Unterarm gedreht, Handflaeche schmaler.
8. Zonen, Zeichnung, Normalen, Zug Q als Linie, Wangenspitzen fuer P.
9. Mittelteile symmetrisch gemacht (Huefte war 150 seitlich verschoben), Ruhepose, Biegeprobe, Bodenkontakt als Kosten.
10. Linker Fuss nach innen in den dunklen Keil probiert: Keil 0,74 auf 0,90 belegt, aber left_leg IoU 0,78 auf 0,65. Verworfen, Fuss bleibt wie im Gerippe. Rueckseiten aus der Zeichnung genommen, Abschluss.

## 6. Was im Bild noch abweicht

Bild 1:
- Rosa Stelle Z1 laeuft im Filter weisser als im Original, Weisskern Z3 ebenfalls. Das Licht hebt beide.
- Dunkler Keil zwischen left_leg und tail (218-255,345-445) ist nur zu 0,74 belegt, dort steht heller Boden. Er traegt Z7, ist also Figur, liegt aber in keinem Nutzer-Teil.
- Schulter rechts: Kante (429,272) um 26 px verfehlt, Z9 liegt neben der Figur. Der Nutzer-Umriss endet dort frueher, ich bin ihm gefolgt. Nutzer fragen, was dort liegt (pruefung.md O1).
- Scheitel: Schwanzkuppe 4-6 px zu hoch, faellt bei x=322 um 13 px zu frueh ab. Linke Kante bei y=342 um 8 px zu weit links.
- L1 und L2 sind nicht ganz frei: 32 und 39 px, Eindringen unter 3 px, die Pruefung verlangt 0.
- Haut flaechiger als das Video, Blau im Filter schwaecher. Malen fehlt.

Bild 2:
- Spitze P: Stelle gedeckt, aber nur 0,26 weiss, die Wangenspitze steht fast laengs zum Blick. Im Filter viel schwaecher als im Original.
- Hand rechts: Das Werkzeug findet die Spitze (274,443) nicht als eigene Spitze, gerechnet liegt der Daumen auf (273,442). Spitze (222,475) ordnet es 53 px daneben zu. Gut getroffen: (145,353) 2 px, (197,476) 6 px.
- Hand links: Spitzen 9 px neben dem Soll, IoU 0,69 bleibt der schwaechste Wert. Die Hand steht rechts ueber den Keil hinaus.
- Rechtes Bein fuellt die Bucht unten links. Es zaehlt nicht zu face_and_upper_body, darum dort Deckung 0,84, jedes Modellteil zusammen 0,97.
- Haken H fehlt, ein Betrachter sieht ihn im Original. Bewusst, siehe Abschnitt 4.
- Schwanzende: Das Werkzeug misst 53 px gegen (251,477). Das Ende laeuft wie im Bild unten heraus, Ist (198,479), Spitze im Modell unterhalb des Bildes.

Nicht geprueft: Gegner-Tick. Die Datei hat nur drei Clips, `FE.pruefe` warnt deshalb, die Tick-Clips 3, 4, 5, 8, 9, 10, 26, 27 fehlen. Biegen nur im Lehmblatt, nicht im Spiel.

## 7. Spiel

`form_spiel.sh`, gdigrab, je Lauf `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`.
- `spiel_pose2_r5090_s11.png`: ROOM5090 Schnitt 11. Figur steht richtig, Zug Q und Unterarmstreifen sichtbar. Feuer des Raums steht anders als im Screenshot und verdeckt die rechte Hand zum Teil.
- `spiel_pose1_r5080_s1.png`: ROOM5080 Schnitt 1, die Sicht aus Bild 1 gibt es dort nicht. Figur steht auf dem Boden, seitlich gesehen ein klares Tier, Licht hebt die Haut violett an.

- `spiel_ruhe_r5080_s1.png`: Clip 2 im selben Schnitt.

Alle drei Bilder sind nach dem letzten Bau von Hand aufgerufen, gleiche Datei wie die Abgabe. `abschluss.sh ende spiel` meldet fuer Pose 1 dagegen `Form nicht im Raum`, dreimal, Ursache nicht gefunden. Der Wagenruecklauf in `lage.txt` war es nicht. Fuer Pose 1 also den Aufruf von Hand nehmen.

Ruhepose: Fuesse stehen 73 ueber dem Boden, die Klauen tragen.

## 8. Neu erzeugen

`python ik1.py` bis `ik9.py` und `kopf_lage.py` sind Einzelschritte, das Ergebnis steht in `posen.json`, nicht in der Reihenfolge wiederholbar ohne Nachlauf. Bauen allein: `python bau.py --pruefe TAG` liest `posen.json` und `ringfit.json`.
Verworfen, nicht in der Abgabe: `ik8.py` (Fuss in den Keil).
