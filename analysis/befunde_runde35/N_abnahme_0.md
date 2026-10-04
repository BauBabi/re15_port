# Runde 35 Spur N "android" — Abnahme 0 (unabhaengig)

Datum 2026-10-04. Baum `.claude/worktrees/r35_android`, Zweig r35/android, HEAD 5c1f0af4, Basis 154a73c1.
Gemessen am selbst gebauten Stand; dem Bau-Agenten wurde nichts geglaubt, was hier nicht nachgemessen ist.
Massstab: Wortlaut AUFTRAG.md Z. 100-102.

**Ergebnis: NICHT bestanden.** Punkt 1 erfuellt, Punkt 2 erfuellt, Punkt 3 teilweise (4 von 12 simulierten
"kuenftigen Aenderungen" an der Urteilslogik bleiben unbemerkt, die Doku behauptet mehr als der Mechanismus leistet).
Alle Gates (Suite, @0x/PORT-WAHL, Pfade, Tests) halten. Maengel M1-M3 unten.

---

## 0. Bau und Werkzeug

- `bash re15_port/tools/local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)` ("ninja: no work to do").
  Damit kein alter Stand gemessen wird: die Objekte/exe der Ziele `r35_android_pruefstand`, `test_r35_android_abgleich`,
  `test_r34a_asset_abgleich` geloescht und neu gebaut (`[9/9] Linking C executable tests\unit\r35_android_pruefstand.exe`).
- Eigene Pruefstaende (Scratchpad, ausserhalb des Baums), gleiche Flags wie build.ninja (`-std=gnu11 -include
  kompat_win.h`, Attrappen aus `tests/unit/r35_android/stub`):
  - **vorher**: `android_glue.c`, `asset_abgleich.c/.h` aus `git show 154a73c1:...` + `pruefstand_main.c` von HEAD;
  - **nachher**: HEAD-Quellen, zusaetzlich mit `-Wall -Wextra` uebersetzt -> **0 Warnungen** (der alte Stand warnt
    `-Wmisleading-indentation` an `if (frac < 0) frac = 0; if (frac > 1) ...`, android_glue.c:313 @154a73c1).
- Python fuer Punkt 3: `release/python_finden.sh` -> `/c/Python310/python (3.10.11)`.

## 1. Punkt 1 — "Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten"

### Messung (eigenes Skript, 17 Displaygroessen x 4 Texte, vorher und nachher)
Groessen: 2400x1080, 2340x1080, 2520x1080, 2960x1440, 3120x1440, 3440x1440, 3840x1080, 5120x1440, **7680x1080**,
1920x1080, 1280x720, 2208x1768, 2560x1600, 1080x2400, 480x320, 320x240, 160x120 (10 davon nicht im ctest des
Bau-Agenten). Texte: Uebergangstitel (44 Z.), `1 DATEIEN ...` (57), `FEHLER: ALTE ASSET-LISTE NICHT LOESCHBAR ...` (58)
und **`1000 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG` (60 Zeichen, laenger als der laut Dossier
laengste Text** - entsteht bei 1000 nicht entpackbaren Dateien, z.B. Speicher voll; Liste mit 1000 fehlenden Eintraegen).
Ausgewertet werden die Kaesten, die die Attrappe von `re15_touch_pc_text` je Zeile meldet (Breite (6*len-1)*s, Hoehe 7*s
= touch_overlay_pc.c:466-486, dort nachgelesen).

| Stand | Laeufe | Laeufe mit Text ausserhalb des Bildes |
|---|---|---|
| vorher (154a73c1) | 68 | **55** (alle ausser 3440x1440-Titel und den 32:9/7680-Laeufen) |
| nachher (HEAD) | 68 | **0** (auch keine Ueberlappung Text/Balken) |

