# Runde 41 — Spur C "ventilkette" — Abnahme 2 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, HEAD 1498d1c8 (Basis master 116e8b0f; master-Tip
d370db3c aendert seit der Basis nur analysis/ — darunter NEU der Orchestrator-Entscheid **C-E2c** 09:00, siehe §5).
Begonnen 2026-10-24 09:05. Pruefer: Abnahme 2 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner: scratchpad
`C41abn2/` (Pfade unten abgekuerzt als `<a2>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn2`).
Vorlage: Abnahme 1 (`C_abnahme_1.md`, Mangel N1), Nachbesserung 2 (Dossier §0c), Entscheid C-E2b.

## 0. Stand / Protokoll (neueste zuerst)
* 09:35 Bericht abgeschlossen (§2-§6). Aufraeumen: keine exe-Kopien `re15_pc_abn2c_*` mehr in `build/platform/pc`
  (die zwei Kopien `re15_pc_r36e_haken_*`/`re15_pc_r37d_lift_*` dort stammen vom 23.10. 22:48/23:05, nicht von mir), Baum:
  nur dieser Bericht neu (`git status --porcelain`).
* 09:29 r41-Integrationsriegel 3/3 gruen (integration_r41_ventilkette 216.67 s, _b 507.28 s, _weg 460.13 s; §3).
* 09:13-09:28 Messserien 1-3 an der exe (§1): Serie 1 Leiter Leon/Elza, Pult ohne Ventil 13, Bruecke, Flut/Tod, Ost-Tuer;
  Serie 2 nach der Flut, Kisten-Hebel, Hebel vorher, Pult 80 V, Hebel nachher, Rechner, 2030 nach dem Rechner; Serie 3 Weg
  nach der Flut Leon + Elza bis zur 11A0-Sperre, Gegenprobe (3,206), Rechner mit Flaggen-Zensus. Alle Laeufe rc=0, eigene
  exe-Kopien je Lauf, nie per Bildname beendet.
* 09:03-09:35 Statik (§1 L10, §2 G1): eigener Tuer-/Bit-Zensus + ROH-Scan, Erreichbarkeit vor der Flut, SCA 2070, Lift-SCD
  2030, Byte-Nachlese aller zitierten RDT-Offsets, drei EXE-Funktionen selbst disassembliert (FUN_800420a0, LAB_8003fdd0,
  FUN_8003f038). Neu auf master gefunden: Orchestrator-Entscheid C-E2c (09:00, j1) — nicht im Zweig, s. §5.
* 09:09 Integrationsriegel `^integration_r41_ventilkette(_b|_weg)?$` im Hintergrund gestartet (Ergebnis §3).
* 09:08 Unit-Gruppe (Regex `<a2>/regex_unit.txt` = Abnahme-1-Regex, 164 Tests): `100% tests passed, 0 tests failed out of 164`
  (44.54 s, `<a2>/ctest_unit.txt`). `test_r41_ventilkette.exe` direkt: `== Leon` ... `ROOM11A0 11A0 zu msgs: 2@Cut0`,
  `== Elza` ... `ROOM11A1 11A0 zu msgs: 2@Cut0`, `J1 Zensus: 206 Raeume, 16 Funde`, `PASS (286 Pruefungen)` (`<a2>/unit_r41_v.txt`).
