#!/usr/bin/env bash
# Pruefer r31/hebetisch: echte exe, beschleunigter Renderer, RE15_FRAMEDUMP, KEIN AUTOSHOT/SOFTWARE_RENDER.
# Tuerweg per RE15_DEBUG_JUMP wie der Bauer (lauf_fahrt.sh), aber mit Mess-Protokoll RE15_HEBETISCH_LOG
# und eigenem Eingabeskript.  $1 = Unterordner von build/r31_hebetisch/ (Praefix p_)
#   RAUM (1150|1151), SKRIPT, SERIE, EXIT_AT, SEK, SKALA, EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r31_hebetisch
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
RAUM="${RAUM:-1150}"
Z="$WT/build/r31_hebetisch/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm modal.log hebetisch.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_DEBUG_JUMP="${RAUM}@240"
export RE15_PLAYER_POS="-21000,-18500,0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,A0.2,W60}"
export RE15_FRAMEDUMP="${SERIE:-220-700/2}:f_"
export RE15_MODAL_LOG="modal.log"
export RE15_HEBETISCH_LOG=hebetisch.log
export RE15_EXIT_AT="${EXIT_AT:-700#$RAUM}"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
