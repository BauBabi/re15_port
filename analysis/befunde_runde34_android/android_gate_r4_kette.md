# Android-Gate R4 — die Kette (B1–B4, README)

Stand: 2026-09-30, Zweig `r34a/android-gate`, Arbeitsbaum `.claude/worktrees/r34a_android`.
Auftrag (Leiter, woertlich aus dem Workflow): nur die Kette — B1 (alter Android-Satz), B2 (8 Mutanten
im Selbsttest), B3 (APK im Split-Satz per sha256), B4 (Selbsttest vor PC-Pfad), README. Der
Geraete-Entpacker N1 gehoert einem anderen Agenten; der Manifestteil von `apk_asset_gate.py` wird nur
angefasst, wo B2 es verlangt.
Belege: `analysis/befunde_runde34_android/android_gate_r4_belege/`, Arbeitsordner (nicht versioniert)
`build/r34a/r4/`.

## 0. Laufprotokoll (fortlaufend)

- Dossier angelegt (erster Werkzeugaufruf). Bestand gelesen: git log c1c91713..HEAD, pruefer_umgehung_r3.md,
  pruefer_echtlauf_r3.md, android_gate_nachbesserung.md (R1+R2), make_package.sh, apk_pruefen.sh,
  build_android.sh, apk_asset_gate.py (Aufbau, manifest_pruefen, Selbsttest), android_glue.c :60-240,
  Pruefer-Werkzeuge r3_* und die Kampagnen-Werkzeuge mutanten_teil.py/hand_r2.py, fuenf Memory-Regeln.
- Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 121 Zeilen) == Endstand Pruefer R3 (`diff` leer).
  Python immer `/c/Python310/python` bzw. `release/python_finden.sh` (-> /c/Python310, 3.10.11).
- B2 gebaut (Selbsttest 223/223 + 61/61), MU0-MU8 + MUP je SELBSTTEST-FEHLER (`selbsttests_r4.txt`),
  Commit 42d72ec1 (nur Selbsttest-Teil des Gates). Kampagne (449) im Hintergrund.
- Kette B1/B3/B4 + README gebaut, Commit 3b677ea0; B3-Sonde (Lauf 1 der Sonde verlor verify_split: awk aus
  msys64 + `>>`, Sonde korrigiert), B1-Sandbox Lauf 1 (L1-L6).
- Drei Selbsttest-Faelle mehr (VT/FF als Trenner, Groesse 64 GiB), Commit 7f269d04; B1 nachgeschaerft
  (verify_split ohne fremde Dateien, Android-Volumes mit sha256 festgehalten), Commit 80559d2f.
- Frischer Android-Bau (APK 66c5d8e1...), Kampagne fertig (445/449), Mutanten/Kette/Sonden am Endstand,
  B1-Sandbox Lauf 2, B4-Sandbox, B3 echter Fluss (Lauf 1 der Sonde: relativer Pfad in der zip-Attrappe -> EXIT 12
  fuer NEU UND ALT, Sondenfehler; korrigiert und wiederholt), make_package echt E1-E3.
- Dabei gefunden: die Gate-Kopie blieb nach JEDEM erfolgreichen Zip-Lauf in /tmp liegen (8 Reste) - rm aus
  msys64 nach der PATH-Ergaenzung. Behoben (Windows-Pfad), Commit cb6617fd; Aufraeum-Test T1-T3, B3 wiederholt.
- Aufgeraeumt, Python-Schnappschuss 2, Abschluss-Commit.

## 1. B2 — Selbsttest faengt MU1-MU8 (Commits 42d72ec1, 7f269d04)

Nur `_faelle()` (24 neue Faelle 203-226), `_innere_proben()` (3 Proben) und eine Fixture-Zahl; der
Pruefcode (`manifest_pruefen`, `struktur_pruefen`, `_manifest_grenze`) ist UNVERAENDERT - die Muster von
`r3_mutanten.py` treffen weiter je genau eine Stelle, die Mutantenliste der Kampagne ist bis auf
Zeilennummern identisch (449). Selbsttest jetzt **226 Faelle + 61 innere Proben**.

