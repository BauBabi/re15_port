# Runde 38 — Spur E "elzaintro": Abnahme 0 (unabhaengig, fortlaufend)

Baum `.claude/worktrees/r38_elzaintro`, Zweig `r38/elzaintro`, geprueft Code-Stand b8d361ea, Merge-Basis 1bcbd0ff.
Abnahme 2026-10-16 18:00-18:18 (Teilbericht e1f0b96c, Sitzungslimit), fortgesetzt 21:00-22:05.
**Ergebnis: BESTANDEN — beide Punkte erfuellt, alle Gates halten, keine Maengel (Abschnitt 6).**
Grundsatz: nur was hier selbst am gebauten Stand gemessen ist, zaehlt. Messordner (Scratchpad, nicht versioniert), kurz `SP` =
`C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`:
`SP/abn/a1_{leon,elza}` (Ton + Log) und `SP/e_abn0/` (Werkzeuge `tools/`, Laeufe `m2_*`, `n_*`, `g_*` = v0.8.25,
`riegel_v0825/`). Bilder = `RE15_FRAMEDUMP` (render_pc.c: `SDL_RenderReadPixels` des beschleunigten Renderers unmittelbar
vor `SDL_RenderPresent`, also NACH Netz s_scene_black, Text und Fade = das praesentierte Bild; kein SOFTWARE_RENDER/AUTOSHOT).
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

## 4. Gegenprobe am ausgelieferten Stand v0.8.25 (Ursache, Leon unveraendert)
exe = `release/pkg-win/re15_port_v0.8.25/re15_pc.exe` (14.10. 10:31, Release-Commit 2077bb8e, sha256 9cd642a2a92f6ce614d3...),
als `re15_pc_v0825_abn0e.exe` in den Build-Ordner des Spurbaums kopiert (Assets/synchro des Baums: zwischen 2077bb8e und
der Basis 1bcbd0ff kamen ausserhalb analysis/ nur Sprachdateien der Raeume 1050/10B1/10F0/1100/1150/1171/11B0/11B1/11C0
dazu, nichts fuer 1240/1241/1031/1170). Laeufe `SP/e_abn0/g_leon`, `g_elza` (`RE15_AUDIO_CAP_SYNC`, `RE15_FRAMEDUMP=
0-445/1`, `RE15_EXIT_AT=446#1170` bzw. `446#1031`), `g_elza_flag` (`RE15_NOAUDIO=1 RE15_SET_FLAG=9:89`, F200,
`210#1031`); alle rc=0. Auswertung `SP/e_abn0/tools/gegenprobe.py` -> `SP/e_abn0/gegenprobe.txt`.
(Die alt_*-Laeufe der ersten Teilsitzung hatten keine benannte exe; sie sind hiermit ersetzt.)

| Messung | v0.8.25 Leon | v0.8.25 Elza | Spurbau Leon | Spurbau Elza |
|---|---|---|---|---|
| `[voice] clip loaded` (Montage) | main00..05 | **0** | main00..05 | main00..05 (6 Ersatzzeilen room1240) |
| Erzaehler F0..F445: Bilder mit hellem Kartenfeld x91..98/y107..112 | 0 | **442 (ab F3)** | 0 | **0** |
| Erzaehler F0..F445: Huelle aller Pixel != 0 | x34..227 y181..211 (Text) | **x0..319 y0..239** | x34..227 y181..211 | **x34..227 y181..211** |
| `[kartenitems] ROOM1031` | - | 2 | - | angelegt (Abschnitt 2) |

- **Punkt 1 vorher bestaetigt:** Elza laedt am ausgelieferten Stand keinen einzigen Montage-Clip. Ton v0.8.25 Leon gegen
  v0.8.25 Elza: bitgleich bis zum ersten Message_on, dann abwechselnd ANDERS 169/160/177/194/210 Takte (= die Clips) und
  GLEICH in den Pausen. Ton v0.8.25 Elza gegen Spurbau Elza (`SP/abn/a1_elza`): GLEICH bis Takt 975, danach ANDERS genau in
  denselben Clip-Strecken (169 = main00 248062 Samples / 1470, 160 = main01 235200 / 1470, 177 = main02, 210 = main04).
  Der Fix fuegt also genau die Stimme hinzu und sonst nichts.
