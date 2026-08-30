# RE2-Retail Pre-Intro: die vier MODE-Handler + Bild-Upload-Pfad

Status: IN ARBEIT (inkrementell)
Datum: 2026-08-30
Teilauftrag: (a) mode3=0x801c0388, (b) mode0=0x801c0474, (c) mode1=0x801c0630 /
mode2=0x801c051c, (d) wie kommen OPEN*.TIM in den Speicher.

Quelle: `info/re2leon/COMMON/BIN/OPENING.BIN` (13524 B), Ladebasis 0x801bfa18,
`datei_offset = addr - 0x801bfa18`. EXE: `info/re2leon/PSX.EXE` (t_addr 0x80010000),
Decompilate `RE2_Quellcode_V2/FUN_*.c`.

Disasm verifiziert gegen den bekannten Skip-Check `@0x801bfb1c: sltiu v0,v0,0x191`. OK.

---

## 0. KORREKTUR ZWEIER ANNAHMEN AUS re2-sequenzer.md

1. **mode3 (0x801c0388) ist NICHT der "Noise"-Handler**, sondern der **Modul-Ausstieg/Teardown**
   (Disasm unten, §5). Der TV-Rausch steckt woanders (Suche laeuft, §6).
2. **0x8002c350 ist KEIN "XA-Stream-Fertig-Test"**, sondern der **FADE-Fertig-Test**:
   `FUN_8002c350.c`: `return (uint)(int)(short)(&DAT_800dfc1c)[a0*0x26] >> 0x1f;`
   — das liest genau den Akkumulator, den `FUN_8002c1a0` (der Fade-Setter) beschreibt.
   Rueckgabe != 0 = Vorzeichenbit gesetzt = **Slot inaktiv/fertig**.
3. Das Fade-Prim-Array steht bei **0x800dfc1c**, nicht 0x800efc1c (Tippfehler in §3.4 des
   Sequenzer-Berichts). Stride stimmt: 0x4c = 76 Byte, Rect-Konstante 0x00f00140.

---

## 1. Das globale FADE-SYSTEM der EXE (Traeger von mode0/mode2)

### 1.1 Datenstruktur — 4 Slots a 0x4c Byte ab `0x800dfc1c`

Belegt aus `FUN_8002c1a0.c`, `FUN_8002c2b0.c`, `FUN_8002c378.c` (`RE2_Quellcode_V2/`):

| Off | Adresse Slot0 | Typ | Bedeutung |
|---|---|---|---|
| +0x00 | 0x800dfc1c | u16 | **Akkumulator** (Fade-Fortschritt). Vorzeichenbit gesetzt = Slot AUS |
| +0x02 | 0x800dfc1e | s16 | **Schrittweite pro Frame** (wird auf den Akku addiert) |
| +0x04 | 0x800dfc20 | u8  | **ABR/Semi-Transparenz-Modus** (geht als `abr<<5` in SetDrawMode-tpage) |
| +0x05 | 0x800dfc21 | u8  | R-Maske (0 oder 0xff) |
| +0x06 | 0x800dfc22 | u8  | G-Maske |
| +0x07 | 0x800dfc23 | u8  | B-Maske |
| +0x08 | 0x800dfc24 | u8  | **OT-Index** des Fade-Prims |
| +0x0c | 0x800dfc28 | TILE| Prim Puffer 0 (tag +0x0c, code/rgb +0x10, xy +0x14, wh +0x18) |
| +0x13 | 0x800dfc2f | u8  | Prim-Code-Byte (Quelle in `FUN_8002c2b0`) |
| +0x1c | 0x800dfc38 | TILE| Prim Puffer 1 (Doppelpuffer, Index `DAT_800ce5e0`) |
| +0x2c | 0x800dfc48 | DR_MODE | Draw-Mode-Prim Puffer 0 |
| +0x38 | 0x800dfc54 | DR_MODE | Draw-Mode-Prim Puffer 1 |
| +0x44 | 0x800dfc60 | u32 | Rect x,y (0) |
| +0x48 | 0x800dfc64 | u32 | Rect w,h = **0x00f00140** = 320x240 |

Slot-Basisadressen: Slot0 0x800dfc1c, Slot1 0x800dfc68, Slot2 0x800dfcb4, Slot3 **0x800dfd00**
(bestaetigt: mode3 schreibt `sh -1, 0x800dfd00` und `sh 0, 0x800dfd02` = Akku/Schritt von Slot 3).

### 1.2 Setter `FUN_8002c1a0(a0, a1, a2, a3)` @0x8002c1a0

```
slot = a0 & 0xff
struct+0x02 (Schritt)  = a1
struct+0x04 (ABR)      = a0 >> 8
struct+0x08 (OT-Index) = a3
struct+0x05/06/07      = (a2&4)?0xff:0 / (a2&2)?0xff:0 / (a2&1)?0xff:0   ; R/G/B-Maske
SetDrawMode(struct+0x2c, dtd=1, dfe=0, tpage = ABR<<5, NULL)
SetDrawMode(struct+0x38, dtd=1, dfe=0, tpage = ABR<<5, NULL)
struct+0x44 = 0            ; x=0, y=0
struct+0x48 = 0x00f00140   ; 320x240 Vollbild
struct+0x00 (Akku) = (a1 & 0x8000) ? (a1 + 0x8000) : 0
```
Der letzte Zweig ist der Kern: **negativer Schritt ⇒ Akku startet bei `a1 + 0x8000`**,
**positiver Schritt ⇒ Akku startet bei 0**.

### 1.3 Per-Frame-Treiber `FUN_8002c378` @0x8002c378 — die eigentliche Fade-Mathematik

