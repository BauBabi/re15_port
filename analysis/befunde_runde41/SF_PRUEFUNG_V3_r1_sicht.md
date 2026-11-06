# SF Sicht-Pruefung V3, Runde 1 (2026-11-06)

Pruefer: Sicht-Pruefer (Auge des Nutzers). Nichts am Code geaendert, nicht committet.
Ergebnis: **NICHT bestanden** - zwei Punkte in der Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde" (A4 Kopf seitlich, B1/B2 Maul).

Vorlage ("Original" des Nutzers): KI-Freisteller C:/workspace/git/reAi_v2/Version_80%/birkin12.png (Seite h85, 390x506), birkin19.png (vorn h118, 346x571), birkin20.png (vorn h125, 414x601), birkin08/11/13.png.
Geprueft: V3-Leisten in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v3/ (01_seite 2800x2162, 02_vorn 3000x1522, 04_gang 3800x7388, 05_kopf_nah 2400x2186, 07_schwanz_nah 1440x590, 08_rundum_V3 1920x1056, 09_farbe, messansicht_kopf_V3.png, riegel_kopf_V3.png 1200x960, riegel_V3.txt 34/34 ok, riegel_V1_gegenprobe.txt 26 ROT).
Eigene Ausschnitte/Masken: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/pruef/ (crops.py, ki19_kopf_x5.png, ki20_kopf_x5.png, ki12_kopf_x5.png, ki12_schwanz_x3.png, v3_h118_kopf.png, v3_h125_kopf.png, v3_h85_kopf.png, seite_v3_h85_kopf_x3.png, seite_ki_h85_kopf_x3.png, seite_ref2_h85_kopf_x3.png, rosa_platte_v3_h85.png, diff_ref2_v3_h118.png, violett_maske_08flanke.png, v3_schwanz_flanke_nah.png, v3_rundum_180.png).

Konventionen: H = Oberkante Buckel bis Sohle; (B) = Betrachter, (W) = Wesen; Pixelangaben nennen die Datei und deren Groesse.

## Klassen
- K1 = wie Vorlage
- K2 = Abweichung klein
- K3 = Abweichung, die der Nutzer wieder ablehnen wuerde

## (a) Augen

| Nr | Punkt | Vorlage (KI) | V3 | Klasse | Beleg |
|---|---|---|---|---|---|
| A1 | Anzahl, Lage | 1 dunkles Kopfauge oben mittig am Stirnrand (ki19_kopf_x5.png: Auge bei x 240-310, y 190-220 = birkin19 (178-192, 38-44)); dazu ein Schulterauge (B) links (birkin19 ca. (40-60, 110-125)) | 1 Kopfauge oben mittig, Schulterauge (B) links sichtbar (05_kopf_nah V3 h118: dunkler Punkt bei (75, 215) des x4-Feldes); Riegel R1 = 1, R2 -0.117 / +0.059 H | K1 | 05_kopf_nah Zeile 1/2 Spalte 5; riegel_V3.txt R1/R2 |
| A2 | Aussehen des Kopfauges von vorn | dunkle mandelfoermige Hoehle ca. 0.02 x 0.01 H, darin hellgrauer Augapfel (K11: L* 71) mit dunkler Pupille; kein Ring, kein Weiss (ki19_kopf_x5.png, ki20_kopf_x5.png) | dunkles Rechteck 0.0176 x 0.0105 H mit EINEM weissen Glanzpixel rechts unten (Kern L* 52.3), kein Ring, kein Kreuz (riegel_kopf_V3.png, gruener Kreis um (655,420); Spielbild 05 V3 h118/h125 ca. 4x2 px nativ). Liest sich als Auge, nicht mehr als Zielmarke | K2 | 05_kopf_nah Zeile 4; 09_farbe untere Reihe; R5-R8 |
| A3 | Umfeld des Auges | Augenumfeld L* 33.1 (birkin19 x 165-205, y 28-56) = so hell wie die (B) linke Schulterhaut (33.0): kein eigener heller Fleck; Brauenwulst grau-braun | Gesichtsfeld ueber dem Maul L* 33.7 (riegel_kopf_V3.png x 560-760, y 340-500) gegen Nachbarhaut 21.1 (links) / 19.1 (rechts): helle "Maske" unter dem Buckel, im Spielbild (05 V3 h118) als hellgraue Platte lesbar | K2 | riegel_kopf_V3.png; 05 Spalte 5 |
| A4 | Auge/Gesicht in der SEITENKAMERA h83/h85/h92 (Hauptansicht des Hangars, 3 von 5 Bezugsbildern) | Schnauze ragt 58 px = 0.115 H nach vorn (birkin12: rechter Rand x 387-388 bei y 110-135 gegen x 329 bei y 150); dunkles Auge bei (387,105); Zaehne bei (341-360, 133-138); Gesicht von der Seite klar sichtbar (seite_ki_h85_kopf_x3.png) | KEIN Auge, KEIN Maul, keine Schnauze. Der Kopf zeigt seitlich dieselbe flache lachsrosa Platte wie Ref2: 380 (h85) / 388 (h83) / 489 (h92) Pixel im V3-Feld der 01_seite (Ref2: 411 / 398 / 525), L*-Streuung 6-9 gegen 14.1 der Oberarmhaut (= untexturiert). Auch in 08_rundum 240/270 Grad ist nur der rote Wundfleck zu sehen, kein Gesicht | **K3** | 01_seite Zeilen 1-3 Spalte 5 vs Spalte 3; seite_v3_h85_kopf_x3.png vs seite_ref2_h85_kopf_x3.png; rosa_platte_v3_h85.png |

