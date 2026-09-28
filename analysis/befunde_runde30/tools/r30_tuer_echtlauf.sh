#!/usr/bin/env bash
# r30_tuer_echtlauf.sh — ECHTLAUF der echten re15_pc.exe fuer den "Tuer verschlossen"-Ton.
# Dossier: analysis/befunde_runde30/tuer-verschlossen.md (Plan §5 Schritt 7).
#
# Weg an die Stelle (dieselben Mess-Haken wie analysis/befunde_2026-09-22/discard-fenster/abzug_1100.sh):
#   RE15_NO_INTRO / RE15_TITLE_SHOT  - Vorspann + Titel durchklicken
#   RE15_DEBUG_JUMP=<raum>@gp        - Debug-Menue-Sprung (Original-UTILITY-MENU-Pfad), sobald spielbar
#   RE15_INPUT_SCRIPT(_START)        - Laufen/Drehen/QUADRAT wie ein Spieler (Pad-Bits)
# Messschienen:
#   RE15_TUERSE_LOG                  - je Ausloesung des Tuer-Tons eine Zeile (F<bild> raum nachricht art satz)
#   RE15_DISCARD_LOG                 - je Bild raum/msg_aktiv/px/pz (Lage des Spielers)
#   RE15_AUDIO_CAP_SYNC              - die GEMISCHTE Ausgabe, bildgetaktet (Offset = Bild * 1470 Stereo-Frames)
#   RE15_MSG_LOG                     - (stderr, falls vorhanden)
#
# Aufruf: r30_tuer_echtlauf.sh <ziel> <raum-hex> "<input-skript>" [start] [sekunden] [bilder]
set -uo pipefail
cd "$(dirname "$0")/../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
RAUM="${2:?Raum fehlt}"
SKRIPT="${3:-W1}"
START="${4:-200}"
SEK="${5:-60}"
BILDER="${6:-1200}"
EXE_DIR="${RE15_EXE_DIR:-re15_port/build/platform/pc}"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/tuerse.log "$ZIEL"/lage.log "$ZIEL"/audio.raw "$EXE_DIR"/befund.log "$EXE_DIR"/befund.1.log

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@${JUMP_BILD:-120}"
export RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_INPUT_SCRIPT_START="$START"
export RE15_TUERSE_LOG="$ZIEL/tuerse.log"
export RE15_DISCARD_LOG="$ZIEL/lage.log"
export RE15_AUDIO_CAP_SYNC="$ZIEL/audio.raw"
export RE15_AUDIO_CAP_FRAMES="$BILDER"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
cp "$EXE_DIR"/befund.log "$ZIEL"/befund.log 2>/dev/null || true
echo "--- tuerse.log"
cat "$ZIEL/tuerse.log" 2>/dev/null || echo "KEINE Zeile"
echo "--- Lage: erste/letzte Zeile im Zielraum"
grep "raum=$(echo "$RAUM" | tr 'A-F' 'a-f')" "$ZIEL/lage.log" 2>/dev/null | head -1
grep "raum=$(echo "$RAUM" | tr 'A-F' 'a-f')" "$ZIEL/lage.log" 2>/dev/null | tail -1
