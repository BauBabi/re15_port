# Runde 35 Spur N "android" — Abnahme 3 (unabhaengig, nach Nachbesserung 3)

Datum 2026-10-04. Baum `.claude/worktrees/r35_android`, Zweig r35/android, HEAD dd4f902c, Basis 154a73c1.
Massstab: Wortlaut AUFTRAG.md Z. 100-102. Gemessen am selbst gebauten Stand; vom Bau-Agenten ist nur uebernommen, was hier
nachgemessen ist. Alle eigenen Messungen sind ANDERE als die des Bau-Agenten und der Abnahmen 0-2: andere Displaygroessen,
drei bisher nicht gemessene Texte, andere Konflikt-Szenarien, andere simulierte Aenderungen am Pruefskript.

**Ergebnis: NICHT bestanden.** Punkt 1 erfuellt, Punkt 2 erfuellt, Punkt 3 teilweise.

Die drei Maengel der Abnahme 2 sind behoben und nachgemessen (J11/J12/J13 rot in den Kontrollen, H10/H17/H18 rot im
Selbsttest). Die Zahlen des Bau-Agenten stimmen (777/777, 887/894, 7, 102; 102 Kontrollen; 328/325/3; ctest 6/6).
Von 12 eigenen Aenderungen am Python-Urteil faengt der Selbsttest 11, von 5 eigenen Aenderungen an den sechs
bash-Urteilsfunktionen fangen die Kontrollen 4.

Offen bleibt ein ganzer Baustein des Pruefskripts: `apk_pruefen()` selbst (apk_pruefen.sh:390-495). Dort wird das
Urteil in das Ergebnis des Skripts uebersetzt (Abbruch oder `APK-PRUEFUNG-OK`), und dort entsteht die unzip-Zaehlung,
die das Urteil gegenprueft. Diese Funktion hat keine Kontrolle, keinen Mutanten und keinen Kettenlauf. Drei
Ein-Zeilen-Aenderungen dort machen aus einer abgelehnten APK `APK-PRUEFUNG-OK`, und nichts wird rot: weder der
Urteils-Selbsttest noch die 102 Kontrollen noch die Mutanten noch die echte ctest-Kette. Das ist genau das Bild A4/F-Y2
der Runde 34a, aus dem der Nutzerpunkt stammt, nur eine Zeile weiter unten. Mangel M1 unten.

Die Gates (Suite, @0x/PORT-WAHL, Pfade, Tests) halten.

---

## 0. Bau und Werkzeug

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to
  do", `=== LOCAL-BUILD-OK (build)`. `ctest -N` -> `Total Tests: 483`.
- Seit Abnahme 2 (876a9f90) ist kein C-Code geaendert: `git diff 876a9f90..HEAD -- re15_port/platform/android/jni` = 0
  Zeilen; engine/include/platform/pc ebenso leer. Geaendert sind release/gate_urteil.py (+ .sha256), release/apk_pruefen.sh,
  tests/unit/r35_android/{urteil_kontrollen.sh, bash_urteil_mutanten.py, test_r35_android_pruefkette.sh},
  probes/r35_android.cmake (nur Kommentar), platform/android/README.md und analysis/.
- Python fuer die eigenen Laeufe: `/c/Python310/python` (3.10.11), wie `release/python_finden.sh` in ctest. bash =
  `C:/Program Files/Git/bin/bash.exe` (= `R35_ANDROID_BASH` in der CMakeCache).
- Vorher-Stand fuer Punkt 1/2: eigener Pruefstand aus `git show 154a73c1:` von android_glue.c, asset_abgleich.c/.h und
  `pruefstand_main.c` von HEAD, uebersetzt wie build.ninja (`gcc -std=gnu11 -include kompat_win.h -I.../stub -I<alt/jni>`)
  -> `pruefstand_alt.exe`. Nachher-Stand: `re15_port/build/tests/unit/r35_android_pruefstand.exe`.
- Eigene Werkzeuge liegen im Scratchpad dieser Sitzung (Abschnitt 7). Jede entscheidende Aenderung steht unten woertlich
  mit Zeilennummer (HEAD) und laesst sich ohne diese Dateien wiederholen.

## 1. Punkt 1 — "Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten"

### Messung (12 neue Displaygroessen x 4 Texte, vorher und nachher, am echten android_glue.c)
- Groessen (keine davon in den Listen des Bau-Agenten, der Abnahmen 0-2 oder der Belege; per grep geprueft):
  3168x1440, 2412x1080, 1600x400, 4096x1080, 6880x2880, 12000x1200, 1792x828, 2048x768, 960x540, 400x240, 4000x300,
  1200x1920 (hochkant).
