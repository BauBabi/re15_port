#!/usr/bin/env bash
# LIVE-ABZUG an der Stelle der Nutzer-Marke F3843 (ROOM10D0, Cut 7, pos 418/0/26497,
# rot -2361) aus dem LAUFENDEN Spiel.
#
# ECHTER RENDERPFAD: RE15_FRAMEDUMP liest das KOMPLETT komponierte Bild INNERHALB von
# re15_render_end_frame() unmittelbar VOR SDL_RenderPresent zurueck (main.c:9597-9625,
# render_pc.c) — kein RE15_AUTOSHOT, kein Softwarerenderer.
#
# Weg in den Raum (dieselbe Kette wie in den Dossiers ada-eintritt-1050 §1.1 und
# birkin-restluecken §6): RE15_NO_INTRO ueberspringt den Vorspannfilm,
# RE15_TITLE_SHOT/_AF klickt den Titel auf NEW GAME weiter, RE15_DEBUG_JUMP nimmt den
# Debug-Menue-Sprung (derselbe Pfad wie ein Quadrat-Druck), RE15_PLAYER_POS setzt die
# Spielerlage auch fuer den Sprung-Spawn (main.c:6248), RE15_FORCE_CUT nagelt den Winkel.
#
# Aufruf: abzug_F3843.sh <ziel> [sekunden] [sprungframe] [von] [bis] [schritt]
set -euo pipefail
cd "$(dirname "$0")/../../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-90}"
SPRUNG="${3:-120}"
VON="${4:-200}"
BIS="${5:-260}"
SCHRITT="${6:-20}"
EXE_DIR="re15_port/build_r19c/platform/pc"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/bild*.ppm debug.log
export RE15_NO_INTRO=1
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="10D0@$SPRUNG"
export RE15_PLAYER_POS=418,26497,-2361
export RE15_FORCE_CUT=7
export RE15_FRAMEDUMP="$VON-$BIS/$SCHRITT:$ZIEL/bild"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" || true
echo "--- Abzuege"
ls -la "$ZIEL"/bild*.ppm 2>&1 || echo "KEINE"
[ -e debug.log ] && cp debug.log "$ZIEL/"
[ -e "$EXE_DIR/befund.log" ] && cp "$EXE_DIR/befund.log" "$ZIEL/"
echo "--- debug.log (Auszug)"
grep -E "AUTO-JUMP|JUMP ->|parity|readback|room" debug.log 2>/dev/null | tail -10 || true
echo "--- befund.log, letzte Zeile in ROOM10D0 C7"
grep -E "R10D0 C7" "$EXE_DIR/befund.log" 2>/dev/null | tail -1 | cut -c1-160 || true
