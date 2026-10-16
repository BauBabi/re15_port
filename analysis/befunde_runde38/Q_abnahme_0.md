# Runde 38 — Spur Q "kleinkram": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-16). Baum `.claude/worktrees/r38_kleinkram`, Zweig `r38/kleinkram`, Kopf 7271cf80,
Basis 1bcbd0ff (= merge-base mit master; `git diff master...HEAD`). Massgeblich: `AUFTRAG.md` (Wortlaut), `VERTRAG.md`.
Nichts aus dem Bau-Dossier wird ohne eigene Messung uebernommen.

STAND: in Arbeit (fortlaufend geschrieben).

## 0. Bau

* `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
  `LOCAL-BUILD-OK (build)`, EXIT 0. Die exe (17:03:19) ist juenger als jede geaenderte Quelle (elza_r36.c 17:02:15,
  kleinkram_r38.c 17:03:13, menu_common.c 17:02:44, scd_vm.c 17:01:47, re15_kleinkram_r38.h 17:00:14), kleinkram_r38.c.obj
  17:03:18 -> gebauter Stand == Kopf 7271cf80 (Baum sauber, `git status` clean).
* ctest -N: 794 Tests registriert (788 + 6 neue der Spur).

## 1. Code-Diff (gelesen)

`git diff master...HEAD -- re15_port`: neu `engine/src/kleinkram_r38.c`, `include/re15_kleinkram_r38.h`,
`tests/unit/test_r38_kleinkram.c`, `tests/unit/probes/r38_kleinkram.cmake`, `tests/integration/test_r38_kleinkram.cmake`;
Haken in `menu_common.c` (include + 3 Rufstellen), `scd_vm.c` (include + Raum-Scan + Opcode-Haken, je 1-2 Zeilen),
`elza_r36.c` (1 Bedingung `re15_game_flag_get(3, 0x9D)` vor dem Slot-2-Umbau); Alt-Riegel `test_r36_elza.c` /
`test_r36_elza.cmake` Teil irons umgestellt.

## 2. Riegel der Spur (selbst gefahren, Spurbaum, seriell)

```
ctest --test-dir re15_port/build -R "^(unit_r38_kleinkram_.*|unit_r36_elza_.*)$" --timeout 600 --output-on-failure
100% tests passed, 0 tests failed out of 11
ctest --test-dir re15_port/build -R "^(integration_r38_kleinkram_.*|integration_r36_elza_irons)$" --timeout 1200 -V
583 r36_elza_irons: OK — Irons 151/151 Bilder tot (Clip 2 Bild 89), Kopf fest 60/60, Anklicken -> Text 20      11.78 s
771 r38_kleinkram_inventar: OK — Satz 6 in F400 (Mitschnitt-Bild 654), Satz 5 in F500 (Bild 754), 14 abw. Bilder 70.88 s
772 r38_kleinkram_irons: OK — Text 20 ab F141 mit Elza gebeugt (mo=11), msg 2 nie, Irons 561/561 tot              26.43 s
773 r38_kleinkram_flammenwerfer: OK — ja: Modal 0x0E x100, genommen 0 -> 2 Zellen, msg 15 nie; nein: ...           48.52 s
100% tests passed, 0 tests failed out of 4
```
Die Riegel messen je Punkt die Mechanik (Unit: echter Viereck-Druck ueber den Scan-Stempel, nicht RE15_FIRE_AOT) und
das Ergebnis an der echten exe (Integration). Inhaltlich gelesen: test_r38_kleinkram.c / test_r38_kleinkram.cmake.

## 3. P1 — Inventar oeffnen mit Ton (Wortlaut: "Das öffnen des Player Inventories hat keinen Sounds, wenn Resident Evil 2
dort Sound hat, möchte ich auch Sound haben.")

### 3.1 RE-Beleg (selbst disassembliert, info/re2leon/PSX.EXE, re2_disasm.py)
* Gate vor der Annahme: `800263fc lbu v0,-3256(v0)` (0x800df348) / `80026404 bne v0,zero,0x80026540` — Wege, deren
  Handler den Schirm selbst anfordern, springen ueber den ganzen Block (Box/Aufnahme/Kartenhinweis ohne Ton) — bestaetigt.
* Annahme: `800264dc andi v0,v1,0x8000` / `800264e0 beq v0,zero,0x80026540` / `800264e8 sw v0,0(a0)` (|0x40),
  `80026518 lbu v1,0x800d5c00`, `80026528 beq v1,v0(=2),0x80026738` (Verzoegerungsplatz `8002652c lui a0,0x406`),
  **`80026530 jal 0x8005ba28` / `80026534 addu a1,zero,zero`** -> Se_on(0x04060000, 0) — bestaetigt.
* Sprungziel 0x8005ba28 selbst disassembliert: `srl t1,a0,24` (Bank) @0x8005ba30, `srl v0,a0,16; andi s7,v0,0xff`
  (Satz) @0x8005ba7c-80, Satz-Tabelle `lw a1,0x800dbb78[bank*4]` @0x8005ba8c = Se_on mit Bank 4, Satz 6.
* Bank 4 = CORE in RE2 (die Skepsis "fuer RE2 unbelegt" aus analysis/re2_ermittlung_2026-08-30/STRAENGE_ROH.md hier
  geschlossen): Lader @0x80059a90 `sw s0,0x800dbb88` (= 0x800dbb78 + 4*4, s0 = 0x801f9d10), Etikett a3 = 0x80011548
  ("CORE EDH", Datei-Text @0x80011556), VB-Zeiger @0x80059ac8 `sw v0,0x800d75b0` (= 0x800d75a0 + 4*4, der Zeiger, den Se_on
  @0x8005bac8 liest).
* Samples: sha256 RE1.5 `SOUND/CORE00.EDH/.VB` == RE2 `CORE00.EDH/.VB` (7948e41e.. / 78514738..) und RE1.5 `CORE04.EDH/.VB`
  == RE2 `CORE01.EDH/.VB` (57e3f9cf.. / 0ae86dd9..) — der Port spielt fuer Leon und Elza damit bytegleich die Saetze 5/6
  der RE2-Leon- bzw. RE2-Claire-Bank.
* Schliessen (Zustand 2 der Kommandoleiste 0x8006a7f0): EXIT Cursor 3 `8006a8cc addiu v0,zero,3` / `8006a8d0 beq
  v1,v0,0x8006a910` mit `8006a8d4 lui a0,0x405` -> `8006a910 sb zero,1(s0)` / `8006a918 jal 0x8005ba28`; Abbruch
  `8006a928 andi v0,v0,0x2000` / `8006a930 lui a0,0x405` / `8006a934 jal`; Statustaste `8006a94c andi v0,v0,0x4000` /
  `8006a968 lui a0,0x405` / `8006a96c jal` / `8006a978 sb zero,0x800d5bf1` — Satz 5 auf allen drei Wegen, bestaetigt.
  (Cursor 0..2 = Reiter: `8006a8c8 lui a0,0x406` -> Satz 6 — der Port behaelt dort se4(6).)
* Konstanten im Diff: `RE15_R38Q_INV_TON_AUF 6` (@0x8002652c/@0x80026530), `RE15_R38Q_INV_TON_ZU 5`
  (@0x8006a930/@0x8006a968/@0x8006a8d4) — Adressen tragen das Behauptete.

### 3.2 Messung (eigene Messkopie `build/platform/pc/re15_pc_abn0q.exe`, Arbeitsordner im Scratchpad)
Env: `RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_AUDIO_CAP_SYNC=cap.raw RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=1000@250 RE15_PLAYER_POS=21850,-13400,2048 RE15_SE_DEBUG=1 RE15_STATE_LOG=state.log
RE15_EXIT_AT=600#1000`, Lauf "mit" zusaetzlich `RE15_PRESS=start@400,start@500`.
* NEU mit: debug.log Z.124 `[se] Stimme: se=6 layer=0 vag=3 note=64 ... vol=90` / Z.125 `[r38q] F400 Inventar-Ton
  Satz 6 (Oeffnen)` / Z.126 `[inv] Sicherung im Statusschirm ...` / Z.130 `[se] Stimme: se=5 ... vag=2 note=63 vol=80` /
  Z.131 `[r38q] F500 Inventar-Ton Satz 5 (Abbruch)`; exit 0.
