# SF V4 Runde 2: Zahlen-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Bau V4 Runde 2 der Sichel-Form (Commits 5a89f427e, 28e928138, 3e603a4c0, 951fde6ed, 2b50e6414, ee470c381 auf
r41/sichel_nb2), Modell EM30_V4.EMD sha256 174b7879..., gemessen gegen SF_MESSBLATT_V4.md (Commit 7a4f3b684). Kein RE-Befund,
keine @0x-Konstante: die Sichel-Form gibt es auf der Disc nicht, alles ist Bildmessung am 80%-Video 4.

**Ergebnis: nicht bestanden.** Die 6 roten Zeilen des Bauers (S3, T7 h85/h100/h110/h125, F14) bestaetige ich. Nach
unabhaengiger Messung kommen dazu:

- **T2-Drehung rot**: 52,5 Grad mit dem Verfahren, das am Original 66-70 Grad liefert. Die Mittellinie des Bauers liefert am
  Original selbst nur 53-65 Grad und wuerde das Original durchfallen lassen.
- **F7 und F8 rot** nach dem Wortlaut des Messblatts (Id ganz). Der Bauer hat die Riegel auf die oberen 58 / 56 % der Zeilen
  umgedeutet und die Messblatt-Lesart im Ausdruck fest auf "OK" gesetzt.
- **F-Mittel grenzwertig**: 3,92-4,33 statt der gemeldeten 3,13.
- **Boden**: Im Lauf ragt der Dorn in h33-h35 bis 57 E. unter den Boden.

Bestaetigt sind: Kopf K1-K10 und S1/S2/S4, die Schwanzlage (T1 nach Wortlaut, T3-T6, TA), die Schwanzfarbe T8, G1/M1-M4, das
Spiel-Asset, die Tests und die Gegenprobe V3 r2.

## 0. Verfahren (eigener Code, nicht riegel_v4.py)

Werkzeuge re15_port/tools/r41_sichel/v4/pz2_*.py (nicht committet), Laufausgaben sfv4/pz2/ (Scratchpad), Belege
SF_belege/v4/PZ2_*.

| Werkzeug | misst |
|---|---|
| birkin_formen.py --variante v4 --aus sfv4/pz2/emd | Neubau aus HEAD ee470c381: **sha256 174b787939214f4a...** = Bauer-Meldung |
| pz2_g1.py, pz2_g1b.py, pz2_m4.py | eigener EMD/MD1/EMR/TIM-Parser: G1, M1-M4, Texel-Diff, geteilte Texel Mesh 10/13 |
| pz2_render.py | b2.zeichne (Messrender Messblatt Abschn. 7; v2/b2.py, v4_gemein.zeichne und posen_v4.json seit r1 unveraendert) fuer V4 / V3 r2 / Ref2 in "85" und "135" |
| pz2_mess.py | alle K-, S-, T-, F-Riegel (Messfunktionen der r1-Pruefung, Selbstpruefung am Original unten) |
| pz2_schwanz.py | T1 nach Wortlaut (Id 16 an Id 7/8) und alternativ (Ids 16-22), T2 Lage/Drehung, TA absolut, Richtungsprofil |
| pz2_t2orig.py, pz2_t2kreuz.py | T2-Drehung mit eigener Mittellinie und mit der Bauer-Mittellinie (v3/P_schwanz.mittellinie), je am Original (3 Leser-Masken) und an V4 |
| pz2_farben_alt.py | F7/F8/F14 in Messblatt-Lesart und Bauer-Lesart, F-Mittel beider Lesarten |
| lauf_v4.sh (eigener Lauf sfv4/pz2/lauf) | Spiellauf mit dem Neubau: **528 hangar-takt-Zeilen bitgleich zu sfv4/r/V4r2, FRAMEDUMP F343 bitgleich** |
| pz2_lauf.py, pz2_lauf_bild.py, pz2_lauf_abs.py, pz2_t7.py, pz2_boden.py, pz2_licht.py | Kettengelenke je Takt, Gelenkwinkel, Projektion in FRAMEDUMP/Videobild, T7-Form, Boden mit Mesh-Dorn, Lichtgegenprobe |

**Selbstpruefung am Original** (pz2_mess.orig_check): Die Messfunktionen treffen die Messblatt-Ziele.

