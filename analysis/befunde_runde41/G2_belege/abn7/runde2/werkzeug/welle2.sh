#!/usr/bin/env bash
# Abnahme 8 Welle 2: Rundweg frei4 (Abnahme-4-Eingaben, Kanister nach der Szene im Becken) und west4 (Abnahme-4-Eingabe, Figur an der Rampe) - Leon und Elza,
# Umgebung wortgleich Abnahme 7 / Bauer Runde 2; eigene Framedump-Fenster.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8"
L="$SP/werkzeug/lauf8.sh"
GEM="RE15_CLIMB_TRACE=climb.log RE15_EQUIP=3 RE15_G2_TRAG_LOG=1 RE15_GIVE=3:1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_K2080_GATOR_HP=24990 RE15_NO_INTRO=1 RE15_PLAYER_POS=-6000,-17500,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,3:245 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
ELZA="RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500"
FREI4_LEON=$(cat "$SP/werkzeug/frei4_leon_input.txt")
FREI4_ELZA=$(cat "$SP/werkzeug/frei4_elza_input.txt")
W4=$(cat "$SP/werkzeug/west4_input.txt")
bash "$L" a8_frei4_leon $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FREI4_LEON" "RE15_FRAMEDUMP=2830-2890/2:fd_" > "$SP/w2_frei4_leon.txt" 2>&1 &
bash "$L" a8_frei4_elza $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FREI4_ELZA" "RE15_FRAMEDUMP=130-200/2:fd_" > "$SP/w2_frei4_elza.txt" 2>&1 &
bash "$L" a8_west4_leon $GEM $LEON RE15_G2_ZEIT_LOG=1 RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=3300#2090 RE15_K2080_RUECK_LAGE=4200,-18800,3072,2 "RE15_INPUT_SCRIPT=$W4" > "$SP/w2_west4_leon.txt" 2>&1 &
bash "$L" a8_west4_elza $GEM $ELZA RE15_G2_ZEIT_LOG=1 RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=3300#2091 RE15_K2080_RUECK_LAGE=6400,-18700,3072,2 "RE15_INPUT_SCRIPT=$W4" > "$SP/w2_west4_elza.txt" 2>&1 &
wait
echo "WELLE2 FERTIG $(date +%T)"
