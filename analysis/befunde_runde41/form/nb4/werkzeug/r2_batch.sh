#!/usr/bin/env bash
R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formnb4/r2; E="C:/workspace/git/reAi_v2/.claude/worktrees/r41_form_nb4/analysis/befunde_runde41/form/abn3/env"
lauf() { bash $R/werkzeug/lauf.sh "$1" 400 "$E/$1.env" > $R/lauf_$1.out 2>&1; }
for n in k_leon k_elza a_leon a_elza v_leon v_elza; do lauf $n & done; wait
for n in e_leon e_elza w_leon w_elza u_leon u_elza; do lauf $n & done; wait
echo BATCH-FERTIG
