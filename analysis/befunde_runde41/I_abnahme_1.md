# Runde 41 — Spur I "zombiefresser": UNABHAENGIGE ABNAHME 1 (2026-10-24)

Baum `.claude/worktrees/r41_zombiefresser`, Zweig `r41/zombiefresser`, HEAD 244a9574 (letzte Code-Aenderung 3083b2da 13:29:25),
Merge-Basis master 7b59e297 (master selbst steht auf ee626242, nur Doku/pri seitdem). Grundlage: Abnahme 0 (`I_abnahme_0.md`,
M1 + V1), Orchestrator-Entscheide I-E1/I-E2 (AUFTRAG.md 12:43), Nachbesserung 1 (Dossier §8). Alles unten ist SELBST gemessen
bzw. SELBST disassembliert; Behauptungen des Bau-Agenten gelten nur, wo hier ein eigener Beleg steht.
(Bericht fortlaufend geschrieben; Stand: abgeschlossen.)

## 0. Nutzer-Punkt (Wortlaut, AUFTRAG.md) und Pruefumfang

> In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen,
> anstatt aufzustehen wenn man ihnen näher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010
> etc. es ist durchgehend falsch.

Dazu aus Abnahme 0 / Orchestrator: M1 (RE1.5-KI-Liegepose vor dem Wecken), I-E2 (Laden IN 2000/2001 zeigt keine Zombies),
V1 (Vertrag, durch I-E1 erledigt).

## 1. Bau

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> Preflight `cc1 --version
  EXIT=0 -> Toolchain OK`, `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`.
- exe `re15_port/build/platform/pc/re15_pc.exe` 13:30:09; juengste Quelle `include/re15_zombiefresser_r41.h` 13:29:00
  (Commit 3083b2da 13:29:25 = letzter Commit mit Code; danach 78191a33/f2d615d6/244a9574 nur `analysis/`), `git status` leer
  -> exe = Codestand HEAD.

## 2. RE-Gate (selbst disassembliert / selbst gelesen)

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (Baum-Kopie); Binaerdateien md5-gleich mit dem Hauptbaum
(STAGE2.BIN 73a0a8e2..., PSX.EXE b55fdaa5...). Neue Zitate der Nachbesserung 1:

| Zitat im Diff (NB1) | Datei | selbst gelesen | Befund |
|---|---|---|---|
| INIT-Pose `+0x94 = 0x0c` @0x80100b70-74 (sel 4/7/9), `0x0e` @0x80100bb4-b8 (sel 5/8/a) | STAGE2 | `80100b4c..60 ori 4/7/9 + beq/bne` / `80100b70 ori v0,zero,0xc` / `80100b74 sb v0,148(v1)`; `80100b90..a4 ori 5/8/0xa` / `80100bb4 ori v0,zero,0xe` / `80100bb8 sb v0,148(v1)` | stimmt |
| INIT `+0x95 = 0`, `+0x8f = 0`, `jal 0x8001f314` @0x80100c34 (a3 = 0x200) | STAGE2 | `80100c0c sb zero,149` / `80100c1c sb zero,143` / `80100c28 addu a2,zero,zero` / `80100c2c lw a0,368` / `80100c30 lw a1,372` / `80100c34 jal 0x8001f314` / `80100c38 ori a3,zero,0x200`; alles hinter dem Tor `80100b44 beq (+0x9&0x80)==0 -> 0x80100cc4` | stimmt |
| FINAL `+0x94 = 0x12` OHNE f314 @0x80100c7c | STAGE2 | `80100c78 ori v0,zero,0x12` / `80100c7c sb v0,148(v1)`, zwischen c34 und c7c kein weiterer `jal` | stimmt |
| STAGE1 f314 @0x80100da0, STAGE3 @0x80100e8c | STAGE1/3 | `80100d78 sb zero,149 / 80100d88 sb zero,143 / 80100da0 jal 0x8001f314 / 80100da4 ori a3,0x200`; `80100e64 / 80100e74 / 80100e8c jal 0x8001f314 / 80100e90 ori a3,0x200`; Tore `80100cb0` / `80100d9c` | stimmt |
| P1 ohne anim_set, `sb 2,6` @0x80103984, `j 0x80103a10` + `sb 0xf,143` @0x80103994-98; P2 eigener Fall, f314 @0x801039a8 | STAGE2 | Weiche `801038fc lbu v1,6` / `80103904 beq v1,1 -> 80103964` / `80103914 beq v1,0 -> 80103940` / `80103928 beq v1,2 -> 8010399c` / `80103930 beq v1,3 -> 801039d0`; P1 `80103964 lhu 156 / 8010396c addiu -1 / 80103970 bne v1,zero,0x80103a10 / 80103974 sh 156 / 80103984 sb 2,6 / 80103994 j 0x80103a10 / 80103998 sb 0xf,143` (kein jal); P2 `801039a8 jal 0x8001f314 / 801039ac ori a3,0x100 / 801039c4 addu v1,v1,v0 / 801039cc sb v1,6` | stimmt |
| Skript-Weckung 9/10: STAGE1 DECIDE 0x801039fc `sb 2,6` @0x80103a1c, `+0x8f = 0xf` @0x80103a30, ANIMATE im selben Takt (@0x80101a20 / @0x80101a54) | STAGE1 | Dispatcher-Tabelle @0x8011f80c: [4]=0x8010187c, [5]=[6]=0x801018f8, [7]=[8]=0x80101974, [9]=[10]=0x801019f0. 0x801019f0: `80101a10 addiu at,..=0x8011f9dc` -> `80101a20 jalr` (DECIDE [0x8011f9dc]=0x801039fc), dann `80101a44 =0x8011f9d4` -> `80101a54 jalr` (ANIMATE [0x8011f9d4]=0x80103a58). 0x801039fc: `80103a0c lbu v0,6 / 80103a14 bne / 80103a1c sb v0(=2),6 / 80103a28 ori 0xf / 80103a2c jal rng / 80103a30 sb v0,143` | stimmt. Nebenbefund: [7]/[8] (0x87/0x88) haben DECIDE [0x8011f9d8] = 0x801039f4 = `jr ra` (kein Wecker), ANIMATE = dieselbe Phasenmaschine 0x80103a58 -> bleiben in P0, bis das Skript sie umstellt (Port-Teil G3 passt dazu) |
| Wurzel-Familie 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f (Registrierung 0x80072bac) | STAGE1/2/3 | eigener Registrierungs-Scan (lui/addiu-Verfolgung bis `sw` nach 0x80072bac+4*Typ): STAGE1 0x80100424, STAGE2 0x801002b8, STAGE3 0x80100510 <- genau die Typen 0x10 0x11 0x12 0x16 0x18 0x1c 0x1d 0x1e 0x1f; 0x13 hat einen eigenen Handler | stimmt |
| Sce_em_set ruft die Wurzel sofort (@0x8004259c) | PSX.EXE | `80042578 sw s0,g_entity(cur)` / `80042588..94 lw v0,0x80072bac[+0x8*4]` / `8004259c jalr v0` | stimmt (INIT posiert im Spawnbild) |
| FUN_8003ef6c: Slot 1 = sub00 unbedingt @0x8003efd4-d8; LADEN @0x8001d5ac, Tuer @0x8001d988 -> FUN_800396fc -> @0x80039a00 | PSX.EXE | `8003efb0 jal 0x8003ee3c` (a0=0 @8003ef94, Zeiger +0x40 = main00) / `8003efc0 ori a0,zero,1` / `8003efc4 lw v0,68(v0)` (+0x44 = sub00) / `8003efd4 jal 0x8003ee3c` / `8003efd8 addu a1,zero,zero`, keine Verzweigung dazwischen; `8001d5ac jal 0x800396fc`, `8001d988 jal 0x800396fc`, `80039a00 jal 0x8003ef6c`. Wort-Scan ueber die ganze EXE: `jal 0x800396fc` genau 2x (0x8001d5ac, 0x8001d988), `jal 0x8003ef6c` genau 1x (0x80039a00), beide Adressen nie als Datenwort | stimmt: im Original startet sub00 bei JEDEM Raumstart, Laden wie Tuer |
| Bankdaten: Clip 0x0c f0 = 0x12d, 0x0e f0 = 0x149 (Clip 0x12 f0 = 0x190, 0x13 f0 = 0x1c2) | CDEMD0.EMS | eigener Leser (Blobkette 2048, dir[3] = Bank-1-EDD), Typ 0x16 Blob @0x0d5800, EDD @Datei 0x0d9388: Clip 0x0c n=1 Wort @0x0d9958 = `0000012d`, 0x0e n=1 @0x0d9a38 = `00000149`, 0x12 n=98 @0x0d9c64 = `00000190`, 0x13 n=120 @0x0d9dec = `000001c2`; Typ 0x10 (@0x004378/4458/4684/480c) und 0x11 (@0x038b18/bf8/e24/fac) wortgleich | stimmt |
| RE2-Stichprobe (aus NB0, weiter im Diff): EXEC[7] P0 Clip 23/22 @0x801037CC-E4, Latch-Clear `andi 0xbfff` @0x80104F0C | EMZ0.BIN (`re2_disasm.py`) | `801037cc lhu v0,538(s0)` / `801037d0 addiu v1,zero,23` / `801037d4 andi v0,v0,0x4` / `801037dc sw v1,332(s0)` / `801037e0 addiu v0,zero,22` / `801037e4 sw v0,332(s0)`; `80104f0c andi v1,v1,0xbfff` / `80104f10 sh v1,270(s0)` (+0x10E) | stimmt |

