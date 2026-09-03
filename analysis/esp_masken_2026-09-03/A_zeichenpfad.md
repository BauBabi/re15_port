# A — Der Original-Zeichenpfad der Vordergrund-Masken ("sprite.pri")

Stand 2026-09-03. Alle Behauptungen sind mit Ghidra-Adresse + Instruktion oder Datei-Byte-Offset
belegt. Quellen: `ghidra1_V2.txt` (RE1.5 PSX.EXE, t_addr=0x80010000, t_size=0xaf000 →
Text endet bei **0x800BF000**), `RE_15_Quellcode_V2/`, `ghidra_re2_Leon.txt` (RE2-Retail),
Originaldaten unter `re15_port/shared_assets/PSX/`.

---

## 0. Kurzfassung / Systemuebersicht

Das gesamte Vordergrund-Masken-System der RE1.5-EXE besteht aus **genau vier Funktionen** und
**drei Globals**. Das ist erschoepfend belegt (Abschnitt 5).

| Adresse | Rolle |
|---|---|
| `FUN_80039270` @0x80039270 | Puffer-Carve aus der Bump-Arena (einmal pro Raumladung) |
| `FUN_800392d4` @0x800392d4 | Parser + Prim-Vorbau, **einmal pro Kamera-Cut-Wechsel** |
| `FUN_80039590` @0x80039590 | Emitter, **jeden Gameplay-Frame** |
| `FUN_800396a8` @0x800396a8 | SCD-Opcode 0x45: Gruppe sichtbar/unsichtbar schalten |

| Global | Bedeutung |
|---|---|
| `DAT_800b2584` | Zeiger auf das Kontroll-Array (4 B je Maske) |
| `DAT_800bb4d4` / `DAT_800bb4d8` | Zeiger auf Prim-Puffer A / B (32 B je Maske, doppelgepuffert) |
| `DAT_800ac778` | RDT-Basiszeiger; Byte[0] = **Zeichenzahl**, Byte[7] = **Max-Masken/Raum** |

---

## 1. `FUN_80039270` — Puffer-Carve aus der Bump-Arena

Aufrufer: `FUN_800396fc` @**0x800399cc** (`jal FUN_80039270`) — die Raumlade-Routine.

```
80039270 lui   a1,0x800b
80039274 addiu a1,a1,-0x3884           ; a1 = &DAT_800ac77c   (Arena-Kopf)
80039278 lw    v1,0x0(a1)              ; v1 = Arena-Kopf
8003927c lui   a0,0x800b
80039280 lw    a0,-0x3888(a0)          ; a0 = DAT_800ac778     (RDT-Basis)
80039288 sw    v1,offset DAT_800b2584(at)      ; Flags-Array   = Arena-Kopf
8003928c lbu   v0,0x7(a0)              ; <<< RDT-Header Byte[7] = N
80039294 sll   v0,v0,0x2               ; N*4
80039298 addu  v0,v0,v1
800392a0 sw    v0,-0x4b2c(at)          ; DAT_800bb4d4 = Flags + N*4      (Prim-Puffer A)
800392a4 lbu   v1,0x7(a0)
800392ac sll   v1,v1,0x5               ; N*0x20
800392b0 addu  v1,v1,v0
800392b8 sw    v1,-0x4b28(at)          ; DAT_800bb4d8 = A + N*0x20       (Prim-Puffer B)
800392bc lbu   v0,0x7(a0)
800392c4 sll   v0,v0,0x5
800392c8 addu  v0,v0,v1
800392d0 sw    v0,0x0(a1)              ; Arena-Kopf = B + N*0x20
```

**Verbrauch: `N * 0x44` = 68 Byte pro Maskenslot** (4 Byte Kontrolle + 2 x 0x20 Byte Prim).
`N` = RDT-Header Byte[7]. Es gibt **keine feste Obergrenze im Code** — die Kapazitaet ist reine
Datenfrage.

**Datenpruefung (alle 206 echten RDTs, `STAGE{1..6}/ROOM*.RDT`):**
* `max(hdr[7]) = 105` (ROOM3000.RDT / ROOM3001.RDT, Cut 3).
* `hdr[7] >= max_ueber_Cuts(Summe der Gruppenzaehler)` in **allen** Faellen, 0 Anomalien.
* `deklarierte Zahl (hdr>>16) <= 255` in allen Faellen, 0 Anomalien.
* Cuts mit echtem Pri-Header in einem Raum mit `hdr[7]==0`: **0**.
  (Wichtig, weil die Loeschschleife in `FUN_800392d4` ein `do{}while` ist — bei `N==0` liefe sie
  256-mal. Der Fall kann durch die Daten nicht eintreten, weil dann jeder Cut bei 0xFFFFFFFF
  frueher aussteigt.)

Die Doppelpufferung wird in `FUN_80039590` @0x800395fc-0x8003960c indiziert:
`(&DAT_800bb4d4)[DAT_800aca34]` — `DAT_800aca34` ist der Frame-Flip (0/1).

---

## 2. `FUN_800392d4` — Parser der Section pro Cut

Aufrufer: `FUN_80021bbc` @**0x80021c28**, gegatet durch `DAT_800aca38 & 0x00100000` (Abschnitt 4).

### 2.1 Einstieg und NULL-Zweig

```
800392d4 lui   a0,0x800b
800392d8 lw    a0,-0x3888(a0)          ; a0 = DAT_800ac778 (RDT-Basis)
800392dc lui   v0,0x800b
800392e0 lh    v0,offset DAT_800b0fe4(v0)   ; v0 = aktueller Cut-Index
80039310 lw    v1,0x24(a0)             ; v1 = RDT+0x24 = Kamera-Cut-Tabelle
80039314 sll   v0,v0,0x5               ; *0x20  (Cut-Record = 32 Byte)
80039318 addu  v0,v0,v1
8003931c lw    t5,0x1c(v0)             ; <<< Cut-Record +0x1C = pri-ZEIGER (absolut)
80039324 lw    v1,0x0(t5)              ; Section-Header
80039328 li    v0,-0x1
8003932c bne   v1,v0,LAB_8003933c
80039334 j     LAB_8003955c            ;  == 0xFFFFFFFF -> raus
80039338 _sb   zero,0x0(a0)            ;  und *DAT_800ac778 (Zeichenzahl) = 0
```

**Wichtig:** `Cut+0x1C` ist zur Laufzeit ein **absoluter Zeiger**. Der Loader rebaset ihn
bedingungslos in `FUN_800396fc`:

```
80039860 lw   v0,0x24(v1)      ; Cut-Tabelle
80039864 lw   v1,0x0(a1)       ; RDT-Basis
80039868 addu a0,a0,v0         ; &cut[i]
8003986c lw   v0,0x1c(a0)      ; RDT-relativer pri_offset
80039874 addu v0,v0,v1         ; += RDT-Basis
80039878 sw   v0,0x1c(a0)      ; zurueckschreiben
...  Schleifenzaehler = *(u8*)(RDT+1) = Anzahl Cuts (@0x80039844/0x80039888)
```

Der Test auf "keine Masken" laeuft also **nicht** auf dem Offsetfeld, sondern auf dem
**Dword, auf das es zeigt**. Ein `pri_offset` von z. B. 0x0100 zeigt nach dem Fixup auf
`RDT+0x100`; erst der dortige Wert `FF FF FF FF` bedeutet "keine Masken".

### 2.2 Section-Header (4 Byte)

```
80039340 move  t8,v1
80039344 andi  t8,t8,0xffff        ; t8 = groupCount      (u16 @ +0)
80039330 srl   t2,v1,0x10          ; t2 = declaredCount   (u16 @ +2)
80039358 sb    t2,0x0(a0)          ; <<< NUR das LOW-BYTE nach *DAT_800ac778
80039348 addiu t5,t5,0x4           ; t5 = &group[0]
8003934c sll   v0,t8,0x3           ; groupCount*8
80039350 addu  a2,t5,v0            ; a2 = &mask[0]  (direkt hinter der Gruppentabelle)
```

`*DAT_800ac778` (= RDT-Header Byte[0], im RAM ueberschrieben!) ist die **Zeichenzahl**, die der
Emitter als Schleifengrenze nutzt. Sie ist `declaredCount & 0xFF`.

