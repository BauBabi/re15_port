#!/usr/bin/env bash
# NB3 Bauer: Cut-2-Lage der Suedkante (west4_leon, RUECK_LAGE 4200,-18800,3072,2) als exe-Lauf, alter Stand a661eb8aa (exe-Kopie d872c8e6) | neu (5581916f),
# gleiche Eingabe wie der exe-Riegel rampe (test_r41_gatorfix.cmake, Leon ohne Kanister), Werkzeug des Messers lauf3.sh (eigene exe-Kopie je Lauf, eigene PID).
# Zweck: die Riegel-Lage rampe (3000,-18800, Cut 0) zeigt Figur und Alligator nicht im Bild - die Cut-2-Lage fuer die Bildleiste nb3_bau_suedkante_cut2_leon.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
W="$SP/werkzeug"
export SPW="$SP/final_runs"
GEM=(RE15_NO_INTRO=1 "RE15_SET_FLAG=9:110,3:245" "RE15_GIVE=3:1" RE15_EQUIP=3 RE15_R39_MESS_SPIELER_HP=100 RE15_K2080_GATOR_HP=24990 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_STATE_LOG=state.log RE15_G2_TRAG_LOG=1 RE15_CLIMB_TRACE=climb.log "RE15_PLAYER_POS=-6000,-17500,3072" RE15_G2_ZEIT_LOG=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=2090@250)
RAMPE="W6.0,M0.6,MA0.1,M0.3,MA0.1,W22.0,A0.3,W2.0,A0.3,W2.0,XU0.8,W120"
cd "$SP"
bash "$W/lauf3.sh" kante2_leon_ohne "${GEM[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "RE15_FRAMEDUMP=100-3600/50:fd_" "RE15_INPUT_SCRIPT=$RAMPE" "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" "RE15_EXIT_AT=3600#2090" > "$SPW/b_kante2_leon_ohne.txt" 2>&1 &
EXE_SRC="$SP/exe_alt/re15_pc_alt_a661eb8aa.exe" bash "$W/lauf3.sh" kante2_leon_ohne_alt "${GEM[@]}" RE15_G2_OHNE_KANISTER=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 "RE15_FRAMEDUMP=100-3600/50:fd_" "RE15_INPUT_SCRIPT=$RAMPE" "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" "RE15_EXIT_AT=3600#2090" > "$SPW/b_kante2_leon_ohne_alt.txt" 2>&1 &
wait
T="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/unit/test_r41_gatorfix.exe"
export PATH="/c/msys64/mingw64/bin:$PATH" SDL_ASSERT=always_ignore
for r in kante2_leon_ohne kante2_leon_ohne_alt; do
  echo "== $r: $(cat $SPW/runs/$r/pid.txt | tr '\n' ' ') exe $(cut -c1-8 $SPW/runs/$r/exe_md5.txt)"
  "$T" haenglog "$SPW/runs/$r" 2>/dev/null | grep -E "^haenglog|Haenge-Fenster F" | cut -c1-330
done
# Ergebnis (final_runs, 2026-11-07 00:0x): alt 1 Haenge-Fenster F2218-F2367 (ohne Fortschritt 134), neu 0 Fenster, ohne Fortschritt 32 Bilder, 31 Treffer.
