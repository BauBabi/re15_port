# Gegenpruefung UMGEHUNG/ROBUSTHEIT, Runde 2 (Stufe-4-Kette B1-B4 + Geraete-Entpacker N1)

Pruefer: Linse UMGEHUNG/ROBUSTHEIT, Runde 2. Baum .claude/worktrees/r34a_android, Zweig r34a/android-gate.
Gegenstand: git log --oneline be8b60f3..HEAD (Kette B1-B4 + README, Entpacker N1, Nachbesserung).
Ich aendere KEINE Werkzeuge/Quellen; Mutanten/Zusatzfaelle nur in Kopien unter build/r34a/pruefer_u2/.

Stand: angelegt 2026-09-30T11:56:50+02:00 - Abschnitte folgen.

## 0. Stand / Plan

- [x] 0.1 Ausgangslage (gelesen)
- [ ] 1 Selbsttest gegen NEUE Ein-Zeilen-Mutanten (v2-Teil, Quellpfade, Ausgabe-/Zaehlcode), Ueberlebende mit Urteil
- [ ] 2 Urteil (gate_urteil) / Pin: Taeuschung durch Ausgabe, Umgebung
- [ ] 3 Format v2: Korpus Gate == Geraete-Leser (neue ASCII-/Segmentregel), Regel-Luecken
- [ ] 4 Entpacker: Waisen (Loeschen), Pfade/Puffer, Abbruch/Neustart, Fehlerpfade (JNI, verschluckt), SHA-256
- [ ] 5 Kette: veraltete Saetze (alle Varianten), fremde Namen, APK getauscht/geloescht, Split-Satz-sha256
- [ ] 6 Urteil

## 0.1 Ausgangslage (gelesen, nicht gemessen)

- HEAD beim Start `8cbf88d6` (Abschluss Nachbesserung R4-1), mein Dossier-Commit `41d10775`. Gegenstand `be8b60f3..HEAD`:
  Kette R4 (B1-B4, README), Entpacker N1, Gegenpruefung R4-1 (meine Linse: `nicht haltbar` - H1 hoch; H5, H3, H2 mittel;
  V1/V2, U1-U4 niedrig) und Nachbesserung R4-1 (`android_r4_nachbesserung.md`: alle behoben). Parallel im selben Baum:
  Pruefer ECHTER LAUF R2 (`pruefer_echtlauf_r4_2.md`, sein Emulator `emulator-5586` lief beim Start) - ich committe nur
  meine Pfade, Emulator nur nach `adb devices`/qemu-Pruefung, eigene Bau-/Sandbox-Ordner unter `build/r34a/pruefer_u2/`.
- Gelesen (HEAD): asset_abgleich.{h,c} ganz (SHA-256, Leser, Pfadregel, Plan, Waisen), android_glue.c ganz (Bootstrap,
  entpacken, liste_schreiben, fehler_halten, Waisen-Aufruf, weg-Schleife), apk_pruefen.sh ganz (gate_pin_pruefen,
  gate_festhalten, gate_urteil, gate_laufen, apk_pruefen 0-6), make_package.sh (Gate-Kopie, Selbsttest, Quellbaum, APK,
  B1, fremde_versionsdateien, verify_split, verify_apk_im_zip, Zippen, Positivliste, Git), python_finden.sh,
  build.gradle writeAssetManifest, Gate: `_pfad_fehler`, `manifest_lesen`, `manifest_pruefen`, `quelldateien`,
  `quellpfade_pruefen`, `pruefen`, `paket_pruefen`, `nur_quellbaum`, `_befunde_ausgeben`, `_widerspruch_pruefen`,
  Selbsttest-Harness (`_fall_laufen`, `_innere_proben`, `_MANIFEST_PROBEN` 69, `_QUELLPFAD_PROBEN` 5, `selbsttest`, `main`),
  Mutanten-Werkzeug der R2-Kampagne (`nachbesserung_r2_belege/mutanten_teil.py`: 449 Mutanten auf Gate v6, 09-30 03:23 -
  also VOR N1/NB: `manifest_lesen`, `_pfad_fehler`, `quellpfade_pruefen` und der v2-Teil von `manifest_pruefen`/`pruefen`
  liefen nie durch eine systematische Kampagne, nur G1-G22 (N1) und 11 Hand-Mutanten (NB) von Hand).

## 0.2 Hypothesen (werden unten gemessen)

