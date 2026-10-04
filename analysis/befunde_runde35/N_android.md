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

Status: FERTIG (Abschluss unten); Nachbesserung 1 nach Abnahme 0 am Ende (M1-M3: Urteil-Mutator mutiert jetzt auch
Zeichenketten/Regex-Muster, 243 Faelle / 781 von 785 Mutanten, 32/32 simulierte Aenderungen; bash-Urteil mit
Kontrollen N10-N21, Streich-Messung 17/17; Doku auf den gemessenen Umfang). Kurz: (1) Schrift aus Hoehe UND Breite - auf dem
Emulator 2400x1080 Titel und laengster Fehlertext vollstaendig; (2) Datei<->Ordner-Konflikte werden im selben Start
geraeumt (Pruefstand am echten Code + Emulator), abgesichert durch Leser-Regeln R1/R2 in Geraet, Gate und Gradle;
(3) das Urteil ist ausgelagert, gepinnt, an 119 Faellen und 260 eigenen Mutanten selbstgeprueft, vor jeder Nutzung und
in ctest; eine zweite Instanz verhindert, dass ein Urteil ein richtiges Gate ueberstimmt; die fuenf F-Y1-Mutanten des
Gates werden jetzt erkannt.

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
     mutiert werden nur Meldungstexte (Format-Argumente, Grund in ende()) [Abnahme 0, M3: zu weit - Zeichenketten, Regex-Muster, MARKE-Tabelle und Pruef-Literale wurden in diesem Stand NICHT mutiert, main()/urteil_rufen() lagen ausserhalb; gemessener Umfang ab Nachbesserung 1 unten]. Erkannt = mindestens ein Fall entscheidet
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

## APK-Bau und Emulator (echte libmain.so, alle drei Punkte)
- **APK-Bau** `release/build_android.sh --version v0.8.22-r35test` (Stand 292c0ded, danach nur Kommentar/Dossier;
  Beleg `N_android_belege/apk_bau_auszug.txt`): BUILD SUCCESSFUL 3m17s, versionName `v0.8.22-r35test`, Signer
  `432bc749...` = Pin, Gate-Pin `f73c3b1a...`, **Urteil selbstgeprueft 119/119 + 257/260**, Urteils-Pin `2b78069e...`,
  Gate-Selbsttest 261/261 + 148/148, `APK-ASSET-GATE-OK: 3629 Dateien ... = unzip-Zaehlung - 1`, `APK-PRUEFUNG-OK`,
  APK `06c8e01c862d28d6451ad520f6531c8bf168e6c012aa739f7d63ef1b84a31ffe`, 364777306 B. = Positiv-Kontrolle der neuen
  Kette an einer echten APK. APK danach geloescht, `release/SHA256SUMS_android.txt` zurueckgesetzt (nicht committet).
- **Emulator** (eigene AVD-Kopie `Medium_Phone_API_36_r35n` im Scratchpad: config.ini des Nutzer-AVD mit den 5
  Aenderungen der Runde 34a, Port 5600, `-wipe-data`, Nutzer-AVD nicht gestartet; Android 16, 1080x2400, App quer =
  2400x1080; Werkzeug `N_android_belege/werkzeug/geraet_r35n.sh`, Belege `N_android_belege/geraet/`):
  - **E1** Uebergang (alter Marker `re15_assets_ok.txt` im Speicherordner): Bild `E1_uebergang_3s.png` (ANGESEHEN):
    `RE1.5 PORT - ASSETS WERDEN EINMALIG GEPRUEFT` **vollstaendig**, x ~142..2248 von 2400 (Pruefstand: 144..2248), darunter
    Balken und `518 / 3629 DATEIEN  (9%)  32 MB`. Runde 34a auf demselben AVD-Typ: `1.5 PORT - ... GEPRUE`.
    logcat: `Abgleich (Uebergang v0.8.19) ... pruefen 3629` -> `Entpacken fertig ... 0 Konflikte geraeumt ... 0 Fehler`.
  - **E2** "zuletzt entpackt" als Ordner: Bild `E2_fehler_liste.png` (ANGESEHEN): `FEHLER: ALTE ASSET-LISTE NICHT
    LOESCHBAR - SIEHE DEBUG.LOG` (58 Zeichen, der laengste Text) **vollstaendig**, x ~154..2239 (Pruefstand 156..2238).
  - **E3** (der Fall i1 der Runde 34a, damals `ABBRUCH`): Ordner `synchro/STAGE1/room1240/main04.wav.neu/` mit Inhalt +
    `main04.wav` geloescht, Liste "zuletzt entpackt" vorhanden -> **im selben Start**: `entpacke .../main04.wav`,
    `Konflikt geraeumt: synchro/STAGE1/room1240/main04.wav.neu war ein Ordner auf dem Namen der Zwischendatei mit 1
    Dateien darin - entfernt`, `Entpacken fertig (Groessen-Nachlauf): ... 1 kopiert ... 1 Konflikte geraeumt ... 0 Fehler`,
    `main04.wav` 2017588 B wieder da.
  Emulator danach beendet (`emu kill`).

## Konstanten (alle PORT-WAHL; kein RE1.5/RE2-Original, die PSX las von CD - Herkunft je Zahl)
| Konstante | Wert | Herkunft |
|---|---|---|
| Rand der Textzeilen | 0.4u, u = H/12 | Randabstand der Bedienelemente touch_overlay_pc.c:175 (L1/R1), Einheit touch_overlay_pc.c:151 |
| Vorschub je Zeichen | 6 x Skalierung | Glyphe 5x7 + 1 Spalte, touch_overlay_pc.c:483 `penx += 6 * scale` |
| Zeilenabstand beim Umbruch | 9 x Skalierung | Glyphe 7 Zeilen hoch (touch_overlay_pc.c:477) + 2 frei (PORT-WAHL, nur unter Skalierung 1 benutzt) |
| Zeilen je Block | <= 6 | PORT-WAHL; laengster Text 58 Zeichen braucht bei 160x120 3 Zeilen (gemessen) |
| Zeile max. | 160 Zeichen | Puffer `l2[160]` in re15_android_bootstrap_assets |
| Raeum-Tiefe / Pfadlaenge | 64 / 4096 | wie re15_abgleich_waisen (asset_abgleich.c WAISEN_TIEFE/WAISEN_PFAD) |
| Gate-Mindestzahlen | 261 Faelle / 148 Proben | gemessen: Selbsttest 261/261, 148/148 |
| Urteils-Mindestzahlen | 243 Faelle / 781 erkannt / <= 4 gleichwertig (Nachbesserung 1; vorher 119/257/3) | gemessen: URTEIL-SELBSTTEST-OK 243/243, 781/785, 4 |

