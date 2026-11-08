#!/usr/bin/env bash
# G2 Abnahme 6: batch6.sh <lauf> <neu|ohne> [alt|r1]   ("alt" = Messkopie a661eb8aa d872c8e6, "r1" = Runde 1 10603425b 029efac6)
#  (c) west4_leon / west4_elza / west5_elza / frei4_leon / frei4_elza = Abnahme-4-Laeufe (Umgebung wortgleich G2_belege/abn4/werkzeug/batch2.sh bzw.
#      abn5/werkzeug/batch5.sh, ohne gdigrab)
#  (a1) oben_leon / oben_elza = Szenario des exe-Riegels rampe_oben(_elza) (test_r41_gatorfix.cmake TEIL rampe_oben); ENDE = Abbruchbild (Vorgabe 4400)
#  (b) kante_<x>_<z>_<cut>_<leon|elza> = eigene Rueckkehr-Lage im Wasser vor einer Blockkante, Figur steht (Schuesse wie alle Raeume, dann W150); KROT = Blick
#  (a2) spawn_<x>_<z>_<leon|elza> = eigene Lage OBEN (RE15_PLAYER_POS Band 1 = y -1800, Blick ROT, Vorgabe 1024), Szene gesehen + Tor offen (9:110,9:113),
#       Kampf sofort (RE15_GB_TEST=1 wie oben_kante / re2gator), Figur steht
#  (d) abn2wand_leon = exe-Riegel abn2_wand (Eingabe wortgleich), abn2rw_alt_leon = alte Eingabe von abn2_rampe_west (vor aa41ec95e), Abbruch Bild 1300
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6"
W="$SP/werkzeug"
EXE_DIR="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/platform/pc"
GEM=(RE15_NO_INTRO=1 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_G2_ZEIT_LOG=1)
BL=("${GEM[@]}" RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250)
BE=("${GEM[@]}" RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250)
IN="$(cat $W/west_input.txt)"
P="${2:-neu}"; X=()
if [ "$P" = "ohne" ]; then X=(RE15_G2_OHNE_KANISTER=1); fi
PF=""
if [ "${3:-}" = "alt" ]; then export KOPIE="$EXE_DIR/re15_pc_g2abn6_alt.exe"; PF="alt_"; fi
if [ "${3:-}" = "r1" ];  then export KOPIE="$EXE_DIR/re15_pc_g2abn6_r1.exe"; PF="r1_"; fi
FD=(); if [ -n "${FDR:-}" ]; then FD=("RE15_FRAMEDUMP=$FDR:fd_"); fi
OBEN="W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170"
STEH="W6.0,M0.6,MA0.1,M0.3,MA0.1,W150"
L="${1:-}"
case "$L" in
west4_leon) bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BL[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2090" "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" ;;
west4_elza) bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BE[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2091" "RE15_K2080_RUECK_LAGE=6400,-18700,3072,2" ;;
west5_elza) bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BE[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_EXIT_AT=3300#2091" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" ;;
frei4_leon) bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BL[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$(cat $W/frei_leon4.txt)" "RE15_EXIT_AT=8400#2090" ;;
frei4_elza) bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BE[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$(cat $W/frei_elza4.txt)" "RE15_EXIT_AT=8400#2091" ;;
oben_leon)  bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" "RE15_EXIT_AT=${ENDE:-4400}#2090" ;;
oben_elza)  bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BE[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" "RE15_EXIT_AT=${ENDE:-4400}#2091" ;;
abn2wand_leon)
            bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$(cat $W/eingabe_abn2_wand.txt)" "RE15_EXIT_AT=${ENDE:-8400}#2090" ;;
abn2rw_alt_leon)
            bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$IN" "RE15_K2080_RUECK_LAGE=4200,-18700,3072,2" "RE15_EXIT_AT=1300" ;;
kante_*)
    IFS=_ read -r _k kx kz kc wer <<< "$L"
    if [ "$wer" = "elza" ]; then B=("${BE[@]}"); RM=2091; else B=("${BL[@]}"); RM=2090; fi
    bash "$W/lauf6.sh" ${PF}${P}_$L${SUF:-} "${B[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$STEH" "RE15_K2080_RUECK_LAGE=$kx,$kz,${KROT:-3072},$kc" "RE15_EXIT_AT=${ENDE:-3600}#$RM" ;;
spawn_*)
    IFS=_ read -r _s sx sz wer <<< "$L"
    if [ "$wer" = "elza" ]; then J=(RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250); RM=2091;
    else J=(RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250); RM=2090; fi
    bash "$W/lauf6.sh" ${PF}${P}_${L}_r${ROT:-1024}${SUF:-} RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${J[@]}" RE15_GB_TEST=1 "RE15_SET_FLAG=9:110,9:113" \
        "RE15_PLAYER_POS=$sx,$sz,${ROT:-1024},1" RE15_R39_MESS_SPIELER_HP=100 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log \
        "${X[@]}" "${FD[@]}" "RE15_EXIT_AT=${ENDE:-4200}#$RM" ;;
*) echo "unbekannt: $L"; exit 2 ;;
esac
echo BATCH_FERTIG
