# SF V4 Runde 3: Zahlen-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Bau V4 Runde 3 der Sichel-Form, Commits eadf288b4, e2ad70c50, 0669ae3dd, d64c021ac, a6baa4eed und ef41724b7 auf
r41/sichel_nb2. Modell EM30_V4.EMD, sha256 dfbc30bc. Gemessen wird gegen SF_MESSBLATT_V4.md (Commit 7a4f3b684) und die Entscheide O1-O6.
Das ist kein RE-Befund und es gibt keine @0x-Konstante: Die Sichel-Form gibt es auf der Disc nicht, alles ist Bildmessung am 80%-Video 4.

## Ergebnis: nicht bestanden

**Die 3 roten Zeilen des Bauers bestaetige ich:**

- **S3**
- **F7 nach dem Wortlaut des Messblatts**
- **FS Oberarm (B)-rechts unten.** Diese Zeile war in Runde 2 noch gruen (Rotanteil 0,09). Hier ist ein bestandener Stand verloren
  gegangen.

**Nach eigener Messung kommen zwei Formbefunde am Schwanz und ein formaler O6-Befund dazu:**

- **T2 Lage, neu rot.** Mit dem einen Werkzeug aus Entscheid O2 (t2_mass_v4.py) liegt die Biegung bei dy +0,100 H unter dem Beginn.
  Soll ist +0,036 +-0,03. Das Original hat mit demselben Werkzeug +0,056. Die gruene Lage des Bauers (+0,061) stammt aus der
  Bauer-Mittellinie, die O2 als Mass ausgeschlossen hat.
- **T5 Monotonie, neu rot.** An der Biegung steht eine Ausbuchtung: Breite 0,153-0,188 H, danach Abfall auf 0,127 H. Der Abfall
  betraegt 0,027-0,061 H, erlaubt sind 0,01 H. In V4 r2 waren es 0,006-0,009 H. Ursache ist die Durchmesserliste des Generators:
  Gelenk 3 hat 0,1599, Gelenk 2 0,1243. Das widerspricht Messblatt Abschnitt 4 Punkt 3.
- **O6.** Die 8 neuen PORT-WAHL-Stellen nennen keinen Nutzer-Wortlaut. Eine Konstante hat gar keine Quelle, zwei Kommentare
  widersprechen ihrem Wert.

**Bestaetigt sind:**

- G1 nach O1, Spiel-Asset und Pin, M1-M4
- K1-K11, S1, S2, S4
- T1 nach Wortlaut, T2-Drehung, T3, T4, T6, TA, T8
- T7 koerperfest nach O3, mit Boden aus allen Mesh-Ecken
- F0, F6, F8 nach Wortlaut, F9, F10, F14 (erodiert), F19, F-Mittel
- FS Unterarm Id 14 nach O5
- die 4 Tests, der Spiellauf (bitgleich nachgefahren) und die Gegenproben

## 0. Verfahren (eigener Code, nicht riegel_v4.py)

Die Werkzeuge liegen unter C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4/pz3_*.py und sind nicht
committet. Ausgaben stehen unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz3/,
Belege unter C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_*.

| Werkzeug | misst |
|---|---|
| birkin_formen.py --variante v4 --aus sfv4/pz3/emd | Neubau aus HEAD ef41724b7: **sha256 dfbc30bc8215b763...** = Bauer-Meldung = sfv4/emd/EM30_V4.EMD |
| lauf_v4.sh (eigener Lauf sfv4/pz3/lauf, exe build/ 11:12:49, ninja -n "no work to do") | **672 hangar-takt-Zeilen bitgleich** zu sfv4/r/V4r3, **191 von 191 FRAMEDUMPs bitgleich** |
| pz3_render.py | b2.zeichne (Messrender Messblatt Abschn. 7) fuer V4 r3, V4 r2 (EM30_V4r2.EMD 174b7879), V3 r2 (1952713c), Ref2; Posen "85"/"135" |
| pz3_mess.py (Kopie pz2_mess) | K1-K10, S1-S4, T1/T3/T4/T6/T8, F6-F19, F0 im Videoraum (Vergleichsregel sigma 1,5, a/b) |
| pz3_schwanz.py, pz3_t2.py | T1 nach Wortlaut, TA; T2 mit **t2_mass_v4.py** (O2): Drehung UND Fenstermitte (Lage), am Original (3 Masken) und am Port |
| pz3_t5.py, pz3_t5orig.py, pz3_t5bild.py | Breitenprofil quer zur Mittellinie (Knick -> Beginn), Original-Masken und Port, Belegbild |
| pz3_k11.py, pz3_k11rot.py | K11 nach der Bauer-Definition nachgerechnet; dazu gleiche Anteils-Fenster bei gleichem Massstab; Rotanteil/a*-Streuung (O4) |
| pz3_g1.py, pz3_g1geo.py | eigener EMD/MD1/EMR/TIM-Parser: Geometrie (Ecken, Normalen, Flaechen, UV ohne CLUT-Feld, TPage), CLUT-Zeilen, Texel |
| pz3_fs.py, pz3_farben_alt.py | FS-Zeilen (Sicht-Boxen) und F7/F8/F14/F6 in beiden Lesarten (Id ganz / erodiert / Fensterlage) |
| pz3_lauf.py | Lauf: Kette gegen Ziel (O3); Form gegen die Ruheform von Vorlage 2 per Kabsch (ohne Ziel-Zeile); Welt-Abfall; Boden mit allen Ecken der Meshes 16-22 |
| pz3_licht.py | Lichtgegenprobe b2 gegen FRAMEDUMP je Id-Gruppe, auch Mesh 10/14 (kommen die neuen CLUT-Zeilen 488/489 im Spiel an?) |

