# Runde 38 — Spur M "npckante" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_npckante`, Zweig `r38/npckante`, Basis f587d34e.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "Kendo springt am Ende von ROOM 10B0 nicht die Kante runter, wie im Original, sondern läuft in die Luft weiter"
2. "Bei Room 1260 Springt Kendo auch nicht die Kanten runter. Machen Marvin und Ada in Leons Szenario übrigens auch immer nicht. Das ist falsch."

Lesart (AUFTRAG "Lesarten"): Elza-Szenario -> ROOM10B1 (John = Typ 0x49, Kendo-Modell aus CDEMD1) und ROOM1261;
Leon-Szenario -> Marvin (0x40) und Ada (0x42) in ROOM1260 und wo sie sonst Hoehenkanten queren (Zensus unten: ROOM2000).

## Stand (Schritte)
- [x] Pflichtlektuere (AUFTRAG/VERTRAG, Memory cutscene-npc-system / npc-ai / npc-global-yield / npc-crossfade-decay)
- [x] Messung vorher (exe, Lade-Weg, alle drei Raeume)
- [x] RE-Belege (Sonde FUN_8002d7d8, Kanten-Sub 0x80052508, Sprung-Sub 0x80052c14, Schwerkraft FUN_8001bd60)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## Messung vorher (Stand f587d34e + nur Messhaken RE15_NPCKANTE_LOG)

Werkzeug: Messhaken `re15_npckante_spur` (engine/src/npckante_r38.c, env RE15_NPCKANTE_LOG, je NPC-Tick: Lage, y, Band
+0x82, Boden-Y +0x1ba, State/Sub/Phase, Clip/Bild, Kanal, Sonden-Ergebnis, "boden" = room_coll unter dem NPC in seinem
Band). Spielstand-Werkzeug `probe_r38_npckante_karte` (Raum, x, **y**, z, Gier, **Kamera-Cut**, Flags), Lauf ueber
CONTINUE (Lade-Weg) und `RE15_FIRE_AOT` (= Tuer betreten, derselbe Pfad wie Hineinlaufen). Skript
`M_belege/messlauf.sh`, Auszuege `M_belege/vorher_*`.

| Lauf | Weg | Befund |
|---|---|---|
| Elza 1261 | Karte 10B1 (-21370,0,3000) Cut 7, Flag (3,0x41) -> Tuer Slot 1 (@0x016F2) -> ROOM1261 Cut 15 | John (Slot 1) geht auf dem Steg (y -1800, Band 1) nach Sueden; **F143** (-20477,-1800,-17374): Sonde = 2 (Absprungkante); der Port geht weiter, ab F156 `boden=0`: **298 Bilder in 1800 Hoehe ueber dem Boden** (bis F400), laeuft unten die ganze Laenge nach Osten in der Luft (z -19224, x -19155 -> 19845), y bleibt -1800 |
| Leon 1260 | Karte 11B0 (-60,0,-18900), Flags (4,0xF4)(4,0xCD) -> Tuer Slot 5 (@0x01034) -> ROOM1260 Cut 7 | Ada (Slot 1, 0x42): F53 (26750,-1800,14500) Sonde = 2, **241 Bilder in der Luft** (F63..F304); Marvin (Slot 2, 0x40): F118 Sonde = 2, **224 Bilder in der Luft** (F128..F352); beide laufen in y -1800 ueber den Kanal und "landen" erst wieder ueber dem Mittelblock |
| Elza 10B1 | Karte 10B1 (0,-9000,-12000) Band 5, Cut 2, Flags (3,0x80)(3,0x82) (= Stand nach ROOM1171 sub02 @0x0139C/@0x013A4) | John startet auf dem Band-5-Steg (15600,-9000,-9884), laeuft sub02; **F115** (-3661,-9000,-8275): Sonde = 2 (Zelle [31], Band 5, u1 0x0e); der Port laeuft in y -9000 weiter bis (-6419,-9000,-7759) = **7200 ueber dem Block [6]** |

(Ohne die Tuer-Flagge (4,0xCD) ist ROOM11B0 Slot 5 der gesperrte Satz @0x01010 — der erste 1260-Lauf blieb in 11B0.)

## RE-Belege (info/Re1.5/PSX.EXE + STAGE*.BIN, selbst disassembliert mit re15_disasm.py)

### 1. Wer ruft die Kantensonde — nur die NPC-Plc_dest-Subs 4 und 5
- NPC-Executor-Tabelle @0x80076ca0: [4]=0x80051148 (Gehen), [5]=0x80051484 (Laufen), [18]=[22]=0x80052508,
  [23]=[24]=0x80052c14, [20]/[21] = `jr ra`-Stummel (0x80052c04/0x80052c0c).
