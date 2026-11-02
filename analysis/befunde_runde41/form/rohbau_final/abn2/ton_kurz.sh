#!/usr/bin/env bash
# Tonbilder aus clip*.obj in Blender: ton_kurz.sh tag clip "az,el;az,el" [ziel:x,y,z,s]
cd "$(dirname "$0")"
B="C:/Program Files/Blender Foundation/Blender 5.2/blender.exe"
Z="$(pwd -W)/netz"
"$B" -b --python-exit-code 3 -P a2_ton.py -- "$2" "$3" "$Z" "tag:$1" ${4:+"$4"} > mess/ton_$1.log 2>&1
echo "$1 rc=$?"
