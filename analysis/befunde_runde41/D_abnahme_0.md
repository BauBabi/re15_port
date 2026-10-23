# Runde 41 — Spur D "kleinkram_a" — ABNAHME 0 (unabhaengig)

Geprueft: Zweig `r41/kleinkram_a`, HEAD 4cbbf4a2 (Basis master 116e8b0f), Baum `.claude/worktrees/r41_kleinkram_a`.
Datum 2026-10-23, 19:0x-19:5x. Gebaut mit `bash re15_port/tools/local_build.sh configure` + `build` (beide
`LOCAL-BUILD-OK`; ninja: alle Objekte juenger als die Quellen, exe enthaelt die Strings `RE2-Treffer (Spinne Slot`,
`Messhaken: HP :=`, `re15_options.cfg`). Messkopie `re15_port/build/platform/pc/re15_pc_abn0.exe`
(nie taskkill, jede Messung mit `timeout`, eigener Laufordner `scratchpad/abn0/<lauf>/`).
Wortlaut der drei Nutzer-Punkte: AUFTRAG.md (massgeblich), Spur-Zuteilung VERTRAG.md §0 Welle 1 D.

## 0. Ergebnis in einem Satz
Alle drei Punkte sind am gebauten Stand erfuellt und ueber den jeweiligen Wortlaut gemessen; RE-Gate und Vertrag
halten; es bleiben nicht blockierende Maengel (zwei veraltete Kommentare, eine ungenaue Instruktions-Adresse,
die Beta-KI-Variante) — siehe §7.

## 1. Punkt 1 — "Mir fehlt in den Optionen eine Lautstaerke Option bei der ich alles lauter oder leiser machen kann"

### 1.1 Messprotokoll (eigene Laeufe, Messkopie re15_pc_abn0.exe)
* Options-Schirm SOUND, `RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=sound.bmp RE15_CONFIG_TAB=4 RE15_NOAUDIO=1
  RE15_WINDOW_SCALE=1`, Optionsdatei per `RE15_OPTIONS_FILE` mit volume=50 / 100 / 0 sowie OHNE Datei:
  Bilder `scratchpad/abn0/a_shot_{50,100,0,default}/sound.png` (+ `_3x.png`, selbst angesehen): unter dem unteren
  Beschriftungskasten (Kastenende y 220) steht eine dritte Kachel derselben Bauart wie STEREO/MONO mit "VOLUME 50",
  "VOLUME 100", "VOLUME 0"; ohne Datei "VOLUME 100" und auf der Konsole
  `[lautstaerke] re15_options.cfg fehlt -> 100 %, Stereo`. Kein Original-Pixel des Kastens verdeckt (y 221..238).
* Wirkung "alles": vier deterministische Mitschnitte `RE15_AUDIO_CAP_SYNC=cap.raw RE15_AUDIO_CAP_FRAMES=330`
  (Leon ROOM2030, Waffe 8, Eingabeskript `M1,MA0.2,W5` ab Bild 20: Schuesse + Giftkugel-Treffer F19 + Raum-BGM,
  dieselbe Mischkette audio_callback wie das SDL-Geraet, audio_pc.c:3719) mit volume=100/50/25/0
  (`scratchpad/abn0/a_cap_{100,50,25,0}/cap.raw`, Werkzeug `test_r41_kleinkram_a.exe rms`):

  | volume | RMS (int16, beide Kanaele) | Samples | Verhaeltnis zu 100 |
  |---|---|---|---|
  | 100 | 1701,145 | 970200 | 1,000 |
  | 50  |  850,572 | 970200 | **0,5000** (500 Promille) |
  | 25  |  425,286 | 970200 | **0,2500** (250 Promille) |
  | 0   |    0,000 | 970200 | 0 (Stille) |

  -> lineare Endstufe Q15 (audio_pc.c audio_callback-Ende, NACH SE/XA/FMV/BGM/Ambience und der Mono-Faltung).
* Sound-Modus mitgespeichert und beim Start angewendet: Mitschnitt mit `volume=100 sound=mono`
  (`scratchpad/abn0/a_cap_mono/cap.raw`): L == R in allen 485100 Stereo-Frames (max|L-R| = 0), beim Stereo-Lauf
  max|L-R| = 4566, mittel 468. Konsole: `... opt.cfg gelesen (2 Schluessel): 100 %, Mono`.
* Echte Tastenkette im Options-Schirm (pc_run_config, `RE15_CONFIG_TEST=1 RE15_CONFIG_TAB=1` = TOP mit Cursor
  SOUND, Eingabeskript ab Tick 0):
  - Bestaetigen: `W5,A0.1,W0.6,L0.1,W0.5,L0.1,W0.5,L0.1,W0.5,A0.1,W0.6,R0.1,W0.5,A0.1,W3` OHNE RE15_OPTIONS_FILE
    (= Standardpfad, CWD wie re15_card.mcr): exit 0, Datei `scratchpad/abn0/a_tasten_confirm/re15_options.cfg` =
    `volume=70 / sound=stereo`, debug.log `[lautstaerke] re15_options.cfg geschrieben: 70 %, Stereo`.
  - Verwerfen: Datei vorher `volume=60 / sound=mono`, Kette `W5,A0.1,W0.6,R0.1,W0.5,R0.1,W0.5,X0.1,W0.6,R0.1,W0.5,
    A0.1,W3` (RIGHT RIGHT = 80, Cross): exit 0, Datei unveraendert `volume=60 / sound=mono`
    (`scratchpad/abn0/a_tasten_cancel/opt.cfg`), Konsole `gelesen (2 Schluessel): 60 %, Mono`.
