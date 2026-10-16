#!/usr/bin/env bash
# Runde 38 Spur W "rolltor20a0": Messlauf an der exe mit eigener Messkopie (Muster Runde 37 D lauf_d.sh).
#   bash lauf_w.sh <laufname> <wegplan-datei|-> KEY=WERT ...
# Laufordner: Scratchpad .../scratchpad/W38/<laufname> (run.log, debug.log, wegplan.txt, state.txt, objfall.txt, push.txt).
# Messkopie: W38_EXE (Default <baum>/re15_port/build/platform/pc/re15_pc.exe) -> re15_pc_w38<suffix>.exe daneben.
set -u
BAUM="${W38_BAUM:-C:/workspace/git/reAi_v2/.claude/worktrees/r38_rolltor20a0}"
EXE_DIR="${W38_EXE_DIR:-$BAUM/re15_port/build/platform/pc}"
KOPIE="$EXE_DIR/re15_pc_w38${W38_SUFFIX:-}.exe"
if [ ! -f "$KOPIE" ] || [ "$EXE_DIR/re15_pc.exe" -nt "$KOPIE" ]; then cp "$EXE_DIR/re15_pc.exe" "$KOPIE"; fi
SP="${W38_SP:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/W38}"
name="$1"; plan="$2"; shift 2
if [ "$plan" != "-" ]; then plan="$(cd "$(dirname "$plan")" && pwd)/$(basename "$plan")"; fi
dir="$SP/$name"
mkdir -p "$dir"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log wegplan.txt state.txt fd_*.ppm
WP=""
if [ "$plan" != "-" ]; then cp "$plan" "$dir/wegplan_eingabe.txt"; WP="RE15_WEGPLAN=$dir/wegplan_eingabe.txt"; fi
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    RE15_WEGPLAN_LOG=wegplan.txt RE15_STATE_LOG=state.txt $WP \
    "$@" "$KOPIE" > run.log 2>&1
echo "exit=$?" >> run.log