Geometrie 2400x1080 (`PRUEFSTAND-ZEILE`/`-AUSSERHALB`, eigene Laeufe):
```
vorher  Titel   x=-120..2510 s=10   AUSSERHALB      nachher Titel   x=144..2248 s=8
vorher  58 Z.   x=-366..2757 s=9    AUSSERHALB      nachher 58 Z.   x=156..2238 s=6
vorher  60 Z.   x=-420..2811 s=9    AUSSERHALB      nachher 60 Z.   x=120..2274 s=6
nachher 160x120: 60 Z. auf 3 Zeilen umbrochen ('1000 DATEIEN KONNTEN' / 'NICHT ENTPACKT WERDEN -' / ...), x=11..148
nachher 7680x1080: Titel s=10 (hoehenbegrenzt, unveraendert) x=2520..5150
```
Die alten Werte (-120 / -366) sind dieselben wie im Dossier und erklaeren die Geraetebilder der Runde 34a Zeichen fuer
Zeichen (Titel ohne "RE"/"FT"); der Fix (Skalierung = min(Hoehe, (W-2*0.4u)/(6*Zeichen)), Umbruch unter s=1) beseitigt
genau diese Ursache - die Vorbedingung des Fixes (Breite nie gegen W geprueft) steht im alten Code
(`int tw1 = strlen(l1)*6*fs; re15_touch_pc_text(r, (W - tw1)/2, ...)` @154a73c1).

Geraetebilder des Bau-Agenten angesehen (Emulator 2400x1080, Bildpunkte x1.2 umgerechnet):
`N_android_belege/geraet/E1_uebergang_3s.png` Titel vollstaendig, ~x=142..2248; `E2_fehler_liste.png` 58-Zeichen-Text
vollstaendig, ~x=152..2239 - deckt sich auf 2-4 px mit meiner Pruefstand-Geometrie. (Den Emulator selbst habe ich nicht
erneut gestartet; die APK wurde nach dem Lauf geloescht.)

Gegenprobe des ctest: `cmake -DPRUEFSTAND=<vorher-exe> -DTEIL=anzeige -P test_r35_android_entpacker.cmake` -> EXIT 1,
`FEHLER: A_2400x1080_uebergang: verboten 'PRUEFSTAND-AUSSERHALB ... x=-120..2510 ...'` - der Test misst.

**Urteil Punkt 1: erfuellt.**

## 2. Punkt 2 — "Ein Update, bei dem eine Datei und ein Ordner gleichen Namens kollidieren, bricht sauber ab; erst der zweite Start heilt es"

### Code gelesen (android_glue.c / asset_abgleich.c, Diff 154a73c1..HEAD)
weg-Schleife: `unlink` -> bei Fehler != ENOENT `re15_abgleich_weg_frei` + `access`-Kontrolle, sonst WARNUNG (F-Y6 nicht
mehr verschluckt); danach `re15_abgleich_leere_eltern` (leer gewordene Ordner, z.B. `q/` bei H8). Vor jedem Entpacken
`re15_abgleich_weg_frei` (Datei auf Elternsegment -> unlink; Ordner auf `<rel>.neu`/`<rel>` -> samt Inhalt, lstat,
Symlinks nie verfolgt). Die Groessen-/Summenentscheidung je Eintrag faellt erst NACH der weg-Schleife (`file_size(dst)`
in der Hauptschleife) - auch wenn das Raeumen einmal etwas Gelistetes trifft, wird es im selben Lauf neu entpackt.
Leser-Regeln R1/R2 (Geraet, Gate, Gradle) gelesen; R2 per bsearch in der mit demselben Vergleich sortierten Liste.

### Messung (eigene Szenarien, vorher/nachher, Start 1 = APK A frisch, Start 2 = APK B als Update, Start 3 = B)
Baumvergleich nach jedem Start: Dateiliste und Inhalte = APK B, keine leeren Ordner, `re15_assets_entpackt.txt` = Liste B.

