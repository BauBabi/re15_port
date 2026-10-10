# Runde 36 — Spur A "klettern": Abnahme 3 (unabhaengig)

Datum 2026-10-10 (Beginn 21:55). Baum `.claude/worktrees/r36_klettern`, Zweig `r36/klettern`, HEAD cfc2a1b6 (Basis
master 78a4a748). Geprueft wird der COMMITTETE Endstand nach "Nachbesserung 3"; am Code wird nichts geaendert. Der
Bericht wird fortlaufend geschrieben (Sitzungslimit). Eigene Skripte/Logs: Scratchpad `abn3_A/` (Pfade unten,
`<S>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`).

## 0. Gegenstand

Wortlaut (AUFTRAG.md Z. 31):
> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B.
> ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

Abnahme 2 (8b489051): Punkt erfuellt, RE-Gate NICHT — Mangel 1 (Beleg 3/4: @0x8002c6c4 als "Spieler-Fall" falsch
eingeordnet, Objekt-Band aendert sich im Original zur Laufzeit; @0x800421d0 fehlte), Mangel 2 (der Port hat den
Objekt-Fall FUN_8002c444 nicht; M5: die 5020-Kiste schwebt hinter der Kante). Orchestrator: Objekt-Fall byte-true
portieren. Behauptung Nachbesserung 3: portiert (`engine/src/objfall_r36.c`, `include/re15_objfall_r36.h`, Haken
game_step_common.c 3 Zeilen + scd_room_setup.c 1 Zeile), RE-Belege B1-B7, 5020/5021 an der exe gemessen, Pin
`unit_r36_objfall`, Mangel 1 berichtigt.

## 1. Diff gelesen

`git log --oneline master..HEAD`: 36 Commits; Nachbesserung 3 = c15b8662, 9ca5e56e, db63a494, e5f0d847, f017201a,
f5f790cc, cfc2a1b6. `git diff master --name-only` ohne analysis/: climb_common.c, game_step_common.c, granate_1150.c,
objfall_r36.c (neu, 590 Z.), scd_room_setup.c, scd_vm.c, sicherung_1150.c, re15_objfall_r36.h (neu), re15_scd.h,
platform/pc/main.c, tests/integration/test_r36_klettern.cmake, tests/unit/probe_r36_klettern_10f0.c,
probe_r36_klettern_zensus.c, probe_r36_objfall.c (neu), probes/r36_klettern.cmake, test_1090_climb_pin.c.

Code selbst gelesen (`git diff master -- re15_port`, objfall_r36.c und der Header vollstaendig):
- Verhalten: climb_common.c:102 `if ((flags & 0x101u) == 0u) continue; /* @0x8002d358-35c */` (unveraendert seit
  Abnahme 0-2). NEU: objfall_r36.c (Typ-Handler-Pass, Ausloeser + obj+0x88-Nullung, FUN_8002c444, FUN_8002d3d8,
  FUN_8002dacc mit libgte-Zwillingen, Wasser-Stempel der Objekte).
- Haken: game_step_common.c +4 Zeilen (Include; `re15_objfall_vor_schub` direkt vor `re15_prop_push_tick` im
  Normalzweig; `if (c->rdt_ok) re15_objfall_nach_pass` hinter `re15_actor_prop_pushout`, JEDES Bild);
  scd_room_setup.c +2 Zeilen (Include; `re15_objfall_install` am Ende des Installer-Blocks "Runde 36 Spur A").
- Sonst: Kommentare (scd_vm.c, re15_scd.h, granate_1150.c, sicherung_1150.c), Messhaken `RE15_TITLE_SHOT_CHAR`
  (main.c, env), Messschrieb `RE15_OBJFALL_LOG` (env, nur fprintf).
- Alle vom Port selbst installierten Props sind Typ 0 (`obj_type =` in dokumente_r34.c:80, granate_1150.c:83,
  inhalt_r35.c:63, irons_tisch_1150.c:62, sicherung_1150.c:115) — der neue Fall kann nur RDT-Typ-4-Objekte treffen.

