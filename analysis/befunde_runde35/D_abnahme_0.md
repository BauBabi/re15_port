# Runde 35 Spur D "redhawk" — Abnahme 0 (unabhaengig)

Abnahme am 2026-10-04 im Baum `.claude/worktrees/r35_redhawk`, Zweig `r35/redhawk`, HEAD `1408e296`
(Basis `154a73c1`, 12 Commits `1e04e7d5..1408e296`). Gemessen wurde am selbst gebauten Stand. Aus dem
Dossier `D_redhawk.md` habe ich nur die Behauptungen uebernommen und jede selbst nachgemessen.

**Ergebnis: BESTANDEN.** Der eine Nutzer-Punkt ist erfuellt, und alle Gates halten (Suite, @0x-Gate,
Pfad- und Vertrags-Gate, Tests). Es gibt keine Maengel, nur Hinweise fuer die Integration (unten H1..H5).

---

## 0. Bau und Suite

| Schritt | Befehl | Ergebnis |
|---|---|---|
| configure | `bash re15_port/tools/local_build.sh configure` | `=== LOCAL-BUILD-OK (configure)` |
| build | `bash re15_port/tools/local_build.sh build` | `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`; die exe entspricht HEAD |
| Suite | `bash re15_port/tools/local_build.sh test` (eigener Lauf, 1145 s) | `100% tests passed, 0 tests failed out of 480` / **`=== LOCAL-BUILD-OK (test) — Tests 480/480`** |
| eigene Tests | `ctest --test-dir re15_port/build -R "r35_redhawk" -V` | `unit_r35_redhawk Passed` (`GRUEN (0 Fehler, erster 0)`), `integration_r35_redhawk Passed` (`Redhawk-Tod Bild 112, fx max 59, fx = 0 ab Bild 164 (<= Tod + 90), Endbild 230 fx 0 - ok`) |

Die fuenf GUI-Haken (boot_bg, dark_start, relatch, save_counter, weste_load) liefen im Suite-Lauf beim
ersten Mal gruen, ein Nachfahren war nicht noetig. Die Dossier-Zeile `=== LOCAL-BUILD-OK (all) — Tests
480/480` ist damit durch einen eigenen Lauf bestaetigt.

---

## 1. Punkt 1 (AUFTRAG.md Z.17, woertlich)

> "Wenn ich mit der Super Redhawk auf die Hunde schieße bleiben die Fleisch Effekte die sich
> rauslösen permanent da in loop."

### 1.1 Messaufbau

* **NACHHER-exe** = `re15_port/build/platform/pc/re15_pc.exe` (HEAD 1408e296). Fuer jeden Lauf habe ich
  eine eigene Kopie unter dem Namen `re15_pc_abn0d_<id>.exe` neben der exe angelegt und danach geloescht.
  Ich habe nie `taskkill /IM` benutzt.
* **VORHER-exe**: Ich habe sie selbst aus dem Basisstand gebaut, mit
  `git archive 154a73c1 re15_port ':(exclude)re15_port/shared_assets'` in den Scratch und
  `cmake -G Ninja -DRE15_BUILD_PC=ON -DCMAKE_BUILD_TYPE=` (gleicher Bautyp wie der Baum). Der Baum blieb
  dabei unveraendert. Gestartet wird sie als Kopie neben der Baum-exe, damit die Asset-Wurzel gleich ist.
* Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_STATE_LOG=state.log`,
  dazu `RE15_DEBUG_JUMP=11D0@250 RE15_SET_FLAG=3:152 RE15_AI_FLAVOR=re2 RE15_GIVE=7:30 RE15_EQUIP=7
  RE15_PLAYER_POS=-7000,-15900,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=18
  RE15_INPUT_SCRIPT=M0.3,MA0.1,M0.6,MA0.1,...,M1,W40` (M = R1 Zielen, A = SQUARE Feuern;
  `main.c:5228`).
* Das Feld `fx=` in state.log zaehlt die lebenden ESP-Plaetze (`re15_esp_fx_count`, `re15_esp.c:312`).
  Das Feld `[n t=20 st=3 ss1=7 ...]` bedeutet: Hund n tot durch Waffe 7 (Super Redhawk).
* fx.log (`RE15_FX_LOG`, `main.c:379`) schreibt je gezeichnetem Partikel Id, Sub, Anim-Record (`frame`),
  Weltlage, Routinen A/B und Flags. Es zeichnet nur, was der aktive Cut zeigt. Deshalb setze ich
  `RE15_FORCE_CUT=6` (Zwinger-Cut). Das aendert nur die Kamera (`main.c:2988-2989`).
* Bilder kommen aus `RE15_FRAMEDUMP=120-250/5:fd_` (komponierter Software-Framebuffer). gdigrab liefert in
  dieser Sitzung weisse Bilder.
* Alle Laeufe, Logs und Bilder liegen unter
  `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn0_D_redhawk/`
  (`lauf_A..G`, `lauf_V`, `abn0d_lauf.sh`, `unit_pin.log`, `integ_vorher.log`, `d_test.log`). Sie sind
  nicht committet.

### 1.2 Messprotokoll

**Lauf V (VORHER, Basis 154a73c1).** state.log, Sitzung ROOM11D0:
```
F112 fx=23  [1 t=20 st=3 ss1=7 ...]        <- Hund 1 stirbt durch die Redhawk
F117 fx=59
F130 fx=6   ... und danach KEINE Aenderung mehr bis F250 (Endbild): fx=6
```
fx.log: Raum-Id 7 wurde in **139 Bildern gezeichnet, erstes Bild F112, letztes F250** (Laufende). Alle
**612** Id-7-Zeilen tragen **B=36**. Der Anim-Record laeuft 0..4 im Kreis. Beispiel aus dem Endbild F250:
`id=7 sub=1 frame=2 ... drift=(0,-6,0) ... A=0 B=36 fl=03`, `... frame=0 ... drift=(0,3,0) ... B=36`.
Die Geschwindigkeit `drift.y` zittert um die Port-Bodenklemme.
=> Der Nutzer-Befund ist am Basisstand reproduziert: 6 Fleisch-Brocken bleiben fuer immer im Flug-Zyklus.

**Lauf A/B/C (NACHHER, HEAD 1408e296), gleicher Lauf.** state.log:
```
F112 fx=23  [1 t=20 st=3 ss1=7 ...]        <- gleicher Redhawk-Tod (deterministisch gleich)
F117 fx=59
F130 fx=6   F144 fx=5   F153 fx=4   F158 fx=3   F160 fx=1   F164 fx=0  ... F250/F262 fx=0
```
fx.log (Lauf B), Raum-Id 7 je Bild, ausgezogen in `{sub frame B fl wpos}`:
```
F114: {sub=1 frame=1 B=36 fl=03 wpos=(-5754,-838,-15603)} ...           Flug, Zeile 0
F141: ... {sub=0 frame=9 B=0 fl=13 wpos=(-5071,85,-12602)} ... {sub=0 frame=4 B=37 fl=03 wpos=(-6084,122,-11352)}
F142: ... {sub=0 frame=6 B=0 fl=13 wpos=(-6084,122,-11352)}             R37: Flags 0x13, Anim 6, Zeile 1 (B=0)
F149: {sub=1 frame=6 B=0 fl=13 wpos=(-5712,12,-13903)} {sub=1 frame=7 B=0 fl=13 wpos=(-5707,110,-13945)} ...
F150: ... {sub=1 frame=2 B=37 fl=03 wpos=(-5755,41,-13592)} ...         Landung (B 36 -> 37)
F160..F163: {sub=1 frame=8|9 B=0 fl=13 wpos=(-5301,30,-13370)}          Aufschlag-Records 7..9
```
Id 7 ist in 52 Bildern gezeichnet, **letztes Bild F163**. Ab **F164 ist fx = 0** bis zum Laufende.
Jeder Brocken laeuft B 36 (Flug, Anim-Schleife 0..4) -> B 37 -> B 0 mit Flags 0x13 und Anim 6..9 und
verschwindet danach. Das ist genau die Kette aus Dossier R3/R4/R6.

**Lauf E (NACHHER, zwei Hunde mit der Redhawk getoetet).** Leon steht bei `RE15_PLAYER_POS=1500,-17700,2048`,
14x `MA0.1,M0.5`, Ende 450#11D0:
```
F88  [5 ... st=3 ss1=7]  fx=17 -> F90 fx=35 -> ... F146 fx=0         (Hund 5, Redhawk)
F160 [3 ... st=3 ss1=7]  fx=17 -> F163 fx=43 -> ... F215 fx=0         (Hund 3, Redhawk)
fx.log Id 7: F88..F140 (6 gleichzeitig), F160..F210 (6 gleichzeitig), danach kein Id-7-Bild mehr; F450 fx=0
```
Die kurzen fx-Spitzen dazwischen (F195/F235/F420) sind Blut an Leon (Hundebisse, Leon stirbt F265). Sie
erzeugen keine Id-7-Zeichnung und gehen ebenfalls auf 0.
=> Der Fix gilt auch fuer mehrere Hunde ("auf die Hunde", Plural).

**Lauf D (wie A, 16 Schuesse).** Der Verlauf deckt sich mit Lauf A (Redhawk-Tod F112, fx = 0 ab F168,
Leon tot F284), weil Leon nach den Bissen nicht mehr zum Schuss kommt. Nach Leons Tod faellt fx bei F305
wieder auf 0.

**Integrationshaken gegen die VORHER-exe** (Kopie `re15_pc_abn0d_vorherref.exe` neben der Baum-exe,
`cmake -DRE15_PC_EXE=... -DWORKDIR=... -P re15_port/tests/integration/test_r35_redhawk.cmake`): **exit 1**,
`r35_redhawk: NUTZER-BEFUND - bis 90 Bilder nach dem Redhawk-Tod (Bild 112) kein Bild mit fx = 0: die
Fleisch-Brocken laufen weiter`. Gegen die NACHHER-exe ist er gruen (s. 0.). Der Haken misst also den Befund.

**Bilder** (Scratch `abn0_D_redhawk/`, nicht committet):
* `abnahme_zoom_vorher_nachher.png`: Ausschnitt (150..290, 140..230) x3, links VORHER, rechts NACHHER, Bilder
  F145/F165/F185/F245. Links liegen ab F165 rote Brocken neben dem toten Hund und wechseln ihr Bild, rechts
  sind sie ab F165 weg. Pixel-Differenz (Schwelle 24): F145 0 px (beide fliegen), F185 31 px in
  (204..222, 184..193), F245 58 px in (200..220, 180..191).
* `lauf_C/abnahme_kontakt.png`: NACHHER F125/135/145/150/155/160/165/185/245. Die Brocken fliegen und
  landen am Hund. Ab F155 sind sie im Bild nicht mehr erkennbar (laut fx.log lebt der letzte Platz noch
  bis F163, in den Aufschlag-Records 8/9).

### 1.3 ROOM1190 (Luken-Hunde)

Ich habe es zweimal versucht (Lauf F: Sprung-Spawn (3050,-11650); Lauf G: Leon bei (690,-23000) vor der
Luke). Beide Male bleiben die drei Hunde 500 bzw. 450 Bilder lang (bis Laufende) bei `st=4 ss1=0 @(690,-28500)`. Das ist
der Skript-Pounce, der auf SCD grid 0x43 wartet (`enemy_ai_common.c:7505-7508`), und ich habe ihn mit
einem Sprung nicht ausgeloest.
Die Abdeckung fuer 1190 kommt deshalb aus diesen Quellen:
* Die Raum-ESP-Bank von ROOM1190 ist laut `probe_r35_redhawk zensus` Zeile fuer Zeile gleich der von
  ROOM11D0: nur Id 7, Records 0..4 Dauer 3, Record 5 `d0100 p20ff` = Schleife, Records 6..9, Record 10
  Terminator `d0000 p0000`, Subs 0..5 Zeile 0 `A=0 B=36 acc=(0,10,0) f0e=13 p16=6`.
* Der Spawnweg ist derselbe (`re2d_fx` -> `re15_esp_fx_spawn_rows(re15_esp_room_bank(), 7, ...)`,
  `enemy_ai_re2_dog.c:390`).
* Pin 3 von `unit_r35_redhawk` (eigener Lauf: `ok 30: 84 Raeume mit Id 7, 504 Brocken-Subs gespawnt`,
  `ok 31: ... Haenger 0 (0), Mechanik-Fehler 0 (0)`) spawnt jeden Brocken-Sub in jedem der 84 Raeume
  einschliesslich 1190 und taktet ihn bis zum Ende.

Ich werte das als ausreichend, weil Mechanik und Daten dieselben sind. Ein exe-Lauf durch die Luke steht
trotzdem aus (Hinweis H3).

### 1.4 Urteil Punkt 1: **erfuellt**

Gemessen gegen den Wortlaut: Nach einem Redhawk-Treffer auf einen Hund fliegen die Fleisch-Brocken
heraus, schlagen auf und sind im NACHHER-Lauf 52 Bilder (etwa 1,7 s) nach dem Tod vollstaendig weg
(fx = 0 ab F164). Im VORHER-Lauf blieben sie im Bild-Loop bis zum Laufende. Das gilt auch fuer einen
zweiten getoeteten Hund (Lauf E).

---

## 2. RE-Gate (Schritt 4)

### 2.1 Stichproben, selbst disassembliert

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, Binaerdatei `info/Re1.5/PSX.EXE`. Fuer RE2
`re2_disasm.py --bin EMD0G_MOD0.BIN`.

1. **Routine 36 @0x800187c4** (`dis 0x800187c4 60`):
   `800187d8 lh v0,40(v1)` / `800187e4 lh v0,42(v1)` / `800187f0 lh v0,44(v1)` (P = slot+0x28/2a/2c),
   `800187dc addu a1,zero,zero`, `800187e8 ori a2,zero,0x8`, `800187f4 ori a3,zero,0x100`,
   `800187f8 jal 0x8001c6e8`, `80018804/0c sll/sra v0,16`, `80018808 lw v1,20(sp)`, `80018810 slt v0,v0,v1`,
   `80018814 beq v0,zero,0x8001883c`, `80018818 ori v1,zero,0x25`, `80018828 sh v1,2(v0)`,
   `8001882c/30/38 sh zero,16/18/20(v0)`; Nein-Zweig `80018848 sh a0,30(v0)`.
   Das entspricht `esp_brocken.c` Zeile fuer Zeile: dieselben Argumente, s16-Vergleich h < Welt-y, B := 37,
   Geschwindigkeit := 0, sonst slot+0x1e := h.
2. **Routine 37 @0x8001885c**: `8001886c lh v1,42(a0)` / `80018870 lh v0,30(a0)` / `80018878 slt` /
   `8001887c beq`, `80018884 lbu v0,14(a0)` / `8001888c sb v0,108(a0)`, `8001889c lbu v0,22(v1)`,
   `800188a0 jal 0x800174e4`, `800188a4 sb v0,110(v1)` (Delay-Slot, laeuft vor dem Sprung). Das entspricht
   dem Code: Flags := row[0x0e], Anim := row[0x16] (aus Zeile 0, vor dem Vorschub), danach der Vorschub.
3. **Tabelle @0x80071d40** (`table 0x80071d40 48`): `[19] -> 0x80017d08`, `[36] -> 0x800187c4`,
   `[37] -> 0x8001885c`. Der B-Waehler sitzt bei `8001a2b4 lhu v0,2(v0)` / `8001a2c4 addiu at,at,7488`
   (=0x80071d40) / `8001a2d4 jalr v0`.
4. **FUN_800174e4** (Sprungziel des jal): `800174f0-fc lbu/addiu/sb 111(v1)` (Cursor++), danach
   Quelle = `*(slot+0x80) + Cursor*40` (`8001750c-24`: sll 2 + addu + sll 3) und eine Kopie nach slot+0.
   Das ist der Zeilen-Vorschub, wie behauptet. (Das Werkzeug beschriftet ihn falsch mit "lunge_finish".)
5. **Tick @0x8001a2a8-0x8001a47c** (fuer die PORT-WAHL und den Terminator): Die Physik `8001a2fc-388` macht
   nur euler += Winkelgeschwindigkeit, xlat (`lw/sw 52/56/60`) += vel (`lh 16/18/20`) und danach
   vel += acc (`lhu 8/10/12`). **Eine Bodenklemme gibt es dort nicht.**
   Anim: `8001a398 lbu v0,109(v1)` (Timer), `8001a3bc-c8` slot+0x6e++ (ausser Flags&0x40),
   `8001a3e8-400` rec[2]==0 && rec[0]==0 -> `8001a40c sb zero,108(a1)` (Flags := 0 = Platz frei),
   `8001a410-430` rec[2]==0xff -> slot+0x6e := rec[0] (Schleife).
6. **room_coll FUN_8001c6e8** (`RE_15_Quellcode_V2/FUN_8001c6e8.c`): Vom Punkt werden nur `*param_1` (x)
   und `param_1[2]` (z) gelesen, y nicht. Die Port-Signatur `re15_collision_room_coll(rdt, x, z, r, band,
   mask)` ist also vollstaendig. Ergebnis ist `((band+1) * -0x708)` bzw. die Objekt-Oberkante.
7. **RE2 Router 0x80104610** (`EMD0G_MOD0.BIN`): `8010464c jal 0x80105070` mit a1=3/a2=0,
   `8010465c` mit a1=2/a2=0, `80104664 jal 0x80015fe8` (rand), `80104674 andi v0,v0,0x1`,
   `80104678 jal 0x80105070` mit a1=2 / `8010467c addiu a2,v0,1`. Das ist FX(Teil 2, fx 1|2), wie behauptet.
   Die FX-Tabelle @0x801056AC (`bytes 0x801056AC 60`): Eintrag 1 = `09 00 00 02 00 08`, Eintrag 2 =
   `09 01 00 02 00 08`. Art 9 Sub 0/1 wird ueber den Port-Dekoder zur Raum-Id 7 (`enemy_ai_re2_dog.c:384`).
   In Lauf B sind tatsaechlich Sub 0 und Sub 1 zu sehen.

Alle zitierten Stellen enthalten, was im Code und im Dossier steht.

### 2.2 Konstanten im Diff

| Konstante | Fundstelle | Beleg |
|---|---|---|
| B == 36 / 37 | `esp_brocken.c` | Tabelle [36]/[37] @0x80071d40 (Stichprobe 3) |
| r 0, Band 8, Maske 0x100 | room_coll-Aufruf | @0x800187dc/e8/f4 (Stichprobe 1) |
| B := 37 (0x25) | `wr16(f->row,0x02,37)` | @0x80018818/28 |
| vel := 0 | drift + row 0x10/12/14 | @0x8001882c/30/38 |
| slot+0x1e := h | `wr16(f->row,0x1e,h)` | @0x80018848 |
| Flags := row[0x0e], Anim := row[0x16] | R37 | @0x80018884/8c, @0x8001889c/a4 |
| Vorschub | `re15_esp_fx_zeile_weiter` | `jal 0x800174e4` @0x800188a0 |
| `f->floor_y = INT32_MAX` | R36 | **gekennzeichnet** als "PORT (keine Original-Adresse)" im Code, als "PORT-WAHL" im Dossier und in der Commit-Message. Die Begruendung ist nachgemessen: Das Original hat keine Klemme (Stichprobe 5). Die Port-Klemme ist ein Port-Artefakt, und ohne die Ausnahme kaeme die Weltlage nie unter h. Im VORHER-Lauf ist die Wirkung zu sehen: `drift.y` zittert +-3..16 auf der Klemme. Der Wert ist derselbe wie bei `ESP_KEIN_BODEN` (`re15_esp.c:559`). |

Testschwellen (90 Bilder, 300/900 Bilder, Frist `3 + 9 + 1`) sind reine Pruefkriterien und kein Verhalten.

### 2.3 Guess-Tells / Env-Schalter

`git diff 154a73c1 -- re15_port | grep "^+" | grep -inE "deferred|tunable|interim|for now|faithful|plausib|TODO|sieht richtig|getenv"`
findet **nichts**. Es gibt keinen neuen Env-Schalter und kein Verhalten hinter einem Env-Schalter. Die
Commit-Message von `1408e296` traegt alle Adressen (@0x800187c4, @0x800187dc/e8/f4/f8, @0x80018818/28,
@0x8001882c-38, @0x80018848, @0x8001885c, @0x80018878, @0x80018884/8c, @0x8001889c/a4, @0x800188a0,
@0x8001a2fc-388, @0x8001a40c).

### 2.4 Erklaert der Fix den Befund?

Die Vorbedingung ist im eigenen VORHER-Lauf gemessen: alle 612 Id-7-Zeichnungen stehen auf B=36, der
Anim-Record kreist 0..4, und fx bleibt bei 6. Statisch heisst das: Ohne R36/R37 kehrt `esp_fx_dispatch_b`
fuer B=36 ohne Wirkung zurueck (Basis kannte nur 12/29), und die Anim-Schleife (Record 5 `p20ff`)
erreicht den Terminator (Record 10) nie.
Nach dem Fix sind die Uebergaenge in der echten exe zu sehen: B 36 -> 37 -> 0, `fl=13`, Anim 6..9,
danach verschwindet der Platz. Der Fix greift damit genau an der gemessenen Ursache an. Ein Symptom wird
nicht nur ueberdeckt (etwa durch einen Zeitablauf).

**RE-Gate: haelt.**

---

## 3. Vertrag und Pfade (Schritt 5)

* `git diff 154a73c1 --name-only`: `analysis/befunde_runde35/D_redhawk.md`, `D_redhawk_bild245.png`,
  `D_redhawk_zoom.png`, `re15_port/engine/src/esp_brocken.c` (NEU), `re15_port/include/re15_esp_brocken.h`
  (NEU), `re15_port/engine/src/re15_esp.c` (**+3 Zeilen**: include, Export `re15_esp_fx_zeile_weiter`, Haken
  in `esp_fx_dispatch_b`), `re15_port/tests/integration/test_r35_redhawk.cmake`,
  `re15_port/tests/unit/probes/r35_redhawk.cmake`, `re15_port/tests/unit/test_r35_redhawk.c`.
* `grep -E "^release/|platform/android|shared_assets/PSX|tests/unit/CMakeLists.txt|tests/integration/CMakeLists.txt"`:
  kein Treffer.
* Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse: keine. Das deckt sich mit der Zuteilung
  (VERTRAG.md: Spur D hat keine). `grep` auf `flag_set|msg_install|scd_event|aot_|re15_game_flag|getenv` in
  den hinzugefuegten engine/include/platform-Zeilen findet nichts.
* Tests sind vorhanden und messen: Der Unit-Pin verfolgt jeden Brocken Takt fuer Takt gegen R36/R37/den
  Terminator. Der Integrationshaken laeuft mit der echten exe und ist gegen die VORHER-exe nachweislich rot
  (1.2). `git status --short` im Baum ist leer.

**Pfad- und Vertrags-Gate: haelt.**

---

## 4. Maengel

**Keine.**

## 5. Hinweise (kein Abnahme-Hindernis, fuer Integration/Nutzer)

* **H1 Zusammenfuehren mit master 87cc8575:** `git merge-tree --write-tree --name-only master HEAD` meldet
  `CONFLICT (content): Merge conflict in re15_port/engine/src/re15_esp.c`. Der einzige Konfliktblock sind die
  Zeilen 19-23 mit zwei konkurrierenden `#include`-Zeilen (`re15_granate_r35.h` von Spur A,
  `re15_esp_brocken.h` von Spur D). Aufloesung: beide behalten. Im Ergebnisbaum steht der Haken
  `if (re15_esp_brocken_b(f)) return;` danach unveraendert vor den Faellen 29/12 in `esp_fx_dispatch_b`.
  master behandelt B 36/37 nicht selbst. Nach dem Mergen die Suite im Integrationsbaum fahren (Pin 2a nutzt
  den HE-Resolver, den Spur A geaendert hat).
