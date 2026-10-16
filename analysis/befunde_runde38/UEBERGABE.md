# Runde 38 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde38/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Ressourcen).
Verfahren wie Runde 37 (`analysis/befunde_runde37/UEBERGABE.md` §2-§4), Release-Ziel **v0.8.26**.
Zeiten IMMER aus der Uhr (PowerShell Get-Date) oder der Commit-Zeit, nie schaetzen.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-17 01:10: Integration H/I gruen (168/168 gezielt), Baeume elzatoene/heliport1171 entfernt.
* 2026-10-17 00:50: Integration P gruen (53/53), Baum r37_masken36 entfernt. H gemergt (scd_vm.c beide Seiten, 2a8b9abf), I gemergt (main.c Include beide Seiten, e10d9efb), Bau OK, gezielte Tests H/I laufen.
* 2026-10-17 00:25: H elzatoene BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_10922d64-fbc). Merge nach dem P-Test (mit I).
* 2026-10-17 00:10: I heliport1171 BESTANDEN (Abnahme 0, wf_88be68ba-db7, Bericht e0b6ed35). Merge nach dem P-Test (Konflikte erwartet: enemy_ai_common.c Include mit B, scd_room_setup.c mit F).
* 2026-10-16 23:50: Welle 5 (r38_w5.js, BASIS d70c234e): N wasser wf_9fc9967b-c4b, O montage wf_83f7268d-331. Damit laufen ALLE 21 Spuren (bzw. sind fertig).
* 2026-10-16 23:40: Integration Q/E/J gruen (105/105 gezielt), Baeume kleinkram/elzaintro/rekorder entfernt. P (r37/masken36) gemergt (87ff0737, konfliktfrei), Bau + Masken-Tests laufen. Welle 5: N, O.
* 2026-10-16 23:10: Welle 4 (r38_w4.js, BASIS f587d34e): D zopf wf_08abf2fa-3ac, M npckante wf_d2a07ce1-679. Platte 67 GB. Offen zu starten: N, O.
* 2026-10-16 23:05: P (r37 masken36) BESTANDEN (Abnahme 2, wf_6fbf718a-cd3, Bericht 0ce11f5f). Merge nach dem laufenden Q/E/J-Test.
* 2026-10-16 22:45: Integration: L gruen (210 Unit + 7 Integration), r37_re2zombie-Baum entfernt. Q, E gemergt (konfliktfrei), J gemergt (Konflikte scd_vm.c/audio_pc.c = Haken nebeneinander, beide Seiten via konflikt_beide.py, 63ea4222). Bau + gezielte Tests Q/E/J laufen.
* 2026-10-16 22:20: J rekorder BESTANDEN (Abnahme 0, wf_fe613bfa-201). S fortgesetzt wf_350d90f9-b55 (r38_w2b.js).
* 2026-10-16 22:12: Bau-Fortsetzung R wf_ad7a778e-ca5 (r38.js), U wf_dc602f1a-a19 (r38_w2b.js). Warten: S (stand), D, M, N, O (neu).
* 2026-10-16 22:10: E elzaintro BESTANDEN (Abnahme 0, wf_4385667f-bb0, Bericht f6b23e6d). Merge nach dem laufenden L-Test (mit Q).
* 2026-10-16 22:05: Q kleinkram BESTANDEN (Abnahme 0, wf_a6f0cd46-193, 150/150 gezielt). Merge nach dem laufenden L-Test.
* 2026-10-16 21:55: L (r37 re2zombie) BESTANDEN (Abnahme 2, wf_232032c3-de2, Bericht ca4f0879): Erholungsleiter P3 wie RE2, Texte richtig. In r38_integration gemergt, Bau + Zombie-Tests laufen.
* 2026-10-16 21:40: Nutzer-Nachtrag 2 (AUFTRAG): John darf in 1141 nicht hereinploppen. F-Lauf wf_9ab3a238-dfd GESTOPPT (TaskStop, Mess-exe PID 46072 beendet), Reste 32754a59 UNGEPRUEFT, master (Nachtrag 2) in r38/dining1141 gemergt (0a680a7d). Neue Loesung: John steht schon beim Betreten im Dachgang hinter dem Rolladen, sichtbar erst beim Hochfahren. F neu gestartet mit r38_f.js (Punkt 4 = Nachtrag 2, Hinweise angepasst): wf_0cca805e-279.
* 2026-10-16 21:15: ⛔ SITZUNGSLIMIT ~18:18 (Reset 21:00) brach ALLE Laeufe ab. Reste gesichert: wip UNGEPRUEFT in r38_cursor/dining1141/heliport1171/itemorte/spinnen, Abnahme-TEILBERICHTE in elzaintro/elzatoene/rekorder/spiderman/tagebuch und r37 L/P. Neuer Nutzer-Befund -> AUFTRAG Nachtrag 1 / Spur W (ROOM20A0 Rolladen vor der Kamera), r38_w3.js (BASIS df08d83a). Fortsetzung 21:05-21:15: Abnahme per Resume (Bau aus dem Cache) E wf_4385667f-bb0, H wf_10922d64-fbc, J wf_fe613bfa-201, Q wf_a6f0cd46-193, G wf_fef4a19f-77f, T wf_7931fdbe-adb, V wf_9f166cb7-bd0; L wf_232032c3-de2 / P wf_6fbf718a-cd3 (modus abnahme, runde0 2, max_nach 1); Bau-Fortsetzung mit stand: F wf_9ab3a238-dfd, B wf_74e3029b-16f, I wf_88be68ba-db7, A wf_9a07eaab-cd0, K wf_239fbc09-716; neu W wf_3d75f890-088. Warten: R, S, U (Bau mit stand), D, M, N, O (neu).
* 2026-10-16 18:30: Integration C gruen 27/27 (r38_werkstatt11b0 + szenenleon + Ton/Dialog), Baum r38_werkstatt11b0 entfernt. CPU ~80 %, RAM 12,9 GB frei, Platte 83 GB -> Welle 2b nur teilweise: S wf_8071033e-d91, U wf_7508e561-3ed (r38_w2b.js, BASIS cb782bda). D, M, N, O folgen, wenn weitere Spuren fertig sind.
* 2026-10-16 18:15: C werkstatt11b0 BESTANDEN (Abnahme 0, wf_55628571-c21): "But, hold on...", Schiebe-Ton aus ROOM1171 Satz 7 (11B0 Satz 7 @0xE4A0 leer), keine Fall-Pose (Bank folgt dem Clip). Integrationsbaum r38_integration angelegt (98d35560), C gemergt (869b9374), Bau + gezielte Tests laufen. Offen fuer die Integration: Spur I gegen szenenleon_r36.c (Sub 0x13 auch in 1171), Android-Configure wegen werkstatt11b0_pc.c.
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
| C | r38_werkstatt11b0 | 11B0: "But, hold on...", Schiebe-Ton, keine Fall-Animation | BESTANDEN (A0), integriert |
| D | r38_zopf | Elzas Zopf wie Claire (RE2) | offen |
| E | r38_elzaintro | Elza-Intro Voiceover, Pre-Intro-Schwarz ohne Raum | BESTANDEN (A0), integriert |
| F | r38_dining1141 | John 1021 -> 1141 (Dining Room Key, Dach, Haemmern, Gorilla-Sperre) + Rolltor-Ton 1140 | offen |
| G | r38_tagebuch | Roys Tagebuch 1011 + Kartenmarken Comms/Prison | offen |
| H | r38_elzatoene | Schuss 1041, Roy-Stoehnen + John-Schuss 1021 | BESTANDEN (A1), integriert |
| I | r38_heliport1171 | Elza haengt in 1171 | BESTANDEN (A0), integriert |
| J | r38_rekorder | Rekorder: WAV -> Piep + Tuer + Meldung | BESTANDEN (A0), integriert |
| K | r38_itemorte | Loescher 1030, Schluessel an die Pinnwand-Marke | offen |
| M | r38_npckante | NPC-Kantensprung (John 10B1/1261, Marvin/Ada) | offen |
| N | r38_wasser | Wasser-Effekt 1260+ | offen |
| O | r38_montage | Montage-Zombies Richtung Kamera | offen |
| Q | r38_kleinkram | Inventar-Ton, Elza 1151 Irons, 30E0 Flammenwerfer | BESTANDEN (A0), integriert |
| R | r38_spinnen | Spinnen Stage 2 (Schaechte, Gift, Minispinnen) | offen |
| S | r38_affen3 | kleine Affen Stage 3 treffen | offen |
| T | r38_spiderman | Spiderman Stage 4 + Spieler bleibt auf der Karte (alle Gegner) | offen |
| U | r38_wasserzombie | Zombies ziehen sich am Wasserrand hoch | offen |
| V | r38_releaseskript | Release-Skript alle Plattformen + Version + Archiv | offen |
| L (r37) | r37_re2zombie | Nachbesserung 2: Texte + Inversion re2z_hit_main case 3 | BESTANDEN (A2), integriert |
| P (r37) | r37_masken36 | Nachbesserung 2: Cut 15 3050/3051, Kopfoberkante | BESTANDEN (A2), integriert |

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
