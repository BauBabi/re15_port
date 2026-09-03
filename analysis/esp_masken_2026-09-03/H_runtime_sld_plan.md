# AUFGABE H — Vordergrundatlas zur LAUFZEIT aus dem BSS (Loesungsentwurf)

Datum 2026-09-03 · Stand: **Entwurf, NICHT implementiert** · Repo `c:/workspace/git/reAi_v2`

Alle Behauptungen unten tragen entweder eine Disassembly-Adresse (`@0x8…`) oder einen
Datei-Byte-Offset. Was nicht belegt ist, steht ausdruecklich als **NICHT BELEGT** da.

---

## 0. Kurzfassung des Befunds (das aendert den Entwurf gegenueber der Aufgabenstellung)

Die Aufgabe stellte in Aussicht, dass es evtl. *keine* deterministische Regel gibt und ein Scan
noetig waere. **Es gibt eine, und sie ist vollstaendig belegt.** Das Original scannt NICHT:

1. Der Cut-Wechsel `FUN_80021bbc` liest aus einer **Stage-Tabelle** eine `u16` `e` und behandelt sie
   als **Byte-Laenge des vom CD geladenen Teils des 64-KB-Chunks**.
2. Die **letzten 8 Bytes dieser Laenge** sind der Trailer: `u32 sld_offset` @`chunk[e-8]`,
   `u32 sld_present` @`chunk[e-4]`.
3. Ist `sld_present == 0`, laedt das Original **keinen** Vordergrund. Sonst dekomprimiert
   `FUN_800c47e8` den Block bei `chunk[sld_offset]` und `FUN_8004ee78` laedt den TIM ins VRAM.

Gemessen ueber **alle 6 Stages / 120 BSS-Dateien / 1119 Cuts**: **359 Cuts** haben einen
Vordergrund, **760** nicht, **0 Fehlschlaege** (jeder der 359 Bloecke dekomprimiert zu einem
gueltigen Sony-TIM). Der bisherige heuristische Scan der Java-/Python-Werkzeuge
**ueberdetektiert** (Beleg §2.5) — er findet Alt-Datenreste jenseits von `e`, die das Original nie
in den RAM holt.

Zusaetzlich gemessen: von den 359 echten Vordergrund-Cuts haben nur **209** eine vorextrahierte
`PRI##.TIM`; **151 fehlen** (u.a. ROOM1020/1030/1040/1070 **und STAGE5 + STAGE6 komplett**).
Der Laufzeit-Decoder schliesst diese Luecke ohne neue Asset-Dateien.

---

## 1. Die Original-Kette, Instruktion fuer Instruktion

### 1.1 `FUN_80021bbc` — Kamera-Cut anwenden (der einzige Aufrufer von `FUN_800c47e8`)

Decompilat: `RE_15_Quellcode_V2/FUN_80021bbc.c`. Roh-Disasm aus `ghidra1_V2.txt`
(Zeilen 101700–101860). Die relevanten Instruktionen:

```
; --- Cut-Index setzen ---
80021be0  0b 80 03 3c / b5 fb 63 90   lbu  v1, DAT_800afbb5        ; neuer Cut
80021bfc  0b 80 01 3c / e4 0f 23 a4   sh   v1, DAT_800b0fe4        ; aktiver Cut

; --- Tabellenzeiger (wird vom Stage-Overlay gesetzt, s. §2.1) ---
80021c3c  0b 80 11 3c                 lui  s1,0x800b
80021c40  c8 52 31 8e                 lw   s1, DAT_800b52c8        ; s1 = BSS-Cut-Tabelle

; --- Index-Rechnung:  e = *(u16*)( s1 + fe2*0x20 + fe4*2 ) ---
80021d38  0b 80 10 3c / e2 0f 10 26   addiu s0, 0x800b0fe2
80021d44  e4 0f 63 84                 lh   v1, DAT_800b0fe4        ; cut
80021d48  00 00 02 86                 lh   v0, 0(s0) = DAT_800b0fe2 ; room
80021d4c  40 18 03 00                 sll  v1,v1,0x1               ; cut*2
80021d50  40 11 02 00                 sll  v0,v0,0x5               ; room*0x20
80021d54  21 10 51 00                 addu v0,v0,s1
80021d58  21 18 62 00                 addu v1,v1,v0
80021d5c  00 00 62 94                 lhu  v0,0(v1)                ; v0 = e

; --- WACHE: Vordergrund vorhanden?  *(u32*)(0x80190000 + e - 4) ---
80021d64  19 80 01 3c                 lui  at,0x8019               ; 0x80190000 = BSS-Chunk-Puffer
80021d68  21 08 41 00                 addu at,v0,at
80021d6c  fc ff 22 8c                 lw   v0,-0x4(at)             ; = chunk[e-4]
80021d74  20 00 40 10                 beq  v0,zero,LAB_80021df8    ; ==0  ->  KEIN Vordergrund

; --- Offset holen und dekomprimieren ---
80021d98  00 00 42 94                 lhu  v0,0(v0)                ; e (neu berechnet)
80021d9c  19 80 11 3c                 lui  s1,0x8019               ; s1 = 0x80190000
80021da0  21 08 51 00                 addu at,v0,s1
80021da4  f8 ff 30 8c                 lw   s0,-0x8(at)             ; s0 = chunk[e-8] = sld_offset
80021da8  1a 80 06 3c                 lui  a2,0x801a
80021dac  00 58 c6 24                 addiu a2,a2,0x5800           ; a2 = 0x801A5800 = ZIEL
80021db0  21 20 30 02                 addu a0,s1,s0                ; a0 = chunk + sld_offset
80021db4  00 00 85 8c                 lw   a1,0x0(a0)              ; a1 = LE32 entpackte Groesse
80021db8  fa 11 03 0c                 jal  FUN_800c47e8
80021dbc  04 00 84 24                 _addiu a0,a0,0x4             ; a0 = Stream (hinter dem Wort)

; --- TIM ins VRAM ---
80021dd4  1a 80 04 3c / 00 58 84 34   ori  a0, 0x801A5800
80021ddc  0b 80 10 3c / 4c ca 10 26   addiu s0, 0x800aca4c
80021de4  00 00 11 96                 lhu  s1,0(s0)                ; VRAM-Slot-Allokator sichern
80021de8  15 00 02 34                 ori  v0,zero,0x15            ; <<< Slot 0x15 erzwingen
80021dec  9e 3b 01 0c                 jal  FUN_8004ee78
80021df0  00 00 02 a6                 _sh  v0,0(s0)
80021df4  00 00 11 a6                 sh   s1,0(s0)                ; Allokator zuruecksetzen
```

Signatur damit belegt: **`FUN_800c47e8(u8 *src_stream, s32 size, u8 *dst)`**
(`a0`=Stream hinter dem Groessenwort, `a1`=entpackte Groesse, `a2`=Ziel).

Dieselbe Sequenz noch einmal im Decompilat (`FUN_80021bbc.c`, Ghidra hat die `s1`-Basis verloren
und schreibt die Adressen absolut aus):

```c
if (*(int *)(&DAT_8018fffc + *(ushort *)((short)DAT_800b0fe4 * 2 + _DAT_800b0fe2 * 0x20)) != 0) {
    FUN_800c47e8(&UNK_80190004 + *(int *)(&DAT_8018fff8 + e),
                 *(undefined4 *)(&DAT_80190000 + *(int *)(&DAT_8018fff8 + e)),
                 &DAT_801a5800);
    uVar1 = _DAT_800aca4c;  _DAT_800aca4c = 0x15;
    FUN_8004ee78(&DAT_801a5800);
    _DAT_800aca4c = uVar1;
}
```
(`&DAT_8018fffc` = `0x80190000 - 4`, `&DAT_8018fff8` = `0x80190000 - 8`.)

### 1.2 `e` ist eine BYTE-LAENGE, kein Zeiger — Beleg

`e` wird vor dem CD-Read in `DAT_800be574` abgelegt (`@0x80021cb8: sw v1,-0x1a8c(at)`) und die
Streaming-Routine behandelt es als Byte-Zaehler mit Teil-Sektor-Behandlung:

```
FUN_80013f80 (vor dem Read):
80013f90  00 00 83 8c   lw   v1, DAT_800be574
80013f98  ff 07 62 30   andi v0,v1,0x7ff          ; Rest modulo 2048 (Sektorgroesse)
80013f9c  0b 00 40 10   beq  v0,zero,...          ; genau sektor-aligned -> nichts zu retten
80013fa4  24 28 65 00   and  a1,v1,-0x800         ; sektor-aligned Teil
80013fb0  80 e5 42 8c   lw   v0, DAT_800be580     ; Zielpuffer
80013fc4  2c 41 00 0c   jal  FUN_800104b0         ; 0x800 Bytes des letzten Sektors SICHERN

FUN_80013fdc (nach dem Read):
80013fec  00 00 a2 8c   lw   v0, DAT_800be574
80013ff4  ff 07 42 30   andi v0,v0,0x7ff
80014010  23 30 c2 00   subu a2,a2,v0             ; 0x800 - rest
80014014  8e 3b 01 0c   jal  FUN_8004ee38         ; Bytes AB e im letzten Sektor ZURUECKSCHREIBEN
```

