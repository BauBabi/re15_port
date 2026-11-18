# SF Sichel-Form V8 Runde 6 - Zahlen-Pruefung (eigener Code)

Stand: Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8`, Zweig r41/sichel_v8, HEAD 3d2d230f5.
Asset `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`, 274644 Bytes, sha256 e9e71bca28f4f8596b888436f4b02406b359ff6a6067aaf21fb344e142c90e83 (= Pin `release/em30_hangar.sha256`).
Pruefer-Ordner P = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr6/`.
Vergleichsstaende in P: V7.EMD, R4.EMD (r4), R5.EMD (r5d), R6B.EMD, V8.EMD (r6). Die Sichel-Form ist Port-Wahl; es werden keine Original-Adressen behauptet.
Riegel und Messwerkzeuge der Bauer wurden nicht als Beleg benutzt; wo ihre Zahl abweicht, steht beides da.

## Ergebnis: NICHT BESTANDEN

ROT (fuenf Punkte):

1. (C) Luecke Ruecken/Kopf: offline 59 von 177 Ansichten ueber Soll (21 px / 7 tief / 3,2 px), in echten Framedumps 49 von 144 Bildern (r5d: 1 von 177 bzw. 1 von 144). Nur 2 Bilder sind durch die Einzelbild-Regel E3 gedeckt.
2. (C) Vorn ohne Kragen, Umriss um den Kopf: Mesh 8 ragt in 12 von 136 Vorderansichten mehr als 2 px ueber r4 hinaus (bis 7,0 px, F315-F319 zusammenhaengend).
3. (C) Helle Bildpunkte im Kasten (420,200)-(640,400), F344-F378: 81..112 je Bild gegen V7 69..78; 35 von 35 Bildern ueber dem V7-Hoechstwert.
4. (C) Texeldichte der neuen Flaechen: 3 Dreiecke mit 0,43- bis 0,46-facher Dichte (Soll 0,5 bis 2), dazu 1 Viereck-Haelfte mit Streckung 2,00 an der Grenze.
5. (A) S3 Kopf-Linie K: 3,4 px in der Grundvariante (r5d 3,0); der eigene S3-Riegel faellt in 5 von 7 Halbpixel-Varianten, r5d bestand in 7 von 7.

Ohne ROT sind die Hand (B) und (F), der Schwanz (D, vier GELB) und (E, vollstaendig GRUEN).

