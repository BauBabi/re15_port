# Rohbau final: Dossier

Stand 2026-11-01, nach Abnahme 1 nachgearbeitet. Abgabestand `lauf/ende`, `stand_final.json`. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert: neues Modell nach zwei Videostandbildern. Formvorlagen nur `bild1.png`, `bild2.png`, bindend die Lesung in `nutzer_teile/`.
Grundlage: Rohbau C (Sieger, 19 Punkte), dazu die Einfaelle aus den drei Urteilen, dann die Maengel aus `rohbau_abnahme_1.md`. Generator `rf_*.py`, die Kopie des Siegers `rc_*.py` liegt daneben. Stand vor der Nacharbeit: `stand_vor_abn1/` (FORM.EMD md5 0d717025).

## 1. Ergebnis

Messung `form_pruefung.py pruefe form.json`, `lauf/ende/bericht.txt`. Zuordnung streng wie im Urteil Mass (Oberkoerper nur 0, 7, 10, 11, 21, 22, 23).

| Teil | Bild | IoU | Deckung | Innen | vor Abn. 1 IoU |
|---|---|---|---|---|---|
| tail | 1 | 0,87 | 0,96 | 0,91 | 0,89 |
| left_leg | 1 | 0,67 | 0,96 | 0,69 | 0,67 |
| right_leg | 1 | **0,82** | 0,97 | 0,84 | 0,77 |
| right_arm | 1 | 0,72 | 0,97 | 0,74 | 0,76 |
| part_of_Body | 1 | 0,80 | 0,87 | 0,91 | 0,81 |
| face_and_upper_body | 2 | 0,82 | 0,86 | 0,94 | 0,84 |
| left_arm_front | 2 | 0,79 | 0,83 | 0,94 | 0,78 |
| right_arm_front | 2 | 0,68 | 0,82 | 0,80 | 0,75 |
| Mittel Bild 1 / Bild 2 | | 0,776 / 0,764 | | | 0,779 / 0,790 |

| Messung | Jetzt | Vor Abn. 1 | Bemerkung |
|---|---|---|---|
| Umriss-IoU Bild 1 / 2 | **0,978** / 0,925 | 0,950 / 0,940 | Weiss gedeckt 0,983 / 0,928 |
| Schwarz getroffen px | 163 / 156 | 186 / 57 | |
| Kantenpunkte | **31/31, 7/7** | 21/31, 6/7 | Herkunft siehe unten |
| Boden frei | 1/3, L1 4 px, L2 49 px | 1/3, L1 23 px, L2 69 px | 2/2 in Bild 2 (vorher 0/2) |
| Verdeckung Bild 2 | **9/9** | 7/9 | V6, BO jetzt frei |
| Zeichnung | 8/9, 7/7 | 6/9, 6/7 | offen nur Z5 |
| Rahmen | 1900 Flaechen, 32 Knochen, 384x256, 3 Clips, Boden je Pose y = 0 | | alles eingehalten |

**Was diese Zahlen beweisen.** IoU misst gegen die Masken, an die gerechnet wurde. Die Kantenpunkte erst recht: Die Rest-NEIN der letzten Laeufe habe ich als Bandzonen in die Passung genommen (`band_ein`, `band_aus` in `rf_fit.py`), 31/31 ist also zum Teil selbst gesetzt. Bestanden heisst: Es passt, nicht: Es ist belegt. Unabhaengig vom Passziel sind nur das Spiegelbild der Abnahme (Oberkante gegen das Original ohne Maske, Abschnitt 2 M5), Rundblick, Biegeprobe und Spielbild.
right_arm und right_arm_front fallen in der IoU gegen den alten Stand, und das ist Absicht: Der Arm steht jetzt in O1 und im Spalt, also ausserhalb der Maske, und der Aussenfinger fuellt die Handmaske nicht mehr als Blatt.

