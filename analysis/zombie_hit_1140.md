# Zombie-Hit-Reaktion ROOM1140 (Typen 0x10/0x11/0x16) — RE-Dossier

Stand 2026-08-02. Quellen: `RE_15_Quellcode_Overlays/STAGE1_full/FUN_*.c` (Ghidra-Decompile der
Overlay-Funktionen), Raw-MIPS direkt aus `info/Re1.5/PSX/BIN/STAGE1.BIN` (laedt headerlos
@0x80100000, verifiziert: `PTR_FUN_801217a0[0..4]` @File-Offset 0x217a0 = 8011d84c/8011d9f4/
8011db40/8011db88/80050be8), `ghidra1_V2.txt` (EXE), Port-Quellen `re15_port/engine/src/*`.
Dynamische Verifikation: Scratchpad-Sonde gegen `libre15_engine.a` (Szenarien A/A2/B/B2/C, Ausgaben
unten woertlich zitiert).

## 0. Symptome (Nutzer-Report)

- **(a)** Ein Zombie, der den Spieler gebissen hat und weggestossen wird (Grab-Release /
  Push-away-Animation), zeigt im Port merkwuerdiges Verhalten, wenn er WAEHREND dieser Animation
  angeschossen wird.
- **(b)** Auf einen am Boden liegenden (noch lebenden) Zombie schiessen zeigt im Port falsches
  Verhalten.

---

## 1. Original-Mechanismus — vollstaendige Karte

### 1.1 Schuss-Anwendung (EXE): FUN_80011f50

Der Gun-/Knife-Resolver. Pro Schuss: Kandidaten-Loop ueber die Entity-Liste (DAT_800acc2c,
Stride 0x1f4), pro Kandidat Band-Gate + Hit-Tester `PTR_LAB_8006e548[weapon]`, dann Anwendung.
Raw-MIPS (ghidra1_V2.txt:76144-76235), woertlich:

```
800123f4 ori  v0,v0,0x80        ; FUN_8001a780 != 0 -> +0x93 |= 0x80 (Front/Back-Latch)
800123f8 sb   v0,0x93(s1)
80012404 andi v0,v1,0x1         ; +0x93 bit0 = hit-once-Guard
80012408 beq  v0,zero,LAB_80012428
8001240c _ori v0,v1,0x2         ; Guard gesetzt -> +0x93 |= 2 ...
80012418 jal  FUN_80011f50      ; ... und REKURSION (naechstes Ziel; das Pick-Gate
                                ;     @0x80012060ff schliesst (+0x93&3)==3 aus:
                                ;     `(&DAT_800accbc)[..] & 0x3000000 == 0x3000000`)
80012428 sb   zero,0x7(s1)      ; +0x7 = 0 (Reaktions-Phase)
80012430-50                     ; +0x6 = DAT_8006f410[player_word0 >> 29] (= {07,00,01,07,02,..}:
                                ;     DOWN-Band->0, LEVEL->1, UP->2)
8001245c-24ac                   ; zwei Y-Korrekturen: playerY<enemyY && +0x6==0 -> 1;
                                ;     enemyY<playerY && +0x6==2 -> 1
800124bc sb   s8,0x5(s1)        ; +0x5 = WEAPON-ID  (unbedingt!)
800124c0-d4                     ; dmg = u16[0x8006e0d0 + type*0x58 + weapon*4]
800124e4/f4/12504               ; hp(+0x9a) -= dmg
800124f8 sb   ... 0x93          ; +0x93 |= 1
800124fc-1251c                  ; (+0x93&0x40 && type<0x20) -> hp = -1  (Crit: weapon 7, oder 8 bei
                                ;     dist<3000; Flag-Set frueher im Apply-Block, Decompile Z.146-148)
80012520-38                     ; +0x4 = 2 (hp>=0) sonst 3
```

**Kernaussage:** Der Schuss ueberschreibt IMMER `+0x5=weapon`, `+0x6=Hoehen-Code`, `+0x7=0` und
erzwingt `+0x4=2/3` — voellig egal, in welchem Zustand (Grab, Liegen, Recovery) der Zombie war.
Es gibt KEIN Zustands-Gate ausser dem hit-once-Guard `+0x93&1` und den Band-Bits.

Band-Bits (wer ist ueberhaupt treffbar): der State-1-Handler FUN_80101224 pflegt sie pro Tick
(Decompile `FUN_80101224.c` Z.48-56):
```c
if ((+0x9 & 0x80) == 0) { word0 = (word0 & 0x1fffffff) | 0x40000000; }   // stehend: LEVEL
func_0x80012aa4(3000);
if ((+0x9 & 0x80) != 0) { word0 &= 0xbfffffff; func_0x80012974(5000); }  // liegend: LEVEL weg,
                                                                          // DOWN-Band wenn dist<5000
```
=> Ein liegender Zombie ist NUR mit Nach-unten-Zielen treffbar (und nur < 5000 Einheiten); ein
stehender nur mit LEVEL. (FUN_80012974 @0x800129cc-f0: `dist<R -> word0 |= 0x20000000`.)

### 1.2 HURT-State [2]: Router FUN_80105a8c

Haupt-State-Tabelle der Live-Zombies @0x8011f7b4 (Dump aus STAGE1.BIN, File-Offset 0x1f7b4):
`[0]=FUN_80100688 INIT, [1]=FUN_80101224 ACTIVE, [2]=FUN_80105a8c HURT, [3]=FUN_80106ba4 DEATH,
[4]=FUN_8010919c, [7]=FUN_80109554 CORPSE`.

