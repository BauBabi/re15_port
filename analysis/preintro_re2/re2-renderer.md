# RE2-Retail Pre-Intro: RENDERER 0x801c1a0c + Element-Zustandsmaschine

Status: **FERTIG** (Teilauftrag a–d beantwortet; offene Punkte in §10)
Datum: 2026-08-30
Teilauftrag: (a) Renderer 0x801c1a0c vollständig disassemblieren, (b) Zustands-Codes
7/11/23/27/39/43/71/75 semantisch klären inkl. Ramp-RATE/Dauer, (c) Herkunft der
Element-Records + Dump, (d) Pan/Zoom/Scroll.

Quelle: `info/re2leon/COMMON/BIN/OPENING.BIN` (13524 B = 0x34D4),
Ladebasis **0x801bfa18**, `datei_offset = addr - 0x801bfa18`.
Disasm-Werkzeug (selbstgeschrieben, MIPS-I R3000 LE; gegen die bekannten Stellen
@0x801bfb1c `sltiu v0,v0,401` und @0x801bfb44 `andi v0,v0,0x0800` verifiziert):
`scratchpad/ovl_dis.py {dis|words|bytes|find} <addr> <n>`, `scratchpad/dump_tab.py`, `scratchpad/scan.py`.

---

## 0. KERNBEFUND in fünf Sätzen

1. Der Renderer baut **pro Element genau EINEN `POLY_FT4`** (40 B) und hängt ihn per
   `AddPrim` in eine OT-Ebene. Es gibt **keine anderen Prim-Typen** im Element-System.
2. Ein-/Ausblenden ist **KEIN Alpha**, sondern eine **Graustufen-Helligkeits-Rampe auf
   der Modulationsfarbe**: `+0x020202` bzw. `−0x020202` (Typ 0) / `−0x010101` (Typ 2)
   pro Frame, geklemmt bei **0x606060 (96,96,96)** und **0**. Das ergibt exakt
   **48 Frames Einblenden**, **48 Frames Ausblenden** (Typ 0) bzw. **96 Frames** (Typ 2).
3. Der Prim ist **semi-transparent (Code 0x2E)** und die TPAGE-Bits erzwingen
   **ABR = 1 = additives Blending (B+F)** — die Rampe ist also ein echtes
   „aus dem Schwarz aufglühen", kein Kreuzblenden zweier Deckflächen.
4. Die Zustands-Codes 7/11/23/27/39/43/71/75 sind **eine Bitmaske**, kein Enum:
   `1=aktiv, 2=Rampe läuft, 4=ein, 8=aus, 0x10=Y-Scroll, 0x20=Zoom auf, 0x40=Zoom zu`.
5. **Pan und Zoom existieren beide**, getaktet von zwei freilaufenden Pulsgebern:
   Y-Scroll **−1 px / 11 Frames**, Zoom **±2 px / 4 Frames** in Breite UND Höhe,
   dabei **immer bildschirmzentriert**.

---

## 1. Renderer 0x801c1a0c — Aufbau

### 1.1 Prolog: zwei freilaufende PULS-Generatoren (@0x801c1a0c..0x801c1abc)

```
0x801c1a10  lhu v0,0x801c2ee0        ; Zähler A
0x801c1a3c  beq v0,zero,0x801c1a60
0x801c1a44  addiu v0,v0,-1           ;  A != 0 : A--
0x801c1a4c  sh   v0,0x801c2ee0
0x801c1a54  sh   zero,0x801c2edc     ;           Puls A := 0
0x801c1a60  addiu v0,zero,10         ;  A == 0 : A := 10
0x801c1a68  sh   v0,0x801c2ee0
0x801c1a74  sh   -1,0x801c2edc       ;           Puls A := -1
```
Identisch für Zähler B @0x801c1a78..0x801c1abc, Reload **3** @0x801c1aa4.

| RAM | Bedeutung | Reload | **Periode** | Beleg |
|---|---|---|---|---|
| `0x801c2ee0` u16 | Zähler A | **10** | — | @0x801c1a60 `addiu v0,zero,10` |
| `0x801c2edc` s16 | **Puls A** = −1 in 1 von 11 Frames, sonst 0 | | **11 Frames** | @0x801c1a74 / @0x801c1a54 |
| `0x801c2ee2` u16 | Zähler B | **3** | — | @0x801c1aa4 `addiu v0,zero,3` |
| `0x801c2ede` s16 | **Puls B** = −1 in 1 von 4 Frames, sonst 0 | | **4 Frames** | @0x801c1ab0 / @0x801c1a94 |

Das sind die **einzigen** Bewegungs-Taktgeber des ganzen Systems.

### 1.2 Schleifen-Setup (@0x801c1abc..0x801c1af4)

```
0x801c1ac0  lw   s7,0x801c2ed8        ; s7 = Zeiger auf Element-Array
0x801c1ac8  lbu  v0,0x800ce5e0        ; EXE: Doppelpuffer-Index (0/1)
0x801c1ad0  lh   t0,0x801c24cc        ; t0 = Element-ANZAHL
0x801c1ad4  sll  v1,v0,2
0x801c1ad8  addu v1,v1,v0             ; idx*5
0x801c1adc  sll  v1,v1,8              ; idx*0x500 (=1280)
0x801c1ae4  addiu v0,0x801c24d8       ; Prim-Scratch-Basis
0x801c1ae8  addu a3,v1,v0             ; a3 = 0x801c24d8 + idx*1280
0x801c1aec  addiu s4,s7,2             ; s4 = Record+2
0x801c1af0  addiu s6,a3,37            ; s6 = Prim+37
```

**Prim-Puffer:** `0x801c24d8 + dbuf*1280`, **1280 B je Puffer**, Prim-Stride 40
⇒ **harte Kapazität 32 Prims/Frame**. Deckt sich mit der 32-Bit-Maske des Setters (§7).
Speicherbild am Dateiende: `0x801c24d8 + 2*0x500 = 0x801c2ed8` = exakt die
Array-Zeiger-Variable ⇒ Puffer stößt lückenlos an, Layout bestätigt.

### 1.3 Element-Schleife (@0x801c1af8..0x801c1f2c)

