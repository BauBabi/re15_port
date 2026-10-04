# Runde 35 Spur N "android" — Abnahme 2 (unabhaengig, nach Nachbesserung 2)

Datum 2026-10-04. Baum `.claude/worktrees/r35_android`, Zweig r35/android, HEAD 009afe17, Basis 154a73c1.
Massstab: Wortlaut AUFTRAG.md Z. 100-102. Gemessen am selbst gebauten Stand. Vom Bau-Agenten ist nur uebernommen,
was hier nachgemessen ist. Die eigenen Messungen sind bewusst ANDERE als die des Bau-Agenten und der Abnahmen 0/1:
andere Displaygroessen, ein bisher nicht gemessener Fehlertext, andere Konflikt-Szenarien und andere simulierte
Aenderungen am Urteil.

**Ergebnis: NICHT bestanden.** Punkt 1 erfuellt, Punkt 2 erfuellt, Punkt 3 teilweise.

Alle Maengel der Abnahme 1 sind behoben und nachgemessen:
- E-Reihe 29/29 bemerkt, darunter E11.
- F1/F3/F4/F6/F8 werden jetzt von genau der Gegenseiten-Kontrolle rot.
- Die Tuer-Soll-Regel ist eine eigene Anweisung, und die Strukturregel meldet die alte Form 3x.
- Die Zahlen des Bau-Agenten stimmen: 280/280, 887/894, 7, 87 Kontrollen, 220/217/3, ctest 6/6.

Dazu wirkt der neue Zwang in beiden Schichten: Eine NEUE Regel ohne Fall bzw. Kontrolle macht den Selbsttest bzw. den
bash-Mutantenlauf rot.

Es bleibt eine Klasse realistischer Ein-Zeilen-Aenderungen, die eine ganze Schutzregel des Urteils entfernt, ohne dass
irgendetwas rot wird, weder der Urteils-Selbsttest noch die Kontrollen noch die echte ctest-Kette:
- Die Uebergabe der Mindestzahlen des Gate-Selbsttests (261/148) und der unzip-Zaehlung an das Urteil.
- Die Regel "genau n Fallzeilen".
- Die Regel "je Baum Quelle = gleich" im Modus paket.

Die Gates (Suite, @0x/PORT-WAHL, Pfade, Tests) halten. Die Maengel M1-M3 stehen unten.

---

## 0. Bau und Werkzeug

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`. `... build` -> "ninja: no work to
  do", `=== LOCAL-BUILD-OK (build)`. `ctest -N` -> `Total Tests: 483` (482 + unit_r35_android_bash_mutanten).
- Seit Abnahme 1 (9e535728) ist kein C-Code geaendert: `git diff 9e535728..HEAD --stat -- re15_port/platform
  re15_port/engine re15_port/include ...` zeigt nur `platform/android/README.md`. Die Nachbesserung 2 aendert
  release/gate_urteil.py (+ .sha256), release/apk_pruefen.sh, tests/unit/r35_android/* und probes/r35_android.cmake.
- Python: `release/python_finden.sh` -> `/c/Python310/python` (3.10.11). Gegenproben liefen mit 3.9.0 und 3.14.7
  (msys64). bash fuer alle Laeufe = `C:/Program Files/Git/bin/bash.exe`, wie `R35_ANDROID_BASH` in der CMakeCache.
- Vorher-Stand fuer Punkt 1/2: eigener Pruefstand, gebaut aus `git show 154a73c1:` von android_glue.c,
  asset_abgleich.c und .h, dazu `pruefstand_main.c` von HEAD. Flags wie build.ninja: `gcc -std=gnu11 -include
  kompat_win.h -I.../stub -I<alt/jni>`. Ergebnis: `pruefstand_alt.exe`. Nachher-Stand:
  `re15_port/build/tests/unit/r35_android_pruefstand.exe`.
- Eigene Werkzeuge liegen im Scratchpad dieser Sitzung, nicht im Repo: `a2_entpacker.py` (Punkt 1/2),
  `a2_varianten.py` (H-, J- und N-Reihe), `a2_kette.py`/`.sh` (K- und G-Reihe an der echten Kette). Jede
  entscheidende Aenderung steht unten woertlich mit Zeilennummer (HEAD) und laesst sich ohne diese Dateien wiederholen.

## 1. Punkt 1 — "Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten"

### Messung (12 eigene Displaygroessen x 4 Texte, vorher und nachher, am echten android_glue.c)
- Groessen, keine davon in den Listen des Bau-Agenten oder der Abnahmen 0/1: 2160x1080, 2280x1080, 3120x1440,
  2000x800, 3840x720, 2560x600, 8192x1080, **20480x1080**, 1024x600, 2732x2048, 828x1792 (hochkant), 1366x768.
- Texte:
  - (e) Erstinstallation `RE1.5 PORT - ASSETS WERDEN ENTPACKT` mit Fortschrittszeile (bisher von keiner Abnahme
    gemessen).
  - (u) Uebergang `... ASSETS WERDEN EINMALIG GEPRUEFT`.
  - (d) `2 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG`.
  - (k) **`FEHLER: ASSET-LISTE DER APK UNGUELTIG`** (bisher nicht gemessen; Liste mit falscher Kopf-Anzahl
    `# re15 assets v2 2 4` bei 1 Zeile).
- Ausgewertet: `PRUEFSTAND-AUSSERHALB`/`-UEBERLAPPUNG` je Lauf, x-Bereich aller `PRUEFSTAND-ZEILE`, und der ganze Text
  im `PRUEFSTAND-BILD`.

