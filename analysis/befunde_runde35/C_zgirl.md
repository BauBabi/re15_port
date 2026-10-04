# Runde 35 Spur C — "Zombie Maedchen" (Typ 0x13, EM013)

Auftrag (woertlich, AUFTRAG.md Z.8): "Was sollen Zombie Mädchen sein? Wenn es das gibt, muss es
natürlich mit portiert werden"

Baum: .claude/worktrees/r35_zgirl, Zweig r35/zgirl, Basis 154a73c1 (+ e7e7131c Orchestrator-Hinweis).

## Fuer den Nutzer (5 Zeilen)
(wird am Ende gefuellt)

## Messung vorher

### M1 Zensus aller Sce_em_set (Opcode 0x44) — alle 6 Stages, main + alle subs (2026-10-04)
Werkzeug: `re15_port/tools/r35_zgirl/em_zensus.py` (opcode-exakter Walk ueber `tools/scd_walk_lib.py`,
dieselbe Laengentabelle wie `scd_vm.c`; KEINE Rohbyte-Suche). Layout laut `op_sce_em_set`
(scd_vm.c): pc[1]=Slot, pc[2]=Typ, pc[3]=Verhalten, pc[7]=Kill-Flag, pc[8..13]=x,y,z, pc[16]=dir.
36 RDTs sind 4-Byte-Platzhalter (1270/1271, 20C0-20F1, 30F0/1, 4060/1, 40C0-40F1, 5150-5171,
6050-6071) und tragen kein SCD.

**Typ 0x13 kommt in GENAU EINEM Raum vor: ROOM4050 (+ Zwilling ROOM4051), main00, zwei Records:**
```
ROOM4050 main00 @0x01eb4  44 00 13 00 00 01 00 a0 54 d9 00 00 7e 04 00 00 00 02 00 00
         slot=0 type=0x13 beh=0x00 killflag=0xa0 pos=(-9900,0,1150)  dir=512
ROOM4050 main00 @0x01f5c  44 00 13 00 00 01 00 7d 40 06 00 00 5c 12 00 00 00 04 00 00
         slot=0 type=0x13 beh=0x00 killflag=0x7d pos=(1600,0,4700)   dir=1024
(ROOM4051 byte-gleich an denselben Offsets)
```
Kontext (scd_dump_room.py ROOM4050 main00): beide Records stehen in einem
`Switch(work_vars[0x0A])` @0x01E56 `13 0a d2 01`. work_vars[0x0A] = DAT_800b0fe4 = der
ANGEZEIGTE Cut, vom Raumlader aus dem Tuer-Payload Byte 10 gestempelt (@0x8001d948
`sh v1,4068(at)`, zitiert in scd_room_setup.c:403-410). D.h. main00 waehlt den Gegner nach dem
EINTRITTS-CUT:
| Case (Cut) | Offset | Gegner | Item_aot_set |
|---|---|---|---|
| 6  | 0x1E5A | 0x18 (Zombie-Variante) beh 0x81 kf 0x9f | slot 30 Item 0x24 |
| 9  | 0x1EAE | **0x13** kf 0xa0 (-9900,0,1150) | slot 30 Item 0x26 |
| 11 | 0x1F02 | 0x18 beh 0x0d kf 0x7c | slot 30 Item 0x23 |
| 14 | 0x1F56 | **0x13** kf 0x7d (1600,0,4700) | slot 30 Item 0x25 |
| 4, 5 | 0x1FAA/0x1FEA | kein Gegner | Items 0x23/0x22 |
ROOM4050 ist ein Labyrinth aus 15 Cuts, dessen 14 Door_aot_set (sce 2) fast alle wieder in
ROOM4050 fuehren (Payload `03 05 <cut>` = Stage-Index 3, Raum 5, Ziel-Cut). Zum Zombie-Maedchen
fuehren: Tuer-Slot 6 (rect (-10300,-23200), Ziel-Cut 9) und Tuer-Slot 7 (rect (-17950,-25750),
Ziel-Cut 0x0e).

Typen-Histogramm (Auszug): 0x10 35 Raeume, 0x11 21, 0x16 11, 0x18 13, 0x13 **2 (4050/4051)**.

### M2 Registrierung Typ 0x13 in den Stage-Overlays (re15_reg_scan.py)
0x13 ist in STAGE1..5 registriert (je eine relozierte Kopie), in STAGE6 nicht:
STAGE1 0x8010a8c8, STAGE2 0x8010a75c, STAGE3 0x8010a9b4, **STAGE4 0x8010a87c** (der Raum, in dem
sie vorkommt), STAGE5 0x8010a9fc. Die bisherige Portierung (RE15_ZOMBIEGIRL_AI.md) wurde an der
STAGE1-Kopie (0x8010a8c8) gelesen.

## RE-Belege
(laufend)

## Umsetzung
(laufend)

## Messung nachher
(laufend)

## Tests
(laufend)

## OFFEN
(laufend)
