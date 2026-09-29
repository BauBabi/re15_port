# Android-Asset-Gate - Nachbesserung Runde 1

Stand: 2026-09-29, Zweig r34a/android-gate, Baum .claude/worktrees/r34a_android.
Auftrag: Befunde der Gegenpruefer (pruefer_*_r1.md) je selbst pruefen, hoch/mittel beheben,
niedrig wo ohne Risiko; Selbsttest + Positiv-/Negativ-Kontrollen erneut fahren.

## 0. Laufprotokoll (fortlaufend)

- Dossier angelegt (erster Werkzeugaufruf).

- 21:50 Python-Schnappschuss 0 (`nachbesserung_r1_belege/py_zustand_0_vorher.txt`, 121 Zeilen,
  Werkzeug des Pruefers echtlauf) = dessen Endstand (`diff` leer).
- 21:50-21:54 alle Pruefer-Behauptungen selbst nachgemessen (Abschnitt 1) - alle bestaetigt.

## 1. Nachpruefung der Befunde (vor jeder Aenderung, Gate-Stand HEAD 568e9b88)

Faelschungen mit dem Rohbyte-Werkzeug des Pruefers (`pruefer_umgehung_r1_belege/umgehung_werkzeug.py`,
/c/Python310) an Kopien von `build/r34a/ref_v0.8.19.apk` (sha256 514bebd5... = Archiv-SUMS), Ablage
`build/r34a/nb/apk/`. Belege: `nachbesserung_r1_belege/vorher_*.txt`.

| Befund | eigene Messung | Urteil |
|---|---|---|
| umgehung B1 (hoch) | F2 `\`, F3 NUL, F4 LFH-CRC, F5 LFH-Groesse: Gate rc 0 unter 3.10.11 UND 3.14.7 (`vorher_gate_HEAD.txt`); aapt2 35.0.0 (libziparchive): F2 `failed to find file.`, F3 `Invalid entry name`, F4/F5 `size/crc32 mismatch ... Inconsistent information` (`vorher_aapt2_libziparchive.txt`); F2 mit dem Debug-Schluessel neu signiert -> apksigner rc 0, Signer 432bc749... = Referenz, `build_android.sh --gate-only F2_signiert --version v0.8.19` -> EXIT=0 `ANDROID-GATES-OK` (`vorher_gate_only_HEAD.txt`) | bestaetigt |
| umgehung B2 (mittel) | apksigner verify: ref rc 0 (v2 true); K0 rc 1 `Missing META-INF/MANIFEST.MF`; F2/F4 rc 1 `CHUNKED_SHA256 digest mismatch` - dieselben APKs bestehen das Gate mit rc 0 (`vorher_apksigner.txt`) | bestaetigt |
| umgehung B3 (mittel) | Mutanten U1-U4 des Pruefers aus dem HEAD-Gate: je `SELBSTTEST-OK: 28/28`; U3 und U4 gegen F1 (CRC32-erhaltend in ENEMSE.VBS) rc 0, echtes Gate rc 1 (`vorher_mutanten_U1_U4.txt`) | bestaetigt |
| umgehung B4 (niedrig) | `ANDROID_SDK_ROOT=/c/gibt_es_nicht ... --gate-only ref --version v9.9.9` -> EXIT=0 mit `(aapt nicht gefunden - badging-Gate uebersprungen)` | bestaetigt |
| umgehung B5 (niedrig) | Code: Pruefung `make_package.sh:422-432` (`$APK_PRUEF`), Zippen `:591-596` (`$APK`, neu ermittelt), dazwischen copy_common/check_tree/check_runtime_assets beider Plattformen; ohne APK bei :423 keine Pruefung, APK bei :592 wird trotzdem gezippt. Keine Frischepruefung fuer die APK (check_binary_fresh nur :492/:524) | bestaetigt (Code) |
| echtlauf B1 (niedrig) | `bash release/python_finden.sh` -> `/c/msys64/mingw64/bin/python3 (3.14.7)`; PATH-Reihenfolge: `/c/Python310` steht an Stelle 12, `/c/msys64/mingw64/bin` an 43 - gewaehlt wird trotzdem MSYS2, weil erst ALLE `python3` und danach alle `python` kommen | bestaetigt |
| echtlauf B2 (niedrig) | `git log -1 -- engine platform include` = f2d26986 20:55:26 (build.gradle); ohne `platform/android` = cf386e32 19:02:06. PC-Bau haengt nicht an platform/android (`re15_port/CMakeLists.txt:92-101`: android/jni nur mit RE15_BUILD_ANDROID, der PC-Bau nur platform/pc) | bestaetigt |
| echtlauf B3 (niedrig) | `git check-ignore -v ...apk.ungeprueft` rc 1; `...android.apk` -> `.gitignore:63` | bestaetigt |
| echtlauf B4 (niedrig) | Code: `build_android.sh:323` `rm -f "$OUT" "$UNGEPRUEFT"` steht NACH Gradle (:311-315); make_package prueft weder versionName noch Codestand der APK | bestaetigt (Code) |
| Altlast | `HKCU\...\Uninstall\pymanager-pythoncore-3.14-64`: DisplayName Python 3.14.7, InstallDate 20260929, InstallLocation (Test-Path False), UninstallString startet den WindowsApps-PythonManager | bestaetigt |
