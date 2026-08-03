# ROOM10D0 — Liegend-Zombie nach Verlassen/Wiederbetreten: RE-Dossier (2026-08-03)

**Nutzer-Report:** "Der Liegend-Zombie in ROOM10D0 verhaelt sich komisch, wenn man den
Raum einmal verlaesst und dann wieder rein kommt, ihn aber vorher noch nicht aktiviert hat."

**Ergebnis in einem Satz:** Nach der Marvin-Cutscene (Flag `(4,247)`, gesetzt NUR von
sub21 +0x07e0, Datei 0x19ec) spawnt der Re-Entry-Zweig von sub00 DENSELBEN Zombie
(kill-flag 0xCD) als **STEHENDEN Schlaefer behavior 0x02** — im Original ein eigener
Grid-Nibble-2-Modus (Handler @0x80101784: decide-Tabelle **@0x8011f960**, deren einziger
Unterschied zu @0x8011f840 der Eintrag `[0] = FUN_80101c7c` ist = Engage bei Arc+LOS+
dist<4000); der Port hat fuer `grid_id & 0xf == 2` nur `default: break`
(enemy_ai_common.c:2621) → der Zombie ist eine **tote Statue** (Clip 0 einmal, Hold-Last
fr=13, keinerlei Reaktion, egal wie nah der Spieler kommt). Der Fix ist minimal, weil
FUN_80101c7c als `re15_ai_decide_approach` (enemy_ai_common.c:263) **bereits portiert,
aber nirgends dispatcht** ist. Scope ist GAME-WEIT: behavior 0x02 ist das
Standard-Muster "Re-Entry-Schlaefer stehend" in **20 Raeumen ueber die Stages 1/2/3/4/5**
(inkl. ROOM1140s eigenem Re-Entry-Zweig).

Dynamisch reproduziert mit `re15_port/tests/unit/probe_zreentry_10d0.c`
(Diagnose-Probe, kein ctest; Szenario A/B, Log unten woertlich).

---

## 1. MESSEN — Port-Repro (probe_zreentry_10d0.exe)

Probe-Ablauf = der echte Tuer-Pfad (Nachbau des Kerns von `re15_room_apply_pending`,
room_common.c:100-171: Aktor-Wipe mit hp/status-Erhalt, Motion-Reset,
`re15_enemy_reset`, Spieler an Tuer-Spawn, `scd_room_reenter`; `g_game.flags`
persistieren): 10D0 rein (Tuer-15-Spawn 7650/11400, fern) → 240 Ticks → raus nach
ROOM10C0 (Tuer 15, main00 +0x196: next=(-6700,0,-2100)) → 120 Ticks → wieder rein →
360 Ticks fern + 600 Ticks nah (5878/24000, dist 1693 < 0xBB8).

### Szenario A — ohne Cutscene (`Ck(4,247)==0` bleibt): PORT BYTE-TRUE ✅

```
[flags nach Re-Entry ] Ck(4,247)=0 Ck(3,50)=0 em-zone7 bit205=0
  slot=1 type=0x40 grid=0x40 …  (MARVIN wieder da)
  slot=2 type=0x10 grid=0x0E st=0 mo=42 pos=(5878,0,25694)
Phase 2 (fern):  st=1 s1=0x12 s2=1 grid=0x00 mo=42 fr=0     (liegt, schlaeft, Frame 0)
Phase 3 (nah):   s2 3→4 (0x2A spielt durch), s1=0x0d Clip 0x29 (59f), s1=0x02 ENGAGE,
                 dann Approach/Grab-Maschine — identisch zur Erst-Betretung.
```

### Szenario B — Cutscene gesehen (`Set(4,247)=1` wie sub21 +0x07e0): **BUG REPRODUZIERT** ⛔

```
[flags nach Re-Entry ] Ck(4,247)=1
== Roster Re-Entry ==            (ELSE-Zweig: KEIN Marvin — korrekt)
  slot=1 type=0x10 grid=0x02 st=0 s1=0x00 mo=0 emflag=0xcd pos=(5878,0,25694) rot=1256
== Phase 2 fern ==
    0 Z2  st=1 s1=0x00 s2=0 grid=0x02 mo=0 fr=1
  240 fr  st=1 s1=0x00 s2=0 grid=0x02 mo=0 fr=13            <- Clip 0 (14f) einmal, HOLD
== Phase 3 nah (dist=1693 < 0xBB8, sogar < 4000) — 600 Ticks ==
  480 fr  st=1 s1=0x00 s2=0 grid=0x02 mo=0 fr=13            <- KEINE Reaktion, NIE Engage
```

