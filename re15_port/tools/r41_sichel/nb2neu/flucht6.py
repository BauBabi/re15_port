"""NB6 Flucht: Spider-Man h68-h90 je Bild, Video gegen Port am gleichen Bild h. Maske = Differenz zum figurfreien eigenen
Hintergrund (Median h100-h120, |d|max > 45, wie sm4.py) OHNE Birkin (Differenzmaske des eigenen Bilds h90, 9x9 erweitert;
Birkin steht h70-h90 fast still), Oeffnen 3x3, y 45-435. Video mit abgezogenem Schwarzwert. Ausgabe: Flaeche, Oberkante (5 %),
Unterkante (95 %), Schwerpunkt, Breite/Hoehe (5-95 %) und Lage (liegt: Hoehe < 0,6 Breite und Unterkante > 360; aufrecht:
Hoehe > 1,3 Breite; sonst geduckt). python flucht6.py lauf[,lauf..]"""
import sys, os, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
k3 = np.ones((3, 3), np.uint8)
def bgm(src): return np.median(np.stack([src(h) for h in range(100, 121, 2)]), axis=0).astype(np.float32)
def dm(im, B):
    m = (np.abs(im.astype(np.float32) - B).max(axis=2) > 45).astype(np.uint8); m[:45] = 0; m[435:] = 0; return m
def flucht(src):
    B = bgm(src); bi = cv2.dilate(dm(src(90), B), np.ones((9, 9), np.uint8)) > 0
    out = {}
    for h in range(68, 91):
        m = cv2.morphologyEx((dm(src(h), B) > 0) & ~bi, cv2.MORPH_OPEN, k3) if False else \
            cv2.morphologyEx(((dm(src(h), B) > 0) & ~bi).astype(np.uint8), cv2.MORPH_OPEN, k3) > 0
        ys, xs = np.nonzero(m)
        if len(xs) < 150: out[h] = None; continue
        t, b = np.percentile(ys, 5), np.percentile(ys, 95); l, r = np.percentile(xs, 5), np.percentile(xs, 95)
        hh, ww = b - t, r - l
        lage = 'liegt' if (hh < 0.6 * ww and b > 360) else ('aufrecht' if hh > 1.3 * ww else 'geduckt')
        out[h] = dict(A=int(len(xs)), oben=int(t), unten=int(b), cx=int(xs.mean()), cy=int(ys.mean()), x=(int(l), int(r)), lage=lage)
    return out
if __name__ == '__main__':
    R = {'Video': flucht(lambda h: vsw(vid(h)))}
    for run in sys.argv[1].split(','): R[run] = flucht(lambda h, r=run: port(r, h))
    for h in range(68, 91):
        z = 'h%-3d' % h
        for k, v in R.items():
            z += ' | %-5s ' % k[:5] + ('-' if v[h] is None else 'A%5d y%3d-%3d c(%3d,%3d) x%3d-%3d %s' % (v[h]['A'], v[h]['oben'], v[h]['unten'], v[h]['cx'], v[h]['cy'], v[h]['x'][0], v[h]['x'][1], v[h]['lage']))
        print(z)
    json.dump(R, open(os.path.join(RUNS, 'flucht_%s.json' % sys.argv[1].replace(',', '_')), 'w'))
