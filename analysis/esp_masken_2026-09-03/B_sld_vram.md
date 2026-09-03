# Aufgabe B — Der SLD-Vordergrundatlas: Dekomprimierung, VRAM-Ziel, CLUT

Datum: 2026-09-03 · Ziel: den Vordergrund-Atlas (die Textur, aus der die sprite.pri-Masken
sampeln) zur LAUFZEIT aus dem BSS holen statt aus vorextrahierten `PRI##.TIM`-Dateien.

Alle Aussagen unten sind mit Adresse/Instruktion oder Datei-Byte-Offset belegt. Wo etwas nicht
belegt werden konnte, steht ausdruecklich **NICHT BELEGT**.

---

## 0. Kernbefund in einem Absatz

`FUN_800c47e8` liegt **nicht** in der `PSX.EXE` (deren Text endet bei `0x800BF000`), sondern in
**`BIN/DEBUG.BIN`**, das beim Boot als Datei-Index 7 nach `0x800C0000` geladen wird — Datei-Offset
`0x47E8`. Der Codec ist Instruktion fuer Instruktion identisch mit `sld.py` / `SldDecoder.java`.
Der entpackte 8-bit-TIM geht ueber `FUN_8004ee78` nach **VRAM (320, 256) = TPage 0x15**, die CLUT
nach **VRAM (0, 480) = Clut-ID 0x7800**; genau diese beiden Werte stehen als Konstanten
(`0x95` @0x80039630, `0x7800` @0x80039498) im Masken-Zeichner. Der SLD-Block wird **nicht gesucht**:
die letzten 8 Bytes der geladenen Cut-Daten sind ein Trailer `{u32 sld_offset; u32 has_fg}`, und die
Laenge der geladenen Cut-Daten steht als **u16 in einer Tabelle im STAGE-Overlay**
(`table[room*0x20 + cut*2]`, Zeiger in `DAT_800b52c8`). Ueber alle 1119 Cuts aller 6 Stages ist die
Regel 1119/1119 konsistent; sie findet **112 Atlanten mehr** als der bisherige Brute-Force-Sweep,
und **die Kategorie "Masken ohne Atlas" existiert nicht (0 von 1119)**.

---

## 1. Wo FUN_800c47e8 lebt — und warum Ghidra nur `??` zeigt

### 1.1 Die EXE endet vor 0x800C0000

`info/Re1.5/PSX.EXE`, PS-X-EXE-Header:

```
Datei 0x00: 50 53 2d 58 20 45 58 45   "PS-X EXE"
Datei 0x18: 00 00 01 80               t_addr = 0x80010000
Datei 0x1C: 00 f0 0a 00               t_size = 0x000AF000
```

