# Liegende Zombies (Deskriptor 0x88/0x87) reagieren im RE2-Flavor auf den Spieler — RE-Dossier

Runde 16, 2026-09-19. Nutzer: "Die Zombies, die im Original nur am Boden liegen, und die man
nicht anschiessen koennen sollte und nicht auf uns reagieren sollten, reagieren auf uns —
befund_1140_F2567_marke1 — ROOM 1140, aber auch z.B. ROOM 10E0." Der Nutzer spielt mit KI-Flavor
RE2 (Default).

Sonde: `re15_port/tests/unit/probe_r16_liegende_zombies.c` (CMake: `tests/unit/probes/r16_liegende-zombies.cmake`,
Build-Verzeichnis `re15_port/build_r16_liegende-zombies`). Aufruf `probe_r16_liegende_zombies 1140|10E0 both`.

## 0. Kurzfassung

1. **Defekt (reproduziert):** Der 0x88-Liegende (ROOM1140 slot 1 Typ 0x16, ROOM10E0 slot 1 Typ 0x10) ist im RE2-Flavor mit Nach-unten-Zielen TREFFBAR (Pistole, Abstand 2600: 7 bzw. 5 Treffer) und steht beim ersten Treffer in EINEM Frame auf (Pose-Hoehe -373 -> -2766), kaempft danach bzw. stirbt. Naehe allein weckt ihn NICHT (Sweep bis Abstand 849, auch mit wachen Fressern: 0 Abweichungen) — die "Reaktion" ist der Schuss.
2. **RE1.5-Original:** Nibble 7/8 hat KEINEN Selbst-Wecker (Decide-Zeile @0x8011f9d8[0] = `jr ra` @0x801039f4) und ist in Phase 0 UNSCHIESSBAR: `lbu v0,147(a1); ori v0,v0,0x1; sb v0,147(a1)` @0x80103aac-ab8 jeden Tick -> Resolver-Latch @0x80012404-18 ueberspringt ihn. Nur ein Skript-`Member_set(12,0x89/0x8A)` (`sb a2,9(a0)` @0x800411f8) weckt ihn; ROOM1140/10E0/1110 haben keinen (Zensus).
3. **RE2-Original:** Schlaefer sind DREIFACH gesperrt — +0x1D3|=0x80 (EXEC[7] P0 @0x80103804-14), +0x10E&0x4000 (INIT @0x80100A34-38) und HP=-1 (INIT @0x80100A3C-40); die Trefferschleife FUN_800470C0 filtert alle drei (@0x80047138-40 / @0x80047148-50 / @0x80047158-64). Beide Originale sind sich einig: der Liegende ist nicht treffbar.
4. **Port-Ist:** `re15_re2z_hit_filter_apply` (enemy_ai_re2_zombie.c:8188-8218) nimmt JEDEN EXEC[7]/EXEC[8]-Spawn von den Gates (2)/(4) aus ("Spawn-Pose vom Filter ausgenommen", eingefuehrt fuer die 0x86-Fresser) — damit auch den 0x88-Liegenden, dessen RE1.5-Zwilling gerade NICHT treffbar ist. Der Treffer reisst ihn ueber den RE2-HURT in Stand/Kampf.
5. **Fix:** Die Ausnahme auf die Fresser beschraenken; fuer Deskriptor-Nibble 7/8 (+Bit 0x80) in EXEC[7] gelten die Original-Gates (2)/(4) unveraendert -> `hit_react |= 1` jeden Tick (= RE1.5 @0x80103aac-ab8 und RE2 @0x80047138-64). Naehe-Wecker und Skript-Wecker (D15.2, Nibble 9/10) bleiben unveraendert; Pin als ctest aus der Sonde.

## 1. Reproduktion/Messung

Sonde faehrt den ECHTEN Weg (`re15_game_step` + Pad, RDT + raum-eigenes sub00, ECHTE Baenke) in beiden
Flavors. Log-Auszuege woertlich (`probe_r16_liegende_zombies 1140 both`, stderr weggefiltert):

### 1.1 Spawns (Roh-Deskriptoren vor dem ersten KI-Tick)

