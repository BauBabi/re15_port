# Runde 7b: Laeufe am Stand 5a358bc9 (Spiel-Asset), NACHEINANDER (Muster r7a/laeufe.sh)
P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7b
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
while tasklist 2>/dev/null | grep -qi "^re15"; do sleep 5; done
(time FD=200-1100/1 bash $L $P/szene - 1110) > $P/lauf_szene.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_KO_NEU $P/mk/KO_NEU.EMD 460) > $P/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do
  (time FD=300-380/1 bash $L $P/mk/lauf_$m $P/mk/$m.EMD 390) > $P/mk/lauf_$m.txt 2>&1
done
(time FD=200-1100/1 bash $L $P/mk/lauf_KL_NEU $P/mk/KL_NEU.EMD 1110) > $P/mk/lauf_KL_NEU.txt 2>&1
echo ALLE-FERTIG > $P/laeufe_fertig.txt
