#!/usr/bin/env bash
# Runde 7b: Voll-Riegel S1-S20 am Stand NEU (Spiel-Asset) mit Szene und Marker-Laeufen von r7b/laeufe_neu.sh,
# dann Gegenproben (NACHEINANDER): Stand vor 7b (a474188a, Laeufe der Runde 7a), Stand 6d (e9e71bca, Laeufe der Runde 6d) und V7.
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
P=$S/r7b
A=$S/r7a
D=$S/r6d
Q=$S/r6b
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde7b
cd $W
python riegel_v8.py --r4 $Q/emd/R4.EMD --mk-a $P/mk/lauf_MK_A --mk-b $P/mk/lauf_MK_B --gr $P/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 \
  --fd $P/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $P/mk/lauf_KO_NEU \
  --aus $O/riegel_v8r7b.txt --json $O/riegel_v8r7b.json > $P/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $P/riegel_neu.out
python riegel_v8.py $P/emd/VOR7B.EMD --r4 $Q/emd/R4.EMD --mk-a $A/mk/lauf_MK_A --mk-b $A/mk/lauf_MK_B --gr $A/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 \
  --fd $A/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $A/mk/lauf_KO_NEU \
  --aus $O/riegel_vor7b_gegenprobe.txt --json $O/riegel_vor7b_gegenprobe.json > $P/riegel_vor7b.out 2>&1
echo "rc_vor7b=$?" >> $P/riegel_vor7b.out
python riegel_v8.py $A/emd/V6d.EMD --r4 $Q/emd/R4.EMD --mk-a $D/mk/lauf_MK_A --mk-b $D/mk/lauf_MK_B --gr $D/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 \
  --fd $D/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $D/mk/lauf_KO_NEU \
  --aus $O/riegel_V8r6d_gegenprobe.txt --json $O/riegel_V8r6d_gegenprobe.json > $P/riegel_6d.out 2>&1
echo "rc_6d=$?" >> $P/riegel_6d.out
python riegel_v8.py $S/emd/EM30_V7.EMD --r4 $Q/emd/R4.EMD \
  --aus $O/riegel_V7_gegenprobe.txt --json $O/riegel_V7_gegenprobe.json > $P/riegel_v7.out 2>&1
echo "rc_v7=$?" >> $P/riegel_v7.out
echo FERTIG > $P/riegel_alle_fertig.txt