**Selbstpruefung:**

- V4 r2 und V3 r2 reproduzieren mit pz3_mess die Werte der Runde-2-Pruefung exakt, zum Beispiel:
  - V4 r2: S3 Abstand 0,015, F7 8,08 / -10,6, F14 7,76 / -8,2, T2 52,5;
  - V3 r2: K1 2,8, K6 34,2.
- Die Masken in v4/t2_masken sind bitgleich zu den Leser-Masken sfv4/schwanz_maske*.npy.
- t2_mass_v4 liefert am Original 70,1 / 65,8 / 69,5 Grad. Das ist dieselbe Mittellinie wie meine pz2_t2orig.

## 1. Riegel-Tabelle Soll / Bauer / Pruefer

Massstab:

- H vorn (Pose 135) = 364 px (Oberkante 145, Sohle 509).
- H seitlich (Pose 85) = 264 px (Oberkante 170, Sohle 434), Scheitel x 403,5.
- H_E im Lauf = 3775 E. (Koerperecken ohne Schwanz, Pose 85).

| Riegel | Soll | Bauer | Pruefer (eigener Code) | Bewertung |
|---|---|---|---|---|
| G1 (O1) | Geometrie der Meshes 0-8/10-15 gleich, Skelett/KF/EDD gleich, CLUT frei | OK | Geometrie-Abweichungen **keine** (tv, tn, qv, qn, tf, qf, UV ohne CLUT-Feld, TPage). CLUT-Feld: Mesh 10 480 -> 489, Mesh 14 482 -> 488. Byte-gleich 13/15 (0-8, 11-13, 15). EMR rel 0-15 und Tabelle gleich. 912 KF x 116 Byte, Bytes 0-83 gleich. Sektionen 0, 1, 3-6 gleich | OK |
| Spiel-Asset / Pin | unveraendert | unveraendert | EM30_HANGAR.EMD sha256 cfd1ac42... = release/em30_hangar.sha256; git diff 84e8a4d97..HEAD leer fuer Asset und Pin | OK |
| M1 / M2 / M3 / M4 | <= 212 / <= 118 / 8 bpp / 0,045-0,091 | 160 / 109 / ok / ok | 160 / 109 / TIM 10 CLUT-Zeilen 480-489 (483, 485 gleich; 486-489 neu) / wie Bauer | OK |
| CLUT 488/489 im Spiel | (Gueltigkeit O1) | - | FRAMEDUMP gegen b2 h110/h118/h125/h135: Mesh 14 dE00 0,15-0,33, Mesh 10 0,36-0,85, Rumpf 0,07-0,25 (h100 Ueberblendung: 4-9, gilt fuer alle Ids) | OK |
| K1 | 1 Komponente, Kontrast >= 20 | 1; 20,4 | 1 (90 px); 20,4 | OK (Rand 0,4) |
| K2 | dE00 <= 6; L* >= KV5 + 20 | 0,7 | 46,8/7,9/-1,6, dE00 0,80; 46,8 >= 24,6 + 20 | OK |
| K3 | 0,016-0,036 x 0,023-0,043 | 0,030 x 0,036 | 10 x 13 px = 0,027 x 0,036 | OK |
| K4 | 0,035-0,065 links; 0,07-0,12 unter OK | 0,060 / 0,107 | 0,058 / 0,106 | OK |
| K5 | dE00 <= 6; L* <= Auge - 25 | 0,7 | 14,3/7,3/1,3, dE00 0,66; 14,3 <= 21,8 | OK |
| K6 | L* <= 18 | 12,7 | 12,8 (V4 r2: 8,8) | OK |
| K7 | dE00 <= 6 | 1,4 | 20,1/6,8/1,7, dE00 1,39 | OK |
| K8 | dE00 <= 6; Anteil L* > 35 <= 0,05 | 3,3 / 0 | 11,7/6,7/2,6, dE00 3,27; Anteil 0,000 (L* max 20,5) | OK |
| K9 | dE00 <= 6, a* >= 11 | 1,6 | 20,6/16,5/5,7, dE00 1,61 | OK |
| K10 | 0,14-0,18 x 0,15-0,20 | 0,154 x 0,187 | 0,154 x 0,187 (inklusiv) | OK |
| K11 (O4) | 41,0 +-6,1; Kopf >= Rumpf | 36,1 / 32,7 | Nachrechnung: Original 9 Boxen 39,4-42,9 (Mittel 41,0), Rumpf 26,2; V4 r3 Kopf 36,1, Rumpf 32,7. **Gleicher Massstab** (Port x 302/364): Kopf 41,1. Gleiche Anteils-Fenster nur im Kopfinneren: Original 24,8-44,2 (innen 32,7), V4 r3 36,3-45,9 (innen 39,9), V4 r2 22,9-39,9 (oben 22,9) | OK (4.6) |
| S1 | 1 Fleck, dE00 <= 6, 0,028-0,052 x 0,040-0,070 | OK | 48,0/3,9/0,8, dE00 0,45; 8 x 13 px = 0,030 x 0,049; 1 Komponente, Kontrast 24,3 | OK |
| S2 | 0,03-0,07; 0,76-0,82 | 0,058 / 0,811 | 0,058 / 0,811 | OK |
| **S3** | L* <= 22, dE00 <= 6, **Abstand <= 0,01 H** | ROT (0,015) | 14,1/2,3/0,6, dE00 3,73; **Abstand 0,019 H** (Fenster 475-480 x 212-226) | **ROT** |
| S4 | dE00 <= 6, a* >= 11 | 0,3 | 27,6/15,4/2,9, dE00 0,28 | OK |
| T1 | 0,15 hinter, 0,767 ueber (+-0,03) | 0,172 / 0,742 | nach Wortlaut (Id 16 an Id 7/8): (358,238) = **0,172 / 0,742**; Ids 16-22: (345,240) = 0,222 / 0,735 | OK nach Wortlaut |
| **T2 Lage (O2)** | dx -0,245, dy +0,036 (+-0,03) vom Beginn | -0,262 / +0,061 (Bauer-Mittellinie) | t2_mass_v4-Fenstermitte (284,0 / 264,3): **-0,280 / +0,100** vom T1-Beginn; Original mit demselben Werkzeug -0,268 / +0,056 (alle 3 Masken) | **ROT** (4.1) |
| T2 Drehung (O2) | 68,4 +-5,0 | 70,6 | **70,6** (t2_mass_v4); V4 r2 52,5; V3 r2 22,9 | OK |
| T3 | dx -0,265 dy +0,257 | -0,248 / 0,265 | (292,5 / 307,0): -0,248 / +0,261; Taille 0,023 | OK |
| T4 | dx -0,245 dy +0,397; 0,37 ueber Sohle | -0,216 / 0,390; 0,353 | (300,4 / 340,8): -0,218 / +0,389; 0,353 ueber Sohle | OK |
| TA | Nutzerpunkte +-0,03 | <= 0,022 | Beginn +0,004 / -0,019; Knick +0,009 / -0,004; Ende +0,020 / -0,008. Biegung gegen die Video-Biegung (295,0 / 252,1): -0,042 / +0,046 (rot); gegen die Original-Fenstermitte (289,1 / 257,2): -0,019 / +0,027 | Beginn/Knick/Ende OK |
| **T5** | 0,03/0,065/0,125/0,14; **monoton (Abfall <= 0,01)** | 0,028/0,080/0,140/0,131; 0,008 | Achse ab Saat: 0,032/**0,095**/0,129/0,142, **Abfall 0,027**. Achse ab T1: 0,032/0,080/0,138/0,127, **Abfall 0,061**. Ausbuchtung an der Biegung 0,153-0,188 H; V4 r2 0,006-0,009 | **ROT** (4.2) |
| T6 | B 0,025-0,05, L 0,12-0,17, 75-95 Grad, Taille schmalste | OK | Median 0,036, L 0,132, 76,9 Grad nach vorn, Taille 0,023 < 0,025 | OK |
| T8 | dE00 <= 6; Wurzel a* >= 7, Ende a* <= 6 | 1,9/2,9/0,7/1,5 | F15 2,44 (a* 14,5), F16 2,61, F17 1,97, F18 1,05 (a* 3,8) | OK |
| T7a (O3) | Gelenke 1/3/5/7 <= 0,06 H vom Ziel, h85 und Gang | 0/47, max 0,042 | 0/47, max 0,042 (h85 Ende) | OK |
| T7 Form ohne Ziel-Zeile | (O3 "Knick und Breitenprofil bleiben") | - | Kabsch auf die Ruheform von Vorlage 2: max 0,040 H, Median 0,014, 0/47 > 0,06; V4 r2 33/47 (max 0,181), V3 r2 35/47 | OK |
| T7b | Biegung >= KF - 10; Knick >= KF/2 | OK | Biegung 51,7-59,2 Grad (Ruhe 55,8), Knick 10,9-16,1 (Ruhe 16,0) | OK |
| T7c (h100) | keine waagerechte Kette | min 0,387 | koerperfest OK; **Welt-Abfall** Biegung -> Ende h100 0,248 H (V4 r2 -0,002), aber h88-h99 nur 0,091-0,10 H (Ruhe 0,389), siehe 4.5 | OK (Hinweis 4.5) |
| T7d | <= 90 Grad | 60,9 | 60,9, ganzer Lauf | OK |
| Boden (O3) | kein Glied-Mesh, keine Dornspitze unter dem Boden | -4 E. | **alle Ecken von Mesh 16-22** (Achs-y x Streckung + Radius quer zur Achse): tiefster Punkt **-30 E. (h68)**, 0/144 Takte darunter; V4 r2 +57 E. (3 Takte), V3 r2 +94 E. (19 Takte) | OK |
| F0 Licht / Wand | <= 3 / <= 2 | 0,1 / 0,1 / 0,52 | Rumpf h118 0,08, h135 0,13; Wand 0,52 | OK |
| F6 | dE00 <= 5, Betrag dL <= 4 | 3,7 / +2,9 | er2 3,67 / +2,9 (ohne Erosion 4,0 / +3,4) | OK |
| **F7 Wortlaut (Id 13 ganz)** | 33,3/5,5/2,7 | ROT 22,7 / -10,6 | 23,0/5,7/1,5, dE00 7,90, dL -10,3 (er2 8,08 / -10,6) | **ROT** (4.4) |
| F7 Fensterlage (obere 58 %) | (Bauer-Lesart seit Runde 2) | 3,6 / -1,9 | 31,4/8,7/2,3, dE00 3,63, dL -1,9 | (Angabe) |
| F8 Wortlaut (Id 10 ganz) | 16,4/8,3/1,5 | 19,6 / +3,2 | 19,9/8,0/2,5, dE00 2,56, dL +3,5 (er2 2,37 / +3,2) | OK (Rand 0,5) |
| F9 / F10 / F19 | dE00 <= 5 | 2,4 / 2,8 / 1,2 | 2,38 / 2,96 (Id 12) / 1,03 | OK |
| F14 | 60,0/11,0/1,2, Betrag dL <= 4 | 2,8 / -3,0 | er2 (Lesart beider Seiten seit Runde 1): 57,0, dE00 2,76, dL -3,0; ohne Erosion 53,7 / 5,83 / -6,3 | OK (er2; Hinweis 4.7) |
| F-Mittel | <= 4 | 2,65 | Messblatt-Lesart (F7/F8 Id ganz, F14 ohne Erosion) 3,81; F7 Fensterlage + er2 2,73 | OK |
| FS Unterarm (W)-rechts Id 14 (O5) | 9,0/-3,8/3,5; Anteil L* > 30 <= Original + 0,05 | 11,4, 0,000 | Original-Box 298-325 x 262-340: 9,0/-3,6/3,4, p90 12,5. V4 r3 11,4/-2,0/2,3, dE00 2,61, dL +2,3, Anteil 0,000, p90 15,5. V4 r2 28,2, Anteil 0,337 | OK |
| **FS Oberarm (B)-rechts unten** | Rotanteil wie Original +-0,15 | ROT 0,49 | Original-Box 455-500 x 228-262: 20,6/3,1/-0,5, Rotanteil 0,04. **V4 r3 21,5/7,0/2,1, dE00 4,89, Rotanteil 0,49**; V4 r2 22,2/4,1/0,1, dE00 1,75, Rotanteil 0,09 | **ROT** (Rueckschritt, 4.3) |
| FS Oberarm (B)-links unten | | 2,7 | 14,6/3,0/0,5 gegen 11,3/2,1/0,9: dE00 2,43, dL +3,3 | OK |