## 2. Bau und Tests

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
  to do.` / `=== LOCAL-BUILD-OK (build)` (`<S>/abn3_A/configure.log`, `build.log`).
- Konsistenz: `git status` sauber; `re15_pc.exe` md5 `e468162bc5f95bdd8c506c561074bcda` (= vom Bau-Agenten genannte
  exe), mtime 21:13:54; alle Objektdateien neuer als ihre Quellen (objfall_r36.c 1791659624 < .obj 1791659633 <
  exe 1791659634; game_step_common.c, scd_room_setup.c, climb_common.c, main.c, Sonden ebenso).
- `ctest -R "^(unit_|probe_|prop_)" -j4 --timeout 600 --output-on-failure` (weil game_step_common.c und
  scd_room_setup.c jeden Raum beruehren) -> **`100% tests passed, 0 tests failed out of 521`** (721 s, Host unter
  Last paralleler Spuren; `<S>/abn3_A/ctest_unit_alle.log`), darin unit_r36_objfall, unit_r36_klettern_zensus,
  unit_r36_klettern_10f0_endlage/_startlage, unit_1090_* (11), unit_r35_cut10f0_* (7), unit_r22_10f0_figur.

## 3. RE-Gate — selbst disassembliert (re15_disasm.py auf info/Re1.5/PSX.EXE, md5 b55fdaa5..., Auslieferungsstand)

| # | zitiert | selbst gelesen | Urteil |
|---|---|---|---|
| 1 | Ausloeser @0x8002c0fc-c14c, obj+0x88 = 0 @0x8002c154 | `8002c0fc lbu v1,8(s2)` / `8002c104 ori v0,zero,0x4` / `8002c108 bne v1,v0,0x8002c150` / `8002c110 lbu v0,4(s2)` / `8002c118 bne v0,zero` / `8002c120 addiu a0,s2,52` / `8002c124 lbu a2,130(s2)` / `8002c128 jal 0x8003b7f0` / `8002c12c ori a1,zero,0x1c2` / `8002c130 andi v0,v0,0x2` / `8002c134 beq` / `8002c13c jal 0x8002d3d8` / `8002c148 sb v0(=1),4(s2)` / `8002c14c sb zero,5(s2)` / `8002c150 bne s3,zero,0x8002bdc8` / `8002c154 sh zero,136(s2)`; Bytes identisch zum Dossier B2 | stimmt |
| 2 | Typ-Handler @0x8002bde0-be08, Tabelle @0x80073c70[4] = FUN_8002c40c, `jal 0x8002c444` @0x8002c42c | `8002bde0 lbu v0,8(s2)` / `beq v0,zero` / `8002bdf4 addiu at,at,15472` (0x80073c70) / `jalr v0` / `addu a0,s2,zero`; Tabelle [1] 0x8002c3f4 [2] 0x8002c3fc [3] 0x8002c404 [4] 0x8002c40c [5] 0x8002cab4; FUN_8002c40c: `lbu v1,4(a0)` / `beq v1,zero` / `bne v1,1` / `8002c42c jal 0x8002c444` | stimmt |
| 3 | Reihenfolge Haupt-Loop | `8001ce0c jal 0x80031c44` (Spieler) / `8001ce14 jal 0x8002bd44` (Objekt-Pass) / `8001ce1c jal 0x800436a8` (AUTO-Scan) | stimmt |
| 4 | FUN_8002c444 komplett (Init 0xd/0xd/8, Kippen, 160, Bild 13, Band, Spritzer, Landung, Zustand 2/3) | Zustand 0 @0x8002c49c-4f8: v0 = 0xd (Delay `ori v0,zero,0xd` @0x8002c474) -> `sh` 0x8008f680/67c, `ori v0,zero,0x8` -> 0x8008f688, Nullen 690/684/698/69a/69c/68c, `sb zero` 0x8008f6a0, obj+5 = 1, obj+6 = 0, faellt nach @0x8002c4fc durch. Zustand 1: x/z += (s16)67c * richt (`mult`/`mflo`, `sw 52/60`); `slti 1024` @0x8002c558, `lhu` 684, `slti 1025` @0x8002c57c, Korrektur `addiu v0,a1,1024` / `subu v0,v0,v1` @0x8002c58c-590; 698 = 684*(-richt_z), 69c = richt_x*684; 684 += 688, 67c += 680 (`lhu`/`addu`/`sh` @0x8002c5e0-614); obj+6++ / `sltiu 0xd` @0x8002c630 -> @0x8002c92c (0x55555556-Idiom, y += (s16)(67c/3), `j 0x8002ca8c`); sonst 680 = 0, y += 68c, `addiu v1,v1,160` @0x8002c654; bei 13 `jal 0x8001c6e8` (a1 0, a2 8, a3 0x200) @0x8002c674-680, 0x91a2b3c5 / `sra 10` / Vorzeichen -> `sb v1,130(s1)` @0x8002c6c4; Spritzer nur 6a0 == 0 UND (s16)+136 != 0 UND +136 < y (`slt v0,v1,v0` @0x8002c6f4), dann 6a0 = 1, `jal 0x80045024` a0 = 0x02080000, 12x `jal 0x80019700` a0 = 0x06022000 (Aufrufe 1/2/12 gelesen: a1 0x200 x+900 z+900 / 0x400 z+225 / 0xa00 z-900, a2 = 0x80072d4c, P.y = obj.y `sw v0,20(sp)` @0x8002c72c); Landung `slt v0,v1,v0` / `sw v1,56(s1)` / `sb 2,5(s1)` @0x8002c914-928; Zustand 2: +136 == 0 -> `jal 0x80045024` a0 = 0x02060000, y -= 30, x/z += 20*richt, 698 = -(30*richt_z), 69c = 30*richt_x, obj+5++; Zustand 3: y += 30, x/z += 20*richt, 698 = 30*richt_z, 69c = -(30*richt_x), `sb zero,4(s1)` @0x8002ca88; immer `jal 0x8002dacc` (a1 = 0x8008f698) @0x8002ca8c-98 | stimmt Zeile fuer Zeile mit objfall_r36.c:305-425 |
| 5 | FUN_8002d3d8 Fallrichtung | dx16 = (u16)obj.x - (u16)playerX (`sll 16`/`sra 16`), `bgez`/`subu` Betrag, `slti v0,v0,900` @0x8002d410 -> richt_z = (dz16 >> 31) \| 1, richt_x = 0 (`sh zero` 0x800b526c), sonst richt_x = (dx16 >> 31) \| 1, richt_z = 0; @0x8002d454-468 obj+0x68/+0x6c -> 0x800b282c/2838 | stimmt (objfall_r36.c:246-259) |
| 6 | FUN_8002dacc | RotMatrix(delta) @0x8002db00, MulMatrix2(R, obj+32) @0x8002db0c, m02/m12/m22 = `lh 36/42/48`, Summe `>> 12` mit `addiu 4095` (`bgez`), `div (m02<<12)/len`, `jal 0x80065de0` (a0 = -m12, a1 = m22) -> `sh 104`, (y*y)>>12, `ori a0,0x1000` / `jal 0x80066120` (SquareRoot12), `jal 0x80065de0` (y, sqrt) -> `sh 106`, `sh zero,108`, RotMatrix(obj+104), `jal 0x80065d90` (Transpose), v = `lhu 32/38/44`, `jal 0x800661c0` (ApplyMatrix), `jal 0x80065de0` (r.y, r.x) -> `sh 108` | stimmt (objfall_r36.c:208-232) |
| 7 | Wasser-Stempel: AUTO-Scan @0x80043768-b4, Tabelle @0x8007469c[8] = 0x8004330c | `80043788 sb zero,0(at)` (obj+0x0B, at = 0x800b3fa3 + 148 i) / `8004378c jal 0x80042bac` a1 = 4, a2 = 0, Schleife ueber Raumkopf+2 (Objektzahl), KEIN Aktiv-Test; Tabelle [8] = 0x8004330c: `lw v1,0x800ac784` / `lhu v0,0(a0)` / `8004331c sh v0,136(v1)` | stimmt |
| 8 | Objekt-Sperre FUN_8003b558 (Freiraum-Zensus 20A0/11A0) | Band-Nibble `sll 16`/`sra 28` gegen +0x82 @0x8003b634-644, u0 = `sll 16`/`sra 24` & Klasse @0x8003b64c-660, Objekt-Flag & 8 -> ueberspringen @0x8003b670, r = box.hx `lhu t0,6(a3)` @0x8003b68c (hx == hz -> direkt @0x8003b738), x: `(u32)(X - (x0 - r)) < w + 2r` `sltu` @0x8003b764, z analog `sltu` @0x8003b790 -> halboffen [x0-r, x0+w+r) | stimmt (Grundlage der 20A0-Taschen-Arithmetik) |

Zusatz (Schub waehrend des Falls): im Objekt-Pass zwischen @0x8002be0c und @0x8002c0ec liest KEINE Instruktion obj+4
(Scan aller `(s2)`-Zugriffe: +8, +130, +140, +0, +52, +60, kein +4) — der Schub laeuft im Original auch fuer ein
fallendes Objekt weiter. Das erklaert die +48 der Endlage am echten Weg (unten 4.2).

**Eigene Nachrechnung des Falls aus den Instruktionen (ohne Port-Code):** Vorschub je Bild 13k (k = 1..13), danach 182;
Absinken Bild 1..12 = 26/3, 39/3, ..., 169/3 (gekappt) = 8+13+17+21+26+30+34+39+43+47+52+56 = **386** -> y nach Bild 12
= -7200 + 386 = **-6814**; Bild 13 y += 0, Boden -> Band; ab Bild 14 y += 160, 320, ... -> Bild 22: -1054 + 1440 = 386 > 0
-> **Landung bei obj+6 = 22**, dann Zustand 2 und 3 -> **24 Bilder**; Kippschritte 0, 8, ..., 96 (Bild 1-13), 104, 112,
120, dann 128 -> 64 korrigiert (690 = 1088) -> Summe **1024**. Deckt sich mit dem Log des Bau-Agenten (F798 y -6814,
Landung Bild 22) und den Riegeln von unit_r36_objfall (13/26, 8/13, 0/8, Landung 22, 24 Bilder).

**Signalwoerter** (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig|geschaetzt`) in
den hinzugefuegten re15_port-Zeilen: (Ergebnis Abschnitt 6).