```
0x801c1afc  lbu  v0,-1(s4)            ; Record+1
0x801c1b04  andi v0,v0,0x0010
0x801c1b08  beq  v0,zero,0x801c1b14
0x801c1b10  sb   zero,0(s7)           ; Bit4 gesetzt ⇒ Record+0 := 0 (siehe §5)
0x801c1b14  lbu  a0,0(s7)             ; ZUSTAND
0x801c1b1c  andi v0,a0,0x0001
0x801c1b20  beq  v0,zero,0x801c1f1c   ; Bit0 aus ⇒ Element überspringen
0x801c1b30  srl  v1,v0,4              ; TYP = (Record+1) >> 4
0x801c1b38  beq  v1,1 -> 0x801c1c5c   ; Typ 1
0x801c1b48  beq  v1,0 -> 0x801c1b6c   ; Typ 0
0x801c1b5c  beq  v1,2 -> 0x801c1cbc   ; Typ 2
0x801c1b64  j    0x801c1e28           ; sonst: ungefiltert Prim bauen
...
0x801c1f1c  addiu s4,s4,12            ; Record-Stride = 12
0x801c1f28  bne  t0,zero,0x801c1afc
0x801c1f2c  addiu s7,s7,12
```

---

## 2. (a) RECORD-LAYOUT — 12 Byte, Array @`*(0x801c2ed8)`, Anzahl @`0x801c24cc`

| Off | Größe | Feld | Beleg |
|---|---|---|---|
| **+0** | u8 | **ZUSTAND** (Bitmaske, §3) | `lbu a0,0(s7)` @0x801c1b14 |
| **+1** | u8 | **TYP** = `>>4`; Bit 4 wirkt zusätzlich als „aus" | `srl v1,v0,4` @0x801c1b30 / `andi 0x10` @0x801c1b04 |
| **+2** | u8 | **OT-Ebene** (Zeichentiefe) | `lbu a0,0(s4)` @0x801c1ef4, `sll a0,a0,2` @0x801c1f08 |
| **+3** | u8 | **Sprite-Index** (10-B-Tabelle) | `lbu v1,1(s4)` @0x801c1b6c / 0x801c1c64 / 0x801c1cbc |
| **+4** | u16 | **X** (Bildschirm) | `lhu a2,2(s4)` @0x801c1c4c / 0x801c1ca4 |
| **+6** | u16 | **Y** (Bildschirm) — Variante A scrollt hier | `lhu fp,4(s4)` @0x801c1c50 / 0x801c1cac |
| **+8** | u32 | **FARBE** `0x00BBGGRR` = das Ein-/Ausblend-Register | `lw v0,6(s4)` @0x801c1ba0 usw. |

Da die Rampe alle drei Kanäle gleich bewegt (`0x020202`), ist die Farbe **immer
grau (R=G=B)** — praktisch ein Helligkeitsregler 0…96.

### 2.1 Die drei TYPEN

| Typ | Handler | Sprite-Tabelle | Prim-Code | TPAGE-Zusatz | Position | Ausblenden |
|---|---|---|---|---|---|---|
| **0** | 0x801c1b6c | **0x801c208c** (14 × 10 B) | **0x2E** FT4 semi-transp. | **160 (0xA0)** | Record +4/+6 | −0x020202/F (**48 F**) |
| **1** | 0x801c1c5c | **0x801c2118** (22 × 10 B) | **0x2C** FT4 **opak** | **32 (0x20)** | Record +4/+6 | **sofort** (1 F) |
| **2** | 0x801c1cbc | **0x801c208c** | **0x2E** FT4 semi-transp. | **160 (0xA0)** | **immer zentriert** | −0x010101/F (**96 F**) |

Belege Tabellenbasis: `addiu v1,v1,8332` (=0x208C) @0x801c1b84 / 0x801c1cd4;
`addiu a0,a0,8472` (=0x2118) @0x801c1c6c. Stride 10: `sll v0,v1,2; addu v0,v0,v1; sll v0,v0,1`
@0x801c1b74..0x801c1b7c.
Belege Prim-Code: `lui a0,0x2e00` @0x801c1c14 (T0), `lui v0,0x2c00` @0x801c1c9c (T1),
`lui a0,0x2e00` @0x801c1d64 (T2); addiert auf die Farbe @0x801c1c24/0x801c1cb0/0x801c1d74.
Belege TPAGE-Zusatz: `addiu t0,zero,160; sh t0,24(sp)` @0x801c1c10/0x801c1c18 (T0),
`addiu t0,zero,32` @0x801c1c98 (T1), `addiu t0,zero,160` @0x801c1d60 (T2).

**Typ 1 setzt die Farbe FEST auf neutral:**
```
0x801c1c5c  lui v1,0x0080
0x801c1c60  ori v1,v1,0x8080
0x801c1c70  sw  v1,6(s4)      ; Farbe := 0x00808080 (= 128,128,128 = 1.0)
0x801c1c88  andi v0,v0,0x0008
0x801c1c94  sb  zero,0(s7)    ; „Ausblenden" ⇒ SOFORT aus, keine Rampe
```

---

## 3. (b) DIE ZUSTANDS-CODES SIND EINE BITMASKE (Record +0)

| Bit | Wert | Bedeutung | Beleg |
|---|---|---|---|
| 0 | 0x01 | **aktiv/sichtbar**, sonst Element übersprungen | `andi v0,a0,0x0001` @0x801c1b1c |
| 1 | 0x02 | **Rampe läuft** (wird am Rampenende mitgelöscht) | `andi v0,v0,0x00f9` @0x801c1bcc |
| 2 | 0x04 | **EINBLENDEN** (Helligkeit rauf) | `andi v0,a0,0x0004` @0x801c1b8c / 0x801c1cdc |
| 3 | 0x08 | **AUSBLENDEN** (Helligkeit runter) | `andi v0,v0,0x0008` @0x801c1bdc / 0x801c1c88 / 0x801c1d2c |
| 4 | 0x10 | **Variante A** = Y-SCROLL (nur Typ 0) | `andi v0,v0,0x0010` @0x801c1c28 |
| 5 | 0x20 | **Variante B** = ZOOM AUF (nur Typ 2) | `andi v0,v0,0x0020` @0x801c1d78 |
| 6 | 0x40 | **Variante C** = ZOOM ZU (nur Typ 2) | `andi v0,v0,0x0040` @0x801c1dbc |