**Zaehlung nach eigener Messung:**

- **Rot** sind S3, T2 Lage, T5, F7 nach Wortlaut und FS Oberarm (B)-rechts unten.
- **Knapp** sind K1 (Rand 0,4) und F8 nach Wortlaut (dL +3,5 bei Grenze 4).

## 2. Tests und Gegenprobe

**Bau**

- exe re15_port/build/platform/pc/re15_pc.exe, Stand 11:12:49. Sie ist juenger als schwanz_r41.c (11:12:47), birkin_formen.c und der
  Header.
- Die Quellen sind sauber gegen HEAD.
- `ninja -n` meldet "no work to do". Einen eigenen Bau habe ich nicht gestartet (parallele Agenten, gleicher build/).

**ctest** -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --timeout 900:

- **4/4 Passed**, 64,27 s (SF_belege/v4/PZ3_ctest.txt).

**Reichweite der Tests:**

- Im Diff ee470c381..HEAD ist keine Testdatei geaendert.
- Ohne Test sind die neuen Engine-Regeln:
  - Mitnahme 0,80, Feder x1,5, Erhalt 0,50;
  - kein haengen() fuer V4;
  - Freigabe h70-h74;
  - Bodenpruefung mit Mesh-Radius und Dorn;
  - re15_schwanz_r41_ziel_gelenk.
