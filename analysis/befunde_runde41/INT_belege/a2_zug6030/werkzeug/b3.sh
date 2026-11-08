#!/usr/bin/env bash
# Befund 3: 5090 mit den AN-Bits wie im echten Weg ((3,89) Tor, (9,133) Zugstart gesehen): AN-Szene -> Riesenform 1000 HP (K5c)
# -> Rueckstoss -> Sturz -> G5-Auftritt. Weg/Waffe wie integration_r41_g5_tod (Rakete), Feuer erst nach dem Kampfstart (sub04).
M="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/int_a2"
NAME="$1"; shift
START=100 bash "$M/lauf.sh" "$NAME" "" "A0.1,W1,U8.5,W16.7,MA13,W80" "2900#5090" 600 \
  RE15_DEBUG_JUMP=5090@250 "RE15_PLAYER_POS=500,-9175,2048,0" RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
  RE15_SET_FLAG=3:89,9:133 RE15_SPAWN_DIAG=1 RE15_GIVE=18:200 RE15_EQUIP=18 RE15_FINISHER_LOG=1 \
  "RE15_FLAG_WATCH=3:89,9:133,3:42,5:28" "$@"
