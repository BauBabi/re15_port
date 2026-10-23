# Runde 41 — UEBERGABE (laufend; neueste Einträge oben)

Für jede nächste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektüre: `CLAUDE.md`, `analysis/befunde_runde41/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Wellen, Ressourcen).
Verfahren wie Runde 38/39 (`analysis/befunde_runde38/UEBERGABE.md` §2-§4). Release-Ziel **v0.8.29**.
Zeiten IMMER aus der Uhr (date) oder der Commit-Zeit, nie schätzen.

## ✅ Fortschritt (neueste zuerst)

* 2026-10-24 00:06: FABLE-LIMIT ~23:45 brach Rest-Lauf 1 ab (wf_4d79520c-fba: 10 Agenten, 5 fertig, 5 abgebrochen, 4,37 Mio Token). Ergebnisse: D Abnahme 0 BESTANDEN (-> r41_integration gemergt, db7205c2); A Abnahme 0 nicht bestanden (M1-M7), Nachbesserung 1 fast fertig (35860e86/42fbd21f, integration_r41_wasser20b0_npc rot in landung_lauf); B Fortsetzung fertig (c2404c61), Abnahme 0 nicht bestanden (M1 Dash/Intro zuenden nicht, M2 Putz, M4-M6), Nachbesserung 1 halb (93f62b6c + 591041e9 gesichert); C zweite Fortsetzung abgebrochen (Flut am Spiel gemessen 76fdeca4, Dossier 19276bbe, Bilder 55f511a8 gesichert); E Bau fertig (ed7bd4ff), Abnahme 0 TEILSTAND (0e6c9a70: P1 teilweise, P3 erfuellt, P2 fehlt); M nicht begonnen. Nutzer-Nachtraege 4 (RE2-Kameraschnitte Alligator, 22:37) und 5 (Kanister-Wurf wie Adas Raketenwerfer-Wurf in RE2, 23:27). Nutzer stellte auf Opus um. REST-LAUF 2 gestartet: wf_a8b604c5-d10 (args orchestrator/args_rest2.json; A/B nachbesserung mit stand, C stand, E abnahme mit Teilbericht, M neu, dazu H/I/J/K/L neu; slots 4, max_nach 2). Neue Baeume r41_kollision/zombiefresser/stage3szenen_a/stage3szenen_b/spinnenanim auf 4c667c97; AUFTRAG/VERTRAG in allen Baeumen = master. F/G folgen auf dem integrierten B.

* 2026-10-23 21:40: SITZUNGSLIMIT ~19:25 (reset 21:30) brach Welle 1 ab (wf_cad46b03-1ad: 8 Agenten, 2 fertig, 6 abgebrochen, 3,16 Mio Token). Stand: A Bau 'teilweise' (13 Commits, Video A_belege/, Abnahme 0 abgebrochen), D Bau 'fertig' (4cbbf4a2, Abnahme 0 abgebrochen), B Bau abgebrochen kurz vor Abschluss (Reste als 86661849 UNGEPRUEFT gesichert), C Bau abgebrochen waehrend der Integrationsgruppe (1745af8e, Baum sauber), E/M nicht begonnen (0 Commits). Orchestrator-Entscheide zu den Fragen von A (E1/E2), D (D1-D3), B (B1-B3), C (a-e) in AUFTRAG.md; Nutzer-Nachtraege 1-3 (DuckStation FFmpeg/mp4, Video Pflicht, NPC-Schritte = Vorgabe). REST-LAUF 1 gestartet: wf_4d79520c-fba, args orchestrator/args_rest1.json (A/D modus abnahme mit bau aus dem Journal, B/C stand-Fortsetzung, E/M neu, slots 4, max_nach 2).

* 2026-10-23 16:58: WELLE 1 gestartet: Workflow wf_cad46b03-1ad (Skript orchestrator/r41-runde.js, args {welle:[A,B,C,D,E,M], slots:4, basis:116e8b0f, max_nach:2}), Journal ~/.claude/projects/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/subagents/workflows/wf_cad46b03-1ad/. Baeume r41_wasser20b0 / re2gator / ventilkette / kleinkram_a / kleinkram_b / masken36 auf 116e8b0f (RE2-Overlays EM23_OVL_0000.BIN u.a. versioniert, 116e8b0f). Waechter (scratchpad/waechter41.sh) meldet Haenger > 75 min.

* 2026-10-23 16:36: Auftrag des Nutzers (Wortlaut in AUFTRAG.md) nach dem Release v0.8.28 (f632a17b). Nutzer-Vorgabe
  zum Ablauf: "Brich all diese Aufgaben in immer wenige parallele Tasks runter" -> Wellen zu je 4 gleichzeitigen Spuren
  (Semaphor), Integration nach jeder Welle, volle Suite einmal vor dem Release. Vorarbeit gelesen: Runde-36/38/39-
  Vertraege, Workflow-Vorlage r36-rest3.js, Memory (Worktree-, Limit-, Haenger-Lehren), Gator-Memory
  (reai-v2-gator-bosskampf.md), RE2-EM23-Vorarbeit (analysis/befunde_2026-09-19/gator-finisher-sounds.md).
  Docker/WSL-Haenger des Release-Laufs 2: Distribution docker-desktop stand nach dem Neustart auf "Uninstalling";
  `wsl --unregister docker-desktop` (Daten-vhdx bleibt), Docker Desktop legte sie neu an, alle Images erhalten.

## 1. Spuren (Status)
siehe VERTRAG.md §0; Status je Spur wird hier nachgetragen (Bau / Abnahme N / bestanden / integriert).

## 2. Bei einem Abbruch (Sitzungs-/Wochen-/Monatslimit)
1. Verwaiste Prozesse beenden (`tasklist`; nur per PID, nie /IM), DuckStation `settings.ini` EnableGDBServer = false prüfen.
2. Je Baum: `git -C .claude/worktrees/r41_<x> status --short` — Reste als `wip(r41-<x>): ... UNGEPRUEFT` committen.
3. Journal des Workflows lesen (`~/.claude/projects/c--workspace-git-reAi-v2/<sitzung>/subagents/workflows/<run>/journal.jsonl`),
   je Spur feststellen: Bau fertig? Abnahme Nr.? Dann Rest-Lauf mit args: unfertiger Bau -> `stand.X`; Abnahme offen ->
   `modus.X='abnahme'` + `bau.X`; Nachbesserung -> `modus.X='nachbesserung'` + `abn.X` + `runde0.X`
   (Muster `analysis/befunde_runde36/orchestrator/args_rest*.json`; Skript `analysis/befunde_runde41/orchestrator/r41-runde.js`).

## 3. Integration (wenn Spuren bestanden sind)
Integrationsbaum `.claude/worktrees/r41_integration` von master; je bestandene Spur `git merge --no-ff r41/<x>`; Konflikte
mit `analysis/befunde_runde36/orchestrator/konflikt_beide.py <dateien>` (beide Seiten); dann `bash re15_port/tools/local_build.sh
configure && ... build`; gezielte Tests je Spur + `ctest -j 6 -R '^(unit_|prop_|probe_)'` (Querbezüge, Memory
reai-v2-gezielte-integration-querbezug); ⛔ neue gemeinsame Regeln können ALTE Riegel widerlegen — nach dem Merge alle
Riegel greppen, die dieselben Bits/Räume setzen. master ff, Hauptbaum-exe neu bauen (Nutzer testet dort).

## 4. Release v0.8.29
Release Notes oben in release/RELEASE_NOTES.md, `bash release/release_alles.sh --version v0.8.29 --tag --linux-jobs 6`
(volle Suite im Linux-Container ist das Gate), Bäume entfernen, Memory, Abschlussmeldung.
