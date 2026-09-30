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

`bash release/build_android.sh --version v0.8.19 --no-toolchain` im Arbeitsbaum (Log
`build/r34a/pruefer_e1/logs/android_voll.log`, Auszug `android_voll_auszug.txt`), 09:52:11-09:54:50:
**EXIT 0, `ANDROID-BUILD-OK`**. Python `/c/Python310/python (3.10.11)` (python_finden.sh). CMake-Cache verworfen,
beide ABIs neu uebersetzt (`buildCMakeRelWithDebInfo[arm64-v8a]` + `[x86_64]`), `BUILD SUCCESSFUL in 2m 15s`
(Asset-Tasks UP-TO-DATE: die Staging-Kopie stammt aus dem N2-Bau von N1 mit gleichem Inhalt - das Gate danach
vergleicht jede Datei der APK mit dem Quellbaum). Kette auf der Pruefkopie: aapt `versionCode='81900'
versionName='v0.8.19'`, beide ABIs, zipalign ok, v2 true, 1 Signer `432bc749...`, Selbsttest **248/248 + 116/116**,
Tuer-Soll RE15DOOR 30/30 + RE2/DOOR 27/27, ZIP 3616/3616 lesbar, **3603/3603 bytegleich** (356678277 B), Manifest
3603 Zeilen. APK **N** = `a340a325855801302a6f916061996e7949a3b217e5ff3f24530e3d0bdcadacbc`, 363479879 B
(Kopie `build/r34a/pruefer_e1/apk/N_v0.8.19.apk`). `release/SHA256SUMS_android.txt` danach per `git restore` auf HEAD.

Unabhaengig (eigenes Skript `pe1_apk_liste.py`, nicht das Gate; `apk_liste_N.txt`): Liste in N 398099 B, sha256
`95770eb5...`, Kopf `# re15 assets v2 3603 356678277`, jede Zeile `<bytes>	<64 hex>	<pfad>`, sortiert, keine
(auch Gross/klein-)Dubletten, Kopf = Summe; Eintraege unter `assets/` = Liste + re15_assets.txt; je Zeile sha256 +
Groesse der APK-Daten UND der Quelldatei = Liste: **0 Abweichungen, LISTE-OK**.

## 2. make_package.sh echt (Binaries aus Archiv v0.8.19) + "nur PC mit altem Android-Satz"

## 3. Emulator headless: v0.8.19 -> neue APK -> Update mit gleich grosser Aenderung -> Update ohne Aenderung -> force-stop mitten im Entpacken

### 3.0 Aufbau
- **Eigene AVD-Kopie** `Medium_Phone_API_36_pe1` unter `build/r34a/pruefer_e1/avd/` (ANDROID_AVD_HOME): config.ini des
  Nutzer-AVD `Medium_Phone_API_36` (Ordner Medium_Phone.avd) mit genau 5 Aenderungen (`diff --strip-trailing-cr`):
  AvdId, Anzeigename, `disk.dataPartition.size = 12884901888` (statt 6 GiB), Kaltstart erzwungen. Grund: auf dem
  Nutzer-AVD scheitert ein Update an `INSTALL_FAILED_INSUFFICIENT_STORAGE` (von N1 gemessen, 1.3 Punkt 3); dort liegen
  fremde Apps, ich fasse ihn nicht an. Dasselbe System-Image (android-36 google_apis_playstore x86_64).
- Start `emulator -avd Medium_Phone_API_36_pe1 -port 5580 -no-window -no-audio -gpu swiftshader_indirect -no-snapshot
  -wipe-data -no-boot-anim` (vorher: `adb devices` leer, kein qemu/emulator-Prozess). Boot 09:58:42; Android 16,
  `ro.build.type=user`, x86_64, /data 12G (11G frei), `de.re15.port` nicht installiert.
  `settings put secure immersive_mode_confirmations confirmed` (Einmal-Hinweis des frischen AVD).
- Werkzeuge (eigene, nicht die von N1): `pe1_adb.sh` (nur `-s emulator-5580`, MSYS_NO_PATHCONV), `pe1_lauf.sh`
  (install -r mit Erfolgspruefung, installierte base.apk per sha256 identifiziert, Kaltstart, Warten auf die
  Abschlusszeile, logcat Tag re15), `pe1_geraet.py` (sha256 JEDER Datei unter shared_assets/ + synchro/ auf dem
  Geraet per toybox sha256sum gegen die Liste der APK, Zusatzdateien, *.neu, Liste "zuletzt entpackt", alter Marker).
