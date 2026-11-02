#!/bin/bash
# Abnahme ZA: eigenes Fenster (eigene PID) per ffmpeg gdigrab hwnd= abgreifen. Nur eigene PID wird beendet.
# Aufruf: RAUM=1140 TAG=x MODELS=re15 ENTROPY=1 BILDER=6 TAKT=2 NACH=6 bash cap.sh
PC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_zombiearm/re15_port/build/platform/pc
Z=$(cd "$(dirname "$0")" && pwd -W 2>/dev/null || dirname "$0")
OUT=${OUT:-$Z/cap}; mkdir -p "$OUT"
cd $PC || exit 1
export PATH="/c/msys64/mingw64/bin:$PATH"
EXE=./re15_pc_zaabn.exe
cp -f ./re15_pc.exe $EXE
rm -f debug.log
EXTRA=""
[ -n "$MODELS" ] && EXTRA="$EXTRA RE15_AI_MODELS=$MODELS"
[ -n "$ENTROPY" ] && EXTRA="$EXTRA RE15_RNG_ENTROPY=1"
[ -n "$ELZA" ] && EXTRA="$EXTRA $ELZA"
[ -n "$CUT" ] && EXTRA="$EXTRA RE15_FORCE_CUT=$CUT"
[ -n "$SCRIPT" ] && EXTRA="$EXTRA RE15_INPUT_SCRIPT=$SCRIPT RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=${SSTART:-330}"
env SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
    RE15_DEBUG_JUMP=${RAUM:-1140}@240 RE15_WINDOW_SCALE=${SCALE:-3} $EXTRA $EXE > "$OUT/${TAG}_stdout.log" 2>&1 &
GP=$!
WPID=""
for w in $(seq 1 60); do sleep 1; grep -q -E "\[enemy\]" debug.log 2>/dev/null && break; done
sleep ${NACH:-6}
WPID=$(powershell -NoProfile -Command "(Get-Process re15_pc_zaabn -ErrorAction SilentlyContinue | Select -First 1).Id")
H=$(/c/Python310/python.exe $Z/hwnd.py $WPID)
echo "PID $GP/$WPID HWND $H"
for t in $(seq 1 ${BILDER:-6}); do
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -y -f gdigrab -draw_mouse 0 -framerate 1 -i "hwnd=$H" -frames:v 1 "$OUT/${TAG}_$t.png"
  sleep ${TAKT:-2}
done
cp debug.log "$OUT/${TAG}_debug.log" 2>/dev/null
kill $GP 2>/dev/null; sleep 1
powershell -NoProfile -Command "Stop-Process -Id $WPID -ErrorAction SilentlyContinue" 2>/dev/null
wait $GP 2>/dev/null
ls -la "$OUT" | grep "$TAG"
