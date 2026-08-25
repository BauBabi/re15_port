# ROOM1010 — „die Zombies kriechen im Original" (FINDING 2, RE2-KI)

**Stand:** 2026-08-24 · **Phase:** RE + Messung, **KEINE Code-Änderung**
**Nutzer-Report:** *„room 1010 die Zombies kriechen im original"* (gemeldet unter RE2-KI)

---

## 0. Urteil

**Der Nutzer hat recht — und zwar nachweisbar aus den Bytes, nicht aus dem Eindruck.**

ROOM1010 spawnt in **einem** seiner beiden Eintritts-Szenarien zwei Zombies mit dem
Sce_em_set-Deskriptor **`0x81`**. Bit `0x80` + Low-Nibble `1` ist im RE1.5-Original **exakt
derselbe Wert, den der ROOM1030-Kriech-Commit zur Laufzeit schreibt** (`ori v0,zero,0x81` /
`sb v0,9(v1)` @0x801050D0-D4). Nibble 1 ist die **Grid-Wurzel 1 = die Kriech-Maschine**
(`@0x8011f80c[1] = FUN_80101708`). ROOM1010 hat diese Zombies also **von Geburt an als
Kriecher im RDT stehen** — kein Skript-Trigger nötig.

**Der Port zeigt in ROOM1010 in KEINEM der beiden KI-Modi einen Kriecher:**

| Modus | gemessenes Port-Verhalten in ROOM1010 (Eintritts-Cut 4) | Original |
|---|---|---|
| **RE1.5** | `+0x05 = 5` **für immer**, Clip `0x0C`, Anim-Frame 0, Position unverändert über 700 Frames — auch bei Spieler-Distanz 849 (< jede Schwelle). **Vollständig eingefroren.** | ab dist < 3000 → `+0x04 = 1` → Kriech-Lokomotion (Clip `0x1A`), SCA-Zeile 8 |
| **RE2** | Liege-Executor `EXEC[7]`, `+0x10E = 0x4002`; bei dist < 3000 **STEHT ER AUF** (`grid 0x81 → 0x00`, `+0x05 → 1 = WALK`) und läuft aufrecht | dito Kriecher |

Ursache ist **eine einzige, bereits im Port als OFFEN markierte Lücke**: die Zeile **5** der
beiden Grid-1-Tabellen (`DECIDE @0x8011F8E0[5] = 0x8010466C`, `ANIMATE @0x8011F920[5] =
0x80104808`) ist nicht portiert. Genau in diese Zeile setzt der Original-INIT jeden
`0x81`-Spawn.

Der Befund gilt **in beiden Modi**, mit unterschiedlichem Symptom (RE1.5: Standbild;
RE2: falsches Aufstehen). Details in §5.

---