| Szenario (eigene, ausser K1-Muster) | vorher Start 2 | vorher Start 3 | nachher Start 2 | nachher Start 3 |
|---|---|---|---|---|
| K1 Ordner `PSX/q/` (c) wird Datei `PSX/q` (H8) | EXIT 1, `rename ... FEHLER`, ABBRUCH | EXIT 0 "ohne Liste" | **EXIT 0, BAUM=APK** | schneller Weg |
| T1 tief: `PSX/q/r/s/c` wird Datei `PSX/q` | EXIT 1 (leer `q/r/s` bleibt) | EXIT 0 | **EXIT 0, BAUM=APK** | schneller Weg |
| T2 Datei `PSX/q` wird `PSX/q/r/s/c` | EXIT 0 | schnell | EXIT 0 | schnell |
| S1 synchro: `.../main04.wav/x` wird Datei `main04.wav` | EXIT 1 | EXIT 0 | **EXIT 0, BAUM=APK** | schneller Weg |
| M1 drei Konflikte zugleich (q, p, synchro/S/z) + unveraenderte Datei | EXIT 1 (2 Dateien) | EXIT 0 | **EXIT 0, BAUM=APK** | schneller Weg |
| K9 wie K1, aber fremde Datei `q/fremd.txt` im alten Ordner | EXIT 1 | EXIT 0 | **EXIT 0**, `Konflikt geraeumt: shared_assets/PSX/q war ein Ordner, wo die Datei hin muss mit 1 Dateien darin` | schneller Weg |
| X1 Ordner `PSX/X.neu/u/v/rest` (Zwischendatei-Name), X geaendert | EXIT 1 | EXIT 0 | **EXIT 0**, `Konflikt geraeumt: ... X.neu war ein Ordner auf dem Namen der Zwischendatei` | schneller Weg |
| R1 NICHT raeumbar: schreibgeschuetzte Datei `PSX/q`, Update braucht Ordner `q/` | EXIT 1 | EXIT 1 | EXIT 1, `FEHLER: Konflikt nicht raeumbar: shared_assets/PSX/q ist eine Datei, wo ein Ordner hin muss (Permission denied)`, ABBRUCH | EXIT 1 |

Damit ist auch der Fall gemessen, den der Bau-Agent nur ueber den Code-Pfad belegt hatte ("nicht raeumbar"): er bricht
fail-closed mit klarer Meldung ab (kein Spielstart mit Loch) - genau das erwartete Verhalten, wenn nichts heilen kann.

ctest des Bau-Agenten: `unit_r35_android_konflikt` gruen (K1-K6 EXIT 0 im Update-Start, L1/L2 fail closed);
Gegenprobe mit meinem vorher-Pruefstand -> EXIT 1 (`FEHLER: K1_ordner_wird_datei_2_update: EXIT=1, soll 0` ...) - misst.
`unit_r35_android_abgleich` `OK: 63 Pruefungen, 0 Fehler`, `unit_r34a_asset_abgleich` gruen.
Geraet: Logcat E3 des Bau-Agenten gelesen (`Konflikt geraeumt: synchro/STAGE1/room1240/main04.wav.neu ... 1 Konflikte
geraeumt ... 0 Fehler`, Datei 2017588 B wieder da) - das Raeumen laeuft auf dem echten App-Speicher.

Rest (kein Mangel, Hinweis): ein echtes APK-A->APK-B-Update mit dem H8-Muster auf dem Emulator ist weder vom Bau-Agenten
noch von mir gefahren worden. Der gemessene Code ist der unveraenderte android_glue.c mit echten Dateioperationen
(NTFS case-insensitiv hier, ext4 beim Bau-Agenten), nur SDL/JNI/AAssetManager sind Attrappen.

**Urteil Punkt 2: erfuellt.**

## 3. Punkt 3 — "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten"

### Nachgemessen, was behauptet ist
- Pins: `sha256sum release/gate_urteil.py` = `2b78069e1e00cc91...` = `release/gate_urteil.sha256`;
  `release/apk_asset_gate.py` = `f73c3b1a770d424d...` = `release/apk_asset_gate.sha256`.
- `"$PY" release/gate_urteil.py --selbsttest` -> `Mutanten: 260 erzeugt, 257 erkannt, 3 als gleichwertig begruendet,
  0 ueberlebt` / `== URTEIL-SELBSTTEST-OK: 119/119 Faelle, 257/260 Mutanten erkannt, 3 als gleichwertig begruendet ==`
  (5,1 s).
- `"$PY" release/apk_asset_gate.py --selbsttest` -> rc 0, `Innere Proben: 148/148`, `== SELBSTTEST-OK: 261/261 Faelle ...`.
- ctest `unit_r35_android_pruefkette` (93,6 s) gruen: P0-P2 ok, N1-N9 alle ROT wie verlangt, `FEHLER=0`.

