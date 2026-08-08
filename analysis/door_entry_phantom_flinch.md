# Tür-Eintritt nach Kampf: Phantom-Flinch + drei verwandte Stale-State-Defekte

**Datum:** 2026-08-08 · **Nutzer-Report:** „Wenn ich von einem Zombie getroffen wurde/mit Zombie
gekämpft habe, hat Leon danach nach Türwechsel am Anfang immer eine komische Animation."
**Status:** GEFIXT + gemessen (probe_hitdoor_entry_anim A/B/C), 116/116 ctest grün.
**Research:** 3 Workflow-Lanes (Repro / Original-Disasm / Port-Zensus), Reports im
Session-Log; Kernbelege unten.

## 1. Reproduktion (deterministisch, Engine-only)

`re15_port/tests/unit/probe_hitdoor_entry_anim.c` (Mess-Sonde, kein ctest-Gate) fährt den ECHTEN
Pfad: ROOM1170-Tür → `re15_room_apply_pending` → ROOM1140 (SCD spawnt Zombies, volle
`re15_game_step`-Frames) → Kampfvariante → echter Tür-Scan aus 1140 → ROOM1130, 120 Ticks Log.

| Lauf | vor Fix (Zielraum t=0..19) | nach Fix |
|---|---|---|
| A Kontrolle | mo=211 (stale Sway-Phase) | **mo=200 neutral** |
| B Hit (hp=90) | **mo=10 (Clip 0xa) + 500 Einheiten Rückwärts-Schub ohne Eingabe**, t=19 mo=0-Blitz | **mo=200, d=(0,0)** |
| C Grab+Mash (hp=90) | identisch zu B | **mo=200, d=(0,0)** |

## 2. Ursache (Port-Konstrukt) + Original-Beleg

Der Port-eigene **HP-Drop-Detektor** (`game_step_common.c`, Hit-Eintritt bei `hp < s_prev_hp`)
bekam beim Raumwechsel eine **falsche Baseline**: `re15_player_cmd_reset()` setzte
`s_prev_hp = 100`, während die echte HP den Wechsel byte-true überlebt (einziger HP-Writer =
Session-Start `@0x80031710-18`, FUN_800314b0-Tail; bei Türwechseln übersprungen, Gate
`@0x80039760-8c`). Erster Tick im Zielraum ⇒ `hp(90) < 100` ⇒ fabrizierter Treffer.

**Im Original mechanisch unerreichbar:** Die Phasen-Maschine nullt bei JEDEM Tür-/Raumwechsel das
Kommandoregister — `sb zero,0x800aca58` **@0x8001cbdc** (unconditional im Modus-1/3-Pfad) — und
der cmd-0-Handler `FUN_800318f8` baut den Spieler frisch: Wort-Store `sw 1,0x800aca58`
**@0x8003192c** (wischt +0x05/+0x06), Clip/Frame **@0x80031c10/@0x80031c18**, +0x8F
**@0x80031c20**, +0x93 **@0x80031964**, Aim-/Turn-Bank **@0x8003196c-94**. Hit-Einträge existieren
ausschließlich über Angriffs-Writer (cmd 2, z. B. `@0x80012ebc` FUN_80012d60, `@0x80113b00`
Krähen-Dive). Kein Kampf-Zustand überlebt einen Raumwechsel.

## 3. Die vier Fixes

| # | Defekt | Fix | Beleg |
|---|---|---|---|
| F1 | Phantom-Flinch: `s_prev_hp=100` beim Reset | `s_prev_hp = -1` („keine Baseline", Detektor kann im ersten Tick nicht feuern, re-basiert am Tick-Ende) | @0x8001cbdc + @0x80031710-18 (HP-Writer nur Session-Start) |
| F2 | Flinch-/Knockdown-Exit schrieb `motion=0` → 1-Tick Base-Clip-0-Blitz (gemessen t=19) | Exit lässt Clip stehen; Knockdown-Sonderfall: lief der Schluss-Clip rückwärts (0xb reversed `@0x800363c8/@0x80036638`), Frame:=0 beim Bit-Clear (= derselbe Keyframe) | cmd-2-Exits schreiben NUR das cmd-Wort: `sw 1` @0x80035c80/@0x80035db8/@0x80035f00/@0x80036084/@0x800362f4/@0x8003640c/@0x8003667c — Clip +0x94 unangetastet |
| F3 | Aim-FSM-Statics (`s_player_aim_phase` LOWER/READY) und Idle-Phase überlebten die Tür (Waffe-heben-Pantomime / Sway statt neutral im Zielraum) | `re15_player_cmd_reset()` ruft jetzt `re15_player_aim_interrupt()` + neu `re15_player_idle_reset()`; Idle-Statics auf File-Scope | @0x8001cbdc (cmd-Kill inkl. ACTION-7), @0x8003196c-94 (Aim-/Turn-Bank), @0x8003192c (+0x05/+0x06-Wisch) |
| F4 | Sentinel-Kollision `RE15_MOTION_AIM_W==213==IDLE_HURT1`: One-Shot-Clamp fror den 30-Frame-Injured-Idle (PL00 Clip 22) bei Frame fc(W)-1 ein; stale Aim posierte die W-Bank statt PL00 | AIM_W → **215** (Sentinels sind Port-Konstrukte; getrennte Werte stellen das Original her: Injured-Idle spielt volle 30 EDD-Frames) | PL00.EDD Clip 22 = 30 Frames; Aim lebt im Original im cmd-1/ACTION-7-Zustand, nie im Clip-Register |

Dateien: `engine/src/game_step_common.c`, `engine/src/player_common.c`,
`tests/unit/test_aim_lower_exit.c`, `tests/unit/test_room1140_combat.c` (Sentinel 213→215),
`tests/unit/probe_hitdoor_entry_anim.c` (neu) + CMake-Registrierung.

## 4. Bewusst NICHT in diesem Fix (offen, mit Beleg-Stand)

1. **Flinch-/Knockdown-Clips posieren in RBJ-Räumen aus der Raum-RBJ-Bank** statt der PLD-Basis
   (Original: `FUN_8001f314([0x800acad8],[0x800acbc0])` = Basis-Paar A `@0x800361b4-e0`,
   `analysis/player_knockdown.md` §2). In RBJ-losen Räumen identisch (def==PL00); betroffen z. B.
   ROOM1170. Ob die 1170-RBJ-Clips 0x8-0x10 real von PL00 abweichen, ist **ungemessen** — und der
   saubere Umschalt-Zeitpunkt (Exit-Tick) braucht eigenes Design (`anim_use_pl00`-Lebenszyklus).
2. **Knockdown-Klasse [4]/[5]** bleibt wie gehabt (player_hit_chain.md F4) — separates Ticket.
3. Visuelle gdigrab-Bestätigung im echten Fenster steht aus (Engine-Messung ist eindeutig; bei
   nächster Live-Session mitprüfen — Skill `re15-port-visual-verify`).
