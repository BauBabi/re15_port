# Abnahme 2 Rohbau final

Stand 2026-11-01. Unabhaengiger Abnehmer, am Rohbau nicht beteiligt. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert. Geprueft: `rohbau_final`, FORM.EMD md5 93f571e7, FORM.TIM md5 1e045a30. Blaetter, Skripte, Messungen: `rohbau_abnahme_2/`.

## 1. Urteil

**Nicht bestanden, aber knapp.** Die Figur stimmt, alle sechs Maengel der Abnahme 1 sind behoben. Durchgefallen ist sie an der zweiten Frage: Von vorn und von der Seite traegt sie einen abgesaegten Stamm auf dem Ruecken. Kein Neubau noetig, zwei kleine Formkorrekturen und eine Posenkorrektur. Sie gehoeren vor die Malerei, Farbe heilt keinen Umriss.

| Frage | Antwort |
|---|---|
| Dieselbe Figur nebeneinander? | **Ja.** Bild 1 sofort, gleiche Haltung, Masse, Lage. Bild 2 ja: gleiche dunkle Masse, Bogen, beide Arme, Klauen. 1:1 ist es noch nicht, das liegt fast ganz an der Malerei |
| Teile an ihrem Platz? | **Ja, acht von acht.** Gesicht zur Kamera in Bild 2, Schwanz mittig in Bild 1, links und rechts stimmen. Zwei Stellen: `right_arm_front` fuellt den linken Lappen nicht, `part_of_Body` traegt rechts unten den Kopf statt Rumpf |
| Aus jeder Richtung starkes, schluessiges Tier? | **Noch nicht.** Pose 2 ja. Pose 1 und Ruhe von hinten ja, von der Seite und von vorn nein: M1. Dazu die Hinterenden M2 und die Hand M3 |

Blatt auf einen Blick: `rohbau_abnahme_2/bild/maengel_blatt.png`.

## 2. Maengel

### M1 Schwanzwurzel als abgesaegter Stamm, hoch. Beheben

Der Schwanz waechst nicht aus dem Koerper. Er liegt als Rohr auf dem Ruecken und endet mitten auf dem Tier in einem flachen Deckel.

- Bauform (`mess/ringe.txt`): Ring Radius 411 bei x 470, Faechermitte nur 30 Einheiten davor bei x 500. Das ist eine Scheibe.
- Deckel `s1`: 0,36 m2, Durchmesser 0,68 m, Randknick 85 Grad, frei 0,75 in Pose 1 auf Hoehe 2,63, frei 0,50 in Ruhe auf Hoehe 2,21 (`mess/deckel2.txt`).
- Rueckenlinie (`mess/mass.txt`): Der Schwanz liegt in Pose 1 von x -600 bis +500 ueber dem Rumpf, Stufe 219 bis 372, dann Abbruch. In Ruhe von -400 bis +300, Stufe 248 bis 357.
- Das ist kein Toneffekt. Im echten Spiel steht die Stufe im Umriss: `spiel/hell_tafel.png`, Tafeln `p1_g1024`, `ruhe_g1024`, `ruhe_g3072`.
- Aus Bild 1 nicht zu sehen, deshalb bestand die Figur jede Messung. Abnahme 1 hat es uebersehen. `rohbau_final/DOSSIER.md` Abschnitt 5 nennt "Schwanz als Buckel in Ruhe" erledigt, das gilt nur fuer die Spitze.
- Ton: `bild/nwurz0.png`, `bild/nrueck0.png`, `bild/nrueck2.png`.

**Weg, gerechnet nur im Speicher (`a2_probe2.py`, `mess/probe2.txt`, `bild/probe_vergleich.png`):** Nur die Faechermitte von Knochen 1 versetzen, (500,219,0) auf (1240,547,0). Der Deckel wird zum langen Kegel, der in den Ruecken taucht.

| | Bild 1 | Bild 2 |
|---|---|---|
| Maske verschieden | 50 px, alle von den Fersen aus M2, Oberkante 0 px | 0 px |
| Umriss-IoU vorher / Probe | 0,9782 / 0,9784 | 0,9246 / 0,9246 |

Nicht verjuengen: Die erste Probe (`a2_probe.py`, `mess/probe.txt`) nahm die vorderen Ringe enger, dabei gingen 357 px der Kuppe verloren, (297-342,186-213). Die Ringe tragen die Oberkante in Bild 1, deshalb liegt die Wurzel dort. Sie bleiben, nur der Faecher aendert sich. Normalen und UV des Kegels sind in der Probe nicht gemacht, und spitze Faecherdreiecke sind nicht auf Risse geprueft.