## Tests (alle in re15_port/tests/unit/probes/r35_android.cmake)
| ctest | misst | Punkt |
|---|---|---|
| unit_r35_android_anzeige | echter android_glue.c im Pruefstand, 11 Displaygroessen x 3 Texte: jede Zeile/jedes Rechteck im Bild, keine Ueberlappung, ganzer Text sichtbar | 1 |
| unit_r35_android_konflikt | echter android_glue.c: K1-K6 Update im selben Start fertig, Baum = APK, Start 3 schneller Weg; L1/L2 fail closed | 2 |
| unit_r35_android_abgleich | asset_abgleich.c: R1/R2 (15 Pruefungen scheitern am alten Stand), F-Y1-Proben, weg_frei/leere_eltern im Temp-Ordner | 2 (+3) |
| unit_r34a_asset_abgleich | (bestehend) eine Zeile an R1 angepasst | 2 |
| unit_r35_android_pruefkette | echte Kette apk_pruefen.sh + Gate + Urteil: Positiv P0-P3, Negativ N1-N21 muessen ROT werden (N10-N21 = je Pruefzeile des bash-Urteils, Nachbesserung 1) | 3 |

## OFFEN
- Ein Update-Lauf (APK A -> APK B) mit dem H8-Muster auf dem Emulator ist nicht gemacht (braucht eine zweite, echt
  gebaute APK mit anderem Baum, Sandbox wie `pruefer_echtlauf_r4_2.md` 3.0). Der Update-Weg ist am ECHTEN
  android_glue.c im Pruefstand gemessen (mingw/NTFS und Linux/ext4), der Raeum-Mechanismus selbst auf dem Emulator (E3).
- (erledigt) Linux-Lauf: Pruefstand, beide C-Tests und beide Szenario-Teile im Container `re15-linux-build:deb11`
  (gcc 10.2, ext4/overlay = case-SENSITIV, uid 0) - 0 Warnungen, `OK: 63 Pruefungen`, r34a 324/0, anzeige + konflikt
  bestanden, dieselben Konflikt-Meldungen (Beleg `N_android_belege/pruefstand_linux.txt`). Nicht gelaufen: die
  Rechte-Faelle (als root ohne Aussage) - Konflikt "nicht raeumbar" ist nur ueber den Code-Pfad (`FEHLER: Konflikt nicht
  raeumbar` -> n_fehler -> fail closed wie jeder Dateifehler) belegt.

## Fuer den Nutzer
- Keine neuen Sprachzeilen, keine neuen Assets (shared_assets unveraendert) - nichts fuers Paket-Gate nachzutragen.
- Android: beim ersten Start nach dem Update ist der Titel auf 20:9/16:9 kleiner, steht aber ganz im Bild; ein Update,
  das eine Datei durch einen gleichnamigen Ordner ersetzt (oder umgekehrt), laeuft im selben Start durch
  (debug.log: `Konflikt geraeumt: ...`).
- Fuer den Orchestrator (Paketbau): `release/gate_urteil.py` + `release/gate_urteil.sha256` sind neu und Pflicht
  (apk_pruefen.sh bricht ohne sie ab). Wer `release/apk_asset_gate.py` aendert (z.B. PFLICHT_DATEI fuer neue Assets
  anderer Spuren): `"$PY" release/apk_asset_gate.py --selbsttest` (jetzt 261/261, 148/148) und den neuen sha256 in
  `release/apk_asset_gate.sha256` - wie bisher. Wer `release/gate_urteil.py` aendert: `--selbsttest` muss
  `URTEIL-SELBSTTEST-OK` melden (sonst einen Fall fuer die neue Regel schreiben), dann sha256 in
  `release/gate_urteil.sha256` und ggf. `GATE_URTEIL_MIN_*` (Stand Nachbesserung 1: 243 / 781 / <= 4,
  Pin `c8fed5ca...`). Wer eine Pruefzeile des bash-Urteils in apk_pruefen.sh aendert: eine Kontrolle in
  test_r35_android_pruefkette.sh (Werkzeug fuer die Streich-Messung: N_android_belege/werkzeug/kette_streichen.sh). Merge-Hinweis: diese Spur aendert apk_asset_gate.py (Proben/Faelle/R1/R2) und dessen
  Pin - traegt der Orchestrator parallel PFLICHT_DATEI-Zeilen ein, danach Selbsttest + Pin neu.
- Jede vor diesem Stand gebaute APK besteht die Kette weiter nur, wenn ihr Manifest R1/R2 erfuellt (alle bisherigen:
  kein Segment `*.neu`, keine Datei+Ordner-Paare - am Quellbaum geprueft: `--quellbaum` 3629 Dateien OK).

## Abschluss
Suite im eigenen Baum (`bash re15_port/tools/local_build.sh all`, nach Emulator-Ende, ohne Parallel-Last dieser Spur):

    === LOCAL-BUILD-OK (all) — Tests 482/482

(478 vorher + 4 neue: unit_r35_android_abgleich, unit_r35_android_anzeige, unit_r35_android_konflikt,
unit_r35_android_pruefkette.) Keine Fenster-Haken rot, kein Nachfahren noetig.

## Nachbesserung 1 (nach Abnahme 0, `N_abnahme_0.md`: Punkt 1 + 2 erfuellt, Punkt 3 teilweise, Maengel M1-M3)

Stand vor der Nachbesserung: HEAD b19c39fa (= 5c1f0af4 + Abnahmebericht). Plan:
- M1: der Mutator von `release/gate_urteil.py` mutiert auch Zeichenketten (Regex-Muster, MARKE-Tabelle, Pruef-Literale)
  mit festgelegten Operatoren; fuer jeden dann ueberlebenden Mutanten ein fester Gegenbeispiel-Fall (mit Soll) oder eine
  begruendete Gleichwertigkeit. Messweg wie die Abnahme: A5/A6/A11 als Textaenderung in einer Kopie -> `--selbsttest`
  muss `URTEIL-SELBSTTEST-FEHLER` melden.
- M2: Negativ-Kontrolle N10 (umgepinntes Urteil: richtige OK-Schlusszeile, Rueckgabe 1 -> Abbruch verlangt) und fuer
  JEDE weitere Pruefung in `gate_urteil_selbsttest` eine eigene Kontrolle; dazu eine Streich-Messung aller
  Urteils-Pruefzeilen in apk_pruefen.sh gegen die Kette.
