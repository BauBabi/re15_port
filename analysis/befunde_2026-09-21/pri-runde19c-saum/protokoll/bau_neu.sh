#!/usr/bin/env bash
# Cut 7 mit der SAUM-Freistellung bauen — dasselbe Werkzeug und dieselben Parameter, mit
# denen sich der Auslieferungsstand byte-genau reproduzieren liess (protokoll/bau_alt.sh).
# Ziel: build/r19c/neu_MASKS (der Auslieferungsbaum bleibt unberuehrt, bis die Abnahme steht).
set -euo pipefail
cd "$(dirname "$0")/../../../.."
export PYTHONIOENCODING=utf-8
mkdir -p build/r19c/neu_MASKS build/r19c/bilder_neu
cp re15_port/shared_assets/PSX/MASKS/* build/r19c/neu_MASKS/
python re15_port/tools/maske/raum.py ROOM10D0 7 \
    --stufe 16 --statistik max \
    --out build/r19c/neu_MASKS --bild build/r19c/bilder_neu
echo "--- Hashes neu gegen Auslieferung"
md5sum build/r19c/neu_MASKS/ROOM10D0.MSK re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK
md5sum build/r19c/neu_MASKS/ROOM10D0_PRI07.TIM re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.TIM
