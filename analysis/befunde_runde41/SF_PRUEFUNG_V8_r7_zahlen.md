# SF Sichel-Form V8 Runde 7 - Zahlen-Pruefung (eigener Code)

Stand: Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8`, Zweig r41/sichel_v8, HEAD d5e3cc06e.
Asset `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`, 277316 Bytes, sha256 5c902d493e6815f8551e0297eb1ef7e8c6cbda4b11ed3ab5191cf6a62ac11f30 (= Pin `release/em30_hangar.sha256`).
Pruefer-Ordner P = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr7/`
(Code: P/w = Pruefer-Code des P5-Ablaufs r3, angepasst; P/a = Pruefer-Code Runde 6, angepasst; P/h = Hand; P/f6 = Riegel-Probe).
Vergleichsstaende in P/m: NEU (5c902d49), R6 = Stand 6d (e9e71bca), R7A (a474188a), R7B (87b02681), R3 (9860f6bd), R2 (969789fd), R4 = V8 r4 (10d02173), R5D = V8 r5d (c4976c62), R6B (95a783a6), V7 (319bbd17).
Eigene Szenenlaeufe (lauf_v4.sh-Kopie, eigene Exe-Kopie 8f15f6c5, nacheinander): je Stand plain + Marker (P/laeufe), Spiel-Asset bis F1110 (P/laeufe/SPIEL), Marker-Laeufe HM/KO/GS/SC/SW/MK/GR und plain F200-F1100 (P/a).
Determinismus: eigener V7-Lauf = Lauf des P5-Pruefers r3 (226 von 226 Bildern bytegleich); eigener NEU-Lauf (Overlay) = Bauer-Lauf r7d/szene (226 von 226 bytegleich).
Riegel und Messwerkzeuge der Bauer sind kein Beleg; nur fuer (F6) wurde riegel_hand.py als Pruefgegenstand laufen gelassen.

## Ergebnis: BESTANDEN (kein ROT), mit GELB-Punkten

Kein Pruefpunkt ist ROT. GELB (mit Zahl, siehe Tabellen): Vorder-Klasse echt 18 statt 17 Bilder (das Zusatzbild F312 ist Erbe des Arms, E12);
Offline-Seitenansichten ohne Szenen-Gegenstueck (E13); helle Haut-Texel auf Mesh 8 links neben dem Kopf (E12, 0 Keile; gegen 6d deutlich mehr);
Umriss um den Kopf gegen r4 in der Griffphase bis 6,7 px (Ecken 17/30, E10 mit erfuellten Bedingungen); Texeldichte-Erbe t36/t44/q3/q12;
S3 K 3,4 (Erbe); Gesicht 15 Port-px in F312 durch den Arm (Erbe); Stufen der Vorderkontur (17 Bilder, 6d 9, r4 16); Schlitz oben 35 = 6d;
Hand: Spitzen-Keil in h132 nur 1-2 px (4 von 5 Paar-Posen in der Spanne), Daumenlaenge offline bis 3,1 px ueber der Spanne (echt 1,4 px, in der
Toleranz); Durchdringung gegen r5d in Clip 20 (= 6d); Schwanz: P2-Band 120-130 im Bild 8,1-9,7 % (Winkel-Artefakt), Deckel-Dreiecke in F312/F313
unveraendert gegen 6d.

