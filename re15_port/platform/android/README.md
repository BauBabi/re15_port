# RE1.5 Port — Android-Paket

Die Android-Fassung ist derselbe Port wie Windows/Linux: `engine/src/*.c` plus die
PC-Plattformquellen (`platform/pc/src/*.c`, `platform/pc/main.c`) werden mit dem NDK zu
`libmain.so` uebersetzt und von SDL2s `SDLActivity` (SDL 2.28.5, dieselbe Version wie der
PC-Bau) gestartet. Alle Android-Besonderheiten liegen in `#if defined(__ANDROID__)`-Bloecken
bzw. in `platform/android/jni/android_glue.c`; Windows- und Linux-Bau sind unveraendert.

Bedient wird das Spiel ueber ein halbtransparentes **On-Screen-Pad** (Multitouch), das die
gleichen PSX-Pad-Bits liefert wie Tastatur und Gamepad. Ein per USB/Bluetooth angeschlossener
Controller funktioniert daneben wie auf dem PC (SDL-GameController).

## Bauen

Voraussetzungen auf dem Bau-Rechner: JDK 17, ein Android-SDK-Ordner (wird ergaenzt),
Internet (SDL2-Tarball, Gradle-Plugins, NDK) und **Python >= 3.8** fuer die Asset-Pruefung.
**Kein Android Studio noetig.** Den Interpreter sucht `release/python_finden.sh`: ein echtes Python
aus dem PATH (je Ordner erst `python3`, dann `python`) bzw. aus `RE15_PYTHON`, unter Windows
zusaetzlich `C:/Python3*`. Der WindowsApps-Alias `python3`, der unter Git-Bash sonst zuerst kommt
(und in v0.8.17 ungefragt Python 3.14 installiert hat), wird am Pfad erkannt und nie gestartet.
Fehlt Python, bricht der Bau **vor** Gradle ab. Diagnose: `bash release/python_finden.sh`.

```bash
release/build_android.sh                    # Version aus git describe --tags
release/build_android.sh --version v0.8.5   # feste Version
release/build_android.sh --gate-only <apk> --version v0.8.19   # nur die Pruefungen, kein Bau
```

Das Skript

1. sucht Python (`python_finden.sh`) und die Pruefwerkzeuge (`aapt`, `zipalign`,
   `lib/apksigner.jar` aus `build-tools;35.0.0`, Java) — fehlt eines, Abbruch vor Gradle,
2. installiert per `sdkmanager` in den vorhandenen SDK-Ordner nach, was fehlt
   (cmdline-tools 13114758, `ndk;27.2.12479018`, `cmake;3.22.1`, `platforms;android-35`,
   `build-tools;35.0.0`) und bestaetigt die Lizenzen,
3. entfernt eine vorige `release/re15_port_<version>_android.apk` (scheitert der Bau, liegt keine
   alte APK unter dem Namen) und den CMake-Cache `app/.cxx` (neue `*.c` fehlten sonst still),
   dann `./gradlew -Pre15Version=<v> fetchSdl2 assembleRelease` in `platform/android/`,
