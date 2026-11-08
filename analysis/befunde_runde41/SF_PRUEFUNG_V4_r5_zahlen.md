# SF V4 Runde 5: Zahlen-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Bau V4 Runde 5 der Sichel-Form, Commits 0936f2e84, fb17942e5, 8a83adce6, 496613e2c, a0638b2dc, 13830bca0, 54a1f60e8 und
7320d8a1f auf r41/sichel_nb2 (Basis cbc917a48 = V4 Runde 4). Modell EM30_V4.EMD = EM30_V4r5.EMD, sha256 a762bd66. Gemessen wird gegen
SF_MESSBLATT_V4.md mit dem Nachtrag Runde 5 (O7), die Riegelzeilen der Sicht-Pruefungen r3/r4 (TQ, TK, FO, KS) und die Entscheide O1-O11.
Kein RE-Befund, keine @0x-Konstante: die Sichel-Form gibt es auf der Disc nicht, alles ist Bildmessung am 80%-Video 4.

## Ergebnis: nicht bestanden

**Die 8 roten Zeilen des Bauers bestaetige ich mit eigenem Code, keine davon ist ein Messartefakt allein:**

| Zeile | Bauer | Pruefer (eigener Code) |
|---|---|---|
| T5 (Bauer-Mittellinie) | f 0,5 0,098 H | eigene Mittellinie nach Wortlaut (ab T1) gruen 0,030/0,049/0,110/0,161; ab der Saat Abfall 0,025 rot (4.1). Die Zeile haengt am Verfahren |
| TQ Plateau | 0,21-0,41 | 0,40 / 0,41 / 0,38 / 0,21 (Original <= 0,31) |
| TN Quernaht | 10,7 bei x 297 | 10,7 bei x 297 (Original 9,5). Auch symmetrisch rot: beide weich 10,7 gegen 8,1, beide roh 13,5 gegen 9,5 |
| TR Rotanteil waagerecht | 0,020 | 0,020 (Original roh 0,099/0,110/0,097). Auch beide weich rot: 0,020 gegen 0,073-0,083 (Abstand 0,057 > 0,05); nur beide roh gruen (0,062 gegen 0,102) |
| AF Pose 85 | Id 6 0,012 | Id 6 0,012 gegen die Quellregion Unterschenkel (Box 356-392 x 352-430), p99 10,3 dE76 |
| FO Spielbild h118 | 2,73 | 2,73 (Drittel 0,218/0,483/0,136; Video 2,91) |
| FR Spielbild h85 | 165 | 165 im Videoraum, 261 roh (Video 2, Grenze 41) |
| KS h135 glatt | 0,120 | 0,120 (9 Lagen 0,089-0,122; Grenze 0,098) |

**Nach eigener Messung kommen dazu:**

- **O11 nicht erfuellt** (4.2): T5 ist an der Bauer-Riegelzeile rot und war in Runde 3 gruen (0,028/0,080/0,140/0,131). T3 ist zwar gruen, liegt
  aber naeher an der Grenze als in Runde 3: dx -0,241 (Bauer -0,239) gegen -0,248 in Runde 3, Rand 0,006 H statt 0,013 H.
- **O6, Farbdateien nicht nachbaubar** (5.2): farben_schwanz_v4.npz und farben_koerper_v4.npz entstehen aus HEAD nicht bitgleich (4 Versuche
  mit verschiedenen Eingaben). Der Aufruf, mit dem die committeten Dateien entstanden, ist nirgends festgehalten. Die Variante mit
  nachgebautem Schwanz misst TN 11,4 / TR 0,033 / TQ 0,26-0,33 statt 10,7 / 0,020 / 0,21-0,41.
- **TV nur im Querschnittsmittel gruen** (4.3): Mittelt man den Querschnitt, ist TV an allen 5 Stationen gruen. Auf der Unterseite des
  waagerechten Teils (Lage des Nutzerpunkts Beginn) fehlt dagegen das Rot: Mitte a* 3,3 gegen 7,8, senkrecht L* 14,6 gegen 7,3. Das ist
  derselbe Befund wie TR.

**Bestaetigt (gruen) sind:**

- G1 nach O1, Spiel-Asset und Pin, M1-M4
- K1-K11, S1-S4
- T1, T2 (Lage und Drehung), T3, T4, T6, TA, T8 und T5q
- T7a-d, Boden
- F0, F6 und F7 an der Fensterlage (O7), F8-F19, F-Mittel
- alle 6 FS-Boxen
- TK, AF Pose 135, FO Buehne, FO h135, FR h100/h118/h135, KS Buehne (Angabe Bauer) und KS h118
- die 4 Tests, der Spiellauf (bitgleich nachgefahren) und die Gegenproben V4 r4 und V3 r2

**Diese Befunde aus Runde 4 sind behoben:**

- S3, gelber Fleck: 0 gelbe Texel in allen 23 Meshes, V4 r4 43 in Mesh 13 / CLUT 490
- S1, Bloecke: TN von 16,7 auf 10,7, TQ von 0,41-0,53 auf 0,21-0,41
- TK: RMS 3,69 px
- FO Buehne und FO h135
- FR h118
- T3, T5q
- F16/F17 klargestellt

**Nicht behoben (rot) sind:**

- T5 nach der Bauer-Mittellinie, TQ, TN, TR, AF 85
- FO h118, FR h85, KS h135

## 0. Verfahren (eigener Code, nicht riegel_v4.py / r5_mass.py)

Die Werkzeuge liegen in C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4/ und sind nicht committet:

- pz5_*.py sind Kopien von pz4_* (Runde 4), umgestellt auf sfv4/pz5. Die Vergleichsstaende sind V4 r5 (eigener Neubau), V4 r4, V3 r2 und Ref2.
- Neu geschrieben sind pz5_neu.py (TN, TV, TR, AF, Texel-Gelb, FR), pz5_af.py, pz5_af2.py, pz5_af3.py, pz5_tv2.py, pz5_fr_lauf.py,
  pz5_r3vgl.py und pz5_nb_vgl.py.

Die Ausgaben stehen unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz5/.
Die Belege stehen unter C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_*.

| Werkzeug | misst |
|---|---|
| birkin_formen.py --variante v4 --aus sfv4/pz5/emd | Neubau aus HEAD 7320d8a1f: **sha256 a762bd66eab76f38...** = Bauer-Meldung = sfv4/emd/EM30_V4.EMD = EM30_V4r5.EMD = r/V4r5/cd/EMD |
| lauf_v4.sh (eigener Lauf sfv4/pz5/lauf, exe build/ 12:37:35, ninja -n "no work to do") | 672 hangar-takt-Zeilen bitgleich zu sfv4/r/V4r5, 191 von 191 FRAMEDUMPs bitgleich |
| pz5_render.py | b2.zeichne (Messrender, Messblatt Abschn. 7) fuer V4 r5, V4 r4 (00d642b7), V3 r2 (1952713c), Ref2; Posen "85"/"135" |
| pz5_mess.py, pz5_schwanz.py, pz5_t2.py, pz5_t5.py, pz5_t5orig.py, pz5_r3vgl.py | K1-K10, S1-S4, T1-T6, TA, T8 (Fenster nach Wortlaut), F6-F19, F0; T2 nach t2_mass_v4 (O2); T5 auf drei Achsen; V4 r3 (dfbc30bc) fuer O11 |
| pz5_g1.py, pz5_g1geo.py, pz5_m4.py | eigener EMD/MD1/EMR/TIM-Parser: Geometrie, CLUT, Keyframes, Dreiecke, Texeldichte |
| pz5_farben_alt.py, pz5_fs.py | F6/F7/F8/F14 in allen Lesarten, F-Mittel, Sicht-Boxen |
| pz5_form.py | TQ/TK, KS (Luma und L*, 9 Lagen), FO-Drittel an Buehne und eigenen Spielbildern h118/h135 |
| pz5_neu.py | TN (Spaltenmitte nach Wortlaut, Bandmittel, entlang der Achse), TR (drei Lesarten), TV (Stationen auf dem Polygonzug), Texel-Gelb je Mesh ueber UV-Polygone und CLUT, FR im eigenen Lauf |
| pz5_af.py / pz5_af2.py / pz5_af3.py | AF: nur Umkreis; Umkreis innerhalb der Port-Silhouette; Beine gegen die Unterschenkel-Box; Kopf gegen die Kopfbox |
| pz5_tv2.py | TV als Querschnittsmittel (Spalten bzw. Zeilen +-3 px), Original roh und weich |
| pz5_fr_lauf.py | FR an den Laeufen V4r5, V4r4 und V3r2 |
| pz5_lauf.py, pz5_licht.py | Lauf: Kette gegen das Ziel, Form gegen die Ruheform, Boden mit allen Ecken der Meshes 16-22; Licht b2 gegen FRAMEDUMP |
| pz5_nb_vgl.py | gemeldete Variante gegen die Variante mit nachgebauter Schwanztextur |

**Selbstpruefung:**

- V4 r4 reproduziert mit pz5_* die Werte der Runde-4-Pruefung:
  - T3 -0,234;
  - T5 f 0,5 0,095;
  - TK 1,77;
  - TQ 0,41-0,53;
  - FO Buehne 2,33;
  - TN 16,7;
  - TR 0,189;
  - Texel-Gelb Mesh 13 43, im Bild 47 px (Sicht r4: 45).
- Die Originalwerte der neuen Riegel stimmen mit dem Bauer ueberein:
  - TN 9,5 an allen drei Masken;
  - TR 0,099 / 0,110 / 0,097;
  - TK 3,87 / 4,71 / 4,69;
  - FR Video h85/h100/h118/h135 2 / 107 / 0 / 156.

## 1. Riegel-Tabelle Soll / Bauer / Pruefer

Massstab:

- H vorn (Pose 135) = 364 px.
- H seitlich (Pose 85) = 264 px.
- H_E im Lauf = 3775 E.

