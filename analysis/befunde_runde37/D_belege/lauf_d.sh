#!/usr/bin/env bash
# Runde 37 Spur D "itemband": Messlauf an der exe mit eigener Messkopie re15_pc_r37d.exe (Muster
# R_belege/lauf_nb3.sh / Abnahme-3-Skript lauf_abn3.sh der Spur R).
#   bash lauf_d.sh <laufname> <wegplan-datei|-> KEY=WERT ...
# Laufordner: Scratchpad .../scratchpad/r37d/<laufname> (debug.log, wegplan.txt, climb.txt, objfall.txt, push.txt,
# itemband.txt = RE15_ITEMBAND_LOG des Fixes, fehlt im Stand vorher).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_itemband"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
KOPIE="$EXE_DIR/re15_pc_r37d.exe"
if [ ! -f "$KOPIE" ] || [ "$EXE_DIR/re15_pc.exe" -nt "$KOPIE" ]; then cp "$EXE_DIR/re15_pc.exe" "$KOPIE"; fi
SP="${R37D_SP:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r37d}"
name="$1"; plan="$2"; shift 2
if [ "$plan" != "-" ]; then plan="$(cd "$(dirname "$plan")" && pwd)/$(basename "$plan")"; fi
dir="$SP/$name"
mkdir -p "$dir"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log stair_dbg.log wegplan.txt itemband.txt
WP=""
if [ "$plan" != "-" ]; then cp "$plan" "$dir/wegplan_eingabe.txt"; WP="RE15_WEGPLAN=$dir/wegplan_eingabe.txt"; fi
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    RE15_WEGPLAN_LOG=wegplan.txt RE15_ITEMBAND_LOG=itemband.txt $WP \
    "$@" "$KOPIE" > run.log 2>&1
echo "exit=$?" >> run.log
