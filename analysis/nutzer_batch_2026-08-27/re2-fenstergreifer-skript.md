# RE2 — SCD-Walker repariert + Suche nach der Fenster-Szene

Auftrag 2, Sitzung 2026-08-27. Binary: `info/re2leon/PSX.EXE` (SLUS_007.48),
`t_addr = 0x80010000` (Header @0x18), Text ab Datei-Offset `0x800`
⇒ `file_off = 0x800 + addr - 0x80010000`.

Werkzeuge dieser Sitzung:
* `analysis/nutzer_batch_2026-08-27/tools/re2_scd_oplen.py` — symbolische Ableitung der
  Opcode-Längen aus den Handlern.
* `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py` — der reparierte Walker.

---

## 1. Der RE2-SCD-Interpreter — Mechanismus (belegt)

### 1.1 Dispatch-Tabelle @0x800a74c8, 0x8F Einträge (Opcode 0x00..0x8E)

Gefunden über die neun im Auftrag genannten Handler-Adressen: jede kommt **genau einmal**
als Wort in der EXE vor, und zwar in einem dichten Block:

| Opcode | Handler | Wort-Adresse | Datei-Offset |
|---|---|---|---|
| 0x01 | 0x800537fc | 0x800a74cc | 0x97ccc |
| 0x03 | 0x80053878 | 0x800a74d4 | 0x97cd4 |
| 0x07 | 0x80053964 | 0x800a74e4 | 0x97ce4 |
| 0x10 | 0x80053e0c | 0x800a7508 | 0x97d08 |
| 0x13 | 0x80054020 | 0x800a7514 | 0x97d14 |
| 0x17 | 0x8005415c | 0x800a7524 | 0x97d24 |
| 0x18 | 0x800541a8 | 0x800a7528 | 0x97d28 |
| 0x19 | 0x80054210 | 0x800a752c | 0x97d2c |
| 0x1a | 0x80054268 | 0x800a7530 | 0x97d30 |

`0x800a74cc − 1*4 = 0x800a74c8` ⇒ **Tabellenbasis = 0x800a74c8** (Datei-Offset `0x97cc8`),
alle neun Indizes gehen auf.

Die Tabelle wird beim Raum-Init in den Scratchpad kopiert:
```
80053598: lui   a1,0x800a
8005359c: addiu a1,a1,29896        ; a1 = 0x800a74c8
800535a0: jal   0x80010778         ; memcpy
800535a4: addiu a2,zero,1024       ; 1024 Byte = 256 Worte
80053588: lui   a0,0x1f80          ; a0 = 0x1f800000 (Scratchpad)
```

### 1.2 ⛔ RE2 hat KEINE Opcode-Längentabelle

Die Hauptschleife des Interpreters ist `FUN_800536c4` (10 Tasks à 372 Byte ab `0x800d7860`):
```
800536fc: lw   v0,28(s0)        ; v0 = task->pc      (task + 0x1c)
80053704: lbu  v0,0(v0)         ; opcode = *pc
80053708: lui  v1,0x1f80
8005370c: sll  v0,v0,2
80053710: addu v0,v0,v1
80053714: lw   v0,0(v0)         ; handler = scratchpad[opcode]
8005371c: jalr v0
80053720: addu a0,s0,zero       ; a0 = task
80053724: addu v1,v0,zero
80053728: addiu v0,zero,1
8005372c: beq  v1,v0,0x800536fc ; return 1  -> naechster Opcode
80053730: addiu v0,zero,2
80053734: beq  v1,v0,0x80053790 ; return 2  -> Task gibt ab (yield)
```
Zwischen dem `jalr` und dem Neu-Laden von `task->pc` steht **keine** Addition und **kein**
Tabellen-Lookup. Die Schleife liest `task->pc` nach jedem Handler frisch aus dem
Task-Block. ⇒ **Jeder Handler schiebt den PC selbst weiter.**

Das ist der eigentliche Fehler des früheren Laufs: gesucht wurde eine Datentabelle, die es
in RE2 nicht gibt. Die Längen stehen als `addiu` **im Code jedes Handlers**.
(Gegenprobe: die Byte-Folge der RE1.5-Port-Tabelle `01 02 01 04 04 02 04 04 02 04 03 01 01 06 02 04`
kommt weder in `info/Re1.5/PSX.EXE` noch in `info/re2leon/PSX.EXE` vor — auch RE1.5 hat
keine solche Datentabelle im Binary; die Tabelle in
`re15_port/_legacy_minimal/engine_src/scd_vm.c:389` ist abgeleitet, nicht extrahiert.)