### 2.3 Loeschschleife (Flags zuruecksetzen)

```
8003936c lbu   a3,0x7(v0)        ; N = hdr[7]
80039370 LAB:  addiu a3,a3,-0x1
80039378       sb   zero,0x0(s5) ; ctrl[i].flags = 0
8003937c       andi v0,a3,0xff
80039380       bne  v0,zero,LAB_80039370
80039384       _addiu s5,s5,0x4
```

### 2.4 Gruppen-Record — 8 Byte

| Offset | Gelesen bei | Bedeutung |
|---|---|---|
| +0 | `lhu v0,0x0(t5)` @**0x800393b8** | u16 maskCount dieser Gruppe (innere Schleifengrenze, @0x8003952c) |
| +2 | **NIRGENDS** | siehe unten |
| +4 | `lhu v1,-0x2(t7)` @**0x800393fc** (t7 = group+6) | destX (Anker) |
| +6 | `lhu v1,0x0(t7)` @**0x80039410** | destY (Anker) |

**Antwort auf "was ist baseRaw bei +2?" — Das Feld wird von der EXE NIE gelesen.**
Beweis: `FUN_800392d4` liest aus dem Gruppen-Record ausschliesslich +0 (0x800393b8),
+4 (0x800393fc) und +6 (0x80039410); `t5` bzw. `t7` werden je Gruppe um 8 erhoeht
(0x80039558 / 0x80039540). Und ausser `FUN_800392d4` dereferenziert **keine** andere Instruktion
in der EXE das Feld `Cut+0x1C` (Abschnitt 5.1) — die Section ist also nur ueber diese eine
Funktion erreichbar.

Der Wert **ist** trotzdem eine CLUT-ID: gemessen ueber alle 2750 Gruppen-Records aller RDTs:

```
0x7800 : 2712     (= CLUT (0,480), exakt der im Emitter HARTKODIERTE Wert)
0x7c00 :   26
0x7c10 :    8
0x8080 :    2
0x8088 :    2
```

Der Emitter schreibt statt dessen die Konstante `0x7800`:
`80039498  ori v0,zero,0x7800` → `sh v0,0x0(s0)` @0x80039500 (= SPRT.clut, Abschnitt 2.6).
**TPage** kommt ebenfalls nicht von dort, sondern hartkodiert aus `SetDrawMode(..., 0x95, ...)`
@0x80039630. → `+2` ist ein **Autoring-Feld, das die Runtime ignoriert** (die 38 abweichenden
Records werden trotzdem mit CLUT 0x7800 gezeichnet).

### 2.5 Masken-Record — 8 Byte, +4 Byte wenn rechteckig

| Offset | Gelesen bei | Bedeutung |
|---|---|---|
| +0 | `lbu s7,0x0(a2)` @**0x80039408** | u8 → SPRT.u0 (srcX im Atlas) |
| +1 | `lbu s8,-0x9(s2)` @**0x80039418** (s2 = mask+0xa) | u8 → SPRT.v0 (srcY) |
| +2 | `lbu v0,-0x8(s2)` @**0x80039400** | u8, **addiert auf group.destX** |
| +3 | `lbu v0,-0x7(s2)` @**0x80039414** | u8, **addiert auf group.destY** |
| +4 | `lhu v0,-0x6(s2)` @**0x800393f0** | u16 **depth** → OT-Index |
| +6 | `lhu a0,-0x4(s2)` @**0x80039404** | u16 **size-Feld** |
| +8 | `lhu s4,-0x2(s2)` @**0x8003942c** | u16 width  — **nur im rechteckigen Fall** |
| +0xA | `lhu s3,0x0(s2)` @**0x80039430** | u16 height — **nur im rechteckigen Fall** |

**Rechteckig-Test und Quadrat-Dimension — byte-true:**

```
80039420 andi v0,a0,0xf000        ; a0 = size-Feld (+6)
80039424 bne  v0,zero,LAB_80039440
80039428 _srl v0,a0,0xc
                                  ; ---- (size & 0xf000) == 0  =>  RECHTECKIG ----
8003942c lhu   s4,-0x2(s2)        ; width  = u16 @ mask+8
80039430 lhu   s3,0x0(s2)         ; height = u16 @ mask+0xa
80039434 addiu s2,s2,0xc          ; Record = 12 Byte
80039438 j     LAB_80039450
8003943c _addiu a2,a2,0xc
LAB_80039440:                     ; ---- sonst QUADRAT ----
80039440 sll   s3,v0,0x3          ; dim = (size >> 12) << 3
80039444 move  s4,s3              ; width = height = dim
80039448 addiu s2,s2,0x8          ; Record = 8 Byte
8003944c addiu a2,a2,0x8
```

Also **ja** zu beiden Teilfragen: Test ist `(size & 0xf000) == 0`, Quadrat-Dimension ist
`(size >> 12) << 3`. Die unteren 12 Bit des size-Feldes werden **nie** benutzt.

**destX/destY-Verrechnung — byte-true:**

```
800393fc lhu  v1,-0x2(t7)     ; group.destX  (als u16 geladen!)
80039400 lbu  v0,-0x8(s2)     ; mask[+2]     (u8)
8003940c addu t0,v1,v0        ; SUMME
80039410 lhu  v1,0x0(t7)      ; group.destY
80039414 lbu  v0,-0x7(s2)     ; mask[+3]
8003941c addu s6,v1,v0
...
800394d0 sh   t0,-0x6(s0)     ; -> SPRT.x0  (16-Bit trunkierend)
800394cc sh   s6,-0x4(s0)     ; -> SPRT.y0
```

Es ist eine **volle Addition mit 16-Bit-Trunkierung**, kein "nur High-Byte". Die Maske traegt nur
8 Bit Position (0..255), der Gruppenanker traegt das Vorzeichen.

**Deklarierter Maskenzaehler als Schleifengrenze?** — **NEIN.**
* Aeussere Schleife: ueber `groupCount` (`t8`) @**0x80039550** (`sltu v0,v0,t8`).
* Innere Schleife: ueber `group[i].maskCount` @**0x80039534** (`lhu v1,0x0(t5)` @0x8003952c, `sltu v0,v0,v1`).
* `declaredCount` wird ausschliesslich @0x80039358 als **Zeichen**zahl abgelegt.

Die beiden Zahlen weichen in echten Daten ab (ROOM1210/1211 Cut 4: deklariert 75, Gruppensumme 77).
Der Port bildet das korrekt ab (`build_total` vs. `draw_count` in `pri_common.c`).

### 2.6 Vorgebauter Prim-Slot (32 Byte)

Je Maske werden **beide** Puffer (A @`t6`, B @`t4`) identisch befuellt. `s0 = t6+0x1a`, `s1 = t4+0x1a`.
Der Slot ist `DR_MODE` (12 B) + `SPRT` (20 B) = 0x20 B:

| Slot-Offset | Instruktion | Inhalt |
|---|---|---|
| +0x00..0x0B | `SetDrawMode` (erst im Emitter) | DR_MODE (tag + code[0] + code[1]) |
| +0x0C..0x0F | `SetSprt` (erst im Emitter) | SPRT.tag |
| +0x10,11,12 | `sb t3,-0xa/-0x9/-0x8(s0)` @**0x800394ac-b4** | r0=g0=b0 = **0x80** (`ori t3,zero,0x80` @0x800393b0) |
| +0x13 | `SetShadeTex(t6+0xC,1)` @**0x8003947c/0x8003948c** | code \|= 1 — **wird ueberschrieben, s.u.** |
| +0x14 | `sh t0,-0x6(s0)` @**0x800394d0** | x0 = group.destX + mask[+2] |
| +0x16 | `sh s6,-0x4(s0)` @**0x800394cc** | y0 = group.destY + mask[+3] |
| +0x18 | `sb s7,-0x2(s0)` @**0x800394dc** | u0 = mask[+0] |
| +0x19 | `sb s8,-0x1(s0)` @**0x800394e0** | v0 = mask[+1] |
| +0x1A | `sh v0,0x0(s0)` @**0x80039500**, v0 = `ori v0,zero,0x7800` @0x80039498 | **clut = 0x7800** |
| +0x1C | `sh s4,0x2(s0)` @**0x800394ec** | w |
| +0x1E | `sh s3,0x4(s0)` @**0x800394f0** | h |

