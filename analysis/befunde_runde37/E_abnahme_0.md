# Runde 37 — Spur E "codepanel" — Abnahme 0 (unabhaengig, 2026-10-12)

Baum `.claude/worktrees/r37_codepanel` (Zweig `r37/codepanel`, Kopf a9bb7855, Basis f1e019ca). Gemessen am selbst
gebauten Stand: `local_build.sh configure` + `build` ("ninja: no work to do", exe 11:52:02 juenger als alle Quellen,
Arbeitsbaum sauber). Massgeblich: `AUFTRAG.md` #9 + Nachtrag 3 + Nachtrag 3b (Wortlaut unten).

STATUS: abgeschlossen — BESTANDEN (E1 erfuellt, E2 erfuellt, Gates halten; Hinweise H1..H7 nicht blockierend)

## Wortlaut (AUFTRAG.md)

> #9: bei den Code panels möchte ich das, so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet.
> Ist er dann richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock
> sound kommt.
>
> Nachtrag 3: Nein, beim Panel in korridor 2 ist falsch - dort soll natürlich rot leuchten - richtig eingegeben - dann
> blau.... / rot = geschlossen
>
> Nachtrag 3b: Es muss schon in der richtigen Farbe leuchten das Panel... hintergrund rot - rot leuchten - hintergrund
> blau - blau leuchten...

Punkte der Abnahme: **E1** Lampe (zu = rot gemalte Lampe leuchtet rot; richtig = Korridor 2 blau, SECUGUARD gruen; in der
gemalten Farbe), **E2** Kamera bleibt auf dem Panel, waehrend Piep und Aufschliess-Ton kommen.

## Messwerkzeug (nicht versioniert, Scratchpad)

* exe-Kopie unter eigenem Namen `re15_port/build/platform/pc/re15_pc_r37e_abn0.exe` (nie `taskkill /IM`), je Lauf eigener
  Arbeitsordner; Laeufer `lauf.sh` (env + timeout).
* ECHTER WEG statt Sprung in den Panel-Raum: Sprung (Debug-Menue) in den NACHBARRAUM, Tuer per Aktionstaste, im Panel-Raum
  per `RE15_WEGPLAN` (GEHE/BLICK/AKTION, geschlossen geregelt) zum Kartenleser, Aktionstaste; danach `RE15_PAD_AT`
  (Ja, Nachrichten, Cursor per D-Pad, Viereck). Karte wie aufgehoben: `RE15_GIVE` + Taken-Bit (sub20 prueft (9,52) bzw.
  (9,136), ROOM10D0 @0x019C0 / ROOM1230 @0x01488, selbst gelesen).
* Ton: `RE15_AUDIO_CAP_SYNC` + `tools/tueren/r36_cap_ticks.py`; Lage des Tons zusaetzlich per DIFFERENZ gegen einen
  identischen Lauf mit falscher letzter Ziffer (erster abweichender Tick = Piep).

## E2 — Kamera bleibt auf dem Panel bei Piep und Aufschliess-Ton

**Urteil: erfuellt.**

