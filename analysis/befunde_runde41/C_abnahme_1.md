# Runde 41 — Spur C "ventilkette" — Abnahme 1 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, HEAD 5ddd98c0 (Basis master 116e8b0f; master-Tip
71b52f4c aendert seit der Basis nur analysis/). Begonnen 2026-10-24 07:03. Pruefer: Abnahme 1 (Opus). Der Bericht
wird FORTLAUFEND geschrieben. Messordner: scratchpad `C41abn1/` (Pfade unten abgekuerzt als `<a1>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn1`).
Vorlage: Abnahme 0 (`C_abnahme_0.md`, Maengel M1-M9, H1-H3), Nachbesserung 1 (Dossier §0b), Entscheide C-E1..C-E3.

## 0. Stand / Protokoll (neueste zuerst)
* 08:05 Bericht abgeschlossen (§2-§6). Aufraeumen: keine exe-Kopien `re15_pc_abn1c_*`/`re15_pc_r41ck_*` mehr in den
  Bau-Ordnern, Verbindung `hakenbau/re15_port/shared_assets` mit `rmdir` entfernt (Ziel unberuehrt), Baum: nur dieser Bericht
  neu (`git status --porcelain`).
* 07:20-08:00 Messserien 1-6 an der exe (§1): Bruecke (L2), Flut/Tod (L3b), Hebel/Kisten/Pult (L5), nach der Flut (L4),
  Ring-Sperren (L5b), Ventile leer (L8), Rechner (L6), Elza (L7), Ost-Tuer ohne Haken (L3o) und — neu — der Weg zum Rechner
  nach der Flut (L9 -> Mangel N1). Drei Laeufe verworfen und neu gefahren: `tuerebene`/`tuerebene2` (die Spinne des Cut-13-
  Eintritts, Kill-Bit 7:158, stand im Weg), Elza-Laeufe mit `RE15_TITLE_SHOT` (luden Leon). Eigene exe-Kopien stets per PID
  beendet, nie per Bildname.
* 07:18 r41-Integrationsriegel 2/2 gruen (§3); 07:20-07:26 Heliport-Riegel 5/5 gruen (§3); 07:17-07:22 Ketten-Riegel im
  Messbau mit Spur-A-Haken gruen (L3k).
* 07:16 Messbau "Spur-A-Haken" (NICHT im Baum; Kopie `<a1>/hakenbau/re15_port` ohne build/shared_assets, Skript
  `<a1>/py/haken_a.py`): in die KOPIE wortgleich aus r41/wasser20b0 @4acdead2 eingesetzt — aot_common.c die Bedingung
  `&& a->type != RE15_AOT_TYPE_WATER && a->type != RE15_AOT_TYPE_RAMP ... && !re15_wasser20b0_r41_auto_aot(...)` (A: Zeilen
  1170-1176), engine/src/wasser20b0_r41.c NUR mit dem Rumpf `re15_wasser20b0_r41_auto_aot` (A: Zeilen 171-176), Kopf mit dem
  Prototyp. Bau `<a1>/hakenbau_bau.sh` (PATH msys64 zuerst wie local_build.sh, Assets/SDL2-Quelle des Spurbaums gelesen):
  `[498/498] Linking C executable platform/pc/re15_pc.exe`, exe 07:16 (String `wasser20b0_r41` 2x). Configure registriert
  dort `integration_r41_ventilkette_kette` (`ctest -N -R r41_ventilkette` -> 4 Tests), im Spurbaum nicht (3 Tests).
* 07:10 P1 Leiter 2000 an der exe (§1 L1).
* 07:06 `ctest -R "^integration_r41_ventilkette(_b)?$" --timeout 1500` gestartet (Protokoll `<a1>/ctest_integ_r41.txt`):
  `1/2 Test #1126: integration_r41_ventilkette ......   Passed  222.08 sec` (Rest §3).
* 07:05 Unit-Gruppe (164 Tests, Regex `<a1>/regex_unit.txt` = Abnahme-0-Regex + Kraehen/Heliport-Gruppen des Bau-Agenten +
  unit_r30_tuer/map_zone_bits/fx_emitter_ai/r38_tagebuch*/room1050_sicherung/npc_back_walk = Unit-Riegel, die 2030/20B0/
  2070/2050 lesen): `100% tests passed, 0 tests failed out of 164` (26.81 s, `<a1>/ctest_unit.txt`, EXIT=0).
  `test_r41_ventilkette.exe` direkt: `== Leon`, `== Elza`, `PASS (274 Pruefungen)` (`<a1>/unit_r41_v.txt`).
* 07:04 Generator nachgefahren (Kopie `<a1>/genrepro`, gleiche Eingabe-RDTs): alle vier gen/ventilkette_r41_*.inc
  byte-gleich (`cmp` ohne Unterschied: texte, original, programme, wasser).
* 07:03 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, Zeile 180 `-- integration_r41_ventilkette_kette
  uebersprungen: Spur A fehlt (engine/src/wasser20b0_r41.c, AUTO-Haken re15_wasser20b0_r41_auto_aot)`;
  `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. exe 06:37:09 (7799077 B).
  Geprueft, dass der temporaere Spur-A-Haken NICHT in der exe steckt: aot_common.c = Basis (`git diff 116e8b0f HEAD`
  leer), Quelle 06:37:07 < Objekt aot_common.c.obj 06:37:08 < libre15_engine.a 06:37:09 < exe 06:37:09; der String
  `wasser20b0_r41` kommt in der exe 0x vor (`ventilkette41` 19x); `git diff --quiet HEAD -- re15_port` = sauber.

## 1. Messungen an der exe (eigene Laeufe; Runner `<a1>/lauf.sh`, exe-Kopie je Lauf `re15_pc_abn1c_<lauf>.exe`, danach
geloescht; erbt keine RE15_-Variablen; Arbeitsordner `<a1>/mess/<lauf>/` mit debug.log/state.txt)

### L1 — Punkt 1 Leiter 2000 (`<a1>/mess/leiter`, Bildbogen `<a1>/leiter2000_sheet.png`)
Aufbau wie Abnahme 0 L1 (beschleunigter Renderer `[render] SDL-Renderer direct3d ... ABR2 (B-F) ok`): `RE15_DEBUG_JUMP=2000@240`,
`RE15_PLAYER_POS=-24500,10300,2048`, `RE15_FORCE_CUT=10` (nur Bildwahl), Eingabe `U0.8,W0.5,A0.1,W5,A0.1,W3` (hingehen,
Aktionstaste), `RE15_EXIT_AT=660#2000`.
* debug.log: `[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40 (Form ROOM2030 @0x01B12)` ->
  `[msgtext] room=2000 id=40 "I cannot climb up the ladder because of the water pressure."` -> `EXIT_AT: Bild 660 in Raum
  2000` (kein `loaded room2050`). state.txt F350 `PL(-26532,10332,rot=2048)` an der Leiterwand, `fx=23` (Strahlen-Effekte
  an), F369 `msg(a=1 fsm=1 id=40)`.