**ORIGINAL-RAM, dynamisch (neu, eigener Leser `scratchpad/I41abn1/tools/orig_pose_ram.py`):** welche Pose traegt das Skelett eines liegenden
0x84 wirklich? FUN_8001f314 merkt das zuletzt gesetzte Bildwort in `+0x168`; FUN_8001f3bc/FUN_8001f664 schreiben je Teil (Basis
`*(+0x188)`, Stride 0xac) die Zielwinkel nach `+0x78/7a/7c` und kopieren sie bei `+0x8f == 0` nach `+0x60/62/64` (aktuelle Winkel).
Keyframes aus dem RAM-Pool `*(+0x170)` (Kopf: u16 @+2 Datenoffset, u32 @+4: Teile = Byte 0, Bildgroesse = (>>18)*4), 12-Bit-Strom ab
Byte 12 wie FUN_8001f664:
```
mzd_stage2_room.sav (Auslieferungs-EXE, @0x80026e4c = 08 00 e0 03), ROOM2000 in Ruhe:
  slot2 t=16 w=1/0/0 g=84 +0x94=12 +0x95=0 +0x8f=0 d=30168 | +0x168=80137e94 -> Clip 0x0c Bild 0 Wort 0000012d
     Teile 15: aktuelle Winkel (+0x60) == Clip0x0c-f0: 15/15, == Clip0x12-f0: 0/15 | Ziel (+0x78) == 0x0c: 15/15, == 0x12: 0/15
  slot3 (d=28816): identisch (15/15 Clip 0x0c, 0/15 Clip 0x12)
I_belege/orig_2000_wecker_lead02.0.sav: slot2 (fern, P0) wie oben; slot3 (P2, w=1/0/2) +0x168 -> Clip 0x12 Bild 31, +0x95 = 32
I_belege/orig_2000_wecker_lead04.5.sav: slot2 (fern, P0) wie oben; slot3 (P3 durch, g=00) +0x94 = 0x02
```
=> Das Original ZEIGT waehrend P0 Clip 0x0c Bild 0 (Keyframe 0x12d), obwohl `+0x94` schon 0x12 traegt — der Mechanismus von M1
ist nicht nur statisch, sondern am Original-RAM belegt (Teil-genau 15/15). P2 setzt danach Bild fuer Bild (Bild 31 -> `+0x95` 32).

Diff-Suche (alle `+`-Zeilen unter re15_port/engine, include, platform): s. 2.1.

### 2.1 Diff-Suche und Konstanten

- Alle 1914 `+`-Zeilen des Zweigs unter `re15_port/` (`git diff master...HEAD -- re15_port`): KEIN "deferred / tunable / interim /
  for now / faithful / plausib / TODO / approx / FIXME / hack / vorerst". Einziger neuer `getenv`: `RE15_ZOMBIEFRESSER_LOG`
  (Messschiene, `zf_log()`, kein Verhalten). Kein Env-Schalter als Abschluss.
- Neue Konstanten der NB1: `RE15_R41_INIT_POSE_A 0x0C` (@0x80100b70-74) / `_B 0x0E` (@0x80100bb4-b8) — beide selbst gelesen;
  Typliste 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f (Registrierung, selbst gescannt); `motion_init_delay = 1` = Port-Vokabular fuer
  "das naechste anim_set posiert Bild 0" (begruendet mit P1 `j 0x80103a10` ohne jal @0x80103994-98, selbst gelesen);
  `g_current_room_id != 0x1170` = die vorhandene Tuerweg-Regel (main.c Raumwechsel-Block `if (g_current_room_id == 0x1170)`,
  selbst gelesen), kein neuer Wert. Die Haken-Kommentare mischen STAGE1- (DECIDE 0x80103980 / ANIMATE 0x80103a58) und
  STAGE2-Adressen (@0x80103994-98) ohne Stage-Kennung im selben Kommentar (enemy_ai_common.c:4907-4908); der Kopf
  re15_zombiefresser_r41.h nennt die Verschiebung STAGE2 = STAGE1 - 0x16c — Schoenheitsfehler, kein Gate-Bruch.
