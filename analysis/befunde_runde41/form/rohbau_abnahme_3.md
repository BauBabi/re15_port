# Abnahme 3 Rohbau final

Stand 2026-11-01. Ich bin unabhängiger Abnehmer und war am Rohbau nicht beteiligt. Arbeitsbaum r41_form, kein Commit, kein RE-Wert. Mein Arbeitsordner ist `rohbau_abnahme_3/`.
Geprüft habe ich `rohbau_final`: FORM.EMD md5 71e7f298, FORM.TIM md5 f98623f4, stand_final.json md5 bfff27f6. Das ist der Stand aus Abschnitt 0 des Bauer-Dossiers, und er ist nach der Abnahme unverändert.

## 1. Urteil

**Bestanden, mit einer Auflage (A1).** Die Form trägt die Malerei. Alle Mängel der Abnahme 2 sind behoben, E1 bis E3 sind gebaut. Die Auflage A1 betrifft nur einen Bereich, den keiner der beiden Screenshots zeigt. Sie kann deshalb parallel zur Malerei erledigt werden, ohne dass eine Marke neu projiziert werden muss.

| Frage | Antwort |
|---|---|
| Würde der Nutzer beide nebeneinander als dieselbe Figur erkennen? | **Ja.** Bild 1 erkennt man sofort: Haltung, Masse und Umriss sind deckungsgleich (IoU 0,980). Bild 2 auch: dunkle Kuppel mit Bogen, beide Arme, große Klauenhand unten links. `bild/neben_bild1.png`, `bild/neben_bild2.png` |
| Liegen seine Teile an ihrem Platz? | **Ja, acht von acht.** In Bild 1 stehen Schwanz mittig von oben bis zum rosa Ende, linkes Bein links, rechtes Bein mittig, rechter Arm rechts, Rumpf mit rosa Stelle. In Bild 2 schaut das Gesicht in die Kamera, der rechte Arm läuft nach links unten in die Klauenhand, der linke nach rechts unten in die Klaue. `pruefung/teile_bild*.png`, `bild/kamtafel_bild*.png` |
| Ist es aus jeder Richtung ein starkes, schlüssiges Tier? | **Ja, mit einer Einschränkung.** Pose 1 und die Ruhepose lesen sich als schweres, tiefes Echsentier mit langen Klauenarmen, Kiefer und langem Schwanz, Pose 2 als sitzendes Tier. Es gibt keine Deckel, keine Löcher und keine Stufe im Spielumriss. Einschränkung: Der Schwanz beginnt als eigener Strang auf der Rückenmitte statt am Becken (A1) |

## 2. Auflage A1: Schwanz beginnt auf der Rückenmitte (klein, Form)

- **Messung** (`mess/schwanzwurzel.txt`): In Pose 1 und in der Ruhepose liegt der Schwanz von x -1000 bis +500 als Strang 220 bis 364 Einheiten über dem Rumpfdach. Er taucht erst bei rund 45 % der Rumpflänge (von hinten gemessen) in den Rücken. Der Deckel aus Abnahme 2 ist weg, die Wurzel endet jetzt als spitzer Kegel auf dem Rücken.
- **Am Bild:** Im gefärbten Ton sieht man einen Schlauch, der über Becken und Hüfte gelegt ist (`bild/nah_wurzel.png`, `bild/far_clip0.png`, `bild/far_clip2.png`). Im ungefärbten Ton von der Seite ist es ein hochgesetzter Schwanz mit Naht auf dem Rücken (`bild/ton/ton_c2_az270_el10.png`). Im Spiel bei dunkler Textur ist es nur ein erhöhtes Hinterteil (`bild/spiel_dreh_hell.png`).
- **Was die Bilder verlangen** (`mess/s1probe.txt`, `mess/s1x.txt`): s1 liefert in Bild 1 die oberste Kante (x 290-330, 12-23 px), und die liegt im Nutzerumriss `tail`. Ein Schwanz über dem Becken ist also verlangt. Die Kamera von Bild 1 blickt aber fast längs der Körperachse. Darum fallen Modell-x -780 bis +378 auf dieselben Bildpunkte, und der Teil über dem Becken erreicht y 185 schon allein.
- **Probe nur im Speicher** (`a3_gratprobe.py`, `a3_gratprobe2.py`, `mess/gratprobe.txt`): Ich habe die s1-Ecken vor Modell-x 0 (Hüftmitte, Pose 1) um 400 Einheiten in den Rumpf gesenkt. In Bild 1 bleibt die IoU bei 0,9798 auf 0,9797, es fehlen 6 Zielpixel, der Schwanzumriss bleibt zu 0,9956 gedeckt, und die Oberkante x 290-316 ändert sich nicht. Bild 2 hat 53 px Unterschied, davon 0 Zielpixel. Gegenprobe ab Modell-x -200: Dann fehlen 258 Zielpixel an der Kuppe, also liegt die Grenze bei der Hüftmitte. Die Behauptung des Bauers, Bild 1 verlange den Grat, gilt damit nur für den Teil über dem Becken.
- **Weg:** Den Kegel von s1 vor der Hüftmitte in den Rumpf führen, sodass der Schwanz oben aus dem Becken wächst. Die Probe am Ton steht in `bild/gratprobe_ton.png`: Der Rücken läuft ruhiger, der Ansatz sitzt hinten. Sie ist roh gesenkt, ohne neue Normalen und ohne UV. Nicht im Spiel geprüft, nicht durch `form_pruefung.py pruefe` gelaufen.

