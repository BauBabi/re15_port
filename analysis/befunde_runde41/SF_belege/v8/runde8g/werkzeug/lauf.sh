#!/usr/bin/env bash
# Hangar-Szene 50A0 mit Framedump (wie schwanz_r9/r3/lauf.sh), Asset aus einer CD-Ueberlagerung. bash lauf.sh <exe> <cdwurzel> <laufordner> [env ...]
# Umgebung: FDR (Standard 317-343/1), EX (Exit, 350), SC (Fensterfaktor, 3)
set -u
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
EXE="$1"; CDR="$2"; D="$3"; shift 3
NAME="re15_8g_$$"
mkdir -p "$D/fd"; rm -f "$D"/fd/*.ppm "$D"/state.log "$D"/out.txt "$D"/debug.log "$D"/schwanz.log
cp -f "$EXE" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE="${SC:-3}" \
    RE15_CD_ROOT="$CDR/PSX" RE15_ASSET_ROOT="$W/re15_port/shared_assets/PSX" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log RE15_SCHWANZ_LOG=schwanz.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10" RE15_EXIT_AT="${EX:-350}" \
    "RE15_FRAMEDUMP=${FDR:-317-343/1}:fd/f" "$@" \
    timeout 400 "./$NAME.exe" > out.txt 2> debug.log
echo "EXIT=$? $(ls fd/*.ppm 2>/dev/null | wc -l) $D"
rm -f "$NAME.exe"
