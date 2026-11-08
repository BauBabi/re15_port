#!/usr/bin/env bash
# Runde 41 Spur SF Nachbesserung 1 — Hangar-Szene (Spur H) mit dem Sichel-Modell im ECHTEN Fenster aufnehmen
# (Skill re15-port-visual-verify: eigene exe-Kopie, kein AUTOSHOT, kein SOFTWARE_RENDER, gdigrab per Fenster-Handle).
# Weg wie Spur H Lauf n8 (debug.log dort): Affen per Kill-Flag 8:42/8:43 aus, Debug-Sprung 50A0, Spieler an die
# Nordtuer, Eingabeskript nach Sueden -> Cut 4 -> Szene in Cut 12.
#
#   bash re15_port/tools/r41_sichel/szene_lauf.sh <laufordner> [sekunden=22] [exit-bild=520] [env ...]
# Ausgabe: <laufordner>/gd_####.png (30 Bilder/s, 320x240), debug.log ([hangar-takt]-Zeilen mit F = Spielbild).
set -u
BAUM="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../.." && pwd -W)"
EXE_SRC="${EXE_SRC:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
NAME="re15_sf_szene_$$"
D="$1"; SEK="${2:-22}"; EX="${3:-520}"; shift; [ $# -gt 0 ] && shift; [ $# -gt 0 ] && shift
mkdir -p "$D"; rm -f "$D"/gd_*.png "$D"/state.log "$D"/out.txt
cp -f "$EXE_SRC" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_CD_ROOT="$BAUM/re15_port/shared_assets/PSX" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10" RE15_EXIT_AT="$EX" "$@" \
    timeout 200 "./$NAME.exe" > out.txt 2>&1 &
LP=$!
H=""
for i in $(seq 1 120); do
  H=$(powershell.exe -NoProfile -Command "(Get-Process $NAME -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
  [ -n "$H" ] && [ "$H" != "0" ] && break
  sleep 0.5
done
echo "hwnd=$H"
if [ -n "$H" ] && [ "$H" != "0" ]; then
  HX=$(printf '0x%x' "$H")
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 30 -draw_mouse 0 \
    -t "$SEK" -i "hwnd=$HX" -vf scale=320:240 gd_%04d.png
  echo "ffmpeg rc=$?"
fi
wait $LP
echo "EXIT=$?"
rm -f "$NAME.exe"
ls gd_*.png 2>/dev/null | wc -l
grep -a "\[hangar\]" debug.log | head -3
