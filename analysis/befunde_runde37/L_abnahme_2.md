# Runde 37 Spur L "re2zombie" — Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Baum `.claude/worktrees/r37_re2zombie`, Zweig `r37/re2zombie`, gepruefter Stand `e4bca518` (Code; HEAD `a17a649a` = nur der
Teilbericht; Basis `e399ee3f`, Merge `44721dca` = master v0.8.25 + Runde-38-Auftrag). Abnahme 2026-10-16 in zwei Teilen:
Teil 1 (Abschnitte 0-5, bis zum Sitzungslimit ~18:18, als Teilbericht `a17a649a` gesichert) und Teil 2 (Abschnitt 6, ab
21:05: Neubau, Reproduktion, Nach-Disassemblierung, Integrationslaeufe, Urteil). Alles unten ist SELBST gemessen bzw. SELBST
disassembliert; aus Abnahme 0/1 und dem Dossier ist nichts ungeprueft uebernommen (wo ich mich darauf beziehe, steht es dabei).
Mess-Skripte und Rohdaten (Scratch der Sitzung, nicht eingecheckt): Treiber `abn2_lauf.sh`, Batches `batch1.sh` /
`batch2_messer.sh`, Auswerter `abn2_state.py` (Zustandswechsel/HP/Griffe je Raumbesuch) und `abn2_messer.py`
(Messertreffer je Anmarsch, P3-Ausgaenge), Laufordner `abn2L/runs/<lauf>`, `abn2L/runs_messer/<lauf>` und (Teil 2)
`abn2L/teil2/`.

## Ergebnis

| # | Punkt (Wortlaut, AUFTRAG.md) | Urteil | Kern-Beleg (selbst gemessen) |
|---|---|---|---|
| 1 | "die Zombies sind immer noch zu passiv ... nach Irons tod ... bleiben einfach nur Stumpf alle im Gang stehen ... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang" | **erfuellt** | echte Tuer 1150 -> 1130 nach der Montage: alle fuenf Zombies gehen ab Bild 6 auf Leon zu (19-39 Tsd. Einheiten Weg in 1100 Bildern); Arme hoch (1/2) bei 1991 / 1936 / 1978 / 4692 (Timer) / 2346 (Leon rennt); Griff aus 0x201 bei 1108 / 1193 / 1180, Griff aus dem Ausfall bei 1280; Biss: Leon-HP 100 -> 70 -> 40 bzw. -> -20. Framedump F856-F904. Basis-exe: kein 1/2 |
| 2 | "das soll nicht nur da gelten, das darf generell für die Zombies gelten" | **erfuellt** | Normalspiel ohne Montage: Leon 1180, **Elza** 1181 (PL04), Leon 1010 — Arme hoch bei 1995 / 1991 / 1978, Griffe aus 0x201, Bisse; Basis nie 1/2. Gemeinsamer Gang-Executor EXEC[1] aller RE2-Zombie-Typen |
| 3 | "schaue einfach nach wie Resident Evil 2 das macht" | **erfuellt** | Code = Bytes (Abschnitt 3, inkl. Nachbesserung 2); RE2-Original-Rohdaten selbst ausgezaehlt: 1/1 -> 1/2 bei 1999 / 1987 / 3565, Griff aus 1/2 bei 1180 / 1152 — Port 1991 / 1936 / 1978 / 1995, 4692, Griff 1108 / 1193 / 1180 |
| 4 | Orchestrator-Auftrag L (a)(b)(c) + Zensus; exe: Arme hoch, auf Leon zu, greifen, beissen; Nahkampf nicht schlechter | **erfuellt** | (a)(b)(c), D12 und die Erholungsleiter P3 Zeile fuer Zeile gegen EMZ0.BIN; an der exe alle drei Produzenten und D12; Erholung 0x10101 statt 0x101 (F1211 Schritt (-4,+13), Bild 14 -> 15); Nahkampf an der exe 18 Paare: alle Messertreffer 41 = 41, je Anmarsch 7 / 31 gegen 4 / 33, Griffe 36 / 35 |
| N2 | Nachbesserung 2 (Abnahme-1-M1 Code + Text, M2 Text) | **erfuellt** | Leiter-Ende/Riegel/0x800152C8/+0x148/Gang-Timer = Bytes; Harness-, Riegel- und exe-Zahlen des Dossiers reproduziert (Abschnitt 4); "weniger Schlaege" aus den Nutzer-Aussagen entfernt, Wegstrecke 35,6 / 25,6 mit der genannten Takt-Regel reproduzierbar |

Gates: **RE-Gate haelt** (Teil 1: Stichproben weit ueber 3 Adressen, alle stimmen, Rohbytes geprueft; Teil 2: DECISION[12]
inkl. der gemeinsamen Ausstiegsstelle 0x801046A0, EXEC[1]-Produzenten 0x80101D60-F78, Treffer-Handler P3 + 0x800152C8 erneut
selbst disassembliert, Abschnitt 6.3; keine Guess-Tells, keine Env-Schalter im Engine-Diff). **Vertrag haelt** (Pfade:
16 Dateien `analysis/befunde_runde37/L_*` + 12 unter re15_port, keine release/, platform/android/, shared_assets/,
CMakeLists; keine Ressourcen; Haken in gemeinsamen Dateien 2 + 3 Zeilen mit "Runde 37 Spur L"; Treffer-Handler nur case 3 =
Freigabe Runde 38, s. Hinweis H1). **Tests**: Spur 10/10 verbose (Teil 1 und Teil 2), Unit-Auswahl 306/306 (Teil 1, dieselben
Binaerdateien), `unit_r36_zombieki_eintritt` 1/1 verbose (Teil 2), Integration **18/18 seriell** (Teil 2, Abschnitt 6.5).
Reproduzierbarkeit: zwei Teil-1-Laeufe mit derselben exe neu gefahren, `state.log` bitgleich (Abschnitt 6.2).

**Urteil: BESTANDEN** — alle vier Nutzer-Punkte und die Nachbesserung 2 sind erfuellt, RE-Gate und Vertrag halten, alle
gezielten Tests gruen. Keine Maengel. Die Hinweise H1-H6 sind fuer die Integration bzw. den Orchestrator (H2: veraltete
Messzahl im Code-Kommentar beim Merge nachziehen; H3/H6: offene RE2-Abweichung im Treffer-Handler und Navigator-Tasche
ROOM1030 als eigene Punkte einplanen).

## Hinweise (kein Mangel dieser Spur)

* **H1 — Umfang der Freigabe.** AUFTRAG Runde 38 gibt woertlich "die Korrektur der vorbestehenden Inversion der
  RE2-Erholungsleiter (re2z_hit_main case 3, RE2 `bne` @0x80105b3c)" frei. Nachbesserung 2 korrigiert in derselben Phase
  zusaetzlich den Ein-Angreifer-Riegel @0x80105AA0-B0 (Abnahme 1 H1/H3 hatte empfohlen, beides als EINEN Punkt zu behandeln)
  und vervollstaendigt den 0x10101-Zweig (0x800152C8-Schritt @0x80105B98, Gang-Zeilen-Timer +0x158 = 30) — alles in case 3,
  byte-belegt, geriegelt (`unit_r37_re2zombie_p3` Teile A-D). Bitte bei der Integration als mitfreigegeben bestaetigen.
* **H2 — veraltete Messzahl im Code-Kommentar.** `enemy_ai_re2_zombie.c` Z.1756 (Kopf des Produzenten-Blocks): "Heute: Messer
  368 -> 373, Pistole 1795 -> 1775." Seit Nachbesserung 2 gilt (selbst gemessen): `unit_re2_zombie_abc` Messer 361 /
  Pistole 1826, Sonde `mess` Maske 0 / 0x7F: Messer 368 / 362, Pistole (waffe 3) 1795 / 1826. Beim Merge die Zahlen nachziehen
  oder streichen (reiner Kommentar, kein Verhalten).
