# Runde 41 — Spur F "gatorszene" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r41_gatorszene`, Zweig `r41/gatorszene`, Basis `aece7457` (= r41/integration 9f10bdb0
+ AUFTRAG/VERTRAG Stand Nachtrag 16 / FG-E1).

## Punkte (Wortlaut AUFTRAG.md, Zuteilung VERTRAG §0 + Entscheid FG-E1 + Nachtrag 6)

1. "Ich moechte eine neue Cutscene: Wenn man Room 2090 betritt, soll der Charakter einen Zombie am Boden liegen sehen
   mit einen blinkenden Schluessel - Blinkend so - wie bei der anderen Implementierung weiter oben der Feuerloescher.
   Es schwimmt bereits ein Zombie im Wasser, nimm vielleicht diesen. Der Aligator soll dann aus dem Wasser springen
   und den Zombie fressen inklusive dem Schluessel. Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe
   seine Leiche sein und blinken."
   + Nachtrag 6 (Szenenfolge): Ankunft -> Kamera auf Zombie-Leiche mit Schluessel -> oben in 2080 kommen die Zombies
   herein -> erneut Kamera auf den Zombie -> Alligator kommt, frisst Zombie samt Schluessel -> Schrei- und
   Schuettel-Animation Richtung Leon -> Kampfbeginn mit RE2-Kampfmusik (ROOM40A0).
2. "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."

Ressourcen (VERTRAG 1.1/1.2): Bits 110-113 (110 Szene gesehen, 111 Schluessel liegt, 112 Schluessel genommen,
113 frei), Nachrichten 2090/2091 Ids 42-46.

## Stand (fortlaufend)

- [ ] Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE r40/r41, Dossiers B/D)
- [ ] Messung vorher
- [ ] RE-Belege
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests

## 0. Protokoll (neueste unten)

* 2026-10-24 19:4x Start: Baum sauber (`status --porcelain` leer), HEAD aece7457 (= 9f10bdb0 + AUFTRAG/VERTRAG).
  Pflichtlektuere: AUFTRAG.md (ganz, Nachtrag 6 + FG-E1 massgeblich), VERTRAG.md, UEBERGABE.md, Dossier B
  (§3.1, §7, §11.1 B9 untergetaucht, §11.8 Bausteine Schrei/Schuetteln/Musik, §13 FRESSEN), Dossier D §2 (Blink-API).
* Bau des Ausgangsstands (configure + build, 2147 Schritte, EXIT 0) — exe `re15_port/build/platform/pc/re15_pc.exe`.
* Messlaeufe immer mit eigener exe-Kopie `re15_pc_r41f_<lauf>.exe` (Helfer scratchpad `F41/lauf.sh`), RE15_WINDOW_SCALE=1.

## Punkt 2 — Spinnen aus ROOM2090 entfernen

### 2.1 Messung vorher (Stand aece7457, Lauf `v20a0`: RE15_DEBUG_JUMP=20A0@250 + RE15_FIRE_AOT=0@40#20A0 = echter
Tuerweg 20A0 -> 2090, Ankunft (6300,-16250) Cut 2)
- state.log F10: Aktor 1 `t=25 @(-100,-8296)` hp 111, Aktor 2 `t=25 @(-2300,-23896)` hp 110, Aktor 35 `t=23` (Boss).
- F70..F550: beide Spinnen laufen auf Leon zu (Aktor 1 bis (6197,-13482), Aktor 2 bis (6194,-19118)), Angriffsclips 4/7/11.
- Bild F121/F161 (Framedump 1..581/20): die Spinnen sind im Wasser vor Leon zu sehen.
- Weitere Cut-Bilder (je Cut ein Framedump, `F41/mess/cut2090_<c>/bild.ppm`): Cut 0 und Cut 8 zeigen die Spinne neben Leon
  in der Leiter-Nische (Spawn von 2080 aus).

### 2.2 RE-Beleg
- Die Spinnen sind zwei RDT-Saetze in main00 (ROOM2090.RDT und ROOM2091.RDT bytegleich, `xxd -s 0xAB0`):
  `@0x00AB0 44 00 25 00 00 00 00 73 9c ff 00 00 98 df 00 00 00 00 00 00` (Slot 0, Typ 0x25, Kill-Flag 0x73, (-100,0,-8296)),
  `@0x00AC4 44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2 00 00 00 00 00 00` (Slot 1, Typ 0x25, Kill-Flag 0x74, (-2300,0,-23896)).
  Feldlage pc[1] Slot, pc[2] Typ, pc[7] Kill-Flag, pc[8..13] Lage (scd_vm.c op_sce_em_set, Handler 0x800420A0).
