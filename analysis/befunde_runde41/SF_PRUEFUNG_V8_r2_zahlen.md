# SF Pruefung V8 Runde 2 - Zahlen-Pruefer (2026-11-09)

Pruefgegenstand: Zweig r41/sichel_v8, HEAD 3db35d865 (Commits af8aa1f02, baf91e8aa, 6dcbb3604, 9cafd73ee, 3db35d865; Diff gegen 6a25db27b).
Spiel-Asset re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD sha256 d0ef852c9070c7a970b53301c961c059f02f166befb525792ecbd19705e2911a
(271 012 Byte) = Pin release/em30_hangar.sha256 = scratchpad sfv8/emd/EM30_V8r2.EMD. Referenz V7 = git show 6a25db27b (sha256 319bbd17),
Gegenprobe V8 Runde 1 = sfv8/emd/EM30_V8.EMD (fa31156c).

**Ergebnis: NICHT BESTANDEN.** S7c ist rot (5,4 % statt 25-80 %); das sagt auch der Bauer. Dazu kommen drei eigene Befunde:
- S7d ist rot, wenn man den neu umgestellten Unterarm (Mesh 14, (W)-rechts) mitzaehlt: 32 > 31.
- Der Huegel H ist nicht rot: 0,0 % gegen 58,3 % in der Vorlage. Bauplan 6b verlangt "rot".
- S3a sitzt genau auf der Schwelle und kippt bei einer halben Pixel-Verschiebung in 3 von 6 Faellen auf 5,0.

Alle anderen Zahlen des Bauers habe ich mit eigenem Code nachgemessen; sie stimmen.

## Verfahren (eigener Pruefer-Code, nichts aus re15_port/tools/r41_sichel importiert)
Ordner C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/pr2/:
- pl.py ist der Pruefer-Code aus Runde 1, unveraendert uebernommen:
  - eigener EMD-Leser (Verzeichnis, MD1, EMR samt Eltern, EDD, TIM/CLUT, EMS-Blobkette fuer das Original-EM30 Typ 0x30)
  - ganzzahlige Pose
  - RID-Kamera 50A0 Cut 12 aus der RDT
  - GTE-Projektion und Homographie SF_belege/kamera_50A0_12.json
  - z-Puffer-Raster mit Alphatest
- Mess-Skripte:
  - linien.py: Nutzerlinien aus Darstellung.png
  - form.py: S1-S4; form_j.py: S1-S4 mit Subpixel-Verschiebung
  - vorlage.py: Gegencheck der Vorlage-Kanten
  - s578.py: S5, S7a, S8 und Flaechenwechsel
  - s7lit2.py: S7b/c/d mit RDT-Licht und Videoraum-Regel
  - s6.py, kette3d.py, szene_eigen.py
- Ausgaben: starr16.txt, seiten.txt, zeilen_pv.txt, huegel_rot.txt, emd_scan.txt.
- Pose PV = Clip 17 Bild 47 Gier 587 x -8246 z -21266 (Messblatt V8 Abschnitt 1).

## Nutzerlinien (eigene Extraktion)
- T2 gruen: 848 px. T1 rot: 209 px (x 266-272, y 293-335).
- T laeuft von (372,180) bis (270,333) und ist geglaettet 219,0 px lang.
- J laeuft von (379,189) bis (357,241), H von (420,190) bis (445,206), K von (454,210) bis (463,244).
- Abweichung zum Messblatt V8 Abschnitt 2: hoechstens 1 px.
- Gegencheck der Vorlage-Kanten (vorlage.py): an 15 von 22 Stationen hoechstens 1 px. Die Ausreisser sind dieselben wie in Runde 1:
  s30, s80, s100 und die T+11-Stationen.

## Soll/Ist

