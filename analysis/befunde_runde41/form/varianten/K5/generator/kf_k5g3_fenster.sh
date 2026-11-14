#!/usr/bin/env bash
# K5g3 im echten Spielfenster fuer Tafel 04 (kf_k5g3_tafeln.py): je Pose ein Lauf mit generator/kf_k5g3_spiel.sh (eigene exe-Kopie, Variante ueber
# RE15_RE2_ASSET_ROOT, gdigrab NUR auf das eigene Fenster ueber die eigene PID, nur eigene PID beendet; kein AUTOSHOT, kein SOFTWARE_RENDER).
# RE15_FORCE_CUT ist Pflicht (K5g3/GEOMETRIE.md 5: sonst haelt die Kamerahalte-Regel jedes Bild). Die Spielerin steht ausserhalb des Bildes und
# schiesst nicht (RE15_INPUT_SCRIPT=W1), damit Muendungsfeuer das Licht nicht veraendert. Spiel-Asset shared_assets/RE2 bleibt unberuehrt.
#   bash kf_k5g3_fenster.sh <Variante-Ordner|-> <aus-Ordner> <praefix>     -> <aus>/<praefix>_{pose1_r5080,vorn_r5080,pose2_r5090_s11,griff_r5090_s11}.png
# Posen wie kf_k5g2_fenster.sh: Bild 1 = Clip 0 Bild 11 (5080 Schnitt 1), 5080 von vorn = Clip 12 Bild 0 (Gier 2095), Bild 2 = Clip 12 Bild 0
# (5090 Schnitt 11), Griff = Clip 27 Bild 20 (5090 Schnitt 11, Hals-Unterseite).
set -u
HIER="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
V=$1; A=$2; P=$3
mkdir -p "$A"
T=${K5G3_SCR:-/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/k5g3tex}/fenster_$$; mkdir -p "$T"
lauf() { ziel=$1; raum=$2; pose=$3; schnitt=$4
  bash "$HIER/kf_k5g3_spiel.sh" "$V" "$T/$ziel" "$raum" "$pose" "500,-9175,2048,0" 900 1 RE15_FORCE_CUT=$schnitt RE15_INPUT_SCRIPT=W1 2>&1 \
    | grep -a "exe Ende\|Bahnsteig-Form\|Bild " | head -3
  if [ -f "$T/$ziel/fenster_1.png" ]; then cp "$T/$ziel/fenster_1.png" "$A/${P}_$ziel.png"; echo "-> $A/${P}_$ziel.png"; else echo "KEIN BILD $ziel"; fi; }
lauf pose1_r5080 5080 "-20711,-15984,756,0,11,1" 1
lauf vorn_r5080 5080 "-20711,-15984,2095,12,0,1" 1
lauf pose2_r5090_s11 5090 "941,-23776,3874,12,0,11" 11
lauf griff_r5090_s11 5090 "941,-23776,3874,27,20,11" 11
rm -rf "$T"
echo FENSTER-FERTIG