* NEU mit gegen NEU ohne (je 854 Mitschnitt-Bilder a 5880 B): 14 abweichende Bilder, genau zwei Laeufe
  [654..660] (Spitzen-Differenz 5488/5464/4563/2694/5496/3907/1772) und [754..760] (1232..4863) = Bild 400 und 500.
* GEGENPROBE v0.8.25 (Paket `release/pkg-win/re15_port_v0.8.25`, Kopie im Scratchpad) mit START@400/500 gegen NEU ohne:
  **0 abweichende Bilder** (bytegleich) — vorher stumm, Nutzerbefund reproduziert; die Ursache (einziger Oeffnen-Ton im
  Debug-Umschalter `re15_menu_toggle`, d4b41678) ist im Code nachvollzogen: alle `re15_menu_toggle`-Aufrufer in main.c
  (Z.5192/5441/5464/5677) sind Env-Messhaken; der echte Weg ist game_step_common.c:1260 -> `re15_menu_start_poll`.
* Weg-Abdeckung: Tastatur, Pad und Android-Overlay-START enden alle in derselben START-Flanke -> derselbe Haken.
  Box/Dokument/Kartenhinweis (`re15_menu_request_box/_doc/_map_hint`) setzen s_stage selbst -> kein Oeffnen-Ton, wie
  RE2 @0x80026404. Alle Schliesswege der Reiterwahl (START, Kreuz, EXIT) -> Satz 5; Raster-START hatte schon se4(5).
