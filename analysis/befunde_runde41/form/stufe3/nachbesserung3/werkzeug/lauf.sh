#!/usr/bin/env bash
# Eigener Lauf (Nachbesserung 3, N1): eigene exe-Kopie, eigene PID per timeout. lauf.sh <name> <limit> <envdatei>
set -uo pipefail
BAUM=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form
NAME="$1"; LIMIT="$2"; ENVF="$3"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
LAUF="$BAUM/re15_port/build/nb3/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_nb3_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
LAUFW=$(cygpath -m "$LAUF")
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
while IFS= read -r kv; do [ -n "$kv" ] && export "$kv"; done < "$ENVF"
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
T0=$(date +%s)
timeout "$LIMIT" "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
echo "exit=$RC Dauer=$(( $(date +%s) - T0 )) s in $LAUF"
rm -f "$(dirname "$EXE")/$EXEN.exe"
