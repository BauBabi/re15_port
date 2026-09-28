#!/usr/bin/env bash
# Runde 30 / irons-diary-welt: ABNAHME-LAUF am GEBAUTEN SPIEL (kein Rig).
#
#   r30_idw_bau_lauf.sh <marke> <cut|auto> [null]
#
#   cut   = RE15_FORCE_CUT (2 = Cut des Nutzerbilds, 6 = Nahaufnahme), "auto" = kein Zwang
#   null  = Nullbild: setzt die Genommen-Bits (9,54) und (9,55) VOR dem Raumeintritt,
#           dann legt re15_irons_tisch_install weder Buch noch Karte an.
#
# Umgebung (optional): SPIELER="x,z,rot" (Standard -20500,-24500,0 = weit weg vom Tisch),
#   SERIE (Framedump-Serie, Standard 400-600/100), SEK (Wanduhr, Standard 40),
#   PAD_AT (RE15_PAD_AT, Tastenflanken), EXIT (RE15_EXIT_AT), JUMP_AB (Standard 240),
#   EXE (Pfad der exe), EXTRA_SET_FLAG (weitere Bits fuer RE15_SET_FLAG).
#
# Bilder: RE15_FRAMEDUMP (Readback vor SDL_RenderPresent, beschleunigter Renderer).
# KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER. Der Prozess endet ueber timeout am EIGENEN
# Kind (nie taskkill /IM).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
CUT="${2:?Cut fehlt}"
NULL="${3:-}"
SPIELER="${SPIELER:--20500,-24500,0}"
EXE="${EXE:-$ROOT/re15_port/build/platform/pc/re15_pc.exe}"
LAUF="$ROOT/build/r30_irons-diary-welt/bau/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_IRONS_LOG=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1150@${JUMP_AB:-240}"
export RE15_PLAYER_POS="$SPIELER"
if [ "$CUT" != "auto" ]; then export RE15_FORCE_CUT="$CUT"; fi
export RE15_FRAMEDUMP="${SERIE:-400-600/100}:f_"
FLAGS=""
if [ "$NULL" = "null" ]; then FLAGS="9:54,9:55"; fi
if [ -n "${EXTRA_SET_FLAG:-}" ]; then FLAGS="${FLAGS:+$FLAGS,}$EXTRA_SET_FLAG"; fi
if [ -n "$FLAGS" ]; then export RE15_SET_FLAG="$FLAGS"; fi
if [ -n "${PAD_AT:-}" ]; then export RE15_PAD_AT="$PAD_AT"; fi
if [ -n "${EXIT:-}" ]; then export RE15_EXIT_AT="$EXIT"; fi
if [ -n "${DOC_LOG:-}" ]; then export RE15_DOC_LOG=1; fi
timeout -k 5 "${SEK:-40}" "$EXE"
rc=$?
echo "[bau-lauf] $MARKE cut=$CUT ${NULL} spieler=$SPIELER rc=$rc Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n "irons-tisch\] Boot\|JUMP ->\|AUTO-JUMP\|setflag\|prop-render\] pi=[56]" debug.log | head -12