## (A) wie Runde 5 / (J) nichts verloren

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Kette zweimal gebaut | bytegleich, = Asset, Stufe 6 = V7 | P/kette_a.txt, kette_b.txt: beide "V8-KETTE-OK sha256 5c902d49...", A = B (cmp), A = Spiel-Asset, Stufe 6 V7 319bbd17 OK | GRUEN |
| Pin | = Asset | release/em30_hangar.sha256 = 5c902d49... = Datei | GRUEN |
| Bau | LOCAL-BUILD-OK | P/build.txt "=== LOCAL-BUILD-OK (build)", BUILD-RC=0 | GRUEN |
| Tests Gruppe | 8/8 | P/ctest_gruppe.txt "100% tests passed, 0 tests failed out of 8", "Total Test time (real) = 337.75 sec" | GRUEN |
| Tests Unit | 1013/1013 | P/ctest_unit.txt "100% tests passed, 0 tests failed out of 1013", "Total Test time (real) = 436.65 sec" | GRUEN |
| Szenen-state.log | bytegleich V7 | eigener Lauf Spiel-Asset bis F1110: sha256 e48e1cad..., 1131 Zeilen, cmp gegen sfv4/r/V7_szene/state.log gleich; debug.log "Szene Ende: Cut 4, Birkin Zustand 0" | GRUEN |
| Code-Diff seit 3d2d230f5 (und seit 5820af00f) | nur Werkzeuge/Asset/Pin/Tests/Belege | ausserhalb analysis/ und tools/r41_sichel/ genau: Asset, re15_port/tests/unit/test_birkin_formen.c (Pins/Erwartungen auf 7c nachgezogen, kein CHECK entfernt), release/em30_hangar.sha256 | GRUEN |
| Stand-Abfolge | 7a nur Hand, 7b nur Schwanz, 7c nur Mesh 8 | P/stand.txt: 6d->NEU Meshes 8, 12, 16-22; 7a->NEU 8, 16-22; 7b->NEU nur 8; Textur, CLUT, EDD, EMR-Knochen bytegleich 6d; Keyframes 0 Knochen anders als 6d | GRUEN |
| Gesicht | Mesh 9 + Zeile bytegleich V7 | Mesh 9 roh 4280 Bytes = V7, einzige Zeile 486 = V7, 6379 Texel gleich, Keyframe-Winkel Knochen 9 gleich (P/a/s5s7s8.txt) | GRUEN |
| S1 Rueckenlinie | Mittel <= 3 (Riegel 4), Max wie 6d | 2,27 / 6,0; 7 Halbpixel-Varianten 2,27..2,50 / 6..7 (6d 2,23..2,41 / 6..7) (P/a/form_V8.txt, jitter.txt) | GRUEN |
| S2 (c) Rumpf ueber Schwanz | <= 3 px alle Varianten | 2,0 px in 7 von 7 (= 6d); S2 (a) faellt in denselben 3 Varianten wie 6d | GRUEN |
| S3 Kopf-Linie K | <= 3, sonst Erbe (E12) | K 3,4 (Varianten 2,6..3,4) = 6d 3,4; Kuppe 3,0; keine weitere Erhebung | GELB - Erbe (6d gleich) |
| S4 Schwanz-Dicke | Form wie 6d | Schwanz-Umriss gegen 6d: 30-47 von 3900-4800 px verschieden (PV, P1, P2, F264, F276, F300, F350); Wurzel 59,2. Mein Ring-Sucher sieht die 7b-Abschnittsringe als eigene Ringe (Messfehler des Werkzeugs, wie in r6) | GRUEN |
| S6 Boden | 0 | 0 von 912 Keyframes, groesster Ueberstand -25 (V7 70) | GRUEN |
| Schwanz-Kette | Gelenk <= 25,5 Grad | max 25,2, 0 Keyframes > 25,5, Glieder 541 E. (P/a/kette3d.txt) | GRUEN |
| S8 Format | gueltig | 23 Meshes, 23 Knochen, 21 Clips, 912 KF, 16 CLUT-Zeilen, Hoehe 4096, 277316 Bytes | GRUEN |
| S10 Wurzel hinten (echt) | 0 | 0 px F300-F380 (R3-Altstand-Gegenprobe 69114) (P/a/s10r5.txt) | GRUEN |
| Schlitz oben (echt, GR) | <= 6d | oberste 40 Zeilen 35, oberste 120: 118 = 6d 35 / 118 (V7 21 / 268, r4 1 / 98) (P/a/loch5.txt) | GELB (= 6d) |
| Kappe | <= 6d | a* >= 30 am Kopf P1 21 / PV 23 / P2 9 = 6d; Kopf sichtbar P1 1272, PV 1120 (= V7), P2 902 (P/a/kappe6.txt) | GRUEN |
| Adern-Herkunft | 0 fremde | Meshes 8/13/14/16-22: 0 Texel ungleich Original, 0 transparent; Mesh 8: 1 Texel ausserhalb der Original-Nutzung (t65 v0 u170, schon in 6d und r5d) (P/a/herkunft4.txt) | GRUEN |
| S9 Stufen Vorderkontur (echt F340-F380) | nicht schlechter als r4 | 17 Bilder > 2 px ueber V7 (6d 9, r4 16), neue Ecken 4 (6d 2, r4 4); groesster 7 Port-px F352 (6d 6); F344 / F354 / F364: 0 px (P/a/s9szene5.txt) | GELB |
| S14 Brust (P1) | Kanten-Sprung <= r5d (E2), 0 weisse Texel in der Brust | echt 18 Bilder Betrag dL 10,93 (6d 10,62, r5d 11,24, V7 7,04); unbeleuchtet P1 10,90 (r5d 11,14); Brust-Texel reinweiss / fast weiss 0 (P/a/p1c.txt, p1b.txt) | GRUEN |
| S14 (b) fast weisse Bildpunkte auf Mesh 8 | 6d gruen | echt (L* > 75, C < 12, Bauer-Schwelle) F344-F378: NEU 7321 px in 35 Bildern, 6d 0, V7 0, r4 15563; ganze Szene 24499 / 6d 8659. Alle NEU-Punkte liegen auch in r4 (F344 255 von 255, F354 193 von 193 ...): Rueckenplatte LINKS neben dem Kopf, Klasse Mesh 8 oben, groesster Fleck 129 px, L* 77 (Kachel P/k_weiss_F354.png). Strenge Schwelle L* >= 80: 0. Nach E12 helle Haut-Texel = GELB | GELB (E12; Rueckschritt gegen 6d) |

