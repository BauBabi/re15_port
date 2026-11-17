#!/usr/bin/env bash
# K5f im echten Spiel messen (Messblatt K5F 1.1 / 4.1 / 4.4): Normal- und Schluessel-Lauf je Szene (Schluessel = jede CLUT-Farbe != 0 -> 0x7FFF,
# STP-Bit bleibt; Pixel/UV/Geometrie gleich), eigene exe-Kopie je Lauf (k5f/skripte/lauf.sh), echter Renderer, RE15_FRAMEDUMP
# (SDL_RenderReadPixels des Fensters), kein AUTOSHOT, kein SOFTWARE_RENDER, Ende per RE15_EXIT_AT bzw. nur ueber die eigene PID.
#   bash kf_k5f_spiel.sh <Variante-Ordner> <name>     -> Scratchpad k5f/spiel/{f80,f90,hp1}_<name>_{normal,key}
# Danach: python kf_k5f_spielmass.py <Variante-Ordner> <name> <aus.json>
set -u
V=$1; N=$2
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
HIER="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
LAUF="$HIER/../k5f/skripte/lauf.sh"
python "$HIER/kf_k5f_spielmass.py" --schluessel "$V" "$S/ueber/form_${N}" || exit 2
P1="-20711,-15984,756,0,11,1"; P2="941,-23776,3874,12,0,11"; PH="941,-23776,3874,27,20,11"; PP="RE15_PLAYER_POS=500,-9175,2048,0"
cd "$S" || exit 2
for a in normal key; do
  bash "$LAUF" f80_${N}_$a 5080 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P1 $PP RE15_RE2_ASSET_ROOT=$S/ueber/form_${N}_$a
  bash "$LAUF" f90_${N}_$a 5090 360-440/20 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$P2 $PP RE15_RE2_ASSET_ROOT=$S/ueber/form_${N}_$a
done
bash "$LAUF" hp1_${N}_normal 5090 400-440/40 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE=$PH $PP RE15_RE2_ASSET_ROOT=$S/ueber/form_${N}_normal
echo SPIEL-K5F-FERTIG