- Texte, drei davon von keiner Abnahme gemessen:
  - (p) Erstinstallation mit **1000 Dateien**: Titel `RE1.5 PORT - ASSETS WERDEN ENTPACKT` und die Fortschrittszeile mit
    vierstelligen Zahlen (`1000 / 1000 DATEIEN  (100%)  0 MB`).
  - (f) `FEHLER: ASSET-LISTE FEHLT IN DER APK` (APK ohne re15_assets.txt), neu.
  - (a) `FEHLER: ASSET-LISTE IM ALTEN FORMAT` (Kopf `# re15 assets 1 4`), neu.
  - (z) `10 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG` (58 Zeichen, zweistellige Zahl), neu.
- Ausgewertet je Lauf: Rueckgabe, `PRUEFSTAND-AUSSERHALB`/`-UEBERLAPPUNG`, x-Bereich aller `PRUEFSTAND-ZEILE`, und ob der
  ganze Text (Leerraum normalisiert) in einem `PRUEFSTAND-BILD text=` steht.

| Stand | Laeufe | mit Text/Rechteck ausserhalb, Ueberlappung oder fehlendem Text |
|---|---|---|
| vorher (154a73c1) | 48 | **15** |
| nachher (HEAD) | 48 | **0**, ganzer Text in allen 48 sichtbar, Rueckgabe wie erwartet |

Auszug:
```
vorher  3168x1440 z x=-504..3660   nachher 3168x1440 z x=192..2968
vorher  2412x1080 z x=-360..2763   nachher 2412x1080 z x=156..2246
vorher  6880x2880 z x=-910..7765   nachher 6880x2880 z x=134..6727
vorher  1200x1920 p x=-1185..2368  nachher 1200x1920 p x=75..1120   (vorher: 100 Verstoesse, Titel UND Fortschrittszeile)
nachher 12000x1200 z x=4260..7730, 4000x300 p x=1790..2208 (hoehenbegrenzt, mittig)
```
ctest `unit_r35_android_anzeige` selbst gefahren: Passed (3,73 s).

**Urteil Punkt 1: erfuellt.** Bei keiner gemessenen Breite verlaesst ein Text das Bild, auch bei den drei neuen Texten und
der vierstelligen Fortschrittszeile. Der alte Code zeigt x < 0 an genau den breiten Stellen (z bei 3168/2412/6880).

## 2. Punkt 2 — "Ein Update, bei dem eine Datei und ein Ordner gleichen Namens kollidieren, bricht sauber ab; erst der zweite Start heilt es"

### Messung (6 neue Szenarien; Start 1 = APK A frisch, Start 2 = APK B als Update, danach B noch einmal)
Nach jedem Start wird der Baum verglichen: jede gelistete Datei bytegleich, keine zusaetzliche Datei, kein Ordner ohne
gelistete Datei, `re15_assets_entpackt.txt` = Liste der APK.

| Szenario (eigene) | vorher Start 2 | nachher Start 2 | nachher letzter Start |
|---|---|---|---|
| W1 Ordner `PSX/k/` mit Datei `k/k` (gleicher Name zweimal) wird Datei `PSX/k` | rc 1, `FEHLER beim Entpacken: rename`, ABBRUCH | **rc 0, SPIELSTART, Baum = B** | schneller Weg |
| W2 Datei `synchro/STAGE2/Dat` wird Ordner `.../dat/` (Gross/klein) mit 2 Ebenen | rc 0 | rc 0, Baum = B | schneller Weg |
| W3 Ordner `PSX/a/` mit 4 Ebenen + Datei wird Datei `PSX/a`, zugleich `PSX/a.bin` geaendert | rc 1, rename, ABBRUCH | **rc 0, Baum = B** | schneller Weg |
| W4 verwaiste DATEI `PSX/n1/n2` (2. Ebene), Update braucht Ordner `n1/n2/` | rc 1, FEHLER, ABBRUCH | **rc 0**, `Konflikt geraeumt: shared_assets/PSX/n1/n2 war eine Datei, wo ein Ordner hin muss - entfernt` | schneller Weg |
| W5 Ordner `PSX/q/` mit Rest-Zwischendatei `q/c.neu` und leerem `q/leer/` wird Datei `PSX/q` | rc 1, rename, ABBRUCH | **rc 0**, `Konflikt geraeumt: shared_assets/PSX/q war ein Ordner, wo die Datei hin muss mit 1 Dateien darin - entfernt` | schneller Weg |
| W6 hin und her ueber 3 Updates: Ordner -> Datei -> Ordner -> Datei | Start 2 und 4: rc 1, ABBRUCH | **Start 2, 3, 4: je rc 0, Baum = APK** | schneller Weg |

Vorher zeigen W1, W3, W4, W5 und W6 genau das Bild des Nutzers: Das Update bricht ab, erst der naechste Start stellt den
Stand her (W6 sogar bei jedem zweiten Update). W2 (Datei wird Ordner) lief schon vorher, wie bei den Abnahmen 0-2. Nachher
werden alle 6 im selben Start fertig, und der Folgestart nimmt den schnellen Weg (`Assets aktuell (schneller Weg)`).
ctest `unit_r35_android_konflikt` Passed (2,29 s), `unit_r35_android_abgleich` Passed (0,02 s), `unit_r34a_asset_abgleich`
Passed (1,06 s). Nicht gemessen (wie OFFEN im Dossier, kein Mangel): ein echtes Update APK A -> B auf dem Emulator.

