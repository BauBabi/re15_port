#!/usr/bin/env bash
# Abnahme 0 SF: Labor-Lauf (Debug-Sprung wie integration_r41_sichel_form), Pistole, Zielen+Schiessen per Eingabeskript, eigene exe-Kopie.
# labor.sh <ordner> <raum> <pos> [env...]   (LABOR_INP ueberschreibt das Eingabeskript)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel
D="$1"; RAUM="$2"; POS="$3"; shift 3
mkdir -p "$D"; rm -f "$D"/*.log "$D"/out.txt
NAME="re15_abn0lab_$$"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
INP="W2"; for i in 1 2 3 4 5 6 7 8 9 10 11 12; do INP="$INP,M0.6,MA0.12"; done; INP="$INP,M0.5,W3"
INP="${LABOR_INP:-$INP}"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_CD_ROOT="$BAUM/re15_port/shared_assets/PSX" RE15_STATE_LOG=state.log RE15_ENEMY_DBG=1 RE15_EQUIP=3 RE15_GIVE=3:400 RE15_SPAWN_DIAG=1 RE15_DEBUG_JUMP=${RAUM}@250 RE15_PLAYER_POS="$POS" RE15_INPUT_SCRIPT="$INP" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300 "RE15_EXIT_AT=1100#${RAUM}" "$@" timeout 300 "./$NAME.exe" > out.txt 2>&1
echo "EXIT=$?"
rm -f "$NAME.exe"