Der Zombie steht als eingefrorene Puppe an der Liegeposition: `s2` bleibt 0 (nicht mal
der Idle-Seed der animate-Maschine laeuft), kein Engage, kein Atmen-Loop. Genau das
Nutzer-Symptom: er wurde nie aktiviert, ist beim Wiederbetreten ploetzlich AUFRECHT
(das ist original-korrekt!) — aber verhaelt sich "komisch", weil er komplett tot ist.

---

## 2. ORIGINAL — die komplette Re-Entry-Kette (Disasm-Belege)

### 2.1 sub00: beide Ck-Zweige byte-exakt (ROOM10D0.RDT, sub_scd @0x120c)

```
+0x2e f00123a  Ifel_ck -> +0x94 ; +0x32 Ck(4,247)==0
IF (Erst-Betretung):
  +0x36 f001242  Aot_set slot=18 sce=3 rect x=4700 z=19400 w/d=800  Evt `ff 18 15` (sub21)
  +0x4a f001256  44 00 40 40 00 00 00 ff | Sce_em_set MARVIN type=0x40 beh=0x40 p7=0xFF
  +0x5e f00126a  44 01 10 0e 00 00 00 cd | Zombie type=0x10 beh=0x0e p7=0xCD (5878,0,25694) yaw=1256
ELSE (+0x94 Else_ck -> +0xd6; Ck(4,247)==1):
  +0x98 f0012a4  44 00 10 02 00 00 00 cd | DERSELBE Zombie: type=0x10 beh=0x02 p7=0xCD,
                 GLEICHE Pos (5878,0,25694), GLEICHES yaw 1256, GLEICHER kill-flag 0xCD
  + 5x Sca_id_set / 5x Sca_floor_set (Kamera-/Floor-Umbau; AI-irrelevant)
```

(Korrektur zu marvin_10d0.md §1.1: der ELSE-Zombie steht an der ZOMBIE-Position, nicht
"an Marvins Stelle".) `Set(4,247)=1` existiert game-weit NUR in ROOM10D0 (Datei 0x19ec,
sub21 +0x07e0, hinter `Message_on(0x0b)`+`Ck(12,31,0)`) und ROOM10D1 (0x19d6) —
Byte-Scan `22 04 f7 01` ueber alle 240 RDTs: 2 Treffer.

### 2.2 Das em_set-Spawn-Gate (PSX.EXE @0x80042120-58, raw verifiziert)

```
800420f8 andi s4,v0,0x7f ; andi s1,v0,0x80   ; slot-Byte pc[1]: &0x7f=Index, &0x80=Skip-Bit
80042120 lbu a1,7(a1)                        ; pc[7] = kill-flag-Index
80042128 beq a1,0xff -> 8004215c             ; 0xFF = kein Persist-Gate -> spawn
80042130 jal 0x8004efe4                      ; flag GET (em-status-Bank 0x800b1038-Scheibe)
80042138 beq v0,zero -> 80042154             ; Flag CLEAR -> weiter
80042140 lbu a0,454(s0) ; jal 0x80039b2c     ; Flag SET: Cleanup ueber +0x1c6, KEIN Spawn
8004214c j 0x80042620 ; ori v0,1
80042154 bne s1,zero -> return               ; slot&0x80 -> Init ueberspringen
8004215c lbu pc[3] -> sb +0x9                ; behavior -> grid  (Init-Pfad)
8004216c sh zero,452(s0)                     ; +0x1c4 = 0
80042170 sb pc[7],454(s0)                    ; +0x1c6 = kill-flag-Latch
```

→ Beim Wiederbetreten spawnt ein Record NUR dann NICHT, wenn sein kill-flag (pc[7])
gesetzt ist (Zombie getoetet). "Anders" spawnen macht nie das Gate, sondern der
SCD-Ck-Zweig (§2.1). Port-Aequivalent scd_vm.c:2929-2934 — identisch.

### 2.3 INIT FUN_80100688 fuer behavior 0x02: state=1, grid BLEIBT 2

- `sb 1,4(v1)` @0x80100704 — INIT setzt +0x4 = **State 1** (unconditional, am Anfang).
- Steer-Ziel = Spieler @0x8010071c/0x80100734 (+0x1bc/+0x1be); HP aus Tabelle
  @0x8011f034+type*0x20 (rng&0xf); +0x94=0/+0x95=0/+0x8f=0 @0x80100c58-c78;
  ein anim_set(+0x84,+0x16c,0,0x100) @0x80100c90.
