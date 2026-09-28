# Nachtrag L — Warum der Linux-Bau über eine Stunde dauerte (Runde 30, 2026-09-28/29)

> Ich will auch das du untersuchst warum der Linux bau über eine Stunde dauert. Das ist doch
> nicht normal...  (AUFTRAG.md, Abschnitt L)

**Kurz:** Nicht der Compiler und nicht die Tests sind langsam, sondern der **Dateizugriff**.
`release/build_linux_deck.sh` hängte das Repo aus Windows per Bindmount als `/src` in den
Container. Unter Docker Desktop (WSL2) ist das ein **9p-Mount** mit rund **10 ms je
Dateioperation**. Das Bauverzeichnis `release/lbuild` lag auf demselben Mount. Ein kalter
Lauf brauchte deshalb gemessen **4906 s (81,8 min)** und war dazu **rot**: zwei Tests
liefen allein wegen des langsamen Mounts in ihre Zeitgrenze. Der neue Weg kopiert den
Quellbaum in das Container-Dateisystem. Er braucht **469 s bzw. 471 s (7,8 min)**, das
Binary ist bis auf Zeitstempel und Build-ID byte-gleich, und die Tests liefern dasselbe
Ergebnis wie unter Windows.

Stand: `80d579a5` (Integrationsstand Runde 30), Zweig `r30/n-linux-bau`.
Die Werkzeuge liegen unter `analysis/befunde_runde30/linux-bau/`.

---

## 1. Umgebung (gemessen)

| | |
|---|---|
| Host | i7-12650H, 10 Kerne / 16 Threads, 32 GB, Windows 11 |
| Docker | Docker Desktop 27.5.1, Backend **WSL2** (`Kernel 5.15.167.4-microsoft-standard-WSL2`, `Operating System: Docker Desktop`), keine `.wslconfig` |
| VM-Ressourcen | 16 CPUs, 15,47 GiB (`docker info`), also keine Drosselung unter der Host-Kernzahl |
| Mount `/src` (alter Weg) | `C:\134 /src 9p rw,dirsync,noatime,aname=drvfs;path=C:\;…;msize=65536,trans=fd` (`/proc/mounts` im Container) |
| Bau-/Testverzeichnis im Container (neu) | `overlayfs` (Container-Dateisystem, `stat -f`) |

⛔ Die Memory `reai-v2-release-baum-warmer-cache` schreibt „Docker läuft hier in einer
qemu-VM". Das stimmt nicht: Es ist die WSL2-VM von Docker Desktop, gemessen per `docker info`.

**Last:** Parallel liefen Agenten-Bauten auf dem Host. Ich habe alle 30 s die Host-Last
aufgezeichnet (`Win32_Processor.LoadPercentage`, Zählung fremder cc1/ninja/ld/ctest/re15_pc-Prozesse).
Die Werte enthalten die eigene VM-Last:

| Lauf | Zeitraum (UTC) | Host-Last Mittel / Max | fremde Bauprozesse Mittel |
|---|---|---|---|
| alter Weg `alt1` | 20:24:21 – 21:46:10 | 23 % / 100 % | 1,7 |
| Spurlauf `spur0` (strace) | 21:46:45 – 21:56:00 | 30 % / 79 % | 0,1 |
| neuer Weg `neu1` | 21:57:45 – 22:05:34 | 29 % / 100 % | 0,9 |
| neuer Weg `neu2` | 22:23:58 – 22:31:49 | 27 % / 100 % | 0,3 |

Alt und neu liefen also unter vergleichbarer Last. Der Unterschied ist zu groß, um von der
Last zu kommen.

## 2. Dateizugriff: 9p-Bindmount gegen Container-Dateisystem

`linux-bau/io_probe.sh` lief in `debian:11`, mit dem Arbeitsbaum als `/src`:

| Probe | `/src` (9p) | `/tmp` (Container) | Faktor |
|---|---|---|---|
| A `find -printf %s` über engine+include (193 Dateien) | 2,836 s | 0,004 s | ~700 |
| D alle `.h` lesen (`cat`) | 2,583 s | 0,003 s | ~860 |
| E 5000× `stat` (je ein Prozess) | 54,4 s | 3,3 s | 16 (Prozessstart abgezogen: ~10 ms gegen ~0 je stat) |
| F 2000 Dateien à 1 KB schreiben | 93,6 s | 1,1 s | 85 |
| G diese 2000 Dateien löschen | 29,9 s | 0,04 s | ~790 |
| H 6 MB lesen (`CAPCOM.STR`), 1. / 2. Mal | 1,05 / 0,80 s | 0,002 s | ~400 |
| I ganz `shared_assets` lesen (315 MB) | 142,0 s | 0,17 s | ~830 |
| B `tar` von `re15_port` (338 MB, 4605 Dateien) über den Mount | 448 s | — | nativ auf dem Host: **1,8 s** |

