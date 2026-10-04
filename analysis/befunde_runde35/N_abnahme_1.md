# Runde 35 Spur N "android" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-04. Baum `.claude/worktrees/r35_android`, Zweig r35/android, HEAD 575c964d, Basis 154a73c1.
Massstab: Wortlaut AUFTRAG.md Z. 100-102. Gemessen am selbst gebauten Stand. Vom Bau-Agenten wurde nur uebernommen,
was hier nachgemessen ist. Eigene Messungen sind bewusst ANDERE als die des Bau-Agenten und der Abnahme 0: andere
Displaygroessen, ein laengerer Text, andere Konflikt-Szenarien und andere simulierte Aenderungen.

**Ergebnis: NICHT bestanden.** Punkt 1 erfuellt, Punkt 2 erfuellt, Punkt 3 teilweise.
Die Behauptungen der Nachbesserung 1 stimmen: 243/243 Faelle, 781/785 Mutanten, 32/32, N10, 17/17, Pins, Python 3.9/3.10/3.14.
Eine neue Klasse kuenftiger Aenderungen bleibt aber unbemerkt. Einseitige Lockerungen von Vergleichen (`!=` -> `<` oder `>`)
und das Streichen der Tuer-Soll-Regel im Modus quellbaum laufen, neu gepinnt, sowohl durch den Urteils-Selbsttest als auch
durch die ganze ctest-Kette (FEHLER=0). Das betrifft 10 von 28 nicht gleichwertigen eigenen Python-Aenderungen, 5 von 13
eigenen bash-Aenderungen und 2 von 2 Ende-zu-Ende-Laeufen. Die Gates (Suite, @0x/PORT-WAHL, Pfade) halten. Maengel M1-M3 unten.

---

## 0. Bau und Werkzeug

- `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)` ("ninja: no work to do").
  Seit Abnahme 0 hat sich kein C-Code geaendert: `git diff 5c1f0af4..HEAD --stat -- re15_port` zeigt nur README.md,
  probes/r35_android.cmake (Kommentar) und test_r35_android_pruefkette.sh.
  `ctest -N` -> `Total Tests: 482`.
- Python: `release/python_finden.sh` -> `/c/Python310/python (3.10.11)`. Gegenproben mit `/c/Python39/python` (3.9.0)
  und `/c/msys64/mingw64/bin/python3` (3.14.7).
- Vorher-Stand fuer Punkt 1/2: eigener Pruefstand im Scratchpad. Gebaut aus `git show 154a73c1:` von android_glue.c,
  asset_abgleich.c und .h, dazu pruefstand_main.c von HEAD, mit denselben Flags wie build.ninja (`-std=gnu11 -include
  kompat_win.h`, Attrappen `tests/unit/r35_android/stub`). Ergebnis: `pruefstand_alt.exe`.
  Nachher-Stand: `re15_port/build/tests/unit/r35_android_pruefstand.exe`.
- Eigene Werkzeuge (Scratchpad dieser Sitzung, nicht im Repo): `n1_mess_entpacker.py` (Punkt 1/2),
  `n1_eigene_aenderungen.py` (Python-Urteil, E-Reihe), `n1_kette_varianten.py`/`.sh` (echte Kette, F/G-Reihe).
  Jede entscheidende Aenderung steht unten woertlich und laesst sich ohne diese Dateien wiederholen.

## 1. Punkt 1 — "Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten"

### Messung (15 eigene Displaygroessen x 3 Texte, vorher und nachher)
- Groessen: 2340x1080, 2520x1080, 2960x1440, 3440x1440, 3840x1600, 5120x2160, 6400x1080, 7680x2160, **10240x1440**,
  **15360x1080** (rund 128:9), 2400x1080, 1920x1080, 854x480, 640x360, 1600x2560 hochkant.
- Texte: (u) Uebergangstitel `RE1.5 PORT - ASSETS WERDEN EINMALIG GEPRUEFT` mit Fortschrittszeile; (f) Liste mit
  10000 gelisteten, fehlenden Dateien -> **`10000 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG` (61 Zeichen)**,
  laenger als alle bisher gemessenen Texte, dazu Fortschrittszeilen bis `10001 / 10001 DATEIEN (100%)`; (l) "zuletzt
  entpackt" ist ein Ordner -> 58-Zeichen-Fehlertext.
