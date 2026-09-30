# Gegenpruefung Runde 1 — Linse UMGEHUNG/ROBUSTHEIT (Stufe-4-Kette + Geraete-Entpacker N1)

Pruefer: Gegenpruefer UMGEHUNG/ROBUSTHEIT, Runde 1 (Sitzung 2026-09-30).
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate).
Gegenstand: Commits `git log --oneline be8b60f3..HEAD` — Stufe-4-Kette (B1-B4, README;
Dossier android_gate_r4_kette.md) und Geraete-Entpacker N1 (Dossier android_entpacker_n1.md).
Regel: Ich aendere KEINE Werkzeuge/Quellen. Mutanten/Zusatzfaelle nur in Kopien unter build/r34a/pruefer_u1/.

Ziel: ein falsches Ergebnis erzwingen (Umgehung) oder einen verschluckten Fehler finden (Robustheit).

## 0. Stand / Fortschritt

- [ ] 0.1 Commit-Liste und Stand aufnehmen
- [ ] 1 Kette: veralteter Android-Satz (nur PC, abgelehnte APK, APK geloescht, --ohne-android, --only)
- [ ] 2 Kette: sha256-Kennung der APK im Split-Satz
- [ ] 3 Kette: make_package-PC-Pfad mit kaputtem Gate (Mutant) -> Abbruch?
- [ ] 4 Selbsttest: eigene neue Ein-Zeilen-Mutanten (auch v2-Manifest-Teil)
- [ ] 5 Format v2: Faelschungen
- [ ] 6 Entpacker: Code + PC-Unit-Test + Zusatzfaelle (Pfade, Abbruch/Neustart, JNI-Fehlerpfade, SHA-256)
- [ ] 7 Urteil


## 0.1 Ausgangslage (gelesen, nicht gemessen)

- HEAD `6b8e8cc8` (Zweig r34a/android-gate). Gegenstand `be8b60f3..HEAD`: Kette 42d72ec1 (B2), 3b677ea0 (B1/B3/B4,
  README), 7f269d04, 80559d2f (B1 nachgeschaerft), cb6617fd (Temp-Pfade), 93f85469 (Dossier); N1 eda2360b ..
  6b8e8cc8 (asset_abgleich.{h,c}, android_glue.c, build.gradle v2, Gate v2 248 Faelle + 116 innere Proben,
  Unit-Test 272 Pruefungen). Parallel arbeitet der Pruefer ECHTER LAUF im selben Baum (pruefer_echtlauf_r4_1.md) -
  ich committe nur meine Pfade, Emulator nur nach `adb devices`/qemu-Pruefung.
- Gelesen: beide Dossiers ganz, asset_abgleich.{h,c} ganz, android_glue.c ganz, Unit-Test ganz, build.gradle
  (stageAssets/writeAssetManifest), make_package.sh (Diff be8b60f3..HEAD + Zip-/Git-Abschnitt), apk_pruefen.sh
  (Schritte 0-6), apk_asset_gate.py (Kopf, Konstanten, `_pfad_fehler`, `manifest_lesen`, `manifest_pruefen`,
  Selbsttest-Harness `_fall_laufen`/`_fall_bewerten`/`selbsttest`, `main`), main.c um die Bootstrap-Aufrufe,
  Mutanten-Werkzeug der R2-Kampagne (`mutanten_teil.py`: "Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/main)").

## 0.2 Hypothesen aus dem Lesen (werden unten gemessen)

- H1 (B4): Das Urteil jedes Gate-Aufrufs ist NUR sein Exit-Code (make_package.sh `(( rc_selbst == 0 ))`, `case "$rc"
  in 0)`; apk_pruefen.sh `(( rc == 0 ))`). Den Exit-Code erzeugt derselbe Code, der geprueft werden soll. Eine
  leere/abgeschnittene Gate-Datei (Memory reai-v2-patchskript-truncate: das ist schon einmal passiert) oder ein
  Ein-Zeilen-Mutant im Ausgangspfad (`sys.exit(main())` -> `main()`, `RC_ABWEICHUNG = 0`, `return rc if ...` ->
  `return RC_GLEICH`) liefert fuer Selbsttest UND jede Pruefung rc 0 - niemand verlangt die OK-Zeilen.
  Die Mutanten-Kampagnen haben main/Selbsttest ausdruecklich ausgenommen.
- H2: build_android.sh uebernimmt `APK_GATE_DATEI` aus der Umgebung (apk_pruefen.sh `${APK_GATE_DATEI:-...}`), ohne
  den Pfad anzuzeigen - ein fremdes/altes Gate prueft dann still.
- H3 (B1): B1 haengt am exakten Namen `${NAME}_android.z*`. SHA256SUMS.txt und git add nehmen weiter JEDE Datei
  `${NAME}_*.z*` (z.B. `${NAME}_ANDROID.zip`, `${NAME}_android_alt.z01`, `${NAME}_android.apk.zip`) ungeprueft mit,
  auch mit `--ohne-android`.
- H4: neue Gate-Mutanten im v2-Teil (`manifest_lesen`/`_pfad_fehler` standen in keiner systematischen Kampagne,
  nur G1-G22 von Hand): `if not z:` -> `if not z.strip():` (Zeile nur aus Leerraum), Steuerzeichen-Grenze
  `c < 0x1f`.
- H5 (Format v2): Faelschungen gegen C-Leser und Gate gemeinsam; dazu Unicode-Gross/klein (Kelvin-Zeichen U+212A,
  Ae/ae) und NFC/NFD - die Dublettenregel faltet nur ASCII, der App-Speicher (ext4/f2fs casefold) faltet Unicode.
- H6 (Entpacker): Unicode-Faltung auf dem Geraet (H5), Reste die nie entfernt werden (weg nur mit gueltiger alter
  Liste; `.neu` nur fuer Pfade der neuen Liste), `unlink(pf_liste)` ohne Rueckgabepruefung, nach Fehlern laeuft das
  Spiel nach 3 s Meldung mit altem/halbem Baum weiter (main.c:3075 hat keine Rueckgabe), SHA-256 gegen Python
  (Zufallslaengen/Stueckelungen, Datei > 4 GiB).
