# AUFGABE G — Audit des sprite.pri-PORT-Pfads gegen das Original

Datum: 2026-09-03 · Nur Analyse, **keine** Code-Aenderung.
Regel: jede Aussage traegt `@0x8.......` (Instruktion) oder Datei:Byte-Offset. Alles ohne Beleg ist
ausdruecklich als **NICHT BELEGT** markiert.

Messwerkzeuge dieses Laufs:
* `ghidra1_V2.txt` (roher Ghidra-Dump PSX.EXE) — alle Disasm-Zitate unten stammen daraus.
* Eigene Sweeps ueber **alle 240 `ROOM*.RDT`** in `re15_port/shared_assets/PSX/` (206 Basisraeume +
  Leon/Elza-Varianten `####0`/`####1`), 2188 Kamera-Cuts, davon 480 mit Maskensatz, 16654 Masken.
* `analysis/esp_masken_2026-09-03/sld.py` (SLD-Dekoder) fuer die BSS-Atlanten.

---

## 0. Das Original — vollstaendig disassembliert (Basis fuer alles Weitere)

### 0.1 Arena-Aufteilung `FUN_80039270` (pro Raum, aus RDT-Header-Byte[7])

```
80039278  lw    v1,0x0(a1)=>DAT_800ac77c      ; Bump-Arena-Kopf
80039280  lw    a0,-0x3888(a0)=>DAT_800ac778  ; RDT-Header im RAM
80039288  sw    v1,DAT_800b2584               ; FLAG-Array  = arena
8003928c  lbu   v0,0x7(a0)                    ; <<< hdr[7]
80039294  sll   v0,v0,0x2                     ; hdr7 * 4
8003929c/a0  sw v0,DAT_800bb4d4               ; PRIM-Puffer A = arena + hdr7*4
800392a4  lbu   v1,0x7(a0)
800392ac  sll   v1,v1,0x5                     ; hdr7 * 0x20
800392b8  sw    v1,DAT_800bb4d8               ; PRIM-Puffer B = A + hdr7*0x20
800392bc  lbu   v0,0x7(a0)
800392c4  sll   v0,v0,0x5
800392d0  sw    v0,0x0(a1)=>DAT_800ac77c      ; Arena-Kopf += hdr7*(4+0x20+0x20) = hdr7*68
```
=> **hdr[7] ist die Element-Zahl aller drei Puffer.** Es gibt im Original **keine** Konstante 64/105.
Flag-Record = **4 Byte**: `[0]=Flags, [1]=Gruppenindex+1, [2..3]=depth`.

### 0.2 Parser/Builder `FUN_800392d4` (einmal pro Kamera-Cut)

Aufruf: `@0x80021c28 jal FUN_800392d4`, **gegated** (siehe §0.5).

```
80039310  lw   v1,0x24(a0)          ; Kameratabelle (RDT+0x24, im RAM fixed-up)
80039314  sll  v0,v0,0x5            ; Cut-Index * 0x20  => Cut-Record = 32 B   [BELEGT]
8003931c  lw   t5,0x1c(v0)          ; Cut+0x1C = pri-Zeiger                     [BELEGT]
80039324  lw   v1,0x0(t5)           ; erstes u32 der Section
80039328  li   v0,-0x1
8003932c  bne  v1,v0,LAB_8003933c   ; TERMINATOR ist NUR das volle u32 == FFFFFFFF
80039330  _srl t2,v1,0x10           ; t2 = deklarierte Maskenzahl (obere Haelfte)
80039334  j    LAB_8003955c
80039338  _sb  zero,0x0(a0)         ; NULL-Section -> hdr[0] = 0
--- Nicht-NULL:
80039340/44 move t8,v1 ; andi t8,t8,0xffff     ; t8 = groupCount
80039348  addiu t5,t5,0x4                       ; t5 -> Gruppentabelle
8003934c  sll  v0,t8,0x3                        ; groupCount*8  => Gruppe = 8 B  [BELEGT]
80039350  addu a2,t5,v0                         ; a2 -> Maskendaten
80039358  sb   t2,0x0(a0)                       ; <<< hdr[0] = deklariert & 0xFF  [BELEGT]
--- Flag-Array loeschen (hdr7 Eintraege):
8003936c  lbu  a3,0x7(v0)
80039370..80039384  { sb zero,0(s5) ; s5 += 4 } while ((a3-1)&0xff != 0)
--- Aeussere Schleife (Gruppen):
800393b8  lhu  v0,0x0(t5)           ; group[i].count  (Gruppe +0)
800393c0  beq  v0,zero,LAB_80039540 ; count == 0  -> Gruppe komplett ueberspringen [BELEGT]
--- Innere Schleife (Masken der Gruppe):
800393d8  lbu  v0,0x0(s5) ; ori v0,v0,0x1 ; sb v0,0x0(s5)     ; Flag |= 1
800393e8  addiu v0,a3,0x1 ; sb v0,-0x1(t1)                    ; Flag[1] = Gruppenindex+1
800393f0  lhu  v0,-0x6(s2)                                    ; Maske+4 = depth
800393f8  sh   v0,0x0(t1)                                     ; Flag[2..3] = depth (unveraendert!)
800393fc  lhu  v1,-0x2(t7)   ; group+4 = destX  (UNSIGNED gelesen)
80039400  lbu  v0,-0x8(s2)   ; Maske+2 = dstX_lo
8003940c  addu t0,v1,v0                                        ; x = destX + dstX_lo  (16-bit-Store!)
80039410  lhu  v1,0x0(t7)    ; group+6 = destY
80039414  lbu  v0,-0x7(s2)   ; Maske+3 = dstY_lo
8003941c  addu s6,v1,v0
80039408  lbu  s7,0x0(a2)    ; Maske+0 = srcX (u0)
80039418  lbu  s8,-0x9(s2)   ; Maske+1 = srcY (v0)
80039404  lhu  a0,-0x4(s2)   ; Maske+6 = 16-Bit-GROESSENFELD
80039420  andi v0,a0,0xf000
80039424  bne  v0,zero,LAB_80039440        ; hohes Nibble != 0 -> QUADRAT
80039428  _srl v0,a0,0xc
   ; RECHTECK-Pfad:
8003942c  lhu  s4,-0x2(s2)   ; Maske+8  = WIDTH
80039430  lhu  s3,0x0(s2)    ; Maske+10 = HEIGHT
80039434/3c addiu s2/a2,+0xc                 ; Record = 12 B
   ; QUADRAT-Pfad:
80039440  sll  s3,v0,0x3     ; (feld>>12)<<3
80039444  move s4,s3
80039448/4c addiu s2/a2,+0x8                 ; Record = 8 B
--- SPRT befuellen (s0 = PRIM+0x1a = SPRT+0x0e; SPRT beginnt bei PRIM+0x0c):
800394ac..c0  sb 0x80 -> SPRT+4/5/6  (r0,g0,b0 = 0x80)
800394d0  sh  t0,-0x6(s0)  = SPRT+0x08 = x0
800394cc  sh  s6,-0x4(s0)  = SPRT+0x0a = y0
800394dc  sb  s7,-0x2(s0)  = SPRT+0x0c = u0 = srcX
800394e0  sb  s8,-0x1(s0)  = SPRT+0x0d = v0 = srcY
80039498  ori v0,zero,0x7800
80039500  sh  v0,0x0(s0)   = SPRT+0x0e = clut = 0x7800   <<< HARTKODIERT, NICHT aus den Daten
800394ec  sh  s4,0x2(s0)   = SPRT+0x10 = w
800394f0  sh  s3,0x4(s0)   = SPRT+0x12 = h
80039494  addiu s5,s5,0x4          ; Flag-Cursor GLOBAL (nicht pro Gruppe zurueckgesetzt)
80039504/08 addiu s0/s1,+0x20      ; PRIM-Cursor GLOBAL
--- Schleifenschranken (BEIDE 8-Bit-maskiert!):
8003950c  andi v0,t2,0xff ; 80039534 sltu v0,v0,v1 ; 80039538 bne -> innere Schleife
8003954c  andi v0,a3,0xff ; 80039550 sltu v0,v0,t8 ; 80039554 bne -> aeussere Schleife
```

