# ⛔ RUNDE 36 — UEBERGABE (Stand 2026-10-11 09:40, Wochenlimit Opus erreicht, Reset 17.10. 08:00)

Dieses Dokument ist fuer den NAECHSTEN Account/die naechste Sitzung geschrieben, ohne Kontext der alten.
Alles liegt LOKAL auf dieser Maschine (Zweige `r36/*`, Arbeitsbaeume unter `C:/workspace/git/reAi_v2/.claude/worktrees/r36_*`).
Nichts davon ist gepusht. Arbeite die Abschnitte 2 → 3 → 4 der Reihe nach ab; hake hier ab und committe dieses Dokument
nach jedem Schritt (`git -C C:/workspace/git/reAi_v2 commit`), damit ein weiterer Abbruch nichts verliert.

Pflichtlektuere vorher: `CLAUDE.md` (STOP-GATE, Build ueber `local_build.sh`), `analysis/befunde_runde36/AUFTRAG.md`
(Wortlaut des Nutzers + Nachtraege 14:00 und abends), `analysis/befunde_runde36/VERTRAG.md` (Bits/IDs/Ereignisse/Dateien je
Spur), auf dem Integrationszweig `analysis/befunde_runde36/NACHLAUF.md` (Funde ausserhalb der Spur-Wortlaute).

