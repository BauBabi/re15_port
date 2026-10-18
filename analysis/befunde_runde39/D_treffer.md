# Runde 39 Spur D "treffer" — Dossier

Baum `.claude/worktrees/r39_treffer`, Zweig `r39/treffer`, Basis c71fa144 (master nach Welle 1).

## Wortlaut des Nutzers (AUFTRAG.md)

1. "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht."
2. "Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein"
3. "Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte, zum Beispiel wenn sie um die
   Ecke sind und nicht mal ansatzweise in Leons Blickfeld mit Wand davor."

## Stand

- [x] Schritt 0: Lektuere (AUFTRAG/VERTRAG, Runde-16-Dossiers aufstehen-schuss/trefferhoehe/liegende-und-aufstehen,
      Runde-38 c939ce5d Zielhilfe/Band-Tore, Memory hit-latch-93 / tiefe-aus-kollision / kollisionszellen-sind-waende)
- [x] Schritt 1: RE-Belege Mechanik (RE1.5 + RE2, alles selbst disassembliert/gedumpt, s. "RE-Belege")
- [ ] Schritt 2: Messung vorher je Punkt (Sonde + echte exe)
- [ ] Schritt 3: Umsetzung
- [ ] Schritt 4: Messung nachher, Tests, Abschluss

## RE-Belege (Mechanik)

Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5 PSX.EXE), `re2_disasm.py` (RE2
`info/re2leon/PSX.EXE`, Overlay `--bin EMOVL10_S0.BIN` = byte-identisch zu `EMZ0.BIN`, cmp 53068 B), eigene
Python-Dumps der RDT-Kollisionsbloecke (RE1.5 `shared_assets/PSX/STAGE*/ROOM*.RDT` @0x20, RE2 `info/re2leon/PL0/RDT`).

### B1 — RE1.5: Sichtlinie des Spieler-Schusses = Zellen mit Art-Nibble 3 (2D, keine Hoehe)

Resolver `FUN_80011f50` ruft nach der Kandidatenwahl `jal 0x8001b9b4` (Gewinner-Position, +0x34) — Ergebnis != 0 ->
`return 0` (kein Treffer). `FUN_8001b9b4` @0x8001b9b4-ba50: Delta Ziel - Spieler (`lw v1,52/56/60(a1)` @0x8001b9dc-ba04),
Schleife s0 = 3..0 (`ori s0,zero,0x3` @0x8001ba08, `addiu s0,s0,-1` @0x8001ba38) je Region `jal 0x8003dcc4` mit
`ori a2,zero,0xf00` @0x8001ba1c / `ori a3,zero,0x300` @0x8001ba24, ODER der Ergebnisse (`or s1,s1,v0` @0x8001ba28).
`FUN_8003dcc4` (RE_15_Quellcode_V2/FUN_8003dcc4.c): je Zelle der Region (Zeiger `*(rdt+0x20) + 8 + region*4`), Filter
`(w10 << 16 >> 28) == Spieler+0x82` (Band) UND `(w10 & a2) == a3` = `(w10 & 0xf00) == 0x300`; w10 = `u1 | floor<<8`
(re15_rdt.h `re15_sca_entry_t`: width,density,x,z,type,u0,u1,floor), also **`(floor & 0x0f) == 3`**. Geometrie: beide
Diagonalen der Zelle gegen den Strahl (OuterProduct0-Vorzeichen), alle Koordinaten /0x12. KEIN Hoehentest.
Zweiter Aufrufer derselben Routine mit anderer Art: `FUN_8001bafc` (Schraegen-Test des Resolvers) `FUN_8003dcc4(.., 0x400,
0x400)` = Art-Bit 2. Die Art-Bits sind also ein Zellen-ATTRIBUT der Designer (Bit0+Bit1 = sperrt Sicht/Schuss, Bit2 =
Schraege). Port: `re15_los_ray_blocked` (enemy_ai_common.c:2943, `(c->floor & 0x0f) != 3 -> continue`), Spieler-Schuss
`re15_schuss_r36_sichtlinie_blockiert` (schuss_r36.c, Runde 36 C) — byte-true RE1.5.

