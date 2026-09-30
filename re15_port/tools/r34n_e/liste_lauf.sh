#!/bin/bash
# liste_lauf.sh - Runde 34 Nacht, Spur E, BAU-Stufe: die FILE-LISTE mit den vier neuen Dokumenten und
# der Leser AUS DER LISTE an der ECHTEN exe (Plan 6.2 Punkt 2; langer Listenname Dok 1, Risiko 7.6).
# Spielstand: eigene Karte (probe_r34n_e_karte) in ROOM1010, FILE-Liste = Dokumente 1,2,3,4 (+0 Irons
# Diary ans Ende), Genommen-Bit (9,60) gesetzt (das Blatt liegt dann nicht mehr auf dem Tisch).
# Ablauf wie leser_lauf.sh (Ermittlung): F400 START (Menue), F520 R1 (FILE), Framedump der Liste,
# DOWN je Zeile, dann VIERECK auf Zeile 0 = Dokument 1 -> Leser aus der Liste.
# Aufruf: bash re15_port/tools/r34n_e/liste_lauf.sh [pad] [dump] [exit]
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM="$(cd "$(dirname "$0")/../../.." && pwd -W)"
PAD="${1:-400:S,520:M,640:D,700:D,760:D,820:D,880:U,940:U,1000:U,1060:U,1120:A,1140:A}"
DUMP="${2:-600-1320/60}"
EXIT_AT="${3:-1330}"
MESS="$BAUM/build/r34n_e/mess_bau"
OUT="$BAUM/build/r34n_e/bau/liste"
mkdir -p "$MESS"; rm -rf "$OUT"; mkdir -p "$OUT/bild"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$MESS/re15_pc_e.exe"
rm -f "$MESS/re15_card.mcr" "$MESS/debug.log"
( cd "$MESS" && "$BAUM/re15_port/build/tests/unit/probe_r34n_e_karte.exe" re15_card.mcr 1010 genommen \
    pos=3400,7200,1024 cut=0 files=1,2,3,4,0 ) || exit 1
cd "$MESS"
SDL_AUDIODRIVER=dummy RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
RE15_WINDOW_SCALE=3 RE15_DOC_LOG=1 RE15_PAD_AT="$PAD" \
RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_EXIT_AT="$EXIT_AT" \
timeout -k 5 300 ./re15_pc_e.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  PAD=$PAD DUMP=$DUMP" > "$OUT/exit.txt"
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
W = 4; H = (len(fs) + W - 1) // W
b = Image.new('RGB', (320 * W, 240 * max(H, 1)))
for i, im in enumerate(fs):
    b.paste(im, ((i % W) * 320, (i // W) * 240))
b.save(os.path.join(os.path.dirname(d), 'bogen.png'))
print(len(fs), 'Bilder')
PY
cat "$OUT/exit.txt"
grep -a "CONTINUE\|r30-doc\] F.*st=3" "$OUT/debug.log" | head -3
