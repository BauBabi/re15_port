# Kameras Bild 1 und Bild 2

Abgabe: `kamera_bild1.json`, `kamera_bild2.json`, `re15_port/tools/form_kamera.py` (lade_kamera, projiziere, strahl_auf_boden, port_nach_bild, bild_nach_port), `kamera_bild1_pruefung.png`, `kamera_bild2_pruefung.png`.
Alle Bildkoordinaten: Originalbild 640x480. Y zeigt nach unten, Boden y = 0.

## Ergebnis

| | Bild 2 | Bild 1 |
|---|---|---|
| Herkunft | ROOM5090 Cut 11, von der Disc | geschaetzt, kein Cut auf der Disc |
| Auge | (7626, -3256, -25360) | (-24116, -2180, -20595) |
| Ziel | (-1504, -986, -21970) | (-12281, -2620, -18664), Gier +9.3 Grad von Ost nach Nord, Nick 2.1 Grad nach oben |
| Brennweite Bild | fx 451, fy 478 | fx 408, fy 428 |
| Hauptpunkt Bild | (320.2, 244.1) | (391.4, 225.5), siehe Abschnitt 4 |
| Restfehler | Median 1.2 px, rms 2.4, max 5.1 (7 Stellen) | rms 5.6 px, max 13.8 (14 Merkmale) |
| Sicherheit | sicher | Hoehe, Horizont, Massstab: wahrscheinlich. Lage in der Tiefe, Hauptpunkt: offen |

## 1. Projektion

- sicher: `ROOM5090.RDT` RID @0x60, Cut 11 @0x1C0: flag 0, fov 26684, pos (7626,-3256,-25360), target (-1504,-986,-21970), pri 0x808. H = fov>>7 = 208 (`camera_common.c`).
- sicher: sx = 160 + H*vx/vz, sy = 120 + H*vy/vz, OFX/OFY 160/120 (`objpoly_r38.c`, @0x8001643c/@0x80016440).
- Kette in `form_kamera.py`: Welt -> Blickraum -> Port 320x240 -> Homographie B -> Bild. Jede JSON traegt B; die Matrix P baut `lade_kamera`.

## 2. Das Video

- sicher: `Version_80%/Biohazard 1.5 Final Build.mp4`, 640x480, 29.97 Bilder/s, 174 Standbilder zu je 24 Videobildern. Bild 1 = Standbild 143 (114.51-115.28 s), Bild 2 = Standbild 145 (116.12-116.88 s). `bild1.png` ist bytegleich mit dem vlcsnap.
- sicher: Es gibt keine feste Abbildung fuer das ganze Video. 56 Kontrollbilder gegen 1119 Hintergruende: Massstab x 1.89-2.33, y 2.02-2.52, Port-Mitte x 262-346, y 217-277.
- wahrscheinlich: Die Trapezverzerrung ist bei den meisten Bildern gleich (22 Bilder mit Gradientenguete ab 0.19: h20 etwa -0.0004, h21 etwa +0.0002, Restfehler des gemeinsamen Modells 2.6-8 px). Die Standbilder 142, 144, 145 rund um beide Vorlagen tragen sie.
- offen: Woher sie kommt. Das Zeilenmuster mit 5 px Periode liegt waagerecht im Bild (Steigung 0.00 gemessen), die Port-Waagerechten dagegen 0.02-0.05 schraeg. Es ist also keine Bezugsrichtung.
- Eine Anzahl Bilder (049, 079, 107, 117, 140) lief ohne Trapezgewinn durch die Suche. Das kann echte Abweichung sein oder ein Haengenbleiben der Optimierung; nicht unterschieden.

## 3. Bild 2

Standbild gegen Port-Hintergrund Cut 11, Doppelwinkel-Gradientenkorrelation, ausgeblendet: Kreatur, Frau, Feuer, Logo.

| Modell | Guete |
|---|---|
| 4 Parameter (x = 2.186u-23.8, y = 2.340v-35.9) | 0.139 |
| Homographie, 8 Parameter | 0.190 |

B = [[1.982795, 0.141114, -21.565264], [-0.088587, 2.333006, -27.489941], [-0.000418, 0.000359, 1.0]]

- Port-Bereich im Bild: Ecken (10.0,12.2), (294.9,23.0), (302.6,219.2), (-5.9,235.2). Ein negativer Wert heisst: dort ragt das Bild minimal ueber den Port-Rand.
- sicher: Rohre, Deckenlamellen, Tuersturz, rote Lampe (Video (258,60)) und Kisten liegen deckungsgleich. Die Lampe ist kein Deckenlicht, die weissen Flaechen rechts (540-625, 300-470) sind die ueberstrahlten Kisten, keine Fenster.
- Restfehler: 7 Stellen lokaler Vergleich: Median 1.2 px, rms 2.4, max 5.1 (bei (360,48) am Rohr). Kantenabstand Median 2.8 px, bei 12 px Verschiebung 5.0.
- wahrscheinlich: Frau, Spawn @0x122A bestaetigt (`44 00 4d 40 .. 58 02 00 00 2c 9d`). Modell Typ 0x4D am Spawn: x 139-195, y 170-358. Bild: Kopf etwa (168,185), Modell-Kopf (160,170), Versatz etwa (+8,+15). Das entspricht einem Standort nahe (1166,0,-25211), rund 570 Einheiten vom Spawn. Sie steht im Bild also nicht exakt am Spawn, oder der Bildstand ist ein anderer. Fuesse: nicht erkennbar (Feuer).

