#!/bin/bash
# lauf3.sh <name> <raum-hex> <char 0|4> <hp> <wundspec> <inv> [ENV=VAL ...]   (Werkzeug test_r39_zustand: karte mit Inventar)
set -u
BASE="C:/workspace/git/reAi_v2/.claude/worktrees/r39_zustand/re15_port"
EXE="${EXE:-$BASE/build/platform/pc/re15_pc_r39f_stand3.exe}"
TOOL="${TOOL:-$BASE/build/tests/unit/test_r39_zustand_kopie3.exe}"
OUT="$BASE/../analysis/befunde_runde39/F_belege/laeufe"
name=$1; raum=$2; chr=$3; hp=$4; wund=$5; inv=$6; shift 6
d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"; cd "$d" || exit 2
"$TOOL" karte re15_card.mcr "$raum" "$chr" -1024 0 -3336 856 "$hp" "$wund" "$inv" 2 > karte.txt || { cat karte.txt; exit 3; }
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 "$@" "$EXE" > stdout.txt
echo "exit=$? $(cat karte.txt)"
