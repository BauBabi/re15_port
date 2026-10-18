#!/bin/bash
# lauf.sh <name> <raum-hex> <char 0|4> <hp> <wundspec> [ENV=VAL ...]
# Spielstand per Kartenwerkzeug (Lage wie test_r36_kleidung_bild: -1024 0 -3336 rot 856, Cut 2), CONTINUE,
# danach die exe (Messkopie, Variable EXE) mit den uebergebenen Umgebungswerten. Logs landen im Laufordner
# (die exe lenkt stderr nach debug.log im Arbeitsordner, main.c:3257).
set -u
BASE="C:/workspace/git/reAi_v2/.claude/worktrees/r39_zustand/re15_port"
EXE="${EXE:-$BASE/build/platform/pc/re15_pc_r39f_vorher.exe}"
TOOL="${TOOL:-$BASE/build/tests/unit/test_r36_kleidung_r39f_kopie.exe}"
OUT="$BASE/../analysis/befunde_runde39/F_belege/laeufe"
name=$1; raum=$2; chr=$3; hp=$4; wund=$5; shift 5
d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"; cd "$d" || exit 2
"$TOOL" karte re15_card.mcr "$raum" "$chr" -1024 0 -3336 856 "$hp" "$wund" 2 > karte.txt || { cat karte.txt; exit 3; }
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
    RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 "$@" "$EXE" > stdout.txt
echo "exit=$? $(cat karte.txt)"
