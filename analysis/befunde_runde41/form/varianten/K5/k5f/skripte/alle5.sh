S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
cd $S; PP="RE15_PLAYER_POS=500,-9175,2048,0"
i=0; for P in "941,-23776,3874,27,20,11" "941,-23776,2850,2,0,11" "941,-23776,3874,27,8,11"; do i=$((i+1))
 for v in form_k5e_f1_normal form_k5e_f1_key0; do bash lauf.sh hp${i}_$v 5090 400-440/40 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P $PP RE15_RE2_ASSET_ROOT=$S/ueber/$v | head -1; done
 bash lauf.sh hp${i}_v3 5090 400-440/40 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P $PP RE15_RE2_ASSET_ROOT=$S/vorschau/v3_f245 | head -1
done; echo ALLE5-FERTIG