```
ROOM1140: slot01 typ=0x16 +0x9=0x88 (nibble 8, bit80=1) pos=(-800,0,-20600) mo=19
          slot02..05 typ=0x10/0x10/0x11/0x11 +0x9=0x86 (Fresser)
ROOM10E0: slot01 typ=0x10 +0x9=0x88 (nibble 8, bit80=1) pos=(-1500,0,4900) mo=22
          slot02..05 typ=0x10 +0x9=0x06 (Fresser OHNE Bit 0x80)
```
RDT-Bytes: ROOM1140.RDT @0x00baa `44 00 16 88 …`, ROOM10E0.RDT @0x00d42 und @0x00daa `44 00 10 88 …`
(beide sub00-Zweige). Spielweiter Zensus der 0x80-gegateten Liege-Deskriptoren in STAGE1 (Sce_em_set
`44 slot typ beh` mit plausiblen Koordinaten, Datei-Offsets < 0x4000):

| Raum | Liege-Spawn | Skript-Wecker `34 0C 89/8A` |
|---|---|---|
| 1020/1021, 1070/1071, 1100/1101, 1200/1201 | 0x88 | 4 / 5 / 5 / 3 |
| 11F0/11F1 | 0x87 | 4 |
| **1140** | 0x88 (Typ 0x16) | **0** |
| **10E0/10E1** | 0x88 (2 Zweige) | **0** |
| **1110/1111** | 0x88 (Typ 0x11) | **0** |
| 1010/1011, 1220/1221 | 0x81 (Kriecher, anderer Executor) | – |

### 1.2 Zustand nach 60 KI-Frames (Spieler fern)

```
RE1.5: slot01 st=1/0/0/0 hp=65 grid=0x88 mo=19/120 fr=0 +93=0x01 1d3=0x00 10e=0x0000 topY=-589
RE2  : slot01 st=1/7/1/0 hp=79 grid=0x88 mo=22/10  fr=0 +93=0x00 1d3=0x80 10e=0x4002 topY=-373
```
RE1.5 traegt den Tick-Guard (+0x93 Bit 0 = 1); RE2 traegt die Original-Latches (+0x1D3=0x80,
+0x10E=0x4002), aber der Port-Filter hat `hit_react` auf 0 gesetzt = KANDIDAT.

### 1.3 Distanz-Sweep (B: nur der Liegende aktiv; B2: alle Aktoren aktiv, Fresser wachen und greifen)

```
RE1.5 B : min ai_dist=849  -> erste Abweichung vom Liege-Zustand: KEINE (st=1/0/0 grid=0x88 mo=19 topY=-589)
RE2   B : min ai_dist=849  -> KEINE (st=1/7/1 grid=0x88 mo=22 topY=-373 10e=0x4002)
RE1.5 B2: 600 Frames       -> KEINE   (Fresser: slot02 st=1/6/2, slot03 st=1/2/1, slot04/05 st=1/19/1 = wach)
RE2   B2: 600 Frames       -> KEINE   (Fresser: alle st=1/1/1 10e=0x0004 = wach, topY -2660..-2766)
```
Identisch fuer ROOM10E0. **Naehe weckt den 0x88-Liegenden in keinem Flavor** — der Nibble-7/8-Ausschluss
im Port-Wecker (enemy_ai_re2_zombie.c:2442-2443) greift.

### 1.4 Schuss (Pistole 3, Abstand 2600, 120 Frames Dauerfeuer, dann 300 Frames Beobachtung)

```
RE1.5 LEVEL: 0 Treffer (Elevations-Band: Liegender nur DOWN)      Ende st=1/0/0 grid=0x88 topY=-589 liegt_noch=1
RE1.5 DOWN : 0 Treffer (+0x93 Bit 0 = Tick-Guard)                 Ende st=1/0/0 grid=0x88 topY=-589 liegt_noch=1
RE2   LEVEL: 0 Treffer (Band)                                     Ende st=1/7/1 grid=0x88 topY=-373 liegt_noch=1
RE2   DOWN : ROOM1140 7 Treffer hp 79->2 / ROOM10E0 5 Treffer hp 79->-1   liegt_noch=0
```
RE2 DOWN, ROOM1140, woertlich:
```
vor-schuss f0    st=1/7/1/0 hp=79 grid=0x88 mo=22/10 +93=0x00 1d3=0x80 10e=0x4002 topY=-373
TREFFER    f1    st=2/3/1/0 hp=68 grid=0x88 mo= 0/65 +93=0x01 1d3=0x03 10e=0x0002 topY=-2766   <- steht in EINEM Frame
TREFFER    f12   st=2/3/1/0 hp=57 … topY=-2783
TREFFER    f23   st=2/3/1/0 hp=46 mo=4/44 topY=-2528
nach-shot  f35   st=1/5/1/0 hp=35 mo=2/60 1d3=0x81 10e=0x2002   (EXEC[5] Sturz)
nach-shot  f80   st=1/5/3/0 … topY=-373                          (liegt wieder)
danach     f192  st=1/1/0/0 grid=0x08 …                          (WALK — er ist wach)
danach     f282  st=1/3/1/0 dist=998                             (GRIFF auf den Spieler)
```
ROOM10E0 RE2 DOWN: 5 Treffer, hp -1, `st=7/7/0` = Leiche. Das ist der Nutzerbefund: der Liegende ist
treffbar, und die "Reaktion" (Aufstehen, Kampf, Tod) folgt aus dem Treffer.

