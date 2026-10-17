# Runde 38 — Spur E "elzaintro" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_elzaintro`, Zweig `r38/elzaintro`, Basis 1bcbd0ff. Start 2026-10-16 16:35.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon."
2. "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza"

## Stand
- 16:35 Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- Messung vorher beider Punkte, Ursache je Punkt, Umsetzung (wip 3c0ca636), Bisect-Bauten vorN/mitN/v0823.
- Messung nachher (Bild + Ton bitgleich mit Leon), Gegenprobe der Riegel, Schlusslauf 21/21 gruen. FERTIG.

## Punkt 1 — Voiceover Elza-Intro
### Messung vorher
Stand 1bcbd0ff (= master nach v0.8.25 + Doku), Bau dieses Baums. Werkzeug `E_elzaintro_werkzeug/e_lauf.sh`
(Neues Spiel ueber Titel + Charakterwahl, `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1` [+ `_SWITCH=1` fuer Elza],
`RE15_AUDIO_CAP_SYNC`, `RE15_WINDOW_SCALE=1`, `RE15_SCD_TRACE=1`, Bild-Waechter `e_waechter.py`).
Laeufe m2_leon / m2_elza (Ausgaben im Scratchpad, Zahlen hier):

| | Leon (ROOM1240) | Elza (ROOM1241) |
|---|---|---|
| Message_on (SCD-Trace) | F181 id0 @0x057E, F383 id1, F563 id2, F765 id3, F967 id4, F1169 id5 @0x05FA | dieselben Bilder, @0x0582 .. @0x05FE |
| `[voice] clip loaded` | main00..main04 (+ main05 nach dem Riegel) | **keine einzige Zeile** |
| msg 5 geparkt (Stimm-Riegel op_message_on) | F1169..F1179 (main04 laeuft noch) | nein |
| Uebergabe in den Folgeraum | F1431 -> ROOM1170 | F1421 -> ROOM1031 |
| Mitschnitt RMS (Mono, Mittel je 30 Bilder) F0..F150 | 0 / 0 / 344 / 785 / 1271 / 1238 | **identisch** (nur Musik) |
| Mitschnitt RMS F180 / F210 / F390 / F570 | 3763 / 2096 / 3522 / 3921 | **949 / 1159 / 1723 / 2374** (keine Stimme) |

Bildvergleich Montage Leon gegen Elza, Bild fuer Bild (1421 Bilder, `e_vergleich.py`): 232 Bilder verschieden, ALLE nur
im Untertitel (F738-739, F940-1420: Leons Text steht laenger, weil op_message_on waehrend der Aufnahme parkt). Die
Montage-Bilder selbst sind gleich. Ursache: siehe RE-Belege.
### Messung vorher — Ursache
`re15_voice_load_clip` (audio_pc.c) sucht `synchro/STAGE1/room1241/main0N.wav`; den Ordner gibt es nicht
(`git log --all -- synchro/STAGE1/room1241` leer = nie vorhanden) -> `re15_voice_play` kehrt still zurueck. Keine
Regression (nie vertont), daher kein Bisect.
### RE-Belege
**Daten (selbst gemessen, Python ueber die RDT-Bytes, `re15_port/tools/rdt_msgdump.py` + Bytevergleich):**
- Nachrichtenblock ROOM1240 @Datei 0x62C (Zeiger RDT+0x3C), ROOM1241 @0x630; Offsettabelle beider
  identisch `[12, 105, 191, 278, 370, 484]` (6 Nachrichten).
- msg 0..5 byte-gleich (je Nachricht bis einschl. 0x01-END verglichen): 92/85/86/91/113/71 Byte, alle EQUAL.
  ROOM1240 msg0 @0x638 = ROOM1241 msg0 @0x63C = `04 00 1d 00 3e 45 56 3d ...` ("A bizarre incident occurred ...").
