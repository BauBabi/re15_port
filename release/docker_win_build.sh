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
# Aufruf (Host, Git-Bash): release/build_win_cross.sh — baut das Image
# re15-wincross-build:deb11 (release/docker/Dockerfile.wincross) und bringt den
# Quellbaum als KOPIE in den Container (release/docker/kopie_lauf.sh). Direkt geht
# auch (unter Windows LANGSAM: 9p-Bindmount, kalt 45+ min in den SDL2-Pruefungen,
# Dossier analysis/befunde_runde30/nachtrag-linux-bau.md):
#   MSYS_NO_PATHCONV=1 docker run --rm -v "/c/workspace/git/reAi_v2":/src \
#       debian:11 bash /src/release/docker_win_build.sh
# Ergebnis: release/win_out/re15_pc.exe (der make_package.sh-Eingang).
# Die Engine-Verifikation laeuft weiter ueber docker_linux_build.sh (ctest);
# die Windows-exe wird auf dem HOST verifiziert (Symbol-/Verhaltensprobe + Smoke).
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$HERE/.." && pwd)"
fi

T_START=$(date +%s); T_PHASE=$T_START; PHASEN=""
phase() {
    local now; now=$(date +%s)
    echo "== Phase $1: $(( now - T_PHASE )) s  (gesamt $(( now - T_START )) s)"
    PHASEN="${PHASEN}$1=$(( now - T_PHASE ))s "
    T_PHASE=$now
}
die() { echo "!!! $*" >&2; exit 1; }

# Kopierter Quellbaum (build_win_cross.sh): Rueckfall-Links auf das Repo unter /host.
# shellcheck source=docker/rueckfall_links.sh
source "$HERE/docker/rueckfall_links.sh"

# Kein altes exe liegen lassen, das make_package.sh nach einem Fehlschlag einpackt.
mkdir -p "$REPO/release/win_out"
rm -f "$REPO/release/win_out/re15_pc.exe"

# mingw-w64 + cmake 3.28.6 (im vorgebauten Image schon da).
# shellcheck source=docker/win_deps.sh
source "$HERE/docker/win_deps.sh"
phase Pakete

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
[[ -L "$BUILD" ]] && die "$BUILD ist ein Link - der Bau muss im Container-Dateisystem laufen"
# ⛔ EINEN FREMDEN CACHE WEGRAEUMEN (2026-09-05). Host- und Container-Build teilen
# sich dieses Verzeichnis, sehen den Baum aber unter verschiedenen Pfaden
# (C:/workspace/... gegen /src/...). Liegt ein Cache der anderen Seite da, bricht
# cmake ab mit
#     CMake Error: The current CMakeCache.txt directory .../CMakeCache.txt is
#     different than the directory ... where CMakeCache.txt was created.
# und der Release-Lauf endet, BEVOR irgendetwas gebaut wurde - waehrend in
# win_out/ noch die alte exe liegt und ein Paketlauf sie klaglos einpacken wuerde.
if [[ -f "$BUILD/CMakeCache.txt" ]] &&
   ! grep -qxF "CMAKE_HOME_DIRECTORY:INTERNAL=$REPO/re15_port" "$BUILD/CMakeCache.txt"; then
    echo "   Cache stammt von einem anderen Pfad - $BUILD wird neu angelegt"
    rm -rf "$BUILD"
fi
cmake -S "$REPO/re15_port" -B "$BUILD" -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="$TC" \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX"
phase Configure
cmake --build "$BUILD" -j"$(nproc)" --target re15_pc
phase Compile+Link

mkdir -p "$REPO/release/win_out"
cp "$BUILD/platform/pc/re15_pc.exe" "$REPO/release/win_out/re15_pc.exe"
echo "WIN-CROSS-BUILD-OK: $(ls -la "$REPO/release/win_out/re15_pc.exe" | awk '{print $5}') Bytes"
phase Ausgabe
echo "== Phasen: ${PHASEN}gesamt=$(( $(date +%s) - T_START ))s"
