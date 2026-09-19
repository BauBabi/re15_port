#!/bin/bash
# Gegenprobe-Lauf mit der exe VOR dem Fix (aus HEAD~1 gebaut, in den Scratchpad kopiert).
# Aufruf: bash run_vorher.sh <exe> <run-verzeichnis> <sekunden> <FRAMEDUMP-Spec>
EXE="$1"; RUNDIR="$2"; SECS="${3:-150}"; FDD="$4"
mkdir -p "$RUNDIR"
cd "$RUNDIR" || exit 2
cp -n C:/workspace/git/reAi_v2/re15_card.mcr . 2>/dev/null
rm -f gator_boss.log state.log debug.log stdout.log
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_DEBUG_JUMP=2090@gp RE15_GB_TEST=1
export RE15_AI_FLAVOR=re2 RE15_STATE_LOG=state.log RE15_BEFUND_MARKE=1
export RE15_SOFTWARE_RENDER=1 RE15_FRAMEDUMP="$FDD" RE15_BOOT_EXIT_AT=2
"$EXE" > stdout.log 2>&1 &
PID=$!
sleep "$SECS"
kill "$PID" 2>/dev/null || taskkill //F //PID "$PID" > /dev/null 2>&1
sleep 2
echo "run beendet (pid $PID)"
grep -n "FRESS-P2\|FSYNC3" gator_boss.log 2>&1 | head -4
ls fd_*.ppm 2>/dev/null | wc -l
