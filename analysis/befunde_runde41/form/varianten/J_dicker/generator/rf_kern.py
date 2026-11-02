#!/usr/bin/env python3
"""Rohbau final, Kern: Knochen, Vorwaertsrechnung, Ringteile mit gebundenen Parametern (Mitte spiegelgleich, links = Spiegel von rechts), weiche Deckung, harte Kennung.
Aus Rohbau C (rc_kern.py) weitergefuehrt. Kein RE-Wert: neues Modell nach zwei Videostandbildern."""
import json
import math
import os
import sys

import cv2
import numpy as np
import torch

HIER = os.path.dirname(os.path.abspath(__file__))
def _form_ordner(h):
    """Varianten: der Generator liegt eine oder zwei Ebenen unter form/. FORM = erster Vorfahr mit formblatt.json."""
    d = h
    for _ in range(4):
        d = os.path.dirname(d)
        if os.path.exists(os.path.join(d, 'formblatt.json')):
            return os.path.normpath(d)
    return os.path.normpath(os.path.join(h, '..'))


FORM = _form_ordner(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_kamera as FK  # noqa: E402

GERAET = 'cuda' if torch.cuda.is_available() else 'cpu'
E = 2 * math.pi / 4096
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}

# Finger: Wurzel im Handraum, Spreizung in der Handebene (Grad, + zur Innenseite), Laenge bis zur Spitze. 1 innen, 2 Mitte, 3 aussen, 4 Daumen. Rechte Hand: innen = +Z, linke Hand Spiegel.
# Linke Hand ist dieselbe Hand, kleiner: Handschnitte 110 gegen 52 px bei Tiefenverhaeltnis 1,16 bis 1,25, Wortlaut Nutzer 'grosse Klauenhand' gegen 'Klaue'. Einziger Eingriff gegen die Spiegelgleichheit.
# Abnahme 2 M4: Masken belegen rund 1,3 (124 gegen 82 px bei Tiefe 5617 gegen 6508), nicht 1,5. HAND_L hebt die linke Hand auf diesen Wert.
HAND_L = 0.76
# M4 Abnahme 1: Pranke breiter und laenger, vier Finger und Daumen. Finger 2 laeuft geradeaus in den linken Lappen der Handmaske, die fuenfte Kralle (KRALLE, starr an der Hand, kein eigener Knochen) trifft (197,476).
# Abnahme 2 M3 und Bild 2: Pranke laenger und schmaler. Bild 1 sieht die flache Hand von hinten, dort zaehlt nur die Breite (Fuss der Maske 47 px, Soll rund 800 Einheiten). Bild 2 sieht die Laenge: dunkler Ballen im linken Lappen. Reichweite Gelenk bis Spitze bleibt.
FINGER = {'1': ((960, 40, 300), 25, 690), '2': ((1030, 40, -110), 0, 740), '3': ((900, 40, -340), -26, 960), '4': ((140, 100, 300), 72, 640)}
KRALLE = ((1040, 40, 95), 20, 660)


def lade_gerippe(start=True):
    with open(os.path.join(FORM, 'gerippe.json'), encoding='utf-8') as f:
        g = json.load(f)
    ks = g['knochen']
    for i, n, e in ((30, 'klaue_l4', 13), (31, 'klaue_r4', 18)):
        ks.append(dict(ks[28 if n.endswith('l4') else 29], index=i, name=n, eltern=e))
    for p in g['posen'].values():
        p['winkel'] = [list(a) for a in p['winkel']] + [list(p['winkel'][28]), list(p['winkel'][29])]
    ix = namen(g)
    for s, vz in (('r', 1), ('l', -1)):
        for f_, ((x, y, z), w, lg) in FINGER.items():
            k = ks[ix['klaue_%s%s' % (s, f_)]]
            sk = HAND_L if s == 'l' else 1.0
            k['versatz'] = [x * sk, y * sk, z * vz * sk]
            a = math.radians(w) * vz
            k['achse_von'] = [0, 0, 0]
            k['achse_bis'] = [lg * math.cos(a), 0, lg * math.sin(a)]
            k['laenge'] = lg
            k['spreiz'] = w
    if start:
        # Startposen: Endstand Rohbau C (gerippe_c.json), Lage und Gier wie gerippe.json.
        with open(os.path.join(HIER, 'gerippe_c.json'), encoding='utf-8') as f:
            gc = json.load(f)
        for n, nm in ((1, 'bild1'), (2, 'bild2')):
            b = gc['keyframes'][gc['clips'][n - 1]['kf']]
            g['posen'][nm]['winkel'] = [list(a) for a in b['winkel']]
            g['posen'][nm]['wurzel'] = list(b['wurzel'])
    return g


