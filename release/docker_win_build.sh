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
    # ⛔ SNAPSHOT STATT LEBENDEM SPIEGEL - dieselbe Haertung wie in
    # docker_linux_build.sh, hier am 2026-09-05 nachgezogen, nachdem der
    # Windows-Build an genau demselben Fehler starb:
    #     E: Failed to fetch .../libperl5.32_5.32.1-4+deb11u5_amd64.deb  404
    # Debian 11 ist in Rente: der normale Spiegel liefert fuer bullseye noch einen
    # INDEX, aber nicht mehr jedes darin genannte Paket. snapshot.debian.org haelt
    # jeden historischen Stand vor, Index und Pool passen dort per Konstruktion
    # zusammen - und der Release-Build wird dadurch reproduzierbar.
    # Zeitstempel und Begruendung: siehe docker_linux_build.sh (2026-07-01 ist der
    # aelteste, der mit debian:11 (11.11) ohne Herabstufungen durchgeht).
    SNAP=20260701T000000Z
    { echo "deb http://snapshot.debian.org/archive/debian/$SNAP/ bullseye main"
      echo "deb http://snapshot.debian.org/archive/debian-security/$SNAP/ bullseye-security main"
    } > /etc/apt/sources.list
    echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99snapshot
    echo 'Acquire::Retries "5";' >> /etc/apt/apt.conf.d/99snapshot
    PKGS="gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 binutils-mingw-w64-x86-64 ninja-build git ca-certificates wget make"
    apt-get update -qq
    if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
        echo "   Snapshot-Spiegel unvollstaendig - raeume die Listen und versuche erneut"
        rm -rf /var/lib/apt/lists/*
        apt-get clean
        apt-get update -qq
        if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
            echo "   apt-Spiegel unvollstaendig - schalte auf archive.debian.org um"
            echo 'deb http://archive.debian.org/debian bullseye main' > /etc/apt/sources.list
            echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99archive
            apt-get update -qq
            apt-get install -y -qq --no-install-recommends \
                --allow-downgrades --allow-change-held-packages $PKGS >/dev/null
        fi
    fi
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
cmake --build "$BUILD" -j"$(nproc)" --target re15_pc

mkdir -p "$REPO/release/win_out"
cp "$BUILD/platform/pc/re15_pc.exe" "$REPO/release/win_out/re15_pc.exe"
echo "WIN-CROSS-BUILD-OK: $(ls -la "$REPO/release/win_out/re15_pc.exe" | awk '{print $5}') Bytes"
