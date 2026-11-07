# Urteil Linse MESSUNG: Rohbau A, B, C

Stand 2026-11-01, Arbeitsbaum r41_form, kein Commit. Unabhaengiger Pruefer, an keinem Rohbau beteiligt. Kein RE-Wert: Vertraeglichkeit mit zwei Videostandbildern.
Rechnungen und Rohausgaben: `urteil_mass/` (Skripte `m1_skelett.py` bis `m10_kanten.py`, Ausgaben `m2.txt` bis `m10.txt`).

## 1. Urteil

| Rohbau | Punkte | Kurz |
|---|---|---|
| A | 4 von 10 | Sauberste Anatomie, aber schlechteste Passung. Steht in Bild 1 um 630 zu nah, Beine dadurch 12 % zu lang, linke Hand sichtbar in L1/L2. Keine versteckten Tricks, Dossier ehrlich |
| B | 5 von 10 | Einzige spiegelgleiche Figur, bester Oberkoerper. Aber der linke Arm steckt in Pose 1 im Rumpf, die Figur steht von der Seite auf drei Gliedern. Knoechel 120 Grad verdreht. Die Bild-1-Zahlen haengen daran |
| C | 6,5 von 10 | Beste Passung auch ohne Maske, alle vier Glieder frei am Boden, kaum Durchdringung. Aber: Vorsprung vor B kommt teils aus ungleichen Gliedern je Seite, Bein zaehlt als Oberkoerper, Kralle als Horn |

1:1 ist keiner. Empfehlung: **C als Grundlage**, Abschnitt 9.

## 2. Nachpruefung der Abgaben

`form_pruefung.py pruefe` je Rohbau selbst gefahren, Ausgabe `urteil_mass/a`, `b`, `c`.
- `bericht.txt` byte-gleich mit der Abgabe, bei allen dreien. `FORM.EMD`, `FORM.TIM` je Rohbau einmal gelesen, md5 bei C wie im Dossier (150d33ab, b8aad8d0).
- A und B: Blaetter byte-gleich. C: `vergleich_bild*.png` sind eigene Nebeneinander-Bilder aus `rc_bau.py`, kein Widerspruch.
- Alle Tabellenwerte der Dossiers stimmen mit Ausnahme von Abschnitt 7.

## 3. Gleiche Messlatte

A und C zaehlen in `form.json` rechtes Bein und rechten Oberarm (Knochen 9, 26, 27, 16) zu `face_and_upper_body`, B nicht. Das ist genau die Falle aus `gerippe_gegenprobe.md`: Oberarm-Pixel wurden dem Rumpf gutgeschrieben. Ich habe alle drei mit der Zuordnung aus `gerippe.json` gemessen (`urteil_mass/streng_*`).

| Messung | A | B | C | Soll / Bemerkung |
|---|---|---|---|---|
| Umriss-IoU Bild 1 / 2 | 0,914 / 0,917 | 0,962 / 0,951 | 0,972 / 0,963 | |
| Schwarz getroffen px | 917 / 53 | 245 / 93 | 112 / 87 | 0 |
| Kantenpunkte | 20/31, 5/7 | 22/31, 7/7 | 25/31, 7/7 | |
| Boden, Verdeckung | 0/3, 6/9 | 1/3, 8/9 | 1/3, 9/9 | |
| Zeichnung | 8/9, 7/7 | 8/9, 6/7 | 8/9, 7/7 | in die Pruefpolygone gebacken, siehe 4.4 |
| Mittlere Teile-IoU, gleiche Zuordnung | 0,735 / 0,686 | 0,827 / 0,742 | 0,867 / 0,790 | |
| `face_and_upper_body` IoU / Deckung streng | 0,72 / 0,76 | **0,79 / 0,84** | 0,74 / 0,77 | A, C im Dossier 0,79 und 0,89 mit Bein |
| Rechtes Bein in dieser Maske | 0,049 | 0,072 | **0,146** | kein Nutzer-Teil |
| Modell ausserhalb aller Teile + 3 px | 3151 / 3939 | 1146 / 2956 | 797 / 1978 | |
| Rahmen | 1917 Flaechen, Werkzeug meldet Ueberschreitung | ok | ok | |