- Y1 Selbsttest-Deckung: systematische Ein-Zeilen-Mutanten (Klassen der R2-Kampagne) in `_pfad_fehler`, `manifest_lesen`,
  `manifest_pruefen`, `quellpfade_pruefen`, `quelldateien`, `pruefen`, `_befunde_ausgeben`, `_widerspruch_pruefen`
  ueberleben Selbsttest 258 + 132. Der Pin schuetzt NICHT gegen einen Fehler, den jemand bei einer gewollten
  Gate-Aenderung einbaut und dann (wie vorgeschrieben) neu pinnt - dann ist der Selbsttest das einzige Netz.
- Y2 Urteil: `gate_urteil` laesst sich mit einer stimmigen Ausgabe erfuellen, obwohl das Gate etwas fand, das die
  Zaehlung nicht aendert; oder Zeilen aus Pfadnamen (Zeilenumbruch in einem APK-Namen) mischen sich ins Urteil.
- Y3 Umgebung: `RE15_PYTHON`, `RE15_APK_SIGNER_SHA256`, `PYTHONPATH`/`sitecustomize` steuern Interpreter/Signer/Module
  des Gates (Grenze, sofern angezeigt/dokumentiert).
- Y4 Format v2: Gate == Geraete-Leser auch unter der neuen ASCII-/Segmentregel; Regel-Luecke: ein ORDNER-Segment
  `X.neu` neben einer Datei `X` besteht Gradle, Gate und Leser, aber der Entpacker kann `X` danach nie mehr schreiben
  (seine Zwischendatei `X.neu` ist ein Ordner -> EISDIR) - Geraet dauerhaft fail closed, Gate gruen (Klasse U3).
- Y5 Waisen (neuer Loesch-Code): Loeschen ausserhalb der Baeume, gelisteter Dateien, ueber lange Namen/Tiefe/Symlinks/
  Gross-klein; unlesbare Ordner nur Warnung.
- Y6 Fehlerpfade: verschluckte Fehler (`unlink` der weg-Pfade ohne Pruefung/Meldung), `fehler_halten` ohne Renderer,
  ein Weg zurueck nach main() ohne vollstaendigen Baum.
