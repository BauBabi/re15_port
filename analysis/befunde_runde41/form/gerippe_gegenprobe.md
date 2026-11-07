# Gegenprobe Gerippe

Stand 2026-11-01, Arbeitsbaum r41_form, kein Commit. Geprueft: `gerippe.json`, `FORMBLATT2.md`, `gerippe_bild1.png`, `gerippe_bild2.png`, `gerippe_rundum.png` aus `lauf/ende3.pt`.
Neuer Abgabestand: `gerippe/lauf/ende4.pt`. Alles Alte liegt in `gegenprobe/vorher/`.
Werkzeug: `gegenprobe/fk.py` (eigene Kette, nur JSON und `form_kamera.py`), `render.py` (echte Kegelstuempfe, Maler), `anatomie.py` (Grenzen, Boden, Voxel 40), `richtung.py`, `kopfachsen.py`, `sichtbar.py`, `lupe.py`. Laeufe `r1.sh` bis `r8.sh`, Umbauten `patch1.py` bis `patch9.py`, Profile `profile2.sh`.

## Urteil

Bild 1 hielt der Pruefung stand. Bild 2 nicht: Rechter Arm, Kopf, Gier und Beine waren falsch, teils von der eigenen Metrik verdeckt. Berichtigt und neu gerechnet. Der Koerper ist jetzt von allen Seiten eine moegliche Figur. Die Hand rechts deckt ihren Umriss schlechter als vorher, das ist der groesste offene Rest.

## Antworten auf die fuenf Fragen

### 1. Stimmt jedes Nutzer-Teil in Lage und Richtung?

Abstand Mittellinie des Teils zur projizierten Knochenkette, Median in px (`richtung_vorher.txt`, `richtung_nachher.txt`).

| Teil | Bild | vorher | nachher | Urteil |
|---|---|---|---|---|
| tail | 1 | 2,2 | 2,3 | stimmte |
| left_leg | 1 | 3,8 | 4,7 | stimmte |
| right_leg | 1 | 2,0 | 1,0 | stimmte |
| right_arm | 1 | 5,0 | 4,0 | stimmte |
| left_arm_front | 2 | 2,4 | 4,7 | stimmte unten, oben 24 px neben der Mittellinie, jetzt 15 |
| right_arm_front | 2 | 16,7, Max 56 | Streifen 0,8 zur Achse, siehe unten | war falsch |
| part_of_Body, face_and_upper_body | | Flaechen, Abschnitt Restfehler | | |

Rechter Arm Bild 2, vorher: Ellbogen (284,351) mitten im Streifen, Handgelenk (229,444) im Logo, 57 px neben der eigenen Lesung (262,395). Die Linie Ellbogen-Handgelenk lag zwar 5-6 px neben der Achse des Streifens, aber der Unterarm sass rund 45 px zu weit unten auf dieser Linie. Oberarm (262,297)-(284,351) lag VOR der Brust, 6211 px sichtbar, gegen die Lesung im selben Blatt ("Rumpf vor den Armansaetzen"). Hand lief vom Handgelenk zurueck nach links oben, von der Kamera weg. Spitze (325-337,316-335) deckte die Brust.

Nachher: Achse des Streifens gemessen, rechte Kante (335,326)-(274,408), 20 px nach links oben. Ellbogen (312,322) 0,8 px daneben, Handgelenk (257,397) 0,2 px daneben, Richtung 53,7 gegen 53,4 Grad. Oberarm 114 px sichtbar. Hand laeuft weiter nach links unten, Fingerspitzen (231,469), (191,460), (167,428). Der Medianwert der Tabelle bleibt bei 14 px, weil die Mittellinie der Maske in den Sporn (149,351) laeuft, den das Modell nicht deckt.

Mein eigener erster Zielpunkt (326,332) lag auf der Kante statt auf der Achse. Das Maler-Bild zeigte es, Lauf `g8` berichtigt.

Linker Arm: Schulter vorher (363,241), Mittellinie laeuft nach (402,264). Rechter Teil von Zug Q ist die Linie, an der der Nutzer Oberkoerper und Arm trennt, rechts davon steht die Schulter hinter dem Kopf. Jetzt (399,223). Fingerspitzen auf den Spitzen des Umrisses: (461,394), (465,413), (438,415) gegen (460,392), (468,417), (437,418).

### 2. Sind beide Posen dieselbe Figur?

Ja. Eine Knochentabelle, Gelenkabstaende Pose 1 gegen Pose 2 gleich bis auf Rundung (`fk.py`, 4e-12). Dicken passen auch im Bild: Unterarm Bild 1 30 px Modell gegen 28-37 gelesen, Bild 2 45 px Modell gegen 40-45.
Rest: Bild 1 zeigt den Unterarm 934 senkrecht, Bild 2 verlangt 1100 Laenge. Das geht nur, wenn der Arm in Pose 1 rund 35 Grad nach hinten geneigt ist. Moeglich, aber eine Folge, kein Befund.

### 3. Sind die Gelenke moeglich?