- unit_schwanz_r41 Teil 9 arbeitet weiter mit den Gliedlaengen von V4 r2 (624/492/589/582/493/258). V4 r3 hat 624/464/689/580/414/258.
- integration_r41_sichel_form laedt das Spiel-Asset Ref2.
- Der Bauer nennt das selbst (Offen 7).

**Gegenprobe V3 r2** (eigene Messung). Rot sind:

- Kopf vorn:
  - K1 (Kontrast 2,8), K3 (0,003), K4 (-0,034);
  - K5 (13,6), K6 (34,2), K7 (8,5), K8 (23,9 / Anteil 0,70);
  - K9 (16,6), K10 (0,126);
  - K11 (Kopf 28,5 < 34,9).
- Kopf seitlich: S1 (2 Komponenten), S2 (0,078 / 0,900), S3 (24,5), S4 (17,4).
- Schwanz und Lauf:
  - T2 22,9 Grad;
  - T8 F15 8,9 / F16 9,0 / F17 6,5;
  - Lauf-Form 35/47 Gangtakte > 0,06 H;
  - Boden +94 E. (19 Takte).
- Koerperfarben: F6 9,8; F8 11,1; F9 9,4; F14 19,1; F19 5,6; Unterarm Id 14 21,3 L* (Anteil 0,121).

**Die Riegel trennen V3 r2 klar.**

**Gegenprobe V4 r2 bei den behobenen Befunden** (eigene Messung). Alle sind dort rot und in V4 r3 gruen:

| Befund | V4 r2 | V4 r3 |
|---|---|---|
| K11 | Kopf 27,7 < 34,9 | Kopf 36,1 |
| Kopf vorn, Fenster "oben" | Gradient 22,9 (Original 42,5) | 45,9 |
| T2-Drehung | 52,5 | 70,6 |
| FS Id 14 | 28,2 L*, Anteil 0,337 | 11,4 L*, Anteil 0 |
| F8 Wortlaut | dL +4,8 | +3,5 |
| F14 | er2 dL -5,2, ohne Erosion -8,2 | er2 -3,0 |
| T7 Form (Kabsch) | 33/47 Takte > 0,06 H (max 0,181) | 0/47 |
| h100 Welt-Abfall | -0,002 H (waagerecht) | 0,248 |
| Boden | Dorn +57 E. (h34) | -30 E. |

## 3. Spiellauf (eigener Lauf sfv4/pz3/lauf, bitgleich zu sfv4/r/V4r3)

**Koerperfest (O3):**

