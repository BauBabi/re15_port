#!/usr/bin/env bash
# Nachbesserung 2, Kopf: echte Fenster fuer das Kopfblatt (s3_n2_kopfblatt.py spiel), je Satz und Ansicht ein Lauf (s3_n2_t_lauf.sh,
# eigene exe-Kopie, gdigrab, nur eigene PID). Asset je Satz ueber RE15_RE2_ASSET_ROOT.
#   s3_n2_kopfreihe.sh <satz>=<Ordner mit BAHNSTEIG_FORM.*> ...
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
for spec in "$@"; do
  satz="${spec%%=*}"; ordner="${spec#*=}"
  for a in "5080 3072" "5080 2560" "5080 2048" "5090 3874" "5090 0" "5090 802"; do
    set -- $a; raum=$1; gier=$2
    if [ "$raum" = 5080 ]; then pose="-23500,-18200,$gier,2,0,4"; else pose="941,-23776,$gier,10,45,11"; fi
    bash "$HIER/s3_n2_t_lauf.sh" "kopf_${satz}_${raum}_g${gier}" 12 2 1264 RE15_RE2_ASSET_ROOT="$ordner" RE15_DEBUG_JUMP=${raum}@120 \
      RE15_NOAUDIO=1 RE15_BB_POSE=$pose 2>&1 | grep -E "Bilder|Bahnsteig|^PID"
  done
done
echo KOPFREIHE-FERTIG
