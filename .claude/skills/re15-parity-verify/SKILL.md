---
name: re15-parity-verify
description: >-
  Verify the PC-port's behavior 1:1 against the PSX original by loading the SAME room in both
  and walking the SAME path, so the same input triggers the same behavior. Use this whenever a
  room/movement/combat/presentation change needs proof (not "sieht richtig aus") — e.g. player
  walk/collision, zombie wake/engage/grab, camera cuts, animations. Covers the two harnesses
  (PC RE15_START_ROOM + RE15_INPUT_SCRIPT ; PSX DuckStation debug-menu JUMP + tank-path), the
  byte-true movement model that defines "the same path", the verification checks, and the
  critical DuckStation variable-fps pitfall + how to work around it. Sister skills:
  re15-room-capture (drives DuckStation), re15-savestate-ghidra (reads savestate values).
---

# RE1.5 Parity-Verify — Port ⇄ PSX-Original 1:1

**Goal (Nutzer-Grundanforderung):** das PC-Port-Verhalten MUSS 100 % identisch zum PSX-Original
sein. Beleg = beide Seiten in DENSELBEN Raum laden, DENSELBEN Weg laufen, und prüfen dass DASSELBE
passiert. Divergenz bei gleichem Input = beweisbar ein Port-Bug, kein „faithful-line". Siehe die
Memories [[reai-v2-parity-oracle]] (der Schiedsrichter) und [[disasm-verify-decompiles]] („Mechanismus
belegen, nicht das Ergebnis").

Der Ablauf ist symmetrisch: **denselben Raum, denselben Weg, dasselbe Verhalten** — einmal im Port,
einmal im Original, dann vergleichen.

---

## 1. Beide Seiten in DENSELBEN Raum laden

| | PC-Port | PSX-Original (DuckStation) |
|---|---|---|
| Raum laden | `RE15_START_ROOM=1140` (HEX Raum-ID) auf `re15_pc.exe` | Debug-Menü → JUMP, via `re15-room-capture/scripts/re15_quickload.py --left/--right N` (HEX-Listennummern; **ROOM1140 = `0x114 BRIEFING` = `--left 16`** von der Base `0x124 OPENING`) |
| Spawn prüfen | erste Frame im `RE15_STATE_LOG` | `re15_enemy_state.py <sav>` + der Player-Pos-Reader (`scripts/ppos.py`) |

Der Spawn MUSS in beiden identisch sein (Position + Yaw), sonst divergiert alles danach.
ROOM1140-Spawn (verifiziert): `(x=-7600, z=-17600, rot=-96)` in beiden.

---

## 2. In BEIDEN denselben Weg laufen (Tank-Control)

Tank-Control-Tokens (kommagetrennt `"<Buchstabe><Sekunden>"`): **U/D** = vor/zurück, **L/R** = DREHEN
(kein Strafe), zusätzlich **X** (Cross) gehalten = RENNEN. Beispiel `"R0.3,U2"` = 0.3 s rechts drehen,
2 s vor.

- **PC-Port:** `RE15_INPUT_SCRIPT="R0.3,U2"` (+ `RE15_INPUT_SCRIPT_START=30` Lead-in-Frames, Default 90).
  Deterministisch bei 30 fps: `0.5 s = 15 Frames`. Optional `RE15_STATE_LOG=<pfad>` (per-Tick
  `PL(x,z,rot,hp)` + je Gegner `ss1/grid/mo/af/dist`) und `RE15_AUTOSHOT=1 RE15_AUTOSHOT_SERIES=A,B,STEP`
  (dumped `shots/series_fNNNNN.bmp` je STEP-te Frame in [A,B], beendet den Port am Ende).
- **PSX-Original:** `re15_quickload.py --left 16 --postload 16 --path "R0.3,U2" [--after N] --out room.sav`.
  `--after N` (dieser Skill, 2026-07-03 ergänzt) = N s STILLSTEHEN nach dem Pfad, damit ein geweckter
  Zombie den Spieler noch GREIFEN kann bevor gespeichert wird (nötig für ein Mid-Grab-Frame).

**Was „derselbe Weg" byte-true bedeutet (die Bewegungs-Konstanten, EXE-verifiziert + Nutzer-bestätigt):**
- **Default = WALK** (Richtung allein); **RUN** nur mit gehaltenem **X/CROSS + Richtung** (RE2 UND RE1.5
  rennen NICHT per Default — Nutzer bestätigt 2026-07-03).
- WALK: **speed 0x4b = 75/Frame**, turn **0x30 = 48/Frame**. RUN: speed 200, turn **0x60 = 96/Frame**.
  (Per-Kamera-Tabelle @`0x80073ea4` (walk) / `0x80073ee4` (run); Player-cmd-FSM @`0x80073f90`,
  [0]=`0x800318f8` default-move.)
- Yaw-Konvention: `0 = +X`, forward `= (cos θ, −sin θ)`; `R` erhöht rot (dreht Richtung Bildschirm-LINKS
  im Briefing), `L` verringert. 4096 = 360°.
- Collision: SCA-Push-out (FUN_8003b0a4 = `re15_collision_constrain`), slide entlang Wänden.
  Verifizierte Wand: **x = −5118** (beide Seiten laufen dort auf + sliden +Z).

---

## 3. Prüfen dass DASSELBE passiert

Dieselbe Bewegung → dasselbe Ergebnis. Prüfe (je nach #Bug):
- **Trajektorie**: PL(x,z,rot) über die Zeit (Port: STATE_LOG; Original: Savestates an Wegpunkten).
- **Verhalten**: Gegner-Wake (`ss1 12→2`, dist < 3000), Engage/Turn/Grab (`ss1 = 3/4`), Spieler-HP-Drain,
  Tod (`state 7`), Kamera-Cut, Animation (AUTOSHOT-Frames screen-by-screen).
- **Werte** aus dem Savestate: `re15_enemy_state.py <sav>` (Gegner-Array + Player), `ppos.py <sav>`
  (nur PL x/z/rot/hp), `re15_ss.py <sav> <png>` (Globals + Framebuffer-PNG).

Divergenz → Port-Bug lokalisieren und **byte-true** fixen (Adresse/Bytes zitieren, nicht raten).

---

## 4. ⚠️ KRITISCHE FALLE: DuckStation läuft mit VARIABLER fps — Wall-clock ist NICHT frame-genau

**Die vgamepad-Automation hält Buttons für WALL-CLOCK-Sekunden, aber DuckStation emuliert mit
schwankender, unbekannter fps (pro Lauf verschieden).** Beweis (2026-07-03, 5 Läufe):
- Derselbe `hold U 3s` bewegte nur ~9 Game-Frames×75 (≈9 fps); ein `hold R 2s`-Turn implizierte ~30-60 fps.
  KEINE einzelne fps erklärt beide.
- **Dasselbe `R0.3` ergab über Läufe +96° / +768° / +960° Drehung** → der Spieler landete an 4+
  verschiedenen Stellen (ein Lauf lief zur Tür raus). Präzises Positionieren via `--path` ist so
  UNMÖGLICH.

**Folgen + Workarounds:**
- **Ein „gleicher Pfad" endet an verschiedenen Stellen** = MESS-ARTEFAKT, KEIN Port-Bug. Nicht
  fälschlich als Divergenz werten.
- **Bewegungs-RATEN NIE per Wall-clock messen.** Konstanten stattdessen aus dem Disasm holen (EXE-Tabellen,
  §2) — das ist der byte-true Beleg.
- **TIMING-UNABHÄNGIGE Invarianten vergleichen** (die bleiben gültig): Spawn-Position + Yaw; forward-
  RICHTUNG (`dz/dx`-ratio, unabhängig von der zurückgelegten Distanz); Wand-/Collision-Positionen (beide
  laufen bei x=−5118 auf); die End-Ruhelage NACH einer Wand.
- **Für einen echten frame-genauen Pfad-Vergleich** braucht es deterministischen Input (DuckStation
  Frame-Advance / TAS-Movie). Die vgamepad-Wall-clock-Automation kann das NICHT — nicht mehr Läufe
  reinstecken in der Hoffnung auf Reproduzierbarkeit.
- **Für Combat/Animation im Port** (wo man ZUVERLÄSSIG einen Grab braucht): den PORT deterministisch
  fahren (er IST byte-true, 30 fps) — ein 30-fps-getunter Tank-Path löst den Grab reproduzierbar aus
  (KEIN Teleport — echte Navigation). Verifizierte Port-Pfade ROOM1140 (Spawn -7600,-17600):
  - `R0.3,U2` → Leon (-3700,-19760), zentral + gut sichtbar → 2 Zombies wecken + greifen (Grab F244,
    Tod ~F645). **Beste Kamera-Framing zum Anschauen.**
  - `R0.3,U4` → Leon (200,-21920), tief im Cluster (Kamera-Rand, schlechter sichtbar).

---

## 5. Was byte-true bereits FESTSTEHT (ROOM1140, 2026-07-03)

Timing-unabhängig gegen `stage_saves/mzd_stage1_briefing.sav` geprüft: Spawn, forward-Richtung
(ratio≈0.145 = rot −96), Wand x=−5118 + Slide — ALLE port==original. Konstanten disasm-verifiziert
(§2). **→ Port-Navigation IST byte-true; die scheinbare Pfad-Divergenz war die DuckStation-fps-Falle.**
Der Combat (Grab-Victim-Anim state 5/6) ist per adversarial RE-Workflow byte-true belegt (bank2 =
victim-set via FUN_80022300, PL00-Struktur via FUN_8001e5b0, variant via FUN_80102548) — das ist der
Mechanismus-Beleg, stärker als ein Pixel-Vergleich.

---

## 6. Werkzeuge

- `scripts/ppos.py <sav>...` — liest nur `PL(x, z, rot, hp)` aus DuckStation-Savestates (Player-Block
  `0x800aca54`: x=+0x34, z=+0x3c, rot=+0x6a, hp=+0x9a). Nutzt die `Ram`-Klasse aus re15-savestate-ghidra.
- `re15-savestate-ghidra/scripts/re15_enemy_state.py <sav>` — Gegner-Array + Player-Felder + Labels.
- `re15-savestate-ghidra/scripts/re15_ss.py <sav> <png>` — Globals + Framebuffer-PNG (RAM-Reads
  zuverlässig; VRAM-Decode NICHT — siehe re15-room-capture).
- `re15-room-capture/scripts/re15_quickload.py` — fährt DuckStation (JUMP + `--path` + `--after`).
- `re15-room-capture/scripts/re15_walk_probe.py` — hält EINE Richtung N s (Einzel-Primitiv-Test).

## 7. Ablauf-Checkliste

1. Raum in BEIDEN laden (`RE15_START_ROOM` / `--left N`), Spawn (Pos+Yaw) gleich? → sonst zuerst das fixen.
2. Denselben Tank-Path in BEIDEN fahren.
3. TIMING-UNABHÄNGIGE Invarianten vergleichen (Richtung, Wände, Verhalten) — NICHT Wall-clock-Distanzen.
4. Divergenz? → byte-true Ursache im Disasm/den Daten belegen und fixen. Keine Divergenz bei den
   Invarianten → Port ist parat; für Screen-by-Screen den PORT deterministisch fahren (verifizierte Pfade §4).
5. Ergebnis in [[reai-v2-parity-oracle]] festhalten.
