#!/usr/bin/env bash
# K5e ruhige Haut: echtes Spiel, echtes Fenster (re15_port/tools/form_spiel.sh: eigene exe-Kopie, Variante ueber RE15_RE2_ASSET_ROOT,
# gdigrab bzw. PrintWindow, nur eigene PID; kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER). Das Spiel-Asset shared_assets/RE2 bleibt unberuehrt.
#   bash kf_k5e_spiel.sh [aus-Ordner]   (Standard ../ruhe/spiel) -> k5e_*.png, heute_*.png
# 10 Laeufe: K5e (28-Clip-Variante ruhe/K5e/BAHNSTEIG_FORM) in 6 Szenen, Asset heute (ohne Umleitung) in 4 Szenen.
# Posen im 28-Clip-Asset (STUFE3.md 3 / FARBE.md E.5): Bild 1 = Clip 0 Bild 11 (Nachstellung -20711,-15984, Gier 756, 5080 Schnitt 1),
# Bild 2 = Clip 12 Bild 0 (941,-23776, Gier 3874, 5090 Schnitt 11), Ruhe = Clip 2 Bild 0.
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
D=$W/analysis/befunde_runde41/form/varianten/K5/ruhe/K5e
A=${1:-$W/analysis/befunde_runde41/form/varianten/K5/ruhe/spiel}
mkdir -p "$A"
cd "$W" || exit 2
lauf1() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh --emd "$D/BAHNSTEIG_FORM.EMD" --tim "$D/BAHNSTEIG_FORM.TIM" "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild\|Laufverzeichnis\|FAILED\|not loaded\|WARNUNG" | cut -c1-200; echo "rc=${PIPESTATUS[0]}"; }
lauf0() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild\|Laufverzeichnis\|FAILED\|not loaded\|WARNUNG" | cut -c1-200; echo "rc=${PIPESTATUS[0]}"; }
lauf1 k5e_pose1_nachstellung_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf1 k5e_ruhe_r5080_s1.png --raum 5080 --pose "-20711,-15984,756,2,0,1"
lauf1 k5e_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf1 k5e_ruhe_r5090_s11.png --pose "941,-23776,3874,2,0,11"
lauf1 k5e_lauf_r5090_s11.png --pose "941,-23776,3874,10,-1,11"
lauf1 k5e_lauf_r5080_s1.png --raum 5080 --pose "-20711,-15984,756,0,-1,1"
lauf0 heute_pose1_nachstellung_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf0 heute_ruhe_r5080_s1.png --raum 5080 --pose "-20711,-15984,756,2,0,1"
lauf0 heute_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf0 heute_ruhe_r5090_s11.png --pose "941,-23776,3874,2,0,11"
echo SPIEL-FERTIG