- Sub 4 Phase 2: `jal 0x800245d8` @0x80051344 (Schritt) -> anim_set @0x80051360 -> **`jal 0x8002d7d8` @0x80051374**
  -> `sb v0,159(v1)` @0x80051388 (+0x9f) -> `!= 0`: `ori v1,zero,0x12` / `sb v1,5(v0)` @0x80051394/@0x800513a4,
  `sb zero,6(v0)` @0x800513b4, Exit (Ankunftstest uebersprungen).
- Sub 5 Phase 2: Schritt @0x800516b0 -> **`jal 0x8002d7d8` @0x800516c4** (VOR anim_set @0x8005171c) -> +0x9f
  @0x800516d8 -> `!= 0`: +0x5 = 0x16 @0x800516e4/f4, +0x6 = 0 @0x80051704, Exit.
- XREFs von FUN_8002d7d8 im EXE-Dump: genau 0x80051374 und 0x800516c4 (ghidra1_V2.txt:120680). Byte-Suche nach
  `f6 b5 00 0c` (jal 0x8002d7d8) in STAGE1..6/DEBUG/TITLE.BIN: 0 Treffer. Die Rueckwaerts-Subs 7/8 (0x80051908/
  0x80051b00), Sub 9 (0x80051cf8) und die Spieler-Walker 0x80030af0/0x80030d28 rufen sie NICHT (`scan`).

### 2. Die Sonde FUN_8002d7d8
- Vektor {800,0,0,0} @0x800109c8 (`read`: [800, 0, 0, 0]), lwl/lwr-Kopie @0x8002d7fc-820.
- Kurs gerastet: `lhu a0,106(s2)` / `addiu a0,a0,512` / `and a0,a0,-1024` / sll/sra 16 @0x8002d82c-844,
  FUN_8004f008 = RotMatrix 0x80068098 + ApplyMatrixSV 0x800661c0 -> Punkt = Lage + 800 * (cos q, -sin q).
- Zellwort: `jal 0x8003b7f0` @0x8002d874 (a1 = 0, a2 = `lbu 130(s2)` = +0x82). FUN_8003b7f0 liefert das Halbwort
  Zelle+0x0a (= u1 | floor<<8) der ersten Zelle des Quadranten mit Band (sign. Nibble Bit 15..12) == +0x82, die den
  Punkt enthaelt (@0x8003b8a0-904), sonst 0.
- Bit 0 (@0x8002d884): Objektpool 0x800b3f98 (Stride 148, Zahl = RDT-Kopf Byte 2), Wort0 & 1: FUN_8002da4c(Punkt, Obj,
  box.hx>>1, Band+1) @0x8002d8fc oder (.., Band-2) @0x8002d928 -> 0; sonst **1** (@0x8002d944).
- sonst: FUN_8002da4c(Punkt, Obj, Entity-Box(+0x78)->+6, Band) @0x8002d9b0 / (.., Band-1) @0x8002d9d4 -> 0;
  Bit 1 (@0x8002d9f0): `jal 0x8001c6e8` @0x8002da04 (Punkt, a1 = 1, Band, a3 = 0x100) = room_coll; `slt v1,y,h`
  @0x8002da14 -> **2**.

### 3. Kanten-Sub 0x80052508 (Phasen +0x6, Sprungtabelle @0x800112e4, 5 Eintraege)
[0] @0x80052548 +0x8c = 0x4b, +0x6 = 1, +0x94 = 5, +0x95 = 0, +0x8f = 7 -> [1] @0x80052598 (Schritt-SE 0x4000,
anim_set(+0x170,+0x174) @0x800525f0, Kurs rasten @0x80052604-2c, `andi 0x3e0` @0x80052644 -> +0x6 = 2; `lb +0x9f == 2`
@0x80052660-68 -> +0x5 = (+0x5 == 0x12) ? 0x18 : 0x17 @0x80052670-84, +0x6 = 0) -> [2] @0x80052688 (+0x94 = 2) -> [3]
@0x800526c8 (VOR anim_set: +0x95 == 0x11 -> +0x93 |= 1; **+0x95 == 0x1d -> +0x8c = 0x47c, pos_advance, y -= 1800
@0x80052730, +0x1ba -= 1800 @0x8005274c, +0x82 += 1 @0x80052768, +0x40/42/44 = Lage**; +0x95 == 0x25 -> +0x93 &= 0xfe;
anim_set(+0x84,+0x16c) @0x800527f8, +0x6 += Clip-Ende) -> [4] @0x80052820 +0x5 = (+0x5 == 0x12) ? 4 : 5.

