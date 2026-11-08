#!/usr/bin/env bash
# G2 Abnahme 4: lauf2.sh <name> <sekunden_video> [KEY=WERT ...] - wie lauf.sh (eigene exe-Kopie re15_pc_g2abn4_<name>.exe aus build/platform/pc/re15_pc.exe,
# gdigrab auf das hwnd DIESES Prozesses, nur eigene PIDs), zusaetzlich: das Fenster wird nach dem Erscheinen in die rechte obere Bildschirmecke geschoben
# (SetWindowPos 1264,0; gdigrab hwnd= liefert sonst den Bildschirmausschnitt INKLUSIVE fremder Fenster anderer Agenten, die zentriert erscheinen - gemessen 2026-11-06
# im Lauf neu_west4_leon: ab ~55 s ein fremdes, stehendes Bild). RE15_FRAMEDUMP (Serie, PPM je Spielbild, Rueckleser vor dem Present) kann als KEY=WERT mitgegeben werden.
set -u
NAME="$1"; shift
DAUER="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
EXE_DIR="${EXE_DIR:-$WT/re15_port/build/platform/pc}"
EXE_SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
SPW="${SPW:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4}"
RUN="$SPW/runs/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN"
KOPIE="$EXE_DIR/re15_pc_g2abn4_$NAME.exe"
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
  powershell -NoProfile -Command "Add-Type -Name U -Namespace X -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int w, int hh, uint f);'; [X.U]::SetWindowPos([IntPtr]$HW, [IntPtr]0, ${WIN_X:-1264}, ${WIN_Y:-0}, 0, 0, 0x0005) | Out-Null" 2>/dev/null
  sleep 0.3
  RECT=$(powershell -NoProfile -Command "Add-Type -Name R -Namespace Y -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool GetWindowRect(IntPtr h, out RECT r); [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }'; \$r = New-Object Y.R+RECT; [Y.R]::GetWindowRect([IntPtr]$HW, [ref]\$r) | Out-Null; \"\$(\$r.L),\$(\$r.T)-\$(\$r.R),\$(\$r.B)\"" 2>/dev/null | tr -d '\r')
  echo "hwnd=$HX rect=$RECT" >> "$RUN/pid.txt"
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
