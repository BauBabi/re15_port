#!/usr/bin/env bash
# Nachbesserung 4 Spur AN: eigener Lauf im ECHTEN Fenster (eigene exe-Kopie re15_pc_n4_*, Renderer des Nutzers, kein AUTOSHOT,
# kein SOFTWARE_RENDER). Bilder per RE15_FRAMEDUMP (komponiertes Bild direkt vor dem Present), Ton per RE15_AUDIO_CAP.
# Beendet wird nur die eigene PID (timeout auf den eigenen Prozess).
#   n4_lauf.sh <name> <zeitlimit-s> <eingabe-datei> [ENV=WERT ...]
# Ausgabe: re15_port/build/an_lauf/n4/<name>/ (debug.log, stderr.log, state.log, f<NNNNNN>.ppm, ton.raw, env.txt)
set -uo pipefail
BAUM=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette
NAME="${1:?name}"; LIMIT="${2:?zeitlimit}"; EING="${3:?eingabe}"; shift 3
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
LAUF="$BAUM/re15_port/build/an_lauf/n4/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_n4_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
LAUFW=$(cygpath -m "$LAUF")
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_TITLE_SHOT="$LAUFW/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
export RE15_INPUT_SCRIPT="$(tr -d ' \r\n' < "$EING")"
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
T0=$(date +%s)
timeout "$LIMIT" "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
echo "exit=$RC Dauer=$(( $(date +%s) - T0 )) s Bilder: $(ls "$LAUF"/f*.ppm 2>/dev/null | wc -l) in $LAUF"
rm -f "$(dirname "$EXE")/$EXEN.exe"
