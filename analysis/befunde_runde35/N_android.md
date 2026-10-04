# Runde 35 Spur N "android" — Dossier

Baum: `.claude/worktrees/r35_android` (Zweig r35/android, Basis master 154a73c1).
Auftrag (AUFTRAG.md Z. 97-104, woertlich):

> - Die Fortschrittsanzeige beim Entpacken wird auf sehr breiten Displays seitlich abgeschnitten.
> - Wenn ein Update eine Datei durch einen Ordner gleichen Namens ersetzt oder umgekehrt, bricht das
>   Entpacken sauber ab; erst der zweite Start stellt den Stand her.
> - Künftige Änderungen am Prüfskript müssen dessen Urteilslogik selbst sorgfältig mittesten.

Hinweis RE-Gate: Die drei Punkte betreffen ausschliesslich Port-Infrastruktur (Android-Startbild,
Geraete-Entpacker, Release-Pruefskript). Es gibt dafuer KEINE Original-Funktion in RE1.5/RE2 (PSX
hat weder Entpacker noch APK). Belege sind hier Messungen (Geometrie-Rechnung, Temp-Verzeichnis-Lauf,
Mutanten-Lauf des Gates) und Quelltext-Zeilen des Ports, keine @0x-Adressen. Jede Zahl im Code wird
mit ihrer Herleitung (Messung/Zeile) kommentiert.

Status: IN ARBEIT

Herkunft der drei Punkte (Runde 34a, gelesen): Memory `reai-v2-runde34a-android-gate` "Offen (alle niedrig)";
Befunde `analysis/befunde_runde34_android/pruefer_echtlauf_r4_2.md` 3.9 (**E2-1**, Fortschrittsanzeige),
`pruefer_umgehung_r4_2.md` 4.3 H8 + S3 (**F-Y4**, Datei/Ordner-Konflikte im Update-Weg), 4.3 (**F-Y6**, verschluckte
unlink-Fehler der weg-Pfade), 1 (**F-Y1**, Selbsttest-Luecken: 5 nicht-aequivalente Ein-Zeilen-Mutanten ueberleben) und
2 (**F-Y2**, das Urteil `gate_urteil` in apk_pruefen.sh ist selbst nicht geprueft: 1 Zeichen `ende(1,` -> `ende(0,`
macht aus einer richtigen ABWEICHUNG `ANDROID-GATES-OK`; der Urteilstest `nb_urteil_test.sh` lief in keiner Kette).

Plan (Werkzeug statt Nachbildung): ein **Pruefstand** uebersetzt den ECHTEN `platform/android/jni/android_glue.c` +
`asset_abgleich.c` mit Attrappen fuer SDL/JNI/AAssetManager (Vorbild: Pruefstand der Gegenpruefung R4-2,
`pruefer_umgehung_r4_2_belege/pruefstand/`) - jetzt im Repo und in ctest, auch unter mingw. Damit werden Punkt 1
(jede gezeichnete Textzeile gegen die Displaygrenzen, fuer viele Displaygroessen) und Punkt 2 (Update-Szenarien mit
Temp-Ordnern, mehrere Starts je Szenario) am echten Code gemessen - vorher (Stand 154a73c1) und nachher.

## Punkt 1 — Fortschrittsanzeige auf sehr breiten Displays

### Messung vorher
- Geraet (Runde 34a, Emulator 2400x1080 quer, `pruefer_echtlauf_r4_2.md` 3.9 h3/i1): Titel `RE1.5 PORT - ASSETS WERDEN
  EINMALIG GEPRUEFT` (44 Zeichen) erscheint als `1.5 PORT - ASSETS WERDEN EINMALIG GEPRUE`; Fehlertext
  `1 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG` als `IEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEB`.
