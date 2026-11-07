# SF V4 Runde 4: Zahlen-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Bau V4 Runde 4 der Sichel-Form, Commits 6488773d2, 41ac7f058, 8ee38bb7d, 5e56f89a0, 89a05d1b0, 72c7d7d4a, 5f71e11fe und
cbc917a48 auf r41/sichel_nb2 (349f4360c = Pruefberichte r3). Modell EM30_V4.EMD, sha256 00d642b7. Gemessen wird gegen
SF_MESSBLATT_V4.md (Commit 7a4f3b684), die neuen Riegelzeilen aus der Sicht-Pruefung V4 r3 (TQ, TK, FO, KS) und die Entscheide O1-O6.
Kein RE-Befund, keine @0x-Konstante: die Sichel-Form gibt es auf der Disc nicht, alles ist Bildmessung am 80%-Video 4.

## Ergebnis: nicht bestanden

**Die 11 roten Zeilen des Bauers bestaetige ich alle mit eigenem Code:**

- **T3** dx -0,234 (Grenze -0,235; Bauer -0,233)
- **T5 / T5q** f 0,5 = 0,095 H (Soll 0,125 +-0,025; Original 0,117-0,121)
- **F6 nach Wortlaut** 22,3/9,7/3,8, dE00 7,09
- **F7 nach Wortlaut** 23,0/4,8/3,1, dE00 7,82, dL -10,3
- **TQ** Plateau-Anteil 0,41-0,53 (Original <= 0,31)
- **TK** RMS 1,77 px (Original 3,87-4,71)
- **FO** Buehne 2,33, Spielbild h118 1,55, h135 1,49 (Ziel >= 2,75); Schultern im Spielbild 0,081 / 0,070 (<= 0,05)
- **KS h135** glatte Pixel 0,134 (<= 0,098)

**Nach eigener Messung kommen hinzu:**

- **F-Mittel nach Wortlaut, neu rot.** Mit F6/F7/F8 Id ganz und ohne Erosion ergibt sich 4,52 (Grenze 4; r3 3,81). Mit 2 px Erosion
  sind es 3,99, also knapp. Die gruenen 2,97 des Bauers mitteln F7/F8 an der Fensterlage. Wie F7 haengt die Zeile am offenen
  Messblatt-Entscheid.
- **TK Oberkante, formal ueber der Grenze.** Bei x 280 liegt sie 8 px ueber dem Original (242 gegen 250). Das sind 0,0303 H und
  damit mehr als 0,03 H. Der Bauer rundet auf "0,030". Die Zeile ist ohnehin wegen RMS rot.
- **O6, nicht erfuellt.** 5 neue PORT-WAHL-Stellen nennen keinen Nutzer-Wortlaut. Ein Kommentar widerspricht seinem Wert.
- **Eichung des Schwanzes weicht vom Dossier ab.** Code und Dossier sagen Verschiedenes, Abschnitt 4.2. Die Regionen F16 und F17
  sind in eichen_v4.py Zeile 783 auskommentiert. Die Farbdatei farben_v4.npz ist aus genau diesem Code entstanden (eigener Nachbau
  bitgleich). Seit Runde 3 faellt F17 von 1,82 auf 5,96 (Grenze 6).

**Bestaetigt (gruen) sind:**

- G1 nach O1, Spiel-Asset und Pin, M1-M4
- K1-K11, S1, S2, S3 (jetzt gruen), S4
- T1, T2 Lage und Drehung (O2), T4, T6, TA, T8
- T7 koerperfest nach O3, Boden mit allen Mesh-Ecken
- F0, F7/F8 an der Fensterlage, F8 nach Wortlaut (2 px erodiert), F9, F10, F14 (erodiert), F19
- alle 6 FS-Boxen, darunter Oberarm (B)-rechts unten (behoben) und Unterarm Id 14 (O5)
- KS Buehne und KS h118
- die 4 Tests, der Spiellauf (bitgleich nachgefahren) und die Gegenproben

**Diese Befunde aus Runde 3 sind behoben:** S3, T2 Lage, T5 Monotonie, FS Oberarm (B)-rechts unten, TQ Stufe und O6 Runde 3.

## 0. Verfahren (eigener Code, nicht riegel_v4.py)

Die Werkzeuge liegen in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4/ und sind nicht committet:

- pz4_*.py sind Kopien der Pruefer-Werkzeuge pz3_* aus Runde 3, umgestellt auf die Renders in sfv4/pz4.
- Neu geschrieben sind pz4_form.py, pz4_t8.py, pz4_t9.py und pz4_m4.py.

Die Ausgaben stehen unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz4/.
Die Belege stehen unter C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_*.