## 3. Mängel der Abnahme 2: behoben

| Mangel | Stand | Beleg |
|---|---|---|
| M1 Schwanzwurzel als Stamm | Der Deckel ist weg. Die Deckelsuche der Abnahme 2 lief unverändert auf diesem Stand: Der vordere s1-Deckel (0,36 m², frei 0,75) fehlt, der hintere Rand (0,30 m²) ist mit frei 0,00 verdeckt. Im Spiel gibt es in 12 Drehungen keine Stufe | `mess/deckel.txt`, `bild/spiel_dreh_hell.png`. Rest: A1 |
| M2 Hinterenden | Die Unterschenkel-Deckel (0,30 m², frei 0,62-0,75) sind weg, die Ferse ist ein Kegel | `mess/deckel.txt`, `bild/nah_fuss.png` |
| M3 Hand auf der Kante | Die rechte Hand liegt in Pose 1 flach auf dem Boden, die Krallen zeigen nach vorn | `bild/nah_handr.png` |
| M4 / E2 Handverhältnis | Gebaut ist 1,32, die Masken belegen 1,27 (1,21-1,35). Das Rechenwerk des Bauers habe ich nicht nachgerechnet | Bauer-Dossier 0.2 |
| M5 Kopf | Krempe und Halsfächer sind frei weg. Der Halsfächer (0,65 m²) ist mit frei 0,00 verdeckt | `mess/deckel.txt`, `bild/nah_kopf.png` |
| E1 Masse rechts | Gebaut: `rand_rechts` +0,5 px | Bericht |
| E3 Kerbe | KERBE, KERB_S, KERB_O und KERB_U sind in Ordnung. Am Bild ist der Keil frei, Masse darüber und darunter sind gedeckt | `bild/kerbe_arm_rechts.png` |

Freie Deckelfläche gesamt: 8,52 / 7,88 / 9,53 m² (Abnahme 2: 13,24 / 11,70 / 13,92). Das Netz ist aus FORM.EMD dicht (32 von 32), die Windung ist einheitlich, es gibt 0 offene Kanten (`mess/netz_dicht.txt`).

## 4. Was im Bild noch abweicht

Form:
- Bild 1: Im Original zerfällt die Figur in Lappen und Knoten. Am deutlichsten ist das am linken Bein mit seiner gewellten Außenkante und drei bis vier Knollen, und am Schwanz mit Ringbändern. Im Modell sind das glatte Röhren. Ein Teil davon ist Form, ein Teil Malerei.
- Bild 1: Die Armkuppe liegt 4,9 px über der Sollkante, der Boden L1/L2 hat 6 und 11 px Treffer, je 1,4 px tief.
- Bild 2: Die Spitzen der linken Klaue sind je 7 px zu kurz (`bild/klaue_links_b2.png`). Leer bleiben oben links 851 px (schwarz auf schwarz) und unter der Hand 265 px dunkel. Die helle Lücke links mit 498 px ist laut Y 91 kein Figurteil.
- Von allen Seiten: Die Oberarme sind runde Kugeln mit einer Falte zum Rumpf. Von vorn wirkt das in Ruhe und in Pose 1 affenartig, im Ton aus Kamera 2 auch das Gesicht (`bild/ton_clip2.png` az 0, `bild/kamtafel_bild2.png`). Das ist eine Beobachtung, kein Widerspruch zu den Bildern. Kopf und Gesicht sind frei gesetzt.

Malerei (nicht bewertet): Z5 ist zu klein und zu rund (im Original ein breites Dreieck), die Bänder sind schwächer, der Arm vorn rechts (G) ist im Original hell gestreift und im Render dunkel. Die Textur ist so dunkel, dass die Figur im Spiel nur als Umriss liest.