## 2. Original-Mechanismus

### 2.1 RE1.5 (STAGE1.BIN, roh @0x80100000; `re15_disasm.py`)

Grid-Nibble-Dispatch @0x8011f80c (16 Eintraege, gedumpt): `[5]=[6]=0x801018f8`, `[7]=[8]=0x80101974`,
`[9]=[10]=0x801019f0`. Der Liege-Dispatcher ist ein Doppel-Dispatch:
```
80101984 lbu v0,5(v0) ; sll 2 ; addiu at,at,-1576 (0x8011f9d8) ; lw ; jalr   ; DECIDE [+0x5]
801019b8 lbu v0,5(v0) ; sll 2 ; addiu at,at,-1580 (0x8011f9d4) ; lw ; jalr   ; ANIMATE [+0x5]
```
Tabelle @0x8011f9d0: `[0x8011f9d0]=0x80103980 (Fresser-Decide, Naehe < 0xFA0)`,
`[0x8011f9d4]=0x80103a58 (ANIMATE, gemeinsam)`, `[0x8011f9d8]=0x801039f4 (Decide 7/8)`,
`[0x8011f9dc]=0x801039fc (Decide 9/10 = Sofort-Wecker)`.
```
801039f4 jr ra            ; DECIDE Nibble 7/8 = STUB: kein Naehe-, kein Zeit-Wecker
801039f8 nop
```
ANIMATE FUN_80103a58, Phase 0 (`+0x6 == 0`):
```
80103aac lbu v0,147(a1)   ; +0x93
80103ab4 ori v0,v0,0x1    ; |= 1  -> Ein-Treffer-Latch JEDEN Tick neu gesetzt
80103ab8 sb  v0,147(a1)
80103acc sb  v0,440(v1)   ; +0x1b8 = 1
```
Phase 1 `lhu +0x9c; addiu -1; bne … ; sh` (@0x80103ad0-e0, bei 0 -> +0x6=2, +0x8f=0xf @0x80103aec-b04);
Phase 2 `jal 0x8001f314` (+0x170/+0x174, a2=0, a3=0x100 @0x80103b08-18) -> `+0x6 += done`;
Phase 3 `sb zero,9(a1)` (+0x9=0) @0x80103b3c, `sw 0x201,4` @0x80103b4c, **`andi 0xfe; sb +0x93`
@0x80103b64-68 (erst HIER wieder treffbar)**, `sb zero,440` @0x80103b78.

Resolver FUN_80011f50 (PSX.EXE): der Latch wirkt so:
```
800123fc lbu v1,147(s1)
80012404 andi v0,v1,0x1
80012408 beq  v0,zero,0x80012428   ; Bit 0 gesetzt ->
8001240c ori  v0,v1,0x2            ;   +0x93 |= 2
80012418 jal  0x80011f50           ;   REKURSION = naechstes Ziel, KEIN Schaden, kein +0x4=2
```
Elevations-Band des Liegenden (FUN_80101224-Tail):
```
801015f0-fc lw v0,0(a0); lui v1,0x4000; or; sw          ; word0 |= LEVEL
80101600    jal 0x80012aa4 ; a0=0xbb8
80101614-20 lbu v0,9(a1); andi 0x80; beq zero -> 80101640
80101624-3c lui v1,0xbfff; ori 0xffff; and; sw ; jal 0x80012974 (a0=0x1388)  ; LEVEL weg, DOWN < 5000
```
=> Ein liegender Zombie ist NUR mit Nach-unten-Zielen (Abstand < 5000) ein Kandidat — und selbst dann
traegt der Schuss wegen @0x80103aac-ab8 keinen Schaden ein. Skript-Wecker: `Member_set(12, 0x89/0x8A)`
= FUN_8004116c Tabelle @0x80010c8c[12] -> `sb a2,9(a0)` @0x800411f8 hebt Nibble 7/8 auf 9/10
(Decide 0x801039fc: `+0x6=2, +0x8f=0xf, 1/4 SE 5` @0x80103a0c-44). In ROOM1140/10E0/1110 gibt es
keinen (Byte-Zensus `34 0C 89|8A`: 0 Treffer; auch `35 0C` in 10E0: 0).