| Punkt | Soll | V8 r2 (eigen) | V7 (eigen) | V8 r1 (eigen) | Bewertung |
|---|---|---|---|---|---|
| S1 Kontur T | Mittel <= 4, Max <= 8 | 1,82 / 5,0 | 16,77 / 50 | 3,05 / 7 | GRUEN (V7 reisst) |
| S2a Oberkontur x 330-385 | <= 4 / <= 6 | 1,70 / 5 | 7,30 / 13 | 3,70 / 7 | GRUEN |
| S2b Sattel x 300-420 | <= 2 | 1,7 bei x 382 | 6,6 bei x 355 | 3,7 | GRUEN |
| S2c Naht | <= 3 | 3,0 bei x 397 (Naht x 386) | 7,0 | 5,0 | GRUEN, genau auf der Schwelle, bei Halbpixel-Verschiebung stabil 3,0 |
| S3a Kuppe | <= 4 | 4,0 (x 405) | 8,0 | 3,0 | **GELB**: genau auf der Schwelle; bei +-0,5 px Verschiebung in 3 von 6 Varianten 5,0 (jitter.txt) |
| S3b Huegel H | 5-10 bei x 428-445 | 6,8 bei x 437 | 4,6 bei 427 | 3,0 bei 423 | GRUEN (Hoehe), Farbe siehe Befund 3 |
| S3c weitere Erhebung | keine | keine | keine | keine | GRUEN |
| S3d Sattel Ruecken -> Kopf | <= 2 | 1,5 bei x 430 | 6,4 | 2,3 | GRUEN (bei Verschiebung bis 2,1) |
| S3e K | <= 3 | 3,0 | 3,0 | 3,0 | GRUEN (Kopf unveraendert, Wert wie V7) |
| S4 Breite | Wurzel >= 55; s 90-130 >= 36; monoton; Ende <= 10 | 56,9; 38,8 / 37,9; ja; 5,9 | 44,3; 34,3 / 24,1 / 28,1; nein | 59,0; 42,6 / 40,0 / 40,3; ja; 6,0 | GRUEN (V7 reisst) |
| S5 Gesicht | bytegleich V7 | Mesh 9: 4280 Byte gleich; Zeile 486 gleich; 6379 Mesh-9-Texel gleich; relpos, Eltern und Keyframe-Winkel von Knochen 9 gleich | - | - | GRUEN |
| S6 Keyframes | 0 von 912 | 0 (Spitze/Dorn und jede Schwanzecke; mindestens 19 E. ueber der tiefsten Koerperecke) | 70 (Spitze/Dorn) / 82 (jede Ecke), bis 579 E. darunter | 0 | GRUEN (V7 reisst) |
| S6 Szene | alle Takte y <= 0 | eigener Lauf: 144 Takte, tiefstes Gelenk y -66 (h36), 0 Takte darunter | -108 | -66 | GRUEN |
| S7a Herkunft | Original-Eintraege und Original-Texel | Eintraege und Flaechen gemessen, siehe Abschnitt S7a | - | - | GRUEN |
| S7b Oberarm 13 | 35-59 % | 43,0 % (Bauer 42,7) | 22,9 % | 43,5 % | GRUEN |
| S7c Rumpf 8 | 25-80 % | **lok x < 0: 5,4 %; lok z < 0 ((W)-rechts): 5,0 %** (Bauer 5,3 / 5,0) | 0,0 / 0,2 % | 29,7 / 32,6 % | **ROT** |
| S7d Ruhe (Knochen 8, 13) | <= Original | 26 <= 27 (Bauer 26) | 15 | 18 | gruen |
| S7d Ruhe mit Mesh 14 | <= Original | **32 > 31** | 15 | 18 | **ROT** (Befund 2) |
| S8 Technik | <= 16 Zeilen, <= 4096, Felder gueltig, 23/23/21 | 16 Zeilen; 4096 (= Grenze); Felder 480-482/484/486-495 gueltig; 23/23/21 | 13 Zeilen, 3328 | 15 Zeilen, 3840 | GRUEN |

Die eigenen Formwerte S1-S4 stimmen auf die Zehntelstelle mit riegel_v8.txt ueberein. Bei S5 sind es 6379 statt 6501 Texel; das ist
eine andere Rasterung und kein Unterschied in der Sache.