**Urteil Punkt 2: erfuellt.**

## 3. Punkt 3 — "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten"

### 3.1 Behauptungen der Nachbesserung 3, nachgemessen
- Pins: `sha256sum release/gate_urteil.py` = `4aa6bdfa3ef9f043bf109ce02df27d1fff2c39635ad606e595cbf5dba9d32753` =
  `release/gate_urteil.sha256`; `apk_asset_gate.py` = `f73c3b1a...f9b6` = Pin (unveraendert). Mindestzahlen
  apk_pruefen.sh:122-125 = 777/887/7/102.
- `release/gate_urteil.py --selbsttest` (10 s): `Stoerungen: 492 erzeugt, 390 mit Soll 2, 102 begruendet ungeprueft (Soll 0,
  10 UNGEPRUEFT-Eintraege)`, `Mutanten: 894 erzeugt, 887 erkannt, 7 als gleichwertig begruendet, 0 ueberlebt`,
  `== URTEIL-SELBSTTEST-OK: 777/777 Faelle, 887/894 Mutanten erkannt, 7 als gleichwertig begruendet, 102 Stoerungen
  begruendet ungeprueft ==`.
- `urteil()`, `urteil_rufen()` und `main()` sind zeichengleich mit 876a9f90 (AST-Segmente verglichen: 7812/961/576 Zeichen,
  alle "gleich"). Die Nachbesserung aendert also kein Urteil, nur Faelle, Stoerungen und Schlusszeile.
- Das Soll der Stoerungsfaelle kommt nicht aus dem Urteil (gate_urteil.py:361-366: Soll 0 nur, wenn `_ungeprueft()` jede
  Stelle deckt, sonst 2). Gegen den Code gelesen.
- ctest `-R "r35_android|r34a_asset"` selbst gefahren: **6/6 Passed**.
  - pruefkette 149,1 s: P0 `777/777 Faelle, 887/894 ..., 102 Stoerungen ungeprueft (Mindestzahlen 777/887/<=7/<=102)`, N26a-c
    und N27a-c/N28 wie beschrieben, `KONTROLLEN: 102 ok, 0 FALSCH`, `FEHLER=0`.
  - bash_mutanten 415,9 s: `Kontrolle (unveraendert): Rueckgabe 0 KONTROLLEN: 102 ok, 0 FALSCH`, `Mutanten je Operator: A 36,
    B 18, C 34, D 112, E 24, F 9, G 7, H 2, I 86`, `== BASH-URTEIL-MUTANTEN-OK: 328 Mutanten, 325 erkannt, 3 als gleichwertig
    begruendet ==`.
  - Die 86 I-Mutanten habe ich ausgezaehlt: alle erkannt. Toeter N25a 36, N8c 16, N25d 13, N10 6, N26 3, N25c 2,
    D4/D9/D16/D21 je 2, wie im Dossier.

### 3.2 Die Maengel der Abnahme 2, nachgemessen
- **M1 (Uebergabe):** J11/J12/J13 woertlich, je in einer Kopie von release/, gegen ALLE Kontrollen ohne `--schnell`:
  - J11 `0 0` -> `KONTROLLEN: 99 ok, 3 FALSCH` (N25a, N25b, N25d).
  - J12 MIN_INNEN doppelt -> 99/3 (N25a, N25b, N25d).
  - J13 `""` statt GATE_APK_EINTRAEGE -> 100/2 (N25b, N25d).

  Der Beleg des Bau-Agenten zur Kette (`nb3_kette_varianten_nachher.txt`, K2/K3/K4 rot ueber N26/N27) ist gelesen und
  passt dazu. **Behoben.**
- **M2 (H10)** und **M3 (H17/H18)** woertlich gegen `--selbsttest`: jede -> `URTEIL-SELBSTTEST-FEHLER: 5 von 777 Faellen
  falsch`.
  - H10: Faelle 281/282 (N3) + Stoerungen `Z6 '[ok] 05' weg`, `Zahl 2/3 Zusatz`.
  - H17: 283 + 4 gegenlaeufige Paare `pk/St: Z1/Z2 Spalte 0/1`.
  - H18: 284 + 4 Zusatzspalten.

  **Behoben.**
- Hinweis H1 (Pin nur Praefix): Die Kontrollen D5b/D10b stehen da, im Lauf `ok`. Hinweis H2 (fester Modus): N25e steht da,
  im Lauf `ok`.