* Bild: der Original-Strahl faellt aus dem Gitterfenster neben der Leiter (ESP @0x0174C ff.), Leon an der Leiter, Text
  F420/F480 zweizeilig, Leon bleibt unten (F540-F660).
* Elza 2001: Riegel `_b` Lauf K (siehe §3).

### L2 — Punkt 2 Bruecke (`<a1>/mess/bruecke`, `<a1>/mess/bruecke3`; Wegplaene der Abnahme 0 unveraendert, `<a1>/plaene/`)
L2a `bruecke` (Start Ost-Tuer (6550,-100), Spinnen-Kill-Bits wie Abnahme 0, Wegplan-Autopilot = echte Pad-Bits + Kollision,
direct3d): `[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt` -> Ring Nord bis vor das Pult F309
(490,-9718), Blick 1072, Aktion F336 -> `[ventilkette41] ROOM2030 Ereignis 24 -> Port-Programm 2030_pult_ohne` ->
`[msgtext] room=2030 id=40 "A control panel for the bridge. The pressure gauge reads zero.Intake valve 13 is closed."`,
Wegplan `FLAG (9,101) = 0`; Treppe (-6356,-7726) Aktion F887 -> hinab bis F1187 (-2082,-7717) -> Landeplatz (-1107,-7717)
F1214; Versuch auf den Steg nach (-1050,-3000): state.txt F1500..F4190 `PL(-635..-1195,-6418)` = haengt an der Steg-
Zelle 12 (Bruecke oben). Kein Cut_chg(4). (Wie Abnahme 0 L2.)

L2b `bruecke3` (Start Landeplatz (-1107,-7717) Band 1 = Endlage der Treppe aus L2a, Wegplan `bruecke3.txt`; Bildbogen
`<a1>/bruecke3_sheet.png`): Ventil 13 (-1430,-6498) Aktion -> `ROOM2030 Ereignis 25 -> Port-Programm 2030_v13_frage` ->
`[msgtext] room=2030 id=41 "Intake valve 13. Will you open it?"` (F125) -> Ja -> `id=42 "You've opened valve 13. The
pressure is rising."` (F280) -> Wegplan F493 `FLAG (9,101) = 1`; Treppe HINAUF (Aktion F572 -> F872 oben (-6404,-7682));
Ring zum Pult (236,-9636) Blick 1040, Aktion F1067 -> `Ereignis 24 -> Port-Programm 2030_pult_senken` -> `id=43 "The
pressure gauge reads 80. Lower the bridge?"` -> Ja F1220 -> `[scd F1221] Cut_chg(4)` -> `[prop-member F1281] prop[0] obj=0x00
m=1 → val=-1800` -> `[scd F1315] letterbox closed` -> F1555 `FLAG (9,102) = 1` -> Aktion -> `2030_pult_unten` -> `id=44 "The
bridge is down."` (F1557); Treppe hinab (F1930-F2230) und ueber den Steg bis F2434 (-1043,2989) `angekommen` = Steg-Zelle 12
offen. Bild: Text 42 (F420), Frage 43 (F1095), Cut 4 Planke sinkt (F1230/F1260/F1290), Text 44 (F1575). Die Kamera bleibt
ausserhalb von Cut 4 bei Cut 0 (Start per PLAYER_POS ausserhalb der Cut-0-Zone, Messartefakt wie Abnahme 0). Die Lage des
Pults in Kamera 2 vor dem Schaltschrank der Nordwand (schaltschrank.png) hat Abnahme 0 L9 im Bild belegt; Code/Rechteck
seitdem unveraendert (`git diff 4af8fa3d HEAD` aendert RE15_R41C_PULT_RECT nicht).

