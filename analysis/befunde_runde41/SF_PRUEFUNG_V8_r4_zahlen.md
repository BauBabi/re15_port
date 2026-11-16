# SF Pruefung V8 Runde 4 - Zahlen-Pruefer (2026-11-09, nicht committet)

Pruefgegenstand: Zweig r41/sichel_v8, HEAD 282e4d594 (Commits a5afacd57, 397c323cb, 457b82aba, 6abb117a8, 6ced0be88, 06b70b094,
282e4d594; Diff gegen 100900c97 = V8 r3).
- Spiel-Asset EMD/EM30_HANGAR.EMD: sha256 10d021738731e7be2bf6ac79de7da77637cc16f6c08f640073dd4d1242d1843f (271 012 Byte).
  Das ist gleich dem Pin release/em30_hangar.sha256 und gleich git show HEAD.
- Gegenproben (git show):
  - V7 = 6a25db27b (319bbd17)
  - V8 r2 = 3db35d865 (d0ef852c)
  - V8 r3 = 100900c97 (66806bc4)

**Ergebnis: NICHT BESTANDEN.** Rot ist genau ein Punkt:
- **S2 (c) Naht Rumpf/Schwanz in PV.** Eigene Messung: 8,0 px bei x 376. Soll <= 3. V8 r3 lag bei 3,0, V8 r2 bei 3,0.
  - In allen 7 Halbpixel-Varianten liegt der Wert bei 7-8 px.
  - Der Bauer meldet denselben Punkt selbst ROT (4,0 px, nach anderer Fensterregel) und legt ihn dem Orchestrator vor (Dossier Offen 1).

Alle uebrigen Vorgaben O1-O8 und die Punkte S1-S8 sind in meiner eigenen Messung im Soll. Gelb sind nur Schwellen- und Rueckschritt-Werte.

## Verfahren
Eigener Pruefer-Code liegt in C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr4/.
- Herkunft: pl.py, form.py, form_j.py, s5s7s8.py, s7r3.py, s7lit3.py, s6.py und kette3d.py sind eigener Code aus Runde 1-3, unveraendert.
- Nichts davon ist aus re15_port/tools importiert. riegel_v8.py wurde nicht gestartet.
- Neu in Runde 4:
  - herkunft4.py: Texel unter 493-495 gegen die Nutzungsmaske des Originals je Zeile; Feld C.
  - schulter4.py und schulter4b.py: Feld-C-Gruppe in PV gegen die Vorlage.
  - s9j.py: Ecken in P1 mit Halbpixel-Verschiebung.
  - mk.py: Marker-Assets.
  - s10r4.py, loch.py, Kappe P1.
- Marker-Assets (mk.py, eigener Code):
  - MK_A: Zeile 487 (nur Schwanz) magenta.
  - MK_B: wie MK_A, dazu die 22 Wurzelflaechen von Mesh 16 auf Zeile 480. Das sind v0-v10 (Band Ring 0 -> Ring 1, jetzt entartet, weil Ring 0 = Ring 1) und d11-d21 (Kegel). Das Alpha-Muster 487 gegen 480 weicht unter diesen Flaechen an 0 Texeln ab. 44 Byte unterscheiden MK_A und MK_B.
  - GR: alle 16 Zeilen gruen.
- Laeufe (laeufe.sh, lauf_v4.sh mit BAUM r41_sichel_v8, eigene exe-Kopie, nacheinander):
  - Szene: FD 200-1100, Ende 1110.
  - MK_A_V8, MK_B_V8, GR_V8: FD 300-380.
  - Die Zeilen "Schwanz B y" der Markerlaeufe sind gleich dem normalen Lauf (0 Unterschiede).
  - Die Gegenproben V8 r2 und V8 r3 in den Framedumps sind meine Laeufe aus Runde 3 (sfv8/pr3, gleiche exe, unveraenderte Engine).

## Soll/Ist

