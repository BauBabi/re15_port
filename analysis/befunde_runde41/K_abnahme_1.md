# Runde 41 — Spur K "stage3szenen_b": UNABHAENGIGE ABNAHME 1

Pruefer: Abnahme-Agent 1 (2026-10-25). Baum `.claude/worktrees/r41_stage3szenen_b`, Zweig `r41/stage3szenen_b`,
HEAD `05655492` (Nachbesserung 1), merge-base mit master `4c667c97`. Nichts am Code geaendert; einzig diese Datei wird
committet. Bericht wird fortlaufend geschrieben (Stand je Abschnitt).
Messdateien (Skripte, Logs, Bilder) dieser Abnahme:
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\k1\`
(im Text kurz `k1/...`).

## 0. Gegenstand (Wortlaut AUFTRAG.md, 2026-10-23 16:3x)
* K1: "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine
  Waffe haben."
* K2: "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."
* Abnahme 0 (`K_abnahme_0.md`): K1 erfuellt, K2 teilweise (M1 Clip-13-Schleife), M2 (Wahlfrage statt Original), M3
  (Familienregel). Nachbesserung 1 (14 Commits de763bb4..05655492) behauptet M1/M2/M3 behoben.

## 1. Bau
`bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
`LOCAL-BUILD-OK (build)`. Gegenprobe: `git status` leer; `re15_pc.exe` 02:23:23 (+0200) juenger als alle geaenderten
Quellen (sherry_r41.c 02:22:26, sherry_pc_r41.c 02:00:40, plcloop_r41.c/player_common.c 00:55); die exe enthaelt die
NB1-Zeichenketten `vorher Waffe %d, HP %d` und `Wurzel-Bind y %d (Modell %d)` -> exe = Stand HEAD.

## 2. Diff der Nachbesserung (cb51ec29..05655492, Code selbst gelesen)
* `engine/src/plcloop_r41.c` + `include/re15_plcloop_r41.h` (neu im Zweig): Spur-E-Modul. Selbst geprueft:
  `git rev-parse dbea1cd2:<pfad>` = `c5a86597` / `8c6a15f2` = dieselben Blobs wie `r41/integration` und `r41/kleinkram_b`
  (f63b4fb2) -> UNVERAENDERT uebernommen; danach (576e6ac7) nur `rbj_spieler_clip_len` auf den Binder-Record
  (`re15_rbj_binder_spieler`, @0x8001b450) umgestellt (+9 Zeilen, markiert "Runde 41 Spur K"). Haken in
  `player_common.c` (2 Zeilen: include + `re15_plcloop_r41_takt(p)` nach `anim_frame++`) wortgleich zu r41/integration.
* `engine/src/sherry_r41.c`: `re15_sherry_r41_modell_setzen` setzt bei FIGUREN-Wechsel (nicht Leon 0..3 <-> 0..3 bzw.
  Elza 4..7 <-> 4..7) Waffe := Tabelle `k_debug_waffe[16]` (= DEBUG.BIN @0x800c00d4), Wunden := 0, HP := 100 (+5 nur fuer
  Modell 1 = Port-Weste, Nutzer-Auftrag 2026-08-29), Westen-Spiegel nachgezogen, Status := 0; Log mit Vorher-Werten. Die
  PORT-WAHL "vorige Waffe zurueck" (`s_waffe_gemerkt`) ist entfernt. Neu `re15_sherry_r41_ruestungswechsel`.
* `platform/pc/src/sherry_pc_r41.c`: Koerper (EDD/EMR) bei jedem Neuladen per Bytevergleich gegen die PLD des gebundenen
  Modells (`koerper_gleich`), sonst `koerper_binden` (W-Verbunde mit der neuen Bindelage); Familienwechsel wie bisher plus
  Vergleich nach `zurueck()`; CORE- und ARMS-Bank je Figurwechsel (@0x800316e8 / @0x80031708).
* Tests: `test_r41_stage3szenen_b.c` (+`schleife`, `marvin`, `marvinlauf`, erweiterte `szene`), `probes/r41_stage3szenen_b.cmake`
  (5 Unit-Teile), `integration/test_r41_stage3szenen_b_{3091 (erweitert), marvin (neu)}.cmake`; Werkzeug
  `tools/r41_k/k2_orig_rueckwechsel.py`.
* Gemeinsame Dateien seit Abnahme 0: nur `player_common.c` (+2 Zeilen, Spur-E-Haken). Uebrige Haken unveraendert (Abnahme 0, 1.).

## 3. Tests (gezielt, keine volle Suite; Bau = HEAD)
* Spur-Riegel: `ctest --test-dir re15_port/build -R "r41_stage3szenen" -j 3 --timeout 2400 --output-on-failure` ->
  **`100% tests passed, 0 tests failed out of 11`** (integration_..._3091 140.6 s, _sherry 44.7 s, _marvin 32.4 s; 5 Unit-Teile
  + 3 J-Binder-Riegel) — `k1/ctest_spur.log`.