## 4. Bild 1

### Raum

Zuordnung gegen die fuenf Cuts von ROOM5080, jede 3D-Lage ueber Cuts trianguliert oder auf eine Ebene geschnitten:

- sicher: Vier Rauten aus den Typ-2-Zellen der SCA, Mitten (-23296,-12869), (-12996,-12869), (-23271,-23369), (-12996,-23369), Halbdiagonale 3640. Bodenkanten aus Cut 0/1/2 treffen sie (Ecke (-13189,-19713) gegen (-12996,-19729)). Es sind Gruben mit Gelaender, alle Cuts stehen ueber ihnen.
- sicher: Gitterrinne ost-west bei z etwa -18000, x -24000 bis -11700.
- sicher: Tuerrahmen oben (-9815,-3747,-16459) und (-9336,-3786,-18779), Lampe (-10126,-4593,-17421).
- sicher: Kupferrohr-Knick (-9925,-3190,-23091) aus drei Cuts, Rest unter 4 px. Zickzack (-9650,*,-22700). Saeule (-12012,*,-24324). Leiter (-10860,*,-23410).
- wahrscheinlich: Ostwand in x -9650 (Tuer -9336 bis -9815). Bandhoehen -5195, -3855, -4280, -1100 aus Cut 1 und 3 auf dieser Ebene.
- wahrscheinlich: Hut-Rand Q (-13097,-5130,-19662), R (-15625,-5146,-23607). Hoehe auf etwa 500 Einheiten unsicher, Rest in Cut 2 bis 9 px.

### Lesung im Bild

| Merkmal | Bild | Sicherheit |
|---|---|---|
| Fachwerk T1 | x 160-178 | sicher |
| rechter Pfosten T2 | x 280, y 130-190 | wahrscheinlich |
| Rahmen ueber der Tuer, frueher als Leiter gelesen | x 353 und 406 | wahrscheinlich |
| Lampenblendung | (375,180) | sicher |
| Band | (330,187) bis (492,183) | sicher |
| Kupferrohr und Strebe | (490,175)-(535,210), (515,160)-(548,190) | sicher |
| Zickzack | x 522, y 225-300 | sicher |
| Hut-Rand | (445,125) bis (640,60) | sicher |
| Gelbe Linie am Boden | (425,315) bis (605,380) | wahrscheinlich Grubenkante, siehe unten |
| Gelaender vorn rechts | (520,455) bis (640,405) | wahrscheinlich |
| Leon | Fuss (125,366), Kopf (126,202) | sicher |

Das Vordach ist der Hut ueber dem Aufzug. Die Leiter am Aufzug ist tatsaechlich bei x 550-580, der Rahmen in der Bildmitte gehoert zur Tuer.

### Rechnung

Fit ueber alle Merkmale, weich begrenzt, Trapezanteil fest aus Abschnitt 2, plus Senkrechte (T1, Tuerpfeiler) und Leon mit 3020 Einheiten Modellhoehe (PL00 Clip 0, gelesen, nicht 2750 wie frueher im Dossier R).

| Merkmal | gemessen | Modell | Rest |
|---|---|---|---|
| T1 rechter Pfosten x | 178 | 180.6 | +2.6 |
| T2 rechter Pfosten x | 280 | 280.0 | 0.0 |
| Tuerpfeiler L x | 353 | 339.2 | -13.8 |
| Tuerpfeiler R x | 406 | 395.4 | -10.6 |
| Zickzack x | 522 | 523.6 | +1.6 |
| Band -5195 y | 155 | 162.3 | +7.3 |
| Band -3855 y | 194 | 197.3 | +3.3 |
| Band -5238 y | 160 | 156.4 | -3.6 |
| Band -4280 y | 187 | 182.9 | -4.1 |
| Band -4200 y | 184 | 180.6 | -3.4 |
| Gitteroberkante y | 276 | 273.5 | -2.5 |
| Zickzack Fuss y | 300 | 301.5 | +1.5 |
| Rohr-Knick x | 535 | 537.7 | +2.7 |
| Rohr-Knick y | 210 | 208.8 | -1.2 |

rms 5.6 px, max 13.8. Linienabstaende: Grubenkante +7.2 / -1.4, Gelaender -0.1 / +0.2, Hut -1.7 / +15.2.