- Selector-Kette: das 0x80-Pose-Paket ist hinter `andi 0x80; beq -> 0x80100e30`
  @0x80100cac-b0 versperrt; danach NUR sel 6 (@0x80100e48, word 0x20c01), sel 0xB
  (@0x80100ecc, pose), sel 0xD (@0x80100f2c, word 0x201), sel 0xE (@0x80100f70,
  word 0x1201). **sel==2 matcht NIRGENDS: kein word-Write, und +0x9 wird NICHT
  geloescht** (Clear nur in den Zweigen 6/0xD/0xE @0x80100eb0/0x80100f54/0x80100fd4).
  → Der stehende Re-Entry-Zombie lebt dauerhaft mit `+0x9=0x02` → **Grid-Nibble 2**.

### 2.4 Grid-Nibble-2-Handler @0x80101784 (State-1-Dispatch @0x8011f80c[2])

```
80101794 lbu v0,5(v0)              ; +0x5 (sub)
801017a4 addiu at,at,-1696        ; DECIDE-Tabelle @0x8011f960   ★ (nibble 0 nutzt 0x8011f840)
801017b4 jalr v0
801017c8 lbu v0,5(v0)
801017d8 addiu at,at,-1904        ; ANIMATE-Tabelle @0x8011f890  (identisch zu nibble 0)
801017e8 jalr v0
```

**Tabellen-Diff (beide 20 Eintraege aus STAGE1.BIN gedumpt): @0x8011f960 ==
@0x8011f840 fuer [1..19]; einziger Unterschied `[0]`:**

| | [0] | [1] | [2] | … [13] | [18] |
|---|---|---|---|---|---|
| @0x8011f840 (nibble 0) | **0x80101b64** (search+Timer) | 0x80101de4 | 0x80102058 | 0x80104a48 | 0x80105470 |
| @0x8011f960 (nibble 2) | **0x80101c7c** (dormant-stand) | 0x80101de4 | 0x80102058 | 0x80104a48 | 0x80105470 |

### 2.5 decide[0] FUN_80101c7c — der Aufwach-Trigger des stehenden Schlaefers (raw)

```
80101c8c jal 0x8001a9cc ; a0=&playerX ; a1=0x5f4   ; arc_test: Spieler im ±0x5f4-Frontbogen?
80101c94 sll v0,16 ; bne -> ret                    ; ausserhalb -> nichts
80101cac lhu v0,472(v1) ; andi 0x10 ; beq -> ret   ; +0x1d8 & 0x10 (LOS "player visible")
80101cc0 lw v0,464(v1) ; sltiu v0,v0,0xfa0         ; dist-Cache +0x1d0 < 4000
80101ccc beq -> ret ; ori v0,0x201
80101cd4 jal rng ; sw v0,4(v1)                     ; word +0x4 = 0x201 ENGAGE (Delay-Slot!)
80101cdc andi v0,7 ; bne -> ret ; ori v0,0x801
80101cf4 sw v0,4(v1)                               ; 1/8: word = 0x801 CHARGE
```

Kein Timer, kein Wander, kein Attack-Commit — anders als nibble-0-decide[0]
FUN_80101b64 (Timer +0x9c → 0x101, dist<2000&&arc(0x2c8) → 0x701, Engage nur
dist>10000-Charge-Variante). Nach dem Engage laufen alle weiteren Subs ueber die
[1..19]-identischen Tabellen — der Zombie wird ein normaler Kampf-Zombie
(sein Nibble bleibt 2, die Tabellen sind deckungsgleich).

### 2.6 animate[0] FUN_80101d08 — der Idle-Stand (raw; von BEIDEN Nibbles geteilt)

```
80101d20 bne +0x6,zero -> 80101d94       ; Phase-Gate
80101d40 sh (rng&0x3f)+300 -> +0x9c      ; Idle-Timer (nur der nibble-0-decide liest ihn)
80101d50 sb 1,6 ; 80101d60 sb 0,148      ; +0x6=1, Clip 0
80101d70 sb 0,149 ; 80101d80 sb 0xf,143  ; Frame 0, +0x8f=0xf (Crossfade 15)
80101d90 sb 0,147                        ; +0x93 = 0
80101da8 jal 0x8001f314 ; a0=+0x84 a1=+0x16c a2=0 a3=0x100   ; JEDEN Tick: Bank-0-Stepper
80101dc4 (+0x8f==0) -> jal 0x8010939c(0,1)                   ; Foot-/Anim-SFX-Hook
```

