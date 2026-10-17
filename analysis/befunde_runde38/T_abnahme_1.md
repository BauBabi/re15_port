# Runde 38 — Spur T "spiderman" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Pruefer: Abnahme-Agent 1, 2026-10-16. Baum `.claude/worktrees/r38_spiderman`, Zweig `r38/spiderman`, gepruefter Stand
0f6e41d7 (Nachbesserung 1 abgeschlossen), Basis (merge-base mit master) e2cc7673. Massstab: AUFTRAG.md (Wortlaut),
VERTRAG.md §1.4/§2, CLAUDE.md STOP-GATE, Maengel M1/M2 aus `T_abnahme_0.md`. Bericht wird fortlaufend geschrieben
(Sitzungslimit).

## 0. Stand der Pruefung
- [x] Pflichtlektuere: AUFTRAG.md, VERTRAG.md, Dossier T_spiderman.md (inkl. Abschnitt "Nachbesserung 1"),
      T_abnahme_0.md, `git log master..HEAD` (8 NB1-Commits 4e3b44b5..0f6e41d7 auf Abnahme 0 1488abd8),
      `git diff master...HEAD` (Spur-Diff 25 Dateien). Die in `git diff master` (zwei Punkte) zusaetzlich geloeschten
      Orchestrator-Dateien und AUFTRAG/VERTRAG-Abweichungen (Nachtraege 1/2, Spur W) sind master-Fortschritt nach e2cc7673.
- [x] Code-Diff gelesen (NB1: `git diff 1488abd8..HEAD -- re15_port`): sm_anim_halten an den Dateianfang verlegt und an
      Austritt-Ende, Griff 1 P0/P1/P5, Griff 2 P0/P1/P5 eingesetzt; P2 beider Griffe `anim_frame = 0`; neues
      re15_r38_sm_bruellen (B[2]) mit Haken in enemy_ai_common.c case 2; Test test_r38_spiderman.c um gezeichnete Pose
      erweitert.
- [x] Bau (diese Sitzung): `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`;
      `... build` -> `ninja: no work to do.`, `LOCAL-BUILD-OK (build)`. re15_pc.exe 2026-10-16 23:06:09, spiderman_r38.c.obj
      23:06:03 (Quelle 23:05:49), enemy_ai_common.c.obj 23:02:13 (Quelle 23:01:17); `find -newer re15_pc.exe` ueber
      engine/include/platform leer; Arbeitsbaum sauber (= 0f6e41d7; letzter Code-Commit 2029729b 23:07:52).
- [x] gezielte Tests (204/204 gruen, Abschnitt 1)
- [ ] Messung Punkt 1 (M1) an der exe
- [ ] Messung Punkt 2 an der exe (nicht verschlechtert)
- [ ] RE-Gate (Stichproben NB1)
- [ ] Vertrag
- [ ] Urteil

## 1. Gezielte Tests (diese Sitzung)
`ctest --test-dir re15_port/build -R "<R>" --timeout 600 -j 5 --output-on-failure` mit R = Auswahl des Bau-Agenten
(`r38_|tyrant|_2b|t2b|5040|5070|4001|4000|beretta|victim|grab|griff|bissarm|1210|dog|hund|re2z|crawl|kriech|affen|gorilla|
maggot|devour|finisher|pushoff|1220|11c0|aim|ivy|enemy_ai|roster|stage4|stage5|spider|spinne|zombie|schatten|shadow|damage|
schuss|treffer|hit|crow|kraehe|gator|g5|tentakel|zellenarm|gitter|birkin|opfer|anker|anchor`) **erweitert um
`shootable|elzaems|anim_`** (unit_enemy_shootable schiesst auf 0x2B, unit_r37_elzaems_* laden die EM2B-Baenke, unit_anim_*
pruefen den Anim-Treiber, den die Nachbesserung am Clip-Ende umgeht) -> **`100% tests passed, 0 tests failed out of 204`,
`Total Test time (real) = 204.49 sec`** (Maschine parallel von anderen Agenten belastet: r37g/r37n/r38c/r38e-Messkopien liefen).
Darin unit_r38_spiderman, alle zehn unit_r38_griff_*, unit_tyrant_ai, integration_r37_beretta, integration_r36_schuss,
integration_r36_elza_schuss, integration_r37_szene11c0.

test_r38_spiderman direkt gestartet (Ausgabe `abn1t/t_spiderman_out.txt` im Scratchpad):
```
== austritt (ROOM4001, Ck(4,20)=1, Ck(3,0x21)=1) ==
  Wechselbild: [1/1/0] Clip 0x1D Bild 34, gezeichnete Wurzel (0,-940,0); Clip 0x1D Bild 34 (0,-940,0), Bild 0 (-1683,1278,0)
  erstes ACTIVE-Bild: [1/2/1] Clip 0x2 Bild 1 frac 6, gezeichnete Wurzel (0,-946,0), Abstand zur Austritts-Endpose 6 (Clip-Ziel 21)
  Bruellen: Fertig-Bild 1, P2-Bild 1, Fehler 0 (Clip 2 fc 50)
== griff1 vorn/hinten Lauf A ...  Latch Bild 108: Spieler-Anker (-14022,-8593), Greifer-Anker (-14022,-8593), aca59 0
  Wechselbilder: Zugriff-Ende 1, Halte-Start 1, Loese-Ende 0, P6 0, Kollaps-Ende 1, Fehler 0; Ruecksprung Modul 0
== griff1 Lauf B (Ecke) ... Latch Bild 95: Spieler-Anker (-13977,-7782) == Greifer-Anker; Fehler 0
== griff1 Losreissen ... Zugriff-Ende 3, Halte-Start 3, Loese-Ende 2, P6 2, Kollaps-Ende 0, Fehler 0
== griff2 ... Griff-2-Wechselbilder: Zugriff-Ende 1, Halte-Start 1, Loese-Ende 1, P6 1, Fehler 0; Ruecksprung 0
test_r38_spiderman: 0 FAIL
```