## (A) wie Runde 5

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Kette zweimal gebaut, Pin, Bau | bytegleich, Pin stimmt | zwei Laeufe bytegleich = Asset = Pin; LOCAL-BUILD-OK (P/kette_a.txt, kette_b.txt, build.txt) | GRUEN |
| Tests | 8/8 und 1013/1013 | ctest-Gruppe 8/8 (307,08 s), Unit-Suite 1013/1013 (327,63 s), nacheinander (P/ctest_gruppe.txt, ctest_unit.txt) | GRUEN |
| Szenen-state.log | bytegleich V7 | sha256 e48e1cad... = V7-Lauf = r4 = r5d | GRUEN |
| Code-Diff seit 5820af00f | nur Werkzeuge/Asset/Pin/Tests/Belege | ausserhalb analysis/ und tools/r41_sichel nur Asset, re15_port/tests/unit/test_birkin_formen.c, release/em30_hangar.sha256 | GRUEN |
| Gesicht (Mesh 9, CLUT-Zeile 486) | bytegleich V7 | Mesh 9 4280 Bytes gleich, Zeile 486 gleich, 6379 Texel gleich, Keyframe-Winkel Knochen 9 gleich (P/s5s7s8.txt) | GRUEN |
| S1 Rueckenlinie | Mittel <= 3, Max wie r5d | Mittel 2,23, Max 6,0; 7 Varianten 2,23..2,41, Max 6..7 (= r5d) | GRUEN |
| S2 (c) Rumpf ueber Schwanz | <= 3 px in allen Varianten | 2,0 px in 7 von 7 Varianten; S2 (a) 2,09..2,64 wie r5d (dieselben 3 Varianten fallen in r5d und r6) | GRUEN |
| S3 Kuppe/K | wie r5d (K 3,0) | K 3,4 (Varianten 2,6..3,4), Sattel 2,0 px bei x 445 (r5d 0,9); eigener Riegel 5 von 7 Varianten nicht ok, r5d 7 von 7 ok. Bauer-Zahl K 3,4 bestaetigt | ROT |
| S4 Schwanz-Dicke | monoton | Umriss des Schwanzes gegen r5d: 2..21 von rund 4500 Bildpunkten verschieden (PV, P1, P2, F300, F350). Mein Ring-Sucher meldet nicht monoton, weil die 12 neuen Naht-Ecken in Mesh 18 als eigener Ring 10,8 erkannt werden - Messfehler des Werkzeugs, kein Formfehler | GRUEN |
| Boden S6 | 0 Keyframes unter dem Boden | 0 von 912 (V7: 70; groesster Ueberstand -25) | GRUEN |
| Wurzel S10 (echte Framedumps) | 0 px | 0 px in F300-F380 (R3-Altstand: 69114 px) | GRUEN |
| Schlitz (echte Framedumps, eingeschlossene Fremd-Bildpunkte) | nicht schlechter als r5d | oberste 40 Zeilen der Figur: 35 px (F340 15, F341 19); r5d 0, r4 1, V7 21. Oberste 120 Zeilen: 118 (r5d 235, r4 98, V7 268). Bauer: 10 px echt | GELB |
| Kappe (rote Bildpunkte am Kopf, unbeleuchtet) | nicht schlechter als r5d | a* >= 30 im Kopf-Umfeld: P1 21 (r5d 19, V7 29), PV 23 (r5d 24, V7 12), P2 9 (r5d 9). Kopf sichtbar P1 1272 px (= V7; r5d 1257), PV 1095 (V7 1120; r5d 1017) | GRUEN |
| Adern/Herkunft | 0 fremde Texel | Meshes 8/13/14/16-22: 0 Texel ungleich Original, 0 rote Texel nicht aus dem Original, 0 transparent; 1 Texel ausserhalb der Original-Nutzung in Mesh 8 (wie r5d) | GRUEN |
| Schwanz-Kette | Gelenk <= 25,5 Grad | Max 25,2 Grad, 0 von 912 Keyframes > 25,5; Glieder 541 E.; Szenenlauf Max 25,2 | GRUEN |
| Format S8 | gueltig | 23 Meshes, 23 Knochen, 21 Clips, 912 Keyframes, 274644 Bytes | GRUEN |
| S9, S11-S13 | - | nicht als eigene Nummern nachgestellt; die Themen sind ueber Stufen der Vorderkontur (C), Weiss/P1 (E) und Herkunft abgedeckt | GELB |

## (B) Hand und Unterarm in den echten Szenenposen (P7)

Vorlage selbst nachgemessen (birkin20/22/23/21/24, Video-px): Unterarm 43,6..50,5; Hand 57,4..68,1; Handgelenk 20,2 19,8 21,9 21,4 25,1; breiteste Stelle 38,3 38,8 44,6 47,0 50,7; Spitzen 9,6 7,6 6,6 10,6 10,5.

Breitenprofil, 8 Stationen (s1/s2 Unterarm, s3 Handgelenk, s4-s6 Hand, s5 breiteste Stelle, s7 Krallenwurzel, s8 Spitzen), aus P/profil6.txt:

| Paar | Vorlage s1 s2 s3 s4 s5 s8 | r6 s1..s8 | r5d = V7 s1..s8 |
|---|---|---|---|
| birkin20 / h125 (F368) | 33,1 34,1 20,2 25,7 38,3 9,6 | 33,0 36,0 20,4 31,6 43,7 43,0 20,4 3,0 | 39,6 39,6 31,1 32,6 33,3 33,3 10,5 2,2 |
| birkin22 / h126 (F369) | 29,2 34,7 19,8 27,2 38,8 7,6 | 33,0 33,0 19,2 31,6 43,9 43,9 20,4 3,0 | 38,3 39,3 28,5 29,8 30,7 29,8 9,0 6,0 |
| birkin23 / h128 (F371) | 34,7 33,1 21,9 32,3 44,6 6,6 | 34,0 36,0 20,2 31,4 45,5 43,6 20,4 3,0 | 40,0 40,0 13,4 13,4 34,0 33,3 9,2 4,2 |
| birkin21 / h130 (F373) | 36,1 34,6 21,4 35,0 47,0 10,6 | 33,1 34,1 20,2 32,3 44,4 43,4 21,2 2,0 | 41,5 40,6 33,7 34,0 34,0 34,0 11,4 2,0 |
| birkin24 / h132 (F375) | 35,4 41,0 25,1 35,1 50,7 10,5 | 33,2 36,2 20,1 31,3 44,4 43,4 20,2 1,0 | 39,9 39,9 32,0 33,3 33,3 32,3 11,4 2,0 |