- Ich zaehle 47 Gangtakte: h85 und alle Takte mit clip 1 von h90 bis h135, also weit ueber die geforderten 6.
- Kette gegen Ziel, Gelenke 1/3/5/7: max 0,042 H (h85 Ende), 0 Takte ueber 0,06 H.
- Ohne die Ziel-Zeile der Engine, per Kabsch auf die Ruheform der Pose 85 aus dem EMD: max 0,040 H, Median 0,014 H. Die Form
  (Biegung 51,7-59,2 Grad, Knick 10,9-16,1 Grad) bleibt also im Gang erhalten.
- Die Breite ist starre Geometrie. Damit gilt sie im Gang so, wie T5 sie an der Ruhe-Pose misst, also mit der Ausbuchtung aus 4.2.

**Rueckfaltung:** keine, Gelenkwinkel max 60,9 Grad im ganzen Lauf.

**Boden:**

- Ich habe alle Ecken der Meshes 16-22 geprueft (eigener MD1-Parser). Die Gliedachse ist bei den Knochen 17-22 die y-Achse (rel =
  (0, L, 0)); den Rollwinkel schaetze ich nach oben ab.
- Der tiefste Weltpunkt im ganzen Lauf liegt bei -30 E. (h68), danach folgen h32-h35 mit -33 bis -44 E. Kein Takt liegt darunter.
- Die Engine-Schranke (Kugel mit dem Mesh-Radius am Gelenk plus Dorn) deckt den Ueberstand der Meshes 16-21 ueber ihre Gelenke nicht
  ab: Mesh 18 reicht 149 E. ueber Gelenk 3 hinaus, Mesh 16 393 E. hinter Gelenk 0. Die Luecke betraegt hoechstens
  sqrt(149^2 + 337^2) - 337 = 31 E.
- Im Lauf traegt das nicht. Es ist ein Hinweis fuer den Test der Regel.

**Licht und CLUT:**

- Die neuen Zeilen 488/489 kommen im Spiel an. Mesh 14 und Mesh 10 sind im FRAMEDUMP so hell wie im b2-Render (Tabelle G1-Zeile).
- h100 weicht fuer alle Ids um 3-9 dE00 ab, weil die Pose dort ueberblendet wird. Der Riegel misst deshalb h118/h135.

## 4. Befunde im Einzelnen

### 4.1 T2 Lage: nach dem O2-Werkzeug rot, die gruene Lage kommt aus der ausgeschlossenen Mittellinie

Was riegel_v4.py misst:

- Die T2-Drehung kommt aus t2_mass_v4.port, also aus der Pruefer-Mittellinie.
- Die T2-Lage kommt dagegen aus s['T2'] = v3/P_schwanz.mittellinie, der Bauer-Mittellinie.
- Ueber die Bauer-Mittellinie hat O2 entschieden: Sie ist am Original nicht stabil (53,3 / 65,3 / 61,8).

In einer Riegelzeile stehen damit zwei Verfahren.

Mit dem einen Werkzeug ist die Biegung die Mitte des Fensters mit der groessten Drehung (t2_mass_v4.drehung, Rueckgabe "mitte"):

| | Fenstermitte (px) | dx / dy vom Beginn (H) |
|---|---|---|
| Original, Maske A / hi / lo | (288,8 / 257,4), (289,4 / 257,1), (289,1 / 257,0) | -0,269 / +0,057, -0,267 / +0,056, -0,269 / +0,055 (Beginn = Nutzerpunkt 357,243) |
| Messblatt T2 | (295,0 / 252,1) | -0,245 / +0,036 |
| V4 r2 | (290,2 / 248,9) | -0,264 / +0,049 (OK) |
| **V4 r3** | (284,0 / 264,3) | **-0,280 / +0,100** (Beginn T1 = 358,238) |

- Gegen das Messblatt liegt V4 r3 um dy +0,064 daneben, gegen das Original mit demselben Werkzeug um +0,044. Erlaubt sind 0,03.
- Ursache: Der Generator setzt das Biegegelenk selbst schon tiefer, Gelenk 3 = (-0,2617, +0,0582) (r3_form_suche, Runde 2:
  (-0,245, 0,035)). Ausserdem ist die Biegung wieder auf zwei Stufen verteilt.

Richtungsprofil (t2_mass_v4, Grad, 0 = waagerecht nach hinten):

| s (H) | 0,25 | 0,30 | 0,35 | 0,375 | 0,40 | 0,425 | 0,45 | 0,475 | 0,50-0,575 |
|---|---|---|---|---|---|---|---|---|---|
| V4 r3 | 28 | 32 | 43 | 48 | 46 | **36** | 65 | **101** | 103-104 |
| Original Maske A (s ab Saat) | 27 | 21 | 37 | 64 | 84 | 92 | 89 | 71 | 65 |

- Vor der Biegung dreht die Richtung bei V4 r3 um 12 Grad zurueck (48 -> 36), dann springt sie auf 101-104 Grad. Das Original
  erreicht 92 Grad.
- Die 70,6 Grad sind also eine Drehung ueber einen Rueckschwung und ein Ueberschwingen hinweg, keine einzelne Biegung wie im Original.

### 4.2 T5: Ausbuchtung an der Biegung, Breitenprofil nicht mehr monoton

Gemessen ist die Breite quer zur eigenen Mittellinie in allen Schwanz-Ids, Achse Knick -> Beginn. Zwei Achsanfaenge:

| Achsanfang | Profil f 0,0 ... 1,0 (je 0,1) bzw. f 0,1/0,3/0,5/0,7 | Abfall |
|---|---|---|
| V4 r3, ab T1 | 0,023, 0,032, 0,045, 0,080, **0,188**, 0,138, 0,129, 0,127, 0,136, 0,142, 0,142 | **0,061** |
| V4 r3, ab Saat | 0,032 / **0,095** / 0,129 / 0,142 (f 0,3 0,095 gegen 0,065 +-0,02: auch dieser Punkt rot), Spitze 0,153 bei f 0,35 | **0,027** |
| V4 r2, ab T1 | 0,036 / 0,070 / 0,125 / 0,131 | 0,006 |
| V4 r2, ab Saat | 0,040 / 0,078 / 0,125 / 0,138 | 0,009 |

Am Original (Leser-Masken, gleiches Verfahren) steigt die Breite zwischen f 0,3 und 0,5 von 0,069-0,073 auf 0,117-0,121, ohne
Ausbuchtung. Das gilt mit einem Vorbehalt: Maske A ist am Knick unterbrochen, der Knick-Anschluss ist dort unsicher. Der Abfall am
Original betraegt 0,000 / 0,016 / 0,020; das ist die Streuung des Verfahrens. V4 r3 liegt mit 0,061 beim Dreifachen.

**Belegbild:** SF_belege/v4/PZ3_t5_breitenprofil.png (Original Maske A | V4 r2 | V4 r3, Ausschnitt x 250-400, y 180-360, x3; der
rote Strahl ist f 0,4). Bei V4 r3 steht die Aussenecke der Biegung als **Spitze** heraus. Der Strahl quer zur Mittellinie laeuft dort
diagonal ueber die Ecke. Bei V4 r2 und im Original ist die Aussenseite gerundet.

**Ursache:**

- Die Durchmesserliste lautet `durchmesser = [0.16, 0.13, 0.1243, 0.1599, 0.0452, 0.027, ...]` (birkin_formen.py SCHWANZ_V4, aus
  r3_form_suche Saat 2 Schritt 78).
- Gelenk 3 (Biegung) ist mit 0,1599 dicker als Gelenk 2 (0,1243) und als das Maximum der Messblatt-Breitenzeile (0,14).
- Messblatt Abschnitt 4 Punkt 3 verlangt "Breite je Glied nach der Breitenzeile (0,14 -> 0,125 -> 0,065 -> 0,03 H zum Knick)".
- Die Bauer-Messung (P_schwanz.dicke) zeigt die Ausbuchtung nur als 0,140 -> 0,131: Abfall 0,009, also 0,001 unter der Grenze.
- Die Formsuche hat auf die eigenen Riegel mit 0,003 H Marge optimiert. Ein unabhaengiges Breitenmass findet den Abfall. Das ist die
  Lage "selbstbestaetigende Metrik".

### 4.3 FS Oberarm (B)-rechts unten: Rueckschritt gegenueber Runde 2

| Stand | Lab | Rotanteil | dE00 |
|---|---|---|---|
| Original (Box 455-500 x 228-262) | 20,6/3,1/-0,5 | 0,04 | - |
| V4 r2 | 22,2/4,1/0,1 | 0,09 | 1,75 (OK) |
| **V4 r3** | 21,5/7,0/2,1 | **0,49** | 4,89 (Grenze 5) |

- Grund (r2_koerper_farben.py Region 'oberarm_br'): Der ganze Mesh 10 bekommt die Verteilung des roten F8-Fensters. So wird F8 nach
  Wortlaut gruen, aber der untere Oberarm wird rot.
- Der Bauer belegt mit Lauf r3 Koerper 2, dass F8 nach Wortlaut (dL +4,4) und die Sicht-Box (20,6 L*) mit getrennten Regionen nicht
  zugleich zu erreichen sind (Offen 3).
- Dasselbe Messblatt-Problem wie bei F7 (4.4): "Id ganz" gegen ein Teilfenster. Rot, Messblatt-Entscheid noetig.

### 4.4 F7 nach Wortlaut

**Formal rot:** 23,0 L*, dL -10,3.

**Sachlich** zeigt der eigene Code, dass der Arm in beiden Teilen wie das Original gefaerbt ist:

- oben (Fensterlage) 31,4 gegen 33,3, dE00 3,63;
- unten 14,6 gegen 11,3, dE00 2,43.

Das F7-Fenster (307-347 x 167-212) deckt nur die obere Haelfte des (B)-linken Oberarms ab. "Id 13 ganz gegen dieses Fenster" kann
ein korrekt gefaerbter Arm nicht erfuellen. Das ist ein Widerspruch im Messblatt: Fensterlage oder Id ganz. Die Entscheidung liegt
beim Orchestrator bzw. beim Messblatt. Bis dahin bleibt die Zeile rot.

### 4.5 Gang: koerperfest ruhig, in der Welt flach waehrend der Ueberblendung; Kette fast starr

**Welt-Abfall:**

- Der Welt-Abfall von der Biegung zum Ende betraegt in h88-h99 nur 0,091-0,10 H, in Ruhe 0,389 H.
- Ab h100 liegt er bei 0,25-0,39 H.
- Die Kette haelt in h88-h99 ihre Ruheform am Koerper (Kabsch 0,02-0,07 H). Der Koerper ist in der Ueberblendung aus clip 11 aber
  vorgeneigt, und der Schwanz ragt dann flach nach hinten.
- Nach O3 (koerperfest) ist das OK. Fuer die Sichtpruefung: Im Spielbild h88-h99 sieht der Schwanz waagerechter aus als in Vorlage 2.

