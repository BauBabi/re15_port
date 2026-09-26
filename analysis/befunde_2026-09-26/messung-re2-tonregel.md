# Messung: Gilt RE2s Bestaetigungston fuer ALLE Ja/Nein-Abfragen?

Status: IN ARBEIT (wird laufend gefuellt)
Datum: 2026-09-26
Binaer: RE2-Retail-Leon EXE (re2_disasm.py), RE2-Overlays

## Auftrag
Ist FUN_80030844 in RE2 der allgemeine Ja/Nein-Abfrage-Weg, oder nur einer von
mehreren — und ist der Ton dort bedingungslos?

## (a) Disassemblat FUN_80030844
(folgt)

## (b) Zensus der Aufrufer
(folgt)

## (c) Zweiter Weg?
(folgt)

## (d) Gegenprobe: stumme Abfrage?
(folgt)

## (e) Urteil
(folgt)

---
## (a) FUN_80030844 — selbst disassembliert (info/re2leon/PSX.EXE, RAM = 0x80010000 + Dateioffset - 0x800)

Signatur (Prolog @0x80030844 `addiu sp,sp,-112`):
`a0 -> s3` (Laufzeit-Struktur, Felder ab +23692 = +0x5C8C),
`a1 -> s0` (6-Byte-Deskriptor), `a2 -> s1` (FLAGBYTE), `a3 -> s6`.

Die Ton-Stelle, roh gelesen:

    80030908: lhu  v0,23720(s3)      ; +0x5CA8
    80030910: andi v0,v0,0x2000
    80030914: bne  v0,zero,0x80030924   ; Gate A umgehen
    80030918: andi v0,s1,0x60           ; Gate A: Freigabebits
    8003091c: beq  v0,zero,0x80030b68   ; -> KOMPLETT STUMM
    80030924: lbu  v0,23692(s3)         ; Auswahl-Byte
    8003092c: andi v1,v0,0xf            ; Index
    80030930: beq  v1,zero,0x80030948   ; Index 0
    80030934: addiu v0,zero,1
    80030938: beq  v1,v0,0x80030954     ; Index 1 -> a0=0x04050000
    8003093c: lui  a0,0x405
    80030940: j    0x80030954           ; Index >=2 -> a0=0x04060000
    80030944: lui  a0,0x406
    80030948: andi v0,s1,0x20           ; Gate B (nur Index 0!)
    8003094c: bne  v0,zero,0x80030b68   ; -> JA STUMM
    80030950: lui  a0,0x406
    80030954: jal  0x8005ba28           ; Se_on
    80030958: addu a1,zero,zero

WAHRHEITSTABELLE (aus den Bytes, nicht modelliert):

| s1 & 0x60 | Index 0 (Ja) | Index 1 (Nein) | Index >=2 |
|---|---|---|---|
| 0x00 | STUMM | STUMM | STUMM |
| 0x20 | **STUMM** (Gate B) | 0x0405 | 0x0406 |
| 0x40 | 0x0406 | 0x0405 | 0x0406 |
| 0x60 | **STUMM** (Gate B) | 0x0405 | 0x0406 |

⛔ Das 0x2000-Bit @0x80030910 umgeht NUR Gate A, NICHT Gate B: der Sprung landet auf
0x80030924, und 0x80030948 wird danach trotzdem durchlaufen.
→ **Der Ton ist NICHT bedingungslos.** Ob das Ja klingt, haengt allein an Bit 0x40/0x20
   des Flagbytes s1.

### Woher s1 kommt — das Flagbyte ist ein OPERAND IM TEXTSTROM, direkt hinter 0xFB

Die Nachrichten-FSM ist FUN_8003027c (Prolog `addiu sp,sp,-48` @0x8003027c).
Zustandsbyte `+23693(s1)`, 7er-Sprungtabelle @0x80010a24 (selbst gelesen):

    0x80010a24: 800302d0 80030328 80030618 800306ec 80030728 8003079c 800307c4
                 St0      St1      St2      St3      St4      St5      St6

Steuercode-Dispatch @0x800303c4 `lbu v1,0(s0)` (s0 = Textcursor, `lw s0,23704(s1)`
@0x80030298). Der Zweig fuer **0xFB (251)**:

    80030418: addiu v0,zero,251
    8003041c: beq   v1,v0,0x800304b8
    …
    800304b8: addiu v0,zero,4
    800304bc: sb    v0,23693(s1)     ; Zustand 4  -> 0x80030728
    800304c0: addiu v0,zero,8
    800304c4: sllv  v0,v0,s2
    800304c8: j     0x80030818
    800304cc: sb    v0,23722(s1)     ; Einblend-Timer

s0 wird dabei NICHT weitergezaehlt. Zustand 4 @0x80030728 liest deshalb:

    80030728: addiu v1,zero,64
    80030730: lw    v0,-1164(v0)     ; 0x800cfb74
    80030734: lbu   a2,1(s0)         ; <<< das Flagbyte = Byte HINTER 0xFB
    80030738: andi  v0,v0,0x840
    8003073c: bne   v0,v1,0x80030770 ; Layouttabelle 0x8009dee4 (a3=0)
    80030744: andi  v0,a2,0xf        ;  bzw. 0x8009df10 (a3=1)
    80030748..5c: a1 = Tabelle + (a2&0xf)*6
    80030760: jal   0x80030844
    80030764: addiu a3,zero,1