### 2.2 RE2 (EMZ0.BIN = EMOVL10_S0.BIN byteidentisch, roh @0x80100000; `re2_disasm.py`; EXE info/re2leon/PSX.EXE)

INIT-Selektor = `+0x10E & 0x3f` (aus dem RE2-Sce_em_set-Record, Feld +4: Handler 0x8005714C
`lhu v0,4(v1); sh v0,270(s0)` @0x8005734c-54):
```
801009c4-f0  ==2 -> Clip 22, word 0x701, +0x21A|=4                 (liegt, steht sofort auf: kein 0x4000)
801009f4-a10 ==4 -> Clip 23, word 0x701
80100a14-60  ==5 -> Clip 22, word 0x701, +0x10E=0x4002 (@0x80100a34-38), HP=-1 (`addiu v0,zero,-1; sh v0,342` @0x80100a3c-40), +0x21A|=4, pool+0x560|=0x4000
80100a64-a4  ==7 -> Clip 23, word 0x701, +0x10E=0x4004 (@0x80100a88-8c), HP=-1 (@0x80100a90-94)
80100aa8-bc  ==6 -> word 0x101 ; 80100ac0-dc ==8 -> Clip 18, word 0x801 (Fresser)
```
EXEC[7] LYING SLEEPER 0x80103780 (Sprungtabelle @0x80100094, `sltiu v1,5` @0x801037a4):
```
P0 801037cc-818: +0x21A&4 ? Clip 22 : 23 (sw 332); jal 0x8002959c (a3=256); +0x6=1;
                 lbu 467; ori 0x80; sb  -> +0x1D3 |= 0x80 (@0x80103804-14);  +0x1C0 |= 1
P1 8010381c-834: lhu v0,270; andi 0x4000; bne -> Ende ; sonst +0x6 = 2       ; HALT solange 0x4000
P2 80103838-8d4: Clip 8 (0xF0008) bzw. 9 bei +0x21A&4, Zufallsframe; +0x6=3; SE 10/11/12, +0x239=150
P3 801038d8-8fc: jal 0x8002959c ; +0x6 += Rueckgabe                            ; Clip ausspielen
P4 80103900-92c: sw 0x101,4 ; andi +0x1D3,0x7f ; andi +0x1C0,0xfe ; word0 |= 0x0C000000
```
**Phasen 3/4 enthalten KEINE Weckbedingung** — sie sind Ausspielen + Commit. DECISION-Tabelle
@0x8010C88C[7] = 0x80103778 = `jr ra; nop` (ebenso [8] = 0x80103b6c): der Executor hat keinen
Entscheider. Der EINZIGE Wecker ist das Loeschen von +0x10E Bit 0x4000. Im Overlay gibt es genau einen
Clear (`andi v1,v1,0xbfff; sh v1,270(s0)` @0x80104f0c-10, EXEC[15] Phase-1-Ende, Commit 0x60501 —
eine Reaktions-, keine Weck-Kette). Der Weck-Weg ist das RAUMSKRIPT: RE2-EXE-Member-Schreiber
(3-Byte-Opcode @0x80055c60-a8: Member = pc[1], Wert = Work-Var[pc[2]] @0x800d47ec; Sprungtabelle
@0x80011228, 44 Faelle): **Fall 7 = `sh a2,270(a0)` @0x80055d10 = +0x10E**. RE2-Raeume wecken ihre
Schlaefer also per Member-Set auf +0x10E (0x4002 -> 0x0002), genau wie RE1.5 per Member_set(12,…).
(Der zweite EXE-Treffer `andi 0xbfff` @0x80062b0c schreibt +0x21C, nicht +0x10E.)

