#!/usr/bin/env bash
# r30_karte3010_szene.sh - die Irons-Szene (ROOM1150 sub08) im ECHTEN Spielablauf fahren
# und protokollieren, was der Port NACH ihrem Ende tut (Runde 30, Thema B, MESSUNG).
#   Spielstand-Kopie laden -> Debug-JUMP ROOM1150 -> RE15_FORCE_EVENT=8 (sub08) -> laufen lassen.
#   Bildabzuege ueber RE15_FRAMEDUMP (komplett komponierter Frame VOR dem Present,
#   render_pc.c) - KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:-szene_1150}"
SEK="${SEK:-100}"
FEUER="${FEUER:-300}"
SERIE="${SERIE:-1800-2100/100}"
BIN="$ROOT/re15_port/build_r30_karte-3010/platform/pc"
OUT="$ROOT/build/r30_karte-3010"
mkdir -p "$OUT"
cd "$BIN" || exit 2
[ -f re15_card.mcr ] || cp "$ROOT/re15_port/build/platform/pc/re15_card.mcr" re15_card.mcr
rm -f debug.log befund.log
mkdir -p mess
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
export RE15_DEBUG_JUMP="1150@gp"
if [ "${FEUER}" != "-" ]; then export RE15_FORCE_EVENT="8@${FEUER}"; else unset RE15_FORCE_EVENT; fi
export RE15_PLAYER_POS="${POS:--20500,-22800,1024}"
export RE15_FLAG_TRACE=1
export RE15_FRAMEDUMP="${SERIE}:mess/${MARKE}_"
timeout -k 5 "$SEK" ./re15_pc.exe > /dev/null 2>&1
rc=$?
cp -f debug.log "$OUT/${MARKE}.debug.log" 2>/dev/null || true
for f in mess/${MARKE}_*.ppm; do [ -f "$f" ] && mv -f "$f" "$OUT/"; done
echo "[szene] $MARKE rc=$rc  Bilder: $(ls "$OUT"/${MARKE}_*.ppm 2>/dev/null | wc -l)"