- Ausgewertet: `PRUEFSTAND-AUSSERHALB`/`-UEBERLAPPUNG` je Bild, dazu der ganze Text im `PRUEFSTAND-BILD`.

| Stand | Laeufe | Text/Rechteck ausserhalb oder Ueberlappung |
|---|---|---|
| vorher (154a73c1) | 45 | **31** |
| nachher (HEAD) | 45 | **0**, ganzer Text in allen 45 sichtbar |

Auszug (`PRUEFSTAND-ZEILE`):
```
vorher  2340x1080 u  x=-150..2480 s=10      nachher 2340x1080 u  x=114..2218 s=8
vorher  2400x1080 f  x=-447..2838 s=9       nachher 2400x1080 f  x=102..2292 s=6   (61 Zeichen)
vorher  2960x1440 f  x=-716..3664 s=12      nachher 2960x1440 f  x=199..2754 s=7
vorher  5120x2160 f  x=-917..6018 s=19      nachher 5120x2160 f  x=181..4926 s=13
vorher  1600x2560 l  x=-3028..4606 s=22     nachher 1600x2560 l  x=104..1492 s=4
nachher 15360x1080 f x=6033..9318 s=9 (hoehenbegrenzt, mittig)   nachher 10240x1440 u x=3404..6823 s=13
```
ctest `unit_r35_android_anzeige` selbst gefahren: Passed (3,6 s).

**Urteil Punkt 1: erfuellt.** Kein Text verlaesst bei irgendeiner gemessenen Breite oder Hoehe das Bild. Die Ursache
(Breite nie gegen W geprueft) ist im alten Code reproduziert (x < 0 an denselben Stellen).

## 2. Punkt 2 — "Ein Update, bei dem eine Datei und ein Ordner gleichen Namens kollidieren, bricht sauber ab; erst der zweite Start heilt es"

### Messung (5 eigene Szenarien; Start 1 = APK A frisch, Start 2 = APK B als Update, Start 3 = B noch einmal)
Nach jedem Start wird der Baum verglichen: Dateien und Inhalte = APK, keine zusaetzliche Datei, kein Ordner ohne
gelistete Datei, `re15_assets_entpackt.txt` = Liste.

| Szenario (eigene) | vorher Start 2 | vorher Start 3 | nachher Start 2 | nachher Start 3 |
|---|---|---|---|---|
| U1 mittlere Ebene wird Datei: `PSX/d/e/f` -> Datei `PSX/d/e`, Nachbar `PSX/d/g` bleibt | rc 1, `FEHLER beim Entpacken: rename`, ABBRUCH | rc 0, voller Lauf | **rc 0, SPIELSTART, Baum = B** | schneller Weg |
| U2 beide Richtungen zugleich: `PSX/m` -> `PSX/m/n/o` UND `PSX/k/l` -> Datei `PSX/k` | rc 1 | rc 0 | **rc 0, Baum = B** | schneller Weg |
| U3 synchro, Gross/klein, tief: `synchro/STAGE1/ROOM9/x.wav` -> Datei `synchro/STAGE1/room9` | rc 1, ABBRUCH | rc 0 | **rc 0, Baum = B** | schneller Weg |
| U4 fremder UNTERORDNER mit 2 Dateien im Ordner `PSX/q/`, der Datei werden muss | rc 1, ABBRUCH | rc 0 | **rc 0**, `Konflikt geraeumt: shared_assets/PSX/q war ein Ordner, wo die Datei hin muss mit 2 Dateien darin - entfernt` | schneller Weg |
| U5 Ordner `PSX/DATA/Y.neu/z/` auf dem Zwischendatei-Namen + Datei `PSX/w` auf kuenftigem Ordnernamen | rc 1, 2 Dateifehler | rc 0 | **rc 0**, zwei Meldungen `Konflikt geraeumt: ...Y.neu war ein Ordner auf dem Namen der Zwischendatei` / `...PSX/w war eine Datei, wo ein Ordner hin muss` | schneller Weg |