### M2 Hinterenden, mittel. Beheben

- Beide Unterschenkel enden unten in einem flachen Deckel: je 0,30 m2, Durchmesser 0,61 m, Randknick 73, Hoehe 0,25-0,35, in allen drei Posen frei 0,62-0,75 (`mess/deckel2.txt`). Fuer die Ferse ist das die Blickrichtung von Bild 1, von hinten ein Achteck wie ein gesaegtes Scheit: `bild/nfuss0.png`.
- Knoechel so dick wie das Knie: Ring-Radius 340-364 unten, 391 oben (`mess/ringe.txt`).
- Ruhe: Das Hinterbein liest sich nicht als Bein, sondern als zwei gestapelte Kloetze unter der Huefte, der Oberschenkel ist nicht zu sehen: `bild/nfuss2.png`, `bild/ton_ruhe_el08.png`. In Pose 1 liest sich dasselbe Bein gut. Das ist die Pose, nicht die Form.
- Zehen ragen nur als Dornen vor, gegen die Klauen der Haende wirken die Fuesse wie Pantoffeln.

**Weg:** Faechermitte herausziehen. Probe mit +95: 45 px in Bild 1 bei (218-231,437-444), Rest 5 px. Ruhe-Bein von Pose 1 ableiten, Huefte heben.

### M3 Rechte Hand Pose 1 auf der Kante, mittel. Beheben

Handflaeche 60 Grad gegen die Waagerechte, alle anderen Haende in allen Posen hoechstens 11 (`mess/mass.txt`). Fingerspitzen bis Hoehe 932 und 795. Von der Seite ein senkrechter Stapel Klingen statt einer tragenden Pranke: `bild/nhand0.png`. Aus Bild 1 unsichtbar. Das Dossier nennt die Stelle selbst.
Weg, nicht gerechnet: Rolle verkleinern, Finger einkrallen statt die Hand zu kippen, die Finger haben eigene Knochen.

### M4 Linke Hand kleiner, Nutzerfrage

Rechts 1650-1800 Einheiten vom Gelenk zur Spitze, links 1050-1150, Faktor 1,5 (`mess/mass.txt`). In Ruhe von vorn und oben sofort zu sehen: `bild/ton_ruhe_el35.png`. Die Masken verlangen rund 124 gegen 82 px, bei Tiefe 5617 gegen 6508 bleibt ein Faktor um 1,3. Etwas davon ist also belegt, nicht alles. Frage 2 des Dossiers bleibt offen, die Antwort sollte vor der Malerei feststehen.

### M5 Kleinigkeiten am Kopf, klein

- Hals-Faecher in Ruhe halb frei: Durchmesser 1,01 m unter der Brust, frei 0,50 (`mess/deckel2.txt`).
- Kopf von schraeg hinten oben Kegel mit Krempe: `bild/nrueck1.png`. Auf Augenhoehe unauffaellig: `bild/nhut.png`.
- Flache Stirn- und Kieferdeckel: `bild/nkopf2.png`. Frei gesetzt, im Spiel bisher kaum sichtbar, unter Farbe und Licht wird es mehr.
- In Pose 2 stecken 19 % der Ecken des rechten Oberarms im Kopf (`mess/mass.txt`), im Bild 2 als schmaler Spalt am Kiefer zu sehen (`bild/kamton_bild2.png`).

### Beobachtung, kein Mangel

- Von vorn liest sich die Gestalt in Ruhe und Pose 1 weiter affenartig: Schulter-Eier, Knoechelgang (`bild/ton_ruhe_el08.png`, az 0). Von der Seite ist es ein schweres Echsentier. Der Nutzer hat den Gorilla verworfen. Das ist ein Risiko, kein Widerspruch zu den Bildern.
- Schulter-Eier stehen von hinten durch eine tiefe Falte vom Rumpf ab, sie fliessen nicht in den Ruecken (`bild/nrueck2.png`).

## 3. Maengel der Abnahme 1

Alle behoben, an meinen eigenen Blaettern geprueft, nicht nur am Bericht.