### L3k — Punkte 3/4 die GANZE Kette in EINEM Lauf mit dem wortgleichen Spur-A-Haken (Messbau, M2 / Entscheid C-E1)
Riegel des Baums `tests/integration/test_r41_ventilkette_kette.cmake` (unveraendert kopiert) gegen die Messbau-exe
(`<a1>/hakenbau`, §0 07:16; Assets des Spurbaums ueber eine Verbindung `hakenbau/re15_port/shared_assets` gelesen — ein
erster Lauf ohne diese Verbindung fand keine Assets (`[asset] cd-root: .../hakenbau/...`) und ist UNGUELTIG,
`<a1>/ctest_kette_haken_ohne_assets_UNGUELTIG.txt`):
`ctest --test-dir <a1>/hakenbau/re15_port/build -R "^integration_r41_ventilkette_kette$" --timeout 900 -V` ->
`-- r41_ventilkette_kette: 2030 Ventile -> Ost-Tuer zu Fuss -> 20B0 Szene sub02/sub05 -> (3,235)=1 -> frei -> Kanal ->
Leiter -> ROOM2050 in einem Lauf (Haken der Spur A: 1)`, `1/1 Test #1117: integration_r41_ventilkette_kette ...   Passed
92.56 sec`, `100% tests passed, 0 tests failed out of 1` (`<a1>/ctest_kette_haken.txt`, EXIT=0).
debug.log (Laufordner `hakenbau/re15_port/build/tests/integration/r41_ventilkette_kette/lauf_5f2cb5bf_voll`):
`ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile` -> `[msgtext] room=2030 id=45 "Numbered valves. ..."` -> `id=46
"WARNING: The water will rise in one minute. Leave the room!"` -> `Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` ->
`[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11` (Aktionstaste, kein FIRE_AOT) -> `loaded room20b0.rdt`
-> `READ Ck zone=3 idx=235 exp=0 got=0` -> `Cut_chg(11)` -> `Bits 103 + 100` -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030
gesperrt (geflutet) -> Sperrtext 41` -> `Plc_dest(... -15920,-6355)` -> `thread start slot=11 first_op=0x46` (sub05) ->
`[scd F105] Cut_chg(7)` -> `[scd F146] Cut_chg(15)` -> `[scd F228] Cut_chg(6)` -> `WRITE Set zone=3 idx=235 op=1` ->
`[scd F242] letterbox closed -> gameplay` -> Wegplan F334 Sprung in den Kanal, F493 Aufstieg, F914 Leiter (-26014,10245)
-> `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` -> `loaded room2050.rdt`
(kein Sperrtext: Bit 100). Damit ist die Behauptung des Bau-Agenten (M2, "mit dem wortgleichen Haken laeuft die Kette in
EINEM Lauf") unabhaengig bestaetigt. Ohne den Haken (Spurbaum-exe) haengt die Szene weiter (L3o).

### L3b — Punkt 3 Zeit abgelaufen im Raum: Flut ueber den Ring und Tod (`<a1>/mess/flut`, direct3d, Bildbogen `<a1>/flut_sheet.png`)
Aufbau wie Abnahme 0 L3b (Spinnen-Kill-Bits, `RE15_PLAYER_POS=6550,-100,2048` vor den Ventilen, Eingabe
`A0.1,W4,A0.1,W2.5,A0.1,W4,A0.1,W4,A0.1,W2,R0.4` = Aktion, Ja, Warnung zu, stehen bleiben), `RE15_FADE_LOG=1`,
Framedumps F2500-2900/6. `[render] SDL-Renderer direct3d, Videotreiber windows, ABR2 (B-F) ok`.
* debug.log: `Ereignis 26 -> Port-Programm 2030_ventile` -> `id=45` -> `id=46 "WARNING: The water will rise in one minute.
  Leave the room!"` -> `Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> `Zaehler laeuft: 1800 Takte` -> `FLUT: Wasser
  steigt` -> `SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400), Effekt 0x06050D00` (genau 1x) ->
  `[fade-log] config ch=0 abr=2 mask=6 step=0 bucket=7`, `kick ch=0 value=0x5000` -> `ERTRUNKEN: Wasser y -6300, Spieler
  y -5400 -> hp -1, blaue Blende`.
* state.txt: Warnung zu F778 (F777 letzte Anzeige) -> Flut F2579 `cam=4 ... pf=21000007 pm=2` = **1801 Bilder = 60 s bei
  30 Bildern/s** -> Spritzer F2686 `fx=1` -> Tod **F2704 `hp=-1`** = 125 Bilder nach Flutbeginn -> YOU DIED.
* Farbe (Werkzeug `test_r41_ventilkette ppm`, Bildmitte (120,80)..(200,160) / Vollbild): F2698 (29,36,29) / (30,37,30) ->
  **F2704 (0,0,30) / (0,0,30)**, F2710/F2716 (0,0,30) = blauer Schleier ab dem Todesbild (Rot+Gruen abgezogen, Blau bleibt).
* Bild: F2572 Leon an den Ventilen, F2584..F2650 Draufsicht Cut 4 (Wasser im Becken unter der oben stehenden Bruecke),
  F2686 Wasser auf Ringhoehe (Gelaender taucht ein), F2698 alles unter Wasser, F2704 blauer Schleier, F2800/F2896 YOU DIED.
* Riegel integration_r41_ventilkette C (mein ctest-Lauf, `lauf_fa6b5ec8_c`): dieselben Saetze, Flut F2621 (cam 4) ->
  Tod F2746 = 125; Bildmitte F2740 (29,36,29), F2742/F2744 (30,37,30), **F2746..F2750 (0,0,30)** — der Riegel misst jetzt
  die Farbe (M4) und verlangt `ABR2 (B-F) ok`.

### L5 — Punkt 5 ROOM2050 (Pult -> Hebel) und M1 Kisten-Hebel (`<a1>/mess/kisten_hebel`, `hebel_vorher`, `pult`, `hebel_nachher`)
* `kisten_hebel` (M1; Wegplan `<a1>/plaene/kisten_hebel2.txt` = Plan der Abnahme 0 mit kuerzerer Wartezeit; Kisten-Bits
  `RE15_SET_FLAG_AT=5:0,5:1,5:2@300` = genau die Bits, die sub01 beim Einschieben setzt, @0x0203C ff.; Pult NICHT geloest):
  `[setflag-at] Frame 300: flag(5,0..2) = 1` -> Wegplan F404 `AOT slot 6 typ 0 ev 2 flags 0x31 band 3 mitte (5100,-11150)`
  (sub01 @0x0201A hat den Hebel auf Ereignis 2 gelegt) -> zu Fuss in die Nische (4243,-11159), Aktion F490 ->
  **`[ventilkette41] ROOM2050 Ereignis 2 -> Port-Programm 2050_hebel_sperre`** -> `[msgtext] room=2050 id=40 "The pump has
  no power. The switchboard must be set first."`; 0x `Cut_chg(4)`, 0x `val=-17700`/`val=-1400`, Wegplan F794 `FLAG (3,109)
  = 0`, F795 `FLAG (9,106) = 0`. Abnahme 0 (`abn0/mess/kisten_hebel`) hatte hier sub02 + steigendes Wasser -> **M1 behoben.**
  (Danach greifen die Gegner des Raums Leon an, hp 20 -> -20 bis F999 — Raumzustand, kein Befund.)
* `hebel_vorher` (Ereignis 4, ohne Kisten): Aktion F299 in der Nische (4291,-11139) -> `ROOM2050 Ereignis 4 -> Port-Programm
  2050_hebel_sperre` -> `[msgtext] room=2050 id=40 ...`, `FLAG (3,104) = 0`.
* Riegel `_b` (mein ctest-Lauf `lauf_355fc649_*`): E2 `ROOM2050 Ereignis 2 -> ... 2050_hebel_sperre` + Text 40, EXIT F520;
  E3 (Bit 106) `[msgtext] room=2050 id=0 "A lever to fill and drain water. Will you move it?"`, `[prop-member F456] prop[2]
  obj=0x01 m=0 → val=-17700` (auch obj 2/3), `[scd F477] Cut_chg(4)`, `[prop-member F657] prop[6] obj=0x05 m=1 →
  val=-5000` = Kisten-Raetsel nach dem Pult weiter loesbar; E4 Elza `loaded room2051.rdt` + `ROOM2051 Ereignis 2 -> ...
  2050_hebel_sperre` + `[msgtext] room=2051 id=40 ...`.
* `pult` (Eingabe der Abnahme 0 = echte Steuerkreuz-/Viereck-Bits, Start vor dem Pult (4900,-7100); Bildbogen
  `<a1>/pult2050_sheet.png`): `Ereignis 9 -> Port-Programm 2050_pult_ein` -> `id=43 "A high voltage switchboard. The pump
  needs 80 volts."` -> `id=41 "Will you push them?"` -> `[scd F534] Cut_chg(15)` -> `Hebel 3 -> EIN, Schalter 0x4, 40 V` ->
  `Hebel 1 -> EIN, Schalter 0x5, 70 V` -> `Hebel 4 -> EIN, Schalter 0xD, 80 V` -> `id=42 "Power supply OK."` -> `GELOEST:
  80 V -> Bit 106` -> `verlassen`. Bild: Zeiger F545 bei 0, F605 bei 40, F680 bei 70, F770 bei 80 (rotes Band), gruene
  Marken unter den EIN-Hebeln, F830 "Power supply OK.", danach wieder Raumbild.
* `hebel_nachher` (`RE15_SET_FLAG=9:106`, zu Fuss in die Nische): Original sub04 `[msgtext] room=2050 id=0 "A lever to fill
  and drain water. Will you move it?"` -> `[prop-member F454] prop[5] obj=0x04 m=1 → val=-1400` -> `Cut_chg(4)` ->
  `[prop-member F654] prop[6] obj=0x05 m=1 → val=-5000` -> `Cut_chg(7)`, Wegplan `FLAG (3,104) = 1` = das Wasser steigt erst
  nach dem Pult. Elza 2051: Riegel `_b` J (D mit Elza bis "Power supply OK." + Original-Hebel) gruen.

