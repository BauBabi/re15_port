#!/usr/bin/env bash
# Runde 34 Nacht, Spur F — MESSLAUF an der ECHTEN exe (Leichen ROOM1110 / ROOM1230).
#
#   r34n_f_lauf.sh <marke> <raum-hex>
#
# Umgebung (optional):
#   SPIELER   "x,z,rot,band" fuer RE15_PLAYER_POS (Sprung-Spawn ersetzt), Standard je Raum s.u.
#   PAD_AT    RE15_PAD_AT (bildgenaue Tastenflanken, "<bild>:<tasten>,..."), Standard s.u.
#   SERIE     RE15_FRAMEDUMP-Serie "<start>-<ende>/<schritt>", Standard 300-900/10
#   EXIT      RE15_EXIT_AT (Bild), Standard 950
#   FLAGS     RE15_SET_FLAG (z.B. "9:61"), leer = keine
#   GIVE      RE15_GIVE (Inventar vorbelegen), leer = keine
#   SEK       Wanduhr-Deckel, Standard 120
#   EXE       Pfad der exe; Standard: Kopie der Bau-exe nach build/r34n_f/bin (eigener Name,
#             damit local_build.sh sie nicht beendet und keine fremde exe getroffen wird)
#
# Ausgabe: build/r34n_f/<marke>/ (debug.log, f_NNNNNN.ppm). Der Prozess endet ueber
# RE15_EXIT_AT bzw. timeout am EIGENEN Kind (nie taskkill /IM).
# Rein messend: kein Spielverhalten wird veraendert.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
RAUM="${2:?Raum fehlt (hex, z.B. 1110)}"
case "$RAUM" in
  1110|1111) SP_DEF="10580,2650,0,0" ;;
  1230|1231) SP_DEF="3280,25900,0,0" ;;
  *)         SP_DEF="keine" ;;
esac
SPIELER="${SPIELER:-$SP_DEF}"
BIN="$ROOT/build/r34n_f/bin"
if [ -z "${EXE:-}" ]; then
  mkdir -p "$BIN"
  cp -f "$ROOT/re15_port/build/platform/pc/re15_pc.exe" "$BIN/re15_pc_r34nf.exe" || exit 2
  for dll in "$ROOT"/re15_port/build/platform/pc/*.dll; do
    [ -f "$dll" ] && cp -f "$dll" "$BIN/"
  done
  EXE="$BIN/re15_pc_r34nf.exe"
fi
LAUF="$ROOT/build/r34n_f/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@${JUMP_AB:-240}"
export RE15_MSG_LOG=1 RE15_MODAL_LOG="$(cygpath -m "$LAUF")/modal.log"
if [ "$SPIELER" != "keine" ]; then export RE15_PLAYER_POS="$SPIELER"; fi
export RE15_FRAMEDUMP="${SERIE:-300-900/10}:f_"
export RE15_PAD_AT="${PAD_AT:-330:A,470:A,560:A,700:A,820:A}"
export RE15_EXIT_AT="${EXIT:-950}"
if [ -n "${FLAGS:-}" ]; then export RE15_SET_FLAG="$FLAGS"; fi
if [ -n "${GIVE:-}" ]; then export RE15_GIVE="$GIVE"; fi
timeout -k 5 "${SEK:-120}" "$EXE"
rc=$?
echo "[r34n-f-lauf] $MARKE raum=$RAUM spieler=$SPIELER rc=$rc Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n "AUTO-JUMP\|JUMP ->\|JUMP-Spawn\|\[msg\]\|\[leiche\]\|setflag\|EXIT_AT" debug.log | head -40
