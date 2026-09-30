#!/usr/bin/env bash
# mut_exe.sh <name> <datei> <old-datei> <new-datei> <R34_NUR>
# Mutation anwenden, exe bauen (local_build.sh build = nur die exe DIESES Bauverzeichnisses beenden),
# integration_r34_granaten (Auswahl R34_NUR) laufen lassen, Datei byte-gleich zurueck, neu bauen.
set -u
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/4d3b5439-572c-42ae-896b-4b076d038232/scratchpad
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int
B=$WT/re15_port/build_r34_int
name=$1; f=$2
cp -f "$f" "$S/mutx_backup.bin"
python "$S/sub.py" "$f" "$3" "$4" || { echo "[$name] MUTATION NICHT ANWENDBAR"; exit 3; }
( cd $WT && RE15_BUILD_DIR=$B bash re15_port/tools/local_build.sh build > "$S/mutx_build_$name.log" 2>&1 ) || { echo "[$name] BAU FEHLGESCHLAGEN"; tail -5 "$S/mutx_build_$name.log"; }
"/c/Program Files/CMake/bin/cmake.exe" -DRE15_PC_EXE="$B/platform/pc/re15_pc.exe" -DRE15_PPM_TOOL="$B/tests/unit/probe_r34_ppm_beitrag.exe" -DWORKDIR="$B/tests/integration/r34_granaten_mut_$name" -DR34_NUR="$5" -P "$WT/re15_port/tests/integration/test_r34_granaten.cmake" > "$S/mutx_test_$name.log" 2>&1
rc=$?
echo "[$name] Test rc=$rc (erwartet != 0)"; grep -E 'r34_granaten' "$S/mutx_test_$name.log" | tail -4
cp -f "$S/mutx_backup.bin" "$f"
cmp -s "$f" "$S/mutx_backup.bin" && echo "[$name] ZURUECK: byte-gleich"
( cd $WT && RE15_BUILD_DIR=$B bash re15_port/tools/local_build.sh build > "$S/mutx_rebuild_$name.log" 2>&1 ) && echo "[$name] NEUBAU OK"
