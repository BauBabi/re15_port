# D3 — GEGENPRUEFUNG (Rolle: FOLGEN-PRUEFER) zur Behauptung D1 "Dark-Start-Bug"

Datum 2026-08-31. Auftrag: nicht den Beleg nachlesen, sondern pruefen, was kaputtgeht,
wenn nach D1 gehandelt wird. Alles unten ist GEMESSEN (Screenshots + md5) oder aus dem
Decompilat zitiert; keine Schaetzung.

## 0. Ergebnis in einem Satz

**D1 haelt stand** (Mechanismus real, unabhaengig reproduziert), aber die **Empfehlung ist
in der Prioritaet vertauscht**: der als "MINIMAL/PRAEZISE" bezeichnete Teil (1) —
`if (re15_gameflow_mode() == RE15_MODE_TITLE)` um main.c:2668-2670 — ist auf **genau dem
Pfad, den die vorgeschlagene Verifikation misst (RE15_TITLE_SHOT), beweisbar ein NO-OP**
(Ausgabe **md5-identisch** zum ungepatchten Build). Erst der als "ZUSAETZLICH" gefuehrte
Teil (2) macht den Sweep gruen.

Einen Raum/Bildschirm/Test, in dem die Empfehlung etwas FALSCHES tut, habe ich **nicht**
gefunden — 5 Raeume und 1 Integrations-Pin gegengemessen, alle sauber.

## 1. Die Fade-Engine: welcher KANAL ist betroffen

`FUN_800217b0(param_1, step, mask, bucket)` (RE_15_Quellcode_V2/FUN_800217b0.c:6-9):

    iVar1 = (param_1 & 0xff) * 0x44;
    (&DAT_800b545a)[(param_1 & 0xff) * 0x22] = param_2;     // step
    (&DAT_800b545c)[iVar1] = (char)(param_1 >> 8);          // ABR-Modus

=> **KANAL = `param_1 & 0xff`**, **ABR = `param_1 >> 8`**.
Damit sind ALLE vier Title-Fades **Kanal 0**:

| Aufruf | Adresse | Kanal | ABR | Port-Variable |
|---|---|---|---|---|
| Title-Fade-in `(0x200,-0x400,7,3)` | @0x80102054-64 (TITLE.BIN) | **0** | 2 sub | `s_tfade_sub` |
| Confirm A `(0x100,-0x800,7,0)` | @0x80102cd4-e4 | **0** | 1 add | `s_tfade_add` |
| Confirm B `(0x200,+0xe0,7,1)` | @0x80102d28-4c | **0** | 2 sub | `s_tfade_sub` |
| Confirm C `(0x200,0,7,1)` | @0x80102d78-9c | **0** | 2 sub | `s_tfade_sub` |

`FUN_80021764(n)` (FUN_80021764.c:3): `(&DAT_800b5458)[n * 0x22] = 0xffff;` — setzt den
Pegel auf 0xffff (Vorzeichenbit); `FUN_80021880` ueberspringt den Kanal dann
(`if (-1 < (int)((uint)*puVar5 << 0x10))`, FUN_80021880.c:14) => **KILL**.
`FUN_80021880` laeuft ueber **4 Kanaele** (`iVar7 = 4; ... puVar5 += 0x22`).

`FUN_800161e0` (Game-Init) ruft **EINMAL** `FUN_80021764(0)` @0x80016420 + `FUN_80021634(2,0)`
@0x8001642c (FUN_800161e0.c:61-62).

=> **Die Zitat-Lage von Empfehlung (2) stimmt inhaltlich** (Kanal 0 traegt beide
Port-Variablen), **aber die STELLE nicht**: main.c:4672 liegt in `while (running)` (3639)
unter `if (md1_ok)` (4214) und laeuft damit **JEDEN Gameplay-Frame**; das Original killt
Kanal 0 **einmal beim Modul-Wechsel**.

### Wer benutzt Kanal 0 sonst noch (= wovon ein Per-Frame-Kill die Finger lassen muss)

    FUN_800264e8.c:10    FUN_800217b0(0x200,0xffffe800,7,0)   Kanal 0, sub, step -0x1800
    FUN_80026594.c:6/11  FUN_800217b0(0x200,0x1800,7,0) / (0x200,0,7,0)   (Karten-Screen)
    FUN_8002dfb0.c:113   FUN_800217b0(0x200,0xffffe800,7,3)   (CONFIG-Task @0x8002dde4)

