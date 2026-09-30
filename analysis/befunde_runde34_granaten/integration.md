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
| S2 W1..W11 | W1-W5 erledigt (c91ae1ed), W6 (5b7d3dc9, 62d52d33), W7 (6f7aa99f), W8 (7fbd45a0, 6e320214), W9 (c49f20fe), W10 (b787f722), W11 (Dossier) — alle erledigt |
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


### W6 — Part-Farben (Code erledigt 5b7d3dc9; Sichtabnahme siehe unten)
* Befund: `re15_pc_re2_part_tint` las nur Parts 0..15 (V4 hatte 16 Felder) und setzte Part i = Bone i ungeprueft.
* Belege (selbst): Hund-Tod-Faerbung EMD0G_MOD0 `sw a0,112(v0)` @0x801047f4 / `sltiu v0,s0,0x11` @0x801047f8 (17 Parts);
  Spinne EMS25 FUN_8010609C `sw a1,112(v0)` @0x801060b0 / `sltiu v0,a2,0x14` @0x801060b4 (20 Parts).
* Gebaut: Schleife ueber alle `re2z_part_tint[20]`; Parameter `re2_rig`: 1 = das GEZEICHNETE Skelett ist das RE2-Rig der
  Bank (reines RE2-EMD oder Hybrid `re2_hybrid_apply`, Bone-Slot = RE2-Part) -> Part = Bone; 0 = RE1.5-Rig (Rueckfall ohne
  RE2-Archiv) -> Bone = `re2_hybrid_perm[Part]` (dieselbe Tabelle wie `re2z_part_to_bone`/`re2z_perm_for`), -1 (Hunde-
  pfoten 7/10) = kein Bone. Neues Bankfeld `re15_enemy_bank_t.re2_rig` (gesetzt in `pc_enemy_load_re2_kind`); main.c
  nimmt `re2_rig` nur, wenn `npc_skel` eines der drei Bank-Skelette ist (die RE1.5-Posenbank des Sitz-Imports faellt heraus).
