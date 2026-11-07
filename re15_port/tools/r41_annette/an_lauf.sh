#!/usr/bin/env bash
# an_lauf.sh - Runde 41 Spur AN: Annette-Szene im ECHTEN Fenster (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT,
# kein SOFTWARE_RENDER), Bildfolge per ffmpeg gdigrab hwnd= (Skill re15-port-visual-verify). Nur die eigene PID wird beendet.
# Muster: analysis/befunde_runde41/form/stufe3/gen/s3_spielfolge.sh.
#
#   an_lauf.sh <name> <raum 5090|5091> <start-s> <dauer-s> <fps> "<input-skript>" [ENV=WERT ...]
#
# Weg wie integration_r41_birkin_pistole: Debug-Sprung, Lage in Wagen B, Tuer 2 nach Wagen C, laufen bis Cut 12.
# RE15_SET_FLAG=3:89 stellt den Stand "Tor offen" (ROOM6000 sub07 @0x011B4) nach.
# Ausgabe: re15_port/build/an_lauf/<name>/ (Logs, bild_NNN.png, video.mp4).
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
NAME="${1:?name}"; RAUM="${2:?raum}"; START="${3:?start}"; DAUER="${4:?dauer}"; FPS="${5:?fps}"; SKRIPT="${6:-W1}"
shift 6
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
LAUF="$BAUM/re15_port/build/an_lauf/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_an_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2

export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT="$LAUF/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@250" RE15_PLAYER_POS="500,-9175,2048,0"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_STATE_LOG="$LAUF/state.log" RE15_SPAWN_DIAG=1
[ "$RAUM" = "5091" ] && export RE15_TITLE_SHOT_CHAR=1
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
GESAMT=$(( START + DAUER + 15 ))
timeout "$GESAMT" "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log &
GP=$!

hole_proz() {
    "$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'
}
WPID=""; HWND="0"
for i in $(seq 1 30); do
    set -- $(hole_proz); WPID="${1:-}"; HWND="${2:-0}"
    [ -n "$WPID" ] && [ "$HWND" != "0" ] && break
    sleep 1
done
echo "exe $EXEN PID ${WPID:-?} Fenster $HWND"
sleep "$START"
set -- $(hole_proz); HWND="${2:-$HWND}"
"$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), 20, 20, 0, 0, 0x41)" >/dev/null 2>&1
HX=$(printf '0x%x' "$HWND")
"$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate "$FPS" -draw_mouse 0 -i "hwnd=$HX" -t "$DAUER" "$LAUF/bild_%03d.png"
echo "Bilder: $(ls "$LAUF"/bild_*.png 2>/dev/null | wc -l) in $LAUF"
if [ -n "$WPID" ]; then
    taskkill //PID "$WPID" >/dev/null 2>&1
    for i in 1 2 3 4 5 6; do kill -0 "$GP" 2>/dev/null || break; sleep 1; done
    kill -0 "$GP" 2>/dev/null && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
fi
wait "$GP" 2>/dev/null
rm -f "$(dirname "$EXE")/$EXEN.exe"
"$FFMPEG" -hide_banner -loglevel error -y -framerate "$FPS" -i "$LAUF/bild_%03d.png" -c:v libx264 -pix_fmt yuv420p "$LAUF/video.mp4" 2>/dev/null
grep -a "\[annette\]" stderr.log debug.log 2>/dev/null | head -30
echo "Lauf: $LAUF"