### Lauf r2b_echt — ROOM10D0 (Leon), ECHTER WEG
```
RE15_DEBUG_JUMP=10C0@120 RE15_PLAYER_POS=-7400,-2250,2048 RE15_GIVE=0x38:1 RE15_SET_FLAG=9:52
RE15_WEGPLAN: RAUM 10C0 / BLICK 2048 / AKTION (Tuer slot 2) / RAUM 10D0 / GEHE 6800 6000 / GEHE 1800 6000 /
              GEHE 1800 -5750 / GEHE 1150 -5750 / BLICK 2048 / AKTION (Kartenleser)
RE15_PAD_AT  = Plan des integration_r37_codepanel (Lauf B) um +360 Bilder verschoben (sub20 startet hier F511,
               dort F151, beides per RE15_SCD_TRACE "@0x19AE op=0x2B"): Ja, Nachrichten, 4-3-1-2 per D-Pad + Viereck
```
```
debug.log  [aot] DOOR FIRE slot=2 rect=(-7600,-2250,...) spawn=(7650,0,11400)   (Tuersequenz 307 Bilder)
           [room] PC loaded room10d0.rdt ; [msgtext] id=7 / 9 / 0 / 1 / 2 / 3 / 5 "You've opened the lock."
auf.log    F723  KARTE_EIN | F1180 KARTE_AUF satz=1(PIEP) bild=+0 | F1195 satz=2(AUF_E) bild=+15
cp.log     F1180 kamera halten: sub19 @0x0199A ... | F1371 kamera zurueck: Toene 2/2, gewartet 0 Bilder
cut.log    F783..F1371 bg=10d0#11 (Panel), F1371 req=0, F1372 bg=10d0#0 (Raum)
discard    F1268..F1370 Wegwerf-Abfrage (Blue Keycard 0x38) auf dem Panel, Antwort Ja F1370
```
Ton (Mitschnitt `cap.raw`, 1470 Frames je Tick): DIFFERENZ gegen Lauf **r2c_falsch4311** (identischer Plan, letzte
Ziffer "1" statt "2" -> "Wrong code, try again.", kein Ton): erster abweichender Tick **t1511** (= Piep-Bild F1180, Versatz
Tick = F + 331); Piep t1511..t1525 (1450-Hz-Anteil 0,47..0,70, Spitze 1470 Hz), t1526 Luecke, **Aufschliess-Ton
t1527..t1562** (<250 Hz dominant, rms(a-b) bis 2184) = Bilder **F1196..F1231**. Beide Toene liegen vollstaendig im
Panel-Abschnitt F783..F1371 (Kamera erst F1372 im Raum). Bilder: `fd_001179` (rot), `fd_001181` (blau, Piep),
`fd_001195` (Aufschliessen, Panel, "You've o"), `fd_001300` (Wegwerf-Abfrage auf dem Panel), `fd_001372` (Raum).

### Lauf r3c_1230 — ROOM1230 SECUGUARD (Leon), ECHTER WEG
Sprung 1160 ("KENNEL", (4,243)=1 = Tuer fuehrt nach 1230 statt 1180, ROOM1160 main00 @0x007C2/@0x007EA selbst gelesen),
Tuer slot 0, in 1230 GEGNERWEG (zwei Hunde 0x20, einer stand am Leser) und GEHE zum Leser, Code 5632 per D-Pad
(Achtung: in 1230 steht der Cursor beim Oeffnen AUF der Taste "8" — mein erster Plan gab damit ungewollt "8" ein und
landete richtig in "Wrong code"; korrigiert: nur ein Viereck fuer msg 0).
```
evt        F1014 sub17 | F1109 sub10 (5) | F1222 sub11 (6) | F1323 sub8 (3) | F1411 sub7 (2) | F1412 sub19
auf.log    F1412 PIEP +0 | F1427 AUF_E +15
cp.log     F1412 kamera halten: sub19 @0x01462 | F1413 lampe OFFEN: Zeile 2 | F1603 kamera zurueck: Toene 2/2
cut.log    F1015..F1603 bg=1230#9 (Panel), F1604 bg=1230#6 (Raum)
cap.raw    Piep t1740..t1754 (= F1412..F1426, 1450 Hz 0,72..0,89), Aufschliess-Ton t1756..t1788 (= F1428..F1460,
           <250 Hz bis 0,98, rms bis 2248 gegen Grundpegel ~300..450); Kamera-Rueckkehr F1604 = t1932 (Versatz +328)
```

### Lauf r4b_elza — ROOM10D1 (Elza, RE15_PSELECT_AUTO_SWITCH), ECHTER WEG 10C1 -> 10D1
`cp.log F1180 kamera halten: sub19 @0x01984 | F1181 lampe OFFEN Zeile 1 | F1371 kamera zurueck`, `auf.log F1180 PIEP /
F1195 AUF_E`, `cut.log F783 bg=10d1#11 ... F1372 bg=10d1#0`. Gleiches Bild wie Leon (Bilder fd_001170/1190/1360/1380).

