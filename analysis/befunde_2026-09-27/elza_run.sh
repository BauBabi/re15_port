#!/usr/bin/env bash
# Ein Messlauf des PC-Ports. $1 = Marke (Dateipraefix), $2 = "elza" schaltet im
# Auswahlschirm nach rechts. Ergebnis: mess/<marke>.log + mess/<marke>_NNNNNN.ppm
# unterhalb von re15_port/build/platform/pc/.
#
# ⛔ MSYS2_ENV_CONV_EXCL / MSYS_NO_PATHCONV sind NICHT Kosmetik: Git-Bash haelt
# "200-2600/200:mess/x_" fuer eine PATH-artige Liste und reicht dem nativen exe
# "200-2600\200;mess\x_" weiter. Der Parser in main.c sucht dann mit
# strchr(dash,'/') vergeblich nach der Schrittweite und dumpt JEDES Bild
# (gemessen: 1221 Dateien / 2,4 GB statt 13 Dateien / 26 MB).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
MARKE="${1:?Marke fehlt}"
WER="${2:-leon}"
SEK="${SEK:-200}"
SERIE="${SERIE:-200-2600/200}"
cd "$ROOT/re15_port/build/platform/pc"
mkdir -p mess
rm -f debug.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_FADE_LOG=1
export RE15_INPUT_SCRIPT="W2,S1,W900" RE15_INPUT_SCRIPT_START=30
export RE15_FRAMEDUMP="$SERIE:mess/${MARKE}_"
if [ "$WER" = "elza" ]; then export RE15_PSELECT_AUTO_SWITCH=1; else unset RE15_PSELECT_AUTO_SWITCH; fi
timeout -k 5 "$SEK" ./re15_pc.exe
rc=$?
cp -f debug.log "mess/${MARKE}.log" 2>/dev/null || true
echo "[run] $MARKE ($WER) rc=$rc  Bilder: $(ls mess/${MARKE}_*.ppm 2>/dev/null | wc -l)"
