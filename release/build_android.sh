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
#           APK, die ein Gate abgelehnt hat.
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
#   release/build_android.sh --gate-only <apk> [--version v0.8.19]
#                                                # NUR die Gates auf eine vorhandene APK:
#                                                # kein JDK/Toolchain/Gradle, schreibt nichts
#                                                # (keine SHA256SUMS, keine Kopie nach release/);
#                                                # Version wird nur mit --version geprueft
#
# UMGEBUNG (alle optional):
#   ANDROID_SDK_ROOT / ANDROID_HOME   Sdk-Ordner (Standard: %LOCALAPPDATA%/Android/Sdk
#                                     bzw. ~/Android/Sdk)
#   JAVA_HOME                         JDK 17 (Standard: Eclipse Adoptium 17 unter
#                                     C:/Program Files, sonst das JDK im PATH)
#   RE15_KEYSTORE / RE15_KEYSTORE_PASS / RE15_KEY_ALIAS / RE15_KEY_PASS
#                                     eigener Signierschluessel; ohne ihn wird mit dem
#                                     Android-Debug-Schluessel signiert (reicht fuer Sideload)
#   RE15_PYTHON / RE15_PYTHON_NUR_PATH
#                                     Interpreter fuer die volle Asset-Pruefung, siehe
#                                     release/python_finden.sh (nie der WindowsApps-Alias)
#
# GATES am Ende (jedes bricht ab; Funktion run_gates, auch per --gate-only):
#   * Stichproben: lib/{arm64-v8a,x86_64}/{libmain.so,libSDL2.so}, assets/re15_assets.txt
#     und je eine Datei aus shared_assets/PSX, extracted_fx, RE2 und synchro
#     (WARNUNG, falls Assets komprimiert gespeichert sind)
#   * aapt dump badging: package 'de.re15.port', versionName = Version, beide ABIs
#     (ohne aapt im SDK: mit Hinweis uebersprungen)
#   * VOLLE Asset-Pruefung (seit Runde 34a, release/apk_asset_gate.py):
#       - erst "--selbsttest": das Gate muss eine gute Mini-APK annehmen und JEDE Faelschung
#         ablehnen (fehlende Datei, gleiche Groesse/anderer Inhalt, Zusatzeintrag, Manifest
#         mit falscher Groesse/fehlender Zeile, Datei nur im Quellbaum, ...)
#       - dann die APK gegen den Quellbaum: JEDE Datei der fuenf Asset-Baeume (Liste gegen
#         app/build.gradle stageAssets geprueft) liegt mit gleicher Groesse und sha256 in der
#         APK, unter assets/ liegt nichts sonst, und re15_assets.txt stimmt Zeile fuer Zeile
#         mit der APK ueberein (danach entpackt die App auf dem Geraet). Zaehlung je Baum und
#         ausdruecklich RE2/DOOR, RE15DOOR, TORSE.VBS.
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

# Pfade fuer Windows-Programme (Python, aapt) ausdruecklich als C:/... uebergeben - nicht auf
# die automatische MSYS-Umwandlung verlassen (MSYS_NO_PATHCONV=1 schaltet sie ab).
nativ_pfad() { if [[ "$HOST" == win ]]; then cygpath -m "$1"; else printf '%s\n' "$1"; fi; }

