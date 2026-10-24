# Runde 41 — Spur C "ventilkette" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, HEAD 49606c0e (Basis master 116e8b0f).
Begonnen 2026-10-24 03:17 (Uhr). Pruefer: Abnahme 0 (Opus). Der Bericht wird FORTLAUFEND geschrieben.
Messordner: scratchpad `abn0/` (Pfade unten absolut abgekuerzt als `<abn0>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn0`).

## 0. Stand / Protokoll (neueste zuerst)
* 04:31-05:17 Querbezug-Riegel (die vom Diff geaenderten fremden Riegel + Heliport wegen Bank 5), seriell,
  `--timeout 1800`, `<abn0>/ctest_integ_quer.txt`: integration_r36_dachkiste Passed 1395.34 s,
  integration_r37_itemband Passed 825.42 s, integration_r37_kistenregel Passed 273.35 s,
  integration_r38_heliport1171_weg_re2/_weg_re15/_laden Passed 59.67/59.19/44.67 s, integration_r39_heliport1171_weg/
  _laden Passed 59.79/45.16 s -> `100% tests passed, 0 tests failed out of 8`, EXIT=0.
* 03:30-05:22 eigene Messlaeufe L1-L9 an der exe (§1; L9 nach dem Querbezug-Lauf, ohne Parallel-Last).
* 03:27 Gezielte Unit-Gruppe (eigene, breitere Auswahl, Regex `<abn0>/regex_unit.txt`; zusaetzlich zu den 37 des
  Bau-Agenten: Heliport 1171 (Bank-5-Bits, s. §2 G3), Tueren r30/r33/r36, Fade, msg, espr, r31/r34n Cursor/Hebetisch,
  Panel 11F0): `100% tests passed, 0 tests failed out of 145` (15.98 s, `<abn0>/ctest_unit.txt`).
  `unit_r41_ventilkette -V`: `== Leon`, `== Elza`, `PASS (239 Pruefungen)`.
* 03:18-03:26 `ctest -R "^integration_r41_ventilkette(_b)?$" --timeout 1200` (exe 02:51): `integration_r41_ventilkette
  ... Passed 212.47 sec`, `integration_r41_ventilkette_b ... Passed 293.63 sec`, `100% tests passed, 0 tests failed
  out of 2` (506.14 s, `<abn0>/ctest_integ_r41.txt`).
* 03:17 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `local_build.sh build` -> `ninja: no work
  to do.` / `=== LOCAL-BUILD-OK (build)`; exe 02:51 (7798220 B). Geprueft: seit 271b05ea keine Aenderung unter
  re15_port (`git diff 271b05ea HEAD --stat -- re15_port` leer); aot_common.c = Basis (`git diff --quiet` ok),
  Quelle 02:47, Objekt 02:51:56 -> der temporaere Spur-A-Haken ist NICHT in der exe.

## 1. Messungen an der exe (eigene Laeufe; exe-Kopie je Lauf `re15_pc_abn0c_<lauf>.exe`, danach geloescht)
Laeufe heissen L1..L9 (nach Nutzer-Punkt), Maengel M1..M9 (§4).
Runner `<abn0>/lauf.sh` (env + `timeout`, Arbeitsordner `<abn0>/mess/<lauf>/`, debug.log/state.txt dort).
Kalibrierung (`<abn0>/mess/kalib`, 2030): Drehen R = +96 je Bild, Gehen ~71 je Bild, Laufen ~191 je Bild;
Blick 0 = +x, 1024 = -z, 2048 = -x, 3072 = +z (state.txt F330..F447).

### L1 — Punkt 1 Leiter 2000 zu Fuss (`<abn0>/mess/leiter`, `<abn0>/mess/leiter10`)
Aufbau: `RE15_DEBUG_JUMP=2000@240`, `RE15_PLAYER_POS=-24500,10300,2048` (rund 2 m vor der Leiter), Eingabe
`U0.8,W0.5,A0.1,W5,A0.1,W3` (hingehen, Aktionstaste). Zweiter Lauf gleich mit `RE15_FORCE_CUT=10` (nur Bildwahl).
* debug.log: `[ventilkette41] ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40 (Form ROOM2030 @0x01B12)` ->
  state.txt F354 Spieler an der Leiterwand `PL(-26532,10332)`, F369 Aktion -> `msg(a=1 fsm=1 id=40)` ->
  `[msgtext] room=2000 id=40 "I cannot climb up the ladder because of the water pressure."`; bis F660 kein Raumwechsel
  (kein `loaded room2050`). Bildreihe `<abn0>/leiter2000_cut10_sheet.png`: der Original-Wasserstrahl faellt aus dem
  Gitterfenster neben der Leiter (ESP 0x0b @0x0174C..@0x0176C, immer an), Leon an der Leiter, Text Zeile 1/2.
  (Ohne FORCE_CUT bleibt die Sprung-Kamera Cut 0 stehen und Leon ist nicht im Bild — Messartefakt des
  Debug-Sprungs, `<abn0>/leiter2000_sheet.png`.)
* Elza (2001): `integration_r41_ventilkette_b` Lauf K (Text 40 woertlich in ROOM2001) in meinem Lauf gruen (§0).