- M3: README + Dossier auf den gemessenen Umfang.

### M1 - Lockerungen der Urteils-Muster blieben unbemerkt (Punkt 3)
**Ursache (Code gelesen, b19c39fa):** `_Mutierer.visit_Constant` in `release/gate_urteil.py` brach bei jeder
Nicht-Ganzzahl ab (`if isinstance(node.value, bool) or not isinstance(node.value, int): return node`) - Regex-Muster,
MARKE-Tabelle und Pruef-Literale wurden NIE veraendert, und `_faelle()` hatte keine Faelle mit gelockerter Zeilenform
(Schlusszeile ohne Klammerteil, Baumzeile ohne " Dateien", TORSE-Zeile ohne "sha256 gleich"). Kein RE-Original
(reine Port-Infrastruktur); Beleg = Quelltextzeilen + Messung.

**Messung vorher** (Werkzeug `N_android_belege/werkzeug/urteil_aenderungen.py`: je simulierte Aenderung EINE
Textaenderung in einer Kopie, dann `--selbsttest`; A* = woertlich die der Abnahme, C* = eigene Lockerungen, D* =
Aenderungen ausserhalb jeder Mutations-Art): A5, A6, A11 NICHT bemerkt (`URTEIL-SELBSTTEST-OK: 119/119 Faelle,
257/260 Mutanten`) - die Abnahme ist reproduziert; dazu C1 (Innere Proben ohne Klammerteil), C2 (`\s+` -> `\s*`),
C3/C4/C7 (Wort -> `.*`), C10 (Text hinter der Paket-Baumzeile), C11 (`\b` weg), C15 (Zahl -> `.*`), C18 (Leerzeichen
weg) ebenfalls nicht. Beleg `N_android_belege/nb1_urteil_aenderungen_vorher.txt` (Lauf gegen das Urteil @b19c39fa).

**Aenderung (release/gate_urteil.py):**
- Mutator: JEDE Zeichenkette ausserhalb der Meldungstexte bekommt einen Mutanten (+ ein fremdes Zeichen) - MARKE-Werte
  und -Schluessel, Modusnamen, "[FEHLER]"/"ABBRUCH"/"Traceback"/"ok", "\r"/"\n"/"" in replace/split, die
  Tuerarchiv-Labels. Meldungstext = NUR das zweite Argument von `ende()` und `genau_eine()`; die zwei Meldungen, die
  vorher ausserhalb lagen (`tuer_soll` gab einen Text zurueck, `zusatz = " = unzip-Zaehlung - 1"`), stehen jetzt im
  Grund von `ende(0, ...)` (gleiches Urteil, nur der Grundtext wird anders zusammengesetzt).
- Jedes Regex-Muster (erstes Argument von re.fullmatch/re.match/re.search/genau_eine, auch links von `%`) bekommt
  die Lockerungen R0-R6 (`_regex_lockerungen`, Stuecke auf oberster Ebene aus `_regex_stuecke`):
  R0 Text hinter dem Muster erlaubt (nur fullmatch, nicht bei Endung `.*`/`(.*)`), R1 Rest ab Stueck k -> `.*`
  (= Art von A5/A6/A11), R2 Wort -> `.*`, R3 `\d+` -> `.*`, R4 `\s+` -> `\s*`, R5 Einzelzeichen weg, R6 Alternative
  einer Gruppe weg.
- Zwei Vergleiche als Zahlen statt als Text: `tuer_soll` `int(g1) != int(g2)` und Fallzeile `int(rc) != int(soll)`.
  Grund: als Text verglichen liess sich `\d+` -> `.*` in g1 bzw. rc nicht von einem Fall unterscheiden (gleicher Text
  nur aus Ziffern), mit int() endet die Lockerung bei einer Nicht-Zahl in einer Ausnahme (erkannt). Das Gate druckt
  diese Felder mit `%d` (apk_asset_gate.py `_tuer_zeilen_drucken` :777 und Fallzeile :3441) - fuer jede echte Ausgabe
  dasselbe Urteil.
- Faelle 119 -> 243 (`_faelle_muster` + 5 in `_faelle`): je gelesene Zeile (a) letztes Zeichen weg / falscher Schluss,
  (b) je Literal-Wort ein anderes, (c) je Zahlfeld 'x', (d) ohne Pflicht-Zwischenraum, (e) Text hinter der Zeile,
  (f) Pflicht-Leerzeichen weg; dazu der echte Wortlaut des Gates bei Abweichung `TORSE.VBS ... sha256 NICHT
  gleich/nicht geprueft` (apk_asset_gate.py pruefen() :1395-1397), je Alternative der Urteilszeilen eine fremde Zeile,
  "[FEHLER]" tiefer eingerueckt. Alle mit festem Soll (meist 2), zwei mit Soll 0 ("== SELBSTTEST-OKAY/-HINWEIS: x =="
  ist keine Urteilszeile - so ist die Regel `-(OK|FEHLER|ABWEICHUNG)\b` festgehalten).
- AEQUIVALENT +1 (jetzt 4): `\[(ok|FEHLER)\]` ohne FEHLER - eine [FEHLER]-Fallzeile endet schon in der Schleife davor
  (`"[FEHLER]" in z`) mit 2.
- Laufzeit: der Selbsttest parst je Mutant nur den Quelltext von urteil() neu statt deepcopy des AST und zerlegt die
  Quelle einmal (vorher get_source_segment ueber die ganze Datei je Mutant): 785 Mutanten in ~4 s (vorher 260 in 5 s).
- Ausgabe: neue Zeile `Mutanten je Operator: ...` (Zaehllauf; muss die Mutantenzahl ergeben, sonst FEHLER).

**Messung nachher:** `URTEIL-SELBSTTEST-OK: 243/243 Faelle, 781/785 Mutanten erkannt, 4 als gleichwertig begruendet`,
`Mutanten je Operator: Aufruf 28, BoolOp 10, Regex R0 13, Regex R1 261, Regex R2 49, Regex R3 30, Regex R4 9, Regex R5
115, Regex R6 11, Vergleich 39, Zahl 118, Zeichenkette 37, if-Bedingung 56, not 9`. Simulierte Aenderungen: im
Zwischenstand (nach R0-R5, vor R6) 29 von 32, D1 (`"[FEHLER]" in z` -> `startswith("   [FEHLER]")`) und D5
(Alternative ABWEICHUNG aus dem Urteilszeilen-Muster) NICHT (Beleg `nb1_urteil_aenderungen_zwischenstand.txt`) - dafuer
der Fall "[FEHLER] tiefer eingerueckt", die Alternativen-Faelle und Operator R6. **Endstand 32 von 32 bemerkt**, darunter
A5 (3 Faelle falsch), A6 (3), A11 (4) - Beleg `nb1_urteil_aenderungen_nachher.txt`. Pin `release/gate_urteil.sha256` =
`c8fed5cab7016a9d699b0b1dc90dc1532e7f1356ec0219a801d911cbc0a25ba8`, `GATE_URTEIL_MIN_FAELLE/ERKANNT/MAX_GLEICH` =
243/781/4 (apk_pruefen.sh).

