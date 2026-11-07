# Rohbau A (Fleisch): Dossier

Stand 2026-11-01. Nur Arbeitsbaum r41_form, Ordner `rohbau_a/`, kein Commit. Kein RE-Wert: neues Modell nach zwei Videostandbildern, nichts davon ist Disc-Beleg.

Lesart FLEISCH: massige Muskulatur, offene Stellen, helle Baender = Sehnen, Knochen, gespannte Haut.
Bauweise: nur Ringe, 6 bis 10 Ecken, Radius je Ecke, Vierecke, starre Teile, Mesh i an Knochen i. Zaehne als 4-eckige Kegel. Keine Metaballs, keine Voxel, kein Skinning.

## 1. Ergebnis

`form_pruefung.py pruefe form.json`, Ausgabe `bericht.txt`, Blaetter `vergleich_bild*.png`, `teile_bild*.png`, `rundum*.png`.

| Bild | Teil | IoU | Deckung | Innen | Bemerkung |
|---|---|---|---|---|---|
| 1 | tail | 0,86 | 0,89 | 0,97 | |
| 1 | right_leg | 0,82 | 0,92 | 0,89 | |
| 1 | part_of_Body | 0,73 | 0,88 | 0,81 | |
| 1 | right_arm | 0,66 | 0,91 | 0,71 | Dorn rechts liegt mit Absicht ausserhalb des Umrisses, Abschnitt 4 |
| 1 | left_leg | 0,60 | 0,89 | 0,65 | Keil zwischen Bein und Schwanz mit Absicht gefuellt, Abschnitt 4 |
| 2 | face_and_upper_body | 0,79 | 0,87 | 0,90 | **Zaehlt rechtes Bein und rechten Oberarm mit.** Ohne die beiden: IoU 0,72, Deckung 0,76 |
| 2 | left_arm_front | 0,68 | 0,85 | 0,78 | |
| 2 | right_arm_front | 0,66 | 0,83 | 0,76 | Unterarm darf nach oben ueber den Streifen stehen, Abschnitt 4 |

Ziel "Ueberdeckung mindestens 0,80": als Deckung erreicht, alle acht 0,83-0,92, mit der Einschraenkung bei `face_and_upper_body`. Als IoU nur tail und right_leg.
Gesamtumriss auf sicheren Pixeln: Bild 1 IoU 0,914, Bild 2 0,917.

| | Bild 1 | Bild 2 |
|---|---|---|
| Kantenpunkte | 20/31 | 5/7 |
| Boden frei, Verdeckung | 0/3 (L1 192 px, L2 104 px, SCH 62 px) | 6/9 (V1, V6, Rand 413) |
| Zeichnung | 8/9 (Z9) | 7/7 |
| Verbote | N2 ok | N1 ok |

Rahmen: 1917 Flaechen (178 Dreiecke, 1739 Vierecke), 32 Knochen, 32 Meshes, Textur 384x256 8 bpp, 3 Clips je 1 Bild, tiefster Punkt je Pose +-0,5 am Boden. `FE.pruefe` 0 Fehler, 1 Warnung: Tick-Clips 3-27 fehlen, gehoeren zur naechsten Stufe.

Wie wenig die Zahlen beweisen: Die Passung zieht das Netz auf dieselben Masken, die IoU und Deckung messen, und die Zeichnung ist in genau die Polygone gelegt, die der Zeichnungstest abfragt. Aussagekraft haben die Bilder.

**Urteil am Bild:** Im gefilterten Vergleich liegt Bild 2 der Vorlage sehr nah: dunkle Kuppel, Klauenhand in drei Lappen, Arm links. Bild 1: Masse, Schwanz, Beine, Arm und Lage der hellen Stellen stimmen, die Figur wirkt unten schlanker als im Video, weil der Schlagschatten fehlt und die Haut ungemalt ist. 1:1 ist es noch nicht, Abschnitt 5.

## 2. Bau