### 1.3 Ableitung der Längen aus den Handlern

`tools/re2_scd_oplen.py` führt jeden der 143 Handler symbolisch aus. Register tragen
den Wert `PC + k`:
* `lw rD,0x1c(task)` → `rD = PC+0`
* `addiu rD,rS,imm` → `PC+(k+imm)`
* `sw rS,0x1c(task)` → **Store**: das ist die exakte Länge, wenn konstant
* `l[bh]u/l[bhw] rD,off(rS)` mit `rS = PC+k` → **Read** bei `k+off` (Mindestlänge)

Für 132 der 143 Opcodes liefert das direkt ein konstantes `pc += N`.

Vollständige Ableitung (`tools/re2_scd_oplen.py`, Ausgabe gekürzt auf die Länge):

```
/*0x00*/ 1, 2, 1, 4, 4, 2, 4, 4, 2, 1, 3, 1, 1, 6, 2, 4
/*0x10*/ 2, 4, 2, 4, 6, 2, 2, 6, 2, 2, 2, 6, 1, 4, 1, 1
/*0x20*/ 1, 4, 4, 6, 4, 3, 6, 4, 1, 2, 1, 6,20,38, 3, 4
/*0x30*/ 1, 1, 8, 8, 4, 3,12, 4, 3, 8,16,32, 2, 3, 6, 4
/*0x40*/ 8,10, 1, 4,22, 5,10, 2,16, 8, 2, 3, 5,22,22, 4
/*0x50*/ 4, 6, 6, 6,22, 6, 4, 8, 4, 4, 2, 2, 3, 2, 2, 2
/*0x60*/14, 4, 2, 1,16, 2, 1,28,40,30, 6, 4, 1, 4, 6, 2
/*0x70*/ 1, 1,16, 8, 4,22, 3, 4, 6, 1,16,16, 6, 6, 6, 6
/*0x80*/ 2, 3, 3, 1, 2, 6, 1, 1, 3, 1, 6, 6, 8,24,24
```

Zwei Handler brauchten ein größeres Analysefenster, ihre Länge steht weit hinten im Rumpf:
* Opcode **0x44** (`sce_em_set`): `@0x800576e4 addiu v1,v1,22` / `@0x800576e8 sw v1,28(s5)` → **22**
* Opcode **0x8E**: `@0x80057d78 addiu v1,v1,24` / `@0x80057d7c sw v1,28(s5)` → **24**

---

## 2. Die neun Kontrollfluss-Opcodes im Einzelnen

Genau diese neun (und nur diese) haben **keinen** konstanten `pc += N`-Store — bestätigt
durch `re2_scd_oplen.py`.

### 0x01 `evt_end` @0x800537FC — **Länge 2** (aus den Daten, §3.2)
```
800537fc: addu  a3,a0,zero
80053800: lb    a2,2(a3)          ; Gosub-Nest
80053808: bne   a2,zero,0x8005381c
8005380c: addiu a2,a2,-1
80053810: sb    zero,1(a3)        ; Nest 0 -> Task-Flag loeschen
80053814: j     0x80053858
80053818: addiu v0,zero,2         ;   -> return 2 = Task beendet
80053828: lw    a1,324(v0)        ; sonst: gespeicherten PC vom Stack
80053850: sw    a1,28(a3)         ; task->pc = a1        (ABSOLUT)
```
Liest **kein** Operandenbyte → Länge nicht aus dem Handler ableitbar.

### 0x03 `evt_chain` @0x80053878 — **Länge 4**
```
80053880: lw    v0,28(a0)
80053888: lbu   a1,3(v0)          ; Operand bei +3  => Instruktion >= 4 Byte
8005388c: jal   0x800530ec        ; = evt_next: setzt task->pc absolut auf
                                  ;   sub_table_base + u16 sub_tab[a1]  (@0x8005312c/0x80053130)
```
Operand bei +3 ⇒ ≥ 4. Das Ziel-Orakel (§3.3) schließt 5 und 6 aus ⇒ **4**.

### 0x07 `else_ck` @0x80053964 — **Länge 4**
```
80053964: lw    v0,320(a0)
8005396c: addiu v0,v0,-4          ; If-Block-Stack poppen
80053974: lhu   v0,2(v1)          ; s16 Blockgroesse bei +2
8005397c: addu  v1,v1,v0
80053980: sw    v1,28(a0)         ; task->pc = pc + size   (RELATIV zum Opcode!)
```
Operand bei +2..+3 ⇒ **4**; Ziel-Orakel bestätigt (jeder andere Wert erzeugt Fehltreffer).

