# Runde 7a: Szenenlauf am Stand a474188a (Spiel-Asset) und Marker-Laeufe, NACHEINANDER (Muster runde6d/laeufe.sh)
P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7a
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
(time FD=200-1100/1 bash $L $P/szene - 1110) > $P/lauf_szene.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_KO_NEU $P/mk/KO_NEU.EMD 460) > $P/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do
  (time FD=300-380/1 bash $L $P/mk/lauf_$m $P/mk/$m.EMD 390) > $P/mk/lauf_$m.txt 2>&1
done
echo ALLE-FERTIG > $P/laeufe_fertig.txt
