# Runde 34 (Granaten) — INTEGRATION

Stand: 2026-09-30, Zweig `r34g/integration`, Baum `.claude/worktrees/r34g_int`, Basis master 7d4d11dd.
Bauverzeichnis: `re15_port/build_r34_int` (nur ueber `RE15_BUILD_DIR=… bash re15_port/tools/local_build.sh`).
Massgeblich: `BAUPLAN.md`, `bau_c0.md`, `bau_a.md`, `bau_b.md`, `bau_c.md`, `bau_d.md` samt Gegenpruefungen und NACHBESSERUNG.
Dieses Dossier wird fortlaufend geschrieben und committet (Sitzungslimit-Schutz).

## 0. Fortschritt (Kurzstand, wird je Schritt nachgezogen)

| Schritt | Stand |
|---|---|
| S1 Merge r34g/b-schaden | erledigt 94c7d41d — konfliktfrei, Bau OK, unit_r34_schaden + unit_r34_reaktion gruen (430 Tests) |
| S1 Merge r34g/a-granate | erledigt c4ebd472 — konfliktfrei, Bau OK, unit_r34_wurf + B-Sonden + ESP-/Waffen-Pins gruen (431 Tests) |
| S1 Merge r34g/c-plattform | erledigt 7c202b7a — konfliktfrei, Bau OK, C-Sonden (2 unit + 2 exe) + A/B-Sonden gruen (435 Tests) |
| S1 Merge r34g/d-re2fx | erledigt 3086da7e — konfliktfrei, Bau OK, alle 12 r34-Sonden gruen (440 Tests) |
| S2 W1..W11 | W1-W5 erledigt (c91ae1ed); W6-W11 in Arbeit |
| S3 volle Suite | offen |

## 1. Merges (Schritt 1)

### 1.1 r34g/b-schaden (enthaelt r34g/c0-vertrag) — Merge-Commit 94c7d41d
* `git merge --no-ff r34g/b-schaden`: **keine Konflikte** (27 Dateien, +5102/-181; C0-Vertrag V1/V2b/V3/V5 + RE2-Assets
  `shared_assets/RE2/CORE00.ESP` 8572 B / `TEX.TIM` 132320 B kommen mit).
* Bau: `local_build.sh configure` + `build` im frischen `build_r34_int` -> `LOCAL-BUILD-OK (configure)` / `(build)`.
* Sonden der Spur: `unit_r34_schaden` Passed, `unit_r34_reaktion` Passed (`probe_r34_reaktion alle: 0 Fehler`, Zensus
  352 Laeufe / 0 Haenger, Ausgang K1 220-222 ok). `ctest -N`: 430 Tests.

### 1.2 r34g/a-granate — Merge-Commit c4ebd472
* `git merge --no-ff r34g/a-granate`: **keine Konflikte** (9 Dateien, +2841/-58: re15_esp.c, re15_esp.h, game_step_common.c,
  player_common.c, menu_common.c, probe_r34_wurf).
* Bau: `configure` (neue Sonde probes/r34_wurf.cmake) + `build` -> `LOCAL-BUILD-OK`.
* Sonden: `unit_r34_wurf` Passed (`probe_r34_wurf: ALLE PRUEFUNGEN GRUEN`; MITTE gesund SE-Folge 0x010a0601..0x010a0001 +
  Liegen, Explosion Bild 109, Zeitlinien HOCH/TIEF/vergiftet wie BAUPLAN 1.1), dazu erneut `unit_r34_schaden`,
  `unit_r34_reaktion` und die ESP-/Waffen-Bestandspins `probe_abzug_takt`, `unit_r17_waffen_loop_pin`, `unit_r26_mg_blut`,
  `unit_r30_granate`, `r30b_muendungshoehe`, `unit_espr_11e0`, `unit_aim*` — 11/11 gruen. `ctest -N`: 431.
### 1.3 r34g/c-plattform — Merge-Commit 7c202b7a
* `git merge --no-ff r34g/c-plattform`: **keine Konflikte** (main.c, audio_pc.c, render_pc.c, room_pc.c, neue
  fx_plattform_pc.c/.h, Sonden probe_r34_plattform*, Werkzeuge bau_c_werkzeug/ + bau_c_gegen_werkzeug/).