| Pruefung | vorher | nachher |
|---|---|---|
| Ellbogen Pose 1 | beide +12 Grad ueberstreckt, Grenze | 0-2 Grad, gestreckt |
| Rechter Oberschenkel im Rumpf, Pose 2 | 45 % | 5 % |
| Rechter Unterschenkel im Rumpf, Pose 2 | 24 % | unter 3 % |
| Linker Oberschenkel im Rumpf, Pose 2 | 17 % | 3,5 % |
| Oberarme im Rumpf | 4-7 % | 3-8 % |
| Kiefer im Rumpf | 52-60 % | unter 3 % |
| Hals im Rumpf | 87-94 % | 47-69 %, kurzer Hals [f] |
| Kopf Roll zur Welt Bild 2 | 59 Grad | 3 Grad |
| Hand rechts zu Unterarm Bild 2 | Hand zeigt nach hinten, Gelenk 65 Grad gedreht | 70 Grad, Handgelenk gebeugt, Finger voraus |
| Fuesse, tiefster Punkt | 23-40 | 15-23 |
| Linke Fingerspitzen Pose 2 | Hoehe 307-438 | 76, 102, aussen 348 |
| Rechte Fingerspitzen Pose 2 | Hoehe 115-843 | 173-184 |
| Finger Pose 1, hoechste Spitze | 760 | 495 |
| Schwanzende Pose 1 | auf dem Boden | 255 ueber dem Boden, unter dem Bildrand, [f] |

Am Ende ihres Bereichs (`ae2.txt`): rechter Oberarm Drehung 98 von 100 Grad, Hals Drehung 60 von 60, Unterarm und Unterschenkel seitlich 12 von 12, Fuss 40 von 40, zwei Klauen 31-32 von 30. Moeglich, aber in Pose 2 ist kein Spielraum nach aussen. Die Drehung des rechten Oberarms hatte die alte Pose auch (93 Grad).
Rechter Oberarm 11 % im Kopf, Kopf steht in Pose 2 an der rechten Schulter. Kein Bild sieht das.

### 4. Sind links und rechts vertauscht?

Nein. Figurraum +X vorn, +Y unten, +Z = X x Y = links, Blickmatrix rechtshaendig (det +1, je Knochen nachgerechnet). Bild 1, Gier 7: Figur blickt von der Kamera weg, linke Schulter (304,259) links der rechten (392,254). Bild 2, Gier -16: Figur blickt zur Kamera, linke Schulter (399,223) rechts der rechten (292,262). Nutzer-Teile passen: left_leg links in Bild 1, left_arm_front rechts in Bild 2.

### 5. Wie plausibel ist der Koerperbau aus der ungesehenen Seite?

Vorher nicht. Sechs Seiten je Pose in `gerippe_rundblick.png` (Stand vorher: `gegenprobe/vorher/`, Bild neu erzeugbar mit `render.py vorher/gerippe.json`):
- Kopf war ein Trichter: 1220 lang, am Ende 1500 hoch, 970 breit, im Bild um 59 Grad gekippt, damit die Hoehe als Breite zaehlt.
- Oberarm 601 gegen Unterarm 1096, Oberarm zur Schulter duenner (274 auf 416): Rest des Maskenrands, keine Form.
- Rumpf in Pose 1 um 24 Grad verdreht, in Pose 2 krumm.
- Kiefer im Hals, Spitze 670 hinter der Schnauze.

Nachher: Tonnenfoermiger Rumpf, gerade in beiden Posen, runder Kopf, Arme von der Schulter, Beine aussen am Bauch, in Pose 2 Hocke. Keine Ansicht zeigt einen Fehler, den ich belegen koennte. Unbelegt bleibt, was kein Bild sieht:
- Rumpflaenge: Profil 1800-2800 gleich gut, 2240 ist Ergebnis, kein Mass.
- Kopflaenge 1300, Halslaenge, Fuss 900: laufen in Grenzen oder sind frei.
- Oberarm: Vorgabe mindestens 0,72 Unterarm.
- Schwanzwurzel sitzt oben auf der Huefte (Versatz -757). Bild 1 braucht den Schwanz ueber dem Ruecken, wo er genau ansetzt, sieht niemand. Der Uebergang gehoert in den Modellbau.

## Restfehler

| Bild | Teil | Blatt vorher | Maler vorher | Blatt nachher | Maler nachher | Figur deckt Teil nachher |
|---|---|---|---|---|---|---|
| 1 | tail | 0,85 | 0,86 | 0,86 | 0,87 | 0,96 |
| 1 | left_leg | 0,76 | 0,75 | 0,78 | 0,75 | 0,94 |
| 1 | right_leg | 0,82 | 0,82 | 0,81 | 0,81 | 0,93 |
| 1 | right_arm | 0,78 | 0,76 | 0,79 | 0,79 | 0,92 |
| 1 | part_of_Body | 0,70 | 0,71 | 0,69 | 0,71 | 0,98 |
| 2 | face_and_upper_body | 0,89 | 0,82 | 0,77 | 0,76 | 0,93 |
| 2 | left_arm_front | 0,83 | 0,80 | 0,70 | 0,71 | 0,95 |
| 2 | right_arm_front | 0,71 | 0,51 | 0,57 | 0,55 | 0,67 |