- APKs: Referenz v0.8.19 = frische Kopie aus dem Archiv (`514bebd5...` = Archiv-SUMS); **N** = Bau aus 1.;
  **M** = derselbe Stand + EINE gleich grosse Aenderung, ECHT gebaut (`pe1_m_bau.sh`, Auszug `android_M_auszug.txt`):
  `shared_assets/PSX/STAGE4/ROOM4010.RDT` (297112 B) Byte 148556 0xf2 -> 0xa8, sha256 `23d5fdc1...` -> `c065442b...`,
  `build_android.sh --version v0.8.19-pe1m --no-toolchain` -> EXIT 0, stageAssets + writeAssetManifest liefen neu,
  Selbsttest 248/248, 3603/3603 bytegleich, APK `56ef3d26b9328097...` (versionCode 81900 wie N). Quelldatei nur
  09:56:37-10:00:00 geaendert (per rename; Linkzahl vorher 1), danach `git restore`: hash-object `d0ba28b8...` = Index,
  `cmp` gegen die Sicherung rc 0, `git status` leer. N gegen M (`liste_N_gegen_M.txt`): Listen je 398099 B, **genau eine
  Zeile anders** (ROOM4010.RDT, nur die sha256), genau zwei assets/-Eintraege mit anderer CRC (Liste + ROOM4010.RDT).

### 3.1 Referenz v0.8.19 frisch (`geraet/a_ref0819_frisch.txt`, `geraet/a_zustand.txt`)
install `Success` (29,3 s), installiert `514bebd5...`; alter Entpacker: `Entpacke 3603 Dateien (356678277 Bytes)` ->
`Entpacken fertig: 3603 geprueft, 3603 kopiert, 0 Fehler` (39,7 s). Oberste Ebene `debug.log re15_assets_ok.txt
shared_assets synchro`, Marker `af45d06a3555f997 3603 356678277`. Gegen die Liste von N: 3603 Dateien, fehlen 0,
sha256 falsch 0, zusaetzlich 0 (v0.8.19 hat dieselben Assets wie N), keine v2-Liste, Marker da.

### 3.2 Update v0.8.19 -> N: Uebergang + Titelbild (`geraet/b_uebergang_ref_zu_N.txt`, `geraet/b_geraet.txt`)
install -r `Success` (22,2 s), `lastUpdateTime` neu, `firstInstallTime` alt, installiert `a340a325...` (= N):
```
08:02:40.636 [android] Abgleich (Uebergang v0.8.19): 3603 Dateien (356678277 Bytes) - behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
08:03:08.888 [android] Entpacken fertig (Uebergang v0.8.19): 3603 geprueft, 0 kopiert (0 B, 0 ms), 3603 per SHA-256 geprueft
             (356678277 B, 18851 ms, 0 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 28364 ms
```
(Uhrzeiten logcat = UTC, Host = UTC+2.) `pe1_geraet.py N`: 3603/0/0/0, 0 `.neu`, `re15_assets_entpackt.txt` =
Liste von N (`95770eb5...`), Marker weg -> **GERAET-KONSISTENT**. Bildschirm (8 Bilder im 6-s-Takt ab 10:03:24,
ANGESEHEN): Bild 1 Intro-Effekt, Bild 8 (10:04:10, `geraet/b_titelbild_nach_uebergang.png`) **Titelbild "BIOHAZARD 2
Sample Ver.2025.01.25", NEW GAME / LOAD GAME / OPTION**, Touch-Overlay; Prozess laeuft (pidof 6277).