Schleife ueber **4 Slots** (Index 0..3), pro Slot:
```
if (Akku >= 0) {                             ; Vorzeichenbit von u16 frei
    level = (u8)( (s16)Akku >> 7 );          ; 0x0000..0x7fff  ->  0..255
    prim.R = Rmaske & level;
    prim.G = Gmaske & level;
    prim.B = Bmaske & level;
    Akku  += Schritt;                        ; <<< die Rate
    prim.xy = struct+0x44;  prim.wh = struct+0x48;
    AddPrim(OT + OT_Index*4, &prim);         ; OT = DAT_800ce2b0 fuer Slot 3,
    AddPrim(OT + OT_Index*4, &drawmode);     ;      DAT_800cc228 fuer Slot 0..2
}
```
Also: **Helligkeit = Akku >> 7**, und der Akku laeuft, bis er ueber 0x7fff kippt
(Vorzeichenbit) — dann ist der Slot von selbst aus. Kein separater Zaehler.

### 1.4 Ausschalter
- `FUN_8002c324(slot)` @0x8002c324: `Akku := 0xffff` (Vorzeichen gesetzt) = Slot sofort AUS.
- `FUN_8002c350(slot)` @0x8002c350: `!= 0` genau dann, wenn Slot AUS/fertig.

---

## 2. mode0 @0x801c0474 — **FADE IN** (kein VRAM-Upload!)

Disasm 0x801c0474..0x801c0518. Zustandsbit: `g_x2ee4 = byte@0x801c2ee4`, Bit **0x01**.
Slot-Nr: `g_slot = byte@0x801c24c0` (in Phase 0 auf **3** gesetzt, ab Phase 5 t=445 auf **0**).

```
if (g_x2ee4 & 1) {                            ; Fade laeuft bereits
    if (FUN_8002c350(g_slot) == 0) return;    ; noch nicht fertig -> MODE bleibt 0
    MODE(0x801c24ce) := 1;                    ; -> mode1 = Leerlauf
    g_x2ee4 ^= 1;
} else {                                      ; ERSTER Aufruf = Kick
    FUN_8002c1a0(a0 = g_slot | 0x200,    @0x801c04e0  ; ABR = 2
                 a1 = -256,              @0x801c04c8  ; Schritt
                 a2 = 7,                 @0x801c04cc  ; R+G+B
                 a3 = 1);                @0x801c04d8  ; OT-Index 1
    MODE := 0;                           @0x801c04f4
    g_x2ee4 |= 1;                        @0x801c0500
    FUN_8002bda8(0, 0);                  @0x801c0504  ; Clear-Modus 0
}
```

**Rechnung (byte-true):** Schritt = -256 = 0xff00, Vorzeichen gesetzt
⇒ Akku_start = 0xff00 + 0x8000 = **0x7f00**.
level = Akku>>7: Start **254**, pro Frame **-2** (0x100>>7).
Frame t: level = 254 - 2t. t=127 → level 0. t=128: Akku = 0x7f00-0x8000 = 0xff00 → Slot aus.
⇒ **128 Frames** Fade, level 254 → 0 in 2er-Schritten.

**Optik:** ABR = 2 ⇒ PSX-Semitransparenz **B - F** (Hintergrund minus Vordergrund), Farbe
grau=level ueber ein 320x240-TILE. level 254 = fast schwarz, level 0 = unsichtbar.
⇒ **Aufblenden aus Schwarz ueber 128 Frames.**

`FUN_8002bda8(0,0)` @0x8002bda8 setzt nur `DAT_800dfd54 := 0` = normaler Clear-Modus.

## 3. mode2 @0x801c051c — **FADE OUT** + Weiterschaltung

Zustandsbit **0x02** in `g_x2ee4`.
```
if (g_x2ee4 & 2) {                        ; Fade laeuft
    if (FUN_8002c350(g_slot) == 0) return;
    v1 = g_x2ee4; g_x2ee4 = v1 & ~2;
    if (v1 & 4) {                         ; @0x801c0564
        MODE := 3;                        ; -> Teardown
        Elementmaske 0x801c2ee8 := 0;
        FUN_801c1f64(0, 1);               ; alle Elemente aus
    } else {
        MODE := (v1 & 8) ? 1 : 0;         ; @0x801c0594..0x801c05b4
        g_x2ee4 &= ~8;
    }
    FUN_8002bda8(2, 0);                   ; @0x801c05d4 Clear-Modus 2, Farbe 0 (schwarz)
} else {                                  ; Kick
    FUN_8002c1a0(g_slot | 0x200, +256, 7, 1);   ; @0x801c05e4..0x801c05f8
    MODE := 2;  g_x2ee4 |= 2;
}
```
**Rechnung:** Schritt = +256, Vorzeichen frei ⇒ Akku_start = **0**.
level = 0, +2 pro Frame, bis Akku 0x8000 kippt: t=128 → Akku 0x8000 → aus.
⇒ **128 Frames**, level 0 → 254. Gleiches ABR 2 (B-F) ⇒ **Abblenden nach Schwarz.**

`FUN_8002bda8(2, 0)` @0x8002bda8: `DAT_800dfd54 := 2` und fuer 2 Eintraege
(Stride 0x98 ab 0x800dfae0): +0x8c = 0 (Farbe), +0x90 = 0 (x,y), +0x94 = 0x00f00140
⇒ Bildschirm wird ab jetzt **schwarz vollflaechig geclearet**.

**Bit-Semantik von `g_x2ee4` (0x801c2ee4), belegt:**
- 0x01 = "mode0-Fade-In laeuft"
- 0x02 = "mode2-Fade-Out laeuft"
- 0x04 = "nach dem Fade-Out ins Teardown (mode3)"
- 0x08 = "nach dem Fade-Out nach mode1 (Leerlauf) statt mode0"
  ⇒ die Phasen-Notiz `0x801c2ee4 := 8` in re2-sequenzer.md §3.5 heisst: *"nach dem
  jetzt gestarteten Ausblenden NICHT automatisch wieder aufblenden, sondern schwarz bleiben"*.

## 4. mode1 @0x801c0630 — LEERLAUF

