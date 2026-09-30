#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Android-APK bauen (reproduzierbar, nativ per Android-SDK)
# =============================================================================
# ERGEBNIS: release/re15_port_<version>_android.apk (+ release/SHA256SUMS_android.txt)
#           Eine einzige, universelle APK: arm64-v8a (Geraete) + x86_64 (Emulator),
#           ALLE Assets (shared_assets/PSX, extracted_fx, RE2, RE15DOOR, synchro/STAGE*)
#           liegen unkomprimiert in der APK (~365 MB) und werden beim ersten Start in den
#           App-Speicherordner entpackt. Sideload: adb install -r <apk>.
#           Die APK erscheint unter diesem Namen ERST, wenn alle Gates bestanden sind
#           (vorher heisst sie <name>.apk.ungeprueft) - make_package.sh zippt sonst eine
#           APK, die ein Gate abgelehnt hat. Eine VORIGE <name>.apk wird schon VOR Gradle
#           entfernt (Nachbesserung R1): scheitert der Bau, liegt keine alte APK unter dem
#           Auslieferungsnamen. Scheitert ein Gate, wird die .ungeprueft-Kopie geloescht (~360 MB,
#           nicht gitignoriert); die gepruefte Gradle-Ausgabe bleibt unter app/build/outputs/.
#
# WARUM DIESER WEG (Entscheidung 2026-09-19, gemessen auf dieser Maschine):
#   * Docker Desktop war nicht gestartet (Engine-Pipe fehlt) — ein Container-Bau
#     wie release/docker_win_build.sh haette hier ueberhaupt nicht laufen koennen.
#     Der SDK-Weg braucht nur, was ohnehin da ist: JDK 17, den vorhandenen Sdk-Ordner
#     (platforms/android-35, build-tools 35.0.0, platform-tools) und Internet.
#   * Was fehlte (cmdline-tools, NDK, SDK-cmake), installiert dieses Skript per
#     sdkmanager in DENSELBEN Sdk-Ordner nach — mit festen Versionen:
#         cmdline-tools 13114758 (latest), ndk;27.2.12479018 (r27c), cmake;3.22.1
#     Dieselben Versionen stehen in app/build.gradle (ndkVersion / cmake.version), der
#     Bau ist damit unabhaengig davon, was Android Studio sonst noch installiert hat.
#   * SDL2 2.28.5 — dieselbe Version wie der PC-Bau (platform/pc/CMakeLists.txt,
#     FetchContent) — laedt der Gradle-Task fetchSdl2 sha256-geprueft nach
#     re15_port/platform/android/_deps/ (gitignoriert). Aus dem Tarball kommen sowohl
#     libSDL2.so (CMake) als auch die Java-Klassen org.libsdl.app.* (SDLActivity).
#   * Gradle 8.11.1 ueber den Wrapper (Distribution wird gecacht), AGP 8.7.3.
#
# AUFRUF (Git-Bash unter Windows oder Linux):
#   release/build_android.sh                     # Version aus `git describe --tags`
#   release/build_android.sh --version v0.8.5    # feste Version
#   release/build_android.sh --prepare           # nur Toolchain nachinstallieren
#   release/build_android.sh --no-toolchain      # sdkmanager ueberspringen (schon da)
#   release/build_android.sh --debug             # assembleDebug statt Release
#   release/build_android.sh --gate-only <apk> --version v0.8.19
#                                                # NUR die Gates auf eine vorhandene APK:
#                                                # kein Toolchain-Nachinstallieren, kein Gradle,
#                                                # schreibt nichts (keine SHA256SUMS, keine Kopie).
#                                                # --version ist Pflicht; aapt, apksigner und Java
#                                                # muessen da sein (sonst Abbruch statt Luecke)
#
# UMGEBUNG (alle optional):
#   ANDROID_SDK_ROOT / ANDROID_HOME   Sdk-Ordner (Standard: %LOCALAPPDATA%/Android/Sdk
#                                     bzw. ~/Android/Sdk)
#   JAVA_HOME                         JDK 17 (Standard: Eclipse Adoptium 17 unter
#                                     C:/Program Files, sonst das JDK im PATH)
#   RE15_KEYSTORE / RE15_KEYSTORE_PASS / RE15_KEY_ALIAS / RE15_KEY_PASS
#                                     eigener Signierschluessel; ohne ihn wird mit dem
#                                     Android-Debug-Schluessel signiert (reicht fuer Sideload).
#                                     Gesetzt, aber die Datei fehlt -> Gradle bricht ab
#                                     (Nachbesserung R2; vorher still der Debug-Schluessel)
#   RE15_APK_SIGNER_SHA256            erwarteter Signer (SHA-256 des Zertifikats) statt
#                                     release/apk_signer.sha256 - fuer einen Lauf mit neuem Schluessel
#   RE15_PYTHON / RE15_PYTHON_NUR_PATH
#                                     Interpreter fuer die volle Asset-Pruefung, siehe
#                                     release/python_finden.sh (nie der WindowsApps-Alias)
#
# GATES am Ende (jedes bricht ab; release/apk_pruefen.sh - DIESELBE Kette ruft make_package.sh
# vor dem Zippen auf; hier ueber run_gates, auch per --gate-only):
#   * Stichproben: lib/{arm64-v8a,x86_64}/{libmain.so,libSDL2.so}, assets/re15_assets.txt
#     und je eine Datei aus shared_assets/PSX, extracted_fx, RE2 und synchro
#   * aapt dump badging: package 'de.re15.port', versionName = Version, beide ABIs
#     (aapt fehlt -> Abbruch; bis zur Nachbesserung R1 still uebersprungen)
#   * zipalign -c -P 16 4: Ausrichtung (Nachbesserung R2) - Android 11+ installiert sonst nicht
#   * apksigner verify: gueltige v2/v3-Signatur (seit Nachbesserung R1 - vorher gingen
#     unsignierte und nach dem Signieren veraenderte APKs durch), genau ein Signer, und zwar der aus
#     release/apk_signer.sha256 (Nachbesserung R2 - sonst kein Update ueber die vorige Version)
#   * alle Schritte lesen EINE private Kopie der APK (Nachbesserung R2): unter dem Auslieferungsnamen
#     landet genau diese gepruefte Kopie, und wurde die Datei waehrend der Pruefung getauscht, bricht
#     die Kette ab
#   * das Asset-Gate laeuft nur als private Kopie mit der sha256 aus release/apk_asset_gate.sha256, und sein Urteil
#     kommt aus Rueckgabe UND Ausgabe (Schlusszeile, Zaehlzeilen, jede Selbsttest-Fallzeile rc = soll) - Nachbesserung
#     R4-1, Gegenpruefung H1: vorher galt allein die Rueckgabe, und ein leeres Gate gab 0. APK_GATE_DATEI aus der
#     Umgebung wird ignoriert (H2).
#   * VOLLE Asset-Pruefung (seit Runde 34a, release/apk_asset_gate.py):
#       - erst "--selbsttest": das Gate muss gute Mini-APKs annehmen und JEDE Faelschung
#         ablehnen (fehlende Datei, gleiche Groesse/anderer Inhalt auch hinter dem 1. MiB und
#         CRC32-erhaltend, Zusatzeintrag, Manifest mit falscher Groesse/fehlender/doppelter/
#         Geister-Zeile, '\' oder NUL im Eintragsnamen, Local Header != Zentralverzeichnis, ...)
#       - dann die APK gegen den Quellbaum, ROH gelesen wie Androids libziparchive: JEDE Datei
#         der fuenf Asset-Baeume (Liste gegen app/build.gradle stageAssets geprueft) liegt unter
#         genau ihrem Namen mit gleicher Groesse und sha256 in der APK, unter assets/ liegt
#         nichts sonst, re15_assets.txt stimmt Zeile fuer Zeile (danach entpackt die App auf dem
#         Geraet), jeder Eintrag ist fuer Android lesbar. Zaehlung je Baum und ausdruecklich
#         RE2/DOOR, RE15DOOR, TORSE.VBS.
#       - Tuer-Soll (Nachbesserung R2): der Quellbaum selbst gegen die Engine-Tabellen - jedes
#         Port-Archiv aus gen/re15_tuer_eigen.inc (30 x Groesse, Aufbau, FNV-1a), jedes RE2-Archiv,
#         das eine Tuerzeile/ein Griff-Tausch/eine Basis nennt; fehlt eines in Quelle UND APK, bricht
#         es ab (vorher lief 29/30 gruen durch)
#     Bis v0.8.19 prueften die Gates davon nur Stichproben - von den 30 Port-Tuerarchiven
#     (RE15DOOR) keines in der APK.
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
PROJ="$REPO/re15_port/platform/android"

