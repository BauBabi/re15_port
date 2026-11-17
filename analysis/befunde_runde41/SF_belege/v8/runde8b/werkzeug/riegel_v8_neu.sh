#!/usr/bin/env bash
# Runde 8b2: nur riegel_v8.py erneut (der erste Lauf scheiterte an der Ausgabezeile S4 Runde 8, KeyError 1 - R8b liefert seit Runde 8b ein
# Woerterbuch je Abstand); Optionen wie riegel_alle_r8c.sh.
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
R=$S/r7c
E=$S/r8/r8c
Q=$S/r6b
P3=$S/p5/pruef_zahlen-r3/laeufe
P2=$S/p5/pruef_zahlen-r2/laeufe
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde8b
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
V7L="--r7c-lm-v7 $P3/V7_LM --r7c-plain-v7 $P3/V7 --r7c-lm-ein $R/laeufe/N7B_LM --r7c-plain-ein $R/laeufe/N7B --r7c-plain-r4 $P2/R4"
export PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1 RE15_RIEGEL_JOBS=2 RE15_RIEGEL_PICKLE=$E/riegel_v8_r.pkl
cd $W
python riegel_v8.py $A --r4 $Q/emd/R4.EMD $V7L --r7c-lm $E/mk/lauf_LM_NEU --r7c-plain $E/szene \
  --mk-a $E/mk/lauf_MK_A --mk-b $E/mk/lauf_MK_B --gr $E/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $E/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $E/mk/lauf_KO_NEU --szene $E/szene/debug.log --farbe b \
  --aus $O/riegel_v8r8b.txt --json $O/riegel_v8r8b.json > $E/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $E/riegel_neu.out
echo FERTIG > $E/riegel_v8_neu_fertig.txt
