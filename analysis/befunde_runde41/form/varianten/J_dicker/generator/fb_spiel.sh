#!/usr/bin/env bash
# Form bemalt: echtes Spiel, echtes Fenster (re15_port/tools/form_spiel.sh, Verfahren waehlt das Skript selbst). Nur eigene PIDs.
# bash fb_spiel.sh PRAEFIX [kurz]   kurz = nur die zwei Pflichtbilder
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/form_bemalt
P=${1:-spiel}
cd "$W" || exit 2
Z=$(python -c "
import json
j=json.load(open(r'$F/form.json'))
a,b=j['bild1'],j['bild2']
print(a['lage'][0],a['lage'][2],a['gier'],b['lage'][0],b['lage'][2],b['gier'])" | tr -d '\015')
read -r X1 Z1 G1 X2 Z2 G2 <<< "$Z"
lauf() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" "$@" --aus "$F/spiel/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild" | cut -c1-160; echo "rc=${PIPESTATUS[0]}"; }
# Pflicht (Auftrag 3): ROOM5090 Schnitt 11 (Bild 2, Pose 2 an der Lage aus form.json) und ROOM5080 Schnitt 1 (Pose 1)
lauf ${P}_pose2_r5090_s11.png --pose "$X2,$Z2,$G2,1,0,11"
lauf ${P}_pose1_r5080_s1.png --raum 5080 --pose "$X1,$Z1,$G1,0,0,1"
[ "${2:-}" = kurz ] && exit 0
lauf ${P}_ruhe_r5080_s1.png --raum 5080 --pose "$X1,$Z1,$G1,2,0,1"
# Nachstellung Bild 1 (Abnahme 1): Figur auf der Blickachse von Schnitt 1, Abstand wie Bild 1
lauf ${P}_pose1_nachstellung_r5080.png --raum 5080 --pose "-20711,-15984,756,0,0,1"
echo SPIEL-FERTIG
