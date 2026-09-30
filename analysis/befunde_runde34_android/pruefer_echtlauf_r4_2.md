# Gegenpruefung Runde 2 - Linse ECHTER LAUF - Stufe-4-Kette + Geraete-Entpacker N1 (nach Nachbesserung R4-1)

Pruefer: Gegenpruefer (Linse ECHTER LAUF, Runde 2). Aendert KEINE Werkzeuge/Quellen.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Gegenstand: Commits be8b60f3..HEAD (Kette R4, Entpacker N1, Nachbesserung R4-1 bis 8cbf88d6); Dossiers
android_gate_r4_kette.md, android_entpacker_n1.md, android_r4_nachbesserung.md.
Arbeitsordner (nicht versioniert): build/r34a/pruefer_e2/ ; Belege: analysis/befunde_runde34_android/pruefer_echtlauf_r4_2_belege/

Stand: IN ARBEIT

## 0. Ausgangslage

- HEAD beim Start `8cbf88d6` (Abschluss Nachbesserung R4-1), mein Dossier-Commit `590d773d` darauf. Seit der
  Gegenpruefung R4-1 (79ebd085) geaendert: release/apk_asset_gate.py (+ neuer Pin apk_asset_gate.sha256),
  apk_pruefen.sh (gate_festhalten/gate_laufen/gate_urteil), build_android.sh, make_package.sh (Positivliste),
  android_glue.c + asset_abgleich.{c,h} (ASCII-Regel, Waisen, fail closed, U1), build.gradle (Pfadregel),
  README, Unit-Test test_r34a_asset_abgleich.c.
- Parallel im selben Baum: Gegenpruefer UMGEHUNG R2 (pruefer_umgehung_r4_2.md, Kopien unter build/r34a/pruefer_u2/).
  Deshalb: eigene Arbeits-/Bauordner, `git add` nur meiner Pfade, make_package.sh mit Wegwerf-Index +
  Wegwerf-Objektspeicher, Emulator nur nach Pruefung "keine andere Instanz", vor jedem schweren Lauf
  Prozessliste (Win32_Process-Befehlszeilen) auf build_android/make_package/gradle in DIESEM Baum.
- Andere Sitzungen beim Start: `local_build.sh all` in r34n_integration (fremder Baum, kein Einfluss ausser Last).
  Kein Emulator/qemu, `adb devices` leer, kein java/gradle.
- Frische-Lage: letzter PC-Code-Commit `cf386e32` (2026-09-29 19:02) -> Archiv-Binaries v0.8.19 (Eintraege 19:34)
  sind frisch; letzter APK-Code-Commit `ef5fa0fe` (2026-09-30 11:15) -> nur eine danach gebaute APK ist frisch.
- Platte C: 48 GB frei (95 % belegt) - Kopien/Abbilder laufend aufraeumen.
- Python-Schnappschuss 0 (11:58): eigenes Skript `pe2_py_zustand.ps1` (liest nur) -> `py_zustand_0_start.txt`
  (178 Zeilen); zusaetzlich das Skript der Vorrunde (`pruefer_echtlauf_r4_1_belege/py_zustand.ps1`): Ausgabe ==
  Endstand der Nachbesserung (11:43, `android_r4_nachbesserung_belege/py_zustand_ende.txt`), `diff` leer.
  (Vorbestand, nicht neu: HKCU-Uninstall `pymanager-pythoncore-3.14-64 Python 3.14.7` = v0.8.17-Vorfall.)

