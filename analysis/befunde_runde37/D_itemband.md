# Runde 37 — Spur D "itemband" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r37_itemband` (Zweig `r37/itemband`, Basis master f1e019ca).
Nutzer-Punkt (AUFTRAG.md #8, woertlich): "Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die
Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden."
Einstieg: R_abnahme_3.md M1/M2/M3/H4, NACHLAUF.md #73/#74 (Runde 36).

## 0. Stand / Fortschritt (Abschluss 2026-10-12)
- [x] Messung vorher (exe): 3050 Band 4 -> (9,92) = 1 ohne Kiste; Lift laesst Band 4 stehen, Klemme unten (Abschnitt 1)
- [x] RE-Belege selbst disassembliert: AOT-Scan @0x80042cac-ccc, Aufrufer @0x80031fdc-fe8, Item_aot_set @0x800406cc,
      Member_set @0x800410b8 -> FUN_8004116c [18] -> @0x80041228, Work_set kind 1 @0x80040da4 (Abschnitt 2)
- [x] Original gemessen: 3050 (Spur R O2), 3010 und 2030 (eigenes Werkzeug, Abschnitt 2.7)
- [x] Zensus: 12 Item_aot_set mit floor != 0, 22 Schreiber Member 0x12 (17 am Spieler) (Abschnitt 3)
- [x] Umsetzung: 2 Haken + itemband_r37.c (Abschnitt 4)
- [x] Messung nachher (exe): alle 6 Stellen falsches Band GESPERRT / vorgesehener Weg FREI + Taken-Bit (Abschnitt 5)
- [x] Tests: unit_r37_itemband + integration_r37_itemband gruen, Gegenproben rot, Regression 120/120 (Abschnitt 6)

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

### 1.1b Der vorgesehene Weg vorher (Wegplan der Spur-R-Abnahme `wp_3050_stufe2.txt`, Laufordner `vorher_3050_stufe2`)
Fall der Kiste wie im Riegel (`F1233 SCHIEB: Objekt-y -7200 -> -7192 (Fall)`, Kiste `(-24058,0,-15591) b0`), Lift AKTION F2200,
`F2601 FLAG (3,82) = 1` — climb.txt `T3201 ... pos=(-7657,0,-14178) ... band=4`: der Spieler steht nach dem Lift auf y 0
mit Band 4; `F2825 GEHE: nach 8 Runden Rest 344` bei x -7657 (Band-4-Wand x -10350..-8125 + 450 + 18), `F3228 KLEMME`;
der Kisten-Schub nach Norden laeuft ins Leere (`F4757 SCHIEB: 1500 Bilder ohne Bedingung`, `F4818 FLAG (3,93) = 0`); am Ende
trotzdem **`F5418 FLAG (9,92) = 1`** von y 0 OHNE Kiste in der Kerbe. = R_abnahme_3.md M2, reproduziert.

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
(re15_collision.c, `re15_collision_debug_band()`); `re15_collision_set_band()` setzt seit 5a415332 (2026-09-01) BEIDE Haelften
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

### 2.7 Original gemessen (Runde 37, eigenes Werkzeug `D_belege/orig_r37_itemband.py`; Logs `D_belege/orig_r37/`)
DuckStation-GDB, MZD-Disc, Auslieferungs-EXE (`EXE-Stub @0x80026e4c: 0800e003`), Raum ueber den Debug-Menue-Zustand geladen
(Zeile 0x800BBE5D = 1, Stage 0x800BBE5E, Raumindex 0x800BBE5F+stage — Muster G2_belege/orig_11f0_spielstand.py), Pruefung
`Zellen im RAM == ROOMxxxx.RDT: True`, Gegner per Aktiv-Bit aus; Spieler per GDB auf Lage/Blick/+0x82 gesetzt, Quadrat ueber
vgamepad; Taken-Bit = FUN_8004efe4(*(0x80074688), n). settings.ini nach jedem Lauf zurueck (`EnableGDBServer = false: True`).
* **ROOM3010** (`o3010_log.txt`): A (-10700,0,844) Blick 0 **Band 0 -> 3x Quadrat -> Taken-Bit 20 = 0**; C Klettern an r0[4]
  (-10322,0,-11175) Blick 3072 Band 0 -> `t=1 s: ... 'y': -1800, 'z': -10027, ... 'band': 1, 'sub': 9` (Port `n3010_weg`:
  nach dem Klettern (-10322,-10027) — dieselbe Lage); B (-10700,-1800,844) **Band 1 -> nach Druck 3 Taken-Bit 20 = 1**.
* **ROOM2030** (`o2030_log.txt`): S3 (-650,-5400,14500) **Band 3 -> SPAS-12 Taken-Bit 109 = 0**, S6 (-650,-10800,14500)
  **Band 6 -> 1**; T Treppe slot 8 aus (-6350,-5400,-7700) Blick 0 Band 3 -> `t=2 s: 'x': -1982, 'y': -1800, 'z': -7694,
  'band': 1` (Abstieg 3 -> 1; Port `n2030_schrot_weg`: `[stair] START dir=DOWN cur_band=3 target_band=1`, Ende (-2082,-7741));
  G3 (4300,-5400,7100) **Band 3 -> Schrot Taken-Bit 108 = 0**, G1 (4300,-1800,7100) **Band 1 -> 1**. (L ohne Strom: keine
  Fahrt — die Meldung "not responding", sub01 @0x01E14.)
* **ROOM2030 mit Strom** (`o2030L_log.txt`, (3,126) vor dem Laden gesetzt): L Lift A aus (6600,-5400,10900) Blick 3072 ->
  y -7680 / -9360 / **`t=3 s: 'y': -10800, 'z': 11618, 'band': 6`** — sub02 @0x01E66 setzt +0x82 = 6; S6L danach Band 6 ->
  Taken-Bit 109 = 1.
-> Fuer ALLE gemessenen erhoehten Items (3050, 3010, 2030 x2) gilt im Original: Aufnahme NUR auf dem Item-Band; die
vorgesehenen Wege (Lift = Member 0x12, Klettern, Treppe) bringen den Spieler auf genau dieses Band.

## 3. Zensus (alle ausgelieferten RDTs, main- und sub-SCD)

Werkzeug `D_belege/zensus_itemband.py` (Walk = tools/scd_dump_room.py) und derselbe Walk in C im Riegel
`unit_r37_itemband` Teil Z (Laengen = `scd_opcode_size_at` des Motors). Beide: **206 RDTs, 40674 Opcodes, 0 Desync,
164 Item_aot_set** (= test_r36_kartenbits).

### 3.1 Item_aot_set mit floor != 0 — genau 12 (floor-Verteilung aller 164: {0: 152, 1: 8, 3: 2, 6: 2}; Bit 0x80: 0)
| Raum | Satz | Slot | floor | Item | tk (Bank 9) | Rechteck x/z/w/d | Weg auf das Band (Port gemessen, Abschnitt 5) |
|---|---|---|---|---|---|---|---|
| 2030/2031 | main00 @0x01BC0 | 10 | 1 | 0x16 SHOTGUN SHELLS x14 | 108 | 4350,6650,800,1000 | Treppe slot 8 @0x01B8E (sce 12, Seite 1, count 2): West-Haelfte = Abstieg Band 3 -> 1, dann Band-1-Gang |
| 2030/2031 | main00 @0x01BD6 | 11 | 6 | 0x0D SPAS-12 x12 | 109 | -50,13500,1000,2000 | Lift A slot 5 -> sub02 (@0x01E66 Member 0x12 = 6) |
| 2040/2041 | main00 @0x01364 | 7 | 3 | 0x32 Head of Akuma | 91 | 1700,-14750,1000,1000 | Halle Band 3 = Ankunft 2050 slot 2 @0x01C8A / 2060 slot 1 @0x00DBA |
| 2090/2091 | main00 @0x00A12 | 5 | 1 | 0x3F Pocket Watch | 90 | 5350,-16700,1000,1000 | Plattform Band 1 = Ankunft 20A0 slot 0 @0x01746 (6300,-1800,-16250) |
| 3010/3011 | main00 @0x02094 | 5 | 1 | 0x16 SHOTGUN SHELLS x7 | 20 | -10533,826,560,630 | Steg Band 1: Klettern an r0[4] (Band 0, u1 0x01, x -13124..-7688 z -10532..-9002) |
| 3050/3051 | main00 @0x01EC6 | 6 | 1 | 0x07 SUPER REDHAWK x6 | 92 | -24800,-23700,1800,800 | Kistendach: Kiste faellt (Band 4 -> 0), Lift sub04 (@0x023D0 Member 0x12 = 0), Kiste in Kerbe slot 7 @0x01F06, Latch (3,93) sub01 @0x02184-2196, Klettern auf die Kiste (Band 0 -> 1) |

Die 152 Saetze mit floor 0 liegen ausnahmslos in Raeumen, deren Tuer-Ankuenfte Band 0 enthalten (Join Ankunfts-Y aller 653
Door_aot_set gegen die Item-Raeume, `zensus_itemband.py --tueren`); Ausnahme nur ROOM2040/2041 sub00 @0x014F8 slot 13
mit Rechteck (0,0,0,0) (Geometrie trifft nie). 9 Saetze haben ein Null-Rechteck, 2 sind traege Erstinstallationen (sce 0,
ROOM1190/1191 sub13 @0x02A30, spaeter Aot_reset).

### 3.2 Schreiber des Members 0x12 — 22, alle Member_set 0x34 (kein 0x35/0x5A/0x5B mit Feld 0x12)
Am SPIELER (letztes Work_set der Region kind 1 -> 0x800ACA54), 17 Stellen:
| Raum | Stelle | Wert | Kontext |
|---|---|---|---|
| 1170 | sub14 @0x01756 | 4 | Pos_set (-11710,-7200,-26500) = Hof (Ankunft Tuer 0 ist schon Band 4) |
| 2030/2031 | sub02 @0x01E66 / @0x01EA8 | 6 / 3 | Lift A auf / ab ((3,222)) |
| 2030/2031 | sub04 @0x01F40 / @0x01F82 | 9 / 3 | Lift B auf / ab ((3,220)) |
| 3050/3051 | sub02 @0x0232C / sub04 @0x023D0 | 4 / 0 | Lift auf / ab ((3,82)) |
| 50D0/50D1 | sub02 @0x013D4 / sub04 @0x0144C | 3 / 0 | Lift mit Frage (Message_on 0, Ck (12,31)); Tueren slot 0/2 floor 3, slot 1/3 floor 0 |
An FIGUREN (Work_set kind 2), 5 Stellen: ROOM10B1 sub00 @0x017DA / sub03 @0x018C2, ROOM11C0 sub02 @0x01AC2 / sub04 @0x01C10 /
sub07 @0x01C66 — bleiben `g_actors[].floor` (kein Kollisionsband).
**Nebenbefund (vom Haken mit behoben):** ohne Member 0x12 -> Kollisionsband blieb der Spieler nach JEDEM dieser Lifte auf
dem alten Band. ROOM50D0 ABFAHRT an der exe gemessen (Abschnitt 5.7: ohne Haken Klemme an der Band-3-Wand auf y 0, keine
untere Tuer; mit Haken Band 0 und Tuer slot 1 -> 50F0). Nach dem Code (Tuer-Band-Gate aot_common.c `door_params[i].band != pb`,
NICHT an der exe gefahren) scheiterten ebenso nach der 50D0-AUFFAHRT die floor-3-Tueren (slot 0 -> 50C0, slot 2 -> 50E0) und
in ROOM2030 nach Lift B die floor-9-Tueren. Riegel Teil M zeigt das Band nach jeder Fahrt: 0 -> 3 -> 0 bzw. 3 -> 9 -> 3.

## 4. Umsetzung

Neue Dateien: `re15_port/include/re15_itemband_r37.h` (Belege, Konstanten), `re15_port/engine/src/itemband_r37.c`.
Haken in gemeinsamen Dateien (je 1 Aufruf + Include, Kommentar "Runde 37 Spur D"):
* `engine/src/aot_common.c` re15_aot_scan, direkt nach dem GENERIC-Gate:
  `if (a->type == RE15_AOT_TYPE_ITEM) re15_itemband_r37_gate(i, &gen_reach, &inside);`
  -> gesperrt, wenn Spielerband (s_coll_band = 0x800ACAD6, @0x80042cc0) >= 0, rec-Band ohne Bit 0x80
  (`RE15_R37D_BAND_EGAL` @0x80042cb4) und rec-Band != Spielerband (@0x80042ccc). Das rec-Band ist schon seit jeher
  `g_aot.slots[slot].band = pc[4]` (op_item_aot_set). Gleiche Bedingung wie das vorhandene GENERIC-Gate (b89ada17).
* `engine/src/actor_common.c` re15_actor_set_member case 18 (`RE15_R37D_MEMBER_BAND`, Tabelle 0x80010c8c[18]):
  nach `a->floor = value` -> `re15_itemband_r37_member_band(slot, value)`: nur Spieler-Slot ->
  `re15_collision_set_band((uint8_t)value)` (setzt s_coll_band UND g_actors[0].floor; `sb a2,130(a0)` @0x80041228
  schreibt das untere Byte). Member_set2/0x5A/0x5B laufen ueber dieselbe Funktion.
* Messschrieb `RE15_ITEMBAND_LOG=<datei>` (Diagnose, kein Verhalten): je Aktionsdruck mit ITEM in Reichweite
  `ITEM slot s floor f ... spieler-band b -> GESPERRT|FREI`, je Member 0x12 am Spieler `MEMBER 0x12 Spieler:
  Kollisionsband a -> b`.
Keine Konstante ohne Beleg: 18 = Member-Id (@0x80010c8c[18] -> @0x80041228), 0x80 = Ignorierbit (@0x80042cb4).
Port-synthetische ITEM-Saetze (dokumente_r34, inhalt_r35, irons_tisch_1150, schluessel_r36) setzen ihr Band selbst
(0 bzw. ROOM1090-Schrot 5 = Dachband) und liegen auf ihrem Spielerband — vom Gate nicht betroffen (geprueft).

## 5. Messung nachher (exe nach dem Fix, Messkopie re15_pc_r37d.exe, `D_belege/lauf_d.sh`, RE15_ITEMBAND_LOG)

Laufordner Scratchpad `r37d/<lauf>/`; Auszuege (wegplan.txt, itemband.txt, climb-Auszug) in `D_belege/<lauf>/`.

### 5.1 ROOM3050 — der Nutzer-Fall (Super Redhawk, floor 1, tk 92)
* **`nachher_3050_oben`** (derselbe Wegplan wie 1.1, obere Ebene Band 4, Kiste unberuehrt): itemband.txt
  `F807/F870/F933 room=3050 ITEM slot 6 floor 1 item 0x07 tk 92 spieler-band 4 spieler=(-24026,-7200,-22770) -> GESPERRT
  (@0x80042ccc bne Band)`; wegplan.txt **`F1036 FLAG (9,92) = 0`** (vorher: 1).
* **`nachher_3050_stufe2`** (vorgesehener Weg, Wegplan der Spur-R-Abnahme `wp_3050_stufe2.txt`: West-Schub, Sued-Schub
  bis zum Fall, Lift, unten zur Kiste, Schub nach Norden in die Kerbe, Klettern, Aufnahme):
  `F1233 SCHIEB: Objekt-y -7200 -> -7192 (Fall)` -> Kiste `(-24058,0,-15591) b0`; Lift AKTION F2200 -> itemband.txt
  **`MEMBER 0x12 Spieler: Kollisionsband 4 -> 0 (@0x80041228) y=0`**, climb.txt `T2400 ... pos=(-2038,-2400,-8179) ...
  band=4` -> `T2450 ... pos=(-2038,0,-8179) ... band=0`; `F2601 FLAG (3,82) = 1`; danach unten OHNE Klemme
  (`F2738 ... (-7990,-8145) angekommen`, `F3108 ... (-24026,-14241) angekommen` — vorher `F2825 GEHE: nach 8 Runden
  Rest 344` an der Band-4-Wand x -7657); `F3451 SCHIEB: Bedingung erfuellt (obj.z = -22225)`, **`F3512 FLAG (3,93) = 1`**
  (Latch sub01 @0x02184-2196); Klettern auf die Kiste (AKTION F3537, mo=230); **`F3805 ITEM slot 6 floor 1 ... spieler-band
  1 spieler=(-24072,-1800,-22732) -> FREI`**, **`F4034 FLAG (9,92) = 1`**.
  -> Der vorgesehene Weg ist an der exe Ende-zu-Ende gangbar (vorher: Klemme nach dem Lift, Aufnahme von y 0 mit Band 4).

### 5.2 ROOM3010 — Steg (SHOTGUN SHELLS x7, floor 1, tk 20)
* **`n3010_unten`** (Band 0 UNTER dem Steg, `RE15_PLAYER_POS=-10700,844,0,0`, Blick Ost, AKTION x3): itemband.txt 3x
  `ITEM slot 5 floor 1 item 0x16 tk 20 spieler-band 0 spieler=(-10700,0,844) -> GESPERRT`; `F314 FLAG (9,20) = 0`.
  (Die Band-0-Flaeche unter dem Steg hat keine Band-0-Zelle — vor dem Fix griff der Spieler hier durch den Steg.)
* **`n3010_weg`** (Ankunft der 3020/30D0-Tuer (-7400,0,-18400) Band 0 -> Nordkante des Stegs, Blick Sued, AKTION =
  Klettern an r0[4]): climb.txt `T464 ... pos=(-10322,0,-11175) rot=3072 band=0 sub=9` (Kletter-Substate) -> Steg;
  `F552 ... (-10707,834) angekommen`, Blick Ost, **`F582 ITEM slot 5 floor 1 ... spieler-band 1 spieler=(-10707,-1800,834)
  -> FREI`**, **`F811 FLAG (9,20) = 1`**.

### 5.3 ROOM2090 — Plattform (Pocket Watch, floor 1, tk 90)
* **`n2090_weg`** (ECHTE Tuer: `RE15_DEBUG_JUMP=20A0`, Spieler an der Tuer slot 0 @0x01746 (floor 2) Band 2 = Ankunftsstelle
  der 2090-Tuer, AKTION): debug.log `[aot] DOOR FIRE slot=0 rect=(4950,5000,hw=1500,hh=500) target_cut=2
  spawn=(6300,-1800,-16250)`, `[tuer] Sequenz ... DOOR26`, `loaded room2090.rdt`; Blick West, **`F87 ITEM slot 5 floor 1
  item 0x3F tk 90 spieler-band 1 spieler=(6300,-1800,-16250) -> FREI`**, `F316 FLAG (9,90) = 1`.
* **`n2090_unten`** (Band 0 im Wasser an derselben x/z, Blick West): der erste Druck startet NICHT die Aufnahme, sondern das
  Klettern auf die Plattform (Kletter-Sonde FUN_8002d474 laeuft @0x80031fd0 VOR dem AOT-Scan; Zelle r0[5] Band 0 u1 0x01):
  climb.txt 31 Bilder `band=0 sub=9`, dann `band=1`; erst danach `F148 ... spieler-band 1 ... -> FREI`. Ein Band-0-Spieler
  erreicht das Rechteck geometrisch nicht (r0[5] umschliesst die Plattform) — in 2090 war der Defekt im Spiel nicht
  sichtbar; das Gate fuer diesen Satz belegt der Riegel (Teil G: Band 0/2/4 -> kein Modal).

### 5.4 ROOM2040 — Halle (Head of Akuma, floor 3, tk 91)
* **`n2040_galerie`** (Band 5 = Galerie-Band an der Item-Stelle (2200,-13800), Blick Nord): 3x `ITEM slot 7 floor 3 item 0x32
  tk 91 spieler-band 5 spieler=(2200,-9000,-13800) -> GESPERRT`; `F314 FLAG (9,91) = 0`.
* `n2040_weg` (Halle Band 3 = Ankunft der 2050-Tuer slot 2 (21700,-5400,-5400), ohne Vorbedingung): Klemme bei (4646,-5582)
  — **Spielvorgabe, kein Port-Defekt**: ROOM2040 sub00 @0x0141A `Ck (3,146)=0` bzw. @0x01452 `Ck (3,53)=0` -> `Sca_id_set
  37 0x 13 ff` = Zelle r*[19] (x 4150..5950 z -6500..-6050, im RDT u0 0x00) wird gesperrt, der Nordgang zur Head of Akuma
  oeffnet erst das Tor-Raetsel ((3,53) in sub02 @0x01624).
* **`n2040_weg2`** (dasselbe mit `RE15_SET_FLAG=3:146,3:53` = Raetsel geloest): `F554 ITEM slot 7 floor 3 ... spieler-band 3
  spieler=(2215,-5400,-13727) -> FREI`, **`F783 FLAG (9,91) = 1`**.

### 5.5 ROOM2030 — SPAS-12 (floor 6, tk 109) ueber Lift A
* **`n2030_spas_b3`** (Band 3 an der SPAS-12 (-650,14500), Blick Ost): 3x `ITEM slot 11 floor 6 item 0x0D tk 109 spieler-band
  3 spieler=(-650,-5400,14500) -> GESPERRT`; `F314 FLAG (9,109) = 0`. (Original S3: Taken-Bit 109 = 0.)
* `n2030_spas_weg` (ohne Strom): AKTION am Lift -> Meldung "The elevator is not responding" (sub01 @0x01E14 `Ck (3,126)=0`
  -> Aot_reset slot 5/7 sce 1) — Spielvorgabe (Sicherung in ROOM2060 sub @0x012AE `Set (3,126)=1`).
* `n2030_spas_weg2` (Strom `RE15_SET_FLAG=3:126`, Start (6594,10846) am Kabinenrand): Lift faehrt, `MEMBER 0x12 Spieler:
  Kollisionsband 3 -> 6 (@0x80041228) y=-10800`, aber die Ausschiebung aus der Band-6-Kabinenwand r0[49] (z 10650..11150)
  setzt den Spieler NACH NORDEN (z 10182, kuerzerer Weg 646 < 754) — das Original stand im Messlauf bei z 10900 (Mitte,
  Gleichstand 700/700) und landete suedlich (z 11618). Startstellung des Messlaufs, kein Mechanismus-Unterschied.
* **`n2030_spas_weg3`** (Strom, Start IN der Kabine (6600,12000), Blick Sued, AKTION): `F361 MEMBER 0x12 Spieler:
  Kollisionsband 3 -> 6 (@0x80041228) y=-10800`; `F807 (6605,13695) angekommen` (Kabine nach Sueden verlassen),
  `F953 (-637,14499) angekommen`; **`F996 ITEM slot 11 floor 6 ... spieler-band 6 spieler=(-637,-10800,14499) -> FREI`**,
  **`F1225 FLAG (9,109) = 1`**. Dauer des Laufs 161 s (2600 Bilder unter Last).

### 5.6 ROOM2030 — SHOTGUN SHELLS x14 (floor 1, tk 108) ueber die Treppe
* **`n2030_schrot_b3`** (Band 3 = Hallenhoehe y -5400 an der Item-Stelle (4300,7100)): 3x `ITEM slot 10 floor 1 item 0x16 tk 108
  spieler-band 3 spieler=(4300,-5400,7100) -> GESPERRT`; `F314 FLAG (9,108) = 0` (Original G3: 0).
* **`n2030_schrot_weg`** (Ankunft aus 2070 (6550,-5400,-100) Band 3 -> Ostgang -> Nordgang -> Absatz (-6364,-7750) -> Blick Ost
  -> AKTION): stair_dbg.log `[stair] START dir=DOWN cur_band=3 target_band=1`, Ende (-2082,-7741) (Original T: (-1982,-1800,-7694)
  Band 1); Band-1-Gang `F1049 (-1008,6978)`, `F1155 (4251,7118)`; **`F1179 ITEM slot 10 floor 1 ... spieler-band 1
  spieler=(4251,-1800,7118) -> FREI`**, **`F1408 FLAG (9,108) = 1`**.
  (Damit ist auch die Treppen-Aussage der Spur R ("2030 Treppe Band 3 -> 5") eingeordnet: sie stammt aus einer Messung mit
  Band 3 am OST-Ende (-1800,-7700), dem Band-1-Ende der Treppe; Seite 1 + Ost-Haelfte = Aufstieg. Vom Absatz (West) fuehrt die
  Treppe im Port wie im Original auf Band 1.)

### 5.7 Nebenbefund Member 0x12 an der exe: ROOM50D0-Lift (Tueren floor 0 nach der Abfahrt)
* **`n50d0_lift`** (Ankunft aus 50C0 (-23437,-5400,-4400) Band 3 -> Lift slot 5 (sce 3 -> sub04, Frage "There's a button here.
  Push it?" JA) -> nach Osten zur Tuer slot 1 @0x011FA (floor 0) -> 50F0): itemband.txt **`F587 room=50D0 MEMBER 0x12 Spieler:
  Kollisionsband 3 -> 0 (@0x80041228) y=0`**; `F987 FLAG (3,83) = 1`; `F1128 (-9101,-14880) angekommen`, `F1235 (-2964,-14864)
  angekommen`; debug.log **`[aot] DOOR FIRE slot=1 rect=(-2021,-14842,hw=329,hh=1058) target_cut=1 spawn=(-24550,0,-23650)`**,
  `loaded room50f0.rdt`.
* **GEGENPROBE `n50d0_lift_ohne_haken`** (derselbe Lauf, NUR der Haken in actor_common.c case 18 auskommentiert, exe neu
  gebaut, danach wiederhergestellt und neu gebaut): nach der Abfahrt (`F987 FLAG (3,83) = 1`, y 0) climb.txt `T1751 room=50D0
  ... pos=(-17297,0,-14319) ... band=3` — der Spieler klemmt auf y 0 an der BAND-3-Wand r0[27] (x -16829..-14773; -16829 - 450
  = -17279), keine Tuer, kein Raumwechsel. Der Stand vor dem Fix machte damit die unteren 50D0-Tueren (slot 1 -> 50F0,
  slot 3 -> 5100) nach einer Abfahrt unerreichbar (bis zur erneuten Auffahrt).

## 6. Tests (registriert NUR in `re15_port/tests/unit/probes/r37_itemband.cmake`)

* **`unit_r37_itemband`** (`tests/unit/test_r37_itemband.c`, 0,33 s):
  Z Zensus (206 RDTs / 40674 Opcodes / 0 Desync / 164 Item_aot_set; genau die 12 erhoehten Saetze, kein Bit 0x80; 22 Schreiber
  Member 0x12, alle 0x34, genau die 17 am Spieler + 5 an Figuren);
  M echte Subs ROOM3050 sub04/sub02 (Band 4 -> 0 / 0 -> 4, y, (3,82)), ROOM2030 Lift A/B (3 -> 6 -> 3, 3 -> 9 -> 3), ROOM50D0
  (0 -> 3 -> 0, Frage mit JA), Gegenprobe Figur-Slot (Band bleibt), Spieler-Slot direkt (Member_get liest denselben Wert);
  G alle 153 Item-Saetze mit Geometrie ueber den echten Opcode-Handler + re15_aot_scan: Modal GENAU bei Band == floor (318/318
  Druecke auf falschem Band gesperrt), die 12 erhoehten bis zum Taken-Bit.
  **Gegenprobe ohne die beiden Haken** (Haken auskommentiert, nur das Testziel neu gebaut, danach wiederhergestellt;
  `D_belege/unit/gegenprobe_ohne_haken.log`): `falsch: 153`, `0 von 318` gesperrt, die erhoehten `(0)`, Lifte ohne
  Bandwechsel (ROOM3050 sub04 / 2030 sub02+sub04 / 50D0 sub02 FAIL) — der Riegel diskriminiert.
* **`integration_r37_itemband`** (`tests/integration/test_r37_itemband.cmake`, echte exe mit eigener Kopie
  `re15_pc_r37d_haken_<id>`, RE15_WEGPLAN + RE15_ITEMBAND_LOG): 12 Laeufe oben3050 / weg3050 / elza3051 / unten3010 / weg3010 /
  weg2090 / galerie2040 / weg2040 / spas2030 / spasb3 / schrot2030 / schrotb3 (je falsches Band GESPERRT + Taken-Bit 0,
  vorgesehener Weg FREI + Taken-Bit 1; Lift-Laeufe verlangen die MEMBER-Zeile, der Treppenlauf `[stair] START dir=DOWN
  cur_band=3 target_band=1`, weg2090 die echte Tuer-Zeile `DOOR FIRE slot=0 ... spawn=(6300,-1800,-16250)`).
  Ergebnis (eigener Lauf, `D_belege/integ_lauf_88ef3bd9/`): `ctest --test-dir re15_port/build -R "^integration_r37_itemband$"
  --timeout 3000 --output-on-failure` -> `1/1 Test #679: integration_r37_itemband .........   Passed  821.83 sec` /
  **`100% tests passed, 0 tests failed out of 1`**; je Lauf: oben3050 3 GESPERRT (9,92)=0 | weg3050 MEMBER 4 -> 0, FREI,
  (9,92)=1 | elza3051 `loaded room3051.rdt`, 3 GESPERRT, (9,92)=0 | unten3010 3 GESPERRT (9,20)=0 | weg3010 FREI (9,20)=1 |
  weg2090 FREI (9,90)=1 | galerie2040 3 GESPERRT (9,91)=0 | weg2040 FREI (9,91)=1 | spas2030 MEMBER 3 -> 6, FREI (9,109)=1 |
  spasb3 3 GESPERRT (9,109)=0 | schrot2030 FREI (9,108)=1 | schrotb3 3 GESPERRT (9,108)=0.

### 6.1 Gezielte Regression (Spurbaum, keine volle Suite; Gruppen jedes Moduls, das der Diff beruehrt)
Diff (`git diff f1e019ca --name-only -- re15_port`): engine/src/aot_common.c (AOT-Scan: Items, Tueren, Ereignisse),
engine/src/actor_common.c (Member-Zugriffe der SCD -> Kollisionsband: Treppen/Klettern/Schub/Fahrstuehle), neue Dateien.
Befehl (Regex auch in `D_belege/unit/regression_ctest.log`):
`ctest --test-dir re15_port/build -R "r37_itemband|aot|item|inventar|itembox|member|scd_|band|stair|klett|climb|dachkiste|objfall|push|fahrstuhl|elevator|1170|1090|10f0|3050|2050|2060|irons_tisch|dokumente|inhalt|schluessel|kartenitems|kartenbits|r36_karte|elza_vollstart|kleidung_spielweg|r16_objekte" -E "^integration_r37_itemband$" --timeout 3000 -j 3 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 120`** (`Total Test time (real) = 1536.11 sec`), darin die Spur-R/A-Riegel
`unit_r36_dachkiste_zensus` / `unit_r36_dachkiste_zellschub` / `integration_r36_dachkiste` (Passed 1359.68 sec) /
`unit_r36_klettern_*` / `integration_r36_klettern` (Passed 503.78 sec) / `unit_r36_objfall`, dazu `integration_elza_vollstart`,
`integration_r36_kleidung_spielweg`, `integration_r36_inventar`, `integration_r36_schluessel*`, `integration_r30_irons_tisch_*`,
`unit_aot_*`, `unit_member`, `unit_scd_*`, `unit_stair_*`, `unit_fahrstuhl_*`, `unit_r37_itemband`. Kein Fenster-Haken musste
nachgefahren werden.

## 7. OFFEN

* Treppen-Endlage ROOM2030 slot 8 (Abstieg vom Absatz): Port (-2082,-7741), Original (-1982,-1800,-7694) — 100 Einheiten in x,
  47 in z (stair_common.c, nicht Teil dieses Auftrags; fuer die Aufnahme ohne Folge, beide auf Band 1 im Gang).
* Lift-Ausschiebung ROOM2030 Lift A bei Start genau auf der Kabinenwand r0[49]: Original (Start z 10900, Gleichstand) nach Sueden,
  Port (Start z 10846) nach Norden — kuerzerer Weg; der Gleichstandsfall selbst ist nicht verglichen (Startstellung des
  Messlaufs; im Spiel steht man IN der Kabine).
* Member 0x13 (+0x1ba) am Spieler: der Port schreibt `g_actors[0].dog_floor_y`, der Kletter-Code fuehrt +0x1ba als eigenen
  Spiegel `s_floor_y` (climb_common.c). Ohne Folge fuer diesen Auftrag: s_floor_y wird vor jedem Lesen gesetzt (@0x80038494).
* ROOM50D0-Lift: an der exe die ABFAHRT gefahren (5.7); die Auffahrt (Tueren floor 3) nur im Riegel (Teil M: 0 -> 3).

## 8. Fuer den Nutzer

* **Behoben:** Gegenstaende auf erhoehten Flaechen nimmt man nur noch auf dem Stockwerk auf, auf dem sie liegen — genau wie im
  Original (am Original gemessen: ROOM3050, 3010, 2030). Die Super Redhawk in ROOM3050 gibt es nur noch von der Kiste aus:
  Kiste oben nach Westen und dann nach Sueden ueber die Kante schieben, mit dem Lift hinunterfahren, die Kiste unten nach
  Norden in die Nische schieben, auf die Kiste klettern, dann die Super Redhawk nehmen.
* Betroffen sind 12 Gegenstaende (6 Stellen, je Leon und Elza): ROOM2030 Schrotpatronen (Gang unten, ueber die Treppe vom
  Absatz) und SPAS-12 (oben, mit Lift A), ROOM2040 Head of Akuma (Halle), ROOM2090 Pocket Watch (Plattform), ROOM3010
  Schrotpatronen (Steg, hochklettern), ROOM3050 Super Redhawk (Kiste). Alle bleiben auf ihrem vorgesehenen Weg erreichbar.
* **Mit behoben:** Fahrstuehle, die das Stockwerk per Skript setzen (ROOM3050, ROOM2030 Lift A/B, ROOM50D0; ROOM1170 sub14 setzt Band 4 = Hof, dort ohne Aenderung),
  setzen jetzt auch das Stockwerk der Kollision — vorher stand Leon nach der Fahrt "im falschen Stockwerk" (ROOM3050: Klemme
  an unsichtbaren Waenden unten).
* Keine neuen Assets (Paket-Gate unveraendert), keine Sprachdateien, keine Bedienaenderung, keine Entscheidungsfrage.

## 9. Abschluss

Zweig `r37/itemband` (Basis master f1e019ca), Code-Commit 6aeb2086 (die zwei Haken + itemband_r37.c), Riegel 8c44bdee /
4c0221e4 / a4331e65, Belege und Dossier in den uebrigen wip-Commits; letzter Commit `fix(r37-itemband)` (dieser Abschnitt).
Gemeinsame Dateien: `re15_port/engine/src/aot_common.c` (+3 Zeilen: Include, Kommentar, Aufruf nach dem GENERIC-Gate),
`re15_port/engine/src/actor_common.c` (+3/-1: Include, case 18 mit Aufruf). Keine Installer-Zeile in scd_room_setup.c noetig
(kein Raum-Installer), keine neuen Assets, keine Sprachdateien, keine neuen Bank-9-Bits, keine Port-Text-Ids.

## Nachbesserung 1 (nach Abnahme 0, 2026-10-12) — fortlaufend

Maengel aus `D_abnahme_0.md` §8: (1) Softlock ROOM2030/2031 Lift A AUFWAERTS aus dem Kabineneingang, (2) dasselbe ABWAERTS
knapp suedlich der oberen Kabine, (3) Dossier §5.5/§7/§8 verharmlosen bzw. verschweigen die Falle, (4) kein Riegel fuer die
Lift-Ausloesung ausserhalb der Kabine.

### NB1.0 Stand
- [ ] Messung vorher an der exe (Falle auf/ab reproduziert)
- [ ] RE-Belege: RE1.5 Lift A (Ausloeser sat 0x31 = nur Punkt 620 voraus) / RE2 Retail-Vorbild
- [ ] Umsetzung
- [ ] Messung nachher (Leon + Elza, beide Richtungen; vorgesehener Weg bleibt)
- [ ] Riegel + Gegenprobe
- [ ] Dossier §5.5/§7/§8 berichtigt
