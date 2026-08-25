# ROOM1220 — „die Zombies sollten kriechen. Auch bei RE2-AI." (FINDING E)

**Stand:** 2026-08-27 · **Phase:** RE + Messung abgeschlossen, **KEINE Code-Änderung**
**Präzedenzfall:** `analysis/nutzer_batch_2026-08-26/room1010-kriecher.md` + Commit `6f7c4cbc` (v0.3.23)

---

## 0. Urteil (vorweg, damit niemand am falschen Ende sucht)

**Der v0.3.23-Fix wirkt in ROOM1220 — die Kriech-Maschine läuft, GEMESSEN, in BEIDEN KI-Modi.**
Das Problem liegt woanders und ist ein anderer, eigener Port-Defekt:

> **Die beiden ROOM1220-Kriecher spawnen im Port GAR NICHT MEHR, sobald der Spieler im
> Vorraum ROOM1210 die dortigen „Writher" (Typ 0x1A) erschossen hat.**
> ROOM1210s Writher und ROOM1220s Zombies tragen **dieselben em-Status-Kill-Flag-Indizes**
> (0x8A, 0x8B, 0xE4…0xEB — alle zehn, byte-belegt, §5). Der Port persistiert den Tod **jedes**
> Aktors, der Zustand 7 (CORPSE) erreicht (`enemy_ai_common.c:11502-11503`) — auch den des
> Writhers. Im **Original** enthält die gesamte Writher-Familie (`0x8010C1EC…0x8010D770`,
> Zustandstabelle `@0x8012093C`) **keinen einzigen** `jal 0x8004ef90` — der Writher-Tod setzt
> dort **kein** Kill-Flag (eigener Byte-Scan über die ganze `STAGE1.BIN`: 17 Treffer, §3c).
>
> Folge im Port: Der Spieler betritt ROOM1220 durch die Tür, hinter der die zwei Kriecher
> liegen — und trifft **nichts** an. Die Zombies, die er im Raum antrifft, sind die der
> anderen vier Eingänge, und die sind per RDT-Daten `beh = 0x02` = **aufrecht**.
> Genau das beschreibt der Nutzer.

Gemessen (Sonde §6): mit gesetzten Flags 0x8A/0x8B spawnt ROOM1220 Case 0 **null** Aktoren —
in RE1.5 **und** RE2. Und: der Port setzt Flag(7,0x8A) tatsächlich, sobald der ROOM1210-Writher
Zustand 7 erreicht (`ERGEBNIS Flag(7,0x8A) = 1`).

---

## 1. Symptom (Nutzer-Wortlaut)

> „Im ROOM1220 sollten die Zombies eigentlich kriechen. Auch bei RE2-AI."

---

## 2. Die RDT-Deskriptoren von ROOM1220 (eigener Byte-Walker)

Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM1220.RDT`, Größe `0x224A8`
Adresstabelle (`rdt_common.c:238-239`, `read_u32_le(&data[0x40])` / `[0x44]`):
`main_scd = 0x0DD0`, `sub_scd = 0x0F1C`, `extra_scd = 0x101C`.
Header `00 0a 03 00 00 00 00 00` → nSprite 0, nCut 10, nOmodel 3.
`sub_scd`-Tabelle @0x0F1C: 2 Einträge → `sub00 @0x0F20`, `sub01 @0x1018` (nur `Evt_end`).

Roh-Bytes des Spawn-Kopfes (`od -A x -t x1 -j 0x0F20 -N 0x40 ROOM1220.RDT`):

```
000f20  13 0a f2 00  14 00 2a 00 00 00  44 00 16 81 00 00
000f30  00 8a 58 9e 00 00 d4 e5 00 00  00 00 00 00  44 01
000f40  16 81 00 00 01 8b 34 a4 00 00  1c da 00 00 00 0c
000f50  00 00  1a 00  14 00 2a 00 02 00  44 02 16 02 00 00
```

Vollständiger `sub00` (Opcode-Walker über die Längentabelle `scd_vm.c:166-215`;
`Sce_em_set`-Feldlayout `scd_vm.c:3113-3130`:
`[1]=slot [2]=type [3]=behavior [4]=soundbank [5]=texture [6]=kill [7]=em-status-Flag
[8..9]=x [10..11]=y [12..13]=z [16..17]=dirY`, alles LE):

```
0x0F20  Switch  work_vars[0x0A]  (Eintritts-Cut), Blocklaenge 0xF2      13 0a f2 00
0x0F24  Case 0   (len 0x2A)                                             14 00 2a 00 00 00
0x0F2A    Sce_em_set slot=0 type=0x16 beh=0x81 emflag=0x8A pos=(-25000,0,-6700)  dirY=0
0x0F3E    Sce_em_set slot=1 type=0x16 beh=0x81 emflag=0x8B pos=(-23500,0,-9700)  dirY=3072
0x0F52    Break
0x0F54  Case 2   (len 0x2A)
0x0F5A    Sce_em_set slot=2 type=0x16 beh=0x02 emflag=0xE4 pos=(-15850,0,-11100) dirY=3072
0x0F6E    Sce_em_set slot=3 type=0x16 beh=0x02 emflag=0xE5 pos=(-14900,0,-8050)  dirY=1024
0x0F82    Break
0x0F84  Case 4   (len 0x2A)
0x0F8A    Sce_em_set slot=4 type=0x16 beh=0x02 emflag=0xE6 pos=(-25950,0,-13650) dirY=2048
0x0F9E    Sce_em_set slot=5 type=0x16 beh=0x02 emflag=0xE7 pos=(-24550,0,-18600) dirY=3072
0x0FB2    Break
0x0FB4  Case 6   (len 0x2A)
0x0FBA    Sce_em_set slot=6 type=0x16 beh=0x02 emflag=0xE8 pos=(-15400,0,-15500) dirY=2048
0x0FCE    Sce_em_set slot=7 type=0x16 beh=0x02 emflag=0xE9 pos=(-13300,0,-17200) dirY=1024
0x0FE2    Break
0x0FE4  Case 8   (len 0x2A)
0x0FEA    Sce_em_set slot=8 type=0x16 beh=0x02 emflag=0xEA pos=(-16000,0,-21050) dirY=1024
0x0FFE    Sce_em_set slot=9 type=0x16 beh=0x02 emflag=0xEB pos=(-14350,0,-23200) dirY=0
0x1012    Break
0x1014  Eswitch     (KEIN Default -> jeder andere Eintritts-Cut spawnt GAR NICHTS)
0x1016  Evt_end
```

### Aktoren-Tabelle (Datei-Byte-Offsets, wie verlangt)

| Case | Record-Offset | Typ-Byte | **Deskriptor-Byte** | em-Flag-Byte | Slot | Typ | **beh** | Position (x,z) |
|---|---|---|---|---|---|---|---|---|
| 0 | 0x0F2A | 0x0F2C = `16` | **0x0F2D = `81`** | 0x0F31 = `8A` | 0 | 0x16 | **0x81** | (−25000, −6700) |
| 0 | 0x0F3E | 0x0F40 = `16` | **0x0F41 = `81`** | 0x0F45 = `8B` | 1 | 0x16 | **0x81** | (−23500, −9700) |
| 2 | 0x0F5A | 0x0F5C = `16` | 0x0F5D = `02` | 0x0F61 = `E4` | 2 | 0x16 | 0x02 | (−15850, −11100) |
| 2 | 0x0F6E | 0x0F70 = `16` | 0x0F71 = `02` | 0x0F75 = `E5` | 3 | 0x16 | 0x02 | (−14900, −8050) |
| 4 | 0x0F8A | 0x0F8C = `16` | 0x0F8D = `02` | 0x0F91 = `E6` | 4 | 0x16 | 0x02 | (−25950, −13650) |
| 4 | 0x0F9E | 0x0FA0 = `16` | 0x0FA1 = `02` | 0x0FA5 = `E7` | 5 | 0x16 | 0x02 | (−24550, −18600) |
| 6 | 0x0FBA | 0x0FBC = `16` | 0x0FBD = `02` | 0x0FC1 = `E8` | 6 | 0x16 | 0x02 | (−15400, −15500) |
| 6 | 0x0FCE | 0x0FD0 = `16` | 0x0FD1 = `02` | 0x0FD5 = `E9` | 7 | 0x16 | 0x02 | (−13300, −17200) |
| 8 | 0x0FEA | 0x0FEC = `16` | 0x0FED = `02` | 0x0FF1 = `EA` | 8 | 0x16 | 0x02 | (−16000, −21050) |
| 8 | 0x0FFE | 0x1000 = `16` | 0x1001 = `02` | 0x1005 = `EB` | 9 | 0x16 | 0x02 | (−14350, −23200) |

`main00 @0x0DD2` enthält 5 `Door_aot_set` (alle → `stage 0 / room 0x21` = ROOM1210),
3 `Obj_model_set`, 3 `Item_aot_set` (Opcode 0x50) — **kein** `Sce_em_set`, **kein**
`Member_set`/`Member_set2`/`Calc` auf Member 9. `sub01` = nur `Evt_end`.
`extra_scd @0x101C` ist keine SCD (Byte 0 = `01 00 00 00 04 00 …`, Binärdaten; laut RDT-Skill
in RE1.5 ungenutzt).

⛔ **Nur Case 0 hat Kriecher.** Cases 2/4/6/8 spawnen `beh = 0x02` (Bit 0x80 CLEAR,
Nibble 2 = der STEHENDE Re-Entry-Schläfer, Grid-Wurzel `@0x8011f80c[2] = FUN_80101784`).
Das ist der Auslieferungs-Datenstand, kein Defekt.

### Welcher Eingang liefert Cut 0?

Tür-Scan über alle 240 RDTs (Ziel `stage=0 / room=0x22`; Felder `pc[22]=stage, pc[23]=room,
pc[24]=target_cut` laut `op_door_aot_set`, `scd_vm.c`):

```
ROOM1210.RDT main00 @0x1CE6 Door slot 1 -> CUT 0  Ziel (-22400,0, -6500) yaw=2048
ROOM1210.RDT main00 @0x1D06 Door slot 2 -> CUT 2  Ziel (-16600,0, -9900) yaw=0
ROOM1210.RDT main00 @0x1D26 Door slot 3 -> CUT 4  Ziel (-22400,0,-14000) yaw=2048
ROOM1210.RDT main00 @0x1D46 Door slot 4 -> CUT 6  Ziel (-16600,0,-17900) yaw=0
ROOM1210.RDT main00 @0x1D66 Door slot 5 -> CUT 8  Ziel (-16600,0,-25050) yaw=0
ROOM1211.RDT identisch
```

ROOM1220 hat **fünf** Eingänge, alle aus ROOM1210. Nur Tür-Slot 1 (Cut 0) spawnt die Kriecher.
Der Tür-Zielpunkt (−22400,−6500) liegt **2598 Einheiten** von Kriecher 0 (−25000,−6700) —
innerhalb der Weck-Schwelle `0xBB8 = 3000` (`sltiu v0,v0,0xbb8` @0x80104718).
ROOM1210s Nordtür (Slot 0, Rect x −28000…−26500 / z −3500…−1300) führt nach ROOM11E0;
Tür-Slot 1 (Rect x −22200…−20700 / z −7200…−5000) ist die **erste** ROOM1220-Tür auf dem Weg
nach Süden.

---

## 3. Was das ORIGINAL tut

### 3a. `0x81` ist wörtlich „Kriecher" — und Typ 0x16 hängt an derselben Wurzel wie 0x10

Die Deskriptor→Kriecher-Kette ist im ROOM1010-Dossier Instruktion für Instruktion belegt; hier
die Ankerpunkte:

```
Sce_em_set schreibt pc[3] -> entity+0x09      FUN_800420a0 (Port-Zwilling scd_vm.c:3253)
INIT-Gate            @0x80100CAC  andi v0,v1,0x80
                     @0x80100CB0  beq  v0,zero,0x80100E30