### L3o — Punkt 3 Ost-Tuer zu Fuss OHNE den Spur-A-Haken (Spurbaum-exe, `<a1>/mess/osttuer`)
Aufbau wie Abnahme 0 L3 (Ventile -> Ja -> Warnung -> UMDREHEN -> Aktionstaste an der Ost-Tuer, kein FIRE_AOT),
`RE15_EXIT_AT=1300#20B0`: `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11` -> `loaded room20b0.rdt`
-> `Cut_chg(11)` -> `Bits 103 + 100` -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet) -> Sperrtext 41` ->
`Plc_dest(... -15920,-6355)` -> **kein** `first_op=0x46`, kein `letterbox closed`; state.txt F109..F1299
`PL(-15820,-6355,rot=2047) pst=4 ... cam=14 ... pm=2` = die Szene haengt im Spurbaum weiter (wie Abnahme 0). Mit dem
Haken der Spur A laeuft sie durch (L3k). Das ist die Integrationsabhaengigkeit, die Entscheid C-E1 der Spur A zuweist.

### L4 — Punkt 4 nach der Flut: 20B0 statt 2000, Leiter frei, kein Strahl (`<a1>/mess/nachflut`)
Zustand "Szene gelaufen" per `RE15_SET_FLAG=3:235,9:103,9:100` (die Szene selbst: L3k), danach ALLES zu Fuss/Aktionstaste
(Wegplan der Abnahme 0 `nachflut.txt`): `ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> `[aot] DOOR FIRE slot=0
rect=(-5300,-4925,hw=500,hh=875) target_cut=10` -> `loaded room20b0.rdt` (nicht 2000) -> `ROOM20B0: Ring-Tuer Slot 3 ...
gesperrt` -> Leiter `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` ->
`loaded room2050.rdt` (kein Sperrtext) -> `ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel ROOM2000)` -> `DOOR FIRE slot=0
rect=(9100,5700,...) target_cut=9` -> `loaded room20b0.rdt`; state.txt `fx=0` in allen 1423 Zeilen (keine Strahlen-Effekte in
20B0 mit Bit 100; in 2000 ohne Flut stehen `fx=23..25`, L1). In L3k (Kette mit Haken) dasselbe nach der echten Szene: Leiter
ohne Sperrtext -> 2050.

### L5b — M5 / Entscheid C-E2: Ring-Zugaenge des gefluteten 2030 (`<a1>/mess/ring20b0`, `ring2070a`, `ring2070b`)
* `ring20b0` (Bits 3:235/103/100, vor der Ost-Tuer (4900,-6400) Blick +x, Aktionstaste): `ROOM20B0: Ring-Tuer Slot 3 nach 2030
  gesperrt (geflutet) -> Sperrtext 41` -> `[msgtext] room=20b0 id=41 "I cannot open the door because of the water pressure."`,
  kein `DOOR FIRE`, `EXIT_AT: Bild 560 in Raum 20b0` (kein Raumwechsel).
* `ring2070a` (Bits 103/100, (4400,-6400) Blick +x): Slot 1 und Slot 2 gesperrt (Log), `[msgtext] room=2070 id=40 "I cannot
  open the door because of the water pressure."`, kein Raumwechsel; `ring2070b` (West-Wand (-18400,6400) Blick -x = Slot 2
  @0x00A7A): derselbe Satz, kein Raumwechsel.
* Gegenproben mit Bit 107 im Riegel `_b` L2/M2 (mein ctest-Lauf): `[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500)
  target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` bzw. `DOOR FIRE slot=1 rect=(5500,-6400,...)` -> 2030.
* Tuer-Zensus SELBST nachgefahren (`<a1>/py/tuer_zensus.py` ueber alle 240 ROOM*.RDT, `<a1>/tuer_zensus.txt`): 14 Tueren nach
  STAGE2 Raum 3; auf den Ring (6550,-5400,-100) nfloor 3 fuehren genau 20B0/20B1 Slot 3 @0x019C2 und 2070/2071 Slot 1 @0x00A5A
  / Slot 2 @0x00A7A; alle anderen (2020/2021 Slot 1, 2060/2061 Slot 0, 2080/2081 Slot 0) kommen auf y -16200 nfloor 9 an —
  deckt sich mit dem Zensus des Bau-Agenten. Die Slots 1/2/3 werden nur in main00 gesetzt (kein spaeteres Aot_reset/
  Door_aot_set in 20B0/2070, scd_dump_room.py) -> die Sperre wird von keinem Skript ueberschrieben.

### L8 — M8 Ventile nach dem Entleeren (`<a1>/mess/ventile_leer`)
Bits 103/100/107 + Spinnen-Kill-Bits, vor den Ventilen, Aktionstaste: `ROOM2030 Ereignis 26 -> Port-Programm
2030_ventile_leer` -> `[msgtext] room=2030 id=49 "The valves are open. The pool has been drained."` -> **M8 behoben**.

### L9 — NEU: der "Weg zum Rechner nach der Flut" (Dossier §7 / Vorschlag f -> C-E2) GEMESSEN (`<a1>/mess/tuerebene3`, `weg20a0`, `weg2090`)
Dossier §7: "Nach der Flut sind die Tueren aus 20B0 (Ost) und 2070 in den Ring von 2030 zu ... der Weg zum Rechner fuehrt
ueber die Tuerebene (2050 -> 2020 -> 2030 oben -> 2080)"; derselbe Satz stand im Vorschlag (f), den Entscheid C-E2 angenommen
hat ("Der Weg zum Rechner bleibt offen (2050 -> 2020 -> 2030 Tuerebene -> 2080, ueber dem Wasser)").
* Lauf `tuerebene3` (Bits 103+100 = geflutet, Spinnen-Kill-Bits 7:155..158 — der 4. Spinnen-Platz Case 13 @0x01CA0
  `44 03 25 00 09 00 00 9e da e4 b8 c0 b2 da` steht auf der Tuerebene und blockierte die ersten zwei Laeufe `tuerebene`/
  `tuerebene2`; Wegplan `<a1>/plaene/tuerebene.txt`, Bildbogen `<a1>/tuerebene_sheet.png`): ROOM2020 vor Slot 1, Aktion ->
  `[msgtext] room=2020 id=2 "You've opened the lock."`, Aktion -> `[aot] DOOR FIRE slot=1 rect=(-14200,3300,...) target_cut=13
  spawn=(-450,-16200,-9700)` -> `loaded room2030.rdt` -> `ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1` ->
  AOTS: Slot 1 (Tuer nach 2080) Mitte (-7700,6400), Slot 6 (Lift B oben, Band 9) Typ 11 = Text, Slot 4 (Lift A oben, Band 6)
  Typ 11 -> Wegplan: F184 `(-6943,-9687) angekommen` (Nordgalerie nach Westen), F279 `(-6983,-4591) angekommen` (Westgalerie
  nach Sueden), F308 **`KLEMME beim Gehen nach (-7000,0): Rest 3918`**, F325 `KLEMME beim Gehen nach (-7000,6400): Rest
  10318`; state.txt F300..F2400 `PL(-6983,-3918)` = Ende der Galerie. Bild: Leon steht am Gelaenderende der Galerie
  (Cut 13).