- Code (android_glue.c:303-320, Stand 154a73c1): `u = H/12`, `fs = u/9`, Zeile 2 `fs-1`, Breite = Zeichen x 6 x fs
  (Glyphe 5x7, Vorschub 6 Spalten: touch_overlay_pc.c:483 `penx += 6 * scale`), x = (W - Breite)/2, keine Begrenzung.
  Rechnung fuer 2400x1080: u=90, fs=10, Titel 44x60 = 2640 px -> x = -120 = genau 2 Zeichen links ("RE") und 2 rechts
  ("FT") abgeschnitten; Fehlertext 57 Zeichen x 54 px = 3078 -> x = -339 -> Zeichen 0..5 ("1 DATE") ganz und 'I' halb
  weg, rechts endet die Sicht bei Zeichen 50 ('B' von DEBUG). Beides deckt sich Zeichen fuer Zeichen mit den Bildern -
  das Modell erklaert den Befund vollstaendig.
- **Pruefstand am echten Code (Stand 154a73c1, mingw, Beleg `N_android_belege/pruefstand_vorher_anzeige.txt`):**
  `PRUEFSTAND-AUSSERHALB Text W=2400 H=1080 x=-120..2510 'RE1.5 PORT - ASSETS WERDEN EINMALIG GEPRUEFT'`,
  `x=-339..2730 '1 DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG'`, `x=-366..2757 'FEHLER: ALTE ASSET-LISTE
  NICHT LOESCHBAR - SIEHE DEBUG.LOG'` - dieselben x-Werte wie die Rechnung, also wie die Geraetebilder. Von 11
  Displaygroessen x 3 Texten sind 27 Laeufe ausserhalb: 2400x1080, 1920x1080, 1280x720, 2560x1080, 3200x1440, 800x480,
  1080x2400, 320x240, 160x120 je alle drei; nur 3840x1080 und 5120x1440 (32:9, H gross genug) passen. Der Fehler haengt
  also nicht an "sehr breit" allein, sondern am Verhaeltnis Textlaenge x 6 x H/108 zu W - 20:9 und 16:9 sind betroffen.

### Beleg / Herleitung (kein RE1.5/RE2-Original: die PSX hat keinen Entpacker - PORT-WAHL)
- Glyphe: 5x7 Punkte, Vorschub 6 Spalten je Skalierung (touch_overlay_pc.c:405-486, `penx += 6 * scale` :483), Kasten
  einer Zeile = (6 x Zeichen - 1) x s breit, 7 x s hoch.
- Einheit u = H/12 (android_glue.c draw_progress, = touch_overlay_pc.c:151 tp_layout); Randabstand der Bedienelemente
  0.4u (touch_overlay_pc.c:175, Schultertasten L1/R1) - derselbe Rand fuer den Text.
- Laengste Texte des Entpackers (grep der fehler_halten-/titel-Literale in android_glue.c): Titel 44 Zeichen
  (`... EINMALIG GEPRUEFT`), Fehler 57 (`%ld DATEIEN KONNTEN ...` mit 1 Datei) und 58 (`FEHLER: ALTE ASSET-LISTE NICHT
  LOESCHBAR - SIEHE DEBUG.LOG`); Fortschrittszeile ~37; Puffer l2 = 160 Zeichen.

### Umsetzung (android_glue.c, Commit 3becd16f)
- `text_block()` neu, `draw_progress` zeichnet beide Zeilen darueber: Skalierung = min(Hoehen-Groesse wie bisher
  (`u/9` bzw. `fs-1`), `(W - 2*0.4u) / (6 * Zeichen)`); nur wenn selbst Skalierung 1 nicht passt, Umbruch an Leerzeichen
  (Zeilenabstand 9 = 7 Glyphe + 2), Titel waechst nach oben, Zeile 2 nach unten (Balken bleibt frei); hoechstens 6 Zeilen.
- Nebenbei `if (frac < 0) ...; if (frac > 1) ...` auf zwei Zeilen (gcc -Wmisleading-indentation, Randnotiz R4-2).