- SCD (scd_dump_room.py): sub02 beider Raeume ruft Message_on 0..5 in derselben Reihenfolge mit denselben
  Sleeps; ROOM1240 `2b 00 00 00` @0x57E, `2b 01` @0x598, `2b 02` @0x5A0, `2b 03` @0x5BA, `2b 04` @0x5DA,
  `2b 05` @0x5FA; ROOM1241 jeweils +4 (@0x582/0x59C/0x5A4/0x5BE/0x5DE/0x5FE), weil sub02 dort mit
  `22 03 6f 01` + `22 03 70 01` (Set(3,0x6F)/(3,0x70)) statt `22 03 8b 01` beginnt. Einziger weiterer
  Unterschied: Tuerziel main00 @0x0531 = 0x17 (Leon -> ROOM1170) bzw. 0x03 (Elza -> ROOM1031).
- BSS-Standbilder `BSS/ROOM1240/BG00..08` = `BSS/ROOM1241/BG00..08` (cmp: alle 9 SAME).
- synchro/: `STAGE1/room1240/main00..05.wav` vorhanden (versioniert, 48 kHz 24 bit Stereo, 160..210
  Spielbilder lang); `STAGE1/room1241/` gibt es NICHT.
- Leons Erzaehler danach (ROOM1170 msg 8..11 "We barricaded ourselves ...") ist NICHT vertont
  (room1170 hat main00..07, 13, 14, 16). Elzas Erzaehler ROOM1031 msg 20..23 (textgleich) ebenfalls nicht.
  "Das gleiche wie bei Leon" betrifft also genau die Montage-Zeilen 0..5.

