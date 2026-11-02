#!/usr/bin/env bash
# Abnahme 2: eigene Spielreihe. Nacheinander, nur form_spiel.sh, nur eigene PIDs. Jeder Laufordner wird nach dem Lauf geloescht.
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/rohbau_final
A=$W/analysis/befunde_runde41/form/rohbau_abnahme_2/spiel
cd "$W" || exit 2
gut() { python - "$1" <<'PY'
import sys, cv2
a = cv2.imread(sys.argv[1])
sys.exit(0 if a is not None and a.shape[:2] == (480, 640) and 8 < a.mean() < 170 else 1)
PY
}
lauf() {
  name=$1; shift
  for v in 1 2 3; do
    o=$(bash re15_port/tools/form_spiel.sh --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" "$@" --aus "$A/$name.png" --nach 5 2>&1)
    rc=$?
    echo "== $name v$v rc=$rc $* : $(echo "$o" | grep -a 'Verfahren\|Bahnsteig-Form\|typ 0x30' | tr '\n' ' ' | cut -c1-200)"
    d=$(echo "$o" | grep -a '^Laufverzeichnis' | sed 's/^Laufverzeichnis: //')
    [ -n "$d" ] && [ -d "$d" ] && rm -rf "$d"
    [ "$rc" = 0 ] && gut "$A/$name.png" && return 0
    echo "   Bild unbrauchbar, neuer Versuch"
  done
  return 1
}
# Pose 2 im Raum von Bild 2, Figur um die senkrechte gedreht (Gier 3874 + k*1024)
for k in 1 2 3; do g=$(( (3874 + k*1024) % 4096 )); lauf p2_dreh${k} --pose 941,-23776,$g,1,0,11; done
# Ruhe im selben Blick, vier Drehungen
for g in 0 1024 2048 3072; do lauf ruhe_g$g --pose 941,-23776,$g,2,0,11; done
# Pose 1 im selben Blick, vier Drehungen
for g in 0 1024 2048 3072; do lauf p1_g$g --pose 941,-23776,$g,0,0,11; done
# Nachstellung Bild 1 aus Abnahme 1, und Passlage aus form.json
lauf p1_nachstellung --raum 5080 --pose -20711,-15984,756,0,0,1
lauf p1_r5080_s1 --raum 5080 --pose -18564,-18386,80,0,0,1
lauf ruhe_nachstellung --raum 5080 --pose -20711,-15984,756,2,0,1
echo ALLES-FERTIG