### 0x10 `ewhile` @0x80053E0C — **Länge 2** (aus den Daten, §3.2)
```
80053e0c: lb    a1,2(a0)          ; Nest
80053e2c: lw    v0,32(v1)         ; Schleifenanfang vom While-Stack
80053e34: sw    v0,28(a0)         ; task->pc = Schleifenanfang  (ABSOLUT)
80053e38: lbu   v0,8(a1)
80053e44: sb    v0,8(a1)          ; while-Zaehler--
```
Liest **kein** Operandenbyte.

### 0x13 `switch` @0x80054020 — **Länge 4** (Kopf), danach folgen die `case`-Einträge
```
80054038: lhu   t0,2(a3)          ; u16 Blockgroesse bei +2
8005403c: lbu   a2,1(a3)          ; Variablen-Index bei +1
80054040: addiu a3,a3,4           ; <-- Kopf ist 4 Byte
8005406c: addu  v0,a3,t0
80054070: sw    v0,0(v1)          ; Ausstiegsadresse = (pc+4) + size
; Case-Scan:
8005409c: lbu   v0,0(a3)
800540a4: bne   v0,t4,...         ; t4 = 21 = 0x15 (default) -> a3 += 2
800540b4: beq   v0,t3,...         ; t3 = 22 = 0x16 (eswitch) -> a3 += 2
800540bc: lhu   t0,2(a3)          ; case: Blockgroesse
800540c0: lh    v0,4(a3)          ; case: Vergleichswert
800540cc: addiu a3,a3,6           ; case-Kopf = 6 Byte
800540d4: addu  a3,a3,t0          ; nicht getroffen -> ueberspringen
```
Der Case-Scanner belegt nebenbei **0x14 = 6**, **0x15 = 2**, **0x16 = 2**
(`addiu t4,zero,21` @0x80054024, `addiu t3,zero,22` @0x80054028).

### 0x17 `goto` @0x8005415C — **Länge 6**
```
80054170: lbu   v1,1(a1)          ; Operand +1 (ifel-Zaehler zuruecksetzen)
80054174: lbu   a3,2(a1)          ; Operand +2 (loop-Zaehler)
80054178: lh    t0,4(a1)          ; s16 Sprungweite bei +4  => >= 6
80054190: addu  a1,a1,t0
800541a4: sw    a1,28(a0)         ; task->pc = pc + t0      (RELATIV)
```

### 0x18 `gosub` @0x800541A8 — **Länge 2 (direkt belegt)**
```
800541a8: lw    v1,28(a0)         ; v1 = pc
800541b0: lbu   a2,1(v1)          ; Sub-Nummer bei +1
800541b4: addiu v1,v1,2           ; <-- Ruecksprungadresse = pc + 2
800541c4: sw    v1,324(v0)        ;     auf den Gosub-Stack
800541f8: lhu   v0,0(a2)          ; sub_tab[a2]
80054204: sw    v0,28(a0)         ; task->pc = base + offset  (ABSOLUT)
```
Der Handler **schreibt selbst** `pc+2` als Rücksprungziel ⇒ die Instruktion ist 2 Byte lang.

### 0x19 `greturn` @0x80054210 — Länge nicht aus dem Handler ableitbar
```
80054210: lbu   v1,2(a0)
80054218: addiu v1,v1,-1
8005421c: sb    v1,2(a0)          ; Nest--
80054230: lw    v0,324(v0)
80054238: sw    v0,28(a0)         ; task->pc = gespeicherte Adresse (ABSOLUT)
```
Liest kein Operandenbyte.

### 0x1A `break` @0x80054268 — **Länge 2** (aus den Daten, §3.2)
```
80054268: lb    v0,2(a0)
80054280: lb    a2,8(a3)          ; loop-Zaehler
80054290: lw    v1,96(v1)         ; Ausstiegsadresse vom Loop-Stack
80054298: sw    v1,28(a0)         ; task->pc = Ausstieg  (ABSOLUT)
800542a4: sb    a2,8(a3)          ; Zaehler--
```
Liest kein Operandenbyte.

---

## 3. Der reparierte Walker und sein Nachweis

### 3.1 RDT-Layout

23 `uint32`-Offsets ab Datei `0x08`. **[16] = SCD_INIT, [17] = SCD_MAIN.**
Verifiziert an ROOM1120: `offs[17] = 0x2928`, Sub-Tabelle (u16, erstes Wort = Tabellenlänge
in Byte), `subs[25] = 0x10b4` → `0x2928 + 0x10b4 = 0x39DC` — exakt der Datei-Offset aus dem
früheren Lauf.

