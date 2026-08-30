# RE2-Leon — Item-Box BILDSCHIRM + BEDIENUNG (RE-Bericht, in Arbeit)

Ziel: der VOLLSTAENDIGE RE2-Mechanismus des Box-Screens (Zustandsautomat, Layout,
Navigation, Animation) als Grundlage fuer den Port.

Quelle: RE2-Leon SLUS-00748, `info/re2leon/PSX.EXE` (t_addr 0x80010000),
Ghidra-Dump `ghidra_re2_Leon.txt`, Decompilate `RE2_Quellcode_V2/FUN_*.c`.
Werkzeug: `python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n>`.

STATUS: **FERTIG** — alle 6 gestellten Fragen sind mit Adressen belegt (siehe §13).

## 0. Arbeitsprotokoll
- [gestartet] Datei angelegt.
- [F1] Screen-Struct-Basis + Funktionsliste gefunden (siehe §1).
- [F2] Box-Sub-State-Machine `switchD_8006fca8` (6 Faelle) lokalisiert.
- [F3] Pad-Bit-Belegung + Controller-Remap-Tabelle belegt (§4).
- [F4] Screen-Dispatcher (Typ x Phase) belegt, Panel-Modell korrigiert (§5).
- [F5] Case 1 = Box-Navigation vollstaendig disassembliert (§6).
- [F6] Layout komplett aus FUN_800710dc: 5 Zeilen, 20 px, Cursor fest in der Mitte (§7).
- [F7] Auto-Wiederholung gefunden: Bit31 DAT_800cfb74, Delay 10 / Rate 6 (§6.1).
- [F8] Blink, Transfer/Fehlermeldung, Texturen/CLUTs, Kurzfassung (§8–§13). FERTIG.

---

## 1. DIE SCREEN-STRUCT @ 0x800d5bf0  (BELEGT)

Alle Box-/Inventar-Screen-Funktionen laden `s1`/`s3` = `0x800d5bf0`
(z.B. `8006fc68: lui s1,0x800d / 8006fc6c: addiu s1,s1,23536` → 0x800d5bf0).
Damit sind die bekannten Einzel-DATs Felder EINER Struktur:

| Offset | Adresse | Bedeutung (bisher belegt) |
|---|---|---|
| +0x00 | DAT_800d5bf0 | **PHASE** des Screens: 0=Init, 1=Tick, 2=Exit (Dispatcher @0x80068c70). Init setzt 1 @0x8006f6bc / @0x8006f944 |
| +0x01 | DAT_800d5bf1 | **HAUPT-STATE** (Dispatch ueber Tabelle `0x800a9ba8` @0x8006f8d0 bzw. `0x800a9bb4` @0x8006fb8c). Init 2 @0x8006f6d8 (Inventar), 3 @0x8006f950 (Box) |
| +0x02 | DAT_800d5bf2 | **SUB-STATE der Box-Seite** → `switchD_8006fca8`, 6 Faelle (0..5) |
| +0x03 | DAT_800d5bf3 | Scroll-Animations-Framezaehler (0..6) |
| +0x0c | DAT_800d5bfc | **Inventar-Cursor** (Slot 0..n) |
| +0x10 | DAT_800d5c00 | **SCREEN-TYP** (Zeile in der Tabelle 0x800a936c). **1 = Item-Box**, 4 = Status |
| +0x16 | DAT_800d5c06 | Ein-Frame-Handshake „Box-Tick laeuft": 1 @0x8006fb7c, in Case 2/3/4/5 auf 0 |
| +0x1a | DAT_800d5c0a | Panel-/Seiten-Index fuer den Rahmen-Zeichner FUN_8006f1c4 |
| +0x34/+0x36 | DAT_800d5c24 / DAT_800d5c26 | zweiter Ursprung fuer die ICONS = (220, 70) @0x80074b88 |
| +0x24 | DAT_800d5c14 | **BOX-SCROLL-Stand** (0..0x3f) |
| +0x25 | DAT_800d5c15 | **Scroll-Pixel-Offset** (Animation, Schritt ±3) |
| +0x28 | DAT_800d5c18 | Blink-Zaehler (Cursor-Highlight) |
| +0x29 | DAT_800d5c19 | Blink-Richtung (0 = heller, 1 = dunkler) |
| +0x50 | DAT_800d5c40 | X-Basis (short) der Box-Liste |
| +0x52 | DAT_800d5c42 | Y-Basis (short) der Box-Liste |
| +0x58 | DAT_800d5c48 | Cursor-Sprite dx |
| +0x5a | DAT_800d5c4a | Cursor-Sprite dy |

Basis-Register `s3 = 0x800cc1e8` in der Box-State-Machine; alle Array-Zugriffe
laufen als `at = s3+idx*4 + 0x10000` und dann:
- `lbu …,-0x77ac(at)` = 0x800cc1e8+0x8854 = **0x800d4a3c** = Inventar[idx].id
- `lbu …,-0x77aa(at)` = **0x800d4a3e** = Inventar[idx]+2
- `lbu …,-0x7780(at)` = 0x800cc1e8+0x8880 = **0x800d4a68** = BOX[idx].id

---

## 2. BETEILIGTE FUNKTIONEN (Rollen, Adressen)

| Adresse | Rolle |
|---|---|
| `FUN_8006f1c4` @0x8006f1c4 | **Rahmen/Panel-Zeichner + Cursor-Blink** des Inventar-Panels. Faehrt `DAT_800d5c18`/`DAT_800d5c19` (Blink) hoch/runter und setzt CLUTs 0x1f2/0x1f5/0x1f7/0x1fa/0x1fd/0x1fe |
| `0x8006f6a8` (unbenannt) | **ENTER/Init des Inventar-Screens**: setzt +0x00=1, +0x29=1, +0x01=2, +0x58/+0x5a=0, +0x28=10; Jumptable @0x80011d20 (5 Faelle, Char-abhaengig via DAT_800d69f2) |
| `0x8006f878` | Cancel-Wache: `DAT_800ce310 & 0x6000` → SFX 0x4050000, +0x01 = 0 |
| `0x8006f8b4` | **Per-Frame-Tick des Inventar-Screens**: dispatch `(*(0x800a9ba8)[state])()`, dann FUN_8006f1c4 + FUN_8007526c |
| `0x8006f900` (unbenannt) | **ENTER/Init des BOX-Screens**: +0x00=1, +0x01=3, +0x25=0, +0x24=0, ruft FUN_80070e58, laedt 6 Icons (1 + Schleife 5) via FUN_80069c40 |
| `0x8006fb64` | **Per-Frame-Tick des BOX-Screens**: +0x16=1, dispatch `(*(0x800a9bb4)[state])()`, dann FUN_800710dc / FUN_80074bb0 / FUN_80072c6c / FUN_80074f4c / FUN_80073ed0 |
| `0x8006fbd4` | **Oberes Panel** (Panel-State 2): roh-DOWN (`DAT_800ce304 & 0x4000`) → SFX 0x0404, `DAT_800d5bfc = 1`, `DAT_800d5bf1 += 1` (ins Grid); sonst logische Flanke `DAT_800ce310 & 0x3000` → SFX 0x0406, `DAT_800d5bf1 = 0` (Fade-Out) |
| `0x8006fc58` | **BOX-SUB-STATE-MACHINE** (`switchD_8006fca8`, Jumptable @0x80011d44, 6 Faelle) |
| `FUN_800703b8` @0x800703b8 | **Transfer-Engine** (aus Case 4 aufgerufen @0x80070314) |
| `FUN_80069c40` @0x80069c40 | Icon-Cache-Upload `(0x10\|7\|…, id, &DAT_8019c000 + id*0x4b0)` |
| `FUN_800693d0` @0x800693d0 | Text-/Namens-Zeichner (a0=xy?, a1=?, a2=6, a3=item-id) |
| `FUN_80073350` @0x80073350 | Beschriftung/Menge? (a0 = Inventar[cursor]+2) |
| `FUN_80070e58` @0x80070e58 | Box-Screen-Vorbereitung (aus Init 0x8006f964) |
| `FUN_800710dc` @0x800710dc | zeichnet Box-/Inventar-Listen (liest DAT_800d5c14 + DAT_800d4a68) |
| `FUN_80072c6c` @0x80072c6c | zeichnet ebenfalls aus DAT_800d5c14/DAT_800d4a68 |
| `FUN_80074bb0`, `FUN_80074f4c`, `FUN_80073ed0` | weitere Zeichner der Box-Seite |
| `FUN_8005ba28` | SFX-Trigger (a0 = Sound-ID<<16) |