### 2.7 Kontroll-Record (4 Byte) im Array `DAT_800b2584`

| Offset | Instruktion | Inhalt |
|---|---|---|
| +0 | `lbu/ori/sb v0,0x0(s5)` @**0x800393d8-0x800393e4** | flags, Bit0 = "sichtbar" |
| +1 | `sb v0,-0x1(t1)` @**0x800393ec** (v0 = a3+1) | **Gruppenindex + 1** |
| +2 | `sh v0,0x0(t1)` @**0x800393f8** (v0 = mask[+4]) | depth (OT-Index), verbatim |

Der Port hat **kein Gegenstueck zu +1** — deshalb ist SCD-Opcode 0x45 dort nicht abbildbar
(Abschnitt 6, offener Punkt).

---

## 3. `FUN_80039590` — Emitter (pro Frame)

```
80039590 lui   v0,0x800b
80039594 lw    v0,-0x3888(v0)          ; RDT-Basis
800395c0 lbu   s4,0x0(v0)              ; s4 = ZEICHENZAHL (= declared & 0xFF)
800395c8 lw    s3,offset DAT_800b2584  ; Kontroll-Array
800395cc beq   s4,zero,LAB_80039678
800395d4 lui   s7,0x800c
800395d8 addiu s7,s7,-0x4b2c           ; s7 = &DAT_800bb4d4  (Puffer-Zeigerpaar)
800395dc lui   s5,0x800b
800395e0 addiu s5,s5,-0x35cc           ; s5 = &DAT_800aca34  (Frame-Flip)
800395e4 addiu s6,s5,-0x235c           ; s6 = 0x800AA6D8     (OT-Basis)
LAB_800395e8:
800395e8 lbu   v0,0x0(s3)
800395f0 andi  v0,v0,0x1
800395f4 beq   v0,zero,LAB_80039668    ; Flag Bit0 aus -> ueberspringen
800395f8 _sll  s0,s2,0x5               ; i*0x20
800395fc lbu   v0,0x0(s5)              ; flip
80039604 sll   v0,v0,0x2
80039608 addu  v0,v0,s7
8003960c lw    v0,0x0(v0)              ; (&DAT_800bb4d4)[flip]
80039614 addu  s0,s0,v0                ; s0 = Slot
80039618 addiu s1,s0,0xc               ; s1 = SPRT
8003961c jal   SetSprt                 ; <<< code = 0x64, len = 4
80039620 _move a0,s1
80039624 move  a0,s0
80039628 ori   a1,zero,0x1             ; dfe = 1
8003962c ori   a2,zero,0x1             ; dtd = 1  (Dither AN)
80039630 ori   a3,zero,0x95            ; <<< tpage = 0x95
80039634 jal   SetDrawMode
80039638 _sw   zero,local_38(sp)       ; tw = NULL
8003963c move  a0,s0
80039640 jal   MargePrim               ; DR_MODE + SPRT zu EINEM Paket
80039644 _move a1,s1
80039648 move  a1,s0
8003964c lbu   v0,0x0(s5)              ; flip
80039650 lh    a0,0x2(s3)              ; <<< depth aus dem Kontroll-Record (SIGNED gelesen)
80039654 sll   v0,v0,0xc               ; flip * 0x1000  (= 1024 Worte je OT)
80039658 sll   a0,a0,0x2               ; <<< depth * 4 BYTE = depth * 1 OT-EINTRAG
8003965c addu  a0,a0,s6                ; + 0x800AA6D8
80039660 jal   AddPrim
80039664 _addu a0,v0,a0
LAB_80039668:
80039668 addiu s2,s2,0x1
8003966c sltu  v0,s2,s4
80039670 bne   v0,zero,LAB_800395e8
80039674 _addiu s3,s3,0x4
```

### 3.1 Primitivtyp

**SPRT** (GPU-Sprite, `code = 0x64`), unmittelbar davor ein **DR_MODE** (TPage-Kommando), beide
per `MargePrim` zu einem Paket verkettet.

`SetSprt` @0x8006b704:
```
8006b704 ori v0,zero,0x4
8006b708 sb  v0,0x3(a0)        ; len = 4
8006b70c ori v0,zero,0x64
8006b714 sb  v0,0x7(a0)        ; code = 0x64   <-- VOLLE Ueberschreibung
```
`SetShadeTex` @0x8006b61c: `lbu v0,0x7(p); ori v0,v0,0x1` — setzt nur Bit0.

**→ Die beiden `SetShadeTex(...,1)`-Aufrufe im Parser (0x8003947c / 0x8003948c) sind TOTER CODE.**
`SetSprt` laeuft jeden Frame danach und schreibt `code` komplett auf 0x64. Zum Zeitpunkt des
`DrawOTag` steht also **code = 0x64** = texturiertes Sprite **mit** Farbmodulation und **ohne**
Semi-Transparenz (ABE-Bit 0x02 nicht gesetzt). Die RGB 0x80/0x80/0x80 sind der neutrale
Modulationswert (0x80 = 1.0), das Ergebnis ist damit optisch identisch zu "raw texture".

`MargePrim`: `len(p0) = len(p0)+len(p1)+1 = 2+4+1 = 7`, und `*(u32*)p1 = 0`
(das SPRT-Tag wird zu einem GP0-NOP-Wort). Ein Paket = 7 Worte.

### 3.2 TPage / CLUT — und wo der Atlas herkommt

`tpage = 0x95` = `0b1_0010_101`:
* Bits0-3 = 5 → TX = 5*64 = **320**
* Bit4 = 1 → TY = **256**
* Bits5-6 = 0 → ABR 0
* Bits7-8 = 1 → **8-Bit CLUT**

`clut = 0x7800` → X = `(0x7800 & 0x3f) << 4` = **0**, Y = `0x7800 >> 6` = **480**.

Das passt exakt zum Upload-Pfad in `FUN_80021bbc`:

```
80021d5c lhu v0,0x0(v1)              ; v0 = Groesse des BSS-Records (Tabelle DAT_800b52c8[room][cut])
80021d68 addu at,v0,at               ; at = 0x80190000 + size
80021d6c lw  v0,-0x4(at)             ; <<< TRAILER: letztes Dword des Records
80021d74 beq v0,zero,LAB_80021df8    ;      == 0 -> kein Vordergrund-Atlas
80021da4 lw  s0,-0x8(at)             ; <<< vorletztes Dword = OFFSET des SLD-Blocks
80021db0 addu a0,s1,s0               ; a0 = 0x80190000 + off
80021db4 lw   a1,0x0(a0)             ; a1 = entpackte Groesse
80021db8 jal  FUN_800c47e8           ; SLD-Dekompressor  (src=a0+4, size=a1, dst=0x801a5800)
80021dbc _addiu a0,a0,0x4
80021de4 lhu  s1,0x0(s0)             ; alten VRAM-Cursor sichern
80021de8 ori  v0,zero,0x15
80021dec jal  FUN_8004ee78           ; TIM-Upload in Slot 0x15
80021df0 _sh  v0,0x0(s0)             ; DAT_800aca4c = 0x15 UND DAT_800aca4d = 0 (16-Bit-Store!)
80021df4 sh   s1,0x0(s0)             ; Cursor zurueck
```

`FUN_8004ee78` (Decompile):
```
sVar4 = DAT_800aca4c * 0x40; if (DAT_800aca4c >= 0x10) sVar4 -= 0x400;
prect->x = sVar4;                       ; 0x15*0x40 - 0x400 = 0x140 = 320
prect->y = (DAT_800aca4c >= 0x10) << 8; ; 256
LoadImage(prect, paddr);
crect->y = DAT_800aca4d + 0x1e0;        ; 0 + 480
LoadImage(crect, caddr);
```

**→ Atlas landet auf VRAM (320,256), CLUT auf (0,480) — genau tpage 0x95 / clut 0x7800.** ✔

