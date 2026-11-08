#!/usr/bin/env bash
# Abnahme 0 SF: echtes Fenster (beschleunigter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER, kein FRAMEDUMP) per gdigrab.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel
D="$1"; mkdir -p "$D/g"; rm -f "$D"/g/*.png "$D"/*.log "$D"/out.txt
NAME="re15_abn0win_$$"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_CD_ROOT="$BAUM/re15_port/shared_assets/PSX" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10" RE15_EXIT_AT=900 \
    timeout 120 "./$NAME.exe" > out.txt 2>&1 &
GP=$!
sleep 2
/c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 4 -t 40 \
    -i "title=RE1.5 Rebuilt — PC" "$D/g/g%04d.png"
echo "ffmpeg EXIT=$?"
wait $GP; echo "EXE EXIT=$?"
rm -f "$NAME.exe"
ls "$D"/g/*.png | wc -l
