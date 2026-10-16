# Runde 38 — Spur E "elzaintro": Abnahme 0 (unabhaengig, fortlaufend)

Baum `.claude/worktrees/r38_elzaintro`, Zweig `r38/elzaintro`, Kopf b8d361ea, Merge-Basis 1bcbd0ff. Abnahme 2026-10-16 ab 18:00.
Grundsatz: nur was hier selbst am gebauten Stand gemessen ist, zaehlt. Messordner (Scratchpad, nicht versioniert), kurz `SP` =
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`:
`SP/abn/a1_{leon,elza}` (Ton + Log) und `SP/e_abn0/` (Werkzeuge `tools/`, Laeufe `m2_*`, `n_*`, `alt_*`).
Laeufe mit eigenem Laeufer `SP/e_abn0/abn_lauf.sh` (Messkopie `re15_pc_abn0e_<pid>.exe` neben der exe, Ende per
`RE15_EXIT_AT`, nie /IM): Neues Spiel ueber Titel + Charakterwahl (`RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1`, Elza zusaetzlich `RE15_PSELECT_AUTO_SWITCH=1`), `RE15_EVT_TRACE=1 RE15_MSG_LOG=1`.

## 0. Stand und Bau
- `git diff master...HEAD --name-only` (Drei-Punkt gegen die Merge-Basis; der Zwei-Punkt-Diff zeigt die master-Commits nach
  1bcbd0ff faelschlich als Loeschungen von `r38_w2.js`/`UEBERGABE.md`/`mk_r38.py` — die gehoeren NICHT zur Spur): 23 Dateien.
  Code: `engine/src/elzaintro_r38.c` (neu), `include/re15_elzaintro_r38.h` (neu), Haken `platform/pc/main.c` (Include + 1 Zeile),
  `platform/pc/src/audio_pc.c` (Include + 4 Zeilen), `platform/pc/src/render_pc.c` (nur Kommentar); Tests `tests/unit/probes/
  r38_elzaintro.cmake`, `tests/unit/test_r38_elzaintro.c`, `tests/unit/probe_r38_elzaintro_auswertung.c`,
  `tests/integration/test_r38_elzaintro.cmake`; `synchro/README.md` (4 Zeilen); Dossier, Bilder, Werkzeug.
- Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (build)`, `ninja: no work to do`
  (exe 17:02 = Quellstand 3c0ca636; Arbeitsbaum sauber; exe enthaelt `[voice] ROOM%04X msg %d <- %s (Runde 38 Spur E)`).

## 1. Punkt 1 — "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon."
### Messung (gebauter Stand)
- Lauf `SP/abn/a1_elza` (`RE15_AUDIO_CAP_SYNC=cap.raw RE15_SCD_TRACE=1 RE15_EXIT_AT=950#1031`, rc=0, 105 s) und
  `SP/abn/a1_leon` (`...#1170`, rc=0, 102 s). debug.log Elza: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, dann
  fuer N = 0..5 je `[voice] ROOM1241 msg N <- synchro/STAGE1/room1240/main0N.wav (Runde 38 Spur E)` +
  `[voice] clip loaded: main0N.wav (48000 Hz x2 -> ... @44100 Hz)` — dieselben sechs Clips mit denselben Laengen
  (248062/235200/259087/288487/308700/242550) wie Leon (`[pl] ... PL00`, `clip loaded: main00..05.wav`, KEINE Ersatzzeile).
- Ton: `SP/e_abn0/tools/cap_cmp.py` richtet die beiden Mitschnitte aus (beste Verschiebung 13 Takte = Elzas kuerzerer Vorlauf
  im Titel) und vergleicht Takt fuer Takt (1470 Stereo-Frames je Spielbild): **Leon-Takte 13..2681 = 2669 Takte
  BITGLEICH** (Titel + Charakterwahl-Rest + Montage 1432 Bilder + Erzaehler 446 Bilder), ab Takt 2682 verschieden = ab dem
  Ende des Erzaehlers (Leon Helipad-Szene, Elza Lobby-Szene). Elzas Montage klingt damit exakt wie Leons, Stimme inklusive.
- Ablauf: Montage-Uebergabe bei Elza in F1431 (Leon F1431), msg 5 parkt wie bei Leon (Montage-Lauf `SP/e_abn0/m2_*`).
- Bild: Montage Bild fuer Bild (`m2_leon`/`m2_elza`, `RE15_AUDIO_CAP_SYNC`, `RE15_FRAMEDUMP=0-1431/1`, `RE15_EXIT_AT=1428#124x`):
  `frames_cmp.py` -> **1429 von 1429 Bildern (F0..F1428) pixelgleich** (inkl. Untertitel-Dauer).