* Alle Einheiten-Gruppen (der player_common.c-Haken trifft jeden Spieler-Plc_motion; sherry_r41.c jeden Modellwechsel):
  `ctest -R "^(unit|probe|prop)_|^(re15_tests|r28_messer_dauerschlag|r29_1d3_doppelabzug|r31_trefferboxen|
  r31_trefferboxen_versteckt|m93r_nachladen|eintritt_pose_einmal)$" -E "^(unit_r35_android_pruefkette|
  unit_r35_android_bash_mutanten|unit_r38_releaseskript_(trocken|ablauf|fehler_bau|fehler_umfeld|cmd))$" -j 3 --timeout 900`
  -> **`100% tests passed, 0 tests failed out of 924`** (577 s) — `k1/ctest_units.log`. Ausgenommen nur die 7 Release-/
  Android-Skripttests, die unter Last in Zeitablaeufe laufen (Abnahme 0 und Bau-Agent: dieselbe Gruppe); Spur K beruehrt
  weder `release/` noch `platform/android/`.
* Integrationsgruppen: s. Abschnitt 8 (laeuft nach den Messlaeufen, um Last-Flattern zu vermeiden).

## 4. RE-Gate (Stichproben SELBST disassembliert / gelesen; richtige Binaerdatei)
| Behauptung (Code/Dossier/Commit) | eigene Pruefung | Ergebnis |
|---|---|---|
| Sub-0-Handler 0x80050cb8: f314 mit Kanal +0x180/+0x184 (`lw a0,384(v0)` @0x80050d40, `lw a1,388(v0)` @0x80050d48, `jal 0x8001f314` @0x80050d4c); Clip-Ende `ori v0,zero,0x2`/`sb v0,6(v1)` @0x80050da4; `lhu v0,452(v1)` @0x80050db4, `andi v0,v0,0x4` @0x80050dbc, `beq` @0x80050dc0, `ori v0,zero,0x1` (Verzoegerungsplatz) + `sb v0,6(v1)` @0x80050dc8 | `re15_disasm.py dis 0x80050cb8 80` (PSX.EXE) | stimmt Wort fuer Wort |
| f314 zaehlt +0x95 hoch und wrappt am Ende: `lbu v0,149(v1)` @0x8001f610, `addiu` @0x8001f618, `sltu v0,v0,s4` @0x8001f624, `sb zero,149(v1)` @0x8001f63c (+ Rueckgabe 1) | `dis 0x8001f600 20` | stimmt |
| Spieler-Sub-Tabelle @0x80073e30[0] = 0x80050cb8; Dispatcher FUN_80030660 `lbu aca59` @0x8003068c, `addiu at,..,0x3e30` @0x8003069c, `lw` @0x800306a4, `jalr` @0x800306ac | `table 0x80073e30 8`, `dis 0x80030660 22` | stimmt |
| Binder FUN_8001b3f8: Schleife ueber alle Records (`lbu t2,4(a2)` Zahl), `andi v0,a0,0x1` @0x8001b440 / `beq` @0x8001b444, `sw v0,0(t3)` @0x8001b450, `sw v0,4(t3)` @0x8001b460, t3 = 0x800acbd4 (= Spieler 0x800aca54 + 0x180) -> der LETZTE Record mit Bit 0 gewinnt | `dis 0x8001b3f8 60` | stimmt |
| FUN_800314b0 bindet EDD `sw v0,-13376(at)` (0x800acbc0 = +0x16c) @0x8003154c und EMR `sw v0,-13608(at)` (0x800acad8 = +0x84) @0x80031578 OHNE Bedingung | `dis 0x800314b0 40`, `dis 0x80031530 40` (zwischen Eintritt und @0x8003157c nur `jal 0x80013b60` = Datei laden, kein Sprung) | stimmt |
| Schwanz: `lbu v1,aca5c` @0x8003167c, `jal 0x800c00a8` @0x80031680, `sb v0,aca5d` @0x80031690, `ori v0,zero,0xf` @0x80031694, `bne v1,v0` @0x800316b8, `sb zero,aca5d` @0x800316c4, `jal 0x80037c1c` @0x800316c8, `jal 0x800440c4` @0x800316e8, `jal 0x80036b68` @0x800316f0, `lbu a0,aca5d` + `jal 0x80043d8c` @0x80031708, `ori v0,zero,0x64` @0x80031710, `sh v0,player.hp` @0x80031718, `sh zero,-13588(at)` (0x800acaec = +0x98) @0x80031720 | `dis 0x80031660 52` | stimmt |
| DEBUG.BIN @0x800c00a8 = `lui t1,0x800b / lbu t1,-13732(t1) / lui t0,0x800c / addiu t0,t0,0xd4 / addu / lbu v0,0(t0) / ... / jr ra`; Tabelle @0x800c00d4 = `01` x15, `00` | Rohbytes `info/Re1.5/PSX/BIN/DEBUG.BIN` @0xa8..0xe8 selbst dekodiert; Original-RAM (`K41/orig/lauf2_30c1.sav`, Stub @0x80026e4c `08 00 e0 03` = sauber) @0x800c00a8 64 Bytes = DEBUG.BIN @0xa8 (`gleich: True`), dort aca5c 15 / aca5d 0 / HP 100 / Status 0 | stimmt |
| Modellwechsel-Bytes: ROOM3061 @0x011F6 `24 10 0f 00`, ROOM3091 @0x01D9C/@0x01DA8 `24 10 04/05 00`, ROOM3020 @0x00D7A `24 10 0d 00`, ROOM3060 @0x010D2/@0x010DE `24 10 00/01 00`; Plc_flg `43 00 04 00` ROOM3091 @0x01712/@0x01906/@0x01A36/@0x01B0E/@0x01B7A/@0x01CA0, `3f 00 0d 00` @0x0170E/@0x01902, ROOM3061 @0x010F8/@0x01126, ROOM6031 @0x0104E, ROOM1011 @0x00D9E | Rohbytes der RDTs (`shared_assets/PSX/STAGE*/`) | alle 18 Stellen stimmen |
| PLD-Teile: bytegleich nur PL00/01/02 (EDD 3160 `038c9653`, EMR 57136 `b467407e`) und PL04..07 (EDD 3156 `de98dbdd`, EMR 56576 `145d7964`); PL03-EMR, PL08..PL0F weichen ab; PL0D EDD 3152 / EMR 56976 | eigener PLD-Verzeichnis-Leser + md5 ueber alle 16 PLD | stimmt (M3-Begruendung jetzt richtig) |
| Original-Rueckwechsel per GDB (`K_belege/nb1_m2_orig_rueckwechsel_gdb.log`): Szenenbild 2800 HP := 37 + Gift; danach `aca5c 4 aca5d 1 HP 100 Status 0000 25c8 0` | Protokoll selbst gelesen; Werkzeug schreibt HP @0x800acaee / Status @0x800acaec (= Disasm-Adressen +0x9a/+0x98) und liest dieselben | schluessig; dynamisch nicht wiederholt (DuckStation-Lauf), statisch durch @0x80031710-20 gedeckt |
Suchworte im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|geraten|vermut|vorerst|Platzhalter`):
**keine**. Gekennzeichnet: Binder-Schutzgrenzen (J, PORT-WAHL), Ausruest-Sperre fuer Sherry (Nutzer-Wortlaut "keine Waffe",
Form RE2), Weste +5 (Nutzer-Auftrag 2026-08-29) und Ruestungs-Wechsel ohne Heilung (Nutzer-Entscheid 2026-08-30). Die
fruehere PORT-WAHL "vorige Waffe zurueck" ist weg (M2 der Abnahme 0 behoben).

## 5. K1 — "Ab 30C1 ... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine Waffe haben."

### 5.1 Lauf A1 — ECHTER Weg bis 3091, Tuer in 30C1 ZU FUSS (`k1/runA1.sh`)
Befehl: Karte `probe_r36_elza_karte re15_card.mcr 3061 -3500 -25500 1024 4:244 3:160 4:8 9:21`, exe-Kopie
`re15_pc_k1abnA1.exe`, `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=420 RE15_INPUT_SCRIPT=U0.5,W0.5,(A0.1,W1.4)x70`, in 30C1 `RE15_PAD_AT` = Drehen links F600-612,
Rennen F620-720, Drehen links F725-737, Gehen F745-776, Drehen rechts F785-797, Gehen F805-845, Viereck F850-950/10;
`RE15_EXIT_AT=60#3091`. KEIN RE15_WORKVAR_AT, KEIN RE15_FIRE_AOT, KEIN Sprung. (Gegenprobe: die Tasten ab F600 fallen in
3061 in die Kriech-Szene und wirken dort nicht — Cut_chg F497/F628/F785/F1391 und Spielerweg F600-F800 bitgleich zu
Abnahme 0 Lauf A ohne Tasten.) debug.log:
```
[save] CONTINUE: resumed in room 3061 (hp=100)
[scd F497] Cut_chg(6) ... [scd F1391] Cut_chg(1)
[aot] DOOR FIRE slot=3 rect=(0,0,hw=0,hh=0) target_cut=10 spawn=(-19100,0,-1350)
[room] PC loaded room30c1.rdt (163416 bytes)
[sherry-r41] Spielermodell 4 -> 15, Waffe (aca5d) 0, HP 100, Status 0000 (vorher Waffe 1, HP 100, Status 0000)
[sherry-r41] Koerper -> Familie 118 (PL0F): 15 Knochen, 24 Clips, W00 9 Clips, Wurzel-Bind y -1268 (Modell 15)
[aot] DOOR FIRE slot=0 rect=(-5750,-300,hw=350,hh=900) target_cut=0 spawn=(-26275,0,-875)   <- Sherry selbst an der Tuer
[room] PC loaded room3091.rdt (272500 bytes)
```
Bilder: `k1/a1_30c1_sheet.png` (Stehen/Drehen/Rennen in 30C1), `k1/k1_k1_30c1_orig_vs_port.png` und Zoom
`k1/k1_k1_30c1_zoom_orig_port.png` (Original-Puffer `K41/orig/lauf2_30c1.sav` y0 | Port F100 | Port F500): Port-Sherry =
Kind-Skelett, Matrosenbluse/dunkler Rock/Socken/Schuhe wie im Original, Glieder geschlossen, Fuesse im Schatten, KEIN
Gegenstand in der Hand; nur die Ruhe-Phase (Armhaltung) weicht ab (anderer Takt im Ruhe-Clip).

