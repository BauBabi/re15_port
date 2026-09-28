#!/usr/bin/env bash
# Nachtrag K (Granate) — MESSLAUF im echten Spiel: Hand Grenade (Item 0x09) ausruesten und werfen.
# ROOM1140 (Briefing-Raum, Zombies) per Debug-Sprung, Waffe per RE15_GIVE/RE15_EQUIP
# (reiner Mess-Harness, main.c), dann R1 halten + Viereck (echte Pad-Bits, input_pc.c).
# KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER: Bilder aus RE15_FRAMEDUMP.
#   $1 Marke (Unterordner von build/r30_n_granate/)   Umgebung: SKRIPT, SERIE, SEK, EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r30_n_granate
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
Z="$WT/build/r30_n_granate/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm wf.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
export RE15_GIVE="${GIVE:-9:5}" RE15_EQUIP="${EQUIP:-9}"
export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-330}"
export RE15_INPUT_SCRIPT="${SKRIPT:-W1,M1,MA0.2,M2.5,W3,M1,MA0.2,M2.5,W3}"
export RE15_FRAMEDUMP="${SERIE:-330-730/4}:f_"
export RE15_WAFFEN_LOG="wf.log"
export RE15_EXIT_AT="${EXIT_AT:-400}"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-120}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) Log=$(wc -l < debug.log 2>/dev/null)" | tee lauf_rc.txt