| Mutant (Pruefer R3) | faengt jetzt (Fall-Nr., Selbsttest 226; `selbsttests_r4.txt`) |
|---|---|
| MU0 sha-Vergleich aus (Kontrolle) | 05, 30, 31, 167, 168 (wie vorher) |
| MU1 `z.rstrip()` | 203 Leerzeichen, 204 Tab, 205 VT, 206 FF, 207 NBSP am Pfadende |
| MU2 `\d+` im Groessenfeld | 216 Vollbreit, 217 arabisch-indisch |
| MU3 `splitlines()` | 205 VT, 206 FF am Zeilenende; 209 U+2028, 210 `\r`, 211 FS, 212 NEL, 213 VT, 214 FF als Trenner statt `\n` |
| MU4 `MANIFEST_MAX = 64 << 30` | innere Probe `_manifest_grenze()` (ohne Haken 64 MiB; Haken 64 MiB + 1 hebt NICHT an); Fall 226 (Manifest 64 MiB + 1 B ohne Haken) - allein belegt: MU4 mit stillgelegten inneren Proben -> genau Fall 226 rot (`mu4_fall_r4.txt`) |
| MU5 `methode > 8` | 223 Methode 1 / 224 Methode 7 auf Deflate-Daten (Mutant rc 0), 225 Methode 1 auf Stored-Daten (Meldung fehlt) |
| MU6 `isdigit()` | 216, 217 (rc 0), 218 hochgestellt (rc 2 - int() wirft) |
| MU7 Kopf `strip()` | 220 Leerzeichen davor, 221 Leerzeichen dahinter, 222 Tab dahinter |
| MU8 `pfad.strip()` | 203-207, 208 Leerzeichen am Pfadanfang |
| MUP Paket-sha aus (B4) | 132, 185, 186 (wie vorher) |
Dazu 219 Kopfzeile in Vollbreit-Ziffern (KOPF_RE; die arabisch-indische gab es schon, B8) und 215 Groessenfeld
64 GiB + 2748 bei passender Kopfzeile (ein Leser mit 32-Bit-Groessen saehe 2748 B; das Geraet liest atoll 64 Bit
und scheitert an der Groesse). "Groesse >= 64 GiB" aus dem Auftrag: ein Manifest >= 64 GiB ist nicht baubar -
getroffen wird die Konstante (Fall 226 + innere Probe) und eine Groessenangabe >= 64 GiB (Fall 215).

Erwartete Texte nur ASCII: die Ausgabe des Gates ist unter Windows in der ANSI-Codepage kodiert (ein NBSP
kaeme als 0xA0 an und waere nach dem UTF-8-Dekodieren im Selbsttest ein Ersatzzeichen).

## 2. B1 — Android-Satz nur aus DIESEM Lauf (Commit 3b677ea0)

**Entscheidung: Abbruch (fail closed) + ausdruecklicher Schalter `--ohne-android`.** Begruendung:
- Ein Satz derselben Version liegt nach dem Release-Commit VERSIONIERT im Repo (v0.8.19: `git ls-files`
  nennt `re15_port_v0.8.19_android.z01/.zip`). Ein Lauf, dem nur die APK fehlt (abgelehnt, oder
  build_android.sh hat sie vor einem gescheiterten Gradle-Lauf geloescht), soll ihn weder ungeprueft
  mitliefern (der Befund) noch ungefragt aus dem Repo werfen (das waere die Falle
  `reai-v2-paket-plattform-kollateral` in neuer Form: ein Aufraeumschritt loescht still, woran gerade nicht
  gearbeitet wird). Abbruch mit Meldung zwingt die Entscheidung; sie kostet einen Befehl.
- "nur PC" bleibt moeglich: ohne Satz derselben Version wie bisher ohne Schalter (neue Version = Normalfall);
  mit Satz derselben Version `--ohne-android` - dann entfernt das Skript den Satz (Datei im Zip-Abschnitt,
  git-Index im Git-Schritt ueber `behalten()`), SUMS/git add enthalten ihn nicht.
- Aeltere Versionen (`re15_port_<alt>_android.z*`) bleiben unangetastet wie bisher (sie tragen ihre
  Version im Namen und stehen nicht in den SUMS der neuen Version).

Umsetzung (release/make_package.sh):
1. vor den Kopierminuten (nach APK-Pruefung): `DO_ZIP && !--ohne-android && keine gepruefte APK && Satz
   dieser Version da` -> `ABBRUCH: Android-Satz DIESER Version liegt vor, aber in diesem Lauf gibt es keine
   gepruefte APK ...` mit beiden Auswegen (build_android.sh / --ohne-android);
2. Zippen: `--ohne-android` ueberspringt APK und entfernt den Satz; sonst wie bisher;
3. Schlusswache vor SHA256SUMS.txt: Satz ohne `ANDROID_GEZIPPT` (auch einer, der erst waehrend der
   Kopierminuten auftaucht) -> Abbruch;
4. SHA256SUMS.txt und `git add` nehmen `<NAME>_android.z*` nur mit `ANDROID_GEZIPPT` (doppelt zur Wache;
   git add auch mit `--no-zip`, wo nie ein Satz entsteht).
Mit `--ohne-android` wird eine vorhandene APK nicht geprueft und nicht gezippt (Meldung).

Nachgeschaerft (Commit 80559d2f, "SHA256SUMS nur aus den in DIESEM Lauf erzeugten Volumes" - die
Abhilfe-Richtung des Pruefers fuer Android zu Ende gedacht):
5. `verify_split` verlangt auch, dass neben dem Satz KEINE fremden Dateien liegen (`<satz>.z*` wurde vor dem
   Zippen geloescht; mehr als die Volumes 1..disk + letztes = nicht aus diesem zip-Lauf) - fuer alle Saetze;