Folge: **nur `chunk[0 .. e)` gelangt jemals in den RAM**. Alles ab `e` wird aus der Sicherung
wiederhergestellt (= Inhalt des vorigen Cuts) und ist auf der Disc Fuellmaterial/Altbestand.
Genau daran scheitert der heuristische Scan (§2.5).

### 1.3 `FUN_800c47e8` — der SLD-Depacker liegt in **DEBUG.BIN**, nicht in der EXE

`info/Re1.5/PSX.EXE` Header: `t_addr = 0x80010000`, `t_size = 0x000AF000`
→ Textende `0x800BF000`. `0x800C47E8` liegt **ausserhalb** der EXE; im Ghidra-Dump steht dort `??`.

Gefunden: **`info/Re1.5/PSX/BIN/DEBUG.BIN`, Datei-Offset `0x47E8`** (Datei ist `0x40000` = 256 KB
gross, laedt @`0x800C0000`, endet exakt an der Overlay-Basis `0x80100000`).
Verifikation gegen den Savestate `stage_saves/lamp_1170_stage1.sav`:
254956/262144 Bytes identisch (97,26 %; die Differenzen sind Laufzeit-Daten im Modul),
und der **Codebereich `0x47E8..0x4908` ist byte-identisch**.

Vollstaendige Disassembly + Hex-Dump: `analysis/esp_masken_2026-09-03/_debugbin_800c47e8.txt`.
Kern (67 Instruktionen, `0x110` Bytes, `jr ra` @`0x800C48F0`):

```
800c47e8  00005821  addu  t3,zero,zero      ; sp   = 0   (Quell-Index)
800c47ec  00006021  addu  t4,zero,zero      ; mask = 0   (Flag-Maske)
800c47f0  18a0003f  blez  a1,0x800c48f0     ; size <= 0 (SIGNED) -> sofort zurueck
800c47f4  00004021  addu  t0,zero,zero      ; dp   = 0   (Ziel-Index)
800c47f8  15800005  bne   t4,zero,0x800c4810
800c4800  340c0080  ori   t4,zero,0x80      ; mask = 0x80
800c4804  904d0000  lbu   t5,0(v0)          ; flagByte = src[sp++]
800c4810  90470000  lbu   a3,0(v0)          ; a = src[sp++]
800c4818  30e20080  andi  v0,a3,0x80
800c481c  14400005  bne   v0,zero,0x800c4834
800c4828  a0470000  sb    a3,0(v0)          ; a < 0x80  -> ROH-Literal, KEIN Flag-Bit verbraucht
800c482c  08031239  j     0x800c48e4        ;             (springt an srl UND andi vorbei)
800c4834  01ac1024  and   v0,t5,t4          ; (flagByte & mask)
800c4838  14400006  bne   v0,zero,0x800c4854
800c4844  a0470000  sb    a3,0(v0)          ; Bit 0 -> Literal >=0x80, Flag-Bit verbraucht
800c4850  000c6042  srl   t4,t4,1
800c4858  90470000  lbu   a3,0(v0)          ; b2 = src[sp++]
800c483c  00071e00  sll   v1,a3,24          ; a<<24   (Delay-Slot von 0x800c4838)
800c4860  00071400  sll   v0,a3,16          ; b2<<16
800c4864  00431025  or    v0,v0,v1
800c4868  00025503  sra   t2,v0,20          ; off12 = SIGN-EXTEND12((a<<4)|(b2>>4))
800c486c  30e2000f  andi  v0,a3,0xf
800c4870  1440000c  bne   v0,zero,0x800c48a4
800c48a4  24490002  addiu t1,v0,2           ; KURZ:  len = (b2&0x0f)+2, off = off12
800c487c  90470000  lbu   a3,0(v0)          ; LANG:  b3 = src[sp++]
800c4884  30e3003f  andi  v1,a3,0x3f
800c4888  24690003  addiu t1,v1,3           ;        len = (b3&0x3f)+3
800c488c  00071182  srl   v0,a3,6
800c4890  000a1880  sll   v1,t2,2
800c4894  00431025  or    v0,v0,v1
800c4898  00021400  sll   v0,v0,16
800c48a0  00025403  sra   t2,v0,16          ;        off = SIGN-EXTEND16((off12<<2)|(b3>>6))
800c48a8  000c6042  srl   t4,t4,1           ; Flag-Bit verbraucht
800c48ac  010a1021  addu  v0,t0,t2          ; base = dp + off   (off ist IMMER negativ, s.u.)
800c48b0  1120000a  beq   t1,zero,0x800c48dc
800c48c4  90e20000  lbu   v0,0(a3)          ; Kopierschleife, BYTEWEISE vorwaerts (Ueberlappung ok)
800c48d0  a1020000  sb    v0,0(t0)
800c48d4  1460fffb  bne   v1,zero,0x800c48c4
800c48dc  01404021  addu  t0,t2,zero        ; dp += len
800c48e0  318cffff  andi  t4,t4,0xffff
800c48e4  0105102a  slt   v0,t0,a1          ; SIGNED-Vergleich dp < size
800c48e8  1440ffc3  bne   v0,zero,0x800c47f8
800c48f0  03e00008  jr    ra
```

Vier Details, die ein naiver Port falsch macht:

| Detail | Beleg |
|---|---|
| Der Roh-Literal-Zweig (`a < 0x80`) verbraucht **kein** Flag-Bit | `j 0x800c48e4` @`0x800c482c` springt an `srl` @`0x800c4850`/`0x800c48a8` **und** `andi` @`0x800c48e0` vorbei |
| `off` ist **immer negativ**: Bit 11 von `off12` ist Bit 7 von `a`, und dieser Zweig wird nur bei `a >= 0x80` erreicht | `@0x800c4818` (`andi 0x80`) + `@0x800c4868` (`sra`, nicht `srl`) |
| Lange Form: `sll v0,v0,16 ; sra t2,v0,16` — 16-Bit-Sign-Extend. Wertebereich `[-8192,-1]`, passt in 16 Bit ⇒ mathematisch identisch zur 32-Bit-Rechnung `(off12<<2)|(b3>>6)` | `@0x800c4898`/`@0x800c48a0`; empirisch: max. Rueckdistanz ueber alle 359 Bloecke = **8191** |
| Abbruch ist ein **do-while** mit `blez`-Vorwache, `slt` = **signed** | `@0x800c47f0`, `@0x800c48e4` |

Die vorhandenen Ports `src/main/java/de/re15/extractors/bss/SldDecoder.java` und
`analysis/esp_masken_2026-09-03/sld.py` sind — Zeile fuer Zeile gegen obige Instruktionen geprueft —
**funktional aequivalent** (die `while`-statt-`do-while`-Form ist bei `size > 0` identisch).

### 1.4 `FUN_8004ee78` — VRAM-Ablage, byte-true nachgerechnet und **gegen echtes VRAM verifiziert**

`RE_15_Quellcode_V2/FUN_8004ee78.c`:

```c
OpenTIM(param_1); pTVar1 = ReadTIM(&TStack_28);
sVar4 = (ushort)DAT_800aca4c * 0x40;              // Slot * 64
if (0xf < DAT_800aca4c) sVar4 += -0x400;          // Slot >= 16: -1024
pTVar1->prect->x = sVar4;
pTVar1->prect->y = (DAT_800aca4c < 0x10 ^ 1) << 8;// Slot >= 16: y = 256
LoadImage(prect, paddr);
DAT_800aca4c += (prect->w + 0x3f) >> 6;
if (caddr) { crect->y = DAT_800aca4d + 0x1e0; LoadImage(crect, caddr); }
```

Mit dem in §1.1 erzwungenen `DAT_800aca4c = 0x15` (`@0x80021de8`):
`x = 21*64 - 1024 = 320`, `y = 256`; CLUT-`y = 0 + 0x1E0 = 480`.
(Der Allokator-Ausgangswert ist uebrigens `0x0a05`, gesetzt beim Raum-Laden
`@0x8003974c: ori v0,zero,0xa05` / `@0x80039754: sh v0,-0x35b4(at)`.)

