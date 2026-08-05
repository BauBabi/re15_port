# BUG: "Nach dem Tod laden → man stirbt sofort wieder" (Laden klappt nicht richtig)

**Datum:** 2026-08-05 · **Status:** REPRODUZIERT (live + deterministische Sonde), Ursache byte-belegt, Fix-Plan steht
**Nutzer-Report:** „Wurde man getötet und lädt das Spiel, stirbt man irgendwie gleich wieder. Das Laden klappt nicht richtig."

**Belege:**
Original — `info/Re1.5/PSX.EXE` (`FUN_800314b0`, `FUN_800396fc`, `FUN_8001c6e8`), `info/Re1.5/PSX/BIN/STAGE1.BIN` (`FUN_8010a6f8`), `ghidra1_V2.txt` (XREF-Zensus), `RE_15_Quellcode_V2/FUN_800314b0.c`.
Port — `re15_port/platform/pc/main.c`, `engine/src/enemy_ai_common.c`, `engine/src/enemy_common.c`, `engine/src/room_common.c`, `engine/src/re15_gameflow.c`, `engine/src/re15_savedata.c`.
Messung — Live-Lauf (`re15_pc.exe`, `RE15_STATE_LOG`/`debug.log`, 5 identische Todes-Zyklen) + `re15_port/tests/unit/probe_load_after_death.c` (engine-only, deterministisch, reproduziert die Live-Zahlen 1:1).

---

## 0. Executive Answer

Der **Spieler-Kommando-Zustand DEVOUR** (Port: `g_player_victim = 2` in `enemy_ai_common.c`, Original: das Kommandoregister `DAT_800aca58 = 6`) **überlebt die Kette Tod → Title → LOAD**, weil der CONTINUE-/Boot-Pfad in `platform/pc/main.c` den Raum-Init-Reset `re15_enemy_reset()` → `re15_player_victim_reset()` **nicht ruft**. Einziger Caller ist `room_common.c:142` = `re15_room_apply_pending` (Tür/Debug-Sprung) — und genau an dem bootet der Lade-Pfad vorbei.

Im ersten getickten Frame nach dem Laden läuft `re15_player_victim_tick()` deshalb in der COLLAPSE-Phase weiter:
1. `re15_clip_root_motion_abs()` setzt die Spielerposition **absolut** aus `anchor_x/anchor_z` — die nach `scd_vm_init()` → `re15_actor_init()` **0** sind → der Spieler landet am **Weltursprung + Clip-Offset**, nicht auf der gespeicherten Position;
2. bei `anim_frame == 0x23` (35) setzt derselbe Tick **`hp = -1`, `state = 7`** (byte-true `FUN_8010a6f8` @`0x8010a7e8`/@`0x8010a80c`) → **zweiter Tod ~1,1 s nach dem Laden** → YOU DIED → Title → (auto-)Load → Endlosschleife.

**HP/Save selbst sind in Ordnung** — die drei anderen Verdachtsflächen aus dem Ticket wurden gemessen und sind ausgeschlossen (§4).

**Zweiter, gleich verursachter Befund:** **NEW GAME direkt nach dem Tod ist genauso kaputt** — `re15_gameflow_new_game()` (`re15_gameflow.c:36-49`) resettet nur die Wund-Tabelle. Gemessen: frisches Spiel in ROOM1240, Tod bei Tick 34 (Sonde M4).

---

## 1. Reproduktion (gemessen, nicht modelliert)

### 1.1 Live (echter Prozess, echter Titel-/Karten-/Boot-Pfad)

Aufbau: Slot-0-Save in ROOM1140 (`re15_card.mcr`, Pos `(-2800,0,-19600)`, hp=100), Lauf mit
`RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_STATE_LOG=state.log`.
Der Spieler wird von einem ROOM1140-Zombie gegriffen und gefressen; danach fährt der Auto-Treiber Title → LOAD GAME → Slot 0.

`state.log` — **5 identische Zyklen** à 296 Frames, jeder Zyklus:

| Segment | F0 | erster Tod |
|---|---|---|
| 0 (erster Load) | `PL(-2800,-19600,rot=-96,hp=100) mo=200` | F220 (echter Devour-Tod, `pst=7`) |
| 1..5 (jeder weitere Load) | `PL(1344,197,rot=-96,hp=100) mo=6` | **F34** `PL(1932,283,…,hp=-1) pst=7` |

`debug.log` derselben Läufe:
```
[boot] player spawn for ROOM1140 = (-7600,0,-17600) yaw=-96
[save] CONTINUE: resumed in room 1140 (hp=100)      <- Restore hat korrekt gearbeitet
[walk] F0  cut=0 mo=6 af=1  frz=0 | pl pos=(1344,0,197) rot=-96   <- Position ZERSTOERT, motion=6
[walk] F30 cut=0 mo=6 af=31 frz=0 | pl pos=(1932,0,283) rot=-96
```
`mo=6` = der Devour-Collapse-Clip (Zombie-Victim-Bank, Variante 0 → `c_collapse = v+6`).

### 1.2 Deterministische Sonde (engine-only, kein SDL)

`re15_port/tests/unit/probe_load_after_death.c` (neu, kein ctest-Gate) fährt exakt die Boot-Reihenfolge aus `main.c` nach (`scd_vm_init()` → Boot-Spawn + `hp=100` → `re15_savedata_restore()`):

```
[M1] nach devour: victim=2 hp=100
[M1] Tod erreicht: victim=2 hp=-1 state=7 pos=(-1953,0) af=35
[M2] nach LOAD: room=1140 pos=(-2800,-19600) hp=100 motion=200 anchor=(0,0) victim=2   <- BUG
[M3] Tick 1: pos (-2800,-19600) -> (1344,197)  motion=6 af=1                            <- Teleport
[M3] nach 60 Ticks: hp=-1 state=7 pos=(1932,283) af=60 | 2. Tod bei Tick 34             <- 2. Tod
[M4] nach NEW GAME (Boot ROOM1240): victim=2 ... Tod bei Tick 34                        <- auch NEW GAME
[M5] nach re15_enemy_reset(): victim=0                                                   <- Fix-Effekt
```
Die Sonde reproduziert die Live-Zahlen **byte-genau** (Tick-1-Position `(1344,197)`, 2. Tod bei Tick 34).

---

## 2. Original — was RE1.5 an dieser Stelle tut (byte-belegt)

### 2.1 Der Spieler-Load nullt das KOMMANDOREGISTER

`FUN_800314b0` (Spieler-/Charakter-Load), Prolog:

```
80031514: lui   at,0x800b
80031518: sw    zero,-13736(at)      ; = sw zero, 0x800aca58   <-- WORT-Null:
                                     ;   aca58 cmd / aca59 Variante / aca5a Phase
...
80031710: ori   v0,zero,0x64
80031714: lui   at,0x800b
80031718: sh    v0,-13586(at)        ; = HP (0x800acaee) := 100
8003171c: lui   at,0x800b
80031720: sh    zero,-13588(at)      ; = Status/Gift (0x800acaec) := 0
```
(Decompilat `RE_15_Quellcode_V2/FUN_800314b0.c` Zeile 14 `_DAT_800aca58 = 0;`, Zeile 55/56.)

`0x800aca58` ist das Register, über das der Spieler-Per-Frame-Dispatch `@0x80073f90[cmd]` läuft:
`0` normal · `2` Hurt/Knockdown · `3` Tod · `5` gegriffen · `6` **gefressen (Collapse)** · `7` Tod-im-Griff.
Die drei Port-Zustände `g_player_victim` / `s_player_grabbed` / `s_knockdown` sind Teilmodelle **dieses einen Registers**.

### 2.2 Einziger Caller = der RAUMLADER

