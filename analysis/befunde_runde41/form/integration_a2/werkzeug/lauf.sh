#!/bin/bash
# lauf.sh TAG TEIL GIVE EQUIP EXIT DUMP OHNE_HAKEN
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2a2b
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port
mkdir -p $S/wd_$1
cmake -DRE15_PC_EXE=$B/build/platform/pc/re15_pc.exe -DWORKDIR=$S/wd_$1 -DRE15_FORM_HP_HEADER=$B/include/re15_birkin_formen.h \
 -DRE15_BILD_TOOL=$B/build/tests/unit/probe_r41_form_kamera_bild.exe -DTEIL=$2 -DMESS_TAG=$1 -DMESS_GIVE=$3 -DMESS_EQUIP=$4 \
 -DMESS_EXIT=$5 -DMESS_DUMP=$6 -DMESS_OHNE_HAKEN=$7 -DMESS_RE2=$8 -DMESS_W=$9 -P $S/mess.cmake > $S/out_$1.txt 2>&1
echo "EXIT=$?" >> $S/out_$1.txt
