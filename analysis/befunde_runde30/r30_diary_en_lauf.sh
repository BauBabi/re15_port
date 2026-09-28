#!/bin/bash
# r30_diary_en_lauf.sh - Nachtrag J (Irons Diary englisch): Framedump-Lauf des Dokument-Lesers
# im ECHTEN Spiel (beschleunigter Renderer, RE15_FRAMEDUMP = Readback vor SDL_RenderPresent;
# kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER).
#
# Aufruf: bash analysis/befunde_runde30/r30_diary_en_lauf.sh <lauf-name> [exe]
#   <lauf-name>  Unterordner unter build/r30_n_diary_en/ (debug.log, stderr.txt, mess/fNNNNNN.ppm)
#   [exe]        Vorgabe: re15_port/build/platform/pc/re15_pc.exe dieses Baums
#
# Spielstand: Speicherkarte des Nutzers (nutzer_marken/re15_card_nutzer_2026-09-27.mcr),
# Platz 0, geladen ueber RE15_CONTINUE_TEST + RE15_CARD_AUTO (wie r30_pfeil_lauf.sh).
# Ablauf (bildgenau, RE15_PAD_AT; Seitenzahl N = max_page aus dem Dateibestand FILE25_pNN):
#   F260          re15_menu_request_doc(0) = Aufnahme-Leser fuer das Irons Diary
#   F320+90k      RECHTS, k = 0..N -> Seiten p01..pN, dann die Ende-Stellung
#   F1800         KREUZ = schliessen -> Meldung "has been filed"
#   F1890         VIERECK = Meldung weg, Menue zu
#   F1940 S, F2060 M       Menue, FILE-Reiter (Liste mit "Irons Diary" auf Zeile 0)
#   F2120 A, F2140 A       Zeilenwahl, Zeile 0 oeffnen (Leser des FILE-Schirms)
#   Framedumps F310..F2200 alle 90 Bilder: F310 = Titel, F310+90k = pk (k = 1..N),
#     F310+90(N+1) = Ende-Stellung, F1840 Meldung, F2110 Liste, F2200 Leser aus der Liste.
# Der Prozess endet ueber RE15_DOC_EXIT_AT (Bild), dazu timeout am EIGENEN Kind (nie taskkill /IM).
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../.." && pwd -W)"
NAME="$1"
EXE="${2:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
OUT="$BAUM/build/r30_n_diary_en/$NAME"
N=1
while [ -f "$BAUM/re15_port/shared_assets/RE2/FILES/FILE25_p$(printf %02d $((N + 1)))_page.TIM" ]; do
    N=$((N + 1))
done
if [ $((320 + 90 * N)) -ge 1800 ]; then echo "zu viele Seiten fuer den Zeitplan: $N"; exit 2; fi
rm -rf "$OUT"
mkdir -p "$OUT/mess"
cp "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$OUT/re15_card.mcr"
PAD=""
for k in $(seq 0 "$N"); do PAD="$PAD$((320 + 90 * k)):R,"; done
PAD="${PAD}1800:X,1890:A,1940:S,2060:M,2120:A,2140:A"
cd "$OUT"
SDL_AUDIODRIVER=dummy RE15_SE_DEBUG=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 \
RE15_DOC_REQUEST=260 RE15_DOC_LOG=1 RE15_PAD_AT="$PAD" \
RE15_FRAMEDUMP="310-2200/90:$OUT/mess/f" RE15_DOC_EXIT_AT=2210 \
timeout -k 5 300 "$EXE" > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  Seiten N=$N" > "$OUT/exit.txt"
[ -f "$OUT/debug.log" ] || cp "$(dirname "$EXE")/debug.log" "$OUT/debug.log" 2>/dev/null
echo "PAD=$PAD" >> "$OUT/exit.txt"
cat "$OUT/exit.txt"
ls "$OUT/mess" | wc -l