| Datei | Inhalt |
|---|---|
| `kn.py` | Knochen aus `gerippe.json`, zwei dazu, Posen, Vorwaertsrechnung, Zuordnung Teile |
| `ringe.py` | Ring-Teil: Stationen, Radius je Ecke aus Superellipse je Quadrant, Beulen und Furchen, Kappen |
| `entwurf.py` | **die Gestalt**: alle 52 Teile als Tafeln |
| `netz.py` | Flaechen aus Ringen, Ausrichtung |
| `fit.py` | Passung: glattes Korrekturfeld q je Ecke ueber dem Entwurf plus Posen, weicher Rasterer in torch, Anker |
| `korr.json`, `posen.json` | Ergebnis der Passung |
| `atlas.py`, `mal.py` | UV-Streifen, Zonen, Zeichnung ueber die UV-Abbildung des Zeichners |
| `ruhe.py` | Clip 2 |
| `bau.py`, `lauf.sh` | EMD, TIM, form.json, ein Durchgang mit Pruefung |
| `ton.py`, `bild.py`, `nah.py` | Tonblaetter |

Neu bauen: `python bau.py`. Mit Passung: `bash lauf.sh NAME --iter 350`, von vorn `--neu`.

Flaechen: Krallen 216, Schwanz 208, Oberarme 152, Unterarme 144, Oberschenkel 144, Unterschenkel 128, Kopf 125, Fuesse 112, Mittelrumpf 100, Haende 96, Huefte 90, Brust 90, Zehen 90, Zaehne 72, Kiefer 56, Hauer 42, Hals 32.
Atlas: je Teil ein Streifen, Umfang U, Laenge V, beide Koerperseiten eigene Streifen. `atlas_karte.png` zeigt sie benannt, `textur.png` den Stand.

Kappen: Innen liegende Enden schliessen hohl, die Mitte liegt im eigenen Rohr. Der Zeichner hat keinen Tiefenpuffer, eine flache Kappe zog sich im Blick laengs des Glieds ueber die Wand des Nachbarn. Oberarm oben geschlossen mit Spitze, er steht in der Ruhepose frei.

## 3. Was frei gesetzt ist

| Stelle | Stand |
|---|---|
| Gesicht | frei: Knochenkamm als Stufe um das Gesicht, tief liegende Augenhoehlen, Nuestern, Lefzen, 12 Zaehne, Kiefer. Belegt ist nur der Kopfrand auf Zug Q |
| Hauer | frei. Rechter Hauer legt seine Spitze auf P (366,304), der linke ist Gegenstueck |
| Blick | Kopf in Pose 2 zur Kamera gedreht, Rand Mitte (325,245), Radius 62 px wie FORMBLATT2 |
| Schulterdorn | Trizepskopf aussen, je Arm, Abschnitt 4 |
| Rippen links an der Flanke | frei, offene Stelle |
| Daumenkralle, Fingerform | frei. Hand mit 4 Krallen, Daumen innen, Aussenkralle 25 % laenger |
| Fussform, 3 Zehen | frei |
| Flanken, Unterseite | frei, keines der Bilder zeigt sie |
| Linker Arm in Pose 1, Beine in Pose 2 | frei innerhalb des Verdeckten |
| Ruhepose (Clip 2) | frei, `ruhe.py`: Arme senkrecht, Knie gebeugt, Schwanz faellt im Bogen zum Boden |
| Klauen, Zaehne | Farbe (84,66,62), frei. Die Bilder zeigen dort nichts Helles |
| Muskelbaeuche | (62,32,42), oberes Ende der Grundhaut, nur zur Lesbarkeit im Rundumblatt |
| Zeichnung Bild 1 | unteres Ende der Palette, sonst laeuft sie im Filter weiss zu. Schwanzende (131,95,104), aus `farbe_bild1_echt.png` durch Lichtfaktor |

## 4. Abweichungen vom Gerippe und von den Masken

Bindend bleibt die Lesung des Nutzers. Diese Stellen sind Entscheidungen, bitte pruefen.

