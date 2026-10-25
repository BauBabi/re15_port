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

### KB_ELZA — dasselbe mit Elza (`kb_elza`, Wegplan mit 2011/2001/2071/2031/20B1)
* `Spieler-Familie PL04` -> `loaded room2011.rdt` -> `room2001.rdt` -> `room2071.rdt` -> `room2031.rdt` -> **`ROOM2031 bindet das RBJ von RE2
  ROOM1080`** -> `room=2031 id=55/53/51/50` -> `Folge 09 07 02 01 -> RICHTIG` -> **`room=2031 id=46 "WARNING: The water will rise in one
  minute. Please leave the room immediately!"`** -> `[voice] clip loaded: main46.wav` (`Start ROOM2031 Id 46 ... capTick 7904`, Korrelation
  mit `synchro/STAGE2/room2031/main46.wav` **0.885**, Gegenprobe -0.004) -> Zaehler -> zu Fuss -> `loaded room20b1.rdt` -> `Bits 103 + 100`.
  20 x Clip 17, 24 x Ventil-Ton. 3x-Ausschnitt **`<a5>/bilder/kb_elza_ventil09_zoom.png`**: Elza greift das rote Rad (der rote Ring an den
  Haenden ab F1330) und dreht; Proportionen normal (das RE2-RBJ verformt das PL04-Modell nicht).
* (Nebenbefund, kein Mangel: `[rbj] room 20B1 has no RBJ ... Leon auf PL00-Basis zurueckgesetzt` steht genauso fuer 2010/2011/2070/2071 -
  allgemeine Protokollzeile des Ladeweges, nicht Spur C.)

### KF — Punkt 3 "Zeit abgelaufen und noch im Raum" + Abnahme-4-Mangel 2 (Plakat waehrend des Zaehlers) (Leon `kf`)
Start Sued-Landeplatz (Bits 101/102, (3,108), 125), Wegplan `<a5>/plaene/kf.txt`: richtige Folge zu Fuss -> Warnung 46 -> zurueck ueber den
Steg zum Plakat (Slot 21) -> zweimal Aktion waehrend des Zaehlers -> stehen bleiben:
* **`[ventilkette41] ROOM2030 Ereignis 23: Plakat gesperrt (Zaehler/Flut laufen, ihre Pad-Sperre bleibt)`** (2x, F4930/F4993), kein
  `2030_plakat`, keine Nahaufnahme -> `FLUT: Wasser steigt (Countdown-Takte 1801 von 1800)` -> `[scd F6345] Cut_chg(4)` -> `SPRITZER: Wasser
  y -1800 erreicht die Fuesse` -> `[fade-log] config ch=0 abr=2 mask=6 step=0 bucket=7` + `kick ch=0 value=0x5000` -> **`ERTRUNKEN: Wasser
  y -2700, Spieler y -1800 -> hp -1, blaue Blende`**; zwischen FLUT und ERTRUNKEN **kein** `letterbox closed -> gameplay` (Mangel 2 behoben).
* Bildbogen **`<a5>/bilder/kf_flut.png`**: Countdown "00:47:16" (F4930) -> "00:02:16" -> "00:00:66" -> "00:00:16" (F6340) -> Cut 4 Draufsicht,
  das Wasser steigt ueber Steg/Bruecke (F6355/F6385) -> blauer Schleier (F6415/F6445) -> heller Uebergang -> "YOU DIED" (F6535..). Lauf endet
  per Zeitlimit nach dem Tod (rc 124, wie Abnahme 4 `fl`). (Kamera vor der Flut Cut 0 = bekanntes Sprungartefakt: nach RE15_DEBUG_JUMP in
  2030 schaltet die Kamera nicht um - ebenso in den Abnahme-4-Laeufen `k2`/`fl`/`plakat`/`dok` mit der alten exe; im echten Weg `kb` stimmen
  die Kameras, `[walk] cut=5/6/3/2...`.)