```
ghidra1_V2.txt:126240   FUN_800314b0   XREF[1]: FUN_800396fc:80039788(c)
ghidra1_V2.txt:137604   80039788 2c c5 00 0c   jal FUN_800314b0
```
→ **Jeder Raum-Load im Original löscht das Devour-Kommando.** Der Port-Gegenpart ist `re15_room_apply_pending` (`room_common.c`), das `re15_enemy_reset()` ruft (`room_common.c:142`) — der Boot-/CONTINUE-Pfad in `main.c` tut das nicht.

### 2.3 Das Original löscht es ZUSÄTZLICH beim Verlassen des Karten-Screens

Im Top-Level-Lifecycle `FUN_8001c6e8`, direkt hinter dem Karten-FSM-Aufruf:
```
8001cba4: jal   0x80025c00          ; die SAVE/LOAD-Karten-FSM
8001cbac: ori   v0,zero,0x3
8001cbb4: sb    v0,21337(at)        ; 0x800b5359 := 3  (Post-Card-State)
...
8001cbdc: sb    zero,-13736(at)     ; = sb zero, 0x800aca58   <-- Kommandoregister := 0
```
Das ist genau der Pfad, den der Port als „Title → LOAD GAME → Spiel" nachbaut — auch dort gehört der Reset hin.

### 2.4 Der Devour-Kill, der im Port erneut feuert

`FUN_8010a6f8` (cmd-6-Handler, STAGE1.BIN):
```
8010a7d8: jal   0x8001ad68           ; Clip-Root-Motion-Platzierung (Port: re15_clip_root_motion_abs)
8010a7e4: lbu   v1,-13591(v1)        ; = 0x800acae9  (Spieler +0x95 = anim_frame)
8010a7e8: ori   v0,zero,0x23         ; == 35
8010a7ec: bne   v1,v0,0x8010a824
...
8010a80c: addiu v0,zero,-1
8010a814: sh    v0,-13586(at)        ; = HP (0x800acaee) := -1
```
Der Port modelliert das byte-true (`enemy_ai_common.c`, `kill_fr = 0x23`) — er darf diesen Handler nach dem Laden nur nicht **erneut** laufen lassen.

---

## 3. Port — der genaue Defektpfad

| Schritt | Ort | Was passiert |
|---|---|---|
| Tod | `enemy_ai_common.c` `re15_player_victim_devour` | `g_player_victim = 2`, Typ/Variante/Zombie-Slot gelatcht |
| Tod-Ende | `main.c:4138-4157` | Audio-Stop, `memset(&g_inv)`, `memset(&g_game)`, `mode=TITLE`, `running=0` — **kein** Victim-/Enemy-Reset |
| Title | `main.c:1729 re_title:` … | LOAD GAME → `s_resume_pending=1`, `g_gameflow.start_room = <Save-Raum>`, `mode=INGAME` (`main.c:1886-1897`) |
| Boot | `main.c:2013` RDT-Install → `2396` `re15_apply_room_cinematic` → `2404` `re15_rbj_bind_room` → `2420` `scd_vm_init()` (→ `re15_actor_init()`: memset **aller** Aktoren, `anchor_x/z = 0`) → `2540-2552` Boot-Spawn + `hp=100` | **`re15_enemy_reset()` wird nirgends gerufen** (grep: einziger Caller `room_common.c:142`) |
| Restore | `main.c:2641-2652` `re15_savedata_restore` | Pos/Rot/HP/Status/Inv/Flags/Box/Wunden korrekt (gemessen) |
| Frame 0 | `game_step_common.c:456` | `re15_player_is_grabbed()` → `vs==2` → **GRABBED-Branch** (kein Pad, kein Steuern) |
| Frame 0 | `game_step_common.c:843` → `re15_player_victim_tick()` | COLLAPSE-Zweig: `motion = 6`, `anim_frame++`, `re15_clip_root_motion_abs(anchor 0,0)` → **Teleport an den Weltursprung** |
| Frame 34 | derselbe Zweig | `anim_frame == 0x23` → `hp = -1`, `state = 7` → **2. Tod** |

