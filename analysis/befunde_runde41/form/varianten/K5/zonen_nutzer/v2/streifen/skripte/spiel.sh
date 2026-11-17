#!/usr/bin/env bash
# spiel.sh <name> <raum> <pose x,z,gier,clip,bild,schnitt> [ENV=WERT ...]
# Eigene exe-Kopie, echter Renderer (kein AUTOSHOT, kein SOFTWARE_RENDER, kein FRAMEDUMP), Spiel-Asset unveraendert (shared_assets/RE2).
# Aufnahme: ffmpeg gdigrab NUR auf das eigene Fenster (hwnd ueber die eigene Windows-PID), Ende nur ueber die eigene PID.
set -u
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/streifen
BAUM=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form
HIER=$BAUM/analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/streifen/skripte
FF=/c/ProgramData/chocolatey/bin/ffmpeg
NAME=$1; RAUM=$2; POSE=$3; shift 3
ID="streifen_${NAME}_$$"
LAUF="$BAUM/re15_port/build/form_spiel/$ID"
OUT="$S/spiel/$NAME"
mkdir -p "$LAUF" "$OUT"
cp "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$LAUF/re15_pc_$ID.exe"
cd "$LAUF"
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_SPAWN_DIAG=1 RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE="$POSE" RE15_PLAYER_POS=500,-9175,2048,0
for kv in "$@"; do export "$kv"; done
"./re15_pc_$ID.exe" >exe_stdout.log 2>exe_stderr.log &
MP=$!
WP=$(cat /proc/$MP/winpid)
echo "msys $MP win $WP"
F=""
for i in $(seq 1 60); do
  F=$(powershell -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w $HIER/fenster.ps1)" "$WP" 2>/dev/null | tr -d '\r')
  [ -n "$F" ] && break
  sleep 0.5
done
echo "fenster $F"
sleep 20
# Fensterkennung direkt vor der Aufnahme erneut ueber die eigene PID lesen (SDL kann das Fenster beim Start neu anlegen)
F=$(powershell -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w $HIER/fenster.ps1)" "$WP" 2>/dev/null | tr -d '')
echo "fenster vor Aufnahme $F"
HW=$(echo $F | cut -d' ' -f1)
"$FF" -hide_banner -loglevel error -y -f gdigrab -framerate 2 -draw_mouse 0 -i "hwnd=$HW" -frames:v 8 "$(cygpath -w $OUT)/hwnd_%02d.png"
echo "ffmpeg rc=$?"
taskkill //PID "$WP" //F >/dev/null 2>&1
wait $MP 2>/dev/null
cp exe_stderr.log "$OUT/stderr.log"
grep -a "Bahnsteig-Form\|BB_POSE\|bb_pose\|\[enemy\]" exe_stderr.log | sort -u | head -8
cd "$BAUM"; rm -rf "$LAUF"
ls "$OUT"