* Riegel der Spur: `integration_r41_kleinkram_a_lautstaerke` selbst gefahren (ctest -R r41_kleinkram_a):
  Passed 50,19 s (RMS 500 Promille, Kachel-Randspalten gleich, Ziffern rechtsbuendig, Tastenkette volume=80).
* Unit: `unit_r41_kleinkram_a` Passed (Parser/Formatierer, Q15, Klemme, Arbeitskopie-Modell, Rundreise).

### 1.2 Belege (selbst gelesen, ghidra1_V2.txt:122560-122800 = FUN_8002f0d4, SOUND-Schirm)
* Kacheln: `ori t4,zero,0x58` @0x8002f0e0 (w 88), `ori t3,zero,0x12` @0x8002f0e4 (h 18), `ori t2,zero,0x74`
  @0x8002f0e8 (x 116), `ori t1,zero,0xe8` @0x8002f0ec (v 232), `ori v0,zero,0x33` @0x8002f118 + `sh v0,0xa(s1)`
  @0x8002f128 (y 51), `sb zero,0xc(s1)` @0x8002f12c (u 0), `ori v0,zero,0x46` @0x8002f17c (y 70). OK
* Eintritt laedt die Arbeitskopie: `lbu v0,DAT_800b0fce` @0x8002f244 / `sb v0,0x7(v1)` @0x8002f24c. OK
* Pad: `lhu v1,DAT_800ac762` @0x8002f268; `andi 0x40` @0x8002f270 (Cross -> Phase++ ohne Speichern @0x8002f27c-8c);
  `andi 0xa0` @0x8002f278 (Kreis/Quadrat -> `sh v0,DAT_800b0fce` @0x8002f2c0, `jal FUN_80043c00` @0x8002f2c8);
  `andi 0x5000` @0x8002f294 (UP|DOWN -> `xori v0,v0,0x1` @0x8002f2e8). Im gesamten Treiber 0x8002f0d4-0x8002f3ec
  gibt es KEIN `andi` mit 0x8000/0x2000 -> LEFT/RIGHT sind im Original ungenutzt. OK
* Es gibt in RE1.5 (und RE2 PSX) keine Lautstaerke-Option -> PORT-WAHL + NUTZER-VORGABE, so im Code gekennzeichnet
  (include/re15_lautstaerke_r41.h, lautstaerke_r41_pc.c). Zulaessig nach VERTRAG §2.

### 1.3 Urteil: **erfuellt.**
Option vorhanden (OPTIONS -> SOUND, LEFT/RIGHT 0..100 in Zehnerschritten, sofort hoerbar, Kreis/Quadrat speichert,
Cross verwirft), wirkt auf die gesamte Mischung (SE, Stimmen, FMV, BGM, Ambience — eine Endstufe am Ende der
Kette), wird in `re15_options.cfg` neben `re15_card.mcr` gespeichert und beim Start angewendet (mit dem Sound-Modus,
der vorher gar nicht gespeichert wurde).
Hinweis (kein Mangel): die VOLUME-Zeile ist eine reine Anzeige ohne Cursor/Pfeile; dass LEFT/RIGHT sie steuern,
steht nur im Dossier §6 (fuer die Release Notes uebernehmen).

## 2. Punkt 2 — "Feuerloescher in room 1030 einen leichten Blink Effekt ... existiert bereits in RE2, uebernehme den"

### 2.1 Messprotokoll
* Leon ROOM1030 Cut 0 (Nutzerbild fire_extinguisher.bmp): `RE15_DEBUG_JUMP=1030@250 RE15_PLAYER_POS=-11500,-19500,2048
  RE15_FRAMEDUMP=400-520/2:fl_ RE15_EXIT_AT=530#1030 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1` -> 61 Bilder
  (`scratchpad/abn0/b1_blink/fl_*.ppm`, Auswertung `scratchpad/puls.py`): Mittel (R+G+B)/3 der Loescher-Huelle
  x 38..48 / y 111..122 (Runde 38 Spur K) und der Pfeilerflaeche x 24..34 daneben:

  | Bild | Takt (Bild mod 60) | Huelle Mittel | hellstes Pixel | Pfeiler |
  |---|---|---|---|---|
  | 420 | 0  | 59,95 | 115 | 48,36 |
  | 436 | 16 | 64,23 | 139 | 48,36 |
  | 450/452 | 30/32 | 67,84 | 164 | 48,36 |
  | 480 | 0  | 59,95 | 115 | 48,36 |
  | 510/512 | 30/32 | 67,84 | 164 | 48,36 |

  Folge ueber alle 61 Bilder streng periodisch mit Periode 60 (Bild 420 == 480, 452 == 512), Hub des Mittels 7,89,
  hellstes Pixel 115 -> 164 (+42,6 %), Pfeiler Hub 0,00. Bilder `crop_420.png` (Grund) / `crop_452.png` (Spitze)
  selbst angesehen: der Loescher am Pfeiler unter dem Monitor ist in der Spitze deutlich heller rot, die Umgebung
  unveraendert — ein weicher Helligkeitspuls (2 s), nicht ein hartes An/Aus.
* ECHTER WEG (Tuer): `RE15_DEBUG_JUMP=1040@250` + `RE15_FIRE_AOT=1@420#1040` (= Tuer 1040 Slot 1 @0x010B8 -> ROOM1030,
  derselbe Pfad wie das Hineinlaufen, Scan -> aot_fire_door): debug.log `[aot] DOOR FIRE slot=1 ... spawn=(-26214,0,-3861)`,
  `[tuer] Sequenz Archiv 2 DOOR07 ...`, `[room] PC loaded room1030.rdt`, `[feuerloescher] ROOM1030: Loescher an der Wand
  obj 5 bei (-2813,-2122,-17904) rot_y 3072, Zone Slot 14 Aufnahme Item 0x31 Bit (9,0x86)` = derselbe Installer
  (feuerloescher_r37.c, dort steht der Haken `re15_blink_r41_setzen(room_id, 5, zone_item)`). Der Tuer-Spawn liegt in
  Cut 8, dort ist der Loescher nicht im Bild (Bilddifferenz 720 vs 752 zeigt nur den Spieler), der Puls selbst ist
  deshalb in Cut 0 gemessen (oben) — er haengt nur an (Raum, Obj-Id) und am Bildzaehler, nicht am Eintrittsweg.
