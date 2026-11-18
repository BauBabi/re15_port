# SF Sichel-Form V8 Runde 5 - Zahlen-Pruefung (eigener Code)

Stand: 2026-11-10. Worktree C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8, Zweig r41/sichel_v8, HEAD 5820af00f.
Geprueft: re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD, sha256 c4976c6214c74fb1ac341fc0ed4ec0768e15c34b0b1939c53b68e04aabd9526a, 271716 Byte
(= Pin release/em30_hangar.sha256).

**Ergebnis: NICHT bestanden. Zwei rote Punkte:**

1. **P5, Bild F311: ROT - Einzelbild, Entscheidung beim Nutzer** (eigene Messung 54 px / 31 tief / 5,8 px; Soll 21 / 7 / 3,2;
   Kachel C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5/F311_kachel.png, mittlere Zeile).
2. **P4, graue Ring-Deckel auf der fleischigen Unterseite: ROT** (gewoehnlicher roter Punkt). Der Mantel der Unterseite ist fleischig und die
   Laengsgrenze zum Grau ist ruhig, aber die grauen Deckel-Dreiecke an den Gliedenden (Zeile 487) blieben grau und stehen als harte graue
   Querbaender in der roten Unterseite: in 69 von 113 Szenenbildern mit Schwanz, bis 837 Port-px je Bild; Kantensprung da* 14,98 gegen
   7,81 an den Haut/Fleisch-Kanten des Original-Birkin, 37,5 Prozent der Kantenlaenge mit da* >= 20 (Original 5,4 Prozent).

Alles andere ist gruen oder gelb (Liste unten). Von den Angaben des Bauers hat sich bestaetigt: Asset/Pin, Tests 8/8 und 1013/1013, state.log
bytegleich V7, F311 als einzige Ansicht ueber dem P5-Soll, Einsatz in keinem Bild ueber dem Gesicht, Huegel rot 45,8 Prozent, P2 erfuellt, Gegenproben rot.
Nicht bestaetigt ist "P4 erfuellt" (Ring-Deckel, vom Riegel des Bauers nicht gemessen).

## Verfahren (eigener Code, nichts vom Bauer uebernommen)

Alle Skripte und Ausgaben liegen in
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr5/ (im Folgenden "pr5/").
Vom Bauer habe ich nur Definitionen gelesen (luecke_r5.py, vorlage_luecke_r5.py, unterseite_r5.py, riegel_v8.py), keinen Code ausgefuehrt und keine Zahl uebernommen.

- Eigener EMD-Leser und Rasterer pl.py (Videoraum 640 x 480 fuer den Vergleich mit den Vorlagen) und port.py (Port-Raum 960 x 720 fuer echte Framedumps).
  Achtung: der Videoraum ist gegen den Port-Schirm senkrecht um 2,069 / 2 gestreckt. Zahlen aus echten Framedumps sind deshalb im Port-Raum gemessen und
  fuer das P5-Soll mit dem Flaechenfaktor 0,4602 (Tiefe / 1,5) in Video-px umgerechnet.
- Eigene Spiellaeufe mit eigener exe-Kopie (Muster lauf_v4.sh, BAUM = dieser Worktree, Framedumps, kein AUTOSHOT, kein SOFTWARE_RENDER):
  pr5/szene (F200-F1100, 901 Bilder) und Marker-Laeufe mit eigenen Marker-Assets (freie CLUT-Zeilen 483/485/488/489 als Farbmarken):
  KO_A_V8 / KO_B_V8 (Kopf, Gesicht, Mesh 8, Einsatz; B = Einsatz entartet), SW_V8 / SC_V8 (Schwanz: Zeile 495 rot, Mantel 487 blau, Deckel 487 gelb),
  MK_A_V8 / MK_B_V8 (Wurzel), GR_V8 (Silhouette). Gegenproben: V7 (sfv4/r/V7_szene, pr3/GR_V7), V8 r4 (pr4/szene, pr4/MK_*, pr4/GR_V8), Stand 91885ed8c (offline).
- Posen: PV = Clip 17 Bild 47, P1 = Clip 20 Bild 139, P2 = Clip 12 Bild 48 (Messblatt), Szenenposen F235-F378 aus den hangar-takt-Zeilen meines Laufs.
- Der Schwanz haengt in der Szene an der Kette; die Clip-Pose des Schwanzes passt deshalb nicht auf echte Bilder. Alle Schwanz-Zahlen aus der Szene sind
  rein markerbasiert (ohne eigenes Raster). Kopf und Rumpf decken sich mit dem eigenen Raster.
