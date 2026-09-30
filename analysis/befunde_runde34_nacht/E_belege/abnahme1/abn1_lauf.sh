#!/bin/bash
# abn1_lauf.sh - Runde 34 Nacht, Spur E, ABNAHME 1 (unabhaengig): ein Lauf der ECHTEN exe.
# Eigene exe-Kopie re15_pc_abn1.exe in build/r34n_e/abn1/mess (local_build.sh beendet nur exe
# des Bauverzeichnisses). Spielstart ueber CONTINUE (RE15_CONTINUE_TEST + RE15_CARD_AUTO) von
# der Karte $KARTE (Vorgabe: Nutzerkarte Runde 30, Platz 0 = ROOM1150), weitere Schalter als
# VAR=wert-Argumente. Bilder: RE15_FRAMEDUMP (Readback vor SDL_RenderPresent, beschleunigter
# Renderer, kein AUTOSHOT/SOFTWARE_RENDER), RE15_WINDOW_SCALE=3.
# Aufruf: bash abn1_lauf.sh <name> <dump "a-b/s"> <exit_at> [VAR=wert ...]
#   KARTE=<pfad> (Umgebung) ersetzt die Karte; KARTE=behalten laesst die Karte im Messordner.
set -u
export PATH="/c/msys64/mingw64/bin:$PATH"
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_dokumente
NAME="$1"; DUMP="$2"; EXIT_AT="$3"; shift 3
MESS="$BAUM/build/r34n_e/abn1/mess"
OUT="$BAUM/build/r34n_e/abn1/$NAME"
mkdir -p "$MESS"; rm -rf "$OUT"; mkdir -p "$OUT/bild"
cp -f "$BAUM/re15_port/build/platform/pc/re15_pc.exe" "$MESS/re15_pc_abn1.exe"
K="${KARTE:-$BAUM/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr}"
if [ "$K" != "behalten" ]; then cp -f "$K" "$MESS/re15_card.mcr"; fi
cp -f "$MESS/re15_card.mcr" "$OUT/karte_vorher.mcr"
rm -f "$MESS/debug.log"
cd "$MESS"
env SDL_AUDIODRIVER=dummy RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 \
    RE15_WINDOW_SCALE=3 RE15_DOC_LOG=1 RE15_MSG_LOG=1 \
    RE15_FRAMEDUMP="$DUMP:$OUT/bild/f" RE15_EXIT_AT="$EXIT_AT" "$@" \
    timeout -k 5 420 ./re15_pc_abn1.exe > "$OUT/stdout.txt" 2> "$OUT/stderr.txt"
echo "exe exit $?  NAME=$NAME DUMP=$DUMP EXIT_AT=$EXIT_AT ARGS=$*" > "$OUT/exit.txt"
cp debug.log "$OUT/debug.log" 2>/dev/null
cp -f re15_card.mcr "$OUT/karte_nachher.mcr" 2>/dev/null
C:/Python310/python.exe - "$OUT/bild" <<'PY'
import os, sys
from PIL import Image
d = sys.argv[1]
fs = []
for n in sorted(os.listdir(d)):
    if n.endswith('.ppm'):
        im = Image.open(os.path.join(d, n)).convert('RGB')
        im.save(os.path.join(d, n[:-4] + '.png'))
        os.remove(os.path.join(d, n))
        fs.append((n, im.resize((320, 240), Image.BILINEAR)))
if fs:
    W = 4; H = (len(fs) + W - 1) // W
    b = Image.new('RGB', (320 * W, 240 * H))
    for i, (n, im) in enumerate(fs):
        b.paste(im, ((i % W) * 320, (i // W) * 240))
    b.save(os.path.join(os.path.dirname(d), 'bogen.png'))
print(len(fs), 'Bilder')
PY
cat "$OUT/exit.txt"
