#!/usr/bin/env bash
# Nachtrag K (Granate) — LADE-WEG und INVENTAR im echten Spiel, Framedump (beschleunigter
# Renderer, KEIN RE15_SOFTWARE_RENDER/RE15_AUTOSHOT).
#   $1 Marke (Unterordner von build/r30_n_granate/)
#   $2 "fahrt"  Spielstand aus probe_r30_granate_karte (Argumente KARTE), CONTINUE, Hebetisch
#               per RE15_FIRE_AOT=1@90 ausloesen, Bilder SERIE
#      "check"  Granate in Inventarplatz 0 (KARTE enthaelt fach0), RE15_INV_CHECK_SHOT
#      "grid"   dito, RE15_INV_GRID_SHOT (Raster mit Icon und Menge)
#   Umgebung: RAUM (1150|1151), KARTE, SERIE, SEK, EXE, WERKZEUG
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r30_n_granate
EXE="${EXE:-$WT/re15_port/build/platform/pc/re15_pc.exe}"
WERKZEUG="${WERKZEUG:-$WT/re15_port/build/tests/unit/probe_r30_granate_karte.exe}"
RAUM="${RAUM:-1150}"
WIE="${2:-fahrt}"
Z="$WT/build/r30_n_granate/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log f_*.ppm re15_card.mcr inv.bmp
"$WERKZEUG" re15_card.mcr "$RAUM" ${KARTE:-} || exit 3
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
case "$WIE" in
  fahrt) export RE15_FIRE_AOT="1@90#$RAUM" RE15_FRAMEDUMP="${SERIE:-90-330/10}:f_" RE15_EXIT_AT="${EXIT_AT:-335#$RAUM}" ;;
  check) export RE15_INV_SHOT="inv.bmp" RE15_INV_CHECK_SHOT=1 RE15_FRAMEDUMP="136-144/4:f_" ;;
  grid)  export RE15_INV_SHOT="inv.bmp" RE15_INV_GRID_SHOT=1  RE15_FRAMEDUMP="48-52/2:f_" ;;
esac
timeout -k 5 "${SEK:-90}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" | tee lauf_rc.txt
grep -n -E "CONTINUE|granate\]|sicherung\] Modal|fire-aot|oid=0x07|CHECK-Foto" debug.log | head -12