**Gegenprobe "aendert die Nachbesserung ein Urteil?"** (Werkzeug `werkzeug/urteil_alt_neu_nb1.py`): das Urteil
@b19c39fa und das neue Urteil auf allen 243 Faellen des neuen Selbsttests -> `243 Faelle, 243 gleiches Urteil, 0 anders`
(Beleg `nb1_urteil_alt_neu.txt`). Die neuen Faelle halten also nur fest, was das Urteil schon tat; die echten
Gate-Ausgaben (P1 Gate-Selbsttest 261/261, P2 Quellbaum 3629 Dateien, N3 ABWEICHUNG rc 1) urteilen in der Kette wie
vorher.

### M2 - Pruefzeile `(( rc == 0 )) || die` ohne Negativ-Kontrolle (Punkt 3)
**Ursache:** `test_r35_android_pruefkette.sh` hatte fuer `gate_urteil_selbsttest` nur N6/N7 (keine gueltige
Schlusszeile) und N9 (Mindestzahl erkannt ueber die echte Datei); fuer `rc != 0` bei richtiger Schlusszeile, f1 != f2,
Mindestzahl Faelle, Summe erkannt + gleich, Hoechstzahl gleichwertig, die Anker des bash-Musters, den Zweig "ohne
Urteilszeile" der zweiten Instanz und die Pin-Pruefung VOR JEDEM Lauf gab es keine Kontrolle.

**Aenderung (test_r35_android_pruefkette.sh, nur Test):** Attrappe des Urteils (`attrappe`: druckt bei --selbsttest
genau die vorgegebenen Zeilen, endet mit der vorgegebenen Rueckgabe; die Zahlen kommen aus den Mindestzahlen von
apk_pruefen.sh, `okz`), umgepinnt wie N6. **P3** = alles richtig -> MUSS angenommen werden (sonst waeren die Abbrueche
wertlos). Je Kontrolle genau EINE verletzte Bedingung: **N10** richtige OK-Zeile + Rueckgabe 1 (= M2), N11 f1 != f2,
N12 f < Mindestzahl, N13 erkannt + gleich != alle, N14 erkannt < Mindestzahl, N15 gleichwertig > Hoechstzahl, N16
OK-Zeile nicht zuletzt, N17 Zusatz dahinter, N18 Text davor; **N19a/b** Urteil 0 ohne Urteilszeile bzw. mit der eines
anderen Modus bei Gate-Rueckgabe 0 (Gate = umgepinnte Attrappe G0) -> rot ueber den Zweig "ohne Urteilszeile";
**N20/N21** private Kopie von Urteil bzw. Gate NACH gate_festhalten um eine Kommentarzeile veraendert -> gate_laufen
bricht mit "... ist NICHT das festgehaltene" ab. Nebenbefund: N8 hatte die Schlusszeile "119/119 ... 257/260" fest im
Text - mit den neuen Mindestzahlen waere N8 am Selbsttest statt an der zweiten Instanz rot geworden (die eigene Pruefung
"ueberstimmt (zweite Instanz)" hat das sofort gemeldet: Lauf `kette1`, `FALSCH N8 ohne 'ueberstimmt (zweite
Instanz)'`); jetzt kommt auch dort die Zeile aus `okz`.

**Messung Kette nachher** (echter Baum, `bash test_r35_android_pruefkette.sh`, 77 s): P0-P3 ok, N1-N21 alle wie
verlangt, `FEHLER=0`. Auszug: `N10_ok_zeile_rueckgabe_1 -> Abbruch: DIE: Selbsttest des Gate-Urteils:
OK-Schlusszeile, aber Rueckgabe 1`, `N19a ... Gate-Urteil ohne Urteilszeile '   Gate-Urteil (selbsttest, Rueckgabe 0):
...' (zweite Instanz)`, `N20_urteil ... DIE: Gate-Urteil ist NICHT das festgehaltene`, `N21_gate ... DIE: Asset-Gate
ist NICHT das festgehaltene`.

### M3 - Doku behauptete mehr, als der Mechanismus leistete
**Ursache:** README "jede Ein-Stellen-Aenderung des eigenen Urteilscodes (260 Mutanten) muss ein Fall erkennen" und
Dossier Punkt 3 "Nicht mutiert werden nur Meldungstexte" - tatsaechlich wurden (Code @b19c39fa, `visit_Constant`, s. M1)
keine Zeichenketten/Regex-Muster/MARKE-Werte/Pruef-Literale mutiert, und main()/urteil_rufen() lagen ausserhalb.
**Aenderung:** M1 behoben UND den Text an den gemessenen Umfang angepasst - an vier Stellen derselbe Wortlaut:
Kopf von `release/gate_urteil.py` (Operatoren-Liste + Abschnitt "NICHT mutiert: Meldungstexte (zweites Argument von
ende() und genau_eine()), urteil_rufen(), main(), der Selbsttest selbst; Aenderungen ausserhalb der Operatoren - z.B.
any -> all - faengt nur die Fallsammlung"), Docstring `_Mutierer`, Kopf von `release/apk_pruefen.sh` und
`re15_port/platform/android/README.md` (Urteil-Pin: 243 Faelle, 785 Mutanten / 781 erkannt / 4 begruendet, nicht
mutierte Teile, 32/32 simulierte Aenderungen mit Belegpfad). Die alte Dossier-Aussage in Punkt 3 ist an Ort und Stelle
als zu weit markiert. Was main()/urteil_rufen() angeht: deren Fehler faengt die Kette (Abnahme B3: `return code` ->
`return 0` -> N5a/N5b FALSCH), nicht der Mutant - so steht es jetzt auch im Text.