→ **`s1` ist kein Laufzeitzustand und keine Tabelle, sondern ein LITERALES BYTE im
Nachrichtentext: das Byte direkt hinter dem Auswahlbox-Code 0xFB.**
Low-Nibble = Layout-Index, Bit 0x20/0x40 = die Ton-Gatter, Bit 0x80 = Glyphenpfad
(@0x80030a74/@0x80030aec). Damit ist die vom Pruefer verlangte Messung MOEGLICH:
alle `FB xx` in RE2s Nachrichtendaten zaehlen.

---
## (b)+(d) ZENSUS — was das Flagbyte in RE2s echten Abfragen WIRKLICH ist

### Wo die Nachrichten liegen (selbst hergeleitet, nicht aus einer Doku)

Die Textzeiger kommen aus dem RDT-Kopf, gelesen @0x800301bc-fc:

    800301c0: lw   v0,-1164(v0)        ; 0x800cfb74  Sprach-/Modusflag
    800301c8: andi v0,v0,0x840
    800301cc: bne  v0,v1,0x800301e8    ; v1 = 64
    800301d4: lw   v0,8508(t3)         ; RDT-Kopfzeiger
    800301dc: lw   v0,64(v0)           ; +0x40 -> Offsetindex (0x40-8)/4 = 14
    800301e4: sw   v0,23700(t0)
    800301f0: lw   v0,60(v0)           ; +0x3C -> Offsetindex 13
    800301f8: sw   v0,23700(t0)

Selbst dekodiert: **Index 13 = japanischer Block, Index 14 = ENGLISCHER Block.**
Zeichentabelle aus `info/re2leon/PL0/RDT/ROOM1010.RDT` Block 14 hergeleitet
(0x00=Leer, 0x01='.', 0x0B-0x14='0'-'9', 0x1D-0x36='A'-'Z', 0x3D-0x56='a'-'z';
Steuercodes 1 Byte: 0xFC/0xFD Zeilenumbruch — 2 Byte: 0xF8/0xF9/0xFA/0xFB/0xFE).
Probe: `2c 3d 4c 41 4e 00 40 3d 50 41 40` = "Paper dated".
Nachricht endet bei 0xFE; das Folgebyte ist die Nachlaufzeit (Zustand 6
@0x800307c4, `sllv v0,v0,s2` @0x80030810).

### Das Ergebnis — 250 RDTs, 135 mit Textblock, 1157 Nachrichten, 122 Ja/Nein-Boxen

| Operand hinter 0xFB | Anzahl | s1&0x60 | Ja (Index 0) | Nein (Index 1) |
|---|---|---|---|---|
| **0x40** | **47** | 0x40 | **0x0406 TON** | 0x0405 TON |
| **0x00** | **39** | 0x00 | **STUMM** | STUMM |
| **0x20** | **23** | 0x20 | **STUMM** (Gate B) | 0x0405 TON |
| 0x01-0x06 | 13 | 0x00 | **STUMM** | STUMM |

**Summe 122.** Ton beim JA: **47 von 122 = 38,5 %.** Irgendein Ton: 70 von 122.
Voellig stumm: 52 von 122.

### (d) Die verlangte GEGENPROBE — RE2 hat stumme Ja/Nein-Abfragen, und zwar 52

Mit Dateibeleg (Byte `FB 00` bzw. `FB 0x`, Offsetindex 14 der jeweiligen RDT):

| RDT / msg | Text | Operand | Ja-Ton |
|---|---|---|---|
| ROOM20B0 msg00 | "A police station map. Will you take it?" | `FB 00` | **STUMM** |
| ROOM4080 msg42 | "There are Blue Herbs growing here. Will you use them?" | `FB 00` | **STUMM** |
| ROOM2080 msg02 | "Will you use the special key?" | `FB 00` | **STUMM** |
| ROOM2090 msg04 | "The lock is broken. Will you use the lockpick?" | `FB 00` | **STUMM** |
| ROOM60C0 msg05 | "Commencing with fingerprint verification. Will you proceed?" | `FB 00` | **STUMM** |
| ROOM20D0 msg04-06 | "Will you turn the faucet on?" | `FB 20` | **STUMM** (Nein toent) |
| ROOM10E0 msg00/01 | "It's a switch. Will you push it?" | `FB 20` | **STUMM** (Nein toent) |
| ROOM20B0 msg13-16 | "First/Second/Third/Forth digit number?" (Zifferneingabe) | `FB 02` | **STUMM** |
| ROOM2130 msg01-05 | "It's a switch. Will you move it?" | `FB 03` | **STUMM** |

⛔ **Der direkte Widerspruch in derselben Klasse:**
`ROOM20B0 msg00` "A **police station map**. Will you take it?" = `FB 00` = STUMM,
`ROOM2130 msg00` "A **police B2 map**. Will you take it?"      = `FB 40` = TON,
`ROOM4040 msg10` "There's a **sewer map**. Will you take it?"   = `FB 40` = TON.
→ RE2 hat **keine Regel**, sondern eine **pro Nachricht handgesetzte Freigabe**.
   Zwei wortgleiche Karten-Aufnahmen klingen verschieden.
