#!/bin/bash
# r33_speichern_lauf.sh - Runde 33 Thema S: Framedump-Lauf an der Speicherstelle ROOM1150 (Telefon)
# im ECHTEN Spiel (beschleunigter Renderer; RE15_FRAMEDUMP = Readback vor SDL_RenderPresent;
# kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER).
#
# Aufruf: bash analysis/befunde_runde33/speichern_werkzeug/r33_speichern_lauf.sh <name> <exe> <pad_at> [karte=0|1] [dumps] [exit]
#   <name>    Unterordner unter build/r33_speichern/ (debug.log, stderr.txt, mess/fNNNNNN.ppm, re15_card.mcr)
#   <exe>     Lauf-exe (unter EIGENEM Namen kopiert - andere Agenten laufen parallel)
#   <pad_at>  RE15_PAD_AT, z.B. "150:A,200:A"
#   karte     1 = RE15_GIVE_CARD (Memory Card x2 ins Inventar, NACH dem Laden)
#   dumps     RE15_FRAMEDUMP-Serie "<start>-<ende>/<schritt>" (Vorgabe 100-700/10)
#   exit      RE15_EXIT_AT-Bild in ROOM1150 (Vorgabe 720)
#
# Spielstand: Karte des Nutzers (analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr),
# Platz 0 = ROOM1150 (-22689,-19693) rot 1864, Leon steht vor dem Telefon (AOT-Slot 3, Event 6),
# Inventar OHNE Memory Card. Geladen ueber RE15_CONTINUE_TEST + RE15_CARD_AUTO, RE15_CARD_SLOT=0,1
# (laden aus 0, speichern in 1). Der Prozess endet ueber RE15_EXIT_AT; timeout am EIGENEN Kind.
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
NAME="$1"; EXE="$2"; PAD="$3"; KARTE="${4:-0}"; DUMPS="${5:-100-700/10}"; EXITF="${6:-720}"
OUT="$BAUM/build/r33_speichern/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT/mess"
cp "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$OUT/re15_card.mcr"
EXTRA=""
[ "$KARTE" = "1" ] && EXTRA="RE15_GIVE_CARD=1"
cd "$OUT"
env SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,1 \
    RE15_WINDOW_SCALE=3 RE15_MSG_LOG=1 RE15_STATE_LOG=state.log RE15_PAD_AT="$PAD" $EXTRA \
    RE15_FRAMEDUMP="$DUMPS:$OUT/mess/f" RE15_EXIT_AT="$EXITF#1150" \
    timeout -k 5 240 "$EXE" > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?" > "$OUT/exit.txt"
[ -f "$OUT/debug.log" ] || cp "$(dirname "$EXE")/debug.log" "$OUT/debug.log" 2>/dev/null
cat "$OUT/exit.txt"; ls "$OUT/mess" | wc -l
