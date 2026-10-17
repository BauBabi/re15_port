# Runde 38 — Spur S "affen3" (Stage 3: die kleinen Affen treffen nicht)

Baum: `.claude/worktrees/r38_affen3`, Zweig `r38/affen3`, Basis master cb782bda. Beginn 2026-10-16.

## Auftrag (woertlich, AUFTRAG.md)
> - In Stage 3 die kleinen Affen können einen nicht treffen, es passiert nichts

Zuteilung (VERTRAG.md §0): "S | affen3 / r38_affen3 | Stage 3 kleine Affen treffen den Spieler wie im Original".
Keine Bank-9-Bits, keine Nachrichten-Ids, keine Items zugeteilt (keine noetig).

## Protokoll (fortlaufend)

### 2026-10-16 Start
- Baum geprueft: `git status --porcelain` leer, HEAD cb782bda, Zweig r38/affen3.
- AUFTRAG.md + VERTRAG.md vollstaendig gelesen.

### Schritt 0 — Welcher Gegner ist "die kleinen Affen"? (Zensus Stage-3-RDTs)
`python re15_port/tools/scd_dump_room.py <RDT> | grep Sce_em_set` ueber alle 32 Stage-3-RDTs
(ROOM3000..30F1). Opcode 0x44 `44 <slot> <typ> <grid> ...`. Gegnertypen ausser Zombies (0x10/0x11),
NPCs (0x40/0x42/0x49/0x4b) und Birkin (0x30/0x36):
- **Typ 0x29: ROOM3040 @0x01310 / @0x01324 und ROOM3041 @0x01310 / @0x01324** (je 2 Stueck,
  `44 00 29 00 01 ...` / `44 01 29 00 01 ...`, grid 0x00).
- Typ 0x20 in ROOM3060/3061 (3 Stueck) — STAGE3 registriert fuer 0x20 KEINEN Handler (Registrierungs-
  Scanner, memory reai-v2-enemy-roster-complete: STAGE3 = 0x29, 0x30+0x36, 0x40). Wird unten geprueft.
- Modell EM029 (CDEMD0.EMS Blob 13, s_ems_order Index 13 = 0x29): Ansichtsblatt
  `python re15_port/tools/emd_ansichtsblatt.py --typ 0x29 --clip 0` -> ein **kleiner Affe** (Fell, Affengesicht
  mit Reisszaehnen, Vierbeiner-Haltung, 18 Knochen, 19 Meshes, 32 Clips, Hoehe 1622 Einheiten). Der Port nennt den
  Typ nach dem RE2-Vorbild "COCKROACH" (BioModels.h: RE2 EM029 = COCKROACH) — in RE1.5 ist EM029 der kleine Affe.
  Der Gorilla (0x27, EM027, ROOM11C0, Stage 1) ist der GROSSE Affe und wird 1,7-fach skaliert gezeichnet.
- Port-KI: `re15_cockroach_ai_tick` (engine/src/enemy_ai_common.c), Wurzel STAGE3.BIN 0x80110b00.

### 2026-10-16 (Fortsetzung nach Sitzungslimit) — Schritt 0 kritisch nachgeprueft
Der Vorgaenger stuetzte "Typ 0x29 = kleiner Affe" auf die Reihenfolge `s_ems_order` (re15_ems.c), die aus dem
Java-Extraktor stammt, nicht aus dem Original. Andere Agenten nannten 0x29 "Kakerlake" (RE2-Name: in RE2 ist EM029 die
Kakerlake). Beleg jetzt aus dem ORIGINAL-Lader:
- **PSX.EXE FUN_80022300 (Gegnermodell laden)** @0x80022324 `lbu v0,0x8(s2)` (entity+0x8 = Typ) / @0x80022338
  `addiu a0,v0,-0x10` / @0x80022344 `sll v1,a0,0x3` / @0x80022378 `lw v0,0(0x80072f38+v1)` (Sektor) / @0x8002235c
  `lw v0,0(0x80072f3c+v1)` (Groesse). Zweig `DAT_800aca3c >= 0` -> Tabelle @0x80072f38 (CDEMD0, Dateinummer 0x26),
  sonst @0x80073178 (CDEMD1, 0x27).
- Tabelle @0x80072f38, Eintrag (0x29-0x10)=25 @0x80073000 = `e2 03 00 00 d8 64 03 00` = Sektor 994, Groesse 0x364d8
  = 222424 Byte (PSX.EXE Datei-Offset 0x63800). CDEMD0.EMS Blob 13 liegt bei 0x1f1000 = 994*2048, Laenge 222424 -> exakt
  dieser Blob. Gegenprobe aller 25 belegten Eintraege: Sektor/Groesse decken sich mit s_ems_order (0x27 -> Sektor 892 =
  Blob 12, 0x2B -> 1103 = Blob 14, ...). Die Extraktor-Reihenfolge ist damit byte-true.
- Blob 13 gerendert (`emd_ansichtsblatt.py --typ 0x29 --clip 0x16`): ein **Menschenaffe** (Fell, Affengesicht mit
  Reisszaehnen, aufgerissener Ruecken mit freiliegendem Fleisch, Knoechelgang), 18 Knochen, 19 Meshes, 32 Clips.
  KEINE Kakerlake. "Cockroach"/"roach" im Port-Code ist nur der RE2-Name; in RE1.5 ist Typ 0x29 der kleine Affe.
- Die grossen Affen in ROOM11C0 sind Typ 0x27 (Tabelle Eintrag 23 -> Sektor 892 = Blob 12), Stage 1.
- Stage 3 kennt sonst keinen Kandidaten: 0x20 (ROOM3060/3061) hat in STAGE3.BIN keinen Handler (Registrierungs-Scanner).
**Zuordnung bestaetigt: "die kleinen Affen in Stage 3" = Typ 0x29, ROOM3040/3041 (je 2), Handler STAGE3.BIN 0x80110b00.**