| Werkzeug | misst |
|---|---|
| birkin_formen.py --variante v4 --aus sfv4/pz4/emd | Neubau aus HEAD cbc917a48: **sha256 00d642b7e88c55f3...** = Bauer-Meldung = sfv4/emd/EM30_V4.EMD = EM30_V4r4.EMD |
| eichen_v4.py --aus sfv4/pz4/farben_neu.npz --arb sfv4/pz4/eich (ohne RE15_V4_*) | Nachbau der Farbdatei: **sha256 971b6cc7 = farben_v4.npz im Baum** (bitgleich, 63 s) |
| lauf_v4.sh (eigener Lauf sfv4/pz4/lauf, exe build/ 12:37:35, ninja -n "no work to do") | **672 hangar-takt-Zeilen bitgleich** zu sfv4/r/V4r4, **191 von 191 FRAMEDUMPs bitgleich** |
| pz4_render.py | b2.zeichne (Messrender, Messblatt Abschn. 7) fuer V4 r4 (eigener Neubau), V4 r3 (dfbc30bc), V3 r2, Ref2; Posen "85"/"135" |
| pz4_mess.py | K1-K10, S1-S4, T1/T3/T4/T6, T8, F6-F19, F0 im Videoraum (Vergleichsregel sigma 1,5, a/b), eigene Mittellinie |
| pz4_t2.py, pz4_t5orig.py, pz4_t5.py | T2 nach t2_mass_v4 (O2), Drehung und Lage; T5 quer zur Mittellinie am Original (3 Masken) und am Port, Achse ab T1 und ab der Saat |
| pz4_g1.py, pz4_g1geo.py, pz4_m4.py | eigener EMD/MD1/EMR/TIM-Parser: Geometrie, CLUT, Keyframes, Dreiecke, Texeldichte |
| pz4_farben_alt.py, pz4_fs.py | F6/F7/F8/F14 in allen Lesarten, F-Mittel, Sicht-Boxen |
| pz4_t8.py | T8 mit den Punkten nach Wortlaut (Beginn T1, Biegung = O2-Fenstermitte) |
| pz4_form.py | TQ/TK (mit Gegenformen), KS (Luma und L*, 9 verschobene Fenster), FO (Drittel) am Original, an der Buehne und an den eigenen Spielbildern h118/h135 (c6.port) |
| pz4_lauf.py | Lauf: Kette gegen Ziel (O3); Form gegen die Ruheform per Kabsch; Welt-Abfall; Boden mit allen Ecken von Mesh 16-22 |
| pz4_licht.py | Lichtgegenprobe b2 gegen FRAMEDUMP je Id-Gruppe (auch die neuen CLUT-Zeilen 488-491) |

**Selbstpruefung:**

- V4 r3 reproduziert mit pz4_* die Werte der Runde-3-Pruefung, zum Beispiel:
  - S3 0,019 H;
  - T2 70,6 Grad bei -0,280 / +0,100;
  - T5 Abfall 0,061;
  - FS Oberarm (B)-rechts unten Rotanteil 0,49.
- Die Originalwerte der neuen Riegel stimmen mit dem Bauer und der Sicht-Pruefung r3 ueberein:
  - TQ Stufe 5,4-9,6, Plateau <= 0,31;
  - TK RMS 3,87 / 4,71 / 4,69;
  - KS 29,1-40,4, glatt 0,073-0,098;
  - FO 2,91 / 2,75.

## 1. Riegel-Tabelle Soll / Bauer / Pruefer

Massstab:

- H vorn (Pose 135) = 364 px.
- H seitlich (Pose 85) = 264 px.
- H_E im Lauf = 3775 E.