### Messung nachher (Pruefstand, echter Code; Beleg `N_android_belege/pruefstand_nachher_anzeige.txt`)
33/33 Laeufe ohne `PRUEFSTAND-AUSSERHALB`/`-UEBERLAPPUNG`, jeder Text vollstaendig im Bild. Gewaehlte Groessen:
| Display | Titel 44 Z. | Fehler 58 Z. |
|---|---|---|
| 2400x1080 (Geraet E2-1) | s=8, x=144..2248 (vorher s=10, x=-120) | s=6, x=156..2238 (vorher x=-366) |
| 1920x1080 | s=7, x=36..1877 | s=5, x=90..1825 |
| 1280x720 | s=4, x=112..1164 | s=3, x=118..1159 |
| 2560x1080 | s=9 | s=7 |
| 3200x1440 | s=11 | s=8 |
| 3840x1080 / 5120x1440 | s=10 / 13 (unveraendert, Hoehe begrenzt) | s=9 / 12 |
| 800x480 | s=2 | s=2 |
| 1080x2400 hochkant | s=3 | s=2 |
| 320x240 | s=1 einzeilig | s=1, 2 Zeilen |
| 160x120 | s=1, 2 Zeilen | s=1, 3 Zeilen |
Auf breiten Displays, die vorher passten (32:9), aendert sich nichts - nur wo es abschnitt, wird die Schrift kleiner.

## Punkt 2 — Datei<->Ordner-Konflikt im Update

### Messung vorher
- Runde 34a (Linux-Pruefstand, echter Code): H8 Ordner `PSX/q/` (mit `c`) wird Datei `PSX/q`: weg-Schleife loescht
  `q/c`, leerer Ordner `q/` bleibt -> `rename .../PSX/q: Is a directory` -> ABBRUCH; erst der naechste Start (ohne
  Liste -> Waisen-Lauf raeumt `q/`) entpackt. S3/Y4: `PSX/X` + `PSX/X.neu/B` -> Zwischendatei `X.neu` ist ein Ordner ->
  `open` EISDIR -> ABBRUCH bei JEDEM Start (dauerhaft, die Liste ist gueltig).
- Code (android_glue.c:499-545): weg-Schleife `if (unlink(dst) == 0) {...}` ohne else (F-Y6), danach je Eintrag nur
  `unlink(tmp)`, `file_size(dst)` (stat, kein Ordnertest auf dem Weg), `entpacken()` -> `mkdirs_for` (mkdir-Fehler
  verschluckt), `open(tmp)`, `rename(tmp,dst)`. Kein Schritt raeumt einen Ordner auf dem Zielnamen, eine Datei auf
  einem Elternnamen oder einen Ordner auf `<ziel>.neu`.
- **Pruefstand am echten Code (Stand 154a73c1, Beleg `N_android_belege/pruefstand_vorher_konflikt.txt`)**, je Szenario
  Start 1 = APK A frisch, Start 2 = APK B als Update, Start 3 = B noch einmal:
  | Szenario | Start 2 | Start 3 |
  |---|---|---|
  | K1 = H8: Ordner `PSX/q/` (mit c) wird Datei `PSX/q` | EXIT 1, `rename .../PSX/q` scheitert, ABBRUCH | EXIT 0 (voller Lauf "ohne Liste") |
  | K2 = H7: Datei `PSX/q` wird Ordner `PSX/q/c` | EXIT 0 | schneller Weg |
  | K3: verwaister Ordner `PSX/X.neu/` (Zwischendatei-Name), X geaendert | EXIT 1 (`X.neu nicht anlegbar`) | EXIT 0 |
  | K4: verwaiste Datei `PSX/q`, Update bringt `PSX/q/c` | EXIT 1 | EXIT 0 |
  | K5: verwaister Ordner `PSX/q/x/y/` auf dem Namen einer neuen Datei `PSX/q` | EXIT 1 (rename) | EXIT 0 |
  | K6: wie H8, nur Gross/klein (`PSX/Q/c` -> `PSX/q`; NTFS wie Geraet case-insensitiv) | EXIT 1 (rename) | EXIT 0 |
  | L1: Liste mit `PSX/X` + `PSX/X.neu/B` | Geraete-Leser NIMMT AN (bricht erst beim Dateizugriff ab) | - |
  | L2: Liste mit Datei `PSX/q` + `psx/Q/c` | Leser NIMMT AN | - |
  Genau das Nutzerbild: "bricht sauber ab; erst der zweite Start stellt den Stand her" (K1/K3-K6). L1/L2 zeigen,
  warum ein blosses "Konfliktpfad loeschen" ohne Leser-Regel nicht sicher waere: eine Liste darf heute einen Pfad
  zugleich als Datei und als Ordner fuehren - ein Raeumer wuerde dann gelistete Dateien loeschen.

