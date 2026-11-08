#!/usr/bin/env bash
# NB3 Bauer: exe-Belegläufe (Messer-Skripte batch3.sh/lauf3.sh, eigener Laufordner bau_runs; die Messer-Läufe in runs/ bleiben als "vorher").
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
W="$SP/werkzeug"
export SPW="$SP/bau_runs"
mkdir -p "$SPW"
cd "$SP"
echo "G1 $(date +%T)"
bash "$W/batch3.sh" oben_leon neu  > "$SPW/batch_oben_leon_neu.txt" 2>&1 &
bash "$W/batch3.sh" oben_leon ohne > "$SPW/batch_oben_leon_ohne.txt" 2>&1 &
bash "$W/batch3.sh" oben_elza ohne > "$SPW/batch_oben_elza_ohne.txt" 2>&1 &
wait
echo "G2 $(date +%T)"
bash "$W/batch3.sh" rampe_leon ohne > "$SPW/batch_rampe_leon_ohne.txt" 2>&1 &
bash "$W/batch3.sh" rampe_leon neu  > "$SPW/batch_rampe_leon_neu.txt" 2>&1 &
bash "$W/batch3.sh" rampe_elza ohne > "$SPW/batch_rampe_elza_ohne.txt" 2>&1 &
wait
echo "G3 $(date +%T)"
bash "$W/batch3.sh" wand_elza ohne > "$SPW/batch_wand_elza_ohne.txt" 2>&1 &
bash "$W/batch3.sh" wand_elza neu  > "$SPW/batch_wand_elza_neu.txt" 2>&1 &
bash "$W/batch3.sh" oben_elza neu  > "$SPW/batch_oben_elza_neu.txt" 2>&1 &
wait
echo "G4 $(date +%T)"
# Leisten Plattform-Suedkante: rampe_leon ohne Kanister mit Framedumps, neue exe und alte exe (a661eb8aa = Messer-Stand d872c8e6)
GEM=(RE15_NO_INTRO=1 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_G2_ZEIT_LOG=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250)
RAMPE="W6.0,M0.6,MA0.1,M0.3,MA0.1,W22.0,A0.3,W2.0,A0.3,W2.0,XU0.8,W120"
bash "$W/lauf3.sh" fd_rampe_leon_ohne_neu "${GEM[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "RE15_FRAMEDUMP=2300-3000/10:fd_" "RE15_INPUT_SCRIPT=$RAMPE" "RE15_K2080_RUECK_LAGE=3000,-18800,3072" "RE15_EXIT_AT=3600#2090" > "$SPW/batch_fd_rampe_leon_ohne_neu.txt" 2>&1 &
EXE_SRC="$SP/exe_alt/re15_pc_alt_a661eb8aa.exe" bash "$W/lauf3.sh" fd_rampe_leon_ohne_alt "${GEM[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "RE15_FRAMEDUMP=2300-3000/10:fd_" "RE15_INPUT_SCRIPT=$RAMPE" "RE15_K2080_RUECK_LAGE=3000,-18800,3072" "RE15_EXIT_AT=3600#2090" > "$SPW/batch_fd_rampe_leon_ohne_alt.txt" 2>&1 &
wait
echo "FERTIG $(date +%T)"
for f in "$SPW"/batch_*.txt; do echo "== $f"; tail -3 "$f"; done