| Stand | Laeufe | mit Text/Rechteck ausserhalb oder Ueberlappung |
|---|---|---|
| vorher (154a73c1) | 48 | **21** |
| nachher (HEAD) | 48 | **0**, ganzer Text in allen 48 sichtbar |

Auszug:
```
vorher  2160x1080 u x=-240..2390   nachher 2160x1080 u x=156..1997
vorher  2160x1080 d x=-459..2610   nachher 2160x1080 d x=54..2100
vorher  828x1792  k x=-1251..2064  nachher 828x1792  k x=72..750
vorher  828x1792  e x=-1266..2078  nachher 828x1792  e x=90..734
nachher 20480x1080 d x=8701..11770 (hoehenbegrenzt, mittig; vorher identisch, passte schon)
```
ctest `unit_r35_android_anzeige` selbst gefahren: Passed (3,62 s).

**Urteil Punkt 1: erfuellt.** Bei keiner gemessenen Breite oder Hoehe verlaesst ein Text das Bild, auch nicht beim
bisher ungemessenen Text (k) und beim Titel der Erstinstallation. Die Ursache (Breite nie gegen W geprueft) ist im
alten Code reproduziert: x < 0 an denselben Stellen.

## 2. Punkt 2 — "Ein Update, bei dem eine Datei und ein Ordner gleichen Namens kollidieren, bricht sauber ab; erst der zweite Start heilt es"

### Messung (5 eigene Szenarien; Start 1 = APK A frisch, Start 2 = APK B als Update, Start 3 = B noch einmal)
Nach jedem Start wird der Baum verglichen: Dateien und Inhalte = APK, keine zusaetzliche Datei, kein Ordner ohne
gelistete Datei, `re15_assets_entpackt.txt` = Liste.

| Szenario (eigene) | vorher Start 2 | vorher Start 3 | nachher Start 2 | nachher Start 3 |
|---|---|---|---|---|
| V1 Ordner mit 2 Ebenen und 2 Dateien `PSX/ST/sub/{a,b}.bin` wird Datei `PSX/ST` | rc 1, `FEHLER beim Entpacken: rename`, ABBRUCH | rc 0, voller Lauf | **rc 0, SPIELSTART, Baum = B** | schneller Weg |
| V2 Datei `synchro/S1/x.wav` wird Ordner `.../x.wav/inner.bin`, zugleich aendert `PSX/a.bin` den Inhalt | rc 0 | schneller Weg | rc 0, Baum = B | schneller Weg |
| V3 Datei `PSX/x` wird Ordner mit zwei neuen Ebenen `PSX/x/y/z.bin` + `PSX/x/y2.bin` | rc 0 | schneller Weg | rc 0, Baum = B | schneller Weg |
| V4 Ordner `PSX/q/` mit gelisteter Datei UND leerem Unterbaum `q/leer/tief/` wird Datei `PSX/q` | rc 1, rename, ABBRUCH | rc 0 | **rc 0**, `Konflikt geraeumt: shared_assets/PSX/q war ein Ordner, wo die Datei hin muss mit 0 Dateien darin - entfernt` | schneller Weg |
| V5 verwaister Ordner `synchro/S1/M.WAV.NEU/` (GROSS, mit Inhalt), `m.wav` aendert sich | rc 1, ABBRUCH | rc 0 | **rc 0**, `Konflikt geraeumt: synchro/S1/m.wav.neu war ein Ordner auf dem Namen der Zwischendatei mit 1 Dateien darin - entfernt` | schneller Weg |

Vorher zeigen V1, V4 und V5 genau das Bild des Nutzers: Start 2 bricht ab, erst Start 3 stellt den Stand her. V2/V3
(Datei wird Ordner) liefen schon vorher, wie K2/H7 im Dossier. Nachher werden alle 5 im selben Start fertig, und
Start 3 nimmt den schnellen Weg (`Assets aktuell (schneller Weg)`).
ctest `unit_r35_android_konflikt` Passed (2,00 s), `unit_r35_android_abgleich` Passed (0,02 s), `unit_r34a_asset_abgleich`
Passed (1,06 s).
Nicht gemessen (wie bisher OFFEN im Dossier, kein Mangel): ein echtes Update APK A -> APK B auf dem Emulator.

**Urteil Punkt 2: erfuellt.**

## 3. Punkt 3 — "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten"

### 3.1 Behauptungen der Nachbesserung 2, nachgemessen
- Pins:
  - `sha256sum release/gate_urteil.py` = `107de123...07a2e` = `release/gate_urteil.sha256`.
  - `apk_asset_gate.py` = `f73c3b1a...f9b6` = Pin, unveraendert.
  - `GATE_URTEIL_MIN_FAELLE/ERKANNT/MAX_GLEICH` = 280/887/7 (apk_pruefen.sh:111-113).
- `"$PY" release/gate_urteil.py --selbsttest` (6,1 s):
  - `Mutanten je Operator: Aufruf 31, BoolOp 10, Regex R0 13, Regex R1 261, Regex R2 49, Regex R3 30, Regex R4 9,
    Regex R5 115, Regex R6 11, Vergleich 145, Zahl 118, Zeichenkette 37, if-Bedingung 56, not 9`.
  - `== URTEIL-SELBSTTEST-OK: 280/280 Faelle, 887/894 Mutanten erkannt, 7 als gleichwertig begruendet ==`.
  - Dasselbe unter Python 3.9.0 und 3.14.7.
