# Runde 41 — Spur I "zombiefresser": UNABHAENGIGE ABNAHME 0 (2026-10-24)

Baum `.claude/worktrees/r41_zombiefresser`, Zweig `r41/zombiefresser`, HEAD 773ffdb8, master 7b59e297 (= Merge-Basis,
der Zweig enthaelt master). Alles unten ist SELBST gemessen bzw. SELBST disassembliert; Behauptungen des Bau-Agenten
gelten nur, wo hier ein eigener Beleg steht. (Bericht fortlaufend geschrieben; Stand: abgeschlossen.)

## 0. Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und fressen,
> anstatt aufzustehen wenn man ihnen näher kommt. gleiches trifft zu bei den Zombies in STAGE3 in ROOM 3000, 3010
> etc. es ist durchgehend falsch.

## 1. Bau

- `bash re15_port/tools/local_build.sh configure` + `... build`: `=== LOCAL-BUILD-OK (configure)`, `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 10:57:38, letzte Aenderung an
  engine/include/platform 10:57:01 (ce430484; danach nur Tests/Doku) -> exe = Codestand HEAD.

## 2. RE-Gate (selbst disassembliert)

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` (Baum-Kopie, Binaerdateien
`info/Re1.5/PSX/BIN/STAGE{1,2,3}.BIN` = md5-gleich mit dem Hauptbaum, `info/re2leon/COMMON/BIN/EMZ0.BIN`).

| Zitat im Diff | Datei | selbst gelesen | Befund |
|---|---|---|---|
| Tor 0x80 @0x80100b44 | STAGE2 | `80100b38 lbu v1,9(v0)` / `80100b40 andi v0,v1,0x80` / `80100b44 beq v0,zero,0x80100cc4` / `80100b48 andi v1,v1,0x1f` | stimmt |
| sel {4,7,9} -> +0x94 = 0x0c @0x80100b74 | STAGE2 | `80100b4c..60 ori 4/7/9 + beq` / `80100b70 ori v0,zero,0xc` / `80100b74 sb v0,148(v1)` | stimmt |
| Pose f314 @0x80100c34 | STAGE2 | `80100c28 addu a2,zero,zero` / `80100c2c lw a0,368` / `80100c30 lw a1,372` / `80100c34 jal 0x8001f314` / `80100c38 ori a3,zero,0x200` | stimmt (Pose = +0x94 = 0x0c Bild 0) |
| FINAL +0x94 = 0x12 @0x80100c7c, 0x13 @0x80100cc0 | STAGE2 | `80100c78 ori v0,zero,0x12` / `80100c7c sb v0,148(v1)`; `80100cbc ori v0,zero,0x13` / `80100cc0 sb v0,148(v1)` (ohne f314) | stimmt |
| Fresser sel 6: grid = 0 @0x80100d44 | STAGE2 | `80100cd4..dc ori 6 / bne` / `80100ce4 ori 0x27 / sb 148` / `80100ce0 lui v1,0x2`+`80100cf4 ori v1,v1,0xc01`+`80100cf8 sw v1,4` (= 0x20c01) / `80100d44 sb zero,9(v0)` | stimmt; Weg 0x80100cc4 wird MIT und OHNE Bit 0x80 erreicht |
| Dispatcher [4]/[5] @0x80117920/24 | STAGE2 | Tabelle @0x80117910: [4]=0x80101710, [5]=[6]=0x8010178c, [7]=[8]=0x80101808 | stimmt |
| DECIDE/ANIMATE 0x80103814 / 0x801038ec | STAGE2 | 0x80101710: `addiu at,at,31436`=0x80117acc, jalr; `addiu at,at,31440`=0x80117ad0, jalr (DECIDE dann ANIMATE). 0x8010178c: 0x80117ad4/0x80117ad8. Tabelle: acc=ad4=0x80103814, ad0=ad8=0x801038ec | stimmt |
| Wecker 0xFA0 @0x8010382c | STAGE2 | `80103824 lw v0,464(v1)` / `8010382c sltiu v0,v0,0xfa0` / `80103830 beq` / `80103838 lbu v0,6` / `80103840 bne` / `8010384c sb v0,6` (v0=1, Delay-Slot) | stimmt |
| Wartezeit rand&0xf @0x80103858-60 | STAGE2 | `80103848 jal 0x8001af20` / `80103858 andi v0,v0,0xf` / `80103860 sh v0,156(v1)` (Delay-Slot) | stimmt |
| Stoehnen 1/4 SE 5 @0x80103864-74 | STAGE2 | `8010385c jal rng` / `80103864 andi v0,v0,0x3` / `80103868 bne` / `80103870 jal 0x800453d0` / `80103874 ori a0,zero,0x5` | stimmt (Reihenfolge rand Wartezeit vor rand SE = Port) |
| P0 +0x93 \|= 1 @0x80103940-4c, +0x1b8 = 1 | STAGE2 | `80103940 lbu v0,147(a1)` / `80103948 ori 1` / `8010394c sb` / `80103960 sb v0,440(v1)` | stimmt |
| P1 Post-Dekrement @0x80103974 | STAGE2 | `80103964 lhu v1,156(a1)` / `8010396c addiu v0,v1,-1` / `80103970 bne v1,zero` / `80103974 sh v0,156(a1)` (Delay-Slot) / `80103984 sb 2,6` / `80103998 sb 0xf,143` | stimmt |
| P2 f314 a3=0x100 @0x801039a8 | STAGE2 | `801039a8 jal 0x8001f314` / `801039ac ori a3,zero,0x100` / `801039c4 addu v1,v1,v0` / `801039cc sb v1,6` | stimmt |
| P3 grid 0 + 0x201 @0x801039d0-e0 | STAGE2 | `801039d0 sb zero,9(a1)` / `80103934 ori v0,zero,0x201` (Delay-Slot) / `801039e0 sw v0,4(v1)` | stimmt |
| STAGE3: Wecker @0x80103a84, Tor @0x80100d9c, FINAL @0x80100ed4 | STAGE3 | `80103a84 sltiu v0,v0,0xfa0`; `80100d98 andi 0x80 / 80100d9c beq`; `80100ed0 ori 0x12 / 80100ed4 sb 148`; Dispatcher @0x8011d95c [4]=0x80101968 [5]=[6]=0x801019e4; Zeilen @0x8011db18..24 = 0x80103a6c/0x80103b44 (x2) | stimmt |
| STAGE1: Wecker @0x80103998 | STAGE1 | `80103990 lw v0,464` / `80103998 sltiu v0,v0,0xfa0`; Dispatcher @0x8011f80c [4]=0x8010187c [5]=[6]=0x801018f8 [7]=[8]=0x80101974; Zeilen @0x8011f9c8..dc = 80103980/80103a58/80103980/80103a58/801039f4/801039fc | stimmt |
| RE2 EXEC[7] P0..P2 @0x801037CC-80 | EMZ0 | `801037cc lhu v0,538` / `801037d0 addiu v1,zero,23` / `801037e0 addiu v0,zero,22` / `80103804..14 +0x1D3 \|= 0x80` / `8010381c lhu v0,270 / 80103824 andi 0x4000 / 80103828 bne` / `80103834 sb 2,6` / `80103838 jal 0x80015fe8 / 80103840..48 0xf0008 + (rand&0xf)<<8` / `8010386c..80 0xf0009` | stimmt |
| RE2 Latch-Clear nur @0x80104F0C | EMZ0 | Byte-Suche ueber ganz EMZ0.BIN: `andi ...,0xbfff` genau 1x (@0x80104f0c, im 0x60501-Sturzzweig); `ori ...,0x4000` 7x (0x80100a5c, 0x80100aa0, 0x80101a14, 0x80102234, 0x80103a4c, 0x801041b8, 0x80108294) | stimmt: RE2-Overlay hat keinen Naehe-Wecker fuer den Latch |
| RE2 DECISION[2] Block A @0x8010205C-94 (OFFEN 1) | EMZ0 | `80102050 lbu +0x23e / bne`, `8010205c sll v0,s3,16 / 80102060 bne`, `80102064 sltiu v0,s2,0x7d0`, `80102070/74 lbu 262(s0)/262(s1) / 8010207c beq`, `80102084 lbu 467(s1) / 8010208c bne`, `80102090 addiu v0,zero,3585` / `80102094 sw v0,4(s0)` (= 0x0E01) | stimmt |

