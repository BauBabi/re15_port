#!/usr/bin/env bash
# Runde 30 Nachschliff, Spur cut-blitz: EIN Messlauf am gebauten Spiel.
#
#   r30_cb_lauf.sh <marke>
#
# Umgebung (alle optional):
#   RAUM      Sprungziel RE15_DEBUG_JUMP (hex, Standard 1150); "intro" = kein Sprung, NEW GAME
#             mit Vorspann (ROOM1240-Montage -> ROOM1170)
#   SPIELER   "x,z,rot" -> RE15_PLAYER_POS; "keine" (Standard) = Tuer-Spawn des Sprungs
#   PAD_AT    RE15_PAD_AT (bildgenaue Tastenflanken)
#   SKRIPT    RE15_INPUT_SCRIPT (gehaltene Tasten, Zeitachse = Spielbilder ab SKRIPT_AB)
#   SKRIPT_AB RE15_INPUT_SCRIPT_START (Standard 60); SKRIPT_BASIS=tick = Zeitachse ab Programmstart
#   SERIE     RE15_FRAMEDUMP-Serie "<a>-<b>/<s>" (Standard keine)
#   EXIT      RE15_EXIT_AT (Standard keiner), SEK Wanduhr (Standard 60)
#   EXE       Pfad der exe (Standard: Bauverzeichnis des Baums)
#   CUT       RE15_FORCE_CUT (Standard keiner)
#   FLAGS     RE15_SET_FLAG
#
# Messschienen: RE15_CUT_SYNC_LOG (je Bild bg/view/sync, main.c pc_cut_sync_log),
# RE15_CAM_TRACE (req/shown), RE15_FADE_LOG (bg-log load/blit, CUT a -> b).
# Bilder nur ueber RE15_FRAMEDUMP (Readback vor SDL_RenderPresent, beschleunigter Renderer);
# KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER. Ende ueber timeout am EIGENEN Kind.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
EXE="${EXE:-$ROOT/re15_port/build/platform/pc/re15_pc.exe}"
LAUF="$ROOT/build/r30_cut_blitz/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm cutsync.log
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_CUT_SYNC_LOG="cutsync.log" RE15_CAM_TRACE=1 RE15_FADE_LOG=1
if [ "${RAUM:-1150}" != "intro" ]; then
    export RE15_NO_INTRO=1
    export RE15_DEBUG_JUMP="${RAUM:-1150}@${JUMP_AB:-240}"
fi
SP="${SPIELER:-keine}"
if [ "$SP" != "keine" ]; then export RE15_PLAYER_POS="$SP"; fi
if [ -n "${PAD_AT:-}" ]; then export RE15_PAD_AT="$PAD_AT"; fi
if [ -n "${SKRIPT:-}" ]; then
    export RE15_INPUT_SCRIPT="$SKRIPT"
    # SKRIPT_BASIS=tick: Zeitachse = gerenderte Bilder ab Programmstart (wie der Gegenpruefer-Lauf
    # telefon2); Standard "spiel" = g_engine.frame_count
    if [ "${SKRIPT_BASIS:-spiel}" = "spiel" ]; then export RE15_INPUT_SCRIPT_BASIS=spiel; fi
    export RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-60}"
fi
if [ -n "${SERIE:-}" ]; then export RE15_FRAMEDUMP="$SERIE:f_"; fi
if [ -n "${EXIT:-}" ]; then export RE15_EXIT_AT="$EXIT"; fi
if [ -n "${CUT:-}" ]; then export RE15_FORCE_CUT="$CUT"; fi
if [ -n "${FLAGS:-}" ]; then export RE15_SET_FLAG="$FLAGS"; fi
timeout -k 5 "${SEK:-60}" "$EXE"
rc=$?
N=$(wc -l < cutsync.log 2>/dev/null || echo 0)
B=$(grep -c " sync=0 " cutsync.log 2>/dev/null)
echo "[cb-lauf] $MARKE rc=$rc Bilder=$N sync0=$B dumps=$(ls f_*.ppm 2>/dev/null | wc -l)"
