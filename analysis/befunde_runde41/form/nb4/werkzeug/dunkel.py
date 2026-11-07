# Anteil fast schwarzer Bildpunkte (< 24, Luma wie PIL 'L') je FRAMEDUMP-Bild, mit Cut aus state.log und Wahl aus kam.log.
#   python dunkel.py <laufdir> [von] [bis]
import sys, os, glob, re
import numpy as np
from PIL import Image
d = sys.argv[1]
von = int(sys.argv[2]) if len(sys.argv) > 2 else 0
bis = int(sys.argv[3]) if len(sys.argv) > 3 else 10**9
cam = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'^F(\d+) pad=.*? cam=(\d+)', line)
    if m: cam[int(m.group(1))] = int(m.group(2))
form = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'^F(\d+) pad=', line)
    if not m: continue
    e = re.search(r't=30 st=(\d+) [^]]*mo=(\d+) af=(\d+) [^]]*@\((-?\d+),(-?\d+)', line)
    if e: form[int(m.group(1))] = 'st%s mo%s x%s' % (e.group(1), e.group(2), e.group(4))
for p in sorted(glob.glob(os.path.join(d, 'fd', 'f*.ppm'))):
    f = int(os.path.basename(p)[1:7])
    if f < von or f > bis: continue
    a = np.asarray(Image.open(p).convert('L'), dtype=np.float32)
    print('F%d cam=%s dunkel=%.1f%% mittel=%.1f %s' % (f, cam.get(f, '?'), (a < 24).mean() * 100, a.mean(), form.get(f, '')))
