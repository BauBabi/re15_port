#!/usr/bin/env bash
# Pruefer-Lauf: lauf_p.sh <dir> <exit-bild> <raum-hex> <skript> [framedump-spec]
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r31_generator
D="$1"; EX="$2"; RM="$3"; S="$4"; FD="${5:-}"
mkdir -p "$D"; cd "$D"; rm -f panel.log debug.log fd*.ppm scd.log
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_DEBUG_JUMP="${RM}@240" RE15_FIRE_AOT="1@40#${RM}" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT="$S" \
    RE15_PANEL_LOG=panel.log ${FD:+RE15_FRAMEDUMP="$FD"} RE15_EXIT_AT="${EX}#${RM}" \
    timeout 500 "$BAUM/re15_port/build/platform/pc/re15_pr31gen.exe" > out.txt 2>&1
echo "EXIT=$?"