| Mangel | Stand | Beleg |
|---|---|---|
| M1 Achselfenster | Behoben. Unterkante Figur liegt in Modell und Original bei y 312-320 | `mess/profil.txt` |
| M2 Aussenkante, O1 | Behoben, `rand_rechts` -1,5, `fr_y324_rechts` -1,5. Frage O1 bleibt | Bericht, `bild/oberkante_nah.png` |
| M3 Brust als Trommel | Behoben, Ruecken Pose 2 ein Bogen ohne Abbruch, Dachlinie 2609 bis 3100 bis 2455 | `mess/mass.txt`, `bild/nrueck1.png` |
| M4 Aussenfinger | Behoben, schlank | `bild/kamton_bild2.png` |
| M5 Kerbe, Kuppe | Behoben, Oberkante x 345-375 liegt 0-3 px am Original, unabhaengig von den Masken gemessen | `mess/oberkante.txt` |
| M6 Schwanz Ruhe | Behoben, tiefster Punkt Hoehe 2 | `bild/ton_ruhe_el08.png` |

Vom Rest des Dossiers habe ich nachgeprueft: Bericht bytegleich, Netz dicht, Windung einheitlich, Spiel laedt. Biegeprobe und Spiegelabstand nicht wiederholt.

## 4. Teile am Bild

Zahlen aus dem eigenen Lauf, `pruefung/bericht.txt` ist bytegleich mit dem des Bauers.

| Teil | Bild | IoU | Deckung | Am Bild |
|---|---|---|---|---|
| tail | 1 | 0,87 | 0,96 | sitzt, rosa Ende am Ort |
| left_leg | 1 | 0,67 | 0,96 | sitzt, Lappen und Kerbe nur angedeutet. Niedrige IoU kommt vom Keil, der im Original dunkel ist |
| right_leg | 1 | 0,82 | 0,97 | sitzt, Innenkante in L2 rund 3 px zu weit |
| right_arm | 1 | 0,72 | 0,97 | sitzt, Masse rechts daneben haengt an O1 |
| part_of_Body | 1 | 0,80 | 0,87 | sitzt. Den Auslaeufer zum Arm fuellt der Kopf, das Bild trennt das nicht |
| face_and_upper_body | 2 | 0,82 | 0,86 | sitzt, Gesicht zur Kamera |
| left_arm_front | 2 | 0,79 | 0,83 | sitzt, Klauen 7,8 und 7,1 px |
| right_arm_front | 2 | 0,68 | 0,82 | sitzt, Spitzen treffen. Linker Lappen 896 px leer, das Original zeigt dort eine runde dunkle Masse, das Modell einen Faecher |

Bericht: Umriss-IoU 0,978 / 0,925, Punkte 31/31, 7/7, Boden 1/3 (L1 4 px, L2 49 px, Eindringen bis 3,6 px, innerhalb der Kante), Verdeckung 9/9, Zeichnung 8/9, 7/7. Diese Zahlen beweisen wenig, sie messen gegen das Passziel. Unabhaengig davon stehen die Karte ohne Masken und die Profile unten.

**Karte ohne Masken (`bild/flecken_bild1.png`, `mess/flecken.txt`).** Die Rotflecken sind fast alle kein Formfehler, das habe ich einzeln nachgemessen:
- Schmetterling (341-381,310-346), 772 px: Original RGB (83,110,142), blau wie Boden im Schatten (64,115,147), Figur waere (25,23,41). Schatten, die Buehne hat keinen.
- Streifen Oberkante rechts: ausgemalter Hintergrund heller als das Original. Die Kante selbst liegt 0-3 px, nur Armkuppe x 385-395 rund 5 px zu hoch.
- (315-383,393-465): Schlagschatten.
- Echter Rest: Rautenspitze L2 oeffnet 5-8 px zu frueh (x 292-300).

## 5. Was im Bild noch abweicht

Form:
- Bild 1: Das Original zerfaellt in Lappen und Wuelste, das Modell ist glatter, besonders das linke Bein. Teils Form, teils Malerei.
- Bild 2: Hand als Faecher gegen dunklen Ballen im Original, Sporn als duenne Nadel.
- Bild 1: Rechts oben rundet der Arm als Kugel, das Original zeigt dort eine schraege dunkle Kante, O1.
- Saegezacken an Ellbogen und Knie im Ton (`bild/nhut.png`), bei RE2-Bauart ueblich, klein.

Malerei, nicht bewertet: Bogen duenner und heller als im Video, Marken in Bild 1 zu rund und zu hart, Hand in Bild 1 am Rand rot, Bild 2 Rotton Unterarm, Textur so dunkel, dass die Figur im Spiel in Schnitt 11 fast nur als Umriss liest (`spiel/tafel.png`).

## 6. Was ich selbst gemacht habe

