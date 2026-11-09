S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
cd $S
P1="-20711,-15984,756,0,11,1"; P2="941,-23776,3874,12,0,11"; PP="RE15_PLAYER_POS=500,-9175,2048,0"
for v in k5e_f1 k5c; do for a in normal key; do
 bash lauf.sh f80_${v}_$a 5080 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P1 $PP RE15_RE2_ASSET_ROOT=$S/ueber/form_${v}_$a
 bash lauf.sh f90_${v}_$a 5090 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P2 $PP RE15_RE2_ASSET_ROOT=$S/ueber/form_${v}_$a
done; done
for a in normal key; do
 bash lauf.sh gorilla_$a 11C0 300-900/60 RE15_CD_ROOT=$S/ueber/gorilla_$a
 bash lauf.sh g1_$a 3070 300-900/60 RE15_CD_ROOT=$S/ueber/g1_$a
 bash lauf.sh hangar_$a 50A0 300-900/60 RE15_CD_ROOT=$S/ueber/hangar_$a
done
bash lauf.sh hangar_key2b 50A0 300-900/60 RE15_CD_ROOT=$S/ueber/hangar_key2b
bash lauf.sh g5_normal 5090 300-900/60 RE15_ENDKAMPF_NUR_G5=1 $PP
bash lauf.sh g5_key 5090 300-900/60 RE15_ENDKAMPF_NUR_G5=1 $PP RE15_RE2_ASSET_ROOT=$S/ueber/g5_key
echo ALLE1-FERTIG