Zu A4: Ursache ist die Vorgabe "Mesh 9 nur UV" - die Seiten-/Oberflaechen des Kopfklotzes liegen weiter auf der flachen Rosa-Flaeche der Textur, nur die Vorderflaeche traegt das Gesicht. Der Nutzer sieht in der Seitenkamera also exakt den Kopf, den er bei V1 abgelehnt hat ("nicht mal ansatzweise wie im Original"). Entweder bekommen die Seitenflaechen Hauttextur + Augenprofil (UV reicht dafuer), oder die Kopfform braucht einen Schnauzenvorsprung (Geometrie; dann Freigabe des Nutzers, weil Kopf nicht Koerper ist).

## (b) Maul

| Nr | Punkt | Vorlage (KI) | V3 | Klasse | Beleg |
|---|---|---|---|---|---|
| B1 | Oeffnung / Lippen | Schmaler, fast geschlossener waagerechter Spalt mit dunkelroten, GLAENZENDEN Lippen (Grinsen); dunkle Zone im 05-KI-Feld (x4) 72 px (h118) / 84 px (h125) hoch, darin 39 / 32 Glanzpixel (> 170) auf den Lippen; birkin20 leicht geoeffnet, Spalt schmal | Weit aufgerissenes schwarzes Maul OHNE Lippen: dunkle Zone 110 px (h118) / 125 px (h125) = 1.5x hoeher, dunkle Flaeche 8522 vs 4891 Pixel = 1.74x, 0 Glanzpixel. Liest sich als Hai-/Piranha-Maul, nicht als Grinsen (v3_h118_kopf.png, v3_h125_kopf.png gegen ki19_kopf_x5.png, ki20_kopf_x5.png) | **K3** | 05_kopf_nah Zeile 1/2, Spalte 1 vs 5; Messung crops/dunkel() |
| B2 | Zaehne | Winzige blasse Nubben, vereinzelt (birkin12: 5 helle Pixel > 150 in (341-360, 133-138); birkin19/20: 6-8 Stummel 2-3 px) | 11 oben / 4 unten spitze Dreiecke, Teil bis 0.0032 H, L* 43 (Haut 32.4) - im Spielbild als durchgehende Zackenreihe lesbar | K3 (mit B1) | 05 Zeile 4 Spalte 5; R14 |
| B3 | Breite, Lage | Maul waagerecht, B/H ca. 2.0 (Messblatt), direkt unter dem Auge | B 0.0805 H, B/H 1.88, 0.0535 H unter dem Auge, Versatz 0.0067 H | K1 | R10-R12 |
| B4 | Farbe Maulzone / Kinn | Lippen dunkelrot glaenzend (L* 16.2, birkin19 x 170-215, y 58-72), Kinn/Muzzle rot-rosa fleischig (L* 15.8) | Maul innen L/Haut 0.213, a* +5.9; Kinn flach rosa ohne Glanz | K2 | 09_farbe; R13 |