4. prueft die Gradle-Ausgabe mit `release/apk_pruefen.sh` — jeder Schritt an EINER privaten
   Kopie der APK, jeder Befund bricht ab:
   * Stichproben: beide ABIs (`libmain.so`, `libSDL2.so`), `re15_assets.txt`, je ein Asset je Baum,
   * `aapt dump badging`: Paket `de.re15.port`, `versionName` = Version, `arm64-v8a` + `x86_64`,
   * `zipalign -c -P 16 4`: Ausrichtung (Android 11+ installiert sonst nicht),
   * `apksigner verify`: gueltige v2/v3-Signatur, genau ein Signer, und zwar der aus
     `release/apk_signer.sha256` (Signer-Pin, siehe unten),
   * das Asset-Gate laeuft nur als private Kopie, deren sha256 in `release/apk_asset_gate.sha256`
     steht (Gate-Pin, siehe unten), und jedes Urteil kommt aus Rueckgabe UND Ausgabe: die letzte Zeile
     muss die Schlusszeile sein, die Zaehlzeilen muessen passen, beim Selbsttest jede Fallzeile
     `[ok]` mit `rc = soll` (Nachbesserung R4-1 - vorher gab ein leeres Gate Rueckgabe 0, und alles
     ging durch); `APK_GATE_DATEI` aus der Umgebung wird ignoriert,
   * `release/apk_asset_gate.py --selbsttest`: das Gate muss seine guten Mini-APKs annehmen und
     jede Faelschung ablehnen, sonst wird ihm nicht getraut,
   * die volle Asset-Pruefung: JEDE Datei der fuenf Asset-Baeume liegt mit gleicher Groesse und
     sha256 in der APK, unter `assets/` liegt nichts sonst, `re15_assets.txt` stimmt Zeile fuer
     Zeile (danach entpackt die App), jeder Eintrag ist so lesbar wie fuer Androids ZIP-Leser;
     dazu das Tuer-Soll aus den Engine-Tabellen (jedes Port-Tuerarchiv in `RE15DOOR`, jedes
     verlangte `RE2/DOOR/DOORxx.DO2` mit Groesse und Aufbau),
   * am Ende: die Kopie ist unveraendert und unter dem Pfad liegen noch dieselben Bytes,
5. legt erst danach genau die gepruefte Kopie als `release/re15_port_<version>_android.apk` ab
   und schreibt `release/SHA256SUMS_android.txt`. Scheitert ein Schritt, liegt keine APK unter
   diesem Namen (die Gradle-Ausgabe bleibt zur Diagnose unter `app/build/outputs/apk/`).

`release/make_package.sh` prueft eine vorhandene APK vor dem Zippen noch einmal mit derselben
Kette (dazu: neuer als der letzte Commit an `engine`, `include`, `platform/pc`,
`platform/android`), bringt sie in den Split-Satz und entpackt sie daraus wieder: ihre sha256
muss die der geprueften Kopie sein. Ein Android-Satz kommt nur in `SHA256SUMS.txt` und in git,
wenn er in DIESEM Lauf so entstanden ist. Liegt ohne APK ein Satz derselben Version da, bricht
make_package ab; `--ohne-android` schnuert nur die PC-Saetze und entfernt den alten Satz.
`SHA256SUMS.txt` und `git add` nehmen nur eine Positivliste (Saetze dieses Laufs, bewusst der Satz der
anderen PC-Plattform derselben Version, Android nur aus diesem Lauf); jede andere Datei
`re15_port_<version>_*.z*` (etwa `..._ANDROID.zip`) bricht ab.

Gradle-Seite (`app/build.gradle`):

* `fetchSdl2` laedt `SDL2-2.28.5.tar.gz` sha256-geprueft nach `platform/android/_deps/`
  (gitignoriert). Daraus kommen `libSDL2.so` (CMake, `jni/CMakeLists.txt`) **und** die
  Java-Klassen `org.libsdl.app.*`.