### L2 — Punkt 2 Bruecke: Ring -> Pult -> Treppe -> Ventil 13 -> Treppe -> Pult -> Fahrt -> Steg, alles zu Fuss
Wegplan-Autopilot `RE15_WEGPLAN` (dachkiste_r36.c: echte Pad-Bits, Kollision), Spinnen als "schon getoetet".
* Lauf `bruecke` (Start Ost-Tuer (6550,-100), Plan `<abn0>/plaene/bruecke.txt`): Ring Ost -> Nord bis vor das Pult
  (490,-9718) Blick 1072 -> Aktion F334 -> `[ventilkette41] ROOM2030 Ereignis 24 -> Port-Programm 2030_pult_ohne` ->
  `[msgtext] room=2030 id=40 "A control panel for the bridge. The pressure gauge reads zero.Intake valve 13 is
  closed."`, `FLAG (9,101) = 0`; weiter zur Treppe (-6356,-7726) Blick 0, Aktion F885 -> Treppe hinab bis
  (-2082,-7717) (Landeplatz Band 1); Versuch auf den Steg nach (-1050,-3000): der Spieler bleibt bei z -6418 haengen
  (state.txt F1250..F4200 `PL(-939..-1195,-6418)`) = Steg-Zelle 12 bei oben stehender Bruecke gesperrt.
* Lauf `bruecke3` (Start Landeplatz (-1107,-7717) Band 1 = Endlage der Treppe aus Lauf `bruecke`, Plan
  `<abn0>/plaene/bruecke3.txt`): Ventil 13 (-1430,-6498) Blick 2032 -> `Ereignis 25 -> Port-Programm 2030_v13_frage` ->
  `[msgtext] room=2030 id=41 "Intake valve 13. Will you open it?"` -> Ja -> `id=42 "You've opened valve 13. The
  pressure is rising."` -> `FLAG (9,101) = 1`; Treppe HINAUF von (-2049,-7689) Blick 2048 (Aktion F572 -> F872 oben bei
  (-6404,-7682)); Ring zum Pult (236,-9636) -> `Ereignis 24 -> Port-Programm 2030_pult_senken` -> `id=43 "The pressure
  gauge reads 80. Lower the bridge?"` -> Ja F1220 -> `[scd F1221] Cut_chg(4)` -> `[prop-member F1281] prop[0] obj=0x00
  m=1 → val=-1800` (30 x 2 Takte) -> `[scd F1315] letterbox closed` -> `FLAG (9,102) = 1` -> Aktion ->
  `2030_pult_unten` -> `id=44 "The bridge is down."`; Treppe hinab und ueber den Steg bis (-1043,2989) (Befehl 37
  "angekommen") = der Steg ist jetzt begehbar. Bildreihe `<abn0>/bruecke_kette_sheet.png` (Texte, Cut 4: Planke sinkt
  F1230/F1260/F1290). Kamera bleibt sonst bei Cut 0 (Start per PLAYER_POS ausserhalb der Cut-0-Zone; Messartefakt,
  im Lauf `bruecke` mit Start an der Ost-Tuer schaltete die RVD-Kamera normal, cam 3 auf dem Landeplatz).

