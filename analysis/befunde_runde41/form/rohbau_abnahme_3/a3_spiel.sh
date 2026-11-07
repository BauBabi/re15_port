#!/usr/bin/env bash
# Abnahme 3: eigene Spiellaeufe mit form_spiel.sh (Original-Skript, Verfahren waehlt es selbst). Nur eigene PIDs, das Skript beendet nur seine.
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/rohbau_final
A=$W/analysis/befunde_runde41/form/rohbau_abnahme_3/spiel
cd "$W" || exit 2
lauf() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" "$@" --aus "$A/$ziel" --nach 6 2>&1 | tail -6; echo "rc=${PIPESTATUS[0]}"; }
# Bild 2: Pose 2 an der Lage aus form.json, Schnitt 11
lauf p2_s11.png --pose "941,-23776,3874,1,0,11"
# Bild 1 hat keinen Schnitt: Nachstellung aus Abnahme 1 in ROOM5080 Schnitt 1
lauf p1_nach_r5080.png --raum 5080 --pose "-20711,-15984,756,0,0,1"
# Rundgang im Spiel, Schnitt 11: Pose 1, Pose 2, Ruhe je in vier Gierwinkeln
for c in 0 1 2; do for g in 0 1024 2048 3072; do lauf c${c}_g${g}_s11.png --pose "941,-23776,$g,$c,0,11"; done; done
echo SPIELREIHE-FERTIG
