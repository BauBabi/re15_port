#!/bin/bash
# Variante K, eine Kopfform von Anfang bis Ende: Bau ohne Naht (fuer die Abstimmung), Abstimmung der Gelenkkoerper (Kugeln am Eltern, Muffen am Kind)
# gegen die Screenshots (Naeherung im Tiefenpuffer, dann je Koerper am echten Pruefweg von form_pruefung nachgeprueft), Mundwinkel-Koerper so klein,
# dass er Ober-/Unterkiefer in Bild 2 nicht verdeckt, Endbau (Lichtmessung, Kiefer-Eichung, form_pruefung), Stufe-3-Fassung, Loch-Messung ueber 28 Clips.
#   bash k_variante.sh K3 ../K3_fit
set -e
cd "$(dirname "$0")"
K=$1; AUS=$2; NAME=$(basename "$AUS")
export PYTHONIOENCODING=utf-8
python k_bau.py --kopf ../_lauf/$K/kopf.json --aus ../_lauf/${K}_ohne_naht --name ${NAME}_ohne_naht --schnell --ohne-pruefung
python k_naht.py ../_lauf/${K}_ohne_naht ../_lauf/${K}_naht_proxy.json
python k_naht.py nachpruefen ../_lauf/${K}_ohne_naht ../_lauf/${K}_naht_proxy.json ../_lauf/${K}_naht_geprueft.json
python k_maulwinkel.py ../_lauf/$K/kopf.json ../_lauf/${K}_naht_geprueft.json ../_lauf/${K}_naht.json
python k_bau.py --kopf ../_lauf/$K/kopf.json --aus "$AUS" --name "$NAME" --naht ../_lauf/${K}_naht.json
cp ../_lauf/${K}_naht.json "$AUS/naht.json"
cp ../_lauf/$K/kopf.json "$AUS/kopf.json"
python k_s3.py "$AUS" ../_lauf/${K}_s3.EMD
python k_loecher.py messen ../_lauf/${K}_s3.EMD --name "$NAME (Stufe-3-Clips)" --aus "$AUS/loecher_stufe3.json"
echo "VARIANTE-FERTIG $NAME"
