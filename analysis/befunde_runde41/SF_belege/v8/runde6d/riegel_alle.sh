#!/usr/bin/env bash
# Runde 6d (Abgabe): Voll-Riegel S1-S20 am Endstand e9e71bca (Spiel-Asset), dann Gegenproben V8 r6b, V8 r5d, V8 r4, V7 - NACHEINANDER.
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
P=$S/r6d
Q=$S/r6b
R=$S/r6c
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde6d
cd $W
python riegel_v8.py --r4 $Q/emd/R4.EMD --mk-a $P/mk/lauf_MK_A --mk-b $P/mk/lauf_MK_B --gr $P/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 \
  --fd $P/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $P/mk/lauf_KO_NEU \
  --aus $O/riegel_v8r6d.txt --json $O/riegel_v8r6d.json > $P/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $P/riegel_neu.out
python riegel_v8.py $Q/emd/W2.EMD --r4 $Q/emd/R4.EMD --mk-a $R/mk/lauf_MK_A --mk-b $R/mk/lauf_MK_B --gr $R/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 \
  --fd $R/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $R/mk/lauf_KO_NEU \
  --aus $O/riegel_V8r6b_gegenprobe.txt --json $O/riegel_V8r6b_gegenprobe.json > $P/riegel_r6b.out 2>&1
echo "rc_r6b=$?" >> $P/riegel_r6b.out
python riegel_v8.py $Q/emd/R5D.EMD --r4 $Q/emd/R4.EMD --fd $S/pr5s/szene_fd/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $Q/mk/lauf_KO_R5D \
  --aus $O/riegel_V8r5d_gegenprobe.txt --json $O/riegel_V8r5d_gegenprobe.json > $P/riegel_r5d.out 2>&1
echo "rc_r5d=$?" >> $P/riegel_r5d.out
python riegel_v8.py $Q/emd/R4.EMD --r4 $Q/emd/R4.EMD \
  --aus $O/riegel_V8r4_gegenprobe.txt --json $O/riegel_V8r4_gegenprobe.json > $P/riegel_r4.out 2>&1
echo "rc_r4=$?" >> $P/riegel_r4.out
python riegel_v8.py $S/emd/EM30_V7.EMD --r4 $Q/emd/R4.EMD \
  --aus $O/riegel_V7_gegenprobe.txt --json $O/riegel_V7_gegenprobe.json > $P/riegel_v7.out 2>&1
echo "rc_v7=$?" >> $P/riegel_v7.out
echo FERTIG > $P/riegel_alle_fertig.txt
