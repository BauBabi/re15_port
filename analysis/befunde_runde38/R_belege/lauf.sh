#!/bin/bash
# lauf.sh <name> <exit_frame> [extra env assignments...]
# Messlauf der echten exe (Messkopie) im Spurbaum r38_spinnen. Env: siehe Aufrufer.
B="C:/workspace/git/reAi_v2/.claude/worktrees/r38_spinnen/re15_port/build"
N="$1"; shift
EX="$1"; shift
D="$B/r38_lauf/$N"
rm -rf "$D"; mkdir -p "$D"
cp -f "$B/platform/pc/re15_pc.exe" "$B/platform/pc/re15_pc_r38sp_mess3.exe"
cd "$D" || exit 1
env RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_SPINNEN_LOG=spinnen.log RE15_EXIT_AT="$EX" "$@" \
    timeout 400 "$B/platform/pc/re15_pc_r38sp_mess3.exe" > stdout.txt 2> stderr.txt
echo "rc=$? $(wc -l < spinnen.log 2>/dev/null) Zeilen"
