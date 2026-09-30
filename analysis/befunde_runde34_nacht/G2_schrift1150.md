# Spur G2 — ROOM1150 Irons' Buero: blinkende Schrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Zweig r34n/schrift1170, Baum .claude/worktrees/r34n_schrift.
Status: IN ARBEIT (Abschnitte werden fortlaufend gefuellt und committet).

## 0 Kurzfassung

(folgt, sobald Messung Original + Port stehen)

Zwischenstand 08:35: Mechanismus gefunden. Die Schrift ist die rote Leuchtschrift hinter den
Jalousien in **Cut 2** (Blick auf Irons' Schreibtisch). Sie blinkt im Original ueber **sprite.pri-
Maskengruppen 6..11** von Cut 2, die das Raumskript **sub05** mit SCD-Opcode **0x45** im Wechsel aus-
und einschaltet (`Sleep 20` dazwischen, Endlosschleife). Der Port ueberspringt Opcode 0x45 (nur
Laenge 3 in `s_opcode_sizes`, Handler `op_unknown`) und zeichnet alle Masken immer — die Schrift steht
dort dauerhaft im "aus"-Zustand.

## 1 Nutzerwortlaut + Lesart

Wortlaut (AUFTRAG.md, letzter Punkt + Korrektur): *"In ROOM 1170 blinkt im Hintergrund die Schrift des
Gebäudes. Bei uns nicht."* — korrigiert: *"Nein, nicht beim Heliport, sondern in Irons Office room
1150 blinkt die Schrift eigentlich im Hintergrund."*

**Zensus der Schriften in ROOM1150.** Alle 9 Hintergruende (ROOM115.BSS = 0x90000 Byte, Cut n =
Datei[n*0x10000]) mit den Engine-Funktionen des Ports dekodiert (`probe_r34n_g_bss`, Bild
`G_belege/G2_01_bss_room115_alle_cuts.jpg`). Schrift im Hintergrund gibt es in genau einem Cut:

| Kandidat | Cut | Lage (320x240) | Beleg |
|---|---|---|---|
| rote Leuchtschrift **draussen hinter den Jalousien** (drittes Fensterfeld hinter Irons' Schreibtisch) | 2 | x139..216 y25..40 | `G2_02`, `G2_03` |
| Fenster mit Jalousien ohne Schrift | 0, 1, 3 | — | `G2_01` |
| Item-Kiste "ITEM LIST / NO DATA" (Bildschirm, keine Gebaeudeschrift) | 8 | ganzes Bild | `G2_01` |

**Gewaehlte Lesart:** die rote Leuchtschrift hinter den Jalousien in Cut 2. Begruendung: einzige
Schrift, die im Hintergrund des Raums liegt (ein Gebaeude gegenueber, durch das Fenster gesehen), und
**genau diese Stelle** schaltet das Original periodisch um (Abschnitt 3) — das ist der einzige
Mechanismus in ROOM1150/1151, der irgendetwas im Bild blinken laesst (Zensus Abschnitt 3.4).
ROOM1151 (Elza) hat dieselben Hintergruende (gleiche BSS), dieselben Masken und dieselbe Schleife.

## 2 Ist-Zustand im Port (gemessen)

(folgt)

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die Schleife im Raumskript (ROOM1150.RDT, Datei-Offsets)

`scd_dump_room.py`, ROOM1150 sub05 (Region 0x10B6..0x10EA), gestartet von sub00
(`Evt_exec 04 ff 18 05` @0x00EB6):

    0x010B6  45 05 00   Col_chg_set  Gruppe 6  := 0
    0x010B9  45 06 00   Col_chg_set  Gruppe 7  := 0
    0x010BC  45 07 00   Col_chg_set  Gruppe 8  := 0
    0x010BF  45 08 00   Col_chg_set  Gruppe 9  := 0
    0x010C2  45 09 00   Col_chg_set  Gruppe 10 := 0
    0x010C5  45 0a 00   Col_chg_set  Gruppe 11 := 0
    0x010C8  09 0a 14 00  Sleep 20
    0x010CC  45 05 01 ... 0x010DB 45 0a 01   Gruppen 6..11 := 1
    0x010DE  09 0a 14 00  Sleep 20
    0x010E2  17 ff ff 00 d4 ff   Goto -0x2C -> 0x010B6 (Endlosschleife)

ROOM1151: dieselben 52 Byte @0x01094..0x010C7 (sub05, gestartet von sub00 @0x00E94).

### 3.2 Opcode 0x45 im Original (PSX.EXE, selbst disassembliert mit re15_disasm.py)

Handler @0x800428d4 (Eintrag 0x45 der Opcode-Tabelle @0x800744a8):

    800428e4: lw  v0,28(s0)          ; pc
    800428ec: lbu a0,1(v0)           ; op1 (Gruppe-1)
    800428f0: lbu a1,2(v0)           ; op2 (neuer Wert)
    800428f4: jal 0x800396a8
    800428f8: addiu a0,a0,1          ; a0 = op1+1
    80042904: addiu v1,v1,3          ; pc += 3
    80042900: ori v0,zero,0x1        ; weiter im selben Bild

FUN_800396a8 (einzige Aufgabe: Byte0 aller Records einer Gruppe setzen):

    800396b0: lw  v0,-14472(v0)      ; DAT_800ac778 = RDT im RAM
    800396b8: lbu a3,0(v0)           ; a3 = RDT[0] = Maskenzahl des AKTUELLEN Cuts (FUN_800392d4 schreibt sie)
    800396c0: lw  v1,9604(v1)        ; DAT_800b2584 = Record-Tabelle, 4 Byte je Maske
    800396c4: beq a3,zero,0x800396f0
    800396d0: lbu v0,1(v1)           ; Record-Byte1 = Gruppe+1
    800396d8: bne v0,a0,0x800396e4
    800396e0: sb  a1,0(v1)           ; Treffer -> Record-Byte0 := op2
    800396e8: bne v0,zero,0x800396d0 ; fuer alle a3 Records
    800396ec: addiu v1,v1,4

Aufbau und Verbrauch der Record-Tabelle (Decompilate `RE_15_Quellcode_V2/FUN_800392d4.c`,
`FUN_80039590.c`, `FUN_80039270.c`):
* FUN_80039270 (Raumladen): DAT_800b2584 = Pool-Zeiger, Platz fuer RDT[7] Records.
* FUN_800392d4 (Cut-Wechsel): alle RDT[7] Records Byte0 := 0; dann je gebauter Maske Byte0 |= 1,
  Byte1 := Gruppenindex+1, Halbwort +2 := Tiefe; RDT[0] := Maskenzahl (Kopf-Halbwort hoch).
* FUN_80039590 (jedes Bild): fuer Record i < RDT[0] **nur wenn Byte0 & 1**: SPRT + DR_MODE (TPage 0x95)
  in die OT bei Tiefe (Halbwort +2) haengen.

=> Opcode 0x45 `Col_chg_set g v` schaltet alle Masken der Gruppe g+1 des aktuell gezeigten Cuts
sichtbar (v=1) oder unsichtbar (v=0). Jeder Cut-Wechsel schaltet alle Masken wieder ein.

### 3.3 Was die Gruppen 6..11 in ROOM1150 sind

sprite.pri von Cut 2 (@0x0066C): 11 Gruppen, 54 Masken. Die Gruppen 6..11 haben je genau eine Maske
(`schrift1150_masken.py`):

| Gruppe | Record (Datei) | Atlas src | Bild dst | Groesse | Tiefe |
|---|---|---|---|---|---|
| 6 | 0x0089C | (168,80) | (139,25) | 16x16 | 0 |
| 7 | 0x008A4 | (184,80) | (160,25) | 8x16 | 0 |
| 8 | 0x008B0 | (192,80) | (167,25) | 16x16 | 0 |
| 9 | 0x008B8 | (208,80) | (176,25) | 16x16 | 0 |
| 10 | 0x008C0 | (224,80) | (190,25) | 16x16 | 0 |
| 11 | 0x008C8 | (240,80) | (201,25) | 16x16 | 0 |

In keinem anderen Cut von ROOM1150/1151 gibt es eine Gruppe >= 6 (Zensus `G2_04`): der Opcode wirkt
also nur, solange Cut 2 steht. Der SLD-Atlas von Cut 2 traegt an (168..255, 80..95) die Jalousie-
Lamellen **ohne** Rotlicht in Buchstabenform (Index 0 = durchsichtig zwischen den Buchstaben), der
BSS-Hintergrund an der Zielstelle dieselben Lamellen **mit** rotem Leuchten (`G2_03`). Gezeichnet
(Byte0 = 1) verdecken die Masken das Rot in Buchstabenform = **Schrift aus**; ausgeschaltet (Byte0 = 0)
sieht man den BSS = **Schrift an**. Unterschied AN/AUS: 671 Pixel im Rechteck x139..216 y25..40
(`G2_02`).

### 3.4 Zensus Opcode 0x45 ueber alle 240 RDTs

`col_chg_zensus.py` (`G_belege/G2_04_zensus_opcode45.txt`): 10 Raeume, 120 Stellen —
ROOM1150/1151 (je 12, die Blink-Schleife), ROOM1211 (2), ROOM3000/3001 (je 18), ROOM3010/3011
(je 12), ROOM3071 (32), ROOM5060/5061 (je 1). Die anderen Raeume schalten Gruppen nur AUS (ausser
ROOM3071 sub2, eine Folge an/aus) — eine allgemeine Umsetzung des Opcodes aendert auch dort das Bild
(Abschnitt 7).

(Fortsetzung folgt: Takt des Sleep, Savestates, Port-Messung)

## 4 Soll-Verhalten (Zeitlinie)

(folgt)

## 5 Bauplan

(folgt)

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen

(folgt)

## 8 Offene Punkte

(folgt)