### M2 - Messung vorher/nachher (Streich-Messung gegen die ECHTE Kette)
- **Vorher** (Stand b19c39fa komplett: apk_pruefen.sh, Gate, Urteil, Pins und der alte test_r35_android_pruefkette.sh,
  in apk_pruefen.sh nur `(( rc == 0 )) || die "Selbsttest des Gate-Urteils: OK-Schlusszeile ...` -> `true || die ...`):
  Kette `FEHLER=0`, `rc=0` - B2 der Abnahme reproduziert. Beleg `N_android_belege/nb1_kette_b2_vorher.txt`.
- **Nachher**: Werkzeug `werkzeug/kette_streichen.py` + `.sh` (je Variante eine Kopie von release/ mit GENAU EINER
  gestrichenen/entschaerften Pruefzeile, re15_port/synchro als Junction - nur mit `cmd /c rmdir` entfernt; die echte
  Kette je Variante). Kontrolle B0 (unveraenderte Kopie) `KONTROLLE-OK rc=0 FALSCH=0`; **17 von 17 Aenderungen
  bemerkt**: B1 zweite Instanz ganz aus (N8, N19a/b), **B2 = M2 (N10)**, B3 Zweig rc != 0 (N8), B4 Zweig ohne
  Urteilszeile (N19a/b), B5 f1 == f2 (N11), B6 Mindestzahl Faelle (N12), B7 e + g == m (N13), B8 Mindestzahl erkannt
  (N9, N14), B9 Hoechstzahl gleichwertig (N15), B10 Endanker `$` (N17), B11 Anfangsanker `^` (N18), B12 "letzte Zeile"
  -> "letzte URTEIL-Zeile" (N16), B13 Urteils-Pin-Vergleich (N20), B14 Gate-Pin-Vergleich (N21), B15 Urteils-Pruefung
  vor jeder Nutzung an beiden Stellen (N20), B16 Gate-Pin vor jedem Lauf (N21), B17 kein Abbruch bei ungueltiger
  Schlusszeile (N6, N7, N16 - Abbruch nur noch spaeter, mit falscher Meldung). Beleg
  `N_android_belege/nb1_kette_streichen_nachher.txt`.
- Hinweis zur Messung: der erste Anlauf legte keine Junctions an (`cmd //c mklink /J` -> MSYS machte aus `/J` einen
  Pfad, "Ungueltige Option"); B0 war rot (`build.gradle fehlt`) und hat das sofort gezeigt - Anlauf verworfen, mit
  `MSYS_NO_PATHCONV=1` neu. Der abgebrochene Lauf lebte nach TaskStop weiter (MSYS-Prozessbaum); nur dessen eigene
  Nachfahren (Elternkette ab der eigenen PID) beendet, nichts Fremdes.

### Nachbesserung 1 - Tests und Querpruefungen
- `release/gate_urteil.py --selbsttest` unter Python 3.10.11 (python_finden.sh), 3.9.0 und 3.14.7 (msys64): jeweils
  `== URTEIL-SELBSTTEST-OK: 243/243 Faelle, 781/785 Mutanten erkannt, 4 als gleichwertig begruendet ==` (~4 s).
- `release/apk_asset_gate.py` unveraendert (Pin `f73c3b1a...`, Selbsttest 261/261, 148/148 - in der Kette P1).
- ctest `unit_r35_android_pruefkette` = die Kette oben (P0-P3, N1-N21); die anderen r35_android-Tests (anzeige,
  konflikt, abgleich) und Punkt 1/2 sind von der Nachbesserung nicht beruehrt (kein C-Code geaendert).
