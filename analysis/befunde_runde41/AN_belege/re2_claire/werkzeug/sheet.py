"""sheet.py OUT.jpg COLS BILD... - Kontaktbogen aus Vorschaubildern (mit Dateinamen)."""
import sys, os
from PIL import Image, ImageDraw
out, cols = sys.argv[1], int(sys.argv[2]); fs = sys.argv[3:]
ims = [Image.open(f).convert("RGB") for f in fs]
w, h = ims[0].size
rows = (len(ims) + cols - 1) // cols
W = Image.new("RGB", (cols * w, rows * (h + 14)), (20, 20, 20))
d = ImageDraw.Draw(W)
for i, (f, im) in enumerate(zip(fs, ims)):
    x, y = (i % cols) * w, (i // cols) * (h + 14)
    W.paste(im.resize((w, h)), (x, y + 14)); d.text((x + 3, y + 1), os.path.basename(f)[:40], fill=(255, 255, 0))
W.save(out, quality=88)