Sub-Grenzen: `subs[i+1] − subs[i]`. Der **letzte** Sub eines Blocks hat keine gespeicherte
Endadresse; dahinter liegt 0..3 Byte 4er-Alignment-Padding (die Padding-Bytes sind **nicht**
genullt — ROOM1020 INIT endet @0x18C2, danach `ed 47` bis zur SCD_MAIN-Basis 0x18C4) oder
direkt eine Sektion, die in der Offsettabelle gar nicht steht (ROOMA010 MAIN: Sub endet
@0x1804, danach eine 32-Bit-Datentabelle).

### 3.2 Ergebnis: 0 echte Desyncs (vorher 129)

| Lauf | Subs | Desync |
|---|---|---|
| naiv `while pc<e`, Ende = nächste Sektionsadresse | 2568 | **129** |
| + Alignment-Padding-Regel | 2568 | 95 |
| + „letzter Sub endet hinter dem letzten `evt_end`" | 2568 | **0** |

Aufgeschlüsselt: **2068 von 2068 nicht-letzten Subs enden auf das Byte genau** an ihrer
Sub-Grenze. Die 129 des früheren Laufs waren **alle** letzte Subs — der Fehler lag nicht bei
den neun Opcodes, sondern bei der Annahme, das Blockende stünde im RDT.

⛔ **Korrektur an der Auftragsprämisse:** die neun Opcode-Längen waren *nicht* falsch. Mit
0x01=2, 0x03=4, 0x07=4, 0x10=2, 0x13=4, 0x17=6, 0x18=2, 0x19=2, 0x1A=2 (den Werten, die der
frühere Lauf aus RE1.5 übernommen hatte) läuft der Walker sauber — sie stimmen für RE2
zufällig überein. Falsch war die *Sub-Ende-Regel*.

### 3.3 Das Block-Ziel-Orakel — unabhängiger Beweis der Längen

Jede Blockgröße im Bytecode ist **Daten**, jede Instruktionsgrenze ist **Ergebnis der
Längentabelle**. Wenn beide zusammenpassen, ist die Tabelle richtig. Geprüft für
`if_ck(0x06)`, `else_ck(0x07)`, `for(0x0D)`, `while(0x0F)`, `do(0x11)`, `switch(0x13)`,
`case(0x14)`, `goto(0x17)` über alle 250 RDTs:

> **6009 von 6009 Block-Zielen treffen exakt eine Instruktionsgrenze. 0 Fehltreffer.**

Sweep je Opcode (Anzahl Fehltreffer bei abweichender Länge):

| Opcode | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| 0x03 `evt_chain` | 0 | 0 | 315 | **0** | 6 | 8 |
| 0x07 `else_ck` | 9818 | 9818 | 706 | **0** | 19691 | 16897 |
| 0x13 `switch` | 1213 | 4006 | 3 | **0** | 354 | 354 |
| 0x17 `goto` | 13002 | 12706 | 9817 | 9817 | 11402 | **0** |
| 0x18 `gosub` | 16395 | **0** | 10879 | 6979 | 21926 | 21120 |
| 0x01 `evt_end` | 0 | **0** | 235404 | 235400 | 248821 | 248918 |
| 0x10 `ewhile` | 0 | **0** | 3709 | 3519 | 5075 | 3211 |
| 0x1A `break` | 0 | **0** | 1264 | 1265 | 3900 | 3911 |
| 0x19 `greturn` | 0 | **0** | 0 | 0 | 2 | 2 |

Damit sind 0x03/0x07/0x13/0x17/0x18 **eindeutig** festgenagelt.

Für 0x01/0x10/0x19/0x1A sind 1 und 2 nicht unterscheidbar — **weil das Folgebyte immer 0x00
ist** und 0x00 (`nop`) selbst 1 Byte lang ist (`@0x800537e4 lw v0,28(a0)` / `addiu v0,v0,1`).
Gemessen über alle Vorkommen:

| Opcode | Folgebyte 0x00 | Folgebyte ≠ 0x00 |
|---|---|---|
| 0x01 `evt_end` | 2632 | 2 (beide in Padding/Datenbereich hinter dem letzten Sub) |
| 0x10 `ewhile` | 199 | 0 |
| 0x19 `greturn` | 6 | 0 |
| 0x1A `break` | 1008 | 0 |