Raw-MIPS des Routers (STAGE1.BIN @0x80105a8c):
```
80105a9c lbu  v0,9(a1) ; andi 0x80 ; beq zero -> 0x80105ae8   ; nicht liegend -> Standing-Pfad
80105ab0 lbu  v0,5(a1)                                        ; LIEGEND: +0x5 lesen (= WEAPON-ID,
80105ab8 addiu v0,v0,-18 ; sltiu v0,v0,2                      ;   s. 1.1!) — in {0x12,0x13}?
80105ac0 bne  v0,zero,0x80105ad8                              ;   ja  -> jal FUN_80106a38 (Prone-B)
80105ac8 jal  0x801068a0                                      ;   nein-> FUN_801068a0 (Standard)
80105ae8 ...  Standing-Dispatch: [0x8011fb90 + (+0x5)*0x20 + (+0x6)*4]() (jalr @0x80105b10)
80105b24 lh   v0,476(v1)          ; +0x1dc POISE nach dem Handler
80105b2c bgez -> return           ; >= 0 -> bleibt
80105b34-68  (+0x9&0x80)==0:  +0x4=1, +0x5=0x11, +0x6=0       ; POISE-BRUCH -> KNOCKDOWN
```

Standing-Dispatch-Tabelle @0x8011fb90 (Zeile = `+0x5` = Weapon, Spalte = `+0x6` = Hoehen-Code,
Dump aus STAGE1.BIN @0x1fb90; nur belegte Zellen):

| +0x5 (Weapon) | Spalte 0 | Spalte 1 | Bedeutung |
|---|---|---|---|
| 1 (Messer), 3, 4 | 80105b7c | 80105b7c | Standard-Stagger |
| 5, 6 | 80106290 | 80106290 | schwerer Stagger |
| 7 (Magnum), 8 (Shotgun) | — | 80106624 | Heavy (nur relevant fuer Typen >= 0x20; <0x20 stirbt am Crit) |
| 0x13 | 80106048 | 80106048 | eigener Stagger der 0x13-Waffenklasse |
| 0x15 | 80105a24/a4c/a6c (alle 8 Spalten) | | — kein Weapon-Row, s.u. @0x8011fe30 |

Alle uebrigen Zellen sind 0 — sie sind unerreichbar, weil das Band-Gate stehende Zombies auf
`+0x6==1/0` beschraenkt.

### 1.3 Standard-Stagger FUN_80105b7c (stehend, Pistole/Messer)

Decompile `FUN_80105b7c.c`, Phasen auf `+0x7`:

- **Phase 0** (Z.71-90): `+0x7=1, +0x9e=2, +0x9c=0, +0x8c=0x14, +0x8f=0` (harter Pose-Cut),
  `+0x94 = +0x1d4` (die Arms-up-Walk-Variante — **KEIN +0x95-Frame-Reset**, der Recoil ist der
  prozedurale Torso-Bend, kein eigener Clip); Blut-FX 0x2000 je nach `+0x6` (1: Brust-Bone
  `DAT_8011f784[type]*0xac+pool+0x40`; 0: pool+0x198; 2: KEIN Spawn), `+0x93|=1`, SE 6, dann
  `(&PTR_LAB_8011fe30)[+0x5]()` — die **Poise-Step-Tabelle @0x8011fe30** (21 Eintraege, Dump):
  Handler `0x80105a24`=jr ra (0), `0x80105a2c`=+0x1dc−1, `0x80105a4c`=−2, `0x80105a6c`=−3.
  Belegung: `[0]=0, [1..2]=−2 (Messerklasse), [3..6]=−3 (Pistolenklasse), [7..16]=0,
  [17..20]=−1`. Poise-Seed: INIT FUN_80100688 `*(u16*)(+0x1dc)=(rand&3)+4` (Decompile Z.47-48);
  Devour-Exit re-armt 3 (`FUN_801031e4.c` Z.40).
- **Phase 1** (faellt aus Phase 0 durch): f314 auf **Bank 0** (`+0x84/+0x16c`) Rate 0x200,
  Spine-Bend `pool+0x518 += +0x9c; +0x9c -= 0x80`, 3 Ticks (`+0x9e`), dann Phase 2.
- **Phase 2/3**: Bend zurueck (`+0x9c += 0x80`), 3 Ticks, dann **EXIT** (Z.31-67):
  `word +0x4 = 0x10201` (ENGAGE Sub 2 mit `+0x6=1` = Entry uebersprungen), `+0x94=+0x1d4`,
  `+0x8f=7`, Gait-Re-Roll `+0x1de` (Tabellen 8011fb00/8011faf0 nach Spawn-Count `DAT_800aca4e<5`),
  **`+0x1de==2 -> +0x5=0x13, +0x6=0`** (Wechsel in die 0x13-Approach-Gangart), `+0x93 &= 0xfe`
  (wieder treffbar), 1/16-Roll `word=0x801` (CHARGE), und `FUN_8001a780(player)!=0 ->
  word=0x701` (Attack-Commit, wenn der Spieler wegdreht).
- Jeder Tick endet mit `func_0x800245d8(0)` — Vorwaerts-Drift mit `+0x8c=0x14`.
- **Reihenweite Knockdown-Entscheidung liegt NICHT im Handler**, sondern im Router-Gate
  @0x80105b24-68 (s. 1.2): Poise `+0x1dc < 0` -> `1/0x11/0`.