### 3.3 Zweites Update N -> M: gleich grosse Inhaltsaenderung (`geraet/c_*.txt`)
Vorher auf dem Geraet: ROOM4010.RDT sha256 `23d5fdc1...` (mtime 08:00:04 UTC), mtime aller 3603 Dateien festgehalten.
install -r M `Success` (10,0 s), installiert `56ef3d26...` (= M), versionName v0.8.19-pe1m:
```
08:05:57.634 [android] Abgleich (Update): 3603 Dateien (356678277 Bytes) - behalten 3602, geaendert 1, neu 0, pruefen 0, weg 0
08:05:58.864 [android] entpacke shared_assets/PSX/STAGE4/ROOM4010.RDT (geaendert)
08:05:59.048 [android] Entpacken fertig (Update): 3603 geprueft, 1 kopiert (297112 B, 56 ms), 0 per SHA-256 geprueft (0 B, 0 ms,
             0 abweichend), 0 entfernt, 0 .neu-Reste, 0 Fehler, 1456 ms
```
- mtime-Vergleich vorher/nachher ueber alle 3603 Dateien: **genau eine** Datei neu geschrieben (ROOM4010.RDT, 08:05:58).
- `adb pull`: sha256 **`c065442b...` = Inhalt in APK M**, `cmp` gegen die aus M gelesenen Bytes rc 0; `cmp -l` gegen die
  Original-Sicherung: genau `148557 250 362` (Byte 148556 0-basiert: 0xa8 statt 0xf2) - der neue Inhalt ist auf dem Geraet.
- `pe1_geraet.py M`: 3603/0/0/0, 0 `.neu`, Liste "zuletzt entpackt" = Liste von M (`ed0433e3...`) -> **GERAET-KONSISTENT**.

### 3.4 Update ohne Aenderung -> schneller Weg (`geraet/d1_*.txt`, `geraet/d2_*.txt`)
- dieselbe APK M noch einmal als Update (install -r `Success`, neuer /data/app-Pfad, lastUpdateTime 08:06:50):
  `Assets aktuell (schneller Weg): Liste = zuletzt entpackt, 3603 Dateien, Groessen geprueft, 134 ms`;
- Neustart (force-stop + start): `Assets aktuell (schneller Weg): ... 349 ms`;
- mtime aller 3603 Dateien danach unveraendert (0 Aenderungen).

### 3.5 Gegenrichtung M -> N (`geraet/e_*.txt`)
`Abgleich (Update): ... behalten 3602, geaendert 1` -> `entpacke shared_assets/PSX/STAGE4/ROOM4010.RDT (geaendert)` ->
`Entpacken fertig (Update): 3603 geprueft, 1 kopiert (297112 B, 29 ms), ... 0 Fehler, 1541 ms`; genau diese eine mtime
neu; `adb pull` == Original-Sicherung (`cmp` rc 0, `23d5fdc1...`); `pe1_geraet.py N` -> GERAET-KONSISTENT.

### 3.6 force-stop mitten im Entpacken + Neustart (`pe1_abbruch.sh`, `geraet/f1_*.txt`, `geraet/f2_*.txt`)
`adb uninstall` (externer Ordner danach weg), N frisch installiert (`Success`), logcat geleert; eine Shell-Schleife AUF
dem Geraet wartet auf `shared_assets/PSX/MOVIE/CAPCOM.STR.neu` (6148352 B, Eintrag 2483 von 3603) und ruft sofort
`cmd activity force-stop de.re15.port`; danach App-Start. Zustand nach dem Abbruch:
- Prozess beendet; logcat endet nach `Abgleich (ohne Liste): ... pruefen 3603` (keine Abschlusszeile);
- `CAPCOM.STR.neu` **3145728 B (halb geschrieben)**, `CAPCOM.STR` **existiert nicht** (nie umbenannt), 2484 Dateien
  (2483 fertige + das .neu), `synchro/` noch nicht angelegt, **keine** `re15_assets_entpackt.txt`, kein Marker.
Neustart (`pe1_lauf.sh`, keine neue APK):
```
08:10:31.408 [android] Abgleich (ohne Liste): 3603 Dateien (356678277 Bytes) - behalten 0, geaendert 0, neu 0, pruefen 3603, weg 0
08:10:45.937 [android] Entpacken fertig (ohne Liste): 3603 geprueft, 1120 kopiert (197043714 B, 6730 ms), 2483 per SHA-256 geprueft
             (159634563 B, 4768 ms, 0 abweichend), 0 entfernt, 1 .neu-Reste, 0 Fehler, 14543 ms
```
(2483 geprueft + 1120 kopiert = 3603; der eine `.neu`-Rest entfernt.) `pe1_geraet.py N`: 3603 Dateien, fehlen 0, sha256
falsch 0, zusaetzlich 0, **`.neu`-Reste 0**, Liste = N -> **GERAET-KONSISTENT**. Bild danach (angesehen,
`geraet/f_titelbild_nach_abbruch_neustart.png`, 10:11:37): Titelbild wie in 3.2.

## 4. PC-Suite local_build.sh all

## 5. Kein Python-Installer

## 6. Befunde

## 7. Urteil