- Erklaert der Fix den Befund? M1: ja — das Original-RAM zeigt waehrend P0 Clip 0x0c Bild 0 (15/15 Teile), der Port zeigte
  Clip 0x12 Bild 0 (Abnahme 0 S5); der Haken liefert dem Renderer genau 0x0c/0x0e bis zum ersten P2-anim_set. I-E2: ja — das
  Original startet sub00 bei JEDEM Raumstart (FUN_8003ef6c, einziger Aufrufer FUN_800396fc, dessen zwei Aufrufer LADEN und
  Tuer sind); der Port hielt sub00 am LADE-Weg zurueck, sobald main00 einen Ereignis-Thread startete (2000/2001 `04 ff 18 07/08`
  @0x0177c UNBEDINGT, eigener Zensus s. 4.3).

## 3. Messungen am gebauten Spiel (Standard-KI = RE2-Hirn)

Werkzeug: Messkopie `re15_port/build/platform/pc/re15_pc_abnI1_<lauf>.exe` (nach dem Lauf geloescht), Laufordner
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/I41abn1/<lauf>/`
(Sitzungsordner, nicht im Repo), geerbte RE15_*-Umgebung geloescht (`lauf.sh`), KEIN `RE15_SOFTWARE_RENDER` (Bilder =
`RE15_FRAMEDUMP`, beschleunigter Renderer, `RE15_WINDOW_SCALE=1`), Ende per `RE15_EXIT_AT`. Zustaende aus `RE15_STATE_LOG`
(Auswertung `tools/zust.py`: Zustandswechsel je Slot + Zaehler "Bild-Slots mit st=1 ss1=8") und `RE15_ZOMBIEFRESSER_LOG`.

### 3.1 S1 — ROOM2000, Leon, ECHTER Tuerweg 2070 -> 2000

`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2070@250
RE15_PLAYER_POS=-18500,-6400,2048,0 RE15_WEGPLAN=plan.txt RE15_ZOMBIEFRESSER_LOG=zf.log RE15_STATE_LOG=state.log
RE15_EXIT_AT=700#2000 RE15_FRAMEDUMP=0-700/10:fd_`, Plan `RAUM 2070 / WARTE 20 / BLICK 2048 / WARTE 10 / AKTION / RAUM 2000 /
WARTE 10 / GEHE -20600 -10600 200 / BLICK 2048 / WARTE 1500`. exit 0, 42 s; debug.log `PC loaded room2070.rdt`, `PC loaded
room2000.rdt`, `thread start slot=1 first_op=0x06` (sub00), `EXIT_AT: Bild 700 in Raum 2000`.
```
zf.log:  F6 slot=3/4 INIT grid=84 Wort=1/7/0 f10e=4002 | F61 slot=4 WECKT d=3949 warte=10 | F71 slot=4 STEHT AUF (Latch frei) d=3337
         F94 slot=3 WECKT d=3998 warte=5 | F99 slot=3 STEHT AUF (Latch frei) d=3882
state:   F6..F70 beide st=1 ss1=7 g=84 mo=23 af=0 (EXEC[7] liegend) | F72 Slot 4 ss2=3 mo=8 (RE2-Aufstehen) .. F140 af=79 |
         F141 ss1=1 g=00 (Gang) | F172 ss1=10 (Hochziehen an der Kante) | F371 gr=1 (Griff) | HP 80 -> 60 (F400) -> 40 (F451)
         Slot 3: F100 mo=8 .. F172 ss1=1 g=00. "Bild-Slots mit st=1 ss1=8 (Fressen): 0"
```
Bilder `s1_tuer2000/kontakt_0_230.png`, `s1_tuer2000/zoom_liegen_aufstehen.png` (F70..F140, Ausschnitt 2x): F70 beide flach im
Kanal, F80..F100 richtet sich der erste auf, F110..F140 steht er; der zweite liegt bis ~F100, dann ebenfalls auf. Kein Knien,
kein Fressen. Ablauf bildgleich zur Abnahme 0 (die NB1-Aenderungen beruehren das RE2-Hirn nicht: `liege_skelett_clip` gibt fuer
RE2-eigene Typen -1, s. unit G1 RE2 "Halt -1").

### 3.2 S2 — ROOM2001, ELZA, ECHTER Tuerweg 2071 -> 2001

Wie S1, Elza ueber die Spielerwahl (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`), Spawn-Bit
`RE15_SET_FLAG=3:0x45` (sub00-Bedingung 2001), Plan `RAUM 2071 ... AKTION / RAUM 2001 ...`, `RE15_EXIT_AT=700#2001`. exit 0,
57 s; `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `PC loaded room2071.rdt`, `PC loaded room2001.rdt`.
zf.log identisch (Slots 1/2: F6 INIT 1/7/0 4002, F61 WECKT d=3949 warte=10, F71 STEHT AUF, F94 WECKT d=3998 warte=5, F99);
state: liegend bis F71/F99, Clip 8, Gang F141/F172, Griff F371; Fressen 0. Bild `s2_elza2001/zoom_liegen_aufstehen.png`:
Elza (rotes Oberteil) auf dem Steg, F70/F80 beide flach, F100..F140 erster steht auf, F120..F180 zweiter.

### 3.3 S3a — ROOM3010, Leon, ECHTER Tuerweg 3020 -> 3010

Story-Stand `RE15_SET_FLAG=4:9,4:0x0A,4:0xC6` (Spawn-Bedingungen 3000/3010, s. Abnahme 0 §3.3), `RE15_DEBUG_JUMP=3020@250
RE15_PLAYER_POS=-26600,-22500,2048,0`, Plan `RAUM 3020 / WARTE 10 / BLICK 2048 / WARTE 5 / AKTION / RAUM 3010 / WARTE 1500`,
`RE15_EXIT_AT=600#3010`. exit 0, 36 s. zf.log: F6 INIT Slots 1..6 (`grid=84` x5, `grid=85` Slot 4, alle 1/7/0 4002),
`F7 slot=2 WECKT d=2202 warte=1`, `F8 STEHT AUF`. state: Slot 2 F9 mo=8 .. F88 af=79, F89 ss1=1 g=00, F132 gr=1 (Griff); alle
anderen liegen 600 Bilder; Fressen 0. Bild `s3a_tuer3010/zoom_3010.png` (3x): F10 liegt der Zombie neben Leon flach, F30..F70
richtet er sich auf, F90 steht er, F130/F150 packt er Leon; `kontakt_0_150.png`: der Zombie im Vordergrund liegt durchgehend.

### 3.4 S3b — ROOM3000, Leon, ECHTER Tuerweg 3010 -> 3000

