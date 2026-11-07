## 10. Aenderungsliste der Gegenprobe

Belege und Zahlen im Dossier `gerippe_gegenprobe.md`. Vorher-Stand in `gegenprobe/vorher/`.

| Nr | Fund | Beleg | Aenderung |
|---|---|---|---|
| 1 | Rechter Arm Bild 2 lief nicht auf dem Nutzer-Teil: Oberarm lag vor der Brust, Handgelenk 57 px neben (262,395), Hand zeigte von der Kamera weg. Metrik verdeckte das: Oberarm-Pixel im Oberkoerper-Umriss wurden dem Rumpf gutgeschrieben. IoU wirklich 0,51, nicht 0,71 | `gegenprobe/render.py`, `richtung.py`, Abschnitt 1 dieses Blatts | Oberarm zaehlt in Bild 2 zu keinem Teil, bleibt verdeckt (114 px statt 6211). Ellbogen an die Spitze, Unterarm auf die Achse des Streifens, Hand am Boden, Finger zum Logo. `rig.py`, `fit.py`, `ansicht.py` |
| 2 | Kopf Bild 2 lag 59 Grad auf der Seite, a 750 an der Klassengrenze, Ende dicker als Mitte (Trichter). Nur so fuellte er die Kuppel | `gegenprobe/kopfachsen.py`, Seitenblick `gerippe_rundblick.png` | Kopf aufrecht (Roll 3 Grad), Rand auf Zug Q [v], Kuppel kommt von Brust und linker Schulter. Grenze a 680, Ende nicht dicker als Mitte |
| 3 | Gier Bild 2 = 21 war Vorgabe und erzwang krummen Rumpf (Huefte +21, Brust -17). Handlinie ergibt -14,4 | Rechnung Dossier, Profil Abschnitt 4 | Gier frei, Ergebnis -15,7. Rumpf gerade. Kopf dreht zur Kamera |
| 4 | Bein im Rumpf Pose 2: rechter Oberschenkel 45 %, Unterschenkel 24 %, linker Oberschenkel 17 % | `gegenprobe/ae1.txt` | Kosten Glieder ausserhalb des Rumpfes. Jetzt 5 %, 0 %, 3,5 %. Hocke, Knie seitlich |
| 5 | Ellbogen Pose 1 auf +12 Grad Ueberstreckung, beide | `gegenprobe/ae1.txt` | Grenze Ellbogen und Knie auf hoechstens 2 Grad. Oberarm 601 auf 800 (Vorgabe mindestens 0,72 Unterarm), Rumpf Pose 1 nicht mehr 24 Grad verdreht |
| 6 | Finger schwebten: linke Klauen Pose 2 bis 350 ueber dem Boden, Spitzen 20 px neben dem Umriss. Finger Pose 1 standen bis Hoehe 760 | `gegenprobe/ae1.txt`, Maske | Linke Spitzen im Bild auf (461,394), (465,413), (438,415), zwei am Boden (Hoehe 76, 102), die aeussere bei 348. Rechte Hand: Handballen am Boden, Knoechel hoch, Spitzen Hoehe 173-184. Pose 1 Spitzen 15-55 am Boden |
| 7 | Kiefer steckte im Hals (52-60 % im Rumpf), Spitze 670 hinter der Schnauze | `gegenprobe/ae1.txt` | Kiefer bei x 500 unter dem Kopf, reicht bis 92 % Kopflaenge. [f] |
| 8 | Oberarm wurde zur Schulter duenner (274 auf 416): Rest der vom Rumpf verdeckten Maske | Abschnitt 2 alte Fassung | Oberarm 331/300/280, Unterarm 305/265/260. Bild 1 deckt der Rumpf die Innenseite |
| 9 | Kante R und Verdeckungen waren nur Pruefpunkte: belegte Kante 0-5 px, gelesene 4-26 px, V2 0,88 | `pruefpunkte.py` | Als Kosten: belegte Kante 0-1,4 px. Gelesene Kante nur Mindestausdehnung, siehe Abschnitt 7. V2 0,95 |
| 10 | Hals, Fusslaenge: laufen wie vorher an Grenzen | Abschnitt 8 | Nicht geaendert, vermerkt |

Nicht geaendert: Links und rechts sind nicht vertauscht. Beide Posen sind dieselbe Figur. Lage von Schwanz, beiden Beinen und rechtem Arm in Bild 1, Teile-Lesung, Kameras, Knochenbaum, Knochennummern.