## 1. Was steht wirklich in ROOM1010.RDT (Byte-Scan, eigener Walker)

Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM1010.RDT`, Größe `0x29368`
Header: `main_scd = 0x08AC`, `sub_scd = 0x092C`; `sub00` liegt bei Datei-Offset **0x0930**.

Vollständiger `sub00` (Opcode-Walker über die byte-true Längentabelle aus
`scd_vm.c:166-215`):

```
0x0930  Obj_model_set  id=0x00  pos=(200,-1600,5500)  rot=(0,3084,0)
0x0952  Obj_model_set  id=0x01  pos=(1800,-1600,5750) rot=(0,2513,0)
0x0974  Obj_model_set  id=0x02  pos=(-1800,-1600,-1900) rot=(0,3084,0)
0x0996  Item_aot_set   slot 2
0x09AC  Item_aot_set   slot 3
0x09C2  Item_aot_set   slot 4
0x09D8  13 0a 62 00                 Switch  work_vars[0x0A]   (= EINTRITTS-CUT), Blocklänge 0x62
0x09DC  14 00 2a 00 00 00           Case 0   (Blocklänge 0x2A)
0x09E2    44 00 10 00 …   Sce_em_set slot=0 type=0x10 beh=0x00 pos=( 3750,0, 5000) dirY=3072
0x09F6    44 01 10 00 …   Sce_em_set slot=1 type=0x10 beh=0x00 pos=( 1200,0, 7150) dirY=0
0x0A0A    Break
0x0A0C  14 00 2a 00 04 00           Case 4   (Blocklänge 0x2A)
0x0A12    44 02 10 81 …   Sce_em_set slot=2 type=0x10 beh=0x81 pos=(  950,0,-1700) dirY=0
0x0A26    44 03 10 81 …   Sce_em_set slot=3 type=0x10 beh=0x81 pos=(  -50,0,-3800) dirY=3584
0x0A3A    Break
0x0A3C  Eswitch            ← KEIN Default: jeder andere Eintritts-Cut spawnt GAR NICHTS
0x0A3E  Evt_end
```

Feld-Layout von `Sce_em_set` (0x44, 20 B) laut `scd_vm.c:3113-3130`:
`[1]=slot [2]=type [3]=behavior [4]=floor/soundbank [6]=kill-flag [7]=spawn-flag
[8..9]=x [10..11]=y [12..13]=z [16..17]=dirY` (alle LE).

**Case-Dekodierung belegt:** `op_switch` (byte-true `LAB_8003fa5c`, `scd_vm.c:1555-1597`) liest
den Case-Wert als `s16 LE @ a3+4` (@0x8003fb00 `lh`). `14 00 2a 00 **00 00**` → Wert 0,
`14 00 2a 00 **04 00**` → Wert **4**.

### Welcher Eingang liefert Cut 4?

`work_vars[0x0A]` = der **Eintritts-Cut der durchschrittenen Tür**
(`FUN_8001d600`: @0x8001d930 `lbu v1,10(a0)` → @0x8001d948 `sh v1,4068(at)`;
Port-Zwilling `scd_room_setup.c:218`, gefüttert aus `g_room_change.target_cut` =
`op_door_aot_set` `t->pc[24]`, `scd_vm.c:3374`).

Eigener Tür-Scan über alle 240 RDTs, Ziel `stage=0 / room=0x01` (= ROOM1010):

```
ROOM1020.RDT  main00@0x1C82  Door slot 0  -> CUT 4  Ziel (3650,0,-3950)  yaw=2048
ROOM1020.RDT  main00@0x1CA2  Door slot 1  -> CUT 0  Ziel (3650,0, 6900)  yaw=2048
ROOM1021.RDT  identisch
```

ROOM1010 ist also ein Gang mit **zwei** Eingängen aus ROOM1020:
* **Nordtür (Cut 0)** → Ziel z=+6900 → Case 0 → die zwei **aufrechten** Zombies (z=5000/7150).
* **Südtür (Cut 4)** → Ziel z=−3950 → Case 4 → die zwei **Kriecher** (z=−1700/−3800).

Die Kriecher liegen also genau in der Hälfte, in die die Südtür führt — Entfernung Tür→Kriecher 1
= 3515 Einheiten (gemessen im Port: `dist=3510`), also **knapp außerhalb** der Aufwach-Schwelle
3000: der Spieler betritt den Raum, macht ein paar Schritte, und **dann** setzen sich die beiden
in Bewegung. Das ist die Inszenierung, die der Nutzer beschreibt.

### Deskriptor-Zensus (eigener Walker, alle 240 RDTs, 844 Sce_em_set)

```
0x00:251  0x01:5  0x02:92  0x03:2  0x04:2  0x05:64  0x06:8  0x0D:142  0x0E:2  0x10:14
0x21:1  0x24:1  0x30:4  0x33:7  0x40:93  0x41:32  0x61:16  0x81:12  0x83:2  0x84:28
0x85:2  0x86:4  0x87:20  0x88:33  0x90:4  0xA1:2  0xAE:1
```

Die **12 `0x81`** + **2 `0x83`** (= gleiche INIT-Familie `sel ∈ {1,3}`) verteilen sich auf:

| Raum | Slot | Typ | beh | Position |
|---|---|---|---|---|
| ROOM1010 / ROOM1011 | 2,3 / 3,4 | 0x10 | 0x81 | (950,−1700) / (−50,−3800) |
| ROOM1220 / ROOM1221 | 0,1 | 0x16 | 0x81 | (−25000,−6700) / (−23500,−9700) |
| ROOM4050 / ROOM4051 | 0 | 0x18 | 0x81 | (−12300,−10750) |
| ROOM5060 / ROOM5061 | 4 | 0x18 | 0x81 | (−6681,−13437) |
| ROOM3010 / ROOM3011 | 6 | 0x11 | 0x83 | (−10316,−3946) |

Die Deskriptoren `0x01`/`0x03` **ohne** Bit 0x80 stehen game-weit nur auf **Nicht-Zombie-Typen**
(0x26 Feuer-Trümmer in ROOM1090, 0x24 in ROOM20B0/B1) — für die Zombie-Familie ist `0x81`/`0x83`
der einzige Weg in die Grid-Wurzel 1 per Spawn.

---

## 2. Welches Feld macht aus einem Zombie einen Kriecher — RE1.5 (byte-belegt)

Kette Deskriptor → Kriecher, jede Stufe selbst aus `info/Re1.5/PSX/BIN/STAGE1.BIN` (RAW
@0x80100000) disassembliert:

### (a) Sce_em_set schreibt den Deskriptor nach `entity+0x09`
`FUN_800420a0` (Port-Zwilling `scd_vm.c:3253` — `a->grid_id = behavior`).
`+0x09` trägt gleichzeitig **Sub-Dispatch-Nibble** (`&0x0F`), **Pose-Selektor** (`&0x1F`)
und **Flag-Bits** (`0x80`).

### (b) INIT (Zustand 0) dekodiert `sel = +0x09 & 0x1F` unter dem 0x80-Gate

```
@0x80100ca4  lbu v1,9(v0)              ; Deskriptor
@0x80100cac  andi v0,v1,0x80
@0x80100cb0  beq  v0,zero,0x80100e30   ; Bit 0x80 CLEAR -> ganzer Liege-Block übersprungen
@0x80100cb4  andi v1,v1,0x1f           ; sel
…
@0x80100d3c  andi v1,v0,0x1f
@0x80100d40  ori  v0,zero,0x1
@0x80100d44  beq  v1,v0,0x80100d54     ; sel == 1
@0x80100d48  ori  v0,zero,0x3
@0x80100d4c  bne  v1,v0,0x80100d6c     ; sel != 3 -> überspringen
@0x80100d54  ori  v0,zero,0xc
@0x80100d58  sb   v0,148(a0)           ; +0x94 = 0x0C   (Liege-/Kriech-Ruhepose)
@0x80100d64  ori  v0,zero,0x5
@0x80100d68  sb   v0,5(v1)             ; +0x05 = 5      <<< DIE SUB-ZEILE
```

Die Familien `{4,7,9}` (→ Endclip 0x12) und `{5,8,0xA}` (→ 0x13) bekommen **nach** dem
gemeinsamen `f314` @0x80100da0 noch einen FINAL-Clip; **`{1,3}` bekommt keinen** und behält
`0x0C` **plus** `+0x05 = 5`. Nur diese Familie trägt eine Sub-Zeile — das ist der
Unterschied zwischen „liegt herum" und „ist ein Kriecher".

### (c) Der Per-Frame-Dispatch landet in der Kriech-Maschine

`@0x80072bac[type]` → `FUN_80100424` → Zustandstabelle `@0x8011f7b4[+0x04]`:

```
[0] 0x80100688 INIT   [1] 0x80101224 ACTIVE   [2] 0x80105a8c HURT   [3] 0x80106ba4 DEATH …
```

ACTIVE-Tail → Sub-Modus-Tabelle `@0x8011f80c[+0x09 & 0x0F]`:

```
[ 0] 0x8010168c   (Standard-Kampf: DECIDE @0x8011f840 / ANIMATE @0x8011f890)
[ 1] 0x80101708   <<< GRID-WURZEL 1 = KRIECH-MASCHINE
[ 5][ 6] 0x801018f8 (feeding)   [ 7][ 8] 0x80101974 (liegend)   [ 9][10] 0x801019f0 (skript-geweckt)
```

`FUN_80101708` (selbst disassembliert) ist ein **Doppel-Dispatch** auf `+0x05`:

```
@0x80101718  lbu v0,5(v0)
@0x80101728  addiu at,at,-1824   -> 0x8011f8e0   ; DECIDE-Tabelle
@0x80101738  jalr v0
@0x8010174c  lbu v0,5(v0)                        ; +0x05 FRISCH lesen (DECIDE darf es ändern)
@0x8010175c  addiu at,at,-1760   -> 0x8011f920   ; ANIMATE-Tabelle
@0x8010176c  jalr v0
```

Tabellen-Dump (`re15_disasm.py table`):

| Idx | DECIDE `@0x8011F8E0` | ANIMATE `@0x8011F920` | Bedeutung |
|---|---|---|---|
| 0 | `0x801035F8` | `0x801036DC` | **Kriech-Lokomotion** |
| 1,2 | `0x80103B8C` (`jr ra`) | `0x80103B94` | Kriech-Grab |
| 3,4 | `0x80104540` (`jr ra`) | `0x80104548` | Kriech-Devour |
| **5** | **`0x8010466C`** | **`0x80104808`** | **Liege-Ruhe + Näherungs-Wecker** |
| 6 | `0x80104F78` (`jr ra`) | `0x80104F80` | Toggle (Hin-/Rückweg) |
| 7..14 | NULL | NULL | im Original Absturz — nie erreichbar |
| 15 | `0x80109E44` | `0x80109E4C` | (RNG-ziehend, unportiert) |

### (d) Zeile 5 — was sie tut (die eigentliche Antwort)

**ANIMATE[5] `FUN_80104808`** (Ruhe-Schleife):
```
@0x80104818  lbu v0,6(v1)          ; +0x06
@0x80104820  bne v0,zero,0x80104860
@0x8010482c  sb 1,6(v1)            ; +0x06 = 1
@0x8010483c  sb 0xc,148(v1)        ; +0x94 = 0x0C
@0x8010484c  sb zero,149(v0)       ; +0x95 = 0
@0x8010485c  sb 7,143(v1)          ; +0x8F = 7
@0x80104874  andi v1,v1,0xffef     ; +0x1D8 &= ~0x10
@0x80104890  jal 0x8001f314  (a3=0x200, a2=0)   ; anim_set(+0x170,+0x174) -> Clip 0x0C läuft
```

**DECIDE[5] `FUN_8010466C`** — zwei Blöcke, **letzter gewinnt**:

```
; --- Block A: NAH (dist < 1200) ---
@0x80104680  lbu v0,0(s0)          ; s0 = 0x800ACAE7 = player.hit_react
@0x80104688  bne v0,zero,0x80104704 ;  != 0 -> Block A überspringen (Block B läuft trotzdem)
@0x8010469c  lw  v0,464(v0)        ; +0x1D0 = gecachte Distanz
@0x801046a4  sltiu v0,v0,0x4b0     ; dist < 1200 ?
@0x801046b4  jal arc_test (a0=0x800ACA88 Spielerblock, a1=0x200)
@0x801046d4  lbu v1,player.floor / lbu v0,130(v0)   ; gleiche Etage ?
@0x801046e4  jal 0x8001a780 facing_aligned (a0=0x800ACA54)
@0x801046ec  addiu v0,v0,1 ; sll 8 ; ori 1
@0x80104700  sw  v0,4(v1)          ; +0x04 = 0x101 / 0x201   (= Kriech-GRAB, Sub 1/2)
; --- FÄLLT DURCH (kein Branch) ---
; --- Block B: MITTEL (dist < 3000) ---
@0x80104710  lw  v0,464(v0)
@0x80104718  sltiu v0,v0,0xbb8     ; dist < 3000 ?
@0x8010471c  beq v0,zero,0x8010474c
@0x8010472c  jal arc_test (a0=0x800ACA88, a1=0x200)
@0x80104734  bne v0,zero,0x8010474c
@0x80104738  ori v0,zero,0x1       ; (Delay-Slot)
@0x80104748  sw  v0,4(v1)          ; +0x04 = 0x00000001  -> +0x05 = 0 = KRIECH-LOKOMOTION
```

> **Nebenbefund (wichtig für einen späteren Einbau):** Block A und Block B benutzen denselben
> `arc_test`-Referenzblock `0x800ACA88` und dieselbe Weite `0x200`. Ist `dist < 1200`, ist auch
> `dist < 3000` — Block B **überschreibt** den Grab-Commit aus Block A im selben Frame
> (`sw` @0x80104748 nach `sw` @0x80104700). **Block A ist damit im ausgelieferten Original
> toter Code.** Das effektive Verhalten von Zeile 5 ist genau eine Regel:
> **dist < 3000 && arc(0x200) → `+0x04 = 1` → Kriech-Lokomotion.**
> (Der Grab kommt danach aus DECIDE[0] `FUN_801035f8` @0x8010360C-8C, dessen Nah-Zweig im Port
> bereits byte-true als `re15_zgirl_overflow_row11` läuft.)

### (e) Die Lokomotion selbst — `FUN_801036DC` (ANIMATE[0])

```
@0x801036ec  lbu v0,6(v1)      ; Erstframe-Gate +0x06 == 0
@0x801036fc  sh  0x1e,140(v1)  ; +0x8C = 0x1E   (Halbwort!)
@0x8010370c  sb  1,6(v1)       ; +0x06 = 1
@0x8010371c  sb  0x1a,148(v1)  ; +0x94 = 0x1A   <<< der KRIECH-Clip
@0x8010372c  sb  zero,149(v0)  ; +0x95 = 0
@0x8010373c  sb  0x0f,143(v1)  ; +0x8F = 0x0F
@0x8010374c  sb  8,471(v1)     ; +0x1D7 = 8     <<< SCA-Zeile „kriechend"
```

Das ist **derselbe Endzustand**, den ROOM1030 über den Skript-Weg erreicht — Beleg, dass
`0x81` und „Kriecher" physisch dasselbe sind:

```
Kriech-Commit im Toggle FUN_80104F80 (ROOM1030-Weg):
@0x801050D0  ori v0,zero,0x81
@0x801050D4  sb  v0,9(v1)      ; +0x09 = 0x81
@0x801050E4  sw  a1,4(v0)      ; +0x04 = 1   -> +0x05 = 0
@0x801050F4  sb  8,471(v1)     ; +0x1D7 = 8
```

---

## 3. Der Vergleichsraum ROOM1030 — warum es DORT im Port funktioniert

| | ROOM1030 (Port zeigt Kriecher ✅) | ROOM1010 (Port zeigt keinen ❌) |
|---|---|---|
| **Daten** | 20× `Sce_em_set type=0x16 beh=0x0D` (**aufrecht**, pre-engaged) + `Save(0x12,6)` Spawn-Deckel | 2× `Sce_em_set type=0x10 beh=**0x81**` (**Kriecher ab Spawn**) |
| **Wie wird er Kriecher** | zur **Laufzeit**: sub07 `3d 04 10 / 26 00 05 04 00 10 / 35 10 04` → `+0x1C4 \|= 0x1000` → Toggle `FUN_80104F80` → `+0x09 = 0x81`, `+0x04 = 1` | **beim Spawn**: Deskriptor `0x81` → INIT @0x80100d3c-68 → `+0x05 = 5` |
| **Ziel-Sub** | `+0x05 = **0**` (Kriech-Lokomotion) | `+0x05 = **5**` (Liege-Ruhe + Wecker) |
| **Port-Pfad** | `re15_enemy_ai_toggle_animate` (enemy_ai_common.c:3389) → Grid-1 **Zeile 0** = `re15_zcrawl_animate` — **portiert** | Grid-1 **Zeile 5** — `enemy_ai_common.c:4180` / `:4205` = **`re15_grid1_open_log(...)`, No-op** |

Die Kriech-Lokomotion, der Kriech-Grab, der Kriech-Devour, die SCA-Maske-8-Klemme und der
Toggle sind im Port **alle da und byte-true**. Es fehlt **nur der Eingang**, den ein
`0x81`-Spawn benutzt.

---

## 4. Messung am Port (Sonde auf der echten RDT)

**Sonde:** `re15_port/tests/unit/probe_1010_kriecher.c`
(registriert in `tests/unit/CMakeLists.txt` **ohne `add_test`** — reine Messung, die
ctest-Zahl bleibt bei 230.)
Sie fährt die echte `ROOM1010.RDT` durch `scd_room_reenter` + `scd_vm_tick` +
`re15_game_step`, mit geladenen EMS-Bänken, in beiden Flavors und mit **beiden**
Eintritts-Cuts.

Aufruf: `./re15_port/build/tests/unit/probe_1010_kriecher.exe`

### Kontrolle: Eintritts-Cut 0 (Nordtür) — der Switch greift

```
RE1.5-cut0 f-1  slot1 type=0x10 +05=0 +09grid=0x00 +94clip=0x00  pos=(3750,5000)
RE1.5-cut0 f-1  slot2 type=0x10 +05=0 +09grid=0x00 +94clip=0x00  pos=(1200,7150)
RE1.5-cut0 f30  slot2 +04state=1 +05=7 … pos=(4182,5666)   ; läuft, greift an — normal
```
→ Case 0 spawnt korrekt zwei **aufrechte** Zombies. Der `Switch(work_vars[0x0A])`-Pfad des
Ports funktioniert.

### Der Fall des Nutzers: Eintritts-Cut 4 (Südtür), Spieler auf dem Tür-Zielpunkt (3650,−3950)

**RE1.5-Flavor** (Auszug, 700 Frames):
```
f-1   slot3 +04state=0 +05=0 +06=0 +09grid=0x81 +94clip=0x0C +95=0 +1D7sca=4  pos=(950,-1700)
f0    slot3 +04state=1 +05=5 +06=0 +09grid=0x81 +94clip=0x0C +95=0 +1D7sca=4  dist=3510
f30   slot3 +04state=1 +05=5 +06=0 +09grid=0x81 +94clip=0x0C +95=0 +1D7sca=4  pos=(950,-1700)
f300  slot3 … identisch …
f700  slot3 +04state=1 +05=5 +06=0 +09grid=0x81 +94clip=0x0C +95=0 +1D7sca=4  pos=(950,-1700)
```
Dazu **einmalig auf stderr**:
```
[grid1] OFFEN: DECIDE[5] nicht portiert (Tabellen 0x8011F8E0/0x8011F920)
[grid1] OFFEN: ANIMATE[5] nicht portiert (Tabellen 0x8011F8E0/0x8011F920)
```
Der INIT setzt `+0x05 = 5` und `+0x94 = 0x0C` **byte-true** (`enemy_ai_common.c:1668`) —
danach **passiert nichts mehr**: `+0x06` bleibt 0 (ANIMATE[5] setzt es im Original auf 1),
`+0x95` bleibt 0 (kein `anim_set`), `+0x1D7` bleibt 4, die Position ändert sich nie.

**RE1.5-Flavor, Spieler direkt daneben (950,−900), `dist = 849`** — also weit **innerhalb**
beider Original-Schwellen (1200 **und** 3000): über 700 Frames **exakt derselbe Standbild-Zustand**
(`+05=5 +06=0 clip=0x0C +95=0 sca=4`, Position unverändert). Der Wecker existiert im Port nicht.

**RE2-Flavor, dieselbe Tür (dist 3510):**
```
f0    slot3 +04state=1 +05=7 +06=0 +09grid=0x81 +94clip=0x0C +10E=0x4002
f1    slot3 +04state=1 +05=7 +06=1 +09grid=0x81 +94clip=0x17 +10E=0x4002
f700  slot3 +04state=1 +05=7 +06=1 +09grid=0x81 +94clip=0x17 +10E=0x4002   (unverändert)
```
→ Liege-Executor `EXEC[7]`, RE2-Clip `0x17`, wartet.

**RE2-Flavor, Spieler daneben (dist 849):**
```
f0    slot3 +05=7 +06=0 +10E=0x4002 clip=0x0C
f5    slot3 +05=7 +06=3 +10E=0x0002 clip=0x08         ; Limpet-Latch 0x4000 gelöscht = geweckt
f120  slot3 +05=1 +06=1 +09grid=0x00 clip=0x00 pos=(1403,-1570)   ; STEHT und LÄUFT
f300  slot3 +05=5 … pos=(2879,-4006)
f700  slot3 +05=6 … pos=(247,-1554) dist=195          ; aufrechter Angriff
```
→ Der RE2-Zweig macht aus dem Kriecher einen **normalen, aufrecht laufenden Zombie**.

Zusammengefasst, gemessen und nicht modelliert:

| | RE1.5 | RE2 | Original |
|---|---|---|---|
| kriecht (Clip 0x1A / `+0x10E&1`) | **nein**, 0/2 | **nein**, 0/2 | ja, 2/2 ab dist < 3000 |
| bewegt sich überhaupt | **nein** | ja — **aufrecht** | ja — kriechend |
| SCA-Zeile 8 (Kriecher-Kollision) | nein (bleibt 4) | nein (bleibt 4) | ja (`sb 8,471` @0x8010374C) |

---

## 5. Gilt der Befund nur unter RE2? — Nein. Getrennt beantwortet.

**RE1.5-Modus: BETROFFEN, und zwar härter.**
Die beiden Zombies sind ein **Standbild** — kein Wecker, keine Animation (`+0x95` bleibt 0,
weil der Original-`anim_set` in ANIMATE[5] @0x80104890 fehlt), keine Bewegung, keine Gefahr.
Ursache: `enemy_ai_common.c:4180-4185` (DECIDE[5] No-op) und `:4205-4208` (ANIMATE[5] No-op).
Das ist eine **schon markierte, bewusst offene** Stelle — dieses Dossier liefert ihr den
ersten belegten Konsumenten und den Beweis, dass sie ausgeliefertes Verhalten kostet.

**RE2-Modus: BETROFFEN, mit anderem Symptom.**
`enemy_ai_re2_zombie.c:7027-7033` zählt `sel ∈ {1,3}` zur `lying_family` und mappt sie —
ausdrücklich als dokumentierte **PORT-OPTION**, nicht als RE2-Byte-Befund — auf den
**Liege**-Executor `EXEC[7]` (`+0x10E = 0x4002`, Wort `0x701`, @0x80100A34-38 / @0x801009E8).
Der zugehörige Wecker (`enemy_ai_re2_zombie.c:2169-2171`) prüft
`(grid & 0x0F) != 7 && != 8 && (f10e & 0x4000) && dist < 0xbb8` — Nibble 1 kommt durch, also
**steht der Zombie auf**.

Das ist inkonsistent mit dem Rest desselben Files: der **RE2-Kriecher ist bereits portiert**
(`+0x10E` Bit 0, Wurzel `0x80101210`, `re2z_crawl`), und es gibt sogar schon einen sauberen
Eintritts-Helfer `re15_re2z_enter_crawler()` (`:2601`, setzt `+0x10E = (…&~0x3F)|1`,
`sw 1,4` @0x80107A54-58, `sca_mask = 8`). Bisher wird er **nur** von der
ROOM1030-Skript-Brücke (`+0x1C4 & 0x1000`, `:7236-7241`) benutzt. Ein Spawn-Deskriptor, der
im RE1.5-Original wörtlich „Kriecher" heißt, gehört an denselben Helfer — nicht an den
Liege-Executor.

**Sound-/Präsentations-Mandat:** berührt diesen Befund nicht. Es geht um eine
**Entscheidungs-/Zustands**-Zuordnung beim Spawn, nicht um Klang oder Darstellung; die
Kriech-Präsentation (Clips, SCA-Zeile) ist in beiden Lanes bereits RE1.5-seitig verdrahtet.

---

## 6. Reichweite des Befundes

Dieselbe Lücke betrifft **jeden** `0x81`/`0x83`-Spawn, also **10 Räume** (5 Paare, jeweils
`x0`/`x1`-Variante), **14 Aktoren**:
ROOM1010/1011 (2), ROOM1220/1221 (2), ROOM3010/3011 (1, `0x83`), ROOM4050/4051 (1),
ROOM5060/5061 (1). Typen 0x10, 0x11, 0x16, 0x18 — alle vier laufen über denselben
Zombie-Root und dieselben Grid-1-Tabellen.

---

## 7. Was ein Fix bräuchte (nächste Phase, **nicht** hier umgesetzt)

1. **Grid-1 DECIDE[5]** = `FUN_8010466C` @0x8010466C portieren. Effektiv (siehe §2d-Nebenbefund):
   `dist(+0x1D0) < 0xBB8 && arc_test(Spielerblock, 0x200) == 0` → `+0x04 = 1`
   (`re15_ai_set_state_word(e, 1)`). Ob Block A (der überschriebene Grab-Commit @0x80104700)
   literal mitportiert wird, ist eine reine Treue-Frage ohne Verhaltensunterschied —
   Empfehlung: literal mitschreiben, damit der Code beim nächsten Leser nicht als „vereinfacht"
   auffällt.
2. **Grid-1 ANIMATE[5]** = `FUN_80104808` @0x80104808 portieren:
   Erstframe (`+0x06 == 0`) → `+0x06 = 1`, `+0x94 = 0x0C`, `+0x95 = 0`, `+0x8F = 7`;
   jeden Frame `+0x1D8 &= ~0x10` und `anim_set(+0x170, +0x174, a2 = 0, a3 = 0x200)`.
3. **RE2-Lane:** `sel ∈ {1,3}` **aus** der `lying_family` (`enemy_ai_re2_zombie.c:7027-7033`)
   herausnehmen und stattdessen `re15_re2z_enter_crawler(e, NULL, 0)` aufrufen — der Helfer
   existiert und ist byte-belegt.
   ⚠ Vorher prüfen, ob RE2 einen eigenen Näherungs-Wecker für den Kriecher hat oder ob der
   Kriecher dort sofort losläuft; sonst startet die RE2-Fassung 3000 Einheiten früher als die
   RE1.5-Fassung.
4. **Regressionswache:** ROOM1030 (Skript-Weg → Zeile 0) darf sich nicht ändern —
   `probe_1030_crawl_live` / `test_1030_trigger_chain` müssen grün bleiben.

---

## 8. Offene Punkte (ehrlich benannt)

* **Hardware-Gegenprobe fehlt.** Die Kette ist statisch **end-to-end** belegt (jede Stufe mit
  Adresse), aber es gibt **keinen** DuckStation-Lauf/Savestate, der ROOM1010 über die Südtür
  betritt und `+0x05`/`+0x94`/`+0x1D7` der beiden Aktoren live zeigt. Das ist der einzige
  Schritt zwischen „byte-belegt" und „hardware-bestätigt". Empfohlen: `re15-room-capture`
  → ROOM1020 → Südtür → Savestate bei Spieler-z ≈ −3000, dann `+0x94 == 0x1A` prüfen.
* **`+0x8C = 0x1E`** (Halbwort @0x801036FC) — welcher Konsument diesen Timer liest, ist in
  diesem Durchgang nicht verfolgt worden (im ROOM1030-Dossier ebenfalls offen).
* **Kriech-Geschwindigkeit** ist laut ROOM1030-Dossier (BLOCKER B) nicht byte-verglichen;
  ein ROOM1010-Fix erbt diese offene Frage unverändert.
* **`0x80109E44`/`0x80109E4C` (Zeile 15)** bleiben unportiert — sie ziehen RNG
  (@0x8010a188) und würden den deterministischen Strom verschieben. Von ROOM1010 nicht berührt.

---

## 9. Belegliste (alles in dieser Runde selbst gelesen)

| Aussage | Beleg |
|---|---|
| ROOM1010 spawnt 2× `beh 0x81` in Case 4 | ROOM1010.RDT Datei 0x0A12 / 0x0A26 |
| Case-Wert LE `s16 @ a3+4` | `op_switch` @0x8003fb00 `lh` (scd_vm.c:1587) |
| Cut 4 = ROOM1020-Südtür | ROOM1020.RDT main00@0x1C82, `pc[24] = 4` |
| Eintritts-Cut → `work_vars[0x0A]` | @0x8001d930 `lbu v1,10(a0)` / @0x8001d948 `sh v1,4068(at)` |
| `+0x09 = Deskriptor` | `FUN_800420a0`; Port `scd_vm.c:3253` |
| Bit 0x80-Gate im INIT | @0x80100ca4-b0 |
| `sel {1,3}` → `+0x94 = 0x0C` | @0x80100d54-58 |
| `sel {1,3}` → `+0x05 = 5` | @0x80100d64-68 |
| Nibble → Sub-Modus-Tabelle | `@0x8011f80c`, [1] = `0x80101708` |
| Grid-1 Doppel-Dispatch | @0x80101718 / @0x80101728 / @0x8010174c / @0x8010175c |
| DECIDE/ANIMATE-Tabellen | `table 0x8011f8e0` / `table 0x8011f920` |
| Zeile-5-Wecker `dist < 0xBB8` → `+0x04 = 1` | @0x80104710-748 |
| Zeile-5-Ruhepose | @0x80104818-890 |
| Kriech-Lokomotion setzt `0x1A`/`sca 8` | @0x8010371c / @0x8010374c |
| ROOM1030-Toggle setzt dasselbe `0x81` | @0x801050d0-f4 |
| Port: Zeile 5 = No-op | `enemy_ai_common.c:4180-4185`, `:4205-4208` |
| Port: RE2 mappt `sel{1,3}` auf Liege-EXEC[7] | `enemy_ai_re2_zombie.c:7027-7033` |
| Port: RE2-Liege-Wecker `dist < 0xbb8` | `enemy_ai_re2_zombie.c:2169-2171` |
| Port-Messung | `probe_1010_kriecher.c`, Ausgabe §4 |
| Deskriptor-Zensus 12× 0x81 / 2× 0x83 | eigener Walker über alle 240 RDTs |
