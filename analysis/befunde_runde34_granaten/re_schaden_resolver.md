# Runde 34 / RE — Flächenschaden der Explosion, Spieler-Eigenschaden, Port-Resolver-Abgleich

Stand: 2026-09-29, abgeschlossen (RE-Phase, keine Port-Änderung). Zusammenfassung: §9 und die drei Schlussabschnitte.
Werkzeuge: `re_schaden_werkzeug/`, Laufzeit-Ausgaben `build/r34g_schaden/` (untracked).
Quellen: RE1.5 `info/Re1.5/PSX.EXE` (t_addr 0x80010000) per `re15_disasm.py`; DEBUG.BIN
(resident @0x800C0000) per `re_schaden_werkzeug/dis_bin.py` (re15_disasm.py kann 0x800Cxxxx
nicht, es bildet die Adresse auf die EXE ab und bricht ab); RE2 per `re2_disasm.py`.
Volle Disasm-Mitschnitte: `build/r34g_schaden/dis_*.txt`.

## Gliederung

1. Aufruf in Routine 31 @0x800185b8 — Argumente a0/a1/a2
2. FUN_80012d60 — Schleife, Abstandstest FUN_8002b5d0, Gate A (Selbstausschluss), Gate B (+0x90)
3. Tabellen DAT_8006f418 (Schaden) / DAT_8006f430 (Reaktion) — Art 2 (3/4)
4. Spieler-Zweig — HP, Blutung, Zustand 2, Tod, Spieler-FSM bei Explosion
5. +0x93-Einmal-Sperre — Löschstellen, Aufrufzahl in Routine 31
6. Sprengstoff-Tester 0x800128A0 / FUN_80011f50 mit Waffe 9..11 — erreichbar?
7. Port-Abgleich re15_resolve_attack & Co.
8. RE1.5-KI liest den Treffer (Zombie-Familie, STAGE1 FUN_80101224)
9. Einordnung fertig/unfertig je Mechanismus

---

## 1. Aufruf in Routine 31 — `jal 0x80012d60` @0x800185b8

`re15_disasm.py dis 0x8001854c 120` (Mitschnitt `build/r34g_schaden/`, hier der Kern):

```
8001854c: lui a1,0x800b / 80018550: lw a1,21188(a1)   ; a1 = *(0x800b52c4) = AKTUELLER ESP-SLOT
80018560: lhu v1,30(a1)                               ; Zuender slot+0x1e
80018568: beq v1,zero,0x80018688                      ; 0 -> Ende (Platz frei)
8001856c: ori v0,zero,0x7
80018570: bne v1,v0,0x800185f4                        ; nur bei Zuender == 7 weiter:
80018574: ori v0,zero,0x1
8001857c: sb v0,21336(at)                             ; 0x800b5358 := 1 (Laerm-Latch)
80018580: ori v0,zero,0x61 / 80018584: sb v0,108(a1)  ; slot+0x6c := 0x61
8001858c: lw v1,21188(v1)                             ; v1 = Slot
80018594: lh v0,40(v1)                                ; slot+0x28 (s16 Welt-X)
80018598: ori a0,zero,0x1f4                           ; a0 = 500
8001859c: sw v0,16(sp)                                ; P.x = (s32)slot+0x28
800185a0: lh v0,42(v1)                                ; slot+0x2a (s16 Welt-Y)
800185a4: addiu a1,sp,16                              ; a1 = &P (sp+16)
800185a8: addiu v0,v0,-500                            ; Y - 500
800185ac: sw v0,20(sp)                                ; P.y = (s32)slot+0x2a - 500
800185b0: lh v0,44(v1)                                ; slot+0x2c (s16 Welt-Z)
800185b4: ori a2,zero,0x2                             ; a2 = 2 (Angriffsart)
800185b8: jal 0x80012d60
800185bc: sw v0,24(sp)                                ; P.z = (s32)slot+0x2c  (Delay-Slot)
```

**Argumente:** `FUN_80012d60(a0 = 500, a1 = &P, a2 = 2)` mit
`P = { (s32)(s16)slot+0x28, (s32)(s16)slot+0x2a − 500, (s32)(s16)slot+0x2c }` (drei s32 auf dem
Stack sp+16/20/24; FUN_80012d60 liest davon `lw 0(a1)`, `lw 4(a1)`, `lw 8(a1)`).

**Was slot+0x28/+0x2a/+0x2c ist:** die Welt-Lage des ESP-Slots (s16), jeden Tick in der
Slot-Schleife FUN_80019e20 neu gerechnet: `sh v0,40(a0)` @0x8001a0e8 / `sh v0,42(a0)` @0x8001a100
bzw. @0x8001a1fc/0x8001a210/0x8001a220 und @0x8001a270/0x8001a28c/0x8001a2a4 (Matrix-Translation
slot+0x60/+0x64/+0x68 + gedrehte lokale Lage). Also: **Mitte = Granatenlage, 500 nach OBEN
versetzt** (Y negativ = oben; Spieler-Hitbox-Mitte liegt bei y−1530, §2.2).

**Worauf DAT_800b52c4 zeigt:** einzige Schreiber im ganzen EXE (`grep 800b52c4 ghidra1_V2.txt`,
146 XREFs, 4 davon W) sind in der ESP-Slot-Schleife FUN_80019e20:

```
80019e48: lui s0,0x800a / 80019e4c: addiu s0,s0,29624   ; s0 = 0x800a73b8 (Slot-Pool)
80019e50: addiu s2,s0,12672                             ; Ende = Pool + 0x3180 (0x60 Slots x 0x84)
80019e58: sw s0,21188(at)                               ; 0x800b52c4 := Slot 0
80019e70: lbu v0,108(v1) / 80019e78: andi v0,v0,0x1     ; Slot aktiv?
80019e84: lhu v0,0(v1)                                  ; Routine A = slot+0x00
80019e90: addu v0,v0,s1 (s1 = 0x80071d40) / 80019e9c: jalr v0
80019eb0: addiu v0,v0,132 / 80019eb8: sw v0,21188(at)   ; naechster Slot (+0x84)
```

(weitere Schreiber @0x80019ecc und @0x8001a494 = zweiter Durchlauf derselben Schleife). Während
Routine 31 läuft, ist DAT_800b52c4 also **der Granaten-Slot selbst**. Das Werkzeug-Etikett
„attack_workstruct" in `re15_disasm.py` ist irreführend: DAT_800b52c4 ist der
**aktuelle ESP-Slot-Zeiger**.

**Wie oft wird der Resolver gerufen?** Genau **einmal je Granate**: nur der Zweig
`Zuender == 7` (@0x80018570) enthält den `jal 0x80012d60`. Zweig `== 2` (@0x80018608) ruft nur
die Kind-Effekte 0x03195000/0x030B5400 (`jal 0x800199d4` @0x80018640/0x80018660), Zweig `== 0`
(@0x80018568 → 0x80018688) nur 0x030B5800 und `sb zero,108(a1)` @0x800186b0 (Platz frei). Der
Zünder sinkt je Tick um 1 (`lhu v0,30(v1)` / `addiu v0,v0,-1` / `sh v0,30(v1)` @0x80018674-84).

---

## 2. FUN_80012d60 — vollständig per Disasm

Mitschnitt `build/r34g_schaden/dis_80012d60.txt` (`re15_disasm.py dis 0x80012d60 260`). Das
Decompilat `RE_15_Quellcode_V2/FUN_80012d60.c` wurde nur als Lesehilfe benutzt.

### 2.1 Aufbau

Register: `s3 = a0` (Radius-Parameter, hier 500), `s5 = a1` (Punkt P), `s7 = a2` (Art),
`s2` = Trefferliste-Zähler, `s4` = Rückgabe-Zähler, `s6 = sp+16` = Trefferliste (Zeiger).

**Schleife 1 — Kandidaten sammeln (alle aktiven Gegner):**
```
80012d68: lui s1,0x800b / 80012d6c: lbu s1,-13746(s1)   ; s1 = u8 @0x800aca4e (Zahl aktiver Gegner)
80012d8c: lui s0,0x800b / 80012d90: addiu s0,s0,-13268  ; s0 = 0x800acc2c (Gegner-Array)
80012da8: beq s1,zero,0x80012e00                        ; keine Gegner -> nur Spieler
80012db4: lw v0,0(s0) / 80012dbc: andi v0,v0,0x1        ; Wort 0 Bit 0 = aktiv
80012dc0: beq v0,zero,0x80012df4                        ; inaktiv: naechster Platz, Zaehler bleibt
80012dc8: addiu s1,s1,-1                                ; Zaehler nur fuer AKTIVE Plaetze
80012dcc: addu a1,s5,zero / 80012dd4: andi a2,s3,0xffff
80012dd0: jal 0x8002b5d0                                ; Abstandstest(e, P, 500)
80012dd8: beq v0,zero,0x80012df4
80012de0: addiu s2,s2,1 ... 80012df0: sw s0,0(v0)       ; Liste[s2++] = e
80012df8: bne v0,zero,0x80012db4 / 80012dfc: addiu s0,s0,500   ; Stride 0x1f4
```