## 4. Messung des Nutzer-Punkts an der exe (eigene Laeufe)

Laufskript `<S>/abn3_A/skripte/lauf.sh`: Messkopie `re15_pc_abn3a_<lauf>.exe` (md5 je Lauf gegen die gebaute exe
geprueft, Kopie danach geloescht), Arbeitsordner `<S>/abn3_A/laeufe/<lauf>/`. Basis-Umgebung aller Laeufe:
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt`.

### 4.1 M1 — ECHTER WEG Leon: ROOM10D0 -> Tuer -> ROOM10F0 -> Tritt schieben -> klettern -> oben -> Tuer -> ROOM1090 (NEUE exe)

`RE15_DEBUG_JUMP=10d0@250 RE15_SET_FLAG=3:50,9:71 RE15_PLAYER_POS=1700,-7000,2048 RE15_INPUT_SCRIPT=W11,A0.1,W1,
<Tuerweg integration B>,<Schluss integration> RE15_EXIT_AT=2500#1090` (exit 0, 169 s, md5 e468162b...):
```
[debug-menu] JUMP -> 10d 2F CORRIDOR (ROOM10D0) spawn=(1700,0,-7000) / [tuer] Sequenz Archiv 2 DOOR13 ... / [room] PC loaded room10f0.rdt
T583  room=10F0 pos=(8400,0,-350) rot=2048 band=0 ... t4=3 t4pos=(-6050,4950) t4fl=0x0001     <- durch die Tuer, Tritt in der Startlage
T1504 room=10F0 pad=0010 pos=(-7125,0,4960) ... push=5                                       <- Schieben +X
T2012 room=10F0 pad=0010 pos=(-5013,0,-58) ... push=5 t4pos=(-4900,-1108)                    <- Schieben -Z bis an die Wand
T2051 room=10F0 pad=8000 pos=(-5013,0,-58) band=0 sub=9 kind=0x81 ph=0 obj=3                  <- KLETTERN
T2103 room=10F0 pos=(-5013,-1800,-1206) rot=1024 band=1 sub=0 obj=3 stand=3                   <- oben
T2156 room=10F0 pad=8000 pos=(-5013,-1800,-1332) band=1 stand=3 -> T2157 room=1090 pos=(-13600,-9000,1300) band=5
[tuer] Sequenz Archiv 2 DOOR1E ... / [room] PC loaded room1090.rdt / [flow] EXIT_AT: Bild 2500 in Raum 1090 erreicht
```
Bildgleich zu Abnahme 2 M1 (alte exe 74a17d61): der neue Objekt-Pass aendert am Kletterweg nichts. objfall.txt: in
10F0 und 1090 **kein AUSLOESER** (314 Zeilen), nur `F1360 room=10F0 p=3 KERBE m0b=10` (Tritt in Slot 10 = Schiebezone);
in 1090 p0 (Band 1, per SCD-Member_set auf x -9836, `[prop-member F6] prop[0] obj=0x00 m=0 -> val=-9836`) und p1 (Band
5, (-5960,-9000,-15482)) bis zum Laufende unveraendert. (Die Bewegung Leons in 1090 ohne Eingabe ist ein Mess-Artefakt:
der Bildzaehler beginnt je Raum neu, das Skript auf der Spielbild-Achse laeuft ab `[input-script] Tick 330 -> F331`
erneut — kein Spielverhalten.)