## (B) / (F) / (H) Hand und Unterarm in den echten Szenenposen

Posen aus dem eigenen Protokoll (P/laeufe/NEU/debug.log, RE15_HANGAR_LOG): h125 F368 Clip 1 Bild 35 Gier 1515 (-9065,-22223); h126 F369 Bild 36 Gier 1513;
h128 F371 Bild 38; h130 F373 Bild 40; h132 F375 Bild 42; Kontrolle h100 F343, h110 F353, h135 F378 - alle gleich den Bauer-Angaben.

Vorlage selbst nachgemessen (P/h/hand6b_vorlage.txt, hand7_vorlage.json; Video-px), birkin20/22/23/21/24: Unterarm 43,6 47,5 47,2 50,5 49,7;
Hand 58,3 57,4 58,7 58,5 68,1; Handgelenk 20,2 19,8 21,9 21,4 25,1; breiteste Stelle 38,3 38,8 44,6 47,0 50,7; Spitzen (2 px vor dem Ende) 9,6 7,6 6,6 10,6 10,5;
Hoehen E 0,627-0,648, W 0,461-0,501, T 0,257-0,319; Daumen-Gabel ab Handgelenk 29,1 25,9 25,2 22,6 32,0; Daumen ab Gabel 18,5 20,2 20,7 20,9 20,4.
STICHPROBE MESSBLATT HAND: Unterarm (MB 42,4-49,8), Hand (56,9-68,5), Handgelenk (20,0-25,5), breiteste Stelle (37,8-50,6), Hoehen und Daumen-Gabel
(22,4-30,5) halten auf +- 1,3 px. Abweichend: Spitzen eigen 6,6-10,6 (MB 5,1-9,0) und Daumenlaenge eigen 18,5-20,9 (MB 16,9-19,4) - andere Messregel.

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| F1 Verdrehung gegen Capcoms Original, 912 KF | ~0 Grad | Mesh -0,72 Grad, Daumen-Winkel-Abweichung Median 0,71 (0,59..0,84); Handruecken nach vorn 86,2 % (Original 84,2, 6d 86,3, V7 72,7). Gegenprobe Rest-Bauer R6B: 118,08 Grad / 47,7 % (P/h/hand6a.txt) | GRUEN |
| F2 Handseite zur Kamera, Paar-Posen | Handruecken wie Original | offline -20,9 / -21,4 / -22,1 / -22,8 / -22,6 Grad (0 = Ruecken frontal); Original -30,5..-32,3; R6B +80,8..+84,3 (Kante). Echtes Spielbild (Marker HM_V8, F368-F375): Handruecken 90,8-93,8 % der Handflaeche ohne Krallen, Innenflaeche 0 px (P/h/hand6b_echt.txt) | GRUEN |
| F3 Ketten-Masse je Paar | in der Vorlagen-Spanne, Toleranz >= Betrachter/Spielbild | offline: Unterarm 48,7-50,1, Hand 67,3-68,7 (Spanne 57,4-68,1, +0,6), Handgelenk 19,2-20,4 (19,8-25,1, -0,6), breiteste 45,1-47,5, Hoehen E 0,632-0,639 / W 0,475-0,488 / T 0,266-0,281. Echt: Handgelenk 19,1-20,4, breiteste 44,9-47,5, Unterarm 47,6-49,6, Hand 65,4-67,7. Betrachter gegen Spielbild: Lage E/W/T <= 2,0 px, Laengen <= 2,6, Breiten <= 1,2 px. Streuung der Vorlage: Handgelenk 5,3, breiteste 12,4 px, Hoehen 0,021 / 0,040 / 0,062 H. (Hoehen im echten Bild nicht auswertbar: Fuesse am Bildrand abgeschnitten, H 286-295 statt 316-323) | GRUEN |
| Hand nicht kleiner als Vorlage | - | Hand 65,4-68,7 px, breiteste 44,9-47,5 px (V7/r5d 30,7-34,0) | GRUEN |
| (H) S17 (a) Spitzen-Keil | 2 px vor dem Ende in der Spanne | offline 8,1 / 7,1 / 9,1 / 8,1 / 1,0 px; echt 6,1 / 8,1 / 8,1 / 6,1 / 2,0 px (h125..h132; Spanne eigen 6,6-10,6, MB 5,1-9,0); 6d: 1,0-3,0 bzw. echt 2,0 in allen. h132 (birkin24, Vorlage 10,5): eine Krallenspitze steht 2 px vor (3 px vor dem Ende 8,1) | GELB (4 von 5) |
| (H) S17 (d) Daumen-Gabel | in der Spanne 22,6-32,0 | offline 28,4-30,9, echt 28,3-30,9 | GRUEN |
| (H) S17 (d) Daumenlaenge ab Gabel | in der Spanne 18,5-20,9 (eigen) | offline 21,2-24,0 (6d 24,9-27,4), echt 20,1-22,3 (je Paar +1,6/+2,1/+0,8/+0,2/0,0 ueber dem eigenen Paarbild); Betrachter/Spielbild bis 2,3 px | GELB (echt in Toleranz) |
| Daumen-Dicke / Fingerblock | Spanne 10,0-15,5 / 23,0-31,1 | offline 12,9-15,0 / 25,7-27,7, echt 11,0-14,1 / 24,7-27,3 | GRUEN |
| Krallen kreuzen | nie | Krallenachsen (2D) in allen 8 Posen: keine Kreuzung; Gegenprobe r5d: Kreuzungen (1,2),(1,3) in allen Posen (P/h/hand7_kreuzung.json) | GRUEN |
| CLUT / Texel Hand | Daumen 491, Finger 492, 0 fremd | Mesh 12: 491 6 Flaechen, 492 18, 490 34; UV/CLUT/Flaechen von Mesh 11 und 12 bytegleich 6d und V7 | GRUEN |
| Texeldichte Hand | 0,5-2 x Original | Hand 0,00462 = 1,69 x Original 0,00274 (6d 1,78 x), Unterarm 0,00606 = Original 0,00607 | GRUEN |
| F4 Gelenk | ohne Luecke | Wurzelring ausserhalb der Unterarm-Huelle Median 14,0 E. (6d 14,2, Original 91,3); echtes Bild Abstand Unterarm/Hand 1,0 Port-px in allen Paar-Posen | GRUEN |
| F4 Durchdringung gegen 6d (Nachtrag 7) | nicht schlechter | echte Flaechen, Tiefe > 20 E.: NEU 25 KF = 6d 25 KF, dieselben Clips (4: 1, 20: 48 Bild-Eintraege); groesste Tiefe Clip 20 267 E. (6d 264), Clip 14 12 (6d 11) (P/h/durch6.txt) | GRUEN |
| F4 Durchdringung gegen r5d | nicht schlechter in zeigbaren Clips | schlechter: Clip 20 Bild 12-59 (48 Eintraege, bis 267 E. bei Bild 20, KF 822) und Clip 4 Bild 42 (KF 230, 27 E. unter dem Boden); besser: Clips 5, 9, 11, 18. Clip 20 = 0x14 = Mutations-Set-Piece (enemy_ai_common.c Sub 6 @0x80117f80), in den Laboren mit dieser Form erreichbar. Seit 6c unveraendert | GELB (Erbe 6c) |
| F5 nur Erlaubtes | 7a: nur Mesh 12 | 6d -> 7a nur Mesh 12; Keyframes 0 Knochen anders als 6d (Knochen 10 aus 6c bleibt, E10); Textur/UV/CLUT der Hand gleich | GRUEN |
| F6 Riegel ehrlich | Varianten fallen | riegel_hand.py auf Variante Verdrehung 30 / 60 / 118 Grad (P/f6/vd.py auf NEU) und Beuge 60 (BG60 des Pruefers r6): S20 ROT in (a,b) / (a,b) / (a,b) / (c), S17 ROT in (a,b,c,d) / (a,c,d) / (a,b,c,d,e) / (a,b,c,d,g); NEU: S17 und S20 GRUEN. Schranke Verdrehung <= 5 Grad, NEU 0,87 (Abstand 4,1); Kamera <= 30 Grad, NEU -4,4..-5,9; Hoehen +- 0,01 um die Spanne | GRUEN |

