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
Internet (SDL2-Tarball, Gradle-Plugins, NDK). **Kein Android Studio noetig.**

```bash
release/build_android.sh                    # Version aus git describe --tags
release/build_android.sh --version v0.8.5   # feste Version
```

Das Skript

1. installiert per `sdkmanager` in den vorhandenen SDK-Ordner nach, was fehlt
   (cmdline-tools 13114758, `ndk;27.2.12479018`, `cmake;3.22.1`, `platforms;android-35`,
   `build-tools;35.0.0`) und bestaetigt die Lizenzen,
2. ruft `./gradlew -Pre15Version=<v> fetchSdl2 assembleRelease` in `platform/android/`,
3. kopiert das Ergebnis nach `release/re15_port_<version>_android.apk`, prueft den Inhalt
   (beide ABIs, alle Asset-Baeume, `aapt dump badging`) und schreibt
   `release/SHA256SUMS_android.txt`.

Gradle-Seite (`app/build.gradle`):

* `fetchSdl2` laedt `SDL2-2.28.5.tar.gz` sha256-geprueft nach `platform/android/_deps/`
  (gitignoriert). Daraus kommen `libSDL2.so` (CMake, `jni/CMakeLists.txt`) **und** die
  Java-Klassen `org.libsdl.app.*`.
* `stageAssets`/`writeAssetManifest` spiegeln `shared_assets/PSX`, `shared_assets/extracted_fx`,
  `shared_assets/RE2` und `synchro/STAGE*` (dieselbe Liste wie `release/make_package.sh`) nach
  `app/build/re15_assets/` und schreiben `re15_assets.txt` (Groesse + Pfad je Datei).
* `noCompress` enthaelt jede Endung der Asset-Baeume: die APK speichert die Assets
  **unkomprimiert** (~365 MB), das Entpacken auf dem Geraet ist damit ein reines Kopieren.
* Signiert wird standardmaessig mit dem Android-Debug-Schluessel (`~/.android/debug.keystore`),
  das reicht fuer Sideload. Eigener Schluessel: `RE15_KEYSTORE`, `RE15_KEYSTORE_PASS`,
  `RE15_KEY_ALIAS`, `RE15_KEY_PASS` in der Umgebung.

Native Seite: `re15_port/CMakeLists.txt` mit `-DRE15_BUILD_PC=OFF -DRE15_BUILD_ANDROID=ON`
-> `platform/android/jni/CMakeLists.txt` (SDL2 shared + `libmain.so`). ABIs: `arm64-v8a`
(Geraete) und `x86_64` (Emulator).

## Installieren

```bash
adb install -r release/re15_port_<version>_android.apk
```

oder die APK aufs Geraet kopieren und dort antippen (Installation aus unbekannten Quellen
erlauben). Mindestens Android 7.0 (API 24), Ziel Android 15 (API 35).

**Erster Start:** die Assets werden aus der APK in den App-Speicherordner entpackt
(Fortschrittsbalken, ~365 MB, je nach Geraet 10-60 s). Danach startet das Spiel wie am PC
(Capcom-Intro, Titel). Weitere Starts pruefen nur einen Marker und starten sofort.

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
| `re15_assets_ok.txt` | Marker "Assets vollstaendig" (loeschen = neu entpacken) |

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
| L1 / R1 | L1 / R1 | R1 = Zielen (halten) |
| START | Start | Inventar / Menue |
| SELECT | Select | Debug-Menue (wie am PC) |
| F9 | — | setzt eine **Marke** in `befund.log` (wie die Taste F9) |

Mehrere Finger gleichzeitig sind vorgesehen (z.B. Richtung + X = Rennen, R1 + [] = Schuss).
Der Finger darf auf dem D-Pad gleiten; die Richtung folgt dem Winkel zum Zentrum
(Totzone in der Mitte). Ein Blitz-Tipp (Finger kommt und geht zwischen zwei
Eingabe-Ticks, z.B. `adb shell input tap`) wird genau einen Tick lang gemeldet — ohne diesen
Latch ging im Emulator jeder zweite Tipp verloren.

Umgebungsvariablen lassen sich auf Android nicht setzen; das Overlay ist dort immer an.
Zurueck-Taste: wird abgefangen (beendet das Spiel nicht — Beenden ueber die App-Uebersicht).

## Am PC pruefen

Das Overlay ist auch im Windows-/Linux-Bau enthalten, dort aber aus. Mit
`RE15_TOUCH_OVERLAY=1` erscheint es, und die **Maus** wirkt als ein Finger
(SDL-Hint `SDL_MOUSE_TOUCH_EVENTS`). `RE15_TOUCH_SELFTEST=1` drueckt zusaetzlich jeden Knopf
synthetisch (9 Knoepfe, 8 D-Pad-Richtungen, Totzone, zwei Finger, Loslassen, Blitz-Tipp)
und schreibt `[touch] SELFTEST RESULT ok=21 fail=0` ins `debug.log`;
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
  Groesse (zusammen ~730 MB). Ein Update der App laesst die entpackten Assets liegen;
  weichen Groessen/Liste ab, wird nur nachkopiert, was fehlt.
* Nur Landscape. Kein Speichern der Fensterlage o.ae. — es gibt kein Fenster.
* Kein Vibrations-/Sensor-Einsatz; das Overlay hat keine Einstellungen (Groesse/Lage fest,
  relativ zur Bildhoehe).
* Ohne angeschlossenen Controller gibt es keine analogen Eingaben — das Spiel ist ohnehin
  digital (PSX-Pad).
* Der Emulator (x86_64) ist nur zum Testen gedacht; die Leistung dort haengt an der
  GPU-Emulation.