ORIGINAL-RAM selbst gelesen (`re15_ss.Ram`, Entity @0x800acc2c Stride 0x1f4):
- Alle drei Saves (`stage_saves/mzd_demo/mzd_stage2_room.sav`, `I_belege/orig_2000_wecker_lead02.0.sav`, `..._lead04.5.sav`)
  laufen auf der AUSLIEFERUNGS-EXE: @0x80026e4c = `08 00 e0 03` (jr ra, nicht der PATCHED-Sprung), und der
  STAGE2-Overlay im RAM ist auf 0x80100000..0x80117000 wortweise identisch mit `info/Re1.5/PSX/BIN/STAGE2.BIN`
  (0 abweichende Worte; Stichworte @0x8010382c/0x80100b44/0x80100c7c/0x801039a8 bytegleich).
- Ruhe (mzd_stage2_room): Slot 2/3 `t=0x16 w=1/0/0/0 g=0x84 +0x94=0x12 +0x95=0 +0x93=0x01 +0x1b8=1 d=30168/28816` -> P0 liegend, unschiessbar.
- lead02.0 (RAM-Patch Slot 3 nach d 3197): `w=1/0/2/0 g=0x84 +0x94=0x12 +0x95=32 +0x93=0x01 +0x9c=-1` -> P2 spielt Clip 0x12; Slot 2
  (fern) unveraendert P0.
- lead04.5: Slot 3 `w=1/2/1/0 g=0x00 +0x94=0x02 +0x93=0x00 +0x1b8=0` -> P3 durch, Kampf-Gang; Slot 2 liegt weiter.

=> Mechanismus belegt: 0x84/0x85 = Liegender (Pose Clip 0x0c Bild 0), Wecker d < 4000, Wartezeit rand&0xf,
Aufstehen Clip 0x12/0x13, dann Kampf. KEIN Fressen. Der Fix erklaert den Befund: der Port hatte sel 4/5 im RE2-INIT in
EXEC[8] (Fressen) geschickt (alter Zweig `sel == 6 || sel == 4 || sel == 5`, Diff enemy_ai_re2_zombie.c:8741).

Diff-Suche (alle `+`-Zeilen unter re15_port/): kein "deferred/tunable/interim/for now/faithful/plausibel/TODO/approx".
Einziger neuer getenv: `RE15_ZOMBIEFRESSER_LOG` (Messschiene, kein Verhalten). `re15_r41_liegewecker_enable()` ist
ein Test-Schalter ohne env (Standard AN). Konstanten: 0xFA0, 0xF, 3, 5 je mit Adresse; `RE15_R41_UNSCHARF = -1` als
Port-Sentinel gekennzeichnet (Original-Wert nach der Wartezeit ebenfalls 0xffff, RAM lead02: t9c = -1 — belegt).