- Luecken-Mass P5 (pr5/luecke_eigen.py, pr5/ko.py, pr5/ko2.py): Kerbe = konvexe Huelle (Kopf + Mesh-8-Punkte nahe am Kopf) ohne Silhouette, nur Stuecke, die
  Kopf UND Ruecken beruehren; Spalt = Schliessung; Tiefe = Abstand zum freien Hintergrund. 177 Ansichten (PV, P1, P2, F235-F378,
  Clip-Stichproben) mal 7 Halbpixel-Varianten offline; dazu jeder echte Framedump F235-F378 (144 Bilder).
- Verdeckungs-Korrektur an echten Bildern (pr5/ko2.py): Wo das eigene Port-Raster Figur hat (ohne Schwanz), das echte Bild aber keine zeigt
  (Szenen-Vordergrund), bekommen zusammenhaengende Stuecke ab 40 Port-px die Klasse des eigenen Rasters; danach dasselbe Mass.

## P5 - Luecke Ruecken/Kopf

Soll P5(d): je Ansicht und Halbpixel-Variante Summe <= 21 px UND Punkte mit Tiefe >= 1,5 px <= 7 UND groesste Tiefe <= 3,2 px.

### Vorlage nachgemessen (pr5/vorlage_luecke_eigen.txt)

| Vorlage | dE > 6 | dE > 9 | dE > 12 |
|---|---|---|---|
| n1439 (PV) | bis 15 / 6 / 3,2 | **21 / 7 / 2,8** bei (448,206) | 11 / 3 / 2,8 |
| n1435 (P2) | - | 10 / 3 / 2,2 | - |
| n1433 | 14-15 / 6 / 3,2 | 14-15 / 6 / 3,2 | - |
| n1440 | - | - | 20 / 6 / 3,2 |

Urteil zum Soll: **haelt**. Die drei Grenzen sind die groessten Einzelwerte der Vorlage ueber n1433-n1441 und dE 6/9/12; das Soll liegt damit am oberen Rand
der Vorlage (in der Summe eher locker: nur n1439 bei dE 9 erreicht 21 und n1440 bei dE 12 erreicht 20, sonst hoechstens 15), aber nicht zu streng.
Auf das Ergebnis hat das keinen Einfluss: alle Ansichten von V8 r5 ausser F311 bestehen auch die engere Grenze 14 / 6 / 3,2 (offline) bzw. 16,6 / 3,7 / 3,0 (echte Bilder).

### Modell V8 r5

Offline, 177 Ansichten x 7 Varianten (pr5/luecke_V8.txt; 70 Ansichten mit Luecke > 0, genau 1 ueber dem Soll):

| Ansicht | Summe px | tief >= 1,5 | groesste Tiefe | Bewertung |
|---|---|---|---|---|
| **F311** | **54** (Varianten 44-54) | **31** | **5,8** | **ROT - Einzelbild, Entscheidung beim Nutzer** |
| F310 | 4 | 1 | 2,0 | im Soll |
| F312 | 7 | 2 | 2,2 | im Soll |
| F313 | 4 | 0 | 1,0 | im Soll |
| F314 | 3 | 0 | 1,0 | im Soll |
| alle uebrigen 172 | <= 9 | <= 3 | <= 3,0 | im Soll |

Echte Framedumps F235-F378, 144 Bilder (pr5/ko.txt, pr5/ko2.txt, in Video-px umgerechnet; 52 Bilder mit Luecke > 0):

