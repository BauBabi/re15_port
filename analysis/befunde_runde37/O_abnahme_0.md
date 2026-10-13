# Runde 37 Spur O "szene11c0" — Abnahme 0 (unabhaengig)

Datum 2026-10-13. Baum `.claude/worktrees/r37_szene11c0`, Zweig `r37/szene11c0`, HEAD 64e98ec4, Basis 07bb463a
(master hat seit der Basis keinen Port-Code geaendert: `git diff 07bb463a master --stat -- re15_port` leer).
Massgeblich: AUFTRAG.md Nachtrag 11 + Nachtrag 12 (Wortlaut), VERTRAG.md §1.4/§1.5.

STAND: abgeschlossen — bestanden (Abschnitt 8).

## 0. Bau

`bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK`, `ninja: no work to do`.
exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-12 16:20; letzte Aenderung an engine/include im Zweig = c44d3df2
(Quellen-mtime 16:19/16:20, Arbeitsbaum sauber) -> die exe entspricht dem Code-Stand HEAD.

## 1. Code-Diff (gelesen, `git diff master...HEAD -- re15_port`)

Spielcode: `irons_tod_1150.c` (Programm 4 `k_p_11c0` neu, Makros `OP_DIR`, `IT_GIER_*`, 1 Include + 1 Haken-Zeile in
`re15_irons_tod_tick`), neu `szene11c0_r37.c` + `re15_szene11c0_r37.h`. Tests: neu `probes/r37_szene11c0.cmake`,
`test_r37_szene11c0.c`, `integration/test_r37_szene11c0.cmake`; angepasst `test_r35_cut1150.c` (+Dir_set-Laenge),
`test_r36_szene1150.c` (msg 12/14), `integration/test_r36_szene1150.cmake` (K2). Werkzeuge `tools/r37_o/*.py`.
Keine release/, platform/android/, shared_assets/, keine Test-CMakeLists.

## 2. RE-Gate (Stichproben SELBST disassembliert, richtige Binaerdatei)

Werkzeug `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` (Hauptbaum, nur gelesen).

| Behauptung (Diff/Dossier) | selbst gelesen | Urteil |
|---|---|---|
| Spawn-Gier Sce_em_set Satz +16 -> +0x6a @0x800421bc/@0x800421c4 (PSX.EXE) | `800420f4 addiu s2,a1,2` (s2 = pc+2), `800421bc lhu v0,14(s2)` (= Satz +16), `800421c4 sh v0,106(s0)` (+0x6a = rot_y), daneben `sh zero,104(s0)` / `sh zero,108(s0)` | haelt |
| Dir_set LAB_80041080, rot_y @0x8004109c, Laenge 8 @0x800410a4 (PSX.EXE) | `80041084 lw a1,340(a0)` (Work +0x154), `80041094 lhu v0,4(v1)` / `8004109c sh v0,106(a1)`, `800410a4 addiu v1,v1,8` | haelt |
| NPC-Kopf-INIT: Klemme 0x2c8 @0x8011c79c/@0x8011c7a0, +0x1b8 @0x8011c768 (STAGE1.BIN) | `8011c768 sb zero,440(v0)` (+0x1b8 = 0), `8011c78c ori v0,zero,0x40` / `8011c790 sh v0,1528(v1)`, `8011c79c ori v0,zero,0x2c8` / `8011c7a0 sh v0,1532(v1)` | haelt |
| RE1.5 Plc_neck @0x80041e98: 0x80 nach +0x1b8, Modus >= 5 ohne Fall (PSX.EXE) | `80041ea4 ori v0,zero,0x80` / `80041ea8 sb v0,440(v1)`, `80041eac sltiu v0,a1,0x5` / `80041eb0 beq v0,zero,0x80041f18` | haelt |
| RE2 Plc_neck Modus 5: +0x172 = 0x8000 @0x80056f70/74 (re2leon PSX.EXE) | `80056f6c sh v0,368(a1)` (+0x170 = Figur), `80056f70 addiu v0,zero,-32768` / `80056f74 sh v0,370(a1)` | haelt |
| RE2 Kopf-FSM @0x800178a0: Weltpunkt-Zweig, Figur-Ziel bei 0x8000 (re2leon PSX.EXE) | `80017870 andi v1,v1,0x4`, `8001787c lh v0,370(s0)`, `800178a0 ori v1,zero,0x8000` / `800178a4 bne v0,v1,..`, `800178b8 lw a0,-492(at)` (Tabelle 0x800cfe14) | haelt |
| Kegel 0x60 @0x80076c41 | `80076c40: 00 60 00 60 ..` -> Byte @0x80076c41 = 0x60 (Zuordnung Typ -> Index nicht nachgeprueft; Wert stammt aus r36 und wird nur im Test benutzt) | haelt (Wert) |
| Datei-Bytes ROOM11C0 @0x01770 Ada rot 475; @0x01886/@0x018C0 Plc_neck m1 Rate 0x64; @0x018CA Plc_dest m9 Bit 1; @0x018D2 `18 06`; @0x01BB2/@0x01BC0 Kopf frei / Nicken | `xxd`: `4400 4240 0000 00ff fbdc 0000 f5c7 0000 db01 ..`; `4101 fbdc 0000 f5c7 6400`; `4101 cbe4 0000 33cf 6400`, `4000 0901 cbe4 33cf`, `1806`; `4100 .. 6400`, `4103 0100 .. 003c` | haelt |
| Datei-Bytes ROOM10D0 @0x01ABA..@0x01AFA (Vorbild Aufstellung) | `290e 3200 fb1d 0000 9150 3300 0000 ea06 0000 4101 5717 0000 714d 6400 ..`, `2e01 00`, `3200 5717 0000 714d 3300 0000 f20e 0000 4101 fb1d 0000 9150 6400` | haelt |
| Peilung = Form des Ports | `re15_s11c0_gier` = woertlich `enemy_ai_common.c:3536` (`re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff`) | haelt |