Der Port bildet **dieselbe Original-Kanal-0** an ZWEI Stellen ab:

* `g_fade_ch[0]` (engine/src/fade_common.c) — gepinnt in
  `tests/unit/test_room_transition_cmd.c:81-85` (level 0x7FFF, step -0x1800, abr 2),
  `test_scd_flag_fade.c:89-96`, `test_fade_step0_hold.c:169-174`;
* `s_tfade_add` / `s_tfade_sub` (platform/pc/src/render_pc.c:93-94, gezeichnet 1190-1205).

Heute ueberlappen die beiden zeitlich nie (grep: `re15_render_pc_title_fade_*` hat
**ausserhalb** der Title-Schleife main.c:2587-2670 KEINEN Aufrufer). Ein Per-Frame-
"Kanal-0-Kill" im Gameplay ist deshalb **heute folgenlos**, zementiert aber ein byte-falsches
Modell: sobald jemand die beiden Repraesentationen zusammenlegt, frisst der Per-Frame-Kill
den Raum-Uebergangs-Fade (FUN_800264e8, Kanal 0).

## 2. Die Title-Schleife hat ZWEI Ausgaenge — der Guard sieht nur einen

main.c (Zeilen exakt):

    2664-2667  Kommentar Title-Fade-in
    2668-2670  s_tfade_sub = 255 - (tblink>>1)*8            <-- der Block aus D1
    2671       re15_render_end_frame();
    2673       re15_render_pc_hide_title_menu();
    2674-2675  if (t_shot && tblink == t_af) { screenshot; re15_gameflow_new_game(0); }
    2676       tblink++;
    2677       }   <- Schleifenende (Bedingung 2540)

* **Ausgang A (interaktiv):** confirm-Block ab 2565; der Modus wird INGAME bei 2624
  (`re15_gameflow_new_game`) bzw. 2647 (`g_gameflow.mode = RE15_MODE_INGAME`) — also
  **VOR** 2668. Hier greift der Guard.
* **Ausgang B (Harness):** `re15_gameflow_new_game(0)` steht in **2675**, also **HINTER**
  2668. In dieser Iteration ist `re15_gameflow_mode()` bei 2668 noch TITLE =>
  **der Guard ist wirkungslos**, `s_tfade_sub` bleibt auf `255-(t_af>>1)*8` stehen.

D1s Beleg-Satz "Der Block liegt HINTER dem gesamten if(confirm)-Block ab 2565" beschreibt
nur Ausgang A. Ausgang B ist der Pfad, den **die Verifikation von D1 selbst** benutzt und
den **drei ctest-Integrations-Pins** benutzen:
`tests/integration/test_boot_bg_pin.cmake` (`RE15_TITLE_SHOT=title.bmp`, ohne AF =>
Default `t_af = 22`, main.c:2674 => B = 167), `test_relatch_pin.cmake`,
`test_save_counter_pin.cmake`.

## 3. Messungen (gebauter Stand, 960x720, `max`/`mean` ueber alle RGB-Kanaele)

Sonde: `<scratchpad>/maxbmp.py`. Patch-Kopien + Exen unter `<scratchpad>/patched/`
(`main_fix1.c` = Empfehlung (1); `main_fix12.c` = (1)+(2); `main_altfix.c` = Vorschlag §5).
Gebaut **ohne** das Repo anzufassen: main.c-Kopie einzeln mit den ninja-Flags kompiliert
und gegen die vorhandenen `.obj` + `libre15_engine.a` + `libSDL2.a` neu gelinkt.

### 3.1 Der Fade-in selbst ist byte-true (Kontrollmessung, Title-Frame)

`max(Title-Shot) = 255 - B` mit `B = 255 - (AF>>1)*8` (32 Ticks a -8, @0x80102054-64):

| AF | tk=AF>>1 | B | erwartet | **gemessen** |
|---|---|---|---|---|
| 4  | 2  | 239 | 16  | **16**  |
| 22 | 11 | 167 | 88  | **88**  |
| 40 | 20 | 95  | 160 | **160** |
| 62 | 31 | 7   | 248 | **248** |
| 64 | 32 | 0   | 255 | **255** |
| 80 | 40 | 0   | 255 | **255** |

(Szenen-Obergrenze hier 255, nicht die 214 aus `D_dark_start.md` §111 — dort wurde der
IN-GAME-Frame bei t=20 s gemessen, hier zusaetzlich der Title-Frame. Kein Widerspruch.
Software- und beschleunigter Renderer liefern beide 255.)

