#!/usr/bin/env bash
# Bauen mit dem PATH-Vorrang aus local_build.sh, ohne dessen Configure (der
# kennt FETCHCONTENT_SOURCE_DIR_SDL2 nicht). Argument: ninja-Ziel, Default alles.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."
export PATH="/c/msys64/mingw64/bin:/c/Program Files/CMake/bin:/c/Python310/Scripts:/usr/bin:/c/Windows/System32:/c/Windows"
taskkill //F //IM re15_pc.exe >/dev/null 2>&1 || true
cmake --build re15_port/build "$@"
