#!/usr/bin/env bash
# Spur B (Runde 34 Nacht) — Messlauf an der ECHTEN exe (Kopie unter eigenem Namen, damit
# local_build.sh sie nicht beendet). Vorbild analysis/befunde_runde31/hebetisch_werkzeug/lauf_fahrt.sh.
# Beschleunigter Renderer, RE15_FRAMEDUMP (Ruecklesen vor SDL_RenderPresent), KEIN AUTOSHOT/SOFTWARE_RENDER.
#   $1 Marke (Unterordner von build/r34n_b/)
#   Umgebung: RAUM (Hex, Default 1150), SKRIPT (Eingabe, Zeitachse Spielbilder ab START), START (200),
#             SERIE ("a-b/s"), EXIT_AT, SEK, SKALA (Default 3), POS ("x,y,z"), JUMP_AT (240),
#             EXTRA_ENV ("A=1 B=2"), AUDIO=1 (Ton an; sonst RE15_NOAUDIO=1)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_r34nb.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
RAUM="${RAUM:-1150}"
Z="$WT/build/r34n_b/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm fb_*.ppm modal.log hebetisch.log lauf_rc.txt
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
[ "${AUDIO:-0}" = "1" ] || export RE15_NOAUDIO=1
export RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SKALA:-3}"
export RE15_DEBUG_JUMP="${RAUM}@${JUMP_AT:-240}"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-200}"
export RE15_INPUT_SCRIPT="${SKRIPT:-W60}"
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="${SERIE}:f_"
export RE15_MODAL_LOG="modal.log" RE15_HEBETISCH_LOG="hebetisch.log"
export RE15_EXIT_AT="${EXIT_AT:-400#$RAUM}"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