**Folgerungen (alle BELEGT):**
* Cut-Record 32 B, `+0x1C` = pri-Zeiger.
* Section-Header 4 B = `u16 groupCount`, `u16 declared`.
* Gruppe 8 B = `u16 count`, `u16 <ungelesen>`, `s16 destX`, `s16 destY`.
  **Gruppe+2 wird von `FUN_800392d4` NIE gelesen.**
* Maske 8 B (Quadrat) / 12 B (Rechteck), Rechteck-Test = `(u16@+6 & 0xF000) == 0`.
* Build-Schranke = **Summe der Gruppen-Counts**, nirgends die deklarierte Zahl.
* Draw-Schranke = `hdr[0]` = `declared & 0xFF` (`sb` @0x80039358 + `lbu` @0x800395c0).
* Es existiert **keine** Wache gegen `groupCount == 0`, `declared == 0` oder `> 256`.
  Bei `> 255` wuerde die 8-Bit-Maskierung @0x8003950c/@0x8003954c sogar eine Endlosschleife bauen.

### 0.3 Per-Frame-Emitter `FUN_80039590`

```
800395c0  lbu  s4,0x0(v0)        ; s4 = hdr[0] = declared&0xFF  = ZAHL DER GEZEICHNETEN
800395cc  beq  s4,zero,Ende
800395e8  lbu  v0,0x0(s3) ; andi v0,v0,0x1 ; beq -> ueberspringen   ; Flag-Bit0
8003961c  jal  SetSprt(prim+0x0c)
80039630  ori  a3,zero,0x95
80039634  jal  SetDrawMode(prim, 1, 1, 0x95, NULL)
80039640  jal  MargePrim(prim, prim+0x0c)
80039650  lh   a0,0x2(s3)        ; depth, VORZEICHENBEHAFTET gelesen
80039654  sll  v0,v0,0xc         ; frameflip * 0x1000
80039658  sll  a0,a0,0x2         ; depth * 4 BYTE = depth * 1 WORT
8003965c  addu a0,a0,s6          ; s6 = 0x800aca34-0x235c = 0x800AA6D8
80039660  jal  AddPrim
8003966c  sltu v0,s2,s4          ; Schleife s2 = 0 .. hdr[0]-1
80039674  _addiu s3,s3,0x4
```
* **OT-Index der Maske = `depth` (×1)** — BELEGT @0x80039658.
* TPage `0x95` = 8-bit-CLUT-Textur, Basis (320,256), abr=0.
* CLUT-Id `0x7800` = VRAM (0,480)  [`clut=(y<<6)|(x>>4)`; 0x7800>>6 = 480].
* SPRT-Code = 0x64, **ABE-Bit (0x02) nicht gesetzt** -> **opak**, keine Semi-Transparenz.
  `SetShadeTex(...,1)` @0x8003947c/8c setzt Bit0 (raw texture); mit r/g/b=0x80 ist das
  rechnerisch identisch zur Modulation.

### 0.4 Das gemeinsame OT und die konkurrierenden Ebenen

```
80020c64  ori  a1,zero,0x400      ; 1024 Eintraege
80020c6c  jal  ClearOTagR
800215c8  addiu v0,s0,-0x1360     ; s0 = 0x800aca34 -> 0x800AB6D4 = 0x800AA6D8 + 1023*4
800215d0  jal  DrawOTag           ; also &ot[1023] -> reverse-verlinkt, Index 0 = VORNE
```

| Ebene | Index-Formel | Beleg |
|---|---|---|
| **sprite.pri-Maske** | `depth` (×1) | `sll a0,a0,0x2` @0x80039658 |
| **Charakter-Polygon** | `otz >> 4`, Near-Gate `otz < 64` verwirft | `stotz` @0x80025648, `sra v0,v1,0x6`+`beq` @0x80025654/58, `sra v1,v1,0x4` @0x8002565c |
| **Boden-Schatten** | `otz >> 4` (aus `RotAverage4`), **kein** Near-Gate | `jal RotAverage4` @0x8001b2c8, `srl v0,v0,0x4` @0x8001b2d8 |
| **ESP-Effekt-Sprite** | `sz >> 6` (roher View-Z), Near-Gate `sz>>2 == 0` | `gte_stsz` @0x8005356c, `sra v0,a0,0x2`+`beq` @0x80053578/7c, `sra v0,a0,0x6` @0x80053620 |
| **HUD / Text** | `0` (ganz vorne) | `sll v0,v0,0xc` + `addu a0,v0,a0` ohne Index-Term @0x80049258/5c |

GTE-Skalierungsfaktoren:
```
80066c70  li  t0,0x155      ; 341
80066c74  gte_ldZSF3 t0
80066c7c  li  t0,0x100      ; 256
80066c80  gte_ldZSF4 t0
```
=> Dreiecke `otz = (1023*vz)>>12`, Index `= (1023*vz)>>16`;
   Vierecke/Schatten `otz = vz>>2`, Index `= vz>>6`;
   ESP-Sprites Index `= vz>>6`.

**Einfuege-Reihenfolge innerhalb eines Frames** (eine Funktion, aufsteigende Adressen):
`0x8001ce54` Masken → `0x8001d09c` (`FUN_8001e8c8` → `FUN_800254a0` @0x8001ed04) Charaktere →
`0x8001d1d0` (`FUN_80052fd0` → ESP). Da `AddPrim` vorne einhaengt, zeichnet der **spaeter**
eingehaengte Prim **frueher** (= weiter hinten). **Bei Index-Gleichstand gewinnt also die Maske.**

### 0.5 ⚠ NEU GEFUNDEN: der Overdraw ist global gegated

```
; Parse-Aufruf (Cut-Wechsel):
80021c14  lw   v0,DAT_800aca38
80021c18  lui  v1,0x10                    ; Maske 0x00100000
80021c1c  and  v0,v0,v1
80021c20  bne  v0,zero,LAB_80021c34       ; Bit gesetzt -> FUN_800392d4 UEBERSPRINGEN
80021c28  jal  FUN_800392d4

; Emit-Aufruf (jeder Frame):
8001ce40  lw   v0,DAT_800aca38
8001ce44  lui  v1,0x10
8001ce48  and  v0,v0,v1
8001ce4c  bne  v0,zero,LAB_8001ce5c       ; Bit gesetzt -> FUN_80039590 UEBERSPRINGEN
8001ce54  jal  FUN_80039590
```
Gesetzt wird das Bit in `LAB_800151a0`:
```
80015214  lui  v1,0x10
80015228  lw   v0,0x0(a1)=>DAT_800aca38
80015230  or   v0,v0,v1                   ; |= 0x00100000
80015240  sw   v0,0x0(a1)=>DAT_800aca38
```
`DAT_800aca38 & 0x00100000` schaltet also **Parse UND Emit** ab. Der Port kennt dieses Gate nicht.
Welche Sequenz `LAB_800151a0` genau ist (Zaehler `DAT_800b522c >= 0x1b`, ruft `FUN_800217b0`
Fade mit `a1=-0x2556` und setzt zusaetzlich `0x04000000` sowie `DAT_800aca3c |= 0x40`) —
**NICHT BELEGT**. Ein Loeschen des Bits habe ich im Dump **nicht** gefunden
(Suche nach `lui …,0xffef` = 0 Treffer).

---

## 1. `re15_port/engine/src/pri_common.c` — Parser

### 1.1 Kappung `RE15_PRI_MAX_MASKS_PER_CUT = 105` (re15_pri.h:50)

Sweep ueber alle 240 RDTs / 2188 Cuts / 480 Maskensaetze:

| Groesse | Wert | Beleg |
|---|---|---|
| `max(hdr[7])` ueber alle Raeume | **105** | `ROOM3000.RDT` Byte 0x07 = `0x69` |
| `max(sum(group counts))` ueber alle Cuts | **105** | `ROOM3000.RDT` Cut 3, pri@0xdcc |
| Cuts mit Summe > 105 | **0** | — |
| Cuts mit Summe > `hdr[7]` desselben Raums | **0** | — |
| Raeume mit `max-über-Cuts(Summe) != hdr[7]` | **0 von 240** | — |