- FORM des Verschwindens im Original: ein Sce_em_set-Satz erscheint nicht, wenn das Kill-Flag-Tor ihn verwirft
  (FUN_800420a0 @0x80042120-38; `ori v0,zero,0x8000` / `sw v0,0(s0)` @0x80042230-38 = Eintrag "verworfen").
- NUTZER-VORGABE: "Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr." -> Port verwirft genau diese
  zwei Saetze beim Lesen (Muster zombie_abstand_r35.c: Schluessel Raum-Basis + Typ + Original-Lage), kein RDT-Patch,
  kein Kill-Flag gesetzt (0x73/0x74 bleiben frei, kein Spielstand-Unterschied).
- Der in der Aufgabe genannte "TEMPORAERE Spinnen-Freeze" (Suchanker "TEMPORAER") existiert im Stand aece7457 nicht mehr:
  `grep -rn TEMPOR` ueber re15_port ohne Treffer; enemy_ai_common.c:14729 traegt nur noch den Kommentar
  "Spinnen-Freeze 2026-09-10..11 ENTFERNT (Nutzer-Freigabe)". Nichts zu entfernen.

### 2.3 Umsetzung
- NEU `re15_port/engine/src/gatorszene_r41.c` + `re15_port/include/re15_gatorszene_r41.h`:
  `re15_gatorszene_r41_spawn_weg(raum, typ, x, z)` (1 = verwerfen), Riegel-Schalter `re15_gatorszene_r41_spinnen_weg_setzen`
  (nur fuer Mess-Riegel, kein Spielschalter, kein getenv), Zaehler `re15_gatorszene_r41_spinnen_verworfen`.
  Konstanten mit Datei-Offset: Typ 0x25 @0x00AB2/@0x00AC6, Lagen @0x00AB8/@0x00ABC/@0x00ACC/@0x00AD0.
- HAKEN `scd_vm.c` (2 Zeilen, "Runde 41 Spur F"): Include + in op_sce_em_set direkt nach dem Kill-Flag-Tor
  `if (re15_gatorszene_r41_spawn_weg(...)) suppress = 1;` (vor dem Gleichzeitig-Limit, verbraucht keinen Zaehler).
- Riegel MIT Beleg angepasst: `tests/unit/probe_sca_wall_hit.c` Teil C2 (Adult-Spinne an der 2090-Geometrie, setzt
  g_current_room_id = 0x2090): misst die SCA-Klemme der Spinne am ORIGINAL-Satz @0x00AB0 -> Verwerfen fuer die Messung
  aus (`re15_gatorszene_r41_spinnen_weg_setzen(0)`), danach zurueck. `unit_re2_spider_water_2090` setzt keinen Raum
  (g_current_room_id bleibt 0) und ist deshalb unberuehrt; `r31_trefferboxen*` zaehlt Spinnen ueber alle Raeume (> 0 verlangt).
- Gezielt (Stand mit Haken): `ctest -R "^(unit_sca_wall_hit|unit_re2_spider_water_2090|r31_trefferboxen|r31_trefferboxen_versteckt)$"`
  -> `100% tests passed, 0 tests failed out of 4`.

### 2.4 Messung nachher (gebauter Stand mit Haken, exe-Kopien)
- Leon, Lauf `n20a0` (echter Tuerweg 20A0 -> 2090 wie 2.1): debug.log
  `[gatorszene] ROOM2090: Spinne 0x25 bei (-100,-8296) verworfen ...` und `... bei (-2300,-23896) verworfen ...`;
  state.log ab der Ankunft (PL(6300,-16250), Cut 2): 0 Zeilen mit einem Aktor `t=25`, nur der Boss `[35 t=23 ...]`.
- Elza, Lauf `n2091` (RE15_PSELECT_AUTO=1 + _SWITCH=1, Tuerweg 20A1 -> 2091, debug.log `Spieler-Familie PL04`):
  dieselben zwei Zeilen fuer ROOM2091, state.log ab der Ankunft 0 Zeilen `t=25`.
- In 20A0/20A1 bleiben die Schachtspinnen (dort nicht verlangt; `t=25` steht vor dem Tuerwechsel im state.log).

## Punkt 1 — Ankunftsszene 2090 (Zwischenstand, fortlaufend)