### 1.4 Knockdown/Get-up FUN_8010512c (State 1, `+0x5=0x11`)

State-1-Sub-Tabellen (Decide @0x8011f840 / Animate @0x8011f890, Dump aus STAGE1.BIN):
`[0]=Search 80101b64/80101d08, [1]=Wander, [2]=Engage 80102058/801021f8, [3]/[4]=GRAB
80102540/80102548, [5]/[6]=Devour-Finish 80102bd0/80102bd8, [7]=Lunge/Turn, [8]=Charge,
[9]/[0xa]=Contact/Edge, [0xb]=Push-off, [0xc]=Feeding, [0xd]=Standup, [0x11]=KNOCKDOWN
80105124/8010512c, [0x12]=Sleeping-Lying 80105470/801054f4, [0x13]=Approach 8010561c/801057bc`.

`FUN_8010512c.c`, Phasen auf `+0x6`:
- **[0]** (Z.15-32): `+0x93|=1`, Fall-Clip `+0x94=0xb`, `+0x95=0`, `+0x8f=0xf`,
  RNG `(rand&3)==0 ->` SE 8/5, `+0x1b8=1`, **`+0x1dc=0x80` (Down-Sentinel)**, **`+0x9|=0x80`**.
- **[1]**: f314 Rate 0x100 -> `+0x6+=done`; `FUN_8010939c(0,0)`.
- **[2]** (Z.39-45): `+0x9|=0x80`, `+0x6=3`, Liege-Timer `+0x9c = DAT_8011fb10[rand&0xf]*30`,
  **`+0x93 &= 0xfe`** (ab Liegebeginn wieder TREFFBAR).
- **[3]**: `+0x9c--`; bei 0 -> `+0x6=4`.
- **[4]** (Z.53-68): Get-up-Clip `+0x94=0x12`, `+0x95=0`, `+0x6=5`, `+0x8f=0xf`,
  **`+0x93|=1`** (waehrend des Aufstehens UNVERWUNDBAR/durchschossen), `+0x1b8=0`,
  RNG `(rand&7)==0 ->` SE 8/5.
- **[5]**: f314 -> `+0x6+=done`.
- **[6]** (Z.74-79): `+0x9 &= 0x7f`, **`word +0x4 = 0x201`** (ENGAGE), `+0x93 &= 0xfe`,
  **Poise-Re-Arm `+0x1dc=(rand&3)+4`**.

### 1.5 Downed-Flinches FUN_801068a0 / FUN_80106a38

Der Downed-Router (1.2) waehlt nach dem frisch geschriebenen `+0x5` (= WEAPON): Waffen
0x12/0x13 -> FUN_80106a38, alle anderen (Pistole=3, Messer=1, ...) -> FUN_801068a0.
(0x12 kommt auch als Reaktions-Code von FUN_80012d60: `DAT_8006f430[attack_type 9] = 0x12`.)

**FUN_801068a0** (Standard, Raw-MIPS @0x801068f0-801069b4, selbst disassembliert):
```
801068f0 sb  s0,7(a0)            ; +0x7 = 1
80106900 sb  0x1e,148(v1)        ; Clip 0x1e (30)
80106910 sb  zero,149(v0)        ; +0x95 = 0
80106920 sb  3,143(v1)           ; +0x8f = 3
8010693c jal 0x80019700          ; Blut 0x1500 @ pool+0x4F4 (Bein-Part 7), Desc &DAT_8011fe9c
80106958 ori v0,v0,0x1 ; sb      ; +0x93 |= 1
8010695c jal 0x800453d0(6)       ; SE 6 (unbedingt)
80106970 lh  v1,476(a0)          ; +0x1dc
80106974 ori v0,zero,0x80
80106978 bne v1,v0,0x801069a4    ; NUR wenn +0x1dc == 0x80 (Knockdown-Sentinel):
80106980 sb  s0,4(a0)            ;   +0x4 = 1
80106990 sb  0x11,5(v1)          ;   +0x5 = 0x11
801069a0 sb  4,6(v1)             ;   +0x6 = 4  -> SOFORT ins Get-up (1.4 [4])
801069b0 lw a0,368(v0); lw a1,372(v0) ; f314 aus +0x170/+0x174 (Default-Bank), Rate 0x400
```
- Phase 1: `+0x7 += f314done`; Phase 2 (`FUN_801068a0.c` Z.17-22):
  `word +0x4 = 1` (State 1, Subs 0/0/0), `(+0x9&0x1f)==3 -> +0x5=5`, `+0x93 &= 0xfe`.

**FUN_80106a38** (Prone-B, Decompile): Clip `0x25+(rand&1)` (37/38), `+0x8f=4`, Blut 0x2000 @
pool+0x40 (Part 0), `+0x93|=1`, SE 6 nur 1/8, f314 Rate 0x200, **KEIN** Sentinel-Get-up;
Exit identisch (`word=1`, grid-3-Sonderfall, Guard-Clear).

### 1.6 DEATH-State [3]: FUN_80106ba4 -> Tabelle @0x8011feac / FUN_80107cb0