* SCA ROOM2030 @0x880 (selbst gelesen, `abn0/py/sca.py`): die Tuerebene (Floor-Byte 0x89) besteht aus ZWEI getrennten
  Flaechen — Nord: Zelle 73 x -6350..500 z -10350..-9000 + Zelle 72 x -7700..-6350 z -10350..-3450 (Tueren 2020 Slot 2
  @0x01B2A/@0x01B12 und 2060 Slot 3 @0x01B4C), abgeschlossen von Wand 66 x -8200..-5850 z -3450..-2950; Sued: Zelle 74
  x -7700..-6350 z 4300..10000 + Zelle 75 x -10500..-6350 z 2950..4300 (Tuer 2080 Slot 1 @0x01AEA rect (-8200,5500,1000,1800)
  und Lift B oben, Slot 6 @0x01DC2 rect (-10550,2850,1500,1500) Band 9). Die Suedflaeche erreicht man nur mit Lift B vom Ring
  (Slot 7 @0x01DD6 Band 3) — der Ring ist geflutet und seit C-E2 von aussen gesperrt (L5b), der Lift oben ist Text.
* Die einzigen anderen Tueren nach 2080 (Zensus, `<a1>/py/tuer_zensus_2080.py`): ROOM2090/2091 Slot 0 @0x00996. Gemessen:
  `weg20a0` (Bits 103+100) ROOM20A0 Slot 0 @0x01746 -> `[aot] DOOR FIRE slot=0 rect=(4950,5000,...) target_cut=2` ->
  `loaded room2090.rdt`; `weg2090` ROOM2090 Slot 0 -> `[aot] DOOR FIRE slot=0 rect=(-8900,-25500,...) target_cut=4` ->
  `loaded room2080.rdt`. Ein Umweg ueber 20A0 -> 2090 (Alligator-Becken) -> 2080 hat also funktionierende Tueren; ob er als
  ganzer Weg begehbar und als Spielablauf gewollt ist (Alligator VOR dem Rechner, Zugang 20A0 ueber 11A0/2070/2040 mit
  Strom), ist NICHT gemessen — weder vom Bau-Agenten noch hier.
=> Die im Dossier (§7, Vorschlag f) behauptete und in C-E2 uebernommene Route existiert nicht (gemessen + SCA). Mangel N1.

### L6 — Punkt 6 Rechner 2080 leert 2030 (`<a1>/mess/rechner`, `<a1>/mess/nach2030`; Bildbogen `<a1>/rechner2080_sheet.png`)
* `rechner` (Bits 103/100/101/102, Sprung vor den Rechner, Aktionstaste per Wegplan der Abnahme 0): `ROOM2080 Ereignis 2 ->
  Port-Programm 2080_rechner_erw` -> `[msgtext] room=2080 id=0 "From here maybe I can control the water level.Will you operate
  the computer?"` -> Ja -> `[scd F344] Cut_chg(7)` (Bild F360..F630: das Wasser in 2080 sinkt, Wand und Gitter tauchen auf)
  -> `[scd F660] Cut_chg(3)` -> `[msgtext] room=2080 id=40 "The treatment pool is being drained as well."` (der Ja-Zweig ist
  ohne Verzweigung: `22 09 6b 01` Set(9,107) steht direkt vor `2b 28` msg 40, gen/ventilkette_r41_programme.inc).
* `nach2030` (Bits 103/100/101/102/107 + (3,245), an der 2080-Tuer, Aktionstaste): `[aot] DOOR FIRE slot=0
  rect=(-23900,-16700,hw=500,hh=1000) target_cut=9 spawn=(-6950,-16200,6450)` -> `loaded room2030.rdt` -> KEIN `unter
  Wasser`-Satz (keine Wasserflaeche), Wegplan `FLAG (9,107) = 1`; einzig `Tuer Slot 0 -> ROOM20B0 Cut 11` bleibt (Frage g).
  Gegenprobe ohne 107 im Riegel `_b` I gruen.
* Elza 2081: unit_r41_ventilkette G (Elza-Schleife) gruen.
* Erreichbarkeit des Rechners NACH der Flut: siehe L9 / Mangel N1 (die Messungen hier beginnen mit einem Sprung nach 2080).

### L7 — Elza (`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, OHNE `RE15_TITLE_SHOT`)
(Zwei erste Elza-Laeufe mit `RE15_TITLE_SHOT` luden Leon (`Spieler-Familie PL00`) und sind verworfen: `<a1>/mess/ring20b1`
/`h1171` der Serie 2 wurden mit Serie 5 ueberschrieben.)
* `kette_elza` (Messbau-exe MIT Spur-A-Haken, Wegplan `<a1>/plaene/kette_elza.txt`): `ROOM2031 Ereignis 26 -> 2030_ventile` ->
  `id=45` -> `id=46` -> `ROOM2031: Tuer Slot 0 -> ROOM20B0 Cut 11` -> `DOOR FIRE slot=0 ... target_cut=11` -> **`loaded
  room20b1.rdt`** -> `Bits 103 + 100` -> `ROOM20B1: Ring-Tuer Slot 3 ... gesperrt` -> `first_op=0x46` -> Cut 7/15/6 ->
  `room=20B1 WRITE Set zone=3 idx=235 op=1` -> `[scd F242] letterbox closed` -> Sprung F334 / Aufstieg F493 -> Leiter
  `DOOR FIRE slot=4 ...` -> **`loaded room2051.rdt`** -> `EXIT_AT: Bild 60 in Raum 2051`. Die Kette laeuft mit Elza gleich.
* `ring20b1` (Spurbaum-exe): `Spieler-Familie PL04` -> `loaded room20b1.rdt` -> `ROOM20B1: Ring-Tuer Slot 3 nach 2030 gesperrt
  (geflutet) -> Sperrtext 41` -> `[msgtext] room=20b1 id=41 "I cannot open the door because of the water pressure."`, kein
  Raumwechsel.
* `h1171` (M3; `RE15_SET_FLAG=3:194`, `RE15_DEBUG_JUMP=1170@250`, `RE15_FLAG_CENSUS=1`): `Spieler-Familie PL04` -> `loaded
  room1171.rdt` -> `[flagcensus] room=1171 WRITE Set zone=5 idx=30 op=1` (@0x013B2) und `idx=29 op=1` (@0x01404). Den
  Zustand NACH dem Installer belegt unit_r41_ventilkette H (`ROOM1171 (3,194): ... (5,0x1E) = 1 nach dem Installer`, PASS)
  und der Code (`re15_game_flag_set(5, 0x1D/0x1E, 0)` nur noch unter `if (ist(room_id, RE15_R41C_RAUM_2030))`,
  ventilkette_r41.c:362-367); Heliport-Riegel 5/5 gruen (§3).
