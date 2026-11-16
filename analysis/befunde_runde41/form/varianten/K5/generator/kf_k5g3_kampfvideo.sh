#!/usr/bin/env bash
# R9-Einbau: kurzes Video des Kampfs in ROOM5090 mit dem SPIEL-ASSET (shared_assets/RE2, ohne Umleitung), echtes Fenster per gdigrab
# (Handle ueber die eigene PID), kein AUTOSHOT, kein SOFTWARE_RENDER. Ablauf wie der Lauf "kampf" in
# re15_port/tests/integration/test_r41_form_dreiecke.cmake (Spielerin 500,-9175, Skript A0.1,W1,U8.5 + 60 x MA4, Pistole 400 Schuss).
# Nur die eigene PID wird beendet.
#   bash kf_k5g3_kampfvideo.sh <aus.mp4> [<sekunden>] [<exit-bild>]
set -u
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
AUS=$1; SEK=${2:-40}; EXITB=${3:-2600}
mkdir -p "$(dirname "$AUS")"
EXE="${RE15_PC_EXE:-$W/re15_port/build/platform/pc/re15_pc.exe}"
ID="$$_$(date +%H%M%S)"
LAUF="$W/re15_port/build/form_spiel_k5g3/video_$ID"
NAME="re15_pc_k5g3v_$ID"
mkdir -p "$LAUF"
cp "$EXE" "$LAUF/$NAME.exe" || exit 2
SKRIPT="A0.1,W1,U8.5"
for i in $(seq 1 60); do SKRIPT="$SKRIPT,MA4"; done
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="5090@250" RE15_PLAYER_POS="500,-9175,2048,0"
export RE15_BIRKIN_FORM=bahnsteig RE15_SPAWN_DIAG=1
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_GIVE=3:400,21:250 RE15_EQUIP=3 RE15_STATE_LOG=state.log
export RE15_EXIT_AT="$EXITB#5090"
cd "$LAUF" || exit 2
timeout 400 "./$NAME.exe" >exe_stdout.log 2>exe_stderr.log &
GP=$!
hole() { "$PS" -NoProfile -Command "Get-Process $NAME -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'; }
WPID=""; HWND=0
for i in $(seq 1 90); do set -- $(hole); WPID="${1:-}"; HWND="${2:-0}"; [ -n "$WPID" ] && [ "$HWND" != "0" ] && break; kill -0 $GP 2>/dev/null || break; sleep 1; done
echo "exe $NAME PID ${WPID:-?} Fenster $HWND"
for i in $(seq 1 150); do grep -aq "Bahnsteig-Form: " debug.log 2>/dev/null && break; kill -0 $GP 2>/dev/null || break; sleep 1; done
sleep ${RE15_VIDEO_WARTE:-2}
set -- $(hole); HWND="${2:-$HWND}"; HX=$(printf '0x%x' "$HWND")
"$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate 30 -draw_mouse 0 -t "$SEK" -i "hwnd=$HX" \
  -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -c:v libx264 -pix_fmt yuv420p -crf 20 "$AUS"
echo "Video $AUS"
wait $GP; RC=$?
echo "exe Ende rc=$RC"
if [ -n "$WPID" ] && "$PS" -NoProfile -Command "Get-Process -Id $WPID -ErrorAction SilentlyContinue" >/dev/null 2>&1; then taskkill //PID "$WPID" //T //F >/dev/null 2>&1; fi
grep -a "Bahnsteig-Form\|EXIT_AT\|UEBERLAUF" debug.log | head -5
grep -a "\[tri-liste\]" debug.log | tail -2
echo "Laufverzeichnis: $LAUF"
