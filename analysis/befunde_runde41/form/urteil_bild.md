# Urteil Linse BILD

Stand 2026-11-01. Unabhaengiger Pruefer, an keinem Rohbau beteiligt. Arbeitsbaum r41_form, kein Commit. Kein RE-Wert.
Massstab: Bild, nicht Zahl. 10 = nach dem Filter nicht vom Original zu trennen, 5 = gleiche Pose und alle Teile am Ort, aber sichtbar eine andere Oberflaeche.

## 1. Ergebnis

| Rohbau | Punkte | Kurz |
|---|---|---|
| A Fleisch | 4,5 | Pose und Masse stimmen, aber Glockenfuss, blendendes Schwanzende, Stern und Bloecke im Gesicht, Loch im Rumpf |
| B Panzer | 5,5 | Dieselbe Figur erkennbar, bester Kopfbogen und Unterarmstreifen, grobe linke Hand |
| C Bildnah | 6,5 | Beste Figur im Bild, alle Teile sitzen, aber Rhombus zwischen Bein und Schwanz, in Bild 2 zu dunkel |

**Sieht man dieselbe Figur?** Bei C und B ja: Haltung, Masse und Teile sind dieselben. 1:1 ist keiner. Das Original wirkt in Bild 1 knotig und voller Glanz, alle drei wirken glatt mit hellen Flecken.

Rangfolge C vor B vor A gilt fuer beide Bilder. Abstand C zu B ist klein: In Bild 2 ist B bei Kopf und Unterarm besser, C bei beiden Haenden und der Ruhe der Flaeche.

## 2. Wie geprueft

Alle Blaetter unter `urteil_bild/`.

| Weg | Dateien | Wofuer |
|---|---|---|
| Blaetter der Rohbauten gelesen | `vergleich_*`, `teile_*`, `rundum_hell`, `spiel_pose2_*` | Ueberblick |
| Auflage: Modell auf die Umkehr des Originals gemalt, dann Filter | `auf_b1_figur.png`, `auf_b2_figur.png`, `auf_b*_*.png` | Hauptblatt fuer Oberflaeche und Marken, ohne Saum |
| Einsatz voll: Platte ohne Figur, dann Filter | `nah_b*_voll.png`, `gesamt_bild*.png` | Zeigt, wo das Modell wirklich fehlt |
| Einsatz eng | `nah_b*_eng.png`, `nah_b*_hell.png` | Gegenprobe |
| Umriss auf aufgehelltem Original | `kontur_b*.png` | Kanten gegen das Bild, nicht nur gegen die Handschnitte |

Falle: Die Auflage schmeichelt. Wo das Modell fehlt, bleibt dort die dunkle Original-Figur stehen. Den Rhombus von C sieht man nur in `nah_b1_unten_voll.png`. Die Platte hat umgekehrt einen hellen Fleck unter der Figur und keinen Schlagschatten, helle Saeume in Einsatz voll und eng sind also kein Modellfehler.

## 3. Was allen dreien fehlt

Das ist der groesste Rest, und er entscheidet 1:1 mehr als die Unterschiede zwischen den Rohbauten.

1. **Oberflaeche Bild 1.** Das Original zerfaellt in Lappen, Baender und Glanzpunkte in Blau, Tuerkis, Violett. Die Modelle sind glatte Roehren mit Flecken. Linkes Bein im Video drei bis vier runde Lappen, in keinem Rohbau so.
2. **Unterarm Bild 2.** Der gerippte Streifen (337,318) bis (262,395) ist nach dem Feuer das Auffaelligste am Bild: orange-rot, weisse Glanzpunkte, Querrippen. Kein Rohbau hat das. Alle zeigen matt violette Stuecke, das untere Drittel ab rund (283,362) fehlt.
3. **Rotes Licht Bild 2.** Hand im Video R 75, A 49, B 35, C 29. Linker Arm R 106 gegen 59, 54, 48. Am meisten fehlt Rot. Der Feuerschein fehlt, Hand und Arm stehen schwarz statt dunkelrot.
4. **Marken zu gross und zu hell.** Z5 im Video 15x9 px, Y 75, in allen rund 30x28, Y 155-196. Z3/Z4 Y 172/141 gegen 215-226/178-224. Z2 umgekehrt zu dunkel, 130 gegen 61-87.
5. **Keine Gesichtszuege.** Innen zeigt das Video schwach Rot und Blaugrau, aufgehellt `auf_b2_kopf_hell.png`. Kein Modell trifft das. Kein Fehler in der Form, aber Augen, Maul und Zaehne sind im Bild nicht zu sehen.
6. **Klauenhand als Fledermausfluegel.** Alle folgen dem Handschnitt bis in die Zacke, das V unter dem Feuer und der Sporn (145,353) stehen schwarz und scharf. Im Video ist die Hand eine dunkelrote Kuppel, Oberkante y 385-395, der Sporn nur dunkelrot (`kontur_b2_hand.png`). Der Sporn bleibt, weil der Umriss ihn fuehrt, sollte aber weich und rot werden statt schwarz.
7. **Dunkelzone zwischen den Armen Bild 2.** Nur 27-35 % gedeckt, sonst heller Boden. Mit Schatten pruefen.