### 3.3 Eigene simulierte Aenderungen am Python-Urteil `release/gate_urteil.py` (Q-Reihe)
Je Aenderung genau EINE Textersetzung in einer Kopie, dann `<kopie> --selbsttest`. BEMERKT = Rueckgabe != 0. Keine steht in
H1-H22/E/NB1 (Abnahmen) oder X1-X10 (Bau-Agent). Kontrolle Q0 (unveraendert): OK 777/777.

| # | Zeile | Aenderung | Selbsttest |
|---|---|---|---|
| Q1 | 154 | TORSE.VBS `sha256 gleich` -> `sha256 \w+` | BEMERKT (Fall 188) |
| Q2 | 111 | Urteilszeilen zaehlen nur noch die eigene Marke (`"== %s-(OK\|FEHLER\|ABWEICHUNG)\b" % re.escape(MARKE)`) | BEMERKT (055/056/058/059 + 1 Mutant) |
| Q3 | 362 | Stoerungs-Generator `dazu()`: `all(d is not None ...)` -> `any(...)` | BEMERKT (52 Faelle) |
| Q4 | 155 | `if apk_eintraege:` -> `if apk_eintraege and int(apk_eintraege) > 0:` | BEMERKT (2 neue Mutanten ueberleben) |
| Q5 | 119 | selbsttest-Schlusszeile `re.fullmatch(r"(\d+)/(\d+) Faelle \(.*\)", rest)` -> `re.match(r"(\d+)/(\d+) Faelle", rest)` | BEMERKT (124/125/228) |
| Q6 | 179 | paket `... for qq, gg in baum)` -> `... in baum[1:])` (erster Baum ungeprueft) | BEMERKT (118, 279, Stoerungen 700/701) |
| Q7 | 152 | Tuerarchive nur noch `t[2] == t[3] > 0` | BEMERKT (28 Faelle) |
| Q8 | 127 | Innere Proben `if a != b or b < min_innen:` -> `if b < min_innen:` | BEMERKT (8 Faelle) |
| Q9 | 147 | apk-Kette ohne Schlusszahl `not (q == a == g == mz)` | BEMERKT (7 Faelle, auch Stoerung 552) |
| Q10 | 151 | fehlende RE2/DOOR- bzw. RE15DOOR-Zeile toleriert (`... if label + ":" in text else [1, 1, 1, 1]`) | BEMERKT (097/103 + Stoerungen 498/512) |
| **Q11** | 147 | `or mb != b:` -> `or (mb and b and mb != b):` (Manifest-Bytes nur, wenn beide > 0) | **NICHT** (777/777, 888/895) |
| Q12 | 167 | quellbaum-Baumzeile `... Dateien"` -> `... Dateien.*"` | BEMERKT (236) |

Bilanz 11 von 12. Die Stoerungsfaelle tragen sichtbar mit: Q6, Q8, Q9 und Q10 werden auch durch erzeugte Faelle rot. Q3
zeigt, dass auch eine Aufweichung des Generators selbst auffaellt.
Folge Q11 (gemessen mit `urteil_rufen`): `Manifest 0 Bytes, SUMME 4096` -> original 2, Q11 0; `Manifest 4096, SUMME 0` ->
original 2, Q11 0; `4095/4096` -> beide 2. Die Regel faellt also nur am Wert 0 weg, kein Fall und keine Stoerung erzeugt
eine 0. Neu gepinnt besteht Q11 auch die echte Kette (`FEHLER=0`, 3.6). Das ist kein Mangel: Die Regel bleibt fuer jeden
anderen Wert bestehen, und eine SUMME von 0 Bytes ist mit den Stichproben aus Schritt 1 nicht erreichbar. Siehe Hinweis H1.

### 3.4 Eigene simulierte Aenderungen an den sechs bash-Urteilsfunktionen (R-Reihe)
Je Aenderung genau EINE Ersetzung in einer Kopie von release/ (Gate, Urteil, Pins daneben), dann
`urteil_kontrollen.sh anlegen` + `pruefen` (alle 102 Kontrollen, ohne `--schnell`). Kontrolle R0: `102 ok, 0 FALSCH`.

| # | Zeile | Aenderung | Kontrollen |
|---|---|---|---|
| R1 | 313 | `u="${BASH_REMATCH[6]}"` -> `[5]` (u = g) | BEMERKT 99/3 (P3, P3b, N15u) |
| R2 | 301/302 | Zwischenspeicher VOR der Pin-Pruefung | BEMERKT 98/4 (D11b, N20, N20b, N20c) |
| R3 | 381 | zweite Instanz: `Rueckgabe 0): ` -> `Rueckgabe [0-9]*): ` | BEMERKT 101/1 (N19c) |
| R4 | 344 | gate_festhalten: `gate_urteil_selbsttest "$uziel"` -> `gate_urteil_pin_pruefen "$uziel"` | BEMERKT 101/1 (D22) |
| **R5** | 371 | gate_laufen: `> "$log" 2>&1` -> `> "$log"` (stderr des Gates nicht mehr in der Ausgabe) | **NICHT** 102/0, Kette `FEHLER=0` |