| Riegel | Soll | Bauer | Pruefer (eigener Code) | Bewertung |
|---|---|---|---|---|
| G1 (O1) | Geometrie 0-8/10-15 gleich, Skelett/KF/EDD gleich, CLUT frei | OK | Geometrie-Abweichungen **keine** (Ecken, Normalen, Flaechen, UV ohne CLUT-Feld, TPage). CLUT-Feld 2/3/5/6 -> 491, 10 -> 489, 13 -> 490, 14 -> 488. Byte-gleich 0, 1, 4, 7, 8, 11, 12, 15. EMR rel 0-15 und Tabelle gleich. 912 KF x 116 Byte, Bytes 0-83 gleich. Sektionen 0, 1, 3-6 gleich. CLUT 480/481/482/484 geaendert, 483/485 gleich, 486-491 neu (Messblatt G1: TIM/CLUT frei) | OK |
| Spiel-Asset / Pin | unveraendert | unveraendert | EM30_HANGAR.EMD sha256 cfd1ac42... = release/em30_hangar.sha256. git diff 84e8a4d97..HEAD leer. Tests, Version_80%/, pri/, synchro/ unveraendert seit ee470c381 | OK |
| M1 / M2 / M3 / M4 | <= 212 / <= 118 / 8 bpp / 0,045-0,091 | 192 / 109 / ok / 0,050-0,091 | 192 (36/24/24/24/24/24/36) / 109 / TIM-Flag 0x9, 12 CLUT-Zeilen / Kopf 0,0570, Schwanz 0,0503-**0,0905** | OK (Rand M4 Glied 20: 0,0005) |
| K1 | 1 Komponente, Kontrast >= 20 | 1; 26,1 | 1 (61 px); 26,1 | OK |
| K2 | dE00 <= 6; L* >= KV5 + 20 | 0,8 | 47,7/8,5/-1,5, dE00 1,54; 47,7 >= 22,6 + 20 | OK |
| K3 | 0,016-0,036 x 0,023-0,043 | 0,030 x 0,025 | 9 x 9 px = 0,025 x 0,025 | OK (Rand Hoehe 0,002) |
| K4 | 0,035-0,065; 0,07-0,12 | 0,057 / 0,102 | 0,056 / 0,102 | OK |
| K5 | dE00 <= 6; L* <= Auge - 25 | 1,0 | 13,9/8,2/0,9, dE00 0,98; 13,9 <= 22,7 | OK |
| K6 | L* <= 18 | 12,5 | 12,3 | OK |
| K7 | dE00 <= 6 | 1,2 | 18,1/7,1/-0,1, dE00 1,21 | OK |
| K8 | dE00 <= 6; Anteil L* > 35 <= 0,05 | 1,7 / 0 | 11,7/8,5/3,5, dE00 1,67; Anteil 0,000 (L* max 26,1) | OK |
| K9 | dE00 <= 6, a* >= 11 | 1,8 | 20,5/16,7/4,9, dE00 1,76 | OK |
| K10 | 0,14-0,18 x 0,15-0,20 | 0,154 x 0,187 | 0,154 x 0,187 | OK |
| K11 (O4) | 41,0 +-6,1; Kopf >= Rumpf | 37,5 / 37,0 | wie Runde 3 nachgerechnet (Werkzeug pz3_k11 unveraendert gueltig); hier nicht wiederholt, weil KS die Spielbild-Struktur misst (unten) | OK (Bauer) |
| S1 | 1 Fleck, dE00 <= 6, 0,028-0,052 x 0,040-0,070 | OK | 48,5/3,1/0,9, dE00 1,20; 11 x 13 px = 0,042 x 0,049; 1 Komponente, Kontrast 23,2 | OK |
| S2 | 0,03-0,07; 0,76-0,82 | 0,054 / 0,809 | 0,054 / 0,809 | OK |
| **S3** | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | OK 0,004 | 17,2/2,8/0,6, dE00 2,53; **Abstand 0,008 H** (Fenster 472-477 x 212-226). r3: 0,019 | **OK** (behoben; Rand 0,002 H = 0,5 px, 4.6) |
| S4 | dE00 <= 6, a* >= 11 | 0,4 | 27,6/15,6/2,9, dE00 0,38 | OK |
| T1 | 0,15 hinter, 0,767 ueber (+-0,03) | 0,172 / 0,742 | Id 16 an Id 7/8 = (358,238): 0,172 / 0,742 | OK |
| **T2 Lage (O2)** | dx -0,245, dy +0,036 (+-0,03) | -0,251 / +0,051 | t2_mass_v4-Fenstermitte (291,6 / 251,4) vom T1-Beginn (358,238): **-0,251 / +0,051**. Original mit demselben Werkzeug -0,268 / +0,056. r3: -0,280 / +0,100 | **OK** (behoben) |
| T2 Drehung (O2) | 68,4 +-5,0 | 66,6 | **66,6** (Original 70,1 / 65,8 / 69,5) | OK |
| **T3** | dx -0,265 dy +0,257 (+-0,03) | ROT -0,233 / 0,266 | Taille (296,2 / 301,0): **-0,234** / +0,239. Knick absolut (TA) +0,024 H neben dem Nutzerpunkt, aber der Port-Beginn liegt 1 px hinter und 5 px ueber dem Nutzer-Beginn | **ROT** (0,001-0,002 H) |
| T4 | dx -0,245 dy +0,397; 0,37 ueber Sohle | -0,215 / 0,385 | (300,7 / 340,9): -0,217 / +0,390; 0,353 ueber Sohle | OK |
| **T5** (Wortlaut: Achse Knick -> T1-Beginn) | 0,03/0,065/0,125/0,14; Abfall <= 0,01 | ROT 0,028/0,053/0,095/0,121 | eigene Mittellinie ab T1: 0,036/0,055/**0,095**/0,129, Abfall 0,000. Ab der Saat (Oberkante) waere es 0,036/0,081/0,117/0,146 (gruen) | **ROT** (f 0,5; 4.1) |
| **T5q** (quer, t2_mass_v4-Mittellinie) | wie T5 | ROT 0,032/0,055/0,095/0,129 | 0,036/0,055/**0,095**/0,129, Abfall 0,000; Original 0,121/0,119/0,117 bei f 0,5 | **ROT** |
| T6 | B 0,025-0,05, L 0,12-0,17, 75-95 Grad, Taille schmalste | OK (L 0,121) | Median 0,028, L 0,152, 83,7 Grad nach vorn, Taille 0,025 < 0,030 davor | OK |
| TA | Nutzerpunkte +-0,03 | <= 0,024 | Beginn +0,004 / -0,019, Knick +0,024 / 0,000 (Bauer-Punkt), Ende +0,023 / -0,013 | OK |
| T8 (Punkte nach Wortlaut) | dE00 <= 6; Wurzel a* >= 7, Ende a* <= 6 | 3,3/2,1/6,0/1,3 | F15 11,5/8,4/3,0 dE00 3,32; F16 2,03; **F17 20,5/2,6/2,2 dE00 5,96**; F18 14,4/4,1/1,8 dE00 1,17 (a* 4,1). r3: F17 1,82 | OK (**F17 Rand 0,04**, 4.2) |
| T9 (Bauer-Riegel, feste Achspunkte) | Mittel dE00 <= 6 | 3,9 | nicht nachgestellt. Eine unabhaengige Abtastung entlang der eigenen Mittellinie (7 Stellen, 5x5) ergibt V4 r4 8,5 und V4 r3 7,7. Die Abtastung ist stark lageabhaengig (zweitoniges Rohr). Die festen Achspunkte des Bauers sind zugleich Eichregionen (eichen_v4), daher ist die Zeile selbstbestaetigend | (Hinweis) |
| T7a (O3) | Gelenke 1/3/5/7 <= 0,06 H vom Ziel, Gang | 0/47, max 0,048 | 0/47, max 0,048 H (h85 Ende) | OK |
| T7b | Biegung >= KF - 10; Knick >= KF/2 | OK | Biegung 61,2-70,5 Grad (Ruhe 67,5), Knick 10,6-16,2 (Ruhe 15,8) | OK |
| T7c | keine waagerechte Kette (koerperfest) | min 0,389 H | koerperfest OK; in der Welt h88-h97 waagerecht bzw. aufgerichtet (4.4) | OK nach O3 (Hinweis 4.4) |
| T7d | <= 90 Grad | 72,7 | 72,7, ganzer Lauf | OK |
| Boden (O3) | kein Glied-Mesh, keine Dornspitze unter dem Boden | -13 E. | alle Ecken von Mesh 16-22: tiefster Punkt **-45 E. (h34)**, 0/144 Takte darunter | OK |
| F0 Licht / Wand | <= 3 / <= 2 | 0,1 / 0,2 / 0,52 | Rumpf h118 0,07, h135 0,19. Je Id h110-h135 0,04-0,90, darunter Mesh 13 (CLUT 490) 0,17-0,42 und Mesh 14 (488) 0,11-0,17. Wand 0,52 | OK |
| **F6 Wortlaut** | dE00 <= 5, Betrag dL <= 4 | ROT 7,0 | 22,3/9,7/3,8, dE00 **7,09**, dL +5,7 (er2 6,97 / +5,5) | **ROT** (Offen Messblatt) |
| F6 Fensterlage | (Bauer-Lesart) | 4,8 | nicht nachgestellt | (Angabe Bauer) |
| **F7 Wortlaut (Id 13 ganz)** | 33,3/5,5/2,7 | ROT 22,8 | 23,0/4,8/3,1, dE00 **7,82**, dL -10,3 (er2 7,93 / -10,5) | **ROT** (Offen Messblatt) |
| F7 Fensterlage (obere 58 %, er2) | | 2,1 | 31,2/6,3/4,0, dE00 2,13 | (Angabe) |
| F8 Wortlaut (Id 10 ganz) | 16,4/8,3/1,5, Betrag dL <= 4 | 3,6 / +3,8 | er2 20,2/5,9/0,9, dE00 3,56, dL **+3,8**. Ohne Erosion 20,8, dE00 4,12, **dL +4,4** | OK nach er2-Lesart (Rand 0,2; ohne Erosion rot, 4.3) |
| F9 / F10 / F19 | dE00 <= 5 | 3,0 / 2,8 / 1,6 | 2,93 / 2,94 (Id 12) / 1,38 | OK |
| F14 | 60,0/11,0/1,2, Betrag dL <= 4 | 2,2 / -2,5 | er2 57,5, dE00 2,24, dL -2,5. Ohne Erosion 54,2 / 5,36 / -5,8 | OK (er2 wie seit Runde 1) |
| **F-Mittel** | <= 4 | 2,97 (F7/F8 Fensterlage) | Wortlaut (Id ganz, ohne Erosion) **4,52**; Id ganz er2 **3,99**; F7 Fensterlage + F8/F14 er2 3,18 | **ROT nach Wortlaut** (r3 3,81; 4.3) |
| FS Unterarm (W)-rechts Id 14 (O5) | 9,0/-3,8/3,5; Anteil L* > 30 <= Original + 0,05 | 11,4, 0,000 | 11,4/-2,0/2,3, dE00 2,62, dL +2,3, Anteil 0,000, p90 15,5 (Original 12,5) | OK |
| **FS Oberarm (B)-rechts unten** | Rotanteil wie Original +-0,15 | 0,02 | Original 20,6/3,1/-0,5 Rot 0,04; V4 r4 22,6/4,6/0,6, dE00 2,48, **Rot 0,02** (r3 0,49) | **OK** (behoben) |
| FS Oberarm (B)-links unten | | 3,7 | 14,9/4,1/1,9 gegen 11,3/2,1/0,9: dE00 3,44, dL +3,6, Rot 0,11 (Original 0,00) | OK (Rand dL 0,4) |
| FS Rumpf+Schultern / Unterarm Id 11 / Unterschenkel | | 2,4 / 2,0 / 3,5 | nicht nachgestellt (Bauer-Werte) | (Angabe Bauer) |
| TQ Stufe | <= 9,6 L* auf 3 px | 8,2-8,7 | 8,2 / 8,2 / 8,7 / 8,7 (r3 16,6-19,8) | OK (behoben) |
| **TQ Plateau** | <= 0,31 | ROT 0,41-0,53 | 0,53 / 0,41 / 0,46 / 0,42. Gegenform: Schwelle 1,0 L*/px V4 r4 0,58-0,79, Original 0,63-0,79 | **ROT** nach Riegel (Messverfahren rauschempfindlich, 4.5) |
| **TK** | RMS >= 3,02 px; Oberkante <= 0,03 H | ROT 1,77 / 0,030 | RMS **1,77** (Sehnenabstand 6,7 px gegen 10,5-17,2). Oberkante 246/242/238/235/232 gegen 251/250/243/236/234: **x 280 8 px = 0,0303 H** | **ROT** |
| **FO Buehne** | Mitte/aussen >= 2,75; Schultern <= 0,05; a*>18 <= Original | ROT 2,33 | Drittel 0,274/0,589/0,231, **2,33**; Schultern 0,017; a*>18 0,016 (Original 0,041) | **ROT** |
| **FO Spielbild h118** | >= 2,75 (Video 2,91); Schultern <= 0,05 | ROT 1,55 / 0,081 | 0,341/0,474/0,271, **1,55**; Schultern (B)-links **0,081** (Video 0,000); a*>18 0,016 (Video 0,019) | **ROT** |
| **FO Spielbild h135** | >= 2,75 (Video 2,75) | ROT 1,49 / 0,070 | 0,291/0,425/0,279, **1,49**; Schultern **0,070**; a*>18 0,013 | **ROT** |
| KS Buehne 320 | 29,1-40,4 | 32,6 | nicht nachgestellt (Naeherung) | (Angabe Bauer) |
| KS h118 | 29,1-40,4; glatt <= 0,098 | 30,9 / 0,072 | Luma-Sobel 30,9, glatt 0,072. Fenster +-3 px 27,2-35,5 / 0,056-0,092. L*-Sobel 14,0 (Original 13,3-17,8). r3 26,9 / 0,126 | OK (an 3 von 9 Lagen unter 29,1) |
| **KS h135** | dito | ROT glatt 0,134 | Gradient 36,8 (34,8-39,0), **glatt 0,134** (0,097-0,136). L*-Sobel: 15,9, glatt 0,297 (Original 0,272-0,331). r3 27,2 / 0,197 | **ROT** nach Riegel (Luma; mit L* im Originalband, 4.6) |