def namen(g):
    return {k['name']: k['index'] for k in g['knochen']}


def rot_achsen(a):
    """a[...,3] Winkel in Bogenmass -> M = Rx Ry Rz."""
    sx, cx = torch.sin(a[..., 0]), torch.cos(a[..., 0])
    sy, cy = torch.sin(a[..., 1]), torch.cos(a[..., 1])
    sz, cz = torch.sin(a[..., 2]), torch.cos(a[..., 2])
    n = torch.zeros_like(sx)
    e = torch.ones_like(sx)
    Rx = torch.stack([e, n, n, n, cx, -sx, n, sx, cx], -1).reshape(a.shape[:-1] + (3, 3))
    Ry = torch.stack([cy, n, sy, n, e, n, -sy, n, cy], -1).reshape(a.shape[:-1] + (3, 3))
    Rz = torch.stack([cz, -sz, n, sz, cz, n, n, n, e], -1).reshape(a.shape[:-1] + (3, 3))
    return Rx @ Ry @ Rz


def twist_y(a):
    """Drehung um die eigene Y-Achse (Swing-Twist) aus Euler M = Rx Ry Rz, Bogenmass. a[...,3] -> Winkel."""
    hx, hy, hz = a[..., 0] / 2, a[..., 1] / 2, a[..., 2] / 2
    cx, sx, cy, sy, cz, sz = torch.cos(hx), torch.sin(hx), torch.cos(hy), torch.sin(hy), torch.cos(hz), torch.sin(hz)
    # q = qx * qy * qz
    w = cx * cy * cz - sx * sy * sz
    y = cx * sy * cz - sx * cy * sz
    return 2 * torch.atan2(y, w)


class Kette:
    def __init__(self, g):
        self.eltern = [k['eltern'] for k in g['knochen']]
        self.n = len(self.eltern)

    def __call__(self, winkel, versatz, wurzel, gier, lage):
        """winkel[n,3] Engine-Einheiten, versatz[n,3], wurzel[3], gier Engine-Einheiten (Zahl oder Tensor). -> R[n,3,3], T[n,3] in Welt."""
        M = rot_achsen(winkel * E)
        Rl, Tl = [None] * self.n, [None] * self.n
        for i, e in enumerate(self.eltern):
            if e < 0:
                Rl[i], Tl[i] = M[i], wurzel
            else:
                Rl[i] = Rl[e] @ M[i]
                Tl[i] = Rl[e] @ versatz[i] + Tl[e]
        R = torch.stack(Rl)
        T = torch.stack(Tl)
        a = torch.as_tensor(gier, dtype=R.dtype, device=R.device) * E
        s, c = torch.sin(a), torch.cos(a)
        Y = torch.stack([torch.stack([c, 0 * c, s]), torch.stack([0 * c, 0 * c + 1, 0 * c]), torch.stack([-s, 0 * c, c])])
        return Y @ R, (T @ Y.T) + lage


class Kamera:
    def __init__(self, nr, geraet=GERAET):
        k = FK.lade_kamera(nr)
        self.k = k
        self.P = torch.tensor(k['P'], dtype=torch.float32, device=geraet)
        self.Pn = np.array(k['P'], float)

    def __call__(self, x):
        h = x @ self.P[:, :3].T + self.P[:, 3]
        return h[..., :2] / h[..., 2:3]

    def tiefe(self, x):
        return x @ self.P[2, :3] + self.P[2, 3]


def ringecken(n, phase=0.0):
    w = 2 * np.pi * (np.arange(n) + phase) / n
    return np.cos(w), np.sin(w)


