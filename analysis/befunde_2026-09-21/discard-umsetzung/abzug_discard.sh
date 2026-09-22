#!/usr/bin/env bash
# LIVE-ABZUG der "Discard it?"-Abfrage aus dem LAUFENDEN Spiel — ROOM10D0, Kartenleser.
#
# ECHTER RENDERPFAD: RE15_FRAMEDUMP liest das KOMPLETT komponierte Bild INNERHALB von
# re15_render_end_frame() unmittelbar VOR SDL_RenderPresent zurueck — kein RE15_AUTOSHOT,
# kein Softwarerenderer. (Gleiche Kette wie analysis/befunde_2026-09-21/
# pri-runde19c-saum/protokoll/abzug_F3843.sh.)
#
# Weg an die Stelle:
#   RE15_NO_INTRO / RE15_TITLE_SHOT  - Vorspann + Titel durchklicken
#   RE15_DEBUG_JUMP=10D0@120         - Debug-Menue-Sprung (derselbe Pfad wie Quadrat)
#   RE15_PLAYER_POS                  - vor den Kartenleser. Die Zone ist AOT-Slot 1,
#                                      ROOM10D0.RDT @0x103E: `2c 01 03 31 00 00 90 01 fa e7
#                                      20 03 20 03 ff 00 18 14` = rect(400,-6150,800,800)
#                                      -> Mitte (800,-5750); Nutzlast `18 14` = Ereignis
#                                      sub20 (der Kartenleser).
#   RE15_GIVE=0x38:1                 - Blaue Keycard ins Inventar
#   RE15_SET_FLAG=9:52               - das "genommen"-Bit der Blauen Keycard. Genau das
#                                      prueft der Kartenleser (@0x19C0 `21 09 34 01`), NICHT
#                                      das Inventar.
#   RE15_INPUT_SCRIPT                - Quadrat-Stoesse: Zone ausloesen, Frage-Seiten
#                                      weiterklicken, "Yes" bestaetigen. Danach STILLE,
#                                      damit die Abfrage stehen bleibt und abgelichtet
#                                      werden kann.
#   RE15_DISCARD_LOG                 - Messchiene: je Bild eine Zeile (auch wenn nichts
#                                      passiert), damit ein Fehllauf nicht als leere Datei
#                                      endet.
#
# Aufruf: abzug_discard.sh <ziel> [sekunden] [von] [bis] [schritt]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-70}"
VON="${3:-560}"
BIS="${4:-760}"
SCHRITT="${5:-10}"
EXE_DIR="${RE15_EXE_DIR:-build_discard/platform/pc}"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/bild*.ppm "$ZIEL"/discard.log debug.log

# Quadrat-Stoesse: 0,2 s druecken / 0,3 s los, 26 mal = 13 s. Danach nichts mehr.
STOSS=""
for i in $(seq 1 26); do STOSS="${STOSS}A0.2,W0.3,"; done

export RE15_NO_INTRO=1
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="10D0@120"
export RE15_PLAYER_POS=800,-5750,2048
export RE15_GIVE="0x38:1"
export RE15_SET_FLAG="9:52"
export RE15_INPUT_SCRIPT="W1,${STOSS}W40"
export RE15_INPUT_SCRIPT_START=200
export RE15_DISCARD_LOG="$ZIEL/discard.log"
export RE15_FRAMEDUMP="$VON-$BIS/$SCHRITT:$ZIEL/bild"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" || true

echo "--- Abzuege"
ls "$ZIEL"/bild*.ppm 2>&1 | head -30 || echo "KEINE"
echo "--- Messchiene: Zeilen mit stehender Abfrage"
grep -c "frage=8" "$ZIEL/discard.log" 2>/dev/null || echo 0
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | head -3 || true
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | tail -3 || true
echo "--- erste Zeile, in der die Abfrage wartet"
grep -n "abfrage=1" "$ZIEL/discard.log" 2>/dev/null | head -2 || echo "KEINE"
