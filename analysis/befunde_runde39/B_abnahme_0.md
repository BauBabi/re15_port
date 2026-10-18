# Runde 39 — Spur B "toene" — ABNAHME 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_toene`, Zweig `r39/toene`, HEAD 66aea987 (Basis 3e49ff87, Baum sauber).
Pruefer: Abnahme-Agent 0. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md §2, CLAUDE.md STOP-GATE.
Ergebnis vorweg: **BESTANDEN** — alle drei Punkte an der gebauten exe selbst gemessen (Leon und Elza, echter Weg ueber die Tuer),
RE-Gate-Stichproben stimmen, Vertrag eingehalten, 5 + 22 + 7 gezielte Tests gruen. Offene Entscheidungsfragen fuer den Nutzer
und zwei kosmetische Maengel (veraltete Kommentare) unter §5.

## 0. Gegenstand

| # | Nutzer-Punkt (Wortlaut) | Umsetzung laut Bau-Agent |
|---|---|---|
| 1 | "In Room 10F0 fehlt noch beim öffnen der Schublade der Sound. Den könnte man zum Beispiel aus Resident Evil 2 holen, wenn dort eine Schublade geöffnet wird." | ROOM11F0/11F1 sub19 @0x017B6 (Cut_chg 11 = offene Schublade): RE2 ROOM1130 Satz 0x28 |
| 2 | "In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom öffnenden Gitter, den könnte man von den öffnenden Gittern der Spinnen aus STAGE2 holen." | ROOM1190/1191 sub11 @0x0286A (For = Aufschwung) / @0x0288C (Member_set(5,0) = Ruhelage): RE1.5 ROOM2060 snd0 0x0E / 0x0D |
| 3 | "In Room 1090 fehlt noch Sound für den bewegenden Schrank nach dem lösen des Rätsels. Den könnte man aus Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage." | ROOM1190/1191 sub10 @0x02796 (Work_set(3,5) = Schrank, erster Fahrschritt): RE2 ROOM1120 Satz 0x0C |

## 1. Statische Pruefung (Diff, Vertrag, RE-Gate)

