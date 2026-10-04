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
  das Modell erklaert den Befund vollstaendig. Messung am echten Code im Pruefstand: folgt.

## Punkt 2 — Datei<->Ordner-Konflikt im Update

### Messung vorher
- Runde 34a (Linux-Pruefstand, echter Code): H8 Ordner `PSX/q/` (mit `c`) wird Datei `PSX/q`: weg-Schleife loescht
  `q/c`, leerer Ordner `q/` bleibt -> `rename .../PSX/q: Is a directory` -> ABBRUCH; erst der naechste Start (ohne
  Liste -> Waisen-Lauf raeumt `q/`) entpackt. S3/Y4: `PSX/X` + `PSX/X.neu/B` -> Zwischendatei `X.neu` ist ein Ordner ->
  `open` EISDIR -> ABBRUCH bei JEDEM Start (dauerhaft, die Liste ist gueltig).
- Code (android_glue.c:499-545): weg-Schleife `if (unlink(dst) == 0) {...}` ohne else (F-Y6), danach je Eintrag nur
  `unlink(tmp)`, `file_size(dst)` (stat, kein Ordnertest auf dem Weg), `entpacken()` -> `mkdirs_for` (mkdir-Fehler
  verschluckt), `open(tmp)`, `rename(tmp,dst)`. Kein Schritt raeumt einen Ordner auf dem Zielnamen, eine Datei auf
  einem Elternnamen oder einen Ordner auf `<ziel>.neu`. Messung am echten Code im Pruefstand: folgt.

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

## OFFEN
(folgt)
