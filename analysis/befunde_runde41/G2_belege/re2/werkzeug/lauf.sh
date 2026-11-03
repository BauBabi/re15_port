#!/usr/bin/env bash
# Abnahme 4: lauf.sh <name> <sekunden_video> [KEY=WERT ...] (Kopie des NB5/Abnahme-3-Skripts, eigener Baum build_abn4)
# eigene exe-Kopie re15_pc_g2re2_<name>.exe, eigener Laufordner, gdigrab auf das hwnd DIESES Prozesses, nur eigene PIDs.
set -u
NAME="$1"; shift
DAUER="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
EXE_DIR="${EXE_DIR:-$WT/re15_port/build_abn4/platform/pc}"
EXE_SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
SPW="${SPW:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2re2}"
RUN="$SPW/runs/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN"
KOPIE="$EXE_DIR/re15_pc_g2re2_$NAME.exe"
cp "$EXE_SRC" "$KOPIE"
md5sum "$EXE_SRC" | cut -c1-32 > "$RUN/exe_md5.txt"
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore
for kv in "$@"; do export "$kv"; done
env | grep -E '^RE15_|^SDL_' | sort > "$RUN/env.txt"
cd "$RUN"
"$KOPIE" > "$RUN/stdout.log" 2> "$RUN/debug.log" &
GP=$!
WINPID=$(cat /proc/$GP/winpid 2>/dev/null)
echo "pid=$GP winpid=$WINPID start=$(date +%T)" > "$RUN/pid.txt"
FF=""
if [ "${KEIN_VIDEO:-0}" != "1" ]; then
  HW=0
  for i in $(seq 1 600); do
    HW=$(powershell -NoProfile -Command "(Get-Process -Id $WINPID -ErrorAction SilentlyContinue).MainWindowHandle" 2>/dev/null | tr -d '\r')
    if [ -n "$HW" ] && [ "$HW" != "0" ]; then break; fi
    sleep 0.2
  done
  HX=$(printf '0x%x' "$HW" 2>/dev/null)
  echo "hwnd=$HX" >> "$RUN/pid.txt"
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate ${FPS:-30} -draw_mouse 0 -i "hwnd=$HX" \
      -t "$DAUER" -c:v libx264 -preset ultrafast -crf ${CRF:-23} -pix_fmt yuv420p "$RUN/video.mp4" &
  FF=$!
fi
T=0; LIM=${LIM_S:-$(( ${DAUER%.*} + 200 ))}
while kill -0 $GP 2>/dev/null; do sleep 1; T=$((T+1)); if [ $T -gt $LIM ]; then kill $GP 2>/dev/null; echo "TIMEOUT" >> "$RUN/pid.txt"; break; fi; done
wait $GP 2>/dev/null; echo "exit=$? ende=$(date +%T)" >> "$RUN/pid.txt"
if [ -n "$FF" ]; then sleep 1; kill -INT $FF 2>/dev/null; wait $FF 2>/dev/null; fi
rm -f "$KOPIE"
echo "fertig $RUN"; cat "$RUN/pid.txt"