Jumptable der SUB-STATES @0x80011d44 (aus `ghidra_re2_Leon.txt:75797`):
```
80011d44  b0 fc 06 80  -> case 0 = 0x8006fcb0
80011d48  d4 fe 06 80  -> case 1 = 0x8006fed4
80011d4c  0c 00 07 80  -> case 2 = 0x8007000c
80011d50  9c 01 07 80  -> case 3 = 0x8007019c
80011d54  10 03 07 80  -> case 4 = 0x80070310
80011d58  24 03 07 80  -> case 5 = 0x80070324
```

---

## 3. ERSTE BELEGE ZUR SCROLL-ANIMATION (Case 2 / Case 3)

Case 2 @0x8007000c (Scroll RUECKWAERTS, `+0x24 -= 1`):
```
8007000c: lbu v0,0x3(s1)          ; Frame-Zaehler
80070014: addiu v1,v0,0x1
80070018: sltiu v0,v0,0x6         ; alt < 6 ?
8007001c: bne  v0,zero,0x80070118 ; ja -> nur animieren
80070020: sb   v1,0x3(s1)
…                                  ; nein -> COMMIT:
8007004c: lbu v0,0x24(s1)
80070050: sb  zero,0x25(s1)       ; Pixel-Offset = 0
80070054: sb  zero,0x3(s1)        ; Frame-Zaehler = 0
80070058: addiu v0,v0,-0x1
8007005c: andi  v0,v0,0x3f        ; WRAP 64
80070060: sb    v0,0x24(s1)
…
80070118: lbu v0,0x25(s1)
80070120: addiu v0,v0,0x3         ; Pixel-Offset += 3
80070124: sb  v0,0x25(s1)
```
Case 3 @0x8007019c (Scroll VORWAERTS, `+0x24 += 1`): identisch gespiegelt,
`800702ac: addiu v0,v0,-0x3` (Pixel-Offset **-3**) und
`80070200: addiu v0,v0,0x1 / 80070204: andi v0,v0,0x3f`.

→ **6 Animationsframes à 3 px, dann 1 Commit-Frame** (Zaehler 0..5 animiert,
Frame 6 committet) = **7 Frames pro Zeile**. Die Alt-Doku „6 Frames / ±3 px" ist
damit VERIFIZIERT.
⚠ WICHTIG: das Zeilenraster ist **20 px** (§7.1), die Animation legt aber nur
**18 px** zurueck — die restlichen **2 px macht der Commit als Sprung**.
Gezeichnete Offsets in Folge: 0 (Frame des Zustandswechsels), 3, 6, 9, 12, 15, 18,
dann 0 mit um 1 verschobenem `DAT_800d5c14`.
`DAT_800d5c15` wird ueberall als **VORZEICHENBEHAFTETES Byte** gelesen
(`800716b0: sll v0,v0,24 / 800716b4: sra v0,v0,24`), beim Runter-Scrollen laeuft es
also 0, −3 … −18.

`LAB_8007005c` ist auch das Sprungziel der SCHULTER-Tasten aus Case 0
(`8006ffa0(j)`, `8006ffc0(j)`) — also SOFORT-Commit ohne Animation.

---

## 4. PAD-WORTE — welche Taste ist welches Bit (BELEGT)

Roh-Pad-Wort `DAT_800ce2fc` wird @0x80038f00-0x80038f40 gebaut:
```
80038f04: lbu v0,DAT_800ce2db      ; Pad2 buf[3]
80038f0c: lbu a0,DAT_800ce2b6      ; Pad1 buf[2]  (SELECT..LEFT)
80038f24: lbu v0,DAT_800ce2b7      ; Pad1 buf[3]  (L2..SQUARE)
80038f1c: sll a0,a0,8
80038f2c: nor v1,v1,v0             ; aktiv-HIGH invertiert
80038f40: sw v1,DAT_800ce2fc
```
⇒ **Roh-16-Bit-Wort = ~(buf[2]<<8 | buf[3])**, also HIGH-Byte = D-Pad/START:

| Bit | Maske | Taste |
|---|---|---|
| 0 | 0x0001 | L2 |
| 1 | 0x0002 | R2 |
| 2 | 0x0004 | **L1** |
| 3 | 0x0008 | **R1** |
| 4 | 0x0010 | △ |
| 5 | 0x0020 | ○ |
| 6 | 0x0040 | ✕ |
| 7 | 0x0080 | □ |
| 8 | 0x0100 | SELECT |
| 11 | 0x0800 | START |
| 12 | 0x1000 | **UP** |
| 13 | 0x2000 | **RIGHT** |
| 14 | 0x4000 | **DOWN** |
| 15 | 0x8000 | **LEFT** |

Ableitung der Pad-Globals @0x80039340-0x800393ac:
| Adresse | Inhalt |
|---|---|
| `DAT_800ce2fc` (u32) | ROH **gehalten** |
| `DAT_800ce300` (u32) | ROH **Druck-Flanke** |
| `DAT_800ce304` (u16) | ROH gehalten, low16 ← **die Box benutzt DAS fuer D-Pad + L1/R1** |
| `DAT_800ce306` (u16) | ROH Flanke, low16 |
| `DAT_800ce30c` (u32) | **LOGISCH** (Controller-Config-gemappt) gehalten |
| `DAT_800ce310` (u32) | **LOGISCH Druck-Flanke** ← die Box benutzt DAS fuer die Aktionstasten |