**Mitnahme 0,80:** Die Kette folgt ihrem Ziel fast starr (Abweichung im Gang Median 0,014 H). Damit ist T7a weitgehend durch die
Bauart erfuellt. Der Bauer belegt das mit einer Messreihe; mit 0,60 lag h85 bei 0,103 H.

**Freigabe des Szenenziels:** Sie liegt jetzt bei h70-h74 statt h74-h80 (V2) bzw. h102-h108 (Ref2). Der Schwanz ist ab h74 frei
sichtbar. Vorlage 2 (h85) zeigt ihn im Video ebenfalls frei. Fuer G2 (Szenenablauf) ist das eine sichtbare Aenderung der Szene ab h74.

### 4.6 K11: Konstruktion der Toleranz

- Die Toleranz 6,1 ist die halbe Spanne der Kopfviertel. Mit der Spanne der Box-Verschiebungen (3,5) allein laege das Band bei
  37,5-44,5, und 36,1 waere rot.
- Ausserdem vergleicht die Bauer-Definition eine erodierte Original-Box, die Silhouettenkanten und Hintergrundecken enthaelt, mit dem
  erodierten Id 9 bei 1,2-fachem Massstab (H 364 gegen 302).
- Mit gleichem Massstab und gleichen Kopfinnen-Fenstern (pz3_k11 b) liegt V4 r3 bei 36,3-45,9 und das Original bei 24,8-44,2. Der
  ganze skalierte Kopf misst 41,1 gegen 41,0. **K11 ist damit auch bei strengerer Messung gruen.**

**Hinweis zu O4 "rot-fleckig":** Der Rotanteil (R-G >= 15) im Kopfinneren ist niedriger als im Original.

| Fenster | Original | V4 r3 | V4 r2 |
|---|---|---|---|
| innen | 0,61 | 0,37 | 0,48 |
| oben | 0,94 | 0,73 | - |
| unten | 0,60 | 0,20 | - |

- Die Streuung von a* ist doppelt so gross: innen 2,67 gegen 1,37.
- Das Mittel von a* stimmt: oben 14,4 gegen 14,3.
- Der Kopf ist also staerker fleckig, aber weniger flaechig rot als das Original. Kein Riegel ist verletzt (K9 gruen); das ist ein
  Punkt fuer die Sichtpruefung.

### 4.7 F14 und Erosion

Nach der Lesart beider Seiten seit Runde 1 (2 px erodiert, Randpixel nach sigma 1,5 gemischt) ist F14 gruen: dL -3,0. Ohne Erosion
waere dL -6,3. Das Messblatt sagt zur Erosion nichts. Ich werte nach der einheitlichen er2-Lesart wie bei F6/F8 und nenne den
Wortlautwert als Hinweis.

## 5. Konstanten im Diff ee470c381..HEAD (O6)

| Stelle | Wert | Quelle laut Kommentar | Urteil |
|---|---|---|---|
| schwanz_r41.c SW_V4_ERHALT / SW_V4_FEDER / SW_V4_MITNAHME | 0,50 / 1,5 / 0,80 | GESTALTUNG (PORT-WAHL), Messreihe Laeufe V4r3c-g (EMD V4r3y) | Messreihe belegt; **PORT-WAHL ohne Nutzer-Wortlaut** (O6) |
| schwanz_r41.c kein haengen() fuer V4 | - | Lauf V4r3b, PORT-WAHL | dito |
| schwanz_r41.c sw_mesh_mass (Radius, Dorn) | aus der Bank gemessen | "Gemessen, keine Konstante" | belegt. Achse = y stimmt (rel 17-22 = (0, L, 0)); Ueberstand der Meshes 16-21 nicht abgedeckt (<= 31 E., 3) |
| birkin_formen.c k_schwanz_gewicht_v4 | {15,0} {19,4096} {70,4096} {74,0} | 15/19 wie Ref2/V2; 70/74 Lauf V4r3m0.95 (Feder ~15 Takte bis h85) | belegt (Quelle Lauf); nicht als PORT-WAHL gekennzeichnet |
| birkin_formen.py SCHWANZ_V4 gelenke 2-4 | (-0,1345,-0,0163) (-0,2617,0,0582) (-0,2577,0,1834) | r3_form_suche Saat 2 Schritt 78 | belegt als Suchergebnis; **Gelenk 3 dy +0,058 liegt selbst 0,022 unter dem Messblatt-Wert +0,036** (4.1) |
| SCHWANZ_V4 durchmesser 2-4 | 0,1243 / **0,1599** / 0,0452 | r3_form_suche | **widerspricht Messblatt Abschn. 4 Punkt 3** (monoton, max 0,14), 4.2 |
| SCHWANZ_V4 tiefe 2/3 | 0,0822 / 0,0443 | PORT-WAHL, r3_form_suche | als PORT-WAHL gekennzeichnet (r2-Befund behoben); ohne Nutzer-Wortlaut |
| KOERPER_ZEILEN_V4 | {10: (489,480), 14: (488,482)} | O1, r2_uv_belegung, Messblatt F7/F8, Sicht 1.4 | belegt; im Spiel wirksam (3) |
| r2_koerper_farben ZEILEN/START, Region oberarm_br, unterarm_wr | 488/489; Boxen | Messblatt F8, Sicht-Box 298-325 x 262-340 | belegt; oberarm_br ist die Ursache von 4.3 |
| eichen_v4 AUGE_RADIAL_VORN | 0,55 | Lauf r3 Struktur 8/9, PORT-WAHL | ohne Nutzer-Wortlaut |
| eichen_v4 S2_MITTE | (0,045, 0,79) | Abweichung zu 0,049 erklaert (gemessen 0,058), PORT-WAHL | r2-Befund behoben |
| eichen_v4 AUGE_S_VOR | 0,0 | Laeufe r3 S3 1/2, zurueckgenommen | belegt (aus) |
| eichen_v4 STRUKTUR_SIGMA / STRUKTUR_GAIN | 3,0 / 10,0 | Runde 1 / "nachgestellt am Riegel K11, Laeufe r3 Struktur 1-n" | Quelle Laufreihe; GAIN ist am eigenen Riegel nachgestellt (K11 strenger nachgerechnet gruen, 4.6) |
| eichen_v4 STRUKTUR_GAIN_FARBE | 1,5 | PORT-WAHL | ohne Nutzer-Wortlaut |
| eichen_v4 **STRUKTUR_FARBE_KLAMMER** | **0,4** | **keine Quelle im Kommentar** | **Beanstandung** (O6) |
| eichen_v4 **STRUKTUR_KLAMMER** | **2,0** | Kommentar: "log Faktor >= -1,0 (Faktor >= 0,37)" | **Kommentar widerspricht dem Wert** (2,0 heisst Faktor >= 0,135; Dossier "Klammer -2,0") |
| eichen_v4 STRUKTUR_HELL | 0,15 | Lauf r3 Struktur 2 (Komponenten K1/S1) | belegt |
| eichen_v4 **STRUKTUR_ZONE_R3** | **0,0** (6 Zonen inkl. v_nase) | Kommentar: "halbe Struktur", "auch dort nur halbe helle Struktur" | **Kommentar widerspricht dem Wert** (0,0 = keine helle Struktur; das Dossier sagt "helle Stufen aus") |
| eichen_v4 AUGE_HOF_1 / _2 / HOF_DUNKEL / HOF_DUNKEL_S | 2,0 / 4,0 / 0,5 / 0,6 | Laeufe r3 Struktur 3-15, PORT-WAHL | ohne Nutzer-Wortlaut |
| eichen_v4 k-Mittel-Saat | 41 | - | Determinismus, kein Verhalten |
| eichen_v4 Umgebungs-Schalter RE15_V4_* | os.environ-Vorgaben | - | Hinweis: der Generatorstand haengt von Umgebungsvariablen ab (bitgleicher Neubau nur ohne gesetzte RE15_V4_*) |

