#!/usr/bin/env bash
# wege.sh <weg F|W|R> <name-suffix> : durchgehender Lauf erster Besuch 6030 -> 6000 Tor -> zweiter Besuch 6030 (Leon)
M="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/int_a2"
WEG="$1"; SUF="${2:-}"
SEG0="A0.3,W8"
VORNE="A0.3,W8,L0.07,XU0.5,W0.5,L0.38,XU2,R0.12,A0.3,W6,XU1,R0.07,A0.3"
case "$WEG" in
  F) SEG1="$VORNE,W68,A0.3,W11,XU1,A0.3,W2,XU3,W75"; F2=5100 ;;
  W) SEG1="A0.3,W8,U3,W62,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W68,A0.3,W11"; F2=5000 ;;
  R) SEG1="$VORNE,W68,A0.3,W11,XU1,A0.3,W2,XU3,W75,L0.40,XU3,W0.5,R0.07,A0.3,W6,XU1,R0.07,A0.3,W20,A0.3,W11"; F2=6800 ;;
  *) echo "Weg?"; exit 2 ;;
esac
SEG2="W5,A0.2,W1,A0.2,W1,A0.2,W30"
SEG3="$VORNE,W45,A0.3,W11,R0.71,A0.3,W65"
bash "$M/lauf.sh" "weg_${WEG}${SUF}" "vor6030 tor_bereit" "$SEG0|$SEG1|$SEG2|$SEG3" "4300#6030!3" 900 \
  "RE15_FIRE_AOT=2@${F2}#6030;5@100#6000;3@900#6000" RE15_SOFTWARE_RENDER=1 RE15_EVT_TRACE=1 \
  "RE15_FLAG_WATCH=3:22,4:23,3:89,9:128,9:133,3:48,3:49,3:239,3:42"
