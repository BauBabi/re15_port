#!/usr/bin/env bash
# Runde 7d (Abgabe, Asset unveraendert = Stand 7c 5c902d49): Teil-Riegel riegel_r7c.py (S3/S9/S16/S18/S21 nach E9-E13) und Voll-Riegel
# S1-S21 (riegel_v8.py --r7c*) am Stand NEU mit den NEUEN Laeufen der Abgabe (sfv8/r7d: Szene FD 200-1100, Marker LM/KO/MK_A/MK_B/GR,
# Marker-Assets bytegleich zu r7c/mk), NACHEINANDER. Gegenproben wie runde7c/riegel_alle.sh (dieselben Laeufe der Pruefer P5 R3/R2 und r7c,
# andere Assets; nur gelesen).
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
R=$S/r7c
D=$S/r7d
Q=$S/r6b
P3=$S/p5/pruef_zahlen-r3/laeufe
P2=$S/p5/pruef_zahlen-r2/laeufe
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde7d
V7L="--r7c-lm-v7 $P3/V7_LM --r7c-plain-v7 $P3/V7 --r7c-lm-ein $R/laeufe/N7B_LM --r7c-plain-ein $R/laeufe/N7B --r7c-plain-r4 $P2/R4"
export PYTHONIOENCODING=utf-8
cd $W
teil() {  # name emd lm plain
  python riegel_r7c.py teil $2 $O/riegel_r7c_$1.txt --r4 $Q/emd/R4.EMD $V7L --r7c-lm $3 --r7c-plain $4 > $D/riegel_r7c_$1.out 2>&1
  echo "rc_$1=$?" >> $D/riegel_r7c_$1.out
}
teil NEU $B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD $D/mk/lauf_LM_NEU $D/szene
teil R3_gegenprobe $R/m/R3B.EMD $R/laeufe/R3B_LM $R/laeufe/R3B
teil R2_gegenprobe $R/m/R2.EMD $P3/R2_LM $P3/R2
teil V7_gegenprobe $R/m/V7.EMD $P3/V7_LM $P3/V7
teil R4_gegenprobe $R/m/R4.EMD $P2/R4_LM $P2/R4
teil R5D_gegenprobe $R/m/R5D.EMD $P2/R5D_LM $P2/R5D
teil vor7c_gegenprobe $R/m/N7B.EMD $R/laeufe/N7B_LM $R/laeufe/N7B
echo TEILE-FERTIG > $D/riegel_teile_fertig.txt
python riegel_v8.py $B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD --r4 $Q/emd/R4.EMD $V7L --r7c-lm $D/mk/lauf_LM_NEU --r7c-plain $D/szene \
  --mk-a $D/mk/lauf_MK_A --mk-b $D/mk/lauf_MK_B --gr $D/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $D/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $D/mk/lauf_KO_NEU \
  --aus $O/riegel_v8r7d.txt --json $O/riegel_v8r7d.json > $D/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $D/riegel_neu.out
# Voll-Gegenproben wie runde6d / runde7b (riegel_v8.py ohne --r7c*): Stand 6d e9e71bca (Laeufe r6d) und V7 (ohne Lauf)
python riegel_v8.py $S/r7a/emd/V6d.EMD --r4 $Q/emd/R4.EMD --mk-a $S/r6d/mk/lauf_MK_A --mk-b $S/r6d/mk/lauf_MK_B --gr $S/r6d/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7   --fd $S/r6d/szene/fd --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $S/r6d/mk/lauf_KO_NEU   --aus $O/riegel_V8r6d_gegenprobe.txt --json $O/riegel_V8r6d_gegenprobe.json > $D/riegel_6d.out 2>&1
echo "rc_6d=$?" >> $D/riegel_6d.out
python riegel_v8.py $S/emd/EM30_V7.EMD --r4 $Q/emd/R4.EMD   --aus $O/riegel_V7_gegenprobe.txt --json $O/riegel_V7_gegenprobe.json > $D/riegel_v7.out 2>&1
echo "rc_v7=$?" >> $D/riegel_v7.out
echo FERTIG > $D/riegel_alle_fertig.txt