VERSION=""
DO_TOOLCHAIN=1
ONLY_PREPARE=0
BUILD_TYPE="release"
GATE_ONLY_APK=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)      VERSION="$2"; shift 2 ;;
        --prepare)      ONLY_PREPARE=1; shift ;;
        --no-toolchain) DO_TOOLCHAIN=0; shift ;;
        --debug)        BUILD_TYPE="debug"; shift ;;
        --gate-only)    [[ $# -ge 2 && -n "$2" ]] || { echo "--gate-only braucht den Pfad der APK" >&2; exit 2; }
                        GATE_ONLY_APK="$2"; shift 2 ;;
        -h|--help)      awk 'NR == 1 { next } /^set -euo pipefail/ { exit } { print }' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done

die() { echo "ABBRUCH: $*" >&2; exit 1; }

# --- Host ---------------------------------------------------------------------
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) HOST=win ;;
    Linux)                HOST=linux ;;
    *) die "nicht unterstuetztes Host-System: $(uname -s)" ;;
esac

# Feste Toolchain-Versionen (Bau UND Gates: aapt kommt aus BUILD_TOOLS_PKG).
CMDLINE_TOOLS_VER=13114758
NDK_PKG="ndk;27.2.12479018"
CMAKE_PKG="cmake;3.22.1"
PLATFORM_PKG="platforms;android-35"
BUILD_TOOLS_PKG="build-tools;35.0.0"