R5 schaltet fuer stderr die Urteilsregel "OK-Schlusszeile, aber im selben Lauf ABBRUCH/Traceback" ab (gate_urteil.py:114-116).
Das echte Gate schreibt `ABBRUCH`/Traceback nur nach stderr (apk_asset_gate.py:3497, 3504-3505) und gibt dabei immer
Rueckgabe 2. Deshalb aendert R5 ein Urteil nur, wenn zugleich ein zweiter Gate-Fehler vorliegt (OK auf stdout und Rueckgabe
0 trotz ABBRUCH). Siehe Hinweis H2.

### 3.5 Eigene simulierte Aenderungen an `apk_pruefen()` selbst (V-Reihe) — Ende zu Ende gemessen
`apk_pruefen()` (apk_pruefen.sh:390-495) ist die Funktion, die build_android.sh:175 und make_package.sh:554 aufrufen. Sie
uebersetzt das Urteil in das Ergebnis des Pruefskripts: Abbruch oder `== APK-PRUEFUNG-OK`. Sie liefert dem Urteil auch die
unzip-Zaehlung (`GATE_APK_EINTRAEGE`).

| # | Zeile | Aenderung (je genau eine Zeile) | Kontrollen | Kette | Ende zu Ende (gleiche Attrappen, original -> Variante) |
|---|---|---|---|---|---|
| V1 | 475 | `rc=0; GATE_APK_EINTRAEGE="$n_assets"` -> `rc=0; GATE_APK_EINTRAEGE=""` | 102 ok / 0 FALSCH | `FEHLER=0` | E2: original `Gate-Urteil (apk, Rueckgabe 0): unzip zaehlt 6 Eintraege unter assets/, das Gate 12 Asset-Dateien + Manifest`, `ABBRUCH: ... keine Aussage moeglich (Urteil 2)`, EXIT 1 -> V1 `APK-ASSET-GATE-OK: 12 Dateien ...`, **`== APK-PRUEFUNG-OK`**, EXIT 0 |
| V2 | 480 | `1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab ..." ;;` -> `1) echo "   WARNUNG: ..." ;;` | 102 / 0 | `FEHLER=0` | E1: original `Gate-Urteil (apk, Rueckgabe 1): das Gate meldet ABWEICHUNG`, `ABBRUCH: APK-Asset-Gate: die APK weicht vom Quellbaum ab`, EXIT 1 -> V2 `WARNUNG: ...`, **`== APK-PRUEFUNG-OK`**, EXIT 0 |
| V3 | 473 | `(( rc == 0 )) \|\| die "Selbsttest des APK-Asset-Gates ...` -> `(( rc <= 1 )) \|\| die ...` | 102 / 0 | `FEHLER=0` | E3: original `Gate-Urteil (selbsttest, Rueckgabe 1): das Gate meldet FEHLER`, `ABBRUCH: Selbsttest des APK-Asset-Gates nicht bestanden ... dem Gate ist nicht zu trauen`, EXIT 1 -> V3 laeuft weiter, **`== APK-PRUEFUNG-OK`**, EXIT 0 |

Messaufbau Ende zu Ende (Werkzeug `a3_ende.sh`):
- apk_pruefen.sh (original bzw. Variante) wird mit den echten gate_festhalten/gate_laufen/gate_urteil und dem echten,
  gepinnten Urteil geladen.
- aapt, zipalign und java/apksigner sind Attrappen, die "alles gut" melden.
- Die Fake-APK enthaelt genau die Stichproben aus Schritt 1 (6 Eintraege unter `assets/`).
- `APK_GATE_KOPIE` ist ein gepinnter Gate-Ersatz, dessen Ausgaben aus `gate_urteil.py` `_selbsttest_log(261, 148)` bzw.
  `_apk_log()` stammen:
  - E1: Selbsttest OK, APK-ABWEICHUNG mit Rueckgabe 1.
  - E2: APK-OK mit 12 Dateien, unzip zaehlt 6.
  - E3: Selbsttest mit `[FEHLER]` und Rueckgabe 1, APK-OK passend zur unzip-Zaehlung.

In jedem Original-Lauf lehnt das Skript richtig ab. Jede Variante liefert `APK-PRUEFUNG-OK`.

Warum nichts rot wird (Code gelesen):
- `bash_urteil_mutanten.py:45` `FUNKTIONEN` nennt nur die sechs gate_*-Funktionen. Die Mutanten des ctest-Laufs liegen
  ausschliesslich in apk_pruefen.sh-Zeilen 261-387 (min/max aus `[erkannt]/[gleichwertig] Zeile N` ausgezaehlt).
  apk_pruefen() (390-495) hat **0 Mutanten**.