Die Reset-Funktion existiert und tut das Richtige — sie wird nur nicht gerufen:
```c
/* enemy_ai_common.c:911 — "Reset on room change / death-continue reload (called from re15_enemy_reset)." */
void re15_player_victim_reset(void) { g_player_victim = 0; g_player_victim_type = 0;
                                      g_player_victim_variant = 0; s_victim_phase = 0;
                                      s_victim_fresh = 0;
                                      g_player_victim_zombie = -1; s_grab_mercy_timer = 0; }
```

**Reichweite:** Der Defekt trifft **Grab-/Devour-Tode** (Zombie, Hund, Krähe — der Normalfall in RE). Ein Tod ohne Griff (`g_player_victim == 0`) lädt sauber — gemessen mit `RE15_KILL_AT=60` in ROOM1240: 3 Zyklen, nach jedem Load `F0 hp=100 mo=200` an der gespeicherten Position, kein Früh-Tod.
Eine Variante: Tod **im Griff, aber vor** dem Devour-Latch (`g_player_victim == 1`) → nach dem Laden greift der STRUGGLE-Zweig, sieht `!s_player_grabbed` und geht in den RELEASE-Zweig — ebenfalls mit `re15_clip_root_motion_abs` aus `anchor(0,0)`: kein Tod, aber Teleport an den Weltursprung. (Abgeleitet aus dem Code, **nicht** einzeln gemessen — der Latch-Einstieg ist static und von außen nicht ansteuerbar.)

---

## 4. Ausgeschlossene Verdachtsflächen (gemessen)

| Verdacht (Ticket) | Messung | Ergebnis |
|---|---|---|
| (a) Save speichert einen toten Zustand | Sonde: `sd.player_hp = 100`, `sd.player_status = 0`; Card-Dump slot0 `HP=100` | **ausgeschlossen** |
| (b) HP wird nicht vor dem ersten Tick gesetzt | `debug.log` „resumed in room 1140 (hp=100)"; `state.log` F0 `hp=100` | **ausgeschlossen** |
| (c) Tod-/Game-Over-FSM-Globals bleiben stehen | Nach jedem Load 34 gesunde Frames, kein Sofort-YOU-DIED; `re15_game_step` ruft `re15_gameover_fsm_reset()` sobald `s_go_on && !dead` (`game_step_common.c:315`) | **ausgeschlossen** (selbstheilend) |
| (d) Raum-Setup stompt die HP nach dem Restore | Restore läuft `main.c:2643`, `scd_vm_tick()` erst `2666`; danach schreibt nichts mehr HP | **ausgeschlossen** |

Ebenfalls geprüft: `s_death_seq` (`re15_damage.c:268`) heilt selbst (`!dead → -1`); `g_death_glow` wird nur gelesen, wenn `g_death_blackbg` gesetzt ist (das ist zurückgesetzt) → harmlos.

---

## 5. Fix-Plan

### F1 (Kernfix, minimal-invasiv) — Raum-Init-State-Reset in den Boot-/CONTINUE-Pfad

Port-Gegenstück zu `sw zero,0x800aca58` @`0x80031518` im Raumlader-Callee (`jal` @`0x80039788`) bzw. `sb zero,0x800aca58` @`0x8001cbdc` im Karten-Exit.

**Ort:** `platform/pc/main.c`, unmittelbar **nach** dem RDT-Install (`g_current_room_id = boot_room;`, Zeile 2013 / Ende des `if (rdt_ok)`-Blocks Zeile 2017) und **vor** `pc_load_room_esp(...)` (Zeile 2019).
**⚠ Nicht später einsetzen:** `re15_apply_room_cinematic` (2396) lädt via `pc_enemy_load` bereits Banks und `re15_rbj_bind_room(rbj_buf,…)` (2404) bindet das Raum-RBJ — `re15_enemy_reset()` würde beides wieder wegräumen (`re15_rbj_bind_room(NULL,0)`).

Zwei Varianten:

* **F1a (empfohlen, kleinste Angriffsfläche):** nur die reinen ZUSTANDS-Resets, ohne Bank-Freigabe:
  ```c
  /* Raum-Init-Reset des Spieler-Kommandoregisters — Port-Gegenstueck zu
   * `sw zero,0x800aca58` @0x80031518 (FUN_800314b0, gerufen vom Raumlader
   * FUN_800396fc @0x80039788) bzw. `sb zero,0x800aca58` @0x8001cbdc (Karten-Exit).
   * Ohne den ueberlebt der Devour-Zustand Tod->Title->LOAD/NEW GAME
   * (analysis/bug_load_after_death.md). */
  re15_player_victim_reset();
  re15_crow_flock_reset();            /* 0x800aca50 = 0 bei Raum-Init (FUN_8010d13c) */
  re15_enemy_spawn_count_reset();     /* DAT_800aca4e Raum-Init-Reset (@0x8003f014) */
  ```
* **F1b (vollständig, deckt auch F2 ab):** stattdessen `re15_enemy_reset()` an derselben Stelle — identisch zum Raumwechsel (`room_common.c:142`). Zusätzlicher Effekt: die Model-Banks der Vor-Session werden freigegeben (siehe F2), dafür größere Angriffsfläche (Bank-Free + PC-Texture-Slots).

Beide Varianten decken **LOAD und NEW GAME** ab, weil beide durch denselben Boot-Block laufen.

### F2 (eigener Befund) — Enemy-Bank-Tabelle läuft über die Mode-Zyklen voll

`re15_enemy_reset()` fehlt auf dem Boot-Pfad ⇒ die Model-Banks der vorigen Session bleiben resident. **Gemessen:** 6 Raum-Boots im Live-Lauf, aber nur **3** `[enemy] EM.. loaded`-Zeilen — d.h. Pass 2..6 haben die Banks des ersten Passes weiterbenutzt. `RE15_ENEMY_MAX = 4` (`re15_enemy.h:34`) und `re15_enemy_alloc()` gibt bei voller Tabelle **NULL** zurück (`enemy_common.c:23-36`). Nach einem Tod in einem 3-Typ-Raum (ROOM1140: 0x16/0x10/0x11) und einem Load in einen Raum mit ≥2 anderen Typen ist die Tabelle voll → neue Gegner bekommen **keine Bank** (falsche Clip-Längen/kein Modell). Mit F1b erledigt; mit F1a separat zu fixen (Bank-Drop an derselben Stelle).

### F3 (verwandt, nicht der Melde-Bug) — Knockdown-/Flinch-Statics ohne Reset

`s_knockdown`, `s_hit_flinch`, `s_ev_reach`, `s_prev_hp` (`game_step_common.c:59-94`) sind ebenfalls Teilmodelle von `0x800aca58` und werden von **keinem** Raum-/Lade-Pfad zurückgesetzt (grep: `s_knockdown = 0` nur an der FSM-eigenen Exit-Stelle `game_step_common.c:223`). Tod **während** eines Knockdowns (0x27-Boss-Heavy-Bite) ⇒ nach dem Laden spielt der Knockdown erneut ab: ~2-3 s ohne Kontrolle + Positionsdrift (`kd_move`), aber kein Tod. Sauber wäre ein exportiertes `re15_game_step_player_cmd_reset()` (nullt alle vier), das an derselben Stelle wie F1 **und** in `re15_room_apply_pending` gerufen wird.

---

## 6. Verifikation nach dem Fix

1. `re15_port/build/tests/unit/probe_load_after_death.exe` → erwartet `[M2] … victim=0`, `[M3] … 2. Tod bei Tick -1`, `[M4] … victim=0`, Endzeile „kein Befund".
2. Live-Wiederholung des §1.1-Laufs → `state.log` darf nach dem Load **keinen** F34-Tod mehr zeigen; F0 muss die gespeicherte Position `(-2800,-19600)` und `mo=200` zeigen.
3. Regressionsnetz: voller `ctest --test-dir re15_port/build --timeout 30` (110 Tests), speziell `unit_crow_death_corpse`, `unit_marvin_render_pose`, `unit_stair_1170_regression`, `unit_zombie_10d0_reentry*` (alle hängen an Enemy-Banks / Victim-FSM / Raumwechsel).
4. Sichtprüfung des Tür-Raumwechsels (Skill `re15-port-visual-verify`): F1 darf den `apply_pending`-Pfad nicht verändern — dort läuft der Reset schon.

