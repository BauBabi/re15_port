"""Pruefer P5: Foto-Pose PV (Nutzer-Darstellung) mit eigener Rasterung (pl, Tiefenpuffer wie Runde 5/6; dazu Maler).
 - Oberkontur x 418-465 gegen die Nutzerlinien (vorn/linien.json, aus Darstellung.png - Darstellung_Vorlage.png > 60 und Linienfarbe)
 - Buckel-Rot: Anteil a* >= 10 (unbeleuchtete Texelfarbe) in den obersten 8 px der Figur, x 428-445 (Wortlaut riegel_v8.huegel_rot;
   dort beleuchtet im Videoraum - hier unbeleuchtet, deshalb mit R4 (Riegel 58,3 %) und V7 geeicht)
 - S3 (b) Huegel: hoechster Punkt der Oberkontur x 405-447 (Riegel-Fenster 428-445) und (e) K: Abweichung der rechten Figurkante y 212-243
   (x 420-500) von der Ausgleichsgeraden (Riegel-Soll K <= 3)
 - S2c: Rumpf (Knochen 0-15) ueber Schwanz (16-22) an der Naht (Wortlaut riegel_v8.j_huckel), 7 Halbpixel-Varianten, Soll <= 3 px
Aufruf: python pv.py"""
import sys, os, json
import numpy as np
from skimage.color import rgb2lab
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn
LIN = json.load(open(os.path.dirname(os.path.abspath(__file__)) + '/../vorn/linien.json'))


def rgb(E, r):
    m = r['bone'] >= 0; c = np.zeros(r['bone'].shape, np.int64)
    c[m] = E.clut[r['zeile'][m] - E.clut_y, E.tex[r['v'][m], r['u'][m]]]
    out = np.zeros(r['bone'].shape + (3,)); out[m] = np.stack([(c[m] & 31), (c[m] >> 5) & 31, (c[m] >> 10) & 31], -1) / 31.0
    return out


def oben(F, x0, x1):
    return {x: int(np.nonzero(F[:, x])[0].min()) for x in range(x0, x1 + 1) if F[:, x].any()}


def naht(E, v):
    L.setvar(v)
    rr = pl.raster(E, pl.world_tris(E, pl.PV, bones=list(range(16)))); rs = pl.raster(E, pl.world_tris(E, pl.PV, bones=list(range(16, 23))))
    tr = oben(rr['bone'] >= 0, 300, 459); ts = oben(rs['bone'] >= 0, 300, 459)
    ob = [x for x in range(330, 421) if x in ts and x in tr and ts[x] <= tr[x]]
    if not ob: return None, 99.0
    xn = max(ob); H = [(ts[x] - tr[x], x) for x in range(xn - 15, xn + 16) if x in ts and x in tr]
    return xn, float(max(H)[0])


res = {}
for n in ['V7', 'R4', 'R5D', 'R6', 'R2', 'R3']:
    E = Vn.lade(n); L.setvar(L.VAR[0]); r = pl.raster(E, pl.world_tris(E, pl.PV)); F = r['bone'] >= 0
    om = oben(F, 380, 470)
    # Linien
    dw = [om[int(x)] - y[0] for x, y in LIN['weiss'].items() if int(x) <= 465 and int(x) in om]
    dh = [om[int(x)] - y[0] for x, y in LIN['hellblau'].items() if int(x) in om]
    # Buckel-Rot
    Lab = rgb2lab(rgb(E, r)); v = []
    for x in range(428, 446):
        ys = np.nonzero(F[:, x])[0]
        if len(ys): v += [Lab[y, x, 1] >= 10 for y in range(ys.min(), ys.min() + 8) if F[y, x]]
    rot = 100.0 * float(np.mean(v))
    # Huegel
    xs = [x for x in range(405, 448) if x in om]; xh = min(xs, key=lambda x: (om[x], x)); hmin = om[xh]
    links = min(om[x] for x in range(380, xh + 1) if x in om); tal = max(om[x] for x in range(390, xh) if x in om) if xh > 390 else hmin
    # K
    fm = {}
    for y in range(212, 244):
        c = np.nonzero(F[y, 420:500])[0]
        if len(c): fm[y] = 420 + int(c.max())
    yy = np.array(sorted(fm)); xx = np.array([fm[y] for y in yy], float); cf = np.polyfit(yy, xx, 1); K = float(np.abs(xx - np.polyval(cf, yy)).max())
    s2 = [naht(E, vv) for vv in L.VAR]
    res[n] = dict(weiss=dw, hellblau=dh, rot=rot, huegel_x=xh, huegel_y=hmin, tal=tal, K=K, s2c=s2, om={x: om[x] for x in range(418, 466) if x in om})
    print('%-4s Oberkontur - Nutzerlinie (px, + = unter der Linie): weiss x452-465 %s | hellblau x418-447 Mittel %+.1f max |%d| | Buckel-Rot %.1f %% | '
          'hoechster Punkt x405-447 bei x %d (y %d), tiefstes Tal davor y %d | K %.1f | S2c je Variante %s' % (
              n, dw, np.mean(dh), max(abs(d) for d in dh), rot, xh, hmin, tal, K, [h for _, h in s2]))
json.dump(res, open(os.path.dirname(os.path.abspath(__file__)) + '/../vorn/pv.json', 'w'))
