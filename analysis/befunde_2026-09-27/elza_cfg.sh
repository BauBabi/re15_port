#!/usr/bin/env bash
# Configure des Elza-Zweig-Baums. Gleicher PATH-Vorrang wie local_build.sh
# (msys64 zuerst, sonst stirbt cc1 still, s. CLAUDE.md) plus die SDL2-Quelle
# aus dem Hauptbaum, damit FetchContent nichts herunterladen muss.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."
export PATH="/c/msys64/mingw64/bin:/c/Program Files/CMake/bin:/c/Python310/Scripts:/usr/bin:/c/Windows/System32:/c/Windows"
export CC="C:/msys64/mingw64/bin/gcc.exe"
export CXX="C:/msys64/mingw64/bin/g++.exe"
SDL2SRC="C:/workspace/$(echo git)/reAi_v2/re15_port/build/_deps/sdl2-src"
rm -f  re15_port/build/CMakeCache.txt
rm -rf re15_port/build/CMakeFiles
cmake -S re15_port -B re15_port/build -G Ninja \
  -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DRE15_BUILD_TOOLS=OFF \
  -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" \
  -DFETCHCONTENT_SOURCE_DIR_SDL2="$SDL2SRC"
