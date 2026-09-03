# AUFGABE C — BSS-Chunk-Layout und CD-Ladepfad, byte-genau

Datum: 2026-09-03 · Repo `c:/workspace/git/reAi_v2`
Werkzeuge dieses Berichts: `analysis/esp_masken_2026-09-03/work/` (siehe §7)

---

## 0. Ergebnis in drei Saetzen

1. **Es gibt eine deterministische Inhaltsregel** fuer den SLD-Offset — sie stimmt auf **360/360**
   Chunks, die einen SLD-Block tragen:
   `O_SLD = 8 + 4 * (floor((B + 10) / 32) + 1)`, wobei `B` die Anzahl der VLC-Bits fuer 300 Makrobloecke ist.
2. **Die Engine benutzt diese Regel NICHT.** Sie liest eine **u16-Laenge `L` pro (Raum, Cut) aus einer
   Tabelle im Stage-Overlay** (`_DAT_800b52c8`), und der SLD-Zeiger steht in einem **8-Byte-Trailer
   am Ende der Nutzdaten**: `u32 @L-8 = SLD-Offset`, `u32 @L-4 = Flag`. Verifiziert: **1118/1119** Chunks
   (die eine Abweichung ist ein Daten-Defekt im Prototyp, §4.3).
3. **Die 114 STAGE5-Cuts „mit Masken, ohne SLD" existieren nicht.** Das war ein Messartefakt: in
   `re15_port/shared_assets/PSX/BSS/` liegen von STAGE5 nur `ROOM5000/ROOM5001` und von STAGE6
   **gar nichts**; `sweep.json` hat dort `"sld": null` (= nicht gescannt), nicht `[]` (= gescannt, nichts gefunden).
   In den Original-BSS haben **alle 57 masken-tragenden Cuts** dieser 8 Raeume einen aktiven Atlas
   (57 Cuts × 2 Spielervarianten = **exakt die 114**).

---

## 1. Der CD-Ladepfad (Aufgabe C.2)

### 1.1 Zwei Aufrufer, ein Lader

| Adresse | Funktion | Rolle |
|---|---|---|
| `0x8001d600` (`FUN_8001d600`) | Raumwechsel/Tuer | laedt den Chunk des Start-Cuts, `FUN_80013c50(0x80190000, 2, "BS")` @`0x8001da80` |
| `0x80021bbc` (`FUN_80021bbc`) | Kamera-Cut-Wechsel | laedt + zerlegt den Chunk, `FUN_80013c50(0x80190000, 0, "BS")` @`0x80021d2c` |
| `0x80013c50` (`FUN_80013c50`) | CD-Auftrag anstossen | setzt `DAT_800be570=1`, `DAT_800be580=Ziel`, ruft die Zustandsmaschine |
| `0x80013df0` (`FUN_80013df0`) | CD-Zustandsmaschine | `CdControl`/`CdIntToPos`/**`CdRead`**/`CdReadSync` |

Die Namensstrings sind beide `"BS"`: `DAT_800106b0` = `42 53 00 00` @`0x800106b0`,
`DAT_80010708` = `42 53 00 00` @`0x80010708`. (Sie werden von `FUN_80013c50` gar nicht ausgewertet —
der Auftrag laeuft ueber LBA, nicht ueber den Dateinamen.)

### 1.2 Auftrags-Struktur @`0x800be570`

Aus `FUN_80021bbc` (Disasm `0x80021c80`–`0x80021d28`) und `FUN_80013df0`:

```
0x800be570  u8   Zustand (1..4, 0 = fertig)
0x800be571  u8   Typ (=2 fuer BSS)
0x800be572  u16  Datei-Index-1
0x800be574  u32  Laenge in BYTES               <- = L, s. §3
0x800be57c  u32  LBA (absolute Sektornummer)
0x800be580  u32  Zielpuffer                    <- 0x80190000
```

Der eigentliche Lesebefehl, `FUN_80013df0` case 3:

```
CdIntToPos(DAT_800be57c, &DAT_800bee70);       ; case 2
CdControl(0x02 /*CdlSetloc*/, "", 0);
CdRead((DAT_800be574 + 0x7FF) >> 11, DAT_800be580, 0x80);   ; case 3
```

**⇒ gelesene Sektoren = `ceil(L / 2048)`.** Gemessen ueber alle 1119 Chunks: 2 … 29 Sektoren,
nie > 32 — also nie in den Slot des naechsten Cuts hinein (§1.4).

### 1.3 LBA-Berechnung — Disasm `0x80021ca4`–`0x80021d28`

```
80021ca8  lh   v0, DAT_800b0fe0(v0)        ; stage (0-basiert! STAGE1 = 0)
80021cb0  sll  v0,v0,0x2
80021cc8  lw   v0, PTR_DAT_8007438c(at)    ; Zeigertabelle Stage -> Raum-Index-Array
80021ccc  sll  a1,a1,0x1                   ; a1 = raum (DAT_800b0fe2)
80021cd4  lhu  v0,0x0(a1)                  ; fileIdx
80021cdc  addiu v0,v0,-0x1                 ; DAT_800be572 = fileIdx-1
80021cf4  sll  v1,v1,0x3                   ; (fileIdx-1)*8
80021d04  lbu  v0, DAT_8006f442(at)        ; LBA Bit 16..23
80021d14  lhu  v1, DAT_8006f440(at)        ; LBA Bit  0..15
80021d18  sll  v0,v0,0x10
80021d1c  addu v0,v0,v1
80021d20  addu v0,v0,a3                    ; a3 = cut<<5   (80021cd8: sll a3,a3,0x5)
80021d28  sw   v0, DAT_800be57c(at)
```

**⇒ `LBA = tabelle[fileIdx-1].lba + cut * 0x20`** — jeder Kamera-Cut belegt **genau 32 Sektoren = 64 KiB**
auf der CD, egal wie viel davon benutzt wird.

Der Dateitabellen-Eintrag ist 8 Byte gross; das Layout ist unabhaengig durch `FUN_800130c4` belegt:

```c
uint FUN_800130c4(int idx0, undefined4 *outSize) {
  if (outSize) *outSize = *(undefined4 *)(&DAT_8006f43c + idx0 * 8);   /* u32  Groesse */
  return (uint)*(uint3 *)(&DAT_8006f440 + idx0 * 8);                   /* u24  LBA     */
}
```

```
Eintrag k  @ 0x8006f43c + k*8 :  u32 size ; u16 lba_lo ; u8 lba_hi ; u8 flag
```

### 1.4 Gegenprobe: Tabelle vs. echte Dateien — **120/120**

`analysis/esp_masken_2026-09-03/work/cdpath3.py`:

```
st  raum  fileIdx  LBA      size_tab   size_datei Chunks Datei
1   0x0   681      17629    589824     589824     9      ROOM100.BSS
1   0x2   687      18565    851968     851968     13     ROOM102.BSS
2   0x0   801      38382    1048576    1048576    16     ROOM200.BSS
5   0x2   951      60135    851968     851968     13     ROOM502.BSS
6   0x0   1017     69959    1048576    1048576    16     ROOM600.BSS
...
Treffer 120 / Abweichungen 0
```

Alle 120 `ROOM*.BSS` sind exakte Vielfache von 65536 — die 32-Sektor-Schrittweite pro Cut ist damit
end-to-end belegt. (17 der 120 Dateien sind 4-Byte-Platzhalter mit Inhalt `00 00 00 00`:
ROOM127, ROOM20C–20F, ROOM30F, ROOM406, ROOM40C–40F, ROOM515–517, ROOM605–607.)

Die Stage-Zeigertabelle `PTR_DAT_8007438c` (aus `info/Re1.5/PSX.EXE`, Ladebasis `0x80010000`, Header `0x800`):

```
[0]=0x8007429c (STAGE1, 40 Raeume)  [1]=0x800742ec (STAGE2, 16)  [2]=0x8007430c (STAGE3, 16)
[3]=0x8007432c (STAGE4, 16)         [4]=0x8007434c (STAGE5, 24)  [5]=0x8007437c (STAGE6, 8)
```
Der Abstand der Zeiger (0x50 / 0x20 / 0x20 / 0x20 / 0x30) entspricht exakt 40/16/16/16/24 Raeumen à 2 Byte.

### 1.5 Wer zerlegt den Chunk in MDEC-Teil und SLD-Teil?

**`FUN_80021bbc` @`0x80021bbc`** — und zwar in dieser Reihenfolge:

```
80021d38..80021d5c   L  = u16 tabelle[raum*0x20 + cut*2]     (Tabelle = _DAT_800b52c8, s. §3)
80021d6c  lw  v0,-0x4(at)      ; at = 0x80190000 + L   -> Flag  @L-4
80021d74  beq v0,zero,LAB_80021df8    ; Flag == 0 -> KEIN Vordergrund
80021da4  lw  s0,-0x8(at)      ;                       -> SLD-Offset @L-8
80021da8  lui a2,0x801a
80021dac  addiu a2,a2,0x5800   ; Ziel = 0x801a5800
80021db0  addu a0,s1,s0        ; s1 = 0x80190000
80021db4  lw  a1,0x0(a0)       ; a1 = entpackte Groesse (u32 am SLD-Offset)
80021db8  jal FUN_800c47e8     ; SLD-Dekompressor(src=a0+4, size=a1, dst=a2)
80021dbc  addiu a0,a0,0x4
80021dd4..80021df4  DAT_800aca4c = 0x15 ; FUN_8004ee78(0x801a5800) ; DAT_800aca4c zurueck
80021e04..80021e38  FUN_80053a8c(&DAT_80072f2c, 0x80190000, 0x80199e00, 0x80198000, 0)
80021e44            StoreImage(&DAT_80072f2c, 0x80198000)
```

`FUN_80053a8c` @`0x80053a8c` ist die MDEC-Kette:
`DecDCTReset(0)` → `DecDCToutCallback(0)` → **`DecDCTvlc(bs=0x80190000, out=0x80199e00)`** →
`DecDCTin(0x80199e00, 0)` → Schleife `DecDCTout(0x80198000, …)` / `DecDCToutSync` / `LoadImage` / `DrawSync`.

`FUN_8004ee78` @`0x8004ee78` ist der TIM-Uploader (`OpenTIM`/`ReadTIM`/`LoadImage`); er ueberschreibt
`prect->x = slot*64` (bei slot ≥ 0x10 zusaetzlich `-0x400`) und `prect->y = (slot>=0x10)<<8`;
mit slot `0x15` ⇒ VRAM (320, 256). CLUT nach `crect->y = DAT_800aca4d + 0x1e0` (= y 480).

**RAM-Karte (aus den Konstanten in `FUN_80021bbc`):**

| Adresse | Inhalt | Groesse |
|---|---|---|
| `0x80190000` | roher BSS-Chunk von der CD | ≤ 0x10000 |
| `0x80198000` | `DecDCTout`-Streifenpuffer | 0x1E00 (16×240 @16bpp) |
| `0x80199e00` | `DecDCTvlc`-Ausgabe (Run-Level) | ≈ `rlw*4` B, bis ~0x16200 |
| `0x801a5800` | entpackter SLD-Block (TIM) | ≤ 0x10220 |

**Wichtig fuer jeden Port:** `0x80199e00 + rlw*4` reicht bei grossen Frames bis ~`0x801AFF80` und
**ueberschreibt damit den SLD-Puffer bei `0x801a5800`**. Das Original ist nur deshalb korrekt, weil der
SLD-Block **vor** `FUN_80053a8c` entpackt **und** per `FUN_8004ee78` ins VRAM geschoben wird.
Die Reihenfolge SLD → MDEC ist also nicht kosmetisch, sondern zwingend.

### 1.6 Der SLD-Dekompressor `FUN_800c47e8` — jetzt statisch lokalisiert

`0x800c47e8` liegt **ausserhalb** des PSX.EXE-Abbilds (`load=0x80010000`, `t_size=0xaf000` ⇒ Ende
`0x800bf000`); im Ghidra-Dump steht dort nur `??`. Der Code kommt aus **`BIN/DEBUG.BIN`**:

```c
/* FUN_8001311c @0x8001311c */
void FUN_8001311c(void) { FUN_80013b60(7, &DAT_800c0000, 0); ... }
```
Dateitabellen-Index 7 → LBA 339, Groesse **262144** = exakt `info/Re1.5/PSX/BIN/DEBUG.BIN`.
⇒ **`FUN_800c47e8` = `DEBUG.BIN` + `0x47E8`** (Ladebasis `0x800c0000`, kein Header).

Disassembliert (`work/dis.py`), Signatur `(a0=src, a1=size, a2=dst)`:

```
800c47e8  00005821  addu   t3,zero,zero        ; sp = 0
800c47ec  00006021  addu   t4,zero,zero        ; flagMask = 0
800c47f0  18a0003f  blez   a1,0x800c48f0
800c47f4  00004021  addu   t0,zero,zero        ; dp = 0
800c47f8  15800005  bne    t4,zero,0x800c4810
800c4800  340c0080  ori    t4,zero,0x80        ; flagMask = 0x80
800c4804  904d0000  lbu    t5,0(v0)            ; flagByte = src[sp++]
800c4810  90470000  lbu    a3,0(v0)            ; a = src[sp++]
800c4818  30e20080  andi   v0,a3,0x80
800c481c  14400005  bne    v0,zero,0x800c4834
800c4828  a0470000  sb     a3,0(v0)            ; a < 0x80 -> Literal, KEIN Flag-Bit
800c4834  01ac1024  and    v0,t5,t4
800c4838  14400006  bne    v0,zero,0x800c4854
800c4844  a0470000  sb     a3,0(v0)            ; Flag-Bit 0 -> Literal (behaelt Bit 7)
800c4850  000c6042  srl    t4,t4,1
800c4858  90470000  lbu    a3,0(v0)            ; b2
800c4860  00071400  sll    v0,a3,16
800c4864  00431025  or     v0,v0,v1            ; v1 = a<<24
800c4868  00025503  sra    t2,v0,20            ; off = signext12((a<<4)|(b2>>4))
800c486c  30e2000f  andi   v0,a3,0xf
800c4870  1440000c  bne    v0,zero,0x800c48a4
800c48a4  24490002  addiu  t1,v0,0x2           ; len = (b2&0xF)+2
800c4884  30e3003f  andi   v1,a3,0x3f          ; sonst: b3
800c4888  24690003  addiu  t1,v1,0x3           ; len = (b3&0x3F)+3
800c488c  00071182  srl    v0,a3,6
800c4890  000a1880  sll    v1,t2,2
800c489c/800c48a0                              ; off = signext16((off<<2)|(b3>>6))
800c48c4..800c48d8                             ; byteweise, ueberlappende Kopie von dst[dp+off]
800c48e4  0105102a  slt    v0,t0,a1            ; while (dp < size)
```

Das ist **byte-genau** der Algorithmus in `src/main/java/de/re15/extractors/bss/SldDecoder.java`
und in `analysis/esp_masken_2026-09-03/sld.py`. Damit ist die bisher nur aus einem Savestate
rekonstruierte Routine jetzt aus einer **ausgelieferten Datei** belegbar.

---

## 2. Chunk-Layout (Aufgabe C.1)

```
+0x0000  u16  runLengthWords   (Anzahl 32-Bit-Woerter der MDEC-Run-Level-Ausgabe)
+0x0002  u16  id = 0x3800      (1119/1119 Chunks)
+0x0004  u16  quant
+0x0006  u16  version          (2 oder 3; gemessen: 479 × ver2, 640 × ver3)
+0x0008  ...  VLC-Bitstrom, 16-Bit-Woerter LE, Bits MSB-first
   B Bits fuer 300 Makrobloecke (320x240 = 20x15 MB à 6 Bloecke)
+B       10 Bit  Endmarker    ver3 = 0b1111111111 (639/640), ver2 = 0b0111111111 (479/479)
         Null-Padding, mindestens 1 Bit, bis zur naechsten 32-Bit-Grenze
O        u32  entpackte Groesse des SLD-Blocks   \
O+4      ...  SLD/LZ-Strom                        > nur wenn Vordergrund vorhanden
E=align4(SLD-Ende)                               /
E        u32  SLD-Offset (= O)      \  8-Byte-Trailer
E+4      u32  Flag (1 = Atlas da)   /
L = E+8  Ende der Nutzdaten  == u16 aus der Overlay-Tabelle (§3)
...      Rest der 64 KiB: von der CD nie gelesen (CdRead nimmt nur ceil(L/2048) Sektoren)
```

### 2.1 Die Inhaltsregel fuer O — 360/360

`B` = exakt konsumierte VLC-Bits, gemessen mit dem instrumentierten Port-Dekoder
(`work/bss_vlc_instr.c`, Kopie von `re15_port/engine/src/bss_vlc.c` mit
`B = src_offset*8 + bit_count - 16`; die Invariante ist exakt, weil `flush_bits` `src_offset*8+bit_count`
beim Nachladen unveraendert laesst).

```
O = 8 + 4 * ( floor((B + 10) / 32) + 1 )
```

Gegenprobe mit allen anderen Konstanten k statt 10 (`work/an3.py`, 360 Chunks mit SLD):

```
k= 0 -> 247/360   k= 6 -> 310/360   k= 9 -> 346/360
k= 8 -> 333/360   k=10 -> 360/360   k=11 -> 349/360   k=12 -> 334/360
```

**Nur k = 10 trifft alle 360.** Die 10 ist genau der Endmarker: die 10 Bits ab `B` sind in
1119/1119 Chunks entweder `1111111111` oder `0111111111`, streng nach Version getrennt
(479/479 ver2 → `0111111111`; 639/640 ver3 → `1111111111`; 1 ver3-Chunk mit `0111111111`).

Ausrichtung: **4 Byte**, mit **mindestens 1 Pad-Bit** (deshalb `floor(...)+1` statt `ceil`);
identisch schreibbar als `O = 8 + 4*ceil((B+11)/32)`. In 39 der 360 Faelle
(`B mod 32 ∈ {22,23,24}`, und nur dort) fuehrt das zu einem **zusaetzlichen Null-DWORD** — genau die
Faelle, in denen `B+10` ein Vielfaches von 32 ist bzw. weniger als 2 Bit uebrig blieben.

Ein **Marker/Magic zwischen VLC und SLD gibt es nicht.** Zwischen dem Endmarker und `O` stehen Nullen.

### 2.2 Trailer-Regel