Router `FUN_80106ba4.c`: nicht liegend -> Dispatch `[0x8011FEAC + (+0x5)*0x20 + (+0x6)*4]`;
liegend -> **FUN_80107cb0**. Tabelle (Dump, belegte Zellen): Zeilen 1/3/4/5/6 Spalten 0/1 ->
**FUN_80106c18** (Steh-Death: Clip `((s8)+0x93>>7)*2+0xd` -> 0xb vorwaerts bei Front/Back-Latch
`+0x93&0x80`, sonst 0xd); Zeile 7: Sp.0=FUN_80108abc (Gib), Sp.1=**FUN_80106edc** (Magnum-Gore,
50%-Defer); Zeile 8: Sp.1=FUN_80107ee0, Sp.6=FUN_80106c18; Spalte 4 in JEDER Zeile =
FUN_80107634; Zeile 0x15 Sp.0=FUN_80108d74 (Ziel des `word=0x1503`-Timeouts aus FUN_80101224).

**FUN_80107cb0** (Downed-Death, Decompile): `+0x93|=1`, Clip **0x1f**, `+0x8f=0xf`,
FX 0x2500 @pool+0x40, `+0x1b8=1`, Voice via 8004ef90 (b1038/b1058 nach DAT_800b0fe0); Frame 7:
SE 8/5-RNG; Frame 0x23: Gore 0x2000 am Typ-Bone; f314 Rate 0x100 -> Corpse.

### 1.7 Grab-FSM FUN_80102548 (State 1, `+0x5=3/4`, Sub-Steps `+0x6`=0..8)

Decompile `FUN_80102548.c` (Zitate mit Zeile):
- **[0]** (Z.14-29): `+0x6=1`, Clip `(+0x5-3)*3`, `+0x95=0`, `+0x8f=7`, Anker FUN_8001ac38,
  **`word0 |= 0x1000`** (Zombie UND Spieler), Grab-Links acbcc/acbd0, **`DAT_800aca58 =
  (+0x5-3)*0x100 | 5`** (Spieler-Cmd 5), `DAT_800acae7 |= 1` (Spieler-Grab-Flag), Yaw-Snap
  a8f8(0x800), **`+0x1d8 |= 1`**, SE 4.
- **[1]**: ad68-Root-Motion + f314(0x200) -> `+0x6+=done`.
- **[2]** (Z.35-43): Clip base+1, Escape `+0x9c=0x6e` (5 bei Mercy `DAT_800aca50&1`),
  `+0x9e=100`, Spieler-HP −10.
- **[3]** (Z.44-66): pro Clip-Wrap −5 + Blut (Typen 0x13/0x14 anderer Bone); Mash 37024:
  `+0x9c -= 1+5*mash`; `+0x9c<0 -> +0x6=4` **und `DAT_800aca5a=4`** (Spieler-Release-Phase,
  `CONCAT12(4,aca58)` Z.59); `+0x9e--`; `+0x9e==0 || hp<0 -> word = ((+0x5)+2)<<8|1` (Devour 5/6).
- **[4]** (Z.67-76): Clip base+2 (Wegstossen), `+0x8f=7`, SE 7 (2x), `+0x1d5=0x5a`,
  `DAT_800aca50|=1` (Mercy), **`+0x1d8 &= ~1`**.
- **[5]** (Z.77-90): ad68+f314; `+0x95==0x18 -> +0x6=6`; Domino-Shove (Kontakt `+0x1c2&2`,
  Ziel lebendig/nicht liegend/`+0x1d8&1` frei -> Ziel-`word=0xb01`).
- **[6]** (Z.91-99): Clip 0x11, `+0x8f=0xf`, Speed `+0x8c=0x32`, `+0x9f=1`,
  **`DAT_800acae7 &= 0xfe`** (Raw @0x80102ac4-ad0), `+0x9e=2`.
- **[7]** (Z.100-112): **Raw @0x80102aec-afc:**
  ```
  80102aec ori a2,zero,0x1        ; REVERSE ist das a2-ARGUMENT dieses EINEN Calls
  80102af0 lw  a0,368(v0)         ; +0x170
  80102af4 lw  a1,372(v0)         ; +0x174
  80102af8 jal 0x8001f314         ; anim_set(bank, clip, reverse=1, rate)
  80102afc ori a3,zero,0x100
  ```
  `+0x9f -= done; ==0 -> +0x6=8`; Rueckwaerts-Slide `800245d8(0x800)`, Speed −2/Tick min 10.
- **[8]** (Z.113-116): `word +0x4 = 0x201`, **`word0 &= ~0x1000`**.

**Wichtig fuer (a):** Der Zombie ist waehrend des GESAMTEN Grabs inkl. Release/Recovery treffbar
(kein `+0x93|=1` auf dem Zombie in FUN_80102548). Ein Schuss in [4..7] reisst ihn per 1.1 hart
nach State 2/3. Das Original hat dabei NICHTS aufzuraeumen: das Rueckwaertsspielen war nur ein
Call-Argument, die Spieler-Release-Phase (`aca5a=4`) laeuft seit [3] autonom weiter. Es LEAKT im
Original nur `word0&0x1000` (bis zum naechsten vollstaendigen Grab) — Konsumeqnt ist das
Windup-Death-Gate `FUN_80101224.c` Z.12: `if ((+0x1d8 & 0x100) && !(word0 & 0x1000)) { +0x1da-- ...
bei 0: word=0x1503 }`, d.h. der zeitgesteuerte Gib-Tod ist solange blockiert (Original-Quirk).

### 1.8 State-1-Architektur: Grid-Nibble-Dispatch + der Scripted-Lyer

`FUN_80101224` endet mit `(*(&PTR_FUN_8011f80c)[+0x9 & 0xf])()` (Decompile Z.57). Tabelle
@0x8011f80c (Dump): `[0]=0x8010168c` (Standard: Decide/Animate-Tabellen aus 1.4),
`[5]/[6]=0x801018f8` (Feeding-Wake), **`[7]/[8]=0x80101974`**, `[9..12]=...`.