**FUN_800c47e8 liegt ausserhalb der EXE** (0x800c47e8 > 0x800BF000). Es ist Laufzeit-Code, den
`FUN_8001311c` @0x8001311c per `FUN_80013b60(7, 0x800c0000, 0)` von der Disc nachlaedt. Deshalb
steht dort in `ghidra1_V2.txt` nur `?? ??`. Der Algorithmus liegt bereits als
`SldDecoder.java` / `analysis/esp_masken_2026-09-03/sld.py` vor.

### 3.3 OT-Index

```
800395e4 addiu s6,s5,-0x235c    ; s5=0x800ACA34 -> s6 = 0x800AA6D8   (OT-Basis)
80039650 lh    a0,0x2(s3)       ; depth
80039654 sll   v0,v0,0xc        ; flip * 0x1000 Byte = 1024 Worte
80039658 sll   a0,a0,0x2        ; depth * 4 Byte = depth * 1 OT-Eintrag
8003965c addu  a0,a0,s6
80039664 addu  a0,v0,a0
```

**OT-Wort-Index = depth (x1), OT-Basis 0x800AA6D8, 1024 Eintraege je Frame-Puffer.**
Bestaetigt die vorhandene Port-Doku in `re15_pri.h`.

---

## 4. Aufrufer und Gates

### 4.1 Parser

`FUN_80021bbc` (Cut-Wechsel: BSS laden, MDEC dekodieren, Kamera setzen):
```
80021bd4 beq  v1,v0,LAB_80021df8   ; DAT_800b5457 == 2 -> ganzer Kopf uebersprungen
80021c10 lui  v0,0x800b
80021c14 lw   v0,-0x35c8(v0)       ; DAT_800aca38
80021c18 lui  v1,0x10              ; 0x00100000
80021c1c and  v0,v0,v1
80021c20 bne  v0,zero,LAB_80021c34 ; Bit gesetzt -> Parser UEBERSPRINGEN
80021c28 jal  FUN_800392d4
```
Aufrufer von `FUN_80021bbc`: **nur** `FUN_8002137c` @0x80021558.

### 4.2 Emitter

`FUN_8001c6e8` (Gameplay-Frame-Step):
```
8001ce34 jal  FUN_8001db28          ; Item-Get-Modal
8001ce3c lui  v0,0x800b
8001ce40 lw   v0,-0x35c8(v0)        ; DAT_800aca38
8001ce44 lui  v1,0x10               ; 0x00100000
8001ce48 and  v0,v0,v1
8001ce4c bne  v0,zero,LAB_8001ce5c  ; Bit gesetzt -> Emitter UEBERSPRINGEN
8001ce54 jal  FUN_80039590
```

**Dasselbe Bit `DAT_800aca38 & 0x00100000` gatet beide.** Einziger Setzer in der ganzen EXE:
```
80015214 lui v1,0x10
80015228 lw  v0,0x0(a1)=>DAT_800aca38
80015230 or  v0,v0,v1
8001523c or  v0,v0,a0          ; a0 = lui 0x400 = 0x04000000
80015240 sw  v0,0x0(a1)
```
in `LAB_800151a0` (Eintrag der Handler-Tabelle @0x80071d14, ein Tod-/Cutscene-Zustand,
ausgeloest nach 27 Frames @0x800151cc `slti v0,v0,0x1b`).
Es gibt in der EXE **kein** `lui .,0xffef` und keinen Store, der genau dieses Bit wieder loescht
— es ist ein Einweg-Latch.

### 4.3 Was passiert bei `FF FF FF FF`

Zweig `80039334 j LAB_8003955c` mit Delay-Slot `80039338 sb zero,0x0(a0)`:
`*DAT_800ac778 = 0`. Der Emitter laedt diese Null @0x800395c0 und springt @0x800395cc sofort
ans Ende. **Kein Prim, kein Loeschen der Flags** — die Flags des vorherigen Cuts bleiben stehen,
werden aber nie gelesen, weil die Zeichenzahl 0 ist.

### 4.4 SCD-Opcode 0x45 — Gruppe ein/aus

Dispatch-Tabelle Basis 0x800744a8; Eintrag @**0x800745bc** → `LAB_800428d4`.
Index = (0x800745bc − 0x800744a8)/4 = **0x45**.

```
800428d4 addiu sp,sp,-0x18
800428e4 lw   v0,0x1c(s0)     ; SCD-PC
800428ec lbu  a0,0x1(v0)      ; op1 = Gruppenindex
800428f0 lbu  a1,0x2(v0)      ; op2 = Wert
800428f4 jal  FUN_800396a8
800428f8 _addiu a0,a0,0x1     ; a0 = Gruppenindex + 1
80042904 ori  v0,zero,0x1
80042908 sw   v1,0x1c(s0)     ; pc += 3
```

`FUN_800396a8` @0x800396d0-0x800396ec: laeuft ueber `*DAT_800ac778` Kontroll-Records und setzt
`ctrl[i].flags = param_2` fuer alle `ctrl[i][+1] == param_1`. Also **Sichtbarkeit pro
Maskengruppe**, geschaltet aus dem Raumskript.

---

## 5. Gibt es einen ZWEITEN Vordergrund-/Occlusion-Mechanismus?

### 5.1 Beweis: die Masken-Section ist geschlossen

**Alle** Instruktionen der EXE, die den Cut-Record dereferenzieren (RDT+0x24 als Basis):

| Adresse | Feld | Zweck |
|---|---|---|
| 0x8001531c / 0x80015338 / 0x80015354 | +0x4, +0xC, +0x8 | Todeskamera (Positionen) |
| 0x80021e68, 0x80046120 | +0x2 | `gte_ldH(v>>7)` = Projektionsdistanz |
| 0x80021e8c, 0x80046140 | +0x4..+0x18 | `FUN_80053ca4` = Kameramatrix |
| **0x8003986c** | **+0x1C** | **Loader-Fixup (FUN_800396fc)** |
| **0x8003931c** | **+0x1C** | **FUN_800392d4 — der einzige Konsument** |

Alle Xrefs auf `DAT_800b2584` (Kontroll-Array) laut Ghidra-Xref-Block (ghidra1_V2.txt Zeile 498802):
`FUN_80039270:80039288(W)`, `FUN_800392d4:80039368(R)`, `FUN_800392d4:8003938c(R)`,
`FUN_80039590:800395c8(R)`, `FUN_800396a8:800396c0(R)` — plus ein **Fehlalarm**
`VS_VH_OBJ_1A0:8005c200(W)`: dort ist `at = lui 0x800b + v0` (0x800c1fc: `addu at,at,v0`) und der
Store geht nach `0x800b2544 + v0` (SPU/VAB-Tabelle), nicht nach 0x800b2584.

Xrefs auf `DAT_800bb4d4`/`DAT_800bb4d8`: nur 0x800392a0/0x800392b8 (W), 0x80039394/0x800393a0 (R),
0x8003960c (R).

**→ Es gibt in der RE1.5-EXE keinen zweiten Konsumenten der RDT-Maskendaten und keinen zweiten
Nutzer der Masken-Puffer.**

### 5.2 Andere SPRT-/TILE-Emitter — keiner ist Occlusion

`jal SetSprt` in der EXE (3 Stellen):
* **0x8003961c** — der Masken-Emitter.
* 0x8002e338 — in `FUN_8002dfb0`, laedt `"C_back.tim"` (String @0x80010a04, Xref 0x8002e03c):
  Inventar-/Statusbildschirm-Hintergrund.
* 0x80053838 (+ `SetSprt8` 0x80053870, `SetSprt16` 0x800538a8) — generische Sprite-Bibliothek
  (`FUN_800537e4`), von ESP/Font benutzt.

`jal SetTile`: 0x80014dd0 (Fade-Quad, direkt gefolgt von `SetSemiTrans` @0x80014de4),
0x800211f0, 0x8002e0d8/0x8002e160/0x8002e1e8/0x8002e270 (Inventar-Rahmen, 4 Kacheln).

`jal SetDrawMode` mit `tpage = 0x95`: **nur** 0x80039634. Alle anderen 14 Stellen holen ihre TPage
per `GetTPage(...)` oder benutzen andere Konstanten (0x300/0xc0/0x22a/0x9a/0x140/0x340).