| Code | Hex | zerlegt | Bedeutung |
|---|---|---|---|
| **7** | 0x07 | 1+2+4 | aktiv, Rampe, **einblenden** |
| **11** | 0x0B | 1+2+8 | aktiv, Rampe, **ausblenden** |
| **23** | 0x17 | 7+0x10 | einblenden **+ Y-Scroll** |
| **27** | 0x1B | 11+0x10 | ausblenden **+ Y-Scroll** |
| **39** | 0x27 | 7+0x20 | einblenden **+ Zoom auf** |
| **43** | 0x2B | 11+0x20 | ausblenden **+ Zoom auf** |
| **71** | 0x47 | 7+0x40 | einblenden **+ Zoom zu** |
| **75** | 0x4B | 11+0x40 | ausblenden **+ Zoom zu** |

### 3.1 Die RAMPE — exakte Raten, Grenzen, Dauern

**EINBLENDEN (Bit 2)** — Typ 0 @0x801c1b8c..0x801c1bd0, Typ 2 @0x801c1cdc..0x801c1d20 (identisch):
```
0x801c1b94  lui  a0,0x0002
0x801c1b98  ori  a0,a0,0x0202      ; SCHRITT = +0x00020202
0x801c1b9c  lui  v1,0x0060
0x801c1ba0  lw   v0,6(s4)
0x801c1ba4  ori  v1,v1,0x605f      ; GRENZE = 0x0060605f
0x801c1ba8  addu v0,v0,a0
0x801c1bac  slt  v1,v1,v0
0x801c1bb0  beq  v1,zero,0x801c1bd4
0x801c1bb4  sw   v0,6(s4)
0x801c1bc0  sw   0x00606060,6(s4)  ; klemmen
0x801c1bcc  andi v0,v0,0x00f9      ; Zustand &= ~0x06  (Rampe beendet)
```
- **+2 je Kanal je Frame** (`0x00020202` @0x801c1b98 / @0x801c1ce8)
- **Ziel 0x606060 = 96** (`0x00606060` @0x801c1bbc / @0x801c1d0c)
- **Dauer 0 → 96 = genau 48 Frames**; im 48. Frame wird geklemmt und der Zustand
  von `7` auf **`5` (0x05)** reduziert (Element bleibt sichtbar, Rampe aus).

**AUSBLENDEN Typ 0 (Bit 3)** @0x801c1bd4..0x801c1c0c:
```
0x801c1be4  lui  v1,0xfffd
0x801c1bec  ori  v1,v1,0xfdfe      ; SCHRITT = 0xFFFDFDFE = −0x00020202
0x801c1bf0  addu v0,v0,v1
0x801c1bf4  bgtz v0,0x801c1c10
0x801c1bfc  sw   zero,6(s4)        ; auf 0 klemmen
0x801c1c08  andi v0,v0,0x00f7      ; Zustand &= ~0x08
```
⇒ **96 → 0 in genau 48 Frames**, Zustand `11` → **`3` (0x03)**.

**AUSBLENDEN Typ 2 (Bit 3)** @0x801c1d24..0x801c1d5c — **andere Rate**:
```
0x801c1d34  lui  v1,0xfffe
0x801c1d3c  ori  v1,v1,0xfeff      ; SCHRITT = 0xFFFEFEFF = −0x00010101
```
⇒ **96 → 0 in genau 96 Frames** (halbe Geschwindigkeit, für die Logo-Zooms).

**AUSBLENDEN Typ 1:** keine Rampe, `sb zero,0(s7)` @0x801c1c94 ⇒ 1 Frame.

### 3.2 Blend-Modus (das eigentliche „Wie sieht die Blende aus")

Prim-Code **0x2E** = `POLY_FT4` + **ABE (semi-transparent)** + Textur-**Modulation**.
Der Semi-Transparenz-MODUS kommt aus den TPAGE-Bits, und die werden hier erzwungen:
```
0x801c1ebc  lhu  t0,24(sp)         ; 160 (Typ 0/2) bzw. 32 (Typ 1)
0x801c1eb0  lhu  v0,0(s5)          ; TPAGE-Basis aus der Sprite-Tabelle
0x801c1ec4  addu v0,t0,v0
0x801c1ec8  sh   v0,-15(s6)        ; ⇒ Prim +22 = TPAGE
```
- **160 = 0xA0** → `ABR = (0xA0>>5)&3 =` **1**, `TP = (0xA0>>7)&3 =` **1 = 8 bpp CLUT**
- **32 = 0x20** → `ABR =` **1**, `TP =` **0 = 4 bpp CLUT**
- PSX-**ABR 1 = B + F (additiv)**.

⇒ Typ 0 und Typ 2 blenden **additiv** ein: Helligkeit 0 = unsichtbar, 96 = 75 % von
neutral. Typ 1 ist opak (Code 0x2C ohne ABE), ABR wirkungslos.

---

## 4. (d) PAN / ZOOM / SCROLL

### 4.1 Variante A (0x10, nur Typ 0): vertikaler SCROLL / Text-Crawl
```
0x801c1c28  andi v0,v0,0x0010
0x801c1c2c  beq  v0,zero,0x801c1c4c
0x801c1c34  lhu  v0,4(s4)          ; Record+6 = Y
0x801c1c3c  lhu  v1,0x801c2edc     ; Puls A (−1 alle 11 Frames)
0x801c1c44  addu v0,v0,v1
0x801c1c48  sh   v0,4(s4)
```
**RATE: Y −1 px alle 11 Frames** (≈ 0,0909 px/Frame), nach oben.
Geschrieben wird in den **Record** ⇒ pro Element eigener Scroll-Zustand.

### 4.2 Typ 2: Position IMMER zentriert, W/H sind die Zoom-Größe
```
0x801c1df8  lbu  v1,8(s5)          ; W
0x801c1dfc  addiu v0,zero,320
0x801c1e00  subu v0,v0,v1
0x801c1e04  srl  v1,v0,31          ; Vorzeichen-Korrektur (Div. Richtung 0)
0x801c1e08  addu v0,v0,v1
0x801c1e0c  srl  a2,v0,1           ; X := (320 − W)/2
0x801c1e10  lbu  v1,9(s5)          ; H
0x801c1e14  addiu v0,zero,240
0x801c1e24  srl  fp,v0,1           ; Y := (240 − H)/2
```
Konstanten **320 @0x801c1dfc**, **240 @0x801c1e14**.

