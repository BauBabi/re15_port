#!/bin/bash
# Runde 37 Spur G Nachbesserung 2 — Gitterarm-Messlauf (Aufbau Abnahme 1 Abschnitt 1f) an der echten exe.
# Wie run_nb1.sh, zusaetzlich RE15_RE2_TRACE=1: der Trace (re2_ki.log) entsteht NEBEN der exe (append) und wird nach dem
# Lauf in das Messverzeichnis verschoben — Laeufe deshalb NACHEINANDER.
# Aufruf: [JUMP=1210] [EXIT=130#1210] bash run_nb2.sh <tag> <waffe> <flavor default|re2|re15> <pos> <input> [extra env ...]
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_beretta"
TAG="$1"; WAFFE="$2"; FLAVOR="$3"; POS="$4"; INPUT="$5"; shift 5
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/${EXE_NAME:-re15_r37g_nb2.exe}"; SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
cmp -s "$SRC" "$EXE" || cp -f "$SRC" "$EXE"
WD="$BAUM/mess_r37g/$TAG"
rm -rf "$WD"; mkdir -p "$WD"
rm -f "$EXE_DIR/re2_ki.log"
FLV=""
[ "$FLAVOR" != "default" ] && FLV="RE15_AI_FLAVOR=$FLAVOR"
( cd "$WD" && env -u RE15_AI_FLAVOR RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 $FLV \
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP="${JUMP:-1210}@250" RE15_PLAYER_POS="$POS" \
    RE15_GIVE="$WAFFE:${MAG:-15}" RE15_EQUIP="$WAFFE" RE15_RE2_TRACE=1 \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="$INPUT" \
    RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_EXIT_AT="${EXIT:-130#1210}" \
    "$@" timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
[ -f "$EXE_DIR/re2_ki.log" ] && mv -f "$EXE_DIR/re2_ki.log" "$WD/re2_ki.log"
cat "$WD/exit.txt"