sel = +0x09 & 0x1F   @0x80100CB4  andi v1,v1,0x1f
sel == 1  ->         @0x80100D54  ori  v0,zero,0xc
                     @0x80100D58  sb   v0,148(a0)      ; +0x94 = 0x0C
                     @0x80100D64  ori  v0,zero,0x5
                     @0x80100D68  sb   v0,5(v1)        ; +0x05 = 5   (die Sub-Zeile)
Grid-Wurzel          @0x8011F80C[1] = 0x80101708  = Kriech-Maschine
DECIDE[5]/ANIMATE[5] @0x8011F8E0[5] = 0x8010466C  /  @0x8011F920[5] = 0x80104808
Weck-Schwelle        @0x80104718  sltiu v0,v0,0xbb8    ; dist < 3000
                     @0x80104748  sw    v0,4(v1)       ; +0x04 = 1 -> Kriech-Lokomotion
Kriech-Lokomotion    @0x8010371C  sb 0x1a,148(v1)      ; Clip 0x1A
                     @0x8010374C  sb 8,471(v1)         ; SCA-Zeile 8
gleicher Wert per Skript in ROOM1030: @0x801050D0 ori v0,zero,0x81 / @0x801050D4 sb v0,9(v1)
```

**Typ 0x16 benutzt exakt dieselbe Wurzel wie Typ 0x10** — selbst disassembliert aus
`info/Re1.5/PSX/BIN/STAGE1.BIN` (Overlay lädt @0x80100000, kein Header):

```
8011e868: lui   v0,0x8010
8011e86c: addiu v0,v0,1060          -> v0 = 0x80100424   (die Zombie-Wurzel)
...
8011e88c: sw    v0,11244(at)        -> 0x80072BEC = Tabelle[0x10]
8011e894: sw    v0,11248(at)        -> 0x80072BF0 = Tabelle[0x11]
8011e89c: sw    v0,11252(at)        -> 0x80072BF4 = Tabelle[0x12]
8011e8a4: sw    v0,11268(at)        -> 0x80072C04 = Tabelle[0x16]   <<<
8011e8ac: sw    v0,11276(at)        -> 0x80072C0C = Tabelle[0x18]
```
(`(0x80072C04 − 0x80072BAC)/4 = 22 = 0x16`.)

⇒ Die zwei ROOM1220-Aktoren aus Case 0 sind im Original **von Geburt an Kriecher**, ohne
Skript-Trigger.

### 3b. Kein raum-spezifischer Eingriff

* `grep 0x22` (Raum-Index) in `RE_15_Quellcode_Overlays/STAGE1_overlay.c`: **0 Treffer**.
* ROOM1220s SCD (main00 + sub00 + sub01, vollständig gewalkt, §2): kein Opcode, der `+0x9`
  schreibt (kein `Member_set`, kein `Calc`), kein `Evt_exec`.

⇒ **Es gibt keinen Laufzeit-Setzer.** Der RDT-Deskriptor ist die einzige Quelle. (Anders als
ROOM1030, das den Kriech-Zustand per `sub07` → `+0x1C4 |= 0x1000` → Toggle `FUN_80104F80`
zur Laufzeit erzeugt.)

### 3c. Der Writher (Typ 0x1A) setzt im Original KEIN em-Status-Kill-Flag

Der Flag-Setzer ist `0x8004ef90(a0 = Flag-Bank, a1 = entity+0x1C6)`. Belegt an den
Zombie-Todes-Commits:

```
801096e8: lui   v0,0x800b
801096ec: lw    v0,-14460(v0)      ; g_entity(cur)
801096f4: lbu   a1,454(v0)         ; +0x1C6 = der em-Status-Index
801096f8: addiu a0,s1,18644
801096fc: jal   0x8004ef90
---
80107164: lbu   a1,454(v0)         ; +0x1C6
80107168: addiu a0,a0,120
8010716c: jal   0x8004ef90
```

**Eigener Byte-Scan über die GANZE `info/Re1.5/PSX/BIN/STAGE1.BIN`** (Muster
`jal 0x8004ef90` = Wort `0x0E013BE4` = Bytes `e4 3b 01 0e`) — **17 Treffer**:

```
0x80106E0C 0x8010716C 0x801074B0 0x801076EC 0x80107DC4 0x801082A4 0x801082C4
0x80109100 0x801096FC            <- alle unterhalb 0x8010A8C8 (Zombie-Familie, Wurzel 0x80100424)
0x80110FBC 0x801111D4            <- Hund   (Wurzel 0x8010D7F8)
0x801146EC 0x801152C4            <- Kraehe (Wurzel 0x80112020)
0x80116AC8                       <- Typ 0x26 (Wurzel 0x80116288)
0x8011B8F4 0x8011BAFC 0x8011BD24 <- Gorilla-Boss 0x27 (Wurzel 0x80116DB8)
```

Die **Writher-Familie** ist die Zustandstabelle `@0x8012093C` (selbst gedumpt,
`re15_disasm.py table 0x8012093c 8 --bin STAGE1.BIN`):

```
[0] 0x8010C33C INIT   [1] 0x8010C488 ACTIVE   [2] 0x8010D0F8 HURT   [3] 0x8010D474 DEATH
[4] 0x8010D768        [5] 0x00000000          [6] 0x00000000        [7] 0x8010D770 CORPSE
```

Wurzel = `0x8010C1EC`, registriert für Typ 0x1A:
`8011e884: addiu v1,v1,-15892` (= 0x8010C1EC) … `8011e9ac: sw v1,11284(at)` → `0x80072C14`,
und `(0x80072C14 − 0x80072BAC)/4 = 26 = 0x1A`.

**In `0x8010C1EC … 0x8010D770` liegt KEIN einziger der 17 Treffer.** Der Todes-Commit selbst
(`re15_disasm.py dis 0x8010d580 16 --bin STAGE1.BIN`):

```
8010d580: lui  v0,0x800b
8010d584: lw   v0,-14460(v0)
8010d588: addu a2,zero,zero
8010d58c: lw   a0,132(v0)
8010d590: lw   a1,364(v0)
8010d594: jal  0x8001f314        ; anim_set (Clip 3 abspielen)
8010d598: ori  a3,zero,0x200
8010d5a8: lbu  v1,7(a0)          ; +0x7 Phase
8010d5b0: addu v1,v1,v0
8010d5b8: sb   v1,7(a0)
8010d5bc: sw   v0,4(a0)          ; +0x4 = 7 = CORPSE   <<< kein jal 0x8004ef90 im ganzen Ast
```

**Gegenprobe EXE-Seite:** `info/Re1.5/PSX.EXE` (Ladeadresse 0x80010000 aus Header +0x18,
0x800-Byte-Header) enthält 12 weitere `jal 0x8004ef90`. **Keiner** liest `entity+0x1C6`:

```
0x8001E0D0  lhu a1,4(s1)                 (AOT/Item)
0x80021FD8  lhu a1,4(s0)                 (Item-Confirm)
0x80030CD8  lbu a1,DAT_800ACC17
0x80030F2C  lbu a1,DAT_800ACC17
0x800311AC / 0x8003131C / 0x800313F8     (derselbe DAT_800ACC17-Block)
0x80051414  lbu a1,451(v0)   = +0x1C3    (NPC-Bibliothek)
0x80051780 / 0x80051AB4 / 0x80051CAC / 0x80051DD8   (alle +0x1C3)
```

⇒ **Im Original persistiert der Writher-Tod nichts.** ROOM1220s Zombies spawnen also auch
dann, wenn der Spieler in ROOM1210 alles abgeräumt hat.

---

## 4. Was der PORT heute tut

### 4a. Die Kriech-Kette (v0.3.23) — funktioniert auch für Typ 0x16

* `re15_port/engine/src/enemy_ai_common.c:4180-4219` — DECIDE[5] = `FUN_8010466C`
* `re15_port/engine/src/enemy_ai_common.c:4241-4257` — ANIMATE[5] = `FUN_80104808`
* `re15_port/engine/src/enemy_ai_common.c:4770-4771` —
  `shares_root = (a->type == 0x10 || 0x11 || 0x12 || 0x16 || 0x18)`
* `re15_port/engine/src/enemy_ai_re2_zombie.c:7025-7042` — RE2 routet `sel ∈ {1,3}` mit
  Bit 0x80 nach `re15_re2z_enter_crawler`

### 4b. Die universelle Kill-Flag-Persistenz — der Defekt

`re15_port/engine/src/enemy_ai_common.c:11494-11503`:

```c
for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
    re15_actor_t *e = &g_actors[s];
    if (!e->active) continue;
    /* EM-STATUS KILL PERSISTENCE (byte-true FUN_80109554 @0x801096fc / FUN_80106edc @0x8010716c
     * set flag[entity+0x1C6] on the death-commit): ... */
    if (e->state == (uint8_t)RE15_AI_STATE_CORPSE && e->em_flag_id != 0xFF)
        re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);