### 4.2 M2 — Elza ROOM10F1, EIGENE Variante: Tritt nur teilweise geschoben, in der Zwischenlage geklettert

`RE15_TITLE_SHOT_CHAR=1 RE15_GOTO_ROOM=10F1 RE15_SET_FLAG=9:71 RE15_INPUT_SCRIPT=<Tuerweg integration B>,U2.6,W1,L0.37,
W0.3,U0.5,W0.3,R0.37,W0.3,U0.55,W0.3,R0.37,W0.5,U6.0,W1,A0.1,W3 RE15_EXIT_AT=1250#10f1` (exit 0, 44 s; -Z-Schub U6.0
statt U10.5 -> der Tritt bleibt AUSSERHALB der Schiebezone, z 1667):
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1) / [goto] queued room change -> ROOM10F1 at F30 / [room] PC loaded room10f1.rdt
T971  room=10F1 pad=0010 pos=(-4897,0,2717) ... push=5                                       <- letzter Schubschritt -Z
T1002 room=10F1 pad=8000 pos=(-4897,0,2717) band=0 sub=9 kind=0x81 obj=3 ... t4pos=(-4892,1667)   <- KLETTERN (Zwischenlage)
T1054 room=10F1 pos=(-4897,-1800,1569) rot=1024 band=1 sub=0 obj=3 stand=3                    <- oben
T1281 (Laufende) unveraendert oben, stand=3; objfall.txt: 0 Ausloeser
```
-> Elza klettert auch in einer Lage, die weder Start- noch Endlage des Raumskripts ist.

### 4.3 Integrationsriegel mit der neuen exe

`ctest -R "^integration_r36_klettern$" --timeout 1500 -V` -> **Passed 516,77 s**: `[A_leon_10f0] start=1 schieben=1
klettern=1 oben=1 zone=1 ziel=1`, `[B_elza_10f1] ...ziel=1`, `[C_leon_szene] ...ziel=1` (`<S>/abn3_A/ctest_integration_r36.log`).
Arbeitsordner `build/tests/integration/r36_klettern_wd/lauf_ad14d1d9_B_elza_10f1` selbst gelesen: `[pl] Spieler-Familie
PL04`, `T1137 sub=9 kind=0x81 obj=3`, `T1189 pos=(-5015,-1800,-1174) band=1 sub=0 stand=3`, `T1243 room=1091`,
`EXIT_AT: Bild 1320 in Raum 1091`.
Dazu die schnellen C-Integrationstests, die Raumladen/Tueren/alle Raeume durchlaufen (`integration_(room_transition|
cross_stage|headless_run|room_probe_sweep|rvd_coverage|map_uebergang|map_raum_live|door_traversal|door_lock|flag_census|
item_name_census|fx_region_cull|keydoor|keypad|pri_masken)`) -> **15/15 bestanden** (`<S>/abn3_A/ctest_integration_c.log`).

### 4.4 M3 — ROOM5020: der portierte Objekt-Fall an der exe (Kette des Bau-Agenten nachgefahren, mit Bildserie)

`RE15_DEBUG_JUMP=5020@250 RE15_PLAYER_POS=-11000,-21800,1024,4 RE15_INPUT_SCRIPT=<Eingabe l5020a des Dossiers>
RE15_FRAMEDUMP=776-2696/8:f_ RE15_EXIT_AT=2700#5020` (exit 0, 102 s, 241 Bilder):
```
F785  AUSLOESER wort=0x4002 band=4 pos=(-8078,-7200,-25332) spieler=(-9428,-25807) richt=(1,0)
F786  zust 0->1 n=1 pos=(-8065,-7192,-25332)   F787 n=2 (-8006,-7179) rot z 8   F788 n=3 (-7952,-7162) rot z 24
F798  BODEN FUN_8001c6e8=0 band 4 -> 0 pos=(-6847,-6814,-25332)     <- y -6814 = meine Nachrechnung (Abschnitt 3)
F807  LANDUNG y=0 band=0 pos=(-5209,-25332)   F808 AUFSETZEN se=snd0:6   F809 zust 3 -> Endlage (-5169,0,-25332) rot (0,0,1029)
F1712 FLAG (4,203)=1 / KERBE m0b=4 pos=(-4393,0,-25332) band=0           <- Latch sub01 (Slot 4, Floor 0)
T2138 room=5020 pad=8000 pos=(-4670,0,-25212) band=0 sub=9 kind=0x81 obj=0 t4pos=(-3320,-25332)   <- KLETTERN auf die gefallene Kiste
T2190 room=5020 pos=(-3522,-1800,-25212) band=1 sub=0 obj=0 stand=0                              <- oben (bis Laufende T2951)
```
Bildgleich zum Messschrieb des Bau-Agenten (`A_belege/nb3_5020_echter_weg.txt`). Die +48 in x (Bild 2/3: -8006/-7952 statt
-8039/-7991) sind der Spieler-Schub, der im Original fuer ein fallendes Objekt weiterlaeuft (Abschnitt 3, Zusatz).
Bildbogen `<S>/abn3_A/blatt_5020_fall.png` (F776-F816: die aktive Kamera zeigt den Fall selbst nicht; ab F816 liegt die
Kiste am Boden vor der Ostwand) und `<S>/abn3_A/blatt_5020_klettern.png` (F1704 Schub, F1920 Aufstieg, F1936/F2000 Leon
steht oben auf der gefallenen Kiste).