Kontroll-Posen r6 (s3 / s5): h100 15,3 / 35,7; h110 17,1 / 39,5; h135 21,1 / 44,2 (h100 und h110 perspektivisch verkuerzt).

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Handgelenk-Breite | in der Streuung der Vorlage 19,8..25,1 | 19,2..20,4 (kleinster Wert 0,6 px unter der Spanne; Unterschied Betrachter/Spielbild 1,2 px). Im Paar birkin24/h132 5,0 px schmaler als das Paar-Bild. r5d 13,4..33,7 | GRUEN |
| breiteste Stelle | in der Streuung 38,3..50,7 | 43,7..45,5 (im Paar birkin24/h132 6,3 px schmaler); r5d 30,7..34,0 | GRUEN |
| Hand nicht kleiner als Vorlage | Hand 57,4..68,1, Unterarm 43,6..50,5 | Hand 67,3..68,7, Unterarm 48,7..50,1. r5d Hand 17,9..29,3 | GRUEN |
| Krallenspitzen-Breite | 6,6..10,6 | 1,0..3,0 (Bauer: 1-2 gegen 5,1..9,0, eigener Riegel S17 (a) ROT). Nicht Teil der ROT-Regel (B) | GELB |
| Daumen-Gabel | Vorlage 22,4..30,5 (Bauer-Mass) | Bauer 18,2..20,0; mit meinem Mass nicht vergleichbar nachgestellt | GELB |
| Krallen: Laenge / Wurzel-Dicke | - | Daumen 163 / 54 E.; Finger 220 / 150, 216 / 148, 215 / 131 (r5d 185 / 38; 320 / 128, 318 / 88, 220 / 67) | GRUEN |
| Krallen-Kreuzung | nicht schlechter als r5d | 29..50 px, r5d 79..102 px | GRUEN |
| CLUT-Zeilen der Krallen | Daumen nur 491, Finger nur 492, 0 fremde Texel | 491: 6 Flaechen, 492: 18 Flaechen; Flaechen-, UV- und CLUT-Bytes der Meshes 11/12 bytegleich V7, nur Ecken und Normalen anders; Textur und CLUT bytegleich | GRUEN |
| Texeldichte Hand | 0,5- bis 2-fach Original | 0,00488 = 1,78-fach Original 0,00274 (r5d 3,36-fach); Unterarm 0,00606 = Original 0,00607 | GRUEN |
| Gelenk ohne Luecke | geschlossen | Wurzelring ausserhalb der Unterarm-Huelle Median 14,2 E. (r5d 40,2; Original 91,3); im Spielbild Abstand 1,0 Port-px | GRUEN |
| Durchdringung gegen V7 und r5d | nicht schlechter | siehe (F4): 25 Keyframes gegen 20; schlechter nur in Clip 20 und einem Keyframe in Clip 4 | GELB |

