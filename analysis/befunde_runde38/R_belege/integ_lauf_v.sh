#!/bin/bash
# lauf_v.sh <name> <exit_frame> [env assignments...]  — Messlauf der echten exe im Integrationsbaum (Runde 38,
# versteckte Schachtspinne). Laufordner re15_port/build/r38_lauf_int/<name>; Messkopie der exe neben dem Original
# (Asset-Wurzel am exe-Pfad).
B="C:/workspace/git/reAi_v2/.claude/worktrees/r38_integration/re15_port/build"
N="$1"; shift
EX="$1"; shift
D="$B/r38_lauf_int/$N"
rm -rf "$D"; mkdir -p "$D"
cp -f "$B/platform/pc/re15_pc.exe" "$B/platform/pc/re15_pc_r38int_mess.exe"
cd "$D" || exit 1
env RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_SPINNEN_LOG=spinnen.log RE15_STATE_LOG=state.log RE15_EXIT_AT="$EX" "$@" \
    timeout 420 "$B/platform/pc/re15_pc_r38int_mess.exe" > stdout.txt 2> stderr.txt
echo "rc=$? $(wc -l < spinnen.log 2>/dev/null) Zeilen spinnen.log, $(wc -l < state.log 2>/dev/null) Zeilen state.log"