- **Punkt 2 vorher bestaetigt und Ursache belegt:** am ausgelieferten Stand steht das Kartenfeld in 442 von 446
  Erzaehlerbildern; mit `RE15_SET_FLAG=9:89` (Lageplan 1F gilt als genommen -> `re15_kartenitems_r36_install` legt kein
  Prop an; Log: ROOM1031 geladen, `Evt_exec sub=15`, `Cut_chg(13)`, KEINE `[kartenitems]`-Zeile) ist das Feld in F200
  dunkel. Das durchscheinende Feld IST der Lageplan-Prop (Runde 36 Spur N); das passt zum Bisect des Bau-Agenten
  (erstmals ab cb1d5f59 / v0.8.24). Der (1,1,1)-Grund (Huelle ganzer Schirm) faellt mit dem Netz ebenfalls weg.
- **Leon unveraendert:** Erzaehler F0..F445 v0.8.25 Leon gegen Spurbau Leon (`n_leon`): **446/446 pixelgleich**; Ton
  v0.8.25 Leon gegen Spurbau Leon (`SP/abn/a1_leon`): **2614 von 2614 Takten bitgleich** (Titel, Charakterwahl, Montage,
  Erzaehler, Szenenbeginn).
- **Zeitliche Zuordnung (git):** `re15_port/engine/src/kartenitems_r36.c` fehlt in v0.8.23, ist in v0.8.24/v0.8.25 da; erster
  Commit 554877f9 (2026-10-11), Merge cb1d5f59 ist Vorfahre von v0.8.24, nicht von v0.8.23. Der genannte Verursacher erklaert
  den Befund (Lageplan-Prop = das Feld, Gegenprobe 9:89).