### L3 — Punkt 3 echter Weg: Ventile -> Warnung -> ZU FUSS durch die Ost-Tuer -> 20B0 (`<abn0>/mess/osttuer`)
Aufbau: `RE15_DEBUG_JUMP=2030@240`, Spinnen als "schon getoetet" (`RE15_SET_FLAG=7:155,7:156,7:157`, Messmittel wie
beim Bau-Agenten), `RE15_PLAYER_POS=6550,-100,2048` (vor den Ventilen), Eingabe
`A0.1,W4,A0.1,W2.5,A0.1,W4,A0.1,W4,A0.1,W4,R0.7,W1,A0.1,U3` (Aktionstaste, Ja, Warnung zu, UMDREHEN, Aktionstaste an
der Ost-Tuer — KEIN RE15_FIRE_AOT), `RE15_EXIT_AT=1100#20B0`, Framedump alle 50 Bilder.
* debug.log: `[ventilkette41] ROOM2030 Ereignis 26 -> Port-Programm 2030_ventile` -> `[msgtext] room=2030 id=45
  "Numbered valves. They control the flood gates of the treatment pool.Will you turn them?"` -> `[msgtext] room=2030
  id=46 "WARNING: The water will rise in one minute. Leave the room!"` -> `Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler
  laeuft)` -> state.txt F900..F950 Drehung 2048 -> 4064 (zu Fuss), F951 Aktion -> `[aot] DOOR FIRE slot=0
  rect=(7350,-200,hw=500,hh=1750) target_cut=11 spawn=(5950,-1800,-6400)` -> `[room] PC loaded room20b0.rdt` ->
  `[scd F951] Cut_chg(11)` -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100` -> `Plc_dest(slot=0 mode=0x05
  dest=(-15920,-6355))`.
* **20B0 haengt in diesem Baum (Softlock)**: state.txt ab F109 `PL(-15820,-6355,rot=2047)` pst 4, `cam=14`, `pm=2`
  bis F1100 (~36 s); die nachgespielten Eingaben R (F900-F920) und U (F954-F1043) drehen/bewegen den Spieler NICHT;
  kein `thread start` fuer sub05, kein `letterbox closed`. Bildreihe `<abn0>/osttuer_20b0_sheet.png` (F50/F100 Lauf
  nach Westen, F150..F1100 Leon steht vor der West-Tuer). Bestaetigt den Querbezug A des Dossiers (§6): die
  Auswasch-Szene laeuft hier erst mit dem AUTO-Haken der Spur A (Zweig r41/wasser20b0, aot_common.c
  `re15_wasser20b0_r41_auto_aot`, im Zweig A vorhanden, hier NICHT).

### L3b — Punkt 3 Zeit abgelaufen im Raum: Flut ueber den Ring und Tod (`<abn0>/mess/flut2` Software, `<abn0>/mess/flut3` Direct3D)
Aufbau wie L3, aber nach der Warnung stehen bleiben (`...,A0.1,W2,R0.4`), `RE15_EXIT_AT=2900`, Framedump 2560-2900/6.
* state.txt: Warnung zu F778 (pf FFFF0007 -> 00000007) -> **F2579 Flut** (cam 4, pf 21000007 = Pad-Sperre, pm 2) =
  778 + 1801 -> F2686 `fx=1` + debug.log `SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400), Effekt
  0x06050D00` (genau einmal) -> **F2704 `hp=-1`** = 125 Bilder nach Flutbeginn + `ERTRUNKEN: Wasser y -6300, Spieler
  y -5400 -> hp -1, blaue Blende`, fade-log `config ch=0 abr=2 mask=6 step=0 bucket=7` / `kick ch=0 value=0x5000`
  -> YOU DIED. Gleich in beiden Renderern.
* **Bild (Direct3D = was der Nutzer sieht, `[render] SDL-Renderer direct3d ... ABR2 (B-F) ok`)**: Bildmitte F2698
  (30,37,30) -> F2704 (0,0,30) = dunkelblauer Schleier, F2716 (0,0,30); Reihe `<abn0>/flut_d3d_sheet.png` (Wasser steigt
  unter Cut 4 ueber die Planke, blauer Schleier, YOU DIED). Die Behauptung des Dossiers (§4 "Blende") stimmt fuer
  Direct3D.
* **Bild im Software-Renderer der Riegel (`RE15_SOFTWARE_RENDER=1`, `[render] SDL-Renderer software ... ABR2 (B-F)
  NICHT unterstuetzt -> Rueckfall BLEND`)**: ab F2704 der GANZE Schirm deckend (160,160,0) GELB bis YOU DIED
  (`<abn0>/flut_sheet.png`). Dasselbe in ALLEN Framedumps F2750 der integration_r41_ventilkette C seit der
  "Blau-Korrektur" (build/tests/integration/r41_ventilkette/lauf_a16acfeb_c 22:23, lauf_d7a34b68_c 23:46,
  lauf_912f1ca0_c 00:50, lauf_9d92c55a_c 03:21 = mein Lauf: Mitte und Ecke (160,160,0)). Der Riegel prueft nur, dass
  die Datei existiert — er hat den gelben Schirm nie bemerkt. (Ursache: der Software-Renderer kennt ABR2 nicht; die
  Kachel faellt auf BLEND mit Alpha 255 = deckend. Vorbestehende Renderer-Grenze, aber der Riegel belegt das Bild nicht.)
* Der Spieler steht an den Ventilen (Ost-Streifen) ausserhalb der Flut-Kamera Cut 4; der eine Spritzer an seinen
  Fuessen ist im Bild nicht zu sehen (F2686/F2692 nur Wasser + Planke). Sichtbar ist: Wasser steigt, blauer Schleier,
  Tod (Orchestrator-Entscheid C-e "Tod + blaue Blende + Spritzer").

### L4 — Punkt 4 nach der Flut: 20B0 statt 2000, Leiter frei, kein Strahl (`<abn0>/mess/nachflut`, `<abn0>/mess/strahl20b0`)
Zustand "Szene gelaufen" per `RE15_SET_FLAG=3:235,9:103,9:100` (die Szene selbst haengt in diesem Baum, L3), dann
ALLES zu Fuss/Aktionstaste (Wegplan `<abn0>/plaene/nachflut.txt`):
* ROOM2010 (Sprung-Spawn (-4600,-4950) an der Tuer nach 2000): `[ventilkette41] ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut
  10 (Ziel ROOM2000)` -> Blick 2048 + Aktion -> `[aot] DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=10
  spawn=(-26450,-1800,-2700)` -> `[room] PC loaded room20b0.rdt` (nicht 2000).
* 20B0: zu Fuss zur Leiter (-25986,10227), Blick 2016, Aktion -> `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,
  hh=500) target_cut=0 spawn=(9100,-5400,4100)` -> `loaded room2050.rdt` (kein Sperrtext) = Leiter frei.
* 2050: `Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel ROOM2000)` -> Blick 3072 + Aktion -> `DOOR FIRE slot=0 ... target_cut=9
  spawn=(-26450,-1800,10250)` -> `loaded room20b0.rdt`.
* Bild `<abn0>/leiter_vergleich_sheet.png`: links 2000 Cut 10 (Strahl aus dem Gitter, Text 40), rechts 20B0 Cut 9 mit
  Bit 100 (kein Strahl; state.txt `fx=0` ueber alle 300 Bilder).

### L5 — Punkt 5 ROOM2050 Pult -> Hebel (`<abn0>/mess/hebel_vorher`, `pult`, `hebel_nachher`, `kisten_hebel`)
* `hebel_vorher` (Wegplan `<abn0>/plaene/hebel_vorher.txt`, Start vor dem Pult (4900,-7100), zu Fuss um die Saeule in
  die Nische (4291,-11139), Blick 64, Aktion): `[ventilkette41] ROOM2050 Ereignis 4 -> Port-Programm 2050_hebel_sperre`
  -> `[msgtext] room=2050 id=40 "The pump has no power. The switchboard must be set first."`, kein Cut_chg(4), kein
  prop-member, `FLAG (3,104) = 0`. (Der Hebel IST zu Fuss erreichbar — der Bau-Agent hatte ihn per FIRE_AOT gefeuert.)
* `pult` (Eingabe = `_pult_skript` des Riegels _b, echte Steuerkreuz-/Viereck-Bits): `Ereignis 9 -> 2050_pult_ein` ->
  `id=43 "A high voltage switchboard. The pump needs 80 volts."` -> `id=41 "Will you push them?"` -> `[scd F534]
  Cut_chg(15)` -> `Hebel 3 -> EIN ... 40 V` -> `Hebel 1 -> EIN ... 70 V` -> `Hebel 4 -> EIN, Schalter 0xD, 80 V` ->
  `id=42 "Power supply OK."` + `GELOEST: 80 V -> Bit 106` -> `verlassen`. Bild `<abn0>/pult2050_sheet.png`; Pixel
  gemessen: roter Laeufer x 165-167 (0 V) / 201-203 (40) / 228-230 (70) / 237-239 (80) = 165.5 + 0.91 * Wert;
  gruene Marken unter Hebel 3 / 1+3 / 1+3+4 (je 24 Pixel), Hebel 2 ohne.
* `hebel_nachher` (wie `hebel_vorher`, aber `RE15_SET_FLAG=9:106` = Pult geloest): kein Port-Programm, Original sub04
  `id=0 "A lever to fill and drain water. Will you move it?"` -> Ja -> `obj=0x04 m=1 → val=-1400`, `Cut_chg(4)`,
  `obj=0x05 ... val=-5000`, `Cut_chg(7)`, `FLAG (3,104) = 1` (`<abn0>/hebel2050_sheet.png`).
* **`kisten_hebel` — DEFEKT**: Zustand "Kisten liegen im Becken" (`RE15_SET_FLAG_AT=5:0,5:1,5:2@300` = die Bits, die
  sub01 beim Einschieben setzt: (5,0) @0x0203C/@0x0205A/@0x02078 nach `2e 03 01` (obj 1), (5,1) @0x02096/@0x020B4/@0x020D2 (obj 2), (5,2) @0x020F0/@0x0210E/@0x0212C (obj 3); Messmittel wie die Kill-Bits), Bit 106 = 0,
  Pult NICHT geloest. sub01 @0x0201A `46 06 03 31 ff 00 18 02` legt den Hebel-Platz Slot 6 auf **Ereignis 2** um
  (Wegplan-AOTS F404: `AOT slot 6 typ 0 ev 2`). Aktion am Hebel -> KEIN Port-Programm (re15_ventilkette_r41_ereignis
  sperrt nur Ereignis 4) -> Original sub02 @0x0220E: `id=0 "A lever to fill and drain water. Will you move it?"` ->
  Ja -> `obj=0x01/0x02/0x03 m=0 → val=-17700`, `obj=0x04 ... -1400`, `obj=0x00 m=1 → val=0`, `Cut_chg(4)`, For 180 x
  Speed -20 (@0x0227C) = **das Wasser steigt** (Cut 4 F670 -> F820: 2645 Pixel im Becken (60,169)-(251,216), Bild
  `<abn0>/kisten_hebel_sheet.png` F760 Wasserflaeche unter dem Gitter) -> `obj=0x05 ... -5000` -> `FLAG (3,109) = 1`,
  `FLAG (9,106) = 0`. => Mit den Kisten im Becken (= dem eigentlichen Wasser-Raetsel des Raums) laesst sich das
  Wasser OHNE das Pult-Raetsel steigen lassen.

### L6 — Punkt 6 Rechner 2080 leert 2030 (`<abn0>/mess/rechner`, `<abn0>/mess/nach2030`)
* `rechner` (Bits 103+100 = Flut gelaufen, Spieler vor dem Rechner (-11185,-7206) Blick 3072, Aktionstaste per
  Wegplan): `[ventilkette41] ROOM2080 Ereignis 2 -> Port-Programm 2080_rechner_erw` -> `[msgtext] room=2080 id=0 "From
  here maybe I can control the water level.Will you operate the computer?"` -> Ja -> `[scd F344] Cut_chg(7)` (Wasser
  2080 sinkt, Original For 148 @0x00CCC) -> `[scd F660] Cut_chg(3)` -> `id=40 "The treatment pool is being drained as
  well."` -> Wegplan `FLAG (9,107) = 1`, `FLAG (3,245) = 1`. (Der anschliessende Fussweg vom Rechner zur 2030-Tuer
  blieb an einer Wand bei x -11932 haengen — mein Wegplan, kein Befund; daher Teil 2 getrennt.)