6. Android: sha256 der Volumes VOR und NACH `verify_apk_im_zip` (gleich = geprueft wurde genau das);
   SHA256SUMS.txt bekommt fuer Android genau diese Zeilen, `git add` genau diese Dateien; unmittelbar vor dem
   Schreiben der SUMS wird noch einmal verglichen (veraendert/ergaenzt -> Abbruch);
7. SHA256SUMS.txt schreibt nur noch Bash selbst (das `sha256sum` nach der PATH-Ergaenzung stammt aus
   `/c/msys64/usr/bin` = andere MSYS-Laufzeit, siehe 3.).

Bleibt (bewusst nicht geaendert, ausserhalb B1):
- Das Fenster zwischen dem letzten Vergleich und `git add` (Millisekunden): ein dort getauschtes Volume kaeme
  mit anderem Inhalt als in SHA256SUMS.txt in den Index - der Archiv-Schritt der Auslieferung (`sha256sum -c`,
  Memory reai-v2-immer-paketieren) wuerde es melden.
- PC-Saetze: mit `--only win|linux` bleiben die Volumes der anderen Plattform derselben Version in
  SHA256SUMS.txt/git (gewollt: Memory reai-v2-paket-plattform-kollateral); sie werden in diesem Lauf nicht neu
  geprueft.
- Der Git-Schritt laeuft auch mit `--no-zip` (merkt vorhandene PC-Volumes der Version vor, raeumt alte
  PC-Versionen ab) - vorbestehend; Android beruehrt er dort nicht mehr (L6).

## 3. B3 — APK im Split-Satz per sha256 (Commit 3b677ea0)

`verify_apk_im_zip`: Stufe 1 wie bisher (Katalog: genau der eine Eintrag, CRC32 + Groesse = Kennung),
Stufe 2 neu: `zip -q -s 0 <satz> --out <tmp>/ganz.zip`, `unzip -Z1` muss genau den Eintragsnamen nennen,
`unzip -p | sha256sum` muss die sha256 der Pruefkopie ergeben (unzip prueft dabei die CRC32). Kein
Namensmuster an unzip (es wuerde `[`/`*`/`?` auswerten) - alle Eintraege = der eine, eben gepruefte.
Aufrufer: `verify_apk_im_zip ... || die "Android-Satz ... enthaelt NICHT die gepruefte APK"`.
- **Falle gemessen:** das `zip` aus `/c/msys64/usr/bin` (make_package traegt es in den PATH nach) hat eine
  eigene Einhaengetabelle - `/tmp` ist dort NICHT der Temp-Ordner von Git-Bash (`zip error: Nothing to do!`).
  Pfade fuer zip/unzip daher per `cygpath -m`.
- **Zweite Falle (in meiner Sonde, nicht im Skript):** ein Programm aus `/c/msys64/usr/bin` (andere
  MSYS-Laufzeit) erbt von Git-Bash das `O_APPEND` von `>>` nicht und schreibt ab Byte 0 - Lauf 1 der Sonde
  verlor so `verify_split` (der zweite awk-Schnitt ueberschrieb den ersten). make_package.sh hat kein `>>`
  (grep), Sonde korrigiert (awk vor dem PATH-Wechsel).

## 4. B4 — Selbsttest vor dem PC-Pfad (Commit 3b677ea0)

