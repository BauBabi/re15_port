S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
cd $S
P1="-20711,-15984,756,0,11,1"; P2="941,-23776,3874,12,0,11"; PP="RE15_PLAYER_POS=500,-9175,2048,0"
for v in v3_f245 v3_f253; do
 bash lauf.sh f80_$v 5080 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P1 $PP RE15_RE2_ASSET_ROOT=$S/vorschau/$v
 bash lauf.sh f90_$v 5090 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P2 $PP RE15_RE2_ASSET_ROOT=$S/vorschau/$v
done
echo ALLE4-FERTIG
