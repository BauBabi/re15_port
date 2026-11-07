#!/usr/bin/env bash
# Abnahme 7 Welle 3 (startet nach Welle 2): eigene Oben-Lagen an den Kanten (Spawn, Kampf sofort, ohne Kanister; Umgebung wie die Abnahme-6-Spawn-Laeufe), Leon und Elza,
# alle vier Blickrichtungen an Nord- und Suedkante, Ost-Ende und NW-Ecke: (W) Sturz an beiden Kanten, (W2) Ostwand/Plattform bleiben Wand, (T) Rammen/Hochbiss oben.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7"
L="$SP/werkzeug/lauf7.sh"
while ! grep -q "WELLE2 FERTIG" "$SP/welle2_ausgabe.txt" 2>/dev/null; do sleep 10; done
GEM="RE15_CLIMB_TRACE=climb.log RE15_G2_OHNE_KANISTER=1 RE15_G2_TRAG_LOG=1 RE15_GB_TEST=1 RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_R39_MESS_SPIELER_HP=100 RE15_SET_FLAG=9:110,9:113 RE15_STATE_LOG=state.log RE15_WINDOW_SCALE=2"
ELZA="RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_TITLE_CONFIRM_MS=1500"
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"
bash "$L" a7_spawn_nord_leon_r1024 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=4200#2090 RE15_PLAYER_POS=3000,-14950,1024,1 > "$SP/w3_1.txt" 2>&1 &
bash "$L" a7_spawn_sued_elza_r1024 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4200#2091 RE15_PLAYER_POS=5800,-17650,1024,1 > "$SP/w3_2.txt" 2>&1 &
bash "$L" a7_spawn_sued_leon_r2048 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=4200#2090 RE15_PLAYER_POS=6000,-17400,2048,1 > "$SP/w3_3.txt" 2>&1 &
bash "$L" a7_spawn_nw_elza_r3072 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4200#2091 RE15_PLAYER_POS=2300,-14950,3072,1 > "$SP/w3_4.txt" 2>&1 &
bash "$L" a7_spawn_ost_elza_r1024 $GEM $ELZA RE15_DEBUG_JUMP=2091@250 RE15_EXIT_AT=4200#2091 RE15_PLAYER_POS=6750,-16300,1024,1 > "$SP/w3_5.txt" 2>&1 &
bash "$L" a7_spawn_mitte_leon_r0 $GEM $LEON RE15_DEBUG_JUMP=2090@250 RE15_EXIT_AT=4200#2090 RE15_PLAYER_POS=4400,-16200,0,1 > "$SP/w3_6.txt" 2>&1 &
wait
echo "WELLE3 FERTIG $(date +%T)"
