# Abnahme 3: Kampf-/Wache-Fenster je Bild: Cut, Lagen, Spieler-HP, fast schwarz, Spieler sichtbar/bedeckt (Slot 0), Form sichtbar/bedeckt.
#   python wache.py <laufdir> <von> <bis> [formslot]
import sys, os, re, glob
import numpy as np
from PIL import Image
d, von, bis = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); fs = int(sys.argv[4]) if len(sys.argv) > 4 else 12
RX = re.compile(r'^F(\d+) pad=([0-9A-Fa-f]+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)')
RXF = re.compile(r't=30 st=(\d+) ss1=(\d+) [^]]*mo=(\d+) [^]]*@\((-?\d+),(-?\d+),r-?\d+\)\] hp=(-?\d+)')
z = {}
for line in open(os.path.join(d, 'state.log'), errors='replace'):
    m = RX.match(line)
    if m and von <= int(m.group(1)) <= bis:
        e = RXF.search(line)
        z[int(m.group(1))] = (int(m.group(7)), int(m.group(3)), int(m.group(6)), (int(e.group(1)), int(e.group(2)), int(e.group(3)), int(e.group(4)), int(e.group(6))) if e else None)
deck = {}
for line in open(os.path.join(d, 'deck.log')):
    m = re.match(r'F(\d+) cam=(-?\d+)(.*)', line)
    if m: deck[int(m.group(1))] = {int(a): (int(b), int(c)) for a, b, c in re.findall(r' s(\d+)=(\d+)/(\d+)', m.group(3))}
segs = []; n600 = 0; maxd = (0, 0); pmax = (0, 0); fsum = {}
for f in sorted(z):
    c, px, php, fo = z[f]
    p = glob.glob(os.path.join(d, 'fd', 'f%06d.ppm' % f))
    dn = -1
    if p:
        a = np.asarray(Image.open(p[0]).convert('L'), dtype=np.int32); dn = int((a < 24).sum() * 1000 // a.size)
    pv, pb = deck.get(f, {}).get(0, (0, 0)); fv, fb = deck.get(f, {}).get(fs, (0, 0))
    if dn > 600: n600 += 1
    if dn > maxd[0]: maxd = (dn, f)
    if pv > pmax[0]: pmax = (pv, f)
    if not segs or segs[-1][0] != c: segs.append([c, f, f, dn, dn, pv, (pb - pv) * 1000 // pb if pb else 0])
    else:
        s = segs[-1]; s[2] = f; s[3] = min(s[3], dn) if dn >= 0 else s[3]; s[4] = max(s[4], dn); s[5] = max(s[5], pv); s[6] = max(s[6], (pb - pv) * 1000 // pb if pb else 0)
    if '--alle' in sys.argv:
        print('F%d cam=%d px=%d hp=%d form=%s dunkel=%d spieler %d/%d form %d/%d' % (f, c, px, php, fo, dn, pv, pb, fv, fb))
print('%s F%d-F%d: %d Bilder > 600 Promille fast schwarz, max %d (F%d), Spieler max %d Bildpunkte (F%d)' % (os.path.basename(d.rstrip('/')), von, bis, n600, maxd[0], maxd[1], pmax[0], pmax[1]))
for s in segs:
    print('  Cut %d F%d-F%d: fast schwarz %d..%d Promille, Spieler max %d Bildpunkte, Spieler max %d Promille verdeckt' % tuple(s))
