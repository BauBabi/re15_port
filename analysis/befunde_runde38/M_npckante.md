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
- [x] Umsetzung (Vorgaenger a279c9b9, Fortsetzung 2026-10-17 gegen die Bytes nachgeprueft, s. "Pruefung der Fortsetzung")
- [x] Messung nachher an der exe (1261 / 1260 / 10B1, gleiche Wege wie vorher)
- [x] ROOM2000 (Leon, Stage 2: Marvin sub02 @0x01A44, Ada sub03 @0x01A8A — "immer"): vorher/nachher an der exe
- [x] Original-Messung DuckStation (10B1: Sprung/Klettern-Wechsel am Westrand von Block [6] = Original, gleiche Lagen)
- [x] Tests (Riegel geschaerft, ROOM2000, Schwerkraft, Gegenprobe rot, Nebenwirkungen 82 + 10 gruen)

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
| Leon 2000 (Fortsetzung 2026-10-17, exe aus re15_port/build_bisect = Haken-AUS-Stand) | Karte 1260 (-8750,-1800,-8400) Band 1 Cut 0, Flag (4,0xF4) -> Tuer Slot 1 (@0x0114A) -> ROOM2000, sub02-Szene (Viereck alle 25 Bilder) | Ada (Slot 1): ab **F1536** (-6242,-1800,-22108) **41 Bilder in der Luft** ueber dem Kanal; Marvin (Slot 2): ab **F1663** (-5287,-1800,-22052) **46 Bilder**; kein Kanten-Sub (`M_belege/vorher_2000_*`) |

Bilder vorher/nachher (Framedump, gleiche Bildnummern): `M_belege/bild_1261_john_vorher_nachher.png` (VORHER geht John
auf Steghoehe weiter auf die Kamera zu, NACHHER springt er ab F166 in den Kanal) und `bild_1260_ada_vorher_nachher.png`
(VORHER laeuft Ada in Steghoehe in die Luft, NACHHER springt sie ab F68).

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

## Pruefung der Fortsetzung (2026-10-17, Abbruch des Vorgaengers ~23:45)

Selbst nachdisassembliert (re15_disasm.py, info/Re1.5/PSX.EXE + STAGE1.BIN), alle Aussagen der RE-Belege 1-5 bestaetigt:
- FUN_8002d7d8 @0x8002d7d8-da44: Vektor @0x800109c8, Kurs `lhu a0,106(s2)` / `addiu a0,a0,512` / `and a0,a0,v0(-1024)`
  @0x8002d82c-838, Zellwort `jal 0x8003b7f0` @0x8002d874 (a1 = 0 im Delay-Slot @0x8002d878, a2 = `lbu 130(s2)`), Bit 0
  `beq` @0x8002d884 -> Objektschleife ueber 0x800b3f98 (Zahl `lbu s1,2(v0)` aus *(0x800ac778)) mit `lhu a2,6(v0)` +
  `srl a2,a2,1` (Objektbox hx/2) und Band+1 / Band-2 -> `ori v0,zero,0x1` @0x8002d948; sonst Objektschleife mit der
  ENTITY-Box `lw v0,120(s2)` / `lhu a2,6(v0)` (= hit_radius_min, NPC-Familie 450, re15_damage.c Kasten @0x80121658) und
  Band / Band-1; Bit 1 @0x8002d9f0 -> `jal 0x8001c6e8` @0x8002da04 (a1 = 1, a3 = 0x100) -> `slt v1,v1,v0` @0x8002da14 ->
  `ori v0,zero,0x2` @0x8002da1c. FUN_8002da4c @0x8002da4c = Punkt in (Box +6 + Rand) x (Box +10 + Rand) bei gleichem Band
  = re15_collision_prop_box_hit.