### 5.2 Waffe/Zielen/Tempo (Stand HEAD)
* aca5d = 0 beim Wechsel auf Sherry (alle Laeufe A1/A2/W), `[equip] W-bank -> W00 (Clips 9, ...)`; kein Hand-Mesh
  (`vis = (eq != 0) && ...`, @0x80036c08). Der Riegel `integration_r41_stage3szenen_b_sherry` (gruen, s. 3.) misst
  R1 40/40 ohne Zielen, Gehen 8/8 x 50, Rennen 10/10 x 120 (Zeile 15 @0x80073ec2/@0x80073f42); in Lauf A1 rennt Sherry in
  30C1 F620-F710 119 je Bild in +x (`state.txt`: -18981 -> -8271 in 90 Bildern, Gier -16 = cos-Anteil von 120).
* Lauf W (Elza traegt vorher die PISTOLE, RE15_EQUIP=3): `Spielermodell 4 -> 15, Waffe (aca5d) 0, HP 100, Status 0000
  (vorher Waffe 3, HP 100, Status 0000)` -> auch eine ausgeruestete Schusswaffe wird beim Wechsel auf Sherry abgelegt.

### 5.3 Urteil K1
**erfuellt.** Am echten Weg (Kriech-Szene 3061 -> Schacht-Tuer -> 30C1 -> zu Fuss durch die Tuer nach 3091) traegt Sherry
ihren eigenen Koerper (PL0F EDD/EMR, Kind-Skelett, W00-Bank) wie FUN_800314b0 (@0x8003154c/@0x80031578) und keine Waffe
(@0x800316c4, @0x80036c08); das Bild deckt sich mit dem Original-Puffer. Die Nachbesserung hat K1 nicht verschlechtert.
Nebenprobe "ab 30C1": Sherry als NPC (Typ 0x4B) in 3071 (Lauf S, `k1/runS.sh`, Karte 3071 mit 4:75/4:239/3:180): `[elzaems]
Typ 0x4B aus EMD/CDEMD1.EMS Sektor 1776 (139016 B)`, `EM4B loaded: 15 meshes, 15 bones, 24 clips`, im Bild als Kind hinter
Elza (`k1/k1_s_3071_zoom.png`) — kein Befund.