* Elza (eigener Lauf, Spielstand ROOM1131, START@100/200): `[se] Stimme: se=6 ... vag=8 note=66` + `[r38q] F100
  Inventar-Ton Satz 6 (Oeffnen)`, `[se] Stimme: se=5 ... vag=6 note=65` + `F200 ... Satz 5 (Abbruch)` — auch bei Elza
  hoerbar (ihre CORE-Bank CORE04 = RE2 CORE01, dieselben Saetze, die ihr Inventar fuer Auswahl/Abbruch spielt).

**Urteil P1: erfuellt.**

## 4. P2 — Elza ROOM1151 Irons (Wortlaut: "... erst - he already passed - und dann beugt sie sich hin und es kommt
"I'll be fine, just worry About yourself for now" ... sie soll sich nur hinbeugen, und es soll stehen "he already passed"...")

### 4.1 RE-Beleg (selbst geprueft)
* RDT-Bytes ROOM1151 (shared_assets, Python-Dump): @0x00D92 `2c 02 05 31 00 00 1c a8 b4 97 a4 06 78 05 00 00 ..` (Slot 2
  sce 5 Flags 0x31); sub01 @0x00E9E `23 00 00 00 02 00` Cmp(var00==2), @0x00EA4 `21 03 9d 00`, @0x00EA8 `22 03 9d 01`,
  @0x00EAC `04 ff 18 03`; sub03 @0x00EEC `3f 01 0b 00` (Hinbeugen), @0x00EF4 `29 07`, **@0x00EFA `2b 02 00 00 2e 02 00 00`**,
  @0x00F02/@0x00F0A `3f 00 06 00 09 0a 32 00`, @0x00F12 `3f 00 03 00 43 00 04 00`, @0x00F16 `43 00 04 00 09 0a 14 00`,
  @0x00F72 `01 00 22 02 00 01 22 02` — alle 6 Anker-Signaturen aus kleinkram_r38.c byte-gleich.
* RE1.5 PSX.EXE (re15_disasm.py): Scan FUN_80042bac `80042ef8 andi v0,v1,0x20` (Vorwaerts-Test), `80042f34 addiu v0,s2,-1`
  / **`80042f3c sh v0,0x800b0fd0`** (Stempel work_vars[0]) VOR `80042f48 lbu v0,0(s0)` (sce) — der Stempel ist sce-blind,
  bestaetigt. Bild-Treiber `8003f040 lw v0,g_pauseflags` / `8003f044 lui v1,0x200` / `8003f04c bne v0,zero,0x8003f090`
  ueberspringt den VM-Lauf samt `jal 0x8003f0a0` (@0x8003f088); der Wisch `8003ebf4..8003ec14 sh -1 -> 0x800b0fd0..fd6`
  wird aus FUN_8003f0a0 @0x8003f18c gerufen (jal-Zensus: 0x8003e5c0 / 0x8003ef7c / 0x8003f18c) — bestaetigt.