### S7a Herkunft im Einzelnen
- Zeilen 493, 494, 495: 62, 50 bzw. 88 Eintraege weichen von der V7-Zeile 480, 481 bzw. 482 ab.
- Jeder dieser Eintraege ist ein Original-EM30-Ader-Eintrag (a* >= 15, L* <= 38). 0 sind fremd, kein Original-Ader-Eintrag fehlt.
- 91 Flaechen liegen auf den Ader-Zeilen: Mesh 8 41, Mesh 13 13, Mesh 14 37.
- Darunter liegen 18 032 Texel. Jeder ist ein Original-Texel an derselben Stelle und im Nutzungsbereich derselben Original-Zeile.
- 4553 dieser Texel sind rot. Davon ist keiner kein Original-Texel, und keiner zeigt auf einen Nicht-Original-Eintrag.
- 0 UV-Schuebe.
- Textur V8 = V7 bytegleich. Zeilen 480-492 = V7.

## Befunde ausserhalb oder am Rand der Riegel

1. **S7c rot bestaetigt, Ursache gemessen.**
   - In PV liegen 96,7 % der sichtbaren (W)-rechten Rumpfpixel auf den Ader-Zeilen (493: 87,9 %, 494: 8,8 %).
   - Trotzdem sind nur 5,0 % rot, denn die Original-UV dieser Inseln liegen auf aderarmen Texeln.
   - Das Original-EM30 hat in derselben Pose an dieser Stelle 49,4 % (lok z < 0) bzw. 60,1 % (lok x < 0), am Oberarm 57,5 %.
   - Der Zielkonflikt des Bauers (Offen 1) ist damit nachvollziehbar. Er ist nicht geloest.
2. **S7d ohne Mesh 14 gemessen.**
   - Mesh 14 ist in Runde 2 neu auf Zeile 495 umgestellt (37 Flaechen).
   - Mesh 14 ist der Unterarm (W)-rechts: Elter 13, rel13 z -1136, in PV 2364 px sichtbar.
   - riegel_v8.rote_flecken zaehlt aber nur die Knochen (8, 13).
   - Mit Mesh 14 hat V8 r2 32 rote Komponenten (>= 4 px), das Original-EM30 in derselben Pose 31.
   - Nach dem Wortlaut der Riegel-Definition ("(W)-rechts <= Original") ist S7d damit rot.
   - Anteil a* >= 10 am Unterarm: V8 r2 17,4 %, Original 22,1 %.
3. **Huegel H nicht rot.**
   - Die Nutzerlinie heisst "Little red hill from the back". Bauplan 6b verlangt "rot (Ader-Zeile, 6c)".
   - Gemessen habe ich die obersten 8 px der Figur in x 428-445 (huegel_rot.txt).
   - Anteil a* >= 10 dort: V8 r2 0,0 % (Median a* 3,9), Vorlage 58,3 % (Median 12,1), V8 r1 27,8 %, V7 0,0 %.
   - Der Riegel S3b misst nur die Hoehe, deshalb faellt das dort nicht auf.
4. **S3a fragil.**
   - Kuppe 4,0 bei einer Schwelle von <= 4.
   - Ich habe die Projektion um eine halbe Pixelstelle verschoben (Offset 0 bzw. 0,75 oder +-0,5 px in x/y). In 3 von 6 Varianten ergibt
     sich dann 5,0, und S3 waere rot.
   - S2c (3,0), S1, S2a und S4 sind dabei stabil.
   - Der Bauer hat per (1+1)-Evolution auf genau diese Masse optimiert (kette_fein.py, ruecken_ecken.py). Die gruenen Grenzwerte haben
     deshalb keinen robusten Abstand.
5. **Szene: Gelenk 0 bis 39,3 Grad.**
   - Bauplan 6a sagt "je hoechstens 25 Grad". Im eigenen Lauf hat Gelenk 0 in h69 31,7 Grad und in h70 39,3 Grad.
   - Ursache ist die Bodenregel unter der Szenenfuehrung. Der Bauer hat es offengelegt (Offen 3).
   - Sonst: Median des Takt-Maximums 20,4 Grad. Die Biegeebene wechselt in 40 von 144 Takten das Vorzeichen (V7 144, V8 r1 121).