- Sub 4: `jal 0x800245d8` @0x80051344 -> anim_set @0x80051360 -> `jal 0x8002d7d8` @0x80051374 -> `sb v0,159(v1)`
  @0x80051388 -> `ori v1,zero,0x12` @0x80051394 / `sb v1,5(v0)` @0x800513a4 / `sb zero,6(v0)` @0x800513b4 / `j 0x80051470`.
  Sub 5: Schritt @0x800516b0 -> `jal 0x8002d7d8` @0x800516c4 -> +0x9f @0x800516d8 -> 0x16 @0x800516e4/f4, +0x6 = 0
  @0x80051704, `j 0x800517dc`; anim_set erst @0x8005171c. Der Port-Haken sitzt genau so (Sub 5 vor, Sub 4 nach
  re15_npc_anim; Ankunftstest uebersprungen).
- Kanten-Sub 0x80052508 und Sprung-Sub 0x80052c14: alle Phasen, Konstanten und Reihenfolgen wie unter 3./4. (Tabellen
  @0x800112e4 = {0x80052548, 0x80052598, 0x80052688, 0x800526c8, 0x80052820}, @0x8001131c = {0x80052c54, 0x80052c94,
  0x80052cd4, 0x80052e1c, 0x80052ea8, 0x80052f50, 0x80052f8c}); 0x12 -> 0x18 / 0x16 -> 0x17 (`bne v1,v0` mit Delay-Slot
  `ori v0,zero,0x17` @0x80052678-7c, sonst `ori v0,zero,0x18` @0x80052684); Fall y += 90 + 50*t (`addiu a0,a0,90`
  @0x80052e34, 25*t*2 @0x80052e44-54); Kehrwert 0x91a2b3c5 @0x80052da4-dc = -(+0x1ba / 1800) mit C-Rundung.
- NPC-Wurzel 0x49 (STAGE1 0x8011dc68): Pause-Gate `g_pauseflags & 0x20000000` @0x8011dc84, +0x9 & 0x20 @0x8011dca4,
  dann `jal 0x8001bd60` @0x8011dcc4 (a0 = -10 @0x8011dcb4, a1 = 0x14 @0x8011dcac), Steuerung 0x80039e7c, Dispatch
  @0x8011dd08, Tail 0x8002b498 / 0x8002aec4 / 0x8002b544 / Klemme 0x8003b0a4 (a1 = Box +6, a2 = 4) @0x8011dd50 / 0x80037358
  / 0x8001b38c @0x8011dd60, Schatten `lh a1,442` @0x8011dd78 + `jal 0x8001b064` @0x8011dd7c.
- Sce_em_set: `sh zero,448(s0)` @0x800421f4 (+0x1c0 = 0) und +0x1ba = -(Band*1800) `sh v0,442(s0)` @0x80042210 fuer
  JEDEN Spawn. Im Port seedet scd_vm.c:3893 (Runde 35 Spur J) +0x1ba schon fuer alle Typen; neu nur +0x1c0 = 0.
- Kein zweiter Pfad: alle NPC-Plc_dest-Walks (Typ 0x40-0x4d, Modi 4/5/7/8/9) laufen im Port durch die State-4-Sub-VM
  (scd_vm.c npc_subvm, walk_active = 0), also durch re15_npc_sub_walk; der NPC-Tick laeuft in JEDEM KI-Flavor ueber
  re15_npc_ai_tick (enemy_ai_common.c Typ-Kette); der generische Anim-Vorschub ueberspringt State-4-NPCs
  (player_common.c:1440). Die Eskorte (State 1, 0x8011ce54 -> 0x8004f100/3a4/4e0) ruft die Sonde NICHT (Byte-Suche).
- Frame-Wort-Toene: die Kanten-Clips 2..5 der Loco-Bank tragen bei John (CDEMD1), Marvin und Ada (CDEMD0) KEIN SE-Bit
  (Wort >> 22 = 0 in allen Bildern, test_r38_npckante clips) -> FUN_8001b38c spielt beim Sprung/Klettern nichts. Clip 5
  der eigenen Bank (Rast-Pose der Phase [1]) traegt das Schritt-Bit 0x4000 in Bild 7/22 (Ada 8/22); Phase [1] liest
  +0x168 VOR anim_set (@0x800525a4-b4) = das im Vorbild posierte Bild und dauert in allen Messungen 1-8 Bilder (Bild 7
  wird erst im letzten Phase-1-Bild posiert, also nie gelesen) -> s. OFFEN.