---

## 3. PORT-IST — die Divergenz (statisch + dynamisch belegt)

### D1 (URSACHE) — der AI-Dispatch hat keinen `case 2`

`enemy_ai_common.c:2508 switch (e->grid_id & 0xf)`: case 0 (voll), case 5/6 (feeding),
case 7/8 (1140-Lyer) — **Nibble 2 faellt in `default: break` (:2621)**. decide laeuft
nie, animate laeuft nie: `s2` bleibt 0 (nicht mal der Seed @FUN_80101d08-Aequivalent
`re15_enemy_ai_live_search_stand` :1414 wird erreicht), der globale Advancer spielt
Clip 0 (14 Frames) einmal und haelt fr=13. Messung §1 Szenario B.

### D2 (IRONIE) — der byte-true decide ist schon da, aber toter Code

`re15_ai_decide_approach` (enemy_ai_common.c:263-274) ist die exakte Portierung von
FUN_80101c7c (arc 0x5f4 + `ai_flags&0x10` + dist<4000 → 0x201, 1/8 → 0x801) — **kein
einziger Caller** (grep: nur Definition + Header). Auch der Header re15_enemy_ai.h:192
dokumentiert die f960-Tabelle bereits korrekt ("same table with [0]=FUN_80101c7c").
Der LOS-Sensor, der `+0x1d8`-Bit 0x10 pflegt, laeuft im Port bereits pro Tick
(:2480-2488, `re15_enemy_los_probe`).

### D3 (KEIN Fehler) — INIT und Spawn-Gate sind byte-true

Port-INIT laesst fuer sel 2 `grid_id=0x02` stehen und setzt mo=0/state 1 — exakt das
Original (§2.3; das fruehere zlyer-Muster "grid muss 0 werden" gilt NUR fuer die
Clear-Zweige 6/0xD/0xE). Das kill-flag-Gate (scd_vm.c:2929) und der ELSE-Zweig-Lauf
(Roster §1 B: kein Marvin, Zombie slot 1, emflag 0xCD) stimmen.

### Scope — behavior 0x02 ist GAME-WEIT das Re-Entry-Schlaefer-Muster

SCD-Byte-Scan (Sce_em_set, Zombie-Typen 0x10/11/12/16/18, `beh&0x9f==2`) ueber alle
240 RDTs — 91 Records in 20 Raeumen (inkl. Alt-Spieler-Varianten):

- STAGE1: 1090/1091 (je 4), 10B0/10B1 (je 4, p7=0xFF!), **10D0/10D1 (je 1)**,
  10E0/10E1 (je 4), **1140 (5)**, 1220/1221 (je 8)
- STAGE2: 2020/2021 (je 4) · STAGE3: 30D0/30D1 (je 8), 30E0/30E1 (je 2)
- STAGE4: 40B0/40B1 (je 2) · STAGE5: 5120/5121 (je 2), 5140/5141 (je 4)

ROOM1140 in-context verifiziert (sub00 +0x78 `Ck(3,210)`: IF = 0x88+4x0x86 Erstbesuch
[= die Tableau-Zombies], ELSE = **dieselben 5 kill-flags 0xd3-0xd7 als beh 0x02 an
verstreuten Positionen**) — d.h. auch der 1140-Re-Entry ist im Port heute eine
5-Statuen-Galerie; `test_room1140_spawn` prueft nur den IF-Zweig. 10B0 traegt p7=0xFF
(kein Persist) — dort koennten 0x02-Spawns sogar Erstbesuch sein (nicht disasm-verifiziert).

---

## 4. FIX-PLAN (mit @0x-Zitaten; KEIN Engine-Code in dieser Diagnose geaendert)

1. **`case 2` in den Dispatch** (enemy_ai_common.c:2508) — teilt den case-0-Body,
   einziger Unterschied ist decide[0] (Tabellen-Diff §2.4):
   ```c
   case 2:   /* @0x8011f80c[2]=FUN_80101784: decide @0x8011f960[+0x5] (@0x801017a4),
              * animate @0x8011f890[+0x5] (@0x801017d8). f960==f840 ausser [0]:
              * FUN_80101c7c statt FUN_80101b64 (Dump §2.4). */
   case 0: {
       re15_actor_t *player = &g_actors[RE15_ACTOR_SLOT_PLAYER];
       if ((e->grid_id & 0xf) == 2 && e->sub_state_1 == 0)
           re15_ai_decide_approach(e, player);   /* f960[0]=FUN_80101c7c @0x80101c8c-cf4 */
       else
           re15_ai_dispatch_decision(e, player); /* f960[1..]==f840[1..] (identische Ptr) */
       … bestehende animate-Kaskade UNVERAENDERT …
   }
   ```
   Damit erreicht sub 0 den vorhandenen Idle-Seed `re15_enemy_ai_live_search_stand`
   (:1414 = FUN_80101d08 @0x80101d40-90) und jeder Folge-Sub (Engage 2, Approach 0x13,
   Grab 3/4, …) exakt dieselben bereits byte-true portierten Handler wie Nibble 0.
