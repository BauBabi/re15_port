#!/bin/bash
# bau_lauf.sh - Runde 34 Nacht, Spur E, BAU-Stufe: Abnahme-Lauf an der ECHTEN exe (mit Port-Code).
# Weg wie raum_lauf.sh (Ermittlung): Spielstart ueber CONTINUE (Speicherkarte des Nutzers Platz 0,
# ROOM1150), dann Debug-Sprung RE15_DEBUG_JUMP "<raum>@gp" (derselbe Weg wie ein Quadrat-Druck im
# Debug-Menue) mit Standplatz RE15_PLAYER_POS, Tasten ueber RE15_PAD_AT, Bilder ueber
# RE15_FRAMEDUMP (Readback vor SDL_RenderPresent, beschleunigter Renderer; kein AUTOSHOT, kein
# SOFTWARE_RENDER). Eigene exe-Kopie re15_pc_e.exe in build/r34n_e/mess_bau (OHNE eigenen
# shared_assets-Baum: die exe nimmt den Baum des Arbeitsbaums, re15_port/shared_assets).
# Aufruf: bash re15_port/tools/r34n_e/bau_lauf.sh <raum-hex> <x,z,rot> <name> [dump] [exit_at] [pad_at]
#   Weitere Schalter per Umgebung durchreichen (z.B. RE15_SET_FLAG=...).
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
RAUM="$1"; POS="$2"; NAME="$3"; DUMP="${4:-150-450/50}"; EXIT_AT="${5:-460}"; PAD="${6:-}"
MESS="$BAUM/build/r34n_e/mess_bau"
OUT="$BAUM/build/r34n_e/bau/$NAME"
mkdir -p "$MESS"
rm -rf "$OUT"; mkdir -p "$OUT/bild"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$MESS/re15_pc_e.exe"
cp -f "$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr" "$MESS/re15_card.mcr"
rm -f "$MESS/debug.log"
cd "$MESS"
SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=3 \
RE15_DEBUG_JUMP="${RAUM}@gp" RE15_PLAYER_POS="$POS" RE15_PAD_AT="$PAD" RE15_DOC_LOG=1 \
RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_EXIT_AT="$EXIT_AT" \
timeout -k 5 300 ./re15_pc_e.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  RAUM=$RAUM POS=$POS DUMP=$DUMP PAD=$PAD" > "$OUT/exit.txt"
cp debug.log "$OUT/debug.log" 2>/dev/null
# PPM -> PNG (zum Ansehen), 960x720 -> 320x240 verkleinert als *_k.png
C:/Python310/python.exe - "$OUT/bild" <<'PY'
import os, sys
from PIL import Image
d = sys.argv[1]
for n in sorted(os.listdir(d)):
    if n.endswith('.ppm'):
        im = Image.open(os.path.join(d, n)).convert('RGB')
        im.save(os.path.join(d, n[:-4] + '.png'))
        im.resize((320, 240), Image.NEAREST).save(os.path.join(d, n[:-4] + '_k.png'))
PY
cat "$OUT/exit.txt"; ls "$OUT/bild" | grep -c ppm
grep -a "AUTO-JUMP\|JUMP ->\|RE15_PLAYER_POS\|\[dokumente\]\|r30-doc\] F.*st=3\|EXIT_AT" "$OUT/debug.log" "$OUT/stderr.txt" 2>/dev/null | head -12
