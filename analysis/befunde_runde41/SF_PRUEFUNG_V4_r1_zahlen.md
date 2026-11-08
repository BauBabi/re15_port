# SF V4 Runde 1: Zahlen-Pruefung (Pruefer, 2026-11-07)

Gegenstand: Bau V4 der Sichel-Form (Commits adfab2a13, d16ea573c, 68624fc6a auf r41/sichel_nb2), Modell
sfv4/emd/EM30_V4.EMD (sha256 0dddbdbe...), gemessen gegen SF_MESSBLATT_V4.md (Commit 7a4f3b684). Kein RE-Befund, keine
@0x-Konstante: die Sichel-Form gibt es auf der Disc nicht, alles ist Bildmessung am 80%-Video 4.

**Ergebnis: nicht bestanden.** Die Bauer-Tabelle (33 gruen / 10 rot) ist in einer Zeile falsch bewertet (T4), die neue
Engine-Konstante SW_GLATT_KNICK_AB wirkt fuer V4 gar nicht, und drei weitere Riegel sind nach unabhaengiger Messung rot
oder grenzwertig. Die Kopfriegel vorn und die Schwanzform relativ zum Beginn bestaetige ich.

## 0. Verfahren (eigener Code, nicht riegel_v4.py)

Alle Werkzeuge in re15_port/tools/r41_sichel/v4/pz_*.py (nicht committet), Laufausgaben im Scratchpad sfv4/pz/.

| Werkzeug | misst |
|---|---|
| pz_g1.py, pz_g1b.py | eigener EMD-/MD1-/EMR-/TIM-Parser: Sektionen, Mesh-Bloecke, Keyframe-Bytes, CLUT-Zeilen, Texel-Diff gegen die Koerper-UVs |
| pz_m4.py | Texeldichte sqrt(UV-Flaeche / Modellflaeche) je Mesh |
| pz_orig.py | Nutzer-Linien: Maske RGB (237,28,36), Skelett-Enden, Belegung je Start x Ende |
| pz_render.py | Render V4 / V3 r2 / Ref2 in posen_v4 "85" und "135" (b2.zeichne = vorgeschriebener Messrender, Messblatt Abschn. 7) |
| pz_mess.py | alle K-, S-, T-, F-Riegel: Vergleichsregel (sigma 1,5, V = a + b x P), L* statt Luminanz, Fleck = hellstes 3x3 im 2 px erodierten Kopf, Mittellinie per geodaetischer Distanz |
| pz_t2.py, pz_t2orig.py | Biegungs-Drehung mit drei Methoden, an V4 und am Original-Schwanz (Masken sfv4/schwanz_maske*.npy) |
| pz_lauf.py, pz_lauf_vgl.py, pz_lauf_bild.py | Spiellauf sfv4/r/V4: Kettengelenke aus debug.log, Gelenkwinkel, Boden, Projektion in die FRAMEDUMP-Bilder |
| pz_licht.py, pz_spielkopf.py | Lichtgegenprobe b2 gegen FRAMEDUMP, Kopfriegel direkt am Spielbild |
| pz_kf.py | Gelenkwinkel der Schwanzkette in den Keyframes (V4 / V3 / Ref2) |

**Selbstpruefung am Original** (pz_mess.py orig_check): meine Fleck- und Fensterfunktionen liefern auf den
Vorlagebildern die Messblatt-Ziele: Auge vorn 46,5 / 7,2 / -1,1 (Ziel 46,9 / 7,2 / -1,1), 9 x 10 px, Schwerpunkt
(377,1 / 167,2); Auge seitlich 48,3 / 3,9 / 0,6 (Ziel 48,4 / 4,0 / 0,5), 10 x 14 px, (452,5 / 237,2); Fenster KV1-KV5,
KS1, KS2 jeweils dE00 0,13-0,58 zum Ziel. **Linien** (pz_orig.py): alle 8 Zuordnungen wie im Messblatt, Belegung 1,000;
beste Alternative Bild 1 Haare 0,946, Bild 2 Haare 0,608, Auge 0,548. Enden Bild 1 (382,165) (403,160) (394,177), Bild 2
(459,235) (467,241) (357,243) (290,308) (294,344).

