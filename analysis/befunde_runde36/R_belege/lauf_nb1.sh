#!/usr/bin/env bash
# Runde 36 Spur R "dachkiste", Nachbesserung 1 — Messlauf an der exe (Messkopie re15_pc_r36r_nb1.exe neben der exe).
#
#   bash lauf_nb1.sh <laufname> KEY=WERT ...
#
# Wie lauf_exe.sh (Grundumgebung tests/integration/test_r36_inventar.cmake), zusaetzlich RE15_STAIR_DBG=1
# (stair_common.c: [STAIRREG]/[stairzone]/[stair] START nach stair_dbg.log im Arbeitsordner — die GUI-exe hat
# kein nutzbares stderr) und RE15_SCD_TRACE fuer die Latches (Set (3,93) in ROOM3050 sub01 @0x02196).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_dachkiste"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
KOPIE="$EXE_DIR/re15_pc_r36r_nb1.exe"
[ -f "$KOPIE" ] || cp "$EXE_DIR/re15_pc.exe" "$KOPIE"
name="$1"; shift
dir="$BAUM/analysis/befunde_runde36/R_belege/$name"
mkdir -p "$dir"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log stair_dbg.log
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 \
    "$@" "$KOPIE" > run.log 2>&1
echo "exit=$?" >> run.log
