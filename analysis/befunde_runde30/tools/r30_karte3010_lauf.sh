#!/usr/bin/env bash
# r30_karte3010_lauf.sh - EIN Kartenschirm-Abzug des PC-Ports (Runde 30, Thema B).
#   $1 = Marke (Dateiname ohne Endung, landet in build/r30_karte-3010/)
#   $2 = Raum (hex, z.B. 1150), in den per Debug-Menue gesprungen wird
#   $3 = Kartenblatt (hex) fuer RE15_MAP_SHOT_PAGE, oder "-" = das Blatt, das der
#        Port von selbst zeigt (Ist-Zustand des Spielstands)
# Laeuft NUR im eigenen Bauverzeichnis re15_port/build_r30_karte-3010 (re15_port/build
# gehoert anderen). Der Spielstand ist eine KOPIE der Nutzer-Karte (nur gelesen).
# Abzug ueber RE15_INV_FB_SHOT = der CPU-gerasterte Statusschirm s_fb5
# (inv_render_pc.c:1108ff) - fenster- und sitzungsunabhaengig, KEIN RE15_AUTOSHOT.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"; RAUM="${2:?Raum fehlt}"; BLATT="${3:--}"
SEK="${SEK:-150}"
BIN="$ROOT/re15_port/build_r30_karte-3010/platform/pc"
OUT="$ROOT/build/r30_karte-3010"
mkdir -p "$OUT"
cd "$BIN" || exit 2
[ -f re15_card.mcr ] || cp "$ROOT/re15_port/build/platform/pc/re15_card.mcr" re15_card.mcr
rm -f debug.log befund.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
export RE15_DEBUG_JUMP="${RAUM}@gp"
export RE15_INV_OPEN_AT="90#${RAUM}"
export RE15_INV_FB_SHOT="$(cygpath -w "$OUT/${MARKE}.bmp")" RE15_INV_FB_SHOT_AT=70
export RE15_BEFUND_MARKE=400
if [ "$BLATT" != "-" ]; then export RE15_MAP_SHOT_PAGE="$BLATT"; else unset RE15_MAP_SHOT_PAGE; fi
timeout -k 5 "$SEK" ./re15_pc.exe > "$OUT/${MARKE}.stdout.txt" 2> "$OUT/${MARKE}.stderr.txt"
rc=$?
cp -f debug.log "$OUT/${MARKE}.debug.log" 2>/dev/null || true
cp -f befund.log "$OUT/${MARKE}.befund.log" 2>/dev/null || true
echo "[lauf] $MARKE Raum=$RAUM Blatt=$BLATT rc=$rc  Abzug: $(ls -la "$OUT/${MARKE}.bmp" 2>/dev/null | awk '{print $5" B"}')"
