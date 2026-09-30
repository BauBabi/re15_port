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
