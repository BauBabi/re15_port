#!/bin/bash
# kette.sh <name> <framedump> [extra env]: ganze Kette 1130 -> 1150 -> ... -> 1150 Rueckkehr, Ende 1500#1150
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/spurL_n1"
name=$1; fd=$2; shift 2
W="$S/runs/$name"
( bash "$S/lauf.sh" "$name" 1130 "nach10f0 ersteszene" "1500#1150" "RE15_FRAMEDUMP=$fd" "$@" > "$S/runs/$name.lauf"; touch "$W/.fertig" ) &
sleep 3
bash "$S/waechter.sh" "$W"
wait