Konstanten im Diff ohne `@0x`: `IT_GIER_ADA_MARVIN 3679` / `IT_GIER_MARVIN_ADA 1631` (berechnet aus den belegten Orten,
der Riegel rechnet sie nach), Sleeps (`OP_SLEEP(3/8/6/36/4/8/20/30/12/60)`) = als PORT-WAHL "Takte" gekennzeichnet
(Kopfkommentar "Takte PORT-WAHL", AUFTRAG Nachtrag 11 "Takte PORT-WAHL"), Signal-Bit (5,13) mit Zensus und Raum-Init
@0x8003ed74 (Port `scd_room_setup.c:305 g_game.flags[5][0] = 0`), Aktor-Nummern 1/4 (r35-Bestand, Kopf des Headers).
Suche nach "deferred|tunable|interim|for now|faithful|plausibel|TODO" im Code-Diff (`git diff master...HEAD -- re15_port | grep -i`): kein Treffer.
Gate RE: **haelt**.

## 3. Eigene Messung an der exe (Lauf A1)

### Weg und Werkzeug
* Messkopie `re15_pc_r37o_abn0_a1.exe` (Kopie der gebauten exe, nach dem Lauf geloescht; nie taskkill).
* ECHTER Weg wie integration_r35_cut1150 Lauf A: Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene`,
  `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1` (debug.log `[save] CONTINUE: resumed in room 1130 (hp=100)`), Aktionstaste an der
  Tuer (`RE15_INPUT_SCRIPT=W1,A0.2,W200`, Basis spiel, Start 60) -> ROOM1150 Szene -> Montage 1130 -> 1040 -> 1030 -> ROOM11C0
  (debug.log `[irons-tod] ROOM11C0 Zustand 5: Montage 11C0 (Programm 4)`), Ende `RE15_EXIT_AT=513#11C0`
  (`[flow] EXIT_AT: Bild 513 in Raum 11c0 erreicht -> exit`). Beschleunigter Renderer (KEIN SOFTWARE_RENDER),
  `RE15_WINDOW_SCALE=1`, `RE15_FRAMEDUMP=0-512/2:f_`, Protokolle `RE15_S11C0_LOG`, `RE15_NECK_TRACE`, `RE15_NECK_LOG`, `RE15_STATE_LOG`.
  Laufzeit 1 min 55 s. Skript `<scratchpad>/abn/lauf.sh`.
* Auswertung mit EIGENEM Skript (`<scratchpad>/abn/auswertung.py`, nicht der Auswerter der Spur): Drehabschnitte aus
  der Gier je Bild, je Zeile Koerper-Abweichung zur Peilung auf den Partner, Kopf-Zielpunkt, und aus neck_trace der
  GEZEICHNETE Kopf (`|kf + acc - tgt|`, Ziel an der Klemme 0x2c8 ja/nein). Bilder: Kontaktboegen aus den Framedumps
  (`<scratchpad>/abn/z_start.png`, `z_gespraech.png`, `z_ende.png`, `b_ende.png`, `z_fade.png`).
  (`<scratchpad>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`;
  die Bilder sind NICHT committet — Auftrag: nur diese Datei.)

