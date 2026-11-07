# K5c Farbe, Versuch 2 Runde 2: Sicht-Pruefung (Pruefer "Auge des Nutzers", 2026-11-07)

Gegenstand: Tafeln 01-08 in `C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe/`
(Stand 03:05-03:09, Commits 8f26798d8 / 69597b13e / eb7f6eefa / 02aed15f7), Spielbilder `farbe/spiel/` (10/10), Riegel
`farbe/riegel_K5c_farbe_v2.txt` (43/45, ROT B12, B17), `farbe/saaten_K5c_farbe_v2.txt`, `farbe/unabh_v2r2/`, `farbe/sicht_mass_v2r2.txt`,
Generator `generator/kf_mal4.py`, `kf_eich4.json`, `kf_korn_mass.py`, `farbe_lesung/N2_korn.json`, `N2_kgraph.json`, Dossier `FARBE.md` V2r2,
Pruefbericht `FARBE_PRUEFUNG_v2_r2_zahlen.md`. Originale `C:/workspace/git/reAi_v2/Version_80%/vlcsnap-2026-10-31-00h28m45s774.png` (Bild 1),
`...00h29m02s830.png` (Bild 2). Massstab: Auftrag Z1/Z2, Nutzer 2026-11-06 ("Pokemon", "passt von den Farben und der Struktur nicht zum Rest des
Spiels"), E8/E10 ("billig einfarbig bemalt", "Wunden fleischig", "Haut mehr als eine Farbe"), E14, E15 (kein Cel-Shading), E16, E22.
Nichts geaendert, nichts committet. Lab = CIE-Lab D65. (W) = Wesen, (B) = Betrachter.
Messskripte und Ausschnitte: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe2/v2r2sicht/`
(`mess08.py`, `mess08b.py`, `atlasmess.py`, `stufen.py`, `richt.py`, `raster.py`; Bilder `08_v2.png`, `04_v2_fleisch_x4.png`,
`atlas_v2_fleisch_adern_x4.png`, `atlas_herkunft_raster_x5.png`, `v2_raster_x10.png`, `sp5080_ruhe_x4.png`, `sp5090_pose2_x2.png`,
`01_Z2_ruecken_x2.png`, `06_v2_kopf_x3.png`).

## 0. Ergebnis: NICHT BESTANDEN

Zwei Punkte in der Klasse "Abweichung, die der Nutzer wieder ablehnen wuerde":

- **P1 Binnenzeichnung des Fleischs (Z1)**: Das Kernziel dieses Versuchs ist nicht erreicht. Es gibt keine Adern mit Richtung, keine Falten als
  Linien und keine gemalte Anatomie. Das Fleisch ist posterisiert: flache Lachs-, Rot- und Dunkelrot-Flaechen mit harten Konturbaendern, dazu
  Loch-Raster aus Punkten. Im Render erscheint es als Mosaik, das den Polygonkanten folgt. Im Spiel 5080 sieht der Ruecken aus wie eine rote,
  gemusterte Satteldecke. Das ist die Machart "Malen nach Zahlen" und kein Fleisch wie bei EM30 oder G4.
- **P2 Farbton des Bahnsteig-Rots (Z2)**: Menge und Ort stimmen, der Farbton nicht. Das Original zeigt am Ruecken einen magenta-rosa Fleck.
  V2 zeigt dort einen rost- bis orangeroten Fleck. Der Zahlen-Pruefer hat das bereits gemessen, ich bestaetige es unabhaengig an Tafel 01.

Was Runde 2 gegen Runde 1 erreicht hat und erhalten bleiben muss:
- Die Laengsstreifung (Fell und Holz, v2 r1 P2) ist weg. Kopf und Ruecken sind glatt.
- Bild 2 und das Spiel 5090 zeigen eine dunkle Masse mit hellem Kieferband.
- Farbfamilie, Glanz 0, Mehrtoenigkeit, Augen, Schwanz und Umrissband sind in Ordnung.

## 1. Befunde

Klassen: **F** = wie Spielfamilie, **K** = Abweichung klein, **A** = Abweichung, die der Nutzer wieder ablehnen wuerde.

| # | Punkt | Soll | Ist K5c V2 R2 (gemessen / gesehen) | Klasse |
|---|---|---|---|---|
| P1 | Binnenzeichnung Fleisch (Z1): Adern mit Richtung, Wundraender, Rotstufen, Falten | Ziel Z1: grosse Fleischflaechen mit gemalter Anatomie, wenige dunkle Adern entlang der Muskel-/Koerperachse, dunkler Rand um hellen Kern, 2-4 Rotstufen, Falten als Linien, kein Rauschen, keine Farbwolken. EM30 (08): dunkle verzweigte Adern auf heller Haut, fleischiger Wundkern. G4: rote Aderlinien auf schwarzer Haut. G5: gebuendelte Furchen | **Erreicht sind nur die Rotstufen und der konzentrische Wundrand** (Atlas: dunkler Rand, Rot, Lachs-Kern). Alles andere fehlt; Messwerte und Belege in Abschnitt 2 | **A** |
| P2 | Bild 1 Bahnsteig: Rot wie das Original (Z2) | Original Z.2 Ruecken: magenta-rosa Lichtfleck auf violetter Masse. E14: Rot nur dort und so, wie es der Screenshot zeigt | Menge und Ort stimmen: K4 5,4 % (Original 5,2), K4b 8,8 % (11,9), B2 4,90/3,95, B9 8,7 %, B9b 5,0 %. Der Farbton stimmt nicht, Messwerte in Abschnitt 3. Gesehen: Original rosa-violett, V2 rostrot | **A** |
| P3 | Laengsstreifung Haut/Kopf (v2 r1 P2) | E16, E22: glatt, keine Fell- oder Holzmaserung | Behoben. Dunkle duenne Linien am Kopf-Render 9,7 / 7,5 / 12,0 % (elongiert 2,2 / 2,3 / 4,0). Zum Vergleich G4 7,9 / 5,5 / 18,9, K5c heute 10,2 / 9,0 / 11,3 (`sicht_mass_v2r2.txt`). 06 und Spiel 5090 zeigen den Kopf glatt | F |
| P4 | Kopf unter neutralem Licht (06) | G4: Knochenschaedel mit Platten. Bild 2: dunkle Masse | Glatt mittelbraun (L* 15,7-18,7, C* 10,9-11,4), runde braune Augenknoepfe, rosa Schnauzenspitze mit schwarzen Sprenkeln. Am Nackenansatz sitzen hellgraue Strichstufen (`06_v2_kopf_x3.png`). Unter neutralem Licht liest sich das als Plueschnager und nicht als Knochen. Im Spiel 5090 und auf Bild 2 ist es eine dunkle Masse und passt (`sp5090_pose2_x2.png`). B12 ROT (Canny 4,1 gegen 25,2) ist die Zahlenseite davon | K (Grenzfall, nur in 06 sichtbar) |
| P5 | Farbfamilie (03) | G4 L* 17,0-21,6, C* 9,6-12,6; EM30 24,5-29,0 / 10,0-12,5 | V2 L* 16,3-19,2, C* 9,5-13,4. Dunkles Rotbraun, Fleischrot an Ruecken/Flanke, Blaugrau an den Gliedern, Krallen beige-rot. In der Ganzfigur neben G4 dieselbe Palette, kein Weiss, kein Magenta | F |
| P6 | Glanz / Neutralweiss | Familie L* > 85: 0,00-0,21 % | 03: 0,00-0,01 %. 06 3/4 vorn: 0,34 % (rosa Schnauzenspitze) | F (06: K) |
| P7 | Mehrtoenigkeit Haut (E10) | Familie sigma_ab je Teil 7,4-10,7 | A2 9,3, B7 10,3 Grad. Rotbraun, Blaugrau, Ocker und Fleisch nebeneinander | F |
| P8 | Texeldichte / PSX-Stil | A6/A7/A10 | Gruen, unveraendert. Die Posterisierung des Fleischs ist unter P1 erfasst | F |
| P9 | Kiefer / Maul | G4: dunkles Maulinneres, rosa Zahnfleisch, beige Zaehne | A8 6,1 / 28,4 / 40,1, B6 Kiefer 0,0, B8 17,7 %. Zahnreihe beige, Maul dunkel. Die rosa Spitze traegt weiter schwarze "Streusel" (06) | K |
| P10 | Bild 2 / Spiel 5090 | Dunkle Masse, Kieferband hell, Klauen rot | K6 dE 1,1, K7 1,9, K8 3,5, K9 26,6/27,9. Gesehen: Kopf dunkel und glatt, Kieferband hell, Klauen rot, keine Maserung mehr | F |
| P11 | Augen (E24) | dunkel | K10 14,6 / 15,9 | F |
| P12 | Schwanz aus einem Stueck (E9) | keine Stufe | A11 0,6, 05 ohne Gliedgrenze | F |
| P13 | Pfoten | Haut wie Koerper, Krallen knochenbeige | Vorderpfoten (W) weiter hellgrau-beige ("Socken"), Krallen rot | K |
| P14 | Flaechengrenzen an Polygonkanten | Fleischgrenze unabhaengig von der Mesh-Kante | Weiter an den Polygonkanten (08 Flanke: Keile und Dreiecke; Spiel 5080: gerade Ober- und Seitenkanten der roten Flaeche). Der Bauer fuehrt das selbst als offen (V2r2-O3). Im Render verstaerkt das P1 | K (Anteil an P1) |
| P15 | Rot im Umrissband | <= 11,3 % | B9 8,7, B9b 5,0. Umriss im Spiel dunkel | F |
| P16 | Konstanten mit Quelle (Z3) | Quelle je Konstante oder Nachweis | Laut Zahlen-Pruefer fehlt der Nachweis fuer K_GRAPH 8 (kg4 und kg16 kippen Riegel), fuer SPIEGEL_ABSTAND 2,0 (sp4 kippt Riegel) und fuer KORN_SIG 1,0. Fuer KORN_SD gibt es keine Messblatt-Zeile, SIG_GROB ist PORT-WAHL | K (Zahlen-Pruefer fuehrt) |
| P17 | Fairness der Tafeln | gleiches Licht, roh, kein Modell aufgehellt | 03, 05, 06 und 08: gleicher Renderer und gleiches Licht. 08: gleiche Anatomie je Modell, der r1-Mangel ist behoben. 01/02: gleiches Disc-Licht x k, gleicher Filter. 07: echtes Fenster, 10/10. Die Fleischflaechen von V2 sind nicht benachteiligt dargestellt | fair |

## 2. Belege zu P1 (Binnenzeichnung)

**Render, Tafel 08** (`mess08b.py`): Linien je 1000 Figurpixel, gestreckte Komponenten aus Black-Top-Hat L* 5x5 > 6 und Top-Hat a* > 8,
Komponente >= 12 px, Streckung >= 3.

| Ausschnitt | G4 | EM30 | r3 (Versuch 1) | V2 R2 |
|---|---|---|---|---|
| Flanke | 0,8 | 47,2 | 1,1 | **1,1** |
| Schulter | 18,5 | 15,2 | 8,7 | **10,8** |
| Ruecken | 24,8 | 51,0 | 6,3 | **10,4** |

- An der Flanke steht V2 genau auf dem Stand von r3, also dem Stand, der in Versuch 1 als "Nutzer lehnt ab" bewertet wurde.
- An Schulter und Ruecken liegt V2 bei etwa der Haelfte von G4 und bei einem Fuenftel bis zwei Dritteln von EM30.
- Dunkle Pixel in der Rotzone: Flanke V2 1,0 %, EM30 15,3 %. Ruecken V2 6,7 %, G4 17,0 %, EM30 19,7 % (`mess08.py`).

Gesehen im 2x-Ausschnitt `08_v2.png`:
- Flanke: Splitter aus Rot, Rosa, Ocker und Schwarz in Dreiecken und Keilen. Die Grenzen folgen den Polygonen, Adern sind keine zu sehen.
- Schulter: blaugraue Haut mit einem roten Keil, in dem graue Punkte stehen.
- Ruecken: braune Flaeche mit lachsfarbenen Dreiecken.

**Atlas, Posterisierung** (`stufen.py`): Fleischtexel mit a* > 15, Nachbarpaare nach Helligkeitsstufe.

| Modell | Fleischtexel | Farben im Fleisch | dL = 0 | 0 < dL < 5 | 5-10 | >= 10 |
|---|---|---|---|---|---|---|
| EM30 | 3 964 | 206 | 28,9 % | 32,7 % | 21,3 % | 17,1 % |
| G4 | 8 504 | 420 | 17,1 % | 34,2 % | 25,6 % | 23,0 % |
| G5 | 20 676 | 390 | 20,1 % | 37,5 % | 24,1 % | 18,3 % |
| r3 | 12 476 | 154 | 36,1 % | 44,7 % | 17,0 % | 2,2 % |
| **V2** | 10 011 | **89** | **52,9 %** | 16,7 % | **1,4 %** | **29,0 %** |

- V2 malt das Fleisch aus wenigen flachen Stufen mit harten Spruengen, ohne die weichen Zwischenstufen, die die ganze Familie hat (21-26 %).
- Im Ausschnitt `v2_raster_x10.png` (Atlas y 75-95, x 0-30) kommen nur die Stufen L* 6 / 18 / 20 / 32 / 37 / 51 / 52 vor.
- Das ist die Machart, die E15 ("kein Cel-Shading") und E8 ("billig einfarbig bemalt") verbieten. Neu ist nur, dass es mehrere Farben sind.
- Der Riegel B16 ("Rotpixel in Flaechen mit >= 2 Stufen", 80,7 %) belohnt genau diese Stufen. Er misst das Vorhandensein von Stufen, nicht ihre Machart.

**Adern ohne Richtung** (`masken.npz` "adern", 2 039 Texel, alle im Fleisch):
- Overlay `atlas_v2_fleisch_adern_x4.png`: 130 kurze gerade Stoecke (Median 9,9 Texel), die kreuz und quer liegen ("Mikado").
- Richtungsbuendelung je Atlasinsel R(2 theta) = 0,17 / 0,20 / 0,18 / 0,22 bei 40 / 37 / 21 / 13 Adern. Das ist Zufallsniveau (Erwartung 0,14-0,25).
- Familie mit derselben Liniendefinition je Insel (`richt.py`):

| Modell | R | Zufallserwartung | Linien |
|---|---|---|---|
| G5 | 0,74 | 0,20 | 60 |
| EM30 | 0,56 | 0,36 | 18 |
| G4 | 0,88 | 0,36 | 6 |

- Der Kontrast der Adern zum Hof betraegt im Median 11,9 L*. Im Atlas sind sie als blassgraue Striche im Lachs-Kern sichtbar, im Render aber nicht mehr (siehe Tabelle oben).
- Falten (`masken.npz` "falten", 2 581 Texel) liegen nur auf der Haut. Im Render (08 Schulter, Haut blaugrau) ist keine Faltenlinie zu erkennen.

**Punkt-Raster im Fleisch** (`atlas_herkunft_raster_x5.png`, Vergleich K5c heute, r3, v2r1 und V2 an derselben Atlasstelle):
- V2 und v2r1 enthalten rechteckige Bloecke aus regelmaessig gesetzten dunklen Punkten im Abstand von 2 Texeln ("Lochblech").
- r3, K5c heute, EM30, G4 und G5 haben das nicht.
- Zaehlung isolierter Punkte mit Raster-Nachbar (`raster.py`, Black-Top-Hat 3x3 > 4), je 1000 Texel:

| Modell | Rasterpunkte |
|---|---|
| V2 | 1,50 (66 von 77 am Fleisch) |
| v2r1 | 0,74 |
| r3 | 0,96 |
| K5c heute | 0,62 |
| EM30 / G4 / G5 | 0,24 / 0,20 / 0,29 |

  Das Mass ist schwach (r3 zaehlt mit), die Bloecke sind aber im Bild eindeutig: `04_v2_fleisch_x4.png` links unten, `sp5080_ruhe_x4.png` (dunkle Punktreihen in der roten Flanke).

**Spiel 5080 Ruhe** (`sp5080_ruhe_x4.png`, 4x, echtes Fenster): Ueber Ruecken und Flanke liegt eine rostrote Flaeche mit lachsrosa Flecken und Punktreihen, mit
geraden Ober- und Seitenkanten. Sie liest sich wie eine aufgelegte rote Decke und nicht wie aufgerissene Haut mit Adern. Neben K4 heute (dunkel, glatt) ist das der
groesste Unterschied im Bild. Der Bauer sagt selbst: "wirkt die Figur roter als K4 heute".

## 3. Belege zu P2 (Farbton Bild 1)

Tafel 01 Z.2 (Videofilter), Ausschnitt Ruecken/Schulter (Tafel x 250-520 je Kachel, y 880-1150), Rotklasse a* >= 12, C* >= 15, L* >= 20 (wie K4),
`01_Z2_ruecken_x2.png`:

| | Rotklasse | Lab | h | b* < 0 |
|---|---|---|---|---|
| Original | 12,2 % | (37,7 / 22,3 / -16,8) | -37,0 Grad | 93 % |
| K4 heute | 15,8 % | (37,0 / 19,2 / -16,7) | -41,0 Grad | 100 % |
| **V2** | 10,4 % | (31,8 / 21,7 / -1,7) | **-4,4 Grad** | 66 % |

- Die Kette des Zahlen-Pruefers zeigt dasselbe: Original h -11,8, V2 h +23,5 Grad, dE der Rot-Mittel 12,8 am Ruecken und 18,0 an der Figur.
- Gesehen: Das Original hat oben am Ruecken einen rosa-violett leuchtenden Streifen. V2 hat dort einen rostroten Fleck auf einer rotbraunen statt violetten Masse.
- In Z.1 (Disc-Licht ohne Filter) ist der V2-Ruecken orange-braun, K4 heute rosa-violett.
- Die Ursache liegt im Messblatt: V2 3.2 gibt als Ziel "Farbton 34 Grad" vor (Familienwunde). Kein Riegel misst den Farbton (K3 vergleicht nur das Zonenmittel).
- Ein Nutzer, der Tafel 01 ansieht, sieht als Erstes einen anders gefaerbten Fleck. Das ist genau die Beschwerde "passt von den Farben her nicht".

## 4. Widerlegungsversuche (was gegen meine A-Urteile spricht)

- **Entfernung**: In 03 und 05 (ganze Figur, ca. 15 k px) liest sich V2 als dunkle Figur mit roten Wunden am Ruecken, ohne Pokemon-Eindruck. Das
  Posterisierungs-Problem ist erst ab 08, im Spiel 5080 (Figur ca. 170 x 150 px) und im Atlas sichtbar.
  - Gegenargument: Das Spiel 5080 ist die Nutzeransicht, und dort ist die rote Decke mit Punktreihen bei 1x schon erkennbar (07 Zeile 2/3).
  - Gegenargument: Der Auftrag fordert ausdruecklich die Nahbetrachtung (Z1).
- **Riegel**: B10, B13, B14, B15 und B16 sind gruen. Sie messen Linienanteil, Canny-Dichte, Streuung und Stufenzahl, aber weder die Richtung noch den Unterschied
  zwischen weichem und hartem Uebergang.
  - B10 (8,81 %) zaehlt im V2-Atlas vor allem die Konturbaender der Wunden als "Linien": 17 Komponenten mit 218 Linientexeln je 1000 Fleischtexel (`atlasmess.py`).
  - Die Riegel bestaetigen damit die Stufen und nicht die Anatomie.
- **Rotmenge Bild 1**: Sie trifft (K4/K4b gruen) und ist damit kein Fehlort. Der Farbton ist aber eine eigene, sichtbare Groesse, und das Original zeigt sie eindeutig.

Beide Widerlegungsversuche tragen nicht.

## 5. Was zum Bestehen fehlt (Sicht)

1. **P1**: Das Fleisch als gemalte Flaeche mit weichen Zwischenstufen und ohne flache Posterisierung, nach Familienmass (Nachbarpaare 5-10 L* ca. 20-25 %, dL = 0 unter 30 %, Farben im Fleisch > 200).
   - Wenige, laengere Adern, je Insel entlang einer Achse gebuendelt (R deutlich ueber Zufall, Familie 0,56-0,88), mit Kontrast, der den Render uebersteht (Linien je 1000 Figurpx an Schulter/Ruecken >= G4 18-25).
   - Keine Punkt-Raster.
   - Wundgrenzen nicht an den Polygonkanten.
2. **P2**: Rotklasse Bild 1 am Ruecken in den Magenta-Bereich (h um -10 bis -35 Grad nach Kette bzw. Tafel 01), ohne K4/K4b zu verlieren. Im Messblatt V2 3.2 fehlt dafuer ein Farbton-Riegel.
3. Nicht verlieren: P3 (keine Streifung), P5-P7, P10-P12, P15.
