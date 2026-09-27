#!/bin/sh
# Sichtpruefung Welle 2: ein GETROFFENER Zombie im KLEIN-Zustand (Kriecher-Box -350/350)
# in ROOM1140. Der Spieler zielt (R1) und feuert in Press-Flanken; nach dem Niederschlag
# liegt der Zombie am Boden — genau der Zustand, in dem EXEC[5] P2 @0x80103464/6C die Box
# auf -350/350 setzt und die Rampe @0x8010366C-94 sie wieder aufzieht.
# KEIN RE15_AUTOSHOT (maskiert Bugs), kein Softwarerenderer.
export PATH="/c/msys64/mingw64/bin:/c/msys64/usr/bin:$PATH"
export RE15_NOAUDIO=1
export RE15_NO_INTRO=1
mkdir -p shots/r31
export RE15_TITLE_SHOT=shots/r31/title.ppm
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1140@240"

# Feuer-Flanken: SQUARE muss losgelassen werden, sonst gibt es nur EINE Press-Flanke.
SCRIPT="M1"
i=0
while [ $i -lt 60 ]; do
    SCRIPT="$SCRIPT,MA0.15,M0.25"
    i=$((i + 1))
done
export RE15_INPUT_SCRIPT="$SCRIPT"
export RE15_INPUT_SCRIPT_START=60
export RE15_FRAMEDUMP="300-1100/40:shots/r31/zombie_"
timeout -k 5 130 ./re15_port/build/platform/pc/re15_pc.exe >shots/r31/run.log 2>&1
echo "RC=$?"
ls shots/r31 | head -40
