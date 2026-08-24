#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Windows-Release-Build als DOCKER-CROSS-BUILD (laeuft IM Container)
# =============================================================================
# ANLASS (2026-08-24): der lokale Build starb mit cc1.exe 0xC0000139
# STATUS_ENTRYPOINT_NOT_FOUND. ⚠️ NACHTRAG SELBEN TAGES — die damalige Diagnose
# "Host-Compiler tot" ist WIDERLEGT: die Toolchain war intakt, Ursache war
# PATH-Shadowing (Gits libwinpthread-1.dll ohne clock_gettime64 stand vor
# msys64s). Fix: re15_port/tools/local_build.sh; Details HANDOVER §0a5.
# Dieser Cross-Build bleibt trotzdem der richtige Weg fuer Release-Artefakte:
# er macht das Windows-Binary maschinenunabhaengig
# reproduzierbar: Debian 11 + gcc-mingw-w64 (GCC 10) + SDL2 statisch aus dem
# FetchContent-Quellbaum — dieselbe Konfiguration wie der native mingw64-Build
# (-static, GUI-Subsystem via CMake/MINGW-Pfad).
#
# Aufruf (Host, Git-Bash):
#   MSYS_NO_PATHCONV=1 docker run --rm -v "/c/workspace/git/reAi_v2":/src \
#       debian:11 bash /src/release/docker_win_build.sh
# Ergebnis: release/win_out/re15_pc.exe (der make_package.sh-Eingang).
# Die Engine-Verifikation laeuft weiter ueber docker_linux_build.sh (ctest);
# die Windows-exe wird auf dem HOST verifiziert (Symbol-/Verhaltensprobe + Smoke).
# =============================================================================
set -euo pipefail

if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
fi

if [[ "$(id -u)" == "0" ]] && ! command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -qq
    apt-get install -y -qq --no-install-recommends \
        gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 binutils-mingw-w64-x86-64 \
        ninja-build git ca-certificates wget make >/dev/null
fi

# cmake wie im Linux-Skript: Kitware-Tarball (bullseye liefert nur 3.18).
if ! cmake --version 2>/dev/null | grep -qE ' 3\.(2[1-9]|[3-9][0-9])| [4-9]\.'; then
    CMV=3.28.6
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMV}/cmake-${CMV}-linux-x86_64.tar.gz" -O /tmp/cmake.tgz
    tar -xzf /tmp/cmake.tgz -C /opt
    export PATH="/opt/cmake-${CMV}-linux-x86_64/bin:$PATH"
fi

# POSIX-Thread-Variante des Cross-GCC (C11-Threads/SDL): Debian installiert
# beide; -posix explizit waehlen, wo vorhanden.
CC_BIN=x86_64-w64-mingw32-gcc
CXX_BIN=x86_64-w64-mingw32-g++
command -v x86_64-w64-mingw32-gcc-posix >/dev/null 2>&1 && CC_BIN=x86_64-w64-mingw32-gcc-posix
command -v x86_64-w64-mingw32-g++-posix >/dev/null 2>&1 && CXX_BIN=x86_64-w64-mingw32-g++-posix

TC=/tmp/mingw_toolchain.cmake
cat > "$TC" <<EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER ${CC_BIN})
set(CMAKE_CXX_COMPILER ${CXX_BIN})
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

BUILD="$REPO/release/wxbuild"
cmake -S "$REPO/re15_port" -B "$BUILD" -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="$TC" \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX"
cmake --build "$BUILD" -j"$(nproc)" --target re15_pc

mkdir -p "$REPO/release/win_out"
cp "$BUILD/platform/pc/re15_pc.exe" "$REPO/release/win_out/re15_pc.exe"
echo "WIN-CROSS-BUILD-OK: $(ls -la "$REPO/release/win_out/re15_pc.exe" | awk '{print $5}') Bytes"
