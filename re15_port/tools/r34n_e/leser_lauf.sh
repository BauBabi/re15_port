#!/bin/bash
# leser_lauf.sh - Runde 34 Nacht, Spur E: die gesetzten Seiten FILE26..29 im LESER DER ECHTEN EXE
# (beschleunigter Renderer, RE15_FRAMEDUMP = Readback vor SDL_RenderPresent; kein AUTOSHOT, kein
# SOFTWARE_RENDER) - OHNE Port-Code: die Dokument-Tabelle kennt FILE26..29 noch nicht.
#
# Kniff: ein Mess-Verzeichnis build/r34n_e/mess/ mit eigener Kopie der exe und eigenem
# shared_assets/ (PSX, extracted_fx, RE15DOOR als Verzeichnis-Verbindung auf den Baum, RE2 als Kopie).
# Darin liegen die neuen Seiten UNTER DEM NAMEN eines RE2-Dokuments mit GLEICHEM max_page und
# GLEICHER Seitenhoehe (u16 @0x800AA144 + doc*4, gen/re2_files_toc.inc):
#   FILE26 (3 Seiten, H 144) als FILE14 "Patrol report"      (max_page 3, H 144)
#   FILE27 (4 Seiten, H 144) als FILE08 "Secretary's diary B" (max_page 4, H 144)
#   FILE28 (2 Seiten, H 176) als FILE03 "Police memorandum"   (max_page 2, H 176)
#   FILE29 (2 Seiten, H 176) als FILE23 "Rookie files"        (max_page 2, H 176)
# Die Ansehhilfe RE15_DOC=<n> (main.c, VIERECK auf einer Listenzeile oeffnet Bildsatz n mit
# dessen max_page) zeigt sie dann im echten Leser. Die exe liest die Kopie, weil das exe-Verzeichnis
# (mit shared_assets/) die erste Basis-Wurzel ist (asset_root_pc.h, Kette 2).
#
# Spielstand: Speicherkarte des Nutzers Platz 0 (ROOM1150), wie r30_diary_en_lauf.sh.
# Ablauf (wie r30_diary_en_lauf.sh: Menue erst lange nach dem Lade-Einblenden, R1 120 Bilder
# nach START, VIERECK 60/80 Bilder danach):
#   F400 START (Menue), F520 R1 (FILE), F580/F600 VIERECK (Zeile 0 waehlen / oeffnen),
#   F700+90k RECHTS; Framedumps F690..F1140 alle 90 (Titel, Seite 1..4, Ende-Stellung).
# Erster Versuch (START F100, R1 F220) landete auf der KARTE: R1 kam nicht an, VIERECK F280
# bestaetigte den Vorgabe-Reiter. Ueberschreibbar: LESER_PAD, LESER_DUMP, LESER_EXIT.
# Aufruf: bash re15_port/tools/r34n_e/leser_lauf.sh <bildsatz> <name>
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
N="$1"; NAME="$2"
MESS="$BAUM/build/r34n_e/mess"
OUT="$BAUM/build/r34n_e/leser/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT/bild"
cp "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$MESS/re15_card.mcr"
PAD="${LESER_PAD:-400:S,520:M,580:A,600:A,700:R,790:R,880:R,970:R,1060:R}"
DUMP="${LESER_DUMP:-690-1140/90}"
EXIT_AT="${LESER_EXIT:-1150}"
cd "$MESS"
SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=3 \
RE15_DOC="$N" RE15_DOC_LOG=1 RE15_PAD_AT="$PAD" \
RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_DOC_EXIT_AT="$EXIT_AT" \
timeout -k 5 300 ./re15_pc_mess.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  PAD=$PAD  DUMP=$DUMP" > "$OUT/exit.txt"
cp debug.log "$OUT/debug.log" 2>/dev/null
cat "$OUT/exit.txt"; ls "$OUT/bild" | tr '\n' ' '; echo
