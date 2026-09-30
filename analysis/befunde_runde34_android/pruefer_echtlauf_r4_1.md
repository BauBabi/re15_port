# Gegenpruefung Runde 1 - Linse ECHTER LAUF - Stufe-4-Kette + Geraete-Entpacker N1

Pruefer: Gegenpruefer (Linse ECHTER LAUF, Runde 1). Aendert KEINE Werkzeuge/Quellen.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Gegenstand: Commits be8b60f3..HEAD, Dossiers android_gate_r4_kette.md, android_entpacker_n1.md

Stand: IN ARBEIT (Abschnitte werden fortlaufend gefuellt und committet)
Arbeitsordner (nicht versioniert): build/r34a/pruefer_e1/ ; Belege: analysis/befunde_runde34_android/pruefer_echtlauf_r4_1_belege/

## 0. Ausgangslage (Commits, HEAD)

- HEAD beim Start: `6b8e8cc8` (Zweig r34a/android-gate). Gegenstand `git log --oneline be8b60f3..HEAD`: Kette R4
  (42d72ec1, 3b677ea0, 7f269d04, 80559d2f, cb6617fd, 93f85469) und N1 (eda2360b .. 6b8e8cc8, darin Merge 82f4af74
  von master 7d4d11dd). N1 ist seit dem Auftragstext weitergelaufen und abgeschlossen (Dossier "4/5", Suite 429/429,
  v0.8.19-APK vom Gate als Liste v1 abgelehnt).
- Parallel im selben Baum: Gegenpruefer UMGEHUNG (Dossier pruefer_umgehung_r4_1.md, Kopien unter
  build/r34a/pruefer_u1/). Deshalb: eigene Bau-/Arbeitsordner, git nur mit `git commit -- <meine Pfade>`,
  make_package.sh mit Wegwerf-Index (GIT_INDEX_FILE), Emulator nur nach Pruefung "keine andere Instanz".
- Frische-Lage: letzter PC-Code-Commit cf386e32 (2026-09-29 19:02) -> Archiv-Binaries v0.8.19 (19:34) frisch;
  letzter Android-Code-Commit 026632a4 (2026-09-30 09:22) -> nur eine NACH 09:22 gebaute APK ist frisch.
- Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 121 Zeilen, Skript `py_zustand.ps1` aus R4 unveraendert,
  liest nur) == Endstand R4 (`diff` leer).
- Kein Emulator/qemu, kein java/gradle beim Start (tasklist, `adb devices` leer).

## 1. Voller Android-Bau build_android.sh

## 2. make_package.sh echt (Binaries aus Archiv v0.8.19) + "nur PC mit altem Android-Satz"

## 3. Emulator headless: v0.8.19 -> neue APK -> Update mit gleich grosser Aenderung -> Update ohne Aenderung -> force-stop mitten im Entpacken

## 4. PC-Suite local_build.sh all

## 5. Kein Python-Installer

## 6. Befunde

## 7. Urteil