**Gegenprobe am echten Geraet** (Savestate `stage_saves/lamp_1170_run3.sav`,
`DAT_800b0fe0/fe2/fe4 = 0/23/1` = STAGE1 / ROOM117 / Cut 1):
SLD aus `STAGE1/ROOM117.BSS` Chunk 1 (`e = 0x5EE4`, `sld_offset = 0x4FCC`) entpackt →
TIM `0x10220` B, `flags = 0x09` (8 bpp + CLUT), Bild `128x256` Halbworte (= 256x256 Pixel),
CLUT `256x1` @(0,480).
- VRAM (320,256) … (447,511): **32768 / 32768 Halbworte identisch** (0 Abweichungen).
- CLUT @(0,480): **256 / 256 identisch**.

Damit ist die Kette Disc-Bytes → Trailer → `FUN_800c47e8` → VRAM **lueckenlos byte-true belegt**.

---

## 2. Lokalisierung des Blocks — die deterministische Regel

### 2.1 Die Stage-Tabelle (`DAT_800b52c8`) — Adressen mit Instruktionsbeleg

Der Zeiger wird ausschliesslich vom Stage-Overlay gesetzt. Disassembliert direkt aus den
`STAGE*.BIN` (RAW @`0x80100000`, **kein** 0x800-Header):

| Stage | Instruktionen (Adresse / Wort) | Tabelle | Datei-Offset in `BIN/STAGE<n>.BIN` |
|---|---|---|---|
| 1 | `8011e970 3c028012 lui v0,0x8012` · `8011e974 2442eae4 addiu v0,v0,-0x151c` · `8011e97c ac2252c8 sw v0,0x52c8(at)` | `0x8011EAE4` | `0x1EAE4` |
| 2 | `80116fc8 3c028011` · `80116fcc 2442713c` · `80116fd4 ac2252c8` | `0x8011713C` | `0x1713C` |
| 3 | `8011cf94 3c028012` · `8011cf98 2442d0f8` · `8011cfa0 ac2252c8` | `0x8011D0F8` | `0x1D0F8` |
| 4 | `8011844c 3c028012` · `80118450 24428590` · `80118458 ac2252c8` | `0x80118590` | `0x18590` |
| 5 | `8011ddbc 3c028012` · `8011ddc0 2442df18` · `8011ddc8 ac2252c8` | `0x8011DF18` | `0x1DF18` |
| 6 | `80101e00 3c028010` · `80101e04 24421e3c` · `80101e0c ac2252c8` | `0x80101E3C` | `0x01E3C` |

(Decompilat-Gegenprobe: `RE_15_Quellcode_Overlays/STAGE1/FUN_8011e064.c:28`
`_DAT_800b52c8 = &DAT_8011eae4;` — analog STAGE2..6. Die Funktionsadressen der
`*_full`-Decompilate liegen um `0x800` verschoben, die **Daten**-Adresse ist in beiden gleich und
stimmt mit dem Roh-Disasm ueberein.)

**Layout:** `u16 tbl[room][16]`, Zeilen-Stride `0x20` (aus `sll v0,v0,5` @`0x80021d50`),
Spalte = Cut (`sll v1,v1,1` @`0x80021d4c`).

### 2.2 Zeilenindex = Raum, Spaltenindex = Cut — verifiziert ueber die Chunk-Zahlen

`DAT_800b0fe0` = Stage, `DAT_800b0fe2` = Raum, beide aus dem Tuer-Record
(`RE_15_Quellcode_V2/FUN_8001d600.c`: `DAT_800b0fe0 = *(byte*)(DAT_800ac9a8+4)`,
`_DAT_800b0fe2 = *(byte*)((int)DAT_800ac9a8+9)`).
Savestate-Gegenprobe: `lamp_1170_run3.sav` → `fe0=0, fe2=23 (0x17)` bei geladenem ROOM1170.

⇒ Port-Abbildung: `stage = (room_id>>12)&0xF`, `row = (room_id>>4)&0xFF`, Datei
`STAGE<stage>/ROOM<stage><row:02X>.BSS`.

Beleg fuer die Zeilenzuordnung: fuer **alle 16 Zeilen von STAGE1** stimmt die Zahl der Eintraege
`!= 0x0008` exakt mit der Chunk-Zahl der zugehoerigen `.BSS` ueberein:

```
Zeile  Tabelle (16 u16)                                                   Datei        Chunks
 0     53bc 607c 78a8 74a8 5564 6dd4 7420 7a54 69b4 0008 …                ROOM100.BSS   9   (9 Eintraege)
 2     a420 a758 9b54 a42c 77a4 9540 8ba0 7ce0 6d40 8808 91d4 56f0 84bc   ROOM102.BSS  13   (13)
 9     a880 7f14 6be4 9424 adec 8464 a208 8fb4 ac3c 8704 6b2c 9d64 …      ROOM109.BSS  16   (16)
12     6f40 542c 63c0 5ab0 73e8 6e64 0008 …                               ROOM10C.BSS   6   (6)
```
`0x0008` ist der Fuellwert fuer nicht existierende Cuts (16/16 Zeilen stimmen).

### 2.3 Der Trailer — Format und Gegenprobe

```
chunk[e-8 .. e-4)  u32 sld_offset    (Byte-Offset des SLD-Blocks im selben Chunk)
chunk[e-4 .. e  )  u32 sld_present   (0 = kein Vordergrund; sonst 1)
chunk[sld_offset]  u32 unpacked_size (LE32)
chunk[sld_offset+4 …]                (Flag-Byte-Stream fuer FUN_800c47e8)
```

Sweep ueber **alle 6 Stages, 120 BSS-Dateien, 1119 Cuts**
(Rohausgabe: `analysis/esp_masken_2026-09-03/_sld_dir_sweep.txt`, `_fg_map.json`):

| Groesse | Wert |
|---|---|
| Cuts gesamt | 1119 |
| `sld_present != 0` | **359** |
| `sld_present == 0` | 760 |
| Bloecke, die NICHT zu einem gueltigen TIM entpacken | **0** |
| Beobachtete `sld_present`-Werte | ausschliesslich `1` |
| `sld_offset < e-8` | **359 / 359** |
| Stream-Ende `<= e-8` | **359 / 359** (Luecke 0/1/2/3 Bytes = 4-Byte-Padding; Verteilung 96/91/97/75) |
| `e`-Wertebereich | `0x0A9C … 0xE720` (immer `> 8` und `< 0x10000`) |
| entpackte Groessen | `0xE420` (1x), `0xF220` (38x), `0x10220` (320x) |
| max. komprimierte Blockgroesse | 30920 B |
| max. Rueckdistanz | **8191** (= 2^13-1) |
| max. Match-Laenge | 66 (= 0x3F+3) |

Der Trailer sitzt also **immer exakt in den letzten 8 Bytes der geladenen Laenge**, und der
SLD-Stream endet unmittelbar davor (auf 4 Bytes aufgerundet).

### 2.4 Warum die deterministische Regel und nicht der Scan

Die Regel ist **das, was das Original tut** (§1.1). Der Scan ist eine Erfindung des Extraktors.
Damit ist die Wahl nach der RE-Disziplin bereits entschieden. Es kommt hinzu, dass der Scan
**messbar falsch** ist:

### 2.5 Beleg: der Scan ueberdetektiert (ROOM305, Cut 14)

`STAGE3/ROOM305.BSS`, Chunk 14, Tabellenwert `e = 0x77CC`:
```
chunk[0x77c4 .. 0x77cc)  =  58 61 ff eb | 00 00 00 00
                            ^^^^^^^^^^^   ^^^^^^^^^^^
                            Muellwert     sld_present = 0   -> KEIN Vordergrund
```
`find_sld()` findet trotzdem einen gueltigen SLD-Block — **bei Offset `0x77CC`, also GENAU AB `e`**,
d. h. jenseits der Laenge, die der CD-Loader ueberhaupt in den RAM bringt (§1.2). Der Extraktor hat
diesen Altbestand als `BSS/ROOM3050/PRI14.TIM` und `BSS/ROOM3051/PRI14.TIM` (je 66080 B)
herausgeschrieben. Gegenprobe: beide Dateien sind **byte-identisch** mit dem Entpack-Ergebnis
dieses Alt-Blocks. Das Original zeigt an dieser Stelle **keinen** Vordergrund.

Das sind die einzigen 2 der 418 vorhandenen `PRI*.TIM`, die die deterministische Regel nicht
kennt — und sie sind belegt falsch.

> **Zur Zahlenlage aus der Aufgabenstellung:** die dort genannten „366 + 130 + 114" stammen aus dem
> Scan und mischen echte Vordergruende mit Altbestand. Die deterministische Zahl ist **359**.
> Die Aussage „STAGE5-Cuts haben Masken, aber keinen SLD-Block" trifft **nicht** zu:
> STAGE5 hat 76 echte Vordergrund-Cuts (ROOM502/503/504/505/506/509/50A/50B/50C/510/511/512/514) —
> ihnen fehlt nur die **extrahierte Datei**, nicht der Block.