| Teil | A IoU/Deck/Innen | B | C |
|---|---|---|---|
| tail | 0,86 0,89 0,97 | 0,89 0,98 0,91 | 0,91 0,97 0,93 |
| left_leg | 0,60 0,89 0,65 | 0,78 0,95 0,82 | 0,90 0,94 0,95 |
| right_leg | 0,82 0,92 0,89 | 0,90 0,96 0,93 | 0,86 0,97 0,88 |
| right_arm | 0,66 0,91 0,71 | 0,78 0,89 0,87 | 0,84 0,91 0,91 |
| part_of_Body | 0,73 0,88 0,81 | 0,78 0,93 0,83 | 0,83 0,96 0,87 |
| face_and_upper_body | 0,72 0,76 0,93 | 0,79 0,84 0,94 | 0,74 0,77 0,95 |
| left_arm_front | 0,68 0,85 0,78 | 0,69 0,88 0,76 | 0,82 0,88 0,92 |
| right_arm_front | 0,66 0,83 0,76 | 0,74 0,86 0,85 | 0,81 0,91 0,89 |

**Maskenfreie Probe** (`m10_kanten.py`): Abstand Render-Umriss zur naechsten Canny-Kante im Original, ohne Nutzer-Maske. Mittel / Median in px. Skala: Nutzer-Soll, und Soll 7 px verschoben.

| | Soll | Soll +7 px | A | B | C |
|---|---|---|---|---|---|
| Bild 1 | 3,48 / 2,0 | 4,97 / 4,0 | 3,72 / 2,8 | 3,74 / 2,2 | **3,35 / 2,0** |
| Bild 2 | 5,65 / 2,2 | 6,91 / 5,0 | 10,57 / 7,2 | 8,52 / 5,4 | **6,94 / 4,2** |

Der Vorsprung von C ist also nicht nur auswendig gelernte Maske: Sein Umriss liegt auch auf echten Bildkanten. Grenze: Bild 2 ist dunkel auf dunkel, die Masse ist grob.

## 4. Suche nach Tricks

### 4.1 Andere Laengen je Pose: nein, bei allen dreien

Gelenkabstaende aller Knochen ueber Clip 0, 1, 2: Unterschied 0,000. Das EMD hat eine Knochentabelle, Meshes sind starr. Gegen `gerippe.json` geaendert sind nur die Fingerwurzeln.
Der eine Maszstab-Hebel ist die Lage:

| | A | B | C |
|---|---|---|---|
| Tiefe Huefte Bild 1 | **5212** | 5842 | 5842 |
| Tiefe Huefte Bild 2 | 7159 | 7125 | 7194 |
| left_leg Laenge Ist/Soll | **196/172** | 168/172 | 171/172 |
| right_leg Laenge Ist/Soll | **190/171** | 172/171 | 168/171 |

A steht in Bild 1 rund 630 naeher an der Kamera, Bild 2 unveraendert. Die Figur erscheint in Bild 1 also 12 % groesser als in Bild 2, gegenueber dem Stand von B und C. FORMBLATT2: Massstab Bild 1 auf rund 4 %, Huefttiefe ab 5600. Im Dossier A als Punkt 6 genannt, Wirkung nicht. Das ist kein Betrug, sondern der Hauptgrund fuer die schwachen Bein-Zahlen von A.

### 4.2 Teile nur fuer eine Kamera hingestellt

**Hauptfund, B:** Linker Arm in Pose 1 steckt im Rumpf.

| Pose 1 | A | B | C |
|---|---|---|---|
| Oberflaeche linker Oberarm im Rumpf | 0,30 | 0,44 | 0,04 |
| linker Unterarm im Rumpf | 0,07 | **0,97** | 0,00 |
| linke Hand im Rumpf | 0,00 | **0,85** | 0,00 |
| linke Hand ueber Boden | -7 | **799** | 3 |
| linker Arm + Hand sichtbar Bild 1 px | 765 | 0 | 19 |

Beleg: `rohbau_b/ik4.py` Zeile 24 setzt die Hand auf Bildpunkt (322,296), Hoehe 1250, also in die Rumpf-Silhouette. Bild: `urteil_mass/lupe_b_p1_links.png`, `rundum_b_pose1_hell.png`: Von links nur ein Schulterstumpf, von vorn ein Arm. Die Figur steht auf drei Gliedern. Auch im Spiel zu sehen: `rohbau_b/spiel_pose1_r5080_s1.png`, der Vorderkoerper haengt ohne linken Arm in der Luft, gegen `rohbau_a/spiel_pose1_room5080_schnitt1.png` und `rohbau_c/spiel_pose1_r5080_s1.png`. Das widerspricht "4-beinig" und den Anatomie-Kosten aus FORMBLATT2 ("Glieder ausserhalb des Rumpfes"). Dossier B sagt "hinter Rumpf und rechtem Bein".