* `nach2030` (Bits 103/100/101/102/107 + (3,245), Spieler an der 2080-Tuer (-23200,-16700) wie bei der Ankunft aus
  2030, Blick 2048 + Aktion): `[aot] DOOR FIRE slot=0 rect=(-23900,-16700,hw=500,hh=1000) target_cut=9
  spawn=(-6950,-16200,6450)` -> `loaded room2030.rdt` -> KEIN `unter Wasser`-Satz, Props nur obj 0 (Bruecke -1800),
  1, 2, 3 (kein obj 4/5 = keine Wasserflaeche), Lift-Rufzonen Slot 4/6 wieder Typ 11 (Original, Wegplan-AOTS) —
  einzig `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)` bleibt (s. Mangel M7).

### L7 — Elza (`<abn0>/mess/elza_osttuer`, `RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`)
`Spieler-Familie PL04` -> `loaded room2031.rdt` -> `ROOM2031: Bruecke obj 0 oben` -> Ventile `Ereignis 26 ->
2030_ventile` -> `id=45` -> `id=46` -> `ROOM2031: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> zu Fuss
umgedreht, Aktion -> `DOOR FIRE slot=0 ... target_cut=11` -> **`loaded room20b1.rdt`** (Elza-Variante, Szenario-Nibble
bleibt) -> `Cut_chg(11)`, `Bits 103 + 100`, `Plc_dest(... -15920,-6355)` (dieselbe Szene wie Leon, haengt wie L3).
Dazu in meinem Lauf gruen: integration_r41_ventilkette_b J (Elza 2051 Pult + Original-Hebel) und K (Elza 2001 Leiter).

### L8 — Frage (f) nachgemessen: Ring-Eintritt in das geflutete 2030 (`<abn0>/mess/ring`, Direct3D)
`RE15_SET_FLAG=3:235,9:103,9:100` (ohne 107), 20B0 Ost-Tuer per Aktionstaste -> `DOOR FIRE slot=3 rect=(5950,-6400,...)
spawn=(6550,-5400,-100)` -> `ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1` -> obj 4/5 bei
(-10800,-7200,-12400)/(-10800,-7200,3800). Bild `<abn0>/ring_nach_flut_sheet.png`: Leon steht bis zur Brust in der
Wasserflaeche, eine Spinne laeuft ueber/im Wasser; Aktion an den Ventilen -> "The valves are open. The pool has been f..."
— Befund des Bau-Agenten (Frage f) bestaetigt.

### L9 — Punkt 2 Lage des Pults gegen schaltschrank.png (`<abn0>/mess/pult_cut2`, Direct3D, `RE15_FORCE_CUT=2`)
Wegplan `<abn0>/plaene/pult_cut2.txt`: von der Ost-Tuer zu Fuss den Ring nach Norden und Westen bis (490,-9718),
Blick 1072, Aktion F334 -> `Ereignis 24 -> Port-Programm 2030_pult_ohne` -> `[msgtext] room=2030 id=40 ...`.
Bildreihe `<abn0>/pult_cut2_sheet.png` (links das Nutzerbild): Leon steht in Kamera 2 genau vor dem Schaltschrank an
der linken Wand, an der Stelle der roten Marke; rechts der Treppe der Tank mit der "13" (= Ventil-13-Platz am
Treppenfuss, Lauf `bruecke3`).

## 2. Gates

### G1 RE-Gate (Stichproben SELBST disassembliert / gelesen)
* PSX.EXE (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE):
  - @0x800456ac `andi v0,s0,0x80`, @0x800456b8 `lui a0,0x605`, @0x800456bc `ori a0,a0,0xd00` (= 0x06050D00),
    @0x800456d0 `lh a1,106(a1)`, @0x80045710 `jal 0x80019700` — Spritzer-Code wie zitiert.
  - @0x8003253c `andi v0,v1,0x4000` -> @0x80032548 `andi v0,v1,0x1000` ... @0x8003255c `jal 0x80045630` — Fuss-Aufsetzer
    wie zitiert.
  - @0x800218cc/d0 `sll v0,v0,16` / `sra a0,v0,23` = level >> 7 (Helligkeit 0x5000 -> 160) wie zitiert.
  - @0x80042a58 Handler 0x56: @0x80042a70 `ori a3,zero,0x7` (OT-Fach 7), Bytes 1/2/3/4-5 -> `jal 0x800217b0` wie zitiert.
  - @0x8003ed74 `sw zero,4136(at)` = 0x800b1028 (Bank 5 Wort 0, Tabelle @0x80074664[5]) beim Raumladen; @0x8003ec1c
    `sw zero,4140(at)` = 0x800b102c (Wort 1) am Ende jedes VM-Laufs — wie zitiert.
  - @0x8002c24c `addiu v0,v0,-900` — steht in einer OBJEKT-Schleife (Schritt 148 @0x8002c1f8) hinter `lbu v1,8(s1)`/
    `ori v0,zero,0x4`/`bne` (@0x8002c234-3c): die y-Lage von Typ-4-Objekten (Kisten/Cursor) wird um 900 angehoben. Das
    ist KEINE Koerperhoehe des Spielers; RE15_R41C_ERTRINK_TIEFE 900 ist damit ein entliehenes Mass (Ertrinken gibt es im
    Original nicht; der Wert steht im Kopf unter "PORT-WAHLEN") -> Hinweis H1, kein Rate-Defekt.
* RDT-Bytes (eigenes Skript `<abn0>/py/bytes.py`, Ausgabe oben im Lauf): 2000 @0x0169E Slot 4 (`3b 04 02 31 01 00 30
  94 48 26 ... 01 05 00 03`), @0x0174C Strahl, 20B0 @0x019C2 Slot 3 -> Raum 3 Cut 0, @0x01AA6 `06 00 64 03 21 03 eb 00`,
  @0x01E96/@0x01EA6/@0x01EB6 die drei `3a 00 0b ...` (x -23900/-23300/-22700), @0x01FBC `22 03 eb 01 22 02 07 00 22 01
  1b 00`; 2030 @0x01B6C obj 0 (-1075,-1800,-275), @0x01ED2 `2e 03 01 00 2f 01 88 ff 0d 00 06 00 2d 00 30 02 02 00 0e
  00 34 01 d0 d5`, @0x01EF2 `2f 01 78 00 ... 34 01 e8 ea`, @0x01B0A `06 00 1c 00 21 03 db 00 2c 02 01 31 09 00 ...`;
  2050 @0x01D30 `06 00 3c 00 21 05 07 01`, @0x01D38 Slot 8, @0x01D4C obj 8, @0x01E28 Slot 6 Ereignis 4, @0x022E6 sub04,
  @0x023F2 sub09 `29 0f 46 08 ... 22 05 04 01 22 05 05 01 22 05 06 01 01 00`; 2080 @0x00C86 sub02 (108 B, @0x00CC8
  `2f 01 32 00`, `0d 00 06 00 94 00`), @0x00C2E obj 0 `2d 00 03 02 09 00 0a 00 1a 00`; 11F0 @0x0174A `56 00 02 07 00 00`,
  @0x018D8/@0x018F1 msg 1/2 — alle wie im Dossier.
* RE2 (info/re2leon/PL0/RDT/ROOM2130.RDT): @0x0171C `09 0a 1e 00` (Sleep 30), @0x01752 `23 00 05 00 50 00` (var5 == 80),
  @0x01762 `36 02 0c 01` (se 0x0C), @0x01192 `36 02 0a 01` (se 0x0A) — wie zitiert.
* Diff-Suche (`git diff 116e8b0f -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/
  plausib/TODO/FIXME/approx: 0 Treffer; kein neues `getenv` in engine/include/platform.
