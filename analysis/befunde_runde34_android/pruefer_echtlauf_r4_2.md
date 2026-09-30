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

## 2. make_package.sh echt (Binaries aus Archiv v0.8.19) + "nur PC mit altem Android-Satz"

(laeuft - Abschnitt folgt)

## 3. Emulator headless

### 3.0 Aufbau
- **Eigene AVD-Kopie** `Medium_Phone_API_36_pe2` unter `build/r34a/pruefer_e2/avd/` (ANDROID_AVD_HOME): config.ini des
  Nutzer-AVD mit genau 5 Aenderungen (`diff --strip-trailing-cr`): AvdId, Anzeigename, `disk.dataPartition.size =
  12884901888` (statt 6 GiB; auf 6 GiB scheitern Updates an INSTALL_FAILED_INSUFFICIENT_STORAGE, N1 1.3),
  `fastboot.forceColdBoot = yes`, `fastboot.forceFastBoot = no`. Dasselbe System-Image (android-36 google_apis_playstore
  x86_64). Nutzer-AVD nicht angefasst. (Beobachtung ohne Bezug: dessen Ordner zeigt einen Start heute 10:32-10:33 und
  eine liegengebliebene `multiinstance.lock` - nicht von mir; ich habe ihn nur gelesen.)
- Vorher: kein qemu/emulator-Prozess, `adb devices` leer. Start `emulator -avd Medium_Phone_API_36_pe2 -port 5586
  -no-window -no-audio -gpu swiftshader_indirect -no-snapshot -wipe-data -no-boot-anim` (12:04), `sys.boot_completed`
  12:06:27; Android 16 / API 36, `ro.build.type=user`, x86_64, /data 12G (11G frei), `de.re15.port` nicht installiert;
  `settings put secure immersive_mode_confirmations confirmed`.
- Werkzeuge (eigene): `pe2_adb.sh` (nur `-s emulator-5586`), `pe2_lauf.sh` (install -r nur mit Zeile `Success`,
  installierte base.apk per sha256 benannt, Kaltstart, Warten auf `Entpacken fertig|Assets aktuell|ABBRUCH`, logcat Tag
  re15), `pe2_geraet.py` (sha256 JEDER Datei unter shared_assets/ + synchro/ per toybox gegen die Liste der APK,
  Zusatzdateien, `.neu` ueberall, leere Ordner, Liste "zuletzt entpackt", alter Marker; dazu je Datei
  `inode|groesse|mtime` fuer "genau diese Datei neu geschrieben"), `pe2_bild.sh` (screencap).
- APKs: **REF** = frische Kopie der Archiv-APK v0.8.19 (`514bebd5...` = Archiv-SUMS); **N** = Bau aus 1.; **M** = derselbe
  Code + EINE gleich grosse Inhaltsaenderung, ECHT gebaut - aber in einer **Sandbox** (`build/r34a/pruefer_e2/sb`), damit
  der Quellbaum des Arbeitsbaums keine Minute veraendert ist (der zweite Pruefer arbeitet darin): release-Skripte + Pin +
  Code als Kopie (per `diff -rq`/`cmp` = Arbeitsbaum), `shared_assets/` und `synchro/` als Hardlinks, `_deps` kopiert.
  Geaendert NUR in der Sandbox: `synchro/STAGE1/room1240/main04.wav` (2017588 B) Byte 1500001 (hinter dem ersten MiB)
  0xb6 -> 0xec per neuer Datei + rename (Hardlink aufgebrochen: Sandbox-Linkzahl 1, Quelle unveraendert
  `d6e26832...`, `git status` leer). sha256 neu `a1d533bf...`. `sb/release/build_android.sh --version v0.8.19-pe2m
  --no-toolchain` (12:05:46-12:10:15, Auszug `android_M_sandbox_auszug.txt`): **EXIT 0**, stageAssets UND
  writeAssetManifest liefen frisch (`re15_assets.txt (v2): 3603 Dateien, 356678277 Bytes, sha256 je Datei`),
  versionCode 81900, Signer `432bc749...`, Selbsttest 258/258 + 132, 3603/3603 bytegleich; APK **M** =
  `222c4bd8970039fec2f478e0264e2fa9f898221319dbcfae5f56f79280b7f6e8`. `pe2_apk_liste.py` M gegen den Sandbox-Baum:
  LISTE-OK (`apk_liste_M.txt`). N gegen M (`liste_N_gegen_M.txt`): Listen je 398099 B, **genau eine Zeile anders**
  (main04.wav, nur die sha256); unter assets/ genau zwei Eintraege mit anderer CRC (Liste + main04.wav).

