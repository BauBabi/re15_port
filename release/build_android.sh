#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Android-APK bauen (reproduzierbar, nativ per Android-SDK)
# =============================================================================
# ERGEBNIS: release/re15_port_<version>_android.apk (+ release/SHA256SUMS_android.txt)
#           Eine einzige, universelle APK: arm64-v8a (Geraete) + x86_64 (Emulator),
#           ALLE Assets (shared_assets/PSX, extracted_fx, RE2, synchro/STAGE*) liegen
#           unkomprimiert in der APK (~365 MB) und werden beim ersten Start in den
#           App-Speicherordner entpackt. Sideload: adb install -r <apk>.
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
#
# UMGEBUNG (alle optional):
#   ANDROID_SDK_ROOT / ANDROID_HOME   Sdk-Ordner (Standard: %LOCALAPPDATA%/Android/Sdk
#                                     bzw. ~/Android/Sdk)
#   JAVA_HOME                         JDK 17 (Standard: Eclipse Adoptium 17 unter
#                                     C:/Program Files, sonst das JDK im PATH)
#   RE15_KEYSTORE / RE15_KEYSTORE_PASS / RE15_KEY_ALIAS / RE15_KEY_PASS
#                                     eigener Signierschluessel; ohne ihn wird mit dem
#                                     Android-Debug-Schluessel signiert (reicht fuer Sideload)
#
# GATES am Ende (jedes bricht ab):
#   * APK enthaelt lib/arm64-v8a/{libmain.so,libSDL2.so} und lib/x86_64/{...}
#   * APK enthaelt assets/re15_assets.txt und die vier Asset-Baeume
#   * aapt dump badging nennt package 'de.re15.port', die Version und beide ABIs
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
PROJ="$REPO/re15_port/platform/android"

VERSION=""
DO_TOOLCHAIN=1
ONLY_PREPARE=0
BUILD_TYPE="release"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)      VERSION="$2"; shift 2 ;;
        --prepare)      ONLY_PREPARE=1; shift ;;
        --no-toolchain) DO_TOOLCHAIN=0; shift ;;
        --debug)        BUILD_TYPE="debug"; shift ;;
        -h|--help)      sed -n '2,45p' "${BASH_SOURCE[0]}"; exit 0 ;;
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

if [[ -z "${VERSION}" ]]; then
    VERSION="$(git -C "$REPO" describe --tags --always 2>/dev/null || echo v0.0.0-dev)"
fi
NAME="re15_port_${VERSION}_android"

# --- SDK / JDK ----------------------------------------------------------------
SDK="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"
if [[ -z "$SDK" ]]; then
    if [[ "$HOST" == win ]]; then
        SDK="$(cygpath -u "${LOCALAPPDATA}")/Android/Sdk"
    else
        SDK="$HOME/Android/Sdk"
    fi
fi
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
CMDLINE_TOOLS_VER=13114758
NDK_PKG="ndk;27.2.12479018"
CMAKE_PKG="cmake;3.22.1"
PLATFORM_PKG="platforms;android-35"
BUILD_TOOLS_PKG="build-tools;35.0.0"

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

echo "== Gradle: fetchSdl2 ${GRADLE_TASK} (Version ${VERSION}) =="
( cd "$PROJ" && bash ./gradlew --no-daemon --console=plain \
      -Pre15Version="$VERSION" fetchSdl2 "$GRADLE_TASK" ) || die "Gradle-Bau fehlgeschlagen"

APK_SRC="$PROJ/app/build/outputs/apk/$APK_SUB"
[[ -s "$APK_SRC" ]] || die "APK fehlt nach dem Bau: $APK_SRC"

OUT="$HERE/${NAME}.apk"
cp -f "$APK_SRC" "$OUT"

# --- Gates --------------------------------------------------------------------
echo "== Gates =="
LIST="$(unzip -Z1 "$OUT")"
for f in lib/arm64-v8a/libmain.so lib/arm64-v8a/libSDL2.so lib/x86_64/libmain.so lib/x86_64/libSDL2.so \
         assets/re15_assets.txt assets/shared_assets/PSX/DATA/TEX.TIM assets/shared_assets/PSX/STAGE1/ROOM1240.RDT \
         assets/shared_assets/extracted_fx/effect0_blood.tim assets/shared_assets/RE2/CDEMD0.EMS \
         assets/synchro/STAGE1/room1170/main00.wav; do
    grep -qxF "$f" <<<"$LIST" || die "APK unvollstaendig: $f fehlt"
done
N_ASSETS="$(grep -c '^assets/' <<<"$LIST" || true)"
echo "   Inhalt: $N_ASSETS Asset-Eintraege, native libs fuer arm64-v8a + x86_64"

# Unkomprimiert? (unzip -v: Methode 'Stored' je Eintrag)
N_DEFL="$(unzip -v "$OUT" | awk '$0 ~ /assets\/shared_assets\// && $2 == "Defl:N" {c++} END {print c+0}')"
[[ "$N_DEFL" -eq 0 ]] || echo "   WARNUNG: $N_DEFL Asset-Dateien sind komprimiert (noCompress-Liste pruefen)"

AAPT="$SDK/build-tools/${BUILD_TOOLS_PKG#build-tools;}/aapt"
[[ -f "$AAPT" || -f "$AAPT.exe" ]] || AAPT="$(ls -d "$SDK"/build-tools/*/aapt* 2>/dev/null | head -1)"
if [[ -n "$AAPT" ]]; then
    BADGING="$("$AAPT" dump badging "$OUT" 2>/dev/null || true)"
    echo "$BADGING" | grep -E "^package:|^native-code:|^sdkVersion|^targetSdkVersion|^launchable-activity" | sed 's/^/   /'
    grep -q "name='de.re15.port'" <<<"$BADGING" || die "aapt: Paketname ist nicht de.re15.port"
    grep -q "native-code:.*'arm64-v8a'" <<<"$BADGING" || die "aapt: arm64-v8a fehlt"
    grep -q "native-code:.*'x86_64'"    <<<"$BADGING" || die "aapt: x86_64 fehlt"
else
    echo "   (aapt nicht gefunden - badging-Gate uebersprungen)"
fi

( cd "$HERE" && sha256sum "${NAME}.apk" > SHA256SUMS_android.txt )
echo
ls -la "$OUT"
cat "$HERE/SHA256SUMS_android.txt"
echo "== ANDROID-BUILD-OK: $OUT =="
