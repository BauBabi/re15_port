#!/usr/bin/env bash
# lauf.sh <name> <limit-s> KEY=VAL...   - eigene exe-Kopie, nur eigene PID (timeout)
set -uo pipefail
B=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/platform/pc
S=/c/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb5
N=$1; L=$2; shift 2
W=$S/l_$N; rm -rf $W; mkdir -p $W
X=re15_pc_nb5_${N}_$$.exe
cp $B/re15_pc.exe $B/$X
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1
export RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log
for kv in "$@"; do export "$kv"; done
cd $W
timeout $L $B/$X >stdout.log 2>stderr.log; echo "exit=$?"
rm -f $B/$X