`DAT_800aca4c = 0x15` (der VRAM-Slot des Atlas) wird ausser @0x80021de8 nur noch
@**0x8002e008/0x8002e010** gesetzt — dort laedt `FUN_8002dfb0` den Inventar-Hintergrund in
**denselben** Slot (320,256). Das ist kein zweiter Occluder, sondern ein Ueberschreiber; beim
Verlassen des Inventars laeuft `FUN_80021bbc` wieder und stellt den Atlas her.

### 5.3 RE2-Retail-Gegenprobe

`ghidra_re2_Leon.txt`, `FUN_80049ca8` @0x80049ca8 (Aufrufer 0x80026770):

```
80049d64 lw   s0,0x0(v0)          ; (&DAT_800ea240)[flip]  -- Puffer-Zeigerpaar
80049d68 sll  v0,s3,0x5           ; i*0x20   <-- gleiche 32-Byte-Slots
80049d70 addiu s1,s0,0xc          ; SPRT bei +0xC
80049d74 jal  SetSprt
80049d80 li   a1,0x1              ; dfe = 1
80049d84 clear a2                 ; dtd = 0   <<< RE1.5 hat hier 1
80049d88 li   a3,0x95             ; tpage = 0x95   <<< IDENTISCH
80049d8c jal  SetDrawMode
80049d98 jal  MargePrim
80049da4 lhu  a0,0x0(s2)          ; depth
80049dac sll  a0,a0,0x2           ; *1 OT-Eintrag  <<< IDENTISCH
80049db0 jal  AddPrim
```

Unterschiede RE2 → RE1.5: RE2 laeuft **rueckwaerts** (`addiu s4,s4,-0x4` @0x80049d28), hat
`dtd = 0` statt 1 und ein zusaetzliches Gate `FUN_80077360(s6, ctrl[+3])` @0x80049d44.
Struktur, Slotgroesse, TPage 0x95 und OT-Skalierung x1 sind **identisch**.
In RE2 gibt es genau **einen** `jal SetSprt` — den Masken-Emitter.

### 5.4 Ergebnis

**Es gibt keinen zweiten Vordergrund-/Occlusion-Mechanismus.** Belegt fuer:
* einen zweiten Konsumenten der RDT-Masken-Section (5.1, erschoepfend),
* einen zweiten Nutzer der Masken-Puffer (5.1, erschoepfend),
* einen zweiten SPRT/TILE-Pfad mit dem Vordergrund-TPage 0x95 (5.2, erschoepfend ueber
  `SetDrawMode`-Aufrufstellen und ueber die Immediate-Suche `ori .,zero,0x95` — Treffer nur
  @0x80039630),
* die RE2-Architektur (5.3).

**NICHT BELEGT:** dass es in der EXE keine Primitiv-Emitter gibt, die ihre GPU-Pakete komplett
per direkter Stores bauen (ohne `SetSprt`/`SetTile`/`SetDrawMode`) und dabei eine TPage-Konstante
aus einer Variablen statt aus einem Immediate ziehen. Ein solcher Emitter existiert nachweislich
fuer Font-Glyphen (`FUN_80028868`), er zeichnet aber Schrift, keine Szenen-Geometrie.

---

## 6. Abgleich mit dem Port + gefundene Abweichungen

### 6.1 Was byte-true stimmt

| Punkt | Port | Original |
|---|---|---|
| Section-Header, NULL-Test `== 0xFFFFFFFF` | `pri_common.c` | @0x8003932c ✔ |
| `draw_count = declared & 0xFF` | `pri_common.c` | @0x80039358 (`sb`) ✔ |
| Bauzahl = Summe der Gruppenzaehler | `build_total` | @0x80039550 / 0x80039534 ✔ |
| Rechteck-Test `(size & 0xf000)==0` | `(mp[7] & 0xf0)==0` | @0x80039420 ✔ (aequivalent, LE) |
| Quadrat-Dim `(size>>12)<<3` | `(size_b>>4)*8` | @0x80039428/0x80039440 ✔ |
| Recordlaenge 12 / 8 | ✔ | @0x80039434 / 0x80039448 ✔ |
| destX/Y = group + mask-Byte, 16-Bit trunkierend | ✔ | @0x8003940c/0x80039410 ✔ |
| OT-Index = depth x1 | `RE15_PRI_MASK_OT_BUCKET` | @0x80039658 ✔ |
| Kapazitaet 105 = max hdr[7] | ✔ | Datenmessung ✔ |
| CLUT[0]=0x0000 → transparent | `bg_pc.c` idx==0 | GPU-Regel; in den Atlanten ist **nur** Index 0 gleich 0x0000 (gemessen) ✔ |

### 6.2 Abweichungen / Luecken

1. **`baseRaw` (group+2) ist im Port als "TPage" kommentiert** (`re15_pri.h`, `pri_common.c`).
   Falsch: TPage kommt aus der Konstante 0x95 @0x80039630, CLUT aus 0x7800 @0x80039498; das Feld
   wird nie gelesen. Kommentar korrigieren (Verhalten ist bereits richtig — der Port ignoriert es).

2. **Kontroll-Byte +1 (Gruppenindex+1) fehlt im Port.** Ohne dieses Feld ist SCD-Opcode **0x45**
   (`FUN_800396a8`, Gruppe sichtbar/unsichtbar) nicht implementierbar. Der Port muesste je Maske
   die Gruppen-ID mitfuehren und eine Flags-Tabelle je Cut halten.

3. **Der Port hat keinen SLD-Dekoder in C.** `bg_pc.c:63 re15_pri_load_cut_atlas()` und
   `pri_psx.c:44 re15_pri_psx_load_cut()` laden eine **vorextrahierte** `BSS/ROOM####/PRI##.TIM`.
   Existiert sie nicht → `return 0` → Overdraw komplett aus.
   Gemessen: **480 Cuts haben Maskenrecords, davon 292 mit vorhandener PRI##.TIM und 188 ohne**
   (Raumliste in Abschnitt 7.4).

4. **PSX-Pfad `render.c:463` `setDrawTPage(tp, 0, 1, tpage)`** = dfe **0**, dtd 1.
   Original: `SetDrawMode(p, 1, 1, 0x95, NULL)` @0x80039624-0x80039638 = dfe **1**, dtd 1.

5. **PSX-Pfad `pri_psx.c` `RE15_PRI_VRAM_ROWS 128`** laedt nur 128 Atlas-Zeilen hoch. Die Atlanten
   sind 226/240/256 Zeilen hoch (siehe 7.2). Masken mit `srcY >= 128` sampeln damit Muell.
   (Der Kommentar dort nennt ROOM1170 als Referenz — dort ist `srcY <= 88`.)

6. **PSX-Pfad klemmt den OT-Index** auf `[1, OT_LENGTH-2]` (`render.c:452-453`). Das Original
   klemmt nicht (`sll a0,a0,0x2; addu` @0x80039658/5c).

7. **Der Port emittiert DR_TPAGE als eigenes Prim nach dem SPRT** (`render.c:471-474`); das
   Original merged DR_MODE **vor** dem SPRT in **ein** Paket (`MargePrim` @0x80039640).
   Bei gleicher OT-Zelle kehrt `addPrim` die Reihenfolge um — funktional gleich, solange nur
   dieses eine TPage im Bucket liegt.

---

## 7. Neue Datenbefunde (belegt, korrigieren die bisherige Sweep-Auswertung)

### 7.1 Der SLD-Block steht im TRAILER des BSS-Records — kein Suchen noetig

Aus `FUN_80021bbc` (Abschnitt 3.2) folgt fuer einen BSS-Record der Laenge `size`
(size = `DAT_800be574` = `u16 tbl[room][cut]`, Tabellenzeiger `DAT_800b52c8`):

```
u32 flag = *(u32*)(rec + size - 4);    // 0 -> kein Vordergrund-Atlas
u32 off  = *(u32*)(rec + size - 8);    // Offset des SLD-Blocks im Record
u32 raw  = *(u32*)(rec + off);         // entpackte Groesse
SLD-Daten ab (rec + off + 4)           // -> ein TIM
```

