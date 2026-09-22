#!/usr/bin/env bash
# LIVE-ABZUG der "Discard it?"-Abfrage aus dem LAUFENDEN Spiel — ROOM10D0, Kartenleser.
# Runde 22: zusaetzlich der PAD-RIEGEL. Nach den Quadrat-Stoessen wird VORWAERTS gedrueckt,
# solange die Abfrage vorgemerkt und noch unsichtbar ist. Im Protokoll muessen px/pz dabei
# stehen bleiben (padsperre=1) — das ist die Lage, in der RE2 sich waehrend derselben
# Spanne befindet (0xFF000000, @0x80051650 -> @0x8002fe90 -> FUN_8003027c case 0, zurueck
# erst LAB_800307e0, sofort wieder @0x80051850).
#
# ECHTER RENDERPFAD: RE15_FRAMEDUMP liest das KOMPLETT komponierte Bild INNERHALB von
# re15_render_end_frame() unmittelbar VOR SDL_RenderPresent zurueck — kein RE15_AUTOSHOT,
# kein Softwarerenderer.
#
# Weg an die Stelle (unveraendert aus analysis/befunde_2026-09-22/discard-nacharbeit/):
#   RE15_NO_INTRO / RE15_TITLE_SHOT  - Vorspann + Titel durchklicken
#   RE15_DEBUG_JUMP=10D0@120         - Debug-Menue-Sprung (derselbe Pfad wie Quadrat)
#   RE15_PLAYER_POS=800,-5750,2048   - vor den Kartenleser (AOT-Slot 1, ROOM10D0.RDT
#                                      @0x103E, rect(400,-6150,800,800), Nutzlast `18 14`
#                                      = Ereignis sub20)
#   RE15_GIVE=0x38:1                 - Blaue Keycard ins Inventar
#   RE15_SET_FLAG=9:52               - "genommen"-Bit, das der Leser prueft (@0x19C0)
#   RE15_DISCARD_LOG                 - Messchiene, JEDE Zeile, mit padsperre/px/pz
#
# Aufruf: abzug_discard.sh <ziel> [sekunden] [von] [bis] [schritt] [stoesse]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-70}"
VON="${3:-300}"
BIS="${4:-700}"
SCHRITT="${5:-4}"
STOESSE="${6:-12}"
EXE_DIR="${RE15_EXE_DIR:-re15_port/build/platform/pc}"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/bild*.ppm "$ZIEL"/discard.log debug.log

# Quadrat-Stoesse: 0,2 s druecken / 0,3 s los. Danach 6 s VORWAERTS (der Beweis, dass der
# Spieler waehrend der Vormerkung nicht laeuft), dann Stille fuer das Standbild.
STOSS=""
for i in $(seq 1 "$STOESSE"); do STOSS="${STOSS}A0.2,W0.3,"; done

export RE15_NO_INTRO=1
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="10D0@120"
export RE15_PLAYER_POS=800,-5750,2048
export RE15_GIVE="0x38:1"
export RE15_SET_FLAG="9:52"
export RE15_INPUT_SCRIPT="W1,${STOSS}U6,W40"
export RE15_INPUT_SCRIPT_START=200
export RE15_DISCARD_LOG="$ZIEL/discard.log"
export RE15_FRAMEDUMP="$VON-$BIS/$SCHRITT:$ZIEL/bild"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" || true

echo "--- Abzuege"
ls "$ZIEL"/bild*.ppm 2>&1 | head -40 || echo "KEINE"
echo "--- erste Zeile mit vorgemerkter Abfrage (abfrage=1)"
grep -n "abfrage=1" "$ZIEL/discard.log" 2>/dev/null | head -2 || echo "KEINE"
echo "--- erste/letzte Zeile mit stehender Abfrage (frage=8)"
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | head -2 || echo "KEINE"
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | tail -2 || true
echo "--- WARTEFENSTER: vorgemerkt, noch unsichtbar"
grep -n "abfrage=1 frage=0" "$ZIEL/discard.log" 2>/dev/null | head -3 || echo "KEINE"
grep -n "abfrage=1 frage=0" "$ZIEL/discard.log" 2>/dev/null | tail -3 || true
echo "--- Wieviele Wartebilder mit FREIEM Pad? (muss 0 sein)"
grep -c "abfrage=1 frage=0 .*padsperre=0 pausepad=0" "$ZIEL/discard.log" 2>/dev/null || echo 0