### KP — Nachtrag 18 Punkt 1: Plakat im Stil des Spiels (Leon `kp`, Elza `kp_elza`; RE15_PRESS wie Abnahme 4)
* `Ereignis 23 -> Port-Programm 2030_plakat` -> `Plakat-Nahaufnahme offen` -> Vollbild -> Viereck -> `Plakat-Nahaufnahme zu (nach 89 Bildern)`
  (Leon und Elza gleich). Bild 3x **`<a5>/bilder/kp_plakat_x3.png`** (F370): vergilbtes, fleckiges Papier mit Klebeband auf einer Stahlwand,
  Schreibmaschinen-/Druckschrift, gedaempfte Farbpunkte, Nummern auf schwarzen Feldern wie die Wandschilder; Inhalt unveraendert
  ("VALVE OPERATION / FLOOD GATES - POOL / 1. GREEN 09 / 2. RED 01 / 3. BLACK 02 / 4. RED 07 / Turn the valves in this order.").
  Vergleich vorher/nachher/11D0/11F0: `C_belege/nb5_plakat_vergleich.png` (gelesen): die Comic-Optik (satte Flaechenfarben, fette
  Pixelschrift) ist weg. Der Aushang in der Welt (Cut 5/6, `kb_ventil09_zoom.png`) zeigt das helle Blatt mit den Farbpunkten links neben "09".

### KZ/KD — Abnahme-4-Mangel 1: Dokument-Zettel neben der Sicherung erkennbar + Leser (Leon `z5/z6/z7_mit|ohne`, `kd`; Elza `kd_elza`)
* Paare mit/ohne Bit 107, RE15_FORCE_CUT nur zur Bildwahl, F330: **Cut 5: 243 Pixel verschieden, Rahmen x 104..116 y 72..91**; Cut 6/7: 0
  (dort ausserhalb des Bildes). Bild **`<a5>/bilder/z5_dokument_mit_ohne.png`** (2x + 6x-Ausschnitt): links vom gelb-schwarzen Pfosten, neben
  dem weissen Schrank der Sicherung, haengt ein heller Zettel mit Schriftzeilen - als beschriebenes Blatt erkennbar (vorher 39 dunkle Pixel
  am Boden). Rechts vom Pfosten der Aushang (Plakat).
* `kd`/`kd_elza` (Spieler (-4850,7280) Blick +z, RE15_PRESS): Leser "TECHNICIAN'S NOTE" 1/3, Text 2/3 und 3/3 (Nutzertext), EXIT, "The
  Technician's Note has been filed." -> **`Dokument 6 "Technician's Note" abgelegt: Bit (9,107) = 1`** (Leon und Elza). Bild
  `<a5>/bilder/kd_dokument.png`.

### KH — Nachtrag 18 Punkt 2: 2050-Hebel gehen beim Umlegen runter (Leon `kh`, Elza `kh_elza`; Eingabeskript der Abnahme 4, Framedump alle 3 Bilder)
* `2050_pult_ein` -> 43 -> 41 -> Cut 15 -> `Hebel 3 -> EIN ... 40 V` -> `Hebel 1 -> EIN ... 70 V` -> `Hebel 4 -> EIN ... 80 V` -> `"Power supply OK."`
  -> `GELOEST: 80 V -> Bit 106` (Elza `room=2051` gleich).
* Bild: Abweichung der Hebelbereiche gegen F522 (`<a5>`-Messung): Hebel 3 waechst F552..F570 von 0 auf 16.8 (= 16 Bilder Bewegung), Hebel 1
  F603..F639, Hebel 4 ab F690; 3x-Ausschnitt **`<a5>/bilder/kh_hebel3_zoom.png`** (F546..F570): der silberne Griff wandert in ~16 Bildern
  von oben ueber den Hebelkoerper nach unten, die gruene Lampe bleibt darunter; Uebersicht `kh_pult.png` / `kh_elza_pult.png`: F600 Hebel 3
  unten, F729/F732 Hebel 1/3/4 unten, Hebel 2 oben. (Am oberen Ende des umgelegten Hebels bleibt ein leicht hellerer Fleck des
  ausgemalten Grundes - Schoenheitsfehler, kein Mangel.)