| Riegel | Soll | Bauer | Pruefer (eigener Code) | Bewertung |
|---|---|---|---|---|
| G1 (O1) | Geometrie 0-8/10-15 gleich, Skelett/KF/EDD gleich, CLUT frei | OK | Geometrie-Abweichungen **keine** (Ecken, Normalen, Flaechen, UV ohne CLUT-Feld). CLUT-Feld 2/3/5/6 -> 491, 10 -> 489, 13 -> 490, 14 -> 488. Byte-gleich 0, 1, 4, 7, 8, 11, 12, 15. EMR rel 0-15 und Tabelle gleich. 912 KF x 116 Byte, Bytes 0-83 gleich. Sektionen 0, 1, 3-6 gleich | OK |
| Spiel-Asset / Pin | unveraendert | unveraendert | EM30_HANGAR.EMD sha256 cfd1ac42... = release/em30_hangar.sha256. git diff 84e8a4d97..HEAD (shared_assets, release) leer. Engine, Tests, Version_80%/, pri/, synchro/ seit cbc917a48 unveraendert | OK |
| M1 / M2 / M3 / M4 | <= 212 / <= 118 / 8 bpp / 0,045-0,091 | 198 / 109 / ok / 0,060-0,072 | 198 (33/22/44/22/22/22/33) / 109 / TIM-Flag 0x9, 12 CLUT-Zeilen, Kopf 486, Schwanz 487 / Kopf 0,0570, Schwanz 0,0603-0,0723 | OK |
| K1 | 1 Komponente, Kontrast >= 20 | 1; 25,3 | 1 (58 px); 25,3 | OK |
| K2 | dE00 <= 6; L* >= KV5 + 20 | 1,6 | 47,9/8,6/-1,5, dE00 1,70; 47,9 >= 22,4 + 20 | OK |
| K3 | 0,016-0,036 x 0,023-0,043 | 0,025 x 0,025 | 9 x 9 px = 0,025 x 0,025 | OK (Rand Hoehe 0,002) |
| K4 | 0,035-0,065; 0,07-0,12 | 0,056 / 0,101 | 0,056 / 0,101 | OK |
| K5 | dE00 <= 6; L* <= Auge - 25 | 0,9 | 13,9/8,1/1,0, dE00 0,89; 13,9 <= 22,9 | OK |
| K6 / K7 | L* <= 18 / dE00 <= 6 | 12,3 / 1,2 | 12,3 / 18,1/7,0/-0,1 dE00 1,20 | OK |
| K8 | dE00 <= 6; Anteil L* > 35 <= 0,05 | 2,1 / 0 | 10,9/7,9/4,1, dE00 2,05; Anteil 0,000 (L* max 19,0) | OK |
| K9 / K10 | dE00 <= 6, a* >= 11 / 0,14-0,18 x 0,15-0,20 | 2,2 / 0,154 x 0,187 | 20,9/13,0/5,4 dE00 2,24 / 0,154 x 0,187 | OK |
| K11 (O4) | 41,0 +-6,1; Kopf >= Rumpf | 37,8 / 27,1 | Kopf 37,8, Rumpf 27,1 (Original 41,0 / 26,2) | OK |
| S1 / S2 | 1 Fleck, dE00 <= 6, Groesse / Lage | OK | 48,5/3,1/0,9 dE00 1,20; 0,042 x 0,049; 1 Komponente / 0,054 hinter, 0,809 ueber | OK |
| S3 | L* <= 22, dE00 <= 6, Abstand <= 0,01 H | 0,004 | 17,2/2,8/0,6 dE00 2,53; Abstand **0,008 H** | OK (Rand 0,5 px, wie r4) |
| S4 | dE00 <= 6, a* >= 11 | 3,1 | 27,5/11,4/3,2, dE00 3,06 | OK (a* r4 15,6 -> 11,4, Rand 0,4) |
| T1 | 0,15 hinter, 0,767 ueber (+-0,03) | 0,172 / 0,742 | Id 16 an Id 7/8 = (358,238): 0,172 / 0,742 | OK |
| T2 Lage / Drehung (O2) | -0,245 / +0,036 (+-0,03); 68,4 +-5,0 | -0,259 / +0,049; 64,0 | t2_mass_v4-Fenstermitte (289,7 / 251,0) vom T1-Beginn: **-0,259 / +0,049**; Drehung **64,0** (Original 70,1 / 65,8 / 69,5). r4 66,6, r3 70,6 | OK (Drehung Rand 0,6 Grad) |
| T3 | dx -0,265 dy +0,257 (+-0,03) | -0,239 / +0,247 | Taille (294,4 / 306,0): **-0,241** / +0,258. r3 -0,248 / +0,261 | OK (Rand 0,006 H; O11 4.2) |
| T4 | dx -0,245 dy +0,397; 0,37 ueber Sohle | -0,218 / 0,388; 0,354 | (299,1 / 340,1): -0,223 / +0,387; 0,356 ueber Sohle | OK |
| **T5** (Bauer-Mittellinie) | 0,03/0,065/0,125/0,14; Abfall <= 0,01 | **ROT** 0,032/0,049/**0,098**/0,163 | eigene Mittellinie ab T1 (Wortlaut "Knick -> Beginn"): 0,030/0,049/0,110/0,161, Abfall 0,002, gruen (Rand f 0,3 und f 0,7 je 0,004). Ab der Saat 0,032/0,062/0,153/0,144, **Abfall 0,025 rot** | **ROT** (Bauer-Riegel; 4.1) |
| T5q | wie T5, quer | OK 0,032/0,047/0,110/0,161 | quer zur t2_mass_v4-Mittellinie: 0,030/0,049/0,110/0,161 (Profil f 0..1 je 0,1: 0,021/0,030/0,036/0,049/0,062/0,110/0,133/0,161/0,157/0,142/0,144), Abfall 0,002. Original 3 Masken f 0,5 0,121/0,119/0,117 | OK |
| T6 | B 0,025-0,05, L 0,12-0,17, 75-95 Grad, Taille schmalste | OK | Median 0,034, L 0,130, 82,1 Grad nach vorn, Taille 0,021 < 0,025 davor | OK |
| TA | Nutzerpunkte +-0,03 | <= 0,020 | Beginn +0,004 / -0,019, Knick +0,017 / -0,008, Ende +0,016 / -0,011 | OK |
| T8 (Punkte nach Wortlaut) | dE00 <= 6; Wurzel a* >= 7, Ende a* <= 6 | 1,3/3,8/5,9/0,7 | F15 9,2/10,0/2,1 dE00 1,29; F16 13,4/8,0/3,5 dE00 3,20; **F17 22,3/3,1/1,3 dE00 5,70**; F18 16,0/2,9/1,1 dE00 0,78 | OK (F17 Rand 0,30) |
| T9 (Bauer-Riegel) | Mittel dE00 <= 6 | 4,9 | nicht nachgestellt. Unabhaengige Abtastung an 7 festen Original-Achsstellen (5 x 5): V4 r5 7,66, V4 r4 8,52; L* 13,6 gegen 19,1 | (Hinweis wie r4) |
| T7a-d (O3) | koerperfest | 0/47, max 0,056 | Kette gegen Ziel: 0/47, max **0,056 H** (h85 Ende); Biegung 69,5-76,8, Knick 11,2-16,2 Grad; max Gelenkwinkel 79,4 | OK (Rand 0,004 H) |
| Boden (O3) | kein Glied-Mesh unter dem Boden | -6 E. | alle Ecken der Meshes 16-22: tiefster Punkt **-19 E. (h35)**, dann h33 -25, h68 -27; 0 von 144 Takten darunter | OK |
| F0 Licht / Wand | <= 3 / <= 2 | 0,0 / 0,2 / 0,52 | Rumpf h118 0,05, h135 0,22; je Id h110-h135 0,03-0,93; Wand 0,52 | OK |
| F6 Fensterlage (O7) | dE00 <= 5, Betrag dL <= 4 | 3,4 (1522 px) | Id-8-Bildpunkte R-G >= 15 im Fenster 372-424 x 197-232: 18,8/13,1/5,8, **dE00 3,41**, dL +2,2 (1551 px; das Fenster liegt zu 96 % auf Id 8) | OK |
| F7 Fensterlage (O7) | dito, mit FS Oberarm (B)-links unten | 1,1 | obere 58 % von Id 13 (er2): 32,8/4,7/2,1, **dE00 1,07**, dL -0,5; FS-Box 14,6/2,4/0,9 gegen 11,3/2,1/0,9, dE00 2,15, dL +3,3, Rot 0,00 | OK |
| F6/F7 Id ganz | INFO nach O7 | 4,2 / 7,9 | 19,6/12,0/4,8 dE00 4,40 / 23,6/3,3/1,5 dE00 7,81 dL -9,7 | INFO (O7) |
| F8 Wortlaut (Id 10 ganz) | dE00 <= 5, Betrag dL <= 4 | 4,0 (er2) | ohne Erosion 19,7/4,6/0,7, dE00 4,50, dL +3,3; er2 3,97 / +2,7 (r4 ohne Erosion dL +4,4 rot) | OK |
| F9 / F10 / F19 | dE00 <= 5 | 1,9 / 3,4 / 1,6 | 1,72 / 3,53 (Id 12; Id 15 0 px) / 1,47 | OK |
| F14 | 60,0/11,0/1,2, Betrag dL <= 4 | 2,9 (er2) | er2 57,4/9,2/1,6 dE00 2,86 dL -2,6; ohne Erosion 54,1 dE00 5,64 dL -5,9 | OK (er2 wie seit Runde 1) |
| F-Mittel | <= 4 | 2,42 | Bauer-Lesart (F8 obere 56 %) 2,42 nachgerechnet; O7-Lesart mit F8 Id ganz: er2 **2,57**, ohne Erosion **3,05** | OK |
| FS (6 Boxen) | dE00 <= 5, Betrag dL <= 4, Rotanteil +-0,15 | OK | Unterarm Id 14 2,46 / +2,3 / L* > 30 0,000; Oberarm (B)-rechts unten 1,85 / +1,8 / Rot 0,00 (Original 0,04); (B)-links unten 2,15 / +3,3; Rumpf + Schultern 2,18 / +2,6 / Rot 0,31 (Original 0,32); Unterarm Id 11 1,84 / -1,9; Unterschenkel 3,24 / +3,4 / Rot 0,04 | OK (Rand dL Unterschenkel/Oberarm 0,6-0,7) |
| **TQ** | Stufe <= 9,6; Plateau <= 0,31 | ROT 0,21-0,41 | Stufe 7,7/5,9/5,0/6,1; **Plateau 0,40/0,41/0,38/0,21**. Gegenform Schwelle 1,0 L*/px: 0,60-0,77 (Original 0,63-0,79) | **ROT** (Plateau-Verfahren rauschempfindlich, wie r4 4.5) |
| TK | RMS >= 3,02; Oberkante <= 0,03 H | OK 3,69 / 0,027 | RMS **3,69** (Sehnenabstand 12,0 px); Oberkante 249/243/238/233/228 gegen 251/250/243/236/234, max 7 px = 0,0265 H | OK |
| **TN** (neu, O8) | Quernaht <= 9,5 dE76 | ROT 10,7 bei x 297 | Spaltenmitte nach Wortlaut **10,7 bei x 297** (Original 9,5 / 9,5 / 9,5). Symmetrisch: Original weich 8,1, Port roh 13,5. Bandmittel je Spalte 3,9 gegen 4,9-6,5 (gruen). Entlang der Achse 22,5 an der Taille (295,303) | **ROT** |
| TV (neu, O8) | 5 Stationen +-4 / +-3 | OK | Querschnittsmittel (pz5_tv2) alle 5 gruen: Wurzel 17,0/6,9 gegen 18,7/7,7; Mitte 13,3/4,8 gegen 13,8/6,6; Biegung 22,2/6,0 gegen 19,1/6,5; senkrecht 12,9/3,6 gegen 12,9/3,0; Endstueck 12,2/4,6 gegen 12,4/4,0. Punktstationen auf dem Polygonzug (Unterseite): **Mitte a* 3,3 gegen 7,8 (-4,5), senkrecht L* 14,6 gegen 7,3 (+7,3)** | OK nach Riegel (Hinweis 4.3) |
| **TR** (neu, O8) | a* > 12 waagerecht 0,102 +-0,05 | ROT 0,020 | Wortlaut (Port weich / Original roh) **0,020** gegen 0,102; beide weich 0,020 gegen 0,077 (ROT); beide roh 0,062 gegen 0,102 (gruen) | **ROT** |
| AF Pose 135 (neu, O9) | je Mesh <= 0,01; gelb gezaehlt | OK max 0,009 | Beine gegen die Quellbox: 0,006/0,004/0,001/0,000 (Id 2/3/5/6); Kopf gegen die Kopfbox 0,000; gelb 0 px; **Texel-Gelb 0 in allen Meshes 0-22** (V4 r4 Mesh 13 43 Texel 56,6/-3,3/40,2 in CLUT 490) | OK |
| **AF Pose 85** (neu, O9) | dito | ROT Id 6 0,012 | **Id 6 0,012** (p99 10,3 dE76), Id 2/3/5 0,002-0,003; Kopf 0,000; gelb 0 px (V4 r4 11 px in Id 13) | **ROT** (Rand 0,002) |
| FO Buehne | >= 2,75; Schultern <= 0,05; a* > 18 <= 0,041 | OK 6,48 | Drittel 0,095/0,582/0,084, **6,48**; Schultern 0,005; a* > 18 0,025 | OK (Hinweis 4.4: aussen halb so rot wie im Original 0,18/0,24) |
| **FO Spielbild h118** | >= 2,75 (Video 2,91); Schultern <= 0,05; a* > 18 <= 0,019 | ROT 2,73 | 0,218/0,483/0,136, **2,73**; Schultern 0,016; a* > 18 0,016 | **ROT** (0,02) |
| FO Spielbild h135 | >= 2,75 (Video 2,75) | OK 2,76 | 0,172/0,429/0,139, **2,76**; Schultern 0,020; a* > 18 0,020 (Video 0,041) | OK (Rand 0,01) |
| **FR h85** (neu, O10) | a* > 22 <= max(10 x Video, 1 Promille) = 41 | ROT 165 | **165** (roh 261; Video 2) | **ROT** |
| FR h100 / h118 / h135 | Grenze 1070 / 20 / 1560 | 0 / 0 / 0 | 0 / 0 / 0 (roh 64 / 16 / 20; Video 107 / 0 / 156) | OK (Hinweis 4.4) |
| KS Buehne 320 | 29,1-40,4 | 32,6 | nicht nachgestellt | (Angabe Bauer) |
| KS h118 | 29,1-40,4; glatt <= 0,098 | 31,0 / 0,048 | Luma 31,0 (30,7-34,7), glatt 0,048 (0,039-0,071); L* 14,1 / 0,206 | OK |
| **KS h135** | dito | ROT 0,120 | Luma 37,5 (35,4-39,4), **glatt 0,120** (0,089-0,122); L*-Sobel glatt 0,287 im Originalband 0,272-0,331 | **ROT** nach Riegel (wie r4 4.6) |

