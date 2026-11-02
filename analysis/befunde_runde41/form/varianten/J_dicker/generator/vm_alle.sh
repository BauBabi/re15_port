#!/usr/bin/env bash
# Varianten, Abgabe: vier Varianten bauen (mit Helligkeitseichung und voller Pruefung), Uebersichten, Handwerksmessung, optional Spielbilder.
# bash vm_alle.sh [spiel]
# G: Ecken und Ringe aus vm_form WENIG_*, Nachpassung zw/delta.json wie A-C (vm_nach.py brachte im echten Mass nichts, VARIANTEN.md).
export PYTHONIOENCODING=utf-8
cd "$(dirname "$0")" || exit 2
bau() { python vm_bau.py "$@" --eichen 2>&1 | grep -a "^Bau\|Eichung\|Pruefung\|Fehler\|Error\|Traceback"; }
bau --farbe A --schwanz e9 --aus ../A_rueckrechnung --name A_rueckrechnung
bau --farbe B --schwanz e9 --aus ../B_screenshot --name B_screenshot
bau --farbe C --schwanz e9 --aus ../C_mitte --name C_mitte
bau --farbe C --schwanz e9 --flaechen wenig --aus ../G_flaechen --name G_flaechen
# D (E10): Farben aus den Screenshots, Wunden je Stelle geeicht (keine Hauteichung)
python vm_bau.py --farbe C --schwanz e9 --aus-bild --ms-sp 4 --ms-sr 16 --aus ../D_fleisch --name D_fleisch 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_nah.py ../D_fleisch/form.json ../D_fleisch/nah_D_fleisch.png "D Fleisch: Farben aus den Screenshots"
# E (E12): Farbfeld aus den Screenshots, gemaltes Detail, Wunden geeicht
python vm_bau.py --farbe C --schwanz e9 --ordnung --aus ../E_ordnung --name E_ordnung 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_vergleich.py ../E_ordnung/vergleich_D_E.png "D=../D_fleisch/form.json" "E=../E_ordnung/form.json"
python vm_chaos.py ../E_ordnung/chaos.json "C=../C_mitte/form.json" "D=../D_fleisch/form.json" "E=../E_ordnung/form.json" --re2
# F (E13): D ohne Pixel-Mischmasch
python vm_bau.py --farbe C --schwanz e9 --sauber --aus ../F_sauber --name F_sauber 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_vergleich.py ../F_sauber/vergleich_D_F.png "D=../D_fleisch/form.json" "F=../F_sauber/form.json"
python vm_chaos.py ../F_sauber/chaos.json "D=../D_fleisch/form.json" "E=../E_ordnung/form.json" "F=../F_sauber/form.json" --re2
# G_treu (E14): F, Zeichnung in den Farben des Screenshots, ohne gemalten Wundaufbau
python vm_bau.py --farbe C --schwanz e9 --treu --aus ../G_treu --name G_treu 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_stellen.py ../G_treu/stellen_G_F "G=../G_treu/form.json" "F=../F_sauber/form.json" > ../G_treu/stellen_G_F.txt
python vm_vergleich.py ../G_treu/vergleich_F_G.png "F=../F_sauber/form.json" "G=../G_treu/form.json"
# H (E15): Farbkarte aus G, Stoff-Schicht h1 prozedural, h2 aus RE2-Birkin-Material (Stil-Uebernahme)
for h in h1 h2; do python vm_bau.py --farbe C --schwanz e9 --stoff $h --aus ../H_stoff/$h --name H_$h 2>&1 | grep -a "^Bau\|Pruefung\|Fehler"; done
python vm_gezeichnet.py ../H_stoff/gezeichnet.json "G=../G_treu/form.json" "H1=../H_stoff/h1/form.json" "H2=../H_stoff/h2/form.json" --spiel
python vm_vergleich.py ../H_stoff/vergleich_G_H1_H2.png "G=../G_treu/form.json" "H1=../H_stoff/h1/form.json" "H2=../H_stoff/h2/form.json"
python vm_stellen.py ../H_stoff/stellen_H2_G "H2=../H_stoff/h2/form.json" "G=../G_treu/form.json" > ../H_stoff/stellen_H2_G.txt
python vm_stil.py ../H_stoff/stil.png "H2 Stoff aus RE2-Material=../H_stoff/h2/form.json" "H1 Stoff prozedural=../H_stoff/h1/form.json" "G treu (Vorstand)=../G_treu/form.json"
# I (E16): Beine und Fuesse nach den Nutzerteilen (zw/beine_I.json), Textur H2; G-Zwischenstufe fuer die Farbkarte
python vm_bau.py --farbe C --schwanz e9 --treu --beine --aus ../I_fuesse/_g --name I_g --ohne-pruefung 2>&1 | grep -a "Fehler\|Error\|Traceback"
python vm_bau.py --farbe C --schwanz e9 --stoff h2 --beine --g-textur ../I_fuesse/_g/textur.png --aus ../I_fuesse --name I_fuesse 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_beinblatt.py ../I_fuesse/form.json ../I_fuesse "H2 (vorher)=../H_stoff/h2/form.json"
# J (E18): wie I, Beine und Fuesse dicker (zw/beine_J.json, vm_beine.DICKER, vm_form.BREITE_FAKTOR), Breitenmass vm_breite.py
python vm_bau.py --farbe C --schwanz e9 --treu --beine --dicker --aus ../J_dicker/_g --name J_g --ohne-pruefung 2>&1 | grep -a "Fehler\|Error\|Traceback"
python vm_bau.py --farbe C --schwanz e9 --stoff h2 --beine --dicker --g-textur ../J_dicker/_g/textur.png --aus ../J_dicker --name J_dicker 2>&1 | grep -a "^Bau\|Pruefung\|Fehler\|Error\|Traceback"
python vm_breite.py ../J_dicker/breite.json "I=../I_fuesse/form.json" "J=../J_dicker/form.json"
python vm_beinblatt.py ../J_dicker/form.json ../J_dicker "I=../I_fuesse/form.json"
python vm_vergleich.py ../J_dicker/vergleich_I_J.png "I=../I_fuesse/form.json" "J=../J_dicker/form.json"
python vm_stil.py ../J_dicker/stil.png "J dicker=../J_dicker/form.json" "I Fuesse=../I_fuesse/form.json"
for d in A_rueckrechnung B_screenshot C_mitte D_fleisch E_ordnung F_sauber G_treu G_flaechen; do
    echo "== $d"; grep -a "Kennzahlen\|(a) Umriss\|Pruefpunkte\|(c) Boden\|(d) Zeichnung\|NEIN\|KERB" ../$d/pruefung/bericht.txt | cut -c1-150