```

Der Kommentar zitiert **zwei Zombie-Adressen** — angewandt wird die Regel aber auf **jeden**
Aktor. Der Writher erreicht CORPSE regulär: `re15_writher_ai_tick`
(`enemy_ai_common.c:10438` ff.), `case 3:` → `e->state = 7;` (Kommentar dort:
`sw 7 into +0x4 @0x8010d5bc`) — erreichbar mit **einem** Schuss (Spawn-HP 0, Hitbox r=300
`re15_damage.c:2147`).

Der Spawn-Filter, der dann greift, ist `re15_port/engine/src/scd_vm.c:3226-3228`:

```c
uint8_t  persist  = t->pc[7];
int      suppress = (persist != 0xFF) && re15_game_flag_get(re15_em_status_zone(), persist);
```

und `re15_em_status_zone()` (`enemy_ai_common.c:11337-11341`) liefert für Stage 0..2 die
**Zone 7** — für ROOM1210 **und** ROOM1220 dieselbe.

---

## 5. Die Divergenz (präzise)

**Flag-Kollisions-Zensus** (eigener Walker über alle 240 RDTs, Paare `em-Status-Index → Typ`):

```
zone 7 flag 0x8A : ROOM1210/1211 Typ 0x1A beh 0x00  ||  ROOM1220/1221 Typ 0x16 beh 0x81
zone 7 flag 0x8B : ROOM1210/1211 Typ 0x1A beh 0x00  ||  ROOM1220/1221 Typ 0x16 beh 0x81
zone 7 flag 0xE4 : ROOM1210/1211 Typ 0x1A beh 0x00  ||  ROOM1220/1221 Typ 0x16 beh 0x02
zone 7 flag 0xE5 : dito     zone 7 flag 0xE6 : dito     zone 7 flag 0xE7 : dito
zone 7 flag 0xE8 : dito     zone 7 flag 0xE9 : dito     zone 7 flag 0xEA : dito
zone 7 flag 0xEB : dito
```

Das ist die **einzige** Kollision zwischen Typ 0x1A und der Zombie-Familie im ganzen Spiel —
und sie betrifft **alle zehn** ROOM1220-Zombies. ROOM1210s zehn Writher
(`ROOM1210.RDT main00 @0x1D86, 0x1D9A, 0x1DAE, 0x1DC2, 0x1DD6, 0x1DEA, 0x1DFE, 0x1E12,
0x1E26, 0x1E3A`, alle `type=0x1A beh=0x00`) tragen genau dieselben zehn Indizes in derselben
Reihenfolge.

| | Original | Port heute |
|---|---|---|
| Writher-Tod (ROOM1210) setzt Flag[+0x1C6]? | **NEIN** — kein `jal 0x8004ef90` in `0x8010C1EC…0x8010D770` | **JA** — universelle CORPSE-Regel `enemy_ai_common.c:11502` |
| ROOM1220 Case 0 spawnt danach | 2 Kriecher (beh 0x81) | **0 Aktoren** (gemessen, §6-D) |
| ROOM1220 Cases 2/4/6/8 spawnen danach | je 2 aufrechte (beh 0x02) | 0, wenn der zugehörige Writher tot ist |
| Was der Spieler sieht | 2 Kriecher an der ersten Tür | keine Kriecher; nur aufrechte Paare an noch „unverbrauchten" Türen |

Zweiter, kleinerer Punkt: **acht der zehn ROOM1220-Zombies sind per Datenstand aufrecht**
(`beh = 0x02`, Deskriptor-Bytes @0x0F5D, 0x0F71, 0x0F8D, 0x0FA1, 0x0FBD, 0x0FD1, 0x0FED,
0x1001). Wer nur an den Türen Cut 2/4/6/8 hereinkommt, sieht auch im Original keine Kriecher.
Das ist **kein** Defekt.

---

## 6. Messung am Port (Sonde auf den echten RDTs)

**Sonde:** `re15_port/tests/unit/probe_1220_kriecher.c`
(registriert in `re15_port/tests/unit/CMakeLists.txt` **ohne `add_test`** — reine Messung,
die ctest-Zahl bleibt unverändert.)
Aufruf: `./re15_port/build/tests/unit/probe_1220_kriecher.exe`
Sie fährt `ROOM1220.RDT` durch `scd_room_reenter` + `scd_vm_tick` + `re15_game_step` —
genau der Aufruf, den `re15_room_apply_pending` in `room_common.c:348` macht — mit geladenen
EMS-Bänken, in beiden Flavors.

### A — Eintritts-Cut 0, sauberer Spielstand: **der Fix von v0.3.23 WIRKT**

```
RE1.5-cut0 f-1  slot1 type=0x16 +04=0 +05= 0 +06=0 +09=0x81 +94=0x0C +95=0 +1D7sca=4 pos=(-25000,-6700)
RE1.5-cut0 f0   slot1 type=0x16 +04=1 +05= 5 +06=0 +09=0x81 +94=0x0C +95=0 +1D7sca=4 dist=2598
RE1.5-cut0 f1   slot1 type=0x16 +04=1 +05= 0 +06=1 +09=0x81 +94=0x1A +95=1 +1D7sca=8   <<< KRIECHT
RE1.5-cut0 f30  slot1 +05=2 +94=0x1B  pos=(-23137,-6044)      ; Kriech-Grab, er ist gekrochen
RE1.5-cut0 f0   slot2 +05=5 ... dist=3376  -> bleibt liegen   ; korrekt: 3376 > 0xBB8 = 3000