**Verifikation an den Bytes** (`BSS/ROOM1020/BG00.BSS`, 65536 B):
```
0xa408: f8 ff f0 ff ff f0 ff ff  f0 ff ff f0 ff ff f0 e2
0xa418: b8 7b 00 00  01 00 00 00   <- off = 0x00007bb8, flag = 1  => size = 0xa420
0xa420..: 00 00 ...                <- reines Padding bis 0x10000
0x7bb8: 20 f2 00 00                <- raw = 0xF220 = 61984
0x7bbc: d0 10 00 00 ...            <- SLD-Strom
```
`BSS/ROOM1220/BG01.BSS`: `0x8824: ec 66 00 00 01 00 00 00`, `0x66ec: 20 02 01 00` = 0x10220 = 66080.

**Gegenprobe gegen den bestehenden Brute-Force-Scan** (`sld.py:find_sld`) ueber alle
1678 extrahierten `BSS/ROOM*/BG*.BSS`: **1678 von 1678 identisch** (gleicher Offset, gleiche
entpackte Groesse), 0 Abweichungen. Die Trailer-Regel ist damit die exakte, deterministische
Fundstelle — `find_sld` kann ersetzt werden.

Gefundene TIM-Groessen: `66080 (0x10220)` 418x, `61984 (0xF220)` 76x, `58400 (0xE420)` 2x
(Summe 496 — deckt sich mit den 366+130 SLD-Cuts der bisherigen Sweep-Auswertung).

Das ist dasselbe Trailer-Prinzip wie beim BGM-Container (`FUN_80044564`/`FUN_80044774`,
Memory `reai-v2-bgm-container`).

### 7.2 Der Atlas ist ein 8-Bit-TIM, 256 Texel breit

`BSS/ROOM1170/PRI01.TIM`, Byte 0..: `10 00 00 00 | 09 00 00 00` (TIM, 8bpp+CLUT).
CLUT-Block 524 B @(0,480) 256x1 — **exakt die Adresse, die `clut = 0x7800` kodiert**.
Bild-Block 65548 B @(0,0) 128x256 Halfwords = **256 x 256 Texel**.
`0xF220`-Variante: 256 x 240 Texel. `0xE420`-Variante: 256 x 226 Texel.
CLUT[0] = 0x0000 und es ist der **einzige** Nulleintrag → "Index 0 = transparent" ist hier
aequivalent zur GPU-Regel "Texel 0x0000 wird nicht gezeichnet".

### 7.3 KORREKTUR: STAGE5 hat sehr wohl SLD-Atlanten

Die bisherige Aussage *"114 Cuts: Masken, aber KEIN SLD-Block im BSS (nur STAGE5:
5030/5040/5060/50A0/50C0/5110/5120/5140)"* ist ein **Werkzeug-Artefakt**: unter
`shared_assets/PSX/BSS/` existieren nur `ROOM5000/` und `ROOM5001/` — die uebrigen STAGE5-Raeume
wurden nie in Cut-Dateien zerlegt. Der Sweep hatte dort schlicht nichts zu lesen.

Direkt auf den Original-Containern `shared_assets/PSX/STAGE5/ROOM50*.BSS` (64 KiB je Cut)
mit der Trailer-Regel:

```
ROOM503.BSS  cuts=12  Cut1..4 -> SLD (0x5a84/0x79d0/0x6b28/0x6b18), je 66080
ROOM504.BSS  cuts=7   Cut0,1,2,3,4,6 -> SLD, je 66080
ROOM506.BSS  cuts=13  Cut1..12 -> SLD, je 66080
ROOM50A.BSS  cuts=15  Cut1..8,10,11 -> SLD, je 66080
ROOM50C.BSS  cuts=7   Cut1..4 -> SLD, je 66080
ROOM511.BSS  cuts=7   Cut1..4 -> SLD, je 66080
ROOM512.BSS  cuts=7   Cut0..4,6 -> SLD, je 66080
ROOM514.BSS  cuts=14  Cut1..10,12 -> SLD, je 66080
```

Und die Entpackung liefert ein gueltiges TIM: `ROOM503.BSS` Cut 1, off 0x5a84, raw 66080 →
`10 00 00 00 09 00 00 00`, CLUT 524 B @(0,480) 256x1, Bild 65548 B 128x256 hw = 256x256 Texel.

**→ Von den 188 "Masken ohne Atlas"-Cuts sind 114 (die STAGE5-Faelle) reine Extraktionsluecken.**

### 7.4 Die verbleibende echte Luecke: 61984-Byte-Atlanten wurden nie extrahiert

Vorhanden sind 418 `PRI##.TIM` — exakt die Zahl der 66080-Byte-Bloecke. Die 76 Bloecke mit
61984 Byte (ROOM1020/1021/1030/1031/1040/1041/1070/1071 sowie ROOM1220/1221 Cut 0) und die
2 mit 58400 Byte (ROOM4040/4041 Cut 5) fehlen komplett.

Raeume mit Maskenrecords und ohne `PRI##.TIM` (Zahl der betroffenen Cuts):
```
ROOM1020 12  ROOM1021 12  ROOM1030 12  ROOM1031 12  ROOM1040 5  ROOM1041 5
ROOM1070  8  ROOM1071  8
ROOM5030  4  ROOM5031  4  ROOM5040 6  ROOM5041 6  ROOM5060 12  ROOM5061 12
ROOM50A0 10  ROOM50A1 10  ROOM50C0 4  ROOM50C1 4  ROOM5110 4   ROOM5111 4
ROOM5120  6  ROOM5121  6  ROOM5140 11 ROOM5141 11
```

---

## 8. Offene Punkte

1. **`DAT_800b52c8`** (Zeiger auf `u16 tbl[room][16]` mit der BSS-Recordlaenge) hat in
   `ghidra1_V2.txt` **keinen Schreiber** — nur zwei Leser (0x8001d9d8, 0x80021c40). Die Tabelle
   wird also indirekt (DMA/memcpy) befuellt. Fuer den Port irrelevant, solange die Trailer-Regel
   ueber "letztes Nicht-Null-Byte, auf 4 aufgerundet" rekonstruiert wird (an 1678/1678 Records
   verifiziert) — fuer einen 100 % byte-true CD-Pfad waere die Herkunft noch zu klaeren.
2. **`FUN_800c47e8`** (SLD-Dekompressor) liegt ausserhalb der EXE (nachgeladenes Modul, Datei-Index
   7, Ziel 0x800c0000, `FUN_8001311c` @0x8001311c). Der Algorithmus ist ueber
   `SldDecoder.java`/`sld.py` vorhanden, aber **nicht** aus einem Disassembly dieses Moduls belegt.
   Um ihn byte-true zu zitieren, muesste die Datei mit Index 7 lokalisiert und disassembliert werden.
3. **`FUN_80077360`** (RE2s zusaetzliches Per-Masken-Gate @0x80049d44) hat in RE1.5 kein
   Gegenstueck — NICHT weiter untersucht.
4. **Aufrufreihenfolge** `FUN_80021bbc` (Parser) vs. `FUN_80039590` (Emitter) innerhalb eines
   Frames ist nicht explizit vermessen; sie ist fuer das Ergebnis irrelevant, weil `SetSprt`
   ohnehin jeden Frame `code` neu setzt.
5. **Warum `dtd` in RE1.5 1 und in RE2 0 ist** (Dither auf einer 8-Bit-Textur) — nicht untersucht.

---

# Gegenpruefung

Unabhaengige Nachpruefung (2026-09-03). Jede zitierte Adresse wurde in `ghidra1_V2.txt` /
`ghidra_re2_Leon.txt` neu aufgeschlagen, jede Datenbehauptung mit eigenem Python gegen die
echten Bytes unter `re15_port/shared_assets/PSX/` nachgerechnet (Werkzeuge:
`analysis/esp_masken_2026-09-03/xcheck2/`).

**Ergebnis: 17 von 22 Behauptungen halten vollstaendig. Eine Kernbehauptung (Nr. 14) ist
sachlich WIDERLEGT, vier weitere tragen Fehler in Adresse, Zaehlung, Methode bzw. Arithmetik.**

## G.1 WIDERLEGT — Nr. 14: Das Gate-Bit ist KEIN Einweg-Latch

Behauptet: *"es existiert kein `lui .,0xffef` und kein Store, der genau dieses Bit wieder
loescht — es ist ein Einweg-Latch."*