* Bau: `configure` + `build` -> `LOCAL-BUILD-OK`. Die drei Warnungen `-Wdiscarded-qualifiers` (main.c:8510/9769/10133)
  sind Altbestand (Gore-Pfad), nicht durch den Merge entstanden.
* Sonden: `unit_r34_plattform`, `unit_r34_plattform_ton`, `integration_r34_plattform_takt` (23.0 s),
  `integration_r34_plattform_esp_eintritt` (9.7 s), dazu `unit_r34_schaden`, `unit_r34_reaktion`, `unit_r34_wurf` —
  7/7 gruen. `ctest -N`: 435.
### 1.4 r34g/d-re2fx — Merge-Commit 3086da7e
* `git merge --no-ff r34g/d-re2fx`: **keine Konflikte** (re2_fx.c, re2_fx.h, re2fx_pc.c/.h, fuenf Sonden, zwei
  Werkzeuge unter `tools/`).
* Bau: `configure` + `build` -> `LOCAL-BUILD-OK`; keine neuen Warnungen.
* Sonden: `unit_r34_re2fx`, `_knochen`, `_bild` (1.1 s), `_raum`, `_pc` + alle frueheren r34-Sonden (A/B/C inkl. der
  zwei exe-Pins) — **12/12 gruen**. `ctest -N`: **440** (= 428 + B 2 + A 1 + C 4 + D 5).
* Befund zum Merge insgesamt: die Spuren hielten den Dateibesitz aus BAUPLAN §3.0 ein; git fand in keinem der vier
  Merges eine Ueberschneidung. Kein Verhalten ging verloren (keine Konfliktaufloesung noetig).

## 2. Integrationswuensche (Schritt 2)

(W1..W11 je mit Beleg oder begruendet OFFEN.) Messlaeufe: `integration_werkzeug/lauf.sh` (byte-gleiche exe-Kopie
`re15_pc_m1.exe` im selben Verzeichnis gegen fremde `taskkill /IM`, Titel-Autostart, Fenster x3, Ausgaben unversioniert
im Scratchpad der Sitzung), Bildbogen `sheet.py` / `crop.py`. Commit W1-W5: c91ae1ed.

### W1 — TIM-Slot der RE2-FX-Seiten 50 -> 52 (erledigt)
* Befund beim Merge: `re2fx_pc.h` `RE2FX_TIM_SLOT 50` = Spur-C-Slot `RE15_TIM_SLOT_FX_SEITE_1E` (main.c) — sobald W2
  die RE2-Seiten laedt, haette das die RE1.5-Seite 0x1E (Rauch/Feuer/Feuerball) ueberschrieben.
* Gebaut: `RE2FX_TIM_SLOT 52` (re2fx_pc.h, Kommentar mit der Belegung), render_pc.c-Slotliste "52 = RE2-FX-Seiten,
  53..55 frei", zwei `_Static_assert` in main.c (52 != 50/51, 52..55). `unit_r34_re2fx_pc` prueft Slot (Pruefung 1/4)
  schon ueber die Konstante (`s_slot.slot != RE2FX_TIM_SLOT`, `a->slot != RE2FX_TIM_SLOT`); nur der Kopf nannte "50".
* Gemessen: `unit_r34_re2fx_pc` gruen; exe `[re2fx] RE2 TEX.TIM 132320 B -> re2fx_pc_lade_tex rc=0 (Slot 52)`.

### W2 — RE2-Assets beim Start (erledigt)
* CORE00.ESP resident + `re2fx_register_core` macht Spur C schon (main.c nach `re15_audio_init`, Puffer `static`,
  gemessen `[re2fx] RE2 CORE00.ESP 8572 B -> re2fx_register_core rc=0`).
* NEU: direkt dahinter `re15_pc_read_re2("TEX.TIM")` -> `re2fx_pc_lade_tex` -> `free` — genau einmal, nach
  `re15_render_init` (main.c ~3175) und nach dem Hochladen der RE1.5-Seiten 50/51. Lader-Belege (RE2 FUN_80076a40):
  x = 28*64 - 1024 = 768 @0x80076a64-80, y = 256 @0x80076a9c-a8, CLUT-y 480 @0x80076b00-0c.
* Gemessen: `rc=0 (Slot 52)` (Lauf w3_a).

