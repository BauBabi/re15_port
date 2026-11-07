#!/usr/bin/env bash
# G2 Abnahme 6 (NB3 Runde 2, Abnehmer): lauf6.sh <name> [KEY=WERT ...]
# Messkopie: KOPIE (Vorgabe build/platform/pc/re15_pc_g2abn6.exe = Kopie der Bau-exe 5910fc24; alter Stand a661eb8aa: re15_pc_g2abn6_alt.exe = Kopie von
# g2nb3/exe_alt/re15_pc_alt_a661eb8aa.exe, md5 d872c8e6; Runde 1 10603425b: re15_pc_g2abn6_r1.exe, md5 029efac6). Kein gdigrab; Bilder nur als
# RE15_FRAMEDUMP-Serie. Nur eigene PIDs, kein taskkill.
set -u
NAME="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
EXE_DIR="$WT/re15_port/build/platform/pc"
KOPIE="${KOPIE:-$EXE_DIR/re15_pc_g2abn6.exe}"
SPW="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6"
RUN="$SPW/runs/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN"
md5sum "$KOPIE" | cut -c1-32 > "$RUN/exe_md5.txt"
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore
for kv in "$@"; do export "$kv"; done
env | grep -E '^RE15_|^SDL_' | sort > "$RUN/env.txt"
cd "$RUN"
"$KOPIE" > "$RUN/stdout.log" 2> "$RUN/debug.log" &
GP=$!
WINPID=$(cat /proc/$GP/winpid 2>/dev/null)
echo "pid=$GP winpid=$WINPID start=$(date +%T)" > "$RUN/pid.txt"
T=0; LIM=${LIM_S:-1500}
while kill -0 $GP 2>/dev/null; do sleep 1; T=$((T+1)); if [ $T -gt $LIM ]; then kill $GP 2>/dev/null; echo "TIMEOUT" >> "$RUN/pid.txt"; break; fi; done
wait $GP 2>/dev/null; echo "exit=$? ende=$(date +%T)" >> "$RUN/pid.txt"
echo "fertig $RUN"; cat "$RUN/pid.txt"