## 3. Messungen am gebauten Spiel

Werkzeug: Messkopie `re15_port/build/platform/pc/re15_pc_abnI0_<lauf>.exe` (nach dem Lauf geloescht), Laufordner im
Scratchpad `abn/<lauf>/` (= `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn/`,
Sitzungsordner, NICHT im Repo; Bilder/Logs dort) (geerbte RE15_*-Umgebung geloescht), KEIN `RE15_SOFTWARE_RENDER` (Bilder =
`RE15_FRAMEDUMP` = Rueckleser vor SDL_RenderPresent, beschleunigter Renderer, `RE15_WINDOW_SCALE=1`), Ende per
`RE15_EXIT_AT=<bild>#<raum>`. Zustaende aus `RE15_STATE_LOG` (`[slot t st ss1 ss2 g mo af d @(x,z)]`) und
`RE15_ZOMBIEFRESSER_LOG`. Zensus selbst gefahren (`tools/r41_zombiefresser/fresser_zensus.py`): 0x84 x28 / 0x85 x2 nur in
2000/2001 (sub00), 3000/3001 (main00, je 7), 3010/3011 (main00, 5x 0x84 + 1x 0x85); echte Fresser sel 6 nur 1140 (0x86 x4)
und 10E0/10E1 (0x06 x4). STAGE3 sonst: 30D0/30D1 8x 0x02, 30E0/30E1 2x 0x02 (stehend), 3010/3011 zusaetzlich 0x83/0x87/0x88.

### 3.1 S1 — ROOM2000, Leon, ECHTER Tuerweg 2070 -> 2000, RE2-KI (Standard)

Befehl (`abn/s1_tuer2000`): `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp
RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2070@250 RE15_PLAYER_POS=-18500,-6400,2048,0 RE15_WEGPLAN=plan.txt
RE15_ZOMBIEFRESSER_LOG=zf.log RE15_STATE_LOG=state.log RE15_EXIT_AT=700#2000 RE15_FRAMEDUMP=0-700/10:fd_`;
Plan `RAUM 2070 / WARTE 20 / BLICK 2048 / WARTE 10 / AKTION / RAUM 2000 / WARTE 10 / GEHE -20600 -10600 200 / BLICK 2048 /
WARTE 1500`. exit 0, 41 s. debug.log: `PC loaded room2070.rdt`, `PC loaded room2000.rdt`, `EXIT_AT: Bild 700 in Raum 2000`.

zf.log (vollstaendig):
```
F6  slot=3 INIT grid=84 Wort=1/7/0 f10e=4002
F6  slot=4 INIT grid=84 Wort=1/7/0 f10e=4002
F61 slot=4 WECKT d=3949 warte=10
F71 slot=4 STEHT AUF (Latch frei) d=3337
F94 slot=3 WECKT d=3998 warte=5
F99 slot=3 STEHT AUF (Latch frei) d=3882
```
state.log (Wechsel je Slot, Auszug): F1 beide `st=0 g=84 mo=23` (Spawnbild) -> F6/F7 `st=1 ss1=7 ss2=1 g=84 mo=23 af=0`
(EXEC[7] liegend) bis F70/F99; Slot 4 F71 `ss2=2`, F72 `ss2=3 mo=8 af=11` (RE2-Aufstehen Clip 8, Startbild rand),
F140 `af=79`, F141 `ss1=1 g=00` (Gang); Slot 3 F100 `mo=8 af=8` ... F172 `ss1=1 g=00`. Danach Slot 4 F172 `ss1=10`
(Hochziehen an der Kante), F350 oben (-20928,-9419), F371 Griff (`gr=1`, `ss1=3`), Spieler-HP 80 -> 60 (F400) -> 40 (F450).
In 700 Bildern KEIN `ss1=8` (Fressen). Bilder: `abn/s1_tuer2000/kontakt_0_350.png` (F0..F350 je 10),
`abn/s1_tuer2000/zoom_liegen_aufstehen.png` (F70/F80/F100/F120/F130/F140): F70 beide flach im Kanal, ab F80 richtet sich
Slot 4 auf, F140 steht er; Slot 3 liegt bis ~F100 und steht dann ebenfalls auf.
Nebenbeobachtung (nicht Teil des Punkts, RE2-Griffausgang): nach dem Loslassen (F474/F475) fallen BEIDE Zombies
(`ss1=5 g=80`, Sturz-Executor) und stehen mit Clip 9 wieder auf (F637/F685).

### 3.2 S2 — ROOM2001, ELZA, ECHTER Tuerweg 2071 -> 2001, RE2-KI