Remap-Tabelle @`0x800a26a0`, 16×u16 pro Config, Config-Index `DAT_800d639e`
(Schleife @0x800391a0-0x800391ec: `logisch |= 1<<i, wenn roh & tab[cfg][i]`).
Config 0 (Type A) @0x800a26a0:
```
bit0 ←0x1000 UP    bit4 ←0x1000 UP    bit8 ←0x0008 R1     bit12←0x0040 ✕
bit1 ←0x2000 RIGHT bit5 ←0x4000 DOWN  bit9 ←0x0080 □      bit13←0x0010 △
bit2 ←0x4000 DOWN  bit6 ←0x0040 ✕     bit10←0x8000 LEFT   bit14←0x0020 ○
bit3 ←0x8000 LEFT  bit7 ←0x0040 ✕     bit11←0x2000 RIGHT  bit15←0
```
Config 1 (Type B) @0x800a26e0 unterscheidet sich NUR in bit13 ← 0x0090 (△|□);
Config 2 (Type C) @0x800a2720 == Config 0.

⇒ Im Box-Screen: `DAT_800ce310 & 0x1000` = **CONFIRM (Type A: ✕)**,
`& 0x2000` = **CANCEL (Type A: △)**, `& 0x4000` = dritte Taste (Type A: ○).

---

## 5. DIE PANEL-/STATE-EBENEN (3 Ebenen, alle belegt)

### 5.1 Ebene A — Screen-Dispatcher @0x80068c70 (VERIFIZIERT)
```
80068c6c: addiu s0,s0,-27796      ; s0 = 0x800a936c  (Tabelle)
80068c70: lbu   v1,0x10(s2)       ; v1 = DAT_800d5c00 = SCREEN-TYP
80068c78: sll   v0,v1,1
80068c7c: addu  v0,v0,v1
80068c80: sll   v0,v0,2           ; v0 = typ * 12  (3 Zeiger je Typ)
80068c84: lbu   v1,0x0(s2)        ; v1 = DAT_800d5bf0 = PHASE
80068c8c: sll   v1,v1,2
80068c94: lw    v0,0(v1+v0+s0)
80068c9c: jalr  v0                ; -> Tabelle[typ][phase]()
80068cac: j     0x80068c70        ; Endlosschleife bis Phase 2 abbricht
```
Tabelle @0x800a936c, je Typ {Phase0 = INIT, Phase1 = TICK, Phase2 = EXIT}:
```
Typ 0: 0x8006a574 / 0x8006a774 / 0x80068cd4
Typ 1: 0x8006f900 / 0x8006fb64 / 0x80068cd4   <== ITEM-BOX
Typ 2: 0x80071ba0 / 0x80071e14 / 0x80068cd4
Typ 3: 0x8006efd8 / 0x8006f164 / 0x80068cd4
Typ 4: 0x8006f6a8 / 0x8006f8b4 / 0x80068cd4   <== STATUS/INVENTAR
```
`DAT_800d5c00` (Struct +0x10) = **Screen-Typ**. Die Zeichner pruefen
`DAT_800d5c00 == 1` (FUN_80072c6c, FUN_80074bb0, FUN_80074f4c), um die
Box-Haelfte ueberhaupt zu zeichnen — **1 = Item-Box**.

### 5.2 Ebene B — `DAT_800d5bf0` = PHASE des Screens
- 0 = INIT (einmalig), 1 = TICK (jeder Frame), 2 = EXIT (bricht die Schleife ab)
- Box-INIT @0x8006f944 setzt Phase = 1; das Fade-Ende @0x80068f88 setzt Phase = 2.

### 5.3 Ebene C — `DAT_800d5bf1` = PANEL-Zustand, Dispatch-Tabellen
BOX-Tick @0x8006fb64 nutzt Tabelle `0x800a9bb4`:
```
[0] 0x80068f08   Fade-OUT starten -> DAT_800d5bf1 = 1
[1] 0x80068f40   auf Fade warten  -> DAT_800d5bf0 = 2 (Screen zu)
[2] 0x8006fbd4   OBERES PANEL: RUNTER (roh 0x4000) -> SFX 0x404, Inv-Cursor=1,
                 DAT_800d5bf1 += 1 (=3); logisch-Flanke &0x3000 -> SFX 0x406, =0 (schliessen)
[3] 0x8006fc58   GRID+BOX-Panel = die SUB-STATE-MACHINE (switchD_8006fca8)
```
STATUS-Tick @0x8006f8b4 nutzt Tabelle `0x800a9ba8` (ueberlappt!):
```
[0] 0x80068f08  [1] 0x80068f40  [2] 0x8006f878 (nur Abbruch, logisch &0x6000 -> SFX 0x405, =0)
[3] 0x80068f08  [4] 0x80068f40  [5] 0x8006fbd4  [6] 0x8006fc58
```
Init-Werte: Box-Init setzt `DAT_800d5bf1 = 3` @0x8006f950 (direkt ins Grid);
Status-Init setzt `= 2` @0x8006f6d8.

⇒ Die Alt-Portierung mit „Panels 0/2/3" trifft die BOX-Tabelle korrekt
(0=Fade-Out-Start, 1=Fade-Warten, 2=oberes Panel, 3=Grid). Der Zustand **1** fehlte.

### 5.4 Ebene D — `DAT_800d5bf2` = SUB-STATE (switchD_8006fca8), 6 Faelle
| Case | Adresse | Rolle |
|---|---|---|
| 0 | 0x8006fcb0 | **INVENTAR-Seite**: 2-spaltige Cursor-Navigation |
| 1 | 0x8006fed4 | **BOX-Seite**: Liste scrollen (das fehlende Stueck) |
| 2 | 0x8007000c | Scroll-Animation AUFWAERTS (scroll−1) |
| 3 | 0x8007019c | Scroll-Animation ABWAERTS (scroll+1) |
| 4 | 0x80070310 | **TRANSFER** — ruft FUN_800703b8, danach Sub-State = 0 |
| 5 | 0x80070324 | WARTEN auf `DAT_800e873c & 0x80`; danach Sub-State = 1 |

`DAT_800d5c06` (+0x16) ist ein **Ein-Frame-Handshake**: der BOX-Tick setzt ihn
@0x8006fb7c vor dem Dispatch auf 1; Cases 2/3/4/5 setzen ihn sofort auf 0
(`sb zero,0x16(s1)` @0x8007000c / 0x8007019c / 0x80070310 / 0x80070338).
Der Status-Tick setzt ihn NIE ⇒ Case 0 kann daran erkennen, ob er im
Box-Screen laeuft (Abfrage @0x8006fea4: `DAT_800d5c06 == 1`).

---

## 6. CASE 1 = DIE BOX-SEITEN-NAVIGATION (das fehlende Herzstueck) — BELEGT