### KN/KG — Nachtrag 16 Folgen 3/6: nach der Flut Herausnehmen und Weg zum Generator 2060 zu Fuss (Leon `kn`, Elza `kn_elza`; `kg`/`kg_elza`)
Zustand nach der Flut (Bits 101/102/103/100, (3,108), (3,235) Szene gelaufen, 106, (3,109)/(3,113)/(3,114)/(3,219)) + Bit 125 (Sicherung steckt),
Wegplan = der Plan des Riegels k2 (`<a5>/plaene/kn.txt`, eigene Laeufe):
* Leon: `loaded room20b0.rdt` -> `DOOR FIRE slot=2 ... spawn=(-4600,0,-4950)` -> `loaded room2010.rdt` -> `Ventil-Sicherungskasten Slot 8 ...,
  Sicherung steckt (Bit 125)` + `ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> `2010_kasten_drin` -> **`id=43 "The fuse is in
  place.Will you take the Fuse?"`** -> Ja -> **`WRITE Set zone=9 idx=125 op=0`** -> **`id=44 "You've taken the Fuse."`** -> `DOOR FIRE slot=0 ...
  target_cut=10` -> `loaded room20b0.rdt` -> Leiter `DOOR FIRE slot=4 ... spawn=(9100,-5400,4100)` -> `loaded room2050.rdt` -> slot 1 ->
  `loaded room2020.rdt` -> slot 1 -> `loaded room2030.rdt` (`unter Wasser (y -7200)`) -> slot 3 -> `loaded room2060.rdt` -> Generator-Kasten:
  **`id=3 "One of the fuses is missing..."` -> `id=4 "Will you use the Fuse?"`** (Original sub18; Wegplan `FLAG (9,125)` 1 -> 0 -> 0 -> 0).
* Elza `kn_elza` Zeile fuer Zeile gleich (`room20b1` -> `room2011` 43/44, Bit 125 = 0 -> `room20b1` -> `room2051` -> `room2021` -> `room2031` ->
  `room2061` `id=3` -> `id=4 "Will you use the Fuse?"`).
* `kg`/`kg_elza` (Sprung nach 2060 mit (3,108) + Bit 125): **`ROOM2060: Sicherung steckt im Ventilkasten 2010 (Bit 125) -> Generator-Kasten
  Slot 9 = Text 3 (@0x010A6)`** -> `id=3 "One of the fuses is missing..."` (2x), keine Frage, kein `idx=144 op=1` (Elza `room=2061` gleich).

### Standbild = BSS/ROOM2030/BG07.BSS (Gegenprobe ohne das Werkzeug der Spur)
* Standbild aus `ka3` F760 gegen das Spielbild ROOM2030 Cut 7 (`z7_mit`, RE15_FORCE_CUT=7, Leon im Bild): **91.4 % der Pixel gleich, 95.4 %
  mit Abweichung <= 6**; die Abweichungen sind nur Leon und das Ring-Gelaender (im Spielbild mit den Vordergrund-Masken neu gezeichnet), Bild
  `<a5>/bilder/standbild_vs_cut7.png` (links Standbild, Mitte Spiel, rechts Abweichung rot). Das Standbild ist der Hintergrund der Ventil-
  Kamera ohne Figuren.

### P4/P5/P6 — Punkte 4/5/6 (Regression; Wegplaene der Abnahme 4, Leon und Elza)
* P4 (`p4`/`p4_elza`, Bits 103/100 + (3,235)): 2010 `Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> **`loaded room20b0.rdt`** (Elza
  `room20b1.rdt`) -> Leiter **`DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)`** (kein Sperrtext) ->
  `loaded room2050.rdt` -> 2050-Leiter `Tuer Slot 0 -> ROOM20B0 Cut 9` -> 20B0. Bild `<a5>/bilder/p4_20b0.png` (Cut 9): Gitterfenster ohne
  Wasserstrahl, Spieler an der Leiter. (Laeufe per Zeitlimit beendet - Plan ohne EXIT_AT, kein Spielbefund.)
* P5 (`p5v`/`p5v_elza`): Hebel ohne Pult -> `2050_hebel_sperre` -> **`id=40 "The pump has no power. The switchboard must be set first."`**,
  `FLAG (3,104) = 0` vorher und nachher; Pult-Loesung siehe KH (`Bit 106`).