### 2.6 Bestandsaufnahme: was der Port heute verliert

Deterministische Vordergrund-Cuts (359) gegen die vorhandenen `PRI##.TIM` (418 Dateien =
209 eindeutige `(Raum,Cut)` + Spieler-1-Duplikate + die 2 falschen aus §2.5):

| | Cuts |
|---|---|
| Vordergrund vorhanden (deterministisch) | 359 |
| davon mit extrahierter `PRI##.TIM` | 209 |
| **davon OHNE Datei → Port zeichnet nichts** | **151** |

Aufschluesselung der 151 fehlenden (Raum, Anzahl Cuts):
ROOM102 12 · ROOM103 12 · ROOM104 5 · ROOM107 8 · ROOM122 1 · ROOM404 1 ·
ROOM502 12 · ROOM503 4 · ROOM504 6 · ROOM505 4 · ROOM506 12 · ROOM509 4 · ROOM50A 10 ·
ROOM50B 3 · ROOM50C 4 · ROOM510 6 · ROOM511 4 · ROOM512 6 · ROOM514 11 ·
ROOM600 9 · ROOM601 12 · ROOM603 5.

Und die Gegenrichtung: fuer **alle 209** gemeinsamen Cuts stimmt die entpackte Groesse exakt mit
der Dateigroesse ueberein (0 Abweichungen) — und der Byte-Vergleich (§4) ergibt
**416/416 identisch, 0 Abweichungen**.

### 2.7 Ein Scan bleibt trotzdem im Entwurf — aber nur als Diagnose

Falls `BIN/STAGE<n>.BIN` fehlt (kaputtes Paket), gibt es keine byte-true Antwort. Der Entwurf sieht
dafuer **keinen stillen Scan-Fallback** vor, sondern „kein Vordergrund + eine Log-Zeile".
Ein Scan waere ein Rate-Defekt: er kann, wie §2.5 zeigt, Vordergruende erfinden, die das Original
nicht hat. Der Scan wird als `re15_sld_scan_chunk()` nur unter `RE15_SLD_SCAN=1` (Mess-Werkzeug)
angeboten und ist im Normalpfad tot.

---

## 3. Der Umbau

### 3.1 Neues Engine-Modul `re15_port/engine/src/sld_common.c` + `include/re15_sld.h`

```c
/* ---- include/re15_sld.h ------------------------------------------------ */
#ifndef RE15_SLD_H
#define RE15_SLD_H
#include <stdint.h>
#include <stddef.h>

/* Groesster gemessener entpackter Atlas ueber alle 6 Stages: 0x10220 = 66080 B
 * (Sweep 2026-09-03, 359 Bloecke: 0xE420 x1, 0xF220 x38, 0x10220 x320). */
#define RE15_SLD_MAX_UNPACKED   0x10220

/* Zeilen-Stride der Stage-Cut-Tabelle (@0x80021d50 `sll v0,v0,5`). */
#define RE15_SLD_TBL_ROW_STRIDE 0x20
/* Spalten je Zeile (16 u16 = 0x20 B; @0x80021d4c `sll v1,v1,1`). */
#define RE15_SLD_TBL_COLS       16

/* Fehlercodes. 0 und positive Werte sind KEIN Fehler. */
enum {
    RE15_SLD_OK            =  0, /* Atlas dekodiert, *out_len gesetzt            */
    RE15_SLD_NO_FOREGROUND =  1, /* chunk[e-4] == 0  -> Original zeichnet nichts */
    RE15_SLD_E_ARG         = -1, /* NULL-Zeiger / negative Groesse               */
    RE15_SLD_E_USEDLEN     = -2, /* e < 8 oder e > chunk_size                    */
    RE15_SLD_E_OFFSET      = -3, /* sld_offset+4 > e-8                           */
    RE15_SLD_E_SIZE        = -4, /* unpacked_size <= 0 oder > out_cap            */
    RE15_SLD_E_SRC_EOF     = -5, /* Stream laeuft ueber chunk_size hinaus        */
    RE15_SLD_E_BACKREF     = -6, /* dp+off < 0  (Rueckreferenz vor Pufferanfang) */
    RE15_SLD_E_DST_OVF     = -7, /* dp+len > unpacked_size                       */
    RE15_SLD_E_NOTABLE     = -8  /* Stage-Tabellenzeile nicht lesbar             */
};

/* Datei-Offset der Cut-Tabelle in BIN/STAGE<stage>.BIN. stage 1..6, sonst 0.
 * Werte disasm-belegt (§2.1). */
uint32_t re15_sld_table_file_offset(int stage);

/* 1:1-Port von FUN_800c47e8 (DEBUG.BIN +0x47E8).
 * src/src_size = der GANZE Chunk (fuer die Bereichspruefung), src_pos = erstes
 * Stream-Byte (= sld_offset+4). Schreibt genau dst_size Bytes.
 * Rueckgabe: >=0 = Index des ersten NICHT gelesenen Quell-Bytes; <0 = Fehlercode. */
int re15_sld_decode(const uint8_t *src, int src_size, int src_pos,
                    uint8_t *dst, int dst_size);

/* Trailer auswerten + dekodieren. `used_len` = e aus der Stage-Tabelle.
 * RE15_SLD_OK -> *out_len = entpackte Groesse (fertiger Sony-TIM in `out`).
 * RE15_SLD_NO_FOREGROUND -> *out_len = 0, `out` unveraendert. */
int re15_sld_atlas_from_chunk(const uint8_t *chunk, int chunk_size,
                              uint16_t used_len,
                              uint8_t *out, int out_cap, int *out_len);

/* NUR Diagnose (RE15_SLD_SCAN=1). NICHT byte-true — findet Altbestand jenseits
 * von e (Beleg: ROOM305 Cut 14, H_runtime_sld_plan.md §2.5). */
int re15_sld_scan_chunk(const uint8_t *chunk, int chunk_size,
                        uint32_t *out_off, uint32_t *out_size);
#endif
```

**Fehlersemantik im Detail** (was das Original tut vs. was der Port tut):

| Fall | Original | Port |
|---|---|---|
| `size <= 0` | `blez a1` @`0x800c47f0` → sofort `jr ra`, Ziel unberuehrt | `RE15_SLD_E_SIZE` (defensiv; kommt in 359/359 nicht vor) |
| `chunk[e-4] == 0` | `beq v0,zero` @`0x80021d74` → Vordergrund uebersprungen | `RE15_SLD_NO_FOREGROUND` (Rueckgabe **1**, kein Fehler) |
| `chunk[e-4] != 0` (irgendein Bit) | wird geladen | wird geladen (Test ist `!= 0`, nicht `== 1`) |
| `sld_offset` zeigt daneben | keine Pruefung, liest weiter | `RE15_SLD_E_OFFSET` |
| Stream laeuft ueber das Chunk-Ende | keine Pruefung, liest ins Nachbar-RAM | `RE15_SLD_E_SRC_EOF` |
| Rueckreferenz vor Pufferanfang | keine Pruefung, liest vor `dst` | `RE15_SLD_E_BACKREF` |
| Ziel-Ueberlauf | keine Pruefung, schreibt hinter `dst` | `RE15_SLD_E_DST_OVF` |
| `e < 8` / `e > chunk_size` | keine Pruefung | `RE15_SLD_E_USEDLEN` |

Die zusaetzlichen Wachen aendern fuer **alle 359 echten Bloecke nichts** (gemessen: keine loest
aus). Sie sind reine Speicherschutz-Wachen fuer korrupte Dateien, **keine Verhaltens-Konstanten**.

Aufrufer-Regel: `RE15_SLD_NO_FOREGROUND` und jeder negative Code fuehren zum **selben sichtbaren
Ergebnis** wie heute ein fehlendes `PRI##.TIM` — Overdraw aus. Nur der Log-Text unterscheidet sie.

### 3.2 Wo der Chunk herkommt (PC) — er liegt bereits im Speicher

