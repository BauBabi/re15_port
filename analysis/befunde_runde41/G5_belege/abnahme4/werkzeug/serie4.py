# Abnahme 4: Bild-fuer-Bild-Serie (Ausschnitt, 2x) RE2 (Videobild bei re2_t(n)+versatz) | Port vorher | Leon NB4 | Elza NB4
import subprocess, sys, os, numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t, FPS
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
src, out = sys.argv[1], sys.argv[2]; ns = [int(x) for x in sys.argv[3].split(",")]; box = tuple(int(v) for v in sys.argv[4].split(","))
vers = float(sys.argv[5]) if len(sys.argv) > 5 else 0.0
A3 = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5abn3/lauf"
def re2(n):
    raw = subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-ss", "%.3f" % (re2_t(n) + (vers + 0.5) / FPS), "-i", src, "-frames:v", "1",
                          "-vf", "scale=640:480,format=rgb24", "-f", "rawvideo", "-"], capture_output=True).stdout
    return Image.frombytes("RGB", (640, 480), raw)
Q = [("RE2 echt", re2), ("vorher", lambda n: Image.open(A3 + "/fd_leon/fd/f%06d.ppm" % (1613 + n))),
     ("Leon NB4", lambda n: Image.open("lauf/fd_leon/fd/f%06d.ppm" % (1613 + n))), ("Elza NB4", lambda n: Image.open("lauf/fd_elza/fd/f%06d.ppm" % (1613 + n)))]
bw, bh = box[2] - box[0], box[3] - box[1]
img = Image.new("RGB", (bw * len(Q), (bh + 14) * len(ns))); dr = ImageDraw.Draw(img)
for r, n in enumerate(ns):
    for c, (nm, f) in enumerate(Q):
        im = f(n).convert("RGB").resize((640, 480), Image.BILINEAR).crop(box)
        img.paste(im, (c * bw, r * (bh + 14) + 14)); dr.text((c * bw + 3, r * (bh + 14) + 1), "%s %d" % (nm, n), fill=(255, 255, 0))
img.save(out, quality=90); print(out)
