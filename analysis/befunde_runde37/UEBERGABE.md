# Runde 37 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde37/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Ressourcen).
Verfahren wie Runde 36 (`analysis/befunde_runde36/UEBERGABE.md` §2-§4, Lehren §6), Release-Ziel **v0.8.25**.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-12 15:45: E INTEGRIERT (r37/integration 330bdd29, gezielt 604/604 gruen).
* 2026-10-12 15:20: E codepanel Abnahme 0 BESTANDEN (E_abnahme_0.md) -> Integrationsbaum `.claude/worktrees/r37_integration` (Zweig r37/integration von master 39ff3c31) angelegt, E GEMERGT (330bdd29; Konflikt nur AUFTRAG.md = master-Fassung, die alle Nachtraege traegt). Bau + gezielte Tests laufen (/tmp/r37int_e.txt). Damit startet F im Hauptlauf. H zombieaktiv Bau FERTIG (RE2-INIT machte aus jedem RE1.5-Deskriptor ausser 0x0d 'Stehen'; RE2 Deskriptor 6 = Gang ab Spawn, am RE2-Original gemessen; grid 0x00 -> RE2-Gang 0x0101; Montage-Nachspawn 1130 als Durchbruch grid 0x0d) — Abnahme H laeuft.
* 2026-10-12 15:00: A Abnahme 0: Leon ROOM1100 ERFUELLT (Nachbau = Nutzer-Marke F892/F240 bildgenau, neue Masken 0 Punkte); M1 blockierend: Elzas ROOM1101 (RDT bytegleich) hat KEINE Masken — systemisch: in STAGE1 fehlen fuer ALLE Elza-Raeume die MASKS-Dateien (Lader nimmt die volle Raum-Id); M2 klein (Leons Stiefelspitze an der Westwand jetzt verdeckt, geometrisch richtig, in den Nutzertext). Orchestrator-Freigabe VERTRAG 1.7 (22812fe9 / A-Baum 0ae925a6): Masken-Rueckfall Elza -> Leon bei gleicher Kamera + Hintergrund, Zensus aller Paare. Nachbesserung A#1 laeuft.
* 2026-10-12 14:45: D itemband Bau FERTIG: Band-Gate fuer JEDEN AOT-Satz (FUN_80042bac @0x80042cac-ccc) + Member 0x12 setzt das Kollisionsband (@0x80041228) — itemband_r37.c + Haken aot_common.c/actor_common.c; am Original gemessen (3010, 2030 SPAS-12/Schrot/Treppe/Lift, 3050); 12 Items, 22 Member-0x12-Schreiber (17 am Spieler: Lifte 3050, 2030 A/B, 50D0, 1170 sub14). Exe: falsches Band gesperrt, vorgesehener Weg frei (3050: Fall -> Lift Band 4->0 -> Kerbe -> Klettern -> Super Redhawk). Nebenbefund ROOM50D0-Lift. Abnahme D laeuft.
* 2026-10-12 14:35: C Abnahme 0: #6 Fahrstuhltuer und #7 Ausstieg ERFUELLT; Nachtrag 5a (Ausstieg ROOM3080 -> 4000) und 5b (RE2-Ton der kleineren Stage-2-Aufzuege) als Maengel -> die Nachbesserung C#1 im Hauptlauf baut beides (der geplante Rest-Lauf C entfaellt). Bestehendes (P27F/P27C, fahrstuhl_r37.c, Haken, Riegel) bleibt.
* 2026-10-12 14:20: E codepanel Bau FERTIG: "Korridor 2" = ROOM10D0/10D1 "2F CORRIDOR" (rote + blaue runde Lampen); 1230/1231 und 11E0/11E1 = SECUGUARD rot/gelb/gruen. Rot leuchtet solange zu, bei richtigem Code blau (10D0) bzw. gruen (SECUGUARD) wie RE2 ROOM4070 (@0x008C6/@0x0088C, Wechsel @0x00DC2-0DD4) und ROOM2130; Kunst = RE2 ESP 0x16 = LAMPE2130.TIM (kein neues Asset). Kamera: Cut_old/Cut_auto erst nach Piep +0 / Aufschliessen +15 (RE2 ROOM2110 @0x01B9C). Hinweis: 11E0/11E1 im Spiel nie offen ((3,139) via ROOM1240 @0x0055A). Abnahme E laeuft.
* 2026-10-12 14:15: Nachtrag 6 (beim Zombie-Biss fehlt teilweise der dranhaengende Arm) -> Spur J bissarm: Baum `.claude/worktrees/r37_bissarm` (Basis 419380dc), EIGENER Workflow `wf_88717cb9-5e3` mit `orchestrator/r37_j.js`. Jetzt vier Workflows (Haupt A-G, H, I, J); Monitor-Skript im Sitzungs-Scratchpad r37_monitor.sh (RUNS-Liste).
* 2026-10-12 14:00: Nachtrag 5 des Nutzers -> Spur C: (a) Ausstieg Lastenaufzug ROOM3080 -> 4000 mit Sequenz, (b) RE2-Ton fuer die kleineren Aufzuege in Stage 2. In master (cc6b68ea) und im C-Baum (df9511a5) in AUFTRAG.md. PLAN: sobald die C-Kette im Hauptlauf endet (Abnahme 0 / Nachbesserungen), Rest-Lauf C mit `stand.C` = "Fortsetzung: Nachtrag 5 (a)+(b) bauen" (r37.js neu aus mk_r37.py erzeugen, C-Punkte tragen Nachtrag 5) -> Bau-Fortsetzung -> Abnahme.
* 2026-10-12 13:50: C fahrstuhl Bau FERTIG: keine der 55 RE2-Tueren ist die gemalte Revier-Fahrstuhltuer -> eigene Port-Archive P27F (Flur) / P27C (Kabine) auf DOOR27-Basis (Ton/SCD bytegleich, neues MD1 zwei glatte Fluegel, Textur aus dem gemalten Fluegelfeld); Ausstieg ROOM1080/1081 mit P27C, ROOM4020/4021 mit DOOR27 V0 (RE2 ROOM6010 @0x004C2); fahrstuhl_r37.c + Haken aot_common.c. Neue Assets shared_assets/RE15DOOR/P27F.DO2 + P27C.DO2 (Paket-Gate prueft RE15DOOR generisch). Offen/Frage: Lastenaufzug ROOM3080 -> 4000 ohne Sequenz (nicht beauftragt). Abnahme C laeuft.
* 2026-10-12 13:35: A pri1100 Bau FERTIG: Zombie an der Marke F892 liegt 468..2329 Kamera-z HINTER der Westwand, wurde aber gemalt, weil die alte Maskenkette jeder 24x24-Kachel das MAXIMUM der Quadertiefe gab (Leckband bis 3253; Masken-OT @0x80039650-660, Figur otz>>4 @0x8002565c, ZSF3 @0x80066c70). Spur O (R36) hatte genau diesen Kopf als "wieder sichtbar" gefuehrt = falsche Richtung. Neu: Masken je Tiefenstufe (C1 52 / C2 53 Rechtecke), beide Marken-Lagen 0 Punkte im Wandfenster, Leon/Schatten unveraendert. Abnahme A laeuft.
* 2026-10-12 13:20: ⛔ Panne: SendMessage an die Workflow-Agenten E und I hat je eine ZWEITE Instanz aus dem Transkript gestartet ("Resuming agent"), die parallel im selben Baum arbeitete; die Workflow-Instanz E meldete das. Beide Kopien per TaskStop beendet (ab480ee892899bd36, a15139f6e0e5ccab7; die Workflow-Instanzen laufen weiter, Journal ohne failed). Ergebnis im Baum: E-Baum traegt Nachtrag 3/3b in AUFTRAG.md (75b08666/985f0ad0) und uncommittete Lampen-Aenderungen der Kopie — die Workflow-Instanz E uebernimmt sie laut ihrer Meldung; I-Baum traegt Nachtrag 4b/4c (11cadf09/99dbe19e). Nachtraege an laufende Spuren ab jetzt NUR noch als Commit in die AUFTRAG.md des Spurbaums (Memory reai-v2-sendmessage-workflow-agent).
* 2026-10-12 13:00: Nachtrag 3b (Panel leuchtet in der GEMALTEN Lampenfarbe) per SendMessage an Bau-Agent E; Nachtrag 4 -> Spur I feuerloescher (Modell bauen; leer am Boden Elza 1091 / Leon 1090 nach dem Loeschen (3,0x84); Loescher aus dem Spind 1000 an einen besseren Ort, Taken (9,0x86)/(3,0x85) bleiben). Baum `.claude/worktrees/r37_feuerloescher` (Basis 64ba31e0), EIGENER Workflow `wf_d8e6232c-55b` mit `orchestrator/r37_i.js`. Jetzt laufen drei Workflows: Haupt A-G (wf_59e74f0e-8ae), H (wf_04596eeb-782), I (wf_d8e6232c-55b).
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
| A pri1100 (Bau fertig, Abnahme laeuft) | r37_pri1100 | ROOM1100 Zombie an der F9-Marke falsch verdeckt (Marken im HAUPTBAUM `re15_port/build/platform/pc/befund_1100_F892_marke1.bmp`, `befund_1100_F240_marke1.bmp`, befund.log MARKE-Block) | gestartet |
| C fahrstuhl (Bau fertig, Abnahme laeuft) | r37_fahrstuhl | Fahrstuhl-Tuer der Tuersequenz korrekt (passende RE2-Tuer oder eigener Bau) + Sequenz beim Aussteigen | gestartet |
| D itemband (Bau fertig, Abnahme laeuft) | r37_itemband | Items nur vom richtigen Band (AOT-Band-Gate @0x80042cac-ccc + Member 0x12 @0x80041228), alle Items erreichbar | gestartet |
| E codepanel (BESTANDEN, integriert) | r37_codepanel | Code-Panels: blau solange falsch, gruen wenn richtig; Kamera bleibt bei Piep + Aufschliessen | gestartet |
| F sperre1050 | r37_sperre1050 | "Chief first"-Sperre auch an ROOM1050 -> 10A0 (Endspiel), ROOM50B1 Elza-Text ohne Ada | gestartet |
| G beretta | r37_beretta | Beretta M93R: jeder Schuss des Feuerstosses trifft (Glock 18 pruefen) | gestartet |
| H zombieaktiv | r37_zombieaktiv | Zombies GENERELL aktiv wie in RE2 (Beispiel Montage nach Irons' Tod) | gestartet (eigener Workflow) |
| I feuerloescher | r37_feuerloescher | Feuerloescher-Modell; leer am Boden 1091 (Elza) / 1090 nach dem Loeschen (Leon); Loescher aus dem Spind 1000 verlegen | gestartet (eigener Workflow) |
| J bissarm | r37_bissarm | beim Zombie-Biss fehlt teilweise der dranhaengende Arm (wie RE2) | gestartet (eigener Workflow wf_88717cb9-5e3) |

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