Wie RE2 den Schlaefer gegen Treffer haelt — NICHT ueber die Hitbox +0x78/+0x98, sondern ueber die
Kandidatenschleife des Angriffs-Aufloesers FUN_800470C0:
```
80047124-30 lw v0,0(s0); andi 1; beq zero -> skip        ; (1) inaktiv
80047138-40 lbu v0,467(s0); bne v0,zero -> skip           ; (2) +0x1D3 != 0   <- P0 setzt 0x80
80047148-50 lh  v0,342(s0); bltz -> skip                  ; (3) HP < 0        <- INIT setzt -1 (Sel 5/7)
80047158-64 lhu v0,270(s0); andi 0xc000; bne -> skip      ; (4) +0x10E&0xC000 <- INIT setzt 0x4000
```
Alle drei Sperren stehen fuer den Schlaefer gleichzeitig; keine davon haengt am Abstand.

### 2.3 Konsens beider Originale

| | RE1.5 (0x88) | RE2 (Sel 5/7) |
|---|---|---|
| Wecker | nur Skript `Member_set(12,0x89/8A)` @0x800411f8 | nur Skript Member 7 -> +0x10E @0x80055d10 |
| Naehe | nein (Decide-Stub @0x801039f4) | nein (Decide-Stub @0x80103778) |
| Treffer | nein (+0x93 Bit 0 jeden Tick @0x80103aac-ab8) | nein (Gates 2/3/4 @0x80047138-64) |
| Treffbar ab | Phase 3 `andi 0xfe` @0x80103b64-68 | P4 `andi 0x7f` @0x80103914-18 + 0x4000-Clear |

## 3. Port-Ist

- **RE1.5-Lane (korrekt):** `enemy_ai_common.c:106-140` `re15_enemy_ai_lying_phase` — Phase 0 `hit_react |= 1`
  (@0x80103aac-ab8), Phase 3 `&= ~1`; Dispatch `case 7: case 8:` :4769-4783, `case 9: case 10:` :4784-4806.
  Resolver-Latch `re15_damage.c:1718-1723` (`hit_react & 1 -> |= 2; goto retry_after_latch` = @0x8001240c/18).
  Band fuer `grid_id & 0x80`: `re15_damage.c:1458-1464` (LEVEL weg, DOWN < 0x1388). Sonde: 0 Treffer, kein Wake.
- **RE2-Lane, Spawn-Remap (korrekt):** `enemy_ai_re2_zombie.c:7660-7712` `re2z_init`: sel 8 -> `+0x10E=0x4002`,
  `0x701`, EXEC[7]. Der Port setzt (anders als RE2 @0x80100a3c-40) KEIN HP=-1 — er nutzt die RE1.5-HP-Zeile,
  damit der Zombie nach dem Skript-Wecken kaempfen kann.
- **RE2-Lane, Wecker (korrekt):** `re2z_exec_lying` :2442-2447 — der gemappte Naehe-Wecker schliesst Nibble 7/8
  aus; `re15_re2z_tick` D15.2 :7857-7935 hebt bei Nibble 9/10 das Latch. Sonde B/B2: kein Wake.
- **RE2-Lane, Trefferfilter (DEFEKT):** `re15_re2z_hit_filter_apply` :8186-8218:
  ```c
  int spawn_pose = (e->state == 1) && (e->sub_state_1 == 7 || e->sub_state_1 == 8);   /* :8188 */
  int hittable = (e->active != 0)
              && (spawn_pose || e->re2z_self1d3 == 0u)        /* (2) :8212 */
              && (e->hp >= 0)                                 /* (3) */
              && (spawn_pose || !(e->re2z_f10e & 0xC000u));   /* (4) :8214 */
  ```
  Die `spawn_pose`-Ausnahme wurde fuer den Nutzerbefund "Im Dining Room trifft man den am Boden fressenden
  Zombie nicht" eingefuehrt (Kommentar :8114-8160: Sollseite = RE1.5, deren Fresser treffbar sind). Sie
  gilt aber pauschal fuer EXEC[7] — und der EXEC[7]-Insasse mit Deskriptor 0x87/0x88 hat in RE1.5 den
  Tick-Guard @0x80103aac-ab8 und in RE2 die Gates (2)/(4). Der Port ist damit der EINZIGE der drei, der
  ihn treffen laesst. Folge (Sonde 1.4): Resolver schreibt +0x4=2 -> `re2z_hurt` (:6553) -> Reaktions-
  Handler mit aufrechtem Clip (topY -2766 im Treffer-Frame) -> EXEC[5]-Sturz -> Aufstehen -> WALK/GRIFF.
