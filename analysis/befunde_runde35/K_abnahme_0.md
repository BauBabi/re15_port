# Runde 35 — Spur K "cut10f0": UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r35_cut10f0`, Zweig `r35/cut10f0`, HEAD 7b20a561 (Basis master 154a73c1), 2026-10-03.
Geprueft gegen den WORTLAUT in `AUFTRAG.md` Z.40-67 und Z.92. Gemessen am gebauten Stand
`re15_port/build/platform/pc/re15_pc.exe` (16:01; `local_build.sh configure` + `build` selbst gefahren:
`ninja: no work to do` / `=== LOCAL-BUILD-OK (build)` — die exe ist der Stand des HEAD).
Kein Code geaendert. Messlaeufe mit einer Kopie der exe unter eigenem Namen (`re15_pc_abn0k.exe`, wieder entfernt).

## 0. Urteil

**NICHT BESTANDEN.** Szene und Karte stimmen; die Musik und eine Geste nicht ganz.

| Punkt (AUFTRAG.md) | Urteil | Kurz |
|---|---|---|
| 1 Szene beim ersten Betreten von ROOM10F0 (Z.41-65) | erfuellt | 18 Zeilen woertlich, Reihenfolge, Kamera, Tuerknall, Abgang, Balken gemessen |
| 2 Karte danach: ROOM11C0, dann ROOM1150, beide bis besucht (Z.66) | erfuellt | Hinweis je 3,95 s; normale Karte: beide blinken, 1150 hoert nach dem Betreten auf |
| 3 MAIN01 durchweg bis zum Parkplatz (Z.92) | teilweise | Maengel 1 und 2 |
| 4 Animationen passend wie bei vergleichbaren Dialogen (Z.67) | teilweise | Mangel 3 |

Gates: Suite gruen (siehe §1), @0x-Gate haelt (§5), Pfad-Gate haelt, Vertrag §1.4 "kleine Haken" verletzt
(Mangel 4), Tests vorhanden und messend, aber mit zwei Luecken (bei Mangel 1 und 3 genannt).

