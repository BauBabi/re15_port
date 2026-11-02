#!/usr/bin/env bash
cd "$(dirname "$0")"
B="C:/Program Files/Blender Foundation/Blender 5.2/blender.exe"
Z="$(pwd -W)/netz"
for c in 0 1 2; do "$B" -b --python-exit-code 3 -P a2_ton.py -- $c "0,-30;90,-30;180,-30;270,-30;0,75" "$Z" ohneboden tag:unt > mess/unt_$c.log 2>&1; echo unt $c rc=$?; done
echo RUND2-FERTIG
