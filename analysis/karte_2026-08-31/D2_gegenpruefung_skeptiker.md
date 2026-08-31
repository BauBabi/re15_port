# Gegenpruefung [D2] — "Ausserhalb der Titel-Schleife loescht keine Codestelle `s_tfade_sub`"

Rolle: SKEPTIKER. Ziel war Widerlegung. **Ergebnis: NICHT widerlegt — unabhaengig bestaetigt,
statisch (Quelle + Objektdatei-Relokationen) UND dynamisch (2 Laeufe, Pixelmessung).**
Datum 2026-08-31.

---

## 1. Statischer Gegen-Xref (nicht der zitierte grep, sondern Symbolebene)

### 1a. Der Static hat GENAU EINEN Schreiber in seiner eigenen TU
`re15_port/platform/pc/src/render_pc.c` — alle 4 Vorkommen von `s_tfade_sub`:

| Zeile | Art |
|---|---|
| 94   | `static uint8_t s_tfade_sub = 0;` (Definition, Datei-Static ⇒ nur diese TU kann schreiben) |
| 1197 | **LESEN** `if (s_tfade_sub > 0) {` |
| 1202 | **LESEN** `SDL_SetRenderDrawColor(s_renderer, s_tfade_sub, s_tfade_sub, s_tfade_sub, 255);` |
| 1302 | **SCHREIBEN** `void re15_render_pc_title_fade_sub(int b) { s_tfade_sub = clamp(b); }` |

⇒ ausserhalb des Setters existiert im ganzen Programm kein Schreibpfad.

### 1b. Der Lese-/Zeichenblock liegt in `re15_render_end_frame`, nicht in einem Title-Pfad
Funktionsgrenzen in `render_pc.c` (objdump-unabhaengig per Symbolzeilen geprueft):
`re15_render_end_frame` beginnt **Zeile 666**, naechste Funktion erst **Zeile 1299**
(`re15_render_pc_set_title_fade`). Die Zeilen 1197-1205 liegen also im Rumpf von
`re15_render_end_frame` ⇒ der ABR2-Vollbildquad
(`SDL_ComposeCustomBlendMode(ONE, ONE, REV_SUBTRACT, ZERO, ONE, ADD)`, Rect 0,0,320,240)
wird in **jedem** Frame gezeichnet, auch im Gameplay. Kein Title-Gate im Umfeld.

### 1c. Setter-Aufrufer: Objektdatei-Relokationen statt grep
`nm` ueber ALLE `.obj` des Builds (`re15_port/build`):
- `U re15_render_pc_title_fade_sub` **nur** in `platform/pc/CMakeFiles/re15_pc.dir/main.c.obj`
- `T re15_render_pc_title_fade_sub` in `.../src/render_pc.c.obj`
⇒ keine weitere Uebersetzungseinheit ruft den Setter (schliesst Aufrufe aus, die ein
Text-grep verfehlen koennte, z.B. ueber Makro/anderes Modul).

`objdump -d -r main.c.obj`: **6** Relokationen auf `re15_render_pc_title_fade_sub`
(Offsets `9ca0`, `9d5f`, `9d76`, `9e38`, `9ee8`, `9f5a`) — alle innerhalb des Symbols
`main` (`0x8e27 <main>`, danach kein weiteres Symbol in der Objektdatei).
Die Quelle hat genau 6 Aufruf-Statements: main.c **2602, 2613, 2618, 2638, 2658, 2670**
(die Zeilen 2588/2668 sind nur `extern`-Deklarationen).

⚠️ Praezisierung zur zitierten Beweisführung: „10 Schreib-Stellen" ist eine **grep-Zeilen**-Zahl
(6 Aufrufe + 2 extern-Deklarationen + Definition + Kommentar/Leseseite), nicht 10 Schreibvorgaenge.
Substanziell aendert das nichts.

### 1d. Klammer-Analyse: liegen wirklich ALLE Aufrufe in der Titel-Schleife?
Eigener Klammertiefen-Scanner ueber `main.c` (String-/Zeichen-/Kommentar-sicher):
Schleifenrumpf `while (re15_gameflow_mode() == RE15_MODE_TITLE) {` hat Tiefe 3;
gemessene Tiefen: 2602→8, 2613→5, 2618→5, 2638→5, 2658→5, **2670→3**,
`re15_render_end_frame()` 2671→3, `tblink++` 2676→3, schliessende Klammer **2677→2**,
`re15_render_pc_hide_title();` **2678→2 (ausserhalb)**.
⇒ alle 6 Schreibaufrufe liegen im Schleifenrumpf; die erste Anweisung nach der Schleife
(`re15_render_pc_hide_title`) schreibt den Wert nicht.

### 1e. Die naheliegenden „Release"-Kandidaten pruefen ihn NICHT
- `re15_render_pc_hide_title` (render_pc.c:1392): `s_title_show = 0;` — sonst nichts.
- `re15_render_pc_hide_title_menu` (render_pc.c:1648): `s_tmoji_show = 0;` — sonst nichts.
- `re15_render_pc_set_fade` (render_pc.c:1303-1312): setzt **nur** `s_fade_alpha`.
- Game-Loop-Nullung `main.c:4671-4675` `{ extern void re15_render_pc_set_fade(int a);
  re15_render_pc_set_fade(0); }` — trifft ebenfalls nur `s_fade_alpha`. **Bestaetigt.**
- `s_title_fade`-Aufrufe: 1814/1842 (Player-Select, Ausgang 0), 2240/2320 (Config, Ausgang 0);
  in der Titel-Schleife selbst kein Aufruf ⇒ `s_title_fade` scheidet als Erklaerung fuer eine
  Rest-Abdunklung im Spiel aus.