**C loest dieselbe Aufgabe sauber:** Linke Hand steht am Boden, Handgelenk (325,355), hinter dem rechten Bein, keine Durchdringung. `rundum_c_pose1_hell.png` zeigt vier Glieder.
A verwarf den Trick ebenfalls (`p18`), dafuer steht seine Hand sichtbar im Spalt.

Weitere Stellen:

| Stelle | A | B | C |
|---|---|---|---|
| Lange Kralle fuer Sporn (145,353): Hoehe der Spitze | 829, 99 Grad seitlich | 682 | **1357**, Horn auf der Hand, `lupe_c_p2_schulter.png` |
| Rechte Fingerspitzen Pose 2, Hoehe | 586, 501, 588 | **124, 112, -1** | 284, 316, 301 |
| Finger links eingeschlagen | nein | **klaue_l4 152 Grad, nur links, in beiden Posen** | nein |
| Schwanzende Hoehe x Breite | **367 x 819, Paddel** | 381 x 642 | 432 x 419, rund |
| Fuss rechts ueber Boden Pose 2 | -2 | 54 | **263** |
| Spitze an Stelle P | Hauer auf den Sehstrahl gelegt | Wangenspitze | Hauer auf den Sehstrahl gelegt |

Keine Scheiben: Sichtbreite gegen Tiefe je Glied 0,71 bis 1,40, bei allen. Der Verdacht "Platten zur Kamera" ist fuer die Glieder widerlegt, nur das Schwanzende von A und B ist flach.
Rechte Fingerspitzen unter dem Logo: A und C ziehen sie per Punktziel auf den Handschnitt unter der Schrift. B laesst sie am Boden und verfehlt dort 16-27 px. B hat hier die bessere Form, das Bild zeigt unter dem Logo nichts.

### 4.3 Unmoegliche Gelenke

Drehung um die Gliedachse, Grad. Grenzen aus `gerippe.json`: Ellbogen und Knie +-20, Knoechel +-40, Hand +-70.

| Pose 2 | A | B | C |
|---|---|---|---|
| Ellbogen links / rechts | -3 / **+37** | **-50** / -4 | -3 / +4 |
| Knie links / rechts | -12 / +6 | +16 / -12 | -13 / **+28** |
| Knoechel links / rechts | -39 / +36 | **+76 / -120** | -38 / +35 |
| Handgelenk links / rechts | +32 / -59 | **+72** / -31 | +13 / -49 |
| Kniebeuge links / rechts | 74 / 95 | 104 / **138** | 74 / 76 |
| Abstand zur Gerippe-Pose, groesster | hand_r 44, unterarm_r 36 | **fuss_r 164, fuss_l 124**, oberschenkel_l 84 | hand_r 77, sonst bis 14 |

Pose 1: Knoechel rechts A -49, B -39, C -38. Ellbogen rechts leicht ueberstreckt, A 3,5, B 4,1, C 6,2 Grad, Grenze 2.
C: Rechte Finger Pose 2 nach hinten ueberstreckt, Z -31 bis -79 Grad, Grenze -30. Deshalb die Horn-Haltung.
Rechter Oberarm Pose 2: alle am Drehlimit (+95 bis +98), Erbe des Gerippes. Er steckt im Kopf, A 12 %, B 24 %, C 27 %. Bei C hat er dazu 64 Einheiten Luft zur Brust, er haengt also am Kopf, nicht an der Schulter (A 6, B 3). Kein Bild zeigt das, im Rundblick sichtbar.

Durchdringung Gruppen, groesster Wert je Pose (`m5.txt`): A 0,09, B **0,77**, C 0,07.

### 4.4 Masken ausgenutzt

**Bein als Oberkoerper.** Abschnitt 3. Bringt A +0,07 und C +0,15 IoU. Beide nennen es, C rechnet es in `rc_fit.py` (`MIT`) sogar als Deckung. Es stammt aus der Vorgabe des Gerippes, ist aber gegen die Lesung: Die Maske heisst Gesicht und Oberkoerper.

