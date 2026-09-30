#!/usr/bin/env bash
# Messung r34g "alle weiteren Gegnertypen" - ein exe-Lauf mit der BYTE-GLEICHEN KOPIE re15_pc_geg.exe
# (gleiches Verzeichnis = gleiche Asset-Wurzel; ein fremdes `taskkill /IM re15_pc.exe` trifft sie nicht).
# Vorlage: integration_werkzeug/lauf.sh, test_r34_granaten.cmake (Env-Satz der Wurf-Laeufe).
#   $1 Marke (Unterordner von build/r34g_mess_geg/)
#   Umgebung: JUMP POS AI GIVE EQUIP SKRIPT SKRIPT_AB SERIE EXIT_AT SEK SCALE EXTRA_ENV
set -uo pipefail
ROOT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int
BD=$ROOT/re15_port/build_r34_int/platform/pc
EXE="$BD/re15_pc_geg.exe"
[ -x "$EXE" ] || { echo "exe-Kopie fehlt: $EXE"; exit 3; }
Z="$ROOT/build/r34g_mess_geg/$1"; rm -rf "$Z"; mkdir -p "$Z"; cd "$Z" || exit 2
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-1}"
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
[ -n "${AI:-}" ] && export RE15_AI_FLAVOR="$AI"
[ -n "${GIVE:-}" ] && export RE15_GIVE="$GIVE"
[ -n "${EQUIP:-}" ] && export RE15_EQUIP="$EQUIP"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel
  export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-1}"
  export RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
export RE15_STATE_LOG=state.log RE15_GRANATE_LOG=gr.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log
[ -n "${EXIT_AT:-}" ] && export RE15_EXIT_AT="$EXIT_AT"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
t0=$(date +%s)
timeout -k 5 "${SEK:-240}" "$EXE" > stdout.txt 2> stderr.txt
rc=$?
t1=$(date +%s)
echo "rc=$rc sek=$((t1-t0)) Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) gr=$(wc -l < gr.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null) exit_at=$(grep -c 'EXIT_AT' debug.log 2>/dev/null)" | tee lauf_rc.txt