class Teil:
    """Ringfolge an einem Knochen. t[T] Lage laengs in Einheiten ab o, c[T,2] Mitte quer (a, b), r[T,n] Radius je Ecke, relief[T,n] feste log-Faktoren.
    spitze0, spitze1: Laenge der Endkappe, apex0, apex1: Versatz (a, b) der Kappenspitze gegen die Ringmitte. feld: gemeinsamer Atlas-Streifen.
    bind = 'sym' (Mitte: Ecken j und n-j gleich, b = 0), Index des Meister-Teils (Spiegel) oder None."""

    def __init__(self, name, knochen, o, d, ea, eb, t, c, r, relief=None, spitze0=0.0, spitze1=0.0, apex0=(0, 0), apex1=(0, 0), feld=None, offen0=False, zone=None):
        self.name, self.knochen = name, knochen
        self.o, self.d, self.ea, self.eb = [np.asarray(v, float) for v in (o, d, ea, eb)]
        self.t = np.asarray(t, float)
        self.c = np.asarray(c, float)
        self.r = np.asarray(r, float)
        self.T, self.n = self.r.shape
        self.relief = np.zeros_like(self.r) if relief is None else np.asarray(relief, float)
        self.spitze0, self.spitze1 = float(spitze0), float(spitze1)
        self.apex0, self.apex1 = np.asarray(apex0, float), np.asarray(apex1, float)
        self.feld = feld
        self.offen0 = offen0
        self.bind = None
        self.zone = zone
        self.skala = 1.0
        self.dt = np.zeros_like(self.r)     # Versatz je Ecke laengs: Ring steht schraeg (Kamm, Rand)

    def flaechen(self):
        return (self.T - 1) * self.n + (0 if self.offen0 else self.n) + self.n


class Netz:
    """Alle Teile als ein Tensorsatz. Freie Parameter gibt es nur je Meister-Teil."""

    def __init__(self, teile, geraet=GERAET):
        self.teile = teile
        self.g = geraet
        self.pl = []
        offs, tris, tk, ve = 0, [], [], []
        self.offs = []
        for p in teile:
            cs, sn = ringecken(p.n)
            T, n = p.T, p.n
            dat = {k: torch.tensor(getattr(p, k), dtype=torch.float32, device=geraet) for k in ('o', 'd', 'ea', 'eb', 't', 'relief', 'apex0', 'apex1', 'dt')}
            dat['cs'] = torch.tensor(cs, dtype=torch.float32, device=geraet)
            dat['sn'] = torch.tensor(sn, dtype=torch.float32, device=geraet)
            dat['spiegel'] = torch.tensor([(-j) % n for j in range(n)], device=geraet)
            self.pl.append(dat)
            self.offs.append(offs)
            idx = lambda a, b: offs + a * n + (b % n)  # noqa: E731
            for a in range(T - 1):
                for b in range(n):
                    q = [idx(a, b), idx(a, b + 1), idx(a + 1, b + 1), idx(a + 1, b)]
                    tris += [[q[0], q[1], q[2]], [q[0], q[2], q[3]]]
                    tk += [p.knochen] * 2
            c0, c1 = offs + T * n, offs + T * n + 1
            for b in range(n):
                if not p.offen0:
                    tris.append([c0, idx(0, b + 1), idx(0, b)])
                    tk.append(p.knochen)
                tris.append([c1, idx(T - 1, b), idx(T - 1, b + 1)])
                tk.append(p.knochen)
            ve += [p.knochen] * (T * n + 2)
            offs += T * n + 2
        self.nv = offs
        self.tris = torch.tensor(tris, dtype=torch.long, device=geraet)
        self.tkn = torch.tensor(tk, dtype=torch.long, device=geraet)
        self.vkn = torch.tensor(ve, dtype=torch.long, device=geraet)
        self.meister = [i for i, p in enumerate(teile) if not isinstance(p.bind, int)]

    def startwerte(self):
        lr, c = {}, {}
        for i in self.meister:
            p = self.teile[i]
            lr[i] = torch.tensor(np.log(np.maximum(p.r, 1.0)), dtype=torch.float32, device=self.g)
            c[i] = torch.tensor(p.c, dtype=torch.float32, device=self.g)
        return lr, c

    def wirksam(self, lr, c):
        """-> je Teil log-Radien und Mitten nach Bindung (Spiegel, Symmetrie)."""
        L, C = [], []
        for i, p in enumerate(self.teile):
            if isinstance(p.bind, int):
                L.append(L[p.bind] + math.log(p.skala))
                C.append(C[p.bind] * p.skala)
                continue
            l_, c_ = lr[i], c[i]
            if p.bind == 'sym':
                l_ = 0.5 * (l_ + l_[:, self.pl[i]['spiegel']])
                c_ = torch.stack([c_[:, 0], 0 * c_[:, 1]], 1)
            L.append(l_)
            C.append(c_)
        return L, C

    def ecken(self, lr, c):
        L, C = self.wirksam(lr, c)
        aus = []
        for p, d, l, m in zip(self.teile, self.pl, L, C):
            r = torch.exp(l + d['relief'])
            pos = d['o'] + d['t'][:, None] * d['d']
            mitte = pos + m[:, :1] * d['ea'] + m[:, 1:2] * d['eb']
            ring = mitte[:, None, :] + (r * d['cs'])[..., None] * d['ea'] + (r * d['sn'])[..., None] * d['eb'] + d['dt'][..., None] * d['d']
            k0 = mitte[0] - p.spitze0 * d['d'] + d['apex0'][0] * d['ea'] + d['apex0'][1] * d['eb']
            k1 = mitte[-1] + p.spitze1 * d['d'] + d['apex1'][0] * d['ea'] + d['apex1'][1] * d['eb']
            aus.append(torch.cat([ring.reshape(-1, 3), k0[None], k1[None]]))
        return torch.cat(aus), L, C

    def welt(self, v, R, T):
        Rv = R[self.vkn]
        return (Rv @ v[..., None])[..., 0] + T[self.vkn]


