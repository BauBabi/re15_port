#!/usr/bin/env bash
# Gegenprobe: Cut 7 mit der ALTEN Freistellung 07_01.png neu bauen und die erzeugten
# Dateien byte-genau gegen den Auslieferungsstand haengen — beweist, dass --stufe 16
# --statistik max die Auslieferungsparameter sind (nicht angenommen, gemessen).
set -euo pipefail
cd "$(dirname "$0")/../../../.."
export PYTHONIOENCODING=utf-8
mkdir -p build/r19c/alt_MASKS build/r19c/bilder
cp re15_port/shared_assets/PSX/MASKS/* build/r19c/alt_MASKS/
python re15_port/tools/maske/raum.py ROOM10D0 7 \
    --stufe 16 --statistik max \
    --out build/r19c/alt_MASKS --bild build/r19c/bilder
echo "--- Hashes: neu gebaut (alte PNG) gegen Auslieferung"
md5sum build/r19c/alt_MASKS/ROOM10D0.MSK re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK
md5sum build/r19c/alt_MASKS/ROOM10D0_PRI07.TIM re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.TIM
md5sum build/r19c/alt_MASKS/ROOM10D0_PRI07.PBM re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM
md5sum build/r19c/alt_MASKS/ROOM10D0_PRI07.STAND re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.STAND