Byte-Beleg `ROOM3000.RDT` (`re15_port/shared_assets/PSX/STAGE3/ROOM3000.RDT`, 158784 B):
```
Offset 0x00 : 00 04 00 00 00 00 00 69      ; nCut=4, hdr[7]=0x69=105
Offset 0x24 : Kameratabelle = 0x60 ; Cut3-Record @0xc0 ; +0x1C -> pri_offset = 0xdcc
Offset 0xdcc: 08 00 69 00                  ; groupCount=8, declared=0x69=105
Gruppen-Counts: 26 23 9 8 1 14 18 6        ; Summe = 105
Gruppe0 @0xdd0: 1a 00 00 78 7b ff de ff    ; count=26, +2=0x7800, destX=-133, destY=-34
Maske0  @0xe10: 00 00 80 80 44 00 b5 00 08 00 28 00
                srcX=0 srcY=0 dstXlo=0x80 dstYlo=0x80 depth=0x0044 size=0x00b5(RECHTECK) w=8 h=40
```
**ERGEBNIS: 105 ist exakt ausreichend und keinen Eintrag zu gross. BELEGT.**
Die alte 64 haette in **26 von 240** Raeumen / **64 von 480** Masken-Cuts Masken verworfen
(z. B. ROOM3000 Cut 3: 41 von 105) — die Angabe in re15_pri.h:47-49 ist **bestaetigt**.

### 1.2 Die Wachen in pri_common.c:49/50

| Port-Zeile | Original? | Wirkung auf ECHTE Daten (480 Masken-Cuts) |
|---|---|---|
| `:48` `gc==0xFFFF && decl==0xFFFF -> return 0` | **JA**, `bne v1,v0` @0x8003932c prueft das volle u32 == 0xFFFFFFFF | korrekt |
| `:49` `group_count == 0 -> return 0` | **NEIN** — es gibt keine solche Wache. Das Original schreibt trotzdem `hdr[0]=declared&0xFF` @0x80039358, loescht das Flag-Array und faellt dann bei `sltu v0,v0,t8` @0x800393a4 sofort heraus. Sichtbares Ergebnis identisch (alle Flags 0 -> `FUN_80039590` zeichnet nichts). | **0 Cuts betroffen** |
| `:49` `mask_count_decl == 0 -> return 0` | **NEIN** — Original setzt `hdr[0]=0`, baut aber die Prims trotzdem. Sichtbar identisch (Draw-Schranke 0). | **0 Cuts betroffen** |
| `:50` `group_count > 256 \|\| mask_count_decl > 256 -> return 0` | **NEIN**. Original maskiert die Schleifenzaehler auf 8 Bit (@0x8003950c, @0x8003954c) — bei `>255` liefe es endlos. | **0 Cuts betroffen** (max `groupCount` gemessen = 34, max `declared` = 105) |

**Verdikt:** drei nicht-byte-true Wachen, aber **keine wirft bei echten Daten auch nur eine Maske
weg** (gemessen ueber alle 480 Masken-Cuts). Kein Datenverlust, aber sie kaschieren, dass das
Original hier abweichende Semantik hat.

### 1.3 `draw_count = declared & 0xFF` (pri_common.c:147)

**BELEGT** durch die Kette:
`_srl t2,v1,0x10` @0x80039330 → `sb t2,0x0(a0)` @0x80039358 (Byte-Store = Trunkierung auf 8 Bit)
→ `lbu s4,0x0(v0)` @0x800395c0 → `sltu v0,s2,s4` @0x8003966c.

Gemessen: `declared > 255` kommt **nie** vor (max 105), d. h. `& 0xFF` ist im Auslieferungsstand
wirkungslos, aber formal richtig.
`(declared & 0xFF) != sum(group counts)` gibt es in **genau 2 Cuts**:
`ROOM1210`/`ROOM1211` Cut 4 — declared 75, Summe 77.
Byte-Beleg (`STAGE1/ROOM1210.RDT`, 140596 B):
```
Offset 0x00 : 00 09 01 00 00 00 00 4d      ; nCut=9, hdr[7]=0x4d=77
Cut4-Record @0xe0 ; +0x1C -> pri_offset = 0x99c
Offset 0x99c: 22 00 4b 00                  ; groupCount=34, declared=0x4b=75
Gruppen-Counts: 5 2 3 1 2 2 3 1 2 1 2 3 5 7 3 1 1 1 1 1 1 1 1 1 1 2 3 1 2 2 3 4 0 8  (Summe 77)
Gruppe0 @0x9a0: 05 00 00 78 6a 00 d8 ff    ; count=5, destX=+106, destY=-40
Maske0  @0xab0: 00 00 80 80 35 00 95 00 08 00 88 00   ; depth=53, RECHTECK 8x136
```
Der Port baut 77 und zeichnet 75 — **identisch zum Original**. Der Klammerzusatz
`if (draw_count > out_count) draw_count = out_count;` (`:148`) ist im Original nicht vorhanden,
greift bei echten Daten aber nie (0 Cuts mit `declared&0xFF > Summe`).

### 1.4 Gruppen-Cursor bei `group_n[i] == 0`

**Kommt in echten Daten vor:** `ROOM1210`/`ROOM1211` Cut 4, Gruppenindex 32 hat `count = 0`
(siehe Liste oben). Original: `lhu v0,0x0(t5)` @0x800393b8 + `beq v0,zero,LAB_80039540`
@0x800393c0 ueberspringt die Gruppe komplett; `t7 += 8` @0x80039540, `a3 += 1` @0x80039544,
`t5 += 8` @0x80039558 — Maskencursor `a2/s2`, Flagcursor `s5` und Prim-Cursor `t4/t6` bleiben
unangetastet.
Port (`pri_common.c:117-120`): `while (grp < group_count && grp_used >= group_n[grp]) { grp++; grp_used = 0; }`
→ bei `group_n[32]==0` ist `0 >= 0` wahr, die Gruppe wird uebersprungen, der Anker kommt aus
Gruppe 33. **Verhalten identisch. KEIN Defekt.**

### 1.5 Weitere Parser-Befunde

| # | Datei:Zeile | Befund | Original |
|---|---|---|---|
| P1 | `pri_common.c:36` | `if (!data \|\| offset + 4 > data_size) return 0;` — `offset` ist `uint32_t`; `0xFFFFFFFF + 4` wickelt auf `3` um, die Wache greift **nicht**, und `data + 0xFFFFFFFF` liest weit ausserhalb. | Gemessen: `pri_offset` ist in **keinem** der 2188 Cuts literal `0xFFFFFFFF` (1708-mal *zeigt* er auf `FF FF FF FF`). Latenter, **nicht ausloesbarer** Defekt im Auslieferungsstand. |
| P2 | `re15_pri.h:13` / `pri_common.c:14-15` | Kommentar `u16 baseRaw (likely TPage / texture bank)` bzw. `baseRaw=0x7800 (TPage)` ist **sachlich falsch**. Gruppe+2 wird von `FUN_800392d4` **nie gelesen**; `0x7800` ist ein Code-Literal `ori v0,zero,0x7800` @0x80039498 und landet im **CLUT**-Feld `SPRT+0x0e` @0x80039500 — nicht im TPage. | Gemessen: Gruppe+2 ist in 2712 von 2750 Gruppen `0x7800`, aber auch `0x7C00`(26), `0x7C10`(8), `0x8080`(2), `0x8088`(2) — das Original benutzt trotzdem immer `0x7800`. Nur ein Doku-Fehler, kein Verhaltensfehler. |
| P3 | `re15_pri.h:18` | Kommentar `depth (Z priority value — 0=front, high=back)` ist richtig; `u8 unknown; u8 sizeByte` beschreibt das 16-Bit-Feld @+6 zerlegt, das Original liest `lhu a0,-0x4(s2)` @0x80039404 und maskiert `0xF000`. Der Port-Test `(mp[7] & 0xf0) == 0` (`:101`) ist dazu **exakt aequivalent** (Little-Endian: mp[7] ist das High-Byte). Ebenso `(size_b>>4)*8` == `(feld>>12)<<3` @0x80039440. **KORREKT.** |
| P4 | `pri_common.c:133-134` | `m->dstX = (uint16_t)(int16_t)(dstX_lo + ax)` — das Original addiert `lhu` (unsigned) + `lbu` und speichert mit `sh` (16-Bit-Trunkierung) @0x800394d0. Ergebnis bitgleich. **KORREKT.** |
| P5 | `pri_common.c:140` | `cursor++` — tote Variable (nie gelesen). Kosmetik. |
| P6 | `re15_pri.h:78` | Kommentar sagt `s6 = 0x800AA6D8 (+frameflip @0x800395e4)`. `@0x800395e4` berechnet nur die **Basis**; die Frameflip-Multiplikation steht `@0x80039654 sll v0,v0,0xc`. Doku-Ungenauigkeit. |
| P7 | — | `re15_pri.h:103-105` (`MASK_OT_BUCKET(depth)=depth`, `CHAR_OT_BUCKET(otz)=otz>>4`, `OTZ_NEAR_DROP=64`) sind alle **unabhaengig nachverifiziert** (§0.3/§0.4). |