* P6 (`p6`/`p6_elza`, nach der Flut mit Strom ueber die echte 20B0-Ost-Tuer): **`ROOM2030: Strom (3,126) -> Wasser knapp unter der Absperrung
  y -5350`**, `[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)`, Treppe `id=48 "I cannot go down the stairs because of the water."`, Lift B
  `FLAG (3,220)` 0 -> 1, 2080 Rechner = Original **`id=0 "From here maybe I can control the water level.Will you operate the computer?"`**,
  `FLAG (3,245)` 0 -> 1 -> `loaded room2090.rdt` (Elza 2031/2081/2091 gleich). **Unveraendert gegen Abnahme 4.**

### KBR — Punkt 2 + Nachtrag 9 auf dem echten Weg 2070 slot 1 -> 2030 Ring (Leon `kbr`, Elza `kbr_elza`; Vorlage Abnahme-4-Plan k1 bis zum Steg)
* `DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` (Elza `room2031.rdt`) ->
  **`ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt`** (`ZELLE r0[12] ... u0 0xff`) -> Pult (Ring Nordwand) `2030_pult_ohne` ->
  **`id=40 "A control panel for the bridge. The pressure gauge reads zero.Intake valve 13 is closed."`** -> Treppe -> Nordventil 01 VOR der
  Bruecke -> `2030_ventile_vorher` -> `id=49 "It's locked."` (Nachtrag 9: kein Ventil vor der Bruecke) -> Ventil 13 -> **`id=41 "Intake valve
  13. Will you open it?"`** -> Ja -> `id=42` -> `FLAG (9,101) = 1` -> Treppe -> Pult `2030_pult_senken` -> **`id=43 "The pressure gauge reads 80.
  Lower the bridge?"`** -> Ja -> `[prop-member F2773] prop[0] obj=0x00 m=1 -> val=-1800` (Elza F2784) -> `FLAG (9,102) = 1` -> `id=44 "The
  bridge is down."` -> Treppe -> `ZELLE r0[12] ... u0 0x00` -> Spieler auf dem Steg (-1042,-2009) (Elza (-1041,-2104)). Bildbogen
  **`<a5>/bilder/kbr_bruecke.png`**: Pult mit Satz 40 (F370), "Lower the bridge? Yes/No" (F2650), Cut 4 Draufsicht mit der fahrenden Bruecke
  (F2740), "The bridge is down" (F3070), Leon auf dem Steg (F3850/F4000). Die Kurbel 2020 ist nicht beteiligt. **Unveraendert gegen Abnahme 4.**

### KE — `integration_r41_ventilkette_kette` von Hand (C-E1; im C-Baum nicht registriert)
* configure: `-- integration_r41_ventilkette_kette uebersprungen: Spur A fehlt (engine/src/wasser20b0_r41.c, AUTO-Haken
  re15_wasser20b0_r41_auto_aot)`. Von Hand (`cmake -DRE15_PC_EXE=... -DRE15_SRC=re15_port -DWORKDIR=<a5>/kette_hand -P
  test_r41_ventilkette_kette.cmake`, `<a5>/kette_hand.txt`): **`r41_ventilkette_kette: 2030 richtige Folge 09/07/02/01 zu Fuss -> Warnung ->
  Ost-Tuer zu Fuss -> ROOM20B0, Bits 103 + 100; Teil ab der Szene uebersprungen: Haken der Spur A fehlt`**, rc=0 (~3,5 min).
* Flut-Programm: die Bytes von `re15_r41c_prg_2030_ventile_flut` sind ausser dem Kommentar der Message_on-Zeile gleich dem Stand der Abnahme 4
  (md5 des Blocks ohne die Message_on-Zeile beidemal `ec0d5d43...`) -> die in Abnahme 4 gemessene Flut UEBER die Ringebene (Lauf `flr`:
  `ERTRUNKEN: Wasser y -6300, Spieler y -5400`) gilt unveraendert; am Landeplatz neu gemessen (KF).