## (C) P5: Luecke Ruecken/Kopf, vorn ohne Kragen

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Foto-Posen offline (7 Varianten) | 21 / 7 / 3,2 | PV 9/0/1,4, P2 10/0/1,4, P1 0 (V7 PV 55/34/15,0, P2 46/23/5,0) (P/off/split.txt) | GRUEN |
| Vorder-Klasse offline (cos >= 0,4) | <= V7 | 28 Ansichten / Summe 1875 (V7 30 / 2086) | GRUEN |
| Seiten/hinten offline (cos < 0,4) | 21 / 7 / 3,2 | 4 Ansichten ueber Soll, alle ohne Szenen-Gegenstueck: PV+1536 39/18/4,1, PV+3840 39/18/4,5, P2+1536 34/10/3,6, P2+3840 26/5/2,2 (V7 8 / 397) | GELB (E13) |
| Vorder-Klasse echt F235-F378 (ohne F316-F333 Vordergrund) | <= V7 | 19 / 1217 roh; ohne F340 (E13: Klassen-Artefakt der Ergaenzungshaelften, V7 und R2 dort 0, Silhouette gleich) 18 / 1041; V7 17 / 1306. Das 18. Bild ist F312 (cos 0,42) mit linker Kerbe 53/28/5,0 bei (404,191): dort zeigt V7 Arm (Klasse Rest, 141 von 141 Kerben-Bildpunkten), NEU, R2, R3 und 6d haben dieselbe Kerbe 53/5,0 -> Folge der Armhaltung aus 6c, nicht Mesh 8/9 (E12). Ohne F312: 17 / 987 | GELB - Erbe (E12) |
| Seiten/hinten echt | 21 / 7 / 3,2 (E3) | F324, F326, F331, F332 ueber Soll - Vordergrund-Bilder, in V7, R2, R3, R4, 6d zahlengleich (163/102/6,1 ...) | GELB (= V7) |
| Zacke + V-Kerbe rechts neben dem Kopf in Folge | 0 | NEU 0 (Einzelbilder F316/F324/F326 = V7, F340 Artefakt); Gegenprobe R2: 10 Bilder in Folge F276-F280, F308-F312; r4 und 6d je 16 (P/laeufe/zacke_rechts.txt) | GRUEN |
| Oranger Texel-Fleck | 0 Bilder | hell-orange Mesh-8-Bildpunkte (L* >= 50, a* >= 10, b* >= 20) F225-F450: NEU 1421 px = R2 1440 = 6d 1304 (normale Haut); R3 2781 (+ Fleck an t60). Dunkle orange-rote Punkte (L* 24-38) gegen R2 sind Ader-Texel an anderer Stelle (P/laeufe/orange7.txt, orange7b.txt) | GRUEN |
| Fremde Texel Mesh 8 | 0 | 0 Texel, die R6/R4/V7/R2 auf derselben Zeile nicht benutzen; R3-Gegenprobe 30 Texel (t60 am Texturrand) | GRUEN |
| Gesicht offline, 177 Ansichten, Maler-Reihenfolge | 0 gegen V7 | durch Mesh 8: 0. Gesamt 7 px: F312 6 (Mesh 10 t16/q8), PV+1792 1 (Mesh 10) = Erbe; gegen 6d 0 (P/vorn/gesicht_alle_*.txt) | GELB - Erbe (Mesh 8: GRUEN) |
| Gesicht echt F225-F450 | 0 | 15 Port-px in F312 (Klasse Rest = Arm, = 6d und R2), Mesh 8: 0; F313: 0, F314: 0 (R3-Gegenprobe F313 1 px Mesh 8) | GELB - Erbe |
| Kragen | 0 | 0 px, hoechster Rumpfpunkt von vorn y8 -1268 (V7 -1260); Gegenprobe r5d 49387 px in 136 Ansichten (P/vorn/vorn_*.txt) | GRUEN |
| Rahmen (Anhang an Mesh 9) | 0 | 0 (Mesh 9 bytegleich V7) | GRUEN |
| Umriss um den Kopf gegen V7 / r4 | E10 | ausserhalb V7: 8321 px, max 14 (R3 8896); ausserhalb r4 und V7 > 2 px in 34 von 136 Vorderansichten, groesster F278 6,7 px (363 px), F276-F280 5,0 (Griffphase, Platte an Ecke 17/30). E10-Bedingungen gemessen: Kragen 0, Rahmen 0, S7 50,0 %, Gesicht durch Mesh 8 0 | GELB (E10) |
| Helle Bildpunkte Kasten (420,200)-(640,400), F344-F378 | E12: kein Keil >= 20 px, den r4 nicht hat | ganze Kastenflaeche 90-130 (Median 104), V7 69-78, r4 89-111, 6d 81-112; Keile 0 | GELB (E12) |
| Texeldichte / Streckung geaenderter Flaechen (Median unberuehrter Rueckenflaechen) | 0,5-2 / <= 2 | t8 0,56 (K-b erfuellt; R3 0,47). Ausserhalb: t36 0,55/2,91 (V7 2,96), t44 0,39/4,48 (r4 0,37/4,47), q3.1 0,62/2,59, q12.0 0,24/5,72, q12.1 0,63/2,22 (r4-Stand der Ecken 6/43) (P/vorn/flaechen_NEU.txt) | GELB - Erbe |
| Normalen-Sprung | keiner | Mesh 8 69 Ecken / 69 Normalen, Normalen 0..68 bytegleich 6d; Ergaenzungsdreiecke teilen Ecken und Normalen ihres Vierecks | GRUEN |
| Buckel-Rot von vorn (beleuchtet) | >= 50 % | 50,0 % (72/144; 6d 57,6, Vorlage 58,3) (P/a/huegel3.txt) | GRUEN (ohne Reserve) |
| Stufen Vorderkontur | gegen r4 | siehe S9 oben: 17 Bilder / 4 neue Ecken (r4 16 / 4) | GELB |