---

## 2. Dynamische Gegenmessung (eigener Lauf, nicht die Zahlen der Untersuchung)

Kanal: `RE15_FRAMEDUMP="<start>-<end>:<pfad>"` — Rueckleseauftrag wird **innerhalb**
`re15_render_end_frame` **vor** `SDL_RenderPresent` ausgefuehrt (main.c:8042-8069 setzt ihn,
render_pc.c:659/1261 fuehrt ihn aus) ⇒ der komplett komponierte Frame **inklusive** der
Fade-Quads (render_pc.c:1177-1210). Kein `RE15_SOFTWARE_RENDER`, kein `RE15_AUTOSHOT`.
Exe: `re15_port/build/platform/pc/re15_pc.exe` (5.773.854 B, 2026-08-31 20:14).
Fenster 960x720, PPM 2.073.615 B.

⚠️ Falle (gemessen): aus Git-Bash gesetzt kommt `RE15_FRAMEDUMP` beim Prozess mangled an
(MSYS-Pfadlisten-Konvertierung am `:`), Lauf B lieferte 0 Dumps. Erst per PowerShell
(`$env:RE15_FRAMEDUMP="150-500:C:\...\B_"`, `Start-Process`) kamen die 351 Frames.

Zwei Laeufe, identischer Pfad, nur `RE15_TITLE_SHOT_AF` verschieden (Auto-Advance-Zeile
main.c:2675 verlaesst die Titel-Schleife bei `tblink == AF`, **nachdem** Zeile 2670 gelaufen ist):

- Lauf A: `RE15_NO_INTRO=1 RE15_TITLE_SHOT_AF=40` ⇒ Vorhersage `B = 255 - (40>>1)*8 = 95`
- Lauf B: `RE15_NO_INTRO=1 RE15_TITLE_SHOT_AF=80` ⇒ Vorhersage `B = 255 - (80>>1)*8 = 0` (geklemmt)

Gemessen (Graustufen-Byte-Statistik ueber die vollen 960x720x3 Frames):

| Gameplay-Frame | A max | A mean | B max | B mean | **B_max − A_max** |
|---|---|---|---|---|---|
| 150 | 121 | 0.56 | 216 | 2.05  | **95** |
| 200 | 121 | 0.56 | 216 | 27.00 | **95** |
| 300 | 121 | 0.83 | 216 | 32.49 | **95** |
| 400 | 121 | 0.16 | 216 | 56.39 | **95** |
| 500 | 160 | 6.87 | 216 | 65.75 | (Szene heller, s.u.) |

- Der Maximalwert ist in A um **exakt 95** kleiner als in B — genau der vorhergesagte
  Restwert von Zeile 2670. Vier Frames in Folge, exakter Zahlentreffer.
- Differenz-Histogramm (jedes 7. Byte) Frame 150: Modus der Nicht-Null-Differenzen liegt
  bei **95** (2406 Treffer); alle uebrigen Differenzen < 95 = an 0 geklemmte Pixel.
  Ein multiplikativer Alpha-Blend haette keine konstante Differenz erzeugt, sondern eine
  helligkeitsproportionale ⇒ die Operation ist **subtraktiv**, wie ABR2 in render_pc.c:1197.
- Der Effekt steht noch bei **Gameplay-Frame 500** (Lauf A max 160 vs B 216, mean 6.87 vs 65.75)
  ⇒ der Wert wird auf dem ganzen gemessenen Spielabschnitt nicht geloescht.

---

## 3. Wo die Behauptung ungenau ist (ohne sie zu kippen)

1. **„10 Schreib-Stellen"** = grep-Zeilen, tatsaechlich **6 Aufrufe** (durch 6 Objekt-Relokationen
   belegt). Kosmetisch.
2. **„der Wert ueberlebt in das gesamte Spiel"** ist bedingt: die *letzte* Schreibstelle im
   Schleifenrumpf (2670) klemmt selbst auf 0, sobald `tblink >= 64`. Bei einem Ausstieg nach
   Frame 64 verlaesst man die Schleife mit **0**, und dann ueberlebt nichts Sichtbares.
   Mein Lauf B (AF=80) ist genau dieser Fall und ist unauffaellig. Die Aussage „niemand loescht
   ausserhalb" bleibt trotzdem wahr — der Effekt haengt nur an der Ausstiegs-Framenummer.
3. Die Messung nutzt (wie die Untersuchung) den Debug-Auto-Advance `RE15_TITLE_SHOT_AF`, nicht
   einen echten Tastendruck. Das ist fuer D2 unschaedlich, weil **beide** Pfade dieselbe Zeile
   2670 als letzte Schreibstelle durchlaufen (Klammertiefen-Nachweis 1d) — aber ein
   Nutzer-Enter-Lauf (gdigrab) ist der noch direktere Beleg fuer die Ursache-Behauptung D1/D3.

---

## 4. Artefakte
- Disassembly: `<scratchpad>/main_dis.txt` (objdump -d -r main.c.obj)
- Frames: `<scratchpad>/D2/A_000{150,200,300,400,500}.ppm`, `B_000{...}.ppm`, `tsA.bmp`, `tsB.bmp`
- Es wurde **keine** Repo-Datei geaendert, nichts committet. Alle Mess-Prozesse wieder beendet
  (PID 15012, 12316, 16180 gestoppt; die parallel laufenden `timeout.exe`-Kinder anderer
  Sitzungen wurden **nicht** angefasst).
