#!/usr/bin/env bash
# raum6.sh <dir> <exitbild> <JUMP> <POS> [env..]  eigene exe-Kopie re15_hsfnb6r_<pid>; INP/FD per Umgebung
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel
SP=/c/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad
EXE_SRC="${EXE_SRC:-$SP/exe/nb2.exe}"
NAME="re15_hsfnb6r_$$"
D="$1"; EX="$2"; J="$3"; P="$4"; shift 4
mkdir -p "$D/fd"; rm -f "$D"/fd/*.ppm "$D"/out.txt "$D"/debug.log "$D"/state.log
cp -f "$EXE_SRC" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_CD_ROOT="$BAUM/re15_port/shared_assets/PSX" RE15_STATE_LOG=state.log \
    RE15_DEBUG_JUMP="$J" RE15_PLAYER_POS="$P" RE15_INPUT_SCRIPT="${INP:-W30}" RE15_EXIT_AT="$EX" \
    RE15_FRAMEDUMP="${FD:-120-300/2}:fd/f" "$@" \
    timeout 600 "./$NAME.exe" > out.txt 2>&1
echo "EXIT=$?"
rm -f "$NAME.exe"
ls fd/*.ppm 2>/dev/null | wc -l
