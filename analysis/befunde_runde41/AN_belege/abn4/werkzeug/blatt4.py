#!/usr/bin/env python3
"""blatt4.py - Abnahme 4: Bildleiste aus einer RE15_FRAMEDUMP-Serie (f<NNNNNN>.ppm), Feld je Bild mit Bildnummer und
optionaler Zeile aus dem state.log (Annette-Slot/Gegner).
  python blatt4.py <laufdir> <ausgabe.jpg> <spalten> <bild1,bild2,...|alle> [breite_feld]"""
import sys, os, glob, re
from PIL import Image, ImageDraw, ImageFont
d, out, cols, wahl = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4]
fw = int(sys.argv[5]) if len(sys.argv) > 5 else 320
files = sorted(glob.glob(os.path.join(d, "f*.ppm")))
nr = {int(re.search(r"f(\d+)\.ppm$", f).group(1)): f for f in files}
if wahl == "alle": sel = sorted(nr)
else: sel = [int(x) for x in wahl.split(",") if int(x) in nr]
if not sel: sys.exit("keine Bilder")
im0 = Image.open(nr[sel[0]]); w, h = im0.size; fh = int(h * fw / w)
rows = (len(sel) + cols - 1) // cols
sheet = Image.new("RGB", (cols * fw, rows * (fh + 18)), (20, 20, 20))
dr = ImageDraw.Draw(sheet)
try: font = ImageFont.truetype("arial.ttf", 13)
except Exception: font = ImageFont.load_default()
for i, f in enumerate(sel):
    im = Image.open(nr[f]).convert("RGB").resize((fw, fh))
    x, y = (i % cols) * fw, (i // cols) * (fh + 18)
    sheet.paste(im, (x, y + 18))
    dr.text((x + 4, y + 2), "F%d" % f, fill=(255, 255, 0), font=font)
sheet.save(out, quality=85)
print(out, len(sel), "Bilder", sheet.size)
