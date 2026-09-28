#!/usr/bin/env bash
# Nachtrag K (Granate) — ABNAHME im echten Spiel mit Tasten: Hebetisch-Fahrt in ROOM1150/1151.
# ROOM per Debug-Sprung, Spieler an der Westseite des Mitteltisches (dieselbe Lage wie
# nachschliff-sicherung-nein_werkzeug/lauf.sh), dann ueber das Eingabeskript (echte Pad-Bits):
# Viereck = Fahrt ausloesen / Modal bestaetigen, R = auf "No", S = Statusschirm.
# Zeitachse = Spielbilder (RE15_INPUT_SCRIPT_BASIS=spiel), Start Bild 200 nach dem Sprung.
# KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER: Bilder aus RE15_FRAMEDUMP.
#   $1 Ausgabeordner (unter build/r30_n_granate/)
#   Umgebung: RAUM (1150|1151), SKRIPT, SERIE, EXIT_AT, SEK, EXE, EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r30_n_granate
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
RAUM="${RAUM:-1150}"
Z="$WT/build/r30_n_granate/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm modal.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@240"
export RE15_PLAYER_POS="-21000,-18500,0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,A0.2,W8,A0.2,W3,A0.2,W4,S0.2,W4}"
export RE15_FRAMEDUMP="${SERIE:-200-1100/10}:f_"
export RE15_MODAL_LOG="modal.log"
export RE15_EXIT_AT="${EXIT_AT:-1100#$RAUM}"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