### 4.3 Variante B (0x20): ZOOM AUF
```
0x801c1d88  lh   v0,0x801c2ede     ; Puls B (−1 alle 4 Frames)
0x801c1d8c  lbu  v1,8(s5)
0x801c1d90  sll  v0,v0,1           ; −2 oder 0
0x801c1d94  subu v1,v1,v0          ; W −= (−2)  ⇒ W += 2
0x801c1d98  sb   v1,8(s5)
     …gleiches für H @0x801c1d9c..0x801c1db0
```
**RATE: W += 2 und H += 2 alle 4 Frames** (0,5 px/Frame je Achse), zentriert.

### 4.4 Variante C (0x40): ZOOM ZU
```
0x801c1dcc  lh   v0,0x801c2ede
0x801c1dd0  lbu  v1,8(s5)
0x801c1dd4  sll  v0,v0,1
0x801c1dd8  addu v1,v1,v0          ; W −= 2
0x801c1ddc  sb   v1,8(s5)
     …gleiches für H @0x801c1de0..0x801c1df4
```
**RATE: W −= 2 und H −= 2 alle 4 Frames**, zentriert.

⚠️ **Zwei Fallen für die Übernahme:**
1. B/C schreiben in die **SPRITE-TABELLE** (`s5+8/+9`), NICHT in den Record. Der
   Zoom-Zustand hängt am Sprite-Eintrag; zwei Elemente mit gleichem Sprite-Index
   würden sich die Größe teilen und doppelt schnell zoomen.
2. W/H sind **`u8`** (`lbu`/`sb`) ⇒ Wertebereich 0..255, Überlauf wird nicht abgefangen.
3. Typ 0 kann **nur** scrollen, Typ 2 **nur** zoomen. Kein Element kann beides;
   die Bits werden im jeweils anderen Typ-Zweig nie gelesen.

### 4.5 Die zwei tatsächlichen Zoom-Fahrten in der Sequenz

| Element | Sprite | Start-W/H | Zustand | gesetzt bei | sichtbar bis | ⇒ Fahrt |
|---|---|---|---|---|---|---|
| **10** | A[4] | **200 × 200** | **71** (ein + zu) | Phase-2-Ende, `mask:=1024` @0x801c08a4, `f64(71,0)` @0x801c08b0 | Fade-out ab Phase-3 t=204 (`f64(75,0)` @0x801c0948), nach 96 F unsichtbar ⇒ t≈300 | **200 → ~50 px** über ~300 Frames |
| **19** | A[9] | **20 × 20** | **39** (ein + auf) | Phase-4 t=580 | Fade-out ab Phase-5 t=261 (Zustand 43), nach 96 F unsichtbar ⇒ t≈357 | **20 → ~198 px** über ~357 Frames |

Das sind die beiden Logo-Einstellungen (Sprite A[4] tpage 0x15 / A[9] tpage 0x16,
beide Texturbereich **189 × 190**): eines schrumpft in die Bildmitte, das andere
wächst aus ihr heraus. Belege für die Masken/Zustände: `addiu v0,zero,1024` @0x801c08a4
und @0x801c0938, `addiu a0,zero,71` @0x801c08a0, `addiu a0,zero,75` @0x801c0944.

---

## 5. (c) HERKUNFT DER RECORDS — drei statische Tabellen im Overlay

Der Array-Zeiger `0x801c2ed8` wird an **genau drei** Stellen gesetzt (verifiziert per
Voll-Scan `scan.py lui_addiu 0x801c21f4 0x801c24ac` — keine weiteren Adressbildungen
in diesen Bereich):

| Setz-Stelle | Anzahl @0x801c24cc | Array | Datei-Offset | Größe |
|---|---|---|---|---|
| `sw v0,0x801c2ed8` @**0x801c0120** | **28** (`addiu v0,zero,28` @0x801c0108) | **0x801c21f4** | 0x27DC | 336 B |
| `sw v0,0x801c2ed8` @**0x801c01e8** | **12** (`addiu v0,zero,12` @0x801c01d0) | **0x801c2344** | 0x292C | 144 B |
| `sw v0,0x801c2ed8` @**0x801c030c** | **18** (`addiu v0,zero,18` @0x801c02f4) | **0x801c23d4** | 0x29BC | 216 B |

Die drei Blöcke liegen **lückenlos hintereinander** (0x21f4 + 336 = 0x2344,
+144 = 0x23d4, +216 = 0x24ac) und stoßen an die Datei-ID-Tabelle @0x801c24ac.
Direkt davor liegen die beiden Sprite-Tabellen:

```
0x801c2068  MODE-Sprungtabelle    4 Worte
0x801c2078  STATE-Sprungtabelle   5 Worte
0x801c208c  SPRITE-TABELLE A     14 × 10 B   (Typ 0 + Typ 2)
0x801c2118  SPRITE-TABELLE B     22 × 10 B   (Typ 1)
0x801c21f4  RECORDS Satz 1       28 × 12 B
0x801c2344  RECORDS Satz 2       12 × 12 B
0x801c23d4  RECORDS Satz 3       18 × 12 B
0x801c24ac  Datei-ID-Tabelle     0x00ad,ae,af,b0,b1,b5,b6,b7,b8,b9
```

### 5.1 Sprite-Tabellen-Zeile (10 Byte)

| Off | Größe | Feld | Beleg |
|---|---|---|---|
| +0 | u16 | **TPAGE-Basis** (VRAM-Seite) | `lhu v0,0(s5)` @0x801c1eb0 |
| +2 | u16 | **CLUT-Id** | `lhu v0,2(s5)` @0x801c1e90 |
| +4 | u8 | **U0** | `lbu s1,4(s5)` @0x801c1e30 |
| +5 | u8 | **V0** | `lbu s0,5(s5)` @0x801c1e38 |
| +6 | u8 | **dU** (Texturbreite) | `lbu v1,6(s5)` @0x801c1e34 |
| +7 | u8 | **dV** (Texturhöhe) | `lbu v0,7(s5)` @0x801c1e3c |
| +8 | u8 | **W** (Bildschirmbreite) — Zoom schreibt hier | `lbu s2,8(s5)` @0x801c1e28 |
| +9 | u8 | **H** (Bildschirmhöhe) — Zoom schreibt hier | `lbu s3,9(s5)` @0x801c1e2c |

