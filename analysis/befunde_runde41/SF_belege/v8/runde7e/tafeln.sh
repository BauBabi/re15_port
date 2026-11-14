#!/usr/bin/env bash
# Runde 7e (Handfarbe): Tafeln 16_hand_r7e.png und 18_handstellung_r7e.png (neben 16_hand.png / 18_handstellung.png in SF_belege/v8) mit
# tafel_r7a.py, Spalten Vorlage | V7 | Stand 7d (5c902d49) | NEU (07b6703f); Laeufe: NEU = sfv8/r7e/szene, Stand 7d = sfv8/r7d/szene,
# V7 = sfv4/r/V7_szene. NACHEINANDER.
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
T=$B/analysis/befunde_runde41/SF_belege/v8
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
ALT=$S/r7e/emd/ALT_5c902d49.EMD
V7=$S/emd/EM30_V7.EMD
V7S=$S/../sfv4/r/V7_szene
export PYTHONIOENCODING=utf-8
cd $W
git -C $B show 639132b77:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD > $ALT
python tafel_r7a.py hand --s6d $ALT --mitte "Stand 7d" --neu $A --v7 $V7 --aus $T/16_hand_r7e.png > $S/r7e/tafel_hand.txt 2>&1
echo "rc=$?" >> $S/r7e/tafel_hand.txt
python tafel_r7a.py stellung --fd-v7 $V7S/fd --fd-6d $S/r7d/szene/fd --fd-neu $S/r7e/szene/fd --s6d $ALT --mitte "Stand 7d" --neu $A --v7 $V7 \
  --aus $T/18_handstellung_r7e.png > $S/r7e/tafel_stellung.txt 2>&1
echo "rc=$?" >> $S/r7e/tafel_stellung.txt
echo TAFELN-FERTIG > $S/r7e/tafeln_fertig.txt