- Warum das im Spiel "einfach so" passiert: die Fresser in ROOM1140 liegen im RE2-Flavor ebenfalls unter
  `grid_id & 0x80` -> auch sie sind nur mit DOWN-Zielen treffbar; wer auf die Fresser schiesst, zielt nach
  unten, und der Schuss-Streifen (50..650 breit, Reichweite 1000+Radius) erfasst den Liegenden daneben.

## 4. Fix-Plan (Phase 2)

1. **`enemy_ai_re2_zombie.c` `re15_re2z_hit_filter_apply` (:8186-8218):** die Ausnahme auf Zustaende beschraenken,
   deren RE1.5-Zwilling treffbar ist. Neu:
   ```c
   unsigned nib = e->grid_id & 0x0fu;
   int passive_lyer = (e->grid_id & 0x80u) && (nib == 7u || nib == 8u);
       /* RE1.5-Zwilling: Nibble 7/8 = FUN_80103a58 Phase 0 `lbu 147; ori 1; sb` @0x80103aac-ab8
        * (unschiessbar, Decide-Stub @0x801039f4 ohne Selbst-Wecker); RE2 selbst: Gates (2)
        * @0x80047138-40 und (4) @0x80047158-64 auf +0x1D3|=0x80 (@0x80103804-14) und
        * +0x10E&0x4000 (@0x80100a34-38). Beide Originale sperren — nur die Port-Ausnahme nicht. */
   int spawn_pose = (e->state == 1) && (e->sub_state_1 == 7 || e->sub_state_1 == 8) && !passive_lyer;
   ```
   Alles Weitere unveraendert: fuer den passiven Liegenden liefern (2)/(4) `hittable=0` -> `hit_react |= 1`
   jeden Tick (dieselbe Aussage wie @0x80103aac-ab8). Nach dem Skript-Bump auf 0x89/0x8A (D15.2 :7920-7930
   loescht 0x4000 und +0x1D3&0x7f) faellt `passive_lyer` weg und die Gates oeffnen — wie RE1.5 Phase 3
   @0x80103b64-68 bzw. RE2 P4 @0x80103914-18.
   Der Kommentarblock :8114-8160 ("Spawn-Pose vom Filter ausgenommen") bekommt den Zusatz, dass die
   Ausnahme NUR fuer Deskriptoren gilt, deren RE1.5-Zwilling treffbar ist (0x86/0x06-Fresser: INIT-Decoder
   @0x80100e60/@0x80100eb0 -> +0x5=0xc, kein Nibble-Dispatch, kein Tick-Guard).
2. **Keine Aenderung** an `re2z_exec_lying` (Naehe-Ausschluss 7/8 :2442 gemessen wirksam), am D15.2-Wecker,
   am Spawn-Remap (HP bleibt RE1.5-Zeile; RE2s HP=-1 @0x80100a3c-40 ist nicht noetig, weil (2)/(4) sperren
   und der Geweckte seine HP braucht) und am Elevations-Band (`re15_damage.c:1458-1464`).
3. **Pin (ctest, in `probes/r16_liegende-zombies.cmake` mit add_test):** aus der Sonde `test_r16_liegende_unschiessbar`:
   RE2-Flavor, ROOM1140 UND ROOM10E0, Pistole DOWN-Zielen, Abstand 2600, 120 Frames Dauerfeuer ->
   **0 Treffer, hp unveraendert, st=1/7/1, topY unveraendert (-373)**; zusaetzlich 600-Frame-Sweep bis 849 ->
   keine Abweichung. Gegenproben im selben Lauf: (P) ein 0x86-Fresser (ROOM1140 slot 2) MUSS mit DOWN-Zielen
   Treffer kassieren (die Ausnahme lebt weiter); (N) nach `grid_id = 0x89` (Skript-Bump, vgl.
   `test_re2_lyer_1140.c` Teil N) MUSS der Liegende aufstehen und danach treffbar sein. RE1.5-Flavor als
   Regressionswache (0 Treffer, +0x93 Bit 0 = 1).
4. **Reihenfolge:** Fix -> Pin gruen -> `test_re2_lyer_1140`, `test_1140_straight_shot`, `probe_1140_feeder_shot`
   (Fresser-Treffbarkeit) und `test_re2_room1140_ab` unveraendert gruen -> gdigrab-Sichtpruefung ROOM1140/10E0
   (Skill re15-port-visual-verify): Liegender bleibt bei DOWN-Feuer liegen, Fresser reagieren.