```
801c0630: jr ra
801c0634: nop
```
Zwei Instruktionen. Reiner No-Op-Zustand: "kein Fade aktiv, Bild steht".

## 5. mode3 @0x801c0388 — **MODUL-TEARDOWN** (nicht "Noise")

```
801c0388  ; Warteschleife bis der Fade-Slot fertig ist:
  do { v0 = FUN_8002c350(g_slot); if (v0) break; FUN_80031f94(1); } while(1)  @0x801c0394..0x801c03b8
  if (DAT_800cfbd8 & 0x20)  FUN_80012c2c();          @0x801c03cc / 0x801c03d8
  stack[16] = 0xffff; DAT_800dfd64 = &stack[16];     @0x801c03e4 / 0x801c03f0
  FUN_8005a97c(); FUN_80059634();                    @0x801c03f4 / 0x801c03fc
  DAT_800cfbd8 &= ~0x10;                             @0x801c0414 / 0x801c041c  ; Letterbox aus
  FUN_8002bda8(2, 0);                                @0x801c0418
  FUN_8002c324(0);  FUN_8002c324(3);                 @0x801c0420 / 0x801c0428  ; Fade-Slots 0+3 AUS
  *(s16*)0x800dfd00 = -1;   *(s16*)0x800dfd02 = 0;   @0x801c0438 / 0x801c0444  ; Slot3 Akku/Schritt
  *(u32*)0x800d5b50 = 120;                           @0x801c044c               ; BGM/Track-Id 120
  FUN_8003210c(0);                                   @0x801c0450
  FUN_80031fe4();                                    @0x801c0458
  return
```
Dass 0x800dfd00/0x800dfd02 = Slot 3 ist, bestaetigt Stride 0x4c: 0x800dfc1c + 3*0x4c = 0x800dfd00.

**⇒ Der "Noise"/TV-Rausch liegt NICHT in mode3.** Suche laeuft (§6).

## 6. Nebenbefund: die Letterbox-Balken (im selben Fade-Treiber)

In `FUN_8002c378` @0x8002c378 (Ende), unabhaengig von den 4 Fade-Slots:
```
if ((DAT_800cfbd8 & 0x10) == 0) { if (DAT_800dfd55 > 0x0f) DAT_800dfd55 -= 0x10; }
else                            { if (DAT_800dfd55 < 0xf0) DAT_800dfd55 += 0x10; }
R=G=B=DAT_800dfd55 in 2 Prims (0x8009dbb0 / 0x8009dbd0, Stride 0x10 je Doppelpuffer)
if (!(DAT_800cfb74 & 0x4000) && DAT_800dfd55) AddPrim(OT DAT_800cc228+0x10, ...) x3
```
⇒ Flag `DAT_800cfbd8 & 0x10` = "Kino-Balken an", Helligkeit rampt **±0x10 pro Frame**
zwischen 0 und 0xf0 = **15 Frames** Ein-/Ausblendung. mode3 loescht das Bit (@0x801c0414).

---

## 7. (b) Der Bild-Upload-Pfad: 0x80198000 → VRAM — es gibt KEINEN Upload-Kick

Das Overlay ruft **nirgends** `LoadImage` fuer die Standbilder. Beweis: die einzigen zwei
libgpu-`jal` im ganzen OPENING.BIN sind `0x8008fa80` @0x801c1e58 und `0x8008f918` @0x801c1f0c
(beide im Renderer = Prim-Init + AddPrim). Statt dessen:

**`FUN_8002b968` @0x8002b968 ist der Per-Frame-Display-Dienst der EXE** (DrawSync/VSync/
PutDrawEnv/PutDispEnv/DrawOTag). Er enthaelt den **stehenden** Hintergrund-Upload:

```
if (DAT_800dfd54 == 0) {                 ; <- von FUN_8002bda8(0,..) gesetzt = mode0
    if (DAT_800dfc1b == 0) {
        p = &DAT_80198000;                              ; RAM-Staging-Puffer
        LoadImage(&DAT_8009dc14, p ...)   x3..4          ; << Bild -> VRAM, JEDEN Frame
    } else {
        FUN_8002bdf4();                                  ; MDEC-Pfad (nicht im OPENING benutzt)
    }
}
else if (DAT_800dfd54 == 2) {            ; <- von FUN_8002bda8(2,..) gesetzt = mode2-Ende
    SetTile(DAT_800dfc10[1].dr_env.code + 3);
    AddPrim(DAT_800ce2b0 + 0x3c, ...);   ; statt Bild: Vollflaechen-TILE in der Farbe von a1
}
DrawOTag(DAT_800ce2b0 + 0x3c);
DrawOTag(DAT_800ce22c + 0xffc);
DrawOTag(DAT_800cc228 + 0x1c);
DAT_800ce5e0 ^= 1;                        ; Doppelpuffer-Index
```

**⇒ Der "Bildwechsel" dauert 0 Frames.** Ein Standbild wechselt in dem Moment, in dem die
Phase `memcpy(0x80198000 <- Quelle, 0x25800)` ausfuehrt (`FUN_80076a00` @0x80076a00, 6 Aufrufe
im Overlay). Der naechste Display-Dienst schiebt den Puffer nach VRAM. `mode0` ist **kein**
Upload-Kick, sondern (i) das Freischalten dieses Upload-Pfads (`DAT_800dfd54 := 0`) und
(ii) der Start des 128-Frame-Aufblendens. Abgedunkelt wird also **nicht** waehrend des
Wechsels — der Wechsel passiert, waehrend der Bildschirm durch den Fade ohnehin schwarz ist.

Upload-Geometrie (aus `FUN_8002b968`): Quelle ist `u_long*`, pro Bildzeile **0xa0 Worte
= 640 Byte = 320 Pixel a 16 bpp**; Gesamthoehe 0xf0 = 240 ⇒ **0x25800 Byte = genau die
memcpy-Laenge**. Ziel-Y = `DAT_800dfc10->ofs[1]` (aktueller Draw-Env-Offset), also der
gerade nicht angezeigte Halbframe.