Das ist falsch. Das Bit wird geloescht — nicht per AND-Maske, sondern per **wortweisem
Nullen ueber einen Zeiger**, weshalb die Suche des Berichts (AND-Masken, `lui 0xffef`) es
nicht sehen konnte.

```
FUN_80021eb4:
80021eb8 sw    s0,0x20(sp)
80021ebc lui   s0,0x800b
80021ec0 addiu s0,s0,0xfe8                    ; s0 = 0x800B0FE8
80021ec4 addiu a0=>DAT_800aca38,s0,-0x45b0    ; a0 = 0x800B0FE8-0x45B0 = 0x800ACA38
80021ee4 jal   FUN_8004ee60
80021ee8 _ori  a1,zero,0x7                    ; n = 7 Worte
```
```
FUN_8004ee60:                                 ; word-bzero
8004ee60 sw    zero,0x0(a0)
8004ee64 addiu a1,a1,-0x1
8004ee68 bgtz  a1,FUN_8004ee60
8004ee6c _addiu a0,a0,0x4
8004ee70 jr    ra
```

`FUN_8004ee60(&DAT_800aca38, 7)` nullt 0x800ACA38..0x800ACA53 **komplett**, Bit 0x00100000
eingeschlossen. Bestaetigt durch das Decompile `RE_15_Quellcode_V2/FUN_80021eb4.c`:

```c
FUN_8004ee60(&DAT_800aca38,7);
...
DAT_800aca38 = DAT_800aca38 | 0xc00000;
```

Drei Aufrufstellen (Ghidra `FUN_80021eb4 XREF[3]`): `@0x8001d200` (nach einem
`and 0x40000000`-Test), `main:@0x80020d14`, `@0x80021100` (direkt nach `ResetGraph`).
Das ist die Grafik-/Zustands-Reinitialisierung.

**Konsequenz fuer den Port:** der Masken-Zeichenpfad ist nach dem 27-Frame-Setzer
(`@0x80015230`) NICHT dauerhaft tot, sondern wird bei der naechsten Reinitialisierung wieder
scharf. Ein Port, der das Gate als Einweg-Latch modelliert, verliert die Vordergrund-Masken
ab dem ersten Tod-/Cutscene-Zustand fuer den Rest der Sitzung.

Gegenprobe zur Vollstaendigkeit: `grep "lui *[a-z0-9]*,0xffef" ghidra1_V2.txt` → **0 Treffer**
(insoweit stimmt der Bericht), und alle 17 direkten `sw`-Stores auf `DAT_800aca38` wurden
einzeln aufgeschlagen; die verwendeten AND-Masken sind `0xfbffffff`, `0xbfffffff`,
`0xf7ffffff`, `0xfffbffff`, `0xffffbfff`, `0xffff7fff`, `0xfffeffff`, `0xdfffffff`,
`0x7fffffff` — keine beruehrt Bit 20. Der Loescher ist ausschliesslich der indirekte bzero.

## G.2 WIDERLEGT — Nr. 14, zitierte Setzer-Adresse

Behauptet: *"Einziger Setzer der EXE ist @0x80039230"*. An `0x80039230` steht:

```
8003922c sra   v0,v0,0x10
80039230 mult  v0,v0
80039234 mflo  a0
80039238 jal   SquareRoot0
```

— eine Distanzberechnung, kein Store und kein Bit-Setzer. Der tatsaechliche Setzer ist
`@0x80015230 or v0,v0,v1` / `@0x80015240 sw v0,0x0(a1)`. Der Fliesstext des Berichts
(Zeile 426-434) nennt ihn korrekt; nur die Zusammenfassung traegt die falsche Adresse.

## G.3 WIDERLEGT — Nr. 18, Dateizaehlung

Behauptet: *"1678 extrahierte BG*.BSS"*. Gemessen:

```
find re15_port/shared_assets/PSX/BSS -name "BG*.BSS" | wc -l   -> 1688
```
1688, keine Datei kleiner als 1 KiB, keine weiteren `*.BSS` im Baum. 1678 ist um 10 zu klein.

## G.4 WIDERLEGT — Nr. 18, Methode: die Trailer-Regel allein reicht NICHT

Der Bericht (Zeile 697) formuliert die Rekonstruktion als *"letztes Nicht-Null-Byte, auf 4
aufgerundet"*. Genau so implementiert (`xcheck2/tr.py`) liefert sie:

```
files 1688 | trailer hits 818 | Abweichungen gegen Brute-Force: 322
z.B. BSS/ROOM1020/BG11.BSS -> off=22244 raw=22244 (Offset zeigt auf sich selbst), find_sld: []
      Groessen darunter: 939539392, 1069059, 13676 ... = Muell
```

Der Grund ist strukturell: der Disasm holt das Record-Ende NICHT aus den Daten, sondern aus
einer Laufzeit-Tabelle — `@0x80021d40 lh v1,DAT_800b0fe4` / `@0x80021d48 lh v0,0x0(s0)` /
`@0x80021d50 sll v0,v0,0x5` / `@0x80021d5c lhu v0,0x0(v1)` → u16-Endoffset pro (Cut, Kamera),
erst dann `-0x4` (Flag) und `-0x8` (SLD-Offset). Ohne diese Tabelle ist "letztes Nicht-Null-Byte"
nur eine Heuristik.

**Mit** einer Gueltigkeitsschranke (Offset < end-8, `0x2000 <= raw <= 0x40000`, erste vier
entpackte Bytes `10 00 00 00` = TIM-Magic) reproduziert sie den Brute-Force dann exakt
(`xcheck2/tr2.py`):

```
brute-force files with SLD: 496 [(66080, 418), (61984, 76), (58400, 2)]
trailer  files with SLD:    496 [(66080, 418), (61984, 76), (58400, 2)]
agree 496 | only_tr 0 | only_bf 0 | diff 0
```

Die Zahlen des Berichts (496 / 418 / 76 / 2) sind damit bestaetigt — die **Regel als solche**
aber nur zusammen mit der Validierung. Fuer den Port ist das der Unterschied zwischen
"funktioniert" und "laedt in 322 von 1688 Faellen Muell".

## G.5 WIDERLEGT — Nr. 20(e), Arithmetik

Behauptet: *"188 ... 114 davon STAGE5 ... 74+2 die nie extrahierten Atlanten"*.
114 + 74 + 2 = **190**, nicht 188. Gemessen (alle 206 RDTs gegen `BSS/ROOM####/PRI##.TIM`):

```
mask cuts 480 | have PRI.TIM 292 | missing 188
missing by room: 1020:12 1021:12 1030:12 1031:12 1040:5 1041:5 1070:8 1071:8   = 74
                 5030:4 5031:4 5040:6 5041:6 5060:12 5061:12 50A0:10 50A1:10
                 50C0:4 50C1:4 5110:4 5111:4 5120:6 5121:6 5140:11 5141:11     = 114
```
188 = 114 + 74. Die zwei 58400-Byte-Bloecke (ROOM4040/4041 Cut 5) sind SLD-Bloecke **ohne**
Maskenrecords und gehoeren gar nicht in diese Menge. Die Kernzahl 188 stimmt exakt; die
Aufschluesselung ist falsch summiert.

## G.6 Bestaetigt (kurz)

Alle uebrigen Behauptungen wurden Instruktion fuer Instruktion bzw. Byte fuer Byte
nachvollzogen und halten:

- **1, 2** — `FUN_80039270` Arena-Schnitt N*0x44 (`@0x8003928c/94/a0/ac/b8/c4/d0`), genau ein
  Aufrufer (`FUN_800396fc:@0x800399cc`); Loader-Rebase `@0x8003986c/74/78` ueber
  `lbu v0,0x1(v1)` Cuts, Cut-Tabelle `lw v0,0x24(v1)`, Recordbreite `sll a2,0x5`. Kein
  Obergrenzen-Vergleich im Code.
- **3, 4, 5, 6, 7** — Section-/Gruppen-/Masken-Layout und Slot-Belegung exakt wie beschrieben
  (Disasm 0x800392d4-0x80039558 vollstaendig gelesen). SPRT-Slot 32 B, DR_MODE 12 B
  (`SetDrawMode` schreibt `len=2` `@0x80069868`), beide Frame-Puffer identisch gefuellt.
