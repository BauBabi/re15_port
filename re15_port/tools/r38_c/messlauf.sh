#!/usr/bin/env bash
# messlauf.sh - Runde 38 Spur C "werkstatt11b0": Messlauf der ECHTEN exe (Muster tools/r36_j/messlauf.sh).
# Titel-Autovorlauf -> Schalter als KEY=VALUE-Argumente (RE15_DEBUG_JUMP, RE15_SET_FLAG, RE15_FRAMEDUMP ...).
# Eigener exe-Name (re15_pc_r38c.exe): local_build.sh / andere Agenten beenden den Lauf nicht.
# Bilder: RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1; Renderer direct3d11 (Runde-36-J-Falle: D3D9 DEVICELOST).
#
# Aufruf: messlauf.sh <zielverz> <sekunden> [KEY=VALUE ...]
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-120}"
shift 2
ORG="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r38c.exe"
cp -f "$ORG" "$EXE"
mkdir -p "$ZIEL"
ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/stdout.txt

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_WINDOW_SCALE=1
export SDL_RENDER_DRIVER="${SDL_RENDER_DRIVER:-direct3d11}"
export SDL_AUDIODRIVER="${SDL_AUDIODRIVER:-dummy}"
for kv in "$@"; do export "$kv"; done
cd "$(dirname "$EXE")"
rm -f debug.log
timeout -k 5 "$SEK" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
cp -f "$(dirname "$EXE")/debug.log" "$ZIEL/debug.log" 2>/dev/null
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
