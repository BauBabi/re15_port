#!/usr/bin/env bash
# gdigrab des ECHTEN Fensters (kein SOFTWARE_RENDER, kein AUTOSHOT), Kartenschirm offen. Zusaetzlich FB_SHOT als Abgleich.
# <tag> <raum> <x> <y> <z> <yaw> [Sonden-Argumente]
set -u
R=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r2
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r42_karte
PROBE=$WT/re15_port/build/tests/unit/probe_r36_karte.exe
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FF=/c/ProgramData/chocolatey/bin/ffmpeg.exe
tag=$1; raum=$2; x=$3; y=$4; z=$5; yaw=$6; shift 6
W="$R/fenster/$tag"; mkdir -p "$W"; NAME="re15_pc_karte_$tag"
cp "$R/exe/re15_pc.exe" "$W/$NAME.exe"
( cd "$W" && rm -f re15_card.mcr karte.bmp fenster.png && PATH="/c/msys64/mingw64/bin:$PATH" "$PROBE" karte re15_card.mcr "$raum" "$x" "$y" "$z" "$yaw" besucht:$raum:$x:$z "$@" > probe.log 2>&1 )
cd "$W"
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
    "RE15_INV_OPEN_AT=40#$raum" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=96 "RE15_EXIT_AT=900#$raum" \
    timeout 200 "./$NAME.exe" > spiel.log 2>&1 &
GP=$!
for i in $(seq 1 150); do [ -f karte.bmp ] && break; kill -0 $GP 2>/dev/null || break; sleep 1; done
[ -f karte.bmp ] || { echo "OHNE KARTE $tag"; kill $GP 2>/dev/null; exit 1; }
sleep 2
set -- $("$PS" -NoProfile -Command "Get-Process $NAME -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" | tr -d '\r')
WPID=${1:-}; HWND=${2:-0}
"$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), 20, 20, 0, 0, 0x41)" >/dev/null 2>&1
sleep 1
"$FF" -hide_banner -loglevel error -y -f gdigrab -framerate 5 -draw_mouse 0 -i "hwnd=$(printf '0x%x' $HWND)" -frames:v 3 -update 1 fenster.png
[ -n "$WPID" ] && /c/Windows/System32/taskkill.exe /PID $WPID /F >/dev/null 2>&1
wait $GP 2>/dev/null
[ -f fenster.png ] && echo "OK $tag" || echo "OHNE FENSTERBILD $tag"
