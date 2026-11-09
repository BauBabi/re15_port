#!/usr/bin/env bash
# K5f im echten Spielfenster (re15_port/tools/form_spiel.sh: eigene exe-Kopie, Variante ueber RE15_RE2_ASSET_ROOT, gdigrab bzw. PrintWindow,
# nur eigene PID; kein AUTOSHOT, kein SOFTWARE_RENDER). Spiel-Asset shared_assets/RE2 (heute K5e_f1) bleibt unberuehrt.
#   bash kf_k5f_fenster.sh [aus-Ordner]   (Standard ../k5f/spiel) -> k5f_*.png, heute_*.png
# Posen wie kf_k5e_spiel.sh: Bild 1 = Clip 0 Bild 11 (5080 Schnitt 1), Bild 2 = Clip 12 Bild 0 (5090 Schnitt 11), Griff = Clip 27 Bild 20 (Hals).
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
D=$W/analysis/befunde_runde41/form/varianten/K5/k5f/K5f
A=${1:-$W/analysis/befunde_runde41/form/varianten/K5/k5f/spiel}
mkdir -p "$A"
cd "$W" || exit 2
lauf1() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh --emd "$D/BAHNSTEIG_FORM.EMD" --tim "$D/BAHNSTEIG_FORM.TIM" "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|unbrauchbar\|kein Bild\|FAILED\|not loaded\|WARNUNG" | cut -c1-200; echo "rc=${PIPESTATUS[0]}"; }
lauf0() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|unbrauchbar\|kein Bild\|FAILED\|not loaded\|WARNUNG" | cut -c1-200; echo "rc=${PIPESTATUS[0]}"; }
lauf1 k5f_pose1_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf1 k5f_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf1 k5f_griff_r5090_s11.png --pose "941,-23776,3874,27,20,11"
lauf0 heute_pose1_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf0 heute_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf0 heute_griff_r5090_s11.png --pose "941,-23776,3874,27,20,11"
echo FENSTER-FERTIG