# (Pfade fuer Windows-Programme - Python, aapt, java - gibt apk_pruefen.sh per apk_nativ als C:/...)

# SDK-Ordner (Bau und --gate-only: Pflicht - aapt und apksigner kommen aus build-tools)
SDK="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"
if [[ -z "$SDK" ]]; then
    if [[ "$HOST" == win ]]; then
        SDK="$(cygpath -u "${LOCALAPPDATA}")/Android/Sdk"
    else
        SDK="$HOME/Android/Sdk"
    fi
fi

# --- Python fuer die volle Asset-Pruefung ---------------------------------------
# VOR dem Bau suchen: ein fehlendes Python soll nicht erst nach 3+ Minuten Gradle auffallen.
# ⛔ Nie "python3" blind aufrufen - unter Git-Bash ist das zuerst der WindowsApps-Alias, der in
# v0.8.17 ungefragt Python 3.14 installiert hat. python_finden.sh startet ihn nie.
if [[ $ONLY_PREPARE -eq 0 || -n "$GATE_ONLY_APK" ]]; then
    # shellcheck source=python_finden.sh
    source "$HERE/python_finden.sh" \
        || die "kein echtes Python >= 3.8 (release/python_finden.sh) - die volle Asset-Pruefung braucht es"
fi

# --- Gates --------------------------------------------------------------------
# Die Pruefkette steht EINMAL in release/apk_pruefen.sh - make_package.sh ruft dieselbe auf.
# shellcheck source=apk_pruefen.sh
source "$HERE/apk_pruefen.sh"
APK_BUILD_TOOLS="${BUILD_TOOLS_PKG#build-tools;}"

# $1 = APK. Jeder Befund bricht ab (die). Aufgerufen nach dem Bau und von --gate-only.
run_gates() {
    apk_pruefen "$1" "$VERSION" "$REPO"
}

if [[ -n "$GATE_ONLY_APK" ]]; then
    [[ -n "$VERSION" ]] || { echo "--gate-only braucht --version <versionName der APK> (Nachbesserung R1:" \
                                  "kein Lauf mehr mit ungeprueftem Feld)" >&2; exit 2; }
    [[ -f "$GATE_ONLY_APK" ]] || die "--gate-only: APK fehlt: $GATE_ONLY_APK"
    echo "== Android-Gates auf eine vorhandene APK (--gate-only: kein Bau, schreibt nichts) =="
    trap apk_pruefen_aufraeumen EXIT          # Pruefkopie (~360 MB) auch bei Abbruch weg
    ANDROID_SDK_ROOT="$SDK" apk_werkzeuge_finden
    run_gates "$GATE_ONLY_APK"
    echo "== ANDROID-GATES-OK (--gate-only): $GATE_ONLY_APK (geprueft: sha256 crc32 Bytes $APK_GEPRUEFT_KENNUNG) =="
    apk_pruefen_aufraeumen
    trap - EXIT
    exit 0
