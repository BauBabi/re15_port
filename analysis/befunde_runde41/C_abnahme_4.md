# Runde 41 — Spur C "ventilkette" — Abnahme 4 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD b08bb0c4 (Basis = merge-base master
116e8b0f; master selbst steht auf 200184bc und enthaelt fremde Spuren — Gegenstand ist nur der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Nachtraegen 6-13 (+ Nachtrag 15 = Freigabe, im
master seit 19:01) und den Entscheiden C-a..e,
C-E1..C-E3, C-E2b (C-E2c widerrufen durch Nachtrag 6). Vorlage: Abnahme 3 (`C_abnahme_3.md`, Maengel 1-4), Nachbesserung 4
(Dossier §0e, Commits a5830413..b08bb0c4).
Begonnen 2026-10-24 ~18:4x. Pruefer: Abnahme 4 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad
`C41abn4/` (unten `<a4>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn4`).

**Ergebnis: (in Arbeit)**

## 0. Stand / Protokoll (neueste zuerst)
* 19:3x Messungen K1/K2/D/P/P1/P4/P5/P6/FL/FLR/N12/KE abgeschlossen (Leon + Elza wo verlangt); Integration r41 4/5 gruen, Haupt-Riegel
  und Raum-/Dokument-Gruppe laufen (`<a4>/ctest_integ_r41.txt`, `<a4>/ctest_integ_raum.txt`). Neu im master (nach dem Start dieser
  Abnahme): AUFTRAG.md **Nachtrag 15 (19:01)** = Freigabe des Ventilraetsels durch den Nutzer (§4).
* 18:5x Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `integration_r41_ventilkette_kette
  uebersprungen: Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a4>/configure.txt`,
  `<a4>/build.txt`); Unit-Gruppe 241/241 (§3); Integrationsgruppen im Hintergrund (`<a4>/integ.sh`); K1 gemessen (§1).
