# Gegenpruefung Runde 3 — Linse UMGEHUNG/ROBUSTHEIT (Android-Asset-Gate)

Pruefer: Gegenpruefer R3 (Umgehung/Robustheit). Stand: 2026-09-30, laufend fortgeschrieben.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate).
Pruefgegenstand: Werkzeugstand eff38fc1 (release/apk_asset_gate.py v6 sha256 72cfa6b2..., apk_pruefen.sh,
build_android.sh, make_package.sh, python_finden.sh, zip_exec_bit.py, apk_signer.sha256, app/build.gradle).
Auftrag: das Gate zu einem FALSCHEN Ergebnis bringen (eigene Faelschungen, die der Bauer nicht
getestet hat), verschluckte Fehler in build_android.sh / make_package.sh suchen,
--selbsttest gegen absichtlich kaputte Gates pruefen. Ich aendere KEINE Werkzeuge.

Belege: analysis/befunde_runde34_android/pruefer_umgehung_r3_belege/
Arbeitskopien/Faelschungen: build/r34a/pruefer_r3/ (nicht versioniert)
Parallel im Baum: Pruefer "echtlauf r3" (voller Android-Bau + make_package.sh in release/). release/ und
platform/android/ fasse ich NICHT an; make_package-Laeufe und Mutanten-Ketten nur in eigenen Sandboxen
(`r3_sandbox_anlegen.sh`: unveraenderte Skriptkopien, Quellbaum per Hardlink, eigenes git-Repo).

## 1. Befunde (Kurzfassung; Einzelheiten Abschnitt 2)

| Nr | Schwere | Befund |
|---|---|---|
| B1 | mittel | make_package.sh liefert einen ALTEN Android-Split-Satz derselben Version ungeprueft aus, wenn die APK fehlt (SHA256SUMS.txt + git add) - Messung laeuft (Abschnitt 2.1) |
| B2 | mittel | Selbsttest (202/202) faengt 8 natuerliche Teil-Abschwaechungen des Manifest-/Methodenlesers NICHT (Klassen ausserhalb A-I des Bauers); zwei davon lassen eine mit dem Release-Schluessel signierte, auf dem Geraet fehlerhafte APK durch die GANZE Kette (EXIT 0) |
| B3 | niedrig | verify_apk_im_zip (make_package.sh) haelt die gezippte APK nur ueber CRC32 + Groesse fest: eine andere Datei gleicher Groesse/CRC32 gilt als "gepruefte APK" |

Was haelt (gemessen, Abschnitt 3): das ECHTE Gate lehnt alle 10 neuen Faelschungen ab bzw. nimmt die zwei
harmlosen an (fuehrende Null, Eintrag ohne Namen); die echte Kette lehnt die signierten Faelschungen ab.

## 2. Einzelheiten

### 2.1 B1 (mittel) Alter Android-Split-Satz wird ungeprueft ausgeliefert
Code (make_package.sh eff38fc1): die APK-Pruefung (:481-499) und das Zippen (:688-708) laufen NUR, wenn
`release/<NAME>_android.apk` da ist; `rm -f "${NAME}_android".z*` steht im selben Zweig (:699). Fehlt die APK,
meldet :707 "(kein Android-Paket: ... fehlt)", aber :709 `sha256sum "${NAME}"_*.z* > SHA256SUMS.txt` und
:763-766 `git add release/${NAME}_*.z*` nehmen JEDEN vorhandenen `<NAME>_android.z*` mit - auch einen aus einem
frueheren Lauf derselben Version (die v0.8.19-Volumes sind im Repo versioniert: `git status` zeigt sie als M).
build_android.sh loescht die APK schon VOR Gradle (:289) - nach einem gescheiterten Android-Bau fehlt sie also
genau in dem Fall, in dem der alte Satz nicht mehr zum Quellbaum passt.
Messung: `r3_mp_veraltet.sh` (Sandbox sb_mp) - Ergebnis folgt.

### 2.2 B2 (mittel) Selbsttest faengt Teil-Abschwaechungen neuer Klassen nicht
Die Mutanten-Probe des Bauers (449 Mutanten, Klassen A befund->pass, B raise->pass, C Namensregel, D Hand,
E Vergleichsrichtung/Grenze, F and/or-Operand, G Tupelpaar, H not, I Zahl +-1) mutiert keine Zeichenklassen,
strip-/split-Varianten, Konstanten oder Mengen. `r3_mutanten.py` erzeugt je EINE Textstelle:

| Mutant | Aenderung (eine Zeile) | --selbsttest |
|---|---|---|
| MU0 (Auftragsbeispiel) | `if a_sha != q_sha:` -> `if False:` (sha-Vergleich aus) | **FEHLER 5/202** (05, 30, 31, 167, 168) - gefangen |
| MU1 | Manifestzeile `z.rstrip("\r")` -> `z.rstrip()` | OK 202/202 |
| MU2 | Groessenfeld `r"[0-9]+"` -> `r"\d+"` (derselbe Fehler, den R2-B8 in KOPF_RE fand) | OK 202/202 |
| MU3 | `text.split("\n")` -> `text.splitlines()` | OK 202/202 |
| MU4 | `MANIFEST_MAX = 64 << 20` -> `64 << 30` | OK 202/202 (Grenze nur ueber den Pruefhaken getestet; praktisch unerreichbar) |
| MU5 | `if e.methode not in (0, 8)` -> `if e.methode > 8` | OK 202/202 |
| MU6 | Groessenfeld `re.fullmatch(r"[0-9]+", g)` -> `g.isdigit()` | OK 202/202 |
| MU7 | Kopfzeile `rstrip("\r")` -> `strip()` | OK 202/202 |
| MU8 | Manifestpfad zusaetzlich `pfad.strip()` | OK 202/202 |
Beleg `selbsttests_uebersicht.txt` (Selbsttest je 11-17 s, echtes Gate 202/202).

Jeder dieser Mutanten nimmt eine Faelschung der ECHTEN APK an, die das echte Gate ablehnt
(`batterie_ergebnis.txt`, unsigniert, je rc selbst abgefangen):
| Faelschung (nur Manifest, Zeile RE15DOOR/P07G.DO2) | echtes Gate | Mutant | Geraet (android_glue.c) |
|---|---|---|---|
| F1 Leerzeichen am Pfadende | 1 (Zeile ohne APK-Eintrag + fehlt im Manifest) | MU1 **0**, MU8 **0** | rel = "...P07G.DO2 " -> SDL_RWFromFile findet kein Asset -> Tuerarchiv nie entpackt, Fehlerbild bei jedem Start |
| F4 Seitenvorschub `\x0c` am Zeilenende | 1 | MU1 **0**, MU3 **0** | wie F1 |
| F9 `\x0b` am Zeilenende | 1 | MU1 **0** | wie F1 |
| F10 U+00A0 am Pfadende | 1 | MU8 **0** | wie F1 |
| F6 U+2028 statt `\n` vor der Zeile | 1 | MU3 **0** | Zeile verschmilzt mit der vorigen -> P07D.DO2 und P07G.DO2 nie entpackt |
| F2 Groesse `５５９０８` (Vollbreite) | 1 (keine Zahl) | MU2 **0**, MU6 **0** | atoll -> 0: Datei wird bei JEDEM Start neu kopiert, got != sz -> Fehlerbild, Marker nie geschrieben |
| F2b Groesse arabisch-indisch | 1 | MU2 **0** | wie F2 |