Vollstaendiger Disasm `0x8006fed4`–`0x8007000b`:
```
8006fed4: lw v0,DAT_800cfb74
8006fee0: bgez v0,0x8006ffc8        ; Eingabe-Gate: nur wenn Bit31 gesetzt
8006fee8: lhu v1,0x211c(s3)         ; v1 = DAT_800ce304 = ROH GEHALTEN (kein Edge!)
; ---- HOCH ----
8006fef0: andi v0,v1,0x1000         ; UP
8006fef4: beq  v0,zero,0x8006ff20
8006fef8: lui  a0,0x214             ; SFX 0x02140000 (Scroll-Tick)
8006fefc: jal  0x8005ba28
8006ff0c: lbu  v0,0x24(s1)          ; scroll
8006ff10: li   v1,2
8006ff14: sb   v1,0x2(s1)           ; SUB-STATE = 2
8006ff18: j    0x8006ff4c
8006ff1c: addiu v0,v0,-1            ; Vorlade-Index = scroll-1
; ---- RUNTER ----
8006ff20: andi v0,v1,0x4000         ; DOWN
8006ff24: beq  v0,zero,0x8006ff8c
8006ff28: andi v0,v1,0x4            ; (Delay-Slot: L1 vorbereiten)
8006ff2c: jal  0x8005ba28           ; SFX 0x02140000 (a0 noch aus 8006fef8)
8006ff3c: lbu  v0,0x24(s1)
8006ff40: li   v1,3
8006ff44: sb   v1,0x2(s1)           ; SUB-STATE = 3
8006ff48: addiu v0,v0,5             ; Vorlade-Index = scroll+5
8006ff4c: andi v0,v0,0x3f           ; <- gemeinsame Vorlade-Route
8006ff60: lbu  a1,-0x7780(at)       ; a1 = BOX[idx].id
8006ff64: li   a0,7                 ; Icon-Cache-SLOT 7
8006ff7c: jal  FUN_80069c40         ; a2 = id*0x4b0 + 0x8019c000
8006ff84: j    switchD::default
; ---- L1 ----
8006ff8c: beq  v0,zero,0x8006ffa8   ; v0 = v1 & 0x0004 = L1
8006ff90: lui  a0,0x406             ; SFX 0x04060000
8006ff94: jal  0x8005ba28
8006ff9c: lbu  v0,0x24(s1)
8006ffa0: j    0x8007005c           ; SOFORT-COMMIT (keine Animation)
8006ffa4: addiu v0,v0,-5            ; scroll -= 5
; ---- R1 ----
8006ffa8: andi v0,v1,0x8            ; R1
8006ffac: beq  v0,zero,0x8006ffc8
8006ffb4: jal  0x8005ba28           ; SFX 0x04060000 (a0 noch aus 8006ff90)
8006ffbc: lbu  v0,0x24(s1)
8006ffc0: j    0x8007005c           ; SOFORT-COMMIT
8006ffc4: addiu v0,v0,5             ; scroll += 5
; ---- Aktionstasten (LOGISCHE Flanke) ----
8006ffc8: lw   v1,0x2128(s3)        ; DAT_800ce310
8006ffd0: andi v0,v1,0x2000         ; CANCEL (Type A: △)
8006ffdc: jal  0x8005ba28           ; SFX 0x04050000
8006ffe4: j    switchD::default
8006ffe8: sb   zero,0x2(s1)         ; SUB-STATE = 0  (zurueck zur Inventar-Seite)
8006ffec: andi v0,v1,0x1000         ; CONFIRM (Type A: ✕)
8006fff8: jal  0x8005ba28           ; SFX 0x04060000
80070000: li   v0,4
80070004: j    switchD::default
80070008: sb   v0,0x2(s1)           ; SUB-STATE = 4  (TRANSFER)
```

### 6.1 Antworten auf die gestellten Fragen
- **Schrittweite pro Tastendruck**: D-Pad HOCH/RUNTER = **±1 Zeile**, ueber die
  Sub-States 2/3 mit Animation. **L1 = −5, R1 = +5**, **SOFORT** ohne Animation
  (`j 0x8007005c`), SFX 0x0406.
  ⇒ „Schulter = ±5, Sofort-Commit @0x8007005c" ist **VERIFIZIERT**.
- **Wrap**: `andi v0,v0,0x3f` @0x8007005c bzw. @0x8006ff4c → **modulo 64**, kein Clamp.
  Die Box hat 64 Plaetze und die Liste ist ein **RING** (scrollt endlos durch).
- **Auto-Wiederholung**: JA — sie sitzt NICHT in der Box, sondern im Pad-Treiber
  und wird ueber **Bit31 von `DAT_800cfb74`** an die Box durchgereicht
  (deshalb das `bgez`-Gate am Anfang von Case 0 @0x8006fcc0 und Case 1 @0x8006fee0).
  Mechanik @0x800393b0-0x8003943a:
```
a0 = DAT_800dfc14                 ; Tasten-MASKE, die wiederholt
v1 = Flanke & a0
if (v1 != 0) {                    ; frischer Tastendruck
    DAT_800a2704 = DAT_800dfc18   ; Zaehler = ANLAUF-Verzoegerung
    DAT_800cfb74 |= 0x80000000    ; Puls JETZT
} else if (DAT_800a2704 != 0) {
    if (gehalten & a0) DAT_800a2704--;
    DAT_800cfb74 &= 0x7fffffff    ; KEIN Puls
} else {
    DAT_800a2704 = DAT_800dfc19   ; Zaehler = WIEDERHOL-Intervall
    DAT_800cfb74 |= 0x80000000    ; Puls
}
```
  Konfiguriert **beim Oeffnen des Item-Screens** in `FUN_800689bc`:
```
800689ec: ori   a0,zero,0xf01c    ; Maske = L1|R1|TRIANGLE|UP|RIGHT|DOWN|LEFT
800689f0: jal   FUN_80039464
800689f4: addiu a1,zero,1546      ; 0x060a  ->  DAT_800dfc18 = 0x0a = 10
                                  ;            DAT_800dfc19 = 0x06 =  6
```
  ⇒ **Anlauf-Verzoegerung 10 Frames, danach Wiederholung alle 6 Frames.**
  Das gilt fuer **L1/R1** (Sofort-Commit, also 5 Zeilen je Puls) und fuer den
  ERSTEN Anstoss von HOCH/RUNTER.
  Fuer das gehaltene HOCH/RUNTER greift die Wiederholung NICHT, weil Case 2/3
  am Ende der Animation den Roh-Halte-Zustand OHNE das Gate erneut pruefen
  (`80070024: lhu v0,0x211c(s3)` bzw. `800701b4`) und im Scroll-State bleiben.
  ⇒ **Halten von HOCH/RUNTER = 1 Zeile pro 7 Frames, gleichmaessig, ohne
  Anlaufverzoegerung. Halten von L1/R1 = +/-5 Zeilen sofort, dann Pause 10
  Frames, dann +/-5 alle 6 Frames.**