Der Host liest dieselben Daten nativ in Sekundenbruchteilen. Der Einbruch sitzt im 9p-Weg
in die VM. Kleine Dateioperationen kosten dort ~10 ms, große Lesezugriffe schaffen ~6–8 MB/s.

## 3. Der alte Weg, kalt gemessen (`alt1`)

Das alte Skript von `80d579a5` lief unverändert: `docker run -v <baum>:/src -w /src debian:11
bash docker_linux_build.sh`. Jede Ausgabezeile bekam einen Zeitstempel (`linux-bau/ts_run.sh`).
Das Bauverzeichnis war frisch, wie in jedem neuen Release-Arbeitsbaum.

| Phase | Dauer | Beleg |
|---|---|---|
| apt aus snapshot.debian.org + cmake-Tarball von GitHub, **bei jedem Lauf** | 81 s | erste cmake-Zeile bei 81,1 s |
| Configure (SDL2-FetchContent + ~200 SDL2-Prüfungen, je **10–19 s**) | 1061,6 s | `Configuring done (1061.6s)` |
| Generate (build.ninja schreiben) | 267,6 s | `Generating done (267.6s)` |
| Compile + Link (1638 Kanten) | 926 s | `[1/1638]` bei 1499,6 s, `[1638/1638]` bei 2354,7 s |
| – davon Summe der Übersetzungskanten (928) | 2988 s | `.ninja_log`, `linux-bau/ninja_phasen.sh` |
| – davon Summe der Link-Kanten (815, v.a. ~600 Test-Executables auf den Mount) | **10717 s** | dito |
| ctest seriell (403 Tests) | **2550,8 s** | `Total Test time (real) = 2550.84 sec` |
| **gesamt** | **4906,6 s = 81,8 min** | `ENDE rc=8` |

**Ergebnis: rot (rc=8), 4 von 403 fehlgeschlagen.**
- `unit_sld_atlas` (Timeout): Grenze 120 s, der Test lief 119,66 s. Neu braucht er 0,08 s.
- `unit_r21_discard_wegwerfen` (Timeout): 239,98 s. Neu braucht er 0,25 s.
- `integration_r30_irons_tisch_licht` (Failed) und `integration_r30_titel_puls` (Failed):
  Beide sind auch auf dem neuen Weg rot, also **unabhängig vom Mount** (siehe §8).

Je Test gemessen: Median-Faktor alt/neu **11,7** über die 48 Tests mit mehr als 0,05 s.
Die größten Faktoren: `unit_bss_room_source` 368,48 s gegen 0,06 s, `unit_sld_atlas` 119,66
gegen 0,08, `unit_stage6_hintergrund` 85,80 gegen 0,07, `unit_r21_discard_wegwerfen` 239,98
gegen 0,25, `unit_pri_kopfschnitt` 72,14 gegen 0,32, `unit_irons_mittelmodell` 52,89 gegen 0,34.
Das sind genau die asset-lastigen Tests.

**Mechanismus in einem Satz:** Jeder Schritt des Baus besteht aus vielen kleinen
Dateioperationen. Das sind die SDL2-`try_compile`-Prüfungen mit je einem eigenen kleinen
CMake-Projekt, die Header-Suche, die Objekt- und Executable-Dateien und das Lesen der
Test-Assets. Jede dieser Operationen kostete über 9p ~10 ms statt Mikrosekunden.
Dazu kamen 81 s apt pro Lauf.

## 4. Lösung