### 3.2 Der Leak in den Gameplay-Frame (ROOM1240, Serien-Shot Frame 210)

| Build | AF=22 Title | AF=22 **In-Game F210** | AF=80 In-Game F210 |
|---|---|---|---|
| unveraendert        | 88 | **49** / mean 0.29 | 216 / 32.49 |
| **Empfehlung (1)**  | 88 | **49** / mean 0.29 | 216 / 32.49 |
| **(1)+(2)**         | 88 | **216** / 32.49    | 216 / 32.49 |
| **Alt-Fix (§5)**    | 88 | **216** / 32.49    | 216 / 32.49 |

`216 - 167 = 49` — der Abzug ist exakt `B(AF=22) = 167`.

**md5 (Beweis, dass (1) auf diesem Pfad NICHTS tut):**

    97cbe8663f4f832eba3e2f464568d7e0  run22/shots/series_f00210.bmp   (unveraendert)
    97cbe8663f4f832eba3e2f464568d7e0  fix1_22/shots/series_f00210.bmp (Empfehlung (1))
    cd9fe4eaf2345869adc000f279c33927  run80/shots/series_f00210.bmp
    cd9fe4eaf2345869adc000f279c33927  fix1_80/shots/series_f00210.bmp
    248c96b2da750975f3a7a1bac077f9b3  run22/t.bmp
    248c96b2da750975f3a7a1bac077f9b3  fix1_22/t.bmp

=> Wer nur (1) einbaut und dann den in D1 verlangten Sweep wiederholt, bekommt **exakt
dieselben Zahlen wie heute** und koennte daraus faelschlich schliessen, D1 sei widerlegt.

### 3.3 Empfehlung (1) repariert den INTERAKTIVEN Pfad — belegt

LOAD GAME ueber `RE15_CONTINUE_TEST` (Confirm bei `tblink == 16` => tk=8 => **B = 191**),
Karte vorher per `RE15_SAVE_TEST` / `RE15_CARD_AUTO` erzeugt, Serien-Shots Frame 210/270:

| Build | max | mean |
|---|---|---|
| unveraendert   | **25**  | 0.08  |
| Empfehlung (1) | **216** | 32.49 |

`216 - 191 = 25` — exakt. Also: (1) ist **richtig und wirksam fuer Ausgang A**, nur eben
nicht fuer Ausgang B.

### 3.4 Drei weitere Raeume (aktive Suche nach einem Gegenbeispiel) — keins gefunden

`RE15_DEBUG_JUMP="<raum>@60"`, Serien-Shot Frame 240, AF=22:

| Raum | unveraendert | (1)+(2) | Differenz |
|---|---|---|---|
| **ROOM1140** (Briefing/Zombies) | max 88 / mean 0.15 | max **255** / 43.02 | 167 |
| **ROOM1170** (Helipad)          | max 49 / mean 0.17 | max **216** / 3.08  | 167 |
| **ROOM10C0** (Kraehen)          | max 88 / mean 0.25 | max **255** / 40.63 | 167 |

In jedem Raum exakt derselbe Abzug 167 — der Defekt ist **global** (der Quad liegt in
`re15_render_pc_end_frame`, render_pc.c:1197-1205, ungegatet), und der Fix stellt in jedem
Raum den sauberen Referenzwert her. **Kein Raum, in dem die Empfehlung das Falsche tut.**

### 3.5 Tests / Pins

* `grep -rn "title_fade|tfade" re15_port/tests/` => **0 Treffer**. Kein Unit-Test und kein
  Pin behauptet etwas ueber die Title-Fade-Ebene => **kein Test faellt**, weder durch (1)
  noch durch (2). Das ist zugleich die Luecke: die Aenderung ist von ctest ungedeckt.
* `test_boot_bg_pin.cmake` gegen den gepatchten Alt-Fix-Build ausgefuehrt:
  `boot_bg_pin OK: '[bg-log] F0 blit room1240#00 (room=1240)' ...` — **gruen**, wie mit dem
  unveraenderten Build (beide Laeufe gemessen).
* Die drei RE15_TITLE_SHOT-Pins laufen heute (Default `t_af=22`) auf einem um 167/255
  abgedunkelten Bild. Sie pruefen Logzeilen, keine Pixel — sie schlagen deshalb nicht an,
  aber jede kuenftige Pixel-Pruefung auf diesem Harness misst Muell, solange Ausgang B
  nicht geschlossen ist.

