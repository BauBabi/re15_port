# Pruefer v0.8.17 — unabhaengige Pruefung am ausgelieferten Artefakt

Stand: 2026-09-29, Zweig r31/integration, HEAD e714d95f (release: v0.8.17).
Pruefer arbeitet NUR am Artefakt (release/*.zip/.z01/.apk + Archiv), nicht an der Quelle.

## Laufendes Protokoll

- [x] 1 Windows-Paket: frisch entpacken ausserhalb Repo, fremdes cwd, Asset-Herkunft, Tuer-Sequenz + FRAMEDUMP
- [x] 2 Linux-Paket in debian:11: x-Bit, run.sh LF, Start-rc, RE2/DOOR
- [x] 3 APK: RE2/DOOR 24, beide ABIs, Version
- [x] 4 Archiv sha256sum -c, v0.8.16 entfernt, Release-Commit-Inhalt

### 4. Archiv + Release-Commit (gemessen 2026-09-29)
- `release/`: `sha256sum -c SHA256SUMS.txt` 6/6 OK rc=0; `sha256sum -c SHA256SUMS_android.txt` 1/1 OK rc=0.
- Archiv `C:/workspace/Re15Data/re15_packages_archiv/v0.8.17/`: 6 Volumes + APK + beide SUMS;
  `sha256sum -c` 6/6 OK rc=0, APK 1/1 OK rc=0. SUMS-Dateien im Archiv == release/ == `git show HEAD:` (cmp).
- Archiv-Verzeichnisse: v0.8.7..v0.8.14, v0.8.17 — **kein v0.8.16** (entfernt), kein v0.8.15.
- Release-Commit e714d95f (`git show --name-status`): 2x M (SHA256SUMS.txt, SHA256SUMS_android.txt) + 6x R
  (v0.8.16 -> v0.8.17: android .z01/.zip, linux .z01/.zip, win64 .z01/.zip) = 8 Dateien, numstat 6/6 + 1/1
  Textzeilen; gleiche Form wie 2073ba13 (8 Dateien, 2 M + 6 R). Rename-Paarung linux.zip<->win64.zip ist nur
  Git-Aehnlichkeitserkennung (auch in 2073ba13 so).
- Blob-Gleichheit: `git rev-parse HEAD:<f>` == `git hash-object <f>` fuer alle 6 Volumes (BLOB_EQ 6/6).
- APK gitignoriert (`.gitignore:63 release/*_android.apk`), 0 APKs in `git ls-files release`; APK liegt im Archiv.
- `git ls-files release | grep re15_port_v`: nur v0.8.17 (6).
- Ergebnis Punkt 4: OK.

### 1. Windows-Paket (gemessen 2026-09-29)
- Quelle: Archiv-Kopie `re15_port_v0.8.17_win64.z01/.zip` (sha256 cf055d2b.../d5ed6c25... = SUMS), nach
  `<scratchpad>/pruefer_v0817/win` kopiert (ausserhalb des Repos), `zip -s 0` -> `unzip` (rc 0/0), 3574 Dateien.
- Paket-exe: sha256 `8227f8d91da796751dadfbe7b2f4de131cf6548f83b3af75640aa0b1746b7e7a` (= Bauer), 4354858 B;
  PE: e_lfanew 0x80, `PE\0\0`, Magic 0x020b, Subsystem 0x0002 (GUI).
- Paket-Baum: shared_assets/{PSX,RE2,extracted_fx}, synchro, exe, 2 .bat, README_0.8.17.txt;
  shared_assets/RE2/DOOR = 24 .DO2 + RE2/TORSE.VBS: `sha256sum -c` gegen re15_port/shared_assets/RE2 25/25 OK.
- Einkompilierter Rueckfall = `/src/re15_port/shared_assets[/PSX]` (Container-Pfad) -> auf dem Host `C:/src` existiert nicht.
- **Selbsttest** (cwd `pruefer_v0817/fremd_a`, env -u RE15_ASSET_ROOT/CD_ROOT/RE2_ASSET_ROOT, RE15_ASSET_SELFTEST=1,
  RE15_ASSET_DEBUG=1): EXIT=0, `[selftest] RESULT ok=26 missing=0`, 0 Treffer ausserhalb des Paketordners
  (inkl. 2x re2 CDEMD0.EMS/ENEMSE.VBS aus `<paket>/shared_assets/RE2/`). Wurzeln: cd[0]/shared[0]/base[0] = Paket,
  cd[1]/shared[1] = /src/... (nicht vorhanden), KEINE cwd-Wurzel.
- **Tuersequenz aus dem Paket** (cwd `pruefer_v0817/fremd_b`, gleiche env-Bereinigung, RE15_TUER_SEITE=S017,S058,
  RE15_TUER_BOGEN + RE15_TUER_SERIE): EXIT=0; debug.log:
  `[tuer-seite] S017 ROOM1050 DOOR13 V0` -> `[tuer] Sequenz Archiv 2 DOOR13 Variante 0 ... (9 Skripte)` ->
  `Sequenz fertig: 286 Bilder + 1 Warten, 1 Se_on, Schliesston 1, Ton geladen, Notizen 0x0`;
  `S058 ROOM1120 DOOR09 V1 Spender 04` -> `Griff-Tausch DOOR09 <- DOOR04: Spender-Griff geladen` -> 301 Bilder, Ton geladen.
  Bilder (Ruecklesen vor dem Present wie RE15_FRAMEDUMP) angesehen: S017 dunkle Metalltuer mit Fenster, Knauf links,
  zu -> aufschwingend; S058 Holz-Kassettentuer mit Messing-Stangengriff (DOOR04), zu -> aufschwingend.
- Hinweis: die Tuer-Logzeilen nennen den DO2-VOLLPFAD nicht (`re2_archiv_lesen` loggt nur beim Fehlen
  `shared_assets/RE2/DOOR/DOORxx.DO2 fehlt`). Herkunft darum ueber Wurzelliste + Gegenproben unten belegt.
- Nebenzeile im Log: `[asset] cannot open DATA/TEST.VH` / `DATA/TEST.VB` (siehe unten, ob auch im Repo-Lauf).

- **Gegenprobe 1 (kein Rueckfall ins Repo):** im entpackten Paket (Pruefer-Kopie) DOOR13.DO2 umbenannt, S017 erneut:
  `[tuer] shared_assets/RE2/DOOR/DOOR13.DO2 fehlt` -> `Archiv 2/13 nicht lesbar -> Tuerwechsel ohne Sequenz` — obwohl
  `re15_port/shared_assets/RE2/DOOR/DOOR13.DO2` im Repo liegt. Danach zurueckbenannt, sha256 9bd7d0b7... = Repo.
- **Gegenprobe 2 (das gezeichnete Modell IST die Paketdatei):** in der Pruefer-Kopie von DOOR13.DO2 die 255 nicht-schwarzen
  CLUT-Eintraege des TIM @0x52E4 (8 bpp, CLUT 256x1 @VRAM 0,480) auf 0x7C1F (Magenta) gesetzt, Groesse unveraendert
  (54532 B, besteht die Tabellenpruefung @0x8009a520). S017 erneut: Sequenz 286 Bilder, Notizen 0; Anfangs- und
  Mittelbild zeigen die Tuer MAGENTA (angesehen, `manipulation_S017.png` im Scratchpad: oben Original, unten manipuliert).
  Datei danach aus der Sicherung zurueck, sha256 9bd7d0b7... .
- **Echtlauf mit RE15_FRAMEDUMP aus dem Paket** (`tools/tueren/tuer_echtlauf.sh` mit RE15_EXE = Paket-exe, cwd
  `pruefer_v0817/echt_s017`, env bereinigt, RE15_ASSET_DEBUG=1; Titel-Vorlauf -> RE15_DEBUG_JUMP=1050@120 ->
  RE15_FIRE_AOT=0@500#1050 -> RE15_EXIT_AT=60#1030): exit=0; Log `AUTO-JUMP -> ROOM1050`, `PC loaded room1050.rdt`,
  `[fire-aot] slot=0 at F500`, `[tuer] Sequenz Archiv 2 DOOR13 Variante 0 ... Seite S017`, Se_on Bild 60 (16397 Hz,
  SE-Stimme 6), Schliesston SE-Stimme 7, `Sequenz fertig: 286 Bilder ... Ton geladen`, `PC loaded room1030.rdt`,
  `EXIT_AT: Bild 60 in Raum 1030`. Wurzeln wie oben (nur Paket + nicht vorhandenes /src). Bilder angesehen
  (`echtlauf_S017.png`): f_000498 = ROOM1050, Leon vor der Tuer (RE15_FRAMEDUMP); b_tuer_020/150/250 = DOOR13 zu,
  aufgehend, fast offen; f_000030/f_000060 = ROOM1030 eingeblendet (RE15_FRAMEDUMP).
- `[asset] cannot open DATA/TEST.VH/.VB`: audio_pc.c:526/527 sucht die Datei, sie existiert auch im Repo
  (shared_assets/PSX/DATA) und im Original-Baum nicht -> kein Paketdefekt.
- Ergebnis Punkt 1: OK.

### 2. Linux-Paket in debian:11 (gemessen 2026-09-29)
- Container `debian:11` (Debian GNU/Linux 11 bullseye), `release/` NUR LESEND eingehaengt, Split-Satz nach /w kopiert:
  sha256 5293e163.../33486b57... `sha256sum -c` 2/2 OK. Werkzeuge per apt (UnZip 6.00 of 20 April 2009, zip,
  binutils, file). Hinweis Pruef-Umgebung: das lokale debian:11-Abbild bekam fuer unzip 6.0-26+deb11u2 aus
  debian-security ein 404 -> Security-Quelle entfernt, unzip aus bullseye main (ebenfalls UnZip 6.00).
- `zip -s 0` rc=0, `unzip` rc=0; Paketordner re15_port_v0.8.17/, 3573 Dateien.
- **x-Bit:** `stat`: `-rwxr-xr-x re15_pc`, `-rwxr-xr-x run.sh`; Zentralverzeichnis (`unzip -Z`): beide `-rwxr-xr-x 3.0 unx`.
- **run.sh LF:** 0 CR, Shebang `#!/usr/bin/env bash`, `file`: "Bourne-Again shell script, UTF-8 Unicode text executable"
  (kein "with CRLF line terminators").
- **Binary:** ELF 64-bit x86-64 PIE, sha256 `54bb5b8c5a9c23c7113ff342eeec1ef06bcd015cdb86e17cbb0eb8f7142e7657` (= Bauer);
  `objdump -T` max GLIBC_2.29; `ldd` 0x "not found" (nacktes debian:11).
- **Start:** `./re15_pc --headless` rc=1 (keine Anzeige), `./run.sh --headless` rc=1 (Skript meldet Binary/Assets/Daten
  im Paketordner); **Negativ-Kontrolle** dieselbe Datei mit Modus 644: rc=126 -> das x-Bit ist echt, nicht selbstbestaetigt.
- **RE2/DOOR:** 24 .DO2 (DOOR04..DOOR31, gleiche Liste wie Windows) + RE2/TORSE.VBS; sha256 der 25 Dateien aus dem
  Container gegen re15_port/shared_assets/RE2: 25/25 OK.
- Keine Logs: Katalog des zusammengefuehrten Linux-Zips 3766 Eintraege, 0 Treffer (.log/.dmp/befund/LastTest/ctest/
  debug.log/re15_card); Wurzel nur re15_pc, run.sh, README_0.8.17.txt. (Die `debug.log`, die im Container nach dem Start
  im Paketordner lag, schrieb mein eigener Start — run.sh nennt den Paketordner als Datenordner.)
- Ergebnis Punkt 2: OK.

### 3. APK (gemessen 2026-09-29)
- APK aus dem Android-Split-Zip des ARCHIVS zusammengefuehrt + entpackt (Katalog: 1 Eintrag = die APK):
  sha256 `7d8ae4b4a4943fb6fee6994c525a69a9bafcddc2fb2a88c61d43f425e575d640` == Archiv-APK == release/-APK == SUMS_android.
- `aapt dump badging` (build-tools 36.0.0): `package: name='de.re15.port' versionCode='81700' versionName='v0.8.17'`,
  sdkVersion 24, targetSdk 35, `native-code: 'arm64-v8a' 'x86_64'`; lib/: libSDL2.so + libmain.so je ABI.
- Assets: 3571 `assets/`-Eintraege; `assets/shared_assets/RE2/DOOR/DOORxx.DO2` = 24 (DOOR13: stored, 54532 B),
  `RE2/TORSE.VBS` = 1; entpackt, sha256 gegen re15_port/shared_assets/RE2: 25/25 OK.
- libmain.so aus der APK (llvm-nm NDK r27.2): arm64-v8a 1310640 B / x86_64 1370168 B; je 18 `T re15_door_seq_*`,
  6 `T re15_hebetisch_*`, String `DOOR/DOOR%02X.DO2` je 1x.
- Signatur: Android-Debug-Zertifikat SHA-256 432bc749..., gleich wie die Archiv-APK v0.8.14 (keine Aenderung).
- Ergebnis Punkt 3: OK.

### Nebenbefunde
- (niedrig) Die Tuer-Logzeilen nennen den DO2-Vollpfad nicht; der Auftrag "Log: Pfad der DO2" ist darum ueber
  RE15_ASSET_DEBUG-Wurzelliste + Gegenprobe 1 (Datei weg -> "fehlt", kein Repo-Rueckfall) + Gegenprobe 2 (Datei
  manipuliert -> Bild magenta) belegt, nicht ueber eine Pfadzeile.
- (niedrig, Host) Die vom Bauer gemeldete Python-Nebenwirkung hat 168 MB unter `release/Python/` IM Arbeitsbaum
  abgelegt (_cache, bin, pythoncore-3.14-64, 10:55:43-10:55:50) plus `release/python_install_*.log`. Beides ist
  gitignoriert (`.gitignore:5 release/Python/`, `release/.gitignore:6 *.log`) und in KEINEM Paket (Katalog-Suche
  "python": Windows 0, Linux 0, APK 0). Aufraeumen ist Sache des Leiters.
- (info) Meine eigenen Laeufe schrieben `befund.log` in die Pruefer-Kopie des Windows-Pakets bzw. `debug.log` in die
  Container-Kopie — nur in den Kopien; die Kataloge der ausgelieferten Zips enthalten 0 Logs.

## Urteil: haltbar
Alle vier Punkte am ausgelieferten Artefakt belegt (Pakete aus release/ bzw. dem Archiv, entpackt ausserhalb des Repos /
im nackten debian:11). Kein Spielcode geaendert; einziger Commit dieses Pruefers = dieses Dossier (liegt nach dem
Release-Commit e714d95f; der Tag gehoert auf e714d95f).