### Beleg / Herleitung (PORT-WAHL, kein Original)
Ursache je Szenario (android_glue.c Stand 154a73c1): K1/K6 - die weg-Schleife (:499-506) loescht nur Dateien, der
leere Ordner `q/` bleibt, `rename(q.neu, q)` (:265) -> EISDIR; K5 - Ordner mit Inhalt auf dem Zielnamen, gleiches
rename; K3 - `unlink(tmp)` (:516) scheitert still an einem Ordner, `open(tmp, O_CREAT)` (:233) -> EISDIR; K4 -
`mkdirs_for` (:182-189) verschluckt das mkdir-Scheitern an der Datei `q`, `open(q/c.neu)` -> ENOTDIR. Heilung im
naechsten Start nur, weil dann die Liste fehlt und `re15_abgleich_waisen` (asset_abgleich.c:527) alles Ungelistete
loescht. Damit ist der Fix vorgegeben: dasselbe Raeumen gezielt fuer den einen Pfad, im laufenden Start.

### Umsetzung (Commit 3becd16f)
- `asset_abgleich.c` (reiner C-Teil, auch im PC-Test): `re15_abgleich_weg_frei(wurzel, rel, melde, ctx)` - Eltern-
  segment, das kein echter Ordner ist -> unlink; echter Ordner auf `<rel>.neu` bzw. `<rel>` -> samt Inhalt loeschen (lstat,
  Symlinks nie verfolgt, Tiefe <= 64 und Pfad < 4096 wie re15_abgleich_waisen); nur fuer Pfade nach
  `re15_abgleich_pfad_ok` (nie ausserhalb der Wurzel). `re15_abgleich_leere_eltern` - leer gewordene Elternordner
  nach dem Loeschen eines weg-Pfads, bis unter das erste Segment.
- Sicherheits-Regeln im Leser `re15_abgleich_lesen`: **R1** kein SEGMENT endet auf `.neu` (vorher nur der ganze Pfad;
  = Abhilfe "D25" der Gegenpruefung R4-2 zu F-Y4), **R2** kein Pfad ist zugleich Ordner eines anderen (ASCII-Gross/
  klein egal, bsearch je Praefix in der gefalteten Liste). Erst damit traegt kein geraeumter Ordner/keine geraeumte
  Datei je etwas Gelistetes. Dieselben Regeln im Gate und in build.gradle (Punkt 3).
- `android_glue.c`: vor JEDEM Entpacken `re15_abgleich_weg_frei` (Meldung `Konflikt geraeumt: <rel> war ...` bzw.
  `FEHLER: Konflikt nicht raeumbar` -> Lauffehler, fail closed wie bisher); weg-Schleife: nach Erfolg
  `re15_abgleich_leere_eltern`; Fehler ausser ENOENT nicht mehr verschluckt (F-Y6): Ordner -> weg_frei, sonst
  `WARNUNG: nicht mehr gelistet, aber nicht loeschbar` + Zaehler. Schlusszeile zusaetzlich `%ld Konflikte geraeumt, %ld
  nicht loeschbar`.