Endstand gegen R3: offline und echt Luecke zahlengleich (32 / 2318 bzw. 26 / 2504); orange Fleck weg (2781 -> 1421 px), fremde Texel 30 -> 0, t8 0,47 -> 0,56,
Gesicht F313 1 -> 0, P2+2048 1 -> 0; Buckel-Rot 50,0 = 50,0. Gegen V7: Foto-Posen von 55/46 auf 9/10 px, Vorder-Klasse offline 30 -> 28, echt 17 -> 18 (F312, Arm).

## (D) P4: Schwanz-Unterseite

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| graue Deckel (Zeile 487) auf der Unterseite | 0 | 0 Dreiecke mit Median-theta <= 110 (6d 7, r5d 91); sichtbar 0 px in 177 Ansichten (P/a/p4c.txt, p4d.txt) | GRUEN |
| Kanten Fleisch/Grau da* >= 15 | wenig | 1 von 55 (Kegel 16 d14/d15, da* 28,3), 6d 6 von 51; laengengewichtet da* 5,66 (6d 7,02) (P/a/p4kante.txt) | GRUEN |
| Szenenbilder mit grauem Ring (echt, Marker SC) | 0 auf der Unterseite | Deckel-Dreiecke sichtbar 2305 px in 58 Bildern (6d 3677 / 57); F312 306 px und F313 315 px bytegenau wie 6d (Deckel-Dreiecke; offline liegt keines auf der Unterseite); F314-F324 0 (P/a/sc.txt) | GELB (F312/F313 = 6d) |
| Band Zeile 495 je theta | 100-110 rot, 110-120 Uebergang, 120-130 <= 5 % | Flaeche (PV): 98,6 / 82,6 / 1,5 %; Bild PV 7 Varianten 100 / 90,5-92,2 / 0,0 %; Bild P2 100 / 96,6-97,5 / 8,1-9,7 % (6d 9,8-10,8; Winkel-Artefakt K5) | GRUEN (PV) / GELB (P2) |
| Herkunft | 0 fremde | Meshes 16-22: 0 Texel ungleich Original, 0 ausserhalb der Original-Nutzung | GRUEN |
| Oberseite | grau | theta >= 135: 0,00 % Zeile 495 | GRUEN |
| Gegenprobe r5d | ROT | 91 graue Dreiecke unten, Band 120-130 37,9 % | GRUEN (Gegenprobe faellt) |