RE2-cut0   f0   slot1 +10E=0x0001 +1D7sca=8 +94=0x0C          ; re15_re2z_enter_crawler
RE2-cut0   f30  slot1 +94=0x05 pos=(-24184,-6653)             ; kriecht auf den Spieler zu
RE2-cut0   f30  slot2 +94=0x05 pos=(-23301,-8909)             ; ebenso
```

Kontroll-Lauf Cut 2 (Case 2, `beh 0x02`): `+09grid=0x02`, `+94clip=0x00`, `+1D7sca=4`,
aufrechtes Laufen — der `Switch`-Pfad greift, und der Datenstand ist aufrecht.

### D — DERSELBE Eintritt, aber Flags 0x8A/0x8B gesetzt (= ROOM1210-Writher erschossen)

```
=== RE1.5-cut0-flags8A8B  Eintritts-Cut=0  Spieler=(-22400,-6500) ===
  -- direkt nach dem Spawn (vor jedem Tick):
  -- Endstand nach 60 Frames:
=== RE2-cut0-flags8A8B  Eintritts-Cut=0  Spieler=(-22400,-6500) ===
  -- direkt nach dem Spawn (vor jedem Tick):
  -- Endstand nach 60 Frames:
```

**Kein einziger Aktor** — in beiden Flavors. Der Raum ist leer.

### E — Setzt der Port das Flag beim Writher-Tod wirklich?

```
=== E: ROOM1210 Writher-Tod setzt em-Status-Flag? ===
  Flag(7,0x8A) vor allem: 0   Flag(7,0x8B): 0
  slot1  type=0x1A em_flag_id=0x8A hp=0 pos=(-25000,-8847)
  slot2  type=0x1A em_flag_id=0x8B hp=0 pos=(-25000,-10247)
  slot3  type=0x1A em_flag_id=0xE4 hp=0 pos=(-14000,-5897)
  ...  (10 Writher, Flags 0x8A,0x8B,0xE4..0xEB)
  -> slot1 (Typ 0x1A, Flag 0x8A) in Zustand 3 (DEATH) versetzt
  nach 240 Frames: slot1 state=7  Flag(7,0x8A)=1
  ERGEBNIS Flag(7,0x8A) = 1  (1 = der Port persistiert den Writher-Tod)
```

### Kontrolle gegen ROOM1010 (der bereits gefixte Raum)

`probe_1010_kriecher.exe` zeigt dieselbe RE2-Endsignatur (`state=7 hp=-1 clip=0x17`) wie
ROOM1220 — der auffällige RE2-Endzustand ist also **nicht** ROOM1220-spezifisch, sondern eine
Eigenschaft des Sonden-Szenarios (der Spieler wehrt sich nie und bewegt sich nicht).
Keine ROOM1220-Divergenz.

---

## 7. Fix-Rezept für den Implementierer

### FIX 1 (der eigentliche Befund) — Kill-Flag-Persistenz auf die belegten Typen einschränken

**Datei:** `re15_port/engine/src/enemy_ai_common.c`, **Zeile 11502-11503**

Heute:
```c
if (e->state == (uint8_t)RE15_AI_STATE_CORPSE && e->em_flag_id != 0xFF)
    re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);
```

Minimal und vollständig belegt: **Typ 0x1A ausnehmen.**
```c
/* Typ 0x1A (WRITHER, ROOM1210/1211) persistiert im Original NICHT: die ganze
 * Writher-Familie (Wurzel 0x8010C1EC, Zustandstabelle @0x8012093C = {0x8010C33C,
 * 0x8010C488, 0x8010D0F8, 0x8010D474, 0x8010D768, 0, 0, 0x8010D770}) enthaelt
 * KEINEN `jal 0x8004ef90` — eigener Byte-Scan ueber die ganze STAGE1.BIN, 17 Treffer,
 * alle ausserhalb 0x8010C1EC..0x8010D770 (Zombie 0x80106E0C/0x8010716C/0x801074B0/
 * 0x801076EC/0x80107DC4/0x801082A4/0x801082C4/0x80109100/0x801096FC, Hund 0x80110FBC/
 * 0x801111D4, Kraehe 0x801146EC/0x801152C4, Typ 0x26 0x80116AC8, Gorilla 0x8011B8F4/
 * 0x8011BAFC/0x8011BD24). Sein Todes-Commit @0x8010D5BC schreibt NUR `sw v0,4(a0)`.
 * ROOM1210s zehn Writher tragen dieselben em-Status-Indizes (0x8A,0x8B,0xE4..0xEB;
 * ROOM1210.RDT main00 @0x1D86..@0x1E3A) wie ROOM1220s zehn Zombies (ROOM1220.RDT
 * sub00 @0x0F2A..@0x0FFE) — mit der universellen Regel loeschte ein Writher-Kill die
 * Kriecher aus ROOM1220 (GEMESSEN: Case 0 spawnt 0 Aktoren, probe_1220_kriecher Fall D). */
if (e->state == (uint8_t)RE15_AI_STATE_CORPSE && e->em_flag_id != 0xFF && e->type != 0x1A)
    re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);
