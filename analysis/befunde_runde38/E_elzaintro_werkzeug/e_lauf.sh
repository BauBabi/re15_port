#!/usr/bin/env bash
# Runde 38 Spur E (elzaintro): EIN Messlauf der gebauten exe dieses Baums, Neues Spiel ueber Titel +
# Charakterwahl (Leon oder Elza), Log + Ton-Mitschnitt + optionale Bildserie.
#
#   e_lauf.sh <ausgabeordner> <leon|elza> <serie|-> <exit_at> [weitere KEY=WERT ...]
#
#   serie    RE15_FRAMEDUMP-Serie "<start>-<ende>/<schritt>" oder "-" fuer keine Bilder
#   exit_at  RE15_EXIT_AT ("<bild>#<raum-hex>"), beendet am Ende dieses Bilds
#
# Bilder: frame_count springt bei JEDEM Raumwechsel auf 0 (main.c "Frame-Cap des Handoffs relativ zum
# Eintritt") -> der Waechter (Runde 30, r30_elza_watch.py) zieht jede fertige Datei auf k<nr>_f<bild>.ppm um.
# Messkopie der exe unter eigenem Namen neben der exe (Asset-Wurzel = exe-Verzeichnis), nie /IM beenden.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
OUT="${1:?Ausgabeordner fehlt}"; WER="${2:-elza}"; SERIE="${3:--}"; EXITAT="${4:?exit_at fehlt}"; shift 4
EXE_SRC="$ROOT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$ROOT/re15_port/build/platform/pc/re15_pc_r38e_$$.exe"
cp -f "$EXE_SRC" "$EXE" || exit 2
mkdir -p "$OUT"; cd "$OUT" || exit 2
rm -f debug.log cap.raw f_*.ppm k*_f*.ppm k*_f*.png
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
export RE15_EVT_TRACE=1 RE15_MSG_LOG=1 RE15_FADE_LOG=1 RE15_AUDIO_CAP_SYNC=cap.raw
export RE15_EXIT_AT="$EXITAT"
if [ "$WER" = "elza" ]; then export RE15_PSELECT_AUTO_SWITCH=1; else unset RE15_PSELECT_AUTO_SWITCH; fi
if [ "$SERIE" != "-" ]; then export RE15_FRAMEDUMP="$SERIE:f_"; else unset RE15_FRAMEDUMP; fi
for kv in "$@"; do export "$kv"; done
WPID=""
if [ "$SERIE" != "-" ]; then
  python "$(cygpath -m "$ROOT/analysis/befunde_runde38/E_elzaintro_werkzeug/e_waechter.py")" . 900 > watch.log 2>&1 &
  WPID=$!
fi
timeout -k 5 900 "$EXE"; rc=$?
if [ -n "$WPID" ]; then sleep 1; kill "$WPID" 2>/dev/null; wait "$WPID" 2>/dev/null; fi
rm -f "$EXE"
echo "[e_lauf] $OUT ($WER) rc=$rc Log=$(wc -l < debug.log 2>/dev/null) Bilder=$(ls k*_f*.ppm 2>/dev/null | wc -l) cap=$(stat -c %s cap.raw 2>/dev/null)"
