#!/bin/bash
# lade_lauf.sh - Runde 34 Nacht, Spur E, BAU-Stufe: Abnahme am LADE-Weg der ECHTEN exe (CONTINUE direkt
# in einen Dokument-Raum, eigene Karte von probe_r34n_e_karte) — der Weg, der nicht durch
# scd_room_reenter geht (Haken in platform/pc/main.c). Tasten per RE15_PAD_AT, Bilder per RE15_FRAMEDUMP.
# Aufruf: bash re15_port/tools/r34n_e/lade_lauf.sh <raum-hex> <x,z,rot> <cut> <name> [pad] [dump] [exit]
#         [karten-zusatz ...]   (z.B. genommen, files=1,2)
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
RAUM="$1"; POS="$2"; CUT="$3"; NAME="$4"; PAD="${5:-}"; DUMP="${6:-150-150/1}"; EXIT_AT="${7:-160}"
shift 7 2>/dev/null || shift $#
MESS="$BAUM/build/r34n_e/mess_bau"
OUT="$BAUM/build/r34n_e/bau/$NAME"
mkdir -p "$MESS"; rm -rf "$OUT"; mkdir -p "$OUT/bild"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$MESS/re15_pc_e.exe"
rm -f "$MESS/re15_card.mcr" "$MESS/debug.log"
( cd "$MESS" && "$BAUM/re15_port/build/tests/unit/probe_r34n_e_karte.exe" re15_card.mcr "$RAUM" \
    "pos=$POS" "cut=$CUT" "$@" ) || exit 1
cd "$MESS"
SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
RE15_WINDOW_SCALE=3 RE15_DOC_LOG=1 RE15_PAD_AT="$PAD" \
RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_EXIT_AT="$EXIT_AT" \
timeout -k 5 300 ./re15_pc_e.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  RAUM=$RAUM POS=$POS CUT=$CUT PAD=$PAD DUMP=$DUMP" > "$OUT/exit.txt"
cp debug.log "$OUT/debug.log" 2>/dev/null
C:/Python310/python.exe - "$OUT/bild" <<'PY'
import os, sys
from PIL import Image
d = sys.argv[1]
fs = []
for n in sorted(os.listdir(d)):
    if n.endswith('.ppm'):
        im = Image.open(os.path.join(d, n)).convert('RGB')
        im.save(os.path.join(d, n[:-4] + '.png'))
        fs.append(im.resize((320, 240), Image.NEAREST))
if fs:
    W = 4; H = (len(fs) + W - 1) // W
    b = Image.new('RGB', (320 * W, 240 * H))
    for i, im in enumerate(fs):
        b.paste(im, ((i % W) * 320, (i // W) * 240))
    b.save(os.path.join(os.path.dirname(d), 'bogen.png'))
print(len(fs), 'Bilder')
PY
cat "$OUT/exit.txt"
grep -a "CONTINUE: resumed\|\[dokumente\]" "$OUT/debug.log" | head -3
