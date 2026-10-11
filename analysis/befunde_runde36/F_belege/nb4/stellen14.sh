#!/bin/bash
# alle 14 exe-Stellen der Abnahme 3 mit Variante $1, 4 parallel
cd "$(dirname "$0")"
v="$1"
printf '%s\n' "-7000,12500" "-14000,2000" "6000,-9000" "-24000,10000" "7000,4000" "9000,10000" "-14746,-4140" "7200,-4130" "-14000,4000" "-14500,6000" "-24452,11938" "-22288,11938" "-26500,15500" "-20000,14500" | xargs -P 4 -I{} bash run.sh "affen_{}" "$v" RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS={},1024,0 RE15_EXIT_AT=2700
echo FERTIG >> runs.log
