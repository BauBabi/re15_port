#!/usr/bin/env bash
# Ein FB_SHOT-Lauf (echtes Spiel): <tag> <raum> <x> <z> <shot_at> [Sonden-Argumente: besucht:/flag:...]
set -u
R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r42_karte
PROBE=$WT/re15_port/build/tests/unit/probe_r36_karte.exe
EXE=${EXE:-$R/exe/re15_pc.exe}
tag=$1; raum=$2; x=$3; z=$4; at=$5; shift 5
W="${SCHUSS:-$R/schuss}/$tag"
mkdir -p "$W"
( cd "$W" && rm -f re15_card.mcr karte.bmp && "$PROBE" karte re15_card.mcr "$raum" "$x" 0 "$z" 0 besucht:$raum:$x:$z "$@" > probe.log 2>&1 )
( cd "$W" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    "RE15_INV_OPEN_AT=${OPEN:-40}#$raum" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=$at \
    "RE15_EXIT_AT=${EXITN:-$((at+110))}#$raum" timeout ${TO:-150} "$EXE" > spiel.log 2>&1 )
if [ -f "$W/karte.bmp" ]; then echo "OK $tag"; else echo "OHNE ABZUG $tag"; fi
