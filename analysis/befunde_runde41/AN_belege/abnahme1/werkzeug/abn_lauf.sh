#!/usr/bin/env bash
# Abnahme 1 Spur AN: eigener Fensterlauf (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER),
# gdigrab hwnd= direkt nach mp4, Ton per RE15_AUDIO_CAP (Echtzeit-Mischung). Nur die eigene PID wird beendet.
#   abn_lauf.sh <name> <dauer-s> <fps> [ENV=WERT ...]
set -uo pipefail
BAUM=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette
NAME="${1:?name}"; DAUER="${2:?dauer}"; FPS="${3:?fps}"; shift 3
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
LAUF="$BAUM/re15_port/build/abn1/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_abn1_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
LAUFW=$(cygpath -m "$LAUF")
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT="$LAUFW/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
export RE15_AUDIO_CAP="$LAUFW/ton.raw"
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
GESAMT=$(( DAUER + 40 ))
timeout "$GESAMT" "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log &
GP=$!
hole_proz() {
    "$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'
}
WPID=""; HWND="0"
for i in $(seq 1 40); do
    set -- $(hole_proz); WPID="${1:-}"; HWND="${2:-0}"
    [ -n "$WPID" ] && [ "$HWND" != "0" ] && break
    sleep 1
done
echo "exe $EXEN PID ${WPID:-?} Fenster $HWND" | tee lauf.txt
sleep 2
set -- $(hole_proz); HWND="${2:-$HWND}"
"$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), 20, 20, 0, 0, 0x41)" >/dev/null 2>&1
HX=$(printf '0x%x' "$HWND")
date +%s.%N > video_start.txt
"$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate "$FPS" -draw_mouse 0 -i "hwnd=$HX" -t "$DAUER" \
    -c:v libx264 -preset ultrafast -crf 20 -pix_fmt yuv420p "$LAUF/video.mp4"
date +%s.%N > video_ende.txt
if [ -n "$WPID" ]; then
    taskkill //PID "$WPID" >/dev/null 2>&1
    for i in 1 2 3 4 5 6; do kill -0 "$GP" 2>/dev/null || break; sleep 1; done
    kill -0 "$GP" 2>/dev/null && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
fi
wait "$GP" 2>/dev/null
rm -f "$(dirname "$EXE")/$EXEN.exe"
ls -la "$LAUF" | head -20
echo "Lauf: $LAUF"