Die 6 memcpy-Stellen im Overlay (`jal 0x80076a00`):
@0x801c06e8, @0x801c09a0, @0x801c0efc, @0x801c10b4, @0x801c1578, @0x801c1758
— jeweils `lui 0x8019` (Ziel 0x80198000) + `lui 0x8011`/`lui 0x8013` (Quelle 0x8011a000
bzw. 0x8013f814).

## 8. (d) Wie die OPEN*.TIM in den Speicher kommen — die komplette Ladeliste

### 8.1 Der Lader: Funktion @0x801bfdc8 (Modul-Init, Frame 0)

CD-Ladefunktion = **`FUN_80012fb8(file_id, dest, mode)` @0x80012fb8** (`RE2_Quellcode_V2/
FUN_80012fb8.c`): schlaegt `file_id` in der **CD-Dateitabelle @0x800988a4** nach
(8 Byte/Eintrag: `+0x00 u32 Groesse`, `+0x04 u16 LBA-lo`, `+0x06 u8 LBA-hi`, `+0x07 u8 Flag`)
und liest die Datei per DsCommand nach `dest`. Alle 8 Aufrufe im Overlay liegen in dieser
einen Funktion (@0x801c0030, 0x801c0140, 0x801c0168, 0x801c0208, 0x801c0230, 0x801c0260,
0x801c0330, 0x801c035c) — es gibt **keinen weiteren Ladepunkt**.

TIM→VRAM-Uploader = **`FUN_80076a40(tim)` @0x80076a40**: `OpenTIM`/`ReadTIM` + `LoadImage`,
Ziel aus dem VRAM-Cursor `DAT_800cfbf0`:
`x = cursor*64; if (cursor >= 0x10) x -= 1024; y = (cursor >= 0x10) ? 256 : 0`,
danach `cursor += ceil(w/64)`; CLUT nach `y = 0x1e0 + DAT_800cfbf1`.

### 8.2 Datei-Id-Tabelle @0x801c24ac (im Overlay-Image, Datei-Offset 0x2A94)

Bytes: `ad 00 ae 00 af 00 b0 00 b1 00 b5 00 b6 00 b7 00 b8 00 b9 00`

Gegengeprueft mit der CD-Tabelle @0x800988a4 (EXE-Datei-Offset 0x890A4) — die Groessen
matchen die Dateien in `info/re2leon/COMMON/DATA/` **exakt**:

| Id | Groesse (CD-Tabelle) | LBA | Datei |
|---|---|---|---|
| 173 = 0xad | 102944 | 45690 | **OPEN00.TIM** |
| 174 = 0xae | 154656 | 45741 | **OPEN01.TIM** |
| 175 = 0xaf | 74016 | 45817 | **OPEN02.TIM** |
| 176 = 0xb0 | 65600 | 45854 | **OPEN03.TIM** |
| 177 = 0xb1 | 65600 | 45887 | **OPEN04.TIM** |
| (178 = 0xb2) | 32832 | 45920 | OPEN05.TIM — **nicht geladen** |
| (179 = 0xb3) | 8736 | 45937 | OPEN06.TIM — **nicht geladen** |
| (180 = 0xb4) | 33312 | 45942 | **OPEN07.TIM (TV-Rausch) — nicht geladen** |
| 181 = 0xb5 | 153600 | 45959 | **OPEN10.TIM** |
| 182 = 0xb6 | 153600 | 46034 | **OPEN11.TIM** |
| 183 = 0xb7 | 153600 | 46109 | **OPEN12.TIM** |
| 184 = 0xb8 | 153600 | 46184 | **OPEN13.TIM** |
| 185 = 0xb9 | 153600 | 46259 | **OPEN14.TIM** |
| (186/187/188) | 17454/17969/23563 | | OPNC/OPNL/OPNW.CPT — **nicht geladen** |

### 8.3 Reihenfolge und Ziele

**A) Immer: 4 Sprite-TIMs in einer Schleife** (@0x801c0008..0x801c0084)
```
DAT_800cfbf1 (CLUT-Cursor) := 0                     @0x801c0004
for (i = 0; i < 4; i++) {                           ; sltiu 0x4 @0x801c007c
    file_id = u16 @0x801c24ac[i]                    ; 173,174,175,176
    FUN_80012fb8(file_id, dest = 0x80165028, 1);    @0x801c0030  (Retry bis != -1)
    DAT_800cfbf0 (VRAM-Cursor) := u8 vram_pages[i]  @0x801c0054
    FUN_80076a40(0x80165028);                       @0x801c0058  ; TIM -> VRAM
    FUN_80031f94(1);                                @0x801c0060  ; ein Frame Service
}
```
`vram_pages[]` wird @0x801bfe00..0x801bfe18 unaligned (lwl/lwr+swl/swr) vom Overlay-Header
**@0x801bfa24 (Datei-Offset 0x0C)** auf den Stack kopiert; Bytes dort: **`05 0a 15 18`**,
5. Byte @0x801bfa28 = **`18`**.

| i | Datei | VRAM-Cursor | VRAM-Position (x,y) |
|---|---|---|---|
| 0 | OPEN00.TIM | 5 | (320, 0) |
| 1 | OPEN01.TIM | 10 | (640, 0) |
| 2 | OPEN02.TIM | 21 | (320, 256) |
| 3 | OPEN03.TIM | 24 | (512, 256) |

(Das zweite 5-Byte-Feld @0x801bfa1c (Datei-Offset 0x04) ist `00 00 00 00 00` und wird zwar
nach sp+56..60 kopiert, aber in der Funktion **nie gelesen** — toter Code.)

**B) Dann, abhaengig vom STATE (0x801c24d2), zwei Vollbilder a 0x25800 Byte:**