**V1 — Gegenprobe ohne Objekt-Fall, GLEICHE Eingabe**: `re15_pc_r36a.exe` des Baums (md5 7e2da559..., 09:52, mit
Kletter-Fix und t4pos-Trace, ohne objfall) als Messkopie `re15_pc_abn3a_gegen.exe`, `RE15_EXIT_AT=1200#5020` (exit 0, 50 s):
```
T1035 t4pos=(-8127,-25332)  T1036 t4pos=(-8078,-25332)   <- dasselbe Bild/dieselbe Lage, an der die neue exe ausloest (F785 = T1036)
T1045 t4pos=(-7928,-25332)  T1100..T1440 t4pos=(-6904,-25332), Spieler geht die Treppe hinunter   <- Kiste bleibt hinter der Kante (x -7175) stehen
```
-> Der Port-Fall erklaert den Befund M5 der Abnahme 2: ohne ihn faehrt die Kiste ueber die Kante und bleibt in der Luft,
mit ihm loest sie an der Grenze der um 450 erweiterten Absprungzelle aus.

### 4.5 M4 — ROOM5020, EIGENE Variante (Kiste auf anderer z-Linie an die Kante)

`... RE15_INPUT_SCRIPT=W8,U2.0,W0.5,R0.37,W0.3,U0.9,W0.3,L0.37,W0.3,U0.83,W0.3,L0.37,W0.5,U8,W3 RE15_EXIT_AT=1300#5020`
(exit 0, 53 s): Kiste -Z nur bis z -24196, dann +X: `F495 p0 (-10687,-7200,-24196)` ... `F615..F1275 p0 (-8590,-7200,-24196)
b4` — sie stoppt 465 vor der Ausloesegrenze -8125, waehrend der Schub weiterlaeuft (gemessen), **kein Ausloeser, keine
schwebende Kiste** (Kistenrand -7690 < Kante -7675). Gefolgert, nicht einzeln an der SCA nachgelesen: die Stopplage passt
zu einer Sperrzelle (FUN_8003b558 Klasse 2) mit x0 = -7675 (Mitte >= -8575 gesperrt, naechster Schritt ~+26). Die
Absprungzone ist auf dieser z-Linie nicht erreichbar — passt zum Freiraum-Zensus des Bau-Agenten (erste Ausloeser-Lage
(-8125,-25100), also Suedstreifen).
(Vorlauf `v5020_kurz` mit falsch geschaetzter Gehweite: Leon lief suedlich an der Kiste vorbei, ebenfalls kein Ausloeser.)

### 4.6 Sonde `probe_r36_objfall pin` selbst gefahren (`<S>/abn3_A/probe_objfall_pin.txt`, exit 0, PASS)

5020/5021: Grenze x -8126 kein Ausloeser / -8125 Ausloeser, Bild 1/2 d 13/26 dy 8/13 w 0/8, Band nach Bild 13 = 0, Landung
Bild 22 y 0, 24 Bilder, snd0:6 x1, Latch (4,203) = 1, Klettern oben y -1800 Band 1 Stand 0. 20A0/20A1 (gesetzte Lage):
Tasche -> Band 2 (y -3600), Laufsteg -> Band 1 (Wasser gestempelt, kein Ton), Kanal-Kante -> Band 0 mit snd0:8 + 36
ESP-Plaetzen, Latch (3,204) = 1. 10F0/10F1: 16641 Rasterpunkte, 0 Ausloeser. Freiraum: 5020 2 Ausloeser-Zellen, 20A0/11A0
0 (unerreichbar), 3050/1090/2050 erreichbar (Diagnose). **Diagnose-Zeilen 3050/3051 und 2050/2051 (kuenstliche
Schubrichtung): "Landung" OBERHALB — 3050 obj 1 Band 4 -> 6 (y -7200 -> -10800), 2050 obj 6 Band 3 -> 5 (y -5400 ->
-9000)**; dazu Abschnitt 5.

### 4.7 V2 — Kletter-Fix gegen ohne Fix, IDENTISCHE Eingabe (Endlage 10F0, Anlauf von Westen, Tuer nach 1090)

`RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=3:102,9:71 RE15_PLAYER_POS=-5925,-850,0 RE15_INPUT_SCRIPT=W8,A0.1,W3,R0.37,W0.5,U0.5,
W0.3,A0.1,W4`:
```
neue exe (e468162b):           T492 pad=8000 pos=(-5925,0,-850) band=0 sub=9 kind=0x81 obj=3   T544 pos=(-4777,-1800,-850) band=1 stand=3
                               T636 room=1090 / [tuer] Sequenz Archiv 2 DOOR1E / EXIT_AT: Bild 60 in Raum 1090 (exit 0, 36 s)
master-exe ohne Fix (2fd81d20, Hauptbaum, nur als Messkopie im Spurbaum):
                               T492 pad=8000 pos=(-5925,0,-850) band=0 sub=0 kind=0x00 obj=-1  — 0 Zeilen sub=9 (exit 0)
```

## 5. Nebenwirkungen des neuen Objekt-Passes (jedes Bild, jeder Raum)

- **Kletterweg 10F0 -> 1090 (Leon) und 10F1 (Elza):** kein Ausloeser, Bildgleichheit zu Abnahme 2 (4.1-4.3, 4.7).
- **ROOM1090 (Zielraum):** obj 1 (Band 5) ist neu fallfaehig, bleibt beim Betreten unberuehrt (4.1); obj 0 (Band 1) hat
  keine Absprungzelle (Zensus `drop-cells-on-band=0`), seine Verschiebung auf x -9836 ist der SCD-Member_set, kein Fall.