* 09:07 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, Zeile 180 `-- integration_r41_ventilkette_kette
  uebersprungen: Spur A fehlt (...)` (C-E1); `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  exe 06:37:09 (7799077 B) = dieselbe exe wie Abnahme 1; `git diff 5ddd98c0 HEAD --name-only -- re15_port/engine
  re15_port/include re15_port/platform` = leer (Nachbesserung 2 aendert nur tests/, tools/, analysis/); `git status
  --porcelain` = leer.

## 1. Messungen an der exe (eigene Laeufe; Runner `<a2>/lauf.sh`, exe-Kopie je Lauf `re15_pc_abn2c_<lauf>.exe`, danach
geloescht; erbt keine RE15_-Variablen; Arbeitsordner `<a2>/mess/<lauf>/` mit debug.log/state.txt/wegplan.txt; Serien
`<a2>/serie1.sh` .. `serie3.sh`; Wegplaene `<a2>/plaene/` — fuer P1-P6 die Plaene der Abnahme 1 unveraendert, fuer den Weg
nach der Flut eigene Plaene `weg_leon.txt`/`weg_elza.txt`/`g206.txt`)

### L1 — Punkt 1 Leiter 2000 (Leon `leiter`, Elza `leiter_elza`; Bildbogen `<a2>/leiter2000_sheet.png`)
Aufbau: `RE15_DEBUG_JUMP=2000@240`, `RE15_PLAYER_POS=-24500,10300,2048`, `RE15_FORCE_CUT=10` (nur Bildwahl), Eingabe
`U0.8,W0.5,A0.1,W5,A0.1,W3` (hingehen, Aktionstaste), beschleunigter Renderer.
* Leon debug.log: `[render] SDL-Renderer direct3d, Videotreiber windows, ABR2 (B-F) ok` -> `[room] PC loaded room2000.rdt` ->
  `[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40 (Form ROOM2030 @0x01B12)` -> `[msgtext] room=2000 id=40
  "I cannot climb up the ladder because of the water pressure."` -> `[flow] EXIT_AT: Bild 660 in Raum 2000 erreicht` (kein
  Raumwechsel). state.txt F350 `PL(-26532,10332,rot=2048)` an der Leiterwand, `fx=23` (Strahlen-Effekte aktiv).
* Bild: Wasserstrahl aus dem Gitterfenster an der Leiter (Original-ESP), Leon unten an der Leiter, F420/F480 der Satz
  zweizeilig, F540-F660 Leon bleibt unten.
* Elza: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)` -> `loaded room2001.rdt` -> `[ventilkette41] ROOM2001: Leiter
  Slot 4 ...` -> `[msgtext] room=2001 id=40 "I cannot climb up the ladder because of the water pressure."` -> `EXIT_AT: Bild
  700 in Raum 2001` (kein Raumwechsel).

### L4 — Punkt 4 nach der Flut (`nachflut`; Zustand "Szene gelaufen" `RE15_SET_FLAG=3:235,9:103,9:100`, sonst zu Fuss)
* `[ventilkette41] ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> `[aot] DOOR FIRE slot=0 rect=(-5300,-4925,
  hw=500,hh=875) target_cut=10` -> `loaded room20b0.rdt` (nicht 2000) -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt
  (geflutet) -> Sperrtext 41` -> Leiter `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0
  spawn=(9100,-5400,4100)` -> `loaded room2050.rdt` (kein Sperrtext) -> `ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9` ->
  `DOOR FIRE slot=0 rect=(9100,5700,...) target_cut=9` -> `loaded room20b0.rdt` -> `EXIT_AT: Bild 700 in Raum 20b0`.
* state.txt: `fx=0` in allen 1423 Zeilen (kein Wasserstrahl in 20B0 nach der Flut; vor der Flut in 2000 `fx=23`, L1).

### L2 — Punkt 2 Bruecke (`pult_ohne`, `bruecke3`; Bildbogen `<a2>/pult2030_sheet.png`)
* `pult_ohne` (Start Ost-Tuer (6550,-100) Band 3, Spinnen-Kill-Bits 7:155..157, Wegplan `bruecke.txt` = Plan der
  Abnahme 0/1, echte Pad-Bits + Kollision): `[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt` ->
  Ring Nord bis vor das Pult F334 (490,-9718) Blick 1072, cam 1 -> `[ventilkette41] ROOM2030 Ereignis 24 -> Port-Programm
  2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the bridge. The pressure gauge reads zero.Intake valve
  13 is closed."`, Wegplan F673 `FLAG (9,101) = 0`; Treppe (-6356,-7726) Aktion F885 -> hinab -> Landeplatz (-1107,-7717)
  F1210 -> Versuch auf den Steg: state.txt F1250..F1800 `PL(-939..-825,-6418)` = haengt an der Steg-Zelle 12 (Bruecke oben).
  Bild: der Schaltschrank des Nutzerbilds `schaltschrank.png` (Manometer, blauer Kasten) mit Leon davor, Text F360/F420;
  F1260/F1500 Leon am Nordende des Stegs, kommt nicht weiter.
* `bruecke3`: siehe unten (Ventil 13 -> Pult -> Fahrt -> Steg).

### L5 — Punkt 5 ROOM2050 (`kisten_hebel`, `hebel_vorher`, `pult`, `hebel_nachher`)
* `kisten_hebel` (Kisten-Bits wie sub01 `RE15_SET_FLAG_AT=5:0,5:1,5:2@300`, Pult NICHT geloest, Wegplan `kisten_hebel2.txt`):
  `[ventilkette41] ROOM2050 Ereignis 2 -> Port-Programm 2050_hebel_sperre` -> `[msgtext] room=2050 id=40 "The pump has no
  power. The switchboard must be set first."`; kein `Cut_chg(4)`, Wegplan F794 `FLAG (3,109) = 0`, F795 `FLAG (9,106) = 0`.
* `hebel_vorher` (Ereignis 4, ohne Kisten): `ROOM2050 Ereignis 4 -> Port-Programm 2050_hebel_sperre` -> `[msgtext]
  room=2050 id=40 ...`, Wegplan F513 `FLAG (3,104) = 0` (Wasser steigt nicht), `EXIT_AT: Bild 900 in Raum 2050`.
* `pult` (Steuerkreuz-/Viereck-Bits, Start vor dem Pult (4900,-7100)): `ROOM2050 Ereignis 9 -> Port-Programm 2050_pult_ein`
  -> `id=43 "A high voltage switchboard. The pump needs 80 volts."` -> `id=41 "Will you push them?"` -> `[scd F534]
  Cut_chg(15)` -> `Hebel 3 -> EIN, Schalter 0x4, 40 V` -> `Hebel 1 -> EIN, Schalter 0x5, 70 V` -> `Hebel 4 -> EIN, Schalter
  0xD, 80 V` -> `id=42 "Power supply OK."` -> `Pult 2050 GELOEST: 80 V -> Bit 106` -> `Pult 2050 verlassen`.
* `hebel_nachher` (`RE15_SET_FLAG=9:106`, zu Fuss in die Nische): Original sub04 `[msgtext] room=2050 id=0 "A lever to fill
  and drain water. Will you move it?"` -> `[prop-member F454] prop[5] obj=0x04 m=1 → val=-1400` -> `[scd F474] Cut_chg(4)`
  -> `[prop-member F654] prop[6] obj=0x05 m=1 → val=-5000` -> `Cut_chg(7)`, Wegplan F1068 `FLAG (3,104) = 1` = das Wasser
  steigt erst nach dem Pult.
* Bild Pult 2050 (`<a2>/pult2050_sheet.png`): Original-Bild des Schaltpults (DANGER High voltage, Skala mit roter 80),
  Zeiger F545 bei 0, F605 bei 40, F680 bei 70, F770 bei 80 (rot), gruene Marken unter den EIN-Hebeln, F830 "Power supply OK.",
  danach Raumbild 2050.

### L2 (Fortsetzung) — `bruecke3` (Start Landeplatz (-1107,-7717) Band 1, Wegplan `bruecke3.txt`; Bildbogen `<a2>/bruecke3_sheet.png`)
`ROOM2030 Ereignis 25 -> Port-Programm 2030_v13_frage` -> `[msgtext] room=2030 id=41 "Intake valve 13. Will you open it?"` ->
Ja -> `id=42 "You've opened valve 13. The pressure is rising."` -> Wegplan F493 `FLAG (9,101) = 1` -> Treppe hinauf -> Ring
zum Pult F1043 (236,-9636) -> `Ereignis 24 -> Port-Programm 2030_pult_senken` -> `id=43 "The pressure gauge reads 80. Lower
the bridge?"` -> Ja -> `[scd F1221] Cut_chg(4)` -> `[prop-member F1281] prop[0] obj=0x00 m=1 → val=-1800` -> `[scd F1315]
letterbox closed` -> F1555 `FLAG (9,102) = 1` -> `2030_pult_unten` -> `id=44 "The bridge is down."` -> Treppe hinab ->
Landeplatz F2257 (-1107,-7712) -> ueber den Steg F2434 (-1043,2989) `angekommen` (Steg-Zelle 12 offen; vorher in `pult_ohne`
bei z -6418 gesperrt). Bild: Text 42 (F420), Frage 43 (F1095), Draufsicht Cut 4 mit sinkender Planke (F1230/F1260/F1290),
Text 44 (F1575). Ausserhalb von Cut 4 bleibt die Kamera hier bei Cut 0 (cam 0 in 3981 Zeilen): Start per PLAYER_POS auf dem
Landeplatz ausserhalb der Zonen des Start-Schnitts — derselbe Messartefakt wie Abnahme 0/1; im Lauf `pult_ohne` (Start an der
Ost-Tuer) schaltet die Kamera normal 0 -> 1 -> 2 -> 3.

### L6 — Punkt 6 Rechner 2080 (`rechner`, `nach2030`; Bildbogen `<a2>/rechner2080_sheet.png`)
* `rechner` (Bits 103/100/101/102, Sprung vor den Rechner (-11185,-7206) Blick 3072 Band 8, Wegplan `rechner.txt`):
  `ROOM2080 Ereignis 2 -> Port-Programm 2080_rechner_erw` -> `[msgtext] room=2080 id=0 "From here maybe I can control the
  water level.Will you operate the computer?"` -> Ja -> `[scd F344] Cut_chg(7)` -> `[scd F660] Cut_chg(3)` -> `[msgtext]
  room=2080 id=40 "The treatment pool is being drained as well."` -> `[scd F674] letterbox closed`. (Die Wegplan-Zeile
  FLAG (9,107) danach erreicht der Lauf vor EXIT_AT nicht — WARTE zaehlt waehrend der Szene nicht; Bit 107 siehe `rechner2`.)
* `nach2030` (Bits 103/100/101/102/107 + (3,245), an der 2080-Tuer, Aktionstaste): `[aot] DOOR FIRE slot=0
  rect=(-23900,-16700,hw=500,hh=1000) target_cut=9 spawn=(-6950,-16200,6450)` -> `loaded room2030.rdt` -> KEIN `unter
  Wasser`-Satz (keine Wasserflaeche), Wegplan F93 `FLAG (9,107) = 1`; einzig `ROOM2030: Tuer Slot 0 -> ROOM20B0 Cut 11
  (Ost-Tuer nach der Flut)` bleibt (Frage g, offen). AOTS F94: Lift-Rufzonen unten Slot 5/7 = Typ 5 (Text ev 1) — ohne Strom
  (3,126) = Original sub01 @0x01E14, s. §2 G1.
* `rechner2` (Bits 103/100 ohne 101/102, `RE15_FLAG_CENSUS=1`, Eingabe statt Wegplan, Satz 40 weggedrueckt): `ROOM2080
  Ereignis 2 -> Port-Programm 2080_rechner_erw` -> `id=0 "From here maybe I can control the water level.Will you operate the
  computer?"` -> **`[flagcensus] room=2080 WRITE Set  zone=9 idx=107 op=1`** -> `[msgtext] room=2080 id=40 "The treatment
  pool is being drained as well."` -> `EXIT_AT: Bild 1300 in Raum 2080`. Bild (`rechner`): Frage Ja/Nein, Cut 7 — der Pegel in
  2080 sinkt (F360 -> F630, Wand und Gitter tauchen auf), Cut 3 am Rechner mit Text 40.

### L3 — Punkt 3 Ventile -> Warnung -> 60 s -> Flut -> Tod (`flut`, Bildbogen `<a2>/flut_sheet.png`) und Ost-Tuer (`osttuer`)
* `flut` (Spinnen-Kill-Bits 7:155..157, `RE15_PLAYER_POS=6550,-100,2048` vor den Ventilen, Eingabe `A0.1,W4,A0.1,W2.5,
  A0.1,W4,A0.1,W4,A0.1,W2,R0.4` = Aktion, Ja, Warnung zu, stehen bleiben; `RE15_FADE_LOG=1`; direct3d `ABR2 (B-F) ok`):
  `ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile` -> `[msgtext] room=2030 id=45 "Numbered valves. They control the
  flood gates of the treatment pool.Will you turn them?"` -> `id=46 "WARNING: The water will rise in one minute. Leave the
  room!"` -> `Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> `Zaehler laeuft: 1800 Takte` -> `FLUT: Wasser steigt` ->
  `[fade-log] F2580 room=2030 CUT 0 -> 4` -> `SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400), Effekt
  0x06050D00` (genau 1x) -> `[fade-log] config ch=0 abr=2 mask=6 step=0 bucket=7`, `kick ch=0 value=0x5000` -> `ERTRUNKEN:
  Wasser y -6300, Spieler y -5400 -> hp -1, blaue Blende`.
* state.txt: Text 46 zuletzt F777 -> Flut F2579 (`cam=4 ... pm=2`) = **1801 Bilder = 60 s bei 30 Bildern/s**; Spritzer
  F2686 `fx=1`; Tod **F2704 `hp=-1`** = 125 Bilder nach Flutbeginn.
* Farbe (`test_r41_ventilkette ppm`, Bildmitte (120,80)..(200,160) / Vollbild): F2694 (29,36,29)/(30,37,30), F2700
  (30,37,30)/(30,37,30) -> **F2706 (0,0,30)/(0,0,30)**, F2712/F2718 (0,0,30) = blauer Schleier ab dem Todesbild; F2802/F2898
  YOU DIED.
* Bild: F576 Frage "Will you turn them?", F2574 Leon an den Ventilen, F2580/F2640 Draufsicht Cut 4 (Wasser steigt unter der
  oben stehenden Bruecke), F2688 Wasser auf Ringhoehe (Gelaender taucht ein), F2700 alles unter Wasser, F2706 blauer
  Schleier, F2898 YOU DIED.
* `osttuer` (Ventile -> Ja -> Warnung -> umdrehen -> Aktionstaste an der Ost-Tuer, kein FIRE_AOT; Spurbaum-exe OHNE den
  AUTO-Haken der Spur A): `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11 spawn=(5950,-1800,-6400)` ->
  `loaded room20b0.rdt` -> `[scd F951] Cut_chg(11)` -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut
  gelaufen, Leiter frei)` -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet) -> Sperrtext 41` -> `Plc_dest(slot=0
  mode=0x05 dest=(-15920,-6355) flag=0)` -> **kein** `first_op=0x46`, kein `letterbox closed`; state.txt F1270..F1300
  `PL(-15820,-6355) ... mo=2 ... cam=14 ... pm=2` = die Auswasch-Szene haengt im Spurbaum (wie Abnahme 0 L3 / Abnahme 1 L3o).
  Das ist die Integrationsabhaengigkeit von Spur A (Entscheid C-E1): Riegel `integration_r41_ventilkette_kette` existiert,
  Configure Z. 180 "uebersprungen: Spur A fehlt" (§0). Den Lauf mit dem wortgleichen Haken (Messbau) hat Abnahme 1 L3k/L7
  auf genau diesem Code-Stand (exe 06:37, seit 5ddd98c0 kein Spiel-Code geaendert) fuer Leon und Elza gruen gemessen;
  hier nicht wiederholt.

### L9 — Nachbesserung 2 / N1: der Weg zum Rechner NACH der Flut (eigene Laeufe `weg_leon`, `weg_elza`, `g206`)
Zustand nach der Flut: `RE15_SET_FLAG=9:103,9:100` (ohne 107), Zeitbombe benutzt `3:146,3:53` (Rost-Tuer 2040 frei),
Spinnen des 2070-Nordgangs als getoetet `7:98,7:99` (Kill-Bit pc[7] der Sce_em_set @0x00C06/@0x00C1A, selbst gelesen `44 00 25
00 00 00 00 62 ...` / `44 01 25 00 00 00 00 63 ...`; Gate FUN_800420a0 selbst disassembliert, §2 G1). Start in 2040 an der
Ankunftslage der 2060-Tuer (-12850,1600) Band 3; ab da zu Fuss (Wegplan-Treiber: echte Pad-Bits, Aktionstaste, kein
FIRE_AOT). Eigene Plaene `<a2>/plaene/weg_leon.txt`/`weg_elza.txt` (Wegpunkte wie Riegel `_weg` a/b, zusaetzlich AOTS-Abfrage
in 11A0 und zwei weitere Aktionsdruecke an der Sperre).
* **Leon** (`weg_leon`, Bildbogen `<a2>/weg_leon_11a0_sheet.png`): `loaded room2040.rdt` -> Rost-Tuer -> Treppe (Aktion F913
  bei (-1356,-13782)) -> `[aot] DOOR FIRE slot=2 rect=(-2400,-19800,hw=400,hh=1000) target_cut=11 spawn=(-3900,-9000,-19850)` ->
  Galerie -> `[aot] DOOR FIRE slot=4 rect=(-20450,-19800,hw=500,hh=1000) target_cut=9 spawn=(4650,0,-19200)` -> `loaded
  room2070.rdt` (Nordgang) -> Riegel (-18594,-19260): `[msgtext] room=2070 id=0 "You've opened the lock."`, Wegplan F565
  `FLAG (3,151) = 1` -> `[aot] DOOR FIRE slot=4 rect=(-19400,-19250,hw=500,hh=1000) target_cut=7 spawn=(6500,0,-3200)` ->
  `loaded room11a0.rdt` -> Wegplan F127 `AOT slot 4 typ 5 ev 2 flags 0x31 band 1 mitte (29000,1300) halb (500,1100)` (= Text-
  Platz msg 2, keine Tuer) -> Kante (6500,-693) Aktion -> Aufstieg -> Steg Ost (28457,1319) Blick 4080 -> Aktion F739 ->
  **`[msgtext] room=11a0 id=2 "It's locked from the other side."`**, Aktion F956 -> derselbe Satz noch einmal; `FLAG (3,206) = 0`
  bei F126/F955/F1142; `EXIT_AT: Bild 1200 in Raum 11a0` — kein `loaded room20a0`.
  Bild: Ankunft im Kanal (F40), Aufstieg an der Kante (F240/F320), Steg Ost Cut 9 (F600), der Satz an der Tuer (F760/F800),
  Leon bleibt in 11A0 (F1200).
* **Elza** (`weg_elza`, ohne TITLE_SHOT, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`; Bildbogen
  `<a2>/weg_elza_11a1_sheet.png`): `[pl] Spieler-Familie PL04` -> `loaded room2041.rdt` -> dieselben DOOR-FIRE-Zeilen ->
  `loaded room2071.rdt` (`ROOM2071: Ring-Tuer Slot 1/2 nach 2030 gesperrt (geflutet) -> Sperrtext 40`) -> `[msgtext]
  room=2071 id=0 "You've opened the lock."` -> `loaded room11a1.rdt` -> AOT Slot 4 Typ 5 ev 2 -> **`[msgtext] room=11a1 id=2
  "It's locked from the other side."`** (2x), `FLAG (3,206) = 0`, `EXIT_AT: Bild 1200 in Raum 11a1`.
