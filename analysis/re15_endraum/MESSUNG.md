# Wo endet RE1.5? — Messung statt Vermutung (2026-08-30)

Anlass: fuer den Import des RE2-Endkampfs muss feststehen, WO in RE1.5 er hingehoert.
Mein erster Versuch war ein Roh-Byte-Scan nach Opcode 0x44 — der lieferte hunderte
Falschtreffer in Datenregionen und ist wertlos. Diese Messung benutzt statt dessen den
opcode-exakten SCD-Walker des Ports (`re15_port/tools/aot_sce_census.py`,
Laengentabelle disasm-verifiziert, game-weit 100,00 % Abdeckung, 0 Desyncs) und den
bereits validierten Tuer-Graphen (`scripts/door_graph.py`, 649/649 echte Tueren loesen auf).

## ERGEBNIS

**RE1.5 endet in ROOM6040.**

```
  ROOM6040   Tiefe 21 vom Startraum ROOM1240
             0 Door_aot_set          <- KEIN Ausgang
             0 Sce_em_set            <- kein Gegner
             ROOM604.BSS vorhanden   <- der Raum hat einen Hintergrund
             RDT 163.048 B
             Eintritt NUR aus ROOM5090 slot 5:
               Spawn (-26214, 0, -3861), yaw 0, cut 0, band 1
```

Der Raum taucht im Tuer-Graphen ueberhaupt nicht als Quelle auf (`'6040' in graph == False`),
weil er keine einzige Tuer installiert. Er ist die einzige echte Einbahn-Sackgasse des
Spiels: alle anderen 22 Sackgassen fuehren zurueck (siehe Tabelle unten).

Das ist dieselbe Form, die RE2s Arena ROOM7040 hat — dort eine Tuer zurueck, hier gar keine.

## DIE ZEHN TIEFSTEN ERREICHBAREN RAEUME

BFS ab ROOM1240 (Startraum laut CLAUDE.md/Headless-Smoke), nur Leon-Variante:

```
  ROOM6040 Tiefe 21  Tueren  0  Ziele —KEINE—            Gegner —                       BG ja
  ROOM5080 Tiefe 20  Tueren  1  Ziele 6010               Gegner 0x30 x1                 BG ja
  ROOM5090 Tiefe 20  Tueren  6  Ziele 5090,6030,6040     Gegner 0x30 x1, 0x4D x1        BG ja
  ROOM4050 Tiefe 19  Tueren 14  Ziele 4040,4050          Gegner 0x13 x2, 0x18 x2        BG ja
  ROOM6010 Tiefe 19  Tueren  4  Ziele 5080,6000          Gegner —                       BG ja
  ROOM6030 Tiefe 19  Tueren  4  Ziele 5090,6000,6030     Gegner 0x40,0x49,0x4B,0x4D x2  BG ja
  ROOM4040 Tiefe 18  Tueren  4  Ziele 4030,4050,4070     Gegner 0x29 x2                 BG ja
  ROOM6000 Tiefe 18  Tueren  4  Ziele 50B0,6010,6030     Gegner 0x4D x2                 BG ja
  ROOM4070 Tiefe 17  Tueren  3  Ziele 4040,50A0,5100     Gegner —                       BG ja
  ROOM5050 Tiefe 17  Tueren  2  Ziele 5040,5120          Gegner —                       BG ja
```

Erreichbar sind 97 der 102 Raeume mit Ausgang (Leon-Variante).

## ⛔ TYP 0x30 STEHT SCHON DA

Die beiden Raeume unmittelbar VOR dem Ende fuehren bereits Typ **0x30** — den Typ, fuer den
RE1.5 ein eigenes Modell in `shared_assets/PSX/EMD/CDEMD0.EMS` hat:

```
  ROOM5090 sub0 @0x124a: Typ 0x30, grid 0x33
  ROOM5080            : Typ 0x30
```
Wer den RE2-Boss ans Ende setzt, hat also einen Vorlauf, in dem die RE1.5-Variante desselben
Typs bereits auftritt. Ob 0x30 hier wirklich als Gegner laeuft oder nur installiert wird,
ist NICHT gemessen — dazu muesste das Raum-Skript gelaufen sein. [offen]

## ⛔ NEBENBEFUND: die Feldlage von Sce_em_set im Port ist falsch dokumentiert

`scd_vm.c` beschreibt den Record als
`[op, slot, type, behavior, hp_lo, hp_hi, pos_x×4, pos_y×4, pos_z×4, rot_y_lo, rot_y_hi,
flags_lo, flags_hi]` — das sind 22 Byte und passt schon rechnerisch nicht zu den
belegten 20. Aus dem Handler FUN_800420A0 selbst gelesen (s2 = Opcode+2, a1 = Opcode):

```
  +1  Slot, `lbu v0,1(a1)` @0x800420f0, Slot = v0 & 0x7F, Bit 0x80 ist eine Flagge
      (Entity = Basis + Slot*500, gerechnet @0x80042100-14)
  +2  TYP            `lbu v0,0(s2)` @0x800422b4  ->  `sb v0,8(s0)`   @0x800422bc  (Entity+0x08)
  +3  grid_id        `lbu v0,1(s2)` @0x8004215c  ->  `sb v0,9(s0)`   @0x80042164  (Entity+0x09)
  +4  Band/Etage     `lbu v0,2(s2)` @0x800421c8  ->  `sb v0,130(s0)` @0x800421d0  (Entity+0x82)
  +6                 `lbu v1,4(s2)` @0x80042254
  +7  Kill-Flagge    `lbu a1,7(a1)` @0x80042120, 0xFF = Bitmap-Test ueberspringen
                     ->  `sb v0,454(s0)` @0x80042170                              (Entity+0x1C6)
  +8  X  (s16)       `lh v0,6(s2)`  @0x80042174  ->  `sw v0,52(s0)`  @0x8004217c  (Entity+0x34)
  +10 Y  (s16)       `lh v0,8(s2)`  @0x80042180  ->  `sw v0,56(s0)`  @0x80042188  (Entity+0x38)
  +12 Z  (s16)       `lh v0,10(s2)` @0x8004218c  ->  `sw v0,60(s0)`  @0x80042194  (Entity+0x3C)
      dazu die u16-Spiegel nach Entity+0x40/0x42/0x44 (@0x80042198-b8)
  +16 Yaw (u16)      `lhu v0,14(s2)`@0x800421bc  ->  `sh v0,106(s0)` @0x800421c4  (Entity+0x6A)
  = 20 Byte, PC += 20 @0x8004262c
```
Also: **die Positionen sind s16 auf +8/+10/+12, nicht 4-Byte-Felder auf +6/+10/+14, und ein
HP-Feld auf +4 gibt es nicht.** Der Typ auf +2 stimmt dagegen.
Das ist NICHT geprueft daraufhin, ob der Port an anderer Stelle trotzdem richtig dekodiert —
nur der Kommentar ist nachweislich falsch. [zu pruefen, bevor jemand danach baut]

## METHODE (nachvollziehbar)

```
python re15_port/tools/aot_sce_census.py <out.json>   # 240 RDTs, 100,00 % Abdeckung, 0 Stops
python scripts/door_graph.py                          # 649/649 Tueren, schreibt door_graph.json
```
Danach BFS ab ROOM1240 ueber die Leon-Varianten und Sce_em_set-Einsammlung mit der oben
belegten Feldlage.