Bau: `local_build.sh build` gruen; die exe des Vorgaengers (23:43:56) war juenger als die zuletzt geaenderte Quelle
(23:43:34) und enthielt die Haken; der Testlauf wurde neu gelinkt (er war noch gegen die Haken-AUS-Bibliothek der
Gegenprobe gelinkt).

## Umsetzung

| Datei | Inhalt |
|---|---|
| engine/src/npckante_r38.c (neu) | Sonde re15_npckante_sonde (= FUN_8002d7d8), Haken re15_npckante_nach_schritt (Sub 4/5), Kanten-Sub sub_kante (0x80052508), Sprung-Sub sub_sprung (0x80052c14), Kanal-Auskunft re15_npckante_kanal (Phasen [0..2] der Kante = eigene Bank +0x170/+0x174, sonst Loco +0x84/+0x16c), Alte-Lage-Schnappschuss re15_npckante_altlage (+0x40/+0x44 nach dem Schub, @0x80052784 / @0x80052de8), Schatten re15_npckante_schatten_y (+0x1ba, @0x8011dd78), Messhaken re15_npckante_spur (RE15_NPCKANTE_LOG, inkl. fall= = Schwerkraft-Kriterium) |
| include/re15_npckante_r38.h (neu) | alle Konstanten mit Adresse (Sondenweg 800 @0x800109c8, Tempo 0x4b @0x80052550, Clips 5/2/3/4/5, Bild 0x11/0x1d/0x25/0xd, Schub 0x47c @0x80052710 / 0x320 @0x80052d48, Hub 500 @0x80052d68, Fall 90 + 50 t @0x80052e34-5c, Landeabstand 800 @0x80052e90, Band 1800) |
| engine/src/enemy_ai_common.c (Haken) | re15_npc_sub_walk: Sonde nach dem Schritt (2 Zeilen); re15_npc_sub_dispatch: case 0x12/0x16/0x17/0x18; re15_npc_ai_tick: Schwerkraft re15_schwerkraft_8001bd60(e, -10, 0x14, Box +6) VOR dem Dispatch (@0x8011dcc4), Yield-Gate laesst die Kanten-Subs laufen, Alte Lage vor der Klemme, Messhaken; Bank-Auskunft re15_actor_uses_own_bank / _loco_bank |
| engine/src/scd_vm.c (1 Zeile) | Sce_em_set: +0x1c0 = 0 fuer jeden Spawn (@0x800421f4) |
| platform/pc/main.c (2 Zeilen) | NPC-Schatten auf +0x1ba, solange Kanten-Sub oder Schwerkraft-Fall laeuft |
| tests/unit/probes/r38_npckante.cmake, test_r38_npckante.c, probe_r38_npckante_karte.c | Riegel + Karten-Werkzeug (Fortsetzung: Mechanik Bild fuer Bild, john10b1 gegen die Original-Messung, j2000, schwerkraft; Szenen-Schritt mit Spieler-Walker) |

Fortsetzung 2026-10-17 (ohne Verhaltensaenderung am Spiel): Messhaken um `fall=` (Schwerkraft-Kriterium
FUN_8003b7f0(&+0x34, -r, +0x82) & 2, @0x8001bd94-b4) und `f1c0=` erweitert; Werkzeuge `M_belege/luft_auswertung.py`,
`kontaktbogen.py`, `nachher_laeufe.sh` (+ MESS_BUILD fuer die Gegenprobe in messlauf.sh), `orig_10b1_john.py`,
`orig_uebergaenge.py`, `schwerkraft_zensus.py`. Die Spiel-Logik des Vorgaengers (a279c9b9) blieb unveraendert — jede
Zeile gegen die Bytes geprueft (Abschnitt "Pruefung der Fortsetzung"), durch die Original-Messung bestaetigt.

