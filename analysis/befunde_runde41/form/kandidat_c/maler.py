# -*- coding: utf-8 -*-
"""Kandidat C: Textur malen. Grundhaut aus der Palette (Beulen, Falten, Knoten, Adern), Toene aus den rueckgerechneten Bildern als harte Stufen,
Zeichnung Z1-Z9, Q1-Q5, G, P aus den gemessenen Polygonen. Unschaerfe gehoert dem Video: hier wird nichts weich uebernommen."""
from atlas import *
from scipy.ndimage import gaussian_filter, map_coordinates, distance_transform_edt
from scipy.spatial import cKDTree
import form_pruefung as PR

M = FB['zeichnung']['materialien']
HAUT = np.array(M['grundhaut_dunkel']['oberflaeche_rgb'], float)
ROT = np.array(M['rotbraun_mittelton']['oberflaeche_rgb'], float)
ROSA = np.array(M['rosa_stelle']['oberflaeche_rgb'], float)
BAND = np.array(M['helle_baender']['oberflaeche_rgb'], float)
LINIE = np.array(M['feine_linien']['oberflaeche_rgb'], float)
F1 = np.array((0.487, 0.432, 0.374))
F2 = np.array((0.385, 0.342, 0.307))
YK = np.array([.299, .587, .114])

# Tonleiter: Helligkeit der Textur -> Farbe. Stufen aus farbe_palette.json, Zwischenstufen gemischt.
LEITER = np.array([(16, 8, 12), (28, 12, 19), (44, 18, 27), (60, 30, 40), (82, 46, 66), (103, 63, 84), (112, 92, 112), (114, 123, 143), (126, 145, 156), (168, 178, 188), (222, 228, 232)], float)
LEITER_Y = LEITER @ YK
HAUT_STUFEN = np.array([(10, 5, 8), (18, 8, 13), (27, 11, 17), (36, 15, 23), (46, 20, 29), (58, 28, 38)], float)


def leiter(y):
    """Texturhelligkeit -> Stufe (hart)."""
    i = np.abs(y[..., None] - LEITER_Y).argmin(-1)
    return LEITER[i], i


class Rauschen:
    def __init__(self, saat, n=96):
        g = np.random.default_rng(saat)
        self.n = n
        a = gaussian_filter(g.standard_normal((n, n, n)), 1.5, mode='wrap')
        self.a = (a - a.mean()) / a.std()

    def __call__(self, p, mass):
        q = (p / mass) * (self.n / 8.0)
        return map_coordinates(self.a, [q[..., 0].ravel(), q[..., 1].ravel(), q[..., 2].ravel()], order=1, mode='grid-wrap').reshape(p.shape[:-1])


