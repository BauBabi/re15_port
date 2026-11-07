"""Stufe 3: Bildfolge aus dem echten Fenster (s3_spielfolge.sh) als Blatt: jedes k-te Bild, Zeitmarke darunter."""
import sys, glob, os
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import s3_rig  # noqa: F401  (Pfad auf re15_port/tools)
import emd_ansichtsblatt as EA
d, aus, schritt = sys.argv[1], sys.argv[2], int(sys.argv[3])
von = int(sys.argv[4]) if len(sys.argv) > 4 else 0
bis = int(sys.argv[5]) if len(sys.argv) > 5 else 10 ** 6
spalten = int(sys.argv[6]) if len(sys.argv) > 6 else 6
fps = float(sys.argv[7]) if len(sys.argv) > 7 else 2.0
titel = sys.argv[8] if len(sys.argv) > 8 else ''
fs = sorted(glob.glob(os.path.join(d, 'bild_*.png')))
sel = [(i, f) for i, f in enumerate(fs) if von <= i < bis][::schritt]
w, h = 400, 300
zeilen = (len(sel) + spalten - 1) // spalten
B = Image.new('RGB', (8 + spalten * (w + 8), 34 + zeilen * (h + 26)), (18, 19, 22))
dr = ImageDraw.Draw(B)
dr.text((8, 8), titel, font=EA.font(16), fill=(200, 203, 210))
for k, (i, f) in enumerate(sel):
    im = Image.open(f).convert('RGB').resize((w, h), Image.LANCZOS)
    x, y = 8 + (k % spalten) * (w + 8), 34 + (k // spalten) * (h + 26)
    B.paste(im, (x, y))
    dr.text((x + 2, y + h + 3), 'Bild %d  t = %.1f s' % (i + 1, i / fps), font=EA.font(13), fill=(214, 138, 74))
os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
B.save(aus)
print(aus, len(sel))