### Eigene Simulation "kuenftiger Aenderungen" (nicht aus dem Mutator des Bau-Agenten)
a) Am Urteil `release/gate_urteil.py` (Kopie im Scratchpad, jeweils genau eine Textaenderung, dann `--selbsttest`):

| # | Aenderung | Selbsttest |
|---|---|---|
| A1 | neue Regel ohne Fall: `if any("WARNUNG" in z for z in zeilen): ende(2, ...)` vor `rest = m_ok.group(1)` | BEMERKT (3 Mutanten ueberleben) |
| A2 | quellbaum: `or 0 in baum` gestrichen | BEMERKT (1 Fall falsch) |
| A3 | apk: `or mb != b` gestrichen | BEMERKT |
| A4 | selbsttest-Fallzeile: `or f.group(6).strip()` gestrichen | BEMERKT (Fall 073) |
| **A5** | selbsttest-Schlusszeile `r"(\d+)/(\d+) Faelle \(.*\)"` -> `r"(\d+)/(\d+) Faelle.*"` | **NICHT bemerkt** (119/119, 257/260) |
| **A6** | Baumzeile quellbaum `r"   \S+\s+(\d+) Dateien"` -> `r"   \S+\s+(\d+).*"` | **NICHT bemerkt** |
| A7 | `"[FEHLER]" in z` -> `"[FEHLER!]" in z` | BEMERKT (Fall 054) |
| A8 | `or z.startswith("Traceback")` gestrichen | BEMERKT (Fall 052) |
| A10 | `if len(urteile) != 1` -> `> 1` | nicht bemerkt - aber GLEICHWERTIG (die letzte Zeile ist immer selbst eine Urteilszeile, len >= 1) |
| **A11** | `r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B, sha256 gleich"` -> `r"... APK (\d+) B.*"` | **NICHT bemerkt** |

Ursache: `_Mutierer.visit_Constant` mutiert nur ganze Zahlen; Zeichenketten (Regex-Muster, MARKE-Werte, die
Pruef-Literale) werden nie veraendert, und fuer A5/A6/A11 gibt es keinen festen Fall, der die gelockerte Form
ablehnt (z.B. `== SELBSTTEST-OK: 261/261 Faelle ==` ohne Klammer, eine TORSE-Zeile ohne `sha256 gleich`).

b) Am Pruefskript `release/apk_pruefen.sh` bzw. am CLI-Teil des Urteils - Scratch-Kopie von `release/`, `re15_port/` und
`synchro/` als Junctions (nur gelesen), Pins wie ein Entwickler neu gesetzt, dann die ECHTE Kette
`test_r35_android_pruefkette.sh <scratch-repo> <arbeit>`:

| # | Aenderung | Kette |
|---|---|---|
| B0 | keine (Kontrolle der Scratch-Kopie) | rc 0, 0 FALSCH |
| B1 | "zweite Instanz" in `gate_laufen` entfernt (`if (( urteil == 0 ))`-Block) | BEMERKT: `FALSCH N8_urteil_luegt_leere_apk -> 0 (soll rot)` |
| **B2** | in `gate_urteil_selbsttest` die Zeile `(( rc == 0 )) \|\| die "Selbsttest des Gate-Urteils: OK-Schlusszeile, aber Rueckgabe $rc ..."` gestrichen | **NICHT bemerkt** (rc 0, 0 FALSCH) |
| B3 | `gate_urteil.py main()`: `return code` -> `return 0`, Pin neu; der Urteils-Selbsttest bleibt `119/119, 257/260` (main() liegt ausserhalb des Mutators) | BEMERKT durch die Kette: `FALSCH N5a_G1_selbsttest -> 0`, `FALSCH N5b_G1_apk -> 0` |

Bilanz: 12 nicht gleichwertige Aenderungen, **8 bemerkt, 4 nicht (A5, A6, A11, B2)**. Der Kern (F-Y2 `ende(1,`->`ende(0,`,
neue Regel ohne Fall, gestrichene Teilbedingungen, ausgebaute zweite Instanz, CLI gibt immer 0) wird erkannt - das ist
eine echte Verbesserung gegenueber 154a73c1 (dort war das Urteil von nichts geprueft). "Sorgfaeltig mittesten" ist aber
nicht vollstaendig: eine Lockerung eines Urteils-Musters laeuft mit neuem Pin unbemerkt durch Selbsttest und Kette.
Zusaetzlich behaupten README und Dossier mehr, als gemessen wird (M3).