`0x80101974` (Raw, oben disassembliert) ist ein Doppel-Dispatcher ueber die LYING-Tabelle
@0x8011f9d4 = `{80103A58, 801039F4, 801039FC, 80109E44, 80109E4C, 80104B38, 80104B40}`:
erst `[0x8011F9D8 + (+0x5)*4]()`, dann `[0x8011F9D4 + (+0x5)*4]()`. Fuer den ROOM1140-Lyer
(Typ 0x16, grid 0x88, `+0x5=0`): 0x801039F4 (Stub) + **FUN_80103a58** (Wake-Maschine, `+0x6`):

```
80103aac lbu v0,147(a1)     ; Phase 0 (passiv): JEDEN Tick
80103ab4 ori v0,v0,0x1      ;   +0x93 |= 1   -> der passive Lyer ist UNSCHIESSBAR
80103ab8 sb  v0,147(a1)     ;   (FUN_80011f50-Guard: kein Schaden, +0x93|=2, Rekursion)
```
- Phase 1: `+0x9c--`, bei 0 -> Phase 2 (`+0x8f=0xf`); Phase 2: f314(+0x170/+0x174, 0x100) ->
  Phase 3; Phase 3: **`+0x9 = 0`** (Downed-Bit UND Nibble weg), `word +0x4 = 0x201`,
  `+0x93 &= 0xfe`, `+0x1b8=0`. (Wake-Ausloeser fuer Phase 0->1: OFFEN, s. §5.)

---

## 2. Was das Original in EXAKT den zwei Situationen tut

### 2.1 (a) Schuss waehrend Grab-Release/Push-away ([4..7])

1. FUN_80011f50 wendet an (1.1): `+0x5=weapon(3)`, `+0x6=1`, `+0x7=0`, hp−=5 (Pistole,
   Zombie-Zeile @0x8006e650), `+0x93|=1`, `+0x4=2` (bzw. 3).
2. Naechster Tick FUN_80105a8c: nicht liegend -> `fb90[3][1]` = **FUN_80105b7c**: Stagger wie 1.3
   (Walk-Clip `+0x1d4` mit weiterlaufendem `+0x95`, Torso-Bend 6 Ticks, Blut, SE 6, Poise −3).
   Das Rueckwaertsspielen des Recovery-Clips ENDET dabei von selbst — es war nur das
   a2=1-Argument @0x80102aec, kein Zustand.
3. Exit: `0x10201` ENGAGE (oder 1/16 CHARGE 0x801, oder 0x701 Attack bei abgewandtem Spieler);
   Poise-Bruch -> Knockdown `1/0x11/0` (Fall-Clip 0xb).
4. Bei hp<0: Steh-Death **FUN_80106c18** (Clip 0xd, bzw. 0xb bei `+0x93&0x80`).
5. Spielerseite laeuft autonom: `aca5a=4` stand seit dem Escape ([3], Z.59); `acae7`-Bit0 wird bei
   [6] geloescht — faellt der Zombie VOR [6] aus dem Grab, uebernimmt die Spieler-Struggle-FSM
   (EXE @0x80036774/800367a4-Region; exakte Phase OFFEN §5.3, Ergebnis identisch: Leon spielt den
   Push-away zu Ende und ist frei).
6. Original-Leak: `word0&0x1000` bleibt bis zum naechsten Grab-[8] gesetzt und blockiert solange
   NUR das `+0x1da`-Windup-Death-Gate (1.7).

### 2.2 (b) Schuss auf am Boden liegenden, lebenden Zombie

Fall 1 — **niedergeschossener Zombie** (Knockdown-Liege, `+0x1dc==0x80`, `+0x93` seit [2] frei):
- Treffer NUR mit Nach-unten-Zielen (DOWN-Band, 1.1) und < 5000.
- FUN_80011f50: `+0x5=3`, `+0x6=0` (DAT_8006f410[1]), hp−=5, `+0x4=2`.
- FUN_80105a8c liegend, `3 ∉ {0x12,0x13}` -> **FUN_801068a0**: Blut am Bein (pool+0x4F4), SE 6,
  Clip 0x1e fuer genau 1 Tick, und wegen `+0x1dc==0x80` SOFORT `+0x4=1/+0x5=0x11/+0x6=4`
  (@0x80106970-a0) -> **der Zombie steht unmittelbar auf** (Get-up-Clip 0x12).
- Waehrend des Aufstehens `+0x93|=1` (Knockdown [4]) -> weitere Schuesse gehen DURCH
  (Rekursion, kein Schaden) bis [6] (`0x201` + `+0x93&=0xfe` + Poise (rand&3)+4).
- Toedlicher Treffer im Liegen: State 3 -> **FUN_80107cb0** (Clip 0x1f, FX 0x2500, Gore@f35).
- Nur mit Waffen 0x12/0x13 (bzw. Attack-Code 0x12 aus FUN_80012d60): **FUN_80106a38**
  (Prone-Flinch 37/38, KEIN Auto-Aufstehen).

Fall 2 — **Scripted-Lyer** (Typ 0x16, grid 0x88, ROOM1140-Ambusher, passiv Phase 0):
- `+0x93|=1` wird JEDEN Tick re-armiert (@0x80103aac-ab8) -> der Schuss traegt NIE Schaden ein:
  FUN_80011f50 setzt nur `+0x93|=2` und rekursiert; naechster Durchlauf schliesst ihn aus
  (`0x3000000`-Pick-Gate). **Keine HP-Aenderung, kein Flinch, keine Reaktion.**