`W/H ≠ dU/dV` ist erlaubt ⇒ **freie Skalierung**. Genutzt wird das massiv:
A[4] zeichnet 189×190 Texel auf 200×200 px, A[9] dieselbe Größe auf **20×20** px.

### 5.2 SPRITE-TABELLE A @0x801c208c (Typ 0 / Typ 2), 14 Einträge

| idx | Adresse | tpage | CLUT | U0 | V0 | dU | dV | W | H | benutzt von |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0x801c208c | 0x0005 | 0x7800 | 0 | 0 | 160 | 160 | 160 | 160 | El. 2 |
| 1 | 0x801c2096 | 0x0006 | 0x7800 | 32 | 0 | 159 | 160 | 160 | 160 | El. 3 |
| 2 | 0x801c20a0 | 0x0007 | 0x7800 | 64 | 0 | 160 | 160 | 160 | 160 | El. 4 |
| 3 | 0x801c20aa | 0x0008 | 0x7800 | 96 | 0 | 159 | 160 | 160 | 160 | El. 5 |
| **4** | 0x801c20b4 | 0x0015 | 0x78c0 | 0 | 0 | **189** | **190** | **200** | **200** | **El. 10 (Zoom zu)** |
| 5 | 0x801c20be | 0x000a | 0x7840 | 0 | 0 | 160 | 160 | 160 | 160 | El. 11 |
| 6 | 0x801c20c8 | 0x000a | 0x7840 | 0 | 160 | 159 | 80 | 160 | 160 | El. 12 |
| 7 | 0x801c20d2 | 0x000b | 0x7840 | 32 | 0 | 160 | 160 | 160 | 160 | El. 13 |
| 8 | 0x801c20dc | 0x000b | 0x7840 | 32 | 160 | 159 | 80 | 160 | 80 | El. 14 |
| **9** | 0x801c20e6 | 0x0016 | 0x7900 | 64 | 0 | **189** | **190** | **20** | **20** | **El. 19 (Zoom auf)** |
| 10 | 0x801c20f0 | 0x000c | 0x7880 | 64 | 0 | 160 | 160 | 160 | 160 | El. 20 |
| 11 | 0x801c20fa | 0x000c | 0x7880 | 64 | 160 | 160 | 80 | 160 | 80 | El. 21 |
| 12 | 0x801c2104 | 0x000d | 0x7880 | 96 | 0 | 159 | 160 | 160 | 160 | El. 22 |
| 13 | 0x801c210e | 0x000d | 0x7880 | 96 | 160 | 159 | 80 | 160 | 80 | El. 23 |

Muster: **ein Standbild = VIER Quads** (2 × 2 zu je 160 × 160 px), weil eine
320 px breite 8-bpp-Textur nicht in eine 256-Texel-TPAGE passt. Drei solcher
Vierergruppen (CLUT 0x7800 / 0x7840 / 0x7880) = **drei Standbilder**,
dazu zwei Einzel-Logos (CLUT 0x78c0 / 0x7900).

### 5.3 SPRITE-TABELLE B @0x801c2118 (Typ 1), 22 Einträge

Alle: tpage **0x18** (idx 0–6) bzw. **0x19** (idx 7–21), CLUT **0x7940**,
U0 = 0, dU = W = **255**, V0 = 0/32/64/96/128/160/192 (bzw. 0/16-Raster),
dV = H = **32** oder **16**.
⇒ **255 × 32 px breite Zeilenstreifen**, aus einer 4-bpp-Textur, gestapelt.
Zusammen mit der Record-Position X = 32, Y = **180 / 196** ist das eindeutig
die **Untertitel-/Erzähltext-Zeile am unteren Bildrand**.

### 5.4 RECORDS Satz 1 @0x801c21f4 (28 × 12 B) — die Hauptsequenz

| El | Adresse | +0 Zust. | +1 Typ | +2 OT | +3 Spr | X | Y | Rohbytes |
|---|---|---|---|---|---|---|---|---|
| 0 | 0x801c21f4 | 06 | 10 → **T1** | 16 | 0 | 32 | 180 | `06 10 10 00 20 00 b4 00 00000000` |
| 1 | 0x801c2200 | 06 | 10 → T1 | 16 | 0 | 32 | 180 | identisch zu El. 0 |
| **2** | 0x801c220c | 00 | 00 → **T0** | **32** | 0 | **0** | **10** | `00 00 20 00 00 00 0a 00 00000000` |
| **3** | 0x801c2218 | 00 | 00 → T0 | 32 | 1 | **160** | **10** | `00 00 20 01 a0 00 0a 00 …` |
| **4** | 0x801c2224 | 00 | 00 → T0 | 32 | 2 | **0** | **170** | `00 00 20 02 00 00 aa 00 …` |
| **5** | 0x801c2230 | 00 | 00 → T0 | 32 | 3 | **160** | **170** | `00 00 20 03 a0 00 aa 00 …` |
| 6 | 0x801c223c | 06 | 11 → T1 | 16 | 1 | 32 | 180 | Untertitel-Paar |
| 7 | 0x801c2248 | 06 | 11 → T1 | 16 | 1 | 32 | 180 | " |
| 8 | 0x801c2254 | 06 | 12 → T1 | 16 | 2 | 32 | 180 | " |
| 9 | 0x801c2260 | 06 | 12 → T1 | 16 | 2 | 32 | 180 | " |
| **10** | 0x801c226c | 00 | **20 → T2** | **21** | 4 | 160 | 120 | `00 20 15 04 a0 00 78 00 …` (Pos. ungenutzt, zentriert) |
| **11** | 0x801c2278 | 00 | 01 → T0 | 32 | 5 | 0 | 0 | Vierergruppe 1 |
| **12** | 0x801c2284 | 00 | 01 → T0 | 32 | 6 | 0 | 160 | " |
| **13** | 0x801c2290 | 00 | 01 → T0 | 32 | 7 | 160 | 0 | " |
| **14** | 0x801c229c | 00 | 01 → T0 | 32 | 8 | 160 | 160 | " |
| 15/16 | 0x801c22a8/b4 | 06 | 13 → T1 | 16 | 3 | 32 | 180 | Untertitel-Paar |
| 17/18 | 0x801c22c0/cc | 06 | 14 → T1 | 16 | 4 | 32 | 180 | " |
| **19** | 0x801c22d8 | 00 | **20 → T2** | **21** | 9 | 160 | 120 | `00 20 15 09 a0 00 78 00 …` |
| **20** | 0x801c22e4 | 00 | 02 → T0 | 32 | 10 | 0 | 0 | Vierergruppe 2 |
| **21** | 0x801c22f0 | 00 | 02 → T0 | 32 | 11 | 0 | 160 | " |
| **22** | 0x801c22fc | 00 | 02 → T0 | 32 | 12 | 160 | 0 | " |
| **23** | 0x801c2308 | 00 | 02 → T0 | 32 | 13 | 160 | 160 | " |
| 24/25 | 0x801c2314/20 | 06 | 15 → T1 | 16 | 5 | 32 | 180 | Untertitel-Paar |
| 26/27 | 0x801c232c/38 | 06 | 16 → T1 | 16 | 6 | 32 | 180 | " |

