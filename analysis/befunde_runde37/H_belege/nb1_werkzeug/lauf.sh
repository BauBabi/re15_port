#!/bin/bash
# lauf.sh <exe_quelle> <name> <ausgabeordner> <ki: re15|re2> <bilder>
# Weg wie integration_r37_zombieaktiv: Karte ROOM1150 vor Tuer Slot 0 (Montage gesehen) + CONTINUE, Aktion -> Tuer -> 1130.
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv
QUELLE=$1; NAME=$2; OUT=$3; KI=$4; BILDER=$5
mkdir -p "$OUT"
KARTE=$BAUM/re15_port/build/tests/unit/probe_r36_gegnerzahl_karte.exe
( cd "$OUT" && "$KARTE" re15_card.mcr 1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7 7:0xc6-0xca > karte.txt 2>&1 )
EXE=$BAUM/re15_port/build/platform/pc/re15_pc_r37h_nb1_${NAME}.exe
cp "$QUELLE" "$EXE"
( cd "$OUT" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 "RE15_INPUT_SCRIPT=W1,A0.2,W2" \
    RE15_STATE_LOG=state.log "RE15_EXIT_AT=${BILDER}#1130" RE15_AI_FLAVOR=$KI \
    timeout 900 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > exit.txt )
rm -f "$EXE"
grep -a -E "CONTINUE: resumed|Nachspawn 1130|EXIT_AT|AI flavor|ai_flavor|flavor" "$OUT/debug.log" | head -8
cat "$OUT/exit.txt"
