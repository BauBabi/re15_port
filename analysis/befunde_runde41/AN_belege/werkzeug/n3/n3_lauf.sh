#!/usr/bin/env bash
# n3_lauf.sh - Spur AN Nachbesserung 3: Lauf im ECHTEN Fenster (eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein
# SOFTWARE_RENDER), Bilder per RE15_FRAMEDUMP (komponiertes Bild vor dem Present, kein Bildausfall). Nur die eigene PID wird beendet.
#   n3_lauf.sh <name> <raum> <bild-von> <bild-bis> <schritt> <exit-bild> "<input-skript>" [ENV=WERT ...]
# Ausgabe: re15_port/build/an_lauf/n3_<name>/ (debug.log, stderr.log, state.log, f<NNNNNN>.ppm)
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../../../.." && pwd)"
NAME="$1"; RAUM="$2"; VON="$3"; BIS="$4"; SCHRITT="$5"; ENDE="$6"; SKRIPT="$7"; shift 7
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
LAUF="$BAUM/re15_port/build/an_lauf/n3_$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_n3_${NAME}_$$"
cp "$EXE" "$(dirname "$EXE")/$EXEN.exe" || exit 2
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_TITLE_SHOT="$LAUF/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@250"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_STATE_LOG="$LAUF/state.log" RE15_SPAWN_DIAG=1
export RE15_FRAMEDUMP="${VON}-${BIS}/${SCHRITT}:$LAUF/f"
export RE15_EXIT_AT="${ENDE}"
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
timeout 400 "$(dirname "$EXE")/$EXEN.exe" >stdout.log 2>stderr.log
echo "exit=$? Bilder: $(ls "$LAUF"/f*.ppm 2>/dev/null | wc -l) in $LAUF"
rm -f "$(dirname "$EXE")/$EXEN.exe"
grep -a "\[annette\]" stderr.log debug.log 2>/dev/null | head -40