- Geaenderte Dateien: release/gate_urteil.py (+ .sha256), release/apk_pruefen.sh (Mindestzahlen + Kopfkommentar, keine
  Pruefzeile geaendert), re15_port/tests/unit/r35_android/test_r35_android_pruefkette.sh, probes/r35_android.cmake
  (nur Kommentar), re15_port/platform/android/README.md, Dossier + Belege. Nicht: RELEASE_NOTES.md, make_package.sh,
  tests/*/CMakeLists.txt, Engine.

### Nachbesserung 1 - Abschluss
Suite im eigenen Baum (`bash re15_port/tools/local_build.sh all`, nach Ende aller Messlaeufe dieser Spur; 1475 s):

    === LOCAL-BUILD-OK (all) — Tests 482/482

`unit_r35_android_pruefkette` Passed 126,9 s (vorher 54-94 s; +P3/N10-N21, Urteils-Selbsttest jetzt ~4 s je Lauf),
`unit_r35_android_abgleich/anzeige/konflikt`, `unit_r34a_asset_abgleich` Passed. Kein Fenster-Haken rot, kein
Nachfahren noetig.

OFFEN (unveraendert aus dem Bau, kein Mangel der Abnahme): echtes APK-A -> APK-B-Update mit dem H8-Muster auf dem
Emulator (braucht zweite, echt gebaute APK); Rechte-Fall "Konflikt nicht raeumbar" hat die Abnahme unter NTFS gemessen
(fail closed). Zum Urteil: Aenderungen ausserhalb der Mutations-Operatoren deckt nur die Fallsammlung (gemessen 32/32,
nicht bewiesen fuer jede denkbare Aenderung) - steht so in Kopf/README. Kein APK-Neubau in der Nachbesserung: kein
C-/Gradle-Code geaendert, die Kette ist dieselbe Funktion mit strengerem Selbsttest (Positiv-Kontrolle P0-P3 am
echten Gate + Quellbaum in jedem Suite-Lauf).

## Nachbesserung 2 (nach Abnahme 1, `N_abnahme_1.md`: Punkt 1 + 2 erfuellt, Punkt 3 teilweise, Maengel M1-M3)

Stand vor der Nachbesserung: HEAD 9e535728 (= 575c964d + Abnahmebericht), Arbeitsbaum sauber. Der Vorgaenger wurde um
12:00 vom Sitzungslimit beendet, bevor er hier etwas geschrieben hatte - diese Sitzung beginnt bei null.
Punkte 1 und 2 sind laut Abnahme 1 erfuellt und werden NICHT angefasst (kein C-Code, kein platform/android/-Code).
Plan (nur Punkt 3, "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten"):
- M1 (Python-Urteil, einseitige Lockerungen von `!=`): Mutations-Operator `TAUSCH` in `release/gate_urteil.py` auf
  einseitige Lockerungen in BEIDE Richtungen erweitern (`!=` -> `<` UND `>`, `==` -> `<=` UND `>=`, zusaetzlich zu
  `==`<->`!=`, `<`<->`<=`, `>`<->`>=`); bei Vergleichen mit einer Zeichenkette keine Ordnungs-Mutanten (eine Ordnung
  auf Texten ist keine "Lockerung"). Dann fuer jeden ueberlebenden Mutanten einen Fall von der ANDEREN Seite (Summe > n,
  len > k, qq < gg, ok_n > n, g1 > g2, a > b mit b >= Mindestzahl, mb < b, je Glied einer Vergleichskette beide Seiten)
  oder eine begruendete Gleichwertigkeit. Mindestzahlen nachziehen.
- M2 (Tuer-Soll-Regel nur im Meldungstext): die Pruefung wird eine eigene Anweisung `tuer_soll()` vor `ende(0, ...)` in
  apk/quellbaum/paket (Mutant "Aufruf weg" je Stelle); die Faelle "Tuer-Soll fehlt" entfernen nur noch die Zeilen, die
  mit `   Tuer-Soll:` beginnen (Schlusszeile bleibt); je Modus Faelle 29/30, 31/30, 0/0, fehlt.
- M3 (bash-Urteil in apk_pruefen.sh, einseitige Kontrollen): Gegenseiten-Kontrollen in test_r35_android_pruefkette.sh:
  N8b (luegendes Urteil, Gate-Rueckgabe 2), N10b (OK-Zeile, Selbsttest-Rueckgabe 2), N11b (f1 > f2), N13b (e + g > m),
  N22a-e (je eine Zahl der Schlusszeile leer).
- Danach die Aenderungen der Abnahme 1 selbst nachfahren (E-Reihe: 28 nicht gleichwertige Python-Aenderungen; F1-F13
  und G1/G2 an der echten Kette) - alle muessen rot werden.

### Nachbesserung 2 - Messung vorher (Python-Urteil, E-Reihe der Abnahme 1)
Werkzeug `N_android_belege/werkzeug/nb2_urteil_aenderungen.py` (die 29 Aenderungen der Abnahme 1, 3.2, woertlich; je
eine Kopie + `--selbsttest`, 6 parallel, 29 s). Gegen das Urteil @9e535728 (Pin c8fed5ca...): Kontrolle K0 OK
(243/243, 781/785, 4); **11 von 29 NICHT bemerkt: E1, E2, E3, E4, E5, E6, E7, E9, E11, E29, E24** - genau die Liste der
Abnahme (E11 = `rc != 0` -> `rc > 0`, dort "in der Praxis gleichwertig"). Die Abnahme ist damit reproduziert.
Beleg `N_android_belege/nb2_urteil_aenderungen_vorher.txt`.

### Nachbesserung 2 - M1 einseitige Lockerungen von Vergleichen (Python-Urteil)
**Ursache (Code gelesen, @9e535728, wie die Abnahme 3.4):** `_Mutierer.TAUSCH` bildete `NotEq` nur auf `Eq` ab (und
`Eq` nur auf `NotEq`) - `!=` -> `<`/`>` und `==` -> `<=`/`>=` waren kein Mutant; und die Fallsammlung pruefte jede
Ungleichung nur von EINER Seite (qb Summe nur < n, pk qq nur > gg, st ok_n nur < n, tuer_soll g1 nur < g2, apk mb nur
> b, len(baum) nur < k; "Innere Proben 3/2" verletzte zugleich die Mindestzahl). Kein RE-Original (Port-Infrastruktur).

**Aenderung (release/gate_urteil.py):**
- `TAUSCH` je Operator eine Liste: `==` und `!=` -> jeder andere der sechs Vergleiche (darunter die einseitigen
  Lockerungen in beide Richtungen), `<`<->`<=`, `>`<->`>=` (Grenze), `<`<->`>`, `<=`<->`>=` (Richtung). Keine
  Ordnungs-Operatoren, wenn eine Seite eine Zeichenkette ist (`modus == "paket"`, `f.group(1) != "ok"`). Ergebnis:
  Vergleich-Mutanten 39 -> 145. Bei den Ungleichungen bewusst NICHT `<` -> `==`/`!=` und `<=` -> `==`: auf den
  Zaehlwerten (nie negativ) waere z.B. `n <= 0` -> `n == 0` gleichwertig, ohne etwas zu pruefen.
- `_faelle_seiten` (37 neue Faelle, 243 -> 280), je Fall genau EINE verletzte Bedingung: st 6/5 Faelle (ok_n > n),
  Innere Proben 4/3 (a > b bei b = Mindestzahl); Rueckgabe -1 je Modus + FEHLER mit -1 (main() nimmt jede ganze Zahl;
  OK nur bei genau 0, Befund nur bei genau 1 - damit ist auch E11 kein "in der Praxis gleichwertig" mehr); apk-Kette
  q == a == g == mz == n je Glied beide Seiten (Werte links vom Glied 12 -+ 1, rechts 12: 8 Faelle); Manifest-Bytes
  4095 < 4096 und 4097 > 4096; Tuerarchiv-Kette Quelle == APK == gleich == von je Glied beide Seiten (6 Faelle);
  Tuer-Soll 31/30 (apk); qb/pk: 3 Baumzeilen bei "in 2 Baeumen" und 2 bei "in 3 Baeumen" (Summe passt), Summe 13 > 12,
  pk Quelle 9 < gleich 10 (Summe gleich passt).
- `AEQUIVALENT` 4 -> 7, je mit Begruendung im Code: `int(m.group(2)) == 0` -> `<= 0` und `qq == 0` -> `<= 0` (beide
  Werte aus `(\d+)`, nie negativ); `len(urteile) != 1` -> `> 1` (an der Stelle hat m_ok die letzte Zeile als
  `== <MARKE>-OK: ... ==` erkannt, die passt immer auch auf das Urteilszeilen-Muster -> len >= 1).

### Nachbesserung 2 - M2 Tuer-Soll-Regel nur im Meldungstext
**Ursache:** `tuer_soll()` stand nur im Grund von `ende(0, ...)` (gate_urteil.py @9e535728 :148/:158/:168); Meldungstexte
werden nicht mutiert, und die Faelle "Tuer-Soll fehlt" filterten mit `"Tuer-Soll" not in z` auch die Schlusszeile
(quellbaum: "... Tuer-Soll erfuellt ==") weg - im Modus quellbaum erreichte kein Fall die Regel.
**Aenderung:** `tuer_soll()` ist eine eigene Anweisung vor `ende(0, ...)` in apk, quellbaum und paket (gibt nichts mehr
zurueck; die Zahl im Grund zaehlt der Meldungstext selbst mit `sum(z.startswith("   Tuer-Soll:") ...)`); damit hat jede
Stelle den Mutanten "Aufruf weg: tuer_soll()" (Aufruf-Mutanten 28 -> 31). Die drei Faelle "Tuer-Soll fehlt" entfernen
nur noch Zeilen, die mit `   Tuer-Soll:` beginnen; qb und pk bekommen 29/30, 31/30, 0/0. Dazu eine **Strukturregel** im
Selbsttest (`_meldung_regel`): kein Meldungstext (2. Argument von ende()/genau_eine()) darf eine im Urteil definierte
Funktion rufen - sonst `[FEHLER] Pruefung im Meldungstext`. Gegenprobe am alten Urteil @9e535728:
`['Zeile 148: tuer_soll()', 'Zeile 158: tuer_soll()', 'Zeile 168: tuer_soll()']` = genau die drei Stellen der Abnahme.

### Nachbesserung 2 - Messung nachher (Python-Urteil)
- `--selbsttest`: `== URTEIL-SELBSTTEST-OK: 280/280 Faelle, 887/894 Mutanten erkannt, 7 als gleichwertig begruendet ==`
  (`Mutanten je Operator: Aufruf 31, BoolOp 10, Regex R0 13, Regex R1 261, Regex R2 49, Regex R3 30, Regex R4 9, Regex R5
  115, Regex R6 11, Vergleich 145, Zahl 118, Zeichenkette 37, if-Bedingung 56, not 9`), ~5 s; gleich unter Python 3.9.0
  und 3.14.7. Zwischenstand nur mit dem neuen TAUSCH (vor den neuen Faellen): 26 Mutanten ueberlebt - genau die Klassen
  der Abnahme (Zeilen 85/99/102/113/117/137/142/158/160/169), nach `_faelle_seiten` 3 = die drei gleichwertigen.
- **E-Reihe der Abnahme 1 nachgefahren** (`nb2_urteil_aenderungen.py`, Beleg `nb2_urteil_aenderungen_nachher.txt`):
  K0 Kontrolle OK; **29 von 29 bemerkt** (vorher 18/29), darunter alle zehn der Abnahme und E11: E1 -> Fall "qb/S: Summe
  der Baumzeilen 13 > 12", E2 -> "pk/S: Quelle 9 < gleich 10", E3 -> "st/S: 6/5 Faelle", E4 -> "apk/S: Tuer-Soll 31/30" +
  "qb/S: Tuer-Soll 31/30", E5 -> "qb/S: 3 Baumzeilen", E6 -> "pk/S: 3 Baumzeilen", E7 -> "pk/S: Summe gleich 13 > 12",
  E9 -> "st/S: Innere Proben 4/3", E11 -> "... gut, Rueckgabe -1" (4 Faelle), E29 -> "apk/S: Manifest 4095 Bytes <
  SUMME 4096", E24 (Form 2 = Anweisung `tuer_soll()` in quellbaum gestrichen; Form 1 `% (n, k, tuer_soll())` gibt es nicht
  mehr) -> "qb: Tuer-Soll fehlt" + "qb/S: Tuer-Soll 29/30" u.a. (4 Faelle), E24b (Form 2) -> 14 Faelle.
- NB1-Reihe (`urteil_aenderungen.py`, 32 Aenderungen) gegen das neue Urteil: weiter **32 von 32** (Beleg
  `nb2_nb1_aenderungen_neu.txt`).
- Alt (@9e535728) gegen neu auf allen 280 Faellen (`urteil_alt_neu_nb1.py`): `280 Faelle, 280 gleiches Urteil, 0 anders`
  (Beleg `nb2_urteil_alt_neu.txt`) - die Nachbesserung aendert kein Urteil, sie haelt nur fest.
- Pin `release/gate_urteil.sha256` = `107de123f9aa2b733d089280f1920d33a5c442a8c8e6fff578eec993e1507a2e`;
  `GATE_URTEIL_MIN_FAELLE/ERKANNT/MAX_GLEICH` = 280/887/7 (apk_pruefen.sh; Kommentar dort nennt die 3 neuen
  Gleichwertigen). Commit e28f9568.

### Nachbesserung 2 - M3 einseitige Kontrollen des bash-Urteils (apk_pruefen.sh)
**Ursache (Code gelesen, wie Abnahme 3.4):** test_r35_android_pruefkette.sh pruefte jede Pruefzeile des bash-Urteils von
EINER Seite: N8 nur Gate-Rueckgabe 1, N10 nur Selbsttest-Rueckgabe 1, N11 nur f1 < f2, N13 nur e + g < m; keine
Kontrolle mit einer leeren oder nicht-numerischen Zahl. Und es gab - anders als beim Python-Urteil - keinen
Mechanismus, der eine KUENFTIGE Aenderung einer bash-Pruefzeile von selbst mittestet: nur feste Kontrollen.

**Aenderung 1 - `re15_port/tests/unit/r35_android/urteil_kontrollen.sh` (neu):** alle Attrappen-Kontrollen des
bash-Urteils (vorher P3 + N10-N21 in der Kette) in einer eigenen Datei, je Kontrolle genau EINE verletzte Bedingung,
Soll = genaue Rueckgabe bzw. Abbruch MIT der Meldung genau dieser Pruefzeile, jede Kontrolle in einer Unterschale mit
`set -euo pipefail` (so laufen die echten Aufrufer build_android.sh/make_package.sh, Kopf apk_pruefen.sh). Gruppen:
- P (Positiv, muss angenommen werden): P3 Selbsttest, P4/P5/P5b gate_laufen mit Gate-Rueckgabe 0/1/2 -> genau 0/1/2
  (ein Befund bleibt Befund, "keine Aussage" bleibt 2), P6 gate_festhalten + gate_laufen, P7 gate_urteil direkt, P8
  zweiter Aufruf aus dem Zwischenspeicher, P9 Gate-Pin richtig.
- ST (gate_urteil_selbsttest): N10/N10b Rueckgabe 1 UND 2, N11/N11b f1 < f2 UND f1 > f2, N12 Mindestzahl, N13/N13b
  e + g < m UND > m, N14, N15, N16-N18 Lage/Anker, N7 leeres Urteil, **N22 je Zahl leer** (5), **N23 je Zahl keine
  Ziffer** (5), **N24 je Wort der Schlusszeile ein anderes** (9), D11 kein Temp-Platz, D11b Zwischenspeicher nur fuer
  GENAU die gepruefte Kopie.
- LAUF (zweite Instanz, gate_urteil): **N8c/N8b luegendes Urteil bei Gate-Rueckgabe 1 UND 2** -> genau 2, N8m Meldung
  "ueberstimmt"; N19a-g Urteilszeile fehlt / anderer Modus / "Rueckgabe 1" / Text davor / ohne Doppelpunkt / zwei statt
  drei Leerzeichen / ohne Leerzeichen nach dem Doppelpunkt, N19w je Wort; D17-D19.
- PIN: D1-D10 Pin-Datei fehlt / kein SHA-256 (abc, 64 x g, 63 und 65 Ziffern) / Datei fehlt / nicht lesbar (PY=false) /
  nicht festgehalten - fuer Gate UND Urteil.
- FH (gate_festhalten): D12-D16, D20-D22 (Ordner fehlt, Ziel schon da, Quelle fehlt, Quelle nicht gepinnt, Selbsttest
  rot), N20/N20b/N21 private Kopien veraendert (N20c: das veraenderte Urteil wird VOR dem Gate-Lauf erkannt).
Die Kette (test_r35_android_pruefkette.sh) ruft die Datei auf (Abschnitt "bash-Urteil (Attrappen)"); P0-P2, N1-N9 mit
echtem Gate/Urteil bleiben dort.

**Nebenbefund + Fix (gemessen):** unter `set -euo pipefail` brach `gate_urteil_selbsttest` bei einem Urteil OHNE jede
Ausgabe (0-Byte-Urteil, N7) in der Zeile `letzte="$(tr ... | grep -v '^[[:space:]]*$' | tail -1)"` ab - grep findet
nichts, Rueckgabe 1, pipefail -> die Shell endet OHNE die Meldung "ohne gueltige Schlusszeile" (Kontrolle N7 rot:
`Rueckgabe 1 ... (Selbsttest des Gate-Urteils, sha256 e3b0c44298fc1c14...)`, danach nichts). Fail-closed war es, aber
ohne Grund. Fix `release/apk_pruefen.sh`: `... | tail -1 || true)"`. Danach N7 gruen; gegen die alte Zeile weiter rot
(Gegenprobe mit `git show HEAD:release/apk_pruefen.sh`). Die alte Kette lief ohne `set -e` und sah das nicht.

**Aenderung 2 - `bash_urteil_mutanten.py` + ctest `unit_r35_android_bash_mutanten` (neu):** das Gegenstueck zum
Mutanten-Selbsttest des Python-Urteils fuer das bash-Urteil. Je Lauf genau EINE Aenderung an den sechs Funktionen
gate_pin_pruefen, gate_urteil_pin_pruefen, gate_urteil_selbsttest, gate_festhalten, gate_urteil, gate_laufen, nur im
Code (nicht in Kommentaren/Meldungstexten; Anfuehrungszeichen ueber Zeilen verfolgt). Operatoren: A `(( ))`-Vergleiche
mit derselben Tabelle wie TAUSCH im Python-Urteil (== / != -> jeder andere, < <-> <=, > <-> >=, < <-> >, <= <-> >=) und
&& <-> ||; B `[[ ]]` == <-> != und Verneinung; C `grep -q`-Muster gelockert (Rest -> .*, Wort -> .*, Zeichen weg);
D `=~`-Muster gelockert (dazu `[0-9]+` -> `[0-9]*` und -> `.*`, Klasse -> `.`, `{64}` -> `+`); E jedes `die` -> `true`;
F Zuweisung name=Zahl -> 0/+-1; G Aufruf einer Pruef-Funktion weg; H `return x` -> 0/1. Je Mutant laeuft
urteil_kontrollen.sh `--schnell` (Ende beim ersten FALSCH; Reihenfolge der Gruppen nach der mutierten Funktion), die
unveraenderte Datei muss ALLE Kontrollen bestehen. Ueberlebt ein Mutant und steht nicht begruendet in AEQUIVALENT ->
`BASH-URTEIL-MUTANTEN-FEHLER`. Gleichwertig (3, je mit Begruendung): `(( rc == 0 ))` -> `<=`, `(( urteil == 0 ))` ->
`<=`, `(( rc != 0 ))` -> `>` - rc/urteil sind Rueckgaben (`$?` 0..255), nie negativ.

**Messung (bash-Urteil):**
- urteil_kontrollen.sh gegen das echte release/apk_pruefen.sh: `KONTROLLEN: 87 ok, 0 FALSCH` (~46 s; Beleg
  `N_android_belege/nb2_urteil_kontrollen.txt` = Stand mit 83 Kontrollen, danach +4: D2d, D7c, D7d, N19g).
- **Erster Mutantenlauf** (83 Kontrollen, ohne AEQUIVALENT; Beleg `nb2_bash_mutanten_erster_lauf.txt`): 220 Mutanten,
  **12 ueberlebt**: Pin-Muster `^[0-9a-f]{64}$` ohne `^` bzw. ohne `$` (Gate und Urteil, 6) und Klasse -> `.` (Urteil-Pin,
  1) - keine Kontrolle hatte 65 Ziffern bzw. 64 Nicht-Hex-Zeichen beim Urteils-Pin; grep-Muster der Urteilszeile ohne das
  Leerzeichen nach dem Doppelpunkt (2); und die drei `$?`-Gleichwertigen. Dazu D2d/D7c (65 Ziffern), D7d (64 x g beim
  Urteil), N19g (Urteilszeile ohne Leerzeichen nach `:`) und die drei AEQUIVALENT-Eintraege.
- **Endstand** (`test_r35_android_bash_mutanten.sh`, 10 parallel, Beleg `nb2_bash_mutanten.txt`): Kontrolle
  `KONTROLLEN: 87 ok, 0 FALSCH`; `Mutanten je Operator: A 34, B 18, C 34, D 93, E 23, F 9, G 7, H 2`;
  **`== BASH-URTEIL-MUTANTEN-OK: 220 Mutanten, 217 erkannt, 3 als gleichwertig begruendet ==`**, Laufzeit 224 s.
  Haeufigste Toeter: N10 (56), N8c (14), N19b (11), N17 (11), P4 (8), N13/N11 (je 7) - die neuen Gegenseiten-Kontrollen
  N8c, N11b, N13b, N22, N24, D2d/D7c toeten je mindestens einen Mutanten, den sonst keine Kontrolle faengt.
  Beispiel G (Aufruf weg): `gate_urteil_selbsttest` in gate_laufen gestrichen -> N20 rot, weil dann die Abbruchmeldung von
  gate_urteil in `$log.urteil` landet und vor dem `cat` verloren geht - die Vorab-Pruefung in gate_laufen ist also nicht
  doppelt, sie macht den Grund sichtbar.
