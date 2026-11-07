#!/usr/bin/env bash
# Rohbau final: aus einem Stand alles bauen und messen. bash rf_abschluss.sh STAND.json
# Reihenfolge: Ruhepose, Marken-Stellschleife, Bau mit voller Pruefung, Abgabe-Blaetter, Spiel, Messreihe der drei Urteile.
export PYTHONIOENCODING=utf-8
cd "$(dirname "$0")" || exit 2
S=$1
cp "$S" stand_final.json
rm -f zw/marken.json
python rf_ruhe.py stand_final.json | head -5
python rf_stell.py stand_final.json 5 | tail -2 | cut -c1-430
rm -rf lauf/ende
python rf_bau.py stand_final.json --tag ende --voll | tail -3 | cut -c1-70
python rf_abgabe.py ende | tail -12
python rf_biege.py form.json biegeprobe.png
python rf_ton.py
md5sum FORM.EMD FORM.TIM
bash rf_spiel.sh 2>&1 | grep -a "Verfahren\|kein Bild\|unbrauchbar\|Bahnsteig-Form\|typ 0x30" | cut -c1-140
python rf_o1.py
cd mess
python m2_gelenke.py > m2.txt 2>&1
python m3_innen.py > m3.txt 2>&1
python m5_paare.py > m5.txt 2>&1
python m10_kanten.py > m10.txt 2>&1
cd spiel
python u_netz.py > netz.txt 2>&1
python u_biege.py > biege.txt 2>&1
python u_krater.py > krater.txt 2>&1
python u_loch.py > loch.txt 2>&1
python u_spalt.py > spalt.txt 2>&1
cd ../bild
python einsatz.py > e.txt 2>&1
python auflage.py > a.txt 2>&1
python marken.py > marken.txt 2>&1
python keil.py > keil.txt 2>&1
python kontur.py > kontur.txt 2>&1
cd ../../abn
python abn_bild.py > mess/bild.txt 2>&1
python abn_fenster.py > mess/fenster.txt 2>&1
python abn_dach.py > mess/dach.txt 2>&1
python abn_kante.py > mess/kante.txt 2>&1
python abn_probe.py > mess/probe_aus.txt 2>&1
python abn_rumpf.py > mess/rumpf_aus.txt 2>&1
python abn_teilrest.py > mess/teilrest_aus.txt 2>&1
python abn_gelenk.py > mess/gelenk_aus.txt 2>&1
python abn_netz.py > mess/netz_aus.txt 2>&1
python abn_nah.py > mess/nah_aus.txt 2>&1
python abn_kamton.py > mess/kamton_aus.txt 2>&1
python abn_dachlinie.py > mess/dachlinie.txt 2>&1
echo ABSCHLUSS FERTIG
