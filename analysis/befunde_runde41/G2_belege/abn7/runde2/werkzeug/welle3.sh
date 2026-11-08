#!/usr/bin/env bash
# Abnahme 8 Welle 3 (nach Welle 2): west5_elza (Abnahme-4-Eingabe), ohne_frei4_leon / ohne_oben_elza (RE15_G2_OHNE_KANISTER=1, Vergleich mit Abnahme 6/7 bis zum
# ersten Treffer), nische (Umgebung wortgleich Riegel integration_r41_re2gator Lauf nische, Bildserie Intro-Sperre + erster Biss), aufn_elza (Elza mit der
# Leon-Eingabe frei4, Bauer-Lauf r2_aufn_elza).
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8"
L="$SP/werkzeug/lauf8.sh"
while ! grep -q "WELLE2 FERTIG" "$SP/welle2_ausgabe.txt" 2>/dev/null; do sleep 10; done
GEM="RE15_CLIMB_TRACE=climb.log RE15_EQUIP=3 RE15_G2_TRAG_LOG=1 RE15_GIVE=3:1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_K2080_GATOR_HP=24990 RE15_NO_INTRO=1 RE15_PLAYER_POS=-6000,-17500,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,3:245 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
ELZA="RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500"
OBEN="W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170"
FREI4_LEON=$(cat "$SP/werkzeug/frei4_leon_input.txt")
W4=$(cat "$SP/werkzeug/west4_input.txt")
bash "$L" a8_west5_elza $GEM $ELZA RE15_G2_ZEIT_LOG=1 RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=3300#2091 RE15_K2080_RUECK_LAGE=3600,-18750,3072,2 "RE15_INPUT_SCRIPT=$W4" > "$SP/w3_west5_elza.txt" 2>&1 &
bash "$L" a8_ohne_frei4_leon $GEM $LEON RE15_G2_OHNE_KANISTER=1 RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FREI4_LEON" > "$SP/w3_ohne_frei4_leon.txt" 2>&1 &
bash "$L" a8_ohne_oben_elza $GEM $ELZA RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4400#2091 RE15_K2080_RUECK_LAGE=3600,-18750,3072,2 "RE15_INPUT_SCRIPT=$OBEN" "RE15_FRAMEDUMP=740-800/2:fd_" > "$SP/w3_ohne_oben_elza.txt" 2>&1 &
LIM_S=900 bash "$L" a8_nische RE15_CLIMB_TRACE=climb.log RE15_G2_TRAG_LOG=1 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=2 RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_SET_FLAG=9:110,9:113 RE15_R41_HP_AT=175:60 "RE15_GATOR_KANISTER=-6000,-15000" RE15_STATE_LOG=state.log "RE15_EXIT_AT=800#2090" "RE15_FRAMEDUMP=276-540/2:fd_" > "$SP/w3_nische.txt" 2>&1 &
bash "$L" a8_aufn_elza $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=1400#2091 "RE15_INPUT_SCRIPT=$FREI4_LEON" "RE15_FRAMEDUMP=1190-1250/2:fd_" > "$SP/w3_aufn_elza.txt" 2>&1 &
wait
echo "WELLE3 FERTIG $(date +%T)"
