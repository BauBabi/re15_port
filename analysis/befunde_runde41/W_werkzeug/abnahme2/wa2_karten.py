"""wa2_karten.py - Abnahme W2: 16 Cut-Karten (blitz_karte.png aus wa2_blitz.py: Medianbild, rot = Blitz, gelb = unruhig)
eines Raums/einer Figur zu einem 4x4-Blatt. Aufruf: wa2_karten.py <runs_dir> <prefix z.B. K_fix_leon_2000> <out.jpg>"""
import os, sys
from PIL import Image, ImageDraw
runs, pre, out = sys.argv[1:4]
S = Image.new("RGB", (4 * 322, 4 * 256), (20, 20, 20)); dr = ImageDraw.Draw(S)
for c in range(16):
    p = os.path.join(runs, "%s_c%02d" % (pre, c), "blitz_karte.png")
    X, Y = (c % 4) * 322, (c // 4) * 256
    if os.path.exists(p): S.paste(Image.open(p).convert("RGB").resize((320, 240)), (X, Y + 14))
    t = open(os.path.join(runs, "%s_c%02d" % (pre, c), "fertig.txt"), encoding="utf-8").read() if os.path.exists(os.path.join(runs, "%s_c%02d" % (pre, c), "fertig.txt")) else "fehlt"
    import re
    m = re.search(r"Blitz-Ereignisse ausserhalb Figur (\d+).*?unruhige Flecken ausserhalb Figur (\d+)", t)
    dr.text((X + 2, Y + 1), "Cut %d: Blitze %s, unruhig %s" % (c, m.group(1) if m else "?", m.group(2) if m else "?"), fill=(255, 255, 0))
S.save(out, quality=88); print(out)