Vorher zeigen alle 5 Szenarien genau das Bild des Nutzers: Start 2 bricht ab, erst Start 3 stellt den Stand her.
Nachher wird jedes im selben Start fertig. ctest `unit_r35_android_konflikt` (K1-K6, L1/L2) Passed (2,1 s),
`unit_r35_android_abgleich` Passed, `unit_r34a_asset_abgleich` Passed.
Nicht gemessen (wie bisher OFFEN im Dossier, kein Mangel): ein echtes Update APK A -> APK B auf dem Emulator.

**Urteil Punkt 2: erfuellt.**

## 3. Punkt 3 — "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten"

### 3.1 Behauptungen der Nachbesserung 1, nachgemessen
- Pins: `sha256sum release/gate_urteil.py` = `c8fed5ca...25ba8` = `release/gate_urteil.sha256`. `apk_asset_gate.py`
  = `f73c3b1a...f9b6` = Pin. `GATE_URTEIL_MIN_FAELLE/ERKANNT/MAX_GLEICH` = 243/781/4 (apk_pruefen.sh:104-106).
- `"$PY" release/gate_urteil.py --selbsttest` (5,0 s): `Mutanten je Operator: Aufruf 28, BoolOp 10, Regex R0 13, Regex
  R1 261, Regex R2 49, Regex R3 30, Regex R4 9, Regex R5 115, Regex R6 11, Vergleich 39, Zahl 118, Zeichenkette 37,
  if-Bedingung 56, not 9` / `== URTEIL-SELBSTTEST-OK: 243/243 Faelle, 781/785 Mutanten erkannt, 4 als gleichwertig
  begruendet ==`. Dasselbe unter Python 3.9.0 und 3.14.7.
- Werkzeug des Bau-Agenten `werkzeug/urteil_aenderungen.py` neu gefahren: **`SUMME: 32 von 32 bemerkt`**, darunter A5
  (3 Faelle falsch), A6 (3) und A11 (4). M1 der Abnahme 0 ist damit behoben.
- Alt (b19c39fa) gegen neu: `urteil_alt_neu_nb1.py` -> `SUMME: 243 Faelle, 243 gleiches Urteil, 0 anders`.
- ctest `unit_r35_android_pruefkette` selbst gefahren: Passed (79 s). P0-P3 ok, N1-N21 ok, darunter `N10_ok_zeile_
  rueckgabe_1 -> Abbruch: DIE: Selbsttest des Gate-Urteils: OK-Schlusszeile, aber Rueckgabe 1`, `FEHLER=0`. M2 der
  Abnahme 0 ist damit behoben. Die Streich-Messung B0-B17 des Bau-Agenten ist gelesen; mit B2 -> N10 deckt sie sich mit F5 unten.
- Zitate nachgelesen: apk_asset_gate.py:777 `print("   Tuer-Soll: ... %d/%d wie die Engine-Tabelle ...")`, :3441
  `rc=%d (soll %d)`, :1395-1397 `"gleich" if name in gleich else "NICHT gleich/nicht geprueft"`. Alle stimmen.

### 3.2 Eigene simulierte "kuenftige Aenderungen" am Urteil `release/gate_urteil.py` (E-Reihe)
Je Aenderung genau EINE Textersetzung in einer Kopie, dann `<kopie> --selbsttest`. BEMERKT heisst Rueckgabe != 0.
Keine der Aenderungen steht in der Liste des Bau-Agenten oder der Abnahme 0.

