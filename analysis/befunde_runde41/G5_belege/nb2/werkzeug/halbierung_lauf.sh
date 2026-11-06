#!/usr/bin/env bash
# Ein Leon-Lauf wie integration_r41_birkin_pistole, mit RE15_NB2_AUS=<maske>. Aufruf: nb2_lauf.sh <maske> <ausgabedir> [raum] [exit_at]
set -u
MASKE="$1"; OUT="$2"; RAUM="${3:-5090}"; EXITAT="${4:-4200}"
EXE_DIR=/c/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/build/platform/pc
TAG=$(echo "$MASKE" | tr -cd '0-9a-fx')_${RAUM}_$$
EXE="$EXE_DIR/re15_pc_nb2_${TAG}.exe"
cp "$EXE_DIR/re15_pc.exe" "$EXE"
mkdir -p "$OUT"; cd "$OUT" || exit 1
SKRIPT="A0.1,W1,U8.5"; for i in $(seq 1 100); do SKRIPT="$SKRIPT,MA4"; done
EXTRA=""
[ "$RAUM" = "5091" ] && export RE15_TITLE_SHOT_CHAR=1
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
  RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=${RAUM}@250 RE15_PLAYER_POS=500,-9175,2048,0 \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT="$SKRIPT" RE15_SPAWN_DIAG=1 \
  RE15_STATE_LOG=state.log RE15_GIVE=3:400,21:250 RE15_EQUIP=3 "RE15_EXIT_AT=${EXITAT}#${RAUM}" \
  RE15_NB2_AUS="$MASKE" RE15_NB2_DIAG=diag.log
start=$(date +%s)
timeout 900 "$EXE" > stdout.txt 2> stderr.txt
rv=$?
rm -f "$EXE"
echo "maske=$MASKE rv=$rv dauer=$(( $(date +%s) - start ))s" > fertig.txt
