# Runde 7c: Marker-Laeufe am Endstand (nacheinander, eine Spiel-Instanz)
R=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
(time FD=225-450/1 bash $L $R/mk/lauf_LM_NEU $R/mk/LM_NEU.EMD 460) > $R/mk/lauf_LM_NEU.txt 2>&1
(time FD=225-450/1 bash $L $R/mk/lauf_KO_NEU $R/mk/KO_NEU.EMD 460) > $R/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do (time FD=300-380/1 bash $L $R/mk/lauf_$m $R/mk/$m.EMD 390) > $R/mk/lauf_$m.txt 2>&1; done
echo ALLE-FERTIG > $R/mk/fertig.txt