## 4. Was NICHT kaputtgeht (geprueft)

* **In-Game-Kartenschirm** (`pc_run_memcard_screen`, Aufruf main.c:3696 aus der
  Gameplay-Schleife): benutzt `s_fade_alpha` (`re15_render_pc_set_fade`), nicht die
  Title-Ebene. Der 255-Halt beim Exit (@0x800265ec) wird weiterhin vom bestehenden
  `set_fade(0)` @main.c:4674 geloest. (2) fasst ihn nicht an.
* **GAME OVER / YOU DIED** (main.c:5341-5353): `set_fade` / `set_white_fade` /
  `set_black_bg` — ebenfalls nicht die Title-Ebene.
* **Tod -> Titel** (`goto re_title`, main.c:8279 -> Label 2465): `tblink` wird bei 2482 neu
  auf 0 gesetzt, der Fade-in laeuft korrekt erneut; der Guard aus (1) ist dort deckungs-
  gleich mit der Schleifenbedingung 2540 und kann den Fade-in nicht abwuergen.
* **LOAD-Abbruch (2652) und OPTION-Rueckkehr (2661)**: `tblink = 0`, Modus bleibt TITLE =>
  Guard laesst den Fade-in laufen. Korrekt.
* **CONFIG/OPTIONS**: das Original hat dort einen EIGENEN Kanal-0-Fade-in
  (`FUN_8002dfb0.c:113`, `(0x200,-0x1800,7,3)`); der Port setzt bei main.c:2659 nur
  `title_fade_sub(0)`. Das ist eine **bestehende, von D1 unabhaengige** Divergenz — weder
  (1) noch (2) machen sie schlimmer; wer sie spaeter byte-true nachruestet, landet in
  `s_tfade_sub` und muss dann wissen, dass (2) diese Variable im Gameplay-Loop pro Frame
  nullt.

## 5. Empfohlene Aenderung an der Empfehlung

Statt "(1) minimal + (2) zusaetzlich" **eine** Stelle, die beide Ausgaenge erschlaegt und
naeher am Original ist (EIN-Schuss statt Per-Frame):

    main.c:2678   re15_render_pc_hide_title();
    + { extern void re15_render_pc_title_fade_sub(int b);
    +   extern void re15_render_pc_title_fade_add(int b);
    +   re15_render_pc_title_fade_sub(0); re15_render_pc_title_fade_add(0); }
        /* Port-Gegenstueck zum EINMALIGEN Kanal-0-Kill des Game-Init:
           FUN_800161e0 `jal FUN_80021764` @0x80016420 (a0=0 im Delay-Slot @0x80016424)
           + FUN_80021634(2,0) @0x8001642c/@0x80016430. Kanal 0 traegt BEIDE Title-Ebenen
           (FUN_800217b0: Kanal = param_1 & 0xff; 0x100/0x200 => 0). */

Zwischen 2678 und dem Beginn der Gameplay-Schleife (3639, erstes `re15_render_begin_frame`
3731) wird **kein** Frame gerendert (geprueft: keine begin/end_frame-Aufrufe in 2680..3730),
die Stelle ist also lueckenlos.

**Gemessen (Alt-Fix, AF-Sweep, In-Game F210):**

| AF | 4 | 22 | 40 | 62 | 80 |
|---|---|---|---|---|---|
| In-Game max | **216** | **216** | **216** | **216** | **216** |
| Title-Shot max (Fade-in bleibt byte-true) | 16 | 88 | 160 | 248 | 255 |

Das ist das richtige Abnahmekriterium: **der In-Game-Frame muss AF-INVARIANT sein**
(nicht "der Title-Shot muss hell sein" — der DARF bei kleinem AF dunkel sein, das ist der
Fade-in @0x80102054-64).

`test_boot_bg_pin` mit diesem Build: **gruen**.

## 6. Praktische Falle beim Nachmessen

Vier Laeufe endeten mit rc=124 (Timeout) und leerem Title-Shot, weil ein **fremder,
haengender Prozess `re15_pc_fix2.exe` (PID 9108)** aus einer anderen Sitzung lief. Nach
dessen Ende liefen dieselben Kommandos sofort durch. Vor jeder Messreihe
`tasklist | grep -i re15` pruefen — sonst liest man ein Werkzeug-Artefakt als Befund.
