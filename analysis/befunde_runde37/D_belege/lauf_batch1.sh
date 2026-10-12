#!/usr/bin/env bash
# Runde 37 Spur D: Messlaeufe 2090/2040/2030 nacheinander (eine Messkopie, kein paralleler Lauf).
D="$(cd "$(dirname "$0")" && pwd)"
L="$D/lauf_d.sh"
timeout 500 bash "$L" n2090_weg    "$D/wp_2090_weg.txt"    RE15_DEBUG_JUMP=20A0@250 RE15_PLAYER_POS=4950,4100,3072,2 RE15_EXIT_AT=900#2090
timeout 300 bash "$L" n2090_unten  "$D/wp_2090_unten.txt"  RE15_DEBUG_JUMP=2090@250 RE15_PLAYER_POS=6300,-16250,2048,0 RE15_EXIT_AT=700
timeout 500 bash "$L" n2040_weg    "$D/wp_2040_weg.txt"    RE15_DEBUG_JUMP=2040@250 RE15_PLAYER_POS=21700,-5400,2048,3 RE15_EXIT_AT=2400
timeout 300 bash "$L" n2040_galerie "$D/wp_2040_galerie.txt" RE15_DEBUG_JUMP=2040@250 RE15_PLAYER_POS=2200,-13800,1024,5 RE15_EXIT_AT=700
timeout 600 bash "$L" n2030_spas_weg "$D/wp_2030_spas_weg.txt" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,-100,3072,3 RE15_EXIT_AT=2600
timeout 300 bash "$L" n2030_spas_b3 "$D/wp_2030_spas_b3.txt" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=-650,14500,0,3 RE15_EXIT_AT=700
timeout 600 bash "$L" n2030_schrot_weg "$D/wp_2030_schrot_weg.txt" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,-100,1024,3 RE15_STAIR_DBG=1 RE15_EXIT_AT=3000
timeout 300 bash "$L" n2030_schrot_b3 "$D/wp_2030_schrot_b3.txt" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=4300,7100,0,3 RE15_EXIT_AT=700
echo BATCH1-FERTIG
