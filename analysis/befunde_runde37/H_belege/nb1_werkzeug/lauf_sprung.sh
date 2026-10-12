#!/bin/bash
# lauf_sprung.sh <exe_quelle> <name> <ausgabeordner> <ki> <raum-hex> <bilder>  (Debug-Menue-Sprung, Neues Spiel)
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv
QUELLE=$1; NAME=$2; OUT=$3; KI=$4; RAUM=$5; BILDER=$6
mkdir -p "$OUT"
EXE=$BAUM/re15_port/build/platform/pc/re15_pc_r37h_nb1_${NAME}.exe
cp "$QUELLE" "$EXE"
( cd "$OUT" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    "RE15_DEBUG_JUMP=${RAUM}@250" RE15_STATE_LOG=state.log "RE15_EXIT_AT=${BILDER}#${RAUM}" RE15_AI_FLAVOR=$KI \
    timeout 600 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > exit.txt )
rm -f "$EXE"
grep -a -E "EXIT_AT|DEBUG_JUMP|loaded room" "$OUT/debug.log" | head -5
cat "$OUT/exit.txt"
