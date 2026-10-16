# Runde 38 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde38/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Ressourcen).
Verfahren wie Runde 37 (`analysis/befunde_runde37/UEBERGABE.md` §2-§4), Release-Ziel **v0.8.26**.
Zeiten IMMER aus der Uhr (PowerShell Get-Date) oder der Commit-Zeit, nie schaetzen.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-16 16:40: Welle 2a laeuft (r38_w2.js, BASIS 1bcbd0ff, Baeume 16:28-16:39): Q wf_a6f0cd46-193, C wf_55628571-c21, K wf_f51a7d4d-ccc, E wf_4385667f-bb0, J wf_fe613bfa-201, A wf_539a5d00-022. Damit 16 Laeufe gleichzeitig. Platte 111 GB frei (vor den Builds) -> Welle 2b (D, M, N, O, S, U) erst, wenn erste Spuren integriert und ihre Baeume entfernt sind.
* 2026-10-16 16:30: Welle 1 laeuft (je Spur ein Workflow, max_nach 2): L wf_64c0b503-94f (r37_l_forts.js), P wf_4979f9b2-77a (r37_p_forts.js; beide Zweige vorher mit master gemergt: L 44721dca, P 2bb1bbe3, Konflikte nur AUFTRAG/VERTRAG -> master), F wf_27c697b6-a0e, G wf_fef4a19f-77f, B wf_904f6df5-9b8, T wf_7931fdbe-adb, R wf_679cb64c-b33, H wf_10922d64-fbc, I wf_43b458d6-510, V wf_9f166cb7-bd0 (alle r38.js, BASIS e2cc7673). Baeume Welle 1 angelegt 16:17-16:27, Platte danach 143 GB frei. Welle 2 (A, C, D, E, J, K, M, N, O, Q, S, U) folgt nach Last/Platte.
* 2026-10-16 16:10: Runde 38 angelegt (Auftrag woertlich, Vertrag mit 20 Spuren A-K, M-O, Q-V + Fortsetzung L/P der
  Runde 37). Orchestrator `orchestrator/r38.js` aus `mk_r38.py` (Vorlage `befunde_runde37/orchestrator/r37_r.js`), je
  Spur ein eigener Workflow-Lauf `args {"welle": ["X"], "max_nach": 2}`; L/P mit `befunde_runde37/orchestrator/r37_l.js`
  / `r37_p.js` im Modus `nachbesserung` (Stand aus den Journalen wf_8e797ea4-b39 / wf_62590deb-1b8, runde0 1).
  Neue Sprachdateien des Nutzers (synchro/) und die Nutzer-Bilder (nutzer/) committet.

## 1. Spuren (Status)

| Spur | Baum | Thema | Status |
|---|---|---|---|
| A | r38_cursor | Cursor-Schatten 1150/11F0 | offen |
| B | r38_szenenschutz | kein Angriff in Szenen ohne Bewegung (1190/11D0), Ausnahme 1171 | offen |
| C | r38_werkstatt11b0 | 11B0: "But, hold on...", Schiebe-Ton, keine Fall-Animation | offen |
| D | r38_zopf | Elzas Zopf wie Claire (RE2) | offen |
| E | r38_elzaintro | Elza-Intro Voiceover, Pre-Intro-Schwarz ohne Raum | offen |
| F | r38_dining1141 | John 1021 -> 1141 (Dining Room Key, Dach, Haemmern, Gorilla-Sperre) + Rolltor-Ton 1140 | offen |
| G | r38_tagebuch | Roys Tagebuch 1011 + Kartenmarken Comms/Prison | offen |
| H | r38_elzatoene | Schuss 1041, Roy-Stoehnen + John-Schuss 1021 | offen |
| I | r38_heliport1171 | Elza haengt in 1171 | offen |
| J | r38_rekorder | Rekorder: WAV -> Piep + Tuer + Meldung | offen |
| K | r38_itemorte | Loescher 1030, Schluessel an die Pinnwand-Marke | offen |
| M | r38_npckante | NPC-Kantensprung (John 10B1/1261, Marvin/Ada) | offen |
| N | r38_wasser | Wasser-Effekt 1260+ | offen |
| O | r38_montage | Montage-Zombies Richtung Kamera | offen |
| Q | r38_kleinkram | Inventar-Ton, Elza 1151 Irons, 30E0 Flammenwerfer | offen |
| R | r38_spinnen | Spinnen Stage 2 (Schaechte, Gift, Minispinnen) | offen |
| S | r38_affen3 | kleine Affen Stage 3 treffen | offen |
| T | r38_spiderman | Spiderman Stage 4 + Spieler bleibt auf der Karte (alle Gegner) | offen |
| U | r38_wasserzombie | Zombies ziehen sich am Wasserrand hoch | offen |
| V | r38_releaseskript | Release-Skript alle Plattformen + Version + Archiv | offen |
| L (r37) | r37_re2zombie | Nachbesserung 2: Texte + Inversion re2z_hit_main case 3 | offen |
| P (r37) | r37_masken36 | Nachbesserung 2: Cut 15 3050/3051, Kopfoberkante | offen |

## 2. Bei einem Abbruch (Sitzungs-/Wochen-/Monatslimit)
1. Verwaiste Prozesse beenden (`tasklist`; nur per PID, nie /IM), DuckStation `settings.ini` EnableGDBServer = false pruefen.
2. Je Baum: `git -C .claude/worktrees/r38_<x> status --short` — Reste als `wip(r38-<x>): ... UNGEPRUEFT` committen.
3. Journal des Workflows lesen (`~/.claude/projects/c--workspace-git-reAi-v2/<sitzung>/subagents/workflows/<run>/journal.jsonl`),
   je Spur feststellen: Bau fertig? Abnahme Nr.? Dann Rest-Lauf mit args: unfertiger Bau -> `stand.X` (Text: Commits,
   was fehlt); Abnahme offen -> `modus.X='abnahme'` + `bau.X`; Nachbesserung -> `modus.X='nachbesserung'` + `abn.X` +
   `runde0.X`. Muster: `analysis/befunde_runde36/orchestrator/args_rest*.json`.

## 3. Integration (wenn Spuren bestanden sind)
Integrationsbaum `.claude/worktrees/r38_integration` von master; je bestandene Spur `git merge --no-ff r38/<x>` (L/P:
`r37/re2zombie`, `r37/masken36`); Konflikte (meist Installer-/Include-Zeilen, k_port[]) mit
`analysis/befunde_runde36/orchestrator/konflikt_beide.py <dateien>` (beide Seiten), AUFTRAG/VERTRAG/UEBERGABE vom master;
dann `bash re15_port/tools/local_build.sh configure && ... build`; gezielte Tests je Spur; ⛔ neue gemeinsame Regeln
koennen ALTE Riegel widerlegen (Runde 37: Q-Kistenregel gegen 3 Alt-Riegel) — nach dem Merge alle Riegel greppen, die
dieselben Bits/Raeume setzen.

## 4. Release v0.8.26
master in die Integration, volle Suite einmal seriell ohne Agenten (`local_build.sh all`, ~3,6 h, 788+ Tests),
RE15_MIN_TESTS anheben (Verlaufszeile), Release Notes (Entwurf `orchestrator/RELEASE_NOTES_v0.8.26_ENTWURF.md`), master ff,
Hauptbaum-exe bauen, dann das neue Release-Skript der Spur V (Windows + Linux/Deck + Android, Paket, Archiv, alte Version
aus dem Archiv) — oder bei Problemen die Einzelschritte wie Runde 37 §4; Release-Commit (`git commit -F`), Tag, Push,
Baeume entfernen, Memory, Abschlussmeldung.
