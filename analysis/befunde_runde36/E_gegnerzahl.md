# Runde 36 Spur E — gegnerzahl (20 Gegner-Plaetze je Raum wie das Original)

Baum `.claude/worktrees/r36_gegnerzahl`, Zweig `r36/gegnerzahl`, Basis master 78a4a748.
Fortlaufend geschrieben; nach jedem Schritt committet.

## 0. Auftrag (AUFTRAG.md Z.15-20, VERTRAG §0 Spur E, woertlich)

> Zu den grenzen: [...] "Der Port verwaltet höchstens 15 Gegner je Raum, das Original 20. In ROOM1040 und 1030 fehlen
> deshalb die letzten fünf Zombies, das war auch vorher schon so." --> Das auch!

VERTRAG Spur E: "Grenze 2: Port fuehrt 15 Gegner je Raum, Original 20 (FUN_80037250 @0x800372b4 `sltiu 0x14`;
Sce_em_set @0x800420dc ohne Grenzpruefung): RE15_ACTOR_MAX anheben, alle Nutzer zensieren, ROOM1040/1030 Records
15..19 erscheinen; Voll-Suite" (Voll-Suite faehrt laut Arbeitsanweisung der Orchestrator; hier gezielte Laeufe).
Eigentuemer laut VERTRAG §1.4: "E = RE15_ACTOR_MAX + alle Arrays".

## 1. Arbeitsprotokoll (chronologisch)
- [start] Baum sauber auf 78a4a748; AUFTRAG/VERTRAG gelesen; L_cut1150.md §10.1/§10.5 (Vorrunde) gelesen.

## 2. Messung vorher
(folgt)

## 3. RE-Belege (Adressen, Bytes)

Alle Zeilen selbst disassembliert (`re15_disasm.py dis`, PSX.EXE des Auslieferungsstands `info/Re1.5/PSX.EXE`).

### 3.1 Sce_em_set (Opcode 0x44, Handler 0x800420a0) legt den Slot OHNE Grenzpruefung ab
```
800420ac lui   a2,0x800b
800420b0 addiu a2,a2,-14468      a2 = 0x800ac77c
800420dc addiu a0,a2,1200        a0 = 0x800acc2c   (Gegner-Feld)
800420f0 lbu   v0,1(a1)          pc[1]
800420f8 andi  s4,v0,0x7f        Slot = pc[1] & 0x7f   (0..127, KEIN Vergleich gegen 20)
800420fc andi  s1,v0,0x80
80042100 sll v0,s4,5 / 80042104 subu / 80042108 sll 2 / 8004210c addu / 80042110 sll 2   -> s4*500
80042118 addu  s0,v0,a0          Entity = 0x800acc2c + Slot*0x1F4
```
Danach Tot-Bit-Gate (`80042120 lbu a1,7(a1)` / `80042128 beq a1,0xff` / `80042130 jal 0x8004efe4` / `80042138 beq v0,zero`)
und Gleichzeitig-Limit (`800421d8/dc` a1 = 0x800b0ff2 / `80042214 lh v0,0(a1)` / `8004221c lh v1,4084(v1)` = 0x800b0ff4 /
`80042224 slt` / `80042228 bne` / voll: `80042230 ori v0,zero,0x8000` + `80042238 sw v0,0(s0)` / frei: `8004223c sh v0,0(a1)`).
Erfolgreicher Spawn zaehlt den Aktiv-Zaehler hoch: `8004254c lbu v0,-13746(v0)` (0x800aca4e) / `80042558 addiu v0,v0,1` /
`80042560 sb v0,-13746(at)`.

### 3.2 Das Gegner-Feld hat 20 Plaetze (drei unabhaengige Belege)
1. **Ziel-Umschaltung FUN_80037250** (naechster Gegner fuer die Zielerfassung) laeuft das Feld mit Umbruch bei 20 ab:
   `800372ac addiu a2,a2,1` / `800372b0 andi v0,a2,0xff` / `800372b4 sltiu v0,v0,0x14` / `800372b8 bne v0,zero,...` /
   `800372bc addiu a1,a1,500` / Umbruch `800372c0 addu a2,zero,zero` + `800372c4 lui a1,0x800b` + `800372c8 addiu a1,a1,-13268`
   (= 0x800acc2c). Decompilat RE_15_Quellcode_V2/FUN_80037250.c: `if (0x13 < (uVar3 & 0xff)) { uVar3 = 0; puVar2 = &DAT_800acc2c; }`.
