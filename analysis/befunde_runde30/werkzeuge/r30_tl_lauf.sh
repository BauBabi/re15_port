#!/usr/bin/env bash
# Runde 30 Nachschliff, Spur tischlicht: EIN Mess-Lauf am gebauten Spiel (Framedump).
#
#   r30_tl_lauf.sh <marke> <cut> [null]
#
#   cut   = RE15_FORCE_CUT (0..8)
#   null  = Nullbild: Genommen-Bits (9,54)/(9,55) vor dem Raumeintritt gesetzt -> weder Buch
#           noch Karte werden angelegt (re15_irons_tisch_install).
#
# Umgebung (optional):
#   EXE        exe (Standard: re15_port/build/platform/pc/re15_pc.exe dieses Baums)
#   SPIELER    "x,z,rot" (Standard -20500,-24500,0 = weit weg vom Tisch), "keine" = Tuer-Spawn
#   SERIE      Framedump-Serie (Standard 400-600/100)
#   ENDE       Bild, an dem der Lauf endet (RE15_EXIT_AT, Standard 610)
#   RAUM       Sprungziel (Standard 1150)
#   LICHT_MESS Wert fuer RE15_IRONS_LICHT_MESS (nur die Mess-exe mit dem Messhaken kennt ihn)
#   AUS        Wurzel der Laeufe (Standard build/r30_tischlicht/laeufe)
#   EXTRA_SET_FLAG  weitere Bits fuer RE15_SET_FLAG ("9:55" = nur das Buch, "9:54" = nur die Karte)
#
# Beschleunigter Renderer, RE15_FRAMEDUMP (Readback vor SDL_RenderPresent). KEIN AUTOSHOT,
# KEIN SOFTWARE_RENDER. Der Prozess endet ueber RE15_EXIT_AT bzw. timeout am EIGENEN Kind.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
CUT="${2:?Cut fehlt}"
NULL="${3:-}"
SPIELER="${SPIELER:--20500,-24500,0}"
EXE="${EXE:-$ROOT/re15_port/build/platform/pc/re15_pc.exe}"
RAUM="${RAUM:-1150}"
LAUF="${AUS:-$ROOT/build/r30_tischlicht/laeufe}/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_IRONS_LOG=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@240"
if [ "$SPIELER" != "keine" ]; then export RE15_PLAYER_POS="$SPIELER"; fi
export RE15_FORCE_CUT="$CUT"
export RE15_FRAMEDUMP="${SERIE:-400-600/100}:f_"
export RE15_EXIT_AT="${ENDE:-610}#${RAUM}"
FLAGS=""
if [ "$NULL" = "null" ]; then FLAGS="9:54,9:55"; fi
if [ -n "${EXTRA_SET_FLAG:-}" ]; then FLAGS="${FLAGS:+$FLAGS,}$EXTRA_SET_FLAG"; fi
if [ -n "$FLAGS" ]; then export RE15_SET_FLAG="$FLAGS"; fi
if [ -n "${LICHT_MESS:-}" ]; then export RE15_IRONS_LICHT_MESS="$LICHT_MESS"; fi
timeout -k 5 "${SEK:-60}" "$EXE"
rc=$?
echo "[tl-lauf] $MARKE cut=$CUT ${NULL} spieler=$SPIELER rc=$rc Bilder: $(ls f_*.ppm 2>/dev/null | wc -l) exe=$(basename "$(dirname "$EXE")")/$(basename "$EXE")"