def innen_tiefe(q, p, dat, l, c, R, T):
    """q[N,3] Welt. -> d[N] > 0: Punkt liegt so tief im Teil p (Abstand zur Wand quer zur Achse), sonst <= 0. Vereinfachung: Querschnitt je Ring, zwischen den Ringen linear."""
    x = (q - T[p.knochen]) @ R[p.knochen]
    rel = x - dat['o']
    t_ = rel @ dat['d']
    ts = dat['t']
    i = torch.searchsorted(ts, t_.detach().contiguous()).clamp(1, p.T - 1)
    f = ((t_ - ts[i - 1]) / (ts[i] - ts[i - 1]))
    gueltig = (f >= 0) & (f <= 1)
    f = f.clamp(0, 1)
    cm = c[i - 1] * (1 - f[:, None]) + c[i] * f[:, None]
    a_ = rel @ dat['ea'] - cm[:, 0]
    b_ = rel @ dat['eb'] - cm[:, 1]
    rho = torch.sqrt(a_ * a_ + b_ * b_ + 1e-3)
    th = torch.atan2(b_, a_) % (2 * math.pi)
    jf = th / (2 * math.pi) * p.n
    j0 = torch.floor(jf).long() % p.n
    j1 = (j0 + 1) % p.n
    g = jf - torch.floor(jf)
    r = torch.exp(l + dat['relief'])
    r0 = r[i - 1, j0] * (1 - g) + r[i - 1, j1] * g
    r1 = r[i, j0] * (1 - g) + r[i, j1] * g
    rw = r0 * (1 - f) + r1 * f
    # Wandabstand: Sehne zwischen zwei Ecken liegt innerhalb des Umkreises, Faktor cos(pi/n) als Mittel
    d = rw * (0.5 + 0.5 * math.cos(math.pi / p.n)) - rho
    return torch.where(gueltig, d, torch.full_like(d, -1e3))


