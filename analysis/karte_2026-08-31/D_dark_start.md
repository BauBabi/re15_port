# D — "Drueckt man am Start zu schnell Enter, bleibt das ganze Spiel dunkel"

Status: **GEFUNDEN, reproduziert, byte-genau vermessen.** Eine einzige Ursache, eine Codestelle.
Datum 2026-08-31. Gemessen auf dem Dev-Build (`re15_port/build/platform/pc/re15_pc.exe`,
sha256 fa8ec7a5d7a37101c9da035c615571627ff40a45327b9a0d079782ea43c4e6ca) UND auf dem
**gepackten Stand des Nutzers** (`release/pkg-win/re15_port_v0.3.70/re15_pc.exe`,
sha256 51f95095de446ae0bbe0362e7f723d473610c3bd1cfa1a6fa278deb0fc8a4b50).

---

## 1. Die Ursache

`re15_port/platform/pc/main.c:2664-2670` — der **Title-Fade-in** am Ende des
Titel-Schleifenkoerpers:

    /* Title-FADE-IN nach Boot/FMV: ... @0x80102054-64 = SUBTRAKTIV, B = 255 -> 0 mit -8/Tick, 32 Ticks */
    { extern void re15_render_pc_title_fade_sub(int b);
      int tk = (int)(tblink >> 1); int B = 255 - tk * 8; if (B < 0) B = 0;
      re15_render_pc_title_fade_sub(B); }
    re15_render_end_frame();
    ...
    tblink++;

Dieser Block laeuft **bedingungslos in JEDER Iteration** — auch in genau der Iteration, in der
der Nutzer bestaetigt hat und die Titel-Schleife danach verlassen wird.

Ablauf im Schleifenkoerper (`main.c:2551-2676`):

| Zeile | Was |
|---|---|
| 2562-2564 | `confirm = pp & (CROSS\|SQUARE\|TRIANGLE\|CIRCLE\|START)` — **Enter/START ist ein Confirm** (Original-Maske 0x8f0 @0x800bc762) |
| 2586-2614 | blockierender Confirm-Fade (Phase A Weissblitz, Phase B Fade-to-black), Ende: `title_fade_sub(255)` |
| 2617-2635 | `cursor==0` NEW GAME: `title_fade_sub(0)` -> `pc_run_player_select()` -> `re15_gameflow_new_game(ch)` ⇒ **mode = INGAME** |
| **2668-2670** | **danach trotzdem noch:** `title_fade_sub(255 - (tblink>>1)*8)` |
| 2676 | `tblink++` — wird waehrend des Confirm-Blocks NICHT hochgezaehlt |
| 2551 | Schleifenbedingung `mode == RE15_MODE_TITLE` jetzt falsch ⇒ **Ausstieg mit dem Restwert** |

`tblink` ist zu diesem Zeitpunkt **exakt die Titel-Frame-Nummer, in der gedrueckt wurde**.
Wer frueh drueckt, verlaesst die Titel-Schleife mit `s_tfade_sub > 0`.

### Und niemand loescht es wieder

Vollstaendiger Xref (`grep -rn "title_fade_sub|s_tfade_sub" re15_port/`): alle 10
Schreib-Stellen liegen **innerhalb** der Titel-Schleife (main.c:2588 … 2670).
Ausserhalb: **keine einzige.**

`s_tfade_sub` (`platform/pc/src/render_pc.c:94`) ist ein Datei-Static und wird in
`re15_render_end_frame` **in jedem Frame des restlichen Spiels** als
**ABR2-Subtraktiv-Vollbildquad** gezeichnet (`render_pc.c:1197-1205`,
`SDL_ComposeCustomBlendMode(ONE, ONE, REV_SUBTRACT, ZERO, ONE, ADD)`, Rect 0,0,320,240):

    if (s_tfade_sub > 0) { ... dst - src ueber das ganze Bild ... }

⇒ **jedes Pixel des gesamten weiteren Spiels wird dauerhaft um `B` abgedunkelt.**
Bei `B >= ~214` ist der Bildschirm schwarz.

### Warum die anderen Frontend-Fades das NICHT tun

- `s_fade_alpha` (Card-Save/-Load haelt beim Exit 255, `main.c:1376`) wird in der Spielschleife
  **jeden Frame** genullt: `main.c:4672-4675` `re15_render_pc_set_fade(0);`
