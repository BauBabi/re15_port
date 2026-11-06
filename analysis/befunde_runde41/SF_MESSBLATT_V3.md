# SF Messblatt V3: Vorgabe Kopf (Augen, Maul) und grosser Schwanz der Sichel-Form, geprueft (2026-11-06)

Kein RE-Befund, keine @0x-Konstante: Die Sichel-Form gibt es auf der Disc nicht. Gestaltungsquelle ist die KI-Fassung
(Nutzerentscheid F4 2026-11-06, PORT-WAHL, in PSX-Machart). Anlass: Der Nutzer hat V1/V2 abgelehnt ("Die Augen sehen nicht
mal ansatzweise aus wie im Original ... Der grosse Schwanz auch nicht und hat komplett die falsche Farbe").
Der Ref2-Koerper (84e8a4d97, EM30_HANGAR.EMD sha256 cfd1ac42...) bleibt unveraendert; geaendert werden nur Kopf und Schwanz.

**Nachtrag 2026-11-06 21 Uhr:** Nutzer-Befund zu V3 Runde 2 (Leiste 05) -> Abschnitt 10 (Riegel N1-N27, gemessen in der
Spielkamera h118/h125). Wo Abschnitt 10 und die Abschnitte 1-9 auseinandergehen, gilt Abschnitt 10.

Grundlage: Leser A (`SF_LESER_V3_A.md`, Freisteller), Leser B (`SF_LESER_V3_B.md`, Videobilder). Der Pruefer hat die
entscheidenden Zahlen mit eigenem Code nachgerechnet (nicht mit den Leser-Skripten):

- `re15_port/tools/r41_sichel/v3/P_farbe.py` - eigene sRGB->Lab (D65) und CIEDE2000 (Selbsttest Sharma Paar 1: 2,0425).
- `P_kopf.py` - Kopfauge, Schulterauge (`auge`), Maul (`maul`) an den Freistellern; `auge` ist auch die Riegel-Messung am Port.
- `P_schwanz.py` - Schwanzmaske, Wurzel (Kerbe -> Naht), Mittellinie, Dicke, Glaette, Farbe, Muster; dieselben Funktionen fuer KI und Port.
- `P_port.py` - Port-Render Spielkamera 50A0/12 (b2.zeichne, RDT-Licht, `sfv2/posen_seite.json` h85/h92) mit den P_schwanz-Funktionen.
- `P_port_kopf.py` - Port "Kopf vorn nah" (EA.render, wie leisten_v2/A_port_v1) rendern und messen.
- `P_video.py` - Freisteller gegen eigenen ffmpeg-Export (RMS) und Original-Video mit den eigenen Masken.

Laufausgaben (JSON) in `analysis/befunde_runde41/SF_belege/v3_lesung/P_*.json` (Kopie aus
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/`).
Belegbilder: `P_1_kopf_KI_V1.png` (birkin19/20 x5 mit Auge gruen, Schulterauge cyan, Maul magenta; V1 Kopf nah),
`P_2_schwanz_KI_masken.png` (birkin11/12/13: Maske, Wurzelschnitt cyan, Mittellinie gelb, Dicken 0,1-0,9 gruen, Knicke orange,
Hautflaeche hellblau), `P_3_port_schwanz_V1_Ref2.png` (Port h85: Maske frei, Mittellinie ab Austritt, Dicken 0,2/0,5/0,9).

Konventionen: H = Oberkante Buckel bis Sohle im selben Bild. birkin12 (h85) H = 504 px (Bild 390x506), birkin19 (h118) 569 px
(346x571), birkin20 (h125) 599 px (414x601; Alpha-Bbox - 2 wie bei 19), birkin11 451 px, birkin13 505 px; Port-Spielkamera
640x480, H = 265 px; Port "Kopf vorn nah" 960x960 mit 0,9 px/E., H = 0,9 x 3865 E. = 3479 px. Koordinaten = Freisteller-Pixel
(+ Versatz aus SF_LESER_B.md Abschnitt 1 = 1280er Videobild). Zuordnung nachgeprueft: birkin11/12/13/19/20 gegen
eigenen Export n1433/1435/1442/1468/1475 RMS 0,00. Seiten: (W) = Sicht des Wesens, (B) = Sicht des Betrachters; von vorn
B-links = W-rechts (Sichelseite). Farben RGB-Mittel und Lab (D65), Flaechen 2 px erodiert; dE = CIEDE2000, wo nicht anders gesagt.

## 1. Augen und Kopf (Vorgabe je Mass)

Messung: `P_kopf.auge` = Minimum (Gauss 1 px) im Fenster; Haut = Median L* im Ring 6-9 px; dunkel = L* < (Lmin + Haut)/2;
Auge = konvexe Huelle der dunklen Komponente (der helle Kern liegt darin); Lid = 2-px-Ring, Haut = Ring 5-7 px um das Auge;
Pupille = dunkelste 25 % des Auges. Werte birkin19 (h118) / birkin20 (h125).

| # | Mass | Pruefer (eigene Rechnung) | A / B | Vorgabe + Toleranz |
|---|---|---|---|---|
| K1 | Augen auf dem Kopf | **1** (h118 Mitte (185,9 40,9), h125 (205,3 41,0)) | A 1 / B 1 | genau 1 |
| K2 | Schulterauge | **1**, Schulter W-rechts = B-links; Mitte h118 (120,0 70,0), h125 (133,4 75,7) | A ja / B ja | genau 1, W-rechts (Sichelseite) |
| K3 | Kopfauge Breite x Hoehe | 11 x 5 px = **0,019 x 0,009 H** / 13 x 7 px = **0,022 x 0,012 H**; Aequivalentdurchmesser 0,0125 / 0,0166 H; Schwelle 0,6 x Haut: gleiche Breite, Hoehe 6/7 px | A 0,019-0,024 x 0,010-0,013 / B Kern 0,012 aequiv. | Breite 0,017-0,025 H, Hoehe 0,008-0,014 H |
| K4 | Form | Mandel, B/H **2,2 / 1,86** | A 1,9 / B 2,0 | B/H 1,5-2,5, waagerecht |
| K5 | Lage | ueber der Maulmitte **0,052 / 0,053 H**; waagerecht 2,6 / 6,7 px = 0,005 / 0,011 H B-links der Maulmitte; in der Kopfbox (A-Boxen) x 0,33 / 0,37, y 0,35 / 0,32 | A 0,06 H / B 0,050-0,055 H | ueber Maulmitte 0,04-0,07 H; waagerechter Versatz zur Maulmitte <= 0,012 H |
| K6 | Pupille (dunkelste 25 %) | Lab [2,6 4,7 -0,6] RGB (18,6,11) / [4,7 4,3 -1,2] (22,13,17); L*/Haut 0,05 / 0,11 | A [5,4-6,3 ...] / B [5,8-7,3 ...] | L* Pupille / L* Haut <= 0,12; a* 2-8, b* -3..+2 |
| K7 | Auge gesamt | Lab [22,5 6,5 -1,3] (66,53,59) / [21,0 5,2 -1,2] (59,49,54); **L* Auge/Haut 0,43 / 0,47** | A 0,46-0,48 / B 0,43-0,45 | L* Auge/Haut 0,35-0,55 |
| K8 | Lidrand (2 px um das Auge) | Lab [40,4 7,1 -0,7] (107,92,97) / [42,1 7,1 -0,7]; L*/Haut 0,78 / 0,95; heller Anteil (L* > Haut+15) 0,00 / 0,01 | A [34-40 7 -1] / B [37-41 7 -0,5] | L* Lid/Haut 0,70-1,00; Anteil L* > Haut+15 im Lidring <= 0,10 (kein heller Ring) |
| K9 | Haut um das Auge (Ring 5-7 px) | Lab [51,9 7,6 0,8] (138,120,123) / [44,3 7,5 0,4] = helle grauviolette Kuppe | A [44-51 7,5 0-1] / B [44-47 7-8 1-2] | Kuppe heller als die Kopfhaut darunter (Beleg A/B Baender 0,04-0,08 H) |
| K10 | Weiss / Sclera | Anteil L* > 80: **0,025 / 0,00**; L* > Haut: 0,175 / 0,09; L* > Haut+15: 0,125 / 0,038 | A 0 (L*>80) / B 0,14 / 0,06 (L*>Haut+15) | L* > 80 <= 0,05; heller Anteil (L* > Haut) 0,05-0,25 und NUR als Kern |
| K11 | heller Kern (Glanz) | 7 px je Bild, L* max 82,4 / 77,7, Lab [71,1 5,7 -2,2] (183,171,178) / [60,1 1,8 -1,3] (147,144,147); Lage in der Augenbox x 0,50 / 0,59, y 0,70 / 0,66 (mittig-unten) | A 2-3 x 3-4 px, x 0,49-0,60 y 0,62-0,72 / B 3-6 px mittig | 1 heller Kern 2 x 3 px (KI-Massstab) = ~0,004 x 0,005 H, Lage x 0,4-0,7 / y 0,55-0,8 der Augenbox, grau, a* <= 7 |
| K12 | Schulterauge Lage | relativ zum Kopfauge **-0,116 / -0,120 H** waagerecht (B-links) und **+0,051 / +0,058 H** tiefer | A 0,11 H / 0,05 H; B 0,11 / 0,05 | -0,10..-0,13 H (B-links), 0,04-0,07 H tiefer |
| K13 | Schulterauge Groesse | Halbwert: 9 x 9 / 11 x 8 px = 0,016 x 0,016 / 0,018 x 0,013 H, aequiv. 0,015 / 0,0145 H; Schwelle 0,7 x Haut: 14 x 9-12 px = 0,023-0,025 H breit (weicher Rand) | A 0,021-0,025 x 0,013-0,019 / B aequiv. 0,020 ("groesser als Kopfauge") | Breite 0,015-0,025 H, Hoehe 0,012-0,020 H; Aequivalent 0,8-1,3 x Kopfauge (bei gleicher Halbwert-Schwelle) |
| K14 | Schulterauge Farbe | Auge Lab [12,4 5,5 2,5] / [11,7 3,6 2,3]; L* Auge/Haut 0,385 / 0,36; kein Glanz (heller Anteil 0,02 / 0,03, L* max 37 / 52) | A L 0,35-0,46 / B 0,42-0,44, kein Glanz | L* Auge/Haut 0,30-0,50; heller Anteil (L* > Haut) <= 0,05 |
| K15 | helle Kuppe / rote Zone | nicht nachgerechnet; A und B stimmen ueberein: helle Kuppe 0,04-0,08 H unter der Kopf-Oberkante (Anteil L* > 40: 0,20-0,26), rote Zone (Maul + Kinn) 0,10-0,25 H darunter (a* > 15: 0,39-0,55 / a* > 12: 0,48-0,80) | A = B | Kuppe heller als Haut um das Maul; Anteil a* > 15 im Band 0,10-0,25 H unter Kopf-OK (Kopfbreite) >= 0,35 |

## 2. Schwanz: Farbe und Muster (Vorgabe relativ zur PORT-Haut)

Gemeint ist der **grosse Bogen** der Seitenansicht h83-h92 (birkin11/12/13). Messung `P_schwanz.farbe`: Schwanzmaske 2 px
erodiert; **Haut = Figur ohne Schwanz im Band Oberkante bis +0,45 H, 2 px erodiert, ohne Jeans (b* < -3) und ohne L* < 5**.
Dieselbe Regel gilt am Port (Figur = Knochen 0-15, Schwanz = sichtbare Knochen 16-22).

| Mass | birkin12 h85 | birkin11 h83 | birkin13 h92 | Original n1435 / n1433 / n1442 (Schwarzwert 17 ab, eigene Masken halbiert) | V1 (Port h85 / h92) | Ref2 (h85) |
|---|---|---|---|---|---|---|
| Lab Schwanz | [18,6 6,3 1,4] RGB (55,43,44) | [18,5 6,1 1,5] | [18,4 7,2 2,3] | [13,7 4,3 2,1] / [13,3 4,2 2,0] / [13,4 5,4 2,6] | [23,8 21,0 8,9] RGB (90,43,45) / [23,1 20,6 8,7] | [25,7 22,2 9,6] |
| Lab Haut (Regel oben) | [29,4 9,7 2,8] RGB (87,65,67) | [30,2 10,1 3,0] | [26,5 8,8 2,1] | [26,3 9,5 4,3] / [27,5 9,9 4,4] / [23,5 8,4 3,4] | [28,4 13,8 7,1] RGB (90,58,57) | [28,2 14,2 7,2] |
| dE00 Schwanz-Haut | **8,5** (dE76 11,3) | 9,3 | 6,0 | 10,3 / 11,5 / 7,5 | **5,8** / 5,8 | 5,4 |
| L* Schwanz / Haut | **0,63** | 0,61 | 0,69 | 0,52 / 0,48 / 0,57 | **0,84** / 0,81 | 0,91 |
| C* Schwanz / Haut | **0,64** (6,5 / 10,1) | 0,60 | 0,83 | 0,46 / 0,43 / 0,66 | **1,47** (22,8 / 15,5) / 1,42 | 1,52 |
| Farbton Schwanz - Haut | -3,9 Grad (12,5 / 16,4) | -2,2 | +4,3 | +1,1 / +2,4 / +3,7 | -4,1 (22,9 / 27,0) | -3,6 |
| Anteil a* > 15 Schwanz / Haut | **0,058 / 0,149** | 0,056 / 0,168 | 0,112 / 0,144 | - | **0,874 / 0,368** | 0,924 / 0,392 |
| dE00 zum Ruecken (Band OK..+0,15 H) | 12,2 (dE76 15,9) | 12,8 | 10,8 | - | 9,4 | 7,5 |
| Muster: AK a* echtes Maximum 0,03-0,15 H (Port-Aufloesung H 265) | 0,13 bei 0,075 H | 0,05 | -0,03 | Bogen nur ~5 L* ueber dem Hintergrund (L* 6,5-8,7 daneben) | 0,16 / 0,13 bei 0,10 H | **0,51** / 0,61 bei 0,094-0,098 H (Ringe) |
| Muster: Rest-Std a* (Trend 0,05 H ab), H 265 | 1,21 | 1,34 | 1,62 | - | **3,95** / 3,70 | 1,96 |

Befund: Der Bogen ist **dieselbe Hautfamilie** (Farbton +-5 Grad), **dunkler** (L* 0,61-0,69 der Haut, Original 0,48-0,57)
und **matter** (C* 0,60-0,83, Original 0,43-0,66), mit **weniger** Rot als die Haut. KI-Bogen gegen Original-Bogen dE00
3,8-4,2 (P_video.json): die KI gibt die Original-Farbe wieder. Keine Querbaender (kein Autokorrelations-Maximum > 0,13),
feine Marmorierung (Rest-Std a* 1,2-1,6). V1 und Ref2 haben dagegen ein gesaettigtes Lachsrot, **bunter** als die eigene
Haut (C* 1,42-1,52) mit 84-92 % roten Bildpunkten; Ref2 traegt periodische Ringe (AK 0,51-0,61 bei 0,094-0,098 H), V1
unregelmaessige Ringe (Rest-Std a* 3,7-4,4 = 2,5-3 x KI).

**dE00 allein trennt nicht**: V1 hat dE00 5,8 zur Port-Haut, die KI 6,0-9,3 - der Riegel muss Helligkeit, Buntheit und Rotanteil
getrennt pruefen. Vorgabe (relativ zur Port-Haut Lab [28,4 13,8 7,1], C* 15,5, h 27 Grad): Schwanz L* 14-20, C* 7-14,
h 17-37 Grad, z. B. Lab [17,5 9,5 4,8]; Rotanteil (a* > 15) hoechstens 0,8 x Haut.

Laengszonen (nur Leser A, nicht nachgerechnet): Oberseite grauer, Unterseite rotbrauner (a* 7-8 gegen 4-5); B: Querprofil
einseitig hell (Licht von oben). Als Gestaltungshinweis, kein Riegel.

## 3. Dickenprofil

Messung `P_schwanz.form`: Wurzel = Strecke Kerbe -> Naht (Kerbe = hoechster Hintergrundpunkt der Luecke unter dem Bogen;
Naht = an der Oberkontur, von der Buckelmitte nach hinten, der Uebergang helle Rueckenhaut -> dunkle Schwanzhaut, mittleres
L* 4-14 px unter der Kante faellt nach >= 40 unter 35); Mittellinie = Schwerpunkte je 2-px-Band des geodaetischen Abstands
von der Wurzel, Gauss 0,01 H; Dicke = Maskenstueck auf der Normalen; s = Anteil der Mittellinienlaenge.
Am Port: s0 = erster Mittellinienpunkt (Wurzel = Gelenk 16), an dem im vollen Render ein Schwanzknochen sichtbar ist.

| s | h85 | h83 | h92 | KI in Port-Aufloesung (H 265) h85/h83/h92 | A h85 | B h85 | V1 h85 / h92 | Ref2 h85 | **Vorgabe** |
|---|---|---|---|---|---|---|---|---|---|
| 0,1 | 0,189 | 0,205 | 0,182 | 0,185 / 0,194 / 0,172 | 0,099 | 0,204 | 0,132 / 0,145 | 0,066 | (Wurzelnaehe, Richtwert 0,17-0,21) |
| 0,2 | **0,179** | 0,195 | 0,163 | 0,168 / 0,175 / 0,155 | 0,171 | 0,149 | **0,140** / 0,136 | 0,055 | **0,15-0,20 H** |
| 0,3 | 0,146 | 0,157 | 0,137 | 0,134 / 0,147 / 0,130 | 0,160 | 0,123 | 0,128 / 0,136 | 0,055 | 0,12-0,17 H |
| 0,5 | **0,088** | 0,100 | 0,079 | 0,083 / 0,092 / 0,070 | 0,100 | 0,079 | **0,083** / 0,079 | 0,051 | **0,07-0,10 H** |
| 0,7 | 0,049 | 0,061 | 0,046 | 0,043 / 0,055 / 0,038 | 0,056 | 0,054 | 0,047 / 0,047 | 0,047 | 0,035-0,065 H |
| 0,8 | 0,066 | 0,084 | 0,061 | 0,060 / 0,074 / 0,047 | - | - | 0,034 / 0,030 | 0,034 | Hand/Kralle: 0,06-0,085; spitz: entfaellt |
| 0,9 | **0,037** | 0,039 | 0,030 | 0,032 / 0,030 / 0,032 | 0,043 | 0,030 | **0,025** / 0,025 | 0,036 | **0,028-0,045 H** |
| Laenge ab Naht (Bild) | 0,77 | 0,87 | 0,64 | 0,76 / 0,86 / 0,62 | 0,85 | 0,70 | 0,60 / 0,64 | 0,67 | Richtwert h85 0,65-0,90 (posenabhaengig, kein Riegel) |

Verlauf: dickste Stelle bei s 0,1-0,2 (0,18-0,21 H = 1,6-1,9 x Oberarm 0,10-0,11 H nach A), danach stetig duenner bis s 0,7;
in der KI schwillt das Ende bei s 0,75-0,85 zur Hand (0,06-0,08 H) und laeuft in eine Kralle aus (0,9: 0,03-0,04 H).
Von der Spitze aus gemessen (h85/h83/h92): 0,05 H vor der Spitze 0,028 / 0,037 / 0,025 H.

## 4. Glaette (Knicke, Winkel)

Tangente = Sehne +-0,01 H; Winkelaenderung je 0,05 H = |theta(s+0,025 H) - theta(s-0,025 H)|; Knick = zusammenhaengender
Abschnitt > 20 Grad (Abschnitte < 0,03 H Abstand verschmolzen, Rand 0,025 H ausgenommen).

| Mass | h85 | h83 | h92 | KI H 265 (h85/h83/h92) | A (h85/h83/h92) | B h85 | V1 h85 / h92 | V1 Kralle h85 | Ref2 h85 | **Vorgabe** |
|---|---|---|---|---|---|---|---|---|---|---|
| max. Winkelaenderung je 0,05 H | **28,3** bei s 0,26 | 28,4 | 32,9 | 30,9 / 28,5 / 32,6 | 36 / 29 / 33 | 35,1 | **38,5** / 40,7 | 84,1 (s 0,95) | 11,2 | **<= 36 Grad** |
| Knicke > 20 Grad | **2** (s 0,26 / 0,87) | 4 (0,23 / 0,40 / 0,70 / 0,86) | 2 (0,43 / 0,85) | 2 / 3 / 1 | 3 / 2 / 3 | 3 | **5** / 4 | 6 | 0 | **<= 3** |
| Gesamtdrehung | 124 Grad | 118 | 83 | - | ~95 | 118 | 53 / 77 | 112 | 29 | Richtwert 80-125 Grad (posenabhaengig) |

Biegungen der Vorlage: Wurzelbogen (s 0,23-0,30), weite Biegung um s 0,4-0,45, Handgelenk s 0,85-0,87. V1 hat bei s 0,56-0,82
drei enge Knicke (30-41 Grad) - der Haken, den der Nutzer sieht.

## 5. Ende und Wurzel

| Mass | Pruefer h85 / h83 / h92 | A | B | **Vorgabe** |
|---|---|---|---|---|
| Kerbe unter OK | 0,220 / 0,246 / 0,222 H ((163,112) / (150,111) / (164,112)) | 0,22 | (712,481) = Freisteller (165,115) | 0,20-0,25 H |
| Naht (Oberkontur) unter OK | 0,018 / 0,020 / 0,022 H ((214,10) / (210,9) / (206,11)) | 0,02-0,03 ((212,11)) | nicht gesucht: Schnitt endet bei (138,21) = auf der Schwanz-Oberkante 0,15 H hinter der Naht | Oberkontur laeuft ohne Stufe vom Buckel in den Schwanz |
| Wurzelschnitt Laenge | 0,228 / 0,264 / 0,218 H | 0,20-0,26 | 0,19 | 0,20-0,27 H (= Wurzeldicke) |
| Wurzelmitte unter OK | **0,119 / 0,133 / 0,122 H** | 0,12 | 0,13 | **0,10-0,16 H** (Port: Austrittspunkt s0) |
| Wurzelmitte hinter Buckelmitte | 0,146 / 0,190 / 0,120 H | - | 0,19 | Richtwert 0,12-0,20 H |
| Austrittsrichtung (Mittellinie erste 0,05 H) | 171 / 172 / 171 Grad = 8-9 Grad unter der Waagerechten nach hinten; bis s 0,2: 160-172 Grad | 150 / 152 / 139 Grad (30-40 Grad abwaerts) | 175-177 Grad | Richtwert 5-30 Grad abwaerts (von der Wurzeldefinition abhaengig, kein Riegel) |
| Ende | KI-Seite: Hand bei s 0,75-0,85 (Verdickung 0,06-0,08 H), danach Kralle; Halteszene h61-h73: duennes rotes Glied mit Klinge | wie Pruefer | wie Pruefer | **offen (Nutzerwahl, F2): spitz oder Hand/Kralle** |

Port V1 (s0): Austritt 0,136 / 0,115 H unter OK (h85 / h92) - erfuellt; Ref2 0,292 H.

## 6. Maul

Messung `P_kopf.maul`: Zahnflecken = Top-Hat (L* - Oeffnung 5x5) > 15 und L* > 40, 1-12 px, mit einem Bildpunkt L* < 20 im
3-px-Umkreis (Zahn am dunklen Maulinneren); Fenster Kopfbreite +-4 px, Augenmitte + 0,028 H bis + 0,08 H; Maul = konvexe
Huelle der Zahnflecken; innen = Huelle ohne Zahnflecken mit L* < 30.

| Mass | Pruefer h118 / h125 | A | B | **Vorgabe** |
|---|---|---|---|---|
| Breite | 48 px = **0,084 H** (x 165-212) / 55 px = **0,092 H** (x 185-239) | 0,079-0,088 | 0,070-0,072 | **0,075-0,095 H** |
| Hoehe | 24 px = 0,042 H / 29 px = 0,048 H | 0,046-0,058 | 0,040-0,042 | **0,038-0,055 H** |
| Form B/H | **2,0 / 1,9**, waagerecht, leicht schraeg | 1,5-1,8 | 1,7 | **1,5-2,3** (waagerecht) |
| Breite / Kopfbreite (A-Boxen 60 / 58 px) | 0,80 / 0,95 | 0,83-0,88 | 0,61 | 0,75-0,95 |
| Mitte unter Augenmitte | 0,052 / 0,053 H | 0,06 | 0,050-0,055 | 0,04-0,07 H |
| innen | Lab [13,5 13,7 4,8] RGB (55,27,28) / [13,5 13,7 4,9]; L* innen / Kuppe 0,26 / 0,30 | [11-12 11-13 4-5] | [16,8 13,9 4,6] | dunkelrot: L* innen / Haut um das Auge 0,20-0,40, a* innen - a* Haut >= +4 |
| Zaehne | 17 / 18 Flecken (oben 11 / 10, unten 6 / 8), je ~3 px; Lab [50,0 17,0 4,4] / [49,4 17,0 4,9] (Fleckmittel inkl. Rand) | 6-9 oben / 2-5 unten; [57-62 15-17 4-5] | 5-6 / 3-6; [58,9 15,6 4,9] | viele kleine Spitzen: oben >= 5, unten >= 3, Einzelzahn <= 0,005 H breit; rosa-grau, L* Zahn <= Haut+20, Anteil L* > 80 <= 0,02, a* 12-20 |

## 7. Port-V1-Ist und Abweichungen (warum der Nutzer ablehnte)

Port V1 = EM30_V1_SPITZ.EMD (scratchpad sfv2/emd), Kopf aus eigenem Render "Kopf vorn nah" (P_port_kopf.json), Schwanz aus
P_port.json; Kopfauge zusaetzlich auf KI-Massstab verkleinert (P_auge_riegel.json).

1. **Augenbild**: V1 rund (B/H 0,92, 0,021 x 0,023 H) mit **hellem Ring** (Lab [54,9 3,8 5,0], 39-44 % des Auges heller als Haut+15)
   und Kreuz; KI Mandel B/H 1,9-2,2, 0,019-0,022 x 0,009-0,012 H, nur ein 7-px-Kern heller (0,04-0,13 ueber Haut+15).
   Auf KI-Massstab misst die Halbwert-Maske bei V1 nur die Pupille (0,007 H) und der Lidring ist zu **94 % hell**
   (KI 0-1 %), L* Lid/Haut 1,81 (KI 0,78-0,95).
2. **Augenhelligkeit**: V1 L* Auge/Haut 0,89-0,92 (Ring + Pupille), KI 0,43-0,47; die Haut um das V1-Auge ist dunkler
   (L* 31,5) als der Ring (54,9) - in der KI ist es umgekehrt (Kuppe L* 44-52 um ein dunkles Auge). Kontrast invertiert.
3. **Schulterauge**: V1 Ringauge 0,0195 x 0,0233 H, L* Auge/Haut 1,30, 47 % hell; KI dunkel (0,36-0,39), ohne Glanz.
4. **Maul**: V1 senkrechtes Oval 0,038 x 0,047 H, B/H **0,79** (dunkle Flaeche + Zaehne, Nahansicht), mit zwei grossen hellen
   Zahnboegen (Lab [55,1 7,1 6,8], 5 Teile); KI waagerecht 0,084-0,092 x 0,042-0,048 H, B/H 1,9-2,0, 17-18 kleine Zahnflecken.
   Breite nur 0,41-0,45 der Vorlage.
5. **Schwanzfarbe**: V1 Lab [23,8 21,0 8,9] = Lachsrot; C*/Haut **1,47** (Vorgabe 0,45-0,90), L*/Haut 0,84 (Vorgabe 0,50-0,72),
   Rotanteil 0,87 gegen Haut 0,37 (Vorgabe <= 0,8 x Haut). dE00 zur Haut 5,8 liegt dabei im KI-Bereich - deshalb der
   getrennte Riegel. V1 hat die Farbe des roten Glieds der Halteszene (A: dE00 1,6-2,9), nicht des grossen Bogens.
6. **Muster**: V1 Rest-Std a* 3,7-4,0 (KI 1,2-1,6) = unruhige Querringe; Ref2 periodische Ringe (AK 0,51-0,61).
7. **Dicke**: V1 bei s 0,2 0,136-0,140 H (Vorgabe 0,15-0,20), bei 0,9 0,025 (Vorgabe 0,028-0,045); die Wurzel ist zu duenn,
   das Ende zu spitz-duenn.
8. **Glaette**: V1 4-5 Knicke, max 38,5-40,7 Grad je 0,05 H (Vorgabe <= 3, <= 36); Kralle-Variante 84 Grad am Ende.
9. **Machart**: Schwanz 720 Dreiecke (7 Rohrglieder x 100-110) = 0,71 x der ganze uebrige Koerper (1017); Texeldichte
   Schwanz 0,086-0,115 Texel/E. (Haut-Meshes EM30 0,046-0,087); eigene CLUT-Zeile 486 mit Rot (Dossier SV2).
10. Erfuellt: Augenzahl 1 + 1 Schulterauge, Schulterseite, Augenlage ueber dem Maul (0,065 H), Wurzelhoehe (0,115-0,136 H).

## 8. RIEGEL-TABELLE

**Korrektur (Nachtrag):** R1-R14 werden in der neutral beleuchteten Nahansicht gemessen. V3 Runde 2 bestand dort alle
(riegel_V3.txt, 0 rot), der Nutzer lehnte den Kopf trotzdem ab. Notwendig, aber nicht hinreichend: Fuer Kopf, Auge, Haare
und Maul gehen die Riegel N1-N27 aus Abschnitt 10 vor. Sie werden in der Spielkamera h118/h125 gemessen, also dort, wo
der Nutzer hinsieht.

Werkzeuge: `re15_port/tools/r41_sichel/v3/P_port.py <EMD>` (Schwanz, Spielkamera, Posen sfv2/posen_seite.json h85 und h92,
beide muessen bestehen) und `P_port_kopf.py <EMD>` (Kopf vorn nah) plus `P_kopf.auge` auf der auf H = 569 px verkleinerten
Nahansicht (Faktor 569/3479, INTER_AREA; Fenster +-12 px um die Augenmitte der Textur). Ein Bauer und ein Pruefer, die
dieselbe EMD mit diesen Skripten messen, bekommen dieselben Zahlen (deterministisch, kein Zufall, keine Handgrenzen ausser
den angegebenen Fenstern).

| Mass | Vorgabe | Toleranz | Messmethode am Port-Render |
|---|---|---|---|
| R1 Kopfaugen | 1 | genau 1 | Nahansicht: Anzahl dunkler Augenflecken (P_kopf.auge-Komponenten mit L* Auge/Haut < 0,6) auf Mesh 9 |
| R2 Schulterauge | 1, W-rechts (B-links von vorn) | genau 1; -0,10..-0,13 H waagerecht, +0,04..+0,07 H tiefer als das Kopfauge | Nahansicht: Mitte Schulterauge minus Mitte Kopfauge, in H (3479 px) |
| R3 Kopfauge Groesse | 0,020 x 0,0105 H | Breite 0,017-0,025, Hoehe 0,008-0,014 H | P_kopf.auge auf Nahansicht H 569 px: b_H, h_H |
| R4 Kopfauge Form | B/H 2,0 | 1,5-2,5 | P_kopf.auge: bh |
| R5 Kopfauge Helligkeit | L* Auge/Haut 0,45 | 0,35-0,55 | P_kopf.auge: L_auge_haut |
| R6 kein heller Ring | Lidring hell 0,00 | lid_hell <= 0,10; L_lid_haut 0,70-1,00 | P_kopf.auge: lid_hell, L_lid_haut |
| R7 keine Sclera, ein Glanzkern | L* > 80: 0; heller Kern 0,13 | anteil_L80 <= 0,05; anteil_heller_haut 0,05-0,25; glanz_rel_box x 0,4-0,7, y 0,55-0,8 | P_kopf.auge: anteil_L80, anteil_heller_haut, glanz_rel_box |
| R8 Pupille | L* Pupille/Haut 0,08 | <= 0,12 | P_kopf.auge: L_pup_haut |
| R9 Schulterauge | dunkel, ohne Glanz, so gross wie Kopfauge | L* Auge/Haut 0,30-0,50; anteil_heller_haut <= 0,05; aequiv_H 0,8-1,3 x Kopfauge | P_kopf.auge (Fenster +-12 px um Schulterauge) |
| R10 Maul Groesse | 0,088 x 0,045 H | Breite 0,075-0,095, Hoehe 0,038-0,055 H | Nahansicht (eigener Massstab): P_port_kopf.maul_port = dunkle Maulflaeche (L* < 0,5 x Kopfhaut-Median) + angrenzende Zahnpixel, Bbox / 3479 |
| R11 Maul Form | B/H 1,95, waagerecht | 1,5-2,3 | wie R10: bh |
| R12 Maul Lage | 0,052 H unter der Augenmitte | 0,04-0,07 H; waagerecht <= 0,012 H Versatz | Bbox-Mitte R10 minus Augenmitte R3 |
| R13 Maul innen | dunkelrot | L* innen / L* Haut am Auge 0,20-0,40; a* innen - a* Haut >= +4 | maul_port lab_innen gegen Haut-Ring (Ring 0,0105-0,0158 H um das Auge) |
| R14 Zaehne | viele kleine, rosa-grau | Zahnteile oben >= 5, unten >= 3; Einzelteil <= 0,005 H breit; L* Zahn <= Haut+20; a* 12-20 | maul_port: Zahnpixel-Komponenten (8er), oben/unten nach Bbox-Mitte |
| R15 Schwanz Helligkeit | L* Schwanz/Haut 0,63 | 0,50-0,72 | P_port.py: farbe.L_verh (h85 und h92) |
| R16 Schwanz Buntheit | C* Schwanz/Haut 0,65 | 0,45-0,90 | P_port.py: farbe.C_verh |
| R17 Schwanz Farbton | gleich Haut | abs(dh) <= 10 Grad | P_port.py: farbe.dh |
| R18 Schwanz Rot | weniger rot als Haut | rot_anteil_schwanz <= 0,8 x rot_anteil_haut | P_port.py: farbe.rot_anteil_* (a* > 15) |
| R19 Schwanz dE00 zur Port-Haut | 8,5 | 6-11 (zusammen mit R15-R18, allein nicht trennscharf) | P_port.py: farbe.de00_haut |
| R20 Muster | keine Querbaender, feine Marmorierung | periode_a_ak <= 0,25; rest_std_a <= 2,0 | P_port.py: muster (Schwanz frei, H 265) |
| R21 Dicke s 0,2 | 0,175 H | 0,15-0,20 H | P_port.py: form.dicke_H['0.2'] (ab Austritt s0) |
| R22 Dicke s 0,5 | 0,085 H | 0,07-0,10 H | form.dicke_H['0.5'] |
| R23 Dicke s 0,9 | 0,035 H | 0,028-0,045 H | form.dicke_H['0.9'] |
| R24 Verlauf | stetig duenner s 0,2 -> 0,7 | breiten_005_095 von s 0,2 bis 0,7: kein Anstieg > 0,01 H; Hand/Kralle: Anstieg erst ab s 0,72, Spitze 0,8 <= 0,085 H | form.breiten_005_095 |
| R25 Glaette | max 28-33 Grad je 0,05 H | <= 36 Grad | form.max_dwinkel_005H |
| R26 Knicke | 2 | <= 3 (> 20 Grad je 0,05 H) | form.knicke |
| R27 Austritt | 0,12 H unter OK | 0,10-0,16 H | P_port.py: austritt_unter_OK_H |
| R28 Koerper unveraendert | Ref2 | Meshes 0-15, Skelett, EDD byte-gleich zu EM30_HANGAR.EMD cfd1ac42 ausser Kopftextur (Mesh 9 UV/CLUT/Texel) und Schulterauge (<= 1 Viereck auf Mesh 8) | Byte-Vergleich je Mesh (birkin_formen MD1-Serializer) |
| R29 Dreiecke Schwanz | Richtwert EM30 Arm rechts 13-15: 212 (Arm links 190) | <= 212 | Summe Dreiecke (Viereck = 2) Meshes 16-22 |
| R30 Dreiecke Kopf | EM30 Kopf 109; RE2-G4 Kiefer 118 | Mesh 9 <= 118; Schulterauge <= 2 | Dreiecke Mesh 9 / neue auf Mesh 8 |
| R31 Texeldichte | wie Haut | Schwanz 0,045-0,090 Texel/E. (EM30-Haut-Meshes 0,046-0,087); Kopfbild 0,05-0,091 | sqrt(Texelflaeche / Modellflaeche) je Mesh |
| R32 Farbtiefe | 8 bpp, Haut-Palette | 8-bpp-TIM; Schwanztexel aus der Haut-CLUT (keine eigene Rot-Zeile); max C* der Schwanz-Texel <= max C* der Haut-Texel; kein Weichzeichner (Texel 1:1, keine Zwischenfarben ausserhalb der CLUT) | TIM-Kopf und CLUT-Zeilen der Schwanz-UVs |

Gegenprobe der Riegel (eigene Messung, P_port.json / P_port_kopf.json / P_auge_riegel.json): V1 SPITZ faellt bei R3 (0,007 H),
R4 (1,0), R5 (0,28), R6 (lid_hell 0,94), R7 (heller Anteil 0,00), R9 (L* 1,30), R10 (0,038 H breit), R11 (0,79), R14 (5 Teile),
R15 (0,84), R16 (1,47), R18 (0,87 gegen 0,37), R19 (5,8), R20 (rest_std_a 3,95), R21 (0,140), R23 (0,025), R24 (Sprung
0,030 -> 0,062 bei s 0,6), R25 (38,5), R26 (5), R29 (720), R31 (bis 0,115), R32 (CLUT-Zeile 486); Ref2 faellt bei R15 (0,91),
R16 (1,52), R18, R19 (5,4), R20 (periode 0,51), R21 (0,055), R22 (0,051), R27 (0,292), R29, R31, R32. Die KI-Bilder erfuellen
R3-R8 (KI-Massstab) und R15-R27 (R20/R25/R26 in Port-Aufloesung H 265: Knicke 2/3/1; in voller Aufloesung hat h83 4 knappe
Knicke von 20,2-28,4 Grad, deshalb wird R26 in Port-Aufloesung gemessen - das tut P_port.py ohnehin).

## 9. Offen

1. **Ende des Bogens (einzige Gestaltungsfrage an den Nutzer, = F2):** KI-Seitenansicht h83-h98 Hand mit Fingern und roter
   Kralle (Verdickung 0,06-0,08 H bei s 0,75-0,85); Halteszene h61-h73 duennes rotes Glied mit spitzer Klinge. Das Original loest
   es nicht auf. R24 hat fuer beide Enden eine Fassung.
2. Rotes Glied der Halteszene h61-h73 (KI und Original rot, a* 21-25): ob es ein zweites Teil ist, entscheidet keine Quelle.
   Nach der Nutzeraussage ("der grosse Schwanz ... falsche Farbe") gilt fuer den einen Port-Schwanz die Bogenfarbe; das rote
   Glied wird nicht nachgebaut. Keine Nutzerfrage noetig.
3. Heller Augenkern: Glanzlicht oder helle Iris ist bei 7 px nicht trennbar; Vorgabe ist nur Lage/Groesse/Helligkeit (R7).
4. Zahnzahl ist eine Fleckenzaehlung (Pruefer 17-18, A 8-14, B 8-12 je nach Fleckkriterium); Riegel nur als Mindestzahl.
5. Austrittsrichtung der Wurzel haengt an der Wurzeldefinition (Pruefer 8-9 Grad, A 30-40 Grad, B 5-10 Grad abwaerts): kein Riegel.
6. Laenge und Gesamtdrehung sind Projektionen der Pose (Port-Pose IoU 0,62 zur KI): nur Richtwerte.
7. Original-Bogen nur ~5 L* ueber dem Hintergrund daneben (L* 13,3-13,7 gegen 6,5-8,7): Original-Farbwerte "wahrscheinlich".
8. Kuppe/rote Zone (K15) und Laengszonen des Schwanzes (A) sind nicht vom Pruefer nachgerechnet; A und B stimmen ueberein.
9. R10-R14 messen am Port die dunkle Maulflaeche, an der KI die Zahnhuelle (das dunkle KI-Maul ist gegen den Kopfschatten
   nicht trennbar); beide meinen die Maeuloeffnung mit Zaehnen.

## Anhang: Abweichungen A/B (> 15 % oder in der Aussage) und Entscheidung

| Mass | A | B | Pruefer | Entscheidung |
|---|---|---|---|---|
| Auge "Durchmesser" | 0,022 H (Mandelbreite) | 0,012 H (Aequivalent des Kerns) | Breite 0,019-0,022, Hoehe 0,009-0,012, Aequivalent 0,0125-0,0166 H | Definitionsunterschied; Vorgabe als Breite x Hoehe (R3) |
| Auge "Ring" | RGB (68,56,62) = Mittelton im Auge | RGB (110,95,100) = Lidrand | Auge (66,53,59)/(59,49,54); Lid (107,92,97)/(111,96,101) | beide richtig, verschiedene Teile; es gibt keinen hellen Irisring (R6) |
| Weissanteil | 0 (L* > 80) | 0,14 / 0,06 (L* > Haut+15) | L* > 80: 0,025 / 0; > Haut+15: 0,125 / 0,038 | Kriterien verschieden; keine Sclera, nur ein Kern (R7) |
| Schulterauge Groesse | 0,021-0,025 H breit | aequiv. 0,020 H, "groesser als Kopfauge" | Halbwert 0,016-0,018 H breit, aequiv. 0,0145-0,015 (Kopf 0,0125-0,0166); Schwelle 0,7: 0,023-0,025 | Schwellenabhaengig (weicher Rand); gleich gross wie das Kopfauge (R9); B's "groesser" ist ein Schwellen-Effekt |
| Maul Breite | 0,079-0,088 H | 0,070-0,072 H | 0,084 / 0,092 H | A; B's Huelle (Freisteller x 168-207) laesst die Eckzaehne bei x 165-167 und 208-212 aus |
| Maul Hoehe / Form | 0,046-0,058 / B/H 1,5-1,8 | 0,040-0,042 / 1,7 | 0,042-0,048 / 1,9-2,0 | dazwischen; Vorgabe 0,038-0,055 H, B/H 1,5-2,3 |
| Schwanz dE zur Haut | dE00 8,2 (Haut = Band OK..0,45 H) | 23,9 = dE76 gegen hellen Ruecken-Fleck [40,3 12,7 2,2] | dE00 8,5 / 9,3 / 6,0; gegen Ruecken-Band dE00 12,2, dE76 15,9 | A's Bezug (ganze Haut, lichtrobust); B's Zahl ist dE76 gegen ein Glanzlicht. In der Aussage einig (gleicher Farbton, dunkler, matter) |
| Dicke s 0,1 | 0,099 H | 0,204 H | 0,189 H | B; A's Normale wird an der schraegen Wurzelstrecke abgeschnitten |
| Dicke s 0,2 / 0,9 | 0,171 / 0,043 | 0,149 / 0,030 | 0,179 / 0,037 | dazwischen, naeher A bei 0,2; Vorgabe 0,15-0,20 / 0,028-0,045 |
| Wurzel oben | Naht (212,11), 0,02 H unter OK | Schnitt bis (138,21) | Naht (214,10) per L*-Uebergang | A: die dunkle Schwanzhaut reicht sichtbar bis (212,11) (P_2, Lupe); B's Punkt liegt auf der Schwanz-Oberkante |
| Austrittsrichtung | 30-40 Grad abwaerts | 5-10 Grad (waagerecht) | 8-9 Grad | naeher B, aber definitionsabhaengig -> kein Riegel |
| Max. Winkel je 0,05 H | 36 | 35,1 | 28,3 (h85), 28-33 (alle) | glaettungsabhaengig (-20 %); Riegel <= 36 Grad |
| V1 Augenzahl | 1 (Kopf) + 1 Schulter | 2 (gesamt) | 1 + 1 | gleiche Aussage, verschieden gezaehlt |
| V1 Schwanz dE zur Haut | 5,5 | 13,94 (gegen Flanke) | 5,8 | A; deshalb R15-R18 getrennt |
| V1 Knicke | 2-5 | 2 | 4-5 (h85 5, h92 4), max 38,5-40,7 | A |

## 10. Nachtrag Nutzer-Befund 2026-11-06 20:35-20:50 (BINDEND, geht jeder Zeile oben vor)

Anlass: Der Nutzer hat Leiste `SF_belege/v3/05_kopf_nah.jpg` gesehen (Stand V3 Runde 2, 6447782e6; Spalten KI | Original |
Ref2 | V1 | V3 r2). Er sagte: (1) "2 aufgemalte Augen und offener Mund ... komplett kacke", (3) "nicht so komische runde aufgemalte
Augen ... schon so, wie in der Vorlage", (4) "Am ehesten die Version rechts. Aber das Auge ist weiter links und rechts ist noch
etwas 'haare' drueber", (5) "der Mund sieht nicht so Comic Haft 100% oval aus".

Lesung, nachgemessen: "die Version rechts" = Spalte 5 = **V3 r2 ist die Basis**. Koerper und Schwanz bleiben (Sicht-Pruefung r2:
K1/K2; Riegel R15-R32 gruen). Die zwei Ringaugen mit dem offenen Oval sind die V1-Spalte (Ringauge B/H 1,00, Maul B/H 0,94).
Deshalb haben Nutzer-Leisten ab jetzt die Spalten **KI | Original | V3 r2 (vorher) | V3 neu**. V1 und Ref2 kommen nur noch in eine
eigene Gegenprobe-Datei (Riegel N26).

Messung (eigener Code, gleiche Funktionen fuer KI und Port):
- `re15_port/tools/r41_sichel/v3/N_render.py EMD NAME`: Port-Render Spielkamera 50A0/12, vorn h118/h125 (`sfv2/posen_vorn.json`),
  640er `b2.zeichne` x2 NEAREST = 1280er Massstab wie die KI-Videobilder. Port-H: 568 (h118) / 580 (h125) px. KI-H: 569 / 599 px.
- `N_mess.py` (Messfunktionen, Definitionen im Dateikopf), `N_riegel.py NAME ...` (Tabelle N1-N25, KI wird immer mitgemessen),
  `N_bild.py` / `N_haarbild.py` (Belegbilder).
- KI-Bilder = Videobilder n1468 / n1475 (KI-Video, 1280x960). Nachgeprueft: birkin19/20 sind bitgleich zu n1468/n1475 an den
  Versaetzen (606,315) / (584,293), RMS 0,0 und max. 0 ueber alle Alpha-Pixel >= 250. Ist = V3 r2 =
  `sfv3/emd/EM30_V3.EMD` (sha256 1952713c..., = riegel_V3.txt). Gegenproben: V3 r1 (`EM30_V3_r1.EMD`), V1 (`sfv2/emd/EM30_V1_SPITZ.EMD`).
- Laufausgaben: `SF_belege/v3_lesung/P_nachtrag_N_riegel.txt` / `.json` (alle Werte), `P_nachtrag_N_mess.json`.
  Belegbilder: `P_nachtrag_kopf_masken_x4.png` (KI h118/h125 gegen V3 r2 x4: Auge gruen, Kern weiss, Kopf links/rechts gelb,
  Kopfmitte orange, Haarzone blau, Straehnen magenta, Maul schwarz rot, Maulumriss gelb, Ellipse hellblau, Zaehne gruen) und
  `P_nachtrag_haare_x4.png` (KI x4 roh und markiert | Original n1468/n1475 x8 | V3 r2 x4).
- Konvention: (B) = Betrachter, (W) = Wesen. Von vorn ist (B)-links = (W)-rechts = Sichelseite.
  Kuppe = Median L* im Ring 0,012-0,02 H um das Auge.

### 10.1 U1 Auge: dunkel eingebettet, mit hellem Kern, kein aufgemaltes Rund

| Mass (Spielkamera) | KI h118 / h125 | V3 r2 h118 / h125 | V3 r1 | Befund |
|---|---|---|---|---|
| Augenhuelle B x H | 0,0176 x 0,0088 / 0,0200 x 0,0117 H | 0,0158 x 0,0106 / 0,0172 x 0,0138 H | wie r2 | Groesse passt (R3) |
| L* Auge / Kuppe | 0,466 / 0,462 | **0,214** / 0,391 | 0,405 / 0,522 | r2 h118: schwarzes Loch statt eines dunklen Auges |
| Kuppe L* | 43,6 / 42,5 | **31,4 / 35,2** | 45,5 / 42,2 | die r2-Kuppe ist zu dunkel; die KI-Kuppe ist hell, ein Auge "in" der Haut |
| heller Kern: Bildpunkte > Kuppe in der Huelle | 5 / 7 | 3 / 12 | 8 / 14 | ein Kern ist da (1 Spielpixel) |
| Kern L* max | **82,4 / 77,7** | **54,5 / 54,5** | 70,4 / 70,4 | r2 grau statt glaenzend |
| Kern Lab | [73,1 5,2 -2,2] / [59,3 2,0 -1,3] | [54,5 4,6 -3,2] / [52,8 4,7 -2,8] | [70,4 7,2 -2,0] | grau-weiss, a* <= 6 |
| Fenster +-0,012 H: Px L* > 55 | **55 / 28** | **0 / 0** | 8 / 16 | = Sicht-Pruefung r2 A2 (dort im x3-Feld: KI 273/207, r2 0) |

Vorgabe: ein mandelfoermiges, dunkles Auge (R3/R4 bleiben) in einer **hellen** Kuppe, mit einem grau-weissen Glanzkern von
1-2 Spielpixeln (L* >= 65). Kein heller Ring, kein Kreuz, keine Sclera (R6/R7 bleiben). Riegel N1-N5.

### 10.2 U2 Augenlage: "das Auge ist weiter links"

Kopf auf Augenhoehe (Band +-0,01 H). KI: links = L*-Minimum der Rille zur Schulter, rechts = Ende der dunklen Haarzone
(L* wieder >= 0,5 x Kuppe, dort beginnt die Schulter). Port: Ausdehnung von Mesh 9 im selben Band.

| Mass | KI h118 / h125 | V3 r2 h118 / h125 | Differenz KI - r2 |
|---|---|---|---|
| Kopf links / rechts (1280er x) | 771 / 837; 769 / 842 | 738 / 791; 746 / 801 | - |
| Kopfbreite | 0,116 / 0,122 H | 0,093 / 0,095 H | KI 0,023-0,027 H breiter |
| Auge, Anteil der Kopfbreite von (B)-links | **0,315 / 0,286** | **0,398 / 0,458** | -0,08 / -0,17 |
| Auge zu Kopfmitte | **-0,0214 / -0,0260 H** | **-0,0095 / -0,0040 H** | **-0,012 / -0,022 H** |
| Maulmitte, Anteil der Kopfbreite | 0,356 / 0,370 | 0,528 / 0,555 | -0,17 / -0,19 |
| Auge zur Maulmitte (waagerecht) | 0,005 / 0,010 H (B)-links | 0,012 / 0,009 H (B)-links | gleich |
| Kopfauge zum Schulterauge (Messblatt K12 / R2) | -0,116 / -0,120 H | -0,118 H | gleich |

**Richtung als Zahl:** Bezogen auf die Kopfmitte liegt das KI-Auge **0,012 H (h118) bzw. 0,022 H (h125) weiter (B)-links
= (W)-rechts (zur Sichelseite)** als das r2-Auge. Beide Bilder zeigen dieselbe Richtung, es gibt keinen Widerspruch. Leser A
(Kopfbox-Anteil 0,34) und Leser B (0,36) bestaetigen es.

Bezogen auf das Maul und das Schulterauge stimmt die r2-Lage schon (Differenz <= 0,007 H). "Weiter links" heisst also: Das
Gesicht (Auge und Maul) sitzt im (B)-linken Drittel des Kopfes. Rechts davon (B) liegt die Haarzone (U3); dort ist der KI-Kopf
0,023-0,027 H breiter.

Wege fuer den Bauer (keine Nutzerfrage, beide erfuellen N6-N8):
- (a) Mesh 9 nach (B)-rechts = (W)-links um 0,01-0,03 H verbreitern, mit Haarflaechen. Das haelt K5/K12/R2.
- (b) Auge und Maul auf der Kopftextur um ~0,012 H nach (B)-links schieben. Dann wird R2 -0,106..-0,110 H, also knapp in -0,10..-0,13.

h85 (Seite): kein Links/Rechts der Kopfmitte, entfaellt.

### 10.3 U3 "Haare" rechts: dunkle Straehnen auf der (B)-rechten Kopfhaelfte

Zone: x von Auge + 0,012 H bis Kopf rechts, y von Auge - 0,05 H bis Auge + 0,025 H. Je Zeile werden die L*-Minima mit
Prominenz >= 5 gesucht; eine Straehne = zusammenhaengende Minima ueber >= 0,015 H senkrecht (Beleg
`P_nachtrag_haare_x4.png`, magenta).

| Mass | KI n1468 (= birkin19) h118 | KI n1475 (= birkin20) h125 | Original n1468 / n1475 | V3 r2 h118 / h125 |
|---|---|---|---|---|
| Zone L* / Kuppe | **0,385** | **0,406** | **0,30 / 0,36** (Schwarzwert 17 ab, 640er, gleiche Zone) | **1,031 / 0,946** |
| Straehnen (>= 0,015 H) | 4 | 4 | nicht aufgeloest (Unschaerfe) | 1 / 1 (Rauschen, Kontrast 1,2 / 2,1 L*) |
| Laengen | 0,042 / 0,039 / 0,026 / 0,023 H | 0,033 / 0,028 / 0,025 / 0,015 H | - | 0,025 / 0,024 H |
| Breite (Halbwertsbreite, Median) | 6,6 px = 0,0116 H | 6,9 px = 0,0115 H | - | - |
| Minima je Zeile (Dichte) | 2,24 | 2,33 | - | 0,71 / 0,50 |
| Lab Straehne / dazwischen | [7,3 5,9 1,1] / [18,3 7,4 1,5] | [7,3 6,5 0,2] / [18,2 7,6 0,9] | - | [31,2 3,4 1,2] / [32,4 4,9 0,0] |
| Kontrast L* dazwischen - Straehne | **11,0** | **10,9** | - | **1,2 / 2,1** |
| Straehne zum Auge | 0,0275 H Abstand; Augenmaske frei | 0,042 H; frei | - | - |
| Zone breit x hoch | 0,069 x 0,074 H | 0,075 x 0,075 H | - | 0,046 / 0,040 x 0,074 H |

Ausgangspunkt und Verlauf (relativ zur Augenmitte, (B)): Die Straehnen beginnen 0,016-0,074 H (B)-rechts und 0,020-0,048 H
oberhalb des Auges, also am Scheitel bis 0,05 H ueber dem Auge. Sie haengen nach unten bis zwischen 0,025 H ueber und 0,022 H
unter Augenhoehe. Neigung: 5 von 8 Straehnen +6 bis +39 Grad (unteres Ende weiter (B)-rechts), 3 von 8 -12 bis -22 Grad.
Sie liegen **neben** dem Auge, nicht darauf (0 Straehnen-Pixel auf der Augenmaske). "Drueber" heisst also ueber den Kopf, nicht
ueber das Auge. Die Kuppe ist nur (B)-links und um das Auge hell: Band (B)-links neben dem Auge L* 40,0 / 37,6, Haarzone 16,8 /
17,3 (Original 32,4 / 35,0 gegen 11,0 / 11,8).

Seitenansicht h85 (birkin12): Die Zone ist nicht nachweisbar. Die Kopfoberseite hinter dem Auge hat L* 35,3 = 1,49 x Ring ums
Auge und 1,06 Minima je Zeile (vorn 2,2-2,3). Das Wesen schaut (B)-rechts; zur Kamera zeigt also seine (W)-rechte Seite (aus der
Blickrichtung abgeleitet), waehrend die Haare auf der (W)-linken Seite liegen. Aus h85 gibt es deshalb keinen Riegel.

**PSX-Machart (Vorgabe):** Die Haare sind Texel auf der Kopfhaut (Mesh 9, 8 bpp, Haut-CLUT, kein Weichzeichner). Es sind
3-5 Straehnen, je 2-4 Texel breit (0,0115 H x 3865 E. x 0,066 Texel/E. = 2,9 Texel) und 4-11 Texel lang (0,015-0,042 H). Die
Straehnen haben L* = 0,17 x Kuppe (KI 7,3 / 43,6), dazwischen L* = 0,42 x Kuppe, Farbton grau-braun (a* 4-8, b* 0-2), nicht
rot. Reicht Mesh 9 dafuer nicht bis zur Schulter (Weg (a) in 10.2), kommen hoechstens 2 flache Dreiecke je Straehne an
Knochen 9 dazu. Mesh 9 bleibt <= 118 Dreiecke (R30; r2 hat 109, es bleiben also 9). Mesh 0-8 und 10-15 bleiben
byte-gleich (R28).

### 10.4 U4 Maul: fast geschlossen, breit, unregelmaessig, Lippen, rotes Kinn, kein Comic-Oval

Maulfenster x Auge - 0,045 .. + 0,06 H, y Auge + 0,02 .. + 0,09 H. Schwarz = L* < 0,25 x Kuppe (Leiste 11). Senkrechte
schwarze Laeufe >= 0,045 H (Rillen Kopf/Schulter und Kinn/Brust der KI) werden entfernt. Maulspalten: KI = Messblatt
Abschnitt 6 (x 165-212 / 185-239); Port = groesste schwarze Komponente +- 2 px. Umriss = schwarze Maske, Schliessung 3x3 2-mal,
groesste Komponente. Ellipse = gleiche Flaeche und gleiche zweite Momente.

| Mass | KI h118 / h125 | V3 r2 | V3 r1 | V1 | Riegel |
|---|---|---|---|---|---|
| **Umriss/Ellipse-IoU ("Oval")** | **0,438 / 0,465** | **0,772 / 0,726** | 0,735 / 0,733 | 0,788 / 0,796 | N14 <= 0,60 |
| Spalt (Median schwarze Px je Spalte) | 0,0088 / 0,0150 H | 0,0282 / 0,0250 H | 0,032 / 0,027 | 0,048 / 0,028 | N15 <= 0,018 H |
| schwarze Flaeche | 11,6 / 15,0 e-4 H^2 | 24,9 / 25,0 | 28,0 / 24,9 | 11,7 / 25,7 | N16 <= 16 |
| Anteil der groessten schwarzen Komponente | 0,29 / 0,66 (17 / 21 Teile) | 1,00 / 0,99 (1 / 3) | 1,00 / 1,00 | 0,75 / 0,99 | N17 <= 0,80 |
| Oberlippenlinie, Rest-Std gegen Gerade | 0,0068 / 0,0071 H | 0,0012 / 0,0024 H | 0,0026 / 0,0040 | 0,0015 / 0,0033 | N18 >= 0,005 H |
| obere Zahnreihe, Rest-Std gegen Gerade | 0,0098 / 0,0089 H | 0,0020 / 0,0027 H | 0,0050 / 0,0007 | 0,0026 / 0,0047 | N19 >= 0,005 H |
| Mundwinkel: (B)-rechts minus (B)-links | **+0,0134 / +0,0044 H** | -0,0033 / +0,0021 | 0,0000 / +0,0007 | +0,0018 / -0,0035 | N20 >= +0,004 H |
| Lippenanteil (dunkelrot, nicht schwarz) | 0,51 / 0,44 | 0,23 / 0,27 | 0,27 / 0,24 | 0,16 / 0,32 | N21 >= 0,38 |
| Lippe Lab, L*/Kuppe | [17,3 17,1 5,7] / [16,9 16,4 5,7]; 0,40 / 0,40 | [12,6 14,5 3,7] / [14,3 15,1 3,7]; 0,40 / 0,41 | 0,36 / 0,34 | 0,42 / 0,47 | N22 0,32-0,48 |
| Lippenglanz max(RGB) > 170 ohne Zaehne | 21 / 0 (Summe 21) | 0 / 0 | 0 / 0 | 0 / 44 | N25 Summe >= 4 |
| Kinn (Auge + 0,09 .. + 0,12 H, Maulmitte +-0,02 H): Lab, Anteil a* >= 12 | [16,6 16,2 4,4] 0,93 / [18,0 17,8 5,7] 0,98 | [22,3 7,1 3,5] 0,15 / [21,1 7,7 3,6] 0,18 | 0,25 / 0,50 | 0,99 / 0,12 | N23 >= 0,70; N24 L* 14-22 |
| Zaehne (Top-Hat-Flecken) oben / unten | 8 / 7 je Bild | 6 / 5 | 4 / 3; 5 / 4 | 3 / 3; 3 / 2 | R14 bleibt |

Befund: Der KI-Mund ist ein flacher, zerklueftet-dunkler Spalt (0,009-0,015 H). Er ist umrahmt von dunkelroten Lippen (rund
die Haelfte der Maulflaeche), die Oberlippenlinie ist unregelmaessig, und der (B)-rechte Mundwinkel liegt tiefer. Darunter sitzt
ein rotes Kinn. Bei V3 r2 ist es ein einziges schwarzes Loch (1 Komponente, IoU zur Ellipse 0,73-0,77, Spalt 0,025-0,028 H)
mit gerader Oberkante und gerader Zahnreihe, das in graue Haut uebergeht. Die Gegenprobe ist rot: r2 faellt bei N14-N21, N23, N24 (h118)
und N25. Die Zahlen der Sicht-Pruefung r2 ("Was der Bauer fuer Runde 3 braucht" 1-3) sind sinngemaess als N15/N16 (dunkle
Flaeche, Spalthoehe), N21/N22/N25 (Lippen Lab um [16 13 3], Glanz) und N23/N24 (Kinn a* >= 12, L* 15-22, 0,03 H hoch)
uebernommen. Fenster und Schwellen sind hier relativ zur Kuppe und in H, damit Bauer und Pruefer am Port dieselbe Zahl messen.
Die absoluten x3-Feld-Zahlen der Sicht-Pruefung (KI 3057 / 4817 Px L* < 15) sind nicht direkt vergleichbar.

### 10.5 RIEGEL N1-N27 (Spielkamera 50A0/12 vorn h118 UND h125, ausser N25-N27)

Messung: `python N_render.py <EMD> <NAME>`, danach `python N_riegel.py <NAME>` (re15_port/tools/r41_sichel/v3/). Die Werte
fuer KI, r2, r1 und V1 stehen in `SF_belege/v3_lesung/P_nachtrag_N_riegel.txt`. Ergebnis: KI 0 rot von 25, V3 r2 23 rot,
V3 r1 18 rot, V1 22 rot. Toleranz = KI-Bereich h118/h125 plus Rand; die Gegenprobe r2 muss rot sein.

| Mass | Vorgabe | Toleranz | Messmethode am Port-Render (N_mess.py) |
|---|---|---|---|
| N1 Augenkern hell | KI 55 / 28 Px | >= 12 Px L* > 55 im Fenster +-0,012 H (1280er) | auge.fenster_L55 |
| N2 Augenkern Glanz | KI L* 82 / 78, grau-weiss | L* max in der Augenhuelle >= 65; Kern a* <= 6 | auge.kern_Lmax, lab_kern |
| N3 Augenkern Groesse | KI 5 / 7 Px = 1-2 Spielpixel | 4-16 Px (L* > Kuppe in der Augenhuelle) | auge.kern_px |
| N4 Auge dunkel eingebettet | KI 0,466 / 0,462 | L* Auge/Kuppe 0,35-0,55 | auge.L_auge_haut |
| N5 helle Kuppe | KI L* 43,6 / 42,5 | 38-50 | auge.Lh (Median L* Ring 0,012-0,02 H) |
| N6 Augenlage im Kopf | KI 0,315 / 0,286 | 0,24-0,36 der Kopfbreite von (B)-links | kopf.lage_kopf (Mesh 9 im Band +-0,01 H) |
| N7 Auge zu Kopfmitte | KI -0,021 / -0,026 H ((B)-links = (W)-rechts) | -0,032..-0,016 H | kopf.auge_zu_kopfmitte_H |
| N8 Maulmitte im Kopf | KI 0,356 / 0,370 | 0,28-0,45 | Mitte der Maulspalten, Anteil der Kopfbreite |
| N9 Haarzone dunkel | KI 0,385 / 0,406; Original 0,30 / 0,36 | L* Zone / Kuppe 0,30-0,50 | haare.L_zone_kuppe |
| N10 Straehnen | KI 4 / 4 | 3-6 Straehnen >= 0,015 H | haare.straehnen |
| N11 Straehnen-Kontrast | KI 11,0 / 10,9 | L* dazwischen - L* Straehne >= 6 | haare.lab_zwischen - lab_straehne |
| N12 Haarzone Breite | KI 0,069 / 0,075 H | >= 0,05 H (Auge + 0,012 H bis Kopf rechts) | haare.zone_H[0] |
| N13 Auge frei | KI 0 / 0 | 0 Straehnen-Px auf der Augenmaske | haare.ueber_auge |
| N14 kein Oval | KI IoU 0,438 / 0,465 | Umriss/Ellipse-IoU <= 0,60 | maul.umriss_iou |
| N15 fast geschlossen | KI 0,0088 / 0,0150 H | Spalt-Median <= 0,018 H | maul.spalt_median_H |
| N16 kleine schwarze Flaeche | KI 11,6 / 15,0 e-4 H^2 | <= 16 e-4 H^2 | maul.schwarz_flaeche_e4H2 |
| N17 zerklueftet | KI 0,29 / 0,66 | Anteil groesste schwarze Komponente <= 0,80 | maul.anteil_groesste |
| N18 Oberlippenlinie unregelmaessig | KI 0,0068 / 0,0071 H | Rest-Std >= 0,005 H | maul.ecken.oberlinie_rest_std_H |
| N19 Zahnreihe unregelmaessig | KI 0,0098 / 0,0089 H | Rest-Std obere Reihe >= 0,005 H | maul.oben_rest_std_H |
| N20 Mundwinkel ungleich | KI +0,0134 / +0,0044 H | (B)-rechts tiefer >= +0,004 H (h125 knapp) | maul.ecken.dy_rechts_minus_links_H |
| N21 Lippen | KI 0,51 / 0,44 | Lippenanteil >= 0,38 | maul.lippe_anteil |
| N22 Lippenhelligkeit | KI 0,40 / 0,40, Lab um [17 17 6] | L* Lippe / Kuppe 0,32-0,48; a* >= 14 | maul.L_lippe_kuppe, lab_lippe |
| N23 rotes Kinn | KI 0,93 / 0,98 | Anteil a* >= 12 >= 0,70 | maul.kinn_anteil_a12 |
| N24 Kinnhelligkeit | KI L* 16,6 / 18,0 | 14-22 | maul.lab_kinn[0] |
| N25 Lippenglanz | KI 21 Px (h118 21, h125 0) | Summe h118 + h125 >= 4 Px max(RGB) > 170 ohne Zaehne | maul.glanz_px |
| N26 Abnahme Seite an Seite (U5) | "schon so, wie in der Vorlage" | Nutzer-Leiste: Spalten KI, Original, V3 r2 (vorher), V3 neu; Zeilen h118, h125, h85 x4 und Nahansicht; V1/Ref2 nur in einer eigenen Datei `..._gegenprobe`. Der Pruefer urteilt am Bild (Sicht-Pruefung K1/K2), die Zahlen sind Pflicht, reichen aber nicht | Leiste + Sicht-Pruefung |
| N27 PSX-Machart Kopf | wie Haut | Haare und Lippen als 8-bpp-Texel auf Mesh 9 aus der Haut-CLUT; Straehne 2-4 Texel breit; <= 2 Dreiecke je Straehne; Mesh 9 <= 118 Dreiecke; Mesh 0-8/10-15 byte-gleich (R28); kein Weichzeichner, keine Farbe ausserhalb der CLUT | R28/R30/R31/R32 auf die neue EMD |

R1-R14 (Nahansicht) bleiben als Mindestbedingung. Bei Widerspruch gilt N, zum Beispiel R5 (Nahansicht 0,418) gegen N4
(Spielkamera 0,214).

### 10.6 Offen (Nachtrag)

1. Keine neue Nutzerfrage. Weg (a) oder (b) in 10.2 waehlt der Bauer, beide erfuellen N6-N8.
2. N20 (Mundwinkel) ist am knappsten: KI h125 +0,0044 gegen die Schwelle +0,004. Er stuetzt sich vor allem auf h118 (+0,0134).
3. Der KI-Maulumriss behaelt einen Rest der rechten Kinn/Brust-Rille (h118 Spalte x +0,042..+0,046 H, sichtbar in
   `P_nachtrag_kopf_masken_x4.png`). Er senkt die KI-IoU um geschaetzt <= 0,05. Die Schwelle N14 (0,60) hat 0,13 Abstand zur
   KI und 0,13 zu r2.
4. Ob die Straehnen Haare, Adern oder Hautfalten sind, entscheidet bei 1-2 Spielpixel Breite kein Bild. Riegel sind nur Dunkelheit,
   Zahl, Laenge und Breite.
5. Das Original-Video loest die Straehnen nicht auf. Es zeigt nur die dunkle Zone (L*/Kuppe 0,30-0,36): wahrscheinlich, nicht sicher.
6. In der Seitenansicht h85 ist die Haarzone nicht sichtbar. Dass dort die (W)-rechte Seite zur Kamera zeigt, ist aus der
   Blickrichtung abgeleitet, nicht gemessen.