1. **Quellbaum als Kopie** (`release/docker/kopie_lauf.sh`, Host-Seite):
   - Die Dateiliste kommt aus `git ls-files --cached --others --exclude-standard` über die
     KOPIE-Pfade, also versioniert plus unversioniert-nicht-ignoriert. Im Arbeitsbaum
     gelöschte Dateien fallen heraus.
   - Der Host packt die Liste nativ in ein tar: 17 s für 18577 Dateien bzw. 550 MB.
   - Das tar geht per stdin **aus einer Datei** in den Container. ⛔ Aus einer Git-Bash-Pipe
     schafft `docker.exe` nur 8 MB/s (200 MB in 24,7 s). Aus einer Datei sind es 200 MB in
     6,4 s. `docker cp` war mit 33 s für re15_port nicht schneller.
   - Ausgepackt wird unter **`/src`** (14–15 s). Das ist derselbe Pfad wie beim alten
     Bindmount. Die im Binary eingebauten Pfade (`RE15_ASSET_ROOT_DEFAULT` =
     `/src/re15_port/shared_assets/PSX`, `__FILE__`) bleiben damit gleich.
   - `release/linux_out` wird direkt eingehängt. Dorthin gehen nur das Binary und die Diagnose.
2. **Kein stilles Überspringen, durch Konstruktion.** Das Repo hängt zusätzlich unter
   `/host`. `release/docker/rueckfall_links.sh` legt im Container für **alles Nicht-Kopierte**
   einen Link `/src/<pfad> -> /host/<pfad>` an:
   - die oberste Ebene,
   - die Geschwister teilweise kopierter Verzeichnisse,
   - jeden git-ignorierten Eintrag in kopierten Verzeichnissen (`git ls-files --others --ignored --directory`).

   Jede Datei, die es im Repo gibt, gibt es damit auch im Container. Ein KOPIE-Eintrag
   entscheidet nur über die Geschwindigkeit, nie über das Ergebnis. Nie verlinkt werden
   `release/lbuild` (das Bauverzeichnis gehört ins Container-Dateisystem, sonst bricht das
   Skript ab) und `release/linux_out`.
3. **Vorgebautes Image** `re15-linux-build:deb11` aus `release/docker/Dockerfile.linux`:
   `debian:11` plus `release/docker/linux_deps.sh`. Dieser Block steht wörtlich so, wie er
   vorher in `docker_linux_build.sh` stand: derselbe Snapshot `20260701T000000Z`, dieselbe
   Paketliste, cmake 3.28.6. Er ist die **eine** Quelle dafür. Der Erstbau dauerte 84 s,
   jeder weitere Lauf braucht dank Schichtcache **2–3 s** statt 81 s. Mit
   `--image debian:11` läuft die Installation wie früher.
   Geprüft: Im nackten `debian:11` per `source` geladen, liefert der Block gcc 10.2.1-6 und
   cmake 3.28.6. Das ist derselbe Compiler, den das alte Protokoll meldet (`GNU 10.2.1`).
4. **Phasenzeiten** in beiden Skripten (`== Phase …`) und **Diagnose nach
   `release/linux_out/diag/`**, auch bei Rot:
   - `ctest_out.txt`, `LastTest.log` und `.ninja_log`,
   - `ctest_fingerprint.tsv`/`.skip` (`release/docker/ctest_fingerprint.sh`): je Test der
     Status, die Zahl der SKIP-, fehlt- und übersprungen-Zeilen und die Dauer,
   - bei rotem ctest das Binary als `re15_pc.UNGEPRUEFT-ctest-rot`. `linux_out/re15_pc`
     bleibt dann leer, und `make_package.sh` liest nur diese Datei.
5. **Gates:** Die bisherigen Gates bleiben unverändert: Summenzeile Pflicht, ctest-Rückgabe
   durchgereicht. Neu hinzu kommen drei:
   - Die Zahl der gelaufenen Tests muss der Zahl der registrierten entsprechen (`ctest -N`).
   - Sie muss mindestens die Untergrenze aus `re15_port/tools/local_build.sh` erreichen
     (`RE15_MIN_TESTS:-403`, **gelesen, nicht dupliziert**).
   - `linux_out/re15_pc`, `ldd.txt`, `glibc_max.txt` und `diag/` werden **zu Beginn
     gelöscht**. Ein Fehlschlag lässt damit kein altes Binary mehr liegen, das der Paketbau
     einpacken könnte (Memory `reai-v2-releasebau-pipe-schluckt-fehler`).
6. **ctest bleibt seriell** (`RE15_CTEST_JOBS`, Standard 1). Port-Wahl, keine
   Original-Adresse. Gemessen: `-j8` braucht für ctest 101 s statt 392 s, gesamt 174 s,
   und liefert **denselben** Fingerabdruck. Das war aber nur **ein** Lauf, und GUI-Haken
   flattern unter Last. Deshalb bleibt es Opt-in. ninja nutzt wie bisher `-j$(nproc)` = 16.
