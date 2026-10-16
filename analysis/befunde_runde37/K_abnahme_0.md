# Runde 37 — Spur K "roy": UNABHAENGIGE ABNAHME 0 (2026-10-12)

Pruefer: Abnahme-Agent (nicht der Bau-Agent). Baum `.claude/worktrees/r37_roy` (Zweig r37/roy, HEAD 9ca3bb15,
Basis master 3feb3a78). Alles hier ist SELBST gemessen; Aussagen des Bau-Agenten sind nur dann uebernommen, wenn
sie unten mit eigenem Befehl/Ausgabe belegt sind. Bericht wird fortlaufend geschrieben (Stand je Abschnitt).

Wortlaut (AUFTRAG.md Nachtrag 7):
> Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine
> Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben.....

Teilpunkte fuer das Urteil: K1 Modell, K2 alle Animationen, K3 keine Dauerschleife (Elzas Szenario: ROOM1011 + ROOM1021).

**ERGEBNIS: BESTANDEN** — K1/K2/K3 erfuellt, alle Gates halten, keine Maengel; Hinweise H1-H5 (§8) gehen an den Orchestrator.

## 0. Bau

* `cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build`
  -> `=== LOCAL-BUILD-OK (configure)`, `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`, EXIT=0.
  Arbeitsbaum sauber (`git status --short` leer), exe enthaelt die neuen Zeichenketten (`[roy] Typ 0x%02X aus %s Sektor %u`).
* Diff (gegen merge-base, `git diff master...HEAD --name-only`): 32 Dateien, davon Code: anim_select_common.c (2 Zeilen),
  enemy_ai_common.c (1), enemy_common.c (1+Kommentar), main.c (3 Haken), NEU roy_r37.c, roy_pc.c, re15_roy_r37.h,
  gen/ems_toc_r37.inc, tools/r37_k/gen_ems_toc_r37.py, Tests (probes/r37_roy.cmake, test_r37_roy.c, test_r37_roy.cmake).
  Hinweis: `git diff master --stat` (ohne merge-base) zeigt zusaetzlich geloeschte Orchestrator-Dateien — das ist nur
  der seit 3feb3a78 weitergelaufene master, NICHT eine Loeschung der Spur.

## 1. Spur-Tests (selbst gefahren)

`ctest --test-dir <baum>/re15_port/build -R "r37_roy" --timeout 600 --output-on-failure -V`
-> `100% tests passed, 0 tests failed out of 8` (Total 208.86 s; integration_r37_roy_szene1011 98.5 s, _szene1021 55.1 s,
_leon1170 55.0 s). Wichtige Zeilen (woertlich aus dem Lauf):
* 680 `Ruhe-Clip 5 (sub03 @0x00DE0 + Plc_flg 4) taktet mit 28 Bildern`, `Ohnmacht @0x00D54 in Bild 1811: Clip 9, 100 Bilder`,
  `Clip 9 laeuft bis Bild 99 und haelt (163 Bilder auf 99, 0 Ruecksprunge)`
* 681 `Plc_dest @0x020B2 (Bild 503): Gang-Clip 5 der eigenen Bank, 61 Bilder`, `Sturz @0x020DE in Bild 683: Loco-Clip 7, 113 Bilder`,
  `Clip 7 laeuft bis Bild 112 und haelt (90 Bilder auf 112, 0 Ruecksprunge ...)`
* 683 (echte exe 1011) `2301 Spurzeilen`, `0 Klemmungen`, `0 Abweichungen` (Bild/Uhr), `0 falsch`, `Schluss-Clip 9 endet auf Bild 99 und haelt (387 Bilder)`
* 684 (echte exe 1021) `1101 Spurzeilen`, `Schluss-Clip 7 endet auf Bild 112 und haelt (262 Bilder)`
* 685 (Leon 1170) `Elliot: 317 Spurzeilen (Uhr != Bild in 281 Zeilen = Leon-Stand vor Runde 37, unveraendert)`

