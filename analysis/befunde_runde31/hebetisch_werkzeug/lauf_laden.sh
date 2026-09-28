#!/usr/bin/env bash
# Runde 31 / H — LADE-WEG im echten Spiel: Spielstand in ROOM1150/1151 (probe_r30_granate_karte),
# CONTINUE, Hebetisch per RE15_FIRE_AOT=1@90 ausloesen; Vorbild
# analysis/befunde_runde30/nachtrag-granate_werkzeug/lauf_laden.sh. Echte exe, beschleunigter
# Renderer, Framedump, KEIN RE15_AUTOSHOT/RE15_SOFTWARE_RENDER.
#   $1 Marke (Unterordner von build/r31_hebetisch/)
#   Umgebung: RAUM (1150|1151), KARTE (Argumente des Karten-Werkzeugs), SERIE, EXIT_AT, SEK, SKALA,
#             SKRIPT (Eingabeskript ab Spielbild 0 nach dem Laden), EXTRA_ENV
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r31_hebetisch
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
WERKZEUG="${WERKZEUG:-$WT/re15_port/build/tests/unit/probe_r30_granate_karte.exe}"
RAUM="${RAUM:-1150}"
Z="$WT/build/r31_hebetisch/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm re15_card.mcr hebetisch.log
"$WERKZEUG" re15_card.mcr "$RAUM" ${KARTE:-} || exit 3
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_FIRE_AOT="1@90#$RAUM" RE15_FRAMEDUMP="${SERIE:-90-330/10}:f_" RE15_EXIT_AT="${EXIT_AT:-335#$RAUM}"
export RE15_HEBETISCH_LOG=hebetisch.log
if [ -n "${SKRIPT:-}" ]; then export RE15_INPUT_SCRIPT="$SKRIPT" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=0; fi
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-120}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" | tee lauf_rc.txt
grep -n -E "CONTINUE|granate\]|sicherung\] (Modal auf|Yes|No|Boot)|fire-aot" debug.log | head -12