**Port-Mechanismus (audio_pc.c re15_voice_load_clip):** Pfad = `synchro/STAGE%u/room%04X/main%02d.wav` mit dem
LAUFENDEN Raum (`re15_voice_play((uint16_t)g_current_room_id, evt.sample_id)`), also fuer Elza
`synchro/STAGE1/room1241/main00.wav` -> Datei fehlt -> `re15_voice_play` kehrt still zurueck.
Kein Original-Mechanismus betroffen: RE1.5 hat keine englische Sprachausgabe (synchro/ = eigene Produktion,
scd_vm.c Kopf von scd_queue_voice). Die Zuordnung 1241 -> 1240 ist NUTZER-VORGABE ("Es ist das gleiche wie
bei Leon"); ihre FORM (Zeile fuer Zeile dieselbe Id) belegen die byte-gleichen Nachrichten + Message_on-Folgen.
### Umsetzung
- NEU `re15_port/include/re15_elzaintro_r38.h`, `re15_port/engine/src/elzaintro_r38.c`: Tabelle {Raum 0x1241 ->
  Quelle 0x1240, msg 0..5}; `re15_elzaintro_r38_stimme_raum`, `re15_elzaintro_r38_stimme_pfad` (Pfad
  `synchro/STAGE1/room1240/main0N.wav`, Regel synchro/README.md).
- HAKEN `platform/pc/src/audio_pc.c` re15_voice_load_clip (4 Zeilen + Include): NACH den beiden eigenen Versuchen
  (Gross-/Klein-Hex) -> eigene Aufnahmen unter `synchro/STAGE1/room1241/` haetten Vorrang ("Datei da = sie spielt").
  Log `[voice] ROOM1241 msg N <- synchro/STAGE1/room1240/main0N.wav (Runde 38 Spur E)`.
- KEINE neuen Assets, keine Kopie der WAVs (6 x ~1,7 MB gespart; Paket-Gate unveraendert).
### Messung nachher
Bau dieses Baums mit dem Fix (wip 3c0ca636), dieselben Laeufe wie vorher (m4_leon / m4_elza, `e_lauf.sh ... 0-1500/1
440#<raum> RE15_SCD_TRACE=1`):
- Elza debug.log: `[voice] ROOM1241 msg N <- synchro/STAGE1/room1240/main0N.wav (Runde 38 Spur E)` fuer N = 0..5,
  danach je `[voice] clip loaded: main0N.wav` — dieselben sechs Clips wie Leon (Leon ohne Ersatzzeile).
- Ablauf: msg 5 parkt bei Elza jetzt wie bei Leon (11 Aufrufe `[msg] room=124x id=5` beide), Uebergabe in den
  Folgeraum bei BEIDEN in F1431 (vorher Elza F1421).
- Mitschnitt RMS je Zeilenfenster (`probe_r38_elzaintro_auswertung rms`, Fenster 60+90 / 181+150 / 383+150 / 563+150 /
  765+150 / 967+150 / 1180+110):

| | Musik F60 | msg0 | msg1 | msg2 | msg3 | msg4 | msg5 |
|---|---|---|---|---|---|---|---|
| Leon vorher = nachher | 906 | 2817 | 3453 | 3435 | 3303 | 3612 | 3570 |
| Elza vorher | 906 | 1348 | 2184 | 2141 | 1872 | 2424 | 2439 |
| **Elza nachher** | 906 | **2817** | **3453** | **3435** | **3303** | **3612** | **3570** |

- Der ganze Mitschnitt von Montage + Erzaehler (1872 Spielbilder, 11.007.360 Byte ab dem jeweiligen Abschnittsbeginn,
  Takt 807 Leon / 810 Elza) ist zwischen Leon und Elza **BITGLEICH**.
- Bild fuer Bild Montage Leon gegen Elza: **1431 von 1431 Bildern gleich** (vorher 232 verschieden, Untertitel-Dauer).

## Punkt 2 — Pre-Intro-Schwarz: Raum scheint durch (Elza)
### Messung vorher
Welche Schwarzblende: Die Montage (ROOM1240/1241) ist bei Leon und Elza bildgleich (s. Punkt 1, nur Untertitel-Dauer).
Der Unterschied liegt im ERZAEHLER danach ("We barricaded ourselves ..." auf Schwarz): Leon ROOM1170 sub11 Cut 7,
Elza ROOM1031 sub15 Cut 13 (@0x02A74 `29 0d`).

Bild fuer Bild (Laeufe m2, 441 Erzaehlerbilder F0..F440): **438 von 441 Bildern verschieden.**
- Elza: ein 8x6-Pixel-Feld bei x 91..98, y 107..112 (320x240) mit Farben (128,120,104), (104,104,96), (72,80,72) ...,
  ab F3 (11 Pixel, Tuer-Einblendung) / F4 (46) / ab F5 dauerhaft 48 Pixel bis F440; dazu Grund (1,1,1) statt (0,0,0).
- Leon: reines (0,0,0) + Text (Sicherheitsnetz s_scene_black, main.c, Raum 0x1170 + Cut 7).
- Bilder: `E_elzaintro_bilder/vorher_erzaehler_f200_leon_links_elza_rechts.png` (Leon links, Elza rechts, das graue
  Feld = die Karte), `vorher_elza_erzaehler_f200_ausschnitt_karte.png`, `..._verstaerkt_x40.png`.
- Log Elza beim Eintritt in ROOM1031 (nach `Cut_chg(13)`): `[kartenitems] ROOM1031: Karte Blatt 2 (Bit 9,89) Prop obj
  15 bei (-1692,-2769,-23638), Zone Slot 8` und `[prop-render] pi=0 oid=0x0F pos=(-1692,-2769,-23638)` — **"die map vom
  Raum" ist woertlich der Lageplan 1F** (Runde 36 Spur N, `engine/src/kartenitems_r36.c`), ein Port-Prop an der Wand
  der Lobby.
- GEGENPROBE (eine Variable): derselbe Elza-Lauf mit `RE15_SET_FLAG=9:89` (Lageplan 1F genommen -> kein Prop,
  `re15_kartenitems_r36_install` legt nur an, solange das Bank-9-Bit fehlt): keine `[kartenitems]`-Zeile, die farbigen
  Pixel je Erzaehlerbild = genau Leons Textpixel (1875 / 1271 / 1914 / 354 / 0). Das Feld IST die Karte.

Warum nur Elza: Elzas Erzaehler laeuft IN der Lobby (ROOM1031), Leons im Helipad-Raum ROOM1170 (dort haengt keine
Karte, und das Netz s_scene_black faerbt Cut 7 ohnehin vollflaechig schwarz). Die Erzaehler-Kamera ist in BEIDEN
Raeumen dieselbe — und in ROOM1031 ist sie die Lobby-Kamera (Bytes s. RE-Belege) — sie sieht den Lageplan.

Regression: `kartenitems_r36.c` kam mit 554877f9 (Runde 36 Spur N), gemergt cb1d5f59, erstmals ausgeliefert v0.8.24
e8b3958f; in v0.8.23 3ec5b1f8 gibt es die Datei nicht (`git ls-tree`).

**Bisect** (Werkzeug `E_elzaintro_werkzeug/e_bisect.sh`: `git archive <stand> re15_port` ohne shared_assets nach
`re15_port/build_bisect/src_<marke>`, Bau mit dem local_build.sh DIESES Stands, Tests aus, Assets aus dem Baum —
seit v0.8.23 aenderten sich dort nur MASKS fuer 10F0/1100/1221/2091; Messung `e_lauf.sh elza 200-200/1 210#1031`,
Erzaehlerbild F200, Feld x 91..98 / y 107..112, `e_feld.py`):

| Stand | `[kartenitems] ROOM1031` | Erzaehler F200 hell gesamt | Feld hell | Urteil |
|---|---|---|---|---|
| v0.8.23 3ec5b1f8 | 0 | 1539 (= Leons Text) | 0 | sauber |
| 5d279697 (= cb1d5f59^1) | 0 | 1539 | 0 | sauber |
| **cb1d5f59** merge(r36): Spur N kartenitems | 1 | 1587 | **48** | **Karte sichtbar** |
| 1bcbd0ff (Basis, = v0.8.25 + Doku) | 1 | 1587 | 48 | Karte sichtbar |

Verursacher: **cb1d5f59** (Merge der Runde-36-Spur N, Lageplan-Props; der Lageplan-Commit selbst 554877f9), erstmals
ausgeliefert in v0.8.24 e8b3958f.
### RE-Belege (Original: der Erzaehler-Schnitt zeigt NUR Standbild + Text)
Alle Bytes selbst aus den ausgelieferten RDT/BSS gelesen (Python, Werte im Riegel `unit_r38_elzaintro_schwarz`):
- **Einziger Weg in Cut 13:** ROOM1031 sub15 @0x02A74 `29 0d` = Cut_chg(13); Opcode-Walk (scd_dump_room.py) ueber
  alle main-/sub-Regionen: kein zweites `29 0d`, Cut_replace nur 0->9, 3->10, 4->11, 6->12. RVD-Zonen (RDT+0x28 =
  0x228, 37 Saetze a 20 Byte): Ziel-Schnitte nur 0..9, KEINE Zone fuehrt in Cut 13. -> Cut 13 == Erzaehler.
- **Leons Gegenstueck:** ROOM1170 sub11 @0x016A6 `29 07` (einziges Cut_chg(7); RVD-Ziele 0,1,2,3,8,9,10,11).
  sub11 (Leon) und sub15 (Elza) sind gleich gebaut: Set(3,125,1), Set(2,7,1), Cut_chg, Sleep 20, 4x Message_on mit
  Sleep 100, Sleep 20, Aot_on (Selbst-Tuer).
- **Kamera:** RDT+0x24 = 0x60, 0x20 Byte je Schnitt. ROOM1031 Cut 13 @0x200 pos (-12834,-3114,-9774) tgt
  (-7794,-2196,-22446) = ROOM1031 Cut 0 @0x60 (LOBBY-Kamera) = ROOM1170 Cut 7 @0x140 = ROOM1240/1241 Cut 0 @0x60.
  In ROOM1170 schaut diese Kamera ins Leere (Kommentar main.c "VOID camera"); in ROOM1031 IST sie die Lobby-Kamera.
- **Zone:** die einzige Zone von Cut 13, ROOM1031 @0x4F8 `01 ff 0d 00 31 c6 44 80 1e c1 d8 dc d5 fe 11 dc 69 10 44 80`,
  ist bis auf die Schnittnummer byte-gleich mit ROOM1170 @0x370 (Cut 7).
- **Standbild:** BSS/ROOM1031/BG13.BSS = BSS/ROOM1170/BG07.BSS = BSS/ROOM1240/BG00.BSS (sha256 44cafcd7ff20b961...).
- **Keine Raum-Objekte im Erzaehler:** ROOM1031 main00 legt Gegner und Objekte nur im Zweig Ck(3,125)==1 an
  (@0x01E50 `06 00 c0 01`, @0x01E54 `21 03 7d 01`: Sce_em_set x20 ab @0x01E5C, Obj_model_set @0x01FEC); der
  Erzaehler-Zweig verlangt (3,125)==0 (@0x0207E `21 03 7d 00` -> Door_aot_set 19 @0x02082, Evt_exec sub15 @0x020A2);
  (3,125) setzt erst sub15 @0x02A68 `22 03 7d 01`.
- **Spieler hinter der Kamera:** Tuer ROOM1241 main00 @0x051A `3b 00 02 31 ... 9a 99 00 00 eb f0 ...` -> Ziel
  (-26214,0,-3861); relativ zur Kamera (-13380,+3114,+5913), Blick (5040,918,-12672): Skalarprodukt -139.506.084 < 0.
Damit ist das Originalbild im Erzaehler: schwarzes Standbild + Text. Der Port-Lageplan (Runde 36) ist das einzige
3D-Objekt vor dieser Kamera. ⛔ Ein (3,125)-Tor am Installer haelt NICHT: `[kartenitems]` laeuft im Log NACH
`Cut_chg(13)`, d.h. sub15 hat (3,125) beim Installieren schon gesetzt (main00 prueft vorher, der Installer danach).

### Umsetzung
- NEU `re15_port/include/re15_elzaintro_r38.h`, `re15_port/engine/src/elzaintro_r38.c`: Tabelle der Erzaehler-Schnitte
  {ROOM1170 Cut 7 @0x016A6 (Leon, bisher woertlich in main.c), ROOM1031 Cut 13 @0x02A74 (Elza)};
  `re15_elzaintro_r38_erzaehler_schwarz(raum, cut)`.
- HAKEN `platform/pc/main.c` (Netz s_scene_black, 1 Zeile + Include): `re15_render_pc_set_scene_black(
  re15_elzaintro_r38_erzaehler_schwarz(g_current_room_id, active_cut_idx))` statt `g_current_room_id == 0x1170 &&
  active_cut_idx == 7`. Leon bleibt bitgleich (dieselbe Bedingung, jetzt aus der Tabelle).
- KOMMENTAR `platform/pc/src/render_pc.c`: das Netz war als "redundant, safe to delete" markiert — fuer Elza ist es die
  Abdeckung; Warnhinweis ergaenzt.
- Wirkung: in ROOM1031 Cut 13 Vollbild-Schwarz UNTER dem Text (Layer-Reihenfolge render_pc.c: Framebuffer -> 3D ->
  Letterbox -> s_scene_black -> Text -> Fade) = Originalbild. Der Lageplan bleibt angelegt und ist ab der Szene
  (Cut 4/6/0) wie bisher sichtbar; jede kuenftige Port-Requisite der Lobby ist im Erzaehler ebenfalls abgedeckt.
### Messung nachher
- Laeufe m4 (wie oben): Erzaehler Bild fuer Bild Leon (ROOM1170 Cut 7) gegen Elza (ROOM1031 Cut 13): **441 von 441
  Bildern pixelgleich** (vorher 438 verschieden). Feld x 91..98 / y 107..112 in allen Erzaehlerbildern dunkel; der
  Lageplan ist trotzdem angelegt (`[kartenitems] ROOM1031: Karte Blatt 2 ...` im Log, vor dem Netz).
- Bilder: `E_elzaintro_bilder/nachher_erzaehler_f200_leon_links_elza_rechts.png`,
  `nachher_elza_erzaehler_f200_verstaerkt_x40.png` (x40 verstaerkt: nur der Text, kein Feld, kein (1,1,1)-Grund).
- Der Lageplan ist NICHT weg, nur im Erzaehler verdeckt: Elza-Lauf m5 (`e_lauf.sh elza 900-900/1 905#1031`) — nach der
  Selbst-Tuer zweite `[kartenitems] ROOM1031`-Zeile, `Cut_chg(0)` F840, Bild F900 (Lobby, "Elza: Hey, is anyone here?!"):
  Feld x 91..98 / y 107..112 = 48 helle Pixel = der Plan an der Wand
  (`E_elzaintro_bilder/nachher_elza_lobby_cut0_f900_karte_sichtbar.png`). Leons Plan ROOM1030 Cut 0/3:
  `integration_r36_kartenitems_bild` gruen.

## Leon unveraendert (beide Punkte)
- Bilder Leon vorher (m1, Basisbau) gegen nachher (m4, Bau mit Fix): 61 gemeinsame Bilder (Montage + Erzaehler, je 15.
  Bild): 0 verschieden.
- Ton Leon vorher = nachher: RMS-Fenster identisch (906;2817;3453;3435;3303;3612;3570), Uebergabe F1431 beide Male.
- Erzaehler-Gate fuer Leon: dieselbe Bedingung (ROOM1170 Cut 7) jetzt als Tabellenzeile; `unit_r38_elzaintro_schwarz`
  prueft sie, `integration_r37_roy_leon1170` / `unit_elliot_1170_run` gruen.

## Gegenprobe der Riegel (rot am Stand vorher)
- `test_r38_elzaintro.cmake TEIL=schwarz` gegen die Bisect-exe cb1d5f59: ROT "Erzaehler F60: 74507 Pixel Elza != Leon,
  Lageplan-Feld 48 hell (soll 0/0)".
- `TEIL=stimme` gegen dieselbe exe: ROT "msg 0: Elza-Ersatz 0x" (diese Bisect-exe findet synchro/ aus build_bisect nicht,
  daher auch Leon 0x — der Elza-Teil allein reicht). Am Basisbau dieses Baums (m2) waeren auch die Tonfenster rot:
  Elza 1348 gegen Leon 2817 im ersten Zeilenfenster (Verhaeltnis 0,48, Schranke 0,9).

## Tests
Registrierung NUR aus `re15_port/tests/unit/probes/r38_elzaintro.cmake` (GLOB), Quellen `tests/unit/test_r38_elzaintro.c`,
`tests/unit/probe_r38_elzaintro_auswertung.c` (Auswertewerkzeug, kein add_test), `tests/integration/test_r38_elzaintro.cmake`.

| Test | misst | Ergebnis |
|---|---|---|
| `unit_r38_elzaintro_stimme` | 26 Pruefungen: Nachrichtenblock @0x62C/@0x630, Offsettabelle, msg 0..5 byte-gleich, Message_on `2b 0N 00 00` an beiden sub02-Offsets, sub02-Kopf (4 Byte Versatz), Abbildung genau (1241, 0..5) -> 1240, Ersatzpfad, sechs WAVs im Repo | gruen |
| `unit_r38_elzaintro_schwarz` | 13 Pruefungen: je Tabellenzeile `29 cut` am Offset = einziges Cut_chg im ganzen SCD (Opcode-Walk `scd_opcode_size_at`), keine RVD-Zone fuehrt hinein, Kamera = ROOM1170 Cut 7, ROOM1031 Cut 13 = Lobby-Kamera Cut 0, BG13 = ROOM1170 BG07, Gate an/aus | gruen |
| `integration_r38_elzaintro_stimme` | echte exe, Neues Spiel Leon + Elza bis Montage-Bild 1300: sechs Ersatzzeilen bei Elza, keine bei Leon, msg 5 parkt gleich oft (11/11), RMS je Zeilenfenster Elza = Leon +-10 % | gruen (RMS gleich: 906;2817;3453;3435;3303;3612;3570) |
| `integration_r38_elzaintro_schwarz` | echte exe, Neues Spiel Leon + Elza bis Erzaehler-Bild 420: Erzaehler-Ereignis je 1x, Lageplan in ROOM1031 angelegt, Bilder F60/F230/F400 Elza = Leon pixelgleich, Lageplan-Feld dunkel | gruen (0 Pixel verschieden, Feld 0) |

Gezielte Laeufe (KEINE volle Suite):
- Lauf 1 (alle 21, darunter die Module, die der Diff beruehrt: Stimme audio_pc.c, Erzaehler-Netz main.c/render_pc.c,
  Elza-Vorspann, Lageplan, Leon-1170):
  `ctest -R "^(unit_r38_elzaintro_|integration_r38_elzaintro_|unit_synchro_stimme$|unit_voice_ausreden$|unit_se_voice_machine$|unit_r17_untertitel_pin$|unit_r34n_f_leiche_stimme$|unit_r30_tuer_stimme$|integration_r36_elza_stoehnen$|integration_elza_vollstart$|integration_r37_roy_leon1170$|unit_elliot_1170_run$|unit_montage_fx$|integration_boot_bg_pin$|integration_dark_start_pin$|integration_r30_cut_blitz$|unit_r36_kartenitems$|integration_r36_kartenitems_bild$|unit_r30_elza_selbsttuer$)"`
  -> 19 gruen, die zwei neuen Integrationsriegel rot an einem TESTFEHLER (macro-Zaehler wertete `\[` doppelt aus,
  0 Treffer auf eine vorhandene Zeile) — behoben 4f251f7e (function statt macro), Logik vorher an den Logs des roten
  Laufs nachgeprueft.
- Lauf 2: `ctest -R "^integration_r38_elzaintro_"` -> `100% tests passed, 0 tests failed out of 2`.
- **Schlusslauf (alle 21 in EINEM Lauf, Stand eb3f8010):** derselbe `-R`-Ausdruck wie Lauf 1 ->
  `100% tests passed, 0 tests failed out of 21` (`Total Test time (real) = 1084.52 sec`); darin
  `integration_r38_elzaintro_stimme` 120,6 s, `integration_r38_elzaintro_schwarz` 168,9 s,
  `integration_r36_kartenitems_bild` 478,6 s, `integration_elza_vollstart` 112,5 s.

## OFFEN
- Nichts aus dem Auftrag dieser Spur. Hinweis: Elzas Erzaehler (ROOM1031 msg 20..23) ist wie Leons Erzaehler (ROOM1170
  msg 8..11) NICHT vertont — beide haben keine Aufnahme; "das gleiche wie bei Leon" ist damit auch dort erfuellt. Wer
  den Erzaehler vertonen will, legt `synchro/STAGE1/room1170/main08..11.wav` (Leon) bzw. `room1031/main20..23.wav`
  (Elza) ab — keine Code-Aenderung noetig.
- Bekannte Randnotiz (kein Befund): die schwarzen MDEC-Standbilder dekodiert der Port auf (1,1,1) statt (0,0,0)
  (Montage-Cut 0, Erzaehler ohne Netz). Unsichtbar; im Erzaehler deckt das Netz jetzt bei beiden Figuren auf (0,0,0).

## Fuer den Nutzer
- **Neue Assets:** keine. **Sprachdateien:** keine neuen; Elzas Montage spricht Leons `synchro/STAGE1/room1240/main00..05.wav`
  (Text Zeile fuer Zeile identisch). Eigene Elza-Aufnahmen koennen jederzeit als `synchro/STAGE1/room1241/main00..05.wav`
  abgelegt werden und haben dann Vorrang.
- **Bedienung:** unveraendert. Neues Spiel -> Elza: der Vorspann ist jetzt in Bild und Ton bitgleich mit Leons
  Vorspann; der Erzaehler-Text "We barricaded ourselves ..." steht auf reinem Schwarz, der Lageplan an der Lobby-Wand
  erscheint erst mit der Szene.
- **Fuer den Release (Orchestrator):** neue Engine-Datei `engine/src/elzaintro_r38.c` — der Android-Bau cacht die
  GLOB-Liste (`app/.cxx`, Memory reai-v2-android-glob-cache) und muss neu konfigurieren.
- **Entscheidungsfragen:** keine.
