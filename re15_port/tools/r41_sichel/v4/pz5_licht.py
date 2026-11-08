"""Zahlen-Pruefer V4 r4: Lichtgegenprobe b2-Render gegen RE15_FRAMEDUMP des eigenen Laufs sfv4/pz5/lauf (bitgleich zu sfv4/r/V4r3),
je Id-Gruppe (Rumpf 7/8, Kopf 9, Oberarme 10/13, Unterarme 11/14 - die neuen CLUT-Zeilen 488/489 muessen im Spiel ankommen -, Schwanz)."""
import sys, os, re, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from pz5_mess import vraum, lab, de
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
os.environ['NB6_RUNS'] = S + 'pz5'
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'nb2neu'))
import c6   # Framedump 320x240 -> Videobild (Registrierung wie riegel_v4.lichtprobe)
mo = b2.modell(S + 'pz5/emd/EM30_V4.EMD')
txt = open(S + 'pz5/lauf/debug.log', encoding='latin-1').read()
GR = {'Rumpf 7/8': (7, 8), 'Kopf 9': (9,), 'Oberarm 10': (10,), 'Oberarm 13': (13,), 'Unterarm 11': (11,), 'Unterarm 14': (14,), 'Schwanz 16-22': tuple(range(16, 23))}
for h in (100, 110, 118, 125, 135):
    m = re.search(r'\[hangar-takt\] h%d F(\d+) B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)' % h, txt)
    if not m: print('h%d fehlt' % h); continue
    F, c, b, g, x, z = map(int, m.groups())
    p = S + 'pz5/lauf/fd/f%06d.ppm' % F
    if not os.path.exists(p): print('h%d F%d kein FRAMEDUMP' % (h, F)); continue
    img, ids = G.zeichne(mo, dict(clip=c, bild=b, gier=g, x=x, z=z))
    fdv = cv2.cvtColor(c6.port('lauf', h, F=F), cv2.COLOR_BGR2RGB)
    zeile = []
    for nm, ii in GR.items():
        rm = cv2.erode(np.isin(ids, ii).astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        if rm.sum() < 30: zeile.append('%s -' % nm); continue
        a, bb = lab(vraum(img)[rm].mean(0)), lab(vraum(fdv)[rm].mean(0))
        zeile.append('%s %.1f/%.1f dE %.2f (%d)' % (nm, a[0], bb[0], de(a, bb), rm.sum()))
    print('h%d F%d clip %d bild %d gier %d: ' % (h, F, c, b, g) + '; '.join(zeile))
    cv2.imwrite(S + 'pz5/licht_h%d.png' % h, cv2.cvtColor(np.hstack([img, fdv]), cv2.COLOR_RGB2BGR))
