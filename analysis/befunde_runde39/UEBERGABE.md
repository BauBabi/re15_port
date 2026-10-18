# Runde 39 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde39/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Wellen, Ressourcen).
Verfahren wie Runde 38 (`analysis/befunde_runde38/UEBERGABE.md` §2-§4), Release-Ziel **v0.8.27**.
Zeiten IMMER aus der Uhr (Get-Date/date) oder der Commit-Zeit, nie schaetzen.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-18 02:36: WELLE 1 GESTARTET auf master 3e49ff87 (nach Release v0.8.26 754f67ad), je Spur ein eigener Lauf von
  orchestrator/r39.js (Waechter je Lauf). Resume nur mit IDENTISCHEN args `{"welle": ["<X>"], "slots": 1, "max_nach": 1,
  "basis": "3e49ff87"}`: A stimmen wf_28cb3826-7cd, B toene wf_6b21d705-6d7, C inventar wf_5fada65b-ca3,
  L optik wf_844e8fa6-7d9. Baeume .claude/worktrees/r39_{stimmen,toene,inventar,optik}.
* 2026-10-18 01:50: Vorbereitung waehrend des Release-Laufs 3 von v0.8.26 (keine Agenten, keine Last): VERTRAG.md (12 Spuren
  A-L, 3 Wellen zu je 4), orchestrator/r39_kopf.js (Spuren + Wortlaut + Einstiege) ->
  mk_r39.py -> r39.js (Rumpf aus r38.js, Syntax geprueft), mk_wt39.sh, nutzer/ (map*_correct.png, F9-Marken 1100 21:49 und
  1090 23:31 + befund.log-Auszug). Offene Nutzerfrage: J Punkt 5 "this is Roy!" — Roy ist in 1141 schon tot, es klopft
  John (Antwort in AUFTRAG.md nachtragen, Spur J liest sie dort).

## 1. Start einer Welle
1. Release v0.8.26 muss stehen (Tag v0.8.26 auf master). Basis der Baeume = master.
2. Baeume: `bash analysis/befunde_runde39/orchestrator/mk_wt39.sh <thema>` je Spur der Welle; `git worktree list` pruefen
   (Memory reai-v2-worktree-veralteter-basisstand).
3. Workflow: scriptPath `analysis/befunde_runde39/orchestrator/r39.js`, args `{"welle": ["A","B","C","L"], "slots": 4,
   "max_nach": 1}` (Welle 2: D,E,F,H; Welle 3: G,I,J,K). Waechter (Agent-Haenger, Platte) wie Runde 38.
4. Nach der Welle: Integration (Runde 38 UEBERGABE §3), gezielte Tests, master, Hauptbaum-exe bauen, Baeume entfernen.

## 2. Bei einem Abbruch
Wie Runde 38 UEBERGABE §2 (Resume mit identischen args aus dem Sitzungs-JSONL; Reste als wip committen).