7. `--mount` stellt den alten Weg wieder her (Vergleichsläufe). Kurz angestartet: Quelle
   und Bau liegen dann auf `v9fs`, wie erwartet.

**Windows-Cross-Bau auf demselben Weg:** `release/build_win_cross.sh`,
`Dockerfile.wincross` und `win_deps.sh`. `docker_win_build.sh` nutzt `rueckfall_links.sh`
und `win_deps.sh`, schreibt Phasenzeiten und entfernt zu Beginn das alte exe. Die KOPIE
enthält nur `re15_port`, weil der Cross-Bau ohne Tests läuft.

## 5. Spurlauf: Was lesen Bau und Tests über die Rückfall-Links?

`linux-bau/trace_inner.sh`: der volle Bau unter
`strace -f -y --seccomp-bpf -e trace=open,openat,openat2`. Gefiltert auf Deskriptoren,
die auf `/host/…` zeigen (`-y` löst den Link auf).

- Configure und Compile öffneten über `/host` nur **einmal `.git`**. Das war SDL2s
  `find_package(Git)` / `git describe` (SDL2 `CMakeLists.txt:3031-3046`), das von
  `_deps/sdl2-src` aufwärts bis `/src/.git` sucht. Im Arbeitsbaum ist `.git` eine Datei mit
  Windows-Pfad; alter und neuer Weg sehen dieselbe Datei, und `git describe` scheitert in
  beiden gleich. Deshalb ist es der Rückfall-Link, keine Kopie.
- Die Tests öffneten 149-mal etwas über `/host`, zusammen 128 Dateien mit 2,3 MB:
  - `pri/STAGE1/**`, 139 Opens (die PRI-Masken-PNGs),
  - `analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr` (5),
  - `analysis/befunde_2026-09-21/10f0-quader-silhouette/messung/pfad_10f0_aus_befundlog.txt` (2),
  - `analysis/befunde_runde30/sicherung_werkzeug/soll/sicherung_zord_normal.md1` (1),
  - `analysis/kartensymbole/symbolkatalog.csv` (1),
  - `.git` (1).

  Die Rückfall-Links wurden also **tatsächlich gebraucht**. Ohne sie wären das stille
  Lücken gewesen. Genau das ist die Falle, vor der die Memory warnte.
- Diese Pfade stehen jetzt in KOPIE (Stand der Läufe `neu1`/`neu2`). Übrig bleibt nur
  der eine `.git`-Zugriff.

## 6. Abnahme: zwei volle Läufe des neuen Wegs

`bash release/build_linux_deck.sh > log 2>&1; rc=$?`, die Rückgabe also ohne Pipe.

| Phase | `neu1` | `neu2` | `alt1` |
|---|---|---|---|
| Image (Cache) | 2 s | 3 s | — |
| Quellbaum packen (Host) | 17 s | 17 s | — |
| Quellbaum auspacken (Container) | 14 s | 15 s | — |
| Rückfall-Links (345) | 1 s | 1 s | — |
| Pakete | 0 s | 1 s | 81 s |
| Configure + Generate | 12 s | 12 s | 1348 s (cmake: 1061,6 + 267,6) |
| Compile + Link | 26 s | 25 s | 926 s |
| – Übersetzen / Linken (Summe der Kanten) | 360,7 / 45,9 s | 349,3 / 42,7 s | 2988 / 10717 s |
| ctest (seriell) | 392 s | 393 s | 2551 s |
| **gesamt** | **469 s** | **471 s** | **4906 s** |
| Tests | 403, **401 grün, 2 rot** | 403, **401 grün, 2 rot** | 403, 399 grün, 4 rot |

Der neue Weg ist **10,4-mal schneller**. ctest macht jetzt 83 % der Zeit aus. Die längsten
Tests sind die Integrationsläufe mit echtem re15_pc: `integration_elza_vollstart` 100 s,
`integration_r30_cut_blitz` 68 s, `integration_r30_irons_tisch_bild` 35 s.

