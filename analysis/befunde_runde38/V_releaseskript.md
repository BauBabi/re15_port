# Runde 38 Spur V — releaseskript (Dossier, laufend)

Baum `.claude/worktrees/r38_releaseskript` (Zweig `r38/releaseskript`, Basis e2cc7673). Angelegt 2026-10-16 16:28 (Uhr).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Ich möchte eine einfache Möglichkeit - über Skript oder sowas - die Ports für alle Systeme zu bauen - der neuen
> Version eine neue Versionsnummer - minor - zukommen zu lassen und die alte ins Archiv zu bringen.

Lesart des Orchestrators (AUFTRAG.md, "Lesarten"): "minor" = wie alle bisherigen Releases die LETZTE Stelle hochzaehlen
(0.8.25 -> 0.8.26, 0.8.99 -> 0.9.0 wie v0.7.99 -> v0.8.0); das Skript nimmt optional eine explizite Version.

## Stand

(laufend, neueste Eintraege oben)

* 2026-10-16 16:55: Messung vorher + Ist-Ablauf belegt (Abschnitte 1/2), Entwurf (Abschnitt 3) folgt.
* 2026-10-16 16:28: Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE Runde 38, UEBERGABE Runde 37 §4,
  Memory-Notizen zur Release-Kette gelesen.

## 1. Messung vorher (2026-10-16, ~16:30-16:55, gemessen, nicht vermutet)

Es gibt KEIN Skript, das die Kette faehrt. Ein Release sind heute vier Einzelskripte + Handarbeit (Archiv, Commit, Tag,
Push); die Version wird an vier Stellen von Hand getippt (build_android, make_package, Archivordner, Tag).

| # | Messung | Ergebnis |
|---|---|---|
| M1 | `grep -n '^VERSION=' release/make_package.sh` | Zeile 69 `VERSION="v0.1.2"` - ein Aufruf ohne `--version` schnuert Pakete namens v0.1.2 |
| M2 | `release/build_android.sh:193` Standard = `git describe --tags --always` | heute `v0.8.25-6-gfed84ccd` (Spurbaum) bzw. `v0.8.25-7-g98d35560` (Hauptbaum) - APK/versionName mit Suffix statt neuer Nummer |
| M3 | `timeout 30 docker info` | rc=1, `open //./pipe/dockerDesktopLinuxEngine: The system cannot find the file specified` - Docker Desktop laeuft nicht; build_win_cross.sh:36-38 bricht dann mit "Weder docker noch podman nutzbar." ab |
| M4 | Tags (`git tag --list 'v*' --sort=v:refname`) | hoechster v0.8.25; Uebertraege v0.3.99 -> v0.4.0 und v0.7.99 -> v0.8.0, aber v0.8.9 -> v0.8.10 (kein Uebertrag bei 9); fruehe Ausreisser v0.2.9->v0.3.0, v0.4.9->v0.5.0, v0.5.9->v0.6.0, v0.6.9->v0.7.0; dazu Nicht-Versions-Tags `v0.1`, `v0.2`, `backup-before-pull-ed20f6d3`, `itembox-hybrid-v1`; v0.8.24/v0.8.25 leichtgewichtig, v0.8.20/v0.7.99 annotiert |
| M5 | Archiv `C:/workspace/Re15Data/re15_packages_archiv` (PowerShell) | 9 Ordner: v0.8.7 .. v0.8.14 (je 820/821 MB) + v0.8.25 (894 MB); je 6 Volumes + APK + SHA256SUMS.txt + SHA256SUMS_android.txt (v0.8.12-14 zusaetzlich RELEASE_NOTES.md) |
| M6 | `Get-PSDrive C` | frei 131 578 617 856 B (122,5 GiB) |
| M7 | Hauptbaum `release/` | v0.8.25-Volumes (6, getrackt) + APK v0.8.25 (385 208 598 B) UND die APK v0.8.24 (379 951 560 B, 2026-10-12) - alte APKs raeumt niemand weg (gitignoriert `.gitignore:63`, make_package.sh entfernt nur `re15_port_v0*.z*`, Zeilen 1018-1030) |
| M8 | Hauptbaum `git status --porcelain --untracked-files=all -- re15_port` | 0 Eintraege (sauber); `-- synchro`: 33 unversionierte Eintraege, darunter `synchro/STAGE2/room2001/` - make_package.sh copy_common (Zeilen 672-685) packt synchro/STAGE* aus dem ARBEITSBAUM, unversionierte Aufnahmen landen also im Paket, aber nicht im Release-Commit |
| M9 | Bau-Ausgaben gitignoriert? (`git check-ignore -v`) | re15_port/build*, android app/build, app/.cxx, _deps, .gradle, local.properties, release/{win_out,linux_out,lbuild,pkg-win,pkg-linux,wxbuild} alle ignoriert -> ein Bau macht re15_port/ nicht "schmutzig" |
| M10 | Platzbedarf je Lauf (Hauptbaum nach v0.8.25, PowerShell) | pkg-win 364 MB + pkg-linux 364 MB + android app/build 1530 MB + app/.cxx 74 MB + 6 Volumes ~535 MB + APK 385 MB (+ .ungeprueft- und Pruefkopie je 385 MB) + Archiv neu 894 MB (+ alt 894 MB, falls es fehlt) = ~6,2 GB auf C:, dazu die Docker-VM |