## Messung nachher

Gleiche Wege wie vorher (`M_belege/nachher_laeufe.sh` -> messlauf.sh: Speicherkarte + CONTINUE + RE15_FIRE_AOT = Tuer
betreten), exe dieses Baums, Messhaken RE15_NPCKANTE_LOG je Bild, Auswertung `M_belege/luft_auswertung.py` ("in der Luft"
= Geh-/Lauf-/Stand-Sub mit y != -(Band*1800) oder Schwerkraft-Kriterium fall=1; geparkte Aktoren (-30000,*,-30000)
ausgenommen). Logs `M_belege/nachher_*_npckante.log`, debug.log-Auszuege `M_belege/nachher_*_debug_auszug.txt`.

| Lauf | Weg (debug.log) | Ergebnis |
|---|---|---|
| Elza 1261 | `CONTINUE: resumed in room 10b1` -> `[fire-aot] slot=1 at F40` -> `PC loaded room1261.rdt` | John: F143 Sonde 2 bei (-20477,-1800,-17374) Band 1 -> Sub 0x12 (Rast, Clip 5 eigene Bank) -> F151 Sub 0x18 -> Clips 3/4/5, F165 Band 1 -> 0 (y -1800 -> -1210, fallend) -> landet auf 0; laeuft unten nach Osten; F430 Sonde 1 bei (20057,0,-18847) -> Sub 0x12 Clip 2, F461 Band 0 -> 1; Ende F1200 (20885,-1800,-5250) Band 1 Sub 6. **0 Bilder in der Luft** (vorher 249 von 400 Stichproben) |
| Leon 1260 | `CONTINUE: resumed in room 11b0` -> `[fire-aot] slot=5 at F40` -> `PC loaded room1260.rdt` | Ada: F53 Sonde 2 (26750,-1800,14500) -> F54 Sub 0x18, F68 Band 1 -> 0; F317 Sonde 1 (-7489,0,16038) -> Clip 2, F348 Band 0 -> 1; Ende (-7502,-1800,-6005). Marvin: F118 Sonde 2 (25400,-1800,14500) -> F119 Sub 0x18, F133 Band 1 -> 0; F366 Sonde 1 (-8820,0,15980), F397 Band 0 -> 1; Ende (-8901,-1800,-4453). **0 Bilder in der Luft** (vorher Ada 241 / Marvin 224) |
| Elza 10B1 | `CONTINUE: resumed in room 10b1` (Band 5, Cut 2, (3,0x80)(3,0x82)) | John: F115 Sonde 2 (-3661,-9000,-8275) Band 5 -> F120 Sub 0x17, F134 Band 5 -> 1 (Fall ueber 7200 auf Block [6]); F180 Sonde 2 (-6411,-1800,-7776) am Westrand von Block [6] (Zelle [18]) -> F200 Band 1 -> 0; F245 Sonde 1 (-7460,0,-7773) -> F280 Band 0 -> 1 (Hochklettern); danach im Wechsel Sprung (F342/F485/F628/F771) und Klettern (F423/F566/F709/F852) bis F900. **0 Bilder in der Luft** (vorher 7200 ueber Block [6]) |
| Leon 2000 (Stage 2, "immer") | `CONTINUE: resumed in room 1260` (Karte (-8750,-1800,-8400) Band 1, Cut 0, (4,0xF4)) -> `[fire-aot] slot=1 at F40` (Tuer @0x0114A, Ziel Stage 2 Raum 00) -> `PC loaded room2000.rdt`; sub00 @0x01794 Ck(4,0xF4) -> sub02-Szene (12 Nachrichten, Viereck alle 25 Bilder) | Ada: F1517 Sonde 2 (-5558,-1800,-20854) -> F1526 Sub 0x18, F1540 Band 1 -> 0; F1605 Sonde 1 (-7242,0,-24461) -> Clip 2, F1644 Band 0 -> 1. Marvin: F1642 Sonde 2 (-4321,-1800,-20813) -> F1652 Sub 0x18, F1666 Band 1 -> 0; F1737 Sonde 1 (-6668,0,-24412), F1777 Band 0 -> 1. **0 Bilder in der Luft**. Gegenprobe VORHER (gleicher Weg, exe aus build_bisect = Haken-AUS-Stand): Ada 41 / Marvin 46 Bilder in der Luft ab F1536 / F1663, kein Kanten-Sub (`M_belege/vorher_2000_*`) |