### 4.2 Messung UEBER DEN ECHTEN WEG (Tuer 1131 -> 1151, gehen, Viereck)
Spielstand Elza ROOM1131 (-5900,16350) Gier 2048 (= Ankunftsort der 1151-Tuer, Blick auf die Tuer; probe_r36_elza_karte),
CONTINUE, `RE15_PRESS=square@60` -> `[aot] DOOR FIRE slot=2 ... spawn=(-17150,0,-11960)`, `[room] PC loaded room1151.rdt`,
`[r38q] F60 ROOM1151: 6 Anker armiert`; AOT-Dump nach der Tuer: `2 EXAMINE Mitte(-21650,-26000) halb(850,700) ev=2` (Original-
Platz sce 5). Gehen per `RE15_INPUT_SCRIPT=U2.3333,R0.1667,U0.6,L0.3667,U2.8333,L0.0667,U1.5667,R0.0667,W0.5,A0.1,W5,A0.1,
W8,A0.1,W3` (BASIS spiel, START 100): Westgang zwischen Konsole (SCA x>=-21782) und Regal, Halt (-21485,-24908) Gier 1024,
Viereck F355. (Der Autopilot `aot:2` blieb am Eingang haengen — reiner Reglerfehler, mit der Hand-Eingabe kein Hindernis,
RE15_COLL_TRACE ohne Treffer an der Haltestelle.)
* NEU (state.log, Wechsel je Bild): F356 pm=2 (Szene) -> **F366 Elza mo=11** (Plc_motion 0x0B Hinbeugen) -> F406 cam 7 ->
  **F436 `msg(a=1 fsm=1 id=20)`** bei mo=11 (= Szenenstart + 80) -> F509 weggedrueckt -> F636 cam 5 -> F696 mo=200 / pm=1 ->
  F710 pm=0 -> zweites Anklicken F751: nur `id=20`, pm bleibt 0 (keine Szene). **msg 2 in keinem Bild**; Irons in jedem
  Bild `st=4 ss2=2 mo=2 af=89` (Totenpose). debug.log: `F436 ROOM1151 sub03 @0x00EFA Message_on 2 -> Text 20`, Gesten
  @0x00F02/@0x00F0A/@0x00F12/@0x00F16 entfallen, `F696 sub03 Ende: Slot 2 -> Text-Platz 20, (3,0x9D)=1`.
* Bild (Framedump 340..800/10, `p2_tuer6/sheet.png` im Pruefer-Scratchpad): Szene Kamera 7, Elza ueber den toten Irons
  gebeugt, Untertitel "He has already passed..." (Schreibmaschine), danach Aufrichten; zweiter Klick nur Untertitel.
* GEGENPROBE v0.8.25, identische Eingabe: F355 sofort `id=20` OHNE Szene (pm 0); nach dem Wegdruecken F510 Szene, F520 mo=11,
  F560 cam 7, **F590 `msg(a=1 fsm=1 id=2)`** und Irons `mo=6 af=1..33` / `mo=3` (der Tote gestikuliert) — exakt der
  Nutzerbefund. Verursacher 3eaf631d (Runde 36, Irons-tot-Umbau 1151): `git merge-base --is-ancestor` -> nicht in v0.8.23,
  in v0.8.24 und v0.8.25. Der Fix (Slot 2 bleibt bis zur Szene sce 5; @0x00EFA -> Text 20; Gesten aus) erklaert den
  Befund: der Stempel aus 4.1 traf vorher den Text-Platz und loeste nach dem Text sub03 aus.
* Leon ROOM1150: keine Anker (Raum-Id-Filter + Signatur), unit_r38_kleinkram_irons `Leon ROOM1150: Original-Gespraech mit
  msg 2 laeuft weiter` — gruen.

