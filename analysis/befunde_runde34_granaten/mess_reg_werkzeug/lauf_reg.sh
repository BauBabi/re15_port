#!/usr/bin/env bash
# Messer "Regressionen, Item-Debug, Zielen" (Runde 34 Integration) - ein exe-Lauf.
# NUR Kopien starten: Integration = re15_pc_reg.exe (byte-gleich, selbes Verzeichnis),
# master v0.8.19 = mst_reg/re15_pc_mreg.exe (byte-gleich zur Hauptbaum-exe, EIGENES Verzeichnis,
# damit befund.log NICHT neben die Nutzer-exe im Hauptbaum geschrieben wird; Asset-Wurzel =
# exe-Vorfahr re15_port/ -> dieselben shared_assets wie die Integrations-exe).
#   $1 = int | mst     $2 = Marke (Unterordner von laeufe/)
#   Umgebung: GIVE EQUIP SKRIPT SKRIPT_AB SERIE EXIT_AT SEK JUMP SCALE AI POS EXTRA_ENV NOLOG
set -uo pipefail
W=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_int
BD=$W/re15_port/build_r34_int/platform/pc
case "$1" in
  int) EXE="$BD/re15_pc_reg.exe" ;;
  mst) EXE="$BD/mst_reg/re15_pc_mreg.exe" ;;
  *) echo "exe?"; exit 2 ;;
esac
Z="$W/build/r34g_mess_reg/laeufe/$1_$2"; rm -rf "$Z"; mkdir -p "$Z"; cd "$Z" || exit 2
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE="${SCALE:-3}"
[ "${JUMP:-1140@250}" != "-" ] && export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
[ -n "${GIVE:-}" ] && export RE15_GIVE="$GIVE"
[ -n "${EQUIP:-}" ] && export RE15_EQUIP="$EQUIP"
if [ -n "${SKRIPT:-}" ]; then
  export RE15_INPUT_SCRIPT_BASIS=spiel
  export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-300}"
  export RE15_INPUT_SCRIPT="$SKRIPT"
fi
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
if [ -z "${NOLOG:-}" ]; then
  export RE15_WAFFEN_LOG="wf.log" RE15_STATE_LOG="state.log" RE15_FX_LOG="fx.log" RE15_GRANATE_LOG="gr.log"
fi
export RE15_EXIT_AT="${EXIT_AT:-600}"
[ -n "${AI:-}" ] && export RE15_AI_FLAVOR="$AI"
[ -n "${POS:-}" ] && export RE15_PLAYER_POS="$POS"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt
echo "$EXE" > exe.txt
t0=$(date +%s.%N)
timeout -k 5 "${SEK:-180}" "$EXE" > stdout.txt 2> stderr.txt
rc=$?
t1=$(date +%s.%N)
echo "rc=$rc dauer=$(python -c "print(round($t1-$t0,1))") Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) state=$(wc -l < state.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null) debug=$(wc -l < debug.log 2>/dev/null)" | tee lauf_rc.txt
