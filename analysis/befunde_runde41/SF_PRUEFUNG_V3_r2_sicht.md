# SF Sicht-Pruefung V3, Runde 2 (2026-11-06)

Pruefer: Sicht-Pruefer (Auge des Nutzers). Nichts am Code geaendert, nicht committet.
Ergebnis: **NICHT bestanden** - zwei Punkte in der Klasse K3 (A2 Auge ohne hellen Kern, B1 aufgerissener Rachen). Beide sind gegenueber Runde 1 nicht besser, A2 ist schlechter geworden.

Vorlage ("Original" des Nutzers): KI-Freisteller C:/workspace/git/reAi_v2/Version_80%/birkin12.png (Seite h85, 390x506), birkin19.png (vorn h118, 346x571), birkin20.png (vorn h125, 414x601), birkin08.png (416x524), birkin11.png (390x453), birkin13.png (372x508).
Geprueft: C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v3/ - 01_seite_h83_h85_h92.jpg (2800x2162), 02_vorn_h118_h125.jpg (3000x1522), 04_gang_h84-h132.jpg (3800x7388), 05_kopf_nah.jpg (2400x2186), 07_schwanz_nah.jpg (1440x590), 08_rundum_V3.jpg (1920x1056), 09_farbe.jpg (1440x440), 10_kopf_seite_spielmassstab.png (1608x600), 11_kopf_vorn_spielmassstab.png (1314x1010, verlustfrei - Hauptquelle der Zahlen), riegel_kopf_V3.png, messansicht_kopf_V3.png (1200x960).
Eigene Ausschnitte und Messungen (python, PIL): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/sicht_r2/ - ki19_kopf_x6.png, ki20_kopf_x6.png, ki12_kopf_x6.png, ki12_schwanz_x3.png, kopf_h118_KI_r1_r2_x9.png (KI | V3 r1 | V3 r2 nebeneinander, x9 des 1280er Spielbilds), regionen_11_oben.png (Messfelder), auge_KI_h118_x8.png, auge_V3r1_h118_x8.png, auge_V3r2_h118_x8.png.

Konventionen: H = Oberkante Buckel bis Sohle; (B) = Betrachter, (W) = Wesen; Lab = CIE-Lab D65 aus dem RGB-Mittel des Feldes; "x3-Feld" = Leiste 11 (1280er Spielbild dreifach vergroessert, 1 Spielpixel = 3 px, 1 Pixel des 640er-Bildes = 6 px).

## Klassen
- K1 = wie Vorlage
- K2 = Abweichung klein
- K3 = Abweichung, die der Nutzer wieder ablehnen wuerde

## (a) Augen