**Zusammenfassung O6:**

- Jede neue Konstante nennt einen Lauf oder ein Messblatt-Mass, mit einer Ausnahme: STRUKTUR_FARBE_KLAMMER hat keine Quelle.
- **Keine der 8 neuen PORT-WAHL-Stellen nennt einen Nutzer-Wortlaut.** O6 verlangt "PORT-WAHL mit Nutzer-Wortlaut". Im Dossier steht der
  Nutzersatz nur allgemein am Abschnittskopf.
- Zwei Kommentare widersprechen ihren Werten: STRUKTUR_KLAMMER und STRUKTUR_ZONE_R3.

## 6. Offen fuer Bauer und Orchestrator

1. **T2 Lage:** T2 vollstaendig mit dem O2-Werkzeug messen, also Drehung und Fenstermitte. Mit ihm liegt V4 r3 bei dy +0,100 H
   (Soll +0,036, Original mit demselben Werkzeug +0,056). Gelenk 3 sitzt im Generator bei +0,058. Ausserdem den Rueckschwung 48 -> 36
   Grad vor der Biegung beseitigen.
2. **T5:** Die Ausbuchtung an der Biegung beseitigen (Durchmesser Gelenk 3 0,1599 > Gelenk 2 0,1243; spitze Aussenecke). Danach
   T5 mit einem zweiten, unabhaengigen Breitenmass gegenpruefen. Das Bauer-Mass zeigt den Abfall mit 0,009 knapp unter der Grenze.
3. **FS Oberarm (B)-rechts unten:** Rotanteil 0,49 gegen 0,04, Rueckschritt gegenueber V4 r2. Das braucht zusammen mit F7 einen
   Messblatt-Entscheid: "Id ganz" oder Fensterlage plus untere Sicht-Box.
4. **S3:** Abstand 0,015-0,019 H, unveraendert rot. Der Weg des Bauers ist die Naht-Verlegung.
5. **O6:** Nutzer-Wortlaut zu jeder PORT-WAHL; Quelle fuer STRUKTUR_FARBE_KLAMMER; die Kommentare zu STRUKTUR_KLAMMER und
   STRUKTUR_ZONE_R3 an ihre Werte angleichen.
6. **Tests:** Ein Unit-Test fuer die neuen V4-Regeln fehlt (Mitnahme, Freigabe, Boden mit Mesh). Teil 9 auf die Gliedlaengen von V4 r3
   ziehen. Die Bodenregel um den Achsueberstand der Meshes 16-21 ergaenzen (<= 31 E.).
7. **Hinweise fuer die Sichtpruefung:**
   - Kopf staerker fleckig, aber weniger flaechig rot als im Original (4.6).
   - Schwanz im Spielbild h88-h99 flach (4.5).
   - Spitze Aussenecke an der Biegung (4.2).
   - K1 knapp (Rand 0,4).

**Belege** (absolute Pfade):

- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_t5_breitenprofil.png
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_mess_V4r3_V4r2_V3r2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_t2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_k11.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_lauf_V4r3.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_ausgaben.txt (Lauf, G1, FS, T5, Licht,
  Farben)
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ3_ctest.txt
- Scratchpad: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz3/
  (Neubau emd/, eigener Lauf lauf/, Renders r_*.npz/png, licht_h*.png, takt_eigen.txt/takt_bauer.txt)