- Gegenprobe vorher: siehe Abschnitt 4.
- Lesart "Voiceover" (Fortsetzung): Leons Erzaehler nach der Montage (ROOM1170 sub11 Message_on 8..11 @0x016AC..@0x016C4)
  hat KEINE Aufnahme (`synchro/STAGE1/room1170/` = main00..07, 13, 14, 16); Elzas Erzaehler (ROOM1031 sub15 Message_on
  0x14..0x17 @0x02A7A..@0x02A92) ebenfalls nicht (`room1031/` = main18, main19 = Elzas eigene Zeilen). Das Voiceover,
  das bei Leon laeuft und bei Elza fehlte, ist also genau die Montage-Erzaehlung msg 0..5 — die ist jetzt da.
### Beleg / RE-Gate
- Zuordnung ROOM1241 msg 0..5 -> `synchro/STAGE1/room1240` ist NUTZER-VORGABE ("Es ist das gleiche wie bei Leon"), im Code
  so gekennzeichnet (`elzaintro_r38.c` k_stimme, Header). Form selbst nachgelesen (Python ueber die ausgelieferten RDT,
  `SP/e_abn0/tools/rdt_bytes.py`; `info/Re1.5/PSX` = `shared_assets/PSX`, sha1 je Datei gleich):
  RDT+0x3C = 0x62C (1240) / 0x630 (1241); Offsettabellen beider `[12, 105, 191, 278, 370, 484]`; msg0..5 byte-gleich
  (92/85/86/91/113/71 Byte; msg0 @0x638 / @0x63C `04 00 1d 00 3e 45 56 3d`); Message_on `2b 0N 00 00` ROOM1240
  @0x057E/0x0598/0x05A0/0x05BA/0x05DA/0x05FA, ROOM1241 je +4; @0x055A 1240 `22 03 8b 01`, 1241 `22 03 6f 01 22 03 70 01`;
  Tuerziel @0x0531 = 0x17 (1240) / 0x03 (1241). Alles wie zitiert.
- ROOM1241 = Elzas Variante: PSX.EXE selbst disassembliert (`re15_disasm.py dis 0x80039780`): @0x800397b8 `lw a0,0x800aca3c`,
  @0x800397e4 `srl a0,a0,31`, @0x800397e0 `lhu v0,0(v1)` (Dateinummer aus Tabelle @0x8007438c[stage][room]),
  @0x800397ec `addu a0,a0,v0` im Delay-Slot von `jal 0x80013b60` -> Datei = Leon-Datei + Bit 31 = ROOMxxx1.

## 2. Punkt 2 — "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza"
### Messung (gebauter Stand)
- Erzaehler: Laeufe `SP/e_abn0/n_leon` / `n_elza` (`RE15_NOAUDIO=1`, `RE15_FRAMEDUMP=0-1431/1:f_`, `RE15_EXIT_AT=950#1170` bzw.
  `950#1031`, rc=0). Elza-Log: `[room] PC loaded room1031.rdt`, `[evt] F1421 room=1031 Evt_exec sub=15`, `[scd F1421]
  Cut_chg(13)`, `[kartenitems] ROOM1031: Karte Blatt 2 (Bit 9,89) Prop obj 15 bei (-1692,-2769,-23638)` (der Lageplan IST
  angelegt), Selbst-Tuer `[evt] F445 room=1031 Evt_exec sub=11`, `[scd F445] Cut_chg(4)`.
- `frames_cmp.py n_leon n_elza 0 470`: **F0..F445 (446 Erzaehlerbilder) Leon = Elza pixelgleich**, erster Unterschied F446
  (Szenen-Einblendung Lobby gegen Helipad, erwartet).
- `bbox.py n_elza 0 445`: Huelle ALLER Pixel != (0,0,0) ueber alle 446 Bilder = x 34..227, y 181..211 (das Untertitelfeld),
  max. Wert 216; Kartenfeld x 91..98 / y 107..112 in **0 von 446** Bildern != 0. F0..F12 und F436..F445 vollstaendig (0,0,0).
  Bild `SP/e_abn0/n_elza_f200.png` (nur "Where we thought it would be safe." auf Schwarz).
- Lageplan in der Szene weiter da: `SP/e_abn0/n_elza_f900.png` (Lobby Cut 0, "Elza: Hey, is anyone here?!", Plan an der Wand),
  Kartenfeld F450..F470 = 48 helle Pixel.
