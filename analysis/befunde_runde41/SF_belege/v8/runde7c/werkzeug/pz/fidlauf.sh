P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 EXE_SRC=$P/exe/re15_pc.exe FD=225-450/1
bash $P/w/lauf_v4.sh $P/laeufe/R3_FID $P/m/R3_FID.EMD 460 > $P/laeufe/R3_FID.txt 2>&1 &
bash $P/w/lauf_v4.sh $P/laeufe/R2_FID $P/m/R2_FID.EMD 460 > $P/laeufe/R2_FID.txt 2>&1 &
wait