⇒ Länge **2** (Opcode + ein Null-Padbyte). Ausführungsseitig ist das nicht von „1 Byte + nop"
unterscheidbar; für den Walker ist es äquivalent.
**0x18 `gosub` kommt in den 250 Raumskripten 0×, 0x19 `greturn` nur 6× vor** — die
Gosub-Mechanik wird in RE2-Raumskripten praktisch nicht benutzt.

### 3.4 Werkzeuge

* `analysis/nutzer_batch_2026-08-27/tools/re2_scd_oplen.py` — Längen aus den Handlern
* `analysis/nutzer_batch_2026-08-27/tools/re2_scd_lens.py` — fertige Tabelle
* `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py` — Walker + Selbsttest
* `analysis/nutzer_batch_2026-08-27/tools/re2_find_window.py` — die Mustersuche aus §5

---

## 4. Die Bausteine der Szene - byte-true belegt

### 4.1 `Sce_em_set` (0x44, 22 Byte) - Feldbelegung aus dem Handler @0x8005714c

Der Handler sichert `task->pc` nach `DAT_800d5be8` (`@0x80057178 sw v0,23528(at)`) und liest
die Operanden von dort:

| Operand | Instruktion | Ziel im Entity |
|---|---|---|
| `u16 @+4` | `@0x8005734c lhu v0,4(v1)` -> `@0x80057354 sh v0,270(s0)` | **+0x10E** (ACTIVE-Dispatch-Feld) |
| `u8 @+9` | `@0x80057358 lbu v0,9(v1)` -> `@0x8005735c sb v0,462(s0)` | +0x1CE |
| `u8 @+7` | `@0x80057274 lbu v0,7(v0)` -> `@0x80057280 sb v0,506(s0)` | +0x1FA |
| `s16 @+10` | `@0x800572c8 lhu v0,10(v1)` -> `sh v0,68(s0)` / `sw v0,56(s0)` | **X** (+0x44 / +0x38) |
| `s16 @+12` | `@0x800572e0 lhu v0,12(v1)` -> `sh v0,70(s0)` / `sw v0,60(s0)` | **Y** (+0x46 / +0x3C) |
| `s16 @+14` | `@0x800572f8 lhu v0,14(v1)` -> `sh v0,72(s0)` / `sw v0,64(s0)` | **Z** (+0x48 / +0x40) |
| `s16 @+10/+14` | `@0x80057310 / @0x8005731c` -> `sh v0,280(s0)` / `sh v0,282(s0)` | Ziel-X/Z (+0x118/+0x11A) |
| `s16 @+16` | `@0x80057328 lhu v0,16(v1)` -> `@0x80057330 sh v0,118(s0)` | Blickrichtung (+0x76) |
| `u8 @+3` | `@0x80057334 lbu v0,3(v1)` | EM-Typ |

=> Das **Flag-Feld bei +4 IST der +0x10E-Startwert.** (Gegenprobe aus den Daten: ROOM1000
spawnt mit `flag=0x1046` und schreibt spaeter `member_set(7, 0x1046)` - derselbe Wert.)

### 4.2 `Work_set` (0x2E, 3 Byte) @0x80055904

```
80055920: lbu   v1,1(v0)          ; kind
80055924: lb    a1,2(v0)          ; index
80055928: addiu v0,v0,3
80055930: addiu v1,v1,-1
80055934: sltiu v0,v1,0x5         ; kind 1..5
8005594c: lw    v0,4592(at)       ; Sprungtabelle @0x800111f0
; kind=3 -> @0x8005597c:
80055988: lw    v0,-484(at)       ; v0 = *(u32*)(0x800cfe1c + idx*4)   = EM-Array
80055990: sw    v0,340(a0)        ; task->work = v0
```
`kind=1` -> `0x800cfe14`, `kind=2` -> `0x800cfe18`, `kind=3` -> `0x800cfe1c + idx*4`
(**Gegner**), `kind=4` -> Objekte (Obj_model_set).

### 4.3 Member-Tabellen (Setter @0x80011228, Getter @0x800112f8, je 44 Eintraege)

```
80055cb0: sltiu v0,a1,0x2c        ; member < 44
80055cc4: lw    v0,4648(at)       ; Tabelle 0x80011228
```

| Member | Setter-Instruktion | Entity-Offset |
|---|---|---|
| m0 | `@0x80055cd8 sh a2,0(a0)` | +0x00 (be_flg, low) |
| **m1** | `@0x80055ce0 sh a2,2(a0)` | **+0x02 (be_flg, high)** |
| m2..m6 | `sb a2,4/5/6/7/8(a0)` | +0x04 .. +0x08 |
| **m7** | `@0x80055d10 sh a2,270(a0)` | **+0x10E** |