6. **Keyframes.**
   - Knochen 16 hat in allen 912 Keyframes denselben Winkel (-76, -25, 420). Das Rohr ist starr am Rumpf.
   - Das groesste Gelenk je Keyframe: Median 18,5, P90 25,0, Maximum 25,2 Grad. 0 Keyframes liegen ueber 25,5 Grad (V8 r1 243, V7 912).
   - Die Biegeebene kehrt in 141 von 912 Keyframes um (Bodenregel). In PV ist sie gleichsinnig: Bild-Drehwinkel -25,4 / -9,5 / -17,0 /
     -9,4 / -9,1 / -16,1, wie der Bauer schreibt.
   - Die Glieder 17-22 haengen im Modellraum und folgen nicht dem Rumpf. Im Raum von Knochen 8 weichen sie gegen PV im Median um 19 Grad
     ab, maximal um 145,7 Grad (Clips mit geneigtem Rumpf). Das ist eine Folge der Gestaltung, kein Riegelbruch.

## Seiten
- "Die rechte Seite" ist (W)-rechts, also z < 0 im Knochenraum 8 (rel13 z -1136, rel10 z +943).
- In PV sind sichtbar: Knochen 13 3642 px, Knochen 14 2364 px, Knochen 10 235 px, Knochen 11 612 px. Die (W)-rechte Flanke ist also die
  zur Kamera gewandte.
- Von den 41 Mesh-8-Flaechen auf 493/494 liegen 39 mit dem Schwerpunkt bei z < 0. Zwei (d50, v8) liegen auf der Rueckenmitte (z +4 / +7).

## Reproduktion und Technik
- v8_kette.sh, zweiter Lauf in pr2/kette: "V8-KETTE-OK sha256 d0ef852c..." (EXIT 0).
  - Die Stufen 1-6 stimmen: a1bc9309, 38562ced, 99703bbd, 784c8f05, 9e0e58ef, V7 319bbd17.
  - Neubau, Pin und Asset sind bytegleich (cmp).
- Bau: "ninja: no work to do", "=== LOCAL-BUILD-OK (build)" (pr2/build.txt).
- Testgruppe: ctest -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole" --timeout 900
  -> "100% tests passed, 0 tests failed out of 8" (305 s, pr2/ctest_gruppe.txt).
- Unit-Suite: ctest -j 6 --timeout 600 -R "^unit_" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)"
  -> "100% tests passed, 0 tests failed out of 1013" (pr2/ctest_unit.txt).
- unit_schwanz_r41 -V (pr2/schwanz_v.txt), stimmt mit dem Bauer ueberein:
  - "Freigabe-Schritt 630" bei Grenze 700
  - "erstes Glied hoechstens 0.1 Grad neben dem Keyframe"
  - Gegenprobe ohne V8-Erkennung: "147.5 Grad"
- Eigener Szenenlauf (lauf_v4.sh, BAUM r41_sichel_v8, EMD "-", Ende 1110, Bilddump nur F1100; pr2/szene):
  - EXIT 0; Log "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD", "23 meshes, 23 bones, 21 clips", "Szene Ende: Cut 4".
  - state.log: 1131 Zeilen, 0 Unterschiede zum Lauf des Bauers und 0 zum freigegebenen V7-Lauf sfv4/r/V7_szene.
  - Alle 144 "Schwanz B y"-Zeilen sind identisch mit szene_v8r2. F1100 ist bytegleich.
- Labor 50E0: nur die Logs des Bauers gelesen.
  - Ab dem Raumsprung (F1, pst=1, Birkin st=1 ss1=9 @(-15000,12200)) sind F1-F1000 von labor_v8r2 gleich labor_v8 und labor_v7.
  - Runde 2 laeuft 10 Takte laenger. Die Form ist geladen (23/23/21).
- Nicht selbst nachgeprueft: die Gegenprobe "Asset V7 bzw. V8 r1 -> unit_birkin_formen und unit_schwanz_r41 rot". Dafuer muesste man das
  Asset im Baum tauschen.

