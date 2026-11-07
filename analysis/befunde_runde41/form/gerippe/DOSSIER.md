# Gerippe und Formblatt 2 - Dossier, fortlaufend

Stand 2026-10-31. Nur Arbeitsbaum r41_form. Werkzeug hier in gerippe/, Zwischenstaende in gerippe/lauf/.

- Schritt 1: teile_messen.py schreibt lauf/teile_mass.json (Mittellinie, Breite, Engstellen je Nutzer-Teil). Masken sind je eine Komponente.
- Konflikte Nutzer gegen ziel_bild: gross nur in Bild 2 unten links (Hand, 2069 px auf ziel=0) und Klauen links (679 px). Dort gewinnt der Nutzer, die alte Maske ist ueberholt.
- Rig (rig.py): 30 Knochen, Eltern vor Kindern, Index 10 = brust, 13/18 = Haende passend zum Tick (Teile 10, 13, 18). Winkel wie Port M = Rx Ry Rz, +Z = links der Figur.
- fit.py: Stapel-Optimierung, Ellipsoid-Ketten als weiche Kegelstuempfe, weiche Tiefe fuer Sichtbarkeit, Abschnitte je Teil (duenne Stellen zaehlen wie breite), Vorlauf nur mit Ankern, K1 sicher.
- Lesebefund Bild 2: Beide Arme kommen UNTER der unteren rechten Rumpfkante heraus, Spitze des rechten Arms (337,316) laeuft dort aus. Der Rumpf liegt also vor dem Armansatz. Hand rechts: dunkle Masse (158-275, 385-445), weiter unter dem Logo, Fingerluecken (205-212,425-455), (258-272,430-445).
- Rechter Unterarm Bild 2 ist gerippt hell (X-Muster) zwischen (262,365)-(330,325) und (275,400)-(340,325), rund 45 px breit.
- Serie A laeuft (lauf/b21-23 Hand rechts am Boden, b31-33 Hand rechts frei, b41-43 grosse Streuung).
- Zwischenstand a13 Start 1 (Probe): Modell in beiden Bildern wiedererkennbar, Ganzzahl-Probe ueber form_emd.pose_aus_bild max 12 Einheiten, 0,85 px.
- Lesung Bild 1: Kappe Z5 sitzt bei y 277-296 auf dem Arm, darunter glatter Zylinder bis y 355, dann Hand bis y 375. Unterarm rund 1000 Einheiten, Oberarm mindestens 900 (Tiefe 6900, 59 px je 1000). Z5 ist damit die Ellbogenstelle.
- Lesung Bild 2 linker Arm: Hand laeuft als flacher Keil nach rechts unten, Spitzen (437,418), (468,417), (460,392). Heller Punkt (412,358) am Handgelenk.
- Alte Schulter S (405-430,238-280) liegt ausserhalb des Nutzer-Umrisses von right_arm, Z9 auch: offen, das Modell deckt sie nicht.
- Falle: drei fit.py parallel fuellen den GPU-Speicher (7,7 von 8,2 GB), Tempo faellt ins Bodenlose (300 Schritte in ueber 11 Minuten). Serie A abgebrochen, Serie B laeuft nacheinander (c51-53 Hand am Boden, c61-63 rechte Hand frei, c71-73 grosse Streuung). Einzelner Lauf: rund 0,22 s je Schritt.
- Beide Ansichten laufen fast laengs der Figur: Rumpflaenge ist aus den Umrissen nur ueber Haende/Fuesse am Boden bestimmbar. Profile geplant.
- Serie B fertig (48 Starts): beste Kosten 8,04 (c51 Start 0, Hand am Boden), 8,22 (c62 Start 2, rechte Hand frei), 8,56 (c72 Start 2). Bild 1 stabil: Huefte Tiefe 4760-5450, Neigung 3-18 Grad. Bild 2 offen: Gier -24 bis +65, Tiefe 6040-7315, Kopf Bild x 265-357, y 221-288. Rechte Hand bleibt auch ohne Zwang nah am Boden (Handgelenk Hoehe 280-550).
- Fehlstart-Art: rechter Arm im Bild 1 verdeckt vom Brustkorb, darum Rumpf aus der Armmaske gedrueckt. Tail in Bild 2 und Kopf/linker Arm in Bild 1 muessen verdeckt bleiben (Term verdeckt), nachdem der Schwanz in c61 Start 1 oben aus dem Rumpf ragte.
- Grenzen nachgezogen: schwanz_y bis -1000, Knie/Knoechel duenner (Oberschenkel-Ende bis 170, Unterschenkel bis 100): Engstelle Bein rechts 30 px = 350 Einheiten.
- Feinlauf f1 aus c51 Start 0: Stride 2, IoU je Teil 0,69-0,93, Pruefpunkte Bild 1 auf 1-4 px.
- Vorgabe Verhaeltnisse (Arm/Bein 1,1, Rumpf mindestens 1600) in fit.py, weil Rumpflaenge sonst auf 1200 schrumpft und die Arme wachsen. Die Bilder sehen das nicht: beide blicken laengs.
- Profile aus f3 (lauf/pr_*.pt, profil_lesen.py, Bildanteil der Kosten): Arm unter 2000 deutlich schlechter (1,29 bei 1500, 0,94 bei 2000, 0,72 ab 2500). Bein flach ab 1550. Rumpf 1000-2200 kaum Unterschied (0,65-0,81), ab 2600 schlechter. Schwanz 3000-3500 am besten, 2500 und 4000 je +0,07. Huefte Tiefe Bild 1: 4600-5000 gut, 5800 deutlich schlechter. Huefte Tiefe Bild 2: je naeher desto besser bis 5500, 7000 deutlich schlechter. Gier Bild 2 flach von -20 bis +80. Gier Bild 1 flach von -10 bis +20.
- (Ueberholt durch die Korrektur: Gier Bild 1 liegt nach dem Fehler-Fix bei -20 bis +10, Abgabe -1,4. Die alte Spanne 5,7-17,3 Grad wird nur am Rand beruehrt.)
- Nachgerechnet: Huefte Tiefe Bild 2 4500: 0,93, 5000: 0,77, 5500: 0,75, 6000: 0,77, 6500: 0,87, Mitte also 5500 +-500. Gier Bild 1 auch 20-35 noch flach (0,75-0,79), 50: 0,91. Rechte Hand frei (f6) bringt nichts: 0,82 gegen 0,83.
- Kopf-Hypothese (f5): Kopfmitte erzwungen auf (325,235), Mitte von Zug Q. Bildanteil 1,01 gegen 0,83 (f4, Kopf frei links unten). Ohne Zutun deckt sich der Kopfumriss bei Tiefe 4600 mit dem Bogen: x 267-381, y 187-277 gegen Zug x 262-388, y 189-296. Das ist kein Beweis, aber die einfachste Lesung fuer "von vorn": Kopf mittig, Bogen ist sein Rand. Zweite Lesung: Kopf links unten bei B10/B11.
- Serie G: Mehrfachstarts mit Kopf unter dem Bogen und Gier 21 (frontal zur Kamera an dieser Stelle).
- FEHLER gefunden (Serie A-G betroffen): weiche Sichtbarkeit lief als Softmax ueber log(o+1e-6) - z/gamma. Ein Primitiv, das das Pixel gar nicht deckt, gewinnt damit, sobald es 2100 Einheiten naeher liegt. Der rechte Arm in Bild 1 (Tiefe 6900) galt deshalb oft als unsichtbar (vpos 1,0, Gradient 0), obwohl er im Tiefenbild sichtbar war. Jetzt zaehlen nur deckende Primitive. Alle Profile und Laeufe davor sind neu zu rechnen.
- Nach der Korrektur: Serie k (lauf/k91-94). k91 frei: 7 von 8 Starts bei Kosten 6-8. m1 aus k91 Start 0: Rumpf 1927, Arm 1756, Bein 1578, Schwanz rund 3400, Gier Bild 2 14,6 Grad (frontal waere 21). m4 aus m1 mit Gier 21 und Kopf nach (325,230): Kosten 5,12, Bildanteil 0,81, IoU je Teil 0,67-0,88. Das ist der Stand fuer die Abgabe, sofern nichts Besseres kommt. Pose 1: auf allen vieren, Schwanz gebogen. Pose 2: Hinterteil abgesenkt (Huefte Hoehe 1170), Rumpf aufgerichtet, Haende am Boden.
- m2 (Multistart unter Zwang, k93 Start 6): Kosten 7,8, linkes Bein schlechter (0,55). m4 ist besser und anatomisch ruhiger.
- Abgabe: lauf/ende3.pt (aus m4, Fuesse flach, Knoechel Bild 2 als Vorgabe). Kosten 5,29, Bildanteil 0,87. gerippe.json, gerippe_bild1/2.png, gerippe_rundum.png, FORMBLATT2.md geschrieben.
- Profile nach der Korrektur: lauf/q_*.pt, profile_q.json. Rechte Hand frei nach der Korrektur 0,85 gegen 0,87 (ende_hand.pt), Hand steigt auf Hoehe 650.
- Gegenprobe 2026-11-01: Abgabestand jetzt lauf/ende4.pt (aus ende3 ueber g2-g8). Befunde und Aenderungen in ../gerippe_gegenprobe.md und FORMBLATT2 Abschnitt 10. fit.py hat neue Schalter --anatomie, --kopfrand, --kante, --anker3d, --setze, --setzed, --vorpunkte, --gier2start.