* **Gegenprobe** (`g206`, `RE15_SET_FLAG=9:103,9:100,3:151,3:206`, Steg Ost (27000,1300) Band 1): Wegplan `FLAG (3,206) = 1`,
  `AOT slot 4 typ 1` (Tuer) -> Aktion -> `[aot] DOOR FIRE slot=4 rect=(29000,1300,hw=500,hh=1100) target_cut=8
  spawn=(25800,-3600,-800)` -> `loaded room20a0.rdt` = (3,206) ist an dieser Stelle die einzige Sperre (Ziel-Bytes @0x01006
  selbst gelesen: Stage-Index 01, Raum 0x0A, Cut 08, nfloor 02, Ankunft (25800,-3600,-800)).
* Die anderen Glieder (2050 -> 2020, 2030-Nordgalerie -> 2060, 2080 ab der 2090-Tuer -> Rechner) misst der Riegel
  `integration_r41_ventilkette_weg` d/e/f (mein ctest-Lauf §3); 2020 -> 2030N und 2060 -> 2040 hat der Bau-Agent aus R36-Riegeln
  uebernommen (nicht wiederholt, fuer das Ergebnis ohne Belang: der Weg endet in jedem Fall an der 11A0-Sperre).

### L10 — Statischer Tuer-/Bit-Zensus und Erreichbarkeit (eigene Werkzeuge, unabhaengig vom Werkzeug des Bau-Agenten)
* `<a2>/py/zensus.py` (Walker-Regionen aus tools/scd_dump_room.py, Tuer-/Bit-Auswertung eigen; Ausgabe `<a2>/zensus.txt`):
  240 RDT-Dateien, 206 mit Kopf; 653 Door_aot_set, davon 4 in der 40-Byte-Form (4030/4031 -> 404x, hier ohne Belang) — mit
  eigener Offset-Verschiebung gelesen; ROH-Scan aller Bytes nach 0x3B-Tuersaetzen mit Ziel in der Wegmenge, die der Walker
  NICHT als Opcode sah: **0 Treffer**. Die 18 Walker-Abbrueche liegen alle HINTER dem Skriptende ihrer Region (Evt_end
  ohne spaetere Sprungziele, z. B. 20A0 sub10 Ende @0x02004 / Abbruch @0x02080, 2080 sub02 Ende @0x00CF0 / Abbruch
  @0x00E3C; `<a2>/py/abbruch.py`) = Daten, kein verpasster Code.
  - Zugaenge **2080**: nur 2030/2031 Slot 1 @0x01AEA (floor 9 = Suedgalerie) und 2090/2091 Slot 0 @0x00996.
  - Zugaenge **2090**: nur 2080/2081 Slot 1 @0x00BC2 und 20A0/20A1 Slot 0 @0x01746.
  - Zugaenge **20A0**: nur 11A0/11A1 Slot 4 @0x01006 (Sonst-Zweig von Ck(3,206)=0 @0x00FEA), 2090/2091 Slot 1 @0x009B6 und
    20A0 selbst (Slot 1 @0x01766).
  - Setzer (Set op 1 ODER op 7 = XOR, LAB_8003fdd0 selbst disassembliert, §2): **(3,206) nur 20A0/20A1 sub10 @0x01FF0**;
    (3,151) nur 2070/2071 sub02 @0x00CE4; (3,126) nur 2060/2061 sub01 @0x012AE; (3,108) nur 2030/2031 sub06 @0x01FF2;
    (3,245) nur 2080/2081 sub02 @0x00C94; (3,53) nur 2040/2041 sub02 @0x01624; (3,146) nur 1110/1111 sub03 @0x00D32.
  - Tueren NACH STAGE2 von ausserhalb: nur 1260/1261 -> 2000 und 11A0/11A1 -> 2070 / 20A0; NACH STAGE3 von ausserhalb: nur
    11A0/11A1 -> 3000 und 4000/4001 -> 3070. Kein Tuersatz in STAGE2/11A0 traegt ein Schluessel-Byte.
  = deckt sich Satz fuer Satz mit `C_belege/nb2_tuergraph_nach_flut.txt` und unit J1 (16 Funde).