| Bild | roh | verdeckt (Port-px) | Luecke liegt im eigenen Raster auf der Figur | korrigiert | Befund |
|---|---|---|---|---|---|
| **F311** | 93,0 / 64,0 / 5,7 | 1352 | 47 % | **49,7 / 26,2 / 5,7** | ueber Soll, siehe oben. Stuecke 43,7 px bei Port (576,277) und 6,0 px bei (610,254) |
| F316 | 335,0 / 283,9 / 13,3 | 15677 | 100 % | 0,9 / 0 / 0,7 | Szenen-Vordergrund verdeckt die Figur, keine Luecke |
| F318 | 145,9 / 124,3 / 20,1 | 22425 | 100 % | 0,9 / 0 / 0,7 | wie F316 |
| F322 | 12,9 / 7,8 / 3,4 | 3632 | 89 % | 6,9 / 2,8 / 2,7 | wie F316 |
| F324 | 123,8 / 59,4 / 4,8 | 29621 | 100 % | 0 | wie F316 |
| F326 | 277,5 / 208,5 / 10,5 | 35811 | 100 % | 0 | wie F316 |
| F332 | 76,9 / 40,0 / 5,5 | 4670 | 25 % | 6,4 / 0 / 1,3 | Keil zwischen Rumpf und verdeckendem Vordergrund, keine Luecke am Kopf |
| F308, F309 | - | - | - | 16,1 / 3,7 / 3,0 | im Soll |
| F310, F312, F313, F314 | - | - | - | 3,7 / 0,5 / 1,9; 7,8 / 1,8 / 2,1; 4,1 / 0 / 1,3; 0 | im Soll |
| alle uebrigen | - | - | - | <= 16,6 / <= 3,7 / <= 3,0 | im Soll |

Die Rohwerte F316-F332 sind **keine** Luecken: in diesen Bildern steht die Figur nah an der Kamera hinter Szenen-Vordergrund, die Figur-Maske
aus dem Bildvergleich ist dort abgeschnitten und die Huelle laeuft ueber den Verdecker. Beleg: pr5/ko_blick.png; die Luecken-Punkte liegen zu 89-100 % dort,
wo das eigene Raster Figur hat. Mit Verdeckungs-Korrektur bleibt genau **ein** Bild ueber dem Soll: **F311**. Damit ist die Angabe des Bauers bestaetigt
(er nennt 63 px / 37 tief / 6,3 px; ich messe 54 / 31 / 5,8 offline und 49,7 / 26,2 / 5,7 im echten Bild).

Zu F311 fuer die Entscheidung: der Kopf zeigt zur Kamera, der Rumpf liegt dahinter. Das grosse Stueck (43,7 px) liegt **links neben dem Gesicht zwischen Kinn
und Unterarm/Brust**, nicht im Nacken; das kleine (6,0 px) zwischen Kopf und Rueckendorn. Die Nachbarbilder F308-F314 liegen im Soll.

### Kopf, Einsatz, Gegenproben

| Punkt | Soll | Ist (eigen) | Bewertung |
|---|---|---|---|
| Kopf Mesh 9 | bytegleich V7 | 4280 Byte gleich, Zeile 486 gleich, 6379 Texel gleich; Mesh 9 Geometrie und UV gleich V7 und V8 r4 | GRUEN |
| Einsatz | - | 18 Flaechen (12 Dreiecke, 6 Vierecke, 14 neue Ecken) an Mesh 8, alle Zeile 494, 180 Texel aus der Wirtsflaeche d65, L* 32,0-61,2, a* 0,3-9,9 | - |
| Einsatz ueber dem Gesicht (Maler-Reihenfolge) | 0 | Echte Bilder: Einsatz in 133 von 144 Bildern sichtbar (Summe 16711 Port-px, groesstes 210 in F376). Rohtreffer ueber Kopf 29 px in F331 und 1 px in F332 sind Hintergrund: Farbe im Lauf ohne Einsatz (13,15,15) = Szenen-Hintergrund, eigenes Raster dort keine Figur. Offline (Z-Puffer, F235-F378 und P1): Einsatz vor Knochen 9 **0 px**. Echte Ueberdeckung des Gesichts: **0** | GRUEN |
| kein neuer Huegel (PV) | S3 | Kuppe 3,0 (<= 4); H 8,0 bei x 429 (5-10); Sattel 0,9 (<= 2); K 3,0 (<= 3) | GRUEN |
| Hinweis Hinterkopf | - | Der verschobene Ruecken (29 von 69 Mesh-8-Ecken bewegt, bis 270 / 160 / 195 E.) verdeckt den hinteren Kopfrand: PV 137 px (12 %, x 430-441, y 201-227), P2 156 px (17 %), P1 15 px. Verdecktes Gesicht (lok x >= 150): 0 px in allen drei | GELB, Hinweis |
| Hinweis Einsatz als Knubbel | - | F331: 141 Port-px als kleiner Hoecker zwischen Rumpf und Kopf sichtbar (pr5/einsatz_kopf.png); von vorn F348/F376 als kleiner Knoten auf der Brust links unter dem Kopf (pr5/vorn_r5.png) | Hinweis |
| Gegenprobe V7 | ROT | 40 Ansichten ueber dem Soll | ROT, wie verlangt |
| Gegenprobe V8 r4 | ROT | 62 Ansichten ueber dem Soll | ROT, wie verlangt |
| Gegenprobe 91885ed8c | ROT | 62 Ansichten ueber dem Soll | ROT, wie verlangt |

