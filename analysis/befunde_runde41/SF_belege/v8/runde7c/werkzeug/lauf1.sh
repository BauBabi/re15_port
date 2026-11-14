# Runde 7c: Laeufe NACHEINANDER (eine Spiel-Instanz), Overlay-EMD, Spiel-Asset unberuehrt.  bash lauf1.sh NAME (EMD = r7c/m/NAME.EMD)
R=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 PYTHONIOENCODING=utf-8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
N="$1"
cd $R/pz && python mark.py $R/m/$N.EMD $R/m/${N}_LM.EMD > $R/laeufe/${N}_mark.txt 2>&1
(time FD=200-1100/1 bash $L $R/laeufe/$N $R/m/$N.EMD 1110) > $R/laeufe/$N.txt 2>&1
(time FD=225-450/1 bash $L $R/laeufe/${N}_LM $R/m/${N}_LM.EMD 460) > $R/laeufe/${N}_LM.txt 2>&1
echo LAUF1-FERTIG > $R/laeufe/${N}_fertig.txt