---

## 7. Offen (ehrlich)

1. Die STRUGGLE-Variante (`g_player_victim == 1` beim Tod) ist **abgeleitet, nicht gemessen** — der Latch (`re15_player_victim_latch`) ist static und von der Sonde nicht direkt ansteuerbar. Messbar über einen Live-Lauf, bei dem der Spieler im Griff (vor dem Devour-Latch) durch einen zweiten Gegner stirbt.
2. Ob das Original beim LOAD zusätzlich `FUN_800396fc` (Raumlader) durchläuft — oder ob der restaurierte GSB erst beim nächsten Raumwechsel wirkt — ist für RE1.5s **dormantes** Save-System nicht abschließend geklärt (siehe `analysis/save_injured_state.md` §3). Für den Fix irrelevant: beide Original-Pfade (Raumlader @`0x80039788`, Karten-Exit @`0x8001cbdc`) nullen `0x800aca58`.
3. F3 (Knockdown/Flinch) ist als Code-Fakt belegt, die Nutzer-sichtbare Folge aber nicht live gemessen.

---

## STATUS 2026-08-06 — BEHOBEN

**Eingebaut** (Variante F1b, an der vom Pruefer bestaetigten Stelle):

- `platform/pc/main.c`, direkt nach dem RDT-Install und VOR `pc_load_room_esp`:
  `re15_enemy_reset()` + `re15_player_cmd_reset()`.
  Port-Gegenstueck zu `sw zero,0x800aca58` @0x80031518 (FUN_800314b0, gerufen vom Raumlader
  FUN_800396fc @0x80039788) bzw. `sb zero,0x800aca58` @0x8001cbdc (Karten-Screen-Exit).
- `engine/src/game_step_common.c`: neue exportierte `re15_player_cmd_reset()` — nullt die
  Knockdown-/Flinch-/Event-Reach-Statics (LAD-3), die ausserhalb von `g_actors` leben und von
  keinem Raum- oder Lade-Pfad angefasst wurden. `s_prev_hp` geht auf 100 (Initialwert).
- `engine/src/room_common.c`: derselbe Aufruf im Raumwechsel, angehaengt an den bestehenden
  Kommandoregister-Block.

`re15_enemy_reset()` statt der drei Einzel-Resets, weil es LAD-2 (nie freigegebene Enemy-Banks)
mit abdeckt. Das vom Dossier befuerchtete Textur-Slot-Risiko besteht nicht: `pc_enemy_load`
leitet den Slot aus dem Bank-INDEX ab (`slot = 11 + (eb - g_enemy)`), ein Re-Load belegt also
dieselben Slots und ueberschreibt sie — keine Slot-Erschoepfung.

**Verifikation**

- `probe_load_after_death` (jetzt ctest `unit_load_after_death`, liefert bei Rueckfall 1):
  `[M2] victim=0`, Spieler bleibt auf der gespeicherten Position (-2800,-19600) statt am
  Weltursprung, `2. Tod bei Tick -1` (vorher 34), `[M4] NEW GAME victim=0`.
- Voller `ctest`: 113/113 gruen (110 vorher + 3 neue Gates).
- Live-Lauf `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`: Boot-Kette unveraendert, insbesondere
  `[rbj] boot room 1150 cinematic overlay: 15 clips` und `[enemy] EM45 loaded ... -> slot 11`
  weiterhin vorhanden (die Reihenfolge-Warnung des Dossiers ist eingehalten).
- Gegen den HEAD-Baseline-Build (eigener Worktree) frame-fuer-frame verglichen: die
  `[walk]`-Spur F0..F300 und alle `[scd] thread start`-Zeilen sind IDENTISCH.

**Offen geblieben:** die STRUGGLE-Variante (Tod im Griff vor dem Devour-Latch) ist weiterhin
nur analytisch abgeleitet, nicht gemessen.