- Montage (das andere Schwarz mit Text des Vorspanns; im Projekt heisst die ROOM1240-Montage "Pre-Intro", Memory
  reai-v2-preintro-re2): s. Punkt 1, Leon = Elza 1429/1429 pixelgleich. ZUSAETZLICH absolut (Fortsetzung, Bilder
  `SP/e_abn0/m2_elza`): die Text-auf-Schwarz-Abschnitte F0..F161 und F1194..F1263 haben ausserhalb des Untertitelbands
  y 176..215 **kein einziges Pixel > 1**; F162..171, F1190..1193, F1264..1268 sind Ueberblendungen der benachbarten
  Standbilder, F792..864 ist das S.T.A.R.S.-Standbild (Ansicht `SP/e_abn0/montage_dunkel_100_800_1200.png`: F100 schwarz,
  F800 Logo x4, F1200 "But the umbrella corporation's experiments were far from finished." auf Schwarz). In Elzas
  Vorspann scheint damit in KEINEM Schwarzbild mit Text etwas durch.
- Gegenprobe vorher: siehe Abschnitt 4.
### Beleg / RE-Gate
- Selbst gelesen (`rdt_bytes.py`, `scd_dump_room.py` -> `SP/e_abn0/scd_1031.txt`, `scd_1170.txt`): ROOM1031 sub15 @0x02A68
  `22 03 7d 01` Set(3,125,1), @0x02A6C `22 03 cf 01`, @0x02A70 `22 02 07 01`, **@0x02A74 `29 0d` Cut_chg(13)**, Sleep 20,
  Message_on 0x14..0x17 mit Sleep 100, Aot_on 0x13 — einziges Cut_chg(13) im Raum (alle Cut_chg: 0c/0b/05/04/06/00/0d);
  Cut_replace nur 0->9, 3->10, 4->11, 6->12; RVD ab 0x228, 37 Saetze, Ziel-Byte 3 nur 0..9 (der Satz @0x4F8
  `01 ff 0d 00 ...` fuehrt AUS Cut 13 nach 0). ROOM1170 sub11 **@0x016A6 `29 07`**, gleich gebaut (Message_on 8..11).
  Kamera RDT+0x24 = 0x60: ROOM1031 Cut 0 @0x60 = Cut 13 @0x200 = ROOM1170 Cut 7 @0x140 = (-12834,-3114,-9774)/(-7794,-2196,-22446).
  ROOM1031 @0x4F8 = ROOM1170 @0x370 bis auf das Schnittbyte. BSS BG13 (1031) = BG07 (1170) = BG00 (1240), sha256 44cafcd7ff20b961.
- main00-Zweig dynamisch bestaetigt (`RE15_SCD_TRACE`, a1_elza): `@0x1E50 op=0x06`, `@0x1E54 op=0x21` -> Sprung nach @0x2014
  (die Sce_em_set/Obj_model_set-Strecke ab @0x01E5C wird uebersprungen), `@0x207E op=0x21` -> `@0x2082 op=0x3B` ->
  `@0x20A2 op=0x04` -> `Evt_exec sub=15`. Bytes @0x01E50 `06 00 c0 01`, @0x01E54 `21 03 7d 01`, @0x0207E `21 03 7d 00`,
  @0x02082 `3b 13 02 31`, @0x020A2 `04 ff 18 0f` wie zitiert.

## 3. RE-Gate und Code (Fortsetzung 2026-10-16 ab 21:00, nach dem Sitzungslimit)
Bau erneut bestaetigt: `local_build.sh configure` + `build` -> `LOCAL-BUILD-OK`, `ninja: no work to do`; exe 17:02:41
(seit 3c0ca636 aenderte sich nur `tests/integration/test_r38_elzaintro.cmake`, kein Engine-/Plattform-Code).
### 3.1 Code-Diff (gelesen, `git diff master...HEAD -- re15_port synchro`)
- `elzaintro_r38.c`: zwei Tabellen. `k_stimme` = genau eine Zeile (0x1241, msg 0..5) -> 0x1240, als NUTZER-VORGABE
  gekennzeichnet; `k_erzaehler` = (0x1170, 7, @0x016A6) + (0x1031, 13, @0x02A74). Keine Zahl ohne Offset/Kennzeichnung.
- `main.c`: Include + die eine Zeile `re15_render_pc_set_scene_black(re15_elzaintro_r38_erzaehler_schwarz(...))`
  statt `g_current_room_id == 0x1170 && active_cut_idx == 7` -> fuer Leon dieselbe Bedingung.
- `audio_pc.c` `re15_voice_load_clip`: Include + 4 Zeilen, greift nur bei `!wav` NACH beiden eigenen Versuchen
  (Gross-/Klein-Hex) -> eigene `room1241`-Dateien haetten Vorrang; trifft nur (0x1241, 0..5).