- **Seitenwechsel Inventar ↔ Box**:
  - Inventar-Seite (Case 0) → Box-Seite: CONFIRM (logisch 0x1000) @0x8006fe4c-0x8006fe68,
    SFX 0x0406, `+0x2 = 1`.
  - Box-Seite (Case 1) → Inventar-Seite: CANCEL (logisch 0x2000) @0x8006ffd0-0x8006ffe8,
    SFX 0x0405, `+0x2 = 0`.
  - Box-Seite → TRANSFER: CONFIRM (logisch 0x1000) → `+0x2 = 4`; Case 4 @0x80070310
    ruft `FUN_800703b8` und setzt `+0x2 = 0` (zurueck aufs Inventar).
  - Inventar-Seite HOCH am oberen Rand (Cursor < 2 und != 10) → `DAT_800d5bf1 -= 1`
    @0x8006fe1c-0x8006fe28 → oberes Panel (Panel-State 2); von dort RUNTER
    (roh 0x4000) @0x8006fbe0 → `DAT_800d5bf1 += 1` und Inv-Cursor = 1.
  - Aus Case 0 zusaetzlich: logisch 0x4000 **und** `DAT_800d5c06 == 1` (also nur im
    BOX-Screen) → SFX 0x0405, `DAT_800d5bf1 = 1` = Fade-Out/Screen zu
    (@0x8006fe88-0x8006fecc). Logisch 0x2000 → `DAT_800d5bf1 = 0` (Fade-Out starten).

### 6.2 Icon-Cache-Slots (BELEGT)
Commit-Tail @0x8007005c–0x8007010c laedt nach jedem Zeilenwechsel neu:
```
Slot  7 <- BOX[(scroll-1) & 0x3f]      (die Zeile UEBER dem Fenster)
Slot 10 <- BOX[(scroll+0) & 0x3f]
Slot 13 <- BOX[(scroll+1) & 0x3f]
Slot 16 <- BOX[(scroll+2) & 0x3f]
Slot 19 <- BOX[(scroll+3) & 0x3f]
Slot 22 <- BOX[(scroll+4) & 0x3f]
```
(`li s2,0xa` @0x8007009c, `addiu s2,s2,0x3` @0x800700c4, 5 Durchlaeufe)
Beim Richtungswechsel in Case 1 wird Slot 7 mit der EINLAUFENDEN Zeile geladen
(`scroll-1` bei HOCH, `scroll+5` bei RUNTER) — Slot 7 ist der Vorlade-Slot.
Init @0x8006f9f8-0x8006fa98 laedt Slot 7 = BOX[scroll] und 22/19/16/13/10 =
BOX[scroll+4..+0].
Ziel-Adresse je Icon: `0x8019c000 + id*0x4b0` (0x4b0 = 1200 Byte/Icon).

---

## 7. DARSTELLUNG / LAYOUT — komplett aus `FUN_800710dc` @0x800710dc (BELEGT)

Panel-Ursprung: **`DAT_800d5c40` (X) = 7**, **`DAT_800d5c42` (Y) = 14**
(gesetzt in `FUN_80070e58`: `80071090: li v0,7 / 80071098: sh v0,DAT_800d5c40`
und `8007109c: li v0,14 / 800710a4: sh v0,DAT_800d5c42`).
Alle folgenden Zahlen sind ABSOLUTE 320×240-Bildschirmkoordinaten
(= Panel-Offset + 7 bzw. + 14).

### 7.1 Es ist eine LISTE mit 5 sichtbaren Zeilen, Cursor FEST in der MITTE
Zeichen-Schleife (`iVar8 = 5 … while (iVar8 != 0)`), y-Start `sVar23 = 0x7f`,
Schrittweite `-0x14`:

| Anzeige-Index k | Box-Slot | Zeilen-Quad y | Item-Name y | Kommentar |
|---|---|---|---|---|
| 0 | `(scroll+0)&0x3f` | 41 – 60 | 43 | |
| 1 | `(scroll+1)&0x3f` | 61 – 80 | 63 | |
| 2 | `(scroll+2)&0x3f` | **81 – 100** | **83** | **AUSGEWAEHLT** |
| 3 | `(scroll+3)&0x3f` | 101 – 120 | 103 | |
| 4 | `(scroll+4)&0x3f` | 121 – 140 | 123 | |

- **Zeilen-Raster = 0x14 = 20 px**, Quad-Hoehe 19 px, Text-Baseline = Quad-Oberkante + 2.
- Item-Name-X = `boxX + 7` = **14**; Aufruf
  `FUN_800693d0(boxX+7, boxY + iVar10 + 0x1d + DAT_800d5c15, 6, id)`
  mit `iVar10 ∈ {80,60,40,20,0}`. Ist `BOX[slot].id == 0`, wird stattdessen
  **ID 0x64 (='d')** gezeichnet = der „leer"-Text.
- **Damit ist `(DAT_800d5c14 + 2) & 0x3f` = die Auswahl BESTAETIGT** —
  der Cursor steht fest auf der 3. von 5 Zeilen, die LISTE bewegt sich.

### 7.2 Icon-Quad je Zeile (POLY_FT4) — KORRIGIERT: es sind UVs, keine Farben
Pro Zeile ein **POLY_FT4** (40 Byte), doppelt gepuffert (`+0x28` je
`DAT_800ce5e0`), Basis `0x80198000` (6 Zeilen x 2 Puffer = 12 Prims).
Aufgebaut in `FUN_80070e58`:
```
GetTPage(1, 0, 0x1c0, 0x100)   -> 8-Bit-CLUT-Modus, Texturseite VRAM (448,256)
GetClut(0, 0x1e5)              -> CLUT bei VRAM (0, 485)
RGB = (0x80,0x80,0x80), code &= ~2  (nicht semitransparent)
```
`FUN_800710dc` setzt pro Frame XY und UV:
```
x0 = x2 = boxX + 0x92                     = 153
x1 = x3 = boxX + 0x93 + w                 = 178 (w=0x18)  bzw. 202 (w=0x30)
y0 = y1 = boxY + rowTop + DAT_800d5c15
y2 = y3 = boxY + rowBot + DAT_800d5c15    (rowBot = rowTop + 19)
u0 = u2 = 0x28 (40)
u1 = u3 = 0x50 (80)  wenn BOX[slot].size == 0,  sonst 0x78 (120)
v0 = v1 / v2 = v3 : je Zeile -30, siehe Tabelle
```
`w` und `u1` haengen an `DAT_800d4a6a[slot*4]` (= BOX[slot].size):
0 → schmal (25 px auf dem Schirm, 40 Texel), sonst → breit (49 px, 80 Texel).

**Zuordnung Zeile → Icon-Cache-Slot → UV-v (Beleg: `cVar17=-0x10`, `cVar19=0x0e`,
je Durchlauf `+0xe2` = −30):**
| Zeile | Cache-Slot | v0/v1 | v2/v3 | VRAM-y |
|---|---|---|---|---|
| einlaufend (Case 2/3) | 7 | 0x3c = 60 | 0x5a = 90 | 316–346 |
| slot+0 | 10 | 90 | 120 | 346–376 |
| slot+1 | 13 | 120 | 150 | 376–406 |
| slot+2 (**gewaehlt**) | 16 | 150 | 180 | 406–436 |
| slot+3 | 19 | 180 | 210 | 436–466 |
| slot+4 | 22 | 210 | 240 | 466–496 |