| Nr | Punkt | Vorlage (KI) | V3 r2 | Klasse | Beleg |
|---|---|---|---|---|---|
| A1 | Anzahl, Lage | 1 Kopfauge oben mittig am Stirnrand (birkin19 ca. (178-192, 38-44); birkin20 ca. (205-215, 38-46)); dazu Schulterauge (B) links (birkin19 ca. (40-60, 110-125)) | 1 Kopfauge oben mittig (riegel_kopf_V3.png gruener Kreis um (655,420)), Schulterauge (B) links (cyan um (235,625)); keine Ringe, kein Kreuz, kein zweites Kopfauge | K1 | riegel_kopf_V3.png; 05_kopf_nah Zeile 1/2 Spalte 5 |
| A2 | Weiss / Glanz / Wirkung "wie ein Auge" | Dunkle Hoehle mit HELLEM Augapfel und Glanz: x3-Feld KI h118 Fenster (200-240, 150-185): 273 von 1400 Px mit L* > 55, Lmax 82; h125 (205-245, 585-625): 207 von 1600 Px, Lmax 79; Messblatt K11 heller Kern L* 71. Im Spielmassstab ein heller Punkt von ca. 2x2 Spielpixeln (1280er) in der dunklen Hoehle (auge_KI_h118_x8.png). Auf birkin19/20 x6 klar als Auge mit weisser Sklera lesbar | Dunkle Hoehle 0.0176 x 0.0105 H mit EINEM grauen Pixel: x3-Feld V3 r2 h118 Fenster (1075-1110, 160-190): 0 Px L* > 55, Lmax 54.5; h125 (1105-1145, 595-630): 0 Px, Lmax 54.5; 09_farbe "V3 heller Kern" L* 41.3 (KI 71.1). V3 r1 hatte 72 Px L* > 55, Lmax 70.4 (Fenster (640-680, 160-190)) - r2 ist DUNKLER geworden, entgegen Runde-1-Vorgabe 3 ("1-2 Texel hellgrauer Kern, KI L* 71"). Im Spielbild (05 V3 h118/h125 x4, 11 r2 x3) ist das Auge ein dunkler 4x2-Fleck auf grauer Platte ohne hellen Punkt: liest sich als Loch/Nasenloch, nicht als Auge. Im 640er-Bild (PSX-Aufloesung) faellt der 1-px-Kern ganz weg | **K3** | 11_kopf_vorn_spielmassstab.png Spalte 1 vs 3; auge_KI_h118_x8.png vs auge_V3r2_h118_x8.png; 09_farbe untere Reihe |
| A3 | Groesse, Lage ueber dem Maul | 0.019-0.022 x 0.009-0.012 H (Messblatt K3-K14), ca. 0.052 H ueber der Maulmitte | 0.0176 x 0.0105 H, B/H 1.67, 0.0535 H ueber dem Maul (05 Zeile 4 Spalte 5) | K1 | 05_kopf_nah Zeile 4; riegel_kopf_V3.png |
| A4 | Kopfoberseite / Augenumfeld (Farbe, Zeichnung) | Stirn Lab [27.9, 6.9, 1.0] (x3-Feld (150-300, 95-140)), helle Kuppe um das Auge [47.1, 7.4, 1.6] ((175-260, 120-160)); grau mit braunen Streifen wie die Koerperhaut, oben hell beleuchtet | Platte Lab [29.4, 7.1, 1.1] ((1000-1180, 100-150)), um das Auge [32.8, 6.3, 1.4] ((1000-1180, 175-215)): gleicher Farbton (a*/b* gleich), aber weniger hell als die KI-Kuppe (L* 33 vs 47) und als einfarbig-verrauschte Flaeche ohne Streifen; r1 lag mit L* 40.3 naeher an der KI-Kuppe. Liest sich im Spielbild als graue Platte/Helm | K2 | regionen_11_oben.png; kopf_h118_KI_r1_r2_x9.png |
| A5 | Kopf in der Seitenkamera h83/h85/h92 | Schnauze grau mit Streifen, ragt 0.115 H nach vorn, dunkler Augenschlitz an der Vorderkante, Zaehne unten vorn (ki12_kopf_x6.png) | Seitenflaechen tragen jetzt Haut (Wundfleisch-Bildpunkte 283/346 -> 0/0, Leiste 10), Kopfauge 1x3 px an der Vorderkante (KI 3.7x2.6), Maulprofil rot mit 5 Stummeln. Rosa-Platte aus Runde 1 ist weg. Kopf bleibt ein grau-lila Block mit rosa Unterteil ohne Schnauzenvorsprung; Augenschlitz im Spielbild (05 Zeile 3 Spalte 5) kaum lesbar | K2 | 10_kopf_seite_spielmassstab.png; 05 Zeile 3; 01_seite Spalte 5 |

## (b) Maul

