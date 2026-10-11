# ⛔ RUNDE 36 — UEBERGABE (Stand 2026-10-11 09:40, Wochenlimit Opus erreicht, Reset 17.10. 08:00)

Dieses Dokument ist fuer den NAECHSTEN Account/die naechste Sitzung geschrieben, ohne Kontext der alten.
Alles liegt LOKAL auf dieser Maschine (Zweige `r36/*`, Arbeitsbaeume unter `C:/workspace/git/reAi_v2/.claude/worktrees/r36_*`).
Nichts davon ist gepusht. Arbeite die Abschnitte 2 → 3 → 4 der Reihe nach ab; hake hier ab und committe dieses Dokument
nach jedem Schritt (`git -C C:/workspace/git/reAi_v2 commit`), damit ein weiterer Abbruch nichts verliert.

Pflichtlektuere vorher: `CLAUDE.md` (STOP-GATE, Build ueber `local_build.sh`), `analysis/befunde_runde36/AUFTRAG.md`
(Wortlaut des Nutzers + Nachtraege 14:00 und abends), `analysis/befunde_runde36/VERTRAG.md` (Bits/IDs/Ereignisse/Dateien je
Spur), auf dem Integrationszweig `analysis/befunde_runde36/NACHLAUF.md` (Funde ausserhalb der Spur-Wortlaute).

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
