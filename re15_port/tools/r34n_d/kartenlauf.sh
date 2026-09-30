#!/usr/bin/env bash
# kartenlauf.sh - Spur D (Runde 34 Nacht, Gegenpruefung Auflage 8a): CONTINUE-Lauf der ECHTEN exe
# mit einer Speicherkarte aus probe_r34n_d_karte (Stand in ROOM1150), dann RE15_DEBUG_JUMP in die
# Umkleide ROOM1000, durch deren Tuer Slot 0 in den 1050-Sued, 0,8 s vorwaerts, Quadrat an der
# 10A0-Tuer. Die Flags kommen aus dem GELADENEN Spielstand. Eigene exe-Kopie + eigenes
# Arbeitsverzeichnis (die Karte liegt dort), Muster tests/integration/test_r30_granate_laden.cmake.
#
# Aufruf: kartenlauf.sh <zielverz> <gesehen|gerettet|gesehen,gerettet> [KEY=VALUE ...]
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"; FLAGS="${2:?gesehen|gerettet fehlt}"; shift 2
ORG="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r34n_d_karte.exe"
KARTE_TOOL="$BAUM/re15_port/build/tests/unit/probe_r34n_d_karte.exe"
cp -f "$ORG" "$EXE"
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/re15_card.mcr
(cd "$ZIEL" && "$KARTE_TOOL" re15_card.mcr ${FLAGS//,/ }) || { echo "Kartenwerkzeug fehlgeschlagen"; exit 1; }
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_DEBUG_JUMP=1000@60 RE15_PLAYER_POS=22230,-13400,0,0
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14
export RE15_PRESS=square@150
for kv in "$@"; do export "$kv"; done
cd "$ZIEL"
timeout -k 5 200 "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