2. **Nichts am INIT/Spawn aendern** — sel 2 ist byte-true "kein Decoder-Zweig, grid
   bleibt" (§2.3, D3).
3. **Kommentar :2621** («[1..4],[9..15] deferred») auf «[1],[3..4],[9..15]» korrigieren.

**Verifikation danach:** `probe_zreentry_10d0.exe B` — erwartet: Re-Entry-Roster
unveraendert; Phase 2 fern: `s1=0 s2=1` (Seed: Timer, Clip 0, +0x8f=0xf), KEIN Engage
(arc erfuellt, aber dist 14372 > 4000); Phase 3 nah: `word 0x201` sobald LOS-Bit 0x10
steht (bzw. 1/8 `0x801`), dann Engage→Approach→Grab wie Szenario A. Szenario A muss
identisch bleiben (Regression). Danach 1140-Re-Entry-Probe (5 Zombies analog) und
Live-Verifikation per gdigrab (Skill re15-port-visual-verify): 10D0 betreten →
Cutscene → raus → rein → Zombie steht und greift an, statt einzufrieren.

---

## 5. OFFEN (ehrlich)

1. **Clip-0-Loop vs Hold im Idle-Stand:** FUN_80101d08 ruft f314 JEDEN Tick
   (@0x80101da8, rate 0x100 = halbe Rate) — ob f314 den Clip am Ende WRAPPT (Atmen-
   Loop) oder haelt, ist nicht RE'd; der Port-Advancer haelt (Play-once-Hold). Betrifft
   auch den bestehenden case-0-sub-0-Pfad. Nach dem Fix messen (fr-Verlauf ueber
   >28 Ticks); ggf. f314 @0x8001f314 disassemblieren.
2. **LOS-Probe-Erfuellung im Probe-Setup** nicht gemessen (Szenario B kam nie bis zum
   decide) — ob `re15_enemy_los_probe` in 10D0s Kollisionsdaten das Bit 0x10 setzt,
   zeigt erst der Post-Fix-Lauf.
3. **Schiessbarkeit/HURT der Statue bzw. des Fix-Zombies** ungetestet (Damage-Router
   ist state-, nicht grid-gekeyt — erwartet OK).
4. **Nibbles 1/3/4 (@0x8011f80c[1]=0x80101708, [3]=0x80101800, [4]=0x8010187c)**
   weiterhin nicht portiert — kein bekannter Spawn nutzt sie (Scan fand nur sel 2);
   Census dazu nicht abgeschlossen.
5. **Wer setzt (3,210) fuer 1140 bzw. die Gates der uebrigen 18 Raeume** — nicht
   verfolgt (fuer 10D0 irrelevant; Gate dort ist (4,247), §2.1).
6. **10B0 p7=0xFF-Records**: Zweig-Kontext (Erstbesuch?) nicht disasm-verifiziert.
7. Szenario A ist nur STATE-seitig verifiziert (Probe); Render-Seite des A-Pfads nicht
   gemessen (kein Hinweis auf Divergenz).

## 6. Artefakte

- Probe: `re15_port/tests/unit/probe_zreentry_10d0.c` (+ CMake-Eintrag, kein ctest;
  `A`/`B` als argv). Logs: Scratchpad `zre_A.txt` / `zre_B.txt`.
- SCD-Dumps: Scratchpad `scd_dump.py` / `scd_10d0_all.txt` (main00 + alle 23 Subs,
  Groessen = byte-true `s_opcode_sizes` aus scd_vm.c).
- Disasm: `re15_disasm.py` gegen STAGE1.BIN (FUN_80100688-Decoder, 0x80101784,
  Tabellen 0x8011f80c/0x8011f840/0x8011f890/0x8011f960, FUN_80101c7c, FUN_80101d08)
  und PSX.EXE (em_set-Gate @0x80042120-58); Decompiles STAGE1_full
  (FUN_80100688/80101b64/80101c7c/80101d08).
