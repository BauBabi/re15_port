# ROOM1210 — Zombie-Haende durch das Gitter (RE2-Vorbild nachruesten)

**Stand:** 2026-08-27 · **Typ:** NACHRUESTUNG nach RE2-Vorbild (ausdrueckliche Nutzer-Entscheidung),
kein byte-true RE1.5-Abgleich. Trotzdem ist JEDE Konstante unten mit `@0x…`-Adresse oder
Datei-Byte-Offset belegt; wo ich etwas NICHT gefunden habe, steht das ausdruecklich als
**NICHT GEFUNDEN** samt der Wege, die ich probiert habe.

---

## 1. Symptom (Nutzer-Wortlaut)

> „Im ROOM1210 strecken ab einem gewissen Punkt alle Zombies die Haende aus dem Gitter.
> Das ist sau schlecht gemacht, weil es noch eine Beta ist. Es ist aber klar, was die Entwickler
> vor hatten. In Resident Evil 2 gibt es einen Flur, wo man durchlaeuft, und kommt man am Fenster
> vorbei, kommen die Haende der Zombies mit Stoehnen und greifen einen. Das wuerde ich gerne auch
> hier so umgesetzt haben wollen."

**„ab einem gewissen Punkt" ist woertlich richtig und ich habe den Punkt gefunden.** Es ist eine
einzige AOT-Zone in der Mitte des Flurs; wer sie betritt, schaltet mit EINEM Skript-Ereignis
**alle zehn** Kreaturen gleichzeitig aktiv. Genau das ist das „sau schlecht gemacht".

---

## 2. Der IST-Zustand von RE1.5 ROOM1210 — die Daten

Datei: `re15_port/shared_assets/PSX/STAGE1/ROOM1210.RDT`, Groesse 140596 B (`0x22534`).

### 2.1 RDT-Kopf + Adresstabelle (selbst gedumpt)

```
0x00  00 09 01 00 00 00 00 4d      nSprite=0 nCut=9 nOmodel=1 nItem=0 nDoor=0 nRoom_at=0
0x08  snd0_edt  = 0x00002B7C (128 B)      0x14  snd1_edt = 0x0000381C (96 B)
0x0C  snd0_vh   = 0x00002BFC (3104 B)     0x18  snd1_vh  = 0x0000387C (3104 B)
0x10  snd0_vb   = 0x0000449C (14624 B)    0x1C  snd1_vb  = 0x00007DBC (61624 B)
0x20  collision = 0x00001B68              0x24  camera   = 0x00000060
0x28  zone      = 0x00000188              0x2C  light    = 0x0000036C
0x40  main_scd  = 0x00001CC4 (476 B)      0x44  sub_scd  = 0x00001EA0 (136 B)
0x48  pScdOld   = 0x00001F28  (laut BioRdt `rdt.h:36` „ALWAYS Evt_end ONLY" — bestaetigt)
```
(Layout = `Bio2RoomProto` in
`info/Resident_Evil_und_Playstation_Information/re1.5-specific/secondary_source/BioRdt-master/rdt.h:14-42`.)

### 2.2 main_scd sub00 — die zehn Kreaturen (Datei-Byte-Offsets)

Opcode-Walker ueber die byte-true Laengentabelle `re15_port/engine/src/scd_vm.c:166-218`,
`Sce_em_set`-Feldlayout `scd_vm.c:3195-3215`.

```
f 0x001CC6 .. 0x001D66   6 x Door_aot_set (0x3B)   -> Tueren
f 0x001D86  44 00 1a 00 00 00 00 8a  58 9e 00 00 71 dd 00 00 00 00 00 00
f 0x001D9A  44 01 1a 00 00 00 01 8b  58 9e 00 00 f9 d7 00 00 00 00 00 00
f 0x001DAE  44 02 1a 00 00 00 01 e4  50 c9 00 00 f7 e8 00 00 00 08 00 00
f 0x001DC2  44 03 1a 00 00 00 01 e5  50 c9 00 00 7f e3 00 00 00 08 00 00
f 0x001DD6  44 04 1a 00 00 00 01 e6  58 9e 00 00 7d c2 00 00 00 00 00 00
f 0x001DEA  44 05 1a 00 00 00 01 e7  58 9e 00 00 16 bd 00 00 00 00 00 00
f 0x001DFE  44 06 1a 00 00 00 01 e8  50 c9 00 00 03 ce 00 00 00 08 00 00
f 0x001E12  44 07 1a 00 00 00 01 e9  50 c9 00 00 8b c8 00 00 00 08 00 00
f 0x001E26  44 08 1a 00 00 00 01 ea  50 c9 00 00 5a ad 00 00 00 08 00 00
f 0x001E3A  44 09 1a 00 00 00 01 eb  50 c9 00 00 a3 a8 00 00 00 08 00 00
f 0x001E4E  Obj_model_set (0x2D)
f 0x001E70 / 0x001E86  2 x Item_aot_set (0x50)
f 0x001E9C  Evt_end
```

Dekodiert (`slot=pc[1]`, `type=pc[2]`, **`behavior/grid=pc[3]`**, `floor=pc[4]`,
`kill_flag=pc[7]`, `x/y/z = LE s16 @pc[8]/[10]/[12]`, `dirY = LE s16 @pc[16]`):

| Datei-Offset | slot | type | **grid pc[3]** | kill pc[7] | x | z | dirY |
|---|---|---|---|---|---|---|---|
| 0x001D86 | 0 | 0x1A | **0x00** | 0x8A | −25000 |  −8847 | 0 |
| 0x001D9A | 1 | 0x1A | **0x00** | 0x8B | −25000 | −10247 | 0 |
| 0x001DAE | 2 | 0x1A | **0x00** | 0xE4 | −14000 |  −5897 | 2048 |
| 0x001DC2 | 3 | 0x1A | **0x00** | 0xE5 | −14000 |  −7297 | 2048 |
| 0x001DD6 | 4 | 0x1A | **0x00** | 0xE6 | −25000 | −15747 | 0 |
| 0x001DEA | 5 | 0x1A | **0x00** | 0xE7 | −25000 | −17130 | 0 |
| 0x001DFE | 6 | 0x1A | **0x00** | 0xE8 | −14000 | −12797 | 2048 |
| 0x001E12 | 7 | 0x1A | **0x00** | 0xE9 | −14000 | −14197 | 2048 |
| 0x001E26 | 8 | 0x1A | **0x00** | 0xEA | −14000 | −21158 | 2048 |
| 0x001E3A | 9 | 0x1A | **0x00** | 0xEB | −14000 | −22365 | 2048 |

**Zwei Reihen** an den beiden Flurwaenden: `x = −25000` (yaw 0) und `x = −14000` (yaw 2048 = 180°) —
sie schauen sich ueber den Flur hinweg an. Das ist die Zellen-/Gitter-Reihe.
Der Nachbarflur ROOM1220 hat exakt dasselbe Layout mit ECHTEN Zombies
(`analysis/nutzer_batch_2026-08-27/room1220-kriecher.md`, x −25000…−13300).

Alle zehn spawnen mit **`grid = 0x00`** — das ist der Angelpunkt (§2.4).

### 2.3 sub_scd — der AUSLOESER („ab einem gewissen Punkt")

sub-Tabelle @`0x001EA0`: 3 Eintraege → sub00 @`0x001EA6`, sub01 @`0x001EC6`, sub02 @`0x001EC8`.

```
--- sub00  (laeuft beim Raum-Start)
f 0x001EA6  06 00 1a 00        If_ck        (Blocklaenge 0x1A)
f 0x001EAA  21 03 2c 00        Ck(bank 3, flag 0x2C) == 0
f 0x001EAE  2c 06 03 41 00 00 ac a9 68 c5 50 14 a4 06 ff 00 18 02 00 00
                               Aot_set  aot=6  sce=0x03(EVENT)  flags=0x41
                               Rect  x=-22100  z=-15000  w=5200  d=1700
                               Payload  p0=0x00FF  p1=0x0218  p2=0x0000
f 0x001EC2  08 00              Endif
f 0x001EC4  01 00              Evt_end

--- sub01
f 0x001EC6  01 00              Evt_end          (leer)

--- sub02  (das Ereignis)
f 0x001EC8  22 03 2c 01        Set(bank 3, flag 0x2C = 1)      -> Einmal-Latch
f 0x001ECC  46 06 00 …         Aot_reset(aot 6, sce 0)         -> Zone abschalten
f 0x001ED6  2e 02 00           Work_set(kind 2, idx 0)   \
f 0x001EDA  34 0c 01 00        Member_set(id 12, 1)       |  10x, idx 0..9
   …                                                      /
f 0x001F22  34 0c 01 00        Member_set(id 12, 1)      (letzte, idx 9)
f 0x001F26  01 00              Evt_end
```

