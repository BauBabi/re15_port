#!/usr/bin/env bash
# s80_lauf.sh - Runde 41 Spur ST80-T: Szenen im ECHTEN Fenster (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT,
# kein SOFTWARE_RENDER), Bildfolge per ffmpeg gdigrab hwnd= (Skill re15-port-visual-verify). Nur die eigene PID wird
# beendet. Muster: tools/r41_annette/an_lauf.sh (Zweig r41/annette).
#
#   s80_lauf.sh <name> "<karten-argumente>" <start-s> <dauer-s> <fps> "<input-skript>" [ENV=WERT ...]
#
# Lade-Weg wie integration_r36_story: Speicherkarte aus probe_r36_story_karte (Stand + Zusaetze), RE15_CONTINUE_TEST=1
# RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 (CONTINUE-Block main.c). Staende: marvin (30C1), john30e1 (30E1, cut=0),
# vor6030 (Tuer 6000 -> 6030 Wagen, Skript "A0.3,W8,U3,W40"), zug_leon (6030 Fuehrerstand).
# Ausgabe: re15_port/build/s80_lauf/<name>/ (stderr.log mit [msgtext]/[story80], bild_NNN.png, video.mp4).
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
NAME="${1:?name}"; KARTE="${2:?karte}"; START="${3:?start}"; DAUER="${4:?dauer}"; FPS="${5:?fps}"; SKRIPT="${6:-W1}"
shift 6
BUILD="$BAUM/re15_port/build"
EXE="$BUILD/platform/pc/re15_pc.exe"
KARTENWERKZEUG="$BUILD/tests/unit/probe_r36_story_karte.exe"
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
LAUF="$BUILD/s80_lauf/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_s80_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
cd "$LAUF" || exit 2
# shellcheck disable=SC2086
"$KARTENWERKZEUG" re15_card.mcr $KARTE > karte.log 2>&1 || { cat karte.log; exit 2; }

export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 RE15_MSG_LOG=1
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT="$SKRIPT"
for kv in "$@"; do export "$kv"; done
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
grep -a "\[story80\]\|\[msgtext\]\|CONTINUE: resumed" stderr.log debug.log 2>/dev/null | head -40
echo "Lauf: $LAUF"