**Glieder je Seite auf die eine Maske gezogen.** Volumen Links/Rechts:

| | A | B | C |
|---|---|---|---|
| Hand | 0,65 | 1,00 | **0,52** |
| Unterschenkel | **1,52** | 1,00 | 0,65 |
| Fuss | 1,45 | 1,00 | 0,61 |
| Oberarm | 1,16 | 1,00 | 1,44 |
| Unterarm | 0,93 | 1,00 | 1,47 |
| Oberschenkel | 0,93 | 1,00 | 0,93 |

Kein Bild verbietet das, aber auch keines belegt es. FORMBLATT2 fuehrt "links gleich rechts". Die Handschnitte tragen eine Hand von doppeltem Volumen nicht: Laut Auftrag legen sie fest, was wo ist, nicht jede Kante. Preis siehe Abschnitt 5.

**Zeichnung.** Alle drei legen die Marken in die Polygone aus `form_pruefung.stellen()`. A und C sagen das, B nicht. Der Test verlangt nur 0,6 Deckung, also hilft Uebergroesse. Groesse Ist/Soll: A Z8 2,38, Q3 2,25, Z4 1,96; C Z4 2,30, Z8 2,10, Z6a 1,92; B Z1 2,08, sonst 0,81-1,45. Die Bestanden-Spalte Zeichnung sagt deshalb wenig.

**Verdeckt sollte bleiben** (FORMBLATT2: Oberarm rechts 114 px, Schwanz 117 px):

| Bild 2 sichtbar px | A | B | C |
|---|---|---|---|
| Oberarm rechts | 1730 | 909 | 621 |
| Schwanz | 502 | 14 | **1040**, fuellt die Stelle Haken |
| Rechtes Bein | 2698 | 2796 | **4733** |

**Gegenrichtung: dunkle Stellen ausserhalb aller Teile.** O3 Keil (218-255,345-445) gedeckt: A 0,87, B 0,54, C 0,17. O1 neben dem Arm: A 0,57, B 0, C 0,04. A hat hier die Bilddaten ernster genommen, bezahlt mit IoU. `b1.rand_rechts`: A ok, B -26,5, C -23,5. O1 ist weiter Nutzerfrage.

### 4.5 Links/Rechts

Nicht vertauscht, bei allen. Vorzeichen Z stimmen, keine Spiegelung (det +1). Bild 1 von hinten: Schulter und Huefte L links von R. Bild 2 von vorn: L rechts von R. Kopf zum Auge Bild 2: A 3, B 14, C 18 Grad. Bild 1: alle von der Kamera weg.

Einzelfehler A: Fingerlaengen haengen am Index, nicht an der Seite. `klaue_r3` bei z -300 aussen 916 lang, `klaue_l3` bei z -300 innen 916 lang. Die lange Kralle sitzt rechts aussen, links innen. Dossier A nennt "Aussenkralle 25 % laenger", links stimmt das nicht. B und C sind korrekt gespiegelt.

## 5. Gegenproben: Was bleibt ohne den Trick

Keine Neupassung, also Untergrenze. Skript `m8_proben.py`, Ausgabe `m8.txt`.

| Stand | Umriss B1/B2 | Schwarz | L1 / L2 / SCH | Punkte | Flaechen | Teile-IoU |
|---|---|---|---|---|---|---|
| B Abgabe | 0,962 / 0,951 | 245 / 93 | 32 / 39 / 0 | 22/31, 7/7 | 1/3, 8/9 | 0,827 / 0,742 |
| B, Arm frei, Winkel aus C | 0,952 / 0,951 | 635 / 93 | **131 / 324** / 9 | 19/31, 7/7 | 0/3, 8/9 | gleich |
| B, Arm frei, Winkel Gerippe | 0,955 / 0,951 | 530 / 93 | 164 / 192 / 0 | 19/31, 7/7 | 1/3, 8/9 | gleich |
| C Abgabe | 0,972 / 0,963 | 112 / 87 | 29 / 25 / 0 | 25/31, 7/7 | 1/3, 9/9 | 0,867 / 0,790 |
| C spiegelgleich gemittelt | 0,946 / 0,909 | 437 / 204 | 29 / 143 / 0 | 17/31, 7/7 | 1/3, 8/9 | **0,816 / 0,675** |
| C, linke Hand = Spiegel der rechten | 0,969 / 0,957 | 204 / 551 | 67 / 79 / 0 | 23/31, 7/7 | 1/3, 8/9 | 0,867 / 0,741, left_arm_front 0,67 |
| A Abgabe | 0,914 / 0,917 | 917 / 53 | 192 / 104 / 62 | 20/31, 5/7 | 0/3, 6/9 | 0,735 / 0,686 |
| A spiegelgleich gemittelt | 0,914 / 0,907 | 916 / 43 | 182 / 119 / 41 | 19/31, 5/7 | 0/3, 4/9 | 0,730 / 0,665 |