`re15_port/platform/pc/src/bg_pc.c:210` (`re15_bg_load_room_cut`) hat den vollstaendigen
64-KB-Chunk als `uint8_t *buf` in der Hand — entweder aus `BSS/ROOM%04X/BG%02d.BSS` (ca. Zeile 216)
oder als Scheibe `cut*0x10000` aus `STAGE%u/ROOM%03X.BSS` (ca. Zeile 264; die Scheiben-Regel ist
dort bereits als „1688 Schnitte, alle byte-identisch" gemessen dokumentiert).
**Er wird am Ende der Funktion mit `free(buf)` verworfen** — deshalb kann
`re15_pri_load_cut_atlas()` (`bg_pc.c:63`), das erst spaeter aus `main.c:4451` gerufen wird, ihn
heute nicht sehen.

Entwurf: der SLD-Auszug passiert **innerhalb** von `re15_bg_load_room_cut`, solange `buf` lebt,
und legt das Ergebnis in einem modul-statischen Cache ab:

```c
static uint8_t s_pri_tim[RE15_SLD_MAX_UNPACKED];
static int     s_pri_tim_len  = 0;
static int     s_pri_tim_room = -1, s_pri_tim_cut = -1;
```

`re15_pri_load_cut_atlas(cut)` liest dann nur noch aus diesem Cache
(`s_pri_tim_room == (int)g_current_room_id && s_pri_tim_cut == cut`) statt eine Datei zu oeffnen —
alles Uebrige (`re15_tim_parse`, CLUT→RGBA, Index 0 = transparent,
`re15_render_pc_set_pri_atlas`) bleibt **unveraendert**. Die Reihenfolge in `main.c:4418` (BG) und
`main.c:4451` (PRI) passt bereits.

Rueckfall-Kette (erster Treffer gewinnt):
1. Laufzeit-Cache aus dem BSS-Chunk (neu, byte-true).
2. `BSS/ROOM%04X/PRI%02d.TIM` (heutiger Pfad) — bleibt als Notnagel drin, bis der neue Weg in
   einem Release bestaetigt ist; danach entfernen (er ist eine Ableitung, keine Quelle).

Die Stage-Tabelle liest ein neuer Mini-Lader ueber `re15_pc_read_cd("BIN/STAGE%u.BIN", …)`
(`asset_root_pc.c:280`); es genuegt, die **32 Bytes** ab
`re15_sld_table_file_offset(stage) + row*0x20` zu behalten. Die Datei liegt bereits im Paket
(`release/make_package.sh:327` kopiert `shared_assets/PSX` komplett, inklusive `BIN/`).
Die Zeile wird pro Raumwechsel einmal gelesen und gecached.

### 3.3 PSX-Pfad

`re15_port/platform/psx/src/bg_psx.c:191` laedt den Chunk nach `re15_cd_staging`
(`RE15_CD_STAGING_SIZE = 0x19000` = 102400, `include/re15_cdfs.h:36`). `pri_psx.c:52` laedt heute
`\BSS\ROOM####\PRI##.TIM` **in denselben Puffer** — das entfaellt.

Problem: `0x10000` (Chunk) + `0x10220` (Atlas) = `0x20220` = 131616 B > `0x19000`.
Zwei Varianten:

* **P1 (einfach, RAM-teuer):** eigener statischer Puffer `uint8_t s_sld_out[RE15_SLD_MAX_UNPACKED]`
  in `pri_psx.c` (+66 080 B `.bss`). Der Chunk bleibt in `re15_cd_staging`; nach `LoadImage` ist
  `s_sld_out` frei. Das ist **exakt das Original-Layout** (Chunk @`0x80190000`, Ziel @`0x801A5800`,
  §1.1).
* **P2 (RAM-sparend, ungetestet):** Fenster-Dekodierung. Die max. Rueckdistanz ist gemessen 8191 und
  formal durch `sra …,20` (@`0x800c4868`) auf 12 Bit ⇒ `off ∈ [-8192,-1]` begrenzt; ein 8-KB-Ring
  genuegt, fertige Bildzeilen (256 Px 8 bpp = 512 B) wandern sofort per `LoadImage` ins VRAM.
  Ausgabe bitgleich, nur die Zwischenspeicherung unterscheidet sich. Braucht eine harte Wache
  `off < -8192 → RE15_SLD_E_BACKREF`.

Empfehlung: **P1**. 66 KB von 2 MB sind wohlfeil, und es ist die Original-Anordnung.

⚠ **Nicht verifizierbar:** das PSX-Target baut derzeit nicht (PSn00bSDK-0.24-Layout,
`FindPSn00bSDK`, Memory `reai-v2-psx-build-gap`). Der PSX-Teil dieses Entwurfs ist deshalb
**ungetestet** und bleibt es, bis der Build steht.

VRAM-Ablage: `pri_psx.c` benutzt heute bewusst (512,256) / CLUT (0,502) statt der Original-Adresse
(320,256) / (0,480) — siehe Kommentar `pri_psx.c:12-33`, weil dort im Port die Heli-Prop-Slots
liegen. Dieser Entwurf **aendert daran nichts**; er ersetzt nur die Datenquelle.

---

## 4. Regressions-ctest gegen die vorhandenen `PRI*.TIM` (das Orakel)

Neue Datei `re15_port/tests/integration/test_sld_oracle.c`, registriert wie die uebrigen
(`add_executable` + `target_link_libraries(… re15_engine re15_test_support)` + `add_test`,
Muster `re15_port/tests/unit/CMakeLists.txt:9-14`).

**Ablauf je (Raum, Cut):**
1. Ueber alle `shared_assets/PSX/BSS/ROOM????/` iterieren; `room_id` aus dem Verzeichnisnamen.
2. `stage = (room_id>>12)&0xF`, `row = (room_id>>4)&0xFF`.
3. 32 B aus `BIN/STAGE<stage>.BIN` @ `re15_sld_table_file_offset(stage) + row*0x20` lesen → `e[16]`.
4. Chunk holen: `BSS/ROOM%04X/BG%02d.BSS`, sonst Scheibe `cut*0x10000` aus `STAGE%u/ROOM%03X.BSS`.
5. `re15_sld_atlas_from_chunk(chunk, 0x10000, e[cut], out, sizeof out, &len)`.
6. Vergleich:
   * `PRI%02d.TIM` existiert **und** `RE15_SLD_OK` → `len == filesize` **und**
     `memcmp(out, file, len) == 0`.
   * `PRI%02d.TIM` existiert, Ergebnis `RE15_SLD_NO_FOREGROUND` → **erwarteter Sonderfall**, nur
     zulaessig fuer `ROOM3050/PRI14.TIM` und `ROOM3051/PRI14.TIM` (§2.5). Jeder weitere Fall ist
     ein Fehlschlag.
   * keine Datei, Ergebnis `RE15_SLD_OK` → neu gewonnener Atlas: pruefen, dass `out` ein gueltiger
     Sony-TIM ist (`re15_tim_parse(out,len,&t) == 0 && t.bpp == 8 && t.has_clut`).
   * jeder negative Code → Fehlschlag.

**Feste Soll-Zahlen (heute unabhaengig in Python gemessen — die Schiene muss ihre ABDECKUNG
ausgeben, Lehre `reai-v2-schiene-abdeckung`):**

```
PRI-Dateien gefunden           418
byte-identisch                 416      (Pflicht: == gefunden - 2)
erwartete Nicht-Ladung           2      (ROOM3050/PRI14, ROOM3051/PRI14)
Abweichungen                     0      (Pflicht: == 0)
Cuts mit Vordergrund (gesamt)  359      ueber alle 6 Stages
davon ohne PRI-Datei           151      (neu gewonnen; alle muessen gueltige TIMs sein)
Cuts ohne Vordergrund          760
```

Faellt „PRI-Dateien gefunden" unter 418 oder „Cuts mit Vordergrund" unter 359, ist der Asset-Baum
unvollstaendig — der Test muss das als **Fehler** melden und nicht still weniger pruefen
(Lehre `reai-v2-proxy-ohne-zielmenge`).

Laufzeit: 1119 Chunk-Lesungen a 64 KB + 359 Dekompressionen ≈ 75 MB I/O; auf dieser Maschine
< 10 s ⇒ `set_tests_properties(… PROPERTIES TIMEOUT 120)`.

Zusaetzlich ein reiner **Unit**-Test `tests/unit/test_sld_decode.c` **ohne Assets**: die vier
Token-Formen (Roh-Literal, geflaggtes Literal, kurze und lange Rueckreferenz) an handgebauten
Streams — insbesondere die Falle „Roh-Literal verbraucht kein Flag-Bit" (@`0x800c482c`) und die
ueberlappende Kopie (`off = -1`, `len = 66`) — plus die sieben Fehlerpfade. Damit faellt ein
Decoder-Regress auch dann auf, wenn der Asset-Baum fehlt.

---

## 5. Kosten

### 5.1 Rechenzeit

Token-Statistik ueber alle 359 Bloecke (Mittel: 2485 Roh-Literale, 1876 geflaggte Literale,
1585 kurze + 1569 lange Rueckreferenzen, 61262 kopierte Bytes, 629 Flag-Bytes).
Worst Case **ROOM121.BSS Cut 4** (`0x10220` entpackt): 6495 / 160 / 5147 / 3370 / 59425 / 1085
⇒ ≈ **605 000 Instruktionen**.

Auf R3000 @ 33,8688 MHz: **≈ 18 ms** bei 1 Instr/Takt, **≈ 36 ms** bei 2 Takten/Instr (RAM-Stalls).
Das sind 1–2 Frames, **einmalig pro Cut-Wechsel**.

**Der entscheidende Beleg ist aber nicht die Schaetzung, sondern:** das Original macht **genau
das** bei jedem Cut-Wechsel mit Vordergrund (`jal FUN_800c47e8` @`0x80021db8`, im selben Aufruf wie
CD-Read und MDEC-Dekodierung). Die Kosten sind per Konstruktion budgetiert.
Auf dem PC ist es ohnehin irrelevant (Groessenordnung 0,3 ms).

### 5.2 RAM / Puffer

Original-Anordnung (belegt in §1.1):

| Puffer | Adresse | Groesse |
|---|---|---|
| BSS-Chunk (CD-Ziel) | `0x80190000` | bis `e`, max. gemessen `0xE720` |
| SLD-Ziel (fertiger TIM) | `0x801A5800` | max. `0x10220` = 66080 |

Die Raum-Arena liegt **darunter** und ist unberuehrt:
`FUN_80039270` (`RE_15_Quellcode_V2/FUN_80039270.c`) verteilt aus dem Bump-Zeiger `DAT_800ac77c`
die Masken-Puffer (`hdr7*4` Flags + zwei Prim-Puffer je `hdr7*0x20`, Vorschub `hdr7*68`
@`0x800392bc`); `DAT_800ac77c`/`DAT_800ac778` werden beim Raum-Laden auf den RDT-Zeiger gesetzt
(`sw a0,-0x3884(at)` @`0x80039738`, `sw a0,-0x3888(at)` @`0x8003973c`).
Savestate `lamp_1170_run3.sav`: RDT-Basis `0x801219B0`, Arena-Kopf `0x8017197C`
⇒ die Arena endet rund `0x1E000` **unterhalb** des Chunk-Puffers. **Der Atlas kommt der Arena also
nicht ins Gehege** — er hat sein eigenes, festes Fenster.

Port-Bedarf: **+66 080 Bytes statisch** (Variante P1) bzw. +8 KB (P2, ungetestet).
PC: irrelevant. PSX: 66 KB von 2 MB — und der Puffer, den `pri_psx.c` heute fuer die
`PRI##.TIM`-Ladung in `re15_cd_staging` belegt, wird im Gegenzug frei.

---

## 6. Schritt-fuer-Schritt-Plan (konkrete Datei-Aenderungen)

1. **`re15_port/include/re15_sld.h` (neu)** — API + Fehlercodes exakt wie §3.1;
   Kopfkommentar traegt `DEBUG.BIN +0x47E8` / `FUN_800c47e8` und die Tabellenadressen aus §2.1.
2. **`re15_port/engine/src/sld_common.c` (neu)** — `re15_sld_decode` als 1:1-Port der 67
   Instruktionen (jede Nicht-Offensichtlichkeit mit `@0x800c48xx` im Kommentar: kein Flag-Bit beim
   Roh-Literal @`0x800c482c`, `sra` statt `srl` @`0x800c4868`, 16-Bit-Sign-Extend
   @`0x800c4898`/@`0x800c48a0`, signed `slt` @`0x800c48e4`), dazu
   `re15_sld_atlas_from_chunk` (Trailer @`0x80021d6c` / @`0x80021da4`),
   `re15_sld_table_file_offset` (Tabelle §2.1) und das gegatete `re15_sld_scan_chunk`.
   *(CMake erfasst `engine/src/*.c` per GLOB — kein CMake-Edit noetig.)*
3. **`re15_port/platform/pc/src/bg_pc.c`**
   - neuer statischer Cache `s_pri_tim[] / s_pri_tim_len / s_pri_tim_room / s_pri_tim_cut`;
   - neuer Helfer `pri_used_len(room_id, cut)` — liest 32 B aus `BIN/STAGE%u.BIN`
     (`re15_pc_read_cd`), cached die Zeile pro Raum;
   - in `re15_bg_load_room_cut`, in **beiden** Erfolgspfaden (vorgeschnittene `BG##.BSS` **und**
     Original-Scheibe), solange `buf` lebt: `re15_sld_atlas_from_chunk(…)` → Cache fuellen bzw. auf
     „kein Vordergrund" setzen;
   - `re15_pri_load_cut_atlas` (Zeile 63): zuerst den Cache benutzen, `PRI##.TIM` nur noch als
     Rueckfall. Der TIM→RGBA-Block bleibt unveraendert.
   > ⛔ Fehlerklasse aus `reai-v2-uebergabe-0825`: die Chunk-Beschaffung steht in **zwei** Zweigen.
   > **Beide** anfassen, sonst faellt der halbe Asset-Baum durch (STAGE5/STAGE6 gehen ausschliesslich
   > ueber den Original-Scheiben-Zweig — genau die Raeume, um die es hier geht).
4. **`re15_port/platform/psx/src/pri_psx.c`** — `re15_pri_psx_load_cut` bekommt eine Variante, die
   den Chunk (bereits in `re15_cd_staging`, `bg_psx.c:191`) + `used_len` nimmt und nach
   `s_sld_out[RE15_SLD_MAX_UNPACKED]` dekodiert; `re15_tim_parse` + `LoadImage` bleiben. Die
   CD-Ladung der `PRI##.TIM` entfaellt. **Ungetestet, PSX-Build steht nicht (§3.3).**
5. **`re15_port/platform/psx/src/bg_psx.c`** — `re15_bg_load_cut` reicht den geladenen Chunk +
   Groesse an den PRI-Lader durch (heute wird er nach `re15_bg_load_from_bss` vergessen);
   Tabellenzeile aus `\BIN\STAGE%u.BIN;1` via `re15_cd_load_file`.
6. **`re15_port/tests/unit/test_sld_decode.c` (neu)** + Eintrag in
   `re15_port/tests/unit/CMakeLists.txt` — Decoder-Einheitstest ohne Assets (§4).
7. **`re15_port/tests/integration/test_sld_oracle.c` (neu)** + Eintrag in
   `re15_port/tests/integration/CMakeLists.txt` — das 418-Dateien-Orakel mit den festen Soll-Zahlen
   und der Abdeckungs-Ausgabe.
8. **Bauen + messen:** `bash re15_port/tools/local_build.sh` (Soll: bisherige Tests + 2 neue gruen;
   Abschlusszeile `LOCAL-BUILD-OK`).
9. **Sicht-Gegenprobe** nach Skill `re15-port-visual-verify` (gdigrab, echtes Fenster, plain exe):
   **ROOM1020** (heute ohne Vordergrund → kuenftig 12 Cuts mit) und **ROOM1170 Cut 1** (heute mit
   `PRI01.TIM` — das Bild darf sich **nicht** aendern; der ctest beweist die Byte-Gleichheit, die
   Sicht-Probe beweist, dass der Einhaengepunkt stimmt und nichts anderes verschoben wurde).
10. **Danach**, als eigener Commit: den `PRI##.TIM`-Rueckfall aus `bg_pc.c` und die 418
    abgeleiteten Dateien aus `shared_assets/PSX/BSS/*/` entfernen — sie sind ab dann redundant,
    zwei davon sind belegt falsch (§2.5), und sie kosten ~27 MB im Paket.

---

## 7. Was NICHT belegt ist

* **`sld_present`** hat in 359/359 Faellen den Wert `1`. Ob eine andere Bitbelegung anders behandelt
  wuerde, ist **NICHT BELEGT** — der Original-Test ist `!= 0` (`beq v0,zero` @`0x80021d74`), also ist
  die Port-Semantik „`!= 0`" byte-true, unabhaengig davon.
* **Der 8192-Fenster-Wert (Variante P2)** folgt aus `sra …,20` @`0x800c4868` (12-Bit-Offset) plus
  `<<2` @`0x800c4890` ⇒ `off ∈ [-8192,-1]`. Das ist ein **Ableitungs**-Argument aus dem Code, kein
  Header-Feld; empirisch belegt sind 8191 als groesster tatsaechlich vorkommender Wert.
* **Warum `DEBUG.BIN` den Depacker traegt** (und nicht die EXE) ist nicht untersucht. Belegt ist nur:
  die Datei laedt @`0x800C0000` (97,26 % Byte-Gleichheit gegen Savestate-RAM, Codebereich 100 %),
  und EXE-Code springt hinein (`jal FUN_800c47e8` @`0x80021db8`, ausserdem `j LAB_800c48f8`
  @`0x80029480`).
* **PSX-Laufzeit** (§5.1) ist eine Instruktions-Schaetzung, keine Messung — der PSX-Build steht
  nicht. Die Aussage „das Budget reicht" stuetzt sich stattdessen darauf, dass das ORIGINAL diese
  Arbeit an derselben Stelle leistet.
* **Die Beziehung zwischen Maskenrecords (RDT `pri_offset`) und Vordergrund-Atlas** ist hier NICHT
  neu untersucht worden. Dieser Entwurf beschafft nur den Atlas; welche Cuts Masken haben, bleibt
  Sache von `re15_pri_parse_section` (`engine/src/pri_common.c`).

---

## 8. Erzeugte Belegdateien

| Datei | Inhalt |
|---|---|
| `analysis/esp_masken_2026-09-03/_debugbin_800c47e8.txt` | Vollstaendige Disassembly + Hex-Dump von `FUN_800c47e8` aus `DEBUG.BIN` (+0x47E8, 0x110 B) |
| `analysis/esp_masken_2026-09-03/_sld_dir_sweep.txt` | 6-Stage-Sweep der deterministischen Regel (359/0/760) |
| `analysis/esp_masken_2026-09-03/_fg_map.json` | Die 359 Vordergrund-Cuts: `(stage, bss, cut, e, flag, sld_offset, unpacked_size, stream_end)` |

---

## Gegenpruefung

Gegenpruefung 2026-09-03 durch einen zweiten Agenten. Auftrag: die 19 Behauptungen WIDERLEGEN.
Vorgehen: jede zitierte Adresse in `ghidra1_V2.txt` / `RE_15_Quellcode_V2/` nachgeschlagen, jeder
Datei-Byte-Offset in den Original-Bytes geprueft, und ALLE Datenaussagen mit EIGENEM Code neu
erhoben — insbesondere ein von Hand aus der Disassembly von `0x800C47E8` neu geschriebener
Depacker (`analysis/esp_masken_2026-09-03/xcheck/x_sld.py`, Instruktion fuer Instruktion, ohne
`sld.py` und ohne die Zahlen des Berichts). Skripte: `xcheck/x_sld.py`, `xcheck/x_sweep.py`,
`xcheck/x_sweep2.py`, `xcheck/x_cmp.py`.

### Was NICHT standhaelt

**(a) Behauptung 11 — "STAGE5 ... = 76" ist falsch, richtig ist 86.**
Die im selben Satz aufgezaehlten Raum-Einzelwerte summieren sich selbst auf 86:
12+4+6+4+12+4+10+3+4+6+4+6+11 = 86. Eigener Mengenvergleich `want(359)` gegen
`re15_port/shared_assets/PSX/BSS/ROOM????/PRI*.TIM` (418 Dateien) ergibt fuer STAGE5:
ROOM502 12, ROOM503 4, ROOM504 6, ROOM505 4, ROOM506 12, ROOM509 4, ROOM50A 10, ROOM50B 3,
ROOM50C 4, ROOM510 6, ROOM511 4, ROOM512 6, ROOM514 11 = **86**. Die Gesamtzahl 151 der
Behauptung beweist den Rechenfehler mit: 39 (STAGE1/4) + 76 + 26 (STAGE6) = 141 != 151;
mit 86 kommt 151 heraus. (`xcheck/x_cmp.py`: `missing (want without file): 151`.)

**(b) Behauptung 17 — "STAGE5 liefert 76 Cuts mit sld_present != 0" ist falsch, richtig ist 86.**
Derselbe Fehler. Eigener deterministischer Sweep (`xcheck/x_sweep2.py`):
`per stage {1: 122, 2: 15, 3: 77, 4: 33, 5: 86, 6: 26}` — Summe 359. Die inhaltliche Aussage
("STAGE5 hat sehr wohl SLD-Bloecke, ihnen fehlt nur die extrahierte Datei") bleibt richtig,
die Zahl nicht.

**(c) Behauptung 14 — die Adresse `@0x8003973c sw a0,-0x3888(at)` steht so nicht da.**
`ghidra1_V2.txt` (Zeile 137584 ff.):

```
80039734 0b 80 01 3c     lui        at,0x800b
80039738 7c c7 24 ac     sw         a0,-0x3884(at)=>DAT_800ac77c
8003973c 0b 80 01 3c     lui        at,0x800b
80039740 78 c7 24 ac     sw         a0,-0x3888(at)=>DAT_800ac778
```

An `0x8003973c` steht ein `lui`, nicht der Store. Richtig waere `@0x80039740`. (`@0x80039738`
und die inhaltliche Aussage stimmen; `FUN_80039270` verteilt tatsaechlich hdr7*4 + 2 x hdr7*0x20
und schiebt `DAT_800ac77c` um hdr7*68 vor — Decompilat `RE_15_Quellcode_V2/FUN_80039270.c`
und Disasm `0x80039270..0x800392d0` gegengeprueft.)
Zusaetzlich: die Nicht-Kollision ist an EINEM Savestate gemessen (Arena-Kopf 0x8017197C); dass
die Arena nie ueber 0x80190000 waechst, ist damit nicht allgemein belegt, sondern nur fuer diese
Stichprobe.

**(d) Behauptung 6 — "alle 16 STAGE1-Zeilen" ist die falsche Zahl.**
STAGE1 hat **40** BSS-Dateien (ROOM100..ROOM127), nicht 16 — das sagt der eigene Sweep des
Berichts in Zeile 1 von `_sld_dir_sweep.txt` selbst: `STAGE1: rooms=40  count-mismatches=1`.
Eigene Nachrechnung: 39 von 40 Zeilen stimmen, **eine nicht** — `ROOM127.BSS` ist 4 Byte gross
(0 Chunks), die Tabellenzeile 0x27 enthaelt 16 Eintraege != 0x0008. Die vier zitierten
Beispielzeilen stimmen exakt (Zeile 0 = `53bc 607c 78a8 74a8 5564 6dd4 7420 7a54 69b4` + 7x0008;
Zeile 2 = 13; Zeile 9 = 16; Zeile 12 = 6). Der Mechanismus (Zeile = Raum, Spalte = Cut) ist
also sogar besser belegt als behauptet, die Angabe "16/16" ist aber sachlich falsch.

**(e) Behauptung 7, Belegzeile — "e-Bereich 0x0A9C..0xE720" gilt nicht fuer die 359 Vordergrund-Cuts.**
Ueber die 359 Cuts mit `sld_present != 0` ist der Bereich **0x57C8..0xE720**. `0x0A9C` ist das
Minimum ueber ALLE 1119 Tabelleneintraege != 0x0008 (`ROOM103` Cut 13). In einer Belegzeile, die
sonst ausschliesslich die 359 beschreibt, ist das irrefuehrend.

**(f) Behauptung 16 — Zeilenangabe und Puffergroesse ungenau.**
`re15_bg_load_room_cut` beginnt in `re15_port/platform/pc/src/bg_pc.c` bei **Zeile 195**, nicht 210.
Und `buf` ist nur im ersten Zweig (`BSS/ROOM%04X/BG%02d.BSS`) 64 KB gross; im Rueckfall-Zweig
(`STAGE%u/ROOM%03X.BSS`, ca. Z. 264) haelt `buf` die GANZE Original-Datei — bis 1 048 576 B
(`ROOM126.BSS`). Die Kernaussage (Chunk liegt beim Laden vollstaendig vor, `free(buf)` steht in
beiden Erfolgszweigen vor dem `return`, Einhaengepunkt ist `re15_bg_load_room_cut`) haelt.
`bg_pc.c:63` und `main.c:4418` / `main.c:4451` stimmen exakt.

### Was bestaetigt wurde (unabhaengig nachgemessen)

* **1** — `PSX.EXE` Header: `t_addr=0x80010000`, `t_size=0x000AF000` -> Textende `0x800BF000`,
  `0x800C47E8` liegt ausserhalb. `DEBUG.BIN` (262144 B = 0x40000) +0x47E8:
  `21 58 00 00 21 60 00 00 3f 00 a0 18 21 40 00 00`, `+0x48F0: 08 00 e0 03 00 00 00 00`.
  Savestate `stage_saves/lamp_1170_stage1.sav`, RAM 0x800C0000..0x80100000 gegen DEBUG.BIN:
  **254956/262144 = 97,26 %** identisch, Codebereich 0x47E8..0x4908 **100 %** identisch — beide
  Zahlen exakt wie behauptet.
* **2** — `0x80021da8 lui a2,0x801a` / `0x80021dac addiu a2,a2,0x5800` / `0x80021db0 addu a0,s1,s0` /
  `0x80021db4 lw a1,0x0(a0)` / `0x80021db8 jal FUN_800c47e8` / `0x80021dbc _addiu a0,a0,0x4`
  wortwoertlich im Dump (Zeilen 101827-101832). Decompilat `FUN_80021bbc.c` bestaetigt.
* **3** — alle sieben Adressen wortwoertlich bestaetigt (0x80021c40 / d4c / d50 / d5c / d64 / d6c /
  d74 / da4). Welche Haelfte Raum und welche Cut ist, ist ueber Savestate `lamp_1170_run3.sav`
  (fe0=0, fe2=23, fe4=1 bei ROOM1170, Cut 1) eindeutig.
* **4** — `0x80021cb8 sw v1,-0x1a8c(at)=>DAT_800be574` und die kompletten Sequenzen in
  `FUN_80013f80` (0x80013f90/f98/fa4/fc4) und `FUN_80013fdc` (0x80013fec/ff4/4010/4014)
  stehen exakt so da.
* **5** — alle sechs Trampoline nachgerechnet:
  STAGE1 +0x1e970 `3c028012 2442eae4 3c01800b ac2252c8` -> 0x8011EAE4 -> Datei 0x1EAE4;
  STAGE2 +0x16fc8 -> 0x8011713C -> 0x1713C; STAGE3 +0x1cf94 -> 0x8011D0F8 -> 0x1D0F8;
  STAGE4 +0x1844c -> 0x80118590 -> 0x18590; STAGE5 +0x1ddbc -> 0x8011DF18 -> 0x1DF18;
  STAGE6 +0x01e00 -> 0x80101E3C -> 0x01E3C. `RE_15_Quellcode_Overlays/STAGE1/FUN_8011e064.c`
  enthaelt `_DAT_800b52c8 = &DAT_8011eae4;`.
* **6** (Mechanismus) — `RE_15_Quellcode_V2/FUN_8001d600.c:45-47`:
  `_DAT_800b0fe2 = *(byte*)((int)DAT_800ac9a8 + 9)`, `DAT_800b0fe0 = *(byte*)(DAT_800ac9a8 + 4)`.
  Savestate `lamp_1170_run3.sav`: fe0=0, fe2=23, fe4=1. (Nur die Zeilenzahl "16" ist falsch, s. (d).)
* **7** — mit dem NEU geschriebenen Depacker: **fg=359, flag0=760, bad=0**, 359+760 = 1119 Cuts.
  Entpackte Groessen `{0xe420: 1, 0xf220: 38, 0x10220: 320}`. `off < e-8` und `Stream-Ende <= e-8`
  gelten **359/359**. Luecke `(e-8) - Stream-Ende` = **{0: 96, 1: 91, 2: 97, 3: 75}** — alle
  Zahlen identisch mit dem Bericht.
* **8** — ROOM117 Chunk 1: `e=0x5EE4`, `chunk[e-4]=1`, `off=0x4FCC`, `unpacked=0x10220`,
  TIM `flags=0x09`, CLUT-Block `x=0 y=480 w=256 h=1`, Bild-Block `w=128 h=256` (Halbworte).
  `FUN_8004ee78` (Decompilat) mit `DAT_800aca4c=0x15` (`@0x80021de8 ori v0,zero,0x15`):
  `21*64 = 1344`, `>0xF` -> `-0x400` -> **x=320**; `y=(0x15<0x10 ^ 1)<<8` = **256**;
  CLUT-y = `DAT_800aca4d + 0x1e0` = **480** (der `sh` @0x80021df0 nullt 0x800aca4d).
  Gegenprobe gegen Savestate `lamp_1170_run3.sav`: VRAM(320,256)..(447,511)
  **32768/32768 Halbworte identisch**, CLUT @(0,480) **256/256 identisch**.
* **9** — ROOM305 Cut 14: Tabellenwert `e=0x77CC`; `chunk[0x77C4..0x77CC) = 58 61 ff eb | 00 00 00 00`
  -> `sld_present = 0`. Das Groessenwort AT `0x77CC` ist `0x10220`, der Altbestand
  entpackt zu 66080 B mit sha1 `71062f7c66869666c1f521a3e65ee0d6d91e2370` — und **beide**
  `BSS/ROOM3050/PRI14.TIM` und `BSS/ROOM3051/PRI14.TIM` haben genau diesen sha1. Bestaetigt.
* **10** — eigener Vergleich der 359 deterministisch gefundenen Bloecke gegen die 418 Dateien:
  **byte-identisch 416, differing 0, ohne Gegenstueck 2** (genau die beiden ROOM305-PRI14).
* **11** (Mechanismus + Gesamtzahl) — **151** fehlende Dateien, Aufschluesselung
  ROOM102 12, ROOM103 12, ROOM104 5, ROOM107 8, ROOM122 1, ROOM404 1, STAGE5 86, STAGE6
  (ROOM600 9, ROOM601 12, ROOM603 5 = 26). Port-Ursache in `bg_pc.c:63` bestaetigt.
  (Nur der STAGE5-Teilbetrag ist falsch, s. (a).)
* **12** — alle vier Details an der eigenen Disassembly von DEBUG.BIN +0x47E8 bestaetigt:
  (1) `800c4818 andi v0,a3,0x80` / `800c481c bne v0,zero,0x800c4834`, und der Roh-Zweig springt
  mit `800c482c j 0x800c48e4` an BEIDEN `srl t4,t4,1` (0x4850, 0x48a8) UND am `andi t4,t4,0xffff`
  (0x48e0) vorbei — kein Flag-Verbrauch.
  (2) `800c483c sll v1,a3,24` + `800c4860 sll v0,a3,16` + `800c4868 sra t2,v0,20`: Bit 31 = Bit 7
  von a3 = 1 -> t2 immer negativ. Ueber alle 359 Bloecke gemessen: **kein einziger nicht-negativer
  Offset**.
  (3) `800c4898 sll v0,v0,16` + `800c48a0 sra t2,v0,16`.
  (4) `800c47f0 blez a1,0x800c48f0` + `800c48e4 slt v0,t0,a1` + `800c48e8 bne`.
  Der aus dieser Disassembly neu geschriebene Depacker reproduziert 416/416 Dateien byte-genau —
  staerkster moeglicher Beleg.
* **13** — eigener Token-Sweep ueber die 359 Bloecke: Mittel `raw 2485,9 / flagged 1876,7 /
  short 1586,0 / long 1569,5 / copied 61262,5 / flag 629,4`; groesster komprimierter Block
  **30920 B**; groesster entpackter **0x10220 = 66080**; **max. Rueckdistanz gemessen 8191**,
  **max. Match-Laenge gemessen 66**. Worst-Case-Block `ROOM121` Cut 4 mit exakt
  `6495 / 160 / 5147 / 3370 / 59425 / 1085`. Die Instruktionszahl bleibt eine Rechnung — als
  `[teilbelegt]` korrekt gekennzeichnet.
* **15** — `re15_port/include/re15_cdfs.h:36 #define RE15_CD_STAGING_SIZE 0x19000` (= 102400);
  `bg_psx.c:191` und `pri_psx.c:52` benutzen beide `re15_cd_staging`. 0x10000 + 0x10220 =
  0x20220 = 131616 > 102400. Fenstergrenze 8192 folgt aus `800c4868 sra ...,20` + `800c4890
  sll v1,t2,2` und ist oben gemessen bestaetigt (max. 8191).
* **18** — `@0x80021d74 beq v0,zero,LAB_80021df8` ist tatsaechlich der einzige Test; eigener
  Sweep: `Counter({1: 359})` — `sld_present` ist in allen 359 Faellen exakt 1. Korrekt als
  NICHT BELEGT markiert.
* **19** — `ghidra1_V2.txt` Zeile 114876: `80029480 3e 12 03 08  j LAB_800c48f8`, und
  Zeile 575245 zeigt `LAB_800c48f8` mit XREF aus `FUN_8002939c:80029480(j)`. Die Einschraenkung
  ("Lademechanismus nicht disassembliert") ist korrekt.

### Fazit

Der Kern des Plans — deterministische Lokalisierung ueber die Overlay-Cut-Tabelle, der
byte-true Depacker aus DEBUG.BIN, die VRAM-Zielrechnung und das 416/416-Regressionsorakel —
haelt der unabhaengigen Nachmessung vollstaendig stand. Widerlegt sind ausschliesslich vier
Zahlen-/Adressfehler in den Belegzeilen (STAGE5 = 86 statt 76 in zwei Behauptungen, `0x80039740`
statt `0x8003973c`, "16 STAGE1-Zeilen" statt 40 mit 39/40 Treffern, e-Bereich der 359 =
0x57C8..0xE720) sowie zwei ungenaue Port-Angaben (`bg_pc.c:195` statt 210; `buf` bis 1 MB statt
64 KB). Keiner dieser Fehler beruehrt den vorgeschlagenen Implementierungsweg — mit einer
Ausnahme fuer die Planung: fuer STAGE5 fehlen **86** Vordergruende, nicht 76.