```

**Warum genau so und nicht breiter:** eine vollständige Positiv-Liste („nur Zombie-Familie,
Hund, Krähe, 0x26, Gorilla") müsste die 17 Treffer noch Funktion für Funktion den Typen
zuordnen — die Adressbereiche sind **nicht** disjunkt (die Zombie-Dame 0x13 hat ihre Wurzel
bei `0x8010A8C8`, Teile ihres Codes liegen laut Port-Kommentar bei `0x80107CB0`). Das bleibt
offen (§9). Der Ausschluss von 0x1A ist einzeln belegt und regressionsfrei: Typ 0x1A kommt
game-weit nur in ROOM1210/1211 vor (RDT-Zensus).

### FIX 2 (Kosmetik, gehört zum selben Commit) — Kommentar `enemy_ai_common.c:11497` ehrlich machen

Der Kommentar verkauft eine **universelle** Regel als „byte-true FUN_80109554 @0x801096fc /
FUN_80106edc @0x8010716c" — beides sind Zombie-Adressen. Nach FIX 1 dort vermerken, dass die
Persistenz **typ-gebunden** ist und die Positiv-Liste noch nicht vollständig aufgelöst wurde.

### KEIN Fix nötig

* Die Kriech-Kette selbst (DECIDE[5]/ANIMATE[5], RE2-`enter_crawler`) — gemessen in Ordnung
  für Typ 0x16 (§6-A).
* Die acht `beh = 0x02`-Zombies (Cases 2/4/6/8) — die sind im Original aufrecht.

---

## 8. Wie man es verifiziert

1. `bash re15_port/tools/local_build.sh` (Ziel `probe_1220_kriecher` wird mitgebaut).
2. `./re15_port/build/tests/unit/probe_1220_kriecher.exe`
   * Fall **A** (`RE1.5-cut0` / `RE2-cut0`) muss weiter kriechen: `+94clip=0x1A` bzw.
     `+10E=0x0001`, `+1D7sca=8`, Position wandert Richtung Spieler.
   * Fall **D** (`*-flags8A8B`) muss **nach dem Fix zwei Aktoren zeigen** (heute: null).
   * Fall **E** muss `ERGEBNIS Flag(7,0x8A) = 0` liefern (heute: 1).
3. Regressionswache: `probe_1010_kriecher`, `probe_1030_crawl_live`,
   `test_1030_trigger_chain` unverändert; volle ctest-Suite grün.
4. Im Spiel: ROOM1210 betreten, die zwei Writher am Nordende (−25000,−8847) und
   (−25000,−10247) erschießen, dann durch Tür-Slot 1 (Rect x −22200…−20700 / z −7200…−5000)
   nach ROOM1220 — dort müssen zwei Kriecher liegen bzw. sofort losskriechen.

---

## 9. Offene Punkte / NICHT GEFUNDEN

* **Hardware-Gegenprobe fehlt.** Es existiert kein DuckStation-Savestate von ROOM1220
  (`stage_saves/` enthält keinen). Die Kette ist statisch end-to-end belegt (jede Stufe mit
  Adresse) und port-seitig gemessen, aber **nicht** live auf PSX gegengeprüft. Nächster Weg:
  `.claude/skills/re15-room-capture` → ROOM1210 → Tür-Slot 1 → Savestate; dann `+0x94`/`+0x1D7`
  der beiden Aktoren lesen **und** zusätzlich nach einem Writher-Kill Flag-Bank 7 Bit 0x8A
  prüfen (das ist die eine Aussage, die dieses Dossier statisch beweist und die live
  gegengeprüft gehört).
* **Visuelle Verifikation in dieser Sitzung NICHT möglich.** `re15_pc.exe` stirbt hier direkt
  nach der Pad-Initialisierung (Exit 1; `debug.log` endet bei `[pad] …`). Die Sitzung ist eine
  RDP-Sitzung ohne Audio-Endpunkt (`SDL_OpenAudioDevice failed: WASAPI can't find requested
  audio endpoint`), und mit `SDL_VIDEODRIVER=dummy` scheitert `SDL_CreateRenderer`. Der
  `gdigrab`-Weg aus `re15-port-visual-verify` ist damit hier verschlossen.
* **Positiv-Liste der persistierenden Typen ist unvollständig.** Die 17 STAGE1-Treffer sind
  gefunden und gelistet, sauber abgegrenzt ist aber nur der Writher-Bereich (über die
  Zustandstabelle `@0x8012093C`). Für die Zombie-Dame 0x13 (`0x8010A8C8`, Code teilweise bei
  `0x80107CB0`) ist **NICHT GEFUNDEN**, welche Treffer zu ihr gehören. Ebenso ungeprüft:
  `STAGE{2..6}.BIN` mit demselben Muster (`e4 3b 01 0e`) scannen und die dortigen
  Flag-Kollisionen zensieren.
* **Kein raum-spezifischer Overlay-Eingriff für Raum 0x22 gefunden** — gesucht per `grep 0x22`
  in `STAGE1_overlay.c` (0 Treffer) und per vollständigem SCD-Walk aller drei Skripte des
  Raums. Damit ist ausgeschlossen, dass die acht `beh 0x02`-Zombies zur Laufzeit zu Kriechern
  werden.
* **Kriech-Geschwindigkeit** bleibt aus dem ROOM1030-Dossier unverändert offen (BLOCKER B);
  ROOM1220 erbt sie.
* **`+0x8C = 0x1E`** (Halbwort `@0x801036FC`) — Konsument weiterhin nicht verfolgt.

---

## 8. Verifikation (unabhaengig nachdisassembliert)

**Pruefer:** adversarialer Nachlauf 2026-08-27. Jede Adresse selbst mit
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` disassembliert, jedes RDT-Byte selbst mit
`od` gelesen, jeder Byte-Scan selbst in Python neu gefahren, die Sonde selbst ausgefuehrt.

**Gesamturteil: TEILWEISE.** Der technische Kern — "die Writher-Familie setzt im Original kein
em-Status-Kill-Flag, der Port tut es, und ROOM1210/1211 teilen sich alle zehn Flag-Indizes mit
ROOM1220/1221" — ist **vollstaendig bestaetigt**, teils mit staerkeren Belegen als im Dossier.
Vier Belegstellen sind jedoch fehlerhaft transkribiert bzw. zusammengeklebt, und die
**Symptom-Zuordnung in §0 traegt nicht**: der beschriebene Defekt erzeugt einen *leeren* Raum,
nicht "aufrechte Zombies statt Kriecher".

### 8.1 BESTAETIGT — byte-exakt nachgeprueft

**(a) ROOM1220.RDT sub00, alle zehn Records.** `od -A x -t x1 -j 0x0F20 -N 0x100` reproduziert die
Zeilen des Dossiers zeichengenau. Adresstabelle `[0x40]=0x0DD0` (main), `[0x44]=0x0F1C` (sub),
`[0x48]=0x101C`, Header `00 0a 03 00` (nCut=10, nOmodel=3) — alles bestaetigt. Alle zwanzig
Positionswerte selbst LE-dekodiert und mit der Dossier-Tabelle abgeglichen: **10/10 Records
identisch**, inkl. Deskriptor-Bytes @0x0F2D/0x0F41 = `81` und @0x0F5D/0x0F71/0x0F8D/0x0FA1/
0x0FBD/0x0FD1/0x0FED/0x1001 = `02`, em-Flag-Bytes @0x0F31=`8A`, @0x0F45=`8B`, @0x0F61..@0x1005 =
`E4`..`EB`. Beispiel slot9 @0x0FFE: `44 09 16 02 00 00 01 eb f2 c7 00 00 60 a5 00 00 00 00 00 00`
-> x=0xc7f2=-14350, z=0xa560=-23200, dirY=0. OK

**(b) main00 hat keinen Sce_em_set.** Voller Dump 0x0DD2..0x0F1C: 5x `3b` (Door_aot_set, je 32 B),
3x `2d` (Obj_model_set), 3x `50` (Item_aot_set), dann `01` (Evt_end). Kein `44`-Opcode. OK

**(c) Typ 0x16 haengt an der Zombie-Wurzel 0x80100424.** Selbst disassembliert:

```
8011e868: lui   v0,0x8010
8011e86c: addiu v0,v0,1060      -> 0x80100424
8011e8a4: sw    v0,11268(at)    -> 0x80072c04   ; (0x80072c04-0x80072bac)/4 = 22 = 0x16
```

Ebenso 0x80072bec/bf0/bf4 (0x10/0x11/0x12) und 0x80072c0c (0x18). OK

**(d) Typ 0x1A = Writher-Wurzel 0x8010C1EC.**

```
8011e880: lui   v1,0x8011
8011e884: addiu v1,v1,-15892    -> 0x8010c1ec
8011e9ac: sw    v1,11284(at)    -> 0x80072c14   ; (0x80072c14-0x80072bac)/4 = 26 = 0x1A
```

Und der Root dispatcht wirklich ueber die im Dossier genannte Tabelle:

```
8010c2a8: lui   at,0x8012
8010c2ac: addiu at,at,2364      -> 0x8012093c
8010c2b4: lw    v0,0(at)
8010c2bc: jalr  v0
```

Tabelle selbst gedumpt: `[0]=0x8010c33c [1]=0x8010c488 [2]=0x8010d0f8 [3]=0x8010d474
[4]=0x8010d768 [5]=0 [6]=0 [7]=0x8010d770` — identisch mit dem Dossier. OK

**(e) Der Writher-CORPSE-Handler ist buchstaeblich leer** (staerker als im Dossier behauptet):

```
8010d768: jr ra
8010d76c: nop
8010d770: jr ra          <<< Zustand 7 = CORPSE
8010d774: nop
```

Es gibt dort nicht nur keinen Flag-Setzer, es gibt dort ueberhaupt keinen Code. OK

**(f) Die Writher-Familie ist ein GESCHLOSSENER Codebereich 0x8010C1EC..0x8010D774.**
Eigene Zusatzpruefung, die das Dossier nicht macht: alle `jal`-Ziele im Bereich aufgelistet —
`0x80019700, 0x8001AF20, 0x8001AF5C, 0x8001BD60, 0x8001F314, 0x800245D8, 0x8002AEC4, 0x8002B498,
0x8002B544, 0x8003B0A4, 0x80065F60` — **ausnahmslos EXE-Helfer, kein einziger Sprung in fremden
Overlay-Code**, und `0x8004ef90` ist nicht darunter. Alle 11 `jalr`-Stellen im Bereich laufen ueber
die Tabellen `0x8012093C / 0x8012095C / 0x80120968 / 0x80120984 / 0x801209A0 / 0x80120C40 /
0x80120C94`; jede davon selbst gedumpt — **jeder Zielzeiger liegt in 0x8010c1ec..0x8010d770**:

```
0x8012095C: 0x8010c510 0x8010c58c 0x8010c58c 0x8010c608
0x80120968: 0x8010c608 0x8010c70c 0x8010c930 0x8010cb2c 0x8010ce4c 0x8010cfd8 0x8010ce4c 0x8010c678
0x80120984: 0x8010c678 0x8010c714 0x8010c938 0x8010cb34 0x8010ce54 0x8010cfe0 0x8010ce54
0x801209A0: (nur Index 8,9,24,25,32,33,40,41,48,49,56,57) alle -> 0x8010d188
0x80120C40: nur 0x8010d0b0 / 0x8010d0b8 / 0x8010d0d8
0x80120C94: nur 0x8010d4c4 (Idx 9,25,33,41,49) und 0x8010d5d0 (Idx 57)
```

(Die beiden `jalr` @0x8010D7A0/@0x8010D7E0 gehoeren zu den Funktionen ab 0x8010D778, die ueber
`0x80120F54` in HUND-Code 0x801108f0/0x80110dc0/... springen — die sind vom Writher-Root **nicht**
erreichbar.) Damit ist die Aussage "der Writher persistiert nie" nicht nur ein Byte-Scan-Argument,
sondern ein Erreichbarkeits-Argument. OK

**(g) Der Todes-Commit schreibt 7 nach +0x4.** Voller DEATH-Ast selbst disassembliert:

```
8010d4d4: lbu v1,7(a0)              ; +0x7 Phase
8010d4dc: beq v1,v0,0x8010d580      ; Phase 1 -> Clip abspielen
8010d4fc: ori v0,zero,0x2
8010d500: beq v1,v0,0x8010d5bc      ; Phase 2 ->
8010d504: ori v0,zero,0x7           ;   Delay-Slot: v0 = 7
...
8010d5bc: sw  v0,4(a0)              ; +0x4 = 7 = CORPSE
```

Kein `jal 0x8004ef90` im gesamten Ast 0x8010d474..0x8010d5cc. OK

**(h) Byte-Scan STAGE1.BIN: 17 Treffer, exakt die gelisteten Adressen.** Eigener Lauf
(Python, wortaligned, `off = addr - 0x80100000`) liefert **exakt** 0x80106E0C, 0x8010716C,
0x801074B0, 0x801076EC, 0x80107DC4, 0x801082A4, 0x801082C4, 0x80109100, 0x801096FC, 0x80110FBC,
0x801111D4, 0x801146EC, 0x801152C4, 0x80116AC8, 0x8011B8F4, 0x8011BAFC, 0x8011BD24 — und **null**
Treffer in 0x8010C1EC..0x8010D7F8, auch keine unaligned. OK (aber siehe 8.2-(1): das im Dossier
zitierte Suchmuster ist falsch abgeschrieben)

**(i) EXE-Gegenprobe: 12 Treffer, keiner liest +0x1C6.** Eigener Scan ueber `info/Re1.5/PSX.EXE`
(`t_addr=0x80010000` aus Header +0x18, Text ab Datei-0x800) liefert genau die 12 Adressen des
Dossiers. Jede a1-Quelle selbst nachgesehen: `lhu a1,4(s1)` (0x8001E0D0), `lhu a1,4(s0)`
(0x80021FD8), `lbu a1,DAT_800ACC17` (0x80030CD8/0x80030F2C/0x800311AC/0x8003131C/0x800313F8 — bei
den drei mittleren erst nach Rueckwaerts-Lesen bis 0x80031198/0x80031308/0x800313E4 sichtbar),
`lbu a1,451(v0)` = +0x1C3 (0x80051414/0x80051780/0x80051AB4/0x80051CAC/0x80051DD8). **Kein `454`.** OK

**(j) Das Flag-Primitiv.** `0x8004ef90` selbst disassembliert und als MSB-first-Bit-Set bestaetigt:

```
8004ef90: srl v1,a1,5 / sll v1,v1,2 / addu v1,v1,a0 / andi a1,a1,0x1f
8004efa0: lui v0,0x8000 / lw a0,0(v1) / srlv v0,v0,a1 / or v0,v0,a0 / sw v0,0(v1)
```

(0x8004efb8 = clear, 0x8004efe4 = get.) OK

**(k) `re15_em_status_zone()` ist byte-true.** Der Bank-Waehler am Zombie-Commit selbst gelesen:

```
801096b8: lui v0,0x800b
801096bc: lh  v0,4064(v0)         ; 0x800b0fe0 = Stage
801096c4: slti v0,v0,3
801096c8: beq  v0,zero,0x801096e8 ; Stage>=3 -> Bank s1+18644 = 0x800B1058
801096cc: addiu a0,s1,18612       ; Stage<3  -> Bank 0x800B1038   (s1 = 0x800ac784)
```

Deckt sich mit `enemy_ai_common.c:11337-11341` (Zone 7 fuer Stage 0..2). OK

**(l) Flag-Kollisions-Zensus — selbst nachgefahren, Ergebnis identisch.** Eigener Walker ueber alle
`STAGE*/ROOM*.RDT` (Record-Heuristik: Opcode 0x44 mit y==0, dirX==0, dirZ==0, 0x10<=Typ<=0x50):

```
STAGE1 0x8A  ROOM1210:t0x1a/b0x0  ROOM1211:t0x1a/b0x0  ROOM1220:t0x16/b0x81  ROOM1221:t0x16/b0x81
STAGE1 0x8B  dito
STAGE1 0xE4..0xEB  ROOM1210/1211:t0x1a/b0x0  ROOM1220/1221:t0x16/b0x02
```