Bilder (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1): `M_belege/bild_nachher_1261.png`, `bild_nachher_1260.png`,
`bild_nachher_10b1.png`, `bild_nachher_2000.png` (Ada springt F1540-F1545 in den dunklen Kanal, geht unten, zieht sich
F1625-F1665 am Gegenufer hoch; Marvin springt F1665-F1670). 10B1 Cut 2: John laeuft links am Steg vorbei, springt (F128-F140 Absprung-Hocke) und faellt aus
dem Bild nach unten (F146 klein unten links); der Wechsel Sprung/Klettern am Westrand von Block [6] liegt AUSSERHALB von
Cut 2.

### Warum John in 10B1 springt UND wieder hochklettert (Herleitung aus Daten + Mechanismus, s. auch Original-Messung)
sub02 @0x01896 schickt John im LAUF (Modus 5, Ankunftsradius 300 @0x80051764) nach (-6694,-7709) — das Ziel liegt AUF
Block [6] (x -6992..-350), 298 vom Westrand. Die Sonde schaut 800 voraus in den gerasteten Kurs (Westen); Zelle [18]
(Band 1, u1 0x02) beginnt bei x < -7162, die Sonde feuert also ab John.x < -6362. Ankommen verlangt Abstand < 300, also
John.x < ~-6400. Jede Lage, die ankommen liesse, laesst zuerst die Sonde feuern (Sub 5 ruft die Sonde @0x800516c4 VOR dem
Ankunftstest @0x80051764) -> John springt am Westrand hinunter, ehe er ankommt. Unten (Band 0) liegt das Ziel wieder
OSTLICH auf dem Block, Sub 5 laeuft dorthin, die Sonde sieht Block [6] (Band 0, u1 0x01) -> Klettern (Ergebnis 1) ->
oben wieder Sprung. Das endet erst, wenn der Spieler Cut 8 erreicht: sub02 `While(Cmp(0x0A != 8))` @0x0189E ->
Evt_exec sub03 @0x018AC setzt John per Pos_set @0x018B6 auf (-9278,0,-6842) Band 0 (Member_set 0x12/0x13 = 0) — genau
der Boden westlich des Blocks, wo ein Sprung von Zelle [18] endet. Dieselbe Folge liefert die unabhaengige Python-
Nachbildung `M_belege/kanten_zensus.py` (SPRUNG an (-6383,-7711), KLETTERN an (-6984,-7711) im Wechsel).


## Original-Messung (DuckStation, 2026-10-17) — ROOM10B1 John am Original

Werkzeug `M_belege/orig_10b1_john.py` (Muster Runde 36 orig_11f0_spielstand.py / R_belege/orig_1090_push.py): MZD-Disc
mit stage_saves/mzd_debugmenu.sav, GDB-Server 127.0.0.1:2345 nur fuer den Lauf (settings.ini gesichert und
zurueckgeschrieben, `EnableGDBServer = false` danach geprueft).
- Code-Gleichheit RAM == info/Re1.5/PSX.EXE: Sonde 0x8002d7d8-da4c, Subs 4/5 0x80051148-0x800517f0, Kanten-/Sprung-Sub
  0x80052508-0x80052fd0, Schwerkraft 0x8001bd60-bf00 — alle `True`; EXE-Stub @0x80026e4c = `0800e003` (Auslieferung).
