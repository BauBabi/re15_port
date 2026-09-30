#!/usr/bin/env bash
# Spur B (Runde 34 Nacht): einen exe-Integrationshaken GENAU wie ctest ausfuehren (Kommando aus
# `ctest -N -V`), aber mit einer Kopie der exe unter eigenem Namen neben dem Original — immun gegen den
# Namens-Kill `taskkill //F //IM re15_pc.exe` (local_build.sh:298-299 bei parallel bauenden Agenten).
# Beleg dafuer, ob ein roter GUI-Haken eine Regression ist oder abgeschossen wurde.
#   $1 Testname (z.B. integration_r30_sicherung_laden), $2 Anzahl Laeufe (Default 2)
set -u
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch
B="$WT/re15_port/build"
export PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH"
ORIG="C:/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch/re15_port/build/platform/pc/re15_pc.exe"
KOPIE="C:/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch/re15_port/build/platform/pc/re15_pc_r34nb_kopie.exe"
cp -f "$B/platform/pc/re15_pc.exe" "$B/platform/pc/re15_pc_r34nb_kopie.exe" || exit 2
cmd="$(ctest --test-dir "$B" -N -V -R "^$1\$" 2>/dev/null | sed -n 's/^[0-9]*: Test command: //p' | head -1)"
[ -n "$cmd" ] || { echo "kein Kommando fuer $1"; exit 3; }
cmd="${cmd//$ORIG/$KOPIE}"
for i in $(seq 1 "${2:-2}"); do
    t0=$(date +%s)
    eval "$cmd" > /dev/null 2>"$B/haken_kopie_$1_$i.err"
    rc=$?
    echo "$1 Lauf $i (exe-Kopie): rc=$rc in $(( $(date +%s) - t0 )) s"
    [ $rc -eq 0 ] || tail -5 "$B/haken_kopie_$1_$i.err"
done
rm -f "$B/platform/pc/re15_pc_r34nb_kopie.exe"
