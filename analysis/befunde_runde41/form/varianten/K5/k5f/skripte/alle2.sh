S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
cd $S
PP="RE15_PLAYER_POS=500,-9175,2048,0"
bash lauf.sh g5c_normal 5090 300-900/60 RE15_ENDKAMPF_NUR_G5=1 $PP RE15_FORCE_CUT=11
bash lauf.sh g5c_key 5090 300-900/60 RE15_ENDKAMPF_NUR_G5=1 $PP RE15_FORCE_CUT=11 RE15_RE2_ASSET_ROOT=$S/ueber/g5_key
for c in 4 12; do
 bash lauf.sh hc${c}_normal 50A0 300-900/60 RE15_FORCE_CUT=$c RE15_CD_ROOT=$S/ueber/hangar_normal
 bash lauf.sh hc${c}_key 50A0 300-900/60 RE15_FORCE_CUT=$c RE15_CD_ROOT=$S/ueber/hangar_key
 bash lauf.sh hc${c}_key2b 50A0 300-900/60 RE15_FORCE_CUT=$c RE15_CD_ROOT=$S/ueber/hangar_key2b
done
bash lauf.sh z91_normal 1091 300-900/60 RE15_CD_ROOT=$S/ueber/zombie_normal
bash lauf.sh z91_key 1091 300-900/60 RE15_CD_ROOT=$S/ueber/zombie_key
bash lauf.sh p4010 4010 400-460/60 RE15_CD_ROOT=$S/ueber/g1_normal
bash lauf.sh p50B0 50B0 400-460/60 RE15_CD_ROOT=$S/ueber/g1_normal
echo ALLE2-FERTIG