| # | Zeile | Aenderung | Selbsttest |
|---|---|---|---|
| **E1** | 155 | quellbaum `sum(baum) != n` -> `sum(baum) < n` | **NICHT** (243/243, 781/785) |
| E1b | 155 | `sum(baum) != n` -> `> n` | BEMERKT (Fall 112) |
| **E2** | 165 | paket `qq != gg` -> `qq > gg` | **NICHT** |
| E2b | 165 | `qq != gg` -> `qq < gg` | BEMERKT (Fall 118) |
| **E3** | 112 | selbsttest `ok_n != n` -> `ok_n < n` | **NICHT** |
| **E4** | 83 | tuer_soll `int(g1) != int(g2)` -> `<` | **NICHT** |
| **E5** | 155 | quellbaum `len(baum) != k` -> `< k` | **NICHT** |
| **E6** | 165 | paket `len(baum) != k` -> `< k` | **NICHT** |
| **E7** | 165 | paket `sum(gg ...) != n` -> `< n` | **NICHT** |
| E7b | 165 | `... != n` -> `> n` | BEMERKT |
| E8 / E8b | 145 | `int(apk_eintraege) != a + 1` -> `<` bzw. `>` | BEMERKT / BEMERKT |
| **E9** | 116 | `a != b` -> `a < b` (Innere Proben) | **NICHT** |
| E10 | 93 | `if rc == 1` -> `if rc >= 1` | BEMERKT (8 Faelle) |
| E11 | 98 | `if rc != 0` -> `if rc > 0` | nicht bemerkt, in der Praxis GLEICHWERTIG (bash-Rueckgabe ist nie < 0) |
| E12 / E12b | 75 | genau_eine `len(t) != 1` -> `< 1` bzw. `> 1` | BEMERKT / BEMERKT |
| **E29** | 136 | `mb != b` -> `mb > b` | **NICHT** |
| E30 | 136 | `mb != b` -> `mb < b` | BEMERKT |
| E13 | 122 | `for f in faelle:` -> `for f in faelle[1:]:` | BEMERKT |
| E14 | 120 | `sorted(...) != list(range(...))` -> `set(...) != set(range(...))` | BEMERKT (Fall 072) |
| E15 | 90 | `m_ok = re.fullmatch(` -> `re.match(` | BEMERKT |
| E16 | 91 | `m_neg = re.fullmatch(` -> `re.search(` | BEMERKT |
| E20 | 139 | `("RE2/DOOR", "RE15DOOR")` -> `("RE2/DOOR",)` | BEMERKT |
| **E24** | 158 | quellbaum `ende(0, ... % (n, k, tuer_soll()))` -> `% (n, k, 2))` | **NICHT** |
| E24b | 148 | apk `..., tuer_soll()))` -> `..., 2))` | BEMERKT (Faelle 108/109) |
| E26 | 116 | `if a != b or b < min_innen:` -> `if a != b:` | BEMERKT |
| E44 | 136 | `q == a == g == mz == n` -> `q == a == g == mz` | BEMERKT |
| E45 | 83 | `if not t or any(` -> `if any(` | BEMERKT |

Bilanz: 29 Aenderungen, 18 bemerkt, 11 nicht. Davon ist E11 in der Praxis gleichwertig. **10 nicht gleichwertige
Lockerungen bleiben unbemerkt** (E1, E2, E3, E4, E5, E6, E7, E9, E29, E24).

Folge nachgewiesen: Die geaenderten Urteile nehmen Gate-Ausgaben an, die das Original ablehnt. Eigene Ausgabe-Dateien,
Aufruf `<urteil> quellbaum <datei> 0 261 148`:
```
Baumzeilen 3622 + 2 + 5 bei "3629 Dateien in 2 Baeumen"   original rc=2 (Baumzeilen [3622, 2, 5] passen nicht ...)   E5  rc=0
Baumzeilen 3627 + 5 bei "3629 Dateien in 2 Baeumen"       original rc=2                                             E1  rc=0
Tuer-Soll "31/30 wie die Engine-Tabelle"                  original rc=2 (Tuer-Soll-Zeilen ... nicht g/g)            E4  rc=0
Tuer-Soll "29/30 wie die Engine-Tabelle"                  original rc=2                                             E24 rc=0,
    Grund: "APK-ASSET-GATE-QUELLBAUM-OK: 3629 Dateien in 2 Baeumen (Summe der Baumzeilen), 2 Tuer-Soll-Zeilen g/g"
```

### 3.3 Eigene Aenderungen am bash-Urteil `release/apk_pruefen.sh` und Ende-zu-Ende (F/G-Reihe, echte Kette)
Fuer jede Variante wird `release/` kopiert und genau EINE Aenderung eingebaut. re15_port und synchro haengen als Junction
im Scratchpad (nur gelesen, entfernt mit `cmd /c rmdir`). Dann laeuft die ECHTE `test_r35_android_pruefkette.sh`
des Baums. Die G-Varianten ersetzen das Urteil durch eine E-Variante und pinnen neu, wie es ein Entwickler tun wuerde.
Der Bau-Agent hat je Pruefzeile GESTRICHEN (B1-B17). Hier wird je Pruefzeile EINSEITIG gelockert.

