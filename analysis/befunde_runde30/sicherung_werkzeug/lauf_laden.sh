#!/usr/bin/env bash
# Runde 30 / Thema H — LADE-WEG: kommt die Sicherung auch an, wenn ROOM1150 der
# STARTRAUM eines geladenen Spielstands ist (Boot-Pfad statt Tuer-Pfad)?
#
# Schritt 1: neues Spiel, Sprung nach ROOM1150, dort speichern (Karte im Arbeitsordner).
# Schritt 2: frischer Prozess, LOAD GAME, Hebetisch ausloesen, Bilder + Log.
#
#   $1 = Marke (Unterordner von build/r30_sicherung/)
#   $2 = optional "genommen": Schritt 1 setzt flag(9,53) VOR dem Raumeintritt, der
#        Spielstand traegt die Sicherung also als GENOMMEN = Gegenprobe OHNE Prop
# BAU (Runde 30): Schritt 2 endet ueber RE15_EXIT_AT am Bild (Standard 335 in ROOM1150),
# nicht an der Wanduhr; EXIT_AB="" schaltet das ab.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd -W 2>/dev/null || pwd)"
MARKE="${1:?Marke fehlt}"
GEGEN="${2:-}"
EXE="$ROOT/re15_port/${BAUVERZ:-build_r30_sicherung}/platform/pc/re15_pc.exe"
ZIEL="$ROOT/build/r30_sicherung/$MARKE"
mkdir -p "$ZIEL"
cd "$ZIEL" || exit 2
rm -f debug.log f_*.ppm re15_card.mcr schritt1.log schritt2.log
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1

(
  export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
  export RE15_DEBUG_JUMP="1150@100"
  export RE15_PLAYER_POS="-22250,-18500,0"
  export RE15_SAVE_TEST_AGAIN=150 RE15_CARD_AUTO=1 RE15_SAVE_TEST_EXIT_AFTER=1
  if [ "$GEGEN" = "genommen" ]; then export RE15_SET_FLAG="9:53"; fi
  timeout -k 5 60 "$EXE"
  echo "[laden] Schritt 1 rc=$?"
)
cp -f debug.log schritt1.log 2>/dev/null
ls -la re15_card.mcr 2>/dev/null || echo "[laden] KEINE Karte geschrieben"

(
  export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
  export RE15_FIRE_AOT="1@90#1150"
  export RE15_FRAMEDUMP="90-330/10:f_"
  EXIT_AB="${EXIT_AB-335}"
  if [ -n "$EXIT_AB" ]; then export RE15_EXIT_AT="${EXIT_AB}#1150"; fi
  timeout -k 5 "${SEK:-45}" "$EXE"
  echo "[laden] Schritt 2 rc=$?"
)
cp -f debug.log schritt2.log 2>/dev/null
echo "[laden] Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
