#!/usr/bin/env bash
# Einbau v0.8.30 (E30): Spiellauf am ECHTEN Asset - re15_port/tools/form_spiel.sh OHNE --emd/--tim (keine RE15_RE2_ASSET_ROOT-Umleitung,
# das Spiel laedt shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM), eigene exe-Kopie, echtes Fenster (gdigrab/PrintWindow), nur eigene PID,
# kein AUTOSHOT, kein SOFTWARE_RENDER.
#   bash kf_einbau_spiel.sh [aus-Ordner]   (Standard ../einbau/spiel) -> einbau_*.png + lauf.log
# Posen im 28-Clip-Asset (STUFE3.md 3): Bild 1 = Clip 0 Bild 11, Bild 2 = Clip 12 Bild 0, Ruhe = Clip 2 Bild 0.
# Lagen: Nachstellung Bild 1 wie fb_spiel.sh / kf_spiel.sh (-20711,-15984, Gier 756, 5080 Schnitt 1); Bild 2 aus form.json (941,-23776, Gier 3874, 5090 Schnitt 11).
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
A=${1:-$W/analysis/befunde_runde41/form/varianten/K5/einbau/spiel}
mkdir -p "$A"
cd "$W" || exit 2
lauf() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild\|Laufverzeichnis\|FAILED\|not loaded" | cut -c1-200; echo "rc=${PIPESTATUS[0]}"; }
lauf einbau_pose1_nachstellung_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf einbau_ruhe_r5080_s1.png --raum 5080 --pose "-20711,-15984,756,2,0,1"
lauf einbau_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf einbau_ruhe_r5090_s11.png --pose "941,-23776,3874,2,0,11"
echo SPIEL-FERTIG
