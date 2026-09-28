#!/usr/bin/env bash
# r30_port_capture.sh - Runde 30 / Thema D.
# Startet die UNVERAENDERTE re15_pc.exe (beschleunigter Renderer, kein AUTOSHOT, kein
# SOFTWARE_RENDER), wartet bis das Titelmenue steht, und nimmt das ECHTE Fenster per
# ffmpeg gdigrab ueber sein FENSTER-HANDLE auf (nicht ueber den Titel: parallel laufende
# Agenten haben Fenster mit demselben Titel). Das Fenster wird dazu in die linke obere
# Bildschirmecke geschoben (Massstab 1 = 320x240), damit es von den zentrierten Fenstern
# der anderen nicht ueberdeckt wird.
# Aufruf: r30_port_capture.sh <exe> <ausgabe.mkv> <sekunden> [fps]
set -uo pipefail
export PATH="/c/msys64/mingw64/bin:$PATH"
EXE="$1"; OUTV="$2"; SECS="$3"; FPS="${4:-144}"
cd "$(dirname "$EXE")"
RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 "./$(basename "$EXE")" >/dev/null 2>&1 &
BPID=$!
sleep 4
WPID=$(powershell -NoProfile -Command "(Get-Process $(basename "$EXE" .exe) | Where-Object { \$_.Path -like '*build_r30_titel-blinken*' } | Select-Object -First 1).Id")
HWND=$(powershell -NoProfile -Command "
Add-Type @'
using System; using System.Runtime.InteropServices;
public class R30W { [DllImport(\"user32.dll\")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint f); }
'@
\$p = Get-Process -Id $WPID
[void][R30W]::SetWindowPos(\$p.MainWindowHandle, [IntPtr]::Zero, 0, 0, 0, 0, 0x0001 -bor 0x0004 -bor 0x0010)
'{0}' -f \$p.MainWindowHandle")
echo "pid=$WPID hwnd=$HWND"
sleep 4
/c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -y -f gdigrab -draw_mouse 0 -framerate "$FPS" \
    -i "hwnd=$HWND" -t "$SECS" -c:v ffv1 -vsync passthrough "$OUTV"
echo "ffmpeg exit $?"
kill $BPID 2>/dev/null
powershell -NoProfile -Command "Stop-Process -Id $WPID -Force -ErrorAction SilentlyContinue"