### 1.1 Messung vorher (Stand aece7457)
- Kein Szenen-Code fuer 2090: Tuerweg 20A0 -> 2090 (Lauf `v20a0`) zeigt Cut 2, Spinnen laufen an, der Boss lauert untergetaucht
  am Lauerplatz (-7200,-9200) (state.log `[35 t=23 ... @(-7200,-9200,r1089)]`), kein Programm, keine Leiche mit Schluessel.
- Kameras ROOM2090 (RID @0x060, 13 Cuts; Bilder `F41/mess/cut2090_<c>/bild.ppm`): Cut 0 (Eintritts-Cut aus 2080) zeigt die
  treibende Zombie-Leiche obj 2 (Obj_model_set @0x00A6C, (-1725,-1740,-21723) Gier 685, Box 1800/450/450) mitten im Suedbecken,
  Cut 5 die zweite Leiche obj 3 (@0x00A8E (-25,-1740,-7423)). Beide wippen (SUB sub02/sub03 `2e 03 02` / `2e 03 03` +
  Speed_set/Add_speed @0x00AF0/@0x00B5E). = "Es schwimmt bereits ein Zombie im Wasser". Prop-Modell 2 (MD1 @0x2054):
  x -444..2626, y -341..966, z -1049..822.
- 2080 von 2090 aus unsichtbar: alle 13 Kameras von 2090 liegen auf y -2131..-5634 und blicken ins Becken; 2080 liegt auf
  Band 8 (ROOM2090 Tuer 0 @0x00996 Nutzlast-y `c0 c7` = -14400). Kameras ROOM2080 (`cut2080_<c>`): Cut 0 = Treppe von oben
  zur 2030-Tuer, Cut 2 = Hauptbereich mit Rechner, Cut 5 = Reling vor "SEPTIC POOL". Spieler-Lage (Lauf p2080b
  (-16182,-14400,-17000)) ist in Cut 0 unsichtbar (hinter der Kamera).