2. **Das naechste Feld beginnt genau bei 0x800acc2c + 20*0x1F4 = 0x800af33c**: FUN_80039b2c (NPC-Licht-Init, Aufruf aus
   Sce_em_set @0x80042144) rechnet `80039b34 lui v1,0x800b` / `80039b38 addiu v1,v1,0x11cc` / `80039b40 addiu v1,v1,-0x1e90`
   = 0x800af33c, Stride 0x6C (`80039b58..80039b68`), Zaehler DAT_800afbb7; FUN_80039ca0 (NPC-Render+Licht) liest dieselbe
   Basis (`80039cb4 lui s3,0x800b` / `80039cb8 addiu s3,s3,-0xcc4` = 0x800af33c; ghidra1_V2.txt:138186 `lw v0,0x0(s3)=>DAT_800af33c`).
   Ein Slot 20 laege also schon IN diesem Feld.
3. Der Opcode 0x53 Art 2 (LAB_80040e18) nimmt dieselbe Basis (`80040e60 addiu v1,v1,4048` / `80040ec0 addiu v1,v1,-17316` =
   0x800acc2c, idx*500 @0x80040eb0..80040ebc); ROOM1030 sub00 @0x020CE..@0x0216E benutzt ihn in einer For-20-Schleife
   (Spur L r35, L_cut1150.md §10.1).

### 3.3 Die Pro-Bild-Schleife FUN_8001a50c ist NICHT an 20 gebunden, sondern an den Aktiv-Zaehler
```
8001a524 lui s1,0x800b / 8001a528 lbu s1,-13746(s1)   s1 = 0x800aca4e (Spawn-Zaehler, u8)
8001a52c addiu v0,v1,1192 / 8001a540 sw v0,0(v1)      Zeiger = 0x800acc2c
8001a55c lw v0,0(v1) / 8001a564 andi v0,v0,0x1 / 8001a568 beq v0,zero,...   inaktiv -> naechster Platz, NICHT gezaehlt
8001a570 lbu v0,8(v1) / ... / 8001a588 jalr v0 (0x80072bac[Typ]) / 8001a58c addiu s0,s0,1 (Delay-Slot: nur aktive zaehlen)
8001a5a8 addiu v0,v0,500 / 8001a5b0 andi v0,s0,0xff / 8001a5b4 sltu v0,v0,s1 / 8001a5b8 bne ...
```
Die Schleife tickt so viele AKTIVE Plaetze, wie Spawns gezaehlt sind — bei hoechstens 20 Records also genau das Feld.

### 3.4 RDT-Zensus der Sce_em_set-Slots (Werkzeug `re15_port/tools/r36_e/sce_em_slot_zensus.py`, Walker = aot_sce_census.py)
`files=240 Sce_em_set-Records=844 walk-stops=0`; Slot -> Records: 0:245 1:178 2:101 3:77 4:48 5:23 6:23 7:14 8:12 9:12 10:9
11..19: je 8, 20..34: je 2; max Slot 34.
| Raum | Records | Slots | Slot >= 15 (bisher verworfen) | Limit Save(0x12,n) |
|---|---|---|---|---|
| ROOM1030 / 1031 | 20 / 20 | 0..19 | 15..19 (Typ 0x16, Tot-Bits 0x0F..0x13) @0x1f12..@0x1f62 / @0x1f88..@0x1fd8 | 6 @0x1de2 / @0x1e58 |
| ROOM1040 / 1041 | 40 / 40 (zwei Zweige je 20) | 0..19 | 15..19 je Zweig (Typ 0x16, Tot-Bits 0x23..0x27) | 5 @0x11f0 / @0x1218 |
| ROOM20B0 / 20B1 | 35 / 35 (sub00, lueckenlos) | 0..34 | 15..34 (Typ 0x24 FX-Emitter, Tot-Bit 0xFF) | keins |
Dazu Work_set (0x2E) Art 2 auf die Gegner 15..19: ROOM1030 sub04 @0x2416..@0x2496 und sub06 @0x26b8..@0x2738, ROOM1031 sub04/06,
ROOM1040 sub09 @0x1d4e..@0x1e16 und sub10 @0x20e8..@0x2198, ROOM1041 sub09/10 — im Port bisher ins Leere (Aktor >= 16).
Kein anderer Raum benutzt Slot >= 15.

**ROOM20B0/20B1 (Slots 20..34):** im Original laegen diese Entities hinter dem 20er-Feld (0x800af33c..0x800b1087: NPC-Feld
0x800af33c, ..., SCD-Variablen 0x800b0fd0, Limit 0x800b0ff2/0x800b0ff4, Tot-Bit-Zonen 0x800b1038/0x800b1058) — ein
Ueberlauf des Prototyps, kein 21. Platz. Der Port verwirft sie weiter (Aktor > 20); dokumentiert unter OFFEN.

## 4. Zensus der Nutzer von RE15_ACTOR_MAX / festen 16/15 / Slot-Bitmasken
(folgt)

## 5. Umsetzung (Dateien, Konstanten)
(folgt)

## 6. Messung nachher
(folgt)

## 7. Tests (Namen, Ergebnis)
(folgt)

## 8. OFFEN
(folgt)

## 9. Fuer den Nutzer
(folgt)