Fuer alle 360 SLD-Chunks gilt ausnahmslos (`work/an.py`, „Trailer-Regel verletzt: 0"):

```
L = align4(SLD-Kompressions-Ende) + 8      und   u32@[L-8] = O ,  u32@[L-4] = 1
```

Chunks ohne SLD (759): `u32@[L-4] == 0` in **759/759**. Der Schwanz sieht dort so aus
(je nach Version verschieden — die Engine liest davon nur `[L-4]`):

| Fall | Anzahl | Schwanz ab O |
|---|---|---|
| ver2 | 467 | `L = O+4`, die 4 Byte bei `O` sind Null ⇒ `[L-4] = 0` |
| ver3 | 257 | `[O] = O`, `[O+4] = O+4`, `[O+8] = 0`, `L = O+12` |
| ver3 | 27 | `[O] = O`, `[O+4] = 0`, `L = O+8` |
| beide | 14 | `L = O` (Nutzdaten enden mit dem Padding) |

---

## 3. Woher `L` kommt: die Overlay-Tabelle `_DAT_800b52c8`

Jedes Stage-Overlay setzt in seiner Init-Funktion:

| Stage | Init-Funktion (Decompile) | Tabelle | Datei-Offset in `BIN/STAGE<N>.BIN` |
|---|---|---|---|
| 1 | `RE_15_Quellcode_Overlays/STAGE1/FUN_8011e064.c:28` | `0x8011eae4` | `0x1EAE4` |
| 2 | `STAGE2/FUN_801166ec.c:25` | `0x8011713c` | `0x1713C` |
| 3 | `STAGE3/FUN_8011c6c0.c:25` | `0x8011d0f8` | `0x1D0F8` |
| 4 | `STAGE4/FUN_80117b80.c:24` | `0x80118590` | `0x18590` |
| 5 | `STAGE5/FUN_8011d4e0.c:25` | `0x8011df18` | `0x1DF18` |
| 6 | `STAGE6/FUN_801015ac.c:13` | `0x80101e3c` | `0x01E3C` |

Ladebasis der Overlays = `0x80100000`, **kein 0x800-Header** — empirisch entschieden:
mit Offset `+0` treffen 359/360 Trailer-Positionen, mit `+0x800` **0/360**
(`work/tbl.py`).

Zugriff (Disasm `0x80021d38`–`0x80021d5c`):

```
L = *(u16*)( _DAT_800b52c8 + raum*0x20 + cut*2 )
```
⇒ 16 Cuts pro Raum, 0x20 Byte pro Raum. Rohbeispiel STAGE1, Raum 2 (`ROOM102`), Datei-Offset `0x1EB24`:

```
20 a4 58 a7 54 9b 2c a4 a4 77 40 95 a0 8b e0 7c 40 6d 08 88 d4 91 f0 56 bc 84 08 00 08 00 08 00
= [42016, 42840, 39764, 42028, 30628, 38208, 35744, 31968, 27968, 34824, 37332, 22256, 33980, 8, 8, 8]
```
(die `8` sind die Fuellwerte fuer nicht existierende Cuts).

**Dieselbe u16 ist auch die CD-Lesegroesse** (`DAT_800be574`, `0x80021cb8`) — Laenge und
Trailer-Position sind per Konstruktion dasselbe Feld.

---

## 4. Verifikation ueber den gesamten Datenbestand

### 4.1 Chunk-Ebene — 1119 Chunks aus 120 Original-BSS

```
id == 0x3800                        1119 / 1119
SLD-Block vorhanden                  360
  davon an O (Inhaltsregel)          360 / 360        (work/an3.py)
  Trailer L-8/L-4 korrekt            359 / 360        (work/tbl2.py)
kein SLD-Block                       759
  Flag @L-4 == 0                     759 / 759
```

### 4.2 Cut-Ebene — alle 2188 Cuts aus `sweep.json` (206 Raumvarianten)

```
Masken(RDT)  Atlas(BSS+Tabelle)   Cuts
   nein            nein           1470
   nein            ja              238
   ja              ja              480
   ja              nein              0     <-- kein einziger Fall
```

**Kein masken-tragender Cut im Spiel ist ohne Atlas.** Die alten Zahlen
(366 / 114 / 130 / 1578) sind durch die fehlenden BSS-Dateien im Port-Baum verzerrt (§5).

### 4.3 Die eine Abweichung: `STAGE3/ROOM305.BSS`, Cut 14

```
Kopf +0x0000: e0 52 00 38 01 00 03 00     (rlw=21216 id=0x3800 quant=1 ver=3)
B = 245238 ; Endmarker @B = 1111111111
O = 30668 = +0x77cc ;  +0x77cc: 20 02 01 00 d0 10 00 00 ...   -> gueltiger SLD-Block, 66080 B entpackt
Tabellenwert L (STAGE3.BIN +0x1D0F8 + 5*0x20 + 14*2) = 30668 = 0x77cc      <-- L == O
+0x77c4 (L-8): 58 61 ff eb 00 00 00 00    -> Trailer {0xEBFF6158, Flag = 0}
CdRead-Sektoren = (30668+0x7ff)>>11 = 15  -> 30720 B; vom SLD-Block waeren nur 52 B im RAM
```

Der Atlas liegt physisch in der Datei, die Laengentabelle schneidet ihn aber ab ⇒ **im
Auslieferungsstand ist der Vordergrund dieses Cuts aus**. `sweep.json` sagt fuer ROOM3050/3051 Cut 14
`masks: 0` — es fehlt also nichts sichtbar. Ein Authoring-Rest des Prototyps, kein Widerspruch zur Regel.

### 4.4 Beleg-Hexdumps (alle aus `work/evidenz.py`)

```
--- MIT Atlas (ver 3)  ROOM102.BSS cut 0
    Kopf   +0x0000: 80 58 00 38 01 00 03 00      (rlw=22656 id=0x3800 quant=1 ver=3)
    B = 253299 ; letztes VLC-Byte +0x7bb6 ; Endmarker @B = 1111111111
    O = 31672 = +0x7bb8
    +0x7bb8: 20 f2 00 00 d0 10 00 00 00 09 ff c1 0c 02 ff b1   (u32 = 61984 entpackt)
    L = 42016 = 0xa420 (STAGE1.BIN +0x1EAE4 + 2*0x20 + 0*2)
    +0xa418: b8 7b 00 00 01 00 00 00      -> {SLD-Offset 31672, Flag 1}
    CdRead = 21 Sektoren

--- MIT Atlas, 4 B Extra-Pad (B mod 32 = 22)  ROOM103.BSS cut 3
    Kopf   +0x0000: 60 36 00 38 02 00 03 00
    B = 154038 ; letztes VLC-Byte +0x4b3e ; Endmarker = 1111111111
    O = 19268 = +0x4b44        (align4(letztes Byte)=+0x4b40, also 4 B mehr)
    +0x4b44: 20 f2 00 00 d0 10 00 00 ...
    L = 30328 ; +0x7670: 44 4b 00 00 01 00 00 00 -> {19268, 1}

--- OHNE Atlas (ver 2)  ROOM100.BSS cut 0
    Kopf   +0x0000: 20 3c 00 38 02 00 02 00
    B = 171374 ; Endmarker = 0111111111 ; O = 21432 = +0x53b8
    +0x53b8: 00 00 00 00 00 00 00 00 ...
    L = 21436 = O+4 ; +0x53b4: e9 d8 00 ff 00 00 00 00 -> Flag 0

--- OHNE Atlas (ver 3, Selbstzeiger-Schwanz)  ROOM102.BSS cut 11
    Kopf   +0x0000: a0 42 00 38 01 00 03 00
    B = 177873 ; O = 22244 = +0x56e4
    +0x56e4: e4 56 00 00 e8 56 00 00 00 00 00 00 ...   ([O]=O, [O+4]=O+4, [O+8]=0)
    L = 22256 = O+12 ; +0x56e8: e8 56 00 00 00 00 00 00 -> Flag 0
```

### 4.5 Was der entpackte SLD-Block ist

`work/sldtim.py` (Sony-TIM nach dem Entpacken):

```
ROOM102 cut0 : packed 10332 -> 61984 ; magic 0x10, flag 0x9 (8bpp+CLUT)
               CLUT len 524 @VRAM(0,480) 256x1 ; IMG len 61452 @VRAM(0,0) 128x240 Halbwoerter = 256x240 px
ROOM109 cut1 : packed  7322 -> 66080 ; IMG 128x256 Halbwoerter = 256x256 px
ROOM50A cut2 : packed 24330 -> 66080 ; IMG 256x256 px
```
Verteilung ueber alle 360 Treffer: `66080` (0x10220, 256×256) **321×**, `61984` (0xF220, 256×240) **38×**,
`58400` (0xE420, 256×226) **1×** — der Sonderfall ist `STAGE4/ROOM404.BSS` Cut 5
(`sld@25892`, `L=49856`, Trailer `(25892,1)`, CLUT 524 B, IMG 57868 B = 128×226 Halbwoerter).
Ein Port darf die Atlas-Groesse also **nicht** hart auf 256×256 annehmen.
Sonst passt es genau auf die Erwartung von `re15_pri_load_cut_atlas` in
`re15_port/platform/pc/src/bg_pc.c:78` (`tim.bpp == 8 && tim.has_clut`).

---

## 5. Aufgabe C.3 — die „114 STAGE5-Cuts ohne SLD"

**Befund: existieren nicht.** `sweep.json` traegt fuer diese Raeume `"sld": null`, nicht `[]`:

```python
>>> sweep['ROOM5030']['cuts'][1]
{'cut': 1, 'st': 'OK', 'masks': 25, 'sld': None, 'tim': False}
```

Ursache: `re15_port/shared_assets/PSX/BSS/` enthaelt von STAGE5 nur `ROOM5000/ROOM5001` und von
STAGE6 gar keinen Raum (156 Verzeichnisse, davon 2 aus STAGE5). Der Sweep konnte dort keine
BSS-Datei oeffnen und hat `null` eingetragen.

Aus den Originaldateien (`info/Re1.5/PSX/STAGE5/ROOM5xx.BSS`, `work/stage5.py`):

```
ROOM5030 (ROOM503.BSS): cut1 masken=25 -> sld@23172 (66080) L=30300 Trailer=(23172,1)
                        cut2 masken=21 -> sld@31184 (66080) L=40824 Trailer=(31184,1)
                        cut3 masken=19 -> sld@27432 (66080) L=32936 Trailer=(27432,1)
                        cut4 masken=14 -> sld@27416 (66080) L=34352 Trailer=(27416,1)
ROOM5040 (ROOM504.BSS): 6 masken-Cuts, alle mit Atlas (30384/30748/20736/21592/31520/21824)
ROOM5060 (ROOM506.BSS): 12 masken-Cuts, alle mit Atlas
ROOM50A0 (ROOM50A.BSS): 10 masken-Cuts, alle mit Atlas
ROOM50C0 (ROOM50C.BSS):  4 masken-Cuts, alle mit Atlas
ROOM5110 (ROOM511.BSS):  4 masken-Cuts, alle mit Atlas
ROOM5120 (ROOM512.BSS):  6 masken-Cuts, alle mit Atlas
ROOM5140 (ROOM514.BSS): 11 masken-Cuts, alle mit Atlas

Cuts mit Maskenrecords: 57   davon mit im Spiel AKTIVEM SLD-Atlas: 57
```

**57 × 2 Spielervarianten = 114** — exakt die gemeldete Zahl. Der SLD-Codec ist dort derselbe
(`FUN_800c47e8`), der Atlas liegt im **selben** Chunk (kein Nachbar-Chunk, keine andere Datei).

---

## 6. Konsequenzen fuer den Port

1. **Der Port braucht keinen vorextrahierten `PRI##.TIM` mehr.** Er hat alle Zutaten im Baum:
   `BSS/ROOM???.BSS`-Chunks liegen als `BSS/ROOM####/BG##.BSS` vor, und `BIN/STAGE<N>.BIN` ist im
   Port-Baum **byte-identisch** zum Original (sha256-Praefix je Stage geprueft, 6/6 IDENT).
2. **Byte-treuer Ablauf im Port:**
   ```
   L    = u16 @ (STAGE<N>.BIN + tblOff[N] + raum*0x20 + cut*2)      // tblOff s. §3
   if (u32 @chunk[L-4] == 0) -> kein Atlas, Overdraw AUS
   off  = u32 @chunk[L-8]
   size = u32 @chunk[off]
   sld_decompress(chunk+off+4, size, dst)   // FUN_800c47e8 = DEBUG.BIN+0x47E8
   -> Sony-TIM, 8bpp, CLUT 256x1
   ```
   Die Inhaltsregel aus §2.1 **nicht** als Primaerquelle nehmen — sie wuerde bei
   `ROOM3050/3051` Cut 14 einen Atlas aktivieren, den das Original nicht laedt (§4.3).
   Sie taugt als Plausibilitaetspruefung (`O == u32@[L-8]`).
3. **Ein SLD-Dekoder in C fehlt noch** (`re15_port` hat nur den Java-`SldDecoder`). Referenz-C-Port
   liegt jetzt in `analysis/esp_masken_2026-09-03/work/layout2.c` (`sld_decomp`), 1:1 aus der
   Disassembly in §1.6.
4. **Luecke heute (`work/portluecke.py`):** von 718 Cuts mit aktivem Atlas hat der Port fuer
   **416** eine `PRI##.TIM`, fuer **302 nicht** — davon **188 mit Maskenrecords**, d. h. dort zeichnet
   `re15_pri_load_cut_atlas` (`bg_pc.c:63`) nichts, obwohl das Original einen Vordergrund hat.
   Betroffen u. a.:
   ```
   ROOM1020/1021 12 (alle mit Masken)   ROOM1030/1031 12 (alle mit Masken)
   ROOM1040/1041  5                     ROOM1070/1071  8
   ROOM5030/5031  4   ROOM5040/5041  6  ROOM5060/5061 12  ROOM50A0/50A1 10
   ROOM50C0/50C1  4   ROOM5110/5111  4  ROOM5120/5121  6  ROOM5140/5141 11
   (dazu Cuts ohne Masken: ROOM1220, ROOM4040, ROOM5020/5050/5090/50B0/5100, ROOM6000/6010/6030)
   ```
5. **Reihenfolge-Falle:** siehe §1.5 — SLD entpacken + hochladen **vor** dem VLC-Dekodieren, sonst
   ueberschreibt die MDEC-Ausgabe im Original-RAM-Layout den Atlas.

---

## 7. Werkzeuge / Reproduktion

Alle unter `analysis/esp_masken_2026-09-03/work/`:

| Datei | Zweck |
|---|---|
| `bss_vlc_instr.c` | Kopie von `re15_port/engine/src/bss_vlc.c` + Bit-Zaehler (`g_vlc_src_used`, `g_vlc_bitcount`) |
| `layout2.c` / `layout2.exe` | pro Chunk: exaktes VLC-Ende, SLD-Suche an `O` + Brute-Force, Trailer; C-Port des SLD-Codecs |
| `orig_all.jsonl` | Rohdaten aller 1119 Chunks aus `info/Re1.5/PSX/STAGE*/ROOM*.BSS` |
| `an.py`, `an2.py`, `an3.py` | Regel-Ableitung + k-Sweep |
| `tbl.py`, `tbl2.py`, `tbl3.py`, `tbl4.py` | Overlay-Tabelle ↔ Trailer ↔ Inhaltsregel |
| `cdpath.py`, `cdpath2.py`, `cdpath3.py` | Dateitabelle `0x8006f43c` gegen echte Dateigroessen |
| `stage5.py` | Aufgabe C.3 |
| `kreuz.py` | Kreuztabelle Masken × Atlas ueber 2188 Cuts |
| `portluecke.py` | fehlende `PRI##.TIM` im Port |
| `evidenz.py` / `evidenz.txt` | die Hexdumps aus §4.4, Marker/Version-Korrelation |
| `sldtim.py` | TIM-Kopf des entpackten Atlas |
| `dis.py` | Mini-MIPS-Disassembler (fuer `DEBUG.BIN+0x47E8`) |

Build: `gcc -O2 -I re15_port/include -o layout2.exe layout2.c bss_vlc_instr.c re15_port/engine/src/bss_common.c`
(mingw64, PATH via `re15_port/tools/local_build.sh`-Regel).

---

## 8. Offen / NICHT BELEGT

- **Warum genau 1 ver3-Chunk** den ver2-Endmarker `0111111111` traegt (639/640) — moeglich ist auch
  ein 1-Bit-Fehler in meiner `B`-Rechnung fuer genau diesen Chunk. Ohne Auswirkung auf die
  Offset-Regel (die 360/360 trifft).
- **Die Selbstzeiger im Schwanz** von Chunks ohne Atlas (`[O]=O`, `[O+4]=O+4`) sind gemessen, aber
  ihre Bedeutung ist **NICHT BELEGT** — die Engine liest ausschliesslich `[L-8]` und `[L-4]`.
- **Das `flag`-Byte** im Dateitabellen-Eintrag (`0x8006f443 + k*8`) ist gemessen, seine Bedeutung
  **NICHT BELEGT** (`FUN_800130c4` liefert nur `size` und `u24 lba`).
- **`_DAT_800b52c8` fuer DEBUG.BIN/TITLE.BIN**: nicht geprueft, ob diese Overlays ebenfalls eine
  BSS-Laengentabelle setzen.

---

# Gegenpruefung

Datum: 2026-09-03 - unabhaengiger Nachbau aller Messungen (eigene Skripte, eigener
instrumentierter VLC-Dekoder), **keine** Zahl aus `work/` uebernommen.

Werkzeuge dieser Gegenpruefung: `analysis/esp_masken_2026-09-03/xcheck3/`
(`myvlc.c` = eigene Instrumentierung von `re15_port/engine/src/bss_vlc.c`, `probe.c`,
`sweep.c` -> `B.txt` mit 1119 Zeilen `stage raum cut rlw quant ver written blocks B`),
plus Inline-Python gegen `info/Re1.5/PSX.EXE`, `info/Re1.5/PSX/BIN/STAGE*.BIN`,
`info/Re1.5/PSX/STAGE*/ROOM*.BSS` und `info/Re1.5/PSX/STAGE*/ROOM????.RDT`.

## G.1 Widerlegt

### W1 - Behauptung 9: "alle sind exakte Vielfache von 65536" ist FALSCH

Behauptung 9 sagt woertlich: *"Die size-Felder stimmen bei 120/120 ROOM\*.BSS mit den echten
Dateigroessen ueberein; alle sind exakte Vielfache von 65536."*

Der erste Halbsatz haelt (siehe G.2). Der zweite ist widerlegt: **17 der 120** `size`-Felder
(und damit auch der Dateien) sind **4 Byte**, also kein Vielfaches von 65536.

```
Dateitabelle 0x8006f43c + k*8, k = fileIdx-1, PSX.EXE (load 0x80010000, Header 0x800)
 st raum fileIdx  LBA    size_tab  size_datei  flag  Datei
  1  39    798    38377      4         4        0    ROOM127.BSS
  2  12    837    46681      4         4        0    ROOM20C.BSS
  ... (17 Zeilen, exakt die in Behauptung 17 genannten)
Vielfaches von 65536: 103 / 120     nicht: 17 / 120
```

Der Bericht widerspricht sich hier **innerhalb desselben Absatzes** (Paragraf 1.4): erst "Alle 120
ROOM\*.BSS sind exakte Vielfache von 65536", dann in Klammern "(17 der 120 Dateien sind
4-Byte-Platzhalter)". Sachlich folgenlos - die 17 Platzhalter tragen keinen Chunk (Behauptung 17
haelt) -, aber als Aussage falsch.

