#!/bin/bash
# run.sh <tag> <exe-variante> [VAR=WERT ...]  (Muster F_abn3/run.sh) -> eigener Ordner <tag>_<variante> in F_nb4
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/F_nb4"
X="C:/workspace/git/reAi_v2/.claude/worktrees/r36_zombieki/re15_port/build/platform/pc"
tag="$1"; v="$2"; shift 2
d="$S/${tag}_${v}"; rm -rf "$d"; mkdir -p "$d"; cd "$d"
echo "re15_pc_$v.exe RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log $*" > cmd.txt
t0=$(date +%s)
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log "$@" timeout 900 "$X/re15_pc_$v.exe" > out.txt 2> err.txt
rc=$?
echo "$tag $v rc=$rc $(( $(date +%s) - t0 ))s" >> "$S/runs.log"
