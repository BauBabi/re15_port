#!/usr/bin/env bash
# NB6 HSF: Lauf mit RE15_FRAMEDUMP (jedes Spielbild, pixelgleich zum Fenster) und optional gdigrab des Fensters (GD=1).
# lauf5.sh <dir> <exitbild> [env...]  (Szenen-Weg wie re15_port/tools/r41_sichel/szene_lauf.sh / Abnahme 2 lauf.sh)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel
EXE_SRC="${EXE_SRC:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
NAME="re15_hsfnb6_$$"
D="$1"; EX="$2"; shift 2
mkdir -p "$D/fd"; rm -f "$D"/gd_*.png "$D"/fd/*.ppm "$D"/state.log "$D"/out.txt "$D"/debug.log
cp -f "$EXE_SRC" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_CD_ROOT="${CDROOT:-$BAUM/re15_port/shared_assets/PSX}" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="${INP:-W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10}" RE15_EXIT_AT="$EX" \
    RE15_FRAMEDUMP="${FD:-225-415/1}:fd/f" "$@" \
    timeout 400 "./$NAME.exe" > out.txt 2>&1 &
LP=$!
if [ "${GD:-0}" = "1" ]; then
  H=""
  for i in $(seq 1 120); do
    H=$(powershell.exe -NoProfile -Command "(Get-Process $NAME -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
    [ -n "$H" ] && [ "$H" != "0" ] && break
    sleep 0.5
  done
  echo "hwnd=$H pid=$LP"
  if [ -n "$H" ] && [ "$H" != "0" ]; then
    HX=$(printf '0x%x' "$H")
    /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 60 -draw_mouse 0 \
      -t "${SEK:-28}" -i "hwnd=$HX" gd_%04d.png
    echo "ffmpeg rc=$?"
  fi
fi
wait $LP
echo "EXIT=$?"
rm -f "$NAME.exe"
ls fd/*.ppm 2>/dev/null | wc -l
grep -a "\[hangar\]" debug.log | head -3
