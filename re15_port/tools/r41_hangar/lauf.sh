#!/usr/bin/env bash
# Runde 41 Spur H "hangar" — Messlauf im ECHTEN Fenster (Skill re15-port-visual-verify): eigene exe-Kopie, kein AUTOSHOT,
# kein SOFTWARE_RENDER, gdigrab per Fenster-Handle (hwnd=, nicht title=: parallele Agenten haben gleich betitelte Fenster).
#
# Aufruf: lauf.sh <ausgabeverzeichnis> <sekunden-aufnahme> <exit-bild> [env-zuweisungen ...]
#   Titel: RE15_TITLE_SHOT (+_AF=2) = feste Vorspann-Bildzahl (Leon); Elza zusaetzlich RE15_TITLE_SHOT_CHAR=1.
#   Die env-Zuweisungen (RE15_DEBUG_JUMP=..., RE15_INPUT_SCRIPT=..., RE15_PLAYER_POS=...) gehen unveraendert an die exe.
#   Ausgabe: <dir>/gd_###.png (4 Bilder/s), <dir>/state.log (RE15_STATE_LOG), <dir>/out.txt (stderr der exe).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_hangar
EXE_SRC="${EXE_SRC:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"   # EXE_SRC=... = Vergleichslauf mit einer anderen exe (Kopie)
NAME=re15_h41hangar
D="$1"; SEK="$2"; EX="$3"; shift 3
mkdir -p "$D"; rm -f "$D"/gd_*.png "$D"/state.log "$D"/out.txt
cp -f "$EXE_SRC" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_STATE_LOG=state.log RE15_EXIT_AT="$EX" "$@" \
    timeout 300 "./$NAME.exe" > out.txt 2>&1 &
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
  # GD_FPS (Standard 4) / GD_VF (z.B. scale=320:240 fuer den Bildvergleich mit dem Video, gleiche Bildrate 30)
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate "${GD_FPS:-4}" -draw_mouse 0 \
    -t "$SEK" -i "hwnd=$HX" ${GD_VF:+-vf "$GD_VF"} gd_%04d.png
  echo "ffmpeg rc=$?"
fi
wait $LP
echo "EXIT=$?"
ls gd_*.png 2>/dev/null | wc -l
