#!/usr/bin/env bash
# Runde 30 / irons-diary-welt: EIN Messlauf des MESS-RIGS (Kopie der Port-Quellen mit den
# zwei Probe-Props, s. r30_idw_rig_patch.py). Das Repo-Spiel wird NICHT angefasst.
#
#   r30_idw_rig_lauf.sh <marke> <cut> [otmax]
#
# Ergebnis: build/r30_irons-diary-welt/rig_lauf/<marke>_NNNNNN.ppm + <marke>.log
#
# MSYS_NO_PATHCONV & Co. sind Pflicht (analysis/befunde_2026-09-27/elza_run.sh): sonst
# zerlegt Git-Bash den RE15_FRAMEDUMP-Wert als PATH-Liste.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
CUT="${2:?Cut fehlt}"
OTMAX="${3:-0}"
AUS="$ROOT/build/r30_irons-diary-welt"
EXE="$AUS/rig_build/platform/pc/re15_pc.exe"
LAUF="$AUS/rig_lauf"
mkdir -p "$LAUF"
cd "$LAUF"
rm -f debug.log
WROOT="$(cygpath -m "$ROOT")"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_ASSET_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_CD_ROOT="$WROOT/re15_port/shared_assets/PSX"
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1
# Titel-Vorlauf: ohne RE15_TITLE_SHOT bleibt das Spiel im Titelbild stehen (gemessen:
# 70 s Lauf, 0 Bilder, Log endet nach "[pad]").
export RE15_TITLE_SHOT="${MARKE}_titel.ppm" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1150@gp"
export RE15_FORCE_CUT="$CUT"
export RE15_PRI_LOG="$(cygpath -m "$LAUF")/${MARKE}_pri.log"
export RE15_FRAMEDUMP="${SERIE:-300-900/100}:${MARKE}_"
export RIG_DIARY="${RIG_DIARY:--23813,-1533,-18296,2990}"
export RIG_KARTE="${RIG_KARTE:--23657,-1520,-18649,3072}"
export RIG_DIARY_MD1="$WROOT/extracted_re2_dokumente/weltmodelle/mesh03_cf9f316d.md1"
export RIG_DIARY_TIM="$WROOT/extracted_re2_dokumente/weltmodelle/mesh03_cf9f316d.tim"
export RIG_KARTE_MD1="$(cygpath -m "$AUS")/rig_karte.md1"
export RIG_KARTE_TIM="$(cygpath -m "$AUS")/rig_karte.tim"
export RIG_OTMAX="$OTMAX"
rm -f "${MARKE}_pri.log"
timeout -k 5 "${SEK:-90}" "$EXE"
rc=$?
cp -f debug.log "${MARKE}.log" 2>/dev/null || true
echo "[rig] $MARKE cut=$CUT otmax=$OTMAX rc=$rc Bilder: $(ls ${MARKE}_*.ppm 2>/dev/null | wc -l)"