* Riegel: `integration_r41_kleinkram_a_blink` Passed 43,19 s; `unit_r41_kleinkram_a` Teil B (wert(n) ==
  Titelpuls fuer n = 0..130) Passed.

### 2.2 Belege (selbst gelesen)
* RE2 hat KEIN Blinken aufnehmbarer Gegenstaende — stichprobenartig nachgelesen (ghidra_re2_Leon.txt):
  Item_aot_set FUN_80054cd4: `lbu s2,0x14(s0)` @0x80054cf8, `lui v1,0x8000` @0x80054db0 / `sw v1,0x413c(v0)`
  @0x80054db4 (Om.word0 := 0x80000000), `sb s1,0x4281(v1)` @0x80054de4 (+0x145 := action), `sw` @0x80054dec — kein
  Zaehler, keine Farbe. Objektpass FUN_800363f0 (`jal` @0x80026964): `jal FUN_80076f88` @0x800364ac, SetColorMatrix
  @0x800364bc, SetLightMatrix @0x800364d8; die einzigen Nicht-Om-Globals, die der Pass liest, sind DAT_800ce338
  (Zeiger) und das Byte DAT_800ce5e0, das als Doppelpuffer-Index (`sll 2/addu/sll 3` = x40 bzw. x52) an die Zeichner
  FUN_8002d3c8/d718/d0e8/d244 geht — kein periodischer Farbwert; der Zweig +0x14c & 8 (@0x80036560) zaehlt +0x1ec bis
  0x1e und loescht word0 (@0x800365bc-dc) = einmaliges Ausblenden. Der Nutzer-Glaube "existiert in RE2" ist damit
  ehrlich als nicht zutreffend belegt; "uebernehme den" war nicht erfuellbar.
* FORM (PORT-WAHL mit belegter Form aus dem eigenen Spiel) = Titelpuls FUN_801028ec, selbst disassembliert
  (`re15_disasm.py dis 0x801028ec 24 --bin TITLE.BIN`): `sltiu v0,v1,0x1f` @0x801028fc, `addiu a0,a0,2` @0x8010290c,
  `addiu a0,a0,-2` @0x80102910, `addiu v1,v1,1` @0x80102914, `ori v0,zero,0x3c` @0x80102918, `ori v1,zero,0x0`
  @0x80102924, `ori a0,zero,0x80` @0x80102928. OK — alle Konstanten in include/re15_blink_r41.h mit diesen Adressen.
* RE1.5-Props: flach 0x808080 bei attr 0x10, sonst NCCT (FUN_8002c18c) — der Loescher ist attr 0 (feuerloescher_r37.c
  RE15_FL37_PROP_ATTR, @0x8002c2f4), also im NCCT-Pfad, wo der Puls den Lichtkontext skaliert.

### 2.3 Urteil: **erfuellt** (im Sinn des Wortlauts "leichter Blink Effekt, dass man sieht, dass er aufnehmbar ist").
Der Teil "uebernehme den aus RE2" ist belegt nicht erfuellbar (RE2 hat keinen); die Spur hat das nicht verschwiegen,
sondern die Form aus dem eigenen Spiel (Titelpuls) genommen und als PORT-WAHL markiert. ENTSCHEIDUNG fuer den Nutzer
(Dossier §6): ob die Form (weicher 2-s-Puls bis +48 %) oder ein hartes Blinken gewuenscht ist, und ob auch Original-
Item-Props pulsieren sollen. API `re15_blink_r41_setzen(raum, obj_id, an)` fuer Spur F vorhanden (nur NCCT-Props).

## 3. Punkt 3 — "Der Spieler kann nicht sterben, wenn er noch mal mit Gift von den Spinnen angeschossen wird, aber bereits auf Danger ist"

### 3.1 Messprotokoll (RE2-KI = Voreinstellung)
* NATUERLICHER WEG ohne Messhaken, Leon ROOM2030 (`RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3`,
  keine Eingabe, `RE15_R39_LOG=gift.log RE15_STATE_LOG=state.log`, `scratchpad/abn0/c1_leon_natur/`):
  `[giftkugel] F19 RE2-Treffer (Spinne Slot 1): HP 100 -> 80 (Kommando 2)`, `F580 62 -> 42`, `F639 41 -> 21`,
  Gift-Takt dazwischen (-1 HP / 31 Bilder) bis HP 9 = DANGER, dann
  `[giftkugel] F1041 RE2-Treffer (Spinne Slot 1): HP 9 -> -11 = TOD (Kommando 3)`; state.log `F1041 ... hp=-11 pst=3`
  durchgehend 114 Bilder (F1041..F1154, Todes-Clip mo=7), danach Neuladen (gift.log `F0 aktiv=1 hp=100`).
  (Vorher laut Dossier §3.1 am Stand 116e8b0f: 17 -> 7 ueberlebt, erst 1 -> -9 toedlich = der Nutzerbefund.)
* NATUERLICHER WEG Elza ROOM2031 (`RE15_TITLE_SHOT_CHAR=1`, debug.log `[pl] Spieler-Familie PL04 (character=4,
  Elza-Bit=1)`, `scratchpad/abn0/c3_elza_natur/`): identische Kette F19/F580/F639, `F1041 HP 9 -> -11 = TOD
  (Kommando 3)`, pst=3 120 Bilder, Neuladen hp=100.