**Damit ist die Bühnenordnung belegt:**
- **OT 32** = die Standbilder (hinten)
- **OT 21** = die Logos
- **OT 16** = die Untertitelzeilen (vorne)

Und die Gruppen entsprechen exakt den Masken aus der Phasen-Timeline:
`{2,3,4,5}` = Maske 60 = Standbild 1, `{11..14}` = Maske 30720 (0x7800) = Standbild 2,
`{20..23}` = Standbild 3, `{10}` = Maske 1024 = Logo A, `{19}` = Logo B.
(Belegt: `addiu v0,zero,60` @0x801c088c, `addiu v0,zero,30720` @0x801c08d4/0x801c09b4,
`addiu v0,zero,1024` @0x801c08a4/0x801c0938.)

Merke: die **Standbild-Vierergruppe 0** sitzt bei Y = **10** und **170**, also
320 × 320 px auf einem 240 px hohen Schirm — sie hängt unten heraus, weil sie mit
Zustand **23 (Y-Scroll)** nach oben durchgefahren wird. Gruppen 1 und 2 sitzen bei
Y = 0/160 und werden nur ein-/ausgeblendet.

### 5.5 RECORDS Satz 2 @0x801c2344 (12) und Satz 3 @0x801c23d4 (18)

**Beide Sätze bestehen AUSSCHLIESSLICH aus Typ-1-Records** (Untertitelzeilen),
angeordnet als identische Paare, Sprite-Indizes 7–12 (Satz 2) bzw. 13–21 (Satz 3),
X = 32, Y = 180 bzw. 196. Sie gehören zu den STATE-Maschinen
`state2 = 0x801c0d58` (12 Phasen, Tabelle @0x801bfa6c) und
`state3 = 0x801c1304` — den reinen **Text-Sequenzen**.

Beispiel state2, Phase 1 @0x801c0e00:
```
0x801c0e08  addiu v0,zero,250      ; t == 250
0x801c0e10  addiu v0,zero,3        ; Maske {0,1}
0x801c0e1c  addiu a0,zero,7        ; einblenden
0x801c0e20  jal   0x801c1f64
0x801c0e30  addiu v0,zero,523      ; t == 523
0x801c0e38  addiu a0,zero,11       ; ausblenden
```
⇒ eine Textzeile steht **273 Frames** (250 → 523), dann Ausblenden.

---

## 6. Der Prim-Aufbau @0x801c1e28..0x801c1f18

```
0x801c1e28  lbu  s2,8(s5)      ; W
0x801c1e2c  lbu  s3,9(s5)      ; H
0x801c1e4c  addu s2,a2,s2      ; x1 = X + W
0x801c1e50  addu s3,fp,s3      ; y1 = Y + H
0x801c1e54  addu s1,s1,v1      ; u1 = U0 + dU
0x801c1e58  jal  0x8008fa80    ; SetPolyFT4(prim)   [Ghidra-Symbol, li v0,9 / li v0,0x2c]
0x801c1e5c  addu s0,s0,v0      ; v1 = V0 + dV
```

| Store | Prim-Offset | Inhalt |
|---|---|---|
| `sw t0,-33(s6)` @0x801c1e68 | **+4** | `Code<<24 \| Farbe` (0x2E… bzw. 0x2C…) |
| `sh a2,-29(s6)` @0x801c1e74 | +8 | x0 = X |
| `sh fp,-27(s6)` @0x801c1e70 | +10 | y0 = Y |
| `sb u0,-25(s6)` @0x801c1e80 | +12 | u0 |
| `sb v0,-24(s6)` @0x801c1e8c | +13 | v0 |
| `sh clut,-23(s6)` @0x801c1ea0 | +14 | CLUT |
| `sh s2,-21(s6)` @0x801c1e94 | +16 | x1 = X+W |
| `sh fp,-19(s6)` @0x801c1e98 | +18 | y1 = Y |
| `sb s1,-17(s6)` @0x801c1e9c | +20 | u1 = U0+dU |
| `sb v0,-16(s6)` @0x801c1eac | +21 | v1 = V0 |
| `sh …,-15(s6)` @0x801c1ec8 | **+22** | **TPAGE = Tab[+0] + 160/32** |
| `sh a2,-13(s6)` @0x801c1eb4 | +24 | x2 = X |
| `sh s3,-11(s6)` @0x801c1eb8 | +26 | y2 = Y+H |
| `sb u0,-9(s6)` @0x801c1ef0 | +28 | u2 = U0 |
| `sb s0,-8(s6)` @0x801c1ed4 | +29 | v2 = V0+dV |
| `sh s2,-5(s6)` @0x801c1ed8 | +32 | x3 = X+W |
| `sh s3,-3(s6)` @0x801c1edc | +34 | y3 = Y+H |
| `sb s1,-1(s6)` @0x801c1ee0 | +36 | u3 = U0+dU |
| `sb s0,0(s6)` @0x801c1ee4 | +37 | v3 = V0+dV |

