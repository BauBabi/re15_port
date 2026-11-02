#!/usr/bin/env bash
cd "$(dirname "$0")"
B="C:/Program Files/Blender Foundation/Blender 5.2/blender.exe"
Z="$(pwd -W)/bild/ton"
BL="0,10;45,10;90,10;135,10;180,10;225,10;270,10;315,10;0,40;45,40;90,40;135,40;180,40;225,40;270,40;315,40;0,80"
for c in 0 1 2; do "$B" -b --python-exit-code 3 -P a3_ton.py -- $c "$BL" "$Z/ton" > mess/ton_$c.log 2>&1; echo ton $c rc=$?; done
for c in 0 1 2; do "$B" -b --python-exit-code 3 -P a3_ton.py -- $c "$BL" "$Z/far" farbig > mess/far_$c.log 2>&1; echo far $c rc=$?; done
echo TONREIHE-FERTIG