**Ganze Kette** (`r3_kette_mutanten.sh`, `kette_mutanten.txt`): F1 und F2 mit zipalign + DEMSELBEN Schluessel
wie der Release-Bau signiert (apksigner v2 true, Signer 432bc749... = apk_signer.sha256, zipalign -c ok,
`signieren.txt`), dann `build_android.sh --gate-only <apk> --version v0.8.19`:
| Lauf | EXIT |
|---|---|
| echte Skripte, K0 signiert (Kontrolle) | 0 |
| echte Skripte, F1 signiert / F2 signiert | 1 / 1 (`APK-ASSET-GATE-ABWEICHUNG`) |
| Sandbox, nur apk_asset_gate.py := MU1, F1 signiert | **0** - Selbsttest 202/202, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK` (`kette_MU1_F1_leerzeichen_sig.log`) |
| Sandbox, nur apk_asset_gate.py := MU2, F2 signiert | **0** - dito (`kette_MU2_F2_vollbreit_sig.log`) |
| Sandbox mit echtem Gate, F1 signiert (Kontrolle der Sandbox) | 1 |
Ursache: keine Selbsttest-Faelle fuer Whitespace/Steuerzeichen (ausser `\r`) am Zeilen-/Pfadende, andere
Zeilentrenner als `\n`, Nicht-ASCII-Ziffern im Groessenfeld (nur in der Kopfzeile), Methoden 1-7; die
64-MiB-Konstante ist nur ueber den Pruefhaken (min(Konstante, Haken)) erreichbar. Das echte Gate ist an diesen
Stellen RICHTIG - der Selbsttest wuerde eine Regression dort aber nicht bemerken.

### 2.3 B3 (niedrig) Identitaet der gezippten APK nur ueber CRC32 + Groesse
make_package.sh:693-697 vergleicht vor dem Zippen die sha256-Kennung (gut); nach dem Zippen prueft
verify_apk_im_zip (:557-582) nur CRC32 und Groesse aus dem Katalog. Zwischen :693 und :700 (du, echo, rm, zip-Start)
geht ein Tausch also nur ueber CRC32/Groesse ins Paket. `r3_zip_kennung_sonde.sh` (`zip_kennung_sonde.txt`):
Referenz-APK, 1 Byte in den Daten von RE15DOOR/P07G.DO2 gekippt + 4 Ausgleichsbytes (`r3_crc_gleich.py`, eigene
GF(2)-Rechnung) -> sha256 2dd9497b... statt 514bebd5..., CRC32 d16ad30a und 363212403 B gleich; gezippt wie :700;
verify_split + verify_apk_im_zip per awk UNVERAENDERT: `APK im Split-Satz = gepruefte APK (CRC32 d16ad30a,
363212403 B)`, rc 0. Dieselbe Datei: Gate rc 1 (`Inhalt weicht ab ... P07G.DO2`), apksigner `DOES NOT VERIFY`.
Einordnung: braucht einen gezielten Tausch in einem Fenster von Millisekunden (unter Windows laesst sich die
Datei waehrend apk_kennung nicht umbenennen); gegen versehentliche Aenderungen reicht CRC32. Die Meldung
"= gepruefte APK" verspricht aber mehr, als geprueft wird (die sha256 liegt vor und koennte nach dem Zippen per
`unzip -p` aus dem Satz nachgerechnet werden).

## 3. Was HAELT (gemessen)
- Echtes Gate gegen 12 neue Faelschungen (`batterie_ergebnis.txt`): K0 0; F1 Leerzeichen 1, F2/F2b Unicode-Ziffern 1,
  F3 NUL am Manifestende 1 (strenger als das Geraet, das am NUL aufhoert - harmlos), F4 FF 1, F6 U+2028 1,
  F7 zusaetzlicher Eintrag `p07g.do2` (gross/klein) + Manifestzeile 1, F9 VT 1, F10 NBSP 1; F5 fuehrende Null
  (`055908`) 0 = richtig (atoll und int lesen 55908), F8 Eintrag mit leerem Namen 0 (wie der Selbsttestfall "letzter Eintrag ohne Name ... gut"; aapt2-Gegenprobe siehe unten).
- Echte Kette gegen signierte F1/F2: EXIT 1.

## 0. Laufprotokoll

- Dossier angelegt (erster Werkzeugaufruf). Bestand gelesen: git log a358fd5d..HEAD (Bauer bis eff38fc1),
  android_gate_nachbesserung.md (R1+R2), pruefer_umgehung_r1/r2.md, Gate-Code ganz (2970 Zeilen),
  apk_pruefen.sh, build_android.sh, make_package.sh, python_finden.sh, android_glue.c (Geraete-Leser).
- Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 121 Zeilen) == Endstand des Bauers R2
  (`nachbesserung_r2_belege/py_zustand_1_nach_allen_laeufen.txt`, diff leer). Laufender python-Prozess
  PID 5008 C:\Python310 = nicht meiner (parallele Sitzungen). `py_zustand.ps1` braucht
  `-ExecutionPolicy Bypass` fuer den einen Aufruf (Systemrichtlinie blockt Skripte; nichts umgestellt).
- Referenz: build/r34a/ref_v0.8.19.apk sha256 514bebd5... = Archiv-SUMS (Archiv nur gelesen).
  Linux-Binary v0.8.19 aus dem Archiv-Split-Satz (sha256sum -c OK, re15_pc abfbe7c5..., mtime 19:34).
- Selbsttests echtes Gate + MU0-MU8; Batterie F1-F10 + Mutanten; Signieren F1/F2/K0; Kette echt/Mutant;
  CRC-Sonde. make_package-Sandbox (B1) laeuft.