## 6. K2 — "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."

### 6.1 Original selbst zerlegt (nicht dem Dossier geglaubt)
Original-GDB-Protokoll `K41/orig/k2_orig_3091_lauf2.log` (md5 7730497b..., = `K_belege/k2_orig_3091_gdb_lauf2.log.gz`
entpackt, bytegleich), eigener Zerleger `k1/orig_c13.py` (Spieler `S:t0f st4 sub0 ... clip13 f<n>`):
```
B1-779: 779 Bilder, 19 Spruenge, 0 Abweichungen von i%40, fl 0004/0024, letzte 3 Bilder [16, 17, 18]
B880-1383: 504 Bilder, 12 Spruenge, 0 Abweichungen, [21, 22, 23]
B1484-1716: 233 Bilder, 5 Spruenge, 0 Abweichungen, [30, 31, 32]
B1817-1976: 160 Bilder, 3 Spruenge, 0 Abweichungen, [37, 38, 39]
B2077-2492: 416 Bilder, 10 Spruenge, 0 Abweichungen, [13, 14, 15]
B2714-2877: 164 Bilder, 4 Spruenge, 0 Abweichungen, [1, 2, 3]
```
= im Original laeuft Sherrys Clip 13 mit gesetztem Bit 0x04 (+0x1c4) durchgehend im Kreis (@0x80050dbc-c8).