Text = `0x80010000 .. 0x800BF000`. Dateigroesse 718848 = `0x800 + 0xAF000`. `0x800C47E8` liegt
**0x57E8 hinter** dem Text-Ende ⇒ in `ghidra1_V2.txt` steht dort korrekt nur `??`
(Zeilen 572933 ff.: `800c4000  ??  ??`). Der Java-Kommentar in `SldDecoder.java` ("it is not in the
static ghidra export") ist damit **bestaetigt** — aber die Begruendung ist nicht "Savestate-only",
sondern: der Code kommt aus einer anderen Datei.

Ghidra kennt trotzdem die Aufrufe (die stehen ja in der EXE):

```
80021db8  fa 11 03 0c   jal   FUN_800c47e8        (ghidra1_V2.txt Zeile 101831)
80029480  3e 12 03 08   j     LAB_800c48f8        (Zeile 114876)
800284f0  6f 1a 03 0c   jal   FUN_800c69bc
80049a2c  28 1b 03 0c   jal   FUN_800c6ca0
80049a94  16 1b 03 0c   jal   FUN_800c6c58
8004a3c4  a8 18 03 0c   jal   FUN_800c62a0
8004a6e8  8c 19 03 0c   jal   FUN_800c6630
```

### 1.2 Der Lader: Datei-Index 7 nach 0x800C0000

`FUN_8001311c` (@0x8001311c, aufgerufen von `FUN_800116f4` @0x80011704, das wiederum aus dem
Boot-Ablauf @0x800212bc kommt):

```
8001311c  fc ff bd 27   addiu sp,sp,-0x4
80013124  07 00 04 34   ori   a0,zero,0x7          ; Datei-Index 7
80013128  0c 80 05 3c   lui   a1,0x800c            ; Ziel 0x800C0000
8001312c  d8 4e 00 0c   jal   FUN_80013b60         ; CD-Datei-Lader
80013130  00 00 06 34   _ori  a2,zero,0x0          ; Modus 0 = blockierend
80013134  01 80 08 3c   lui   t0,0x8001
80013138  58 31 08 8d   lw    t0,0x3158(t0)        ; DAT_80013158 = 0x080300B0 = "j 0x800C02C0"
8001313c  01 80 09 3c   lui   t1,0x8001
80013140  60 3b 29 25   addiu t1,t1,0x3b60         ; t1 = 0x80013b60
80013144  1c 00 28 ad   sw    t0,0x1c(t1)          ; PATCH: 0x80013b7c := j 0x800C02C0
8001314c  20 00 20 ad   sw    zero,0x20(t1)        ; PATCH: 0x80013b80 := nop
```

Der geladene Block uebernimmt also sofort den Datei-Lader selbst (`FUN_80013b60` springt ab
Instruktion 8 nach `0x800C02C0`).

### 1.3 Datei-Index 7 == DEBUG.BIN (bewiesen, nicht geraten)

Die CD-Datei-Tabelle liegt @`0x8006F43C`, Stride 8 (`FUN_80013b60` @0x80013b94-0x80013bdc:
`at = 0x8006f43c + idx*8`, `lw` = Groesse → `DAT_800be574`; `lhu 0x8006f440[idx*8]` +
`lbu 0x8006f442[idx*8] << 16` = LBA → `DAT_800be57c`). Dump (Datei-Offset `0x5FC3C` in PSX.EXE):

| idx | Groesse | LBA | Datei (Groesse identisch) |
|----|---------|-----|---------------------------|
| 0 | 0x219B0 = 137648 | 25 | STAGE1.BIN (137648) |
| 1 | 0x19330 = 103216 | 93 | STAGE2.BIN (103216) |
| 2 | 0x1F80C = 129036 | 144 | STAGE3.BIN (129036) |
| 3 | 0x1A620 = 108064 | 208 | STAGE4.BIN (108064) |
| 4 | 0x208A0 = 133280 | 261 | STAGE5.BIN (133280) |
| 5 | 0x2854 = 10324 | 327 | STAGE6.BIN (10324) |
| 6 | 0x2E38 = 11832 | 333 | TITLE.BIN (11832) |
| **7** | **0x40000 = 262144** | **339** | **DEBUG.BIN (262144)** |

Zweiter, unabhaengiger Beleg: `DEBUG.BIN` enthaelt selbst die Dateinamen-Tabelle und den
Log-String des Laders:

```
$ strings -a -n 5 info/Re1.5/PSX/BIN/DEBUG.BIN | head
Loading %s at %x from %x
DUMMY
STAGE1.BIN
STAGE2.BIN
... TITLE.BIN / DEBUG.BIN / CONFIG.TIM / CORE00.ESP / C_BACK2.TIM / ITEMALL.PIX / MAP01.PIX ...
```

Dritter Beleg: `DEBUG.BIN`-Offset `0x02C0` disassembliert als sinnvolle Fortsetzung von
`FUN_80013b60` (Register-Uebernahme + `srl a0,a0,3` = das `sll a0,a0,3` aus dem EXE-Prolog
rueckgaengig):

```
800c02c0  sw   s1,0(sp)                800c02cc  addu s1,a1,zero
800c02c4  sw   s2,4(sp)                800c02d0  addu s2,a2,zero
800c02c8  sw   s3,8(sp)                800c02d4  srl  a0,a0,3
800c02d8  jal  0x800c0318              800c02dc  addu s3,a3,zero
800c02e4  lhu  a0,-6798(0x800c0000)  ; = DAT_800be572 (Datei-Index aus dem EXE-Prolog)
```

⇒ **`DEBUG.BIN` ist nicht "nur das Debug-Menue", sondern der residente Datei-/Utility-Modul der
Engine.** Er ist beim Boot bedingungslos geladen; `FUN_800c47e8` ist damit immer verfuegbar.

`sha256(info/Re1.5/PSX/BIN/DEBUG.BIN) = 6780c59bd0170d2e1ecc9c3797ee712ff27189f05ccbd375317ea9220c0b3dfe`
(byte-identisch zu `re15_port/shared_assets/PSX/BIN/DEBUG.BIN`).

---

## 2. Der Codec — vollstaendige Disassembly (DEBUG.BIN @0x47E8, 0x110 Bytes / 68 Instr.)

Signatur aus dem Aufrufer (§3): `FUN_800c47e8(a0 = src_stream, a1 = decompressed_size, a2 = dst)`.
Register: `t3` = src-Index, `t0` = dst-Index, `t4` = flagMask, `t5` = flagByte, `a3` = aktuelles Byte,
`t1` = len, `t2` = offset.

```
  800c47e8  21 58 00 00  addu t3,zero,zero          ; sp   = 0
  800c47ec  21 60 00 00  addu t4,zero,zero          ; mask = 0
  800c47f0  3f 00 a0 18  blez a1,0x800c48f0         ; size <= 0 -> return
  800c47f4  21 40 00 00  addu t0,zero,zero          ; dp   = 0
LOOP:
  800c47f8  05 00 80 15  bne  t4,zero,0x800c4810    ; mask != 0 ? -> kein neues Flagbyte
  800c47fc  21 10 8b 00  addu v0,a0,t3
  800c4800  80 00 0c 34  ori  t4,zero,0x80          ; mask = 0x80
  800c4804  00 00 4d 90  lbu  t5,0(v0)              ; flagByte = src[sp]
  800c4808  01 00 6b 25  addiu t3,t3,1              ; sp++
  800c480c  21 10 8b 00  addu v0,a0,t3
  800c4810  00 00 47 90  lbu  a3,0(v0)              ; a = src[sp]
  800c4814  00 00 00 00  nop
  800c4818  80 00 e2 30  andi v0,a3,0x80
  800c481c  05 00 40 14  bne  v0,zero,0x800c4834    ; a >= 0x80 ?
  800c4820  01 00 6b 25  addiu t3,t3,1              ; sp++ (immer)
  800c4824  21 10 c8 00  addu v0,a2,t0              ; --- ROH-LITERAL (a < 0x80) ---
  800c4828  00 00 47 a0  sb   a3,0(v0)              ; dst[dp] = a
  800c482c  39 12 03 08  j    0x800c48e4            ; !!! KEIN Flagbit verbraucht
  800c4830  01 00 08 25  addiu t0,t0,1
  800c4834  24 10 ac 01  and  v0,t5,t4              ; --- a >= 0x80: Flagbit pruefen ---
  800c4838  06 00 40 14  bne  v0,zero,0x800c4854    ; Bit gesetzt -> Rueckverweis
  800c483c  00 1e 07 00  sll  v1,a3,24              ; (delay) v1 = a << 24
  800c4840  21 10 c8 00  addu v0,a2,t0              ; --- GEFLAGGTES LITERAL ---
  800c4844  00 00 47 a0  sb   a3,0(v0)
  800c4848  01 00 08 25  addiu t0,t0,1
  800c484c  38 12 03 08  j    0x800c48e0
  800c4850  42 60 0c 00  srl  t4,t4,1               ; mask >>= 1
  800c4854  21 10 8b 00  addu v0,a0,t3              ; --- RUECKVERWEIS ---
  800c4858  00 00 47 90  lbu  a3,0(v0)              ; b2
  800c485c  00 00 00 00  nop
  800c4860  00 14 07 00  sll  v0,a3,16
  800c4864  25 10 43 00  or   v0,v0,v1              ; (a<<24)|(b2<<16)
  800c4868  03 55 02 00  sra  t2,v0,20              ; off12 = signext12((a<<4)|(b2>>4))
  800c486c  0f 00 e2 30  andi v0,a3,0xf
  800c4870  0c 00 40 14  bne  v0,zero,0x800c48a4    ; (b2 & 0x0f) != 0 -> Kurzform
  800c4874  01 00 6b 25  addiu t3,t3,1
  800c4878  21 10 8b 00  addu v0,a0,t3              ; --- LANGFORM ---
  800c487c  00 00 47 90  lbu  a3,0(v0)              ; b3
  800c4880  01 00 6b 25  addiu t3,t3,1
  800c4884  3f 00 e3 30  andi v1,a3,0x3f
  800c4888  03 00 69 24  addiu t1,v1,3              ; len = (b3 & 0x3f) + 3
  800c488c  82 11 07 00  srl  v0,a3,6
  800c4890  80 18 0a 00  sll  v1,t2,2
  800c4894  25 10 43 00  or   v0,v0,v1
  800c4898  00 14 02 00  sll  v0,v0,16
  800c489c  2a 12 03 08  j    0x800c48a8
  800c48a0  03 54 02 00  sra  t2,v0,16              ; off = signext16((off12<<2)|(b3>>6))
  800c48a4  02 00 49 24  addiu t1,v0,2              ; --- KURZFORM: len = (b2 & 0x0f) + 2
  800c48a8  42 60 0c 00  srl  t4,t4,1               ; mask >>= 1
  800c48ac  21 10 0a 01  addu v0,t0,t2              ; base = dp + off
  800c48b0  0a 00 20 11  beq  t1,zero,0x800c48dc
  800c48b4  21 50 09 01  addu t2,t0,t1              ; neues dp = dp + len
  800c48b8  21 40 06 01  addu t0,t0,a2              ; Schreibzeiger
  800c48bc  21 38 46 00  addu a3,v0,a2              ; Lesezeiger
  800c48c0  21 18 20 01  addu v1,t1,zero
  800c48c4  00 00 e2 90  lbu  v0,0(a3)              ; BYTEWEISE vorwaerts (Overlap erlaubt)
  800c48c8  01 00 e7 24  addiu a3,a3,1
  800c48cc  ff ff 63 24  addiu v1,v1,-1
  800c48d0  00 00 02 a1  sb   v0,0(t0)
  800c48d4  fb ff 60 14  bne  v1,zero,0x800c48c4
  800c48d8  01 00 08 25  addiu t0,t0,1
  800c48dc  21 40 40 01  addu t0,t2,zero            ; dp = dp + len
  800c48e0  ff ff 8c 31  andi t4,t4,0xffff          ; No-Op (u16-Artefakt des Compilers)
  800c48e4  2a 10 05 01  slt  v0,t0,a1              ; while (dp < size)
  800c48e8  c3 ff 40 14  bne  v0,zero,0x800c47f8
  800c48ec  00 00 00 00  nop
  800c48f0  08 00 e0 03  jr   ra
  800c48f4  00 00 00 00  nop
```

### 2.1 Abgleich mit `sld.py` / `SldDecoder.java` — Ergebnis: IDENTISCH

| Punkt | ROM | `sld.py` / Java | Urteil |
|---|---|---|---|
| Schleifenbedingung | `slt t0,a1` @0x800c48e4 (`dp < size`) | `while dp < size` | gleich |
| Roh-Literal `a < 0x80` verbraucht KEIN Flagbit | Sprung @0x800c482c auf `0x800c48e4`, also **hinter** alle `srl t4,t4,1` | `dst[dp]=a; dp+=1` ohne `flagMask >>= 1` | gleich |
| Geflaggtes Literal | `srl t4,t4,1` @0x800c4850 | `flagMask >>= 1` | gleich |
| Rueckverweis | `srl t4,t4,1` @0x800c48a8 | `flagMask >>= 1` | gleich |
| off (Kurzform) | `sra t2,v0,20` @0x800c4868 = signext12((a<<4)+(b2>>4)) | `raw-0x1000 if raw & 0x800` | gleich |
| len (Kurzform) | `addiu t1,v0,2` @0x800c48a4, v0 = `b2 & 0x0f` | `(b2 & 0x0F) + 2` | gleich |
| off (Langform) | `sll v1,t2,2` + `or` + `sra 16` @0x800c4890-a0 | `(s << 2) or (b3 >> 6)` | gleich (Pythons Zweierkomplement-`or` auf negativen Zahlen deckt sich mit MIPS) |
| len (Langform) | `addiu t1,v1,3` @0x800c4888, v1 = `b3 & 0x3f` | `(b3 & 0x3F) + 3` | gleich |
| Kopie | byteweise vorwaerts @0x800c48c4-d8 (Overlap!) | `for k in range(ln): dst[dp+k]=dst[base+k]` | gleich |
| Flagbyte-Nachladen | nur wenn `t4 == 0` @0x800c47f8 | `if flagMask == 0` | gleich |

Einziger struktureller Unterschied: **das ROM prueft keine Grenzen.** `sld.py` wirft
`ValueError("ovf")` bzw. `("neg")`; das ROM wuerde ueber `size` hinausschreiben. Gemessen ueber alle
359 echten Atlanten: **0 Faelle** von `dp+len > size`, **0** Vorwaerts-Referenzen
(Offset-Bereich `-8191 .. -1`), und `dp` landet **359/359 exakt auf `size`**. Der komprimierte Strom
endet 0..3 Bytes vor dem Trailer (4-Byte-Padding). Ein C-Port kann die Grenzpruefungen also als
Assertion fuehren — die realen Daten reizen sie nie aus.

Zusaetzlicher Integritaetsbeweis (waere bei einem einzigen falsch dekodierten Byte kaputt):
fuer **359/359** Atlanten gilt `8 + clut_block_len + image_block_len == decompressed_size` exakt
und `flags == 0x09`.

---

## 3. Wer ruft FUN_800c47e8 auf — und was passiert mit dem TIM

### 3.1 Genau EIN Aufrufer im ganzen Spiel

Bytemuster `jal 0x800c47e8` = `fa 11 03 0c`, gesucht in PSX.EXE + allen 9 `BIN/*.BIN`:

```
PSX.EXE                  : 0x80021db8   <- der einzige Treffer
DEBUG.BIN                : keine
STAGE1..6.BIN, TITLE.BIN : keine
```

Aufrufer = `FUN_80021bbc` = "Kamera-Cut-Hintergrund laden/anzeigen"
(`RE_15_Quellcode_V2/FUN_80021bbc.c`, Xref: `FUN_8002137c` @0x80021558).

### 3.2 Der relevante Ausschnitt, Instruktion fuer Instruktion

```
; --- Groessen-Tabelle des Stage-Overlays holen -------------------------------
80021c40  c8 52 31 8e   lw   s1,0x52c8(0x800b0000)     ; s1 = DAT_800b52c8 = Zeiger auf die u16-Tabelle
; --- CD-Read-Parameter setzen ------------------------------------------------
80021c88  e4 0f e7 84   lh   a3,0x0fe4(0x800b0000)     ; a3 = cut   (DAT_800b0fe4)
80021c90  e2 0f a5 84   lh   a1,0x0fe2(0x800b0000)     ; a1 = room  (DAT_800b0fe2)
80021c94  40 18 07 00   sll  v1,a3,0x1                 ; cut*2
80021c98  40 11 05 00   sll  v0,a1,0x5                 ; room*0x20
80021c9c  21 10 51 00   addu v0,v0,s1
80021ca0  21 18 62 00   addu v1,v1,v0                  ; &tab[room*0x20 + cut*2]
80021cac  00 00 63 94   lhu  v1,0x0(v1)                ; ---> size (Bytes!)
80021cb8  74 e5 23 ac   sw   v1,-0x1a8c(0x800c0000)    ; DAT_800be574 = size
80021cc8  00 00 22 8c   lw   v0,0x0(at)                ; PTR_8007438c[stage] = Datei-Index-Tabelle
80021cd4  00 00 a2 94   lhu  v0,0x0(a1)                ;   [room]
80021cdc  ff ff 42 24   addiu v0,v0,-0x1               ;   -1  = CD-Datei-Index
80021ce4  72 e5 22 a4   sh   v0,-0x1a8e(0x800c0000)    ; DAT_800be572
80021cd8  40 39 07 00   sll  a3,a3,0x5                 ; cut * 0x20 SEKTOREN = cut * 64 KB
80021d20  21 10 47 00   addu v0,v0,a3                  ; LBA = datei_LBA + cut*0x20
80021d28  7c e5 22 ac   sw   v0,-0x1a84(0x800c0000)    ; DAT_800be57c
80021d2c  14 4f 00 0c   jal  FUN_80013c50              ; Read -> Puffer 0x80190000
; --- Vordergrund-Test + Dekompression ----------------------------------------
80021d5c  00 00 62 94   lhu  v0,0x0(v1)                ; v0 = size (nochmal aus der Tabelle)
80021d64  19 80 01 3c   lui  at,0x8019
80021d68  21 08 41 00   addu at,v0,at                  ; at = 0x80190000 + size
80021d6c  fc ff 22 8c   lw   v0,-0x4(at)               ; FLAG = *(u32*)(buf + size - 4)
80021d74  20 00 40 10   beq  v0,zero,LAB_80021df8      ; == 0 -> KEIN Vordergrund, alles ueberspringen
80021d98  00 00 42 94   lhu  v0,0x0(v0)                ; size
80021d9c  19 80 11 3c   lui  s1,0x8019                 ; s1 = 0x80190000
80021da0  21 08 51 00   addu at,v0,s1
80021da4  f8 ff 30 8c   lw   s0,-0x8(at)               ; OFF  = *(u32*)(buf + size - 8)
80021da8  1a 80 06 3c   lui  a2,0x801a
80021dac  00 58 c6 24   addiu a2,a2,0x5800             ; a2 = dst = 0x801A5800
80021db0  21 20 30 02   addu a0,s1,s0                  ; a0 = buf + OFF (Blockanfang)
80021db4  00 00 85 8c   lw   a1,0x0(a0)                ; a1 = LE32 decompressed_size
80021db8  fa 11 03 0c   jal  FUN_800c47e8
80021dbc  04 00 84 24   _addiu a0,a0,0x4               ; a0 = Blockanfang + 4 = Stream
; --- TIM in den VRAM ----------------------------------------------------------
80021dd4  1a 80 04 3c   lui  a0,0x801a
80021dd8  00 58 84 34   ori  a0,a0,0x5800              ; a0 = 0x801A5800
80021ddc  0b 80 10 3c   lui  s0,0x800b
80021de0  4c ca 10 26   addiu s0,s0,-0x35b4            ; s0 = 0x800ACA4C
80021de4  00 00 11 96   lhu  s1,0x0(s0)                ; alte (slot,clut_row) sichern
80021de8  15 00 02 34   ori  v0,zero,0x15
80021dec  9e 3b 01 0c   jal  FUN_8004ee78              ; TIM -> VRAM
80021df0  00 00 02 a6   _sh  v0,0x0(s0)                ; DELAY: [0x800ACA4C]=0x15, [0x800ACA4D]=0x00
80021df4  00 00 11 a6   sh   s1,0x0(s0)                ; beides wieder herstellen
```

Wichtige Nebenbefunde aus derselben Funktion:

* Der Vordergrund-Test laeuft **auch dann**, wenn der CD-Read uebersprungen wurde
  (`DAT_800aca38 & 0x20000000` = "Chunk liegt schon im RAM", @0x80021c48) — der Puffer
  `0x80190000` gilt weiter.
* Die Masken werden **vorher** geparst: `if ((DAT_800aca38 & 0x100000) == 0) FUN_800392d4();`
  (@0x80021c20 / @0x80021c28).
* `FUN_8001d600` (@0x8001d9d8) liest dieselbe Tabelle, startet aber nur den **asynchronen** Vorab-Read
  (Modus 2) beim Raumbetreten — es dekomprimiert **nicht**. Die Dekompression passiert immer und nur
  in `FUN_80021bbc`, also **bei jedem Kamera-Cut-Wechsel neu** (kein Cache).

### 3.3 FUN_8004ee78 — VRAM-Rechtecke, Bild UND CLUT

```
8004ee84  jal  OpenTIM
8004ee8c  jal  ReadTIM(&stack)
8004eea0  lbu  v1,0x0(0x800ACA4C)     ; slot = 0x15
8004eea4  lw   v0,0xc(s0)             ; TIM_IMAGE.prect
8004eea8  sll  a0,v1,0x6              ; x = slot * 64
8004eeac  sltiu v1,v1,0x10
8004eeb0  bne  v1,zero,+8
8004eeb8  addiu a0,a0,-0x400          ; slot >= 0x10 -> x -= 1024
8004eebc  sh   a0,0x0(v0)             ; *** prect->x  UEBERSCHRIEBEN ***
8004eecc  xori v0,v0,0x1
8004eed0  sll  v0,v0,0x8              ; y = (slot >= 0x10) ? 256 : 0
8004eed4  sh   v0,0x2(v1)             ; *** prect->y  UEBERSCHRIEBEN ***
8004eee0  jal  LoadImage(prect, paddr)
8004eef0  lh   v0,0x4(prect)          ; w (16-Bit-Einheiten)
8004ef0c  sra  v1,v1,0x6              ; slot += ceil(w/64)
8004ef18  sb   v0,0x0(0x800ACA4C)
8004ef1c  lw   v0,0x8(s0)             ; caddr
8004ef24  beq  v0,zero, ende          ; keine CLUT -> fertig
8004ef30  lbu  v0,-0x35b3(0x800b0000) ; DAT_800ACA4D (clut_row)
8004ef38  addiu v0,v0,0x1e0           ; y = clut_row + 480
8004ef3c  sh   v0,0x2(crect)          ; *** crect->y UEBERSCHRIEBEN, crect->x NICHT ***
8004ef48  jal  LoadImage(crect, caddr)
8004ef6c  sb   v0,-0x35b3(...)        ; clut_row += crect->h
```

Mit `slot = 0x15` und `clut_row = 0` (beide durch das `sh 0x0015` @0x80021df0 gesetzt — `sh` schreibt
*beide* Bytes, `0x800ACA4C = 0x15` und `0x800ACA4D = 0x00`):

| | Quelle | Wert |
|---|---|---|
| Bild-Rechteck x | **ueberschrieben** @0x8004eebc | `0x15*64 - 1024 = 320` |
| Bild-Rechteck y | **ueberschrieben** @0x8004eed4 | `256` |
| Bild-Rechteck w,h | aus dem TIM-Header | `128` Halbwords (= 256 Pixel bei 8 bpp) x `256` (bzw. 240 / 226) |
| CLUT-Rechteck x | **aus dem TIM-Header** (nicht angefasst) | `0` |
| CLUT-Rechteck y | **ueberschrieben** @0x8004ef3c | `0 + 480 = 480` |
| CLUT-Rechteck w,h | aus dem TIM-Header | `256 x 1` |

Da der TIM-Header selbst schon `CLUT (0,480,256,1)` sagt (gemessen: **359/359** Atlanten), ist die
CLUT-Ueberschreibung ein No-Op; die **Bild**-Koordinaten des TIM (`0,0`) werden dagegen **verworfen**.

⇒ **Atlas-Bild: VRAM (320,256), 256x256 (bzw. 256x240 / 256x226) 8-bit. CLUT: VRAM (0,480), 256 Eintraege.**

Beispiel-Header (entpackt, ROOM117 Cut 1): `magic=0x10 flags=0x09`,
CLUT-Block `len=524 (x=0,y=480,w=256,h=1)`, Bild-Block `len=65548 (x=0,y=0,w=128,h=256)`, Summe = 66080.

### 3.4 Verknuepfung mit TPage/CLUT der Masken (Bruecke zu Aufgabe A)

Der Masken-Zeichner `FUN_80039590` setzt den Zeichenmodus **hart**:

```
8003962c  01 00 06 34   ori  a2,zero,0x1
80039630  95 00 07 34   ori  a3,zero,0x95          ; <<< tpage = 0x95
80039634  16 a6 01 0c   jal  SetDrawMode(prim, dtd=1, dfe=1, tpage=0x95, NULL)
```

`0x95` decodiert als PSX-TPage: `tx = 0x95 & 0x0F = 5` → x = `5*64 = 320`;
`ty = (0x95>>4) & 1 = 1` → y = `256`; `abr = (0x95>>5) & 3 = 0`; `tp = (0x95>>7) & 3 = 1` = **8-bit CLUT**.
Das ist **bitgenau** das Rechteck, in das `FUN_8004ee78` den Atlas laedt — und `0x95 & 0x1F = 0x15`
ist derselbe Slot-Index, der @0x80021de8 gesetzt wird.

Die CLUT-ID der Sprites steht ebenfalls hart im Masken-Aufbau `FUN_800392d4`:

```
80039498  00 78 02 34   ori  v0,zero,0x7800
800394fc  00 00 22 a6   sh   v0,0x0(s1)            ; SPRT.clut (Feld +0x1A des 0x20-Prim-Blocks)
80039500  00 00 02 a6   sh   v0,0x0(s0)            ; zweiter Prim-Puffer
```

`0x7800` = `getClut(x=0, y=480)` (`clut = (y<<6) | (x>>4)` ⇒ `480*64 = 30720 = 0x7800`). Also
**exakt die CLUT-Position, die der Atlas-TIM mitbringt.**

Ebenfalls in `FUN_800392d4` belegt (Masken-Record; `a2` = Record-Anfang, `s2` = a2 + 10):

```
80039408  00 00 d7 90   lbu  s7,0x0(a2)      ; SPRT.u0 = record[0]   <- Spalte IM ATLAS
80039418  f7 ff 5e 92   lbu  s8,-0x9(s2)     ; SPRT.v0 = record[1]   <- Zeile  IM ATLAS
80039400  f8 ff 42 92   lbu  v0,-0x8(s2)     ; record[2] + group.destX = SPRT.x0
80039414  f9 ff 42 92   lbu  v0,-0x7(s2)     ; record[3] + group.destY = SPRT.y0
800393f0  fa ff 42 96   lhu  v0,-0x6(s2)     ; record[4..5] = depth -> OT-Index
80039404  fc ff 44 96   lhu  a0,-0x4(s2)     ; record[6..7]: obere 4 Bit != 0 -> w=h=(nibble<<3), Record 8 B
8003942c  fe ff 54 96   lhu  s4,-0x2(s2)     ;   sonst: record[8..9] = w, record[10..11] = h, Record 12 B
80039430  00 00 53 96   lhu  s3,0x0(s2)
800393b0  80 00 0b 34   ori  t3,zero,0x80    ; r0=g0=b0=0x80 (neutral), gespeichert @0x800394ac-c0
```

⇒ **`srcX/srcY` eines Masken-Records sind u0/v0 IM SLD-VORDERGRUNDATLAS**, nicht im Hintergrundbild.
Der Kommentar in `re15_port/include/re15_pri.h` ("sample position in BG image (256-wide TIM)") ist
insofern irrefuehrend und sollte praezisiert werden.

**Korrektur zu Aufgabe A:** das u16 bei `group + 2` (im Port `baseRaw`, kommentiert als
"likely TPage / texture bank") wird von `FUN_800392d4` **nie gelesen** — im gesamten Funktionskoerper
`0x800392d4..0x8003958c` existiert kein Load mit Offset `0x2` auf den Gruppenzeiger (`t5` laeuft mit
`addiu t5,t5,0x8` @0x80039558, gelesen wird nur `lhu v1,0x0(t5)` @0x8003952c = die Maskenzahl; `t7`
laeuft ebenfalls mit `addiu t7,t7,0x8` @0x80039540 und liefert `-0x2(t7)` = destX und `0x0(t7)` = destY).
Die TPage ist die Konstante `0x95`. Dass `group+2` eine TPage sei, ist **NICHT BELEGT**.

### 3.5 Transparenz — Index 0

Auf PSX ist ein Texel transparent, wenn der CLUT-**Eintrag** `0x0000` ist (nicht "Index 0"). Gemessen
ueber alle 359 Atlanten: `clut[0] == 0x0000` in **359/359**, und `0x0000` kommt **in keinem** Atlas ein
zweites Mal vor. Die Abkuerzung im Port (`if (ix == 0) transparent`, `bg_pc.c`) ist damit fuer die
realen Daten **byte-aequivalent** — jetzt belegt statt angenommen.

---

## 4. Wie die Engine den SLD-Block findet (die zentrale Frage)

**Es wird nicht gesucht.** Die Regel ist deterministisch:

```
size      = u16 tab[stage][room*0x20 + cut*2]        (tab = *(u32*)0x800B52C8, gesetzt vom Stage-Overlay)
has_fg    = *(u32*)(chunk + size - 4)                 (@0x80021d6c)  != 0 -> Vordergrund vorhanden
sld_off   = *(u32*)(chunk + size - 8)                 (@0x80021da4)  Byte-Offset relativ Chunk-Anfang
dec_size  = *(u32*)(chunk + sld_off)                  (@0x80021db4)
tim       = FUN_800c47e8(chunk + sld_off + 4, dec_size, 0x801A5800)
```

`size` ist die **Byte**-Laenge der fuer diesen Cut von der CD gelesenen Daten (dieselbe Groesse geht
@0x80021cb8 als `DAT_800be574` in den Lader; bei Voll-Datei-Loads steht dort nachweislich die
Dateigroesse, z.B. `0x219B0` = STAGE1.BIN). Ein Cut belegt immer `0x20` Sektoren = **64 KB**
(`sll a3,a3,0x5` @0x80021cd8; ROOM102.BSS = 851968 = 13 x 64 KB), der Rest ist Padding.

### 4.1 Die Groessen-Tabelle — Adressen aller sechs Stages

Der Zeiger `DAT_800b52c8` wird ausschliesslich vom jeweiligen Stage-Overlay gesetzt
(Instruktionswort `ac 22 52 c8` = `sw v0,0x52c8(at)`):

| Stage | Store-Instruktion | Tabelle | Datei-Offset in `STAGEn.BIN` |
|---|---|---|---|
| 1 | `@0x8011e97c` | `0x8011EAE4` | `0x0EAE4` |
| 2 | `@0x80116fd4` | `0x8011713C` | `0x0713C` |
| 3 | `@0x8011cfa0` | `0x8011D0F8` | `0x0D0F8` |
| 4 | `@0x80118458` | `0x80118590` | `0x08590` |
| 5 | `@0x8011ddc8` | `0x8011DF18` | `0x0DF18` |
| 6 | `@0x80101e0c` | `0x80101E3C` | `0x01E3C` |

(Belegt per Direkt-Disasm, z.B. STAGE1: `8011e970 lui v0,0x8012` / `8011e974 addiu v0,v0,-5404` (=0x8011eae4) /
`8011e978 lui at,0x800b` / `8011e97c sw v0,21192(at)` (=0x800b52c8).)

Zeilenformat: 16 x u16 pro Raum (`room*0x20`), Index = **untere 8 Bit der Raumnummer**
(ROOM102 → 2, ROOM11C → 0x1C). Nicht benutzte Cuts stehen auf `8` (= nur der Trailer).
Beispiel STAGE1, Raum 2 (`0x8011EB24`):

```
[42016, 42840, 39764, 42028, 30628, 38208, 35744, 31968, 27968, 34824, 37332, 22256, 33980, 8, 8, 8]
```

Gegenprobe in `ROOM102.BSS`, Cut 0: das Wort bei Chunk-Offset `42008` (= 42016-8) ist `31672`, das
Wort bei `42012` ist `1` — und `31672` ist genau der SLD-Offset, den der Brute-Force-Sweep gefunden
hat. Cut 1: `42832 -> 25164`, Flag `1`, size = 42840. Passt.

### 4.2 Validierung ueber ALLE Daten (6 Stages, 120 BSS-Dateien, 1119 Cuts)

| Ergebnis | Anzahl |
|---|---|
| Cuts gesamt | 1119 |
| `has_fg != 0` → SLD dekodiert zu gueltigem TIM (`magic 0x10`, `flags 0x09`, Blocklaengen == dec_size) | **359 / 359** |
| davon: Offset identisch mit dem Brute-Force-Sweep, wo dieser gelaufen war | **247 / 247** |
| davon: vom Sweep **nie gescannt** (`"sld": null`) — jetzt gefunden | **112** |
| `has_fg == 0` und Sweep hat gescannt und nichts gefunden | **593 / 593** |
| Widersprueche | **0** |

Entpackte Groessen: `66080` (0x10220, 256 Zeilen) x320, `61984` (0xF220, 240 Zeilen) x38,
`58400` (0xE420, 226 Zeilen) x1 (ROOM404 Cut 5).

### 4.3 Zwei Korrekturen an der bisherigen Messung

1. **"114 Cuts: Masken, aber KEIN SLD-Block im BSS (nur STAGE5)" ist ein Messartefakt.**
   In `sweep.json` steht fuer diese Cuts `"sld": null` (nie gescannt), nicht `[]`. Mit der
   Trailer-Regel gilt game-weit: **Cuts mit Masken und ohne Atlas = 0 von 1119.**
   Umgekehrt gibt es **119** Cuts mit Atlas und *ohne* Masken (u.a. ROOM1220, ROOM3040, ROOM3050,
   ROOM3090, ROOM30A0, ROOM30E0, ROOM4040, ROOM4080, ROOM40A0) — dort wird der Atlas geladen, aber
   nie gesampelt.
2. **Ein einziger Datenfehler im Auslieferungsstand:** STAGE3, Raum 5 (ROOM3050), Cut 14. Die
   Tabelle sagt `size = 30668`, der echte Trailer liegt aber bei Chunk-Offset `45892` (size 45900).
   `30668` ist zufaellig genau der SLD-Offset dieses Cuts — das Build-Werkzeug hat offensichtlich den
   falschen Wert in die Zelle geschrieben. Konsequenz **im echten Spiel**: bei `buf+30664` steht
   `0x00000000`, die Engine liest also `has_fg = 0` und zeigt fuer diesen Cut **keinen Vordergrund**
   (und laedt einen abgeschnittenen Hintergrund). Der Port muss diesen Fehler **nachbilden**, nicht
   reparieren — die Trailer-Regel tut das automatisch.

---

## 5. Frage 4 — Wie entscheidet die Engine, ob ein Cut Vordergrund hat, und was sonst?

* **Entscheidung:** einzig und allein `*(u32*)(chunk + size - 4) != 0` (@0x80021d6c / @0x80021d74).
  Kein Flag im Chunk-Header, keine Groessenheuristik, kein Suchen. Ist der Wert 0, wird der
  gesamte Block uebersprungen (`beq v0,zero,LAB_80021df8`) — weder `FUN_800c47e8` noch
  `FUN_8004ee78` laufen.
* **Folge fuer den VRAM:** TPage `0x15` (VRAM 320,256) behaelt den Inhalt des **vorherigen** Cuts /
  Raums. Das ist unkritisch, weil ein Cut ohne Atlas nach §4.2 nie Masken hat, die daraus sampeln
  wuerden. (Ausnahme ausserhalb des Spielablaufs: der CONFIG-Screen `FUN_8002dfb0` laedt
  `Config.tim` in denselben Slot, `_DAT_800aca4c = 0x15`.)
* **Masken-Seite:** `FUN_800392d4` liest den Cut-Record `+0x1C` als Zeiger; ist das erste Wort
  `0xFFFFFFFF`, setzt es `*DAT_800ac778 = 0` (null Masken). Beide Wege sind unabhaengig voneinander —
  in den echten Daten aber immer konsistent (0 Faelle Masken-ohne-Atlas).

---

## 6. Was das fuer den Port bedeutet

### 6.1 Der Ist-Zustand ist nachweislich lueckenhaft

`re15_port/platform/pc/src/bg_pc.c:63 re15_pri_load_cut_atlas()` laedt eine **vorextrahierte**
`BSS/ROOM####/PRI##.TIM`. Bestandsaufnahme in `re15_port/shared_assets/PSX`:

* benoetigt (nur fuer die 156 vorhandenen Raum-Verzeichnisse): **494** Dateien
* vorhanden: **416** → **78 fehlen** (= 39 Cuts x 2 Varianten)
* Korrelation ist perfekt: **alle 416 vorhandenen** Atlanten sind `66080` Bytes gross; **alle 78
  fehlenden** sind `61984` (38 Cuts) bzw. `58400` (1 Cut). Das Extraktionswerkzeug hat also nur die
  256-Zeilen-Variante erwischt. Betroffen: ROOM1020/1021 (Cuts 0-10,12), ROOM1030/1031 (0-7,9-12),
  ROOM1040/1041 (1,2,3,4,6), ROOM1070/1071 (0-7), ROOM1220/1221 (0), ROOM4040/4041 (5).
  *(Welches Werkzeug die `PRI##.TIM` erzeugt hat, ist im Repo nicht auffindbar —
  `BssExtractor.java:86` schreibt `<base>_pri.tim`, nicht `PRI##.TIM`. Ursache der Selektion:
  **NICHT BELEGT**, die Korrelation dagegen ist gemessen.)*

### 6.2 Die Laufzeit-Loesung braucht keine neuen Assets

Beides liegt bereits byte-identisch in `shared_assets` (verifiziert per sha256):
`BSS/ROOM####/BG##.BSS` sind die vollen **64-KB-Chunks** (ROOM1020/BG00.BSS ==
ROOM102.BSS[0:0x10000]), und `BIN/STAGE{1..6}.BIN` sind identisch zu `info/Re1.5/PSX/BIN/`.

```c
/* Laufzeit-Rezept, alle Konstanten belegt:
   Tabellen-Adressen §4.1, Trailer @0x80021d6c/@0x80021da4, Codec DEBUG.BIN@0x47E8 */
static const uint32_t k_bss_size_tab[7] = {          /* [stage] -> Overlay-Adresse */
    0, 0x8011EAE4, 0x8011713C, 0x8011D0F8, 0x80118590, 0x8011DF18, 0x80101E3C };

int stage = (room_id >> 12) & 0xF;                    /* ROOM1020 -> 1 */
int room  = (room_id >>  4) & 0xFF;                   /* ROOM1020 -> 0x02 */
uint32_t off = k_bss_size_tab[stage] - 0x80100000 + room*0x20 + cut*2;
uint16_t size = le16(stage_bin + off);                /* aus STAGE<stage>.BIN */

if (size >= 0x100 && size <= 0x10000 &&
    le32(chunk + size - 4) != 0) {                    /* @0x80021d6c  has_fg */
    uint32_t so  = le32(chunk + size - 8);            /* @0x80021da4  sld_offset */
    uint32_t dsz = le32(chunk + so);                  /* @0x80021db4  decompressed size */
    sld_decompress(chunk + so + 4, dsz, atlas);       /* FUN_800c47e8 */
    /* atlas = Sony-TIM, 8 bpp + CLUT; Bild -> "TPage 0x15", CLUT -> (0,480) */
}
```

Der C-Codec ist eine 1:1-Umsetzung der Disassembly aus §2 (es gibt bisher **keinen** SLD-Decoder in C
im Port — nur `src/main/java/de/re15/extractors/bss/SldDecoder.java`).

### 6.3 Kleinere Praezisierungen, die dabei mitgenommen gehoeren

* `re15_pri.h`: `srcX/srcY` = u0/v0 **im Vordergrundatlas** (nicht "in the BG image").
  Belege: `lbu s7,0x0(a2)` @0x80039408 / `lbu s8,-0x9(s2)` @0x80039418 + `SetDrawMode(...,0x95)` @0x80039630.
* `re15_pri.h`: `baseRaw` (group+2) ist im Zeichenpfad **tot** — die TPage ist die Konstante `0x95`
  (§3.4). Das "likely TPage / texture bank" sollte raus oder als unbenutzt markiert werden.
* `bg_pc.c`: der Fallback "Datei fehlt → Overdraw komplett AUS" wird mit dem Laufzeit-Pfad
  gegenstandslos; das Original schaltet den Overdraw **nur** ueber `has_fg == 0` ab.

---

## 7. Offene Punkte

1. **Byte-Gegenprobe gegen echten PSX-VRAM** wurde in dieser Sitzung nicht wiederholt. Der
   Java-Kommentar behauptet sie fuer ROOM1170 Cuts 1/2/4; meine Verifikation ist datenseitig
   (359/359 struktur- und laengenkonsistente TIMs, exakte `dp == size`-Landung) und
   disassembly-seitig (Instruktion fuer Instruktion). Ein Savestate-VRAM-Dump bei `(320,256)`
   waere die letzte Stufe.
2. **Wer die vorhandenen `PRI##.TIM` erzeugt hat**, ist im Repo nicht auffindbar → Ursache der
   66080-Selektion **NICHT BELEGT** (die Korrelation 416/416 vs. 78/78 ist gemessen).
3. **Die Zeilenzahl der Groessen-Tabellen** wurde aus dem jeweils naechsten Overlay-Symbol
   abgeleitet (z.B. STAGE1: `0x8011EAE4..0x8011EFC4` = 39 Zeilen) — das ist eine Inferenz, kein
   Beleg. Praktisch irrelevant: die vier Raeume mit Null-Zeile (ROOM127, ROOM20E, ROOM40E, ROOM605)
   haben 4-Byte-Stub-BSS-Dateien und damit 0 Cuts.
4. **`FUN_800c62a0 / 800c6630 / 800c69bc / 800c6c58 / 800c6ca0`** (weitere DEBUG.BIN-Routinen, aus
   der EXE aufgerufen) sind nicht untersucht — sie gehoeren nicht zum Vordergrund-Pfad, koennten aber
   weitere "fehlende" Engine-Teile enthalten.
5. **Warum `DAT_800aca4d` (CLUT-Zeile) beim Atlas-Load auf 0 steht**, ist durch das `sh 0x0015`
   @0x80021df0 belegt und damit unabhaengig vom Raumzustand — aber es waere gut, das gegen einen
   Savestate zu bestaetigen, falls je eine CLUT ausserhalb von y=480 auftaucht.

---

## Anhang A — Reproduktion

```bash
# Codec-Disassembly (DEBUG.BIN laedt nach 0x800C0000, das Skript kennt nur 0x80100000):
sed "s/^OVERLAY_LOAD = 0x80100000$/OVERLAY_LOAD = 0x800c0000/" \
  .claude/skills/re15-psx-disasm/scripts/re15_disasm.py > /tmp/dbgdis.py
RE_OVERLAY_DIR=$PWD/info/Re1.5/PSX/BIN python /tmp/dbgdis.py dis 0x800c47e8 68 --bin DEBUG.BIN

# Groessen-Tabelle STAGE1, Raeume 0..5:
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py \
  read 0x8011eae4 16 --w 2 --rows 6 --rowstride 0x20 --bin STAGE1.BIN

# Trailer-Regel gegen alle 1119 Cuts (Tabellenadressen aus §4.1, sld.py aus diesem Verzeichnis).
```

## Anhang B — Adressregister

| Adresse / Offset | Bedeutung |
|---|---|
| `DEBUG.BIN + 0x47E8` = `0x800C47E8` | SLD-Dekompressor (68 Instr., Ende `0x800C48F4`) |
| `DEBUG.BIN + 0x02C0` = `0x800C02C0` | uebernommener CD-Datei-Lader (Patch-Ziel) |
| `0x8001311c` | laedt Datei-Index 7 (DEBUG.BIN) nach `0x800C0000` und patcht `0x80013b7c` |
| `0x8006F43C` | CD-Datei-Tabelle, Stride 8: `{u32 size; u16 lba_lo; u8 lba_hi; u8 pad}` |
| `0x80013B60` | CD-Datei-Lader (a0=index, a1=dest, a2=mode) |
| `0x80013C50` | CD-Read mit vorgesetzten `DAT_800be572/574/57c` (BSS-Cut-Pfad) |
| `0x80021BBC` | Cut-Hintergrund laden/anzeigen — der einzige SLD-Aufrufer |
| `0x80021D6C` | `lw v0,-0x4(at)` — has_fg |
| `0x80021DA4` | `lw s0,-0x8(at)` — sld_offset |
| `0x80021DB8` | `jal FUN_800c47e8` |
| `0x80021DE8` | `ori v0,zero,0x15` — VRAM-Slot 0x15 |
| `0x8004EE78` | TIM → VRAM (Slot-Allokator); `prect->x/y` @0x8004eebc/@0x8004eed4, `crect->y` @0x8004ef3c |
| `0x800392D4` | Masken-Records → SPRT-Prims; `clut = 0x7800` @0x80039498/@0x800394fc |
| `0x80039590` | Masken zeichnen; `SetDrawMode(tpage=0x95)` @0x80039630; OT-Index = depth @0x80039658 |
| `0x800B52C8` | Zeiger auf die u16-Groessen-Tabelle des aktiven Stage-Overlays |
| `0x800B0FE0/FE2/FE4` | Stage / Raum-Index / Cut-Index |
| `0x800ACA4C / 4D` | VRAM-Slot (TPage-Index) / CLUT-Zeile des TIM-Allokators |
| `0x80190000` | BSS-Chunk-Puffer (64 KB) |
| `0x801A5800` | Ziel des entpackten Atlas-TIM |
| `0x800ACA38` | Flags: `0x100000` = Masken-Parse ueberspringen, `0x20000000` = CD-Read ueberspringen |

---

## Gegenpruefung

Datum: 2026-09-03 · Gegenpruefer-Agent · Methode: jede zitierte Adresse eigenstaendig aus
`info/Re1.5/PSX.EXE` / `info/Re1.5/PSX/BIN/*.BIN` als Rohbytes gezogen und mit einem eigenen
MIPS-R3000-Dekoder disassembliert; jede Datenbehauptung mit einem eigenen Sweep-Skript
nachgerechnet (unabhaengig von `sweep.json` und von den Zahlen des Berichts).

### Ergebnis in einem Satz

Der **Mechanismus** haelt vollstaendig: Codec-Ort, Codec-Semantik, Aufrufer, VRAM-Ziel, CLUT,
TPage, u0/v0-Herkunft, Trailer-Regel, Groessen-Tabellen. Es fallen **zwei Zahlen-/Formulierungs-
fehler** (Behauptung 12 und 17) und **eine Reichweiten-Ueberdehnung** (Behauptung 8).

### Was ich Byte fuer Byte bestaetigen konnte

| # | Nachgeprueft mit | Ergebnis |
|---|---|---|
| 1 | `PSX.EXE[0x18]=00 00 01 80`, `[0x1C]=00 f0 0a 00`; Disasm `0x8001311c..0x8001315c` (roh: `34040007 / 3c05800c / 0c004ed8 / 34060000`, `DAT_80013158 = 080300b0` = `j 0x800C02C0`); CD-Tabelle `PSX.EXE+0x5FC3C` Eintrag 7 = `{0x40000, LBA 339}`; `sha256(DEBUG.BIN)=6780c59b…0b3dfe`, 262144 B. Zusaetzlich selbst ermittelt: `jal 0x8001311c` steht **genau einmal** (@0x80011704 in `FUN_800116f4`), und `jal 0x800116f4` steht **genau einmal** (@0x800212bc, Boot). Der Load ist unbedingt. | **bestaetigt (+ Boot-Pfad zusaetzlich belegt)** |
| 2 | Alle 68 Instruktionen `DEBUG.BIN[0x47E8..0x48F7]` roh gedumpt — **byte-identisch** zum Listing im Bericht. Schluesselwoerter selbst dekodiert: `15800005`=bne t4,zero,0x800c4810; `340c0080`=ori t4,zero,0x80; `08031239`=j 0x800c48e4; `000c6042`=srl t4,t4,1 (2x); `00025503`=sra t2,v0,20; `00025403`=sra t2,v0,16; `24490002`=addiu t1,v0,2; `24690003`=addiu t1,v1,3; `0105102a`=slt v0,t0,a1. Delay-Slot-Semantik (`addu v0,a0,t3` @0x800c47fc vor dem Flagbyte-Load) und die Zeigerumrechnung im Kopierblock (`v0=dp+off` @0x800c48ac **vor** `t2=dp+len` @0x800c48b4) gegen `sld.py` durchgerechnet: gleich. Langform-Offset: MIPS trunkiert auf 16 Bit (`sll 16`/`sra 16`), der Wertebereich `-8192..-1` passt hinein, deckungsgleich mit Pythons `|` auf negativen Zahlen. | **bestaetigt** |
| 3 | `80021da8 3c06801a` / `80021dac 24c65800` / `80021db4 8c850000` / `80021db8 0c0311fa` / `80021dbc 24840004` | **bestaetigt** |
| 4 | Eigene Bytemuster-Suche `0c0311fa` ueber `PSX.EXE` + alle 8 Dateien in `PSX/BIN/`: **ein** Treffer, `PSX.EXE+0x125B8` = `0x80021db8`. `ghidra1_V2.txt:574972` zeigt `XREF[1]: FUN_80021bbc:80021db8(c)`. | **bestaetigt** |
| 5 | `FUN_8004ee78` selbst disassembliert: `8004ee98 addiu s1,s1,-13748` = `0x800ACA4C`; `8004eea8 sll a0,v1,6` (0x15*64=1344), `8004eeac sltiu v1,v1,16` -> 0, `8004eeb8 addiu a0,a0,-1024` -> **320**, `8004eebc sh a0,0(prect)`; `8004eecc xori` + `8004eed0 sll v0,v0,8` -> **256**, `8004eed4 sh v0,2(prect)`; CLUT: `8004ef30 lbu v0,-13747(0x800b0000)` (=0x800ACA4D, vom Aufrufer auf 0 gesetzt) + `8004ef38 addiu v0,v0,480` + `8004ef3c sh v0,2(crect)` -> **480**; `crect->x` wird in der ganzen Funktion (0x8004ee78..0x8004ef8c) **nie** geschrieben. Aufrufer: `80021de8 ori v0,zero,0x15` + Delay-Slot `80021df0 sh v0,0(0x800ACA4C)`, danach `80021df4 sh s1,0(…)` (Restore). Datenprobe (eigener Sweep, alle 359): CLUT-Rect **359/359 = (0,480,256,1)**, Bild-Rect (0,0,128,256) x320, (0,0,128,240) x38, (0,0,128,226) x1, `flags==0x09` 359/359. | **bestaetigt** |
| 6 | `80039630 34070095` = ori a3,zero,0x95; `80039634 0c01a616` = jal `0x80069858`; `ghidra1_V2.txt:212264` benennt `0x80069858` als **`SetDrawMode`** und listet `FUN_80039590:80039634(c)` unter den XREFs; `sw zero,16(sp)` = `tw=NULL`. `0x95` = tx 5 -> x 320, ty 1 -> y 256, tp 1 -> 8 bpp (Gegenprobe: `GetTPage(1,0,320,256)` = 0x95). `80039498 34027800` = ori v0,zero,0x7800, gespeichert `800394fc`/`80039500 sh v0,0(s1)/0(s0)`. Prim-Layout gegengerechnet: `s0` zeigt auf Block+0x1A, daher ergeben die Stores auf -10/-9/-8, -6, -4, -2, -1, 0, +2, +4 exakt `SPRT{r,g,b @+4..6; x0,y0 @+8,10; u0,v0 @+12,13; clut @+14; w,h @+16,18}` ab Block+12, mit `DR_MODE` in Block+0..11 — und `FUN_80039590` setzt `SetDrawMode` auf `s0` und `AddPrim` auf `s1=s0+12`. | **bestaetigt** |
| 7 | `80039408 90d70000` = lbu s7,0(a2); `80039418 925efff7` = lbu fp(s8),-9(s2) mit `800393d4 addiu s2,a2,10`, also `s2-9 = a2+1`; abgelegt `800394dc a217fffe` = sb s7,-2(s0) und `800394e0 a21effff` = sb fp,-1(s0) = SPRT `u0`/`v0`. Zusammen mit #6 (tpage 0x95) sind `record[0]/record[1]` damit **Atlas-Koordinaten**. Der Kommentar in `re15_port/include/re15_pri.h` ("sample position in BG image (256-wide TIM)") ist tatsaechlich falsch. | **bestaetigt** |
| 8 | `800393b4 addiu t7,t5,6` (selbst gefunden), daher `lhu v1,-2(t7)`=Gruppe+4, `lhu v1,0(t7)`=Gruppe+6, `lhu v0/v1,0(t5)`=Gruppe+0; beide Zeiger `+= 8`. Im gesamten Bereich `0x800392d4..0x8003958c` existiert kein Load auf Gruppe+2. | **bestaetigt (mit Einschraenkung, s.u.)** |
| 9 | `80021d64 3c018019` / `80021d68 00410821` / `80021d6c 8c22fffc` / `80021d74 10400020` (-> 0x80021df8) / `80021da4 8c30fff8` / `80021d9c lui s1,0x8019` / `80021db0 02302021` / `80021db4 8c850000`. Datenprobe: `ROOM102.BSS[42008]=31672`, `[42012]=1`, Tabellenwert `size=42016`. Eigener Sweep: `has_fg` ist ueber **alle 1119 Cuts nur 0 oder 1** (760 / 359), nie ein anderer Wert. | **bestaetigt** |
| 10 | `80021c40 8e3152c8` = lw s1,21192(0x800b0000) = `DAT_800b52c8`; `80021c94 sll v1,a3,1`; `80021c98 sll v0,a1,5`; `80021cac lhu v1,0(v1)`; `80021cb8 ac23e574` = sw v1,-6796(0x800c0000) = `0x800BE574`; `80021cd8 sll a3,a3,5`; `80021d20 addu v0,v0,a3`. Room/Cut stammen aus `lh 4066/4068(0x800b0000)`. | **bestaetigt** |
| 11 | Eigene Bytemuster-Suche `c85222ac` (`sw v0,0x52c8(at)`) in den sechs `STAGEn.BIN`: **je genau ein** Treffer, bei Datei-Offset = Adresse-0x80100000 -> 0x8011e97c / 0x80116fd4 / 0x8011cfa0 / 0x80118458 / 0x8011ddc8 / 0x80101e0c. Die lui/addiu-Paare davor ergeben **0x8011EAE4 / 0x8011713C / 0x8011D0F8 / 0x80118590 / 0x8011DF18 / 0x80101E3C** — alle sechs identisch zum Bericht. Gegenprobe STAGE1 Raum 2 (`0x8011EB24`): `[42016, 42840, 39764, 42028, 30628, 38208, 35744, 31968, 27968, 34824, 37332, 22256, 33980, 8, 8, 8]`; die ersten 13 Werte liefern in `ROOM102.BSS` 13/13 gueltige Trailer. Die STAGE-Overlays haben **keinen** 0x800-Header (bestaetigt durch die Treffer-Offsets). | **bestaetigt** |
| 12 | Eigener Sweep ueber alle 120 BSS-Dateien: **1119 Cuts**, 359 mit `has_fg!=0`, alle 359 dekodieren fehlerfrei; `8+clut_len+img_len == dsz` **359/359**; `flags==0x09` **359/359**; entpackte Groessen **66080 x320, 61984 x38, 58400 x1**. | **Kern bestaetigt — Zahlen s. "Was nicht standhaelt"** |
| 13 | Eigene Kreuztabelle (Maskenzahl direkt aus den RDTs: `nCut=hdr[1]`, Cut-Tabelle @0x24, Record 32 B, `+0x1C` -> Gruppenkopf, Masken = Summe der Gruppen-`u16`): **(has_fg, masks>0) = 240 · (has_fg, masks=0) = 119 · (kein fg, masks=0) = 735 · (kein fg, masks>0) = 0 · 25 Cuts ohne RDT-Gegenstueck** — Ziffer fuer Ziffer wie im Bericht. `ROOM503`: genau die Cuts 1,2,3,4 tragen Masken (25/21/19/14) und genau diese vier haben `has_fg=1`. In `sweep.json` steht fuer sie `"sld": null`, die alte Kategorie "114 Masken ohne Atlas" war also wirklich ein Messartefakt. | **bestaetigt** |
| 14 | `STAGE3.BIN` Zeile Raum 5 = `[…, 30668, 58364]`, Eintrag 14 = **30668**. `ROOM305.BSS` Cut 14, Bytes 30656..30671 = `c6 b6 db 6c | 58 61 ff eb | 00 00 00 00 | 20 02 01 00`, also Wort @30664 = **0** = `has_fg`. Echter Trailer @45892 = 30668 / @45896 = 1; der SLD-Block @30668 entpackt zu 66080 B mit TIM-Magic `10 00 00 00 09 00 00 00`, und der Strom endet bei 45891, also **1 Byte vor** dem echten Trailer. | **bestaetigt** |
| 15 | Eigene Auswertung aller 359 CLUT-Bloecke: `clut[0]==0` in 359/359, Anzahl Null-Eintraege **exakt 1** in 359/359. | **bestaetigt** |
| 16 | `sha256(shared_assets/PSX/BSS/ROOM1020/BG00.BSS) == sha256(ROOM102.BSS[0:0x10000]) = 04ff1dec1444eb4d…`; `ROOM1021/BG12.BSS == ROOM102.BSS[12*0x10000:13*0x10000] = 6594361064…`; `STAGE1..6.BIN` in `shared_assets` **6/6 hash-gleich** zum Original. | **bestaetigt** |
| 17 | Eigener Abgleich: **156** Raumverzeichnisse, **494** benoetigte `PRI##.TIM`, **416** vorhanden (alle **66080 B**), **78** fehlend. Fehlende Cuts exakt: ROOM1020/1021 `[0..10,12]`, ROOM1030/1031 `[0..7,9,10,11,12]`, ROOM1040/1041 `[1,2,3,4,6]`, ROOM1070/1071 `[0..7]`, ROOM1220/1221 `[0]`, ROOM4040/4041 `[5]`. | **bestaetigt — Formulierung s.u.** |
| 18 | `80021c34..80021c48`: `v1 = DAT_800aca38`, `and v0,v1,0x20000000`, `beq v0,zero,0x80021c68`; der Zweig mit gesetztem Flag loescht es und macht `80021c60 j 0x80021d38` — und `0x80021d38 < 0x80021d5c` (Trailer-Test). Also laeuft die Dekompression auch bei uebersprungenem CD-Read. `FUN_8001d600 @0x8001d9d8 8ca552c8` = lw a1,21192(0x800b0000) — dieselbe Tabelle; ruft den Codec nicht (nur ein `jal`-Treffer game-weit, s. #4). | **bestaetigt** |
| 19 | Der `beq` @0x80021d74 ueberspringt Dekompression **und** VRAM-Load; im Pfad steht kein Clear von VRAM(320,256). `RE_15_Quellcode_V2/FUN_8002dfb0.c` Zeile 16/17: `_DAT_800aca4c = 0x15;` `FUN_8004ee78(&DAT_80198000);` (Config.tim) — derselbe Slot. Datenlage 0/1119 (s. #13) stuetzt das "unkritisch". | **bestaetigt** |
| 20 | Instrumentierter Eigen-Durchlauf ueber alle 359: **0** Vorwaertsreferenzen, kleinster Offset **-8191**, **0** Faelle `dp+len > size`, `dp == size` in **359/359**, Abstand Stream-Ende zum Trailer in {0,1,2,3} (96/91/97/75). | **bestaetigt** |
| 21 | STAGE1: naechstes Symbol `0x8011EFC4` (lui/addiu @0x8011e980/84, Store nach `0x800ac998` @0x8011e98c), Spanne 0x4E0 = **39** Zeilen; `ROOM127.BSS`, `ROOM20E.BSS`, `ROOM40E.BSS`, `ROOM605.BSS` sind alle **4 Byte** gross. Die Ableitung bleibt indirekt — der Bericht kennzeichnet sie korrekt als *teilbelegt*. | **bestaetigt (als teilbelegt)** |
| 22 | Repo-weite Suche nach Erzeugern von `PRI%02d.TIM`: es gibt nur **Leser** (`re15_port/platform/pc/src/bg_pc.c:70`, `re15_port/platform/psx/src/pri_psx.c:51`, dazu die Kopien unter `psx_dev/`). `BssExtractor.java:86/89` schreibt `<base>_pri.tim`. Kein Werkzeug im Baum schreibt den vom Port erwarteten Namen. | **"NICHT BELEGT" ist korrekt so** |

### Zusaetzlicher, unabhaengiger Beweis (im Bericht nicht enthalten)

Ich habe die **Kette Tabelle -> Trailer -> Codec** gegen die bereits extrahierten Dateien gehalten:
40 zufaellig gezogene Atlas-Cuts, davon 19 mit vorhandener `PRI##.TIM` — **19/19 byte-identisch**
(`decoded == PRI##.TIM`, 0 Abweichungen). Damit ist die Laufzeit-Rekonstruktion nicht nur
strukturell, sondern gegen den Ist-Bestand des Ports byte-verifiziert.

Zweiter Zusatzbefund zu Behauptung 8: das nie gelesene `u16` bei **Gruppe+2** ist keine TPage,
sondern eine **CLUT-ID**. Ueber alle 206 RDTs (2750 Gruppen) steht dort **2712x `0x7800`** —
genau die Konstante, die die Engine @0x80039498 hart einsetzt — sowie 26x `0x7C00`, 8x `0x7C10`,
2x `0x8080`, 2x `0x8088`. Die Engine ignoriert das Feld und benutzt in **allen** Faellen `0x7800`.
Der Port-Kommentar in `re15_pri.h` ("`u16 baseRaw` (likely TPage / texture bank)") ist damit
doppelt falsch: falsche Bedeutung **und** irrelevant fuer die Wiedergabe.

### Was NICHT standhaelt

**(a) Behauptung 12 — "0 Widersprueche" ist falsch; es ist genau 1.**
Die Kreuzpruefung der Trailer-Regel gegen die `sld`-Felder in `sweep.json` (nur die tatsaechlich
gescannten Cuts, auf eindeutige `(stage, raum, cut)` dedupliziert; 842 eindeutige Cuts) ergibt:

```
Sweep fand Atlas  / Trailer sagt fg        : 247
Sweep fand nichts / Trailer sagt kein fg   : 594
Sweep fand Atlas  / Trailer sagt KEIN fg   :   1   <-- Widerspruch
Sweep fand nichts / Trailer sagt fg        :   0
```

Der eine Widerspruch ist `ROOM305` Cut 14 (in `sweep.json` unter `ROOM3050` **und** `ROOM3051`,
also 2 RDT-Zeilen auf 1 BSS-Cut) — genau der Fall, den der Bericht unter Behauptung **14**
selbst als korrupten Tabelleneintrag beschreibt. Behauptung 12 haette lauten muessen
"1 Widerspruch, und zwar der aus Paragraph 14"; "0 Widersprueche" ist als absolute Aussage
widerlegt.

Zweitens die Zahl **593**: ich zaehle **594** gescannte Cuts mit `has_fg==0`, in denen der alte
Sweep ebenfalls nichts fand (bzw. **595** gescannte `has_fg==0`-Cuts insgesamt, inklusive
ROOM305 Cut 14). Damit stimmt auch die Restrechnung nicht: es sind **165** ungescannte
`has_fg==0`-Cuts, nicht 167. Die Zahl **112 zusaetzlich gefundene Atlanten** (359 - 247) ist
dagegen korrekt.

**(b) Behauptung 17 — Einheitenfehler im selben Satz.**
"alle 78 fehlenden waeren 61984 (38 Cuts) bzw. 58400 (1 Cut) Bytes": 38 + 1 = 39, nicht 78.
Gemessen sind es **76 fehlende Dateien a 61984 B und 2 a 58400 B** — die 39 sind *eindeutige
BSS-Cuts*, die 78 sind *Dateien* (jeder Cut zweimal: Spieler-0- und Spieler-1-Verzeichnis).
Die Substanz (494/416/78, die Cut-Liste, die Groessen) stimmt; der Satz mischt zwei Einheiten.

**(c) Behauptung 8 — Reichweite ueberdehnt.**
"wird vom Original **nie** gelesen" ist belegt fuer `FUN_800392d4` (0x800392d4..0x8003958c) und
nur dort. Eine game-weite Aussage waere erst mit einer Xref-Analyse aller Leser des
Cut-Records `+0x1C` bewiesen; die liefert der Bericht nicht. Fuer den Port ist die Folgerung
trotzdem tragfaehig, weil `FUN_800392d4` der einzige Parser des Blocks ist und `FUN_80039590`
ausschliesslich aus den daraus gebauten Prim-Puffern zeichnet.

### Nicht widerlegt, aber der Vollstaendigkeit halber

* Behauptung 4 beweist "genau ein Aufrufer" per `jal`-Bytemuster. Ein Aufruf ueber `jalr`
  (Funktionszeiger) wuerde die Suche nicht finden. Der Bericht nennt die Methode ausdruecklich,
  daher kein Mangel — nur eine Grenze der Aussage.
* Behauptung 14 nennt nur die Folge "kein Vordergrund". Der falsche Tabellenwert 30668 statt
  45900 ist aber zugleich die an `DAT_800be574` uebergebene **Lese-Laenge** (@0x80021cb8), d.h.
  der Cut-Chunk wird auch **kuerzer von CD gelesen**. Ob der MDEC-Hintergrund dieses Cuts
  dadurch ebenfalls beschnitten wird, ist hier **NICHT BELEGT** — das sollte gemessen werden,
  bevor der Port nur den Vordergrund unterdrueckt.
