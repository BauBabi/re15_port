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
