#!/bin/bash
# Nachbesserung 3 Spur C — dieselben 320 Laeufe wie Abnahme 2 (abn2C/blut), Sonde abn2_blut.c neu gelinkt (abn2_blut_neu.exe).
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/c_nb3_r36"
mkdir -p "$S/blut_neu"
for w in 3 4 5 6 7 8 12 13 19 20; do for s in px pz nx nz; do for h in eben tief; do for p in 2 3 4 5; do
  "$S/abn2_blut_neu.exe" $w $s $h $p > "$S/blut_neu/w${w}_${s}_${h}_p${p}.txt" 2>&1
done; done; done; done
echo FERTIG > "$S/blut_neu_FERTIG"