### 4. Sprung-Sub 0x80052c14 (Phasen +0x6, Sprungtabelle @0x8001131c, 7 Eintraege)
[0] @0x80052c54 +0x94 = 3 -> [1] @0x80052c94 anim_set(+0x84,+0x16c), danach +0x95 == 0xd -> +0x6 = 2 -> [2] @0x80052cd4
+0x93 |= 1, +0x94 = 4, +0x8f = 0, +0x9c = 0, **+0x8c = 0x320 + pos_advance, y += 500, +0x1ba = room_coll(&Lage, 0,
+0x82, 0x100) @0x80052d84, +0x82 = -(+0x1ba/1800) (Kehrwert 0x91a2b3c5 @0x80052da4-d0)**, +0x40/42/44 = Lage -> [3]
@0x80052e1c **t = +0x9c++; y += 90 + 50*t**; anim_set; (+0x1ba - 800) < y -> [4] @0x80052ea8 +0x94 = 5, +0x8f = 0,
**y = +0x1ba**, +0x93 &= 0xfe -> [5] @0x80052f50 anim_set, +0x6 += Clip-Ende -> [6] @0x80052f8c +0x5 = (+0x5 == 0x18) ? 4 : 5.

### 5. Die NPC-Wurzeln: Schwerkraft FUN_8001bd60 + Schatten auf +0x1ba
Alle 17 NPC-Wurzeln (STAGE1 0x40/42/45/47/49/4b @0x8011c5a0/cb70/d140/d6d4/dc68/e22c; STAGE2-6 je 0x40/42/49/4b, STAGE5
0x4d; Registrierungen `_DAT_80072cac..ce0` in STAGE*_overlay.c) rufen (Scan): `jal 0x8001bd60` (a0 = -10, a1 = 0x14;
0x49: @0x8011dcc4, a0 @0x8011dcb4, a1 @0x8011dcac) VOR dem Dispatch @0x8011dd08, `jal 0x8003b0a4` (Klemme, Maske 4) nach
dem Dispatch, und `lh a1,442` + `jal 0x8001b064` (Schatten auf **+0x1ba**) @0x8011dd78/7c. FUN_8001bd60 = die im Port als
re15_schwerkraft_8001bd60 portierte Engine-Schwerkraft (trage_1200.c, Runde 35 Spur H): Zelle UNTER dem Aktor
(Radius -r) mit Bit 1 und y == -(Band*1800) -> Fall ueber 1 + ((u1>>2)&3) Baender, y += -10 + 20*t bis +0x1ba. Im Port
nur fuer die Zombie-Wurzeln und die Zombie-Dame verdrahtet (enemy_ai_common.c Kommentar "[hier noch nicht portiert]").
Sce_em_set setzt +0x1c0 = 0 fuer JEDEN Spawn: `sh zero,448(s0)` @0x800421f4 (Port: nur Zombie-Typen,
re15_schwerkraft_seed).

### 6. Daten: die Kanten liegen genau auf den Wegen
SCA (RDT+0x20, 12-B-Zellen, Port-Struktur re15_sca_entry_t): Zellwort Bit 0 = Kletterkante, Bit 1 = Absprungkante;
u0 = 0x01 (nur Spieler fest, Klemm-Maske 4 der NPCs greift nicht) bzw. 0xff.
- ROOM1260/1261 (gleiche Geometrie): [28] x -10550..28400 z 15250..18350 Band 1 u0 0x01 u1 0x02; [29] x -23150..22850
  z -21250..-18100 Band 1 u1 0x02; [12] x -9800..-6600 z -9350..15250 Band 0 u0 0xff u1 0x01; [13] x 24550..27650 z
  8700..15250 Band 0 u1 0x01; [14] x -21850..-18750 z -18100..-5700 Band 0 u1 0x01; [15] x 18700..21800 z -18100..-2100
  Band 0 u1 0x01.
- ROOM10B1: [31] x -6386..-4316 z -9776..-5510 Band 5 u0 0x01 u1 0x0e (Fall 1+3 = 4 Baender); [18] x -9070..-7162 z
  -11468..-4484 Band 1 u0 0x01 u1 0x02; [6] x -6992..-350 z -10126..-5892 Band 0 u0 0xff u1 0x01.
- Wege (SCD): 1261 sub04 @0x012F0 (Plc_dest 9/4/9/5/9/4/9/5/9 -> (-20350,-19300), (20300,-19300), (20300,-14800),
  (20900,-5000)); 1260 sub04 @0x01302 (Ada) / sub05 @0x013BC (Marvin); 10B1 sub02 @0x0182E (John, Lauf @0x01896 zu
  (-6694,-7709) nach `While(Cmp(0x0A != 2))` @0x01888).
- Zensus `M_belege/kanten_zensus.py` (lineare SCD-Dekodierung aller 258 RDTs, NPC-Plc_dest 4/5 gegen die Sonde):
  Treffer nur in ROOM10B1 (John), ROOM1260 (Ada, Marvin), ROOM1261 (John), ROOM2000 (Marvin sub2 @0x01A44, Ada sub3
  @0x01A8A — Leon, Stage 2).

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