* GRENZE Danger: Messhaken `RE15_R41_HP_AT=5:20` (HP 20 = Caution, Danger ist HP < 20): `F19 RE2-Treffer: HP 20 -> 0
  (Kommando 2)`, state.log `F19 hp=0 pst=2 ps1=2` (Trefferreaktion), kein pst=3 — lebt. Mit HP 15 (Riegel der Spur):
  `HP 15 -> -5 = TOD (Kommando 3)`.
* Riegel: `integration_r41_kleinkram_a_gift` (Leon HP 15 -> Tod, Gegenprobe HP 100 -> 80 lebt, Elza 2031 -> Tod):
  siehe §6 (Lauf). `unit_r41_kleinkram_a` Teil C (HP 15/19 -> Tod, 30 -> 10, 20 -> 0 lebt, Tore, RE1.5-Gehirn nicht
  zustaendig) Passed; `unit_r38_spinnen` (RE2-Lauf jetzt 100 - 20, RE1.5-Lauf 100 - 10) Passed.
* Nebenwirkung Treffer-Rueckgabe: re15_resolve_attack zaehlt den Spielertreffer auch bei gesetztem hit-once
  (`hits++` @80012efe-Analog, re15_damage.c:3648) -> der Klumpen schlaegt weiter auf (AUFSCHLAG-Zweig), nur der
  HP-Abzug des RE1.5-Zweigs entfaellt (Guard @0x80012e24-30). Szenenschutz (hit_react Bit 0 gesetzt) und Griff
  sperren den neuen Pfad genauso (giftkugel_r41.c Tore).

### 3.2 Belege (selbst gelesen, ghidra_re2_Leon.txt / ghidra1_V2.txt)
* RE2 Op 32: `li a1,0x320` @0x800200d0 (Radius 800), `li a2,0x14` @0x800200dc (**Schaden 20**), `jal FUN_80047664`
  @0x800200e0. OK
* FUN_80047664: Tore `andi v0,v0,0x1` @0x800476e0, `lbu v0,0x1d3(s0)`/`bne` @0x800476ec-f4, `lh v0,0x156(s0)`/`bltz`
  @0x800476fc-704, `andi 0xc000` @0x80047714; Treffer `sw s8,0x4(s3)` @0x800477b8 mit s8 = 2 (`li s8,0x2` @0x800476c8)
  = Kommando 2; Spieler `jal FUN_800401d4` @0x800477fc (a0 = s6 = Schaden, a1 = 0); `bne v0,s8` @0x80047804 ->
  `sw s7,0x4(s3)` @0x8004780c mit s7 = 3 (`li s7,0x3` @0x800476cc) = Kommando 3 = TOD. OK
* FUN_800401d4 Modus 0: HP -= dmg (`subu a0,v0,a0` @0x80040250, `sh a0,0x156(a2)` @0x8004025c); `bgez` @0x80040284
  -> 0 (lebt); `slti v0,v0,-0xe` @0x80040288 / Latch `andi 0x1000` @0x8004029c -> Todeszweig @0x800402c0-e4
  (`DAT_800cfdcb |= 0x80` @0x800402c0-d0, `DAT_800cfb74 |= 0x04000000` @0x800402d4-e4, v0 = 2); sonst HP := 0 + Latch
  (`sh zero,0x156` @0x800402b0, `ori 0x1000` @0x800402b4, v0 = 1). Der Port laesst diesen Rettungszweig fuer die
  Giftkugel bewusst weg (NUTZER-VORGABE, im Header begruendet) — damit ist bei Danger JEDER Treffer toedlich.
* Danger-Schwelle: FUN_8004ed6c `lb v1,DAT_800112b5` @0x8004ed7c (= 0x14), `lh a0,DAT_800acaee` @0x8004edb8,
  `slt v1,a0,v1` @0x8004ede0 -> HP < 20 = 2 (Danger), sonst 1 (Caution). OK (deshalb 20 -> 0 = lebt, 19 -> -1 = Tod)
* RE1.5 (Beta): Routine 25 `ori a0,zero,0x1f4` @0x80017ffc (Kasten 500), FUN_80012d60(500,&P,0) @0x80018008,
  Art 0 = 10 HP, Tod nur HP < 0 (`bgez` @0x80012ee8) — unveraendert fuer das RE1.5-Gehirn.

### 3.3 Urteil: **erfuellt** (unter der RE2-KI, der Voreinstellung seit 2026-08-22).
Der Fix ERKLAERT den Befund: Beta-Schaden 10 < Danger-Spanne 1..19 -> Ueberleben moeglich; RE2-Schaden 20 >= jede
Danger-HP -> HP < 0 -> Kommando 3. Einschraenkung (Hinweis, kein Mangel am Wortlaut): unter der Option "AI RE1.5"
bleibt der byte-true Beta-Wert 10, dort ueberlebt der Spieler bei HP 10..19 weiterhin (Messung §6 C4). Die
RE2-Rettung "einmal je Raum" (FUN_800401d4 Latch) ist fuer die Giftkugel bewusst nicht uebernommen — ENTSCHEIDUNG
fuer den Nutzer (Dossier §6 (2)).

## 4. RE-Gate
* Alle verhaltensrelevanten Konstanten im Diff tragen `@0x…` oder sind als PORT-WAHL / NUTZER-VORGABE gekennzeichnet
  (Lautstaerke: Form @0x8002f0e0-12c/@0x8002f244-4c/@0x8002f2c0/@0x8002f27c, Wirkung PORT-WAHL; Blink: FUN_801028ec;
  Giftkugel: @0x800200dc, @0x800477b8/@0x8004780c, @0x800402c0-d0, @0x800112b5, @0x80017ffc).
