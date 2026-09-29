#!/usr/bin/env bash
# r33_karte_abnahme.sh - ABNAHME Runde 33 / Thema K an der ECHTEN exe (Kopie unter eigenem
# Namen, damit parallele Agenten nicht getroffen werden): Karte nach dem Irons-Hinweis.
# Bilder per RE15_FRAMEDUMP (komplett komponierter Frame VOR dem Present, beschleunigter
# Renderer) - KEIN RE15_AUTOSHOT, KEIN RE15_SOFTWARE_RENDER. Muster: Runde 30
# analysis/befunde_runde30/tools/r30_karte3010_abnahme.sh.
#   $1 = Marke (Dateien landen in build/r33_karte_abnahme/<Marke>*)
#   env: EXE (Name der Kopie, Standard re15_pc_r33k.exe), KARTE (Speicherkarte, wird in
#        mess/ kopiert und dort benutzt), SEK, PADAT (RE15_PAD_AT), SERIE ("a-b/step"),
#        SLOTS (RE15_CARD_SLOT "Laden,Speichern"), SAVE_AT, JUMP (Raum fuer RE15_DEBUG_JUMP
#        oder leer), POS (RE15_PLAYER_POS), INPUT/INSTART (RE15_INPUT_SCRIPT, Spielbilder)
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
SEK="${SEK:-120}"
EXE="${EXE:-re15_pc_r33k.exe}"
BIN="$ROOT/re15_port/build/platform/pc"
OUT="$ROOT/build/r33_karte_abnahme"
mkdir -p "$OUT"
[ -n "${KARTE:-}" ] && KARTE="$(cd "$(dirname "$KARTE")" && pwd)/$(basename "$KARTE")"
cd "$BIN" || exit 2
[ -f "$EXE" ] || { echo "exe-Kopie $EXE fehlt"; exit 2; }
mkdir -p mess
# Die exe liest re15_card.mcr aus ihrem Verzeichnis; eine KARTE aus der Umgebung wird
# vorher dorthin kopiert (Hauptbaum und andere Baeume nur gelesen).
if [ -n "${KARTE:-}" ]; then cp -f "$KARTE" re15_card.mcr; fi
rm -f debug.log befund.log
rm -f mess/${MARKE}_*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export SDL_AUDIODRIVER=dummy RE15_SE_DEBUG=1
export RE15_CARD_AUTO=1 RE15_CONTINUE_TEST=1 RE15_WINDOW_SCALE=3
export RE15_CARD_SLOT="${SLOTS:-0,0}"
if [ -n "${JUMP:-}" ]; then
    export RE15_DEBUG_JUMP="${JUMP}@gp"
    if [ -n "${POS:-}" ]; then export RE15_PLAYER_POS="$POS"; else unset RE15_PLAYER_POS; fi
else
    unset RE15_DEBUG_JUMP RE15_PLAYER_POS
fi
unset RE15_FORCE_EVENT RE15_AUTOSHOT RE15_SOFTWARE_RENDER RE15_NOAUDIO
# INPUT = RE15_INPUT_SCRIPT auf der Zeitachse der Spielbilder (Buchstaben S = Start, Q = L1,
# U/D = hoch/runter; Sekunden x 30 Bilder), Beginn INSTART (Standard 1).
if [ -n "${INPUT:-}" ]; then export RE15_INPUT_SCRIPT="$INPUT" RE15_INPUT_SCRIPT_START="${INSTART:-1}" RE15_INPUT_SCRIPT_BASIS=spiel; else unset RE15_INPUT_SCRIPT; fi
export RE15_FLAG_TRACE=1
if [ -n "${PADAT:-}" ]; then export RE15_PAD_AT="$PADAT"; else unset RE15_PAD_AT; fi
if [ -n "${SERIE:-}" ]; then export RE15_FRAMEDUMP="${SERIE}:mess/${MARKE}_"; else unset RE15_FRAMEDUMP; fi
if [ -n "${SAVE_AT:-}" ]; then export RE15_SAVE_TEST_AGAIN="$SAVE_AT" RE15_SAVE_TEST_EXIT_AFTER=1; else unset RE15_SAVE_TEST_AGAIN RE15_SAVE_TEST_EXIT_AFTER; fi
T0=$(date +%s.%N)
timeout -k 5 "$SEK" "./$EXE" > /dev/null 2>&1
rc=$?
T1=$(date +%s.%N)
cp -f debug.log "$OUT/${MARKE}.debug.log" 2>/dev/null || true
cp -f re15_card.mcr "$OUT/${MARKE}_karte_danach.mcr" 2>/dev/null || true
for f in mess/${MARKE}_*.ppm; do [ -f "$f" ] && mv -f "$f" "$OUT/"; done
echo "[abnahme] $MARKE rc=$rc  Wanduhr $(python -c "print(round($T1-$T0,2))") s  Bilder: $(ls "$OUT"/${MARKE}_*.ppm 2>/dev/null | wc -l)"