**Spieler-Block — IMMER geprüft, danach:**
```
80012e00: lui s0,0x800b / 80012e04: addiu s0,s0,-13740  ; s0 = 0x800aca54 (Spieler)
80012e10: jal 0x8002b5d0                                ; Abstandstest(Spieler, P, 500)
80012e18: beq v0,zero,0x80012f08                        ; kein Treffer -> Gegner-Anwendung
80012e24: lbu v0,-13593(v0) / 80012e2c: andi v0,v0,0x1  ; Spieler+0x93 Bit 0
80012e30: bne v0,zero,0x80012f00                        ; gesperrt -> nur Zaehler s4++
   ... Spieler-Zweig, §4 ...
80012f00: addiu s4,s4,1
```

**Schleife 2 — Anwendung auf die Liste, rückwärts:**
```
80012f10: andi s0,s7,0xff                               ; Art
80012f14..20: s3 = 0x8006f418 + 2*Art                   ; Zeiger auf Schaden
80012f24: addiu s2,s2,-1                                ; letzter Index
80012f34: lw s1,16(v0)                                  ; e = Liste[s2]
80012f38: lui v1,0x800b / 80012f3c: lw v1,21188(v1)     ; v1 = *(0x800b52c4) = ESP-Slot
80012f40: lw v0,392(s1)                                 ; e+0x188
80012f44: lw v1,116(v1)                                 ; slot+0x74
80012f48: addiu v0,v0,64                                ; e+0x188 + 0x40
80012f4c: beq v0,v1,0x8001302c                          ; GATE A: gleich -> ueberspringen
80012f54: lw v0,144(s1) / 80012f58: lui v1,0x300        ; e+0x90, Maske 0x03000000
80012f5c: and v0,v0,v1
80012f60: beq v0,v1,0x8001302c                          ; GATE B: beide Bits -> ueberspringen
80012f68: sltiu v0,s0,0x2 / 80012f6c: beq v0,zero,...   ; Art < 2 ?
80012f74: jal 0x800453d0 / 80012f78: ori a0,zero,0xa    ;   ja: SE 10 (nicht bei Art 2!)
80012f7c: lbu v0,147(s1) / 80012f84: andi v0,v0,0x1
80012f88: sb v0,147(s1)                                 ; e+0x93 &= 1 (Bits 1..7 loeschen)
80012f8c: lw a1,0(s5) / 80012f90: lw a2,8(s5)           ; P.x, P.z
80012f94: jal 0x8001a7a8                                ; Hinten-Test(e, P.x, P.z)
80012f9c: beq v0,zero,0x80012fb4
80012fac: ori v0,v0,0x80 / 80012fb0: sb v0,147(s1)      ;   1 -> e+0x93 |= 0x80
80012fb4: lbu v1,147(s1) / 80012fbc: andi v0,v1,0x1
80012fc0: beq v0,zero,0x80012fd0
80012fc4: ori v0,v1,0x2                                 ; (Delay-Slot)
80012fc8: j 0x80013024 / 80012fcc: sb v0,147(s1)        ; gesperrt: e+0x93 |= 2, KEIN Schaden
80012fd0: ori v0,zero,0x1
80012fd4: sb zero,7(s1)                                 ; e+0x07 := 0
80012fd8: sb v0,6(s1)                                   ; e+0x06 := 1
80012fdc..e8: lbu v0,0(0x8006f430 + Art)                ; Reaktion
80012ff0: sb v0,5(s1)                                   ; e+0x05 := DAT_8006f430[Art]
80012ff4: lhu a0,0(s3)                                  ; Schaden
80012ffc: subu v1,v1,a0 / 80013000: sh v1,154(s1)       ; e+0x9a (HP) -= Schaden
80013008: ori v0,v0,0x1 / 8001300c: sb v0,147(s1)       ; e+0x93 |= 1
80013014: bgez v1,0x80013024 / 80013018: sb v0(=2),4(s1); e+0x04 := 2
8001301c: ori v0,zero,0x3 / 80013020: sb v0,4(s1)       ; HP < 0 (signiert) -> e+0x04 := 3
80013024: addiu s4,s4,1                                 ; auch gesperrte zaehlen
8001302c: bne v0,zero,0x80012f28 / 80013030: addiu s2,s2,-1
8001303c..64: return s4 & 0xff
```

Wichtig für die Granate: **Art 2 ist ≥ 2 → kein SE 10 und keine Blutungs-Würfel** (weder beim
Gegner @0x80012f68 noch beim Spieler @0x80012e58/e68).

### 2.2 Abstandstest FUN_8002b5d0(e, P, r) — wie kombiniert sich 500 mit der Ziel-Hitbox

Mitschnitt `build/r34g_schaden/dis_8002b5d0.txt`.

```
8002b608: lw s2,120(s3)          ; H = *(e+0x78)  Hitbox-Masse (u16 +6 r1, +8 h, +10 r2)
8002b60c: lw s5,124(s3)          ; O = *(e+0x7c)  Mitten-Versatz (s16 x,y,z)
8002b610: lhu v1,6(s2) / 8002b614: lhu v0,10(s2) / 8002b618: lhu a3,6(s2)
8002b61c: beq v1,v0,0x8002b6fc   ; r1 == r2 -> Kreis, a3 = r1
   (sonst Sektor: Winkel FUN_80065de0(dz,dx) - e+0x6a, auf 0..0x400 gefaltet
    @8002b650-698, r = r1 + (r2-r1)*FUN_800683e8(w)>>12 bzw. r2 + (r1-r2)*FUN_80068348(w)>>12)
8002b6fc: andi v0,a3,0xffff
8002b700: andi s1,s7,0xffff      ; s1 = r (Parameter, 500)
8002b704: addu s0,v0,s1          ; R = r_ziel + 500
8002b708: sll a2,s0,1            ; 2R
8002b70c: lh v0,0(s5) / 8002b710: lw v1,52(s3) / 8002b718: addu v0,v0,v1   ; cx = e+0x34 + O.x
8002b71c: subu a1,a0,v0          ; dx = P.x - cx
8002b720: addu v0,a1,s0 / 8002b724: sltu v0,a2,v0 / 8002b728: bne -> 0   ; |dx| <= R
8002b730..4c: dz = P.z - (e+0x3c + O.z), gleiche Probe                  ; |dz| <= R
8002b754..64: jal 0x80065f60 (SquareRoot0) mit dx*dx + dz*dz
8002b76c: subu s0,s0,v0 / 8002b770: blez s0 -> 0                         ; Abstand < R (streng)
8002b778: lhu v0,8(s2)           ; h
8002b780: addu a2,s1,v0          ; Y-Band = 500 + h
8002b784: lh v0,2(s5) / 8002b77c: lw v1,56(s3) / 8002b78c: addu v0,v0,v1   ; cy = e+0x38 + O.y
8002b790: subu a0,a0,v0          ; dy = P.y - cy
8002b794: subu v0,zero,a2 / 8002b798: slt v0,v0,a0 / 8002b79c: beq -> 0  ; -(500+h) < dy
8002b7a0: slt v0,a0,a2 / 8002b7a4: beq -> 0                              ; dy < 500+h
8002b7ac: ori fp,zero,0x1        ; Treffer
```

**Antwort:** Der Radius-Parameter wird **auf den Ziel-Radius addiert**:
horizontal `sqrt(dx²+dz²) < r_ziel + 500` (vorher `|dx|,|dz| <= R`), vertikal
`|P.y − (e.y + O.y)| < 500 + h` (streng). Es gibt also KEIN „Abstand < 500".

**Spieler-Hitbox (Laufzeit belegt):** `re_schaden_werkzeug/ss_spieler_hitbox.py` über alle
sauberen `stage_saves/*.sav`: in allen 40+ Spiel-Saves ist Spieler+0x78 (0x800acacc) =
**0x80073e94** und +0x7c (0x800acad0) = **0x800b2354**, dort liegt **(0, −1530, 0)**. Die Zeiger
setzt FUN_800314b0 (`sw v1,-13620(at)` @0x80031664 mit v1 = `lw 0x80073ea0`, `addiu v1,v1,9044` /
`sw v1,-13616(at)` @0x8003166c-74). H @0x80073e94 = Bytes `00 00 06 fa 00 00 c2 01 fa 05 c2 01`
→ r1 = r2 = 450, h = 1530.

**Eingesetzt für die Granate** (Explosion auf dem Boden, Granate y = g, Spieler steht auf y = s):
Spieler getroffen, wenn horizontal < 450 + 500 = **950** und
`|(g − 500) − (s − 1530)| < 2030` ⇔ `−3060 < g − s < 1000`. Auf gleicher Ebene (g = s) also
immer. Ein Gegner mit Kreis-Hitbox r/h und Versatz (0,−h,0): horizontal < r + 500, vertikal
`|g − 500 − (y − h)| < 500 + h`.

### 2.3 Gate A — Selbstausschluss (@0x80012f38-4c)