### W2 - Behauptung 18: "L = O in 14 Faellen" ist FALSCH (gemessen: 8 bzw. 9)

Eigene Messung ueber alle 1119 Chunks (Inhaltsregel `Oc = 8+4*(floor((B+10)/32)+1)` aus eigenem
`B`, `L` aus der Overlay-Tabelle, Trailer aus dem Chunk):

```
kein aktiver Atlas (Flag@L-4 != 1), L - O_inhalt:
   {0: 9, 4: 467, 8: 27, 12: 257}      Summe 760
joint (version, L-O):
   (2,0): 6   (2,4): 467   (3,0): 3   (3,8): 27   (3,12): 257
Formen bestaetigt: '4'  -> [O]==0                        (467x)
                   '8'  -> [O]==O, [O+4]==0               (27x)
                   '12' -> [O]==O, [O+4]==O+4, [O+8]==0  (257x)
```

Der `0`-Eimer hat **9** Eintraege (bzw. **8**, wenn man den Sonderfall `ROOM305.BSS` Cut 14
herausrechnet, der als einziger Flag-0-Chunk physisch einen SLD-Block traegt) - **nie 14**.

Die eigene Beleg-Zeile der Behauptung sagt selbst `{4:467, 12:257, 8:27, 0:8}`; die Prosa der
Behauptung und die Tabelle in Paragraf 2.2 ("beide | 14 | L = O") widersprechen dem Beleg. Auch die
Summe verraet es: 467+257+27+**14** = 765 != 759 Chunks ohne SLD; 467+257+27+**8** = 759.