## P4 - Unterseite des grossen Schwanzes

Nutzer 2026-11-09: "Die Unterseite des großen Schwanzes ist fleischig wenn man in die Vorlagen Bilder schaut. Nur die oberseite ist durchweg grau."

| Punkt | Soll | Ist (eigen) | Bewertung |
|---|---|---|---|
| Grenze an den Vorlagen | eigene Messung | Lage um die Achse theta: Vorlage rot bis theta 110, Uebergang 110-120, ab 120 grau (0 % a* >= 10). Die Angabe des Bauers stimmt | - |
| Herkunft | 0 fremde Texel | 72 Schwanz-Flaechen auf Zeile 495 (= Original-EM30-Zeile 482, 0 abweichende Eintraege), Texel ungleich Original: 0. Textur und CLUT bytegleich V8 r4 | GRUEN |
| Unterseite fleischig (Mantel) | ja | PV / P2 unbeleuchtet, theta <= 100: Zeile 495 99,6 / 99,7 %, a* >= 10 63,5 / 64,9 %. Szene, 45 Bilder F262-F343: Fleisch-Anteil a* >= 10 beleuchtet im Mittel 41,8 % | GRUEN |
| Oberseite durchweg grau | ja | theta >= 135: 0 % Zeile 495, 0 % a* >= 10, a* max 3,7; grau in der Szene 0,00 % a* >= 10 | GRUEN |
| Uebergang Mantel Fleisch zu Grau | so ruhig wie im Original-Birkin zwischen Haut und Fleisch | 16 Kanten, da* 5,69, dL 8,50, 0 % der Laenge mit da* >= 15 (groesstes 11,1). Original-EM30, 45 Kanten Haut/Fleisch: da* 7,81, dL 7,54, 12,5 % >= 15, groesstes 21,5. Echte Bilder: 6536 Grenz-px, da* +4,45, 0 px mit da* >= 15 | GRUEN |
| **kein Streifen: Ring-Deckel** | so ruhig wie im Original-Birkin; kein Streifen | Die Deckel-Dreiecke an den Gliedenden (Zeile 487) sind grau geblieben und liegen auch auf der Unterseite. Kanten Fleisch/Deckel: 98 Kanten, **da* 14,98, 62,7 % der Laenge >= 10, 40,5 % >= 15, 37,5 % >= 20**, groesstes 27,3 (Original Haut/Fleisch 7,81; 26,3 %; 12,5 %; 5,4 %; 21,5). Echte Bilder: Deckel sichtbar in 78 von 113 Schwanz-Bildern (Summe 17059 Port-px, groesstes 859 in F313); **von Fleisch umschlossen 16768 px in 69 Bildern** (F317 837, F318 763, F275 634, F264 491, F322 472); Grenze Fleisch/Deckel 3589 px, da* +19,79, 81,9 % >= 15. Im Bild: graue harte Querbaender in der roten Unterseite (pr5/p4_deckel_echt.png) | **ROT** |
| Lage der Grenze | wie Vorlage (110-120) | Modell rund 10 Grad weiter oben: Band 120-130 traegt 35,6 / 53,7 % Zeile 495 und 9,1 / 15,9 % a* >= 10 (Vorlage 0 %) | GELB |
| Fleisch bis zur Spitze | Vorlage | Vorlage Rot-Anteil Unterseite je Glied PV: 16 84,5 %, 17 50,9 %, 18 8,4 %, 19 2,1 %, 20-22 0 % (dunkel, nicht lesbar). Modell 54-68 % auf allen Gliedern bis zur Spitze. Ab Glied 19 ist das nicht aus der Vorlage belegt, sondern aus dem Nutzer-Wortlaut gefolgert (der Bauer nennt es selbst offen) | GELB, Entscheidung beim Nutzer |
| Gegenprobe V8 r4 | ROT | 0 Flaechen auf 495, 0 % Fleisch | ROT, wie verlangt |
| S1 / S2 a-c unveraendert gruen | <= 4 / <= 8; <= 4 / 6; <= 2; <= 3 | S1 2,23 / 6,0; S2a 2,20 / 5; S2b 1,7; S2c 2,0 | GRUEN |