## (C) P5-neu: Luecke, Kragen, Nackenstueck

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Luecke offline, 177 Ansichten x 7 Varianten | 21 px / 7 tief / 3,2 px | 59 von 177 ueber Soll. PV 19 / 6 / 18,4; P2 35 / 14 / 3,6; F311 204 / 147 / 11,4; F278 171 / 126 / 9,2; F310 164 / 115 / 9,2; F312 125 / 70 / 6,3. Bauer: 62 von 177. V7 40, r4 62, r5d 1, R6B 59 | ROT |
| Luecke in echten Framedumps F235-F378 | wie oben, E3 | 49 von 144 Bildern ueber Soll (roh 52); r5d 1. Nach E3 nur F315 und F320 Einzelbild, 47 Bilder nicht gedeckt; groesste F311 roh 234,2 / 168,9 / 11,3 (korrigiert 198,8 / 134,4 / 11,0), F310 183,6 / 125,6 / 9,4, F278 174 / 122,9 / 8,9 | ROT |
| Gegenprobe Luecke r4 | r4 muss ROT sein | r4 62 von 177 ueber Soll | GRUEN |
| Verdeckte V7-Gesichts-Bildpunkte | 0 | durch Mesh 8 oder Einsatz: 0 (offline und echt). Durch den Oberarm (P7-Pose): F312 10 px offline bzw. 5 bis 15 Port-px echt; F311, F313, F314: 0. Bauer: 25 px in 4 Ansichten, F312 18 px - eigene Zahl 11 px in 2 Ansichten | GELB - Einzelbild |
| Gegenprobe Kragen r5d | r5d muss ROT sein | r5d verdeckt 507 Gesichts-Bildpunkte in 142 Ansichten, alle durch Mesh 8 | GRUEN |
| Umriss um den Kopf (vorn ohne Kragen) | hoechstens 2 px ueber r4 | Mesh 8 ueber r4: 12 von 136 Vorderansichten > 2 px: F315 bis F319 5,0..7,0 px, F365 2,8, F270/F271/F285/F286/F302/F303 je 3,6 px; Flaechensumme 752 px (r5d 52 von 136, bis 10,0 px, 7876 px). Bauer-Riegel S18 (a): 116 von 136, anderes Mass | ROT |
| Helle Bildpunkte im Kasten, F344-F378 | wie V7 (69..78) | 81..112, Median 96; 35 von 35 Bildern ueber 78. Bauer-Zahl 81-112 bestaetigt. Vorbehalt: r4 (ohne Kragen) 89..111, r5d 95..130 - das Mass trennt den Kragen nicht sauber | ROT |
| Texeldichte / Streckung neue Flaechen | 0,5- bis 2-fach, Streckung <= 2 | gegen r5d neue Flaechen (41 Flaechen, 55 Dreiecke): 3 Dreiecke Dichte 0,43 / 0,46 / 0,46 (t65, t8, t85) und q8 Streckung 2,00; 7,8 % der neuen Flaeche. Bauer: 3 Dreiecke, bestaetigt | ROT |
| Normalen-Sprung Nackenstueck | kein Sprung | 0 Grad an allen 76 Ecken von Mesh 8 (je Ecke ein Normalenwert); Normale gegen Flaechennormale an den neuen Flaechen 90 % 59,1 Grad, max 85,6 (r4 46,8 / 81,6; r5d 46,9 / 89,1) | GRUEN |
| Buckel-Rot von vorn | >= 50 % | 57,6 % (r5d 45,8 %; Vorlage 58,3 %) | GRUEN |
| Stufen der Vorderkontur (echte Framedumps F344-F380) | nicht schlechter als r4 | 9 Bilder mit mehr als 3 Port-px ueber V7 (r4 16, r5d 14); kein Bild mehr als 2 px ueber r4; neue Ecken in 2 Bildern (r4 4, r5d 12). F354: 0 px (Bauer: 5 px) | GRUEN |