## (c) Schwanzfarbe und Muster

| Nr | Punkt | Vorlage (KI) | V3 | Klasse | Beleg |
|---|---|---|---|---|---|
| C1 | Farbe gegen Koerperhaut | Lab Schwanz [18.6, 6.3, 1.4] vs Haut [29.4, 9.7, 2.8]; L/Haut 0.63, C/Haut 0.64 (gleiche Hautfamilie, dunkler, kaum bunter) | Lab [16.4, 8.0, 2.8] vs Port-Haut [28.3, 13.9, 7.0]; L/Haut 0.58, C/Haut 0.55, dh -7.3 Grad, dE00 zur Haut 10.1; Lab-Differenz KI-Schwanz zu V3-Schwanz 2.2 / 1.7 / 1.4 (dE00 ca. 3). V1 lag bei L/Haut 0.84, C/Haut 1.47 (lachsrot mit roten Ringen) - das ist weg | K1 | 09_farbe obere Reihe; R15-R19 |
| C2 | Muster | Schuppig-grau mit roter Marmorierung entlang der Unterseite, keine Ringe (ki12_schwanz_x3.png) | Hautstreifen A der Figur, keine Ringbaender (R20 Periode 0.10, Rest-Std a* 1.67); Adernzeichnung wie am Koerper; gleichmaessig dunkelbraun-rot, weniger grau-gruen als die KI | K2 | 07_schwanz_nah; R20 |
| C3 | Fremdfarbe | keine | violett-blaue Adernpixel: 28 px in 08_rundum "Schwanz rechte Flanke nah" (BBox 118-291 / 11-135 im 480x490-Ausschnitt), 6 px im 180-Grad-Feld; im Spielbild 07 V3 h85 0 px (Schwelle b > r+15, b > g+25). Dieselben Adern hat die Koerperhaut (Ref2), also keine schwanzeigene Fehlfarbe | K2 | violett_maske_08flanke.png |

## (d) Schwanzform

| Nr | Punkt | Vorlage (KI) | V3 | Klasse | Beleg |
|---|---|---|---|---|---|
| D1 | Glaette / Bogen | glatter Bogen, 2 Knicke, max 28.3 Grad je 0.05 H (birkin12) | max 27.4 Grad, 2 Knicke (h85); Gang h84-h132 max 16.2 Grad, 0 Gelenke > 20 Grad; V1 hatte 38.5 Grad / 5 Knicke | K1 | 01_seite Zeile 2; R25/R26, R25b |
| D2 | Dicke | 0.179 / 0.088 / 0.037 H bei s 0.2 / 0.5 / 0.9 (birkin12) | 0.164 / 0.085 / 0.036 H (h85), kein Dickenanstieg | K1 | R21-R24 |
| D3 | Wurzel / Austritt | aus dem oberen Ruecken hinter dem Buckel, so dick wie der Oberarm | Austritt 0.145 H unter OK (Soll 0.10-0.16) | K1 | R27 |
| D4 | Ende | spitz, leicht gebogen, dunkelrote Spitze (ki12_schwanz_x3.png unten links) | spitz, keine Kralle, Spitze farblich nicht abgesetzt | K2 | 07_schwanz_nah; 08 0-Grad-Feld |
| D5 | Querschnitt (nur Rundum, nicht Hangar-Seitenkamera) | rund, armartig mit Gelenkknick (birkin12) | flache Klinge: in 08_rundum 180 Grad eine breite flache Finne mit Kante (v3_rundum_180.png), Flanke nah als Band mit scharfer Oberkante. In der Spielkamera h90/h96 (04_gang) wirkt der Bogen wie im Original-Video (breiter dunkler Bogen). Kein KI-Rueckbild in dieser Pose (birkin08 zeigt eine andere Pose mit haengendem Schwanz) | K2 / offen | 08_rundum 120/180/Flanke; 04_gang h90/h96 |

