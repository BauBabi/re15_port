#!/usr/bin/env bash
# Runde 30, Nachschliff "sicherung-nein" — Abnahme im echten Spiel mit Tasten.
# ROOM1150 per Debug-Sprung, Spieler an der Westseite des Mitteltisches, dann ueber das
# Eingabeskript (echte Pad-Bits, input_pc.c):
#   Viereck        -> Hebetisch-Fahrt 1 (sub04)
#   Rechts, Viereck-> "No" im Sicherungs-Modal
#   Viereck        -> Hebetisch-Fahrt 2
#   Viereck        -> "Yes"
#   Start          -> Statusschirm (Item 0x40 im Inventar?)
# Zeitachse = Spielbilder (RE15_INPUT_SCRIPT_BASIS=spiel), Start Bild 200 nach dem Sprung.
# $1 Ausgabeordner (unter build/r30_n_sicherung/)   $2 optional: exe
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r30_n_sicherung
EXE="${2:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
Z="$WT/build/r30_n_sicherung/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm modal.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1150@240"
export RE15_PLAYER_POS="-21000,-18500,0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=200
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,A0.2,W8,R0.2,W0.5,A0.2,W22,A0.2,W8,A0.2,W4,S0.2,W4}"
export RE15_FRAMEDUMP="${SERIE:-200-1800/10}:f_"
export RE15_MODAL_LOG="modal.log"   # relativ: MSYS-Pfadumsetzung ist aus
export RE15_EXIT_AT="${EXIT_AT:-1800#1150}"
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