Gemessene Datenkennzahlen (fuer die spaetere Verwendung):
* Groessenfeld hohes Nibble: `0`=14346 (Rechteck), `1`=706, `2`=1006, `3`=496, `4`=80, `5`=16, `7`=4.
  Nie 8..15 → Quadrat-Kantenlaenge 8..56.
* `depth`-Bereich 0..1023 (max = 1023, `ROOM1090` Cut 0), **nie negativ** (bei `lh` @0x80039650 relevant).
  `depth == 0` in **272** Masken, `depth == 1023` in **202** Masken.
* max `w` = 128, max `h` = 136.

---

## 2. `re15_port/platform/pc/src/bg_pc.c:63` — `re15_pri_load_cut_atlas`

Ablehnungsbedingungen (`bg_pc.c:75-82`):
1. Datei `BSS/ROOM####/PRI##.TIM` (Fallback `room####_pri##.tim`) fehlt → `return 0`.
2. `re15_tim_parse != 0`
3. `tim.bpp != 8`
4. `!tim.has_clut`
5. `tim.width * tim.height > 65536` (Groesse von `s_pri_atlas_rgba[256*256]`)

### 2.1 Wuerden die STAGE1-Atlanten (61984 B) akzeptiert? — **JA.**

Ich habe den SLD-Block von `ROOM1020` **tatsaechlich dekodiert**
(`BSS/ROOM1020/BG00.BSS`, SLD-Block @Byte 31672, entpackt 61984 B):
```
erste 16 Byte: 10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e0 01
  magic 0x00000010, flags 0x00000009  -> bpp-Code 1 = 8 Bit, has_clut = 1
  CLUT-Block: size 524, VRAM (0,480), 256 x 1   <<< = CLUT-Id 0x7800, exakt @0x80039498
  IMG -Block: size 61452, VRAM (0,0), 128 Halbwoerter x 240  -> 256 x 240 Pixel
  8 + 524 + 61452 = 61984   [exakte Rechnung, keine Schaetzung]
  clut[0] = 0x0000, genau 1 Null-Eintrag
```
`re15_tim_common.c:79` rechnet fuer bpp 8 `width = data_w * 2` = **256**, `height = 240`
(`:83`) → `256*240 = 61440 <= 65536` → **Bedingung 5 greift NICHT.**
=> Der PC-Lader wuerde die STAGE1-Atlanten **anstandslos annehmen**.
Der einzige Grund fuer den fehlenden Overdraw ist Bedingung **1: die Datei existiert nicht.**

Zum Vergleich: die 418 tatsaechlich extrahierten `PRI##.TIM` sind **alle** 66080 B =
256 x 256 x 8 bpp + 256er-CLUT bei VRAM (0,480). `256*256 = 65536` ist **exakt** die
Puffergrenze — `> 65536` ist falsch, also gerade noch akzeptiert. Ein 256x257-Atlas fiele durch.

### 2.2 Abdeckungsluecke (gemessen)

| Groesse | Wert |
|---|---|
| Cuts mit Maskensatz | **480** |
| davon **ohne** passende `PRI##.TIM` | **188 (39,2 %)** |
| Raeume mit Masken | 68 (Varianten mitgezaehlt) |
| Raeume ganz ohne Atlas | **24**: `ROOM1020/1021/1030/1031/1040/1041/1070/1071`, `ROOM5030/5031/5040/5041/5060/5061/50A0/50A1/50C0/50C1/5110/5111/5120/5121/5140/5141` |

=> **39 % der gesamten Vordergrund-Verdeckung des Spiels sind im PC-Port tot**, allein wegen
fehlender Extraktion. `main.c:4453` gated die Masken mit `has_fg && pri_n > 0`; das Original
kennt kein solches Gate (der Atlas liegt im BSS-Cut und ist immer geladen).

### 2.3 Farbschluessel

Port (`bg_pc.c:85`): `if (ix == 0) alpha = 0`.
PSX-Regel: fuer 4/8-Bit-Texturen wird **nach** dem CLUT-Lookup gegen `0x0000` getestet.
Gemessen ueber **alle 418** extrahierten Atlanten **und** den dekodierten ROOM1020-Block:
`clut[0] == 0x0000` und **genau ein** Null-Eintrag pro CLUT.
=> Beide Regeln fallen hier zusammen. **Formal abweichend, praktisch aequivalent (418/418 + 1/1).**

Farbtiefe: `bg_pc.c:88-90` expandiert 5→8 Bit mit `<<3` (0..248 statt 0..255). Konsistent mit
dem uebrigen Port; kein sprite.pri-spezifischer Befund.

---

## 3. `re15_port/platform/pc/src/render_pc.c` ~700-840 — Tiefensortierung und Blit

### 3.1 Index 0 = transparent

Original: SPRT-Code `0x64` (aus `SetSprt`), **ABE-Bit 0x02 nicht gesetzt** → opak; der
`abr=0`-Anteil in TPage `0x95` (`ori a3,zero,0x95` @0x80039630) ist damit wirkungslos.
Transparenz kommt ausschliesslich vom Texel-0-/CLUT-0x0000-Mechanismus.
Port: `SDL_BLENDMODE_BLEND` mit alpha 0 fuer Index 0 → gleiches Ergebnis.
**KORREKT** (siehe §2.3 fuer den formalen Unterschied).

### 3.2 Die Herleitung `re15_pri_mask_camera_z(depth) = depth*64`

Mit den in §0.4 belegten Formeln:

| Gegenspieler | OT-Index | Maske verdeckt ihn, wenn | Schwelle in View-Z |
|---|---|---|---|
| **ESP-Sprite** | `vz >> 6` | `depth <= vz>>6` (Gleichstand geht an die Maske, §0.4) | `vz >= depth*64` — **exakt** |
| **Schatten / Vierecke** (AVSZ4, ZSF4=256 → `otz = vz>>2`) | `otz>>4 = vz>>6` | dito | `vz >= depth*64` — **exakt** |
| **Charakter-Dreiecke** (AVSZ3, ZSF3=341 → `otz = (1023*vz)>>12`) | `(1023*vz)>>16` | dito | `vz >= depth*65536/1023 = depth*64,0625` — **0,098 % zu niedrig** |

Der PC-Sortierschluessel ist der rohe View-Z: `main.c:1585` `avgz = (wz0+wz1+wz2)/3`,
`main.c:392` fuer ESP `z = (int)vz` — dieselbe Einheit. `depth*64` ist damit **richtig
hergeleitet und fuer 2 von 3 Ebenen exakt**. Die frueheren Werte (`depth*2`, `K=32`) sind
zu Recht verworfen.

**Restabweichung D1 — Vergleichsrichtung (`render_pc.c:793-794`, `:823`):**
```c
int mask_due = (mi < mask_n) && ((float)re15_pri_mask_camera_z(...) >= tri_depth);
```
`>=` bedeutet: bei `vz == depth*64` wird die Maske **unter** das Dreieck gelegt.
Original: gleicher OT-Index → Gleichstand → die frueher eingehaengte **Maske gewinnt** und
liegt **oben** (Einfuege-Reihenfolge belegt in §0.4: Masken @0x8001ce54 vor Chars @0x8001d09c
vor ESP @0x8001d1d0). Korrekt waere `>`. Betrifft genau den Gleichheitsfall — **Exaktheits-
befund, kosmetisch**.

**Restabweichung D2 — Quantisierung:** das Original sortiert in 1024 ganzzahlige Eimer, der
PC-Port kontinuierlich. Innerhalb eines Eimers (63 View-Z-Einheiten) entscheidet im Original
die Einfuegereihenfolge, im Port der exakte Z-Wert. Bauartbedingt; `depth*64` ist die bestmoegliche
kontinuierliche Naeherung.