### 6.2 Lauf A2 — ECHTER Weg in die Szene (`k1/runA2.sh`)
Wie Lauf A1 bis 30C1 (Karte + CONTINUE + Kriech-Szene), dort die Tuer Slot 0 per `RE15_FIRE_AOT=0@60#30C1` (derselbe
Tuerpfad, den Lauf A1 zu Fuss nahm: identische Zeile `DOOR FIRE slot=0 rect=(-5750,-300,hw=350,hh=900)`); in 3091 GEHT
Sherry vor die Riegeltuer (F1600-1616 hoch, F1630-1642 links, F1660-1696 hoch) und drueckt VIERECK (F1710) -> sub07;
Nachrichten mit Kreuz alle 30 Bilder; Ende -> Tuer -> 30C1 als Elza; `RE15_ANIM_TRACE`, `RE15_STATE_LOG`,
`RE15_FRAMEDUMP=1763-4663/50`. debug.log:
```
[rbj-binder] ROOM3091 Spieler rec2 (alt rec0) Gegner 0:rec1(alt rec1) 1:rec2(alt rec-1)
[scd F1757] Cut_chg(15)
[rbj-binder] ROOM3091 Aktor 1 Typ 0x49 Marker-Slot 1 -> rec1 gebunden
[rbj-binder] ROOM3091 Aktor 2 Typ 0x42 Marker-Slot 2 -> rec2 gebunden
[aot] DOOR FIRE slot=0 rect=(-26795,-1020,hw=295,hh=720) target_cut=1 spawn=(-6400,0,-400)
[sherry-r41] Spielermodell 15 -> 4, Waffe (aca5d) 1, HP 100, Status 0000 (vorher Waffe 0, HP 100, Status 0000)
[sherry-r41] Koerper -> Familie 97 (PL04): 15 Knochen, 24 Clips, W00 14 Clips, Wurzel-Bind y -1709 (Modell 4)
```
Eigener Zerleger `k1/port_c13.py` (Spieler-Zeilen `0 0 13 13 40 cur slot` im 3091-Block):
```
F1758-2536: 779 Bilder, 19 Spruenge 39->0, 0 Abweichungen von i%40, laengstes gleiches Bild 1, letzte 3 [16, 17, 18]
F2637-3140: 504 Bilder, 12 Spruenge, 0 Abweichungen, 1, [21, 22, 23]
F3241-3473: 233 Bilder,  5 Spruenge, 0 Abweichungen, 1, [30, 31, 32]
F3574-3733: 160 Bilder,  3 Spruenge, 0 Abweichungen, 1, [37, 38, 39]
F3834-4249: 416 Bilder, 10 Spruenge, 0 Abweichungen, 1, [13, 14, 15]
F4471-4634: 164 Bilder,  4 Spruenge, 0 Abweichungen, 1, [1, 2, 3]
```
= Original Abschnitt fuer Abschnitt (Versatz F = B + 1757 fuer alle sechs Anfaenge: 1758/2637/3241/3574/3834/4471 <->
B1/B880/B1484/B1817/B2077/B2714), gleiche Laengen, gleiche Umlaeufe, gleiche Endbilder; kein gehaltenes Bild mehr
(Abnahme 0: 2016 Bilder auf Slot 39).

### 6.3 Bild fuer Bild, alle drei Figuren (`k1/vergleich2.py`)
Vergleich Original B (Clip, posiertes Bild: Phase 1 vorwaerts = f, rueckwaerts (fl & 0x80) = fc-1-f, Phase 2 = zuletzt
posiertes Bild) gegen Port F = B + 1757 (Clip, slot), B1..B2952:

| Figur | gewertete Bilder | gleich | Clip ungleich | Bild ungleich |
|---|---|---|---|---|
| Spieler (Sherry) | 2868 | 2864 | 0 | 4 (B2905-2908, Geh-Sub am Szenenende, Original f1 / Port 0) |
| John (0x49) | 2642 | 2639 | 3 (je 1 Bild an Sub-Wechseln B1702/B2147/B2941) | 0 |
| Elza-NPC (0x42) | 2613 | 2601 | 2 (je 1 Bild B89/B2902) | 10 (B2942-2951, Gang am Ende) |