**Urteil P2: erfuellt.**

## 5. P3 — ROOM30E0 Flammenwerfer (Wortlaut: "... soll beim aufnehmen des Flammenwerfers nicht mehr kommen - not available
in the preview, sondern einfach der Flammenwerfer aufgenommen werden.")

### 5.1 RE-Beleg (selbst geprueft)
* ROOM30E0 (scd_dump_room.py + Bytes): main00 @0x00A52 `06 00 64 00`, @0x00A56 `21 03 9c 00`, @0x00A5A
  `2c 03 03 31 00 00 e0 98 ec 96 e8 03 e8 03 ff 00 18 09 00 00` (Slot 3 sce 3 Flags 0x31, Nutzlast `ff 00 18 09` ->
  RE15_R38Q_FW_PANEL_P0 0x00FF / P1 0x0918 korrekt); sub04 @0x00FE0 `46 03 ..` Aot_reset(3), **@0x00FEE `2b 0f ff ff 02 00 29 02`**,
  **@0x00FFA `22 03 9c 01 22 02 07 00`** — Anker-Signaturen byte-gleich; sub09 @0x01038 `29 02` + Set(5,0..4,6), sub10
  @0x01054 `29 03` Set(5,4,0)/(5,5,1).
* Waffen-Record: `re15_disasm.py bytes 0x80074e50` -> `64 00 00 00 a8 4c 07 80 02 01 00 00`; Tabellenbasis/Schritt selbst
  disassembliert: `80049788 sll v0,a0,1 / 8004978c addu / 80049790 sll v0,v0,2 / 80049798 addiu at,at,0x4da8` (= Id*12 +
  0x80074da8) -> Id 0x0E -> 0x80074e50, Magazin 100. Nachbarn passend zum Zensus der Spur: 0x07 -> 6 (@0x80074dfc),
  0x08 -> 7 (@0x80074e08), 0x0c -> 100 (@0x80074e38), 0x0d -> 12 (@0x80074e44). Port-Name Id 0x0e = "FLAMETHROWER"
  (inventory_common.c:416), Port-Waffentabelle `{100,0x18}` fuer 14 (inventory_common.c:209).
* Item-Handler @0x80043328: `8004332c lbu v0,0x80072d3b` / `80043334 bne v0,zero,..` (nur aus Zustand 0), `8004334c sb 1`,
  `8004335c sb v0,0x800afbb6`, `80043364 sw a0,0x800aca30` — bestaetigt (Haken parkt den Faden waehrend des Modals).

### 5.2 Messung (Leon-Spielstand ROOM30E0 (-25000,-26400) Gier 2048 + CONTINUE, echter Panel-Ablauf)
Eingabe `RE15_INPUT_SCRIPT=A0.1,W0.5,L0.8,W0.5,A0.1,W1,R0.13,W0.5,A0.1,W3,A0.1,W3` (BASIS spiel, START 60), Framedump 60..460/10.
* NEU "Ja": debug.log `[r38q] F170 ROOM30E0 sub04 @0x00FEE Message_on 15 -> Aufnahme-Modal Item 0x0E x100`, `F263 ...
  Flammenwerfer genommen: Zellen 0 -> 2`, danach `Cut_chg(2)`; state.log: 0 Bilder mit `id=15`. Bilder: Panel (Cut 2),
  Spind offen mit Fadenkreuz (Cut 3), Modal mit ITPS-Bild + "Will you take the Flamethrower? > Yes No", danach Cut 2
  (geschlossene Spindtuer, kein Rest-Flammenwerfer im Bild) und Cut 1.
* NEU "Nein" und zweiter Anlauf (Eingabe + `W4,R0.1,W0.3,A0.1,W3,A0.1,W0.5,L0.13,W0.5,A0.1,W1,R0.13,W0.5,A0.1,W4,A0.1,W3,
  S0.1,W2,A0.1,W2`): `F322 Flammenwerfer NICHT genommen (No/voll): Panel Slot 3 wieder scharf, Zellen 0`, `F353 sub04
  @0x00FFA Set(3,0x9C) entfaellt`, Panel erneut (`first_op=0x29`, Cut 2/3), `F487 ... Aufnahme-Modal`, **`F610 ... genommen:
  Zellen 0 -> 2`**, `F702 Inventar-Ton Satz 6`; Framedump F800: Statusschirm, Item list = 2-Zellen-Flammenwerfer "100",
  Name "Flamethrower" (`p3_nein/inv_800.png`).