- **8, 9, 10, 11** — Kontroll-Record 4 B; `SetSprt`-Body `@0x8006b704 len=4`,
  `@0x8006b70c/14 code=0x64`; `SetShadeTex`-Body `@0x8006b61c ori v0,v0,0x1` → tot;
  `MargePrim` `@0x8006b958-64: len = 2+4+1 = 7`; OT-Basis `0x800B0000-0x35CC-0x235C =
  0x800AA6D8`, Frame-Schritt `sll v0,0xc` = 1024 Worte, Index `sll a0,0x2` = depth x1.
- **12** — TPage 0x95 → (320,256)/8-bit/ABR0, CLUT 0x7800 → (0,480); `@0x80021de8/f0`
  `sh 0x15` und `FUN_8004ee78` erzwingen `prect->x=320` (`@0x8004eea8/b0/b8`),
  `prect->y=256` (`@0x8004eec8-d4`), `crect->y=0x1e0` (`@0x8004ef30-3c`). Datenprobe
  `BSS/ROOM1170/PRI01.TIM`: len 66080, CLUT-Rect (0,480,256,1), Bild-Rect (0,0,128,256).
- **13** — `@0x80039328-38` Zweig + Delay-Slot; Loeschschleife erst `@0x80039370-84`.
- **15** — Tabelle `0x800744a8`, Eintrag `0x800745bc` → `LAB_800428d4`, Index 0x45;
  `FUN_800396a8` setzt `ctrl[i].flags` fuer `ctrl[i][+1] == a0`.
- **16** — `DAT_800b2584`: 5 echte Instruktionen in 4 Funktionen; `@0x8005c200` ist tatsaechlich
  ein Ghidra-Fehlalarm (`@0x8005c1f8 lui at,0x800b` + `@0x8005c1fc addu at,at,v0`).
  `DAT_800bb4d4/d8`: genau die 5 genannten Instruktionen in 3 Funktionen. Alle 15
  `jal SetDrawMode`-Stellen einzeln aufgeschlagen — nur `@0x80039630` traegt 0x95. Alle
  RDT+0x24-Leser geprueft: keiner ausser `@0x8003931c`/`@0x8003986c` dereferenziert +0x1C
  (auch `FUN_80053ca4` nicht — dessen `0x1c(..)`-Zugriffe gehen auf `DAT_80072d68`/`DAT_800b52a4`).
- **17** — RE2 `@0x80049d68-db0` exakt wie zitiert (32-B-Slot, SPRT +0xC, TPage 0x95,
  depth x1, Rueckwaertslauf `@0x80049d28`, Extra-Gate `@0x80049d44`, dtd=0
  `@0x80049d84 clear a2`).
- **18 (Daten)** — `BSS/ROOM1020/BG00.BSS` @0xa418 = `b8 7b 00 00`, @0xa41c = `01 00 00 00`,
  @0x7bb8 = `20 f2 00 00`; `BSS/ROOM1220/BG01.BSS` @0x8824 = `ec 66 00 00 01 00 00 00`,
  @0x66ec = `20 02 01 00`. Byte fuer Byte bestaetigt.
- **19** — Trailer-/SLD-Scan auf `STAGE5/*.BSS` reproduziert **jeden** genannten Offset:
  ROOM503 Cut1-4 = 0x5a84/0x79d0/0x6b28/0x6b18; ROOM504 Cut0,1,2,3,4,6 =
  0x76b0/0x781c/0x5100/0x5458/0x7b20/0x5540; ROOM506/50A/50C/511/512/514 ebenso, alle 66080.
  Unter `BSS/` existieren tatsaechlich nur `ROOM5000/` und `ROOM5001/`.
- **20 (a-d)** — `re15_pri.h:13` und `pri_common.c:13/62` sagen "baseRaw ... TPage" (falsch);
  `render.c:463 setDrawTPage(tp,0,1,...)` = dfe 0 (SDK-Signatur `(p,dfe,dtd,tpage)`, bestaetigt
  in `PSn00bSDK/.../psxgpu.h:34` und `Psy-Q_47/INCLUDE/LIBGPU.H:279`) gegen `a1=1` im Original;
  `pri_psx.c:34/58` klemmt auf 128 Zeilen (Atlanten 256/240/226 Zeilen, nachgerechnet aus
  66080/61984/58400); `render.c:452-453` klemmt den OT-Index, das Original nicht.
- **21** — `t_addr=0x80010000`, `t_size=0x000af000` → Textende **0x800BF000**, also liegt
  0x800C47E8 ausserhalb; `ghidra1_V2.txt` Z.574973 zeigt dort `?? ??`; Loader
  `@0x80013124 ori a0,zero,0x7` / `@0x80013128 lui a1,0x800c` / `jal FUN_80013b60`. Die
  Einstufung "teilbelegt" ist korrekt.
- **22** — korrekt als NICHT BELEGT gekennzeichnet; die Einschraenkung (Emitter mit variabler
  TPage nicht erschoepfend durchsucht) ist berechtigt: `@0x80021848/@0x80021864`
  (`lbu a3,0x4(s0)` + `sll 5`) und `@0x800295f8/@0x80029624/@0x8002e074`
  (`andi a3,v0,0xffff`) ziehen ihre TPage tatsaechlich aus Variablen.

## G.7 Zusatzbefunde aus der Gegenpruefung (nicht im Bericht)

1. **Die unteren 12 Bit des Masken-size-Feldes sind die AUTORISIERTE TPage.** Ueber alle
   16654 Maskenrecords: `0x095` 8210x, `0x0B5` 3454x, `0x0D5` 3284x, `0x0F5` 1682x,
   `0x000` 20x, `0x800` 4x. Das sind TPage 0x95 mit vier verschiedenen ABR-Stufen (Bits 5-6).
   Die Runtime ignoriert sie und haert 0x95/ABR0 ein — exakt das gleiche Muster wie beim
   Gruppenfeld +2 (`0x7800` = CLUT). Nr. 5 ("werden nie benutzt") ist als Laufzeit-Aussage
   richtig; als Format-Aussage waere sie falsch.
2. **Gruppenfeld +2 unabhaengig nachgezaehlt** (206 RDTs, 2188 Cuts, 480 mit Masken,
   2750 Gruppen): `0x7800` 2712x, `0x7c00` 26x, `0x7c10` 8x, `0x8080` 2x, `0x8088` 2x —
   identisch zu Nr. 4.
3. **ROOM1210/ROOM1211 Cut 4: declaredCount 75, Gruppensumme 77.** In 478 von 480
   Masken-Cuts sind beide gleich; hier nicht. Da der Parser 77 Slots fuellt, der Emitter aber
   `*DAT_800ac778` = 75 als Schleifengrenze nimmt (`@0x800395c0/@0x8003966c`), werden im
   Original zwei gebaute Masken **nie gezeichnet**. `pri_common.c` baut korrekt 77 — der
   ZEICHEN-Pfad muss dann aber auf 75 begrenzt werden, sonst zeichnet der Port zwei Masken
   zu viel. Byte-true Divergenz, die Nr. 3 impliziert, aber nicht ausspricht.
4. **Alle Tiefenwerte liegen in 0..1023** (16654 Records, kein einziger >= 0x8000). Der
   1024-Wort-OT wird exakt ausgeschoepft, und das `lh` (signed) im Emitter kann bei echten
   Daten nie negativ werden — die Klemmung des PSX-Ports (Nr. 20d) ist bei realen Daten
   folgenlos, bleibt aber eine Abweichung.
5. **Zwei Port-Abweichungen fehlen in Nr. 20:** `pri_common.c` bricht bei
   `group_count == 0 || mask_count_decl == 0` und bei `> 256` ab. Das Original kennt
   ausschliesslich den Test `u32 == 0xFFFFFFFF` (`@0x80039328-2c`); bei `groupCount == 0`
   laeuft es regulaer durch (`@0x800393a8 beq v0,zero,LAB_8003955c` — erst NACH
   `@0x80039358 sb t2,0x0(a0)`, die Zeichenzahl bleibt also auf declaredCount stehen
   statt auf 0).
