#!/usr/bin/env bash
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r38_cursor
EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a.exe"
[ "${ALT:-0}" = "1" ] && EXE="$WT/re15_port/build/platform/pc/re15_pc_r38a_alt.exe"
Z="$WT/re15_port/build/r38_a/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm trilog.txt
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
export RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=11F0@250 RE15_FORCE_CUT=10
export RE15_SET_FLAG_AT="5:0,5:1,5:2,5:3,5:4,5:5,5:6,5:7,5:8,5:9,5:10,5:11,5:12@420"
export RE15_TRILOG="trilog.txt@140,100,180,138" RE15_FRAMEDUMP="500:f_1.ppm" RE15_EXIT_AT=502
timeout -k 5 150 "$EXE"
echo "rc=$?"