## 2. Gates

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung, `<a5>/py/re2_1080.py`, `<a5>/py/re_stich.py`)
1. **RE2 ROOM1080 sub08 @0x0210A..@0x0221E** (Rohbytes aus `info/re2leon/PL0/RDT/ROOM1080.RDT`, eigene Lesung + Gegenlesung mit
   `re2_scd_dump.py`): `3f 00 0f 00` @0x0213C, `3f 00 10 00` @0x02156, `3f 00 11 00` @0x02186 in For 2 `0d 00 2c 00 02 00` @0x02178, @0x021B4 in
   For 2 `0d 00 1a 00 02 00` @0x021AE, einzeln @0x021CE = **5 Drehtakte**; se_on `36 02 05 01 01 00 70 17 38 ff 00 00` @0x02168 + 2 + 2 + @0x021D6
   = **6 Toene**; Sleeps 20+30+28+22+4x(18+12)+18+15+12+50+30 = **345**; rueckwaerts `3f 00 10 c0` @0x021EC / `3f 00 0f c0` @0x021F6 nach
   `5b 32`/`5b 1e` (plc_cnt), plc_neck @0x021FE/@0x0220C, cut_be_set `61 05 01 00` @0x0221A, plc_ret `42` @0x0221E - **bestaetigt**; die
   Port-Folge (programme.py) bildet genau das nach. Kopfwort [22] @0x60 = 0x39AC; `VENTIL1080.RBJ` (13128 B) == RDT[0x39AC..]; EDD 19 Clips,
   15/16/17 = **30/50/40** Bilder; `VENTIL1080.EDT/.VH/.VB` == RDT @0x6CF4/@0x6DB4/@0x79D4; EDT-Satz 5 = `00 00 54 16` - **bestaetigt**.
   Nicht uebernommen (Dossier B5-3 nennt nur plc_neck/cut_be_set/plc_cnt): der Else-Zweig `Ck(1,0)` (@0x0210E/@0x02134/...: Pos_set
   (-19502,0,-21200) und Plc_motion mit drittem Byte 1) - der Port nimmt fuer Leon UND Elza den ersten Zweig; im Bild
   (`kb_elza_ventil09_zoom.png`) passt die Hand ans Rad (Hinweis, kein Mangel).
2. **RE2 EXE @0x800515f8** (`ghidra_re2_Leon.txt` Z. 169493): `8a 6e 01 0c jal FUN_8005ba28` mit `lui a0,0x226` @0x800515f4 (Delay `clear a1`)
   im Zweig `s0 == 0xFE` der Tuer-Schloss-Routine (0xFF -> `lui a0,0x216` @0x8005160c) = SE-Aufruf **Satz 0x26** - **bestaetigt**.
3. **RE2 ROOM6040 @0x022E8** `57 00 04 06 11 25 00 80` und @0x022F0 `57 00 05 06 11 25 00 80` (Sce_bgmtbl_set -> SUB 0x25); `info/re2leon/
   COMMON/SOUND/SUB_25.BGM` 100688 B, RE1.5 `SOUND/SUB_25.BGM` **4 B** - **bestaetigt** (Grundlage der offenen Alarm-Frage).
4. **ROOM11F0 @0x01332** `2f 05 40 00`, **@0x01336** `0d 00 04 00 10 00`, **@0x00E76** Obj_model_set rot_z = **512** - **bestaetigt** (16 x 64 =
   1024 = 90 Grad; Hebel in 16 Bildern).
5. **ROOM2040 msg 4 @0x19A6** `... 05 02 0d 0c 00 4f 41 3f 4b 4a 40 4f 05 00 ...` = "<Farbe 2>10 seconds<Farbe 0>" - **bestaetigt** (Farbe 2 der
   Countdown-Ziffern). **DEBUG.BIN @0x4416** + Glyph: '0' 8, '1' 7, '2'..'9' 8, ':' 4 px - **bestaetigt**.
6. **ROOM2060 @0x0109E..@0x010E2**: `06 00 1c 00` `21 03 6c 00` Ck(3,108)=0 -> `2c 09 01 31 ... 03 00 ff ff` Slot 9 Text 3; Else `21 03 90 00`
   Ck(3,144)=0 -> `2c 09 03 31 ... ff 00 18 12` Ereignis 18; sonst Text; **sub19 @0x0169E** `22 03 90 01` - **bestaetigt** (Form des Kastens).
