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

## 1. Voller Android-Bau build_android.sh

`bash release/build_android.sh --version v0.8.19 --no-toolchain` im Arbeitsbaum (vorher: kein build_android/make_package/
gradle-Prozess, Log `build/r34a/pruefer_e2/logs/android_voll.log`, Auszug `android_voll_auszug.txt`), 12:00:47-12:03:24:
**EXIT 0, `ANDROID-BUILD-OK`**. Python `/c/Python310/python (3.10.11)`. CMake-Cache verworfen, beide ABIs neu uebersetzt
(`buildCMakeRelWithDebInfo[arm64-v8a]` + `[x86_64]`), `BUILD SUCCESSFUL in 2m 12s`; stageAssets/writeAssetManifest
UP-TO-DATE (Staging aus dem NB1-Bau, gleicher Inhalt - das Gate vergleicht jede Datei mit dem Quellbaum; der frische
writeAssetManifest-Lauf folgt im Sandbox-Bau M, Abschnitt 3.0). Kette: aapt `versionCode='81900' versionName='v0.8.19'`,
zipalign ok, 1 Signer `432bc749...` (= apk_signer.sha256), Gate als private Kopie `sha256 8a0e3f15... = festgehalten`
(von mir nachgerechnet: `sha256sum release/apk_asset_gate.py` = Pin-Datei), `Gate-Urteil (selbsttest, Rueckgabe 0):
SELBSTTEST-OK 258/258, jede Fallzeile [ok] mit rc = soll, innere Proben 132/132`, Tuer-Soll RE15DOOR 30/30 + RE2/DOOR
27/27, ZIP 3616/3616 lesbar, **3603/3603 bytegleich** (356678277 B), `Gate-Urteil (apk, Rueckgabe 0):
APK-ASSET-GATE-OK ... Quelle = APK = unzip-Zaehlung - 1 = gleich = Manifestzeilen`. APK **N** =
`2c6a1c30c97eb978d49e07ff76dbb5d2c61b835a3f29a065ab2d15ac8ddd15ea`, 363483287 B (Kopie `build/r34a/pruefer_e2/apk/N.apk`);
`release/SHA256SUMS_android.txt` danach per `git restore --source=HEAD` zurueck (`git status --short release/` leer).

Unabhaengig (eigenes Skript `pe2_apk_liste.py`, Python-zipfile, nicht das Gate; `apk_liste_N.txt`): Liste in N 398099 B,
sha256 `95770eb5...` (= Liste von N in R4-1: Assets unveraendert), Kopf `# re15 assets v2 3603 356678277`, jede Zeile
`<bytes>\t<64 hex>\t<pfad>`, Pfade druckbares ASCII / Segment <= 251, sortiert, keine Gross/klein-Dubletten, Kopf = Summe;
jeder Eintrag STORED, Groesse + sha256 der APK-Daten = Zeile (3603/3603); assets/-Eintraege = Liste + re15_assets.txt;
Quellbaum (4 shared_assets-Baeume + synchro/STAGE*) = genau die Liste, 3603/3603 Groesse + sha256 gleich; Tuerarchive
in der Liste RE15DOOR 30, RE2/DOOR 27: **LISTE-OK**.

