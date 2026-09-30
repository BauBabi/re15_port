#!/usr/bin/env bash
# Gegenpruefung r34g/c — Messlauf (Kopie von bau_c_werkzeug/lauf.sh, Raum parametrisiert)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c
EXE="${EXE:-$WT/re15_port/build_r34_c/platform/pc/re15_pc.exe}"
Z="$WT/build/r34g_c/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log wf.log state.log fx.log lauf_rc.txt f_*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-1}"
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
[ -n "${GIVE:-}" ] && export RE15_GIVE="$GIVE"
[ -n "${EQUIP:-}" ] && export RE15_EQUIP="$EQUIP"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-300}" RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_WAFFEN_LOG="wf.log" RE15_STATE_LOG="state.log" RE15_FX_LOG="fx.log"
export RE15_EXIT_AT="${EXIT_AT:-600}#${RAUM:-1140}"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
echo "EXE=$EXE" >> env.txt
timeout -k 5 "${SEK:-150}" "$EXE" 2> debug.log
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null)" | tee lauf_rc.txt