* Lifte 2030 (eigener SCD-Dump `<a2>/scd_2030.txt`): Lift A Slot 4 (oben, floor 6 = y -10800, NICHT die Galerie) / Slot 5
  (unten, Ring) je nach (3,222) @0x01CCE; Lift B Slot 6 (oben, floor 9 = Suedgalerie) / Slot 7 (unten, Ring) je nach (3,220)
  @0x01D70; sub01 @0x01E10: `21 03 7e 00` Ck(3,126)=0 -> @0x01E18/@0x01E22 Aot_reset Slot 5 und 7 sce 1 (Text) = ohne Strom
  sind BEIDE Lifte unten nur Text, oben inert (Slot 4/6 sce 0, solange der Lift unten steht). sub01 laeuft jedes Spielbild
  (FUN_8003f038, §2). Die Nordgalerie (Tueren 2020/2060) hat keinen Lift.
* Erreichbarkeit VOR der Flut (`<a2>/py/erreichbar.py`, Ausgabe `<a2>/erreichbar.txt`; Ueber-Approximation: ALLE
  Tuerbedingungen ignoriert ausser den drei Eintritten Leiter 2000 Slot 4 (Port-Sperre bis Bit 100), 11A0 Slot 0 ((3,151)),
  11A0 Slot 4 ((3,206)); Lift B erst, wenn 2060 erreicht ist; 2030 in Ring/Nordgalerie/Suedgalerie und 2070 in drei Gaenge
  geteilt): von 1000/1240 aus 41 Knoten erreichbar, darunter 2000/2010/2070-Mittelgang/2030-Ring, aber **keiner** von 2050,
  2020, 2030-N, 2060, 2040, 2070-N, 11A0, 20A0, 2090, 2080, 2030-S, 3000, 3070, 4000. => Die Aussage des Bau-Agenten "vor der
  Flut ist 2080 ebenfalls unerreichbar; bei der Flut gilt in jeder Reihenfolge (3,206)=(3,245)=(3,126)=0" haelt.