* **H2 Pin 2b (Pistole):** Im eigenen Lauf meldet er `Entstehungen 0 = Enden 0`, prueft die Brocken-Mechanik
  also nicht. Er ist nicht falsch, aber schwach. Die Mechanik pruefen Pin 1/2a/3.
* **H3 ROOM1190:** Die Luken-Hunde habe ich mit der exe nicht erreicht (1.3, wartender Skript-Pounce grid
  0x43). Mechanik und Daten sind dieselben wie in 11D0, und Pin 3 deckt 1190 ab. Den Lauf durch die Luke
  kann der Nutzer bei seiner Gegenprobe nachholen.
* **H4 Beobachtung ausserhalb der Spur:** Im Software-Framedump erscheint die Lache unter dem toten Hund
  **tuerkis**. Sie ist VORHER und NACHHER gleich und verschwindet mit `RE15_NO_SHADOW=1` (Scratch
  `teal_shadow_test.png`), gehoert also zur Schatten-/Lachen-Schicht und nicht zu Spur D. Ob der beschleunigte
  Renderer sie ebenso zeigt, ist ungeprueft (gdigrab ist in dieser Sitzung weiss). Das ist ein Kandidat
  fuer eine eigene Pruefung.
* **H5 OFFEN-Liste des Dossiers** (R19 noop, Hunde-FX 7/8 Fehl-Sub, Id 13/Id 1 im Zensus): Jeder Punkt steht
  mit Adresse und naechstem Messweg im Dossier. Keiner betrifft den Nutzer-Punkt, und im Code ist nichts
  davon versteckt.

## 6. Abschluss

| Gate | Stand |
|---|---|
| Punkt 1 | erfuellt (VORHER fx dauerhaft 6 und Id 7 bis zum Laufende; NACHHER fx = 0 ab F164, letztes Id-7-Bild F163; zwei Hunde: F146/F215) |
| Suite | 480/480 (eigener Lauf) |
| @0x-Gate | haelt (7 Stichproben selbst disassembliert, PORT-WAHL gekennzeichnet und begruendet) |
| Pfad-/Vertrags-Gate | haelt |
| Tests | unit_r35_redhawk + integration_r35_redhawk gruen; integration gegen VORHER rot |

**bestanden = true**
