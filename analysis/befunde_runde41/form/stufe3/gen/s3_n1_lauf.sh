#!/usr/bin/env bash
# Nachbesserung 1, Asset-Spur: echtes Fenster (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER), ffmpeg gdigrab
# hwnd= auf das eigene Fenster, nur die eigene PID wird beendet. Nach dem Muster stufe3/abnahme1/werkzeug/lauf.sh.
#   s3_n1_lauf.sh <name> <sek-gesamt> <fps> <win-x> [ENV=WERT ...]
# EXE: Vorgabe re15_port/build/platform/pc/re15_pc_n1a.exe (Kopie, liest die Assets zur Laufzeit), anders mit N1_EXE=<pfad>.
set -uo pipefail
BAUM="$(cd "$(dirname "$0")/../../../../.." && pwd)"
NAME="$1"; GES="$2"; FPS="$3"; WX="$4"; shift 4
EXE="${N1_EXE:-$BAUM/re15_port/build/platform/pc/re15_pc_n1a.exe}"
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
LAUF="$BAUM/re15_port/build_n1a/spiel/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF/bild"
EXEN="re15_pc_n1a_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT="$LAUF/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUF/state.log" RE15_SPAWN_DIAG=1 RE15_BIRKIN_DBG=1
for kv in "$@"; do export "$kv"; done
env | grep '^RE15_' | sort > "$LAUF/env.txt"
cd "$LAUF" || exit 2
timeout $(( GES + 25 )) "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log &
GP=$!
hole() { "$PS" -NoProfile -Command "Get-Process $EXEN -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'; }
WPID=""; HWND=0
for i in $(seq 1 40); do set -- $(hole); WPID="${1:-}"; HWND="${2:-0}"; [ -n "$WPID" ] && [ "$HWND" != "0" ] && break; sleep 1; done
echo "PID $WPID HWND $HWND"
"$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), $WX, 500, 0, 0, 0x41)" >/dev/null 2>&1
HX=$(printf '0x%x' "$HWND")
"$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate "$FPS" -draw_mouse 0 -i "hwnd=$HX" -t "$GES" "$LAUF/bild/b_%04d.png"
echo "Bilder: $(ls "$LAUF"/bild/*.png | wc -l)"
if [ -n "$WPID" ]; then
  taskkill //PID "$WPID" >/dev/null 2>&1
  for i in 1 2 3 4 5 6 7 8; do kill -0 "$GP" 2>/dev/null || break; sleep 1; done
  kill -0 "$GP" 2>/dev/null && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
fi
wait "$GP" 2>/dev/null
rm -f "$(dirname "$EXE")/$EXEN.exe"
grep -a "Bahnsteig-Form" stderr.log debug.log 2>/dev/null | head -2
echo "Lauf: $LAUF"