- `s_title_fade` wird an jedem Frontend-Ausgang genullt: `main.c:1842` (Player-Select),
  `main.c:2320` (Config).
- `s_tfade_add` loescht sich selbst am Ende von Phase A: `main.c:2611`.
- **Nur `s_tfade_sub` hat keinen Release.**

Der Kommentar in `main.c:4664-4672` benennt den Original-Mechanismus bereits korrekt — er wurde
nur auf `s_fade_alpha` angewandt, nicht auf die Title-Ebenen.

---

## 2. Original-Beleg (was das Original an dieser Stelle tut)

Der Game-Init `FUN_800161e0` **killt den Frontend-Fade-Kanal 0 bedingungslos** und loescht danach
auf Schwarz — genau die Operation, die dem Port fuer die Title-Ebene fehlt.

`ghidra1_V2.txt:82579-82582`:

    80016420  d9 85 00 0c   jal    FUN_80021764      ; fade_kill(ch)
    80016424  21 20 00 00   _clear a0                ; Delay-Slot: a0 = 0  -> KANAL 0
    80016428  02 00 04 34   ori    a0,zero,0x2
    8001642c  8d 85 00 0c   jal    FUN_80021634      ; Schwarz-Clear
    80016430  21 28 00 00   _clear a1                ; Delay-Slot: a1 = 0

Decompilat `RE_15_Quellcode_V2/FUN_800161e0.c:61-62`:

    FUN_80021764(0);
    FUN_80021634(2,0);

`FUN_80021764` = Fade-Kill (`level = 0xffff`, Bit15 = idle ⇒ der Kanal wird nicht mehr gezeichnet;
Port-Gegenstueck `re15_fade_kill`, `engine/src/fade_common.c:64-71`, verifiziert @0x80021780).

Die Fade-in-RATE im Port (`FUN_800217b0(0x200,-0x400,7,3)` @0x80102054-64: B 255 -> 0, -8/Tick,
32 Ticks a 2 Vsyncs = 64 Frames) ist korrekt. **Die Rate ist nicht der Fehler — der fehlende Kill
beim Verlassen ist es.**

---

## 3. Messung — byte-genaue Bestaetigung

Harness: echtes Fenster, `ffmpeg -f gdigrab -i "title=RE1.5 Rebuilt — PC"`, plain exe,
**kein** `RE15_SOFTWARE_RENDER`, **kein** `RE15_AUTOSHOT` (Skill `re15-port-visual-verify`).
Skripte: `<scratchpad>/D/run.sh` (Dev-Build), `<scratchpad>/D/runpkg.sh` (Paket).
Helligkeit = Mittelwert/Max ueber alle Pixel des Capture-PNG (Graustufen, 960x720).

### 3a. Vorhersage vs. Messung — der Restwert ist EXAKT `B = 255 - (tblink>>1)*8`

Lauf: `RE15_NO_INTRO=1 RE15_TITLE_SHOT=ts.bmp RE15_TITLE_SHOT_AF=<tblink>` — die
Auto-Advance-Zeile `main.c:2673-2674` verlaesst die Titel-Schleife bei `tblink == AF` und laeuft
davor durch **dieselbe** Zeile 2670.

| AF (= tblink) | tk = AF>>1 | B (Vorhersage) | erwartet max = 214 - B | **gemessen max** | mean @t=20s |
|---|---|---|---|---|---|
| 4  | 2  | 239 | 0 (geklemmt) | **0**   | 0.00 |
| 40 | 20 | 95  | **119**      | **119** | 12.51 |
| 62 | 31 | 7   | **207**      | **207** | 68.11 |
| 63 | 31 | 7   | **207**      | **207** | 68.11 |
| 64 | 32 | 0   | **214**      | **214** | 73.77 |
| 80 | 40 | 0   | **214**      | **214** | 75.04 |

Vier exakte Zahlen-Treffer. Das schliesst zugleich einen Alpha-Blend aus: ein Blend haette bei
B=95 den Wert 214*(1-95/255) = **134** ergeben, gemessen wurden **119** = 214-95.
⇒ Es ist der **subtraktive** ABR2-Quad aus `render_pc.c:1197-1205`, geschrieben ausschliesslich
von `main.c:2670`.

Rohmessung AF=4 (Dev-Build), Capture bei 10/20/30/45/60 s: mean 0.00 / 0.00 / 0.00 / 0.00 / 0.02,
max 0 / 0 / 0 / 0 / 16 — also ueber die ganze Laufzeit schwarz, es "heilt" nie.
Kontrolle AF=80: mean 33.49 / 75.04 / 43.46 / 2.01 / 46.86, max bis 255.

