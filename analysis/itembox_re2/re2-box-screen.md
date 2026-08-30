# RE2-Leon — Item-Box BILDSCHIRM + BEDIENUNG (RE-Bericht, in Arbeit)

Ziel: der VOLLSTAENDIGE RE2-Mechanismus des Box-Screens (Zustandsautomat, Layout,
Navigation, Animation) als Grundlage fuer den Port.

Quelle: RE2-Leon SLUS-00748, `info/re2leon/PSX.EXE` (t_addr 0x80010000),
Ghidra-Dump `ghidra_re2_Leon.txt`, Decompilate `RE2_Quellcode_V2/FUN_*.c`.
Werkzeug: `python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n>`.

STATUS: IN ARBEIT — wird inkrementell ergaenzt.

## 0. Arbeitsprotokoll
- [gestartet] Datei angelegt.
- [F1] Screen-Struct-Basis + Funktionsliste gefunden (siehe §1).
- [F2] Box-Sub-State-Machine `switchD_8006fca8` (6 Faelle) lokalisiert.

---

## 1. DIE SCREEN-STRUCT @ 0x800d5bf0  (BELEGT)

Alle Box-/Inventar-Screen-Funktionen laden `s1`/`s3` = `0x800d5bf0`
(z.B. `8006fc68: lui s1,0x800d / 8006fc6c: addiu s1,s1,23536` → 0x800d5bf0).
Damit sind die bekannten Einzel-DATs Felder EINER Struktur:

| Offset | Adresse | Bedeutung (bisher belegt) |
|---|---|---|
| +0x00 | DAT_800d5bf0 | Screen aktiv / „Panel-Kette"-Index. Init = 1 @0x8006f6bc, @0x8006f944. Wird bei ✕-Cancel `+1` @0x8006fc0c |
| +0x01 | DAT_800d5bf1 | **HAUPT-STATE** (Dispatch ueber Tabelle `0x800a9ba8` @0x8006f8d0 bzw. `0x800a9bb4` @0x8006fb8c). Init 2 @0x8006f6d8 (Inventar), 3 @0x8006f950 (Box) |
| +0x02 | DAT_800d5bf2 | **SUB-STATE der Box-Seite** → `switchD_8006fca8`, 6 Faelle (0..5) |
| +0x03 | DAT_800d5bf3 | Scroll-Animations-Framezaehler (0..6) |
| +0x0c | DAT_800d5bfc | **Inventar-Cursor** (Slot 0..n) |
| +0x16 | DAT_800d5c06 | Flag, in `0x8006fb7c` auf 1 gesetzt (Zeichnen-Enable?) — in den Box-Cases auf 0 gesetzt |
| +0x1a | DAT_800d5c0a | Panel-/Seiten-Index fuer den Rahmen-Zeichner FUN_8006f1c4 |
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
| `0x8006fbd4` | ✕/○-Wache: `DAT_800ce304 & 0x4000` → SFX 0x4040000, `DAT_800d5bfc=1`, +0x00 += 1; sonst `DAT_800ce310 & 0x3000` → SFX 0x4060000, +0x01 = 0 (Screen zu) |
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

→ **6 Animationsframes à 3 px = 18 px pro Zeile** (Zaehler 0..5 animiert,
Frame 6 committet). Die Alt-Doku „6 Frames / ±3 px" ist damit VERIFIZIERT
(Details/Zeilenhoehe siehe §5, noch zu vervollstaendigen).

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

### 5.1 Ebene A — Screen-Tabelle @0x800a936c (init / tick / exit, je 3 Zeiger)
```
[0] 0x8006a574 / 0x8006a774 / 0x80068cd4
[1] 0x8006f900 / 0x8006fb64 / 0x80068cd4   <== ITEM-BOX-SCREEN
[2] 0x80071ba0 / 0x80071e14 / 0x80068cd4
[3] 0x8006efd8 / 0x8006f164 / 0x80068cd4
[4] 0x8006f6a8 / 0x8006f8b4 / 0x80068cd4   <== STATUS/INVENTAR-SCREEN
```
(gelesen @0x80068c94)

### 5.2 Ebene B — `DAT_800d5bf0` = AEUSSERER Screen-Zustand
- 1 = laeuft (gesetzt beim Init @0x8006f944 bzw. @0x8006f6bc)
- 2 = fertig/geschlossen (gesetzt @0x80068f88 nach Ende des Fade-Out)

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
