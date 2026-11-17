"""Kachel PV/P2 offline (unbeleuchtete Texelfarbe, Maler-Reihenfolge), Ausschnitt um den Kopf, Nutzerlinien (vorn/linien.json) in PV
eingezeichnet (weiss/hellblau, nur ein Punkt je Spalte = Oberkante der Linie), Hintergrund schwarz. Zeilen V7, R6, R2."""
import sys, os, json, numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn
LIN = json.load(open(os.path.dirname(os.path.abspath(__file__)) + '/../vorn/linien.json'))


def rgb(E, r):
    m = r['bone'] >= 0; c = np.zeros(r['bone'].shape, np.int64)
    c[m] = E.clut[r['zeile'][m] - E.clut_y, E.tex[r['v'][m], r['u'][m]]]
    out = np.zeros(r['bone'].shape + (3,), np.uint8)
    out[m] = (np.stack([(c[m] & 31), (c[m] >> 5) & 31, (c[m] >> 10) & 31], -1) * 255 // 31).astype(np.uint8)
    return out


A = dict(L.ansichten()); rows = []
for n in ('V7', 'R6', 'R2', 'R3'):
    E = Vn.lade(n); cols = []
    for nm, (cx, cy) in (('PV', (440, 215)), ('P2', (None, None))):
        L.setvar(L.VAR[0]); r = pl.raster_maler(E, pl.world_tris(E, A[nm])); im = rgb(E, r)
        if cy is None:
            ys, xs = np.nonzero(r['bone'] == 9); cx, cy = int(xs.mean()), int(ys.mean())
        x0, y0 = cx - 60, cy - 50
        c = Image.fromarray(im[y0:y0 + 100, x0:x0 + 120]).resize((480, 400), Image.NEAREST); d = ImageDraw.Draw(c)
        if nm == 'PV':
            for key, col in (('weiss', (255, 255, 255)), ('hellblau', (80, 200, 255))):
                for x, (ya, yb) in LIN[key].items():
                    X, Y = (int(x) - x0) * 4, (ya - y0) * 4
                    d.rectangle([X, Y, X + 3, Y + 3], outline=col)
        d.text((4, 4), '%s %s' % (n, nm), fill=(255, 255, 0)); cols.append(np.asarray(c))
    rows.append(np.concatenate(cols, 1))
Image.fromarray(np.concatenate(rows, 0)).save(os.path.dirname(os.path.abspath(__file__)) + '/../k/k4_pv_p2.png')