* Jede verhaltensrelevante Konstante im Kopf re15_ventilkette_r41.h traegt @0x…/Byte-Offset oder steht als PORT-WAHL/
  NUTZER-VORGABE (Bruecke oben -5400, Fahrt-Cut 4, Parkplatz, Wasser -7200, Gewichte 30/20/40/10, Zonen aus den
  Bildpixeln, Blendenmaske/-level, Marken-y 168); Orchestrator-Entscheide C-a..e decken die Wahlen a-e.
=> RE-Gate haelt (mit Hinweis H1).

### G2 Vertrag
* Bank 9: nur 100, 101, 102, 103, 106, 107 (Kopf Z. 121-127), je eine Zeile in k_port[] (test_r36_kartenbits.c),
  unit_r36_kartenbits gruen. Text-Ids: 2000/20B0 40, 2030 40-48, 2050 40-44, 2080 40 — innerhalb der Zuteilung.
  Ereignisse 24-26 / Plaetze 12-14 in 2030: kein anderes Modul belegt sie (grep 0x2030 in engine/src: lift2030_r37,
  itemband_r37, spinnen/re2-KI — andere Slots), `ereignis_platz` legt nicht ueber belegte Plaetze.
* Pfade (`git diff 116e8b0f --stat`): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/, keine
  CMakeLists (Registrierung ueber tests/unit/probes/r41_ventilkette.cmake). Riegel parallel-fest (exe-Kopien
  re15_pc_r41c_haken_/re15_pc_r41cb_, RUN_SERIAL), `cmake_minimum_required` in den -P-Skripten, `.exe` nur per MATCHES.
