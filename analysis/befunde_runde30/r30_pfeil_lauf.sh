#!/bin/bash
# r30_pfeil_lauf.sh - Nachschliff Runde 30, Spur pfeil: Framedump-Lauf des Dokument-Lesers.
#
# Aufruf: bash analysis/befunde_runde30/r30_pfeil_lauf.sh <lauf-name> [exe]
#   <lauf-name>  Unterordner unter build/r30_n_pfeil/ (debug.log, mess/fNNNNNN.ppm)
#   [exe]        Vorgabe: re15_port/build/platform/pc/re15_pc.exe dieses Baums
#
# Ablauf (bildgenau, RE15_PAD_AT):
#   F260          re15_menu_request_doc(0) = Aufnahme-Leser fuer das Irons Diary
#   F320+90k      RECHTS, k = 0..17 -> Seiten p01..p17, dann die Ende-Stellung
#                 (90 Bilder je Seite: 20 Blaetterbilder + 70 Lesebilder - deckt beide
#                 Wipp-Stellungen von RE1.5 (31/30 Bilder) und RE2 (40/39 Bilder) ab)
#   F1900         KREUZ = schliessen -> Meldung "has been filed"
#   F1990         VIERECK = Meldung weg, Menue zu
#   F2040 S, F2085 M, F2125 A, F2145 A = Menue, FILE-Reiter, Zeilenwahl, Zeile 0 oeffnen
#                 (= der Leser des FILE-Schirms, RE2 Zustand 13)
#   F2230+60k     RECHTS, k = 0..3 -> vier Seiten im Listen-Leser
#   Framedumps F280..F2500 alle 4 Bilder.
# Spielstand: Speicherkarte des Nutzers (nutzer_marken/re15_card_nutzer_2026-09-27.mcr),
# Platz 0, geladen ueber RE15_CONTINUE_TEST + RE15_CARD_AUTO.
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../.." && pwd -W)"
NAME="$1"
EXE="${2:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
OUT="$BAUM/build/r30_n_pfeil/$NAME"
rm -rf "$OUT"
mkdir -p "$OUT/mess"
cp "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$OUT/re15_card.mcr"
PAD=""
for k in $(seq 0 17); do PAD="$PAD$((320 + 90 * k)):R,"; done
PAD="${PAD}1900:X,1990:A,2040:S,2085:M,2125:A,2145:A,"
for k in $(seq 0 3); do PAD="$PAD$((2230 + 60 * k)):R,"; done
PAD="${PAD%,}"
cd "$OUT"
SDL_AUDIODRIVER=dummy RE15_SE_DEBUG=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 \
RE15_DOC_REQUEST=260 RE15_DOC_LOG=1 RE15_PAD_AT="$PAD" \
RE15_FRAMEDUMP="280-2500/4:$OUT/mess/f" RE15_DOC_EXIT_AT=2510 \
"$EXE" > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?" > "$OUT/exit.txt"
[ -f "$OUT/debug.log" ] || cp "$(dirname "$EXE")/debug.log" "$OUT/debug.log" 2>/dev/null
echo "PAD=$PAD" >> "$OUT/exit.txt"
