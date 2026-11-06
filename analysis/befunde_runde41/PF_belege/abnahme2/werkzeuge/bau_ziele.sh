#!/usr/bin/env bash
# Baut einzelne Ziele mit demselben PATH wie re15_port/tools/local_build.sh (msys64 vorn, Git-mingw64 raus).
# Grund: die Platte hat ~250 MB frei; local_build.sh build baut alle ~1060 Test-exe (2,6 GB).
set -euo pipefail
BD="$1"; shift
export PATH="/c/msys64/mingw64/bin:/c/Program Files/CMake/bin:/c/Python310/Scripts:/usr/bin:/c/Windows/System32:/c/Windows:/c/Windows/System32/Wbem"
export CC=C:/msys64/mingw64/bin/gcc.exe CXX=C:/msys64/mingw64/bin/g++.exe
cc1="$(gcc -print-prog-name=cc1)"; "$cc1" --version >/dev/null 2>&1 && echo "cc1 OK: $cc1"
cmake --build "$BD" --target "$@"
