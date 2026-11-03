#!/usr/bin/env bash
# Nachbesserung 2: ganze Kette vom Kopf-Fit bis zum Spiel-Asset (nachbaubar, deterministisch bis auf die Fits).
#   bash s3_n2_kette.sh <Fit-Lauf, z.B. K8>
# 1 Variante K4_kroko (k_variante.sh: Bau, Gelenkkoerper, Mundwinkel, Licht/Kiefer-Eichung, form_pruefung, Loecher mit alten Clips)
# 2 Textur-Basis stufe3/basis_n2 (s3_n2_textur.py bauen)   3 Clips + Asset (s3_clips.py), Kopie nach re15_port/shared_assets/RE2
# 4 Testkopf form_j_posen.h, Zahlenpruefung, Kopfmasse, Formpruefung am Spiel-Asset, Kiefer-/Krallenmasse, Loecher am Spiel-Asset
set -euo pipefail
export PYTHONIOENCODING=utf-8
FIT="$1"
HIER="$(cd "$(dirname "$0")" && pwd)"
FORM="$(cd "$HIER/../.." && pwd)"
REPO="$(cd "$FORM/../../.." && pwd)"
cd "$FORM/varianten/K_alligator/generator"
bash k_variante.sh "$FIT" ../K4_kroko
cd "$FORM"
python stufe3/gen/s3_n2_textur.py bauen varianten/K_alligator/K4_kroko stufe3/basis_n2 | tail -3
cd "$HIER"
python s3_clips.py
cp ../asset/BAHNSTEIG_FORM.EMD ../asset/BAHNSTEIG_FORM.TIM "$REPO/re15_port/shared_assets/RE2/"
python s3_testkopf.py
python s3_pruef.py --json ../nachbesserung2/pruef.json | tail -16
python s3_kopfmass.py --json ../nachbesserung2/kopfmass.json | head -1
cd "$FORM"
python ../../../re15_port/tools/form_pruefung.py pruefe stufe3/formpruefung.json --aus stufe3/formpruefung > stufe3/roh/n2_kopf/formpruefung.log 2>&1
grep -E "IoU|Pruefpunkte|Boden frei, Verdeckung|Zeichnung [0-9]" stufe3/formpruefung/bericht.txt | head -8
cd varianten/K_alligator/generator
python k_mass.py ../../../stufe3/nachbesserung2/mass_kiefer_krallen.json "K3=../K3_fit" "K4=../K4_kroko" | tail -2
python k_loecher.py messen ../../../stufe3/asset/BAHNSTEIG_FORM.EMD --name "NB2 Spiel-Asset K4" --aus ../../../stufe3/nachbesserung2/loecher/nachher.json | tail -2
echo KETTE-FERTIG