Die Geometrie der Glieder 17-22 ist bytegleich V8 r4, nur die CLUT-Zeile der Unterseiten-Vierecke wechselt. Der rote Punkt ist im Spiel deutlich sichtbar,
und der Riegel des Bauers misst ihn nicht (er misst nur den Mantel). Zum Vergleich: V7-Schwanz alle Kanten 487/487, da* 0,69.

## P1 / P2 - Brust und Naht

P1-Soll: "Kanten-Sprung Brust/Flanke von vorn (dL, da* entlang der Grenze) hoechstens wie V7 an derselben Stelle; Zahl reinweisser Texel in der Brust 0; Gegenprobe V8 r4 ROT."

| Mass (18 echte Bilder F344-F378, pr5/p1c.txt) | V8 r5 | V7 | V8 r4 |
|---|---|---|---|
| gerichteter Sprung dL / da* (Brust minus Flanke) | **+0,87 / -1,03** | +0,89 / +2,60 | -11,17 / -1,63 |
| Betrag je Grenzpunkt dL / da* | **11,24** (9,77-12,65) / **4,10** | 7,04 (5,57-7,55) / 3,43 | 13,77 / 3,82 |
| Rauheit innen Flanke / Brust | 7,41 / 7,03 | 4,66 / 4,42 | - |
| Grenze / innen | 1,52 | 1,51 | 1,86 |
| fast weisse Bildpunkte Brust / uebriges Mesh 8 | 0 / 0 | 0 / 0 | 33 / 450 |
| Fenster-Zaehlung fast weiss | 1 | 1 | 628 |

Offline unbeleuchtet (P1 + F344-F378, pr5/p1b.txt): gerichtet dL V8 -0,12, V7 +0,94, r4 -14,79; Betrag 10,55-10,60 / 5,81-5,85 / 16,3.
Reinweisse Texel in der Brust: 0 (34 Brust-Flaechen jetzt auf Zeile 494 = Original 481, 28 davon mit den V7-UV).

Bewertung **GELB**: Als gerichteter Sprung (eine Stufe an der Grenze) ist das Soll erfuellt, V8 r5 liegt bei dL und da* nicht ueber V7, und das Verhaeltnis
Grenze/innen ist gleich V7 (1,52 gegen 1,51). Als Betrag je Grenzpunkt gelesen ist es **nicht** erfuellt (dL 1,6-fach, da* 1,2-fach von V7). Der Betrag misst die
Rauheit der Original-Haut mit: die Flanke ist schon innen rauer (7,41) als die V7-Grenze (7,04), mit Original-Haut ist dieser Wert also nicht erreichbar.
Der Bauer nennt dieselbe Einschraenkung ("strenge Lesart dL <= V7 nicht erfuellt"). Welche Lesart gilt, entscheidet der Nutzer.
Weiss 0: GRUEN. Gegenprobe V8 r4: ROT (gerichtete Stufe -11 bis -15, 483 fast weisse Bildpunkte), wie verlangt.

P2: S2c **2,0 px bei x 367 in allen 7 Halbpixel-Varianten** (Soll <= 3): GRUEN. Gegenproben V7 7,0, V8 r4 und 91885ed8c 8,0: ROT. Dazu wurde Mesh 16 (Schwanzwurzel) geaendert.
Hinweis: S2b 1,7, in 2 von 7 Varianten 2,3 (Soll <= 2), wie in Runde 4 fragil.

## Uebrige Punkte wie Runde 4

