#!/usr/bin/env bash
# Abnahme 1, zweite Spielreihe: Bild-1-Blick in Cut 1 nachgestellt und Rundgang im Spiel. Kein RE-Wert.
# Abstand 5976 wie Bild 1 (kamera_bild1.json gegen form.json), Lage auf der Blickachse von Cut 1, Gier so, dass die Figur der Kamera dieselbe Seite zeigt: 88 - (-37 - 21,7) Grad = 756.
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/rohbau_final
A=$W/analysis/befunde_runde41/form/rohbau_abnahme_1/spiel2
mkdir -p "$A"
cd "$W" || exit 2
LOG="$A/reihe.log"
: > "$LOG"
lauf() {
    echo "=== $1 raum $2 pose $3 $(date +%H:%M:%S)" >> "$LOG"
    o=$(bash re15_port/tools/form_spiel.sh --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" --raum "$2" --pose "$3" --aus "$A/$1.png" --nach 3 --zeit 110 2>&1)
    rc=$?
    echo "$o" | tr -d '\r' | tail -7 >> "$LOG"
    echo "rc=$rc" >> "$LOG"
    d=$(echo "$o" | tr -d '\r' | grep -a '^Laufverzeichnis: ' | sed 's/^Laufverzeichnis: //')
    case "$d" in
        */re15_port/build/form_spiel/*) rm -rf "$d" ;;
    esac
}
X=-20711; Z=-15984; G=756
lauf b1blick_pose1 5080 $X,$Z,$G,0,0,1
for k in 0 1 2 3 4 5 6 7; do g=$(( (G + k*512) & 4095 )); lauf rund_ruhe_$k 5080 $X,$Z,$g,2,0,1; done
for k in 1 2 3; do g=$(( (G + k*1024) & 4095 )); lauf rund_pose1_$k 5080 $X,$Z,$g,0,0,1; done
for k in 0 1 2 3; do g=$(( (G + k*1024) & 4095 )); lauf rund_pose2_$k 5080 $X,$Z,$g,1,0,1; done
echo FERTIG >> "$LOG"