**Zaehlung nach eigener Messung:**

- **Rot** sind die 8 Zeilen des Bauers: T5 (Bauer-Riegel), TQ, TN, TR, AF 85, FO h118, FR h85, KS h135.
- Dazu kommen die Befunde **O11** (4.2) und **O6 Nachbau** (5.2).
- **Knapp gruen** sind:
  - T2 Drehung (64,0 bei Grenze 63,4);
  - T3 (Rand 0,006 H);
  - T7a (0,056 bei Grenze 0,06);
  - T8 F17 (5,70 bei Grenze 6);
  - FO h135 (2,76 bei Grenze 2,75);
  - S3 (0,008 bei Grenze 0,01);
  - S4 a* (11,4 bei Grenze 11);
  - K3-Hoehe;
  - T5 nach Wortlaut f 0,3 / f 0,7 (je 0,004).

## 2. Tests und Gegenproben

**Bau und Tests:**

- Die exe re15_port/build/platform/pc/re15_pc.exe hat den Stand 12:37:35. `ninja -n` meldet "no work to do", und die Engine ist seit cbc917a48 unveraendert.
- Einen eigenen Bau habe ich nicht gestartet (parallele Agenten im selben build/).
- ctest -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form" --timeout 900 lief **4/4 Passed** in 64,6 s
  (SF_belege/v4/PZ5_ctest.txt).