* Selbst disassembliert/gelesen (8 Stellen, alle wie behauptet): FUN_8002f0d4 (ghidra1_V2.txt:122560-122800),
  FUN_8004ed6c (@0x8004ed7c/@0x8004ede0), RE2 Op 32 (@0x800200d0/dc/e0), FUN_80047664 (@0x800476e0-@0x8004780c),
  FUN_800401d4 (@0x800401d4-@0x800402ec), TITLE.BIN FUN_801028ec (re15_disasm.py), RE2 FUN_80054cd4 (@0x80054cf8/
  db0/db4/de4/dec), RE2 FUN_800363f0 (@0x800364ac/bc/d8, @0x800365bc-dc, Globals-Zensus).
* Guess-Tells (`deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME`) im Code-Diff: keine Treffer.
* Kein Env-Schalter als Abschluss: RE15_OPTIONS_FILE / RE15_R41_HP_AT sind Messschienen (Standardpfad bzw. aus).
* Der Fix erklaert den gemessenen Befund (§3.3), die Messung nachher widerspricht dem Befund (§3.1).

## 5. Vertrag (VERTRAG.md)
* Pfade (`git diff master --name-only`): nur engine/src, include, platform/pc, tests/{unit,integration,unit/probes},
  Dossier. Kein release/, platform/android/, shared_assets/, pri/, synchro/, keine Test-CMakeLists (Registrierung nur
  ueber `tests/unit/probes/r41_kleinkram_a.cmake`, GLOB). OK
* Bank-9-Bits: keine neuen; Nachrichten-Ids: keine; Assets: keine. OK
* Gemeinsame Dateien: main.c (D zugeteilt) extern-Block 7 Zeilen + 6 Ein-Zeilen-Haken (Start, TOP->SOUND, Shot-TAB-4,
  Overlay, Eingabe/Confirm/Cancel, Prop-Lichtkontext), audio_pc.c 4 Zeilen (D zugeteilt), feuerloescher_r37.c 2,
  spinnen_r38.c 2 (Datei ist Spur L zugeteilt — Haken ist zwei Zeilen VOR dem RE1.5-Resolver, bei der Integration
  gegen L mergen), game_step_common.c 2 (Datei ist Spur H zugeteilt — reine Messschiene). Alle mit "Runde 41 Spur D".
* Tests je Punkt vorhanden und messend (eigener Laufordner mit Zufalls-Id, exe-Kopie `re15_pc_r41d_<thema>_<id>`,
  cmake_minimum_required 3.21, kein Backslash, `m` unter Nicht-WIN32, `_DEFAULT_SOURCE`). OK
* Beide Szenarien gemessen (Leon 2030 / Elza 2031; Loescher nur in 1030 = Leon, 1031 hat keinen; Lautstaerke
  szenario-unabhaengig). Nachbarraeume: ROOM1040 -> 1030 Tuerweg gemessen (§2.1).

## 6. Testlaeufe (eigene, im Spurbaum, nur `ctest -R`)
* `ctest -R "r41_kleinkram_a" -j 2 --timeout 1800`: unit_r41_kleinkram_a Passed 0,01 s;
  integration_r41_kleinkram_a_lautstaerke Passed 50,19 s; integration_r41_kleinkram_a_blink Passed 43,19 s;
  integration_r41_kleinkram_a_gift: Passed 851,75 s (§9).
* Unit-Nachbarn `ctest -R "unit_r41_kleinkram_a|unit_r38_spinnen|unit_r39_zustand|unit_r37_feuerloescher|
  unit_r38_itemorte|unit_damage|unit_bleed_rng|r30_titel_blinken|unit_audio"`: **100% tests passed, 0 failed out of 22**.
* Integrations-Nachbarn `ctest -R "integration_r38_spinnen_echtlauf|integration_r37_feuerloescher|
  integration_r38_itemorte|integration_r39_zustand|integration_r30_titel_puls|integration_r39_johnruf1041" -j 1`:
  durch das Sitzungslimit abgebrochen — Ergebnis in §10.4/§10.5 (Sitzung 2).
* C4 (RE1.5-KI, `RE15_AI_FLAVOR=re15`, HP 15 per Messhaken): siehe §9.

## 7. Maengel (nummeriert; keiner blockiert die Abnahme)
1. Veralteter Kommentar `platform/pc/main.c` (Overlay-Haken, ~Z. 2740): "VOLUME-Zeile, dritte Kachel y 89 im Raster
   @0x8002f128/@0x8002f17c" — die Zeile liegt gemessen und im Riegel bei y 221 (lautstaerke_r41_pc.c LAUT_PC_KACHEL_Y).
   Ebenso `tests/unit/probes/r41_kleinkram_a.cmake` Kopf: "dritte Kachel y 89". Beide auf y 221 korrigieren.
2. Ungenaue Instruktions-Adresse fuer x = 116: `lautstaerke_r41_pc.c` LAUT_PC_KACHEL_X "@0x8002f124 `ori t2,zero,0x74`"
   und Dossier §1.3 — der `ori t2,zero,0x74` steht @0x8002f0e8; @0x8002f124 ist der Store `sh t2,0x8(s1)` (selbst
   gelesen). Wert richtig, Zitat auf @0x8002f0e8 (ori) bzw. @0x8002f124 (sh) praezisieren.
3. Die Zeile `[lautstaerke] … gelesen/fehlt` faellt vor die stderr-Umleitung (main.c: pc_start vor dem debug.log-
   Umbau) und steht nur auf der Konsole — fuer den Nutzer (GUI-exe ohne Konsole) unsichtbar. Aufruf hinter die
   Umleitung ziehen oder die Zeile beim ersten Spielbild nachholen (nur Diagnose, kein Spielverhalten).