### 1.2 RE-Belege
- **Szenenform** (Port-Muster mit Original-Vorbildern): SCD-Bytecode aus Original-Opcodes (cut_10f0.c, irons_tod_1150.c);
  Opcode-Formen: Set `22 bb ii vv` (ROOM1150 sub08 @0x01110), Sleep (@0x01162), Cut_chg (@0x01178), Riegel Set(2,7)/(1,27)
  (@0x01114/@0x01118), Plc_ret + Cut_auto (@0x012E0/@0x012E2), Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`
  (@0x0112A-@0x01132), Sce_em_set (ROOM2020 main00 @0x008F6), Door_aot_set-Nutzlast (ROOM2090 main00 @0x00996), Aot_on
  (ROOM1240 sub02 / ROOM1080 sub07). Montage = Raumwechsel in der Szene ueber Door_aot_set + Aot_on (irons_tod Muster).
- **Ereignis 18**: 2090-Sub-Tabelle @0x0ADC `08 00` = 4 Subs, 2080 @0x0BF8 `06 00` = 3 Subs; kein anderer Port-Hook nutzt 18.
- **2080-Zombies**: Satzform grid 0x0d = ENGAGE ab INIT (STAGE1.BIN @0x80100f40-44; Raum-Records ROOM1040 @0x011FC /
  ROOM1030 @0x01DE6; irons_tod EM_1130_DURCH), Kill-Flag 0xff (kein Tor @0x80042128). Typen 0x16/0x10/0x11/0x12 = die
  Zombie-Typen der STAGE2-Wurzel (Rosterscan wasserzombie_r38.c), 0x16 = einziger Raum-Zombie-Typ von STAGE2 (Zensus
  `F41/emset.py`: ROOM2000 @0x017CC.., ROOM2020 @0x008F6..). Lage = Treppenfuss vor der 2030-Tuer (Tuer 2080 Slot 0
  @0x00BA2, Ankunft aus 2030 (-23200,-9000,-16700) ROOM2030 @0x01AEA), Band 5.
- **Metalltuer-Ton**: Door_exit-Satz (Tonkopf 1) des RE2-Archivs der Tuer, durch die die Zombies kommen: 2080 Slot 0 =
  DOOR06 (gen/tuer_zuordnung.inc S161), Tonteil 0x4C98 B (Archivtabelle @0x8009A568) - Form cut_10f0.c Tuerknall.
- **Gator-Clips (RE2 EM23, selbst disassembliert `re2_disasm.py --bin EM23_OVL_0000.BIN`)**: FUN_8001a330-Rufe
  @0x801010f4 (`addiu a1,zero,4` = FRESSEN P1 Clip 4), @0x80101188 (Clip 5 P2), @0x80102ad0 (Clip 8 = R3-Tod, nicht genutzt),
  @0x801027e8 (Clip 7 = R2-Todesrolle), @0x80102c48 (Clip 10, `addiu a2,zero,3` = Blend 3, R4-Grossreaktion), Intro Clip 6
  @0x801008ac-b8. Wurzelbahnen (EMR Keyframe-Bytes 6..11, `F41/em23_bahn.py`): Clip 6 125 Bilder off -7040 -> +2458
  (Auftauchen: Pose-y +1489 -> -1210 in Bild 0..30), Clip 4 off 810 -> 3301, Clip 5 2803 -> 3834 -> 3132 (Schuetteln),
  Clip 11 3132 konstant (Kauen), Clip 10 0 -> -1025 (Aufbaeumen, Pose-y -2033 -> -1053).
- **FRESSEN-Anker** (RE2): Koppelbild 13 @0x80101104-34, Anker FUN_80015b94 (@0x80101110-34), absolute Lage FUN_80015cb8
  (`sw v0,56(s1)` @0x80015d68, `sw v0,64(s1)` @0x80015d90), P2/P3 @0x80101174/@0x801011cc.
- **Leiche im Maul** = RE2-Kanister-Form: Kind der Part-6-Weltmatrix (`addiu a0,s0,1104` @0x801018d0 / `sw a0,128(s2)`
  @0x801018e0), Relation im Koppelbild eingefroren wie FUN_8002d030 @0x801018f0 (rel = R6^T*Ro, lokal = R6^T*(o-p6),
  `sw` @0x80101910-18).
- **Wende auf Leon**: RE2 R1 Ziel-Gier 75/Bild (FUN_8001569c @0x801025f8-608).
- **Schrei + Schuetteln** = RE2-Grossreaktion R4: SE 0 (`addu a0,zero,zero` @0x80102c0c + `jal 0x8005bd6c` @0x80102c10,
  ENEMSE Bank 17), Clip 10 Blend 3 (@0x80102c40-48), dazu das Stoss-Ereignis 1 (Bildwort 0x28 -> +0x227 = 1 @0x801003f0-40c,
  Ereignis-Maschine @0x80101d44: FUN_800395b8(10,120,50,0) Ruetteln + Bildschirm-Schuetteln).
- **RE2-Kampfmusik** (ROOM40A0): Raumtabelle aus COMMON/DATA/INIT_TBL.DAT (zwei Szenarioblöcke je 0x16C8 B, Tabelle @0x22C,
  Stage-Versatz-Tabelle @0x800a80b0 `00 1f 3d 4d 60 6c 86` -> Stage 4 = 0x4d; Eintrag 0x4d+0x0A @0x2DA = `5a ff` in BEIDEN
  Bloecken) = MAIN 0x1A mit Manuell-Start-Bit 0x40, kein SUB. Gegenprobe der Tabellenlage: ROOM4090/4100/2020 setzen per
  Sce_bgmtbl_set (0x57, Handler @0x80057e98: a0 = pc[2..3]<<16 | pc[4..5] | pc[6..7] -> FUN_8005b9f4) genau die Werte, die
  @0x22C+2*(Versatz+Raum) im Block 0x16C8 stehen (4090 `14 98`, 4100 `14 ff`, 2020 `06 ff`). Gestartet von sub11
  `51 00 01 00 78 40` @SCD 0x071F (Handler @0x80057e20: a0 = pc1<<28|pc2<<24|pc3<<16|pc4<<8|pc5; FUN_8005b6f0 Fall 1 =
  SsSeqPlay), ausgeblendet `51 00 05 00 00 00` @0x0762/@0x078A (Fall 5 FUN_8007a120(seq,0x7f,0x5a)); beim Verlassen setzt
  ROOM40A0 sub27/sub28 @0x0D6E/@0x0DC4 den Eintrag auf `ff 00`. Datei COMMON/SOUND/MAIN1A.BGM (228152 B, md5
  01835748815e388e2519d4977d48aab5), Containeranhang wie RE1.5 (u32 @size-4 = 0 SEQ, @size-8 = 0x64 VH, @size-12 = 0xE88 VB;
  FUN_8005a97c).