- Die 7 AEQUIVALENT-Eintraege gegen den Code gelesen (gate_urteil.py:96-107, :90, :174), alle stimmen:
  - `int(m.group(2)) == 0` -> `<= 0` und `qq == 0` -> `<= 0`: Beide Werte kommen aus `(\d+)`, also nie negativ.
  - `len(urteile) != 1` -> `> 1`: Die von `m_ok` erkannte letzte Zeile `== <MARKE>-OK: ... ==` passt fuer jede der vier
    MARKEN auf das Muster `== (SELBSTTEST|APK-ASSET-GATE(-PAKET|-QUELLBAUM)?)-(OK|...)\b`, also ist len >= 1.
  - Die drei bash-Eintraege (bash_urteil_mutanten.py:51-61): `rc` und `urteil` stammen nur aus `|| rc=$?` bzw.
    `|| urteil=$?` oder aus den Zuweisungen 0/2 (apk_pruefen.sh:288, :293, :346, :354, :356, :363, :366), sind also
    nie negativ.
- Strukturregel `_meldung_regel` selbst gefahren:
  - Am Urteil @9e535728 -> `['Zeile 148: tuer_soll()', 'Zeile 158: tuer_soll()', 'Zeile 168: tuer_soll()']`.
  - An HEAD -> `[]`.
- ctest `-R "r35_android|r34a_asset"` selbst gefahren: **6/6 Passed**.
  - pruefkette 125,3 s, `FEHLER=0`.
  - bash_mutanten 213,6 s: `Kontrolle (unveraendert): Rueckgabe 0 KONTROLLEN: 87 ok, 0 FALSCH`, `Mutanten je
    Operator: A 34, B 18, C 34, D 93, E 23, F 9, G 7, H 2`, `== BASH-URTEIL-MUTANTEN-OK: 220 Mutanten, 217 erkannt,
    3 als gleichwertig begruendet ==`.

### 3.2 Die Maengel der Abnahme 1, nachgemessen
- **M1/M2 (Python):** Das Werkzeug `werkzeug/nb2_urteil_aenderungen.py` (die E-Reihe der Abnahme 1 woertlich) lief
  gegen HEAD: K0 `KONTROLLE-OK`, **`SUMME: 29 von 29 Aenderungen bemerkt`**. Darunter:
  - E1, E2, E3, E4, E5, E6, E7, E9 und E29, je 1-3 Faelle falsch.
  - E11 (4 Faelle Rueckgabe -1).
  - E24 in Form 2, die Anweisung `tuer_soll()` gestrichen (4 Faelle).

  M1 und M2 sind behoben.
- **M3 (bash):** F1/F3/F4/F6/F8 der Abnahme 1 liefen woertlich, je in einer Kopie, gegen `urteil_kontrollen.sh pruefen`
  (ohne `--schnell`). Alle fuenf sind rot:
  - F1 -> `N8b_urteil_luegt_gate_2` + `N8m`.
  - F3 -> `N11b_f1_groesser_f2`.
  - F4 -> `N13b_summe_groesser_alle`.
  - F6 -> `N10b_ok_zeile_rueckgabe_2`.
  - F8 -> `N22_zahl5_leer`.

  M3 ist behoben. Die Belege des Bau-Agenten zur echten Kette (15/15) sind gelesen und decken sich damit.

### 3.3 Eigene simulierte Aenderungen am Python-Urteil `release/gate_urteil.py` (H-Reihe)
Je Aenderung genau EINE Textersetzung in einer Kopie, dann `<kopie> --selbsttest`. BEMERKT heisst Rueckgabe != 0.
Keine dieser Aenderungen steht in den Listen des Bau-Agenten (A/C/D, E-Reihe) oder der Abnahmen 0/1. Kontrolle H0
(unveraenderte Kopie): OK.

| # | Zeile | Aenderung | Selbsttest |
|---|---|---|---|
| H1 | 163 | qb `... or 0 in baum` gestrichen | BEMERKT |
| H2 | 174 | pk `or qq == 0` gestrichen | BEMERKT |
| H3 | 163 | qb `n <= 0 or` gestrichen | BEMERKT |
| H4 | 142 | apk `or mb != b` gestrichen | BEMERKT |
| H5 | 147 | Tuerarchiv `... == t[3] > 0` -> ohne `> 0` | BEMERKT |
| H6 | 130 | `or f.group(6).strip()` gestrichen | BEMERKT (Fall 077) |
| H7 | 110 | `z.startswith("ABBRUCH")` -> `startswith("ABBRUCH:")` | BEMERKT (Fall 053) |
| H8 | 95 | `letzte = zeilen[-1]` -> letzte Zeile, die mit `== ` beginnt | BEMERKT |
| H9 | 118 | `n < min_faelle` -> `n < min_faelle - 1` | BEMERKT (Fall 065) |
| **H10** | 126 | `!= list(range(1, n + 1))` -> `!= list(range(1, len(faelle) + 1))` | **NICHT** (280/280, 887/894) |
| H11 | 80 | genau_eine `if len(t) != 1:` -> `if not t:` | BEMERKT (Fall 067) |
| H12 | 90 | tuer_soll `any(` -> `all(` | BEMERKT |
| H13 | 99 | `if rc == 1:` -> `if rc in (1, 2):` | BEMERKT (Faelle 023/026/029) |
| H14 | 162 | qb Baumzeile `re.fullmatch` -> `re.search` | BEMERKT |
| H15 | 118 | `or n < min_faelle` gestrichen | BEMERKT (Fall 065) |
| H16 | 142 | apk `n <= 0 or` gestrichen | BEMERKT |
| **H17** | 174 | pk `any(qq != gg or qq == 0 ...)` -> `any(qq == 0 ...) or sum(qq ...) != sum(gg ...)` (Quelle = gleich nur noch als Summe) | **NICHT** (280/280, 886/893) |
| **H18** | 173 | pk Baumzeile `r"   \S+\s+(\d+)\s+(\d+)"` -> `r"   \S+\s+(\d+)\s+(?:\S*\s+)?(\d+)"` | **NICHT** (280/280, 889/896) |
| H19 | 87/90 | tuer_soll wertet nur die ERSTE Tuer-Soll-Zeile (`t = t[:1]`) | BEMERKT |
| H20 | 150 | unzip-Gegenprobe nur noch, wenn unzip WENIGER zaehlt | BEMERKT |
| H21 | 126 | Fallnummern: `if len(faelle) != n:` statt genau 1..n | BEMERKT (Fall 073) |
| H22 | 163 | Toleranz `abs(sum(baum) - n) > 1` | BEMERKT |