4. Beta-KI: unter "AI RE1.5" bleibt der Nutzerbefund (Giftklumpen 10 HP, Ueberleben bei HP 10..19) bestehen — byte-true
   zum Original, aber der Nutzer-Wortlaut gilt dort nicht. Entweder im Release-Text nennen oder die RE2-Giftkugel
   auch dort anbieten (Nutzer-Entscheidung).
5. Hinweis Integration: spinnen_r38.c (Spur L) und game_step_common.c (Spur H) tragen je einen 2-Zeilen-Haken von D.

## 8. Fuer Release Notes / Nutzer (aus den Messungen)
* OPTIONS -> SOUND: LEFT/RIGHT = Lautstaerke 0..100 (Zehnerschritte, sofort hoerbar), Kreis/Quadrat speichert (mit
  Stereo/Mono), Cross verwirft; Datei `re15_options.cfg` neben `re15_card.mcr`; loeschen = 100 %, Stereo.
* Feuerloescher 1030 pulsiert (2 s, bis +48 % Helligkeit); RE2 hat kein Item-Blinken (belegt) — Form = Titelpuls.
  Frage: so belassen oder hartes Blinken; auch Original-Items?
* Giftkugel (RE2-KI): 20 HP wie RE2; bei Danger (HP < 20) ist jeder Treffer toedlich; HP 20 (Caution) -> 0 lebt.
  Frage: RE2-Rettung "einmal je Raum" (FUN_800401d4) auch hier? Unter "AI RE1.5" weiterhin Beta-Wert 10.

## 9. Nachtraege (fortlaufend)
* 19:2x — `ctest -R "r41_kleinkram_a"` komplett: **100% tests passed, 0 tests failed out of 4**
  (unit 0,01 s, lautstaerke 50,19 s, blink 43,19 s, gift 851,75 s).
* 19:2x — C4 (RE1.5-KI, `RE15_AI_FLAVOR=re15`, Messhaken HP 15, 400 Bilder, `scratchpad/abn0/c4_re15_flavor/`):
  kein `[giftkugel]`-Treffer (nicht zustaendig, wie der Unit-Riegel "RE1.5-Gehirn: nicht zustaendig"); die RE1.5-
  Spinnen spuckten in 400 Bildern nicht (HP blieb 15, pst=1) — der Beta-Pfad (Routine 25 -> FUN_80012d60, 10 HP,
  Tod nur HP < 0 @0x80012ee8) ist im Diff unveraendert (spinnen_r38.c: Haken vor dem Resolver, Resolver-Aufruf
  unveraendert). Mangel 4 bleibt als Hinweis.

## 10. Fortsetzung der Abnahme 0 (Sitzung 2, 2026-10-23 ab 21:4x — Sitzungslimit hatte §6 offen gelassen)

### 10.1 Stand
* HEAD jetzt 69142b95 = 4cbbf4a2 + zwei Doku-Commits (20caeac3 dieser Bericht, 69142b95 AUFTRAG D1-D3).
  `git diff 4cbbf4a2 HEAD --stat`: nur analysis/befunde_runde41/{AUFTRAG.md,D_abnahme_0.md} — der Code-Stand ist
  unveraendert. Baum sauber (`git status --short` leer).
* Bau erneut: `bash re15_port/tools/local_build.sh build` -> Preflight OK, `ninja: no work to do`, `LOCAL-BUILD-OK
  (build)`. exe re15_pc.exe 18:32:16 ist juenger als jede Quelle des Diffs (juengste: main.c 17:47:39,
  lautstaerke_r41_pc.c 17:42:56). `cmp re15_pc.exe re15_pc_abn0.exe` = IDENTISCH (md5 9a89f3d700698e8c605669bff2b822bb)
  — alle Messungen aus §1-§3 und §9 gelten damit fuer den gebauten Stand dieses Berichts.
