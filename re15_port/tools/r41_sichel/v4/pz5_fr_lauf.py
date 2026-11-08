"""Zahlen-Pruefer V4 r5: FR (O10) und FO-Drittel an einem beliebigen Lauf (Gegenprobe V4 r4 / V3 r2), eigener Zaehler wie pz5_neu."""
import os, sys, re
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
from pz5_mess import vraum, lab, orig
lauf = sys.argv[1]
os.environ['NB6_RUNS'] = os.path.dirname(lauf)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import c6, r4_mass as R4
txt = open(os.path.join(lauf, 'debug.log'), encoding='latin-1').read()
O1 = orig(1)
for h, fen in ((85, (300, 150, 480, 380)), (100, (300, 150, 480, 330)), (118, None), (135, None)):
    mm = re.search(r'\[hangar-takt\] h%d F(\d+) ' % h, txt)
    F = int(mm.group(1)) if mm else None
    im = c6.port(os.path.basename(lauf), h, F=F)
    if im is None:
        print(h, 'kein Bild'); continue
    im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB).astype(float)
    vid = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB).astype(float) if h != 135 else O1
    if fen:
        mp = np.zeros((480, 640), bool); mp[fen[1]:fen[3], fen[0]:fen[2]] = True; mv = mp
    else:
        mp = R4.poly_maske(R4.PS_SPIEL[h]); mv = R4.poly_maske(R4.PV_SPIEL[h])
    n = int((lab(vraum(im)[mp])[:, 1] > 22).sum()); nv = int((lab(vid[mv])[:, 1] > 22).sum())
    gr = max(10 * nv, int(0.001 * mp.sum()))
    print('h%d F%s: Spiel %d, Video %d, Grenze %d -> %s' % (h, F, n, nv, gr, 'OK' if n <= gr else 'ROT'))