⇒ Die Icons werden auf **~62 %** skaliert gezeichnet (40x30 Texel → 25x19 px).

**Grosse (2-Feld-)Items belegen ZWEI Cache-Spalten:** `FUN_80069c40` laedt
@0x80069cd8-0x80069d64 eine ZWEITE 20x30-Kachel nach `slot+1`, wenn
`id-14 < 6 && id != 19` (also **id 14,15,16,17,18**), Quelle
`src + 0x19320` = `0x8019c000 + (id + 0x56) * 0x4b0`.
Deshalb reicht `u1 = 0x78` (120) genau ueber die Spalten 1 UND 2.

### 7.3 Feste Panel-Elemente (Sprite-Offsets relativ zu (7,14))
Tabelle @`0x800a9c88`-`0x800a9c9b` (s16-Paare):
```
0x800a9c8c: (94, 5)     HOCH/L1-Pfeil   -> absolut (101, 19)
0x800a9c90: (94,127)    RUNTER/R1-Pfeil -> absolut (101,141)
0x800a9c94: (248, 0)    CLUT 0x1e4
0x800a9c98: (252, 1)    CLUT 0x1f0, Helligkeit 0x40, bei DAT_800d5bf1==2: 0x80
```
16 Rahmen-Sprites, Koordinaten-Tabelle @`0x800a9c38`-`0x800a9c77` (s16-Paare):
`(3,0) (3,149) (131,0) (131,149) (0,0) (208,0) (211,0) (6,6) (6,127) (134,6)
 (134,127) (4,6) (196,6) (199,11) (199,4) (199,142)`
⇒ Panel-Ausdehnung relativ 0..211 × 0..149, absolut x 7..218, y 14..163.

**Pfeil-Hervorhebung (die Schulter-Tasten-Anzeige) — direkter Beleg fuer L1/R1:**
```
if ((DAT_800ce304 & 0x4008) == 0 || DAT_800d5bf2 == 0) hell=0x78 else hell=0x64  ; RUNTER|R1
if ((DAT_800ce304 & 0x1004) == 0 || DAT_800d5bf2 == 0) hell=0x50 else hell=0x3c  ; HOCH |L1
```
0x4008 = DOWN|R1, 0x1004 = UP|L1 → die Pfeile leuchten auf, solange die Taste
gehalten wird. Das ist ein zweiter, unabhaengiger Beweis fuer die
L1=0x0004 / R1=0x0008-Zuordnung aus §4.

### 7.4 Hintergrund-Streifen der NICHT gewaehlten Zeilen
4-fach-Schleife, Datensaetze `{y:u8, r:u8, g:u8, b:u8}` @`0x800a9c28`:
```
(26,48,48,48) (46,48,48,48) (87,48,48,48) (107,48,48,48)
```
Quad von `x = boxX+6 (13)` bis `boxX+0xc5 (204)`, Hoehe 0x14.
Absolut: y 40–60, 60–80, 101–121, 121–141 — die MITTLERE Zeile (81–100)
bleibt frei, dort liegt stattdessen das pulsierende Auswahl-Band (§8).

### 7.5 Scrollbalken (Ring-Position)
5 Marken, alle bei `x = boxX + 200 = 207`,
`y = boxY + 12 + ((DAT_800d5c14 + k) & 0x3f) * 2` = **26 + slot*2**, k = 0..4.
(Koordinaten-Tabelle @0x800a9c78-0x800a9c8b, alle Eintraege (200,12).)
⇒ 128 px hoher Balken (y 26..152) fuer die 64 Ring-Plaetze, 2 px je Platz.

### 7.6 Ring-Naht-Linie (Listenanfang/-ende)
```
x1 = boxX + 6 = 13 ; x2 = boxX + 0xc4 = 203
y  = DAT_800d5c15 + boxY + (0x3f - ((DAT_800d5c14 - 4) & 0x3f)) * 0x14 + 7
gezeichnet nur, wenn ((DAT_800d5c14 + 2) & 0x3f) < 5  ||  == 0x3f
```
Auswertung: scroll = 61/62/63/0/1/2 → y = 141/121/101/81/61/41.
Das ist genau die **Oberkante von Box-Slot 2** — und Slot 2 ist der bei
`scroll = 0` ausgewaehlte Platz, also der LISTENANFANG. Die Linie markiert
somit die Naht zwischen letztem (Slot 1) und erstem (Slot 2) Eintrag des Rings.

### 7.7 Anzahl Plaetze
`andi …,0x3f` an JEDER Stelle (0x8006ff4c, 0x8007005c, 0x80070084, 0x800700cc,
0x80070134, 0x800702c0, FUN_800710dc) ⇒ **64 Box-Plaetze**, Ring ohne Clamp.

### 7.8 Icon-Positionen + Icon-VRAM-Cache
Zweiter Ursprung `DAT_800d5c24`/`DAT_800d5c26` (Struct +0x34/+0x36) =
**(220, 70)**, gesetzt @0x80074b88/0x80074b94 (`li v0,220 / sh v0,52(s1)`,
`li v0,70 / sh v0,54(s1)`).
Icon-Offset-Tabelle @0x800aae2c (s16-Paare, aus `FUN_80072c6c`, iVar7=0x38..0x48):
```
slot+0: (-67,-18)  slot+1: (-67,  2)  slot+2: (-67, 22)
slot+3: (-67, 42)  slot+4: (-67, 62)
einlaufende Zeile:  x=-67,  y-Offset = -0x26 (hoch)  bzw. +0x52 (runter)
```
⇒ absolut **x = 153**, y = **52 / 72 / 92 / 112 / 132** (+ `DAT_800d5c15`),
also exakt an der linken Kante der Icon-Quads aus §7.2, Raster 20 px. ✓

`FUN_80069c40(slot, id, src)` @0x80069c40 laedt ein Icon per `LoadImage` in
VRAM: **Rect 20×30 (u16)**, `x = 448 + (slot%3)*20`, `y = 256 + (slot/3)*30`
(`80069c74: li v0,20 → w`, `80069c7c: li v0,30 → h`,
`80069cb0: addiu v0,v0,512` mit `-128 … >>1`, `80069cc4: addiu v0,v0,256`),
Quelle `0x8019c000 + id*0x4b0` (0x4b0 = 1200 = 20*30*2 Byte pro Icon).
Die Box-Slots 7/10/13/16/19/22 liegen alle in **Spalte 1**:
`x = 468`, `y = 316 / 346 / 376 / 406 / 436 / 466`.

---

## 8. CURSOR-HERVORHEBUNG UND BLINKEN — BELEGT

### 8.1 Das Auswahl-Band der Box-Liste (`FUN_800710dc`, Ende)
Zwei waagerechte Linien, `x1 = boxX+6 = 13`, `x2 = boxX+0xc4 = 203`,
`y = boxY + 0x6a - 0x14*n` → **y = 100 und y = 80** (n = 1, 2),
also die Ober- und Unterkante der mittleren (ausgewaehlten) Zeile 81–100.
Farbe:
```
if (DAT_800d5bf2 == 0)              // Inventar-Seite aktiv -> Band dunkel/statisch
    R=0x30  G=0x20  B=0x08
else                                 // Box-Seite aktiv -> Band pulsiert
    R = DAT_800d4cd0 + 0x50
    G = DAT_800d4cd0 + 0x40
    B = DAT_800d4cd0 & 0x1f
```