## Messung vorher (Port, master-Stand cb782bda, gebaute exe dieses Baums)
Messkopie `re15_pc_r38s_mess.exe` neben der exe. Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=3040@250 RE15_STATE_LOG=state.log`.
- **m1** (RE2-KI, Leon am Sprung-Spawn (-25600,-1350), steht still, 1800 Bilder): beide Affen (Platz 1 (-8800,-6150),
  Platz 2 (-13500,-11800), HP 87/99) wechseln nach ~100 Bildern in Sub 3 (Annaeherung) mit Clip 4 und kriechen mit ~3
  Einheiten/Bild; Affe 2 bleibt bei (-13600,-6900) haengen, Affe 1 bei (-13332,-2132). Keiner erreicht Leon.
- **m2** (RE2-KI, `RE15_PLAYER_POS=-9100,-2600,1024` = 2300 vor Affe 1, 1200 Bilder): Affe 1 geht in Sub 3 (Clip 5), bei
  Mittenabstand 2740 in **Sub 5 = BISS (Clip 0x12)**, spielt den Clip ganz (Bild 0..24), **Leon hp=100 bleibt**, zurueck in
  Sub 3 Clip 4, kriecht von z=-4878 nach -5072 RUECKWAERTS und wieder auf -4878, erneut BISS — Schleife alle ~45 Bilder,
  ueber 1200 Bilder **kein einziger HP-Abzug, keine Reaktion**. Genau der Nutzer-Befund ("es passiert nichts").
  Auszug state.log: `F50 ... PL(-9100,-2600,rot=1024,hp=100) ... [1 t=29 st=1 ss1=5 ... mo=18 af=0 ... @(-9118,-5343,...)]`
  ... `F95 [1 ... ss1=5 ... mo=18 af=0 @(-9118,-4878)]` ... `F237 hp=100`.
- Ursache im Port (enemy_ai_common.c re15_cockroach_ai_tick): der BISS prueft `re15_dog_arc(e, pl, 1000, 0x400)` =
  MITTEN-Abstand < 1000. Die Koerperschiebung haelt Affe und Leon auf >= 1100 + 450 = 1550 (Kasten des Affen
  STAGE3.BIN @0x8011ec14 = {0,-1080,0, 1100, 1080, 1100}, Leon 450 @0x80073e94) -> die Bedingung kann NIE wahr werden.
  Der SCHWERE Angriff (Sub 6) prueft `dist < 800` -> ebenso unmoeglich. Die Annaeherung (Sub 3) schiebt per
  `re15_clip_root_motion_delta` statt per Fuss-Sperre -> Hin-und-her ohne Netto-Weg.

## RE-Belege (Original, selbst disassembliert, STAGE3.BIN = Overlay @0x80100000 ohne Kopf)
**Der kleine Affe 0x29 ist der Code der Gorilla-Familie 0x27 (STAGE1) mit anderen Konstanten.** Normalisierter
Befehlsvergleich (Tabellenadressen ausgeblendet): B[5] 181/181 Befehle gleich bis auf Fenstertabelle, SE und Schaden;
B[6] 236/237; B[3] und B[4] identisch; die A-Entscheider sind beim Affen schlanker (keine Zonen-Spruenge).
Zustandstabelle @0x8011eca4 = {INIT 0x80110c98, ACTIVE 0x8011101c, HURT 0x80114790, DEATH 0x80114fb4, 0x80115734 x2,
0x80116228, CORPSE 0x80115a6c}; A-Tabelle @0x8011ecc4, B-Tabelle @0x8011ed04 (je 16 Eintraege, wie der Gorilla).
- ACTIVE 0x8011101c: A[+0x5] `jalr` @0x8011110c, +0x5 NEU gelesen @0x80111120, B[+0x5] `jalr` @0x80111140 — im SELBEN
  Tick (wie Gorilla, re15_affen.h (10)); +0x1dc-- danach @0x801111c0-d4.
- INIT-Schwanz: `+0x0 |= 0x800` @0x80110f44-48, **+0x166 = 0x14cc** @0x80110f54-58 (Massstab 1,3; Gorilla 0x1b33);
  Kasten +0x78 = *(0x8011ec44 + 4*(+0x1e4)) @0x80110f68-88, +0x1e4 = (grid&8)?4:0 @0x80110e4c-70; Part 18 an Record 1,
  rel (0x66,-810,0) @0x80110fc8-1008 (wie Gorilla @0x80117200-3c).
- **B[5] BISS 0x80111d68**: Phase 0 @0x80111dcc-dfc: +0x6=1, Clip 0x12, +0x95=0, +0x8f=7. Jeder Tick: anim_set
  `jal 0x8001f314` (a3=0x200) @0x80111e14, +0x6 += Rueckgabe; **Fuss-Sperre `jal 0x80115b68` (0,0)** @0x80111e34;
  Versatz a1 = (0x64,0,0) (`ori v0,zero,0x64` / `sw v0,16(sp)` @0x80111e78-7c auf 0x80072d60 = (0,0,0)); SE 9 bei
  +0x95 == 0xb @0x80111e8c-a0; Tor Spieler+0x93 == 0 @0x80111ea4-b4; **Treffer `jal 0x8001bff8`(Pool+1612 = Record 9
  +0x40, a1, r = 0x3e8, &Spieler+0x34)** @0x80111eb8-c8 -> +0x1da; Fenster +0x95 in Tabelle @0x8011ed48 = {12,13,14,15}
  @0x80111ef4-24; Treffer: **Spieler-HP -= 5** @0x80111f54-64, +0x1dc = 0x2d @0x80111f68-70, **SE 5** (a0=5
  @0x80111f4c), aca58 = 2 @0x80111f7c, aca59 = FUN_8001a780(Spieler)+2 @0x80111f80-94, aca5a = 0 @0x80111f9c,
  HP < 0 -> aca58 = 3 / aca59 = 0 @0x80111fa0-b4, Spieler+0x93 |= 1 @0x80111fb8-cc. Phase 2 @0x80111fd8-2018:
  +0x1dc == 0 -> 0x14; +0x5 = 3, +0x6 = 0, **+0x7 = 0** (Port setzte 1).
- **B[6] SCHWER 0x80112044**: Phase 0 @0x801120ac-dc Clip 0x13; anim_set @0x801120f4; Fuss-Sperre nach +0x95 (nach
  dem Vorschub): < 6 -> `jal 0x80115c3c`(0,0) @0x80112138, < 0x15 -> `jal 0x80115b68`(0,1), sonst (0,0) @0x80112148-5c;
  SE 9 bei 0x14 @0x80112170-84; Tor +0x93 @0x80112188-98; zwei Treffer `jal 0x8001bff8` Pool+1096 (Record 6) und
  Pool+1784 (Record 10), a1 = (0,0,0) aus 0x80072d60, **r = 0x320** @0x801121a0-208; Fenster @0x8011ed4c =
  {21,22,23,24,33,34}; Treffer: **HP -= 10** @0x801122a8-b8, +0x1dc = 0x2d, **SE 4** (a0=4 @0x80112290), aca58 = 2,
  aca59 = a780 + **4** @0x801122d4-e8 (Niederschlag), HP<0 -> 3/0, +0x93 |= 1. Phase 2 @0x8011232c-3cc: +0x93 == 0 und
  FUN_8001a804(0xfa0, 0x180) < 0 -> (+0x1dc == 0 ? 5 : 3), sonst (+0x1dc == 0 -> +0x1dc = 0x14) und 3; +0x6 = +0x7 = 0.
- **A[3] 0x801117d8**: +0x93 == 0 und a804(0xbb8, 0x180) < 0 und +0x1dc == 0 -> +0x5 = 5 @0x801117e8-824; sonst bei
  +0x93 == 0: Abstand(+0x1d4) < 0x1771 und Sicht(+0x1d0&1) und +0x7 != 0 -> +0x6 = +0x7 = 0 @0x80111848-890; Abstand >=
  0x1771 und Sicht -> +0x5 = 4 @0x801118b4-8fc.
- **B[3] 0x80111910**: Phase 0: Clip 4 @0x80111940, +0x7 == 0 -> Clip 5 + +0x9e = (rng&0x1f)+2 @0x80111958-78, +0x95 =
  0, +0x8f = 7; jeder Tick +0x9e = (rng&0x1f)+6 @0x8011199c-b4, Lenken FUN_8001a8f8(&Spieler, +0x9e) @0x801119d0,
  anim_set @0x801119ec, **Fuss-Sperre `jal 0x80115b68`(0, +0x95 < Fenster ? 1 : 0)**, Fenster 0x18 (+0x7 == 0)
  @0x80111a18-30 / 0x29 @0x80111a64-7c, SE 8 bei +0x95 == 1 oder == Fenster @0x80111a44-ac.
- **A[4] 0x80111ac0**: +0x93 == 0 und a804(0xfa0, 0xc0) < 0 und +0x1dc == 0 -> +0x5 = 6 @0x80111ad4-b18.
- **FUN_80115b68** (= Gorilla FUN_8011bf50): Kette +0x20 * Record 0 * Record (12+3a1) * (13+3a1) * (14+3a1)
  (`jal 0x80022da0` @0x80115b98/bbc/bcc/bdc, Block a1*0x204 + 0x968), +0x34 -= m.tx - rec[84] @0x80115be4-c00,
  +0x3c -= m.tz - rec[92] @0x80115c04-20 -> verfolgter Knochen 14 + 3*a1.
- **FUN_8001bff8** (PSX.EXE): Matrix(Record+0x40) * (I | a1) (`jal 0x80022da0` @0x8001c078), dann
  `(plX - ptX + r) & 0xffff > 2r -> kein Treffer` @0x8001c080-a0, Z ebenso @0x8001c0a8-c0 = achsenparalleles Quadrat.

## Original gemessen (DuckStation-GDB, MZD-Disc, ohne Pad)
Werkzeug (Scratch, nicht im Baum): `affen_spur.py` — DuckStation per `Start-Process -batch -statefile
stage_saves/mzd_debugmenu.sav <MZD-cue>` (settings.ini `[Debug] EnableGDBServer = true` nur waehrend der Messung,
Original gesichert und danach zurueckgespielt), GDB-Port 2345. Raumsprung OHNE Menue/Pad: am Halt der Hauptschleife
0x8001c994 (erst wenn der Savestate aktiv ist: 0x800b0fe2 == 0x24) Stage-Index 0x800bbe5e := 2, Raumindex
0x800bbe61 := 4, Tor 0x800ac9a8 := 0 (@0x8001d618), Sprungflag 0x800b5359 := 1 — genau der Schreibsatz des
Menues @0x80014a44-58, den die Hauptschleife @0x8001c994 liest. Danach Haltepunkt `Z0` auf die Affen-Wurzel
0x80110b00; je Bild (cur == Entity Slot 0 = 0x800acc2c) Spieler 0x800aca54, Entity 0/1 (je 0x1f4 B), aca58, VSync.
- **Code-Identitaet:** RAM 0x80110b00..0x80116230 (22320 B) nach dem Sprung == info/Re1.5/PSX/BIN/STAGE3.BIN
  Bytes 0x10b00.. — **0 Abweichungen**. Die MZD-Disc faehrt also genau den ausgelieferten Affen-Code.
- **o1 (Leon am Sprung-Spawn (-25600,0,-1350), steht, 1500 Bilder):** beide Affen y = -1800, +0x82 = 1
  (Plattform-Ebene, Band 1). A: Spawn (-8800,-6150) -> nach dem ersten Klemmen (-8800,-6082), Ruhe-Clip 0x16
  ~290 Bilder, dann Sub 3 blind (Clip 4) und **steht fest bei (-8707 +-12, -6082)**, r 2227, +0x1d6 = 1
  (Wandkontakt), Sicht 0. B: (-13618,-11800) -> Sub 3, kriecht +z mit ~9-11 Einheiten/Bild, **steht ab ~F815 bei
  (-13610,-6910)**. Leon hp 100, kein Angriff.
- **o2 (Leon auf dem Boden (-9100,0,-2600), Band 0, 2000 Bilder):** A sofort Sicht 1, Sub 3 sehend (Clip 5),
  **steht bei (-8707..-8725, -6082)**, r 2997..3001, Abstand 3500 -> der Biss-Commit a804(0xbb8) wird NIE wahr;
  B steht bei (-13618,-10663). **0 HP-Abzuege in 2000 Bildern.** Die Affen erreichen Leon von der Plattform aus
  hier im Original NICHT.
- **Port mit dem neuen Code, dieselben Aufbauten:** p1 (Spawn): Affe 1 steht ab F~100 bei (-8707 +-12, -6082)
  r2227, Affe 2 kriecht von -11800 nach +z und steht bei (-13600 +-20, -6900) — **wie o1**. n2 (Boden
  (-9100,-2600)): Affe 1 Sub 3 Clip 5 bei (-8707..-8725, -6082) r2997..2999 — **wie o2**. Der alte Port schob den
  Affen dagegen per Clip-Wurzelweg durch die Wand bis z = -4878 und biss dort ins Leere (m2).
- **o3 (Leon sofort auf der Plattform (-8800,-1800,-4600), Band 1):** ORIGINAL-FEHLER beobachtet — Affe A wechselt
  im ERSTEN aktiven Bild (Abstand 1481 < 0xbb8) in Sub 3; die Fuss-Sperre FUN_80115b68 zieht die noch nie
  gezeichneten Records (Weltlage ~0) ab und versetzt ihn in EINEM Bild von (-8800,-6082) nach (245,747). Nur ein
  Mess-Artefakt (Leon steht beim Aktivwerden schon im Bereich); der Port bildet das nicht nach (sein Zeichenstand
  wird im INIT angelegt). Fuer die Treffer-Messung deshalb o4.
- **o4 (Leon erst nach 120 aufgezeichneten Bildern auf die Plattform (-8800,-1800,-4600) -> Schub -> -4531):**
  F137 Leon erscheint, A ruht (Clip 0x16 Bild 56, (-8800,-6082), r3072); F138 A Sub 3 blind (Clip 4) bei
  (-8707,-6082); **F139 Sub 5 = BISS, Clip 0x12** (A und B im selben Tick); x waehrend des Bisses -8698, -8675,
  -8666, -8683.., -8652, -8532 (Fuss-Sperre schiebt OSTWAERTS, z bleibt an der Wand -6082); **F150 = Bild 12:
  TREFFER, Leon HP 100 -> 95**, aca58..5b = 02 02 01 00 (cmd 2, aca59 = 2 = a780 0 + 2), +0x1dc = 44 (0x2d, im
  selben Tick um 1 abgebaut), Leon-Rueckstoss x -8800 -> -9000/-9150/-9250/-9300 (200/150/100/50), Flinch bis F171;
  F163 Biss-Ende -> F164 Sub 3 mit +0x7 = 0 (sehend); **F195 zweiter Biss, F207 = Bild 13: 95 -> 90**; danach
  GLEICHTAKT: Treffer alle **57 Bilder** (F264, 321, ... 1233) je **-5** bei Bild 13, Affe bei (-8297,-6082) r2813;
  F1290 HP 0 -> -5 -> aca58 = 3 (Tod). 21 Bisse, kein schwerer Angriff (Leon steht nie >= 0x1771 entfernt).
- **Port p4 (dieselbe Lage, Leon aber schon ab Bild 0 da):** Treffer -5 alle 59 Bilder bei Bild 15, Affe bei
  (-9118,-4240) — er ist im allerersten Biss (ohne Ruhephase, Pool-Ueberblendung vom INIT-Clip 0) durch den Gang
  nach Norden gerutscht. Mechanik (Sperre 0x2d, Fenster, -5, cmd 2) gleich, Lage verschieden, weil die Vorgeschichte
  verschieden ist -> der Riegel startet den Port aus dem Original-Zustand F137 (s. Tests).

### 2026-10-17 (zweite Fortsetzung nach Sitzungslimit ~23:45)
- Baum geprueft: `git status --porcelain` leer, HEAD 930e385c (Vorgaenger-Commits 49a4b741..930e385c), Basis cb782bda.
  master hat seit cb782bda nur Doku-Commits (Orchestrator); Spur T (spiderman) liegt in r38/integration, Spur B
  (szenenschutz) noch in ihrem Baum (Nachbesserung 1).
- Offen beim Abbruch: 930e385c (integration-Riegel: Leon-Gier 0 statt 3072, Folgebild-Pruefung Clip 8 statt pst=2)
  UNGEPRUEFT, nichts gebaut/getestet seit da97247d, Messung nachher an der exe, Tests-Abschnitt, OFFEN, Fuer den Nutzer.
- Bau gestartet (local_build.sh configure + build).

## Messung vorher/nachher an der EXE — Weg wie im Spiel (2026-10-17)
Der alte Integrations-Riegel (930e385c, UNGEPRUEFT) setzte Leon schon in Bild 0 neben Affe A (Lage o3). Gemessen
(lauf_90edaa30): der Affe beisst im 3. aktiven Bild und rutscht dabei durch den Gang nach Norden ((-8713,-6082) ->
(-9118,-5743) -> -5111 -> -4470 je Bild), Leon flincht deshalb mit Clip 9 statt 8 -> "F68/F127: kein Clip 8" ROT. Der Fall
entspricht o3 (Original-Fehler beim allerersten aktiven Bild, s. oben), nicht dem Spiel. Neuer Aufbau, so wie der Nutzer
auf die Affen trifft: Leon steht im Nordgang der Plattform (-8800,-2400, Band 1, Gier 1024 = Blick nach Sueden), Affe A
sieht ihn und wartet in Sub 3 an seiner Nische (-8707..-8722,-6082) wie im Original o2; nach 5 s geht Leon 25 s geradeaus
auf ihn zu (`RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-8800,-2400,1024,1 RE15_INPUT_SCRIPT_START=250
RE15_INPUT_SCRIPT=W5,U25 RE15_EXIT_AT=1000#3040`).

| Lauf | Stand | KI | HP-Abzuege in 1000 Bildern | Bisse | Affe A |
|---|---|---|---|---|---|
| v1_std  | master cb782bda (Quellen per `git archive`, eigener Bau im Scratch) | RE2 (Standard) | **0** | 22 Biss-Starts (alle ~45 Bilder) | rutscht in den Gang bis (-9118,-4893..-4981) |
| v1_re15 | master cb782bda | RE1.5 | **0** | 531 Biss-Bilder | dito |
| w1_re2  | dieser Baum (Stand 930e385c + Riegel) | RE2 (Standard) | **15 x -5** | Treffer F169 (Bild 14), F225 (Bild 12), dann F282..F909 alle **57** Bilder in **Bild 13** | steht an der Nische, Lunge ostwaerts -8707 -> -8127; Gleichtakt bei **(-8307,-6082) r2787** |
| w1_re15 | dieser Baum | RE1.5 | **15 x -5** | identisch zu w1_re2 | identisch |

