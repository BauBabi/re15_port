#!/usr/bin/env bash
# Runde 8 (Schwanz, Asset 66db2244, Runde 8b2): Voll-Riegel S1-S21 (riegel_v8.py mit denselben Optionen wie runde7e/riegel_alle.sh, Laeufe sfv8/r8) dazu
# --szene (debug.log des Szenenlaufs: S6 c mit Nadelspitze) und --farbe b; Teil-Riegel riegel_r7c.py NEU; Schwanz-Riegel R8 laufen in riegel_v8
# mit (Zeilen "R8a-R8f"). NACHEINANDER, S16 mit RE15_RIEGEL_JOBS=2 (Auftrag hoechstens 2 Rechenprozesse).
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
export PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1 RE15_RIEGEL_JOBS=2
cd $W
python riegel_r7c.py teil $A $O/riegel_r7c_NEU.txt --r4 $Q/emd/R4.EMD $V7L --r7c-lm $E/mk/lauf_LM_NEU --r7c-plain $E/szene > $E/riegel_r7c_NEU.out 2>&1
echo "rc_teil=$?" >> $E/riegel_r7c_NEU.out
python riegel_v8.py $A --r4 $Q/emd/R4.EMD $V7L --r7c-lm $E/mk/lauf_LM_NEU --r7c-plain $E/szene \
  --mk-a $E/mk/lauf_MK_A --mk-b $E/mk/lauf_MK_B --gr $E/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $E/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $E/mk/lauf_KO_NEU --szene $E/szene/debug.log --farbe b \
  --aus $O/riegel_v8r8b.txt --json $O/riegel_v8r8b.json > $E/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $E/riegel_neu.out
echo FERTIG > $E/riegel_alle_fertig.txt
# Runde 8b: Schwanz-Riegel R8a-R8g mit Szene (R8g gegen den 7e-Lauf), Farbprobe b und a; Gegenprobe R8g: der Lauf der Runde 8 (4cf28242) mit
# den Massen von 07b6703f bewertet (dann ist keine Verschiebung begruendet)
python riegel_schwanz_r8.py $A --farbe b --szene $S/r7e/szene/debug.log $E/szene/debug.log $S/r7e/szene/state.log $E/szene/state.log \
  --aus $O/r8_b.txt --json $O/r8_b.json > $E/r8_b.out 2>&1; echo "rc_r8b=$?" >> $E/r8_b.out
python riegel_schwanz_r8.py $E/../emd/ALT_07b6703f.EMD --farbe b --szene $S/r7e/szene/debug.log $S/r8/szene/debug.log $S/r7e/szene/state.log $S/r8/szene/state.log \
  --aus $O/r8g_gegenprobe_lauf4cf28242_masse07b6703f.txt > $E/r8g_gegen.out 2>&1; echo "rc_gegen=$?" >> $E/r8g_gegen.out
echo FERTIG2 > $E/riegel_alle2_fertig.txt
