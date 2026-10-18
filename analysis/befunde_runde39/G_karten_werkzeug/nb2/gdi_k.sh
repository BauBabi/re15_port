#!/bin/bash
# Nachbesserung 2: Abnahme-Weg ROOM1210 Flur -> Kammer, Kartenschirm offen halten, FB_SHOT UND gdigrab des ECHTEN Fensters
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten"
PROBE="$WT/re15_port/build/tests/unit/probe_r36_karte.exe"
SRC="$WT/re15_port/build/platform/pc/re15_pc.exe"
WD="$1"
mkdir -p "$WD"; rm -f "$WD"/*.bmp "$WD"/*.png "$WD/re15_card.mcr" "$WD/debug.log"
( cd "$WD" && "$PROBE" karte re15_card.mcr 1210 -19500 0 -21700 2048 besucht:11E0:0:0 besucht:1210:-19500:-21700 ) > "$WD/karte.txt" 2>&1 || { echo "KARTE FAIL"; exit 1; }
ID=$RANDOM$RANDOM; NAME="re15_pc_g39nb2_gdi_$ID"; EXE="$(dirname "$SRC")/$NAME.exe"; cp "$SRC" "$EXE"
( cd "$WD" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 \
    RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U4,W5 "RE15_INV_OPEN_AT=200#1210" \
    RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=1500#1210" \
    timeout 400 "$EXE" > "$WD/stdout.txt" 2> "$WD/stderr.txt" ) &
for i in $(seq 1 200); do [ -f "$WD/karte.bmp" ] && break; sleep 1; done
sleep 3
H=$(powershell -NoProfile -Command "(Get-Process -Name '$NAME' -ErrorAction SilentlyContinue | Select-Object -First 1).MainWindowHandle")
echo "hwnd=$H"
/c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -draw_mouse 0 -framerate 1 -i "hwnd=$H" -frames:v 1 "$WD/gdi.png"
ls -la "$WD/gdi.png" 2>&1
powershell -NoProfile -Command "Get-Process -Name '$NAME' -ErrorAction SilentlyContinue | Stop-Process -Force"
wait; rm -f "$EXE"
