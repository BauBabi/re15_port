S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
cd $S
P1="-20711,-15984,756,0,11,1"; P2="941,-23776,3874,12,0,11"; PP="RE15_PLAYER_POS=500,-9175,2048,0"
for v in v7; do
 bash lauf.sh f80_$v 5080 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P1 $PP RE15_RE2_ASSET_ROOT=$S/vorschau/$v
 bash lauf.sh f90_$v 5090 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P2 $PP RE15_RE2_ASSET_ROOT=$S/vorschau/$v
done
echo ALLE9-FERTIG
i=0; for P in "941,-23776,3874,27,20,11" "941,-23776,2850,2,0,11" "941,-23776,3874,27,8,11"; do i=$((i+1))
 bash lauf.sh hp${i}_v7 5090 400-440/40 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P $PP RE15_RE2_ASSET_ROOT=$S/vorschau/v7 | head -1; done
echo ALLE9B-FERTIG