* ROOM2070 SCA @0x714 (selbst gelesen: Kopf ceiling (55888,51982), 5 x 12 Zellen, alle Typ 1, u0 0xff = Wand): je Region drei
  geschlossene Wand-Rechtecke — Mittelgang x -19450..5500 / z -8200..-4600 (Zellen 0-3), Suedgang x -19450..6200 / z
  4650..8100 (Zellen 4-7), Nordgang x -19400..5450 / z -21000..-17450 (Zellen 8-11). Zwischen den Gaengen liegt keine Flaeche
  => drei getrennte Gaenge, wie der Bau-Agent schreibt (§0b M7 berichtigt); meine Erreichbarkeits-Teilung oben folgt dem.

## 2. Gates

### G1 RE-Gate
* Nachbesserung 2 aendert keinen Spiel-Code (`git diff 5ddd98c0 HEAD --name-only -- re15_port/engine re15_port/include
  re15_port/platform` leer). Neu sind nur Riegel (unit J, integration `_weg`), das Werkzeug `tuergraph_nach_flut.py` und
  Dossier. Alle darin verwendeten Konstanten sind RDT-Byte-Offsets; selbst nachgelesen (Python, `shared_assets/PSX`):
  11A0/11A1 @0x00FE6 `06 00 1c 00`, @0x00FEA `21 03 ce 00`, @0x00FEE `2c 04 01 31 01 00 54 6f c8 00 e8 03 98 08 02 00 ff ff
  00 00` (Text-Platz Slot 4, Band 1, msg 2), @0x01002 `07 00 26 00`, @0x01006 `3b 04 02 31 01 00 54 6f c8 00 e8 03 98 08 c8 64
  f0 f1 e0 fc 00 04 01 0a 08 02 ...` (Tuer -> Stage-Index 1 Raum 0x0A, Cut 8, nfloor 2, Ankunft (25800,-3600,-800)); msg 2
  @0x142C = "It's locked from the other side." (tools/rdt_msgdump.py); 20A0/20A1 @0x01FE6 `46 0a ...`, @0x01FF0 `22 03 ce 01`,
  @0x01FF4 `2b 02 ff ff` (= "You've opened the lock." @0x204C), @0x01FFA `46 02 02 31 ...`; 2070 @0x00ABE `21 03 97 00`,
  @0x00CE4 `22 03 97 01`, Sce_em_set @0x00C06/@0x00C1A/@0x00C6C/@0x00C80 pc[7] = 0x62/0x63; 2040 @0x01624 `22 03 35 01`,
  @0x0133C `2c 05 0d 31 03 ...` (Treppe sce 13), @0x0131C (Tuer -> 2070); 1110 @0x00D32 `22 03 92 01`; 2060 @0x012AE
  `22 03 7e 01`, @0x010A2 `21 03 6c 00`, @0x010C2 `21 03 90 00`; 2030 @0x01E14 `21 03 7e 00`, @0x01BA6 `21 03 6c 00`,
  @0x01BAA Slot 9 (Sicherung, Band 1); 2090 @0x00996 (-> 2080), @0x009B6 (-> 20A0); 2080 @0x00BC2 (-> 2090), @0x00C02
  `21 03 f5 00`, @0x00C06 Text msg 1, @0x00C94 `22 03 f5 01` — **alles wie zitiert**.