- `urteil_kontrollen.sh` ruft apk_pruefen() nie auf, ebenso `test_r35_android_pruefkette.sh`. Deren Kopf sagt woertlich:
  "aapt/apksigner braucht nur apk_pruefen, nicht diese Bausteine". Die Kette setzt `GATE_APK_EINTRAEGE` selbst (N27) und
  liest Zeile 475 nicht.
- Dossier, README und Kopf von apk_pruefen.sh begrenzen das bash-Urteil auf die sechs Funktionen. Unter OFFEN steht
  apk_pruefen() nicht.

V1 ist der Zwilling von J13, also Mangel M1 der Abnahme 2: dieselbe unzip-Gegenprobe, still abgeschaltet mit einer
Zeile, nur eine Aufrufebene hoeher. V2 und V3 haben dieselbe Wirkung wie A4 in Runde 34a
(`analysis/befunde_runde34_android/pruefer_umgehung_r4_2.md` Z. 199-207, 230: "Urteil 1 Zeichen geaendert ->
APK-PRUEFUNG-OK"). Daraus ist der Nutzerpunkt entstanden.

### 3.6 Ende zu Ende: eigene Varianten an der ECHTEN Kette
Je Variante eine Repo-Kopie im Scratchpad mit GENAU EINER Aenderung. Die Assets sind Hardlinks auf eine volle Kopie im
Scratchpad, nie auf den Baum. Dann laeuft die echte `test_r35_android_pruefkette.sh` des Baums (Werkzeug `a3_kette.py`,
3 parallel).

| # | Aenderung | Kette |
|---|---|---|
| K0 | keine (Kontrolle) | `FEHLER=0`, KONTROLLE-OK (P0 777/887/7/102, P1 261/261 148/148, P2 3629 Dateien in 5 Baeumen) |
| R5 | 2>&1 weg (3.4) | NICHT: `FEHLER=0` |
| **V1** | GATE_APK_EINTRAEGE leer (3.5) | **NICHT: `FEHLER=0`** |
| **V2** | Urteil 1 nur WARNUNG (3.5) | **NICHT: `FEHLER=0`** |
| **V3** | Gate-Selbsttest mit Urteil 1 angenommen (3.5) | **NICHT: `FEHLER=0`** |
| Q11 | Manifest-Bytes nur bei > 0, Urteil neu gepinnt (3.3) | NICHT: `FEHLER=0`, P0 `777/777, 888/895` |

### 3.7 Ursache
Die Nachbesserungen 1-3 haben das Python-Urteil und die sechs bash-Funktionen davor gruendlich abgesichert: Mutanten in
beiden Richtungen, Faelle beider Seiten, erzeugte Stoerungsfaelle, Kontrollen der Uebergabe und Kettenlaeufe an den
Grenzen. Die letzte Stufe des Pruefskripts liegt ausserhalb jedes Mechanismus:
- die Uebersetzung `Urteil -> Abbruch/OK` (473, 478-482),
- die Herkunft der unzip-Zaehlung (421, 475-477).

Ebenso die uebrigen Pruefzeilen von apk_pruefen() (Stichproben, aapt, zipalign, Signer, Kennung). Eine Aenderung dort wird
heute von nichts mitgetestet. Der Schutz der sechs Funktionen wirkt nur, solange niemand das Ergebnis eine Ebene darueber
umdeutet.

**Urteil Punkt 3: teilweise.**

Erfuellt ist:
- Das Python-Urteil ist jetzt in der Breite abgesichert (11/12 eigene Aenderungen, alle Aenderungen der Abnahmen 0-2).
- Die sechs bash-Funktionen sind abgesichert (4/5 eigene, J11-J13, 328 Mutanten).
- Neue Regeln werden in beiden Schichten erzwungen.

Nicht erfuellt ist "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten" fuer den
Teil des Pruefskripts, der das Urteil anwendet: Drei realistische Ein-Zeilen-Aenderungen in apk_pruefen.sh lassen eine
abgelehnte APK als `APK-PRUEFUNG-OK` durch, und die ganze Suite bleibt gruen.

## 4. RE-Gate

- Die Spur ist reine Port-Infrastruktur (Android-Entpacker, Release-Pruefkette). Die PSX hat weder Entpacker noch APK, also
  gibt es kein Original zum Disassemblieren. Im Diff 154a73c1..HEAD ohne analysis/ stehen **0** `@0x`-Zitate und
  **4** `PORT-WAHL`-Kennzeichnungen. Die neuen Zahlen 777/102 sind als gemessen gekennzeichnet (apk_pruefen.sh:109-125,
  Dossier-Tabelle "Konstanten"). Die Stoerungsweite +-1/Zusatz/Paar ist als PORT-WAHL mit Grund gefuehrt.
- Statt einer Disassembly habe ich 3 zitierte Quellstellen selbst nachgelesen. Alle enthalten das Behauptete:
  - UNGEPRUEFT "TORSE.VBS: ... die Bytezahlen vergleicht es nicht": gate_urteil.py:154, `genau_eine(...)` ohne Vergleich
    der Gruppen.
  - UNGEPRUEFT "Modus apk: die Baumzahl 'in k Baeumen' liest das Urteil nicht": gate_urteil.py:140-144, nur
    `int(m.group(1))` wird benutzt.
  - Dossier "N26/N27: 261/148 aus apk_pruefen.sh; 12 Dateien + Manifest = 13": apk_pruefen.sh:107-108 = 261/148,
    gate_urteil.py:156 `int(apk_eintraege) != a + 1`.
- Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/vorerst/Platzhalter) in den `+`-Zeilen ohne
  analysis/: **0 Treffer** (Basis 154a73c1 und NB3 einzeln). Neue `getenv` in engine/platform/include: **0**.