Befehl (`abn/s2_elza2001`): wie S1, aber Elza ueber die Spielerwahl (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1`, VERTRAG-Rezept), `RE15_SET_FLAG=3:0x45` (Spawn-Bedingung 2001 sub00 @0x01826 `If Ck(3,0x45,1)`;
das Spiel setzt das Bit selbst in 2001 @0x01a8e `Set(3,0x45,1)`), `RE15_DEBUG_JUMP=2070@250` (Loader -> ROOM2071), Plan
`RAUM 2071 ... AKTION / RAUM 2001 ...`, `RE15_EXIT_AT=700#2001`. exit 0, 56 s. debug.log: `[pl] Spieler-Familie PL04
(character=4, Elza-Bit=1)`, `PC loaded room2071.rdt`, `PC loaded room2001.rdt`, `EXIT_AT: Bild 700 in Raum 2001`.
zf.log: `F6 slot=1/2 INIT grid=84 Wort=1/7/0 f10e=4002`, `F61 slot=2 WECKT d=3949 warte=10`, `F71 STEHT AUF`, `F94 slot=1
WECKT d=3998 warte=5`, `F99 STEHT AUF`. state.log: identischer Ablauf wie S1 (liegend `ss1=7 mo=23` bis F71/F99, Clip 8,
Gang F141/F172, Hochziehen, Griff F371 `gr=1`, HP 80 -> 40); 0 Bild-Slots mit `ss1=8`. Bild `abn/s2_elza2001/kontakt_60_170.png`:
Elza auf dem Steg, F70 beide Zombies flach im Kanal, F80..F140 richtet sich der erste auf, F110..F170 der zweite.

### 3.3 S3a — ROOM3010, Leon, ECHTER Tuerweg 3020 -> 3010, RE2-KI

Story-Stand: die 3000/3010-Zombies spawnen nur mit 4:9 (3000 main00 @0x014b6 `If Ck(4,9,0)` / Else = Spawns) bzw. 4:0x0A
(3010 main00 @0x0210e `If Ck(4,0x0A,0)` / Else = Spawns); beide setzt erst ROOM3060/3061 (`Set(4,9,1)` @0x0104e,
`Set(4,0x0A,1)` @0x01052), 4:0xC6 setzt 3000 selbst (@0x0166e, Marvin-Szene gesehen). Daher `RE15_SET_FLAG=4:9,4:0x0A,4:0xC6`
(= Stand nach 3060). Tuer 3020 id 0 (Rechteck x -27050..-26450, z -23450..-21550 -> Stage 2 Raum 01 = 3010 bei (-7400,-18400)).
Befehl (`abn/s3a_tuer3010`): `RE15_DEBUG_JUMP=3020@250 RE15_PLAYER_POS=-26600,-22500,2048,0`, Plan `RAUM 3020 / WARTE 10 /
BLICK 2048 / WARTE 5 / AKTION / RAUM 3010 / WARTE 1500`, `RE15_EXIT_AT=600#3010`, FRAMEDUMP 0-600/10. exit 0, 36 s
(`PC loaded room3020.rdt`, `PC loaded room3010.rdt`, `EXIT_AT: Bild 600 in Raum 3010`).
zf.log: F6 INIT Slots 1..6 (`grid=84` x5, `grid=85` Slot 4, alle `Wort=1/7/0 f10e=4002`), `F7 slot=2 WECKT d=2202 warte=1`,
`F8 slot=2 STEHT AUF`. state.log: Slot 2 F9 `ss2=3 mo=8 af=0` .. F88 `af=79`, F89 `ss1=1 g=00` (Gang), F132 Griff (`gr=1 ss1=3`),
HP 100 -> 60 (F200) -> 20 (F529); Slot 6 (d 4748..6165) und alle anderen liegen 600 Bilder (`ss1=7 ss2=1 g=84/85`);
0 Bild-Slots `ss1=8`. Bilder `abn/s3a_tuer3010/kontakt_0_150.png`, `zoom_3010.png`: F10 liegt der Zombie neben Leon flach,
F30..F70 richtet er sich auf, F90 steht er, F130 greift er; ein zweiter Zombie im Vordergrund liegt die ganze Zeit flach.

### 3.4 S3b — ROOM3000, Leon, ECHTER Tuerweg 3010 -> 3000, RE2-KI

Tuer 3010 id 0 (Rechteck x -25000..-23400, z 10620..12440 -> Stage 2 Raum 00 = 3000 bei (-6544,-17239), Blick 2096).
Befehl (`abn/s3b_tuer3000`): `RE15_SET_FLAG=4:9,4:0x0A,4:0xC6 RE15_DEBUG_JUMP=3010@250 RE15_PLAYER_POS=-24000,11500,2048,0`,
Plan `RAUM 3010 / WARTE 10 / BLICK 2048 / WARTE 5 / AKTION / RAUM 3000 / WARTE 40 / GEHE -8200 -15800 200 / WARTE 1500`,
`RE15_EXIT_AT=600#3000`. exit 0, 37 s (`PC loaded room3010.rdt`, `PC loaded room3000.rdt`, `EXIT_AT: Bild 600 in Raum 3000`).
zf.log: F6 INIT 3000-Slots 1..7 (`grid=84`, `Wort=1/7/0 f10e=4002`); `F80 slot=1 WECKT d=3998 warte=3`, `F83 slot=1 STEHT AUF`.
state.log: Slot 1 liegt bei d=4127 (Eintritt) bis Leon losgeht; F84 `mo=8 af=9`, F154 `af=79`, F155 `ss1=1 g=00`, F229 Griff
(`gr=1`), HP 100 -> 60; Slots 2..7 (d 4344..16508) liegen alle 600 Bilder (`ss1=7 ss2=1 g=84`); 0 Bild-Slots `ss1=8`.
Bild `abn/s3b_tuer3000/kontakt_0_230.png`: Polizisten-Zombie neben dem Streifenwagen liegt flach (d > 4000, bleibt liegen), der
Zombie links unten steht ab F100 auf und packt Leon (F210..F240).

### 3.5 S5 — ROOM3000 mit RE1.5-KI (`RE15_AI_FLAVOR=re15`), Tuerweg 3010 -> 3000

Wie S3b plus `RE15_AI_FLAVOR=re15` (`abn/s5_re15_3000`). zf.log: `F80 slot=1 WECKT(RE1.5) d=3998 warte=3`. state.log: alle 7
`st=1 ss1=0 ss2=0 g=84 mo=18 af=0` (P0); Slot 1 F80 `ss2=1`, F83 `ss2=2` (P2, Clip 0x12), F180 `af=97 ss2=3`, F181 `ss1=2 g=00`
(0x201), danach Annaeherung (Sub 0x13) und F466 Griff (`gr=1`), HP 100 -> 90 -> 75; die anderen 6 liegen 600 Bilder (`ss2=0`);
0 Bild-Slots `ss1=8`. Pose-Hoehen selbst gemessen (`tools/r41_zombiefresser/clip_hoehe.py 0x16 1 ...`): Clip 0x0c f0 -234
(Original-Liegepose), Clip 0x12 f0 -457 / f97 -2361 (Aufstehen aus dem Liegen, 98 Bilder), Fress-Schleifen 0x27/0x28 -680..-1197,
0x29 (-763 -> -2377) = Aufstehen aus dem KNIEN. Der Port zeigt mit RE1.5-KI vor dem Wecken Clip 0x12 Bild 0 (-457) statt
Clip 0x0c Bild 0 (-234) — beide liegend, Abweichung der Pose (Dossier OFFEN 3, s. Maengel).

### 3.6 S7 — ROOM3001, ELZA, ECHTER Tuerweg 3011 -> 3001, RE2-KI

Wie S3b, Elza ueber die Spielerwahl (`RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`), Plan `RAUM 3011 ...
AKTION / RAUM 3001 / ...`, `RE15_EXIT_AT=500#3001` (`abn/s7_elza3001`). exit 0, 49 s; `[pl] Spieler-Familie PL04`, `JUMP -> 301
DOWNLOAD AREA (ROOM3011)`, `PC loaded room3011.rdt`, `PC loaded room3001.rdt`. zf.log: 3001-Slots 1..7 INIT `grid=84 Wort=1/7/0
f10e=4002`, `F80 slot=1 WECKT d=3998 warte=3`, `F83 STEHT AUF`; state.log wie S3b (Clip 8 F84..F154, Gang F155, Griff F229, HP 100 -> 60,
die anderen 6 liegen); 0 Bild-Slots `ss1=8`.

### 3.7 S4 — Kanal 2000, Spieler DIREKT UEBER den Zombies (Dossier OFFEN 1), RE2-KI

`abn/s4_2000_ueber`: `RE15_DEBUG_JUMP=2000@250 RE15_PLAYER_POS=-21300,-10100,2048,1` (Oststeg, Band 1), kein Plan, `RE15_EXIT_AT=900#2000`.
zf.log: `F7 slot=3 WECKT d=3072 warte=10`, `F7 slot=4 WECKT d=1366 warte=5`, `F12/F17 STEHT AUF`. Slot 4: Clip 8, Gang F83, Hochziehen
F113..F290, oben F291 (-20884,-9301), Griff F637 (`gr=1`), HP 100 -> 60. Slot 3: steht F90 auf und schnappt dann unter dem Spieler
wiederholt nach oben (`ss1=14 mo=17` -> `ss1=0` -> `ss1=2` -> `ss1=14`, F170..F644, bei (-22068,-10100)), erst als der Spieler im
Griff verschoben wird, zieht er sich hoch (F647 `ss1=10`). Das ist die selbst gelesene RE2-DECISION[2] Block A (@0x80102064 `sltiu
0x7d0`, @0x80102070-7c Etagenvergleich +0x106, @0x80102090-94 `sw 0x0E01`), also RE2-Retail-Verhalten des RE2-Hirns, kein Rest des
Fixes. (Die Kamera des Debug-Sprungs zeigte in diesem Lauf nicht den Steg — nur Zustandsmessung.)

### 3.8 S6 — Nachbarn/Gegenprobe: echte Fresser bleiben Fresser

- `abn/s6_1140_re2` (RE2-KI, `RE15_DEBUG_JUMP=1140@250`, 150 Bilder): Slots 2..5 (`g=86` -> `g=00`) `st=1 ss1=8` (EXEC[8]) mit Clips
  18/19/20 durchgehend; Slot 1 (0x88-Liegender) `ss1=7 g=88` unveraendert; zf.log leer (kein Wecker-Eingriff).
- `abn/s6b_10e0_re15` (RE1.5-KI, `RE15_DEBUG_JUMP=10E0@250`, 150 Bilder): Slots 2..5 (`g=06` -> `g=00`) `ss1=12` (Sub 0xc
  Fressen) mit Clips 39/40 (0x27/0x28); Slot 1 (0x88) `ss1=0 g=88`; zf.log leer. Der neue RE1.5-Haken (Nibble 4..6 ohne Bit-0x80-Test)
  erreicht 0x06/0x86 nicht, weil der RE1.5-INIT sel 6 auf grid 0 setzt (enemy_ai_common.c:2110, `sb zero,9` @0x80100eb0).

### 3.9 S8 — Nebenbefund des Dossiers (OFFEN 2): LADEN in ROOM2000

`abn/s8_laden2000`: Karte per `probe_r41_zombiefresser_karte re15_card.mcr 2000 -20600 -10600 2048 1 leon`, dann `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_EXIT_AT=300#2000`: `[save] CONTINUE: resumed in room 2000`, state.log 300 Bilder OHNE einen
einzigen Zombie-Aktor. Bestaetigt (Boot-Pfad platform/pc/main.c, vom Diff nicht beruehrt -> vorbestehend, kein Rest dieses Fixes). Ob ein
Spielstand IN 2000/2001 im normalen Spiel entstehen kann (Speicherpunkt im Raum), habe ich NICHT belegt — der Weg ist nur mit dem
Kartenwerkzeug erzeugt. Als Folgeauftrag gemeldet (Hinweis H2), nicht als Mangel dieser Spur gewertet.

### 3.10 S9 — Modell-Hybrid (RE2-Hirn + RE1.5-Modelle, `RE15_AI_MODELS=re15`), Tuerweg 3010 -> 3000

Wie S3b plus `RE15_AI_MODELS=re15` (`abn/s9_hybrid_3000`, `RE15_EXIT_AT=400#3000`), exit 0, 32 s. zf.log `F80 slot=1 WECKT d=3998
warte=3`, `F83 STEHT AUF`; state.log wie S3b (Clip 8 F84..F154, Gang F155, Griff F229, HP 100 -> 60; Slot 7 d 4344..5608 liegt);
0 Bild-Slots `ss1=8`. Bild `abn/s9_hybrid_3000/kontakt_70_240.png`: liegender Polizist am Wagen bleibt liegen, der Geweckte kommt
von links unten und packt Leon (F230).


## 4. Tests (gezielt, KEINE volle Suite)

Der Diff beruehrt enemy_ai_common.c / enemy_ai_re2_zombie.c / player_common.c (Zombie-KI beider Varianten, Anim-Vorschub), daher:
- Spur selbst, verbose: `ctest -R "^unit_r41_zombiefresser$" -V` -> Passed 0.70 s. Ausgabe gelesen (keine Schein-Pruefung):
  `[A 2000 RE2] slot 3/4 ... grid 84 Wort 1/7/1 f10e 4002 Clip 23`, `[A 3010 RE2] slot 4 ... grid 85 ... Clip 22`, `[A ... RE1.5]
  ... Wort 1/0/0 Clip 18/19`, `[B 2000/3000 ...] 300 Bilder: 0 Bild-Slots nicht liegend, 0 schiessbar`, `[C 3000 RE2] ... steht F73
  angriff F76`, `[C 3010 RE2] ... steht F82 angriff F128`, `[C 2000-Steg RE2] ... steht F74 angriff F306`, `[C 2000 RE2] d0=1366 ...
  angriff F-1` (Spieler direkt ueber dem Kanal = OFFEN 1, vom Riegel bewusst nicht als Angriff gefordert), `[D] ... 4008 (d=4007)
  geweckt F-1 / 4007 (d=3998) geweckt F0/F9`, `[E] 48 Seeds: ... 15 verschiedene Wartezeiten`, `[F 1140 RE1.5/RE2] Fresser 4,
  faelschlich Liegewecker 0`, `== OK ==`.
- Unit-/Probe-Gruppen der beruehrten Module (Regex in `abn/rx_u.txt`: unit_damage, unit_enemy_ai*, unit_zombie_*, unit_anim_*,
  unit_room1140_*, unit_re2_*, unit_re2z_*, unit_re15_re2z_*, unit_r16_*, unit_r19_*, unit_r36_zombieki_*, unit_r36_gegnerzahl_*,
  unit_r36_schuss*, unit_r37_re2zombie_*, unit_r37_zombieaktiv_*, unit_r38_wasserzombie, unit_r38_griff_zombie*, unit_r38_npckante_*,
  unit_r39_elzazombies_*, unit_r39_treffer_*, unit_r39_gegner_*, unit_r41_zombiefresser, unit_r41_wasser20b0_*, unit_p3_restposten_pin,
  unit_aot_sce_census, probe_10d0_*, probe_re2z_*, ...): `100% tests passed, 0 tests failed out of 208` (39.9 s, -j 4).
- Integration der Zombie-Module/Raeume (Regex `abn/rx_i.txt`: r41_zombiefresser_*, r38_wasserzombie, r37_zombieaktiv, r37_re2zombie,
  r39_elzazombies_*, r36_gegnerzahl, r36_schuss, r36_elza_schuss, r39_gegner_*, r41_wasser20b0*), -j 3 unter Fremdlast (andere Agenten):
  `100% tests passed, 0 tests failed out of 24` (744.7 s, EXIT=0), keine Flatter-Wiederholung noetig;
  darunter integration_r41_zombiefresser_{tuer2000 41.5 s, elza2001 40.3 s, leon3000 19.7 s, leon3010 19.3 s, re15_3000 19.3 s}
  und integration_r38_wasserzombie (nachgezogenes Bildfenster) 124.3 s — alle Passed.
- Ein abgebrochener Vorversuch mit der ganzen Unit-Ebene (937, -j 4) wurde von mir nach 2 Tests gestoppt (Release-Skript-Tests
  ohne Bezug zum Diff, Laufzeit); seine Kindprozesse per PID beendet. Kein Befund daraus.


## 5. Vertrag (VERTRAG.md)

- Pfade (`git diff master --name-only`, 26 Dateien): analysis/befunde_runde41/I_*, re15_port/engine/src/{enemy_ai_common.c,
  enemy_ai_re2_zombie.c, player_common.c, zombiefresser_r41.c}, include/re15_zombiefresser_r41.h, tests/integration/
  {test_r38_wasserzombie.cmake, test_r41_zombiefresser.cmake}, tests/unit/{probe_p3_restposten.c, probe_r41_zombiefresser_karte.c,
  probes/r41_zombiefresser.cmake, test_anim_rate.c, test_r41_zombiefresser.c, test_room1140_combat.c}, tools/r41_zombiefresser/*.
  KEINE release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine Test-CMakeLists (Registrierung ueber
  tests/unit/probes/*.cmake, `file(GLOB ...)` tests/unit/CMakeLists.txt:4060). OK.
- Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine (Diff ohne flag_set/MSG-Id/k_port). OK (Spur I hat keine Zuteilung).
- Gemeinsame Dateien: enemy_ai_re2_zombie.c (I zugeteilt) 5 Haken je 1-3 Zeilen, markiert "Runde 41 Spur I". OK.
  ABWEICHUNG: enemy_ai_common.c ist laut VERTRAG §1.3 nur A (EM024) und L (Spinne) zugeteilt — Spur I setzt dort Include + 2-Zeilen-
  Haken im RE1.5-Grid-Zweig `case 5: case 6:` (Z. 4906-4907); player_common.c steht gar nicht in der Liste — 1 Zeile (`nib >= 7` ->
  `nib >= 4`, Z. 1508). Beide klein, markiert und sachlich noetig (RE1.5-Dispatcher bzw. Pin-Freigabe der geteilten
  Phasenmaschine; sonst ware die RE1.5-KI ungefixt). Kein Konflikt mit A erkennbar (A-Haken im EM024-Zweig, andere Stellen).
  -> Mangel V1 (Vertrag, klein; Orchestrator-Entscheid wie B5/E-E1).
- Nachgezogene Alt-Tests (Praemisse "Grid [4]/[5]/[6] = Fresser" widerlegt — selbst bestaetigt: [4]/[5]/[6] laden in STAGE1/2/3
  dieselben Zeilen Wecker/Liege-Phasenmaschine, der sel-6-Fresser verlaesst den Grid-Dispatch per `sb zero,9`):
  probe_p3_restposten.c [D], test_anim_rate.c PIN 4, test_room1140_combat.c (13b) — Aenderungen zitieren Adressen und messen
  weiter das Original-Verhalten; test_r38_wasserzombie.cmake Teil e nur Bildfenster 406 -> 560 (Testparameter, kein
  Spielverhalten; selbst gefahren gruen, s. 4). Kein "gruen gemacht" erkennbar.
- Tests je Punkt: unit_r41_zombiefresser (echte RDT/SCD/SCA + game_step, beide KI, 2000/2001/3000/3001/3010/3011, Ruhe/Wecken/
  Grenze 0xFA0/Wartezeit/Gegenprobe 1140) und 5 exe-Riegel (2 echte Tuerwege Leon/Elza, 3 Lade-Wege 3000/3010/RE1.5). Messend
  (Zustand + Angriff + nie `ss1=8`). Parallel-/Linux-fest: eigene Messkopie `re15_pc_r41i_<teil>_<zufall>`, eigener Laufordner,
  geerbte RE15_* geloescht, `${_exe_ext}`, `cmake_minimum_required`, `m` fuer Nicht-WIN32. OK.
- Nebenwirkungen: beide Szenarien gemessen (S1/S2, S3b/S7), Nachbarn/echte Fresser 1140/10E0 unveraendert (S6), Modell-Hybrid
  (`RE15_AI_MODELS=re15`, `abn/s9_hybrid_3000`) gleicher Ablauf (Wecken F80, Aufstehen, Griff F229, ferne liegen).

## 6. Urteil und Maengel

### 6.1 Urteil je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg |
|---|---|---|
| ROOM2000 (Leon) — "am Boden und fressen, anstatt aufzustehen wenn man ihnen naeher kommt" (Standard-KI RE2) | **erfuellt** | S1 echter Tuerweg 2070 -> 2000: liegen flach (EXEC[7] Clip 23), 0 Bilder Fressen, Wecken bei d 3949/3998 (< 4000), Aufstehen nach 10/5 Bildern (Clip 8), Gang, Hochziehen, Griff F371 |
| ROOM2001 (Elza), Standard-KI | **erfuellt** | S2 echter Tuerweg 2071 -> 2001 als Elza (PL04): identischer Ablauf, 0 Bilder Fressen |
| STAGE3 ROOM3000/3010 "etc." (3001/3011 = Elza-Varianten; Zensus: sonst keine 0x84/0x85 in STAGE3), Standard-KI | **erfuellt** | S3a Tuer 3020 -> 3010 (Wecken d 2202, Griff F132, Ferne liegen), S3b Tuer 3010 -> 3000 (Wecken d 3998, Griff F229, 6 Ferne liegen 600 Bilder), S7 Elza Tuer 3011 -> 3001 (gleich); 3011 per unit_r41_zombiefresser A/C |
| Dieselben Zombies mit RE1.5-KI (Optionsmenue) | **teilweise** | S5: liegen, wecken bei d 3998, Aufstehen mit dem Original-Clip 0x12 (98 Bilder), Angriff — ABER die Liegepose VOR dem Wecken ist Clip 0x12 Bild 0 statt der Original-Pose Clip 0x0c Bild 0 (M1) |
| Modell-Hybrid (`RE15_AI_MODELS=re15`) | erfuellt (Nebenprobe) | S9: gleicher Ablauf wie S3b, Bilder liegend/aufstehend |

Gates: RE-Gate **haelt** (2.: 19 Tabellenzeilen mit ueber 30 zitierten Adressen in STAGE1/2/3 + EMZ0 selbst disassembliert, alle wie zitiert; Original-RAM selbst
gelesen; keine Guess-Tells; der Fix erklaert den Befund: alter RE2-INIT-Zweig `sel == 6 || sel == 4 || sel == 5` -> EXEC[8]).
Tests **gruen** (4.: 1 + 208 + 24). Vertrag **haelt mit Abweichung** (V1).

### 6.2 Maengel (nummeriert, nachpruefbar)

**M1 (RE1.5-KI, Pose vor dem Wecken; = Dossier OFFEN 3, vom Bau-Agenten selbst offen gelassen).** Mit `RE15_AI_FLAVOR=re15` zeigen
alle 0x84/0x85-Zombies bis zum Wecken Clip 0x12 Bild 0 (state.log S5: `mo=18 af=0`, Keyframe 0x190), das Original zeigt die
INIT-Pose Clip 0x0c Bild 0 (Keyframe 0x12d; bei 0x85 Clip 0x0e). Selbst belegt: INIT setzt `+0x94 = 0x0c` @0x80100b74, `+0x95 = 0`
@0x80100c0c, `+0x8f = 0` @0x80100c1c und ruft `jal 0x8001f314` @0x80100c34; FUN_8001f314 nimmt fuer das Bildwort ohne Bit 0x8000
(Clip 0x0c f0 = 0x0000012d) FUN_8001f3bc, das bei `+0x8f == 0` den Keyframe DIREKT setzt (RE_15_Quellcode_V2/FUN_8001f3bc.c: Zweig `uVar5 == 0` kopiert Wurzel + Winkel und baut RotMatrix ohne Ueberblendung, sonst
FUN_80020510-Mischung mit `+0x8f -= 1`);
danach `+0x94 = 0x12` @0x80100c7c OHNE f314, und P0 @0x80103940-60 ruft kein f314 -> die Pose bleibt Clip 0x0c Bild 0 bis P2
(@0x801039a8, dann mit `+0x8f = 0xf` @0x80103998 ueberblendet). Sichtbar (Ansichtsblatt `abn/pose_vergleich_0c_12.png`, Typ 0x10
Bank 1): 0x0c f0 Arme abgespreizt (Breite 2030, Hoehe 572), 0x12 f0 Arme am Koerper, Schulter angehoben (Breite 1428, Hoehe 684).
Abhilfe: angezeigte Pose von +0x94 trennen (INIT-Pose 0x0c/0x0e Bild 0 halten, in P2 auf 0x12/0x13 mit +0x8f 0xf ueberblenden);
gilt ebenso fuer die vorbestehenden 0x87/0x88. Riegel: RE1.5-Lauf 2000/3000, Pose-Keyframe vor dem Wecken == 0x12d (0x84) bzw.
0x149 (0x85). Die Standard-KI (RE2) ist NICHT betroffen (RE2-Liegepose EXEC[7] Clip 23/22 @0x801037CC-E4).

**V1 (Vertrag, klein).** enemy_ai_common.c ist laut VERTRAG §1.3 nur A (EM024) und L (Spinne) zugeteilt, player_common.c steht
nicht in der Liste; Spur I hat dort Include + 2 Zeilen (enemy_ai_common.c:61, 4906-4907) bzw. 1 Zeile (player_common.c:1508)
geaendert. Klein, markiert "Runde 41 Spur I", sachlich noetig (RE1.5-Grid-Dispatcher 0x80117910[4..6] bzw. Pin-Freigabe der
geteilten Phasenmaschine). Orchestrator-Entscheid (Muster B5 / E-E1); bei der Integration mit A (enemy_ai_common.c) gegenlesen.

### 6.3 Hinweise (kein Mangel dieser Spur)

- **H1 (= OFFEN 1, RE2-Hirn):** Steht der Spieler direkt ueber dem Kanal (< 2000), schnappt ein aufgestandener Kanal-Zombie
  wiederholt nach oben (S4: Slot 3 F170..F644 `ss1=14 mo=17`), der naehere zieht sich hoch und greift (F637). Selbst gelesen:
  RE2-DECISION[2] Block A @0x8010205C-94 (EMZ0 = EMOVL10_S0, 0 abweichende Worte). RE2-Retail-Verhalten, im Dossier fuer den
  Nutzer benannt. Die RE1.5-KI kennt den Etagen-Schnapper nicht.
- **H2 (= OFFEN 2, vorbestehend):** LADEN eines Spielstands IN 2000/2001 (CONTINUE) -> keine Zombies (S8: 300 Bilder ohne
  Zombie-Aktor). Ursache laut Dossier Boot-Pfad platform/pc/main.c (vom Diff nicht beruehrt). Als eigener Folgeauftrag
  empfohlen; Tuerweg und Debug-Sprung sind korrekt (S1/S2/S4).
- **H3 (beobachtet, nicht Teil des Punkts):** Nach dem Loesen aus dem RE2-Griff fallen beide Kanal-Zombies (`ss1=5 g=80`) und
  rutschen dabei bis ~2 500 Einheiten weg (S1 Slot 4 F462 (-20573,-9304) -> F474 (-20784,-6781); S4 Slot 4 F727 -> F740
  (-20351,-13719)); vorher jeweils `ss1=9` (EXEC[9], laut Port-Kommentar Schub +0x144 = 400 @0x80103F60-64, -30 je Bild
  @0x80103FCC) — nicht im Diff, von mir nicht selbst disassembliert.

### 6.4 Gesamt

**bestanden = NEIN.** Der Kern des Nutzer-Punkts ist mit der Standard-KI in allen genannten Raeumen (2000, 2001, 3000, 3010, dazu
3001) ueber echte Tuerwege belegt erfuellt: die 0x84/0x85-Zombies liegen, fressen nie, stehen nach dem Original-Wecker
(d < 0xFA0 @0x8010382c, Wartezeit rand&0xf @0x80103858-60) auf und greifen an; ferne bleiben liegen; echte Fresser (1140/10E0)
bleiben Fresser. Offen bleiben M1 (RE1.5-KI-Liegepose, klein, belegt) und V1 (Vertrag, formal) — beide klein und gezielt
nachzubessern bzw. vom Orchestrator zu entscheiden.