fi

if [[ -z "${VERSION}" ]]; then
    VERSION="$(git -C "$REPO" describe --tags --always 2>/dev/null || echo v0.0.0-dev)"
fi
# ⛔ Die Version MUSS x.y.z enthalten (gemessen 2026-09-19): app/build.gradle leitet daraus den
# versionCode ab (v0.8.4 -> 80400); ohne Treffer wird er 1, und "adb install -r" lehnt die
# APK dann als DOWNGRADE ab - waehrend ein Testskript, das die Meldung verschluckt, munter
# das ALTE Binary weitertestet. Drei Diagnoselaeufe gingen so ins Leere.
[[ "$VERSION" =~ [0-9]+\.[0-9]+\.[0-9]+ ]] || die "Version '$VERSION' enthaelt kein x.y.z (versionCode waere 1 -> Android-Downgrade-Sperre)"
NAME="re15_port_${VERSION}_android"

# --- SDK / JDK ----------------------------------------------------------------
# (SDK-Standardordner oben bestimmt - --gate-only braucht ihn fuer aapt)
[[ -d "$SDK" ]] || die "Android-SDK-Ordner fehlt: $SDK (ANDROID_SDK_ROOT setzen)"

if [[ -z "${JAVA_HOME:-}" ]]; then
    if [[ "$HOST" == win && -d "/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot" ]]; then
        JAVA_HOME="C:/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot"
    elif command -v java >/dev/null 2>&1; then
        JAVA_HOME="$(dirname "$(dirname "$(command -v java)")")"
    fi
fi
[[ -n "${JAVA_HOME:-}" ]] || die "JAVA_HOME nicht gesetzt und kein JDK gefunden"
if [[ "$HOST" == win ]]; then
    JAVA_BIN="$(cygpath -u "$JAVA_HOME")/bin/java"
else
    JAVA_BIN="$JAVA_HOME/bin/java"
fi
[[ -x "$JAVA_BIN" || -f "$JAVA_BIN.exe" ]] || die "java fehlt unter $JAVA_HOME/bin"
"$JAVA_BIN" -version 2>&1 | head -1 | grep -q '"17\.' || die "JDK 17 erwartet, gefunden: $("$JAVA_BIN" -version 2>&1 | head -1)"

# Windows-Schreibweisen fuer Gradle/Java (Git-Bash-Pfade versteht die JVM nicht).
sdk_native() { if [[ "$HOST" == win ]]; then cygpath -m "$SDK"; else echo "$SDK"; fi; }
SDK_NATIVE="$(sdk_native)"

export ANDROID_HOME="$SDK_NATIVE"
export ANDROID_SDK_ROOT="$SDK_NATIVE"
export JAVA_HOME
# Gradle/AGP sollen NICHT auf ein anderes NDK/cmake im PATH stossen.
unset ANDROID_NDK_HOME ANDROID_NDK_ROOT ANDROID_NDK 2>/dev/null || true

echo "== Android-Bau ${NAME} =="
echo "   Host:    $HOST"
echo "   SDK:     $SDK_NATIVE"
echo "   JDK:     $JAVA_HOME"
echo "   Projekt: $PROJ"