class Maler:
    def __init__(self, T, karten, posen_trafo):
        """posen_trafo: {1: f(ti, V), 2: f(ti, V)} Ecken/Punkte eines Teils in die Pose des Bildes."""
        self.T, self.K = T, karten
        self.P, self.N, self.ID = karten_rastern(T, karten)
        self.belegt = self.ID >= 0
        self.tr = posen_trafo
        self.R1 = Rauschen(11)
        self.R2 = Rauschen(23)

    def punkte_in_pose(self, n):
        Q = self.P.copy()
        for ti in range(len(self.T)):
            m = self.ID == ti
            if m.any():
                Q[m] = self.tr[n](ti, self.P[m].astype(float))
        return Q

    def sicht(self, n, s):
        Z = z_puffer(self.T, s, self.tr[n])
        Q = self.punkte_in_pose(n)
        p = s.bild(Q.reshape(-1, 3)).reshape(Q.shape[0], Q.shape[1], 2)
        d = s.tiefe(Q.reshape(-1, 3)).reshape(Q.shape[:2])
        xi = np.clip(np.round(np.nan_to_num(p[..., 0])).astype(int), 0, 639)
        yi = np.clip(np.round(np.nan_to_num(p[..., 1])).astype(int), 0, 479)
        Nn = self.N.copy()
        for ti in range(len(self.T)):
            m = self.ID == ti
            if m.any():
                a = self.tr[n](ti, self.P[m].astype(float) + self.N[m] * 100.0)
                Nn[m] = (a - Q[m]) / 100.0
        r = s.auge_aktor() - Q
        r /= np.linalg.norm(r, axis=2, keepdims=True) + 1e-6
        nv = (Nn * r).sum(2)
        vis = self.belegt & (d <= Z[yi, xi] + 90) & (nv > 0.08)
        return dict(p=p, xi=xi, yi=yi, vis=vis, nv=nv, Z=Z)

    def streu(self, r, saat):
        """Punkte auf der Flaeche mit Mindestabstand r."""
        g = np.random.default_rng(saat)
        ys, xs = np.nonzero(self.belegt)
        idx = g.permutation(len(ys))[:60000]
        Q = self.P[ys[idx], xs[idx]].astype(float)
        zelle = {}
        aus = []
        for q in Q:
            k = tuple((q // r).astype(int))
            nah = False
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    for dz in (-1, 0, 1):
                        for w in zelle.get((k[0] + dx, k[1] + dy, k[2] + dz), ()):
                            if np.sum((w - q) ** 2) < r * r:
                                nah = True
            if not nah:
                zelle.setdefault(k, []).append(q)
                aus.append(q)
        return np.array(aus)

    def charts(self):
        gesehen = []
        for t, k in zip(self.T, self.K):
            if any(k is q for q in gesehen):
                continue
            gesehen.append(k)
            yield t, k

    def grundhaut(self):
        """Dunkle Haut wie Elefantenhaut, gemalt: feines Rissnetz im Raum (laeuft ueber Teilgrenzen), wenige grosse Falten je Karte, Wuelste, Licht von oben eingebrannt,
        alles in harte Stufen gesetzt. Bleibt unter (70,40,45), im Video also schwarz."""
        b = self.belegt
        g = np.random.default_rng(31)
        P = self.P[b].astype(float)
        N = self.N[b].astype(float)
        N = N / (np.linalg.norm(N, axis=1, keepdims=True) + 1e-6)
        # Rissnetz
        pk = self.streu(105.0, 4)
        d, i = cKDTree(pk).query(P, k=2)
        riss = d[:, 1] - d[:, 0]
        self.zelle = np.full(b.shape, -1, np.int32)
        self.zelle[b] = i[:, 0]
        self.rissmaske = np.zeros(b.shape, bool)
        self.rissmaske[b] = riss < 7.0
        kuppe = np.clip(1 - d[:, 0] / 95.0, 0, 1)
        hoch = -(P[:, 1] - pk[i[:, 0], 1]) / 105.0
        ton = 0.30 + 0.20 * self.R1(P, 620.0) + 0.07 * self.R2(P, 240.0) + 0.14 * (-N[:, 1]) + 0.16 * hoch * (0.3 + kuppe)
        q = np.clip(np.round(ton * (len(HAUT_STUFEN) - 1)).astype(int), 0, len(HAUT_STUFEN) - 1)
        q = np.where(riss < 15.0, np.maximum(q - 2, 0), q)
        q = np.where(riss < 7.0, 0, q)
        rgb = np.zeros(self.P.shape, np.float32)
        rgb[b] = HAUT_STUFEN[q]

        # grosse Falten: je Karte einzelne Striche quer zum Glied, Lichtkante darueber, Schatten darunter
        def feld(h, w, sy, sx):
            f = gaussian_filter(g.standard_normal((h, w)), (sy, sx), mode=('nearest', 'wrap'))
            return f / (f.std() + 1e-9)

        for t, k in self.charts():
            x0, y0, w, h = k['px'], k['py'], k['w'], k['h']
            rumpf = t['name'] == 'rumpf'
            H = h + 12
            q_ = feld(H, w, 3.4 if rumpf else 2.4, w * (0.05 if rumpf else 0.11))
            v = feld(H, w, 9, w * 0.05) * 7.0
            yy, xx = np.mgrid[0:H, 0:w].astype(np.float32)
            q_ = cv2.remap(q_.astype(np.float32), xx, np.clip(yy + v.astype(np.float32), 0, H - 1), cv2.INTER_LINEAR)
            sperre = feld(H, w, 9, w * 0.05)                     # Falten nur stellenweise
            kern = (np.abs(q_) < 0.10) & (sperre > 0.15)
            kern = kern[6:6 + h]
            licht = np.zeros_like(kern)
            licht[:-1] = kern[1:]
            licht &= ~kern
            o = rgb[y0:y0 + h, x0:x0 + w]
            mb = b[y0:y0 + h, x0:x0 + w]
            o[licht & mb] = (52, 24, 33)
            o[kern & mb] = (10, 5, 8)
        return rgb

    def adern(self, rgb, n=64, saat=9):
        """Adern als verzweigte Striche auf den Karten, laengs zum Glied. Eine Reihe darunter dunkel, im Stamm zwei Texel breit."""
        g = np.random.default_rng(saat)
        ks = list(self.charts())
        fl = np.array([k['w'] * k['h'] for _, k in ks], float)
        fl /= fl.sum()

        def strich(k, x, y, a, lg, br):
            for s in range(lg):
                a += g.normal(0, 0.28)
                x += math.sin(a) * 1.0
                y += math.cos(a) * 1.0
                X, Y = int(round(x)), int(round(y))
                if not (k['px'] + 1 <= X < k['px'] + k['w'] - 1 and k['py'] + 1 <= Y < k['py'] + k['h'] - 2):
                    return
                if not self.belegt[Y, X]:
                    continue
                dick = br if s < lg * 0.55 else 1
                for d in range(dick):
                    if X + d < TEX_B:
                        rgb[Y, X + d] = (64, 34, 46)
                        rgb[Y + 1, X + d] = (12, 6, 9)
                if lg > 14 and g.uniform() < 0.05:
                    strich(k, x, y, a + g.choice([-1, 1]) * g.uniform(0.5, 1.0), int(lg * 0.5), 1)

        for _ in range(n):
            t, k = ks[int(g.choice(len(ks), p=fl))]
            x = k['px'] + g.uniform(0.03, 0.97) * k['w']
            y = k['py'] + g.uniform(0.03, 0.97) * k['h']
            strich(k, x, y, g.normal(0, 0.45), int(g.integers(16, 46)), int(g.integers(1, 3)))
        return rgb

    def knoten(self, rgb, n=46, saat=5):
        """Knoten in Gruppen: runde Kuppe, oben Lichtkante, unten Schatten."""
        g = np.random.default_rng(saat)
        ys, xs = np.nonzero(self.belegt)
        for i in g.integers(0, len(ys), n):
            cy, cx = ys[i], xs[i]
            for _ in range(int(g.integers(2, 7))):
                y = int(cy + g.normal(0, 6))
                x = int(cx + g.normal(0, 6))
                r = float(g.uniform(1.4, 3.4))
                R = int(r) + 2
                for dy in range(-R, R + 1):
                    for dx in range(-R, R + 1):
                        Y, X = y + dy, x + dx
                        if not (0 <= Y < TEX_H and 0 <= X < TEX_B) or not self.belegt[Y, X]:
                            continue
                        d = (dx * dx + dy * dy) ** 0.5
                        if d <= r:
                            rgb[Y, X] = (62, 32, 44) if (dy < -0.35 * r) else ((44, 20, 30) if dy < 0.4 * r else (28, 12, 18))
                        elif d <= r + 1.0 and dy >= 0:
                            rgb[Y, X] = (10, 5, 8)
        return rgb


def wulst(rgb, maske, grund, rand_dunkel=0.80, kern_hell=1.13, halo=None, grenzen=None, mod=None):
    """Marke mit gemaltem Wulst: Rand dunkler, Kern heller, optional Mittelton als Saum. grenzen = (von, bis) haelt die Palette. mod: Faktorkarte, damit Risse der Haut durchscheinen."""
    d = distance_transform_edt(maske)
    f = np.ones(maske.shape, np.float32)
    f[(d > 0) & (d <= 1.01)] = rand_dunkel
    f[d >= 3.0] = kern_hell
    if mod is not None:
        f = f * mod
    c = (grund[None, None, :] * f[..., None])
    if grenzen is not None:
        c = np.clip(c, grenzen[0], grenzen[1])
    if halo is not None:
        sa = (distance_transform_edt(~maske) <= 2.0) & ~maske & (rgb.sum(-1) > 0)
        rgb[sa] = np.maximum(rgb[sa], halo)
    rgb[maske] = c[maske]
    return rgb


def vorlage_bild(n, U):
    """Helligkeit der Figur aus ECHT, Rand frei von Bodenfarbe, Zeichnung geschaerft. U = Rendermaske."""
    e = ECHT[n].astype(np.float32)
    m = U.astype(np.uint8)
    d = cv2.distanceTransform(m, cv2.DIST_L2, 5)
    k = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11))
    dunkel = cv2.erode(e, k)
    a = np.clip((d - 2.0) / 7.0, 0, 1)[..., None]
    e2 = a * e + (1 - a) * dunkel
    e2 = e2 * (m[..., None] > 0) + cv2.blur(e2 * (m[..., None] > 0), (9, 9)) * (m[..., None] == 0)
    gl = cv2.bilateralFilter(e2, 7, 18, 3)
    bl = cv2.GaussianBlur(gl, (0, 0), 3.0)
    sch = gl + 1.1 * (gl - bl)
    return np.clip(sch, 0, 255)


def lesen(bild, s):
    return np.stack([map_coordinates(bild[..., c], [np.nan_to_num(s['p'][..., 1]).ravel(), np.nan_to_num(s['p'][..., 0]).ravel()], order=1, mode='nearest').reshape(s['xi'].shape) for c in range(bild.shape[2])], -1)