| Nr | Punkt | Vorlage (KI) | V3 r2 | Klasse | Beleg |
|---|---|---|---|---|---|
| B1 | Oeffnung | Fast geschlossenes waagerechtes Grinsen; dunkle Zone besteht aus dunkelroten Lippen + schmalem Spalt. x3-Feld, Fenster KI h118 (150-300, 165-230): dunkle Flaeche L* < 15 = 3057 px, Spaltenhoehe median 50 px (= 8.3 px im 640er); L* < 12 median 6 px (= 2 Spielpixel). h125 (160-310, 605-690): 4817 px / 53 px | Weit offener schwarzer Rachen, so hoch wie breit: V3 r2 h118 (1030-1210, 210-320): L* < 15 = 12492 px (4.1x KI), Spaltenhoehe median 78 px (13 px im 640er, 1.6x KI); L* < 12 median 60 px (20 Spielpixel, 10x KI). h125 (1040-1220, 650-770): 11668 px (2.4x KI) / 80 px. Gegen r1 (L* < 12 median 54 px) ist die Oeffnung NICHT kleiner, sondern 60 px - Runde-1-Vorgabe 1 ("hoechstens 2/3 der Hoehe") nicht umgesetzt. Sichtbar in kopf_h118_KI_r1_r2_x9.png: links Grinsen mit Lippen, rechts Hai-Rachen | **K3** | 11_kopf_vorn_spielmassstab.png; 05_kopf_nah Zeile 1/2/4 Spalte 5; kopf_h118_KI_r1_r2_x9.png |
| B2 | Lippen / Glanz | Dunkelrote glaenzende Lippen, Glanzpixel max(RGB) > 170: 69 (h118) / 31 (h125) (Leiste 11 Fusszeile) | 0 / 0 Glanzpixel; keine Lippenkante, der Rachenrand geht direkt in Haut ueber | K2 (verstaerkt B1) | 11 Fusszeile; kopf_h118_KI_r1_r2_x9.png |
| B3 | Zaehne | Winzige blasse Nubben, 6-8 Stummel von 2-3 px auf 571 H (ki19_kopf_x6.png) | Stummel 7 oben / 6 unten, 0.0031 x 0.0053 H, keine Dreiecke mehr (r1 hatte Zackenreihe). Im Spielbild als einzelne helle Pixel lesbar | K2 | 05 Zeile 4 Spalte 5; 11 Fusszeile |
| B4 | Breite, Lage, Richtung | waagerecht, B/H ca. 2.0, direkt unter dem Auge | waagerecht, 0.0831 x 0.0454 H, B/H 1.83, 0.0535 H unter dem Auge | K1 | 05 Zeile 4 Spalte 5 |
| B5 | Kinn / Schnauze unter dem Maul | Grosses rundes dunkelrotes Kinn (ca. 40 % der Kopfhoehe): Lab [14.7, 12.8, 3.4] (x3-Feld (170-270, 330-380), h118), [20.3, 17.5, 5.7] (h125) | Kein Kinn: unter dem Rachen folgt sofort Koerperhaut, Lab [39.4, 2.9, 2.8] ((1040-1140, 365-395)) / [26.9, 6.0, 3.2] (h125); der rote Anteil der KI-Schnauze ist in V3 nur die Unterwand des Rachens | K2 | kopf_h118_KI_r1_r2_x9.png unten |

## (c) Schwanzfarbe und Muster

| Nr | Punkt | Vorlage (KI) | V3 r2 | Klasse | Beleg |
|---|---|---|---|---|---|
| C1 | Farbe gegen Koerperhaut | Schwanz Lab [18.6, 6.3, 1.4] vs Haut [29.4, 9.7, 2.8]; L/Haut 0.63, C/Haut 0.64 (gleiche Hautfamilie, dunkler, kaum bunter) | Lab [16.2, 8.1, 2.8] vs Port-Haut [28.3, 13.6, 6.8]; L/Haut 0.57, C/Haut 0.56, dh -7.7 Grad, Rot 0.23/0.36. Lab-Abstand KI-Schwanz zu V3-Schwanz 2.4 / 1.8 / 1.4 (dE00 ca. 3). V1-Lachsrot (L/Haut 0.84, C/Haut 1.47) ist weg; im Spielbild (07 V3 h85) dieselbe grau-braune Haut wie Ruecken und Arm | K1 | 09_farbe obere Reihe; 07_schwanz_nah |
| C2 | Muster | Schuppig-grau mit rotbrauner Marmorierung laengs, keine Ringe (ki12_schwanz_x3.png) | Hautstreifen A der Figur (Zeile 482), keine Ringbaender; Adern wie die Koerperhaut, darunter violett-blaue Adernpixel (08 "Schwanz rechte Flanke nah"), die im Spielbild 07 nicht sichtbar sind; etwas rotbrauner und gleichmaessiger als die KI | K2 | 07_schwanz_nah; 08 Flanke nah |

## (d) Schwanzform

