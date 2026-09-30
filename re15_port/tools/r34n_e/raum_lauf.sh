#!/bin/bash
# raum_lauf.sh - Runde 34 Nacht, Spur E: NULLBILD eines Raums in der ECHTEN exe (vor jedem Port-Code).
# Zweck: wie hell rendert der echte Renderer die ORIGINAL-Gegenstaende dort, wo die Dokumente liegen
# sollen (Lichtsatz des Raums, E_dokumente.md 2.8), und wie sieht der Cut des Nutzerbilds im Spiel aus.
# Weg: Debug-Sprung (RE15_DEBUG_JUMP "<raum>@gp" = derselbe Weg wie ein Quadrat-Druck im Debug-Menue,
# main.c) + Standplatz RE15_PLAYER_POS + RE15_FRAMEDUMP (Readback vor SDL_RenderPresent, beschleunigter
# Renderer; kein AUTOSHOT, kein SOFTWARE_RENDER). Laeuft im Mess-Verzeichnis build/r34n_e/mess
# (eigene exe-Kopie re15_pc_mess.exe, s. leser_lauf.sh).
# Aufruf: bash re15_port/tools/r34n_e/raum_lauf.sh <raum-hex> <x,z,rot> <name> [dump] [exit_at]
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
RAUM="$1"; POS="$2"; NAME="$3"; DUMP="${4:-200-600/40}"; EXIT_AT="${5:-610}"
MESS="$BAUM/build/r34n_e/mess"
OUT="$BAUM/build/r34n_e/raum/$NAME"
rm -rf "$OUT"; mkdir -p "$OUT/bild"
# Spielstart ueber CONTINUE (Speicherkarte des Nutzers Platz 0, ROOM1150, wie leser_lauf.sh); ohne
# Spielstand bleibt die exe im Titel stehen (erster Versuch: Zeitablauf nach 300 s, 0 Bilder).
cp "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$MESS/re15_card.mcr"
cd "$MESS"
SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=3 \
RE15_DEBUG_JUMP="${RAUM}@gp" RE15_PLAYER_POS="$POS" \
RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_EXIT_AT="$EXIT_AT" \
timeout -k 5 300 ./re15_pc_mess.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  RAUM=$RAUM POS=$POS DUMP=$DUMP" > "$OUT/exit.txt"
cp debug.log "$OUT/debug.log" 2>/dev/null
cat "$OUT/exit.txt"; ls "$OUT/bild" | tr '\n' ' '; echo
grep -a "AUTO-JUMP\|\[light\] cut\|\[pri\] cut\|player spawn\|PLAYER_POS" "$OUT/debug.log" | head -12
