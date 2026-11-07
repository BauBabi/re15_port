#!/usr/bin/env python3
"""Rohbau C, Kern: Knochen aus gerippe.json (dazu je Hand ein vierter Finger), Vorwaertsrechnung wie im Port (torch), Ringteile -> Ecken und Flaechen,
weicher Silhouetten-Rasterer fuer die Passung an die Nutzer-Masken. Kein RE-Wert: neues Modell nach zwei Videostandbildern."""
import json
import math
import os
import sys

import cv2
import numpy as np
import torch

HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_kamera as FK  # noqa: E402

GERAET = 'cuda' if torch.cuda.is_available() else 'cpu'
E = 2 * math.pi / 4096
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}


def lade_gerippe():
    with open(os.path.join(FORM, 'gerippe.json'), encoding='utf-8') as f:
        g = json.load(f)
    # Rohbau: vierter Finger je Hand (Nutzer-Umriss Bild 2 hat vier Spitzen), Knochen 30 und 31 haengen an den Haenden 13 und 18.
    ks = g['knochen']
    for i, n, e in ((30, 'klaue_l4', 13), (31, 'klaue_r4', 18)):
        ks.append(dict(ks[28 if n.endswith('l4') else 29], index=i, name=n, eltern=e))
    for p in g['posen'].values():
        p['winkel'] = [list(a) for a in p['winkel']] + [list(p['winkel'][28]), list(p['winkel'][29])]
    # Finger: Wurzeln auf die Breite der Hand, alle Achsen gerade +X. Innen = zum Koerper: rechte Hand +Z, linke Hand -Z. Daumen (4) sitzt weiter hinten. [f]
    ix = namen(g)
    for s, vz in (('r', 1), ('l', -1)):
        fuss = {'1': (640, 40, 300), '2': (700, 40, 30), '3': (600, 40, -320), '4': (230, 110, 450)}
        for f, (x, y, z) in fuss.items():
            # gerippe: l1 aussen, l3 innen; r1 innen, r3 aussen. Hier gilt fuer beide Haende 1 = innen, 2 = Mitte, 3 = aussen, 4 = Daumen.
            k = ks[ix['klaue_%s%s' % (s, f)]]
            k['versatz'] = [x, y, z * vz]
            k['achse_von'] = [0, 0, 0]
            k['achse_bis'] = [612, 0, 0]
            k['laenge'] = 612
    p = os.path.join(HIER, 'zw', 'pose_ik.json')
    if os.path.exists(p):
        with open(p) as f:
            ik = json.load(f)
        for n in ('1', '2'):
            g['posen']['bild' + n]['winkel'] = ik[n]
    # Nachstellung: Kiefer in Pose 2 weiter offen, damit er vor dem Ellbogen bis an die Kante R reicht (gerippe 113 = 10 Grad).
    g['posen']['bild2']['winkel'][ix['kiefer']][2] = 300
    # Nachstellung: In Pose 1 ragten Kopf und Zaehne unter dem Bauch hervor, Bild 1 zeigt dort nichts. Hals und Kopf je rund 11 Grad gehoben, Kopf bleibt verdeckt.
    for b in ('hals', 'kopf'):
        g['posen']['bild1']['winkel'][ix[b]][2] -= 130
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


class Kette:
    """Vorwaertsrechnung, Eltern vor Kindern. versatz und winkel duerfen Gradienten tragen."""

    def __init__(self, g):
        self.eltern = [k['eltern'] for k in g['knochen']]
        self.n = len(self.eltern)

    def __call__(self, winkel, versatz, wurzel, gier, lage):
        """winkel[n,3] in Engine-Einheiten, versatz[n,3], wurzel[3], gier in Engine-Einheiten. -> R[n,3,3], T[n,3] in Welt."""
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
        a = torch.as_tensor(gier * E, dtype=R.dtype, device=R.device)
        s, c = torch.sin(a), torch.cos(a)
        Y = torch.stack([torch.stack([c, 0 * c, s]), torch.stack([0 * c, 0 * c + 1, 0 * c]), torch.stack([-s, 0 * c, c])])
        return Y @ R, (T @ Y.T) + lage


class Kamera:
    def __init__(self, nr, geraet=GERAET):
        k = FK.lade_kamera(nr)
        self.k = k
        self.P = torch.tensor(k['P'], dtype=torch.float32, device=geraet)

    def __call__(self, x):
        h = x @ self.P[:, :3].T + self.P[:, 3]
        return h[..., :2] / h[..., 2:3]


def ringecken(n, phase=0.0):
    w = 2 * np.pi * (np.arange(n) + phase) / n
    return np.cos(w), np.sin(w)