## (I) Schwanz-Rest 7b: Glaette der Unterseite (echt)

Eigenes Mass (P/a/glatt7.py): 5x5-Fenster ganz in der Klasse (2 px erodiert), mittleres L* >= 10, F263-F398, Klassen aus SW (Zeile 495) und KO (Mesh 8):

| Stand | Unterseite rel. Streuung Median (Mittel) | sd L* | L* Median | Kanten-Anteil grad >= 6 | Rumpf rel. / L* |
|---|---|---|---|---|---|
| NEU | 0,120 (0,130) | 2,06 | 16,9 | 0,005 | 0,180 / 39,2 |
| 6d | 0,070 (0,092) | 1,16 | 16,5 | 0,009 | 0,189 / 38,0 |

- GRUEN: die Unterseite ist deutlich weniger glatt als 6d (+71 % rel. Streuung, +78 % sd L*), ohne neue Naehte/Streifen (Kanten-Anteil 0,005 statt 0,009; Kanten
  495/487 mit da* >= 15: 1 statt 6; Grenze Fleisch/grau echt da* 8,0 statt 9,6, dL 11,1 statt 10,0). Wurzel F312-F324: Deckel wie 6d, sonst weniger.
- Band 0,171-0,196: NICHT richtig verankert. Es kommt von Rumpf- und Beinhaut (andere Textur, doppelt so hell: L* 39 gegen 17); die relative Streuung haengt
  am mittleren L*. Das Original-Fleisch selbst lag nach der eigenen Messung des Bauers offline bei 0,154 (unter dem Band). Ein Fleisch-Bezug muesste das
  Original-Fleischfeld (Zeile 482) in vergleichbarer Helligkeit sein.