- Y7 Kette: veraltete Saetze in allen Varianten (auch der PC-Satz der ANDEREN Plattform: "frueherer Lauf, nicht neu
  geprueft" - Asymmetrie zu B1), fremde Namen (Gross/klein, Zusatzendungen), APK getauscht/geloescht, sha256 im Split-Satz.
- Y8 SHA-256 gegen Testvektoren und grosse Dateien (Stand HEAD).
- Y9 Abbruch + Neustart mit dem neuen Waisen-Schritt (halbe Liste, .neu-Reste).

## 0.3 Laufprotokoll

- 12:00 Python-Schnappschuss 0 (`py_zustand.ps1` der R4-Belege, Aufruf `powershell -ExecutionPolicy Bypass -File`, nur fuer
  diesen Prozess - die Systemrichtlinie blockt Skripte): `build/r34a/pruefer_u2/py_zustand_0_start.txt` (121 Zeilen) ==
  Endstand der Nachbesserung (`android_r4_nachbesserung_belege/py_zustand_ende.txt`, diff leer). Python nur `/c/Python310/python`.
- 12:05 Grundlauf: echtes Gate (Kopie `gate_echt.py`, sha256 8a0e3f15... = Pin) `--selbsttest` 24 s, `SELBSTTEST-OK 258/258`,
  `gate_urteil` (u2_urteil.sh: gate_urteil aus release/apk_pruefen.sh, unveraendert geladen) -> Urteil 0.

## 3. Format v2: Korpus Gate == Geraete-Leser (Y4)

Werkzeug `u2_korpus.py` = Runde-1-Korpus (`u1_korpus.py`, 57 Listen, unveraendert importiert) + 48 neue Listen; Geraete-Leser =
`u1_liste_korpus.c` (R4-1, unveraendert) mit asset_abgleich.c HEAD uebersetzt (gcc 16.2, `-std=c11 -Wall -Wextra -Wpedantic
-Wshadow -Wconversion`, ohne Warnung); Gate = `manifest_lesen` aus gate_echt.py. Beleg `korpus_ergebnis.txt`.

- **Gate == Geraet auf allen 105 Listen** (0 Abweichungen). Neu u.a.: ASCII-Grenzen 0x20/0x7e (A) / 0x7f/0x80/0xff/UTF-8/
  Kelvin allein/Tab am Ende (V); Segment 251 (A) / 252 als Datei, Ordner, erstes Segment (V); Pfad 512 (A) / 513 (V);
  `.neu`/`.NEU`/`.nEu` (V), `x.neu ` / `x.neu.` (A); Dubletten `Z`/`z` und exakt doppelt (Kopf zaehlt ein- bzw. zweimal) (V),
  `[`/`{`, `@`/`` ` `` (A, keine Buchstaben); Kopf mit CR vorn, Leerzeichen/Tab hinten, 19 Ziffern in Anzahl ODER Bytes,
  CR innen (V); Zeile mit CR vorn, LF-CR (V); Groesse 0 (A), 19 Ziffern (V). Die fuenf Unicode-Paare der Runde 1
  (Kelvin/K, Kelvin/k, Ae/ae, NFC/NFD, sz/ss) lehnen jetzt BEIDE ab (`unzulaessiger Pfad`) - H5 an dieser Stelle behoben.
  (Drei Runde-1-Zeilen mit Nicht-ASCII-Pfad, dort "soll A", sind unter der neuen Regel richtig V.)
- **Regel-Luecke Y4 bestaetigt (Regelebene):** `n_Y4_datei_und_neu_ordner` (`shared_assets/PSX/X` + `shared_assets/PSX/X.neu/B`)
  und `n_Y4_datei_und_NEU_ordner_gk` (`.../x` + `.../X.NEU/B`) nehmen Leser UND Gate an; Gradle ebenso (writeAssetManifest
  prueft `.neu` nur als Endung des GANZEN Pfads, build.gradle:157). Folge im Entpacker (android_glue.c, Code): die Zwischendatei
  fuer `X` ist `<s_root>/.../X.neu` (`pfad_bauen(tmp, ..., RE15_ABGLEICH_NEU_ENDUNG)` :511); nach der Erstinstallation ist das ein
  ORDNER (`X` < `X.neu/B` in strcmp-Reihenfolge: `X` wird zuerst geschrieben, danach legt `mkdirs_for` den Ordner `X.neu/` an).
  Jeder spaetere Lauf, der `X` neu schreiben muss (Update mit geaendertem `X`, oder Lauf "ohne Liste" mit abweichender Summe):
  `unlink(tmp)` :516 scheitert still (Ordner), `open(tmp, O_WRONLY|O_CREAT|O_TRUNC)` :233 -> EISDIR -> `n_fehler` ->
  `fehler_halten` - bei JEDEM Start, bis die App-Daten geloescht werden; `re15_abgleich_waisen` laesst `X.neu/` stehen (sein
  Inhalt ist gelistet). Gleiche Klasse wie U3 der Runde 1 (Gate gruen, Geraet kann die Liste nie umsetzen); heute ohne Folge
  (kein Asset-Ordner endet auf `.neu`), und NTFS haelt `X` + `X.neu\` nebeneinander - Befund F-Y4 (niedrig), Messung auf dem
  Geraet: Abschnitt 4 (sofern der Emulator frei wird).

## 4. Entpacker (Y5, Y6, Y8, Y9)

### 4.1 SHA-256 (Y8) - haltbar

Der SHA-Teil von asset_abgleich.c ist seit der Runde 1 unveraendert (`git diff 6b8e8cc8 HEAD`: keine Zeile in SHA-Code).
Trotzdem neu gemessen, gegen HEAD uebersetzt (Belege `sha_fips.txt`, `sha_zufall.txt`, Werkzeug `u2_sha_fips.c` + R4-1-Werkzeug
`u1_sha_zusatz.c`/`u1_sha_vergleich.py` unveraendert):
- FIPS-180-/NIST-Vektoren: leer, `abc`, 448 Bit (Stueck 1 und am Stueck), 896 Bit (Stuecke zu 7 B), 1 000 000 x `a` (je 1 B),
  RFC-6234-TEST4, **NIST-Langnachricht 64 B x 16777216 = 1 GiB** (`50e72a0e...`): **8/8 ok**.
- 200 Zufallsnachrichten mit NEUEM Seed 777001 (Laengen 2..1044923, 87 < 300 B, 57 >= 64 KiB, zufaellige Stueckelung inkl.
  0-B-Aufrufe), Stroeme **5 GiB + 1 B**, genau 4 GiB, 4 GiB - 1 B (Byte- und Bitzaehler ueber 2^32), `re15_sha256_datei` auf NB1.apk
  (363483287 B, `d6921014...`): **0 falsch** gegen Python hashlib.

### 4.2 Waisen-Loeschen unter echtem POSIX (Y5, Y9) - haltbar

Der Projekt-Unit-Test laeuft nur unter mingw - dort ist `re15_lstat` = `stat` und `RE15_IST_LINK` = 0 (asset_abgleich.c:23-31):
die Symlink-Regel des NEUEN Loesch-Codes lief bis hier nie. `u2_waisen_test.c` (asset_abgleich.c HEAD unveraendert, sha256
7699ee31...) im Linux-Container `re15-linux-build:deb11` (gcc 10.2, Test-Ordner im Container-/tmp, nicht auf dem 9p-Mount),
einmal als uid 1000, einmal als root (Belege `waisen_linux_1000_1000.txt` 19/19 ok, `waisen_linux_0_0.txt` 17/17 ok, 2 Rechte-Faelle
als root nicht aussagekraeftig):
| Fall | Ergebnis |
|---|---|
| W1 `shared_assets` selbst ist Symlink nach aussen | Ziel unberuehrt, Link bleibt ("kein Baum", 0 geloescht) |
| W2 Unterordner `PSX` ist Symlink nach aussen | nur der Link geloescht, Ziel unberuehrt |
| W3 Datei-Symlinks | gelisteter Name bleibt (Entpacker: stat folgt, rename ersetzt den Link), ungelisteter Link weg, Ziel bleibt |
| W4 Tiefe 70 (Grenze 64) | Waise in Tiefe 63 weg, in Tiefe 70 bleibt, Fehler gezaehlt |
| W5 Pfade 3671 B / 4877 B (Grenze 4096) | kurzer Pfad geloescht, langer bleibt, Fehler gezaehlt - kein Loeschen eines abgeschnittenen Pfads |
| W6 Ordner mit dem Namen einer gelisteten Datei / Datei mit dem Namen eines gelisteten Ordners | beide weg (Entpacker kann schreiben) |
| W7 Ordner 0000 / Ordner 0555 (uid 1000) | Inhalt bleibt, als Fehler gezaehlt bzw. `melde(ok=0)` - Warnung, kein Abbruch (so dokumentiert) |
| W8 `.neu`-Rest, `a.bin` neben gelistetem `A.BIN` (Linux: 2 Dateien), Name `\xff\xfe.bin`, `.nomedia`, FIFO | alle weg, gelistete Datei bleibt; zweiter Lauf (= Neustart nach Abbruch) 0/0 |
| W9 Wurzel mit `/` am Ende, Spielstand + Listen in der Wurzel, `shared_assets` fehlt | Wurzel-Dateien unberuehrt, Waise in `synchro` weg, kein Fehler |
Ausserhalb von `<s_root>/{shared_assets,synchro}` wird nichts geloescht; die Engine schreibt in beide Baeume nichts (grep aller
`fopen(...,"w"/"a"...)`/`SDL_RWFromFile(...,"w")`: nur Logs im Arbeitsverzeichnis, Pfade aus Umgebungsvariablen, re15_card.mcr).

## 1. Selbsttest gegen NEUE Ein-Zeilen-Mutanten (Y1)

Werkzeug `u2_mutanten.py`: die Klassen der R2-Kampagne (`mutanten_teil_r2.py` = `nachbesserung_r2_belege/mutanten_teil.py`,
unveraendert importiert: A befund->pass, B raise->pass, E Vergleich eine Richtung/Grenze, F Teilbedingung weg, G Tupelpaar weg,
H `not` weg, I Zahl +-1), gerichtet auf den seit R2 neuen/geaenderten Pruefcode (`_pfad_fehler`, `manifest_lesen`,
`manifest_pruefen`, `quellpfade_pruefen`, `quelldateien`, `pruefen`, `paket_pruefen`, `nur_quellbaum`, `_widerspruch_pruefen`,
`_befunde_ausgeben`, `_manifest_grenze`, `quellbaum_pruefen`, `wurzel_pruefen`, `_tuer_zeilen_drucken`), dazu drei Klassen, die
R2 nicht erzeugt (A2 `fehler.append(...)`->pass - manifest_lesen meldet NICHT ueber befund; C2 Regel-`return` in `_pfad_fehler`->pass;
K `continue`->pass) und 29 Hand-Mutanten D01-D29 (`u2_hand.py`: Konstanten auf Modulebene, strip/rstrip/split/fullmatch-Varianten).
Je Mutant `--selbsttest` im Schnellmodus; Ueberlebender = Rueckgabe 0 UND Schlusszeile SELBSTTEST-OK; auf jedem Ueberlebenden danach
`gate_urteil` (apk_pruefen.sh) = die Pruefung, die make_package/build_android vor jeder Nutzung eines NEU GEPINNTEN Gates machen.
Belege `mut_k1_ergebnis.tsv`, `mut_k1_urteile.txt`, `mut_k1_ueberlebende_diff.txt`. (Erster Urteilslauf rief ueber Python
`bash` = WSL-Starter auf; Urteile danach mit `u2_urteil_nach.sh` ueber Git-Bash nachgerechnet.)

**201 Mutanten, 183 erkannt, 18 ueberleben - alle 18 mit Urteil 0** (`SELBSTTEST-OK 258/258, jede Fallzeile [ok] ..., innere Proben
132/132`). Je Klasse ueberlebt: A 0/21, A2 0/16, B 0/3, C2 2/9, E 4/58, F 2/23, G 0/2, H 0/19, I 1/14, K 2/7, D 7/29.
Einordnung jedes Ueberlebenden (am Code, die Schwaechungen zusaetzlich am Korpus `korpus_mutanten.txt` gemessen):

| Mutant | Aenderung | Einordnung |
|---|---|---|
| I__pfad_fehler_Z1123_minus1, E__pfad_fehler_Z1123_ge | `c > 0x7f` -> `c > 126` / `c >= 0x7f` | aequivalent (0x7f faengt schon Zeile 1121) |
| F_manifest_lesen_Z1164_ohne0 | `not m and V1` -> `V1` | aequivalent (v1-Muster passt nie auf einen v2-Kopf) |
| F_manifest_lesen_Z1211_ohne1, E_manifest_lesen_Z1208_ge, K_Z1183, K_Z1204 | Zusatzmeldung / `>=` / continue->pass | aequivalent im Urteil (Fehler steht schon; Summe >= 2^63-1 mit 18-Ziffern-Kopf unerreichbar) |
| E_pruefen_Z1355_gt | `n != man_name` -> `n > man_name` | aequivalent (alle Baeume `assets/s...` > `assets/re15_assets.txt`) |
| C2__pfad_fehler_Z1118 | `return "leer"` weg | aequivalent (leerer Pfad scheitert an "ohne '/'") |
| C2__pfad_fehler_Z1127 | Backslash-Regel weg | im Leser schwaecher, in der Kette ausgeglichen (NTFS-Quelle kann kein `\` tragen -> "zusaetzlich in der APK") |
| D25_neu_je_segment, D28, D29 | `.neu` je Segment / Quellpfad mit `assets/` / `str.lower` | STRENGER bzw. aequivalent (D25 waere die Abhilfe zu F-Y4) |
| **E_manifest_pruefen_Z1247_gt** | `a_sha != m_sha` -> `a_sha > m_sha` | **Schwaechung**: falsche Listen-Summe nur noch in EINER Richtung gemeldet (Kette: Abschnitt 5, A3) |
| **D06_ASCII_KLEIN_ohne_Z** | Faltungstabelle ohne `Z` | **Schwaechung**: `.../Z.bin` + `.../z.bin` -> Gate A, Geraet V (`n_dub_Z_z`) |
| **D08_KOPF_RE_plus_anzahl** | Kopf-Anzahl `[0-9]+` statt `{1,18}` | **Schwaechung**: 19-stellige Anzahl -> Gate A, Geraet V (`n_kopf_anzahl_19_ziffern`) |
| **D13_kopf_strip_cr** | `kopf ... rstrip(b"\r")` -> `strip(b"\r")` | **Schwaechung**: CR VOR der Kopfzeile -> Gate A, Geraet V (`n_kopf_cr_vorn`) |
| **D15_zeile_strip_cr** | `z.rstrip(b"\r")` -> `z.strip(b"\r")` | **Schwaechung**: Zeile mit CR vorn / Zeilenende LF-CR -> Gate A, Geraet V (`n_zeile_cr_vorn`, `n_zeile_lf_cr`, `zeilenende_lf_cr`) |

Fuenf nicht-aequivalente Ein-Zeilen-Mutanten im v2-Teil bestehen Selbsttest UND Urteil. Alle fuenf machen das Gate NACHSICHTIGER als
den Geraete-Leser (Klasse V1/V2 der Runde 1): eine APK geht gruen durch die Kette, das Geraet verwirft Liste bzw. Datei und startet
nicht (fail closed, mit Meldung). Keiner laesst FALSCHEN INHALT auf das Geraet: der schaedliche Fall (Tuerarchiv falsch, Liste
passend) haengt am Quellbaum-Vergleich in `pruefen`, und dort ueberlebte kein Mutant (E_pruefen 1/23, aequivalent). Die Proben
`_MANIFEST_PROBEN`/Faelle haben: CR vor Kopf- bzw. Datenzeile (nur CR HINTER), 19 Ziffern nur im Bytes-Feld des Kopfs, die
Gross/klein-Dublette nur als `B`/`b`, falsche Listen-Summe nur in einer Richtung (Fall 228 `0123456789abcdef..`, Fall 248 `0000..` - beide kleiner als jede echte Summe; fuer den Quellbaum- und Paketvergleich gibt es dagegen beide Richtungen: Faelle 167/168, 185/186).
**Befund F-Y1 (niedrig)** - dieselbe Klasse wie V1/V2 der Runde 1, dort als niedrig gefuehrt und mit 16 Proben + 10 Faellen behoben.

### 4.3 Der ECHTE Entpacker auf einem Linux-Pruefstand (Y4, Y6, Y9)

Der Emulator gehoerte beim Start dem Pruefer ECHTER LAUF (`emulator-5586`). Statt ihn zu teilen: android_glue.c + asset_abgleich.c
HEAD UNVERAENDERT (sha256 0e295345... / 7699ee31...) mit Attrappen fuer SDL (keine Anzeige, `SDL_PollEvent` liefert beim 3. Aufruf
SDL_QUIT = "App geschlossen"), JNI (Activity.getAssets), AAssetManager (liest die "APK-Assets" aus einem Ordner) und android/log
im Linux-Container uebersetzt (gcc 10.2, uid 1000; Werkzeug `pruefstand/harness_main.c`, `pruefstand/stub/`, `pruefstand/szenarien.sh`,
Beleg `pruefstand_1000.txt`). Der Speicher ist dort case-SENSITIV (ext4/overlay) - fuer diese Faelle ohne Belang.
| Szenario | EXIT | Ergebnis |
|---|---|---|
| S0 frisch / S1 Neustart / S2 Update mit gleich grosser Aenderung (N1a) | 0/0/0 | `ohne Liste` 3 kopiert; `schneller Weg`; `Update ... geaendert 1` -> b.bin neu (bbbb2) |
| **S3 Y4**: `PSX/X` + `PSX/X.neu/B` frisch, dann Update mit geaendertem X | 0 / **1 / 1 / 1** | frisch ok (X zuerst, dann Ordner X.neu/); Update: `FEHLER beim Entpacken: .../PSX/X.neu nicht anlegbar: Is a directory` -> `ABBRUCH: 1 DATEIEN KONNTEN NICHT ENTPACKT WERDEN`; JEDER weitere Start (`ohne Liste`, Summe weicht ab -> neu) scheitert gleich - **dauerhaft**, bis die App-Daten geloescht werden |
| **S4 verschluckt**: Update streicht `ALT/w.bin`, Ordner `ALT/` nur lesbar | **0 / 0** | `Abgleich (Update) ... weg 1`, dann `0 entfernt ... 0 Fehler`, SPIELSTART, Liste geschrieben; `ALT/w.bin` bleibt, KEINE Warnung/Zeile; Neustart `schneller Weg` - die Datei bleibt fuer immer (kein Waisen-Lauf mit gueltiger Liste) |
| S5 kein AssetManager (JNI) | 1 | `ABBRUCH: FEHLER: KEIN ZUGRIFF AUF DIE APK-ASSETS`, Meldung gezeichnet, Ende erst mit SDL_QUIT - fail closed mit Anzeige |
| S6 kein AssetManager UND kein Renderer | 1 | fail closed, aber 0 Zeichenaufrufe: schwarzer Schirm bis zum Schliessen (Grenze: ohne Renderer laeuft das Spiel ohnehin nicht) |
| S7 Abbruch mitten im Entpacken (`_exit` im AAsset_read), Neustart | 9 / 0 | `.neu`-Rest, keine Liste; Neustart: `Waise entfernt ... a.bin.neu`, alles kopiert, 0 `.neu`, Inhalt = APK |
| S9 Liste v1 / S10 Datei fehlt in der APK / S11 falsche Listen-Summe | 1 / 1 / 1,1 | je `ABBRUCH` mit Meldung; S11 bei JEDEM Start (Folge von A3 in Abschnitt 5) |
Bestaetigt: F-Y4 am echten Code (dauerhaft fail closed). **Neu F-Y6 (niedrig):** das `unlink` der weg-Pfade (android_glue.c:499-506,
`if (unlink(dst) == 0) {...}` ohne else) verschluckt jeden Fehler ausser dem Erfolg - kein Zaehler, keine Zeile, "0 Fehler", die Liste
wird geschrieben und die Datei nie mehr angefasst. Gleiche Klasse wie U1 der Runde 1 (dort fuer `unlink(pf_liste)` behoben: "ausser
ENOENT -> Fehler"), und gegenlaeufig zu `re15_abgleich_waisen`, das nicht loeschbare Waisen wenigstens als WARNUNG meldet. Folge nur,
wenn die Engine eine gestrichene Datei noch oeffnet - z.B. die Stimmen: audio_pc.c `re15_voice_load_clip` baut `synchro/STAGE%u/room%04X/main%02d.wav` aus dem Raum und spielt, was DA ist; eine gestrichene Aufnahme bliebe hoerbar. Auf dem Geraet
schwer herbeizufuehren (App-eigener Speicher); am Pruefstand eindeutig.
(Randnotiz ohne Befund: gcc meldet in draw_progress :313 `-Wmisleading-indentation` fuer `if (frac < 0) frac = 0; if (frac > 1) frac = 1;` - Verhalten richtig.)

## 2. Pin und Urteil (Y2, Y3)

- **Pin-Datei** (`kette_P.txt`, `gate_pin_pruefen` aus der Sandbox-Kopie von apk_pruefen.sh, echtes Gate): Grossbuchstaben + CRLF +
  Kommentare werden angenommen (Normalisierung wie dokumentiert); alter + neuer Wert je Zeile ("Nachtragen statt Ersetzen"), die ganze
  `sha256sum`-Zeile (`<wert> *release/...`), Wert zweimal, leer, nur als Kommentar, Praefix `sha256:` -> je Abbruch "kein SHA-256";
  Wert des Gates von be8b60f3 -> "NICHT das festgehaltene". Haltbar.
- **apk_pruefen.sh abgeschnitten** (`kette_T_abgeschnitten.txt`, --gate-only NB1): 0 B -> EXIT 127 (`apk_werkzeuge_finden: command not
  found`), mitten in gate_urteil -> EXIT 2 (`syntax error: unexpected end of file`), vor gate_laufen -> EXIT 127. Fail closed.
- **Urteil = einzige Instanz** (A4 in Abschnitt 5): `gate_laufen` gibt NUR das Urteil zurueck; die Rueckgabe des Gates geht als
  Eingabe in `gate_urteil` ein, aber nicht als eigene UND-Bedingung. Eine 1-Zeichen-Aenderung im (weder gepinnten noch zur Laufzeit
  selbstgeprueften) Urteilscode - `ende(1, "das Gate meldet ...` -> `ende(0, ...` - macht aus dem richtigen `APK-ASSET-GATE-ABWEICHUNG`
  (Rueckgabe 1) des ECHTEN, gepinnten Gates ein `ANDROID-GATES-OK` fuer das falsche Tuerarchiv; im selben Log steht "das Gate meldet
  ABWEICHUNG" - dasselbe Bild wie H1 der Runde 1, nur eine Stufe weiter. Eine nachsichtige Regression im Urteil ist STILL (jeder gute
  Lauf bleibt gruen), und das Urteil MUSS bei jeder Wortlaut-Aenderung des Gates mitgezogen werden (android_r4_nachbesserung.md 8).
  Der Urteilstest der Nachbesserung (`nb_urteil_test.sh`) laeuft in keiner Kette und keinem ctest. Abschneiden faellt (oben), eine
  Logik-Aenderung nicht. **Befund F-Y2 (niedrig)**; Abhilfe-Richtung: in `gate_laufen` zusaetzlich `rc == 0` verlangen (ein richtiges
  Gate kann dann nicht mehr ueberstimmt werden) und/oder den Urteilstest mit festen Logs vor jeder Nutzung laufen lassen bzw. apk_pruefen.sh
  mitpinnen.
- Umgebung (Y3), gelesen: `RE15_PYTHON` (python_finden.sh), `RE15_APK_SIGNER_SHA256`, `APK_BUILD_TOOLS`, `JAVA_HOME`, `ANDROID_SDK_ROOT`
  steuern Interpreter/Signer/Werkzeuge - jeweils dokumentiert und im Log angezeigt (`Python: ...`, `erwarteter Signer: ... (Quelle)`,
  `APK-Werkzeuge: ...`); das Gate laeuft ohne `-I`/`-E`, `PYTHONPATH` wirkt also. Grenze (bewusste Bedienung), kein Befund.

## 5. Kette (Y7) in eigener Sandbox

Sandbox `build/r34a/pruefer_u2/sb` (R4-Werkzeug `r4_sandbox_anlegen.sh`, Quellbaum per Hardlink, eigenes git-Repo mit Commit
2026-09-29 12:00; Skripte + Pin vor jedem Teil = Arbeitsbaum per `cmp`). Werkzeug `u2_kette.sh`, Faelschungen `u2_faelschen.sh` aus
NB1.apk (v0.8.20-nb1, Liste v2, d6921014...; zipalign + derselbe Debug-Schluessel, verify rc 0, Signer 432bc749...):
FS_sig (c9b5785e...: Liste - sha256 von RE15DOOR/P07G.DO2 -> `f` x 64, Daten unveraendert), FD_sig (6bdf017a...: P07G.DO2 1 Byte
gekippt + passende Listen-Summe). Beleg `kette_A.txt`.

### 5.1 build_android.sh --gate-only

| Lauf | Gate / Urteil | APK | EXIT | Ergebnis |
|---|---|---|---|---|
| A0 | echt, Pin echt | NB1 | 0 | 258/258, `APK-ASSET-GATE-OK`, `ANDROID-GATES-OK` (Kontrolle) |
| A1 | echt | FS_sig | 1 | `Manifest-Pruefsumme falsch: .../P07G.DO2 Manifest ffff.., APK 68306346..` |
| A2 | echt | FD_sig | 1 | `Inhalt weicht ab (sha256, gleiche Groesse 55908 B)` |
| **A3** | **Ueberlebender E_manifest_pruefen_Z1247_gt, NEU GEPINNT** | FS_sig | **0** | Selbsttest-Urteil 0 (258/258), APK-Urteil 0, **`ANDROID-GATES-OK`** - das Geraet verwirft P07G.DO2 bei JEDEM Start (Pruefstand S11) |
| A3b | derselbe Mutant | FD_sig | 1 | der schaedliche Fall faellt weiter (Quellbaum-Vergleich) |
| **A4** | echt + Pin echt, **Urteil 1 Zeichen geaendert** | FD_sig | **0** | Gate: `APK-ASSET-GATE-ABWEICHUNG`, Rueckgabe 1; Urteil 0 -> `APK-PRUEFUNG-OK`, `ANDROID-GATES-OK` fuer das falsche Tuerarchiv |
Danach Sandbox-Gate + Pin + apk_pruefen.sh = Arbeitsbaum, 0 Pruefkopien, 0 Temp-Reste.

Pruefstand Teil 2 (`pruefstand/szenarien2.sh`, Beleg `pruefstand2_1000.txt`):
| Szenario | EXIT | Ergebnis |
|---|---|---|
| H1 Datei auf dem Geraet von aussen gleich gross veraendert, Neustart | 0 | `schneller Weg`, Datei bleibt veraendert - so dokumentiert ("pruefen nur die Dateigroessen"); kein APK-Update betroffen (jede Aenderung aendert die Liste) - Grenze |
| H4 "zuletzt entpackt"-Liste auf 40 B abgeschnitten | 0 | `unlesbar ... -> jede Datei pruefen`, Waise entfernt, 2 per SHA-256 geprueft - konsistent |
| H7 Update: Datei `PSX/q` wird Ordner `PSX/q/c` | 0 | `entfernt ... PSX/q`, `entpacke PSX/q/c` - konsistent |
| H8 Update: Ordner `PSX/q/` (mit `c`) wird Datei `PSX/q` | **1**, dann 0 | weg-Schleife loescht `q/c`, der leere Ordner `q/` bleibt (im Update-Weg kein rmdir, kein Waisen-Lauf) -> `rename .../PSX/q: Is a directory` -> ABBRUCH mit Meldung; erst der NAECHSTE Start (ohne Liste -> Waisen-Lauf raeumt `q/` ab) entpackt. Niedrig (fail closed, heilt beim zweiten Start) - Teil von F-Y4 (Ordner/Datei-Namenskonflikte im Update-Weg) |
| H9 Update `X.BIN` -> `x.bin` (case-sensitiver Speicher) | 0 | `entfernt X.BIN`, `entpacke x.bin` - konsistent |
