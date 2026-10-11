#!/usr/bin/env bash
# Runde 36 Spur R "dachkiste" — Messlauf an der exe (Messkopie re15_pc_r36r.exe neben der exe).
#
#   bash lauf_exe.sh <laufname> KEY=WERT ...
#
# Laeuft im Ordner R_belege/<laufname>/ (debug.log = stderr der exe, climb.txt = RE15_CLIMB_TRACE,
# objfall.txt = RE15_OBJFALL_LOG (Spur A), push.txt = RE15_PUSH_LOG (Spur R, engine/src/dachkiste_r36.c:
# je Schubversuch der Kette FUN_8002bd44 eine Zeile mit Ziel-Lage und Ergebnis von FUN_8003b558(obj,2)).
# Grundumgebung wie tests/integration/test_r36_inventar.cmake (Debug-Menue-Sprung nach dem Spielstart,
# RE15_PSELECT_AUTO_SWITCH=1 fuer Elza -> Raum xxx1).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_dachkiste"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
KOPIE="$EXE_DIR/re15_pc_r36r.exe"
name="$1"; shift
dir="$BAUM/analysis/befunde_runde36/R_belege/$name"
mkdir -p "$dir"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    "$@" "$KOPIE" > run.log 2>&1
echo "exit=$?" >> run.log
