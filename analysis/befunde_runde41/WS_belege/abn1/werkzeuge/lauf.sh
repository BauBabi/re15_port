#!/usr/bin/env bash
# Abnahme WS Runde 1: ein exe-Lauf mit der Messkopie re15_pc_wsabn.exe.
# Aufruf: lauf.sh <name> <timeout_s> KEY=WERT ...
set -u
NAME="$1"; TO="$2"; shift 2
ROOT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen/re15_port/build/platform/pc"
KOPIE="$ROOT/re15_pc_wsabn.exe"
OUT="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1/laeufe/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT"
cp -f "$ROOT/re15_pc.exe" "$KOPIE"
export PATH=/c/msys64/mingw64/bin:$PATH
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_STATE_LOG=state.log
for kv in "$@"; do export "$kv"; done
cd "$OUT"
start=$(date +%s)
timeout "$TO" "$KOPIE" > stdout.log 2> stderr.log
rv=$?
end=$(date +%s)
echo "lauf $NAME exit=$rv dauer=$((end-start))s" | tee -a "$OUT/lauf.txt"
printf '%s\n' "$@" >> "$OUT/lauf.txt"