### W3 — eine Kamera fuer den RE2-Zeichner (erledigt)
* Befund: Spur C legte die Ansicht in `re15_pc_fx_kamera_setzen` ab (kein Leser), Spur D erwartet
  `re2fx_pc_set_ansicht` — die wurde nie gerufen, `s_ansicht.gueltig` blieb 0, **re2fx_pc_draw zeichnete nichts**.
* Gebaut (EIN Weg): main.c ruft nach `pc_draw_effects(&cam_view, cx, cy, cam_has_region, cam_region_xs, cam_region_zs)`
  `re2fx_pc_set_ansicht(&cam_view, cx, cy, pc_fx_camf(), cam_has_region, cam_region_xs, cam_region_zs)` — dieselben
  Variablen, camf = derselbe `pc_fx_set_camf`-Wert des angezeigten Cuts — dann `re2fx_pc_draw()`, danach
  `re2fx_pc_set_ansicht(NULL, ...)` (ungueltig ausserhalb des Passes). Die leserlose Ablage `re15_pc_fx_kamera_*`
  (fx_plattform_pc.c/.h) ist entfernt (kein Test, kein Aufrufer).
* Abnahme (Lauf w3_a: ROOM1140 `RE15_DEBUG_JUMP=1140@250`, `RE15_FORCE_AUFSCHLAG=2@320,1@400`, Framedump 310-520/2,
  x3): Saeure (q = (-6100,0,-17600) = 1500 vor Leon, Gier 0): F322 erste Puffs, F324 grauer Puff, F326-F332 orange-
  roter Puff/Feuerball (CLUT 483), F334-F336 dunkler Rauch, danach leer; Brand: F402 Feuerball + Flammen, die Flammen
  gleiten (nach links im Bild), brennen bis ~F500 und verloeschen — `integration_werkzeug/w3_saeure_crop.png`,
  `w3_brand_crop.png`. Die Tischkante verdeckt den Fuss der Effekte (PRI-Maske des Tischs, vorne). Lage-Abnahme an
  der echten Granate: W9 (Explosion an der Liegestelle des RE1.5-Sprites).

### W4 — re2fx_reset im SCD-Raumaufbau (erledigt)
* `engine/src/scd_room_setup.c`: `#include "re2_fx.h"`, `re2fx_reset()` direkt hinter `re15_esp_fx_reset()`.
  Port-Zuordnung nach RE1.5 (Pool-Wisch `sb zero,0(at)` @0x80019378, einziger Aufrufer `jal 0x80019354`
  @0x8003996c im Raumlader); RE2 leert nur ueber FUN_8001d07c (einziger Rufer `jal 0x8001d07c` @0x800569a8) bzw. beim
  Boot (@0x8001bac4-e4: 96 x `sh zero,-29432(at)`) — selbst nachgelesen (`re2_disasm.py`/`re15_disasm.py`).
  Der zweite Weg (neben room_pc.c) erfasst die Selbst-Tuer (ROOM1090) und jeden SCD-Raumaufbau.

### W5 — SELECT-Tor des UTILITY/DEBUG-MENU (erledigt)
* Belege (selbst): FUN_8001443c testet SELECT (`lhu v0,-14494(v0)` = 0x800ac762, `andi v0,v0,0x100` @0x80014440-4c);
  Wort-Scan nach `jal 0x8001443c` ueber PSX.EXE + STAGE1..6/DEBUG/TITLE.BIN: **nur @0x8001c988** (Spielschleife).
  Statusschirm: `jal 0x80029a98` @0x8001cb40 mit a0 = 1, a1 = 0x8004603c (Task 1 = Statusschirm, Task-Start:
  `sw a1` / `sh v0(=2)` @0x80029aac/bc), dann `jal 0x80029ac8` @0x8001cb48 (a0 = 1): `sh a0,2(v1)` @0x80029adc
  (warte auf Task 1), `sh v0(=1),0(v1)` @0x80029ae8, `jal 0x8006e3c8` @0x80029ae4 (Wechsel) — die Spielschleife ist
  geparkt, FUN_8001443c laeuft waehrend des Statusschirms nie.
* Gebaut: main.c `if ((gctx.pad_pressed & RE15_PAD_BIT_SELECT) && !re15_menu_gameplay_frozen())` (frozen = Menue
  lebt / Stufe != 0 / Latch, menu_common.c:146). Abnahme ueber den Item-Debug-Weg in W9 (debug.log ohne
  `[debug-menu] OPEN`).


## 3. Volle Suite (Schritt 3)

## 4. OFFEN