* EXE selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE`, Hauptbaum nur
  gelesen):
  1. **FUN_800420a0** (Sce_em_set; Zitat der Spinnen-Kill-Bits im Riegel `_weg`): @0x800420d8 `lh v1,4064(v1)` (Stufe),
     @0x800420ec `slti v1,v1,3` -> Basis 0x800b48bc (Zone 7) bzw. @0x8004211c 0x800b48dc; @0x80042120 `lbu a1,7(a1)` (pc[7]),
     @0x80042124/28 `ori v0,zero,0xff` / `beq a1,v0` (0xFF = ohne Kill-Bit), @0x80042130 `jal 0x8004efe4` (Bit-Test),
     @0x80042138 `beq v0,zero,0x80042154` (nicht getoetet -> Spawn), sonst @0x80042144 `jal 0x80039b2c` + @0x8004214c/50
     Rueckgabe 1 = kein Spawn. => Kill-Bit (7,98)/(7,99) unterdrueckt genau die zwei Nordgang-Spinnen. Stimmt.
  2. **LAB_8003fdd0** (Set): @0x8003fe10/14 op==1 -> @0x8003fe64-6c `lw`/`srlv`/`or` (setzen); op==0 -> @0x8003fe4c-58
     `srlv`/`nor`/`and` (loeschen); @0x8003fe38/3c op==7 -> 0x8003fe78 (XOR); sonst kein Schreiben. => Setzer = op 1 und op 7
     (mein Zensus zaehlt beide; der Bau-Agent nur op 1 — kein op-7-Setzer der Wegbits vorhanden, ohne Folge).
  3. **FUN_8003f038** (Spielbild): @0x8003f040-4c Pausen-Gate `g_pauseflags & 0x02000000`, @0x8003f064 `ori a0,zero,0x1`
     (Thread-Platz 1), @0x8003f070 `lw v0,68(v0)` (RDT+0x44 = sub-Tabelle), @0x8003f080 `jal 0x8003ee3c` mit a1 = 1 => sub01
     laeuft jedes Spielbild neu; damit gilt die Strom-Pruefung 2030 sub01 @0x01E14 (Lifte unten = Text ohne (3,126)) dauernd.
     Das stuetzt die Praemisse von C-E2c ("beide 2030-Lifte haengen am Generator 2060").
* Diff-Suche (`git diff 5ddd98c0 HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/
  TODO/FIXME/approx/ungefaehr/geschaetzt/vermutlich/wahrscheinlich: **0 Treffer**; kein neues `getenv` in engine/include/
  platform (gesamter Zweig gegen master).
* Hinweis (kein Mangel, unveraendert seit Abnahme 0): `RE15_R41C_WASSER_Y_RUHE (-7200)` traegt im Kopf nur "1800 ueber dem
  Ring" — die Kennzeichnung PORT-WAHL steht im Dossier (§3.6/§6), nicht am #define.
=> **RE-Gate haelt.** Die Behauptungen des Bau-Agenten zu Tueren, Sperren, Bits und Liften stimmen mit den Bytes und mit
meinem unabhaengigen Zensus ueberein (L10).

### G2 Vertrag
* Pfade (`git diff master --name-only` ohne analysis/): seit Abnahme 1 nur neu `tests/integration/test_r41_ventilkette_weg.cmake`,
  `tools/r41_ventilkette/tuergraph_nach_flut.py`, Aenderungen an `tests/unit/test_r41_ventilkette.c` (+156) und
  `tests/unit/probes/r41_ventilkette.cmake` (+13, Registrierung `_weg`). Keine release/, platform/android/,
  shared_assets/PSX/, pri/, synchro/, keine tests/CMakeLists.txt. Gemeinsame Dateien unveraendert gegenueber Abnahme 1
  (C-E3 angenommen).
* Keine neuen Bank-9-Bits, Text-Ids, Plaetze oder Ereignisse (Nachbesserung 2 ohne Spiel-Code).
* Neuer Riegel `_weg`: eigener exe-Praefix `re15_pc_r41cw_`, alte Kopien vorher geloescht, `RUN_SERIAL`, eigener Laufordner
  `lauf_<id>_<x>`, `cmake_minimum_required(VERSION 3.21)`, `.exe` nur per `MATCHES`, keine Backslash-Pfade. Messend: Log-Saetze
  (DOOR FIRE mit Rechteck/Ziel, msgtext woertlich), Flags (3,53)/(3,151)/(3,206)/(9,107) aus dem Wegplan-Log, Zellenzustand.
* C-E2b verlangte den Weg "Leon UND Elza": Elza ist im entscheidenden Abschnitt (2041 -> 2071 -> 11A1 -> Sperre) gemessen
  (Riegel b, mein Lauf `weg_elza`); die Glieder 2050 -> 2020, 2030N -> 2060 und 2080 ab 2090 nur mit Leon (Tuersaetze der
  Elza-Raeume byte-gleich, eigener Zensus). Fuer das Ergebnis ohne Belang (der Weg endet fuer beide an derselben Sperre);
  bei der Umsetzung von C-E2c ist der GANZE Weg mit beiden zu messen (steht so in C-E2c).

### G3 Nebenwirkungen
* Keine neuen: kein Spiel-Code geaendert. Die r41-Riegel laufen auf derselben exe gruen (§3), Elza-Laeufe gleich (L1, L9).
* Die neuen Riegel (unit J, `_weg` a/b) schreiben den BLOCKIERTEN Zustand fest ("Sperre in 11A0 haelt"); mit C-E2c j1 sind
  sie umzudrehen (der Bau-Agent weist selbst darauf hin).

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe 164 Tests (Regex `<a2>/regex_unit.txt` = Abnahme 1): `100% tests passed, 0 tests failed out of 164` (44.54 s,
  `<a2>/ctest_unit.txt`); `test_r41_ventilkette.exe`: `PASS (286 Pruefungen)`, J1 `206 Raeume, 16 Funde`, J2 Leon + Elza.