* **Gemeinsame Dateien ausserhalb der Zuteilung (VERTRAG §1.3 nennt C nur bei scd_room_setup.c, scd_vm.c,
  test_r36_kartenbits.c):** platform/pc/main.c (4 Stellen, 11 Zeilen: include, Prop-Lader-Schleife, Installer-Zeile
  im Boot-/CONTINUE-Weg, Zeichner) und engine/src/game_step_common.c (1 Zeile Takt; §1.3 = Spur H). Je Stelle klein
  und "Runde 41 Spur C" kommentiert, aber nicht zugeteilt -> Mangel M6 (Orchestrator-Entscheid wie B5).
  Dazu zwei FREMDE Riegel geaendert (test_r36_dachkiste.cmake +2 Zeilen `RE15_SET_FLAG=9:100`, test_r37_itemband.cmake
  `RE15_SET_FLAG=9:101,9:102`) — begruendet (neue Sperren widerlegen alte Riegel), aber Integrationsliste.
* Bank 5 (nicht zugeteilt, Raum-Scratch): Marker (5,0x1D)/(5,0x1E) nur in 2030 gesetzt (ROOM2030/2031-Skripte
  benutzen Bank 5 nicht: 0 Treffer `21 05`/`22 05` im SCD-Bereich), ABER der Installer loescht beide Bits in JEDEM Raum
  nach dem Init-Lauf -> G3.

### G3 Nebenwirkungen
* **ROOM1171 (Elza, Heliport-Kraehenszene)** (`<abn0>/mess/h1171`, `RE15_SET_FLAG=3:194`, `RE15_SCD_TRACE=1`):
  im Init-Lauf F250 startet sub00 -> Gosub sub07 -> `@0x1704 op=0x04` Evt_exec sub02 -> `[scd] F250 slot=10 @0x13B2
  op=0x22` = `22 05 1e 01` Set(5,0x1E)=1 — im SELBEN Init-Takt, danach ruft scd_room_reenter (scd_room_setup.c:519)
  re15_ventilkette_r41_install, das `re15_game_flag_set(5, 0x1E, 0)` ausfuehrt (ventilkette_r41.c:337-338, fuer
  JEDEN Raum). Das Bit ist das Kraehen-Flag DIVE-ARM (enemy_ai_common.c:6643 `@0x801155e8 testbit 0x1e`). In meinem
  Szenenlauf (`<abn0>/mess/h1171b`) erreichen die Kraehen Unterzustand 4 nie (st 4 ss1 0->1->2->3 -> st 3 -> 7), daher
  heute ohne sichtbare Folge; unit_r38/r39_heliport1171_* 19/19 gruen. Der Original-Zustand wird trotzdem in einem
  fremden Raum verfaelscht -> Mangel M3.
* 2070 nach der Flut: 2030 Slot 0 bleibt auch mit Bit 107 auf 20B0 umgelenkt (Lauf L6 `nach2030`), 20B0 hat keine Tuer
  nach 2070 -> 2070 (und dessen Tuer nach 11A0) ist nach der Flut nur noch ueber 2040 (Aufzug mit Strom) erreichbar. Das
  Dossier §6 sagt "von 2030/2040 aus erreichbar" — falsch -> Mangel M7.
* Software-Renderer der Riegel: Ertrink-Blende deckend gelb (L3b) -> Mangel M4.

## 3. Urteil je Nutzer-Punkt (gegen den Wortlaut, am gebauten Stand exe 02:51)

