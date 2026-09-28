#!/usr/bin/env bash
# Runde 30, Thema F (Karten-Marken): ein Messlauf des PC-Ports im EIGENEN Build-
# Verzeichnis re15_port/build_r30_karten-marken (re15_port/build wird NICHT beruehrt).
#
#   $1 = Marke (Dateipraefix der Ausgaben unter build/r30_karten-marken/)
#   uebrige Schalter kommen als Umgebungsvariablen vom Aufrufer (RE15_*).
#
# ⛔ MSYS_NO_PATHCONV & Co. sind noetig: Git-Bash deutet "60#1150" und Pfadlisten um
# (s. analysis/befunde_2026-09-27/elza_run.sh).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
MARKE="${1:?Marke fehlt}"
SEK="${SEK:-240}"
AUS="$ROOT/build/r30_karten-marken"
mkdir -p "$AUS"
cd "$ROOT/re15_port/build_r30_karten-marken/platform/pc"
rm -f debug.log befund.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1
timeout -k 5 "$SEK" ./re15_pc.exe > "$AUS/${MARKE}.stdout.log" 2> "$AUS/${MARKE}.stderr.log"
rc=$?
cp -f debug.log  "$AUS/${MARKE}.debug.log"  2>/dev/null || true
cp -f befund.log "$AUS/${MARKE}.befund.log" 2>/dev/null || true
echo "[run] $MARKE rc=$rc"