* Riegel `_b`: J (Elza 2051 Pult + Original-Hebel), K (Elza 2001 Leiter, `[msgtext] room=2001 id=40 "I cannot climb up the
  ladder because of the water pressure."`), E4 (Elza 2051 Kisten-Hebel gesperrt) gruen; unit_r41_ventilkette `== Elza`
  (B-I) gruen.

## 2. Gates

### G1 RE-Gate (Stichproben SELBST disassembliert, re15_disasm.py auf info/Re1.5/PSX.EXE; RDT-Bytes selbst gelesen)
* M2 @0x8001ce1c `jal 0x800436a8` in der Folge @0x8001cdec..@0x8001ce34 ohne Verzweigung (`jal 0x8003f038`, `0x8004f090`,
  `0x8001500c`, `0x8001a50c`, `0x80031c44`, `0x8002bd44`, **`0x800436a8`**, `0x8004f0b0`, `0x80019e20`, `0x8001db28`);
  FUN_800436a8 @0x800436b4 `addiu s1,s1,-13740` (Spieler), @0x800436bc `ori a1,zero,0x1`, @0x800436cc `jal 0x80042bac` —
  wie im Dossier zitiert (keine Szenen-Abfrage davor). (Ob FUN_80042bac intern filtert, ist Sache der Spur A.)
* M3 @0x8003ed74 `sw zero,4136(at)` = 0x800b1028 (Bank 5 Wort 0 beim Raumladen), @0x8003ec1c `sw zero,4140(at)` =
  0x800b102c — wie zitiert.
* H1 @0x8002c234 `lbu v1,8(s1)`, @0x8002c238 `ori v0,zero,0x4`, @0x8002c23c `bne v1,v0,...`, @0x8002c244 `lw v0,96(s1)`,
  @0x8002c24c `addiu v0,v0,-900`, @0x8002c250 `sw v0,96(s1)` = Typ-4-Objekte; im Kopf jetzt als PORT-WAHL mit entliehenem
  Mass gekennzeichnet — korrekt.
* M4 Handler 0x56 @0x80042a58: @0x80042a70 `ori a3,zero,0x7` (OT-Fach 7), @0x80042a74 `lbu a0,2(s0)` (Byte 2) `sll a0,a0,8`
  | @0x80042a78 `lbu v0,1(s0)` (Byte 1 = Kanal), @0x80042a80 `lbu a2,3(s0)` (Byte 3 = Maske), `jal 0x800217b0`; dort
  @0x800217b8 `srl a2,a0,8` -> `sb a2,4(s0)` (ABR = Byte 2), Maske Bit 2 -> +5 (R), Bit 1 -> +6 (G), Bit 0 -> +7 (B) =
  "ABR = Byte 2, Maske Rot+Gruen = 0x6" stimmt. Spritzer @0x800456ac `andi v0,s0,0x80`, @0x800456b8/bc `lui a0,0x605`/
  `ori a0,a0,0xd00`, @0x80045710 `jal 0x80019700` — wie zitiert.
* RDT-Bytes (`<a1>` Python-Lesung): 2050 @0x02006 `06 00 1c 00`, @0x0200A `21 03 6d 00`, @0x0200E `21 05 00 01 21 05 01 01
  21 05 02 01`, **@0x0201A `46 06 03 31 ff 00 18 02 00 00`** (2051 gleich), @0x0220E `2b 00 ff ff`, @0x02218 `21 0c 1f 00`,
  @0x0221C `22 03 6d 01`, @0x02268 `34 01 00 00`, @0x02270 `29 04`, @0x0227C `2f 01 ec ff`, @0x02280 `0d 00 06 00 b4 00`;
  1171 @0x013B2 `22 05 1e 01`, @0x013C6 `09 0a 01 00`, @0x014A4 `22 05 1e 00 22 05 1d 00`; 20B0/20B1 @0x019C2, 2070/2071
  @0x00A5A/@0x00A7A (Ziel `01 03`, Ankunft `96 19 e8 ea 9c ff` = (6550,-5400,-100), nfloor 3); 20B0 @0x01DF2 `2c 07 03 41 01 00
  08 bc 04 de ca 08 f8 11 ff 00 18 05 00 00`, @0x01EF8 `22 02 07 01 22 01 1c 01` (= (2,7) und **(1,28)** — Korrektur des
  Bau-Agenten stimmt), @0x01FBC `22 03 eb 01 22 02 07 00 22 01 1b 00`, @0x01AAA `21 03 eb 00` — alles wie zitiert.
