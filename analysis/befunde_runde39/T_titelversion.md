# Runde 39 Spur T "titelversion" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r39_titelversion` (Zweig `r39/titelversion`, Basis 832b68bd).
Nutzer (Nachtrag 14, woertlich): "Eine letzte Ergänzung - bei dieser und zukünftig neuen Versionen passe mir bitte die
Version im Titelscreen an. Entferne das bestehende. Ich glaube dafür gibt es auch schon ein Skript iregndwo."

Status: fertig 2026-10-19 (alle Punkte gemessen, belegt, gebaut, Riegel gruen).

## 1. Messung vorher

### 1.1 Wo die alte Beschriftung liegt (Datei-Messung, kein Raten)
`re15_port/shared_assets/PSX/DATA/TITLEU.TIM` (153620 B, sha256 `1acd3339ff6b60e8207e89fab87abb547f63bda5b21c043c134d48bd08e445c3`):
Kopf `10 00 00 00 02 00 00 00 0c 58 02 00 c0 02 00 00 40 01 f0 00` = Magic 0x10, Flag 2 (16 bpp, keine CLUT),
Datenblock 0x2580C B, VRAM (704,0), 320 x 240. Pixel (x,y) liegt bei Datei-Byte-Offset `0x14 + 2*(y*320+x)`.
Anzeige: `platform/pc/src/render_pc.c` `re15_render_pc_show_title` (einmalige Umwandlung RGB555 -> ARGB, Kanal = 5 bit << 3,
Alpha immer 0xFF -> das STP-Bit spielt fuer das Titelbild keine Rolle). Android uebersetzt dieselbe Datei
(`platform/android/jni/CMakeLists.txt`: libmain = platform/pc/main.c + platform/pc/src/*.c + engine).

Werkzeug: `python re15_port/tools/r39_titelversion/messen.py` (liest nur; gibt 1.1 und 1.2 genau so aus, ~30 s).
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
Modell-Anpassung (`messen.py`: je Glyphe fuer jede der 25 Phasen auf dem 1/25-px-Raster ein Startmuster aus den
Rasterzellen-Mitten, dann Bit-Abstieg gegen das Flaechenmodell; Raster 28/25 px, Quadrat 38/25 px, oben 622/25 px;
Nachbarspalten, die zur Nachbar-Glyphe gehoeren, ausgeblendet):

| Glyphe (Stelle) | Phase (Tinte links) | Bitmuster (Zeile 0..6) | SSE | max. Abweichung |
|---|---|---|---|---|
| 0 (x233) | 233,60 | .###. #...# #..## #.#.# ##..# #...# .###. | 135 | 5 |
| 0 (x257) | 257,36 | dasselbe | 136 | 4 |
| 2 (x226) | 226,84 | .###. #...# ....# ...#. ..#.. .#... ##### | 146 | 4 |
| 2 (x240) | 240,36 | dasselbe | 162 | 5 |
| 5 (x247) | 247,08 | ##### #.... ####. #...# ....# #...# .###. | 132 | 4 |
| 5 (x277) | 277,60 | dasselbe | 96 | 5 |
| V (x198) | 198,80 | ###.### .#...#. .#...#. .#..#.. .#.#... .##.... .#..... (7 breit) | 157 | 5 |
| e (x207) | 207,84 | ..... ..... .###. #...# ##### #.... .#### | 100 | 4 |
| r (x214) | 214,60 | ..... ..... #.##. ##..# #.... #.... #.... | 77 | 4 |
| 1 (x264) | 264,00 | .# ## .# .# .# .# .# (2 breit) | 73 | 7 |
| . (x221) | 221,32 | 2x2 in Zeile 5..6 | 62 | 4 |
| . (x253) | 253,80 | dasselbe | 58 | 3 |

(je Glyphe ~70 Pixel; SSE in 5-bit-Stufen^2). Das Modell trifft die Originalwerte bis auf die aeusseren Ecken
(dort +-3..5 Stufen: das Original rundet die Ecken etwas weicher). "0" und "2" sind bitgleich mit der bekannten
5x7-Schrift des HD44780 (A00), "5" weicht in Zeile 3 ab (#...#), "V" ist 7 breit mit Kopfbalken.

### 1.3 Vorhandenes Skript fuer die Titelversion (selbst gesucht)
- Dieser Baum: `git ls-files` nach Namen (titl/titel/version/stempel) und Inhalt (`TITLEU`, `Sample Ver`, `Ver.20..`):
  nur die Runde-30-Werkzeuge `analysis/befunde_runde30/titel-blinken_tools/*` (Puls/Blinken des Menues, nicht die
  Beschriftung). `RE15_REBORN_VERSION_STRING "v0.1.0 Phase 4.1"` (`include/re15_engine.h:24`) wird nirgends benutzt
  (grep: nur die Definition). git-Historie: kein Commit zur Titelbild-Version vor dieser Runde.
- Vorgaenger-Ordner `C:/workspace/git/reAi`: enthaelt nur noch `psx_dev/re15_reborn_pc/build/Debug/nul` (1 Datei, kein
  git) - dort gibt es nichts mehr.
- `release/release_alles.sh` bestimmt die Version (hoechster Tag +1 bzw. `--version`) und gab sie bisher nur an
  `build_android.sh --version` (versionName der APK) und `make_package.sh --version` (Dateinamen) - nicht ans Spiel.
**Ergebnis: es gab kein Skript dafuer** (dem Nutzer gesagt, Abschnitt 7).

### 1.4 Titelabzug vorher (gebauter Stand 832b68bd, Fensterskala 1)
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=150
RE15_INV_SHOT=exit34.bmp` (exe-Kopie mit eigenem Namen). Bild: `T_belege/titel_vorher.png`, Ausschnitt
`T_belege/titel_vorher_zoom.png`. Gegen die rohe Umwandlung von TITLEU.TIM (Kanal = 5 bit << 3): 2222 Abweichungen, alle
im Menue (x88..229 y136..218, NEW GAME/LOAD GAME/OPTION aus TMOJI.TIM); im Kasten x188..295 y4..35: **0** - die alte
Beschriftung wird 1:1 aus der Datei gezeigt.

## 2. RE-Belege
Kein Original-Verhalten: die Beschriftung ist Teil der Bilddatei, keine Zeichenroutine. Belege:
- `DATA/TITLEU.TIM` ist bytegleich mit dem Auslieferungsstand `info/Re1.5/PSX/DATA/TITLEU.TIM` (sha256 `1acd3339...`;
  `TITLEU.TIM.bak` daneben ebenso). Die Beschriftung steht als Pixel in der Datei: Pixel (x,y) ab Datei-Byte
  `0x14 + 2*(y*320+x)`; z. B. "V" Zeile 25 ab Byte `0x4020`: `63 0c 39 67 39 67 39 67 ef 3d ef 3d 39 67 39 67 39 67 63 0c`
  = Grau 3 25 25 25 15 15 25 25 25 3 (`0x0c63`/`0x6739`/`0x3def`, xxd selbst gelesen).
- `info/Re1.5/PSX/DATA/TITLEJ.TIM` (japanisches Titelbild derselben Scheibe, sha256 `b0d6733e...`) traegt im selben Kasten
  nur 14 Nicht-Schwarz-Pixel und kein "TM" - dort steht keine Beschriftung.
- Der Port zeigt das Bild ohne Zutat (render_pc.c `re15_render_pc_show_title`: RGB555 -> ARGB, Alpha 0xFF).
Damit ist alles hier **NUTZER-VORGABE** (Nachtrag 14), die FORM belegt aus der Datei: Ort (Maske, Kasten), Hintergrund
(Ring 608/608 = `0x8000`), Schrift (Raster 1,12 / Quadrat 1,52 / y 24,88 / round(25 x Deckung)), Lage (rechtsbuendig an
der Tintenkante 283,60 der alten Zeile, Abstaende 0,72 / 2,74 px). Die ergaenzten Glyphen 3 4 6 7 8 9 d v haben keine
Vorlage in der Datei: ihre Bitmuster sind **PORT-WAHL** (HD44780 A00, die 5x7-Schrift, aus der "0" und "2" der Zeile
bitgleich stammen); Raster und Schattierung belegt (3.3, Riegel `unit_r39_titelversion_glyphen`).

## 3. Umsetzung

### 3.1 Dateien
| Datei | Inhalt |
|---|---|
| `re15_port/engine/src/titelversion_r39.c` + `include/re15_titelversion_r39.h` | Maske, Entfernen (0x8000), Text, Layout, Zeichnen, Haken-Funktion `re15_titelversion_r39_bild` |
| `re15_port/engine/src/gen/titelversion_r39_glyphen.inc` | Glyphen-Tabelle (16 Kacheln, Herkunft je Glyphe im Kommentar) |
| `re15_port/tools/r39_titelversion/glyphen_bauen.py` | Generator der Tabelle aus TITLEU.TIM (`--pruefen` vergleicht nur) |
| `re15_port/cmake/re15_version_r39.cmake` | RE15_VERSION: `-D` > Umgebung > `git describe --tags --abbrev=0 --match "v[0-9]*"` + " dev" > "dev"; Definition nur fuer titelversion_r39.c; lokal Configure-Abhaengigkeit auf packed-refs / refs/tags (neuer Tag -> naechster Bau konfiguriert neu) |
| Haken `re15_port/engine/CMakeLists.txt` (1 Zeile) | `include(re15_version_r39)` |
| Haken `re15_port/platform/pc/src/render_pc.c` (3 Zeilen + include) | `re15_render_pc_show_title`: die Umwandlung liest `re15_titelversion_r39_bild(tim->pixels, ...)` (Kopie im Speicher) |
| `release/release_alles.sh` (Ausnahme 1.3) | Schritte windows/linux mit `env RE15_VERSION="$NEU"` (Umgebung statt Argument: die Aufruflisten der r38-Riegel bleiben woertlich gleich); Plan- und Kopftext |
| `release/build_win_cross.sh`, `release/build_linux_deck.sh` (Ausnahme 1.3) | `--version V` oder Umgebung, sonst Host-git (Tag + " dev"); `ENV_WEITER += RE15_VERSION` -> `docker run -e` -> docker_*_build.sh -> cmake liest die Umgebung; distrobox-Weg reicht sie im Befehl durch |
| `re15_port/platform/android/app/build.gradle` (Ausnahme 1.3, 3 Zeilen) | `-DRE15_VERSION=` aus `-Pre15Version` (nur wenn angegeben); build_android.sh gibt `-Pre15Version` schon weiter |

`TITLEU.TIM` bleibt bytegleich (Riegel `anwenden` liest die Datei nach dem Umschreiben erneut: bytegleich). Kein
Asset-Patch, keine neue Datei im Paket. docker_win_build.sh / docker_linux_build.sh / make_package.sh / Gates unveraendert.

### 3.2 Konstanten (je mit Beleg)
| Konstante | Wert | Beleg |
|---|---|---|
| Kasten | x188..295, y4..35 | Rand 0 Nicht-Schwarz-Pixel; enthaelt alle 929 Beschriftungs-Pixel (bbox x198..283 y11..33) |
| `RE15_R39T_KERN` | 20 | Beschriftungs-Komponenten: Maximum 25; Kunst-Staub: Maximum 1 bzw. 2 (Tabelle 1.1) |
| `RE15_R39T_HINTERGRUND` | 0x8000 | Ring um die Maske 608/608 Pixel = 0x8000 |
| Raster / Quadrat / oben | 28/25 px, 38/25 px, 622/25 px | Strich- und Balkenkanten 1.2 (z. B. "1" Spalten 265/266 = 22/15; Balken [24,88; 26,40)) |
| `RE15_R39T_RECHTS25` | 7090 (283,60 px) | Tinte der letzten "5" [277,60; 283,60): Spalte 277 = 10 (0,4), 283 = 15 (0,6) |
| `RE15_R39T_ABSTAND25` | 18 (0,72 px) | 1 Raster - 0,40; gemessen 0,65..0,92 zwischen den Zeichen der Zeile |
| `RE15_R39T_WORT25` | 69 (2,74 px) | Tinte "." (Ver.) bis 224,10, "2" ab 226,84 |
| Schriftfarbe | 25 (5 bit), STP 0 | Kerne 25 (`0x6739`), alle 929 Maskenpixel STP 0 |
| Kachel-Tintenkanten | je Glyphe im .inc | Phase aus der Modell-Anpassung 1.2 (`messen.py`, 1/25-px-Raster): V .80, e .84, r .60, . .32, 0 .60, 1 .00, 2 .84, 5 .60 |

### 3.3 Glyphen
| Zeichen | Quelle | Kachel |
|---|---|---|
| V e r . 0 1 2 5 | AUSGESCHNITTEN aus TITLEU.TIM, Zeilen 24..33: V x198..207 (Spalte 207 nur Zeilen 24..26), e x207..213 (Spalte 207 nur 27..33), r x214..220, . x221..223, 0 x233..239, 1 x264..266, 2 x226..232 (= x270..276 bitgleich), 5 x277..283 | Werte bytegleich (Riegel) |
| 3 4 6 7 8 9 d v | ERGAENZT: Bitmuster HD44780 A00, Phase .60 wie 0/5/r; je Pixel der Wert derselben lokalen Lage (Spalte, Zeile, Nachbar-Schrift-Pixel) aus 0/5/r, sonst Flaechenmodell. Aus Original-Lage: 3:18, 6:15, 8:26, 9:12, d:4 Pixel; Rest Modell | 7 Spalten, Tinte [0,60; 6,60) |

Gegenprobe des reinen Flaechenmodells an den Lern-Glyphen: 0(x233) SSE 135 max 5, 5(x277) SSE 96 max 5, r(x214) SSE 77
max 4. Eigenes Modell im Riegel (unabhaengig vom Generator): Ausschnitte weichen hoechstens 4..7 Stufen ab (V 5, e 4, r 4,
. 4, 0 5, 1 7, 2 4, 5 5), die ergaenzten 0..5 (3 5, 4 0, 6 5, 7 0, 8 5, 9 3, d 5, v 0) - also nicht mehr als die
Ausschnitte. Vorschau aller Ziffern + "dev": `T_belege/glyphen_vorschau.png` (oben das Original).

### 3.4 Layout und Zeichnen
Rechtsbuendig von rechts nach links: Kachelspalte = round((Ziel-Tintenkante - Tinte rechts)/25); Ziel fuer das naechste
Zeichen = Tinte links - 0,72 px (nach "Ver." und vor "dev" 2,74 px). Gegenprobe: die alte Zeile "Ver.2025.01.25" landet mit
diesen Konstanten an V199 e208 r215 .222 2227 0234 2240 5247 .254 0257 1264 .267 2270 5277 - hoechstens 1 Spalte neben den
Original-Stellen 198/207/214/221/226/233/240/247/253/257/264/267/270/277, die letzte "5" genau bei 277.
Zeichnen: Deckung v/25 als Alpha in Schriftfarbe 25 ueber das Bild (auf 0x8000 genau der Kachelwert), STP 0.
Anzeige: "v0.8.27" -> "Ver.0.8.27"; alles hinter X.Y.Z (" dev", "-5-gabc" aus `git describe --always`) -> " dev"; ohne
Ziffern "Ver.dev". Passt die Zeile nicht rechts von x188, wird nur entfernt ("NICHT GEZEICHNET" im Protokoll).
Protokoll (debug.log, einmal je Prozess): `[titel_r39] alte Beschriftung entfernt: 929 Pixel; neu "Ver.0.8.26 dev"
(RE15_VERSION "v0.8.26 dev") Spalten 200..283`.

## 4. Messung nachher
| Lauf | Ergebnis |
|---|---|
| lokaler Bau (ohne Angabe), Endstand, AF 600 | debug.log `neu "Ver.0.8.26 dev" (RE15_VERSION "v0.8.26 dev") Spalten 200..283`; Abzug vorher (AF 150) / nachher ausserhalb des (pulsierenden) Menues NUR in x198..283 y11..33 verschieden (981 Pixel); nachher gegen rohes TITLEU.TIM ausserhalb Kasten + Menue: 0. `T_belege/titel_nachher_v0.8.26_dev.png` / `_zoom.png` (erster Stand vor der Phasen-Angleichung e/.: Spalten 199..283, 955 Pixel bei AF 150) |
| `-DRE15_VERSION=v9.9.9` (eigener Bau `re15_port/build_v999`; in PowerShell das Argument in Anfuehrungszeichen - ohne trennt PowerShell 5.1 bei "." und CMake sah "v9") | Configure `RE15_VERSION (Titelbild): 'v9.9.9' (CMake-Variable RE15_VERSION)`; debug.log `neu "Ver.9.9.9" (RE15_VERSION "v9.9.9") Spalten 229..283`; Abzug AF 600: ausserhalb Kasten + Menue 0 Abweichungen, Zeile "Sample": Maske schwarz, Rest 0 Abweichungen. `T_belege/titel_v9.9.9.png` / `_zoom.png` |
| Linux-Container (Image `re15-linux-build:deb11` wie build_linux_deck.sh, Quellbaum als Kopie, `-e RE15_VERSION=v9.9.9` wie `container_lauf`, derselbe Configure wie docker_linux_build.sh:70-72) | `RE15_VERSION (Titelbild): 'v9.9.9' (Umgebung RE15_VERSION)`; Compile-Befehl titelversion_r39.c `-DRE15_VERSION=\"v9.9.9\"`; `strings re15_pc` -> `v9.9.9`; `ctest -R r39_titelversion` dort: 9 Unit-Riegel gruen (Linux-fest), Integration: Protokollzeile `neu "Ver.9.9.9" (RE15_VERSION "v9.9.9")` richtig, Bildvergleich dort nicht aussagekraeftig (Videotreiber offscreen + Mesa-GL, Bild 150 noch in der Einblende: 23325 Abweichungen, `T_belege/titel_linux_container_bild150.png`) -> wird dort seitdem wie in test_r30_titel_puls SICHTBAR uebersprungen (RE15_OHNE_GPU), die Protokoll-Pruefungen bleiben Pflicht |
| Android | Abschnitt 4.1 |

### 4.1 Android
Weg von `release/build_android.sh` ohne APK/Assets (nur der native Teil, Messskript im scratchpad): `app/.cxx` geloescht
(wie build_android.sh), `local.properties` sdk.dir, `bash ./gradlew --no-daemon --console=plain -Pre15Version=v9.9.9
fetchSdl2 :app:externalNativeBuildRelease` im Baum (JDK 17.0.15, SDK `%LOCALAPPDATA%/Android/Sdk`, NDK 27.2, SDK-CMake
3.22.1, AGP 8.7.3) -> **BUILD SUCCESSFUL in 5m 23s**.
- `app/build/intermediates/cxx/RelWithDebInfo/41306e4f/logs/{arm64-v8a,x86_64}/configure_command.bat`: alle bisherigen
  Argumente (`-DRE15_BUILD_PC=OFF` ... `-DANDROID_PLATFORM=android-24`) und dazu `"-DRE15_VERSION=v9.9.9"` - das
  zweite `arguments` haengt an, es ersetzt nichts.
- `.../logs/{arm64-v8a,x86_64}/configure_stdout.txt`: `-- RE15_VERSION (Titelbild): 'v9.9.9' (CMake-Variable RE15_VERSION)`.
- Compile-Befehl titelversion_r39.c (compile_commands.json, beide ABIs): `-DRE15_VERSION=\"v9.9.9\"`.
- `libmain.so` arm64-v8a und x86_64 (`app/build/intermediates/cmake/release/obj/...`): je 1 Treffer `v9.9.9` und
  `[titel_r39]`.
Ohne `-Pre15Version` gibt build.gradle nichts weiter (`if (project.findProperty("re15Version"))`); build_android.sh selbst
setzt ohne `--version` `git describe --tags --always` (z. B. `v0.8.26-5-g832b68bd`) -> Anzeige "Ver.0.8.26 dev" (Riegel
`text`). Ein Titelabzug auf Geraet/Emulator wurde nicht gemacht (Abschnitt 6).

### 4.2 Linux-Container, zweiter Lauf (Endstand der Riegel)
Derselbe Weg wie oben mit dem Endstand: `ctest -R r39_titelversion` im Container **100% tests passed, 0 tests failed
out of 10**; der Integrations-Riegel meldet dort (ausfuehrlich, RE15_OHNE_GPU=1 wie docker_linux_build.sh:92):
`r39_titelversion_titel: [titel_r39] alte Beschriftung entfernt: 929 Pixel; neu "Ver.9.9.9" (RE15_VERSION "v9.9.9")
Spalten 229..283` und `SKIP Bildvergleich (RE15_OHNE_GPU gesetzt - keine echte GPU); Protokollzeile geprueft:
'Ver.9.9.9', RE15_VERSION 'v9.9.9', 929 Pixel entfernt`.

## 5. Tests
Neue Riegel (registriert nur aus `re15_port/tests/unit/probes/r39_titelversion.cmake`; Unit-Quelle
`tests/unit/test_r39_titelversion.c`, Skripte `tests/unit/r39_titelversion/`, Integration
`tests/integration/test_r39_titelversion_titel.cmake`):

| Test | misst | Ergebnis (Windows) |
|---|---|---|
| unit_r39_titelversion_glyphen | 16 Glyphen; Ausschnitte bytegleich mit TITLEU.TIM an der dokumentierten Stelle; eigenes Flaechenmodell (Raster 1,12 / Quadrat 1,52 / y 24,88) trifft die Ausschnitte bis 4..7 Stufen und die ergaenzten bis 0..5 (nie mehr als die Ausschnitte); Kern 25, Halo-Zeilen 24/33 <= 4 wie bei 0/1/2/5 | 50/50 |
| unit_r39_titelversion_text | Version -> Anzeige (9 Faelle + NULL + zu kleiner Puffer) | 11/11 |
| unit_r39_titelversion_layout | alte Zeile "Ver.2025.01.25" mit den Konstanten hoechstens 1 Spalte neben den Original-Stellen, letzte "5" genau x277; neue Zeilen rechts an 283,60 (Tinte 7090/25), Abstaende 0,72/2,74 px +-0,48, links >= x188 | 10/10 |
| unit_r39_titelversion_maske | 929 Pixel, bbox x198..283 y11..33, 357/572 je Zeile, grau + STP 0, Ring 608 x 0x8000, jeder Kern >= 20 im Kasten erfasst, Kunst-Staub (9 Pixel) nicht in der Maske; Kunstfaelle (Rand, rot, ohne Kern) bleiben | 11/11 |
| unit_r39_titelversion_anwenden | "v0.8.27"/"v0.8.26 dev"/"v9.9.9": 929 entfernt, ausserhalb Maske + neuer Kacheln bytegleich, Maske ohne Kachel = 0x8000, neue Zeile exakt; letzte "9" bei x277 = Kachel; falsches Mass -> -1; TITLEU.TIM auf der Platte bytegleich | 15/15 |
| unit_r39_titelversion_version | einkompilierte Version == die von CMake bestimmte | 2/2 |
| unit_r39_titelversion_cmake | Versionsquelle (cmake -P): -D, Umgebung, -D schlaegt Umgebung, leeres -D, git-Tag + " dev" (Fremd-Tag danach zaehlt nicht, neuer Tag zaehlt), ohne git "dev" | 8/8 |
| unit_r39_titelversion_huelle | ECHTE build_win_cross.sh / build_linux_deck.sh mit docker-Attrappe: --version, Umgebung, --version schlaegt Umgebung, ohne Angabe Host-Tag + " dev", ohne git keine Weitergabe -> `docker run ... -e RE15_VERSION=...` | 12/12 |
| unit_r39_titelversion_ablauf | ECHTES release_alles.sh (Wegwerf-Repo, r38-Attrappen umhuellt): Trockenlauf zeigt RE15_VERSION=v0.8.26 an windows/linux; Lauf bis linux: windows/linux bekommen RE15_VERSION=v0.8.26, android --version v0.8.26; Aufrufliste der r38-Attrappen unveraendert | 12/12 |
| integration_r39_titelversion_titel | echte exe (Kopie mit eigenem Namen, eigener Laufordner, keine RE15_* geerbt), Titelabzug Bild 600 Fensterskala 1: debug.log-Zeile mit 929 Pixeln und der CMake-Version; ausserhalb des Kastens (ohne Menue) = rohes TITLEU.TIM (0); Zeile "Sample" weg (0); Abzug == Erwartung pixelgleich (0 Abweichungen, 499 neue Pixel, "Ver.0.8.26 dev" Spalten 200..283). Ohne GPU (RE15_OHNE_GPU) Bildvergleich sichtbar uebersprungen | gruen |

Gezielte Laeufe im Spurbaum (keine volle Suite), Endstand:
- `ctest --test-dir re15_port/build -R "r39_titelversion|r30_titel|boot_bg_pin|dark_start_pin" --timeout 600`:
  **100% tests passed, 0 tests failed out of 14**
- `ctest --test-dir re15_port/build -R "r39_titelversion|r38_releaseskript|r35_android|r34a_asset_abgleich|r30_android|r30_titel|boot_bg_pin|dark_start_pin" --timeout 600`
  (Release-Riegel test_r38_releaseskript*, unit_r35_android_* inkl. Pruefkette/apk-Gate-Mutanten, Titel-Haken):
  **100% tests passed, 0 tests failed out of 30** (vor der Phasen-Angleichung e/.; release/ seitdem unveraendert)
- alle Unit-/Prop-/Sonden-Riegel `ctest --test-dir re15_port/build -R "^(unit_|prop_|probe_)" -j4 --timeout 600`:
  **100% tests passed, 0 tests failed out of 888** (Endstand, 603 s; erster Lauf vor der Phasen-Angleichung ebenfalls 888/888)
- Linux-Container (`re15-linux-build:deb11`, `-e RE15_VERSION=v9.9.9`): `ctest -R r39_titelversion`
  **100% tests passed, 0 tests failed out of 10**

Linux-fest: im Container `re15-linux-build:deb11` liefen alle 10 Riegel gruen (9 Unit + Integration mit sichtbar
uebersprungenem Bildvergleich, Abschnitt 4.2). Parallel-fest: eigene
Laufordner/Zufalls-Id, exe-Kopie mit Praefix `re15_pc_r39t_<id>`, kein Glob ueber fremde Kopien, RE15_* des Aufrufers
geloescht; `cmake_minimum_required(VERSION 3.16)` in jedem -P-Skript; keine libm-Funktionen.

## 6. OFFEN
- `RE15_MIN_TESTS` (re15_port/tools/local_build.sh, Untergrenze 960) nicht angehoben: +10 Tests dieser Spur - gemeinsame
  Datei, Anhebung bei der Integration (die Untergrenze bleibt ohnehin erfuellt).
- Linux-Container: der Bildvergleich des Integrations-Riegels ist dort nicht aussagekraeftig (offscreen + Mesa-GL, Titel
  bei Bild 150 noch in der Einblende) und wird wie bei integration_r30_titel_puls sichtbar uebersprungen; die
  Protokoll-Pruefungen (Version, 929 Pixel, Anzeige) laufen auch dort.
- Android: nur Konfigurations-/Kompilier-Beleg (Abschnitt 4.1), kein Titelabzug auf einem Geraet/Emulator - der
  Titel laeuft dort durch denselben Code (render_pc.c + engine), die Version kommt ueber -Pre15Version.
- Ein vollstaendiger build_linux_deck.sh-Lauf (mit voller Suite) gehoert zum Release, nicht zur Spur.
- Integration: neue Quelldatei `engine/src/titelversion_r39.c` (GLOB) -> im Hauptbaum `local_build.sh configure` vor dem
  Bau; RELEASE_NOTES.md (tabu) nennt die Titelbild-Version noch nicht.

## 7. Fuer den Nutzer
- **"Ich glaube dafuer gibt es auch schon ein Skript irgendwo":** Es gab keins - weder in diesem Repo noch im alten
  reAi-Ordner (der enthaelt nur noch eine leere Datei). Die Version kam bisher nur in die Paket-Dateinamen und in die APK,
  nicht ins Spiel. Das ist jetzt eingebaut.
- **Was du siehst:** Oben rechts im Titelbild stehen "Sample" und "Ver.2025.01.25" nicht mehr. Dort steht jetzt die
  Version in derselben Pixelschrift, rechtsbuendig wie vorher, z. B. **"Ver.0.8.27"** im Release v0.8.27. Logo und "TM"
  bleiben unveraendert. Die Bilddatei TITLEU.TIM wird nicht veraendert - das Spiel schreibt die Zeile beim Laden.
- **Automatisch bei jeder neuen Version:** `release/release_alles.sh` gibt seine Version an den Windows-, Linux- und
  Android-Bau weiter. Du musst nichts tun. Von Hand geht es auch: `bash release/build_win_cross.sh --version v0.8.27`,
  `bash release/build_linux_deck.sh --version v0.8.27`, `bash release/build_android.sh --version v0.8.27` (wie bisher).
- **Lokaler Bau** (die exe im Hauptverzeichnis, `local_build.sh`): zeigt die letzte veroeffentlichte Version mit "dev",
  z. B. **"Ver.0.8.26 dev"**, nach dem Release-Tag v0.8.27 dann "Ver.0.8.27 dev". So erkennst du, dass es kein
  Release-Paket ist.
- **Ziffern 3, 4, 6, 7, 8, 9 und "dev":** die alte Zeile enthielt nur 0, 1, 2 und 5. Die uebrigen Zeichen sind im selben
  Raster und mit derselben Schattierung ergaenzt (Form der klassischen 5x7-Pixelschrift, aus der auch die 0 und die 2
  der alten Zeile stammen). Vorschau: `analysis/befunde_runde39/T_belege/glyphen_vorschau.png`.
- Neue Assets fuer das Paket-Gate: **keine**. Sprachdateien: **keine**. Bedienung: unveraendert.
- Entscheidungsfragen: keine offenen. (Falls dir eine der neuen Ziffern nicht gefaellt: sag welche - die Form steht
  an einer Stelle, `re15_port/tools/r39_titelversion/glyphen_bauen.py`, ERGAENZT.)

## 8. Nachbesserung 1 (Abnahme 0, Mangel M1)
Status: in Arbeit (2026-10-19). Abnahmebericht: `analysis/befunde_runde39/T_abnahme_0.md` Abschnitt 4 und 10.
Mangel M1 (T4, Auskunft an den Nutzer): Abschnitt 1.3 ("Ergebnis: es gab kein Skript dafuer") und Abschnitt 7 ("Es gab
keins - weder in diesem Repo noch im alten reAi-Ordner ...") sind widerlegt. Abhilfe laut Abnahme: nur Dokument, kein Code.
