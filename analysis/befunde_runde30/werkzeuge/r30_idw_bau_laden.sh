#!/usr/bin/env bash
# Runde 30 / irons-diary-welt: ABNAHME AM LADE-WEG (CONTINUE) mit dem gebauten Spiel.
#
#   r30_idw_bau_laden.sh <marke> <raum 1150|1151> <cut|auto> [genommen]
#
# Schritt 1: probe_r30_irons_tisch_karte schreibt eine Speicherkarte mit EINEM Spielstand im
#            Raum (Spielerlage SPIELER="x,z,rot", Standard -20500,-24500,0 = weit weg vom
#            Tisch; "genommen" setzt die Bits (9,54)/(9,55) im Spielstand).
# Schritt 2: die exe laedt ihn (RE15_CONTINUE_TEST), Framedump-Serie SERIE (Standard
#            100-300/50), optionale Tasten PAD_AT, Ende EXIT (Standard 320#<raum>).
# Zustand je Wechsel im debug.log ueber RE15_IRONS_LOG. KEIN AUTOSHOT, KEIN SOFTWARE_RENDER.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
RAUM="${2:?Raum fehlt}"
CUT="${3:?Cut fehlt}"
GEN="${4:-}"
SPIELER="${SPIELER:--20500,-24500,0}"
EXE="${EXE:-$ROOT/re15_port/build/platform/pc/re15_pc.exe}"
KARTE="${KARTE_TOOL:-$ROOT/re15_port/build/tests/unit/probe_r30_irons_tisch_karte.exe}"
LAUF="$ROOT/build/r30_irons-diary-welt/bau/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm re15_card.mcr
"$KARTE" re15_card.mcr "$RAUM" ${GEN:+genommen} "pos=$SPIELER" || exit 3
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_IRONS_LOG=1
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
if [ "$CUT" != "auto" ]; then export RE15_FORCE_CUT="$CUT"; fi
export RE15_FRAMEDUMP="${SERIE:-100-300/50}:f_"
export RE15_EXIT_AT="${EXIT:-320#$RAUM}"
if [ -n "${PAD_AT:-}" ]; then export RE15_PAD_AT="$PAD_AT"; fi
if [ -n "${DOC_LOG:-}" ]; then export RE15_DOC_LOG=1; fi
timeout -k 5 "${SEK:-60}" "$EXE"
rc=$?
echo "[bau-laden] $MARKE raum=$RAUM cut=$CUT $GEN spieler=$SPIELER rc=$rc Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n "CONTINUE: resumed\|irons-tisch\]\|prop-render\] pi=[56]" debug.log | head -12
