#!/usr/bin/env bash
# Abnahme 4 G5A: lauf4.sh NAME RAUM EXITAT MODUS [VIDEO_SECS] [FRAMEDUMP-SPEC]
# eigene exe-Kopie re15_g5abn4_<NAME>.exe, kein AUTOSHOT/SOFTWARE_RENDER/NOAUDIO; MODUS steh|frei|pistole
# VIDEO_SECS>0 -> gdigrab auf das hwnd der eigenen PID (20 B/s). Am Ende nur die EIGENE PID beenden.
set -u
NAME=$1; RAUM=$2; EXITAT=$3; MODUS=$4; SECS=${5:-0}; FD=${6:-}
SP="C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn4/lauf"
PC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/build/platform/pc
EXE=re15_g5abn4_$NAME
cp -f "$PC/re15_pc.exe" "$PC/$EXE.exe"
WD="$SP/$NAME"; rm -rf "$WD"; mkdir -p "$WD/fd"
SKRIPT="A0.1,W1,U8.5"
if [ "$MODUS" = "steh" ]; then for i in $(seq 1 100); do SKRIPT="$SKRIPT,MA4"; done
elif [ "$MODUS" = "pistole" ]; then for i in $(seq 1 18); do SKRIPT="$SKRIPT,MA4"; done; SKRIPT="$SKRIPT,D12"; for i in $(seq 1 60); do SKRIPT="$SKRIPT,MA4,W0.2"; done
else for i in $(seq 1 18); do SKRIPT="$SKRIPT,MA4"; done; SKRIPT="$SKRIPT,W90"; fi
EXTRA=""
[ "$RAUM" = "5091" ] && EXTRA="RE15_TITLE_SHOT_CHAR=1"
[ -n "$FD" ] && EXTRA="$EXTRA RE15_FRAMEDUMP=$FD"
EXTRA="$EXTRA ${EXTRA2:-}"
cd "$WD"
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_AUDIO_CAP_SYNC=ton.raw RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 $EXTRA \
  RE15_DEBUG_JUMP=${RAUM}@250 "RE15_PLAYER_POS=500,-9175,2048,0" \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_INPUT_SCRIPT=$SKRIPT" \
  RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log "RE15_GIVE=3:400,21:250" RE15_EQUIP=3 \
  "RE15_EXIT_AT=${EXITAT}#${RAUM}" "$PC/$EXE.exe" > stdout.txt 2>&1 &
LP=$!
sleep 2
# Windows-PID der EIGENEN exe-Kopie (eindeutiger Name re15_g5abn4_<NAME>) - nur diese wird gefilmt/beendet
WPID=$(powershell.exe -NoProfile -Command "(Get-Process $EXE -ErrorAction SilentlyContinue | Select-Object -First 1).Id" 2>/dev/null | tr -d '
 ')
echo "pid=$LP winpid=$WPID"
if [ "$SECS" != "0" ]; then
  H=""
  for i in $(seq 1 120); do
    H=$(powershell.exe -NoProfile -Command "(Get-Process $EXE -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
    [ -n "$H" ] && [ "$H" != "0" ] && break
    sleep 0.5
  done
  HX=$(printf '0x%x' "$H"); echo "hwnd=$HX"
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 20 -draw_mouse 0 -t "$SECS" -i "hwnd=$HX" \
    -c:v libx264 -crf 16 -pix_fmt yuv420p "$WD/fenster.mp4"
  echo "ffmpeg rc=$?"
fi
for i in $(seq 1 ${WAIT_N:-1200}); do kill -0 $LP 2>/dev/null || break; sleep 0.5; done
if kill -0 $LP 2>/dev/null; then powershell.exe -NoProfile -Command "Stop-Process -Id $WPID -Force" >/dev/null 2>&1; echo "beendet (eigene PID $WPID)"; fi
wait $LP
echo "exe rc=$?"
rm -f "$PC/$EXE.exe"
grep -a "g5auftritt\]" "$WD/stdout.txt" "$WD/debug.log" 2>/dev/null | head -8
ls "$WD/fd" | wc -l
