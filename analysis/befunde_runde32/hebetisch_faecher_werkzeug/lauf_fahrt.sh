#!/usr/bin/env bash
# Runde 32 / H (Kopie aus Runde 31, Pfade auf r32) — ABNAHME im echten Spiel: Hebetisch-Fahrt in ROOM1150 (Tuerweg per Debug-Sprung).
# Vorbild analysis/befunde_runde30/nachtrag-granate_werkzeug/lauf_fahrt.sh (dieselbe Spielerlage,
# dasselbe Eingabeskript-Schema: A = Viereck, R = rechts, S = Start; Zeitachse Spielbilder ab 200).
# Echte exe, beschleunigter Renderer, KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER; Bilder aus
# RE15_FRAMEDUMP (Ruecklesen vor SDL_RenderPresent).
#   $1 Ausgabeordner (unter build/r32_hebetisch/)
#   Umgebung: RAUM (1150|1151), SKRIPT, SERIE, EXIT_AT, SEK, EXE, SKALA (Fensterskalierung, 1 = 320x240),
#             EXTRA_ENV (z.B. "RE15_SET_FLAG=9:56" = OHNE Granate)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r32_hebetisch
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
RAUM="${RAUM:-1150}"
Z="$WT/build/r32_hebetisch/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm modal.log hebetisch.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_DEBUG_JUMP="${RAUM}@240"
export RE15_PLAYER_POS="-21000,-18500,0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,A0.2,W60}"
export RE15_FRAMEDUMP="${SERIE:-220-700/2}:f_"
export RE15_MODAL_LOG="modal.log" RE15_HEBETISCH_LOG=hebetisch.log
export RE15_EXIT_AT="${EXIT_AT:-700#$RAUM}"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
