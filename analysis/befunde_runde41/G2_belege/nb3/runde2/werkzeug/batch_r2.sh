#!/usr/bin/env bash
# G2-RE2 NB3 Runde 2 (Bauer): batch_r2.sh <lauf> <neu|alt> <mit|ohne> [FDR=<start-end/step>]
#   Umgebung wortgleich G2_belege/abn5/werkzeug/batch5.sh (Abnahme 5): west5_elza (Abnahme-4-Lauf), abn2wand_leon (exe-Riegel abn2_wand), oben_leon|oben_elza
#   (exe-Riegel rampe_oben), spawn_<x>_<z>_<leon|elza> (Figur oben per RE15_PLAYER_POS, Kampf sofort), rw_leon (exe-Riegel abn2_rampe_west, Eingabe Runde 2).
#   neu = re15_pc_g2nb3r2.exe (Bau 5a94392bf), alt = re15_pc_g2nb3r2_alt.exe (Runde-1-exe 029efac6 = Stand 10603425b). mit|ohne = Kanister (RE15_G2_OHNE_KANISTER=1).
SPW="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/r2"
W="$SPW/werkzeug"; A5="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn5r2/werkzeug"
EXE_DIR="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/platform/pc"
GEM=(RE15_NO_INTRO=1 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072")
BL=("${GEM[@]}" RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250)
BE=("${GEM[@]}" RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250)
L="$1"; ST="${2:-neu}"; K="${3:-ohne}"
X=(); if [ "$K" = "ohne" ]; then X=(RE15_G2_OHNE_KANISTER=1); fi
if [ "$ST" = "alt" ]; then export KOPIE="$EXE_DIR/re15_pc_g2nb3r2_alt.exe"; else export KOPIE="$EXE_DIR/re15_pc_g2nb3r2.exe"; fi
FD=(); if [ -n "${FDR:-}" ]; then FD=("RE15_FRAMEDUMP=$FDR:fd_"); fi
OBEN="W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170"
N="${ST}_${K}_${L}"
case "$L" in
west5_elza) bash "$W/lauf_r2.sh" "$N" "${BE[@]}" RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$(cat $A5/west_input.txt)" "RE15_EXIT_AT=${ENDE:-3300}#2091" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" ;;
abn2wand_leon) bash "$W/lauf_r2.sh" "$N" "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$(cat $A5/eingabe_abn2_wand.txt)" "RE15_EXIT_AT=${ENDE:-8400}#2090" ;;
oben_leon)  bash "$W/lauf_r2.sh" "$N" "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" "RE15_EXIT_AT=${ENDE:-4400}#2090" ;;
oben_elza)  bash "$W/lauf_r2.sh" "$N" "${BE[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=$OBEN" "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" "RE15_EXIT_AT=${ENDE:-4400}#2091" ;;
rw_leon)    bash "$W/lauf_r2.sh" "$N" "${BL[@]}" RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${X[@]}" "${FD[@]}" "RE15_INPUT_SCRIPT=W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W260" "RE15_K2080_RUECK_LAGE=2318,-18750,3072,2" "RE15_EXIT_AT=${ENDE:-8000}#2090" ;;
spawn_*)
    IFS=_ read -r _s sx sz wer <<< "$L"
    if [ "$wer" = "elza" ]; then J=(RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500 RE15_DEBUG_JUMP=2091@250); RM=2091;
    else J=(RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250); RM=2090; fi
    bash "$W/lauf_r2.sh" "$N" RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "${J[@]}" RE15_GB_TEST=1 "RE15_SET_FLAG=9:110,9:113" \
        "RE15_PLAYER_POS=$sx,$sz,${ROT:-1024},1" RE15_R39_MESS_SPIELER_HP=100 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log \
        "${FD[@]}" "RE15_EXIT_AT=${ENDE:-4200}#$RM" ;;
*) echo "unbekannt: $L"; exit 2 ;;
esac
echo BATCH_FERTIG
