#!/usr/bin/env bash
# run_port.sh NAME RAUM SECS EXITAT : Abnahme G5A - Port-Lauf wie integration_r41_birkin_pistole, aber OHNE SOFTWARE_RENDER,
# echtes Fenster per gdigrab hwnd (eigene exe-Kopie, eigene PID) als mp4 aufnehmen.
set -u
NAME=$1; RAUM=$2; SECS=$3; EXITAT=$4
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/port"
PC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/build_abn/platform/pc
EXE=re15_g5abn_$NAME
cp -f "$PC/re15_pc.exe" "$PC/$EXE.exe"
WD="$SP/$NAME"; rm -rf "$WD"; mkdir -p "$WD"
SKRIPT="A0.1,W1,U8.5"; for i in $(seq 1 100); do SKRIPT="$SKRIPT,MA4"; done
EXTRA=""
[ "$RAUM" = "5091" ] && EXTRA="RE15_TITLE_SHOT_CHAR=1"
cd "$WD"
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 $EXTRA \
  RE15_DEBUG_JUMP=${RAUM}@250 "RE15_PLAYER_POS=500,-9175,2048,0" \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=$SKRIPT" \
  RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3 \
  "RE15_EXIT_AT=${EXITAT}#${RAUM}" "$PC/$EXE.exe" > stdout.txt 2>&1 &
LP=$!
H=""
for i in $(seq 1 120); do
  H=$(powershell.exe -NoProfile -Command "(Get-Process $EXE -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
  [ -n "$H" ] && [ "$H" != "0" ] && break
  sleep 0.5
done
echo "hwnd=$H"
HX=$(printf '0x%x' "$H")
/c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 20 -draw_mouse 0 -t "$SECS" -i "hwnd=$HX" \
  -c:v libx264 -crf 16 -pix_fmt yuv420p "$WD/fenster.mp4"
echo "ffmpeg rc=$?"
wait $LP
echo "exe rc=$?"
rm -f "$PC/$EXE.exe"
ls -la "$WD"