Bilanz: 22 Aenderungen, 19 bemerkt, 3 nicht. Abnahme 1 hatte an ihrer Reihe 18/29.

Folge nachgewiesen. Die drei Varianten nehmen an, was das Original ablehnt. Gemessen mit `urteil_rufen` auf
selbstgebauten Gate-Ausgaben, Werkzeug `a2_folge.py`:
```
H10  selbsttest "5/5 Faelle", nur Fallzeilen 01..04     original -> 2 (4 Fallzeilen, Nummern nicht genau 1..5)
                                                         Variante -> 0 (SELBSTTEST-OK 5/5, jede Fallzeile [ok] ...)
H17  paket "PSX 9 10" + "synchro 3 2" (Summen 12/12)     original -> 2 (Baumzeilen (Quelle, gleich) [(9, 10), (3, 2)] ...)
                                                         Variante -> 0 (APK-ASSET-GATE-PAKET-OK ... je Baum Quelle = gleich ...)
H18  paket "PSX 10 9 10" (drei Zahlen)                   original -> 2 (Baumzeilen [(2, 2)] passen nicht ...)
                                                         Variante -> 0 (APK-ASSET-GATE-PAKET-OK ...)
```
H17 behauptet im OK-Grund sogar woertlich "je Baum Quelle = gleich", obwohl die Regel weg ist.

### 3.4 Eigene simulierte Aenderungen am bash-Urteil `release/apk_pruefen.sh` (J-Reihe)
Je Aenderung genau EINE Textersetzung in einer Kopie. Dann laeuft `urteil_kontrollen.sh pruefen <kopie> <attrappen>
<arbeit>`, also alle 87 Kontrollen ohne `--schnell`. Kontrolle J0 (unveraendert): `KONTROLLEN: 87 ok, 0 FALSCH`.
Keine Aenderung steht in F1-F13 (Abnahme 1) oder B1-B17 (Nachbesserung 1).

| # | Zeile | Aenderung | Kontrollen |
|---|---|---|---|
| J1 | 290 | Zwischenspeicher fuer JEDE schon gepruefte Kopie: `[[ -n "$GATE_URTEIL_GEPRUEFT" ]] && return 0` | BEMERKT (D11b) |
| J2 | 293 | Selbsttest `\|\| rc=$?` -> `\|\| true` | BEMERKT (N10, N10b, D11b) |
| J3 | 305 | `(( f1 == f2 && f2 >= MIN ))` -> `(( f1 >= MIN && f2 >= MIN ))` | BEMERKT (N11b) |
| J4 | 307 | `e >= GATE_URTEIL_MIN_ERKANNT` -> `m >= ...` | BEMERKT (N14) |
| J5 | 356 | `\|\| urteil=$?` -> `\|\| urteil=2` | BEMERKT (P5) |
| J6 | 364 | Urteilszeile mit festem Modus `(selbsttest, Rueckgabe 0)` statt `($modus, ...)` | NICHT (87 ok), aber die Kette faengt es (K1 unten) |
| J7 | 370 | `return "$urteil"` -> `return "$rc"` | BEMERKT (N8c, N19a, N19b) |
| J8 | 364 | grep-Muster ohne Anker/Einrueckung/Doppelpunkt `"Gate-Urteil ($modus, Rueckgabe 0)"` | BEMERKT (N19d/e/f) |
| J9 | 258 | Gate-Pin: `[[ "$ist" == "${soll:0:16}"* ]]` (nur 16 Hexziffern) | NICHT (87 ok) |
| J10 | 276 | Urteils-Pin: dasselbe | NICHT (87 ok) |
| **J11** | 338 | gate_urteil uebergibt `0 0` statt `"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"` | **NICHT** (87 ok) |
| **J12** | 338 | Kopierfehler `"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_INNEN"` (min_faelle 148 statt 261) | **NICHT** (87 ok) |
| **J13** | 339 | `"${GATE_APK_EINTRAEGE:-}"` -> `""` (unzip-Zaehlung nicht mehr uebergeben) | **NICHT** (87 ok) |
| J14 | 354 | Gate aus `${GATE_QUELLE:-$gate}` statt der gepinnten Kopie | BEMERKT (P5, P5b, N8c, N8b) |
| J15 | 338 | Urteil aus `$GATE_URTEIL_QUELLE` statt der geprueften Kopie | BEMERKT (P4, P7, N8c) |
| J16 | 297 | `letzte` = letzte Zeile mit `URTEIL-SELBSTTEST-` (grep) statt der letzten Zeile | BEMERKT (N16) |

