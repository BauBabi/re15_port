#!/usr/bin/env bash
# NB4 Runde 3 (Bauer): lauf9.sh <name> [KEY=WERT ...] - Messkopie build/platform/pc/re15_pc_g2nb4r3.exe (= Bau-exe 42a1c8f7 des Stands 9c3659bf7).
# EXE_QUELLE=<andere exe> fuer Gegenproben (Kopie re15_pc_g2nb4r3_alt.exe). Kein gdigrab; Bilder nur als RE15_FRAMEDUMP-Serie. Nur eigene PIDs, kein taskkill.
set -u
NAME="$1"; shift
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
if [ -n "${EXE_QUELLE:-}" ]; then KOPIE="$WT/re15_port/build/platform/pc/re15_pc_g2nb4r3_alt.exe"; [ -f "$KOPIE" ] || cp "$EXE_QUELLE" "$KOPIE";
else KOPIE="$WT/re15_port/build/platform/pc/re15_pc_g2nb4r3.exe"; fi
RUN="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb4r3/runs/$NAME"
rm -rf "$RUN"; mkdir -p "$RUN"
md5sum "$KOPIE" | cut -c1-32 > "$RUN/exe_md5.txt"
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore
for kv in "$@"; do export "$kv"; done
env | grep -E '^RE15_|^SDL_' | sort > "$RUN/env.txt"
cd "$RUN"
"$KOPIE" > "$RUN/stdout.log" 2> "$RUN/debug.log" &
GP=$!
echo "pid=$GP start=$(date +%T)" > "$RUN/pid.txt"
T=0; LIM=${LIM_S:-2400}
while kill -0 $GP 2>/dev/null; do sleep 1; T=$((T+1)); if [ $T -gt $LIM ]; then kill $GP 2>/dev/null; echo "TIMEOUT" >> "$RUN/pid.txt"; break; fi; done
wait $GP 2>/dev/null; echo "exit=$? ende=$(date +%T)" >> "$RUN/pid.txt"
echo "fertig $RUN"; cat "$RUN/pid.txt"
