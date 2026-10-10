import sys, os
from PIL import Image, ImageChops
a, b = sys.argv[1], sys.argv[2]
for n in range(40, 331):
    fa = os.path.join(a, "f%06d.ppm" % n); fb = os.path.join(b, "f%06d.ppm" % n)
    if not (os.path.exists(fa) and os.path.exists(fb)): continue
    ia = Image.open(fa).convert("RGB"); ib = Image.open(fb).convert("RGB")
    d = ImageChops.difference(ia, ib).convert("L").point(lambda v: 255 if v > 24 else 0)
    bb = d.getbbox(); cnt = sum(1 for v in d.getdata() if v)
    if cnt: print("F%d diff px %d bbox %s" % (n, cnt, bb))
