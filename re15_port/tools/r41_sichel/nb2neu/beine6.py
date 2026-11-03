"""NB6 Beinhaltung: Kniewinkel, Fuss-/Knieabstand, Beckenhoehe und Knie aussen/innen des gezeichneten Birkin je Videobild h
aus dem Messhaken RE15_BEIN_LOG (main.c: Knochen 0-6 in Weltkoordinaten nach allen Posen-Haken), Zuordnung h ueber die
Letterbox (c6.off). Dazu die Bildlage der Knoechel/Knie (c6.proj, Video-Koordinaten 640x480).
python beine6.py <lauf> [h,h,..]  -> Tabelle; als Modul: beine(lauf) -> {h: dict}"""
import sys, os, re, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *

def lese(run):
    R = {}
    for L in open(os.path.join(rundir(run), 'bein.log'), errors='ignore'):
        m = re.match(r'\[bein\] F(\d+) slot(\d+) mo=(\d+) af=(\d+) gier=(-?\d+)(.*)', L)
        if not m: continue
        P = {int(b): np.array([int(x), int(y), int(z)], float) for b, x, y, z in re.findall(r' (\d)\((-?\d+),(-?\d+),(-?\d+)\)', m.group(6))}
        R[int(m.group(1))] = dict(slot=int(m.group(2)), mo=int(m.group(3)), af=int(m.group(4)), gier=int(m.group(5)), P=P)
    return R

def winkel(a, b, c):
    """Winkel bei b zwischen b->a und b->c in Grad (180 = gestreckt)."""
    u, v = a - b, c - b
    return float(np.degrees(np.arccos(np.clip(u @ v / (np.linalg.norm(u) * np.linalg.norm(v)), -1, 1))))

def kennzahlen(P):
    hr, kr, ar, hl, kl, al = P[1], P[2], P[3], P[4], P[5], P[6]
    quer = hl - hr; quer[1] = 0; quer /= max(np.linalg.norm(quer), 1e-9)          # Beckenquerachse rechts -> links
    def aussen(h, k, a, seite):
        """Knie neben der Linie Huefte -> Knoechel, auf die Querachse projiziert, + = nach aussen (O-Bein), - = innen (X-Bein)"""
        u = (a - h) / np.linalg.norm(a - h); t = (k - h) @ u; lot = (k - h) - t * u
        return float(lot @ quer * seite)
    return dict(knie_r=round(winkel(hr, kr, ar), 1), knie_l=round(winkel(hl, kl, al), 1),
                fuss=int(np.hypot(*(al - ar)[[0, 2]])), fuss_quer=int(abs((al - ar) @ quer)),
                knie_ab=int(np.hypot(*(kl - kr)[[0, 2]])), huefte_ab=int(np.hypot(*(hl - hr)[[0, 2]])),
                becken_hoehe=int(-(P[0][1] - (ar[1] + al[1]) / 2)),
                aussen_r=int(aussen(hr, kr, ar, -1.0)), aussen_l=int(aussen(hl, kl, al, 1.0)))

def beine(run):
    L = lese(run); o = off(run); out = {}
    for F, e in L.items():
        if e['slot'] < 0: continue
        h = F - o
        k = kennzahlen(e['P'])
        k['bild'] = {n: [int(round(c)) for c in proj(e['P'][b])] for n, b in (('knoechel_r', 3), ('knoechel_l', 6), ('knie_r', 2), ('knie_l', 5), ('becken', 0))}
        k['mo'], k['af'] = e['mo'], e['af']
        out[h] = k
    return out

if __name__ == '__main__':
    run = sys.argv[1]
    hs = [int(x) for x in sys.argv[2].split(',')] if len(sys.argv) > 2 else list(range(0, 170, 4))
    B = beine(run)
    print('%-5s %5s %5s %6s %6s %6s %6s %6s %6s  %s' % ('h', 'KnieR', 'KnieL', 'Fuss', 'FussQ', 'KnieAb', 'Becken', 'aussR', 'aussL', 'Bild Knoechel r/l, Knie r/l'))
    for h in hs:
        if h not in B: continue
        k = B[h]
        print('h%-4d %5.0f %5.0f %6d %6d %6d %6d %6d %6d  %s %s %s %s  clip %d B%d' % (h, k['knie_r'], k['knie_l'], k['fuss'], k['fuss_quer'], k['knie_ab'],
              k['becken_hoehe'], k['aussen_r'], k['aussen_l'], k['bild']['knoechel_r'], k['bild']['knoechel_l'], k['bild']['knie_r'], k['bild']['knie_l'], k['mo'], k['af']))
    json.dump(B, open(os.path.join(RUNS, 'beine_%s.json' % run), 'w'))

def fuesse_bild(im, y0=370, y1=480, x0=250, x1=600):
    """Fusspunkte am BILD (Video und Port gleich): rote Fuesse (r > g + 30, r > 70, r > b + 25) im Bodenband y0..y1, unterste
    60 Zeilen der roten Bildpunkte; zwei Haufen an der groessten x-Luecke -> Schwerpunkte (xl, yl), (xr, yr)."""
    c = im[y0:y1, x0:x1].astype(int); b, g, r = c[..., 0], c[..., 1], c[..., 2]
    m = (r > g + 30) & (r > 70) & (r > b + 25)
    m = cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_OPEN, np.ones((2, 2), np.uint8)) > 0
    ys, xs = np.nonzero(m)
    if len(xs) < 30: return None
    sel = ys >= ys.max() - 60; ys, xs = ys[sel], xs[sel]
    o = np.sort(np.unique(xs)); gaps = np.diff(o)
    if len(gaps) == 0 or gaps.max() < 12: return None
    cut = o[np.argmax(gaps)] + 0.5
    L, R = xs < cut, xs >= cut
    if L.sum() < 8 or R.sum() < 8: return None
    return [int(xs[L].mean() + x0), int(ys[L].mean() + y0), int(xs[R].mean() + x0), int(ys[R].mean() + y0)]
