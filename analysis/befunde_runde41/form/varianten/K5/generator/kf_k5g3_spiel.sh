#!/usr/bin/env bash
# K5g3 Spiellauf (Geometrie-Stufe, G3-Zaehler): eigene exe-Kopie, Variante ueber RE15_RE2_ASSET_ROOT (fehlende RE2-Dateien, hier die TIM,
# fallen auf shared_assets/RE2 = K5e_f1 zurueck), Form per Messhaken RE15_BB_POSE in fester Lage/Clip/Schnitt, Spielerin schiesst
# (Zielen + Feuer gehalten), gdigrab NUR auf das eigene Fenster (Handle ueber die eigene PID), Ende ueber RE15_EXIT_AT bzw. nur die eigene PID.
# Kein AUTOSHOT, kein SOFTWARE_RENDER. Spiel-Asset shared_assets/RE2 bleibt unberuehrt.
#
#   bash kf_k5g3_spiel.sh <variante-ordner|-> <aus-ordner> <raum> <bb_pose> <spieler x,z,rot,band> <exit-bild> <bilder> [KEY=WERT ...]
#     <bb_pose>: x,z,gier,clip,bild,schnitt fuer RE15_BB_POSE oder "-" (freier Kampf)
#     <bilder>: Anzahl Fensterbilder (je 4 s Abstand ab Erscheinen der Form), 0 = keine
# Ausgabe: <aus-ordner>/debug.log (Kopie), bb_pose.log, fenster_<n>.png, Zeilen "[tri-liste]" auf stdout.
set -u
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
V=$1; A=$2; RAUM=$3; POSE=$4; SPIELER=$5; EXITB=$6; NB=$7; shift 7
mkdir -p "$A"; A=$(cd "$A" && pwd -W)
[ "$V" != "-" ] && V=$(cd "$V" && pwd -W)
EXE="${RE15_PC_EXE:-$W/re15_port/build/platform/pc/re15_pc.exe}"
ID="$$_$(date +%H%M%S)"
LAUF="$W/re15_port/build/form_spiel_k5g3/$ID"
NAME="re15_pc_k5g3_$ID"
mkdir -p "$LAUF"
cp "$EXE" "$LAUF/$NAME.exe" || exit 2
SKRIPT="A0.1,W1"
for i in $(seq 1 60); do SKRIPT="$SKRIPT,MA4"; done
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_PLAYER_POS="$SPIELER"
export RE15_BIRKIN_FORM=bahnsteig RE15_SPAWN_DIAG=1
[ "$POSE" != "-" ] && export RE15_BB_POSE="$POSE"       # "-" = kein Messhaken, die Form kaempft frei
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300 RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_GIVE=3:400 RE15_EQUIP=3 RE15_STATE_LOG=state.log
export RE15_EXIT_AT="$EXITB#$RAUM"
[ "$V" != "-" ] && export RE15_RE2_ASSET_ROOT="$V"
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
timeout 400 "./$NAME.exe" >exe_stdout.log 2>exe_stderr.log &
GP=$!
hole() { "$PS" -NoProfile -Command "Get-Process $NAME -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'; }
WPID=""; HWND=0
for i in $(seq 1 90); do set -- $(hole); WPID="${1:-}"; HWND="${2:-0}"; [ -n "$WPID" ] && [ "$HWND" != "0" ] && break; kill -0 $GP 2>/dev/null || break; sleep 1; done
echo "exe $NAME PID ${WPID:-?} Fenster $HWND"
if [ "$NB" -gt 0 ]; then
  for i in $(seq 1 150); do grep -aq "Bahnsteig-Form: " debug.log 2>/dev/null && break; kill -0 $GP 2>/dev/null || break; sleep 1; done
  sleep ${RE15_K5G3_ERSTES_BILD:-12}
  for n in $(seq 1 "$NB"); do
    kill -0 $GP 2>/dev/null || break
    set -- $(hole); HWND="${2:-$HWND}"; HX=$(printf '0x%x' "$HWND")
    "$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate 5 -draw_mouse 0 -i "hwnd=$HX" -frames:v 3 -update 1 "$A/fenster_$n.png" 2>/dev/null
    echo "Bild $A/fenster_$n.png"
    sleep 4
  done
fi
wait $GP; RC=$?
echo "exe Ende rc=$RC"
if [ -n "$WPID" ] && "$PS" -NoProfile -Command "Get-Process -Id $WPID -ErrorAction SilentlyContinue" >/dev/null 2>&1; then taskkill //PID "$WPID" //T //F >/dev/null 2>&1; fi
cp debug.log bb_pose.log state.log "$A/" 2>/dev/null
grep -a "Bahnsteig-Form\|EXIT_AT\|UEBERLAUF" debug.log | head -5
grep -a "\[tri-liste\]" debug.log | tail -3
echo "Laufverzeichnis: $LAUF"