## Code-Diff gegen 6a25db27b
- Geaenderte Dateien:
  - Code: schwanz_r41.c, re15_schwanz_r41.h und birkin_formen.c (bf_schwanz_v2 mit !v8, bf_schwanz_v4 mit || v8)
  - Tests
  - lauf_v4.sh (BAUM per Umgebung) und make_package.sh (nur Text)
- Neue Zahlen:
  - SW_V8_VERH 1,10, SW_V8_GLIED_MIN 500, SW_V8_GLATT_MAX 25 Grad, SW_V8_WURZEL_X -760
  - SW_V8_STARR 1, SW_V8_RUECK 5 Grad
  - verteilte Bodenregel: 1 Grad je Schritt, hoechstens 180 Schritte

  Jede dieser Zahlen ist als GESTALTUNG/PORT-WAHL kommentiert und mit Messung oder Bauplan belegt.
- Kleinigkeiten (gelb):
  - Die Kappe "u1_n < 64" ist unkommentiert. Sie ist wirkungslos, denn 64 x 5 Grad > 180 Grad.
  - Der SW_V8-Kopfkommentar nennt noch die Werte aus Runde 1 ("538 E.", "Ruhe-Gelenkwinkel 11-27 Grad"). Runde 2 hat 526 E.; die
    Erkennung greift trotzdem (526 >= 500).
  - ziel_anwenden() hat einen toten Parameter v8 ("(void)v8", a0 = 0).
- "@0x"-Stellen: Hinzugefuegt sind nur Texte der Form "keine @0x-Adresse/-Konstante". Es gibt keine erfundene Adresse.
- Wirkungsbereich:
  - Ich habe die Erkennung re15_schwanz_r41_v8_kette (kette_ok, gleiche Glieder >= 500, Wurzel y < 0, x >= -760) mit eigenem Code ueber
    alle EMD/*.EMD laufen lassen.
  - Nur EM30_HANGAR.EMD hat 23 Knochen und erfuellt sie (Glieder 6 x 526, Wurzel (-714,-214,130)).
  - Der Aufruf in main.c:10378 gilt fuer alle NPCs, ohne V8-Kette aendert er nichts.
  - Ausserhalb von Hangar/Labor ist nichts betroffen.
- Asset gegen V7:
  - Geaendert sind die Meshes 8, 13, 14 und 16-22; relpos und Keyframes nur bei den Knochen 16-22.
  - Clips/EDD sind gleich, die Textur ist bytegleich, die CLUT-Zeilen 480-492 sind bytegleich.
  - Flaechenwechsel: Mesh 8 480->493 35 und 481->494 6, Mesh 13 480->493 13, Mesh 14 482->495 37; Mesh 16 ist neu.
  - Unveraendert: Mesh 9 (Kopf), Mesh 12 (linke Hand V7), Meshes 0-7 (Beine).
- Tests: Die V7-Pruefungen sind durch strengere V8-Pruefungen ersetzt (Zeilenzahlen je Mesh, 62/50/88 Ader-Eintraege, 0 fremd, UV ausser
  dem CLUT-Feld gleich). Sie sind nicht ersatzlos gestrichen.

## Fazit
- Rot:
  - S7c: 5,4 / 5,0 % statt 25-80 %
  - S7d mit Mesh 14: 32 > 31
  - Huegel H ohne Rot: 0,0 % gegen 58,3 % (Bauplan 6b)
- Gelb:
  - S3a auf der Schwelle, kippt bei Halbpixel-Verschiebung
  - Gelenk 0 in der Szene bis 39,3 Grad
  - veraltete Kommentarzahlen
- Gruen und belastbar:
  - S1, S2, S4, S5, S6, S7a, S7b und S8
  - Kette und Pin
  - Tests 8/8 und 1013/1013
  - Szene: bytegleich mit dem Lauf des Bauers, state.log gleich V7
  - Labor und Wirkungsbereich

**bestanden = nein.**