Vergleich: `*(e+0x188) + 0x40 == *(DAT_800b52c4 + 0x74)`.
* DAT_800b52c4 = der laufende ESP-Slot (§1).
* slot+0x74 = der Matrix-Zeiger `a2` beim Spawn: FUN_80019700 `sw a2,116(t0)` @0x8001980c
  (mit Versatz-Tripel) bzw. @0x8001981c (ohne) — Mitschnitt `dis_80019700.txt`. Kein weiterer
  Schreiber auf slot+0x74 in den Routinen 29/30/31 (`dis 0x80018320`, `dis 0x8001843c`,
  `dis 0x8001854c`: kein `sw …,116(…)`).
* Der Granaten-Slot wird vom Spieler-FSM gespawnt (`jal 0x80019700` @0x800336bc-ec/
  0x8003371c-4c/0x80033778-a8, `waffen_fsm_2026-09-12/SPEC.md` §1.5) mit a2 = Waffenknochen-
  Matrix des SPIELERS `[DAT_800acbdc]+0x7a4`.
* e+0x188+0x40 = Teil-0-Matrix des GEGNER-Modells. Die ist nie die Spieler-Handmatrix.

→ **Gate A schließt bei der Explosion keinen Gegner aus. Die Explosion trifft jeden Gegner im
Radius.** (Gate A existiert für Effekte, die AN einem Gegner-Knochen gespawnt wurden — z.B. ein
Gegner-Angriffseffekt, der seinen eigenen Werfer nicht treffen soll.) Den Spieler prüft
Gate A ohnehin nicht (Spieler-Zweig @0x80012e00-f04 hat kein Gate A/B).

### 2.4 Gate B — `(e+0x90 & 0x03000000) == 0x03000000` (@0x80012f54-60) = **+0x93 Bits 0 UND 1**

`lw v0,144(s1)` liest das 32-bit-Wort e+0x90..e+0x93 (Little-Endian): Bits 24..31 sind das Byte
**e+0x93**. Die Maske 0x03000000 prüft also `(e+0x93 & 3) == 3` — „schon getroffen (Bit 0) UND
Nachtreffer markiert (Bit 1)". Live belegt: `re_schaden_werkzeug/ss_gegner_hp.py` zeigt für den
Typ-0x16-Zombie in ROOM1140 `+93=0x01 +90=0x01000400` (oberstes Byte des Worts = +0x93), bei
allen anderen Gegnern `+93=0x00 +90=0x00000x00`. Setzer der zwei Bits sind die bekannten
+0x93-Schreiber (Bit 0: @0x8001300c, FUN_80011f50 @0x800124f0-f8 usw.; Bit 1: @0x80012fcc und
FUN_80011f50 @0x80012410). FUN_80011f50 benutzt dieselbe Probe (`lui s4,0x300` @0x800120c0,
`lw v0,144(s0)` / `and v0,v0,s4` / `beq v0,s4` @0x800120f4-0x80012100).

**Die Port-Aussage „Gate B = Tod/Despawn-Flags, Schreiber nicht portiert → inert" ist falsch**
(`re15_damage.c:3334-3339`, ebenso `RE15_FUN_CATALOG.md` Z. 57). Das Feld ist im Port
`hit_react` und wird gepflegt. Folge der Auslassung siehe §7.

---

## 3. Tabellen DAT_8006f418 / DAT_8006f430

`re15_disasm.py read 0x8006f418 11 --w 2 --signed`, `read 0x8006f430 11 --w 1`,
`bytes 0x8006f418 40`:

```
8006f418: 0a 00 14 00 e8 03 e8 03 e8 03 32 00 64 00 c8 00
8006f428: 2c 01 e8 03 00 00 00 00 03 03 09 0a 0b 0e 0f 10
8006f438: 11 12 14 00
```

| Art | Schaden DAT_8006f418[Art] | Reaktion DAT_8006f430[Art] (Gegner +0x05) | Aufrufer mit dieser Art |
|---|---|---|---|
| 0 | 10 (@0x8006f418) | 3 (@0x8006f430) | FUN_80017fa4 @0x80018008 (`addu a2,zero,zero`) |
| 1 | 20 | 3 | keiner |
| **2** | **1000** (@0x8006f41c = `e8 03`) | **9** (@0x8006f432 = `09`) | **Routine 31 @0x800185b8 (Granate)** |
| 3 | 1000 (@0x8006f41e) | 10 (0x0a, @0x8006f433) | keiner |
| 4 | 1000 (@0x8006f420) | 11 (0x0b, @0x8006f434) | keiner |
| 5..9 | 50/100/200/300/1000 | 14/15/16/17/18 | keiner |
| 10 | 0 | 20 | keiner |

**Aufrufer-Zensus FUN_80012d60** (`re_schaden_werkzeug`-Scan über PSX.EXE + DEBUG.BIN +
STAGE1..6 + TITLE nach `jal 0x80012d60` und nach dem Datenwort 0x80012d60): **genau zwei** —
@0x80018008 (Art 0) und @0x800185b8 (Art 2). Kein Overlay, kein Funktionszeiger.
→ Die Arten 3 und 4 (je 1000, Reaktion 10/11) sind im Auslieferungsstand **ohne Aufrufer**. Dass
sie Säure/Brand sein sollten, ist durch nichts im Code belegt (Plan-Tabelle ohne Nutzer). Für
0x0A/0x0B gilt die Beta→Retail-Regel (RE2), siehe Schwester-Dossier `re_saeure_brand_*`.

Port: `re15_damage_table[11]` = {10,20,1000,1000,1000,50,100,200,300,1000,0} und
`re15_react_table[11]` = {3,3,9,10,11,14,15,16,17,18,20} (`re15_damage.c:41-58`) — **bytegleich**.

---

## 4. Spieler-Zweig — was eine Explosion mit dem Spieler macht

### 4.1 Resolver-Zweig (@0x80012e18-0x80012f00)

```
80012e24: lbu v0,-13593(v0) / 80012e2c: andi v0,v0,0x1 / 80012e30: bne v0,zero,0x80012f00  ; +0x93 Bit0 -> nichts
80012e38: andi a0,s7,0xff / 80012e3c: sll v1,a0,1
80012e44: lhu v0,-13586(v0)                  ; HP (0x800acaee)
80012e4c..54: lhu v1,0(0x8006f418 + 2*Art)   ; 1000 bei Art 2
80012e58: sltiu a0,a0,0x2                    ; Art < 2 ?
80012e5c: subu v0,v0,v1 / 80012e64: sh v0,-13586(at)   ; HP -= 1000
80012e68: beq a0,zero,0x80012ebc / 80012e6c: ori v0,zero,0x2   ; Art 2: direkt zu +0x04 := 2
   (nur Art<2: jal 0x800453d0(0xa) @0x80012e70; 2x jal 0x8001af20 @0x80012e78/80;
    beide &1 -> 0x800acaec |= 2 @0x80012ea0-b4)
80012ebc: sb v0,4(s1)                        ; Modus  +0x04 := 2
80012ec0: lw a1,0(s5) / 80012ec4: lw a2,8(s5)          ; P.x, P.z
80012ec8: jal 0x8001a7a8 / 80012ed0: addiu v0,v0,2
80012ed4: sb v0,5(s1)                        ; Zustand +0x05 := 2 + FUN_8001a7a8(Spieler,P.x,P.z)
80012ee0: sb zero,6(s1)                      ; +0x06 := 0
80012ee4: ori v0,v0,0x1 / 80012eec: sb v0,147(s1)       ; +0x93 |= 1 (Delay-Slot, immer)
80012edc: lh v1,154(s1) / 80012ee8: bgez v1,0x80012f00  ; HP >= 0 -> fertig
80012ef0: ori v0,zero,0x3 / 80012ef4: sb v0,4(s1)       ; HP < 0: Modus +0x04 := 3 (Tod)
80012ef8: sb zero,5(s1) / 80012efc: sb zero,6(s1)
```

**Explosion (Art 2): kein SE 10, keine Vergiftungs-Würfel, 1000 Schaden.** Spieler-HP startet
bei 100 (`ori v0,zero,0x64` @0x80031710 / `sh v0,-13586(at)` @0x80031718). Heilungen addieren
ungeklemmt (Port `item_use_common.c:19-23`, Belege @0x8004afa8-b0), HP > 999 ist also nur mit
≥ 36 Heilungen über Voll möglich. **Im Spielbetrieb tötet die eigene Explosion den Spieler
sofort** (Modus 3), wenn er in Reichweite (§2.2: waagrecht < 950) und nicht gesperrt ist.

Hinweis zur Benennung: Bit 0x2 von 0x800acaec ist **Vergiftung**, nicht Blutung — das
Gegengift-Spray / Blaues Kraut löscht es (`andi v0,v0,0xfffd` @0x8004af8c / @0x8004b010).
Für Art 2 ohne Belang.

### 4.2 FUN_8001a7a8 — Seiten-Test (Position, nicht Blickrichtung)

