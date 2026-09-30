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
  Commit 42d72ec1 (nur Selbsttest-Teil des Gates). Kampagne (449) laeuft im Hintergrund.

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
im APK-Abschnitt). Kosten: ein Selbsttest mehr je Paketlauf (12-14 s ruhig; mit APK laeuft apk_pruefen
seinen eigenen Selbsttest auf derselben Kopie zusaetzlich - bewusst belassen, build_android.sh braucht ihn).

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
`/c/Python310/python release/apk_asset_gate.py --selbsttest` -> **SELBSTTEST-OK 223/223 + 61/61 innere Proben**,
12,4-13,4 s (vorher 202 + 58, 11-12 s). Der Fall 223 (Manifest 64 MiB + 1 B) kostet rund 1 s.

### 6.2 `--gate-only` gegen die Referenz-APK (`gate_only_ref_r4.txt`)
`bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19` -> **EXIT 0** (62 s unter
Last): Pruefkopie 514bebd5... (= Archiv-SUMS), aapt v0.8.19 + beide ABIs, zipalign ok, v2 true, Signer 432bc749...,
Selbsttest 223/223 + 61/61, Tuer-Soll RE15DOOR 30/30 + RE2/DOOR 27/27, ZIP 3616/3616 lesbar, 3603/3603 bytegleich,
Manifest 3603 Zeilen, `ANDROID-GATES-OK`.

### 6.3 Mutanten MU0-MU8 + MUP (`selbsttests_r4.txt`, Werkzeug der Pruefer unveraendert)
`r3_mutanten.py` erzeugt alle neun Mutanten aus dem neuen Gate (jedes Muster genau einmal gefunden). ECHT OK
223/223; MU0 FEHLER 5, MU1 5, MU2 2, MU3 6, MU4 2 innere Proben, MU5 3, MU6 3, MU7 3, MU8 6, MUP 3 - **alle acht
MU1-MU8 gefangen** (vorher alle OK 202/202). Fallzuordnung Abschnitt 1.

### 6.4 B3 Funktions-Sonde (`zip_kennung_sonde_r4.txt`, `r4_zip_kennung_sonde.sh`)
CRC32-/Groessen-gleiche Faelschung (r3_crc_gleich.py: sha256 2dd9497b..., CRC32 d16ad30a, 363212403 B wie die
Referenz 514bebd5...), gezippt wie make_package (zip -s 90m -j), verify_split + verify_apk_im_zip per awk
unveraendert geschnitten (Endstand 80559d2f bzw. 9d2337e4):
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

### 6.6 B1 in der Sandbox, Lauf 1 (Stand 3b677ea0; `build/r34a/r4/mp_veraltet_r4_lauf1_stand3b677ea0.txt`)
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
Wiederholung mit dem Endstand (80559d2f): 6.7.

(weitere Messungen folgen)
