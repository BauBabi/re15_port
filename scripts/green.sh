#!/usr/bin/env bash
# =====================================================================
#  green.sh  ·  Task I0-4 of PORTING_ROADMAP.md (Phase 0 foundation)
#
#  The ONE command that establishes a reproducible green baseline:
#    kill the self-lock -> configure (PC + TESTS on) -> build -> ctest.
#  Every other task/agent builds on this; run it before + after a change.
#
#  Usage:  bash scripts/green.sh            (build + all tests)
#          bash scripts/green.sh --build    (build only, skip ctest)
#  Toolchain: mingw64 GCC + Ninja + CMake (per CLAUDE.md).
# =====================================================================
set -euo pipefail

export PATH="/c/msys64/mingw64/bin:$PATH"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="$ROOT/re15_port/build"

# 1) release the exe self-lock (a running re15_pc.exe blocks the linker)
taskkill //F //IM re15_pc.exe >/dev/null 2>&1 || true

# 2) configure once (tests default OFF in CMake -> force them ON)
# ⚠️ Wenn $BUILD schon existiert, aber OHNE RE15_BUILD_TESTS konfiguriert wurde,
#    wird hier NICHT neu konfiguriert — ctest meldet dann "No tests were found!!!"
#    und beendet mit EXIT 0. Deshalb prueft Schritt 4 unten die Summenzeile.
if [ ! -f "$BUILD/build.ninja" ]; then
    echo ">> configuring (PC + TESTS)…"
    cmake -S "$ROOT/re15_port" -B "$BUILD" -G Ninja \
          -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON >/dev/null
fi

# 3) build
echo ">> building…"
cmake --build "$BUILD"

if [ "${1:-}" = "--build" ]; then
    echo ">> GREEN (build only)"; exit 0
fi

# 4) test — print the canonical pass line, fail loudly
echo ">> testing…"
if ctest --test-dir "$BUILD" --timeout 60 --output-on-failure | tee /tmp/re15_ctest.log | tail -1; then
    line="$(grep -E '[0-9]+% tests passed' /tmp/re15_ctest.log | tail -1 || true)"
    # KEINE Summenzeile = ctest hat gar keine Tests gefunden und trotzdem 0
    # geliefert. Das frueher hier gedruckte ">> GREEN BASELINE: unknown" war ein
    # falsches Gruen mit Exit 0 (gemessen 2026-08-24) -> jetzt ROT.
    [ -n "$line" ] || { echo ">> RED — ctest lieferte KEINE Summenzeile (0 Tests konfiguriert? RE15_BUILD_TESTS=ON fehlt im vorhandenen $BUILD)"; exit 1; }
    echo ">> GREEN BASELINE: $line"
else
    echo ">> RED — see failures above"; exit 1
fi