### Messung nachher (Pruefstand, echter Code; Beleg `N_android_belege/pruefstand_nachher_konflikt.txt`)
| Szenario | Start 2 (Update) | Start 3 |
|---|---|---|
| K1 = H8 | EXIT 0, `entfernt ... PSX/q/c` (leerer Ordner q/ per leere_eltern weg), Baum = B | schneller Weg |
| K2 = H7 | EXIT 0 (unveraendert) | schneller Weg |
| K3 Ordner `X.neu/` | EXIT 0, `Konflikt geraeumt: shared_assets/PSX/X.neu war ein Ordner auf dem Namen der Zwischendatei` | schneller Weg |
| K4 Datei `q` auf Ordnername | EXIT 0, `Konflikt geraeumt: shared_assets/PSX/q war eine Datei, wo ein Ordner hin muss` | schneller Weg |
| K5 Ordner `q/x/y/` auf Dateiname | EXIT 0, `... war ein Ordner, wo die Datei hin muss` (2 Dateien) | schneller Weg |
| K6 `Q/c` -> `q` | EXIT 0 | schneller Weg |
| L1 Liste mit `X.neu/B` | EXIT 1, `ungueltig (Zeile 3: unzulaessiger Pfad)`, nichts entpackt | - |
| L2 Liste `q` + `psx/Q/c` | EXIT 1, `ungueltig (Datei und Ordner gleichen Namens: shared_assets/PSX/q / shared_assets/psx/Q/c)` | - |
Jeder Update-Start endet mit SPIELSTART, Speicher bytegleich der APK, kein verwaister Ordner, "zuletzt entpackt" = Liste.

### Tests (ctest, probes/r35_android.cmake)
- `unit_r35_android_konflikt` - die Szenarien oben am echten android_glue.c (Pruefstand `r35_android_pruefstand`).
  Gegenprobe am Stand 154a73c1: K1/K3/K4/K5/K6 Start 2 EXIT 1, L1/L2 angenommen (Beleg vorher).
- `unit_r35_android_abgleich` (63 Pruefungen): R1/R2 im Leser (15 davon scheitern am alten asset_abgleich.c -
  gemessen), F-Y1-Proben, weg_frei (Ordner auf Ziel mit Unterordnern, Ordner auf .neu, Datei auf Elternsegment, nichts
  im Weg, 6 unzulaessige Pfade raeumen nie), leere_eltern (stoppt an nicht leerem Ordner, Baum bleibt).
- `unit_r34a_asset_abgleich`: eine Zeile an R1 angepasst (`a/b.neu/c` jetzt abgelehnt, vorher ausdruecklich erlaubt).

## Punkt 3 — Urteilslogik des Pruefskripts selbst mittesten

### Messung vorher
- Gate-Selbsttest am Stand 154a73c1 (`/c/Python310/python release/apk_asset_gate.py --selbsttest`):
  `== SELBSTTEST-OK: 258/258 Faelle ...`, `Innere Proben: 132/132`, Laufzeit 13.9 s (14.1 s Wand), Rueckgabe 0.
- `gate_urteil` (apk_pruefen.sh:237-349) ist ein Python-Heredoc, kein Test ruft es; die Mindestzahlen
  `GATE_SELBSTTEST_MIN_FAELLE=258/MIN_INNEN=132` (apk_pruefen.sh:72-73). Kein ctest beruehrt release/ (grep
  `apk_asset_gate|selbsttest` in re15_port/tests: 0 Treffer).
- Die fuenf ueberlebenden Gate-Mutanten (F-Y1) entsprechen fehlenden Proben in `_MANIFEST_PROBEN`
  (apk_asset_gate.py:3177-3246): Z/z-Dublette (nur B/b vorhanden), 19-stellige Kopf-ANZAHL (nur Bytes-Feld), CR VOR
  der Kopfzeile / VOR einer Datenzeile (nur dahinter), Listen-Summe GROESSER als die echte (Faelle 228/248 nur kleiner).

- **Gegenprobe F-Y2 am alten Stand** (Beleg `N_android_belege/urteil_vorher_fy2.txt`, Werkzeug: alte apk_pruefen.sh
  @154a73c1 mit dem 1-Zeichen-Fehler `ende(1, "das Gate meldet` -> `ende(0, ...`, echtes gepinntes Gate f53fbbea..,
  APK nur mit Manifest): Gate `== APK-ASSET-GATE-ABWEICHUNG: 3630 Befunde ==`, Rueckgabe 1 -> `gate_laufen ... -> 0`.
  Genau das Bild der Gegenpruefung (A4): ein richtiges Gate wird von einem unbemerkt veraenderten Urteil ueberstimmt.