**Zaehlung nach eigener Messung:**

- **Rot** sind die 11 Zeilen des Bauers (T3, T5, T5q, F6, F7 Wortlaut, TQ, TK, FO x 3, KS h135) und dazu das F-Mittel nach Wortlaut.
- **Knapp gruen** sind:
  - S3 (Rand 0,5 px);
  - T8 F17 (5,96 bei Grenze 6);
  - F8 Wortlaut er2 (dL +3,8 bei Grenze 4);
  - M4 Glied 20 (0,0905 bei Grenze 0,091);
  - K3-Hoehe (0,025 bei Grenze 0,023);
  - FS Oberarm (B)-links unten (dL +3,6 bei Grenze 4).

## 2. Tests und Gegenproben

**Bau**

- exe re15_port/build/platform/pc/re15_pc.exe, Stand 12:37:35. Sie ist juenger als schwanz_r41.c und birkin_formen.c (12:19:55).
- `ninja -n` meldet "no work to do". Ich habe keinen eigenen Bau gestartet (parallele Agenten, gleicher build/).

**ctest** -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --timeout 900:

- **4/4 Passed**, 64,9 s (SF_belege/v4/PZ4_ctest.txt).

**Reichweite der Tests:**

- Im Diff ee470c381..HEAD ist keine Testdatei geaendert.
- Die Engine hat sich in Runde 4 nur in Kommentaren geaendert.
- Der Hinweis aus Runde 3 bleibt: Kein Test deckt die V4-Regeln ab (Mitnahme, Freigabe h70-h74, Boden mit Mesh).
- integration_r41_sichel_form laedt das Spiel-Asset Ref2. Die Tests sagen also nichts ueber die Variante 00d642b7. Der Bauer nennt das
  in Offen 8.

