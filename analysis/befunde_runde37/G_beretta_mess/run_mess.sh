#!/bin/bash
# Runde 37 Spur G — Messlauf an der ECHTEN exe: EIN Feuerstoss (bzw. Einzelschuss) auf einen Zombie in ROOM1140.
# Aufruf: [ELZA=1] bash run_mess.sh <tag> <waffe> <flavor re2|re15> <pos x,z,rot> <input> [extra env ...]
#   ELZA=1: Start-Rezept wie tests/integration/test_r35_werfer_elza.cmake (Spielerwahl -> Elza; der Debug-Sprung
#           landet in Elzas Raumvariante 11x1 -> EXIT_AT 260#1141).
# Ausgabe: <baum>/mess_r37g/<tag>/{state.log,wf.log,fx.log,debug.log,stdout.txt,stderr.txt,exit.txt}
# Messkopie der exe unter eigenem Namen (re15_r37g.exe) neben der exe (nie taskkill /IM re15_pc.exe).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_beretta"
TAG="$1"; WAFFE="$2"; FLAVOR="$3"; POS="$4"; INPUT="$5"; shift 5
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_r37g.exe"
cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
WD="$BAUM/mess_r37g/$TAG"
rm -rf "$WD"; mkdir -p "$WD"
if [ "${ELZA:-0}" = "1" ]; then
    START="RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1"
    EXIT_DEF="260#1141"
else
    START="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
    EXIT_DEF="260#1140"
fi
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 $START \
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP="${JUMP:-1140}@250" RE15_PLAYER_POS="$POS" \
    RE15_AI_FLAVOR="$FLAVOR" RE15_GIVE="$WAFFE:15" RE15_EQUIP="$WAFFE" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="$INPUT" \
    RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log RE15_EXIT_AT="${RE15_EXIT_AT_OVR:-$EXIT_DEF}" \
    "$@" timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
cat "$WD/exit.txt"