| Punkt | Soll | V8 r5 (eigen) | Bewertung |
|---|---|---|---|
| S1 Kontur T | <= 4 / <= 8 | 2,23 / 6,0 (Varianten 2,23-2,41 / 6-7) | GRUEN |
| S2 a / b / c | <= 4 / 6; <= 2; <= 3 | 2,20 / 5; 1,7 bei x 333; 2,0 bei x 367 | GRUEN (b fragil) |
| S3 | Kuppe <= 4; H 5-10 bei x 428-445; Sattel <= 2; K <= 3 | 3,0; 8,0 bei x 429; 0,9; 3,0 | GRUEN |
| S4 Breite | Wurzel >= 55; s 90-130 >= 36; Ende <= 10; monoton | 59,2; 40,4 / 38,2 / 37,9; 5,6; monoton | GRUEN |
| S5 Gesicht | bytegleich V7 | Mesh 9 4280 Byte, Zeile 486, 6379 Texel gleich | GRUEN |
| S6 Boden | 0 / 912; Szene y <= 0 | 0 / 912 (max -25 E.); Szene tiefstes y -122, 0 Takte y > 0 | GRUEN |
| Gelenke | <= 25 Grad | Gliedlaengen 541 x 6; max 25,2; 0 ueber 25,5 | GRUEN |
| S7 Herkunft / Adern | jeder Texel aus dem Original-EM30 | Zeilen 493/494/495 = Original 480/481/482 (0 abweichende Eintraege); Texel ungleich Original 0; Textur = V7 | GRUEN |
| S7 Hinweis | - | 1 Texel ausserhalb der Original-Nutzungsmaske: Mesh 8 d65 (Wirtsflaeche des Einsatzes), Texel (170,0), Randtexel | Hinweis |
| O1 Oberarm 13 | 35-59 % UND <= Original 57,5 | 49,0 % | GRUEN |
| O1 Rumpf 8 | 25-80 UND <= Original 49,4 | lok z < 0: 48,6 % (knapp); lok x < 0: 59,0 % (Original 60,1) | GRUEN, knapp |
| O1 Flecken je 1000 px | <= Original | 8+13: **6,18** (Original 6,09); 8+13+14: 4,93 (Original 5,03); 14: 1,69 (Original 2,32) | GELB: 8+13 um 0,09 ueber dem Original |
| O1 Schulter Feld C | kein Fleck | a* >= 10 beleuchtet 51,2 % (Vorlage 50,5 %), Median a* 10,4, unveraendert gegen r4 | GRUEN |
| O2 Huegel rot | wie Vorlage 58,3 % | **45,8 %**, Median a* 9,9 (Vorlage 58,3 % / 12,1; V8 r4 58,3 %; V7 0,0). Das Bauer-Soll >= 40 ist eine eigene Toleranz | GELB: 12,5 Punkte unter Vorlage und unter r4 |
| S8 Technik | <= 16 Zeilen; <= 4096 | 16 Zeilen, Hoehe 4096; 23 Meshes, 23 Knochen, 21 Clips | GRUEN |
| O3 Kontur von vorn P1 | hoechstens wie V7; keine neue Ecke | max 2 px ueber V7 (in 1 von 9 Varianten 3 px), Flaeche 16 px (r4 12). Neue Stufe bei x 457-461, Hoehe 2,5-4,0 in 9 von 9 Varianten (bei x 460/461 faellt die Oberkante von y 170 auf 174; V7 glatt; r4 nur Schwellenfall) | GELB |
| O3 Kontur von vorn in der Szene (960 x 720) | hoechstens wie V7 | max 6 px ueber V7 (F347-F378; = 4 px bei 640), 14 Bilder ueber 3 px, neue Ecken in 12 Bildern (r4: max 7, 16 Bilder ueber 3 px, 4 Bilder mit Ecke). Kein Kamm, keine Platte (pr5/vorn_r5.png) | GELB |
| O3 Kappe | 0 | a* >= 30 im Kopf-Umfeld: P1 2 px (V7 3, r4 3), PV 1 px (V7 0); a* >= 20 P1 34 (V7 49) | GRUEN |
| O3 Schlitz echte Bilder F340-F380 | wie V7 | oberste 40 Zeilen der Figur: 0 px (V7 21, V8 r4 1); oberste 120: 235 (V7 268, r4 98) | GRUEN |
| S10 Wurzel von hinten | 0 px F328-F346 | **0 px** in F328-F346 und F300-F380 (r4 0; V8 r3 zur Probe 46090) | GRUEN |
| Szene / state.log | gleich V7 | EXIT 0, 901 Bilder; state.log 1131 Zeilen bytegleich V7; Szene Ende: Cut 4, Birkin Zustand 0 | GRUEN |
| P2 Haken | keiner | Bild-Drehwinkel PV -24,4 / -13,1 / -19,1 / -4,8 / -6,1 / -19,9, alle gleichsinnig | GRUEN |

## Kette, Pin, Tests, Code

