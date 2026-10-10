# A — Varianten-Zensus Leon <-> Elza (RE1.5)

Stand 2026-10-05. NUR ANALYSE (kein Port-Code geaendert). Werkzeug: `zensus_varianten.py`
(dieser Ordner), Ausgaben `zensus.csv`, `zensus.md`, `zensus.json`.

## 1. Welche RDT gehoert zu wem? (belegt)

**xxx0 = Leon, xxx1 = Elza.** Beleg aus der Auslieferungs-EXE `info/Re1.5/PSX.EXE` (selbst disassembliert
mit `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):

Raumlader FUN_800396fc:
```
800397b8  lw   a0,-13764(a0)     ; a0 = DAT_800aca3c
800397cc  addiu at,at,17292      ; 0x8007438c = Stage-Tabelle der Basis-Dateiindizes
800397e0  lhu  v0,0(v1)          ; Basis-Index [Stage][Raum]
800397e4  srl  a0,a0,31          ; Bit 31 von DAT_800aca3c
800397e8  jal  0x80013b60        ; Datei laden (Index)
800397ec  addu a0,a0,v0          ; Index = Basis + Bit31   (Delay-Slot)
```
Basis-Tabelle Stage 1 @0x8007429c (u16): 681, 684, 687, ... (Schritt 3 je Raum).
Dateitabelle @0x8006f43c (8 B je Eintrag, u32 Groesse bei +0; Lesestelle 0x80013b98ff):

| Index | Eintrag @ | Groesse | Datei (Groesse auf Disc) |
|---|---|---|---|
| 680 | 0x8007097c | 0x90000 = 589824 | ROOM100.BSS (589824) |
| 681 | 0x80070984 | 0x2b388 = 177032 | ROOM1000.RDT (177032) |
| 682 | 0x8007098c | 0x2b370 = 177008 | ROOM1001.RDT (177008) |
| 684 | 0x8007099c | 0x29368 = 168808 | ROOM1010.RDT (168808) |
| 685 | 0x800709a4 | 0x33608 = 210440 | ROOM1011.RDT (210440) |

Also Basis = ROOMxxx0, Basis+1 = ROOMxxx1; die eine BSS (Hintergrundbilder) liegt bei Basis-1 und ist
fuer beide Varianten DIESELBE Datei.

Welche Figur Bit 31 bedeutet — Neues-Spiel-Einstieg FUN_8001d22c:
```
8001d29c  lw   v0,-13764(v0)     ; DAT_800aca3c
8001d2a4  bltz v0,0x8001d324     ; Bit 31 gesetzt ->
8001d2a8  ori  v0,zero,0x17      ;   sonst Raum 0x17 (HELIPORT = ROOM1170, Leons Start)
8001d2b0  sh   v0,4066(at)       ;   -> 0x800b0fe2 (Raumindex)
8001d324  ori  v0,zero,0x3       ; Bit 31: Raum 0x03 (LOBBY = ROOM1031, Elzas Start)
8001d32c  sh   v0,4066(at)
```
Dazu die beiden Vorspann-RDTs (re15_gameflow.c:17-27, Bytes nachgelesen): ROOM1240 @0x0531 = 0x17,
ROOM1241 @0x0531 = 0x03. Bit 31 = Elza, Bit 31 = 0 = Leon.

### 1.1 Szenario-Abfrage INNERHALB einer RDT

Die SCD-Flagbank-Tabelle @0x80074664 (selbst gelesen):
`[0] 0x800aca38  [1] 0x800aca3c  [2] 0x800aca40  [3] 0x800b0ff8  [4] 0x800b1018  [5] 0x800b1028
 [6] 0x800b1030  [7] 0x800b1038  [8] 0x800b1058  [9] 0x800b1078  [10] 0x800b1098  [11] 0x800aca44
 [12] 0x800b8520  [13] 0x800b10a8`.
Ck-Handler @0x8003fcf4: Bank = pc[1] (`lbu a2,1(v0)`, `sll a2,a2,2`, Tabelle @0x80074664),
Bit = pc[2] (`andi a0,a1,0x1f`, Wort `srl v1,a1,3; andi v1,v1,0x1c`), Test `lui v0,0x8000; srlv v0,v0,a0`
= MSB-first. **Das Elza-Bit (DAT_800aca3c Bit 31) ist also SCD-Flag (1,0).** Eine Szenario-Verzweigung
innerhalb einer RDT muesste als `21 01 00 xx` (Ck) stehen. Ergebnis des Zensus: siehe Abschnitt 3.

(Sections 2 ff. werden fortlaufend ergaenzt.)

Fortsetzung 2026-10-10 (Runde 36, Spur P): Figuren/Dialoge/Kreuz-Zeitleiste in `B_figuren.md`, Hauptwege in
`ablauf_leon.md` (mit Port-Ergaenzungen [PORT]) und `ablauf_elza.md`.
