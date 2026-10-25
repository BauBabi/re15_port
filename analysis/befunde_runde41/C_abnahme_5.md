# Runde 41 — Spur C "ventilkette" — Abnahme 5 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD e1f1b6d6 (Basis = merge-base master
116e8b0f; master steht inzwischen auf aa05ffd0 und enthaelt fremde Spuren — Gegenstand ist der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Nachtraegen 6-19 (Spur-C-relevant: 6-10, 12, 13,
15, 16, 18, 19; die master-Nachtraege 20-24 betreffen F/G/B, gelesen) und den Entscheiden C-a..e, C-E1..C-E3, C-E2b (C-E2c widerrufen).
Vorlage: Abnahme 4 (`C_abnahme_4.md`, Maengel 1-3), Nachbesserung 5 (Dossier §0f, Commits 4e70c77c..e1f1b6d6).
Begonnen 2026-10-25 ~02:0x. Pruefer: Abnahme 5 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad `C41abn5/`
(unten `<a5>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn5`).

**Ergebnis: (laeuft)**

## 0. Stand / Protokoll (neueste zuerst)
* 02:1x Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `integration_r41_ventilkette_kette uebersprungen:
  Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a5>/configure.txt`, `<a5>/build.txt`);
  exe `re15_pc.exe` 2026-10-25 00:40:35 (7942482 B) = Code HEAD (ventilkette_r41.c 00:38:10 -> .obj 00:40:02; `git diff HEAD` leer).
  Unit-Gruppe (Regex `<a5>/regex_unit.txt` = Abnahme-4-Regex + `unit_anim_.*|unit_plc_.*|unit_audio_.*|unit_se_.*|
  unit_r36_tueren_aufschliessen|unit_bss_room_source|unit_r36_karte.*|unit_fahrstuhl_.*` fuer RBJ-Ueberlagerung, Toene, BSS-Standbild):
  **`100% tests passed, 0 tests failed out of 261`** (`Total Test time (real) =  67.44 sec`, `<a5>/ctest_unit.txt`);
  `unit_r41_ventilkette` direkt: **`PASS (517 Pruefungen)`**. Integration r41 in zwei Stroemen gestartet (`<a5>/integ.sh`).
