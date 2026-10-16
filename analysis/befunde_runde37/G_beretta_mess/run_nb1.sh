#!/bin/bash
# Runde 37 Spur G Nachbesserung 1 — Messlauf an der echten exe (Aufbau wie Abnahme 0 abn_run.sh, Anhang B).
# Aufruf: [ELZA=1] [JUMP=xxxx] [EXIT=Bild#Raum] [MAG=n] [EXE_SRC=..] [EXE_NAME=..]
#         bash run_nb1.sh <tag> <waffe> <flavor default|re2|re15> <pos x,z,rot> <input> [extra env ...]
# Ausgabe <baum>/mess_r37g/<tag>/ (unversioniert); Messkopie unter eigenem Namen neben der exe (kein taskkill /IM).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_beretta"
TAG="$1"; WAFFE="$2"; FLAVOR="$3"; POS="$4"; INPUT="$5"; shift 5
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/${EXE_NAME:-re15_r37g_nb1.exe}"; SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
cmp -s "$SRC" "$EXE" || cp -f "$SRC" "$EXE"
WD="$BAUM/mess_r37g/$TAG"
rm -rf "$WD"; mkdir -p "$WD"
if [ "${ELZA:-0}" = "1" ]; then
    START="RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1"
else
    START="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
fi
FLV=""
[ "$FLAVOR" != "default" ] && FLV="RE15_AI_FLAVOR=$FLAVOR"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 $START $FLV \
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP="${JUMP:-1140}@250" RE15_PLAYER_POS="$POS" \
    RE15_GIVE="$WAFFE:${MAG:-15}" RE15_EQUIP="$WAFFE" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="$INPUT" \
    RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log RE15_EXIT_AT="${EXIT:-260#1140}" \
    "$@" timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
cat "$WD/exit.txt"
