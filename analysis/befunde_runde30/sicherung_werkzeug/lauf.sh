#!/usr/bin/env bash
# Runde 30 / Thema H — Sichtlauf "Sicherung im Hebetisch", ROOM1150.
#
#   $1 = Marke (Unterordner von build/r30_sicherung/)
#   $2 = Ausloeser: "taste" (Spieler steht im AOT-Slot-1-Rechteck und drueckt Aktion)
#                   "aot"   (RE15_FIRE_AOT — derselbe re15_aot_fire_slot-Weg)
#                   "sub"   (RE15_DEBUG_SUB 4 — sub04 direkt, ohne AOT)
#   $3 = optional "genommen": setzt flag(9,53) VOR dem Raumeintritt = Gegenprobe OHNE Prop
#
# KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER (Skill re15-port-visual-verify).
# Bilder kommen aus RE15_FRAMEDUMP = Readback unmittelbar VOR SDL_RenderPresent
# (main.c:10002-10030), also der komplett komponierte Frame inkl. 3D-Props.
#
# MSYS_NO_PATHCONV & Co. sind Pflicht: Git-Bash zerlegt sonst "a-b/c:pfad" als PATH-Liste
# (analysis/befunde_2026-09-27/elza_run.sh, gemessen 1221 statt 13 Dateien).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd -W 2>/dev/null || pwd)"
MARKE="${1:?Marke fehlt}"
WIE="${2:-taste}"
GEGEN="${3:-}"
SEK="${SEK:-120}"
SERIE="${SERIE:-0-400/2}"
JUMP_AB="${JUMP_AB:-240}"
AUSL_AB="${AUSL_AB:-90}"
# VARIANTE=1 nimmt die Mess-Variante (probes/r30_sicherung.cmake) statt des Spiels.
if [ -n "${VARIANTE:-}" ]; then
  EXE="$ROOT/re15_port/${BAUVERZ:-build_r30_sicherung}/tests/unit/re15_pc_r30_sicherung.exe"
else
  EXE="$ROOT/re15_port/${BAUVERZ:-build_r30_sicherung}/platform/pc/re15_pc.exe"
fi
ZIEL="$ROOT/build/r30_sicherung/$MARKE"
mkdir -p "$ZIEL"
cd "$ZIEL" || exit 2
rm -f debug.log debug_sub.log f_*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1150@${JUMP_AB}"
# Westseite des Mitteltisches = im Rechteck des autorisierten Records slot 1 @0x0D7E
# (x -21800..-20300, z -20000..-16700); Blick nach Osten auf den Tisch.
export RE15_PLAYER_POS="${PLAYER_POS:--21000,-18500,0}"
export RE15_FRAMEDUMP="${SERIE}:f_"
case "$WIE" in
  taste) export RE15_INPUT_SCRIPT="${SKRIPT:-W3,A0.2,W20}" RE15_INPUT_SCRIPT_START="${SKRIPT_AB:-400}" ;;
  aot)   export RE15_FIRE_AOT="1@${AUSL_AB}#1150" ;;
  sub)   export RE15_DEBUG_SUB="4@${AUSL_AB}" ;;
esac
if [ "$GEGEN" = "genommen" ]; then export RE15_SET_FLAG="9:53"; fi
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "$SEK" "$EXE"
rc=$?
echo "[lauf] $MARKE ($WIE ${GEGEN}) rc=$rc  Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)  Log: $(wc -l < debug.log 2>/dev/null) Zeilen"
