#!/usr/bin/env bash
# r30_karte3010_abnahme.sh - ABNAHME des Kartenhinweises (Runde 30, Thema B, Bau S10)
# an der ECHTEN exe im echten Ablauf: Spielstand-Kopie laden (Titel -> LOAD GAME),
# Debug-JUMP ROOM1150 an eine Stelle IN der AUTO-Zone Slot 6 (ROOM1150 main00 @0x00DEA,
# rect x -27300..-15800, z -23800..-21800) -> die Szene sub08 laeuft von selbst (KEIN
# RE15_FORCE_EVENT) -> Hinweis. Eingaben per RE15_INPUT_SCRIPT, Bilder per RE15_FRAMEDUMP
# (komplett komponierter Frame VOR dem Present) - KEIN RE15_AUTOSHOT, KEIN
# RE15_SOFTWARE_RENDER. Ton: SDL-Dummy-Treiber (still), RE15_SE_DEBUG=1 protokolliert jeden
# gespielten SE samt Pitch.
#   $1 = Marke (Dateien landen in build/r30_karte-3010_bau/<Marke>*)
#   env: SEK (Laufzeit), INPUT (RE15_INPUT_SCRIPT), SERIE ("a-b/step" fuer RE15_FRAMEDUMP),
#        SLOTS (RE15_CARD_SLOT, "Laden,Speichern"), SAVE_AT (Bild fuer einen Speicherpunkt),
#        JUMP (1 = Debug-JUMP 1150 mit Lage in der Zone, 0 = nur laden), FRISCH (1 = Karte neu
#        aus dem Hauptbaum kopieren)
# Laeuft im Bauverzeichnis des Arbeitsbaums (re15_port/build/platform/pc) mit einer KOPIE
# der Speicherkarte aus dem Hauptbaum (nur gelesen).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MARKE="${1:?Marke fehlt}"
SEK="${SEK:-120}"
BIN="$ROOT/re15_port/build/platform/pc"
OUT="$ROOT/build/r30_karte-3010_bau"
KARTE_SRC="C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_card.mcr"
mkdir -p "$OUT"
cd "$BIN" || exit 2
if [ "${FRISCH:-0}" = "1" ] || [ ! -f re15_card.mcr ]; then cp -f "$KARTE_SRC" re15_card.mcr; fi
rm -f debug.log befund.log
mkdir -p mess
rm -f mess/${MARKE}_*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export SDL_AUDIODRIVER=dummy RE15_SE_DEBUG=1
export RE15_CARD_AUTO=1
if [ "${LADEN:-1}" = "1" ]; then export RE15_CONTINUE_TEST=1; else unset RE15_CONTINUE_TEST; fi
export RE15_CARD_SLOT="${SLOTS:-0,0}"
if [ "${JUMP:-1}" = "1" ]; then
    export RE15_DEBUG_JUMP="1150@gp"
    export RE15_PLAYER_POS="${POS:--20500,-22800,1024}"
else
    unset RE15_DEBUG_JUMP RE15_PLAYER_POS
fi
unset RE15_FORCE_EVENT RE15_AUTOSHOT RE15_SOFTWARE_RENDER RE15_NOAUDIO
export RE15_FLAG_TRACE=1
[ -n "${PADDBG:-}" ] && export RE15_PAD_DEBUG=1
if [ -n "${INPUT:-}" ]; then export RE15_INPUT_SCRIPT="$INPUT" RE15_INPUT_SCRIPT_START="${INSTART:-1}" RE15_INPUT_SCRIPT_BASIS=spiel; else unset RE15_INPUT_SCRIPT; fi
if [ -n "${SERIE:-}" ]; then export RE15_FRAMEDUMP="${SERIE}:mess/${MARKE}_"; else unset RE15_FRAMEDUMP; fi
if [ -n "${SAVE_AT:-}" ]; then export RE15_SAVE_TEST_AGAIN="$SAVE_AT" RE15_SAVE_TEST_EXIT_AFTER=1; else unset RE15_SAVE_TEST_AGAIN RE15_SAVE_TEST_EXIT_AFTER; fi
T0=$(date +%s.%N)
timeout -k 5 "$SEK" ./re15_pc.exe > /dev/null 2>&1
rc=$?
T1=$(date +%s.%N)
cp -f debug.log "$OUT/${MARKE}.debug.log" 2>/dev/null || true
for f in mess/${MARKE}_*.ppm; do [ -f "$f" ] && mv -f "$f" "$OUT/"; done
echo "[abnahme] $MARKE rc=$rc  Wanduhr $(python -c "print(round($T1-$T0,2))") s  Bilder: $(ls "$OUT"/${MARKE}_*.ppm 2>/dev/null | wc -l)"