| # | Aenderung | Kette |
|---|---|---|
| F0 | keine (Kontrolle der Kopie) | rc 0, FALSCH=0 (Kontrolle gruen) |
| **F1** | apk_pruefen.sh:352 zweite Instanz `if (( rc != 0 ))` -> `if (( rc == 1 ))` (Gate-Rueckgabe 2 wird nicht mehr ueberstimmt) | **NICHT** (rc 0, FALSCH=0) |
| F2 | :352 `rc != 0` -> `rc == 2` | BEMERKT (N8) |
| **F3** | :296 `(( f1 == f2 && ...` -> `(( f1 >= f2 && ...` | **NICHT** |
| **F4** | :298 `(( e + g == m && ...` -> `(( e + g >= m && ...` | **NICHT** |
| F5 | :295 `(( rc == 0 )) \|\| die` -> `(( rc <= 1 )) \|\| die` | BEMERKT (N10) |
| **F6** | :295 `(( rc == 0 )) \|\| die` -> `(( rc != 1 )) \|\| die` (Selbsttest-Rueckgabe 2 mit OK-Zeile angenommen) | **NICHT** |
| F7 | :345 `\|\| rc=$?` -> `\|\| true` (Gate-Rueckgabe verschluckt) | BEMERKT (N8) |
| **F8** | :290 `Mutanten\ erkannt,\ ([0-9]+)\ als` -> `([0-9]*)` | **NICHT** |
| F9 | :288 `tail -1` -> `tail -2 \| head -1` | BEMERKT (P0) |
| F10 | :355 Urteilszeile in `"$log"` statt `"$log.urteil"` gesucht | BEMERKT (P1/P2) |
| F11 | :355 `"^   Gate-Urteil ($modus, Rueckgabe 0): "` -> `"^   Gate-Urteil ("` | BEMERKT (N19b) |
| F12 | :296 `f2 >= MIN` -> `f2 >= MIN - 1` | BEMERKT (N12) |
| F13 | :298 `g <= MAX` -> `g <= MAX + 1` | BEMERKT (N15) |
| **G1** | Urteil = E24 (quellbaum ohne tuer_soll()), neu gepinnt | **NICHT**: P0 `Gate-Urteil selbstgeprueft: 243/243 Faelle, 781/785 ...`, P1-P3 ok, N1-N21 ok, `FEHLER=0` |
| **G2** | Urteil = E4 (tuer_soll `!=` -> `<`), neu gepinnt | **NICHT**: `FEHLER=0` |

Bilanz: 15 Aenderungen, 8 bemerkt, 7 nicht. F1 und F6 sind realistische Lockerungen; F3, F4 und F8 sind unwahrscheinlicher,
aber nicht gleichwertig, denn das bash-Urteil soll dem Python-Urteil gerade nicht trauen. G1 und G2 zeigen: Eine
kuenftige Aenderung, die eine Urteilsregel lockert oder ganz streicht, kommt mit neuem Pin durch den Urteils-Selbsttest
UND durch die ganze ctest-Kette.

### 3.4 Ursachen (Code gelesen)
- `_Mutierer.TAUSCH` (gate_urteil.py:677) bildet `NotEq` nur auf `Eq` ab, nicht auf `Lt`/`Gt`. Einseitige Lockerungen
  sind deshalb kein Mutant. Die Fallsammlung prueft jede dieser Ungleichungen nur von EINER Seite:
  - "qb: Summe der Baumzeilen 11" hat nur Summe < n.
  - "pk: Quelle != gleich" und "pk: Quelle 11 != gleich 10" haben beide qq > gg.
  - "st: 6/7 Faelle" hat nur ok_n < n.
  - "apk: Tuer-Soll 29/30" hat nur g1 < g2.
  - "st: Innere Proben 3/2" verletzt zugleich die Mindestzahl (b=2 < 3) und isoliert a > b deshalb nicht.
  - "apk: SUMME Bytes 4095" hat nur mb > b.
  - "Baumzeile fehlt" hat nur len < k.
