# Farbe und Malstil K5c gegen die Spielfamilie (Leser A "Spiel-Stil", 2026-11-06)

Auftrag: Orchestrator, Spur Form, nach der Ablehnung des Nutzers zu `rundum_bild1_K5c.png` ("passt von den Farben ... ueberhaupt nicht und von der
Struktur ... sieht eher aus wie ein Pokemon"). Nur Messung, kein Bau, kein Commit. Kein RE-Wert: K5c ist ein neues Modell (PORT-WAHL); gemessen wird,
wie die vorhandenen Spielmodelle gemalt sind und wie weit K5c davon abweicht. farbe.md / bewertung_bild.md nur als Quellenhinweis, alle Werte hier neu gemessen.

Werkzeuge (Scratchpad, nicht versioniert): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/A/`
`lade.py` (Modelle), `atlas.py` (Texel je Flaeche mit ihrer CLUT-Zeile, Mesh-Kennung je Texel), `zonen.py`, `messen.py` -> `messung.json`, `render.py` -> `render.json`,
`herkunft.py` -> `herkunft.json`, `bilder.py`. Parser: `re15_port/tools/emd_ansichtsblatt.py` (MD1/EMR/EDD/TIM), `form_emd.modell`, `birkin_bahnsteig.modell` (RE2), Raster `form_buehne.rastern`.

## 0. Modelle und Zonen

| Kurz | Datei | Zonen (Atlas-Texel, die eine Flaeche deckt) |
|---|---|---|
| EM30 | `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` (TIM = Sektion 8; Capcom-D0-Texel byte-gleich, birkin_formen.py Kopf) | Kopf = Mesh 9; Koerper = Rest; Haut = Koerper ohne Jeans/Stiefel (Mesh 0, 1, 3; Lab b* -6..-10) |
| G5 | `re15_port/shared_assets/RE2/CDEMD0.EMS` Typ 0x36 (EMD @0x6e6800, TIM @0x6c6000) | Maul = radiales Maul Atlas x 0-227, y 143-255; Augen x 450-497/196-241 und 168-189/95-116; Koerper = Rest |
| RE2-G4 | dieselbe EMS Typ 0x34 (EMD @0x677000, TIM @0x65e800) | Kopf/Maul = Mesh 26; Schulterauge x 270-321, y 160-245; Krallen/Knochen Mesh 4, 8, 14, 19, 22, 25; Koerper = Rest |
| Zombie | `re15_port/shared_assets/PSX/EMD/CDEMD0.EMS` Typ 0x10 (RE1.5) | Kopf = Mesh 14; Koerper = Rest (Kleidung eingeschlossen) |
| Hund | dieselbe, Typ 0x20 (RE1.5) | Kopf = Mesh 3 + Kiefer Mesh 4; Koerper = Rest |
| K4 | `varianten/K5/K4_heute/FORM.EMD/.TIM` | wie K5c |
| K5c | `varianten/K5/K5c/FORM.EMD/.TIM` | Koerper = Seiten 0-2 ohne Krallen (J_dicker/bau.json kralle_r/l, Seite 3 'kralle'); Kopf = Seite 3 'kopf' (384,0,128,104); Kiefer = 'kiefer' (384,106,128,72); Zahn (384,180,24,14); Auge (384,196,24,10) (`K_alligator/generator/k_form.py:11`) |

Lab = CIE-Lab D65 (OpenCV, RGB/255). Texelfarbe = 15-Bit-CLUT-Eintrag * 255/31. "Rot" (T7) = a* > 15, C* > 20, Farbton -40..70 Grad.

## 1. Stil der Textur (Texel, ohne Licht)

| Mass | EM30 | G5 | RE2-G4 | Zombie | Hund | K4 | K5c | Sicherheit |
|---|---|---|---|---|---|---|---|---|
| Texel Koerper | 40 365 | 54 473 | 42 623 | 25 112 | 11 036 | 63 663 | 63 663 | sicher |
| **T6 Chroma C\* Mittel / Streuung** | 13,1 / 8,4 (Haut 14,4) | **19,9** / 10,8 | 12,9 / 17,6 | 12,6 / 5,3 | 9,5 / 10,9 | 5,2 / 4,7 | **4,9 / 4,3** | sicher |
| Lab-Mittel Koerper | (34,3, 8,6, 2,5) | (28,8, 14,9, 12,3) | (19,0, 10,2, 4,3) | (29,8, 6,2, -8,8) | (8,0, 7,3, 5,4) | (23,7, 2,5, -3,1) | **(22,5, 2,6, -2,8)** | sicher |
| RGB-Mittel Koerper | (96,76,77) | (95,59,50) | (63,41,40) | (73,68,84) | (35,20,15) | (58,55,61) | **(56,53,58)** | sicher |
| Farbton der Familie | rot-rosa (a\* > b\* > 0) | rotbraun | rotschwarz | blaugrau (Kleidung) | braunrot | grau-violett | **grau-violett (a\* 2,6, b\* -2,8)** | sicher |
| **T7 Rotes Fleisch, Anteil Koerper** | 13,4 % (Haut 18,2 %) | **38,0 %** | 24,0 % | 2,0 % | 11,8 % | 0,5 % | **0,5 %** | sicher |
| T7 Rot-Cluster > 5 % / L\*-Spanne p5-p95 | 5 / 12,7-53,5 | 6 / 11,1-62,2 | 6 / 12,9-59,5 | 4 / 16,5-50,9 | 6 / 7,8-44,9 | 6 / 22,1-70,3 | 6 / 22,1-70,3, aber Mittel (47,29,-6) = Magenta-Rosa, kein Fleischrot | sicher |
| **T4 Glanz L\* > 85** (Koerper ohne Kopf/Zaehne/Augen/Krallen) | 0,02 % | 0,27 % | 0,41 % | 0,21 % | 0,00 % | 0,92 % | **0,79 %** | sicher |
| davon neutralweiss (C\* < 10) | 0,02 % | **0,00 %** | 0,22 % | 0,03 % | 0,00 % | 0,83 % | **0,78 %** (Texel (247,247,249)) | sicher |
| L\* Mittel / Streuung / p5-p95 | 34,3 / 20,4 / 5-69 | 28,8 / 19,4 / 0,5-62 | 19,0 / 15,0 / 5-50 | 29,8 / 11,0 / 13-46 | 8,0 / 9,3 / 0,5-31 | 23,7 / 13,9 / 8-49 | 22,5 / **12,5** / 8-44 | sicher |
| T2 Farben je Teil (Cluster > 5 % im Mesh, Mittel) | 3,4 | 5,3 | 3,6 | 3,9 | 4,6 | 4,4 | 4,5 | sicher (trennt nicht) |
| T3 Texeldichte, Texel/Einheit (Mittel Dreiecke / flaechengew.) | 0,067 / 0,057 | 0,040 / 0,034 | 0,050 / 0,043 | 0,099 / 0,086 | 0,074 / 0,067 | 0,042 / 0,030 | 0,042 / 0,030 (Koerper 0,035, Kopf 0,065, Kiefer 0,060) | sicher |
| T3 Texel ueber die Hoehe (Mittel x Hoehe Pose) | 270 (H 4022) | 98 (H 2489) | 113 (H 2263) | 293 (H 2943) | 113 (H 1531) | 133 (H 3149) | 133 (H 3149) | sicher |
| **T5 Hochfrequenz** (L\*, f > 0,25 / alles, je Atlasfeld) Koerper | 0,126 | 0,056 | 0,144 | 0,221 | 0,078 | 0,103 | 0,104 | sicher |
| T5 Kopf / Maul | 0,038 | 0,077 (Maul) | 0,155 | 0,260 | 0,067 | Kopf 0,131, Kiefer 0,466 | **Kopf 0,119, Kiefer 0,462** | sicher |
| T6 Farbtonstreuung (Texel mit C\* > 8) | 56 (mit Jeans), Haut 22 | 13 | 15 | 27 | 14 | 46 | 40 (nur 9 % der Texel haben C\* > 8) | wahrscheinlich |

Palettenfelder (T1) mit Anteilen: `farbe_lesung/A_paletten_lab.png`; Zahlen in `messung.json` (Scratchpad). Kurz:
- EM30 Haut: (12,6,0) 19 %, (19,24,12) 10 %, (31,10,4) 10 %, (37,38,21) 3 %, (44,18,7) 11 %, (45,8,6) 15 %, (56,11,7) 16 %, (69,7,7) 15 % - rosa Fleisch in fuenf Helligkeiten plus zwei gesaettigte Rote.
- G5: (2,4,1) 15 %, (14,13,9) 20 %, (16,26,19) 7 %, (29,15,12) 18 %, (32,37,29) 4 %, (43,16,14) 20 %, (56,18,15) 12 %, (71,8,28) 4 % - jede Stufe rotbraun.
- G4: (6,1,-2) 30 %, (15,3,-1) 36 %, (23,24,12) 8 %, (26,40,25) 4 %, (31,7,3) 7 %, (46,25,16) 9 %, (46,62,34) 4 %, (85,10,17) 1 % - Schwarzgrau mit kraeftigem Rot.
- **K5c Koerper: (11,3,-1) 24 %, (19,4,-5) 23 %, (20,1,1) 17 %, (26,3,-4) 19 %, (35,2,-4) 11 %, (47,29,-6) 0,6 %, (52,0,-4) 4 %, (92,1,-2) 1 %** - sechs Graustufen, ein Magenta-Rosa, ein Weiss.

Spektren: `farbe_lesung/A_spektren.png`. Die Familie faellt mit der Frequenz (gemalte grosse Formen + Korn). K5c Koerper liegt mitten in der Familie
(Rueckenstoff H2 ist RE2-Birkin-Hochpass, VARIANTEN.md Abschnitt 11). **Kopf und Kiefer von K5c sind flach ueber alle Frequenzen = weisses Rauschen
ohne Form**: Kiefer 0,46 der Energie ueber 0,25 (Familie 0,04-0,26), Kopf ohne die Gaumenstreifen ebenso flach.

## 2. Kiefer, Maul, Zaehne (T8)

| Teil | EM30 Kopf | G5 Maul | RE2-G4 Kopf | K4 Kiefer | K5c Kiefer | K5c Kopf | Sicherheit |
|---|---|---|---|---|---|---|---|
| Zahn (L\* > 60, C\* < 35): Anteil, RGB, Lab | 18 %, (195,170,166), (72,8,5) | 2 %, (204,141,132), (65,23,15) | 9 %, (190,158,136), (67,9,16) | 43 % | **77 %**, (176,152,132), (65,6,14) | 15 % (Gaumenstreifen) | sicher |
| Maul rot (a\* > 20): Anteil, RGB, Lab | 20 %, (144,57,54), (36,37,21) | 34 %, (124,56,46), (33,29,20) | 33 %, (154,60,61), (39,39,20) | 0 | **0** | 0 | sicher |
| Rest (Zahnfleisch, Haut): RGB, Lab | (78,62,55), (28,6,7) | (39,23,21), (10,8,5) | (67,52,51), (23,7,3) | (156,135,121) | (158,138,119), (59,5,13) | (57,52,57), (22,3,-2) | sicher |
| Zone gesamt: Lab, C\* Mittel / Streuung | (37,12,9), 17,5 / 14,2 | (18,15,10), 18,2 / 14,7 | (32,18,10), 22,0 / 18,8 | (60,6,12), 13,6 / 2,6 | **(63,5,14), 14,9 / 2,1** | (29,3,1), 6,2 / 5,1 | sicher |
| Zahnfeld eigens | - | - | - | (229,222,209) | (234,228,218) L 91 | - | sicher |
| Augenfeld | - | rot-gelb (Augapfel rot) | rot-gelb | (54,50,56) | **(207,95,161)**, Lab (56,52,-15) Magenta | - | sicher |

Die Familie malt Maeuler dunkel mit rotem Fleisch (20-34 %) und davon getrennten hellen Zaehnen (2-18 %). Der K5c-Kiefer ist eine Flaeche in
einer Farbe (C\* Streuung 2,1 gegen 14-19), ganz in Zahnfarbe, ohne Rot und ohne dunkle Tiefen; das Maulinnere (Gaumen) hat dieselbe Farbe.
Herkunft: `k_bau.malen` (`K_alligator/generator/k_bau.py:166-206`): Kiefer = Basis x Faktor 'unterseite' = 'fleisch' = (3,29, 3,12, 2,47)
(`K5c/bau.json` farben, Kiefer-Eichung gegen Bild 2), Struktur = Quilting der J-Brust, durch Gauss 10 entformt (`stufe3/gen/s3_n2_textur.py:813-850`).
Nutzerentscheid E21 hatte den "beigen Unterkiefer ohne Raumlicht" fuer K3 behalten.

## 3. Render-Gegenprobe (`farbe_lesung/A_render_gegenprobe.png`, `render.json`)

Selber Renderer (`form_buehne.rastern`, Gouraud, Textur x Licht / 255), selbes weisses Licht 0,35 + 0,65 max(0, N.L) aus oben links vorn,
gleiche Groesse (Umkugel auf 0,46 der Kachel), drei Winkel. EM30 Clip 0, K5c Clip 2 (stehen), G4 Clip 0 zur Einordnung. Gamma 0,5 nur auf die
Figurpixel, wie die alten Tafeln (`K5/generator/k5_blatt.py:263` Gamma 0,5 Rundum; `k5_rueckbild.py:65` Gamma 0,6 Bild-1-Kamera).

| Mittel ueber 3 Winkel | EM30 roh | EM30 Gamma 0,5 | G4 roh | G4 Gamma 0,5 | K5c roh | K5c Gamma 0,5 | Sicherheit |
|---|---|---|---|---|---|---|---|
| L\* | 22,4 | 45,7 | 15,7 | 38,6 | 16,2 | 40,7 | sicher |
| C\* | 9,4 | 9,9 | 9,6 | 9,8 | **4,0** | **4,5** | sicher |
| Anteil L\* > 85 | 0,00 % | 0,03 % | 0,06 % | 0,32 % | **0,70 %** | **0,93 %** | sicher |
| helle Flecken lokal (L\* > Umgebung + 15 und > 50) | 2,1 % | 2,7 % | 1,8 % | 2,4 % | 2,2 % | 3,2 % | sicher |

Was die Aufhellung tut: Gamma 0,5 hebt L\* bei allen um das 2,0-2,5-Fache und laesst C\* fast gleich. Bei EM30/G4 bleibt Rot sichtbar; bei K5c
(C\* 4) wird aus dunklem Grauviolett ein helles, fast unbuntes Grau mit weissen Spitzlichtern, cremefarbenem Maul und Magenta-Augen
(Spalten 4-6 der Tafel). Das ist der "Pokemon"-Eindruck des Rundumblatts: glatt-grau, Spielzeugfarben, keine Fleischtoene. Ohne Gamma
ist K5c gleich dunkel wie G4 (L\* 16), aber halb so bunt und das einzige Modell mit nennenswert weissen Pixeln.

## 4. Herkunft der K5c-Texel

| Atlasfeld | Herkunft (Datei) | Rueckprojektion? |
|---|---|---|
| Seiten 0-2 ausser Fussfeldern (x < 384; Huefte, Wirbel, Brust, Schwanz, Glieder, Haende, Hals) | `J_dicker/FORM.TIM`, Pixel und CLUT-Zeilen 0-2 bytegleich (Diff K5c/J: nur 4184 Texel in x 258-371, y 149-232; K4/J: 0) | ja, mittelbar (Kette unten) |
| Fussfelder fuss_r/fuss_l/zeh (x 258-371, y 149-232) | `K5/generator/k5_bau.py:92-113` fuss_textur: Farbton/Helligkeit des Unterschenkels, Index der Zeile 2 | nein (umgefaerbt aus J) |
| Seite 3 Kopf, Kiefer, Zahn, Kralle, Naht, Auge | `k5_bau.py:178-199` -> `k_bau.malen` mit `s3_n2_textur.haut_stoff`; Faktoren aus K4 (`K5c/bau.json` farben: haut 1, fleisch/unterseite (3,29, 3,12, 2,47), hell (4,97, 5,07, 4,30)); Auge auf (82,36,52) im Render geeicht -> Textur (200,93,157) | nein (gemalt: eine Farbe je Feld x entformte Brust-Koernung) |

Kette der Seiten 0-2 (BAU.txt je Ordner): `J_dicker` = `vm_bau.py --farbe C --treu --stoff h2 --g-textur J_dicker/_g/textur.png` (J_dicker/generator/BAU.txt)
= Farbkarte G_treu (`vm_treu.py`: Haut wie F = Projektion Schirmfarbe / Port-Licht, Bild 2 nur Helligkeit, Farbton aus Bild 1, unsichtbare Texel aus
3D-Farbfeld; Zeichnungen je Texel aus der Projektion, Helligkeit je Stelle am Screenshot geeicht) + H2-Stoff (RE2-Birkin-Hochpass, Staerke 1,15,
`gen/vm_stoff.py:25`). Laut `G_treu/bau.json` eichung: 25 666 von 76 890 Texeln direkt aus den Bildern.

Chroma der Kette (Texel, gemessen an `*/textur.png`): C_mitte 12,7 -> D_fleisch 18,0 -> **F_sauber 4,5** -> G_treu 4,5 -> H2 4,3 -> I 4,8 -> J 4,7.
Die Entsaettigung kam mit F (Farbton nur aus Bild 1, dessen zurueckgerechnete Farben fast unbunt sind; 3D-Feld). Zum Vergleich die eigene
Rueckrechnung `farbe_palette.json` (nur Quellenhinweis): Grundhaut (44,18,27) C\* 14,6, Rotbraun (103,63,84) C\* 21,4, Rosa (181,137,150) C\* 18,7 -
die Textur ist 3-4-mal weniger bunt als ihre eigene Quelle.

**Welche Texel Rueckprojektionen von Video-Hellstellen sind** (`herkunft.py`, J_dicker in seiner Pose/Lage aus `J_dicker/form.json`, Kamera
`kamera_bild1/2.json` ueber `form_pruefung.kamera` = R/t + Homographie B, UV-Raster mit Tiefenordnung; Bild `farbe_lesung/A_herkunft_weiss_rosa.png`):

| Messung | Bild 1 | Bild 2 | Sicherheit |
|---|---|---|---|
| Koerpertexel ohne Fuesse 56 882; in keinem Bild sichtbar | 74,4 % | | sicher |
| weisse Texel L\* > 85: 495, davon in Bild 1 oder 2 sichtbar | 421 = **85 %** (Koerper gesamt nur 25,6 %) | | sicher |
| Schirmpixel auf weissen Texeln | 264 | 1310 | sicher |
| Video-L\* dort / uebrige Figur | **41,1 / 19,6** | **39,2 / 16,4** | sicher |
| Rang dieser Videopixel in der Figur (Median) / Anteil ueber p95 | 0,83 / 26 % | 0,91 / 35 % | sicher |
| Video-RGB dort (bild1/2.png) | (108,98,98) | (137,73,93) rosa, C\* 31 | sicher |
| zurueckgerechnet (`farbe_bild1/2_echt.png`) dort | (48,41,30) | (61,47,45) | wahrscheinlich (Filtermodell farbe.md) |
| Texel dort | (248,248,250) = am Anschlag | (248,248,249) | sicher |
| Texel / zurueckgerechnete Schirmfarbe je Kanal | 5,1 / 6,0 / 8,3 | 4,0 / 5,2 / 5,5 | sicher |
| zum Vergleich: 1 / Raumlicht farbe.md (Textur = Schirm / Licht) | 2,1 / 2,3 / 2,7 | 2,6 / 2,9 / 3,3 | Quellenhinweis |
| Groesste Flecken (Schirm x, y, w, h) | (370,225,15,15) Ruecken; (323,283,8,10) Video (171,170,190) | (279,330,40,41) = Unterkiefer-Band Bild 2 (E20), auf J-Unterarm projiziert; (276,166,17,32); (397,206,23,24) | sicher |
| weisse Texel in beiden Bildern sichtbar: 47; davon in beiden Videos ueber p90 | 3 (34 in keinem) | | wahrscheinlich |

Befund: Die weissen K5c-Flecken sind **Rueckprojektionen der hellsten Videostellen, uebersteuert und entfaerbt**: Sie sitzen fast nur dort, wo ein
Bild die Figur zeigt, auf den hellsten 10-17 % der Figurpixel, sind 4-8-mal heller als die zurueckgerechnete Schirmfarbe (Lichtteilung erklaert nur
2-3-fach) und laufen an den Anschlag (248). In Bild 2 ist die Quelle rosa-rot (Feuerlicht, Unterkiefer-Band, das J noch als rechten Unterarm las), die
Textur ist neutralweiss (Farbton aus Bild 1). Ob die Videostelle eine Glanzstelle oder eine helle Zeichnung ist, entscheidet diese Messung nicht ganz
(von 47 in beiden Bildern sichtbaren weissen Texeln sind nur 3 in beiden Videos hell). Sicher ist: die Engine rechnet rein diffus (farbe.md Abschnitt 5,
`re15_light.h`), und der Weiss-Wert steht in keinem Bild so da. Die **rosa Punkte**: 593 Koerpertexel Lab (41,25,-7) RGB (131,81,110), 424 davon
sichtbar, in Bild 1 auf Videopixeln (122,66,105) (magenta umrandet in A_herkunft; Zuordnung zu den Stellen Z1/Xc nicht gemessen); dazu die
Augen auf Seite 3 (207,95,161).

## 5. Abweichungen K5c gegen die Spielfamilie

1. **Farbe fehlt (Hauptbefund).** K5c Koerper C\* 4,9 gegen 9,5-19,9 bei allen fuenf Spielmodellen (EM30-Haut 14,4, G5 19,9, G4 12,9). Lab-Mittel
   a\* 2,6 / b\* -2,8 (grau-violett) gegen a\* 6-15, b\* 2-12 bei der Birkin-Familie (rotbraun bis rosa). Sechs von acht Palettenclustern sind Grau.
2. **Kein Fleischrot.** Rot-Anteil 0,5 % (und das ist Magenta, Farbton -13 Grad) gegen 12-38 % bei EM30, Hund, G4, G5. Die Familie malt Fleisch
   in 5-6 Rotstufen von L\* 11-13 bis 54-62.
3. **Weisse Spitzlichter in der Textur.** 0,78 % neutralweisse Texel (247,247,249) gegen 0,00-0,22 % (G5 0,00); im neutralen Render 0,70 % der
   Figur ueber L\* 85, Familie 0,00-0,06 %. Herkunft: Rueckprojektion (Abschnitt 4).
4. **Kiefer und Kopf ohne Malerei.** Kiefer eine Farbe (172,149,129), C\*-Streuung 2,1 gegen 14-19, kein Rot, kein dunkles Maulinneres, ganz in
   Zahnfarbe; Spektrum flach (HF 0,46 gegen 0,04-0,26 der Familie) = Rauschen statt Form. Kopf grau (57,52,57), C\* 4,5, gleich flach. Die Familie
   malt Kopf/Maul als dunkles Zahnfleisch + Fleischrot 20-34 % + getrennte Zaehne 2-18 %.
5. **Magenta-Augen** (207,95,161) C\* 54: die buntesten Texel des Modells, im neutralen Licht rosa. Familie: rote Augaepfel mit gelber Iris (G4, G5).
6. **Weniger Helligkeitsumfang.** L\*-Streuung 12,5 (Familie 9-20; Birkin-Familie 15-20), p95 44 gegen 50-69: weniger helle gemalte Oberkanten.
7. **Nicht der Unterschied:** Texeldichte (133 Texel ueber die Hoehe, G4 113, G5 98), Farben je Teil (4,5 gegen 3,4-5,3) und Hochfrequenz des
   Koerpers (0,104 gegen 0,056-0,22). Der Rumpfstoff ist in der Koernung familienartig; was fehlt, ist die Farbe und die gemalte Grossform an Kopf/Kiefer.
8. **Die Tafeln haben den Eindruck verstaerkt.** Gamma 0,5 (Rundum) bzw. 0,6 (Bild-1-Kamera) machen aus C\* 4 ein helles unbuntes Grau; die Familie
   behaelt unter derselben Aufhellung ihr Rot. Jede neue Tafel: ohne Gamma oder EM30/G4 gleich behandelt daneben.

Bilder (`varianten/K5/farbe_lesung/`): `A_atlas_gleiche_texelskala.png`, `A_paletten_lab.png`, `A_render_gegenprobe.png`, `A_spektren.png`,
`A_herkunft_weiss_rosa.png`. (`B_*` stammen von Leser B.)

## 6. Offen

- E19 ("J ist die Form; Form und Textur werden nicht mehr veraendert") und E21 (beiger Unterkiefer bleibt) stehen gegen die heutige Ablehnung der
  Farben; ob die H2-Textur der Seiten 0-2 neu gefaerbt werden darf, entscheidet der Orchestrator/Nutzer.
- Glanz oder helle Zeichnung an den Videostellen: nicht ganz entschieden (Abschnitt 4, 3 von 47 konsistent hell).
- Pose der Rueckprojektion: J_dicker, nicht K5c (K5c Clip 1 hat Wirbel/Brust gedreht); die Texel der Seiten 0-2 sind J, also gilt J.
