#!/usr/bin/env bash
cd "$(dirname "$0")"
B="C:/Program Files/Blender Foundation/Blender 5.2/blender.exe"
Z="$(pwd -W)/netz"
BL="0,8;45,8;90,8;135,8;180,8;225,8;270,8;315,8;0,35;45,35;90,35;135,35;180,35;225,35;270,35;315,35"
for c in 0 1 2; do "$B" -b --python-exit-code 3 -P a2_ton.py -- $c "$BL" "$Z" tag:ton > mess/ton_$c.log 2>&1; echo clip $c rc=$?; done
BF="0,12;90,12;180,12;270,12"
for c in 0 1 2; do "$B" -b --python-exit-code 3 -P a2_ton.py -- $c "$BF" "$Z" farbig tag:far > mess/far_$c.log 2>&1; echo farbig $c rc=$?; done
echo TONREIHE-FERTIG