* Mess-Artefakte aus §1-§3 nachgeprueft (scratchpad/abn0/...): c1_leon_natur/debug.log enthaelt `[giftkugel] F19 ... 100 -> 80`,
  `F580 62 -> 42`, `F639 41 -> 21`; state.log F1041..F1154 `hp=-11 pst=3`, F1155 pst=7, danach `F1041 ... hp=100 pst=0`
  (Neuladen); c3_elza_natur: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)` + `F1041 HP 9 -> -11 = TOD (Kommando 3)`;
  c2_hook20: `F19 HP 20 -> 0 (Kommando 2)`, state.log F19 `hp=0 pst=2 ps1=2`; a_tasten_confirm/re15_options.cfg =
  `volume=70 / sound=stereo` + `geschrieben: 70 %, Stereo`; a_tasten_cancel/opt.cfg unveraendert `volume=60 / sound=mono`;
  a_cap_{100,50,25,0,mono}/konsole.txt je `gelesen (2 Schluessel): <p> %, <Stereo|Mono>`; b1_blink 72 Dateien (61 ppm +
  crops); b2_tuer1040/debug.log `[aot] DOOR FIRE slot=1 ... spawn=(-26214,0,-3861)`, `[room] PC loaded room1030.rdt`,
  `[feuerloescher] ROOM1030: Loescher an der Wand obj 5 ...`.

### 10.2 Orchestrator-Entscheide D1-D3 (AUFTRAG.md 21:37) gegen den Code geprueft
* D1 "Puls nur fuer Port-Gegenstaende": einziger Aufrufer von `re15_blink_r41_setzen` ist feuerloescher_r37.c:195 mit
  `g->zone_item` (k_lagen: 0x1030 zone 1, 0x1090/0x1091 zone 0); der Render-Haken main.c:10935 fragt die Tabelle je
  (Raum, oid) ab. In ROOM1030 Cut 0 werden drei Props gezeichnet (b1_blink/debug.log `[prop-render] pi=0 oid=0x00`,
  `pi=1 oid=0x05`, `pi=2 oid=0x0F`), nur 0x05 ist eingetragen -> Original-Props bleiben wie im Original. Unit-Riegel
  "andere Raum-/Objekt-Ids bleiben inaktiv" deckt das ab. ERFUELLT.
* D2 "kein One-Save": giftkugel_r41.c:159-167 — `if (pl->hp < 0) { re2z_self1d3 |= 0x80; state = 3; ... return 2; }`,
  kein Latch, kein `HP := 0`-Zweig; Header re15_giftkugel_r41.h kennzeichnet das als NUTZER-VORGABE. Messung §3.1
  (HP 9 -> -11 = TOD, beide Szenarien). ERFUELLT.
* D3 "VOLUME-Zeile y 221": lautstaerke_r41_pc.c `LAUT_PC_KACHEL_Y 221`, `LAUT_PC_TEXT_Y 223`; Riegel vergleicht
  y 221..238 (Zeilen 2..15) mit Kachel 1; Shot a_shot_50/sound.png (§1.1). ERFUELLT.

### 10.3 RE-Gate, Stichproben dieser Sitzung (selbst gegriffen, nicht aus dem Dossier uebernommen)
* ghidra1_V2.txt:122564-122567 `8002f0e0 ori t4,zero,0x58` / `8002f0e4 ori t3,zero,0x12` / `8002f0e8 ori t2,zero,0x74`
  / `8002f0ec ori t1,zero,0xe8`; :122581 `8002f124 sh t2,0x8(s1)`, :122582 `8002f128 sh v0,0xa(s1)`, :122583
  `8002f12c sb zero,0xc(s1)`, :122603 `8002f17c ori v0,zero,0x46`; :122655 `8002f244 lbu v0,DAT_800b0fce`, :122657
  `8002f24c sb v0,0x7(v1)`; :122667 `8002f270 andi v0,v1,0x40`, :122669 `8002f278 _andi v0,v1,0xa0`, :122677
  `8002f294 _andi v0,v1,0x5000`, :122688 `8002f2c0 sh v0,DAT_800b0fce`, :122699 `8002f2e8 xori v0,v0,0x1`.
  -> Mangel 2 (§7) bestaetigt: der `ori t2,zero,0x74` steht @0x8002f0e8, @0x8002f124 ist der Store.
* Danger: ghidra1_V2.txt:74727 `800112b5 undefined1 14h`, :169708 `8004ed7c lb v1,DAT_800112b5`, :169734
  `8004ede0 slt v1,a0,v1`. Routine 25 / Damage: :84671 `80017ffc ori a0,zero,0x1f4`, :84674 `80018008 jal FUN_80012d60`,
  :77709 `80012ee8 bgez v1`, :77659 `80012e24 lbu v0,DAT_800acae7`, :77678 `80012e70 jal FUN_800453d0`.
* RE2 (ghidra_re2_Leon.txt): :100360 `800200d0 li a1,0x320`, :100363 `800200dc li a2,0x14`, :100364 `800200e0 jal
  FUN_80047664`; :152330/1 `800476c8 li s8,0x2` / `800476cc li s7,0x3`, :152337 `800476e0 andi v0,v0,0x1`, :152340
  `800476ec lbu v0,0x1d3(s0)`, :152344/6 `800476fc lh v0,0x156(s0)` / `80047704 bltz`, :152394 `800477b8 _sw s8,0x4(s3)`,
  :152412 `800477fc jal FUN_800401d4`, :152414 `80047804 bne v0,s8`, :152416 `8004780c sw s7,0x4(s3)`; FUN_800401d4
  :143377 `80040250 subu a0,v0,a0`, :143380 `8004025c _sh a0,0x156(a2)`, :143391/2 `80040284 bgez` / `80040288 _slti
  v0,v0,-0xe`, :143397 `8004029c andi v0,v0,0x1000`, :143403/4 `800402b0 sh zero,0x156(a2)` / `800402b4 ori v1,v1,0x1000`,
  :143408/12 `800402c0 lbu v1,0x1d3(a2)` / `800402d0 sb v1,0x1d3(a2)` (DAT_800cfdcb), :143413/17 `800402d4 lw` /
  `800402e4 _sw` DAT_800cfb74; Item_aot_set :173924 `80054cf8 lbu s2,0x14(s0)`, :173973/4 `80054db0 lui v1,0x8000` /
  `80054db4 sw`, :173987 `80054de4 _sb s1,0x4281(v1)`, :173989 `80054dec sw`. Alle wie im Diff zitiert.
* TITLE.BIN (info/Re1.5/PSX/BIN/TITLE.BIN, 11832 Byte, identisch mit shared_assets; KEIN 0x800-Header: Datei-Offset =
  Adresse - 0x80100000): Bytefolge `2c62001f` (sltiu v0,v1,0x1f) genau einmal @0x28fc = 0x801028fc; `24840002`
  (addiu a0,a0,2) @0x290c; `2484fffe` (addiu a0,a0,-2) @0x2910; `24630001` @0x2914; `3402003c` (ori v0,zero,0x3c)
  @0x2918; `34040080` (ori a0,zero,0x80) @0x2928. Woerter ab 0x28ec: 3c038010 94632946 3c048010 94842944 2c62001f
  10400003 00000000 08040a45 24840002 2484fffe 24630001 3402003c 14620003 00000000 34030000 34040080 — die Konstanten
  in include/re15_blink_r41.h stimmen (und sind dieselben wie include/re15_title_pulse.h).
* Guess-Tells im Code-Diff (`git diff master...HEAD -- re15_port`, nur +-Zeilen, deferred|tunable|interim|for now|
  faithful|plausibel|TODO|FIXME|hack): keine Treffer.
* Pfade (`git diff master...HEAD --name-only`, 21 Dateien): analysis/ (3), engine/src (6), include (3), platform/pc (3),
  tests/{integration,unit,unit/probes} (6). Kein release/, platform/android/, shared_assets/, pri/, synchro/, keine
  Test-CMakeLists (Registrierung ueber tests/unit/probes/r41_kleinkram_a.cmake, GLOB). OK.
* re15_options.cfg liegt wirklich "neben re15_card.mcr": beide CWD-relativ (main.c:1605 `RE15_CARD_PATH "re15_card.mcr"`,
  lautstaerke_r41.h RE15_LAUT_R41_DATEI), CWD = exe-Ordner (main.c:3242). OK. Mangel 3 im Code nachvollzogen:
  `re15_lautstaerke_r41_pc_start()` main.c:3249 liegt VOR `freopen("debug.log","w",stderr)` main.c:3271.

### 10.3a Beleg "alles lauter oder leiser" (statisch, audio_pc.c audio_callback, Zeilen 416-521)
Reihenfolge im Rueckruf (selbst gelesen): `SDL_memset(stream, 0)` -> SE-Slots (s_active[], je Voice Q15-Pegel und Tail-Fade)
-> XA-Stimmen (`s_xa`, cd_vol_l/r >> 14) -> FMV-Ton (`s_fmv_audio`, cd_vol >> 7) -> BGM `re15_ss_render_bgm(out, frames)`
-> Ambience `re15_amb_mix(out, frames)` -> Mono-Faltung (`s_audio_mono`) -> **Endstufe der Spur** (q != 0x8000: out[i] =
out[i] * q >> 15 ueber frames*2 Samples) -> `fwrite` des Cap-Hakens. Es gibt keinen Ausgabepfad an der Endstufe vorbei;
der Mitschnitt RE15_AUDIO_CAP_SYNC (§1.1) nimmt hinter der Endstufe ab, deshalb ist das gemessene RMS-Verhaeltnis 0,5000
der Beweis fuer die ganze Mischung.

### 10.4 Nachbar-Echtlaeufe (Abschluss von §6)
* Sitzung 1 (LastTest.log.tmpcef94, Start 19:07, abgebrochen durch das Sitzungslimit 19:13): integration_r30_titel_puls
  **Passed**, integration_r37_feuerloescher **Passed**.
* Sitzung 2: `ctest --test-dir re15_port/build -R "integration_r38_itemorte|integration_r38_spinnen_echtlauf|
  integration_r39_johnruf1041|integration_r39_zustand" -j 1 --timeout 900 --output-on-failure` (12 Tests) — Ergebnis
  in §10.5.

