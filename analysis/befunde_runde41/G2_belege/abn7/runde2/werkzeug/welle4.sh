#!/usr/bin/env bash
# Abnahme 8 Welle 4 (nach Welle 3), in drei Gruppen zu je hoechstens 4 Laeufen:
#  A: nische neu (EXIT_AT 780 im Finisher, sonst ueberschreiben die Folgeraeume die Bildnummern), frei4_elza mit Bildserie F890-F956 (Schub im Sub 3),
#     (K) Rundweg: Leon frei4_leon mit Start W4.0 / W8.0
#  B: (K) Rundweg: Elza frei4_elza mit Start W4.0 / W8.0, Leon mit der Elza-Eingabe, Elza mit der Leon-Eingabe ueber 8400 Bilder
#  C: (W)/(A) eigene Oben-Lagen an den Kanten (Spawn auf dem Deck, Kampf sofort, ohne Kanister), Leon und Elza
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8"
L="$SP/werkzeug/lauf8.sh"
while ! grep -q "WELLE3 FERTIG" "$SP/welle3_ausgabe.txt" 2>/dev/null; do sleep 10; done
GEM="RE15_CLIMB_TRACE=climb.log RE15_EQUIP=3 RE15_G2_TRAG_LOG=1 RE15_GIVE=3:1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_K2080_GATOR_HP=24990 RE15_NO_INTRO=1 RE15_PLAYER_POS=-6000,-17500,3072 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,3:245 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
ELZA="RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500"
FL=$(cat "$SP/werkzeug/frei4_leon_input.txt")
FE=$(cat "$SP/werkzeug/frei4_elza_input.txt")
FL4="W4.0${FL#W6.0}"; FL8="W8.0${FL#W6.0}"; FE4="W4.0${FE#W6.0}"; FE8="W8.0${FE#W6.0}"
# --- A
LIM_S=600 bash "$L" a8_nische2 RE15_CLIMB_TRACE=climb.log RE15_G2_TRAG_LOG=1 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=2 RE15_DEBUG_JUMP=2090@250 RE15_GB_TEST=1 RE15_SET_FLAG=9:110,9:113 RE15_R41_HP_AT=175:60 "RE15_GATOR_KANISTER=-6000,-15000" RE15_STATE_LOG=state.log "RE15_EXIT_AT=780#2090" "RE15_FRAMEDUMP=276-540/2:fd_" > "$SP/w4_nische2.txt" 2>&1 &
bash "$L" a8_frei4_elza_schub $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=1000#2091 "RE15_INPUT_SCRIPT=$FE" "RE15_FRAMEDUMP=890-956/2:fd_" > "$SP/w4_frei4_elza_schub.txt" 2>&1 &
bash "$L" a8_k_leon_w4 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FL4" > "$SP/w4_k_leon_w4.txt" 2>&1 &
bash "$L" a8_k_leon_w8 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FL8" > "$SP/w4_k_leon_w8.txt" 2>&1 &
wait
echo "GRUPPE A FERTIG $(date +%T)"
# --- B
bash "$L" a8_k_elza_w4 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FE4" > "$SP/w4_k_elza_w4.txt" 2>&1 &
bash "$L" a8_k_elza_w8 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FE8" > "$SP/w4_k_elza_w8.txt" 2>&1 &
bash "$L" a8_k_leon_eingabe_elza $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=8400#2090 "RE15_INPUT_SCRIPT=$FE" > "$SP/w4_k_leon_eingabe_elza.txt" 2>&1 &
bash "$L" a8_k_elza_eingabe_leon $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=8400#2091 "RE15_INPUT_SCRIPT=$FL" > "$SP/w4_k_elza_eingabe_leon.txt" 2>&1 &
wait
echo "GRUPPE B FERTIG $(date +%T)"
# --- C
SPAWN="RE15_CLIMB_TRACE=climb.log RE15_G2_OHNE_KANISTER=1 RE15_G2_TRAG_LOG=1 RE15_GB_TEST=1 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,9:113 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
bash "$L" a8_spawn_nord_elza_r1024 $SPAWN $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4200#2091 RE15_PLAYER_POS=3000,-14950,1024,1 > "$SP/w4_s1.txt" 2>&1 &
bash "$L" a8_spawn_sued_leon_r1024 $SPAWN $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=4200#2090 RE15_PLAYER_POS=5800,-17650,1024,1 "RE15_FRAMEDUMP=355-400/1:fd_" > "$SP/w4_s2.txt" 2>&1 &
bash "$L" a8_spawn_ost_leon_r3072 $SPAWN $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=4200#2090 RE15_PLAYER_POS=6600,-15800,3072,1 > "$SP/w4_s3.txt" 2>&1 &
bash "$L" a8_spawn_mitte_elza_r2048 $SPAWN $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4200#2091 RE15_PLAYER_POS=4500,-16000,2048,1 > "$SP/w4_s4.txt" 2>&1 &
wait
echo "WELLE4 FERTIG $(date +%T)"