Vergleich Original o4 (GDB-Spur, s. oben): Gleichtakt-Treffer alle **57** Bilder in **Bild 13**, -5, Affe bei
**(-8297,-6082) r2813**; Rueckstoss 200/150/100/50 — Port w1: z -4125 -> -3925/-3775/-3675/-3625 = **200/150/100/50**;
Flinch-Clip im Folgebild **8** (a780 = ((1024-3053+0x400)&0xfff) = 3091 >= 0x800 -> 0 -> aca59 = 2). Affe B (blind, Sub 3
Clip 4) kriecht im Port von -11800 nach (-13618,-10663..-10680) — Original o4 B: (-13618,-10662..-10691).
Langer Lauf w2_tod (U45, bis Bild 1700): 21 Treffer, HP 0 -> -5 in F1308 (Leon Clip 7), Spieler-Zustand 7 ab ~F1430,
danach der Tod-Ablauf des Ports (zurueck zum Titel) — der Affe toetet Leon wie im Original o4 (F1290 HP 0 -> -5, cmd 3).
**Nachtrag (o10, s. unten): der Vergleich w1 <-> o4 war ein Szenen-Mismatch.** In o4 STEHT Leon, in w1 GEHT er weiter in
den Affen hinein. w1 lief noch ohne den Selbst-Schub des Affen (aec4 @0x80110b88, s. Umsetzung) — der Affe wurde deshalb
nicht geschoben und landete beim Steh-Takt 57 von o4. Das Original auf demselben Weg (o10) hat Takt 56 und schiebt den
Affen nach Westen; der jetzige Stand trifft genau das.