- `render_pc.c`: nur Kommentar (Netz nicht loeschen). `synchro/README.md`: 4 Zeilen Doku.
- Guess-Tell-Suche in den hinzugefuegten Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|
  ungefaehr|geschaetzt|vermutlich|sieht richtig`): **0 Treffer**. Neue `getenv`-Schalter: **0**.
### 3.2 Stichprobe EXE selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)
Der Code-Diff zitiert nur RDT-Datei-Offsets; geprueft habe ich die EXE-Mechanismen, auf denen diese Belege ruhen:
1. **SCD-Sprungtabelle @0x800744a8** (Kontrolle: Eintrag 0x38 -> 0x800417ac = Flr_set wie in scd_vm.c):
   Eintrag 0x29 @0x8007454c -> **0x800402a0**, Eintrag 0x2B @0x80074554 -> **0x800404f4**, Eintrag 0x3B @0x80074594 ->
   **0x800405bc**.
2. **Cut_chg 0x800402a0**: `800402b8 lw v0,28(s0)` (Skript-pc), `800402c4 lbu a0,1(v0)` (= pc[1]), `800402fc sh a0,
   4068(at)` -> 0x800b0fe4 (Schnitt), `80040310 addiu v1,v1,2` (Laenge 2). -> `29 0d` = Cut_chg(13), `29 07` = Cut_chg(7).
3. **Message_on 0x800404f4**: `80040504 lbu a2,1(v0)` (Nachrichten-Nr. = pc[1]), `80040508 lhu a3,2(v0)`,
   `8004050c addiu v0,v0,4` (Laenge 4), `80040518 jal 0x80027e68`. -> `2b 0N 00 00` = Nachricht N (Punkt 1).
4. **Door_aot_set 0x800405bc**: `80040618 lbu v0,3(v1)`, `80040620 andi v0,v0,0x80`, `80040630 addiu v0,v1,40` /
   `80040634 addiu v0,v1,32` -> 32/40 Byte (Grundlage meines Tuer-Scans 3.3).
5. (Teil 1) Elza-Raum = Leon-Raum + Bit 31 @0x800397e4/@0x800397ec (Abschnitt 1).
### 3.3 Nebenwirkung des Netzes: kann ROOM1031 Cut 13 ausserhalb des Erzaehlers entstehen?
Das Netz schwaerzt (0x1031, 13) vollflaechig unter dem Text. Wuerde Cut 13 auch bei einem normalen Lobby-Eintritt
aktiv, waere das eine Regression. Geprueft (eigenes Werkzeug `SP/e_abn0/tools/tueren_nach.py`, Laengen aus
`scd_dump_room.py`):
- **Tueren**: alle 653 `Door_aot_set` aller RDTs; die 8 mit Ziel Stage 0 / Raum-Byte 0x03 (1030/1031) haben Ziel-Schnitt
  0, 2, 6, 7, 8 — **nie 13** (ROOM1031 main00 @0x02082 -> 6, 1040/1041 @0x010B8 -> 8, 1050/1051 @0x00ADA -> 2,
  1070/1071 @0x0145E -> 7, 1241 @0x0051A -> 0). Rohe Bytesuche ohne Walk (`3b ?? 02 ..`, Ziel 00 03 0d bei +22/+30):
  **0 Treffer**. Gegenprobe Leon: Tueren nach Raum-Byte 0x17 (1170/1171) -> Schnitte 0, 3, 8, 9, 11, nie 7.
- **RVD**: der Port setzt bei Zonen `cam_id = event_id` = Ziel-Byte 3 (aot_common.c 1570/1651); in ROOM1031 fuehrt
  keine Zone nach 13 (Riegel + Abschnitt 2).
- **Port-Code**: alle Schreiber von `g_scd.cam_id` (aot_common.c 762/1570/1651, game_step_common.c 2232,
  scd_room_setup.c 435, scd_vm.c 1481/1742, main.c 3034/4675/4973/8419) — keiner setzt 13 fuer ROOM1031 ausser dem
  SCD-Cut_chg; kein Port-Modul nennt ROOM1031 zusammen mit einem Schnitt.
- ROOM1031 @0x01E50..0x02014 (Ifel_ck-Block, `06 00 c0 01`): 20x Sce_em_set ab @0x01E5C, das einzige Obj_model_set des
  Raums @0x01FEC — wie im Header zitiert.
Ergebnis: Cut 13 gibt es in ROOM1031 nur im Erzaehler; das Netz trifft keinen normalen Spielmoment.

(Abschnitte 4-6 folgen)