```
8001a7c4: lh a2,52(s0) / 8001a7c8: lh a3,60(s0)   ; e.x, e.z
8001a7cc: jal 0x8001a6d4                          ; w = Winkel des Vektors P -> e
8001a7d8: lh v1,106(s0) / 8001a7e0: subu v0,v0,v1 ; w - e+0x6a (Blickrichtung)
8001a7e4: addiu v0,v0,1024 / 8001a7e8: andi v0,v0,0xfff
8001a7ec: slti v0,v0,2048                         ; ((w - yaw + 0x400) & 0xfff) < 0x800
```
FUN_8001a6d4(sx,sz,dx,dz) rechnet den Winkel von (dx−sx, dz−sz) (`subu s0,a2,a0` @0x8001a6f0,
`subu a0,a3,a1` @0x8001a704). FUN_8001aac4 dreht mit demselben Winkel (Selbst → Ziel) auf ein Ziel
zu (@0x8001aaf8, `sh a0,106(v1)` @0x8001ab54); die Blickrichtung +0x6a und der Winkel sind also
dasselbe System. Ergebnis 1 ⇔ der Vektor „Angriffspunkt → Figur" zeigt in Blickrichtung ⇔ **der
Angriffspunkt liegt HINTER der Figur**. Das Etikett „front" im Port (`re15_damage.c:83-96`,
`hit_from_front`) ist vertauscht; die Formel ist aber dieselbe, das Ergebnis also bytegleich.
Belegt durch die Handler: Zustand 3 schiebt NACH VORN (`jal 0x800245d8` mit `addu a0,zero,zero`
@0x8003609c-a0), Zustand 2 NACH HINTEN (`ori a0,zero,0x800` @0x80035f18-1c) — jeweils weg vom
Angreifer.

### 4.3 Spieler-Modus 3 (Tod) — was danach läuft

Modus-Tabelle @0x80073f90 (`table 0x80073f90 8`): [2] = 0x80035AF0 (Treffer), **[3] = 0x800366BC
(Tod)**, [5] = 0x80036834 (Griff), [7] = 0x8003694C (Blutlache). Aufruf jeden Frame über
FUN_80031c44 (`lbu v1,-13736(v1)` @0x80031c8c, `jalr` @0x80031cb4). Todes-FSM 0x80036718
(Mitschnitt `dis_800366bc.txt`):

| Phase (aca5a) | Adresse | Inhalt |
|---|---|---|
| 0 | @0x80036764-c4 | aca5a := 1; Clip +0x94 := **7** (`ori v1,zero,0x7` @0x80036778, `sb v1,-13592(at)` @0x80036780); +0x95 := 0; +0x8f := 7; +0x8c := 0; +0x93 \|= 1 (@0x800367a4); **SE 0x04030001** an der Spielerlage (`lui a0,0x403` @0x80036744 / `ori a0,a0,0x1` @0x80036764 / `jal 0x80045024` @0x800367a8, a1 = 0x800aca88); aca3c \|= 0xc0; fällt in Phase 1 durch |
| 1 | @0x800367c8-fc | `jal 0x8001f314` mit Bank-Paar [0x800acad8]/[0x800acbc0] (PL00-Basisbank, NICHT Waffenbank), a2 = 0, a3 = 0x200; Clip-Ende → aca5a := 2 |
| 2 | @0x80036804-14 | `jal 0x80045630(2,0)` (Boden-Aufschlag-SE), **Modus := 7** (`sh v0,-13736(at)` @0x80036814) |
| jeder Tick | @0x8003681c | `jal 0x800369f8(0,1)` |

Es gibt **keinen eigenen Explosions-Clip und keinen Rückstoß** für den Spieler: der Explosionstod
ist der allgemeine Tod (Clip 7 der PL00-Basisbank), derselbe wie beim tödlichen Zombie-Treffer
aus dem Resolver.

### 4.4 Nicht-tödlicher Fall (nur HP > 999) — Modus 2, Zustand 2/3

Tabelle @0x800741a8 (`table 0x800741a8 6`): [2] 0x80035DE0, [3] 0x80035F64.

| Zustand | Adresse | Clip | SE | Schub (FUN_800245d8) | Ende |
|---|---|---|---|---|---|
| 2 (Punkt vorn) | 0x80035DE0 | 8 (`ori v0,zero,0x8` @0x80035e38) | 0x04010001 (`lui a0,0x401` @0x80035e0c) | a0 = 0x800 (rückwärts) @0x80035f18-1c | @0x80035edc: +0x93 &= 0xfe (@0x80035ee8/f0), Wort aca58 := 1 (`sw a0,-13736(at)` @0x80035f00) |
| 3 (Punkt hinten) | 0x80035F64 | 9 (`ori v0,zero,0x9` @0x80035fbc) | 0x04020001 (`lui a0,0x402` @0x80035f90) | a0 = 0 (vorwärts) @0x8003609c-a0 | @0x80036060: +0x93 &= 0xfe (@0x8003606c/74), aca58 := 1 (@0x80036084) |

Beide: Schubstärke acae0 := 0xc8 (200) (@0x80035e44 / @0x80035fc8), Abbau je Tick um acaf2 :=
0x32 (50) (@0x80035e70 / @0x80035ff4, Abzug @0x80035f3c / @0x800360c0, Klemme ≥ 0).

### 4.5 Reihenfolge im Hauptlauf

`dis 0x8001cdd0 24`: `jal 0x8001a50c` (Gegner-Schleife) @0x8001ce04 → `jal 0x80031c44`
(Spieler) @0x8001ce0c → … → `jal 0x80019e20` (ESP-Slots, also Routine 31) @0x8001ce2c. Die
Explosion schreibt Modus/Zustand also NACH Gegnern und Spieler; ausgewertet wird im Folgebild.

---

## 5. +0x93-Einmal-Sperre

### 5.1 Aufrufzahl

Routine 31 ruft FUN_80012d60 genau einmal pro Granate (§1). Pro Ziel gibt es also genau **ein**
Schadensbild — außer das Ziel ist schon gesperrt.

### 5.2 Spieler — Setzer / Löscher von 0x800acae7 Bit 0

`grep DAT_800acae7 ghidra1_V2.txt` (sb-Schreiber):
Jede Stelle einzeln disassembliert (die 3 Instruktionen davor):
* setzen (`ori v?,v?,0x1`): Resolver @0x80012eec; Treffer-Zustände 2/3 Phase 0 @0x80035e7c /
  @0x80036000; Tod Phase 0 @0x800367a4; Treffer-Zustände 0/1/4/5 @0x80035bfc, @0x80035d34,
  @0x80036180, @0x800364ec; im Bereich 0x80038xxx u.a. @0x80038180 (→ O3).
* Bit 0 löschen (`andi v0,v0,0xfe`): **Ende Treffer-Zustand 2** @0x80035ee8 / `sb` @0x80035ef0;
  **Ende Zustand 3** @0x8003606c / @0x80036074; Enden der Zustände 0/1 @0x80035c68-70 und
  @0x80035da0-a8; 0x80038248-58, 0x80038748-54 (→ O3).
* ganz löschen (`sb zero`): @0x80031964 (Modus-0-Init FUN_800318f8), @0x800362fc, @0x80036420,
  @0x80036690.

Live: in allen Griff-Saves (Modus 5) ist Spieler+0x93 = 0x01 (`ss_spieler_hitbox.py`:
mzd_death_cmd5_struggle, mzd_stage1_hit_effect, mzd_blood_decals_hp30), ebenso nach dem Tod
(Modus 7). **Ein Spieler im Treffer-Taumel, im Griff oder tot wird von der Explosion nicht
geschädigt** (@0x80012e30).

### 5.3 Gegner — Setzer / Löscher von e+0x93 Bit 0

* setzen: Resolver @0x8001300c; FUN_80011f50 @0x800124f0-f8; Zombie-Gore-Setup (Port
  `re15_damage.c:3100`, FUN_80106edc).
* löschen (RE1.5, Memory reai-v2-hit-latch-93): die Treffer-Reaktions-Handler beim Rücksprung
  nach ACTIVE, z.B. `andi v0,v0,0xfe` @0x80105fa4 (STAGE1), ebenso 0x80106b8c-90 /
  0x80106a1c-20 / 0x80103b5c-68; Hund `+0x93 = 0` @0x80110b70 / @0x80110d90.
* Der Resolver selbst löscht NUR Bits 1..7 (`andi v0,v0,0x1` / `sb` @0x80012f84-88), nie Bit 0.

Folge für die Granate: Ein Gegner, der noch in einer Treffer-Reaktion steckt (Bit 0 gesetzt,
z.B. gerade angeschossen), bekommt **keinen Explosionsschaden**; er bekommt nur Bit 1
(@0x80012fcc, Gore-Auslöser) und Bit 7 nach der Seite. Hat er Bit 0 und 1 schon, greift Gate B
und er wird ganz übersprungen.

---

## 6. Sprengstoff-Tester 0x800128A0 und FUN_80011f50 mit Waffe 9..11 — NICHT erreichbar

### 6.1 Tester-Tabelle @0x8006E548 (`re15_disasm.py table 0x8006e548 22`)