- **Riegel gegen v0.8.25 (beide ROT, wie verlangt):** `cmake -DRE15_PC_EXE=<v0.8.25-Kopie> -DRE15_AUSWERT_TOOL=<probe> -DWORKDIR=
  SP/e_abn0/riegel_v0825 -DTEIL=<teil> -P tests/integration/test_r38_elzaintro.cmake`:
  - `schwarz`: exit 1, "Erzaehler F60: 74507 Pixel Elza != Leon, Lageplan-Feld 48 hell (soll 0/0), Text 2341 Pixel".
  - `stimme`: exit 1, "msg 0: Elza-Ersatz 0x (soll 1), Leon-Clip 1x (soll 1)". Weil das die eigene Logzeile ist, habe ich
    die eigentliche Tonmessung auf denselben v0.8.25-Mitschnitten nachgerechnet (`probe_r38_elzaintro_auswertung rms ...
    1300 0 60 90 181 150 383 150 563 150 765 150 967 150 1180 110`): Leon 906/2817/3453/3435/3303/3612/3570, Elza
    906/**1348/2184/2141/1872/2424/2439** (0,48..0,71 von Leon -> ausserhalb +-10 %), msg 5 Aufrufe Leon 11 / Elza **1** ->
    auch diese Pruefungen waeren rot. Am Spurbau (mein gruener ctest-Lauf, `build/r38_elzaintro_wd/lauf_stimme_181870a6`):
    Elza = Leon = 906/2817/3453/3435/3303/3612/3570, msg 5 11/11.

## 5. Vertrag und Tests
### 5.1 Vertrag (VERTRAG.md)
- Pfade (`git diff master...HEAD --name-only` ohne analysis/): `engine/src/elzaintro_r38.c`, `include/re15_elzaintro_r38.h`,
  `platform/pc/main.c`, `platform/pc/src/audio_pc.c`, `platform/pc/src/render_pc.c`, `tests/integration/
  test_r38_elzaintro.cmake`, `tests/unit/probe_r38_elzaintro_auswertung.c`, `tests/unit/probes/r38_elzaintro.cmake`,
  `tests/unit/test_r38_elzaintro.c`, `synchro/README.md`. Kein `release/`, kein `platform/android/`, kein
  `shared_assets/PSX/`, keine CMakeLists (Registrierung ueber die GLOB-Sonde `probes/r38_elzaintro.cmake`).
- Gemeinsame Dateien nur Haken mit "Runde 38 Spur E": main.c +4/-1 (Include + 1 Zeile + 2 Kommentarzeilen), audio_pc.c +5
  (Include + 4), render_pc.c +3/-1 (Kommentar).
- Ressourcen: keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots, keine neuen Assets/Sprachdateien (der einzige
  Treffer auf "Bank-9" ist der Header-Kommentar zur Gegenprobe 9:89). Die Spur hatte laut VERTRAG §1 auch keine zugeteilt.
- `snprintf` in der neuen Engine-Datei: schon in 5 anderen `engine/src`-Dateien benutzt -> kein neues Risiko fuer den
  PSX-Bau. Hinweis Orchestrator (Dossier): neue Engine-Datei -> Android-GLOB-Cache neu konfigurieren.
### 5.2 Riegel je Punkt (gelesen)
- Punkt 1: `unit_r38_elzaintro_stimme` (Nachrichtenblock, Offsettabelle, msg 0..5 byte-gleich, Message_on-Offsets beider
  sub02, Abbildung genau (1241,0..5)->1240, Ersatzpfad, sechs WAVs) und `integration_r38_elzaintro_stimme` (echte exe, Neues
  Spiel Leon + Elza: sechs Ersatzzeilen nur bei Elza, msg 5 parkt gleich oft, **RMS je Zeilenfenster Elza = Leon +-10 %**
  aus dem RE15_AUDIO_CAP_SYNC-Mitschnitt — misst den Ton, nicht nur die eigene Logzeile).
- Punkt 2: `unit_r38_elzaintro_schwarz` (je Tabellenzeile `29 cut` am Offset = einziges Cut_chg im SCD per Opcode-Walk,
  keine RVD-Zone hinein, Kamera = ROOM1170 Cut 7, BG13 = BG07, Gate genau fuer die zwei Paare) und
  `integration_r38_elzaintro_schwarz` (echte exe: Erzaehlerbilder F60/F230/F400 Elza = Leon pixelgleich, Kartenfeld
  dunkel, Lageplan in ROOM1031 nachweislich angelegt, Text >= 100 Pixel).
- Gegenprobe: beide Integrationsriegel am ausgelieferten Stand v0.8.25 ROT (Abschnitt 4), am Spurbau gruen (5.3).
- Luecke (kein Mangel, selbst nachgeprueft in 3.3): kein Riegel prueft Tueren mit Ziel ROOM1031 Cut 13.
### 5.3 Gezielter Lauf (keine volle Suite)
`ctest --test-dir re15_port/build -R "<38 Tests>" --timeout 600 --output-on-failure`, Ausdruck in
`SP/e_abn0/ctest_regex2.txt`, Protokoll `SP/e_abn0/ctest_gezielt2.log`, 21:10:22..21:42:14, seriell:
die vier Spur-Tests + Sprachausgabe (unit_synchro_stimme, unit_voice_ausreden, unit_se_voice_machine,
unit_r17_untertitel_pin, unit_r30_tuer_stimme, unit_r34n_f_leiche_stimme, unit/integration_r36_elza_stoehnen) +
Erzaehler-Netz/ROOM1170 (unit_elliot_1170_run, unit_stair_1170_regression, unit_door_1170_lock, unit_tor_1170_anfrage,
unit/integration_r37_roy_leon1170, integration_boot_bg_pin, integration_dark_start_pin, integration_r30_cut_blitz,
unit_montage_fx) + Lobby ROOM1030/1031 (unit_cam_1030_reentry, unit_1030_spawncap/_crawl_live/_trigger_chain/
_stuck_who/_bandsweep, unit_r35_cut1150_montage_1030, unit_r36_gegnerzahl_raum1030/1031, unit_r30_elza_selbsttuer,
unit_r35_zgirl_selbsttueren, integration_elza_vollstart, integration_r36_story_elza) + Lageplan (unit_r36_kartenitems,
unit_r36_kartenbits, integration_r36_kartenitems_bild).
Ergebnis: **`100% tests passed, 0 tests failed out of 38`**, `Total Test time (real) = 1911.85 sec`
(integration_r38_elzaintro_stimme 125,34 s, _schwarz 164,73 s, integration_r36_kartenitems_bild 619,53 s,
integration_r36_story_elza 482,17 s, integration_elza_vollstart 104,85 s).

## 6. Urteil
| Punkt (Wortlaut) | Urteil | Beleg (kurz) |
|---|---|---|
| 1 "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon." | **erfuellt** | Spurbau: Elza laedt main00..05 aus `synchro/STAGE1/room1240` (6 Ersatzzeilen), Mitschnitt Titel..Erzaehler 2669 Takte bitgleich mit Leon, Tonfenster 906/2817/3453/3435/3303/3612/3570 = Leon, msg 5 parkt 11/11, Montage 1429/1429 Bilder = Leon. v0.8.25: 0 Clips, Tonfenster 0,48..0,71 von Leon. Form der Nutzervorgabe belegt (msg 0..5 byte-gleich @0x638/@0x63C.., Message_on @0x057E../+4, Handler 0x800404f4). |
| 2 "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza" | **erfuellt** | Spurbau: Erzaehler ROOM1031 Cut 13 F0..F445 — Huelle aller Pixel != 0 = Untertitelfeld x34..227/y181..211, Kartenfeld 0/446 (v0.8.25: 442/446, ganzer Schirm != 0); Montage-Schwarzbilder mit Text ohne ein Pixel > 1 ausserhalb des Textbands; Ursache = Lageplan-Prop (Gegenprobe 9:89, ab v0.8.24); Lageplan in der Lobby-Szene weiter sichtbar (F900). Original: Cut 13 nur ueber `29 0d` @0x02A74 (Handler 0x800402a0), keine Tuer/RVD-Zone hinein, BG13 = schwarzes Standbild, Objekte nur im Zweig Ck(3,125)==1. |

### Gates
- RE-Gate: haelt. Jede Konstante im Code mit Datei-Offset bzw. als NUTZER-VORGABE gekennzeichnet; alle zitierten Offsets
  selbst gelesen; vier EXE-Handler/-Stellen selbst disassembliert (3.2); keine Guess-Tells, keine neuen Env-Schalter;
  der Fix erklaert jeweils den gemessenen Befund (Punkt 1: fehlender Ordner room1241 -> Ersatz room1240, Differenz alt/neu
  = genau die Clips; Punkt 2: Lageplan-Prop im Erzaehlerbild -> Netz unter dem Text, Differenz = Feld + (1,1,1)-Grund).
- Vertrag: haelt (5.1). Tests: je Punkt Unit- + Integrationsriegel, messend, am alten Stand rot (4), gezielt 38/38 gruen (5.3).
- Nebenwirkungen: Leon Bild + Ton alt = neu (4); Netz trifft keinen normalen Lobby-Moment (3.3); Elza-Vollstart,
  Story-Lauf Elza, Lageplan-Bild, Roy/Leon 1170, Boot-/Dark-Start-Pins gruen.

### Maengel
Keine.

### Hinweise (nicht blockierend, keine Nachbesserung noetig)
1. Ein Riegel fuer "keine Tuer fuehrt in ROOM1031 Cut 13 / ROOM1170 Cut 7" fehlt; der Zustand ist heute selbst nachgeprueft
   (3.3: 653 Tueren, Ziel-Schnitte nach 1030/1031 nur 0/2/6/7/8). Wer das Netz spaeter erweitert, sollte ihn ergaenzen.
2. `integration_r38_elzaintro_stimme` bricht am alten Stand schon an der eigenen Logzeile ab; die Tonfenster waeren ebenfalls
   rot (Abschnitt 4 nachgerechnet) — die Messung traegt also.
3. Release (Orchestrator): neue Engine-Datei `engine/src/elzaintro_r38.c` -> Android-GLOB-Cache (`app/.cxx`) neu
   konfigurieren (Memory reai-v2-android-glob-cache).
4. Der Erzaehler-Text ("We barricaded ourselves ...") ist bei Leon (ROOM1170 msg 8..11) wie bei Elza (ROOM1031 msg 20..23)
   unvertont; "das gleiche wie bei Leon" ist damit auch dort gewahrt (Abschnitt 1, Lesart "Voiceover").

Aufgeraeumt: Messkopien `re15_pc_abn0e_*.exe`, `re15_pc_r38e_*.exe` und `re15_pc_v0825_abn0e.exe` aus dem Build-Ordner
entfernt (nur `re15_pc.exe` liegt dort); kein Code geaendert, nur diese Datei committet.
