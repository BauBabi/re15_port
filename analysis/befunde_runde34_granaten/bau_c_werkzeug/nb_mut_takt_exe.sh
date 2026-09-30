#!/usr/bin/env bash
# Mutationsprobe M2 (exe-Pin integration_r34_plattform_takt): Variante $1
#   vor   = re15_pc_fx_takt() VOR re15_game_step (Zeile 7467), Aufruf 7516 entfernt
#   weg   = Aufruf 7516 entfernt (nur der Rueckfall hinter dem Zeichenblock tickt)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c
cd "$WT" || exit 2
M=re15_port/platform/pc/main.c
L=build/r34g_c/nb
[ "$(sed -n 7467p $M)" = "                re15_game_step(&gctx);" ] || { echo "Zeile 7467 passt nicht"; exit 3; }
[ "$(sed -n 7516p $M)" = "                re15_pc_fx_takt();" ] || { echo "Zeile 7516 passt nicht"; exit 3; }
case "$1" in
  vor) sed -i '7516d' $M && sed -i '7467i\                re15_pc_fx_takt();   /* MUTATION M2-vor */' $M ;;
  weg) sed -i '7516d' $M ;;
  *) echo "Variante?"; exit 4 ;;
esac
git diff --stat -- $M
git diff -- $M > $L/mut_takt_$1.diff
RE15_BUILD_DIR=$WT/re15_port/build_r34_c bash re15_port/tools/local_build.sh build > $L/mut_takt_$1.build.log 2>&1
echo "build rc=$?"; tail -2 $L/mut_takt_$1.build.log
PATH=/c/msys64/mingw64/bin:$PATH ctest --test-dir re15_port/build_r34_c -R "^integration_r34_plattform_takt$" --output-on-failure > $L/mut_takt_$1.log 2>&1
echo "ctest rc=$?"; grep -E "r34_plattform_takt:|Passed|Failed" $L/mut_takt_$1.log | head -8
git checkout -- $M
git diff --stat -- $M; echo "zurueckgesetzt: $(git status --short -- $M | wc -l) Aenderungen"
