#!/bin/bash
# lauf.sh <name> <exit_frame> [env assignments...]  — Messlauf der echten exe (Messkopie) im Spurbaum r41_spinnenanim.
B="C:/workspace/git/reAi_v2/.claude/worktrees/r41_spinnenanim/re15_port/build"
N="$1"; shift
EX="$1"; shift
D="$B/r41L_lauf/$N"
rm -rf "$D"; mkdir -p "$D"
cp -f "$B/platform/pc/re15_pc.exe" "$B/platform/pc/re15_pc_r41L_mess.exe"
cd "$D" || exit 1
env RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_SPINNEN_LOG=spinnen.log RE15_EXIT_AT="$EX" "$@" \
    timeout 400 "$B/platform/pc/re15_pc_r41L_mess.exe" > stdout.txt 2> stderr.txt
echo "rc=$? $(wc -l < spinnen.log 2>/dev/null) Zeilen in $D"