| STATE | Verzweigung | Bild 1 → **0x8011a000** | Bild 2 → **0x8013f814** | Extra |
|---|---|---|---|---|
| 0 / 1 | @0x801c00b4 | id@0x801c24b6 = 181 = **OPEN10** @0x801c0140 | id@0x801c24b8 = 182 = **OPEN11** @0x801c0168 | — |
| 2 | @0x801c0180 | id@0x801c24bc = 184 = **OPEN13** @0x801c0208 | id@0x801c24be = 185 = **OPEN14** @0x801c0230 | — |
| 3 | @0x801c0248 | id@0x801c24ba = 183 = **OPEN12** @0x801c0330 | id@0x801c24be = 185 = **OPEN14** @0x801c035c | id@0x801c24b4 = 177 = **OPEN04** → 0x80165028 → VRAM-Cursor `18` (@0x801c0294), davor `DAT_800cfbf1--` @0x801c0288 (CLUT-Zeile von OPEN03 wird ueberschrieben) |

Vor jedem Vollbild-Paar setzt der Lader zusaetzlich:
- `*(0x800ce32c) := 0x8011a000` (Lade-Puffer-Zeiger der EXE), danach wieder zurueck
  (@0x801c00cc / @0x801c0124)
- **Element-Anzahl** `sh @0x801c24cc` und **Element-Array** `sw @0x801c2ed8`:
  | STATE | Anzahl | Array |
  |---|---|---|
  | 0/1 | **28** @0x801c0110 | **0x801c21f4** @0x801c0118 |
  | 2 | **12** @0x801c01d4 | **0x801c2344** @0x801c01e0 |
  | 3 | **18** @0x801c02f4 | **0x801c23d4** @0x801c0304 |
  (Konsistent: 0x801c21f4 + 28*12 = 0x801c2344; + 12*12 = 0x801c23d4; + 18*12 = 0x801c24ac
  = direkt die Datei-Id-Tabelle. 58 Records a 12 Byte.)
- `DAT_800d75c4 = 100`, `DAT_800d75c6 = 100`, `DAT_800d75c1 = 0`, dann `FUN_8005a97c()`
  (@0x801c00e4..0x801c0100) — XA-Lautstaerke-Setup.
- `DAT_800dfd64 := &lokal` mit Inhalt **0xff2b** (STATE 0/1, @0x801c00d0) bzw.
  **0xff2c** (STATE 2 @0x801c0198 und STATE 3 @0x801c02bc) — die Kapitel-/Untertitel-Id.

**⇒ OPEN05, OPEN06 und OPEN07 (der TV-Rausch) sowie die drei .CPT-Dateien werden vom
OPENING-Modul NIE geladen.** Die Standbild-Sequenz hat also **keinen TV-Rausch-Effekt**.

## 9. (a-Ersatz) Statt TV-Rausch: zwei freilaufende Effekt-TAKTE im Renderer-Kopf

Da mode3 der Teardown ist und OPEN07 nie geladen wird, treibt der Kopf des Renderers
@0x801c1a0c..0x801c1abc die gesamte Animation — zwei freilaufende Zaehler:

```
; X-Achse:
v = u16 @0x801c2ee0;
if (v != 0) { @0x801c2ee0 = v-1;  @0x801c2edc =  0; }   @0x801c1a44 / 0x801c1a54
else        { @0x801c2ee0 = 10;   @0x801c2edc = -1; }   @0x801c1a60 / 0x801c1a74
; Y-Achse:
v = u16 @0x801c2ee2;
if (v != 0) { @0x801c2ee2 = v-1;  @0x801c2ede =  0; }   @0x801c1a88 / 0x801c1a98
else        { @0x801c2ee2 = 3;    @0x801c2ede = -1; }   @0x801c1aa4 / 0x801c1ab8
```
⇒ `0x801c2edc` ist **jeden 11. Frame** -1 (Reload 10), `0x801c2ede` **jeden 4. Frame** -1
(Reload 3), sonst jeweils 0.

`0x801c2ede` geht direkt in die Quad-Groesse (@0x801c1dc8..0x801c1df4):
```
quad_W += 2 * (s16)@0x801c2ede;      ; sll v0,v0,1  @0x801c1dd4
quad_H += 2 * (s16)@0x801c2ede;      ;              @0x801c1dec
```
⇒ alle 4 Frames schrumpft der Quad um 2 px in Breite und Hoehe = **1 px pro Seite**, weil
er anschliessend zentriert wird:
```
x0 = (320 - W) / 2   @0x801c1dfc..0x801c1e0c   (mit srl 31 / addu = Aufrundung Richtung 0)
y0 = (240 - H) / 2   @0x801c1e10..0x801c1e24
```
`0x801c2edc` (Periode 11) wird @0x801c1c3c gelesen — anderer Zweig, noch nicht zerlegt.

Beide Zaehler werden beim Modul-Init auf 0 gesetzt (`sh zero,0x801c2ee0` @0x801bfea4,
`sh zero,0x801c2ee2` @0x801bfeac) ⇒ im allerersten Frame feuern beide sofort.

### 9.1 KORREKTUR zu §9: der Jitter ist in Wahrheit ein ZOOM- und ein DRIFT-Takt

`0x801c2ede` (Periode 4) und `0x801c2edc` (Periode 11) sind **Takte**, keine Wackler.
Sie wirken nur auf Elemente, deren Zustandscode das passende Bit gesetzt hat (§10.2).

## 10. Das Element-/Effekt-System (der Kern der Praesentation)

### 10.1 Datenlayout

**Element-Records — 12 Byte**, Array-Zeiger `*(0x801c2ed8)`, Anzahl `lh @0x801c24cc`:

| Off | Typ | Bedeutung |
|---|---|---|
| +0x00 | u8 | **Zustands-/Effekt-Code** (Bitfeld, §10.2). 0 = aus |
| +0x01 | u8 | high nibble = **TYP** (0/1/2), **Bit 0x10 = hart deaktiviert** |
| +0x02 | u8 | **OT-Index** (Zeichenpriorität) — geht in `AddPrim(*(0x800ce22c) + b2*4, prim)` |
| +0x03 | u8 | **Index in das Definitions-Array** |
| +0x04 | u16 | X (nur TYP 0/1; TYP 2 wird zentriert) |
| +0x06 | u16 | Y (nur TYP 0/1) |
| +0x08 | u32 | **Farb-Akkumulator 0x00BBGGRR** — der Element-Fade |

`Bit 0x10 in +0x01` wird an **zwei** Stellen ausgewertet und erzwingt `+0x00 := 0`:
Renderer @0x801c1b04/0x801c1b10 und Setter @0x801c1fac/0x801c1fb8.

**Definitions-Records — 10 Byte**, Basis **0x801c208c** (Overlay-Datei-Offset 0x2674),
Index = Element `+0x03`; TYP 1 nutzt dieselbe Tabelle ab **0x801c2118** (@0x801c1c6c):

| Off | Typ | Bedeutung |
|---|---|---|
| +0x00 | u16 | tpage-Anteil (wird zur tpage-Basis addiert) |
| +0x02 | u16 | CLUT-Id |
| +0x04 | u8 | u0 |
| +0x05 | u8 | v0 |
| +0x06 | u8 | Textur-Breite |
| +0x07 | u8 | Textur-Hoehe |
| +0x08 | u8 | **Quad-Breite W** (wird vom Zoom mutiert!) |
| +0x09 | u8 | **Quad-Hoehe H** (dito) |

Gegenprobe: Def 0 = `05 00 | 00 78 | 00 00 | a0 a0 | a0 a0` ⇒ tpage 5 + Basis 160 = 0xa5
⇒ VRAM-Page x = 5*64 = **320**, y = 0, TP = 8 bit ⇒ genau dorthin hat der Lader
**OPEN00.TIM** gelegt (Cursor 5, §8.3). Def 4 = `15 00 c0 78 00 00 bd be c8 c8`
⇒ tpage 0x15+160 = 0xb5 ⇒ x = 320, y = 256 ⇒ **OPEN02.TIM** (Logos), Quad 200x200.
Die TYP-1-Defs `18 00 | 40 79 | 00 00 | ff 20 | ff 20` ⇒ tpage 0x18+32 = 0x38
⇒ x = 512, y = 256, TP = 4 bit ⇒ **OPEN03/OPEN04.TIM** (die vorgerenderten Textseiten),
Quad **255x32** = Textzeilen-Streifen.

### 10.2 Die Bits im Zustands-Code (Element +0x00) — die komplette Effekt-Sprache

| Bit | Wert | Wirkung | Beleg |
|---|---|---|---|
| 0 | 0x01 | **sichtbar** (sonst wird das Element uebersprungen) | @0x801c1b1c / 0x801c1b20 |
| 1 | 0x02 | Begleitbit des Einblendens (wird mit Bit2 zusammen geloescht) | `andi 0xf9` @0x801c1bcc |
| 2 | 0x04 | **EINBLENDEN** | @0x801c1b8c (TYP0) / @0x801c1cdc (TYP2) |
| 3 | 0x08 | **AUSBLENDEN** | @0x801c1bdc (TYP0) / @0x801c1d2c (TYP2) |
| 4 | 0x10 | **Y-DRIFT** (nur TYP 0) | @0x801c1c28 |
| 5 | 0x20 | **ZOOM HINEIN** (nur TYP 2) | @0x801c1d78 |
| 6 | 0x40 | **ZOOM HINAUS** (nur TYP 2) | @0x801c1dbc |

Damit sind die im Sequenzer-Bericht beobachteten Codes vollstaendig aufgeloest:

| Code | = | Bedeutung |
|---|---|---|
| **7** | 1+2+4 | einblenden |
| **11** | 1+2+8 | ausblenden |
| **23** | 7 + 0x10 | einblenden **+ Y-Drift** |
| **27** | 11 + 0x10 | ausblenden + Y-Drift |
| **39** | 7 + 0x20 | einblenden **+ Zoom hinein** |
| **43** | 11 + 0x20 | ausblenden + Zoom hinein |
| **71** | 7 + 0x40 | einblenden **+ Zoom hinaus** |
| **75** | 11 + 0x40 | ausblenden + Zoom hinaus |

### 10.3 Der Element-Fade (die eigentliche "Ueberblendung")

**TYP 0** (@0x801c1b8c..0x801c1c0c):
```
if (code & 0x04) {                                  ; einblenden
    c = *(u32*)(elem+8) + 0x00020202;               ; +2 je Kanal, lui 0x2/ori 0x202 @0x801c1b94/98
    *(u32*)(elem+8) = c;
    if (c > 0x0060605f) {                           ; lui 0x60/ori 0x605f @0x801c1b9c/0x801c1ba4
        *(u32*)(elem+8) = 0x00606060;               ; @0x801c1bb8/0x801c1bc0
        code &= 0xf9;                               ; Bits 0x04|0x02 loeschen @0x801c1bcc
    }
}
if (code & 0x08) {                                  ; ausblenden
    c = *(u32*)(elem+8) + 0xfffdfdfe;               ; = -0x00020202  @0x801c1be4/0x801c1bec
    *(u32*)(elem+8) = c;
    if (c <= 0) { *(u32*)(elem+8) = 0; code &= 0xf7; }   ; @0x801c1bfc/0x801c1c08
}
```
⇒ **Rampe 0 → 0x60 in Schritten von 2 = 48 Frames ein, 48 Frames aus.**
Maximum ist **0x60 = 96**, nicht 0x80 = 128 — die Elemente bleiben also bewusst unter
neutraler Modulationshelligkeit (96/128 = 75 %).

**TYP 2** (@0x801c1cdc..0x801c1d5c): einblenden identisch (**+2 → 48 Frames**),
**ausblenden aber `+0xfffefeff` = -0x00010101** (@0x801c1d34/0x801c1d3c)
⇒ **-1 je Kanal = 96 Frames** — die Logos verschwinden doppelt so langsam, wie sie kommen.

