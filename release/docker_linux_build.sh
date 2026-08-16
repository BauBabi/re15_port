#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Linux-/Steam-Deck-Release-Build (laeuft IN der Bauumgebung)
# =============================================================================
# BASIS: Debian 11 (glibc 2.31, GCC 10) — derselbe Stand wie das offizielle
# Steam-Runtime-3.0-SDK ("sniper"), gegen das Steam-Spiele auf dem Deck laufen.
#
# NICHT ubuntu:22.04 verwenden. Damit gebaut verlangte das v0.1.1-Binary
# GLIBC_2.34; SteamOS 3.4 liefert nur 2.33, das Spiel startet dort gar nicht.
# Auf Debian-11-Basis kommt das gleiche Binary mit GLIBC_2.29 aus.
# Deshalb existiert dazu ein Gate in release/make_package.sh (check_glibc).
#
# Aufruf (Host): release/build_linux_deck.sh — der waehlt Container/distrobox
# und ruft dieses Skript hier drinnen auf. Direkt geht auch:
#   podman run --rm -v "$PWD:/src" -w /src debian:11 bash release/docker_linux_build.sh
# =============================================================================
set -euo pipefail

if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
fi

# SDL2 2.28.5 wird statisch aus Quelle gebaut (FetchContent) und braucht die
# Entwicklungs-Header der Anzeige-/Audio-Stacks. In einer frischen Container-
# Basis fehlen sie; in einer bereits eingerichteten distrobox nicht — deshalb
# nur installieren, wenn wir root sind und der Compiler noch fehlt.
# (libdecor-0-dev gibt es in Debian 11 NICHT — SDL2 faellt dort auf den
# eingebauten Wayland-Dekorationspfad zurueck.)
if [[ "$(id -u)" == "0" ]] && ! command -v gcc >/dev/null 2>&1; then
    export DEBIAN_FRONTEND=noninteractive
    apt-get update -qq
    apt-get install -y -qq --no-install-recommends \
        build-essential ninja-build git ca-certificates wget \
        libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxss-dev \
        libxfixes-dev libwayland-dev libxkbcommon-dev wayland-protocols \
        libasound2-dev libpulse-dev libdbus-1-dev libudev-dev \
        libgl1-mesa-dev libegl1-mesa-dev >/dev/null
fi
# cmake: Debian 11 (bullseye) liefert nur 3.18, re15_port verlangt >=3.21, und
# bullseye-backports ist inzwischen archiviert (kein Release-File). Deshalb das
# offizielle Kitware-Binary-Tarball (statisch, aendert die glibc-Anforderung
# des SPIELS nicht — Gate check_glibc prueft weiterhin das Binary).
if ! cmake --version 2>/dev/null | grep -qE ' 3\.(2[1-9]|[3-9][0-9])| [4-9]\.'; then
    CMV=3.28.6
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMV}/cmake-${CMV}-linux-x86_64.tar.gz" -O /tmp/cmake.tgz
    tar -xzf /tmp/cmake.tgz -C /opt
    export PATH="/opt/cmake-${CMV}-linux-x86_64/bin:$PATH"
fi

BUILD="$REPO/release/lbuild"
cmake -S "$REPO/re15_port" -B "$BUILD" -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX"
cmake --build "$BUILD" -j"$(nproc)"

( cd "$BUILD" && ctest --timeout 120 --output-on-failure 2>&1 | tail -3 )

mkdir -p "$REPO/release/linux_out"
cp "$BUILD/platform/pc/re15_pc" "$REPO/release/linux_out/"
ldd    "$REPO/release/linux_out/re15_pc" > "$REPO/release/linux_out/ldd.txt" 2>&1 || true
objdump -T "$REPO/release/linux_out/re15_pc" 2>/dev/null \
    | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1 \
    > "$REPO/release/linux_out/glibc_max.txt" || true

echo "hoechste glibc-Anforderung: $(cat "$REPO/release/linux_out/glibc_max.txt" 2>/dev/null)"
echo LINUX-BUILD-OK
