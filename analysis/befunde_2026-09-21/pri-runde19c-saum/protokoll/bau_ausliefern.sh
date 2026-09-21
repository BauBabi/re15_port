#!/usr/bin/env bash
# Cut 7 in den AUSLIEFERUNGSBAUM schreiben — erst nachdem die Abnahme im Probebau stand
# (bau_neu.sh: Deckung fehlt 0 / zuviel 0, VORverd 0, 104 von 105 Rechtecken).
set -euo pipefail
cd "$(dirname "$0")/../../../.."
export PYTHONIOENCODING=utf-8
mkdir -p build/r19c/bilder_liefer
python re15_port/tools/maske/raum.py ROOM10D0 7 \
    --stufe 16 --statistik max \
    --bild build/r19c/bilder_liefer
echo "--- Auslieferungsdateien nach dem Schreiben"
md5sum re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK \
       re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.TIM \
       re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.PBM \
       re15_port/shared_assets/PSX/MASKS/ROOM10D0_PRI07.STAND
echo "--- gegen den Probebau (muss gleich sein)"
md5sum build/r19c/neu_MASKS/ROOM10D0.MSK build/r19c/neu_MASKS/ROOM10D0_PRI07.TIM
