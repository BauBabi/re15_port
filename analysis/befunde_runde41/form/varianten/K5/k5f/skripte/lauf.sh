#!/usr/bin/env bash
# lauf.sh <name> <raum> <von-bis/schritt> [ENV=WERT ...]  -> $S/spiel/<name>/dump_NNNNNN.ppm
# Eigene exe-Kopie (eindeutiger Name), echter Renderer (kein AUTOSHOT/SOFTWARE_RENDER), Framedump = SDL_RenderReadPixels des Fensters,
# Ende per RE15_EXIT_AT bzw. nur die eigene PID.
set -u
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f
BAUM=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form
NAME=$1; RAUM=$2; SERIE=$3; shift 3
ENDE=$(echo "$SERIE" | sed 's/.*-\([0-9]*\).*/\1/')
ID="k5f_${NAME}_$$"
LAUF="$BAUM/re15_port/build/form_spiel/$ID"
mkdir -p "$LAUF" "$S/spiel/$NAME"
cp "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$LAUF/re15_pc_$ID.exe"
cd "$LAUF"
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP="$SERIE:dump_" RE15_EXIT_AT="$((ENDE + 20))#$RAUM"
for kv in "$@"; do export "$kv"; done
timeout 240 "./re15_pc_$ID.exe" >exe_stdout.log 2>exe_stderr.log
echo "rc=$? $(ls dump_*.ppm 2>/dev/null | wc -l) Bilder"
cp dump_*.ppm "$S/spiel/$NAME/" 2>/dev/null
cp exe_stderr.log "$S/spiel/$NAME/stderr.log"; cp debug.log "$S/spiel/$NAME/debug.log" 2>/dev/null
grep -a "\[enemy\]\|spawn-diag\|Bahnsteig-Form\|FINALER" exe_stderr.log debug.log 2>/dev/null | sort -u | head -12
