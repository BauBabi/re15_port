#!/usr/bin/env bash
# Abnahme 7 Welle 1c: Tod-Probe neu (mit RE15_TITLE_SHOT, sonst blieb Leon im Titel) und W4-Abstieg per Aktionstaste A/B (Bauer-Umgebung w4_abstieg; neu = 6f4314fb, alt = re15_pc_g2nb4.exe 5910fc24 Stand 3d57083b1)
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7"
L="$SP/werkzeug/lauf7.sh"
W="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/platform/pc"
LIM_S=900 bash "$L" a7_tod_leon RE15_CLIMB_TRACE=climb.log RE15_G2_OHNE_KANISTER=1 RE15_G2_TRAG_LOG=1 RE15_GB_TEST=1 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_SET_FLAG=9:110,9:113 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=1800#2090 RE15_PLAYER_POS=-3000,-23000,0,0 "RE15_FRAMEDUMP=280-1800/10:fd_" > "$SP/w1c_tod_leon.txt" 2>&1 &
W4="RE15_CLIMB_TRACE=climb.log RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=400#2091 RE15_INPUT_SCRIPT=W2.0,A0.3,W4.0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_KEIN_GATOR=1 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PLAYER_POS=6732,-14968,3072,1 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_SET_FLAG=9:110,9:113 RE15_STATE_LOG=state.log RE15_TITLE_CONFIRM_MS=1500 RE15_WINDOW_SCALE=2"
LIM_S=600 bash "$L" a7_w4_abstieg_neu $W4 "RE15_FRAMEDUMP=60-130/2:fd_" > "$SP/w1c_w4_neu.txt" 2>&1 &
EXE_QUELLE="$W/re15_pc_g2nb4.exe" LIM_S=600 bash "$L" a7_w4_abstieg_alt $W4 > "$SP/w1c_w4_alt.txt" 2>&1 &
wait
echo "WELLE1C FERTIG $(date +%T)"