- Debug-Menue JUMP 10B (0x800BBE5F := 0x0B) mit Elza-Bit 0x800ACA3C | 0x80000000 und Flags (3,0x80)(3,0x82) (Wort
  0x800B1008). Danach im RAM: SCA-Zellen == ROOM10B1.RDT `True`, Sub-SCD 0x8012313c == ROOM10B1.RDT @0x0178C (852 B)
  `True`; (3,0x80) geloescht und (3,0x81) gesetzt = sub02 @0x0182E lief.
- Lauf 1 (`orig_10b1_john_lauf1` nicht eingecheckt): Spieler nur auf den Steg (0,-9000,-12000) gesetzt -> Var 0x0A
  (0x800b0fe4, Cmp @0x8003ff94) bleibt 0, John wartet 100 s in Sub 6 bei (-2696,-9000,-8562) = sub02 `While(Cmp(0x0A !=
  2))` @0x01888 haelt. (Ein Teleport schaltet die Kamera nicht: die RVD-Zonen gehoeren zur aktuellen Gruppe.)
- Lauf 2 (`M_belege/orig_10b1_john.log`, 1206 Stichproben in 75 s, Zusammenfassung `orig_10b1_john_uebergaenge.txt`):
  zusaetzlich Cut 2 ANGEFORDERT ueber DAT_800afbb5 = 2 (der Per-Bild-Vergleich @0x800214f8-14 schaltet ueber den eigenen
  Apply). Ergebnis am ORIGINAL:

| t (s) | Original (Entity 0x800ACC2C) | Port (exe, nachher_10b1) |
|---|---|---|
| 0.50 | Sonde 2 (+0x9f = 2) bei (-3676,-9000,-8352) Band 5 -> Sub 0x16, Rast Clip 5 | F115 Sonde 2 bei (-3661,-9000,-8275) -> 0x16 (anderer Startpunkt: am Original wartete John schon am Wegpunkt) |
| 0.75-1.37 | Sub 0x17, Clip 3 bis Bild 13, dann Phase 3 Band 1, y -8270, +0x1ba -1800 | F120-F134 dasselbe |
| 1.99 | Landung y -1800 (= +0x1ba) auf Block [6], Clip 5 | F149 Landung y -1800 |
| 3.24 | Sonde 2 bei (-6407,-1800,-7789) -> 0x16 -> 0x17 | F180 Sonde 2 bei (-6411,-1800,-7776) |
| 4.12-4.30 | Phase 3 Band 0 bei **x -7460** (Schub 800 + Klemme), y -1210 -> -640 -> 0 | F200 Band 0 bei **x -7460**, y -1210 ... 0 |
| 5.98-7.47 | Sonde 1 (+0x9f = 1) bei (-7460,0,-7792) -> 0x16, Clip 2, Stufe auf **x -6314** y -1800 Band 1 | F245 Sonde 1 bei (-7460,0,-7773), F280 **x -6314** y -1800 Band 1 |
| 9.09 ... 74.7 | danach im Wechsel Sprung (x -7460) / Klettern (x -6314), 12 Zyklen in 75 s, Cut bleibt 2 | dasselbe bis F900 |

**Damit ist der Wechsel Sprung/Klettern am Westrand von Block [6] ORIGINAL-Verhalten** (gleiche Lagen bis auf die
Einheit), nicht ein Port-Fehler; er liegt ausserhalb von Cut 2 und endet mit Cut 8 (sub03 Pos_set @0x018B6).

## Tests

Riegel `re15_port/tests/unit/test_r38_npckante.c` (registriert NUR in `tests/unit/probes/r38_npckante.cmake`, je Teil ein
ctest `unit_r38_npckante_<teil>`), headless am Engine-Kern mit den echten RDTs und Modellarchiven:

| Teil | misst |
|---|---|
| clips | Kanten-Clips 2..5 der Loco-Bank je NPC (John CDEMD1, Marvin/Ada CDEMD0): Bildzahl reicht fuer Bild 0x11/0x1d/0x25 (Klettern) und 0xd (Absprung); Wurzel-Y/Frame-Wort-SE-Bits je Bild (Info) |
| sonde | FUN_8002d7d8-Zwilling an den Messstellen (1261 Zelle [29]/[15], 1260 [28]/[12], 10B1 [31]/[18]) inkl. "ein Schritt davor = 0" |
| john1261 | Elza ROOM1261 (3,0x41) Cut 15: Sprung (Clips 3/4/5), genau 1 Sprung + 1 Hochklettern, nie in der Luft, Ende Band 1 am Ziel; Fallkurve Bild fuer Bild (+590, dann +90+50k), Landung = +0x1ba, Kletterstufe -1800/0x47c/Band+1 |
| ada1260 | Leon ROOM1260 (4,0xF4) Cut 7: Ada UND Marvin springen + klettern, nie in der Luft, Ende Band 1; Mechanik wie oben je Figur |
| john10b1 | Elza ROOM10B1 sub02 Cut 2: Sprung 5 -> 1 (genau einmal), danach Wechsel Sprung/Klettern mit Landung bei x -7460 und Stufe bei x -6314 = DuckStation-Messung am Original; Mechanik |
| gegen | Ada ROOM1050 (Weg ohne Kante): kein Kanten-Sub, kein Bandwechsel |
| j2000 | Leon ROOM2000 (4,0xF4) sub02/sub03 (Szenen-Schritt wie test_r36_szenenleon, Viereck alle 25 Bilder): Ada und Marvin springen genau einmal (Clips 3/4/5) und klettern genau einmal, nie in der Luft; Mechanik |
| schwerkraft | NPC-Wurzel ruft FUN_8001bd60: Ada (Sub 6) mitten in Zelle [28] gesetzt -> y += -10 + 20 t Bild fuer Bild, Landung y 0 / Band 0 / +0x1ba 0 / Fallbit geloescht, Schatten auf +0x1ba |

Ergebnisse (woertlich):
- `ctest --test-dir re15_port/build -R "r38_npckante" --timeout 600 --output-on-failure`:
  **100% tests passed, 0 tests failed out of 8**
- Gegenprobe gegen den Haken-AUS-Stand (enemy_ai_common.c / scd_vm.c / main.c aus 0bcf46ac = f587d34e + Messhaken; zwischen
  v0.8.25 2077bb8e und f587d34e gibt es keine Aenderung unter re15_port/engine|platform|include), gebaut in
  re15_port/build_bisect, `ctest --test-dir re15_port/build_bisect -R "r38_npckante" --timeout 600`:
  **38% tests passed, 5 tests failed out of 8** (rot: john1261, ada1260, john10b1, j2000, schwerkraft; gruen wie erwartet:
  clips, sonde, gegen — die messen Daten bzw. den Weg ohne Kante). Einzelheiten `M_belege/riegel_ohne_haken.txt`.
- Nebenwirkungen, alle NPC-/Szenen-/Raum-Gruppen, die der Diff beruehrt (enemy_ai_common.c NPC-Tick, scd_vm.c Sce_em_set,
  main.c NPC-Schatten; Namensliste aus `ctest -N` gefiltert nach npc|marvin|ada|elliot|irons|sherry|szenen|11b0|1260|1261|
  10b1|10d0|1170|1090|2000|schwer|trage|schatten|plc_|walk|roy|1141|1021|1011|zombieki|r38_ ...):
  - 82 Unit-/Probe-Tests (-R-Ausdruck woertlich in `M_belege/ctest_nebenwirkung.txt`, beginnt `^(unit_npc_ai|unit_npc_scd_pose|unit_sherry_wall_pin|unit_elliot_1170_run|...`, `-j4`):
    **100% tests passed, 0 tests failed out of 82**
  - 10 Integrationstests seriell (`-R "^(integration_r30_irons_tisch_laden|integration_r30_irons_tisch_bild|integration_r30_irons_tisch_licht|integration_r36_elza_irons|integration_r36_pri_schatten|integration_r36_szenenleon|integration_r37_elzaems_elza1141|integration_r37_roy_szene1011|integration_r37_roy_szene1021|integration_r37_roy_leon1170)$"`):
    **100% tests passed, 0 tests failed out of 10**
