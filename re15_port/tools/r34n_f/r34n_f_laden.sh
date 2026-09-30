#!/usr/bin/env bash
# Runde 34 Nacht, Spur F — LADE-LAUF an der ECHTEN exe (Leichen ROOM1110 / ROOM1230).
#
#   r34n_f_laden.sh <marke> <raum-hex> [genommen]
#
# Schreibt mit probe_r34n_f_karte eine Speicherkarte (Stand vor der Leiche, optional mit dem
# Bank-9-Bit), startet die exe ueber den echten Lade-Weg (RE15_CONTINUE_TEST + RE15_CARD_AUTO,
# Slot 0) und drueckt per RE15_PAD_AT das Quadrat. Rein messend.
# Umgebung (optional): PAD_AT (Standard "90:A,200:A,290:A,420:A"), SERIE (Standard 60-600/10),
#   EXIT (Standard 620), SEK (Standard 150).
# Ausgabe: build/r34n_f/<marke>/ (debug.log, modal.log, f_NNNNNN.ppm).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
RAUM="${2:?Raum fehlt (hex, z.B. 1110)}"
GEN="${3:-}"
BIN="$ROOT/build/r34n_f/bin"
mkdir -p "$BIN"
cp -f "$ROOT/re15_port/build/platform/pc/re15_pc.exe" "$BIN/re15_pc_r34nf.exe" || exit 2
for dll in "$ROOT"/re15_port/build/platform/pc/*.dll; do [ -f "$dll" ] && cp -f "$dll" "$BIN/"; done
EXE="$BIN/re15_pc_r34nf.exe"
LAUF="$ROOT/build/r34n_f/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log modal.log f_*.ppm re15_card.mcr
"$ROOT/re15_port/build/tests/unit/probe_r34n_f_karte.exe" re15_card.mcr "$RAUM" $GEN || exit 3
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_MSG_LOG=1 RE15_MODAL_LOG="$(cygpath -m "$LAUF")/modal.log"
export RE15_FRAMEDUMP="${SERIE:-60-600/10}:f_"
export RE15_PAD_AT="${PAD_AT:-90:A,200:A,290:A,420:A}"
export RE15_EXIT_AT="${EXIT:-620}"
timeout -k 5 "${SEK:-150}" "$EXE"
rc=$?
echo "[r34n-f-laden] $MARKE raum=$RAUM $GEN rc=$rc Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n "\[card\]\|LOAD\|loaded room\|\[msg\] room=\|\[leiche\]\|EXIT_AT" debug.log | head -30