### Ergebnis (wortgetreu aus der eigenen Auswertung)
```
Bilder 508 F6..F513
Zeilen: 10 F72..F169 11 F170..F224 12 F225..F284 13 F285..F383 14 F405..F503
Ada: Start-Gier 3679 (Peilung auf Partner 3679, +0.0 Grad daneben)
  Ada dreht F397..F404 3679 -> 3009  -670 Einh = -58.9 Grad  (7 Bilder, 8.4 Grad/Bild)
Marvin: Start-Gier 1631 (Peilung auf Partner 1631, +0.0 Grad daneben)
  Marvin dreht F385..F400 1631 -> 2749  +1118 Einh = +98.3 Grad  (15 Bilder, 6.6 Grad/Bild)
  Marvin dreht F427..F458 2749 -> 2803  +54 Einh = +4.7 Grad  (31 Bilder, 0.2 Grad/Bild)
  Marvin dreht F479..F513 2803 -> 2689  -114 Einh = -10.0 Grad  (34 Bilder, 0.3 Grad/Bild)
msg 10 Sprecher Ada   : Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 98/98, frei 0, Tuer 0
msg 10 Zuhoerer Marvin: Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 98/98, frei 0, Tuer 0
msg 11 Zuhoerer Ada   : Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 55/55, frei 0, Tuer 0
msg 11 Sprecher Marvin: Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 30/55, frei 25, Tuer 0
msg 12 Zuhoerer Ada   : Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 60/60, frei 0, Tuer 0
msg 12 Sprecher Marvin: Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 60/60, frei 0, Tuer 0
msg 13 Zuhoerer Ada   : Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 99/99, frei 0, Tuer 0
msg 13 Sprecher Marvin: Koerper zum Partner max 0.0 Grad, Mittel 0.0 | Kopf-Ziel Partner 99/99, frei 0, Tuer 0
msg 14 Sprecher Ada   : Koerper zum Partner max 28.1 Grad, Mittel 21.1 | Kopf-Ziel Partner 99/99, frei 0, Tuer 0
msg 14 Zuhoerer Marvin: Koerper zum Partner max 180.0 Grad, Mittel 173.3 | Kopf-Ziel Partner 0/99, frei 99, Tuer 0
vor msg 10 A: Kopf zur Tuer in 36 Bildern (F24..F59), Gieren [3679]
vor msg 10 M: Kopf zur Tuer in 46 Bildern (F18..F63), Gieren [1631]
neck msg 10 Ada   : 98 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 3.3 Grad
neck msg 10 Marvin: 98 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 6.5 Grad
neck msg 11 Ada   : 55 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 2.4 Grad
neck msg 11 Marvin: 55 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 6.5 Grad
neck msg 12 Ada   : 60 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 2.4 Grad
neck msg 12 Marvin: 60 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 6.5 Grad
neck msg 13 Ada   : 99 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 2.4 Grad
neck msg 13 Marvin: 99 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 4.7 Grad
neck msg 14 Ada   : 99 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 27.0 Grad
neck msg 14 Marvin: 99 Bilder, Ziel an der Klemme 0, max |kf+acc-tgt| 0.0 Grad, max |acc| 0.0 Grad
```
Schluesselbilder (s11c0.log, Gier r, Peilung gp auf den Partner):
```
F396 Ada r=3679 gp=3421 | Marvin p=(-8443,-13462) r=2591 (laeuft los)
F400 Ada r=3391 s=9 nt=(-8943,-12530) (dreht, Kopf-Ziel = Marvins Kopf-Ort) | Marvin p=(-8873,-12787)
F404 Ada r=3009 gp=2989 (zugewandt, 20 Einheiten)  F405 msg=1:14 "Marvin!..."
F416 Ada r=3009 gp=2865 nt=(-10496,-9781) | Marvin p=(-10393,-9971)
F456 Ada r=3009 gp=2691 (318 Einheiten = 28 Grad neben dem Koerper, Kopf folgt) | Marvin p=(-15514,-4475)
```
Die Zahlen stimmen bildgenau mit dem Dossier (Lauf n5) ueberein: Zeilen 10 F72..F169 .. 14 F405..F503, Ada -58,9 Grad
F397..F404, Marvin +98,3 Grad F385..F400 (+4,7 / -10,0 Nachsteuern im Lauf). Der Lauf ist deterministisch reproduziert.