- Auge vorn: 46,5 / 7,2 / -1,1, 9 x 10 px, Schwerpunkt (377,1 / 167,2), Kontrast 29,6.
- Auge seitlich: 48,3 / 3,9 / 0,6, 10 x 14 px.
- Fenster KV1-KV5, KS1, KS2: dE00 <= 0,6 zum Ziel.

## 1. Riegel-Tabelle Soll / Bauer / Pruefer

H vorn (Pose 135) 364 px (Oberkante 145, Sohle 509), H seitlich (Pose 85) 264 px (170 / 434), Scheitel (oberste Zeile) x 403,5.

| Riegel | Soll | Bauer | Pruefer (eigener Code) | Bewertung |
|---|---|---|---|---|
| G1 | Meshes 0-8, 10-15, Skelett 0-15, KF 0-15, EDD byte-gleich | OK | Sektionen 0, 1, 3-6 byte-gleich; EMR rel 0-15 und Tabelle gleich; 912 Keyframes x 116 Byte, Bytes 0-83 alle gleich; MD1 Meshes 0-8/10-15: alle 8 Bloecke gleich (0 Abweichungen); TIM: CLUT 480/481/482/484 geaendert, 483/485 gleich, 486/487 neu; 57389 Texel geaendert, davon 34433 in Koerper-UVs (vom Messblatt erlaubt: "TIM-Texel und CLUT") | OK |
| Spiel-Asset | = Pin | unveraendert | EM30_HANGAR.EMD sha256 cfd1ac42... = release/em30_hangar.sha256; git diff 84e8a4d97..HEAD leer fuer Asset und Pin | OK |
| M1 / M2 | <= 212 / <= 118 | 160 / 109 | 30/20/20/20/20/20/30 = 160; Kopf 109 | OK |
| M3 | 8 bpp, CLUT | OK | TIM-Flag 0x9; Kopf CLUT 486 TPage 0, Schwanz 487 TPage 2, Koerper 480/481/482/484 | OK |
| M4 | 0,045-0,091 | 0,054 / 0,051-0,066 | Kopf 0,054; Schwanz 0,051/0,056/0,051/0,062/0,066/0,066/0,059 | OK |
| K1 | 1 Fleck, Kontrast >= 20 | 1, 20,3 | 1 Komponente (92 px), Kontrast 20,3 | OK (Rand 0,3) |
| K2 | 46,9/7,2/-1,1, dE00 <= 6; L* >= KV5 + 20 | 0,5 | 47,0/7,7/-0,8, dE00 0,53; 47,0 >= 24,6 + 20 | OK |
| K3 | 0,016-0,036 x 0,023-0,043 | 0,030 x 0,036 | 10 x 13 px = 0,027 x 0,036 | OK |
| K4 | 0,035-0,065 links (B); 0,07-0,12 unter OK | 0,059 / 0,107 | 0,058 / 0,106 | OK |
| K5 | 13,3/7,4/1,5; L* <= Auge - 25 | 0,8 | 14,4/7,2/1,7, dE00 0,76; 14,4 <= 22,0 | OK |
| K6 | L* <= 18 | 9,0 | 8,8 (Box 369-378 x 187-199) | OK |
| K7 | 18,2/7,1/1,3 | 1,4 | 20,2/6,8/1,0, dE00 1,43 | OK |
| K8 | 9,9/9,9/3,8; Anteil L* > 35 <= 0,05 | 3,3 / 0,000 | 11,7/6,7/2,8, dE00 3,31; Anteil 0,000, L* max 20,0 | OK |
| K9 | 22,6/15,4/5,2, a* >= 11 | 1,1 | 21,3/16,1/5,5, dE00 1,05 | OK |
| K10 | 0,14-0,18 x 0,15-0,20 | 0,154 x 0,187 | Box 325-380 x 154-221: 0,151 x 0,184 (Differenz) / 0,154 x 0,187 (inklusiv) | OK |
| S1 | 48,4/4,0/0,5; 0,028-0,052 x 0,040-0,070; 1 Komponente | OK | 47,8/3,8/0,9, dE00 0,78; 9 x 13 px = 0,034 x 0,049; 1 Komponente (78 px), Kontrast 25,9 | OK |
| S2 | 0,03-0,07 hinter VK; 0,76-0,82 ueber Sohle | 0,058 / 0,811 | 0,058 / 0,811 | OK |
| S3 | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | ROT (0,011) | 13,8/2,0/1,0, dE00 3,71; **Abstand 0,015 H** (4 px) | **ROT** |
| S4 | 27,6/15,3/3,3, a* >= 11 | 0,4 | 27,3/14,8/3,1, dE00 0,43 | OK |
| T1 | 0,15 hinter Scheitel, 0,767 ueber Sohle (+-0,03) | 0,165 / 0,750 (Id 16) | **nach Wortlaut (Id 16 an Id 7/8): (360,236) = 0,165 / 0,750, OK**; alternativ (unterster Schwanzpixel Ids 16-22 an Id 7/8, Id 17 vor der Flanke): (349,241) = 0,206 / 0,731, rot | OK nach Wortlaut (Hinweis 4.4) |
| T2 Lage | dx -0,245 dy +0,036 (+-0,03) | -0,252 / 0,022 | Biegung (Mitte des Fensters mit groesster Drehung) (290,2 / 248,9): -0,264 / +0,049 | OK |
| T2 Drehung | 60-100 Grad in 0,10 H (Tangenten +-0,025 H) | 67,3 | **52,5** (eigene Mittellinie; Original mit derselben Linie 65,8 / 69,5 / 70,1); Sehnen 44,1 (Original 45-48) | **ROT** (4.1) |
| T3 | dx -0,265 dy +0,257 | -0,266 / 0,244 | (289,8 / 301,0): -0,266 / +0,246; Taille 0,025 H | OK |
| T4 | dx -0,245 dy +0,397; 0,37 H ueber Sohle | -0,243 / 0,393; 0,357 | (295,0 / 340,9): -0,246 / +0,397; 0,353 H ueber Sohle | OK |
| TA (Bauer-Zusatz) | Nutzerpunkte je Achse +-0,03 H | OK | Beginn (360,236) +0,011/-0,027; Knick (289,8/301,0) -0,001/-0,026; Ende (295,0/340,9) 0,000/-0,008; Biegung gegen Video (295,0/252,1) -0,018/-0,012 | OK (Beginn/Knick y nahe der Grenze) |
| T5 | 0,03/0,065/0,125/0,14 (+-0,015/0,02/0,025/0,025), Abfall <= 0,01 | 0,036/0,066/0,127/0,129, 0,008 | 0,040/0,078/0,125/0,138; Abfall 0,009 | OK |
| T6 | B 0,025-0,05, L 0,12-0,17, 75-95 Grad, Taille schmalste | 0,034/0,152/81 | Median 0,032, L 0,152, 82,5 Grad nach vorn; Taille 0,025 < 0,028 (schmalste ab Biegung) | OK |
| T8 | F15/F16/F17/F18 dE00 <= 6; Wurzel a* >= 7, Ende a* <= 6 | 1,0/1,1/1,3/1,2 | F15 8,9/12,6/0,9 dE00 1,10; F16 9,7/9,4/1,6 1,24; F17 27,0/0,1/1,2 1,15; F18 13,8/3,4/1,7 0,97 | OK (Hinweis 4.6) |
| T7 Gang h85 | T1-T4 +-0,06 H | ROT (0,294) | **ROT**: Abfall Biegung -> Knick 0,104 H (Ruhe 0,245), -> Ende 0,130 H (Ruhe 0,358); Abweichung 0,228 H (pz2_t7, Weltgelenke) | **ROT** |
| T7 Gang uebrige | (Bauer-Zusatz) | h100/h110/h125 rot, h118/h135 OK | Abfall-Mass: h90 0,134 / h95 0,185 / **h100 0,359 (Kette waagerecht)** / h105 0,145 rot, h110-h135 <= 0,054 OK; **18 von 46 Gangtakten > 0,06 H** (Hoehenmass, also Untergrenze; der Bauer findet ueber dx/dy zusaetzlich h110/h125) | **ROT** |
| F0 Licht h118/h125/h135 | dE00 <= 3 | 0,1 / 0,3 | Rumpf 0,22 / 0,27 / 0,18; Kopf 0,50 / 0,39 / 0,54 (eigene Entzerrung FRAMEDUMP -> Video) | OK |
| F6 | dE00 <= 5, abs(dL) <= 4 | 3,3 / +2,1 | 19,3/12,9/5,3, dE00 3,62, dL +2,7 | OK |
| F7 (Id 13) | 33,3/5,5/2,7 | 3,9 / -2,8 (obere 58 %) | **Id ganz: 22,7/5,7/1,7, dE00 8,08, dL -10,6** (2 px erodiert 8,28 / -10,9); obere 58 % erodiert 30,5/8,6/2,6, 3,90, -2,8 (= Bauer) | **ROT nach Messblatt** (4.2) |
| F8 (Id 10) | 16,4/8,3/1,5 | 3,0 / +3,7 (obere 56 %) | **Id ganz: 21,2/5,4/1,1, dE00 4,41, dL +4,8** (erodiert 4,11 / +4,5); obere 56 % erodiert 3,02 / +3,7 (= Bauer) | **ROT nach Messblatt** (dL) |
| F9 | 12,7/-0,9/-1,5 | 2,1 | 14,2/-0,6/-3,4, dE00 1,99 | OK |
| F10 (Id 12) | 11,2/14,0/2,5 | 2,7 | 13,4/10,9/2,0, dE00 2,86 | OK |
| F14 | 60,0/11,0/1,2, abs(dL) <= 4 | ROT 5,0 / -5,2 | ohne Erosion 51,8/10,6/2,9, dE00 7,76, dL -8,2; erodiert 54,8 / 4,97 / -5,2 (= Bauer) | **ROT** |
| F19 | 18,4/0,5/-3,5 | 1,7 | 17,2/-0,2/-4,5, dE00 1,58 | OK |
| F-Mittel | <= 4 | 3,11 | Messblatt-Lesart 4,33 (ohne Erosion) bzw. 3,92 (F7/F8/F14 erodiert); Bauer-Lesart 3,13 | **grenzwertig** |
| F0 Wand | dE00 <= 2 | 0,52 | 65,2/0,2/-2,5, dE00 0,52 | OK |

