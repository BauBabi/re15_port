#!/bin/bash
# Variante K5, ganze Kette (offline, nicht ins Spiel-Asset). Aus generator/:
#   bash k5_alles.sh fit        # Fusspose + vier Fits (parallel, je rund 10 min)
#   bash k5_alles.sh bau        # Bau, form_pruefung, Messung R1-R10, Bildleisten, Rundum-Tafeln, Vergleichstafel
# Fit-Ergebnisse liegen in ../_lauf/<V>/fit.json (nicht versioniert) und als Kopie in ../<V>/fit.json.
set -e
cd "$(dirname "$0")"
export PYTHONIOENCODING=utf-8
# Feld 5: rechter Maulwinkel (W) nicht links von x (k5_fit --rechts-min); K5b mit 205 = Kopfrand rechts (W), Lauf K5b6 (K5.md Abschnitt 3)
VARIANTEN="K5a:kurz:hals:25,10,40:214 K5b:lang:hals:30,50,15:205 K5c:kurz:rumpf:25,10,40:214 K5d:lang:rumpf:30,50,15:214"
if [ "$1" = fit ]; then
  python k5_fuss.py ../_lauf/fuss.json 700
  for v in $VARIANTEN; do
    IFS=: read -r N F4 F5 Y RM <<< "$v"
    ( python k5_fit.py --name "$N" --f4 "$F4" --f5 "$F5" --aus "../_lauf/$N/fit.json" --iters 5000,900,300,400 --yaws "$Y" --rechts-min "$RM" \
        > "../_lauf/$N.log" 2>&1 && python k5_arm.py "../_lauf/$N/fit.json" 350 >> "../_lauf/$N.log" 2>&1 ) &
  done
  wait
fi
if [ "$1" = bau ]; then
  cp ../_lauf/fuss.json ../fuss.json
  for v in $VARIANTEN; do
    IFS=: read -r N F4 F5 Y RM <<< "$v"
    python k5_bau.py "../_lauf/$N/fit.json" "../$N" --fuss ../_lauf/fuss.json
    cp "../_lauf/$N/fit.json" "../$N/fit.json"
    python k5_mass.py messen "../$N"
    python k5_blatt.py leiste "../leiste_$N.png" "$N=../$N"
    python k5_blatt.py rundum "../rundum_$N.png" "$N=../$N"
  done
  python k5_mass.py messen ../K4_heute --k4
  python k5_blatt.py rundum ../rundum_K4.png "K4=../K4_heute"
  python k5_blatt.py vergleich ../vergleich_bild2.png "K4 heute=../K4_heute" "K5a=../K5a" "K5b=../K5b" "K5c=../K5c" "K5d=../K5d"
fi
echo K5-FERTIG "$1"