**TYP 1** (Text, @0x801c1c5c..0x801c1c70): **kein** Fade — die Farbe wird hart auf
`0x00808080` gesetzt (lui 0x80 / ori 0x8080), und `code & 0x08` (ausblenden) setzt den
Code sofort auf 0 (@0x801c1c94) = harter Schnitt.

### 10.4 Prim-Code und tpage-Basis (die Semitransparenz)

| TYP | rgb/code-Wort | tpage-Basis (`sp+24`) | Beleg |
|---|---|---|---|
| 0 | `farbe + 0x2e000000` | **160** (0xa0) | @0x801c1c10..0x801c1c30 |
| 1 | `0x00808080 + 0x2c000000` | **32** (0x20) | @0x801c1c98..0x801c1cb8 |
| 2 | `farbe + 0x2e000000` | **160** (0xa0) | @0x801c1d60..0x801c1d80 |

- Prim-Code **0x2E** = POLY_FT4 mit **ABE (Semitransparenz) EIN** (0x20|0x08|0x04|0x02).
- Prim-Code **0x2C** = POLY_FT4 **opak** (ABE aus) — der Text.
- tpage-Basis **160 = 0xa0** ⇒ Bits 5-6 = **ABR 1 = B + F (additiv)**, Bits 7-8 = TP 1 = 8 bpp.
- tpage-Basis **32 = 0x20** ⇒ TP 0 = 4 bpp (Textseiten).

⇒ **Die Bild-Elemente werden ADDITIV ueber das Standbild geblendet**, mit einer
Farbrampe 0→96. Das ist die "Ueberblendung" — nicht ein Alpha-Crossfade, sondern
ein additiver Aufheller.

### 10.5 Zoom und Drift (die Takte aus §9)

**Zoom, nur TYP 2** — mutiert die **Definitions**-Bytes W/H direkt (persistent!):
```
if (code & 0x20)  { def.W -= 2 * (s16)@0x801c2ede;  def.H -= 2 * (s16)@0x801c2ede; }
                    ; @0x801c1d8c..0x801c1d98 / 0x801c1da4..0x801c1db0   (subu)
if (code & 0x40)  { def.W += 2 * (s16)@0x801c2ede;  def.H += 2 * (s16)@0x801c2ede; }
                    ; @0x801c1dd0..0x801c1ddc / 0x801c1de8..0x801c1df4   (addu)
```
Da `@0x801c2ede` **jeden 4. Frame -1** ist (sonst 0):
- Bit 0x20 ⇒ W,H **+2 alle 4 Frames** = Quad waechst, zentriert ⇒ **+1 px pro Seite alle 4 Frames = 0,125 px/Frame/Seite**
- Bit 0x40 ⇒ W,H **-2 alle 4 Frames** = Quad schrumpft

**Y-Drift, nur TYP 0** (@0x801c1c34..0x801c1c48):
```
if (code & 0x10)  elem.Y += (s16)@0x801c2edc;      ; jeden 11. Frame -1
```
⇒ **1 px nach oben alle 11 Frames** — der langsame Aufwaerts-Crawl der Textseiten.

### 10.6 Zentrierung (nur TYP 2) und Prim-Pool

```
x0 = (320 - def.W) / 2      ; @0x801c1dfc..0x801c1e0c  (srl 31 + addu = Rundung gegen 0)
y0 = (240 - def.H) / 2      ; @0x801c1e10..0x801c1e24
```
TYP 0 und TYP 1 nehmen stattdessen `elem+0x04` / `elem+0x06` direkt (@0x801c1c4c/0x801c1ca4).

Prim-Pool: `0x801c24d8 + Doppelpuffer * 0x500` (@0x801c1ad4..0x801c1af0),
`0x500` = 1280 Byte = **32 POLY_FT4 je Puffer** (Elementanzahl max. 28 ⇒ passt).

## 11. Nebenbefund: die Elemente werden als POLY_FT4 gezeichnet

Renderer-Schleifenende @0x801c1e28..0x801c1f2c: pro Element ein **40-Byte-POLY_FT4**
(Prim-Basis = `s6 - 37`, Schrittweite `s6 += 40` @0x801c1f00), Feld-fuer-Feld belegt:

| Prim-Off | Store | Quelle |
|---|---|---|
| +0x04 rgb/code | `sw t0,-33(s6)` | sp+32 |
| +0x08 x0 / +0x0a y0 | `sh a2,-29` / `sh fp,-27` | (320-W)/2 , (240-H)/2 |
| +0x0c u0 / +0x0d v0 | `sb -25` / `sb -24` | Def+0x04 / Def+0x05 |
| +0x0e clut | `sh v0,-23` | `lhu 2(Def)` |
| +0x10 x1 / +0x12 y1 | `sh s2,-21` / `sh fp,-19` | x0+W , y0 |
| +0x14 u1 / +0x15 v1 | `sb s1,-17` / `sb -16` | u0+Def+0x06 , v0 |
| +0x16 tpage | `sh v0,-15` | `lhu 0(Def)` **+ halfword sp+24** |
| +0x18 x2 / +0x1a y2 | `sh a2,-13` / `sh s3,-11` | x0 , y0+H |
| +0x1c u2 / +0x1d v2 | `sb -9` / `sb s0,-8` | u0 , v0+Def+0x07 |
| +0x20 x3 / +0x22 y3 | `sh s2,-5` / `sh s3,-3` | x0+W , y0+H |
| +0x24 u3 / +0x25 v3 | `sb s1,-1` / `sb s0,0` | u0+uw , v0+vh |

`AddPrim(*(0x800ce22c) + element[+0x02]*4, prim)` @0x801c1ef4/0x801c1efc/0x801c1f0c
(`lbu a0,0(s4)` mit `s4 = elem+2`) ⇒ der OT-Index ist **Element +0x02**, NICHT der
Zustandscode. Beobachtete Werte in den Daten: 0x10 (Text), 0x15, 0x20.

