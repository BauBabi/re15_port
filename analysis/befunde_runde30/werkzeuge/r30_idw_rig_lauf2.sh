#!/usr/bin/env bash
# Runde 30 / irons-diary-welt: Messlauf des MESS-RIGS, zweite Fassung.
# Die erste (r30_idw_rig_lauf.sh) sprang mit "1150@gp" und kam in 90 s nie aus dem Intro
# heraus (Log c2_ohne.log: Raum 1170 bis Bild 930). Hier: fester Sprung-Frame wie im
# Schwester-Thema (sicherung_werkzeug/lauf.sh) und RE15_PLAYER_POS.
#
#   r30_idw_rig_lauf2.sh <marke> <cut> <otmax> [spieler "x,z,rot"]
#
# Bilder: RE15_FRAMEDUMP (Readback vor SDL_RenderPresent), KEIN AUTOSHOT, KEIN SOFTWARE_RENDER.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
CUT="${2:?Cut fehlt}"
OTMAX="${3:-0}"
SPIELER="${4:--20500,-24500,0}"
AUS="$ROOT/build/r30_irons-diary-welt"
EXE="$AUS/rig_build/platform/pc/re15_pc.exe"
LAUF="$AUS/rig_lauf/$MARKE"
mkdir -p "$LAUF"
cd "$LAUF" || exit 2
rm -f debug.log f_*.ppm pri.log
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1150@${JUMP_AB:-240}"
export RE15_PLAYER_POS="$SPIELER"
# Cut "auto" = KEIN Zwang, die Kamerazonen des Raums entscheiden
if [ "$CUT" != "auto" ]; then export RE15_FORCE_CUT="$CUT"; fi
export RE15_PRI_LOG="$(cygpath -m "$LAUF")/pri.log"
export RE15_FRAMEDUMP="${SERIE:-400-600/100}:f_"
[ -n "${RIG_DIARY:-}" ] || export RIG_DIARY="-23656,-1533,-18291,3072"
[ -n "${RIG_KARTE:-}" ] || export RIG_KARTE="-23657,-1520,-18649,3072"
export RIG_DIARY_MD1="$WROOT/extracted_re2_dokumente/weltmodelle/mesh03_cf9f316d.md1"
export RIG_DIARY_TIM="$WROOT/extracted_re2_dokumente/weltmodelle/mesh03_cf9f316d.tim"
export RIG_KARTE_MD1="${KARTE_MD1:-$(cygpath -m "$AUS")/rig_karte.md1}"
export RIG_KARTE_TIM="${KARTE_TIM:-$(cygpath -m "$AUS")/rig_karte.tim}"
export RIG_OTMAX="$OTMAX"
# timeout beendet nur den EIGENEN Kindprozess (nie taskkill /IM)
timeout -k 5 "${SEK:-40}" "$EXE"
echo "[rig2] $MARKE cut=$CUT otmax=$OTMAX spieler=$SPIELER Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n "rig\]\|AUTO-JUMP\|JUMP ->\|\[pri\] cut=$CUT" debug.log | head -12