### Was "das Pruefskript" und "dessen Urteilslogik" sind (Lesart, belegt aus Runde 34a)
Pruefskript = die Asset-Pruefkette `release/apk_asset_gate.py` + `release/apk_pruefen.sh` (gate_festhalten/
gate_laufen/gate_urteil); Urteilslogik = `gate_urteil` (entscheidet aus Rueckgabe UND Ausgabe des Gates OK/Befund/
keine Aussage) und der Selbsttest des Gates. Die zwei offenen Befunde dazu: F-Y2 (Urteil ungeprueft) und F-Y1 (Selbsttest
des Gates erkennt fuenf Ein-Zeilen-Aenderungen nicht). "Kuenftige Aenderungen ... mittesten" verlangt einen Mechanismus,
der bei JEDER spaeteren Aenderung von selbst greift - nicht einen einmaligen Test.

### Umsetzung (Commits b4b43da5, 3f9d8b16, 93caa0d8, 292c0ded)
1. **Urteil ausgelagert und selbstpruefend: `release/gate_urteil.py`** (Code wortgleich zum Heredoc, als Funktion; ein
   unbekannter Modus ergibt jetzt 2 statt Traceback = 1). `--selbsttest`:
   - 119 feste Faelle: gute Laeufe je Modus (selbsttest/apk/quellbaum/paket, auch CRLF, Randwerte 1 Datei/1 Baum/
     Tuerarchive 1), dieselben mit Rueckgabe 1/2, FEHLER/ABWEICHUNG je Modus mit Rueckgabe 1 (-> 1) und 0/2 (-> 2), und je
     Regel ein Gegenbeispiel (Schlusszeile fehlt/falscher Modus, Traceback/ABBRUCH/[FEHLER], zwei Urteilszeilen, n/n,
     Mindestzahlen, Fallnummern, rc != soll, SUMME/Manifest/Tuerarchive/TORSE/unzip-Zaehlung, Baumzeilen ...).
   - **Mutanten des eigenen Urteilscodes** (AST, je Lauf genau eine Aenderung: Vergleich ==/!=, </<=, >/>=, in/not in;
     and/or; not weg; if-Bedingung -> True/False; Zahl +-1 (auch die Urteilszahlen 0/1/2 in ende()); Aufruf weg). Nicht
     mutiert werden nur Meldungstexte (Format-Argumente, Grund in ende()). Erkannt = mindestens ein Fall entscheidet
     anders ODER stuerzt ab, wo das Original begruendet entscheidet. Ergebnis: **260 Mutanten, 257 erkannt, 3 als
     gleichwertig begruendet** (`AEQUIVALENT` mit Begruendung im Code: tuer_soll group(2)->group(1) hinter g1 == g2;
     das ende() fuer "FEHLER-Schluss mit Rueckgabe != 1" - der naechste Schritt entscheidet denselben Fall mit 2;
     `if modus == "paket"` -> True - nur paket erreicht die Stelle). Ein ueberlebender Mutant oder ein veralteter
     AEQUIVALENT-Eintrag = SELBSTTEST-FEHLER. **Wer das Urteil aendert und keinen Fall dazu schreibt, faellt hier.**
   - Laufzeit 4.5 s.
2. **apk_pruefen.sh**: `gate_festhalten` kopiert neben dem Gate auch das Urteil privat, prueft dessen sha256 gegen
   `release/gate_urteil.sha256` (neu) und laesst den Urteils-Selbsttest laufen; die Schlusszeile wird in BASH gegen
   `GATE_URTEIL_MIN_FAELLE=119`, `GATE_URTEIL_MIN_ERKANNT=257`, `GATE_URTEIL_MAX_GLEICH=3` geprueft (ein geschwaechter
   Selbsttest oder mehr "gleichwertige" = Abbruch). `gate_laufen` prueft vor JEDEM Lauf beide Pins und verlangt fuer
   ein OK-Urteil zusaetzlich - unabhaengig vom Urteilscode - **Rueckgabe 0 des Gates und die Urteilszeile**
   (`Gate-Urteil (<modus>, Rueckgabe 0): `) - die Abhilfe "in gate_laufen zusaetzlich rc == 0 verlangen" aus F-Y2.
   Der alte Heredoc ist entfernt (git-Historie @154a73c1).
