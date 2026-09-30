#!/bin/bash
# Abnahme 1 Spur G2: ein Messlauf der echten exe (Kopie re15_pc_abn1g2.exe) in eigenem Arbeitsordner.
# Aufruf: lauf.sh <name> <timeout_s> [KEY=WERT ...]
set -u
SP=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/6b88e19a-7fc0-4017-9ec3-f7270c110524/scratchpad/g2_abn1_x7
EXE=${EXE:-C:/workspace/git/reAi_v2/.claude/worktrees/r34n_schrift/re15_port/build/platform/pc/re15_pc_abn1g2.exe}
name=$1; to=$2; shift 2
W=$SP/$name
rm -rf "$W"; mkdir -p "$W"
if [ -n "${KARTE:-}" ]; then cp "$KARTE" "$W/re15_card.mcr"; fi
cd "$W" || exit 3
export PATH="/c/msys64/mingw64/bin:$PATH"
t0=$(date +%s)
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=3 "$@" timeout "$to" "$EXE" > stdout.txt 2>&1
rv=$?
t1=$(date +%s)
echo "[$name] exit=$rv dauer=$((t1-t0))s" | tee -a "$W/lauf.txt"