**Tests gegen den alten Weg und gegen Windows** (`linux-bau/vergleich.sh`,
`ctest_fingerprint.sh`). Die Windows-Referenz ist `local_build.sh` im Integrationsbaum
bei `80d579a5`: 403/403 grün.
- **Dieselben 403 Testnamen** in allen vier Läufen (alt1, neu1, neu2, Windows).
- **Die Zahl der SKIP-, fehlt- und übersprungen-Zeilen je Test ist gleich**, auch gegen
  Windows. Die 21 Zeilen stimmen wörtlich überein. Die einzige Ausnahme ist die Zufallszahl
  `skipped=N` von `prop_fixpoint_precision`: 811 / 748 / 717 / Windows 744, am 27.09. 781.
  Das ist ein Zufalls-Eigenschaftstest und kein Datei-Überspringen. **Keine** einzige
  `SKIP: … fehlt`-Zeile in irgendeinem Lauf.
- Statusunterschied alt gegen neu: **nur** die zwei Mount-Timeouts
  (`unit_sld_atlas`, `unit_r21_discard_wegwerfen`), die neu grün sind.
- Unterschied neu gegen Windows: die zwei Linux-Fehlschläge aus §8. Die fallen auf dem
  alten Weg genauso.

**Binary** (`linux-bau/vergleich.sh` im Bau-Image). Das Binary des alten Wegs stammt aus
`release/lbuild/platform/pc/re15_pc`, weil das alte Skript es bei rotem ctest nicht kopiert.

| | alt1 | neu1 | neu2 |
|---|---|---|---|
| Größe | 3 732 200 B | 3 732 200 B | 3 732 200 B |
| abweichende Bytes gegen alt1 | — | 24 | 24 |
| davon `.note.gnu.build-id` | — | 20 | 20 |
| davon `.rodata` | — | 4 (`__TIME__` in `main.c:5941`: „20:49:55" gegen „21:58:35") | 4 |
| `ldd` | 6 Zeilen | gleich | gleich |
| GLIBC-Versionen (`objdump -T`) | 2.2.5 2.3 2.3.2 2.4 2.7 2.9 2.14 2.17 2.27 **2.29** | gleich | gleich |

Das Binary ist also byte-gleich bis auf den Übersetzungszeitpunkt (`__DATE__`/`__TIME__`)
und die daraus folgende Build-ID. Die glibc-Untergrenze bleibt **GLIBC_2.29** (debian:11).

**Rauchstart im Container** (`linux-bau/rauchstart.sh`, Repo unter `/src`):

| | alt1 | neu1 | neu2 |
|---|---|---|---|
| `RE15_ASSET_SELFTEST=1` | rc=0, `[selftest] RESULT ok=26 missing=0` | gleich | gleich |
| Spielstart wie `integration_boot_bg_pin` (`RE15_NO_INTRO`, `RE15_TITLE_SHOT`, `RE15_INV_SHOT`=exit bei Spielbild 34) | rc=0, `exit34.bmp` 775d33ea27ed2c71 | rc=0, **gleiches** exit34.bmp | rc=0, gleiches exit34.bmp |

`title.bmp` unterscheidet sich schon zwischen zwei Starts **desselben** alten Binarys
(a3bcd06e…, 4bbf199a…, acb8fdc0…). Es hängt an der Uhr, daher kein Kriterium.
`--headless` allein und `RE15_EXIT_AT=300` allein kehren nicht zurück, weil der Titel auf
Eingabe wartet. Beide Binarys enden gleich mit rc=124 nach 120 s.

**Windows-Cross-Bau:**

| | alter Weg, **warm** (Cache aus dem Haupt-Checkout kopiert) | neuer Weg, **kalt** |
|---|---|---|
| Image | — (apt im Lauf: ~45 s) | 57 s beim ersten Mal, danach Cache |
| Quellbaum packen / auspacken | — | 8 s / 9–11 s (4613 Dateien, 333 MB) |
| Configure + Generate | 22,7 + 24,1 s | 22 s / 25 s |
| Compile + Link | ~101 s (103 Kanten, dazu 42 s ninja-Start) | 19 s / 22 s (373 Kanten inkl. SDL2) |
| gesamt | **261 s** | **118 s** mit Image-Erstbau; **75 s** im zweiten Lauf mit Image-Cache |
| exe | 4 277 722 B | 4 277 722 B |

Gegen den alten Weg unterscheidet sich das exe in 7 Bytes (1. Lauf) bzw. 8 Bytes (2. Lauf):
PE-TimeDateStamp @0x88–0x89, PE-CheckSum @0xd8–0xd9 und die `__TIME__`-Zeichenkette
@0x242f78–0x242f7c. Kalt auf dem alten Weg
habe ich es nicht noch einmal gemessen. Die Memory nennt 45+ min, und die SDL2-Prüfungen
kosteten auf dem Linux-Weg gemessen 10–19 s je Stück.

