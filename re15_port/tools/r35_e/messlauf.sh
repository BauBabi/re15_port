#!/usr/bin/env bash
# messlauf.sh - Runde 35 Spur E (Kopie von r34n_d/messlauf.sh, eigener exe-Name): Messlauf der ECHTEN exe in ROOM1050 an der Tuer nach ROOM10A0.
#
# Weg wie tools/tueren/tuer_echtlauf_taste.sh (Runde 33): Titel-Autovorlauf -> RE15_DEBUG_JUMP (Original-
# UTILITY-MENU) -> Spawn ersetzt durch RE15_PLAYER_POS (Standplatz aus probe_r33_tueren standplatz 1050 10A0
# = 16580,-13700,0,0) -> Tasten per RE15_INPUT_SCRIPT oder RE15_PRESS. Eigener exe-Name, damit
# local_build.sh / andere Agenten den Lauf nicht beenden. Bilder: RE15_FRAMEDUMP (komplett komponiert,
# vor dem Present). Alle weiteren Schalter als KEY=VALUE-Argumente.
#
# Aufruf: messlauf.sh <zielverz> <sekunden> [KEY=VALUE ...]
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-120}"
shift 2
ORG="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r35e.exe"
cp -f "$ORG" "$EXE"
mkdir -p "$ZIEL"
ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/stdout.txt

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE=3
for kv in "$@"; do export "$kv"; done
cd "$(dirname "$EXE")"
rm -f debug.log
timeout -k 5 "$SEK" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
cp -f "$(dirname "$EXE")/debug.log" "$ZIEL/debug.log" 2>/dev/null
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