Zellen-Zensus (eigener Dump, alle Stages, (shape,u0,u1,Art)): 14826 Zellen Art 3, Art 2 u.a. ROOM1020 [2],[3],[11],[13],
[33] (Moebelbloecke 14230x1824 / 10340x3138 / 10482x3304, band 0) — die Designer haben in 1020 Moebel als NICHT
schuss-sperrend (Art 2) markiert. **ROOM1140 (Briefing, Zombies 0x10/0x11/0x16): alle 19 Zellen Art 3 (floor=0x03)**,
darunter die Tischbloecke [3] 22450x9550 @(-5750,-7800), [6] 12100x6100 @(-4650,-17450), [5] 10250x2850 @(4450,-23600),
[9] 19250x1400 @(-4550,-9200), [11]/[12]/[13]/[10]/[7]/[8] und 5 Kreise 1000x1000 (Stuehle). D.h. im RE1.5-Original
sperrt JEDER Tisch in 1140 den Schuss (2D-Strahl, Art 3) — unfertiger Datenstand der Beta (RE2 vgl. B2).

### B2 — RE2: Sichtlinie = Designer-Flag 0x80 je Zelle + Hoehentest; GLEICHE Routine fuer Schuss UND Zielhilfe

`FUN_80050858(a0 = Punkt A xyz, a1 = Punkt B xyz, a2 = Flagmaske, a3 = Hoehentest)` (RE2_Quellcode_V2/FUN_80050858.c,
selbst disassembliert): SCA-Zeiger `lw t1,32(v0)` @0x80050890 (RDT+0x20, wie RE1.5), Zaehler `lw v0,4(t1)` @0x80050898,
Zellen 16 B ab +16 (`addiu t1,t1,16` @0x800508b0); je Zelle `lhu v1,8(t1)` @0x800508bc / `and v0,v1,fp` @0x800508c4 /
`beq v0,zero,0x800508b0` @0x800508c8 = **ohne Flagtreffer wird die Zelle uebersprungen**; Formtabelle `DAT_800a73b4`
@0x800508d0-e4 (Formen 1-5,13 nie), dann XZ-Kreuzungstest (Rechteckkanten), bei a3 != 0 zusaetzlich YZ/YX-Kreuzung mit
der Zellenhoehe: oben = -((type>>11)*100 + ((type>>6)&0x1f)*1800), unten = -1800*ctz(floormask).
Aufrufer (ghidra_re2_Leon.txt XREF): **Schuss-Applier FUN_800410CC** `addiu a2,zero,128` @0x800412e0 / `addiu a3,zero,1`
@0x800412ec / `jal` @0x800412f0 — VOR der Geometrie, je Kandidat (Decompile: `iVar3 = FUN_80050858(local_30,&local_a0,
0x80,1); if (iVar3 == 0) {...}`); **Zielhilfe FUN_80042144** `jal` @0x800422a8 (Decompile `FUN_80050858(Spieler-Teil
+0x1C1 +0x5c, &local_38 = (Gegner+0x84, Teil-y, Gegner+0x8C), 0x80, 1) == 0` als Kandidatenbedingung; danach Vorrang
stehend (`+0x10E & 0x2000 == 0`) vor liegend, naechster gewinnt). Weitere Aufrufer: Navigator 0x8004a8d8 (Maske 0x2000),
FUN_8003d96c/db38 (0x2080), Kraehe 0x8400.
RE2-Zellenzensus (eigener Dump aller `PL0/RDT/ROOM*.RDT`, 16-B-Satz x,z,w,d,id,type,floormask): ~16 % der Zellen OHNE
Flag 0x80 (z.B. 480 von 3283 Rechtecken, dazu 54 Kreise, 123 Form 7, 118 Form 8), Hoehe fast immer 1800 (type>>11 = 18).
**RE2 loest "ueber Moebel schiessen" also ueber das Designer-Flag je Zelle, nicht ueber die Hoehe.** Beispiele ohne Flag
im RPD 1F: ROOM1030 [34]/[35] 1155x13084, ROOM1050 [19] 3690x3490, ROOM1160 [17] 5340x1160, ROOM1190 [4] 10414x4634.

### B3 — Zielhilfe: RE1.5 OHNE Sichtlinie, RE2 MIT

