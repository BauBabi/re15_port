# -*- coding: utf-8 -*-
"""Kandidat C: Kamera, Rahmen, Schnellraster. Kein RE-Wert, nur Modellbau gegen die beiden Bilder."""
import sys, os, json, math
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.normpath(os.path.join(HIER, '..'))
BAUM = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
TOOLS = os.path.join(BAUM, 're15_port', 'tools')
sys.path.insert(0, TOOLS)
import form_kamera as FK          # noqa: E402

KAM = {1: FK.lade_kamera(1), 2: FK.lade_kamera(2)}
FB = json.load(open(os.path.join(FORM, 'formblatt.json'), encoding='utf-8'))
ZIEL = {n: cv2.imread(os.path.join(FORM, 'ziel_bild%d.png' % n), 0) for n in (1, 2)}
FIGUR = {n: cv2.imread(os.path.join(FORM, 'pruefung', 'figur_bild%d.png' % n), 0) > 0 for n in (1, 2)}
BILD = {n: cv2.imread(os.path.join(FORM, 'bild%d.png' % n))[:, :, ::-1].copy() for n in (1, 2)}
ECHT = {n: cv2.imread(os.path.join(FORM, 'farbe_bild%d_echt.png' % n))[:, :, ::-1].copy() for n in (1, 2)}


def yrot(g):
    a = g * 2 * math.pi / 4096.0
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


class Stand:
    """Aktorlage + Gier + Kamera. Aktorraum: +X = Ende N (vorn), Y unten, +Z = Seite R (rechts in Bild 1, weil Ende N zur Kamera zeigt)."""

    def __init__(self, n, lage, gier):
        self.n, self.lage, self.gier = n, np.array(lage, float), gier
        self.R = yrot(gier)
        self.k = KAM[n]

    def welt(self, p):
        return (np.asarray(p, float) @ self.R.T) + self.lage

    def aktor(self, w):
        return (np.asarray(w, float) - self.lage) @ self.R

    def bild(self, p):
        return FK.projiziere(self.k, self.welt(p))

    def tiefe(self, p):
        return FK.tiefe(self.k, self.welt(p))

    def auf_tiefe(self, xy, d):
        """Bildpunkte + Tiefe -> Aktorpunkte."""
        xy = np.asarray(xy, float)
        auge, r = FK.strahl(self.k, xy)
        r = np.atleast_2d(r)
        d = np.broadcast_to(np.asarray(d, float), (len(r),))
        t = d / (r @ self.k['R'][2])
        w = auge + r * t[:, None]
        p = self.aktor(w)
        return p[0] if xy.ndim == 1 else p

    def auf_boden(self, xy, h=0.0):
        w = FK.strahl_auf_boden(self.k, np.asarray(xy, float), -h)
        return self.aktor(w)

    def auge_aktor(self):
        return self.aktor(np.asarray(self.k['auge'], float))


class SpiegelStand(Stand):
    """Gedachte Ansicht: Figur an der Ebene Z = zm gespiegelt. Ergaenzt die unsichtbare Seite (Annahme, kein Beleg)."""

    def __init__(self, s, zm):
        super().__init__(s.n, s.lage, s.gier)
        self.zm = zm

    def _sp(self, p):
        q = np.array(p, float)
        q[..., 2] = 2 * self.zm - q[..., 2]
        return q

    def bild(self, p):
        return super().bild(self._sp(p))

    def tiefe(self, p):
        return super().tiefe(self._sp(p))

    def auge_aktor(self):
        return self._sp(super().auge_aktor())


class DrehStand(Stand):
    """Ansicht eines Teils, das in dieser Pose um J gedreht ist: p_pose = J + R (p - J)."""

    def __init__(self, s, R, J):
        super().__init__(s.n, s.lage, s.gier)
        self.dr, self.J = np.asarray(R, float), np.asarray(J, float)

    def _p(self, p):
        return (np.asarray(p, float) - self.J) @ self.dr.T + self.J

    def bild(self, p):
        return super().bild(self._p(p))

    def tiefe(self, p):
        return super().tiefe(self._p(p))

    def auge_aktor(self):
        return (super().auge_aktor() - self.J) @ self.dr + self.J


def poly_flaeche(pts):
    a = np.asarray(pts, float)
    return 0.5 * np.sum(a[:, 0] * np.roll(a[:, 1], -1) - np.roll(a[:, 0], -1) * a[:, 1])


def raster(V, F, s, ziel=None):
    """Silhouette von Netz V (Aktor) mit Flaechen F (Index-Listen) in Stand s."""
    if ziel is None:
        ziel = np.zeros((480, 640), np.uint8)
    p = s.bild(V)
    for f in F:
        q = p[list(f)]
        if not np.isfinite(q).all():
            continue
        cv2.fillPoly(ziel, [np.round(q * 8).astype(np.int32)], 1, lineType=cv2.LINE_8, shift=3)
    return ziel


def kante_zeile(m, y, links=True, x0=None, x1=None):
    """Randspalte in Zeile y der Maske m im Fenster x0..x1."""
    z = m[int(y)]
    idx = np.nonzero(z)[0]
    if x0 is not None:
        idx = idx[(idx >= x0) & (idx <= x1)]
    if not len(idx):
        return None
    return float(idx.min() if links else idx.max() + 1)