- Asset = Pin (sha256 oben, 271716 Byte). Eigener Kettenlauf liefert dasselbe Asset (pr5/kette_lauf.txt).
- Bau ueber local_build.sh build mit dem Bau-Baum dieses Worktrees: nichts zu tun, Stand aktuell (pr5/build.txt).
- ctest Gruppe Birkin-Formen **8/8 gruen** (354,32 s); Unit-Suite **1013 von 1013 gruen** (381,92 s).
- Code-Diff 282e4d594..HEAD (73 Dateien): ausserhalb von analysis/ und re15_port/tools/r41_sichel/ nur EM30_HANGAR.EMD, release/em30_hangar.sha256 und
  re15_port/tests/unit/test_birkin_formen.c. **Keine Engine-Datei geaendert** (auch gegen c29e05f4d ist test_birkin_formen.c die einzige C-Datei).
  Keine erfundene Adress-Konstante: die 2 neuen Treffer der Adress-Schreibweise sind der Satz "keine Zahl hier ist eine @0x-Konstante. Jede Zahl ist PORT-WAHL";
  13 neue PORT-WAHL-Vermerke.
- test_birkin_formen.c erwartet: Mesh 8 Zeile 493 56 Flaechen, 494 48 + Einsatz 18, 488 0; Einsatz 12 Dreiecke / 6 Vierecke / 14 Ecken; Wirtsflaeche d65 mit
  UV (37,19) (42,0) (50,22). Deckt sich mit meiner Zaehlung am Asset.
- Geometrie gegen V8 r4: nur Mesh 8 (29 Ecken bewegt + 14 neue) und Mesh 16 geaendert; Meshes 17-22 Geometrie gleich (nur CLUT-Zeilen der Unterseite);
  Meshes 0-7 und 9-15 in allem gleich r4; Textur und CLUT gleich r4.

## Belege (alle unter pr5/, voller Pfad siehe Verfahren)

- P5: luecke_V8.txt, luecke_R4.txt, luecke_P4.txt, luecke_V7.txt, vorlage_luecke_eigen.txt, kasten_modell.txt, ko.txt, ko.json, ko2.txt, ko2.json,
  ko_blick.png (Rohtreffer F316/F318/F326/F311/F324/F332), F311_kachel.png (Zeilen F310, F311, F312; Spalten Bild, Luecke gelb + Verdeckung blau, Marker),
  einsatz_kopf.png, einsatz_z.txt.
- P4: p4.txt, p4b.txt, p4kante.txt, p4orig.txt, sw.txt, sc.txt, p4_szene_blick.png, p4_schwanz_echt.png, p4_deckel_echt.png.
- P1 / P2: p1b.txt, p1c.txt, weiss.txt, form_V8.txt, form_R4.txt, form_P4.txt, form_V7.txt, jitter.txt, vorn_r5.png.
- Uebriges: s5s7s8.txt, s7r3_V8.txt, s7lit3.txt, s6.txt, kette3d.txt, huegel3_r5.txt, schulter4_r5.txt, kappe_r5.txt, s9_r5.txt, s9j_r5.txt, s9szene5.txt,
  loch5.txt, s10r5.txt, szene_eigen.txt, ctest_gruppe.txt, ctest_unit.txt, build.txt.

## Fazit

Nicht bestanden.

- ROT: P4 Ring-Deckel (graue harte Querbaender in der fleischigen Unterseite, 69 Szenenbilder).
- ROT - Einzelbild, Entscheidung beim Nutzer: P5 Bild F311 (54 px / 31 tief / 5,8 px; Nachbarn im Soll).
- GELB: P1 Kantensprung (gerichtet wie V7, als Betrag 1,6-fach); P4 Grenze rund 10 Grad zu hoch; P4 Fleisch ab Glied 19 nicht aus der Vorlage belegt;
  Huegel rot 45,8 % (r4 58,3 %); Kontur von vorn mit neuer Stufe (P1 und 12 Szenenbilder); Hinterkopf-Rand 12-17 % verdeckt; Flecken 8+13 6,18 gegen 6,09; S2b fragil.
- GRUEN: P2, P5 in allen uebrigen 176 Ansichten und 143 echten Bildern, Kopf bytegleich, Einsatz nie ueber dem Gesicht, S1-S8, S10, Schlitz, Kappe, Schulter,
  Boden, Gelenke, Herkunft, Szene/state.log, Kette, Pin, Tests, Code-Diff. Gegenproben V7, V8 r4 und 91885ed8c sind wie verlangt rot.
