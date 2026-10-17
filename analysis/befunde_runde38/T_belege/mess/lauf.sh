#!/usr/bin/env bash
# Runde 38 Spur T: ein Messlauf der echten exe (Messkopie re15_pc_r38t_<name>.exe neben der exe).
# Aufruf: lauf.sh <name> <arbeitsordner> [KEY=WERT ...]
# Startet immer mit RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1; weitere Umgebung per Argument.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r38_spiderman"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
name="$1"; wd="$2"; shift 2
kopie="$BAUM/re15_port/build/platform/pc/re15_pc_r38t_${name}.exe"
cp -f "$EXE" "$kopie"
mkdir -p "$wd"
cd "$wd" || exit 2
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
for kv in "$@"; do export "$kv"; done
timeout 300 "$kopie" > stdout.txt 2> stderr.txt
rv=$?
echo "exit=$rv"
rm -f "$kopie"
exit $rv