## (E) nicht schlechter als r5d

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| P1 Kanten-Sprung Brust/Flanke | <= r5d | echt 10,93 (r5d 11,24, 6d 10,62); unbeleuchtet P1 10,90 (r5d 11,14) | GRUEN |
| P2 S2 (c) | <= 3 px alle Varianten | 2,0 in 7 von 7 | GRUEN |
| P3 Schulter/Oberarm-Rot | wie r5d | 51,2 % gelichtet (= 6d, Vorlage 50,5) (P/a/schulter4.txt) | GRUEN |
| O1 Flecken | <= r5d | 6,01 je 1000 px (6d 5,90, r5d 6,18) (P/a/s7lit3.txt) | GRUEN |

## Bauer-Angaben gegen eigene Messung

- Kette, Pin, Tests 8/8 und 1013/1013, state.log = V7, Mesh 9 = V7, Framedumps = 7c: bestaetigt.
- S16 "echt vorn 18 / 1041 gegen V7 17 / 1306, zusaetzlich F312": bestaetigt; F312 ist nachweislich die Armhaltung aus 6c (E12).
- S18 "Gesichts-Bedingung E10 verletzt (Mesh 8 verdeckt PV+1024 4 px, P2+2048 2 px)": NICHT bestaetigt - eigenes Mass: Mesh 8 verdeckt 0 Gesichts-Bildpunkte in 177 Ansichten und in F225-F450.
- S14 "1804 fast weisse Bildpunkte offline": Richtung bestaetigt (echt L* > 75: 7321 px F344-F378, 6d 0), aber alle auch in r4 und links neben dem Kopf, nicht auf der Brust; nach E12 GELB.
- S17 "Spitzen 7,0-10,0 in allen Paar-Posen": eigenes Mass h132 1,0 / echt 2,0 px. "Daumenlaenge 16,7-19,4": eigen 21,2-24,0 offline, 20,1-22,3 echt (andere Regel).
- S9 "F354 5 px": eigen 0 Port-px in echt F354.
- S19 "d65 0,46, v13 2,42": bestaetigt als Erbe (t65 0,46, q13 0,44/4,35 in allen Staenden seit r5d).
- S20 "0,87 Grad": eigen -0,72 Grad (anderes Mass), Richtung gleich.
- Glaette "0,145, Band nicht erreicht": eigenes Mass 0,120 gegen 6d 0,070; Band fehlverankert (siehe I).