### 4.4 `Calc` (0x26, 6 Byte) @0x8005458c

```
8005459c: lhu   v1,2(v0)          ; low byte = Operator, high byte = Variablen-Index
800545a0: lh    a2,4(v0)          ; s16 Operand
800545a4: addiu v0,v0,6
800545b4: srl   a1,v1,8           ; var
800545bc: andi  a0,v1,0xff        ; op
80054648: lw    v0,4544(at)       ; Operator-Tabelle @0x800111c0, 12 Eintraege
```
Operatoren: 0 `+` (@0x80054658), 1 `-` (@0x80054664), 2 `*`, 3 `/`, 4 `%`,
**5 ODER (@0x80054700 `or v0,v0,a2`)**, **6 UND (@0x8005470c `and v0,v0,a2`)**,
7 XOR (@0x80054718), 8 NOR (@0x80054724), 9 Linksshift (@0x80054730), 10, 11.

---

## 5. Die Fenster-Szene: **ROOM1120** - und nur ROOM1120

### 5.1 Systematische Suche ueber alle 250 RE2-Raumskripte

Kriterien: (a) mindestens 3 Gegner werden per `pos_set` auf **dieselbe** x- oder z-Linie
gesetzt, (b) es gibt `Obj_model_set` in der Naehe derselben Linie, (c) lange schmale `aot_set`.

> **Genau ein Treffer: `ROOM1120.RDT`, Linie x = -16360.**

Die weichere Suche (mindestens 3 `Sce_em_set` mit identischer Spawn-Koordinate) liefert
7 Subs - davon zeigen nur **zwei** das "off-map geparkt + gestaffelt"-Muster:

| Raum | Sub | Datei-Offset | EM-Typ | +0x10E-Flag | Spawn-Position |
|---|---|---|---|---|---|
| **ROOM1120** | MAIN sub25 | **0x039DC** | 4x 0x1F | 0x400A | x=+32000, z=32000/31000/30000/29000 |
| **ROOM1010** | MAIN sub4 | **0x01A94** | 4x 0x1F | 0x0040 | x=-32000, z=-32000/-31000/-30000/-29000 |

Die uebrigen fuenf liegen auf realen Raumkoordinaten und haben weder Obj_model_set noch das
Nachpositionier-Muster:

| Raum | Sub | Datei-Offset | EM-Typ | Flag | Linie |
|---|---|---|---|---|---|
| ROOM1090 | MAIN sub9 | 0x02722 | 4x 0x21 | 0x4002 | z=-15500, x=-10300/-9800/-6900/-2900 |
| ROOM1090 | MAIN sub10 | 0x027A8 | 3x 0x21 | 0x0000 | z=-14064, x=-9660/-6360/-2860 |
| ROOM2050 | MAIN sub0 | 0x017C2 | 5x 0x2D | 0x0000/0x0001/0x0010 | z=-7000 |
| ROOM6130 | MAIN sub0 | 0x00E98 | 4x 0x3B | 0x2008 | z=-17000 |
| ROOMC070 | INIT sub0 | 0x01062 | 3x 0x26 | 0x2002 | x=-3100 |

### 5.2 ROOM1120 MAIN sub25 @Datei 0x039DC - der Aufbau (vollstaendig, 304 Byte)

```
+0    0x039DC  aot_set_4p    67 0a 05 41 00 00 | 1a c6 12 94 ae a5 26 b2 42 e9 3e e0 36 d2 12 94 | ff 00 18 20 00 00
                             aot=10 sce=5 sat=0x41 floor=0
+28   0x039F8  obj_model_set 2d 0d ... idx=13   Pos (x=-18080, y=-32000, z=-14486)
+66   0x03A1E  obj_model_set 2d 0e ... idx=14   Pos (x=-18080, y=-32000, z= -9036)
+104  0x03A44  obj_model_set 2d 0f ... idx=15   Pos (x=-18080, y=-32000, z= -3486)
+142  0x03A6A  sce_em_set    44 00 00 1f 0a 40 00 00 03 ff | 00 7d 00 00 00 7d | 00 00 00 00 00 00
+164  0x03A80  sce_em_set    44 00 01 1f 0a 40 00 00 00 ff | 00 7d 00 00 18 79 | ...
+186  0x03A96  sce_em_set    44 00 02 1f 0a 40 00 00 03 ff | 00 7d 00 00 30 75 | ...
+208  0x03AAC  sce_em_set    44 00 03 1f 0a 40 00 00 01 ff | 00 7d 00 00 48 71 | ...
+230  0x03AC2  work_set 2e 03 00 | member_copy 3d 10 01 | calc 26 00 05 10 08 00 | member_set2 35 01 10
+248  0x03AD4  work_set 2e 03 01 | ... dieselbe Folge ...
+266  0x03AE6  work_set 2e 03 02 | ...
+284  0x03AF8  work_set 2e 03 03 | ...
+302  0x03B0A  evt_end 01 00
```