* `stageAssets`/`writeAssetManifest` spiegeln die fuenf Asset-Baeume `shared_assets/PSX`,
  `shared_assets/extracted_fx`, `shared_assets/RE2`, `shared_assets/RE15DOOR` (Port-Tuerarchive,
  seit Runde 33) und `synchro/STAGE*` nach `app/build/re15_assets/` und schreiben
  `re15_assets.txt` im **Format v2** (Runde 34a N1): Kopfzeile `# re15 assets v2 <anzahl> <bytes>`,
  je Datei `<bytes>\t<sha256>\t<pfad>`, nach Pfad sortiert. Die Regeln (fail closed) stehen in
  `jni/asset_abgleich.h`; `release/apk_asset_gate.py` liest die Liste in der APK nach denselben
  Regeln und prueft jede Summe gegen die Daten der APK (die Liste v1 bis v0.8.19 wird abgelehnt).
  **Asset-Pfade: nur druckbares ASCII** (0x20-0x7e), jedes Segment hoechstens 251 Bytes, keine zwei
  Pfade, die sich nur in Gross/klein unterscheiden (Nachbesserung R4-1): der App-Speicher des Geraets
  faltet Unicode-Gross/klein und -Normalform (Kelvin-Zeichen und `K`, `ss` und `sz`-Ligatur werden EINE
  Datei, gemessen im Emulator API 36), und laengere Namen legt er nicht an. Gradle, Gate und Geraet
  lehnen solche Pfade ab; `apk_asset_gate.py --quellbaum` meldet sie schon im Quellbaum.
  Dieselbe Baum-Liste steht in
  `release/apk_asset_gate.py` (`BAEUME`, wird gegen `stageAssets` geprueft - weicht sie ab, bricht
  die Pruefung ab) und in `release/make_package.sh` (`copy_common`, jedes PC-Paket wird per
  `apk_asset_gate.py --paket` gegen die Liste geprueft).
* `noCompress` enthaelt jede Endung der Asset-Baeume: die APK speichert die Assets
  **unkomprimiert** (~365 MB), das Entpacken auf dem Geraet ist damit ein reines Kopieren.
* Signiert wird standardmaessig mit dem Android-Debug-Schluessel (`~/.android/debug.keystore`),
  das reicht fuer Sideload. Eigener Schluessel: `RE15_KEYSTORE`, `RE15_KEYSTORE_PASS`,
  `RE15_KEY_ALIAS`, `RE15_KEY_PASS` in der Umgebung (gesetzt, aber die Datei fehlt: Gradle
  bricht ab, kein stiller Rueckfall auf den Debug-Schluessel).
* **Signer-Pin:** die Pruefkette verlangt genau den Signer aus `release/apk_signer.sha256`
  (SHA-256 des Signer-Zertifikats; Stand v0.8.19 der Debug-Schluessel der Bau-Maschine,
  `432bc749...`). Eine anders signierte APK installiert sich nicht als Update ueber die vorige,
  und wer deshalb deinstalliert, verliert die Spielstaende. **Wer mit eigenem Schluessel (oder auf
  einer anderen Maschine mit anderem `debug.keystore`) baut**, bekommt deshalb den Abbruch
  `apksigner: Signer-Zertifikat ..., erwartet 432bc749...`. Ist der neue Schluessel Absicht: den
  Wert aus `java -jar <build-tools>/lib/apksigner.jar verify --print-certs <apk>` (Zeile
  `Signer #1 certificate SHA-256 digest`) in `release/apk_signer.sha256` eintragen und committen,
  oder fuer einen einzelnen Lauf `RE15_APK_SIGNER_SHA256=<64 Hexziffern>` setzen.
* **Gate-Pin** (Nachbesserung R4-1): `release/apk_asset_gate.sha256` haelt die sha256 von
  `release/apk_asset_gate.py` fest; `build_android.sh` und `make_package.sh` fuehren nur ein Gate mit
  genau dieser Summe aus. **Wer das Gate aendert**, laesst danach `"$PY" release/apk_asset_gate.py
  --selbsttest` laufen (muss `SELBSTTEST-OK` melden), traegt den Wert von `sha256sum
  release/apk_asset_gate.py` in `release/apk_asset_gate.sha256` ein und committet beides zusammen.
  Werden Selbsttest-Faelle entfernt, bricht die Kette ab, bis die Mindestzahlen
  (`GATE_SELBSTTEST_MIN_FAELLE`/`_INNEN` in `release/apk_pruefen.sh`) bewusst gesenkt sind.
