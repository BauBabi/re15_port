#!/usr/bin/env bash
# mut.sh <datei> <old-datei> <new-datei> <ziel> <test-regex>
# Mutation anwenden (sub.py), Ziel bauen, Test laufen lassen, Datei byte-gleich zurueck, Ziel neu bauen.
set -u
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/4d3b5439-572c-42ae-896b-4b076d038232/scratchpad
B=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int/re15_port/build_r34_int
f="$1"; cp -f "$f" "$S/mut_backup.bin"
python "$S/sub.py" "$f" "$2" "$3" || { echo "MUTATION NICHT ANWENDBAR"; exit 3; }
export PATH=/c/msys64/mingw64/bin:"/c/Program Files/CMake/bin":/c/Python310/Scripts:$PATH
cmake --build "$B" --target "$4" > "$S/mut_build.log" 2>&1 || { echo "BAU FEHLGESCHLAGEN"; tail -5 "$S/mut_build.log"; }
( cd "$B" && ctest -R "$5" --output-on-failure 2>&1 | grep -E 'FAIL|Passed|Failed|GRUEN' | head -8 )
cp -f "$S/mut_backup.bin" "$f"
cmp -s "$f" "$S/mut_backup.bin" && echo "ZURUECK: byte-gleich"
cmake --build "$B" --target "$4" > "$S/mut_build.log" 2>&1 && echo "NEUBAU OK"
