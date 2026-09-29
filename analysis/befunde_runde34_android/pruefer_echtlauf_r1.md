# Gegenpruefung Runde 1 — Linse ECHTER LAUF / INTEGRATION

Pruefer: Gegenpruefer (echter Lauf), Runde 1
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Beginn: 2026-09-29

Auftrag (Kurzfassung): Die vom Bauer eingebaute Android-Asset-Pruefung im echten Ablauf pruefen:
1. Voller Android-Bau (build_android.sh --version v0.8.19 --no-toolchain) — Gates im echten Fluss, ANDROID-BUILD-OK, Laufzeit.
2. Gebaute APK gegen Referenz-APK v0.8.19 (Asset-Liste + Inhalte gleich).
3. make_package.sh echt laufen lassen (Python-Finder, kein Installer, neue cmp-Gates RE2/DOOR + TORSE.VBS).
4. apk_asset_gate.py unter Linux (Docker-Bau-Image), inkl. --selbsttest.

Belege: analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/

## Stand

(laufend fortgeschrieben)
- 21:1x Dossier angelegt, Commits/Bauer-Dossier/Werkzeuge gelesen, Python-Schnappschuss 0.
- 21:16:57 voller Android-Bau gestartet (Hintergrund, Log mit Zeitstempel je Zeile).

## 0. Ausgangslage

- Pruefgegenstand: `git log --oneline a358fd5d..HEAD` = c1c91713 (Auftrag) .. d8a3ad09 (feat: volle
  Android-Asset-Pruefung). Geaendert: `release/apk_asset_gate.py` (neu, 971 Z.), `release/python_finden.sh`
  (neu), `release/build_android.sh`, `release/make_package.sh`, `re15_port/platform/android/app/build.gradle`.
- Parallel im selben Baum: Gegenpruefer "Umgehung" (Dossier pruefer_umgehung_r1.md, Faelschungen unter
  build/r34a/pruefer_umgehung_r1/). Beim Start liefen keine Bau-Prozesse im Baum (Prozessliste 21:15:
  nur die Granaten-Sitzung mit C:\Python310\python.exe lauf.py, anderer Baum).
- Seit dem Tag v0.8.19 (64170c05) aendert sich unter `re15_port/engine re15_port/platform re15_port/include`
  NUR `re15_port/platform/android/app/build.gradle` (`git diff --stat v0.8.19..HEAD`) -> die PC-Binaries
  aus dem v0.8.19-Archiv entsprechen dem heutigen PC-Code.
- Werkzeuge des Pruefers (nur Belege, keine Aenderung an release/*):
  `pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh` (Zeitstempel je Ausgabezeile per `$EPOCHREALTIME`,
  Rueckgabe per Prozess-Substitution statt Pipe, haengt `EXIT=<rc>` an; Selbstprobe `exit 3` -> rc 3,
  `EXIT=3`), `pruefer_echtlauf_r1_belege/py_zustand.ps1` (Python-Installer-Schnappschuss: HKCU/HKLM
  `Software\Python` rekursiv MIT Werten, HKCU-Uninstall-Eintraege "Python", Startmenue beider Wurzeln
  rekursiv, `%LOCALAPPDATA%\Python` + `\Programs\Python` rekursiv (Anzahl + neuester Eintrag),
  WindowsApps-Aliase, `release/Python` + `python_install*`-Logs im Baum — startet nichts).

### 0.1 Python-Schnappschuss 0 (vor allen Laeufen, 21:14) — `py_zustand_0_vorher.txt`, 121 Zeilen
- HKCU `Software\Python\PythonCore` ohne Unterschluessel; HKLM 3.9/3.10/3.12; Startmenue nur ProgramData
  3.9/3.10/3.12; `%LOCALAPPDATA%\Python` = nur `_cache\last_welcome.txt` (15:39:13); kein `Programs\Python`;
  kein `release/Python` im Baum.
- **Altlast (vor dieser Runde, nicht vom Bauer):** HKCU
  `Software\Microsoft\Windows\CurrentVersion\Uninstall\pymanager-pythoncore-3.14-64` existiert noch:
  DisplayName "Python 3.14.7", InstallDate 20260929, InstallLocation
  `C:\workspace\git\reAi_v2\.claude\worktrees\r31_integration\release\Python\pythoncore-3.14-64` (Ordner
  existiert NICHT mehr), UninstallString ruft den WindowsApps-PythonManager. Das ist der Rest des
  v0.8.17-Unfalls: die Entfernung 2026-09-29 (Memory runde31) nahm PythonCore\3.14, Startmenue und Ordner
  weg, aber nicht diesen Uninstall-Eintrag -> unter "Installierte Apps" steht weiter "Python 3.14.7".
  Fuer die Vorher/Nachher-Vergleiche unten ist er Teil der Basislinie.

## 1. Voller Android-Bau

## 2. APK gegen Referenz-APK

## 3. make_package.sh echt

## 4. Linux-Lauf

## Befunde

## Urteil