### 1.1 Diff (git diff 3e49ff87..66aea987, nur eigene Dateien)
15 Dateien, +1606 Zeilen: `engine/src/toene_r39.c` (neu), `include/re15_toene_r39.h` (neu), `platform/pc/src/toene_r39_pc.c`
(neu), `engine/src/gen/re2_toene39_bank.inc` (neu, generiert), `shared_assets/RE2/TOENE39.VBS` (neu, 23272 B, md5
dc776048cec6fd83af2aa6a881ab99b1), `tools/r39_b/toene39_se_cut.py` (neu), Tests (`tests/unit/test_r39_toene.c`,
`tests/unit/probes/r39_toene.cmake`, `tests/unit/probe_r39_toene_kreuz.c`, `tests/integration/test_r39_toene.cmake`, alle neu),
Dossier + 2 Belegbilder. Gemeinsame Dateien: `scd_vm.c` +5 Zeilen (Include, Anker-Scan, Opcode-Haken; Muster Runde 38 H),
`platform/pc/main.c` +1 Zeile (Init-Haken). Die in `git diff master --stat` sichtbaren Aenderungen an AUFTRAG/UEBERGABE/VERTRAG/
orchestrator stammen aus den DREI master-Commits NACH der Basis (e76b9924, cb7d7855, 6d4abe35), nicht aus der Spur
(`git diff 3e49ff87 HEAD -- analysis/befunde_runde39/VERTRAG.md` = leer). Keine `release/`, `platform/android/`, `shared_assets/PSX/`,
keine Test-CMakeLists (probes/*.cmake wird per GLOB `tests/unit/CMakeLists.txt:4060` eingesammelt).
Vertrag §1.1/§1.2: keine Bank-9-Bits, keine Nachrichten-Ids belegt. Guess-Tells (`deferred|tunable|interim|for now|faithful|
plausibel|TODO|getenv`) im Code-Diff: KEIN Treffer (grep ueber alle 8 Code-/Testdateien).
Linux-Festigkeit (VERTRAG §2): `cmake_minimum_required(VERSION 3.16)` im -P-Skript, Kreuz-Werkzeug linkt `m`, Umgebung nur ueber
`set(ENV)` (spiel_lauf.cmake), exe-Kopie mit Praefix `re15_pc_r39b_<id>`, eigener Laufordner je Lauf. `file(COPY_FILE)` und
`.exe`-Kopiename sind das Muster der bereits im Linux-Release v0.8.26 gelaufenen Skripte (test_r34n_b_cursor, test_r35_*,
test_r38_elzatoene) — kein neues Risiko.

### 1.2 Mechanik (gelesen)
* Anker = RDT-Datei-Offset + 8-Byte-Signatur, einmal je Raum in `scd_register_current_rdt` gesucht (nur Raeume 11F0/11F1/1190/1191,
  sonst `g_re15_toene39_anker_n = 0`, der Opcode-Haken kostet in allen anderen Raeumen einen Integer-Vergleich). Signatur weicht ab
  -> Anker nicht armiert (stumm statt falsch). Opcode-Haken vergleicht Zeiger-Identitaet `t->pc == raw + off` vor jedem Opcode.
  Keiner der vier Anker-Opcodes (Cut_chg, Work_set, For, Member_set) ist ein wartender Opcode -> ein Treffer je Ausfuehrung
  (gemessen: 1 / 1 / 3 / 3, §3).
* Plattform: `toene_r39_pc.c` laedt TOENE39.VBS einmal (ein Versuch, Fehlermeldung ins debug.log), spielt ueber
  `re15_audio_se_play_bank` -> `se_play_layers` (dieselbe Stimmen-/Prioritaets-Maschine wie Raum-SE), nicht positional.
  EDT-Prio/-Stimme je Satz wie die Quelle (Schublade Prio 4 / Stimme 6 wie RE2 `00 00 54 16`; Gitter Prio 1 / Stimme 1 wie
  `00 00 b1 11`; Regal `00 00 63 00` = Direkt-Zweig ohne Gate wie in RE2s Raumbank). PSX-Build: Haken bleibt NULL -> stumm,
  `fprintf` nur unter `RE15_PLATFORM_PC`.
* `#error`-Riegel in toene_r39_pc.c haelt die Satznummern von .inc und .h deckungsgleich.

### 1.3 RE-Gate — Stichproben SELBST nachgelesen (Bytes/Instruktionen)
Alle vom Bau-Agenten zitierten Stellen, die ich nachgeschlagen habe, enthalten das Behauptete:

| Zitat (Diff/Dossier) | Selbst gelesen | Ergebnis |
|---|---|---|
| ROOM11F0 @0x017B6 `29 0b 22 02 07 01 2e 01` (Cut_chg 11, Set(2,7,1), Work_set(1,0)) | `xxd -s 0x17B6`: `290b 2202 0701 2e01 0000 3401 b8c0 ...` | OK |
| ROOM11F0 main00 @0x00CE6 Aot_set 13 sce 3 -> `ff 00 18 13` (Evt_exec sub 0x13 = sub19) | `2c0d 0331 0000 d827 b2c1 dc05 e803 ff00 1813` | OK |
| ROOM11F0 == ROOM11F1 | `cmp` -> identisch | OK |
| ROOM1190 @0x02790 `29 08` Cut 8, `09 0a 14 00` Sleep 20, @0x02796 `2e 03 05` Work_set(3,5) + `00` Nop, @0x0279A `2f 00 9c ff`, @0x0279E `0d 00 06 00 14 00 30 02 02 0e 00` (For 20 {Add_speed, Evt_next, Evt_next}) | `2908 090a 1400 2e03 0500 2f00 9cff 0d00 0600 1400 3002 0200 0e00` | OK |
| ROOM1190 @0x027DC Sleep 20, @0x027E0 `29 0b` Cut 11, Gosub 0x0B @0x027EA/@0x027F8/@0x02806 | `090a 1400 290b 2e02 0000 340c 4300 180b ... 180b ... 180b` | OK |
| ROOM1190 sub11: @0x02854 Sleep 10, @0x02858 Work_set(3,4), @0x02860 Speed_set(5,150), @0x02864 Speed_set(0x0B,-6), @0x02868 Evt_next, @0x0286A `0d 00 06 00 0a 00 30 31` For 10 {Add_speed, Add_aspeed, Evt_next}, @0x02876/@0x0287A Speed_set(5,-150)/(0x0B,6), **@0x0287E `02 00` Evt_next**, @0x02880 For 10, @0x0288C `34 05 00 00 01 00 2b 00` (Member_set(5,0), Evt_end, sub12 `2b 00`) | `090a 0a00 2e03 0400 3405 0000 2f05 9600 2f0b faff 0200 0d00 0600 0a00 3031 0200 0e00 2f05 6aff 2f0b 0600 0200 0d00 0600 0a00 3031 0200 0e00 3405 0000 0100 2b00` | OK (auch der Evt_next @0x0287E, der die 21 statt 20 Bilder erklaert; Add_aspeed -6/+6 erklaert die Summe 1230 = 150+144+...+96) |
| ROOM1190 vs ROOM1191 nur ab @0x2D22 verschieden | `cmp -l`: 48 Bytes, erstes @ Byte 11555 (= Offset 0x2D22) | OK |
| RE1.5 ROOM2060 sub02 @0x0134A `36 02 0e 00 03 02` Se_on(2,0x0E) vor @0x01356 Speed_set(5,25) / @0x0135A For 20; @0x01376 `36 02 0d 00 00 02` nach dem Rueckschwung | `2e03 0000 3602 0e00 0302 0000 0000 0000 2f05 1900 0d00 0400 1400 3002 0e00 090a 0600 2f05 e7ff 0d00 0400 1400 3002 0e00 3602 0d00 0002` | OK |
| RE1.5 ROOM2060 snd0 (RDT +0x08): EDT @0x5B38 -> Satz 0x0D @0x05B6C `00 00 a2 11`, 0x0E @0x05B70 `00 00 b1 11` | Kopf `385b 0000 b85b 0000 5876 0000`; @0x5B6C `0000 a211 0000 b111` | OK |
| RE2 ROOM1120 @0x02F2E `36 02 0c 01 04 00` Se_on(2,0x0C), @0x02F58 `18 08` Gosub sub08, @0x02F62 Se_on(2,0x0C) | `3602 0c01 0400 0000 0000 0000 1d04 ... 1808 1a00 1400 2e00 0100 3602 0c01 0400` | OK |
| RE2 ROOM1120 Raumbank EDT @0x14C74 -> Satz 0x0C @0x14CA4 `00 00 63 00` | Kopf `744c 0100`; @0x14CA4 `0000 6300` | OK |
| RE2 ROOM1130 @0x00D50 `2c 03 0d 31` Aot_set 3 sce 13 (Schublade); EDT @0x11A38 -> 0x27 @0x11AD4 `00 00 44 16`, 0x28 @0x11AD8 `00 00 54 16` | `2c03 0d31 0000 d6c4 ...`; Kopf `381a 0100`; `0000 4416` / `0000 5416` | OK |
| RE2 PSX.EXE AOT-Tabelle @0x800a73c4 [13] = @0x800a73f8 -> 0x80052044 | `re2_disasm.py table 0x800a73c4`: `[13] 0x800a73f8 -> 0x80052044`; ghidra_re2_Leon.txt Z.330452 | OK |
| RE2 @0x800520f0 `jal FUN_80077360` / @0x800520f8 `bne v0,zero,0x800522a0` (schon genommen -> Text) | `re2_disasm.py dis 0x800520f0`: `jal 0x80077360 / nop / bne v0,zero,0x800522a0` | OK |
| RE2 @0x80052110 `bne v0,zero,0x80052180`, Delay-Slot @0x80052114 `lui a0,0x228`; @0x80052118 `jal 0x800696cc` a0=31 | `80052110: bne v0,zero,0x80052180 / 80052114: lui a0,0x228 / 80052118: jal 0x800696cc / 8005211c: addiu a0,zero,31` (roh aus PSX.EXE) | OK |
| RE2 @0x80052188 `addu a1,zero,zero`, @0x8005218c `jal 0x8005ba28` | `80052188: addu a1,zero,zero / 8005218c: jal 0x8005ba28`; FUN_8005ba28 beginnt `srl t1,a0,24` (Bank = a0>>24 = 2) — dieselbe Se-Routine, die re15_audio.h fuer RE2-Lifte/Tore zitiert | OK |
| RE2 @0x800521cc `ori v1,v1,0x11`, @0x800521d8 `jal 0x8002c7ac` mit a0 = `lhu a0,4(s1)`; FUN_8002c7ac = `sb a0,-1038(at)` = 0x800cfbf2 | `800521cc: ori v1,v1,0x11 ... 800521d4: lhu a0,4(s1) / 800521d8: jal 0x8002c7ac`; `8002c7b8: sb a0,-1038(at) 0x800cfbf2` (DAT_800cfbf2 wird @0x8005209c als aktueller Schnitt gelesen) | OK |
| RE1.5 Se_on Bank 2 = snd0 (FUN_80045024) | include/re15_audio.h:128-160 (`bank 2 = *(DAT_800ac778+8)` = RDT snd0), RE_15_Quellcode_V2/FUN_80045024.c case 2 | OK (Bestand) |
| Opcode-Laengen der Signaturen (2E=3, 34=4, 0D=6, 2F=4, 29=2, 22=4, 36=12, 02=1, 0E=2, 01=2, 2B=4) | tools/scd_dump_room.py SIZES (aus scd_vm.c s_opcode_sizes) | OK |
| TOENE39.VBS reproduzierbar | `python tools/r39_b/toene39_se_cut.py` (Selbsttest gegen EDT-/Tone-/VAG-Offsets + sha1 lief durch): Ausgabe md5 dc776048cec6fd83af2aa6a881ab99b1 == installierte Datei, .inc byteidentisch | OK |

Bewertung RE-Gate: Alle verhaltensrelevanten Konstanten im Diff tragen Datei-Offsets (RDT) bzw. `@0x8005xxxx` (RE2-EXE) oder sind als
NUTZER-VORGABE (Herkunft der Toene) / PORT-WAHL (ROOM1130 unter vier RE2-Schubladen) gekennzeichnet. Der Takt der Toene ist kein
eigener Zaehler, sondern der Opcode-Takt des Original-Skripts (Anker am Opcode). Die Stichproben stimmen — Ghidra-Dump und
Roh-Disassembly aus `info/re2leon/PSX.EXE` (re2_disasm.py) decken sich. Die Korrelations-Schwellen der Integrationsriegel
(0.7 / 0.5 / 0.35 / 0.2, Anschlag 100 ms) sind Mess-Toleranzen, keine Spielkonstanten; Herleitung aus den gemessenen Werten
und dem Rauschboden steht im Skript.

### 1.4 Raumzuordnung (Nutzer "10F0" / "1090") — selbst geprueft
* ROOM10F0.RDT (Communications Room, Marvin, Pinnwand): `scd_dump_room.py` -> **0 Cut_chg** in allen Skripten; main00 nur
  Tueren, 8 Aot_set sce 1 (Texte), 4 Item_aot_set (Slots 11-14), 3 Obj_model_set Typ 0 (Item-Modelle), sub00/sub01 zwei
  Typ-4-Kisten (Spieler schiebt). Keine Schublade, die sich oeffnet.
* ROOM11F0 sub19 (AOT 13 = Schreibtisch): Cut 11 = die GEOEFFNETE Schublade mit der Patronenschachtel — eigener Bilddump
  F205 (§3.1). Andere in frueheren Runden benannte "Schubladen" sind die Leichenschubladen ROOM1200 (Flach-Objekte, keine
  Oeffnungs-Szene) — kein zweiter Kandidat gefunden. Ziffernverschiebung 10xx -> 11xx wie Runde 36 M ("ROOM 1090" = 1190).
* ROOM1090.RDT Sce_em_set: Typen 0x26 (Feuer, 7x), 0x42, 0x10 (Zombies) — **keine Hunde (0x20)**, keine fahrende Requisite.
  ROOM1190.RDT: drei Hunde 0x20 (@0x02900/@0x02914/@0x02928 im Schacht), Schrank obj 5, Luke obj 4 — die Szene des Nutzers.
=> Die Zuordnung des Bau-Agenten ist belegt; der Nutzer sollte sie dennoch bestaetigen (siehe §5 Hinweis H1).

## 2. Tests (gezielt, im Spurbaum; keine volle Suite — VERTRAG)
* Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK` (ninja: no work to do — exe 03:06 entspricht
  den HEAD-Quellen: `git diff 08b64d7b HEAD -- re15_port/engine re15_port/platform re15_port/include` = leer).
* `ctest -R "unit_r39_toene"`: **5/5 gruen** (schublade, gitter, schrank, bank, signatur; 0.17 s). Einzelausgaben gelesen:
  schublade: Anker @0x017B6, Cut 11 in Bild 3 = Ton-Bild, 2. Oeffnen ebenso, Gegenprobe ohne Anker 0 Toene, ROOM11F1 1 Anker;
  gitter: 3 Anker, Auf 134/166/198 = Bild des ersten Luken-Schritts, Zu = Auf + 21, Luke ruht im Zu-Bild, Gegenprobe 0 Toene,
  ROOM1191 3/3; schrank: 1 Ton in Bild 63 = erster Fahrschritt (3920 -> 3820), Endlage 1920 nach +38, Ton = Cut 8 + 20,
  Cut 11 = Ton + 60, ROOM1191 63/63.
* Gruppen der beruehrten Module/Raeume (unit): `unit_scd_opcodes`, `unit_gen_11f0_*`, `unit_prop_texel_key_11f0`,
  `unit_11f0_cut_after_puzzle`, `unit_r26_panel_11f0`, `unit_abtastphase_11f0`, `unit_re2_room1190_ab`, `unit_1190_weste`,
  `unit_1190_ruestung`, `unit_r36_karte_r1190`, `unit_r36_ziel1190`, `unit_r38_elzatoene_*` (7), `unit_audio_vab`,
  `unit_audio_decode`, `unit_audio_sperren`: **22/22 gruen** (1.2 s).

### 2.1 Integration (GUI, nacheinander, ctest -j1)
```
ctest --test-dir re15_port/build -R "integration_r39_toene|integration_r36_ziel1190|integration_r38_elzatoene" --timeout 1200 --output-on-failure
1/7 integration_r36_ziel1190_scheiben      Passed 232.45 sec
2/7 integration_r38_elzatoene_gunshot1041  Passed  74.99 sec
3/7 integration_r38_elzatoene_szene1021    Passed  65.99 sec
4/7 integration_r39_toene_schublade        Passed  36.46 sec
5/7 integration_r39_toene_schublade_elza   Passed  37.81 sec
6/7 integration_r39_toene_szene            Passed  49.03 sec
7/7 integration_r39_toene_szene_elza       Passed  48.91 sec
100% tests passed, 0 tests failed out of 7   (Total Test time (real) = 545.68 sec)
```
Kein roter Fensterhaken, keine Wiederholung noetig. Die Nachbarn derselben Hakenkette (r38_elzatoene) und des Raetsel-Raums
(r36_ziel1190, der echte Zielscheiben-Weg zu Bank-5-Bits) laufen mit den neuen Haken unveraendert gruen.

## 3. Eigene Messung an der exe (je Nutzer-Punkt)
Messkopie `re15_pc_abn0_<lauf>.exe` neben der exe (Asset-Wurzel = exe-Ordner), Laufordner `re15_port/build/abn0_wd/<lauf>/`
(nicht committet; enthaelt debug.log, cap.raw, karte.txt, fd_*.png). Weg = der ECHTE: Spielstand (probe_r36_elza_karte) im
Vorraum vor der Tuer, CONTINUE (RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0), Viereck in Bild 60 (RE15_PAD_AT=60:A)
am Tuersatz -> Tuer -> Zielraum; Ton-Mitschnitt RE15_AUDIO_CAP_SYNC=cap.raw (1470 Stereo-Frames je Bild, deterministisch),
RE15_SE_DEBUG=1, RE15_WINDOW_SCALE=1, Bilddumps RE15_FRAMEDUMP. Skript: Scratchpad `abn0_runs.sh` (vier Laeufe, 27/34/37/47 s).
Auswertung: (a) debug.log, (b) Kreuz-Werkzeug `probe_r39_toene_kreuz` (Vorlage = Satz der Mini-Bank, so getaktet wie der Mischer),
(c) UNABHAENGIG davon ein eigenes RMS-je-Bild-Mass ueber den Mitschnitt (Scratchpad `rms.py`, mono, 1470 Frames je Bild),
(d) Bilddumps angesehen.

### 3.1 Punkt 1 — Schublade (Leon ROOM11E0 -> 11F0; Elza ROOM11E1 -> 11F1)
Befehl: `karte 11E0 -25100 -8300 1024`; exe mit `RE15_PAD_AT=60:A RE15_FIRE_AOT=13@200#11F0 RE15_EXIT_AT=400#11F0 RE15_FRAMEDUMP=195-235/5:fd_`.
debug.log (Leon): `[pl] Spieler-Familie PL00`, `[aot] DOOR FIRE slot=0 rect=(-25100,-9550,hw=2000,hh=750) ... spawn=(250,0,250)`,
`[toene39] ROOM11F0: 1 Anker armiert`, `[fire-aot] slot=13 at F200 (Raum 11F0)`, `[toene39] TOENE39.VBS geladen: 4 Saetze, 4 VAGs`,
`[se] Stimme: se=0 layer=0 vag=0 note=64 fine=0 center=88 vol=110 pitch=0x400 (11025 Hz) -> SE-Stimme 6 (SPU 22)`,
**`[toene39] F201 ROOM11F0 Anker @0x017B6 -> Ton 0`** und **`[scd F201] Cut_chg(11)`** — Ton und Schnitt im SELBEN Bild (RE2-Form).
Elza (11E1 -> 11F1): `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, dieselben Zeilen, F201 / Cut_chg(11) F201.
Kreuzkorrelation Satz 0 (Leon und Elza bytegleich): Bild 201 **1.000** (Versatz +1 Bild = Mischer-Takt), Kontrollen 171/231/301
-0.006; falsche Vorlage (Satz 3 Regal) an 201: 0.098 -> das Werkzeug unterscheidet.
Eigenes RMS-Mass (11F0 ohne Musik): Bilder 190..200 RMS 1.0 (Stille); 202: 14; 204: 175; 206: 654; 214: **1757 (Spitze 6774 =
-13.7 dBFS)**; 224: 580; 226: 9.7; ab 228 wieder 1.0 -> ein Ton von ~25 Bildern (0,83 s = Laenge der RE2-Welle) ab dem Cut-11-Bild,
deutlich hoerbar, nichts davor, nichts danach.
Bilder: `abn0_wd/schublade_leon/fd_000200.png` = Cut 0 (Leon an der Tuer, vor dem Schnitt), **`fd_000205.png` = Cut 11: die
GEOEFFNETE Schublade mit der roten Patronenschachtel** (= das Bild, in dem der Ton beginnt).
Urteil P1: **erfuellt** — beim gezeigten Oeffnen der Schublade erklingt der RE2-Schubladen-Ton (ROOM1130 Satz 0x28), bei Leon und
Elza, im Bild des Schnitts wie im RE2-Handler. Vorbehalt Raumnummer (11F0 statt 10F0) siehe §1.4 / H1.

### 3.2 Punkt 3 — Schrank nach dem Raetsel (Leon ROOM1180 -> 1190; Elza ROOM1181 -> 1191)
Befehl: `karte 1180 -8117 10300 1024 4:0xF3` (Strom); exe mit `RE15_PAD_AT=60:A RE15_SET_FLAG_AT=5:4,5:5@300 RE15_EXIT_AT=620#1190
RE15_FRAMEDUMP=340-540/10:fd_` (Bank 5 ist raumlokal, darum die Zielscheiben-Bits nach dem Raumaufbau; sub01 @0x02468 ist der echte
Ausloeser, kein FORCE_EVENT).
debug.log (Leon): `[aot] DOOR FIRE slot=2 ... spawn=(3174,0,-11182)`, `[toene39] ROOM1190: 3 Anker armiert`, `[setflag-at] Frame 300:
flag(5,4)=1 / flag(5,5)=1`, `[scd F341] Cut_chg(8)`, `[se] Stimme: se=3 layer=0 vag=3 note=65 fine=57 center=95 vol=110 pitch=0x2f9
(8193 Hz) -> SE-Stimme -16` (Direkt-Zweig wie RE2s Record), **`[toene39] F361 ROOM1190 Anker @0x02796 -> Ton 3`** (= Cut 8 + 20 =
erster Fahrschritt), `[scd F421] Cut_chg(11)` (= Ton + 60 = 40 Bilder Fahrt + Sleep 20). Elza (1191): identische Bilder.
Kreuzkorrelation Satz 3 an 361: **0.965** (Versatz 0; Gegenprobe/Kontrollen 300/331/430: 0.10-0.12 = Raummusik); falsche Vorlage
(Satz 0) an 361: 0.116.
Eigenes RMS-Mass: Raummusik Bilder 330..360 RMS 350-590; ab Bild 363 **1598..2868 (Spitzen 5300-6700)** durchgehend bis ~420 -> das
Regal-Rumpeln liegt klar ueber der Musik; ab 423 der Hunde-Schnitt (Cut 11).
Bilder: `abn0_wd/szene_leon/fd_000350.png` (Cut 8, Schrank geschlossen vor der Wand), `fd_000380.png` (Schrank faehrt, Regal mit
Flaschen wird frei), `fd_000420.png` (Endlage, Regal frei) -> das ist der "bewegende Schrank nach dem Loesen des Raetsels".
Urteil P3: **erfuellt** — einmal beim Losfahren der RE2-Regal-Ton (ROOM1120 Satz 0x0C), Leon und Elza. Offen bleibt die vom Bau-Agenten
benannte Entscheidungsfrage (Welle 2,36 s laenger als die Fahrt 1,33 s, klingt ~1 s nach dem Halt aus; RE2-Form ohne Abbruch) — H2.

### 3.3 Punkt 2 — Lueftungsgitter der Hunde-Szene (gleicher Lauf wie 3.2)
debug.log (Leon): nach `[scd F421] Cut_chg(11)` und dem Hunde-SE (`se=5 ... SE-Stimme 7`): `[prop-member F431] prop[4] m=5 -> 0`
(sub11-Start Member_set(5,0) @0x0285C), **`[toene39] F432 ROOM1190 Anker @0x0286A -> Ton 1`** (`se=1 vag=1 note=70 center=94 vol=127
pitch=0x400 -> SE-Stimme 1`) = Beginn des Aufschwungs, **`[toene39] F453 ROOM1190 Anker @0x0288C -> Ton 2`** (`se=2 vag=2 note=69
center=93`) im Bild von `[prop-member F453] prop[4] m=5 -> 0` (Ruhelage) = Auf + 21; Hund 1: 464 / 485; Hund 2: 496 / 517; danach
`[scd F519] Cut_chg(2)`. Elza (1191): identische Bilder und Stimmen.
Kreuzkorrelation (Leon = Elza bytegleich): Auf (Satz 1, ganze Welle) an 432/464/496 **0.495 / 0.576 / 0.563** (Kontrollen 380/560:
0.006/0.039); Zu (Satz 2) Anschlag 0..100 ms an 453/485/517 **0.591 / 0.605 / 0.943**, ganze Welle 0.412 / 0.417 / 0.835 (Kontrollen
400/580: 0.016/0.020). Die niedrigeren Werte der ersten beiden Hunde liegen unter dem laufenden Hunde-SE 5 (0,66 s), wie vom
Bau-Agenten beschrieben; Hund 2 ohne folgendes Bellen 0.943.
Eigenes RMS-Mass: Bild 432: 1897 (Spitze 6277) / 441: 1387 (Aufschwung-Klappern ueber dem Ausklang des Bellens), 453: 574 ->
456: **3301 (Spitze 9105)** (Zu-Anschlag + Bellen), 486: 1783 -> 489: 3004, 519: 1450 -> 522: 1479 — jede Auf-/Zu-Stelle traegt einen
Pegelsprung ueber der Musik (~400-600).
Bilder: `abn0_wd/szene_leon/fd_000430.png` = Cut 11, das Lueftungsgitter noch geschlossen (vor dem Auf-Ton 432), `fd_000440.png` =
der erste Hund springt durch das Gitter (Bild 8 nach dem Auf-Ton), `fd_000450.png` = Gitter wieder in Ruhelage (vor dem Zu-Ton 453).
Urteil P2: **erfuellt** — je Hund beim Aufschwingen der Luke der Stage-2-Gitterton (ROOM2060 snd0 0x0E) und beim Zurueckschlagen
der Ruhelage-Ton (0x0D), Leon und Elza; Form = die Spinnengitter-Sequenz ROOM2060 sub02 (Se_on 0x0E vor For, 0x0D nach dem Rueckschwung).
Der Zu-Ton geht ueber den Wortlaut ("Sound vom oeffnenden Gitter") hinaus, folgt aber der vom Nutzer genannten Quelle — H3.

### 3.4 Nebenwirkungen
* Andere Raeume: Anker-Zahl 0 (Unit `signatur`: 10F0/10F1/1090/1091 = 0; die vier Zielraeume 1/1/3/3); kein Haken feuert ohne Anker.
* Beide Szenarien gemessen (Leon PL00 / Elza PL04), Bilder und Mitschnitt-Werte bytegleich.
* Stimmen: Schublade auf SE-Stimme 6 (Prio 4) wie RE2; Gitter auf Stimme 1 (Prio 1), Zu-Ton ersetzt den laengst beendeten Auf-Ton
  (0,38 s < 21 Bilder); Hunde-SE 5 auf Stimme 7 bleibt unberuehrt (3x gesehen). Kein `[se] GATE ... VERWORFEN` im Lauf.
* Kein Spielzustand, kein Flag, keine Nachricht wird neu geschrieben; `(9,99)` bleibt bis Aot_on 14 unberuehrt (Unit schublade).
* Paket: `release/make_package.sh` kopiert `shared_assets/RE2` als Ganzes (`cp -r "$RE2"`, Z.669) -> TOENE39.VBS liegt im Paket
  automatisch bei; nur der explizite Pruefeintrag (Vorhanden + cmp wie ELZASE.VBS, Z.269) fehlt noch (H6, Orchestrator).

## 4. Urteil

| Punkt | Urteil | Beleg (Kurzform) |
|---|---|---|
| 1 Schublade (Nutzer "10F0" = ROOM11F0/11F1) | **erfuellt** | Ton im Cut-11-Bild (F201 = Cut_chg(11)), Korrelation 1.000, RMS 1 -> 1757 -> 1, Bild fd_000205 = offene Schublade; RE2-Form @0x8005218c/@0x800521d8; Leon + Elza |
| 2 Gitter der Hunde-Szene (Nutzer "1090" = ROOM1190/1191) | **erfuellt** | je Hund Auf-Ton im Bild des ersten Luken-Schritts (432/464/496), Zu-Ton in der Ruhelage (+21, Evt_next @0x0287E), Korrelation >= 0.495 / Anschlag >= 0.591, Quelle ROOM2060 @0x0134A/@0x01376; Leon + Elza |
| 3 Schrank nach dem Raetsel (ROOM1190/1191) | **erfuellt** | ein Ton im Bild des ersten Fahrschritts (F361 = Cut 8 + 20), Korrelation 0.965, RMS 400 -> 1600-2900, Bilder 350/380/420; Quelle RE2 ROOM1120 @0x02F2E vor Gosub sub08; Leon + Elza |

Gates: RE-Gate OK (§1.3), Vertrag OK (§1.1), Tests je Punkt vorhanden und messend (Unit 5/5, Integration 4/4; Nachbarn 22/22 + 3/3),
Nebenwirkungen geprueft (§3.4). **bestanden = true.**

## 5. Maengel und Hinweise

Maengel (kosmetisch, kein Verhalten, nachpruefbar):
* **M1** `tests/integration/test_r39_toene.cmake` Z.15: Kopfkommentar sagt "zu = auf + 20", der Riegel Z.160 prueft 21 (richtig,
  Evt_next @0x0287E). Kommentar nachziehen.
* **M2** `tests/unit/test_r39_toene.c` Z.16: Kopfkommentar "20 Bilder spaeter (10 + 10 Evt_next)", der Riegel Z.286 prueft 21.
  Kommentar nachziehen.

Hinweise / Entscheidungen (fuer Orchestrator und Nutzer, kein Grund fuer eine Zurueckweisung):
* **H1 Raumnummern bestaetigen**: Die vom Nutzer genannten Raeume 10F0 (Communications Room) und 1090 (Parkplatz) tragen weder eine
  sich oeffnende Schublade noch Hunde/Schrank (§1.4). Umgesetzt wurde an der einzigen gefundenen geoeffneten Schublade (ROOM11F0/11F1,
  Schreibtisch neben dem Generator) und an der Hunde-/Schrank-Szene ROOM1190/1191. Dem Nutzer mit den Bildern vorlegen.
* **H2 Regal-Welle**: RE2s Regal-Ton (2,36 s) klingt ~1 s nach dem Halt des Schranks (Fahrt 1,33 s) aus und endet ~0,36 s nach dem
  Schnitt auf die Hunde. RE2-Form (ein Se_on, Welle ohne Wiederhol-Bit). Belassen oder auf die Fahrt kuerzen = Nutzer-Entscheidung.
* **H3 Zu-Ton am Gitter**: Zusaetzlich zum geforderten Oeffnungs-Ton spielt beim Zurueckschlagen der Luke der Ruhelage-Ton 0x0D der
  Spinnengitter (ROOM2060 @0x01376). Das ist die Form der vom Nutzer genannten Quelle; falls nur der Oeffnungs-Ton gewuenscht ist,
  waere der Anker @0x0288C zu entfernen (eine Tabellenzeile).
* **H4 Schublade bei jedem Oeffnen**: RE1.5 zeigt die Schublade auch nach dem Nehmen der Patronen erneut (sub19 ohne Aot_on 14), der Ton
  spielt dann wieder; RE2 oeffnet eine leere Schublade nicht (@0x800520f8 -> Text). Stand folgt dem gezeigten Bild.
* **H5 Nicht positional**: alle drei Toene ohne Lage (wie Runde 36 L / 38 H; RE2-Schublade ist auch im Original ohne Lage,
  @0x80052188 `addu a1,zero,zero`).
* **H6 Paket-Gate (TABU fuer die Spur)**: in `release/make_package.sh` Z.269 `TOENE39.VBS` in die RE2-Asset-Pruefung aufnehmen
  (md5 dc776048cec6fd83af2aa6a881ab99b1). Die Datei selbst kommt ueber `cp -r "$RE2"` bereits ins Paket.
* Hauptbaum nach dem Fast-Forward neu bauen (Memory `reai-v2-hauptbaum-exe-neu-bauen`), damit der Nutzer die Toene in
  `re15_port/build/platform/pc/re15_pc.exe` hoert.