### 1b. Gezielte Regression (Module des Diffs; KEINE volle Suite im Spurbaum)

`ctest --test-dir <baum>/re15_port/build -R "r37_roy|elza|elliot|unit_npc|marvin|anim_pose|anim_rate|anim_reverse|unit_ems|unit_re2_ems|
r35_entladen|cut10f0|adaruf|r35_affen|irons|r35_cut1150_szene|r36_szene1150|zombie_pose|ada_eintritt|ada_cutscene|r22_10f0|1170"
--timeout 600 --output-on-failure -j2` (117 Tests, davon 28 Integration: alle Elza-Szenen Runde 30/33/34/35/36, Elliot/1170, NPC-Executor
und Kanaele, Marker-Binder (cut10f0-Alias, adaruf, Affen, Irons), anim_select, EMS-Leser, Entladen a-l, Szene 1150)
-> **`100% tests passed, 0 tests failed out of 117`** (Total Test time 1037.33 s, kein Wiederholungslauf noetig).

## 2. Original — selbst nachgemessen (RE-Gate, Disasm + RAM + Disc)

Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE` (md5 b55fdaa5…, Spurbaum ==
Hauptbaum; `@0x80026e4c = 08 00 e0 03` = Auslieferungsstand, kein Patch-Build); eigene Skripte im scratchpad (`k_abn/v_toc.py`,
`v_ram.py`, `v_ram2.py`, `v_ram3.py`, ISO-Leser inline).

Stichproben (5 statt 3), jeweils Sprungziel bzw. Speicherstelle selbst disassembliert:
1. **FUN_80022300 (Modell-Lader)** — `80022324 lbu v0,8(s2)`, `8002232c lw v1,-13764(v1)` (=0x800aca3c), `80022334 bltz v1,0x800223ac`,
   `80022338 addiu a0,v0,-16`; Leon `8002233c ori v0,zero,0x26`, Laenge `80022354` 0x80072f3c, Sektor `80022370` 0x80072f38;
   Elza `800223ac ori v0,zero,0x27`, Laenge `800223c4` 0x8007317c, Sektor `800223e0` 0x80073178; danach `8002242c-80022464`
   LBA = Dateitabelle 0x8006f440[id*8] (lhu +0 / lbu +2) + TOC-Sektor. Aufrufer `80042328 jal 0x80022300` (Sce_em_set). **== Behauptung.**
2. **FUN_8001b3f8 (RBJ-Marker-Binder)** — `8001b3fc lw v0,0x800ac778` / `8001b404 lw a2,92(v0)` (RDT+0x5C), Bit 0 -> Spieler
   0x800acbd4/0x800acbd8, jedes weitere Bit -> `8001b498 addiu at,..,0x800acdac` + a3*0x1F4 (+0x180) und `8001b4b4 0x800acdb0` (+0x184).
   Kein Typ-Test im Original. **== Behauptung** (Marker 0x2 = Gegner 0).
3. **Plc_motion-Handler @0x80041b90** — `80041ba8 sb a1,148(v0)` (+0x94 Clip), `80041bc4 sb a2,5(v0)` (+0x5 Sub), `80041bc8 sh a1,452(v0)`
   (+0x1c4 = pc[3]). **== Behauptung.**
4. **Executor-Subs** (Tabelle @0x80076ca0: [0]=0x80050cb8 [1]=0x80050ddc …): Sub 0 `80050d40 lw a0,384` (+0x180) / `80050d48 lw a1,388`
   (+0x184) -> `jal 0x8001f314`; Sub 1 `80050e64 lw a0,132` (+0x84) / `80050e6c lw a1,364` (+0x16c). Phase 2 (+6 = 2) ruft KEIN
   anim_set mehr (= Halten), Schleife nur bei +0x1c4 & 4 (`80050dbc andi v0,v0,0x4`). **== Behauptung.**
5. **Executor-Sub 4 (Plc_dest-Gang)** @0x80051148: `800511dc sb v0(=5),148(v1)` (+0x94 = 5), `800512bc/80051358 lw a0,368` (+0x170) /
   `lw a1,372` (+0x174) -> anim_set = eigene Bank dir[4]/dir[3] Clip 5. **== Behauptung** (Gang 61 Bilder aus CDEMD1).

Daten:
* TOC: `ems_toc_r37.inc` beide Tabellen (je 128 u32) **bytegleich** mit PSX.EXE @0x80072f38 / @0x80073178 (v_toc.py: `== EXE: True`);
  Eintrag 0x47: Leon @0x800730f0 = 1644 / 127640, Elza @0x80073330 = 1639 / 142272.
* Dateitabelle: Datei 0x26 Bytes `24 05 00 ..` -> LBA 1316, Datei 0x27 `2b 0e 00 ..` -> LBA 3627. ISO-Verzeichnis der MZD-Disc
  (`…/ePSXe2018/Biohazard 1.5 (MZD Mod) Update 25-01-2025.bin`, nur gelesen): `CDEMD0.EMS LBA 1316`, `CDEMD1.EMS LBA 3627`;
  Roy-Blob @Sektor 1639 auf der Disc == shared_assets/PSX/EMD/CDEMD1.EMS (md5 der Datei a3ff26b3…, == info/Re1.5/PSX).
* **Original-RAM** (Nutzer-Spielstaende `%LOCALAPPDATA%/DuckStation/savestates/HASH-957757946319438E_{1,2,3,5}.sav`, md5 4b254b79/
  a7b970d5/e5c87e78/c87ebb0e = die im Dossier genannten, nur gelesen): in allen vier `@0x80026e4c = 0800e003` (saubere EXE), Elza-Bit = 1.
  Roys Entity (Typ 0x47): EDD +0x16c (dir1, 2008 B), EMR +0x84 (dir2, 34096 B), EDD +0x174 (dir3, 836 B), EMR +0x170 (dir4, 13048 B)
  **== CDEMD1: True, == CDEMD0: False** (alle vier Staende); Mesh +0x10 = Blob+0xc364 (CDEMD1 dir7 0xc358 + 0xC).
  Slot 1/2/3 (RDT-Animationsabschnitt @+0x1384 im RAM bytegleich ROOM1011): +0x180/+0x184 zeigen auf **rec1 (Marker 0x2)**.
  Slot 1: Sub 0, Phase 1, Clip 5 (Kopf {28 Bilder}), +0x168 -> Bild 18, +0x1c4 0x24 (Schleife). Slot 2/3: Sub 0, **Phase 2**, Clip 9
  ({100 Bilder}), +0x168 -> **Bild 99**, +0x1c4 0x20. Slot 5 (ROOM1021): Roy = Gegner 1, +0x180/+0x184 = 0/0 (kein Record),
  Sub 1, Clip 23 ({30 Bilder}), +0x1c4 0x04.
  -> Das Original laedt fuer Elzas Typ 0x47 CDEMD1 (nicht Elliot/CDEMD0), bindet in 1011 rec1, haelt Clip 9 auf dem letzten Bild.
* RBJ ROOM1011 aus der Datei (eigener Leser): rec0 Marker 0x1 = 10 Clips {1,1,1,1,1,20,25,20,18,20}; rec1 Marker 0x2 = 14 Clips
  {1,1,1,1,1,28,78,30,80,100,60,22,35,2}; rec1 Clip 13 = Schluesselbilder 0x183/0x184.

Gate-Urteil RE: **haelt.** Jede Konstante des Diffs traegt @0x… oder kommt roh aus der EXE (TOC-Generator mit Byte-Ankern);
2048 = CD-Sektor (Einheit der TOC-Sektoren, durch RAM == Datei@Sektor*2048 belegt). Kein "deferred/tunable/interim/for now/
faithful/plausibel/TODO" im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen, grep leer), kein Env-Schalter als Abschluss.

## 3. Eigene Messung an der exe (Messkopien `re15_pc_kabn0_*.exe`, Arbeitsordner scratchpad `k_abn/<lauf>`)

### 3.1 M1 — ROOM1011 ueber die Tuer (Szene laeuft von selbst)
Karte: `probe_r36_elza_karte re15_card.mcr 1021 -4000 -18000 2695 3:0xC9`; Lauf: `RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1
RE15_ENEMY_DIAG=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_ANIM_TRACE=anim.log RE15_FIRE_AOT=1@40#1021
RE15_EXIT_AT=2400#1011 RE15_FRAMEDUMP=0-2400/25:f_` -> rc=0, 96 s. (Tuer-AOT 1 = derselbe Pfad wie Hineinlaufen: re15_aot_fire_slot.)
debug.log: `[fire-aot] slot=1 at F40 (Raum 1021)`, `[room] PC loaded room1011.rdt`, `[roy] Typ 0x47 aus EMD/CDEMD1.EMS Sektor 1639
(142272 B) — Elza-Archiv (@0x800223ac)`, `[enemy-diag] actor1 type=0x47 model=enemy-bank slot=11 motion=5 anim_frame=0 clips=14`;
**keine** `[elliot]`-Zeile.
Eigene Spur-Auswertung (`k_abn/spur.py`, Segmente Typ 0x47): 21 Segmente, **0 Klemmungen, 0 Uhr!=Bild**:
`5/28 Schleife (Skript sub03 Plc_flg 4)` | `7/30 vor 1..29 + 22 halten, rueck 28..0 + 22 halten` | `6/78 1..77 + 24 halten` |
`10/60 vor 1..50, rueck 58..9` (Skript: Sleep 50 je Richtung, @0x00C76/@0x00C82) | … | **`9/100 ab B1816: 1..99, haelt 487 Bilder auf 99`**.
Abfolge Clip fuer Clip == ROOM1011 sub02 (Work_set 2 = Roy: @0x00B92 7/7r/5+4, @0x00BEA 6/5+4, @0x00C4A 7/7r/6/10/10r/5+4,
@0x00CD2 7/7r/6/5+4, @0x00D22 10/10r/7/7r/**9**), Sleep-Laengen (0x32/0x33/0x64) == Segmentlaengen.
Bilder (FRAMEDUMP, WINDOW_SCALE=1; `k_abn/m1011_blatt1.png`, `_blatt2.png`, `_blatt3.png`, `m1011_zoom.png`): Roy in RPD-Uniform (schwarze
Weste "POLICE", Abzeichen, blaues Hemd) sitzt an der Wand, Elza kniet daneben; ab "*groans*" (B1825) sackt er zusammen, ab ~B1925 liegt
er flach, bis B2400 unveraendert liegend.
Trace-Riegel auf MEINER Spur: `test_r37_roy spur m1011/anim.log 1011` -> `OK (0 Fehler)`.

### 3.2 M2 — ROOM1021 ueber die Tuer aus 1011 + Hineinlaufen in den Szenen-AOT (ECHTER Weg, kein GOTO/FORCE_EVENT)
Karte: `probe_r36_elza_karte re15_card.mcr 1011 4650 -4000 1024 4:0xF1 4:0xFA 3:0x74 4:0x60 3:0xC9` (1011 nach der Szene, 1021 scharf wie
nach ROOM1211 sub02). Lauf: Basis wie M1 + `RE15_PAD_AT=40:A RE15_FIRE_AOT=1@80#1011 RE15_INPUT_SCRIPT=XU5 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=60 RE15_STATE_LOG=state.log RE15_EXIT_AT=1500#1021 RE15_FRAMEDUMP=0-1500/25:f_` -> rc=0, 67 s.
* 1011 (Wiederbetreten): `[roy] … CDEMD1.EMS Sektor 1639`, `[enemy-diag] actor1 type=0x47 model=enemy-bank … motion=13 … clips=14`
  (sub05 @0x00E00 Clip 13, 2 Bilder, Schleife = liegt bewusstlos).
* Tuer: das Viereck @F40 griff nicht (Lage/Blickrichtung), der Rueckfall `[fire-aot] slot=1 at F80 (Raum 1011)` nahm die Tuer;
  `[tuer] Sequenz … DOOR07`, `[room] PC loaded room1021.rdt`, `[roy] … Sektor 1639`, `[enemy-diag] actor2 type=0x47 model=enemy-bank
  slot=12 motion=23 anim_frame=0 clips=24`, `[enemy] EM49 loaded` (John).
* state.log: Elza laeuft ab F60 (pad 0x4010 = Kreuz+hoch) von (-3976,-7440) nach Westen, F100 (-11171,-7486) = im AOT-Rechteck
  slot 5 (-13100..-11000, -9100..-5100, flags 0x41 = AUTO) -> `pm=2`, `msg(a=1 … id=0)` = Szene sub02 laeuft.
* Spur Roy (Aktor 2): `23/30 Schleife B0-596` | `5/61 B597-776 (eigene Bank, Halbtakt, Plc_flg 0x14)` | **`7/113 ab B777: 1..112,
  haelt 613 Bilder auf 112`**. Riegel auf meiner Spur (ohne die 80 Zeilen Aktor 1 aus 1011): `test_r37_roy spur … 1021` -> `OK (0 Fehler)`.
* Bilder `k_abn/m1021_blatt1.png`: Roy (Weste "POLICE") steht im Gang (B300-600), geht auf John zu (B650-775), sinkt in die Knie und
  faellt nach vorn (B800-900), liegt (B950-1500).

### 3.3 Gegenprobe VORHER (erklaert der Fix den Befund?)
exe ohne Spur K (Kopie der exe aus `r37_kiste`, Code-Diff dort nur Inventar; master hat seit 3feb3a78 keine Aenderung an den vier
beruehrten Dateien — `git log 3feb3a78..master -- <4 Dateien>` leer), Lauf identisch M1 (`k_abn/v1011`):
`[elliot] PL05 loaded … (Raum 1011, Spawn 0x47)`, `[elliot] Raum-RBJ-Overlay: 10 clips`, `[enemy-diag] actor1 type=0x47 model=elliot …
clips=10`. Spur: Clip 5 = **20** Bilder in Schleife (Elzas rec0), Clip 7 = 20 mit Uhr 1, **Clip 10 -> Clip 0 (1 Bild)**, Clip 9 = 20
Bilder, haelt Bild 19 (aufrecht). `test_r37_roy spur v1011/anim.log 1011` -> `FEHLER (4 Fehler)` (201 Klemmungen, 1288 Uhr!=Bild,
2401 falsche Bildzahl, Clip 9 endet auf 19) — der Riegel misst den Befund. Bilder `k_abn/k_abn_vorher_nachher_1011.png`: vorher Elliot
(hellblaues Hemd, graue Hose), kniet/steht, bei "*groans*" aufrecht; nachher Roy in Uniform, sitzt, liegt.
=> Der Fix erklaert alle drei Teilbefunde: falsches Modell (ELLIOT.* = CDEMD0), falsche Animationen (Elzas rec0 statt Roys rec1;
CDEMD0-Baenke in 1021), "wiederholen sich staendig" = Elzas 20-Bilder-Geste als Inhalt der Ruhe-Schleife Clip 5.

### 3.4 Nebenwirkung Leon (Elliot ROOM1170) — selbst gemessen
Leon-Neuspiel -> Montage -> Tuer -> 1170 (`RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_ANIM_TRACE RE15_EXIT_AT=1200#1170`), exe vorher (ohne K)
und r37/roy: beide `[elliot] PL05 loaded … (Raum 1170, Spawn 0x47)`, `model=elliot … clips=6`, kein `[roy]`;
Spur Typ 0x47: 317 Zeilen, **md5 beider Spuren gleich (3677cfe2…)** -> Elliot bytegleich unveraendert.

### 3.5 Wiederbetreten ROOM1011 (Roy liegt) — Schleife sichtbar?
Karte `1011 1500 6500 2048 4:0xF1 4:0xFA 3:0x74`, CONTINUE, `RE15_FORCE_CUT=1 RE15_EXIT_AT=70#1011 RE15_FRAMEDUMP=40-47/1:f_` (Lauf `k_abn/r1011`):
`[roy] … CDEMD1.EMS Sektor 1639`, `motion=13 … clips=14`; Spur: Clip 13 (2 Bilder, Skript-Schleife sub05) wechselt 0/1 je Bild;
Pixelvergleich aufeinanderfolgender Bilder B40..B47: **0 Pixel geaendert** (Schwelle 30) — keine sichtbare Wiederholung, Roy liegt
ruhig (Bild `k_abn/r1011_b44.png`).

## 4. Code-Diff gelesen (Befund -> Mechanik -> Fix)

* `roy_r37.c` / `re15_roy_r37.h`: `re15_ems_archiv_aktiv()` = `re15_char_variant()` (vorhandener Port-Zwilling des Elza-Bits, TITLE.BIN
  @0x801024a8/@0x801024e4); `re15_ems_toc()` Index `type-0x10` (@0x80022338 `addiu a0,v0,-16`), 8 B je Eintrag (`sll v1,a0,3`), 64 Eintraege
  bis zum Byte-Array @0x80073138/@0x80073378; `re15_typ47_ist_elliot()` = Typ 0x47 UND Leon-Archiv; `re15_roy_aus_elza_archiv()` = Typ 0x47
  UND Elza-Archiv.
* `roy_pc.c`: liest NUR fuer Elzas 0x47 den Blob aus `EMD/CDEMD1.EMS` @Sektor*2048 (TOC), sonst NULL -> bisheriger Weg unveraendert.
* Haken (je 1-3 Zeilen, Kommentar "Runde 37 Spur K"): `main.c` pc_enemy_read_re15_emd (zuerst Roy-Lader), Elliot-Lader an 2 Stellen nur
  fuer Leons 0x47; `anim_select_common.c` Elliot-Zweig / allgemeiner Gegner-Zweig; `enemy_ai_common.c` re15_npc_channel_anim (Elliot-Gestenbank);
  `enemy_common.c` rbj_resolve_slot (Ausnahme O3).
* Leon-Pfad: bei character 0 ist `re15_typ47_ist_elliot(t)` == alte Bedingung `t == 0x47` (praedikatsgleich) — gemessen bestaetigt (§3.4).
* Weitere Typ-0x47-Stellen im Code (grep `0x47` engine/platform): actor_locomotion.c:277/357 (Walker; laut Kommentar seit 2026-08-09 nur
  noch vom Spieler erreicht — Roys Gang laeuft gemessen ueber die Sub-VM, Clip 5 = 61 aus der eigenen Bank), Familienlisten 0x40-0x4d
  (game_step_common.c:950, player_common.c:1431, enemy_ai_common.c:14745, scd_vm.c:3712 — typbasiert wie das Original @0x80072bac),
  Render-Zweig `is_elliot = (av.mesh == &elliot_md1)` folgt der neuen Weiche. Keine Stelle behandelt Elzas 0x47 noch als Elliot.
* Zensus Typ 0x47 (scd_dump_room.py ueber alle 240 RDT, selbst gefahren): ROOM1011 @0x009F6/@0x00A3E, ROOM1021 @0x01E0A, ROOM1170 @0x017D2
  — sonst keiner. Die Weiche trifft also genau Roy (Elza) und Elliot (Leon).

## 5. Vertrag / Gates

| Gate | Ergebnis | Beleg |
|---|---|---|
| RE-Gate (Konstanten @0x, 3+ Stichproben) | haelt | §2: 5 Stellen selbst disassembliert, TOC bytegleich EXE, RAM == CDEMD1 |
| Guess-Tells im Diff | keine | grep der `+`-Zeilen (deferred/tunable/interim/for now/faithful/plausibel/TODO) leer |
| Fix erklaert Befund | ja | §3.3 Vorher-Lauf: Elliot-Modell + rec0 (Clip 5 = 20, Clip 10 -> 0, Clip 9 = 20) — Riegel faellt mit 4 Fehlern |
| Pfade | ok | `git diff master...HEAD --name-only`: kein release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists |
| Gemeinsame Dateien | ok | main.c/anim_select_common.c/enemy_ai_common.c/enemy_common.c je 1-3 Zeilen mit "Runde 37 Spur K" |
| Bank-9-Bits / Msg-Ids / Ereignisse / Slots | keine belegt | Diff enthaelt keine Flag-/Text-/Ereignis-Aenderung |
| Tests | 8 neue, messend | probes/r37_roy.cmake (GLOB), Integration mit `cmake_minimum_required(VERSION 3.16)`, Messkopie der exe; Gegenprobe vorher FAELLT (§3.3) |
| Gezielte Regression | 117/117 gruen | §1b (Obermenge der 93 Tests des Bau-Agenten), Spur-Tests 8/8 (§1) |
| Assets | keine neuen | CDEMD1.EMS ist Original (md5 a3ff26b3…, == info/Re1.5/PSX, == Disc) |
| Nebenwirkung Leon/Elliot 1170 | keine | §3.4 Spur md5-gleich vorher/nachher |
| Neue Quelldateien | Hinweis | engine/src/roy_r37.c, platform/pc/src/roy_pc.c: Android-Configure frisch (GLOB-Cache), RE15_MIN_TESTS +8 |

## 6. Urteil je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg (selbst gemessen) |
|---|---|---|
| K1 "weder das Modell von Roy …" | **erfuellt** | Original: Roys Baenke im RAM aller vier Elza-Staende == CDEMD1 (== CDEMD0: False), Lader FUN_80022300 `bltz` @0x80022334 -> Datei 0x27 (LBA 3627 = CDEMD1.EMS laut ISO). Port: `[roy] Typ 0x47 aus EMD/CDEMD1.EMS Sektor 1639 (142272 B)` in 1011 UND 1021 (M1/M2), kein `[elliot]`, `model=enemy-bank`; Bilder: RPD-Uniform statt Elliots hellblauem Hemd (§3.3 vorher/nachher). |
| K2 "… noch all seine Animationen" | **erfuellt** (Hinweis H1) | 1011: Kanal = rec1 (14 Clips; Original-RAM +0x180/+0x184 -> rec1), jeder Clip mit Original-Bildzahl (5/28, 6/78, 7/30, 9/100, 10/60, 13/2), Abfolge == sub02 Clip fuer Clip, 0 Klemmungen, Bild == Uhr. 1021 (echter Weg): 23/30 Schleife, Gang 5/61 eigene Bank (Sub 4 @0x800511dc/@0x800512bc), Sturz 7/113. Wiederbetreten 1011: Clip 13 (liegt). |
| K3 "… wiederholen sich … staendig" | **erfuellt** | Einmal-Clips halten wie das Original (Phase 2 ohne anim_set): 1011 Clip 9 haelt Bild 99 (487 Bilder, Original-RAM Phase 2 -> Bild 99), 1021 Clip 7 haelt Bild 112 (613 Bilder), Gesten vor/rueck halten am Ende. Geschleift wird nur, wo das Skript Plc_flg 4 setzt (Clip 5/13/23 — Original-RAM +0x1c4 0x24/0x04), und Clip 5 zeigt jetzt Roys Ruhe-Pose (sitzend, Hand an der Wunde) statt Elzas 20-Bilder-Knie-Greif-Geste (= die gemeldete Dauerschleife, §3.3). |

## 7. Maengel

Keine Maengel der Spur K gefunden.

## 8. Hinweise (keine Maengel der Spur K, an den Orchestrator)

* **H1 (allgemein, vorbestehend; Dossier OFFEN 4)**: Executor-Sub 0/1 des Ports zeigt im ersten Takt nach Plc_motion Bild 1 statt 0
  (vorwaerts) bzw. fc-2 statt fc-1 (rueckwaerts). Original selbst geprueft: Phase 0 `80050d0c sb zero,149(v0)` (+0x95 = 0), dann
  anim_set @0x8001f314: Index aus +0x95 (rueckwaerts `subu v0,t1,v0` / `addiu v0,v0,-1`), `8001f36c sw a2,360(t0)` (+0x168 = Pose) VOR dem
  Vorschub -> erstes Bild 0 bzw. fc-1. Port-Spur: `7/30 vor 1..29`, `rueck 28..0`, `10/60 vor 1..50, rueck 58..9`. 1 Takt (33 ms) Phase, durch
  die 7-Bild-Ueberblendung (+0x8f = 7 @0x80050cfc) kaum sichtbar; betrifft alle NPC-Gesten (Elliot, Marvin, Ada, Irons, John), nicht durch K
  eingefuehrt -> eigene Spur.
* **H2 (Dossier OFFEN 1, master-Plan "Spur N")**: FUN_80022300 waehlt das Archiv fuer JEDEN Typ nach dem Elza-Bit (kein Typ-Test vor
  `bltz` @0x80022334, selbst gesehen); der Port laedt bei Elza ausser Roy weiter CDEMD0 (TOC-Laengen unterscheiden sich z.B. Typ 0x10
  0x345e0 / 0x34720). Infrastruktur (re15_ems_toc) steht.
* **H3**: integration_r37_roy_szene1021 startet die Szene per `RE15_GOTO_ROOM=1021` + `RE15_FORCE_EVENT=2`; mein echter Weg (M2: Tuer
  1011 -> 1021, Hineinlaufen in AOT 5) liefert dasselbe Ergebnis. Kein Mangel, nur Anmerkung.
* **H4 (Leon, vorbestehend, bytegleich vorher/nachher)**: Elliots Geste Clip 15 in 1170 — Bild aus dem REC0-Overlay (20 Bilder), Uhr 1
  (O3-Ausnahme Runde 35) -> Spur `15/20 vor 0..9 + 83 halten, rueck 19..10 + 164 halten`; nicht Auftrag dieser Spur (Elliot unveraendert).
* **H5 (Nutzer-Beobachtung aus dem Workflow-Aufruf, woertlich)**: "bei deinen aktuellen testlauf in STAGE2 sieht man, das leon den
  elevator nicht verlassen kann und irgendwo in der Luft rumlaufen kann...." — NICHT Spur K: alle vier Haken sind an Typ 0x47 gebunden,
  roy_pc liefert fuer jeden anderen Typ NULL, Typ 0x47 kommt in STAGE2 nicht vor (Zensus §4), K-Tests laufen nur in 1011/1021/1170. Zur
  Messzeit lief laut Prozessliste `re15_pc_r37d_lift_*` (Spur D, "Lift A 2030 nur aus der Kabine", master 593fc086) — Beobachtung an
  Spur D/C weitergeben.

## 9. Transparenz (Zugriffe ausserhalb des Baums, nur lesend)

* Vorher-exe = Kopie von `.claude/worktrees/r37_kiste/re15_port/build/platform/pc/re15_pc.exe` (nur gelesen, md5 0d2d33b0…), als
  Messkopie in MEINEM build/ gestartet und danach geloescht (kein master-Bau ohne K vorhanden; Hauptbaum-exe bewusst NICHT gestartet,
  weil die exe `befund.log` neben sich schreibt/rotiert — das ist die F9-Datei des Nutzers).
* Nutzer-Spielstaende (DuckStation) und die MZD-Disc nur gelesen (md5 vorher == Dossier), nichts geschrieben.
* TOC-Generator `tools/r37_k/gen_ems_toc_r37.py` mit umgelenktem OUT in den scratchpad ausgefuehrt: Ausgabe == eingecheckte
  `gen/ems_toc_r37.inc` (cmp gleich, Byte-Anker-asserts gruen).