RE1.5 `FUN_8003703c` @0x8003703c-801371a0 (selbst disassembliert): je aktives Wesen `SquareRoot0(dx^2+dz^2)`
@0x800370f8, `lbu a0,-145(s0)` +0x09 / `andi 0x60` @0x8003710c (versteckt), `lh v0,0(s0)` +0x9A HP `bltz` @0x80037120
(Eimer hp<0), `andi a0,0x80` @0x80037124 (Eimer liegend), sonst naechster (`sltu v0,v1,s5` @0x80037130). **Kein
Aufruf von 0x8001b9b4/0x8003dcc4, keine Sichtlinie.** Port `re15_player_aim_target` (re15_damage.c:1075) = dito.
RE2 `FUN_80042144` prueft je Kandidat die Sichtlinie (B2). Beta -> Retail: RE2 ist das Ziel, der Nutzerpunkt 3 ist
exakt das RE2-Verhalten.

### B4 — RE2: Zielhoehe ist MANUELL (D-Pad), keine automatische Nachfuehrung zum Ziel

Ziel-FSM (Halten): `andi v0,s2,0x20` @0x80043588 (Pad TIEF) -> `+0x154 = (+0x154 & 0x1fff) | 0x2000` @0x8004359c-a8;
`andi v0,s2,0x10` @0x800435e0 (Pad HOCH) -> `| 0x8000` @0x800435f8; beide erst ab Bild `DAT_800a6d78[(Waffe-1)*3]&0x7f`
des Halte-Clips (`lbu v1,333(s0)` @0x80043578/d0). Die Bits 29-31 des Spieler-word0 (Geometriegruppe des Appliers
`srl s2,v0,29` @0x80047d18) setzt `FUN_80042368` @0x80042368-246c beim Schuss aus der HANDHOEHE: a1 = Pool[Teil 14].ty
(`lw a1,2504(v0)` @0x80043a28, Waffe 2: Teil 11 `lw a1,1988(v0)` @0x80043a10), Schwelle `(Spieler-y + DAT_800a6dcc[Figur]
[Waffe]) * +0x1DE >> 12` + `DAT_800a6dc8[Waffe]` (lo/hi, Dump: Pistole -200/300, h Leon -2427 / Claire -2120),
`slt v0,t1,v0` @0x80042418 -> UP 0x80000000, `slt v0,v0,t1` @0x80042444 -> DOWN 0x20000000, sonst LEVEL 0x40000000.
Es gibt keinen Leser des Zielzeigers +0x1B4, der die Hoehe/Pose nachfuehrt (Vollsuche `0x1b4)` in RE2_Quellcode_V2:
nur Setzer in FUN_80042144/8003d96c/80045c10). => In RE2 muss der Spieler fuer liegende/aufstehende Zombies selbst
TIEF zielen (bekannte RE2-Mechanik).

### B5 — RE2: Teile-Maske waehrend des Aufstehens (Runde 16 trefferhoehe.md, hier nachgelesen)

EXEC[5] P7 @0x80103628-754 (EMOVL10_S0.BIN): Radius +0x9A/+0x9C/+0x90/+0x92 +10 bis 500 @0x80103628-68, Advance
`jal 0x8002959c` a3=256 @0x801036a0, fertig -> P8; sonst `lbu v1,333(s2)` / `addiu v0,zero,55` / `bne v1,v0,0x80103754`
@0x801036d0-d8 -> NUR bei Clip-Bild 55 `lui v1,0xc00 / or / sw v0,0(s2)` @0x801036dc-f0 (Maske Beine+Rumpf zurueck).
Vorher (P0..P7 < Bild 55) Maske = nur Beine (0x04000000, @0x801032B8-EC). Applier-Zeile EBEN bei dy=0: row 3, Satz
`DAT_800a6db4+9+3` = [02 00 00] -> Treffer nur mit Rumpf-Bit -> **EBEN trifft den Aufsteher in RE2 erst ab Bild 55**,
TIEF (row 6 = [01 02 00], Beine zuerst) trifft ihn die ganze Zeit (Nahbox < 4100). Port: `re2z_frame_slot(e) == 55`
(enemy_ai_re2_zombie.c:2508) und `re15_re2_gun_probe` (re15_damage.c:1933) — nachzumessen (Schritt 2).

## Messung vorher

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
