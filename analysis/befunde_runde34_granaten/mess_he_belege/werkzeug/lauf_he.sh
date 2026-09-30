#!/usr/bin/env bash
# mess_he - Messlauf "Handgranate 0x09 an der echten exe" (Runde 34, Integration r34g).
# Startet NUR die byte-gleiche Messkopie re15_pc_he.exe (gleiches Verzeichnis = gleiche Asset-Wurzel),
# damit ein fremdes `taskkill /IM re15_pc.exe` den Lauf nicht trifft. KEIN AUTOSHOT, KEIN SOFTWARE_RENDER.
#   $1 Marke (Unterordner von build/r34g_mess_he/)
#   Umgebung: POS AI SKRIPT SKRIPT_AB SERIE EXIT_AT SEK JUMP SCALE GIVE EQUIP EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int
BD=$WT/re15_port/build_r34_int/platform/pc
EXE="$BD/re15_pc_he.exe"
cmp -s "$BD/re15_pc.exe" "$EXE" || { echo "MESSKOPIE NICHT BYTE-GLEICH"; exit 3; }
Z="$WT/build/r34g_mess_he/$1"; rm -rf "$Z"; mkdir -p "$Z"; cd "$Z" || exit 2
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-3}"
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
export RE15_GIVE="${GIVE:-9:5}" RE15_EQUIP="${EQUIP:-9}"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel
  export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-1}"
  export RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_WAFFEN_LOG="wf.log" RE15_STATE_LOG="state.log" RE15_FX_LOG="fx.log" RE15_GRANATE_LOG="gr.log"
export RE15_EXIT_AT="${EXIT_AT:-230}#1140"
export RE15_AI_FLAVOR="${AI:-re2}"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
t0=$(date +%s)
timeout -k 5 "${SEK:-240}" "$EXE" > stdout.txt 2> stderr.txt
rc=$?
t1=$(date +%s)
echo "rc=$rc dauer=$((t1-t0))s Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) gr=$(wc -l < gr.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null) exit_at=$(grep -c 'EXIT_AT' debug.log 2>/dev/null)" | tee lauf_rc.txt