| Index (Waffen-Id) | Tester |
|---|---|
| 0, 3..8, 12, 13, 19, 21 | 0x80012574 |
| 1, 2 | 0x800127fc |
| **9, 10, 11**, 14, 15, 16, 17, 18, 20 | **0x800128A0** |

FUN_800128A0(a0 = Reichweite, a1 = e, a2 = Punkt s16):
```
800128ac: lh v1,0(a2) / 800128b0: lw v0,52(a1) / 800128b8: subu   ; dx = e+0x34 - P.x
800128c0: lh v1,4(a2) / 800128c4: lw v0,60(a1) / 800128cc: subu   ; dz = e+0x3c - P.z
800128d4: lw v0,120(a1) / 800128dc: lhu s0,6(v0)                    ; r1 der Ziel-Hitbox
800128e4: addu s0,s0,a0                                             ; r1 + Reichweite
800128ec: jal 0x80065f60                                            ; SquareRoot0(dx²+dz²)
800128f8: sltu s0,v1,s0 / 800128fc: beq -> 0                        ; Abstand < r1 + Reichweite
80012904..20: Abstand < *(0x8008f5e0) -> 0x8008f5e0 := Abstand, return 1   ; naechstes Ziel
```
Ein reiner Kreis-Test „nächstes Ziel in Reichweite" (keine Höhe).

### 6.2 Wer ruft FUN_80011f50 — mit welcher Id?

Scan nach `jal 0x80011f50` (PSX.EXE + DEBUG.BIN + alle Overlays) — **11 Stellen**, alle mit
`a0 = lbu 0x800aca5d` (ausgerüstete Waffe) bzw. rekursiv mit demselben a0:

| Stelle | Handler | erreichbar für Id |
|---|---|---|
| @0x80012418 | FUN_80011f50 selbst (Wiederholung, `addu a0,s0,zero`, s0 = `andi fp,0xff`) | wie Aufrufer |
| @0x80033554 | Standard-Sub 2, Schrot-Zusatz | nur 8: `lbu v1,-13731(v1)` / `ori v0,zero,0x8` / `bne v1,v0,0x8003355c` @0x8003350c-14 |
| @0x80033880 | Entlade 0x800337BC | 3, 4 (`table 0x80074100`: [3],[4]) |
| @0x8003396c | Entlade 0x800338A8 | 5, 6 |
| @0x80033a34 | Entlade 0x800339A4 | 7 |
| @0x80033b10 | Entlade 0x80033A58 | 8 |
| @0x80033c50 | Entlade 0x80033B98 | 13 |
| @0x800349ec | Entlade 0x800347F8 (Dauerfeuer) | 12 |
| @0x80034c24 | Entlade 0x80034A30 (Dauerfeuer) | 19 |
| @0x800353cc | Nahkampf-SLASH | 0..2 (`table 0x80074030`: [0..2] = 0x80034E70) |
| DEBUG.BIN @0x800c47a4 | Entlade 0x800C45A8 (Flammenwerfer) | 14 (`lbu a0,-13731(a0)` @0x800c479c) |

Die Entlade-Einträge für 9/10/11 (`table 0x80074100`: [9] 0x80033B38, [10] 0x80033B58,
[11] 0x80033B78) sind je 8 Instruktionen und rufen NUR den Munitionsabzug:
```
80033b38: addiu sp,sp,-24 / 80033b3c: sw ra,16(sp) / 80033b40: jal 0x8004eae4 / 80033b44: nop
80033b48: lw ra,16(sp) / 80033b4c: addiu sp,sp,24 / 80033b50: jr ra / 80033b54: nop
(0x80033b58.. und 0x80033b78.. identisch, jal 0x8004eae4 @0x80033b60 / @0x80033b80)
```
Die Waffen-Dispatch-Tabelle schickt 9/10/11 in den Standard-FSM (`table 0x80074030`:
[9],[10],[11] = 0x80032E9C), nicht in Nahkampf oder Dauerfeuer.

**Ergebnis:** FUN_80011f50 wird im Auslieferungsstand **nie mit 9, 10 oder 11** gerufen. Die
Tester-Einträge [9]/[10]/[11] sind tot. Lebendig ist 0x800128A0 nur über [14] (Flammenwerfer,
DEBUG.BIN @0x800c47a4). Der einzige Schaden einer Handgranate im Original ist
`FUN_80012d60(500, P, 2)` aus Routine 31. **Die Port-Brücke `ENT[9].resolve = 1`
(Sofort-Treffer beim Abzug über `re15_player_weapon_fire(9)`) hat kein Original-Gegenstück und
muss wegfallen** (resolve = 0 für 9/10/11, wie SPEC Schritt 1 schon fordert).

---

---

## 7. Port-Abgleich im Detail (Resolver-Kette)

### 7.1 Was bytegleich ist

| Port | Original | Befund |
|---|---|---|
| `re15_damage_table` / `re15_react_table` (`re15_damage.c:41-58`) | DAT_8006f418 / DAT_8006f430 (§3) | bytegleich |
| `re15_hitbox_overlap` (`re15_damage.c:3243-3264`) | FUN_8002b5d0 @0x8002b6fc-0x8002b7ac (§2.2) | gleich: R = r_ziel + a0, `(u32)(d+R) > (u32)2R`, `R - dist <= 0`, `-(a0+h) < dy < a0+h` |
| `re15_hitbox_test` Sektor über `re15_ellipse_radius` (`re15_math.c:276-286`) | @0x8002b624-0x8002b6f8 | gleiche Faltung auf 0..0x400 und rsin/rcos-Mischung; setzt rot_y = +0x6a roh voraus (Gegner-KI schreibt rot_y als +0x6a, z.B. `enemy_ai_common.c:3452`) |
| Spieler-Hitbox (`re15_damage.c:3445-3460`) | Laufzeit: +0x78 = 0x80073e94, +0x7c-Vektor (0,−1530,0) in allen 40+ Saves (§2.2) | gleich |
| Zombie-Hitbox 0x10/0x11/0x16 400/1440, Versatz −1440 | RAM (`ss_gegner_hp.py`): (400,1440,400), (0,−1440,0); Box @0x8011f778 (INIT @0x80100778) | gleich |
| `re15_player_take_damage` (`re15_damage.c:198-246`) | §4.1 | gleich für Art 2: kein SE, kein Würfel, HP −1000, +0x5 = 2 + Seite, +0x93 \|= 1, HP<0 → 3/0/0 |
| `hit_from_front` (`re15_damage.c:191-196`) | FUN_8001a7a8 | **Formel gleich**, weil `re15_atan2_q12` = FUN_8001a6d4 + 0x400 (`actor_locomotion.c:128-140`) und das `+1024` auf rot_y das aufhebt: rel = fa6d4 − rot_y + 0x400. Nur das Etikett „front" ist falsch (1 = Punkt hinten, §4.2). |
| Reihenfolge Gegner-Anwendung rückwärts, Spieler immer | @0x80012e00 / @0x80012f24-0x8001302c | gleich |
| Spieler-Tod cmd 3 (`game_step_common.c:175-258`, Auslöser :1296-1299) | Modus 3 @0x800366bc (§4.3) | Clip 7 der PL00-Bank, SE CORE 3, Phase 2 → Zustand 7: gleich |

### 7.2 Abweichungen, die für die Explosion zählen

