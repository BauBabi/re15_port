# Leser A V3: Uebersichtstafel der KI-Freisteller birkin01-24 auf Grau (nur Ansicht)
import sys
from PIL import Image, ImageDraw
SRC = r'C:/workspace/git/reAi_v2/Version_80%/'
OUT = sys.argv[1]
ims = [Image.open(SRC + 'birkin%02d.png' % i).convert('RGBA') for i in range(1, 25)]
W, Hh = 240, 330
sheet = Image.new('RGB', (W * 8, Hh * 3), (90, 140, 90))
d = ImageDraw.Draw(sheet)
for k, im in enumerate(ims):
    s = min(W / im.width, (Hh - 14) / im.height)
    t = im.resize((int(im.width * s), int(im.height * s)), Image.LANCZOS)
    x, y = (k % 8) * W, (k // 8) * Hh
    sheet.paste(t, (x, y + 14), t)
    d.text((x + 3, y + 1), 'birkin%02d' % (k + 1), fill=(255, 255, 0))
sheet.save(OUT)
