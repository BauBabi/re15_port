#!/usr/bin/env bash
# =============================================================================
# RE1.5-Port — Linux-/Steam-Deck-Build
# =============================================================================
# Baut die UNVERAENDERTE Basis (re15_port/CMakeLists.txt) fuer Linux x86-64.
# Am Port selbst wird nichts umgestellt — der PC-Zweig ist bereits
# plattformsauber (kein <windows.h>; die einzige Win32-Nutzung in input_pc.c
# steht hinter #ifdef _WIN32).
#
# Zwei Betriebsarten:
#   1) Im Container (empfohlen, reproduzierbar):
#        docker build -t re15-deck re15_port/targets/steamdeck
#        docker run --rm -v "$PWD:/src" re15-deck
#   2) Direkt auf dem Deck (Desktop-Modus, siehe README — SteamOS ist
#      read-only, deshalb Abhaengigkeiten in einer distrobox installieren):
#        ./re15_port/targets/steamdeck/build.sh
#
# Ergebnis: <repo>/re15_port/targets/steamdeck/build/platform/pc/re15_pc
# =============================================================================
set -euo pipefail

# Repo-Wurzel bestimmen: im Container liegt sie auf /src, sonst relativ zu
# diesem Skript.
if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
fi

BUILD_DIR="$REPO/re15_port/targets/steamdeck/build"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

echo "== RE1.5 — Linux-/Steam-Deck-Build =="
echo "   Repo        : $REPO"
echo "   Bauordner   : $BUILD_DIR"
echo "   Parallelitaet: $JOBS"
echo

cmake -S "$REPO/re15_port" -B "$BUILD_DIR" -G Ninja \
      -DRE15_BUILD_PC=ON \
      -DCMAKE_BUILD_TYPE=Release

cmake --build "$BUILD_DIR" -j "$JOBS"

BIN="$BUILD_DIR/platform/pc/re15_pc"
echo
echo "== Fertig =="
ls -la "$BIN"
echo
echo "-- Laufzeit-Abhaengigkeiten (SDL2 ist statisch eingebettet) --"
ldd "$BIN" | sed 's/^\s*/   /'
echo
echo "-- hoechste benoetigte glibc-Version --"
objdump -T "$BIN" 2>/dev/null | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1 | sed 's/^/   /'
echo
echo "Weiter: targets/steamdeck/README.md (Paketieren + In Steam eintragen)"