* 18:4x Start: `git status` leer, HEAD b08bb0c4; Spur-Diff (`git diff master...HEAD --name-only`, ohne analysis/) = 32 Pfade:
  engine/src/ventilkette_r41.c (+839), include/re15_ventilkette_r41.h (+454), gen/ventilkette_r41_{original,plakat,programme,
  texte,wasser}.inc, tools/r41_ventilkette/*.py (5), Tests (unit 1 + probes 1 + integration 5 eigene), Haken in
  game_step_common.c (+1), re15_files.c (+10), scd_room_setup.c (+2), scd_vm.c (+4), platform/pc/main.c (+16),
  test_r36_kartenbits.c (+7), test_r36_dachkiste.cmake (+2), test_r37_itemband.cmake (+4), neue Assets
  shared_assets/RE2/FILES/FILE31_{title_page,p00_page,p01_page,p02_page,title_paper}.TIM. Last auf der Maschine: fremde
  Abnahme-/Bau-exe laufen (h41abn1, r41b, E41n4b, r37d) -> Fenster-Riegel ggf. einzeln nachfahren.

## 1. Messungen an der exe (eigene Laeufe)
Werkzeug: `<a4>/lauf.sh <name> <timeout> KEY=VAL...` (eigene exe-Kopie `re15_pc_abn4c_<name>.exe` neben der exe, erbt keine
RE15_-Variablen, Laufordner `<a4>/mess/<name>`, Kopie danach geloescht), Wegplaene `<a4>/plaene/*.txt` (Treiber RE15_WEGPLAN =
echte Pad-Bits, dachkiste_r36.c). Beschleunigter Renderer (kein RE15_SOFTWARE_RENDER), RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP.
Leon = Titel-Haken, Elza = RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 (`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`).
Spinnen 2030 als getoetet (RE15_SET_FLAG 7:155..158, wie die Riegel). exe = `re15_pc.exe` 16:59 (7889599 B) = Code HEAD
(nach 56b0eaa8 nur Doku; `local_build.sh build` -> `ninja: no work to do.`).

### K1 — Punkt 2 + Nachtraege 9/10/13 Teil 1: echte Tuer 2070 -> 2030, Bruecke, Ventil 13, Steg, Ventil ohne Sicherung, Sicherung (Leon `k1`, Elza `k1_elza`)
Start ROOM2070/2071 (4400,-6400) vor Tuer Slot 1, Wegplan `<a4>/plaene/k1.txt`. Hinweis: der Wegplan-Treiber nimmt hoechstens 96 Befehle
(`#define WP_MAX 96`, dachkiste_r36.c:165); mein Plan hatte 192 -> nach der Sicherung (Befehl 95) blieb der Treiber stehen, die zwei
Laeufe habe ich per PID beendet (eigene Kopien). Die zweite Haelfte faehrt K2 ab genau diesem Zustand.
Gemessen (debug.log / wegplan.txt, Leon; Elza Zeile fuer Zeile gleich mit `room=2031`):
* `[aot] DOOR FIRE slot=1 rect=(5500,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` (Elza
  `room2031.rdt`) -> `[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt` + `Ventile 01/02/03/07/08/09 (Slots
  14-19), Plakat Slot 21, Dokument Slot 20 (Item 0x4E)`; Startbits `FLAG (9,101) = 0`, `(9,102) = 0`, `(3,108) = 0`, `(9,103) = 0`,
  `(9,107) = 0`; `ZELLE r0[12] ... u0 0xff` (Steg gesperrt, F69 und am Treppenfuss F1269).
* Pult (Ring Nordwand, Cut 2): `Ereignis 24 -> Port-Programm 2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the
  bridge. The pressure gauge reads zero.Intake valve 13 is closed."`.
* Treppe hinab, Nordventil 01 VOR der Bruecke: `Ereignis 17 -> Port-Programm 2030_ventile_vorher` -> `id=49 "I should look around here
  first."` (Nachtrag 13 Folge 1: neutral).
* Ventil 13 (Tank): `Ereignis 25 -> 2030_v13_frage` -> `id=41 "Intake valve 13. Will you open it?"` -> Ja -> `id=42 "You've opened
  valve 13. The pressure is rising."` -> `FLAG (9,101) = 1` (F1974).
* Treppe hinauf, Pult: `2030_pult_senken` -> `id=43 "The pressure gauge reads 80. Lower the bridge?"` -> Ja -> `[scd F2713] Cut_chg(4)`
  -> `[prop-member F2773] prop[0] obj=0x00 m=1 -> val=-1800` -> `FLAG (9,102) = 1` (F3047); erneute Aktion `2030_pult_unten` -> `id=44
  "The bridge is down."`.
* Treppe hinab: `ZELLE r0[12] ... u0 0x00` (F3745), ueber den Steg (Cut 4, `pl pos=(-1061,-1800,-3711)` .. `(-1001,-1800,729)`) zum
  Sued-Landeplatz; Ventil 09 OHNE Sicherung: `Ereignis 22 -> 2030_ventile_vorher` -> `id=49`; Sicherung (Original Slot 9): `id=2 "Will
  you take the Fuse?"` -> Ja -> `id=3 "You've taken the Fuse."`.
* Bildbogen `<a4>/bilder/k1_teil1.png` (F280..F4450): Pult an der Nordwand mit Text, Nordventile 01/02 mit Satz 49, Frage "Lower the
  bridge? Yes/No", Cut 4 Bruecke in der Fahrt (F2740/F2770), Spieler auf dem Steg (F3820), Ventil 09 Satz 49 (F4090), Sicherungsfrage.


### K2 — Punkt 3 / Nachtrag 13 Teil 2: falsche Folge (Plakat) -> zurueck, richtige Folge -> Warnung -> zu Fuss zur Ost-Tuer -> 20B0 (Leon `k2`, Elza `k2_elza`)
Fortsetzung von K1 ab genau dem dort gemessenen Zustand (Bits 101/102 + (3,108); Start am Sued-Landeplatz (-4686,7144) = Standort nach
der Sicherung), Wegplan `<a4>/plaene/k2.txt` (91 Befehle). Leon und Elza Zeile fuer Zeile gleich (Elza `room=2031`, `room20b1.rdt`):
* Plakat-Folge zu Fuss (Sued 09, Nord 01, Nord 02, Sued 07): `Ereignis 22 -> 2030_ventil_09` -> `id=55 "Valve 09, red wheel. Will you turn
  it?"` -> Ja -> `Ventil 09 gedreht (Drehung 1/4)`; `id=50 "Valve 01, black wheel. ..."` -> `Drehung 2/4`; `id=51 "Valve 02, green
  wheel. ..."` -> `Drehung 3/4`; `id=53 "Valve 07, black wheel. ..."` -> `Drehung 4/4` -> **`Folge 09 01 02 07 -> FALSCH: zurueckgesetzt`**
  -> `Ereignis 27 -> 2030_ventile_falsch` -> **`id=45 "The valves turned back to their original position."`**; danach `FLAG (5,16) = 0`,
  `FLAG (5,19) = 0` (Drehbits zurueck). Keine Rueckmeldung je Schritt (die ersten drei Drehungen ohne Text).
* Richtige Folge (Sued 09, Sued 07, Nord 02, Nord 01): Drehung 1/4..4/4 -> **`Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut`** ->
  `Ereignis 26 -> 2030_ventile_flut` -> **`id=46 "WARNING: The water will rise in one minute. Leave the room!"`** -> `ROOM2030: Tuer Slot 0
  -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> `Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt`; `FLAG (5,29) = 1`.
* Flucht zu Fuss (Treppe hinauf ~F3960 `pos=(-4488,-3549,-7704)`, Ring Nord -> Ost): `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,
  hh=1750) target_cut=11 spawn=(5950,-1800,-6400)` (~F4650, ~960 Bilder = 32 s nach dem Zaehlerstart) -> `loaded room20b0.rdt` ->
  **`Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)`** -> `ROOM20B0: Ring-Tuer Slot 3 nach 2030
  gesperrt (geflutet) -> Sperrtext 41`; keine `FLUT`/`ERTRUNKEN`-Zeile; `EXIT_AT: Bild 300 in Raum 20b0`.
* Fehlerton: `re15_audio_re2_panel_se(0x0B)` (ventilkette_r41.c) -> Bank PANEL2130.EDT (192 B = 48 Saetze) Satz 0x0B = `00 00 83 01`
  (nicht leer; 0x0A `00 00 74 00`, 0x0C `00 00 93 01`). Laeufe mit RE15_NOAUDIO -> Ton nicht mitgeschnitten (nur Code + Bank geprueft).

### D/P — Dokument und Plakat (Nachtrag 13 Folgen 2/3; Leon `dok`/`plakat`, Elza `dok_elza`/`plakat_elza`, RE15_PRESS wie die Riegel)
* Dokument (Platz Slot 20, Spieler (-4600,6820) Blick +z): Viereck F330 -> Leser: Titel "TECHNICIAN'S NOTE" 1/3, Seite 2/3 und 3/3 mit
  dem Nutzertext, EXIT, "The Technician's Note has been filed." -> `[ventilkette41] Dokument 6 "Technician's Note" abgelegt: Bit (9,107)
  = 1` (Leon und Elza). Bildbogen `<a4>/bilder/dok_leon.png`, Textseite 2x `<a4>/bilder/dok_leon_s2.png`. Text gegen den Nutzerwortlaut
  (AUFTRAG.md Nachtrag 13) Wort fuer Wort gelesen — im Spielbild und in den Assets (`<a4>/bilder/FILE31_p01_page.png`,
  `FILE31_p02_page.png`, Titel `FILE31_title_page.png`, aus shared_assets/RE2/FILES/FILE31_*.TIM selbst dekodiert): gleich bis auf die
  zwei Gedankenstriche U+2014 -> '-' (RE2-Dokumentschrift ohne Gedankenstrich; dokument_satz.py nimmt den Text direkt aus AUFTRAG.md).
* **Sichtbarkeit des Dokument-Blatts in der Welt** (eigene Messung, Paare mit/ohne Bit 107 bei gleicher Kamera, RE15_FORCE_CUT nur zur
  Bildwahl, Spieler ausserhalb des Bilds; alle 16 Cuts von ROOM2030, `<a4>/sicht_rest.sh`, `<a4>/sicht_rest.txt`): Cut 0/1/2/3/4/6/
  7/8/9/10/11/12/13/14/15 -> **0 Pixel Unterschied**; Cut 5 (die Kamera des Sued-Landeplatzes) ->
  **39 Pixel** (Rahmen x 102..113 y 143..149, ein dunkelgrauer Fleck hinter dem Gelaender, `<a4>/bilder/sicht_c5_dok.png`,
  Ausschnitt 4x `<a4>/bilder/sicht_c5_dok_diff.png`). `[prop-render] pi=5 oid=0x06 pos=(-4600,-1800,7150)` wird gezeichnet, ist aber
  praktisch nicht zu sehen; in K1 steht der Spieler zum Aufheben der Sicherung (-4686,7144) genau auf dem Blatt. -> Mangel 1 (§5).
* Plakat (Platz Slot 21, Spieler (-3650,7280) Blick +z): `Ereignis 23 -> 2030_plakat` -> `Plakat-Nahaufnahme offen` -> Vollbild (F340/F400:
  "VALVE OPERATION / FLOOD GATES - POOL / 1. GREEN 09 / 2. RED 01 / 3. BLACK 02 / 4. RED 07 / Turn the valves in this order.") -> Viereck
  F420 -> `Plakat-Nahaufnahme zu (nach 89 Bildern)` -> Raum (F430). Waehrend offen: `up` an F360/F370/F380 -> Lage unveraendert
  (-3650,-1800,7280); kein Klettern (mo 212/211, y -1800; mit Band 2 greift die Sonde @0x8002d370 nicht). Leon und Elza gleich.
  Bildbogen `<a4>/bilder/plakat.png`. Der Aushang an der Suedwand ist im Spielbild (Cut 5) deutlich zu sehen (weisses Blatt mit
  Punkten links vom roten Rad unter "09", `<a4>/bilder/k1_sued.png`).
* Ableitung der Loesung (selbst nachvollzogen): Raeder in der Welt 09 rot, 07 schwarz, 02 gruen, 01 schwarz (BSS Cut 3/5/7 + Fragetexte
  50-55). Plakat 09 GREEN / 01 RED / 02 BLACK / 07 RED -> Farbtausch einheitlich (Plakat-GRUEN = rot, -ROT = schwarz, -SCHWARZ = gruen,
  "They mixed up black, green, and red"), die zwei schwarzen Ventile 01/07 vertauscht ("swapped the numbers for black") -> 09, 07, 02, 01
  = RE15_R41C_LOESUNG. Die Lesart "Farben stimmen" fuehrt zu keinem eindeutigen Ventil (zwei gruene 02/08) -> eindeutig.

### P1 — Punkt 1: Leiter 2000 vor der Flut (Leon `p1`/`p1r`, Elza `p1_elza`/`p1r_elza`)
* Echter Weg `p1r`: 2010 -> `[aot] DOOR FIRE slot=0 rect=(-5300,-4925,hw=500,hh=875) target_cut=14 spawn=(-26450,-1800,-2700)` ->
  `loaded room2000.rdt` (Elza `room2001.rdt`) -> `ROOM2000: Leiter Slot 4 (@0x0169E) -> Sperrtext 40` -> zu Fuss an die Leiter (Cut 14 ->
  9 -> 10) -> Aktion -> **`id=40 "I cannot climb up the ladder because of the water pressure."`** (2x), kein Raumwechsel, `FLAG (9,100) = 0`
  vorher und nachher. Bild (`<a4>/bilder/p1r_leiter.png`, Cut 10 wie im Spiel): Wasserstrahl aus dem Gitterfenster neben der Leiter, Leon
  bzw. Elza an der Leiter, Text. Sprunglauf `p1` (Cut 0, Kamera folgt nach dem Sprung nicht) gleiches Protokoll.

### P4 — Punkt 4: nach der Flut 20B0 statt 2000, Leiter frei (Leon `p4`, Elza `p4_elza`; Bits 103/100 + (3,235) vorgesetzt)
* 2010: `ROOM2010: Tuer Slot 0 -> ROOM20B0 Cut 10 (Ziel ROOM2000)` -> `DOOR FIRE slot=0 ... target_cut=10` -> **`loaded room20b0.rdt`**
  (Elza `room20b1.rdt`) -> zu Fuss zur Leiter -> **`[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0
  spawn=(9100,-5400,4100)`** (kein Sperrtext) -> `loaded room2050.rdt` -> 2050-Leiter: `ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel
  ROOM2000)` -> `loaded room20b0.rdt`. Bild `<a4>/bilder/p4_20b0.png` (Cut 9): Gitterfenster OHNE Strahl, Spieler an der Leiter.
  (Laufende per Zeitlimit, weil mein EXIT_AT auf 2050 zielte und der Plan vorher nach 20B0 zurueckging — kein Spielbefund.)

### P6 — Punkt 6 / Nachtraege 7/8/12 (k): nach der Flut MIT Strom ueber die echte 20B0-Ost-Tuer -> 2030 Ring -> Lift B -> 2080 -> 2090 (Leon `p6`)
Zustand nach der Flut + Strom (Bits 103/100/101/102/106, (3,108), (3,126), (3,144), Kisten/Kurbel/Tuer-Bits wie die Riegel; 20B0-Szene
(3,235) gelaufen), Start 20B0 (4950,-6400) vor Tuer Slot 3, Wegplan `<a4>/plaene/p6.txt`:
* `[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)` -> `loaded room2030.rdt` ->
  **`[ventilkette41] ROOM2030: Strom (3,126) -> Wasser knapp unter der Absperrung y -5350 (Schachtoeffnung), Treppe Slot 8 -> Sperrtext 48,
  Ring frei`**, **`[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)`** (Ringboden y -5400 -> Wasser 50 darunter). AOT-Liste ohne die Slots
  14-21 (Ventile/Dokument/Plakat unter Wasser), Treppe Slot 8 `typ 5 ev 48 band 3`.
* Treppe vom Ring: **`id=48 "I cannot go down the stairs because of the water."`**; Ring trocken begehbar (Nord -> Ost -> Sued -> West),
  Lift B: `FLAG (3,220)` 0 -> 1; Suedgalerie -> `DOOR FIRE slot=1 rect=(-7700,6400,...)` -> `loaded room2080.rdt` -> Rechner = ORIGINAL
  (`id=0 "From here maybe I can control the water level.Will you operate the computer?"`, `FLAG (3,245)` 0 -> 1, kein Port-Programm) ->
  `DOOR FIRE slot=1 rect=(-9150,-25650,...)` -> `loaded room2090.rdt`.
* Elza `p6_elza` Zeile fuer Zeile gleich: `loaded room20b1.rdt` -> Tuer Slot 3 -> `loaded room2031.rdt` -> `ROOM2031: Strom (3,126) ->
  Wasser knapp unter der Absperrung y -5350 ...`, `[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)` -> `room=2031 id=48` -> `FLAG (3,220)`
  0 -> 1 -> `loaded room2081.rdt` -> `room=2081 id=0 "From here maybe I can control the water level..."` -> `FLAG (3,245)` 0 -> 1 ->
  `loaded room2091.rdt`.

### FL — Punkt 3: Zeit abgelaufen und noch im Raum -> Flut -> Tod mit Ertrink-Effekt (Leon `fl`; ausgeloest ueber die richtige Folge)
Start Sued-Landeplatz (Bits 101/102 + (3,108)), Wegplan `<a4>/plaene/fl.txt`: 09/07/02/01 zu Fuss -> `RICHTIG` -> `id=46` -> `Zaehler
laeuft: 1800 Takte` (~F1750) -> Spieler bleibt am Nord-Landeplatz (-1320,-1800,-7887) -> `[scd F3524] Cut_chg(4)` -> `FLUT: Wasser steigt`
-> `SPRITZER: Wasser y -1800 erreicht die Fuesse (Spieler y -1800), Effekt 0x06050D00` -> `[fade-log] config ch=0 abr=2 mask=6 step=0
bucket=7` + `kick ch=0 value=0x5000` -> **`ERTRUNKEN: Wasser y -2700, Spieler y -1800 -> hp -1, blaue Blende`** -> mo 7 (Sterben).
Bildbogen `<a4>/bilder/fl_flut.png`: F3530..F3570 Draufsicht Cut 4, das Wasser steigt ueber den Steg, F3590/F3610 blauer Schleier,
F3700 "YOU DIED" (F3650 ein heller Vollbild-Uebergang der Todeskette).

### N12 — Nachtrag 12 (k) im Bild: Pegel mit Strom knapp unter der Absperrung (Leon `pg_<cut>` / `pt_<cut>`)
Je Cut zwei Laeufe, RE15_FORCE_CUT nur zur Bildwahl, F150: `pg_<cut>` = nach der Flut mit Strom, Eintritt ueber die echte 20B0-Ost-Tuer
(`Strom (3,126) -> Wasser knapp unter der Absperrung y -5350`, `[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)`), `pt_<cut>` = trocken (vor
der Flut, Sprung). Bildbogen `<a4>/bilder/pegel_sheet.png` (Zeilen Cut 1/2/7/15/4, links trocken, rechts mit Strom): in jeder Ring-Kamera
fuellt die Wasserflaeche den Schacht bis unmittelbar unter den Ringboden; Ringboden (gruener Laufsteg) und Stabgelaender stehen ueber
dem Wasser, nichts laeuft auf den Ring; Cut 4 (Draufsicht) zeigt den ganzen Schacht bedeckt. Zusammen mit P6 (Ring zu Fuss begehbar,
Treppe = Text 48) und G1 Nr. 5 (Wasser-Rechtecke innerhalb der Absperrzellen) -> Nachtrag 12 (k) erfuellt.

### KE — Abnahme-3-Mangel 1: `integration_r41_ventilkette_kette` von Hand am Endstand (C-E1)
`cmake -DRE15_PC_EXE=<baum>/re15_port/build/platform/pc/re15_pc.exe -DRE15_SRC=<baum>/re15_port -DWORKDIR=<a4>/kette_hand -P
<baum>/re15_port/tests/integration/test_r41_ventilkette_kette.cmake` (ohne RE15_-Umgebung) -> **rc=0**, Abschlusszeile `-- r41_ventilkette_kette:
2030 richtige Folge 09/07/02/01 zu Fuss -> Warnung -> Ost-Tuer zu Fuss -> ROOM20B0, Bits 103 + 100; Teil ab der Szene uebersprungen: Haken
der Spur A fehlt (aot_common.c re15_wasser20b0_r41_auto_aot)` (`<a4>/kette_hand/cmake_out.txt`). debug.log (lauf_365eb1cb_bis20b0):
`Folge 09 07 02 01 -> RICHTIG` -> `Ereignis 26 -> 2030_ventile_flut` -> `id=46` -> `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750)
target_cut=11` (kein `[fire-aot]`) -> `loaded room20b0.rdt` -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100` -> `EXIT_AT: Bild
200 in Raum 20b0`. Keine liegengebliebene `re15_pc_r41ck_*`-Kopie. Der Riegel faehrt jetzt die heutige Kette (Sued-Landeplatz, Ventile
zu Fuss, Flucht ueber Steg/Treppe/Ring); Registrierung unveraendert nur mit A's Modul. Merge-Probe: `git merge-tree --write-tree master
HEAD` -> einzige Kollision `analysis/befunde_runde41/AUFTRAG.md` (Doku), game_step_common.c / scd_vm.c / main.c mergen automatisch
-> im Integrationsbaum wird der Riegel registriert (master enthaelt engine/src/wasser20b0_r41.c). -> Mangel 1 der Abnahme 3 behoben.

### P5 — Punkt 5: 2050 Hebel erst nach dem Pult-Raetsel (Leon `p5v`/`p5p`/`p5h`, Elza `p5v_elza`/`p5p_elza`/`p5h_elza`)
(Code fuer 2050 seit Abnahme 3 unveraendert: `git diff 2fb94b25..HEAD` ohne 2050-Zeilen in ventilkette_r41.c/programme.inc; trotzdem neu
gemessen.) Gegner per GEGNERWEG weg, Start (4900,-7100) Ring Band 3:
* vorher: Hebel Slot 6 -> `Ereignis 4 -> 2050_hebel_sperre` -> **`id=40 "The pump has no power. The switchboard must be set first."`**,
  `FLAG (3,104) = 0` vorher/nachher, `FLAG (9,106) = 0` (Leon `room=2050`, Elza `room=2051`).
* Pult (RE15_INPUT_SCRIPT der Abnahme 3, Spielbild-Achse): `Ereignis 9 -> 2050_pult_ein` -> `id=43 "A high voltage switchboard. The pump
  needs 80 volts."` -> `id=41 "Will you push them?"` -> `[scd F504] Cut_chg(15)` -> `Hebel 3 -> EIN, ... 40 V` -> `Hebel 1 -> EIN, ...
  70 V` -> `Hebel 4 -> EIN, Schalter 0xD, 80 V` -> **`id=42 "Power supply OK."`** -> `Pult 2050 GELOEST: 80 V -> Bit 106` -> Ausstieg ->
  erneut: `2050_pult_zustand` -> `id=44 "The switchboard is set to 80 volts. The pump is powered."` (Leon und Elza gleich). Bild
  `<a4>/bilder/p5_pult.png`: Pult "DANGER High voltage", Skala 0..90 (80 rot), Zeiger, gruene Marken unter den EIN-Hebeln, "Power supply OK.".
* nachher (Bit 106): Hebel -> Original `id=0 "A lever to fill and drain water. Will you move it?"` -> Ja -> `Cut_chg(4)` -> `Cut_chg(7)` ->
  `FLAG (3,104)` 0 -> 1 (Original sub04, Wasser steigt), Leon und Elza.

### FLR — Punkt 3: Wasser UEBER die Ringebene, Spieler auf dem Ring (Leon `flr`)
Wie FL, nach der Warnung zu Fuss ueber Treppe und Ring vor die Ost-Tuer (6606,-5400,-1338), Tuer NICHT geoeffnet, Plan
`<a4>/plaene/flr.txt`: `[scd F3524] Cut_chg(4)` -> `FLUT: Wasser steigt` -> **`SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y
-5400)`** -> **`ERTRUNKEN: Wasser y -6300, Spieler y -5400 -> hp -1, blaue Blende`** (Wasser 900 ueber der Ringebene). Bildbogen
`<a4>/bilder/flr_ring.png`: F3540 Wasser ueber dem Steg, F3580/F3620 der Ring unter der Flaeche, F3660 blauer Schleier, F3800 "YOU DIED"
(F3700/F3740 Todes-Kamera der Engine).
Elza `flr_elza` gleich: `Spieler-Familie PL04` -> `RICHTIG` -> `ROOM2031: Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> `FLUT` ->
`SPRITZER: Wasser y -5400 ...` -> `[fade-log] config ch=0 abr=2 mask=6 step=0 bucket=7` -> `ERTRUNKEN: Wasser y -6300, Spieler y -5400`.

### PK — Randfall Plakat waehrend des Zaehlers bis in die Flut (Leon `pk`)
Plan `<a4>/plaene/pk.txt` (richtige Folge, Warnung, zurueck zum Plakat, Aktion) + `RE15_PRESS=square@3540`: `Plakat-Nahaufnahme offen` (F2118)
-> `[scd F3524] Cut_chg(4)` -> `FLUT: Wasser steigt` -> `Plakat-Nahaufnahme zu (nach 1421 Bildern)` (F3540) -> **`[scd F3555] letterbox
closed -> gameplay (player_mode=0)`** -> `Ereignis 23 -> 2030_plakat` -> `Plakat-Nahaufnahme offen` -> `SPRITZER` -> `ERTRUNKEN: Wasser y
-2700, Spieler y -1800` (F3585) -> Mangel 2 (§5).

## 2. Gates

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung)
Stichproben der in Nachbesserung 4 NEU zitierten Stellen (alle selbst gelesen, richtige Binaerdatei):
1. **@0x8002d370** (Kletter-Sonde FUN_8002d2c0, Begruendung von RE15_R41C_BLATT_BAND 2) — `re15_disasm.py dis 0x8002d2c0 64`
   (info/Re1.5/PSX.EXE): Schleife ueber die Objekte `0x800b3f98 + i*148` (`addiu s2,s2,-148` @0x8002d360), aktiv `andi v0,v0,0x101`
   @0x8002d358, `lbu v1,0(s3)` @0x8002d364 mit s3 = 0x800aca3c + 154 = **0x800acad6**, `lbu v0,130(s0)` @0x8002d368 = Objekt+0x82,
   **`bne v1,v0,0x8002d3a4` @0x8002d370**, sonst `jal 0x8002d1e8` / `jal 0x8002da4c`. Gegenprobe **@0x80031cfc** `lbu v0,130(a0)`,
   @0x80031d04 `lbu v1,-13610(v1)` = 0x800acad6 (vom Disassembler als `player.floor(+0x82)` benannt), @0x80031d08 `addiu v0,v0,1`,
   @0x80031d0c `bne v0,v1` -> "Objekt-Band == Spielerband" (Sonde) und "Objekt-Band + 1 == Spielerband" (Standtest) stimmen; Band 2
   liegt am Landeplatz (Spielerband 1) in keinem der beiden Faelle. **Bestaetigt.**
2. **@0x80028698** (Schliessen der Plakat-Nahaufnahme) — `dis 0x80028680 12`: `lw v0,-14484(v0)` = 0x800ac76c, **`andi v0,v0,0xc000`
   @0x80028698**, `beq v0,zero,0x80028748`. **Bestaetigt.**
3. **RE2 ROOM2130** (info/re2leon/PL0/RDT/ROOM2130.RDT, Datei-Offsets, selbst gelesen): @0x01752 `23 00 05 00 50 00` (cmp var5 == 80),
   @0x017D6 `23 00 05 00 00 00 2b 00`, @0x017E8 `23 00 05 01 50 00 2b 00`, **@0x01800 `36 02 0b 01`** (Fehlerton Satz 0x0B), @0x01138
   `24 05 00 00`, @0x01192 `36 02 0a 01` (Klick), @0x01762 `36 02 0c 01`, @0x0171C `09 0a 1e 00`. **Bestaetigt.** RE2 **@0x80071d04**
   `addiu a0,a3,-104` (`re2_disasm.py dis 0x80071cf0`, info/re2leon/PSX.EXE). **Bestaetigt.**
4. **RE1.5 ROOM11F0 sub17** (STAGE1/ROOM11F0.RDT): @0x0168C..@0x016BC `22 05 00 00`..`22 05 0c 00`, **@0x016C0..@0x016E4 `22 05 0d 00`..
   `22 05 16 00`**, @0x016E8 `22 02 00 00`, @0x016EC `22 02 02 00`, @0x016F0 `2a 00 3c 01`; @0x01602 `32 00 9e b3 00 00 9c 58`, @0x0160E
   `33 00 00 00 00 00 00 02`. **Bestaetigt.** ROOM1051 @0x00DA4 `22 02 07 01` / @0x00DD2 `22 02 07 00` **bestaetigt**; ROOM2080
   @0x00CC4 `2e 03 00 00` **@0x00CC8 `2f 01 32 00`** @0x00CCC `0d 00 06 00 94 00` **bestaetigt**.
5. **ROOM2030 SCA** (STAGE2/ROOM2030.RDT, 12 B je Zelle w,d,x,z,typ,u0,u1,fl) — Absperrung: 19 @0x0097C `f4 01 f2 44 18 15 d8 dc 01 ff 00 33`
   (x 5400..5900 z -9000..8650), 20 @0x00988 (x -5900..5900 z -9000..-8500), 21 @0x00994 (x -6350..5900 z 8150..8650), 22 @0x009A0
   (x -6350..-5850 z 2450..8650), 31 @0x00A0C (x -8150..-5400 z -6900..-6400), 32 @0x00A18 (x -5900..-5400 z -9000..-6400), alle typ 1
   fl 0x33; Ringboden 36..41 @0x00A48..@0x00A84 u1 01 fl 0x23 (z. B. 36 `08 07 7a 0d ec e1 92 d7 01 ff 01 23`); Wandzellen 6 @0x008E0
   (z -9000..-8500) / 7 @0x008EC (z 7900..8400) fl 0x11 = Wandflaechen der Ventile. Die drei Wasser-Rechtecke (x -5400..5400 z -8500..
   -6400; x -7700..5400 z -6400..2450; x -5850..5400 z 2450..8150) liegen innerhalb der Absperrung und ueberdecken keine Ringboden-
   Zelle (selbst nachgerechnet). **Bestaetigt.** Sicherung Slot 9 @0x01BAA `2c 09 03 31 01 00 f4 e8 fa 19 20 03 e8 03 ...`
   = x -5900..-5100 z 6650..7650; @0x01FF2 `22 03 6c 01`; Bruecke @0x01B6C `2d 00 00 00 00 00 0a 00 ...`; Treppe @0x01B8E `2c 08 0c
   31 80 00 ...` **bestaetigt**.
6. **"2030 benutzt Bank 5 nicht"** (Begruendung der Raum-Scratch-Bits 0x10..0x13/0x1B/0x1C): `scd_dump_room.py ROOM2030.RDT` (main
   0x1AC8..0x1CBC, sub 0x1CBC..0x3DD8, 7 Regionen) -> **0 Zeilen `bank=5`** (`<a4>/scd_2030.txt`). **Bestaetigt.**
7. Dokument-Basis 0x48 (u8 @0x800c7370): liegt hinter dem PSX.EXE-Bild (t_addr 0x80010000 + t_size 0xaf000 = 0x800bf000; laut
   re15_files.h DEBUG.BIN) — kein neuer Beleg dieser Spur (vorhandene Konstante RE15_FILES_FIRST_ITEM_ID), nicht nachgeprueft.
* Konstanten-Durchsicht (Kopf re15_ventilkette_r41.h, alle NB4-Zeilen): jede traegt @0x.../Datei-Offset oder ist als PORT-WAHL bzw.
  NUTZER-VORGABE benannt (Ventil-x = Rueckprojektion BSS + "auf 50 gerundet", Zuordnung Rad -> Nummer PORT-WAHL, Loesung PORT-WAHL zur
  Freigabe, Pegel-Abstand 50 PORT-WAHL mit Mass @0x00CC8, Blatt-Band PORT-WAHL mit @0x8002d370/@0x80031cfc, Plakat-/Dokument-Lage
  PORT-WAHL mit Bezug Slot 9 @0x01BAA bzw. Wand z 7900). Ids/Slots/Ereignisse = Vertrags-Zuteilungen.
* Rate-Woerter: `git diff master...HEAD -- re15_port` (ohne die zwei Daten-inc) nach deferred|tunable|interim|for now|faithful|plausib|
  TODO|FIXME -> 0 Treffer (2 Fehltreffer "ROOMxxx1"). Kein Env-Schalter als Abschluss (die neuen getenv sind keine; Plakat/Dokument
  laufen ohne Haken).


### G2 Vertrag (VERTRAG.md §1/§2)
* Pfade (`git diff master...HEAD --name-only`, ohne analysis/): 32; KEIN Pfad unter release/, platform/android/, shared_assets/PSX/, pri/,
  synchro/; keine Test-CMakeLists (nur die eigene probes/r41_ventilkette.cmake). Neue Assets nur unter shared_assets/RE2/FILES/
  (FILE31_title_page/p00/p01/p02/title_paper.TIM), im Dossier §7 fuers Paket-Gate genannt.
* Gemeinsame Dateien (Diff selbst gelesen, alle Zeilen "Runde 41 Spur C"): game_step_common.c +1 (Tick, C-E3), scd_room_setup.c +2
  (include + Installer-Zeile am Ende des Blocks), scd_vm.c +4 (include, Ereignis-Kette, Strahlen-Haken in op_sce_espr_on; f3173450 nur
  Kommentar "2030/17-27"), platform/pc/main.c +16 (include; Prop-Lader-Schleife jetzt k 0..3 = Wasser obj 4/5 + Blatt obj 6 + Aushang
  obj 7; Boot-/CONTINUE-Install; Pult-Zeiger 2050; NEU der Plakat-Haken: 5 Zeilen re15_render_pc_show_fmv/hide_fmv, nur die eigene
  Ebene), re15_files.c +10 (ein Tabelleneintrag s_docs[6] + Name; nicht in VERTRAG 1.3 gelistet, aber von AUFTRAG.md Nachtrag 13
  Folge 2 "Dokument-System des Ports (re15_files, Ids ab 0x48)" verlangt), test_r36_kartenbits.c k_port +6 (C erlaubt),
  test_r36_dachkiste.cmake +2 / test_r37_itemband.cmake +4 (Querbezug-Flags, C-E3). Klein und markiert -> haelt.
* Bank-9-Bits: 100, 101, 102, 103, 106, 107 (= Zuteilung C); 107 jetzt Taken-Bit des Dokuments, in k_port eingetragen;
  `unit_r36_kartenbits` gruen (Unit-Gruppe §3). Nachrichten-Ids: 2000/20B0 40, 2030 40-49 + 50-55 (Nachtrag 13 / VERTRAG 1.2), 2050
  40-44, 20B0 41, 2070 40 — alle in der Zuteilung; 2080 keine (Nachtrag 7). Ereignisse 2030 17-27 und Plaetze 12-21 liegen ausserhalb
  der Original-Subs/-Plaetze von 2030 (sub00..06, Slots 0-11; ROOM2030 gehoert nur Spur C).
* Neue getenv im Engine-/Plattform-Diff: 0. Tests je Punkt vorhanden und messend (Unit M: Folge, Zuruecksetzen, Nein, Dokument-Zone,
  Plakat; Integration `_weg`/`_weg_elza` v2a/v2b/v2d/v2p, Haupt A/C/D, `_b` i2/l2 Pegel -5350, `_kette` von Hand); eigene exe-Kopien je
  Riegel; -P-Skripte mit `cmake_minimum_required`.
* Kein SendMessage, kein Code ausserhalb des Baums (Pruefer).

### G3 Nebenwirkungen (beide Szenarien, Nachbarraeume)
* Beide Szenarien gemessen: K1/K2/D/P/P1/P4/P5/P6 je mit Leon UND Elza (`Spieler-Familie PL04`, Raeume 20x1), FL Leon, FLR Leon UND Elza;
  Unit-Fall M fuer 2030 UND 2031.
* Nachbarraeume: 2000/2001 (Leiter), 2010/2011 (Tuer -> 20B0 erst nach der Flut), 20B0/20B1, 2050/2051, 2070/2071 (Ring-Sperre, `_sperre`
  gruen), 2080/2081 (Rechner Original, P6), 2090/2091 (Ankunft P6) — alle wie vorgesehen; 1260 (Tuer nach 2000 -> 20B0 nur mit Bit 103)
  nur ueber Unit-Fall E.
* Dokument-System (re15_files.c +1 Eintrag): die Dokument-Riegel der Unit-Gruppe gruen (unit_r34n_e_dokumente, unit_r38_tagebuch_dokument,
  unit_r30_irons_diary_dokument, unit_re2doc_bildebene, unit_r34a_asset_abgleich); Integrations-Dokumentriegel in der Raumgruppe (§3).
* Render-Haken main.c: Plakat-Ebene nur, solange `re15_ventilkette_r41_plakat_rgba()` != NULL (nur 2030 mit offener Nahaufnahme), versteckt
  nur die eigene Ebene; Prop-Lader k 2/3 nur in 2030/2031 (`re15_ventilkette_r41_obj_id`).
* Nebenwirkung gefunden: Plakat-Schliessen hebt die Flut-Pad-Sperre auf (Mangel 2, §1 PK).
* Merge-Probe gegen den aktuellen master: nur AUFTRAG.md kollidiert (§1 KE).

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe (Regex `<a4>/regex_unit.txt` = Regex der Nachbesserung 4 + Dokument-/Asset-Riegel unit_r38_dining.*, unit_r37_dining.*,
  unit_files.*, unit_.*dokument.*, unit_r34_dok.*, unit_r40_.*nahaufnahme.*, unit_r34a_.*, probe_r16_objekte_paket; 241 Tests):
  `ctest --test-dir re15_port/build -R "<regex>" --timeout 600 --output-on-failure -j 4` -> **`100% tests passed, 0 tests failed out of
  241`** (32.35 s, `<a4>/ctest_unit.txt`). `test_r41_ventilkette.exe` direkt: `PASS (446 Pruefungen)`, rc=0 (`<a4>/unit_r41_v.txt`;
  Fall M fuer 2030 und 2031: falsche Folge -> 45 + zurueck, falsches Ventil mitten in der Folge, Nein vermerkt nichts, richtige Folge ->
  46 + Zaehler, Dokument-Zone Slot 20 (Item 0x4E, Bit 107, obj 6), Leser angefordert, Aushang Band 2, Plakat offen/zu).
* Integration r41 (`<a4>/integ.sh`, `-j 2`, alle RUN_SERIAL, Protokoll `<a4>/ctest_integ_r41.txt`, unter Last fremder Abnahmen und meiner
  Messlaeufe): `integration_r41_ventilkette_weg_elza Passed 1203.69 sec`, `integration_r41_ventilkette_weg Passed 1032.25 sec`,
  `integration_r41_ventilkette_sperre Passed 688.48 sec`, `integration_r41_ventilkette_b Passed 721.66 sec`, `integration_r41_ventilkette
  Passed 520.42 sec` -> **`100% tests passed, 0 tests failed out of 5`** (Total Test time (real) = 4166.59 sec). Haupt-Riegel lauf_883883f4:
  a `RICHTIG` -> `loaded room20b0.rdt` -> `Bits 103 + 100`; b `room=2000 id=40`; c `SPRITZER: Wasser y -5400` -> `ERTRUNKEN: Wasser y -6300,
  Spieler y -5400`; d `id=49 "I should look around here first."`. Laufordner selbst gelesen, z. B. `_weg` lauf_e25cc082_v2b:
  `Folge 09 07 02 01 -> RICHTIG` -> `DOOR FIRE slot=0 ... target_cut=11` -> `loaded room20b0.rdt` -> `Bits 103 + 100`; w3: `Wasser knapp
  unter der Absperrung y -5350`, `[prop-render] pi=4 oid=0x04 pos=(0,-5350,0)`.
* `_kette` von Hand (C-E1, im Spurbaum nicht registriert): rc=0 bis zur 20B0-Ankunft (§1 KE).
* Raum-/Dokument-Gruppe (17 Riegel, Regex `<a4>/regex_integ_raum.txt`, `-j 2`): (laeuft, Protokoll `<a4>/ctest_integ_raum.txt`)


## 4. Urteil je Nutzer-Punkt (gegen den Wortlaut, AUFTRAG.md; gebauter Stand exe 16:59 = Code HEAD b08bb0c4)
AUFTRAG.md ist massgeblich; im master steht inzwischen **Nachtrag 15 (19:01, nach dem Start dieser Abnahme)**: der Nutzer hat das
Ventilraetsel freigegeben ("Freigeben" — Plakat, Loesung 09 -> 07 -> 02 -> 01, Titel, Texte, Dokument "neben der Sicherung"; Fragen
(m)/(n) beantwortet). Keine neue Forderung; die Freigabe setzt die vorgelegte Beschreibung voraus ("Dokument ... neben der Sicherung").

| Punkt | Urteil | Beleg (eigene Messung) |
|---|---|---|
| 1 Leiter 2000 -> 2050 mit herunterlaufendem Wasserstrom blockiert, "I cannot climb up the ladder because of the water pressure", bis die Flut gelaufen ist | **erfuellt** | P1/P1r Leon + Elza ueber die echte Tuer 2010 -> 2000: Text 40 woertlich (2x), kein Raumwechsel, Bit 100 = 0; Bild Cut 10: Strahl aus dem Gitter neben der Leiter (`p1r_leiter.png`) |
| 2 2030: Bruecke zunaechst OBEN; Pult links (Kamera 2, Nordwand): zu wenig Druck / Ventil 13 zu -> Ventil 13 -> "Lower the bridge?" -> Bruecke faehrt herunter; nicht die Kurbel | **erfuellt** | K1 Leon + Elza ueber die echte Tuer 2070 -> 2030: Bruecke oben y -5400, Zelle 12 u0 0xff; Pult Text 40; Ventil 13 41/42, Bit 101; Pult 43 -> Ja -> Cut 4, Bruecke -> -1800, Bit 102, 44; Zelle 12 u0 0x00, ueber den Steg (`k1_teil1.png`) |
| 3 Ventilraetsel -> Warnung (1 min, Raum verlassen) -> 60 s -> Wasser ueber die Ringebene; Ost-Tuer -> 20B0 (Auswasch-Szene); Zeit abgelaufen im Raum: Tod mit Ertrink-Effekt | **erfuellt** (20B0-Szene selbst nach C-E1 mit dem Haken der Spur A, Riegel `_kette` vorhanden und im C-Baum bis zur 20B0-Ankunft gruen) | K2 Leon + Elza: richtige Folge -> 46 woertlich -> Zaehler 1800 -> zu Fuss zur Ost-Tuer -> `loaded room20b0/20b1.rdt`, Bits 103 + 100; FL (Landeplatz) und FLR (Ring): Cut 4, Flut, Spritzer bei den Fuessen, `ERTRUNKEN: Wasser y -6300, Spieler y -5400` (ueber der Ringebene) + blaue Blende, YOU DIED; KE `_kette` rc=0 |
| 4 Danach 20B0 statt 2000 (spaeter wieder frei ohne Wasser), Leiter frei (kein Wasser laeuft mehr herunter) | **erfuellt** | P4 Leon + Elza: 2010-Tuer -> `ROOM20B0 Cut 10` -> `loaded room20b0/20b1.rdt`, Leiter -> 2050 ohne Sperrtext, 2050-Leiter -> 20B0; Bild Cut 9 ohne Strahl (`p4_20b0.png`) |
| 5 2050: Wasserlevel (Hebel) erst nach dem Pult-Raetsel (80 V wie 11F0) | **erfuellt** | P5 Leon + Elza: Hebel vorher Text 40, (3,104) bleibt 0; Pult 43/41 -> Cut 15 -> 40/70/80 V -> "Power supply OK." -> Bit 106 (Bild `p5_pult.png`); danach Hebel = Original msg 0 -> Cut 4/7 -> (3,104) = 1 |
| 6 2080-Rechner entleert danach auch 2030 — ERSETZT durch Nachtraege 7/8/12 (k) (Strom leert 2030 bis knapp unter die Absperrung, Rechner nur Original) | **erfuellt** | P6 Leon + Elza ueber die echte 20B0-Ost-Tuer: `Wasser knapp unter der Absperrung y -5350`, `pos=(0,-5350,0)`, Treppe Text 48, Ring zu Fuss, Lift B (3,220), 2080 Rechner = Original msg 0 -> (3,245) -> 2090; N12 Bildbogen `pegel_sheet.png` |
| Nachtrag 12 (k): Pegel nach dem Strom "unterhalb der Absperrung, sodass es nicht drueber 'rein' laufen wuerde" | **erfuellt** | y -5350 = Ringboden -5400 + 50 (G1 Nr. 5 SCA-Belege, PORT-WAHL mit Mass @0x00CC8), Bilder Cut 1/2/7/15/4 (Ring trocken, Gelaender ueber dem Wasser) |
| Nachtrag 13 (+ Freigabe Nachtrag 15): echtes Ventilraetsel — (1) Text 49 neutral, (2) Dokument mit dem Nutzer-Text woertlich, (3) Plakat als Nahaufnahme mit der falschen Anleitung, (4) Dreh-Raetsel mit Reihenfolge und Zuruecksetzen, Loesung -> Warnung -> Flut, (5) Dossier §7 zur Freigabe | **teilweise** | (1) K1 `id=49 "I should look around here first."`; (2) Leser Leon + Elza: Text woertlich (nur U+2014 -> '-'), Bit 107 — ABER das Blatt ist in der Welt nicht zu sehen (15 von 16 Cuts 0 Pixel, Cut 5 39 dunkle Pixel; Mangel 1); (3) Plakat-Nahaufnahme Leon + Elza, Aushang an der Suedwand sichtbar; (4) K2 Leon + Elza falsch -> 45 + zurueck, richtig -> 46; Ableitung eindeutig (§1 D/P); (5) Dossier §7 + Freigabe Nachtrag 15 |
| Nachtraege 6, 9/10 | **erfuellt** | keine zusaetzliche Tuer (Diff; `_sperre` gruen); Ventile vor Bruecke (K1 Nordventil 01: 49) und vor Sicherung (K1 Ventil 09: 49), danach Fragen 50-55 (K2) |
| Abnahme 3 Mangel 1 (`_kette`) / Mangel 2 (Doku) | **behoben** | KE rc=0 auf der heutigen Kette; Dossier §7/§0b/probes-Kopf berichtigt (gelesen) |


## 5. Maengel (nummeriert, nachpruefbar)
* **Mangel 1 (mittel, Nachtrag 13 Folge 2 / Freigabe Nachtrag 15 "Dokument ... neben der Sicherung") — das Dokument-Blatt ist in der
  Welt nicht zu erkennen.** Ist (§1 D/P, eigene Messung): das Weltmodell obj 6 (RE2-Blatt mesh01, (-4600,-1800,7150), Band 2) wird
  gezeichnet (`[prop-render] pi=5 oid=0x06 pos=(-4600,-1800,7150)`), aber zwischen "Dokument da" und "Dokument genommen" (Bit 107
  vorgesetzt, sonst gleicher Lauf, RE15_FORCE_CUT nur zur Bildwahl) unterscheiden sich in 15 der 16 Kameras von ROOM2030 **0 Pixel**
  (Cut 0-4, 6-15) und in Cut 5 — der Kamera, die der Spieler am Sued-Landeplatz sieht (K1 `cut=5` an der Sicherung) — nur **39 Pixel**
  (x 102..113, y 143..149), ein dunkelgrauer Fleck hinter dem Gelaender (`<a4>/bilder/sicht_c5_dok.png`, 4x
  `<a4>/bilder/sicht_c5_dok_diff.png`, `<a4>/sicht_rest.txt`). Wer die Sicherung aufhebt, steht genau auf dem Blatt (K1 (-4686,7144)).
  Das Dokument ist nur durch blindes Druecken der Aktionstaste in seinem 1000 x 1000-Rechteck zu finden; die Loesung haengt aber an ihm
  (das Plakat ist absichtlich falsch; Nachtrag 13 Folge 4: "Loesung muss aus Plakat + Dokument + den sichtbaren Ventilen eindeutig
  ableitbar sein"), und Dossier §7 legte es dem Nutzer als "Blatt am Sued-Landeplatz direkt neben der Sicherung" vor (Grundlage der
  Freigabe). Das Plakat dagegen ist im Spielbild deutlich sichtbar (K1 Cut 5). Forderung: das Blatt am freigegebenen Fundort "neben der
  Sicherung" so legen bzw. kenntlich machen, dass es in der Kamera des Landeplatzes (Cut 5; Gegenprobe Cut 6/7) als Blatt erkennbar ist
  (z. B. eine Stelle des Landeplatzes vor dem Gelaender bzw. im freien Bodenbereich von Cut 5, oder an Wand/Gelaender neben der Tuer
  wie das Plakat, oder die RE2-Blink-Form der Spur D (re15_blink, Feuerloescher 1030) — Form mit Beleg, Begruendung im Dossier);
  Zone weiter neben der Sicherung erreichbar; Messung wie oben (Pixel-Unterschied mit/ohne Bit 107 in Cut 5 deutlich > 39, Bild 1x/4x),
  Riegel mit Leon UND Elza; Dossier §7 fuer den Nutzer nachziehen.

* **Mangel 2 (niedrig, Nebenwirkung des Plakats auf Punkt 3) — Schliessen der Plakat-Nahaufnahme hebt die Pad-Sperre der Flut auf.**
  Ist (Lauf `pk`, Leon, `<a4>/plaene/pk.txt` + RE15_PRESS square@3540): richtige Folge -> Warnung -> zurueck zum Plakat, Nahaufnahme offen
  (F2118) -> Zaehler laeuft ab -> `[scd F3524] Cut_chg(4)` / `FLUT: Wasser steigt` (das Flut-Programm setzt Set(2,7)=1/Set(2,2)=1) ->
  Viereck F3540 -> `Plakat-Nahaufnahme zu (nach 1421 Bildern)` -> **`[scd F3555] letterbox closed -> gameplay (player_mode=0)` mitten in der
  Flut** -> der noch offene AKTION-Befehl des Wegplan-Treibers (der waehrend player_mode 2 wartet) loest `Ereignis 23 -> 2030_plakat`
  erneut aus -> `Plakat-Nahaufnahme offen` waehrend der Flut -> `SPRITZER` -> `ERTRUNKEN` (F3585). Ursache im Code: `plakat_tick`
  (ventilkette_r41.c) setzt beim Schliessen (2,7) und (2,2) unbedingt auf 0, und `re15_ventilkette_r41_ereignis` laesst das Plakat auch
  waehrend Zaehler/Flut zu (nur `!s_plakat_offen && !bit(103)`). Folge: unter der Flut-Kamera Cut 4 ist der Spieler frei (Form der
  Pad-Sperre ROOM2030 sub02 @0x01E30 durch den Port gebrochen), das Plakat laesst sich in der Flut oeffnen; der Tod bleibt (am Landeplatz
  Spritzer ~36, Tod ~61 Bilder nach dem Flutbeginn). Forderung: die Sperre der Flut darf durch das Plakat nicht aufgehoben werden (z. B.
  Plakat waehrend Zaehler/Flut nicht oeffnen oder beim Schliessen nur die eigene Sperre zuruecknehmen, wenn keine Flut laeuft) — Form mit
  Beleg, Riegel (Plakat offen bis in die Flut -> nach dem Schliessen player_mode bleibt 2 bis zum Tod).

Hinweise (kein Mangel):
* (b) Fehlerton 0x0B: Aufruf und Bank-Eintrag (PANEL2130.EDT Satz 0x0B `00 00 83 01`) geprueft, nicht mitgeschnitten (RE15_NOAUDIO).
* (c) PSX-Ziel ungebaut: Plakat-Nahaufnahme ueber die PC-Vollbild-Ebene, Wassernetze/Blaetter nur unter RE15_PLATFORM_PC (bekannt, H3).
* (d) Ein Messlauf (sv_6_mit) blieb nach `[flow] EXIT_AT ... -> exit` haengen und endete erst am 200-s-Zeitlimit (allgemeiner
  Beenden-Haenger der exe unter Last, nicht Spur C).
* (e) Fragen (g)/(h) aus frueheren Abnahmen bleiben offen (keine Maengel der Spur); (m)/(n) mit Nachtrag 15 beantwortet.
* (f) 20B0-Auswasch-Szene: nach C-E1 nur mit dem Haken der Spur A; im Integrationsbaum wird `_kette` registriert (Merge-Probe §1 KE).