**Gegenprobe V3 r2** (eigene Messung). Rot sind:

- Kopf vorn:
  - K1 (Kontrast 2,8), K3 (0,003), K4 (-0,034);
  - K5 (13,6), K6 (34,2), K7 (8,5), K8 (23,9 / Anteil 0,70);
  - K9 (16,6), K10 (0,124).
- Kopf seitlich: S1 (2 Komponenten), S2 (0,078 / 0,900), S3 (24,5), S4 (17,4).
- Schwanz: T2 22,9 Grad; T8 F15 8,9 / F16 9,0 / F17 6,5.
- Koerperfarben: F6 9,8; F8 11,1; F9 9,4; F14 19,1; Unterarm Id 14 13,98 dE00 (Anteil 0,121).
- FS Oberarm (B)-rechts unten Rot 0,58.

**Die Riegel trennen V3 r2 klar.**

**Gegenprobe V4 r3 bei den behobenen Befunden** (eigene Messung, dieselben Werkzeuge). Alle sind dort rot und in V4 r4 gruen:

| Befund | V4 r3 | V4 r4 |
|---|---|---|
| S3 Abstand | 0,019 H | 0,008 H |
| T2 Lage (O2) | -0,280 / +0,100 | -0,251 / +0,051 |
| T5q Abfall | 0,061 | 0,000 |
| FS Oberarm (B)-rechts unten Rot | 0,49 | 0,02 |
| TQ Stufe | 16,6-19,8 | 8,2-8,7 |
| KS h118 | 26,9 / glatt 0,126 | 30,9 / 0,072 |
| KS h135 Gradient | 27,2 (glatt 0,197) | 36,8 (glatt 0,134 weiter rot) |

**FO** wird besser, bleibt aber rot:

| | V4 r3 | V4 r4 |
|---|---|---|
| Buehne | 1,21 | 2,33 |
| h118 | 1,18 | 1,55 |
| h135 | 1,06 | 1,49 |
| Schultern h118 / h135 | 0,128 / 0,116 | 0,081 / 0,070 |

**TK** wird besser, bleibt aber rot: RMS 0,60 -> 1,77, Oberkante max 16 px -> 8 px.

**Rueckschritte gegenueber V4 r3** (dieselben Werkzeuge):

| Zeile | V4 r3 | V4 r4 | Bemerkung |
|---|---|---|---|
| T3 | gruen | rot | |
| T5 f 0,5 | | 0,095 | |
| F6 | 4,00 | 7,09 | |
| F-Mittel nach Wortlaut | 3,81 | 4,52 | |
| T8 F17 | 1,82 | 5,96 | |
| F8 ohne Erosion, dL | +3,5 | +4,4 | |
| Lauf-Form (Kabsch auf die Ruheform) | 0/47 Takte > 0,06 H | 11/47, max 0,071 H | 4.4 |
| Welt-Abfall h88-h97 | 0,091-0,10 H | 0,000-0,017 H | 4.4 |

## 3. Spiellauf (eigener Lauf sfv4/pz4/lauf, bitgleich zu sfv4/r/V4r4)

**Koerperfest (O3):**

- Ich zaehle 47 Gangtakte: h85 und alle Takte mit clip 1 von h90 bis h135, also weit ueber die geforderten 6.
- Kette gegen Ziel (Keyframe der gezeichneten Pose, Gelenke 1/3/5/7): max 0,048 H (h85 Ende), 0 Takte ueber 0,06 H.
- Biegung 61,2-70,5 Grad, Knick 10,6-16,2 Grad (Ruhe 67,5 / 15,8). Die Breite ist starre Geometrie.

**Rueckfaltung:** keine, Gelenkwinkel max 72,7 Grad im ganzen Lauf.

**Boden:**

- Ich habe alle Ecken der Meshes 16-22 geprueft (eigener MD1-Parser, exakte Schranke ohne Rollwinkel).
- Der tiefste Weltpunkt liegt bei -45 E. (h34), dann folgen h35 mit -46 und h68 mit -89. Kein Takt von 144 liegt darunter.

**Licht und CLUT:**

- Die neuen Zeilen 490 (Mesh 13) und 488 (Mesh 14) kommen im Spiel an: dE00 b2 gegen FRAMEDUMP 0,11-0,42 in h110-h135.
- h100 liegt in der Ueberblendung, dort 2,7-7,9 fuer alle Ids. Das gilt wie in Runde 3 und ist kein Farbfehler.

## 4. Befunde im Einzelnen