## 4. Teile des Nutzers am Bild

| Teil | Original | A | B | C |
|---|---|---|---|---|
| tail | dick, Kuppe y 185, rosa-rot-weiss gestreiftes Ende | Lage gut. Ende blendet weiss, 2589 px gegen 911, breiter als im Video | Lage gut, Kuppe 4-6 px zu hoch. Ende nur weisse Wolke, 414 px, ohne Form | **am besten.** Rosa Keil liest sich als Ende, Streifen fehlen |
| left_leg | Lappen, Querstreifen, dunkler Keil zum Schwanz | glatter Stiefel, Masse am naechsten, Keil zu 86 % gedeckt. Hand steht in L1, L2 (192 + 104 px) | **Umriss mit Stufen am naechsten an den Lappen**, Keil 54 % | Lage am besten, aber glatte Banane und heller Rhombus (218-255,395-445), Keil nur 19 % |
| right_leg | drei Baender, Luecke zum Schwanz endet y 363 | oben zu schmal, Luecke bis y 404, Bloecke | Luecke bis y 407, nur ein Band liest sich | **Luecke endet y 372, drei Baender am Ort**, aber runde Scheiben, ueberstrahlt |
| right_arm | gerader Zylinder, kleiner Fuss, Spitze (425,363) | **Glockenfuss** x 365-439 bei y 365, Soll 377-422. Deckt als einziger die dunkle Masse O1 | Zylinder gut, Taille, Fuss endet y 368 | **am besten**, Breiten auf 1-6 px am Nutzerumriss |
| part_of_Body | schmaler rosa Schraegfleck, Band Z2 | Z1 breit, weisslich, Z3 runde Scheibe | Z1 viel zu gross, 820 px gegen 290 | Z1 426 px und spitz, Z2 als schmaler Strich in richtiger Richtung |
| face_and_upper_body | fast schwarz, duenner lila Bogen, Glanz P (366,304) | flache Bloecke statt Linien, Stern bei P, **Loch 556 px bei (245,310)**, durch das im Spiel Feuer scheint | **Bogen durchgehend wie im Video**, aber zu klar. Heller Ring ums Knie (220,311) | ruhigste Flaeche, P sitzt klein und richtig. Bogen unterbrochen, zu dunkel, Y 17 gegen 24 |
| left_arm_front | rotbraun, gerade Schneide, duenne Klauen | Knoechelblock rund 27 px ueber der Schneide im Feuer | wie A, 16 % des rechten Feuers bedeckt | **Keil mit Schneide**, 2 % Feuer, aber schwarz |
| right_arm_front | gerippter Streifen, Hand dunkelrote Kuppel | Wurst mit Haken, Hand deckt 26 % des linken Feuers | **Planke an richtiger Stelle**, G Y 103 gegen 113. Hand grob | Streifen nur noch Punkte, G Y 61. Hand am besten geformt, fast schwarz |

## 5. Zahlen mit gleicher Zuordnung

A und C zaehlen in `form.json` das rechte Bein zu `face_and_upper_body`, B nicht. Die Zahlen der Dossiers sind damit nicht vergleichbar. Neu gemessen, `urteil_bild/tabelle.txt`:

| | A | B | C |
|---|---|---|---|
| face IoU / Deckung, ohne Bein | 0,77 / 0,82 | **0,82 / 0,87** | 0,76 / 0,79 |
| face IoU / Deckung, mit Bein | 0,79 / 0,87 | 0,87 / 0,94 | 0,89 / 0,94 |
| left_arm_front IoU | 0,68 | 0,69 | **0,82** |
| right_arm_front IoU | 0,66 | 0,74 | **0,81** |
| mittlere IoU Bild 1 | 0,735 | 0,827 | **0,867** |
| mittlere IoU Bild 2, ohne Bein | 0,704 | 0,751 | **0,797** |

Die 0,89 von C stehen also auf dem Bein: 15 % des Oberkoerper-Umrisses fuellt dort das rechte Bein. Das widerspricht der Lesung "Gesicht und Oberkoerper". Im Bild ist es dunkel auf dunkel und nicht zu sehen. Alle drei tun es, C am meisten.

Grundfarbe Bild 1, Kern gegen Video: Armzylinder Video (30,30,52), A (44,25,48), B (33,26,52), C (29,22,42). Schwanz Video (48,36,59), B (48,35,60). `urteil_bild/farbe_kern.py`.
Rest: `stellen.txt`, `keil.txt`, `marken.txt`. Kleinigkeit: In Rohbau-C-Dossier steht IoU Bild 1 0,975, das ist `Weiss gedeckt`, die IoU ist 0,972. Die Berichte von A, B und C liessen sich neu rechnen und gleichen.

## 6. Das Beste je Rohbau

**A:** Masse rechts neben dem Arm (O1, Abschnitt 7), Nutzer vorher fragen. Fuellung des linken Keils, dort steht kein heller Boden. Rotbrauner Ton der Hand, dem Video am naechsten. Helligkeit von Z6a (135 gegen 134) und Z7 (108 gegen 101). Z5 als Dreieck mit gerader Kante, aber flacher.
Nicht nehmen: Glockenfuss, Stern P, Bloecke am Kopf, Schwanzende.

**B:** Zug Q als duenne durchgehende Linie, dunkler und schmaler. Unterarmstreifen in Laenge und Helligkeit, mit Rippen brechen und rot faerben. Stufen im Umriss der Beine fuer die Lappen. Grundfarbe in Bild 1. Rumpf, der den Oberkoerper-Umriss ohne Bein fuellt.
Nicht nehmen: linke Hand, Ring am Knie, Luecke bis y 407, Schwanzende.

**C:** Grundstock: Pose, Umriss, alle Klauen. Dazu Zylinder des rechten Arms, Luecke Schwanz und Bein, Keil der linken Hand, rosa Keil am Schwanz, Glanz P, Rueckprojektion der Marken (`rc_textur.py`), die Bandform Z2, Z6, Z7 aus dem Bild holt.
Nicht nehmen: leeren Keil links, Oberkoerper aus dem Bein.

Empfehlung: C als Grundstock, B und A nach dieser Liste einarbeiten, dann Lappen und Malen.

## 7. Offen

- **O1 rechts neben dem Arm (405-429,262-300), wichtigste Frage fuer Bild 1.** Stark vergroessert (`o1_blick.png`, `o1_voll.png`) liegt dort dunkle, violette Masse in der Farbe der Figur, nicht die orange Wand. Sie endet hart bei (429,272), der helle Strich Z9 liegt auf ihr. Der Nutzerumriss endet davor. Gedeckt: A 0,67, B 0,19, C 0,23. Bei B und C steht dort im Einsatz voll helle Wand. Das Bild spricht fuer Figur, die Lesung sagt nichts dazu. Nutzer fragen, was dort liegt, und das Bild zeigen.
- Linker Keil Bild 1: Er traegt Z7, ist also wohl Figur, liegt aber in keinem Nutzer-Teil. Was ihn fuellt, zeigt das Bild nicht, Nutzer fragen.
- Rotes Licht: Ob Textur oder Raumlicht, entscheidet nur `form_spiel.sh`. Die Buehne zeigt keinen Feuerschein. Die Marken stehen dort in den Spielbildern aller drei etwa so wie hier, eine eigene Spielabnahme ist das nicht.
