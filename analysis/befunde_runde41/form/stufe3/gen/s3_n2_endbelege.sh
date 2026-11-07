#!/usr/bin/env bash
# Nachbesserung 2: Belege am Endstand (nach s3_n2_kette.sh). Textur: Bildmass + Heatmap J | NB1 | NB2 (7 Spielansichten, Buehne),
# echtes Fenster J | NB1 | NB2 (5 Ansichten); Kopf: echtes Fenster K3 | K4 (6 Ansichten), Masken-Overlay Bild 2, Rundum mit Spiel-Licht.
# J = Spiel-Asset der Abnahme 1 (stufe3/roh/n1_vorher), NB1 = Spiel-Asset vor dieser Nachbesserung (stufe3/roh/n2_kopf/asset_K3), NB2 = stufe3/asset.
set -uo pipefail
export PYTHONIOENCODING=utf-8
HIER="$(cd "$(dirname "$0")" && pwd)"
S3="$(cd "$HIER/.." && pwd -W)"      # Windows-Form (C:/...): Python liest die Pfade in den Satz-Angaben NAME=EMD,TIM,naht
REPO="$(cd "$S3/../../../.." && pwd)"
EXE="$REPO/re15_port/build/platform/pc/re15_pc.exe"
cd "$HIER"
J="J=$S3/roh/n1_vorher/BAHNSTEIG_FORM.EMD,$S3/roh/n1_vorher/BAHNSTEIG_FORM.TIM"
N1="NB1=$S3/roh/n2_kopf/asset_K3/BAHNSTEIG_FORM.EMD,$S3/roh/n2_kopf/asset_K3/BAHNSTEIG_FORM.TIM,$S3/basis_n1/naht.json"
N2="NB2=$S3/asset/BAHNSTEIG_FORM.EMD,$S3/asset/BAHNSTEIG_FORM.TIM,$S3/basis_n2/naht.json"
python s3_n2_textur.py bild "$J" "$N1" "$N2" --aus "$S3/nachbesserung2/textur/heatmap_J_NB1_NB2_endstand.png" --json "$S3/nachbesserung2/textur/bild_J_NB1_NB2_endstand.json" | tail -3
python s3_n2_t_tabelle.py "$S3/nachbesserung2/textur/bild_J_NB1_NB2_endstand.json" --md > "$S3/nachbesserung2/textur/tabelle_endstand.md"
python s3_n2_textur.py netz "$N2" --json "$S3/nachbesserung2/textur/netz_NB2_endstand.json" | tail -1
N2_EXE="$EXE" bash s3_n2_t_spielreihe.sh "NB2E=$S3/asset" | tail -1
python s3_n2_t_blatt.py echt "$S3/nachbesserung2/textur/echt_J_NB1_NB2_endstand.png" J NB1 NB2E | tail -1
N2_EXE="$EXE" bash s3_n2_kopfreihe.sh "K4=$S3/asset" | tail -1
python s3_n2_kopfblatt.py spiel "$S3/nachbesserung2/kopf/spielkamera_alt_neu.png"
python s3_n2_kopfblatt.py maske "$S3/nachbesserung2/kopf/bild2_masken_alt_neu.png"
python s3_n2_kopfblatt.py rundum "$S3/nachbesserung2/kopf/rundum_spiellicht_alt_neu.png"
echo ENDBELEGE-FERTIG