### 10.5 Ergebnis der Nachbar-Echtlaeufe (Sitzung 2, Start 21:45:40, -j 1, exe md5 9a89f3d7...)
```
 1/12 integration_r38_itemorte                 Passed  404.59 s
 2/12 integration_r38_spinnen_echtlauf         Passed  120.76 s
 3/12 integration_r39_johnruf1041_a            Passed   44.54 s
 4/12 integration_r39_johnruf1041_b            Passed  281.13 s
 5/12 integration_r39_johnruf1041_tuer_t1041   Passed   26.49 s
 6/12 integration_r39_johnruf1041_tuer_t1041o  Passed   35.68 s
 7/12 integration_r39_johnruf1041_tuer_t1051   Passed   29.10 s
 8/12 integration_r39_johnruf1041_tuer_t1051o  Passed   32.07 s
 9/12 integration_r39_johnruf1041_tuer_t11b1   Passed   27.59 s
10/12 integration_r39_johnruf1041_tuer_t11b1d  Passed   29.50 s
11/12 integration_r39_zustand_kleidung         Passed  184.96 s
12/12 integration_r39_zustand_gift             Passed   99.73 s
100% tests passed, 0 tests failed out of 12   Total Test time (real) = 1316.24 sec
```
(Log: scratchpad/abn0/ctest_nachbarn_rest.log. Waehrend des Laufs liefen parallel Echtlauf-Riegel der Baeume r41_re2gator
und r41_ventilkette — keine Rotfaerbung, kein Nachfahren noetig.) Damit sind alle in §6 angesetzten Nachbarn gruen:
Unit-Nachbarn 22/22, Spur-Riegel 4/4 (§9), Integrations-Nachbarn 14/14 (2 aus Sitzung 1 + 12 aus Sitzung 2).

### 10.6 Schluss (Sitzung 2)
* Urteile unveraendert: Punkt 1 **erfuellt** (§1.3), Punkt 2 **erfuellt** (§2.3; "uebernehme den aus RE2" belegt nicht
  erfuellbar, PORT-WAHL mit belegter Form, Entscheid D1 eingehalten), Punkt 3 **erfuellt** (§3.3; Entscheid D2 eingehalten).
  RE-Gate (§4, §10.3) und Vertrag (§5) halten; D3 eingehalten (§10.2).
* Maengel: §7 Nr. 1-5 bleiben (nicht blockierend; 1 und 2 sind reine Kommentar-/Zitatkorrekturen fuer die Integration,
  3 ist Diagnose-Sichtbarkeit, 4 ist eine Nutzer-/Release-Note-Entscheidung, 5 ein Integrationshinweis). Neu nur ein
  HINWEIS ohne Mangelstatus: der Riegel integration_r41_kleinkram_a_gift braucht 851 s (drei Laeufe a 700 Bilder mit
  RE15_STATE_LOG) — innerhalb seines TIMEOUT 1800, aber ein spuerbarer Posten fuer die volle Suite im Release-Container.
* ABNAHME 0: **BESTANDEN** (bestanden = true), Maengelliste §7 zur Nachbesserung in der Integration.