* `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b|_weg)?$" --timeout 1800 --output-on-failure`
  (09:09-09:29, exe 06:37, unter Parallel-Last durch andere Spuren und meine Messlaeufe):
  `1/3 Test #1126: integration_r41_ventilkette .......   Passed  216.67 sec`,
  `2/3 Test #1127: integration_r41_ventilkette_b .....   Passed  507.28 sec`,
  `3/3 Test #1128: integration_r41_ventilkette_weg ...   Passed  460.13 sec`,
  `100% tests passed, 0 tests failed out of 3` (`Total Test time (real) = 1184.19 sec`, EXIT=0, `<a2>/ctest_integ_r41.txt`).
  `_weg`-Laufordner `build/tests/integration/r41_ventilkette_weg/lauf_f3a7355b_{a..f}` selbst gelesen: a Leon / b Elza
  (`PL04`) je `You've opened the lock.` -> `loaded room11a0/11a1.rdt` -> `[msgtext] room=11a0/11a1 id=2 "It's locked from the
  other side."`, `FLAG (3,206) = 0` (2x), kein `loaded room20a`; c `DOOR FIRE slot=4 rect=(29000,1300,hw=500,hh=1100)
  target_cut=8` -> `loaded room20a0.rdt`; d `ZELLE r0[51] LAUFZEIT: ... u0 0x00` -> `DOOR FIRE slot=1 rect=(-5150,4450,...)` ->
  `loaded room2020.rdt`; e `ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1` -> `DOOR FIRE slot=3
  rect=(-7700,-7250,...)` -> `loaded room2060.rdt`; f `2080_rechner_erw` -> `Cut_chg(7)` -> `id=40` -> `FLAG (9,107) = 1`.
* `integration_r41_ventilkette_kette`: im Spurbaum nicht registriert (Configure Z. 180 "uebersprungen: Spur A fehlt") = C-E1.
* Nicht erneut gefahren: Heliport 5/5, dachkiste/itemband/kistenregel (Abnahme 0/1 gruen; seit Abnahme 1 kein Spiel-Code).

## 4. Urteil je Nutzer-Punkt (gegen den Wortlaut, AUFTRAG.md; gebauter Stand exe 06:37 = HEAD 1498d1c8)

| Punkt | Urteil | Beleg (eigene Messung) |
|---|---|---|
| 1 Leiter 2000 -> 2050 mit herunterlaufendem Wasserstrom blockiert, "I cannot climb up the ladder because of the water pressure", bis die Flut gelaufen ist | **erfuellt** | L1: Leon `leiter` + Elza `leiter_elza` zu Fuss + Aktionstaste: Text 40 woertlich, kein Raumwechsel, Strahl am Gitter (`fx=23`, Bildbogen); nach der Flut frei (L4: 20B0-Leiter -> 2050 ohne Sperrtext) |
| 2 Bruecke zunaechst oben; Pult links (Kamera 2, Nordwand); zu wenig Druck / Ventil 13 zu -> Ventil 13 -> "Lower the bridge?" -> Bruecke faehrt herunter; nicht die Kurbel | **erfuellt** | L2: `pult_ohne` Text 40 (Manometer null, Ventil 13 zu), Steg bei z -6418 gesperrt, Schaltschrank = Nutzerbild; `bruecke3` Ventil 13 Text 41/42 + Bit 101 -> Pult Text 43 "... Lower the bridge?" -> Cut 4, Planke -1800 -> Bit 102 -> Text 44 -> ueber den Steg bis (-1043,2989) |
| 3 Ventilraetsel-Platzhalter -> Warnung (1 min, Raum verlassen) -> 60 s -> Wasser ueber die Ringebene; Ost-Tuer -> 20B0 (Auswasch-Szene); Zeit abgelaufen im Raum: Tod mit Ertrink-Effekt | **erfuellt** (Szene 20B0 mit dem Spur-A-Haken, Entscheid C-E1) | L3: Warnung woertlich, 1801 Bilder bis zur Flut, Wasser ueber den Ring (Bild), ein Spritzer, Tod +125 Bilder, Blende (0,0,30) im beschleunigten Renderer, YOU DIED; Ost-Tuer zu Fuss -> 20B0 + Bits 103/100; die Auswasch-Szene haengt im Spurbaum ohne den AUTO-Haken der Spur A (C-E1; Riegel `_kette` vorhanden, sauber uebersprungen; mit Haken von Abnahme 1 auf demselben Code gemessen) |
| 4 Danach 20B0 statt 2000 (spaeter wieder frei ohne Wasser), Leiter frei (kein Wasser laeuft mehr herunter) | **erfuellt** | L4: 2010-Tuer -> 20B0 Cut 10, 20B0-Leiter -> 2050 ohne Text, 2050-Leiter -> 20B0 Cut 9, `fx=0` in allen 1423 Bildern (kein Strahl) |
| 5 2050: Wasserlevel (Hebel) erst nach dem Pult-Raetsel (80 V wie 11F0) | **erfuellt** | L5: Hebel Ereignis 2 (Kisten) und 4 ohne Bit 106 = Text 40, kein Cut 4, (3,104)=0/(3,109)=0; Pult per Steuerkreuz 40/70/80 V -> "Power supply OK." -> Bit 106; danach Original sub04 -> (3,104)=1 (Wasser steigt) |
| 6 2080: der Rechner senkt das Wasser in 2080 und entleert danach auch 2030 (Raum wieder frei ohne Wasser) | **teilweise** | Mechanik erfuellt (L6: Frage -> Cut 7, Pegel 2080 sinkt -> `WRITE Set zone=9 idx=107 op=1` -> Text 40; 2030 mit Bit 107 ohne Wasserflaeche). ABER im Spielablauf nicht erreichbar: nach der Flut fuehren nach 2080 nur die 2030-Suedgalerie (Lift B vom gefluteten, nach C-E2 gesperrten Ring) und 2090; nach 2090/20A0 nur ueber die 11A0-Tuer, die die ORIGINAL-Sperre Ck(3,206)=0 @0x00FEA -> msg 2 @0x00FEE haelt ((3,206) setzt nur 20A0 sub10 @0x01FF0) — zu Fuss gemessen mit Leon und Elza (L9), statisch belegt (L10); vor der Flut ebenso unerreichbar (L10). "2030 wieder frei" tritt in keinem Spielablauf ein. Der Orchestrator-Entscheid C-E2c (j1, master 09:00) ist im Zweig NICHT umgesetzt |

## 5. Maengel (nummeriert, nachpruefbar)
Stand der Abnahme-1-Maengel: **N1 offen** (unten). Die Nachbesserung 2 hat den Auftrag C-E2b vollstaendig ausgefuehrt
(Tuergraph, Weg zu Fuss bis zur Sperre mit Leon und Elza, blockierender Punkt mit Tuer/Bit/SCD-Offset/Bild benannt, nichts
erfunden, §7/§0b M5/M7 richtiggestellt, Riegel) — und ihr Ergebnis ist unabhaengig bestaetigt (L9, L10, G1). Erfuellt ist
Punkt 6 damit trotzdem nicht; die Entscheidung dazu liegt seit 09:00 vor (C-E2c).

