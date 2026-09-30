#!/usr/bin/env bash
# Nachbesserung M1: Messlauf ueber den LADE-Pfad (Speicherkarte -> Continue) in einen Raum.
#   $1 Marke  $2 Raum-hex  $3 x  $4 z  $5 rot ; Umgebung: EXE EXIT_AT SERIE SEK SCALE EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c
SP="$WT/build/r34g_c"   # dort liegt nb_karte_raum.exe (aus nb_karte_raum.c uebersetzt)
EXE="${EXE:-$WT/re15_port/build_r34_c/platform/pc/re15_pc.exe}"
Z="$WT/build/r34g_c/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log wf.log fx.log state.log lauf_rc.txt f_*.ppm re15_card.mcr
"$SP/nb_karte_raum.exe" re15_card.mcr "$2" "$3" "$4" "${5:-0}" || exit 3
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_WINDOW_SCALE="${SCALE:-1}"
export RE15_WAFFEN_LOG="wf.log" RE15_STATE_LOG="state.log" RE15_FX_LOG="fx.log"
export RE15_EXIT_AT="${EXIT_AT:-60}#$2"
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="$SERIE:f_"
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
env | grep -E '^RE15_' | sort > env.txt; echo "EXE=$EXE" >> env.txt
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l) fx=$(wc -l < fx.log 2>/dev/null) wf=$(wc -l < wf.log 2>/dev/null)" | tee lauf_rc.txt