## 1. Riegel-Tabelle Soll / Bauer / Pruefer

H vorn (Pose 135) 364 px (Oberkante 145, Sohle 509 auf der verlaengerten Leinwand), H seitlich (Pose 85) 264 px
(170 / 434). Lab = (L*, a*, b*), Vergleichsregel des Messblatts.

| Riegel | Soll | Bauer | Pruefer (eigener Code) | Bewertung |
|---|---|---|---|---|
| G1 Koerper = Ref2 | Meshes 0-8, 10-15, Skelett, KF 0-15, EDD byte-gleich | OK | Sektionen 0, 1, 3-6 byte-gleich; EMR: rel 0-15 und Knochentabelle gleich, rel 16-22 neu; Keyframes 912 x 116 Byte, Bytes 0-83 (Kopf + Knochen 0-15) in allen 912 gleich, Abweichung nur ab Byte 84; MD1 Meshes 0-8, 10-15: alle 8 Bloecke (tv, tn, tf, tuv, qv, qn, qf, quv) gleich; TIM: CLUT 480-482 geaendert, 483-485 gleich, 486/487 neu; 22970 Texel geaendert, davon 0 in UV-Flaechen der Koerpermeshes (3047 Kopf vorn, 3557 Kopf seitlich, 16366 Schwanz) | OK |
| Spiel-Asset | EM30_HANGAR.EMD = Pin | unveraendert | sha256 cfd1ac42729e... = release/em30_hangar.sha256; 7a4f3b684..HEAD beruehrt weder Asset noch Pin | OK |
| M1 | <= 212 | 160 | 30/20/20/20/20/20/30 = 160 (Ref2 720) | OK |
| M2 | <= 118 | 111 | 111 (Ref2 109; +1 Viereck "Straehne") | OK |
| M3 | 8 bpp, CLUT | OK | TIM-Flag 0x9 (8 bpp mit CLUT), Kopf CLUT 486 / TPage 128, Schwanz 487 / 130, Koerper 480-482/484 | OK |
| M4 | 0,045-0,091 | Kopf 0,062, Schwanz 0,049-0,066 | gleich (Ref2: Kopf 0,091, Schwanz 0,086-0,115) | OK |
| K1 | 1 Fleck, Kontrast >= 20 | 1, 31,6 | 1 Komponente (78 px), 31,6 | OK |
| K2 | 46,9 / 7,2 / -1,1, dE00 <= 6 | 47,3 / 7,3 / -1,4, 0,5 | 47,7 / 7,3 / -1,6, dE00 0,88; L* 47,7 >= KV5 20,0 + 20 | OK |
| K3 | 0,016-0,036 x 0,023-0,043 H | 0,022 x 0,036 | 7 x 13 px = 0,019 x 0,036 | OK |
| K4 | 0,035-0,065 links (B), 0,07-0,12 unter OK | 0,056 / 0,112 | 0,055 / 0,111 | OK (Hinweis 4.3) |
| K5 | 13,3 / 7,4 / 1,5, L* <= Auge - 25 | 0,3 | 13,2 / 7,2 / 1,7, dE00 0,32, L* 13,2 <= 22,7 | OK |
| K6 | L* <= 18 | 11,2 | 11,1 (Spiegelbox 370-377 x 188-200) | OK |
| K7 | 18,2 / 7,1 / 1,3 | 0,9 | 18,3 / 6,3 / 1,0, dE00 0,86 | OK |
| K8 | 9,9 / 9,9 / 3,8; Anteil L* > 35 <= 0,05 | 3,1 / 0,000 | 11,9 / 13,2 / 4,3, dE00 3,05; Anteil 0,000, L* max 16,1 | OK |
| K9 | 22,6 / 15,4 / 5,2, a* >= 11 | 0,2 | 22,9 / 15,4 / 5,4, dE00 0,25 | OK |
| K10 | 0,14-0,18 x 0,15-0,20 H | 0,143 x 0,187 | Box 327-378 x 154-221: 0,140 x 0,184 (Differenz) bzw. 0,143 x 0,187 (inklusiv) | OK, Breite an der Untergrenze |
| S1 | 48,4 / 4,0 / 0,5; 0,028-0,052 x 0,040-0,070 H | ROT (0,023 x 0,030, 5 Komp.) | 47,5 / 3,6 / 0,5, dE00 0,97; 6 x 8 px = 0,023 x 0,030 H; 1 Komponente (36 px) | **ROT** (Groesse) |
| S2 | 0,03-0,07 hinter VK; 0,76-0,82 ueber Sohle | 0,046 / 0,818 | 0,046 / 0,818 | OK, aber nur durch das Straehnen-Viereck (4.4) |
| S3 | L* <= 22, 19,1 / 1,2 / 1,1, Abstand <= 0,01 H | 0,8 / 0,008 | 19,5 / 0,9 / 1,6, dE00 0,75; Abstand Fleck-Fenster 0,011 H (3 px) | grenzwertig |
| S4 | 27,6 / 15,3 / 3,3 | 0,8 | 27,5 / 14,3 / 2,9, dE00 0,75 | OK |
| T1 | 0,15 hinter Scheitel, 0,767 ueber Sohle (+-0,03) | ROT (0,214 / 0,727) | Beginn (346,242): 0,218 H (Scheitel = Mitte oberste Zeile 403,5) bzw. 0,246 H (Plateau-Mitte 411,0, Plateau x 395-427); **0,727 H ueber Sohle** | **ROT in x UND y** (4.1) |
| T2 Lage | dx -0,245, dy +0,036 | -0,244 / 0,025 | 45-Grad-Durchgang der Achse (286,2 / 249,0): -0,227 / +0,026 | OK |
| T2 Drehung | 60-100 Grad in <= 0,10 H | 68 | max. in 0,10 H: Tangente +-0,025 H **54,1**, Sehnen je 0,05 H **44,2**; Original mit denselben Methoden 66-70 bzw. 45-48 | **grenzwertig / ROT** (4.2) |
| T3 | -0,265 / +0,257 | -0,263 / 0,255 | (277,9 / 311,0): -0,258 / +0,261, Breite 0,021 H | OK |
| T4 | -0,245 / +0,397 **und 0,37 H ueber Sohle** (+-0,03) | OK (Text: "0,330 H ueber Sohle") | -0,239 / +0,391; **0,336 H ueber Sohle** (unterster Pixel 348: 0,326) | **ROT** - riegel_v4.py prueft die Sohlenhoehe nicht (Zeile 245-247) |
| T5 | 0,03 / 0,065 / 0,125 / 0,14, Abfall <= 0,01 | 0,034 / 0,066 / 0,123 / 0,140 | 0,032 / 0,066 / 0,127 / 0,144; Abfall 0,000 | OK |
| T6 | B 0,025-0,05, L 0,12-0,17, 75-95 Grad, Taille schmalste | 0,028 / 0,144 / 81 | Median 0,028, L 0,131, 81,9 Grad (nach vorn), Taille 0,021 < 0,025 (schmalste ab Biegung) | OK |
| T7 Gang | T1-T4 +-0,06 H | ROT (alle Takte) | bestaetigt; zusaetzlich Rueckfaltung im Lauf (Abschnitt 3) | **ROT** |
| T8 | dE00 <= 6, Wurzel a* >= 7, Ende a* <= 6 | 3,0 / 0,9 / 0,4 / 0,3 | F15 9,3 / 8,5 / 3,2 dE00 3,02 (a* 8,5); F16 9,1 / 8,8 / 3,4 dE00 1,04; F17 26,5 / 0,0 / 1,9 dE00 0,52; F18 15,3 / 2,9 / 1,5 dE00 0,36 (a* 2,9) | OK |
| F6 Brust | 16,6 / 17,3 / 6,2 | 1,5 | 16,6 / 14,9 / 5,9, dE00 1,66, dL 0,0 | OK |
| F7 Oberarm (W)-rechts | 33,3 / 5,5 / 2,7 | 4,2 | 35,3 / 8,8 / 1,7, dE00 3,84, dL +2,0 | OK |
| F8 Oberarm (W)-links | 16,4 / 8,3 / 1,5 | ROT 18,5 | 39,7 / 6,4 / 0,5, dE00 17,72, dL +23,3; **Ref2 31,4 L*, dE00 11,07: V4 ist um 8 L* weiter weg als vorher** | **ROT** |
| F9 Jeans | 12,7 / -0,9 / -1,5 | 2,4 | 15,2 / 0,4 / -2,3, dE00 2,64 | OK |
| F10 Klaue (B)-rechts | 11,2 / 14,0 / 2,5 | 0,8 (Id 12) | Id 12: 11,1 / 13,4 / 1,6, dE00 0,78. Id 12 ist richtig: Id 15 ist die Sichel (W)-rechts bei x 294-387, (B)-rechts liegt Id 12 (x 409-447); das Messblatt nennt faelschlich Id 15 | OK |
| F14 Buckel | 60,0 / 11,0 / 1,2, abs(dL) <= 4 | 4,3 / dL -3,7 | 55,4 / 8,3 / 0,8, dE00 4,83, **dL -4,6** (P70 auf L*) | grenzwertig / ROT nach eigener Messung |
| F19 Jeans seitlich | 18,4 / 0,5 / -3,5 | 2,4 | 16,1 / 0,1 / -2,0, dE00 2,15 | OK |
| F-Mittel | <= 4 | ROT 4,87 | 4,80 (ohne F8 2,65) | **ROT** |
| F0 Wand | dE00 <= 2 | 0,52 | 65,2 / 0,2 / -2,5, dE00 0,52 | OK |
| F0 Licht | dE00 <= 3 | 0,1 / 0,1 | eigene Entzerrung FRAMEDUMP -> Video (M x Skalierung 1/3): Rumpf h118 / h125 / h135 dE00 0,21 / 0,36 / 0,25; Kopf 0,51 / 0,42 / 0,73 | OK |

