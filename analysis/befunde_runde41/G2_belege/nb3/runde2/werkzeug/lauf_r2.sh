#!/usr/bin/env bash
# G2-RE2 NB3 Runde 2 (Bauer): lauf_r2.sh <name> [KEY=WERT ...]
# Messkopie KOPIE (Vorgabe build/platform/pc/re15_pc_g2nb3r2.exe = Kopie der Bau-exe nach 5a94392bf; "vorher" = re15_pc_g2nb3r2_alt.exe = Kopie der Runde-1-exe
# 029efac6 = Stand 10603425b). Bilder nur als RE15_FRAMEDUMP-Serie (KEY=WERT), kein gdigrab. Nur eigene PIDs, kein taskkill.
set -u
NAME="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
EXE_DIR="$WT/re15_port/build/platform/pc"
KOPIE="${KOPIE:-$EXE_DIR/re15_pc_g2nb3r2.exe}"
SPW="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/r2"
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