7. **ROOM20A0 msg 1 @0x203C** `04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00` = "It's locked."; **ROOM10D0 msg 17 @0x21BD** = eine Seite mit zwei
   `08` (drei Zeilen) - **bestaetigt**. **ROOM2010 slot 0 @0x00746** (Rechteck (-5800,-5800,1000,1750) -> 2000 (-26450,-1800,-2700) Cut 14) und
   **ROOM2000 slot 2 @0x0165E** ((-27600,-3650,1000,2000) -> 2010 (-4600,0,-4950)) - **bestaetigt**. ROOM2010/2011/2030/2031: **0** Ck/Set auf
   Bank 5 im Original-SCD (`scd_dump_room.py`) -> die Marker (5,0x1A)/(5,0x1F) sind frei - **bestaetigt**.
* Rate-Woerter im Spur-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME`): **0 Treffer** (nur "ROOMxxx1" als Fehltreffer).
  Neue `getenv` nur `RE15_SE_DEBUG` (Protokoll). Jede neue Konstante im Kopf mit @0x.../Byte-Offset oder als PORT-WAHL markiert (Kasten-Platz,
  Dokument-Lage, Countdown-Format/-Lage, Satz "A fuse box for the valve controls.").
* Erklaert der Fix den Befund? Mangel 1: Ursache flaches Blatt am Boden -> Zettel aufrecht an der Wand, gemessen 243 statt 39 Pixel; Mangel 2:
  Ursache unbedingtes Zuruecksetzen (2,7)/(2,2) in plakat_tick und das Plakat liess sich in Zaehler/Flut oeffnen -> Bedingung + Sperre, gemessen
  (KF); Mangel 3: je Nachtrag umgesetzt bis auf den Alarm. **G1 haelt.**
* Doku-Rest (kein RE-Verstoss): der Kopf `include/re15_ventilkette_r41.h:147` beschreibt die Ventil-Bedingung noch als "Bit 102 UND (3,108)
  (Nachtraege 9/10; vorher Satz 49 'I should look around here first.')" - der Code prueft seit Nachbesserung 5 Bit 102 UND Bit 125, Satz 49 ist
  "It's locked." (ventilkette_r41.c re15_ventilkette_r41_ereignis); Z. 160 "Blatt am Boden neben der Sicherung" (seit Nachbesserung 5 ein Zettel an
  der Wand; im selben Absatz weiter unten richtig) -> Mangel 2 (Doku, niedrig).

### G2 Vertrag (VERTRAG.md §1/§2)
* **Bits:** 100-103/106/107 + **125** (Ventilkasten). 125 nach Nachtrag 16 Folge 5 ("naechste freie nach Zensus, im VERTRAG nachtragen"): Zensus
  (original frei 110..133, 110-124 an F/G/E/J/K vergeben) -> 125, im Spur-VERTRAG §1.1 nachgetragen, k_port[] in test_r36_kartenbits.c
  (`unit_r36_kartenbits` gruen). Kein anderer r41-Baum belegt Bank-9-Bit 125 (die k_port[] der anderen Baeume haben keine R41-Zeilen; (3,125)
  in re15_elzaintro_r38.h ist Bank 3). Der master-VERTRAG sagt weiter "125-133 vergibt der Orchestrator" -> beim Merge eintragen (Hinweis V2).
* **Text-Ids:** 2010/2011 40-44 (VERTRAG 1.2, Nachtrag 16 Folge 5), 2030 40-55 - eingehalten.
* **Pfade** (`git diff master...HEAD --name-only`): keine `release/`, `platform/android/`, `shared_assets/PSX/`, `pri/`, keine Test-CMakeLists;
  `synchro/STAGE2/room2030|room2031/main46.wav` kommen aus dem Orchestrator-Commit f091aa04, Blob `2f750a04...` == master (3da82407) - kein
  Eingriff der Spur. Neue RE2-Daten `shared_assets/RE2/VENTIL1080.{RBJ,EDT,VH,VB}` (Dossier §7 fuers Paket-Gate genannt).
* **Gemeinsame Dateien:** game_step_common.c 1, scd_room_setup.c 2, scd_vm.c 4 (Kommentar geaendert), re15_files.c 10, test_r36_kartenbits.c 8,
  test_r36_dachkiste.cmake 2, test_r37_itemband.cmake 4 - wie in C-E3 angenommen. **platform/pc/main.c** jetzt 13 Zeilen an 6 Stellen, davon **zwei
  neue Haken** (RBJ-Leihe `re15_ventilkette_r41_pc_rbj` im Boot- und im Tuer-Ladeweg, je 2 Zeilen, markiert "Runde 41 Spur C"); der fruehere
  Pult-/Plakat-Block ist durch EINEN Aufruf `re15_ventilkette_r41_pc_bild()` ersetzt. C-E3 hat "4 Stellen" angenommen -> die zwei RBJ-Haken
  brauchen die Bestaetigung des Orchestrators (Hinweis V1, formal; Muster wie J-E1). Neue Datei platform/pc/src/ventilkette_r41_pc.c = eigene
  Datei der Spur (GLOB).
* **Tests je Punkt:** Unit K (Ventile ohne Bit 125), N (Kasten), O (2060 mit Bit 125), P (Plakat in der Flut), Q (Animation/Toene/Lage), F (Hebel
  0..16), D (+345), A (Text 46); Integration v2a/v2b/v2c/v2d/v2p/k1/k2/k3 je Figur, Haupt A-E (E = Mangel 2). Alle gruen (§3). Neue Tests
  parallel-fest (eigene exe-Praefixe/Laufordner), `cmake_minimum_required` in jedem -P-Skript, `.exe` nur bedingt, keine Backslash-Pfade,
  re15_pc linkt `m` (cos/lround in ventilkette_r41_pc.c). Kein Riegel fuer den Alarm (nicht gebaut).
* **G2 haelt** bis auf die formalen Hinweise V1/V2.

### G3 Nebenwirkungen (beide Szenarien, Nachbarraeume)
* Alle geaenderten Punkte mit Leon UND Elza gemessen (§1). Echte Tueren 2000/2010/2020/2030/2050/2060/2070/20B0/2080/2090 in beiden Szenarien.
* RBJ-Bindung 2030/2031: das Original-SCD von ROOM2030 hat **kein** Plc_motion (nur Plc_ret @0x01EC0/@0x01F9A) -> keine Original-Szene greift
  auf die RE2-Clips; Spielerbewegung, Treppe, Lifte, Bruecke unveraendert (KBR, P6). Leon und Elza behalten ihre Proportionen.
* Kamera nach RE15_DEBUG_JUMP in 2030 bleibt Cut 0 - vorbestehendes Sprungartefakt (gleich in Abnahme 4 mit der alten exe), nicht Spur-C-Code.
* Raumgruppe 17/17 und Unit-Gruppe 261/261 gruen (§3). **G3 haelt.**

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe (`<a5>/regex_unit.txt`, 261 Tests, -j 2): **`100% tests passed, 0 tests failed out of 261`** (67.44 s); `unit_r41_ventilkette`
  **`PASS (517 Pruefungen)`**.
* Integration r41 (zwei Stroeme, `<a5>/ctest_integ1.txt`/`ctest_integ2.txt`): `integration_r41_ventilkette` **Passed 731.27 sec**,
  `integration_r41_ventilkette_weg` **Passed 1498.20 sec**, `integration_r41_ventilkette_b` **Passed 536.04 sec**,
  `integration_r41_ventilkette_weg_elza` **Passed 1736.83 sec**, `integration_r41_ventilkette_sperre` **Passed 307.62 sec** - 5/5.
* Raumgruppe (`<a5>/regex_integ_raum.txt` = Abnahme 4, 17 Tests: fahrstuhl_n1, spinnen_echtlauf, itemband(_lift), gegnerzahl, wasser(zombie),
  kiste(2feld/regel), dachkiste, dokumente_bild, tagebuch_lesen/karte/leon, irons_tisch_laden/bild): **`100% tests passed, 0 tests failed out of
  17`** (`<a5>/ctest_integ_raum.txt`).
* `_kette` von Hand rc=0 (KE). Kein roter Test, nichts nachzufahren.
* Hinweis (Laufzeit): die fuenf r41-Riegel laufen RUN_SERIAL zusammen ~80 min (Zeitgrenzen jetzt 1500/1200/3600/3600/1500 s) - fuer die
  Release-Suite einplanen.