Zaehlung nach eigener Messung: rot S1, T1, T4, T7, F8, F-Mittel; grenzwertig (je nach Messdefinition rot) T2-Drehung,
F14, S3. Gegenueber der Bauer-Tabelle neu rot: **T4** (sicher), dazu T2/F14 je nach Methode.

## 2. Tests und Gegenprobe

- Bau: "bash re15_port/tools/local_build.sh build" -> "ninja: no work to do", LOCAL-BUILD-OK (sfv4/pz/build.log).
- ctest -R "unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form": **4/4 gruen**, 65,8 s
  (sfv4/pz/ctest.log). Einschraenkung: integration_r41_sichel_form laedt das Spiel-Asset (Ref2,
  tests/integration/test_r41_sichel_form.cmake Zeile 50-54), unit_schwanz_r41 enthaelt SW_GLATT_KNICK_AB nicht. Die Tests
  sagen also nichts ueber V4 und nichts ueber die neue Engine-Regel.
- Gegenprobe V3 r2 (eigene Messung, sfv4/pz/mess_V4_V3_REF2.json): rot K1 (Kontrast 2,8), K3 (1 x 1 px), K4 (-0,034),
  K5 (dE00 13,6), K6 (34,2), K7 (8,5), K8 (23,9, Anteil 0,70), K9 (16,6), K10 (0,124), S1 (2 Komponenten), S2 (0,078 / 0,900),
  S3 (24,5), S4 (17,4), T1, T2 (Drehung 15-25 Grad), T3 (-0,306 / 0,401), T4 (0,266 ueber Sohle), T5 (f 0,7 0,174), T6
  (L 0,083), T8 F15 / F16 / F17 (8,9 / 9,0 / 6,5), F6-F10, F14 (19,1), F19 (5,6). **Die Riegel trennen V3 r2 klar.**