### 3b. Der echte Nutzer-Pfad (START am Titel -> Player-Select -> Spiel)

Script `RE15_INPUT_SCRIPT="S0.2,W11,B6"`, `RE15_NO_INTRO=1`
(S = START, W = warten ueber den 326-Tick-Confirm-Fade, B = CROSS-Mash fuer den Player-Select):

| RE15_INPUT_SCRIPT_START | t=25s | t=35s | t=50s | t=65s |
|---|---|---|---|---|
| **6** (frueh) | mean 0.00 / max 0 | 0.00 / 0 | 0.00 / 0 | 0.00 / 0 |
| **80** (spaet) | 33.49 / 214 | 75.43 / 152 | 16.56 / 214 | 0.60 / 214 |

### 3c. Mit Boot-FMV (Konfiguration des Nutzers, ohne RE15_NO_INTRO)

| Script (START=0) | t=25 / 40 / 60 s |
|---|---|
| `S0.1,W0.2,S0.1,W11,B6` (Enter skippt Film, Enter sofort nochmal) | **0.00 / 0.00 / 0.00 — schwarz** |
| `S0.1,W2,S0.1,W11,B6` (2 s Skript-Pause) | 28.55 / 11.47 / 1.87, max **207** ⇒ B=7, immer noch leicht abgedunkelt |

### 3d. Auf dem GEPACKTEN Stand v0.3.70 (das, was der Nutzer startet)

`cd release/pkg-win/re15_port_v0.3.70`

| Script (START=0) | t=30 | t=45 | t=60 |
|---|---|---|---|
| `S0.1,W0.2,S0.1,W11,B6` | mean 0.00 / max 0 | 0.00 / 15 | 0.00 / 0 |
| `S0.1,W6,S0.1,W11,B6`   | 71.09 / 214 | 42.18 / 237 | 2.01 / 214 |

### 3e. Wie gross ist das Zeitfenster?

Die Titel-Schleife laeuft mit `SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC`
(`render_pc.c:481-485`), also mit der Bildwiederholrate des Monitors.
Gemessen auf dieser Maschine (Zeit vom Start bis die Zeile `[fps] target=` in `debug.log` steht —
diese Zeile wird erst NACH der Titel-Schleife geschrieben, `main.c` `[fps]`-fprintf;
`RE15_TITLE_SHOT_AF` = 4 / 304 / 604 ⇒ 1.006 s / 2.900 s / 4.979 s):
**300 Titel-Frames in 2.079 s = 144.3 fps.**

⇒ Gefahrenfenster = die ersten **64 Titel-Frames**:
- **~0.44 s** bei 144 Hz (diese Maschine),
- **~1.07 s** bei 60 Hz.

Der Abdunkelungsgrad ist ein Kontinuum: je frueher gedrueckt, desto dunkler (0 Frames = B 255 =
pechschwarz, 62 Frames = B 7 = kaum sichtbar, ab 64 Frames = B 0 = sauber). Genau deshalb sieht
der Nutzer den Fehler "oefter", aber nicht immer.

---

## 4. Betroffene Wege (alle derselbe Root)

Zeile 2668-2670 liegt hinter dem gesamten `if (confirm)`-Block, deshalb trifft es alle Ausstiege:

- **NEW GAME** (`cursor==0`, main.c:2617) — der gemeldete Fall.
- **LOAD GAME mit Erfolg** (`cursor==1`, main.c:2636-2650 setzt `mode = INGAME`) — gleiches Bild.
- **`RE15_TITLE_SHOT`-Auto-Advance** (main.c:2673-2674, Default `AF=22` ⇒ Restwert **B=167**):
  jeder Screenshot-Harness-Lauf faehrt seither mit abgedunkeltem Spiel. Latent, aber real —
  das verfaelscht rueckwirkend Helligkeits-Vergleiche, die ueber diesen Harness gemacht wurden.

LOAD-GAME-Abbruch und OPTION setzen `tblink = 0` und bleiben in der Schleife ⇒ unauffaellig.

Ausgeloest wird es von **jedem** Confirm-Knopf, nicht nur Enter: Maske `main.c:2562-2564`
= CROSS | SQUARE | TRIANGLE | CIRCLE | START.

---

## 5. Ausgeschlossene Verdaechtige (geprueft, nicht Ursache)