# SDK-Ordner (Bau: Pflicht, siehe unten; --gate-only: nur fuer aapt)
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
# $1 = APK. Jeder Befund bricht ab (die). Aufgerufen nach dem Bau und von --gate-only.
run_gates() {
    local apk="$1" list f n_assets n_defl aapt badging rc gate
    echo "== Gates: $apk =="
    list="$(unzip -Z1 "$apk")" || die "APK nicht lesbar (unzip -Z1): $apk"
    for f in lib/arm64-v8a/libmain.so lib/arm64-v8a/libSDL2.so lib/x86_64/libmain.so lib/x86_64/libSDL2.so \
             assets/re15_assets.txt assets/shared_assets/PSX/DATA/TEX.TIM assets/shared_assets/PSX/STAGE1/ROOM1240.RDT \
             assets/shared_assets/extracted_fx/effect0_blood.tim assets/shared_assets/RE2/CDEMD0.EMS \
             assets/synchro/STAGE1/room1170/main00.wav; do
        grep -qxF "$f" <<<"$list" || die "APK unvollstaendig: $f fehlt"
    done
    n_assets="$(grep -c '^assets/' <<<"$list" || true)"
    echo "   Inhalt: $n_assets Asset-Eintraege, native libs fuer arm64-v8a + x86_64"

    # Unkomprimiert? (unzip -v: Methode 'Stored' je Eintrag)
    n_defl="$(unzip -v "$apk" | awk '$0 ~ /assets\/shared_assets\// && $2 == "Defl:N" {c++} END {print c+0}')" \
        || die "APK nicht lesbar (unzip -v): $apk"
    [[ "$n_defl" -eq 0 ]] || echo "   WARNUNG: $n_defl Asset-Dateien sind komprimiert (noCompress-Liste pruefen)"

    aapt="$SDK/build-tools/${BUILD_TOOLS_PKG#build-tools;}/aapt"
    [[ -f "$aapt" || -f "$aapt.exe" ]] || aapt="$(ls -d "$SDK"/build-tools/*/aapt* 2>/dev/null | head -1 || true)"
    if [[ -n "$aapt" ]]; then
        badging="$("$aapt" dump badging "$(nativ_pfad "$apk")" 2>/dev/null || true)"
        grep -E "^package:|^native-code:|^sdkVersion|^targetSdkVersion|^launchable-activity" <<<"$badging" \
            | sed 's/^/   /' || true
        grep -q "name='de.re15.port'" <<<"$badging" || die "aapt: Paketname ist nicht de.re15.port"
        if [[ -n "$VERSION" ]]; then
            grep -qF "versionName='${VERSION}'" <<<"$badging" || die "aapt: versionName ist nicht '${VERSION}'"
        else
            echo "   (versionName nicht geprueft: --gate-only ohne --version)"
        fi
        grep -q "native-code:.*'arm64-v8a'" <<<"$badging" || die "aapt: arm64-v8a fehlt"
        grep -q "native-code:.*'x86_64'"    <<<"$badging" || die "aapt: x86_64 fehlt"
    else
        echo "   (aapt nicht gefunden - badging-Gate uebersprungen)"
    fi

    # VOLLE Asset-Pruefung (Runde 34a): erst beweist das Gate an Faelschungen, dass es sie
    # erkennt, dann vergleicht es JEDE Datei der Asset-Baeume mit der APK und dem Manifest.
    # Rueckgabe getrennt abfangen - kein Pipe, keine Subshell, die sie verschlucken koennte.
    gate="$(nativ_pfad "$HERE/apk_asset_gate.py")"
    echo "== Volle Asset-Pruefung 1/2: Selbsttest des Gates ($PY) =="
    rc=0; "$PY" "$gate" --selbsttest || rc=$?
    (( rc == 0 )) || die "Selbsttest des APK-Asset-Gates fehlgeschlagen (rc=$rc) - dem Gate ist nicht zu trauen"
    echo "== Volle Asset-Pruefung 2/2: APK gegen den Quellbaum =="
    rc=0; "$PY" "$gate" --repo "$(nativ_pfad "$REPO")" "$(nativ_pfad "$apk")" || rc=$?
    case "$rc" in
        0) ;;
        1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab (Befunde oben)" ;;
        *) die "APK-Asset-Gate: keine Aussage moeglich (rc=$rc, Meldung oben)" ;;
    esac
}

if [[ -n "$GATE_ONLY_APK" ]]; then
    [[ -f "$GATE_ONLY_APK" ]] || die "--gate-only: APK fehlt: $GATE_ONLY_APK"
    [[ -d "$SDK" ]] || echo "   (Android-SDK-Ordner fehlt: $SDK - ohne aapt)"
    echo "== Android-Gates auf eine vorhandene APK (--gate-only: kein Bau, schreibt nichts) =="
    run_gates "$GATE_ONLY_APK"
    echo "== ANDROID-GATES-OK (--gate-only): $GATE_ONLY_APK =="
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

OUT="$HERE/${NAME}.apk"
# ⛔ Erst pruefen, dann unter dem Auslieferungsnamen ablegen (Runde 34a). Bis v0.8.19 lag die
# APK schon VOR den Gates unter release/<name>.apk; ein Gate-Abbruch liess sie dort liegen,
# und make_package.sh haette genau diese abgelehnte APK gezippt. Jetzt: Kopie als
# .ungeprueft, Gates darauf, erst danach mv (Umbenennen = dieselben Bytes) + SHA256SUMS.
UNGEPRUEFT="$OUT.ungeprueft"
rm -f "$OUT" "$UNGEPRUEFT"
cp -f "$APK_SRC" "$UNGEPRUEFT"

# --- Gates (Funktion run_gates oben) -----------------------------------------
run_gates "$UNGEPRUEFT"
mv -f "$UNGEPRUEFT" "$OUT"

( cd "$HERE" && sha256sum "${NAME}.apk" > SHA256SUMS_android.txt )
echo
ls -la "$OUT"
cat "$HERE/SHA256SUMS_android.txt"
echo "== ANDROID-BUILD-OK: $OUT =="
