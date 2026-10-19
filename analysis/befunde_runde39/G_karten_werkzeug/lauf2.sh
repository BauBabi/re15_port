#!/bin/bash
# G39 Kartenlauf (Fortsetzung): lauf2.sh <arbeitsordner> <oeffnen_raum_hex> <timeout_s> <kartenargs...> -- <ENV=...>...
# Spielstand per probe_r35_karte karte (Slot 0), echte exe als Kopie mit eigenem Namen, Titel -> LOAD GAME
# (RE15_CONTINUE_TEST + RE15_CARD_AUTO), Kartenschirm per RE15_INV_OPEN_AT, Abzug per Env (FB_SHOT/SWEEP).
B="$(cd "$(dirname "$0")/../../../re15_port/build" && pwd)"
WD="$1"; shift; RAUM="$1"; shift; TO="$1"; shift
KA=()
while [ $# -gt 0 ] && [ "$1" != "--" ]; do KA+=("$1"); shift; done
[ "$1" = "--" ] && shift
mkdir -p "$WD"; rm -f "$WD/re15_card.mcr" "$WD"/*.bmp "$WD/debug.log"
( cd "$WD" && "$B/tests/unit/probe_r35_karte.exe" karte re15_card.mcr "${KA[@]}" ) > "$WD/karte.txt" 2>&1 || { echo "KARTE FAIL"; cat "$WD/karte.txt"; exit 1; }
ID=$RANDOM$RANDOM
EXE="$B/platform/pc/re15_pc_g39f_$ID.exe"
cp "$B/platform/pc/re15_pc.exe" "$EXE"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
     RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 "RE15_INV_OPEN_AT=40#$RAUM" "$@" \
     timeout "$TO" "$EXE" > /dev/null 2>&1 ); echo "exit=$?"
rm -f "$EXE"
