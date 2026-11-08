#!/usr/bin/env bash
# Abnahme 8 Welle 5 (nach Welle 4): west5_elza mit Bildserie F1118-F1160 (Hochbiss beim Hinaufklettern F1126), (K) Rundweg mit Start W5.0 / W7.0 fuer Leon
# (frei4_leon-Eingabe) und Elza (frei4_elza-Eingabe).
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8"
L="$SP/werkzeug/lauf8.sh"
while ! grep -q "WELLE4 FERTIG" "$SP/welle4_ausgabe.txt" 2>/dev/null; do sleep 10; done
GEM="RE15_CLIMB_TRACE=climb.log RE15_EQUIP=3 RE15_G2_TRAG_LOG=1 RE15_GIVE=3:1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_K2080_GATOR_HP=24990 RE15_NO_INTRO=1 RE15_PLAYER_POS=-6000,-17500,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,3:245 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
ELZA="RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500"
FL=$(cat "$SP/werkzeug/frei4_leon_input.txt"); FE=$(cat "$SP/werkzeug/frei4_elza_input.txt"); W4=$(cat "$SP/werkzeug/west4_input.txt")
FL5="W5.0${FL#W6.0}"; FL7="W7.0${FL#W6.0}"; FE5="W5.0${FE#W6.0}"; FE7="W7.0${FE#W6.0}"
bash "$L" a8_west5_elza_kletter $GEM $ELZA RE15_G2_ZEIT_LOG=1 RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=1200#2091 RE15_K2080_RUECK_LAGE=3600,-18750,3072,2 "RE15_INPUT_SCRIPT=$W4" "RE15_FRAMEDUMP=1116-1160/1:fd_" > "$SP/w5_kletter.txt" 2>&1 &
bash "$L" a8_k_leon_w5 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FL5" > "$SP/w5_k_leon_w5.txt" 2>&1 &
bash "$L" a8_k_leon_w7 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FL7" > "$SP/w5_k_leon_w7.txt" 2>&1 &
bash "$L" a8_k_elza_w5 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FE5" > "$SP/w5_k_elza_w5.txt" 2>&1 &
bash "$L" a8_k_elza_w7 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FE7" > "$SP/w5_k_elza_w7.txt" 2>&1 &
wait
echo "WELLE5 FERTIG $(date +%T)"
