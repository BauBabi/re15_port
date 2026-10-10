#!/bin/bash
# Nachbesserung 3 Spur C — ein exe-Lauf mit eigener Messkopie und eigenem Arbeitsordner (Muster abn2C/lauf.sh).
# Aufruf: lauf.sh <kopie-name> <ausgabeordner> <timeout_s> VAR=WERT ...
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/$1"; shift
OUT="$1"; shift
TO="$1"; shift
rm -rf "$OUT"; mkdir -p "$OUT/bilder"
( cd "$OUT" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 \
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 \
      RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log "$@" \
      timeout "$TO" "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
echo "$(cat "$OUT/exit.txt") $OUT"
