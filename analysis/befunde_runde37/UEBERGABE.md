# Runde 37 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde37/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Ressourcen).
Verfahren wie Runde 36 (`analysis/befunde_runde36/UEBERGABE.md` §2-§4, Lehren §6), Release-Ziel **v0.8.25**.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-12 12:40: Nachtrag 3 des Nutzers (Code-Panel "Korridor 2": ROT solange zu, BLAU wenn richtig; "rot = geschlossen") per SendMessage an den laufenden Bau-Agenten E (ab480ee892899bd36) uebermittelt, in AUFTRAG.md (master) und in den E-Punkten von mk_r37.py nachgetragen. ⛔ Die laufende Abnahme E im Workflow wf_59e74f0e-8ae kennt nur die alten Punkte (blau/gruen) aus dem Skript — AUFTRAG.md im E-Baum ist massgeblich (der Agent soll den Nachtrag dort committen); faellt die Abnahme deshalb falsch aus, Rest-Lauf fuer E mit modus abnahme und dem neuen r37.js (mk_r37.py erzeugen).
* 2026-10-12 12:10: Nachtrag 2 + Ergaenzung des Nutzers -> Spur H zombieaktiv (GENERELL alle Zombies, Vorlage RE2). Baum `.claude/worktrees/r37_zombieaktiv` (Zweig r37/zombieaktiv, Basis 44881ac2), EIGENER Workflow `wf_04596eeb-782` mit `orchestrator/r37_h.js` (args `{welle:[H], slots:1, max_nach:2}`). Hauptlauf `wf_59e74f0e-8ae` (A/C/D/E/F/G, Basis f1e019ca) laeuft unveraendert weiter. Zusammen 5 parallele Agenten. Der Nutzer hat die Lesart der Runde bestaetigt ("ja, perfekt.").
* 2026-10-12 11:40: Workflow GESTARTET: Run `wf_59e74f0e-8ae` (args `{welle:[A,C,D,E,F,G], slots:4, max_nach:2}`), Journal `~/.claude/projects/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/subagents/workflows/wf_59e74f0e-8ae/journal.jsonl`.
* 2026-10-12 11:30: Runde aufgesetzt. Basis master f1e019ca (= v0.8.24 + Auftrag/Vertrag). Sechs Spur-Baeume
  `.claude/worktrees/r37_{pri1100,fahrstuhl,itemband,codepanel,sperre1050,beretta}` (Zweige `r37/<thema>`, alle auf
  f1e019ca). Orchestrator `orchestrator/r37.js` (erzeugt von `orchestrator/mk_r37.py` aus dem Runde-36-Skript: Bau ->
  unabhaengige Abnahme -> bis 2 Nachbesserungen; Modi ueber args wie Runde 36: `modus.X`, `stand.X`, `abn.X`, `bau.X`,
  `runde0.X`). Start: Workflow mit `{welle:[A,C,D,E,F,G], slots:4, max_nach:2}` — Run-Id folgt.

## 1. Spuren

| Spur | Baum / Zweig | Inhalt | Stand |
|---|---|---|---|
| A pri1100 | r37_pri1100 | ROOM1100 Zombie an der F9-Marke falsch verdeckt (Marken im HAUPTBAUM `re15_port/build/platform/pc/befund_1100_F892_marke1.bmp`, `befund_1100_F240_marke1.bmp`, befund.log MARKE-Block) | gestartet |
| C fahrstuhl | r37_fahrstuhl | Fahrstuhl-Tuer der Tuersequenz korrekt (passende RE2-Tuer oder eigener Bau) + Sequenz beim Aussteigen | gestartet |
| D itemband | r37_itemband | Items nur vom richtigen Band (AOT-Band-Gate @0x80042cac-ccc + Member 0x12 @0x80041228), alle Items erreichbar | gestartet |
| E codepanel | r37_codepanel | Code-Panels: blau solange falsch, gruen wenn richtig; Kamera bleibt bei Piep + Aufschliessen | gestartet |
| F sperre1050 | r37_sperre1050 | "Chief first"-Sperre auch an ROOM1050 -> 10A0 (Endspiel), ROOM50B1 Elza-Text ohne Ada | gestartet |
| G beretta | r37_beretta | Beretta M93R: jeder Schuss des Feuerstosses trifft (Glock 18 pruefen) | gestartet |
| H zombieaktiv | r37_zombieaktiv | Zombies GENERELL aktiv wie in RE2 (Beispiel Montage nach Irons' Tod) | gestartet (eigener Workflow) |

## 2. Bei einem Abbruch (Sitzungs-/Wochenlimit)
1. Verwaiste Prozesse beenden (`tasklist`; nur per PID, nie /IM), DuckStation `settings.ini` EnableGDBServer = false pruefen.
2. Je Baum: `git -C .claude/worktrees/r37_<x> status --short` — Reste als `wip(r37-<x>): ... UNGEPRUEFT` committen.
3. Journal des Workflows lesen (`~/.claude/projects/c--workspace-git-reAi-v2/<sitzung>/subagents/workflows/<run>/journal.jsonl`),
   je Spur feststellen: Bau fertig? Abnahme Nr.? Dann Rest-Lauf mit args: unfertiger Bau -> `stand.X` (Text: Commits,
   was fehlt); Abnahme offen -> `modus.X='abnahme'` + `bau.X`; Nachbesserung -> `modus.X='nachbesserung'` + `abn.X` +
   `runde0.X`. Muster: `analysis/befunde_runde36/orchestrator/args_rest*.json`.

## 3. Integration (wenn Spuren bestanden sind)
Integrationsbaum `.claude/worktrees/r37_integration` von master; je bestandene Spur `git merge --no-ff r37/<x>`; Konflikte
(meist Installer-/Include-Zeilen) mit `analysis/befunde_runde36/orchestrator/konflikt_beide.py <dateien>` (beide Seiten),
dann von Hand pruefen; `bash re15_port/tools/local_build.sh configure && ... build`; gezielte Tests
`PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build -R "^(unit_|probe_|prop_)|r36_|r37_|<module>" -E android_bash
--timeout 2700 -j 6`. ⛔ Neue gemeinsame Mechanik kann ALTE Riegel widerlegen (Runde 36: Ruhe-Zweig vs. Generator-Riegel) —
rote Alt-Riegel erst am Original klaeren, dann Riegel ODER Code aendern.

## 4. Release v0.8.25
Wie Runde 36 §4: master in Integration, volle Suite einmal seriell ohne Agenten (`local_build.sh all`, ~2,7 h),
RE15_MIN_TESTS anheben (698 -> neu, Verlaufszeile), Release Notes (Entwurf `orchestrator/RELEASE_NOTES_v0.8.25_ENTWURF.md`),
master ff, Hauptbaum-exe bauen, `build_win_cross.sh`, `build_linux_deck.sh` (bei Rot nach Portabilitaet suchen: popen ohne
_DEFAULT_SOURCE, CMake-Skripte ohne cmake_minimum_required; zweiter Lauf mit `RE15_CTEST_JOBS=3`), `build_android.sh --version
v0.8.25`, `make_package.sh --version v0.8.25`, Archiv (`C:\workspace\Re15Data\re15_packages_archiv\v0.8.25`, sha256sum -c,
v0.8.24 entfernen), Release-Commit (`git commit -F`), Tag, Push, Baeume entfernen, Memory, Abschlussmeldung.