**Abweichung D3 — Vierecke:** das Original benutzt fuer ein Viereck **einen** AVSZ4-Wert
(Mittel ueber 4 Ecken, ZSF4=256). Der Port zerlegt Vierecke in zwei Dreiecke mit je **eigenem
3-Ecken-Mittel** (`main.c:6711/6712`, `main.c:6926/6927`), waehrend ein anderer Pfad
(`main.c:7933`) `wz_avg /= 4` benutzt. Inkonsistent und in beiden Faellen != AVSZ4. Beeinflusst
die Maske-gegen-Viereck-Entscheidung.

### 3.3 Einsortierung gegen Schatten / ESP / HUD

| Ebene | Original | PC-Port | Verdikt |
|---|---|---|---|
| **Schatten** | eigener OT-Index `otz>>4` (`srl v0,v0,0x4` @0x8001b2d8) → ein Schatten **naeher** als die Maske (`otz>>4 < depth`) zeichnet **spaeter** und **ueberdeckt die Maske** | `render_pc.c:702-710` "Step 1.5": **alle** Schatten werden **vor** dem gesamten Tri/Masken-Merge gezeichnet → die Maske liegt **immer** ueber jedem Schatten | **ABWEICHUNG D4.** Der Schatten ist im Port nie vor einer Maske. (Gleichstandsrichtung fuer Schatten: **NICHT BELEGT** — den Einfuegepunkt von `FUN_8001b064` im Frame habe ich nicht bis zum Frame-Treiber zurueckverfolgt; Aufrufer ist `@0x80031dcc`.) |
| **ESP-Effekte** | Index `sz>>6` @0x80053620 | ESP-Sprites gehen ueber `re15_render_textured_tri(..., z=vz, ...)` (`main.c:422-425`, `z` aus `main.c:392`) in **dieselbe** Tri-Warteschlange und damit in denselben Merge wie die Masken | **KORREKT** bis auf D1/D2 |
| **HUD / Text / Letterbox / Fade** | Index `0` = ganz vorne (@0x80049258) | werden in `end_frame` **nach** dem Merge gezeichnet | **KORREKT** — mit der Einschraenkung, dass eine Maske mit `depth == 0` (272 Stueck gemessen) im Original im **gleichen** Eimer 0 liegt wie der Text und dort (frueher eingehaengt) sogar **ueber** ihm laege. Praktisch belanglos, formal Abweichung. |

### 3.4 Weitere Renderer-Befunde

| # | Datei:Zeile | Befund |
|---|---|---|
| D5 | `render_pc.c:761` | `mask_n = (s_pri_atlas_tex && s_pri_rect_count > 0) ? … : 0` — ohne Atlas-Textur **kein** Overdraw (siehe §2.2, 39 % der Cuts). |
| D6 | `render_pc.c:790-830` | Der Masken-Merge liegt **innerhalb** von `if (s_textri_count > 0 && s_tim_texture)`. Fehlt die Tri-Textur, werden alle Masken erst in der Nachlaufschleife `:832-838` gezeichnet — Reihenfolge gegenueber Nicht-Tri-Ebenen dadurch anders. Kein Datenverlust. |
| D7 | `main.c:4460` | Der Port liest `pri.masks[0 … draw_count-1]`. Original: `FUN_80039590` laeuft ueber Flag-Eintraege `0 … hdr[0]-1`, und der Flag-Index **ist** der Build-Index (`s5 += 4` @0x80039494 global). **Identisch. KORREKT.** |
| D8 | `render_pc.c:1762` | `if (count > RE15_PRI_RECTS_MAX) count = …` — greift nie (max 105 = Kapazitaet). |
| D9 | Geometrie | Gemessen: **1444 von 16654** Masken ragen ueber `320x240` hinaus (x −6 … 370, y −22 … 460). Beide Seiten clippen (PSX per DrawEnv, SDL per Render-Target) — kein Defekt, aber ein Hinweis, dass `dstX/dstY` **vorzeichenbehaftet** verarbeitet werden muessen; `main.c:4464` castet korrekt `(int16_t)`. |
| D10 | Gate | Das Original-Gate `DAT_800aca38 & 0x00100000` (§0.5) existiert im Port **nicht**, weder fuer den Parse noch fuer das Emit. |

---

## 4. `re15_port/platform/psx/src/render.c:440` — `re15_render_pri_sprites`

| # | Zeile | Befund | Original |
|---|---|---|---|
| X1 | `:451` `if (z < 1) z = 1;` | **ABWEICHUNG.** Betrifft die **272** gemessenen Masken mit `depth == 0`; sie landen in Eimer 1 statt 0. | `sll a0,a0,0x2` @0x80039658 ohne jede Klemmung; `depth=0` → `ot[0]`. |
| X2 | `:452` `if (z >= OT_LENGTH-1) z = OT_LENGTH-2;` | **ABWEICHUNG.** Betrifft die **202** gemessenen Masken mit `depth == 1023`; sie landen in Eimer 1022 statt 1023. `ot[1023]` ist im Port selbst der `DrawOTagEnv`-Kopf (`:259`) und wird gezeichnet. | `DrawOTag(&ot[1023])` @0x800215d0 → Eimer 1023 ist gueltig und der hinterste. |
| X3 | `:454-457` Kommentar | `"Added to a frontmost OT bucket (ot[1] …)"` und `"(Per-mask OT depth = a future byte-true refinement.)"` sind **veraltet** — der Code benutzt bereits `RE15_PRI_MASK_OT_BUCKET(m->depth)`. |
| X4 | `:465` `setDrawTPage(tp, 0, 1, tpage)` | dfe = 0. Original `SetDrawMode(p, 1, 1, 0x95, NULL)` @0x80039630/34 setzt **beide** Argumente auf 1. **ABWEICHUNG** (dfe = "draw to display area"). |
| X5 | `:458-460` | `sp->clut = re15_pri_psx_clut` (Laufzeitwert) statt hartkodiertem `0x7800`; `setRGB0(sp,128,128,128)` ohne `SetShadeTex`. Rechnerisch identisch (Modulation mit 0x80 = Identitaet), aber nicht dieselbe Instruktionsfolge. **Aequivalent, nicht identisch.** |
| X6 | `:463-466` Reihenfolge | SPRT wird vor DR_TPAGE per `addPrim` eingehaengt → DR_TPAGE zeichnet zuerst. **KORREKT.** Original erreicht dasselbe mit `MargePrim` @0x80039640 (ein einziger Prim). |
| X7 | `:444` `if (!re15_pri_psx_ok \|\| !pri \|\| pri->draw_count <= 0) return;` | Atlas-Gate wie auf PC (siehe §2.2). Kein Original-Gegenstueck. |
| X8 | `pri_psx.c:58` `int rows = (t.height < 128) ? t.height : 128;` | **SCHWERSTE PSX-ABWEICHUNG.** Nur die ersten **128** von 256 Atlaszeilen werden ins VRAM geladen. Gemessen: `max(srcY + h)` ueber alle Masken = **256** (`ROOM3070` Cut 4 Maske 38: srcY=184, h=72), und **2080 von 16654 Masken (12,5 %)** samplen unterhalb Zeile 128 → sie lesen fremdes/undefiniertes VRAM. |
| X9 | `pri_psx.c:31-35` | VRAM-Ablage Bild (512,256), CLUT (0,502). Original: Bild-TPage `0x95` = (320,256), CLUT `0x7800` = (0,480). Bewusst verlegt und im Kommentar dokumentiert — bei einem VRAM-Vergleich gegen das Original ist das eine erwartete Differenz. |
| X10 | `psx/main.c:471-477` | Parse laeuft nur fuer `re15_test_rdt`; `re15_render_pri_sprites(&s_pri_cut)` @`psx/main.c:706`. Kein `re15_pri_parse_section`-Aufruf pro Cut ausserhalb dieses Demo-Pfads. |

Die vier STAGE1-Raeume mit 240-Zeilen-Atlas sind von X8 **nicht** betroffen
(gemessenes `max(srcY+h)`: ROOM1020 = 160, ROOM1030 = 224, ROOM1040 = 184, ROOM1070 = 136).

---

## 5. Abweichungsliste (Datei:Zeile → Original-Adresse)