3. **Gate (F-Y1)**: `_MANIFEST_PROBEN` + Dublette Z/z, Kopf-Anzahl 19 Ziffern, CR vor der Kopfzeile, CR vor einer
   Datenzeile, Zeilenende LF-CR; Fall 259 "Pruefsumme falsch, GROESSER als die echte (ff..ff)"; dazu R1/R2 als Regeln
   (`_pfad_fehler`: Ordner-Segment `.neu`; `manifest_lesen`: Datei und Ordner gleichen Namens) mit Proben und Faellen
   260/261 - Gate und Geraete-Leser lesen wieder nach denselben Regeln (dieselben Proben im C-Test
   `test_r35_android_abgleich.c`). Selbsttest **261/261, Innere Proben 148/148** (vorher 258/132); Pin
   `release/apk_asset_gate.sha256` = `f73c3b1a770d424ddec8c57480227813ee353ea361a96d4e5b6997903bb8f9b6`,
   `GATE_SELBSTTEST_MIN_FAELLE/INNEN` 261/148. build.gradle writeAssetManifest prueft R1/R2 ebenfalls (Bau bricht frueh ab).
4. **ctest `unit_r35_android_pruefkette`** (probes/r35_android.cmake, `r35_android/test_r35_android_pruefkette.sh`):
   faehrt die ECHTE Kette mit dem echten, gepinnten Gate und Urteil bei jedem Suite-Lauf (bash nur Git/MSYS, nie WSL;
   Python ueber python_finden.sh).

### Messung nachher
- Die fuenf Ueberlebenden der Gegenpruefung R4-2 und je ein R1/R2-Mutant gegen das neue Gate (Werkzeug
  `N_android_belege/werkzeug/fy1_mutanten.py`, Beleg `gate_fy1_mutanten.txt`): **7/7 erkannt** - E_manifest_pruefen_gt
  (Fall 259), D06 (Probe Dublette Z/z), D08 (Kopf-Anzahl 19), D13 (CR vor Kopfzeile), D15 (CR vor Datenzeile, LF-CR),
  R1/R2.
- Urteil alt (Heredoc @154a73c1) gegen neu (Werkzeug `urteil_alt_neu.sh`, Beleg `urteil_alt_neu.txt`): 118 feste Faelle +
  4 echte Gate-Ausgaben (Selbsttest 261/261, Quellbaum 3629 Dateien, APK-ABWEICHUNG rc 1, ABBRUCH rc 2) **122 gleich**,
  1 anders = unbekannter Modus (alt Traceback -> 1 "Befund", neu 2 "keine Aussage") - gewollt.
- `unit_r35_android_pruefkette` (54 s): P0 gate_festhalten (`119/119 Faelle, 257/260 Mutanten erkannt, 3 gleichwertig`),
  P1 Gate-Selbsttest -> 0 (`SELBSTTEST-OK 261/261 ... 148/148`), P2 Quellbaum -> 0 (`3629 Dateien in 5 Baeumen`);
  Negativ-Kontrollen alle ROT: N1 0-Byte-APK -> 2, N2 leeres ZIP -> 2, N3 nur Manifest -> 1 (ABWEICHUNG), N4 Zufallsbytes
  -> 2, N5 kaputtes Gate (main() ohne sys.exit) umgepinnt -> 2/2, **N6 Urteil mit dem F-Y2-Fehler umgepinnt -> Abbruch**
  (`URTEIL-SELBSTTEST-FEHLER: 8 von 119 Faellen falsch`), N7 Urteil 0 Byte -> Abbruch, **N8 Urteil luegt komplett
  (Selbsttest-OK gefaelscht, sagt immer 0) -> die zweite Instanz ueberstimmt (`Urteil 0, aber das Gate gab Rueckgabe 1`)
  -> 2**, N9 Mindestzahl Mutanten hochgesetzt -> Abbruch; Gate/Urteil/Pins danach unveraendert.

## OFFEN
(folgt)
