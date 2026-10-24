# Runde 41 — Spur J "stage3szenen_a" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_stage3szenen_a`, Zweig `r41/stage3szenen_a`, Basis 4c667c97 (+ ae8ea0a7 AUFTRAG A-E3..A-E5).
Start 2026-10-24 (Uhr siehe Commit-Zeit). Kein Vorgaenger-Stand dieser Spur im Baum (0 eigene Commits beim Start).

## Punkte (Wortlaut, AUFTRAG.md)
1. "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060."
2. "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von
   dieser Cutscene hat Elza auch erneut eine falsche Animation."
0. (Orchestrator-Entscheid A-E5, 2026-10-24 04:01) ERSTER Schritt: RBJ-Binder FUN_8001b3f8 byte-true bauen, in allen 12
   abweichenden Raeumen zur Laufzeit messen (welcher Record je Slot, vorher/nachher), eigener Commit mit `rbj-binder` in
   der ersten Zeile. Spur K uebernimmt ihn per cherry-pick unveraendert.

## Plan
- §0 RBJ-Binder: Disasm FUN_8001b3f8 -> Port rbj_resolve_slot / re15_emd_parse_rbj -> Laufzeitmessung 12 Raeume -> Commit `rbj-binder`.
- §1 3060 Marvin: SCD-Disasm ROOM3060 (Marvin-Abschnitt-Ende), Port-Trace (RE15_ANIM_TRACE/RE15_SCD_TRACE) + Framedump, Vergleich.
- §2 3061 Sherry-Schacht + Elza-Ende: SCD-Disasm ROOM3061, Port-Trace mit Elza, Vergleich.

## Laufendes Protokoll
(neueste Eintraege unten je Abschnitt)

## §0 RBJ-Marker-Binder FUN_8001b3f8 (A-E5, erster Schritt)

### §0.1 RE-Beleg (selbst disassembliert: `re15_disasm.py dis 0x8001b3f8 48` aus info/Re1.5/PSX.EXE)
Rohbytes @0x8001b3f8: `0b 80 02 3c 78 c7 42 8c 00 00 00 00 5c 00 46 8c 00 00 00 00 33 00 c0 10 21 38 00 00 ...`
| Adresse | Instruktion | Bedeutung |
|---|---|---|
| 0x8001b3fc / 0x8001b404 | `lw v0,-14472(v0)` (DAT_800ac778) / `lw a2,92(v0)` | RBJ-Zeiger = RDT-Kopf +0x5C |
| 0x8001b40c / 0x8001b410 | `beq a2,zero,0x8001b4dc` / Delay `addu a3,zero,zero` | kein RBJ -> nichts; **a3 = 0 EINMAL vor allen Records** |
| 0x8001b41c / 0x8001b424 / 0x8001b420 | `lw v0,0(a2)` / `addu t0,a2,v0` / `lbu t2,4(a2)` | Trailer = RBJ + u32@0; Record-Zahl = Byte @+4 |
| 0x8001b42c..38 | `lw v0,0(t0)` / `addu v1,a2,v0` / `lw a0,0(v1)` | Marker-u32 am EMR-Praefix des Records |
| 0x8001b440 / 0x8001b444 / 0x8001b448 | `andi v0,a0,0x1` / `beq` / `srl a0,a0,1` | Bit 0 = Spieler |
| 0x8001b450 / 0x8001b460 | `sw v0,0(t3)` = 0x800acbd4 / `sw v0,4(t3)` = 0x800acbd8 | Spieler-Kanal +0x180/+0x184; JEDER Record mit Bit 0 schreibt -> **der letzte gewinnt** |
| 0x8001b464 | `beq a0,zero,0x8001b4c8` | keine Gegner-Bits -> a3 bleibt |
| 0x8001b470..78 | `andi v0,a0,0x1` / `beq v0,zero,0x8001b4c0` / `srl a0,a0,1` | je Bitposition |
| 0x8001b47c..90 | `andi v0,a3,0xff` ... `sll v1,v1,2` | Index a3&0xff * 0x1F4 (Entity-Stride) |
| 0x8001b4a0 / 0x8001b4bc | `sw t1,0(at)` = 0x800acdac+i*0x1F4 / `sw v0,0(at)` = 0x800acdb0+i*0x1F4 | Gegner-Kanal +0x180/+0x184 |
| 0x8001b4c0 / 0x8001b4c4 | `bne a0,zero,0x8001b470` / Delay `addiu a3,a3,1` | **a3++ je Bitposition (auch 0-Bits unter dem hoechsten), NIE je Record zurueckgesetzt** |
| 0x8001b4c8..d8 | `addiu a1,a1,8` / `addiu t2,t2,-1` / `bne` / `addiu t0,t0,8` | naechster Record |
Aufrufer: Raumlader `jal 0x8001b3f8` @0x80039a08 (nach FUN_8003ef6c @0x80039a00). Einzige Schreiber der Kanaele:
ghidra1_V2.txt XREF DAT_800acbd4/acbd8/acdac/acdb0 je genau 1 (W) aus FUN_8001b3f8; kein `sw ..,0x180(..)` auf Entities
sonst im EXE (einziger Treffer 0x8002e3a0 = SPRT-Puffer). Sce_em_set (@0x800420a0, Tabelle @0x800745b8) loescht +0x180 nicht.

Port vor Runde 41: `rbj_resolve_slot` (enemy_common.c) = ERSTER Record mit Marker-Bit (1+i); Spieler = Record 0
(`re15_apply_room_cinematic` -> `re15_emd_parse_rbj`).

### §0.2 Zensus (statisch, eigenes Werkzeug re15_port/tools/r41_j/rbj_binder_zensus.py = Nachbau der Schleife oben)
Ergebnis deckungsgleich mit Spur A (A_belege/nb1d/rbj_a3_zensus.txt): 240 Raeume, 47 mit RBJ, 12 abweichend:
1040/1041 (Marker 0x7e: Spieler orig KEIN Record, Port rec0), 2000/3091/30E0/4000/4001/4010/5011/6030/6031 (Marker
0x1/0x2/0x3: Spieler orig rec2 / Port rec0; Gegner 1 orig rec2 / Port ungebunden), 3060 (0x1/0x2/0x4: Gegner orig
{0:1, 2:2} / Port {0:1, 1:2}).
Inhalt (rbj_zensus.py json, Clip-Hashes): 2000/30E0/4000 rec0 == rec2 (25/25 Clips gleich) -> Spieler unveraendert;
4001/4010/5011 rec2 = rec0 + Zusatzclips (11->14, 25->27, 20->21); **3091/6030/6031 rec0 = 1 Clip mit 2 Bildern
(Platzhalter), rec2 = die echten 14/22/20 Clips** -> Spieler bekommt dort erstmals die Szenen-Gesten (Spur K 3091).
1040/1041: kein Plc_motion im ganzen SCD (scd_dump_room.py) -> Spieler-Kanal ungenutzt.