- Erst wenn seine Wake-Maschine Phase 3 erreicht (`+0x9=0`, `word=0x201`), ist er ein normaler,
  treffbarer Zombie.

---

## 3. Port-Ist-Zustand

- Schuss-Anwendung: `re15_port/engine/src/re15_damage.c` `re15_player_weapon_fire`
  (:468-622) — Band-Gate :532-547, `+0x93&=1`+Crit :577-579, Latch/Rekursion :580-583,
  `+0x5=weapon` :587, hp :588, `+0x93|=1` :589, Front/Back :599-600, Crit-Kill :603-604,
  `+0x7=0` :605, `+0x4` :606, `+0x6=Elevation` :610-614. Deckungsgleich mit 1.1 (die zwei
  Y-Korrekturen fehlen, s. D4).
- HURT: `enemy_ai_common.c` `re15_enemy_ai_live_hurt` :2491-2715 — Downed-Router :2501-2517
  (gleiches `+0x5∈{0x12,0x13}`-Kriterium), Flinch-Phasen :2518-2556 (Sentinel-Get-up
  :2539-2543), Standing-Stagger :2559-2714 (Poise-Steps :2607-2609 == fe30-Dump; Exit
  0x10201+Re-Roll :2649-2690; Router-Gate :2708-2714).
- Knockdown: `re15_enemy_ai_live_knockdown` :1427-1470.
- Grab: `re15_enemy_ai_live_grab` :1066-1284 — Throw-off [4] :1218-1230, Recovery [6]
  :1248-1262 (**:1256 `e->anim_flags |= 0x80`**), [7] :1263-1273, Exit [8] :1274-1282
  (**:1275 einziger Clear von 0x80**).
- Grid-Nibble-Dispatch: :2388-2402 — **Nibble 7/8 = leerer `break`** (:2391-2400), der Lyer ist
  im Port passiv-ohne-Guard; die Wake-Maschine FUN_80103a58 ist unportiert (HONEST-OPEN im
  Audit `RE15_ENEMY_AI_AUDIT.md`).
- Anim-Vorschub: `player_common.c` `re15_actors_anim_advance` :680-712 — Liege-Pin
  (0x0C/0x0E/0x12/0x13) :704-705, **unbegrenztes `anim_frame++`** :709 (der Wrap-Fix ceba1ba1
  wurde in 991e566e revertiert, weil er die falsche Bank nahm; die Bank-Regel existiert
  inzwischen: `re15_actor_uses_loco_bank`/`re15_actor_clip_len` enemy_ai_common.c:2757-2786).
- Reverse-Konsument: `anim_select_common.c` :42/:142 (`a->anim_flags & 0x80`).

### Dynamische Messung (Scratchpad-Sonde gegen libre15_engine, 2026-08-02)

**(A2) Schuss in Recovery [7]** (`fl=0080` wie nach Case 6):
```
recovery  f0  st=1 ss1=3 ss2=7 mo=17 fl=0080
SCHUSS    f0  st=2 ss1=3 ...   mo=17 fl=0080
  nach    f1  st=2 ... mo=2 (Stagger) fl=0080     <- Stagger posiert RUECKWAERTS
  nach    f6  st=1 ss1=7 (TURN)       fl=0080
  nach    f26 st=1 ss1=19 (Approach)  fl=0084     <- Walk loopt RUECKWAERTS
anim_flags&0x80 nach 30 Ticks: 1   (LEAK)
knockdown f37 st=1 ss1=17 ...         fl=0084     <- FALL-Clip 0xb spielt RUECKWAERTS
```
**(B) Knockdown-Liege, Schuss mit AIM DOWN**: `SCHUSS -> mo=30(0x1e) 1 Tick -> ss1=0x11 ss2=4
-> mo=18(0x12 Get-up) -> [6] 0x201, stun=(rand&3)+4` — deckungsgleich mit 2.2 Fall 1. ✓
**(B2) LEVEL-Schuss auf Liegenden**: `hit=0` — Schuss geht drueber, wie Original. ✓
**(C) Scripted-Lyer grid=0x88**: `hit_react=00` (kein Guard) -> `weapon_fire=TREFFER,
hp 300->295, Flinch mo=30`, Exit `st=1 ss1=0 grid=88` -> **dauerhaft passiv** (Nibble-8-break),
`mo=30` waechst hold-last ins Unendliche.

---

## 4. Divergenzen

### D1 (HIGH — Nutzer-Symptom a): Reverse-Playback-Flag leckt beim Schuss in die Grab-Recovery
- **Original:** Rueckwaertsspielen des Recovery-Clips 0x11 ist das a2=1-ARGUMENT genau eines
  anim_set-Calls in Grab-[7]: `80102aec ori a2,zero,0x1; 80102af8 jal 0x8001f314`. Es existiert
  KEIN persistenter Zustand; ein Schuss (1.1 erzwingt `+0x4=2/3`) beendet das Rueckwaertsspielen
  implizit.
- **Port:** `enemy_ai_common.c:1256` setzt `anim_flags |= 0x80` (persistent), einziger Clear ist
  Grab-Exit [8] `:1275`. Wird der Zombie in [6]/[7] angeschossen (`re15_damage.c:606` hijackt den
  State), bleibt 0x80 fuer immer gesetzt: Stagger, TURN, Approach-Walk, Fall-Clip 0xb, Death —
  alles posiert rueckwaerts (`anim_select_common.c:42/:142`; Sonde A2: `fl=0080/0084` durch
  Stagger→Turn→Approach→Knockdown, „Un-Fall“).
