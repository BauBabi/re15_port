# -*- coding: utf-8 -*-
"""Runde 31 / H — kleiner z-Puffer-Rasterer der Cut-4-Szene (NUR PLANUNG, Float-Naeherung; die
Engine sortiert Dreiecke nach mittlerer Tiefe, Massstab bleibt der Framedump).
Szene: Prop 0 (Plattform, rot_y 2048, Lage (-20700, py, -17460) = main00 Obj_model_set @0x0E00
`2d 00 ... 24 af 9c b0 cc bb ... 00 08`), Prop 1/2 an ihrer Elternmatrix (@0x0E22 / @0x0E44 `01 c0`,
Lage 0, offen z +150 / -150), Sicherung und Granate an der Plattform-Elternmatrix.
ID-Puffer: 0 Hintergrund, 1 Plattform, 2 Deckel, 3 Sicherung, 4 Granate."""
import math, os, sys
import numpy as np
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, H)
import r31_geo as G  # noqa
from r30_lib import props, md1_lesen, rdt_laden  # noqa

W_, H_ = 320, 240


def tris_aus(m):
    v = m['tv']
    out = [[v[i] for i in t] for t in m['tris']]
    for q in m['quads']:
        a, b, c, d = (v[i] for i in q)
        out.append([a, b, d]); out.append([a, d, c])
    return np.array(out, float)


class Szene:
    def __init__(self, raum='ROOM1150.RDT'):
        rdt = rdt_laden(raum)
        pr = props(rdt)
        self.plat = tris_aus(md1_lesen(pr[0]['md1'])['meshes'][0])
        self.d1 = tris_aus(md1_lesen(pr[1]['md1'])['meshes'][0])
        self.d2 = tris_aus(md1_lesen(pr[2]['md1'])['meshes'][0])
        self.sich = tris_aus(G.inc_md1('sicherung_prop.inc', 're15_sicherung_md1'))
        self.gran = tris_aus(G.inc_md1('granate_prop.inc', 're15_granate_md1'))
        f = G.Fach(raum)
        V = f.view
        self.R = np.array(V['rot'], float).reshape(3, 3) / 4096.0
        self.T = np.array(V['trans'], float)
        self.Hh = V['H']

    @staticmethod
    def dreh(T, ry):
        w = ry * 2 * math.pi / 4096
        c, s = math.cos(w), math.sin(w)
        x, y, z = T[..., 0], T[..., 1], T[..., 2]
        return np.stack([x * c + z * s, y, -x * s + z * c], -1)

    def render(self, py, deckel=150, s_sitz=None, g_sitz=None):
        teile = [(self.plat, 1), (self.d1 + np.array([0, 0, deckel]), 2), (self.d2 + np.array([0, 0, -deckel]), 2)]
        if s_sitz:
            (x, y, z, ry) = s_sitz
            teile.append((self.dreh(self.sich, ry) + np.array([x, y, z]), 3))
        if g_sitz:
            (x, y, z, ry) = g_sitz
            teile.append((self.dreh(self.gran, ry) + np.array([x, y, z]), 4))
        zb = np.full((H_, W_), np.inf)
        idb = np.zeros((H_, W_), np.uint8)
        for T, ident in teile:
            Wl = np.stack([-20700 - T[..., 0], py + T[..., 1], -17460 - T[..., 2]], -1)
            C = Wl @ self.R.T + self.T
            for tri in C:
                if np.any(tri[:, 2] < 64):
                    continue
                sx = 160 + tri[:, 0] * self.Hh / tri[:, 2]
                sy = 120 + tri[:, 1] * self.Hh / tri[:, 2]
                x0, x1 = int(max(0, math.floor(sx.min()))), int(min(W_ - 1, math.ceil(sx.max())))
                y0, y1 = int(max(0, math.floor(sy.min()))), int(min(H_ - 1, math.ceil(sy.max())))
                if x0 > x1 or y0 > y1:
                    continue
                den = (sy[1] - sy[2]) * (sx[0] - sx[2]) + (sx[2] - sx[1]) * (sy[0] - sy[2])
                if abs(den) < 1e-9:
                    continue
                gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
                l1 = ((sy[1] - sy[2]) * (gx - sx[2]) + (sx[2] - sx[1]) * (gy - sy[2])) / den
                l2 = ((sy[2] - sy[0]) * (gx - sx[2]) + (sx[0] - sx[2]) * (gy - sy[2])) / den
                l3 = 1 - l1 - l2
                m = (l1 >= 0) & (l2 >= 0) & (l3 >= 0)
                if not m.any():
                    continue
                z = l1 * tri[0, 2] + l2 * tri[1, 2] + l3 * tri[2, 2]
                sub = zb[y0:y1 + 1, x0:x1 + 1]
                ids = idb[y0:y1 + 1, x0:x1 + 1]
                w = m & (z < sub)
                sub[w] = z[w]
                ids[w] = ident
        return idb

    @staticmethod
    def auswerten(idb):
        aus = {}
        for ident, name in ((3, 'S'), (4, 'G')):
            ys, xs = np.nonzero(idb == ident)
            aus[name] = (len(xs), float(xs.mean()) if len(xs) else None, (xs.min(), xs.max(), ys.min(), ys.max()) if len(xs) else None)
        return aus


if __name__ == '__main__':
    sz = Szene()
    for py in (-305, -1205):
        idb = sz.render(py, 150, (-280, -1062, 1260, 1024), (-362, -1088, 1260, 1024))
        print('BESTAND py %d: %s' % (py, sz.auswerten(idb)))