Alle diese Merkmale sind im Fit benutzt, die Tabelle ist also kein unabhaengiger Test. Vertraeglich, aber kein Beweis: Der Block links (140-235, 280-330) faellt in die Raute NO, die hellen Pfosten links unten (20-30, 360-390) liegen nahe der Raute NW.

### Leon-Probe

Echtes PL00, Clip 0, Fuss (125,367), Welt (-18042,0,-14670) auf begehbarem Boden. Modell x 94-153, y 192-373 (182 px). Bild etwa x 104-150, y 200-368. Der Kopf liegt etwa 8 px zu hoch und 5 px zu weit links, die Beine liegen auf. Ueberdeckung gegen eine grobe Bildmaske 0.57; die Maske ist unscharf, die Zahl taugt nur zum Vergleich der Kandidaten. Pose und Blickrichtung Leons sind nicht erkennbar, Blickwert 2048 ist Bestwert der Probe.

### Was feststeht und was nicht

Vier Kandidaten bleiben gegen Mosaike aus den Cuts (Boden und Ostwand entzerrt, Gradientenguete):

| Kandidat | Auge | Gier | Guete | Boden | Wand |
|---|---|---|---|---|---|
| Endwert | (-24116,-2180,-20595) | +9.3 | 0.086 | 0.076 | 0.100 |
| Endwert, Guete nachoptimiert | (-24477,-2123,-20804) | +9.8 | 0.126 | 0.116 | 0.140 |
| Nord | (-26003,-2348,-11973) | -8.6 | 0.130 | 0.101 | 0.161 |
| West | (-27980,-2371,-17040) | +5.8 | 0.116 | 0.093 | 0.144 |
| Suedwest | (-26494,-2157,-18864) | +11.4 | 0.114 | 0.095 | 0.140 |

Die Guete trennt die Kandidaten nicht, die Zahlen unterscheiden sich im Rauschen. Das Nachoptimieren verschiebt das Auge nur um rund 420 Einheiten, verschlechtert aber Leon (Kopf y 184), deshalb bleibt der Merkmals-Fit der Endwert.

Fuer den Endwert sprechen: Er erklaert das Gelaender vorn rechts mit seiner Steigung, die Steigung des Hut-Rands und den durchgehenden Boden unten im Bild. Er steht am Rand der Grube SW, rund 3300 Einheiten von Cut 2 (-23614,-22606), in dessen Bild Gelaender und Hut gleich laufen.
Dagegen: Die Linien des Zugs und die Gelbe Linie als Gitterrinne deuten auf einen Fluchtpunkt bei x 250-270, wie Kandidat Nord. Nord legt aber die Bildmitte unten (320,470) in die Grube NW und erklaert das Gelaender nicht.

- offen: Hauptpunkt x 391 liegt ausserhalb der 262-346 aller Kontrollbilder. Bei H = 208 endete das Port-Bild links bei x = 78, das Bild hat dort aber Inhalt. Passend waere H etwa 167 bei Massstab 2.44, das liegt ueber den gemessenen 2.33. Mit Hauptpunkt 325 (fest) steigt der Rest auf das Doppelte, Tuerpfeiler -17, Hut +24. Der Widerspruch ist nicht geloest. H = 208 in der JSON ist Rechengroesse, bestimmt ist nur das Produkt H*B.
- offen: Die Gelbe Linie kann Grubenkante oder Gitterrinne sein.
- offen: Abweichung der Tuerpfeiler. Moeglich, dass der Raum der Beta dort anders ist; nicht pruefbar.
- wahrscheinlich: Augenhoehe 2180-2370, Horizont Zeile 232-241 in allen Kandidaten.

Fuer die Modellpruefung zaehlt: Der Bodenmassstab haengt nur an Hoehe und Horizont. Eine 3000 hohe Senkrechte misst je Kandidat:

| Bodenzeile | Pixelhoehe | Streuung |
|---|---|---|
| 372 | 178-190 | etwa 3 % |
| 405 | 220-236 | etwa 4 % |
| 440 | 265-286 | etwa 4 % |

Die Grosse der Kreatur ist damit auf etwa 4 % belegt, ihre Weltlage nicht.

## 5. Pruefbilder

Beide selbst angesehen.

- `kamera_bild2_pruefung.png`: Hintergrundkanten gruen auf dem Standbild, Frau tuerkis am Spawn. Kanten liegen, Frau mit dem Versatz aus Abschnitt 3.
- `kamera_bild1_pruefung.png`: Raum als Linien, Leon tuerkis. Fachwerk, Rohr, Gelaender, Gruben stimmen, Tuerpfeiler 11-14 px links der Bildlinien, Hut bis 15 px unter dem Rand.

## 6. Nicht gemacht

- Nur Geometrie der Kameras, keine Aussage zur Form.
- Die Zwischenskripte liegen im Scratchpad dieser Sitzung und sind nicht Teil der Abgabe; Rechenweg siehe oben.