* **H3 — verbleibende RE2-Abweichung im Nahkampf (OFFEN, nicht freigegeben, mit Adresse im Dossier):** der P2-Schwanz des
  Treffer-Handlers greift im Original WAEHREND der Trefferreaktion zu (selbst gelesen: `80105980 sltiu v0,s3,0x514`,
  `8010598c-a0 lbu 0x800cfdcb / andi 0x80 / bne`, `801059a8-b8` Etage, `801059c0-d8 lw 0x800cfe18 / lw 0(v0) / andi 1 / bne`,
  zwei Sektoren `jal 0x80015758` a3 = 288 @0x80105A00 / @0x80105A38, `addiu v0,zero,769` / `sw` @0x80105A0C-10 / @0x80105A44-4C).
  Im Port fehlt das; der Nahkampf ist dadurch leichter als in RE2. Der Nutzertext ("Fuer den Nutzer") nennt das nicht —
  fuer die Release-Notes erwaehnenswert, weil es genau die "Bedrohung" im Nahkampf betrifft. Ebenso OFFEN (Beschreibung
  stimmt, selbst gelesen): P0 faellt nach P1 durch (@0x8010571C-3C), P1 halbiert +0x144/+0x148 (@0x80105770-94).
* **H4 — Werkzeug-Befund bestaetigt (fuer alle Spuren):** `local_build.sh` Z.326 beendet `re15_pc`-Prozesse per
  `$_.Path.StartsWith('$_bw', ...)` ohne Pfadtrenner; `integration_r34_granaten` startet `${RE15_PC_EXE}` direkt (kein
  Kopie-Name) — ein Bau in `re15_port/build` trifft also auch `re15_port/build_<x>/.../re15_pc.exe`. Vorschlag des Dossiers
  (`'$_bw\'`) passt.
* **H5 — Stage 2 nicht aussagekraeftig gemessen:** Versuch ROOM2020 (vier Zombies Typ 0x16) mit Kartenwerkzeug: der Spieler
  landet unabhaengig von der Karten-z auf z = 3768 (Raum-Spawn laut debug.log `(-14200,-1800,2450)`, also anderes y); dort
  erreichen die Zombies Leon in BEIDEN exe nicht (dmin 2374..4990, danach Wege weg von Leon). In der Spur-L-exe feuert der
  Timer-Produzent (1) trotzdem (1/1 -> 1/2 bei 4857 / 4990 / 4096 < 0x1388), Basis nie. Wegen der unklaren Spielerlage
  nicht als Beleg gewertet; falls der Nutzer in Stage 2 "passive" Zombies meldet, zuerst Erreichbarkeit/Navigator
  (Dossier OFFEN "Navigator-Tasche") pruefen — Spur L aendert daran nichts (L und Basis gleich bis auf die 1/2-Wechsel).
* **H6 — Navigator-Tasche ROOM1030 bleibt (vorbestehend, Teil 2 selbst gemessen; Vorschlag eigene Spur).**
  `ctest -R "^unit_r36_zombieki_eintritt$" -V` (gruen) druckt: `1030<-1050 cut2 Leon(1172,-12558) hin slot1 typ16 grid0D ...
  still 330 ab F780 @(-6175,-24838) d 2535 | Ende st1/1 (-6144,-24838) ... KLEMME` und `[vorbestehend] Stand bis Runde 37:
  600 Bilder aufrecht still (Gegenlauf Maske 0)` -> Summe `PASSIV 0, KLEMME 0 (vorbestehend 1)`. Spur L hat diesen Riegel der
  Runde 36 so angepasst, dass ein Stillstand, den es im Stand bis Runde 37 ebenso gibt, nicht als KLEMME zaehlt (Begruendung
  im Diff: die 0x201-Wechsel machen den gehenden Zombie erst zu einem "reagierten" Fall). Die Anpassung ist nachvollziehbar
  (330 = Zaehlung NACH der Reaktion im Spur-L-Lauf, 600 = Zaehlung OHNE Reaktions-Bedingung im Gegenlauf Maske 0 — zwei
  verschiedene Zaehlungen des Riegels, kein Vorher/Nachher-Mass), aber damit steht im Normalspiel weiterhin ein Zombie
  (Typ 0x16) mindestens 330 Bilder (~11 s) an der Tresen-/Wandzelle, 2535 vor Leon, bis zum Laufende — genau die
  Fehlerklasse des Nutzers ("bleiben ... stehen"). Ursache
  laut Dossier OFFEN: Steuerziel des RE1.5-Navigators (re15_nav_update_steer = FUN_80039e7c-Zwilling) liegt in einer soliden
  Zelle; nicht im Code der Spur L (VERTRAG 1.4: L aendert DECISION[0]/EXEC[0..2]). Fuer den Orchestrator: eigene Spur
  "Navigator ROOM1030" einplanen (Dossier-Vorschlag: Zonen/Verbindungspunkte ROOM1030.RDT gegen FUN_80039e7c).