| # | Port (Datei:Zeile) | Original (Adresse) | Folge für die Granate |
|---|---|---|---|
| A1 | `game_step_common.c:1776` `ENT[9] = {1,1,1,0}` → `re15_player_weapon_fire(9)` :1808-1809 = Sofort-Hitscan beim Abzug | Entlade 0x80033B38 nur `jal 0x8004eae4` @0x80033b40; FUN_80011f50 nie mit 9 (§6) | **resolve muss 0 werden** (`{1,0,1,0}` wie [10]/[11]); der einzige Schaden ist A2 |
| A2 | kein Aufrufer mit Art 2 (Routine 31 fehlt in `re15_esp.c`, `esp_fx_dispatch` kennt 31 nicht, Dossier Runde 30 §2.2) | @0x800185b8 `FUN_80012d60(500, P, 2)`, P = (slot.x, slot.y−500, slot.z) | im R31-Zweig „Zünder == 7" genau einmal `re15_resolve_attack(&{x, y−500, z, radius 500}, 2, −1)` |
| A3 | Gate A: `slot == attacker_slot` (`re15_damage.c:3332`) | `e+0x188+0x40 == slot+0x74` (@0x80012f40-4c) | Granate hat keinen Aktor als Werfer: `attacker_slot = −1` übergeben (niemand ausgeschlossen, §2.3) |
| A4 | **Gate B fehlt**, Kommentar „Tod/Despawn-Flags, inert" (`re15_damage.c:3334-3339`) falsch | `(e+0x93 & 3) == 3` → überspringen (@0x80012f54-60, §2.4) | vor `e->hit_react &= 1` einfügen: `if ((e->hit_react & 3u) == 3u) continue;`. Heute: ein Ziel mit Bit 0+1 verliert Bits 2..6 (u.a. 0x40 aus FUN_80011f50 @0x800123b4), bekommt Bit 7 neu und zählt als Treffer |
| A5 | `re15_re2_stamp_hit(e,1,2)` → `re2z_row_from_atktype[2] = 17` (`enemy_ai_re2_zombie.c:3847`), Spalte aus der Spieler-Zielhöhe, Klammer 0 (`re15_damage.c:2934-2940`) | RE2-Explosion: Hitcode 0x10020009 = **Zeile 9, Klammer 1**, Zone aus P.y gegen Gegner (Schwester-Dossier `re_gegner_re2_familie.md` §1.2, @0x80020d54 / @0x800472a4-0x80047330) | Art 2 muss auf RE2-Zeile **9** (nicht 17) mit Klammer 1 und Zone aus dem Explosionspunkt stempeln; die Waffe 9 mappt der Port schon auf 9 (`re2z_row_from_weapon[9] = 9`, :3831) |
| A6 | `re15_re15_re2z_gore_hit(e, &PLAYER, 1, 2)` (`re15_damage.c:2986`): Peilquelle Spieler | RE1.5: Bit 0x80 aus P (@0x80012f94); RE2: Winkel P → Gegner (@0x80047358) | bei Art 2 heute folgenlos (tödlich → `e->state == 3` → früher Rücksprung `enemy_ai_re2_zombie.c:6696`); korrekt wäre P als Quelle |
| A7 | Hund 0x20 `r = 500, h = 600` „faithful-line" (`re15_damage.c:3508`) | INIT 0x8010d93c (@0x80120f74[0]): `lw v0,3952(v0)` = *(0x80120f70) = 0x80120f64, `sw v0,120(v1)` @0x8010da70; Box `00 00 30 fd 00 00 84 03 d0 02 c2 01` = {0,−720,0, **900,720,450**} (Sektor) | Explosionsreichweite am Hund: 450..900 (nach Winkel) + 500 statt 1000 rund; Y-Band 500+720 |
| A8 | RE1.5-Flavor Tod `re15_enemy_ai_live_death` (`enemy_ai_common.c:5542-5580`) ignoriert +0x5/+0x6, spielt immer Clip 0x0b/0x0d | Todeswurzel 0x80106ba4: `jalr @0x8011feac[+0x5*32 + +0x6*4]`; [9][1] = **NULL** (§8) | Original = Sprung nach 0; der Port zeigt den Standard-Tod. Ziel laut Beta→Retail: RE2-Reaktion (A5) |
| A9 | ESP-Tick `main.c:5413` läuft VOR `re15_game_step` (`main.c:7358`) | ESP-Tick @0x8001ce2c NACH Gegnern @0x8001ce04 und Spieler @0x8001ce0c (§4.5) | im Port würde R31 den Zustand noch im selben Bild vor der KI setzen und gegen die Vorbild-Lagen prüfen: 1 Bild früher als das Original. Der Bau muss den Resolver-Aufruf hinter den Spieler-/KI-Schritt legen oder die Verschiebung benennen |
| A10 | Spieler-Sperre: `pl->hit_react = 0` in JEDEM Normal-Bild (`game_step_common.c:1520`), Kommentar nennt @0x80031964 | @0x80031964 liegt in FUN_800318f8 = Modus-0-Einmal-Init (`sw v0(=1),-13736(at)` @0x8003192c setzt Modus 1); gelöscht wird sonst am Ende der Treffer-Zustände (@0x80035ee8, @0x8003606c) und in 0x800362fc/0x80036420/0x80036690 | gleichwertig nach einem Treffer. Abweichung nur, wenn das Original Bit 0 im Modus 1 gesetzt hält (Schreiber 0x80038180 ff., nicht untersucht → OFFEN) |
| A11 | Tod-Auslöser gegated `!re15_player_is_grabbed() && !re15_stair_active()` (`game_step_common.c:1296-1298`) | Resolver setzt Modus 3 ohne Treppen-Gate (@0x80012ef4); im Griff blockt schon +0x93 Bit 0 (@0x80012e30) | Explosionstod auf der Treppe: Port setzt `state = 3` (in take_damage), startet aber die cmd-3-Animation nicht |
| A12 | Spieler-Trefferbild (Flinch) ohne Resolver-Richtung: der HP-Fall-Detektor (`game_step_common.c:1270-1284`) nimmt FUN_8001a780 gegen den nächsten Gegner | Resolver-Treffer: +0x5 = 2 + FUN_8001a7a8(Spieler, P) | nur nicht-tödliche Resolver-Treffer (Art 0 oder HP > 999); bei der Granate folgenlos |
| A13 | NPC-Typen ≥ 0x40 laufen durch `re15_resolve_attack` wie Gegner | FUN_80012d60 hat **keinen Typfilter** (FUN_80011f50 schon: `lbu v0,8(s1)` / `sltiu v0,v0,0x40` / `beq v0,zero,0x80012540` @0x80012178-84); NPC-HP ist −1 (RAM: Typ 0x40/0x42/0x47 `hp=-1`, +0x93 = 0) → nach 1000 Schaden Zustand 3, +0x5 = 9, +0x6 = 1 | siehe §8.3. Betrifft ROOM1150 (Irons). Port: `re15_npc_ai_tick` fällt für Zustand 3 in `default:` (`enemy_ai_common.c:10550`) = Idle-Pose halten; Original 0x80050DDC über @0x801217d0[+0x6]. Folgen fürs Skript → O2 |

---

## 8. Wie eine RE1.5-KI den Explosionstreffer liest

### 8.1 Zombie-Familie STAGE1 (Typ 0x10/0x11/0x12/0x16/0x18/0x1c..0x1f)

Einstieg (Laufzeit-Tabelle, 3 STAGE1-Saves): 0x80072bac[0x10] = 0x80100424. FUN_80100424 verteilt
auf +0x4: `lbu v0,4(v0)` / `lui at,0x8012` / `addiu at,at,-2124` (= 0x8011f7b4) / `jalr v0`
@0x80100568-88. `table 0x8011f7b4 5`: [1] 0x80101224 (ACTIVE), [2] 0x80105a8c (HURT),
[3] 0x80106ba4 (DEATH).

Der Resolver schreibt bei Art 2: +0x4 = 2 bzw. **3** (HP < 0), +0x5 = **9**, +0x6 = **1**,
+0x7 = 0, +0x93 |= 1 (Bit 0x80 = Punkt hinten). Schaden 1000 gegen Zombie-HP 81..105 (RAM,
`ss_gegner_hp.py`) → **immer Zustand 3**.

HURT-Wurzel 0x80105a8c (`dis_80105a8c.txt`):
```
80105a9c: lbu v0,9(a1) / 80105aa4: andi v0,v0,0x80 / 80105aa8: beq v0,zero,0x80105ae8
   (+9 & 0x80, liegend: (+0x5 - 18) < 2 ? jal 0x80106a38 : jal 0x801068a0 — ohne Tabelle)
80105ae8: lui a0,0x8012 / 80105aec: addiu a0,a0,-1136      ; 0x8011fb90
80105af0: lbu v1,5(a1) / 80105af4: lbu v0,6(a1)
80105af8: sll v1,v1,5 / 80105afc: addu v1,v1,a0 / 80105b00: sll v0,v0,2 / 80105b04: addu v0,v0,v1
80105b08: lw v0,0(v0) / 80105b10: jalr v0                   ; Tabelle[+0x5][+0x6]
```
DEATH-Wurzel 0x80106ba4 (`dis_80106ba4.txt`):
```
80106bb4: lbu v0,9(a1) / 80106bbc: andi v0,v0,0x80 / 80106bc0: beq -> 0x80106bd8
   (+9 & 0x80: jal 0x80107cb0 = liegender Tod, ohne Tabelle)
80106bd8: lui a0,0x8012 / 80106bdc: addiu a0,a0,-340       ; 0x8011feac
80106be0: lbu v1,5(a1) / 80106be4: lbu v0,6(a1) / 80106be8..f4: [+0x5*32 + +0x6*4]
80106bf8: lw v0,0(v0) / 80106c00: jalr v0
```

Matrizen (`re_schaden_werkzeug/tab2d_matrix.py`, Ausgabe `build/r34g_schaden/zombie_*_matrix.txt`;
Zeile = +0x5, Spalte = +0x6):

| Zeile (+0x5) | Bedeutung | HURT @0x8011fb90 Sp0/Sp1 | DEATH @0x8011feac Sp0/Sp1/Sp4 |
|---|---|---|---|
| 1 | Messer (FUN_80011f50 schreibt `sb fp,5(s1)` = Waffen-Id @0x800124bc) | 0x80105B7C / 0x80105B7C | 0x80106C18 / 0x80106C18 / 0x80107634 |
| 3 | Browning (Waffe 3) **und** Resolver-Art 0/1 (DAT_8006f430[0/1] = 3) | 0x80105B7C / 0x80105B7C | 0x80106C18 / 0x80106C18 / 0x80107634 |
| 4..6 | Waffen 4..6 | vorhanden | vorhanden |
| 7, 8 | Revolver, Remington | –/0x80106624 | 0x80108ABC/0x80106EDC bzw. 0x80107EE0 / 0x80107634 |
| **9** | **Handgranate = Resolver-Art 2** | **NULL / NULL** | **NULL / NULL** / 0x80107634 |
| 10, 11 | Säure-/Brandgranate (= Art 3/4) | NULL / NULL | NULL / NULL / 0x80107634 |
| 12..18 | Ingram, SPAS, Flammenwerfer, Werfer | NULL / NULL | NULL / NULL / 0x80107634 |
| 19 | MC51 | 0x80106048 / 0x80106048 | 0x80108ABC / 0x80106C18 / 0x80107634 |

