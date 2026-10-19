# Runde 39 Spur T "titelversion" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r39_titelversion` (Zweig `r39/titelversion`, Basis 832b68bd).
Nutzer (Nachtrag 14, woertlich): "Eine letzte Ergänzung - bei dieser und zukünftig neuen Versionen passe mir bitte die
Version im Titelscreen an. Entferne das bestehende. Ich glaube dafür gibt es auch schon ein Skript iregndwo."

Status: begonnen 2026-10-19.

## 1. Messung vorher

### 1.1 Wo die alte Beschriftung liegt (Datei-Messung, kein Raten)
`re15_port/shared_assets/PSX/DATA/TITLEU.TIM` (153620 B, sha256 `1acd3339ff6b60e8207e89fab87abb547f63bda5b21c043c134d48bd08e445c3`):
Kopf `10 00 00 00 02 00 00 00 0c 58 02 00 c0 02 00 00 40 01 f0 00` = Magic 0x10, Flag 2 (16 bpp, keine CLUT),
Datenblock 0x2580C B, VRAM (704,0), 320 x 240. Pixel (x,y) liegt bei Datei-Byte-Offset `0x14 + 2*(y*320+x)`.
Anzeige: `platform/pc/src/render_pc.c` `re15_render_pc_show_title` (einmalige Umwandlung RGB555 -> ARGB, Kanal = 5 bit << 3,
Alpha immer 0xFF -> das STP-Bit spielt fuer das Titelbild keine Rolle). Android uebersetzt dieselbe Datei
(`platform/android/jni/CMakeLists.txt`: libmain = platform/pc/main.c + platform/pc/src/*.c + engine).

Werkzeug: Python-Auswertung der Rohpixel (Skripte im Spur-Werkzeug `re15_port/tools/r39_titelversion/`).
Suchkasten x 188..295, y 4..35 (Rand ist komplett schwarz, 0 Nicht-Schwarz-Pixel auf dem Rand gemessen).
8-zusammenhaengende Komponenten der Nicht-Schwarz-Pixel im Kasten:

| Komponente (bbox) | Pixel | grau r=g=b | Maximum (5 bit) | Bedeutung |
|---|---|---|---|---|
| x213..223 y11..22 | 91 | ja | 25 | "S" |
| x225..232 y14..22 | 56 | ja | 25 | "a" |
| x234..241 y14..21 | 50 | ja | 25 | "m" |
| x226..283 y14..33 | 491 | ja | 25 | "p" (Unterlaenge beruehrt Zeile 2) + "2025.01.25" |
| x253..259 y11..22 | 36 | ja | 25 | "l" |
| x261..268 y14..22 | 63 | ja | 25 | "e" |
| x198..207 y24..33 | 57 | ja | 25 | "V" |
| x207..223 y27..33 | 85 | ja | 25 | "er." |
| x204 y21 | 1 | ja | 1 | Staub der Hintergrundkunst (Ader links oben, x150..196) |
| x191..196 y22..25 | 8 | ja | 2 | Auslaeufer derselben Hintergrund-Ader |

Trennung ist eindeutig: jede Beschriftungs-Komponente hat Glyphenkerne mit Wert 25, die beiden Kunst-Reste haben hoechstens 2.
**Maske = alle grauen Komponenten mit Maximum >= 20: 929 Pixel, bbox x198..283 y11..33** (Zeile "Sample" y11..23: 357,
Zeile "Ver.2025.01.25" y24..33: 572). Werte in der Maske (5 bit): 1:96 2:1 3:109 4:2 5:19 8:55 10:78 12:56 13:67 15:56 17:16
18:20 20:95 22:71 23:7 25:181; alle 929 mit STP=0.
**Hintergrund unter der Beschriftung: der 1-Pixel-Ring um die Maske (608 Pixel, 8-Nachbarschaft) ist zu 100 % `0x8000`**
(RGB 0,0,0 mit STP). Die Beschriftung steht also auf einheitlichem `0x8000`; Entfernen = Maskenpixel auf `0x8000`.
Logo/TM beginnen erst bei y 36 (rot, g=0) - ausserhalb des Kastens.

### 1.2 Die Pixelschrift der Zeile "Ver.2025.01.25" (Raster + Schattierung, gemessen)
Die Glyphen sind KEINE freie Vektorschrift, sondern eine Pixelschrift auf einem Raster von **1,12 px je Schrift-Pixel**,
jedes Schrift-Pixel als **1,52 px grosses Quadrat** gezeichnet (0,4 px Ueberlappung = fett), oberste Zeile ab **y 24,88**,
Glyphen 7 Schrift-Zeilen hoch; Wert = **round(25 x Flaechendeckung)** (25 = Schriftfarbe `0x6739` = (25,25,25)).
Belege (aus den Rohwerten, Spalten/Zeilen-Profile):
- senkrechte Striche: z. B. "1" Spalte 265 = 22 (0,88), Spalte 266 = 15 (0,60) -> Strich [265,12; 266,60) = 1,48 px;
  "0"(x233) Spalte 233 = 10 (0,40), 234 = 25 -> linker Strich ab 233,60.
- waagrechte Balken: Zeile 24 = 3 (0,12), Zeile 25 = 25, Zeile 26 = 10 (0,40) -> Balken [24,88; 26,40) = 1,52 px;
  Mittelbalken der "5" [27,12; 28,64) = 24,88 + 2 x 1,12; Unterbalken [31,60; 33,12) = 24,88 + 6 x 1,12.
- Tintenbreite der Ziffern 6,00 px = 5 x 1,12 + 0,40; Hoehe 8,24 = 7 x 1,12 + 0,40; Ziffern-Vorschub 6,72..6,76 px = 6 Raster.
- Gleiche Glyphe, gleiche Phase -> bitgleich: "2" bei x226 und x270 (Abstand 44 px) sind identisch; "0"(x233) und "5"(x277)
  haben dieselbe Phase (Tintenkante links bei .60).
Modell-Anpassung (Bitmuster je Glyphe per Bit-Abstieg, Phase in 0,04-px-Schritten, Raster 28/25 px, Quadrat 38/25 px):

| Glyphe (Stelle) | Phase (Tinte links) | Bitmuster 5x7 (Zeile 0..6) | SSE | max. Abweichung |
|---|---|---|---|---|
| 0 (x233) | 233,60 | .###. #...# #..## #.#.# ##..# #...# .###. | 135 | 5 |
| 0 (x257) | 257,36 | dasselbe | 136 | 4 |
| 2 (x226/x270) | 226,84 | .###. #...# ....# ...#. ..#.. .#... ##### | 146 | 4 |
| 2 (x240) | 240,36 | dasselbe | 162 | 5 |
| 5 (x247) | 247,09 | ##### #.... ####. #...# ....# #...# .###. | 132 | 4 |
| 5 (x277) | 277,56 | dasselbe | 102 | 4 |
| V (x198) | 198,80 | ###.### .#...#. .#...#. .#..#.. .#.#... .##.... .#..... (7 breit) | 185 | 5 |
| e (x207) | 207,80 | ..... ..... .###. #...# ##### #.... .#### | 85 | 4 |
| r (x214) | 214,60 | ..... ..... #.##. ##..# #.... #.... #.... | 77 | 4 |
| 1 (x264) | 264,01 | .# ## .# .# .# .# .# (2 breit) | 73 | 7 |
| . (x221 / x253) | 221,46 / 253,80 | 2x2 in Zeile 5..6 | 63 / 58 | 5 / 3 |

(je Glyphe ~70 Pixel; SSE in 5-bit-Stufen^2). Das Modell trifft die Originalwerte bis auf die aeusseren Ecken
(dort +-3..5 Stufen: das Original rundet die Ecken etwas weicher). "0" und "2" sind bitgleich mit der bekannten
5x7-Schrift des HD44780 (A00), "5" weicht in Zeile 3 ab (#...#), "V" ist 7 breit mit Kopfbalken.

### 1.3 Vorhandenes Skript fuer die Titelversion
Gesucht in diesem Baum und im Vorgaenger-Repo `C:/workspace/git/reAi` (Dateinamen und Inhalte: TITLEU, title/titel,
version/stempel): **es gibt keins.** `RE15_REBORN_VERSION_STRING "v0.1.0 Phase 4.1"` (`include/re15_engine.h:24`) wird
nirgends benutzt. `release/release_alles.sh` bestimmt die Version (hoechster Tag +1 bzw. `--version`) und gab sie bisher nur an
`build_android.sh --version` (versionName der APK) und `make_package.sh --version` (Dateinamen) - nicht an das Spiel.

## 2. RE-Belege
(folgt)

## 3. Umsetzung
(folgt)

## 4. Messung nachher
(folgt)

## 5. Tests
(folgt)

## 6. OFFEN
(folgt)

## 7. Fuer den Nutzer
(folgt)
