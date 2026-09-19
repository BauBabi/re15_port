#!/bin/bash
# STAGE1-Bau Phase 3. $1 = "pruefen" (nichts schreiben) oder "schreiben".
cd "$(dirname "$0")/../.."
export PYTHONIOENCODING=utf-8
ARG=""
[ "$1" = "pruefen" ] && ARG="--nur-pruefen"
for R in 1000 1010 1050 1060 10A0 10C0 10D0 10E0 10F0 1110 1120 1130 1140 1160 1180 11B0 11D0 11F0 1220 1230; do
  echo "=== ROOM$R ==="
  python re15_port/tools/maske/raum.py ROOM$R --stufe 16 --statistik max \
      --dump build/p2/floor_p2.txt --bild build/p3/bilder $ARG 2>&1
done
echo "=== FERTIG ==="