## 12. Roh-Daten: das Element-Array fuer STATE 0/1 (28 Records, 0x801c21f4)

```
idx  +0 +1 +2 +3   X      Y     | TYP  Def  OT   Bemerkung
 0   06 10 10 00  0x0020 0x00b4 |  1    0   0x10  Text (b1&0x10 -> hart deaktiviert)
 1   06 10 10 00  0x0020 0x00b4 |  1    0   0x10  Duplikat
 2   00 00 20 00  0x0000 0x000a |  0    0   0x20  OPEN00 Kachel  (160x160)
 3   00 00 20 01  0x00a0 0x000a |  0    1   0x20  OPEN00 Kachel
 4   00 00 20 02  0x0000 0x00aa |  0    2   0x20  OPEN00 Kachel
 5   00 00 20 03  0x00a0 0x00aa |  0    3   0x20  OPEN00 Kachel
 6/7 06 11 10 01  0x0020 0x00b4 |  1    1   0x10  Text-Seite 1 (deaktiviert)
 8/9 06 12 10 02  0x0020 0x00b4 |  1    2   0x10  Text-Seite 2 (deaktiviert)
10   00 20 15 04  0x00a0 0x0078 |  2    4   0x15  OPEN02-Logo, ZOOM-faehig (200x200)
11   00 01 20 05  0x0000 0x0000 |  0    5   0x20  OPEN01 Kachel
12   00 01 20 06  0x0000 0x00a0 |  0    6   0x20
13   00 01 20 07  0x00a0 0x0000 |  0    7   0x20
14   00 01 20 08  0x00a0 0x00a0 |  0    8   0x20
15/16 06 13 10 03 0x0020 0x00b4 |  1    3   0x10  Text-Seite 3 (deaktiviert)
17/18 06 14 10 04 0x0020 0x00b4 |  1    4   0x10  Text-Seite 4 (deaktiviert)
19   00 20 15 09  0x00a0 0x0078 |  2    9   0x15  Logo 2, ZOOM-faehig
20   00 02 20 0a  0x0000 0x0000 |  0   10   0x20
21   00 02 20 0b  0x0000 0x00a0 |  0   11   0x20
22   00 02 20 0c  0x00a0 0x0000 |  0   12   0x20
23   00 02 20 0d  0x00a0 0x00a0 |  0   13   0x20
24/25 06 15 10 05 0x0020 0x00b4 |  1    5   0x10  Text-Seite 5 (deaktiviert)
26/27 06 16 10 06 0x0020 0x00b4 |  1    6   0x10  Text-Seite 6 (deaktiviert)
```
Muster: die **Vollbilder werden aus 4 Kacheln a 160x160** aufgebaut (Elemente 2-5,
11-14, 20-23), weil eine PSX-Texturseite nur 256 px breit ist. Die **7 Textseiten**
liegen als Paare vor (Elemente 0/1, 6/7, 8/9, 15/16, 17/18, 24/25, 26/27) und tragen
alle `+0x01 & 0x10` ⇒ nach dem Code (§10.1) werden sie **jeden Frame auf 0 gezwungen**,
also nie gezeichnet — die Untertitel kommen in diesem Build offenbar aus dem
EXE-Textsystem (`DAT_800dfd64` = &0xff2b / &0xff2c, gesetzt vom Lader @0x801c00d0 /
0x801c0198 / 0x801c02bc) und den .CPT-Dateien, die das Overlay selbst nicht laedt.
⚠️ Dieser letzte Schluss ist aus dem statischen Code + den statischen Daten gezogen;
eine Live-Gegenprobe (Savestate/Emulator) steht aus.

## 13. Zusammenfassung: die Praesentations-Bausteine, die uebernommen werden koennen

| Baustein | Konstante | Adresse |
|---|---|---|
| Vollbild-Wechsel | **0 Frames** (memcpy 0x25800 nach 0x80198000, EXE laedt jeden Frame hoch) | 0x801c06e8 u.a. / FUN_8002b968 |
| Global-Fade EIN | 128 Frames, Level 254→0, Schritt -2/Frame, ABR 2 (B-F), 320x240 | mode0 @0x801c04c8, Treiber @0x8002c378 |
| Global-Fade AUS | 128 Frames, Level 0→254, Schritt +2/Frame, ABR 2 (B-F) | mode2 @0x801c05e4 |
| Element-Fade EIN | 48 Frames, RGB 0→0x60, Schritt +2/Frame, additiv (ABR 1) | @0x801c1b94 / 0x801c1ce8 |
| Element-Fade AUS (Bild) | 48 Frames, Schritt -2/Frame | @0x801c1be4 |
| Element-Fade AUS (Logo/TYP2) | **96 Frames**, Schritt -1/Frame | @0x801c1d34 |
| Text-Element | kein Fade, feste Farbe 0x808080, opak (Code 0x2C) | @0x801c1c5c |
| Zoom | ±2 px (W und H) alle **4** Frames, Quad zentriert | @0x801c1aa4 / 0x801c1d8c |
| Y-Drift | -1 px alle **11** Frames | @0x801c1a60 / 0x801c1c34 |
| Kino-Balken | Rampe ±0x10/Frame, 0..0xf0 = 15 Frames | @0x8002c378 |

## 14. Offen
- [ ] Live-Gegenprobe, ob die 14 Text-Elemente wirklich stumm bleiben (Savestate)
- [ ] Wer laedt OPEN05/06/07 + OPNC/OPNL/OPNW (anderes Modul — nicht OPENING)
- [ ] Bedeutung des Element-Bits 0x02 (wird nur zusammen mit 0x04 geloescht)
- [ ] `FUN_8005a97c` / `FUN_80059634` / `FUN_8003210c` / `FUN_80031fe4` (Teardown-Aufrufe)
