# Abnahme 1 Rohbau final

Stand 2026-11-01. Unabhaengiger Abnehmer, am Rohbau nicht beteiligt. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert. Geprueft: `rohbau_final`, FORM.EMD md5 0d717025, FORM.TIM md5 8ad6f5f9. Blaetter, Skripte, Messungen: `rohbau_abnahme_1/`.

## 1. Urteil

**Nicht bestanden, aber nah.** Kein Neubau noetig: eine Posenkorrektur, zwei Formkorrekturen, zwei Kleinigkeiten, eine Nutzerfrage. Das gehoert vor die Malerei, denn die Marken werden auf die Geometrie zurueckprojiziert.

| Frage | Antwort |
|---|---|
| Dieselbe Figur nebeneinander? | **Ja.** Bild 1: gleiche Haltung, Masse, Lage, sofort erkennbar. Bild 2: gleiche dunkle Masse, Bogen, beide Arme, Klauen. 1:1 ist es nicht, das liegt zum groessten Teil an der Malerei |
| Teile an ihrem Platz? | **Sieben von acht ganz, `part_of_Body` nur teilweise:** Sein rechter Auslaeufer zum Arm fehlt (M1) |
| Aus jeder Richtung starkes, schluessiges Tier? | **Weitgehend ja.** Pose 1 ja. Ruhe ja, aber Dach flach. Pose 2 traegt die Brust als Trommel auf dem Ruecken (M3) |

Blatt auf einen Blick: `rohbau_abnahme_1/bild/maengel_blatt.png`.

## 2. Maengel

### M1 Achselfenster Bild 1, hoch. Beheben

Zwischen Koerper und rechtem Arm steht im Render ein helles Fenster, im Original ist dort Figur. Das ist die groesste Formabweichung im Bild und schon in voller Bildgroesse zu sehen.

- Flaeche 831 px, (340-381,284-321), nur 8 % davon liegen im Modell (`abn_fenster.py`). Spalten x 362-380: Original bis y rund 318 dunkel, Y 25-50, Render ab y 287-300 hell, Y 225.
- Figur, nicht Raum: im Fenster RGB (25,23,41), Arm (29,28,47), Raum rechts in denselben Zeilen (158,149,147), links (153,107,80), Boden (177,195,205).
- `part_of_Body` leer 407 px bei (361-381,283-318). `FORMBLATT2` selbst sagt: rechte Kante laeuft zum Arm, Unterkante y 317.
- Das Dossier nennt das "Spalt schmal, aber noch offen". 41 x 37 px sind nicht schmal.

**Weg, gerechnet nur im Speicher (`abn_probe.py`, `bild/probe_kopf_35.png`):** Der Kopf haengt verdeckt genau hinter dem Fenster, Kopf (354,256), Kiefer (348,274), und ist in Pose 1 frei. Hals um Gelenk 21 tiefer, nur Clip 0:

| Hals tiefer | Fenster gedeckt | helles Original gedeckt | Kopf tiefster Punkt |
|---|---|---|---|
| 0 | 0,20 | 0,15 | 979 |
| 25 Grad | 0,66 | 0,15 | 492 |
| 35 Grad | **0,74** | 0,19 | 305 |
| 45 Grad | 0,74 | 0,30 | 164 |

Bild 2 und Ruhe bleiben unberuehrt, die Form auch. Der Rest ist ein Schlitz am Arm (375-381,287-320), siehe M2.

### M2 Rechter Arm, Aussenkante und O1, hoch. Nutzerfrage, Rest beheben

- `b1.rand_rechts` (429,272): -13,5 px gegen Grenze 3,6, harte Kante. Dunkle Masse (405-431,255-300) liegt ausserhalb des Modells, gedeckt 0,27. In `bild/probe_kopf_35.png` sichtbar rechts neben dem weissen Umriss.
- Unabhaengig von O1 und innerhalb der Nutzermaske: `right_arm` leer 97 px bei (411-419,308-337), `b1.fr_y324_rechts` -3,5 gegen 2,8. Die ganze Aussenkante liegt zu weit innen.
- Das widerspricht der Lesung nicht, der Arm steht ja dort. Die Maske endet davor, aber Handschnitte legen nicht jede Kante fest. Deshalb dem Nutzer `urteil_bild/o1_blick.png` und `bild/probe_kopf_35.png` zeigen.
- Gelenke Bild 1: Schulter (397,263), Ellbogen (397,296). Der Oberarm zeigt fast zur Kamera. Ein Ellbogen weiter nach aussen koennte Aussenkante und Schlitz aus M1 zugleich schliessen. Das ist nicht gerechnet, nur ein Hinweis.

### M3 Brust als Trommel, mittel. Beheben

