#!/usr/bin/env bash
# Varianten, echtes Spiel: Nachstellung von Bild 1 in ROOM5080 Schnitt 1 (Figur auf der Blickachse, Abstand wie Bild 1, Pose 1; wie form_bemalt/fb_spiel.sh).
# bash vm_spiel.sh VARIANTE   -> ../VARIANTE/spiel_VARIANTE.png. Nur eigene PIDs (form_spiel.sh).
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
V=$1
D=$W/analysis/befunde_runde41/form/varianten/$V
cd "$W" || exit 2
bash re15_port/tools/form_spiel.sh --emd "$D/FORM.EMD" --tim "$D/FORM.TIM" --raum 5080 --pose "-20711,-15984,756,0,0,1" --aus "$D/spiel_$V.png" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild" | cut -c1-160
echo "rc=${PIPESTATUS[0]} $V"
