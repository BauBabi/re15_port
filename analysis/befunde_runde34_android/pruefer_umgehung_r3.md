# Gegenpruefung Runde 3 — Linse UMGEHUNG/ROBUSTHEIT (Android-Asset-Gate)

Pruefer: Gegenpruefer R3 (Umgehung/Robustheit). Stand: 2026-09-30, abgeschlossen. Ich aendere KEINE Werkzeuge.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate).
Pruefgegenstand: Werkzeugstand eff38fc1 (release/apk_asset_gate.py v6 sha256 72cfa6b2..., apk_pruefen.sh,
build_android.sh, make_package.sh, python_finden.sh, zip_exec_bit.py, apk_signer.sha256, app/build.gradle).
Belege: analysis/befunde_runde34_android/pruefer_umgehung_r3_belege/ (Werkzeuge r3_*.sh/.py, Logs).
Arbeitsordner (nicht versioniert): build/r34a/pruefer_r3/ - nach dem Lauf bis auf logs/ und mutanten/ geleert.
Parallel im Baum: Pruefer "echtlauf r3" (voller Android-Bau + make_package.sh in release/). release/ und
platform/android/ habe ich NICHT angefasst; make_package-Laeufe und Mutanten-Ketten nur in eigenen Sandboxen
(`r3_sandbox_anlegen.sh`: unveraenderte Skriptkopien - per cmp belegt -, Quellbaum per Hardlink, eigenes git-Repo).

## Urteil: NICHT HALTBAR

Das ECHTE Gate haelt jede neue Faelschung (12 an der echten APK, 10 Quellbaum-Varianten, 6 Regressionsfaelle,
12 --gate-only-Randfaelle; signierte Faelschungen scheitern an der echten Kette). Nicht haltbar ist die
Umgebung des Gates: (B1) make_package.sh liefert einen alten, von ihm selbst im Lauf davor abgelehnten
Android-Satz derselben Version aus, sobald die APK fehlt - echter Lauf EXIT 0; (B2) der Selbsttest faengt acht
natuerliche Ein-Zeilen-Abschwaechungen des Manifestlesers nicht, zwei davon lassen eine mit dem Release-Schluessel
signierte, auf dem Geraet fehlerhafte APK durch die GANZE Kette. Dazu zwei niedrige (B3, B4).

## 1. Befunde (Kurzfassung; Einzelheiten Abschnitt 2)

| Nr | Schwere | Befund |
|---|---|---|
| B1 | mittel | make_package.sh liefert einen ALTEN Android-Split-Satz derselben Version ungeprueft aus, wenn die APK fehlt: echter Lauf EXIT 0 mit "(kein Android-Paket ...)", aber der Satz steht in SHA256SUMS.txt und wird git-vorgemerkt - dieselbe APK hatte make_package.sh im Lauf davor als "weicht vom Quellbaum ab" abgelehnt |
| B2 | mittel | Selbsttest (202/202) faengt 8 natuerliche Teil-Abschwaechungen (Zeichenklassen, strip/split, Konstante, Methodenmenge - ausserhalb der Mutantenklassen A-I des Bauers) NICHT; MU1 und MU2 lassen je eine mit dem Release-Schluessel signierte Faelschung durch die GANZE Kette (`ANDROID-GATES-OK`, EXIT 0), die das Geraet nicht richtig entpackt |
| B3 | niedrig | verify_apk_im_zip haelt die gezippte APK nur ueber CRC32 + Groesse fest: eine Datei gleicher Groesse/CRC32, anderer sha256 (apksigner DOES NOT VERIFY) meldet "APK im Split-Satz = gepruefte APK" (Funktions-Sonde) |
| B4 | niedrig | make_package.sh benutzt das Gate im PC-Pfad (--quellbaum, --paket) OHNE dessen --selbsttest: ein Gate ohne Paket-sha-Vergleich (sein eigener Selbsttest: FEHLER 3/202) liess ein Paket mit veraenderter Datei durch, echter Lauf EXIT 0 |

## 2. Einzelheiten