make_package.sh kopiert `release/apk_asset_gate.py` nach dem Python-Finder in einen privaten Temp-Ordner
(`MP_TMP`), faehrt `--selbsttest` auf der Kopie (Abbruch bei rc != 0: "dem Gate ist nicht zu trauen;
Quellbaum-, APK- und Paketpruefung unterbleiben, nichts wird kopiert oder gezippt"), und GENAU diese Kopie
macht `--quellbaum`, `--paket` (check_tree) und ueber `APK_GATE_DATEI` auch Schritt 5 in apk_pruefen.sh
(build_android.sh setzt die Variable nicht -> dort wie bisher release/apk_asset_gate.py). Eine EXIT-Falle
`mp_aufraeumen` raeumt Gate-Kopie und APK-Pruefkopie ab (ersetzt das fruehere `trap ... EXIT; trap - EXIT`
im APK-Abschnitt). Kosten: ein Selbsttest mehr je Paketlauf (12-14 s ruhig, 26 s unter Last im echten Lauf; mit
APK laeuft apk_pruefen seinen eigenen Selbsttest auf derselben Kopie zusaetzlich - bewusst belassen,
build_android.sh braucht ihn).
- **Leck gefunden und behoben (Commit cb6617fd):** mit dem `/tmp/...`-Pfad blieb die Gate-Kopie nach JEDEM
  erfolgreichen Lauf MIT Zippen liegen (8 Reste; Abbrueche und `--no-zip` raeumten ab): der Zip-Abschnitt stellt
  `/c/msys64/usr/bin` vorn in den PATH, danach ist `rm` das rm von MSYS2, dessen `/tmp` `C:/msys64/tmp` ist - die
  Falle loeschte dort "nichts" (per `bash -x` gesehen: `rm -rf /tmp/re15_make_package.X` lief, der Ordner blieb).
  Jetzt werden `MP_TMP` und (apk_pruefen.sh) `APK_PRUEF_TMP` sofort per `cygpath -m` zum `C:/...`-Pfad;
  `verify_apk_im_zip` benutzt nach `mktemp` nur noch den Windows-Pfad. Beleg 6.13.

## 5. README (Commit 3b677ea0)

`re15_port/platform/android/README.md`, Abschnitt "Bauen": Python >= 3.8 ueber `release/python_finden.sh`
(Kandidaten, WindowsApps-Alias nie gestartet, Abbruch vor Gradle), `--gate-only`, die Pruefschritte der Kette
(Pruefkopie, Stichproben, aapt, zipalign, apksigner + Signer-Pin, Selbsttest, volle Asset-Pruefung mit
Tuer-Soll, Auslieferungsname erst danach), was make_package.sh zusaetzlich tut (Frische, sha256 aus dem Satz,
Android-Satz nur aus diesem Lauf, `--ohne-android`), die fuenf Baeume MIT `shared_assets/RE15DOOR` (und wo
dieselbe Liste noch steht), Signer-Pin bei eigenem Schluessel bzw. anderer Bau-Maschine (Abbruchmeldung,
`release/apk_signer.sha256` oder `RE15_APK_SIGNER_SHA256`). Den Abschnitt "Bekannte Grenzen" (Update-Verhalten
des Entpackers) habe ich NICHT angefasst - das ist N1 (anderer Agent).

## 6. Messungen (Maschine unter Last: Granaten-Sitzung baut parallel, dazu meine Kampagne)

### 6.1 Selbsttest
`/c/Python310/python release/apk_asset_gate.py --selbsttest` -> **SELBSTTEST-OK 226/226 + 61/61 innere Proben**
(Endstand; 223 Faelle: 12,4-13,4 s ruhig, vorher 202 + 58 in 11-12 s; 226 Faelle unter Last 14-45 s). Der Fall
226 (Manifest 64 MiB + 1 B) kostet rund 1 s.

### 6.2 `--gate-only` gegen die Referenz-APK (`gate_only_ref_r4.txt`)
`bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19` -> **EXIT 0** (62 s unter
Last): Pruefkopie 514bebd5... (= Archiv-SUMS), aapt v0.8.19 + beide ABIs, zipalign ok, v2 true, Signer 432bc749...,
Selbsttest 223/223 + 61/61, Tuer-Soll RE15DOOR 30/30 + RE2/DOOR 27/27, ZIP 3616/3616 lesbar, 3603/3603 bytegleich,
Manifest 3603 Zeilen, `ANDROID-GATES-OK`. Am Endstand (226 Faelle, Windows-Pfad der Pruefkopie) wiederholt: 6.13 T2, EXIT 0.

### 6.3 Mutanten MU0-MU8 + MUP (`selbsttests_r4.txt`, Werkzeug der Pruefer unveraendert)
`r3_mutanten.py` erzeugt alle neun Mutanten aus dem Endstand-Gate (jedes Muster genau einmal gefunden). ECHT OK
226/226; MU0 FEHLER 5, MU1 5, MU2 2, MU3 8, MU4 2 innere Proben, MU5 3, MU6 3, MU7 3, MU8 6, MUP 3 - **alle acht
MU1-MU8 gefangen** (vorher alle OK 202/202). Fallzuordnung Abschnitt 1. Fall 226 allein gegen MU4
(`mu4_fall_r4.txt`, `r4_mu4_fall.sh`): innere Proben stillgelegt -> ohne MU4 OK 226/226, mit MU4 genau Fall 226
FEHLER (`fehlende Meldung: 'Manifest 67108865 B > 67108864 B (64 MiB)...'`).

### 6.4 B3 Funktions-Sonde (`zip_kennung_sonde_r4.txt`, `r4_zip_kennung_sonde.sh`)
CRC32-/Groessen-gleiche Faelschung (r3_crc_gleich.py: sha256 2dd9497b..., CRC32 d16ad30a, 363212403 B wie die
Referenz 514bebd5...), gezippt wie make_package (zip -s 90m -j), verify_split + verify_apk_im_zip per awk
unveraendert geschnitten (Endstand cb6617fd bzw. 9d2337e4; Ergebnis am Stand 80559d2f gleich):
| Satz | NEU | ALT (9d2337e4) |
|---|---|---|
| K Referenz | rc 0 `APK im Split-Satz = gepruefte APK (entpackt: sha256 514bebd55bd6b8ec... = Pruefkopie)` | rc 0 |
| F Faelschung | **rc 1** `die entpackte APK hat sha256 2dd9497b..., die gepruefte 514bebd5... - NICHT die gepruefte Datei` | rc 0 `= gepruefte APK (CRC32 d16ad30a, ...)` (der Befund) |
| E Satz K + fremde `...android.z05` daneben | **verify_split rc 1** `fremde Dateien neben dem Satz (nicht aus diesem zip-Lauf)` | rc 0 |
Keine Temp-Reste (`re15_apk_satz.*`: 0).

### 6.5 Fruehere Kampagne (`kampagne_r4_*.txt`/`.tsv`, Werkzeug mutanten_teil.py + hand_r2.py unveraendert)
Gegen das Gate mit 223 Faellen (Pruefcode unveraendert; die drei spaeteren Faelle 213-215 koennen nur mehr
fangen): **449 Mutanten, 445 erkannt, 4 ueberlebt** (1551 s, --parallel 3). Namensgenauer Vergleich mit
Kampagne 4 der R2 (`kampagne_r4_vergleich.txt`, Zeilenversatz +4 durch 4 Doku-Zeilen im Kopf): dieselben 449
Namen, **kein Mutant schlechter**; die 4 Ueberlebenden sind genau die 4 in der R2 als aequivalent begruendeten
(`_ant_passt` i/j `== -> >=`, `tuer_soll` spender `== -> >=`, `_eintrag_pruefen` `n != usize -> <`). Je Klasse
A 47/47, B 45/45 (1 per Zeitgrenze wie R2), C 5/5, D 35/35, E 169/173, F 71/71, G 5/5, H 42/42, I 26/26.

### 6.6 B1 in der Sandbox, Lauf 1 (Stand 3b677ea0; `mp_veraltet_r4_lauf1_stand3b677ea0.txt`)
`r4_mp_veraltet.sh`, make_package.sh ECHT (Sandbox: Skripte = Arbeitsbaum, Quellbaum per Hardlink, eigenes
git-Repo), `--version v0.8.19 --only linux`, Linux-Binary aus dem Archiv (19:34, kein touch):
| Lauf | EXIT | Ergebnis |
|---|---|---|
| L1 APK = Referenz | 0 (233 s) | Selbsttest der Gate-Kopie 223/223, QUELLBAUM-OK, APK-PRUEFUNG-OK, PAKET-OK, Katalog + `entpackt: sha256 514bebd5... = Pruefkopie`, SUMS 4 Volumes, 4 vorgemerkt; danach Release-Commit in der Sandbox (Satz versioniert) |
| Quellbaum aendert sich | - | effect0_blood.tim in der Sandbox 1 Byte anders (Original unveraendert) |
| L2 APK noch da | 1 (101 s) | APK-Asset-Gate: weicht vom Quellbaum ab |
| L3n ohne APK, NEUES Skript | **1** (50 s) | `ABBRUCH: Android-Satz DIESER Version liegt vor, aber in diesem Lauf gibt es keine gepruefte APK ... Entweder ... build_android.sh ... oder ... --ohne-android`; 0 x "Assets kopieren"; Satz + SUMS `sha256sum -c` OK gegen den Release-Commit, Index gegen HEAD leer |
| L3a ohne APK, ALTES Skript (9d2337e4) | 0 (73 s) | Kontrolle = der Befund: `(kein Android-Paket ...)`, SUMS nennt den alten Satz, `4 neue vorgemerkt` |
| L4 `--ohne-android` | **0** (71 s) | `der Satz dieser Version wird entfernt`, beide Volumes entfernt, SUMS nur Linux (2 Volumes), Index: `D` android.z01/.zip, `M` linux |
| L5 Satz unversioniert aber vorgemerkt, `--ohne-android --zip-only` | **0** (74 s) | Satz entfernt und aus dem Index (vorher `A`), SUMS nur Linux |
| L6 Satz unversioniert, nicht vorgemerkt, `--no-zip` | **0** (35 s) | Satz bleibt liegen und wird NICHT vorgemerkt (vorgemerkt nur die Linux-Volumes - vorbestehend: der Git-Schritt laeuft auch mit --no-zip) |
Wiederholung mit dem Endstand: 6.7.

### 6.7 B1 in der Sandbox, Lauf 2 (Stand 80559d2f; `mp_veraltet_r4.txt`, Logs `build/r34a/r4/logs/mp/`)
Dieselbe Folge mit den nachgeschaerften Skripten (Sandbox-Commit 82c1ab5):
| Lauf | EXIT | Ergebnis |
|---|---|---|
| L1 APK = Referenz | 0 (149 s) | `APK im Split-Satz = gepruefte APK (entpackt: sha256 514bebd5... = Pruefkopie)`, `SHA256SUMS.txt geschrieben (4 Volumes, Android-Satz aus diesem Lauf)`, 4 vorgemerkt (A x4), Release-Commit |
| L2 APK noch da, Quellbaum geaendert | 1 (55 s) | APK-Asset-Gate weicht ab |
| L3n ohne APK, NEU | **1** (23 s) | B1-ABBRUCH, 0 x "Assets kopieren", Satz + SUMS `sha256sum -c` OK, Index leer |
| L3a ohne APK, ALT (9d2337e4) | 0 (64 s) | der Befund: SHA256SUMS.txt nennt `...android.z01/.zip` des alten Satzes |
| L4 `--ohne-android` | **0** (89 s) | Satz entfernt, SUMS 2 Volumes (nur Linux), Index `D` android, `M` linux |
| L5 vorgemerkt + `--ohne-android --zip-only` | **0** (75 s) | Satz entfernt, aus dem Index |
| L6 unversioniert + `--no-zip` | **0** (90 s) | Satz bleibt, NICHT vorgemerkt |
Der Stand cb6617fd (Temp-Pfade) aendert an diesen Wegen nichts ausser dem Aufraeumen (6.13).

### 6.8 B4 in der Sandbox (`mp_ohne_selbsttest_r4.txt`, `r4_mp_ohne_selbsttest.sh`)
Wie `r3_mp_ohne_selbsttest.sh`: `--version v0.8.19 --only linux --zip-only`, 1 Byte in der PAKET-Kopie von
`shared_assets/PSX/DATA/TEX.TIM` (Linkzahl 1 geprueft, danach zurueck):
| Lauf | EXIT | Ergebnis |
|---|---|---|
| L0 `--no-zip`, echtes Gate (Paket anlegen) | 0 (47 s) | Selbsttest der Gate-Kopie 226/226, QUELLBAUM-OK, PAKET-OK |
| LA echtes Gate | 1 (51 s) | `Inhalt weicht ab: shared_assets/PSX/DATA/TEX.TIM ... APK-ASSET-GATE-PAKET-ABWEICHUNG` |
| LB Gate := MUP (`if q_sha != p_sha:` -> `if False:`), NEUES Skript | **1** (17 s) | `SELBSTTEST-FEHLER: 3 von 226` (132, 185, 186) -> `ABBRUCH: Selbsttest des Asset-Gates fehlgeschlagen (rc=1) - dem Gate ist nicht zu trauen` - VOR `--quellbaum` (kein QUELLBAUM-OK im Log) |
| LC MUP + ALTES Skript (9d2337e4) | 0 (44 s) | der Befund: `PAKET-OK` mit veraenderter Datei, `== Fertig ==` |

### 6.9 B3 im echten Fluss (`mp_zip_tausch_r4.txt`, `r4_mp_zip_tausch.sh`)
Sandbox, APK = Referenz, eine zip-ATTRAPPE vorn im PATH reicht alles an `/c/msys64/usr/bin/zip` durch, legt aber
beim Zippen der APK die CRC32-/Groessen-gleiche Faelschung (gleicher Name) in den Satz - die Datei unter release/
bleibt die gepruefte, die Kennungspruefung VOR dem Zippen sieht nichts:
| Lauf | EXIT | Ergebnis |
|---|---|---|
| LN NEUES Skript (Endstand) | **1** (94 s) | Katalog `CRC32 d16ad30a, 363212403 B = gepruefte Kennung` (Stufe 1 haelt es NICHT), dann `die entpackte APK hat sha256 2dd9497b..., die gepruefte 514bebd5...` -> `ABBRUCH: Android-Satz ... enthaelt NICHT die gepruefte APK - nicht ausgeliefert`; keine SHA256SUMS.txt |
| LA ALTES Skript (9d2337e4) | 0 (85 s) | der Befund: `APK im Split-Satz = gepruefte APK (CRC32 d16ad30a, ...)`, SUMS mit 2 Android-Zeilen, im Satz steckt die Faelschung (sha256 2dd9497b...) |
(Lauf 1 dieser Sonde brach fuer NEU und ALT mit EXIT 12 ab - die Attrappe gab einen RELATIVEN Pfad weiter, zip
lief mit cwd release/: "Nothing to do". Sondenfehler, korrigiert; Protokoll `mp_zip_tausch_r4_lauf1_testfehler.txt`.)

### 6.10 Ganze Kette mit signierten Faelschungen und Mutanten-Gates (`kette_mutanten_r4.txt`, `r4_kette_mutanten.sh`)
Faelschungen mit den unveraenderten Pruefer-Werkzeugen (`r3_faelschen.py`, `r3_signieren.sh`: zipalign + DERSELBE
Debug-Schluessel, apksigner verify rc 0, Signer 432bc749...), `build_android.sh --gate-only ... --version v0.8.19`:
| Lauf | EXIT | vorher (R3) |
|---|---|---|
| echte Skripte: K0_sig / F1_leerzeichen_sig / F2_vollbreit_sig | 0 / 1 / 1 | 0 / 1 / 1 |
| Sandbox, Gate := MU1, F1_sig | **1** (`SELBSTTEST-FEHLER` -> `ABBRUCH: Selbsttest des APK-Asset-Gates fehlgeschlagen`) | **0** (`ANDROID-GATES-OK`) |
| Sandbox, Gate := MU2, F2_sig | **1** (dito) | **0** |
| Sandbox, Gate := MU1 ... MU8, je mit K0_sig | je **1** am Selbsttest (MU4 nach 8 s an den inneren Proben) | - |
| Sandbox, echtes Gate: F1_sig / K0_sig | 1 / 0 | 1 / - |
Keine Pruefkopie blieb liegen.

### 6.11 Voller Android-Bau mit der Endstand-Kette (`android_voll_r4_auszug.txt`)
`bash release/build_android.sh --version v0.8.19 --no-toolchain` (04:45:15-04:50:43) -> **EXIT 0 `ANDROID-BUILD-OK`**:
Python /c/Python310 (3.10.11), Werkzeuge + Signer VOR Gradle, stageAssets `RE2/DOOR: 27`, `RE15DOOR: 30`,
`BUILD SUCCESSFUL in 4m 54s`; Kette auf der Pruefkopie 29,9 s (Selbsttest 226/226 in 16,8 s, Gate 5,4 s): zipalign
ok, v2 true, Signer 432bc749..., Tuer-Soll 30/30 + 27/27, ZIP 3616/3616, 3603/3603 bytegleich. APK
`66c5d8e1...` (363212403 B, 04:50:14). `SHA256SUMS_android.txt` danach per `git restore` zurueck (6.12).

### 6.12 make_package.sh ECHT im Arbeitsbaum (`mp_echt_r4.txt`, `r4_mp_echt.sh`, Logs `build/r34a/r4/logs/echt/`)
PC-Binaries v0.8.19 aus dem Archiv (`r4_binaries.sh`: Split-Saetze `sha256sum -c` 4 x OK, zusammengefuehrt, nur die
Binaries entpackt; re15_pc.exe 30d5b67b..., re15_pc abfbe7c5..., mtime 19:34 - KEIN touch), APK aus 6.11; git nur in
einen Wegwerf-Index (`mp_isoliert_r4.sh`, echter Index vorher und nachher 0 Eintraege). Letzter Commit an den
PC-Pfaden cf386e32 19:02, an den APK-Pfaden 3b677ea0 04:40 (README) - beide Frische-Gates ohne Meldung.
| Lauf | EXIT | Ergebnis |
|---|---|---|
| E1 `--version v0.8.19` | **0** (235,6 s) | Selbsttest der Gate-Kopie 226/226 (26,3 s unter Last), QUELLBAUM-OK, APK-Kette (Selbsttest 226/226 auf derselben Kopie, APK-PRUEFUNG-OK `66c5d8e1... 58626da3 363212403`), Linux: Optimierung 4, glibc 2.29, Tuerarchive 27 + 30, PAKET-OK, LF; Windows: Optimierung 3, 27 + 30, PAKET-OK, Laufzeit-Gate 26/26 + 26/26 aus dem Paket; Zippen linux 3800 (x-Bit ok), win64 3801, android: Katalog `CRC32 58626da3 = gepruefte Kennung`, `entpackt: sha256 66c5d8e146ca6178... = Pruefkopie` (Zippen bis Satzpruefung fertig 22,2 s); `SHA256SUMS.txt geschrieben (6 Volumes, Android-Satz aus diesem Lauf)`, 6 vorgemerkt (Wegwerf-Index) |
| unabhaengig | - | APK aus dem neuen Satz per `zip -s 0` + `unzip -p`: sha256 `66c5d8e1...` = gebaute APK |
| APK weg | - | nach build/r34a/r4/apk_echt/ |
| E2 `--only win --zip-only` (Satz aus E1 liegt) | **1** (33 s) | Selbsttest, QUELLBAUM-OK, dann B1-ABBRUCH; Wegwerf-Index leer, SUMS unveraendert |
| E3 dasselbe + `--ohne-android` | **0** (101 s) | Satz entfernt, `SHA256SUMS.txt geschrieben (4 Volumes)` = linux + win64 (Linux derselben Version bleibt, gewollt), Wegwerf-Index `D` android.z01/.zip, `M` linux/win64 |
Aufraeumen: `git restore --source=HEAD` fuer SHA256SUMS.txt, SHA256SUMS_android.txt und die 6 Volumes; pkg-*,
win_out, linux_out geloescht. `git status --short release/ re15_port/` leer, `--ignored release/` leer, echter
Index 0. (Die Meldung `release/: ABWEICHUNG` des Werkzeugs betrifft nur SHA256SUMS_android.txt: die "vorher"-Liste
wurde NACH dem Android-Bau genommen, der sie beschrieben hatte - jetzt wieder HEAD, cd139335... wie bei R3.)
E1-E3 liefen am Stand 80559d2f (vor dem Temp-Pfad-Fix); das Aufraeumen des Endstands belegt 6.13.

### 6.13 Aufraeumen der Temp-Ordner (`aufraeum_test_r4.txt`, `r4_aufraeum_test.sh`, Stand cb6617fd)
Gezaehlt vor/nach jedem Lauf: `/tmp/re15_make_package.*`, `re15_apk_pruefen.*`, `re15_apk_satz.*`,
`apk_gate_selbsttest_*` und dasselbe unter `C:/msys64/tmp`:
| Lauf | EXIT | Reste vorher -> nachher |
|---|---|---|
| T1 Sandbox, `--only linux`, APK = Referenz (Zippen, APK-Kette, Satzpruefung) | 0 | 0 -> 0 |
| T2 `build_android.sh --gate-only <Referenz>` (Arbeitsbaum) | 0 | 0 -> 0 |
| T3 Sandbox, Abbruch NACH der PATH-Ergaenzung (Ordner `<satz>.z07` laesst das `rm -f` scheitern, `/usr/bin/rm` = msys64) | 1 | 0 -> 0 |
Vorher (Stand 3b677ea0/80559d2f) blieb je erfolgreichem Zip-Lauf ein `/tmp/re15_make_package.*` (168 KB) liegen -
8 Stueck, alle entfernt. Die Wiederholungen von 6.4 und 6.9 am Stand cb6617fd: dieselben Ergebnisse, 0 Reste.

## 7. Endstand

- Commits (Zweig r34a/android-gate): 42d72ec1 (B2 Selbsttest), 3b677ea0 (Kette B1/B3/B4, README), 7f269d04 (B2 drei
  Faelle), 80559d2f (B1 nachgeschaerft), cb6617fd (Temp-Pfade), dazu der Abschluss-Commit (Dossier + Belege).
  Werkzeugdateien: release/apk_asset_gate.py (NUR Selbsttest-Teil + Doku-Kopf), release/make_package.sh,
  release/apk_pruefen.sh, re15_port/platform/android/README.md. Kein Engine-/PC-Spielcode, kein android_glue.c,
  keine build.gradle-Aenderung.
- Python-Schnappschuss 1 (nach den Hauptlaeufen) und 2 (Ende) == Schnappschuss 0 (121 Zeilen): kein Unterschluessel
  unter `HKCU\Software\Python\PythonCore` (der Schluessel selbst existierte schon, leer), kein Startmenue-Ordner
  "Python 3.1x" (nur ProgramData 3.9/3.10/3.12 wie vorher), kein pymanager/msiexec. Alle Laeufe mit /c/Python310
  bzw. python_finden.sh (-> /c/Python310); der einzige fremde python-Prozess am Ende gehoerte einer anderen
  Sitzung (`build/r34n_c/...`).
- Aufgeraeumt: Sandboxen sb_mp/sb_b4/sb_b3/sb_kette/sb_tmp (Hardlinks - Linkzahlen der Originale wieder 1,
  effect0_blood.tim a7ac86c7... unveraendert), signierte Faelschungen, CRC-Saetze, zip-Attrappe, frische APK,
  Binary-Kopien, Wegwerf-Index, Kampagnen-Mutanten; Gradle-Ausgaben (`app/build` 1,5 GB, `app/.cxx`, `.gradle`,
  `build`, `local.properties`; `_deps` lag schon vorher da). Liegen gelassen: `build/r34a/r4/{logs,mutanten,kampagne,alt}`
  (4 MB), `build/r34a/ref_v0.8.19.apk` (sha256 514bebd5... unveraendert).
- `git status --short release/ re15_port/ synchro/` leer.

## 8. Offen / Hinweise

- **N1 (Geraete-Entpacker)** nicht angefasst - anderer Agent. Wenn N1 das Manifestformat aendert (z.B. eine
  Pruefsummen-Spalte), passen `manifest_pruefen` und die Faelle 203-226 (sie bauen das Manifest ueber
  `_Fall.manifest_text` und ersetzen einzelne Zeilen) mit.
- Die APK-Frische misst den letzten Commit an `platform/android` - auch ein reiner README-Commit macht eine vorher
  gebaute APK "VERALTET" (hier: README-Commit 3b677ea0, danach frisch gebaut). Bewusst nicht geaendert.
- Vorbestehend, nicht Teil von B1: der Git-Schritt laeuft auch mit `--no-zip`; mit `--only` bleiben die PC-Volumes
  der anderen Plattform derselben Version in den SUMS (gewollt). Restfenster zwischen letztem Vergleich und
  `git add`: Abschnitt 2.
- Allgemeine Falle fuer Werkzeuge unter Git-Bash: Programme aus `/c/msys64/usr/bin` sind eine ANDERE MSYS-Laufzeit
  (eigenes `/tmp` = C:/msys64/tmp, kein geerbtes `O_APPEND` bei `>>`). make_package.sh stellt sie im Zip-Abschnitt
  vorn in den PATH - danach nur Windows-Pfade an Dateiwerkzeuge geben und Dateien nur von Bash schreiben lassen.