### 4.1 T5 / T3: der Teil an der Biegung ist zu schmal, der Knick sitzt 1 px zu weit vorn

Breite bei f 0,5 quer zur Mittellinie (Achse Knick -> Beginn):

| | Breite f 0,5 |
|---|---|
| V4 r4 | 0,095 H |
| Original, Masken A / hi / lo | 0,121 / 0,119 / 0,117 H |

- Das Soll ist 0,125 +-0,025, die Untergrenze also 0,100. V4 r4 liegt 0,005 H darunter.
- Die Ausbuchtung aus Runde 3 ist weg: Abfall 0,000, das Profil steigt monoton.
- Das Profil steigt aber spaet. In f 0,3-0,6 bleibt es bei 0,055-0,104, das Original bei 0,069-0,134.
- Ursache: die Durchmesser 0,139 / 0,130 / 0,079 / 0,046 H (Gelenke 1-4). Gelenk 3 an der Biegung hat 0,079; zwischen Gelenk 2
  (0,130) und Gelenk 3 faellt die Breite auf 61 %.
- Der Bauer belegt den Zielkonflikt: Ein dickeres Gelenk 3 hob die Oberkante (TK) und senkte die Drehung (Laeufe d3).

**Abhaengigkeit vom Messverfahren:**

- Mit der Achse ab der Saat (Oberkante des Ansatzes, Werkzeug pz2/pz3) misst V4 r4 0,036 / 0,081 / 0,117 / 0,146, Abfall 0,006.
  Das ist gruen.
- Der Messblatt-Wortlaut sagt "Knick -> Beginn", und Beginn = T1. Danach bleibt T5 rot.

T3: Die Taille liegt bei dx -0,234 H vom T1-Beginn (358,238), die Grenze ist -0,235. Absolut (TA) liegt der Knick nur +0,024 H
neben dem Nutzerpunkt (290,308). Rot ist T3, weil der Port-Beginn 1 px weiter hinten liegt als der Nutzer-Beginn (357,243).

### 4.2 Schwanzeichung: F16/F17 auskommentiert, F17 an der Grenze

Dossier V4 Runde 4, Zeile Sicht "Vierkant-Balken": "Eichregionen F15/F16/F17 nach Baendern aus dem Code-Render". Der Code
(eichen_v4.py Zeile 783, Commit 8ee38bb7d) sagt etwas anderes:

```
rt = [('F15', kreis15, RV.ZIEL['F15'], 1.0),     # (Lauf r4 Eichung n mit Gewicht 2: F17 kippte auf 4,9 L*, verworfen) ('F16', kreis16, RV.ZIEL['F16'], 1.0), ('F17', r17, RV.ZIEL['F17'], 2.0),
      ('F18', rmask['F18'], RV.ZIEL['F18'], 1.0)]
```

- Die Tupel F16 und F17 stehen hinter dem `#`. Geloest wird also nur mit F15, F18, den 7 Achspunkten (T9) und der Bindung der
  Aussenkante an ihre Startfarbe (rgw 3,0).
- Der Nachbau mit eichen_v4.py aus HEAD ergibt bitgleich die committete farben_v4.npz (sha256 971b6cc7). Der Stand ist also mit dieser
  Zeile entstanden.
- Folge:
  - T8 F17 (beleuchtete Oberseite) faellt von 1,82 (V4 r3) auf **5,96** (Grenze 6);
  - F17 ist zu dunkel (20,5 gegen 26,8 L*);
  - die Riegelzeile ist nur noch durch den Abstand von 0,04 gruen.
- Ob die Zeile Absicht ist (Kommentar "verworfen") oder ein Zeilenumbruch-Fehler, laesst sich aus dem Code nicht entscheiden. Der
  Kommentar zum Regularisierungsgewicht ("Aussenkante stark an ihre Startfarbe (F17-Ziel ...) gebunden") und das Dossier setzen F17
  als Eichregion voraus.
- Klarstellung noetig: entweder F16/F17 wieder in die Eichung, oder das Dossier berichtigen.

### 4.3 F-Mittel und F8: die gruene Zeile haengt an der Lesart

**F-Mittel nach dem Messblatt-Wortlaut** (F7/F8 = Id ganz, F14 = Id 8 mit L >= P70):

| Lesart | F-Mittel |
|---|---|
| ohne Erosion | **4,52** |
| 2 px Erosion | 3,99 |

- r3 lag ohne Erosion bei 3,81.
- Den Ausschlag gibt F6, das von 4,00 auf 7,09 steigt. Grund ist die Rot-Umverteilung in die Brustmitte (FO), wie im Dossier Offen 1
  begruendet.
- Die gruenen 2,97 des Bauers mitteln F7 und F8 an der Fensterlage.
- Wie F6/F7 braucht die Zeile den Messblatt-Entscheid "Fensterlage oder Id ganz". Bis dahin werte ich sie nach Wortlaut als rot.

**F8 nach Wortlaut:**

- Mit 2 px Erosion (Lesart beider Seiten seit Runde 1) ist dL +3,8, also gruen. Ohne Erosion ist dL +4,4, also rot.
- Der Wert ist auf die Grenze nachgestellt: r4_koerper_korrektur.json "F8 -3,84 -> -4,6 (Messblatt-Wortlaut F8 Id ganz mit -3,84 dL
  +4,1 > 4)".
- Das ist eine Nachstellung am eigenen Riegel und trifft die Zielfarbe nicht. Der Kern des Fensters ist gruen (Fensterlage 2,11).

### 4.4 Gang: koerperfest ruhig, in der Welt h85-h99 aufgerichtet

**Nach O3 (koerperfest) gruen.** Zwei Messungen zeigen aber gegenueber Runde 3 einen sichtbaren Rueckschritt im Bild:

**Welt-Lage** (y nach unten, in H):