Die drei Tabellen sind statisch und im RAM identisch (`tab_bin_vs_ram.py`:
mzd_stage1_engage_live / room1140_entry / mzd_stage1_briefing_live je 5/5, 96/96, 176/176).
Spalte 4 (0x80107634) ist das spätere Nachzucken der Leiche (Port-Kommentar
`enemy_ai_common.c:5628-5631`), nicht der Sturz; der Resolver schreibt Spalte 1.

**Ergebnis:** Ein stehender Zombie, den die Explosion tötet, springt im nächsten Bild über
`jalr` @0x80106c00 nach **0x00000000** — es gibt keinen Handler. Nur ein liegender Zombie
(+9 & 0x80) stirbt sauber über FUN_80107cb0.

Querschnitt über alle Stages (`tab2d_suche.py`, Ausgabe `build/r34g_schaden/tab2d_suche.txt`): alle
26 gefundenen 2D-Treffer-/Todestabellen in STAGE1..5 (Zombies, Zombie-Mädchen @0x8012039c/
0x8012063c, weitere Familien @0x801209a0/0x80120c94, STAGE3/5 @0x8011ef44 ff.) haben Zeile 9 in
den Spalten 0..3 **NULL**.

### 8.2 Warum DAT_8006f430[2..4] = 9/10/11

DAT_8006f430[2..4] = {9, 10, 11} sind genau die Item-/Waffen-Ids der drei Granaten (§1 Runde 30:
0x09 Hand, 0x0A Acid, 0x0B Incendiary), und die Reaktionszeilen der Gegner-Tabellen sind nach
Waffen-Id angelegt (FUN_80011f50 schreibt die Waffen-Id nach +0x5). Die Resolver-Art 2 ist also
als „Granate 9" vorgesehen, Art 3/4 als „Granate 10/11" — nur hat keine Gegner-KI diese Zeilen
befüllt.

### 8.3 NPCs (Typ ≥ 0x40) — die Explosion trifft sie

Laufzeit-Dispatch STAGE1 (lamp_1170_run4 / mzd_stage1_maggot / mzd_stage1_engage_live, gleich):
0x40 → 0x8011c5a0, 0x42 → 0x8011cb70, 0x45 → 0x8011d140, 0x47 → 0x8011d6d4,
0x49 → 0x8011dc68, 0x4b → 0x8011e22c. Jede Wurzel verteilt +0x4 über eine eigene Tabelle
(@0x80121598 / 0x80121668 / 0x80121738 / 0x801217a0 / 0x80121808 / 0x801218d8); Eintrag [4] ist
überall 0x80050BE8 (Skript-Bewegung), [3] ist ein echter Handler. Beispiel 0x47:
[3] 0x8011db88 → `lbu v0,6(v0)` / `jalr @0x801217d0[+0x6]` @0x8011db98-b8 → mit +0x6 = 1:
**0x80050DDC** (EXE) = f314 auf den laufenden Clip (@0x80050e70), bei Clip-Ende +0x6 := 2
(@0x80050ec8). Folge im Original: das NPC verlässt seinen Skript-Zustand 4 und spielt nur noch
seinen letzten Clip ab. Ob und wie das Skript das wieder einfängt, ist nicht gemessen → OFFEN.

---

## 9. Einordnung fertig/unfertig je Mechanismus (Beta→Retail)

| Mechanismus | RE1.5 | Ziel | Beleg |
|---|---|---|---|
| Zustellung: Resolver-Aufruf aus Routine 31, einmal je Granate, Radius 500, Punkt 500 über der Liegestelle, Art 2 | **fertig** | RE1.5 | @0x800185b8, §1 |
| Resolver FUN_80012d60 (Abstandstest, Gate A/B, Spieler- und Gegnerzweig, Schaden 1000, Reaktion 9) | **fertig** | RE1.5 | §2, §3 |
| Spieler-Eigenschaden (Tod über Modus 3, Clip 7, SE 0x04030001) | **fertig** | RE1.5 | §4 |
| Einmal-Sperre +0x93 | **fertig** | RE1.5 | §5 |
| Gegnerreaktion auf Reaktion 9 (Hurt und Tod) | **unfertig**: Zeile 9 NULL in allen 2D-Tabellen aller Stages | **RE2 Retail** (Zeile 9, Klammer 1; Schwester-Dossier `re_gegner_re2_familie.md`) | §8.1 |
| NPC-Treffer durch die Explosion | Resolver trifft NPCs (kein Typfilter), NPC-Zustand 3 hat Handler | RE1.5 laut Code; Auswirkung offen | §8.3, OFFEN |
| Art 3/4 (Säure/Brand) | Tabellen-Werte ohne Aufrufer | RE2 (Schwester-Dossier `re_saeure_brand.md`) | §3 |
| Hitscan für 9..11 (Tester 0x800128A0) | tot (kein Aufrufer mit 9..11) | entfällt | §6 |

---

Randnotiz Gate A: auch der zweite Resolver-Aufrufer ist eine ESP-Row-Routine —
`table 0x80071d40 48`: [22] 0x80017EB0, [24] 0x80017F50, **[25] 0x80017FA4** (@0x80071da4),
[29] 0x80018320, [30] 0x8001843C, [31] 0x8001854C. Gate A schließt dort den Gegner aus, an dessen
Knochenmatrix der Angriffs-Effekt hängt (slot+0x74). Der Port bildet das als Aktor-Slot ab
(`re15_enemy_attack`, `re15_damage.c:3375-3385`), was für Art 0 gleichwertig ist.

---

## KONSTANTEN FUER DEN BAU

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| Explosions-Radius (a0) | 500 | @0x80018598 | `ori a0,zero,0x1f4` | `re15_attack_box_t.radius` im R31-Aufruf |
| Explosionspunkt Y | slot.y − 500 | @0x800185a0-ac | `lh v0,42(v1)` / `addiu v0,v0,-500` / `sw v0,20(sp)` | `box.y` (X/Z = `lh 40(v1)` @0x80018594, `lh 44(v1)` @0x800185b0) |
| Angriffsart | 2 | @0x800185b4 | `ori a2,zero,0x2` | `attack_type` |
| Resolver-Bild | Zünder == 7, einmal | @0x8001856c-70, jal @0x800185b8 | `ori v0,zero,0x7` / `bne v1,v0,0x800185f4` | genau ein Aufruf je Granate |
| Schaden Art 2 | 1000 | @0x8006f41c | `e8 03` | Port `re15_damage_table[2]` (bytegleich) |
| Reaktion Art 2 | 9 | @0x8006f432 | `09` | Gegner +0x5; Port `re15_react_table[2]` (bytegleich) |
| Schaden/Reaktion Art 3, 4 | 1000/10, 1000/11 | @0x8006f41e/20, @0x8006f433/34 | `e8 03 e8 03`, `0a 0b` | ohne Aufrufer (§3) |
| Gegner-Trefferspalte | +0x6 := 1 | @0x80012fd0-d8 | `ori v0,zero,0x1` / `sb v0,6(s1)` | Resolver-Gegnerzweig |
| Gegner +0x7 | 0 | @0x80012fd4 | `sb zero,7(s1)` | Resolver-Gegnerzweig |
| Art-<2-Tor (SE 10, Vergiftung) | Art < 2 | @0x80012e58 / @0x80012f68 | `sltiu a0,a0,0x2` / `sltiu v0,s0,0x2` | Art 2: kein SE, kein Würfel |
| Abstand horizontal | < r_ziel + 500 (streng), vorher \|dx\|,\|dz\| ≤ R | @0x8002b704-0x8002b770 | `addu s0,v0,s1` / `sltu v0,a2,v0` / `blez s0` | `re15_hitbox_overlap` (bytegleich) |
| Höhenband | \|P.y − (e.y + O.y)\| < 500 + h (streng) | @0x8002b778-0x8002b7a4 | `addu a2,s1,v0` / `slt` / `slt` | `re15_hitbox_overlap` (bytegleich) |
| Gate A | e+0x188+0x40 == slot+0x74 | @0x80012f38-4c | `lw v0,392(s1)` / `lw v1,116(v1)` / `addiu v0,v0,64` / `beq` | Granate: `attacker_slot = −1` |
| Gate B | (e+0x93 & 3) == 3 → überspringen | @0x80012f54-60 | `lw v0,144(s1)` / `lui v1,0x300` / `and` / `beq v0,v1` | fehlt im Port (A4) |
| Seiten-Test | ((w − yaw + 0x400) & 0xfff) < 0x800 = Punkt HINTEN | @0x8001a7e0-ec | `subu` / `addiu v0,v0,1024` / `andi 0xfff` / `slti v0,v0,2048` | Gegner +0x93 Bit 0x80, Spieler +0x5 − 2 |
| Spieler-Hitbox | r 450, h 1530, Versatz (0,−1530,0) | Box @0x80073e94 | `00 00 06 fa 00 00 c2 01 fa 05 c2 01`; +0x7c → 0x800b2354 = (0,−1530,0) in allen Spiel-Saves | im Port schon so |
| Hunde-Hitbox (Typ 0x20) | Sektor r1 900 / h 720 / r2 450, Versatz (0,−720,0) | Box @0x80120f64, INIT `sw v0,120(v1)` @0x8010da70 | `00 00 30 fd 00 00 84 03 d0 02 c2 01` | ersetzt Port 500/600 (A7) |
| Zombie-Hitbox | 400 / 1440, Versatz (0,−1440,0) | Box @0x8011f778, INIT @0x80100778 | RAM `ss_gegner_hp.py` | im Port schon so |
| Spieler-HP Start | 100 | @0x80031710/18 | `ori v0,zero,0x64` / `sh v0,-13586(at)` | 1000 Schaden = Tod |
| Spieler-Tod: Clip | 7 (PL00-Basisbank) | @0x80036778/80 | `ori v1,zero,0x7` / `sb v1,-13592(at)` | cmd 3 (im Port vorhanden) |
| Spieler-Tod: SE | 0x04030001 | @0x80036744 / @0x80036764, jal @0x800367a8 | `lui a0,0x403` / `ori a0,a0,0x1` | cmd 3 (im Port vorhanden) |
| Reihenfolge Hauptlauf | Gegner → Spieler → ESP | @0x8001ce04 / @0x8001ce0c / @0x8001ce2c | `jal 0x8001a50c` / `jal 0x80031c44` / `jal 0x80019e20` | A9 |
| Entlade 9/10/11 | nur Munition | @0x80033b40 / @0x80033b60 / @0x80033b80 | `jal 0x8004eae4` (je 8 Instruktionen) | `ENT[9].resolve = 0` |
| Zombie-Reaktionszeile 9 | NULL (Hurt Sp0..7, Tod Sp0..3) | @0x8011fcb0-cc / @0x8011ffcc-d8 | Tabellenwörter 0 | RE1.5-Reaktion fehlt → RE2 |