**Urteil Punkt 3: teilweise.**

## 4. RE-Gate

- Spur ist reine Port-Infrastruktur (Android-Entpacker, Release-Pruefkette); die PSX hat weder Entpacker noch APK -
  es gibt kein Original, das zu disassemblieren waere. Im Diff (ohne analysis/) **0** `@0x`-Zitate, dafuer jede Zahl als
  PORT-WAHL mit Herkunft (Kommentar ueber `text_block`, Commit 5c1f0af4, Dossier-Tabelle "Konstanten").
- Stichprobe statt Disassembly (die zitierten Quellstellen SELBST nachgelesen, "zitierte Stelle ist kein Beleg"):
  touch_overlay_pc.c:151 `int u = H / 12; if (u < 8) u = 8;` (Einheit u) - stimmt; :175
  `tp_add_rect(K_RECT, (int)(0.4f * u), ...TP_L1, "L1")` (Rand 0.4u) - stimmt; :483 `penx += 6 * scale;` (Vorschub 6) -
  stimmt; zusaetzlich :470 `if (scale < 1) scale = 1;`, :477 Glyphe 7 Zeilen, android_glue.c:571 `char l2[160];`
  (FORTSCHRITT_ZEILE_MAX 160) - stimmen.
- Rate-Woerter im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO/FIXME/vorerst): **0 Treffer**; keine
  neuen `getenv`-Schalter in platform/engine/include.
- Erklaert der Fix den Befund? Punkt 1 ja (alte Breitenrechnung ohne W-Pruefung, x=-120/-366 reproduziert); Punkt 2 ja
  (alte weg-Schleife ohne Ordnerraeumen, `rename` auf leeren Ordner -> EISDIR, reproduziert in 6 Szenarien).

**Gate: haelt** (PORT-WAHL sauber gekennzeichnet, Zitate korrekt).

## 5. Vertrag / Pfade / Tests

- Echte Aenderungen der Spur = `git diff 154a73c1..HEAD --stat`: 46 Dateien - `analysis/befunde_runde35/N_android*`,
  `re15_port/platform/android/*` (README, build.gradle, android_glue.c, asset_abgleich.c/.h), `release/apk_asset_gate.py`
  + `.sha256`, `release/apk_pruefen.sh`, `release/gate_urteil.py` + `.sha256` (neu),
  `re15_port/tests/unit/probes/r35_android.cmake`, `tests/unit/r35_android/*`, `tests/unit/test_r35_android_abgleich.c`,
  und 1 Zeile in `tests/unit/test_r34a_asset_abgleich.c` (Android-Test der Runde 34a, an R1 angepasst - im eigenen Gebiet).
  **Nicht** beruehrt: `release/RELEASE_NOTES.md`, `tests/unit/CMakeLists.txt`, `tests/integration/CMakeLists.txt`,
  gemeinsame Engine-Dateien. Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots oder Ereignisse belegt (keine Spiellogik).
  (`git diff master --stat` zeigt 200 Dateien, weil master seit 154a73c1 weitergelaufen ist - das sind Aenderungen
  anderer Spuren, nicht dieser.) `git merge-tree --write-tree master HEAD` -> ohne Konflikt.
- Tests: `ctest -R "r35_android|r34a_asset"` -> **5/5 Passed** (unit_r34a_asset_abgleich, unit_r35_android_abgleich,
  unit_r35_android_anzeige, unit_r35_android_konflikt, unit_r35_android_pruefkette). anzeige/konflikt werden mit dem alten
  Code ROT (gemessen, s.o.) - die Tests messen. Hinweis: `unit_r35_android_pruefkette` wird nur registriert, wenn bash +
  Python >= 3.8 gefunden werden (sonst nur `message(STATUS ...)`); auf einer Maschine ohne Python faellt der Test still
  weg - die Schranke RE15_MIN_TESTS faengt das nur, solange sie hoch genug steht.