| Nr | Punkt | Vorlage (KI) | V3 r2 | Klasse | Beleg |
|---|---|---|---|---|---|
| D1 | Glaette | glatter Bogen, max 28.3 Grad je 0.05 H, 2 Knicke (birkin12) | max 27.4 Grad, 2 Knicke > 20 Grad (h85); Gang h84-h132 max 16 Grad, 0-3 Gelenke > 20 (V1: 165 Grad / 5) | K1 | 01_seite Zeile 2 Spalte 5; 04_gang Fusszeilen |
| D2 | Dicke | 0.179 / 0.088 / 0.037 H bei s 0.2 / 0.5 / 0.9 | 0.162 / 0.089 / 0.032 H (h85); 0.176 / 0.099 / 0.035 (h83) | K1 | 01_seite Fusszeilen |
| D3 | Wurzel | aus dem oberen Ruecken hinter dem Buckel, so dick wie der Oberarm | Austritt 0.145 H unter OK (Vorgabe 0.10-0.16), Wurzel 0.16-0.18 H dick = Oberarm | K1 | 01_seite; 07 |
| D4 | Verlauf / Ende | J-Form: vom Buckel waagerecht nach (B) links, Ellbogen bei ca. 1/3 der Laenge, dann fast gerade nach unten; Spitze leicht eingebogen, dunkelrot abgesetzt (ki12_schwanz_x3.png unten links) | Gleichmaessiger C-Bogen ohne Ellbogen (Drehung 67 Grad gegen Ref2 29), Spitze spitz, nicht farblich abgesetzt; Endpunkt liegt aber am selben Ort wie bei der KI (Magenta-Maske deckt sich) | K2 | 01_seite Zeilen 1-3 Spalte 1 vs 5; 07 |
| D5 | Querschnitt (nur Rundum) | rund, armartig | hinten (08, 180 Grad) eine breite flache Finne mit Kante; in der Hangar-Spielkamera (04_gang h90/h96) wie im Original-Video ein breiter dunkler Bogen | K2 | 08_rundum 120/180 Grad |

## (e) Gesamteindruck / Koerper unveraendert

| Nr | Punkt | Soll (Ref2) | V3 r2 | Klasse | Beleg |
|---|---|---|---|---|---|
| E1 | Koerper vorn | identisch zu Ref2 | 02_vorn: "Koerper = Referenz 2", Schwanz sichtbar 0 (h118) / 3 (h125) Bildpunkte im 640er (Original: hinter dem Koerper); Rumpf, Arme, Sichel, Jeans, Klauen gleich | K1 | 02_vorn Spalte 3 vs 5 |
| E2 | Gang / Szene | Ref2-Gang | 04_gang h84-h132: Koerperpose Spalte 3 = Spalte 5 in allen 9 Takten; nur Kopf und Schwanz unterscheiden sich | K1 | 04_gang |
| E3 | Seitenkamera gesamt | - | 01_seite: Koerper, Sichel, Jeans wie Ref2; Schwanz deckt die KI-Kontur; Kopf = grauer Block (A4/A5) | K1 ausser Kopf | 01_seite |
| E4 | Gesamteindruck Kopf in der Spielkamera | Grauer Kopf mit hellem Auge, geschlossenem Grinsen und rotem Kinn | Grauer Block mit dunklem Fleck und schwarzem Rachen. Schwanz und Koerper sind in Ordnung; der Kopf ist es nicht | K3 (= A2 + B1) | 05_kopf_nah Spalte 1 vs 5 |

## Urteil
- bestanden = **false**.
- K3: A2 (Auge ohne hellen Kern: 0 Px L* > 55 gegen 207-273 bei der KI; r2 dunkler als r1) und B1 (Rachen: dunkle Flaeche 2.4-4.1x KI, Hoehe 1.6x; gegen r1 nicht verkleinert).
- K2: A4, A5, B2, B3, B5, C2, D4, D5.
- K1: A1, A3, B4, C1, D1, D2, D3, E1, E2, E3.
- Gegenueber Runde 1 erledigt: Seitenkamera-Rosa-Platte (A4 alt) und Zahn-Dreiecke (B2 alt). Nicht erledigt: Mauloeffnung (B1), Augenkern (jetzt schlechter).

## Was der Bauer fuer Runde 3 braucht (Zahlen aus dieser Pruefung)
1. Maul: dunkle Flaeche (L* < 15) im x3-Feld h118 von 12492 auf <= 4000 px, Spaltenhoehe median von 78 auf <= 55 px; darueber und darunter ein dunkelroter Lippenstreifen (Lab um [16, 13, 3]) mit 1-2 Glanztexeln (max(RGB) > 170), Ziel 10-40 Glanzpixel im x3-Feld statt 0.
2. Auge: in der Hoehle einen 2x2-Spielpixel (1280er) grossen Kern mit L* 65-75 (KI 71), kein Ring; Ziel >= 20 Px L* > 55 im x3-Fenster (KI 207-273, r1 72, r2 0).
3. Kinn: unter dem Maul ein dunkelrotes Feld (a* >= 12, L* 15-22) von ca. 0.03-0.04 H Hoehe statt Koerperhaut.
4. Nichts am Schwanz und am Koerper aendern - beide sind K1/K2.