Die drei uebrigen Auspraegungen und ihre Formen halten exakt (Zahlen und Byte-Muster oben).

### W3 - Behauptung 10: die Zahl "bis ~0x801AFF80" ist falsch gerechnet

Behauptung 10 belegt die Ueberschreibung mit: *"ROOM102 Cut0: rlw=22656 -> 0x80199e00 + 22656*4 =
0x801AFF80 > 0x801a5800."*

```
22656 * 4 = 90624 = 0x16200
0x80199E00 + 0x16200 = 0x801B0000       (nicht 0x801AFF80)
0x801AFF80 - 0x80199E00 = 0x16180 = 90496 = 22624*4   -> entspraeche rlw 22624, nicht 22656
```

Auch die zweite denkbare Lesart trifft nicht: der Dekoder schreibt fuer diesen Chunk tatsaechlich
45260 int16 = 90520 B -> Ende `0x801AFFD8`. Der Chunk-Kopf ist nachgemessen
`80 58 00 38 01 00 03 00`, also `rlw = 0x5880 = 22656`.

**Die Schlussfolgerung bleibt unberuehrt** (sie wird sogar staerker): `0x801B0000 > 0x801a5800`,
der SLD-Puffer wird ueberschrieben, die Reihenfolge SLD -> MDEC ist zwingend. Widerlegt ist nur die
zitierte Zahl.

