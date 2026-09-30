#!/usr/bin/env bash
# Runde 34 (Granaten) - AUSGANGSMESSUNG im echten Spiel (vorhandene exe, NICHT neu gebaut).
# Vorlage: analysis/befunde_runde30/nachtrag-granate_werkzeug/lauf_wurf.sh.
# ROOM1140 (Briefing-Raum, Zombies) per Debug-Sprung, Granate per RE15_GIVE/RE15_EQUIP
# (Mess-Harness platform/pc/main.c:4215-4252), dann R1 halten + Quadrat (echte Pad-Bits,
# platform/pc/src/input_pc.c). KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER: Bilder aus RE15_FRAMEDUMP.
#
# ZEITACHSE: RE15_INPUT_SCRIPT_BASIS=spiel -> das Skript laeuft auf g_engine.frame_count. Der
# Zaehler wird beim Raumeintritt auf 0 gesetzt (platform/pc/main.c:7798), der Sprung liegt bei
# Spielbild 250 -> SKRIPT_AB muss > 250 sein, sonst liefe das Skript schon VOR dem Sprung an.
# Alle Bildnummern in state.log / wf.log / Framedumps / EXIT_AT sind damit Bilder NACH dem Eintritt.
#
#   $1 Marke (Unterordner von build/r34g_baseline/)
#   Umgebung: GIVE EQUIP SKRIPT SKRIPT_AB SERIE EXIT_AT SEK AI EXTRA_ENV
set -uo pipefail
ROOT=/c/workspace/git/reAi_v2
EXE="${EXE:-$ROOT/re15_port/build/platform/pc/re15_pc.exe}"
Z="$ROOT/build/r34g_baseline/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm wf.log state.log fx.log lauf_rc.txt
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-3}"
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
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) Log=$(wc -l < debug.log 2>/dev/null)" | tee lauf_rc.txt
