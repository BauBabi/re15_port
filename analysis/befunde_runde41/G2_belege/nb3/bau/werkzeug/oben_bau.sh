#!/usr/bin/env bash
# NB3 Bauer: exe-Belegläufe Vorgabe A (Figur oben): Leon/Elza x mit/ohne Kanister, Eingabe wie der exe-Riegel rampe_oben (Klettern vor den Schüssen),
# Framedumps F0..F4400 je 30 Bilder; dazu Leon ohne Kanister mit der ALTEN exe (a661eb8aa) als "vorher" mit derselben Eingabe.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
W="$SP/werkzeug"
export SPW="$SP/bau_runs"
GEM=(RE15_NO_INTRO=1 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_G2_ZEIT_LOG=1)
BL=("${GEM[@]}" RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250)
BE=("${GEM[@]}" RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250)
OBEN="W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170"
FD="RE15_FRAMEDUMP=0-4400/30:fd_"
cd "$SP"
echo "O1 $(date +%T)"
bash "$W/lauf3.sh" a2_oben_leon_ohne "${BL[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "$FD" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" "RE15_EXIT_AT=4400#2090" > "$SPW/batch_a2_oben_leon_ohne.txt" 2>&1 &
bash "$W/lauf3.sh" a2_oben_leon_neu  "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "$FD" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" "RE15_EXIT_AT=4400#2090" > "$SPW/batch_a2_oben_leon_neu.txt" 2>&1 &
bash "$W/lauf3.sh" a2_oben_elza_ohne "${BE[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "$FD" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" "RE15_EXIT_AT=4400#2091" > "$SPW/batch_a2_oben_elza_ohne.txt" 2>&1 &
wait
echo "O2 $(date +%T)"
bash "$W/lauf3.sh" a2_oben_elza_neu  "${BE[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "$FD" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" "RE15_EXIT_AT=4400#2091" > "$SPW/batch_a2_oben_elza_neu.txt" 2>&1 &
EXE_SRC="$SP/exe_alt/re15_pc_alt_a661eb8aa.exe" bash "$W/lauf3.sh" a2_oben_leon_ohne_alt "${BL[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "$FD" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" "RE15_EXIT_AT=4400#2090" > "$SPW/batch_a2_oben_leon_ohne_alt.txt" 2>&1 &
wait
echo "FERTIG $(date +%T)"
for n in a2_oben_leon_ohne a2_oben_leon_neu a2_oben_elza_ohne a2_oben_elza_neu a2_oben_leon_ohne_alt; do echo "== $n: $(cat "$SPW/runs/$n/pid.txt" | tr '\n' ' ') exe $(cut -c1-8 "$SPW/runs/$n/exe_md5.txt")"; done