## G.2 Gegengeprueft und BESTAETIGT (unabhaengig nachgemessen)

| # | Thema | eigenes Ergebnis |
|---|---|---|
| 1 | Chunk-Layout, Kopf, Trailer, kein Magic | `id==0x3800` **1119/1119**; ver2 **479**, ver3 **640**. Zwischen Endmarker `B+10` und `O` sind **alle** Bits 0 (359/359 Chunks mit Atlas geprueft), Padlaenge **1..32 Bit** => "mind. 1 Pad-Bit, 4-Byte-Ausrichtung". Alle vier Hexdumps aus Paragraf 4.4 Byte fuer Byte nachgeschlagen und identisch (ROOM102 c0 `+0x7bb8`/`+0xa418`, ROOM100 c0 `+0x53b4`/`+0x53b8`, ROOM102 c11 `+0x56e4`, ROOM103 c3 `+0x4b44`). |
| 2 | Inhaltsregel `O = 8+4*(floor((B+10)/32)+1)` | eigener instrumentierter Dekoder (1800 Bloecke = 300 Makrobloecke in **1119/1119** Chunks): **360/360** Treffer, **0 Verletzungen**. Eigener k-Sweep k=0..19: `0->247, 6->310, 8->333, 9->346, 10->360, 11->349, 12->334` - identisch zum Bericht, nur k=10 trifft alles. Stichprobe: ROOM102 c0 `B=253299`, ROOM100 c0 `B=171374` (beides exakt die berichteten Werte). |
| 3 | 10-Bit-Endmarker, versionsabhaengig | eigene Bit-Extraktion `[B,B+10)` ueber alle 1119: **(ver2,0111111111)=479, (ver3,1111111111)=639, (ver3,0111111111)=1**. Den Ausreisser habe ich lokalisiert: **STAGE4/ROOM401.BSS Cut 4** (Kopf `a0 38 00 38 01 00 03 00`, `B=191907`, Bits `011111111100`). Caveat: "streng versionsabhaengig" ist durch die eigene Zahl 639/640 relativiert - die Zahlen selbst sind korrekt. |
| 4 | Engine liest Trailer ueber `L` | Disasm in `ghidra1_V2.txt` (Zeilen 101700ff) Instruktion fuer Instruktion nachgeschlagen: `80021d5c lhu v0,0x0(v1)`, `80021d64 lui at,0x8019`, `80021d68 addu at,v0,at`, `80021d6c lw v0,-0x4(at)`, `80021d74 beq v0,zero,LAB_80021df8`, `80021da4 lw s0,-0x8(at)`, `80021da8/dac lui a2,0x801a / addiu a2,a2,0x5800`, `80021db0 addu a0,s1,s0`, `80021db4 lw a1,0x0(a0)`, `80021db8 jal FUN_800c47e8`, `80021dbc addiu a0,a0,0x4` - alle **wortgleich** vorhanden. |
| 5 | Tabelle `_DAT_800b52c8`, Datei-Offsets | Alle sechs Zuweisungen in den Overlay-Decompiles vorhanden. Eigener Sweep mit Offset **+0**: `L` in **0/1119** Faellen ausserhalb `[16,65536]`, Flag in {0,1} bei 1119/1119. Mit **+0x800**: 291 Chunks mit unbrauchbarem `L`, 615 mit Flag nicht in {0,1}, **0** mit Flag 1. Je Stage traegt die Tabelle: S1 122, S2 15, S3 77, S4 33, S5 86, S6 26 aktive Atlanten - und in **jeder** Stage stimmt `u32@[L-8]` mit der unabhaengig gerechneten Inhaltsregel ueberein (122/15/77/33/86/26). Rohbytes `STAGE1.BIN+0x1EB24` identisch reproduziert (= Basis `0x1EAE4` + Raum 2 * 0x20 - im Behauptungstext fehlt das Label "Raum 2", in Paragraf 3 des Berichts steht es). |
| 6 | `L = align4(SLD-Ende)+8`, `[L-8]=O`, `[L-4]=1` | eigener Voll-Dekode aller 359 aktiven Atlanten mit eigenem SLD-Lauf: **0 Verletzungen** der Align-Regel, **0 Dekodier-Fehler**. Flag-0-Chunks: **760** (= 759 + der ROOM305-Sonderfall). |
| 7 | CD-Ladepfad | `RE_15_Quellcode_V2/FUN_80013df0.c` enthaelt woertlich `CdRead(DAT_800be574 + 0x7ffU >> 0xb, DAT_800be580, 0x80)` (case 3) und `CdIntToPos(DAT_800be57c,...)` (case 2). `8001da04 sw v1,-0x1a8c(at)=>DAT_800be574` (Tabellen-u16) und `80021cb8` identisch. `8001da80 jal FUN_80013c50` mit `a0=lui 0x8019`, `a1=2`, `a2=DAT_800106b0`. Strings nachgeschlagen: `800106b0: 42 53 00 00`, `80010708: 42 53 00 00`. Sektorzahlen `ceil(L/2048)` ueber alle 1119: **min 2, max 29**, nie > 32. |
| 8 | `LBA = tabelle[fileIdx-1].lba + cut*0x20` | Disasm-Kette `80021ca8 / 80021cb0 / 80021cc8 / 80021ccc / 80021cd4 / 80021cd8 / 80021cdc / 80021d04 / 80021d14 / 80021d18 / 80021d1c / 80021d20` wortgleich vorhanden. Zeigertabelle direkt aus `PSX.EXE` gelesen: `8007429c / 800742ec / 8007430c / 8007432c / 8007434c / 8007437c` (Abstaende 0x50/0x20/0x20/0x20/0x30). Die 0-Basis der Stage ist empirisch bewiesen: mit `st-1` treffen 120/120 Dateigroessen, mit 1-Basis waere ROOM100 auf STAGE2s Array gelaufen. |
| 9 | Dateitabellen-Eintrag 8 B | `FUN_800130c4.c` woertlich wie zitiert. **120/120 Treffer**, 0 Abweichungen (z. B. STAGE1 raum0 fileIdx 681, LBA 17629, size 589824 = ROOM100.BSS). Nur der Zusatz "alle Vielfache von 65536" ist falsch -> **W1**. |
| 10 | Zerlegung SLD zuerst, MDEC danach | RAM-Konstanten nachgeschlagen: `80021e08 lui a1,0x8019`, `80021e0c/e10 -> 0x80199e00`, `80021e14/e20 -> 0x80198000`, `80021da8/dac -> 0x801a5800`. `FUN_80053a8c @80053ad0 jal DecDCTvlc` mit `a0=s0` (=Bitstrom 0x80190000), `a1=s1` (=0x80199e00). Reihenfolge im Kontrollfluss belegt (SLD-Block `80021da4..80021df4` liegt vor `LAB_80021df8` -> MDEC ab `80021e04`). Nur die Beispielzahl ist falsch -> **W3**. |
| 11 | `FUN_800c47e8` = `DEBUG.BIN + 0x47E8` | `PSX.EXE`-Kopf selbst gelesen: `load=0x80010000, t_size=0x000af000` => Ende `0x800bf000` < `0x800c47e8`. Dateitabellen-Index 7: `size=262144, LBA=339`; `DEBUG.BIN` ist 262144 B. **Alle 11 zitierten Instruktionen bei `DEBUG.BIN+0x47E8` roh nachgerechnet und identisch** (`00005821, 340c0080, 30e20080, 01ac1024, 00025503, 30e2000f, 24490002, 30e3003f, 24690003, 000a1880, 0105102a`); zusaetzlich die Verzweigungsziele geprueft (`blez a1 -> 0x800c48f0`, `j 0x800c48e4/0x800c48e0`, Rueckwaerts-`bne` nach `0x800c47f8`, `jr ra @0x800c48f0`). Semantik-Abgleich mit `sld.py`: Literal `<0x80` **ohne** Flag-Shift (Verzoegerungsschlitz `0x800c4830 addiu t0,t0,1`), Flag-Bit-0-Literal **mit** Shift (`0x800c4850 srl t4,t4,1` im Verzoegerungsschlitz von `j 0x800c48e0`), `sra 20` = signext12, `(b3>>6)|(off<<2)` + `sll 16 / sra 16` = signext16 - deckungsgleich. |
| 12 | "114 STAGE5-Cuts ohne SLD" = Messartefakt | bestaetigt. `re15_port/shared_assets/PSX/BSS/` hat 156 Verzeichnisse, davon aus STAGE5 nur `ROOM5000/ROOM5001`, aus STAGE6 **keins**. Eigene Zaehlung ueber die Originaldateien: STAGE5-Raeume mit Masken **und** aktivem Atlas = 5030/5031 je 4, 5040/5041 je 6, 5060/5061 je 12, 50A0/50A1 je 10, 50C0/50C1 je 4, 5110/5111 je 4, 5120/5121 je 6, 5140/5141 je 11 => **Summe 114** (= 57 x 2). |
| 13 | Kreuztabelle 2188 Cuts | **exakt reproduziert**: 240 RDTs, davon 34 Platzhalter (<0x60 B), **206 echt**, **2188 Cuts**; `nein/nein 1470`, `nein/ja 238`, `ja/ja 480`, **`ja/nein 0`**; Cuts ohne BSS-Chunk **0**. |
| 14 | SLD-Block = 8bpp-TIM mit CLUT | eigener Voll-Dekode aller aktiven Atlanten: `(magic,flag) = (0x10,0x9)` in **359/359**; Groessen **66080 x320, 61984 x38, 58400 x1**; IMG-Rects `128x256 / 128x240 / 128x226` Halbwoerter, immer `x=0,y=0`. ROOM102 c0: CLUT `len=524 x=0 y=480 w=256 h=1`, IMG `len=61452`, gepackt 10332 B. `FUN_8004ee78.c` woertlich wie zitiert (`prect->x = slot*0x40`, `-0x400` ab slot 0x10, `prect->y = (slot>=0x10)<<8`, `crect->y = DAT_800aca4d + 0x1e0`); Slot `0x15` per `80021de8 ori v0,zero,0x15` / `80021df0 sh v0,0x0(s0)` mit `s0 = 0x800ACA4C` => VRAM (320,256). **Hinweis:** die "321x66080" gelten fuer die 360 *physischen* Bloecke; von den 359 *im Spiel geladenen* sind es 320 - kein Widerspruch, aber die beiden Bezugsmengen unterscheiden sich um genau den ROOM305-Sonderfall. |
| 15 | Anomalie ROOM305 Cut 14 | Byte fuer Byte bestaetigt: Kopf `e0 52 00 38 01 00 03 00`; `L` (STAGE3.BIN+0x1D0F8+5*0x20+14*2) `= 30668 = 0x77cc`; `+0x77cc: 20 02 01 00 d0 10 00 00` (66080, entpackt zu gueltigem TIM `magic 0x10 flag 0x9`); `+0x77c4: 58 61 ff eb 00 00 00 00` => Flag 0; CdRead 15 Sektoren. Und: in meiner eigenen Kreuztabelle hat ROOM3050/3051 Cut 14 **0 Masken** (Kategorie "Masken ohne Atlas" ist leer). |
| 16 | Port-Luecke | **exakt reproduziert**: aktive Atlanten **718**, mit `PRI##.TIM` **416**, ohne **302**, davon mit Masken **188**; Raumliste identisch (1020/1021 12, 1030/1031 12, 1040/1041 5, 1070/1071 8, 5030/5031 4, 5040/5041 6, 5060/5061 12, 50A0/50A1 10, 50C0/50C1 4, 5110/5111 4, 5120/5121 6, 5140/5141 11). `BIN/STAGE1..6.BIN` sha256-identisch **6/6** (zusaetzlich `DEBUG.BIN` identisch). |
| 17 | 17 Platzhalter-BSS | bestaetigt, exakt die genannten Dateien, alle 4 B. |
| 18 | Schwanz ohne Atlas | drei der vier Auspraegungen und ihre Byte-Muster bestaetigt; die vierte Zahl ist falsch -> **W2**. Die Selbstzeiger-Deutung bleibt korrekt als "NICHT BELEGT" markiert. |

## G.3 Fazit

Von 18 Behauptungen halten **15 vollstaendig**. Drei enthalten je **eine falsche Zahl**
(W1 Vielfaches-Aussage, W2 "14" statt 8/9, W3 `0x801AFF80` statt `0x801B0000`); in allen drei
Faellen bleibt der **Mechanismus** unberuehrt und wird durch meine unabhaengige Messung gestuetzt.
Kein Beleg war unauffindbar, keine Adresse falsch zitiert.