* **Urteil-Pin** (Runde 35, `analysis/befunde_runde35/N_android.md` Punkt 3): das Urteil ueber jeden
  Gate-Lauf steht in `release/gate_urteil.py` (vorher ungeprueft als Heredoc in apk_pruefen.sh),
  festgehalten in `release/gate_urteil.sha256` und vor jeder Nutzung selbstgeprueft (Stand
  Nachbesserung 3): 777 feste Gate-Ausgaben mit Soll-Urteil - 285 handgeschriebene (jede Vergleichsstelle
  von BEIDEN Seiten) und 492 ERZEUGTE Stoerungsfaelle (aus jeder guten Ausgabe: jede Zeile weg/doppelt,
  jede Zahl -1/+1, Zusatzspalte hinter jeder Zahl, zwei Zahlen derselben Spalte gegenlaeufig; Soll nach
  Regel "keine Aussage", Ausnahmen nur mit Grund in `UNGEPRUEFT`, deren Zahl (102) `apk_pruefen.sh` mit
  `GATE_URTEIL_MAX_UNGEPRUEFT` begrenzt) -
  und Mutanten der Funktion `urteil()` - Vergleiche (`==`/`!=` durch jeden anderen Vergleich, also auch
  die einseitigen Lockerungen `!=` -> `<`/`>` und `==` -> `<=`/`>=`; `<`/`<=`/`>`/`>=` Grenze und
  Richtung), and/or, not, if-Bedingungen, ganze Zahlen, weggelassene Aufrufe (auch die Pruef-Anweisung
  `tuer_soll()`), jede Zeichenkette ausserhalb der Meldungstexte (MARKE-Tabelle, Modusnamen,
  Pruef-Literale wie `[FEHLER]`) und jedes Regex-Muster mit sieben Lockerungen (Text dahinter erlaubt,
  Rest ab Stueck k -> `.*`, Wort -> `.*`, `\d+` -> `.*`, `\s+` -> `\s*`, Einzelzeichen weg, Alternative
  weg): 894 Mutanten, 887 von einem Fall erkannt, 7 im Code als gleichwertig begruendet. Strukturregel:
  kein Meldungstext ruft eine Funktion des Urteils (eine Pruefung dort waere fuer die Mutanten
  unsichtbar). NICHT mutiert werden die Meldungstexte, `main()` und `urteil_rufen()`; Aenderungen
  ausserhalb dieser Operatoren (z.B. `any` -> `all`, eine Variable durch einen anderen Ausdruck) faengt
  die Fallsammlung samt Stoerungsfaellen: jede Aenderung, nach der das Urteil eine bisher gepruefte
  Zeile, Zahl oder Spalte nicht mehr prueft, macht eine Stoerung zu "Urteil 0 (soll 2)" (gemessen: 32/32,
  29/29 und 31/32 simulierte Aenderungen erkannt - die eine ist gleichwertig -, `analysis/befunde_runde35/
  N_android_belege/nb2_nb1_aenderungen_neu.txt`, `nb2_urteil_aenderungen_nachher.txt`,
  `nb3_urteil_aenderungen_nachher.txt`). **Wer das Urteil aendert**, schreibt fuer die neue Regel einen
  Fall (sonst ueberlebt ein Mutant = `URTEIL-SELBSTTEST-FEHLER`) und traegt dann die neue sha256 ein;
  `GATE_URTEIL_MIN_*` nur, wenn Faelle bzw. Mutationsstellen dazugekommen sind. Eine Regel entfernen
  geht nur mit einer neuen begruendeten Ausnahme in `UNGEPRUEFT` UND einer hoeheren
  `GATE_URTEIL_MAX_UNGEPRUEFT` - zwei sichtbare Zeilen. Zusaetzlich gilt ein OK-Urteil nur mit
  Rueckgabe 0 des Gates (zweite Instanz in `gate_laufen`).
  Das **bash-Urteil** in `apk_pruefen.sh` (gate_pin_pruefen, gate_urteil_pin_pruefen,
  gate_urteil_selbsttest, gate_festhalten, gate_urteil, gate_laufen) hat seine Kontrollen in
  `re15_port/tests/unit/r35_android/urteil_kontrollen.sh` (102 Attrappen-Kontrollen, jede Vergleichsstelle
  von beiden Seiten, die UEBERGABE an Urteil und Gate woertlich - Modus, Rueckgabe, Mindestzahlen,
  unzip-Zaehlung, Ausgabe-Datei, `"$@"` -, unter `set -euo pipefail` wie die echten Aufrufer) und wird von
  ctest `unit_r35_android_bash_mutanten` selbst mutiert (`bash_urteil_mutanten.py`: Vergleiche in `(( ))`
  mit derselben Tabelle, `[[ ]]`, `grep -q`- und `=~`-Muster, `die` -> `true`, Zahlen, weggelassene
  Pruef-Aufrufe, `return`, und jedes uebergebene Argument -> `""`/`0`/ein anderes Argument desselben
  Aufrufs; 328 Mutanten, 325 erkannt, 3 begruendet gleichwertig). **Wer dort eine Pruefzeile oder eine
  Argumentliste aendert oder neu schreibt**, ergaenzt in `urteil_kontrollen.sh` eine Kontrolle, sonst
  ueberlebt ein Mutant. ctest `unit_r35_android_pruefkette` faehrt die ganze Kette (echtes Gate +
  Urteil + diese Kontrollen, dazu das echte Urteil an der Grenze der Mindestzahlen 261/148 und der
  unzip-Zaehlung) bei jedem Suite-Lauf.

