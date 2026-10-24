# Runde 41 — Spur K "stage3szenen_b": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-24/25). Baum `.claude/worktrees/r41_stage3szenen_b`, Zweig `r41/stage3szenen_b`,
HEAD `0661fc6e`, merge-base mit master `4c667c97`. Nichts am Code geaendert; einzig diese Datei wird committet.
Bericht wird fortlaufend geschrieben (Stand je Abschnitt).
Messdateien (Skripte, Logs, Bilder) dieser Abnahme: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\k0\` (im Text kurz `scratchpad/k0/...`); Original-Spielstaende und GDB-Protokolle des Bau-Agenten im selben Scratchpad
unter `K41\orig\` (im Text `K41/orig/...`), von der Abnahme selbst dekodiert.

## 0. Gegenstand (Wortlaut AUFTRAG.md, 2026-10-23 16:3x)
* K1: "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine
  Waffe haben."
* K2: "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."
* Orchestrator A-E5: Binder FUN_8001b3f8 gehoert Spur J; K uebernimmt J's `rbj-binder`-Commit UNVERAENDERT.

## 1. Diff (git diff master...HEAD, nur Zweig-eigene Aenderungen)
Code: `engine/src/sherry_r41.c` + `include/re15_sherry_r41.h` (neu), `platform/pc/src/sherry_pc_r41.c/.h` (neu),
Haken in `platform/pc/main.c` (6 Stellen), `engine/src/player_common.c` (4 Haken), `engine/src/re15_damage.c` (2 Zeilen),
`engine/src/climb_common.c` (1 Zeile); J-Binder (cherry-pick 299b6deb): `engine/src/rbj_binder_r41.c`,
`include/re15_rbj_binder_r41.h`, `engine/src/enemy_common.c`, `platform/pc/main.c` (+3), `platform/psx/src/asset_psx.c`.
Tests: `tests/unit/test_r41_stage3szenen_b.c`, `tests/unit/probes/r41_stage3szenen_b.cmake`,
`tests/integration/test_r41_stage3szenen_b_{sherry,3091}.cmake`; Werkzeuge `tools/r41_k/*`; Belege `K_belege/*`.
* Cherry-pick-Pruefung: `git patch-id --stable` von J's `e4a9f39c` und K's `299b6deb` = beide
  `a46e3a057a256ee09e8adce7c7969f3d3b83ae0b` -> **unveraendert uebernommen** (A-E5 eingehalten).
* `AUFTRAG.md`/`VERTRAG.md`-Aenderungen im Zweig stammen aus den Orchestrator-Commits 618a17cc/700483de (Stand vom
  master), nicht von der Spur.

## 2. Bau
`bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
`LOCAL-BUILD-OK (build)`. Gegenprobe: `re15_pc.exe` 20:55:14 juenger als alle geaenderten Quellen (main.c 20:54:39,
sherry_pc_r41.c 20:54:04, player_common.c 20:52:10), Arbeitsbaum sauber (`git status` leer), exe enthaelt die neue
Zeichenkette `Koerper -> Familie` -> die exe ist der Stand HEAD.

## 3. Tests (gezielt, keine volle Suite)
* `ctest -R "r41_stage3szenen" -j 4 --timeout 2400` -> **8/8 gruen** (unit_r41_stage3szenen_b_{pins,waffe,koerper},
  unit_r41_stage3szenen_a_rbj_{mechanik,zensus,kanal}, integration_r41_stage3szenen_b_sherry 53 s,
  integration_r41_stage3szenen_b_3091 189 s).

## 4. K1 — "Ab 30C1 ... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine Waffe haben."

### 4.1 Original selbst gelesen (nicht dem Dossier geglaubt)
* Statisch (eigener Disasm, `re15_disasm.py dis 0x800314b0 150` / `dis 0x80036b68 45`, DEBUG.BIN roh ab 0x800c0000):
  - `@0x800314d4 lbu v0,-13732(v0)` (aca5c) -> `@0x80031500 addiu at,at,16240` (= Tabelle 0x80073f70, `read`:
    [60..75], Modell 15 -> Datei 75 = CD-Eintrag 74 = 126892 B = `PLD/PL0F.PLD` 126892 B).
  - Bindung ALLER Teile: `@0x8003154c sw v0,-13376(at)` (0x800acbc0, dir[0] EDD), `@0x80031578 sw v0,-13608(at)`
    (0x800acad8, dir[1] EMR), `@0x800315a0 sw v0,-13724(at)` (0x800aca64, MD1), `@0x800315b0 jal 0x8004ee78` (TIM),
    `@0x80031614 jal 0x8001e56c` / `@0x80031624 jal 0x8001e5b0` (Skelett).
  - Waffe: `@0x80031680 jal 0x800c00a8` -> DEBUG.BIN @0xa8 `lbu t1,aca5c; addiu t0,0x800c00d4; lbu v0,0(t0+t1)`,
    Tabelle @0xd4 = `01 x15, 00`; `@0x80031690 sb v0,aca5d`; `@0x80031694 ori v0,zero,0xf`, `@0x800316b8 bne v1,v0`,
    **`@0x800316c4 sb zero,-13731(at)` = Modell 0x0F -> Waffe 0**. Danach `@0x800316c8 jal 0x80037c1c` (Wunden),
    `@0x800316d0 jal 0x800c01c4` (DEBUG.BIN: Tabelle 0x80073ad8 [44..59] -> STPIC_<Modell>.TIM, 12492 B; im Port seit
    Runde 39 ueber work_vars[0x10] vorhanden, `inventar_r39.c re15_inventar_r39_stpic_index`), `@0x800316e8 jal 0x800440c4`
    (CORE), `@0x800316f0 jal 0x80036b68` (Waffe), `@0x80031718 sh v0(=100),player.hp`.
  - FUN_80036b68: `@0x80036bac addiu at,at,16872` (0x800741e8, `read`: [76 x4, 97 x4, 76 x7, 118]) + Waffe ->
    Modell 15/Waffe 0 = Datei 118 = CD-Eintrag 117 = 14696 B = `PLD/PL0FW00.PLW` 14696 B; `@0x80036be4` W-EDD,
    `@0x80036c04` W-EMR, **`@0x80036bf8 lbu v1,aca5d` / `@0x80036c08 beq v1,zero,0x80036da0`** = Waffe 0 -> kein Anhang.
  - Zielen nur mit Waffe != 0 (eigener Disasm): `@0x8003200c lbu aca5d / @0x80032014 beq v0,zero` (vor
    `ori v0,zero,0x701` -> aca58), `@0x80032430 bne`, `@0x80032720 beq` (Rennen: R1 -> +0x2f2 = 2 nur mit Waffe),
    `@0x80032a80 bne`, `@0x80032cb4 bne`. Klettern `@0x80038188 lbu aca5c / @0x8003818c ori v0,zero,0xf / beq` -> 0x63
    sonst 0x1d.
  - Bewegungstabellen (eigener Leser ueber den EXE-Kopf): Zeile 15 GEHEN @0x80073ec2 (50,48), RUECK @0x80073ee2 (35,48),
    DREHEN @0x80073f02 (0,80), RENNEN @0x80073f42 (120,64); Leser laut Ghidra-XREF @0x800324d4/@0x80032d5c/@0x800329cc/
    @0x800327e8. Zeile 0 (75,48)(70,48)(0,96)(200,72), Zeile 4 (70,48)(65,48)(0,96)(210,72).
* Dynamisch (DuckStation-Spielstand selbst dekodiert, `scratchpad/k0/orig_ss.py` mit `re15_ss.Ram`):
  `K41/orig/lauf2_30c1.sav` (md5 86737ee1..., = heutiger `HASH-957757946319438E_1.sav`): Stub @0x80026e4c `0800e003`
  (sauberer Auslieferungsstand), **aca5c = 15, aca5d = 0, work_vars[0x10] = 15**, Stage 2 Raum 0C Cut 10, Spieler
  (-19100,-1350) Gier 1024. Angezeigter VRAM-Puffer: Sherry als Kind, Beine geschlossen, KEINE Waffe
  (`scratchpad/k0/orig_lauf2_30c1_y0.png`).

### 4.2 Port gemessen — ECHTER Weg (Lauf A, `scratchpad/k0/runA.sh`)
Befehl: Karte `probe_r36_elza_karte re15_card.mcr 3061 -3500 -25500 1024 4:244 3:160`; exe-Kopie `re15_pc_k0abnA.exe`;
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=420
RE15_INPUT_SCRIPT=U0.5,W0.5,(A0.1,W1.4)x70 RE15_EXIT_AT=400#30C1 RE15_FRAMEDUMP=20-400/20:fd_` — KEIN
RE15_WORKVAR_AT, KEIN RE15_FIRE_AOT: die Kriech-Szene sub02 selbst schreibt work_vars[0x10] und nimmt die Tuer.
debug.log (Auszug):
```
[save] CONTINUE: resumed in room 3061 (hp=100)
[scd F497] Cut_chg(6) ... [scd F628] Cut_chg(1) ... [scd F785] Cut_chg(5) ... [scd F1391] Cut_chg(1)
[aot] DOOR FIRE slot=3 rect=(0,0,hw=0,hh=0) target_cut=10 spawn=(-19100,0,-1350)
[room] PC loaded room30c1.rdt (163416 bytes)
[sherry-r41] Spielermodell 4 -> 15, Waffe (aca5d) 0
[sherry-r41] Koerper -> Familie 118 (PL0F): 15 Knochen, 24 Clips, W00 9 Clips, Wurzel-Bind y -1268
[pld] Spielermodell -> PLD/PL0F.PLD (Mesh 15 Teile, Textur 256x256)
[equip] W-bank -> W00 (Clips 9, Rueckstoss-Clip7 fc=1)
[walk] F30 cut=10 mo=200 ... pl pos=(-19100,0,-1350) rot=1024
[flow] EXIT_AT: Bild 400 in Raum 30c1 erreicht -> exit
```
=> Der Spielweg erreicht 30C1 im K-Baum (die Szene haengt NICHT), Lage/Gier/Cut = Original (-19100,-1350,1024, Cut 10).
Bild: `scratchpad/k0/k0_k1_30c1_sheet.png` + Zoom `k0_k1_30c1_zoom.png` (Original-Puffer | Port F20 | F200 | F400):
Port-Sherry = Kind-Skelett, Glieder geschlossen, Schuhe am Boden im Schatten, Matrosenbluse/Rock wie Original,
KEIN Gegenstand in der Hand — deckungsgleich mit dem Original-Puffer (nur die Ruhe-Animationsphase weicht ab).

### 4.3 Port gemessen — Bewegung, R1, Bilder (Lauf B, `scratchpad/k0/runB.sh`)
Weg wie der Riegel (Elza-Neuspiel, `RE15_DEBUG_JUMP=3060@250`, `RE15_WORKVAR_AT=0x10:15@30#3061` = was 3061 @0x011F6
schreibt, `RE15_FIRE_AOT=3@60#3061` = Schacht-Tuer) — die Raumlader-Zeilen sind dieselben wie im echten Weg (Lauf A):
`[sherry-r41] Spielermodell 4 -> 15, Waffe (aca5d) 0`, `Koerper -> Familie 118 (PL0F) ... Wurzel-Bind y -1268`.
Tasten (RE15_PAD_AT, Bild je Raum): 300-329 hoch, 360-389 hoch+Kreuz, 430-469 R1, 480-499 R1+runter, 510-529 links,
540-559 runter. Auswertung `state.txt` (Schritt je Bild) + `anim.txt` (Spieler):

| Eingabe | Port je Bild | Clip (Port) | Original |
|---|---|---|---|
| hoch | 50 (30/30 Bilder, z -1350 -> -2850) | W00-Clip 5, 28 Bilder | 50, W-Clip 5 (`K_belege/k1_orig_tempo_30c1.txt`), Zeile 15 @0x80073ec2 = 50 |
| hoch+Kreuz | 120 (6 Bilder bis z -3632, dann Wand) | W00-Clip 0, 20 Bilder | 120, W-Clip 0 (20 Bilder), Wand -3632 (`k1_orig_rennen_30c1.txt`), @0x80073f42 = 120 |
| R1 (40 Bilder) | kein Zielen: mo bleibt 200 (Ruhe), Lage unveraendert | Clip 3 (1 Bild) | aca5d = 0 -> `beq` @0x80032014 |
| R1 + runter | rueckwaerts 35 je Bild, kein Zielen | Clip 5 rueckwaerts | @0x80032cb4 `bne` (nur mit Waffe), RUECK Zeile 15 @0x80073ee2 = 35 |
| links (Stand) | 80 je Bild (1024 -> -576 in 20 Bildern) | Clip 5 | DREHEN Zeile 15 @0x80073f03 = 80 |
| runter | 35 je Bild | Clip 5 rueckwaerts | 35 |

Bilder `scratchpad/k0/k0_k1_30c1_bewegung.png` (F288 Ruhe, F304/F316 Gehen, F364/F376 Rennen, F440/F460 R1, F484/F496
R1+zurueck, F516/F524 Drehen, F548 rueckwaerts): durchgehend ein geschlossenes Kind-Modell mit stimmigen Lauf-/Renn-
posen, KEINE Waffe, KEINE Zielhaltung.
Abweichung (nicht K-Wortlaut, bekannt): Renn-Stopp — Original bremst nach dem Loslassen 80/40/0/-40 (Original-Log B52-B55),
Port haelt sofort. Gilt fuer alle Figuren (Dossier OFFEN 3, Spur E / E-E2).

### 4.4 Urteil K1
**erfuellt.** Modell: am echten Spielweg (Kriech-Szene 3061 -> Schacht-Tuer -> 30C1) bindet der Raumlader Sherrys
Koerper (EDD/EMR PL0F, Kind-Skelett Wurzel -1268/-1261, W00-Bank) wie FUN_800314b0/FUN_80036b68; das Bild deckt sich
mit dem Original-Puffer; Gehen/Rennen/Drehen/Rueckwaerts in Tempo und Clip = Original. Waffe: aca5d = 0 wie @0x800316c4,
kein Hand-Mesh (@0x80036c08), R1 zielt nicht (@0x80032014 ff.), Ausruesten gesperrt (Nutzer-Wortlaut "sollte keine
Waffe haben"; Form RE2). Nebenbefunde -> Maengel M2/M3 unten (nicht Wortlaut, aber Original-Abweichungen im
selben Lader).

## 5. K2 — "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."

### 5.1 Original selbst gelesen
* Binder: ROOM3091.RDT selbst geparst (`hdr+0x5C` = RBJ @0x2510, Trailer 0xc36c, 3 Records): rec0 Marker 0x1, rec1 0x2,
  rec2 0x3 (EMR-Praefix 0x565c, EDD 0xbd38). Im Original-RAM (eigene Dekodierung der 7 Szenen-Spielstaende
  `K41/orig/k2_orig_3091_lauf5.log.lb_B*.sav`, Stub `0800e003` sauber, aca5c = 15, aca5d = 0): Spieler und Gegner 1
  (Typ 0x42) `w180/w184 = 8012737c/8012da54` = RBJ(0x80121d1c)+rec2 (EMR+4 / EDD), Gegner 0 (0x49)
  `80121de4/80126edc` = rec1 -> Bindung Spieler rec2 / John rec1 / Elza-NPC rec2 bestaetigt (= J's FUN_8001b3f8-Regel;
  alte Port-Regel: Spieler rec0, Gegner 1 ungebunden).
* Cherry-pick 299b6deb = J's e4a9f39c (patch-id gleich), Binder selbst nicht von K veraendert (A-E5 eingehalten).
* Wiedergabe-Mechanik Sub 0 (eigener Disasm): Spieler-Tabelle @0x80073e30[0] und NPC-Tabelle @0x80076ca0[0] zeigen
  beide auf **FUN_80050cb8**; dort am Clip-Ende `@0x80050da4 sb 2,+0x06` (halten), dann `@0x80050db4 lhu +0x1c4`,
  **`@0x80050dbc andi v0,v0,0x4` / `@0x80050dc0 beq` / `@0x80050dc8 sb 1,+0x06` = mit Plc_flg 0x04 laeuft der Clip in
  Schleife weiter** — fuer Spieler UND NPCs.
* 3091 sub07 (K_belege/k2_scd3091.txt, Bytes selbst gelesen): jedes Spieler-`Plc_motion(0,13)` (`3f 00 0d 00`
  @0x0170E, @0x01902, ...) ist von `Plc_flg 43 00 04 00` gefolgt (@0x01712, @0x01906) = Schleife.
* Original-GDB-Protokoll (`K41/orig/k2_orig_3091_lauf2.log`, eigener Zerleger `scratchpad/k0/orig_folge.py`, Ausgabe
  `orig_folge_lauf2.txt`): Spieler Clip 13 laeuft im Original 0..39, 0..39, ... (z.B. B1-779: 19,5 Umlaeufe,
  fl 0x24/0x04), John/Elza-NPC halten ihre Gesten (ph 2) wie im Port.

### 5.2 Port gemessen — ECHTER Weg (Lauf C, `scratchpad/k0/runC.sh`)
Elza-Spielstand 3061 mit den beiden Szenen-Vorbedingungen auf der KARTE (4:8 Sicherung oben = 3091 sub05 @0x016AA,
9:21 Riegelschluessel = sub06 @0x016C2), CONTINUE, Kriech-Szene (echt) -> 30C1 als Sherry, `RE15_FIRE_AOT=0@60#30C1` =
Tuer Slot 0 von 30C1 (@0x00B5A, Nutzlast Ziel Stage 3 Raum 9) -> Tuersequenz DOOR06 -> 3091 an der Tuer-Landung
(-26275,-875) Gier 0; Sherry GEHT per Steuerkreuz vor die Riegeltuer (F1600-1616 hoch, F1630-1642 links, F1660-1696 hoch
-> (-24712,938)) und drueckt Viereck (F1710) -> AOT-Scan -> sub07; Nachrichten per Viereck (Skript) / Kreuz alle 30 Bilder.
debug.log: `[aot] DOOR FIRE slot=0 ... spawn=(-26275,0,-875)`, `[room] PC loaded room3091.rdt`,
`[rbj-binder] ROOM3091 Spieler rec2 (alt rec0) Gegner 0:rec1(alt rec1) 1:rec2(alt rec-1)`,
`[bone]  0: rel=(0,-1261,0)` (Kind), `[scd F1757] Cut_chg(15)` (Szene), Spieler danach (-23592,109) Gier 2048 =
Original-Szenenlage, Szenenende `[aot] DOOR FIRE slot=0 ... spawn=(-6400,0,-400)` -> 30C1,
`[sherry-r41] Spielermodell 15 -> 4, Waffe (aca5d) 1`, `Koerper -> Familie 97 (PL04) ... Wurzel-Bind y -1709`.

Clipfolgen (Port `scratchpad/k0/port_folge_runC.txt` aus RE15_ANIM_TRACE, eigener Zerleger `port_folge.py`) gegen das
Original (`orig_folge_lauf2.txt`), ALLE Subs (nicht nur Kanal 0), Abschnittslaengen in Bildern:
* Spieler: 13(779) 12(50) 12r(50) 13(504) 11(50) 11r(50) 13(233) 11(50) 11r(50) 13(160) 12(50) 12r(50) 13(416) 12(50)
  12r(71) 11(50) 11r(50) 13(164), dann Gang W-Clip 5/0/5/0/5/0 (6/9/2/10/5/29) + W-Clip 1/2 — **Folge UND Laengen
  bildgenau = Original** (Original-Abtastung hat 1 Takt Etikettenversatz bei Sub-Wechseln).
* John (0x49): 2/0/1 (Gang), 3 3r 2 0 0r 8 0 0r 8 1 1r 8 4 4r 3 3r 8 1 1r 0 0r 8 (435), Wende 5/1/2, eigene Bank Clip 6
  (125) 6r (20), 4 4r 0 0r 8 (165), 5/1/2 (125), 3 3r 0 0r 8 (383), Gang — **= Original, Laengen gleich (+-1 Takt Abtastversatz an Sub-Wechseln)**.
* Elza-NPC (0x42): 2 (27) 0/5/0/5 (Gang) 1, 2 (209), 4 4r 3 3r 9 1 1r 0 0r 9 4 4r 9 1 1r 0 0r 9 4 4r 9, 5/1, eigene Bank
  Clip 6 (390) 6r (20), 5/1, 5 5r 2 9 (482) 6 6r 9 (41), Gang — **= Original, Laengen gleich (+-1 Takt Abtastversatz an Sub-Wechseln)**.

**ABER — bildgenau innerhalb der Clips:** Spieler-Clip 13 (40 Bilder, rec2) mit Plc_flg 0x04:
```
ORIGINAL B1..B779 (Spieler f):   0 1 2 ... 39 0 1 2 ... 39 0 1 ... (Schleife, 19,5 Umlaeufe)
PORT     F1758..F2536 (Slot):    0 1 2 ... 39 39 39 39 ... 39   (einmal, dann HALTEN auf Bild 39)
anim.txt: 1798 0 0 13 13 40 40 39 ... / 1802 0 0 13 13 40 44 39   (cur laeuft weiter, Slot bleibt 39)
debug.log: [scd F1758] Plc_flg(subop=0, mask=0x0004) -> slot=0 anim_flags=0x0004   (Flag ist gesetzt)
```
Die Original-Spielstaende zeigen Sherry entsprechend in wechselnden Clip-13-Bildern (B306 f25, B1506 f22, B1906 f9,
B2306 f29; Bild `scratchpad/k0/orig_sherry_clip13_phasen.png`: Kopf-/Armhaltung wechselt), der Port zeigt in allen
Clip-13-Abschnitten (779+504+233+160+416+164 = 2256 von 2878 Szenenbildern, ~78 %) **dieselbe eingefrorene Pose
(Bild 39)**. Ursache im Port: der Spieler-Pfad (`anim_select_common.c re15_compute_actor_kf`, HOLD-LAST bei
`cur >= frame_count`) wertet anim_flags & 0x04 fuer den Spieler-Kanal 0 nicht aus; der NPC-Pfad
(`enemy_ai_common.c:9852`, @0x80050dbc) tut es. Derselbe Port-Pfad haelt im K-Baum auch Elza in 3061
(Lauf A: `Plc_motion(0,10)` + `Plc_flg 0x04` @F805 -> Slot haelt 19 von F825 bis F928; Original dort nicht
mitgeschrieben, gleicher Handler FUN_80050cb8) — also kein neuer Fehler der
Spur K, aber er sitzt mitten in der vom Nutzer beanstandeten Szene, und der Riegel `integration_r41_stage3szenen_b_3091`
prueft nur die Clip-FOLGE (Wechsel), nicht die Wiedergabe im Clip -> er bleibt gruen.

### 5.3 Bild gegen Bild (eigene Paare, gleicher Szenentakt)
Die 7 Original-Spielstaende (Szenenbild 300..2700, gespeichert ~6 Takte spaeter) per RAM-Zustand im Protokoll lauf5
verortet (Spieler-/John-/Elza-Clip+Bild eindeutig): B306, B706, B1106, B1506, B1906, B2306, B2706. Port-Gegenstueck =
Szenenstart F1757 + B (Lauf D = Lauf C mit `RE15_FRAMEDUMP=2063-4463/400`; Lauf C und D haben bitgleiche
RE15_ANIM_TRACE-Bloecke -> deterministisch). Bilder: `scratchpad/k0/k0_k2_3091_orig_vs_port.png` (voll),
`k0_k2_zoom_a.png` / `k0_k2_zoom_b.png` (Figuren), `k0_k2_sherry_clip13_orig_vs_port.png` (Sherry).
* Gleich: Textzeile, Aufstellung (Elza links, John Mitte, Sherry rechts; state.txt: Spieler (-23592,109) Gier 2048 und John (-24750,1150) Gier 1024/1831 = Original exakt, Elza-NPC (-25758,-3) Gier 39/2011 gegen Original (-25761,-8) Gier 39/2015 = 3-5 Einheiten, unter 1 Pixel), John-Gesten (Hand
  ausstrecken B706, Hand an die Brust B2306), Elzas Pistolenhaltung (eigene Bank Clip 6, B1906), Sherrys Kopf-senken
  Clip 11 rueckwaerts (B2706), Sherry als Kind.
* Ungleich (Animation): Sherry in Clip 13. Pixelvergleich Sherry-Ausschnitt (52x100 @ (180,80), Schwelle 24):
  Port F2063 gegen F2863/F3663/F4063 = **0 / 0 / 0 Pixel** verschieden (eingefroren auf Slot 39); Original B306 gegen
  B1106/B1906/B2306 = **26 / 117 / 48 Pixel** (Schleife f25 -> f26 / f9 / f29: Kopf-/Armhaltung wechselt).
* Ungleich (nicht Animation, nur Hinweis): Licht — im Original tragen alle drei Figuren einen gelb-gruenen Lichtton
  (Haare/Arme), im Port neutral/blasser. Nicht Wortlaut K2; eigenes Thema (Licht 3091).

### 5.4 Urteil K2
**teilweise.** Der Kern ist behoben und am echten Weg gemessen: Binder (J, unveraendert) + Sherry-Koerper (K) ->
alle drei Figuren spielen in 3091 dieselben Clips in derselben Reihenfolge und Laenge wie das Original (Kanal 0 UND
die Geh-/Dreh-/Eigenbank-Subs), die 7 Vergleichsbilder zeigen dieselben Gesten. **Offen:** die Spielfigur Sherry spielt
ihren Schleifen-Clip 13 (Plc_flg 0x04, @0x80050dbc) nur EINMAL und steht dann ~2256 von 2878 Szenenbildern starr
auf Bild 39, waehrend das Original den Clip durchgehend wiederholt. Das ist genau die Art Abweichung, die der Nutzer
als "staendig falsche Animationen" beschreibt (das Dossier nannte vorher selbst "die Figur steht starr" als Befund),
und der Riegel misst sie nicht (M1).

## 6. RE-Gate (Stichproben selbst disassembliert, nicht dem Dossier geglaubt)
| Behauptung (Code/Dossier) | eigene Pruefung | Ergebnis |
|---|---|---|
| EDD @0x8003154c, EMR @0x80031578, MD1 @0x800315a0, TIM @0x800315b0, Skelett @0x80031614/24 | `dis 0x800314b0 150` | stimmt (sw 0x800acbc0 / 0x800acad8 / 0x800aca64, jal 0x8004ee78/0x8001e56c/0x8001e5b0) |
| Modell 0x0F -> Waffe 0 @0x800316c4, DEBUG-Tabelle @0x800c00d4 | `dis`, DEBUG.BIN roh ab 0x800c0000 (@0xa8 lbu aca5c / addiu 0x800c00d4; Tabelle `01 x15, 00`) | stimmt |
| Wunden @0x800316c8, CORE @0x800316e8, Waffenlader @0x800316f0, HP := 100 @0x80031710/18 | `dis` | stimmt (jal 0x80037c1c / 0x800440c4 / 0x80036b68; `ori v0,zero,0x64` + `sh v0,player.hp`) |
| CORE0F: Tabellen @0x80073a88/@0x80073ab0 | `read` [161..191]/[162..192]; CD-Eintrag 190/191 = 3176/41200 B = SOUND/CORE0F.EDH/.VB | stimmt |
| PL0FW00 = @0x800741e8[15] = 118, kein Anhang bei Waffe 0 @0x80036c08 | `dis 0x80036b68`, `read` [76x4,97x4,76x7,118], CD-Eintrag 117 = 14696 B = PL0FW00.PLW | stimmt |
| Zeile 15 der Bewegungstabellen @0x80073ec2/ee2/f02/f42 | eigener Leser ueber t_addr: (50,48)(35,48)(0,80)(120,64) | stimmt |
| Zielen nur mit Waffe != 0 @0x80032014/430/720/a80/cb4 | `dis` je Stelle (lbu aca5d + beq/bne v0,zero) | stimmt (@0x80032720 = Renn-Zweig R1 -> +0x2f2) |
| Klettern 0x63 fuer Modell 0x0F @0x8003818c | `dis 0x80038174` | stimmt |
| Geh-/Renn-Zweig @0x8003258c/@0x80032898 ueberspringt fuer 0x0F | `dis` (lbu aca5c / ori 0xf / beq) | stimmt |
| Wund-Zeile = aca5c @0x80037c8c, Tabelle 0x80074208 8 Zeilen | `dis 0x80037c84` (lbu -120(s3) = aca5c, sll 4), Zeile 15 @0x800742f8 = Fremddaten `33 03 36 03 ..` | stimmt |
| (vom Bau-Agenten nicht genannt) @0x800316d0 jal 0x800c01c4 | DEBUG.BIN: Tabelle 0x80073ad8 [44..59] -> ITEM/STPIC_0F.TIM (12492 B) | im Port seit Runde 39 vorhanden (`re15_inventar_r39_stpic_index` ueber work_vars[0x10]) |
| (Abnahme) Schleife Sub 0 @0x80050dbc | `dis 0x80050cb8 74`, Tabellen @0x80073e30[0] = @0x80076ca0[0] = 0x80050cb8 | Spieler-Pfad im Port fehlt -> M1 |
Suchworte im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geraten|vermut`): **keine**.
Gekennzeichnete Wahlen: Binder-Schutzgrenzen (J, PORT-WAHL), "vorherige Waffe zurueck" (K, PORT-WAHL),
Ausruest-Sperre (Nutzer-Wortlaut "keine Waffe", Form RE2). Jede K-Konstante im Code traegt ihre Adresse.
Erklaert der Fix den Befund? K1 ja (gemessen vorher: nur MD1/TIM auf Elza-Skelett + Messer-Hand; nachher Koerper +
W00 + Waffe 0). K2 zum groessten Teil (Binder + Koerper), die Clip-13-Schleife nicht (M1).

## 7. Vertrag (VERTRAG.md)
* Pfade (`git diff master...HEAD --name-only`): nur analysis/befunde_runde41/ (Dossier, K_belege, Orchestrator-
  Commits AUFTRAG/VERTRAG), re15_port/engine|include|platform/pc|platform/psx (letzteres nur J's Binder), neue Tests
  (tests/unit/probes/r41_stage3szenen_b.cmake, Quelle, 2 Integrations-Skripte), tools/r41_k. KEINE Aenderung an
  release/, platform/android/, shared_assets/PSX/, pri/, synchro/, Test-CMakeLists -> **eingehalten**.
* Bank-9-Bits / Nachrichten: keine belegt, keine neuen Texte -> eingehalten.
* Gemeinsame Dateien: main.c 6 kleine Haken (1-3 Zeilen, je "Runde 41 Spur K"), player_common.c 4 Haken (je 2 Zeilen),
  re15_damage.c 2 Zeilen (Datei ist in 1.3 der Spur D zugeordnet; 2-Zeilen-Haken, konfliktfrei gegen master),
  climb_common.c 1 Zeile. Probemerge `git merge-tree --write-tree master HEAD`: K-eigene Dateien konfliktfrei; Konflikt
  NUR in enemy_common.c (J's Binder-Cherry-pick gegen Spur A d1809dbf `mslot < 32`) und in AUFTRAG/VERTRAG
  (Orchestrator) -> bei der J/K-Integration aufzuloesen (Hinweis H1).
* Riegel je Punkt vorhanden (K1: pins/waffe/koerper + integration_sherry; K2: pins (RBJ) + integration_3091),
  parallel-fest (exe-Kopien re15_pc_r41ks_/r41kz_ + Zufalls-Laufordner, RE15_*-Umgebung geloescht), Linux-fest
  (libm, cmake_minimum_required, keine .exe-Pfade). Schwaeche: der 3091-Riegel verdichtet je Aktor auf Clip-Wechsel
  und sieht die Wiedergabe im Clip nicht (M1).

## 9. Maengel (nummeriert, nachpruefbar)
**M1 (K2, blockierend) — Spielfigur-Schleife fehlt: Sherry erstarrt in der 3091-Szene.**
Original: Sub-0-Handler FUN_80050cb8 (Spieler @0x80073e30[0], NPC @0x80076ca0[0]) setzt am Clip-Ende +0x06 = 2
(@0x80050da4) und mit `+0x1c4 & 4` (`andi v0,v0,0x4` @0x80050dbc, `sb 1,+0x06` @0x80050dc8) wieder 1 = Schleife.
3091 sub07 setzt fuer jeden Spieler-Clip 13 `Plc_flg 43 00 04 00` (@0x01712, @0x01906, ...). Original-GDB-Protokoll:
Spieler-Clip 13 laeuft 0..39 in Schleife (B1-779 19,5 Umlaeufe). Port (Lauf C/D, echter Weg): Slot 0..39 einmal, dann
Slot 39 bei cur 40..778 (anim.txt `1798 0 0 13 13 40 40 39`), obwohl `[scd F1758] Plc_flg(subop=0, mask=0x0004) ->
slot=0 anim_flags=0x0004` geloggt ist -> 2256 von 2878 Szenenbildern eingefroren; Pixelvergleich Sherry-Ausschnitt Port
0/0/0 gegen Original 26/117/48 (Bild `k0_k2_sherry_clip13_orig_vs_port.png`). Ursache: Spieler-Posenpfad
(`anim_select_common.c` HOLD-LAST bei `cur >= frame_count`) wertet anim_flags & 0x04 fuer den Spieler nicht aus; der
NPC-Pfad tut es (`enemy_ai_common.c:9852`). Derselbe Port-Pfad haelt auch Elza in 3061 (Lauf A: Plc_motion(0,10) + Plc_flg 4
@F805, Slot haelt 19 bis F928; Original dort nicht mitgeschrieben, gleicher Handler; Querbezug Spur J). Riegel `integration_r41_stage3szenen_b_3091` verdichtet auf
Clip-Wechsel und bleibt dabei gruen -> braucht eine Pruefung "Spieler-Slot springt im Clip-13-Abschnitt nach 39 auf 0"
(Soll aus dem Original-Protokoll).

**M2 (K1-Umfeld, Regel "Original oder nicht") — FUN_800314b0 nur teilweise portiert, Rest als Wahlfrage.**
Original setzt bei jedem Modell-Neuladen `ori v0,zero,0x64` @0x80031710 / `sh v0,player.hp` @0x80031718 (HP := 100;
Original-Spielstand 30C1: HP 100) und die Waffe aus DEBUG.BIN @0x800c00d4[Modell] (Elza = 1, Messer). Der Port laesst
die HP beim Wechsel auf Sherry und zurueck stehen (sherry_r41.c/sherry_pc_r41.c schreiben keine HP; Dossier OFFEN 1
bestaetigt; Sherry erbt damit Elzas Energie) und gibt beim Rueckwechsel die vorige
Waffe zurueck (PORT-WAHL). Das Dossier legt das dem Nutzer als "Entscheidungsfrage (a)" vor — die Projektregel
(Memory reai-v2-original-oder-nicht) verbietet genau diese Wahlfrage; der einzige Nutzer-Entscheid (2026-08-30,
scd_room_setup.c) betrifft die WESTE, nicht Sherry. Gefordert: Original (HP 100 bei jedem Wechsel, Messer beim
Rueckwechsel) oder eine mit RE2-Adressen belegte Retail-Form (Beta -> Retail) — nicht offen lassen. Kein Wortlaut-Punkt,
aendert das Urteil K1 nicht.

**M3 (Code-Behauptung nicht belegt, Querbezug J) — Familienregel "gleiche Waffen-Basis = bytegleicher Koerper".**
sherry_pc_r41.c bindet EDD/EMR nur bei neuer Waffen-Familie (@0x800741e8: 76/97/118) und begruendet das im Kopf mit
"EDD/EMR/W-Banken bleiben (bytegleich, Westen-Weg unveraendert)". Eigener md5-Vergleich der PLD-Teile: bytegleich nur
PL00/01/02 und PL04-07; PL03 (EMR) und PL08-PL0E (EDD+EMR) weichen ab. Im Spiel genutzt: Modell 0x0D = Marvin als
Spielfigur (ROOM3020 @0x00D7A `24 10 0d 00`, Census aller RDT-`24 10 xx 00`): PL0D EDD 3152 B / EMR 56976 B gegen PL00
3160/57136; Bindlagen Rumpf/Arme (Knochen 8-14) laenger, z.B. Knochen 10 y 485 statt 422, Knochen 11 454 statt 408, Clip 1
32 statt 34 Bilder -> Marvin behaelt Leons Skelett/Animation (vorbestehend, K aendert es nicht, behauptet aber
Bytegleichheit). Original bindet EDD/EMR bei JEDEM Neuladen (@0x8003154c/@0x80031578). Gehoert zu Spur J (Marvin
3060) bzw. in K's Modul (Bindung bei abweichenden Teilen statt nach Familie).

## 10. Hinweise (kein Mangel der Spur K)
* H1 Integration: `enemy_common.c` kollidiert beim Probemerge zwischen J's Binder (Cherry-pick 299b6deb) und Spur A
  d1809dbf (`mslot < 32`-Schutz in rbj_resolve_slot) — bei der J/K-Integration zusammenfuehren (Schutz in den Binder-
  Aufruf uebernehmen).
* H2 Licht 3091: im Original tragen Elza/John/Sherry einen gelb-gruenen Lichtton, im Port neutral (Vergleichsbilder 5.3).
  Nicht Wortlaut K2.
* H3 Die vom Bau-Agenten gemeldete "haengende Kriech-Szene" war ein RE15_SUBSTART-Artefakt: ueber den echten AOT-Ausloeser
  laeuft sub02 im K-Baum durch und nimmt die Schacht-Tuer (Lauf A).
* H4 Bekannt/offen laut Dossier und nicht K: Renn-Stopp (Spur E, E-E2), Elzas Tempo-Zeile 4 (70/65/210) gegen Port
  75/70/200, Wund-Reset beim Westen-Wechsel, PSX-Ziel ohne Koerper-Tausch.