- Suite: Dossier woertlich `=== LOCAL-BUILD-OK (all) — Tests 482/482` (>= 478). Selbst gefahren:
  `bash re15_port/tools/local_build.sh test` -> `100% tests passed, 0 tests failed out of 482` (1186,5 s) /
  `=== LOCAL-BUILD-OK (test) — Tests 482/482`; kein Fenster-Haken rot, kein Nachfahren noetig.

**Gates Pfad/Tests/Suite: halten.**

## 6. Maengel (nummeriert, nachpruefbar)

**M1 (Punkt 3) - Lockerungen der Urteils-Muster bleiben unbemerkt.** In `release/gate_urteil.py` (urteil()) laufen diese
Einzelaenderungen mit neuem Pin durch `--selbsttest` (`119/119, 257/260`) UND durch ctest `unit_r35_android_pruefkette`:
(A5) `r"(\d+)/(\d+) Faelle \(.*\)"` -> `r"(\d+)/(\d+) Faelle.*"`; (A6) `r"   \S+\s+(\d+) Dateien"` -> `r"   \S+\s+(\d+).*"`;
(A11) `r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B, sha256 gleich"` -> `r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B.*"`.
Ursache: `_Mutierer.visit_Constant` mutiert nur `int`, nie Zeichenketten/Regex-Muster, und `_faelle()` hat keine Faelle
mit gelockerter Form. Abhilfe-Richtung: je Muster ein Gegenbeispiel-Fall (Schlusszeile ohne `(...)`, Baumzeile ohne
` Dateien`, TORSE-Zeile ohne `sha256 gleich`) und/oder Regex-Mutanten (Literal-Wort streichen, `\(.*\)`->`.*`,
`\d+`->`.*`) in den Mutator; danach Mindestzahlen in apk_pruefen.sh nachziehen. Messweg: Textaenderung in einer Kopie,
`"$PY" <kopie> --selbsttest` muss `URTEIL-SELBSTTEST-FEHLER` melden.

**M2 (Punkt 3) - eine Urteilsregel des Pruefskripts ohne Negativ-Kontrolle.** `release/apk_pruefen.sh`
`gate_urteil_selbsttest`: die Zeile `(( rc == 0 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile, aber Rueckgabe
$rc ..."` kann gestrichen werden, ohne dass `unit_r35_android_pruefkette` rot wird (B2: rc 0, 0 FALSCH). Abhilfe-Richtung:
Kontrolle N10 - umgepinntes Urteil, dessen `--selbsttest` die korrekte OK-Schlusszeile druckt, aber mit 1 endet -> Abbruch
verlangt.

**M3 (Doku) - Behauptung groesser als der Mechanismus.** `re15_port/platform/android/README.md` (Urteil-Pin):
"jede Ein-Stellen-Aenderung des eigenen Urteilscodes (260 Mutanten) muss ein Fall erkennen", Dossier Punkt 3:
"Nicht mutiert werden nur Meldungstexte (Format-Argumente, Grund in ende())". Tatsaechlich werden auch alle
Regex-Muster, die MARKE-Tabelle und Pruef-Literale (`"[FEHLER]"`, `"ABBRUCH"`, `"Traceback"`) nicht mutiert, und
main()/urteil_rufen() liegen ausserhalb (B3 faengt nur die Kette). Text an den gemessenen Umfang anpassen (oder M1 so
beheben, dass er stimmt).

Keine Maengel an Punkt 1 und 2. Hinweise ohne Mangelcharakter: echtes APK-A->B-Update mit H8 auf dem Emulator steht
weiter aus (OFFEN im Dossier); pruefkette-Registrierung haengt an Python (s. 5).

## 7. Belege dieser Abnahme (Scratchpad der Abnahme-Sitzung, nicht im Repo)
`n_mess_anzeige.sh` + `n_anz_summary.txt` (68+68 Laeufe), `n_mess_konflikt.sh` + `n_konf_summary.txt` (8 Szenarien x 2
Staende), `n_urteil_aenderungen.py` + `n_urteil_aend.txt` (A1-A11), `n_kette_varianten.sh` + `n_kette_varianten.txt`
(B0-B3), `n_ctest_r35.log` (ctest -V der 5 Tests), `n_suite.log`. Die entscheidenden Aenderungen sind oben woertlich
angegeben und damit ohne diese Dateien wiederholbar.