- Schwerkraft-Zensus `M_belege/schwerkraft_zensus.py` (alle 34 RDTs mit Absprungzellen, jede Sce_em_set-/Pos_set-/
  Member_set-0x12-Stelle einer NPC 0x40..0x4d): **0** Stellen, an denen eine Original-Szene eine NPC auf Bandhoehe in eine
  Absprungzelle stellt -> die neue Schwerkraft greift nur ueber Bewegung an Kanten. ROOM11B0 (Spur C, Marvin-Container)
  hat KEINE Absprungzelle (u1 & 2), dort kann die Schwerkraft nicht ausloesen.

## OFFEN

- **NPC-Schritt-Ton in der Rast-Phase [1]** (`lw v0,360` +0x168, `andi 0x4000` @0x800525b4 -> FUN_80045630(0, 7 -
  3*((w>>12)&1)) @0x800525d4): die Rast-Pose ist Clip 5 der eigenen Bank (= derselbe Clip wie der NPC-Gehzyklus,
  Schritt-Bit in Bild 7/22, Ada 8/22). Der Port spielt NPC-Schritte generell nicht (vorbestehende, im Code benannte Luecke
  re15_npc_sub_walk "OPEN (Footstep-SE der Phase 1 ...)": FUN_80045630 nimmt fuer Typ >= 0x40 den Zweig `addiu s3,a0,-8`
  @0x8005175c, den re15_audio_footstep nicht abbildet, ohne audio_pc.c anzufassen). In allen gemessenen Kanten liest Phase
  [1] Bild 7 nie (sie endet nach 1-8 Bildern, das Bit wird erst im Folgebild gelesen) — hoerbar waere hoechstens ein
  Schritt im ersten Rastbild aus dem letzten Gehbild. Nicht Teil der Nutzer-Punkte; bleibt bei der NPC-Schritt-Luecke.
- Die Kanten-Clips selbst tragen KEINE Frame-Wort-SE-Bits (FUN_8001b38c spielt beim Sprung/Klettern am Original nichts) —
  kein offener Ton fuer Sprung/Landung.
- Original-Messung nur fuer 10B1 (der einzige Raum mit einem Verhalten, das nach einem Port-Fehler aussehen koennte). 1260 /
  1261 / 2000 laufen ueber dieselben, gegen PSX.EXE byte-gleich geprueften Funktionen (Code RAM == PSX.EXE am Original).

## Fuer den Nutzer

- Neue Assets: keine. Sprachdateien: keine. Paket-Gate: nichts Neues.
- Wo zu sehen: Leon — ROOM11B0 Tuer zur Kanalisation -> ROOM1260: Ada und Marvin springen nacheinander vom Steg in den Kanal,
  gehen unten weiter und klettern auf den Mittelblock; Stage 2 ROOM2000 (nach der Unterhaltung): beide springen in den
  Kanal und ziehen sich am anderen Ufer hoch. Elza — ROOM10B1 (Kamera auf dem oberen Steg): John laeuft zum Stegende und
  springt hinunter (Fall ueber vier Stockwerke auf den Block); ROOM1261: John springt vom Steg in den Kanal und klettert
  am Ostende wieder hinauf.
- Zu wissen (kein Fehler): in ROOM10B1 springt John unten am Westrand des Blocks noch einmal hinunter und klettert wieder
  hinauf, solange die Kamera auf dem oberen Steg bleibt — genau so am Original gemessen (DuckStation, dieselben Lagen); das
  Ziel des Skripts liegt knapp neben der Kante. Von der Steg-Kamera aus ist das nicht zu sehen; sobald Elza Kameraschnitt 8 erreicht
  (sub02 `While(Cmp(0x0A != 8))` @0x0189E -> sub03), stellt das Skript John unten westlich des Blocks ab (wie im Original).
- Entscheidungsfragen: keine.
