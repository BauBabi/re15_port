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
#
# ZEITBASIS DES SKRIPTS (Nachbesserung Gegenpruefer, Punkt 4): Standard ist hier
# RE15_INPUT_SCRIPT_BASIS=spiel (Skript-Tick 0 = Spielbild <start>, input_pc.c:70-74/110-111).
# Ohne diesen Schalter zaehlt der Parser die gerenderten Bilder ab Programmstart; beim
# Tuerweg (RAUM=6010 + RE15_FIRE_AOT) verschiebt sich das Skript dann um ~123 Bilder
# (gemessen: "Tick 0 -> F160" mit spiel, "Tick 0 -> F37" ohne), und die Generator-AOT wird
# nie ausgeloest. BASIS=roh schaltet ihn ab (so lief der Gegenpruefer-Nachlauf "tuerweg").
#   Tuerweg (Dossier 4.2/4.5):
#     RAUM=6010 r30_5080_echtlauf.sh <ziel> "<skript>" 160 80 RE15_FIRE_AOT=2@60#6010
#   Debug-Sprung (Dossier 4.1, Lauf lauf1 lief mit der ROHEN Basis: "Tick 0 -> F77"):
#     BASIS=roh r30_5080_echtlauf.sh <ziel> "<skript>" 200 80
#   Birkin vor der Folge (Dossier 8.1, Gegenpruefer-Lauf "tuerweg" = Tuerweg mit ROHER Basis:
#   das Skript laeuft in ROOM6010 an, Leon bleibt in ROOM5080 bei (-23350,-18200) stehen):
#     BASIS=roh RAUM=6010 r30_5080_echtlauf.sh <ziel> "<tuerweg-skript>" 160 80 RE15_FIRE_AOT=2@60#6010
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
if [ "${BASIS:-spiel}" = "roh" ]; then unset RE15_INPUT_SCRIPT_BASIS
else export RE15_INPUT_SCRIPT_BASIS="${BASIS:-spiel}"; fi
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
