#!/bin/bash
# Spur M r36: echter Lauf der exe-Kopie mit Spielstand aus probe_r35_karte karte
# lauf.sh <arbeitsordner> <oeffnen_raum_hex> <timeout_s> <kartenargs...> -- <ENV=...>...
B="C:/workspace/git/reAi_v2/.claude/worktrees/r36_karte/re15_port/build"
WD="$1"; shift; RAUM="$1"; shift; TO="$1"; shift
KA=()
while [ $# -gt 0 ] && [ "$1" != "--" ]; do KA+=("$1"); shift; done
[ "$1" = "--" ] && shift
mkdir -p "$WD"; rm -f "$WD/re15_card.mcr" "$WD"/*.bmp
( cd "$WD" && "$B/tests/unit/probe_r35_karte.exe" karte re15_card.mcr "${KA[@]}" ) || { echo "KARTE FAIL"; exit 1; }
ID=$RANDOM$RANDOM
EXE="$B/platform/pc/re15_pc_r36m_$ID.exe"
cp "$B/platform/pc/re15_pc.exe" "$EXE"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 "RE15_INV_OPEN_AT=40#$RAUM" "$@" \
     timeout "$TO" "$EXE" > /dev/null 2>&1 ); echo "exit=$?"
rm -f "$EXE"