### Bildbefund (Framedumps, selbst angesehen)
* F0..F4 Einblendung (F4 x1,4 aufgehellt: Silhouetten, nicht beurteilbar); F6 erstes helles Bild: Marvin links im Profil
  nach rechts, Ada rechts im Profil nach links — einander zugewandt (z_start.png F6/F8/F14).
* Knall F18..F63: nur die Koepfe drehen zur Kamera-Seite (Gesichter sichtbar F24..F60), Koerper bleiben im Profil;
  F66/F72 Koepfe wieder zueinander.
* msg 10 (F90..F150): Ada fragt mit Geste (Clip 19), beide im Profil zueinander. msg 11 (F176..F214): Marvin nickt,
  sieht Ada an. msg 12 (F240/F270): beide zueinander. msg 13 (F300/F330): Marvins Arm zu Ada (Clip 15), Koerper zu Ada.
* F384: Untertitel 13 weg; F385..F400 Marvin dreht zur Kamera und rennt nach vorn aus dem Bild (bis ~F416). Ada steht
  F396 noch im Profil, dreht F398..F404 in Marvins Richtung (zur Kamera), F404/F405 "Ada: Marvin!..." ihm zugewandt, ab
  F416 Arm-Geste in seine Richtung; danach steht sie zur Kamera-Seite (Kopf folgt ihm, im Bild kaum sichtbar).
* Kein Gleiten, kein Schnappen, keine haengende INIT-Pose (der n1-Befund der Spur ist im Endstand weg: F6 Ada in Standpose).
* Marvin laeuft nicht durch Ada: Weg (-0,48/+0,88), Ada liegt hinter ihm (Skalarprodukt < 0); die Ueberdeckung F398..F402
  ist Perspektive (er ist naeher an der Kamera).

### Trennschaerfe des Riegels (Mutationsprobe, selbst)
Auswerter der Spur (`test_r37_szene11c0 auswerten`) auf MEINEN Lauf A1: `auswerten: 0 Fehler` (gleiche Werte wie Dossier).
Dann auf vier gezielt verfaelschte Kopien meines s11c0.log (neck_trace unveraendert):
| Mutation (= ein Nutzer-Befund) | Ergebnis |
|---|---|
| Ada bleibt bei msg 14 stehen (r=3679 ab F397) | 2 Fehler ("genau EINE Drehung Adas" 0 Abschnitte; Koerper bei msg 14 710 Einheiten daneben) |
| beide Anfangs-Gier 475 bis F44 (alte Aufstellung) | 4 Fehler (erstes Bild nicht zugewandt; Drehungen im Gespraech) |
| Marvin Kopf-Ziel zur Tuer bei msg 12 | 2 Fehler (Kopf 0/60; gezeichneter Blick 104,2 Grad) |
| Marvin Koerper bei msg 12 98 Grad weg (r=2745) | 2 Fehler (Drehungen vor dem Lauf; Koerper 0/60) |
Der Riegel erkennt jede der vier Nutzer-Beobachtungen.

## 4. Vertrag (VERTRAG.md §1)