done
python vm_matrix.py --aus .. \
  "Ausgang form_bemalt|alte Textur, alter Schwanz=../../form_bemalt/form.json" \
  "A Rueckrechnung|Textur RE2-Stil, Palette farbe_palette.json=../A_rueckrechnung/form.json" \
  "B Screenshot|Textur RE2-Stil, Farbton des Videos=../B_screenshot/form.json" \
  "C Mitte|Textur RE2-Stil, Mitte A/B=../C_mitte/form.json" \
  "D Fleisch (E10)|Farben aus den Screenshots, Wunden geeicht=../D_fleisch/form.json" \
  "F Sauber (E13)|wie D, ohne Pixel-Mischmasch=../F_sauber/form.json" \
  "G treu (E14)|wie F, Zeichnung in Bildfarben=../G_treu/form.json" \
  "H1 Stoff prozedural (E15)|Farbkarte G, Stoff-Schicht prozedural=../H_stoff/h1/form.json" \
  "H2 Stoff RE2-Material (E15)|Farbkarte G, Stoff aus RE2-Birkin (Stil-Uebernahme)=../H_stoff/h2/form.json" \
  "E Ordnung (E12)|Farbfeld aus den Screenshots, gemalt=../E_ordnung/form.json" \
  "G weniger Flaechen|wie C, 2100 statt 2398 Flaechen=../G_flaechen/form.json"
args=""
for d in A_rueckrechnung B_screenshot C_mitte D_fleisch E_ordnung F_sauber G_treu G_flaechen; do args="$args $d,../$d/FORM.EMD,../$d/FORM.TIM"; done
python re2_handwerk.py handwerk_varianten "form_bemalt,../../form_bemalt/FORM.EMD,../../form_bemalt/FORM.TIM" $args 2>&1 | grep -a "form_bemalt\|_rueck\|_screen\|_mitte\|_flaech"
if [ "${1:-}" = spiel ]; then
    for d in A_rueckrechnung B_screenshot C_mitte D_fleisch E_ordnung F_sauber G_treu G_flaechen I_fuesse J_dicker; do bash vm_spiel.sh "$d"; done
fi
echo VARIANTEN-FERTIG