Die Blatt-Zahlen 0,89 und 0,71 von vorher waren geschoent: `ansicht.restfehler` zog Oberarm-Pixel im Oberkoerper-Umriss vom Arm ab und schrieb sie dem Rumpf gut. Die Metrik belohnte damit genau den Fehler. Regel entfernt. Unabhaengig gemessen stieg der rechte Arm von 0,51 auf 0,55.
Oberkoerper sinkt, weil das rechte Bein die untere Spitze fuellt und nicht zum Teil zaehlt, Deckung 0,93. Arm links sinkt, weil die Hand so gross ist wie die rechte.
Rechter Arm: Deckung fiel von 0,81 auf 0,67. Der alte Stand deckte die Flaeche mit einem Arm an falscher Stelle. Fehlt jetzt: Sporn links oben, Flaeche links unter dem Logo, rechte Ecke (279,420).

Messpunkte (`gerippe/lauf/pruefpunkte_ende.json`): Kante R belegt 0-1,4 px (vorher 0-5,4). V1, V3, V4, V5 voll, V2 0,95, BO frei. L1 231, L2 292 Pixel belegt, schlechter als vorher (178, 223), siehe FORMBLATT2 Abschnitt 7.

## Korrekturen

Liste mit Belegen in `FORMBLATT2.md` Abschnitt 10. Geaenderte Dateien: `gerippe.json`, `FORMBLATT2.md`, `gerippe_bild1.png`, `gerippe_bild2.png`, `gerippe_rundum.png`, neu `gerippe_rundblick.png`. Werkzeug: `gerippe/rig.py`, `fit.py`, `ansicht.py`, `abgabe.py`, `bericht.py`, `formblatt2_vorlage.md`.

## Offen, nicht entschieden

- Zweite Lesung: Kopf links unten bei B10-B12, Kuppel ist das Hinterteil. Wuerde die Rumpfkante und das Hinaufsteigen der Masse nach rechts oben ohne Aufrichten erklaeren. Widerspricht der Lesung nicht, ich habe sie nicht gerechnet, weil Zug Q als Kopfrand vorgegeben blieb.
- Sporn (147,350) und Lappen der Hand: Handschnitt schemenhafter Dinge. Mit drei Kegelfingern nicht zu fassen, Modellbau.
- Heller Fleck P (362,303): Modell hat dort eine Luecke. Kann Feuer im Hintergrund sein.
- Gier Bild 2: siehe Profile. 21 ist schlechter, -40 bis 0 gleich gut.

## Protokoll

- Nachrechnung Abgabe: eigene Kette trifft FORMBLATT2 Abschnitt 5 auf 1 px und 1-2 Einheiten.
- Rumpf Bild 2: Handlinie ergibt Gier -14,4. Gesetzte Gier 21 erzwang Huefte +21, Brust -17. Altes Profil -30 bis +45 flach, kein Bildbeleg fuer 21.
- Geometrie rechter Arm: Handgelenk Hoehe 300 auf Tiefe rund 5300, Ellbogen gleiche Tiefe Hoehe 1100. Abstand passt zum Unterarm ohne Laengenaenderung. Die Hand steht also am Boden.
- Oberarm: Dossier gerippe las selbst "mindestens 900", Abgabe hatte 601.
- `g1`: nur Punkte, Kopf frei. Kopf wird Tonne, Hand schwebt. Verworfen.
- `g2`: Anker im Raum, gerader Rumpf. Seitenansicht deutlich besser. Hand faechert falsch, Oberarm schrumpft auf Untergrenze.
- `g3`: Knie, Knoechel, Klauen verankert. Finger bis 20 Einheiten geschrumpft.
- `g4`: Mindestdicken. Gier frei auf -14,8. Kante R 11-14 px innen, V2 0,60, Wirbelsaeule Zickzack.
- `g5`: Kante beidseitig. Wespentaille, weil der Rumpf links innerhalb der gelesenen Kante bleiben musste. FORMBLATT: "Silhouette kann weiter aussen liegen". Verworfen.
- `g6`: gelesene Kante nur Mindestausdehnung. Tonne, Kante R 0-1,4 px, V2 0,95. Kopf 31 Grad neben der Kamera.
- `g7`: Blick-Kosten, 20 Grad. Unterarm 5-10 px rechts neben dem Streifen.
- `g8` = `ende4`: Zielpunkte auf die Achse.
- Falle: Versaetze und Dicken an einer Grenze haben keinen Gradienten (min/max auf Rohwert). `kiefer_x`, `huefte_hinten`, `schenkel_x`, `klaue_z` standen so fest. `fit.py --setze`, `--setzed` holen sie heraus.
- Rumpf 1800 statt 2240 (`t_rumpf` Zeile 2) macht den Arm in Pose 1 nicht steiler, Schulter Tiefe 7600 gegen 7705. Die Neigung haengt am Unterarm.