* GEGENPROBE v0.8.25, gleiche Eingabe: 291 Bilder `msg(a=1 fsm=1 id=15)`, Framedump F200 "There's a Flamethrow..." —
  Nutzerbefund reproduziert.
* ROOM30E1 (Elza): Anker nur fuer Raum 0x30E0 (k_anker), Zensus der Spur: kein Flammenwerfer/Panel in 30E1 -> unberuehrt.

**Urteil P3: erfuellt.**

## 6. RE-Gate (Diff gegen Belege)

Selbst disassembliert / byte-geprueft (Auswahl, mehr als 3): RE2 @0x800263fc/@0x80026404, @0x8002652c/@0x80026530/
@0x80026534, Sprungziel 0x8005ba28, Lader @0x80059a90/@0x80059ac8, @0x8006a8c8..@0x8006a978; RE1.5 @0x80042ef8/@0x80042f3c/
@0x80042f48, @0x8003f040-4c, @0x8003ebf4-ec14 + Aufrufer @0x8003f18c, Record @0x80074e50 + Indexierung @0x80049788-98,
Item-Handler @0x80043328-64, Modal @0x8001df40/@0x8001df44 (Typ 14..19 = 2-Zellen-Waffe), @0x8001e054 `bltz` / @0x8001e068
`andi 0x1` (voll / "No" -> Zustand 8, kein Einfuegen); RDT-Bytes aller 8 Anker. **Jede zitierte Stelle traegt das Behauptete.**

| Konstante im Diff | Beleg | geprueft |
|---|---|---|
| RE15_R38Q_INV_TON_AUF 6 | RE2 `8002652c lui a0,0x406` + `80026530 jal 0x8005ba28` | ja |
| RE15_R38Q_INV_TON_ZU 5 | RE2 `8006a930`/`8006a968`/`8006a8d4 lui a0,0x405` | ja |
| Anker 1151 0x00EFA/F02/F0A/F12/F16/F72 + 8-Byte-Signaturen | ROOM1151.RDT Datei-Offsets | ja, byte-gleich |
| Anker 30E0 0x00FEE/0x00FFA + Signaturen | ROOM30E0.RDT Datei-Offsets | ja, byte-gleich |
| RE15_R38Q_FW_ITEM 0x0E | Record @0x80074da8 + 14*12 = @0x80074e50, Name "FLAMETHROWER" | ja |
| RE15_R38Q_FW_MENGE 100 | `64 00 00 00` @0x80074e50; Regel Menge == Magazin: eigener Zensus 240 RDTs = 13 Waffen-Item_aot_set, 7 Typen, 13/13 EQ | ja |
| PANEL_SLOT 3 / SCE 3 / FLAGS 0x31 / P0 0x00FF / P1 0x0918 | main00 @0x00A5A `2c 03 03 31 .. ff 00 18 09 00 00` | ja |
| (3,0x9D) / (3,0x9C) | Zensus 240 RDTs: nur 1150/1151 @0x0EC6/0x0ECA bzw. 0x0EA4/0x0EA8; nur 30E0 @0x0A56/0x0FFA | ja |
| Maske `pc[2..3] << 16` (irons_text) | wie op_message_on (scd_vm.c, @0x80040508/@0x8004051c) | ja |

Grep im Code-Diff nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/getenv: einziger Treffer ist das
Nutzerzitat "... for now" im Kopfkommentar — kein Env-Schalter, kein Platzhalter. `re15_item_modal_start(0x0E, 100, 0, -1,
0xFF)`: taken_bit 0 = keines (item_modal_common.c `s_taken`), kein Zonen-Slot, kein Weltmodell; die Nutzer-Halbierung
`re15_pickup_menge_nutzer` laesst Waffen unveraendert (inventory_common.c:278) -> 100 im Inventar gemessen.
**Fix erklaert den Befund** (P2 Gegenprobe v0.8.25 + Stempel @0x80042f3c; P1 einziger Oeffnen-Ton im Debug-Umschalter;
P3 msg 15 @0x00FEE).