Lesart:
- B: Mit freiem Arm ist die Luecke L2 nicht mehr frei, Blatt `probe_b_arm_c/lauf/ausschnitt_bild1.png`. Die grosse linke Hand passt nicht hinter das Bein. Die Bild-1-Zahlen von B sind also Folge des Tricks.
- C: Mit gleichen Gliedern faellt C in Bild 2 unter B. Der Vorsprung von C vor B dort kommt aus den ungleichen Seiten. Faire Reihenfolge der Silhouette: C und B liegen enger beieinander als die Abgaben zeigen.
- A: Ungleiche Seiten bringen ihm fast nichts.

## 6. Am Bild

**A.** Bild 1: Beine zu lang und unten zu schlank, Differenz-Tafel hell links unten. Linke Hand im Spalt. Rechte Hand als Glocke, Kralle 20 px ueber die Spitze. Rosa Ende zu gross, Z3 runde weisse Scheibe. Bild 2: Kuppel gut, Schulter rechts endet bei x=399 statt 413, Lichtbalken V1 nicht ganz verdeckt. Rundum: glaubhaftes Tier, klar gegliedert, Ruhepose am besten. Schwanz sitzt als abgeschnittene Wurst auf der Huefte, Krallen sind breite helle Scheiben.

**B.** Bild 1: Massige Wirkung dem Video am naechsten, aber Plattenlinien, die das Video nicht zeigt. Rosa Stelle zu weiss. Bild 2: Bogen Zug Q liest sich als Bogen, am naechsten am Original. Ring am rechten Knie (219,309) ist im Original nicht da. Stelle P fehlt fast ganz. Rundum: Panzer, kantig. In Pose 1 von links fehlt der Arm.

**C.** Bild 1: Umriss am engsten, Raute L2 stimmt, Schwanz und Arm liegen auf den Kanten. Oberflaeche glatte Roehren, im Video Lappen. Marken viel zu hell, Z5 196 gegen 75, Z4 224 gegen 141, Z2 zu dunkel 61 gegen 130. Keil links offen. Bild 2: Lage, Dunkelheit und Haende treffen, Zeichnung zu schwach, Zug Q 30 gegen 58, G 61 gegen 113. Loch bei P. Rundum: Zacke auf dem Ruecken in Pose 2, Horn an der rechten Hand, Handflaechen kastig. Ruhepose ist nur Pose 1 mit Kopf hoch, verdreht, keine neutrale Haltung.

Spielbilder: Alle drei laden, Bahnsteig-Form 32 Meshes, 32 Knochen, 3 Clips. Sie belegen Laden und Stand, nicht Aehnlichkeit.

## 7. Dossiers gegen Messung

| Rohbau | Stelle | Befund |
|---|---|---|
| A | Punkt 6 Lage | Abweichung genannt, Folge 12 % und zu lange Beine nicht |
| A | Hand | "Aussenkralle laenger" gilt links nicht |
| A | Ungleiche Seiten | Oberarm genannt, Unterschenkel 1,52 und Hand 0,65 nicht |
| B | Abschnitt 3 | "hinter Rumpf": tatsaechlich 97 % und 85 % im Rumpf, Hand 799 ueber Boden. **Wichtigste Luecke** |
| B | Knoechel | 120 und 76 Grad Drehung, Fuss 164 Grad von der Gerippe-Pose: nicht genannt, nur "Beine neue Hocke" |
| B | Zeichnung | Polygone aus dem Test nicht genannt |
| B | Normalen | `NORM_U0`, `NORM_G` gestaucht fuer das Licht von Bild 1: genannt, bleibt ein Eingriff fuer eine Kamera |
| C | Abschnitt 5 | L1 32 px, Bericht 29 |
| C | Abschnitt 4 | Unterarm und Haende genannt, Schienbein 0,65, Fuss 0,61, Oberarm 1,44 nicht. Tabelle der Klauenspitzen stimmt auf 1 px |
| C | Schulter | 64 Einheiten Luft, Oberarm 27 % im Kopf: nicht genannt |