## Grenzen

- Hoehen der Hand im echten Bild nicht direkt: Fuesse am Bildrand abgeschnitten; Hoehen offline, Lage offline/echt <= 2 px verglichen.
- Daumen- und Spitzenmass sind eigene Regeln (Gabel = erste Station ab 25 % der Hand mit zwei Laeufen und Luecke > 1,5 px, 2 px lang); Zahlen des Messblatts abweichend.
- Die Erreichbarkeit von Clip 20 mit der Laborform ist aus dem Code abgeleitet (Sub 6), nicht im Spiel ausgeloest.
- "Zacke + V" ist mit einer Regel gemessen (Kerbe rechts neben dem Kopf, Mesh-8-Spitze >= 3 px ueber dem Kerbengrund), die R2/r4/6d trennt; eine Sicht-Pruefung ersetzt sie nicht.

## Dateien (P = sfv8/pr7)

P/stand.txt, P/kette_a.txt, P/kette_b.txt, P/build.txt, P/ctest_gruppe.txt, P/ctest_unit.txt, P/laeufe/SPIEL/state.log, P/off/luecke_*.txt, P/off/split.txt,
P/laeufe/echt_out.txt, P/laeufe/echt_c.txt, P/laeufe/zacke_rechts.txt, P/laeufe/orange7*.txt, P/laeufe/weiss75*.txt, P/k_weiss_F354.png, P/vorn/*.txt,
P/a/*.txt (form_V8, jitter, s5s7s8, s6, kette3d, herkunft4, kappe6, p4c, p4d, p4kante, p1b, p1c, weiss, s7lit3, huegel3, schulter4, loch5, s9szene5, s10r5, sc, sw, glatt7),
P/h/hand6a.txt, durch6.txt, hand6d.txt, hand6b_*.txt, hand7_*.json, P/f6/f6_*.txt.
