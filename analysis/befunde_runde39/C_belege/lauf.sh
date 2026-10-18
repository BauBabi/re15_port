#!/bin/bash
# lauf.sh <name> "<kartenargs>" <exit_at> [ENV=WERT ...]
# Karte schreiben, exe (Messkopie re15_pc_r39c_mess.exe) per CONTINUE starten, debug.log behalten.
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/c
B="C:/workspace/git/reAi_v2/.claude/worktrees/r39_inventar/re15_port/build"
name=$1; karte=$2; ende=$3; shift 3
d=$S/runs/$name; rm -rf "$d"; mkdir -p "$d"; cd "$d" || exit 9
"$B/tests/unit/probe_r37_feuerloescher_karte.exe" re15_card.mcr $karte > karte.txt 2>&1 || { cat karte.txt; exit 8; }
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    RE15_MSG_LOG=1 RE15_INV_LOG=1 RE15_EXIT_AT=$ende "$@" \
    timeout 400 "$B/platform/pc/re15_pc_r39c_mess.exe" > out.txt 2>&1
echo "exit $? ($d)"