- Pose 2: Knick Wirbel/Brust 47 Grad, Brust/Hals 68, Huefte 51 Grad schraeg, Brust nur 13. Die Hinterkante der Brust faellt 492 Einheiten auf 40 Einheiten Weg (x 309), Dach 520 flach auf Hoehe 3105. Im Ton eine Kiste auf dem Ruecken: `netz/nah_pose2_gesamt.png`, `netz/nah_pose2_ruecken.png`.
- Ruhe: Dach 960 Einheiten flach auf Hoehe 2677, im Ton eine Platte: `netz/nah_ruhe_gesamt.png`.
- Im Spiel liest es sich auf der dunklen Textur nur als harter Buckel: `spiel2/rund_pose2_2.png`. Der Umriss bleibt aber kantig, und die Malerei hilft dort nicht.
- Weg: Hinterkante schraegen, Ruecken woelben. Sie ist in Bild 2 von der Kamera abgewandt, der Umriss bleibt.

Im Dossier und in `rundblick_pose2.png` ist die Stelle zu sehen, aber nicht als Mangel genannt.

### M4 Aussenfinger rechte Hand, mittel. Beheben

Finger 29 ist kein Finger, sondern ein Blatt. Laenge 1170, Breite 592, Dicke 514, Geschwister 19 und 20 Breite 317 und 329, Dicke 281 und 318. Links dasselbe, 474 gegen 251-261. Der Sporn trifft (145,353) auf 4,1 px, und das soll bleiben. Dafuer schlank und gekruemmt, Rest der Handmaske anders fuellen. In Pose 2 steht er als Segel hoch: `netz/nah_hand_r_pose2.png`, `netz/kamton_bild2.png`.

### M5 Kerbe und Kuppe Bild 1, klein

Oberkante: Kerbe zwischen Schwanz und Koerper zu tief, x 330 Modell 210 gegen Original 195, x 335 207 gegen 199. Die Kuppe kippt nach links: `scheitel_x300` -5,5, `_x305` -4,5, `_x322` +9,5. Das Original laeuft dort als eine Dachlinie durch, das Modell zeigt zwei Buckel.

### M6 Ruhe: Schwanz schwebt, klein

Tiefster Punkt des Schwanzes 433 ueber dem Boden, Bein 25, Hand 0-8. Das Dossier sagt "laeuft zum Boden". In Pose 1 sind es 367, das Ende liegt dort ausserhalb des Bilds, unkritisch.

### Beobachtung, kein Mangel

- Von vorn in Ruhe (`netz/nah_kopf_ruhe.png`) liest sich die Gestalt affenartig: spitze Schulter-Eier, Knoechelgang, Ringkapuze. Der Nutzer hat den Gorilla verworfen. Das ist ein Risiko, frei gesetzt, kein Widerspruch zu den Bildern.
- Zug Q: Bogenpunkt (290,172) liegt 21 px neben dem Kopf auf der Brust, die uebrigen 1-6 px. Die Angabe "Kreis zu 1,00 gedeckt" meint den kleineren Kreis r62, nicht den Bogen. Bewegt sich der Kopf, reisst der Bogen. Fuer die Malerei wichtig.

## 3. Teile am Bild

Zahlen aus dem eigenen Lauf. `pruefung/bericht.txt` ist bytegleich mit dem Bericht des Bauers.

| Teil | Bild | IoU | Deckung | Am Bild |
|---|---|---|---|---|
| tail | 1 | 0,89 | 0,95 | sitzt, rosa Ende am Ort. Kuppe M5 |
| left_leg | 1 | 0,67 | 0,94 | sitzt. Aussenkante bis auf rund 3 px am Soll (`mess/lappen.txt`). Niedrige IoU kommt allein vom Keil O3, der im Original dunkel ist. Nutzerfrage des Bauers bleibt |
| right_leg | 1 | 0,77 | 0,92 | sitzt |
| right_arm | 1 | 0,76 | 0,94 | sitzt, Aussenkante M2 |
| part_of_Body | 1 | 0,81 | 0,89 | **Auslaeufer zum Arm fehlt, M1** |
| face_and_upper_body | 2 | 0,84 | 0,88 | sitzt, Dom und Bogen am Ort. Fehlflecken nur dunkel auf dunkel |
| left_arm_front | 2 | 0,78 | 0,81 | sitzt, Klauen 3,6 und 5,8 px |
| right_arm_front | 2 | 0,75 | 0,87 | sitzt, Fledermausfluegel M4 |