* `sce = 0x03` = EVENT-Handler `@0x800430F0`; `sub = p1>>8 = 0x02` → **sub02**
  (Payload-Semantik belegt in `re15_port/engine/src/aot_common.c:206`: „sce 3 EVENT @0x800430F0:
  FUN_8003ee3c(cond u16@+0, sub u8@+3)").
* `flags = 0x41` → Bit `0x10` ist CLEAR → **AUTO**-Zone (kein Aktionsknopf), siehe
  `aot_common.c:242-246` (`(flags & 0x10) ? GENERIC : AUTO_EVENT`).
* `Member_set` id **12** = Entity **`+0x9`** = das grid/behavior-Byte
  (byte-true Tabelle `re15_port/engine/src/actor_common.c:88-105`, Setter `FUN_8004116c`
  `@0x8004116c`, `case 12: a->grid_id`). Wert LE `01 00` = **1**
  (`op_member_set` liest LE, `scd_vm.c:2906`, Beleg `LAB_800410b8`).
* `Work_set(2, N)` bindet Aktor-Slot `N+1` (`scd_vm.c:2751-2760`).

**Damit ist der Ausloeser belegt:** Betritt der Spieler das Rechteck
`x ∈ [−22100, −16900]`, `z ∈ [−15000, −13300]` (Flur-Mitte, zwischen den Kreaturen-Paaren bei
z≈−12797/−14197 und z≈−15747), setzt sub02 bei **allen zehn gleichzeitig `+0x9 = 1`**
und schaltet die Zone permanent ab (Flag 3/0x2C).

### 2.4 Was `+0x9 = 1` in der Kreatur ausloest (STAGE1-Overlay, selbst disassembliert)

Typ `0x1A` = **EM01A**, in den Port-Quellen „WRITHE-HAZARD"
(`re15_port/engine/src/enemy_ai_common.c:10395`).
Zustandstabelle `@0x8012093C` (`re15_disasm.py read 0x8012093c 8 --w 4 --bin STAGE1.BIN`):

```
0x8012093c: 3c c3 10 80 | 88 c4 10 80 | f8 d0 10 80 | 74 d4 10 80
            [0]INIT 0x8010C33C  [1]ACTIVE 0x8010C488  [2]HURT 0x8010D0F8  [3]DEATH 0x8010D474
0x8012094c: 68 d7 10 80 | 00 00 00 00 | 00 00 00 00 | 70 d7 10 80
            [4]0x8010D768        [5]0    [6]0        [7]CORPSE 0x8010D770 (= jr ra)
```

**ACTIVE `@0x8010C488` liest das grid-Byte und springt:**
```
8010c4d4: lbu v0,9(v0)          ; entity +0x9  = grid
8010c4dc: andi v0,v0,0xf
8010c4e0: sll  v0,v0,2
8010c4e8: addiu at,at,2396      ; Tabelle 0x8012095C
8010c4f0: lw   v0,0(at)
8010c4f8: jalr v0
```
Tabelle `@0x8012095C` (roh): `10 c5 10 80 | 8c c5 10 80 | 8c c5 10 80 | …`
→ `[0]=0x8010C510`, `[1]=0x8010C58C`, `[2]=0x8010C58C`.
`0x8010C510` und `0x8010C58C` sind **byte-identische Duplikate** derselben Dual-Dispatch-Funktion
(`A[+0x5]` aus `@0x80120968`, dann `B[+0x5]` aus `@0x80120984`) — verglichen `dis 0x8010c510 32`
vs `dis 0x8010c58c 35`. Das grid-Byte aendert also NICHT den Dispatch.

**Die Umschaltung passiert in `A[0]` `@0x8010C608`:**
```
8010c608: lui a0,0x800b
8010c60c: lw  a0,-14460(a0)     ; aktuelle Entity
8010c614: lbu v0,9(a0)          ; grid
8010c618: ori v1,zero,0x1
8010c61c: andi v0,v0,0x1f
8010c620: bne v0,v1,0x8010c644  ; grid&0x1f != 1 ?
8010c624: ori v0,zero,0x1
8010c628: sb  v0,5(a0)          ; +0x5 (sub_state_1) = 1     <=== HIER
8010c638: sb  zero,6(v0)        ; +0x6 = 0
8010c648: lbu v0,9(a0)
8010c64c: ori v1,zero,0x2
8010c650: andi v0,v0,0x1f
8010c654: bne v0,v1,0x8010c670  ; grid&0x1f != 2 ?
8010c658: ori v0,zero,0x2
8010c65c: sb  v0,5(a0)          ; +0x5 = 2
8010c66c: sb  zero,6(v0)        ; +0x6 = 0
8010c670: jr ra
```

**A/B-Tabellen** (roh `bytes 0x80120968 96`):
```
80120968: 08 c6 10 80  0c c7 10 80  30 c9 10 80  2c cb 10 80
80120978: 4c ce 10 80  d8 cf 10 80  4c ce 10 80    | A[0..6]
80120984: 78 c6 10 80  14 c7 10 80  38 c9 10 80  34 cb 10 80
80120994: 54 ce 10 80  e0 cf 10 80  54 ce 10 80    | B[0..6]
```
A[0]=0x8010C608 · A[1]=0x8010C70C (`jr ra`) · A[2]=0x8010C930 (`jr ra`) · A[3]=0x8010CB2C ·
A[4]=0x8010CE4C · A[5]=0x8010CFD8 · A[6]=0x8010CE4C
B[0]=0x8010C678 · B[1]=0x8010C714 · B[2]=0x8010C938 · B[3]=0x8010CB34 · B[4]=0x8010CE54 ·
B[5]=0x8010CFE0 · B[6]=0x8010CE54

**B[0] (`+0x5 = 0`, grid 0) `@0x8010C678` = die Dauer-Ruhe:**
```
8010c6a8: ori v0,zero,0x1 ; sb v0,6(a0)   ; +0x6 = 1  (einmalig)
8010c6bc: sb zero,148(v0)                 ; +0x94 = CLIP 0
8010c6cc: sb zero,149(v0)                 ; +0x95 = Frame 0
8010c6dc: ori v0,zero,0x7 ; sb v0,143(v1) ; +0x8f = 7 (Blend)
8010c6f4: jal 0x8001f314                  ; anim_set — sonst NICHTS
```
→ Mit grid 0 laeuft Clip 0 fuer immer in Schleife. Keine Bewegung, kein Clipwechsel.

**B[1] (`+0x5 = 1`) `@0x8010C714` = die AUSFAHR-Choreographie** (Phasenzaehler `+0x6`):
```
Phase 0  @0x8010c768:  +0x6=1 ; +0x94=0 (Clip 0) ; +0x95=0 ; +0x8f=7
         @0x8010c7a8:  +0x9c = 3          (Timer)
         @0x8010c7b8:  +0x8c = 0x320=800  (Schrittweite fuer pos_advance)
Phase 1  @0x8010c7bc:  +0x9c-- ; solange !=0 -> anim_set + pos_advance(a0=0)
         @0x8010c7e8:  bei 0 -> +0x6=2
         @0x8010c7f8:  +0x8c = 0x14 = 20
         @0x8010c808:  +0x9c = 0x1e = 30
         @0x8010c818:  +0x94 = 2  (CLIP 2) ; @0x8010c828 +0x8f = 7
Phase 2  @0x8010c830:  +0x9c-- ; anim_set + **pos_advance(a0=0x800)** (@0x8010c8b4 / @0x8010c918)
         @0x8010c850:  bei 0 -> +0x6=3 ; @0x8010c860 +0x8c=0xc8=200 ; @0x8010c870 +0x9c=3
         @0x8010c880:  +0x94 = 0 (Clip 0) ; @0x8010c890 +0x8f=7
Phase 3  @0x8010c8b8:  +0x9c-- ; bei 0:
         @0x8010c8cc:  rng() ; @0x8010c8d4 &1 ; @0x8010c8e0 +2 -> **+0x5 = 2 oder 3** (@0x8010c8e4)
         @0x8010c8f4:  +0x6 = 0
```

**B[2] (`+0x5 = 2`) `@0x8010C938` = die eigentliche Zuck-/Greif-Schleife:**
```
@0x8010c98c  +0x95 = 0
@0x8010c990  rng() ; @0x8010c9a4 &3 +1 -> **+0x9f = 1..4** (Wiederholungen)
@0x8010c9ac  rng()             -> +0x9e (Dauer)
@0x8010c9b4  rng()&0x7f ; @0x8010c9c0 +50 -> **+0x8c = 50..177** (Schrittweite)
@0x8010c9d4  +0x6 = 2
@0x8010c9e4  +0x94 = 0 (Clip 0) ; @0x8010c9f4 +0x8f = 7
@0x8010c9fc  rng()&1 -> @0x8010ca14 **+0x94 = 1 (CLIP 1)**, @0x8010ca24 +0x95 = 0
Phase 2 @0x8010ca28: +0x9e-- ; anim_set @0x8010ca6c ; **pos_advance(0x800)** @0x8010ca78 / @0x8010cb14
         bei 0 -> @0x8010ca54 +0x6 = 3
Phase 3 @0x8010ca7c: +0x9f-- ; bei 0 -> @0x8010ca9c +0x6 = 1 (neu wuerfeln)
         und +0x1d0-- (@0x8010cab4); bei 0 -> @0x8010cacc **+0x5 = 3**
```
`+0x1d0` wird im INIT geseedet: `(rng & 0x1f) + 30` `@0x8010C3FC` = 30..61 Runden.

**Die Clips existieren und sind gezaehlt** — EM01A = Blob 6 in `EMD/CDEMD0.EMS`
(`re15_port/engine/src/re15_ems.c:63-66`, `s_ems_order[6] = 0x1A`), Datei-Offset **0x13F800**,
Laenge 77504. EMD-Directory `dirOff=0x12E9C`, `count=9`; Bank 0 EDD = `dir[1] = 0x0C`:

```
Clip 0 : 30 Frames   (Keyframes 0x00..0x1D)   Ruhe / Ausgangspose
Clip 1 : 39 Frames   (0x1E..0x31, je 2 Frames pro Keyframe)
Clip 2 : 30 Frames   (0x32..0x4F)             Ausfahren (B[1] Phase 1->2)
Clip 3 : 54 Frames   (0x50..)                 TOD (+0x94=3 @0x8010D520)
```
`dir[3] = 0x1038` (Bank 1) und `dir[5] = 0x1040` (Bank 2) zeigen auf **leere** EDDs (Clip-Zahl 0) —
EM01A hat KEINE Entity-Bank und KEINE Opfer-Bank, also **keinen Greif-Clip fuer den Spieler**.

### 2.5 Zwei harte Fakten, die die Nachruestung bestimmen

**(a) EM01A ist STUMM.** Voller `jal`-Scan ueber die ganze Familie `0x8010C1EC .. 0x8010D800`
(selbst gescannt) liefert genau diese Ziele:
```
0x80019700 (Gore-FX)      0x8001af20 (rng)          0x8001af5c (Schatten)   0x8001bd60
0x8001f314 (anim_set)     0x800245d8 (pos_advance)  0x8002aec4 / 0x8002b498 / 0x8002b544 (Body-Push)
0x8003b0a4 (Wand-Clamp)   0x80065f60 (sqrt)
```
**Kein einziges `jal 0x800453d0`** (= die Raum-SE-Routine). Das Stoehnen muss also NEU dazukommen —
an dieser Kreatur existiert es im Original nicht.

**(b) EM01A GREIFT NICHT und macht keinen Schaden.** Derselbe Scan findet weder
`jal 0x80012d60` (Spieler-Damage-Entry) noch `jal 0x80037edc`; ein Store-Scan ueber
`0x8010C1EC..0x8010D800` findet **keinen** Schreibzugriff auf `DAT_800aca58/59`
(= das Spieler-Kommando-Paar, das Zombie/Hund/Alligator fuer Knockdown und Griff benutzen,
Fundstellen in `enemy_ai_common.c:10513-10516`: Zombie `@0x8010F30C`, Spinne `@0x8011638C`).
Die Kreatur ist ausschliesslich ein **Body-Push-Hindernis** (Root `@0x8010C2E8-324`, Kette
b498/aec4/b544/b0a4) mit 300er Radius
(`+0x78 = @0x8012091C = {0,−1440,0,300,1440,300}`, gesetzt `@0x8010C3C4`).

---

## 3. Was der PORT heute tut

`re15_port/engine/src/enemy_ai_common.c:11735-11741` — Typ-Gate:
```c
else if (t == 0x1a) {   /* WRITHE-HAZARD (type 0x1a, EM01A) — anchored, KILLABLE, solid 300-radius … */
    re15_writher_ai_tick(s);
    re15_enemy_body_push_tail(s, e);
}
```

`re15_port/engine/src/enemy_ai_common.c:10456-10461` — `re15_writher_ai_tick`, `case 1`:
```c
case 1:   /* ACTIVE 0x8010c488 -> A[0]/B[0]: grid-0 rooted idle. X/Z (+0x34/+0x3c) NEVER advanced;
           * clip 0 looped by anim_set. No clip cycling, no emerge — those were invented (#2/#3). */
    e->motion = 0;                                            /* B[0] +0x94=0 @0x8010c6bc */
    e->anim_frame++;                                          /* clip-0 anim advances, looped @0x8010c6f4 */
    break;
```

**Der Port liest `grid_id` an dieser Stelle GAR NICHT.** Der Kopfkommentar
`enemy_ai_common.c:10428-10436` begruendet das ausdruecklich damit, dass „both ROOM1210/1211 =
10 `Sce_em_set(0x1a)` records, grid byte 0x00" haben und die Choreographie darum
„UNREACHABLE / OPEN (latent)" sei:

> „OPEN (latent — UNREACHABLE with the shipped grid-0x00 + HP-0 spawns; both ROOM1210/1211 = 10
> Sce_em_set(0x1a) records, grid byte 0x00): the 7-sub dual-brain writhe/submerge choreography …"

⛔ **Das ist der Port-Fehler.** Der SPAWN-grid ist 0x00 — aber `sub02` schreibt zur Laufzeit
`Member_set(12, 1)` = `+0x9 = 1` in alle zehn (§2.3). Die Choreographie ist NICHT unreachable,
sie ist per AOT-Ereignis scharf. Der Port setzt `grid_id` per `op_member_set` korrekt
(`scd_vm.c:2912` → `actor_common.c:161` `case 12: a->grid_id`), aber
`re15_writher_ai_tick` case 1 wertet es nie aus → im Port bleibt Clip 0 **immer** stehen.

---

## 4. Die Divergenz (praezise)

| | Original RE1.5 | Port heute |
|---|---|---|
| Spawn | 10 × Typ 0x1A, grid 0, Clip 0 | identisch ✅ |
| Ausloeser | AOT 6, sce 3 AUTO, Rect x −22100…−16900 / z −15000…−13300, einmalig (Flag 3/0x2C) | AOT-Kette vorhanden, `grid_id` wird auf 1 gesetzt ✅ |
| Reaktion der Kreatur | `A[0] @0x8010C608`: `grid&0x1f==1 → +0x5 = 1` → B[1]-Ausfahren (Clip 0→Clip 2→Clip 0) → per `rng&1` Sub 2 oder 3; Sub 2 = Zuck-Schleife mit rng-Clip 0/1 und `pos_advance(0x800)`, Schrittweite 50…177 | **FEHLT** — `case 1` erzwingt `motion = 0` (`enemy_ai_common.c:10458`) |
| Stoehnen | existiert im Original NICHT (kein `jal 0x800453d0` in der ganzen Familie) | existiert nicht |
| Griff/Schaden | existiert im Original NICHT (kein `aca58`-Store, keine Opfer-Bank im EMD) | existiert nicht |

**Fazit:** Der Nutzer beschreibt korrekt, was das ORIGINAL tut (alle zehn auf einen Schlag).
Der Port zeigt heute noch weniger als das Original — die Kreaturen bleiben permanent in Clip 0.
Die Nachruestung muss also (1) die vorhandene Original-Maschine wieder erreichbar machen und
(2) den RE2-artigen Ueberbau (pro Fenster, Naeherungs-Trigger, Stoehnen, Griff) daraufsetzen.

---

## 5. Das RE2-Vorbild — was ich gefunden habe und was NICHT

### 5.1 Werkzeug, das ich dafuer gebaut habe (reproduzierbar)

RE2-Retail hat eine **eigene** SCD-Maschine; die RE1.5-Laengentabelle passt nicht. Ich habe die
RE2-Tabelle nicht geraten, sondern **aus dem Binary abgeleitet**:

* RE2-SCD-Dispatch-Tabelle = **`@0x800A74C8`, 143 Eintraege** (`0x00..0x8E`), gefunden als
  einziger zusammenhaengender Zeiger-Lauf dieser Groesse in `info/re2leon/PSX.EXE`;
  Eintrag[0] = `0x800537E4`.
* Jeder Handler rueckt den PC selbst vor, Muster identisch zu RE1.5:
  ```
  800537e4: lw    v0,28(a0)      ; instance +0x1C = pc
  800537ec: addiu v0,v0,1
  800537f0: sw    v0,28(a0)
  ```
  Ich habe fuer alle 143 Handler die `addiu`-Distanz per Datenfluss extrahiert. Stichproben:
  `0x2C aot_set = 20`, `0x2D obj_model_set = 38`, `0x3B door_aot_set = 32`,
  **`0x44 sce_em_set = 22`** (`@0x800576E4 addiu v1,v1,22`), `0x4E item_aot_set = 22`,
  `0x67 aot_set_4p = 28`, `0x68 door_aot_set_4p = 40`, `0x69 item_aot_set_4p = 30`, `0x8D = 24`.
* RE2-RDT-Kopf = `Bio2Room` (BioRdt `rdt.h:44-77`): **main_scd @0x48, sub_scd @0x4C**,
  Kopf 0x64 Bytes (8 + 23 × u32) — im Gegensatz zu RE1.5 (`Bio2RoomProto`, main @0x40 / sub @0x44).
* RE2-`Sce_em_set`-Feldlayout, selbst aus `FUN_8005714C` gelesen:
  `pc[2]`=slot (`@0x80057204 lb v0,2(v0)`), **`pc[3]`=Typ** (`@0x80057334-3C lbu 3 → sb 8(s0)`),
  `pc[4..5]`=u16 Spawn-Flags → Entity `+0x10E` (`@0x8005734C-54`), `pc[6]`=floor
  (`@0x80057388-90`, plus `+0x1C2 = −(pc[6]·1800)` `@0x800573A8-C0`), `pc[7]`=em-Id
  (`@0x80057274-80` → `+0x1FA`), `pc[9]`=Kill-Flag (`@0x80057358-60` → `+0x1CE`, `0xFF` = Gate aus
  `@0x800571C4-C8`), `pc[10/12/14]`=X/Y/Z (`@0x800572C8-0x8005730C`), `pc[16]`=dirY
  (`@0x80057328-30`).

Damit habe ich **alle 495 RE2-Leon-RDTs** (`info/re2leon/PL0/RDT/`) gewalkt: **931
`Sce_em_set`-Records**. Der Typ-Raum deckt sich mit RE1.5 (0x21 = Kraehe: ROOM1090 mit 8+ Records
mit variierendem Y = Flug; 0x22 = Licker: ROOM10A0), die Ableitung ist also kalibriert.

### 5.2 Belegtes Negativ-Ergebnis: RE2 hat KEINE verankerte „Greif-Hand"-KI

* RE2-Zombie-Zustandstabelle `@0x8010C830` (`info/re2leon/COMMON/BIN/EMZ0.BIN`, roh @0x80100000):
  `[0] INIT 0x8010065C  [1] ACTIVE 0x8010114C  [2] HURT 0x80104F40  [3] DEATH 0x80108250
   [7] CORPSE 0x8010A440  [8] 0x80109CFC` (zitiert in `enemy_ai_re2_zombie.c:7-10`).
* ACTIVE verzweigt **einmal** ueber `(+0x10E & 0x3F)` in `@0x8010C854`; diese 14 Eintraege
  alternieren nur auf Bit 0 → genau **zwei** Varianten: `0x8010118C` aufrecht /
  `0x80101210` kriechend (`enemy_ai_re2_zombie.c:15-17`). Es gibt keinen dritten,
  ortsgebundenen Zweig.
* Die Spawn-Pose-Varianten im INIT sind ebenfalls keine Fenster-Pose. Selbst disassembliert
  `@0x801009C4-0x80100AA8` (`re2_disasm.py --bin EMZ0.BIN`):
  ```
  801009cc andi v1,v1,0x3f ; ==2 -> @0x801009dc Clip 22 (0x16), Zustandswort 0x701
  801009fc andi v0,v0,0x3f ; ==4 -> @0x80100a08 Clip 23 (0x17), 0x701
  80100a1c andi v0,v0,0x3f ; ==5 -> @0x80100a28 Clip 22, 0x701, @0x80100a34 +0x10E = 0x4002
  80100a6c andi v0,v0,0x3f ; ==7 -> Clip 23 …
  ```
  = liegend / kollabiert / fressend, keine „durch das Fenster greifen"-Pose.
* `info/re2leon/COMMON/BIN/` enthaelt kein Hand-/Arm-Modell: nur `EM_TYPE20.EMD/.TIM`, `EMZ0*`,
  `EMS25/26`, `EMOVL10_S0/S1`, `EMOVL21_S0/S1`, `EMD0G_MOD0`.

**Schlussfolgerung, belegt:** RE2-Retail hat **keinen** ortsgebundenen Greifer-Gegner und keine
Zombie-Sub-Maschine „Arm durch das Gitter". Was RE2 an solchen Stellen hat, sind normale Zombies,
die per Skript-Ereignis in einem Korridor auftauchen. Muster nachgewiesen, z.B.
`ROOM1120 sub25 @Datei 0x039DC` (mein RE2-Walker):
```
+0x10B4 (f 0x039DC) aot_set_4p    67 0a 05 41 0000 1ac6 1294 aea5 26b2 42e9 3ee0 36d2 1294 ff00 1820 0000
+0x10D0 (f 0x039F8) obj_model_set 2d 0d …      \
+0x10F6 (f 0x03A1E) obj_model_set 2d 0e …       > drei Objekte (Fenster/Bretter)
+0x111C (f 0x03A44) obj_model_set 2d 0f …      /
+0x1142 (f 0x03A6A) sce_em_set    44 00 00 1f 0a40 00 00 03 ff 007d 0000 007d 0000 0000 0000
+0x1158 (f 0x03A80) sce_em_set    44 00 01 1f 0a40 00 00 00 ff 007d 0000 1879 0000 0000 0000
+0x116E (f 0x03A96) sce_em_set    44 00 02 1f 0a40 …
+0x1184 (f 0x03AAC) sce_em_set    44 00 03 1f 0a40 …
   -> 4 x Typ 0x1F, Spawn-Flags 0x400A, geparkt auf (32000, 0, 32000/31000/30000/29000)
+0x119A .. +0x11E1  4 x { Work_set(3,n) ; Member_copy(0x10,1) ; Calc ; Member_set2(1,0x10) }
```

### 5.3 ⛔ NICHT GEFUNDEN

**Die konkrete RE2-Flurszene „Haende kommen mit Stoehnen aus dem Fenster und greifen dich"
konnte ich nicht als byte-true Maschine belegen.** Ich habe nicht geraten und schreibe kein
Ergebnis hin, das ich nicht zeigen kann.

Wege, die ich gegangen bin:
1. RE2-Overlay-Zombie (`EMZ0.BIN`) — ACTIVE-Dispatch und INIT-Spawn-Posen vollstaendig geprueft
   (§5.2). Ergebnis: kein ortsgebundener Reach/Grab-Zweig.
2. RE2-Gegner-Roster ueber alle 495 RDTs (eigener, aus dem Binary abgeleiteter Walker).
   Ergebnis: kein Typ, der in Gruppen entlang einer Wand ohne Fortbewegung sitzt (die
   Gruppen-Treffer sind Kraehen 0x21, Licker 0x22, Zombies 0x1F, Insekten 0x37/0x2F).
3. `info/re2leon/COMMON/BIN/` auf einen „Arm"-EM-Typ geprueft — existiert nicht.
4. RE2-Kandidaten-Raum `ROOM1120 sub25` disassembliert (§5.2) — es IST ein Fenster-/Objekt-Setup
   mit geparkten Zombies, aber ich kann ohne die Positionierungs-Subs NICHT belegen, dass das
   die vom Nutzer gemeinte Szene ist.

**Naechster Weg (fuer den, der es zu Ende bringt):** Die neun Kontrollfluss-Opcode-Laengen
`0x01/03/07/10/13/17/18/19/1A` habe ich aus der RE1.5-Tabelle uebernommen, weil ihre RE2-Handler
den PC absolut setzen und mein Datenfluss-Extraktor sie deshalb nicht sieht — dadurch desyncen
129 von ~3000 RE2-Sub-Skripten. Diese neun Laengen aus den RE2-Handlern
(`@0x800537FC`, `@0x80053878`, `@0x80053964`, `@0x80053E0C`, `@0x80054020`, `@0x8005415C`,
`@0x800541A8`, `@0x80054210`, `@0x80054268`) nachziehen, den Walker erneut laufen lassen und
gezielt nach Subs suchen, die `aot_set` + `pos_set`/`Member_set` auf bereits geparkte
Zombie-Slots kombinieren. Alternativ: RE2 `ROOM1120` / `ROOM1010` per DuckStation anfahren und
die Szene direkt beobachten (Skill `re15-room-capture`, RE2-Disc noetig).

---

## 6. Fix-Rezept fuer den Implementierer

Der Nutzer will das RE2-GEFUEHL, nicht die RE2-Bytes (die es fuer diese Szene nachweislich nicht
gibt). Der ehrliche Bauplan: **die vorhandene, vollstaendig RE'te RE1.5-Maschine erreichbar
machen und den Trigger von „global, einmal" auf „pro Fenster, per Naehe" umstellen.**

### Schritt 1 (reine Fehlerkorrektur, byte-true, ohne Design-Entscheidung)

**Datei:** `re15_port/engine/src/enemy_ai_common.c`, `re15_writher_ai_tick`, `case 1` ab **Zeile 10456**.

Ersetze den Rumpf durch die byte-true Dual-Dispatch-Kette:

```c
case 1: /* ACTIVE 0x8010c488 — grid-Tabelle 0x8012095c[grid&0xf]; [0]/[1]/[2] sind
         * byte-identische Duplikate derselben A/B-Dual-Dispatch (0x8010c510 == 0x8010c58c). */
    if (e->sub_state_1 == 0) {                               /* A[0] @0x8010c608 */
        uint8_t g = (uint8_t)(e->grid_id & 0x1f);            /* andi 0x1f @0x8010c61c / @0x8010c650 */
        if      (g == 1) { e->sub_state_1 = 1; e->sub_state_2 = 0; }  /* @0x8010c628 / @0x8010c638 */
        else if (g == 2) { e->sub_state_1 = 2; e->sub_state_2 = 0; }  /* @0x8010c65c / @0x8010c66c */
    }
    switch (e->sub_state_1) {
    case 0: /* B[0] @0x8010c678 — unveraendert wie heute */ break;
    case 1: /* B[1] @0x8010c714 — Ausfahren, Phasen auf sub_state_2 (+0x6) */ break;
    case 2: /* B[2] @0x8010c938 — Zuck-Schleife */ break;
    }
    break;
```

Konstanten mit Beleg (alle aus §2.4):
`+0x9c = 3` `@0x8010C7A8` · `+0x8c = 0x320 (800)` `@0x8010C7B8` · `+0x6 = 2` `@0x8010C7E8` ·
`+0x8c = 0x14 (20)` `@0x8010C7F8` · `+0x9c = 0x1E (30)` `@0x8010C808` · `+0x94 = 2` `@0x8010C818` ·
`+0x8f = 7` `@0x8010C828` · `pos_advance(0x800)` `@0x8010C8B4` und `@0x8010CA78` ·
`+0x6 = 3` `@0x8010C850` · `+0x8c = 0xC8 (200)` `@0x8010C860` · `+0x9c = 3` `@0x8010C870` ·
`rng&1 + 2 → +0x5` `@0x8010C8D4`/`@0x8010C8E0`/`@0x8010C8E4` ·
`+0x9f = (rng&3)+1` `@0x8010C9A4` · `+0x8c = (rng&0x7f)+50` `@0x8010C9B4`/`@0x8010C9C0` ·
`+0x94 = 1 (Clip 1)` `@0x8010CA14` · `+0x6 = 3` `@0x8010CA54` · `+0x6 = 1` `@0x8010CA9C` ·
`+0x5 = 3` `@0x8010CACC` · `+0x1d0 = (rng&0x1f)+30` `@0x8010C3FC`.

**Der Kopfkommentar `enemy_ai_common.c:10428-10436` muss korrigiert werden** — die Behauptung
„UNREACHABLE with the shipped grid-0x00 spawns" ist widerlegt durch `ROOM1210.RDT` sub02
@Datei `0x001EDA` (`34 0c 01 00` = `Member_set(12, 1)`), das die zehn Aktoren zur Laufzeit auf
grid 1 setzt. Dieselbe Korrektur gilt fuer ROOM1211.

### Schritt 2 (die eigentliche Nachruestung) — Trigger pro Fenster statt global

**Wo:** derselbe `t == 0x1a`-Zweig, `enemy_ai_common.c:11735`, vor `re15_writher_ai_tick(s)`.

Der Original-Trigger ist EINE Zone fuer alle zehn (`Aot_set(6, sce 3)` @Datei `0x001EAE`).
RE2-artig ist ein Naeherungs-Gate PRO Kreatur — es fuellt genau das Byte, das sub02 fuellt:

```c
/* RE2-artige Nachruestung (Nutzer-Entscheidung 2026-08-27): statt EINES globalen
 * Aot_set(6,sce3) (ROOM1210.RDT @0x001EAE) je Kreatur ein Naeherungs-Gate. Der Effekt
 * selbst bleibt byte-true die Original-Kette A[0] @0x8010c608 -> B[1] @0x8010c714. */
if (e->grid_id == 0 && re15_enemy_player_dist(e, pl) < RE15_WRITHER_WINDOW_DIST)
    e->grid_id = 1;                       /* == sub02 Member_set(12,1), Datei 0x001EDA */
```

⛔ `RE15_WRITHER_WINDOW_DIST` ist **kein** belegter Original-Wert (das Original hat keine Distanz,
sondern ein Rechteck). Belegbare Anker, aus denen der Wert abgeleitet werden kann:
* Flur-Breite: `x = −25000` vs `x = −14000` (Datei `0x001D8E` bzw. `0x001DB6`) = **11000** Einheiten.
* Z-Abstand innerhalb eines Paares: **1400** (z. B. −12797 → −14197, Datei `0x001E06` / `0x001E1A`).
* Original-Rechteck-Tiefe: **`d = 1700`** (Datei `0x001EBC`, Bytes `a4 06`).
Empfehlung: **1700** (= die Original-Rechteck-Tiefe), damit die Zahl aus einer Original-Quelle
stammt. Wer eine andere nimmt, muss sie als Design-Entscheidung kennzeichnen — **nicht** als
byte-true.

### Schritt 3 — das Stoehnen (Sound belegt, nicht erfunden)

⛔ Im Original ist EM01A stumm (§2.5a). Der Sound ist neu — aber er liegt bereits IM RAUM:

`ROOM1210.RDT` snd1-Bank (`snd1_edt @0x0000381C`, `snd1_vh @0x0000387C`, `snd1_vb @0x00007DBC`):
```
ROOM1210 snd1_vh  sha256[0:16] = 12476e7465729b12
ROOM1140 snd1_vh  sha256[0:16] = 12476e7465729b12   (identisch)
ROOM1220 snd1_vh  sha256[0:16] = 12476e7465729b12   (identisch)

ROOM1210 snd1_vb  sha256[0:16] = 1d150fca097930a8
ROOM1220 snd1_vb  sha256[0:16] = 1d150fca097930a8   (BYTE-IDENTISCH, 61624 B)
```
Die EDT-Records 3..9 sind zwischen ROOM1210 und ROOM1220/ROOM1140 byte-gleich:
```
ROOM1210 @0x00382C  [4] 00 00 5b 15      ROOM1220 @0x0037A0  [4] 00 00 5b 15
ROOM1210 @0x003830  [5] 00 00 69 15      ROOM1220 @0x0037A4  [5] 00 00 69 15
```
(EDT-Record-Zerlegung: `re15_port/engine/src/vab_common.c:243-265`, `FUN_800453d0`.)

ROOM1220 ist der Nachbarflur mit zehn ECHTEN Zombies (Typ 0x16), deren KI genau diese SE spielt —
selbst disassembliert aus `STAGE1.BIN`:
```
80104acc: jal  0x8001af20        ; rng
80104ad4: andi v0,v0,0x3         ; 1-aus-4
80104ad8: bne  v0,zero,0x80104ae8
80104ae0: jal  0x800453d0        ; Raum-SE (snd1)
80104ae4: ori  a0,zero,0x5       ; SE 5 = Zombie-Stoehnen
```

→ **`re15_audio_room_se(5)`** (alternativ 4, dieselbe Bank; der Zombie waehlt an anderen Stellen
`rng&1 ? 4 : 5`, `enemy_ai_common.c:2680`). Es ist dieselbe Wellenform-Bank, die im selben
Flur-Komplex fuer Zombie-Stoehnen benutzt wird — kein erfundener Sound, kein fremder Bank-Index.
Kadenz: das Original-Muster `rng&3 == 0` (`@0x80104AD4`) beim Uebergang `+0x5: 0 → 1` verwenden.
Port-API: `re15_audio.h:122`, PC-Implementierung `platform/pc/src/audio_pc.c:818` (snd1-Bank).

### Schritt 4 — der Griff

⛔ **Ein Greif-Clip fuer EM01A EXISTIERT NICHT.** Beleg: EMD-Blob 6 (`CDEMD0.EMS` @0x13F800) hat
`dir[3] = 0x1038` und `dir[5] = 0x1040`, beide zeigen auf leere EDDs (Clip-Zahl 0) — es gibt nur
Bank 0 mit **vier** Clips (30 / 39 / 30 / 54, §2.4). Insbesondere gibt es **keine Opfer-Bank**
(Bank 2 = `dir[5]`), also keine Leon-Pose fuer einen Griff.

Optionen, sauber getrennt:
* **(A) Ohne neue Assets — Knockdown statt Griff.** Beim Ausfahren (B[1] Phase 2, Clip 2,
  `@0x8010C818`) einen Kontakt-Test im 300er Radius (`+0x78` Box `@0x8012091C` =
  `{0,−1440,0,300,1440,300}`, gesetzt `@0x8010C3C4`) laufen lassen und den Spieler-Knockdown ueber
  dasselbe Kommando-Paar ausloesen, das Zombie/Hund/Alligator benutzen: `DAT_800aca58 = 2`,
  `DAT_800aca59 = facing+2` — belegte Fundstellen: Zombie `@0x8010F30C`, Spinne `@0x8011638C`
  (zitiert in `enemy_ai_common.c:10513-10516`). Das ist der Import einer BELEGTEN
  Original-Mechanik in einen Gegner, der sie nicht hatte — als solchen kennzeichnen.
* **(B) Mit Assets — echter Griff.** Braucht eine Opfer-Bank fuer EM01A, die auf der Disc nicht
  existiert. Erst machen, wenn der Nutzer neue Animationen will.

**Empfehlung: (A).** Der Body-Push mit 300er Radius laeuft ohnehin schon
(`enemy_ai_common.c:11735-11741`), das „Angefasst-Werden" ist damit sofort spuerbar.

### Schritt 5 — Optionen-Gate

Schritt 1 ist byte-true und geht bedingungslos rein. **Schritte 2-4 sind eine gewuenschte
ABWEICHUNG** und gehoeren hinter denselben Schalter wie die anderen Nutzer-Abweichungen
(z. B. neben der `AI`-Option), damit der Auslieferungsstand reproduzierbar bleibt.

---

## 7. Wie man es verifiziert

1. **Statisch (sofort, ohne Build):**
   `python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x8010c608 20 --bin STAGE1.BIN`
   → muss `lbu v0,9(a0)` / `andi v0,v0,0x1f` / `sb v0,5(a0)` zeigen.
2. **Port-Probe (ctest, Muster `re15_port/tests/unit/test_room1140_spawn.c`):**
   ```c
   slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT"); re15_rdt_parse(...);
   re15_actor_init(); re15_aot_init(); scd_vm_init(); scd_register_current_rdt(&rdt);
   scd_thread_start(0, rdt.main_scd);  scd_thread_start(1, rdt.sub_scd[0]);
   for (int i = 0; i < 120; i++) scd_vm_tick();
   /* ERWARTET: 10 Aktoren, type 0x1A, grid_id == 0, motion == 0 */
   scd_thread_start(2, rdt.sub_scd[2]);        /* = das AOT-Ereignis von Hand ausloesen */
   for (int i = 0; i < 8; i++) scd_vm_tick();
   /* ERWARTET: grid_id == 1 bei allen 10 (das tut der Port schon heute) */
   for (int f = 0; f < 200; f++) re15_enemy_ai_run_all(0);
   /* HEUTE:    motion bleibt 0 bei allen 10   -> der Fehler
      NACH FIX: motion 0 -> 2 -> 0, dann 0/1 wechselnd; sub_state_1 in {2,3} */
   ```
   (Diese Probe habe ich NICHT gebaut/ausgefuehrt — §8 Punkt 2.)
3. **Visuell** (Pflicht bei jedem visuellen Report, Memory `reai-v2-visual-verify-gdigrab`):
   Skill `re15-port-visual-verify`, echtes Fenster per ffmpeg-gdigrab, Einstieg
   `RE15_DEBUG_JUMP="1210@<frame>"`, dann per `RE15_INPUT_SCRIPT` den Flur durchlaufen.
   `RE15_AUTOSHOT` / `RE15_SOFTWARE_RENDER` NICHT verwenden (maskieren Textur-/Pose-Fehler).
4. **Hoerprobe:** ROOM1220 betreten (gleiche `snd1_vb`, sha `1d150fca097930a8`), Zombie-Moan SE 5
   hoeren, dann in ROOM1210 gegenpruefen, dass es derselbe Klang ist.

---

## 8. Offene Punkte / NICHT GEFUNDEN

1. ⛔ **Die konkrete RE2-Flurszene ist NICHT GEFUNDEN** (§5.3). RE2 hat belegbar keinen
   verankerten Greifer-Gegner; welche RE2-Skriptszene der Nutzer genau meint, konnte ich nicht
   byte-true festnageln. Naechster Weg steht in §5.3.
2. **Kein Port-Lauf gemessen.** Aussagen zum Port-Verhalten in §3 stammen aus dem Quelltext
   (`enemy_ai_common.c:10456-10461`, `11735-11741`), nicht aus einer Messung. Die Probe in §7.2
   ist der Weg, das nachzuholen; sie braucht einen neuen ctest-Eintrag in
   `re15_port/tests/unit/CMakeLists.txt`.
3. **Welche Lesart von „ab einem gewissen Punkt"?** Ich habe den raeumlichen Ausloeser belegt
   (AOT-Rechteck, §2.3). Ob der Nutzer diesen Umschaltmoment meint oder „ab einer Stelle im Flur
   stehen ueberall Haende", ist aus dem Wortlaut nicht entscheidbar. Das Fix-Rezept deckt beides,
   weil es die Reaktion ueberhaupt erst herstellt.
4. **Clip-Rolle von Clip 1 (39 Frames) nicht visuell bestaetigt.** Belegt ist, DASS `B[2]` per
   `rng&1` zwischen Clip 0 und Clip 1 waehlt (`@0x8010CA14`) — nicht, WIE Clip 1 aussieht.
   Zum Absichern: Render-Katalog oder ROOM1210-Savestate.
5. **Sub 3 und Subs 4-6 (Absinken / Wieder-Auftauchen)** habe ich nur ueber die Tabellen-Adressen
   erfasst (A[3]=0x8010CB2C / B[3]=0x8010CB34, A[4]/B[4]=0x8010CE4C/0x8010CE54,
   A[5]/B[5]=0x8010CFD8/0x8010CFE0), nicht instruktionsweise. Fuer Schritt 1 reicht Sub 0/1/2;
   wer Sub 3+ mitportiert, muss die dortigen Konstanten selbst belegen.
6. **`RE15_WRITHER_WINDOW_DIST` ist eine Design-Zahl, kein Original-Wert** — siehe Schritt 2.
7. Die 9 RE2-Kontrollfluss-Opcode-Laengen in meinem RE2-Walker stammen aus der RE1.5-Tabelle,
   nicht aus RE2-Handlern; 129 von ~3000 RE2-Sub-Skripten desyncen dadurch. Fuer die hier
   gezogenen Schluesse irrelevant (die betroffenen Subs wurden nicht ausgewertet), aber wer den
   Walker weiterverwendet, muss das schliessen.

---

## 8. Verifikation (unabhaengig nachdisassembliert)

**Pruefer:** adversarialer Nach-Lauf 2026-08-27. Jede Adresse unten wurde SELBST mit
`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py … --bin STAGE1.BIN` bzw. per
eigenem Python-Parser auf der RDT-/EMS-Datei nachgezogen.

**Gesamturteil: TEILWEISE.** Die Kernkette (Spawn → AOT → sub02 → `+0x9=1` → A[0] → B[1]/B[2])
traegt vollstaendig und ist als Fix-Grundlage belastbar. Sechs Einzelheiten sind falsch bzw.
ungenau; eine davon (`+0x9e`) wuerde beim 1:1-Nachbau eine falsche Timer-Quelle in den Port tragen.

### 8.1 BESTAETIGT (eigene Disasm/Bytes)

**RDT-Kopf** `ROOM1210.RDT`, 140596 B = 0x22534. Eigener Dump:
```
0x00 00 09 01 00 00 00 00 4d
0x14 snd1_edt=0x0000381C  0x18 snd1_vh=0x0000387C  0x1C snd1_vb=0x00007DBC
0x40 main_scd=0x00001CC4  0x44 sub_scd=0x00001EA0  0x48 pScdOld=0x00001F28
```

**Die zehn Sce_em_set-Records** — alle zehn Datei-Offsets, Typ, grid, kill-Flag, Position
und dirY exakt wie im Dossier. Eigener Dekode (LE s16 @pc[8]/[10]/[12], dirY @pc[16]):
```
0x1d86 44 00 1a 00 00 00 00 8a 58 9e 00 00 71 dd 00 00 00 00 00 00  slot0 type1a grid00 kill8a (-25000,0,-8847)  dir0
0x1dae 44 02 1a 00 00 00 01 e4 50 c9 00 00 f7 e8 00 00 00 08 00 00  slot2 type1a grid00 kille4 (-14000,0,-5897)  dir2048
0x1e3a 44 09 1a 00 00 00 01 eb 50 c9 00 00 a3 a8 00 00 00 08 00 00  slot9 type1a grid00 killeb (-14000,0,-22365) dir2048
```
Alle 10 tragen `pc[3] = 0x00`.

**sub-Tabelle** @0x1EA0 = `06 00 26 00 28 00` → sub00 @0x1EA6, sub01 @0x1EC6, sub02 @0x1EC8.

**sub00 = der Ausloeser** (eigener Byte-Dump ab 0x1EA6):
```
06 00 1a 00                                        If_ck len 0x1A
21 03 2c 00                                        Ck(bank3, 0x2C) == 0
2c 06 03 41 00 00 ac a9 68 c5 50 14 a4 06 ff 00 18 02 00 00
08 00  01 00
```
Feld-fuer-Feld gegen `op_aot_set` (`scd_vm.c:2471-2670`) nachgerechnet:
pc[1]=0x06 slot · pc[2]=0x03 sce · pc[3]=0x41 (Bit 0x80 CLEAR → Kurzform, PC +20)
· pc[6..7]=`ac a9`=-22100 · pc[8..9]=`68 c5`=-15000 · pc[10..11]=`50 14`=5200
· pc[12..13]=`a4 06`=1700 · **pc[17]=0x02 = ev** (`scd_vm.c:2550` `uint8_t ev = t->pc[17]`).
`scd_vm.c:2628-2632`: `(flags & 0x10) ? GENERIC : AUTO_EVENT` → 0x41 & 0x10 == 0 →
**AUTO_EVENT auf sub 2**. Traegt.

**sub02** (eigener Byte-Dump 0x1EC8..0x1F2A):
```
22 03 2c 01                          Set(bank3, 0x2C = 1)
46 06 00 00 00 00 00 00 00 00        Aot_reset(6, sce 0)
2e 02 00 | 00 | 34 0c 01 00          Work_set(2,0) ; Nop ; Member_set(12, 1)
… 10x, idx 0..9 …
01 00 04 00                          Evt_end
```
`Member_set(12, LE 0x0001)` → `re15_actor_set_member(ws, 12, 1)` → `actor_common.c:161`
`case 12: a->grid_id` (Tabelle `actor_common.c:88-105`, `+0x9`, Setter `FUN_8004116c`).
`Work_set(2, N)` → `scd_vm.c:2745-2747` `slot = idx + 1`; `Sce_em_set` slot N →
`SCRIPT_SLOT_TO_ACTOR` `scd_vm.c:2708` = `N + 1`. **Beide Wege zeigen auf denselben Aktor.**

**Zustandstabelle @0x8012093C** (eigener `read 8 --w 4`):
```
0x8012093c: [0x8010C33C, 0x8010C488, 0x8010D0F8, 0x8010D474, 0x8010D768, 0, 0, 0x8010D770]
```

**ACTIVE @0x8010C488** (eigene Disasm der ganzen Funktion):
```
8010c4d4: lbu v0,9(v0)
8010c4dc: andi v0,v0,0xf
8010c4e0: sll v0,v0,2
8010c4e8: addiu at,at,2396     ; 0x8012095c
8010c4f0: lw v0,0(at)
8010c4f8: jalr v0
```
Tabelle @0x8012095c roh: `10 c5 10 80 | 8c c5 10 80 | 8c c5 10 80 | 08 c6 10 80 …`

**0x8010C510 == 0x8010C58C**: beide 32 Instruktionen, instruktionsgleich; beide `lbu +0x5` →
A-Tabelle 0x80120968, danach nochmal `lbu +0x5` → B-Tabelle 0x80120984. Duplikat-Behauptung traegt.

**A[0] @0x8010C608** — vollstaendig nachdisassembliert, exakt wie zitiert:
```
8010c614: lbu v0,9(a0)
8010c61c: andi v0,v0,0x1f
8010c620: bne v0,v1,0x8010c644      ; v1 = 1
8010c628: sb v0,5(a0)               ; +0x5 = 1
8010c638: sb zero,6(v0)             ; +0x6 = 0
8010c648: lbu v0,9(a0)
8010c650: andi v0,v0,0x1f
8010c654: bne v0,v1,0x8010c670      ; v1 = 2
8010c65c: sb v0,5(a0)               ; +0x5 = 2
8010c670: jr ra
```
A[1] @0x8010C70C = `jr ra`/`nop`, A[2] @0x8010C930 = `jr ra`/`nop` — die Umschaltung laeuft
also wirklich NUR solange `+0x5 == 0`.

**B[1] @0x8010C714** — 136 Instruktionen selbst gelesen. Phasen-Dispatch auf `lbu v1,6(a0)`:
0→0x8010c768, 1→0x8010c7bc, 2→0x8010c830, 3→0x8010c8b8, sonst Exit 0x8010c920.
Konstanten bestaetigt: `+0x94=0` @0x8010c778 · `+0x95=0` @0x8010c788 · `+0x8f=7` @0x8010c798 ·
`+0x9c=3` @0x8010c7a8 · `+0x8c=0x320` @0x8010c7b8 · `+0x6=2` @0x8010c7e8 · `+0x8c=0x14` @0x8010c7f8 ·
`+0x9c=0x1e` @0x8010c808 · `+0x94=2` @0x8010c818 (a0=2 aus @0x8010c7e4) · `+0x6=3` @0x8010c850 ·
`+0x8c=0xc8` @0x8010c860 · `+0x9c=3` @0x8010c870 · `+0x94=0` @0x8010c880 ·
`rng` @0x8010c8cc / `andi 1` @0x8010c8d4 / `addiu 2` @0x8010c8e0 / `sb v0,5(v1)` @0x8010c8e4 ·
`+0x6=0` @0x8010c8f4 · `pos_advance(0x800)` (Delay-Slot @0x8010c8b4 → `jal 0x800245d8` @0x8010c918) ·
`pos_advance(0)` (@0x8010c914 → @0x8010c918).

**B[2] @0x8010C938** — 125 Instruktionen selbst gelesen. `+0x95=0` @0x8010c98c ·
`+0x9f=(rng&3)+1` @0x8010c998/9a4/9a8 · `+0x8c=(rng&0x7f)+50` @0x8010c9b4/9c0/9c4 ·
`+0x6=2` @0x8010c9d4 · `+0x94=0` @0x8010c9e4 · `+0x8f=7` @0x8010c9f8 (Delay-Slot) ·
`rng&1` @0x8010c9fc → `+0x94=1` @0x8010ca14, `+0x95=0` @0x8010ca24 ·
Phase 2 `+0x9e--` @0x8010ca34-44, bei 0 `+0x6=3` @0x8010ca54, dann `pos_advance(0x800)`
@0x8010ca78/cb14 · Phase 3 `+0x9f--` @0x8010ca7c-8c, bei 0 `+0x6=1` @0x8010ca9c, dann
`+0x1d0--` @0x8010caac-bc, bei 0 `+0x5=3` @0x8010cacc, `+0x6=0` @0x8010cadc und
**Neu-Seed `+0x1d0 = (rng&0x1f)+30` @0x8010caf0**.

**INIT @0x8010C33C**: `+0x4=1` @0x8010c350 · `+0x1bc/+0x1be = playerX/Z` @0x8010c368/380 ·
`flags |= 0x40000000` @0x8010c39c · `+0x9c=0x14` @0x8010c3ac · `+0x1d0=(rng&0x1f)+30` @0x8010c3fc
(Delay-Slot des zweiten `jal rng`) · `+0x1d2=(rng&3)+1` @0x8010c41c.

**EM01A ist stumm und greift nie** — eigener `jal`-Scan (Python, MIPS-Opcode 3) ueber
0x8010C1EC..0x8010D800:
```
0x80019700 x3 · 0x8001af20 x15 · 0x8001af5c x1 · 0x8001bd60 x1 · 0x8001f314 x11
0x800245d8 x5 · 0x8002aec4 x1 · 0x8002b498 x1 · 0x8002b544 x1 · 0x8003b0a4 x1 · 0x80065f60 x1
```
**Kein `jal 0x800453d0`, kein `jal 0x80012d60`.**
Eigener Store-/Load-Scan (lui+Offset-Rekonstruktion) auf 0x800ac000..0x800ad000 derselben Spanne:
nur `0x800ac784` (`lw`, Pause/Freeze-Wort), `0x800aca88`/`0x800aca90` (`lhu` playerX/Z) und zwei
**Lesezugriffe** `lbu 0x800aca59` @0x8010D77C und @0x8010D7BC — die liegen aber bereits in der
NAECHSTEN Funktion (EM01A endet mit `jr ra` @0x8010D770/74; @0x8010D778 beginnt ein neuer Prolog,
der 0x800aca59 als Index in eine Tabelle @0x80120f54 nimmt). **Kein einziger Schreibzugriff auf
0x800aca58/59.**

**Familien-Grenze verifiziert:** `0x8010C1EC` ist wirklich der Root. Eigener Scan nach der einzigen
`lui/addiu`-Bildung von `0x8012093c` findet genau `@0x8010c2ac`, also im Root-Rumpf. Davor
der globale Freeze-Gate (`lw 0x800ac784`, `lui v1,0x2000`, `and`, `bne` @0x8010c200-20c) und der
Per-Entity-Gate `lbu v0,9(a0); andi v0,v0,0x20; bne` @0x8010c220-22c; danach der Push-Chain
`jal 0x8002b498` @0x8010c2e8 / `0x8002aec4` @0x8010c2f8 / `0x8002b544` @0x8010c300 /
`0x8003b0a4` @0x8010c320.

**Der Stoehn-Sound** — eigener Hash-/Byte-Vergleich:
```
ROOM1210 snd1_vb @0x7DBC (61624 B)  sha256[0:16] = 1d150fca097930a8
ROOM1220 snd1_vb @0x7D30 (61624 B)  sha256[0:16] = 1d150fca097930a8   IDENTISCH
snd1_vh (3104 B) sha256[0:16] = 12476e7465729b12 in ROOM1210 / ROOM1220 / ROOM1140
ROOM1210 @0x00382C [4] 00 00 5b 15  ==  ROOM1220 @0x0037A0 [4] 00 00 5b 15
ROOM1210 @0x003830 [5] 00 00 69 15  ==  ROOM1220 @0x0037A4 [5] 00 00 69 15
```
Records [1] und [2] weichen ab (`28 14` vs `29 14`, `30 10` vs `31 10`); [0] und [3..9] sind gleich —
die Dossier-Formulierung „Records 3..9 byte-gleich" trifft zu.

**Zombie-Moan-Kadenz @0x80104ACC** — eigene Disasm, exakt wie zitiert:
```
80104acc: jal 0x8001af20        ; rng   (Delay-Slot 80104ad0: sb v0,143(v1) = +0x8f = 7)
80104ad4: andi v0,v0,0x3
80104ad8: bne v0,zero,0x80104ae8
80104ae0: jal 0x800453d0
80104ae4: ori a0,zero,0x5
```

**EMD-Clip-Zahlen** — eigener EMS-Walk (Algorithmus aus `re15_ems.c:23-60` nachgebaut):
Blob 6 @Datei **0x13F800**, Laenge **77504**, `dirOff=0x12E9C`.
Sektions-Verzeichnis roh @0x13F800+0x12E9C:
```
08 00 00 00 | 0c 00 00 00  84 02 00 00  38 10 00 00  3c 10 00 00
              40 10 00 00  44 10 00 00  48 10 00 00  7c 2a 00 00
```
EDD Bank 0 @+0x0C, erste 16 Bytes `1e 00 10 00 27 00 88 00 1e 00 24 01 36 00 9c 01`
= vier Eintraege `{30,off 16} {39,off 136} {30,off 292} {54,off 412}` →
**Clip-Laengen [30, 39, 30, 54]**. Die Sektionen @0x1038/0x103C/0x1040/0x1044/0x1048 liegen je
4 Byte auseinander und sind komplett `00` → leer, also keine Entity-/Opfer-Bank.

**Port-Zitate** (Datei/Zeile gegen echten Inhalt geprueft):
* `enemy_ai_common.c:10395` = `/* ===== WRITHE-HAZARD (type 0x1a, EM01A) … */`
* `enemy_ai_common.c:10456-10461` = `case 1:` mit `e->motion = 0;` (10458) und
  `e->anim_frame++;` (10459) — wortgleich; `grid_id` kommt in `re15_writher_ai_tick`
  nirgends vor (eigener grep ueber die ganze Funktion). **Die Kern-Divergenz stimmt.**
* `enemy_ai_common.c:11735-11741` = `else if (t == 0x1a) { … re15_writher_ai_tick(s);
  re15_enemy_body_push_tail(s, e); }`
* `enemy_ai_common.c:2680` = `re15_audio_room_se((re15_engine_rand8() & 1) ? 4 : 5);`
* `scd_vm.c:2906/2912`, `actor_common.c:88-105` und `:161`, `aot_common.c:206`,
  `re15_ems.c:63-66` (`s_ems_order[6] = 0x1A`) — alle korrekt.

### 8.2 WIDERLEGT / KORRIGIERT (mit dem, was wirklich dort steht)

#### (W1) ⛔ `+0x9e` ist KEIN eigener rng-Wurf — es ist eine KOPIE von `+0x9f` (Delay-Slot)

Dossier §2.4, B[2]: „`@0x8010c9ac  rng()             -> +0x9e (Dauer)`".
Das ist falsch und wuerde beim 1:1-Nachbau eine erfundene Zufallsdauer (0..255) in den Port tragen.
Der Store liegt im **Verzweigungs-Verzoegerungsschlitz** des `jal` und schreibt deshalb das
**alte** `v0` — also den Wert, der eine Instruktion vorher schon nach `+0x9f` ging:

```
8010c990: jal   0x8001af20            ; rng #1
8010c998: andi  v0,v0,0x3
8010c9a4: addiu v0,v0,1               ; v0 = (rng1 & 3) + 1
8010c9a8: sb    v0,159(v1)            ; +0x9f = (rng1&3)+1
8010c9ac: jal   0x8001af20            ; rng #2
8010c9b0: sb    v0,158(v1)            ; <== DELAY-SLOT, v0 ist NOCH (rng1&3)+1  ->  +0x9e = +0x9f
8010c9b4: andi  v0,v0,0x7f            ; erst HIER wirkt rng #2
8010c9c0: addiu v0,v0,50
8010c9c4: sh    v0,140(v1)            ; +0x8c = (rng2&0x7f)+50
```
Roh-Bytes als Gegenprobe (`bytes 0x8010c9a8 16`):
```
8010c9a8: 9f 00 62 a0 | c8 6b 00 0c | 9e 00 62 a0 | 7f 00 42 30
          sb v0,0x9f(v1)  jal 0x8001af20  sb v0,0x9e(v1)  andi v0,v0,0x7f
```
**Korrekt ist:** `+0x9e = +0x9f = (rng & 3) + 1`, also **1..4**. Die innere Zuck-Dauer (Phase 2,
`+0x9e--` @0x8010ca34) ist damit **1..4 Frames**, nicht ein zufaelliges Byte. Wer die
Dossier-Fassung implementiert, baut eine Schleife, die im Mittel ~64x zu lang laeuft.

#### (W2) ⛔ Der RE2-Zombie-ACTIVE hat NICHT „14 Eintraege / genau zwei Varianten" — es sind 52 Zweige

Dossier §5.2 (und daraus §5.3 „belegtes Negativ-Ergebnis", §6 „RE2 hat keinen ortsgebundenen
Greifer"): „ACTIVE verzweigt einmal ueber `(+0x10E & 0x3F)` in `@0x8010C854`; diese **14** Eintraege
alternieren nur auf Bit 0 → genau **zwei** Varianten … Es gibt keinen dritten, ortsgebundenen Zweig."

Der Dispatch maskiert mit `0x3f`, die Tabelle ist also bis zu **64** Eintraege lang. Eigene Disasm:
```
8010114c: addiu sp,sp,-24
80101154: lhu  v0,270(a0)        ; +0x10E
8010115c: andi v0,v0,0x3f        ; <== 0..63, nicht 0..13
80101160: sll  v0,v0,2
8010116c: lw   v0,-14252(at)     ; 0x8010c854
80101174: jalr v0
```
Eigener Dump der Tabelle @0x8010C854 (EMZ0.BIN, Code-Bereich 0x80100000..0x8010CF4C):
```
[ 0..13]  8010118C 80101210 8010118C 80101210 … (alternierend — das ist der Teil, den das Dossier sah)
[14] 80101294  [15] 80101714  [16] 80101F7C  [17] 801025E4  [18] 80103170  [19] 80103180
[20] 8010394C  [21] 80103778  [22] 80103B6C  [23] 80103E40  [24] 80104174  [25] 80104394
[26] 8010460C  [27] 80104920  [28] 80104D6C  [29] 80104E4C  [30] 801013F4  [31] 80101A40
[32] 80102260  [33] 801025EC  [34] 80103178  [35] 80103188  [36] 80103954  [37] 80103780
[38] 80103B74  [39] 80103E48  [40] 8010417C  [41] 8010439C  [42] 80104748  [43] 80104928
[44] 80104D74  [45] 80104E54  [46] 80102EE4  [47] 801025E4  [48] 80103A70  [49] 80103024
[50] 801025EC  [51] 80103B48
[52] 803200BE  ...  <== erst hier keine gueltigen Code-Zeiger mehr (Daten)
```
**52 gueltige Handler**, davon 38 verschiedene Funktionen jenseits von Index 13. Die Behauptung
„genau zwei Varianten" ist damit **WIDERLEGT**.

⚠ Herkunft des Fehlers: Der Satz steht wortgleich schon als Port-Kommentar in
`re15_port/engine/src/enemy_ai_re2_zombie.c:15-17` („whose 14 entries alternate on bit0 only ->
exactly two variants"). Das Dossier hat ihn als eigene Disasm ausgegeben, statt ihn nachzuziehen —
**der Port-Kommentar ist ebenfalls falsch und gehoert korrigiert.**

Folge fuer das Dossier: §5.2/§5.3 duerfen NICHT mehr als „belegtes Negativ" gelesen werden.
Ob unter den 38 zusaetzlichen Zweigen ein orts-/fenstergebundener Reach-/Grab-Zweig steckt, ist
**offen** (siehe §8.3). Die INIT-Zitate @0x801009C4-A38 stimmen dagegen (unten), sie sagen nur nichts
ueber ACTIVE aus.

#### (W3) Die Datei-Liste `info/re2leon/COMMON/BIN/` im Dossier ist unvollstaendig

Dossier §5.2: „`info/re2leon/COMMON/BIN/` enthaelt kein Hand-/Arm-Modell: nur `EM_TYPE20.EMD/.TIM`,
`EMZ0*`, `EMS25/26`, `EMOVL10_S0/S1`, `EMOVL21_S0/S1`, `EMD0G_MOD0`."
Tatsaechlicher Verzeichnisinhalt (eigenes `ls`):
```
CONFIG.BIN DIEDEMO.BIN EMD0G_MOD0.BIN EMOVL10_S0.BIN EMOVL10_S1.BIN EMOVL21_S0.BIN
EMOVL21_S1.BIN EMS25.BIN EMS26.BIN EMZ0.BIN EMZ0_d1.BIN EM_TYPE20.EMD EM_TYPE20.TIM
ENDING.BIN MEM_CARD.BIN OPENING.BIN RESULT.BIN SELECT.BIN STAGE1..STAGE7.BIN TITLE.BIN
```
`EMZ0_d1.BIN` fehlt in der Dossier-Liste, und `STAGE1..7.BIN` sind die RE2-Stage-Overlays — die
Gegner-Modelle der Stages liegen ausserdem NICHT alle hier. **Aus diesem Verzeichnis laesst sich
kein „es existiert kein Arm-Modell" ableiten.** Der Punkt ist als Beleg zu streichen.

#### (W4) `+0x78` wird INDIREKT geladen — die Adresse im Dossier ist das Ziel, nicht die Quelle

Dossier §2.5b/§6 Schritt 4: „`+0x78 = @0x8012091C = {0,−1440,0,300,1440,300}`, gesetzt `@0x8010C3C4`".
Eigene Disasm:
```
8010c3b8: lui v0,0x8012
8010c3bc: lw  v0,2356(v0)      ; v0 = *(0x80120934)     <== ZEIGER-Ladung
8010c3c4: sw  v0,120(v1)       ; +0x78 = dieser Zeiger
```
`read 0x80120934 2 --w 4` → `[0x8012091C, 0x80120928]`. Das **Ergebnis** stimmt (+0x78 == 0x8012091C,
Inhalt `00 00 60 fa 00 00 2c 01 a0 05 2c 01`), aber wer nur die Dossier-Zeile liest, sucht bei
@0x8010C3C4 vergeblich nach einer `addiu`-Bildung von 0x8012091C. Direkt hinter der Box liegt eine
zweite @0x80120928 = `{0,-180,0,300,180,300}` — die Tabelle @0x80120934 hat also zwei Eintraege.

#### (W5) Kleinere Zitat-/Offset-Fehler (nicht sinnverzerrend, aber falsch)

| Dossier | Tatsaechlich |
|---|---|
| §6 Schritt 2: „Original-Rechteck-Tiefe: `d = 1700` (Datei **0x001EBC**, Bytes `a4 06`)" | `a4 06` liegt bei Datei **0x001EBA** (= `pc[12..13]`, Opcode beginnt @0x001EAE). 0x001EBC ist `ff 00` (Payload p0). |
| §2.4: „EMD `dirOff=0x12E9C`, **`count=9`**" | Das Zaehl-Wort @0x13F800+0x12E9C ist `08 00 00 00` = **8**. Die zitierten Offsets (0x0C, 0x1038, 0x1040) stimmen; das Dossier zaehlt die Eintraege nur 1-basiert und addiert das Zaehl-Wort mit. |
| §2.5b: „Fundstellen in `enemy_ai_common.c:10513-10516`" | Die aca58-Fundstellen stehen in `enemy_ai_common.c:10506` („the zombie @0x8010f30c and spider @0x8011638c knockdown"); 10513-10516 ist die Feld-Map des HUNDES. Die zitierten Adressen selbst sind korrekt — nachdisassembliert: `8010f304 ori v0,zero,0x2` / `8010f30c sb v0,-13736(at)` = `0x800aca58 = 2`; `80116388 ori v0,zero,0x2` / `80116390 sb v0,-13736(at)`. |
| §2.3: Byte-Zeile „`2e 02 00   Work_set`" | Im Strom steht `2e 02 NN **00**`: `Work_set` ist 3 B (`scd_vm.c:181`), danach folgt ein `00`-Nop als Ausrichtung. Die Offsets im Dossier stimmen trotzdem. |
| §5.2: „ACTIVE-Dispatch … **14** Eintraege" | siehe (W2) — 52. |

### 8.3 NICHT GEPRUEFT / weiterhin offen

1. **Kein Port-Lauf.** Das Dossier gesteht das in §8.2 selbst ein, markiert aber in der
   Divergenz-Tabelle §4 „AOT-Kette vorhanden, `grid_id` wird auf 1 gesetzt ✅". Diese Zeile ist
   **nicht gemessen** — statisch traegt sie (AUTO_EVENT-Install `scd_vm.c:2628-2632`,
   `Work_set`/`Sce_em_set`-Slot-Abbildung identisch), aber ein Haken gehoert erst dahinter, wenn
   die Probe aus §7.2 wirklich gelaufen ist.
2. **Erreicht ein ausgeliefertes RE2-`Sce_em_set` je Index ≥ 14 der Tabelle @0x8010C854?**
   Nicht geprueft. Das einzige im Dossier dekodierte Beispiel (`ROOM1120` @0x03A6A,
   Spawn-Flags `0x400A`) ergibt `0x400A & 0x3F = 10` → Index 10 = `0x8010118C` (aufrecht), liegt
   also noch im alternierenden Bereich. **Naechster Weg:** die 931 RE2-`Sce_em_set`-Records mit dem
   (nach §8.7 des Dossiers korrigierten) Walker auf `pc[4..5] & 0x3F` histogrammieren; jeder Wert
   ≥ 14 zeigt direkt auf einen der 38 unerforschten ACTIVE-Zweige.
3. **Clip-Rollen** („Clip 2 = Ausfahren", „Clip 1 = Variante") sind aus den Clip-INDIZES abgeleitet,
   nicht aus gerenderten Posen. Das Dossier fuehrt das in §8.4 korrekt als offen.
4. **Subs 3-6** (A[3]/B[3] 0x8010CB2C/34, A[4]/B[4] 0x8010CE4C/54, A[5]/B[5] 0x8010CFD8/E0) habe
   ich ebenfalls NICHT instruktionsweise gelesen — die Adressen aus der A/B-Tabelle stimmen
   (eigener `bytes 0x80120968 96`), der Inhalt ist ungeprueft.

### 8.4 Was fuer den Implementierer stehen bleibt

Die **Fehlerdiagnose des Dossiers ist richtig und belastbar**: die zehn Kreaturen spawnen mit
`grid = 0x00` (10 x `pc[3]=0x00`), `sub02` schreibt zur Laufzeit `Member_set(12,1)` → `+0x9 = 1`
(Datei 0x001EDA ff.), `A[0] @0x8010C608` schaltet daraufhin `+0x5 = 1`, und `B[1] @0x8010C714` ist
die Ausfahr-Choreographie. Der Port-Kopfkommentar `enemy_ai_common.c:10429-10437`
(„UNREACHABLE with the shipped grid-0x00 … spawns") ist damit **widerlegt** und `case 1`
(`enemy_ai_common.c:10456-10461`, `e->motion = 0;`) ist eine echte Divergenz. Schritt 1 des
Fix-Rezepts kann so gebaut werden — **mit einer Korrektur**:

```
+0x9f = (rng & 3) + 1        @0x8010C9A8   (Wiederholungen)
+0x9e = derselbe Wert        @0x8010C9B0   (Delay-Slot!)  -> Zuck-Dauer 1..4 Frames
+0x8c = (rng & 0x7f) + 50    @0x8010C9C4
```
Alles, was das Dossier auf RE2 stuetzt (§5.2 „kein ortsgebundener Greifer", §5.3 „NICHT GEFUNDEN",
und die daraus gezogene Empfehlung in §6 Schritt 4, einen Griff mangels RE2-Vorbild zu erfinden),
steht nach (W2)/(W3) **ohne Beleg** da und muss neu erhoben werden, bevor daraus eine
Design-Entscheidung wird.

**Bestaetigt bleiben die RE2-Werkzeug-Belege** (eigene Disasm auf `info/re2leon/PSX.EXE`):
```
0x800A74C8  Dispatch-Tabelle, [0]=0x800537E4, letzter gueltiger Eintrag [0x8E]=0x80057714,
            [0x8F]=0x10111010 (Daten)  -> genau 143 Eintraege        ✓
800537e4: lw v0,28(a0) / addiu v0,v0,1 / sw v0,28(a0)                ✓ PC-Advance-Muster
[0x44] = 0x8005714C                                                  ✓
800576e4: addiu v1,v1,22        ; Sce_em_set = 22 Bytes              ✓
80057204: lb  v0,2(v0)          ; pc[2] = slot                       ✓
80057334: lbu v0,3(v1) / 8005733c: sb v0,8(s0)      ; pc[3] = Typ    ✓
8005734c: lhu v0,4(v1) / 80057354: sh v0,270(s0)    ; pc[4..5]->+0x10E ✓
80057358: lbu v0,9(v1) / 80057360: sb v0,462(s0)    ; pc[9]->+0x1CE  ✓
800572c8/800572e0/800572f8: lhu 10/12/14(v1) -> +0x44/+0x46/+0x48    ✓ X/Y/Z
```
und die RE2-Zustandstabelle @0x8010C830 = `[0]0x8010065C [1]0x8010114C [2]0x80104F40
[3]0x80108250 [4]0x80065C88(EXE) [5][6]0 [7]0x8010A440 [8]0x80109CFC` ✓, die INIT-Zweige
@0x801009CC (`==2` → 22 / 0x701), @0x801009FC (`==4` → 23 / 0x701), @0x80100A1C (`==5` → 22,
`+0x10E = 0x4002` @0x80100A34-38) ✓, sowie die ROOM1120-Bytes @Datei 0x039DC / 0x03A6A / 0x03A80 /
0x03A96 / 0x03AAC (4 x Typ 0x1F, Flags 0x400A, X=32000, Z=32000/31000/30000/29000) ✓ und
495 RDTs in `info/re2leon/PL0/RDT/` ✓.
