# Runde 41 — Spur C "ventilkette" — Abnahme 4 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD b08bb0c4 (Basis = merge-base master
116e8b0f; master selbst steht auf 200184bc und enthaelt fremde Spuren — Gegenstand ist nur der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Nachtraegen 6-13 und den Entscheiden C-a..e,
C-E1..C-E3, C-E2b (C-E2c widerrufen durch Nachtrag 6). Vorlage: Abnahme 3 (`C_abnahme_3.md`, Maengel 1-4), Nachbesserung 4
(Dossier §0e, Commits a5830413..b08bb0c4).
Begonnen 2026-10-24 ~18:4x. Pruefer: Abnahme 4 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad
`C41abn4/` (unten `<a4>` =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn4`).

**Ergebnis: (in Arbeit)**

## 0. Stand / Protokoll (neueste zuerst)
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


## 2. Gates
(folgt)

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
(folgt)

## 4. Urteil je Nutzer-Punkt
(folgt)

## 5. Maengel
(folgt)