# --- Toolchain nachinstallieren -------------------------------------------------
# (Versionen CMDLINE_TOOLS_VER / NDK_PKG / CMAKE_PKG / PLATFORM_PKG / BUILD_TOOLS_PKG: oben)
if [[ $DO_TOOLCHAIN -eq 1 ]]; then
    if [[ ! -x "$SDK/cmdline-tools/latest/bin/sdkmanager" && ! -f "$SDK/cmdline-tools/latest/bin/sdkmanager.bat" ]]; then
        echo "== cmdline-tools ${CMDLINE_TOOLS_VER} installieren =="
        if [[ "$HOST" == win ]]; then ZIP="commandlinetools-win-${CMDLINE_TOOLS_VER}_latest.zip"
        else ZIP="commandlinetools-linux-${CMDLINE_TOOLS_VER}_latest.zip"; fi
        TMP="$SDK/.tmp_cmdline"
        rm -rf "$TMP"; mkdir -p "$TMP"
        curl -fL --retry 3 -o "$TMP/$ZIP" "https://dl.google.com/android/repository/$ZIP" \
            || die "Download der cmdline-tools fehlgeschlagen"
        ( cd "$TMP" && unzip -q "$ZIP" ) || die "cmdline-tools entpacken fehlgeschlagen"
        mkdir -p "$SDK/cmdline-tools"
        rm -rf "$SDK/cmdline-tools/latest"
        mv "$TMP/cmdline-tools" "$SDK/cmdline-tools/latest"
        rm -rf "$TMP"
    fi
    if [[ "$HOST" == win ]]; then
        SDKMANAGER="$SDK/cmdline-tools/latest/bin/sdkmanager.bat"
    else
        SDKMANAGER="$SDK/cmdline-tools/latest/bin/sdkmanager"
    fi
    [[ -f "$SDKMANAGER" ]] || die "sdkmanager fehlt: $SDKMANAGER"

    echo "== Lizenzen bestaetigen =="
    yes 2>/dev/null | "$SDKMANAGER" --sdk_root="$SDK_NATIVE" --licenses >/dev/null 2>&1 || true

    echo "== SDK-Pakete: platform-tools, ${PLATFORM_PKG}, ${BUILD_TOOLS_PKG}, ${NDK_PKG}, ${CMAKE_PKG} =="
    "$SDKMANAGER" --sdk_root="$SDK_NATIVE" "platform-tools" "$PLATFORM_PKG" "$BUILD_TOOLS_PKG" "$NDK_PKG" "$CMAKE_PKG" \
        < /dev/null || die "sdkmanager konnte die Pakete nicht installieren"
fi

NDK_DIR="$SDK/ndk/${NDK_PKG#ndk;}"
[[ -d "$NDK_DIR" ]] || die "NDK fehlt: $NDK_DIR (ohne --no-toolchain starten)"
[[ -d "$SDK/cmake/${CMAKE_PKG#cmake;}" ]] || die "SDK-cmake fehlt: $SDK/cmake/${CMAKE_PKG#cmake;}"
echo "   NDK:     $NDK_DIR"

# local.properties: sdk.dir — Gradle liest den Pfad von hier (gitignoriert).
printf 'sdk.dir=%s\n' "$SDK_NATIVE" > "$PROJ/local.properties"

if [[ $ONLY_PREPARE -eq 1 ]]; then
    echo "== Toolchain bereit (--prepare) =="
    exit 0
fi

# --- Gradle -------------------------------------------------------------------
[[ -d "$REPO/re15_port/shared_assets/PSX/STAGE1" ]] || die "Asset-Baum fehlt: re15_port/shared_assets/PSX"
[[ -s "$REPO/synchro/STAGE1/room1170/main00.wav" ]]  || die "Voiceover fehlt: synchro/STAGE1"

GRADLE_TASK="assembleRelease"; APK_SUB="release/app-release.apk"
if [[ "$BUILD_TYPE" == debug ]]; then GRADLE_TASK="assembleDebug"; APK_SUB="debug/app-debug.apk"; fi

# Pruefwerkzeuge (aapt, apksigner, Java) VOR den Bauminuten suchen - fehlt eines, bricht der
# Bau hier ab und nicht erst nach Gradle.
apk_werkzeuge_finden

OUT="$HERE/${NAME}.apk"
UNGEPRUEFT="$OUT.ungeprueft"
# ⛔ Die VORIGE APK unter dem Auslieferungsnamen VOR dem Bau entfernen (Nachbesserung R1,
# Gegenpruefung echtlauf B4). Bis dahin geschah das erst nach einem erfolgreichen Gradle-Lauf:
# scheiterte Gradle, blieb die alte <name>.apk liegen, und make_package.sh haette sie gezippt -
# die Asset-Pruefung allein faengt das nicht, wenn sich nur Code geaendert hat.
rm -f "$OUT" "$UNGEPRUEFT"