| # | Ort | Abweichung | Original-Beleg | Datenwirkung (gemessen) |
|---|---|---|---|---|
| **A1** | `bg_pc.c:75-77` + `main.c:4453` | Overdraw haengt an einer vorextrahierten `PRI##.TIM`; fehlt sie, ist er komplett aus | Original hat kein Atlas-Gate; `FUN_80039590` @0x80039590 laeuft immer, wenn `hdr[0] != 0` | **188 von 480 Masken-Cuts (39 %) ohne jede Verdeckung**, 24 Raum-Varianten |
| **A2** | `psx/src/pri_psx.c:58` | nur 128 von 256 Atlaszeilen ins VRAM | Original laedt den ganzen SLD-TIM | **2080 von 16654 Masken (12,5 %)** samplen ins Leere |
| **A3** | `psx/src/render.c:451` | `z < 1 → 1` | `sll a0,a0,0x2` @0x80039658 (keine Klemmung) | **272 Masken** (depth 0) einen Eimer zu weit hinten |
| **A4** | `psx/src/render.c:452` | `z >= 1023 → 1022` | `DrawOTag(&ot[1023])` @0x800215d0 | **202 Masken** (depth 1023) einen Eimer zu weit vorne |
| **A5** | `render_pc.c:702-710` | alle Schatten pauschal vor dem Merge | Schatten-Index `otz>>4` @0x8001b2d8 im **selben** OT | Maske liegt nie hinter einem Schatten |
| **A6** | — (fehlt im Port) | Gate `DAT_800aca38 & 0x00100000` fuer Parse **und** Emit | `bne` @0x80021c20 und @0x8001ce4c, gesetzt @0x80015230 | Port zeichnet Masken in einem Zustand, in dem das Original sie unterdrueckt |
| **A7** | `render_pc.c:793`, `:823` | `>=` statt `>` | Gleichstand geht an die frueher eingehaengte Maske (`AddPrim`, Reihenfolge §0.4) | nur `vz == depth*64` exakt |
| **A8** | `main.c:6711/6712`, `:6926/6927` vs `:7933` | Viereck-Tiefe als 2x 3-Ecken-Mittel bzw. `/4` statt AVSZ4 | `gte_ldZSF4 0x100` @0x80066c80 | Maske-gegen-Viereck-Entscheidung leicht verschoben |
| **A9** | `pri_common.c:49` | `group_count == 0 \|\| declared == 0 → return 0` | keine solche Wache; Original schreibt trotzdem `hdr[0]` @0x80039358 | **0 Cuts** betroffen |
| **A10** | `pri_common.c:50` | `> 256 → return 0` | keine solche Wache; Original maskiert 8-bittig @0x8003950c/@0x8003954c | **0 Cuts** betroffen |
| **A11** | `pri_common.c:148` | `draw_count` auf `out_count` geklemmt | keine Klemmung im Original | **0 Cuts** betroffen |
| **A12** | `pri_common.c:36` | `offset + 4` wickelt bei `offset == 0xFFFFFFFF` um → OOB-Lesezugriff | — | **0 Cuts** betroffen (nie literal 0xFFFFFFFF) |
| **A13** | `bg_pc.c:85` | Transparenz an Index 0 statt an `clut[ix] == 0x0000` | PSX-Texel-/CLUT-Regel | **0** Atlanten betroffen (418/418 haben genau `clut[0]==0`) |
| **A14** | `psx/src/render.c:465` | `dfe = 0` | `SetDrawMode(p,1,1,0x95,NULL)` @0x80039634 | — |
| **A15** | `re15_pri.h:13`, `pri_common.c:14-15` | Doku: `baseRaw` sei TPage | Gruppe+2 wird nie gelesen; `0x7800` ist Literal @0x80039498 und ein **CLUT** @0x80039500 | reiner Doku-Fehler |
| **A16** | `psx/src/render.c:454-457` | Kommentar behauptet `ot[1]`/"future refinement" | Code macht laengst `depth` | reiner Doku-Fehler |
| **A17** | `re15_pri.h:78` | Frameflip-Adresse falsch zitiert | `sll v0,v0,0xc` @0x80039654 | reiner Doku-Fehler |

### Als KORREKT bestaetigt (nicht anzufassen)
* `RE15_PRI_MAX_MASKS_PER_CUT = 105` — exakt ausreichend, 0 Ueberlaeufe, `hdr[7] == max-Summe` in **240/240** Raeumen.
* Terminator-Test `== 0xFFFFFFFF` (`pri_common.c:48`) ↔ `bne v1,v0` @0x8003932c.
* `draw_count = declared & 0xFF` (`pri_common.c:147`) ↔ `sb t2` @0x80039358 / `lbu s4` @0x800395c0.
* Build-Schranke = Summe der Gruppen-Counts ↔ verschachtelte Schleifen @0x800393b8/@0x800393d8.
* Rechteck-Test `(mp[7] & 0xf0) == 0` und Quadrat-Kante `(mp[7]>>4)*8` ↔ @0x80039420/@0x80039440.
* Feldreihenfolge srcX/srcY/dstX/dstY/depth/size ↔ @0x80039408/18/00/14/f0/04.
* Gruppen-Anker `+4/+6`, 16-Bit-trunkierende Addition ↔ @0x800393fc/@0x80039410/@0x800394d0.
* `group_n[i] == 0`-Behandlung ↔ `beq` @0x800393c0.
* `MASK_OT_BUCKET = depth`, `CHAR_OT_BUCKET = otz>>4`, `OTZ_NEAR_DROP = 64`, `mask_camera_z = depth*64`.

---

## 6. Offene Punkte (bewusst NICHT BELEGT)

1. **Was `LAB_800151a0` ist** (die Sequenz, die `DAT_800aca38 |= 0x00100000` @0x80015230 setzt) und
   **ob/wo das Bit je geloescht wird** — im Dump kein `lui …,0xffef` gefunden.
2. **Gleichstandsrichtung Maske vs. Schatten**: Einfuegepunkt von `FUN_8001b064` (Aufrufer
   `@0x80031dcc`) relativ zu `FUN_80039590` (@0x8001ce54) im Frame nicht bis zum Treiber verfolgt.
3. **Wer die Bild-VRAM-Koordinate des Atlas setzt.** Der SLD-TIM traegt im Kopf Bild-VRAM `(0,0)`,
   die TPage `0x95` @0x80039630 zeigt aber auf `(320,256)`. Die CLUT-Koordinate `(0,480)` im
   TIM-Kopf stimmt dagegen exakt mit `clut = 0x7800` @0x80039498 ueberein. Die Bild-Ablage-Stelle
   habe ich nicht disassembliert.
4. **Ob `nSprite` (RDT-Byte 0) im Port irgendwo als Sprite-Zahl benutzt wird** — im Original wird
   dieses Byte bei jedem Cut-Wechsel mit dem pri-Draw-Count ueberschrieben (`sb t2,0x0(a0)`
   @0x80039358 / `sb zero,0x0(a0)` @0x80039338), ist also kein stabiler Zaehler.
   `rdt_common.c:228` liest es als `out->nSprite`; die Verwendung habe ich nicht geprueft.

---

# Gegenpruefung

Gegenpruefer, 2026-09-03. Auftrag: alle 20 Behauptungen widerlegen. Jede zitierte Adresse wurde in
ghidra1_V2.txt geoeffnet, jede Port-Zeile in der Datei nachgezaehlt, jede Datenzahl mit einem
EIGENEN Python-Sweep ueber die Original-Bytes neu erhoben (Zahlen des Berichts NICHT uebernommen).

## Ergebnis: keine der 20 Behauptungen faellt in der SACHE. Sechs Beleg-Zeilen sind ungenau.

### Was NICHT standhaelt

**W1 - Behauptung 2, Abdeckung: "in 240/240 Raeumen ... 0 Abweichungen" ist falsch beziffert.**
Von den 240 `STAGE*/ROOM*.RDT` sind **34 reine 4-Byte-Stummel** (ROOM1270/1271, 20C0..20F1,
30F0/30F1, 4060/4061, 40C0..40F1, 5150..5171, 6050..6071) - sie haben weder `hdr[7]` noch einen
Kamera-Record und wurden von der Messung nie beruehrt. Die belegbare Aussage lautet **206/206**.
Gemessen (eigener Sweep; beide Baeume `re15_port/shared_assets/PSX` und `info/Re1.5/PSX` sind
md5-identisch, 240/240 Dateien): 2188 Cuts, 480 Maskensaetze, max(hdr[7]) = 105,
max(Gruppen-Summe) = 105, 0 Abweichungen zwischen hdr[7] und max-ueber-Cuts(Summe).
Die SACHE (105 reicht exakt, kein Eintrag zu gross) bleibt bestaetigt.