## 7. Vertrag

* Pfade (`git diff master...HEAD --name-only`): analysis/befunde_runde38/Q_* (Dossier + 4 Belegbilder), engine/src/
  {kleinkram_r38.c (neu), menu_common.c, scd_vm.c, elza_r36.c}, include/re15_kleinkram_r38.h (neu), tests/unit/
  {test_r38_kleinkram.c (neu), probes/r38_kleinkram.cmake (neu), test_r36_elza.c}, tests/integration/{test_r38_kleinkram.cmake
  (neu), test_r36_elza.cmake}. Kein release/, kein platform/android/, kein shared_assets/PSX/, keine Test-CMakeLists
  (Registrierung ueber probes/<thema>.cmake). `git diff master --stat` zeigt zusaetzlich r38_w2.js/UEBERGABE.md als
  "geloescht" — das ist nur master, der nach der Basis 1bcbd0ff weiterlief (98d35560); der Zweig selbst beruehrt sie nicht.
* Bank-9-Bits: keine. Nachrichten-Ids: keine neuen (1151 Text 20 = Runde 36; 30E0 ohne Text) — VERTRAG §1.2 Spur Q erfuellt.
* Gemeinsame Dateien: scd_vm.c 3 Zeilen (include, Raum-Scan, Opcode-Haken; §1.4 gelistet); menu_common.c include + 3
  Rufstellen (je 1-2 Zeilen, "Runde 38 Spur Q") — die Inventar-FSM liegt in menu_common.c, nicht in den fuer Q gelisteten
  re15_inv_screen.c/inventory_common.c; elza_r36.c 1 Bedingung (§1.4: "H; F/G nur Haken" — Q ist dort nicht gelistet).
  Beides kleine, kommentierte Haken und im Dossier als Integrations-Hinweis ausgewiesen -> **kein Mangel, Hinweis an die
  Integration** (Konflikt-Kandidat elza_r36.c mit Spur H).
* Riegel je Punkt vorhanden und messend (6 neu, ctest -N = 794). RE15_MIN_TESTS (local_build.sh, 788) hebt laut Dossier der
  Orchestrator um 6 an.
* Alt-Riegel test_r36_elza (Teil irons) umgestellt: die alte Behauptung "kein sce-5-Platz mehr -> work_vars[0] wird nie 2"
  war der Fehler selbst (Gegenprobe 4.2); neu: (3,0x9D)=0 -> sce 5, (3,0x9D)=1 -> Text-Platz + Anklicken = Text 20 ohne msg 2.
  Der erste Klick wird jetzt ueber den echten Stempel-Weg in unit/integration_r38_kleinkram_irons geprueft — keine
  Abschwaechung der Abdeckung.

## 8. Weitere Gegenpruefungen
* RE1.5 selbst ist beim Oeffnen stumm: jal-Zensus `jal 0x80045024` ueber info/Re1.5/PSX.EXE = 41 Stellen, keine in
  0x8001ca00..0x8001cf00 (Oeffnen-Poll/Blende) und keine in 0x80049700..0x80049b00 (Reiterwahl) -> RE2 ist das Ziel
  (Beta -> Retail), wie der Nutzer es verlangt ("wenn Resident Evil 2 dort Sound hat").
* ROOM30E1 (Elza) gegengelesen: msg 15 ist dort "Elza: We have no choice but to go / inside..." — der Anker ist an die
  Raum-Id 0x30E0 gebunden und trifft 30E1 nicht (sonst waere Elzas Dialog zerstoert).
* Sprachdatei: synchro/STAGE1/room1151 existiert nicht -> Text 20 bleibt stumm wie vorher; die 1150-Stimme main02.wav
  ("I'll be fine") ist an Raum 1150 gebunden (audio_pc.c sucht `room%04X` exakt) und kann in 1151 nicht spielen.