| Nr | Stelle | Grund |
|---|---|---|
| 1 | 2 Knochen dazu: 30 `klaue_l4`, 31 `klaue_r4` | Umriss der rechten Hand hat 4 Spitzen: (145,353), (197,476), (236,475), (273,443) |
| 2 | Fingerwurzeln z 153 auf 300 | Handbreite 508, Finger standen sonst auf 300 Breite |
| 3 | Pose 2: rechter Unterarm 35,6 Grad um die eigene Achse, Hand dreht gegen, Handgelenk bleibt | Blick im Knochenraum (-0,58 0,07 -0,81), der Streifen ist nur 20-28 px breit, flache Seite zur Bildebene |
| 4 | Pose 2: Kopf zur Kamera, 3 Anker | Rand als Ring um den Kopf statt schraeg |
| 5 | Pose 2: rechtes Bein gehockt, Knie (228,314), Knoechel (252,357) | fuellt die untere Spitze von `face_and_upper_body` |
| 6 | Aktorlage Bild 1 (-19141,0,-18630), Bild 2 (745,0,-23923) | Wurzelversatz der Passung in die Lage gezogen. Bild 1 rund 650 von FORMBLATT2, Kamera dort geschaetzt. Bild 2 rund 210 |
| 7 | Mesh `oberarm_l` 18 % dicker als rechts | Bild 2 verlangt rechts oben Masse, in Bild 1 ist er verdeckt |
| 8 | Linke Seite weicht von rechts ab | q je Seite getrennt, die Figur ist nicht spiegelgleich |

Zusaetze nur in `fit.py`, die Pruefung misst weiter gegen die Originale:

| Teil | Zusatz | Grund |
|---|---|---|
| left_leg | Keil (222-250,345-445) | dunkel im Bild, in keinem Teil, pruefung.md O3 |
| right_arm | (404-431,244-296) | gemessene Kante (429,272), Z9, pruefung.md O1. **Nutzer fragen, was dort liegt** |
| right_arm_front | Ueberstand in `face_and_upper_body` kostet nichts | FORMBLATT2: nur die rechte Kante des Streifens ist echt. Dafuer ist der Unterarm von der Seite nicht duenn |

In `form.json` zaehlen rechtes Bein und rechter Oberarm in Bild 2 zu `face_and_upper_body`: Sie stehen dort im Umriss. Zahl ohne sie steht in Abschnitt 1.

## 5. Was im Bild noch abweicht

Bild 1:
- Linke Hand steht neben dem Knie in den Luecken L1 und L2, 192 + 104 px dunkel auf hellem Boden, bis 11 px tief. Versuch, den Arm im Schritt zu heben, misslang (Abschnitt 7). Schwerster Fehler in diesem Bild.
- Kuppel rechts oben: Flanke (366,215) 9 px zu tief. Dort ziehen Bild 1 und Bild 2 gegeneinander.
- Oberkante Schwanz bei x 322 rund 12 px zu kurz.
- Rechte Schwanzkante y 318-342 rund 5 px zu schmal, Knie rechts y 346 rund 5 px zu schmal.
- Hand rechts: Aussenkralle steht 20 px ueber die Spitze (425,363) hinaus, Daumen links ueber den Umriss.
- Z9 nur 0,44 getroffen, der Dorn steht tiefer als der Schraegstrich.
- Die Zeichnung ist Fleck auf Grund, im Video zerfaellt sie weicher. Z3 liest sich als runde weisse Scheibe. Zu hell im gefilterten Render, Y gegen Video: Z1 193/142, Z3 226/172, Z5 155/75.
- Schlagschatten fehlt in der Buehne, die Beine wirken in den Blaettern dadurch schmaler.

Bild 2:
- Schulter rechts im Bild: Rand 399 statt 413, Rohr-Ende (411,206) 9-12 px zu kurz.
- Lichtbalken V1 nur 0,88 verdeckt, Luecke 9 px.
- Die Lappen der Hand treffen, die Kanten nicht auf den Pixel. Spitze (273,443) meldet das Mass als fehlend: Daumen liegt dort, ragt aber unter 12 px aus dem Sattel. Der Sporn links oben ist die lange Aussenkralle, Spitze 833 ueber dem Boden.
- Gesicht ist im Bild zu dunkel, um es zu beurteilen.

Rundum:
- Klares Tier: buckliger Rumpf, schwere Schultern, langer Schwanz, tief haengender Schaedel. Keine Ballons, Querschnitte sind kantig.
- Beine sind das Schwaechste: Oberschenkel liegen bei rund 60 % des Entwurfs, von der Seite wirken sie duenn gegen den Rumpf. Beide Bilder lassen wenig Spielraum, solange die Posen bleiben.
- Muskeln sind Furchen und Beulen je Ecke, ohne Bemalung im Dunkel kaum zu sehen.
- Rumpfstuecke treffen sich als Kerbe an den Gelenken.
- Verzahnte Grenzen zwischen ueberlappenden Teilen sind die Tiefensortierung.