- Die Tuer-Soll-Regel wird nur INNERHALB des Meldungstexts von `ende(0, ...)` aufgerufen (gate_urteil.py:148 apk,
  :158 quellbaum, :168 paket). Meldungstexte mutiert der Mutator absichtlich nicht (:702-706). Die Faelle "qb: Tuer-Soll
  fehlt" (:510) und "apk: Tuer-Soll fehlt" (:499) filtern `"Tuer-Soll" not in z`. Damit verschwindet auch die
  Schlusszeile (`... Tuer-Soll erfuellt ==`), und der Fall endet an `letzte Zeile ist nicht '== ...-OK: ... =='`. Die
  Regel wird nie erreicht (nachgemessen: Grund `letzte Zeile ist nicht '== APK-ASSET-GATE-QUELLBAUM-OK: ... ==',
  sondern '   synchro   2 Dateie...`). Im Modus quellbaum gibt es keinen Fall, der eine Tuer-Soll-Abweichung mit erhaltener
  Schlusszeile zeigt. Nur paket ("pk: Tuer-Soll fehlt", greift wirklich) und apk (29/30, 0/0) sind geschuetzt.
- bash: Jede Kontrolle N8/N10/N11/N13 prueft ihre Bedingung von einer Seite (N8 nur Gate-Rueckgabe 1, N10 nur
  Selbsttest-Rueckgabe 1, N11 nur f1 < f2, N13 nur e + g < m). Eine Kontrolle mit fehlender Zahl gibt es nicht.

**Urteil Punkt 3: teilweise.** Das Mittesten hat sich gegenueber Abnahme 0 deutlich verbessert: Muster-Lockerungen,
gestrichene Pruefzeilen und die Pruefung `rc == 0` werden jetzt erkannt. "Sorgfaeltig" ist es noch nicht. Eine
verbreitete Art von Ein-Stellen-Aenderung bleibt in beiden Schichten unbemerkt, die einseitige Lockerung eines
Vergleichs. Eine ganze Regel (Tuer-Soll im Modus quellbaum) laesst sich ohne Signal entfernen. Ende zu Ende
(G1/G2) haelt die Kette eine gelockerte, neu gepinnte Urteilslogik nicht auf. Die Doku beschreibt den Umfang korrekt
(Operatorliste, "nicht bewiesen fuer jede denkbare Aenderung"). Der Mangel liegt im Mechanismus, nicht in einer
Ueberbehauptung.

## 4. RE-Gate

- Die Spur ist reine Port-Infrastruktur (Android-Entpacker, Release-Pruefkette). Die PSX hat weder Entpacker noch APK,
  es gibt also kein Original zum Disassemblieren. Im Diff ohne analysis/ stehen 0 `@0x`-Zitate und 4 `PORT-WAHL`-Kennzeichnungen.
  Die neuen Zahlen der Nachbesserung (243/781/4) sind als gemessen gekennzeichnet (apk_pruefen.sh:98-106, Dossier-Tabelle).
  Statt einer Disassembly wurden 3 zitierte Quellstellen selbst nachgelesen: apk_asset_gate.py:777, :3441 und :1395-1397.
  Alle drei enthalten das Behauptete (siehe 3.1).
- Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/vorerst/Platzhalter) in den
  `+`-Zeilen ohne analysis/: **0 Treffer**. Keine neuen `getenv`-Schalter in engine/platform/include.
- Erklaert der Fix den Befund? Punkt 1: ja (vorher x < 0 an denselben Stellen reproduziert). Punkt 2: ja (vorher
  `rename`-Fehler -> ABBRUCH, Heilung erst im Start 3, in 5/5 eigenen Szenarien reproduziert). Nachbesserung 1: Die
  Ursachen M1/M2 der Abnahme 0 (visit_Constant nur int, fehlende N10) sind im Code behoben und gemessen.

**Gate: haelt.**

## 5. Vertrag / Pfade / Tests / Suite