- Erklaert der Fix den Befund?
  - Punkt 1: ja, vorher x < 0 an denselben Stellen (15/48 eigene Laeufe).
  - Punkt 2: ja, vorher ABBRUCH im Update-Start, Heilung erst im Folgestart (W1, W3-W6).
  - NB3: die Ursachen der Abnahme 2 (keine Attrappe las argv[2]/argv[4..6], keine Faelle fuer luecken- und
    gegenlaeufige Stoerungen) sind im Code behoben. J11-J13 und H10/H17/H18 werden rot.

**Gate: haelt.**

## 5. Vertrag / Pfade / Tests / Suite

- Pfade = `git diff 154a73c1..HEAD --name-only`:
  - analysis/befunde_runde35/N_*.
  - re15_port/platform/android/* (README, build.gradle, android_glue.c, asset_abgleich.c/.h).
  - release/apk_asset_gate.py + .sha256, release/apk_pruefen.sh, release/gate_urteil.py + .sha256.
  - re15_port/tests/unit/probes/r35_android.cmake, tests/unit/r35_android/*, tests/unit/test_r35_android_abgleich.c.
  - 1 Pruefzeile in tests/unit/test_r34a_asset_abgleich.c (Android-Test der Runde 34a, Regel R1, eigenes Gebiet).

  **Nicht** beruehrt: release/RELEASE_NOTES.md, release/make_package.sh, tests/unit/CMakeLists.txt,
  tests/integration/CMakeLists.txt, engine/src, include, shared_assets. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots oder
  Ereignisse (keine Spiellogik). `git merge-tree --write-tree master HEAD` -> rc 0, ohne Konflikt. Hinweis fuer den Merge:
  master steht bei RE15_MIN_TESTS 517, die Spur bringt +5 Tests.
- Tests: `ctest -R "r35_android|r34a_asset"` -> **6/6 Passed**:

  | Test | Laufzeit |
  |---|---|
  | r34a | 1,06 s |
  | abgleich | 0,02 s |
  | anzeige | 3,73 s |
  | konflikt | 2,29 s |
  | pruefkette | 149,06 s |
  | bash_mutanten | 415,90 s |

  anzeige und konflikt messen, was sie sollen: Die eigenen Szenarien werden mit dem alten Code rot (1/2 oben). Pruefkette
  und bash_mutanten messen, was sie versprechen, aber nicht apk_pruefen() (M1).
- Suite:
  - Das Dossier enthaelt woertlich `=== LOCAL-BUILD-OK (all) — Tests 483/483` (Abschnitt "Nachbesserung 3 - Abschluss",
    N >= 478). `ctest -N` = 483.
  - Seit Abnahme 2 ist kein C-Code geaendert. Alle betroffenen Tests (die 6 oben) habe ich selbst gefahren.
  - Fenster-Haken waren nicht betroffen, kein Nachfahren noetig.
- Arbeitsbaum nach allen Messungen: `git status --short` leer (alle Kopien und Laeufe im Scratchpad).

**Gates Pfad/Tests/Suite: halten.**

## 6. Maengel (nummeriert, nachpruefbar)

**M1 (Punkt 3): `apk_pruefen()` — die Stufe, die das Urteil anwendet und ihm die unzip-Zaehlung liefert — ist von keiner
Pruefung erfasst.** In `release/apk_pruefen.sh` laufen diese Einzelaenderungen, jeweils in einer Kopie, durch:
- (V1) :475 `rc=0; GATE_APK_EINTRAEGE="$n_assets"` -> `rc=0; GATE_APK_EINTRAEGE=""`
- (V2) :480 `1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab (Befunde oben)" ;;` -> `1) echo "   WARNUNG: ..." ;;`
- (V3) :473 `(( rc == 0 )) || die "Selbsttest des APK-Asset-Gates nicht bestanden ...` -> `(( rc <= 1 )) || die ...`

Ergebnisse:
- Jede besteht `urteil_kontrollen.sh` mit `KONTROLLEN: 102 ok, 0 FALSCH`.
- bash_urteil_mutanten.py erzeugt in apk_pruefen() keinen einzigen Mutanten (`FUNKTIONEN`, Zeile 45; Mutanten nur in
  Zeilen 261-387).
- Die echte `test_r35_android_pruefkette.sh` endet je mit `FEHLER=0`.
- Folge, gemessen Ende zu Ende mit dem echten, gepinnten Urteil (3.5):
  - eine APK, fuer die das Gate ABWEICHUNG meldet (V2),
  - eine APK, deren unzip-Zaehlung nicht zum Gate passt (V1),
  - ein Gate, dessen Selbsttest einen Befund meldet (V3),

  ergeben je `== APK-PRUEFUNG-OK` und EXIT 0. Das Original lehnt alle drei ab (EXIT 1).

Abhilfe-Richtung:
- apk_pruefen() in die Kontrollen aufnehmen, mit Attrappen fuer aapt/zipalign/java und einem gepinnten Gate-Ersatz ueber
  `APK_GATE_KOPIE`, wie `a3_ende.sh`. Mindestens diese Kontrollen: gut -> `APK-PRUEFUNG-OK`; E1 -> Abbruch "weicht ab"; E2
  (unzip passt nicht) -> Abbruch "keine Aussage"; E3 (Gate-Selbsttest Urteil 1) -> Abbruch "Selbsttest".
- apk_pruefen() (oder die Schritte 1-6 als eigene Funktion) in `FUNKTIONEN` eintragen, damit Operatoren A-I auch dort
  mutieren.
- Den Umfang in Dossier, README und Kopf von apk_pruefen.sh danach ausrichten.

Messweg: V1/V2/V3 wie oben. Danach muss mindestens eine Kontrolle bzw. die Kette rot werden, und
`unit_r35_android_bash_mutanten` darf an diesen Zeilen keinen Ueberlebenden haben.

**Hinweise ohne Mangelcharakter**
- H1: Q11 (gate_urteil.py:147 `or mb != b` -> `or (mb and b and mb != b)`) bleibt in Selbsttest und Kette unbemerkt. Die
  Regel faellt nur am Wert 0 weg (gemessen 3.3), weil kein Fall und keine Stoerung eine 0 erzeugt. Ein Fall
  "Manifest 0 Bytes / SUMME 4096 -> 2" kostet eine Zeile. Dieselbe Luecke "Wert 0" kann an anderen Zahlen auftreten: Die
  Stoerungen verschieben nur um +-1.
- H2: R5 (apk_pruefen.sh:371, `2>&1` weg) bleibt in Kontrollen, Mutanten und Kette unbemerkt. Keine Gate-Attrappe schreibt
  nach stderr. Die Wirkung tritt nur zusammen mit einem zweiten Gate-Fehler auf (3.4). Eine Attrappe "OK auf stdout,
  ABBRUCH auf stderr, Rueckgabe 0 -> 2" schliesst das.
- H3: Dieselbe Klasse wie M1 steht in `release/make_package.sh` (:301 paket, :513 selbsttest, :523 quellbaum: Auswertung
  der Rueckgabe von gate_laufen). Das ist nicht "das Pruefskript" dieses Punkts und nicht von der Spur geaendert, beim
  Beheben von M1 aber gleich mitdenken.
- H4: Wie Abnahme 2 H4: Eine Pruefung in einer NEUEN bash-Funktion wird erst mutiert, wenn sie in `FUNKTIONEN` steht.
  Operator I deckt keine Argumente in `$( ... )` (im Dossier unter OFFEN).
- H5: Ein echtes Update APK A -> B mit dem H8-Muster auf dem Emulator steht weiter aus (OFFEN im Dossier, wie Abnahmen 1/2).

## 7. Belege dieser Abnahme (Scratchpad der Abnahme-Sitzung `.../scratchpad/a3/`, nicht im Repo)

- Punkt 1/2: `a3_entpacker.py`; `p1_neu.txt`/`p1_alt.txt` (48 + 48 Laeufe); `p2_neu.txt`/`p2_alt.txt` (6 + 6 Szenarien);
  `alt/pruefstand_alt.exe`.
- Punkt 3:
  - Python: `a3_q_reihe.py`, `q_reihe.txt`, `q/<Q>/selbsttest.txt`.
  - Abnahme-2-Nachmessung: `m2/H10|H17|H18/` und `a3_j_reihe.py`, `j_reihe.txt`.
  - bash: `a3_rv_reihe.py`, `rv_reihe.txt`, `rv/<R|V>/kontrollen.txt`.
  - Ende zu Ende: `a3_ende.sh`, `ende.txt`, `ende/<V>_<E>/apk_pruefen.txt`.
  - Kette: `a3_kette.py`, `kette_reihe.txt`, `kv/<id>/kette.log`.
- ctest: `ctest_r35_android.txt` (6/6, -V).
- Alle entscheidenden Aenderungen stehen oben woertlich mit Zeilennummer und sind ohne diese Dateien wiederholbar.
