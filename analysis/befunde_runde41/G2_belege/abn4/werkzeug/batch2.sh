#!/usr/bin/env bash
# G2 Abnahme 4: Fensterlaeufe (Eingaben/Umgebung der Abnahme-4-Laeufe = G2_belege/v4/werkzeug/batch.sh des Bauers, wortgleich) ueber lauf2.sh (Fenster oben rechts),
# Stand "neu" (mit Kanister) / "ohne" (Messhaken RE15_G2_OHNE_KANISTER=1); dazu je Lauf eine RE15_FRAMEDUMP-Serie (PPM je Spielbild, fd_NNNNNN.ppm) fuer F-genaue Bildleisten.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4"
W="$SP/werkzeug"
BL=(RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_WINDOW_SCALE=2 RE15_G2_ZEIT_LOG=1)
BE=(RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_WINDOW_SCALE=2 RE15_G2_ZEIT_LOG=1)
IN="$(cat $W/west_input.txt)"
P="${2:-neu}"; X=()
if [ "$P" = "ohne" ]; then X=(RE15_G2_OHNE_KANISTER=1); fi
FD="${3:-}"; FDA=(); if [ -n "$FD" ]; then FDA=("RE15_FRAMEDUMP=$FD"); fi
case "${1:-a}" in
frei4_leon) bash "$W/lauf2.sh" ${PFX:-}${P}_frei4_leon 285 "${BL[@]}" "${X[@]}" "${FDA[@]}" "RE15_INPUT_SCRIPT=$(cat $W/frei_leon4.txt)" "RE15_EXIT_AT=8400#2090" ;;
west4_leon) bash "$W/lauf2.sh" ${PFX:-}${P}_west4_leon 125 "${BL[@]}" "${X[@]}" "${FDA[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2090" "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" ;;
west4_elza) bash "$W/lauf2.sh" ${PFX:-}${P}_west4_elza 125 "${BE[@]}" "${X[@]}" "${FDA[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2091" "RE15_K2080_RUECK_LAGE=6400,-18700,3072,2" ;;
west5_elza) bash "$W/lauf2.sh" ${PFX:-}${P}_west5_elza 125 "${BE[@]}" "${X[@]}" "${FDA[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2091" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" ;;
frei4_elza) bash "$W/lauf2.sh" ${PFX:-}${P}_frei4_elza 285 "${BE[@]}" "${X[@]}" "${FDA[@]}" "RE15_INPUT_SCRIPT=$(cat $W/frei_elza4.txt)" "RE15_EXIT_AT=8400#2091" ;;
esac
echo BATCH_FERTIG