## (D) P4-neu: Schwanz-Unterseite

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| graue Deckel-Flaechen auf der Unterseite | 0 | 7 Dreiecke, alle Wurzel-Deckel in Mesh 16, im Rumpf: sichtbar 1 px in 1 von 177 Ansichten. Viereck-Haelften 0. r5d: 91 Dreiecke | GELB |
| Kanten Fleisch/Grau | 0 | 51 Kanten 495/487 (r5d 122), davon 6 mit Farbsprung da* >= 15, alle Mesh 16 (r5d 38) | GELB |
| Szenenbilder mit grauem Ring (echte Framedumps) | 0 | 2 Bilder mit kontrastreicher Grenze Fleisch/Deckel (F312 33, F313 26 Grenz-px; graue Flecken 306 und 315 Port-px), 8 Bilder mit irgendeiner Grenze; r5d 26 bzw. 29 Bilder; Deckel sichtbar 3677 px in 57 Bildern gegen r5d 17059 px in 78. Ob diese Deckel auf der Unterseite liegen, konnte ich nicht zuordnen: meine Offline-Pose weicht in F311-F313 stark vom echten Bild ab | GELB |
| Anteil Zeile 495 je theta-Band | 100-110 / 110-120 rot, 120-130 hoechstens 5 % | Flaeche (PV): 94,4 % / 79,8 % / 0,3 %. Bild PV 7 Varianten: Band 120-130 0,0 %. Bild P2: 8,8..10,8 % (Flaeche 4,2 %; dort ist das Winkelmass unsicher). r5d 37,9 % | GELB |
| Herkunft Schwanz-Texel | 0 fremde | Meshes 16-22: 0 Texel ungleich Original, 0 ausserhalb der Original-Nutzung | GRUEN |
| Oberseite grau | 0 % rot | theta >= 135: 0,00 % Zeile 495 | GRUEN |
| Gegenprobe r5d | r5d muss ROT sein | r5d: 91 graue Dreiecke unten, sichtbar 9327 px in 145 von 177 Ansichten, Band 120-130 37,9 % | GRUEN |

## (E) nicht schlechter als r5d

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| P1 Kanten-Sprung Brust/Flanke | <= r5d | unbeleuchtet Betrag dL 10,46 (r5d 11,14); echte Bilder 18 Bilder Betrag dL 10,62 (r5d 11,24); fast weiss 0 px | GRUEN |
| P2 S2 (c) | <= 3 px in allen Varianten | 2,0 px in 7 von 7 | GRUEN |
| P3 Schulter/Oberarm-Rot (Zuordnung P3 von mir so gelesen) | wie r5d | 51,2 % = r5d | GRUEN |
| O1 Flecken | <= r5d | 5,90 Flecken je 1000 px (r5d 6,18) | GRUEN |

## (F) Handstellung

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| F1 Verdrehung gegen Capcoms Original | 0 Grad | Mesh -0,04 Grad; ueber 912 Keyframes Median 0,04 (min -0,08, max 0,17). Gegenprobe R6B -117,98. Bauer -0,02 | GRUEN |
| F2 Handseite zur Kamera | Handruecken wie beim Original | offline 86,3 % Handruecken (Original 84,2 %, r5d 72,7 %, R6B 47,7 %); echtes Spielbild 90,2..93,8 % (Original 91,4..92,5 %, R6B 24,2..28,0 %). Bauer 82,8 % | GRUEN |
| F3 Ketten-Masse je Paar | in der Vorlagen-Spanne; Toleranz mindestens Unterschied Betrachter/Spielbild | Hoehen in Koerperhoehen: Ellbogen 0,633..0,640 (Vorlage 0,627..0,648), Handgelenk 0,476..0,489 (0,461..0,501), Krallenspitze 0,267..0,282 (0,257..0,319); je Paar bis 0,041 H an der Spitze bei Vorlagen-Streuung 0,062 H. Betrachter/Spielbild: Lage <= 2,0 px, Handgelenk <= 1,2 px, breiteste Stelle <= 1,0 px | GRUEN |
| F4 Gelenk und Durchdringung, 912 Keyframes | nicht schlechter als V7/r5d | Keyframes mit Tiefe > 20 E.: V7 = r5d 20, R6B 57, r6 25, Original 38 (Bauer 68 / 96 / 30, anderes Mass, gleiche Reihenfolge). Schlechter als r5d: Clip 20 Bild 12-59 (24 Keyframes) Rumpf bis 264 E. (KF 822); Clip 4 Bild 42 (KF 230) Krallenspitze 27 E. unter dem Boden. Besser: Clips 5, 9, 11, 18. Clip 20 ist mit dieser Form nach Code nicht erreichbar; ein spaeteres Member_set habe ich nicht ausgeschlossen. Der Bauer nennt Clip 14 Bild 18 - falls hexadezimal 0x14 gemeint ist, ist es derselbe Clip | GELB |
| F5 nur Erlaubtes geaendert | Meshes 11/12; Keyframes Knochen 11, Knochen 10 nur mit Vorlagen-Beleg | R6B zu r6: nur Meshes 11, 12 und Keyframes Knochen 10 (888 KF) und 11 (889 KF); Textur, UV, CLUT der Hand bytegleich; state.log gleich V7. Der Beleg fuer Knochen 10 im Dossier (SF_sichel.md Z. 3859-3878, 3943-3944) stuetzt sich vor allem auf die Durchdringung, das Vorlagen-Argument ist schmal | GELB |
| F6 Riegel S17/S20 ehrlich | Varianten muessen fallen | Verdrehung 30 / 60 / 118 und Beuge 60: alle ROT in S20; r6 S20 GRUEN, S17 ROT in (a) und (d) | GRUEN |