- **Fix:** Das 0x80-Bit ist eine Port-Erfindung; byte-true ist Reverse eine Eigenschaft des
  Aufrufs. Vorschlag: Reverse im Renderer aus dem Zustand ableiten (`state==1 && ss1∈{3,4} &&
  ss2==7`) und das persistente Bit entfernen; alternativ (minimal) das Port-Bit an JEDEM
  unfreiwilligen Grab-Exit loeschen = bei HURT-/DEATH-EINTRITT (Phase 0), kommentiert als
  Port-Buchhaltung (das Original hat keinen Store, weil es kein Flag hat).

### D2 (HIGH — Nutzer-Symptom b, Scripted-Lyer): passiver Lyer ist schiessbar und danach dauerhaft tot-passiv
- **Original:** FUN_80103a58 Phase 0 re-armiert `+0x93|=1` JEDEN Tick (@0x80103aac-ab8) ->
  FUN_80011f50-Guard (@0x80012404-18) -> Schuss traegt KEINEN Schaden ein, keine Reaktion; erst
  die Wake-Maschine (Phase 3 @`FUN_80103a58.c` Z.32-37: `+0x9=0`, `word=0x201`) macht ihn zum
  normalen Zombie.
- **Port:** Nibble-7/8-Zweig ist ein leerer `break` (`enemy_ai_common.c:2391-2400`): kein
  Tick-Guard, keine Wake-Maschine. Sonde C: Schuss landet (hp 300→295), Flinch 0x1e spielt, Exit
  `state1/sub0` -> wegen des leeren Zweigs **fuer immer regungslos**, eingefroren auf dem letzten
  Flinch-Frame (mo=30 hold-last) statt der Liegepose.
- **Fix:** (i) Minimal: im Nibble-7/8-Zweig Phase-0-Verhalten portieren: solange `+0x6==0`
  pro Tick `hit_react|=1` (@0x80103aac-ab8) — macht den Lyer byte-true unverwundbar; (ii) die
  Wake-Maschine FUN_80103a58 Phasen 1-3 + Decide-Rows @0x8011f9d4 (Get-up-Paar 0x80104b38/40)
  portieren (bereits HONEST-OPEN im Enemy-AI-Audit).

### D3 (LOW): Knockdown-SE-Rolls fehlen
- **Original:** FUN_8010512c [0]: `(rand&3)==0 -> SE(rand&1?5:8)` (`FUN_8010512c.c` Z.21-29);
  [4]: `(rand&7)==0 -> SE(rand&1?5:8)` (Z.60-68).
- **Port:** `re15_enemy_ai_live_knockdown` [0] :1431-1439 und [4] :1454-1459 — keine SE-Rolls
  (auch keine RNG-Draws).
- **Fix:** beide Rolls mit `re15_engine_rand8()` + `re15_audio_room_se` nachziehen (@-Zitate wie
  oben).

### D4 (LOW, in ROOM1140 no-op): Y-Korrekturen des Hoehen-Codes +0x6 fehlen
- **Original:** @0x8001245c-24ac: `playerY<enemyY && +0x6==0 -> +0x6=1` und
  `enemyY<playerY && +0x6==2 -> +0x6=1`.
- **Port:** `re15_damage.c:610-614` mappt nur die Aim-Elevation. Auf ebenem Boden (playerY==
  enemyY, strikte slt) feuern beide Korrekturen nie -> ROOM1140 identisch.
- **Fix:** beide Vergleiche ergaenzen (wirksam nur bei Ebenen-Differenz).

### D5 (MED-LOW): `word0`-Bit 0x1000 (Grab-Choreo-Latch) unmodelliert
- **Original:** gesetzt in Grab-[0] (`FUN_80102548.c` Z.21-22, Zombie+Spieler), geloescht in [8]
  (Z.115). Konsument: Windup-Death-Gate `FUN_80101224.c` Z.12
  (`(+0x1d8&0x100) && !(word0&0x1000)`), d.h. der `+0x1da`-Timeout-Tod (`word=0x1503`,
  Death-Row 0x15 = FUN_80108d74) ist waehrend (und nach einem abgebrochenen) Grab blockiert.
- **Port:** `enemy_ai_common.c:2407-2417` zaehlt `ai_attack_timer` ohne dieses Gate; das Bit
  existiert nicht.
- **Fix:** Latch als Actor-Bit einfuehren (Set [0]/Clear [8]) und das Windup-Gate darauf
  konditionieren — inklusive des Original-Leaks (KEIN Clear beim Schuss-Hijack; das Original
  laesst das Bit bis zum naechsten [8] stehen).

### D6 (LOW): `+0x1d8`-Bit0 (aktiver Grab) im Port nicht gesetzt/geloescht
- **Original:** Set in [0] (`FUN_80102548.c` Z.28), Clear in [4] (Z.75); Konsument u.a. das
  Domino-Shove-Gate (Z.87 `(+0x1d8&1)==0`).
- **Port:** Grab-Case 0 (:1103-1134) setzt es nicht; das Shove-Gate :1244 liest `ai_flags&1`,
  das nie jemand setzt -> Bystander sind immer shovebar.
- **Fix:** `ai_flags|=1` in [0], `&=~1` in [4].