* Generator nachgefahren: gen/ventilkette_r41_{texte,original,programme,wasser}.inc byte-gleich (§0).
* Diff-Suche (`git diff 116e8b0f HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
  TODO/FIXME/approx/ungefaehr/geschaetzt: **0 Treffer**; kein neues `getenv` in engine/include/platform.
* Neue Konstanten der Nachbesserung: `RE15_R41C_EV_2050_KISTEN 2` (@0x0201A), `RE15_R41C_SLOT_20B0_RING 3` (@0x019C2),
  `_2070_RING_A/B 1/2` (@0x00A5A/@0x00A7A), `RE15_R41C_RAUM_2030_ID 0x03` (Door_aot_set pc[23]), `MSG_RING_20B0 41` /
  `MSG_RING_2070 40` / `MSG_VENTILE_LEER 49` (VERTRAG §1.2), Texte PORT-WAHL; Riegel-Schwellen C (Rot/Gruen >= 15 vorher,
  <= 4 / Blau >= 20 nachher) aus der Messung abgeleitet und im Kopf begruendet.
=> **RE-Gate haelt.** ABER: die Wege-Behauptung des Dossiers (§7 / Vorschlag f) war ungemessen und ist falsch (L9, N1) —
"vor fertig fuer JEDE Behauptung den Beleg zeigen" ist dort verletzt.

### G2 Vertrag
* Bank 9: Schreiber nur 101/102/107 (Programme `22 09 65/66/6b 01`) und 100/103/106 (ventilkette_r41.c `bit_setzen`) =
  Zuteilung C; k_port[] traegt alle sechs; unit_r36_kartenbits gruen.
* Text-Ids: 2000/20B0 40, 2030 40-49, 2050 40-44 (+41/42 = 11F0-Bytes), 2080 40, 20B0/20B1 41, 2070/2071 40 = VERTRAG §1.2
  (inkl. C-E2-Ergaenzung). Ereignisse 2030 24-26 / Plaetze 12-14 unveraendert.
* Pfade (`git diff 116e8b0f HEAD --name-only`, master seit der Basis nur analysis/): keine release/, platform/android/,
  shared_assets/PSX/, pri/, synchro/, keine CMakeLists.txt. Gemeinsame Dateien: scd_room_setup.c (+2), scd_vm.c (+4),
  test_r36_kartenbits.c (+7) zugeteilt; main.c (+11), game_step_common.c (+1), test_r36_dachkiste.cmake (+2),
  test_r37_itemband.cmake (+2/-1) per **C-E3 angenommen**.
* Tests je Punkt: unit A-I/D2, integration A/B/C, `_b` D/E/E2/E3/E4/F/G/H/I/J/K/L/L2/M/M2, `_kette` (nur mit Spur A
  registriert, C-E1: Configure-Zeile Z. 180 vorhanden, `ctest -N` im Spurbaum 3 Tests, im Messbau mit Haken 4) — alle messend
  (Log-Saetze, Bits, Takte, Farbe). Kein Riegel fuer die Erreichbarkeit des Rechners nach der Flut (N1).
* Parallel-/Linux-Festigkeit: eigene exe-Praefixe (re15_pc_r41c_haken_/r41cb_/r41ck_), RUN_SERIAL, `cmake_minimum_required`,
  `.exe` per MATCHES. Hinweis: Riegel C verlangt jetzt `ABR2 (B-F) ok` (beschleunigter Renderer); unter Linux nur ueber den
  opengl-Weg seit 81ee1efc moeglich — nicht hier gemessen (Windows), faellt dort laut statt still.

### G3 Nebenwirkungen
* M3 behoben: Marker-Loeschen nur in 2030/2031 (Code + unit H + Heliport-Riegel 5/5).
* M1-Sperre trifft nur Ereignis 2/4 aus dem Hebel-Platz: ROOM2050 hat kein Evt_exec auf sub02/sub04 (scd_dump_room.py: Evt_exec
  nur 05/06/07/08/0a/03) -> kein Skript-Aufruf wird abgefangen.
* 2070 bekommt jetzt immer den Port-Text Id 40 in seine Tabelle (texte_einsetzen in install_2070) — 2070 hat nur msg 0; ohne
  Folge.
* **Erreichbarkeit des Rechners 2080 nach der Flut** (L9): durch die Ring-Sperre (C-E2) + Lift-Sperre oben ist die Suedgalerie
  von 2030 (einzige 2030-Tuer nach 2080) nicht mehr erreichbar; die behauptete Ersatzroute ueber die Nordgalerie endet an der
  Wand z -3450. -> N1.

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe 164 Tests (Regex `<a1>/regex_unit.txt`): `100% tests passed, 0 tests failed out of 164` (26.81 s);
  `test_r41_ventilkette.exe`: `PASS (274 Pruefungen)`, Leon + Elza.
* `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b)?$" --timeout 1500 --output-on-failure`
  (07:06:42-07:18:49, exe 06:37): `1/2 Test #1126: integration_r41_ventilkette ......   Passed  222.08 sec`,
  `2/2 Test #1127: integration_r41_ventilkette_b ....   Passed  504.60 sec`, `100% tests passed, 0 tests failed out of 2`
  (`<a1>/ctest_integ_r41.txt`, EXIT=0). Unter Parallel-Last (andere Spuren + eigene Messlaeufe) ohne Ausfall.
* `ctest -R "^integration_r3[89]_heliport1171_.*$" --timeout 900` (Querbezug M3): weg_re2 72.88 s, weg_re15 70.16 s, laden
  54.86 s, r39 weg 77.15 s, r39 laden 56.47 s -> `100% tests passed, 0 tests failed out of 5` (`<a1>/ctest_heliport.txt`).
* `integration_r41_ventilkette_kette`: im Spurbaum nicht registriert (Configure Z. 180 "uebersprungen: Spur A fehlt") =
  C-E1; im Messbau mit dem wortgleichen Spur-A-Haken `Passed 92.56 sec` (L3k).
* Nicht erneut gefahren (Abnahme 0: Querbezug 8/8 gruen): integration_r36_dachkiste / r37_itemband / r37_kistenregel.
  Begruendung: die Nachbesserung aendert nur 2050 Ereignis 2 (diese Riegel bedienen den 2050-Hebel nicht, grep), Ring-Tueren
  mit Bit 103 (setzt keiner dieser Riegel), Text 49 und das Marker-Loeschen ausserhalb 2030 (weniger Wirkung als vorher).

## 4. Urteil je Nutzer-Punkt (gegen den Wortlaut; gebauter Stand exe 06:37 = HEAD 5ddd98c0)

| Punkt | Urteil | Beleg (Messung) |
|---|---|---|
| 1 Leiter 2000 mit Wasserstrom blockiert, "I cannot climb up the ladder because of the water pressure", bis die Flut lief | **erfuellt** | L1 (zu Fuss + Aktionstaste: Text 40 woertlich, kein Raumwechsel, Strahl am Gitter `fx=23`); Elza 2001 `_b` K; nach der Flut frei (L3k, L4) |
| 2 Bruecke oben, Pult links (Kamera 2 Nordwand): zu wenig Druck / Ventil 13 zu -> Ventil 13 -> "Lower the bridge?" -> Bruecke faehrt herunter, nicht die Kurbel | **erfuellt** | L2a/L2b (ganze Kette zu Fuss: Pult Text 40, Steg gesperrt z -6418, Ventil 13 Text 41/42 + Bit 101, Pult Text 43 -> Cut 4 -> Planke -1800 -> Bit 102 -> Text 44, Steg begehbar bis (-1043,2989)); Pult-Lage Cut 2 = Abnahme 0 L9 (Code unveraendert) |
| 3 Ventile -> Warnung (1 min, Raum verlassen) -> 60 s -> Wasser ueber die Ringebene; Ost-Tuer -> 20B0 (Auswasch-Szene des Originals); Zeit abgelaufen im Raum: Tod mit Ertrink-Effekt | **erfuellt** (mit dem Spur-A-Haken, Entscheid C-E1) | L3b (Warnung woertlich, 1801 Bilder, Flut ueber den Ring, Spritzer, Tod +125 Bilder, Blende (0,0,30) im beschleunigten Renderer); L3k (Leon) + L7 (Elza): Ost-Tuer zu Fuss -> 20B0/20B1 -> sub02/sub05 -> (3,235)=1 -> frei, in EINEM Lauf mit dem wortgleichen Haken; ohne Haken haengt die Szene im Spurbaum (L3o) = Spur-A-Abhaengigkeit, laut C-E1 im Integrationsbaum A+C zu messen |
| 4 Danach 20B0 statt 2000 (spaeter wieder frei ohne Wasser), Leiter frei (kein Wasser laeuft mehr herunter) | **erfuellt** | L4 (2010-Tuer -> 20B0 Cut 10, 2050-Leiter -> 20B0 Cut 9, 20B0-Leiter -> 2050 ohne Sperrtext, `fx=0` in allen Bildern), L3k/L7 (nach der echten Szene: keine Wasserwand, Leiter -> 2050/2051) |
| 5 2050: Wasserlevel (Hebel) erst nach dem Pult-Raetsel (80 V wie 11F0) | **erfuellt** | L5: Hebel Ereignis 4 UND (M1, neu) Ereignis 2 mit den Kisten im Becken ohne Bit 106 = Text 40, kein Cut 4, kein Member_set, (3,109)=0; Pult per Steuerkreuz 40/70/80 V -> Bit 106 -> Original sub04 hebt das Wasser; mit Bit 106 bleibt das Kisten-Raetsel (sub02) loesbar (`_b` E3); Elza `_b` J/E4 |
| 6 2080: Rechner senkt 2080 und entleert danach auch 2030 (Raum wieder frei ohne Wasser) | **teilweise** | Mechanik erfuellt: L6 (Rechner -> Original-Frage -> Cut 7 Wasser sinkt -> Text 40 -> Bit 107; 2030 danach ohne Wasserflaeche; Ventile Text 49). ABER der Weg zum Rechner NACH der Flut ist nicht belegt: die im Dossier §7 angegebene Route "2050 -> 2020 -> 2030 oben -> 2080" endet an der Wand z -3450 (L9, gemessen + SCA); die Suedgalerie mit der 2080-Tuer erreicht man nur per Lift B vom gefluteten, seit C-E2 gesperrten Ring; der einzige Ersatz (20A0 -> 2090 -> 2080) ist als Weg ungemessen (Mangel N1) |

