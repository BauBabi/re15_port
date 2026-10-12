#!/usr/bin/env bash
# Runde 37 Spur D Nachbesserung 1: Messlauf an der exe mit eigener Messkopie re15_pc_r37d_nb1.exe
# (Muster D_belege/lauf_d.sh; Vorher-Stand: NB1_EXE=<pfad> setzt eine andere Quell-exe, Kopie re15_pc_r37d_nb1v.exe).
#   bash lauf_nb1.sh <laufname> <wegplan-datei|-> KEY=WERT ...
# Laufordner: Scratchpad .../scratchpad/nb1/laeufe/<laufname> (debug.log, wegplan.txt, climb.txt, itemband.txt, run.log).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r37_itemband"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
if [ -n "${NB1_EXE:-}" ]; then SRC="$NB1_EXE"; KOPIE="$EXE_DIR/re15_pc_r37d_nb1v.exe"; else SRC="$EXE_DIR/re15_pc.exe"; KOPIE="$EXE_DIR/re15_pc_r37d_nb1.exe"; fi
if [ ! -f "$KOPIE" ] || ! cmp -s "$SRC" "$KOPIE"; then cp "$SRC" "$KOPIE"; fi
SP="${NB1_SP:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb1/laeufe}"
name="$1"; plan="$2"; shift 2
if [ "$plan" != "-" ]; then plan="$(cd "$(dirname "$plan")" && pwd)/$(basename "$plan")"; fi
dir="$SP/$name"
mkdir -p "$dir"
cd "$dir" || exit 2
rm -f debug.log climb.txt objfall.txt push.txt run.log stair_dbg.log wegplan.txt itemband.txt
WP=""
if [ "$plan" != "-" ]; then cp "$plan" "$dir/wegplan_eingabe.txt"; WP="RE15_WEGPLAN=$dir/wegplan_eingabe.txt"; fi
start=$(date +%s)
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 SDL_ASSERT=always_ignore \
    RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_STAIR_DBG=1 \
    RE15_CLIMB_TRACE=climb.txt RE15_OBJFALL_LOG=objfall.txt RE15_PUSH_LOG=push.txt \
    RE15_WEGPLAN_LOG=wegplan.txt RE15_ITEMBAND_LOG=itemband.txt $WP \
    "$@" timeout "${NB1_TIMEOUT:-900}" "$KOPIE" > run.log 2>&1
rc=$?
echo "exit=$rc dauer=$(( $(date +%s) - start ))s" >> run.log
echo "[$name] exit=$rc dauer=$(( $(date +%s) - start ))s"