### Grenzfall r2d_halten — Viereck ab dem Piep-Bild GEHALTEN (schnellstmoeglich weiterdruecken)
`msg 5 zu F1194, Wegwerf-Abfrage F1194..F1218 (Ja), kamera zurueck F1219, Raum ab F1220`. Der Aufschliess-Ton (F1196..F1231)
beginnt damit auf dem Panel und laeuft seine letzten ~12 Bilder in der Raumansicht aus. Das entspricht RE2 ROOM2110
(`@0x01B90 Se_on(2,0x0B)` und sofort `@0x01B9C Cut_chg 9`, selbst gelesen) und haelt den Wortlaut ("wenn der Piep und
unlock sound kommt" — beide kommen auf dem Panel). Kein Mangel, Hinweis H2.

### Mechanismus / Befund erklaert
Selbst gelesen: alle sechs RDTs tragen den sub19-Schwanz `2a 00 3c 01 2b 05 ff ff 02 00 22 01 1b 00 22 02 07 00`
(10D0 @0x0199A, 10D1 @0x01984, 11E0/11E1 @0x01F82, 1230/1231 @0x01462) — Cut_old/Cut_auto VOR Message_on 5, an dem seit
Runde 36 der Piep haengt. Das erklaert den Befund (Kamera im Piep-Bild weg). Der Haken (scd_vm.c, 1 Zeile + Kommentar)
ueberspringt X..X+3 und fuehrt an X+10 die ORIGINAL-Handler `op_cut_old`/`op_cut_auto` auf den Original-Bytes aus
(RE1.5-Tabelle selbst disassembliert: `[42] @0x80074550 -> 0x8004032c`, `[60] @0x80074598 -> 0x800403ac`; 0x8004032c
`lbu a0,0x3f7b` @0x8004033c, `sh v1 -> 0x800b0fe8` @0x8004035c, `sh a0 -> 0x800b0fe4` @0x80040364, `&= ~0x100` (-257)
@0x80040378..84; 0x800403ac `lbu v1,1(v0)` + Bit 0x100 je nach pc[1]). Vorbild RE1.5 ROOM1100 sub02 selbst gelesen:
`@0x00CBA 2b 02 ff ff` (msg 2 = "You've opened the lock.", rdt_msgdump) / `02 00` / `22 02 07 00` / `@0x00CC4 29 06` /
`3c 01` — Kamera nach der Nachricht. Unit-Gegenprobe (Haken aus) = Befund vorher (Kamera im Piep-Bild weg).

## E1 — Lampe: rot solange zu, richtig -> gemalte blaue (Korridor 2) bzw. gruene (SECUGUARD) Lampe, in ihrer Farbe

**Urteil: erfuellt.**

"Korridor 2" selbst nachgeschlagen: `include/debug_jump_table.h` Stage 0 Index 13 = "2F CORRIDOR" (= ROOM10D0), Index 30
"B2 CORRIDOR" (= 11E0, SECUGUARD rot/gelb/gruen, im Spiel nie zu oeffnen, s.u.), Index 35 "B1 CORR. LIGHT" (= 1230).
Nur 10D0 traegt eine BLAU gemalte Lampe -> "richtig eingegeben - dann blau" kann nur 10D0 meinen.

Gemaltes Glas selbst aus den Framedumps vermessen (Bild, in dem die jeweilige Lampe NICHT leuchtet; Farbmaske
Kanal minus Rest >= 20 bzw. 12): 10D0 rot x 116..127 / y 72..81 -> Mitte (121.5, 76.5), blau x 164..173 / y 72..81 ->
(168.5, 76.5); SECUGUARD rot x 122..135 / y 166..183 -> (128.5, 174.5), gruen x 174..187 / y 166..183 -> (180.5, 174.5)
= Port-Tabelle (243,153)/(337,153)/(257,349)/(361,349) halbe Pixel. Schwerpunkt der LEUCHTE (Bilddifferenz an/aus):
10D0 rot (121.3, 75.4), blau (167.6, 75.5), SECUGUARD rot (129.0, 175.0), gruen (180.9, 174.9) — sitzt auf dem Glas.

Mittel ueber das gemalte Glas (RGB, HSV-Farbton), ECHTER WEG:

| Lauf / Bild | Zustand | rot gemaltes Glas | andere Lampe |
|---|---|---|---|
| r2a F790 (Tastenfeld offen, vor der 1. Ziffer) | zu | (180,48,47) 0 Grad LEUCHTET | blau (62,77,95) 213 = dunkel wie gemalt |
| r2b F1179 (vor der 4. Ziffer) | zu | (241,79,73) 2 Grad LEUCHTET | blau (62,77,95) 213 dunkel |
| r2b F1181 (Bild nach dem Piep) | offen | (70,40,31) 14 Grad = gemalt, aus | **blau (122,172,229) 212 LEUCHTET** |
| r2b F1300 (Wegwerf-Abfrage) | offen | (70,40,31) aus | blau (86,125,208) 221 leuchtet |
| r2c F1200/F1260 (falscher Code 4311, "Wrong code, try again.") | zu | (180,48,47) 0 Grad LEUCHTET | blau dunkel; nie OFFEN in cp.log |
| r3b F1100 (1230, Tastenfeld offen) | zu | (212,29,38) 357 LEUCHTET | gruen (8,51,9) 121 dunkel |
| r3c F1411 (1230, vor der 4. Ziffer) | zu | (232,52,50) 1 LEUCHTET | gruen dunkel |
| r3c F1413 (1230, Bild nach dem Piep) | offen | (46,10,8) 3 = gemalt, aus | **gruen (64,225,65) 120 LEUCHTET** |
| r4b F1170 / F1190 (Elza, 10D1) | zu / offen | rot leuchtet / aus | blau dunkel / leuchtet |

Gemaltes Glas (Riegel zeichner, selbst gelaufen): 10D0 rot 14 Grad, blau 213, SECUGUARD rot 3, gruen 121 -> leuchtend
358..2 / 212..221 / 357..1 / 120: jede Lampe leuchtet in IHRER gemalten Farbe (Nachtrag 3b), rot = geschlossen
(Nachtrag 3), die gelbe SECUGUARD-Lampe bleibt wie gemalt. Lampe nur im Panel-Cut (11 / 9), sonst nichts gezeichnet
(cp.log, unit_r37_codepanel_lampe "keine Lampe ausserhalb des Panel-Cuts", anderer Raum ROOM10C0 Cut 11 -> keine Lampe).
Sichtpruefung der 4x-Ausschnitte (Scratchpad `e_zoom_lampen.png`): weicher Schein um die runde Lampe (48/64 px),
leuchtendes Quadrat auf der eckigen SECUGUARD-Lampe; der runde Schein streift die Oberkanten der Tasten 1/2 bzw. 2/3
(vom Bau-Agenten als OFFEN gemeldet, Hinweis H4).

Kunst / RE-Herkunft selbst geprueft: RE1.5 hat keine Leuchtkunst fuer die Panels (BSS-Chunks = Cuts: ROOM10D.BSS 983040 B
= 15 Chunks = nCut 15, ROOM11E.BSS 16 = 16, ROOM123.BSS 11 = 11) -> RE2 (Beta -> Retail). RE2 PSX.EXE selbst
disassembliert: Tabelle `[100] @0x800a7658 -> 0x80056644` (sce_espr_on2: `lhu s0,2(s1)` @0x8005666c, `sll a0,s0,24`
@0x8005667c, `srl s0,s0,8`/`sll s0,s0,16` @0x80056680/84, `jal 0x8001c8c4` @0x800566c0, PC+16 @0x800566cc), Spawner
`andi t5,v0,0xff` @0x8001c8d4, `srl v0,t5,3` @0x8001c9e0, `sll v0,v0,6` @0x8001c9e4, `addu v1,v1,v0` @0x8001c9f8,
`sh v1,50(t0)` @0x8001c9fc = CLUT-Zeile = Unterindex>>3; Prim `andi v0,s2,0x1000` @0x80077a44 -> 46 (0x2E, halbdurchsichtig)
@0x80077a50. RE2-Skriptbytes selbst gelesen: ROOM4070 @0x0088C `64 03 16 0c` (offen, blau rechts), @0x008C6 `64 02 16 04`
(zu, rot links); ROOM2130 @0x01774 `64 0b 16 00` rot, @0x01798 Se_on 0x0F, @0x017A4 `65 0b 65 0c`, @0x017A8 `64 0d 16 10`
gruen, @0x017C8 Sleep 30, @0x017CC `29 04`; ROOM2110 @0x01B5C Piep / @0x01B68 `65 01 65 02` / @0x01B6C `64 01 16 08` /
@0x01B8C Sleep 15 / @0x01B90 Se_on 0x0B / @0x01B9C `29 09` `3c 01`. ESP-0x16-Block ROOM2130 effect.esp selbst gelesen:
Saetze @0x10 `05 01 01 18`,`07..`,`06..`,`07..`,`07 01 ff 20` (rund) bzw. `03 01 01 20`,`04 01 01 20`,`04 01 ff 20`
(eckig); Zellen @0x70 `60 00 f0 f0`/`80 00`/`a0 00 f4 f4`/`b8 00`/`d8 00` = k_u0 {96,128,160,184,216}, k_s {32,32,24,32,32};
Strom-Tabelle @0x90, Strom 2 -> Satz 4 (@0x110), Strom 4 -> Satz 7 (@0x180); Block == ROOM2110 effect.esp @0xBFC (492 B).
Kanten 48/64/26 px sind im Code als PORT-WAHL (Herleitung RE2 4070/2130) gekennzeichnet.

## Gates

### RE-Gate — haelt
* Jede verhaltensrelevante Konstante im Diff traegt @0x/Datei-Offset (Cut 11/15/9 @0x018D6/@0x018C0/@0x01EBE/@0x0139E,
  Flag 50/139/137 @0x0152A/@0x01514/@0x01A8A/@0x00FF2, X @0x0199A/@0x01984/@0x01F82/@0x01462 + 18-Byte-Signatur, Zeilen
  0/1/2 @0x8001c9e0..fc, Zellfolgen/Zellen effect.esp @0x10/@0x70) oder ist als PORT-WAHL gekennzeichnet (Kanten 48/64/26 px)
  bzw. ist Nutzer-Vorgabe (Farbzuordnung, Nachtrag 3/3b). Lampenmitten sind Messwerte am Hintergrund (selbst nachgemessen,
  s. E1). Commit-Messages zitieren die Adressen.
* Selbst disassembliert / gelesen (Auswahl): RE1.5 0x8004032c, 0x800403ac, Tabelle @0x80074550/@0x80074554/@0x80074598;
  RE2 0x80056644, 0x8001c8c4..d8, 0x8001c9e0..fc, 0x80077a44..50, 0x8001dc48..60 (`or v1,v1,a0` / `sh v1,42(v0)`: TPAGE |=
  Halbwort +20 — dass dieses 0x20 traegt, ist aus Runde 34 uebernommen, nicht von mir nachverfolgt); RDT-Bytes RE1.5 10D0/
  10D1/11E0/11E1/1230/1231/1100/1240, RE2 2110/2130/4070, effect.esp 2130/2110. Alles wie behauptet.
* Keine Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO) im Code-Diff; einziges getenv im
  Produktcode ist der Log-Haken `RE15_CODEPANEL_LOG` (kein Verhaltensschalter).