**Am Bild.** Bild 1 ist der groesste Schritt: Koerper, Arm und Kopf bilden wie im Original eine geschlossene dunkle Masse, das Achselfenster ist weg, die Oberkante laeuft durch (`vergleich_bild1.png`, Nachstellung im Spiel `spiel_pose1_nachstellung_r5080.png`). Bild 2: gleiche Masse, Bogen, beide Arme, grosse Klauenhand aus Pranke und Krallen (`vergleich_bild2.png`). Im Rundblick (`rundblick_*.png`) ein schweres, tiefes Tier mit durchgehendem Ruecken. 1:1 ist es nicht, Abschnitt 8.

Spiel: `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, je Lauf 0x30 im Raum: `spiel_pose2_r5090_s11.png`, `spiel_pose1_r5080_s1.png`, `spiel_ruhe_r5080_s1.png`, dazu `spiel_pose1_nachstellung_r5080.png` (Koordinaten aus Abnahme 1). Verfahren PrintWindow, die Sitzung ist getrennt, gdigrab per hwnd lieferte in dieser Sitzung weisse Bilder, obwohl die Desktop-Probe durchging (`rf_spiel.sh` erzwingt darum PrintWindow und prueft jedes Bild).

## 2. Maengel der Abnahme 1

| Mangel | Vorher | Jetzt | Beleg |
|---|---|---|---|
| **M1** Achselfenster Bild 1 | 863 px hell (Zaehlung `abn_probe.py`), gedeckt 0,20, `part_of_Body` leer 407 px | **83 px**, davon 0,90 gedeckt, `part_of_Body` ohne Fleck ab 40 px. Helles Original gedeckt 0,15 wie vorher | `abn/mess/probe.txt`, `abn/mess/teilrest_aus.txt` |
| | Weg | Hals Pose 1 um 30 Grad tiefer, nur Clip 0 (`rf_a1.dreh`). Tiefster Punkt Kopf/Kiefer Hoehe 402, Kiefer steht im Fenster, Bild 2 und Ruhe unberuehrt. Im Spiel geprueft | `spiel_pose1_nachstellung_r5080.png` |
| **M2** Aussenkante rechter Arm | `right_arm` leer 97 px (411-419,308-337), `b1.fr_y324_rechts` -3,5 | Fleck weg, -1,5 | `bericht.txt`, `abn/mess/teilrest_aus.txt` |
| | Weg | Ellbogen nach aussen wie im Hinweis: (397,296) auf (402,298), Handgelenk (402,362), Unterarm steht jetzt senkrecht in der Maske | `abn/mess/gelenk_aus.txt` |
| **M2 O1** dunkle Masse rechts | `b1.rand_rechts` -13,5 gegen Grenze 3,6, gedeckt 0,27 | **-1,5**. Oberarm und Ellbogen laufen bis x 427, Kante Original 429-431 | `o1_blatt.png`. Bleibt Nutzerfrage, Abschnitt 9 |
| **M3** Brust als Trommel | Pose 2 Hinterkante 492 auf 40 Weg, Dach 520 flach. Ruhe Dach 960 flach | Keine Hinterkante mehr: Pose 2 groesster Sprung am Rumpf 173 auf 40, das ist der steile Anstieg am Wirbel (1752, 2062, 2451 auf je 120), keine Stufe. Dach Pose 2 240, Ruhe 240 flach. Der Ruecken liest sich als ein Bogen, Linie je Clip in `abn/mess/dachlinie.txt` | `abn/mess/probe.txt`, `abn/netz/nah_pose2_ruecken.png`, `abn/netz/nah_pose2_gesamt.png`, `abn/netz/nah_ruhe_gesamt.png` |
| | Weg | Abschnitt 3 | |
| **M4** Aussenfinger als Blatt | Breite 592, Dicke 514, Geschwister 317-329, links 474 | **313 / 254**, Laenge 1186, gekruemmt (Saebel nach aussen). Links 207 / 168. Sporn trifft (145,353) auf 4,1 px (Bericht) | `abn/mess/probe.txt`, `abn/netz/nah_hand_r_pose2.png` |
| | Rest der Handmaske | Pranke breiter (1,16) und laenger (1,12), Kuppel als Handruecken, Mittelfinger staerker und laeuft in den linken Lappen, **fuenfte Kralle** starr an der Hand trifft (197,476), Daumen (274,443) auf 0,8 px. Deckung 0,82 (0,87). Leer bleibt 896 px (153-184,374-436), Y 44 | `teile_bild2.png`, `abn/mess/teilrest_aus.txt` |
| **M5** Kerbe | x 330 Modell 210 gegen 195, x 335 207 gegen 199 | **193 gegen 195, 197 gegen 199** | `abn/mess/dach.txt` |
| **M5** Kuppe | `scheitel_x300` -5,5, x305 -4,5, x322 +9,5 | **-1,5, -0,5, +1,5**. Oberkante x 330-375 liegt rundum 0-2 px am Original | `bericht.txt`, `abn/mess/dach.txt` |
| **M6** Schwanz Ruhe | tiefster Punkt 433 | **2**, Bein 2, Hand 0-2 | `abn/mess/netz_aus.txt` |

Fund nebenbei, der mehr erklaert als M5: `form_emd.boden_nachfuehren` hebt die ganze Figur um den tiefsten Punkt. Ein einzelner Fingerpunkt unter dem Boden versetzt Bild 1 sichtbar. Vor der Nacharbeit waren es 23 Einheiten (Wurzel Stand -1884, EMD -1907), also rund 2 px nach oben, in den Laeufen dieser Runde bis 55 Einheiten, 5 px. Die Passung sah das nicht, sie rechnet ohne Hub. Dort steckte ein Teil der Kuppenabweichung und mehrere Kanten-NEIN. Jetzt straft `rf_fit.py` die sechs tiefsten Punkte einzeln, Hub 2 und 3 Einheiten.

Beobachtung der Abnahme zu Zug Q: Bogenpunkt (290,172) liegt auf der Brust, nicht auf dem Kopf. Die Marken sind neu rueckprojiziert, sie liegen wieder am Bild. Bewegt sich der Kopf, reisst der Bogen weiterhin.

## 3. Bau

Ring-Bauweise, keine Metaballs, kein Voxel, kein Skinning. Mesh i haengt an Knochen i. Abgabe: FORM.EMD md5 93f571e7, FORM.TIM md5 1e045a30.

| Gruppe | Knochen | Ecken je Ring | Flaechen |
|---|---|---|---|
| Rumpf (Huefte, Wirbel, Brust) | 0, 7, 10 | 10 | 310 |
| Hals, Kopf, Kiefer, 4 Fangzaehne, 2 Wangenspitzen | 21, 22, 23 | 8, 10, 8, 4 | 262 |
| Schwanz | 1-6 | 8 | 256 |
| Arme | 11, 12, 16, 17 | 8 | 288 |
| Beine | 8, 9, 24, 26 | 8 | 240 |
| Haende mit vier Fingern und je einer starren Kralle | 13-15, 18-20, 28-31 | 8, 6 | 400 |
| Fuesse mit drei Zehen | 25, 27 | 8, 4 | 144 |

- Mitte spiegelgleich gebunden, links ist der Spiegel von rechts. Volumen links/rechts 1,00 an allen Paaren, nur Hand und Finger 0,29 (`mess/paare.txt`): ein einziger Skalenfaktor `HAND_L = 0.66` (vorher 0,80). Die rechte Hand wuchs, die linke blieb im Mass stehen, sonst steht sie in Bild 1 in L2 und in Bild 2 ueber ihrer Maske.
- **Ruecken als Kugelgelenk (M3).** Der Knick Wirbel/Brust reicht von -33 Grad (Pose 1) bis +47 (Pose 2). Mit Hinterkante quer zur Achse muss dabei ein Rand aus dem Ruecken treten. Darum laufen beide Rueckenseiten als Kappen um den Gelenkpunkt, Radius rund 950: Die Brust zieht ihre Rueckenecken mit schraegem Rand bis 330 Einheiten nach hinten (`rf_plan.BRUST_KAPPE`), der Wirbel hebt seine vorderen Rueckenringe auf denselben Radius (`rf_a1.ruecken`). In Pose 2 deckt die Kappe das Tal zwischen Brustdach und Wirbelbuckel, in Pose 1 und Ruhe liegt sie im Wirbel. Bauch und Flanke behalten den Teleskop-Rand. Das Dach bekam dabei einen Scheitel bei t 60-420 statt einer Ebene.
- Eine Normale je Ecke wie RE2. Offene Kanten 0, Durchblick 0 in 24 Blicken je Clip (`abn/mess/netz_aus.txt`).
- Finger: vier Ringe statt fuenf, frei gewordene 48 Flaechen tragen die beiden Krallen. Stand der Finger neu aus dem Plan, die alten Ringmitten liefen bis 476 neben die Achse (das war das Blatt). In der Passung sind Fingermitten jetzt streng gebunden.
- Atlas: je Teil ein benannter Streifen, U Umfang, V Laenge, `atlas.json`, `atlas_blick.png`, 0,0285 Texel je Einheit.

## 4. Was aus den Rohbauten kam

| Aus | Uebernommen |
|---|---|
| C | Grundstock Gerippe, Passung Radien und Ringmitten, Schwanz mit Keil an der Wurzel, Bein, Klauenspitzen als Punktziele, Rueckprojektion der Marken, Kind eingezogen |
| B | Spiegelgleiche Grundform, Rumpf fuellt den Oberkoerper ohne Bein, Kopf mit Schild und Schnauze, Zug Q als durchgehende Linie, Grundton nah am Video, Stufen im Beinumriss, Plattenrand am Rumpf |
| A | Pranke mit getrennten, gekruemmten Krallen, Kuppel-Enden, Keil und dunkle Stellen neben den Masken ernst nehmen, neutrale Ruhepose, Kopf zur Kamera (10 Grad, C 18), Masse rechts neben dem Arm |
| RE2 | Eine Normale je Ecke, Zaehne in der Textur (Kopf 110 Flaechen, C 277) |

## 5. Maengel der Urteile

Stand nach der Nacharbeit. Geprueft am neuen Stand mit derselben Messreihe in `mess/`.

| Urteil | Mangel | Stand |
|---|---|---|
| Mass 1 | Oberkoerper nur 0, 7, 10, 11, 21, 22, 23 | erledigt, 0,82 / 0,86 |
| Mass 2 | Kein Glied ueber 0,10 im Rumpf | erledigt, Glieder bis 0,06 |
| Mass 3 | Vier Glieder am Boden je Pose | erledigt, tiefster Punkt je Hand und Fuss hoechstens 8 Einheiten (`abn/mess/netz_aus.txt`) |
| Mass 4 | Fuss rechts Pose 2, Schulter an die Brust, Horn | erledigt. Sporn Hoehe 577 (vorher 699, C 1357) |
| Mass 5 | Volumen links/rechts im Bericht | `mess/paare.txt`, Abweichung nur Hand, Frage 2 in Abschnitt 9 |
| Mass | Rechter Unterarm ausgerenkt, ungleiche Seiten | erledigt, Ellbogen Drehung hoechstens 18 Grad |
| Bild | Rhombus Bein/Schwanz | erledigt, Keil links 99 %, rechts 100 % |
| Bild | Marken zu gross, zu hell | Z5 16x11 gegen 15x9, Y 73 gegen 75. Z1 283 gegen 290 px, Z3/4 281 gegen 308 px, X 996 gegen 911 px |
| Bild | Unterarm G | 88 % der Laenge, Rot 106 gegen 157, Rippen aus dem Bild |
| Bild | Rotes Licht Bild 2 | Zonen Rotbraun auf Handruecken und Armfront, nur was Bild 1 nicht sieht |
| Bild | Dunkelzone zwischen den Armen | 73 % |
| Bild | Bogen unterbrochen | Zug Q als Linie, im Filter lesbar |
| Spiel | Schief, Spiegelabstand bis 155 | erledigt, 0 ausser Hand |
| Spiel | Kopf Stachelball, Hals Stufenringe | erledigt, vier Fangzaehne, Zahnreihe in der Textur |
| Spiel | Rumpf 190 Flaechen, Saegeschnitte | 310 Flaechen, Krater 0 |
| Spiel | Pose 2 Bein schwebt 266 | erledigt |
| Spiel | Schwanz als Buckel in Ruhe | erledigt, Ruhe laeuft aus dem Ruecken bis zum Boden. In Pose 1 bleibt er, Bild 1 verlangt ihn |
| Spiel | Ruhepose nur Pose 1 mit Kopf hoch | erledigt, neutral und spiegelgleich, Bauch rund 715 ueber Boden |

Biegeprobe (`mess/spiel/biege.txt`, `krater.txt`, `spalt.txt`, Blatt `biegeprobe.png`): Krater 0 an Ellbogen, Knie, Brust. Kein Riss. Brust Kind im Eltern 0,36, Schlitz 0: Die Kappe haelt bei +-45 Grad.

## 6. Zeichnung

Marken in echter Form statt ganzer Prueffelder: Pixel ueber halber Hoehe zwischen Umgebung und Spitze (`rf_textur.marke_form`). Farbe je Marke aus der Stellschleife `rf_stell.py`: Render gegen zurueckgerechnete Schirmfarbe, Grenzen je Marke. Faktoren `zw/marken.json`. Alles nach der Formaenderung neu rueckprojiziert.

Die Pruefung zaehlt Material im Albedo und belohnt Uebergroesse.
- Z5: Material 0,44, im Bild 16x11 gegen 15x9, Y 73 gegen 75. Echte Groesse deckt das Polygon nicht, darum NEIN.
- Z9: jetzt gebaut, weil der Arm in O1 steht, Figur 0,49, Y 75 gegen 55. Haengt an Frage 1.
- Bild-1-Stellen, die zu hell oder zu dunkel bleiben: Z2 Y 72 gegen 130, Z6a 170 gegen 134.
- Bild 2 zu dunkel: Q1 41 gegen 58, Q4 52 gegen 96, P 73 gegen 116, G 84 gegen 113.

Rechter Unterarm: Bild 1 sieht ihn von hinten, Bild 2 den Streifen G. Deshalb rollt der Unterarm in Pose 1 rund 60 Grad, Bild 1 sieht die Ecken 1-5, Bild 2 die Ecken 5-8. Der Rest bleibt in Bild 1 dunkel, N2 0,001.

## 7. Gerippe und Posen gegen FORMBLATT2

| Aenderung | Grund |
|---|---|
| Pose 2: Becken tief, Rumpf rund 45 Grad, Wurzelhoehe rund 1110, sitzend aufgestuetzt | Nur so fuellt der Rumpf den Oberkoerper-Umriss links unten ohne Bein |
| Pose 2: rechtes Bein unter dem Unterarm, linker Fuss in der Dunkelzone auf V3 | Beide Fuesse am Boden, kein Bein links neben dem Umriss |
| Pose 1: linker Arm hinter dem rechten Bein, Handgelenk (328,354), Hand am Boden | L2 frei |
| Pose 1: Hals 30 Grad tiefer gegen den Stand vor Abnahme 1 | M1 |
| Pose 1: rechter Arm, Ellbogen (402,298), Handgelenk (402,362) Hoehe 396, Hand steht auf der Aussenkante, Rolle rund 60 Grad | Bild 1 sieht die Hand von hinten, breit flach passt sie nicht in den Fuss der Maske. Innere Krallen stehen hinter dem Unterarm bis Hoehe 800. Abschnitt 8 |
| Pose 1: rechter Oberarm gedreht, Unterarm rollt | Abschnitt 6 |
| Finger: Wurzeln auf Handbreite, Grundspreizung im Mesh (25, 0, -26, 50 Grad), fuenfte Kralle 38 Grad starr | Faecher von Bild 2 |
| Hand laenger: Stationen 1,12fach, Finger 830, 930, 1060, Kralle 830, Daumen 640 | Reichweite zum Sporn und zum linken Lappen. Laengere Pranke ging nicht, Bild 1 sieht sie von hinten |
| Gier Bild 1 80 statt 88, Bild 2 3874, Lage unveraendert | Passung |

Laengen und Versaetze ausser Fingern unveraendert, Knochen 10, 13, 18 bleiben fuer den Tick.
Gelenke: Ellbogen und Knie Drehung hoechstens 18 Grad. Ueber Grenze (`mess/m2.txt`): Aussenfinger links Pose 2 Y 48 gegen 30, im Handraum sind das 22 Grad nach innen, nur der Bezugswert aus dem Gerippe passt nicht (rechts braucht dieselbe Ruhelage 34 nach aussen). Sonst Finger Y bis 36, Fingerrolle bis 8, `fuss_r` Y 40,5 gegen 40, Unterarm rechts X 13,2 gegen 12, Oberarm rechts Pose 2 am Drehlimit (+100), Erbe des Gerippes.

## 8. Was im Bild noch abweicht

Bild 1:
- Oberflaeche: Das Original zerfaellt in Lappen, Baender und Glanzpunkte, das Modell ist glatt mit Marken. Das ist das Malen.
- Armkuppe x 380-395: 4-7 px ueber der dunklen Kante (240 gegen 244-247), Bein und Maske sind dort einig, die helle Schulterkante des Originals nicht.
- Rechts x 405-425 liegt die Oberkante des Arms tiefer als das dunkle Rohr hinter ihm, das ist kein Figurteil.
- Keil links gefuellt, IoU `left_leg` bleibt deshalb 0,67.
- Rosa Ende im Filter gleichmaessig, im Original zerrissen. Haende in Bild 1 dunkelrot am Rand, im Original dunkel.
- Schlagschatten der Platte fehlt, das ist die Buehne.

Bild 2:
- Linker Lappen der Hand: 896 px leer (153-184,374-436), Y 44. Dort steht im Video dunkle Kuppel, im Modell Luecke zwischen Sporn und Mittelfinger. Fuellen ginge nur mit Blatt oder groesserer Handflaeche, das zweite stoesst in Bild 1 an.
- Oberkoerper oben links 851 px leer (236-295,146-216), Y 15, schwarz auf schwarz, die gelesene Kante liegt ohnehin 5-28 px innerhalb des Nutzerumrisses.
- Zug Q, Q4, P schwaecher als im Video, Abschnitt 6. Rot 106 gegen 157 am Unterarm: Der Feuerschein fehlt, die Engine hat in ROOM5090 kein Farblicht.
- Innen keine Gesichtszuege. Das Maul ist im Spiel als dunkelrote Raute lesbar, im Video nicht.
- Rechter Oberarm steckt im Kopf, kein Bild zeigt das.

Rundblick und Spiel:
- Pose 1: rechte Hand steht auf der Kante, innere Krallen hoch hinter dem Unterarm. Aus Bild 1 unsichtbar, aus Seitenblicken auffaellig (`zw/a/end_h1.png`). Das ist der Preis der grossen Hand, und das ist die Stelle, die ich zuerst nachfassen wuerde.
- Hinter dem Hals sitzt in Pose 1 der Kopf tief, der Hals ist von vorn lang.
- Saegezacken an Ueberlappungen: Maler ohne Tiefenpuffer, wie im Urteil auch bei RE2 gemessen, im Ton deutlich, auf der dunklen Textur kaum.
- Spiel Bild 1 hat keinen Schnitt, Schnitt 1 zeigt die Figur klein von der anderen Seite, darum die Nachstellung.
- Feuer des Ports steht in Schnitt 11 vor Hand und Unterarm, im Video nicht.

Nicht geprueft: bewegte Clips, Tick-Clips 3 bis 27 (`FE.pruefe` warnt), Kasten, Schatten-Festwerte, Vordergrundmasken, PSX-Last (3184 Dreiecke).

## 9. Fragen an den Nutzer

1. **O1:** Gehoert die dunkle Masse rechts neben dem Arm zur Figur? Blatt `o1_blatt.png`. Ich habe sie als Arm gebaut, das Bild spricht dafuer. Wenn nicht: `rf_a1.py` mit `o1=0` und ohne Hebung des Oberarms, Kosten rechter Arm enger, `b1.rand_rechts` faellt wieder.
2. **Hand:** Darf die rechte Hand deutlich groesser sein als die linke? Gesetzt: linke Hand 0,66, Volumen 0,29, vorher 0,80 und 0,51. Beleg: Handschnitte 110 gegen 52 px bei Tiefenverhaeltnis 1,16 bis 1,25, Wortlaut "grosse Klauenhand" gegen "Klaue". Das bleibt Lesart, kein Beweis.
3. **Fuenf Krallen:** Reicht das, oder sieht er im Bild vier? Gesetzt: vier Finger mit eigenen Knochen und eine starre Kralle.
4. **Pose 2:** Stimmt die sitzende Haltung, Becken tief? Sie folgt aus "Oberkoerper ohne Bein". Das Bild zeigt das Becken nicht.
5. **Keil links:** Hier ist es das linke Bein. Es kann auch etwas anderes sein.

## 10. Frei gesetzt

Gesicht, Kieferform, Kammform, Fangzaehne, Wangenspitze (Spitze zielt auf P), Zahnreihe, Augen, Fingerzahl, fuenfte Kralle, Saebelform des Aussenfingers, Kuppel der Hand, Zehen, Rumpfseiten, Rueckenlinie (Bogen ist Formwunsch aus M3, kein Bildbeleg), Ruhepose, alle Ring-Reliefs. Belegt sind nur Umrisse und Lage aus den Bildern.

## 11. Durchgaenge

Vor Abnahme 1:

| Lauf | Aenderung | Befund |
|---|---|---|
| s1 | Pose 2 ueber Anker neu, Becken tief | Erst Hueftgelenk als Anker, Brust sackte ab. Anker aufs Rumpfende, Rumpf 45 Grad |
| s2 | Spiegelbindung, strenge Masken | Mittel 0,79 / 0,67, Hand links steht ueber, linker Arm in L2 |
| s3 | Anker linker Arm, linke Hand 0,82 | Oberkoerper 0,83, L2 frei |
| s4 | Kamm neu, rechte Hand groesser | Passung blaest den Hals auf, verworfen |
| s5 | Hals festgehalten | Bester Zwischenstand 0,81 / 0,80, Spiel laeuft |
| s6-s8 | Spalt ueber den Rumpf | Brust waechst ueber den Arm, Drift, verworfen |
| s9-s10 | Von s5: Rumpf in Pose 1 fest, Rolle Unterarm | Streifen G frei von Bild 1 |
| s11-s12 | Kante 7, V1-V5 als Pflicht, Farben enger | Zeichnung 7/9, 7/7 |
| s13 | Oberschenkelkopf, Kopf hinten | Innen-Test unter 0,03, aber P verloren |
| s14, ende | P zurueck, N2 | Abgabe zur Abnahme 1 |

Nach Abnahme 1 (`zw/a/`, Blaetter `a*.png`, `a*_n2.png`):

| Lauf | Aenderung | Befund am Bild |
|---|---|---|
| a1 | Hals 30, Dach gesenkt, Rand nach vorn gekippt, Aussenfinger aus dem Plan | Fenster zu, Finger schlank. Trommel bleibt als Keil, Rand steht weiter aus dem Ruecken. Handmaske links leer |
| a2 | Kappe nach hinten, Wirbel vorn gehoben | Ruecken zum ersten Mal ein Bogen. Spitze ueber dem Dach in Bild 2: Die Kerbenzone hatte Ring 3 hochgezogen |
| a3 | Rumpf gesperrt, Pranke, Hand 1,25 lang | Dach sauber, Hand gross. Finger in L2, im Fuss rechts |
| a4 | Fuenfte Kralle | Kopf riesig: Neue Teile verschoben die Stand-Nummern, Fehler von mir, `umziehen` behebt ihn |
| a5-a8 | Pranke schmaler, kuerzer, Daumen vor, Finger laenger, Anker in Pose 1 | FUGE, L2 zurueck, Oberkante blieb 5 px daneben, ohne erkennbaren Grund |
| a9 | Bandzonen, Saebel | Hand in der Maske, Kuppe schlechter statt besser |
| a10 | Fit gegen Render verglichen (`zw_verg.py`): 5 px Versatz, Ursache Bodenhub. Strafe auf die tiefsten Punkte | Sprung: Bild 1 von 0,949 auf 0,976, 26/31, tail 0,88, L2 18 px |
| a11 | Feinzonen, linke Hand einsortiert, Ruhe neu | 29/31, 9/9 |
| a12 | Restkanten, Oberschenkel und Schwanz 4, 5 frei | 31/31, 7/7 |
| a13, ende | `rf_a2.py`: nur Fingergelenke links in Pose 2, Aussenfinger stand 13 px neben dem Soll | 3 px |

Falle im eigenen Aufruf (alt): Python schreibt unter Windows CRLF, das CR hing an der Gier, `sscanf` im Spiel verwarf die Pose. `rf_spiel.sh` entfernt das CR.

## 12. Dateien und Lauf

Generator: `rf_kern.py` (Knochen, Ringteile, Bindung, Raster), `rf_plan.py` (Bauplan), `rf_fit.py` (Passung), `rf_s1.py` bis `rf_s5.py` (Stufen vor Abnahme 1), `rf_a1.py`, `rf_a2.py` (Nacharbeit), `rf_ruhe.py` (Clip 2), `rf_bau.py` (EMD, Atlas, Normalen), `rf_textur.py`, `rf_marken.py`, `rf_stell.py`, `rf_abgabe.py`, `rf_ton.py`, `rf_biege.py`, `rf_spiel.sh`, `rf_o1.py`. Sichten: `rf_nah.py` (Tiefenpuffer ohne EMD), `rf_verg.py` (Fit gegen Pruefrender, zeigt den Bodenhub), `rf_ring.py`, `rf_spitzen.py`, `rf_auflage.py`, `rf_blick.py`, `rf_gelenke.py`, `rf_seiten.py`. Messreihe der Abnahme, Kopie auf diesen Stand: `abn/` mit `abn/mess`, `abn/bild`, `abn/netz`.

```bash
cd analysis/befunde_runde41/form/rohbau_final
export PYTHONIOENCODING=utf-8
python rf_bau.py stand_final.json --tag ende --voll     # FORM.EMD, FORM.TIM, form.json, atlas.json, lauf/ende (mit zw/ruhe.json, zw/marken.json)
bash rf_abschluss.sh stand_final.json                    # alles neu, samt Ruhepose, Marken, Spiel, Messreihe
# Nacharbeit von vorn:
python rf_a1.py stand_vor_abn1/stand_final.json x1.json 30 250 120 80 && python rf_a1.py x1.json x2.json 0 120 100 80 && python rf_a1.py x2.json x3.json 0 60 100 80 && python rf_a2.py x3.json x4.json
```

`stand_final.json` zweimal gebaut: FORM.EMD, FORM.TIM und Bericht bytegleich. Gegenprobe von `stand_vor_abn1/` mit dem Endstand des Codes, die drei `rf_a1.py`-Stufen, rund 13 Minuten, `zw/a/g1.json` bis `g3.json`, Blaetter `lauf/g3/`: Umriss 0,978 / 0,919, Punkte 28/31, 7/7, Verdeckung 7/9, IoU 0,64-0,88. Der Weg traegt, er landet aber nicht auf der Abgabe: Die ist ueber die Kette a9 bis a13 mit Code-Aenderungen dazwischen entstanden und liegt etwas besser.

Abgabe: `FORM.EMD`, `FORM.TIM`, `form.json`, `stand_final.json`, `gerippe_final.json`, `atlas.json`, `atlas_blick.png`, `textur.png`, `vergleich_bild1.png`, `vergleich_bild2.png`, `teile_bild1.png`, `teile_bild2.png`, `rundum.png`, `rundum_hell.png`, `rundblick_pose1.png`, `rundblick_pose2.png`, `rundblick_ruhe.png`, `biegeprobe.png`, `spiel_pose1_r5080_s1.png`, `spiel_pose2_r5090_s11.png`, `spiel_ruhe_r5080_s1.png`, `spiel_pose1_nachstellung_r5080.png`, `o1_blatt.png`, `bericht.txt`, `lauf/ende/`. Messreihe in `mess/` (Skripte der Urteile, auf diesen Stand umgestellt), `abn/`.

Kein Commit. Geschrieben nur in diesem Ordner und in `re15_port/build/form_spiel/`, eigene Laufordner danach geloescht. Hauptbaum, `pri/`, `synchro/`, `rohbau_abnahme_1/` nicht beruehrt.