Bühne: In Schnitt 11 steht das Feuer des Ports vor Hand und Unterarm (`bild/spielfilter_bild2.png`). Für Bild 1 gibt es keinen Schnitt, die Nachstellung in ROOM5080 zeigt dieselbe Haltung (`bild/spielfilter_bild1.png`).

## 5. Was ich selbst gemacht habe

| Prüfung | Ergebnis | Beleg |
|---|---|---|
| `form_pruefung.py pruefe form.json` | Bericht bytegleich mit `rohbau_final/bericht.txt`. Umriss 0,980 / 0,917, Punkte 31/31 und 7/7, Boden und Verdeckung 5/7 und 9/9, Zeichnung 8/9 (Z5) und 7/7. Teile-IoU Bild 1 0,68-0,87, Bild 2 0,70-0,83 | `pruefung/` |
| Original neben gefiltertem Render, normal und Gamma 0,5 mit Nutzerumrissen | Dieselbe Figur, die Teile am Platz. Heller Fleck (316-332, 290-302): Original Y 116-217, Render 123-226. Dunkelzone zwischen den Armen: Original Y 28-40, Render 18-30. Beides kein Mangel | `a3_neben.py`, `bild/neben_bild*.png` |
| Ton in Blender Workbench mit Tiefenpuffer, Netz nur aus FORM.EMD, 3 Clips × 17 Blicke, grau und je Teil gefärbt | Geschlossenes Tier, links und rechts richtig (rechter Arm erscheint von vorn links im Bild). Die Sägezacken in den Bauer-Bildern kommen vom Maler ohne Tiefenpuffer, hier gibt es sie nicht | `a3_netz.py`, `a3_ton.py`, `bild/ton_clip*.png`, `bild/far_clip*.png` |
| Ton aus den Screenshot-Kameras mit Nutzerumrissen | Teile am Platz, Gesicht zur Kamera. Lesehilfe mit genäherter Ausrichtung, keine Messung | `a3_kamton.py`, `bild/kamtafel_bild*.png` |
| Nahbilder Wurzel, Hand, Fuß, Kopf | Abschnitt 3 und A1 | `bild/nah_*.png` |
| Deckelsuche der Abnahme 2, unverändert | Abschnitt 3 | `mess/deckel.txt` |
| s1-Beitrag und Grat-Probe im Speicher | A1 | `mess/s1probe.txt`, `mess/s1x.txt`, `mess/gratprobe.txt`, `bild/gratprobe_ton.png` |
| Echtes Spiel, 14 eigene Läufe mit `form_spiel.sh` (gdigrab, Sitzung aktiv): Pose 2 an der Lage von Bild 2 in Schnitt 11, die Nachstellung von Bild 1 in ROOM5080, 3 Clips × 4 Gier in Schnitt 11 | Alle rc=0, jeweils `Bahnsteig-Form: 32 Meshes, 32 Knochen, 3 Clips`, Typ 0x30 an der angeforderten Lage und Gier. Die Figur ist geschlossen, ohne Stufe. Das Spielbild durch `video_filter` neben dem Original zeigt dieselbe Figur | `a3_spiel.sh`, `mess/spielreihe.log`, `spiel/`, `bild/spiel_dreh_*.png`, `bild/spielfilter_bild*.png` |

## 6. Grenzen

Nicht geprüft: bewegte Clips, Tick-Clips (`FE.pruefe` warnt weiter), Kasten, Schatten, Vordergrundmasken, PSX-Last, das E2-Rechenwerk. Die Grat-Probe ist roh und nicht gebaut. Mein Ton aus Kamera 1 ist gegen die Prüfbühne um einige Pixel versetzt, er ist nur Lesehilfe.

## 7. Dateien und Ordnung

Skripte `a3_*.py` und `a3_*.sh`, dazu Kopien von `a2_deckel2.py`, `a2_deckel_kern.py` und `a2_netz_lib.py`. Messungen in `mess/`, Bilder in `bild/`, Spielbilder in `spiel/`, Prüfung in `pruefung/`, alles unter `rohbau_abnahme_3/`. Ich habe nur eigene PIDs gestartet (`re15_pc_form_*`) und meine 14 eigenen Laufordner unter `re15_port/build/form_spiel/` gelöscht. Fremde Prozesse habe ich nicht angefasst.
Unverändert sind FORM.EMD und FORM.TIM, `rohbau_final/`, `shared_assets/`, `pri/`, `synchro/` und der Hauptbaum.
