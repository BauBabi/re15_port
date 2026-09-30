#!/usr/bin/env bash
# Integration r34g - Messlauf mit einer BYTE-GLEICHEN KOPIE der exe (re15_pc_m1.exe, gleiches Verzeichnis
# = gleiche Asset-Wurzel), damit ein fremdes `taskkill /IM re15_pc.exe` den Lauf nicht trifft.
#   $1 Marke (Unterordner von $S/laeufe)
#   Umgebung: GIVE EQUIP SKRIPT SKRIPT_AB SERIE EXIT_AT SEK JUMP SCALE AI EXTRA_ENV POS
set -uo pipefail
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/4d3b5439-572c-42ae-896b-4b076d038232/scratchpad
BD=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int/re15_port/build_r34_int/platform/pc
cp -f "$BD/re15_pc.exe" "$BD/re15_pc_m1.exe"
EXE="$BD/re15_pc_m1.exe"
Z="$S/laeufe/$1"; rm -rf "$Z"; mkdir -p "$Z"; cd "$Z" || exit 2
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-3}"
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
[ -n "${GIVE:-}" ] && export RE15_GIVE="$GIVE"
[ -n "${EQUIP:-}" ] && export RE15_EQUIP="$EQUIP"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel
  export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-300}"
  export RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_WAFFEN_LOG="wf.log"
export RE15_STATE_LOG="state.log"
export RE15_FX_LOG="fx.log"
export RE15_EXIT_AT="${EXIT_AT:-600}"
[ -n "${AI:-}" ] && export RE15_AI_FLAVOR="$AI"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
timeout -k 5 "${SEK:-180}" "$EXE" > stdout.txt 2> stderr.txt
rc=$?
echo "rc=$rc Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null)" | tee lauf_rc.txt
