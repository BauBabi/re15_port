#!/usr/bin/env bash
# Sicht-Pruefer Einbau Runde 1: eigene Spiellaeufe am echten Asset (ohne Umleitung) und eine Gegenprobe der 17-Zeilen-Variante (Umleitung).
W=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form
A=$W/analysis/befunde_runde41/form/varianten/K5/einbau/sicht
V=$W/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v3
cd "$W" || exit 2
lauf() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild\|Laufverzeichnis\|FAILED\|not loaded\|\[tim\]" | cut -c1-220; echo "rc=${PIPESTATUS[0]}"; }
# Asset (shared_assets/RE2), Lagen der Vorlage-Tafel 05 (form.json bild1: -18564,-18386, Gier 80) und Nachstellung
lauf sicht_ruhe_r5080_s1.png      --raum 5080 --pose "-18564,-18386,80,2,0,1" --env "RE15_TIM_LOG=1"
lauf sicht_pose1_r5080_s1.png     --raum 5080 --pose "-18564,-18386,80,0,11,1"
lauf sicht_nachst_r5080.png       --raum 5080 --pose "-20711,-15984,756,0,11,1"
lauf sicht_pose2_r5090_s11.png    --pose "941,-23776,3874,12,0,11" --bilder 3
lauf sicht_ruhe_r5090_s11.png     --pose "941,-23776,3874,2,0,11" --bilder 3
# Gegenprobe: 17-Zeilen-Variante (3 Clips: 0 Bild1, 1 Bild2, 2 Ruhe) ueber Umleitung, gleiche Kette
lauf var17_ruhe_r5080_s1.png      --emd "$V/FORM.EMD" --tim "$V/FORM.TIM" --raum 5080 --pose "-18564,-18386,80,2,0,1"
lauf var17_pose2_r5090_s11.png    --emd "$V/FORM.EMD" --tim "$V/FORM.TIM" --pose "941,-23776,3874,1,0,11" --bilder 3
lauf var17_ruhe_r5090_s11.png     --emd "$V/FORM.EMD" --tim "$V/FORM.TIM" --pose "941,-23776,3874,2,0,11" --bilder 3
echo SICHT-FERTIG