## 8. Verworfene Verdachte

- Offene Mesh-Raender bei B (174 Kanten) und C (91): nur Kegelfuesse von Zaehnen, Zehen, Schuppen auf der Haut. Harmlos. A hat 0.
- Durchsichtige Texel: bei keinem benutzt.
- Euler-Alarme im Ruhepose-Blatt von B: zum Teil Doppeldeutigkeit. Echt bleibt Ellbogen links 89, rechts 37 Grad gedreht.
- Risse Eltern-Kind: ausser der Schulter von C alle Teile verbunden, Abstand bis 7.

Grenzen: Innen-Test mit 1500 Proben je Glied, rund 1-2 % Streuung. Gegenproben ohne Neupassung. Kantenprobe haengt am Canny-Pegel, nur der Vergleich zaehlt. Bild-1-Kamera ist geschaetzt.

## 9. Das Beste je Rohbau

Grundlage C, dazu:

| Aus | Uebernehmen |
|---|---|
| **C** | Passung Radien und Ringmitten, keine freien Ecken. Linker Arm Pose 1 frei am Boden hinter dem rechten Bein. Pose 2 nah am Gerippe, Ellbogen ohne Verdrehung. Geringste Durchdringung. Rippen von G aus der Bildhelligkeit |
| **B** | Spiegelgleiche Grundform: erst symmetrisch passen, Abweichung nur mit Bildbeleg. Oberkoerper ohne Bein, strenge Zuordnung als Mass. Rechte Hand flach am Boden, Spitzen unter dem Logo nicht erzwingen. Zug Q als durchgehender Bogen. Ehrliche Markengroessen |
| **A** | Neutrale, symmetrische Ruhepose fuer Clip 2. Keil O3 und dunkle Stellen neben den Masken ernst nehmen. Schulterkopf fuer Kante (429,272), falls der Nutzer O1 bestaetigt. Kopf genau zur Kamera, 3 Grad |

Auflagen fuer den Endstand:
1. `face_and_upper_body` nur Knochen 0, 7, 10, 11, 21, 22, 23. Ziel streng messen.
2. Innen-Test als Abnahme: kein Glied ueber 0,10 im Rumpf, je Pose.
3. Vier Glieder am Boden je Pose, tiefster Punkt je Hand und Fuss, nicht nur je Figur.
4. Fuss rechts Pose 2 auf den Boden, Schulter rechts an die Brust, Horn kuerzen oder den Sporn als offene Stelle fuehren.
5. Links/Rechts-Volumen je Paar im Bericht ausgeben. Abweichung ueber rund 15 % nur mit Beleg.

**Nutzer fragen:** O1, was rechts neben dem Arm liegt. Und: Soll die Figur spiegelgleich sein, oder darf die rechte Hand doppelt so gross wie die linke sein? Das entscheidet 0,05 bis 0,12 mittlere Teile-IoU in Bild 2 und laesst sich aus den Bildern nicht ableiten.

## 10. Dateien

| Datei | Inhalt |
|---|---|
| `urteil_mass/m1_skelett.py` ... `m10_kanten.py` | Pruefskripte, eigene Vorwaertsrechnung |
| `urteil_mass/m2.txt`, `m3.txt`, `m5.txt`, `m6.txt`, `m7.txt`, `m8.txt`, `m9.txt`, `m10.txt` | Rohausgaben: Gelenke, Sichtbarkeit und Innen, Paare und Querschnitte, Raender, Schleifen, Gegenproben, Posenabstand, Kanten |
| `urteil_mass/a`, `b`, `c` | eigene Laeufe `form_pruefung.py pruefe` |
| `urteil_mass/streng_a`, `_b`, `_c` | gleiche Zuordnung, `teile_bild*.png`, `teile.json` |
| `urteil_mass/rundum_*_pose1_hell.png`, `rundum_*_pose2_hell.png` | Rundum je Pose |
| `urteil_mass/lupe_*.png` | Nahblicke |
| `urteil_mass/probe_*` | Gegenproben |
