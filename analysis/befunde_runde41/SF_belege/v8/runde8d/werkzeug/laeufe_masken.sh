#!/usr/bin/env bash
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1
export EXE_SRC=$BAUM/re15_port/build_r8/platform/pc/re15_pc.exe
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
for k in A8c MG_16 MG_17 MG_18 MG_19 MG_20 MG_21 MG_22 MB_16 MB_17 MB_18 MB_19 MB_20 MB_21 MB_22; do
  FD=255-320/1 bash $L $S/r8e/lauf_$k $S/r8e/$k.EMD 330 2>&1 | head -n 2 | tr '\n' ' '; echo $k
done
echo FERTIG