### 3.1 Referenz v0.8.19 frisch (`geraet/a_ref0819_frisch.txt`, `geraet/a_zustand.txt`)
install `Success` (33,4 s), installiert `514bebd5...`; alter Entpacker: `Entpacke 3603 Dateien (356678277 Bytes)` ->
`Entpacken fertig: 3603 geprueft, 3603 kopiert, 0 Fehler` (30 s). Speicherordner `/storage/emulated/0/Android/data/
de.re15.port/files (extern=1)`. Oberste Ebene `debug.log re15_assets_ok.txt shared_assets synchro`, Marker
`af45d06a3555f997 3603 356678277`; gegen die Liste von N: 3603 Dateien, fehlen 0, sha256 falsch 0, zusaetzlich 0,
keine v2-Liste (erwartet fuer v0.8.19).

### 3.2 Update v0.8.19 -> N: Uebergang + Titelbild (`geraet/b_*.txt`)
install -r `Success` (29,7 s), lastUpdateTime neu, firstInstallTime alt, installiert `2c6a1c30...` (= N):
```
10:10:03.027 [android] Abgleich (Uebergang v0.8.19): 3603 Dateien (356678277 Bytes) - behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
10:10:19.582 [android] Entpacken fertig (Uebergang v0.8.19): 3603 geprueft, 0 kopiert (0 B, 0 ms), 3603 per SHA-256 geprueft
             (356678277 B, 13254 ms, 0 abweichend), 0 entfernt, 0 Waisen entfernt (0 nicht loeschbar), 0 .neu-Reste, 0 Fehler, 17294 ms
```
(logcat-Zeit = UTC, Host = UTC+2.) `pe2_geraet.py N`: 3603/0/0/0, 0 `.neu`, 0 leere Ordner, `re15_assets_entpackt.txt` =
Liste von N (`95770eb5...`), Marker weg -> **GERAET-KONSISTENT**; inode/Groesse/mtime aller 3603 Dateien unveraendert
(nur geprueft, nichts neu geschrieben). Bildschirm (10 Bilder im ~7-s-Takt ab 12:10:29, ANGESEHEN): Bild 1 Intro-Effekt
(`geraet/b_intro_121029.png`), ab Bild 3 (12:10:44, `geraet/b_titelbild_nach_uebergang_121044.png`) **Titelbild
"BIOHAZARD 2 Sample Ver.2025.01.25", NEW GAME / LOAD GAME / OPTION**, Touch-Overlay; Bild 10 (12:11:35) unveraendert,
Prozess laeuft (pidof 6447).

### 3.3 Zweites Update N -> M: gleich grosse Inhaltsaenderung (`geraet/c_*.txt`)
Vorher auf dem Geraet: main04.wav inode 500213, mtime 10:08:00.86, sha256 `d6e26832...`; stat aller Dateien festgehalten.
install -r M `Success` (34,0 s), installiert `222c4bd8...` (= M), versionName v0.8.19-pe2m:
```
10:14:48.334 [android] Abgleich (Update): 3603 Dateien (356678277 Bytes) - behalten 3602, geaendert 1, neu 0, pruefen 0, weg 0
10:14:50.592 [android] entpacke synchro/STAGE1/room1240/main04.wav (geaendert)
10:14:50.984 [android] Entpacken fertig (Update): 3603 geprueft, 1 kopiert (2017588 B, 122 ms), 0 per SHA-256 geprueft (0 B, 0 ms,
             0 abweichend), 0 entfernt, 0 Waisen entfernt (0 nicht loeschbar), 0 .neu-Reste, 0 Fehler, 2827 ms
```
- stat vorher/nachher ueber alle 3603 Dateien (`geraet/c_stat_diff.txt`): **genau eine** Datei anders -
  main04.wav, neuer inode 501240 (rename der .neu), mtime 10:14:50.672.
- `adb pull`: sha256 **`a1d533bf...` = Inhalt in APK M** (aus M gelesen: gleich), `cmp` gegen die Sandbox-Quelle rc 0;
  `cmp -l` gegen die Original-Sicherung: genau `1500002 266 354` (Byte 1500001 0-basiert: 0xec statt 0xb6).
- `pe2_geraet.py M`: 3603/0/0/0, 0 `.neu`, Liste "zuletzt entpackt" = Liste von M (`1b017f2b...`) -> **GERAET-KONSISTENT**.

### 3.4 Update ohne Aenderung -> schneller Weg (`geraet/d1_*.txt`, `geraet/d2_*.txt`, `geraet/d_geraet.txt`)
- dieselbe APK M noch einmal als Update (install -r `Success`, neuer /data/app-Pfad, lastUpdateTime 10:16:17):
  `Assets aktuell (schneller Weg): Liste = zuletzt entpackt, 3603 Dateien, Groessen geprueft, 343 ms`;
- Neustart (force-stop + Kaltstart): `Assets aktuell (schneller Weg): ... 694 ms`;
- stat aller 3603 Dateien danach: 0 Unterschiede; GERAET-KONSISTENT.