* 02:0x Start: `git status` = nur `?? build/`; HEAD e1f1b6d6; Spur-Diff (`git diff master...HEAD --stat`, ohne analysis/) = 42 Pfade
  (+15063/-1); seit dem Code-Stand der Abnahme 4 (b08bb0c4..HEAD): ventilkette_r41.c (+195/-), re15_ventilkette_r41.h (+99/-),
  platform/pc/src/ventilkette_r41_pc.c (neu, 222), main.c (15 +-), scd_vm.c (2 +-), gen/*.inc (hebel neu, plakat/programme/texte/
  original geaendert), neue Assets shared_assets/RE2/VENTIL1080.{RBJ,EDT,VH,VB}, Tests (unit + 3 integration + probes), Werkzeuge
  (hebel2050.py, re2_ventil_export.py neu), synchro/STAGE2/room203{0,1}/main46.wav (Orchestrator-Commit f091aa04). Bau laeuft.

## 1. Messungen an der exe (eigene Laeufe)
Werkzeug: `<a5>/lauf.sh <name> <timeout> KEY=VAL...` (eigene exe-Kopie `re15_pc_abn5c_<name>.exe` neben der exe, erbt keine
RE15_-Variablen, Laufordner `<a5>/mess/<name>`, Kopie danach geloescht), Wegplaene `<a5>/plaene/*.txt` (Treiber RE15_WEGPLAN = echte
Pad-Bits, dachkiste_r36.c, hoechstens 96 Befehle). Beschleunigter Renderer (kein RE15_SOFTWARE_RENDER), RE15_WINDOW_SCALE=1,
RE15_FRAMEDUMP (Bildzaehler je Raum: g_engine.frame_count wird beim Raumladen auf 0 gesetzt, main.c:8248 - RE15_PRESS und Framedump
zaehlen je Raum). Leon = Titel-Haken, Elza = RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 (`[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)`). Spinnen 2030 als getoetet (RE15_SET_FLAG 7:155..158) wie die Riegel. exe = `re15_pc.exe` 00:40 = Code HEAD.
Bildbogen: `<a5>/py/sheet.py`.

### P1 — Punkt 1: Leiter 2000 vor der Flut, echte Tuer 2010 -> 2000 (Leon `p1r`, Elza `p1r_elza`; Wegplan der Abnahme 4)
* `[aot] DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=14 spawn=(-26450,-1800,-2700)` -> `loaded room2000.rdt` (Elza
  `room2001.rdt`) -> `ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40` -> zu Fuss an die Leiter -> **`[msgtext] room=2000 id=40 "I
  cannot climb up the ladder because of the water pressure."`** (2x), kein Raumwechsel; Wegplan `FLAG (9,100) = 0` vor und nach der
  Leiter. Neu im Log: `ROOM2010: Ventil-Sicherungskasten Slot 8 (x 4900..5900 z -3470..-2070), Sicherung fehlt`.
* Bild `<a5>/bilder/p1r.png` / `p1r_elza.png` (Cut 10, F240..F480): Wasserstrahl aus dem Gitterfenster neben der Leiter, Leon bzw.
  Elza an der Leiter, Satz 40. **Unveraendert gegen Abnahme 4.**

### N16 — Nachtrag 16 Folgen 1-4: Ventil-Sicherungskasten 2010 (Leon `ka`/`ka2`/`ka3`/`kk`, Elza `ka3_elza`)
* `ka` (Leon; Start 2030 Sued-Landeplatz, Bits 101/102 = Bruecke unten, Wegplan `<a5>/plaene/ka.txt`): Ventil 09 OHNE Sicherung im Kasten
  -> **`[msgtext] room=2030 id=49 "It's locked."`**; Sicherung (Original Slot 9): `id=2 "Will you take the Fuse?"` -> Ja -> `id=3
  "You've taken the Fuse."` (Wegplan `FLAG (3,108) = 1`); Ventil 09 erneut -> **`id=49 "It's locked."`** (mit der Sicherung in der Hand
  gesperrt, Bit 125 = 0); zu Fuss Steg -> Treppe -> Ring -> Ost-Tuer: `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750)
  target_cut=0 spawn=(4800,0,-6300)` -> `loaded room2070.rdt` -> Mittelgang -> `DOOR FIRE slot=0 rect=(-19450,-6400,hw=500,hh=1500)
  target_cut=13 spawn=(-20650,-1800,-6200)` -> `loaded room2000.rdt`. Dort lief mein Wegplan (gerade Linie zur 2010-Tuer) an der
  Kanalkante fest (`KLEMME ... Rest 4982`, mein Planfehler: der Westgang liegt jenseits des Kanals) -> Lauf per PID beendet (eigene
  Kopie), Fortsetzung `ka2`.
* `ka2` (Leon; Start 2070 Mittelgang (-15000,-6300) mit (3,108), Wegplan `<a5>/plaene/ka2.txt`, MIT Ton + RE15_SE_DEBUG +
  RE15_AUDIO_CAP_SYNC): `DOOR FIRE slot=0 ... spawn=(-20650,-1800,-6200)` -> `loaded room2000.rdt` -> Kanal hinab/hinauf -> `DOOR FIRE
  slot=2 rect=(-27100,-2650,hw=500,hh=1000) target_cut=0 spawn=(-4600,0,-4950)` -> `loaded room2010.rdt` -> zu Fuss an den Schaltschrank
  der Ostwand (Cut 3) -> `ROOM2010 Ereignis 20 -> Port-Programm 2010_kasten_frage` -> **`id=40 "A fuse box for the valve controls.One of
  the fuses is missing..."`** (2 Seiten) -> **`id=41 "Will you use the Fuse?"`** -> Ja -> `Ventil-Sicherungskasten 2010: Sicherung
  eingesetzt -> Bit (9,125) = 1` -> **`[se] Runde 41 Spur C: Aufschliess-Ton KARTSE Satz 3 (RE2 Satz 0x26, EXE @0x800515f8) F748`** ->
  `Standbild der Ventile offen (ROOM2030 Cut 7)` -> `Standbild BSS/ROOM2030/BG07.BSS dekodiert`. Ton in der Aufnahme (`<a5>/py/
  cap_rms.py`, Hochpass-RMS je Spielbild; die Raummusik laeuft durch): Median der letzten 700 Bilder 64, an Raumbild ~746..751 230..467
  und **das Maximum des ganzen Fensters 1083 an Raumbild ~761** = die Transienten des Aufschliess-Tons direkt nach dem Ja (F745/F748).
* `ka3` (= ka2 + RE15_PRESS square@830/910, ohne Ton): Standbild -> `Standbild der Ventile zu (nach 82 Bildern)` -> zurueck in Cut 3 am
  Kasten -> `id=42 "You've used the Fuse."`. Bildbogen **`<a5>/bilder/ka3_kasten.png`**: F740 Frage "Will you use the Fuse? Yes/No" am
  Schaltschrank, F760/F820 Vollbild = die Ventilwand von 2030 (Schilder 09/08/07/06, Raeder), F840..F930 zurueck am Kasten mit Satz 42.
  `ka2_kasten.png`: F430 Satz 40 Seite 1, F530 Seite 2, F610..F730 Frage, F750 Standbild.
* `ka3_elza` (Elza, gleicher Plan mit 2071/2001/2011): Zeile fuer Zeile gleich (`loaded room2071.rdt` -> `room2001.rdt` -> `room2011.rdt`,
  `room=2011 id=40/41/42`, Bit 125 = 1, Standbild zu nach 82 Bildern). `BSS/ROOM2030/BG07.BSS` = `ROOM2031/BG07.BSS` bytegleich
  (md5 a1fe38b98e66c55eb9eab11b2b30a4dc beide).
* `kk` (Leon, Sprung an den Kasten mit (3,108) + RE15_FLAG_CENSUS + RE15_PRESS, Muster des Riegels k1, um auch das HERAUSNEHMEN zu
  messen): `WRITE Set zone=9 idx=125 op=1` -> Standbild -> 42 -> erneute Aktion `2010_kasten_drin` -> **`id=43 "The fuse is in place.Will
  you take the Fuse?"`** -> Ja -> **`WRITE Set zone=9 idx=125 op=0`** -> **`id=44 "You've taken the Fuse."`** -> `Sicherung genommen ->
  Bit (9,125) = 0` -> erneute Aktion -> wieder `2010_kasten_frage` (40 -> 41): Einsetzen/Herausnehmen beliebig oft. (Kamera in diesem
  Sprunglauf Cut 0, weil der Spieler nicht lief - Sprungartefakt, im echten Weg `ka3` Cut 3.)
* Gesperrte Ventile NACH dem Herausnehmen: der Zustand "herausgenommen" ist Bit 125 = 0 mit (3,108) = 1 - genau der Zustand von `ka` Teil 1
  (Ventil 09 = 49 mit der Sicherung in der Hand); die Bedingung im Code ist dieselbe (`bit(102) && bit(125)`, ventilkette_r41.c
  re15_ventilkette_r41_ereignis). Riegel: Unit K, `_weg` v2a.

### KB — Nachtrag 16 Folge 2 (Ventile frei mit der Kasten-Sicherung) + Nachtrag 18 Punkt 3 (RE2-Dreh-Animation) + Nachtrag 19 (Text 46) + Nachtrag 18 Punkt 4 (Stimme, Countdown) + Punkt 3 (Flucht -> 20B0), ECHTER Weg (Leon `kb`)
Start 2010 am Kasten (Bits 101/102, (3,108), (9,125) = Zustand nach `ka3`), MIT Ton (RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC), Wegplan
`<a5>/plaene/kb.txt` (93 Befehle), Framedump alle 15 Bilder:
* Weg: `ROOM2010: Ventil-Sicherungskasten Slot 8 ..., Sicherung steckt (Bit 125)` -> Tuer slot 0 -> `loaded room2000.rdt` (Westgang) -> Kanal
  hinab/hinauf -> `DOOR FIRE slot=3 rect=(-19800,-6200,hw=500,hh=2000) target_cut=3 spawn=(-18650,0,-6450)` -> `loaded room2070.rdt` ->
  `DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` (Ring) -> Treppe ->
  Steg -> Sued-Landeplatz.
* Ventile frei (Bit 125): `Ereignis 22 -> Port-Programm 2030_ventil_09` -> **`id=55 "Valve 09, red wheel. Will you turn it?"`** -> Ja ->
  `Ventil 09 gedreht (Drehung 1/4)`; 07 (`id=53`), ueber den Steg 02 (`id=51`), 01 (`id=50`) -> **`Folge 09 07 02 01 -> RICHTIG: Warnung,
  Zaehler, Flut`** -> `Ereignis 26 -> 2030_ventile_flut` -> **`[msgtext] room=2030 id=46 "WARNING: The water will rise in one minute. Please
  leave the room immediately!"`** (Nachtrag 19 woertlich; im Bild drei Zeilen "WARNING: The water will rise / in one minute. Please leave /
  the room immediately!", `<a5>/bilder/kb_countdown.png` F5485/F5545).
* RE2-Animation: `[rbj] Runde 41 Spur C: ROOM2030 bindet das RBJ von RE2 ROOM1080 (VENTIL1080.RBJ, 13128 B; Ventil-Animation)`; je Drehung
  `Plc_motion(entity=0, motion=15, flags=0x00)`, `motion=16`, 5 x `motion=17`, `motion=16, flags=0xC0`, `motion=15, flags=0xC0` = Folge RE2
  ROOM1080 sub08 (selbst gelesen, G1 Nr. 1); gezaehlt **20 x Clip 17, je 4 x 15/16 vorwaerts und rueckwaerts**. Bild: `<a5>/bilder/
  kb_ventil09.png` (F1165 Frage, F1315..F1750 waehrend der Drehung) und 3x-Ausschnitt **`<a5>/bilder/kb_ventil09_zoom.png`** (F1300..F1630):
  Leon steht vor der Suedwand links unter dem Schild "09", hebt die Haende an das Rad und dreht (Arme wandern von F1360 bis F1630), danach
  Hand zurueck - keine Verrenkung, kein Sprung der Lage.
* Ventil-Ton: `Bank VENTIL1080 (RE2 ROOM1080 snd0) geladen (9 VAGs, 48 Saetze)`, **24 x `[se] Runde 41 Spur C: Ventil-Ton VENTIL1080 Satz 5`**
  (6 je Drehung wie RE2 sub08); in der Aufnahme an allen 24 Stellen eine Transiente (Hochpass-RMS 4.9..5.5 x Median 117.9 der 2030-Zeit;
  Zuordnung capTick = Raumbild + 2078 aus dem Stimmen-Start).
* Stimme: `[voice] clip loaded: main46.wav (32000 Hz x1 -> 226817 @44100 Hz)` + `[voice] Start ROOM2030 Id 46 ... capTick 7189`; die
  Aufnahme korreliert mit `synchro/STAGE2/room2030/main46.wav` (auf 44,1 kHz) **0.903 bei Versatz 0** (Gegenprobe 2000 Takte frueher
  0.004) - die Aufnahme des Nutzers ist im Mix zu hoeren. Die Ventil-Fragen 50/51/53/55 melden `keine Aufnahme ... - Zeile bleibt stumm`.
* Countdown: `Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt`; Bild (`kb_countdown.png`): oben rechts rot **"00:59:06"**
  (F5590) -> "00:56:06" (F5680) -> "00:52:06" -> "00:47:06" -> "00:42:06" -> **"00:33:06"** (F6370, an der Ost-Tuer); laeuft in Echtzeit
  (90 Bilder = 3 s Unterschied), waehrend der Spieler zu Fuss ueber Treppe und Ring laeuft.
* Flucht: `DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11 spawn=(5950,-1800,-6400)` -> `loaded room20b0.rdt` ->
  **`Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)`** -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030
  gesperrt (geflutet) -> Sperrtext 41`; kein `FLUT`/`ERTRUNKEN`.