### D7 (MED): `+0x95` unbegrenzt statt Original-Wrap — Stagger mit uebernommenem Frame friert
- **Original:** anim_set haelt `+0x95` IMMER in `[0, fc-1]` (@0x8001F610-3C, Wrap auf 0
  @0x8001F63C; Zitat in Commit ceba1ba1). Der Stagger uebernimmt den Frame des vorherigen Clips
  byte-true OHNE Reset (kein `+0x95`-Store in FUN_80105b7c) — dank Wrap laeuft der Walk-Zyklus
  einfach weiter.
- **Port:** `player_common.c:709` `anim_frame++` unbegrenzt; Play-once-Render HOLD-LASTs bei
  `af>=fc`. Ein Schuss z.B. in Grab-[5] (af bis 0x18) laesst den Stagger-Walk-Clip (Loco-Bank,
  teils kuerzer) auf dem letzten Frame stehen — der Zombie „friert“ fuer die Stagger-Dauer in der
  Pose (Rest-Symptom des in ceba1ba1/991e566e dokumentierten, damals mit falscher Bank
  gefixten und darum revertierten Problems).
- **Fix:** den Wrap aus ceba1ba1 wiederherstellen, aber die Framezahl aus
  `re15_actor_clip_len()` (Bank-Regel `enemy_ai_common.c:2757-2786`) beziehen statt aus der
  Default-Bank — exakt die im Revert 991e566e notierte Bedingung.

### Verifiziert-DECKUNGSGLEICH (keine Divergenz, fuer die Einordnung der Symptome wichtig)
- Downed-Router-Kriterium `+0x5∈{0x12,0x13}` = WEAPON-Klasse (Port :2512 identisch).
- Sentinel-Get-up `+0x1dc==0x80` (@0x80106970-a0) — Port :2539-2543; Sonde B: sofortiges
  Aufstehen. Ein von ANFANG an liegender Zombie (Poise-Seed 4..7 statt 0x80) steht auch im
  Original NICHT sofort auf.
- Liegender Zombie nur mit DOWN-Aim treffbar (Band-Strip/-Add @FUN_80101224 + FUN_80012974
  @0x800129cc-f0) — Port :532-547; Sonde B2: LEVEL-Schuss = kein Treffer. Das ist KEIN Bug.
- Get-up ist unverwundbar (`+0x93|=1` in [4], Clear erst [6]) — Port :1458/:1463; Schuesse
  waehrend des Aufstehens wirken auch im Original nicht.
- Poise-Steps @0x8011fe30 (0/−2/−3/0/−1-Klassen) — Port :2607-2609 wortgleich.

## 5. Offene Fragen (mit naechstem konkreten RE-Schritt)

1. **Welche Waffen-IDs sind 0x12/0x13** (Prone-B-Klasse; Zombie-Damage 400 bzw. 20 @0x8006e650)?
   Naechster Schritt: Item-Katalog gegen `DAT_800aca5d`-Werte laufen lassen — die Item-Klassen-
   Splitstelle @0x80074030 und den Equip-Commit @0x80046688 fuer IDs 18/19 dekodieren; parallel
   Savestate mit anderem Loadout ziehen und aca5d ablesen.
2. **Wake-Ausloeser des Scripted-Lyers** (FUN_80103a58 Phase 0 -> 1, wer setzt `+0x6=1` und
   `+0x9c`): Xrefs auf die Decide-Rows 0x801039f4/0x801039fc disassemblieren und das ROOM1140-SCD
   auf Member_SET(+0x6)-Opcodes fuer den Lyer-Slot scannen; alternativ DuckStation-Savestate
   vor/nach dem Aufwachen diffen (`re15-room-capture`).
3. **Spieler-Grab-Flag-Clear bei Grab-Abbruch vor [6]:** Die Struggle-FSM-Region @0x80036774/
   0x800367a4 (RMW auf DAT_800acae7) phasengenau dekodieren, um zu belegen, WANN der Spieler-
   Guard bei einem in [4]/[5] erschossenen Zombie faellt. (Port-Ergebnis via Victim-Release
   :813-814 ist funktional gleich; nur die Beleg-Luecke schliessen.)
4. **DEATH-Spalte 4 (FUN_80107634 in jeder Zeile):** welcher Producer erreicht State 3 mit
   `+0x6==4`? Kandidat: Get-up-Reroute schreibt `+0x6=4` — aber dort ist `+0x93|=1`. Xref-Suche
   auf `+0x6=4`-Stores in STAGE1.BIN, dann FUN_80107634 dekodieren.
5. **FUN_80106290 / FUN_80106624 / FUN_80106048** (schwere Stagger-Handler, Waffen 5/6, 7/8,
   0x13): noch nicht tief dekompiliert — fuer Magnum-/Shotgun-Fernschuesse auf Zombies relevant
   (nur Nicht-Crit-Faelle). Naechster Schritt: Decompile-Files lesen + gegen Port-Stagger pruefen.

## 6. Sonden-Artefakt

Die Analyse-Sonde (Repo-fremd, Scratchpad) baut gegen `re15_port/build/engine/libre15_engine.a` +
`libre15_test_support.a` (`--start-group`) und reproduziert A2/B/B2/C deterministisch; Quelltext
liegt in der Session-Scratchpad (`probe_hit_scenarios.c`). Im Repo existiert zusaetzlich
`re15_port/tests/unit/probe_zombie_hit.c` (ohne CMake-Eintrag seit Revert 991e566e).