- Wie in r4 gilt: kein Test deckt die V4-Regeln ab, und integration_r41_sichel_form laedt das Spiel-Asset Ref2, nicht die Variante a762bd66.

**Gegenprobe V4 r4** (S1-S4 der Sicht r4; eigene Messung, dieselben Werkzeuge). Alle neuen Riegel faerben V4 r4 rot:

| Befund r4 | Riegel | V4 r4 | V4 r5 |
|---|---|---|---|
| S1 Bloecke, Quernaehte | TN | 16,7 | 10,7 |
| | TQ Plateau | 0,41-0,53 | 0,21-0,41 |
| | TK | 1,77 | 3,69 |
| S2 Schwanzfarbe | TR | 0,189 (roh 0,203) | 0,020 |
| | TV Querschnitt | Mitte 22,4/2,8 (+8,6 L*), Biegung 7,7 L* (-11,4) | gruen |
| S3 Gelb | Texel-Gelb Mesh 13 | 43 | 0 |
| | Bild-Gelb | 47 px vorn / 11 px seitlich | 0 / 0 |
| | AF Beine 135 / 85 | 0,079 / 0,108 | 0,000 / 0,012 |
| S4 Oberkoerper | FR h85 | 1170 | 165 |
| | FR h118 | 155 | 0 |
| | FO Buehne | 2,33 | 6,48 |

**Gegenprobe V3 r2** (eigene Messung). Die Riegel trennen V3 r2 klar:

- K1 2,8, K5 13,6, K8 23,9, K9 16,6
- S1 2 Komponenten, S3 24,5
- T2 22,9 Grad
- F6 9,8, F8 11,1, F14 19,1
- TN 16,9 (roh 36,9)
- AF Beine 0,36-0,58
- FR h85 765, FR h118 984

**Was die neuen Riegel nicht fangen:**

- FR h100 in V4 r4 hat 531 Bildpunkte bei Video 107, also das Fuenffache. Das ist der Befund S4 der Sicht r4, aber unter der Grenze 1070 und
  damit gruen.
- Im Gelb-Kriterium von AF zaehlt nur der Anteil, die gelben Bildpunkte selbst zaehlen nicht. Siehe 4.4.

## 3. Spiellauf (eigener Lauf sfv4/pz5/lauf, bitgleich zu sfv4/r/V4r5)

**Lauf:** Overlay-Wurzel pz5/lauf/cd mit dem eigenen Neubau a762bd66. Laut debug.log hat der Lauf "Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)"
geladen. Das Spiel-Asset im Baum ist unveraendert.

**Koerperfest (O3):**

- 47 Gangtakte: h85 und clip 1 von h90 bis h135.
- Kette gegen Ziel: max 0,056 H (h85 Ende), 0 Takte ueber 0,06.

**Hinweise fuer die Sicht** (kein Messblatt-Riegel; SF_belege/v4/PZ5_lauf_V4r5.json):

- Form gegen die Ruheform (Kabsch): 8 von 47 Takten ueber 0,06 H, max 0,065 bei h95. In V4 r4 waren es 11/47, in V4 r3 0/47.
- Welt-Abfall Biegung -> Ende: h88 0,038, h90 0,016, h94 -0,004 H. Das Endstueck liegt in h88-h97 also waagerecht und an h94 ueber der
  Biegung, wie in r4 4.4. Die Ursache ist die gezeichnete Pose (clip 11 / Ueberblendung), nicht die Kette.

**Rueckfaltung:** keine, max 79,4 Grad.

**Boden:** tiefster Punkt -19 E. (h35), kein Takt von 144 darunter.

**Licht:**

- Fuer die Ids 7/8, 9, 10, 11, 13 und 14 liegt dE00 zwischen b2 und FRAMEDUMP in h110-h135 bei 0,03-0,93.
- h100 liegt in der Ueberblendung (2,7-8,1), wie in r3/r4.

## 4. Befunde im Einzelnen

### 4.1 T5: rot oder gruen je nach Mittellinie

Breite bei f 0,5 der Achse Knick -> Beginn:

| Verfahren | f 0,1 / 0,3 / 0,5 / 0,7 | Abfall | |
|---|---|---|---|
| Bauer (riegel_v4.schwanz_mass_h) | 0,032 / 0,049 / **0,098** / 0,163 | 0,000 | ROT |
| Pruefer, Achse ab T1 (Wortlaut "Beginn" = T1) | 0,030 / 0,049 / 0,110 / 0,161 | 0,002 | gruen |
| Pruefer, Achse ab der Saat (Oberkante Ansatz) | 0,032 / 0,062 / 0,153 / 0,144 | **0,025** | ROT |
| T5q, quer zur t2_mass_v4-Mittellinie | 0,030 / 0,049 / 0,110 / 0,161 | 0,002 | gruen |
| Original, 3 Masken (dasselbe Verfahren wie T5q) | f 0,5 0,121 / 0,119 / 0,117 | 0,016 / 0,020 / 0,000 | |

- Das Profil springt jetzt zwischen f 0,4 und f 0,5 (0,062 -> 0,110 bzw. 0,047 -> 0,081 -> 0,110). Das ist der Zwischenring 'bauch'
  (Glied 2, t 0,15, Radius x 1,374).
- Im Original steigt die Breite von f 0,4 bis f 0,6 stetig (0,075 -> 0,121 -> 0,134).
- f 0,3 (0,049) und f 0,7 (0,161) liegen je 0,004 H an der Grenze.
- Wo die Saat beginnt, verschiebt den Sprung bis f 0,5. Die Zeile ist also nicht robust.

### 4.2 O11: T3/T5 gegen Runde 3

| | V4 r3 (dfbc30bc) | V4 r4 | V4 r5 |
|---|---|---|---|
| T3 dx (Pruefer) / Bauer | -0,248 / -0,248 | -0,234 / -0,233 | **-0,241 / -0,239** |
| Abstand zur Grenze -0,235 | 0,013 | -0,001 (rot) | 0,006 / 0,004 |
| T5 Bauer-Riegel | gruen (0,028/0,080/0,140/0,131) | rot | **rot** (0,098) |
| T5 Pruefer ab T1 | 0,032/0,080/0,138/0,127 (Abfall-Riegel rot, r4 4.1) | rot | gruen |
| T2 Drehung | 70,6 | 66,6 | 64,0 |

O11 lautet: "T3/T5 duerfen nicht schlechter sein als in Runde 3".

- T5 ist an der Riegelzeile des Bauers schlechter als in Runde 3: damals gruen, jetzt rot.
- T3 ist gruen, hat aber weniger als halb so viel Rand wie in Runde 3.
- Die Biegungsdrehung T2 naehert sich von Runde zu Runde der Untergrenze: 70,6 -> 66,6 -> 64,0 bei Grenze 63,4.
- Der Bauer belegt den Zielkonflikt T5 <-> T2-Drehung in Dossier Offen 1.

### 4.3 Schwanzfarbe: Mittel stimmt, die rote Unterseite fehlt