5. **Risiken:** (a) ROOM11F0 (0x87, Skript-Wecker vorhanden): vor dem Wecken jetzt unschiessbar — das ist
   RE1.5-konform (Phase 0 Guard). (b) Der Zombie behaelt `hit_react|=1`, bis der Filter ihn freigibt — der
   Filter laeuft auch fuer uebersprungene Ticks (`re15_re2z_hit_filter_apply` aus game_step), also kein
   Einfrieren. (c) Melee/Messer laufen ueber denselben Latch (re15_damage.c:203 / :1721) -> ebenfalls gesperrt,
   wie im Original.

## 5. Offen / nicht belegt

1. **Nibble 5 (0x85) und 4 (0x84)** kommen in STAGE1 nicht vor (Zensus §1.1); ihr RE1.5-Verhalten
   (Nibble 5: Naehe-Wecker 0x80103980 dist<0xFA0 mit demselben Phase-0-Guard) ist im RE2-Flavor nicht
   geprueft. STAGE2/3 (Nibble 3/4, 30 Records) laufen ueber andere Overlays — nicht Teil dieses Befunds.
2. **Treffbarkeit WAEHREND des Aufstehens** nach dem Skript-Bump: RE1.5 bleibt bis Phase 3 gesperrt
   (Guard-Clear erst @0x80103b64-68); im Port oeffnet D15.2 das Latch sofort -> waehrend der Aufsteh-Clips
   8/9 treffbar. Kleine Abweichung, nicht Gegenstand des Nutzerbefunds; Entscheidung in Phase 2.
3. **RE2-Skript-Opcode-Name** des 3-Byte-Member-Schreibers @0x80055c60 (Sprungtabelle @0x80011228) ist
   nicht ueber eine Opcode-Tabelle belegt, nur ueber den Code; fuer den Port irrelevant (RE1.5-Raeume
   nutzen Member_set(12,…)).
4. **Welche Zielhaltung der Nutzer bei F2567 tatsaechlich hatte** (LEVEL oder DOWN) ist nicht messbar
   (Screenshot nicht im Repo). Gemessen: LEVEL trifft in keinem Flavor, DOWN trifft nur im RE2-Flavor;
   ein anderer Reaktionsweg (Naehe, wache Fresser, Griff-Abbruch) wurde in B/B2 ausgeschlossen.
5. ROOM10E1/1141 (Alternativ-Spieler) nicht dynamisch geprobt — identische Spawn-Records (§1.1), derselbe Pfad.

## 6. Umsetzung (Phase 2)

Siehe `liegende-und-aufstehen.md` §6 (Umsetzung, Messwerte vorher/nachher, Pins, Offenes).

## 7. Nacharbeit (Phase 3, 2026-09-19, Thema re-restposten)

### 7.1 §5 Punkt 2 — Treffbarkeit WAEHREND des Aufstehens

Beide Originale sperren den Gebumpten bis zum Ende der Pose; nur der Port gab ihn im Bump-Bild frei.

* **RE1.5:** der Skript-Wecker FUN_801039FC setzt `+0x6 = 2` (@0x80103A1C) und fasst +0x93 nicht
  an; der Riegel faellt erst in Phase 3 der Liege-Phasenmaschine (`andi 0xfe / sb v0,147(a1)`
  @0x80103B64-68).
* **RE2:** EXEC[7] loescht +0x1D3 Bit 7 erst in P4 (`lbu v0,467(s0) / andi 0x7f / sb v0,467(s0)`
  @0x80103904-18, im selben Block wie `sw 0x101,4` @0x80103900-0C), EXEC[8] in P3 @0x80103CE4-FC.
* Die bisher als „gepaarter Clear" zitierte Stelle @0x80104F00-04 gehoert **nicht** hierher: das
  ist EXEC[15]s eigener Ausgang (Commit 0x60501 @0x80104EE8-EC), eine Reaktions-, keine Weckkette.

Aenderungen (`enemy_ai_re2_zombie.c`): D15.2 loescht nur noch `+0x10E & 0x4000` (@0x80104F0C), und
`passive_lyer` im Trefferfilter umfasst jetzt die Nibbles **7..10** statt nur 7/8 — 0x89/0x8A ist
derselbe Koerper, nur mit gesetztem Wecker, und laeuft im Original durch dieselbe Phasenmaschine
(Dispatcher @0x8011F80C[9]/[10] = 0x801019F0 laedt die ANIMATE-Zeile aus derselben Basis
0x8011F9D4 wie [7]/[8]). Der Pose-Ausgang raeumt beide Bytes selbst, danach faellt die Ausnahme
ueber `grid_id = 0` ohnehin weg.