Rest des Berichts: Umriss-IoU 0,950 / 0,940, Punkte 21/31, 6/7, Boden 1/3, Verdeckung 7/9.
Raute L2 stimmt: Zeile 352 frei 281-306 gegen hell 288-334, Zeile 358 281-294 gegen 284-303, ab 364 beide zu. Der zweite grosse Fleck der Karte (314-366,407-458) ist Schlagschatten im Original, die Platte hat keinen. Kein Mangel.

## 4. Was ich selbst gemacht habe

| Pruefung | Ergebnis | Beleg |
|---|---|---|
| `form_pruefung.py pruefe` | bytegleich mit Abgabe | `pruefung/` |
| Original neben gefiltertem Render, ohne Einsetzen, also ohne schmeichelnden Saum | oben | `bild/ganz_bild*.png`, `bild/nah_*.png` |
| Karte Original dunkel/Render hell ohne Nutzermasken | fand M1 | `bild/fenster_bild*.png` |
| Netz aus FORM.EMD, ohne Generator | 3208 Dreiecke, 0 offene Kanten, Durchblick 0 in 24 Blicken je Clip | `mess/netz.txt` |
| Ton mit Tiefenpuffer, 24 Blicke je Clip, dazu aus den Screenshot-Kameras | Form tragfaehig, M3, M4 | `netz/ton_*.png`, `netz/kamton_bild*.png` |
| Spiel, 31 Laeufe, alle rc=0, gdigrab, je `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips` | Laedt, geschlossen aus jeder Drehung | `spiel_tafel.png` |
| Spielbild Bild 2 durch `video_filter` neben das Original | gleiche Lage, gleiche Masse | `bild/spiel_ganz_bild2.png` |
| Blick von Bild 1 im Spiel nachgestellt | liest sich als dieselbe Figur | `spiel2/b1blick_pose1.png`, `bild/spiel_ganz_bild1.png` |

Nachstellung Bild 1: Die Passlage ist nur in Cut 1 sichtbar, Cut 2 sieht sie nicht, obwohl er dem Auge von Bild 1 am naechsten steht. Deshalb Figur auf die Blickachse von Cut 1 gesetzt, Abstand 5976 wie Bild 1, Gier so, dass der Kamera dieselbe Seite zugewandt ist: `-20711,-15984,756,0,0,1` in ROOM5080. Die Kamera steht dort hoeher (3040 gegen 2180) und sieht deshalb flacher von oben.

Bestaetigt: Durchblick 0, offene Kanten 0, Boden Hand und Fuss je Pose hoechstens 25, Bericht, Spiel laedt.

## 5. Fuer die Malerei, nicht bewertet

- Haende rosa in ROOM5080 und Bild 1 (`spiel/ruhe_r5080_s2_g1112.png`), im Original dunkel. Der Rotton fuer Bild 2 gehoert nicht in die Hand in Bild 1.
- Zwei helle Striche in Bild 1 (345-370,217-235) auf Knochen 7 und 10, im Original dunkel.
- Rote Flaechen am Kopf eckig, Raute in Bild 2 deutlich, das Video zeigt sie nicht. Im Ton ist das Gesicht da (`netz/kamton_bild2.png`), die dunkle Textur verdeckt es.
- Lappen, Baender, Glanzpunkte im Bein sind Binnenform, nicht Umriss.
- Laut Bericht offen: Z4, Z5, Z9, Q5.
- Kein Thema des Modells: Feuer des Ports steht in Schnitt 11 vor Hand und Unterarm, im Video nicht (`bild/spiel_ganz_bild2.png`).

## 6. Fragen an den Nutzer

1. O1: Gehoert die dunkle Masse rechts neben dem Arm zur Figur? `urteil_bild/o1_blick.png`, `rohbau_abnahme_1/bild/probe_kopf_35.png`. Das Bild spricht dafuer.
2. Die Fragen 2-4 aus `rohbau_final/DOSSIER.md` bleiben offen: Hand groesser, sitzende Haltung, Keil links.

## 7. Grenzen

Nicht geprueft: bewegte Clips, Tick-Clips, Kasten, Schatten, Vordergrundmasken, PSX-Last. Bild 1 gibt es im Spiel nur nachgestellt. Fensterflaeche nach Oeffnung 3 x 3, die Probe zaehlt ohne Oeffnung 863 px. Die Kopf-Probe aendert keine Datei und ist nicht im Spiel gelaufen.

## 8. Dateien

Skripte `abn_*.py`, `abn_spielreihe*.sh`, Messungen `mess/`, Bilder `bild/`, `netz/`, `spiel/`, `spiel2/`, `pruefung/`, alle unter `rohbau_abnahme_1/`. Nur eigene PIDs, eigene Laufordner geloescht, keine Prozesse uebrig. Hauptbaum, `pri/`, `synchro/`, `rohbau_final/` nicht beruehrt.