Messhinweis zu J14/J15: Im ersten Lauf lag die Variante allein in einem Arbeitsordner, und beide wurden nur ueber
`can't open file` rot (GATE_QUELLE zeigt neben die Kopie). Mit den release-Dateien daneben sind beide sauber durch
Kontrollen rot (Zeilen oben). Dieselbe Eigenart hat `bash_urteil_mutanten.py` (Mutanten liegen in `arbeit/m%03d/` ohne
Gate/Urteil daneben), siehe Hinweis H3.

Folge J11/J12/J13 nachgewiesen, mit dem echten Urteil und selbstgebauten Gate-Ausgaben:
```
Gate-Selbsttest meldet 200/200 Faelle, 148/148 Proben (Mindestzahl 261):
  gate_urteil.py selbsttest <datei> 0 261 148  -> rc=2 "SELBSTTEST-OK meldet 200/200 Faelle, verlangt n/n mit n >= 261 ..."
  J11 (0 0)                                     -> rc=0 "SELBSTTEST-OK 200/200 ... (Mindestzahlen 0/0)"
  J12 (148 148)                                 -> rc=0 "SELBSTTEST-OK 200/200 ... (Mindestzahlen 148/148)"
apk: Gate meldet 12 Dateien, unzip zaehlt 14 Eintraege unter assets/:
  gate_urteil.py apk <datei> 0 261 148 14       -> rc=2 "unzip zaehlt 14 Eintraege unter assets/, das Gate 12 Asset-Dateien + Manifest"
  J13 (apk_eintraege leer)                      -> rc=0 "APK-ASSET-GATE-OK: 12 Dateien ..."
```
J11/J12 lassen also einen geschwaechten Gate-Selbsttest durch. Genau davor soll `GATE_SELBSTTEST_MIN_FAELLE=261`
schuetzen (Kopf apk_pruefen.sh: "hier senken (git-Diff), nicht still"); hier wird die Mindestzahl still ausgehebelt.
J13 schaltet die unzip-Gegenprobe still ab.

Bilanz: 16 Aenderungen, 10 von den Kontrollen bemerkt. J6 faengt die Kette. J9/J10 sind praktisch harmlos: Ein
veraendertes Gate muesste 64 Bit Praefix treffen (Hinweis H1). **J11, J12 und J13 bleiben unbemerkt.**

### 3.5 Ende zu Ende: eigene Varianten an der ECHTEN Kette (K-/G-Reihe)
Je Variante eine Kopie der release-Dateien mit GENAU EINER Aenderung. re15_port und synchro haengen als Junction im
Scratchpad (entfernt mit `cmd /c rmdir`; danach liegt in jedem repo-Ordner nur `release/`). Dann laeuft die echte
`test_r35_android_pruefkette.sh` des Baums. Die G-Varianten ersetzen das Urteil durch eine H-Variante und pinnen neu.

| # | Aenderung | Kette |
|---|---|---|
| K0 | keine (Kontrolle) | `KONTROLLE-OK rc=0 FALSCH=0` |
| K1 | J6 | BEMERKT: `P2_quellbaum -> 2 (soll 0) Gate-Urteil ohne Urteilszeile '   Gate-Urteil (quellbaum, ...` |
| **K2** | J11 | **NICHT**: P0 ok, `FEHLER=0` |
| **K3** | J12 | **NICHT**: `FEHLER=0` |
| **K4** | J13 | **NICHT**: `FEHLER=0` |
| K5 | J9 | NICHT: `FEHLER=0` (Hinweis H1) |
| **G3** | Urteil = H10, neu gepinnt | **NICHT**: P0 `Gate-Urteil selbstgeprueft: 280/280 Faelle, 887/894 ...`, `FEHLER=0` |
| G4 | Urteil = H17, neu gepinnt | BEMERKT, aber nur ueber die Mindestzahl: P0 bricht ab, weil H17 eine Mutationsstelle weniger hat (886/893 < 887) |
| **G4b** | wie G4, dazu `GATE_URTEIL_MIN_ERKANNT=886`. Das ist genau der Schritt, den Skript und README verlangen ("ggf. GATE_URTEIL_MIN_* anpassen - im selben Commit") | **NICHT**: P0 `280/280 Faelle, 886/893 ... (Mindestzahlen 280/886/<=7)`, `KONTROLLEN: 87 ok, 0 FALSCH`, `FEHLER=0` |

### 3.6 Zwang fuer NEUE Regeln (gemessen, wirkt)
- Python N1: In quellbaum kommt neu `if len(zeilen) > 100000: ende(2, "zu viele Zeilen")` dazu, ohne Fall.
  Ergebnis: `URTEIL-SELBSTTEST-FEHLER`, 6 Mutanten ueberleben (`if-Bedingung -> False`, `Aufruf weg`, `Zahl 100000 ->
  100001`, ...).
- Python N2: In apk kommt neu `if b <= 0: ende(2, "SUMME 0 Bytes")` dazu, ohne Fall. Ergebnis: SELBSTTEST-FEHLER, 6
  Mutanten ueberleben.
- bash: In gate_laufen kommt nach `cat "$log"` neu `[[ -s "$log" ]] || die "gate_laufen: Gate ohne jede Ausgabe"` dazu,
  ohne Kontrolle. Die Mutanten dieser Zeile (bash_urteil_mutanten.mutanten) liefen gegen urteil_kontrollen.sh:
  `B ! davor` ist erkannt, **`E die -> true` UEBERLEBT** -> unit_r35_android_bash_mutanten wuerde rot.