Ellbogen-Beuge mit eigener Winkeldefinition: r6 Median 60,0 Grad gegen 95,9 (V7, r5d, R6B) und 41,4 (Original). Der Bauer nennt 60 nach 20; die Winkel sind anders definiert, die Richtung stimmt.

## Bauer-Angaben gegen eigene Messung

- Luecke 62 von 177: eigen 59 von 177 (gleiche Aussage).
- Verdeckte Gesichts-Bildpunkte 25 in 4 Ansichten: eigen 11 in 2 Ansichten, alle durch den Oberarm, keine durch Mesh 8.
- Heller Keil 81-112: bestaetigt.
- Nackenstueck in 116 von 136 Vorderansichten: mit meinem Umriss-Mass 12 von 136 ueber 2 px.
- 3 Dreiecke ausserhalb der Texeldichte: bestaetigt.
- S3 K 3,4: bestaetigt. S9 F354 5 px ueber V7: nicht bestaetigt (eigen 0 px in F354).
- Schlitz 10 px in echten Framedumps: eigen 35 px in den obersten 40 Zeilen (anderes Fenster).
- Durchdringung 68 / 96 / 30: eigen 20 / 57 / 25 (Schwelle 20 E., echte Flaechen); Reihenfolge gleich.
- Tests 8/8 und 1013/1013, state.log bytegleich V7: bestaetigt.

## Grenzen dieser Pruefung

- Die Offline-Szenenposen treffen das echte Bild in schnellen Bildern schlecht (F311: 25161 von 48775 Bildpunkten Abweichung; F300 6646 von 61125; F350 2755 von 70383). Fuer Szenenbilder zaehlen deshalb die echten Framedumps; die Offline-Zahlen der Bilder F310-F319 sind nur Anhalt. Das betrifft auch den Kragen-Umriss in F315-F319.
- Das theta-Mass am Schwanz gilt nur in Posen mit kleinem Ebenen-Rest (PV, P2); in Szenenposen ist es unbrauchbar.
- Das Mass der hellen Bildpunkte ist nicht kragenspezifisch (r4 ohne Kragen liegt ebenfalls in 35 von 35 Bildern ueber V7).
- Daumen-Gabel nicht mit vergleichbarem Mass nachgemessen. P3 habe ich als Schulter/Oberarm-Rot gelesen. S9 und S11-S13 nicht als eigene Nummern nachgestellt.
- Ein spaeteres Member_set auf Clip 20 fuer diese Form wurde nicht ausgeschlossen.

## Dateien

Alle Ausgaben in P: luecke_V8.txt, ko.txt, ko2.txt, c6a.txt, c6b.txt, c6c.txt, c6d.txt, kappe6.txt, p4c.txt, p4d.txt, p4kante.txt, sc.txt, sw.txt, hand6a.txt, hand6b_echt.txt, hand6c.txt, hand6d.txt, hand6e.txt, profil6.txt, durch6.txt, zensus.txt, form_V8.txt, jitter.txt, s5s7s8.txt, s6.txt, s10r5.txt, loch5.txt, s9szene5.txt, p1b.txt, p1c.txt, s7lit3.txt, huegel3.txt, schulter4.txt, herkunft4.txt, kette3d.txt, szene_eigen.txt, port.txt, weiss.txt, kette_a.txt, kette_b.txt, build.txt, ctest_gruppe.txt, ctest_unit.txt.