## PORT-ABGLEICH

Was der Port heute tut und wo die Lücke ist (Details §7):

1. **Sofort-Treffer beim Abzug statt Explosion** — `game_step_common.c:1776` `ENT[9] = {1,1,1,0}`,
   Aufruf `re15_player_weapon_fire(9)` :1808-1809. Original: nur Munition (@0x80033b40). → resolve 0.
2. **Kein Explosions-Resolver** — Routine 31 fehlt (`re15_esp.c`, Dispatch kennt 31 nicht). Soll:
   `re15_resolve_attack(&(re15_attack_box_t){x, y−500, z, 500}, 2, −1)` genau im Bild Zünder == 7.
3. **Gate B fehlt** (`re15_damage.c:3334-3339`, Kommentar falsch): `(hit_react & 3) == 3` → weiter.
4. **Zeitpunkt** — `main.c:5413` (ESP) vor `main.c:7358` (game_step); Original ESP nach Gegnern
   und Spieler. Der Resolver-Aufruf gehört hinter den Aktor-Schritt (sonst 1 Bild früher und gegen
   Vorbild-Lagen).
5. **RE2-Stempel für Art 2** — `enemy_ai_re2_zombie.c:3847` `re2z_row_from_atktype[2] = 17`,
   Klammer 0, Zone aus der Spieler-Zielhöhe (`re15_damage.c:2934-2940`). RE2 selbst: Zeile 9,
   Klammer 1, Zone aus dem Punkt (Schwester-Dossier `re_gegner_re2_familie.md` §1.2).
6. **Peilquelle Spieler statt Punkt** — `re15_damage.c:2986` (bei Art 2 folgenlos, weil tödlich).
7. **Hunde-Hitbox** — `re15_damage.c:3508` 500/600 geschätzt; Original Sektor 900/720/450 @0x80120f64.
8. **RE1.5-Flavor-Tod** — `enemy_ai_common.c:5542` spielt immer den Standard-Tod; das Original hat
   für Zeile 9 keinen Handler (jalr 0). Ziel ist die RE2-Reaktion (Beta→Retail).
9. **NPCs** — `re15_resolve_attack` trifft wie das Original auch NPCs (kein Typfilter); der Port
   hält sie dann im `default`-Zweig in der Idle-Pose (`enemy_ai_common.c:10550`,
   re15_npc_ai_tick), das Original spielt 0x80050DDC über @0x801217d0[+0x6]. Beides nimmt das NPC aus
   seinem Skript-Zustand 4. Relevanz: Irons in ROOM1150.
10. **Kleinere Abweichungen** ohne Folge für die Granate: Spieler-Sperre jedes Normalbild gelöscht
    (`game_step_common.c:1520`), Treppen-Gate beim Tod (:1296-1298), Flinch-Richtung aus
    FUN_8001a780 statt aus dem Resolver (:1270-1284), Etikett „front" (`re15_damage.c:83-96`),
    Kommentar „Gate B inert" (`re15_damage.c:3334`) und Katalog-Zeile 57/61
    (`RE15_FUN_CATALOG.md`: FUN_8001a7a8 „2=back,3=front" ist vertauscht; Gate B ist +0x93).

Bytegleich und ohne Handlungsbedarf: Tabellen, Abstandstest (Kreis und Sektor), Spieler-Hitbox,
Zombie-Hitbox, Spielerzweig (`re15_player_take_damage`), Todes-FSM des Spielers (cmd 3).

## OFFEN

| # | Offen | Versuchte Wege | Nächster Weg |
|---|---|---|---|
| O1 | Wer den Inhalt des Gegner-Versatzvektors (+0x7c-Ziel) schreibt. RAM zeigt für Zombies/NPCs/Gorilla genau Box[0..2] (z.B. (0,−1440,0)); für den Hund fehlt ein Save mit lebendem Hund, der Wert (0,−720,0) ist deshalb nur aus der Box-Struktur abgeleitet, nicht gemessen | statischer Scan aller `sw rt,124(rs)` in STAGE1 (nur sp-relativ @0x80107ef4) und EXE (@0x800198b0/0x80019b84 = ESP-Slots, @0x800422dc = Zeiger beim Spawn, danach Nullen durch FUN_8004ee60 @0x800422d8) | Schreib-Watchpoint auf den +0x7c-Block (Skill `re15-pcsx-watchpoint`) oder Savestate mit Hund (`re15-room-capture`) |
| O2 | Was ein NPC im Original nach Zustand 3 tut (0x80050DDC, danach +0x6 = 2 → @0x801217d0[2] = 0x80050F00) und ob das Skript es wieder einfängt | statische Kette bis 0x80050ddc gelesen (§8.3) | Savestate in ROOM1150 mit Irons, Resolver-Aufruf erzwingen (Savestate-Patch `re15_ss_patch.py`), 60 Bilder beobachten |
| O3 | Spieler +0x93 Bit 0 im Modus 1: Setzer im Bereich 0x80038180/0x8003841c/0x80038a58 ff. (Funktion und Modus nicht bestimmt) | `grep DAT_800acae7` (26 Schreiber gelistet, §5.2) | Funktionsgrenzen per `scan` bestimmen, Modus-Tabelle zuordnen |
| O4 | FUN_80065de0 als ratan2(y = dz, x = dx) nur aus dem Port übernommen, hier nicht selbst disassembliert (für Kreis-Boxen irrelevant, für die Hunde-Sektorbox relevant) | — | `re15_disasm.py dis 0x80065de0 80` gegen PsyQ `ratan2` |
| O5 | Was die PSX bei `jalr` auf 0x00000000 tatsächlich tut (stehender Zombie stirbt an der Explosion) | nur statisch (§8.1) | DuckStation-Lauf mit Debug-Granate; für den Port ohne Belang, weil RE2 das Ziel ist |
| O6 | Reaktion der Nicht-2D-Familien (Hund 0x20 Hurt 0x801108F0 / Tod 0x80110DC0, Krähe, Spinne, Gorilla 0x27, Writher 0x1a) auf +0x5 = 9 im RE1.5-Flavor | nur die 26 2D-Tabellen geprüft (§8.1) | je Wurzel die +0x5-Verwendung disassemblieren; die RE2-Seite je Typ steht im Schwester-Dossier `re_gegner_bosse_sonstige.md` / `re_gegner_re2_familie.md` |
| O7 | Zeitliche Messung im Original (in welchem Bild nach dem Aufschlag die Reaktion startet) | nur Aufruf-Reihenfolge (§4.5) | Laufzeit-Trace mit Debug-Granate |

Stand: 2026-09-29, alle Abschnitte geschrieben.