Und: **Typ 0x1A existiert game-weit NUR in ROOM1210 (10 Records @0x1D86..@0x1E3A) und ROOM1211
(10 Records @0x1E86..@0x1F3A)** — die Behauptung "der Ausschluss von 0x1A ist regressionsfrei"
haelt. OK

**(m) ROOM1210.RDT Records + Tuer.** `od -j 0x1D86 -N 0xD0` reproduziert die zehn 0x1A-Records
zeichengenau (Flags 0x8A,0x8B,0xE4..0xEB in dieser Reihenfolge, alle beh=0x00).
Tuer @0x1CE6 selbst dekodiert: `pc[1]=0x01` (Slot 1), Rect x=0xa948=-22200 / z=0xe3e0=-7200 /
w=0x5dc=1500 / d=0x898=2200, Ziel x=0xa880=-22400 / z=0xe69c=-6500, yaw=0x0800=2048,
`pc[22]=0x00`, `pc[23]=0x22`, `pc[24]=0x00` (Cut 0). OK
**Eigener Zusatz-Zensus** ueber alle 240 RDTs nach Door_aot_set mit Ziel stage 0 / room 0x22:
nur ROOM1210 und ROOM1211, je 5 Tueren, Slots 1..5 -> Cuts 0/2/4/6/8. Kein weiterer Eingang. OK

**(n) Die Kriech-Kette (§3a).** Alle Ankerpunkte selbst disassembliert und bestaetigt:
`80100cac andi v0,v1,0x80` / `80100cb0 beq v0,zero,0x80100e30` / `80100cb4 andi v1,v1,0x1f`;
`80100d40 ori v0,zero,0x1` / `80100d44 beq v1,v0,0x80100d54` / `80100d48 ori v0,zero,0x3` /
`80100d4c bne v1,v0,0x80100d6c` -> `80100d54 ori v0,zero,0xc` / `80100d58 sb v0,148(a0)` /
`80100d64 ori v0,zero,0x5` / `80100d68 sb v0,5(v1)`;
Tabellen `0x8011f80c[1]=0x80101708`, `[2]=0x80101784`; `0x8011f8e0[5]=0x8010466c`;
`0x8011f920[5]=0x80104808`; `80104718 sltiu v0,v0,0xbb8`; `80104748 sw v0,4(v1)`;
`8010371c sb 0x1a,148(v1)`; `8010374c sb 8,471(v1)`; `801050d0 ori v0,zero,0x81` /
`801050d4 sb v0,9(v1)`. **Alle 1:1 wie zitiert.** OK

**(o) Port-Zitate.** `enemy_ai_common.c:11502-11503` steht woertlich so da (Kommentar ab 11497,
nennt tatsaechlich nur die beiden Zombie-Adressen). `scd_vm.c:3227-3228` = das Spawn-Gate wie
zitiert. `enemy_ai_common.c:11337-11341` = `re15_em_status_zone()`. `rdt_common.c:238-239` =
`main_scd_start`/`sub_scd_start` aus `[0x40]`/`[0x44]`. `enemy_ai_common.c:4180-4184` DECIDE[5],
`:4241-4244` ANIMATE[5], `:4770-4771` `shares_root`, `enemy_ai_re2_zombie.c:7024-7040`
`re15_re2z_enter_crawler`. `re15_damage.c:2143-2147` Writher-Hitbox r=300. **Alle vorhanden.** OK

**(p) Die Kill-Kette im Port ist real, nicht nur in der Sonde erzwungen.**
`re15_damage.c:1630 if (e->hp < 0) e->state = 3;` (@0x80013020) und
`enemy_ai_common.c:10469 case 3:` -> `e->state = 7` (`sw 7 into +0x4 @0x8010d5bc`). Mit Spawn-HP 0
und der 300er-Hitbox reicht ein Treffer. Die Sonde springt nur die Abkuerzung (`e->state = 3`
direkt, `probe_1220_kriecher.c:287`), der Weg dorthin existiert aber. OK

**(q) Sonde selbst ausgefuehrt** (`re15_port/build/tests/unit/probe_1220_kriecher.exe`, Binary lag
gebaut vor). Alle vier zitierten Bloecke reproduziert:

```
RE1.5-cut0 f1  slot1 +04state=1 +05=0 +06=1 +09grid=0x81 +94clip=0x1A +95=1 +1D7sca=8
RE1.5-cut0 f30 slot1 +05=2 +94clip=0x1B pos=(-23137,-6044)
RE2-cut0   f0  slot1 +10E=0x0001 +1D7sca=8
RE2-cut0   f30 slot1 +94clip=0x05 pos=(-24184,-6653)
=== RE1.5-cut0-flags8A8B ===   (leer)
=== RE2-cut0-flags8A8B  ===    (leer)
  nach 240 Frames: slot1 state=7  Flag(7,0x8A)=1
```

OK — Fall A, D und E exakt wie im Dossier.

### 8.2 WIDERLEGT / KORRIGIERT

**(1) Das zitierte Suchmuster ist falsch.** Das Dossier (§3c und §7) nennt
"`jal 0x8004ef90` = Wort `0x0E013BE4` = Bytes `e4 3b 01 0e`". Mit **genau diesem** Muster liefert
mein Scan ueber STAGE1.BIN **0 Treffer**. Die echte Kodierung steht in den rohen Bytes
(`re15_disasm.py bytes 0x801096f0 24 --bin STAGE1.BIN`):

```
801096f0: 00 00 00 00  c6 01 45 90  d4 48 24 26  e4 3b 01 0c
                                                 ^^^^^^^^^^^  = 0x0C013BE4 = jal 0x8004ef90
```

(`0x8004ef90 >> 2 = 0x02013BE4`, `& 0x03FFFFFF = 0x0013BE4`, `| 0x0C000000` -> `0x0C013BE4`.
`0x0E013BE4` dekodiert zu `jal 0x8804ef90` — keine gueltige Adresse.)
Mit dem **korrekten** Muster `e4 3b 01 0c` kommen exakt die 17 gelisteten Adressen heraus. Das
**Ergebnis** des Dossiers stimmt also, die **Beweisfuehrung** ist nicht nachvollziehbar wie
abgedruckt. Wer den Scan nachbaut, muss `e4 3b 01 0c` nehmen.

**(2) Das §3c-Zitat des Zombie-Flag-Setzers zeigt den FALSCHEN Zweig.** Zitiert wird
`801096e8..801096fc` mit `addiu a0,s1,18644`. Fuer STAGE1 (Stage 0) wird dieser Zweig **nie**
ausgefuehrt — er ist der `Stage>=3`-Pfad (Bank 0x800B1058 = Zone 8). Der genommene Pfad ist:

```
801096c4: slti  v0,v0,3
801096c8: beq   v0,zero,0x801096e8
801096cc: addiu a0,s1,18612        ; Delay-Slot -> Bank 0x800B1038 = Zone 7
801096dc: lbu   a1,454(v0)
801096e0: j     0x801096fc
```

Inhaltlich aendert das nichts (beides `jal 0x8004ef90` mit `a1 = +0x1C6`), aber die im Dossier
abgedruckte Zeile belegt fuer STAGE1 die falsche Bank.

**(3) Das §3c-Zitat des Writher-Todes-Commits ist ZUSAMMENGEKLEBT.** Das Dossier druckt
0x8010d580...0x8010d5bc als einen fortlaufenden Block, an dessen Ende `sw v0,4(a0)` steht. Real ist
`0x8010d5bc` **kein Fallthrough**, sondern ein eigenes Sprungziel aus der Phasen-Weiche:

```
8010d500: beq v1,v0,0x8010d5bc     ; Phase == 2
8010d504: ori v0,zero,0x7          ; Delay-Slot liefert die 7
...
8010d5b4: j   0x8010d5c0           ; <- Ende des zitierten Phase-1-Blocks
8010d5b8: sb  v1,7(a0)             ;    Delay-Slot: Phase++
8010d5bc: sw  v0,4(a0)             ; <- separates Ziel, Phase 2
```