Die Rest-Abweichungen sind einzelne Takte an Sub-Wechseln bzw. im Gang nach der Szene (1 Bild Versatz) — keine falsche
Animation. Ohne Rekonstruktion des posierten Bildes (`k1/vergleich.py`) erscheinen zusaetzlich die Halte-Phasen (Original
zeigt +0x95 = 0 nach dem Wrap @0x8001f63c, posiert bleibt fc-1) und Rueckwaerts-Clips als "ungleich" — das ist
Darstellung des Zaehlers, nicht Bild.
Lage/Gier (`k1/lagen.py`, Original `pos()/dir` gegen Port `PL()`/`@()` an B1/306/706/1106/1506/1906/2306/2706): Sherry
und John an allen Takten exakt gleich ((-23592,109) Gier 2048; John (-24750,1150) Gier 1024 bzw. 1831 bei B1906), Elza-NPC
(-25758,-3) gegen (-25761,-8) = 3/5 Einheiten (unter einem Pixel), Gier gleich bzw. -4 bei B1906; erst im Abgang B2877 der
bekannte 1-Takt-Versatz (John ein Schritt, Elza ein Drehschritt).

### 6.4 Bild gegen Bild (eigene Paare)
7 Original-Spielstaende `K41/orig/k2_orig_3091_lauf5.log.lb_B{0300..2700}.sav` (Puffer mit Textzeile automatisch gewaehlt,
hier jeweils y240) gegen Port F = 1757 + B + 6: `k1/k1_k2_3091_orig_vs_port.png` (Teile `k1_k2_sheet_a.png`/`_b.png`):
gleiche Textzeilen, Aufstellung und Gesten an allen 7 Takten (John streckt die Hand aus B706, Elza zielt zur Tuer B1906,
John Hand an der Brust B2306, Sherry senkt den Kopf B2706). Sherry-Ausschnitt 52x100 @ (180,80), Schwelle 24, gegen das
jeweils erste Bild (B306): **Port 32/11/95/49 Pixel**, **Original 28/22/123/51 Pixel** (B1106/B1506/B1906/B2306) — Abnahme 0
hatte Port 0/0/0 (eingefroren). Zoom `k1/k1_k2_sherry_clip13_orig_oben_port_unten.png`: Kopf-/Armhaltung wechselt im Port
wie im Original. Weiter verschieden: Lichtton (Original gelb-gruen, Port neutral; Abnahme 0 H2, nicht Wortlaut K2).

### 6.5 Urteil K2
**erfuellt.** Am echten Weg spielen alle drei Figuren in 3091 dieselben Clips in derselben Reihenfolge, Laenge und Bildfolge
wie das Original (Bild fuer Bild 2864/2868, 2639/2642, 2601/2613 der gewerteten Takte gleich, Rest = Einzeltakte an
Sub-Wechseln/im Gang nach der Szene); der in Abnahme 0 offene Mangel M1 (Sherrys Schleifen-Clip 13 eingefroren) ist behoben
und am Riegel messend abgesichert.

## 7. Maengel der Abnahme 0 — nachgeprueft

### 7.1 M1 (Spielfigur-Schleife Clip 13) — **behoben** (Abschnitt 6.2/6.3; Riegel `unit_..._schleife` + erweiterter
`integration_..._3091` gruen; Gegenprobe: dieselbe Auswertung `test_r41_stage3szenen_b szene` auf dem alten Lauf
`build/tests/integration/r41_stage3szenen_b_3091_wd/lauf_7d91e6dc` -> `FAIL ... 0 Spruenge 39->0, 721/453/189/117/367/121
Abweichungen` = der Riegel misst).

### 7.2 M2 (FUN_800314b0-Schwanz statt Wahlfrage) — **behoben**
* Statisch: Abschnitt 4 (@0x80031680/90, @0x800316c4, @0x800316c8, @0x80031710/18, @0x80031720 selbst disassembliert;
  DEBUG.BIN-Tabelle im Original-RAM resident).
* Lauf W (`k1/runW.sh`: Elza-Neuspiel, `RE15_EQUIP=3` Pistole, Sprung 3060 -> 3061 an den Kriech-AOT, Kriech-Szene ECHT per
  Viereck, 30C1, Tuer, 3091 zu Fuss + Viereck, Szene, Rueckwechsel):
  `Spielermodell 4 -> 15, Waffe (aca5d) 0, HP 100, Status 0000 (vorher Waffe 3, HP 100, Status 0000)` und
  `Spielermodell 15 -> 4, Waffe (aca5d) 1, HP 100, Status 0000 (vorher Waffe 0, ...)`, danach `[equip] W-bank -> W01` —
  die Pistole kommt NICHT zurueck (alte PORT-WAHL), Elza hat wie im Original (GDB: `aca5d 1`) das Messer; Bilder
  `k1/k1_w_30c1_elza_nach.png` (Marvin-Szene 30C1 "Who are you?": Elza im eigenen Koerper PL04, Messer an der Hand).
  (Der Gift-Haken RE15_R39_GIFT_AT=1720 griff in diesem Lauf nicht — Spieler im Untersuchen-Takt nicht "aktiv"; HP/Status
  daher hier nicht trennscharf.)
