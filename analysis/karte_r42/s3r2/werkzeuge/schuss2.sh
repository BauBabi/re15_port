#!/usr/bin/env bash
# Ein FB_SHOT-Lauf (echtes Spiel), Stufe 3 Runde 2: <tag> <raum> <x> <y> <z> <yaw> <gehen_sek> [besucht:/flag: ...]
# Lage = Ankunft einer Tuer (Door_aot_set next_pos + Blickrichtung). gehen_sek > 0: nach dem Laden so lange
# vorwaerts gehen (echte Bewegung mit Kollision), DANN Karte oeffnen - zeigt das Mitlaufen an einer echten Lage.
set -u
R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r2
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r42_karte
PROBE=$WT/re15_port/build/tests/unit/probe_r36_karte.exe
EXE=${EXE:-$R/exe/re15_pc.exe}
tag=$1; raum=$2; x=$3; y=$4; z=$5; yaw=$6; geh=$7; shift 7
W="${SCHUSS:-$R/schuss}/$tag"
mkdir -p "$W"
( cd "$W" && rm -f re15_card.mcr karte.bmp && "$PROBE" karte re15_card.mcr "$raum" "$x" "$y" "$z" "$yaw" besucht:$raum:$x:$z "$@" > probe.log 2>&1 )
if [ "$geh" != "0" ]; then
  SKRIPT="W1,XU$geh"; OPEN=$(( 40 + ${geh%.*} * 30 + 45 ))
  ( cd "$W" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
      RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
      RE15_INPUT_SCRIPT="$SKRIPT" RE15_INPUT_SCRIPT_START=0 RE15_INPUT_SCRIPT_BASIS=spiel \
      "RE15_INV_OPEN_AT=$OPEN#$raum" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=96 \
      "RE15_EXIT_AT=$((OPEN+220))#$raum" timeout ${TO:-170} "$EXE" > spiel.log 2>&1 )
else
  ( cd "$W" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
      RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
      "RE15_INV_OPEN_AT=40#$raum" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=96 \
      "RE15_EXIT_AT=206#$raum" timeout ${TO:-150} "$EXE" > spiel.log 2>&1 )
fi
if [ -f "$W/karte.bmp" ]; then echo "OK $tag"; else echo "OHNE ABZUG $tag"; fi
