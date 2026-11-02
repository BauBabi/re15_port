#!/usr/bin/env bash
# Form bemalt: Abgabe aus dem aktuellen Stand. bash fb_abgabe.sh TAG [spiel]
# Reihenfolge: Nachpassung ist schon in zw/delta.json. Stellschleife (Marken, Haut je Knochen, Hautton), Bau mit voller Pruefung, Blaetter, optional Spiel.
export PYTHONIOENCODING=utf-8
cd "$(dirname "$0")" || exit 2
T=${1:-ende}
python fb_stell.py stand_final.json 3 | grep -a "Hautton" | tail -1
python fb_bau.py stand_final.json --tag "$T" --voll | grep -a "Flaechen\|Farben"
python fb_neben.py "$T" .
cp "lauf/$T/rundum.png" "lauf/$T/rundum_hell.png" "lauf/$T/bericht.txt" "lauf/$T/teile_bild1.png" "lauf/$T/teile_bild2.png" .
cp zw/textur.png textur.png
md5sum FORM.EMD FORM.TIM
if [ "${2:-}" = spiel ]; then
    bash fb_spiel.sh spiel
    for f in spiel/spiel_*.png; do cp "$f" .; done
fi
echo ABGABE-FERTIG