| | V4 r3 h88-h97 | V4 r4 h88-h97 | Ruhe h118 |
|---|---|---|---|
| Biegung -> Ende | +0,091 bis +0,100 | **+0,000 bis +0,017** | +0,39 |
| Beginn -> Ende | -0,10 | **-0,19 bis -0,24** | |

- Bei Beginn -> Ende heisst ein negativer Wert: das Ende liegt ueber dem Beginn.
- Der Schwanz zeigt im Spielbild h85-h99 als Rohr schraeg nach oben ueber den Ruecken. Das Endstueck liegt waagerecht.
- Ursache ist die gezeichnete Pose, nicht die Kette: Der Rumpf ist in der Ueberblendung aus clip 11 vorgeneigt, und das Ziel selbst
  liegt so (Abweichung zum Ziel h95 0,003 H).
- Beleg: SF_belege/v4/PZ4_gang_h85_h88_h95_h99_h100_h118.png. Oben h85 / h88 / h95: das rosa Rohr links oben ist der Schwanz.
- Vorlage 2 (h85) zeigt den Schwanz haengend. Die Port-Pose bei h85 ist aber clip 11 mit anderer Gier (bekannt seit Runde 2).

**Form ohne Ziel-Zeile** (Kabsch auf die Ruheform von Pose 85):

- V4 r4: 11 von 47 Gangtakten ueber 0,06 H, max 0,071 H (h85, h90-h99).
- V4 r3: 0/47, max 0,040.
- Die Keyframes selbst weichen nur bis 0,045 H ab. Der Rest kommt aus der Seitenvorgabe der Engine (zm[j][2] -= seite x j^2/N^2)
  zusammen mit den neuen Gliedlaengen (624/440/815/558/497/268/249).
- Das ist kein Messblatt-Riegel. Es ist ein Punkt fuer die Sichtpruefung: T7a bis T7d bleiben gruen.

### 4.5 TQ Plateau: Riegel rot, das Verfahren ist rauschempfindlich

- Der Plateau-Anteil zaehlt Schritte mit |dL/dy| < 0,3 L* je Pixel. Das Original ist rohes Video mit MDEC-Rauschen, der Port ein
  rauschfreier Render nach Gauss 1,5.
- Mit Schwelle 1,0 L*/px liegt V4 r4 bei 0,58-0,79, das Original bei 0,63-0,79. Mit 0,5 sind es 0,50-0,68 gegen 0,43-0,61.
- Auch V4 r3 (Vierkant-Balken) laege bei 1,0 im Band (0,70-0,79). Das Plateau-Mass trennt die beiden Staende also nicht. Die Stufe
  trennt sie: 16,6-19,8 gegen 8,2-8,7.
- Ich werte nach dem Riegel als rot. Der Orchestrator sollte entscheiden, ob TQ Plateau als Riegel taugt.

### 4.6 KS h135 und S3: rot bzw. gruen jeweils an der Grenze des Verfahrens

**KS h135:**

- Mit dem Luma-Sobel des Riegels ist der Anteil glatter Pixel 0,134 (0,097-0,136 in 9 Lagen), also rot.
- Mit L*-Sobel liegt der Kopf im Originalband: glatt 0,297 gegen 0,272-0,331, Gradient 15,9 gegen 13,3-17,8. Die Luma-Schwelle 8
  bewertet dunkle Flaechen strenger.
- KS h118 ist an 3 von 9 Fensterlagen unter 29,1 (bis 27,2). KS h135 ist an 8 von 9 Lagen glatter als 0,098.

**S3:** Ich messe 0,008 H, der Bauer 0,004 H. Die Blob-Schwelle bewegt den Fleckrand um 1 px. Bei 0,01 H Grenze bleibt ein halber
Pixel Rand. Der Bauer belegt selbst, dass S3 bei vorbau 0,050/0,053 zwischen 0,004 und 0,011 schwankte.

## 5. Konstanten im Diff (O6)

Der Auftrag nennt den Diff ee470c381..HEAD. Die Stellen der Runden 2/3 sind in SF_PRUEFUNG_V4_r3_zahlen.md Abschnitt 5 geprueft. Neu
in Runde 4 ist der Diff 349f4360c..HEAD. Die Engine hat sich dort nur in Kommentaren geaendert.