- **ROOM3050 (neu fallfaehig, an der exe bisher nicht gefahren) — SELBST mit echtem Schub gefahren:**
  `RE15_DEBUG_JUMP=3050@250 RE15_PLAYER_POS=-16500,-20800,2048,4 RE15_INPUT_SCRIPT=W8,U10.3,W0.5,L0.37,W0.3,U0.83,W0.3,R0.37,
  W0.3,U0.77,W0.3,R0.37,W0.5,U5,W3 RE15_EXIT_AT=1200#3050` (Lauf `r3050_c`, exit 0; Vorlaeufe `r3050_a/_b`: Kiste auf z -20800
  bis x -23230 geschoben, dann +Z bis z -18951 = Sperrzelle z0 -18050 oestlich der Absprungzelle, kein Ausloeser):
  ```
  F570 p1 (-24072,-7200,-20800) b4      <- -X geschoben (Startlage @0x01F1A (-18000,-7200,-20800))
  F831 AUSLOESER wort=0x4002 band=4 pos=(-24072,-7200,-18488) spieler=(-24298,-19838) richt=(0,1)   <- Spieler suedlich, Schub +Z
  F844 BODEN FUN_8001c6e8=0 band 4 -> 0 pos=(-24072,-6814,-17239)
  F853 LANDUNG y=0 band=0 pos=(-24072,-15601)   F854 AUFSETZEN se=snd0:6 (Satz 6 in 3050 leer -> stumm)
  F870..F1200 p1 (-24072,0,-15561) b0 r(-1029,0,0)
  ```
  -> Mit ECHTER Schubrichtung faellt die 3050-Kiste regulaer auf den Boden (Band 0). Der "Fall nach OBEN" der Sonde
  (4.6, 3050 Band 4 -> 6) ist ein Artefakt der KUENSTLICHEN Richtung (Spieler auf die Startlage gesetzt -> |dx| = 5875
  -> richt_x = -1 @0x8002d410-450); er folgt aus FUN_8001c6e8, das die Baender von OBEN nach unten absucht (s3 = a2<<12
  @0x8001c744, `addiu s3,s3,-4096` @0x8001c7a4, `bne s3,zero` @0x8001c918 — selbst gelesen, Port-Zwilling identisch) und der
  Landung ohne Richtungstest (@0x8002c904-928). Die Kamera der aktiven Einstellung zeigt den Fall nicht (Bildbogen
  `<S>/abn3_A/blatt_3050_fall.png`), Beleg ist der Messschrieb.
- **ROOM2050 (neu fallfaehig): an der exe NICHT fahrbar** — Lauf `r2050_a` (`RE15_DEBUG_JUMP=2050@250
  RE15_PLAYER_POS=-3100,-9348,0,3`, Schub +X): der Gegner EM25 des Raums toetet Leon nach ~700 Bildern (mo 105/7), das
  Spiel startet neu ueber ROOM1240 (`[boot] room RDT: STAGE1/ROOM1240.RDT`), Kiste nur bis x -782. Kein Messhaken schaltet
  Gegner ab (`grep getenv RE15_*ENEM*/NOAI/GOD` -> nur RE15_ENEMY_DBG/DIAG). Gefolgert, NICHT gemessen: die einzige
  erreichbare Ausloeser-Elementarzelle beginnt bei (7550,-12050), Flaeche 35000; liegt sie an der Nordkante der um 450
  erweiterten Zelle (z < -11700), kommt die Kiste von Norden (Schub -Z, Fall nach Sueden) statt in der Diagnose-Richtung +X.
- **Port-Props:** alle vom Port selbst installierten Props sind Typ 0 (Abschnitt 1) — der Fall kann sie nicht erfassen.
- **Elza, echter Tuerdurchgang 10D1 -> 10F1:** nicht gefahren. Versuch `e10d1_weg` (GOTO 10D1, Spawn (7650,0,11400)): Wand
  bei x 5573 und z 4816, die Aktionstaste oeffnete Slot 14 -> ROOM10E1. Die Tuer 10D1 -> 10F1 ist Door_aot_set Slot 0/2
  @0x01174 (Rechteck x 300..1300, z -8050..-6050, Ziel (8400,0,-350) Gierung 2048) — genau die Spawnlage, die
  `RE15_GOTO_ROOM=10F1` in M2/Integration B benutzt; der Raum 10F1 selbst ist mit Elza (PL04) gefahren.

## 6. Vertrag, Dateien, Gates

