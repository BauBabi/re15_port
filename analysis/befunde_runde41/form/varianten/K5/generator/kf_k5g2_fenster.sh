#!/usr/bin/env bash
# K5g2 im echten Spielfenster (re15_port/tools/form_spiel.sh: eigene exe-Kopie, Variante ueber RE15_RE2_ASSET_ROOT, gdigrab auf das eigene
# Fenster ueber die eigene PID, nur eigene PID beendet; kein AUTOSHOT, kein SOFTWARE_RENDER). Spiel-Asset shared_assets/RE2 bleibt unberuehrt.
#   bash kf_k5g2_fenster.sh <Variante-Ordner> <aus-Ordner> <praefix>     -> <praefix>_*.png
# Posen wie kf_k5f_fenster.sh: Bild 1 = Clip 0 Bild 11 (5080 Schnitt 1), Bild 2 = Clip 12 Bild 0 (5090 Schnitt 11), Griff = Clip 27 Bild 20
# (Hals-Unterseite), dazu 5080 von vorn (Gier 2095, streifen/STREIFEN_HERKUNFT.md 2). Ohne Variante (leer) laeuft das Spiel-Asset.
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
D=$1; A=$2; P=$3
mkdir -p "$A"
cd "$W" || exit 2
lauf() { ziel=$1; shift; echo "=== $ziel $*"
  if [ -n "$D" ]; then set -- --emd "$D/BAHNSTEIG_FORM.EMD" --tim "$D/BAHNSTEIG_FORM.TIM" "$@"; fi
  bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|unbrauchbar\|kein Bild\|FAILED\|not loaded\|WARNUNG" | cut -c1-200
  echo "rc=${PIPESTATUS[0]}"; }
lauf ${P}_pose1_r5080.png --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf ${P}_vorn_r5080.png --raum 5080 --pose "-20711,-15984,2095,12,0,1"
lauf ${P}_pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
lauf ${P}_griff_r5090_s11.png --pose "941,-23776,3874,27,20,11"
echo FENSTER-FERTIG
