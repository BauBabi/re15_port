#!/usr/bin/env bash
# Runde 38 Spur W Nachbesserung 1: Messlauf mit eigener Messkopie. bash lauf_n1.sh <neu|alt> <name> KEY=WERT ...
#   neu = exe des Spurbaums (Kopie re15_pc_W38nb1.exe daneben), alt = v0.8.25-Paket-exe (Kopie im Paketordner).
set -u
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r38_rolltor20a0"
var="$1"; name="$2"; shift 2
if [ "$var" = "neu" ]; then
  EXE_DIR="$BAUM/re15_port/build/platform/pc"; KOPIE="$EXE_DIR/re15_pc_W38nb1.exe"; cp -f "$EXE_DIR/re15_pc.exe" "$KOPIE"
else
  PKG="$S/abnW0_rolltor/v0825/pkg/re15_port_v0.8.25"; KOPIE="$PKG/re15_pc_W38nb1alt.exe"; cp -f "$PKG/re15_pc.exe" "$KOPIE"
fi
dir="$S/W38nb1/$name"; mkdir -p "$dir"; cd "$dir" || exit 2
rm -f debug.log run.log state.txt objpoly.txt fd_*.ppm
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore RE15_TITLE_CONFIRM_MS=1500 \
    RE15_STATE_LOG=state.txt RE15_OBJPOLY_LOG=objpoly.txt "$@" "$KOPIE" > run.log 2>&1
rc=$?
echo "exit=$rc" >> run.log
rm -f "$KOPIE"
exit 0