class Teil:
    """Ringfolge an einem Knochen. t[T] Lage laengs der Achse in Einheiten ab achse_von, c[T,2] Mitte quer (a, b), r[T,n] Radius je Ecke, relief[T,n] feste Wulst-Faktoren (log).
    spitze0, spitze1: Laenge der Endkappe (Einheiten, 0 = flach), Mitte der Kappe liegt auf der Ringmitte + spitze * Achse.
    spiegel: Ecken laufen andersherum (linke Seite), damit Muster von rechts nach links gespiegelt werden."""

    def __init__(self, name, knochen, o, d, ea, eb, t, c, r, relief=None, spitze0=0.0, spitze1=0.0, feld=None, frei=True, offen0=False):
        self.name, self.knochen = name, knochen
        self.o, self.d, self.ea, self.eb = [np.asarray(v, float) for v in (o, d, ea, eb)]
        self.t = np.asarray(t, float)
        self.c = np.asarray(c, float)
        self.r = np.asarray(r, float)
        self.T, self.n = self.r.shape
        self.relief = np.zeros_like(self.r) if relief is None else np.asarray(relief, float)
        self.spitze0, self.spitze1 = float(spitze0), float(spitze1)
        self.feld = feld
        self.frei = frei
        self.offen0 = offen0

    def flaechen(self):
        return (self.T - 1) * self.n + (0 if self.offen0 else self.n) + self.n


class Netz:
    """Alle Teile als ein Tensorsatz: Ecken im Knochenraum aus Parametern, Dreiecke fuer die Silhouette."""

    def __init__(self, teile, geraet=GERAET):
        self.teile = teile
        self.g = geraet
        self.pl = []          # je Teil Tensoren
        offs, tris, tk, ve = 0, [], [], []
        for p in teile:
            cs, sn = ringecken(p.n)
            T, n = p.T, p.n
            dat = dict(o=torch.tensor(p.o, dtype=torch.float32, device=geraet), d=torch.tensor(p.d, dtype=torch.float32, device=geraet),
                       ea=torch.tensor(p.ea, dtype=torch.float32, device=geraet), eb=torch.tensor(p.eb, dtype=torch.float32, device=geraet),
                       t=torch.tensor(p.t, dtype=torch.float32, device=geraet), cs=torch.tensor(cs, dtype=torch.float32, device=geraet),
                       sn=torch.tensor(sn, dtype=torch.float32, device=geraet), relief=torch.tensor(p.relief, dtype=torch.float32, device=geraet))
            self.pl.append(dat)
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

    def startwerte(self):
        lr = [torch.tensor(np.log(np.maximum(p.r, 1.0)), dtype=torch.float32, device=self.g) for p in self.teile]
        c = [torch.tensor(p.c, dtype=torch.float32, device=self.g) for p in self.teile]
        return lr, c

    def ecken(self, lr, c):
        """-> v[nv,3] im Knochenraum."""
        aus = []
        for p, d, l, m in zip(self.teile, self.pl, lr, c):
            r = torch.exp(l + d['relief'])
            pos = d['o'] + d['t'][:, None] * d['d']
            mitte = pos + m[:, :1] * d['ea'] + m[:, 1:2] * d['eb']
            ring = mitte[:, None, :] + (r * d['cs'])[..., None] * d['ea'] + (r * d['sn'])[..., None] * d['eb']
            k0 = mitte[0] - p.spitze0 * d['d']
            k1 = mitte[-1] + p.spitze1 * d['d']
            aus.append(torch.cat([ring.reshape(-1, 3), k0[None], k1[None]]))
        return torch.cat(aus)

    def welt(self, v, R, T):
        Rv = R[self.vkn]
        return (Rv @ v[..., None])[..., 0] + T[self.vkn]


def weich(xy, tris, kanal, H, W, sigma, schritt=1, x0=0, y0=0):
    """Weiche Deckung je Kanal. xy[N,2] Bildpixel, tris[M,3], kanal[M]. Raster: Pixelmitten x0+schritt*(i+0.5). -> S[C,H,W], C = kanal.max()+1."""
    g = xy.device
    C = int(kanal.max().item()) + 1
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
    # Fuer positive Flaeche liegt das Innere links der Kante im Sinn von fl > 0 (Bildkoordinaten): Vorzeichen so, dass innen positiv ist.
    sa = torch.sigmoid(d / s)
    deck = sa[0] * sa[1] * sa[2]
    lg = torch.log1p(-deck.clamp(max=0.9995))
    aus = torch.zeros(C * H * W, device=g, dtype=lg.dtype)
    aus = aus.index_add(0, ka[it] * (H * W) + py * W + px, lg)
    return 1 - torch.exp(aus.reshape(C, H, W))


def masken():
    m = {}
    for t in TEILE[1] + TEILE[2]:
        x = cv2.imread(os.path.join(FORM, 'nutzer_teile', 'maske_%s.png' % t), 0)
        m[t] = x > 0
    logo = cv2.imread(os.path.join(FORM, 'pruefung', 'logo_maske.png'), 0) > 0
    return m, logo


def ziel(n):
    return cv2.imread(os.path.join(FORM, 'ziel2_bild%d.png' % n), 0)