| Punkt | Soll | V8 r4 (eigen) | V8 r3 | V8 r2 | V7 | Bewertung |
|---|---|---|---|---|---|---|
| S1 Kontur T | <= 4 / <= 8 | 2,23 / 6,0 (Halbpixel 2,23-2,41 / 6-7) | 3,05 / 7 | 1,82 / 5 | 16,77 / 50 | GELB: im Soll, aber nicht ganz der zu haltende r2-Wert |
| S2 a / b | <= 4 / 6; <= 2 | 2,20 / 5; 1,7 bei x 333 (Halbpixel b 2,0-2,3 in 3 von 7) | 2,02 / 5; 1,6 | 1,70 / 5; 1,7 | 7,30 / 13; 6,6 | GRUEN (b fragil) |
| **S2 c Naht** | <= 3 | **8,0 bei x 376** (Naht x 361; alle 7 Varianten 7-8) | 3,0 | 3,0 | 7,0 | **ROT** |
| S3 Kuppe / H / Sattel / K | <= 4; 5-10 bei x 428-445; <= 2; <= 3 | 4,0; 7,6 bei x 429; 2,0 bei x 445; 3,0 | 4,0; 6,6/429; 1,5; 3,0 | 4,0; 6,8; 1,5; 3,0 | rot | GRUEN, aber jede Schwelle ist knapp: in 4 von 7 Halbpixel-Varianten Sattel 2,7, in 3 von 7 H bei x 426/427 |
| S4 / O4 Breite | Wurzel >= 55; s 90-130 >= 36; unten >= 27 / >= 9; Ende <= 10; monoton | 58,9; 40,4 / 38,2 / 37,9; 31,3 / 30,9; 18,8 / 18,5 / 11,1 / 11,0; 5,6; monoton | 60,6 ... | unten 20,4 | rot | GRUEN |
| O4 Bildbreite des Schwanzes in PV | Vorlage bei y 270 x 282-314 | y 270: x 283-315 = 33 px; y 280 24; y 290 17; y 300 12 | 31 / 24 / 19 / 13 | 21 / 13 / 9 / 9 | - | GRUEN |
| S5 Gesicht | bytegleich V7 | Mesh 9 4280 Byte gleich, Zeile 486 gleich, 6379 Texel gleich, Knochen 9 (rel, Eltern, Keyframes) gleich | gleich | gleich | - | GRUEN |
| S6 Boden | 0 / 912; Szene y <= 0 | 0 / 912 (max -25 E.); Szene 144 Takte, tiefstes y -122, 0 Takte y > 0 | 0; -118 | 0; -66 | 70 / 82 | GRUEN |
| O6 Gelenk 0 h69 / h70 | <= 25 Grad | 24,9 / 25,1; Szene Gelenk max 25,2; 0 Takte > 25,5 | 25,1 / 25,0 | **31,7 / 39,3** | - | GRUEN (0,1 Grad = Rundung der ganzzahligen Log-Koordinaten) |
| S7a / O1 Herkunft | jeder Eintrag und Texel aus dem Original-EM30 | Zeilen 493/494/495 = Original 480/481/482 (0 abweichende Eintraege). 163 Flaechen auf 493-495 (Mesh 8 70, 13 56, 14 37), alle mit passender Original-Zeile. 36 585 Texel darunter: 0 ungleich Original, **0 ausserhalb der Texel, die eine Original-Flaeche derselben Zeile zeigt**, 0 durchsichtig | 210 Texel ausserhalb | - | - | GRUEN |
| O1 kein Feld C | 0 | 0 Texel der neuen Flaechen in Feld C (Seite 0, u 78-127, v 0-100). Die 24 Feld-C-Flaechen von Mesh 13 haben einen gemeinsamen Schub (-66, +5). | 0 | - | - | GRUEN |
| O1 Schulter (Nutzer V5 "roter Fleck oben auf der rechten Schulter") | kein Fleck | Feld-C-Gruppe in PV (3027 px): a* >= 30 nur 6 px (groesster Fleck 2 px), oberste 20 Zeilen 0 px. a* >= 10 51,2 % (Vorlage am selben Ort 50,5 %), Median a* 10,4 (Vorlage 10,0) | 279 px, Fleck 219 | - | 279 px, Fleck 219 | GRUEN |
| O1 Brust wie V7 | 34 Flaechen 488, UV = V7 | 34 auf 488, UV 34/34 = V7 | gleich | - | - | GRUEN |
| O1 Oberarm 13 | 35-59 % UND <= Original 57,5 | **48,8 %** (roh 48,2) | 24,3 | 43,0 | 22,9 | GRUEN |
| O1 Rumpf 8 (W)-rechts | 25-80 UND <= Original 49,4 | lok z < 0: **42,8 %**; lok x < 0 (O1-Wortlaut): 49,6 % (Original 60,1) | 62,2 | 5,0 | 0,2 | GRUEN |
| O1 Unterarm 14 | <= Original 22,1; kein Band | 1,0 % (roh 16,1; Original roh 32,6). 33 von 37 Flaechen haben Original-UV und -Zeile; die 4 Leisten-Flaechen v9/v10/d19/d22 zeigen andere Original-Unterarm-Texel (Skalierung 0,6, Schub (11, -63)). Rote px auf Knochen 14 mit a* >= 15, ungelichtet: 144 (Original 392) | 20,4 | 17,4 (Band) | 0,0 | GRUEN; GELB fuer den O1-Satz "Original-Faerbung seiner eigenen Flaechen": in 4 Flaechen bewusst nicht, damit das Band entfaellt |
| O1 Flecken (a* >= 15, >= 4 px, ohne Licht) | je 1000 px <= Original | 8+13: 45 (5,94; Original 27 = 6,09); 8+13+14: 47 (4,73; Original 31 = 5,03) | 6,02 / 5,19 | 3,49 / 3,26 | 2,13 | GRUEN je Flaeche; GELB absolut (45 gegen 27, der V8-Rumpf ist in PV groesser: 7570 gegen 4435 px) |
| O2 Huegel rot | oberste 8 px x 428-445, a* >= 10, wie Vorlage 58,3 % | 58,3 % (Median a* 11,0; Vorlage 12,1). Die Huegel-Flaeche v8 hat den Schub (54, 2) auf Texel von Original-Flaechen mit Zeile 480 | 78,5 | 0,0 | 0,0 | GRUEN |
| S8 Technik | <= 16 Zeilen; <= 4096 | 16 Zeilen, Hoehe 4096, Felder 480-495 gueltig; 23 Meshes, 23 Knochen, 21 Clips | gleich | - | - | GRUEN |
| O3 Kontur von vorn P1 | <= 2 px ueber V7; keine neue Ecke | 2 px (x 454), Flaeche 12 px. Eine "Ecke" bei x 459 liegt genau auf der Schwelle 2,5 (V7 dort 2,0) und ist in 8 von 8 Halbpixel-Varianten weg | 2 px | 17 px, 10 Ecken | Referenz | GRUEN (Schwellenfall) |
| O3 Kappe P1 | 0 | a* >= 30 ueber Kopf+5: 0 px (a* >= 20: 50) | 309 | - | 0 | GRUEN |
| O3 Schlitz in den echten Framedumps F340-F380 | wie V7 | oberste 40 Zeilen der Figur: 1 px (V7 21, V8 r3 478, V8 r2 132); Ueberschuss gegen V7 0. Oberste 120 Zeilen: Ueberschuss 59 px in F347-F349/F353, alle bei x 483-485 / y 341-346, also 115 Zeilen unter dem Scheitel und nicht am Kopf | 478 | 132 | Referenz | GRUEN |
| O3 Kontur von vorn in der Szene, F344-F378 (960 x 720) | - | max 7 px ueber V7 (F352, x 619; = 4,7 px bei 640), 14 Bilder > 3 px, neue Ecken in 4 Bildern (F349/F350 x 582: 3 px hoeher als V7). Die Ueberhoehung liegt rechts neben bzw. hinter dem Kopf (x 575-622, Scheitel V7 x 546). Kein Kamm, keine rote Platte (Tafel vorn_r4.png) | max 4, 5 Bilder, 13 mit Ecke | max 27, 35, 25 | Referenz | GELB: Rueckschritt gegen r3 bei der Hoehe; weniger Ecken |
| S10 / O5 Wurzel von hinten | 0 | **0 px** in F328-F346 und in F300-F380 (z-Test MK_A gegen MK_B) | 46 090 | 48 331 | - | GRUEN. Bildvergleich s10_hinten_r4b.png: kein dunkler Keil mehr am Ansatz (V8 r3 daneben mit Keil) |
| Szene / state.log | gleich V7 | EXIT 0, 901 Bilder; state.log 1131 Zeilen bytegleich V7 (sfv4/r/V7_szene) und V8 r3; "Szene Ende: Cut 4", "23 meshes, 23 bones, 21 clips" | gleich | - | - | GRUEN |
| P2 Haken | keiner | Bild-Drehwinkel -17,9 / -12,1 / -18,7 / -4,8 / -6,1 / -20,8 (alle gleichsinnig), Spitze (272,330) | gleichsinnig | (271,339) | - | GRUEN |