def weich(xy, tris, kanal, H, W, sigma, schritt=1, x0=0, y0=0, C=None):
    """Weiche Deckung je Kanal. xy[N,2] Bildpixel, tris[M,3], kanal[M]. Raster: Pixelmitten x0+schritt*(i+0.5). -> S[C,H,W]."""
    g = xy.device
    C = C or int(kanal.max().item()) + 1
    q = (xy - torch.tensor([x0, y0], dtype=xy.dtype, device=g)) / schritt - 0.5
    s = sigma / schritt
    a, b, c = q[tris[:, 0]], q[tris[:, 1]], q[tris[:, 2]]
    fl = (b[:, 0] - a[:, 0]) * (c[:, 1] - a[:, 1]) - (b[:, 1] - a[:, 1]) * (c[:, 0] - a[:, 0])
    tausch = fl < 0
    b2 = torch.where(tausch[:, None], c, b)
    c2 = torch.where(tausch[:, None], b, c)
    ok = fl.abs() > 1e-4
    a, b2, c2, ka = a[ok], b2[ok], c2[ok], kanal[ok]
    with torch.no_grad():
        mn = torch.minimum(torch.minimum(a, b2), c2) - 3 * s
        mx = torch.maximum(torch.maximum(a, b2), c2) + 3 * s
        x_0 = torch.clamp(torch.ceil(mn[:, 0]), 0, W - 1).long()
        y_0 = torch.clamp(torch.ceil(mn[:, 1]), 0, H - 1).long()
        x_1 = torch.clamp(torch.floor(mx[:, 0]), 0, W - 1).long()
        y_1 = torch.clamp(torch.floor(mx[:, 1]), 0, H - 1).long()
        wi = (x_1 - x_0 + 1).clamp(min=0)
        he = (y_1 - y_0 + 1).clamp(min=0)
        an = wi * he
        an[(mx[:, 0] < 0) | (mx[:, 1] < 0) | (mn[:, 0] > W - 1) | (mn[:, 1] > H - 1)] = 0
        sel = an > 0
        it = torch.repeat_interleave(torch.arange(len(an), device=g)[sel], an[sel])
        st = torch.cumsum(torch.cat([torch.zeros(1, dtype=torch.long, device=g), an[sel][:-1]]), 0)
        rel = torch.arange(len(it), device=g) - torch.repeat_interleave(st, an[sel])
        px = x_0[it] + rel % wi[it]
        py = y_0[it] + rel // wi[it]
    pf, qf = px.float(), py.float()

    def rand(u, v):
        e = v - u
        l = torch.sqrt((e * e).sum(1) + 1e-6)
        return ((pf - u[it, 0]) * (-e[it, 1]) + (qf - u[it, 1]) * e[it, 0]) / l[it]
    d = torch.stack([rand(a, b2), rand(b2, c2), rand(c2, a)], 0)
    sa = torch.sigmoid(d / s)
    deck = sa[0] * sa[1] * sa[2]
    lg = torch.log1p(-deck.clamp(max=0.9995))
    aus = torch.zeros(C * H * W, device=g, dtype=lg.dtype)
    aus = aus.index_add(0, ka[it] * (H * W) + py * W + px, lg)
    return 1 - torch.exp(aus.reshape(C, H, W))


def kennbild(xy, z, tris, kanal, H, W, schritt=1, x0=0, y0=0):
    """Harte Kennung: fuer jedes Pixel der Kanal der vordersten Flaeche (Z-Puffer auf Dreieckmitte, wie das Spiel nach Tiefe sortiert). -> id[H,W], -1 leer."""
    q = ((xy - np.array([x0, y0], float)) / schritt)
    zt = z[tris].mean(1)
    folge = np.argsort(-zt)
    bild = np.full((H, W), -1, np.int32)
    k = (q[tris] * 16).astype(np.int32)
    for i in folge:
        cv2.fillConvexPoly(bild, k[i], int(kanal[i]), lineType=cv2.LINE_8, shift=4)
    return bild


def masken():
    m = {}
    for t in TEILE[1] + TEILE[2]:
        x = cv2.imread(os.path.join(FORM, 'nutzer_teile', 'maske_%s.png' % t), 0)
        m[t] = x > 0
    logo = cv2.imread(os.path.join(FORM, 'pruefung', 'logo_maske.png'), 0) > 0
    return m, logo


def ziel(n):
    return cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