### 8.2 Der Blink-Zaehler `DAT_800d4cd0` (u16) + Richtung `DAT_800d766c` (u16)
Am Anfang von `FUN_800710dc`:
```
if (DAT_800d766c == 0) DAT_800d4cd0 += 2;  else DAT_800d4cd0 -= 2;
if (DAT_800d4cd0 > 0x3e) { DAT_800d4cd0 = 0x3f; DAT_800d766c = 1; }
if (DAT_800d4cd0 <  1)   { DAT_800d4cd0 = 0;    DAT_800d766c = 0; }
```
Initialisiert im Box-Init: `DAT_800d4cd0 = 1` @0x8006fa40, `DAT_800d766c = 0`
@0x8006fa48.
⇒ **Dreieck-Welle 0…0x3f, Schritt 2/Frame ⇒ ~32 Frames hoch + 32 runter
= 64-Frame-Zyklus (≈1,07 s bei 60 Hz, ≈1,28 s bei 50 Hz).**
Farbverlauf: R 0x50→0x8f, G 0x40→0x7f, B 0x00→0x1f (B laeuft wegen `&0x1f`
zweimal je Halbwelle durch).

### 8.3 Blinken des INVENTAR-Cursors (`FUN_8006f1c4`, anderer Zaehler)
```
if (DAT_800d5c19 == 0) { if (DAT_800d5c18 > 0x50) DAT_800d5c19 = 1;
                         DAT_800d5c18 += 2; }
else                   { if (DAT_800d5c18 < 10) { FUN_8005ba28(0x22b0000,0);
                                                  DAT_800d5c19 = 0; }
                         DAT_800d5c18 -= 2; }
```
(Init `DAT_800d5c18 = 10`, `DAT_800d5c19 = 1` @0x8006f6dc-0x8006f6f4.)
⇒ Bereich 10…0x50(80), Schritt 2 ⇒ 35 Frames je Richtung, ~70-Frame-Zyklus;
beim Erreichen des unteren Endes wird **SFX 0x022b** getriggert.
Die CLUT des markierten Inventar-Feldes wechselt dabei zwischen
`0x1f2` / `0x1f7` (blinkend) und `0x1f5+1` bzw. `0x1fa`.

---

## 9. TRANSFER (Case 4) + FEHLERMELDUNG (Case 5) — BELEGT

`FUN_800703b8` (`RE2_Quellcode_V2/FUN_800703b8.c`) arbeitet mit
`invCur = DAT_800d5bfc` und **`boxSel = (DAT_800d5c14 + 2) & 0x3f`** (Zeile 28).

Groessen-Byte (+2) im 4-Byte-Datensatz: **0 = 1 Feld, 1 = LINKE Haelfte eines
2-Feld-Items, 2 = RECHTE Haelfte** (Beleg: `DAT_800d4a3e = 1; DAT_800d4a42 = 2`
beim Einsetzen eines grossen Items in die Slots 0/1).

```
cVar12 = (INV[invCur].size != 0) * 2 + (BOX[boxSel].size != 0)
```
Fall `cVar12 == 1` (kleines Item in der Hand, grosses in der Box) und
zu wenig Platz (`FUN_80069668(1)`):
```
FUN_8002fe38(0xaf0010, 0xe400, 8, 0);   // Meldungs-Box oeffnen
DAT_800d5bf2 = 5;                       // -> Case 5
```
Case 5 @0x80070324 wartet, bis `DAT_800e873c & 0x80` geloescht ist
(Meldung geschlossen) und setzt dann `DAT_800d5bf2 = 1` (zurueck auf die
BOX-Seite).

Reihenfolge in Case 4 beachten: `sb zero,0x2(s1)` steht im **Delay-Slot** von
`jal FUN_800703b8` @0x80070314 — der Sub-State wird also ZUERST auf 0
(Inventar-Seite) gesetzt und nur im Fehlerfall von der Transfer-Engine
auf 5 ueberschrieben.

Zusaetzlich schreibt die Engine nach jedem Tausch den Icon-Cache-Slot **0x10**
neu: `FUN_80069c40(0x10, BOX[boxSel].id, 0x8019c000 + id*0x4b0)`
(Slot 0x10 = 16 ⇒ VRAM (468, 406) = die mittlere, ausgewaehlte Zeile). ✓

---

## 10. SFX-IDs (FUN_8005ba28(id<<16, 0))
| ID | Wo | Bedeutung |
|---|---|---|
| 0x0214 | 0x8006fef8 / 0x80070034 / 0x800701c4 | Zeilen-Scroll-Tick (auch bei Auto-Wiederholung) |
| 0x0404 | 0x8006fbec | vom oberen Panel ins Grid |
| 0x0405 | 0x8006fe74, 0x8006feb4, 0x8006ffd8, 0x8006f890 | Abbruch / Seite zurueck |
| 0x0406 | 0x8006fe54, 0x8006ff90, 0x8006fff4, 0x8006fc34 | Bestaetigen / L1 / R1 |
| 0x0409 | 0x8006f85c | Ende des Box-Inits |
| 0x022b | FUN_8006f1c4 | Inventar-Cursor-Blink-Umkehr |

---

## 11. CASE 0 = DIE INVENTAR-SEITE (2-Spalten-Gitter) — BELEGT

Alles auf `DAT_800d5bfc` (Struct +0x0c), Kapazitaet `DAT_800d46ac`,
Groessen-Byte `DAT_800d4a3e[slot*4]` (0 = 1 Feld, 1 = linke Haelfte, 2 = rechte Haelfte).
Gate: `8006fcc0: bgez DAT_800cfb74 -> ueberspringen` (Auto-Wiederholungs-Puls, §6.1).
Gelesen wird wieder das ROHE HALTE-Wort `DAT_800ce304`.

```
RECHTS 0x2000 @0x8006fcd0 : if (cursor < cap-1) { if (INV[cursor].size == 1) cursor++;  cursor++; }
LINKS  0x8000 @0x8006fd34 : if (cursor != 0 && cursor != 10) {
                                if (INV[cursor].size == 2) cursor = (cursor==1) ? cursor+1 : cursor-1;
                                cursor--; }
RUNTER 0x4000 @0x8006fda0 : if (cursor < cap-2) cursor += 2;  else if (cursor == 10) cursor = 1;
HOCH   0x1000 @0x8006fde8 : if (cursor >= 2 && cursor != 10) cursor -= 2;
                            else DAT_800d5bf1--;      // raus ins obere Panel
SFX    @0x8006fe2c        : hat sich der Cursor geaendert -> SFX 0x0404
```
⇒ **2 Spalten, zeilenweise** (±1 waagerecht, ±2 senkrecht); 2-Feld-Items werden
uebersprungen; Slot **10** ist ein Sonderplatz ausserhalb des Gitters
(RUNTER am Gitterende springt dorthin nicht — nur `10 → 1`).