* **N1 (hoch, Punkt 6; offen, Entscheid C-E2c j1 liegt vor, Umsetzung fehlt)** Der Rechner 2080 ist im Spiel nicht
  erreichbar, also wird 2030 nie wieder frei, und 2080/2090/20A0 (Alligator, Szenen der Spuren B/F/G) sind in keinem
  Spielablauf erreichbar.
  - Messung: L9 `weg_leon`/`weg_elza` (2040 -> Treppe -> 2070-Nordgang -> Riegel -> 11A0/11A1 -> Steg Ost -> Aktion ->
    `[msgtext] room=11a0/11a1 id=2 "It's locked from the other side."`, `FLAG (3,206) = 0`, kein Raumwechsel; Bildboegen
    `<a2>/weg_leon_11a0_sheet.png`, `<a2>/weg_elza_11a1_sheet.png`); Gegenprobe `g206` -> 20A0; Riegel `_weg` a-f gruen.
  - Beleg: 11A0/11A1 main00 @0x00FEA `21 03 ce 00` / @0x00FEE Text-Platz msg 2 / @0x01006 Tuer nur im Sonst-Zweig; einziger
    Setzer von (3,206) ist 20A0/20A1 sub10 @0x01FF0; einzige Zugaenge zu 2080/2090/20A0 laut eigenem Zensus (L10); Lifte 2030
    ohne Strom nur Text (sub01 @0x01E14, jedes Bild FUN_8003f038), Strom (3,126) nur 2060 @0x012AE, 2060 vor der Flut
    unerreichbar (Leiter-Sperre; Erreichbarkeit L10).
  - Forderung (= Orchestrator-Entscheid C-E2c, AUFTRAG.md auf master d370db3c): mit dem Ende der Flut (Bits 103 + 100)
    (3,206) = 1 setzen (PORT-WAHL als Folge der Port-Flut, kein neuer Text); Riegel "(3,206) vor der Flut 0, danach 1"; den
    Weg 11A0 -> 20A0 -> 2090 -> 2080 -> Rechner -> Bit 107 -> 2030 entleert zu Fuss messen, Leon UND Elza (Alligator per
    Testhaken ruhig/entfernt) — insbesondere die noch nie gegangenen Strecken IN 20A0 (Ankunft aus 11A0 (25800,-3600,-800)
    nfloor 2 -> Slot 0 Rechteck x 3450..6450 z 4500..5500 floor 2; 20A0 hat ein Rolltor, sub00 @0x01A3E Ck(3,11) mit
    Sca_id_set der Zellen 0x20, und Gegner-Plaetze unter (3,170)/(3,171) @0x018CA/@0x01900) und DURCH 2090 (Ankunft
    (6300,-1800,-16250) nfloor 1 -> Slot 0 x -9400..-8400 z -26000..-25000 floor 0); unit J2 und `_weg` a/b auf den neuen
    Zustand umstellen; Dossier §7 mit dem Weg fuer den Nutzer.

Hinweise (kein Mangel):
* H-a (Einordnung N1): "ORIGINAL-Sperre" stimmt fuer den in C-E2b angenommenen Umweg. Der Original-Zugang zu 2080 (Ring ->
  Lift B mit Strom aus 2060 -> Suedgalerie -> 2030 Slot 1) ist durch Port-Eingriffe der Runde 41 zu: Leiter-Sperre bis zur
  Flut (damit 2060/Strom vor der Flut unerreichbar), danach Flut + Ring-Sperre C-E2 + Lift-Rufzonen-Text. C-E2c benennt das
  richtig als Folge der Port-Flut.
* H-b: Die Sicherung (3,108) liegt in 2030 Slot 9 @0x01BAA auf dem Steg-Suedlandeplatz (Band 1, y -1800). Wer sie vor der
  Flut nicht holt, kommt erst nach dem Entleeren wieder hin (Ruhepegel -7200); bis dahin bleiben die 2030-Lifte ohne Strom.
  Der j1-Weg braucht keinen Strom (gemessen: Rechner mit nur 103/100 in `rechner2`, mit 103/100/206 in `_weg` f). Zur
  Kenntnis fuer Nutzer und Orchestrator.
* H-c: Das Werkzeug `tuergraph_nach_flut.py` zaehlt als Setzer nur `Set ... 01` (nicht op 7) und listet Zugaenge nur fuer
  2080/2090/20A0/2070/2040/11A0; mein Zensus (op 1 + 7, alle Ziele, ROH-Scan) findet nichts Zusaetzliches — ohne Folge.
* H-d: Fragen (g) (Ost-Tuer 2030 nach Bit 107) und (h) (Spritzer ausserhalb Cut 4) sind in AUFTRAG.md (master) weiter ohne
  Entscheid; Punkt 3 haengt fuer die 20B0-Szene an Spur A (C-E1); PSX-Ziel ungebaut (H3); Port-Texte Vorschlag bis zur
  Freigabe des Nutzers.

## 6. Fazit
**Nicht bestanden.** Punkte 1-5 sind am gebauten Stand erfuellt (Punkt 3 mit der Spur-A-Abhaengigkeit laut C-E1), RE-Gate und
Vertrag halten, alle gefahrenen Riegel sind gruen (Unit 164/164, `unit_r41_ventilkette` 286 Pruefungen, r41-Integration 3/3
inklusive des neuen `_weg`). Das Ergebnis der Nachbesserung 2 ist unabhaengig bestaetigt: nach der Flut gibt es keinen Weg zum
Rechner 2080 (die letzte Sperre ist die Original-Einbahntuer 11A0 Slot 4, (3,206), nur von 20A0 aus zu oeffnen), und vor der
Flut auch nicht — in jeder Spielreihenfolge. Die Spur hat das gemaess C-E2b korrekt als OFFEN gemeldet und nichts erfunden.
Der Orchestrator hat um 09:00 entschieden (C-E2c, j1: (3,206) = 1 mit dem Ende der Flut); solange das nicht umgesetzt und der
Weg 11A0 -> 20A0 -> 2090 -> 2080 mit Leon und Elza gemessen ist, bleibt Punkt 6 ("entleert danach auch 2030") im Spiel
unerreichbar: teilweise, Mangel N1.