# ⛔ DEN GECACHTEN CMAKE-LAUF WEGWERFEN (2026-09-19, gemessen).
# engine/CMakeLists.txt sammelt seine Quellen per `file(GLOB src/*.c)`. GLOB wird beim
# CONFIGURE ausgewertet, und Gradle/AGP legt das Ergebnis in app/.cxx/<typ>/<hash>/ ab und
# verwendet es wieder, solange sich Gradle-Eingaben (nicht: der Quellbaum) nicht aendern.
# Folge: Quelldateien, die NACH dem letzten Configure dazukommen, fehlen still in
# libmain.so — der Bau stirbt erst im Linker mit "undefined symbol" (hier: das G5-Haut-
# Modul re15_g5_skin.c und enemy_ai_re2_zellenarm.c nach dem Zusammenfuehren von Runde 16),
# und haette ohne Linker-Fehler ein stilles Loch ins Paket gerissen.
# Der Desktop-Bau hat das Problem nicht, weil local_build.sh ohnehin neu konfiguriert.
rm -rf "$PROJ/app/.cxx"
echo "== CMake-Cache des NDK-Baus verworfen (GLOB neu auswerten) =="

echo "== Gradle: fetchSdl2 ${GRADLE_TASK} (Version ${VERSION}) =="
( cd "$PROJ" && bash ./gradlew --no-daemon --console=plain \
      -Pre15Version="$VERSION" fetchSdl2 "$GRADLE_TASK" ) || die "Gradle-Bau fehlgeschlagen"

APK_SRC="$PROJ/app/build/outputs/apk/$APK_SUB"
[[ -s "$APK_SRC" ]] || die "APK fehlt nach dem Bau: $APK_SRC"

# ⛔ Erst pruefen, dann unter dem Auslieferungsnamen ablegen (Runde 34a). Bis v0.8.19 lag die
# APK schon VOR den Gates unter release/<name>.apk; ein Gate-Abbruch liess sie dort liegen,
# und make_package.sh haette genau diese abgelehnte APK gezippt. Jetzt: Kopie als
# .ungeprueft, Gates darauf, erst danach mv (Umbenennen = dieselben Bytes) + SHA256SUMS.
# Scheitert ein Gate, loescht die Falle die .ungeprueft-Kopie (Nachbesserung R1, echtlauf B3:
# ~360 MB, nicht gitignoriert - ein "git add release/" haette sie ueber GitHubs 100-MB-Grenze
# vorgemerkt). Zur Diagnose bleibt die Gradle-Ausgabe $APK_SRC liegen.
aufraeumen_ungeprueft() {
    apk_pruefen_aufraeumen                    # Pruefkopie aus apk_pruefen.sh (Temp-Ordner)
    if [[ -f "$UNGEPRUEFT" ]]; then
        rm -f "$UNGEPRUEFT"
        echo "   (Gate-Abbruch: $(basename "$UNGEPRUEFT") geloescht; dieselben Bytes liegen weiter unter $APK_SRC)" >&2
    fi
}
trap aufraeumen_ungeprueft EXIT
cp -f "$APK_SRC" "$UNGEPRUEFT"

# --- Gates (Funktion run_gates oben) -----------------------------------------
run_gates "$UNGEPRUEFT"
# Ausgeliefert wird die GEPRUEFTE Kopie selbst (Nachbesserung R2, Gegenpruefung B3): apk_pruefen hat
# jeden Schritt an seiner privaten Kopie gemacht und am Ende verglichen, dass unter $UNGEPRUEFT noch
# dieselben Bytes liegen. Mit dem mv der Kopie kann auch danach nichts anderes mehr hineinrutschen.
mv -f "$APK_GEPRUEFT_KOPIE" "$OUT"
rm -f "$UNGEPRUEFT"
apk_pruefen_aufraeumen
trap - EXIT

( cd "$HERE" && sha256sum "${NAME}.apk" > SHA256SUMS_android.txt )
sha_out="$(cut -d' ' -f1 "$HERE/SHA256SUMS_android.txt")"
[[ "$sha_out" == "${APK_GEPRUEFT_KENNUNG%% *}" ]] || { rm -f "$OUT";
    die "ausgelieferte APK ($sha_out) ist nicht die gepruefte (${APK_GEPRUEFT_KENNUNG%% *}) - entfernt"; }
echo
ls -la "$OUT"
cat "$HERE/SHA256SUMS_android.txt"
echo "== ANDROID-BUILD-OK: $OUT =="