## 3. Spiellauf sfv4/r/V4 (RE15_FRAMEDUMP 225-415, debug.log)

- **Bild**: In allen Gangbildern h90-h135 (Clip 1) zeigt die Spielkamera 50A0/12 die Figur von vorn; die in die
  FRAMEDUMP-Bilder projizierte Kette liegt ganz innerhalb der Koerpersilhouette (sfv4/pz/lauf_kette.png). Knick und
  Breite sind im Spielbild des Gangs **nicht messbar**, nur h20 (Clip 11) zeigt den Schwanz links neben der Figur.
- **Kette (Weltkoordinaten aus "[hangar-takt] hN Schwanz B")**: Gliedlaengen in allen 144 Takten 393-395 / 646-649 /
  650-652 / 572-573 / 482-484 / 262-264 / 262-264 (Knick bleibt am Gelenk 5). Biegung am Gelenk 3 in h118 / h125 / h135:
  67,9 / 58,3 / 59,3 Grad (Keyframe 63,0 / 61,8 / 63,0). In h100 / h110 liegt sie nicht dort (21,5 / 20,9 Grad).
- **Rueckfaltung**: in **73 von 144 Takten** ist ein Gelenk ueber 90 Grad abgeknickt, darunter **20 von 46 Gangtakten**
  (h90-h109), Maximum **160,9 Grad**. Beispiel h100: Gelenk 6 144,6 Grad, Kette fast waagerecht, Spitze 1531 E. ueber dem
  Boden. Die Keyframes selbst haben hoechstens 84,5 Grad (pz_kf.py). Vergleich gleicher Lauf: V3 r2 0 Takte (max. 16,2 Grad),
  Ref2 (R2n) 81 Takte (max. 168,5 Grad).
