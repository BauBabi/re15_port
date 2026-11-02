#!/usr/bin/env bash
# Abnahme 1: eigene Spielreihe. Nur eigene PIDs (form_spiel.sh), Laufordner danach geloescht. Kein RE-Wert.
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/rohbau_final
A=$W/analysis/befunde_runde41/form/rohbau_abnahme_1/spiel
mkdir -p "$A"
cd "$W" || exit 2
LOG="$A/reihe.log"
: > "$LOG"
lauf() {  # name raum pose
    echo "=== $1 raum $2 pose $3 $(date +%H:%M:%S)" >> "$LOG"
    o=$(bash re15_port/tools/form_spiel.sh --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" --raum "$2" --pose "$3" --aus "$A/$1.png" --nach 3 --zeit 110 2>&1)
    rc=$?
    echo "$o" | tr -d '\r' | tail -7 >> "$LOG"
    echo "rc=$rc" >> "$LOG"
    d=$(echo "$o" | tr -d '\r' | grep -a '^Laufverzeichnis: ' | sed 's/^Laufverzeichnis: //')
    case "$d" in
        */re15_port/build/form_spiel/*) cp "$d/debug.log" "$A/$1_debug.log" 2>/dev/null; rm -rf "$d" ;;
    esac
}
# Pose 2 am Passort, Schnitt 11: Wiederholung des Bauers
lauf p2_r5090_s11 5090 941,-23776,3871,1,0,11
# Pose 1 am Passort, alle fuenf Schnitte von ROOM5080. Cut 2 steht dem Auge von Bild 1 am naechsten (kamera.md)
for s in 0 1 2 3 4; do lauf p1_r5080_s$s 5080 -18564,-18386,88,0,0,$s; done
# Ruhe am Passort, Cut 2, viermal gedreht
for g in 88 1112 2136 3160; do lauf ruhe_r5080_s2_g$g 5080 -18564,-18386,$g,2,0,2; done
# Ruhe und Pose 2 in ROOM5090, Schnitt 11, gedreht
for g in 0 1024 2048 3072; do lauf ruhe_r5090_s11_g$g 5090 941,-23776,$g,2,0,11; done
# Pose 1 am Passort von Bild 2, zum Vergleich unter dem Licht von Schnitt 11
lauf p1_r5090_s11 5090 941,-23776,3871,0,0,11
echo FERTIG >> "$LOG"