## 6. Spielprobe

`form_spiel.sh`, gdigrab, echtes Fenster, debug.log: `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`.

| Bild | Aufruf | Befund |
|---|---|---|
| `spiel_pose2_room5090_schnitt11.png` | `--pose 745,-23923,3917,1,0,11` | laedt, steht, Flammen des Raums davor |
| `spiel_pose1_room5080_schnitt1.png` | `--raum 5080 --pose -19141,-18630,83,0,0,1` | laedt, steht. Seitlich und klein, Bild 1 hat keinen Schnitt auf der Disc. In Schnitt 2 ist die Figur nicht im Bild |
| `spiel_ruhe_room5080_schnitt1.png` | `--raum 5080 --pose -19141,-18630,83,2,0,1` | Clip 2 steht auf dem Boden |

Das belegt Laden, Zeichnen und Stand, keine Aehnlichkeit. In ROOM5080 zieht die Haut ins Violette, nicht bewertet.

## 7. Durchgaenge

Laeufe `zw/p0`-`p18` plus `zw/ende`, Abgabe = Stand p17. Zahl = IoU `right_arm_front`.

| Lauf | Aenderung | Befund |
|---|---|---|
| p0 | Roehren aus dem Gerippe | 0,58, Figur 120 Einheiten zu hoch |
| p1 | Passung, Zaehne, Hauer | 0,45. Fit legte den Kopf quer in den Umriss, Finger waren vertauscht zugeordnet |
| p2-p3 | Unterarm flach und gedreht, Rumpf oben flach, Keil | 0,53 und 0,58 |
| p4 | Randband 4-fach, tiefe Beine, Dorn | 0,61 |
| p5 | Entwurf skaliert, Rumpfenden tauchen untereinander | Stufen am Ruecken weg |
| p6 | Schulter kuerzer, Finger dicker | 0,69 |
| p7-p9 | Kopf neu, Wurzelhoehe gebunden | 0,73-0,74, Wurzel sank sonst 50 Einheiten |
| p10 | hohle Kappen, Flaechen gespart | Scheiben weg |
| p11-p13 | Boden in der Passung, Bein fuellt Spitze | Fund: Posen wanderten von Lauf zu Lauf, Prior galt gegen den letzten Lauf statt gegen das Gerippe |
| p14 | von vorn, Prior gegen Gerippe | tail 0,85, right_leg 0,83, Hand 0,62 |
| p15-p17 | Handspielraum, Unterarm und Beine behalten Tiefe | Abgabe |
| p18 | linker Arm gehoben, Hand hinter die Brust | **verworfen**: Arm stach aus dem Rumpf, `part_of_Body` 0,57, Schwarz 2163 px |

Fehler unterwegs: Normalen zeigten nach innen und das Tonblatt sah Rueckseiten (nach p1 behoben). Hueftkopf mit Spitze stach durch den Schwanz. Kappen trugen die Farbe der Zeichnung, bis Zeilen 0-7 des Atlas gesperrt waren.

## 8. Dateien und Reste

Abgabe: `FORM.EMD`, `FORM.TIM`, `form.json`, `vergleich_bild1.png`, `vergleich_bild2.png`, `teile_bild1.png`, `teile_bild2.png`, `rundum.png`, `rundum_hell.png`, `spiel_*.png`, `bericht.txt`, `atlas_karte.png`, `textur.png`, Generator wie in Abschnitt 2. Zwischenstaende in `zw/`.

Liegen geblieben: eigene Laufordner unter `re15_port/build/form_spiel/`: 12811_061613, 17898_073040, 17995_073106, 18084_073154, 18443_073814, 18517_073835, 18588_073857. Eine Sammel-Loeschung wurde von der Sicherheitspruefung abgelehnt und nicht wiederholt. Die Ordner sind unversioniert, sie enthalten je eine exe-Kopie und koennen von Hand weg.

Kein Commit. Nichts unter `pri/`, `synchro/` und im Hauptbaum angefasst, keine fremden Prozesse beendet.