| Punkt | Urteil | Beleg (Messung) |
|---|---|---|
| 1 Leiter 2000 mit Wasserstrom blockiert, "I cannot climb up the ladder because of the water pressure", bis die Flut lief | **erfuellt** | L1 (zu Fuss + Aktionstaste: Text 40 woertlich, kein Raumwechsel; Strahl aus dem Gitter neben der Leiter = Original-ESP @0x0174C ff.), Elza 2001 = Riegel _b K gruen; nach der Flut frei (L4) |
| 2 Bruecke oben, Pult links (Cut 2 Nordwand) meldet zu wenig Druck / Ventil 13 zu -> Ventil 13 -> "Lower the bridge?" -> Bruecke faehrt herunter (nicht die Kurbel) | **erfuellt** | L2 (ganze Kette zu Fuss: Pult Text 40, Steg gesperrt z -6418, Treppe, Ventil 13 Text 41/42 + Bit 101, Treppe hinauf, Pult Text 43 -> Cut 4 -> Planke -1800 -> Bit 102 -> Text 44, Steg begehbar bis z 2989); L9: Pult-Platz in Kamera 2 an der Stelle der Nutzer-Marke |
| 3 Ventile -> Warnung (1 min, Raum verlassen) -> 60 s -> Wasser ueber die Ringebene; Ost-Tuer -> 20B0 (Auswasch-Szene des Originals); Zeit abgelaufen im Raum: Tod mit Ertrink-Effekt | **teilweise** | L3/L3b/L7: Warnung, 1801 Bilder, Flut bis -16200, Tod 125 Bilder nach Flutbeginn mit blauem Schleier (Direct3D) + einem Spritzer, Ost-Tuer zu Fuss -> 20B0/20B1 + Szene startet (Cut 11, Plc_dest) — **aber die Auswasch-Szene haengt in diesem Baum: Leon steht ab F109 unter Pad-Sperre in Cut 14, Eingaben wirkungslos (Softlock, L3)**; laeuft erst mit dem Haken der Spur A (Mangel M2) |
| 4 Danach 20B0 statt 2000 (spaeter wieder frei ohne Wasser), Leiter frei (kein Wasser laeuft mehr herunter) | **teilweise** | L4: jede Tuer nach 2000 fuehrt nach 20B0 (2010 zu Fuss, 2050-Leiter), Leiter 20B0 -> 2050 frei, kein Strahl (fx 0) — gemessen mit vorgesetztem (3,235). Im gebauten Stand ist dieser Zustand NICHT erreichbar: der erste Eintritt in 20B0 startet sub02 (Ck(3,235)=0 @0x01AAA) und haengt (M2) |
| 5 2050: Wasserlevel (Hebel) erst nach dem Pult-Raetsel (80 V wie 11F0) | **teilweise** | L5: Hebel Ereignis 4 vorher gesperrt (Text 40), Pult zu Fuss/Steuerkreuz 40/70/80 V -> Bit 106 -> Original sub04 hebt das Wasser — **aber mit den Kisten im Becken legt das Original den Hebel auf Ereignis 2 (sub01 @0x0201A) und sub02 @0x0220E laesst das Wasser OHNE Pult steigen** (`kisten_hebel`, Mangel M1) |
| 6 2080: Rechner senkt 2080 und entleert danach auch 2030 (Raum wieder frei ohne Wasser) | **erfuellt** | L6: Rechner -> Original-Frage -> Cut 7 Senken -> Text 40 + Bit 107; Tuer 2080 -> 2030 zu Fuss: keine Wasserflaeche, Lift-Rufzonen wieder Original |

## 4. Maengel (nummeriert, nachpruefbar)
* **M1 (hoch, Punkt 5)** ROOM2050: das Pult sperrt nur Ereignis 4 (`re15_ventilkette_r41_ereignis`, ventilkette_r41.c:370-373).
  Liegen die drei Kisten im Becken ((5,0)/(5,1)/(5,2) = 1: sub01 setzt sie, wenn Kiste obj 1/2/3 mit Member 0x0F auf
  Feld 3/4/5 steht, `3e 00 0f 00 03 00` @0x02036 ff.; Abfrage @0x02006 `06 00 1c 00` + @0x0200A-@0x02016), setzt sub01 @0x0201A
  `46 06 03 31 ff 00 18 02` den Hebel-Platz Slot 6 auf Ereignis 2; die Aktionstaste startet dann Original sub02
  @0x0220E (Frage msg 0 -> Set(3,109) @0x0221C -> obj 0 auf 0 @0x02268 -> Speed -20 For 180 @0x0227C) = das Wasser
  steigt ohne Bit 106. Messung `<abn0>/mess/kisten_hebel` (Bild `<abn0>/kisten_hebel_sheet.png`, Wegplan-Log `FLAG (3,109)
  = 1`, `FLAG (9,106) = 0`). Das ist der eigentliche Wasser-Raetselweg des Raums (Kisten schwimmen). Kein Riegel deckt
  Ereignis 2 ab. Forderung: Ereignis 2 in 2050/2051 ohne Bit 106 ebenfalls auf den Sperrsatz legen (gleiche Form) +
  Riegel (Unit F und exe) mit dem Kisten-Zustand.
* **M2 (hoch, Punkte 3/4, Integrationsabhaengigkeit)** Im gebauten Stand haengt die 20B0-Szene: nach dem echten Weg
  (Ost-Tuer zu Fuss) steht Leon ab F109 bei (-15820,-6355) in Cut 14 unter Pad-Sperre (pm 2) bis F1100, Eingaben
  R/U ohne Wirkung, sub05 startet nie (Ursache laut Dossier §6: aot_common.c:1169 ueberspringt AUTO-Plaetze waehrend
  der Szene). Der Haken liegt im Zweig r41/wasser20b0 (aot_common.c `re15_wasser20b0_r41_auto_aot`), NICHT hier. Damit
  ist die Kette in diesem Baum nicht durchspielbar (jeder erste 20B0-Eintritt startet sub02). Forderung: vor der
  Freigabe der Spur C den Spur-A-Haken integrieren und die GANZE Kette in EINEM exe-Lauf belegen (2030 Ventile ->
  Ost-Tuer -> Szene -> `letterbox closed` -> (3,235)=1 -> Leiter -> 2050), mit Riegel, der den Szenen-Abschluss prueft
  (integration_r41_ventilkette A prueft ihn bewusst nicht).
