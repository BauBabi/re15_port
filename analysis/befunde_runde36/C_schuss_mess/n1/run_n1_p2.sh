#!/bin/bash
# Nachbesserung 1 Spur C — Punkt 2 an der echten exe (Aufbau wie run_abn_p2.sh der Abnahme 0):
# ROOM1140, Leon (-5000,-19600) Blick +x, Pistole, RE2-KI, Dauerfeuer bis zum Tod, fx.log + state.log.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_r36c_n1.exe"
cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
OUT="$BAUM/mess_r36c/n1/exe_p2/${1:-nachher}"
FD="${2:-}"
rm -rf "$OUT"; mkdir -p "$OUT"
ENVV=(RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
      RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=-5000,-19600,0"
      RE15_AI_FLAVOR=re2 "RE15_GIVE=3:60" RE15_EQUIP=3
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=W1.2,M0.6,MA9.0,M0.5,W2"
      RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log "RE15_EXIT_AT=420#1140")
if [ -n "$FD" ]; then ENVV+=("RE15_FRAMEDUMP=$FD"); fi
( cd "$OUT" && env "${ENVV[@]}" timeout 300 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
rm -f "$EXE"
echo "== $(cat "$OUT/exit.txt") $OUT"