## Weitere Treffer-Wege des Originals (Subs 7..15) — Befund 2026-10-17
Der Vorgaenger hat nur die Subs 0..6 umgestellt; 7..15 liefen weiter im alten Port-Code ("LEAP-ESCAPE / FLIGHT-ESCAPE
-> idle"). Selbst disassembliert (STAGE3.BIN, Tabellen A @0x8011ecc4 / B @0x8011ed04):
- **B[7] 0x801123fc Sprung** (Clip 0x14, +0x8c (rng&0x1f)+180 / beim Absprung Bild 0xa (rng&0x1f)+200, +0x1e0 = 1,
  ballistisch FUN_8001c1a4(+0x8c, 600, -60, +0x1ba) @0x8011261c, Landung -> SE 1, +0x8c 100, Phase 3 -> Sub 4); in der
  Luft bei Bild 0x13: **Spieler-HP < 50** (`slti v0,v0,50` @0x8011269c) + +0x93 == 0 + a804(0x1388,0xc0) + +0x1dc == 0
  -> **Sub 8** + SE 6 @0x801126c0-710.
- **B[8] 0x80112784 Todessprung** (Clip 0x15): Quadrate Records 6/10 r 0x320 (@0x80112a34/@0x80112a54), Fenster
  @0x8011ed54 = {8,9,10}; Treffer: **HP -= 500** (`addiu v0,v0,-500` @0x80112b00), +0x1dc = 0x12c, SE 0, aca58 = 6
  (`sw` @0x80112b24), acbfc/acbcc/acbd0 = Affe/+0x178/+0x17c, +0x93 |= 1, Wundstempel FUN_80037edc(0,0x32) @0x80112b64.
- **HURT-Ausgang** (Spur 0 @0x801149ac-a2c): +0x1e3 (INIT-Standard 1 @0x80110e18) -> **Sub 9**. Port: dorthin, aber der
  alte Sub-9-Code spielt nur Clip 0xc am Platz und kehrt nach Sub 0 zurueck (gemessen s2_schuss, Browning RE15_EQUIP=3:
  Treffer 87 -> 73, HURT, Sub 9 Clip 12 am Platz (-8720,-6082), F343 Sub 0, F344 Sub 3 — der Affe bleibt in seiner Nische).
- **B[9] 0x80112d48 Sprung an die Decke** (Phasen @0x801002d4): Clip 0xc, Bild 0x10 -> +0x1e0 = 1, Kasten +0x78 =
  @0x8011ec48[+0x1e4] (**Radius 800** statt 1100, Hoehe 1530 @0x8011ec20), FUN_8001c1a4(0, 0x190, -20, +0x1ba) bis
  y = +0x1ba - 2700 (@0x80112f10-28) -> Sub 12.
- **B[12]/A[13]/B[13]** Decke: kriechen (Clip 0xf / 0x18, Fuss-Sperre FUN_80115c3c), Lenken auf den Spieler;
  **B[14] 0x80113e90 Hieb von der Decke** (Clip 0x1a): Fenster @0x8011ed5c {8..13} Record 6 / @0x8011ed64 {43..48}
  Record 10, r 0x320; Treffer **HP -10** (@0x801141f4), +0x1dc 0x2d, SE 5, aca58 = 2, aca59 = a780+2.
  **B[15] 0x801142c4 Griff von der Decke** (A[13]: Abstand < 0x7d0, a780(Spieler) != 0 = Spieler kehrt den Ruecken zu,
  +0x1dc == 0 @0x80113a24-a94): y = +0x1ba + 400, aca58 = 5, Opfer-Bank +0x178/+0x17c, Ringen +0x9c = 100 - Druecken
  (FUN_80037024), danach Fall auf den Boden (y = +0x1ba, Kasten zurueck) -> Sub 2. **B[10] 0x801130f8** Absprung von
  der Decke -> Sub 4.

### Original gemessen (DuckStation-GDB, Werkzeug affen_spur2.py = affen_spur.py + `--write Bild:Adresse:Bytes`)
Aufbau wie w1 (Leon (-8800,-2400) Band 1), in Bild 150 Affe A in den HURT gesetzt (+0x4..7 = 02 03 00 00, HP 73) —
dieselbe Lage wie nach einem ueberlebten Treffer (Spur 0 -> Ausgang Sub 9).
- **o7 (Leon blickt zum Affen, Gier 1024):** F180 Sub 9, F205 y -1800 -> **-4500** (Decke), Kasten 0x8011ec20; F277
  Sub 12 (Clip 15), F327 Sub 13 (Clip 24) — der Affe kriecht AN DER DECKE durch den Gang (z -6371 -> -4994 -> -3763),
  den er am Boden (Radius 1100) nicht passiert; F377.. Sub 14 Hiebe (Clip 26) im Wechsel mit Sub 13, alle ~71 Bilder;
  **kein HP-Abzug** in 1600 Bildern (Leon wird nur geschoben). Kein Griff (a780 = 0: Leon blickt den Affen an).
- **o8 (Leon kehrt den Ruecken zu, Gier 3072):** wie o7 bis F337; **F341 Sub 15 = Griff**: Affe faellt auf
  (-8818,-1400,-3283), Spieler cmd 05 (aca5a 1 -> 2 -> 3 -> 5 -> 6 -> 7 -> 9), Spieler-Clips 1/2/3/16/11 aus der
  Opfer-Bank; **kein HP-Abzug im Griff**; F532 Affe auf dem Boden (-9118,-1800,-3547), Kasten 0x8011ec14, F552 Sub 2,
  F573 Spieler frei; danach Bisse aus dem Gang: **F625, 683, 741, 799, 857, 915, 973, 1031 je -5** (Leon von hinten:
  aca59 = 3, Clip 9). Der Affe verlaesst seine Nische also im Original ueber die Decke.
Folge fuer den Nutzer-Punkt: ein angeschossener Affe greift im Original von der Decke an und kommt aus der Nische,
im Port bleibt er dort. Wird in diesem Baum nachgezogen (Subs 7..15 nach den obigen Adressen).

## Umsetzung Subs 7..15 (2026-10-17) — affen3_r38.c traegt jetzt das ganze ACTIVE-Gehirn
Alle 16 Subs laufen in `re15_affen3_aktiv` (A[+0x5], dann B[+0x5 neu] im selben Bild, auch wenn A den Affen gerade in
den HURT gestuerzt hat); der alte Port-Code fuer 7..15 in `re15_cockroach_ai_tick` ist damit tot (bleibt stehen, nur
Haken). Schwanz: Zielbaender @0x80111148-1b0 (Boden LEVEL + DOWN bei Abstand < 0xfa0, in der Luft/an der Decke NUR UP bei
< 0x1388; Abstand nach dem B neu wie FUN_80012974/FUN_80012a0c @0x800129c0/@0x80012a58).
| Teil | Original | Port |
|---|---|---|
| Sprung B[7] | 0x801123fc: Clip 0x14, +0x8c (rng&0x1f)+180, Bild 0xa Absprung (rng&0x1f)+200, +0x1e0 1, +0x93 \|= 1, c1a4(+0x8c,600,-60) | `b_sprung` |
| Todessprung B[8] | 0x80112784: nur aus B[7] Bild 0x13 bei Spieler-HP < 50 (@0x8011269c), Quadrate Rec 6/10 r 0x320, Fenster {8,9,10} @0x8011ed54, HP -500, +0x1dc 0x12c, SE 0, aca58 6, Wunde (0,0x32) | `b_todessprung`; Spieler-Haken cmd 6 0x80116064 -> 0x801160a4 ist befehlsgleich zum Gorilla-Finisher 0x8011c3d4 -> 0x8011c414 -> derselbe Port-Weg (re15_player_victim_devour / _tick: 0x29 wie 0x27) |
| Decke hoch B[9] | Phasen @0x801002d4; Bild 0x10: +0x1e0 1, Kasten @0x8011ec48[+0x1e4] (r 800, h 1530); c1a4(0,0x190,-20) bis y = +0x1ba - 2700 | `b_decke_hoch` |
| Decke runter B[10] | Phasen @0x801002ec; Bild 0x18 halten, c1a4(+0x8c,0,-70), Landung SE 1, Bodenkasten, -> Sub 4 | `b_decke_runter` |
| Drehen B[11] | Clip 0x1b, Bilder 1..7/17..20: +0x9e (rng&0x1f)+80, +0x1da = FUN_8001a9cc, Gier += +0x1da; +0x1da == 0 -> Sub 12, ohne Sicht oder Abstand >= 0x1389 -> Sub 10 | `b_decke_drehen` (+0x1da = Port-Feld ai_attack_timer) |
| Kriechen B[12] | Phasen @0x80100304: Clip 0xf (-> Sub 13 bei Bild 0x32), Clip 0xf ab 0x32 / Clip 0x19 (-> Sub 11), Fuss-Sperre c3c | `b_decke_kriechen` |
| A[13] | Flug-Treffer; Abstand < 0x7d0 + a780 != 0 + +0x1dc 0 -> Sub 15 (Griff); Bild {0,0x31} + a804(0x1388,0x180) -> Sub 14 (Hieb); Bild {0,0x31,0x18} + Abstand >= 0xfa1 + keine Sicht -> Sub 12 Phase 2/5 | `a_decke` |
| Jagd B[13] | Clip 0x18, Bilder 5..12/33..37 Lenken +0x9e (rng&0x30)+48 (+24 unter 0x7d0), c3c(0, Bild < 0x19) | `b_decke_jagd` |
| Hieb B[14] | Clip 0x1a, Fenster @0x8011ed5c {8..13} Rec 6 / @0x8011ed64 {43..48} Rec 10, r 0x320; HP -10, +0x1dc 0x2d, SE 5, aca59 a780+2 | `b_decke_hieb` |
| Griff B[15] | Phasen @0x80100324: y = +0x1ba + 400, Anker = Spieler, aca58 5, Ringen +0x9c 100 - 1 - 100*Druecken (FUN_80037024), Loesen Clip 0x1e, Fall auf den Boden, Sub 2 | `b_griff` |
| Spieler-Haken cmd 5 | 0x80115d30 -> 0x80115d70, aca5a-Phasen @0x80100364: Clip 1/2/3 aus der Opfer-Bank mit ad68-Platzierung, Blut 0x1500 an Part 8, Clip 0x10 und 0xb (a2 = 1) aus Leons Bank, aca58 1 | `re15_affen3_opfer_tick` (Haken in re15_player_victim_tick) |
| Flug-Treffer A[9..14] | +0x93 & 2: & 0x40 -> HURT +0x5 9 +0x6 3 (Absturz), sonst Blut | `luft_treffer` |
| HURT/DEATH-Kopf | +0x1e0 -> c1a4(0,0,-50), Landung Kasten + SE 1 (@0x801147a0-804 / @0x80114fc4-5028) | `re15_affen3_luft_landung` |
Weitere Haken (je 1-3 Zeilen, "Runde 38 Spur S"): enemy_ai_common.c — Affen-Root ruft jetzt aec4(&Spieler, Affe)
(`jal 0x8002aec4` @0x80110b88; im Port hatte nur der Hund diesen Selbst-Schub) und klemmt JEDES Bild mit Band +0x82 und dem
aktuellen Kastenradius (@0x80110ba0-cc, vorher Bewegungs-Gate + Band aus y); +0x1dc als s16 (dog_blocked_ctr; 0x12c passt
nicht in das u8 dog_atk_cd); Griff-Paar/Opfer-Wege fuer 0x29 (devour, victim_tick, own_bank, re15_player_victim_gorilla);
re15_damage.c — 0x29 liest aim_band wie 0x27; affen_11c0.c — re15_affen_griff_paar fragt re15_affen3_griff_paar.
INIT-Haken: +0x1ba = -(+0x82)*0x708 (Sce_em_set-Saat `sh v0,442(s0)` @0x80042210), Kasten nach grid&8 (@0x80110f68-88).

## Messung nachher Subs 7..15 (Port-Riegel gegen die Original-Spuren, ROOM3040, beide KI-Varianten)
- **o7 Decke** (`unit_r38_affen3_decke`): T13 Sub 9, T38 y -4500 Kasten 800, T110 Sub 12, T111 (-8706,-4500,-6076),
  T160 (-8701,-4500,-6382), T161 (-8685,-4500,-6371), T177 (-8818,-4500,-3646), T210 Hieb (-8987,-4500,-3763) r3075,
  T281 Hieb 2 (-8989,-4500,-3651) r3243 — **alle bildgleich zum Original**; danach weicht die Bahn ab (Ersatz-Zufall),
  Endlage im Wechsel Hieb/Jagd bei (-9222,-2387) r3905 wie im Original; 0 HP-Abzug wie im Original.
- **o8 Griff** (`unit_r38_affen3_griff`): Griff T174 (-8818,-1400,-3283), Haken-Phasen 1 (T174) / 2 (T212) / 3 (T213) /
  5 (T313) / 6 (T364) / 7 Clip 0x10 (T365) / 8 (T380) / 9 Clip 0xb (T381) / 10 (T405) / frei (T406), Leon (-8777,-2711) T210,
  (-9132,-1993) T330, Affe am Boden (-9118,-1800,-3547) T365, Sub 2 T385 — **alle bildgleich**; kein HP-Abzug im Griff;
  danach Bisse T458/516/574/632/690/748/806/864 je -5 = **dieselben Bilder wie im Original**.
- **o9 Sprung/Todessprung** (`unit_r38_affen3_sprung`, neue GDB-Spur o9: Affe auf offenen Boden versetzt, Leon HP 40):
  Original T31 Sub 3, T32 Sub 4 (-16974,-8808), T41 Sprung, T50 Absprung, T72 Landung, T81 Schwer, T102 -10, T284
  Todessprung (HP 20), T293 -500 (cmd 6), T305 Sub 2, Tod; Port T31/T32 (-16977,-8815)/T42/T51/T73/T82/T104 -10,
  Todessprung T205 (HP 30), -500 Bild 8, cmd 6, Sub 2, Tod. 1-2 Bilder Versatz = Ersatz-Zufallsquelle (der Sprung-
  Entscheid zieht rng&1 @0x80111b88-ac; FUN_8001af20 haengt im Original am a0 des Aufrufers, re15_damage.c).
- Alt-Riegel `unit_cockroach_ai` (1) begruendet nachgezogen: INIT schreibt Clip 0 (`sb zero,148` @0x80110ce4), den
  Ruhe-Clip 0x16 setzt erst B[0] (@0x80111348-384).

## Messung nachher an der EXE — angeschossener Affe (Decke, Griff, Bisse)
Aufbau s3 (wie w1, dazu `RE15_EQUIP=3` Browning, Eingabe `W5,M1,MA0.3,M1,L2.2,W40`: zielen, EIN Schuss, 2,2 s nach links
drehen = Ruecken zum Affen): F177 Treffer 87 -> 73 (HURT), F189 Sub 9, F214 an der Decke, F286 Sub 12, F336 Sub 13,
**F350 Griff (Sub 15)**, Leon-Clips 1 -> 2 -> 3 (Opfer-Bank EM029) -> 16 -> 11 (eigene Bank), F541 Affe auf dem Boden, F561
Sub 2, **kein HP-Abzug im Griff**; danach Bisse aus dem Gang **F645, 700, 763, 822, 881, 940, 999 je -5**.
Vorher (Stand 930e385c, Subs 7..15 im alten Port-Code, Messung s2_schuss): Treffer 87 -> 73, Sub 9 spielte Clip 0xc AM
PLATZ (-8720,-6082), F343 zurueck in Sub 0/3 — der Affe blieb in seiner Nische, kein Angriff.
Bildkontrolle (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, Schnitt per RE15_FORCE_CUT 5/6/12, weil der Sprung-Teleport keinen
Zonenwechsel ausloest): F340 Affe haengt kopfueber an der Decke (Schnitt 5/12), F360-F610 Griff von hinten, Ringen, Wurf,
Aufstehen, Affe daneben — Modelle und Opfer-Posen geschlossen, keine zerrissenen Meshes.

## Original o10 — derselbe Weg wie der exe-Riegel (Leon geht in den Affen hinein), 2026-10-17
Anlass: nach dem Selbst-Schub (aec4 @0x80110b88) und dem Klemmen in jedem Bild (@0x80110ba0-cc) lief der alte
exe-Riegel rot (Takt 56 statt 57, Affe wandert). Statt die Erwartung zu raten: das Original auf genau diesem Weg gemessen.
Werkzeug `affen_spur3.py` (Scratch) = affen_spur2.py + `--pad Start:Ende:Wert`: zweiter Haltepunkt auf 0x8003047c in
FUN_80030444 (`jal PadRead` @0x80030464, `sw v0,DAT_800ac758` @0x80030478, `lw t0,-0x4(s0)` @0x8003047c liest das
Pad-Wort zurueck). In den aufgezeichneten Bildern 143..892 schreibt das Werkzeug an diesem Halt 0x800ac758 := 0x1000
(PadRead Bit 12 = Steuerkreuz hoch), bevor der Befehl es liest. Den PC liefert das `g`-Paket (Register 37); DuckStation
beantwortet `p25` nicht — der erste Versuch las deshalb keinen PC, speiste nichts ein, Leon stand (verworfen). Aufbau
sonst wie w1: Leon (-8800,-2400) Band 1, Gier 1024. settings.ini danach wieder `EnableGDBServer = false` (Sicherung
zurueckgespielt, ohne BOM, `diff` leer).

Ablauf o10: F143 Pad an, F161 Leon geht (aca58/59 = 01 01, Clip 5), F169 Biss-Entscheid (Abstand 2985 < 0xbb8), 15 Bisse.
Port = dieser Baum, derselbe Aufbau an der exe (`integration_r38_affen3`, Lauf std; der Lauf re15 ist bildgleich).
Leon-Lage im Treffer-Bild: im Original schon nach dem ersten Rueckstoss-Schritt (s. unten), im Port davor.

| Biss | Original: Bild (Clip-Bild) | Affe x | Leon (x,z) | Port: Bild (Clip-Bild) | Affe x | Leon (x,z) |
|---|---|---|---|---|---|---|
| 1 | F182 (14) | -8127 | (-8800,-3903) | F169 (14) | -8127 | (-8800,-4125) |
| 2 | F238 (12) | -8205 | (-9241,-4617) | F225 (12) | -8213 | (-9235,-4802) |
| 3 | F294 (12) | -8068 | (-9357,-5126) | F281 (12) | -8068 | (-9357,-5364) |
| 4 | F350 (12) | -8068 | (-9357,-5459) | F337 (12) | -8068 | (-9357,-5773) |
| 5 | F406 (12) | -8068 | (-9623,-6164) | F393 (12) | -8068 | (-9623,-6179) |
| 6 | F462 (12) | -8312 | (-9644,-6732) | F450 (13) | -8356 | (-9686,-6732) |
| 7 | F518 (12) | -8352 | (-9683,-6732) | F506 (12) | -8371 | (-9703,-6732) |
| 8 | F574 (12) | -8389 | (-9718,-6732) | F562 (12) | -8412 | (-9742,-6732) |
| 9 | F630 (12) | -8428 | (-9757,-6732) | F618 (12) | -8448 | (-9782,-6732) |
| 10 | F686 (12) | -8464 | (-9797,-6732) | F674 (12) | -8488 | (-9817,-6732) |
| 11 | F742 (12) | -8506 | (-9837,-6732) | F730 (12) | -8529 | (-9862,-6732) |
| 12 | F798 (12) | -8545 | (-9876,-6732) | F786 (12) | -8570 | (-9901,-6732) |
| 13 | F854 (12) | -8582 | (-9911,-6732) | F842 (12) | -8608 | (-9940,-6732) |
| 14 | F910 (12) | -8619 | (-9951,-6732) | F898 (12) | -8674 | (-10018,-6732) |
| 15 | F966 (12) | -8685 | (-10029,-6732) | F954 (12) | -8707 | (-10086,-6732) |

- Takt: Original 14 x 56 Bilder; Port 13 x 56 + 1 x 57 (Biss 6 in Clip-Bild 13 statt 12). Jeder Biss -5, Affe z beim
  Treffer immer -6082 (Wand). Der Affe laeuft zuerst bis an den Ost-Anschlag -8068 (Bisse 3..5) und wird danach vom
  gehenden Leon Biss fuer Biss nach Westen geschoben (-8312 -> -8685) — im Port ebenso (-8356 -> -8707).
- Flinch: Bisse 1..5 aca59 = 2 -> Leon-Clip 8, ab Biss 6 aca59 = 3 -> Clip 9 (Original) — im Port dieselbe Folge.
- Biss 1 Bild fuer Bild (Original F174..F187 / Port F161..F174): Affe x -8707 (Clip-Bild 6..9), -8648, -8530, -8383,
  -8237, **-8127 Treffer**, -8066, -8068, -8068, -8081, -8197 — **identisch**.
- Die Unterschiede liegen bei LEON, nicht beim Affen: (a) Gehtempo im Original 76..80 Einheiten/Bild (F174..F181:
  -3557 -> -4103), im Port konstant 75; (b) der Rueckstoss beginnt im Original IM Treffer-Bild (F181 -4103 -> F182 -3903
  -> -3753 -> -3653 -> -3603 = 200/150/100/50), im Port ein Bild spaeter (F169 -4125 -> F170 -3925 -> -3775 -> -3675 ->
  -3625). Beides Spieler-Code ausserhalb dieser Spur (OFFEN). Daraus kommen die kleinen Lageunterschiede ab Biss 2 und der
  eine 57er-Abstand.
- Der exe-Riegel `integration_r38_affen3` prueft jetzt gegen o10 (>= 10 Abstaende genau 56, jeder 50..61, z = -6082,
  ein Treffer am Ost-Anschlag -8068 +-40, letzter Treffer >= 400 westlich davon) statt gegen den Steh-Fall o4.

## Wurzel-Tor +0x9 & 0x20 — gesperrte Ereignis-Affen (Stage 4/5), 2026-10-17
Zensus ALLER RDTs nach `Sce_em_set` Typ 0x29 (scd_dump_room.py): ausser ROOM3040/3041 (je 2, grid 0x00) auch ROOM4000
(@0x01262 grid 0x00, @0x01318 grid **0x61**, @0x0133A 0x00), ROOM4040/4041 und 4080/4081 (Platz 0: **0x61** oder 0x00),
ROOM5000/5001 (0x00), ROOM5031 (@0x010CA **0x61**), ROOM50A0/50A1 (Platz 0/1: **0x61** oder 0x00). Die Wurzel 0x80110b00
(selbst disassembliert, STAGE3.BIN):
`lw v0,0(s0)` (g_pauseflags) / `lui v1,0x2000` / `and` / `bne v0,zero,0x80110bd0` @0x80110b14-20 und
`lbu v0,9(v1)` / `andi v0,v0,0x20` / `bne v0,zero,0x80110bd0` @0x80110b34-40 — gesperrt laeuft WEDER der Zustand (auch
kein INIT) NOCH b498 / aec4 / b544 / Wandklemme; danach nur der Schatten-Block @0x80110bd0 (+0x9 & 0x40). Die Pausen-Haelfte
steht im Port am run_all-Aufruf (game_step_common.c, `RE15_PAUSE_AI`); die +0x9-Haelfte fehlte fuer 0x29. Die 0x61-Affen
gibt erst ihr Skript frei, z.B. ROOM4040 sub04 (Ausloeser Aot_set Platz 5 sce 3 @0x00E9C): Work_set(2,0), Pos_set
(-25090,-10000,9230) @0x011CE, **Member_set(12, 0x41)** @0x011D6 (Member 12 = +0x9, actor_common.c).
Haken: in run_all laufen Zustand, Selbst-Schub, Schub und Klemme des Affen nur noch bei `!(grid_id & 0x20)`.
Gemessen an der exe (ROOM4040 per Debug-Sprung, beide Male dieselbe Umgebung):
| Lauf | Stand | Leon | Affe (Platz 1, grid 0x61) |
|---|---|---|---|
| g1 | ohne Tor | Sprung-Spawn (-26400,17150) | F6 INIT (st 1, Sub 1) an der Spawn-Stelle (-14040,19130), F214 wach, F235.. laeuft los -> (-16877,17861) bei F600 — VOR seinem Auftritt |
| g2 | mit Tor | dito | bis F600 st 0, grid 0x61, unbewegt bei (-14040,19130) |
| g4 | ohne Tor | im Ausloeser (-25000,4000) | F16 Freigabe (grid 0x41, Lage (-25090,9230)), F214 wach, Bisse F314 / F370 je -5 |
| g3 | mit Tor | dito | F16 Freigabe + INIT, F224 wach, Bisse F324 / F380 je -5 |
Nach der Freigabe sind beide Staende gleich (nur 10 Bilder spaeter, weil INIT erst mit der Freigabe laeuft); das Tor
aendert also nur die Zeit VOR dem Auftritt. Stage 3 (grid 0x00, kein Member_set(12) in ROOM3040/3041) ist unberuehrt.

## Spieler-Tor der neuen Angriffswege (AUFTRAG-Nachtrag Integration 2026-10-17 07:36, Spur B)
Jeder Angriffsweg des kleinen Affen liest im Original das RE1.5-Tor Spieler+0x93 (`lbu` von 0x800acae7 = 0x800aca54 +
0x93, ganzes Byte, `bne v0,zero` = kein Angriff) — selbst disassembliert, STAGE3.BIN. Der Port liest dasselbe Byte
(`pl->hit_react`) an derselben Stelle; Spur B setzt dort in Szenen Bit 0, also sieht jeder Weg den Szenenschutz.
| Angriffsweg | Tor-Lesen im Original | Port (affen3_r38.c) |
|---|---|---|
| Biss-Entscheid A[3] -> Sub 5 | `lbu v0,0(a2)` @0x801117e8 (a2 = 0x800acae7 @0x801117e0), weitere @0x8011182c / @0x80111898 | a_jagd |
| Biss-Treffer B[5] | `lbu v0,0(a3)` @0x80111eac | Biss-Zweig: `if (pl->hit_react != 0) return` |
| Schwer-Entscheid A[4] -> Sub 6 | `lbu v0,0(a2)` @0x80111ad4, `bne` @0x80111adc | a_auswahl |
| Schwer-Treffer B[6] | `lbu v0,0(s2)` @0x80112190, `bne` @0x80112198 | Schwer-Zweig |
| Todessprung-Entscheid B[7] Bild 0x13 | `lbu` @0x801126ac, `bne` @0x801126b4 | b_sprung |
| Todessprung-Treffer B[8] (-500) | `lbu v0,0(s1)` @0x801129dc | b_todessprung |
| Griff-Entscheid A[13] -> Sub 15 | `lbu v0,0(a0)` @0x80113a24, `bne` @0x80113a2c | a_decke |
| Hieb-Entscheid A[13] -> Sub 14 | `lbu v0,0(a2)` @0x80113af8, `bne` @0x80113b00 | a_decke |
| Hieb-Treffer B[14] (-10) | `lbu` @0x8011406c, `bne` @0x80114074 | b_decke_hieb |
| Griff B[15] selbst | **KEIN Tor-Lesen**: P0 liest +0x93 nur zum Setzen (`lbu` @0x801143e0, `ori v1,v1,0x1` @0x801143f4, `sb` @0x8011441c); das Tor steht im Entscheid A[13] desselben Bildes (A vor B) | b_griff, wie das Original ohne eigenes Tor |
Kein Weg setzt einen Status (kein Gift o.ae.). Das RE2-Tor PL+0x1D3 & 0x80 liest der Affe nicht — er faehrt in beiden
KI-Varianten den RE1.5-Code, dessen Tor +0x93 ist.

## Tests (gezielt in diesem Baum, keine volle Suite)
Neue Riegel (`re15_port/tests/unit/probes/r38_affen3.cmake`, Quelle `tests/unit/test_r38_affen3.c` bzw.
`tests/integration/test_r38_affen3*.cmake`):
| Test | prueft |
|---|---|
| unit_r38_affen3_typ | Typ 0x29 = der Blob, den FUN_80022300 laedt (Tabelle @0x80073000 -> CDEMD0 Blob 13) |
| unit_r38_affen3_biss | Port ab dem Original-Zustand F137 von o4 (Leon steht): Entscheid, Treffer -5 im Fenster {12..15}, Takt 57 |
| unit_r38_affen3_flavor | dasselbe unter RE2-KI (Standard) und RE1.5-KI |
| unit_r38_affen3_quadrat | FUN_8001bff8 = achsenparalleles Quadrat um Record 9 + (0x64,0,0) |
| unit_r38_affen3_schwer | Sub 6: -10 im Fenster {21..24,33,34}, Records 6/10, r 0x320 |
| unit_r38_affen3_decke | o7: Sub 9 an die Decke, Kriechen/Jagd/Hieb bildgleich bis T281, beide KI |
| unit_r38_affen3_griff | o8: Griff Sub 15 + Spieler-Haken cmd 5 bildgleich, Bisse T458.. wie im Original, beide KI |
| unit_r38_affen3_sprung | o9: Sprung -> Schwer -10 -> Todessprung bei HP < 50 -> -500 -> cmd 6 -> Tod, beide KI |
| integration_r38_affen3 | exe, Weg wie o10: Bisse -5 im Takt 56, Affe an der Wand nach Westen geschoben, Flinch-Clip, beide KI |
| integration_r38_affen3_decke | exe: Schuss -> Sub 9/12/13/15 -> Griff ohne HP (Leon-Clips 1/2/3/16/11) -> >= 3 Bisse -5, beide KI |
| integration_r38_affen3_tor | exe, ROOM4040: gesperrter Ereignis-Affe (g=61) bleibt in Zustand 0 bis Member_set(12, 0x41), beide KI |
Alt-Riegel begruendet nachgezogen: `unit_cockroach_ai` (1) (INIT-Clip 0 @0x80110ce4, Ruhe-Clip 0x16 erst B[0]
@0x80111348-384). Mitgelaufen als Regression: Gorilla-Familie (`unit_r35_affen_*`, `unit_maggot_*`), geteilte
Opfer-/Griff-/Schub-Wege (`unit_re2_victim_collapse`, `unit_dog_grab_anchor`, `unit_re2_dog_grab_anchor`, `unit_re2z_*`,
`unit_enemy_body_push`, `unit_dog_aim_band`, `unit_r36_*`), exe-Laeufe mit Griff/Schuss/Opfer (`integration_r36_*`,
`integration_r37_*`).
Laeufe (Stand mit Wurzel-Tor, gebaut mit `local_build.sh build`; `ctest --test-dir re15_port/build -R "<Ausdruck>"
--timeout 600 --output-on-failure`):
- 53 Tests, `-R "^(unit_(cockroach_ai|maggot_|enemy_body_push|re2_victim_collapse|dog_grab_anchor|re2_dog_grab_anchor|re2z_grab_core_se|re2z_finisher_bite|r35_affen_|r36_kleidung_griff_|r36_zombieki_affen|r36_szenenleon_schub|r36_dachkiste_zellschub|dog_aim_band|r38_affen3_).*|integration_(r38_affen3|r38_affen3_decke|r37_szene11c0|r36_kleidung_hundeweg|r37_elzaems_elza1181griff|r36_schuss|r36_elza_schuss|r36_ziel1190_scheiben))$"`
  -> `100% tests passed, 0 tests failed out of 53` (Total Test time (real) = 776.91 sec)
- danach der neue Tor-Riegel, `-R "^integration_r38_affen3_tor$"` -> `100% tests passed, 0 tests failed out of 1`
  (Total Test time (real) = 87.89 sec). Gegenprobe mit der exe OHNE Tor (Stand vor dem Haken): ROT, "gesperrter Affe lief
  (395 aktive Bilder bei g=61)".
- integration_r38_affen3 vor der Umstellung auf o10 (alter o4-Riegel, Stand mit Selbst-Schub): ROT "Gleichtakt wie
  Original o4 fehlt" — die Erwartung war falsch, nicht der Code (s. o10).

## OFFEN
1. **Zufallsquelle:** FUN_8001af20 haengt im Original am a0 des Aufrufers; der Port nimmt re15_engine_rand8 (Ersatz,
   re15_damage.c). Wo der Affe wuerfelt (Sprung-Entscheid rng&1 @0x80111b88-ac, Lenkraten +0x9e in B[3]/B[13], Sprungweite
   +0x8c in B[7]), laufen die Bahnen nach 1-2 Bildern (o9) bzw. nach einigen hundert Bildern (o7 ab T281) auseinander. Die
   Mechanik ist gleich; bildgleich wird es erst mit einer Engine-weiten Zufallsquelle nach dem Original (nicht diese Spur).
2. **Spieler-Code, in o10 gemessen (nicht der Affe):** Rueckstoss nach einem Biss ein Bild spaeter als im Original;
   Leons Gehtempo 75 statt 76..80 Einheiten/Bild. Betrifft jeden Gegner-Treffer bzw. jedes Gehen, nicht nur die Affen.
3. **Anzeige-Bits aca3c:** der Opfer-Haken cmd 5 setzt `aca3c |= 0xc0` (@0x80115df8-e00) und loescht sie beim Freiwerden
   (@0x80116020-44); der Port hat dafuer kein Gegenstueck (wie beim Gorilla-Finisher). Sichtbare Folge nicht gemessen.
4. **o3 (Original-Fehler):** steht Leon schon beim allerersten aktiven Bild des Affen im Biss-Bereich, versetzt das Original
   den Affen in einem Bild nach (245,747) (Fuss-Sperre auf nie gezeichnete Records). Der Port bildet das nicht nach.
   Nachbesserung 1 dazu gemessen/gelesen: der INIT laeuft jetzt wie im Original im Spawn-Aufruf (@0x8004259c); das Original
   hat die EMD dort schon geladen (FUN_80022300 aus Sce_em_set @0x80042328, VOR dem `jalr`), die exe laedt die Bank erst
   nach scd_vm_tick (main.c RL-4-Vorladen) — der Spawn-INIT findet im Port also noch keinen Pool-Stand. Wirkung nur in
   genau diesem o3-Randfall (erstes aktives Bild mit Fuss-Sperre; B[0]/B[1] rufen keine Fuss-Sperre); Ereignis-Affen:
   keine (Fallbahn und Treffer-Clipbilder = o11).
5. ~~Ereignis-Affen Stage 4/5 nicht gegen das Original gemessen~~ — **erledigt in Nachbesserung 1** (Spawn-Wurzelaufruf,
   INIT-Zweig 0x40/0x41, Zustand 4/5; gegen Original o11 gemessen, s. Abschnitt "Nachbesserung 1").

## Fuer den Nutzer
**Was war falsch:** Die kleinen Affen in Stage 3 (ROOM3040 und ROOM3041, je zwei) bissen im Port ins Leere — ihr Biss
pruefte einen Abstand, den die Koerper-Kollision nie zuliess, und der Affe rutschte dabei durch die Wand. Angeschossene
Affen blieben in ihrer Nische sitzen.

**Jetzt wie im Original** (gegen das laufende Original in DuckStation gemessen, Bild fuer Bild):
- Biss: -5 HP, Leon zuckt zurueck. Geht man auf den Affen zu, beisst er alle 56 Bilder (knapp 2 s) und wird dabei von
  Leon an der Wand entlang geschoben.
- Stuermt der Affe aus groesserer Entfernung heran, greift er schwer an (-10 HP, Leon geht zu Boden) oder springt.
- Ein angeschossener Affe springt an die Decke, kriecht ueber den Gang, schlaegt von oben zu (-10 HP) und springt Leon von
  hinten an: der Griff selbst kostet keine HP, mit schnellem Tastendruecken kommt man frei; danach beisst er vom Boden.
- Hat Leon weniger als 50 HP, kann ein Sprung zum Todessprung werden (Leon stirbt) — wie im Original.
- An der Decke trifft man den Affen nur mit Ziel nach oben. Normale Treffer lassen ihn dort nur bluten; die Magnum oder
  die Schrotflinte aus der Naehe holen ihn herunter.
- Beide KI-Einstellungen spielen hier denselben Original-Code (RE2 hat an dieser Stelle die Kakerlake, kein Vorbild
  fuer den Affen).
- Dieselben Affen kommen auch in Stage 4 und 5 vor. Die, die dort erst bei einem Ereignis auftauchen (z.B. ROOM4040:
  das Skript setzt den Affen erst um und gibt ihn frei, wenn Leon einen bestimmten Bereich des Gangs betritt), warten
  jetzt wie im Original gesperrt auf ihren Auftritt — vorher liefen sie sofort los.
- (Nachbesserung 1) Bei ihrem Auftritt fallen diese Affen jetzt wie im Original aus der Deckenklappe (18 Bilder, gut eine
  halbe Sekunde), landen und greifen erst vom Boden aus an (zuerst meist der schwere Angriff, -10 HP). Vorher liefen sie
  10000 Einheiten ueber dem Boden umher und bissen Leon von dort aus. Skripte mit diesem Deckenfall (Pos_set y = -10000
  bzw. -10800 + Freigabe): ROOM4040/4041, 4080/4081, 50A0/50A1, 5100/5101, 50B0; in der Zwischensequenz von ROOM5031 wird
  der Affe am Boden freigegeben und 20 Bilder spaeter wieder gesperrt.

**Zum Ausprobieren:** Stage 3, ROOM3040, auf die Plattform mit den zwei Affen gehen (der erste sitzt in der Nische an der
Suedwand) und auf ihn zulaufen. Fuer die Decke: einen Affen anschiessen (z. B. Browning) und Leon umdrehen.

Keine neuen Assets, keine neuen Sprachdateien.

## Nachbesserung 1 (2026-10-17, nach Abnahme 0)
Abnahmebericht `S_abnahme_0.md`: Nutzer-Punkt erfuellt, aber **Mangel 1**: Ereignis-Affen in Stage 4/5 (Typ 0x29, gesperrt
mit grid 0x61, Freigabe per Pos_set y = -10000 + Member_set(12, 0x41)) fallen im Port nie von der Decke, sondern laufen
auf y = -10000 umher und beissen Leon aus 10000 Einheiten Hoehe. Ursache laut Abnahme: der INIT-Zweig @0x80110e80-eb8
(+0x9 == 0x40/0x41 -> Zustandswort 0x4/0x104) und Zustand 4 (0x80115734, Unter-Tabelle @0x8011ee34) fehlen im Port.

### Stand beim Start
- Baum geprueft: `git status --porcelain` leer, HEAD 8bb74063 (Abnahme 0), Basis cb782bda.
- Abnahmebericht, Dossier, AUFTRAG.md, VERTRAG.md gelesen.

### Mangel 1 — Messung vorher (aus der Abnahme, nicht wiederholt)
Abnahme M5 (Lauf s4_4040, Spur-exe 4ff3b973): `RE15_DEBUG_JUMP=4040@250 RE15_PLAYER_POS=-25000,4000,1024,0
RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=state.log RE15_EXIT_AT=900#4040` -> Affe Platz 1 ab F16 bei (-25090,-10000,9230),
INIT -> Zustand 1 Sub 1 (Schlaf), F224 wach, F311 Biss auf y = -10000, F324 / F380 je -5 HP (Leon y = 0), bis F900 auf
y = -10000. Basis-exe cb782bda: Affe ebenfalls auf y = -10000, schlaeft, 0 Treffer.

### Mangel 1 — RE-Belege (selbst disassembliert, re15_disasm.py)
ROOM4040 (scd_dump_room.py): `Ifel_ck` / `Ck(4,0xd0) == 0` -> @0x00EB0 `Sce_em_set 44 00 29 61 00 00 ..` (Platz 0, Typ 0x29,
grid 0x61 = gesperrt (0x20) + 0x40 + 0x01, Band pc[4] = 0 -> +0x1ba = 0, Parkstelle (-14040,0,19130)); Aot_set Platz 5 sce 3
flags 0x41 @0x00E9C, Rechteck x -27899..-22159, z 2608..5608; sub04: `Set(4,0xd0)`, Aot_reset 5, Se_on, Evt_exec 5/6
(Klappen obj 1/2 auf y -6300 oeffnen), `Work_set(2,0)` @0x011CA, `Pos_set(-25090,-10000,9230)` @0x011CE,
`Member_set(12,0x41)` @0x011D6 (= +0x9 := 0x41, Port actor_common.c case 12).

INIT 0x80110c98 (STAGE3.BIN; STAGE4.BIN 0x8010c338, Zweig @0x8010c520-558 befehlsgleich):
- `ori v0,zero,0x1` / `sw v0,4(v1)` @0x80110ca4-ac (Zustandswort 1), +0x9 & 1 -> `ori v0,zero,0x101` / `sw` @0x80110cd0-d4.
- +0x94/+0x95/+0x8f/+0x93 = 0 @0x80110ce4/cf4/d04/d14; +0x1dc = 0 @0x80110da4; +0x1e0 = 0 @0x80110dc4; +0x1e4 = (grid&8)?4:0.
- **@0x80110e80 `lbu v1,9(a0)` / `ori v0,zero,0x40` / `bne v1,v0,0x80110eac` / (Delay `ori v0,zero,0x41`) / `ori v0,zero,0x4` /
  `sw v0,4(a0)` @0x80110e94; @0x80110ea4 `lbu v1,9(a0)` / `ori v0,zero,0x41` / `bne v1,v0,0x80110ebc` / `ori v0,zero,0x104` /
  `sw v0,4(a0)` @0x80110eb8** -> +0x9 == 0x40: Zustandswort 4 (+0x4 4, +0x5 0); +0x9 == 0x41: 0x104 (+0x4 4, +0x5 1).
- danach anim_set @0x80110ed0 (Clip 0 Bild 0), HP-Zeile, +0x166 = 0x14cc, Kasten +0x78 = @0x8011ec44[+0x1e4] — fuer alle Faelle.
Zustandstabelle @0x8011eca4: [4] = [5] = 0x80115734 (STAGE4 @0x80119ed4: [4] = [5] = 0x80110dd4).
Zustand 4 0x80115734: `lbu v0,5(v0)` @0x80115744, Tabelle @0x8011ee34 (`addiu at,at,-4556`), `jalr` @0x80115764;
@0x8011ee34 = {0x8011577c, 0x801158e8, 0x80115a5c (`jr ra`), 0x80115a64 (`jr ra`)} (STAGE4 @0x8011a064 = {0x80110e1c,
0x80110f88, ..}).
- **Unterzustand 0 (+0x9 == 0x40) 0x8011577c**, Phase +0x6: 0 (@0x801157c8): +0x6 = 1, +0x94 = 0x1f (`ori v0,zero,0x1f` /
  `sb v0,148` @0x801157d4-d8), +0x95 = 0, +0x8f = 7, +0x8c = 0 (`sh zero,140` @0x80115808), +0x9c = 0 (@0x80115818), faellt in
  Phase 1 (@0x8011581c): anim_set `jal 0x8001f314` @0x80115830, +0x6 += Rueckgabe (`addu v1,v1,v0` / `sb v1,6` @0x8011584c-54);
  Phase 2 (@0x80115858): +0x9 &= 0xbf (`andi v0,v0,0xbf` @0x80115860), +0x4 = 1 @0x80115874, +0x5 = 2 @0x80115884, +0x6 = 1
  @0x80115894, +0x7 = 0 @0x801158a4, +0x95 = 0x1d @0x801158b4, +0x8f = 0 @0x801158c4, +0x94 = 3 @0x801158d4 (= B[2] Phase 1,
  Clip 3 ab Bild 0x1d).
- **Unterzustand 1 (+0x9 == 0x41) 0x801158e8**, EIN Bild: +0x93 |= 1 (@0x80115900-04), |= 2 (@0x8011591c-20), +0x1e0 = 1
  (`sb a0,480` @0x80115930), +0x78 = @0x8011ec48[+0x1e4] (Luftkasten, @0x80115940-60), +0x9 &= 0xbf (@0x80115978-7c), +0x4 = 1
  @0x8011598c, +0x5 = 0xa @0x8011599c, +0x6 = 2 @0x801159ac, +0x7 = 0 @0x801159bc, +0x94 = 0xd @0x801159cc, +0x95 = 0x13
  @0x801159dc, +0x8f = 0 @0x801159ec, +0x8c = 0 @0x801159fc, +0x9c = 0 @0x80115a0c, anim_set @0x80115a24, +0x95 = 0x14
  @0x80115a38, +0x8f = 0 @0x80115a48 -> naechstes Bild ACTIVE B[10] Phase 2 = der Fall (c1a4(+0x8c,0,-70) bis +0x1ba, im Port
  b_decke_runter) — Landung Phase 3 (SE 1, Bodenkasten, +0x93 = 0), Phase 4 Clip 0xd zu Ende -> Sub 4.
- A[10] 0x80113010 im ersten Fallbild: +0x6 < 3 (`sltiu v0,v0,0x3` @0x80113028) und +0x93 & 2 (@0x8011303c) -> ohne 0x40:
  +0x93 &= 0xfd (@0x80113094-98) und FUN_80019700(0x2000, +0x6a, Record 0 + 0x40, &0x8011ec84 = {200,-800,0,0})
  @0x801130e0 (Effekt-Instanz: Pool 0x800a73b8, Bank a0>>24 = 0, Effekt (a0>>16)&0xff = 0, Flags 0x2000 @0x800197d0-dc) —
  im Port luft_treffer -> blut() (vorhanden).
- Wurzel-Schattenblock @0x80110bd0-c64: +0x9 & 0x40 -> KEINE Schattengroessen-Aktualisierung (+0x1d8 -> +0xb4/+0xb6); danach
  FUN_8001b064(&+0xb0, +0x1ba) @0x80110c7c. Nur der Schatten; der Port zeichnet den Gegnerschatten anders (nicht diese Spur).

### Mangel 1 — Original gemessen: o11 (DuckStation-GDB, ROOM4040, Freigabe im Ausloeser)
Werkzeug `s_nb1/gdb/affen_spur4.py` (Scratch) = affen_spur3.py mit einstellbarer Wurzel (`--root 8010c1a0` = STAGE4.BIN-Wurzel,
Registrierung @0x801183e8); DuckStation per Start-Process (`-batch -statefile stage_saves/mzd_debugmenu.sav <MZD-cue>`),
`--attach <PID>`; Sprung Stage-Index 3 / Raumindex 4 (ROOM4040); Spieler nach 40 aufgezeichneten Bildern auf (-25000,0,4000)
Band 0 (im Rechteck von AOT 5). settings.ini `[Debug] EnableGDBServer` nur waehrend der Messung true, danach die Sicherung
byte-gleich zurueckgespielt (kein BOM, `EnableGDBServer = false`). **Code-Identitaet: RAM 0x8010c1a0..+0x5730 (22320 B) ==
STAGE4.BIN, 0 Abweichungen.** 450 Bilder (Zeile = Halt an der Wurzel mit cur = Entity 0 = Platz 0 des Raums).
| Zeile (Bild) | Affe +0x4.5.6.7 | +0x9 | Lage (x,y,z) | weitere |
|---|---|---|---|---|
| F0  | 0.0.0.0 | **41** | (-14040,0,19130) | +0x78 = 0x80072be0 (Sce_em_set-Vorgabe @0x800422cc-d0) — DER SPAWN-WURZELAUFRUF (s. unten) |
| F18..F56 | **4.1.0.0** | 61 | (-14040,0,19130) | gesperrt; der INIT lief schon beim Spawn |
| F57 | 4.1.0.0 | 41 | (-25090,**-10000**,9230) | Freigabe im selben Bild (Pos_set @0x011CE + Member_set @0x011D6); dieser Wurzel-Halt faehrt Zustand 4/1 |
| F58 | 1.10.2.0 | 01 | (-25090,-10000,9230) | +0x1e0 1, +0x93 03, Clip 13 Bild 20, +0x78 = 0x80119e50 (Luftkasten) |
| F59 | 1.10.2.0 | 01 | y -10000 | +0x93 01 (A[10] loescht Bit 1, Effekt), Bild 21, +0x9c 1 |
| F60..F75 | 1.10.2.0 | 01 | y -9930, -9790, -9580, -9300, -8950, -8530, -8040, -7480, -6850, -6150, -5380, -4540, -3630, -2650, -1600, -480 | Bild 22, 23, dann 23 gehalten; +0x9c 2..17 |
| F76 | 1.10.3.0 | 01 | y **0** (= +0x1ba) | gelandet |
| F77 | 1.10.4.0 | 01 | y 0 | +0x1e0 0, +0x93 0, Bild 25, +0x78 = 0x80119e44 (Bodenkasten) |
| F92 | 1.4.0.0 | 01 | (-25090,0,9230) | Clip 13 zu Ende -> Sub 4 (Anlauf), F93 Clip 6 +0x8c 185 |
| F106 | 1.6.1.0 | 01 | (-23859,0,7548) | Sub 6 schwerer Angriff (Clip 19) |
| F128 | | | | **erster Treffer: Leon 100 -> 90** (schwer, cmd 02 04), danach Bisse -5 bei F225, 283, 340, 397, 454 |
Der Fall: y(n) = -10000 + 70*n(n-1)/2 (c1a4(0,0,-70) mit +0x9c = n), 18 Fallbilder, Landung im 18., Clip 0xd danach 15 Bilder.
**Der Affe greift erst nach der Landung an, vom Boden aus.**

### Mangel 1 — der Spawn-Wurzelaufruf (warum der gesperrte Affe schon in Zustand 4 steht)
Sce_em_set FUN_800420a0 (PSX.EXE, selbst disassembliert): `sb v0,9(s0)` @0x80042164 (+0x9 = pc[3] = 0x61);
@0x80042564 `lbu v1,9(s0)` / @0x8004256c `andi v0,v1,0xdf` / @0x80042570 `sb v0,9(s0)` (Sperrbit 0x20 geloescht) /
@0x80042578 `sw s0,-14460(at)` (cur = Affe) / @0x8004257c-94 `lbu v0,8(s0)` .. `lw v0,0(0x80072bac + 4*Typ)` /
**@0x8004259c `jalr v0`** (die Typ-Wurzel EINMAL, INIT sieht +0x9 = 0x41 -> Zustandswort 0x104) / Delay @0x800425a0
`andi s4,v1,0x20` / @0x80042604 `or v0,v0,s4` / @0x80042608 `sb v0,9(s0)` (Sperrbit zurueck, +0x9 = 0x61).
Der Port hat diesen Aufruf (`re15_enemy_spawn_root`, enemy_ai_common.c, aus op_sce_em_set), aber nur fuer 0x30/0x36/0x27
verdrahtet ("fuer die uebrigen Typen OFFEN"). Fuer 0x29 lief der INIT deshalb erst im ersten ungesperrten run_all-Bild —
also NACH Member_set(12,0x41) — und kannte den 0x40/0x41-Zweig nicht (Zustand 1 / Sub 1 = Schlaf auf y = -10000).
Folge fuer die Umsetzung: (1) 0x29 in re15_enemy_spawn_root, (2) INIT-Zweig 0x40/0x41, (3) Zustand 4/5 mit beiden
Unterzustaenden. Der alte Riegel integration_r38_affen3_tor erwartete "gesperrt in Zustand 0, INIT erst nach der
Freigabe" — das widerspricht o11 (gesperrt in Zustand 4.1) und wird auf das Original umgestellt.

### 2026-10-17 ~09:55 — Fortsetzung der Nachbesserung 1 (Vorgaenger beim Sitzungslimit ~09:25 beendet)
- Stand: 6bd30a91 (Code: INIT-Zweig, Zustand 4/5, Spawn-Wurzelaufruf 0x29), 3bfe08ef (Riegel `ereignis`/`ereignis40` +
  `integration_r38_affen3_tor` umgestellt, UNGEPRUEFT). `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`.
- Erster Lauf der neuen Unit-Riegel: `ereignis40` gruen; `ereignis` ROT — (a) RE1.5-Durchgang im selben Prozess spawnt
  den Nach-Ereignis-Affen (g=00 bei (-23590,-4320)), weil room_boot die Spielflags behaelt und der RE2-Durchgang
  `Set(4,0xd0)` (sub04) schon gesetzt hat -> Riegel-Fehler (Flag vor dem Lauf loeschen); (b) RE2-Durchgang: Fallbahn
  Bild fuer Bild = o11, Landung R+18, Sub 4 R+34 (gruen), aber in 400 Bildern kein HP-Abzug -> wird untersucht.
- Riegel-Fehler behoben (Test-Code, nicht Engine): (a) `ereignis` loescht vor jedem Durchgang `Set(4,0xd0)` (sonst
  Nach-Ereignis-Affe); (b) beide neuen Unit-Teile laden die EM029-Bank NACH room_boot — room_boot ruft
  re15_enemy_reset, das ALLE Banken verwirft; mit der Bank davor fehlten Pool/Fuss-Sperre/Trefferpunkt (Affe stand im
  schweren Angriff still, kein Treffer). Nach der Reihenfolge der exe: main.c laedt die Raum-Banken nach scd_vm_tick
  (RL-4-Vorladen), also nach dem Spawn — der Spawn-INIT laeuft dort ebenso ohne Bank. (c) integration_r38_affen3_tor:
  der gy.log-Ausdruck hatte 10 Gruppen; CMake setzt nur CMAKE_MATCH_0..9 (gemessen, cmake 4.3.2: CMAKE_MATCH_10 leer,
  CMAKE_MATCH_COUNT = 9) -> z war leer, jeder gesperrte Bild "falsch". Jetzt 9 Gruppen (+0x7 nicht eingefangen).
- Original o11 (Scratch s_nb1/gdb/o11_4040.txt, selbst mit dec11.py dekodiert) — alle HP-Wechsel:
  F128 100->90 Affe st1.6.1 Clip 19 Bild 23 (schwer, -10) | F225 90->85 st1.5.1 Clip 18 Bild 15 | F283 -> 80 Bild 14 |
  F340 -> 75 Bild 13 | F397 -> 70 Bild 13 | F454 -> 65 Bild 13 (Bisse -5). Relativ zu R = F58: R+70, R+167, R+225, R+282,
  R+339, R+396; Biss-Takt 58/57/57/57.
- Port (Unit `ereignis`, beide KI): R+71 (-10, Sub 6, Clip 19 Bild 23), R+166 / 224 / 281 / 338 / 395 (je -5, Sub 5,
  Clip-Bilder 15/14/13/13/13) — Takt 58/57/57/57 identisch, Clip-Bilder der Treffer identisch, Affen-Lage im Biss
  (-25326,2920) .. (-25424,2962) gegen o11 (-25323,2926) .. (-25417,2968). Der erste Treffer liegt 1 Bild spaeter, die
  Bisse 1 Bild frueher: der Anlauf B[4] wuerfelt +0x8c/+0x9e (Zufallsquelle, OFFEN 1).

### Mangel 1 — Ursache (zusammengefasst)
Drei fehlende Original-Stuecke, alle drei noetig fuer den Fall von der Decke:
1. **Spawn-Wurzelaufruf** (PSX.EXE Sce_em_set FUN_800420a0: `andi v0,v1,0xdf` @0x8004256c, `jalr v0` @0x8004259c,
   `or v0,v0,s4` / `sb v0,9(s0)` @0x80042604-08) war im Port nur fuer 0x30/0x36/0x27 verdrahtet. Fuer 0x29 lief der INIT
   erst im ersten UNGESPERRTEN run_all-Bild, also nach Member_set(12,0x41) — mit +0x9 = 0x41 statt beim Spawn.
2. **INIT-Zweig** @0x80110e80-eb8 (+0x9 == 0x40 -> Zustandswort 4, == 0x41 -> 0x104) fehlte -> der Port setzte Zustand 1
   Sub 1 (Schlaf, +0x9 & 1) an der Pos_set-Lage y = -10000.
3. **Zustand 4/5** 0x80115734 (Tabelle @0x8011eca4 [4] = [5], Unter-Tabelle @0x8011ee34) fehlte ganz -> nichts leitete
   ACTIVE B[10] Phase 2 (den Fall c1a4(0,0,-70) bis +0x1ba) ein. B[10] selbst (b_decke_runter) war schon da.
Der Fehler war vor der Spur stumm (Affe schlief auf y = -10000, 0 Treffer) und wurde mit dem Biss-Quadrat (prueft nur X/Z,
FUN_8001bff8) sichtbar: Bisse aus 10000 Einheiten Hoehe (Abnahme M5).

### Mangel 1 — Aenderung (Commit 6bd30a91, Code; Riegel 3bfe08ef + aad46e72)
| Datei | Aenderung | Beleg |
|---|---|---|
| engine/src/enemy_ai_common.c `re15_enemy_spawn_root` | 0x29 in die Typ-Liste; ruft fuer 0x29 `re15_affen3_wurzel_rumpf` (statt re15_birkin_root) zwischen Sperrbit-Loeschen und -Zuruecksetzen | `andi` @0x8004256c, `jalr` @0x8004259c, `or`/`sb` @0x80042604-08; Original o11 F0 (st0 g41 = Halt im Spawn-Aufruf) -> F18 4.1.0.0 g61; o1 Klemme beim Spawn (z -6150 -> -6082) |
| enemy_ai_common.c `re15_affen3_wurzel_rumpf` (neu, static) | Rumpf der Wurzel 0x80110b00 hinter den Toren: Zustand, aec4, b498/b544, Wandklemme — aus run_all (jedes Bild, hinter dem Tor +0x9 & 0x20 @0x80110b34-40) und aus dem Spawn-Aufruf; run_all-Zweig 0x29 ruft ihn (vorher dieselben vier Aufrufe inline) | `jalr` @0x80110b68, `jal 0x8002b498` @0x80110b78, `jal 0x8002aec4` @0x80110b88, `jal 0x8002b544` @0x80110b98, `jal 0x8003b0a4` @0x80110bb8 |
| enemy_ai_common.c `re15_cockroach_ai_tick` | `case 4: case 5:` -> `re15_affen3_zustand4(e)` | Tabelle @0x8011eca4 [4] = [5] = 0x80115734 |
| engine/src/affen3_r38.c `re15_affen3_init` | nach dem Grid-Block: `grid == 0x40` -> state 4 / Sub 0, `grid == 0x41` -> state 4 / Sub 1 (+0x6 = +0x7 = 0) | @0x80110e80-e94 (`sw v0,4(a0)` Wort 0x4), @0x80110ea4-eb8 (Wort 0x104) |
| affen3_r38.c `re15_affen3_zustand4` (neu) | Unterzustand 0 (0x8011577c): Phase 0 Clip 0x1f / +0x95 0 / +0x8f 7 / +0x8c 0 / +0x9c 0, faellt in Phase 1 (anim_set, +0x6 += Ende), Phase 2: +0x9 &= 0xbf, ACTIVE Sub 2 Phase 1, Clip 3 ab Bild 0x1d. Unterzustand 1 (0x801158e8), EIN Bild: +0x93 \|= 1 \|= 2, +0x1e0 = 1, Luftkasten, +0x9 &= 0xbf, ACTIVE Sub 10 Phase 2, Clip 0xd Bild 0x13 -> anim_set -> Bild 0x14, +0x8f 0, +0x8c 0, +0x9c 0. Unterzustand 2/3 `jr ra` | 0x8011577c..0x801158e0 und 0x801158e8..0x80115a54 selbst disassembliert (alle Stores, s. RE-Belege); Luftkasten-Zeiger `*(0x8011ec48 + 4*+0x1e4)` = @0x8011ec44[1] = 0x8011ec20 {0,-1530,0,800,1530,800} (r 800) |
| include/re15_affen3_r38.h | Deklaration re15_affen3_zustand4, Kopf-Kommentar Spawn-Aufruf | — |
Neue Konstanten: keine Zahl ohne Adresse — 0x40/0x41/0x4/0x104 @0x80110e84/ea8/e90/eb4, Clip 0x1f @0x801157d4, 7 @0x801157f4,
0x1d @0x801158b0, 3 @0x801158d0, 0xa @0x80115998, 2 @0x801159a8, 0xd @0x801159c8, 0x13 @0x801159d8, 0x14 @0x80115a34,
0xbf @0x80115978 / @0x80115860.
Pausen-Tor @0x80110b14-20 im Spawn-Aufruf: nicht nachgebildet (wie beim Gorilla 0x27). Geprueft: keines der Skripte mit
`Sce_em_set` Typ 0x29 (3040/3041, 4000, 4040/4041, 4080/4081, 5000/5001, 5031, 50A0/50A1, 50B0, 5100/5101) setzt
`Set(2,2,*)` (= RE15_PAUSE_AI 0x20000000); o11 F0 zeigt den INIT im Spawn-Aufruf.

### Mangel 1 — Zensus erweitert (alle RDTs, scd_dump_room.py)
Zusaetzlich zum Zensus oben: ROOM50B0 (Platz 1/2 0x61, Member_set(12, 0x41 @0x008E6 / 0x61 @0x00936 / 0x00 @0x00968 /
0x41 @0x0097C)), ROOM5100/5101 (Platz 0/1 0x61; sub01 Pos_set y = -10000 + Member_set(12,0x41) @0x008F8/@0x0092E; sub00
beim Wiederbetreten Pos_set y = 0 + Member_set(12,0x00) @0x0088C/@0x008CE), ROOM5031 (Platz 1 0x61; Zwischensequenz sub03:
Member_set(12,0x41) @0x0119E, 20 Bilder spaeter Member_set(12,0x61) @0x011AC = wieder gesperrt). Weder `Sce_em_set` 0x29 mit
+0x9 = 0x40/0x60 noch `Member_set(12,0x40)` -> Unterzustand 0 (0x8011577c) wird im Auslieferungsstand nicht erreicht; jede
Freigabe eines 0x61-Affen (auch mit 0x00) faehrt Unterzustand 1, weil der Spawn-INIT das Wort 0x104 schon gesetzt hat.

### Mangel 1 — Messung nachher (exe dieses Baums, Stand aad46e72 = Code 6bd30a91; Messkopie re15_pc_s_nb1c.exe)
Gemeinsame Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log RE15_STATE_LOG=state.log`; Auswertung mit eigenem Skript (Scratch
s_nb1c/auswert.py, gy.log + state.log je Bild). R = erstes Bild nach der Freigabe.
| Lauf | Aufbau | Freigabe | Fallbahn y R..R+18 | Landung / Sub 4 | HP-Abzuege (Affe dabei) |
|---|---|---|---|---|---|
| m1_4040_std | **Aufbau der Abnahme M5**: `RE15_DEBUG_JUMP=4040@250 RE15_PLAYER_POS=-25000,4000,1024,0 RE15_EXIT_AT=900#4040`, RE2-KI | F16: 1/10/2 g01 Clip 13 Bild 20 @(-25090,-10000,9230); F1..F15 gesperrt 4/1/0 g61 | -10000 -10000 -9930 -9790 -9580 -9300 -8950 -8530 -8040 -7480 -6850 -6150 -5380 -4540 -3630 -2650 -1600 -480 0 (= o11 F58..F76) | R+18 / R+34 | F87 (R+71) 100->90 Sub 6 Clip 19 Bild 23 y 0; dann 13 x -5 Sub 5 Clip 18 (F193 .. F885), alle y 0; Affe ab R+19 nur y = 0 |
| m2_4040_re15 | wie m1, `RE15_AI_FLAVOR=re15` | wie m1 | wie m1 | R+18 / R+34 | bildgleich zu m1 |
| m3_50A0_std | Stage 5: `RE15_DEBUG_JUMP=50A0@250 RE15_PLAYER_POS=-12350,150,1024,0` (im Ausloeser AOT 0 sce 3 @0x0165C), RE2-KI | F16: 1/10/2 g01 Clip 13 Bild 20 @(-12490,-10000,3080) (Pos_set @0x01982) | wie o11 | R+18 / R+34 | F83 (R+67) 100->90 Sub 6 y 0; dann -5 Sub 5 im Takt 56 (F187, 243, 299, ...), alle y 0 |
| m4_50A0_re15 | wie m3, RE1.5-KI | wie m3 | wie m3 | R+18 / R+34 | bildgleich zu m3 |
| m5_5100_wieder | Stage 5, Wiederbetreten: `RE15_DEBUG_JUMP=5100@250 RE15_SET_FLAG=3:0x2d` (sub00: Pos_set y = 0 + Member_set(12,0) @0x0088C) | F6: 1/10/2 g00 @(-10588,0,-14198) | y 0 (Start am Boden) | Phase 3 F8 (R+2), Phase 4 Bild 25 F9, Sub 4 F24 (R+18) | Leon ausser Reichweite (kein Abzug); zweiter Affe (g61) bleibt gesperrt 4/1/0 |
Gegen die Abnahme (vorher, M5): Affe bis F900 auf y = -10000, Bisse F324/F380 aus der Luft. Nachher: Fall in 18 Bildern
wie das Original, jeder HP-Abzug vom Boden.
Bildkontrolle (Hardware-Renderer, OHNE RE15_SOFTWARE_RENDER, `RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=10-100/3:fd_`, Aufbau m1):
Kontaktbogen `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\s_nb1c\laeufe\b1_4040_bild\fall_zoom.png` — F25 die Deckenklappe
oeffnet sich (Evt_exec sub04), F28 Klappe im Bild, F31/F34/F37 der Affe faellt im Vordergrund herab, F40 Landung,
F43..F52 kauert er am Boden; Modell geschlossen.

### Mangel 1 — Tests (gezielt, keine volle Suite; Stand Code 6bd30a91 + Riegel aad46e72/5c54afd7, `local_build.sh build` OK)
Neu/umgestellt (`re15_port/tests/unit/probes/r38_affen3.cmake`):
| Test | prueft |
|---|---|
| unit_r38_affen3_ereignis | ROOM4040 ueber Skript/AOT 5: Spawn-Wurzelaufruf -> gesperrt in 4/1 (g61, Parkstelle, Massstab = INIT lief); Freigabe-Bild 1/10/2 g01 Clip 13 Bild 0x14, +0x93 3, +0x1e0 1, Luftkasten r 800; erstes Fallbild +0x93 1; Fallbahn Bild fuer Bild = o11 F58..F76; Landung R+18 (Phase 4 Bild 25, Bodenkasten), Sub 4 R+34; kein HP-Abzug aus der Luft; erster Treffer -10 aus Sub 6 in R+70 +-2 (o11 R+70); danach Bisse -5 aus Sub 5 im Takt 58/57/57/57 (o11 F225/283/340/397/454); RE2-KI und RE1.5-KI |
| unit_r38_affen3_ereignis40 | Unterzustand 0 (+0x9 0x40): Clip 0x1f ab Bild 0 -> Ende T70 (70 Bilder, EDD EM029) -> T71 ACTIVE Sub 2 Phase 1 Clip 3 Bild 0x1d, +0x9 0 -> T113 Sub 3 (Clip 3 ab 0x1d zu Ende, B[2] @0x80111744-7c4) |
| integration_r38_affen3_tor | exe ROOM4040, je KI: [ruhe] 400 Bilder gesperrt 4/1 an der Parkstelle; [frei] (Leon im Ausloeser, Gier 0 wie o11) Freigabe-Bild = o11 F58, Fallbahn = o11, Landung R+18 / Phase 4 Bild 25 R+19, Sub 4 R+34, jeder HP-Abzug nach R+18 mit Affe auf y = 0 |
Laeufe:
- `ctest --test-dir re15_port/build -j3 -R "^(unit_(cockroach_ai|maggot_|enemy_body_push|re2_victim_collapse|dog_grab_anchor|re2_dog_grab_anchor|re2z_|dog_aim_band|dog_devour|sca_wall_hit|knockdown_|re2_stagger_knockdown|r35_affen_|r35_cut1150_montage_11c0|r36_kleidung_|r36_zombieki_|r36_szenenleon_|r36_dachkiste_|r36_schuss|r36_elza_schuss|r36_ziel1190|r36_karte_tuer11c0|r37_szene11c0_|r37_bissarm_|r38_affen3_|npc_wall_clamp|r20_birkin_push_pin|1090_push_pin|r34n_f_leiche_latch|sherry_wall_pin|birkin_ai|g5_|room1140_spawn|marvin_spawn_bank|1030_spawncap|r37_zombieaktiv_nachspawn1130)|probe_p2_birkin_g5|prop_spawn_entered)" --timeout 600`
  -> `100% tests passed, 0 tests failed out of 107` (Total Test time (real) = 26.52 sec) — alle Unit-Riegel der Abnahme +
  die Aufrufer von re15_enemy_spawn_root (birkin_ai, g5_*, Spawn-Riegel).
- exe seriell: `ctest --test-dir re15_port/build -V -R "^integration_(r38_affen3|r38_affen3_decke|r38_affen3_tor|r37_szene11c0|r36_kleidung_hundeweg|r36_kleidung_spielweg|r37_elzaems_elza1181griff|r36_schuss|r36_elza_schuss|r36_ziel1190_scheiben|r36_dachkiste|r36_szenenleon|r37_bissarm_echtlauf)$" --timeout 900`
  -> `100% tests passed, 0 tests failed out of 13` (Total Test time (real) = 2513.69 sec). Wortlaut der Spur-Riegel:
  `r38_affen3 [std]: 15 Treffer in Bild 169;225;281;337;393;450;506;562;618;674;730;786;842;898;954; Abstaende
  56;56;56;56;57;56;56;56;56;56;56;56;56;56` (re15 identisch; = Abnahme 0, Stage 3 durch den Spawn-Aufruf unveraendert);
  `r38_affen3_decke [std]: getroffen=1, ... Griff F350, Sub 2 F561; ... HP-Abzuege im Griff 0; Bisse danach 8` (re15
  identisch); `r38_affen3_tor [std_frei]: Fallbahn -10000;-10000;-9930;...;-480;0; Sub 4 R+34; HP-Abzuege
  F87(R+71,100->90,y=0);F182(R+166,90->85,y=0);F240(R+224,85->80,y=0);F297(R+281,80->75,y=0);F354(R+338,75->70,y=0)` (re15
  identisch), `[std_ruhe]: gesperrt (g=61) 400 Bilder, davon nicht 4/1 an (-14040,19130): 0`.
- nach dem letzten (Kommentar-)Commit neu gebaut: `ctest --test-dir re15_port/build -R "^unit_(r38_affen3_|cockroach_ai$)"
  --timeout 600` -> `100% tests passed, 0 tests failed out of 11`.
Gegenprobe: derselbe Riegel `integration_r38_affen3_tor` mit dem Stand der Abnahme (vor 6bd30a91) — Mangel-Aufbau der Abnahme
M5 (Affe bleibt auf y = -10000, Bisse aus der Luft) erfuellt weder "gesperrt in 4/1" (dort Zustand 0) noch die Fallbahn;
gemessen hat das die Abnahme selbst (M5), nicht wiederholt.

### Nachbesserung 1 — Ergebnis
Mangel 1 behoben und gegen das Original gemessen: Ereignis-Affen (Typ 0x29, Stage 4/5) bekommen ihren INIT wie im Original im
Spawn-Aufruf (Zustandswort 0x104), warten gesperrt und fallen nach der Freigabe in 18 Bildern Bild fuer Bild wie o11 von der
Decke; Angriffe nur vom Boden, Treffer-Werte (-10 schwer, -5 Biss) und Biss-Takt wie o11. Der Nutzer-Punkt (Stage 3) ist
unveraendert erfuellt (integration_r38_affen3 bildgleich zur Abnahme). Neue Assets: keine. Sprachdateien: keine.