| Stelle | Wert | Quelle laut Kommentar | Urteil |
|---|---|---|---|
| schwanz_r41.c SW_V4_ERHALT / FEDER / MITNAHME, haengen() aus fuer V4; birkin_formen.c k_schwanz_gewicht_v4 | unveraendert | jetzt mit Nutzer-Wortlaut "Ich will EXAKT diese Umsetzung" und Entscheid O3 | r3-Beanstandung behoben |
| eichen_v4 STRUKTUR_FARBE_KLAMMER | 0,4 | gemessen: p99 der Original-Farbtonabweichung x 1,5 (vorn 0,30, seitlich 0,15, max 0,95) | behoben |
| eichen_v4 STRUKTUR_KLAMMER / STRUKTUR_ZONE_R3 | 2,0 / 0,0 | Kommentare jetzt "Faktor >= 0,135" / "keine helle Struktur" | behoben (Kommentar = Wert) |
| eichen_v4 AUGE_RADIAL_VORN, GAIN_FARBE, AUGE_HOF_1/2, HOF_DUNKEL | unveraendert | jetzt mit Nutzer-Wortlaut | behoben |
| birkin_formen.py SCHWANZ_V4 gelenke 2-7, durchmesser 1-4, tiefe 2-3 | (-0,1462,-0,0115) ... / 0,1391/0,1301/0,0789/0,0459 / 0,1463/0,0696 | r4_form_suche Saat 1 Schritt 253 und Saat 22 Schritt 35; Protokolle SF_belege/v4/r4/r4_form_suche_saat1.log, _ende22.log vorhanden; Durchmesser monoton nach Messblatt Abschn. 4 Punkt 3; tiefe PORT-WAHL mit Wortlaut | belegt; gegen Messblatt-Breite 0,125 bei f 0,5 zu schmal (4.1) |
| SCHWANZ_V4 seiten 12 | 12 | Sicht-Pruefung r3 1.3 Empfehlung; M1 192 <= 212 | belegt |
| SCHWANZ_V4 uv verlauf zeilen_je_c | 22/24/9/22 | Laeufe r4 Dichte 1/2/4, M4; "**PORT-WAHL (Machart, Messblatt M4)**" | **PORT-WAHL ohne Nutzer-Wortlaut** |
| KOPF_V4 vorbau vor | 0,048 | Laeufe r4 Eichung f-o; "**PORT-WAHL, Quelle der Masse Messblatt S2/S3**" | **PORT-WAHL ohne Nutzer-Wortlaut**; ausserdem nennt der erste Satz "vor 0,06 -> 0,053 H", der Wert ist 0,048 (Kommentar widerspricht sich) |
| KOERPER_ZEILEN_V4 13 -> 490, 2/3/5/6 -> 491 | | Sicht 1.5, Laeufe r4 Koerper 2/6, O1 | belegt; im Spiel wirksam (Licht 3) |
| eichen_v4 UEBERGANG | 0,3 | PORT-WAHL mit Nutzer-Wortlaut | OK |
| eichen_v4 HOF_DUNKELSTRUKTUR / STRUKTUR_HELL | 1,0 / 0,10 | Wortlaut / Lauf r4 Eichung k | OK |
| eichen_v4 rgw (Aussenkante) | 3,0 | Laeufe r4 Eichung r-v; "**PORT-WAHL (Regularisierung der Eichung, keine Spielkonstante)**" | **PORT-WAHL ohne Nutzer-Wortlaut** |
| eichen_v4 rt (Eichregionen Schwanz) | F15, F18 (F16/F17 im Kommentar) | Dossier sagt F15/F16/F17 | **Code widerspricht dem Dossier** (4.2) |
| r2_koerper_farben MU | 0,25 | "**PORT-WAHL**" | **ohne Nutzer-Wortlaut** |
| r2_koerper_farben KACHEL_MIN | 30 | "**PORT-WAHL**" | **ohne Nutzer-Wortlaut** |
| r2_koerper_farben KACHELN / HF_OBERKOERPER | (6,4) / 0,5 | PORT-WAHL mit Nutzer-Wortlaut; Laeufe r4 Koerper 6-8 | OK |
| r4_koerper_korrektur.json F8 / beine | -4,6 L* / Saettigung x 0,4 | Lauf koerper_eichen4, dann auf den Riegel "F8 Wortlaut" nachgestellt | Quelle genannt; **am eigenen Riegel auf die Grenze gezogen** (4.3) |

**Zusammenfassung O6:**

- Die Beanstandungen aus Runde 3 sind behoben.
- **5 neue PORT-WAHL-Stellen nennen keinen Nutzer-Wortlaut:** zeilen_je_c, vorbau 0,048, rgw 3,0, MU 0,25, KACHEL_MIN 30.
- Ein Kommentar widerspricht seinem Wert: vorbau.
- Bei rt widerspricht der Code dem Dossier.
- Die Laufprotokolle "r4 Eichung a-v" liegen nur im Scratchpad (sfv4/r4/eich_*), nicht im Baum. Die Form-Suche und die Koerper-Eichung
  sind dagegen unter SF_belege/v4/r4/ abgelegt.

## 6. Offen fuer Bauer und Orchestrator

1. **Messblatt-Entscheid F6/F7/F-Mittel:** "Fensterlage" oder "Id ganz". Nach Wortlaut sind F6 (7,09), F7 (7,82) und das F-Mittel
   (4,52) rot. Der Bauer belegt den Widerspruch mit dem Rechenweg in Dossier Offen 1/2.
2. **Schwanz:** Die Breite an der Biegung fehlt (f 0,5 0,095 gegen 0,117-0,121 im Original). T3 liegt 0,001 H daneben. TK ist rot:
   RMS 1,77 gegen >= 3,02, Oberkante x 280 8 px.
3. **Eichung F16/F17 (4.2):** klaeren und F17 vom Grenzwert 5,96 wegholen (r3 1,82).
4. **FO:** Rot Mitte/aussen 1,49-2,33 gegen >= 2,75. Die (B)-linke Schulter im Spielbild ist zu rot (a* > 12 0,07-0,08, Video 0,00).
5. **KS h135 glatt 0,134** sowie die Riegel-Tauglichkeit von TQ Plateau und KS-Luma (4.5, 4.6): Entscheid des Orchestrators.
6. **O6:** 5 PORT-WAHL mit Nutzer-Wortlaut versehen; den vorbau-Kommentar berichtigen; die Eichprotokolle in den Baum legen.
7. **Hinweise fuer die Sichtpruefung:**
   - Schwanz im Spielbild h85-h99 schraeg nach oben ueber den Ruecken (4.4, Bild PZ4_gang_*).
   - S3, F8, T8 F17 und M4 sind jeweils knapp an der Grenze.
8. **Tests:** Ein Test der V4-Regeln fehlt weiter. Die Integration prueft Ref2, nicht die Variante.

**Belege** (absolute Pfade):

- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_mess_V4r4_V4r3_V3r2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_t2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_tq_tk_ks_fo.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_lauf_V4r4.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_ausgaben.txt (T5, Farben, FS, G1, M4,
  T8, T9, Lauf r4/r3, Licht)
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_ctest.txt
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_eichung_nachbau.log
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ4_gang_h85_h88_h95_h99_h100_h118.png
- Scratchpad: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz4/
  (Neubau emd/, eigener Lauf lauf/, Renders r_*.npz/png, Spielbilder spiel_V4r4_h118/h135.png, takt_eigen.txt/takt_bauer.txt,
  farben_neu.npz)