## 7. Warum das Ergebnis gleich bleibt

- Dasselbe Basis-Image `debian:11` mit derselben Paketliste aus derselben Datei und
  demselben Snapshot: gcc 10.2.1-6, cmake 3.28.6, ninja 1.10.1.
- Derselbe cmake-Aufruf (`Release`, `RE15_BUILD_TESTS=ON`) und derselbe Pfad `/src`.
  Geändert hat sich nur das Dateisystem darunter.
- Dieselben Bytes: Das tar packt den **Arbeitsbaum** so, wie er auf der Platte liegt,
  mit den autocrlf-Zeilenenden. Der alte Mount zeigte genau diese Dateien.
- Jede Datei des Repos ist im Container vorhanden, als Kopie oder als Link. Belegt ist das
  durch den gleichen SKIP-Fingerabdruck gegen den alten Weg und gegen Windows.

## 8. Offene Punkte (nicht Teil dieser Spur)

1. ⛔ **Die Linux-Suite ist bei `80d579a5` rot, unabhängig vom Mount.** Beide Tests
   scheitern auf dem alten und dem neuen Weg, in allen drei Läufen. Solange sie rot sind,
   liefert `build_linux_deck.sh` kein `linux_out/re15_pc`. **Ein Linux-Release ist damit
   gesperrt.**
   - `integration_r30_irons_tisch_licht` meldet `FEHLER: Bildgroessen 640x480 / 640x480,
     erwartet 960x720`. Die Framedumps im Container haben also eine andere Größe als unter
     Windows.
   - `integration_r30_titel_puls` scheitert an den Kriterien (B), (C) und (D):
     `Pulswerte mit mehr als einem Zeilenbild: 3215`, `1364 Bilder weichen vom
     Original-Bildpuffer ab`, `Periode 1: 1937845 us, 58 Engine-Schritte … AUSSERHALB`.

   Beide Tests stammen aus Runde 30. Die Ursache liegt in der Linux-Laufzeit oder im Test,
   nicht im Bauweg. Das ist nicht untersucht, weil es Sache der jeweiligen Spur ist.
2. Die Memory `reai-v2-releasebau-pipe-schluckt-fehler` rät: „Tests NICHT auf eine
   Container-Kopie verlegen". Das ist durch die Rückfall-Links und den Fingerabdruck-Vergleich
   erledigt. Die Memory `reai-v2-release-baum-warmer-cache` („qemu-VM", „Caches
   hineinkopieren") ist für den Kopie-Weg überholt. Beides pflegt der Auftraggeber.
3. Der podman-Weg auf einem echten Linux-Host und der distrobox-Weg sind nicht erneut
   gelaufen. distrobox ist unverändert bis auf die per `source` geladenen Blöcke; ohne
   Linkliste ist `rueckfall_links.sh` wirkungslos. Unter Linux ist ein Bindmount nativ und
   schnell, dort kostet die Kopie nur das Packen und Auspacken.
4. Nach jedem Kopie-Lauf bleibt kein Bauverzeichnis auf dem Host. Ein inkrementeller
   Neubau fällt damit weg, jeder Lauf baut kalt. Das kostet gemessen 12 s Configure und
   25 s Compile. Ein kalter Bau ist für ein Release die sauberere Wahl, und es gibt keine
   Gefahr durch fremde Caches wie früher bei `wxbuild`.

## 9. Werkzeuge (`analysis/befunde_runde30/linux-bau/`)

| Datei | Zweck |
|---|---|
| `io_probe.sh` | 9p gegen Container-Dateisystem (§2) |
| `ts_run.sh` | Zeitstempel je Ausgabezeile für den alten Weg (§3) |
| `ninja_phasen.sh` | `.ninja_log` → Übersetzen gegen Linken (Summe/Spanne) |
| `trace_inner.sh` | Spurlauf mit `strace -y` auf Zugriffe über `/host` (§5) |
| `vergleich.sh` | Binary (cmp je ELF-Abschnitt, Build-ID, ldd, GLIBC) + ctest-Fingerabdruck (§6) |
| `rauchstart.sh` | Selbsttest + Spielstart bis Bild 34 im Container (§6) |
