#!/bin/sh
# Sichtpruefung Runde 28: Messer-Dauerschlag nach unten in ROOM1140.
# KEIN RE15_AUTOSHOT (maskiert Bugs) — RE15_FRAMEDUMP liefert den komplett komponierten Frame.
export PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH"
export RE15_NOAUDIO=1
export RE15_NO_INTRO=1
export RE15_TITLE_SHOT=shots/r28/title.ppm
export RE15_DEBUG_JUMP="1140@gp"
export RE15_INPUT_SCRIPT="W2,MD1,MDA30"     # R1 halten, RUNTER, dann Dauerschlag
export RE15_INPUT_SCRIPT_START=40
export RE15_FRAMEDUMP="120-900/60:shots/r28/nach_"
timeout -k 5 110 ./re15_port/build/platform/pc/re15_pc.exe >/dev/null 2>&1
echo "RC=$?"