## 5. Maengel (nummeriert, nachpruefbar)
Abnahme-0-Maengel: **M1 behoben** (L5 `kisten_hebel`, `_b` E2/E3/E4, unit F); **M2 erfuellt im Sinne von C-E1** (Riegel
`_kette` existiert, im Spurbaum sauber uebersprungen, Ablauf im Dossier §0b; mit dem wortgleichen Haken unabhaengig gruen,
L3k/L7); **M3 behoben** (Code, unit H, Heliport 5/5); **M4 behoben** (Riegel C misst die Farbe im beschleunigten Renderer;
eigene Messung (0,0,30)); **M5 umgesetzt nach C-E2** (L5b, Elza 20B1, Zensus bestaetigt) — mit der Folge N1; **M6 per C-E3
angenommen**; **M7 korrigiert** (Dossier; Frage g offen); **M8 behoben** (L8); **M9 behoben** (Kopf des Unit-Riegels);
H1 korrigiert; H2/H3 offen (Fragen/Integration).

* **N1 (hoch, Punkt 6 + Kette nach der Flut, neu; Folge von C-E2 + ungemessener Behauptung)** Nach der Flut ist der Weg
  zum Rechner 2080 nicht belegt, und der dokumentierte Weg existiert nicht.
  - Behauptung: Dossier §7 "der Weg zum Rechner fuehrt ueber die Tuerebene (2050 -> 2020 -> 2030 oben -> 2080)" und
    "Zum Rechner 2080 geht es ueber 2050 -> 2020 -> die Tuerebene von 2030 (ueber dem Wasser) -> 2080"; Vorschlag (f), den
    C-E2 uebernommen hat ("Der Weg zum Rechner bleibt offen (2050 -> 2020 -> 2030 Tuerebene -> 2080, ueber dem Wasser)").
  - Messung (L9, `<a1>/mess/tuerebene3`, Bildbogen `<a1>/tuerebene_sheet.png`): 2020 Slot 1 -> 2030 (-450,-16200,-9700) ->
    zu Fuss West F184 (-6943,-9687) -> Sued F279 (-6983,-4591) -> `KLEMME beim Gehen nach (-7000,0): Rest 3918` /
    `(-7000,6400): Rest 10318`, Leon steht ab F300 bei (-6983,-3918) am Galerieende.
  - Beleg: SCA ROOM2030 @0x880 — Nordgalerie (Zellen 72/73, Floor 0x89; Tueren 2020 Slot 2, 2060 Slot 3) endet an Wand 66
    (x -8200..-5850, z -3450..-2950); Suedgalerie (Zellen 74/75; Tuer 2080 Slot 1 @0x01AEA, Lift B oben Slot 6 @0x01DC2) ist
    nur ueber Lift B vom Ring (Slot 7 @0x01DD6, Band 3) erreichbar; der Ring ist geflutet und seit C-E2 von aussen gesperrt
    (20B0 Slot 3, 2070 Slots 1/2 — die einzigen Ring-Tueren, Zensus), Lift oben = Text. Einzige andere Tuer nach 2080:
    2090 Slot 0 (Zensus) — die Tueren 20A0 -> 2090 -> 2080 feuern (gemessen `weg20a0`, `weg2090`); der Zugang
    20A0 <- 11A0 <- 2070 <- 2040 (Aufzug; Nutzer: "benoetigt der Strom") und der Weg durch das Alligator-Becken sind nicht
    gemessen.
  - Folge: Ob das Spiel nach der Flut zum Rechner (und damit zu "2030 wieder frei" und 2080/2090) weitergeht, ist offen; der
    Nutzer bekommt im Dossier eine Route genannt, an der er stecken bleibt.
  - Forderung: den tatsaechlichen Weg von 20B0/2050 zum Rechner nach der Flut (Bits 103+100, ohne 107) zu Fuss messen, Leon
    + Elza, mit Riegel; existiert keiner ohne den Ring, Entscheid C-E2 dem Orchestrator mit diesem Befund zur Revision
    vorlegen (z. B. eine Ring-Tuer oder Lift B nach der Flut nutzbar lassen); Dossier §7/§0b M5 korrigieren.

Hinweise (kein Mangel): Frage g (2030 Ost-Tuer nach Bit 107 weiter -> 20B0) und h (Spritzer ausserhalb Cut 4) bleiben
Nutzer-/Orchestrator-Fragen; H3 PSX-Ziel ungebaut; der Software-Renderer zeichnet ABR-2-Flaechen deckend (Spur D, Riegel C
misst deshalb im beschleunigten Renderer — unter Linux nur ueber opengl seit 81ee1efc, hier nicht gemessen); Port-Texte
(20B0 41, 2070 40, 2030 49 neu) sind Vorschlag bis zur Freigabe.

## 6. Fazit
**Nicht bestanden.** Die Nachbesserung 1 hat alle Abnahme-0-Maengel erledigt, und das ist unabhaengig gemessen: Kisten-Hebel
gesperrt (M1), die ganze Kette laeuft mit dem wortgleichen Spur-A-Haken in EINEM Lauf (M2, Leon und Elza; ohne Haken haengt
die Szene erwartungsgemaess, C-E1), Bank-5-Marker nur in 2030 (M3), Blendenfarbe im Riegel gemessen (M4), Ring-Sperre nach
C-E2 (M5), Text 49 (M8). Punkte 1-5 sind am gebauten Stand erfuellt (Punkt 3 mit der Spur-A-Abhaengigkeit laut C-E1);
RE-Gate und Vertrag halten; alle gefahrenen Riegel gruen (Unit 164/164, r41-Integration 2/2, Heliport 5/5, Kette im Messbau
1/1). Offen ist N1: der im Dossier genannte und in C-E2 vorausgesetzte Weg zum Rechner ueber die Tuerebene von 2030 existiert
nicht (gemessen); mit der Ring-Sperre ist die 2080-Tuer von 2030 aus nach der Flut unerreichbar, und ein anderer Weg ist nicht
belegt. Damit ist Punkt 6 ("danach auch 2030 frei") im Spielablauf nach der Flut nicht nachgewiesen.
