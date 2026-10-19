#!/bin/bash
# Nachbesserung 3 (N2): jeder Raum der Polizeistation ALLEIN besucht (Ankunftslagen der Tueren, Liste der Abnahme 2 =
# allein_liste.txt), echte exe, Kartenschirm, Abzug karte.bmp je Ort; danach tueren_bmp.py ueber alle Abzuege.
# allein_nb3.sh <ausgabe-ordner>
OUT="$1"
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten"
W="$WT/analysis/befunde_runde39/G_karten_werkzeug/nb3"
mkdir -p "$OUT"
while read tag r x y z; do
  [ -z "$tag" ] && continue
  b=$(( -y / 1800 )); [ $b -lt 0 ] && b=0
  bash "$W/lauf_nb3.sh" "$WT/re15_port/build/platform/pc/re15_pc.exe" "$OUT/al_$tag" $r 120 $r $x $y $z 0 "besucht:$r:$x:$z:$b" -- \
       RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=300#$r" > /dev/null
  [ -f "$OUT/al_$tag/karte.bmp" ] || echo "FEHLT al_$tag"
done < "$W/allein_liste.txt"
for d in "$OUT"/al_*; do [ -f "$d/karte.bmp" ] && python "$W/tueren_bmp.py" "$d/karte.bmp"; done > "$OUT/tueren_alle.txt" 2>&1
echo FERTIG