- TV ist als Querschnittsmittel an allen 5 Stationen gruen. Das Mittel der Schwanzfarbe trifft das Original.
- Auf dem Polygonzug Beginn -> Biegung (Unterseite, Nutzerpunkt Beginn (357,243) liegt an der Unterkante) fehlt das Rot:
  - Mitte (326,248): Original 6,8/7,8, Port (324,245) 8,1/3,3;
  - senkrecht: Original (292,280) 7,3/5,8, Port (294,278) 14,6/2,6.
- Dasselbe misst TR: a* > 12 im waagerechten Teil 0,020 gegen 0,102. Mit Weichzeichnung auch am Original ist es 0,077, der Port bleibt also
  auch in der fairen Lesart rot.
- Der Bauer erklaert TR mit der Weichzeichnung (Offen 3). Das erklaert nur einen Teil:
  - roh gegen roh ergibt 0,062 gegen 0,102;
  - weich gegen weich ergibt 0,020 gegen 0,077.
- TN: Die Quernaht bei x 297 (Zwischenring an der Biegung) bleibt in jeder Lesart ueber dem Original: weich 10,7 gegen 8,1, roh 13,5 gegen
  9,5. Mittelt man ueber den ganzen Querschnitt, gibt es keine Naht (3,9 gegen 4,9-6,5). Die Naht ist ein Sprung in der Bandmitte und kein
  Farbsprung ueber das ganze Glied.

### 4.4 Verfahren der neuen Riegel (Hinweise fuer den Orchestrator)

- **FR ist einseitig und an h100/h135 weit:**
  - Die Grenze "10 x Video" laesst V4 r4 h100 (531 Bildpunkte bei Video 107, Befund S4 der Sicht r4) passieren.
  - V4 r5 hat an h100/h135 0 Bildpunkte mit a* > 22, das Video 107/156. Das kraeftige Rot der Brustmitte fehlt im Spielbild also ganz.
  - Dazu passt: FO Buehne 6,48, aussen 0,095/0,084 gegen Original 0,178/0,242. Die Seiten sind halb so rot wie im Original.
  - "Rot nur in der Brustmitte wie im Original" (O10) ist damit uebererfuellt. Beide Riegel pruefen nur eine Richtung.
- **AF:**
  - Die Farbmenge enthaelt die Quellregion und einen Umkreis von 15 px, der Hintergrund einschliesst.
  - Ohne Quellregion (nur Umkreis, pz5_af.py) liegen die Bein-Meshes vorn bei 0,10-0,45. Port-Beine und Original-Beine liegen an
    verschiedenen Stellen (Pose 135, IoU 0,62). Die Lesart mit Quellregion ist die richtige; ich bestaetige sie fuer Beine und Kopf.
  - Die gelben Bildpunkte gehen nur ueber den Anteil ein, nicht als eigene Bedingung "gelb = 0". Mesh 13 in V4 r4 (47 px von 2626) waere
    allein dadurch knapp gefangen.
- **FO h118/h135 ist selbstbestaetigend:**
  - r2_koerper_farben.py stellt die Drittel-Faktoren an denselben Laufposen h118/h135 auf das Video-Verhaeltnis ein (DRITTEL_NUR_SPIEL,
    Lauf DRITTEL_LAUF = sfv4/r/V4r4).
  - Der Riegel misst genau dort: h135 2,76 gegen Video 2,75, h118 2,73 gegen 2,91.
  - Das ist eine Eichung auf die Messgroesse. Der Bauer nennt sie im Code als Ziel "Original-Verteilung, nicht die Riegelgrenze".

## 5. Konstanten im Diff cbc917a48..HEAD (O6)

### 5.1 Stellen

Die Engine hat sich nicht geaendert. Geaendert sind birkin_formen.py, r2_koerper_farben.py, r5_schwanz_textur.py, eichen_v4.py (nur
Kommentar) und riegel_v4.py/r5_mass.py (Riegel).

| Stelle | Wert | Quelle laut Kommentar | Urteil |
|---|---|---|---|
| SCHWANZ_V4 gelenke 3 | (-0,3003, +0,0743) | r5_form_suche Lauf "bauch" Saat 5 Schritt 46; Protokoll SF_belege/v4/r5/r5_form_suche_bauch5.log vorhanden | belegt |
| SCHWANZ_V4 gelenke 5-7 | +0,0075 / +0,015 H nach hinten, Knick 0,006 / Ende 0,002 H tiefer | "Laeufe r5 Form, formprobe" | **Protokoll nicht im Baum** (nur bauch5, q0.5, q0.7 unter SF_belege/v4/r5/) |
| durchmesser 3 | 0,0636 | Lauf "bauch" | belegt |
| bauch t / radius / aussen | 0,15 / 1,374 / 0,011 | Lauf "bauch"; Nutzer-Wortlaut/Entscheid O8, Messblatt 4 | belegt |
| seiten 11 | 11 | M1-Rechnung 198 <= 212 | belegt (nachgezaehlt 198) |
| uv art 'achse', v-Dichte | 255 / 3593 E. | M4-Rechnung; M4 0,060-0,072 nachgemessen | belegt |
| FARB_A_MAX | 21,7 | p99 a* der Vorlage 1 im POLY_135 | **nachgemessen 21,71** (p99,9 25,06, max 29,32) |
| gelb_grenze | 9,9 | max b* Rumpfbox / Kopfbox Vorlage 1 | **nachgemessen 9,9 / 9,1** |
| X_RUECKEN | 357 - round(0,05 x 253) = 344 | "dort endet die Leser-Maske oben (x 336)" | **Kommentar und Wert passen nicht** (344 gegen 336) |
| QUER_MAX | 1,0 | "Laeufe r5 Schwanz q0.5 / q0.7 / q1.0: rot 5 / 5 / 4" | Quelle vorhanden; ausgewaehlt nach der Zahl roter Riegel |
| LAM_MIN | 0,35 (= RK.LAM_MIN) | wie Koerper | OK |
| AF_DE / AF_ANTEIL / AF_UMKREIS (Riegel) | 10 dE76 / 0,01 / 15 px | Original-Nachbarbilder 0,0000 ueber 10; Pose-Versatz 0,05 H | Riegelparameter mit Begruendung; Umkreis siehe 4.4 |
| FR-Grenze | max(10 x Video, 1 Promille) | "Groessenordnung" | Lesart des Bauers; einseitig (4.4) |
| zeilen_je_c, rgw, MU, KACHEL_MIN, vorbau | | jetzt mit Nutzer-Wortlaut; vorbau-Kommentar nennt 0,053 (erster Versuch) und 0,048 (Endstand) | **r4-Beanstandung behoben** |
| DRITTEL_LAUF | sfv4/r/V4r4/debug.log | Laufposen h118/h135 | **Eingabe liegt nur im Scratchpad** (Lauf-Log nicht im Baum) |
| eichen_v4 rt | F15, F18 (F16/F17 "nicht benutzt") | Klarstellung + Dossier r5 berichtigt | O11 erfuellt (Dossier berichtigt, Abschnitt V4 r4 selbst nicht geaendert) |