- Echte Aenderungen der Spur = `git diff master...HEAD --name-only` (Basis 154a73c1): analysis/befunde_runde35/N_*,
  re15_port/platform/android/* (README, build.gradle, android_glue.c, asset_abgleich.c/.h), release/apk_asset_gate.py +
  .sha256, release/apk_pruefen.sh, release/gate_urteil.py + .sha256, re15_port/tests/unit/probes/r35_android.cmake,
  tests/unit/r35_android/*, tests/unit/test_r35_android_abgleich.c und 1 Zeile in tests/unit/test_r34a_asset_abgleich.c
  (Android-Test der Runde 34a, eigenes Gebiet).
  **Nicht** beruehrt: release/RELEASE_NOTES.md, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt,
  engine/src, include. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots oder Ereignisse (keine Spiellogik).
  `git diff master --stat` zeigt mehr Dateien, weil master weitergelaufen ist (v0.8.22). `git merge-tree --write-tree
  master HEAD` laeuft ohne Konflikt durch.
  Hinweis fuer den Merge: master hat RE15_MIN_TESTS inzwischen auf 517 (6e1a3771), diese Spur bringt +4.
- Tests: `ctest -R "r35_android|r34a_asset"` -> **5/5 Passed** (abgleich 0,02 s, anzeige 3,6 s, konflikt 2,1 s,
  pruefkette 79 s, r34a 1,0 s). anzeige und konflikt werden mit dem alten Code ROT (Abnahme 0, hier zusaetzlich mit
  eigenen Szenarien gemessen). Die Pruefkette misst, was sie verspricht, faengt aber die Klassen aus 3.2/3.3 nicht (M1-M3).
- Suite: Das Dossier enthaelt woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482` (>= 478; Abschnitt "Nachbesserung 1 -
  Abschluss"). `ctest -N` = 482. Nach Abnahme 0 (Suite 482/482 selbst gefahren) wurden nur release/,
  test_r35_android_pruefkette.sh, README und ein Kommentar in probes/r35_android.cmake geaendert. Den einzigen davon
  betroffenen Test (pruefkette) habe ich selbst gefahren. Fenster-Haken waren nicht betroffen.

**Gates Pfad/Tests/Suite: halten.**

## 6. Maengel (nummeriert, nachpruefbar)

**M1 (Punkt 3, Python-Urteil): einseitige Lockerungen von `!=` bleiben unbemerkt.** In `release/gate_urteil.py`
laufen diese Einzelaenderungen, jeweils in einer Kopie, mit `--selbsttest` -> `== URTEIL-SELBSTTEST-OK: 243/243 Faelle,
781/785 Mutanten erkannt, 4 als gleichwertig begruendet ==` (rc 0) durch:
- :83 `int(m.group(1)) != int(m.group(2))` -> `<`
- :112 `ok_n != n` -> `ok_n < n`
- :116 `a != b` -> `a < b`
- :136 `mb != b` -> `mb > b`
- :155 `len(baum) != k` -> `<` und `sum(baum) != n` -> `<`
- :165 `len(baum) != k` -> `<`, `qq != gg` -> `qq > gg` und `sum(gg for _qq, gg in baum) != n` -> `<`

Die geaenderten Urteile nehmen an, was das Original mit 2 ablehnt (gemessen: Baumzeilen 3622+2+5 bzw. 3627+5 bei "3629
Dateien in 2 Baeumen", Tuer-Soll "31/30"). E4 neu gepinnt (G2) besteht die ganze ctest-Kette mit `FEHLER=0`.
Ursache siehe 3.4: TAUSCH (:677) `NotEq -> Eq` allein, und die Faelle pruefen nur eine Seite.
Abhilfe-Richtung: TAUSCH um `!=` -> `<`/`>` (und `==` -> `<=`/`>=`) erweitern, je Stelle ein Fall von der anderen
Seite (Summe > n, len > k, qq < gg, ok_n > n, g1 > g2, a > b mit b >= Mindestzahl, mb < b), danach die Mindestzahlen
nachziehen. Messweg: eine Textaenderung wie oben -> `--selbsttest` muss `URTEIL-SELBSTTEST-FEHLER` melden.

**M2 (Punkt 3, Python-Urteil): Die Tuer-Soll-Regel im Modus quellbaum laesst sich ungestraft entfernen.**
`gate_urteil.py:157-158`: `% (n, k, tuer_soll()))` -> `% (n, k, 2))`.
- `--selbsttest` meldet weiter OK (243/243, 781/785).
- Das Urteil sagt 0 zu einer Quellbaum-Ausgabe mit `Tuer-Soll: ... 29/30 wie die Engine-Tabelle`, das Original sagt 2.
  Im Grund behauptet es sogar "2 Tuer-Soll-Zeilen g/g".
- Neu gepinnt (G1) besteht es die ganze Kette, `FEHLER=0`.

Ursache: `tuer_soll()` steht nur im Meldungstext von `ende(0, ...)`. Den mutiert der Mutator absichtlich nicht (:702-706).
Die Faelle "qb: Tuer-Soll fehlt" (:510) und "apk: Tuer-Soll fehlt" (:499) loeschen mit dem Filter `"Tuer-Soll" not in z`
auch die Schlusszeile und erreichen die Regel nie.

Abhilfe-Richtung: Die Tuer-Soll-Pruefung als eigene Anweisung vor `ende(0, ...)` setzen (dann gibt es den Mutanten
"Aufruf weg"). Den Filter der beiden Faelle auf Zeilen beschraenken, die mit `   Tuer-Soll:` beginnen. Je einen
quellbaum-Fall "Tuer-Soll 29/30" und "0/0" ergaenzen. Messweg: die Ersetzung oben -> `--selbsttest` muss scheitern.

**M3 (Punkt 3, bash-Urteil): einseitige Lockerungen in `release/apk_pruefen.sh` bleiben unbemerkt.** Mit genau EINER
dieser Aenderungen in einer Kopie von release/ endet die echte `test_r35_android_pruefkette.sh` mit rc 0, FALSCH=0
(Kontrolle F0 gruen):
- (F1) :352 `if (( rc != 0 ))` -> `if (( rc == 1 ))`
- (F6) :295 `(( rc == 0 )) || die` -> `(( rc != 1 )) || die`
- (F3) :296 `f1 == f2` -> `f1 >= f2`
- (F4) :298 `e + g == m` -> `e + g >= m`
- (F8) :290 `([0-9]+)\ als\ gleichwertig` -> `([0-9]*)\ als\ gleichwertig`

Ursache: N8 kennt nur die Gate-Rueckgabe 1, N10 nur die Selbsttest-Rueckgabe 1, N11 nur f1 < f2, N13 nur e + g < m,
und keine Kontrolle hat eine leere Zahl. Abhilfe-Richtung: Gegenseiten-Kontrollen ergaenzen (N8b: luegendes Urteil bei
Gate-Rueckgabe 2; N10b: OK-Zeile mit Rueckgabe 2; N11b: f1 > f2; N13b: e + g > m; N22: Schlusszeile mit leerer Zahl).
Messweg: mein Variantenlauf (F1/F3/F4/F6/F8) muss danach BEMERKT melden.

**Hinweise ohne Mangelcharakter**
- H1: Der Docstring von `_Mutierer` (:675) nennt "die Lockerungen R0-R5", der Docstring von `_regex_lockerungen`
  (:607-614) fuehrt R6 nicht auf. Der Code wendet R6 an (:638-641, `Regex R6 11` in der Ausgabe). Kopf (:23-25) und
  README ("sieben Lockerungen") sind korrekt. Das ist eine Untertreibung, keine Ueberbehauptung, sollte aber beim
  naechsten Commit mit angeglichen werden.
- H2: `unit_r35_android_pruefkette` wird nur registriert, wenn bash + Python >= 3.8 gefunden werden (wie Abnahme 0).
- H3: Ein echtes Update APK A -> B mit dem H8-Muster auf dem Emulator steht weiter aus (OFFEN im Dossier).

## 7. Belege dieser Abnahme (Scratchpad der Abnahme-Sitzung, nicht im Repo)
- Punkt 1/2: `n1_mess_entpacker.py`, `n1_anzeige.txt` / `n1_anzeige_alt.txt` (45 + 45 Laeufe), `n1_konflikt.txt` /
  `n1_konflikt_alt.txt` (5 + 5 Szenarien), `n1_alt/pruefstand_alt.exe`.
- Punkt 3 Python: `n1_eigene_aenderungen.py`, `n1_eigene_aenderungen.txt` (E-Reihe), `n1_qb_E1/E4/E5.txt` und
  `n1_qb_tuer_29_30.txt` (Ausgaben, die die Varianten faelschlich annehmen), `n1_selbsttest.txt`.
- Punkt 3 Kette: `n1_kette_varianten.py`/`.sh`, `n1_kette_varianten.txt`, `n1_kv/<id>/kette.log` (F0-F13, G1, G2).
- Alle entscheidenden Aenderungen stehen oben woertlich mit Zeilennummer und sind damit ohne diese Dateien wiederholbar.