**Messung** (`probe_p3_restposten` Abschnitt [C], echter Game-Step, Spieler 12000 entfernt):

| | vorher | nachher |
|---|---|---|
| 0x88 liegend | gesperrt (+0x93 Bit 0 = 1) | unveraendert gesperrt |
| Bump auf 0x89, Bild 0 | **sofort treffbar** (+0x1D3 = 0x00) | gesperrt, +0x1D3 = 0x80 |
| Aufsteh-Clip 9 (Bilder 1..75) | treffbar | gesperrt |
| Pose-Ausgang | – | **Bild 76**: st 1/1, grid 0x00, +0x1D3 = 0x00, +0x93 = 0x00 = treffbar |

Der Fresser (EXEC[8]) bleibt frueher dran als der Liegende — sein Clear steht im Original schon in
P3 (@0x80103CE4-FC), nicht erst im Exit.

### 7.2 §5 Punkt 1 — Nibble 5 (0x85) und Nibble 4 (0x84)

Die Frage war „ungeprueftes Verhalten im RE2-Flavor". Der Befund ist staerker: **0x84 und 0x85
gehoeren gar nicht zur Liege-, sondern zur FRESSER-Familie**, und zwar in JEDEM Stage-Overlay.

Der Grid-Dispatcher @0x8011F80C[4] (= 0x8010187C) laedt seine DECIDE-Zeile aus 0x8011F9C8 und
seine ANIMATE-Zeile aus 0x8011F9CC; [5]/[6] (= 0x801018F8) laden aus 0x8011F9D0 / 0x8011F9D4.
Beide Paare zeigen auf dieselben zwei Funktionen — eigener Dump der `lui/addiu`-Paare beider
Dispatcher und der Zielworte:

```
STAGE1  0x8011F9C8/CC == 0x8011F9D0/D4  ->  0x80103980 (Naehe-Wecker)  /  0x80103A58 (Phasen)
STAGE2  0x80117ACC/D0 == 0x80117AD4/D8  ->  0x80103814 / 0x801038EC
STAGE3  0x8011DB18/1C == 0x8011DB20/24  ->  0x80103A6C / 0x80103B44
STAGE4  0x80118EF0/F4 == 0x80118EF8/FC  ->  0x80103934 / 0x80103A0C
STAGE5  0x8011EA58/5C == 0x8011EA60/64  ->  0x80103AB4 / 0x80103B8C
```

Der Wecker 0x80103980 ist derselbe, den der Port fuer 5/6 bereits fuehrt
(`lw v0,464(v1) / sltiu v0,v0,0xfa0 / beq` @0x80103990-9C, nur bei +0x6 == 0 @0x801039A4-AC, dann
`sb v0,6` = Phase 1 und `+0x9C = rand&0xF` @0x801039B8-CC).

Port-Ist davor und Aenderung:

| | RE1.5-Lane (`enemy_ai_common.c`) | RE2-Lane (`enemy_ai_re2_zombie.c`) |
|---|---|---|
| vorher | Nibble 4 fiel in den deferrten `default`-Zweig → **gar kein Tick**; 5/6 → `live_feeding` | sel 4/5 in `lying_family` → EXEC[7] (Liegender); sel 6 → EXEC[8] (Fresser) |
| nachher | `case 4:` faellt in denselben Zweig wie `case 5: case 6:` | sel 4/5/6 → EXEC[8] |

**Messung** (Abschnitt [D], synthetischer Spawn auf demselben Slot, gleicher Seed):

```
0x84 -> st=1 sub=8/0 f10e=4004 clip=18
0x85 -> st=1 sub=8/0 f10e=4004 clip=18
0x86 -> st=1 sub=8/0 f10e=4004 clip=18
```

Reichweite: eigener Byte-Zensus der ausgelieferten RDTs — **0x84 = 28 Records** (ROOM2000/2001,
ROOM3000/3001, ROOM3010/3011), **0x85 = 2 Records** (ROOM3010/3011). In STAGE1 kommt keiner von
beiden vor; deshalb war die Zuordnung bis jetzt ungeprueft. Eine Sichtpruefung im Spiel steht aus
(die betroffenen Raeume liegen in STAGE2/3).
