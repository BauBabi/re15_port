#!/usr/bin/env bash
# Endstand: Spielbilder der Abnahme-Tafel (Stufe M1 sichtbar in p1_g1024, ruhe_g1024). Nur form_spiel ueber zw/form_spiel_pw.sh, nur eigene PIDs, Laufordner danach geloescht.
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$W/analysis/befunde_runde41/form/rohbau_final
A=$F/abn2/spiel3
mkdir -p "$A"
cd "$W" || exit 2
lauf() {
  name=$1; shift
  for v in 1 2 3; do
    o=$(bash "$F/zw/form_spiel_pw.sh" --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" "$@" --aus "$A/$name.png" --nach 5 2>&1)
    rc=$?
    echo "== $name v$v rc=$rc $* : $(echo "$o" | grep -a 'Verfahren\|Bahnsteig-Form\|typ 0x30' | tr '\n' ' ' | cut -c1-200)"
    d=$(echo "$o" | grep -a '^Laufverzeichnis' | sed 's/^Laufverzeichnis: //')
    [ -n "$d" ] && [ -d "$d" ] && rm -rf "$d"
    [ "$rc" = 0 ] && python -c "import sys,cv2;a=cv2.imread(sys.argv[1]);sys.exit(0 if a is not None and 8<a.mean()<170 else 1)" "$A/$name.png" && return 0
  done
  return 1
}
lauf p1_g1024 --pose 941,-23776,1024,0,0,11
lauf ruhe_g1024 --pose 941,-23776,1024,2,0,11
lauf ruhe_g3072 --pose 941,-23776,3072,2,0,11
lauf p1_g3072 --pose 941,-23776,3072,0,0,11
echo SPIEL3-FERTIG
