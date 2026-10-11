#!/usr/bin/env bash
# Runde 36 Spur R "dachkiste", Nachbesserung 3 — Messlauf an der exe mit Wegplan (Messkopie re15_pc_r36r_nb3.exe).
#   bash lauf_nb3.sh <laufname> <wegplan-datei> KEY=WERT ...
# Grundumgebung wie lauf_nb1.sh (tests/integration/test_r36_inventar.cmake); RE15_WEGPLAN = geschlossen geregelter
# Lauf (dachkiste_r36.c), Log wegplan.txt; RE15_PUSH_LOG/RE15_OBJFALL_LOG/RE15_CLIMB_TRACE daneben.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_dachkiste"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
KOPIE="$EXE_DIR/re15_pc_r36r_nb3.exe"
if [ ! -f "$KOPIE" ] || [ "$EXE_DIR/re15_pc.exe" -nt "$KOPIE" ]; then cp "$EXE_DIR/re15_pc.exe" "$KOPIE"; fi
name="$1"; plan="$2"; shift 2
dir="$BAUM/analysis/befunde_runde36/R_belege/$name"
mkdir -p "$dir"
cp "$plan" "$dir/wegplan_eingabe.txt"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log stair_dbg.log wegplan.txt
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    RE15_WEGPLAN="$dir/wegplan_eingabe.txt" RE15_WEGPLAN_LOG=wegplan.txt \
    "$@" "$KOPIE" > run.log 2>&1
echo "exit=$?" >> run.log