* HP/Status trennscharf: mein Lauf von `integration_r41_stage3szenen_b_3091` (lauf_929dc0b1, Auswertung selbst erneut
  aufgerufen): `Elza -> Sherry: Waffe 0 (vorher 3), HP 100 (vorher 96), Status 0 (vorher 2)` und `Sherry -> Elza: Waffe 1 =
  Messer (vorher 0), HP 100 (vorher 1), Status 0 (vorher 2)`; Lauf M (unten): `Spielermodell 0 -> 13, Waffe (aca5d) 1,
  HP 100, Status 0000 (vorher Waffe 3, HP 94, Status 0002)`. = Original-Schwanz; die Wahlfrage ist aus dem Dossier
  entfernt ("Keine Entscheidungsfragen").

### 7.3 M3 (Koerper je Neuladen) — **behoben**
* md5-Zensus selbst (Abschnitt 4): die alte Kopf-Behauptung war falsch, die neue stimmt; Code bindet nach Bytevergleich.
* Lauf M (`k1/runM.sh`: Leon-Neuspiel mit Pistole + Gift, Sprung 3020, `RE15_WORKVAR_AT=0x10:13@30#3020` (= sub02
  @0x00D7A), `RE15_FIRE_AOT=2@60#3020` (= `47 02` @0x00D7E) -> 30C0; gehen/drehen/rennen/R1):
  `Koerper -> Familie 76 (PL00): 15 Knochen, 24 Clips, W00 14 Clips, Wurzel-Bind y -1810 (Modell 13)` (PL0D, Leon -1804),
  `[equip] W-bank -> W01`. Bilder `k1/k1_m_marvin_30c0.png`, Zoom `k1/k1_m_marvin_zoom.png`: Marvin steht/geht/dreht/rennt
  mit dem Messer, geschlossene Glieder, keine Bruchstellen; R1 = Messerhaltung. (Kein Original-Spielstand mit Marvin als
  Spielfigur vorhanden — Beleg bleibt der bedingungslose Code @0x8003154c/@0x80031578; der Weg ueber RE15_WORKVAR_AT ist
  der Riegel-Weg, die echte 3020-Szene prueft Spur J.)
* Westen-Weg unveraendert: `unit_r41_stage3szenen_b_{waffe,koerper,marvin}` gruen (Ruestung 4->5, 0->2: Waffe/HP/Status
  bleiben; Weste ab = kein Koerperwechsel); Integrationsgruppe Weste/Kleidung s. 8.

## 8. Vertrag (VERTRAG.md)
* Pfade (`git diff master...HEAD --name-only`): nur `analysis/befunde_runde41/` (K-Dossier, K_belege, K_abnahme_0;
  AUFTRAG/VERTRAG nur aus den Orchestrator-Commits 618a17cc/700483de), `re15_port/engine|include|platform/pc|platform/psx`
  (letzteres nur J's Binder), neue Testdateien (keine Test-CMakeLists), `tools/r41_k`. Keine Aenderung an `release/`,
  `platform/android/`, `shared_assets/PSX/`, `pri/`, `synchro/` -> **eingehalten**.
* Bank-9-Bits / Nachrichten-Ids / Slots: keine belegt, keine neuen Texte -> eingehalten.
* Gemeinsame Dateien: seit Abnahme 0 nur `player_common.c` +2 Zeilen (Spur-E-Haken, wortgleich zu r41/integration);
  sonst die in Abnahme 0 angenommenen kleinen Haken (main.c, re15_damage.c, climb_common.c).
* Probemerge (eigener `git merge-tree --write-tree r41/integration 05655492`): Konflikte in `plcloop_r41.c` (add/add;
  K-Fassung = E-Blob c5a86597 + 9 Zeilen Binder-Record -> K nehmen), `player_common.c` (nur Nachbarzeilen zu E's
  Rennstopp-Haken: `#include "re15_rennstopp_r41.h"` neben dem leeren K-Block und `zielt_schon`-Zeile `|| s2_41` neben K's
  Waffe-0-Sperre -> beide behalten), `enemy_common.c` (J-Binder gegen A `mslot < 32`, Abnahme 0 H1) und AUFTRAG/VERTRAG
  (Orchestrator). `rbj_binder_r41.c/.h` sind zwischen K und r41/integration bytegleich (J unveraendert).
* Riegel je Punkt vorhanden und messend (K1: pins/waffe/koerper + integration_sherry; K2: schleife + integration_3091;
  M2/M3: waffe/marvin + integration_marvin), parallel-fest (exe-Kopien `re15_pc_r41k{s,z,m}_<id>`, Zufalls-Laufordner,
  RE15_*-Umgebung geloescht), Linux-fest (`cmake_minimum_required` in allen -P-Skripten, libm, exe-Endung aus
  RE15_PC_EXE, keine Backslash-Pfade).

## 9. Integrationsgruppen (laeuft)

## 10. Hinweise (kein Mangel der Spur K; fuer Integration/Orchestrator)
* **H1 Integration plcloop_r41.c:** add/add gegen r41/integration — die K-Fassung nehmen (= E-Blob c5a86597 + Binder-Record,
  selbst per `git rev-parse` geprueft). Wichtig fuer alle Raeume mit zwei Spieler-Records (eigener RBJ-Zensus ueber alle RDTs:
  2000, 3091, 30E0, 4000, 4001, 4010, 5011, 6030, 6031 — rec0 und rec2 mit Bit 0): dort rechnete E's Fassung mit rec0
  (3091: 1 Clip, 6030/6031: 1 Clip, 4001: 11 statt 14 Clips), der Zeichner (J's Binder) aber mit rec2. In
  `player_common.c` sind die Konflikte nur Nachbarzeilen zu E's Rennstopp (`#include "re15_rennstopp_r41.h"`,
  `(!zielt_schon || s2_41)`) — beide Seiten behalten; danach Sherrys Renntempo 120 (Zeile 15 @0x80073f42) gegen E's
  Rennstopp-Modul nachmessen (`integration_r41_stage3szenen_b_sherry` mitlaufen lassen). `enemy_common.c` wie Abnahme 0 H1.
* **H2 Nachfahren bei der Integration:** E's Kurbel-Riegel (ROOM2020, 1 Record -> unveraendert, `unit_..._schleife` prueft
  40 Bilder), J's `integration_r41_stage3szenen_a_3060/3061` (Marvin jetzt PL0D-Koerper, Messer, HP 100 — deckt sich mit
  J's eigener Original-Messung "Marvin Modell 0x0D, Waffe 1", J_stage3szenen_a.md §1.3).