* Hinweis: die Aufgabe nennt "Gegenpruefung C H5" — gemeint ist H3 (`bau_c.gegenpruefung.md` §8: "Part i = Bone i ...
  `re2z_bone_to_part`"); H5 ist dort die Suite-Zielzeile.
* Sonde `unit_r34_plattform` 115-119 (RE2-Rig Hund Part 16, RE1.5-Rig Hund Part 8 -> Bone 7 / 16 -> 14 / Pfote weg,
  Negativ-Kontrolle RE2-Lesart, Spinne 19 in beiden Rigs): 82/82 gruen. Mutationen: "Grenze 16" -> rot 115/116/118/119;
  "Part = Bone immer" -> rot 116.

### W9 — Integrationstest `integration_r34_granaten` (erledigt c49f20fe)
* Dateien: `tests/integration/test_r34_granaten.cmake` (Skript), `tests/unit/probes/r34_granaten_exe.cmake` (Registrierung,
  Muster probes/r30_granate.cmake, TIMEOUT 1200). 8 exe-Laeufe, gemessen 176 s.
* Aufstellung (gemessen, weil die Fresser in ROOM1140 bei Leon < 4000 aufwachen — RE1.5-Fresser-Tor `ai_dist < 0xfa0`,
  `re2z_exec_feeding` — und RE2-/RE1.5-KI danach verschieden laufen): RE2-KI Leon (-1676,-18070) Blick 1076 -> TIEF-Granate
  liegt bei (-2199,-20548), Zombie 3 (0x10, HP 80) im Bild X in 771; RE1.5-KI Leon (-4311,-19289) Blick 0 -> Granate bei
  (-1856,-19902), Zombie 2 (0x10, HP 75) in 513. Skript ab Spielbild 1: `MD0.6,MDA0.2,MD2.5,W5` (TIEF, Abzug im Bild 19).
  Faellt die Vorbedingung (kein Treffer), meldet der Test "AUFSTELLUNG" statt eines Granatenfehlers.
* Harness-Befunde: (1) der Bildzaehler faellt beim Debug-Sprung auf 1 zurueck (Zustandslog wird ab dem Rueckgang gelesen);
  (2) das Skript laeuft mit Basis "spiel" zweimal (Startraum ROOM1240 bis zum Sprung, dort player_mode 2 — gemessen: mg
  bleibt 5, kein Wurf) — deshalb "genau ein SPAWN"; (3) der Zustandslog steht in main.c HINTER dem ESP-Takt: der HP-
  Verlust und Zustand 2/3 stehen schon in Zeile X, die Reaktion (neuer Clip / +0x6/+0x7) in Zeile X+1; (4) START wirkt nach
  dem Sprung erst ab Bild ~6 (Pausebits FF000007 in F1-F5) — der Item-Debug-Lauf beginnt bei Bild 260 (> Sprungbild 250,
  der Startraum erreicht das Skript nie) mit Leon am Tuer-Sprungpunkt (alle Fresser > 4000, keiner wacht).
* Ergebnis je Lauf (Auszug): RE2 g9/g10/g11: A 19, S 43, L 83, X 119, Zombie 3 HP 80 -> -120, Zeile 9/11/10, Leiche ab
  Bild 180/180/170; RE1.5 g9/g10/g11: X 119, Zombie 2 HP 75 -> -125, Zeile 9/10/11, Leiche ab 173; debug: Menge 255, W09,
  kein `[debug-menu] OPEN` nach dem Sprung, A 593, S 617, L 657, X 693; abzug: Zombie 2 in 849 vor Leon, kein HP-Verlust.
* Negativ-Kontrolle (im Skript, vor jedem exe-Lauf): erfundene Zeilen "Schaden im Abzugsbild", "keine Reaktion in X+1",
  "falsche Zeile" muessen fallen, "kein Treffer" muss leer bleiben — sonst "Auswerter defekt". Beim ersten Lauf fing sie
  einen echten Auswerterfehler (CMake kennt nur CMAKE_MATCH_0..9, der 10. Fang lieferte leer).
* Mutationsproben (`mut_exe.sh`: Datei aendern, exe bauen, Test mit Auswahl `-DR34_NUR`, byte-gleich zurueck, neu bauen):
  | Id | Mutation | Lauf | Ergebnis |
  |---|---|---|---|
  | M1 | `ENT[9] = {1,1,1,0}` (Sofort-Bruecke zurueck) | abzug | ROT "Gegner 2 verliert im Abzugsbild HP 50 -> -150" (g9_re2 bleibt gruen: Ziel ausser Reichweite 1000) |
  | M2 | Zuender 42 -> 43 (re15_esp.c Routine 30) | g9_re2 | ROT "Explosion im Bild 120, erwartet L + 36 = 119" |
  | M3 | W5-Tor entfernt | debug | ROT "SELECT im ITEM-Raster oeffnete das UTILITY/DEBUG-MENU" |
  | M4 | `re2z_row_from_atktype[3]` 11 -> 10 | g10_re2 | GRUEN — diese Tabelle speist nur den Direktaufruf; der Explosionsstempel nimmt `re15_react_table[Art]` + Tausch 10/11 (re15_damage.c `re2_gl_explosion_stempel`) |
  | M4b | Tausch 10/11 in `re2_gl_explosion_stempel` entfernt | g10_re2 | ROT "Reaktionszeile +0x5 = 10, erwartet 11" |
  | M5 | RE1.5-Todeshandler kehrt fuer Zeile 9..11 sofort zurueck (Nachbau des Original-Haengers: Zeile NULL) | g9_re15 | ROT "keine Reaktion im Bild X+1" |

### W6 — Sichtabnahme (erledigt 62d52d33, 9c58453b)
* Weg: der Wurf trifft einen Hund/eine Spinne nicht verlaesslich (Hunde in ROOM11D0 rennen sofort los; die ruhenden Hunde
  in ROOM1190 liegen ausserhalb des Kastens/Bands — `eingriffe=0` bei 577 Abstand gemessen; MITTE landet wegen der
  Seitendrift 1752 immer ~1400 neben dem Ziel der Auto-Nachfuehrung). Deshalb ein MESS-HAKEN ohne Spielverhalten:
  `RE15_FORCE_EXPLOSION="<art>@<bild>:<slot>"` (fx_plattform_pc.c) = Routine 31 am Gegner: P.y = y - 500 (`lh v0,42(v1)`
  @0x800185a0 / `addiu v0,v0,-500` @0x800185a8), FUN_80012d60(0x1f4 @0x80018598, `jal` @0x800185b8), Art 3/4 + RE2-Aufschlag
  (E8). Parse-Sonde `unit_r34_plattform` 104-106. Wurf/Flug selbst nimmt W9 ab.
* Framedumps (x3): ROOM11D0 + `RE15_SET_FLAG=3:152`, Hund Slot 5, Bild 35: Art 3 -> HP 95 -> -205, +0x5 = 10 (Spur-B-
  Konvention des Hundes: RE1.5-Waffen-Id 10 = Saeure -> RE2-Zeile 11, bau_b Sonde 140), Koerper ab F36 einheitlich
  dunkel-oliv (0x00003F2F) statt braun/hellbraun (F34) — `integration_werkzeug/w6_hund_saeure_crop.png`; Art 4 -> +0x5 11
  (-> Zeile 10), Koerper schwarz verkohlt (0x00202020) mit Flammen — `w6_hund_brand_crop.png`. ROOM2060 Spinne Slot 1
  (Decke), Bild 40: Art 4 -> HP 111 -> -19, +0x5 11 -> 10, Koerper dunkel (0x00202F2F) — `w6_spinne_brand_sheet.png`.
  (Der Haken setzt die "Granate" auf die Deckenhoehe der Spinne; ein echter Wurf liegt am Boden und erreicht sie dort nicht.)

### W8 — Helligkeit der RE1.5-ESP-Effekte (Befund BESTAETIGT, behoben 7fbd45a0, Pin 6e320214)
* Messung (Framedump x1, HE-Explosion am Tuer-Sprungpunkt, Feuerball 0x03195000: Routine 10 Flags 0x13 = ABE, ABR 0,
  Palette 483 Index 1 = (248,248,248) aus DATA/TEX.TIM): wirksamer Beitrag 2*out - B im Feuerball-Ausschnitt max
  **125/125/125** (905 Pixel) = 248 x 128/255.
* Original (selbst disassembliert): FUN_800537e4 (Aufrufer `jal 0x800537e4` @0x800212fc) setzt die Primitivfarbe aller
  1024 ESP-POLY_FT4 ab 0x80093394 auf 0x80 (`ori s3,zero,0x80` @0x800537ec, `sb s3,4/5/6(s0)` @0x800538fc/900/908); der
  Sprite-Bau FUN_800534c4 schreibt je Quad nur das Code-Byte (`sb v0,7(t2)` @0x8005369c; Code 0x2C/0x2E = moduliert) ->
  Texel x 0x80/0x80 = x 1.0. Port: `pc_draw_effects` gab 128 an den unbeleuchteten Queue-Weg, der die Farbe als SDL-
  Faktor /255 nutzt (render_pc.c `re15_render_textured_tri`: `SDL_Color tint = { r, g, b, ... }`).
* Fix: 255 (PSX 0x80 = SDL 0xFF wie `psx_prim_to_sdl_vert` im beleuchteten Weg; re2fx_pc nutzt dieselbe 255 fuer die RE2-
  Paketfarbe 0x808080 @0x800783cc-d0). Nachmessung: **249/249/249**, dieselben 905 Pixel. Wirkt auf ALLE RE1.5-ESP-Effekte
  (Muendung, Rauch, Huelse, Blut, Feuer, Feuerball) — sie waren seit jeher halb so hell (Release-Hinweis).
* Pin: Lauf "debug" von `integration_r34_granaten` misst denselben Beitrag (Werkzeug `probe_r34_ppm_beitrag`, Schranke
  >= 240 je Kanal, >= 200 Pixel). Mutation M6 (Farbe wieder 128): ROT "Feuerball im Explosionsbild zu dunkel".

### W7 — ESP-Routinen 41/42 (erledigt 6f7aa99f)
* Belege (selbst disassembliert): R41 @0x80018ef4 (41 Instruktionen), R42 @0x80018f98 (bis @0x80019174), Vorschub
  FUN_800174e4 @0x800174e4-0x800175d8, "RNG" FUN_8001af20 @0x8001af20-54, Treffer-Test FUN_8002b7e8 @0x8002b7e8-894,
  Spawn-Anim FUN_80019700 @0x8001989c-bc / FUN_800199d4 @0x80019b70-90, Anim-Schritt @0x8001a38c-47c — Zitate im Code
  (re15_esp.c case 41/42, esp_treffer_test, re15_esp_fx_spawn_ex).
* Befunde beim Portieren (ueber bau_c.md N1.1 hinaus):
  1. Das "RNG" von R41 wertet das Register a0 aus, das der Vorschub FUN_800174e4 hinterlaesst: den dritten Wortladebefehl
     des zweiten 16-Byte-Blocks (`lw a0,8(a2)` @0x80017594 bzw. `lwl/lwr a0,0xb/8(a2)` @0x80017548/4c) = u32 der NEUEN
     Zeile ab +0x18. Effekt 0x0b: dort 0 -> +0x0a bleibt 12 (Sonde 12); synthetisch w = 0x81 -> +2 (Sonde 50).
  2. Der SCD-Spawn gibt `floor_y = y` (scd_vm.c) — die Port-Sammelklemme hielt den Strahl auf Spawnhoehe fest. Der Takt hat
     keine Klemme (@0x8001a2fc-388), Effekt 0x0b hat B = 0 -> `floor_y = ESP_KEIN_BODEN` in R41 (Muster A6/E12).
  3. Die Spawner setzen +0x6e := 1 und +0x6d := Satz[0].Byte2; der Port startete mit 0/0. R41 setzt +0x6e OHNE den
     Zeitgeber — mit dem alten Start sprang Strom 0 im Spawnbild einen Satz zu weit. Port jetzt wie das Original. Zensus
     aller 506 Effekte der Auslieferung (501 aus den RDTs, 5 aus CORE00): Dauer(Satz 0) == Dauer(Satz 1) ueberall -> fuer
     alle anderen Effekte dieselbe Bildfolge; sichtbar anders nur ein Platz mit Bild-Stopp im Spawnbild (Satz 1 statt 0):
     Pin `unit_r34_wurf` 167/168 (Salven-Huelse, Flags 0x63) mit Beleg @0x80019b74-78 nachgezogen.
* Sonde `unit_r34_wasser` (neu, `probes/r34_wasser.cmake`, echte ROOM2000-Daten, 19 Pruefungen): Freigabe Strom 0 im Bild 1
  (A 42, Flags 0x13, Satz 0 / Zeitgeber 3 -> 2, +0x0a 12, xlat 50, drift (49,12)), Stroeme 1..5 halten 5k (Flags 0x61),
  Zaehler 1..25, defH +768 ab 16 (0x1cc4 ... 0x37c4), kein Bodenklemmen (xlat_y 660 nach Bild 11), Schleife im Bild 27
  (Zeile 0, xlat 0, Satz 1 / Zeitgeber 3 -> 2), Bild 28 neu frei; Strom 3 frei im Bild 16 (Satz 11 -> Anim-Schritt 12, weil
  sein Zeitgeber nach 15 Bildern Bild-Stopp auf 0 steht); RNG-Weg mit w = 0x81; Treffer an wpos -> Flags 0x33 + Physik-
  Stopp, NEGATIV ohne Gegner Flags 0x13. Mutationen MW1-MW5 (Schwelle 16->17, RNG aus alter Zeile, kein Physik-Stopp,
  mit Bodenklemme, Spawn-Satz 0) je ROT, byte-gleich zurueck.
* Sichtabnahme (Framedump x3, ROOM2000 per Debug-Sprung = derselbe `re15_room_apply_pending`-Weg wie die Tuer, Leon an der
  Tuer-Ankunft aus ROOM2050 (-26450,10250), `RE15_FORCE_CUT=9` — Cut 0 des Sprungs cullt die Lage, sichtbar nur Cuts
  5..10/13, bau_c.md N1.2): ein durchgehender Wasserfall aus dem Gitter der Kanalwand ab F20, bis F80 unveraendert
  laufend (`integration_werkzeug/w7_wasserstrahl_room2000_cut9.png`); vorher drei 1-Pixel-Punkte fuer 27-32 Takte.
  FX-Zaehler im Zustandslog bleibt 21 (18 Strahl + 3 Effekt 0x06) — die Stroeme sterben nicht mehr am Satz-10-Terminator.
* Nicht umgesetzt (OFFEN, bau_c.md INTEGRATIONSWUNSCH 6 zweiter Teil): Routinen 24/25 (@0x80017f50/@0x80017fa4) und 13/19
  (@0x80017990/@0x80017d08) — kein Auslieferungs-Raum spawnt Effekt 0x0d (Zensus bau_c.md N1.3), sie sind unerreichbar.

### W10 — RE15_MIN_TESTS (erledigt b787f722)
* `tools/local_build.sh`: 428 -> **442** (Kopf-Historie: B +2, A +1, C +4, D +5, Integration +2 = W9 + W7); `ctest -N` im
  Integrationsbaum = 442.

### W11 — Fuer die Release-Notes (Verhaltensaenderungen UEBER die Granate hinaus)
* **G5 (Endkampf ROOM5090/5091): Flinch-Zuschlag jetzt nach der Treffer-ZEILE fuer ALLE Waffen** (Spur B, B9): Zuschlag aus
  dem Byte @0x801056B3 + Zeile statt je Waffe, Zerfall -1 je 16 Bilder mit Fenster 15, Sperre +0x1D3 = 15 mit Abbau.
  Wirkt auch auf Pistole/Schrot/MG (bau_b.md B9, OFFEN 11: Abnahme durch Nutzer/Orchestrator).
* **Drehen im Zielen fuer ALLE Waffen korrigiert** (Spur A, A8): nur noch die Ziel-Drehung, LINKS minus / RECHTS plus, 24 je
  Bild beim Heben/Senken/Nachladen/Abzug, 48 im Halten (@0x80033000-4c, @0x800333a0-e8, @0x8003355c-fc, @0x80033cf0,
  @0x80033e00) statt vorher Lauf- + Ziel-Drehung zusammen (72); im Halten mit OBEN/UNTEN dreht Leon jetzt (vorher netto 0).
* **Auto-Nachfuehrung beim Heben nach FUN_8001a8f8** (Spur A, H-1): Schritte ohne `& 0xfff`, Richtungsgrenze um +s
  verschoben (@0x8001a958-9b0) — fuer Ziele knapp unter 180 Grad dreht Leon jetzt in die Original-Richtung.
* **Raum-Effektbank steht vor dem SCD-Eintrittstakt** (Spur C, N1): Eintritts-Effekte (ROOM2000/2001/20B0/20B1 Wasser,
  ROOM11E0-Funke, ROOM20A0) erscheinen auch beim Betreten ueber eine Tuer sofort (vorher eine Schleifenperiode spaeter bzw.
  nie).
* **Alle RE1.5-Effekte (Muendung, Rauch, Huelse, Blut, Feuer, Feuerball) mit voller Helligkeit** (Integration W8): sie
  liefen seit jeher halb so hell (Primitivfarbe 128 als SDL-Faktor statt PSX 0x80 = x 1.0, FUN_800537e4 @0x800537ec).
* **Wasserstrahl ROOM2000/2001/20B0/20B1** (Integration W7): Routinen 41/42 portiert — durchgehender Strahl aus dem Gitter
  statt drei 1-Pixel-Punkten fuer ~1 s; Tropfen, die Leon oder einen Gegner treffen, bleiben stehen (FUN_8002b7e8).
* **Effekt-Start wie das Original** (Integration W7): Satz 1 / Zeitgeber Satz[0] beim Spawn (FUN_80019700 @0x800198a0);
  sichtbar nur bei Plaetzen mit Bild-Stopp im Spawnbild (z.B. Salven-Huelse: erster Satz 1 statt 0).
* **SELECT im Inventar oeffnet das UTILITY/DEBUG-MENU nicht mehr** (Integration W5); das Item-Debug (SELECT + R1/L1/R2/L2,
  Menge 255) funktioniert wie im Original (Spur A10).
* **RE2-Aufschlaege sichtbar** (Integration W1-W3): Saeure-/Brand-Aufschlag und Bodenflammen werden gezeichnet (vorher ohne
  Kamera -> nie).
* **RE2-Part-Farben fuer Hund (17 Parts) und Spinne (20 Parts)** beim Tod durch Brand/Saeure (Integration W6).
* **Treppen-Unverwundbarkeit des Spielers** (Spur B, B10): auf der Treppe trifft ihn weder Explosion noch Resolver-Angriff
  (+0x93 Bit 0 @0x80038a58/@0x80038c24/@0x80038d00/@0x80038ec0).
* Paket: `shared_assets/RE2/CORE00.ESP` und `TEX.TIM` sind jetzt Pflicht (Gates in make_package.sh / build_android.sh /
  build.gradle); Android-Bau braucht einen frischen Configure (neue Quellen fx_plattform_pc.c, re2fx_pc.c, re2_fx.c —
  Memory reai-v2-android-glob-cache; release/build_android.sh verwirft app/.cxx selbst).

## 3. Volle Suite (Schritt 3)

## 4. OFFEN

Aus der Integration (neu):
* **I-1 Aufstellung von `integration_r34_granaten`**: die Treffer-Laeufe haengen an den gemessenen KI-Wegen der ROOM1140-
  Fresser (RE2-KI: Zombie 3 steht im Bild 119 in 771 der Granate; RE1.5-KI: Zombie 2 in 513). Aendert eine spaetere Runde die
  Zombie-KI, meldet der Test "AUFSTELLUNG ... neu messen" (Vorbedingung), keinen Granatenfehler. Weg: `integration_werkzeug/
  auswert.py` auf einen Probelauf, Leon so setzen, dass die TIEF-Granate (Landung ~2511 vor, ~310 seitlich) den Gegner im
  Bild X trifft.
* **I-2 Hund/Spinne ueber den echten Wurf**: die Part-Farben sind ueber den Mess-Haken `RE15_FORCE_EXPLOSION` abgenommen
  (W6). Ein Wurf an einen Hund/eine Spinne ist nicht verlaesslich (Hunde in ROOM11D0 rennen sofort; die ruhenden Hunde in
  ROOM1190 bei (690,-28500) nimmt der Resolver bei 577 Abstand nicht — Kasten/Band der Ruhe-Pose ungeprueft; MITTE landet
  wegen der Seitendrift 1752 ~1400 neben dem Ziel der Auto-Nachfuehrung). Weg: Kasten/Hoehe des ruhenden Hundes (Zustand 4,
  grid 0x40) im Savestate lesen.
* **I-3 Routinen 24/25 (@0x80017f50/@0x80017fa4) und 13/19 (@0x80017990/@0x80017d08)** nicht portiert: kein Auslieferungs-
  Raum spawnt Effekt 0x0d (bau_c.md N1.3 Zensus) — unerreichbar; Routine 25 brauchte room_coll + FUN_80012d60 + Kind 0x030C.
* **I-4 Spawn ohne Bank**: `re15_esp_fx_spawn_ex` setzt den Zeitgeber aus Satz[0] nur mit aufgeloester Bank; ein Platz ohne
  Bank (eff_idx < 0) behaelt 0 und faellt im ersten Anim-Schritt wie bisher weg.
* **I-5 BAUPLAN §1.6 Text** (bau_b.md INTEGRATIONSWUNSCH N-2): "HE DEATH[9][0] = 0x80107438" ist durch Spur B (K1) ersetzt:
  HE an der RE2-Zombie-Familie stempelt Spalte Zone + 3 -> DEATH[9][3] = 0x80108530 (Sturz-Tod), Brad HURT[9][3] =
  0x80105438. Gemessen im Integrationstest: RE2-Zombie 0x10 nach HE -> Zeile 9, Leiche ab Bild 180. BAUPLAN selbst nicht
  geaendert (Planungsdokument der RE-Phase).

Uebernommen aus den Spuren (unveraendert offen, Details in den Bau-Dossiers):
* A: Weltlage fuer Flags&0x80 (Anker-Matrix, bau_a OFFEN 1), Liegen-SE-Lage = Stapelrest (E14), Item-Debug-Nebenwirkung
  ITEMALL/MIXITEM (OFFEN 4), Kind-Spawns weiterer Routinen 2/19/25/39/43 ueber den 0x0a-Weg (OFFEN 7), INTEGRATIONSWUNSCH 9
  (`re15_enemy_steer_point` maskiert je Schritt `& 0x0fff`, Lauf-Lenker FUN_8001aac4 nicht — Gegenstueck zu H-1).
* B: N-a Saeure-Spalte (RE2 Op 49 = K1, gebaut K0 nach E6), N-b RE2-Wiederbelebung nach dem Sturz-Tod (~1/8, @0x80108918-9ec)
  fehlt im Port fuer alle Waffen, N-c Setzer von 0x800CFBD8 Bit 0x10000000, OFFEN 1-15 (Hunde-Koerper-Schub mit Original-
  Kasten, Kraehe +0x98, Applier-Zeilen ausser 9/10/11, Part-19-Flug der Spinne, Kakerlake/Alligator/Tyrant-Details, G5-
  Ruettler/Zeilen-Partikel, schlafender RE2-Arm), H1-H8 der Gegenpruefung.
* C: O-C2 Flags-Bit 3 (zweites Zeichnen), O-C3/D Mischreihenfolge innerhalb eines OT-Eimers, O-C4 Paletten-Versatz der Raum-
  TIMs, O-C5 Lage-Flag der SEs (FUN_80045a64), O-C8 Bank-5-Satztor, O-C9 Objekte der Tabelle 0x800af33c ohne Ein-Bild-Licht.
  **O-C1 ist mit W9 geschlossen**: Wurf-Toene im Waffen-Log (TIEF: 5 Abpraller 0x010a0401..0x010a0001 + Liegen 0x010a0001 ->
  ARMS Satz 10), Explosion 0x04080001 -> CORE Satz 8 (nur Art 2), Saeure 0x01130001 -> ARMS10 Satz 10, Brand 0x01120001 ->
  ARMS11 Satz 10 (E9), Licht-Latch `[licht] F119 Latch -> Cut 1 Licht2 Typ 0 Farbe (210,140,80) ... Hell 6000` im
  Explosionsbild.
* D: O-VB2-Formtypen 1..13 (Kreise/Schraegen/Treppen) im Flammen-Bodentest, RE2-Paketpuffer-Ueberlauf (@0x80077e40-54).
* Infrastruktur: D M9 — drei Skills schreiben `taskkill //F //IM re15_pc.exe` vor (`re15-room-probe/SKILL.md:13/:63`,
  `re15-enemy-ai-re/SKILL.md:260`, `re15-pc-render-order/SKILL.md:74`); `scripts/green.sh` erwaehnt den alten Weg nur noch
  im Kommentar. Umstellung auf den Pfad-Filter von local_build.sh = Sache des Orchestrators (Skills sind nicht Teil dieses
  Auftrags).