* Der Fix ERKLAERT den Befund (Cut_old vor Message_on 5 in sub19, s. E2); Gegenprobe ohne Haken = Befund vorher
  (unit_r37_codepanel_kamera, selbst gelaufen: "GEGENPROBE ohne Haken: Piep Bild 312 -> angezeigter Cut 0").

### Vertrag — haelt
* Pfade (`git diff master...HEAD --name-only`): nur analysis/befunde_runde37/{AUFTRAG.md, E_codepanel.md, E_belege/*},
  engine/src/codepanel_r37.c (neu), include/re15_codepanel_r37.h (neu), platform/pc/src/codepanel_r37_pc.c (neu),
  engine/src/scd_vm.c (+5 Zeilen: include + 2 Haken mit "Runde 37 Spur E"), platform/pc/main.c (+2 Zeilen),
  tests/unit/probes/r37_codepanel.cmake + tests/unit/probe_r37_codepanel.c, tests/integration/test_r37_codepanel.cmake
  (cmake_minimum_required 3.16), tools/r37_e/*.py. Kein release/, platform/android/, shared_assets/, keine Test-CMakeLists.
* Keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse/Slots belegt (Modul liest nur re15_game_flag_get(3,F)).
  Kein neues Asset (LAMPE2130.TIM == room2110/esp16.tim, Riegel anker, selbst gelaufen).
* Tests je Punkt vorhanden und messend: E2 unit_r37_codepanel_kamera (+Gegenprobe, Frueh-Schliessen), E1
  unit_r37_codepanel_lampe / _zeichner, beide integration_r37_codepanel (exe, Logs + Framedump-Farbdominanz), Bytes _anker.

### Tests (selbst gelaufen, nur ctest -R)
```
ctest -R "r37_codepanel" --timeout 900 -V           -> 100% tests passed, 0 tests failed out of 5 (integration 81.85 s)
ctest -R "<57 Tests>" --timeout 600 -j 3            -> 100% tests passed, 0 tests failed out of 57
  (unit_scd_*, prop_scd_pc_advance, unit_npc_scd_pose, unit_aot_*, unit_msg_dialog, unit_action_msg_gate, unit_camera_view,
   unit_cut_old_displayed, unit_r34n_b_cut_old, unit_11f0_cut_after_puzzle, unit_zombie_10d0_*, unit_zombie_se_ab_10d0,
   probe_10d0_situp_handoff, r19_marke1_10d0_c1, unit_espr_11e0, unit_r34n_f_leiche_*_1230, unit_r35_karte_r1230,
   unit_r21_discard_wegwerfen, unit_r26_panel_11f0, unit_r27_panel_schalterwerte, unit_r31_generator, unit_r34n_c_generator,
   unit_r36_tueren_*, integration_r36_tueren, integration_door_lock, integration_keydoor, integration_headless_run,
   unit_r35_entladen_*, integration_r35_entladen_a, unit_r36_kartenbits)
```

## Nebenwirkungen (gemessen)
* Beide Szenarien: Leon 10D0/1230 und Elza 10D1 am echten Weg gleich (s. E2). 11E0/11E1: Schloss-Flag (3,139) setzt der
  Neuspiel-Raum ROOM1240 sub02 @0x0055A `22 03 8b 01` (selbst gelesen; evt-Trace "F0 room=1240 Evt_exec sub=2") -> dort
  erscheint das Tastenfeld im Spiel nie; nur headless gemessen (Riegel kamera/lampe gruen).
* "You've opened the lock." und die Wegwerf-Abfrage (Runde 21/22) stehen jetzt auf dem Panel statt im Raum; Antwort und
  Wirkung unveraendert (discard.log weg=1, unit_r21_discard_wegwerfen gruen). Danach Raumkamera wie vorher (Cut von vor
  dem Panel: 10D0 Cut 0, 1230 Cut 6).
* Andere Raeume: Haken nur bei armiertem Anker (Wachposten 0), keine Lampe ausserhalb der sechs Raeume/Panel-Cuts.

## Hinweise (nicht blockierend)
* **H1** `engine/src/codepanel_r37.c:162` Notbremse `s_folge_letztes + 2` traegt weder @0x noch "PORT-WAHL"; sie greift nur,
  wenn die Aufschliess-Folge nicht weiterzaehlt (der Zaehler laeuft in der Engine je Spielbild, unabhaengig vom Ton-Geraet).
  In allen meinen Laeufen "gewartet 0 Bilder". Empfehlung: als PORT-WAHL (Notbremse) kennzeichnen.
* **H2** Schliesst man Nachricht + Wegwerf-Abfrage so schnell wie moeglich (Viereck gehalten, r2d), geht die Kamera F1219
  zurueck; der Aufschliess-Ton (F1196..F1231) beginnt auf dem Panel, die letzten ~12 Bilder klingen im Raum aus — wie RE2
  ROOM2110 (Kamera direkt hinter dem Aufschliess-Se_on).
* **H3** Waehrend Nachricht/Abfrage bleibt der Ziffern-Cursor auf der letzten Taste sichtbar (Panel-Ansicht gehalten).
* **H4** Der runde Schein (48/64 px) streift die Oberkanten der Tasten 1/2 bzw. 2/3 (vom Bau-Agenten gemeldet).
* **H5** Veraltete Beschreibungen: `re15_port/tests/unit/probes/r37_codepanel.cmake:6` ("lampe E1: blau bis zum Piep-Bild,
  gruen ab dem naechsten ... falscher Code bleibt blau") und die Punkteliste oben im Dossier `E_codepanel.md` ("E1 ... blau,
  solange ...; gruen, sobald ...") widersprechen dem gebauten und geprueften Stand (rot -> blau/gruen, Nachtrag 3/3b;
  §2.4 des Dossiers ist richtig). Nur Text, Tests messen das Richtige.
* **H6** Integration: `AUFTRAG.md` im Zweig (Orchestrator-Commits 75b08666/985f0ad0) ist textlich anders als auf master
  (master traegt Nachtrag 2/3/3b/4/4b/4c/5/6) -> Konflikt beim Zusammenfuehren; master-Fassung nehmen (enthaelt 3/3b).
  Neue Datei `platform/pc/src/codepanel_r37_pc.c` -> Android-GLOB-Cache neu konfigurieren; RE15_MIN_TESTS +5.
* **H7** In der Raumansicht leuchten die (winzigen) gemalten Lampen des Lesers nicht (RE2 setzt seine ESP-Lampen im Raum,
  hier nur im Panel-Cut). Nach dem Oeffnen erscheint das Panel nie wieder; vom Wortlaut nicht verlangt.

## Maengel (blockierend)

Keine.

## Ergebnis

| Punkt | Urteil | Beleg (kurz) |
|---|---|---|
| E1 Lampe | erfuellt | echter Weg 10D0/10D1/1230: zu -> rot gemaltes Glas leuchtet rot (0..2 Grad, z.B. (241,79,73)), auch bei "Wrong code"; richtig -> 10D0 blau (212..221 Grad, (122,172,229)), SECUGUARD gruen (120 Grad, (64,225,65)), andere Lampe wie gemalt; Wechsel im Bild nach dem Piep |
| E2 Kamera | erfuellt | echter Weg 10D0: Panel-Cut 11 von F783 bis F1371, Piep F1180 (Mitschnitt t1511..1525), Aufschliessen F1195 (hoerbar F1196..F1231), Raum erst F1372; 1230 Piep F1412 / Aufschliessen F1427 bei Cut 9, Raum F1604; Elza 10D1 gleich |

Gates: RE-Gate haelt, Vertrag haelt, Tests 5/5 + 57/57 gruen. **Bestanden** (Hinweise H1..H7 nicht blockierend; H5/H6
bei der Integration mitnehmen).

Messordner (nicht versioniert): Scratchpad `runs/r1_weg, r0_bref, r1b_weg, r2a_echt, r2b_echt, r2c_falsch4311, r2d_halten,
r3a_weg1230, r3b_1230, r3c_1230, r4a_elza_weg, r4b_elza` (debug.log, cp.log, auf.log, cut.log, discard.log, wp_log.txt,
cap.raw, fd_*.ppm), Bildbogen `e_r2_reihe1/2.png`, `e_r2c_falsch.png`, `e_r3_1230_a/b.png`, `e_r4_elza.png`,
`e_zoom_lampen.png`.
