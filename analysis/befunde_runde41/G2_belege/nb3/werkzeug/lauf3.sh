#!/usr/bin/env bash
# G2 NB3 (Messer): lauf3.sh <name> [KEY=WERT ...] - eigene exe-Kopie re15_pc_g2nb3_<name>.exe aus build/platform/pc/re15_pc.exe, kein Video (Framedumps per
# RE15_FRAMEDUMP als KEY=WERT), Logs im Laufordner runs/<name>; nur eigene PIDs. Kern wie G2_belege/abn4/werkzeug/lauf2.sh.
set -u
NAME="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
EXE_DIR="${EXE_DIR:-$WT/re15_port/build/platform/pc}"
EXE_SRC="${EXE_SRC:-$EXE_DIR/re15_pc.exe}"
SPW="${SPW:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3}"
RUN="$SPW/runs/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN"
KOPIE="$EXE_DIR/re15_pc_g2nb3_$NAME.exe"
cp "$EXE_SRC" "$KOPIE"
md5sum "$EXE_SRC" | cut -c1-32 > "$RUN/exe_md5.txt"
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore
for kv in "$@"; do export "$kv"; done
env | grep -E '^RE15_|^SDL_' | sort > "$RUN/env.txt"
cd "$RUN"
"$KOPIE" > "$RUN/stdout.log" 2> "$RUN/debug.log" &
GP=$!
echo "pid=$GP start=$(date +%T)" > "$RUN/pid.txt"
T=0; LIM=${LIM_S:-900}
while kill -0 $GP 2>/dev/null; do sleep 1; T=$((T+1)); if [ $T -gt $LIM ]; then kill $GP 2>/dev/null; echo "TIMEOUT" >> "$RUN/pid.txt"; break; fi; done
wait $GP 2>/dev/null; echo "exit=$? ende=$(date +%T)" >> "$RUN/pid.txt"
rm -f "$KOPIE"
echo "fertig $RUN"; cat "$RUN/pid.txt"