### 2.1 B1 (mittel) Alter Android-Split-Satz wird ungeprueft ausgeliefert
Code (make_package.sh eff38fc1): die APK-Pruefung (:481-499) und das Zippen (:688-708) laufen NUR, wenn
`release/<NAME>_android.apk` da ist; `rm -f "${NAME}_android".z*` steht im selben Zweig (:699). Fehlt die APK,
meldet :707 "(kein Android-Paket: ... fehlt)", aber :709 `sha256sum "${NAME}"_*.z* > SHA256SUMS.txt` und
:763-766 `git add release/${NAME}_*.z*` nehmen JEDEN vorhandenen `<NAME>_android.z*` mit - auch einen aus einem
frueheren Lauf derselben Version (die v0.8.19-Volumes sind im Repo versioniert, `git ls-files`, Commit 64170c05).
build_android.sh loescht die APK schon VOR Gradle (:289) - nach einem gescheiterten Android-Bau fehlt sie also
genau dann, wenn der alte Satz nicht mehr zum Quellbaum passt.
Messung `r3_mp_veraltet.sh` (`mp_veraltet.txt`, `mp_lauf1_mit_apk.log`, `mp_lauf2_apk_veraltet.log`,
`mp_lauf3_ohne_apk.log`, `mp_gate_aus_satz.log`): make_package.sh ECHT, unveraenderte Kopie (cmp: make_package.sh,
apk_pruefen.sh, apk_asset_gate.py, python_finden.sh gleich) in der Sandbox sb_mp (Quellbaum = Hardlinks, eigenes
git-Repo mit Commit 2026-09-29 12:00, Linux-Binary v0.8.19 aus dem Archiv mit Original-mtime 19:34 - kein touch),
jeweils `--version v0.8.19 --only linux`:
| Schritt | Ergebnis |
|---|---|
| Lauf 1, APK = Referenz v0.8.19 | **EXIT 0**: QUELLBAUM-OK, APK-PRUEFUNG-OK (514bebd5...), PAKET-OK, `APK im Split-Satz = gepruefte APK (CRC32 d16ad30a)`, 4 Volumes vorgemerkt |
| Quellbaum aendert sich | extracted_fx/effect0_blood.tim in der Sandbox 1 Byte anders (Link vorher durch Kopie ersetzt; Original a7ac86c7... unveraendert) |
| Lauf 2, APK noch da | **EXIT 1** `ABBRUCH: APK-Asset-Gate: die APK weicht vom Quellbaum ab` (`Inhalt weicht ab ... effect0_blood.tim`) - die Pruefung greift |
| APK entfernt | wie nach einer Ablehnung bzw. nach build_android.sh mit gescheitertem Gradle-Lauf |
| Lauf 3, ohne APK | **EXIT 0** `== Fertig ==`; Log: `(kein Android-Paket: re15_port_v0.8.19_android.apk fehlt ...)` und direkt danach listet das Skript `re15_port_v0.8.19_android.z01/.zip` (03:48 = Lauf 1); SHA256SUMS.txt nennt beide Android-Volumes; `4 neue vorgemerkt` (2 Linux + 2 Android) |
| Android-Satz nach Lauf 3 | `sha256sum -c` gegen Lauf 1: beide OK - unveraendert der alte Satz |
| APK aus dem ausgelieferten Satz | sha256 514bebd5... = die in Lauf 2 abgelehnte APK; Gate gegen den aktuellen Quellbaum: **rc 1** `Inhalt weicht ab ... effect0_blood.tim` |
Folge: der Zweck der Pruefung in make_package.sh (:466-470 "Zwischen Android-Bau und Paket kann sich der Quellbaum
geaendert haben ... die APK waere dann veraltet und wuerde trotzdem gezippt") ist umgangen - ohne jedes
Faelschungswerkzeug, nur durch "Pruefung schlaegt an -> APK weg -> neu paketieren". Der Kopf (:24-27) verspricht
"fehlt sie, gibt es keinen Android-Satz". Dieselbe Stelle behaelt mit `--only win|linux` die Volumes der jeweils
anderen PC-Plattform derselben Version in SHA256SUMS/git add (aus dem Code, nicht gemessen; dort ohne Asset-Gate).
Abhilfe-Richtung (nicht gebaut): fehlt die APK, alte `${NAME}_android.z*` entfernen oder abbrechen; SHA256SUMS
nur aus den in DIESEM Lauf erzeugten Volumes.

### 2.2 B2 (mittel) Selbsttest faengt Teil-Abschwaechungen neuer Klassen nicht
Die Mutanten-Probe des Bauers (449 Mutanten, Klassen A befund->pass, B raise->pass, C Namensregel, D Hand,
E Vergleichsrichtung/Grenze, F and/or-Operand, G Tupelpaar, H not, I Zahl +-1) mutiert keine Zeichenklassen,
strip-/split-Varianten, Konstanten oder Mengen. `r3_mutanten.py` erzeugt je EINE Textstelle
(`selbsttests_uebersicht.txt`, `selbsttest_MU*.log`; Selbsttest je 11-17 s):
| Mutant | Aenderung (eine Zeile) | --selbsttest |
|---|---|---|
| MU0 (Auftragsbeispiel) | `if a_sha != q_sha:` -> `if False:` (sha-Vergleich aus) | **FEHLER 5/202** (Faelle 05, 30, 31, 167, 168) - gefangen |
| MU1 | Manifestzeile `z.rstrip("\r")` -> `z.rstrip()` | OK 202/202 |
| MU2 | Groessenfeld `r"[0-9]+"` -> `r"\d+"` (derselbe Fehler, den R2-B8 in KOPF_RE fand) | OK 202/202 |
| MU3 | `text.split("\n")` -> `text.splitlines()` | OK 202/202 |
| MU4 | `MANIFEST_MAX = 64 << 20` -> `64 << 30` | OK 202/202 (nur ueber den Pruefhaken getestet; praktisch unerreichbar) |
| MU5 | `if e.methode not in (0, 8)` -> `if e.methode > 8` | OK 202/202 |
| MU6 | Groessenfeld `re.fullmatch(r"[0-9]+", g)` -> `g.isdigit()` | OK 202/202 |
| MU7 | Kopfzeile `rstrip("\r")` -> `strip()` | OK 202/202 |
| MU8 | Manifestpfad zusaetzlich `pfad.strip()` | OK 202/202 |
Jeder der Manifest-Mutanten nimmt eine Faelschung der ECHTEN APK an, die das echte Gate ablehnt
(`batterie_ergebnis.txt`; Faelschungen mit dem eigenen Rohbyte-Werkzeug `r3_faelschen.py`, unsigniert):
| Faelschung (nur Manifest, Zeile RE15DOOR/P07G.DO2) | echtes Gate | Mutant | Geraet (android_glue.c:196-229) |
|---|---|---|---|
| F1 Leerzeichen am Pfadende | 1 (Zeile ohne APK-Eintrag + fehlt im Manifest) | MU1 **0**, MU8 **0** | rel = "...P07G.DO2 " -> SDL_RWFromFile findet kein Asset -> Tuerarchiv nie entpackt, Fehlerbild bei jedem Start |
| F4 `\x0c` am Zeilenende | 1 | MU1 **0**, MU3 **0** | wie F1 |
| F9 `\x0b` am Zeilenende | 1 | MU1 **0** | wie F1 |
| F10 U+00A0 am Pfadende | 1 | MU8 **0** | wie F1 |
| F6 U+2028 statt `\n` vor der Zeile | 1 | MU3 **0** | Zeile verschmilzt mit der vorigen -> P07D.DO2 und P07G.DO2 nie entpackt |
| F2 Groesse `５５９０８` (Vollbreite) | 1 (keine Zahl) | MU2 **0**, MU6 **0** | atoll -> 0: Datei wird bei JEDEM Start neu kopiert, got != sz -> Fehlerbild, Marker nie geschrieben |
| F2b Groesse arabisch-indisch | 1 | MU2 **0** | wie F2 |
**Ganze Kette** (`r3_kette_mutanten.sh`, `kette_mutanten.txt`, `kette_*.log`): F1 und F2 mit zipalign -P 16 4 und
DEMSELBEN Schluessel wie der Release-Bau signiert (`r3_signieren.sh`: apksigner v2 true, Signer 432bc749... =
apk_signer.sha256, zipalign -c ok, `signieren.txt`), dann `build_android.sh --gate-only <apk> --version v0.8.19`:
| Lauf | EXIT |
|---|---|
| echte Skripte, K0 signiert (Kontrolle) | 0 |
| echte Skripte, F1 signiert / F2 signiert | 1 / 1 (`APK-ASSET-GATE-ABWEICHUNG`) |
| Sandbox, nur apk_asset_gate.py := MU1, F1 signiert | **0** - Stichproben, aapt, zipalign, Signer-Pin, Selbsttest 202/202, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK` |
| Sandbox, nur apk_asset_gate.py := MU2, F2 signiert | **0** - dito |
| Sandbox mit echtem Gate, F1 signiert (Kontrolle der Sandbox) | 1 |
Ursache: keine Selbsttest-Faelle fuer Whitespace/Steuerzeichen (ausser `\r`) am Zeilen-/Pfadende, andere
Zeilentrenner als `\n`, Nicht-ASCII-Ziffern im Groessenfeld (nur in der Kopfzeile), Methoden 1-7; die
64-MiB-Konstante ist nur ueber den Pruefhaken erreichbar (min(Konstante, Haken)). Das echte Gate ist an diesen
Stellen RICHTIG - der Selbsttest bemerkt eine Regression dort aber nicht, und genau dafuer laeuft er vor jedem Gate.

### 2.3 B3 (niedrig) Identitaet der gezippten APK nur ueber CRC32 + Groesse
make_package.sh:693-697 vergleicht vor dem Zippen die sha256-Kennung (gut); nach dem Zippen prueft
verify_apk_im_zip (:557-582) nur CRC32 und Groesse aus dem Katalog. `r3_zip_kennung_sonde.sh` (`zip_kennung_sonde.txt`,
`gate_crc_faelschung.log`): Referenz-APK, 1 Byte in den Daten von RE15DOOR/P07G.DO2 gekippt + 4 Ausgleichsbytes
(`r3_crc_gleich.py`, eigene GF(2)-Rechnung) -> sha256 2dd9497b... statt 514bebd5..., CRC32 d16ad30a und 363212403 B
gleich; gezippt wie :700 (`zip -q -s 90m -j`); verify_split + verify_apk_im_zip per awk UNVERAENDERT aus
make_package.sh: `APK im Split-Satz = gepruefte APK (CRC32 d16ad30a, 363212403 B)`, rc 0. Dieselbe Datei: Gate rc 1
(`Inhalt weicht ab ... P07G.DO2`), apksigner `DOES NOT VERIFY ... CHUNKED_SHA256 digest mismatch`.
Einordnung: Funktions-Sonde, kein echter make_package-Lauf; wirksam nur, wenn die Datei im kurzen Abschnitt
zwischen :693 und :700 ersetzt wird. Gegen versehentliche Aenderungen reicht CRC32; die Meldung "= gepruefte APK"
verspricht aber mehr als geprueft wird - die sha256 der Pruefkopie liegt vor und liesse sich nach dem Zippen aus
dem Satz nachrechnen (`unzip -p` des einen Eintrags).

### 2.4 B4 (niedrig) PC-Pfad nutzt das Gate ohne Selbsttest
make_package.sh ruft `apk_asset_gate.py --quellbaum` (:459) und `--paket` (:268) auf, aber nirgends `--selbsttest`
(`grep -n selbsttest release/make_package.sh`: leer); der Selbsttest laeuft nur in apk_pruefen (:230), also nur mit
APK und Zippen. `r3_mp_ohne_selbsttest.sh` (`mp_ohne_selbsttest.txt`, `mp2_*.log`, Sandbox sb_mp, `--zip-only --only
linux`, 1 Byte in der Paket-Kopie von shared_assets/PSX/DATA/TEX.TIM, Linkzahl 1 geprueft):
| Lauf | Ergebnis |
|---|---|
| A, echtes Gate | **EXIT 1** `APK-ASSET-GATE-PAKET-ABWEICHUNG` (`Inhalt weicht ab: shared_assets/PSX/DATA/TEX.TIM`) |
| Mutant MUP (in paket_pruefen `if q_sha != p_sha:` -> `if False:`), sein --selbsttest | **FEHLER 3/202** (Faelle 132, 185, 186) - der Selbsttest wuerde ihn fangen |
| B, Sandbox-Gate := MUP | **EXIT 0** `APK-ASSET-GATE-PAKET-OK`, `== Fertig ==` - das Paket mit der veraenderten Datei wurde gezippt |
Die alten Gates in check_tree (Pflichtdateien, cmp fuer TORSE.VBS/RE2-DOOR/RE15DOOR, WAV-Zahl) bleiben davon
unberuehrt (2.5). Abhilfe-Richtung: `--selbsttest` einmal vor der ersten Gate-Nutzung in make_package.sh.

### 2.5 Hinweise (keine Urteilsgrundlage)
- N1 Eintrag mit leerem Namen direkt vor dem Zentralverzeichnis (F8, unsigniert): Gate rc 0, aapt2 35.0.0
  (libziparchive) `Zip: bad local hdr offset ... failed opening zip: Invalid offset` (Local Header endet genau am
  Zentralverzeichnis; libziparchive prueft `lho + 30 >= cd_offset`). Der Selbsttestfall "letzter Eintrag ohne Name
  ... gut" hat Polster zwischen Kopf und Zentralverzeichnis und erreicht diese Grenze nicht. In der Kette kein
  Durchlass: unsigniert -> apksigner; direkt signiert liest aapt2 (Signing Block dazwischen), aber `zipalign -c`
  stuerzt ab (rc 139, auch `zipalign -f`) -> Abbruch in Schritt 3. aapt v1 (badging) oeffnet F8.
- N2 Kollisionen, die erst auf dem Geraet entstehen (gross/klein, NFC/NFD - der App-Speicher unter
  Android/data ist case-insensitiv), prueft das Gate nicht; im jetzigen Baum 0 Nicht-ASCII-Namen, 0 gross/klein-
  Doppel (gemessen) - derzeit theoretisch.
- N3 NUL am Manifestende: Gate rc 1, das Geraet hoert am NUL auf (harmlos) - strenger als noetig, kein Durchlass.

## 3. Was HAELT (gemessen)
- **Echtes Gate, 12 neue Faelschungen der echten APK** (`batterie_ergebnis.txt`): K0 0; F1 Leerzeichen 1,
  F2/F2b Unicode-Ziffern 1, F3 NUL am Manifestende 1, F4 FF 1, F6 U+2028 1, F7 Zusatzeintrag `p07g.do2`
  (gross/klein) + Manifestzeile 1, F9 VT 1, F10 NBSP 1; F5 fuehrende Null `055908` 0 (richtig: atoll und int lesen
  55908), F8 leerer Name 0 (N1).
- **Echte Kette** gegen signierte F1/F2: EXIT 1.
- **--gate-only-Randfaelle** (`rest_ergebnis.txt` A): Pfad fehlt/Ordner/Textdatei/0 Byte/abgeschnitten -> 1,
  leerer Pfad -> 2, Leerzeichen im Pfad -> 0 (K0 signiert), Version mit Leerzeichen / Praefix `v0.8.1` -> 1,
  Version vor dem Pfad -> 0, ohne Version -> 2, `--version` ohne Wert -> 1 (set -u).
- **Quellbaum-Varianten** (Sandbox sb_kette gegen die Referenz-APK, `rest_ergebnis.txt` B): RE15DOOR leer 1, fehlt 1,
  synchro/STAGE2 -> unused2 1, synchro/README.md 0 (richtig, ausserhalb STAGE*), synchro/STAGEX.txt oben 1,
  build.gradle mit Zusatzbaum 2, neuer Ordner unter shared_assets 1, Punktdatei 1 (mit Hinweis), 0-Byte-Datei 1;
  Sandbox danach wieder 0.
- **Regression** mit dem Werkzeug der R2 (`rest_ergebnis.txt` C): Doppeleintrag 1, Verzeichnis 1, `\` nur im CD 1,
  Ordner kleingeschrieben 1, Manifest CRLF 0, Geisterzeile 1.
- **cmp-Gates** in check_tree, echter Lauf (`r3_mp_cmp.sh`): TORSE.VBS, RE2/DOOR/DOOR04.DO2, RE15DOOR/P07G.DO2 im
  Paket letztes Byte ^1 -> je EXIT 1 mit der richtigen Meldung.
- **python_finden.sh** (`python_finden_sonde.txt`, Sonde = Original bis auf die zwei Startzeilen, diff belegt):
  11 feindliche Lagen (nur WindowsApps lang/8.3, 8.3 + /usr/bin, RE15_PYTHON = Alias lang/8.3/GROSS/fehlt/relativ,
  kein Python, PATH `.` bzw. `.:/usr/bin` mit cwd = WindowsApps) -> 0 x "WUERDE STARTEN", immer rc 1.
- **bash -n** ok fuer make_package.sh, build_android.sh, apk_pruefen.sh, python_finden.sh; in release/*.sh kein
  blanker python/python3-Aufruf (nur `"$PY"`: make_package.sh :268/:459/:538/:558/:666/:669, apk_pruefen.sh
  :119/:135/:230/:233).
- **Verschlucken** (Code gelesen): jede Gate-Rueckgabe `rc=0; ... || rc=$?` + case/die; `|| true` nur an Anzeigen
  (apk_pruefen.sh :187/:192/:202/:211/:216) bzw. vor expliziten Pruefungen; kein die in einer Kommandosubstitution;
  run_gates/apk_pruefen nie im Bedingungskontext; `source python_finden.sh || die` arbeitet mit expliziten
  Rueckgaben. Stille Abbrueche unter set -e/pipefail (java -version | head, objdump | grep, find | wc) enden alle
  mit Rueckgabe != 0 (fail closed). Kein Weg zu einem falschen EXIT 0 gefunden - B1 ist ein Logik-, kein
  Verschluck-Fehler.
- Selbsttest faengt das Auftragsbeispiel (MU0) und MUP.

## 0. Laufprotokoll
- Dossier angelegt (erster Werkzeugaufruf). Bestand gelesen: git log a358fd5d..HEAD, android_gate_nachbesserung.md,
  pruefer_umgehung_r1/r2.md, Gate-Code ganz, apk_pruefen.sh, build_android.sh, make_package.sh, python_finden.sh,
  android_glue.c.
- Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 121 Zeilen) == Endstand des Bauers R2. `py_zustand.ps1` nur mit
  `-ExecutionPolicy Bypass` fuer den einen Aufruf (nichts umgestellt). Laufender python.exe (C:\Python310, PID 5008)
  gehoerte nicht mir.
- Referenz build/r34a/ref_v0.8.19.apk sha256 514bebd5... = Archiv-SUMS; Linux-Binary v0.8.19 aus dem Archiv-Split-
  Satz (sha256sum -c OK, abfbe7c5..., mtime 19:34). Archiv nur gelesen.
- Selbsttests (echt + MU0-MU8), Batterie, Signieren, Kette echt/Mutant, CRC-Sonde, make_package-Sandbox B1,
  python_finden-Sonde, Randfaelle A/B/C, PC-Pfad B4, cmp-Gates.
- Endstand: Python-Schnappschuss 1 (`py_zustand_1_nachher.txt`) == Schnappschuss 0 (121 Zeilen, diff leer), kein
  python/pymanager/msiexec-Prozess. Geloescht: Faelschungen, Sandboxen sb_mp/sb_kette (Hardlinks - Linkzahlen der
  Originale wieder 1), Binary-Kopien (~9 GB); liegen gelassen build/r34a/pruefer_r3/{logs,mutanten} (2,8 MB).
  `git status --short release/ re15_port/ synchro/` leer; build/r34a/ref_v0.8.19.apk unveraendert (514bebd5...).