## ✅ Fortschritt seit der Uebergabe (fortlaufend, neueste zuerst)
* 2026-10-11 18:30: F INTEGRIERT (238a70db + NACHLAUF 47-50 f6bfef5a; gezielt 645/645 gruen, 1167 s). Damit alle 17 Spuren A..Q in r36/integration. Offen NUR Spur R (Nachbesserung 1 laeuft: Dossier 4.3 ROOM2070-Tuer, Zensusdatei, Pin 3050/2050 — kein Verhaltens-Code). Release-Notes-Entwurf vollstaendig bis auf @@SUITE@@ (4258471f).
* 2026-10-11 18:00: F Abnahme 6 BESTANDEN (Bericht F_abnahme_6.md d6fe3cc9: P1/P2/P3 erfuellt, keine Maengel; Hinweise: Flugklemme Pad-Mitte OFFEN @0x80118c3c-cc4, Anker-400 +8 %, Controller-Artefakt ohne RE15_INPUT_SCRIPT, PSX-Laufzeit offen). F in r36/integration GEMERGT (238a70db; Konflikt enemy_ai_common.c Includes = beide Seiten). Bau + gezielte Tests laufen (/tmp/r36int_f_lauf.txt). Offen nur noch R: Nachbesserung 1 (Dossier 4.3 + Zensusdatei + Pin 3050/2050) laeuft, danach Abnahme 1 -> mergen -> Abschnitt 4.
* 2026-10-11 17:40: R Abnahme 0 (Bericht R_abnahme_0.md, 03a64b28): Punkte 1-3 ERFUELLT (Softlock NEIN, Port = Original, vom Abnahme-Agenten selbst an exe UND Original (DuckStation-GDB) wiederholt, alle Adressen selbst disassembliert), Punkt 4 teilweise -> bestanden=false NUR wegen Dossier/Test: M1 Dossier 4.3 falsch (ROOM2070/2071 main00 @0x00B1C hat eine unbedingte Tuer slot 5 -> 2040), M2 Ersatzweg 2030->2060->2040 unbelegt (Etagen), M3 kein Pin fuer 3050/2050. Nachbesserung R#1 laeuft automatisch (nachbesserung:R-dachkiste#1; kein Verhaltens-Code). F Abnahme 6 laeuft noch. Release-Notes-Entwurf: F/R-Absaetze + F-Grenze eingetragen (f9c23900), R-Satz zu 2040 auf den belegten Weg ueber ROOM2070 berichtigt.
* 2026-10-11 17:10: R Bau FERTIG (f487909d: Dach-Kiste ROOM1090/1091 — Softlock NEIN, Port = Original: jeder Schub an der Bruestung SCHUB-GESPERRT ueber FUN_8003b558(obj,2) @0x8002bfb0 / Sperre @0x8002c0b0 wie im Original; KEIN Fix noetig; aot_common.c-Aenderung = 3-Zeilen-Messhaken RE15_PUSH_LOG ohne Verhalten; Riegel integration_r36_dachkiste 1/1, Unit-Gruppen 18/18) -> Abnahme 0 laeuft (abnahme:R-dachkiste#0). F NB6 FERTIG (654b27e0: Ostgasse hinter Pad #36 Angriff 157 statt nie, exe-Stelle 8 468 -> 0, Nachruecken Biss 105 statt 588 Bilder nach dem Tod, 14 Stellen 7500 -> 6695, FERN 300 -> 255, gezielt 232/232 + 25/25) -> Abnahme 6 laeuft (abnahme:F-zombieki#6). Danach je Ergebnis: Abschnitt 3 (mergen), F notfalls mit Grenze.
* 2026-10-11 16:55: Abschnitt 4 Schritt 10 VORGEZOGEN: die 16 gemergten Spur-Baeume r36_<x> entfernt (git worktree remove --force; alle Zweige r36/<x> bleiben und sind in r36/integration enthalten; vorher geprueft: in r36/integration gemergt, Baeume sauber bis auf r36_schuss/mess_r36c = 38 MB Bau-Logs). Platte 71 -> 180 GB frei. Uebrig: r36_integration, r36_zombieki (F), r36_dachkiste (R). master (bis ca6e8630) in r36/integration gemergt (0101ff78, nur Doku/Orchestrator-Dateien, kein Bau noetig).
* 2026-10-11 16:45: N INTEGRIERT (cb1d5f59, gezielt 618/618 gruen, 1014 s). Integriert 16/17 der Spuren A..Q (alle ausser F) + Spur R offen. Laufen: F Nachbesserung 6 (Agent nachbesserung:F-zombieki#6) und R Bau-Fortsetzung (Agent bau:R-dachkiste-forts) in wf_aa00e266-7a9. Danach: F/R mergen (Abschnitt 3), dann Abschnitt 4 (Release).
* 2026-10-11 16:25: Rest-Lauf 5 GESTARTET: Workflow `wf_aa00e266-7a9` mit `orchestrator/args_rest5_FR.json` (F Nachbesserung 6 Fortsetzung = Agent nachbesserung:F-zombieki#6, R Bau-Fortsetzung = Agent bau:R-dachkiste-forts; 2 Plaetze, Modell = Sitzung/Fable). Journal: `~/.claude/projects/c--workspace-git-reAi-v2/c41eae99-.../subagents/workflows/wf_aa00e266-7a9/journal.jsonl`. N-Integration: Bau OK (LOCAL-BUILD-OK), gezielte Tests laufen noch (Ergebnis in /tmp/r36int_n_lauf.txt). Bricht die Sitzung ab: Workflow mit resumeFromRunId wf_aa00e266-7a9 fortsetzen ODER Baeume r36_zombieki/r36_dachkiste sichern (wip UNGEPRUEFT) und Rest-Lauf 6 mit neuem stand/abn starten.
* 2026-10-11 16:15: FABLE-SITZUNGSLIMIT ~13:20 (Reset 16:10) beendete F (NB6, bis 30c2ecb3 NB6g) und R (bis 4911c7b4: ORIGINAL laesst die Dach-Kiste 1090 NICHT bewegen; Port-Zensus mit Spieler 0 Ausloeser). Reste als wip UNGEPRUEFT gesichert (F b2177510, R 085940e6 inkl. ungebauter aot_common.c-Aenderung); 15 verwaiste Monitore + eine stehende Mess-exe beendet; DuckStation EnableGDBServer = false geprueft. N kartenitems Abnahme 1 BESTANDEN -> in r36/integration GEMERGT (cb1d5f59; 3 Konflikte beide Seiten), Bau + Tests laufen. Naechster Schritt: Rest-Lauf 5 mit orchestrator/args_rest5_FR.json (F NB6-Fortsetzung, R Bau-Fortsetzung).
* 2026-10-11 13:35: O INTEGRIERT (gezielt 611/611 gruen) + MASKS-Paket-Gate (75a65020). Integriert 15/17. N: Abnahme 1 laeuft (nach Licht-Nachbesserung). F NB6 und R laufen.
* 2026-10-11 13:20: O pri Abnahme 0 BESTANDEN, in r36/integration GEMERGT (6e40fe1c; render_pc.c Konflikt mit E = beide Seiten), Bau + Tests laufen; NACHLAUF 38-42, Release-Notes PRI-Absatz. Offen: N (NB1 Licht), F (NB6), R (Bau).
* 2026-10-11 13:05: H INTEGRIERT (9a75d143, gezielt 606/606 gruen). Integriert jetzt 14/17: A B C D E G H I J K L M P Q. Offen: N (NB1 Licht), O (Abnahme 0), F (NB6), R (neu).
* 2026-10-11 12:55: Spur R dachkiste gestartet: Workflow wf_42c73e2c-d1d (args {welle:[R], slots 1, max_nach 2}), Baum r36_dachkiste (Zweig r36/dachkiste, Basis r36/integration 99398dc7). Skript r36-rest3.js kennt jetzt je Spur ein Feld basis (Diff/Log gegen den Integrationsstand). Nach R: mergen wie die anderen (Abschnitt 3), Basis ist schon der Integrationsstand.
* 2026-10-11 12:40: H ziel1190 Abnahme 0 BESTANDEN, in r36/integration GEMERGT (9a75d143; scd_vm.c von Hand: G-Notice + H-Zeile), Bau + Tests laufen. ⛔ NEUER BEFUND aus H (Dossier §2.4/OFFEN, NACHLAUF 37): die Dach-Kiste ROOM1090 obj 1 ist die einzige Stufe zum Tor nach ROOM1100; seit Spur A kann sie vom Dach fallen -> moeglicher Softlock. Ob das Original das Schieben bis zur Kante zulaesst, ist ungemessen -> Spur R (Baum r36_dachkiste aus r36/integration) misst Port + DuckStation (stage_saves/room1090_orig.sav): blockt das Original -> Port muss blocken; sonst Grenze. O pri Bau fertig, Abnahme 0 laeuft.
* 2026-10-11 12:20: G INTEGRIERT (gezielt 613/613 gruen). Offen: N Nachbesserung 1 (Licht), F NB6, H Bau, O Bau.
* 2026-10-11 12:05: G tueren in r36/integration GEMERGT (9361d9c3; Konflikte .gitattributes/game_step_common.c/audio_pc.c = beide Seiten behalten) + Paket-Gate KARTSE.VBS; Bau + gezielte Tests laufen. NACHLAUF 29-32 (G), Release-Notes-Entwurf Tueren-Absatz/Fragen/Grenze.
* 2026-10-11 11:50: G tueren Abnahme 0 BESTANDEN (Bericht 73a856f1, keine Maengel, Hinweise H1-H7) -> Merge in r36/integration + Paket-Gate KARTSE.VBS laufen. N kartenitems Abnahme 0: Mangel M1 = erfundene Licht-Konstanten der Karten-Props (statt Original-Weg FUN_8002c18c NCCT/Flach) -> Nachbesserung 1 im Workflow. F/H/O arbeiten.
* 2026-10-11 11:45: M karte INTEGRIERT (db644dac, gezielt 604/604 gruen); M-Hinweise in NACHLAUF.md 23-28, Karte-Absatz + 2 Fragen im Release-Notes-Entwurf.
* 2026-10-11 11:25 (neuer Account, Fable): M karte in r36/integration gemergt (konfliktfrei); Bau + gezielte Tests laufen.
  Rest-Lauf 4 gestartet: Workflow `wf_74d1d9dc-c24` mit `orchestrator/args_rest4_FGNHO.json` (F Nachbesserung 6,
  G/N Abnahme-Fortsetzung mit Teilbericht, H Bau-Fortsetzung, O Bau; 5 Plaetze, Modell = Sitzung/Fable).
  Monitor: `~/.claude/projects/c--workspace-git-reAi-v2/c41eae99-.../subagents/workflows/wf_74d1d9dc-c24/journal.jsonl`.
  Naechste Schritte unveraendert: Abschnitt 3 je bestandener Spur (G braucht Paket-Gate-Eintrag KARTSE.VBS + P0CD.DO2-Pruefung), dann Abschnitt 4.

## 0. Lage in Kuerze

17 Spuren A..Q (Liste in VERTRAG.md §0). Jede Spur: eigener Arbeitsbaum, Bau-Agent → unabhaengige
Abnahme → Nachbesserung. Bestandene Spuren werden in den **Integrationsbaum** `.claude/worktrees/r36_integration`
(Zweig `r36/integration`) gemergt, dort gebaut und gezielt getestet. Am Ende: volle Suite, Release v0.8.24.

**Integriert (11):** A klettern, B kleidung, C schuss, D granate, E gegnerzahl, I inventar, J szenenleon, K szene1150,
L elza, P story, Q schluessel. Integrationsbaum-Stand `r36/integration` e2ca28bc (+ dieses Dokument nach Merge von master),
gezielt 601/601 gruen (alle Unit-Tests + alle r36-Tests + betroffene Integrationstests), volle Suite NOCH NICHT gefahren.

**Offen (6):** M karte (BESTANDEN laut Bericht, nur noch mergen), G tueren, N kartenitems, H ziel1190, O pri, F zombieki.

## 1. Je offene Spur: Stand und naechster Schritt

| Spur | Baum / Zweig | Stand (Commit) | Naechster Schritt |
|---|---|---|---|
| **M karte** | r36_karte / r36/karte | Nachbesserung 1 2c5433a3; Abnahme 1 Bericht `M_abnahme_1.md` (863d01cc, Commit-Text sagt "TEILSTAND", der Bericht ist aber VOLLSTAENDIG: alle 6 Punkte erfuellt, "Maengel: Keine", **BESTANDEN**) | **Mergen** (Abschnitt 2), Hinweise H1..H7 aus dem Bericht in NACHLAUF.md uebernehmen |
| **G tueren** | r36_tueren / r36/tueren | Bau fertig (9d564d45 feat, 10754312 fix); Abnahme 0 TEILSTAND `G_abnahme_0.md` (414977b6): Punkt 1 (Fahrstuhl 1080 = RE2 DOOR27, Tor 2090 = DOOR26) **erfuellt**; Punkte 2 (Tuer-Textur 10C0↔10D0) und 3 (Karten-Tuer: Piep, dann Aufschliess-Ton) noch NICHT geprueft | Abnahme 0 fortsetzen (Modus `abnahme`, Hinweis auf den Teilbericht) |
| **N kartenitems** | r36_kartenitems / r36/kartenitems | Bau fertig (3c0737c0 feat); Abnahme 0 TEILSTAND `N_abnahme_0.md` (e3983f5c): alle 5 Wortlaut-Punkte **erfuellt**, Gates bis 3.2 geprueft; ein Mangel **M1 "Licht/Darstellung"** ist erwaehnt, Abschnitt 4 aber NICHT geschrieben | Abnahme 0 fortsetzen (M1 ausformulieren, Regressionsgruppen fahren) → bei Mangel Nachbesserung |
| **H ziel1190** | r36_ziel1190 / r36/ziel1190 | Bau UNFERTIG: 3ced5952 (Messung vorher + RE), ac70f38f (Scheiben-Texte hinten an den Scheiben, Knoepfe wieder Original), f833268f (UNGEPRUEFTE Sonde probe_r36_ziel1190_karte.c + probes-Eintrag — erst bauen!). Dossier `H_ziel1190.md`: Punkt 1 Messung nachher + Pins offen, Punkt 2 (Zweck der Kiste ROOM1090) offen | Bau fortsetzen (Modus Bau mit `stand.H`); Hinweis: Spur A hat die Kiste ROOM1090/1091 obj 1 als fallfaehig erkannt (Band 5 → 3) — fuer Punkt 2 nutzen |
| **O pri** | r36_pri / r36/pri | NICHT begonnen (0 Commits, Basis 78a4a748) | Bau von vorn. ROOM1100-Marker-Datei des Nutzers wurde NIE gefunden → selbst messen + den Nutzer in der Abschlussmeldung fragen |
| **F zombieki** | r36_zombieki / r36/zombieki | P1 (passive Zombies) + P3 (Gorilla-Blutlache) **erfuellt** seit Abnahme 2; P2 (Affen blockiert, "irgendwie verbessern") **teilweise** nach Abnahme 5 (8030e41f): fest 25936 (v0.8.23) → 7500 Bilder, FERN 19812 → 300. Danach UNGEPRUEFTER Code der Nachbesserung 6: 5a02b824 (Nah-Umweg ueber 0x10-Pads, verkeilt springen, Nachruecken), d9c0d09e, cedb3849, 94d762de (probes-Eintrag, ungebaut) | Nachbesserung 6 fertigstellen + Abnahme 6 (Maengel = `F_abnahme_5.md`). Besteht P2 wieder nicht: **trotzdem integrieren** (Orchestrator-Entscheidung, P2 ist PORT-WAHL "verbessern", an keiner gemessenen Stelle schlechter als v0.8.23) — dann den Stand des letzten Abnahme-Berichts mergen und den Rest als Grenze in die Release Notes |

## 2. Workflow fuer die offenen Spuren starten

Werkzeuge liegen versioniert unter `analysis/befunde_runde36/orchestrator/`:
* `r36-rest3.js` — das Workflow-Skript (LF-Zeilenenden PFLICHT; CRLF wird abgelehnt). Modi je Spur ueber `args`:
  ohne Modus = Bau (mit `stand.X` = Fortsetzung), `modus.X = "abnahme"` (+ `bau.X` Zusammenfassung),
  `modus.X = "nachbesserung"` (+ `abn.X` = {bestanden:false, urteile:[...], maengel:[...]}, `runde0.X` = Nummer der
  letzten Abnahme). `slots` = parallele Agenten (4 bewaehrt), `max_nach` = Nachbesserungen je Spur (2), `model`.
* `args_beispiel_nachbesserung_F.json` — vollstaendiges args-Beispiel (Modus nachbesserung).
* `mk_args36c_beispiel.py` — wie man abn/bau/stand aus Journal und Berichten baut.
* `mk_wt36.sh` — schlanker Arbeitsbaum (sparse) fuer eine neue Spur.
* `konflikt_beide.py` — loest "beide fuegen an derselben Stelle ein"-Konflikte (behaelt HEAD, dann den Zweig).

Aufruf (Beispiel, Basis-Konstante im Skript `BASIS = '78a4a748'` passt fuer alle r36-Baeume):
```
Workflow({scriptPath: "C:/workspace/git/reAi_v2/analysis/befunde_runde36/orchestrator/r36-rest3.js",
  args: {welle: ["G","N","H","O","F"], slots: 4, max_nach: 2, model: "opus",
         modus: {G: "abnahme", N: "abnahme", F: "nachbesserung"},
         bau: {G: {...}, N: {...}, F: {...}}, abn: {F: <aus F_abnahme_5.md>}, runde0: {F: 5},
         stand: {H: "<Stand aus Abschnitt 1>", F: "<NB6 ungeprueft, Commits ...>"}}})
```
Hinweis fuer `modus: "abnahme"` mit Teilbericht: der Abnahme-Prompt kennt keinen Teilbericht — in `bau.X.zusammenfassung`
ausdruecklich schreiben "Teilbericht <datei> einer abgebrochenen Abnahme liegt vor (Punkt 1 erfuellt ...); belegte
Messungen uebernehmen, fehlende Punkte selbst messen".
Monitor: `journal.jsonl` unter `~/.claude/projects/c--workspace-git-reAi-v2/<session>/subagents/workflows/<runId>/` mit
`tail -F | awk ... fflush()` (kein `cut`/`grep` ohne line-buffered).

**Sofort nach jedem Abbruch (Limit/Guthaben):** je Baum `git status --porcelain` → Reste als
`wip(...)/doc(...): TEILSTAND/UNGEPRUEFT` committen; verwaiste Prozesse per Wurzel-PID beenden (`taskkill /PID <bash> /T /F`,
nie `/IM re15_pc.exe`), auch tail/awk-Reste alter Monitore. Haengende Workflow-Agenten (Transkript > 30 min still, kein
Prozess im Baum) lassen sich per `SendMessage({to: "<agentId>"})` wecken — das Ergebnis geht dann an die Hauptsitzung,
NICHT mehr an den Workflow (dieser bleibt haengen → nach dem Rest per TaskStop beenden).

## 3. Integration jeder bestandenen Spur

Im Baum `C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration`:
1. `git merge --no-ff -m "merge(r36): Spur X <thema> (Abnahme N bestanden; ...)" r36/<thema>`
2. Konflikte: fast immer Include-/Installer-/Ereignis-Zeilen, die beide Seiten an derselben Stelle einfuegen →
   `python -X utf8 -I analysis/befunde_runde36/orchestrator/konflikt_beide.py <dateien>`; DANACH jede Datei ansehen.
   ⛔ Endet ein Konfliktblock mitten in einer Funktion (Fall audio_pc.c, Spur J gegen L), die fehlende `}` zwischen den
   Haelften selbst setzen (sonst offene Funktion).
3. `bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build`
4. `PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build -R "^(unit_|probe_|prop_)|r36_|<betroffene Raeume/Module>" -E "android_bash" --timeout 900 -j 6`
5. Rot? Erst einzeln nachfahren (Fenster-Haken flattern unter Last). Echte Kollision zwischen Spuren → nach Eigentuemer
   (VERTRAG §1.4) aufloesen, mit Begruendung committen. Bisher: Irons-Kopf J/L (35c02fab), synchro-Zensus (99b397e7).
6. Paket-Gate: neue Dateien unter `re15_port/shared_assets/RE2/` der Spur in `release/make_package.sh` (Block "Seit Runde 36")
   eintragen (existiert fuer ELZASE.VBS, TUERSE.VBS, SCHLOSS1010.*; G/N/O/H/M pruefen: Dossier "Fuer den Nutzer" → neue Assets).
7. Hinweise/Funde des Abnahmeberichts in `analysis/befunde_runde36/NACHLAUF.md` eintragen, committen.

## 4. Release v0.8.24 (wenn alle Spuren integriert sind — oder die Reste bewusst als Grenze stehen)

1. master in den Integrationszweig mergen (`git -C <integration> merge master`; master traegt Doku/Sprachdateien:
   836023f5, 565da62b, dieses Dokument).
2. **Volle Suite einmal**, ohne parallele Agenten: `bash re15_port/tools/local_build.sh all` im Integrationsbaum.
   Rote Fenster-Haken einzeln nachfahren. Dann `RE15_MIN_TESTS` in `re15_port/tools/local_build.sh` auf die neue Zahl
   (mit Verlaufskommentar, Muster der Runde 35: 517 → 592) — Integrationsstand hat schon > 601 Tests.
3. Release Notes: `orchestrator/RELEASE_NOTES_v0.8.24_ENTWURF.md` oben in `release/RELEASE_NOTES.md` einfuegen;
   Platzhalter fuellen: `@@SUITE@@` (Suite alt → neu), `@@WEITERE@@` (M/G/N/H/O/F aus den Dossiers "Fuer den Nutzer"),
   `@@ENTSCHEIDEN@@`, `@@GRENZEN@@` (aus NACHLAUF.md: Hund-Claim RE2, Fress-Tropfen, Feuer-Versatz, liegender
   Spawn-Zombie, Roy sackt in 1021 nicht zusammen, Spinne 2060, Affen-Rest falls F nicht bestanden, PSX-RAM E-H5).
4. master vorziehen: `git -C C:/workspace/git/reAi_v2 merge --ff-only r36/integration`.
5. **Hauptbaum-exe neu bauen** (Nutzer testet `re15_port/build/platform/pc/re15_pc.exe` im Hauptbaum):
   `cd C:/workspace/git/reAi_v2 && bash re15_port/tools/local_build.sh build`.
6. Release-Bauten (je Exit-Code pruefen, keine Pipe, die Fehler schluckt): `bash release/build_win_cross.sh`,
   `bash release/build_linux_deck.sh`, `bash release/build_android.sh --version v0.8.24`.
7. Paket: `PATH="$PATH:/c/msys64/usr/bin" bash release/make_package.sh --version v0.8.24` (baut NICHT selbst; Gate prueft
   Assets inkl. synchro-WAVs — die Elza-Sprachdateien sind seit 565da62b versioniert).
8. Archiv: Pakete + APK nach `C:\workspace\Re15Data\re15_packages_archiv\v0.8.24`, `sha256sum -c`, Ordner v0.8.23 entfernen.
9. Release-Commit (`git commit -F <datei>`, Backticks!), `git tag v0.8.24`, `git push origin master v0.8.24`.
10. Arbeitsbaeume entfernen (`git worktree remove .claude/worktrees/r36_<x>` je Baum, VORHER pruefen, dass alles gemergt
    und nichts Unversioniertes Wichtiges drin liegt); Plattenplatz ~5 GB je Baum.
11. Memory: `reai-v2-runde36-*.md` + Zeile in MEMORY.md; die ⛔-Zeile zu dieser Uebergabe in `CLAUDE.md` entfernen.
12. Abschlussmeldung an den Nutzer (deutsch) mit Abschnitt 5.

## 5. Fragen an den Nutzer (in die Abschlussmeldung)

* ROOM1011 main16.5 (Roys Stoehnen): Untertitel steht auf "Roy: groans" — welcher Wortlaut?
* Freigabe der 11 umgeschriebenen Textzeilen (Tabelle im Release-Notes-Entwurf) + der zwei neuen Spuren-Texte;
  ROOM50B1 "It's Ada." so lassen? 6000 msg 0/3 auch Original lassen?
* Tuerknall: ROOM106 (Schlag gegen die Tuer) gewaehlt; Alternative ROOM205 (Holz splittert)?
* Texte an der Parkplatz-Tuer / Schluessel-Texte (eigene Formulierungen in RE2-Form) ok?
* ROOM1100 "Leon von halbtransparentem Boden ueberdeckt": die Marker-Datei wurde nie gefunden — wo liegt sie?
* Spur M hat den Raum "1090 mit Hunde-Luke und Weste" als ROOM1190 belegt — war 1190 gemeint?
* Marvins Schliess-Geste an der Werkstatt-Tuer ist Clip 20 (linke Hand) — gefaellt sie?
* Sprachdateien zum Aufnehmen: synchro/STAGE1/room11b0/main20.wav + main21.wav (Marvin), room1210/main20.wav,
  room1091/main20.wav.

## 6. Lehren dieser Runde (fuer die naechste Sitzung)

* Limits: Fable-Sitzungslimit, Fable-Guthaben, Opus-Sitzungslimit (5-h-Fenster, ~3 Mio Agenten-Token) und am Ende das
  Opus-WOCHENlimit. 4 parallele Agenten sind der beste Kompromiss; jeder Agent committet fortlaufend.
* Keine vollen Suiten in Spurbaeumen (5 parallel = 81 min + Last-Timeouts); gezielt `ctest -R`, Suite einmal integriert.
* Abnahmen sind streng und finden echte Luecken (Kisten-Fall FUN_8002c444, Kleidung am Hundeweg, Tuerbild 11B0) — nicht
  ueberspringen. Aber: Endlos-Verbesserungsschleifen (F/P2) nach 2-3 Runden per Orchestrator-Entscheidung beenden.
* In einer RDP-Sitzung braucht jede exe `SDL_ASSERT=always_ignore` (WIN_AddDisplay-Assertion), sonst haengt sie.
