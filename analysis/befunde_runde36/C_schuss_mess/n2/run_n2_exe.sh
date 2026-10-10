#!/bin/bash
# Nachbesserung 2 Spur C — Punkt 2 an der echten exe (Aufbau = abn1C/p2_batch.sh der Abnahme 1):
# ROOM1140 per RE15_DEBUG_JUMP, Leon (-5000,-19600) Blick +x, RE2-KI, fx.log + state.log (+ Framedump),
# Messkopie re15_pc_r36c_n2.exe neben der exe (danach geloescht).
# Aufruf: run_n2_exe.sh <ausgabeordner> <waffe> <eingabe> <exit_bild> [framedump]
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_pc_r36c_n2.exe"
if [ -n "${N2_ALT:-}" ]; then EXE="$EXE_DIR/re15_pc_r36c_n2a.exe"; cp -f "$N2_ALT" "$EXE"; else cp -f "$EXE_DIR/re15_pc.exe" "$EXE"; fi
OUT="$1"; W="$2"; IN="$3"; EX="$4"; FD="${5:-}"
rm -rf "$OUT"; mkdir -p "$OUT/bilder"
ENVV=(RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
      RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 "RE15_PLAYER_POS=${N2_POS:--5000,-19600,0}"
      RE15_AI_FLAVOR=re2 "RE15_GIVE=$W:60" "RE15_EQUIP=$W"
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 "RE15_INPUT_SCRIPT=$IN"
      RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log "RE15_EXIT_AT=$EX#1140")
if [ -n "$FD" ]; then ENVV+=("RE15_FRAMEDUMP=$FD:bilder/f"); fi
( cd "$OUT" && env "${ENVV[@]}" timeout 300 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
rm -f "$EXE"
echo "== $(cat "$OUT/exit.txt") $OUT"