## Kette, Pin, Tests, Code
- Kette:
  - v8_kette.sh mit eigenem Ziel pr4/kette: "V8-KETTE-OK sha256 10d02173...", EXIT 0, 31 s.
  - Stufe 6 = V7 319bbd17 OK.
  - kette/emd/EM30_V8.EMD ist bytegleich mit git show HEAD (cmp).
- Bau: "ninja: no work to do", "=== LOCAL-BUILD-OK (build)".
- Testgruppe (ctest -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole" --timeout 900): "100% tests passed, 0 tests failed out of 8" (305 s).
- Unit-Suite (-j 6, Ausschluesse wie beim Bauer): "100% tests passed, 0 tests failed out of 1013" (366 s).
- test_birkin_formen.c erwartet:
  - Mesh 8: 488:34, 493:56, 494:14, 480/481: 0, 10 UV-Schuebe
  - Mesh 13: 56 auf 493, 24 Schuebe
  - Mesh 14: 37 auf 495, 4 Schuebe
  - Das deckt sich Zahl fuer Zahl mit meiner Zaehlung (s7r3.py).
  - Die Gegenproben mit getauschtem Asset (V8 r3 rot, V7 rot) liegen als Dateien des Bauers vor (runde4/gegenprobe_*.txt). Ich habe sie nicht wiederholt, weil das Asset im Arbeitsbaum dafuer haette getauscht werden muessen.
- Code-Diff 100900c97..HEAD:
  - Kein C-Code von Engine, Plattform oder Include geaendert.
  - Geaendert sind nur test_birkin_formen.c sowie die Python-Werkzeuge und die JSON-Daten.
  - Keine neue "@0x"-Konstante; das einzige "@0x" im Diff ist der Satz "keine Zahl hier ist eine @0x-Konstante".
  - Die neuen Werkzeug-Konstanten haben Mess- oder Riegel-Kommentare (WURZEL_TIEF 15, ZUG 0,70/1,40, KEGEL -150, OBERARM_BAND 38-54, HAUT_ZIEL 0,40, BAND v9/v10/d19/d22).
  - GELB: Suchgewichte ohne Beleg-Kommentar: ruecken_r4.py W_SCHLITZ 2.0, W_BEDECKT 0.2, SCHRITTE.
- Labor 50E0: kein eigener Lauf. Der Kampf laeuft gruen in integration_r41_birkin_pistole.

## S2 (c) im Einzelnen (form.py, PV)
Oberkante je Spalte, Rumpf (Knochen 0-15) / Schwanz (16-22) / sichtbar:
- V8 r4:
  - Spalten: x 361: 192/191/191 | 367: 188/190/188 | 373: 188/192/188 | 376: 187/195/187 | ab x 379 kein Schwanz mehr.
  - Der Schwanz taucht also zwischen x 361 und x 377 unter die Rumpf-Oberkante und endet dort verdeckt. Die Rumpf-Oberkante liegt bis zu 8 px darueber.
- V8 r3: Der Schwanz lief bis x 397 oben mit (Abstand <= 3).
- Sichtbar bleibt die Kontur glatt und fallend: S2a 2,20/5, S2b 1,7, keine Erhebung ausser der Kuppe bei x 407.
- Nach der Regel S2c ("Rumpf ueber Schwanz an der Naht <= 3") ist der Punkt ROT.
- Der Bauer misst mit seiner Fensterregel 4,0. Auch das ist ROT.
- Ob S2c kuenftig nur an der sichtbaren Kontur gemessen wird, entscheidet der Orchestrator (Dossier Offen 1). Als Pruefer werte ich nach der geltenden Regel.

## Belege (sfv8/pr4)
- Formmessung: form_V8.txt, form_V7.txt, form_R2.txt, form_R3.txt; jitter.txt.
- Adern und Faerbung: s5s7s8.txt, s7r3_V8.txt, s7lit3.txt; herkunft4.py liefert seine Ausgabe in der Konsole.
- Boden und Kette: s6.txt, kette3d.txt.
- Front, Wurzel, Schlitz: s9.txt, loch.txt, vorn_r4.png, s10_hinten_r4.png, s10_hinten_r4b.png.
- Laeufe und Tests: szene/ (state.log, debug.log), MK_A_V8/, MK_B_V8/, GR_V8/, ctest_gruppe.txt, ctest_unit.txt, kette_lauf.txt.

## Fazit
- Rot:
  - S2 (c) 8,0 px (Soll <= 3; r2/r3 lagen bei 3,0). Der Bauer meldet den Punkt selbst offen.
- Gelb:
  - S1 2,23/6,0 statt r2 1,82/5,0.
  - S3 auf allen Schwellen und nicht halbpixelfest (Sattel 2,7 in 4 von 7 Varianten).
  - Szenenkontur von vorn bis 7 px (960) ueber V7, ein Rueckschritt gegen r3.
  - Flecken absolut 45 gegen 27.
  - Unterarm: 4 Flaechen absichtlich nicht Original-UV.
  - Suchgewichte ohne Beleg.
- Gruen:
  - O1: Herkunft, Feld C, Schulter, Brust, Oberarm 48,8, Rumpf 42,8, Unterarm 1,0, Flecken je Flaeche.
  - O2 58,3 %.
  - O3: P1 2 px, Kappe 0, Schlitz am Kopf 0.
  - O4 33 px bei y 270.
  - O5: Wurzel 0 px gegen 46 090.
  - O6 24,9/25,1.
  - S4, S5, S6, S8, P2.
  - Kette, Pin, Tests 8/8 und 1013/1013.
  - state.log gleich V7.

**bestanden = nein** (ein roter Punkt: S2c).