**W2 - Behauptung 13, die Kennzahl misst etwas anderes als der Satz sagt.**
"2080 von 16654 Masken (12,5 %) samplen **unterhalb** Zeile 128" ist so falsch. Eigene Messung ueber
alle 16654 gebauten Masken:
- `srcY >= 128` (Maske beginnt unterhalb Zeile 128): **1192 = 7,16 %**
- `srcY + h > 128` (Sample-Rechteck reicht ueber Zeile 128 hinaus): **2080 = 12,49 %**

Die Zahl 2080 gehoert also zur zweiten Groesse. Der Defekt selbst (pri_psx.c laedt nur 128 von 256
Zeilen) bleibt bestaetigt, ebenso max(srcY+h) = 256 bei ROOM3070 Cut 4 Maske 38 (srcY=184, h=72)
und "alle 418 PRI##.TIM sind 256x256 / 66080 B".
Zusaetzlich: `RE15_PRI_VRAM_ROWS 128` steht auf **pri_psx.c:34**, nicht :33 (:33 ist
`RE15_PRI_VRAM_Y 256`).

**W3 - Behauptung 9, Instruktion um eins verschoben.**
Zitiert: "HUD/Text bei Index 0. BELEG: @0x80049258 sll v0,v0,0xc + **@0x8004925c addu a0,v0,a0**".
An 0x8004925c steht `jal AddPrim`; das `addu a0,v0,a0` steht im Delay-Slot **@0x80049260**.
Der Sachverhalt (OT-Index 0, Basis a0 = 0x800aca34 - 0x235c = 0x800AA6D8 @0x80049254) stimmt.

**W4 - Behauptung 6, Port-Zeile falsch zitiert und eine zweite Fundstelle uebersehen.**
Der falsche Text "baseRaw=0x7800 (TPage)" steht in pri_common.c auf **Zeile 13**, nicht 14-15.
Uebersehen wurde die ZWEITE Fundstelle **pri_common.c:62**
`/* baseRaw at +2 - TPage, unused for our re-blit */`. Wer nur die Kopfkommentare korrigiert,
laesst die falsche Beschriftung im Code stehen.

**W5 - Behauptung 7/8/14, drei Zeilennummern zeigen auf die Nachbarzeile.**
- "bg_pc.c:80 prueft > 65536" - der Vergleich `tim.width * tim.height > (int)(sizeof s_pri_atlas_rgba / ...)`
  steht auf **:79**; :80 ist der Ablehnungs-Rumpf.
- "main.c:4453 if (has_fg && pri_n > 0 ...)" - die Bedingung steht auf **:4455**; :4453 ist Kommentar.
- "render_pc.c ... vor Step 2 ab :745" - der "Step 2"-Kommentar beginnt auf **:733**, der Sortier-/
  Merge-Block auf **:746**, der Tri-Flush auf **:814**. :745 ist die letzte Kommentarzeile.

Alle drei Sachaussagen bleiben unberuehrt.

**W6 - Behauptung 19, der negative Beleg kann nicht belegen, was er soll.**
"Suche nach einer Loeschmaske lui ...,0xffef im gesamten ghidra1_V2.txt: 0 Treffer" - die 0 Treffer
habe ich bestaetigt, aber ein solcher Grep ist **strukturell blind** fuer eine Maske, die in einem
REGISTER ankommt. Gegenbeispiel im selben Umfeld: @0x80015810 `lui a1,0xfbff` + @0x80015814
`ori a1,a1,0xffff` baut 0xFBFFFFFF, und @0x8001582c `and v0,v0,a1` loescht damit Bit 0x04000000
aus DAT_800aca38 - genau die Form, die der Grep nicht findet. Ich habe alle 19 sw-Schreibstellen
auf DAT_800aca38 geoeffnet (0x80015240, 0x80015254, 0x80015838, 0x80016218, 0x8001ca2c, 0x8001ca50,
0x8001cafc, 0x8001cb20, 0x8001cb94, 0x8001cbf4, 0x8001cd60, 0x8001d9b8, 0x80020c48, 0x80020f30,
0x80021c5c, 0x80021f54, 0x800305fc, 0x80030628, 0x80037214) und ebenfalls **keinen Loescher von
Bit 0x00100000** gefunden - die Schlussfolgerung "nicht bestimmt" bleibt also richtig, die
BEGRUENDUNG traegt sie aber nicht.

### Was ich unabhaengig BESTAETIGT habe

Disassembly (jede Adresse geoeffnet):

- **B1** FUN_800392d4: @0x80039314 `sll v0,v0,0x5` (Cut*0x20), @0x8003931c `lw t5,0x1c(v0)`,
  @0x80039324 `lw v1,0x0(t5)`, @0x80039328 `li v0,-0x1`, @0x8003932c `bne` - Terminator ist
  ausschliesslich das volle u32. pri_common.c:48 korrekt. (Beh. 1)
- **B2** Arena FUN_80039270: @0x8003928c `lbu v0,0x7(a0)` + `sll ,0x2` (Flags),
  @0x800392ac / @0x800392c4 `sll ,0x5` (zwei Prim-Puffer) - Dimensionierung allein aus hdr[7]. (Beh. 2)
- **B3** Keine Wache im Original: @0x80039358 `sb t2,0x0(a0)` laeuft vor jedem Abbruch, Abbruch erst
  @0x800393a4 `sltu v0,v0,t8` / @0x800393a8 `beq`. 8-Bit-Zaehler @0x8003950c `andi v0,t2,0xff`,
  @0x8003954c `andi v0,a3,0xff`. (Beh. 3)
- **B4** draw_count: @0x80039330 `srl t2,v1,0x10`, @0x80039358 `sb t2,0x0(a0)` (BYTE!),
  Emitter @0x800395c0 `lbu s4,0x0(v0)`, @0x8003966c `sltu v0,s2,s4`. (Beh. 4)
- **B5** Null-Gruppe: @0x800393b8 `lhu v0,0x0(t5)`, @0x800393c0 `beq v0,zero,LAB_80039540`,
  @0x80039540 `addiu t7,t7,0x8`, @0x80039544 `addiu a3,a3,0x1`, @0x80039558 `addiu t5,t5,0x8` -
  der Maskenzeiger a2 wandert NICHT mit. Port pri_common.c:117-120 aequivalent. (Beh. 5)
- **B6** Gruppe+2 wird nie gelesen. Gelesen werden nur t5+0 (@0x800393b8), t7-2 = t5+4
  (@0x800393fc) und t7+0 = t5+6 (@0x80039410); t7 = t5+6 @0x800393b4. Das Literal
  @0x80039498 `ori v0,zero,0x7800` geht per @0x80039500 `sh v0,0x0(s0)` mit s0 = t6+0x1a
  = SPRT+0x0e = **clut** (0x7800 -> y=480, x=0). TPage separat @0x80039630 `ori a3,zero,0x95`
  + @0x80039634 `jal SetDrawMode` (tx=5 -> 320, ty=1 -> 256, tp=1 -> 8 bpp). Komplette
  SPRT-Belegung nachgerechnet: r/g/b=0x80 @0x800394ac/b0/b4, x @0x800394d0, y @0x800394cc,
  u @0x800394dc, v @0x800394e0, w @0x800394ec, h @0x800394f0. (Beh. 6)