Die Schlussfolgerung ("kein `jal 0x8004ef90` im ganzen Ast") ist trotzdem korrekt — ich habe den
kompletten Ast 0x8010d474..0x8010d5cc gelesen. Aber das Zitat suggeriert einen Kontrollfluss, den
es nicht gibt. Genau der Fehlertyp, vor dem der Auftrag warnt.

**(4) §0 erklaert NICHT das gemeldete Symptom.** Das Dossier schreibt: "Der Spieler betritt
ROOM1220 durch die Tuer, hinter der die zwei Kriecher liegen — und trifft nichts an. Die Zombies,
die er im Raum antrifft, sind die der anderen vier Eingaenge. [...] Genau das beschreibt der
Nutzer." Das traegt nicht, und zwar aus dem Datenstand des Dossiers selbst:
`sub00` ist ein `Switch(work_vars[0x0A])` **ohne Default** (§2, @0x1014 `Eswitch` direkt nach
Case 8) — pro Raumbesuch laeuft **genau ein** Case und spawnt **genau ein** Paar. Die Paare der
anderen vier Eingaenge existieren in diesem Besuch gar nicht. `work_vars[0x0A]` wird beim Betreten
aus dem Tuer-Byte `pc[24]` gesetzt (`scd_room_setup.c:218`, `room_common.c:333-339`,
`@0x8001d930 lbu v1,10(a0)` / `@0x8001d948 sh v1,4068(at)`).
=> Mit gesetzten Flags 0x8A/0x8B und Eintritt ueber Cut 0 sieht der Spieler einen **voellig leeren
Raum** — nicht "aufrechte Zombies statt Kriecher". Das ist auch genau das, was die eigene Sonde in
Fall D zeigt (null Aktoren). Das gemeldete Symptom ("die Zombies sollten kriechen") setzt voraus,
dass der Spieler Zombies **sieht**, also ueber Cut 2/4/6/8 hereinkam — und dort sind sie per
Auslieferungs-Datenstand `beh=0x02` aufrecht. Erklaerend ist damit der "zweite, kleinere Punkt"
aus §5, nicht der Flag-Kollisions-Defekt. Der Flag-Defekt ist ein echter, unabhaengiger Port-Bug
(bestaetigt), aber er ist **nicht** die Ursache des Nutzer-Reports. §0 muss umgeschrieben werden.

**(5) §2 `extra_scd`-Bytes sind um 4 verschoben.** Das Dossier schreibt "`extra_scd @0x101C` ist
keine SCD (Byte 0 = `01 00 00 00 04 00 ...`)". Die zitierten Bytes stehen bei **0x1018** (= `sub01`,
`01` = `Evt_end`). Bei 0x101C steht `04 00 06 00`. Die Aussage ("keine SCD") bleibt damit unbelegt;
das Zitat stuetzt sie nicht.

**(6) §2 Distanz 2598 ist der Port-Naeherungswert, nicht die Geometrie.** Tuer-Ziel (-22400,-6500)
zu Kriecher 0 (-25000,-6700): dx=2600, dz=200 -> exakt `sqrt(6.800.000) = 2607,7`. Die 2598 stammen
aus der `dist=`-Spalte der Sonde (BIOS-`SquareRoot0`-Approximation). Fuer die Aussage
"< 0xBB8 = 3000" irrelevant, aber es ist keine gerechnete Zahl.

**(7) §3a "Weck-Schwelle 0xBB8" ist unvollstaendig.** `sltiu v0,v0,0xbb8` @0x80104718 ist nur die
**erste** von zwei Bedingungen. Selbst gelesen:

```
80104718: sltiu v0,v0,0xbb8
8010471c: beq   v0,zero,0x8010474c     ; dist >= 3000 -> KEIN Wecken
80104724: lui   a0,0x800b
80104728: addiu a0,a0,-13688           ; playerX
8010472c: jal   0x8001a9cc             ; arc_test
80104730: ori   a1,zero,0x200          ; Kegel 0x200
80104734: bne   v0,zero,0x8010474c     ; arc_test != 0 -> KEIN Wecken
80104738: ori   v0,zero,0x1
80104748: sw    v0,4(v1)               ; +0x4 = 1
```

Wer nur `0xBB8` in den Port uebernimmt, laesst den `arc_test(..., 0x200)`-Filter weg.

**(8) Zeilen-Zitate leicht daneben (kosmetisch).** `scd_vm.c:3226-3228` -> real `3227-3228`.
`scd_vm.c:3113-3130` (Feldlayout) -> der Layout-Kommentarblock liegt bei `~3132-3145`.
`enemy_ai_re2_zombie.c:7025-7042` -> real `7024-7040`. Kein inhaltlicher Fehler.

### 8.3 NICHT GEPRUEFT / weiterhin offen

* **Hardware-Gegenprobe fehlt weiterhin** (das Dossier sagt es selbst). Keine PSX-Messung, weder
  fuer die Kriech-Pose noch fuer "Writher-Kill setzt Flag-Bank 7 Bit 0x8A nicht".
* **NICHT GEFUNDEN: setzt der ROOM1220-KRIECHER im Original sein Flag?** Das Dossier belegt nur die
  Gegenrichtung. Nach FIX 1 bleibt die Unterdrueckung "ROOM1220-Kill -> ROOM1210-Writher spawnt
  nicht mehr" bestehen. Welcher der 9 Zombie-Treffer im Kriech-Pfad (Grid-Wurzel
  `0x8011f80c[1] = 0x80101708`) liegt, hat weder das Dossier noch dieser Nachlauf aufgeloest.
  Naechster Weg: den DEATH-Ast der Grid-1-Maschine ab 0x80101708 durchdispatchen und pruefen,
  welcher der Treffer 0x80106E0C/0x8010716C/0x801074B0/0x801076EC/0x80107DC4/0x801082A4/
  0x801082C4/0x80109100/0x801096FC von dort erreichbar ist.
* **Unbemerkte Sonden-Anomalie (eigener Befund):** in Fall A wandert der RE1.5-Kriecher slot1 bis
  f300 auf `pos=(-12072, 12082)` (`dist=21184`) — z = **+12082**, weit ausserhalb des Raums (alle
  RDT-Records liegen bei z ~ -6700...-23200). Dasselbe im "RE1.5-vorne"-Lauf:
  `pos=(-25080, 14553)`. Das ist im Dossier nicht erwaehnt und riecht nach einem eigenen
  Lokomotions-/Kollisions-Defekt der Kriech-Maschine. Ungeklaert.
* **Positiv-Liste der persistierenden Typen** bleibt unaufgeloest (wie im Dossier §9 eingeraeumt);
  `STAGE{2..6}.BIN` wurden auch hier nicht gescannt.

### 8.4 Empfehlung an den Implementierer

FIX 1 (`e->type != 0x1A` in `enemy_ai_common.c:11502`) ist **belegt und uebernehmbar** — die
Beweislage dafuer ist nach diesem Nachlauf sogar besser als im Dossier (Erreichbarkeits-Argument
8.1-(f) statt nur Byte-Scan). Beim Uebernehmen des Kommentars aber:

* Byte-Muster auf `e4 3b 01 0c` korrigieren (nicht `...0e`),
* die 0x801096FC-Zeile als Stage>=3-Zweig kennzeichnen oder durch 0x801096CC/0x801096DC ersetzen,
* nicht behaupten, der Commit @0x8010D5BC sei der Fallthrough des Clip-Blocks.

§0 des Dossiers ist **umzuschreiben**: der Nutzer-Report wird durch den Auslieferungs-Datenstand
(`beh=0x02` an Cut 2/4/6/8) erklaert, nicht durch die Flag-Kollision. Die Flag-Kollision ist ein
separater Bug mit dem Symptom "ROOM1220 ist leer".
