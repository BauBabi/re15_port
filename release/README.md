# Release bauen (`release/`)

Ein Aufruf baut den Port fuer **alle Systeme** (Windows, Linux/Steam Deck, Android), gibt ihm die **naechste
Versionsnummer**, packt ihn und bringt die **alte Version ins Archiv** (Runde 38, Spur V; Dossier
`analysis/befunde_runde38/V_releaseskript.md`).

## Kurz

| Was | Wie |
|---|---|
| Doppelklick | `release\release_alles.cmd` - zeigt erst den Trockenlauf, dann die Wahl: [1] bauen, packen, archivieren; [2] dazu am Ende Commit + Tag + Push; [3] nur Commit + Tag + Push (schon gebaut); [4] nichts |
| Nur ansehen, nichts bauen | `bash release/release_alles.sh --dry-run` |
| Alles bauen, packen, archivieren | `bash release/release_alles.sh` |
| ... und am Ende Commit + Tag + Push | `bash release/release_alles.sh --tag` (die Release-Notes `# vX.Y.Z ...` muessen oben in `release/RELEASE_NOTES.md` stehen) |
| Nach einem Abbruch weitermachen | `bash release/release_alles.sh --version vX.Y.Z --ab <schritt>` (die Zeile steht in der Abbruchmeldung) |
| Commit + Tag + Push spaeter | `bash release/release_alles.sh --ab abschluss --tag` (stehen Commit und Tag schon, z.B. weil der Push scheiterte, wird nur noch gepusht) |
| Nur einen Teil, z.B. nur Windows | `bash release/release_alles.sh --ab windows --bis windows` |

Dauer (gemessen): Windows 78-107 s, Android ~3 min, Linux/Deck ~3 h (volle Testsuite im Container, seriell:
ctest 10827 s bei v0.8.25; `--linux-jobs 3` brauchte bei v0.8.24 2595 s statt 7488 s), Paket ~3 min, Archiv ~1 min.
Docker Desktop startet das Skript selbst (`docker desktop start`, gemessen 19-24 s).

## Versionsnummer

Standard: der hoechste Tag `vX.Y.Z` plus eins in der letzten Stelle (`v0.8.25 -> v0.8.26`). Bei 99 gibt es einen
Uebertrag (`v0.8.99 -> v0.9.0`, wie damals `v0.7.99 -> v0.8.0`) - die 99 kommt aus dem Android-versionCode
(`re15_port/platform/android/app/build.gradle:43-49`: X*1000000 + Y*10000 + Z*100). Eine eigene Nummer mit
`--version vX.Y.Z`; sie muss groesser sein als jeder vorhandene Tag (sonst nimmt Android das Update nicht an).
Nach `v0.99.99` waehlt das Skript nichts selbst (das waere kein "minor" mehr) - dann `--version`.

## Was das Skript tut (jeder Schritt mit eigenem Protokoll unter `release/logs/<version>_<zeit>/`)

1. **Vorbedingungen**: Tag frei, `re15_port/` und das Release-Werkzeug ohne nicht committete Aenderung, Docker
   (startet Docker Desktop), Platz (>= 10 GiB), zip/unzip/Python, Archiv, keine zweite Instanz. Nicht committete
   Aufnahmen unter `synchro/STAGE*` werden gemeldet (sie kommen ins Paket, aber nicht in den Commit). Liegt die neue
   Version schon im Archiv, wird nicht neu gebaut (dann `--ab abschluss --tag`, oder den Archivordner vorher umbenennen).
2. **alt**: die bisherige Version aus `release/` kommt ins Archiv (`<archiv>/<alte version>/`, `sha256sum -c`) -
   oder wird dort geprueft, wenn sie schon da ist. Das passiert VOR dem Bauen.
3. **windows** `build_win_cross.sh`, **android** `build_android.sh --version V`, **linux** `build_linux_deck.sh`:
   je Rueckgabe 0 + Erfolgsmarke im Protokoll + frisches Artefakt (nicht aelter als der Schrittbeginn).
4. **paket** `make_package.sh --version V`: Volumes aller drei Systeme + `SHA256SUMS.txt`; die alten Volumes
   verschwinden aus `release/` und aus dem git-Index (noch kein Commit).
5. **archiv**: neue Version nach `<archiv>/<version>/` (`sha256sum -c`); die alte APK verlaesst `release/`, sobald
   sie bytegleich im Archiv liegt. **Im Archiv wird nichts geloescht.**
6. **abschluss** (nur mit `--tag`): Release-Commit `release: vX.Y.Z (...)` (nur Paketdateien + Notes), annotierter Tag,
   `git push origin master vX.Y.Z` (`--kein-push`: ohne Push). Verweigert, wenn nach dem Paketbau Paket-Inhalt
   committet wurde. Was ein frueherer Lauf schon angelegt hat (Commit, Tag), wird geprueft und nicht neu angelegt; der
   Push gilt erst, wenn origin selbst `master` und den Tag traegt (`git ls-remote origin`).

Archiv-Standard: `C:/workspace/Re15Data/re15_packages_archiv` (anders: `--archiv ORDNER` oder Umgebung `RE15_ARCHIV`).
Hilfe zu allen Optionen: `bash release/release_alles.sh --help`.

## Bei einem Abbruch

Die Meldung nennt Schritt, Grund, Protokoll und die Zeile zum Weitermachen (mit denselben Optionen wie der Aufruf,
z.B. `--archiv`). Ist der Linux-Schritt nur an Zeitgrenzen unter Last rot (Memory
reai-v2-releasebau-pipe-schluckt-fehler): Maschine frei machen, dann `--ab linux --linux-jobs 3`. Diagnose des Linux-Baus: `release/linux_out/diag/`.

Scheitert der **Push** (Netz, Anmeldung): Release-Commit und Tag stehen dann schon lokal. Die Meldung sagt das;
nach der Behebung pusht ihre Zeile (oder Doppelklick-Wahl [3], `--ab abschluss --tag`) nur noch - kein zweiter
Commit, keine neue Version. Von Hand geht es auch: `git push origin master vX.Y.Z`. Der Trockenlauf (auch der des
Doppelklicks) meldet einen solchen nicht gepushten Release mit `[!!] ... der Push fehlt`.

## Einzelskripte (werden von release_alles.sh aufgerufen, gehen weiter einzeln)

`build_win_cross.sh`, `build_linux_deck.sh`, `build_android.sh`, `make_package.sh` (baut nicht, packt nur),
`docker_bereit.sh` (Docker pruefen/starten). Fallen, die das Gesamtskript abfaengt: ein gepipeter Bau meldet exit 0
trotz Fehler; ein altes Artefakt geht als neues durch; `make_package.sh` liefert ohne APK still nur die PC-Pakete;
ohne `--version` heissen die Pakete `v0.1.2` (make_package.sh) bzw. `v0.8.25-7-g...` (build_android.sh).
