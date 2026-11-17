#!/usr/bin/env bash
# Schwanz-Bewegung V8 (Code-Bauer): Hangar-Szene 50A0 mit Framedump F225-F400 (960x720), debug.log mit [hangar-takt],
# pose.log (RE15_HANGAR_POSE_LOG, bis zur v0.8.31-Basis RE15_POSE_LOG: gezeichnete Knochen je Bild), state.log.
#   bash lauf.sh <exe> <laufordner> [env ...]     (CDR=<cd-root> fuer Farbprobe, FD=0 ohne Framedump)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut
EXE="$1"; D="$2"; shift 2
NAME="re15_swv8_$$"
mkdir -p "$D/fd"; rm -f "$D"/fd/*.ppm "$D"/state.log "$D"/out.txt "$D"/debug.log "$D"/pose.log
cp -f "$EXE" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
FDARG="RE15_FRAMEDUMP=225-400/1:fd/f"
[ "${FD:-1}" = "0" ] && FDARG="RE15_FD_AUS=1"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_HANGAR_POSE_LOG=pose.log \
    RE15_CD_ROOT="${CDR:-$BAUM/re15_port/shared_assets/PSX}" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10" RE15_EXIT_AT="${EX:-405}" \
    "$FDARG" "$@" \
    timeout 300 "./$NAME.exe" > out.txt 2>&1
echo "EXIT=$?"
rm -f "$NAME.exe"
ls fd/*.ppm 2>/dev/null | wc -l