Zaehlung nach eigener Messung: rot sind **S3, T2-Drehung, T7, F7, F8 und F14**. Grenzwertig sind das F-Mittel und K1 (Rand 0,3).
Dazu kommt der Bodenbefund im Lauf (Abschnitt 3). Gegenueber der Bauer-Tabelle neu rot: T2-Drehung, F7 und F8.

## 2. Tests und Gegenprobe

- **Bau**: "bash re15_port/tools/local_build.sh build" (RE15_BUILD_DIR = Worktree-Build) meldet "ninja: no work to do" und
  LOCAL-BUILD-OK. re15_pc.exe (01:56:55) ist juenger als schwanz_r41.c (01:56:53); der Lauf V4r2 (02:18) lief mit diesem Stand.
- **ctest** -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --timeout 900: **4/4 Passed**,
  64,72 s (SF_belege/v4/PZ2_ctest.txt).
- **Reichweite der Tests**: Die Tests sagen nichts ueber T7.
  - integration_r41_sichel_form laedt das Spiel-Asset Ref2 (test_r41_sichel_form.cmake, "EM30: Ersatzmodell
    EMD/EM30_HANGAR.EMD"), nicht V4.
  - unit_schwanz_r41 Teil 9 prueft nur die **Obergrenze** max(16, Keyframe + 3) Grad mit den V4-Gliedlaengen.
  - Die neue **Untergrenze SW_BIEGUNG_SPIEL** (Commit 3e603a4c0) und die Biegungs-Ausnahme ueber 30 Grad beruehrt kein Test.
- **Gegenprobe V3 r2** (eigene Messung, PZ2_mess_V4_V3_REF2.json): **Die Riegel trennen V3 r2 klar.** Rot sind:
  - Kopf vorn: K1 (Kontrast 2,8), K3 (0,003), K4 (-0,034), K5 (13,6), K6 (34,2), K7 (8,5), K8 (23,9 / Anteil 0,70), K9 (16,6),
    K10 (Breite 0,124).
  - Kopf seitlich: S1 (2 Komponenten), S2 (0,078 / 0,900), S3 (24,5), S4 (17,4).
  - Schwanz: T2 (14,7 Grad), T3 (-0,306 / 0,401), T4 (0,266 ueber Sohle), T6 (L 0,083), T8 F15/F16/F17 (8,9 / 9,0 / 6,5).
  - Koerperfarben: F6-F10, F14 (19,1), F19 (5,6), F-Mittel 9,27.

## 3. Spiellauf (sfv4/r/V4r2, eigener Wiederholungslauf bitgleich)

- **Rueckfaltung behoben**: 0 von 144 Takten haben ein Gelenk ueber 90 Grad (r1: 73).
  - Groesster Gelenkwinkel je Gelenk: 16,1 / 15,2 / **67,7** / 16,2 / 16,3 / 15,6.
  - Biegung (Gelenk 3) im Lauf 44,4-67,7 Grad, Median 55,0.
  - Gliedlaengen stabil: 621-623 / 489-491 / 585-588 / 577-579 / 488-491 / 255-257 / 256-257.
- **Knick und Breite in Gangbildern**: Im Spielbild des Gangs sind Knick und Breite **nicht messbar**. In den Gangbildern h90-h135
  (clip 1, gier 1513-1561) zeigt die Spielkamera die Figur von vorn. Die Kette liegt in der Projektion innerhalb der
  Koerpersilhouette (PZ2_lauf_kette.png, Kacheln h95/h100/h105/h110/h118/h135). Am 3D-Stand gemessen:
  - Knick (Gelenk 5) im Gang 6,5-16,3 Grad. Die Ruhe-Gelenkliste hat im Bild 21,7 Grad, SW_GLATT_MAX begrenzt auf 16.
  - Die Breite ist starre Geometrie (Taille Mesh 20/21, im Render 0,025 H).
- **Gang-Form (T7)**: In 18 von 46 Gangtakten liegt der senkrechte Teil mehr als 0,06 H zu hoch.
  - h100: Gelenke 3-7 bei y -2051 / -2152 / -2157 / -2124 / -2057. **Die Kette steht waagerecht** (Abfall Biegung -> Ende -0,001 H
    statt 0,358 H).
  - Ursache: Die Gelenke 1/2 bleiben gerade (0,0-0,1 Grad), und die Biegung haelt 49 Grad. Gehoben wird die ganze Kette an der
    Wurzel.
  - Die Begruendung von SW_BIEGUNG_SPIEL ("das senkrechte Stueck schwenkt am Knick um <= 0,043 H < 0,06 H") tritt im Lauf nicht ein.
- **Absolute Lage im Lauf**: In keinem Takt h80-h135 liegen Biegung, Knick und Ende zugleich innerhalb 0,06 H an den Video-Punkten
  (pz2_lauf_abs: Abweichung 0,32-0,63 H).
  - Grund ist nicht der Schwanz, sondern die Szene. Bei h85 steht das Spiel in clip 11 bild 64 gier 1893; Vorlage 2 / Pose "85" ist
    clip 1 bild 26 gier 556.
  - Die Messblatt-Annahme "im Spiel bei h85 Spielkamera wie Vorlage 2" stimmt fuer die Kamera, aber nicht fuer die Figur. Unter G2
    (Szene unveraendert) laesst sich T7 nur relativ zum Koerper messen, wie es der Bauer tut.
- **Boden**: Im Auftrag gefordert ist "Schwanz nicht unter dem Boden". Das ist knapp verletzt: **ROT**.
  - Der tiefste Kettenpunkt im ganzen Lauf liegt bei y -6 (h34; r1: -172).
  - Mesh 22 reicht im Knochenraum bis y 319, also 63 E. ueber das geloggte Gliedende hinaus (Dorn).
  - Damit liegt **die Dornspitze in h33 / h34 / h35 bei y +6 / +57 / +45 unter dem Boden** (Boden = "Schwanz B y 0", clip 11 vor
    dem Gang). 57 E. = 0,013 H (H ~ 4319 E. aus den Gliedern 3-7).
  - glaetten() prueft nur die Kettenpunkte gegen den Boden, nicht den Dorn.
- **Licht**: Lichtgegenprobe siehe Tabelle (F0 Licht OK).

## 4. Befunde im Einzelnen

### 4.1 T2-Drehung: das Bauer-Mass besteht die Gegenprobe am Original nicht

Riegel: Drehung 60-100 Grad, Verfahren "Tangenten +-0,025 H am Anfang und Ende eines 0,10-H-Fensters" (r1-Pruefung 4.2, vom Bauer
uebernommen). Das Ergebnis haengt an der Mittellinie (pz2_t2kreuz.py):

| Form | Bauer-Mittellinie (P_schwanz, 2-px-Baender, Gauss 0,01 H) | Pruefer-Mittellinie (3-px-Baender, 3er-Mittel) |
|---|---|---|
| Original, Maske A | **53,3** | 70,1 |
| Original, Maske hi | 65,3 | 65,8 |
| Original, Maske lo | 61,8 | 69,5 |
| V4 r2 | 67,3 | **52,5** |

Mit der Bauer-Linie faellt das **Original selbst** in einer Maske unter 60 Grad. Die Linie ist damit als Riegel-Messung nicht
geeicht (Messblatt Abschnitt 7: "Gegenprobe: das Original besteht alle Riegel"). Mit der Linie, die am Original 66-70 Grad liefert,
hat V4 52,5 Grad: rot.

Das Richtungsprofil zeigt den Grund: Die Biegung ist auf zwei Stufen verteilt (0,30-0,35 und 0,40-0,45 H).

| s (H) | 0,30 | 0,325 | 0,35 | 0,375 | 0,40 | 0,425 | 0,45 |
|---|---|---|---|---|---|---|---|
| Richtung (Grad) | 18 | 31 | 43 | 46 | 52 | 75 | 95 |

Die Lage der Biegung stimmt (T2 Lage OK, Abstand zur Video-Biegung -0,018 / -0,012 H). Das Drehungsmass haengt um ~15 Grad vom
Verfahren ab. Vorschlag: Die Drehung am Messblatt mit einer festgelegten Mittellinie entscheiden.

### 4.2 F7/F8: Riegel umgedeutet, Messblatt-Zeile im Ausdruck fest "OK"

Das Messblatt (Abschnitt 7, Zeile F-Riegel) misst "Oberarm (W)-rechts Id 13, (W)-links Id 10". riegel_v4.py misst in Runde 2 F7/F8
nur in den oberen 58 / 56 % der Zeilen der Id. Die Messblatt-Lesart gibt es nur noch als "F7g/F8g (Id ganz, Runde-1-Mass) ... nur
Angabe" aus, mit fest gesetztem OK (riegel_v4.py koerper_farben: zeile(..., True, ...)). Die Bauer-Zahlen fuer seine Lesart
bestaetige ich (3,90 / 3,02). Nach dem Messblatt sind beide rot: F7 dE00 8,08 mit dL -10,6, F8 dL +4,8.

Der Grund des Bauers ist richtig und belegt:

- Mesh 10 und 13 teilen **5797 Texel** (UV-Belegung mit eigenem Parser; Bauer 5798) und die CLUT-Zeile 480.
- Das Original hat links 33,3 und rechts 16,4 L*. Der Port beleuchtet beide Arme etwa gleich (22,7 / 21,2).
- Mit festen UVs (G1) ist beides zugleich nicht erreichbar.

Das ist ein **Widerspruch zwischen G1 und F7/F8 im Messblatt**. Der Bauer darf ihn nicht allein durch Umdeuten aufloesen. Er gehoert
in die Messblatt-Entscheidung: entweder die Fenster-Lesart statt "Id ganz", oder eine UV-Trennung der Arme erlauben.

### 4.3 F14 und die Helligkeitsdecke

F14 ist rot (erodiert dL -5,2, ohne Erosion -8,2). r2_koerper_farben.py setzt LUM_MAX = 190 als PORT-WAHL. Laut eigenem Kommentar
ergaeben 200 / 215 dL -4,6 / -4,4, also auch dann rot. Gegenueber Runde 1 (55,4) ist F14 dunkler geworden (51,8 ohne Erosion).

### 4.4 T1: Wortlaut erfuellt, aber nur durch den Ansatz vor der Flanke

Seit Runde 2 liegt der Ansatz 0,12 H zur Kamera vor der Flanke (SCHWANZ_V4 tiefe = 0,12 / 0,12 / 0,08 / 0,03). Dadurch ist Id 16
sichtbar, und T1 nach Wortlaut ergibt 0,165 / 0,750 (OK). Ich werte nach dem Wortlaut des Messblatts (Id 16). Drei Einschraenkungen:

- Der unterste Schwanzpixel an der Rumpfgrenze ist Id 17 bei (349,241): 0,206 / 0,731 (rot), TA dx -0,030 H (an der Grenze).
- Die Tiefe 0,12 H stammt aus r2_expt Lauf a1/a2 (Sichtbarkeit nach der Malreihenfolge). Im Generator ist sie nicht als PORT-WAHL
  bezeichnet.
- Laut Messblatt (Abschnitt 9 Punkt 4) zeigt das Bild die Lage in der Tiefe nicht.

Fuer die Sichtpruefung: Der Schwanz tritt damit seitlich der Mittelebene aus.

### 4.5 S3

Der Abstand des Haarkantenfensters KS2 zum Augenfleck ist 0,015 H (4 px), erlaubt sind <= 0,01 H. Farbe und L* sind OK. Rot
bestaetigt.

### 4.6 T8 ist ein Auswahlmass

F17 misst nur Schwanzpixel mit Chroma < 6 oberhalb T2. Deren Anteil ist 0,367 (Original-Bogen 835 von 1859 = 0,45). Fuer den ganzen
Schwanz gilt:

- oberhalb T2: 16,8 / 7,4 / 0,6 (2850 px);
- bis zum Knick: 14,7 / 6,5 / 0,9 gegen "Bogen gesamt" im Original 22,4 / 3,1 / 2,2 (Messblatt Abschnitt 4, ohne Riegel),
  dE00 6,67.

Kein Riegel ist verletzt. Hinweis fuer die Sichtpruefung: Im Mittel ist der Schwanz dunkler und roeter als der Original-Bogen.

## 5. Konstanten im Diff 7a4f3b684..HEAD (Runde-2-Anteil; Runde 1 siehe SF_PRUEFUNG_V4_r1_zahlen.md Abschnitt 5)

| Stelle | Wert | Quelle laut Kommentar | Urteil |
|---|---|---|---|
| schwanz_r41.c SW_GLATT_V4_VERH | 2,0 | gemessene Gliedverhaeltnisse Ref2 1,00, V3 1,00, V2 1,15, V1 1,49, V4 2,4; PORT-WAHL | belegt (Neubau: 624/253 = 2,47) |
| SW_GLATT_KNICK_AB | 30 Grad | PORT-WAHL, Biegung 55-70 Grad | belegt; jetzt fuer V4 wirksam (Lauf: Biegung 44-68 Grad) |
| SW_BIEGUNG_SPIEL | 10 Grad | PORT-WAHL, Herleitung 0,25 x sin 10 = 0,043 H < 0,06 | Quelle genannt, **Herleitung im Lauf widerlegt** (Abschnitt 3, T7: Kette hebt sich an der Wurzel, h100 0,36 H); kein Test |
| SCHWANZ_V4 beginn | (0,15, 0,767) | Messblatt T1 | belegt |
| gelenke | 8 Punkte | r2_form_suche (Ziel = Mitte aus Messblatt-relativ und Nutzerpunkt absolut), danach "von Hand geschaerft" | belegt als Riegel-Anpassung; das Schaerfen zielte auf die Bauer-Drehung (4.1) |
| durchmesser | 0,16 0,13 0,1368 0,131 0,0518 **0,027** 0,038 0,038 | Knick = Mitte Taille 0,024-0,032; 2-4 r2_form_suche; Ende 0,038 (Ziel 0,035) | belegt (r1-Befund 0,022 behoben) |
| tiefe | 0,12 0,12 0,08 0,03 0... | Strahl durch (357,243) trifft Mesh 8 bei z -475; r2_expt a1/a2 | Experiment, **nicht als PORT-WAHL gekennzeichnet** (4.4) |
| uv grenze / baender | 0,5 = cos 60; Baender 56/40 Zeilen | Teilung 4/2/4 Seiten; M4 | belegt (Gestaltung) |
| KOPF_V4 skal | (1,3 0,9 1,55) | r2_kopf_expt k6/k9/k10; Tiefe 1,3 = groesste mit K10-Hoehe <= 0,20 | belegt (r1-Befund Tiefe ohne Herleitung behoben) |
| KOPF_V4 auge | (360, -306, -71) | Sehstrahl der Vorderkamera an der K4-Stelle (r2_auge_suche) | belegt |
| KOPF_V4 vorbau | vor 0,06; z0 -10 z1 30; xa 100 xb 250; y1 40 y2 170 | vor = S2 0,049 + Rundung (Lauf k5); Rest Eckenreihen, PORT-WAHL | belegt (r1-Befund z0..yb ohne Quelle behoben) |
| schwelle | 0,05 = cos 87 Grad | PORT-WAHL | belegt (r1-Befund behoben) |
| Bogenmitte | halber Weg Gelenk 3 -> Innenecke (x/z Gelenk 2, y Gelenk 5) | L-Form Messblatt Abschnitt 4 | Konstruktion beschrieben, Faktor 0,5 ohne Mass |
| eichen_v4 S2_MITTE | **(0,045, 0,79)** | Kommentar: "Messblatt V4 S2: 0,049 H hinter der Vorderkante" | **Wert weicht vom zitierten Mass ab (0,045 statt 0,049)**, ohne Begruendung |
| eichen_v4 AUGE_ACHSEN / RAND / RAND_VORN / NAH / S_GEMALT / STRUKTUR_ZONE | 88/74/88; 0,5; 0,8; 360; 1,40; 0,5 | jeweils Eichlauf + Riegel (K1/K3/S1), PORT-WAHL | belegt als Eichung |
| eichen_v4 Startwert / 0,6, F17-Gewicht 2,0 | - | mittleres lambda erster Lichtrender; Mischpixel F17 | belegt |
| r2_koerper_farben LAM_MIN / LUM_MAX / MU | 0,35 / 190 / 0,25 | PORT-WAHL mit Laufbelegen | belegt; LUM_MAX kostet F14 (4.3) |
| r2_koerper_farben GEWICHT | oberarm_br_oben 1,6, oberarm_bl_unten 2,0, buckel_s_hell 8,0, hals 2,0 | "schwaechere Regionen bekommen mehr Gewicht" (Laeufe r2 Koerper 5-10) | PORT-WAHL, **auf die eigenen Riegel nachgestellt** (Mass und Ziel sind dieselben Fenster) |
| riegel_v4 F7/F8 zeilenteil | 0,58 / 0,56 | Lage der Messblatt-Fenster im Original-Oberarm | **Riegel-Aenderung ohne Messblatt-Beschluss** (4.2) |

## 6. Offen fuer Bauer und Messblatt

1. **T2-Drehung**: Eine Mittellinie festlegen, die am Original 66-70 Grad liefert. Mit ihr hat V4 52,5 Grad (rot); die Biegung ist
   auf zwei Stufen verteilt.
2. **F7/F8**: Den Widerspruch G1 (geteilte Texel 5797, CLUT 480) gegen die Id-ganz-Riegel dem Messblatt bzw. dem Nutzer vorlegen.
   Bis dahin rot werten und nicht fest "OK" ausgeben.
3. **T7**: Die Kette hebt sich in 18 von 46 Gangtakten an der Wurzel (h100 waagerecht); SW_BIEGUNG_SPIEL greift dort nicht.
   Ausserdem steht die Figur bei h85 im Spiel anders als in Vorlage 2 (gier 1893 gegen 556), deshalb ist T7 h85 nur relativ zum
   Koerper messbar.
4. **Boden**: Der Dorn liegt in h33-h35 bis 57 E. unter dem Boden; glaetten() prueft nur Kettenpunkte.
5. **Weitere rote und knappe Riegel**: S3 (0,015 H) und F14 (dL -5,2 / -8,2) bleiben rot. K1 liegt nur 0,3 ueber der Grenze. Das
   F-Mittel ist nach Messblatt-Lesart 3,92-4,33.
6. **Konstanten**: S2_MITTE 0,045 gegen die zitierten 0,049 erklaeren oder angleichen; tiefe als PORT-WAHL kennzeichnen.
7. **Tests**: Die Untergrenze SW_BIEGUNG_SPIEL und die Biegungs-Ausnahme haben keinen Test. integration_r41_sichel_form prueft Ref2,
   nicht V4.

Belege:

- SF_belege/v4/: PZ2_mess_V4_V3_REF2.json, PZ2_schwanz_V4.json, PZ2_lauf_kette.png, PZ2_seite_original_v4_ids_x2.png (Original |
  V4 r2 | Id-Karte, Nutzerpunkte gruen), PZ2_ctest.txt.
- Scratchpad sfv4/pz2/: Renders r_*.npz/png, mess_alle.txt, build.log, ctest.log, eigener Lauf lauf/, licht_h118/125/135.png.