## (e) Gesamteindruck / Koerper unveraendert

| Nr | Punkt | Soll (Ref2) | V3 | Klasse | Beleg |
|---|---|---|---|---|---|
| E1 | Koerper in der Vorderansicht | identisch zu Ref2 | Pixel-Diff Ref2 vs V3 h118 (02_vorn, Felder ca. 594x640 bei 3000 px Breite, Schwelle Summe RGB > 60): 6416 von 380160 Pixeln (1.7 %); Lage: Kopf (Zeilenbaender 0-1: 5108 px), Schulterauge, und die bei Ref2 zwischen den Beinen sichtbare Schwanzspitze (Baender 3-4: 1308 px; bei V3 verdeckt = wie Original "Schwanz hinter dem Koerper"); Rumpf/Arme/Beine/Jeans/Klauen 0 Differenz (diff_ref2_v3_h118.png). R28: Meshes 0-15, Skelett, EDD identisch, Mesh 8 +1 Viereck | K1 | 02_vorn; diff_ref2_v3_h118.png; R28 |
| E2 | Gang / Szene | Ref2-Gang | 04_gang h84-h132: Koerperpose Spalte 3 = Spalte 5 in allen 9 Takten; Schwanz folgt als Bogen, max Gelenkwinkel 16 Grad; V1 schlug mit bis 165 Grad aus | K1 | 04_gang; R25b/R26b |
| E3 | Seitenkamera gesamt (h83/h85/h92) | - | Koerper, Sichel, Jeans wie Ref2; Schwanz deckt sich mit der KI-Kontur (Magenta-Maske); Kopf siehe A4 | K1 ausser A4 | 01_seite |

## Urteil
- bestanden = **false**.
- K3: A4 (seitlich kein Gesicht, flache Rosa-Platte wie Ref2 in 3 von 5 Bezugsbildern) und B1/B2 (aufgerissenes Hai-Maul mit Zackenreihe statt geschlossenem Grinsen mit glaenzenden Lippen).
- K2: A2 (Auge = Rechteck + 1 Glanzpixel statt Hoehle mit grauem Augapfel), A3 (helle Gesichtsplatte), B4, C2, C3 (violette Adernpixel nur in der Neutralansicht), D4, D5.
- K1: Augenzahl/Lage, Maulbreite/Lage, Schwanzfarbe (dE00 KI zu V3 ca. 3; V1-Lachsrot ist weg), Schwanzglaette/Dicke/Wurzel, Koerper/Gang = Ref2.

## Was der Bauer fuer Runde 2 braucht
1. Maul: dunkle Oeffnung auf hoechstens 2/3 der jetzigen Hoehe (Ziel: Verhaeltnis dunkle Zone V3/KI 1.0 statt 1.5); darueber und darunter ein dunkelroter Lippenstreifen mit 1-2 hellen Glanztexeln (KI: 32-39 Glanzpixel > 170 im x4-Feld, V3: 0); Zaehne als Stummel ohne Dreiecksspitze (KI: 5 helle Pixel in birkin12, Hoehe 2-3 px von 504 H = 0.005 H).
2. Kopf seitlich: Seiten- und Oberflaechen von Mesh 9 auf Hauttextur legen (Rosa-Platte 380-489 px im 01_seite-Feld muss auf 0); Augenprofil (W) rechts und links (dunkler Fleck ca. 0.02 x 0.01 H an der Stirnkante) auf die Seitenflaechen. Ob ein Schnauzenvorsprung (KI 0.115 H) ohne Geometrieaenderung erreichbar ist, bleibt offen - Geometrie waere eine Kopf-Aenderung, die der Nutzer freigeben muss.
3. Auge: in der Hoehle einen 1-2 Texel grossen hellgrauen Kern (KI L* 71) statt eines einzelnen weissen Pixels; Gesichtsfeld nicht heller als Nachbarhaut +5 L*.
