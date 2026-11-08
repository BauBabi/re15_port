#!/bin/bash
# gegen.sh EXE-KUERZEL TEIL
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2a2b
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port
mkdir -p $S/gp_$1_$2
cmake -DRE15_PC_EXE=$B/build/platform/pc/re15_pc_$1_w2a2.exe -DWORKDIR=$S/gp_$1_$2 -DRE15_FORM_HP_HEADER=$B/include/re15_birkin_formen.h \
 -DRE15_BILD_TOOL=$B/build/tests/unit/probe_r41_form_kamera_bild.exe -DTEIL=$2 -P $B/tests/integration/test_r41_form_wagenweg.cmake > $S/gp_$1_$2.txt 2>&1
echo "EXIT=$?" >> $S/gp_$1_$2.txt
