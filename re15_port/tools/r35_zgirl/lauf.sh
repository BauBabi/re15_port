#!/usr/bin/env bash
# Runde 35 Spur C: ein Messlauf mit der KOPIE re15_pc_zgirl.exe (fremde Kills treffen sie nicht).
#   $1 Marke (Unterordner von build/r35_zgirl_mess/)
#   Umgebung: JUMP FIRE SEK EXIT_AT POS AI SERIE GIVE EQUIP EXTRA_ENV SKRIPT SKRIPT_AB
set -uo pipefail
ROOT=/c/workspace/git/reAi_v2/.claude/worktrees/r35_zgirl
BD=$ROOT/re15_port/build/platform/pc
cp -f "$BD/re15_pc.exe" "$BD/re15_pc_zgirl.exe" || exit 3
Z="$ROOT/build/r35_zgirl_mess/$1"; rm -rf "$Z"; mkdir -p "$Z"; cd "$Z" || exit 2
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${JUMP:-4050@gp}"
[ -n "${FIRE:-}" ] && export RE15_FIRE_AOT="$FIRE"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
[ -n "${AI:-}" ] && export RE15_AI_FLAVOR="$AI"
[ -n "${GIVE:-}" ] && export RE15_GIVE="$GIVE"
[ -n "${EQUIP:-}" ] && export RE15_EQUIP="$EQUIP"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-1}" RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1
[ -n "${EXIT_AT:-}" ] && export RE15_EXIT_AT="$EXIT_AT"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
timeout -k 5 "${SEK:-120}" "$BD/re15_pc_zgirl.exe" > stdout.txt 2> stderr.txt
echo "rc=$? state=$(wc -l < state.log 2>/dev/null)" | tee lauf_rc.txt