| Kandidat | Warum nicht |
|---|---|
| `s_fade_alpha` (Card-Exit haelt 255, `main.c:1376`) | wird in der Spielschleife **jeden Frame** genullt, `main.c:4673-4674` |
| `s_title_fade` | an jedem Frontend-Ausgang 0 gesetzt: `main.c:1842`, `main.c:2320` |
| `s_tfade_add` | Phase A endet mit `title_fade_add(0)`, `main.c:2611` |
| Boot-FMV-Skip vor Wiedergabestart | `re15_render_pc_hide_fmv()` laeuft unbedingt nach der Film-Schleife (`main.c:2456`); scheitert `re15_str_open`, wurde `s_fmv_show` nie gesetzt (`render_pc.c:1654-1672`) |
| `s_request`-Sticky in `re15_menu_start_poll` (`menu_common.c:126-139`) | oeffnet nur das Inventar, `return` wenn `s_stage != 0`; keine Render-Wirkung |
| `g_fade_ch[]` (SCD-Kanaele) | der Titel-Pfad konfiguriert sie nicht; `re15_fade_init` killt alle beim Boot (`fade_common.c:118-126`) |
| `s_scene_black` / `s_black_bg` | raum-/FSM-gebunden, kein Titel-Schreiber |

---

## 6. Repro-Kommandos (nachfahrbar)

**Schnellster A/B (Dev-Build, ~25 s je Lauf):**

    cd /c/workspace/git/reAi_v2/re15_port/build/platform/pc
    RE15_NO_INTRO=1 RE15_TITLE_SHOT=ts.bmp RE15_TITLE_SHOT_AF=4  ./re15_pc.exe   # -> Spiel bleibt schwarz
    RE15_NO_INTRO=1 RE15_TITLE_SHOT=ts.bmp RE15_TITLE_SHOT_AF=64 ./re15_pc.exe   # -> normal hell

**Echter Nutzer-Pfad auf dem Paket:**

    cd /c/workspace/git/reAi_v2/release/pkg-win/re15_port_v0.3.70
    RE15_INPUT_SCRIPT="S0.1,W0.2,S0.1,W11,B6" RE15_INPUT_SCRIPT_START=0 ./re15_pc.exe   # schwarz
    RE15_INPUT_SCRIPT="S0.1,W6,S0.1,W11,B6"   RE15_INPUT_SCRIPT_START=0 ./re15_pc.exe   # normal

**Von Hand (das, was der Nutzer tut):** exe starten -> Enter (Film ueberspringen) -> sofort
nochmal Enter (innerhalb ~0.5 s nachdem der Titel erscheint) -> Charakter bestaetigen.

**Messung:** `<scratchpad>/D/run.sh <tag> <sek,sek,...> <ENV=...>` bzw. `runpkg.sh <tag> ...`;
Bilder landen unter `<scratchpad>/D/<tag>/t<sek>.png`.

---

## 7. Empfohlene Aenderung (NICHT ausgefuehrt — dies ist ein Beleg-Bericht)

Byte-true-Vorbild: `FUN_800161e0` killt beim Game-Init den Frontend-Fade-Kanal 0
(`jal FUN_80021764` @0x80016420, `a0 = 0` im Delay-Slot @0x80016424) und loescht auf Schwarz
(`FUN_80021634(2,0)`, @0x8001642c / @0x80016430).

Zwei Stellen, beide sinnvoll:

1. **`platform/pc/main.c:2668-2670`** — den Fade-in-Block nur ausfuehren, solange die
   Titel-Schleife auch weiterlaeuft, z.B. umschliessen mit
   `if (re15_gameflow_mode() == RE15_MODE_TITLE) { ... }`.
   Das ist die minimale, praezise Korrektur (der Fade-in gehoert zum Titel, nicht zum Ausstieg).

2. **`platform/pc/main.c:4672-4675`** — den vorhandenen Frontend-Release (dort bereits als
   Port-Gegenstueck von @0x80016420 kommentiert) um die Title-Ebenen erweitern:
   `re15_render_pc_title_fade_sub(0); re15_render_pc_title_fade_add(0);`
   neben dem bestehenden `re15_render_pc_set_fade(0);`.

Verifikation nach dem Fix: Tabelle 3a erneut fahren — **alle** AF-Werte (4 … 80) muessen
`max == 214` liefern; danach 3d auf dem neu gepackten Stand wiederholen.