## 1. Bau und Suite

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`.
- Ganze Suite NICHT selbst gefahren: das Dossier traegt die woertliche Zeile `=== LOCAL-BUILD-OK (all) — Tests 486/486`
  (§8.8), also greift die Regel des Auftrags. Gegengeprueft: `re15_port/build/local_build_ctest.log` (16:25) nennt
  `100% tests passed, 0 tests failed out of 486`, 486 Zeilen "Passed"; `git diff f2ac79e0 7b20a561 --stat` = nur das
  Dossier, der Code des Suite-Laufs ist der des HEAD.
- Selbst gefahren: `ctest -R "^unit_r35_cut10f0_|^unit_cam_selfheal$"` -> 8/8 gruen (0,41 s);
  `ctest -R "^integration_r35_cut10f0$"` -> gruen (215 s). Zusammen `-R r35_cut10f0` 8/8.

## 2. Messlaeufe (alle selbst gefahren)

Karte je Lauf aus `probe_r35_cut10f0_karte.exe`, Start `RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0`, beschleunigter Renderer (kein RE15_SOFTWARE_RENDER), Bilder `RE15_FRAMEDUMP`.

| Lauf | Stand / Weg | Zweck |
|---|---|---|
| A | ROOM10D0 vor der Tuer, Aktionstaste, echte Tuer (DOOR13) -> ROOM10F0; `RE15_STATE_LOG`, `RE15_NECK_LOG`, `RE15_MSG_LOG`, `RE15_SE_DEBUG`, `RE15_CAM_TRACE`, `RE15_AUDIO_CAP_SYNC`, FRAMEDUMP 0-3300/20 | ganze Szene, Ton, Hinweis |
| G, G2, G3, G4 | wie A, danach Skript START / L1 / RUNTER / HOCH / Abbruch / Vorwaerts | Inventar und normale Karte nach der Szene, Steuerung |
| H1 | Stand in ROOM10F0 mit (9,71)=1 | normale Karte nach dem Laden, MAIN01 nach dem Laden |
| H2 | Stand IN ROOM10F0 mit ausstehender Szene (Boot-Weg) | Szene am Lade-Weg, Inventar danach |
| K1 | wie H1 + `RE15_MAP_SHOT_PAGE=4` (Blatt 3F ganz besucht) | ROOM1150 blinkt trotz Besuch |
| J1 | wie H1 + `RE15_SET_FLAG=3:94`, Raumwechsel nach ROOM1150 | Latch (9,72), Karte danach, MAIN01 |
| J2 | Stand in ROOM11B0 vor der Tuer mit (9,71)=1, echte Tuer -> ROOM11C0, Raumwechsel zurueck | Ende von MAIN01 |
| J3 | wie H1, Raumwechsel nach ROOM1030, `RE15_BGM_CTL_DEBUG`, Ton | Skript-BGM in der Lobby |
| J4, J5 | wie H1 + `RE15_SET_FLAG=7:209,7:210,7:221,7:44,7:45`, Raumwechsel nach ROOM11D0, dann echte Tuer -> ROOM1180, Ton | Mangel 1 |

Tuer-Zensus (alle STAGE1-RDTs, `scd_dump_room.py`): Tueren mit Ziel-Byte 0x0F gibt es nur in ROOM10D0 (@0x01052,
@0x01174). Der erste Eintritt geschieht also immer am Spawn (8400,0,-350), die Szene kann nicht von einer anderen
Seite her angetroffen werden.

## 3. Punkt 1 — Szene (erfuellt)

Texte selbst aus den Rohbytes in `cut_10f0.c` dekodiert (eigene Umkehrung der Glyphentabelle `msg_common.c:179`):
alle 18 Zeilen woertlich wie AUFTRAG.md, Sprecher "Leon:" Farbe 1, "Woman:" Farbe 2 bei 7/8/9, "Ada:" bei 13,
"Marvin:" Farbe 7; die lange Woman-Zeile und Leons lange Zeile sind je in zwei Nachrichten geteilt (8/9, 21/22).

Ablauf Lauf A (`state.log`, Bildnummern = Bilder im Raum):

| Bild | Gemessen | Wortlaut |
|---|---|---|
| F6 | `[scd F6] Cut_chg(2)`; Ada Typ 0x42 bei (6000,11500) Gierung 3072; Marvin 0x40 geparkt (-30000,-30000) | "Zuerst Kamera auf CUT2 - Ada" |
| F56 | `Cut_chg(0)` | "Dann Kamera auf CUT0 - Leon" |
| F76-F166 | Nachricht 6, Leon Clip 15 (Arm), dann 23 | "Arm strecken ... Hey - how did you came in here?" |
| F208-F384 | Leon geht (Clip 105) ueber (5542,2457) nach (4792,11359); cam 0 -> 1 (F270) -> 2 (F325) | "auf sie zulaufen", Kamera wechselt, solange er laeuft |
| F395 | Leon Gierung 4019 (zu Ada), x 4792 < Adas 6000 | "links von ihr" |
| F419 | Nachricht 7, Ada Clip 19 vor + rueckwaerts, dann 23 | "180 grad gedrehte Arm Gestik" |
| F529 / F639 | Nachricht 8 / 9; `PLC_NECK slot=1 mode=2 tgt=(0,0,300)`, `mode=4 tgt=(3,0,0)` | Kopfschuetteln, Kopf gesenkt |
| F749-F756 | `[se] Se_on bank=14 id=1 -> RE2-Tuerbank`; Ton: Pegel 1828 (F750), 2998/2351/2391 (F754-756) ueber Grund ~800 | "Tuer knallen Sound" |
| F761 | Marvin bei (8400,-350), `Cut_chg(0)` | "dann Marvin Laden - dann zu Cut0" |
| F790 | Nachricht 10, Marvin Gierung 2877 (= Richtung Leon), Clip 15 | "schaut ... zu Leon", Arm |
| F900-F1068 | `Cut_chg(2)`, Marvin geht nach (3823,9862), Gierung 3423 | "schraeg links", alle drei im Bild (Bild s4) |
| F1094 | Nachricht 11, Leon Gierung 1413 (zu Marvin), Clip 15 | "Hey Marvin ... arm strecken" |
| F1220 | Nachricht 12, Leon Gierung 4019 (zu Ada), Clip 15 | "arm strecken Richtung Ada" |
| F1330 / F1440 / F1530 | 13 (Ada Clip 18), 14 (`slot=0 mode=3` Nicken), 15 (Marvin 15, 18) | Vorstellung |
| F1656 | Nachricht 16, `slot=0 mode=2` + `mode=4` | Leon Kopfschuetteln, Kopf gebeugt |
| F1786 / F1896 | 17 (Marvin Clip 20), 18 "..."; bis F2026 130 Bilder | "etwas pause" |
| F2026-F2466 | 19 (21, 17), 20 (15, 16), 21 (15, 16), 22 (17), 23 (15, 16) | Reihenfolge wie Auftrag |
| F2589 / F2608 | Marvin, dann Ada laufen los (Clip 0 = rennen); F2636 `Cut_chg(0)`; geparkt F2659 / F2676; zweiter Knall F2677-F2683 | "rennen hintereinander ... Richtung Tuer" |
| F2686-F2731 | `Cut_chg(2)`, Leon allein, Balken noch 45 Bilder; F2731 Spielerzustand frei | "kurz weiter Cutscene Balken" |

Zeilenabstaende aus `state.log`: 110 / 110 / 151 / 304 / 126 / 110 / 110 / 90 / 126 / 130 / 110 / 130 / 110 / 110 /
110 / 110 — Minimum 90, wie behauptet.

Genau einmal: Integration Lauf B ((9,71)=1) ohne `[cut10f0]`-Zeile; H1 ebenso. Boot-Weg (H2): Szene und Leihe laufen.
Nach der Szene (G2/G3/G4/H2): START oeffnet den Statusschirm (Bild F3070), Leon laeuft mit Vorwaerts
(4750,11298) -> (2902,8668), Kamera wechselt selbst auf Cut 3. Der Inventar-Fehler der ROOM1050-Szene tritt hier nicht auf.

Auslegung, die der Nutzer am Bild beurteilen muss (kein Mangel): Ada steht im Cut 2 hinten bei etwa 49 % der
Bildbreite — am rechten Rand des begehbaren Gangs, die rechte Bildhaelfte ist die Konsole. Die drei Figuren sind
im Cut 2 etwa 60 von 480 Bildpunkten hoch.

## 4. Punkt 2 — Karte (erfuellt)

- Hinweis am Szenenende (Lauf A): `[hint] F2741 begin`, Blatt "POLICE STATION B1" mit einer Kachel (ROOM11C0),
  rot F2742 / Umriss F2762 / rot F2781 / Umriss F2801 / rot F2820 / Umriss F2840, je mit `+Ton Se(2,0x2B)`;
  `[hint] F2859 Folge-Hinweis 1 -> 2 (Zeit)`, Blatt "3F" mit der Kachel ROOM1150, drei Perioden;
  `[hint] F2977 schliessen (Abbruch/Zeit)` bei t = 3 953 649 us. Dauer je Ziel 3,95 s auf der Wanduhr (in Lauf G nach
  einem Rechner-Stillstand von 2,5 s nur 60 Bilder, aber dieselben 3,95 s).
- Normale Karte nach der Szene, gleiche Sitzung (G2): Blatt B1 Kachel 11C0 rot (F3340) / Umriss (F3360); Blatt 3F
  Kachel 1150 rot (F3580) / Umriss (F3600, F3620). Nach dem Laden (H1) dasselbe.
- ROOM1150 schon besucht (K1, Blatt 3F ganz aufgedeckt): alle Raeume gruen, Kachel 1150 wechselt rot (F200, F230,
  F270, F310) / Umriss (F210, F250, F290).
- ROOM1150 nach der Szene betreten (J1, Raumwechsel-Weg): `[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1,
  Kartenziel ROOM1150 erreicht`; Karte: 3F-Kachel steht (aktueller Raum), B1-Kachel 11C0 blinkt weiter
  (Umriss F420, rot F440/F480/F500, Umriss F520/F560).
- ROOM11C0 hat eine einzige Zonenzeile (`re15_map_zones.h:168`, Blatt 0 Rechteck 4), ROOM1150 ebenso (:154).

## 5. Punkt 3 — MAIN01 (teilweise)

Gemessen und in Ordnung:
- Szenenende (A): `[cut10f0] Szene zu Ende ...`, `[bgm] stage=0 room=0F entry=FF01 -> MAIN01(flag 0) SUB--`,
  `[bgm] loaded: 9 VAGs, SEQ 7788B` = MAIN01.BGM (Trailer SEQ 7788, VabHdr vs=9; MAIN20 hat 6524 / 7). Ton: Pegel
  faellt F2740-F2790 auf 118, ab F2795 wieder Musik.
- Nach dem Laden (H1): `entry=FF20`, `[save] CONTINUE: resumed in room 10f0`, `entry=FF01 -> MAIN01`.
- Raumwechsel im Fenster (J1, J3): `room=15 entry=FF01 ... [unveraendert, laeuft durch]`, `room=03 entry=FF01 ...
  [unveraendert, laeuft durch]`; in ROOM1030 setzt `Sce_bgm_control slot=0 op=1` nur die Lautstaerke (kein Neustart),
  Pegel danach 363-1688.
- Ende (J2, echte Tuer DOOR1A): `[bgm] stage=0 room=1C entry=FF56 -> MAIN16(flag 1)`; zurueck in ROOM11B0
  `entry=FF1D -> MAIN1D` (Tabelle wieder normal).

Nicht in Ordnung: Maengel 1 und 2 (§8).

## 6. Punkt 4 — Animationen (teilweise)

17 von 18 Zeilen: Sprecher dem Angesprochenen zugewandt, Geste aus der Bibliothek 15-23, Takt 110 (§3). Bilder
(Ausschnitte 1,5-fach): F440-F480 Ada Arm seitlich hinaus, F560-F620 Kopf gesenkt, F1120-F1160 Leon zu Marvin,
F1240-F1280 Leons Arm waagerecht auf Ada, F1340/F1360 Ada Hand zur Brust. Die eine Ausnahme ist Mangel 3.

## 7. Gates

### 7.1 RE-Gate (@0x)
Selbst disassembliert (`re15_disasm.py` / `re2_disasm.py`), jede Stelle enthaelt das Behauptete:
- Plc_dest @0x80041be4: `sb v0,4(a1)` @0x80041c14 (v0 = 4), `sb a2,5(a1)` @0x80041c18, `sb v1,451(a1)` @0x80041c24,
  `sh zero,452(a1)` @0x80041c4c.
- Plc_neck @0x80041e98: `lw v1,340(a0)` @0x80041e9c; Sprungtabelle @0x80010dfc = 0x80041ed8/ee4/ef0/efc/f08 ->
  Modus 0 `ori 0x12`, 1 `0x4`, 2 `0x8`, 3 `0x2a`, 4 `0x58`.
- BGM-Cache @0x80044278 `andi a0,s0,0x3f` / @0x8004427c `andi v1,v1,0x3f` / @0x80044280 `beq a0,v1,0x800442f4`;
  `srl v0,s0,6` @0x800442cc.
- LOAD @0x80026290-a0: `addiu a0,a0,3516` (0x800b0dbc), `jal 0x8004ee38`, `ori a2,zero,0x1430`.
- Tabelle @0x80074828: [0x0F] 0xFF20, [0x0D] 0xFF1F, [0x15] 0xFF1E, [0x1B] 0xFF1D, [0x1C] 0xFF56, [0x03] 0x4041.
- RE2 @0x8009A604 `a8 3d 04 95 08 00 00 00 96 c1 00 00`; @0x8006F884 `andi v0,v0,0x6000`; Opcode 0x84 @0x800591C4
  (Modus 4 @0x800591DC, Bit 0x8000 @0x800591EC, Nummer @0x80059210).
- RDT-Bytes gelesen und gleich: ROOM11B0 @0x014EE (40/50/20), @0x01574, @0x0154E, @0x01478, @0x01080; ROOM11C0
  @0x0185E, @0x018A0, @0x018D8, @0x01886; ROOM10D0 @0x01174, @0x01A02, @0x01ABC; ROOM1050 @0x00DCA, @0x00DF2;
  ROOM1150 @0x01110. `shared_assets/PSX` == `info/Re1.5/PSX` fuer 10F0/11B0/11C0/10D0.
- Eingebackener Tuerton == `DOOR13.DO2[0..0x3DA8)` (sha1 5ec7fd64...). Zensus 240 RDTs: kein Se_on mit Bank 0x0E,
  kein Ck/Set (9,71) oder (9,72).
- Suchworte im Diff (deferred, tunable, interim, for now, faithful, plausibel, TODO): keine. Kein Env-Schalter als
  Abschluss (`RE15_SE_DEBUG` ist eine Logzeile).
- Ungenauigkeit im Dossier §2: "@0x80044280 -> LAB_800443b0" — das Sprungziel ist 0x800442f4 (§8.5 nennt es richtig).

### 7.2 Vertrag und Pfade
- Bank-9-Bits 71 und 72, Nachrichten 6..23, Ereignis 20, kein AOT-Slot: innerhalb der Zuteilung.
- `git diff master --name-only`: nichts unter `release/`, `platform/android/`, `shared_assets/PSX/`, keine
  `tests/*/CMakeLists.txt`.
- Kleine Haken: `scd_room_setup.c` +5, `scd_vm.c` +3, `game_step_common.c` +2, `enemy_common.c` +4 — in Ordnung.
  `menu_common.c` und `platform/pc/main.c` nicht: Mangel 4.

### 7.3 Tests
Je Punkt vorhanden und messend (echte VM, echte exe am Tuerweg). Luecken: Leons Gierung wird fuer die Zeilen
7/11/12/16/19/21/22 geprueft, fuer Zeile 6 nicht; kein Test faehrt ein Raumskript, das den MAIN-Kanal stoppt.

## 8. Maengel

1. **MAIN01 verstummt dauerhaft, sobald ein Raumskript im Fenster den MAIN-Kanal stoppt** (Punkt 3, "durchweg").
   ROOM11D0 "KENNEL LIGHT" sub01 @0x01710 `54 00 02 00 00 00` (Sce_bgm_control Slot 0, op 2 = Stop), Bedingung
   @0x016E4-@0x0170C: Ck(7,209), (7,210), (7,221), (7,44), (7,45) alle 1 (die fuenf Gegner des Raums tot) und (5,0)=0.
   Messung J4/J5: `[bgm] stage=0 room=1D entry=FF01 ... [unveraendert, laeuft durch]`,
   `[bgm] Sce_bgm_control slot=0 op=2 ... capTick=519`, Tonpegel davor 580, danach 0. Echte Tuer (DOOR1A) nach
   ROOM1180: `[bgm] stage=0 room=18 entry=FF01 -> MAIN01 ... [unveraendert, laeuft durch]` — Pegel 0 ueber 450 Ticks
   (15 s). Gegenprobe J3 (ROOM1030, kein Stop): Pegel 363-1688.
   Ursache: die Weiche liefert in jedem Raum denselben MAIN, der Gleich-Zweig (@0x80044280) tut nichts, also
   startet kein Raum die Sequenz neu.
   Erreichbar: ROOM1180 main00 @0x009EA Ck(4,243)==0 waehlt die Tuer Slot 0 -> ROOM1160, sonst @0x00A12 -> ROOM11D0;
   mit Strom (ohne den der Parkplatz nicht erreichbar ist) liegt der Zwinger am Pflichtflur (ROOM1180 Slot 3
   @0x00A74 -> ROOM11B0 -> ROOM11C0). Die Dossier-Aussage §8.4 "kein Raum auf dem Weg STOPPT den MAIN-Kanal
   ausserhalb bereits gelaufener Szenen" ist fuer ROOM11D0 sub01 nicht belegt.
   Nicht gemessen, gleiche Bauart: ROOM1090 sub03 @0x024DA `54 00 00 01 01 41` (Programm 0 stumm) und @0x024E0
   `54 00 02 00 00 00`; ROOM10F0 Tuer Slot 1 @0x00F52 fuehrt direkt nach ROOM1090.
   Es fehlt ein Test, der im Fenster einen solchen Raum betritt und danach den Pegel bzw. den Sequenz-Zustand prueft.

2. **MAIN01 beginnt eine Szene zu frueh** (Punkt 3, Beginn). AUFTRAG.md Z.92 steht hinter der 1150-Montage
   ("Bis Leon dann den Parking Lot erreicht hat"). Gemessen: MAIN01 startet am Ende der 10F0-Szene (A, F2731) und
   gilt in ROOM1150 (J1: `room=15 entry=FF01` statt Tabelle 0xFF1E). Der Weg zu Irons und dessen Todesszene (Spur L)
   liefen damit unter MAIN01. Der Bau-Agent meldet das selbst als offene Lesart; nach dem Wortlaut ist es eine
   Abweichung. Stelle: `re15_cut10f0_bgm_eintrag` (cut_10f0.c:307) zusaetzlich auf (9,73) gaten (VERTRAG §1.1,
   Spur L) und die Raummusik am Montage-Ende anstossen.

3. **Zeile 6: Leon streckt den Arm an Ada vorbei** (Punkt 4). `state.log` F76-F166: `PL(8400,-350,rot=2048)`,
   `mo=15`, Nachricht 6. Ada steht bei (6000,11500): Soll-Gierung 2942, Abweichung 894/4096 = 78,6 Grad. Leons
   Kopf dreht erst nach der Zeile (`neck.log` Zeile 2 `PLC_NECK slot=0 mode=1 tgt=(6000,0,11500)`). Im Bild
   (Cut 0, F100-F160) zeigt der Arm im Profil auf die Westwand. Vergleichbarer Dialog: ROOM11C0 sub02 dreht
   (@0x0185E Plc_dest Modus 9) und blickt (@0x01886 Plc_neck) VOR der Zeile @0x018A0 mit Clip 15; die Spur wendet
   diese Regel selbst auf die Zeilen 10, 11, 12 und 16 an (Dossier §8.2), auf Zeile 6 nicht.
   `unit_r35_cut10f0_szene` prueft Zeile 6 nicht.

4. **Vertrag §1.4 "NUR kleine Haken (1-5 Zeilen)" in zwei gemeinsamen Dateien ueberschritten.**
   `engine/src/menu_common.c` +41/-2 (`git diff master --numstat`): Block in `map_mode` (rund 20 Zeilen), neue
   Funktion `hint_wechsel` (12), `menu_task_step` (6). `platform/pc/main.c` +43: neue Funktion `pc_rbj_leihen` (20),
   vier Haken zu 6/6/3/6 Zeilen.
   Risiko beim Zusammenfuehren mit den Spuren E, G und L, die dieselben Dateien anfassen.

## 9. Beobachtungen ohne Urteil

- **Ein Absturz, nicht zugeordnet.** Lauf G endete mit exit 139 (Segmentation fault) bei F3040, dem ersten
  `RE15_FRAMEDUMP`-Bild des Laufs (keine PPM geschrieben, `state.log` endet mit F3040), 4 s nach einem
  Rechner-Stillstand von 2,5 s (F2899 -> F2915). Drei identische Wiederholungen (G2 unter gdb, G3, G4) und H2 liefen
  durch; kein Eintrag im Windows-Ereignisprotokoll. Ursache nicht belegt, nicht als Mangel gewertet.
- **Zusammenfuehrung K <-> L.** "Parkplatz erreicht" haengt am Besucht-Bit der Zone ROOM11C0 (Kachel und Ende von
  MAIN01). Die Montage von Spur L laedt ROOM11C0 (Cut 13). Setzt dieser Raumaufbau das Besucht-Bit, hoert die
  Kachel auf zu blinken und MAIN01 endet, bevor Leon dort ist. Beim Zusammenfuehren messen; ein eigener Latch
  braucht ein freies Bank-9-Bit (84, 64, 66-70).
- War Leon vor der Szene schon im Parkplatz, blinkt die Kachel nicht und MAIN01 beginnt nicht (Dossier OFFEN, nicht
  nachgemessen).
- Leon traegt in der Szene das Messer in der Hand (Stand ohne Waffe); Sache der Spur E.
- Android und PSX nicht gemessen (BGM-Weiche und CONTINUE-Haken liegen in `platform/pc`).
- Belegbilder dieser Abnahme liegen nur im Sitzungs-Scratch (Kontaktboegen s1-s8, d1-d3, g2a/g2b, h1a/h1b, j1,
  k1); die Zahlen oben stammen aus den Logs derselben Laeufe.
