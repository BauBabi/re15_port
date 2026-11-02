#!/usr/bin/env bash
# Rohbau B: Abgabe zusammenstellen. bash abschluss.sh TAG [spiel]
# Baut FORM.EMD/.TIM, form.json, prueft, kopiert die Blaetter, und fragt mit "spiel" das echte Spiel ab (Pose 2 ROOM5090 Schnitt 11, Pose 1 ROOM5080 Schnitt 1).
set -u
HIER="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -W)"
BAUM="$(cd -- "$HIER/../../../.." && pwd -W)"
TAG="${1:-ende}"
cd "$HIER" || exit 2
export PYTHONIOENCODING=utf-8
python bau.py --pruefe "$TAG" | tail -12 | cut -c1-110
L="lauf/$TAG"
cp "$L/blatt_bild1.png" vergleich_bild1.png
cp "$L/blatt_bild2.png" vergleich_bild2.png
cp "$L/ausschnitt_bild1.png" vergleich_bild1_nah.png
cp "$L/ausschnitt_bild2.png" vergleich_bild2_nah.png
cp "$L/teile_bild1.png" teile_bild1.png
cp "$L/teile_bild2.png" teile_bild2.png
cp "$L/rundum.png" rundum.png
cp "$L/rundum_hell.png" rundum_hell.png
cp "$L/bericht.txt" bericht.txt
grep -E "^\(|^Rahmen|^--- Bild|^  [a-z_A-Z]+ +[0-9,]+ +[0-9]+ +0\.|NEIN" bericht.txt | cut -c1-170 > ZAHLEN.txt
if [ "${2:-}" = spiel ]; then
    python - <<'XEOF' > lauf/lage.txt
import json
j = json.load(open('form.json'))
print(j['bild2']['lage'][0], j['bild2']['lage'][2], j['bild2']['gier'], j['bild1']['lage'][0], j['bild1']['lage'][2], j['bild1']['gier'])
XEOF
    read -r x2 z2 g2 x1 z1 g1 < <(tr -d '
' < lauf/lage.txt)
    cd "$BAUM" || exit 2
    bash re15_port/tools/form_spiel.sh --emd "$HIER/FORM.EMD" --tim "$HIER/FORM.TIM" --pose "$x2,$z2,$g2,1,0,11" --aus "$HIER/spiel_pose2_r5090_s11.png" --zeit 150 2>&1 | tail -7 | cut -c1-200
    bash re15_port/tools/form_spiel.sh --emd "$HIER/FORM.EMD" --tim "$HIER/FORM.TIM" --raum 5080 --pose "$x1,$z1,$g1,0,0,1" --aus "$HIER/spiel_pose1_r5080_s1.png" --zeit 150 2>&1 | tail -7 | cut -c1-200
fi
