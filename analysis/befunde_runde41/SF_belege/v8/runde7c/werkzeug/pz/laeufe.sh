#!/usr/bin/env bash
# Pruefer P5 Runde 3: eigene Szenenlaeufe, zwei Spuren. Eigene Exe-Kopie pruef_zahlen-r3/exe (8f15f6c5).
P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 EXE_SRC=$P/exe/re15_pc.exe FD=225-450/1
spur() { for n in "$@"; do s=$(date +%s); bash $P/w/lauf_v4.sh $P/laeufe/$n $P/m/$n.EMD 460 > $P/laeufe/$n.txt 2>&1; echo "$n fertig $(( $(date +%s) - s )) s" >> $P/laeufe/zeit.txt; done; }
spur R3 R3_LM R2 &
spur V7 V7_LM R2_LM &
wait
echo ALLE > $P/laeufe/alle_fertig.txt
