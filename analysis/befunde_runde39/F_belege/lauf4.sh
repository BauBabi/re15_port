#!/bin/bash
# lauf4.sh <name> <raum-hex> <char 0|4> <hp> <wundspec> <inv> <flags|-> [ENV=VAL ...]
# Nachbesserung 1: wie lauf3.sh, zusaetzlich Story-Flags in der Karte (Werkzeug karte, 13. Argument),
# z.B. 3:0x75 = R.P.D.-Weste -> Modell PL01 (nur Container PLD/PL01.PLD). EXE/TOOL per Umgebung.
set -u
BASE="C:/workspace/git/reAi_v2/.claude/worktrees/r39_zustand/re15_port"
EXE="${EXE:-$BASE/build/platform/pc/re15_pc_r39f_nb1.exe}"
TOOL="${TOOL:-$BASE/build/tests/unit/test_r39_zustand_nb1.exe}"
OUT="$BASE/../analysis/befunde_runde39/F_belege/laeufe"
name=$1; raum=$2; chr=$3; hp=$4; wund=$5; inv=$6; flags=$7; shift 7
d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"; cd "$d" || exit 2
"$TOOL" karte re15_card.mcr "$raum" "$chr" -1024 0 -3336 856 "$hp" "$wund" "$inv" 2 "$flags" > karte.txt || { cat karte.txt; exit 3; }
timeout 300 env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    "RE15_PAD_AT=200:S,260:A,300:A,340:A,500:S" "RE15_FRAMEDUMP=150-650/500:fd_" "RE15_EXIT_AT=700#$raum" \
    "$@" "$EXE" > stdout.txt
echo "exit=$? $(cat karte.txt)"