Folgen fuer den Nutzer heute: Version an vier Stellen von Hand, zwei Standardwerte falsch (M1, M2), Docker muss
von Hand gestartet werden (M3), Fehlschlaege einzelner Schritte nur ueber Rueckgabewert + Datum erkennbar
(Memory reai-v2-releasebau-pipe-schluckt-fehler: v0.8.15 meldete ein gepipter Linux-Bau exit 0 trotz rotem ctest).

## 2. Belege (Ist-Ablauf, Skripte, Zeilen)

Nicht Spielverhalten, sondern Bau-Umgebung: es gibt keine Original-Adresse. Belegt wird jede Regel/Konstante mit
Datei:Zeile oder Messung (Muster der vorhandenen Skripte: "Port-Wahl, keine Original-Adresse (Bau-Umgebung)").

| Regel / Wert | Beleg |
|---|---|
| Uebertrag bei 99 (Z=99 -> Y+1.0) | `re15_port/platform/android/app/build.gradle:43-49`: versionCode = Major*1000000 + Minor*10000 + Patch*100 - Patch 100 ergaebe denselben Code wie Minor+1 (0.8.100 -> 90000 = 0.9.0), Minor 100 denselben wie Major+1; dazu die Tags v0.3.99 -> v0.4.0, v0.7.99 -> v0.8.0 (M4); AUFTRAG.md "Lesarten" |
| neue Version muss GROESSER sein als jede vorhandene | `release/build_android.sh:195-199`: kleinerer versionCode -> "adb install -r lehnt die APK als DOWNGRADE ab" |
| x.y.z Pflicht | `release/build_android.sh:199` (sonst versionCode 1) |
| Ablauf/Reihenfolge der Einzelschritte | Runde 37 UEBERGABE §4 (Zeilen 127-133) + Eintrag 2026-10-14 10:40; Runde 32 `release_v0818.md` (Protokoll aller Schritte) |
| Erfolgsmarken der Einzelskripte | `docker_win_build.sh:103` "WIN-CROSS-BUILD-OK", `docker_linux_build.sh:142` "LINUX-BUILD-OK" + `:131` "Tests: N gelaufen = N registriert", `build_android.sh:355` "ANDROID-BUILD-OK", `make_package.sh:1051` "== Fertig ==" |
| Artefakte | `release/win_out/re15_pc.exe` (docker_win_build.sh:102), `release/linux_out/re15_pc` (docker_linux_build.sh:133), `release/re15_port_<v>_android.apk` + `SHA256SUMS_android.txt` (build_android.sh:294, 348), Volumes + `SHA256SUMS.txt` (make_package.sh 839-976) |
| make_package baut nicht, tauscht aber die Volumes im git-Index | make_package.sh:979-1049 (`git rm --cached` alt, `git add` Positivliste neu; KEIN Commit) |
| SHA256SUMS*.txt werden NICHT vorgemerkt | make_package.sh:1040-1045 (nur Volumes) - Release-Commit 2077bb8e enthielt sie trotzdem (8 Dateien: 6 R + 2 M) -> der Commit-Schritt muss sie selbst adden |
| Docker-Start dauert ~20 s | Runde 32 `release_v0818.md:24` ("15:08:18 gestartet, 15:08:38 bereit") |
| zip nur aus /c/msys64/usr/bin, HINTEN anhaengen | make_package.sh:22-28 (vorn verliert MSYS2-bash LOCALAPPDATA/ANDROID_SDK_ROOT) |
| Archiv-Inhalt je Version | M5 + Runde 32 `release_v0818.md` Abschnitt 6 (6 Volumes + APK + beide SUMS, `sha256sum -c` 6x + 1x OK) |

### 2.1 "die alte ins Archiv bringen" gegen "alte Archivversion entfernen"

Wortlaut 2026-10-16: "... und die alte ins Archiv zu bringen"; VERTRAG.md Spur V: "packt und bringt die alte Version ins
Archiv". Der bisherige Ablauf (seit HANDOVER_2026-08-25 Zeile 175, Runde 32 release_v0818.md Abschnitt 6, Runde 37 §4)
legt dagegen die NEUE Version ins Archiv und LOESCHT dort die vorige ("liegt weiter in Git"). Mit dem Loeschen waere die
alte Version nach dem Lauf gerade NICHT im Archiv - das widerspricht dem neuen Wortlaut. Aeltere Nutzer-Vorgaben dazu:
make_package.sh:980-981 (2026-08-22) "alte Packages vom Repo loeschen" und Memory reai-v2-immer-paketieren (2026-08-24)
"auch im archiv, auch pushen und das letzte davor entfernen" - beide sprechen vom REPO (Pakete im Repo), nicht vom
Archiv; das Loeschen im Archiv ist eine spaetere Auslegung der Agenten. Umsetzung daher:
1. die alte Version (die Volumes in release/ vor dem Lauf) kommt ins Archiv bzw. wird dort geprueft (sha256sum -c), bevor
   irgendetwas gebaut wird;
2. die neue Version kommt zusaetzlich ins Archiv (stehende Anweisung "auch im archiv");
3. aus release/ verschwindet die alte Version (Volumes: make_package.sh; APK: nach geprueftem Archiv) - im Archiv bleibt sie;
4. im Archiv wird NICHTS geloescht. Entscheidungsfrage an den Nutzer in Abschnitt 7.

## 3. Umsetzung

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
