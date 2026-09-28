#!/usr/bin/env bash
# r30_5080_echtlauf.sh - ECHTLAUF der echten re15_pc.exe in ROOM5080 (Generator-Folge).
# Dossier: analysis/befunde_runde30/nachschliff-room5080.md
#
# Weg: Vorspann/Titel durchklicken -> RE15_DEBUG_JUMP (Original-UTILITY-MENU-Pfad) ->
#      RE15_INPUT_SCRIPT (Pad-Bits wie ein Spieler).
# Messschienen (alle Dateien im Zielverzeichnis, das auch das Arbeitsverzeichnis der exe ist,
# damit debug.log und befund.log dort landen):
#   RE15_STATE_LOG   - je Bild: pad, Spieler (x,z,rot,hp,state,sub,phase,motion), pauseflags,
#                      Nachricht, und jeder lebende Aktor (Typ, state, grid, Lage)
#   RE15_DISCARD_LOG - je Bild raum/msg/px/pz
#   debug.log        - [scd]-Zeilen (Plc_dest, Cut_chg, letterbox, thread start)
#
# Aufruf: r30_5080_echtlauf.sh <ziel> "<input-skript>" [start] [sekunden] [zusatz-env ...]
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"
SKRIPT="${2:-W1}"
START="${3:-200}"
SEK="${4:-80}"
shift 4 2>/dev/null || shift $#
EXE="${RE15_EXE:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
mkdir -p "$ZIEL"
ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/state.log "$ZIEL"/lage.log "$ZIEL"/debug.log "$ZIEL"/befund.log

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM:-5080}@${JUMP_BILD:-120}"
export RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_INPUT_SCRIPT_START="$START"
export RE15_STATE_LOG="$ZIEL/state.log"
export RE15_DISCARD_LOG="$ZIEL/lage.log"
for kv in "$@"; do export "$kv"; done
cd "$ZIEL"
timeout -k 5 "$SEK" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt" &
PID=$!
wait $PID
echo "exit=$?"
EXE_DIR="$(dirname "$EXE")"
cp "$EXE_DIR"/befund.log "$ZIEL"/befund.log 2>/dev/null || true
echo "--- debug.log [scd]"
grep -a "\[scd" "$ZIEL/debug.log" 2>/dev/null | grep -v "thread start" | tail -40