| Pruefung | Ergebnis | Beleg |
|---|---|---|
| `form_pruefung.py pruefe` | bytegleich mit Abgabe | `pruefung/` |
| Original neben gefiltertem Render, ohne Einsetzen, auch aufgehellt | oben | `bild/ganz_bild*.png`, `bild/nah_*.png` |
| Karte Original gegen Render ohne jede Maske, Profile | Abschnitt 4 | `a2_flecken.py`, `a2_profil.py`, `a2_oben.py` |
| Spiel, 15 eigene Laeufe (1 einzeln, 14 Reihe), gdigrab, alle rc=0, je `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, Lage und Gier im Log wie angefordert | Laedt, geschlossen aus jeder Drehung, Stufe M1 sichtbar | `spiel/tafel.png`, `spiel/hell_tafel.png`, `mess/spielreihe.log` |
| Spielbild durch `video_filter` neben das Original | selbe Figur | `bild/spiel_neben_1.png`, `bild/spiel_neben_2.png` |
| Netz nur aus FORM.EMD, 32 von 32 dicht, 0 offene Kanten, Windung einheitlich | in Ordnung | `mess/deckel.txt` |
| Ton in Blender, rechtshaendig uebertragen, 3 Clips, 8 Azimute, 2 Hoehen, dazu unten und oben | fand M1, M2, M3 | `bild/ton_*.png`, `bild/unten_oben.png`, `bild/far_*.png` |
| Ton aus den Screenshot-Kameras mit Nutzerumrissen | Teile am Platz | `bild/kamton_bild1.png`, `bild/kamton_bild2.png` |
| Deckelsuche, Rueckenlinie, Fuss, Hand, Eindringen | Abschnitt 2 | `mess/deckel2.txt`, `mess/mass.txt` |
| Zwei Proben im Speicher | Weg M1, M2 | `mess/probe.txt`, `mess/probe2.txt` |

Eigene Fehlgriffe, korrigiert: Der erste OBJ-Export spiegelte links und rechts (`(x,-z,-y)`, Determinante -1), vor dem ersten Blick behoben. Linker Fuss in Pose 2 sah nach senkrecht aus, die Messung gibt 8 Grad zum Boden, gestrichen. Den Schmetterling hielt ich zuerst fuer ein offenes Achselfenster, die Farbe zeigte Schatten. Erste Wurzel-Probe zerstoerte die Kuppe, verworfen.

## 7. Fuer die Malerei

- Erst M1 bis M3, Marken werden auf die Geometrie zurueckprojiziert. Von M1 betroffen ist nur die Vorderkappe von Knochen 1.
- Die Lage von Bild 1 hat keinen Schnitt, Nachstellung `-20711,-15984,756,0,0,1` in ROOM5080 aus Abnahme 1 traegt weiter: `spiel/p1_nachstellung.png`.
- Feuer des Ports steht in Schnitt 11 vor Hand und Unterarm, im Video nicht. Kein Thema des Modells.

## 8. Fragen an den Nutzer

1. O1: Gehoert die dunkle Masse rechts neben dem Arm zur Figur? `rohbau_final/o1_blatt.png`, `bild/oberkante_nah.png`. Die Kante bei x 429 ist hart, die Schiene dahinter beginnt erst bei x 431, das spricht dafuer. Beleg ist es nicht.
2. Hand: Darf die rechte Hand so viel groesser sein? `bild/ton_ruhe_el35.png`.
3. Die Fragen 3-5 aus `rohbau_final/DOSSIER.md` bleiben offen: Krallenzahl, sitzende Haltung, Keil links.

## 9. Grenzen

Nicht geprueft: bewegte Clips, Tick-Clips (`FE.pruefe` warnt weiter), Kasten, Schatten, Vordergrundmasken, PSX-Last. Bild 1 gibt es im Spiel nur nachgestellt. Die Proben aendern keine Datei, liefen nicht im Spiel und nicht durch `form_pruefung.py pruefe`, Netz nach dem Versatz nicht neu auf Risse gemessen. Weg M3 ist nicht gerechnet. Die aufgehellten Spielbilder sind Lesehilfe, kein Zielbild.

## 10. Dateien

Skripte `a2_*.py`, `a2_*.sh`, Messungen `mess/`, Bilder `bild/`, `netz/`, `spiel/`, `pruefung/`, alle unter `rohbau_abnahme_2/`. Nur eigene PIDs, eigene Laufordner geloescht, keine Prozesse uebrig. FORM.EMD und FORM.TIM unveraendert. Hauptbaum, `pri/`, `synchro/`, `rohbau_final/` nicht beruehrt.