Aktionstasten (LOGISCHE Flanke `DAT_800ce310`) @0x8006fe44:
```
& 0x1000 CONFIRM -> SFX 0x0406,  DAT_800d5bf2 = 1      (auf die BOX-Seite)
& 0x2000 CANCEL  -> SFX 0x0405,  DAT_800d5bf1 = 0      (Fade-Out / Screen zu)
& 0x4000 + DAT_800d5c06 == 1 -> SFX 0x0405, DAT_800d5bf1 = 1
                                 (nur im BOX-Screen erreichbar, §5.4)
```

Icons der Inventar-Seite zeichnet `FUN_80072c6c` @0x80072c6c (13er-Schleife ueber
`DAT_800d4a3c + k*4`, uebersprungen wenn `size == 2` oder `k >= DAT_800d46ac`),
Gitter-Rahmen `FUN_80074bb0` (Hoehe `(cap>>1)*0x1e`), Waffen-Markierung
`FUN_80074f4c` (`DAT_800d5bf8` = ausgeruestete Slot-Nr., 0x80 = keine),
Cursor-Sprite + Cursor-Blink `FUN_80073350`
(im BOX-Screen: `DAT_800d5c18` laeuft dort mit **Schritt 3 im Bereich 0x32..0x78**,
nicht mit 2/10..0x50 wie im Status-Screen — `FUN_80073350` und `FUN_8006f1c4`
benutzen dieselben Variablen mit unterschiedlichen Konstanten).

---

## 12. TEXTUR / CLUT — Uebersicht (BELEGT)

| Element | TPage / Quelle | CLUT |
|---|---|---|
| Box-Zeilen-Icons (POLY_FT4) | `GetTPage(1,0,0x1c0,0x100)` = 8-bit, VRAM (448,256) | `GetClut(0,0x1e5)` = VRAM (0,485) |
| Icon-Cache-Upload | `LoadImage` 20x30 u16 nach `(448+(slot%3)*20, 256+(slot/3)*30)`, Quelle `0x8019c000 + id*0x4b0`; 2. Kachel fuer id 14–18 aus `+(0x56)*0x4b0` | — |
| Panel-Rahmen / Pfeile | Prim-Templates in `FUN_80070e58` | `GetClut(0,0x1e7)`, `GetClut(0,0x1e4)`, `GetClut(0x100,0x1f0)` |
| Waffen-Markierung | `FUN_80074f4c` | `GetClut(0,0x1e5)` |
| Inventar-Cursor-Blink-CLUTs | `FUN_8006f1c4` | 0x1f2 / 0x1f5 / 0x1f7 / 0x1fa / 0x1fd / 0x1fe |
| Item-Namen / Text | `FUN_800693d0` → `FUN_80031070(x, y, flags\|0x4000, strId)` | Font-System |

Prim-Puffer (alle doppelt gepuffert ueber `DAT_800ce5e0`, Offset +0x14/+0x18/+0x28):
`0x80198000` Zeilen-Icons, `0x80198800` Zeilen-Hintergrund, `0x80199000`
Naht-/Blink-Linien, `0x80199200` Rahmen+Pfeile+Scrollbalken, `0x80199600`
Mengen-Ziffern (FUN_80072c6c), `0x8019a600` Waffen-Markierung,
`0x8019ac00` Inventar-Gitter, `0x8019b200/0x8019b280` unteres Panel.
OT-Buckets: `0x800cc1f0 + DAT_800ce5e0*0x20` (Liste), `+0x4` (Icons/Ziffern),
`+0x8` (Waffe), `+0xc` (Rahmen), `+0x10` (Zeiger-Kette).

---

## 13. KURZFASSUNG — was der Port braucht

1. **Zustaende**
   - Screen-Typ `DAT_800d5c00 = 1`; Phasen `DAT_800d5bf0` 0=Init, 1=Tick, 2=Exit.
   - Panel `DAT_800d5bf1`: 0 = Fade-Out starten, 1 = auf Fade warten (dann Phase 2),
     2 = oberes Panel, 3 = Grid+Box. Box-Init startet mit **3**.
   - Sub-State `DAT_800d5bf2`: 0 = Inventar-Seite, 1 = Box-Seite, 2 = Scroll hoch,
     3 = Scroll runter, 4 = Transfer, 5 = Fehlermeldung.
2. **Box-Seite (Sub-State 1)** — Eingaben ROH/gehalten `DAT_800ce304`,
   gegated durch den Auto-Wiederholungs-Puls (Bit31 `DAT_800cfb74`, 10/6 Frames):
   HOCH → Sub 2, RUNTER → Sub 3, L1 → scroll−5 sofort, R1 → scroll+5 sofort.
   Aktion (logische Flanke `DAT_800ce310`): 0x1000 → Sub 4 (Transfer),
   0x2000 → Sub 0 (Inventar-Seite).
3. **Scrollen**: `DAT_800d5c14` (0..63, `& 0x3f`, Ring). Auswahl =
   `(DAT_800d5c14 + 2) & 0x3f`. Animation: Zaehler `DAT_800d5bf3` 0..6;
   Frames 0–5 → `DAT_800d5c15 += 3` (hoch) bzw. `−= 3` (runter);
   Frame 6 → `DAT_800d5c15 = 0`, Zaehler = 0, `DAT_800d5c14 ∓ 1`.
   **6 x 3 px = 18 px von 20 px Zeilenraster, die letzten 2 px macht der Commit-Sprung.**
   Am Ende der Animation wird der Halte-Zustand erneut geprueft → nahtlose Wiederholung.
4. **Darstellung**: 5 sichtbare Zeilen à 20 px, y 41/61/81/101/121 (Quad-Hoehe 19),
   Panel-Ursprung (7,14). Cursor FEST auf Zeile 3 (y 81–100), markiert durch zwei
   waagerechte Linien y=80 und y=100 (x 13..203), Farbe pulsierend
   R=t+0x50, G=t+0x40, B=t&0x1f mit t = `DAT_800d4cd0` (0..0x3f, ±2/Frame, 64-Frame-Zyklus).
   Item-Name links bei x=14 (leer → String-ID 0x64), Icon rechts bei x=153
   (25x19 px, bei 2-Feld-Items 49x19).
5. **Scrollbalken** rechts bei x=207, 5 Marken bei y = 26 + slot*2.
6. **Naht-Linie** (Listenanfang) bei der Oberkante von Box-Slot 2, nur sichtbar
   solange `((scroll+2)&0x3f) < 5 || == 0x3f`.
7. **Icon-Cache**: 6 Slots (7 = Vorlader, 10/13/16/19/22 = die 5 Zeilen), bei jedem
   Zeilenwechsel neu geladen; Slot 0x10 (=16, die gewaehlte Zeile) wird zusaetzlich
   von der Transfer-Engine aktualisiert.