### 5.2 Farbdateien nicht aus HEAD nachbaubar

Die committeten Dateien farben_schwanz_v4.npz (496613e2c) und farben_koerper_v4.npz (a0638b2dc) sind die eigentlichen Farb-"Konstanten".
Mit dem Code aus HEAD entstehen sie nicht bitgleich:

| Datei | Nachbau | Ergebnis |
|---|---|---|
| farben_schwanz_v4.npz | r5_schwanz_textur.py (Standard, 3 Runden) | Indizes max 241, CLUT max 28 verschieden; Protokoll Runde 3 dE76 1,86 gegen 1,70 committet; Runde 0 schon verschieden (2,1797 gegen 2,1783) |
| | dito mit RE15_V4_KOERPER = Koerperfarben Stand cbc917a48 (Stand vor dem Koerper-Commit) | verschieden (dE76 1,77) |
| farben_koerper_v4.npz | r2_koerper_farben.py mit --emd Standard (sfv4/r2), EM30_V4r4, EM30_V4_vor_r5, EM30_V4 (Endstand) | alle 4 verschieden; Drittel-Faktoren 0,681-0,792 gegen committet 0,754 / 0,801 |

- Der Aufruf (Eingabe-EMD, Umgebungsvariablen RE15_R5_*, Reihenfolge) steht weder im Dossier noch in den Protokollen.
- Folge fuer die Zahlen: Die Variante mit nachgebautem Schwanz (ad293f22) misst TN 11,4 (gemeldet 10,7), TR 0,033 (0,020) und TQ-Plateau
  0,26-0,33 (0,21-0,41). Sie haette dieselben roten Zeilen. Die Abweichung ist also klein, aber nicht null.
- In Runde 4 war die Eichdatei farben_v4.npz bitgleich nachbaubar.

## 6. Offen fuer Bauer und Orchestrator

1. **Rot nach Riegel:** T5 (Bauer-Mittellinie), TQ, TN, TR, AF 85 (Id 6), FO h118, FR h85, KS h135. Alle sind mit eigenem Code bestaetigt.
2. **O11:** T5 ist an der eigenen Riegelzeile schlechter als in Runde 3. T3 und T2 verlieren je Runde Rand (4.2).
3. **Schwanz:** Die rote Unterseite des waagerechten Teils fehlt (TR, Punktstationen 4.3). Die Bandmitte hat eine Naht am Zwischenring
   (TN 10,7 / roh 13,5).
4. **Riegel-Verfahren:** FR und FO sind einseitig. Der Port hat jetzt weniger Rot aussen und kein kraeftiges Rot, wo das Video welches
   zeigt (4.4). Die T5-Achse und TQ-Plateau haengen am Verfahren. Entscheiden muss der Orchestrator.
5. **O6:** Den Aufruf der Farbgeneratoren dokumentieren, damit die npz aus HEAD bitgleich entstehen. Dazu: die formprobe-Protokolle der
   Gelenke 5-7 und den Lauf DRITTEL_LAUF in den Baum legen und den X_RUECKEN-Kommentar berichtigen (5.1/5.2).
6. **Hinweise fuer die Sichtpruefung:**
   - Der Schwanz liegt in h88-h97 waagerecht (Form gegen Ruhe 8/47 ueber 0,06 H, r3 0/47).
   - Knapp an der Grenze sind T2 (64,0), T7a (0,056), T8 F17 (5,70), FO h135 (2,76), S3 und S4 a* (11,4).
7. **Tests:** Ein Test der V4-Regeln fehlt weiter. Die Integration prueft Ref2, nicht die Variante.

**Belege** (absolute Pfade):

- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_mess_V4r5_V4r4_V3r2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_neu_TN_TV_TR_AF_FR.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_af_lesarten.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_tq_tk_ks_fo.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_t2.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_lauf_V4r5.json
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_ctest.txt
- C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/PZ5_ausgaben.txt (T5, T5orig, r3-Vergleich, T8, T9,
  FS, Farben, G1, M4, AF Beine/Kopf, TV Querschnitt, Nachbau-Vergleich, FR je Lauf, Lauf, Licht)
- Scratchpad: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz5/
  (Neubau emd/, eigener Lauf lauf/, Renders r_*.npz/png, takt_eigen.txt/takt_bauer.txt, Nachbau nachbau/ und emd_nb/, Spielbilder
  spiel_V4r5_h118/h135.png, fr_spiel_h*.png)
