#!/usr/bin/env bash
cd C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/off; export PYTHONIOENCODING=utf-8 SZLOG=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/laeufe/V7/debug.log
for n in R3 R2 V7 R6 R4; do s=$(date +%s); python C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/w/luecke_eigen.py C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/m/$n.EMD 7 8 > luecke_$n.txt 2>&1; echo "$n $(( $(date +%s) - s ))" >> zeit.txt; done
echo FERTIG >> zeit.txt
