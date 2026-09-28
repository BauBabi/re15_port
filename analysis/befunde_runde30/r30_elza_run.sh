#!/usr/bin/env bash
# Runde 30, Thema G (elza-intro): EIN Messlauf des PC-Ports aus dem EIGENEN Bauverzeichnis
# re15_port/build_r30_elza-intro. Vorlage: analysis/befunde_2026-09-27/elza_run.sh.
#
#   r30_elza_run.sh <marke> <leon|elza> [skript] [serie] [sekunden]
#
#   marke    Unterordner unter build/r30_elza-intro/ (Log + Bilder)
#   skript   RE15_INPUT_SCRIPT (Default "W2,S1,W900" = Titel bestaetigen, dann nichts)
#   serie    RE15_FRAMEDUMP-Serie "<start>-<ende>/<schritt>" oder "-" fuer keine Bilder
#   sekunden Laufzeit-Deckel (timeout), Default 200
#
# Zusaetzliche Umgebungsvariablen werden durchgereicht (z.B. RE15_SCD_TRACE=1).
#
# ⛔ MSYS*_CONV_EXCL: Git-Bash haelt "200-2600/200:pfad" sonst fuer eine PATH-Liste und
# verstuemmelt sie (elza_run.sh, gemessen 1221 statt 13 Bilder).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
MARKE="${1:?Marke fehlt}"
WER="${2:-elza}"
SKRIPT="${3:-W2,S1,W900}"
SERIE="${4:--}"
SEK="${5:-200}"
EXE="$ROOT/re15_port/build_r30_elza-intro/platform/pc/re15_pc.exe"
OUT="$ROOT/build/r30_elza-intro/$MARKE"
mkdir -p "$OUT"
cd "$OUT" || exit 2
rm -f debug.log f_*.ppm k*_f*.ppm k*_f*.png
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_FADE_LOG=1
export RE15_EVT_TRACE=1 RE15_FLAG_TRACE=1
export RE15_INPUT_SCRIPT="$SKRIPT" RE15_INPUT_SCRIPT_START=30
if [ "$SERIE" != "-" ]; then export RE15_FRAMEDUMP="$SERIE:f_"; else unset RE15_FRAMEDUMP; fi
if [ "$WER" = "elza" ]; then export RE15_PSELECT_AUTO_SWITCH=1; else unset RE15_PSELECT_AUTO_SWITCH; fi
WPID=""
if [ "$SERIE" != "-" ]; then
  # Bild-Waechter: frame_count springt bei jedem Raumwechsel auf 0 (r30_elza_watch.py)
  # ⛔ cygpath: mit MSYS_NO_PATHCONV=1 bekaeme das native Python "/c/..." unuebersetzt
  # (gemessen: "can't open file C:\c\workspace...", der erste Lauf m2 lief OHNE Waechter).
  python "$(cygpath -m "$ROOT/analysis/befunde_runde30/r30_elza_watch.py")" . "$((SEK + 3))" > watch.log 2>&1 &
  WPID=$!
fi
timeout -k 5 "$SEK" "$EXE"
rc=$?
if [ -n "$WPID" ]; then sleep 1; kill "$WPID" 2>/dev/null; wait "$WPID" 2>/dev/null; fi
echo "[run] $MARKE ($WER) rc=$rc  Log: $(wc -l < debug.log 2>/dev/null) Zeilen  Bilder: $(ls k*_f*.ppm f_*.ppm 2>/dev/null | wc -l)"