- **Ursache**: glaetten() laeuft nur fuer Ketten mit bone_relative_pos[16].x < SW_GLATT_WURZEL_X = -700
  (schwanz_r41.c Zeile 431). V4 hat rel16 = (-588, -355, 40), V3 (-764, ...), Ref2 (-380, ...). **Fuer V4 wird glaetten()
  nie aufgerufen; die neue Konstante SW_GLATT_KNICK_AB (Commit adfab2a13) ist fuer V4 wirkungslos.** Der Kommentar im
  Generator und in schwanz_r41.c ("Dieselbe Regel gilt beim Nachschwingen", "die Biegung behaelt ihren Winkel") beschreibt
  ein Verhalten, das im Spiel nicht eintritt. Zusaetzlich ist die Aussage "V3 bleibt unveraendert" falsch: 214 von 1358
  V3-Keyframes haben Gelenkwinkel ueber 30 Grad (bis 75,5); dort hebt die Regel die 16-Grad-Grenze auf.
- **Boden**: tiefster Kettenpunkt ueber den ganzen Lauf y -172 (h65), Fussgelenke tiefstens -209: **Schwanz nicht unter
  dem Boden, OK**.

## 4. Befunde im Einzelnen

### 4.1 Schwanz absolut 11-12 px hinter den Nutzerpunkten

Die Riegel T2-T4 messen relativ zum Beginn, und der Beginn ist per PORT-WAHL verschoben (beginn x 0,21 statt 0,15). In
derselben Kamera (Pose "85" gegen Vorlage 2) liegen die Port-Punkte direkt im Bild:

| Punkt | Nutzer-Linienende | V4 | Abstand |
|---|---|---|---|
| Beginn | (357,243) | (346,242) | -11 / -1 px |
| Knick | (290,308) | (277,9 / 311,0) | -12 / +3 px |
| Ende | (295,343) | (282,8 / 345,3), unterster Pixel (283 / 348) | -12 / +2..5 px |

11-12 px = 0,045 H2 (253) bzw. 0,044 H (264): ausserhalb +-0,03 H. Der ganze Schwanz sitzt um diesen Betrag weiter
hinten als die beschrifteten Punkte. Dazu ist T1 auch in der Hoehe rot (0,727 statt 0,767 H): das liegt nicht an der
PORT-WAHL x, sondern daran, dass die Unterseite des 0,15 H dicken Ansatzes tiefer liegt als der Bezugspunkt "beginn" des
Generators. Die Rueckenkontur von Ref2 (Knochen 0-15) liegt in 0,767 H Hoehe 0,229 H hinter dem Scheitel (oberste Zeile)
bzw. 0,258 H (Plateau-Mitte), in 0,727 H 0,210 H (pz_t2.py): die Begruendung der PORT-WAHL stimmt in der Groessenordnung.
Entscheidung offen: relative Form (jetzt) oder absolute Nutzerpunkte; beides geht mit G1 nicht gleichzeitig.

### 4.2 Biegung weicher als im Original

