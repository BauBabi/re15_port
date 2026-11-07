# Abnahme 3: vorher/nachher je Bild - Schnittfolge (state.log) und fast schwarz (Framedump, PIL-L < 24) in einem Bildfenster.
#   python dunkel_vergleich.py <laufdir> <von> <bis>
import sys, os, re, glob
import numpy as np
from PIL import Image
d, von, bis = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
cam = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = re.match(r'^F(\d+) pad=.*? cam=(\d+)', line)
    if m and von <= int(m.group(1)) <= bis and int(m.group(1)) not in cam: cam[int(m.group(1))] = int(m.group(2))
seq = []
for f in sorted(cam):
    if not seq or seq[-1][0] != cam[f]: seq.append([cam[f], f, f])
    else: seq[-1][2] = f
n600 = 0; tot = 0; mx = (0, 0); je = {}
for p in sorted(glob.glob(os.path.join(d, 'fd', 'f*.ppm'))):
    f = int(os.path.basename(p)[1:7])
    if not (von <= f <= bis): continue
    a = np.asarray(Image.open(p).convert('L'), dtype=np.int32); dn = int((a < 24).sum() * 1000 // a.size); tot += 1
    if dn > 600:
        n600 += 1; je[cam.get(f)] = je.get(cam.get(f), 0) + 1
    if dn > mx[0]: mx = (dn, f)
print('%s F%d-F%d: Schnitte %s; %d Bilder, > 600 Promille fast schwarz: %d %s, max %d (F%d)' % (
    os.path.basename(d.rstrip('/')), von, bis, ' '.join('%d:F%d-F%d' % tuple(s) for s in seq), tot, n600, je, mx[0], mx[1]))