**Die vier Bloecke setzen NICHT +0x10E, sondern Bit 3 im be_flg-High-Halbwort.**
`member_copy(var0x10 <- m1)` / `calc(var0x10 |= 8)` / `member_set2(m1 <- var0x10)`
= `*(u16*)(entity+0x02) |= 0x0008` = **be_flg-Bit 0x00080000** fuer die Gegner-Slots 0..3.
Der Gegenwert `calc op6 (UND) mit -9 = 0xFFF7` kommt in denselben Skripten vor
(ROOM1120 sub53 @0x047D2 fuer die Objekte 5 und 11) - das Bit ist ein
**Sichtbarkeits-/Aktiv-Schalter**, kein Zustandswert.

**Die vier Zombies landen auf +0x10E = 0x400A** - das ist der Wert aus `sce_em_set` Byte +4
(`0a 40`, siehe 4.1); sub25 fasst +0x10E nicht an. `0x400A & 0x3F = 0x0A` (gerade) => im
ACTIVE-Dispatcher der Zweig "aufrecht" (`0x8010118C`, siehe Schwester-Dossier
`re2-fenstergreifer-active.md`). Erst sub53 schreibt +0x10E: `|= 0x8000` ergibt 0xC00A.

### 5.3 Wohin die vier Zombies wirklich laufen - die Nachbar-Subs

Die Spawn-Position (32000, 0, 32000..29000) ist eine **Parkposition weit ausserhalb der
Raumgeometrie**. Die Inszenierung passiert in diesen Subs:

| Sub | Datei-Offset | Was passiert |
|---|---|---|
| **sub30** | 0x03DEA..0x03E14 | alle vier auf die **Wandlinie x = -16360**: em0 z=-8700, em1 z=-14238, em2 z=-18450, em3 z=-9600 |
| **sub34** | 0x040BC / 0x040CE | `work_set(4,5)` und `work_set(4,11)`: Member1 ODER 8 - zwei Objekte einblenden |
| **sub35** | 0x04114 | em0 -> (-18200, **180**, -12936) |
| **sub36** | 0x04130 | em1 -> (-18200, **180**, -8136) |
| **sub37** | 0x0414C | em2 -> (-18380, **360**, -2586) |
| **sub38** | 0x04168 | em3 -> (-18380, **360**, -13836) |
| **sub40** | 0x04214 | em0 -> (-16360, 0, -9700) |
| **sub41** | 0x04230 | em1 -> (-16360, 0, -14238) |
| **sub42** | 0x0424C | em2 -> (-16180, 180, -19500) |
| **sub43** | 0x04268 | em3 -> (-16000, 180, -8900) |
| **sub53** | 0x0475A..0x047F4 | Abbau (siehe 5.4) |

**Das ist die gesuchte Mechanik:**
* x = **-18080** - die drei `Obj_model_set` (idx 13/14/15), zunaechst auf **y = -32000**
  geparkt (unsichtbar unter der Welt), z = -14486 / -9036 / -3486
  => **drei Fenster/Gitter auf einer Wandlinie**.
* x = **-18200 / -18380** und **y = 180 / 360** - die Zombies stehen 120..300 Einheiten
  **hinter** den Fenstermodellen und **erhoeht**; die y-Anhebung ist das
  "durch die Oeffnung lehnen".
* x = **-16360 .. -16000** - dieselben Zombies auf der **Korridorseite**.
* `aot_set` **aot=14** in sub2 @0x02A9A: `x=-17245 z=-12445 w=12100 d=1800` und
  **aot=17** @0x02AD6: `x=-20355 z=-12040 w=7700 d=1800` - die langen schmalen Rechtecke
  des Flurs vor der Fensterwand (sce=4, sat=0x31, floor=0 bzw. 3).
* `aot_set_4p` **aot=10** in sub25 (sce=5, sat=0x41) ist der Vierpunkt-Trigger, den sub53
  zusammen mit dem Effekt wieder abraeumt.

### 5.4 sub53 im Klartext - der Beweis, dass sub25/30/34-43/53 EINE Szene sind

