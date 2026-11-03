#!/usr/bin/env bash
# Nachbesserung 2, Textur-Spur: echte Fenster fuer die Texturbelege, je Satz (J / NB1 / NB2) und Ansicht ein Lauf (s3_n2_t_lauf.sh).
#   s3_n2_t_spielreihe.sh <satz>=<Ordner mit BAHNSTEIG_FORM.*> ...
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
for spec in "$@"; do
  satz="${spec%%=*}"; ordner="${spec#*=}"
  for a in "5080 0" "5080 2048" "5080 3072" "5090 3874" "5090 802"; do
    set -- $a; raum=$1; gier=$2
    if [ "$raum" = 5080 ]; then pose="-23500,-18200,$gier,2,0,4"; else pose="941,-23776,$gier,10,45,11"; fi
    bash "$HIER/s3_n2_t_lauf.sh" "tex_${satz}_${raum}_g${gier}" 12 2 1264 RE15_RE2_ASSET_ROOT="$ordner" RE15_DEBUG_JUMP=${raum}@120 \
      RE15_NOAUDIO=1 RE15_BB_POSE=$pose 2>&1 | grep -E "Bilder|Bahnsteig|^PID"
  done
done
echo SPIELREIHE-FERTIG
