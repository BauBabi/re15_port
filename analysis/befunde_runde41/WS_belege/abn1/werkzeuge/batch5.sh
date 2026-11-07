#!/usr/bin/env bash
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1"
L="$S/lauf.sh"
# H8 Gitterhaende 0x1A ROOM1210: eine Explosion Art 2 an Slot 1 (erwartet 250 -> -1 -> DEATH-Zweig, HP zurueck auf 250)
bash "$L" gitter_l 240 RE15_DEBUG_JUMP=1210@250 "RE15_FORCE_EXPLOSION=2@120:1" "RE15_EXIT_AT=300#1210" "RE15_FRAMEDUMP=116-236/12:fd_"
# H9 Feuer-Emitter 0x26 ROOM1090: Kandidat? erwartet 100 -> -151
bash "$L" feuer_l 240 RE15_DEBUG_JUMP=1090@250 "RE15_FORCE_EXPLOSION=2@120:1" "RE15_EXIT_AT=200#1090"
echo BATCH5-FERTIG