Wer eine neue Pruefung schreibt, wird also in beiden Schichten gezwungen, sie mitzutesten. Das ist der Kern des
Nutzerpunkts, und er ist gegenueber Abnahme 1 neu und wirksam.

### 3.7 Ursachen (Code gelesen)
- **Schnittstelle bash -> Python-Urteil ungeprueft (J11-J13).** `gate_urteil()` (apk_pruefen.sh:338-339) uebergibt
  `GATE_SELBSTTEST_MIN_FAELLE`, `GATE_SELBSTTEST_MIN_INNEN` und `GATE_APK_EINTRAEGE`. Keine Pruefung beruehrt diese
  Uebergabe:
  - Alle Urteils-Attrappen in urteil_kontrollen.sh entscheiden nur nach der Gate-Rueckgabe (`ehrlich`, :71) und lesen
    argv[4..6] nie.
  - Die Kette ruft `gate_laufen apk` ohne `GATE_APK_EINTRAEGE` (test_r35_android_pruefkette.sh:83, :93, :139).
  - Kein Lauf fuettert eine Gate-Ausgabe unter der Mindestzahl.
  - bash_urteil_mutanten.py hat keinen Operator fuer Argumente bzw. Variablen. Das steht offen im Kopf ("in bash
    Umleitungen oder Variablennamen").

  Damit trifft der README-Satz "Wer dort eine Pruefzeile aendert oder neu schreibt, ergaenzt in urteil_kontrollen.sh
  eine Kontrolle, sonst ueberlebt ein Mutant" fuer diese Zeile nicht zu: Sie laesst sich aendern, ohne dass ein
  Mutant ueberlebt oder eine Kontrolle rot wird.
- **H10:** Die Regel "genau n Fallzeilen" haengt an `n` im Ausdruck `range(1, n + 1)` (gate_urteil.py:126). Die
  Fallsammlung kennt "Fallnummer 6 statt 5" (073) und "Fallnummern doppelt", aber keinen Fall mit FEHLENDER Fallzeile
  bei sonst lueckenlosen Nummern. Der Zahl-Mutant `n + 1 -> n + 2` wird anders erkannt.
- **H17/H18:** Im Modus paket pruefen die Faelle Quelle != gleich nur in EINEM Baum ("pk: Quelle != gleich", "pk/S:
  Quelle 9 < gleich 10"). Es gibt keinen Fall mit ausgleichenden Abweichungen ueber zwei Baeume (H17) und keine
  Baumzeile mit drei Zahlen (H18). Dass G4 nur ueber die Mindestzahl rot wird, ist Zufall der Mutantenzaehlung: Das
  Skript selbst weist an, die Mindestzahl dann nachzuziehen (G4b).

**Urteil Punkt 3: teilweise.** Der Mechanismus ist gegenueber Abnahme 1 deutlich staerker:
- Vergleiche werden in beide Richtungen mutiert.
- Jede Vergleichsstelle hat Faelle bzw. Kontrollen von beiden Seiten.
- Die Strukturregel verbietet Pruefungen im Meldungstext.
- Das bash-Urteil hat einen eigenen Mutantenlauf in ctest.
- Neue Regeln werden in beiden Schichten erzwungen.

"Sorgfaeltig" ist das Mittesten aber noch nicht vollstaendig. Drei Schutzregeln lassen sich mit je einer realistischen
Ein-Zeilen-Aenderung entfernen, und die Kette bleibt `FEHLER=0`:
- die Mindestzahlen des Gate-Selbsttests (ein Kopierfehler in einer Argumentliste genuegt, J12);
- die unzip-Gegenprobe (J13);
- "genau n Fallzeilen" (H10).

Dazu kommt "je Baum Quelle = gleich" (H17), die mit dem vom Skript selbst verlangten Nachziehen der Mindestzahl
ebenfalls durchkommt. Das ist dieselbe Mangelart wie Abnahme 1 M2 (ganze Regel ohne Signal entfernbar). Der
Bau-Agent beschreibt die Grenze ehrlich ("nicht fuer jede denkbare Aenderung bewiesen"). Die Schnittstelle
bash -> Python-Urteil ist aber nicht eine von vielen denkbaren Aenderungen, sondern ein ganzer ungetesteter
Baustein des Urteils.

## 4. RE-Gate

- Die Spur ist reine Port-Infrastruktur (Android-Entpacker, Release-Pruefkette). Die PSX hat weder Entpacker noch APK,
  also gibt es kein Original zum Disassemblieren. Im Diff 154a73c1..HEAD ohne analysis/ stehen **0** `@0x`-Zitate und
  **4** `PORT-WAHL`-Kennzeichnungen. Die neuen Zahlen 280/887/7 sind als gemessen gekennzeichnet
  (apk_pruefen.sh:102-113, Dossier). Die neuen AEQUIVALENT-Eintraege sind im Code einzeln begruendet und hier gegen den
  Code geprueft (3.1).
- Statt einer Disassembly habe ich 3 zitierte Quellstellen selbst nachgelesen. Alle enthalten das Behauptete:
  - apk_asset_gate.py:777 `print("   Tuer-Soll: %-15s %-34s %d/%d wie die Engine-Tabelle ...")`.
  - apk_asset_gate.py:3441 `"   [%s] %02d %-58s rc=%d (soll %d)%s"`.
  - gate_urteil.py:1002 `modus, log, rc = argv[1], argv[2], int(argv[3])` (Grundlage fuer "main() nimmt jede ganze
    Zahl", Faelle mit Rueckgabe -1).
- Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/vorerst/Platzhalter) in den
  `+`-Zeilen ohne analysis/: **0 Treffer**. Neue `getenv` in engine/platform/include: **0**.
- Erklaert der Fix den Befund?
  - Punkt 1: ja, vorher x < 0 an denselben Stellen (21/48 eigene Laeufe).
  - Punkt 2: ja, vorher ABBRUCH in Start 2 mit Heilung erst in Start 3 (V1/V4/V5).
  - Nachbesserung 2: Die Ursachen der Abnahme 1 (TAUSCH nur `!=` <-> `==`, einseitige Faelle, tuer_soll() im
    Meldungstext, einseitige bash-Kontrollen) sind im Code behoben, und die E-Reihe 29/29 sowie F1/F3/F4/F6/F8
    werden rot.

**Gate: haelt.**

## 5. Vertrag / Pfade / Tests / Suite

- Pfade = `git diff 154a73c1..HEAD --name-only`:
  - analysis/befunde_runde35/N_*.
  - re15_port/platform/android/* (README, build.gradle, android_glue.c, asset_abgleich.c/.h).
  - release/apk_asset_gate.py + .sha256, release/apk_pruefen.sh, release/gate_urteil.py + .sha256.
  - re15_port/tests/unit/probes/r35_android.cmake, tests/unit/r35_android/*, tests/unit/test_r35_android_abgleich.c.
  - 1 Zeile in tests/unit/test_r34a_asset_abgleich.c (Android-Test der Runde 34a, eigenes Gebiet).

  **Nicht** beruehrt: release/RELEASE_NOTES.md, release/make_package.sh, tests/unit/CMakeLists.txt,
  tests/integration/CMakeLists.txt, engine/src, include, shared_assets. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots
  oder Ereignisse (keine Spiellogik). `git merge-tree --write-tree master HEAD` -> rc 0, ohne Konflikt. Hinweis fuer
  den Merge: master steht bei RE15_MIN_TESTS 517 (6e1a3771), die Spur bringt +5 Tests.
- Tests: `ctest -R "r35_android|r34a_asset"` -> **6/6 Passed**:

  | Test | Laufzeit |
  |---|---|
  | r34a | 1,06 s |
  | abgleich | 0,02 s |
  | anzeige | 3,62 s |
  | konflikt | 2,00 s |
  | pruefkette | 125,26 s |
  | bash_mutanten | 213,61 s |

  anzeige und konflikt messen, was sie sollen: Die eigenen Szenarien werden mit dem alten Code rot (1/2 oben). Die
  Pruefkette und bash_mutanten messen, was sie versprechen, fangen aber die Aenderungen aus M1-M3 nicht.
- Suite:
  - Das Dossier enthaelt woertlich `=== LOCAL-BUILD-OK (all) — Tests 483/483` (Abschnitt "Nachbesserung 2 -
    Abschluss", N >= 478). `ctest -N` = 483.
  - Seit Abnahme 1 wurden nur release/, tests/unit/r35_android/*, probes/r35_android.cmake und README geaendert,
    kein C-Code. Alle davon betroffenen Tests (die 6 oben) habe ich selbst gefahren.
  - Die volle Suite habe ich nicht selbst gefahren; das ist nach Vorgabe nicht noetig, weil die Zeile im Dossier
    steht. Fenster-Haken waren nicht betroffen.

**Gates Pfad/Tests/Suite: halten.**

## 6. Maengel (nummeriert, nachpruefbar)

**M1 (Punkt 3, bash-Urteil): Die Uebergabe der Mindestzahlen und der unzip-Zaehlung an das Urteil ist von nichts
geprueft.** In `release/apk_pruefen.sh` gate_urteil() laufen diese Einzelaenderungen, jeweils in einer Kopie, durch:
- (J11) :338 `"$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN"` -> `0 0`
- (J12) :338 dasselbe -> `"$GATE_SELBSTTEST_MIN_INNEN" "$GATE_SELBSTTEST_MIN_INNEN"`
- (J13) :339 `"${GATE_APK_EINTRAEGE:-}"` -> `""`

Ergebnisse:
- Jede besteht `urteil_kontrollen.sh` mit `KONTROLLEN: 87 ok, 0 FALSCH`.
- bash_urteil_mutanten.py erzeugt an diesen Stellen keinen Mutanten.
- Die echte `test_r35_android_pruefkette.sh` endet mit `FEHLER=0` (K2/K3/K4).
- Folge, gemessen: Ein Gate-Selbsttest mit 200/200 Faellen (< 261) wird angenommen (original rc 2, J11/J12 rc 0). Eine
  APK, bei der unzip 14 statt 13 Eintraege zaehlt, wird angenommen (original rc 2, J13 rc 0).

Abhilfe-Richtung: Kontrollen, die die Uebergabe festhalten, z.B. eine Urteils-Attrappe, die argv[4..6] ausgibt bzw.
gegen 261/148/<n> prueft. Oder Kettenlaeufe mit dem echten Urteil: ein umgepinntes Gate, das einen Selbsttest unter
der Mindestzahl meldet (-> 2), und `gate_laufen apk` mit gesetztem, falschem `GATE_APK_EINTRAEGE` (-> 2).
Messweg: J11/J12/J13 wie oben -> mindestens eine Kontrolle bzw. die Kette muss rot werden.

**M2 (Punkt 3, Python-Urteil): Die Regel "genau n Fallzeilen" laesst sich ungestraft entfernen.**
`gate_urteil.py:126` `!= list(range(1, n + 1))` -> `!= list(range(1, len(faelle) + 1))`:
- `--selbsttest` -> `== URTEIL-SELBSTTEST-OK: 280/280 Faelle, 887/894 Mutanten erkannt, 7 als gleichwertig begruendet ==`.
- Das Urteil sagt 0 zu "5/5 Faelle" mit nur 4 Fallzeilen, das Original sagt 2.
- Neu gepinnt (G3) besteht es die ganze Kette, `FEHLER=0`.

Abhilfe-Richtung: ein Fall "Fallzeile fehlt (Nummern 1..n-1 lueckenlos, Schlusszeile n/n)" mit Soll 2.
Messweg: die Ersetzung oben -> `--selbsttest` muss `URTEIL-SELBSTTEST-FEHLER` melden.

**M3 (Punkt 3, Python-Urteil, Modus paket): Die Regel "je Baum Quelle = gleich" und die Form der Baumzeile sind
nur einseitig festgehalten.**
- (H17) `gate_urteil.py:174` `any(qq != gg or qq == 0 for qq, gg in baum)` ->
  `any(qq == 0 for qq, gg in baum) or sum(qq for qq, _g in baum) != sum(gg for _q, gg in baum)`:
  - `--selbsttest` OK (280/280, 886/893, 7).
  - Mit `GATE_URTEIL_MIN_ERKANNT=886` (Nachziehen, wie vom Skript verlangt) und neuem Pin besteht es die ganze Kette,
    `FEHLER=0` (G4b).
  - "PSX 9 10" + "synchro 3 2" -> original 2, Variante 0 mit dem Grund "je Baum Quelle = gleich".
- (H18) `gate_urteil.py:173` `r"   \S+\s+(\d+)\s+(\d+)"` -> `r"   \S+\s+(\d+)\s+(?:\S*\s+)?(\d+)"`:
  - `--selbsttest` OK (280/280, 889/896).
  - "PSX 10 9 10" -> original 2, Variante 0.

Abhilfe-Richtung: je ein Fall "Quelle != gleich in zwei Baeumen, Summen gleich" und "paket-Baumzeile mit drei Zahlen"
(Soll 2). Messweg wie M2.

**Hinweise ohne Mangelcharakter**
- H1: J9/J10 (Pin-Vergleich nur ueber die ersten 16 Hexziffern) bleiben in Kontrollen und Kette unbemerkt. Ein
  veraendertes Gate oder Urteil wird weiter erkannt (64 Bit Praefix), nur ein am Ende falsch eingetragener Pin wuerde
  angenommen. Eine Kontrolle "Pin mit falscher letzter Ziffer" kostet eine Zeile.
- H2: J6 (fester Modus in der Urteilszeile) faengt nur die Kette (P2 quellbaum), nicht urteil_kontrollen.sh, weil alle
  Attrappen-Laeufe im Modus selbsttest stehen. Die Kette laeuft in ctest; das genuegt, ist aber eine schmale Stelle.
- H3: `bash_urteil_mutanten.py` schreibt jeden Mutanten nach `arbeit/m%03d/apk_pruefen.sh` ohne Gate/Urteil daneben.
  Ein Mutant, der implizit `GATE_QUELLE`/`GATE_URTEIL_QUELLE` benutzt, wird dort ueber `can't open file` rot und
  zaehlt als "erkannt", ohne dass eine Kontrolle ihn erkannt haette. Bei den heutigen Operatoren A-H tritt das nicht
  auf (J14/J15 waren keine Operator-Mutanten). Als Messqualitaet bei kuenftigen Operatoren beachten.
- H4: Beide Mutationsmechanismen decken nur `urteil()` bzw. die sechs Funktionen in `FUNKTIONEN` ab. Eine Pruefung in
  einer NEUEN Funktion wuerde nicht mutiert. Das steht so im Kopf; bei kuenftigen Umbauten FUNKTIONEN mitpflegen.
- H5: Ein echtes Update APK A -> B mit dem H8-Muster auf dem Emulator steht weiter aus (OFFEN im Dossier, wie Abnahme 1).

## 7. Belege dieser Abnahme (Scratchpad der Abnahme-Sitzung, nicht im Repo)
- Punkt 1/2: `a2/a2_entpacker.py`, `a2/p1_neu.txt` / `a2/p1_alt.txt` (48 + 48 Laeufe), `a2/p2_neu.txt` /
  `a2/p2_alt.txt` (5 + 5 Szenarien), `a2/alt/pruefstand_alt.exe`.
- Punkt 3 Python: `a2/a2_varianten.py`, `a2/H_reihe.txt` (H0-H22, N1, N2), `a2/v/H/<id>/selbsttest.txt`,
  `a2/a2_folge.py` + `H10/H17/H18_eingabe.txt`, `a2/ereihe.txt` (E-Reihe der Abnahme 1 gegen HEAD, 29/29).
- Punkt 3 bash: `a2/J_reihe.txt` (J0-J16), `a2/v/J/<id>/kontrollen*.txt`, `a2/fv/<F1..F8>/k.txt`,
  `a2/J11_gate_selbsttest_200.txt`, `a2/J13_apk_12_unzip14.txt`.
- Kette: `a2/a2_kette.py`/`.sh`, `a2/kette_reihe.txt`, `a2/kv/<id>/kette.log` (K0-K5, G3, G4), `a2/kv2/G4b/kette.log`.
- ctest: `a2/ctest_r35_android.txt` (6/6, -V).
- Alle entscheidenden Aenderungen stehen oben woertlich mit Zeilennummer und sind ohne diese Dateien wiederholbar.