- `git diff master --name-only` (ohne analysis/): nur engine/src/{climb_common,game_step_common,granate_1150,objfall_r36,
  scd_room_setup,scd_vm,sicherung_1150}.c, include/{re15_objfall_r36,re15_scd}.h, platform/pc/main.c, tests/integration/
  test_r36_klettern.cmake, tests/unit/{probe_r36_klettern_10f0,probe_r36_klettern_zensus,probe_r36_objfall,
  test_1090_climb_pin}.c, tests/unit/probes/r36_klettern.cmake. KEINE release/, platform/android/, shared_assets/PSX/,
  tests/*/CMakeLists.txt, RELEASE_NOTES. Neue Logik in NEUER Datei `objfall_r36.c` + `re15_objfall_r36.h` (VERTRAG 1.4).
- Gemeinsame Dateien nur kleine Haken: game_step_common.c +4 Zeilen (Include + 2 Aufrufe + Bedingung), scd_room_setup.c
  +2 Zeilen (Include + Installer-Zeile am Ende des Blocks mit "Runde 36 Spur A"), climb_common.c 1 Logikzeile + env-Trace,
  main.c 4 Zeilen (Messhaken `RE15_TITLE_SHOT_CHAR`).
- Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine (`git diff master -- re15_port/engine re15_port/include
  re15_port/platform | grep '^+' | grep -E 're15_game_flag_set|re15_msg_install|scd_event_fire'` -> leer;
  `re15_game_flag_get` nur im env-gegateten Messschrieb).
- Env-Haken im Engine-Diff: `RE15_OBJFALL_LOG` (nur fprintf), `RE15_TITLE_SHOT_CHAR` (Messhaken) — kein Env-Schalter
  als Abschluss.
- **Signalwoerter** (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig|geschaetzt|ungefaehr`)
  in den hinzugefuegten re15_port-Zeilen: **keine**.
- **PORT-WAHL** (Fallzustand beim Raumaufbau 0, objfall_r36.c:430-445) gekennzeichnet und begruendet; selbst geprueft:
  Installer LAB_80040914 schreibt nur +0x08/+0x09/+0x82/+0x00/+0x0C/+0x34/+0x38/+0x3C/+0x68/+0x6A/+0x6C/+0x74/+0x78
  (Stores @0x8004095c-80040aa0), der Raumlader nur +0x00 (`sw zero,0(at)` @0x8003eabc, Schleife `sltiu 0x20` @0x8003eac4)
  -> obj+4/+5/+6/+0x88 ueberleben im Original den Raumwechsel; ein laufender Fall waehrend eines Tuerdurchgangs ist kein
  Spielweg.
- **Mangel 1 der Abnahme 2 berichtigt:** Dossier Beleg 3 (Ueberschrift "⛔ FALSCH"), Beleg 4 ("⛔ BERICHTIGT"), OFFEN
  Stand NB2 ("⛔ BERICHTIGT"), B7-Tabelle (@0x8002c6c4 = Objekt-Fall, @0x800421d0 Sce_em_set eingeordnet), eeca9a89 als
  ueberholt vermerkt (A_klettern.md:560), Sondenkopf `probe_r36_klettern_zensus.c:54-63`, `probes/r36_klettern.cmake:36-41`.
  Restvorkommen von "Spieler-Fall"/"nur der Installer" stehen nur noch im ⛔-Kontext.
- **Mangel 2 der Abnahme 2 behoben:** Objekt-Fall portiert und an der exe gemessen (4.4, 4.5, 5), Gegenprobe V1.
- **Tests je Punkt vorhanden und messend:** integration_r36_klettern (echte exe, Leon/Elza/Szene), unit_r36_klettern_*
  (VM, Gate), unit_r36_objfall (Fall-Mechanik mit exakten Grenzen, Gegenprobe 10F0/10F1, Freiraum-Riegel 5020 > 0 /
  20A0 = 0 / 11A0 = 0) — alle gruen (Abschnitt 2, 4.3, 4.6).

## 7. Urteil je Punkt

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1. "kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich" | **erfuellt** | Mit der NEUEN exe (e468162b, inkl. Objekt-Fall): M1 echter Tuerweg 10D0 -> 10F0, Tritt geschoben (+X, -Z), geklettert (T2051 sub=9/0x81 obj=3), oben (T2103 y -1800 Band 1 stand=3), Tuer -> ROOM1090 (T2157); V2 identische Eingabe: mit Fix Klettern + Tuer (T492/T544/T636), ohne Fix (master-exe 2fd81d20) kein Start (0 Zeilen sub=9); M2 Elza (PL04) in einer Zwischenlage geklettert (T1002/T1054); integration_r36_klettern A/B/C gruen; Klettern auf die gefallene 5020-Kiste (T2138/T2190, Bildbogen F1920/F1936). Fix byte-true @0x8002d358-35c (Abnahme 0-2), Objekt-Fall Instruktion fuer Instruktion gegen FUN_8002c444/FUN_8002d3d8/FUN_8002dacc/Objekt-Pass/AUTO-Scan gelesen (Abschnitt 3). |

## 8. Maengel

Keine.

## 9. Hinweise (kein Mangel, fuer OFFEN/Release-Text)

1. **"Fuer den Nutzer" (NB3) nennt ROOM3050/3051 und ROOM2050/2051 nicht**, obwohl dort Kisten jetzt ebenfalls fallen
   koennen; sie stehen nur unter OFFEN ("nicht gefahren"). 3050 ist jetzt gemessen (Abschnitt 5: echter Schub +Z -> Kiste
   faellt auf Band 0, stumm). Die 1090-Zeile "landet eine Ebene tiefer (in der Sonde gemessen)" beruht auf der KUENSTLICHEN
   Schubrichtung der Diagnose (Spieler auf der Startlage der Kiste) — fuer einen Release-Text vorsichtiger formulieren
   (Landestelle haengt an der echten Schubrichtung, FUN_8002d3d8 @0x8002d410-450).
2. **ROOM2050 an der exe ungemessen** (Gegner EM25 toetet Leon waehrend des Schubwegs, kein Haken zum Abschalten). Messweg:
   Raum vorher leeren (Gegner toeten) oder Spielstand mit erledigtem Gegner; Kiste obj 6 von (-1600,-5400,-9348) nach +X bis
   x ~7600 und dann nach -Z an die Absprungzelle x 6350..8750 / z -13950..-12150 schieben, `RE15_OBJFALL_LOG` lesen
   (erwartet: Ausloeser bei Mitte z < -11700, Richtung (0,-1), Landung unterhalb Band 3; Satz 8 = Spritzer vorhanden).
3. **FUN_8001c6e8 prueft im Original KEIN Aktiv-Flag** in der Objektschleife (Zaehler Raumkopf+2, Pool 0x800b3f04 + 148 n,
   Selbst-Ausschluss `beq s5,v0` @0x8001c800, `jal 0x8002da4c` @0x8001c810 — selbst gelesen), der Port-Zwilling
   `re15_collision_room_coll` (re15_collision.c:339) ueberspringt inaktive Props. Vorbestehend, nicht Teil dieses Diffs;
   objfall_r36.c nutzt das fuer den Selbst-Ausschluss (Prop kurz inaktiv). Wirkt nur, wenn im Original ein nicht
   installierter Pool-Eintrag mit Altdaten unter der Fall-Lage laege.
4. Die Spritzer uebergeben als Bodenebene den Bild-13-Boden (DAT_8008f694) — das ist die bestehende Port-API
   `re15_esp_fx_spawn_rows(..., floor_y, ...)` (alle Aufrufer uebergeben eine Ebene; Kommentar objfall_r36.c:293-295 nennt
   die Abweichung "Original fragt je Takt FUN_8001c6e8"). Betrifft nur Wasser-Faelle (20A0/11A0 unerreichbar, 2050 ungemessen).
5. Unveraendert OFFEN laut Dossier (Adressen und Messwege dort): Gegenmessung des Falls am laufenden Original (Endwerte
   rot z 1029, Endlage nur am Port), SquareRoot12 ausserhalb 0 < a < 2^19 (@0x80066120 selbst gelesen: `addiu t1,zero,19`
   @0x80066148, Tabelle 0x8007d984, kein `srl 12` — Herleitung im Code stimmt), FUN_8002dc48 (Herunterfall des Spielers),
   H1 ROOM2060 Spinne, H3 Null-/Kleinbox-Objekte.
6. Mess-Artefakt fuer kuenftige Laeufe: mit `RE15_INPUT_SCRIPT_BASIS=spiel` beginnt das Skript nach jedem Raumwechsel neu
   (Bildzaehler je Raum ab 0; `[input-script] Tick 330 -> F331` in 1090). Laeufe ueber mehrere Raeume muessen das
   einplanen (die Skripte der Spur tun es: W11 am Anfang = Ruhe im Folgeraum).

## 10. Ergebnis

**bestanden = JA.** Der Nutzer-Punkt ist an der neuen exe erfuellt (echter Tuerweg Leon 10D0 -> 10F0 -> Klettern -> 1090,
Elza 10F1 in neuer Lage, Fix/ohne-Fix-Gegenprobe bei identischer Eingabe, Integrationsriegel A/B/C). Beide Maengel der
Abnahme 2 sind behoben: die RE-Aussagen sind berichtigt, der Objekt-Fall FUN_8002c444 ist byte-true portiert (Instruktionen
selbst gelesen, Fall-Arithmetik unabhaengig nachgerechnet) und an der exe gemessen (5020-Kette mit Latch und Klettern auf
die gefallene Kiste, Gegenprobe ohne Fall, eigene Variante mit Sperrzelle, 3050 mit echtem Schub). Gates halten: RE-Gate
(9 Stichproben + Zusatz, alle Konstanten mit @0x, PORT-WAHL belegt), Vertrag (Pfade, keine Bits/IDs/Ereignisse, kleine
Haken), keine Signalwoerter, 521/521 VM-Tests, 15/15 C-Integration, integration_r36_klettern gruen. Offene Punkte sind
unter OFFEN mit Adressen und Messweg gefuehrt; neu gemessen: 3050 (Fall nach unten mit echter Richtung).

## 11. Messprotokoll

| Lauf | Zweck | Kern-Umgebung (zusaetzlich zur Basis 4.) | Ergebnis |
|---|---|---|---|
| Bau | configure + build | local_build.sh | OK, `ninja: no work to do`, exe md5 e468162b... |
| ctest VM | alle unit_/probe_/prop_ | `-j4 --timeout 600` | 521/521, 721 s |
| ctest C-Integration | Raumladen/Tueren/Sweep | 15 Tests (Abschnitt 4.3) | 15/15, 10 s |
| ctest integration_r36 | echte exe A/B/C | `^integration_r36_klettern$` | Passed 516,77 s |
| m1_leon_10d0_10f0_1090 | echter Weg Leon + Fall-Log | DEBUG_JUMP=10d0@250, SET_FLAG=3:50,9:71, PLAYER_POS=1700,-7000,2048 | T2051 sub=9, T2103 oben, T2157 room=1090, 0 Ausloeser |
| e10f1_zwischen | Elza, Zwischenlage | TITLE_SHOT_CHAR=1, GOTO_ROOM=10F1, SET_FLAG=9:71, -Z-Schub U6.0 | PL04, T1002 sub=9 (t4 z 1667), T1054 oben |
| l5020_kette | Objekt-Fall + Latch + Klettern | DEBUG_JUMP=5020@250, PLAYER_POS=-11000,-21800,1024,4, Eingabe l5020a, FRAMEDUMP 776-2696/8 | F785 Ausloeser, F798 y -6814 Band 0, F807 Landung, F1712 (4,203)=1, T2138/T2190 oben |
| g5020_ohne_fall | Gegenprobe ohne Objekt-Fall | re15_pc_r36a.exe (7e2da559), gleiche Eingabe | Kiste bis x -6904, bleibt hinter der Kante |
| v5020_kurz / v5020_kurz2 | eigene 5020-Varianten | -Z-Schub U2.0, andere z-Linie | kein Ausloeser; Kiste stoppt x -8590 an Sperrzelle x0 -7675 |
| probe_r36_objfall pin | VM-Mechanik selbst | build/tests/unit/probe_r36_objfall.exe pin | PASS (Diagnose 3050/2050 "nach oben") |
| r3050_a / _b / _c | Nebenwirkung 3050, echter Schub | DEBUG_JUMP=3050@250, PLAYER_POS=-16500,-20800,2048,4 | c: F831 Ausloeser richt (0,1), F844 Band 4 -> 0, F853 Landung y 0 |
| r2050_a | Nebenwirkung 2050 | DEBUG_JUMP=2050@250, PLAYER_POS=-3100,-9348,0,3 | nicht messbar: Gegner EM25 toetet Leon (exit 124 = eigenes Zeitlimit) |
| e10d1_spawn / e10d1_weg | Elza-Tuerweg 10D1 | GOTO_ROOM=10D1 | Spawn (7650,0,11400); Weg blockiert, Tuer -> 10E1 (exit 124) |
| k_west_neu / k_west_ohne_fix | Fix gegen ohne Fix | DEBUG_JUMP=10f0@250, SET_FLAG=3:102,9:71, PLAYER_POS=-5925,-850,0 | neu: T492 sub=9, T636 room=1090; ohne Fix (2fd81d20): 0 Zeilen sub=9 |

Alle exe-Laeufe: Messkopie `re15_pc_abn3a_<lauf>.exe` im Spurbaum (danach geloescht; nie taskkill), Arbeitsordner
`<S>/abn3_A/laeufe/<lauf>/` (env.txt, md5.txt, debug.log, climb.txt, objfall.txt, ergebnis.txt, PPM-Serien), Skripte
`<S>/abn3_A/skripte/` (lauf.sh, ppm_blatt.py), Bildboegen `<S>/abn3_A/blatt_5020_fall.png`, `blatt_5020_klettern.png`,
`blatt_3050_fall.png`. Im Hauptbaum wurde nichts geschrieben (nur die master-exe gelesen/kopiert).
