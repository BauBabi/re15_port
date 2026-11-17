#!/usr/bin/env bash
# Runde 8b2 nacheinander (hoechstens 2 Rechenprozesse): suche (Beleg suche_r8b.txt neu), Kette Lauf 3 (nach dem Pin-Wechsel), riegel_v8 neu.
R8=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde8b
export PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1
cd $B/re15_port/tools/r41_sichel/v8
{ time python schwanz_r8.py suche $R8/emd/ALT_07b6703f.EMD ; } > $O/suche_r8b.txt 2>&1
echo "rc_suche=$?" >> $O/suche_r8b.txt
cd $B
rm -rf $R8/r8c/kette_3
bash re15_port/tools/r41_sichel/v8_kette.sh $R8/r8c/kette_3 > $R8/r8c/kette_3.txt 2>&1
echo "rc_kette=$?" >> $R8/r8c/kette_3.txt
bash $R8/r8c/riegel_v8_neu.sh
echo FERTIG > $R8/r8c/rest_fertig.txt