Exakt das PsyQ-`POLY_FT4`-Layout, **40 B** (`addiu a3,a3,40` @0x801c1eec,
`addiu s6,s6,40` @0x801c1f00).

**Einhängen:**
```
0x801c1ef4  lbu  a0,0(s4)        ; Record+2 = OT-Ebene
0x801c1efc  lw   v0,0x800ce22c   ; EXE: Zeiger auf das aktuelle OT
0x801c1f08  sll  a0,a0,2
0x801c1f0c  jal  0x8008f918      ; AddPrim(OT + ebene*4, prim)  [Ghidra-Symbol]
0x801c1f10  addu a0,v0,a0
```

**EXE-Helfer verifiziert** (Ghidra-Symbole aus `ghidra_re2_Leon.txt`):
- **0x8008fa80 = `SetPolyFT4`** (`li v0,0x9` @0x8008fa80, `li v0,0x2c` @0x8008fa88)
- **0x8008f918 = `AddPrim`** (`lui a2,0xff` … PsyQ-`addPrim`-Makro @0x8008f918..0x8008f950)

---

## 7. Der Zustands-Setter 0x801c1f64(zustand, hide_others)

```
0x801c1f68  lw   v1,0x801c2ed8       ; Element-Array
0x801c1f6c  addu a2,zero,zero        ; i = 0
0x801c1f70  andi a1,a1,0x00ff
0x801c1f78  lw   v0,0x801c2ee8       ; MASKE (32 Bit)
0x801c1f80  srlv v0,v0,a2
0x801c1f84  andi v0,v0,0x0001
0x801c1f88  beq  v0,zero,0x801c1f98
0x801c1f94  sb   a0,0(v1)            ; Maskenbit i ⇒ Zustand := a0
0x801c1f98  beq  a1,zero,0x801c1fa4
0x801c1fa0  sb   zero,0(v1)          ; sonst + hide_others ⇒ Zustand := 0
0x801c1fa4  lbu  v0,1(v1)
0x801c1fac  andi v0,v0,0x0010
0x801c1fb8  sb   zero,0(v1)
0x801c1fbc  addiu a2,a2,1
0x801c1fc4  lh   v0,0x801c24cc
```
Maske ist **32 bit** (`srlv`) ⇒ max. 32 Elemente, deckungsgleich mit der Prim-Kapazität.
In der gesamten Phasen-Timeline wird `hide_others` (a1) fast durchweg **0** übergeben
(`addu a1,zero,zero` @0x801c089c/0x801c08b4/0x801c08e8/0x801c091c/0x801c0920/0x801c094c/
0x801c09c4/0x801c0b50/0x801c0e24/0x801c0e4c) — nur der harte „alles aus"-Aufruf nutzt
`addiu a1,zero,1` (z. B. @0x801c0588). **Deshalb ÜBERLAPPEN Aus- und Einblendungen.**

---

## 8. Vollbild-Blende 0x801c1fe0 (separates EXE-System, NICHT das Element-System)

```
0x801c1fe0  sll  v0,a0,2 / addu v0,v0,a0 / sll v0,v0,2 / subu v0,v0,a0 / sll v0,v0,2
                                          ; v0 = a0 * 76
0x801c1ff8  addiu v1,v1,-996             ; v1 = 0x800efc1c + slot*76   (Stride 76 = 19 Worte)
0x801c2000  lbu  v0,19(v1)               ; vorhandenes Code-Byte
0x801c2004  sh   a1,0(v1)
0x801c2008  sll  v0,v0,24
0x801c200c  or   a2,a2,v0                ; a2 = Farbe | (Code<<24)
0x801c2010  sw   a2,16(v1)
0x801c2014  beq  a3,zero,0x801c204c
0x801c2018  sw   a2,32(v1)
   a3 != 0 : +20 := rect[0]; +24 := rect[4]; +36 := rect[0]; +40 := rect[4]
   a3 == 0 : +20 := 0; +24 := 0x00f00140; +36 := 0; +40 := 0x00f00140
0x801c204c  lui  v0,0x00f0
0x801c2050  ori  v0,v0,0x0140            ; 0x00f00140 = (w=0x140=320, h=0x00f0=240)
```
Zwei identische Rechteck-Prims je Slot (+16 und +32) = Doppelpuffer.
Phase 0 ruft `0x801c1fe0(3, 0, 0x00ffffff, 0)` @0x801c066c..0x801c0680
(auch state2 Phase 0: `addiu a0,zero,3` @0x801c0d8c, `lui a2,0x00ff / ori a2,0xffff`
@0x801c0d94/0x801c0d98, `jal 0x801c1fe0` @0x801c0d9c) ⇒ **weiße Vollbildfläche**.

⇒ Der Weiß-Blitz/Weiß-Grund am Sequenzbeginn ist ein **EXE-Fade-Slot**, unabhängig
von der Element-Rampe. Für unsere Übernahme sind das **zwei getrennte Mechanismen**.

---

## 9. Sprungtabellen (verifiziert per `words`)

```
MODE  @0x801c2068 : 0x801c0474, 0x801c0630, 0x801c051c, 0x801c0388
STATE @0x801c2078 : 0x801c0638, 0x801c0638, 0x801c0d58, 0x801c1304, 0x801bfcfc
PHASE(state0/1) @0x801bfa3c (11) : 066c 06c0 07b0 08c0 09d0 0aa0 0c48 0c94 0cb4 0cd8 0d08
PHASE(state2)   @0x801bfa6c (12) : 0d8c 0de0 0e58 0f14 0fb8 1030 10e0 11f4 1240 1260 1284 12b4
```
(alle mit Präfix 0x801c…; deckungsgleich mit `analysis/preintro_re2/re2-sequenzer.md` §3.1/3.2)