## 0. Bau und Grundlage

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`. `re15_pc.exe` 2026-10-16 16:57:25 juenger als alle geaenderten Quellen
  (`enemy_ai_re2_zombie.c` 16:43:25, `enemy_ai_common.c` 16:30:21, `re15_actor.h` 16:30:13); `git status` sauber.
* Code-Diff `git diff master...HEAD -- re15_port/engine re15_port/include` (569 Zeilen) selbst gelesen:
  `enemy_ai_re2_zombie.c` (Produzenten (1)(2)(3), (b), (c), DECISION[12], Sonden-Maske/Zaehler, Kommentar-Zensus; NEU in
  Nachbesserung 2: `re2z_hit_main` case 3 Riegel + Leiter-Ende + Helfer `r2z_r37_152c8`), `enemy_ai_common.c` (+2 Zeilen:
  +0x148 ablegen), `re15_actor.h` (+1 Feld `re2z_root148`), NEU `re15_re2zombie_r37.h`. `git diff --stat 44721dca HEAD`
  (= Nachbesserung 2) im Code: nur diese vier Dateien + `test_r37_re2zombie.c` (`pin_p3`, Zaehler) + `probes/r37_re2zombie.cmake`.
* `git diff master --stat` zeigt Loeschungen `analysis/befunde_runde38/orchestrator/*.js` — das ist nur der seit dem Merge
  weitergelaufene master; im Drei-Punkt-Diff `master...HEAD` nicht vorhanden.
* Basis-exe fuer Vorher/Nachher: Hauptbaum `re15_port/build/platform/pc/re15_pc.exe` (2026-10-14 07:19:50, nach dem
  letzten re15_port-Commit 1c295bd1 07:18:36; `git diff e2cc7673 master -- re15_port` leer, `git diff master HEAD --
  re15_port` = genau die Spur-L-Dateien) = Stand OHNE Spur L. Nur gelesen und als Messkopie im exe-Ordner der Spur L
  gefahren (gleiche Assets). Alle Basis-Werte unten sind bitgleich zur Basis-Spalte der Abnahme 1.

## 1. Punkte 1 und 2 an der exe (eigener Treiber, Messkopie `re15_pc_abn2L_<lauf>.exe`, eigener Auswerter)

Treiber `abn2_lauf.sh`: Karte per `probe_r36_gegnerzahl_karte` (Montage `1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15
7:0xd3-0xd7 7:0xc6-0xca`) bzw. `probe_r36_elza_karte`, `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_STATE_LOG=state.log
RE15_EXIT_AT=<bild>#<raum>`. Weg in den Gang 1130 = ECHTE Tuer 1150 -> 1130 (`W1,A0.2,W2`; debug.log `[save] CONTINUE:
resumed in room 1150 (hp=100)`, `[irons-tod] ROOM1130 Zustand 0: Nachspawn 1130 (Programm 10)`, `[flow] EXIT_AT: Bild 1100
in Raum 1130 erreicht -> exit`). L = exe dieses Baums, B = Basis-exe ohne Spur L (Abschnitt 0). Laeufe mit Tod-/Continue-
Schleife (1010, 1130 rennend) nach dem ersten Besuch per eigener PID beendet (nur `re15_pc_abn2L_*`, nie /IM).

### 1.1 Punkt 1 — Nutzer-Beispiel: Gang 1130 nach Irons' Tod (Montage-Zombies), echte Tuer

| Lauf | Ergebnis (state.log, letzter Raumbesuch, `abn2_state.py`) |
|---|---|
| L1 Leon steht 1100 Bilder | alle 5 Montage-Zombies gehen ab Bild 6 auf Leon zu (Wege 19445 / 26755 / 19185 / 30611 / 38931 Einheiten, Platz 1-5 Typ 0x16/0x10/0x10/0x11/0x11); Platz 5 **F869 1/1 -> 1/2 bei +0x1F0 1991** (Arme hoch), **F888 1/2 -> 1/3 bei 1108** (Griff aus dem Arme-hoch-Gang); Leon gr 0 -> 1 F888; **Leon-HP 100 -> 70 (F909) -> 40 (F947)** (Biss) |
| B1 Basis, gleich | alle 5 gehen ebenfalls auf Leon zu (das ist Spur H); KEIN 1/2; Platz 5 F896 1/1 -> 1/3 bei 1166 (Griff aus dem langsamen Gang); HP -> 70 (F917) -> 40 (F955) |
| L2 Leon geht den Gang hinunter (`...,R0.33,U25`) | Platz 5 F458 1/2 bei 1936 -> F474 Griff aus 0x201 (1193); Platz 3 F707 1/2 bei **4692** (Timer-Produzent (1), < 0x1388); Platz 4 F772 1/2 bei 1978 -> F793 Griff (1180); Platz 2 F818 1/2 bei 4283; Leon-HP 100 -> 70 (F495) -> 40 -> 10 -> **-20 (F852)** |
| L3 Leon RENNT (`...,R0.33,UX25`), erster 1130-Besuch | Platz 5 F343 / Platz 4 F346 / Platz 1 F474 je 1/2 bei **2346** (Leon rennt, < 0x9C4 = Produzent (3)); Platz 3 F461 1/2 bei 2442 -> F471 Ausfall 1/12 -> **F481 1/12 -> 1/3 bei 1280** (Griff aus dem Ausfall = DECISION[12]); Platz 4 F352 Griff aus 0x201 (1180); Leon-HP -> -20 (F610) |

Bild (eigener Framedump, `RE15_FRAMEDUMP=840-960/8:fd_`, `RE15_WINDOW_SCALE=1`, Lauf L1b = L1 bis Bild 960, Ereignisse
bitgleich zu L1): `abn2L/runs/L1b_1130_bild/abn2_1130_F856-904.png` (Scratch) — Chief-Office-Tuer, F856 / F864 der fette
Zombie kommt im Gang heran, F872 / F880 im Arme-hoch-Gang mit nach vorn gestreckten Armen direkt vor Leon, F888 Griff,
F904 Biss.

Urteil Punkt 1: **erfuellt** — die Montage-Zombies stehen nicht im Gang, sie gehen auf Leon zu, heben vor ihm die Arme
(RE2-Arme-hoch-Gang), greifen und beissen; an der Basis-exe nie 1/2. Alle Ereignisbilder sind bitgleich zu Abnahme 1
(L1/L2/L3 dort) — Nachbesserung 2 aendert diesen Ablauf nicht (kein Treffer im Lauf, also kein Treffer-Handler P3).

### 1.2 Punkt 2 — "generell": Normalspiel ohne Montage, Leon UND Elza, andere Raeume

| Lauf | Ergebnis |
|---|---|
| L4 Leon ROOM1180 (Karte `1180 -4729 -16018 0`, debug.log `Spieler-Familie PL00`), steht 900 Bilder | Platz 3 (Typ 0x10) **F283 1/2 bei 1995** -> F311 Griff aus 0x201 (1193); **F802 1/2 bei 1991** -> F836 Griff (1180); Leon-HP 100 -> 80 (F332) -> 60 -> 40 -> 20 (F895) |
| B4 Basis, gleich | KEIN 1/2; Griffe aus 1/1 (F333 bei 1187, F693 bei 1173) |
| L5 **Elza** ROOM1181 (`probe_r36_elza_karte 1181 -4729 -16018 0`, debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `CONTINUE: resumed in room 1181`) | wie L4: 1/2 bei 1995 / 1991, Griffe aus 0x201 bei 1193 / 1180; Elza-HP 100 -> 20 |
| B5 Basis Elza, gleich | KEIN 1/2; Griffe aus 1/1 bei 1187 / 1173 |
| L6 Leon ROOM1010 (Karte `1010 3650 6900 2048`), erster Besuch | Platz 2 **F72 1/2 bei 1978** -> F93 Griff aus 0x201 (1193); Platz 1 **F83 1/2 bei 1995**, F286 1/2 bei 849 -> F287 Griff aus 0x201; F561 1/2 bei 1995; Leon-HP 100 -> 80 (F114) -> ... -> -20 (F626) |
| B6 Basis, gleich | KEIN 1/2; Griffe aus 1/1 (F101 bei 1173, F209 bei 849, F494 bei 1193); HP -> -20 (F553) |

Urteil Punkt 2: **erfuellt** — in jedem gemessenen Raum (1130 Montage, 1180, 1181 Elza, 1010) und jeder Spieler-Haltung
(steht / geht / rennt) heben gehende RE2-Zombies (Typen 0x10, 0x11, 0x16) vor dem Spieler die Arme und greifen aus diesem
Gang; an der Basis-exe nie. Der Mechanismus sitzt im gemeinsamen Gang-Executor EXEC[1] (`re2z_exec_walk`), den jeder
RE2-Zombie (`re15_re2z_owns_type`: 0x10/0x11/0x12/0x13/0x16/0x18) durchlaeuft. Werte bitgleich zu Abnahme 1 (L4/L5/L6).
(Stage-2-Versuch ROOM2020: Hinweis H5.)

## 2. Punkt 3 — "schaue einfach nach wie Resident Evil 2 das macht"

* Code = RE2-Bytes: alle Stellen des Diffs selbst gegen EMZ0.BIN / RE2-PSX.EXE gelesen (Abschnitt 3), einschliesslich der
  Nachbesserung 2 (Leiter-Ende P3, Riegel, 0x800152C8, +0x148, Gang-Zeilen-Timer).
* RE2-Original-Verhalten selbst nachgezaehlt aus den Rohdaten des Bau-Agenten
  (`L_belege/re2_original_room20a0_jeder_takt.txt.gz`, DuckStation-GDB, ROOM20A0, 6 Zombies x 900 Takte; eigener
  Zaehler, nicht `re2_0x201_ausw.py`): Wechsel 1/1 -> 1/2 genau 3x — `8014777c` Takt 421 bei +0x1F0 **1999**,
  `80156d64` Takt 773 bei **1987**, `8016634c` Takt 696 bei **3565** (Wort `00010101 -> 00000201`); Griff aus 1/2 2x bei
  **1180** / **1152** (`00010201 -> 00010301`), 1x zurueck 1/2 -> 1/1 bei 1792. Alle Wechsel: 1/1->1/2 3, 1/2->1/3 2,
  1/3->1/5 2, 1/5->1/1 3, 1/1->1/9 5, 1/9->1/5 5, 1/2->1/1 1 — deckt sich mit dem Dossier (M2).
  Weg je Takt (eigene Zaehlung ueber ALLE Takte gleicher Phase): 0x101 25,5 / 0x201 34,5; mit der Takt-Auswahl des
  Bau-Agenten (ss2 != 0 in beiden Proben, `re2_0x201_ausw.py` selbst gefahren) 25,6 / 35,6 (|dx|+|dz| 32,2 / 44,1) —
  die Dossier-Zahlen sind mit der genannten Regel reproduzierbar.
* Port an der exe (Abschnitt 1): Arme hoch bei 1991 / 1936 / 1978 / 1995 (Produzent (2), RE2 1999 / 1987), 4692 / 4283
  (Produzent (1), RE2 3565), 2346 (Produzent (3), Leon rennt); Griff aus 0x201 bei 1108 / 1193 / 1180 (RE2 1180 / 1152;
  Schwelle beider Gaenge `sltiu 0x4b0` = 1200).

Urteil Punkt 3: **erfuellt**.

## 3. RE-Gate — selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, RE2 `info/re2leon/COMMON/BIN/EMZ0.BIN` @0x80100000 bzw. `info/re2leon/PSX.EXE`; EMZ0.BIN = EMOVL10_S0.BIN = EMZ0_d1.BIN, md5 7f7a39e651064e07c95830db1bebdb89)

| Stelle (Diff) | selbst gelesen | Urteil |
|---|---|---|
| **N2: Ein-Angreifer-Riegel P3 @0x80105AA0-B0** | `80105a7c jal 0x80015614` (a3 = 512) / `80105a84 bne v0,zero,0x80105ad8` / `80105a94 sltiu v0,v0,0xfa0` / `80105aa0 lui v0,0x800d` / `80105aa4 lbu v0,-565(v0)` (= 0x800CFDCB = Spieler 0x800CFBF8 + 0x1D3) / `80105aac andi v0,v0,0x80` / `80105ab0 bne v0,zero,0x80105ad8` / erst `80105ab8 jal 0x80015fe8` / `80105ac0 andi 0xf` / `80105acc addiu v0,zero,2817` / `80105ad4 sw v0,4(s1)` | stimmt; Port case 3: Riegel vor dem Wurf (C-Kurzschluss), Zwilling `re15_player_is_grabbed()` wie Produzent (2) |
| **N2: Leiter-Ende @0x80105B30-9C** | `80105b30 lbu a0,536(s1)` / `80105b34 lbu v1,332(s1)` / `80105b38 addiu v0,zero,257` / `80105b3c bne a0,v1,0x80105ba0` / `80105b40 sw v0,4(s1)` (Delay) / `80105b44 lui v0,0x1` / `80105b48 ori v0,v0,0x101` / `80105b4c jal 0x80015fe8` / `80105b50 sw v0,4(s1)` (Delay, 0x10101) / `80105b54 andi 0xf` / `80105b58 sll 1` / `80105b5c sb v0,363(s1)` / `80105b60 addiu v0,zero,30` / `80105b64 jal rand` / `80105b68 sh v0,344(s1)` (Delay) / `80105b6c andi 0x1f` / `80105b70 addiu 30` / `80105b74 jal rand` / `80105b78 sb v0,362(s1)` (Delay) / `80105b7c addiu v0,v0,300` / `80105b80 sh v0,346(s1)` / `80105b84 lbu v1,536` / `80105b88 lbu v0,332` / `80105b90 bne v1,v0,0x80105ba0` / `80105b98 jal 0x800152c8` / `80105b9c addu a1,zero,zero` / `80105ba0` Epilog | stimmt; Port: gleicher Clip -> 0x10101, Wurfreihenfolge r1 -> +0x16B, r2 -> +0x16A, r3 -> +0x15A = r3 + 300, danach `r2z_r37_152c8`; anderer Clip -> 0x101 ohne Schritt |
| N2: Rohbytes | `80105aa0: 0d 80 02 3c cb fd 42 90 00 00 00 00 80 00 42 30 09 00 40 14`; `80105b30: 18 02 24 92 4c 01 23 92 01 01 02 24 18 00 83 14 04 00 22 ae 01 00 02 3c 01 01 42 34 fa 57 00 0c`; `80105b90: 03 00 62 14 21 20 20 02 b2 54 00 0c` (von Hand dekodiert: `0x14830018` = bne a0,v1,+0x18 -> 0x80105ba0; `0x0c0054b2` = jal 0x800152c8) | gleich dem Dossier R5 |
| N2: P3 ohne e7c, Vektor = letzter P2-Takt | Scan 0x80105438-BBC: `jal 0x80015e7c` nur @0x80105590 / @0x801055A8 (P0), @0x8010575C (P1), @0x801058AC (P2); P1/P2 enden im gemeinsamen Schwanz `j 0x80105b84` (@0x80105884, @0x80105A48 u.a.); P2 halbiert NICHT (P1 halbiert @0x80105770-94: `sll 16` / `sra 17`) | stimmt; der in P3 erneut angewandte Vektor ist der volle P2-Vektor = Port `re2z_root144/148` aus `re2z_hit_move` |
| N2: P0 erzwingt den Gang-Clip | `801054bc lbu v1,536(s1)` / `801054c0 lbu v0,332(s1)` / `801054c8 beq v1,v0,0x801054d8` / `801054d0 addu v0,v1,v0` (0xF0000) / `801054d4 sw v0,332(s1)` | stimmt (Gleichheit = Normalfall) |
| N2: 0x800152C8 (RE2 PSX.EXE) | `800152e4 lhu v0,118(s1)` / `800152f0 addu v0,v0,a1` / `800152f8 jal 0x8008e1f4` (Winkel (0, +0x76+a1, 0)) / `80015304 addiu a1,s1,324` / `80015308 jal 0x8008dba4` / `80015310-20` +0x38 += out.x / `80015324-34` +0x40 += out.z | stimmt; Port-Helfer dreht wie `re15_clip_root_motion_delta` (x += (c*dx + s*dz)>>12, z += (-s*dx + c*dz)>>12) |
| N2: e7c legt +0x148 ab | `80015fd8 sh v1,324(t0)` / `80015fdc sh a0,326(t0)` / `80015fe0 jr ra` / `80015fe4 sh a1,328(t0)` (Delay) | stimmt (`re2z_root148` in `enemy_ai_common.c`) |
| N2: Gang-Zeilen-Timer +0x158 | EXEC[1] `80101a5c beq v1,zero,0x80101a74` (P0) / `80101a64 beq v1,v0(=1),0x80101b24` (P0 uebersprungen) / P0 `80101ac8 sh v1,344(s1)` (Zeilen-Timer) / P1 `80101b24 lhu v1,344(s1)` / `80101b2c addiu v0,v1,-1` / `80101b30 bne v1,zero,0x80101bac` / `80101b34 sh v0,344(s1)` | stimmt: nach 0x10101 liest P1 die 30 aus P3 als Zeilen-Timer (30 -> 29); Port `re2z_gaittmr = 30` + Gang-Tag (sonst zweite Gang-Init mit 2 Wuerfen) |
| RE2-rand | `80016004 andi v0,v0,0xff` / `80016014 andi v0,v0,0xff` | 0..255 -> +0x15A = 300..555 (Port `re2z_rand` `return v & 0xff`) |
| (1) Timer @0x80101D68-DBC | `lhu 346` / `addiu -1` / `sh` / `sll 16` / `bne -> 0x80101dc0` / `jal rand` / `sra 3` / `addiu 100` / `jal rand` (Delay `sh 346`) / `andi 3` / `bne` / `lw 496` / `sltiu 0x1388` / `beq` / `addiu 513` / `sw v0,4(s1)` | stimmt (`re2z_t15a--` ... `r2z_r37_auf_0x201(e,0)`) |
| (2) @0x80101EC4-F10, (3) @0x80101F14-F60 | `sltiu 0x7d0` / `jal 0x80015614` a3 = 512 / `lbu 0x800cfdcb` / `andi 0x80` / `addiu 513` / `sw`; `lhu 0x800cfbf6` / `andi 0x4` / `sltiu 0x9c4` / `jal 0x80015614` a3 = 1324 / `addiu 513` / `sw` | stimmt |
| (b) @0x801013BC-D0, Block 2 @0x80101360-78 | Block 3 `andi 1` / `beq v0,zero,0x801013d4` / `addiu 3073` / `sw`; Block 2 `andi 1` / `bne v0,zero,0x80101380` / `addiu 3073` / `sw` | stimmt (Block 3 bei 1, Block 2 bei 0) |
| (c) @0x8010151C-5A0 | `80101540 sb v0,6(s0)` faellt nach `80101544 lh v0,344(s0)`; `8010157c sw 257`; `80101588 beq v0,zero,0x801016ac`; `8010159c j 0x801016ac` (Delay `sh` Dekrement) | stimmt; Port-Wecker nur im P1-Block erreichbar |
| D12 @0x8010460C, Tabelle 0x8010C88C | `bne v0,v1(3)` / `sltiu a0,0x514` / `lbu 0x800cfdcb & 0x80` / `lbu 333 sltiu 0xb` / `lbu 262` vs `lbu 0x800cfcfe` / `lbu 0x800cfc00 xori 0xf` / zwei Sektoren `jal 0x80015758` a2 = +0x76 +-256, a3 = 256 -> `addiu 769` / `sw`; Tabelle [12] = 0x8010460c, [3..11],[13..15] je `jr ra` / `nop` (alle 12 selbst gelesen) | stimmt |
| OFFEN-Eintraege P0/P1/P2 des Treffer-Handlers | `8010571c jal 0x80015558` (a3 = 16), `80105724-38` Zeile-14-Funke, `8010573c` = P1-Einstieg, kein Sprung; P1 `80105770-94` halbiert; P2-Schwanz @0x80105980-A4C (Hinweis H3) | Beschreibungen stimmen |

Guess-Tells in hinzugefuegten Code-Zeilen (`deferred|tunable|interim|for now|faithful|plausib|TODO|vorerst|ungefaehr|
geschaetzt|approx|getenv`): keine. Jede neue Verhaltenskonstante im Code-Diff traegt ihre @0x-Adresse in derselben Zeile oder
im Zitatblock direkt darueber (0x7d0/512, 0x9c4/1324, 0x20/0x40, 30, +300, 0xf/0x1f); `>> 12` = GTE-Q12 des ApplyMatrixSV;
`0x80 | sub_state_1` = Port-Tag der Gang-Init (bestehende Port-Konvention `re15_re2z_gait_init`). Die Sonden-Maske ist
ohne Env-Weg (Standard 0x7F = alles an; Maske 0 = Stand bis Runde 37 inkl. der alten umgekehrten Bedingung).
Der Fix ERKLAERT den Befund der Abnahme 1: dort F1228 `mo=2` (= Gang-Clip) -> F1229 `ss2=0` (0x101) -> Clip-Neustart; jetzt
(Abschnitt 4.3) im selben Aufbau F1211 `ss2=1` (0x10101) mit dem 0x800152C8-Schritt und fortlaufendem Bild.

## 4. Punkt 4 — Orchestrator-Auftrag L (a)(b)(c) + Nachbesserung 2 (Erholungsleiter P3), Nahkampf an der exe

### 4.1 Code gegen Bytes, historischer Zusammenbruch
(a) Produzenten (1)(2)(3), (b) Block 3, (c) Wecker, DECISION[12] und die Erholungsleiter P3 der Nachbesserung 2: Abschnitt 3,
jede Zeile stimmt. Der historische Nahkampf-Zusammenbruch "3523 -> 0": Ursache laut Dossier M1 = Testaufbau (Item-Modal fror
den Prozess ein), behoben in 182db277 — `git show 182db277` selbst gelesen: `frame()` in `test_re2_zombie_abc.c` faehrt seitdem
das Item-Modal leer (`re15_item_modal.h`, Kommentar mit `game_step_common.c:867`). Heute kein Zusammenbruch: Harness
`test_r37_re2zombie mess <maske> 64 1` selbst gefahren: Maske 0 (Stand bis Runde 37) **368**, 0x3F (Spur L ohne P3-Fix)
**369**, 0x7F (Spur L komplett) **362** Messertreffer (Schranke > 250, >= 95 % von 368). (Den Nachbau an 187fd244 habe ich
nicht wiederholt.)

### 4.2 Nachbesserung 2 — Harness (selbst gefahren, `test_r37_re2zombie.exe mess <maske> 64 <waffe>`)

| Sweep | 0x3F (vor dem Fix) | 0x7F (nach dem Fix) | Dossier |
|---|---|---|---|
| Messer (waffe 1): Treffer / Tode / Griff-Bilder | 369 / 2 / 27473 | 362 / 2 / 27609 | gleich |
| Messer: P3-Ausgaenge 0xB01 / 0xC01 / 0x201 / 0x10101 / 0x101; Wuerfe | 5 / 1 / 47 / 0 / 61; 15305 | 2 / 1 / 57 / 54 / 0; 15325 | gleich |
| Pistole (waffe 3 = Browning HP, wie `run_sweep(...,3,...)` in test_re2_zombie_abc): Treffer / Tode / Griff-Bilder | 1775 / 90 / 17472 | 1826 / 90 / 16520 | gleich |
| Pistole: P3-Ausgaenge; Wuerfe | 7 / 3 / 86 / 0 / 82; 14118 | 1 / 2 / 96 / 80 / 0; 14280 | gleich |

(Maske 0, Pistole: 1795 / 96 Tode — Stand bis Runde 37.) Die fuenf verschobenen Riegel (Dossier-Tabelle "neu") selbst
verbose gelesen: `unit_re2_zombie_abc` "Pistole: 1826 Treffer / 90 Tode / DOWN 96..322 ueber 250 Fenster; Messer: 361
Treffer", `unit_r37_re2zombie_nahkampf` "361 gegen 368", `unit_r36_zombieki_szenen` schuss3 F148 weg=804 / schuss6 F321
weg=807, `unit_re2_weapon_rows` w=3/4/7/12/20 Clip 0->4 / 0->0 / 0->1 / 0->4 / 0->0, `unit_re2z_bandlock_pin` 331 (0.54 %)
in 3/64 — alle wie im Dossier, alle gruen. Den "alt"-Wert (Stand vor dem Fix) habe ich nicht neu gebaut.

### 4.3 An der exe: Erholungsleiter Bild fuer Bild (Messer-Lauf w = 20.200, Platz 5 Typ 0x11, state.log)

| Bild | Spur L (exe dieses Baums) |
|---|---|
| F1210 | `st=2 ss1=1 ss2=3 mo=2 af=13` @(-4969,13767) — P3 steht an |
| F1211 | `st=1 ss1=1 ss2=1 mo=2 af=14` @(-4973,13780) — **0x10101**, Schritt (-4,+13) = 0x800152C8 @0x80105B98 |
| F1212 | `ss2=1 af=15` — Gang laeuft weiter (kein P0, kein Zufalls-Startbild) |

Gegenstueck an der Basis-exe (alte, umgekehrte Bedingung), Lauf w = 20.167: F1194 `st=2 ss1=1 ss2=3 af=46` -> F1195 `st=1
ss1=1 ss2=0 af=47` (0x101, kein Schritt) -> **F1196 `ss2=1 af=7`** (EXEC[1] P0: Clip neu auf Zufallsbild). Damit ist die
Dossier-Aussage "F1211 0x10101, Schritt (-4,+13), Bild 14 -> 15" an der exe bestaetigt; der alte Weg zeigt den Clip-Neustart.
Alle 18 0x10101-Ausgaenge der Spur-L-Laeufe tragen einen Schritt (z.B. (-4,+13), (+10,+21), (-1,+24), (+13,+19)); alle
0x201-/0xB01-/0x101-Ausgaenge (beide exe) +0,+0.

### 4.4 Nahkampf an der exe — 18 Messer-Paarlaeufe Spur L / Basis (eigener Treiber `batch2_messer.sh`, Zaehler `abn2_messer.py`)

Aufbau wie Abnahme 1 Abschnitt 2 (selbst neu gefahren, beide exe): Montage-Karte, echte Tuer 1150 -> 1130, `RE15_EQUIP=1`
(debug.log `[equip] RE15_EQUIP -> item 1 (slot 128)`, `W-bank -> W01`), Skript `W1,A0.2,W2,W<w>` + 50x `M0.4,MA0.1`,
`RE15_EXIT_AT=1400#1130` (debug.log `EXIT_AT: Bild 1400 in Raum 1130`); w = 20 + k/30 s (k = 0..14, alle 15 Schlagphasen) +
21.3 / 22.2 / 23.7. Gezaehlt fuer Platz 5 (Typ 0x11, HP 250, 3 je Treffer): HP-Abfaelle vor dem 1. Griff (Anm. 1), zwischen
1. und 2. Griff (Anm. 2; "+" = kein 2. Griff bis F1400), alle bis F1400, Griffe; P3-Ausgaenge aus dem Bild nach dem letzten
`st=2 ss2=3`.

| w | Spur L: Anm.1 / Anm.2 (Griffe) | Basis: Anm.1 / Anm.2 (Griffe) |
|---|---|---|
| 20.000 | 0 / 2 (F888, F1236) | 1 / 3 (F917, F1265) |
| 20.033 | 0 / 1 (F888, F1213) | 1 / 2+ (F918, kein 2. Griff) |
| 20.067 / 20.100 / 20.133 | je 0 / 1 (F888, F1214) | 1 / 1; 0 / 2; 0 / 2 |
| 20.167 | 0 / 2 (F888, F1216) | 0 / 2 (F896, F1226) |
| 20.200 / 20.233 / 20.267 | je 0 / 3 (F888, F1272..1274) | je 0 / 2 |
| 20.300 | 1 / 2 (F904, F1230) | 0 / 2 |
| 20.333 / 20.367 / 20.400 | je 1 / 1 (F904..905, F1333..1357) | je 0 / 2 |
| 20.433 / 20.467 | je 1 / 2 | je 0 / 2 |
| 21.3 | 1 / 2 (F904, F1230) | 0 / 2 |
| 22.2 / 23.7 | je 0 / 3 (F888, F1272) | 0 / 2; 1 / 3 |
| **Summe** | **Anm.1 7**, Anm.2 (16 Paare wie Abnahme 1) **31**, alle **41**, Griffe **36** | **Anm.1 4**, Anm.2 (16 Paare) **33**, alle **41**, Griffe **35** |

P3-Ausgaenge Spur L: 20x 0x201, **18x 0x10101**, 3x 0xB01, **0x 0x101**; Basis: 0x101 in fast jeder Erholung. Die Spur-L-Zeilen
sind Lauf fuer Lauf gleich der Dossier-Spalte "nach" (7 / 31 / 41 / 36, P3 20 / 18 / 3), die Basis-Zeilen gleich der
Basis-Spalte der Abnahme 1 (4 / 33 / 41 / 35). Der Nutzertext "je Anmarsch im Mittel GLEICH viele Treffer ... (erster
Anmarsch 7, zweiter 31; vor Spur L 4 / 33), je nach Schlagtakt einer mehr oder weniger" stimmt damit.

Urteil Punkt 4: **erfuellt** — Code = Bytes fuer (a)(b)(c), D12 und die Erholungsleiter; an der exe heben die Zombies die
Arme und kommen auf Leon zu (Abschnitt 1), greifen aus dem Arme-hoch-Gang und aus dem Ausfall und beissen (HP-Verlauf);
der Nahkampf wird nicht schlechter (alle Messertreffer bis F1400 41 = 41, je Anmarsch 7 / 31 gegen 4 / 33, Griffe 36 gegen
35). Die Erholung nach einem Treffer laeuft jetzt wie im Original (0x10101 statt 0x101 -> Clip-Neustart). Zensus:
die verbleibenden OFFEN-Marken mit Wirkung auf die Aktivitaet (P2-Schwanz) sind mit Adresse und Vertragsgrund begruendet
(Hinweis H3), wie der Auftrag es zulaesst ("mitschliessen oder mit Adresse begruenden, warum nicht").

## 5. Vertrag (VERTRAG.md Runde 37 + Runde 38) und Tests

* Pfade `git diff master...HEAD --name-only`: `analysis/befunde_runde37/L_*`, `engine/src/enemy_ai_re2_zombie.c` (VERTRAG 37
  §1.4 Ausnahme Spur L; Treffer-Handler case 3 = Freigabe AUFTRAG/VERTRAG Runde 38 "Fortsetzung L"), `engine/src/
  enemy_ai_common.c` (+2 Zeilen, `(Runde 37 Spur L)`, gemeinsame Datei nach §1.4), `include/re15_actor.h` (+1 Feld +
  Kommentar), NEU `include/re15_re2zombie_r37.h`, Tests (`tests/unit/test_r37_re2zombie.c`, `tests/unit/probes/
  r37_re2zombie.cmake`, `tests/integration/test_r37_re2zombie.cmake` mit `cmake_minimum_required(VERSION 3.16)`; angepasst
  `test_r36_zombieki_eintritt.c`, `probe_r36_blut.c`, `probe_10d0_situp_re2.c`, `test_r34_granaten.cmake`,
  `test_r35_granate.cmake`, je mit @0x-Begruendung im Kommentar und alt/neu im Dossier). Keine `release/`,
  `platform/android/`, `shared_assets/PSX/`, keine Test-CMakeLists, keine neue .c-Datei. Keine POSIX-Aufrufe im neuen Test.
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine. `re15_actor_t` wird nirgends roh serialisiert (nur memset;
  grep `sizeof(re15_actor_t)` / `fwrite`), das neue Feld aendert kein Speicherformat.
* Griff-/Biss-Code (Spur J) und `re2z_init` (Spur H): unberuehrt. Runde 38: Spur U haengt parallel in
  `enemy_ai_re2_zombie.c`, Spuren B/T in `enemy_ai_common.c` — Merge-Naehe bei der Integration beachten.
* Treffer-Handler: nur case 3 (H1); P0-P2 und der Advancer unveraendert (OFFEN mit Adressen).

Testlaeufe (Baum r37_re2zombie, keine volle Suite):
1. `ctest --test-dir re15_port/build -R "^unit_r37_re2zombie_" -V --timeout 600` -> **`100% tests passed, 0 tests failed out
   of 10`**; gelesen: prod1 4/4, prod2 5/5, prod3 5/5, b 3/3, c 6/6, d12 6/6, **p3 8/8** (16x 0x10101 / 16x 0x201, Schritt
   (37,-5) -> (-5,-37), Folgetakt Zeilen-Timer 30 -> 29, Gegenproben anderer Clip / beansprucht (45 Lagen ohne 0xB01) / Stand
   bis Runde 37), nahkampf 2/2 ("361 gegen 368"), raum1130 4/4 (Wechsel bei 1991), raum1180 4/4 (Mittel 1993).
2. `ctest --test-dir re15_port/build -R "re2|zomb|zgirl|granat|lyer|messer|crawl|kriech|gegnerzahl|1140|1130|1180|enemy|biss|griff|grab|bump|kfjump|teardeath|blut|bandlock|rise|r37_|irons|1150|dog|crow|spider|affen|situp|10d0|hund|kraehe|gore|wecker|birkin|elza|gator|g5|boss|zellenarm|maggot|wurm|plant|spinne|gorilla|root|ragdoll|schuss|knife|hit|kampf|ai_" -E "^integration_" -j 4 --timeout 600 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 306`** (52 s).
3. Integration seriell: Teil 1 brach nach 7 gruenen von 16 Laeufen am Sitzungslimit ab (Scratch `abn2L/integ.log`, Start
   18:01:15: r30_granate_laden, r34_granaten, r35_granate, r35_werfer_elza, r35_zgirl, r35_zgirl_5090, r36_elza_schuss je
   `Passed`, dann Abbruch in r36_gegnerzahl). In Teil 2 vollstaendig neu gefahren (18 Haken): Abschnitt 6.5.

## 6. Teil 2 — Fortsetzung nach dem Sitzungsabbruch (2026-10-16 ab 21:05)

Teil 1 (Abschnitte 0-5) endete am Sitzungslimit vor dem Ende der Integrationslaeufe; die Platzhalter fuer das Urteil und das
Integrationsergebnis standen noch im Text. Teil 2 hat den Stand neu gebaut, die Belege aus Teil 1 gegen den gebauten Stand
reproduziert bzw. aus den Rohdaten nachgezaehlt, drei Stellen erneut selbst disassembliert, zwei Port-Eigenheiten des P3-Fixes im
Code nachgeprueft und die Integrationslaeufe vollstaendig gefahren. Rohdaten Teil 2: Scratch `abn2L/teil2/`.

### 6.1 Bau und Stand
* HEAD beim Start von Teil 2 `a17a649a` (seit `e4bca518` nur der Teilbericht), `git status` sauber. `bash re15_port/tools/local_build.sh configure`
  -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (EXIT=0).
* exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-16 16:57:25, md5 `03503eba76de5a0aaefc6dba426cf039` = dieselbe exe
  wie in Teil 1 (juengste Quelle `enemy_ai_re2_zombie.c` 16:43:25). Der letzte re15_port-Commit `e2daa221` aendert nur
  einen Meldungstext in `test_r37_re2zombie.c` (`git show e2daa221 -- re15_port`); `test_r37_re2zombie.exe` 17:23:11 ist
  juenger als die Quelle (17:07:05).
* Basis-exe (Hauptbaum, ohne Spur L) md5 `bf27fdb9aab1cee84d267ae29627e604`, 2026-10-14 07:19:50; `git merge-base HEAD
  master` = `e2cc7673`; master seither nur Doku (`git diff master --name-only` unter re15_port = genau die zwoelf
  Spur-L-Dateien, gleich dem Drei-Punkt-Diff).

### 6.2 Reproduktion und Nachzaehlung der Teil-1-Belege
* **Determinismus:** L1 (Montage-Karte, echte Tuer 1150 -> 1130, Leon steht, `RE15_EXIT_AT=1100#1130`) mit derselben exe neu
  gefahren (`abn2L/teil2/runs/T2_L1_1130_steht`, exit=0, 64 s): `state.log` md5 `7c36bc80c677f6fd17461a61d059d6a2` =
  Teil-1-Lauf `runs/L1_1130_steht` BITGLEICH (1191 Zeilen). Ebenso der Messer-Lauf w = 20.200 (`RE15_EQUIP=1`, debug.log
  `[equip] RE15_EQUIP -> item 1 (slot 128)`, `EXIT_AT: Bild 1400 in Raum 1130`; `teil2/runs/T2_Lm_w20.200`, 94 s): `state.log`
  md5 `159ae87ead1fbb88960b196b4af57a27` = `runs_messer/Lm_w20.200` BITGLEICH, Zaehler: `anm1 0 anm2 3 alle 3 griffe 2 |
  ... | P3 F1211 0x10101 (af 13->14, Lage -4,+13); F1241 0x10101 (af 21->22, Lage +10,+21); F1271 0x10101 (af 21->22,
  Lage -1,+24)`. Damit gelten die Rohdaten aus Teil 1 fuer den gebauten Stand.
* **Nachzaehlung (abn2_state.py auf den Teil-1-Rohdaten):** L1 Platz 5 `F869 1/1->1/2 d=1991`, `F888 1/2->1/3 d=1108`,
  Spieler-HP `F909 100->70, F947 70->40`; B1 kein 1/2, `F896 1/1->1/3 d=1166`. L2 1/2 bei 4283 / 1717 / 4692 / 1978 / 1936 /
  1273 / 738, Griffe aus 0x201 bei 1180 / 1193, HP -> -20 (F852). L3 (erster 1130-Besuch) 1/2 bei 2346 / 2442 / 2346 / 2346,
  `F471 1/2->1/12` Ausfall, `F481 1/12->1/3 d=1280` Griff aus dem Ausfall, `F352 1/2->1/3 d=1180`. L4 1180 / L5 1181
  (debug.log `[boot] room RDT: STAGE1/ROOM1181.RDT`, `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `CONTINUE:
  resumed in room 1181`) je `F283 1/2 d=1995 -> F311 Griff d=1193`, `F802 1/2 d=1991 -> F836 Griff d=1180`; B4/B5 nur
  Griffe aus dem Gang (1187 / 1173). L6 1010 1/2 bei 1995 / 849 / 1978 / 1995, Griffe aus 0x201 bei 849 / 1193; B6 kein
  1/2. Alles wie Abschnitt 1.
* **Messer (36 Laeufe `runs_messer`, abn2_messer.py, Platz 5 bis F1400):** L Anm.1 **7**, alle **41**, Griffe **36**, P3
  **18x 0x10101 / 20x 0x201 / 3x 0xB01 / 0x 0x101**; B Anm.1 **4**, alle **41**, Griffe **35**. (Anm.2 ueber ALLE 18 Laeufe:
  L 34 aus 18 vollstaendigen, B 35 aus 17 — Abschnitt 4.4 nennt die 16 Paare der Abnahme 1: 31 / 33.) w = 20.200: L P3
  `F1211 0x10101 (af 13->14, Lage -4,+13)`, `F1241 (+10,+21)`, `F1271 (-1,+24)`; B `F1196 0x201`, `F1226 0x101 (+0,+0)`.

### 6.3 RE-Gate — in Teil 2 erneut selbst disassembliert (`re2_disasm.py`; EMZ0.BIN = EMOVL10_S0.BIN = EMZ0_d1.BIN, md5 `7f7a39e651064e07c95830db1bebdb89` selbst geprueft)
1. **DECISION[12] 0x8010460C-744** (`dis 0x8010460C 80 --bin EMZ0.BIN`): alle fruehen Ausstiege — `80104634 bne v0,v1,0x801046a0`
   (Phase != 3), `8010463c sltiu v0,a0,0x514` / `beq`, `80104658 bne` (0x800CFDCB & 0x80), `8010466c beq` (+0x14D < 11),
   `80104684 bne` (Etage +0x106 gegen 0x800CFCFE) — landen auf `801046a0 beq a1,zero,0x80104730` mit a1 = 0 aus dem
   Delay-Slot `80104638 addu a1,zero,zero` -> Epilog. Nur der Durchlauf setzt `8010469c sltu a1,zero,v0` (Spieler+0x8 != 15).
   Danach Sektor 1 `801046a8 lhu 538 / andi 0x20 / bne`, `801046c0 lh a2,118` / `801046c4 addiu a3,zero,256` /
   `801046c8 jal 0x80015758` / Delay `addiu a2,a2,256`, `801046d0 bne` / `801046d4 addiu v0,zero,769` / `801046d8 sw v0,4(s0)`,
   `801046dc-e8` PL+0x1D3 |= 0x80; Sektor 2 `801046ec-72c` mit `andi 0x40` und `addiu a2,a2,-256`. Port `re2z_decide_lunge`:
   fruehe `return`s, dann beide Sektoren (last writer wins) — stimmt; die Zwischenstation 0x801046A0 ist KEIN zweiter Weg in
   die Sektoren.
2. **EXEC[1] 0x80101D60-F78** (`dis 0x80101D60 92`, `dis 0x80101EC4 46`): (1) `80101d68 lhu v0,346` / `addiu -1` / `sh` /
   `80101d78 sll v0,v0,16` / `80101d7c bne v0,zero,0x80101dc0` / `80101d84 jal 0x80015fe8` / `sra 3` / `addiu 100` /
   `80101d94 jal 0x80015fe8` (Delay `sh v0,346`) / `andi 3` / `bne` / `80101da8 lw v0,496` / `sltiu 0x1388` / `beq` /
   Delay `addiu v0,zero,513` / `80101dbc sw v0,4(s1)`; (2) `80101ecc sltiu 0x7d0`, `80101ee8 jal 0x80015614` (a1/a2 =
   0x800CFC30/0x800CFC38 = Spieler x/z, `addiu a3,zero,512`), `80101efc lbu 0x800cfdcb` / `andi 0x80` / `bne`,
   `80101f0c addiu 513` / `80101f10 sw` und OHNE Sprung weiter nach (3): `80101f18 lhu 0x800cfbf6` / `andi 0x4` / `beq ->
   0x80101f64`, `80101f34 sltiu 0x9c4`, `80101f54 addiu a3,zero,1324`, `80101f5c addiu 513` / `80101f60 sw`; Epilog
   0x80101F64. Port: (1) im Timer-Block, (2) und (3) nacheinander nach dem DoT, beide ausgewertet — stimmt.
3. **Treffer-Handler P3 0x80105A50-BA0** (Rohbytes 0x80105B30-9F wie Abschnitt 3) und **0x800152C8 (RE2 PSX.EXE)**:
   `800152e0 sh zero,16(sp)` / `800152e4 lhu v0,118(s1)` / `800152ec sh zero,20(sp)` / `800152f0 addu v0,v0,a1` /
   `800152f8 jal 0x8008e1f4` (Delay `sh v0,18(sp)`) = Winkel (0, +0x76 + a1, 0) -> Matrix; `80015304 addiu a1,s1,324` /
   `80015308 jal 0x8008dba4` = Matrix auf den Vektor +0x144/+0x146/+0x148; `80015310-20` +0x38 += out.x,
   `80015324-34` +0x40 += out.z. Der Port-Helfer `r2z_r37_152c8` benutzt die IDENTISCHE Drehung wie die seit Runde 12
   bestehende Anwendung desselben Paars e7c+152c8 in `re15_clip_root_motion_delta` (enemy_ai_common.c Z.773-775:
   `x += (c*dx + s*dz) >> 12`, `z += (-s*dx + c*dz) >> 12`) — stimmt.
Weitere Pruefungen am Code (Teil 2):
* `re15_ai_set_state_word` (enemy_ai_common.c Z.103-110) schreibt nur die vier Bytes, keine Nebenwirkung — das doppelte
  Setzen 0x101 -> 0x10101 im Gleichheitsfall entspricht den zwei `sw` @0x80105B40 (Delay-Slot) / @0x80105B50.
* Herkunft des Vektors: `re2z_root144/148` schreibt nur `re15_clip_root_motion_delta` (Aufrufer fuer RE2-Zombies:
  `re15_re2z_move_root` / `re15_re2z_root_probe`; die zwei weiteren Aufrufer Z.10061 / Z.11391 sind NPC- bzw.
  Schaben-Pfade). Im P3-Takt laeuft vor `case 3` kein weiterer e7c-Zwilling fuer diesen Platz -> der Vektor ist der des
  letzten P2-Takts, wie im Original (e7c nur in P0/P1/P2 @0x80105590/A8/75C/8AC, Teil 1 Abschnitt 3).
* Zwei Felder fuer das eine Original-Feld +0x158: der Gang liest im Port nur `re2z_gaittmr` (re2z_gait_tick Z.407-427,
  Tag-Pruefung Z.415); `re2z_t158` liest im Gang niemand (grep: Leser nur in EXEC[0]/[2]/[3]/..., im Treffer-Lean
  re2z_lean_angle Z.4389 nur in den Reaktions-Phasen P1/P2). Das Tag `0x80 | 1` aus P3 verhindert die zweite Gang-Init im
  Folgetakt; jeder spaetere Gang-Eintritt ueber 0x101 laeuft weiter durch P0 (`re2z_exec_walk` Z.1670-1687 setzt Tag und
  Seeds neu). Kein Befund.
* Guess-Tells im hinzugefuegten Code (`deferred|tunable|interim|for now|faithful|plausib|TODO|vorerst|ungefaehr|geschaetzt|
  approx|getenv|sieht richtig|nach Gefuehl`): im Engine-/Include-Diff keine; einziges `getenv` im ganzen Diff ist
  `RE15_ASSET_DIR` im Test `test_r37_re2zombie.c` Z.177 (Asset-Pfad der Sonde, kein Verhalten).
* Werkzeug-Befund H4 selbst gelesen: `local_build.sh` Z.326 `Get-Process re15_pc | Where-Object { $_.Path.StartsWith('$_bw',
  ...) } | Stop-Process -Force` — trifft nur Prozesse mit dem Namen `re15_pc`, deren Pfad mit dem Bauordner BEGINNT, also
  auch `re15_port/build_l2/.../re15_pc.exe`; Messkopien mit eigenem Namen (`re15_pc_abn2L_*`, `re15_pc_r37l_haken_*`) sind
  nicht betroffen.

### 6.4 Spur-Tests (Teil 2)
`ctest --test-dir re15_port/build -R "^unit_r37_re2zombie_" -V --timeout 600` -> **`100% tests passed, 0 tests failed out
of 10`**; gelesen: prod1 4/4, prod2 5/5, prod3 5/5, b 3/3, c 6/6, d12 6/6, **p3 8/8** ("16 x 0x10101, 16 x 0x201", "(37,-5)
-> (-5,-37)", "Gang-Zeilen-Timer 30 -> 29", "anderer Clip ... 0x101 ... 16 Lagen", "0xB01 genau bei r0&0xf == 0 (8 Lagen)",
"BEANSPRUCHT ... nie 0xB01 ... (45 Lagen)", "Stand bis Runde 37 ... 16 / 8 Lagen"), nahkampf 2/2 ("361 Treffer", "361 gegen
368 (>= 95 %)"), raum1130 4/4 ("1 Wechsel, davon (2) 1", "1991", "1 Griffe aus 0x201"), raum1180 4/4 ("2 Wechsel", "1993",
"2 Griffe aus 0x201"). Gegenproben "Maske 0: kein Arme-hoch-Gang vor Leon (0)" in beiden Raum-Tests.

### 6.5 Integration (Teil 2, seriell, Baum r37_re2zombie)
Auswahl = die 16 zombie-/kampfnahen Haken aus Teil 1 + `integration_r36_kleidung_hundeweg` (Hund, zieht aus DEMSELBEN
RE2-Zufallsstrom `re15_re2_rand`) + `integration_r37_elzaems_elza1101` (Elza, Zombies in 1101):
`ctest --test-dir re15_port/build -R "^integration_(r30_granate_laden|r34_granaten|r35_granate|r35_werfer_elza|r35_zgirl|r35_zgirl_5090|r36_elza_schuss|r36_gegnerzahl|r36_granate|r36_schuss|r37_beretta|r37_bissarm_echtlauf|r37_elzaems_leon1100|r37_elzaems_elza1181griff|r37_re2zombie|r37_zombieaktiv|r36_kleidung_hundeweg|r37_elzaems_elza1101)$" --timeout 1200 -V`
(seriell, Start 21:10; parallel bauten/testeten die Baeume r37_masken36 und r38_* — Laufzeiten deshalb hoeher) ->
**`100% tests passed, 0 tests failed out of 18`**, `Total Test time (real) = 2404.37 sec`, EXIT=0 (Log `abn2L/teil2/integ_v.log`).

| Test | Zeit | Meldung (Auszug aus `-V`) |
|---|---|---|
| integration_r30_granate_laden | 135,8 s | Passed |
| integration_r34_granaten | 348,9 s | `[g9_re2]: Gegner 2 ... Leiche ab Bild 165`, `Gegner 3 ... Leiche ab Bild 258` (= Dossier-Wert nach Spur L), `[abzug]: ok` |
| integration_r35_granate | 313,1 s | Passed |
| integration_r35_werfer_elza | 40,8 s | Passed |
| integration_r35_zgirl / _5090 | 476,2 / 172,9 s | Passed |
| integration_r36_elza_schuss | 47,2 s | Passed |
| integration_r36_gegnerzahl | 97,3 s | `OK - 1040 Records 15..19 = Aktor 16..20 (a), 1030 dito (b), Kontrolle 1..5 (c), 1030 17 tot -> 18..20 (d), 20B0 21 Schatten (e)` |
| integration_r36_granate | 101,2 s | Passed |
| integration_r36_kleidung_hundeweg | 70,6 s | Passed |
| integration_r36_schuss | 160,0 s | Passed |
| integration_r37_beretta | 196,4 s | Passed |
| integration_r37_bissarm_echtlauf | 66,5 s | `OK — im Griff liegen die gezeichneten Zombie-Haende auf dem RE2-Kontaktpunkt (Leon und Elza), das Opfer bleibt am Greifer` |
| integration_r37_elzaems_elza1101 / leon1100 / elza1181griff | 43,1 / 26,0 / 15,0 s | `[elza1181griff]: Zombie 0x10 aus CDEMD1 ...`, `Elza im Griff: Opfer-Clip 1 / 6 ...`, je `OK` |
| **integration_r37_re2zombie** | 57,2 s | `ROOM1130 Wechsel in den Arme-hoch-Gang 1 (davon aus dem Gang bei +0x1F0 < 0x7D0: 1; Abstaende 1991), Griffe aus dem Arme-hoch-Gang 1`, `OK` |
| integration_r37_zombieaktiv | 36,1 s | `OK — 5/5 im Gang ab Bild 7, 5/5 >= 4500 naeher nach 300 Bildern` (Slot 1..5: 4952 / 6616 / 4892 / 7412 / 8948 naeher) |

Ergebnis Tests: Spur-Riegel 10/10, Integration 18/18 (seriell, unter Last ohne einen einzigen roten Lauf), dazu die
Unit-Auswahl 306/306 aus Teil 1 auf denselben Binaerdateien. Keine Regression in den Nachbarmodulen (Granaten-Zeile 9,
Beretta, Schuss-Resolver, Biss-Arm, Elza-EMS, Hund im selben Zufallsstrom, Gegnerzahl/Schatten).

### 6.6 Urteil Teil 2
Teil 2 bestaetigt jedes Teil-1-Urteil: die Teil-1-Rohdaten sind mit der gebauten exe bitgleich reproduzierbar (6.2), der
Code entspricht an den nachgeprueften Stellen den RE2-Bytes (6.3), die zwei Port-Eigenheiten des P3-Fixes (doppelte
Zustandswort-Schreibung, eigenes Gang-Timer-Feld mit Tag) verhalten sich wie das Original (6.3), und alle gezielten Tests
sind gruen (6.4, 6.5). **BESTANDEN.** Keine Maengel; Hinweise H1-H6 oben.

Nicht Teil dieser Abnahme: der Nutzer-Befund vom 2026-10-16 "ROOM 20A0 ... the shutter ... DIRECTLY in the camera" ist
Runde 38 Spur W zugeteilt (master `df08d83a`); Spur L beruehrt nur die Zombie-KI (keine Prop-/Kamera-/Masken-Datei im Diff).
