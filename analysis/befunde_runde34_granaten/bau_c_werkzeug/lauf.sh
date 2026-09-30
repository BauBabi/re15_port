#!/usr/bin/env bash
# Runde 34 Spur C - Messlauf mit der exe DIESES Arbeitsbaums (Vorlage: C0 lauf_v5.sh /
# port_inventar_werkzeug/lauf_baseline.sh). KEIN AUTOSHOT, KEIN SOFTWARE_RENDER.
#   $1 Marke (Unterordner von build/r34g_c/)
#   Umgebung: EXE GIVE EQUIP SKRIPT SKRIPT_AB SERIE EXIT_AT SEK JUMP SCALE AI EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c
EXE="${EXE:-$WT/re15_port/build_r34_c/platform/pc/re15_pc.exe}"
Z="$WT/build/r34g_c/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log wf.log state.log fx.log lauf_rc.txt f_*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-1}"
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
export RE15_GIVE="${GIVE:-9:5}" RE15_EQUIP="${EQUIP:-9}"
export RE15_INPUT_SCRIPT_BASIS=spiel
export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-300}"
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,M1,MA0.2,M2.5,W4}"
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_WAFFEN_LOG="wf.log"
export RE15_STATE_LOG="state.log"
export RE15_FX_LOG="fx.log"
export RE15_EXIT_AT="${EXIT_AT:-600}#1140"
[ -n "${AI:-}" ] && export RE15_AI_FLAVOR="$AI"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
echo "EXE=$EXE" >> env.txt
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null)" | tee lauf_rc.txt