* **M3 (mittel, Nebenwirkung)** `re15_ventilkette_r41_install` loescht (5,0x1D)/(5,0x1E) in JEDEM Raum nach dem
  Init-Lauf (ventilkette_r41.c:337-338). ROOM1171 sub02 @0x013B2 `22 05 1e 01` laeuft im Init-Takt (SCD-Trace F250
  slot 10, `<abn0>/mess/h1171/debug.log`) und wird damit sofort wieder geloescht (Kraehen-Flag DIVE-ARM @0x801155e8).
  Heute ohne sichtbare Folge (`<abn0>/mess/h1171b`: Unterzustand 4 nie erreicht), aber ein Original-Zustand eines
  fremden Raums wird verfaelscht. Forderung: nur in 2030/2031 loeschen (das Raumladen wischt Wort 0 ohnehin @0x8003ed74).
* **M4 (mittel, Beleg)** Riegel C "Framedump F2750" belegt das Bild nicht: in seinem Software-Renderer ist F2750 deckend
  (160,160,0) GELB (alle vier Laeufe seit 22:23, auch mein Lauf lauf_9d92c55a_c), weil der Software-Renderer ABR2 nicht
  kann (`ABR2 (B-F) NICHT unterstuetzt -> Rueckfall BLEND`). Blau ist es nur in Direct3D (`<abn0>/mess/flut3`: (0,0,30)).
  Forderung: Riegel misst die Farbe (Renderer mit ABR2 oder Blendenzustand), Dossier §4/§5 nennt die Einschraenkung.
* **M5 (niedrig, Entscheidung)** Frage (f): geflutetes 2030 ueber den Ring betretbar (20B0 Ost-Tuer, 2070 Slots 1/2) —
  Leon watet bis zur Brust in Wasser, das waehrend der Flut toedlich ist (L8). Braucht Orchestrator-Entscheid +
  Id-Zuteilung (20B0/20B1 Id 41, 2070/2071 Id 40); innerhalb des Vertrags nicht loesbar.
* **M6 (formal, Vertrag §1.3)** Haken in platform/pc/main.c (4 Stellen, 11 Zeilen) und engine/src/game_step_common.c
  (1 Zeile) ohne Zuteilung fuer C; dazu zwei fremde Riegel geaendert (test_r36_dachkiste.cmake, test_r37_itemband.cmake,
  Querbezug-Flags). Orchestrator-Entscheid noetig (vgl. B5) und Integrationsliste.
* **M7 (niedrig, Dossier)** §6 "2070 bleibt nach der Flut nur von 2030/2040 aus erreichbar" stimmt nicht: 2030 Slot 0 bleibt
  auch nach dem Entleeren (Bit 107) auf 20B0 umgelenkt (`tueren_umlenken`, Lauf `nach2030`), also ist 2070 (und dessen
  Tuer nach 11A0) nach der Flut nur noch ueber 2040 erreichbar. Als Folge benennen bzw. entscheiden lassen.
* **M8 (niedrig, Text)** Nach dem Entleeren (Bit 107) sagen die Ventile weiter "The valves are open. The pool has been
  flooded." (`2030_ventile_zustand` haengt nur an Bit 103, ventilkette_r41.c:369).
* **M9 (niedrig, Doku)** Kopf von tests/unit/test_r41_ventilkette.c nennt die Marker "(5,0x3D)"/"(5,0x3E)" — der Code
  nutzt 0x1D/0x1E (Wort 0). Veraltet.
* Hinweis H1: RE15_R41C_ERTRINK_TIEFE 900 zitiert @0x8002c24c "als Mass einer Koerperhaelfte" — die Stelle hebt
  Typ-4-OBJEKTE an (G1); klar als PORT-WAHL schreiben. Hinweis H2: der Spritzer am Spieler liegt ausserhalb der
  Flut-Kamera Cut 4, wenn Leon an den Ventilen steht (L3b) — sichtbar sind Wasser, blauer Schleier, Tod. Hinweis H3:
  PSX-Ziel weder gebaut noch gemessen (Wasserflaechen nur `RE15_PLATFORM_PC`).

## 5. Fazit
**Nicht bestanden.** Punkte 1, 2 und 6 sind am gebauten Stand zu Fuss/mit der Aktionstaste erfuellt (Leon und, wo
gemessen, Elza). Punkt 5 ist nur fuer den Hebel ohne Kisten gesperrt: mit den Kisten im Becken laesst das Original-sub02
das Wasser ohne Pult steigen (M1). Punkte 3 und 4 sind in diesem Baum nicht durchspielbar, weil die 20B0-Auswasch-Szene
ohne den Haken der Spur A haengt (M2; die Mechanik selbst — Warnung, 60 s, Flut, Ertrinken, Umlenkung, Leiter frei,
kein Strahl — ist gemessen und stimmt). Dazu: Bank-5-Bits werden in jedem Raum geloescht (M3, ROOM1171 gemessen), der
Bild-Riegel der Ertrink-Blende prueft nichts und sein eigenes Bild ist im Test-Renderer gelb (M4), Frage (f) offen (M5),
Haken in main.c/game_step_common.c ohne Zuteilung (M6), kleinere Doku-/Textpunkte (M7-M9).
RE-Gate: haelt (alle Stichproben stimmen, keine geratenen Konstanten; Hinweis H1). Tests: alle gefahrenen gruen
(Unit 145/145, r41-Integration 2/2, Querbezug 8/8) — sie decken M1-M4 nicht ab.
