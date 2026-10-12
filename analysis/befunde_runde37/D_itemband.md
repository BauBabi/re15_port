# Runde 37 — Spur D "itemband" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r37_itemband` (Zweig `r37/itemband`, Basis master f1e019ca).
Nutzer-Punkt (AUFTRAG.md #8, woertlich): "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die
Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden."
Einstieg: R_abnahme_3.md M1/M2/M3/H4, NACHLAUF.md #73/#74 (Runde 36).

## 0. Stand / Fortschritt
- [ ] Messung vorher (exe, ROOM3050 Band 4 -> Super Redhawk)
- [ ] RE-Belege selbst disassembliert (AOT-Scan @0x80042bac, Member_set 0x12 @0x8004116c)
- [ ] Zensus Item_aot_set floor != 0 + Member_set 0x12 in allen RDTs
- [ ] Umsetzung
- [ ] Messung nachher (falsches Band: keine Aufnahme; vorgesehener Weg: Aufnahme)
- [ ] Tests (probes/r37_itemband.cmake)

## 1. Messung vorher

### 1.1 Reproduktion an der exe (Stand master f1e019ca, Messkopie re15_pc_r37d.exe)
Befehl `bash D_belege/lauf_d.sh vorher_3050_oben D_belege/wp_3050_oben_item.txt RE15_DEBUG_JUMP=3050@250
RE15_PLAYER_POS=-3596,1523,1024,4 RE15_EXIT_AT=1500` (Wegplan der Spur-R-Abnahme 3, M3: Nordgang der OBEREN Ebene bis
(-24070,-22782), Blick 1024, AKTION x3; die Kiste wird nicht beruehrt). Laufordner Scratchpad `r37d/vorher_3050_oben/`
(Auszug in D_belege/vorher_3050_oben/):
* wegplan.txt: `F61 ... FLAG (9,92) = 0`, `F776 ... spieler=(-24026,-22770) ... obj1=(-18000,-7200,-20800) b4 | -> Befehl 10
  beendet: angekommen`, AKTION F807/F870/F933, **`F1036 ... FLAG (9,92) = 1`**, `F1037 FLAG (3,93) = 0`, Kiste unberuehrt
  `obj1=(-18000,-7200,-20800) b4`.
* climb.txt: `T1058 room=3050 pm=0 pad=8000 pos=(-24026,-7200,-22770) rot=1072 band=4 sub=0` (Square-Druck auf y -7200,
  Kollisionsband 4).
-> **REPRODUZIERT:** die Super Redhawk (Item_aot_set slot 6 @0x01EC6, floor 1, Taken-Bit (9,92)) wird von der oberen Ebene
(Band 4) ohne die Kiste aufgenommen. Dasselbe Ergebnis wie R_abnahme_3.md M3.

### 1.2 Herkunft (kein Bisect noetig: beide Stellen sind seit Juni/Juli 2026 unveraendert)
* `git show <c>:re15_port/engine/src/aot_common.c | grep "a->type == RE15_AOT_TYPE_GENERIC || a->type == RE15_AOT_TYPE_AUTO_EVENT"`
  -> dieselbe Gate-Liste OHNE ITEM in v0.8.21 9362dd7e (Z. 1311), v0.8.22 87cc8575 (Z. 1311), v0.8.23 3ec5b1f8 (Z. 1314),
  master f1e019ca (Z. 1318). Eingefuehrt mit b89ada17 (2026-07-07 "feat(#14c aot): generic band-gate for event AOTs") —
  ITEM war nie in der Liste; vorher gab es fuer Items gar kein Band-Gate.
* `case 18: a->floor = (uint8_t)value;` (actor_common.c Z. 159) in allen vier Staenden; eingefuehrt mit a62005fe
  (2026-06-28 "#11/#12 byte-true Member_set/cmp id-table"); das getrennte Kollisionsband `s_coll_band` stammt aus dem
  Engine-Import aa346af1 (2026-06-28).
-> Nutzer-Satz "Das war schon in v0.8.23 so" bestaetigt; KEINE Regression der Runden 35/36.

## 2. RE-Belege (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)

### 2.1 Der Aktions-Scan des Spielers ruft FUN_80042bac mit der Spieler-Entity
```
80031f40  lui   s0,0x800b
80031f44  addiu s0,s0,-14488        ; s0 = 0x800ac768
80031fc4  andi  v0,v0,0x80          ; DAT_800ac76c & 0x80 = Square-Flanke
80031fd0  jal   0x8002d474          ; zuerst die Kletter-Sonde
80031fd8  bne   v0,zero,0x80032024  ; Klettern gestartet -> kein AOT-Scan
80031fdc  addiu a0,s0,748           ; (Delay-Slot) a0 = 0x800ac768+0x2ec = 0x800ACA54 = Spieler-Entity
80031fe0  ori   a1,zero,0x1         ; Pool-Maske 1 = Spieler
80031fe4  jal   0x80042bac          ; AOT-Scan
80031fe8  ori   a2,zero,0x10        ; (Delay-Slot) ACTION = 0x10
```
(XREFs von FUN_80042bac in ghidra1_V2.txt: 80031fe4, 80032400, 800326d0, 80032a50, 80032c84 = die Sub-ENTRYs des
Spieler-FSM, plus 800436cc/80043724/8004378c im Treiber FUN_800436a8.)

### 2.2 FUN_80042bac prueft das Band fuer JEDEN Satz, VOR dem Typ (sce)
```
80042bb4  addu  s1,a0,zero          ; s1 = Entity (Spieler)
80042c20  addiu a0,a0,-14460        ; 0x800ac784
80042c48  addiu fp,a0,556           ; fp = 0x800ac9b0 = AOT-Zeigertabelle
80042c50  lw    s0,0(s4)            ; s0 = rec (Satzzeiger)
80042c84  lbu v0,1(s0) / 80042c8c and v0,v0,a3 / 80042c90 beq -> 0x80043018    ; Pool-Maske (rec[1])
80042c98  lbu v0,1(s0) / 80042ca0 andi 0x10 / 80042ca4 bne v0,s6,0x8004301c   ; ACTION-Bit (rec[1] & 0x10)
80042cac  lbu   v0,2(s0)            ; rec[2] = floor
80042cb4  andi  v0,v0,0x80
80042cb8  bne   v0,zero,0x80042cd4  ; Bit 0x80 = Band egal
80042cc0  lbu   v1,130(s1)          ; Entity+0x82 = Spielerband (0x800ACAD6)
80042cc4  lbu   v0,2(s0)
80042ccc  bne   v1,v0,0x8004301c    ; Band != floor -> NAECHSTER Satz
80042cd4  ...                       ; erst danach Geometrie (Mitte @0x80042ea8, 620 voraus @0x80042ef8)
80042f48  lbu   v0,0(s0)            ; rec[0] = sce, erst HIER gelesen
80042f74  lbu v0,0(s0) / sll 2 / addu v0,v0,s5 (s5 = 0x8007469c) / lw / 80042f8c jalr v0
```
Handler-Tabelle `table 0x8007469c`: [9] = 0x80043328 (ITEM). Zwischen @0x80042c50 und @0x80042ccc wird rec[0] (sce)
nicht gelesen -> das Band-Gate gilt fuer ITEM genauso wie fuer Tuer/Ereignis.

### 2.3 Item_aot_set legt rec = pc+2 an (rec[2] = pc[4] = floor)
```
80040690  lbu   v0,1(a2)            ; Slot = pc[1]
80040698  addiu v1,v1,-13904        ; 0x800ac9b0 (dieselbe Tabelle wie fp oben)
800406cc  addiu v0,a2,2             ; rec = pc + 2
800406d0  sw    v0,0(s0)            ; 0x800ac9b0[slot] = rec
```
Port: `op_item_aot_set` (scd_vm.c) speichert pc[4] bereits in `g_aot.slots[slot].band` — nur der Scan liest es fuer ITEM nicht.

### 2.4 Member_set 0x12 schreibt Entity+0x82 (das Band-Byte des Scans)
```
800410cc  lw    a0,340(s0)          ; Work-Entity (thread+0x154)
800410d0  lbu   a1,1(v0)            ; Member-Id = pc[1]
800410d4  lh    a2,2(v0)            ; Wert = (s16) pc[2..3]
800410d8  jal   0x8004116c
8004116c  sltiu v0,a1,0x14 / 80041178-7c Tabelle 0x80010c8c / 8004118c jr v0
          table 0x80010c8c [18] -> 0x80041224
80041224  j     0x80041230
80041228  sb    a2,130(a0)          ; (Delay-Slot) Entity+0x82 = Wert
```
Work_set kind 1 (LAB_80040d2c): `80040da4 lui v0,0x800b / 80040da8 addiu v0,v0,-13740 (= 0x800ACA54) / 80040db0 sw
v0,340(a0)` -> Member_set 0x12 nach Work_set(1,0) schreibt **0x800ACAD6** = genau das Byte, das @0x80042cc0 liest
(und Schubkette @0x8002bf38, Kletter-Sonde @0x800381c8). Im Port ist dieses Byte `s_coll_band`
(re15_collision.c, `re15_collision_debug_band()`); `re15_collision_set_band()` setzt seit Runde 30 BEIDE Haelften
(s_coll_band UND g_actors[0].floor) — `re15_actor_set_member` case 18 schreibt aber nur `floor`.

### 2.5 ROOM3050 sub04 (Lift abwaerts) — selbst dekodiert (`tools/scd_dump_room.py ROOM3050.RDT`)
`0x02390 Work_set 2e 01 00` (Spieler) ... `0x023CC Member_set 34 01 00 00` (y = 0) / **`0x023D0 Member_set 34 12 00 00`
(Band 0)** / `0x023D4 Member_set 34 13 00 00` / `0x023EC Set (3,82)=1`. sub02 (aufwaerts): `0x02328 Member_set y -7200` /
**`0x0232C Member_set 34 12 04 00` (Band 4)**.

### 2.6 Original gemessen (Spur-R-Abnahme 3, O2 — Werkzeug + Log jetzt in D_belege/orig_r36/)
DuckStation-GDB, Spielstand R_belege/room3050_orig.sav, Spieler per GDB auf (-24026,y,-22770) Blick 1072:
* A (y 0, +0x82 = 0): 3x Square -> Taken-Bit 92 = 0;  A2 (y -7200, +0x82 = 4): -> 0;  **B (y 0, +0x82 = 1): -> 1**
  (dieselbe x/z/Blick-Stellung, nur das Band anders: das Gate @0x80042cac-ccc entscheidet).
* C Lift sub04: `t=6 s: ... 'y': 0, ... 'band': 0 ..., (3,82) = 1` (vorher band 4) — der Abstieg setzt +0x82 = 0.

## 3. Zensus

## 4. Umsetzung

## 5. Messung nachher

## 6. Tests

## 7. OFFEN

## 8. Fuer den Nutzer