| Pruefpunkt | Befund | Urteil |
|---|---|---|
| Pfade (`git diff master...HEAD --name-only`) | nur re15_port/engine/src/irons_tod_1150.c, engine/src/szene11c0_r37.c (neu), include/re15_szene11c0_r37.h (neu), tests/unit/{probes/r37_szene11c0.cmake, test_r37_szene11c0.c, test_r35_cut1150.c, test_r36_szene1150.c}, tests/integration/{test_r37_szene11c0.cmake, test_r36_szene1150.cmake}, tools/r37_o/*.py, analysis/befunde_runde37/*. Keine release/, platform/android/, shared_assets/PSX/, keine tests/CMakeLists | haelt |
| §1.4 Spur O: Programm 4 an Ort und Stelle, Kommentar "Runde 37 Spur O"; uebrige Programme (Spur H) und k_msg28 nicht anfassen | Hunks in irons_tod_1150.c: @@16 (Include, Kommentar "Runde 37 Spur O"), @@418/@@429 (nur Kommentar + Makros + `k_p_11c0`), @@1174 (1 Haken-Zeile, Kommentar "Runde 37 Spur O"). `k_msg28` kommt im Diff nicht vor; kein anderes Programm geaendert | haelt |
| Neue Logik in neuer Datei `<thema>_r37.c` | `szene11c0_r37.c` / `re15_szene11c0_r37.h` | haelt |
| §1.1 Bank-9-Bits / §1.2 Nachrichten-Ids / §1.3 Ereignisse | keine. Neu: Bank 5 Wort 0 Bit 13 (Raum-Scratch, Port-Init `scd_room_setup.c:305` = @0x8003ed74) nur in ROOM11C0 waehrend Programm 4; selbst geprueft: irons_tod nutzt Bank 5 Bits 12/20/28/29/30, kein anderes Modul liest/setzt (5,13) | haelt |
| §1.5 Tests nur ueber probes/r37_szene11c0.cmake; integration mit `cmake_minimum_required(VERSION 3.16)`; POSIX/popen | ja; `test_r37_szene11c0.cmake` Zeile 11 `cmake_minimum_required(VERSION 3.16)`; kein popen (nur fopen/sscanf) | haelt |
| Tests je Punkt vorhanden und messend | aufstellung/ablauf/integration decken (a)-(d) + Nachtrag 12 ab; Mutationsprobe (3.) zeigt Trennschaerfe | haelt |
| Merge-Faehigkeit | `git merge-tree --write-tree r37/integration HEAD` -> sauber (kein Konflikt), auch gegen Spur H (r37/zombieaktiv aendert irons_tod_1150.c an anderer Stelle) | haelt (textuell) |

## 5. Tests (selbst gefahren, KEINE volle Suite)

Regex = Gruppen des Bau-Agenten PLUS die Gruppen, die er ausgelassen hat, obwohl ihre Quellen `irons_tod`/`re15_irons_tod`
benutzen (`grep -l irons_tod tests/unit/*.c tests/integration/*.cmake`: test_r36_gegnerzahl.c, test_r36_kartenbits.c,
test_r36_szenenleon.c) und die Nachbarn `unit_r35_affen_*`, `unit_r36_zombieki_szenen`:
```
ctest --test-dir re15_port/build -R '^unit_r37_szene11c0_|^integration_r37_szene11c0$|^unit_r35_cut1150_|^integration_r35_cut1150$|^unit_r36_szene1150_|^integration_r36_szene1150$|^unit_irons_|^unit_room1150_itembox$|^unit_sicherung_1150$|^unit_r30_irons|^integration_r30_irons|^unit_montage_fx$|^unit_marvin_|^unit_neck_headlook$|^unit_r35_affen_|^unit_r36_karte_tuer11c0$|^unit_r36_szenenleon_|^integration_r36_szenenleon$|^unit_r36_elza_irons$|^integration_r36_elza_irons$|^integration_r34n_g_schrift1150$|^unit_door_seq$|^integration_room_transition$|^unit_r35_cut10f0_|^integration_r35_cut10f0$|^unit_1030_|^unit_room1040_switch$|^unit_cam_1030_reentry$|^probe_r16_tuer1040|^unit_r36_gegnerzahl_|^unit_r36_kartenbits$|^unit_r36_zombieki_szenen$' --timeout 900 --output-on-failure -j 3
100% tests passed, 0 tests failed out of 97
Total Test time (real) = 523.82 sec
```
Darin: integration_r37_szene11c0 114,3 s, integration_r36_szene1150 116,9 s, integration_r35_cut1150 513,9 s,
integration_r35_cut10f0 243,3 s, integration_r36_szenenleon 161,6 s, integration_r36_elza_irons 16,5 s (Elza-Szenario:
die Kette laeuft dort nicht, Riegel gruen), unit_r36_gegnerzahl_* / unit_r36_kartenbits gruen. Kein roter Fenster-Haken,
keine Nachfahrt noetig (unter Last anderer Agenten: r35l/r35k/r36k/r36r/r37p6-Messkopien liefen parallel).

## 6. Urteil je Nutzer-Punkt (gegen den Wortlaut)

| # | Wortlaut | Messung (Lauf A1 + Bilder) | Urteil |
|---|---|---|---|
| 1 | "Und die Dialogszene mit Ada/Marvin nach Irons Tod ist auch noch nicht so geil von den Animationen...." | Die vier konkret genannten Fehler (Punkt 2) sind am Bild und im Protokoll weg; keine neuen Bildfehler (kein Gleiten, kein Schnappen, keine haengende INIT-Pose, Gesten Clip 19 / Nicken / Clip 15 wie r35/r36, Marvin laeuft nicht durch Ada). Was "geil" ist, bleibt Geschmack des Nutzers — gemessen ist alles, was er beanstandet hat | erfuellt |
| 2a | "Erst schauen sie komplett in die falsche Richtung." | erstes helles Bild F6: Ada Gier 3679 = Peilung auf Marvin (0,0 Grad), Marvin 1631 = Peilung auf Ada (0,0 Grad); Bild z_start F6/F8/F14 Profil zueinander. Ursache der alten Richtung belegt (Ada-Record @0x01770 rot 475, Marvin uebernahm 475) | erfuellt |
| 2b | "Dann drehen sie sich einmal um 180 grad um." | vor msg 10 Gieren unveraendert (Ada nur 3679, Marvin nur 1631); Knall = nur Koepfe (Kopf-Ziel Tuer Marvin F18..F63, Ada F24..F59). Groesste Koerperdrehung der ganzen Szene Marvin +98,3 Grad F385..F400 = Losrennen zur Tuer hinter der Kamera, NACH seiner letzten Zeile | erfuellt |
| 2c | "Dann dreht sich Ada noch mal um 90 grad um." | Ada dreht im Gespraech (msg 10..13) gar nicht; einzige Drehung F397..F404 -58,9 Grad = die vom Nutzer gewuenschte Drehung nach Marvin (Nachtrag 12) | erfuellt |
| 2d | "Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada...." | msg 11/12/13 (Marvin spricht): Koerper 0,0 Grad zur Peilung auf Ada in 55/60/99 Bildern, Kopf-Ziel Ada 30/55 (+25 frei = Nicken) / 60/60 / 99/99, gezeichneter Kopf nie an der Klemme, Rest `|kf+acc-tgt|` 0,0 Grad; Zuhoererin Ada ebenso. Bilder F176..F360 | erfuellt |
| N12 | "wenn Ada sagt: "Marvin!" kann sie sich gerne den weglaufenden Marvin nach drehen." | Marvin rennt ab F385 (Drehung) / F396 (Ort aendert sich); Ada dreht F397..F404 genau einmal (-58,9 Grad, 8,4 Grad/Bild = Drehrate der Engine wie die uebrigen Drehungen), ihr Drehpunkt folgt Marvins Ort (s11c0.log F400 nt=(-8943,-12530) ~ Marvin (-8873,-12787)); F405 "Ada: Marvin!..." ihm zugewandt (gp 2969 vs r 3009 = 3,5 Grad), Kopf-Ziel folgt ihm 99/99 Bilder, gezeichneter Blick max 1,9 Grad (Auswerter) / Rest 0,0 Grad (eigene Auswertung); Bild z_ende F396..F416 | erfuellt |

## 7. Maengel

Keine blockierenden Maengel. Hinweise (nicht abnahmerelevant, fuer den Nutzer/Integration):
1. Der gezielte Lauf des Bau-Agenten (64 Tests) liess Gruppen aus, deren Quellen das Modul benutzen
   (unit_r36_gegnerzahl_*, unit_r36_kartenbits, unit_r36_szenenleon_* ausser _irons, integration_r36_szenenleon,
   unit_r35_affen_kette, unit_r36_zombieki_szenen). Selbst nachgefahren: alle gruen (97/97).
2. Nach der Drehung folgt nur noch Adas Kopf; der Koerper bleibt bei 3009 stehen, Marvins Peilung wandert bis 28 Grad
   daneben (F456, er ist da schon aus dem Bild). Deckt den Wortlaut ("nach drehen"); falls der Nutzer eine laufende
   Koerper-Nachfuehrung will, waere das ein neuer Wunsch.
3. "Ada folgt Marvin" ist ein szenen-eigener Port-Takt (RE1.5 kennt Plc_neck Modus 5 nicht, @0x80041eac selbst gesehen;
   RE2-Vorbild @0x80056f70/@0x800178a0 selbst gesehen) — im Dossier OFFEN sauber benannt.
4. Integration: irons_tod_1150.c wird auch von Spur H/L geaendert; `git merge-tree` gegen r37/integration ist sauber,
   der Integrationsbaum muss die Kette trotzdem einmal mit beiden Staenden fahren (integration_r35_cut1150, _r36_szene1150,
   _r37_szene11c0).

## 8. Ergebnis

Alle Punkte erfuellt, RE-Gate haelt (10 Stichproben selbst disassembliert/gelesen, alle stimmen), Vertrag haelt,
gezielte Tests 97/97 gruen, Riegel trennscharf (Mutationsprobe). **bestanden = ja.**