`RE15_SET_FLAG=4:9,4:0x0A,4:0xC6 RE15_DEBUG_JUMP=3010@250 RE15_PLAYER_POS=-24000,11500,2048,0`, Plan `RAUM 3010 / ... / AKTION /
RAUM 3000 / WARTE 40 / GEHE -8200 -15800 200 / WARTE 1500`, `RE15_EXIT_AT=600#3000`. exit 0, 37 s. zf.log: 3000-Slots 1..7 INIT
`grid=84 Wort=1/7/0 f10e=4002`, `F80 slot=1 WECKT d=3998 warte=3`, `F83 STEHT AUF`. state: F84 mo=8 af=9, F154 af=79, F155 ss1=1
g=00, F229 gr=1, HP 100 -> 60; Slots 2..7 (d 8318..16318) liegen 600 Bilder (`ss1=7 g=84 mo=23`); Fressen 0.
Bild `s3b_tuer3000/kontakt_70_240.png`: Polizisten-Zombie am Streifenwagen liegt flach und bleibt liegen, der Geweckte kommt
von links unten und packt Leon (F230/F240).

### 3.5 S7 — ROOM3001, ELZA, ECHTER Tuerweg 3011 -> 3001

Wie S3b mit Elza (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`), Plan `RAUM 3011 ... AKTION / RAUM
3001 ...`, `RE15_EXIT_AT=500#3001`. exit 0, 66 s; `[pl] Spieler-Familie PL04`, `JUMP -> 301 DOWNLOAD AREA (ROOM3011)`, `PC loaded
room3011.rdt`, `PC loaded room3001.rdt`. zf.log: 3001-Slots 1..7 INIT 1/7/0 4002, `F80 slot=1 WECKT d=3998 warte=3`, `F83 STEHT
AUF`; state wie S3b (Clip 8 F84..F154, Gang F155, Griff F229; die 6 fernen liegen); Fressen 0.

### 3.6 S7b — ROOM3011, ELZA, ECHTER Tuerweg 3021 -> 3011

Wie S3a mit Elza (Spielerwahl wie S2, `RE15_DEBUG_JUMP=3020@250` -> ROOM3021), Plan `RAUM 3021 / ... / AKTION / RAUM 3011 / WARTE 1500`,
`RE15_EXIT_AT=400#3011`. exit 0, 58 s; `Spieler-Familie PL04`, `PC loaded room3021.rdt`, `PC loaded room3011.rdt`. zf.log: F6 INIT
Slots 1..6 (`grid=84` x5, `grid=85` Slot 4, alle 1/7/0 4002), `F7 slot=2 WECKT d=2202 warte=1`, `F8 STEHT AUF`; state: Slot 2 F9 mo=8
.. F88 af=79, F89 g=00 Gang, F132 gr=1; Fressen 0. Damit sind alle sechs Raeume des Punkts (2000/2001, 3000/3001, 3010/3011) ueber
echte Tuerwege gemessen.

## 4. Messungen zur Nachbesserung 1

### 4.1 M1 — RE1.5-KI (`RE15_AI_FLAVOR=re15`), Liegepose vor dem Wecken, ECHTER Tuerweg

**S5, ROOM3000 (Tuer 3010 -> 3000)**, wie S3b plus `RE15_AI_FLAVOR=re15 RE15_POSE_DUMP=pose.log` (Renderer-Ebene: `re15_compute_actor_kf`
mit dem `clip_override` aus `re15_actor_anim_select`, main.c:10157). exit 0, 36 s.
```
zf.log:   F80 slot=1 WECKT(RE1.5) d=3998 warte=3 | F83 slot=1 P2-SAAT clip=18 frac=15
pose.log (Segment Raum 3000, 1202 Zeilen):
  slot 1: F0..F83 Keyframe 301 (= 0x12d, Clip 0x0c Bild 0) — 84 Bilder | F84 mo=18 af=0 frac=15 kf=400 (Clip 0x12 Bild 0) |
          F85 af=1 frac=14 kf=401 | F86 af=2 frac=13 ...
  slot 7 (sichtbar, fern): F0..F600 alle 601 Bilder Keyframe 301
  (Raum 3010 davor: slot 4 = 0x85 kf=329 = 0x149, slot 5 = 0x84 kf=301)
state:    F6 alle 7 st=1 ss1=0 ss2=0 g=84 mo=18 | F80 Slot 1 ss2=1 | F83 ss2=2 | F181 ss2=3 | F182 ss1=2 g=00 (P3, 0x201) |
          F467 gr=1; Fressen (RE2 ss1=8 / RE1.5 ss1=12) 0
```
=> Halt bis einschliesslich des P1->P2-Takts (F83), erstes P2-anim_set einen Takt spaeter (F84) mit Bild 0 / +0x8f 15, P2 = 98
Takte (F83..F180, +0x6 = 3 in F181 = T+98, P3 in F182 = T+99) — genau die selbst gelesene Folge P1 `j 0x80103a10` ohne jal
(@0x80103994-98) / P2 f314 (@0x801039a8) / Wrap `addu v1,v1,v0` (@0x801039c4). Bild `s5_re15_3000/vergleich_pose_vorher_nachher.png`
(3x, gleicher Lauf, oben exe der Abnahme 0, unten NB1): vorher liegt der Polizist am Streifenwagen mit angelegten Armen
(Clip 0x12 Bild 0), nachher mit abgespreiztem Arm (Clip 0x0c Bild 0) — das, was das Original-RAM zeigt (§2, 15/15 Teile).

**S5b, ROOM2000 (Tuer 2070 -> 2000), RE1.5-KI**, wie S1 plus `RE15_AI_FLAVOR=re15 RE15_POSE_DUMP=pose.log`. exit 0, 41 s.
`F61 slot=4 WECKT(RE1.5) d=3949 warte=3`, `F64 P2-SAAT`, `F94 slot=3 WECKT(RE1.5) d=3998 warte=10`, `F104 P2-SAAT`; pose: slot 4
Keyframe 301 F0..F64, erstes Fremdbild F65 `kf=400 af=0 frac=15`; slot 3 Keyframe 301 F0..F104, F105 `kf=400 af=0 frac=15`. state:
Slot 4 ss2=2 F64..F161, ss2=3 F162, F163 ss1=2 g=00; Hochziehen F186 (ss1=10), Griff F356 (gr=1). Nebenbefund ohne Wertung: ab F630
(Spieler-HP -1, im RE1.5-Griff gestorben) frisst Slot 3 den toten Spieler (`ss1=12 mo=41` = Sub 0xc, 71 Bild-Slots) — das ist das
RE1.5-Verhalten an der Spielerleiche, nicht der Nutzer-Befund; VOR dem Tod 0 Bilder Sub 0xc / Sub 8.

### 4.2 I-E2 — LADEN eines Spielstands IN ROOM2000/2001

Karte je Lauf mit `probe_r41_zombiefresser_karte` (Werkzeug des Baus, Quelltext gelesen: schreibt per `re15_savedata_capture`/
`re15_memcard_save` einen Spielstand, liest ihn zurueck), dann `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`.

| Lauf | Karte | debug.log (Reihenfolge) | zf.log / state | Ergebnis |
|---|---|---|---|---|
| L1 `l1_laden2000` | `2000 -20600 -10600 2048 1 leon cam=7` | `thread start slot=0 first_op=0x3B` -> `CONTINUE: resumed in room 2000` -> `slot=10 first_op=0x3A` (main00-Evt_exec sub07) -> **`slot=1 first_op=0x06`** (sub00) -> `EXIT_AT: Bild 450`; keine `FALLBACK`-Zeile | F0 Slots 3/4 INIT 1/7/0 4002, F1 WECKT d=3593/2011 warte 10/5, F6/F11 STEHT AUF, F76 Gang, Griff F319; Fressen 0 | Zombies da, liegen, stehen auf, greifen an |
| L2 `l2_laden2001` | `2001 ... elza 3:0x45 cam=7` | `Spieler-Familie PL04`, dann wie L1 (`slot=1 first_op=0x06`) | Slots 1/2 identisch, Griff F319; Fressen 0 | wie L1, als Elza |
| L3 `l3_laden2000_fern` | `2000 -20650 -6200 2048 1 leon cam=13` (Eintrittspunkt der Tuer) | wie L1 | F0 INIT, d = 5724/4314 (>= 4000): 600 Bilder `ss1=7 ss2=1 g=84 mo=23`, kein WECKT | liegen in Ruhe, fressen nie |
| L4 `l4_re15_laden2000` | wie L1, `RE15_AI_FLAVOR=re15`, POSE_DUMP | wie L1 | F1 WECKT(RE1.5) beide warte 10, F11 P2-SAAT; pose: F0..F11 kf=301 frac 0 (F11 frac 15), F12 kf=400 af=0 frac=15, F13 af=1 frac=14 | Halt 0x12d bis P2, dann Blende |

Bilder: `l1_laden2000/kontakt_0_110.png` und `l2_laden2001/kontakt_0_110.png` (Kamera 7): F0/F10 beide flach im Kanal, F20..F60
richten sie sich auf, ab F70 stehen sie, F100/F110 zieht sich einer an der Kante hoch; `l4_re15_laden2000/zoom_halt_blende.png`
(4x, RE1.5): F0/F11 beide flach mit abgespreizten Armen, F12/F14 kaum Aenderung (Blende 0xf00), F18..F40 Aufstehen.
Vorher (Abnahme 0 S8, alte exe): 300 Bilder ohne Zombie-Aktor, kein `slot=1`. => I-E2 behoben, am Lade-Weg wie am Tuerweg.

### 4.3 Nebenwirkungen der Nachbesserung 1

**RE1.5-Liegende in STAGE1 (0x87/0x88), Renderer-Haken.** Der Haken gilt auch fuer sel 7/8 (z.B. 1070, 1100, 1140 Slot 0) unter
einem RE1.5-Hirn. Original-RAM selbst gelesen (`stage_saves/room1140_entry.sav`, Auslieferungs-EXE): Slot 0 `g=88 +0x94=13`,
`+0x168` -> **Clip 0x0e Bild 0** (Wort `00000149`), aktuelle Winkel == Clip 0x0e Bild 0 **15/15**, == Clip 0x13 Bild 0 0/15 — die
Ausweitung auf 0x87/0x88 ist also ebenfalls originaltreu (dieselbe INIT, `sb 0xe,148` + f314 vor `sb 0x13,148`). Die vier echten
Fresser im selben Save (0x86 -> grid 00, Wort 1/12/3) zeigen Clip 0x27 Bild 46 / 0x28 Bild 33 — vom Haken unberuehrt (grid 0).
Am Spiel gemessen (`n2_1070_re15` vs. `n2m_1070_re15`, RE1.5-KI, `RE15_DEBUG_JUMP=1070@250`, 120 Bilder, POSE_DUMP): der sichtbare
0x87-Liegende (Slot 5, +0x94 = 18) zeigt mit der exe dieses Zweigs 121/121 Bilder Keyframe 301 (Clip 0x0c Bild 0), mit der
master-exe (Hauptbaum, 09:57, Codestand 5705fafb, als Messkopie `re15_pc_abnI1m_*` im Spurbaum gestartet) 121/121 Bilder Keyframe 400
(Clip 0x12 Bild 0). Die Aenderung wirkt also wie beabsichtigt auch auf STAGE1; im Bild dieser Kamera ist der Liegende verdeckt
(`n2_1070_re15/vergleich_master_nb1.png` ohne sichtbaren Unterschied) — Beleg ist hier die Renderer-Zeile, nicht das Bild.

**Reichweite des Renderer-Hakens.** `re15_actor_anim_select` hat nur Darstellungs-Aufrufer (`grep`: platform/pc/main.c:8707 Spieler,
:9985 Gegner/NPC, platform/psx/main.c:701, wasser_r38.c:188 nur Spieler); keine Spiellogik (Treffer, Griff, Cliplaenge) liest den
`clip_override` — die KI rechnet weiter mit +0x94 (P2 = 98 Takte, s. 4.1). Der Haken aendert also nur, was gezeichnet wird.

**RE2-Hirn unberuehrt (Standard-KI und Modell-Hybrid).** Standard-KI: S1..S7b zeigen mo=23/22 (EXEC[7]) bis zum Wecken, unit G1 RE2
"Renderer Clip 23 Keyframe 0x2dc (Halt -1)". Modell-Hybrid `RE15_AI_MODELS=re15` (RE2-Hirn + RE1.5-Modelle, `s9_hybrid_3000`, Tuer 3010
-> 3000, POSE_DUMP): vor dem Wecken zeigen die sichtbaren Slots 1 und 7 je 80/80 Bilder `mo=23 kf=732` (RE2-Liegeclip auf dem
RE1.5-Modell), nie Keyframe 301; zf.log `F80 slot=1 WECKT d=3998 warte=3`, `F83 STEHT AUF` wie S3b.

**LADE-Weg in anderen Raeumen.** Eigener Zensus (`scratchpad/I41abn1/tools/main00_evt.py`, Walker `re15_port/tools/aot_sce_census.py`,
206 RDTs, kein Walk-Abbruch): `Evt_exec` (0x04) im main00 nur in 1021 (@0x01e1e, If), 1031 (@0x0200e/02072/020a2/020b2, If),
1170 (@0x012bc/012ec, If), **2000 (@0x0177c `04 ff 18 07`, Tiefe 0 = unbedingt)**, **2001 (@0x0177c `04 ff 18 08`, unbedingt)**,
4010 (@0x006a0, If), 4031 (@0x0057e, If), 5011 (@0x0051c, If); `Evt_chain` (0x03) in keinem main00. Nur diese Raeume koennen die
Aenderung in main.c ueberhaupt spueren (fuer alle anderen war `s_preintro` schon vorher 0); 1170 behaelt die alte Erkennung
(`g_current_room_id != 0x1170` -> break nur fuer andere Raeume), 1171 hat kein main00-Evt_exec.
Gemessen `n1_laden4010` (Karte `4010 -21044 -24000 0 0 leon`, `RE15_EVT_TRACE=1`, 300 Bilder): `slot=0` main00 -> `CONTINUE:
resumed in room 4010` -> `[evt] F0 Evt_exec sub=3 -> slot 10` -> `thread start slot=1 first_op=0x06` (sub00) -> `[evt] F0 Evt_exec
sub=2 -> slot 10` -> `EXIT_AT: Bild 300`; keine `door-3 self-reentry FALLBACK`-Zeile. Bild `n1_laden4010/kontakt.png`: die Szene
(Dialog "Leon, that wound..." / "Yeah, we need to hurry") laeuft durch, kein Haenger. Vorher haette die Halte-Logik sub00
zurueckgehalten und nach dem Ende von sub03 main00+sub00 per `scd_room_reenter` ein ZWEITES Mal laufen lassen (main.c:5663-5673,
gelesen) — die Aenderung entfernt also eine Port-Doppelung und folgt FUN_8003ef6c.

## 5. Tests (gezielt, KEINE volle Suite)

Der Diff beruehrt anim_select_common.c (Gegner-Renderauswahl, jede Darstellung), enemy_ai_common.c / player_common.c (in fast jedem
Unit-Test), enemy_ai_re2_zombie.c und den Boot-/LADE-Pfad in main.c. Deshalb:

- **Ganze Unit-Ebene ohne Release-/Android-Skripttests** (eigener Lauf 14:3x, `-j 4`, Log `scratchpad/I41abn1/ctest_unit_alle.log`):
  `ctest -R "^(unit_|probe_|prop_|re15_tests$|r19_marke1_10d0_c1$|eintritt_pose_einmal$|test_r26_kaefig_release0c$|m93r_nachladen$|
  r28_|r29_|r30_|r30b_|r31_|r33_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_)"` ->
  **`100% tests passed, 0 tests failed out of 927`** (283.39 s, EXIT=0). Darin unit_r41_zombiefresser, unit_anim_rate,
  unit_room1140_combat, unit_p3_restposten_pin, unit_ai_flavor_mixed, unit_r38_wasserzombie.
- `unit_r41_zombiefresser -V` gelesen (`scratchpad/I41abn1/unit_r41_V.log`, keine Schein-Pruefung): D-Grenze 4008 (d=4007) weckt nie,
  4007 (d=3998) weckt (RE1.5 F9, RE2 F0); E 48 Seeds / 15 Wartezeiten; F 1140 Fresser 4 / faelschlich Liegewecker 0 (beide KI);
  G1 2000/3010 RE1.5+MIXED: Spawnbild und Ruhe `Renderer Clip 12 Keyframe 0x12d` bzw. (0x85) `Clip 14 Keyframe 0x149`, RE2 `Clip 23
  0x2dc / Clip 22 0x2d2 (Halt -1)`; G2 2000: `5 Bilder Halt (falsch 0), Saat-Takt F4, erstes P2-Bild F5 Clip 18 Bild 0 +0x8f 15,
  Blende: Abstand zur Halt-Pose 335 / zum ungemischten Bild 4612, nach 16 Bildern 0; +0x6 = 2 fuer 98 Takte (Cliplaenge 98)`,
  3010-0x85 Clip 19 / 120 Takte, 3000 Clip 18 / 98; G3 1070: 5 Liegende (0x88 x3 Keyframe 0x149, 0x87 x2 0x12d), nach Member_set
  0x89/0x8a sofort `Wort 1/0/2 +0x8f 15 Saat 0 -> Renderer Clip 18/19 Bild 0`; `== OK ==`.
- **Integration** (eigener Lauf 14:51-15:26, `-j 3`, unter Fremdlast durch drei weitere Agenten-Suiten, Log
  `scratchpad/I41abn1/ctest_integration_final.log`): Zombie-Module/-Raeume, ALLE Lade-/Boot-Riegel der Raeume mit main00-Evt_exec
  (1021/1031/1170/1171: r36_elza_*, r37_roy_*, r38_elzatoene_*, r38_itemorte, r39_elza_*, r39_johnruf1041*, r38/r39_heliport1171*,
  r39_stimmen_*, r35_entladen_a..l, r38_elzaintro_*, elza_vollstart, boot_bg_pin), RE1.5-/MIXED-Riegel (r36_schuss, r37_elzaems_*,
  r38_affen3*, r38_szenenschutz, r38_wasserzombie, r37_beretta), Speicher-Pins (save_counter_pin, weste_load_pin), room_probe_sweep
  und die 8 eigenen Teile:
  ```
  ctest -R "^integration_(r41_zombiefresser_.*|r38_wasserzombie|r37_zombieaktiv|r37_re2zombie|r39_elzazombies_.*|r36_gegnerzahl|
  r36_schuss|r36_elza_schuss|r39_gegner_.*|r41_wasser20b0.*|r36_elza_.*|r37_roy_.*|r38_elzatoene_.*|r38_itemorte|r39_elza_.*|
  r39_johnruf1041.*|r38_heliport1171.*|r39_heliport1171.*|r39_stimmen.*|r35_entladen_.*|r37_elzaems_.*|r38_affen3.*|
  r38_szenenschutz.*|r37_beretta|r37_kistenregel.*|boot_bg_pin|elza_vollstart|r38_elzaintro_.*|save_counter_pin|weste_load_pin|
  room_probe_sweep)$" -j 3 --timeout 600 --output-on-failure
  100% tests passed, 0 tests failed out of 94          (Total Test time (real) = 2046.04 sec, EXIT=0)
  ```
  Eigene Teile: tuer2000 42.52 s, elza2001 42.36 s, leon3000 19.93 s, leon3010 20.12 s, re15_3000 20.46 s, laden2000 20.53 s,
  laden2001 20.56 s, re15_2000 28.11 s — kein Flattern, keine Wiederholung noetig. Arbeitsordner des Laufs gelesen
  (`build/tests/integration/r41_zombiefresser_wd/re15_2000_95f1d18f`: zf.log `F11 P2-SAAT` beide Slots, pose.log 24 Halt-Zeilen
  kf=301, `F12 ... af=0 frac=15 kf=400`; `laden2000_1c63fffd`: `thread start slot=1` nach `CONTINUE`, INIT/WECKT/STEHT AUF wie L1)
  — die Riegel messen, was sie behaupten. Gegenueber dem Bau-Lauf (61 + 9) zusaetzlich: r35_entladen_a/b/c/g/i/j/k/l,
  r37_elzaems_* (alle 12), r38_affen3*, r38_szenenschutz, r37_kistenregel, r39_stimmen_*, r39_johnruf1041_tuer_*, weste_load_pin.

## 6. Vertrag (VERTRAG.md)

- Pfade (`git diff master...HEAD --name-only`, Zweig-eigene Aenderungen): analysis/befunde_runde41/{AUFTRAG.md (nur vom master
  uebernommen: 9cdcc979/61802b39 = Orchestrator-Commits, gegen master nur zurueck, keine eigene Zeile), I_abnahme_0.md, I_belege/*,
  I_zombiefresser.md}, re15_port/engine/src/{anim_select_common.c, enemy_ai_common.c, enemy_ai_re2_zombie.c, player_common.c,
  zombiefresser_r41.c}, include/re15_zombiefresser_r41.h, platform/pc/main.c, tests/integration/{test_r38_wasserzombie.cmake,
  test_r41_zombiefresser.cmake}, tests/unit/{probe_p3_restposten.c, probe_r41_zombiefresser_karte.c, probes/r41_zombiefresser.cmake,
  test_anim_rate.c, test_r41_zombiefresser.c, test_room1140_combat.c}, tools/r41_zombiefresser/*. Verbotene Pfade (release/,
  platform/android/, shared_assets/, pri/, synchro/, CMakeLists.txt): **0 Treffer**. Registrierung ueber `tests/unit/probes/*.cmake`.
- Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine. Die zwei `re15_game_flag_set` im Diff stehen im Kartenwerkzeug bzw.
  im Unit-Test (Story-Bits fuer den Messstand, z.B. 3:0x45 / 4:9), nicht im Spielcode.
- NB1 aenderte KEINE fremden Tests (Commit-Liste a06bd0ac..244a9574: nur test_r41_zombiefresser.c/.cmake, probes/r41_zombiefresser.cmake,
  probe_r41_zombiefresser_karte.c); die vier alten Pins der NB0 hat Abnahme 0 geprueft.
- Gemeinsame Dateien (§1.3), alle Haken klein und markiert "Runde 41 Spur I":
  * enemy_ai_re2_zombie.c (Spur I zugeteilt): unveraendert seit Abnahme 0.
  * enemy_ai_common.c (I-E1 angenommen: Include + 2 Zeilen): jetzt Include + 3 Zeilen (Z. 4906-4908, +0x6 merken + `re15_r41_liege_p2_saat`).
  * player_common.c (I-E1 angenommen): unveraendert 1 Zeile.
  * **anim_select_common.c (laut §1.3 Spur J/K): NEU 3 Zeilen** im GEGNER-Zweig von `re15_actor_anim_select` (Z. 292-294) — nicht
    durch I-E1 gedeckt. Sachlich die einzige gemeinsame Auswahlstelle beider Renderer; die NPC-Zweige (J/K) beruehrt der Haken nicht.
  * **platform/pc/main.c (laut §1.3 D/F/G): NEU 2 Zeilen** im Boot-/LADE-Pfad (Z. 4884-4885) — inhaltlich von I-E2 verlangt ("Ursache
    im Boot-/Lade-Pfad belegen"), aber nicht in der Buchstabenliste.
  Bewertung wie die Praxis dieser Runde (B5, A-E3, I-E1; Abnahme A 1 §6 "klein und markiert ... fuer die Integration: Konfliktflaechen
  ... Kein Mangel"): **kein Mangel, aber Integrationshinweis V2** (s. 7.3): bei der Integration mit J/K (anim_select_common.c) und
  D/F/G (main.c) beidseitig gegenlesen; der Orchestrator moege §1.3 wie bei B5/I-E1 um Spur I erweitern.
- Riegel je Punkt (gelesen, nicht nur gezaehlt): Nutzer-Punkt = unit_r41_zombiefresser A-F + integration tuer2000/elza2001 (echte
  Tuerwege)/leon3000/leon3010/re15_3000; M1 = unit Teil G (G1 Spawnbild/Ruhe RE1.5/MIXED/RE2, G2 Wecken mit Saat-Takt und Blende,
  G3 0x87/0x88 Skript-Wecken) + integration re15_2000/re15_3000 (POSE_DUMP-Pruefung "Keyframe 301/329 bis zum ersten P2-Bild, dieses
  genau einen Takt nach P2-SAAT, Bild 0 / +0x8f 15"); I-E2 = integration laden2000/laden2001 (`thread start slot=1` nach dem
  CONTINUE, erstes Bild 2 Liegende grid 84, INIT, Wecken d < 4000, Wartezeit <= 16, aufrecht, Angriff, nie Fressen). Messend.
  Parallel-/Linux-fest (Muster wie Abnahme 0 geprueft: eigener Laufordner `${TEIL}_${_lauf_id}`, Messkopie `re15_pc_r41i_<teil>_*`,
  geerbte RE15_* geloescht, `${_exe_ext}`, `cmake_minimum_required`, `m` ausser WIN32).
  Kein Riegel fuer die Lade-Nebenwirkung in 4010/4031/5011 (Bau-Agent: "ohne vorhandenen Test") — Hinweis H5, kein Mangel (kein
  Nutzer-Punkt; selbst gemessen 4.3).

## 7. Urteil, Maengel, Hinweise

### 7.1 Urteil je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| ROOM2000 (Leon): "am Boden und fressen, anstatt aufzustehen wenn man ihnen naeher kommt" (Standard-KI) | **erfuellt** | S1 echter Tuerweg 2070 -> 2000: beide liegen flach (EXEC[7] Clip 23), 0 Bilder Fressen, Wecken bei d 3949/3998 (< 0xFA0), Aufstehen nach 10/5 Takten (Clip 8), Gang, Hochziehen, Griff F371; Bild F70..F140 |
| ROOM2001 (Elza) | **erfuellt** | S2 echter Tuerweg 2071 -> 2001 als Elza (PL04): identisch, 0 Bilder Fressen; Bild F70..F180 |
| STAGE3 ROOM3000 / 3010 "etc." (3001/3011 = Elza-Varianten; laut Zensus sonst keine 0x84/0x85 in STAGE3) | **erfuellt** | S3a Tuer 3020 -> 3010 (Wecken d 2202, Griff F132, Vordergrund-Zombie liegt durch), S3b Tuer 3010 -> 3000 (Wecken d 3998, Griff F229, 6 ferne liegen 600 Bilder), S7 Elza 3011 -> 3001, S7b Elza 3021 -> 3011; Fressen ueberall 0 |
| Dieselben Zombies mit RE1.5-KI (M1 der Abnahme 0: Liegepose vor dem Wecken) | **erfuellt** | S5/S5b (Tuerwege) + L4 (Laden): Keyframe 0x12d bis einschliesslich P1->P2-Takt, erstes P2-Bild einen Takt spaeter Bild 0 / +0x8f 15, P2 98 Takte, dann Kampf — deckungsgleich mit Original-RAM (Clip 0x0c Bild 0, 15/15 Teile) und dem selbst gelesenen Code; Bild vorher/nachher |
| I-E2: Laden eines Spielstands IN 2000/2001 zeigt keine Zombies | **erfuellt** | L1/L2/L3: sub00 startet nach dem CONTINUE (`thread start slot=1`), beide 0x84 liegen, wecken, stehen auf, greifen an bzw. liegen in Ruhe; Original FUN_8003ef6c startet sub00 an jedem Raumstart (selbst gelesen) |
| V1 (Vertrag der Abnahme 0) | erledigt | Orchestrator-Entscheid I-E1 |

Gates: **RE-Gate haelt** (§2: elf Tabellenzeilen mit ueber 40 selbst gelesenen Adressen in STAGE1/2/3, PSX.EXE, CDEMD0.EMS und
EMZ0, alle wie zitiert; Original-RAM 15/15 Teile fuer 0x84 (2000) und 0x88 (1140); keine Guess-Tells, ein Messschienen-getenv;
der Fix erklaert beide Befunde). **Tests gruen** (§5: 927/927 Unit-Ebene, 94/94 Integration, ohne Wiederholung). **Vertrag haelt**
(§6: Pfade, keine Bits/Ids/Slots/Ereignisse, alle Haken klein und markiert; Integrationshinweis V2).

### 7.2 Maengel

Keine. (M1 der Abnahme 0 ist behoben und am Original-RAM wie am Spiel belegt; V1 ist durch I-E1 erledigt; I-E2 ist behoben.)

### 7.3 Hinweise (kein Mangel dieser Spur)

- **V2 (Vertrag/Integration):** neue kleine, markierte Haken ausserhalb der §1.3-Buchstabenliste: `anim_select_common.c:292-294`
  (Datei laut §1.3 Spur J/K; Haken nur im GEGNER-Zweig) und `platform/pc/main.c:4884-4885` (laut §1.3 D/F/G; von I-E2 inhaltlich
  verlangt). Bei der Integration beidseitig gegenlesen; Bitte an den Orchestrator, §1.3 wie bei B5/I-E1 um Spur I zu erweitern.
  `enemy_ai_common.c`: der in I-E1 angenommene Haken ist um eine Zeile gewachsen (Include + 3 Zeilen, 4906-4908; A-Haken EM024 an
  anderer Stelle).
- **H1 (unveraendert, RE2-Hirn):** Spieler direkt ueber dem Kanal (< 2000): ein aufgestandener Kanal-Zombie schnappt nach oben
  (RE2-DECISION[2] Block A @0x8010205C-94 / EXEC[14], von Abnahme 0 selbst gelesen). In S1 kurz sichtbar (Slot 3 F333..F361
  `ss1=14 mo=17`), danach zieht er sich hoch (F388 `ss1=10`). RE2-Retail-Verhalten.
- **H2 (unveraendert):** Standard-KI steht mit RE2-Clip 8/9 (80 Bilder) auf, RE1.5-KI mit Clip 0x12/0x13 (98/120) — Praesentation
  des jeweiligen Hirns.
- **H3 (Abnahme 0, jetzt teilweise selbst gelesen):** Nach dem Loesen aus dem RE2-Griff gleiten die Kanal-Zombies ~2500-2700
  Einheiten weg (S1 Slot 3 F461 (-22068,-10099) -> F474 (-24782,-10099) in `ss1=9`; Slot 4 F462 (-20573,-9304) -> F474 (-20784,-6781)).
  RE2 EXEC[9] selbst gelesen (EMZ0): `80103f60 addiu v0,zero,400 / 80103f64 sh v0,324(s1)` (+0x144 = 400), je Takt `80103fcc addiu
  v0,v0,-30 / 80103fd0 sh v0,324(s1)`, Bewegung `jal 0x800152c8` (RE2_Quellcode_V2/FUN_800152c8.c: ApplyMatrixSV auf +0x144, Ergebnis
  ungeskaliert auf x/z) -> Summe 400+370+...+10 ~ 2870 Einheiten. Der Weg des Partners (EXEC[9]) liegt damit in der RE2-Groessen-
  ordnung; den Weg des Griff-Zombies (`ss1=3` Phase 8 -> `ss1=5`) habe ich nicht disassembliert. Kein Teil dieses Nutzer-Punkts.
- **H4 (Messhinweis des Baus, bestaetigt):** waehrend des Halts nennt `RE15_ANIM_TRACE` den Uhr-Clip 0x12 neben dem Render-Clip 0x0c;
  der Blutanker `schuss_r36.c zeichen_bank` kennt den Halt nicht — folgenlos, solange P0..P2 unschiessbar sind (+0x93 Bit 0
  @0x80103940-4c, Abnahme 0 selbst gelesen).
- **H5 (Riegel-Luecke, klein):** Die Lade-Nebenwirkung in 4010/4031/5011 (sub00 sofort statt Halte + zweiter main00-Lauf) hat keinen
  eigenen Riegel; selbst gemessen fuer 4010 (4.3), vom Bau fuer 4031/5011 (I_belege/nb1_laden_nebenwirkung.txt). Ein Lade-Riegel
  fuer einen dieser Raeume waere billig.
- **Kommentar-Schoenheitsfehler:** enemy_ai_common.c:4906-4908 mischt STAGE1- und STAGE2-Adressen ohne Stage-Kennung (s. 2.1).

### 7.4 Gesamt

**bestanden = JA.** Der Nutzer-Punkt ist in allen genannten Raeumen ueber ECHTE Tuerwege erfuellt (2000 Leon, 2001 Elza, 3000/3010
Leon, 3001/3011 Elza): die 0x84/0x85-Zombies liegen flach, fressen nie, stehen nach dem Original-Wecker (+0x1d0 < 0xFA0, Wartezeit
rand&0xf) auf und greifen an; ferne bleiben liegen. Die Nachbesserung 1 ist belegt: mit der RE1.5-KI zeigt der Port bis zum ersten
P2-anim_set die Original-Liegepose Clip 0x0c/0x0e Bild 0 (Original-RAM 15/15) und blendet einen Takt nach P1->P2 in das Aufstehen,
P2 98/120 Takte wie im Original; das Laden eines Spielstands IN 2000/2001 zeigt die Zombies wie der Tuerweg (sub00 wie FUN_8003ef6c).
Offen fuer den Orchestrator nur der Integrationshinweis V2 (Haken in anim_select_common.c / main.c ausserhalb der §1.3-Liste) und die
Hinweise H1..H5 (kein Teil dieses Punkts).