**Korrektur zum Sequenzer-Bericht:** `0x801c0474` (MODE 0) und `0x801c051c` (MODE 2)
sind **kein „VRAM-Upload-Kick"**, sondern die **CD-XA-Stream-Steuerung**
(`jal 0x8002c350` XA-fertig-Test @0x801c0494/0x801c053c, `jal 0x8002c1a0` @0x801c04dc,
`jal 0x8002bda8` @0x801c0504). Das Overlay ruft **nirgends** `LoadImage`/`DrawSync` —
die VRAM-Übertragung der Standbilder läuft komplett EXE-seitig
(vollständige jal-Liste: `scratchpad/jals.py`).

---

## 10. ⚠️ HARTER BEFUND + der eine offene Punkt: Typ 1 ist in DIESEM Build tot

**Fakt 1** — drei voneinander unabhängige Codestellen löschen den Zustand,
sobald `Record+1 & 0x10` gesetzt ist:
| Stelle | Kontext |
|---|---|
| `andi v0,v0,0x0010` @**0x801c1b04** → `sb zero,0(s7)` @0x801c1b10 | Renderer, Schleifenkopf |
| `andi v0,v0,0x0010` @**0x801c1fac** → `sb zero,0(v1)` @0x801c1fb8 | Setter 0x801c1f64 |
| `andi v0,v0,0x0010` @**0x801bfd18** → `sb zero,0(v1)` @0x801bfd24 | state4-Handler 0x801bfcfc |

**Fakt 2** — `TYP = (Record+1) >> 4` @0x801c1b30, d. h. **jedes** Typ-1-Record hat
`Record+1 ∈ [0x10,0x1F]` und damit Bit 4 gesetzt.

**Fakt 3** — in den echten Daten tragen alle Typ-1-Records `0x10`…`0x18`
(§5.4/§5.5), und **kein einziger Befehl im gesamten Overlay schreibt jemals
`Record+1`**. Voll-Scan aller `sb`/`sh` mit Offset 1–11 (`scan.py sb1`) findet nur:
`sh v0,4(s4)` @0x801c1c48 (Y-Scroll), `sb v1,8/9(s5)` @0x801c1d98/0x801c1db0/
0x801c1ddc/0x801c1df4 (Zoom). Adressbildung in den Record-Bereich gibt es nur an
den drei Init-Stellen aus §5.

⇒ **Statisch belegt: sämtliche Typ-1-Elemente (die Untertitelstreifen) werden in
diesem Build NIE gezeichnet** — und damit rendern die STATE-Maschinen 2 und 3
(Element-Sätze 2 und 3, 30 von 58 Records) überhaupt nichts.
Passend dazu: die Erzähltexte liegen als **vorgerenderte Bildseiten**
(OPEN03/04/05.TIM) vor und werden als Typ-0-Vierergruppen mit Y-Scroll gezeigt.

**OFFEN (nicht statisch entscheidbar):** ob auf echter Hardware doch Untertitel
erscheinen. Das würde einen Laufzeit-Beweis brauchen (Savestate/Emulator:
`*(0x801c2ed8)` und die Bytes `+1` der Records zur Laufzeit lesen).
Ich behaupte es nicht als Spiel-Verhalten — nur als das, was der Code sagt.

**Weitere offene Punkte:**
- Zuordnung `OPENxx.TIM` → TPAGE: die TIM-Header tragen alle VRAM (0,0), das Ziel
  wird beim Laden gesetzt; der Upload passiert EXE-seitig (§9). Genutzte Seiten
  laut Tabellen: **0x05–0x08 / 0x0a–0x0d** (Y=0) und **0x15, 0x16, 0x18, 0x19** (Y=256),
  CLUTs bei VRAM-Y **480–485**, X=0.
- **Bildrate:** der Haupt-Tick ruft `0x80031f94(1)` @0x801bfcc8. Alle Zeiten oben sind
  in **Frames** angegeben; die Umrechnung in Sekunden hängt davon ab, ob dieser Tick
  30 oder 60 Hz läuft — nicht von mir verifiziert, deshalb **keine Sekundenangabe**.
- Warum die Untertitel-Records paarweise identisch doppelt vorliegen (El. 0/1, 6/7,
  8/9, …) ist unklar; beide Kopien haben byteweise denselben Inhalt.

---

## 11. Übernahme-Rezept für unsere RE1.5-Standbild-Montage (nur Fakten, kein Einbau)

| Was | RE2-Original | Adresse |
|---|---|---|
| Blende-Mechanismus | additiver, texturmodulierter `POLY_FT4` (Code **0x2E**, TPAGE-ABR **1**) | @0x801c1c14, @0x801c1ec8 |
| Einblenden | Grauwert **0 → 96**, **+2/Frame** ⇒ **48 Frames** | @0x801c1b98, @0x801c1bbc |
| Ausblenden Standbild | **96 → 0**, **−2/Frame** ⇒ **48 Frames** | @0x801c1bec |
| Ausblenden Logo/Zoom | **96 → 0**, **−1/Frame** ⇒ **96 Frames** | @0x801c1d3c |
| Kreuzblende | Aus- und Einblendung werden **gleichzeitig** gesetzt (`hide_others = 0`) | @0x801c088c..0x801c08b4 |
| Text-Crawl | **Y −1 px alle 11 Frames** | @0x801c1c3c, Reload 10 @0x801c1a60 |
| Zoom auf | **W,H +2 px alle 4 Frames**, zentriert | @0x801c1d94, Reload 3 @0x801c1aa4 |
| Zoom zu | **W,H −2 px alle 4 Frames**, zentriert | @0x801c1dd8 |
| Zentrierung | `X=(320−W)/2`, `Y=(240−H)/2` | @0x801c1dfc / @0x801c1e14 |
| Tiefenordnung | OT **32** Bilder, **21** Logos, **16** Text | Record+2, §5.4 |
| Weiß-Vollbild | separater EXE-Slot, `0x801c1fe0(3,0,0xffffff,0)` | @0x801c066c |

Standbild-Standzeiten aus der Phasen-Timeline (Frames, `re2-sequenzer.md` §3.5):
Bild 1 sichtbar t=430…793 (**363 F**), Textseite ab t=823, Logo A Phase-3 t=0…300,
Bild 2 Phase-3 t=94…Phase-4 t=94, Logo B Phase-5 t=0…357.
Typische Textzeilen-Standzeit in state2: **273 F** (t=250 → 523, @0x801c0e08/0x801c0e30).