Native Seite: `re15_port/CMakeLists.txt` mit `-DRE15_BUILD_PC=OFF -DRE15_BUILD_ANDROID=ON`
-> `platform/android/jni/CMakeLists.txt` (SDL2 shared + `libmain.so`). ABIs: `arm64-v8a`
(Geraete) und `x86_64` (Emulator).

## Aus dem Repo holen (Split-Zip)

Die APK ist rund 358 MB und liegt damit ueber GitHubs Dateigrenze. Im Repo liegt sie
deshalb als Split-Zip mit derselben Volume-Groesse wie die Windows- und Linux-Pakete:

```
release/re15_port_<version>_android.z01     (90 MB)
release/re15_port_<version>_android.zip     (letztes Volume, enthaelt den Katalog)
```

⛔ Die Volumes lassen sich NICHT einzeln entpacken und `unzip -t` bricht auf ihnen ab
(„At least one error"). Erst zusammenfuegen, dann entpacken — genau wie bei den anderen
Paketen:

```bash
cd release
zip -s 0 re15_port_<version>_android.zip --out joined.zip   # Volumes zusammenfuegen
unzip -t joined.zip                                          # sollte "No errors" sagen
unzip joined.zip                                             # -> re15_port_<version>_android.apk
sha256sum -c SHA256SUMS_android.txt                          # muss OK sagen
```

Geprueft 2026-09-19: die so wiederhergestellte APK ist bytegleich mit der gebauten
(sha256 identisch).

## Installieren

```bash
adb install -r release/re15_port_<version>_android.apk
```

oder die APK aufs Geraet kopieren und dort antippen (Installation aus unbekannten Quellen
erlauben). Mindestens Android 7.0 (API 24), Ziel Android 15 (API 35).

**Erster Start:** die Assets werden aus der APK in den App-Speicherordner entpackt
(Fortschrittsbalken, ~365 MB, je nach Geraet 10-60 s). Danach startet das Spiel wie am PC
(Capcom-Intro, Titel). Weitere Starts vergleichen die Liste der APK mit der zuletzt entpackten
(`re15_assets_entpackt.txt`) und pruefen nur die Dateigroessen - das Spiel startet sofort.

**Update:** nur Dateien, deren Groesse ODER sha256 sich gegenueber dem zuletzt entpackten Stand
geaendert hat (oder die fehlen), werden neu entpackt; Dateien, die nicht mehr in der Liste
stehen, werden geloescht (`adb logcat -s re15` nennt jede Datei). Jede Datei wird als
`<ziel>.neu` geschrieben und erst nach passender Groesse und sha256 umbenannt. Von v0.8.19
kommend (nur der alte Marker `re15_assets_ok.txt`) wird einmal jede vorhandene Datei per
sha256 geprueft ("ASSETS WERDEN EINMALIG GEPRUEFT"); ebenso nach einem abgebrochenen Entpacken.
In diesen beiden Faellen (keine gueltige "zuletzt entpackt"-Liste) werden vorher `shared_assets/`
und `synchro/` durchgegangen und alle Dateien, die nicht in der neuen Liste stehen, geloescht -
auch halbe `.neu`-Reste und Dateien, die ein Update nach einem Abbruch gestrichen hat
("Waise entfernt" im Log; Nachbesserung R4-1). Was sich nicht loeschen laesst, meldet `debug.log`
als Warnung.
Liegt einer Datei etwas mit falschem Typ im Weg - ein Ordner auf ihrem Namen, eine Datei auf einem
ihrer Ordnernamen oder ein Ordner auf `<ziel>.neu` (z.B. wenn ein Update eine Datei durch einen
gleichnamigen Ordner ersetzt oder umgekehrt) -, wird es im SELBEN Start entfernt
(`Konflikt geraeumt: ...` im Log; Runde 35). Sicher, weil Liste, Gate und Gradle keinen Pfad zulassen,
der zugleich Datei und Ordner ist, und keinen Ordner auf `.neu`.
**Fehler:** fehlt die Asset-Liste der APK, ist sie ungueltig, oder laesst sich eine Datei nicht
entpacken (z.B. Speicher voll), bleibt die Fehlermeldung stehen, bis die App geschlossen wird - das
Spiel startet dann NICHT mit einem alten oder halben Asset-Baum (bis Nachbesserung R4-1: 3 s Meldung,
danach Start). Ursache: `debug.log` im Speicherordner bzw. `adb logcat -s re15`; der naechste Start
prueft jede Datei neu.
Die Quelle ist ausschliesslich die APK (AAssetManager). Dokumentation:
`analysis/befunde_runde34_android/android_entpacker_n1.md`,
`analysis/befunde_runde34_android/android_r4_nachbesserung.md`.

Der App-Speicherordner ist der **exe-Anker** des Ports (so wie am PC das Verzeichnis neben
der exe): `/storage/emulated/0/Android/data/de.re15.port/files/` (per USB-Dateiuebertragung
oder `adb` einsehbar), sonst der interne `files`-Ordner der App. Dort liegen

| Datei | Inhalt |
|---|---|
| `debug.log` | das Laufzeitprotokoll (stderr des Ports) |
| `befund.log` | das Befund-Log (Marke ueber den F9-Knopf) |
| `re15_card.mcr` | die Memory-Card (Spielstaende) |
| `re2_ki.log` | RE2-KI-Trace (nur mit `RE15_RE2_TRACE`) |
| `shared_assets/`, `synchro/` | die entpackten Assets |
| `re15_assets_entpackt.txt` | Kopie der Asset-Liste nach vollstaendigem Entpacken (loeschen = beim naechsten Start jede Datei per sha256 pruefen) |

Auslesen z.B. mit `adb pull /sdcard/Android/data/de.re15.port/files/befund.log`.

## Bedienung (On-Screen-Pad)

Querformat, 4:3-Spielbild zentriert (Letterbox); die Knoepfe liegen teils in den schwarzen
Streifen. Layout in Fensterkoordinaten, u = Bildhoehe/12:

```
 [L1]            [F9]              [R1]
                                   ( /\ )
   ^                          ( [] )    ( O )
 < + >                             ( X )
   v
              [SELECT] [START]
```

| Knopf | PSX | Im Spiel |
|---|---|---|
| D-Pad (8 Richtungen, Diagonale = zwei Bits) | Richtungen | Tank-Steuerung |
| X (unten, blau) | Kreuz | Rennen (halten), Abbrechen |
| [] (links, rosa) | Viereck | Aktion / Bestaetigen / Schuss beim Zielen |
| O (rechts, rot) | Kreis | Abbrechen |
| /\ (oben, gruen) | Dreieck | Abbrechen |
| L1 / R1 | L1 / R1 | R1 = Zielen als **Umschalter**: antippen hebt die Waffe, die Kampfpose bleibt stehen; erneut antippen senkt sie |
| START | Start | Inventar / Menue |
| SELECT | Select | Debug-Menue (wie am PC) |
| F9 | — | setzt eine **Marke** in `befund.log` (wie die Taste F9) |

Mehrere Finger gleichzeitig sind vorgesehen (z.B. Richtung + X = Rennen).

**R1 rastet ein (Runde 30).** R1 halten und dazu [] tippen war auf dem Touchscreen ein
Fingerknoten. Im Spiel genuegt deshalb ein kurzer Tipp auf R1: die Waffe geht hoch und bleibt
oben, [] feuert, ein zweiter Tipp auf R1 senkt sie wieder. Solange die Pose eingerastet ist,
zeigt der R1-Knopf den Gedrueckt-Stil mit einem zweiten, inneren Rahmen. Wie beim gehaltenen
R1 im Original sind dabei Tueren und Untersuchen gesperrt, bis R1 wieder angetippt wird.
Die Raste **faellt** von selbst, sobald das Spiel die Steuerung abgibt: beim Oeffnen des
Inventars (START), in der Item-Kiste, bei Texten, Cutscenes, Tuer- und Raumblenden und beim Tod.
Danach ist die Waffe unten, ein neuer Tipp hebt sie frisch. Ein Treffer laesst die Raste
stehen — die Waffe kommt danach von selbst wieder hoch. In allen Menues (Inventar, Karte,
Dateien, Kiste) sowie im Titel und in den Optionen ist R1 unveraendert eine normale Taste
(z.B. R1 im Statusschirm = Sprung auf FILE, R1 in der Kiste = weiterblaettern).
Nur das On-Screen-Pad rastet: ein angeschlossener Controller (auch Bluetooth am Geraet) und die
Tastatur am PC behalten R1 als Halte-Taste.
Der Finger darf auf dem D-Pad gleiten; die Richtung folgt dem Winkel zum Zentrum
(Totzone in der Mitte). Ein Blitz-Tipp (Finger kommt und geht zwischen zwei
Eingabe-Ticks, z.B. `adb shell input tap`) wird genau einen Tick lang gemeldet.

**Warum ein eigener SDL-Event-Watch (touch_overlay_pc.c):** SDLs Renderer rechnet bei
gesetzter logischer Groesse (320x240) Finger-Koordinaten auf den 4:3-Ausschnitt um und
**klemmt sie auf [0,1]** — alles in den Letterbox-Streifen landet am Rand (gemessen im
Emulator: Tipp bei x=243 kam als 0.000 an, START bei x=1362 als 0.613 = (1362-480)/1440).
Das Modul registriert deshalb VOR dem Renderer einen eigenen Watch und liest die rohen,
fenster-normierten Werte. Auf dem Desktop (Fenster exakt 4:3) ist die Abbildung die
Identitaet, dort faellt der Unterschied nicht auf.

Umgebungsvariablen lassen sich auf Android nicht setzen; das Overlay ist dort immer an.
Zurueck-Taste: wird abgefangen (beendet das Spiel nicht — Beenden ueber die App-Uebersicht).

## Am PC pruefen

Das Overlay ist auch im Windows-/Linux-Bau enthalten, dort aber aus. Mit
`RE15_TOUCH_OVERLAY=1` erscheint es, und die **Maus** wirkt als ein Finger
(SDL-Hint `SDL_MOUSE_TOUCH_EVENTS`). `RE15_TOUCH_SELFTEST=1` drueckt zusaetzlich jeden Knopf
synthetisch (9 Knoepfe, 8 D-Pad-Richtungen, Totzone, zwei Finger, Loslassen, Blitz-Tipp,
dazu 11 Faelle der R1-Raste) und schreibt `[touch] SELFTEST RESULT ok=32 fail=0` ins
`debug.log`; im Eingabeskript (`RE15_INPUT_SCRIPT`) steht der Buchstabe `P` fuer einen Finger
auf dem Overlay-R1 (`P0.1` = ein Tipp);
`RE15_FRAMEDUMP=<frame>:<datei.ppm>` liefert ein Bild mit Overlay. Beispiel (PowerShell):

```powershell
$env:RE15_TOUCH_OVERLAY="1"; $env:RE15_TOUCH_SELFTEST="1"; $env:RE15_NO_INTRO="1"
$env:RE15_TITLE_SHOT="title.bmp"; $env:RE15_INV_SHOT="exit34.bmp"; $env:RE15_FRAMEDUMP="33:overlay33.ppm"
.\re15_pc.exe        # exit 0 bei Spielframe 34; debug.log + overlay33.ppm im Arbeitsverzeichnis
```

Nachweise der Abnahme vom 2026-09-19 liegen unter `release/android_verify/` (Desktop-Frame mit
Overlay ueber dem Inventar, Selbsttest-Log, Emulator-Screenshots Titel/Charakterwahl).

## Emulator

Der Bau enthaelt x86_64; ein AVD mit API 24+ genuegt (getestet: Medium Phone, API 36,
1080x2400). Ablauf: `adb install -r <apk>`, `adb shell am start -n de.re15.port/.RE15Activity`,
`adb logcat -s re15` zeigt Speicherordner und Entpack-Fortschritt, `adb exec-out screencap -p
> shot.png` das Bild, `adb shell input tap <x> <y>` drueckt einen Overlay-Knopf (Knopfmitten
stehen im `debug.log` nicht; bei 2400x1080 mit u=90: D-Pad-Zentrum (243,801), START (1362,1008)).
Android zeigt beim ersten Vollbild den Systemhinweis "Viewing full screen" ueber dem Spiel —
einmal "Got it" antippen.

## Bekannte Grenzen

* Die APK ist ~365 MB gross und entpackt sich beim ersten Start noch einmal in derselben
  Groesse (zusammen ~730 MB). Ein Update der App laesst die entpackten Assets liegen und
  entpackt nur geaenderte (Groesse oder sha256), neue und fehlende Dateien nach. Fuer die
  Installation eines Updates braucht Android zusaetzlich Platz fuer die neue APK (~365 MB) plus
  seine Speicherreserve - im Emulator mit 6 GB `/data` und ~850 MB frei schlug das mit
  `INSTALL_FAILED_INSUFFICIENT_STORAGE` fehl.
* Nur Landscape. Kein Speichern der Fensterlage o.ae. — es gibt kein Fenster.
* Kein Vibrations-/Sensor-Einsatz; das Overlay hat keine Einstellungen (Groesse/Lage fest,
  relativ zur Bildhoehe).
* Controller-Belegung TYPE C (OPTIONS): dort liegt Zielen auf R2 und R1 dreht nach rechts. Das
  Overlay hat weder L2 noch R2, Zielen ist mit TYPE C auf dem Touchscreen also nicht erreichbar;
  R1 bleibt dort Halte-Taste (keine Raste). TYPE A und B rasten wie oben beschrieben.
* Ohne angeschlossenen Controller gibt es keine analogen Eingaben — das Spiel ist ohnehin
  digital (PSX-Pad).
* Der Emulator (x86_64) ist nur zum Testen gedacht; die Leistung dort haengt an der
  GPU-Emulation.