- **B7** OT: @0x800395dc/e0 s5=0x800ACA34, @0x800395e4 `addiu s6,s5,-0x235c` = **0x800AA6D8**;
  @0x80039650 `lh a0,0x2(s3)`, @0x80039658 `sll a0,a0,0x2`, @0x8003965c `addu a0,a0,s6`,
  @0x80039654 `sll v0,v0,0xc` (Frame-Flip). OT-Groesse @0x80020c64 `ori a1,zero,0x400`
  + @0x80020c6c `jal ClearOTagR`; @0x800215c8 `addiu v0,s0,-0x1360` mit s0=0x800aca34
  = **0x800AB6D4 = ot[1023]** + @0x800215d0 `jal DrawOTag`. Char @0x80025648 `stotz`,
  @0x80025654 `sra v0,v1,0x6` + @0x80025658 `beq` (Near-Gate), @0x8002565c `sra v1,v1,0x4`.
  Schatten @0x8001b2c8 `jal RotAverage4`, @0x8001b2d8 `srl v0,v0,0x4`, @0x8001b2e0 `sll v0,v0,0x2`.
  ESP @0x8005356c `gte_stsz`, @0x80053578 `sra v0,a0,0x2` + @0x8005357c `beq`,
  @0x80053620 `sra v0,a0,0x6`. (Beh. 9)
- **B8** GTE: @0x80066c70 `li t0,0x155` (341) + @0x80066c74 `gte_ldZSF3`;
  @0x80066c7c `li t0,0x100` (256) + @0x80066c80 `gte_ldZSF4`. Nachgerechnet:
  Dreieck otz>>4 = (1023*vz)>>16 >= depth  <=>  vz >= depth*65536/1023 = depth*64,0625;
  Viereck/ESP vz>>6 >= depth  <=>  vz >= depth*64 - exakt. 64,0625/64 = +0,098 %.
  Der Gleichstand ist dabei korrekt EINGERECHNET (die Maske gewinnt ihn im Original, s. B12);
  ohne den Gleichstand waere die Schwelle (depth+1)*64. (Beh. 10)
- **B9** Gate: Parse @0x80021c14 `lw v0,DAT_800aca38`, @0x80021c18 `lui v1,0x10`,
  @0x80021c1c `and`, @0x80021c20 `bne ...,LAB_80021c34`, @0x80021c28 `jal FUN_800392d4`.
  Emit @0x8001ce40/44/48/4c + @0x8001ce54 `jal FUN_80039590`. Setzer @0x800151cc `slti v0,v0,0x1b`,
  @0x800151d8 `li a1,-0x2556` + @0x800151e0 `jal FUN_800217b0`, @0x80015214 `lui v1,0x10`,
  @0x80015220 `lui a0,0x400`, @0x80015228 `lw`, @0x80015230 `or v0,v0,v1`, @0x8001523c `or v0,v0,a0`,
  @0x80015240 `sw`, @0x80015248 `ori v1,v1,0x40`. Der Port kennt das Gate nicht (grep ueber
  engine/platform/include: kein Treffer fuer 0x00100000 im pri-Pfad). (Beh. 11)
- **B10** Rechteck/Quadrat: @0x80039404 `lhu a0,-0x4(s2)`, @0x80039420 `andi v0,a0,0xf000`,
  @0x80039424 `bne`, @0x80039428 `srl v0,a0,0xc`, @0x80039440 `sll s3,v0,0x3`,
  Rechteck-Record 12 B @0x80039434, Quadrat-Record 8 B @0x80039448. w = a2+8 (-> SPRT w @0x800394ec),
  h = a2+0xa (-> SPRT h @0x800394f0), passt zum Port. (Beh. 15)
- **B11** Sprite opak: @0x8003961c `jal SetSprt` (Code 0x64, ABE-Bit 0x02 nicht gesetzt),
  @0x8003947c / @0x8003948c `jal SetShadeTex(...,1)`, r=g=b=0x80 aus @0x800393b0 `ori t3,zero,0x80`. (Beh. 16)
- **B12** Reihenfolge im Frame: @0x8001ce54 (Masken) < @0x8001d09c `jal FUN_8001e8c8`
  (-> @0x8001ed04 `jal FUN_800254a0`, Charaktere) < @0x8001d1d0 `jal FUN_80052fd0` (ESP) -
  alle drei liegen in DERSELBEN Funktion FUN_8001c6e8 (letzter FUNCTION-Marker davor).
  Mit ClearOTagR-Reverse-OT + AddPrim-Prepend gewinnt der frueher eingehaengte Prim den Gleichstand.
  Der Port vergleicht mit `>=` (render_pc.c:793-794, :823) und verliert ihn. (Beh. 18)

Daten (eigener Sweep: 240 RDT-Dateien / 206 mit Header / 2188 Cuts / 480 Maskensaetze / 2750 Gruppen
/ 16654 gebaute Masken):

- pri_offset: **0x** literal 0xFFFFFFFF, **0x** ==0, **0x** jenseits EOF, **1708** zeigen auf FFFFFFFF,
  **480** echte Maskensaetze. (Beh. 17 bestaetigt, inkl. der uint32-Umlauf-Analyse zu pri_common.c:36:
  `offset+4` ist uint32-Arithmetik, 0xFFFFFFFF+4 = 3, erst danach Konversion nach size_t.)
- group_count==0: 0x, declared==0: 0x, >256: 0x; max group_count = 34, max declared = 105. (Beh. 3)
- max hdr[7] = 105, max Gruppen-Summe = 105, 0 Abweichungen. ROOM3000 Byte 0x00-0x07 =
  `00 04 00 00 00 00 00 69`, Byte 0xdcc = `08 00 69 00`, Gruppen 26+23+9+8+1+14+18+6 = 105. (Beh. 2)
- ROOM1210/ROOM1211 Byte 0x00-0x07 = `00 09 01 00 00 00 00 4d` (hdr7=77), Byte 0x99c = `22 00 4b 00`
  (gc=34, declared=75), Gruppensumme 77, group_n[32] = 0. Es sind ueber ALLE 480 Maskensaetze die
  **einzigen zwei** Cuts mit (declared & 0xFF) != Summe; **kein** Cut hat declared > Summe. (Beh. 4/5)
- Gruppe+2-Histogramm: 0x7800 -> 2712, 0x7C00 -> 26, 0x7C10 -> 8, 0x8080 -> 2, 0x8088 -> 2;
  **38 von 2750** nicht 0x7800. (Beh. 6)
- depth: 0 -> **272x**, 1023 -> **202x**, Max **1023**, kein Wert >= 0x8000. (Beh. 12)
- Groessen-Nibble: 0 -> **14346**, 1 -> 706, 2 -> 1006, 3 -> 496, 4 -> 80, 5 -> 16, 7 -> 4; nie 8..15. (Beh. 15)
- **418** PRI##.TIM, alle 66080 B / 256x256 / 8 bpp, alle mit clut[0] == 0x0000 und **genau einem**
  Null-Eintrag (Histogramm {1: 418}). (Beh. 13/16)
- Atlas-Abdeckung: **292** Maskensaetze mit Datei, **188 ohne = 39,2 %**, verteilt auf exakt die
  24 Raum-Varianten ROOM1020/1021/1030/1031/1040/1041/1070/1071 und
  ROOM5030/5031/5040/5041/5060/5061/50A0/50A1/50C0/50C1/5110/5111/5120/5121/5140/5141. (Beh. 8)
- ROOM1020/BG00.BSS SLD-Block @Byte **31672**, entpackt **61984** B, erste 16 Byte
  `10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e0 01`; CLUT-Block 524 B, VRAM (0,480), 256x1,
  clut[0]=0x0000 (genau 1 Null); IMG-Block 61452 B, VRAM (0,0), data_w=128 -> 256 px, h=240;
  8+524+61452 = 61984. 256*240 = 61440 <= 65536 -> bg_pc.c:78-79 akzeptiert, tim_common.c:79
  `width = data_w*2`, :83 `height = data_h`. Zusatzpruefung zu "AUSSCHLIESSLICH": in ROOM1020/1030/
  1040/1070 ist max(srcY+h) = 160 / 224 / 184 / 136, also **alles innerhalb der 240 Zeilen** - eine
  256x240-Extraktion wuerde jede Maske dieser Raeume bedienen; der Vorwurf "liegt ausschliesslich an
  der fehlenden Datei" haelt damit auch gegen den naheliegenden Einwand (240 != 256 Zeilen).
- Beide Asset-Baeume identisch: re15_port/shared_assets/PSX vs info/Re1.5/PSX, 240/240 RDTs
  md5-gleich, 0 Unterschiede.

Werkzeug dieser Gegenpruefung: neu geschriebene Sweeps (verify.py / v2.py / v3.py / v4.py im
Scratchpad), NICHT f_sweep.py uebernommen; sld.py nur als Codec fuer den ROOM1020-Block benutzt.
