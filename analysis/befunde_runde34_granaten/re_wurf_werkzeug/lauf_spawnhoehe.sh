#!/usr/bin/env bash
# Runde 34 / Granate — MESSLAUF (nur lesen, kein Bau): Spawnpunkt der geworfenen Granate im PORT
# je Zielhoehe, um die Zeitlinie (wurf_sim.py) an eine konkrete Spawnhoehe h zu haengen.
# Der Port berechnet den Spawnpunkt wie das Original als R_knochen11 * Versatz + T_knochen11
# (game_step_common.c: re15_player_gunbone_world, Versaetze @0x800336d8-e8/@0x80033738-44/
# @0x80033794-a0). Die Hoehe ist damit eine PORT-Messung (Renderer-Knochen), keine Original-Adresse.
# ROOM1140 per Debug-Sprung, Waffe per RE15_GIVE/RE15_EQUIP (Mess-Harness main.c), R1 halten +
# Viereck (echte Pad-Bits). Oben/Unten waehrend HOLD waehlt die Zielhoehe (@0x800331e0-0x800332c8).
#   $1 Marke (MITTE|HOCH|TIEF)
set -uo pipefail
R=/c/workspace/git/reAi_v2
EXE="${EXE:-$R/re15_port/build/platform/pc/re15_pc.exe}"
case "$1" in
  MITTE) S="W1,M1,MA0.2,M2.5,W2" ;;
  HOCH)  S="W1,MU1,MUA0.2,MU2.5,W2" ;;
  TIEF)  S="W1,MD1,MDA0.2,MD2.5,W2" ;;
  *) echo "Marke?"; exit 2 ;;
esac
Z="$R/build/r34g_wurf/lauf_$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log wf.log fx.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_INPUT_SCRIPT_BASIS=spiel   # Skript-Zeitachse = Spielbilder (Vorspann-Laenge egal)
export RE15_DEBUG_JUMP="${JUMP:-1140@250}"
export RE15_GIVE="9:5" RE15_EQUIP="9"
export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-330}"
export RE15_INPUT_SCRIPT="$S"
export RE15_WAFFEN_LOG="wf.log"
export RE15_FX_LOG="fx.log"
export RE15_EXIT_AT="${EXIT_AT:-560}"
timeout -k 5 "${SEK:-150}" "$EXE" > stdout.txt 2> stderr.txt
echo "rc=$? wf=$(wc -l < wf.log 2>/dev/null) fx=$(wc -l < fx.log 2>/dev/null)" | tee lauf_rc.txt