Richtungsprofil der V4-Mittellinie (Grad, 0 = waagerecht nach hinten, 90 = senkrecht): 20-24 bei s 0,17-0,22 H, 40-46
bei 0,25-0,31, 76 bei 0,34, 98 bei 0,41. Die Drehung verteilt sich auf zwei Stufen (20 -> 45 -> 95 Grad). Groesste Drehung
in 0,10 H: 54,1 Grad (Tangente +-0,025 H), 44,2 Grad (zwei 0,05-H-Sehnen). Der Bauer kommt mit Tangenten ueber +-0,01 H
(2-3 px, pixelnah) auf 68 Grad. Am Original-Schwanz (Masken der Leser) ergeben dieselben Methoden 66-70 Grad (Tangente)
bzw. 45-48 Grad (Sehnen). Mit der Tangenten-Methode ist V4 also 12-16 Grad weicher als das Original und unter 60 Grad.

### 4.3 Kopf im echten Spielbild h135

Die Kopfriegel gelten fuer die angepasste Pose "135" (Clip 0 Bild 9, Gier 1737). Das Spiel zeigt bei h135 Clip 1 Bild
45, Gier 1513. Am entzerrten FRAMEDUMP F378 (Kopfbox aus dem b2-Render der Laufpose) gemessen: K3 0,037 x 0,040 H
(Breite > 0,036), K4 0,033 links / 0,126 unter OK (beide ausserhalb), K5 dE00 6,0 (an der Grenze), K6 L* 20,4 (> 18),
K10 0,139 x 0,201 (beide knapp ausserhalb). h118 und h125 liegen ueberwiegend innerhalb (K6 15,8 / 18,5, K5 dE00 4,5 / 5,5).
Im Kopfkasten liegt das Auge bei 0,11 der Breite vom (B)-linken Rand (Original 0,19): K4 ist erfuellt, weil der Kopf
schmaler ist (K10 0,140 gegen 0,159), das Auge sitzt dadurch fast auf der Kontur.

### 4.4 S2 und S3 haengen am Straehnen-Viereck

Spalten x 478-483 der Kopf-Ids in Pose 85 sind zu 100 % L* < 12, Zeilen 210-230 je Spalte gleich: ein flaches dunkles
Viereck, das 6 px vor die eigentliche Kopfvorderkante (x ~477) ragt. Die Vorderkante fuer S2 (max x Id 9 = 483) ist
dieses Viereck; ohne es laege der Augenfleck 0,023 H hinter der Kante (S2 rot). Das Messblatt erlaubt 2 flache Dreiecke
fuer die Vorderkante (Abschnitt 8 zu N27), Masse 6 x 21 px passen zur Haarkante (2-6 x 21 px). Im Bild ist es ein
hartkantiges Rechteck ausserhalb der Kopfkontur (Sichtpruefung A1 entscheidet).

### 4.5 F8: Eichung verschlechtert den Oberarm (W)-links

Ref2 31,4 L* -> V4 39,7 L* (Ziel 16,4). Die Haut-CLUT-Zeilen 480-482 werden fuer beide Arme gemeinsam geeicht; F7 (Ziel
33,3) zieht heller, F8 (Ziel 16,4) braucht dunkler. Mit einer gemeinsamen CLUT ist F8 nicht erreichbar, solange der Arm
im Port so beleuchtet ist; F8 ist der einzige Grund fuer F-Mittel rot (ohne F8 2,65).

## 5. Konstanten im Generator-Diff 7a4f3b684..HEAD