```
0x0475A  work_set    2e 01 00                  ; kind=1 (Spieler-Work)
0x0475E  member_set  34 2b 00 00               ; m43 = 0  -> sb a2,467(a0)  (+0x1D3)
0x04762  work_set    2e 03 00                  ; Gegner 0
0x04766  pos_set     32 00 00 7d 00 00 00 7d   ; (32000, 0, 32000)
0x0476E  member_copy 3d 10 07                  ; var0x10 <- +0x10E
0x04772  calc        26 00 05 10 00 80         ; var0x10 ODER 0x8000
0x04778  member_set2 35 07 10                  ; +0x10E = var0x10   -> 0xC00A
   ... identisch fuer Gegner 1 (z=31000), 2 (z=30000), 3 (z=29000) ...
0x047CA  work_set 2e 04 05 | member_copy 3d 10 01 | calc 26 00 06 10 f7 ff | member_set2 35 01 10
0x047DC  work_set 2e 04 0b | ... dieselbe Folge (Objekte 5 und 11 ausblenden)
0x047EE  sce_espr_kill 4c 04 0a 00 00
0x047F4  evt_end 01 00
```
Dieselben vier Gegner-Slots, dieselben vier Parkkoordinaten wie in sub25 - sub25 baut auf,
sub53 raeumt ab.

---

## 6. Was NICHT belegt ist

* **EM-Typ 0x1F ist kein Sonder-"Fenstergreifer".** Typ 0x1F kommt in 35 Raeumen als ganz
  gewoehnlicher Zombie vor (z.B. ROOM1000 INIT @0x011EA..0x012E0, ROOM1040 INIT
  @0x00CB8..0x00DAE, ROOM1130 MAIN sub5 @0x011EA..0x012C4). RE2 inszeniert die Fensterszene
  **nicht** ueber einen eigenen Gegnertyp, sondern ueber Positionierung (x hinter der Wand,
  y angehoben) plus ein-/ausgeblendete Fenster-Objektmodelle. Ein
  "Haende-durch-das-Gitter"-Gegnertyp existiert in den Raumskripten **nicht**.
* **Bedeutung von be_flg-Bit 0x00080000 (Member 1, Bit 3):** aus dem Paarungsmuster
  ODER-8 / UND-0xFFF7 (163 Fundstellen game-weit) als Sichtbarkeits-/Aktivschalter belegt;
  die *lesende* Stelle in der Engine ist in dieser Sitzung **NICHT GEFUNDEN**. Naechster Weg:
  EXE-Scan nach `lhu rX,2(rY)` + `andi rZ,rX,0x8` bzw. `lw rX,0(rY)` + Test gegen
  0x00080000 im Render- und Kollisionspfad.
* **Bedeutung der +0x10E-Bits oberhalb von 0x3F** (0x4000 in ROOM1120, 0x0040 in ROOM1010,
  0x8000 als "aus" in sub53): **NICHT GEFUNDEN**. Naechster Weg: Xrefs auf +0x10E in
  `EMZ0.BIN` und in der EXE ausserhalb des ACTIVE-Dispatchers @0x8010114C.
* **`aot_set` (0x2C) Feldbelegung** (x/z/w/d bei +6/+8/+10/+12): nach dem
  Standard-RE2-Layout gelesen, **nicht** aus dem Handler @0x80054af4 verifiziert. Die
  Geometrie ist konsistent, der Handler-Beweis fehlt.
* **`aot_set_4p` (0x67) Punktbelegung**: die acht s16 ab +6 wurden **nicht** aus dem Handler
  @0x80054b60 verifiziert; die vier Punkte sind oben nur roh zitiert.
* **`Obj_model_set` (0x2D) Positionsfelder bei +14/+16/+18**: aus dem Datenmuster
  erschlossen (ROOM1120 idx 0..12 liefern plausible Raumkoordinaten), **nicht** aus dem
  Handler @0x80055260 verifiziert.
* **Opcode 0x8E** (24 Byte, 340 Vorkommen) hat denselben Prolog wie `sce_em_set` (0x44) und
  schreibt ebenfalls `task->pc` nach `DAT_800d5be8` (@0x80057740) - die Feldbelegung wurde
  **nicht** auseinandergenommen.
* **Laenge von 0x19 `greturn`**: nur 6 Vorkommen game-weit, alle mit Folgebyte 0x00; 1 und 2
  sind aus den Daten nicht unterscheidbar, und der Handler @0x80054210 liest keinen
  Operanden. Fuer den Walker irrelevant, weil 0x18 `gosub` null mal vorkommt.
