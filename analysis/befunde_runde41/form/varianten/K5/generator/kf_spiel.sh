#!/usr/bin/env bash
# K5c Farbe (Runde 1, ab Runde 2 mit Vergleich K4 heute): echtes Spiel, echtes Fenster (re15_port/tools/form_spiel.sh: eigene exe-Kopie, Kandidat ueber RE15_RE2_ASSET_ROOT,
# gdigrab bzw. PrintWindow, nur eigene PID; kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER). Spiel-Asset bleibt unberuehrt.
#   bash kf_spiel.sh [Variantenordner] [--mit-heute]   (Standard ../farbe/K5c_farbe_v2) -> ../farbe/spiel/*.png
#   --mit-heute: dieselben Szenen zusaetzlich ohne Kandidat (Spiel-Asset shared_assets/RE2 = K4 heute) -> ../farbe/spiel/heute_*.png
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
D=$W/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v2
HEUTE=0
for a in "$@"; do case "$a" in --mit-heute) HEUTE=1 ;; *) D=$a ;; esac; done
A=$W/analysis/befunde_runde41/form/varianten/K5/farbe/spiel
mkdir -p "$A"
cd "$W" || exit 2
Z=$(python -c "
import json
j=json.load(open(r'$D/form.json'))
a,b=j['bild1'],j['bild2']
print(a['lage'][0],a['lage'][2],a['gier'],b['lage'][0],b['lage'][2],b['gier'])" | tr -d '\015')
read -r X1 Z1 G1 X2 Z2 G2 <<< "$Z"
lauf1() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh --emd "$D/FORM.EMD" --tim "$D/FORM.TIM" "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild" | cut -c1-160; echo "rc=${PIPESTATUS[0]}"; }
lauf0() { ziel=$1; shift; echo "=== heute_$ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/heute_$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild" | cut -c1-160; echo "rc=${PIPESTATUS[0]}"; }
lauf() { lauf1 "$@"; [ $HEUTE = 1 ] && lauf0 "$@"; return 0; }
# Lagen aus form.json (K5c = J): Bild 2 Pose Clip 1 in ROOM5090 Schnitt 11, Bild 1 Pose Clip 0 in ROOM5080 Schnitt 1, Ruhe Clip 2 in beiden;
# Nachstellung Bild 1 wie fb_spiel.sh (Figur auf der Blickachse von Schnitt 1, Lage -20711,-15984, Gier 756).
lauf spiel_pose2_r5090_s11.png --pose "$X2,$Z2,$G2,1,0,11"
lauf spiel_ruhe_r5090_s11.png --pose "$X2,$Z2,$G2,2,0,11"
lauf spiel_pose1_r5080_s1.png --raum 5080 --pose "$X1,$Z1,$G1,0,0,1"
lauf spiel_ruhe_r5080_s1.png --raum 5080 --pose "$X1,$Z1,$G1,2,0,1"
lauf spiel_pose1_nachstellung_r5080.png --raum 5080 --pose "-20711,-15984,756,0,0,1"
echo SPIEL-FERTIG