* **H3 Bewegungstabellen (Orchestrator, spielweit):** selbst gelesen @0x80073ea4/ec4/ee4/f24 (je 16 Zeilen [Tempo,Drehung]):
  Zeilen 0-3 und 8-11 = 75/70/(0,96)/200, Zeilen 4-7 und 12-14 = 70/65/(0,96)/210, Zeile 15 = 50/35/(0,80)/120. Der Port
  nutzt ausser fuer Sherry ueberall 75/70/96/200 (player_common.c WALK/BACK/RUN_SPEED). Betroffen sind nicht nur Elza
  (Zeile 4, Dossier OFFEN 2), sondern auch **Marvin als Spielfigur (Zeile 13 = 70/65/96/210)**, seit Nachbesserung 1 mit
  eigenem Koerper.
* **H4 Ausruest-Anzeige nach dem Rueckwechsel (aus dem Code abgeleitet, nicht am Spiel gemessen):** FUN_800314b0 setzt nur
  aca5d (Messer), nicht den Ausruest-Platz 0x800b25c8 (eigener Scan: FUN_800314b0 bis @0x80031730 und die Aufrufe FUN_80036b68/FUN_80037c1c/FUN_800440c4/FUN_80043d8c greifen 0x800b25c8 nicht an; Gegenprobe des Musters: der Menue-Commit liest es `lbu v1,9672(v1)` @0x80046658). Hatte Elza vor 3061
  eine Waffe AUS DEM INVENTAR ausgeruestet, zeigt das Inventar danach diese Waffe als ausgeruestet, in der Hand ist das
  Messer; ein erneutes "Ausruesten" derselben Waffe committet nicht (menu_common.c Schliess-Commit nur bei geaendertem
  inv[25c8], @0x800465f4-661c) — erst ueber eine andere Waffe. Im Original gleich (gleicher Mechanismus); der Satz im Dossier
  "die Pistole ... laesst sich wieder ausruesten" stimmt nur mit diesem Umweg. Gilt sinngemaess fuer Leon -> Marvin.
* **H5** (aus Abnahme 0, unveraendert): Status-Schirm als Sherry zeigt "Equip Arms" = Messer (25c8), im Spiel keine Waffe —
  Original gleich; Entscheid "keine Waffe auch in der Anzeige" (RE2-Retail) liegt beim Orchestrator/Nutzer.
* **H6** (aus Abnahme 0, unveraendert): Lichtton 3091 (Original gelb-gruen, Port neutral) — eigenes Thema, nicht Wortlaut K2.
* **H7** Dossier OFFEN 4 (Ruestungs-Wechsel: Original setzt auch dort Waffe/Wunden/Status, Port behaelt sie) und OFFEN 8
  (Port-Spielstand im Sherry-Abschnitt laedt als Elza) sind vorbestehend und korrekt an den Orchestrator gegeben.

(weitere Abschnitte folgen)