| Stelle | Wert | Quelle laut Kommentar | Urteil |
|---|---|---|---|
| SCHWANZ_V4 pose / V4_POSE_SEITE / V4_POSE_VORN | posen_v4 "85" / "135" | Messblatt Abschn. 7, posen_v4.py | belegt (IoU 0,62 / 0,619 nicht unabhaengig nachgemessen) |
| beginn | (0,21, 0,767) | x PORT-WAHL Rueckenkontur, y Messblatt T1 | x belegt als PORT-WAHL; y wird im Bild nicht erreicht (0,727) |
| gelenke (8 Punkte) | (0,12 -0,08) ... (-0,245 0,422) | Messblatt Abschn. 4, nachgestellt an T1-T6 (form_suche_v4, gitter_v4) | belegt als Riegel-Anpassung |
| durchmesser | 0,16 0,15 0,153 0,128 0,0475 **0,022** 0,038 0,038 | Abschn. 4 Breitenzeile, Knick 0,022 "damit schmalste" | Knick 0,022 H liegt **unter** der gemessenen Taille 0,024-0,032 H (Messblatt Abschn. 4); als PORT-WAHL nicht gekennzeichnet |
| dorn | 0,8 | PORT-WAHL, Spitze nicht aufloesbar | belegt |
| glieder / seiten / ueberlapp / deckel | 7 / 10 / 0,45 / True | M1, "wie V3" | belegt |
| uv clut / page / u / dichte / baender | 487 / 130 / 64-127 / 0,068 / 4 x 64 Zeilen | Atlasbelegung Ref2, M4-Mitte | belegt |
| bewegung knick ab | 30 Grad | PORT-WAHL (Ruhewinkel 60 Grad) | belegt als Gestaltung; im Generator (Keyframes) wirksam, in der Engine fuer V4 **wirkungslos** (Abschnitt 3) |
| KOPF_V4 skal | (1,3, 0,9, 1,4) | Breite/Hoehe aus Eichlauf 1 (0,159/0,137 x 1,2 = 1,39; 0,172/0,192 = 0,90 nachgerechnet) | Breite, Hoehe belegt; **Tiefe 1,3 ohne Herleitung** (Begruendung nennt 0,22 H gegen 0,11 H = Faktor 2,0) |
| KOPF_V4 vorbau | d 200, z0 -60, z1 60, xa 100, xb 320, ya -50, yb -150 | d aus S2 (0,049 H = 13 px = ~190 E.) | d belegt; **z0, z1, xa, xb, ya, yb ohne Quelle** |
| straehne | x 0,86-0,99, y 0,50-0,84, vor 100 | KS2 0,84-0,96 x 0,52-0,80, Eichlaeufe 6 und 8 | belegt als Eichlauf; x reicht bis 0,99 ueber KS2 hinaus |
| vorn / seite u, v | 0-47 / 56-111, v 160-223 | freie Atlasflaeche | belegt |
| schwelle | 0,05 | - | **ohne Quelle** |
| Bogenmitte | Gelenk 2 x/z, Gelenk 5 y, halber Weg zu Gelenk 3 | "liegt zwischen Ansatz und senkrechtem Stueck" | **ohne Mass-Quelle**; bestimmt die Grenze aussen/unten der Schwanzfarben |
| schwanz_r41.c SW_GLATT_KNICK_AB | 0,5236 (30 Grad) | PORT-WAHL | Quelle genannt, aber fuer V4 tot (rel16 x -588 >= -700) |

## 6. Offen fuer den Bauer

1. T4 neu bewerten (rot) und riegel_v4.py um die Sohlenhoehe ergaenzen.
2. glaetten() fuer V4 aktivieren (SW_GLATT_WURZEL_X oder eigenes Kriterium), sonst bleibt die Rueckfaltung bis 161 Grad
   in 20 von 46 Gangtakten; danach Lauf und T7 wiederholen. Kommentare in schwanz_r41.c und birkin_formen.py berichtigen.
3. Entscheid absolute Nutzerpunkte gegen relative Form (4.1) dem Nutzer vorlegen; T1 y (0,727) erklaeren oder beheben.
4. Biegung: Drehungsmass festlegen (Tangentenfenster) und am Original gegenrechnen (4.2).
5. S1 (Auge seitlich zu klein) und F8 bleiben rot; F14 und S3 liegen an der Grenze.
6. Konstanten ohne Quelle (Abschnitt 5) belegen oder als PORT-WAHL kennzeichnen.

Belege: sfv4/pz/ (Scratchpad) mess_V4_V3_REF2.json, mess_alle.txt, g1_V4.json, r_*_85/135.png/.npz, lauf_kette.png,
lauf_uebersicht.png, licht_h118/125/135.png, spielkopf_h118/125/135.png, kopf_vgl.png, build.log, ctest.log.
