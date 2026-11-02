# -*- coding: utf-8 -*-
"""Rohbau B: Ring-Bauweise wie 1997. Ein Teil ist eine Folge von Ringen entlang des Knochens, je Ring Mitte, Ausrichtung und n Ecken mit eigenem Radius.
Vierecke zwischen den Ringen, Enden mit Pol oder Deckel. Keine Metaballs, keine Voxel. Starre Teile je Knochen, Mesh i haengt an Knochen i.

Ringraum: Laengsachse E, Querachsen A und B. Ecke k liegt bei Winkel 2 pi k / n, gezaehlt von +A nach +B.
  X-Knochen (Rumpf, Kopf, Hand, Fuss, Schwanz, Klaue): A = -Y (oben), B = +Z (figur-links), Ecke 0 oben, Ecke n/2 unten.
  Y-Knochen (Arm, Bein):                               A = +X (vorn),  B = aussen (links +Z, rechts -Z), Ecke 0 vorn, n/4 aussen, n/2 hinten.
"""
import math
import numpy as np


def _norm(v):
    return np.asarray(v, float) / (np.linalg.norm(v) or 1.0)


# Querschnitte fuer acht Ecken, Faktor je Ecke. Reihenfolge: A+, dann ueber B+ nach A-, ueber B- zurueck.
Q8 = dict(
    rund=(1, 1, 1, 1, 1, 1, 1, 1),
    kasten=(1, 1.22, 1, 1.22, 1, 1.22, 1, 1.22),      # eckig
    kiel=(1.22, .96, .94, .96, .9, .96, .94, .96),    # Grat auf A+
    raute=(1.2, .86, 1.1, .86, 1.2, .86, 1.1, .86),
    keil=(1.3, 1.0, .9, .8, .72, .8, .9, 1.0),        # breit an A+, schmal an A-
    platt=(.82, 1.06, 1.14, 1.06, .82, 1.06, 1.14, 1.06),
)


class Teil:
    """Eine Ringfolge, gehoert zu genau einem Knochen."""

    def __init__(self, name, knochen, n=8, achse='x', seite=0, richtung=1, a0=(0, 0, 0), mat='haut', ab=None):
        self.name, self.knochen, self.n, self.mat = name, knochen, n, mat
        e = {'x': (1, 0, 0), 'y': (0, 1, 0), 'z': (0, 0, 1)}[achse]
        self.E = np.array(e, float) * richtung
        if ab is not None:
            self.A, self.B = np.array(ab[0], float), np.array(ab[1], float)
        elif achse == 'x':
            self.A, self.B = np.array((0, -1, 0.)), np.array((0, 0, 1.))
        elif achse == 'y':
            self.A, self.B = np.array((1, 0, 0.)), np.array((0, 0, 1. if seite >= 0 else -1.))
        else:
            self.A, self.B = np.array((1, 0, 0.)), np.array((0, 1, 0.))
        self.a0 = np.array(a0, float)
        self.ringe = []        # n x 3
        self.tag = []          # je Ring Zonenname oder None
        self.pol_a = None      # Punkt vor dem ersten Ring
        self.pol_e = None      # Punkt nach dem letzten Ring
        self.offen_a = self.offen_e = False
        self.hart_ringe = set()

    def r(self, t, a, b=None, q='rund', ca=0.0, cb=0.0, tw=0.0, tag=None, kipp=(0.0, 0.0), hart=False):
        """Ring bei t mit Halbachsen a (A) und b (B). q = Name oder Faktoren je Ecke. kipp = Neigung des Rings (Grad um B, um A): Ring steht schraeg zur Achse."""
        b = a if b is None else b
        if isinstance(q, str):
            q = Q8[q] if self.n == 8 else (1,) * self.n
        q = np.asarray(q, float)
        phi = 2 * np.pi * np.arange(self.n) / self.n + math.radians(tw)
        p = np.outer(a * q * np.cos(phi) + ca, self.A) + np.outer(b * q * np.sin(phi) + cb, self.B)
        ka, kb = math.tan(math.radians(kipp[0])), math.tan(math.radians(kipp[1]))
        lage = (a * q * np.cos(phi)) * ka + (b * q * np.sin(phi)) * kb
        p = p + self.a0 + np.outer(t + lage, self.E)
        self.ringe.append(p)
        self.tag.append(tag)
        if hart:
            self.hart_ringe.add(len(self.ringe) - 1)
        return self

    def punkte(self, t, pts, tag=None, hart=False):
        """Ring aus freien Querschnittspunkten (ua, ub) je Ecke; t Zahl oder je Ecke."""
        pts = np.asarray(pts, float)
        assert len(pts) == self.n, (self.name, len(pts))
        tt = np.broadcast_to(np.asarray(t, float), (self.n,))
        self.ringe.append(self.a0 + np.outer(pts[:, 0], self.A) + np.outer(pts[:, 1], self.B) + np.outer(tt, self.E))
        self.tag.append(tag)
        if hart:
            self.hart_ringe.add(len(self.ringe) - 1)
        return self

    def stufe(self, t, dt, a, b=None, faktor=.84, q='rund', ca=0.0, cb=0.0, tag=None, tw=0.0, kipp=(0.0, 0.0)):
        """Plattenkante: Ring bei t, dahinter (t+dt) gleich mit Faktor. Ergibt eine harte Stufe mit Schattenfuge."""
        b = a if b is None else b
        self.r(t, a, b, q, ca, cb, tw, tag=tag, kipp=kipp, hart=True)
        self.r(t + dt, a * faktor, b * faktor, q, ca, cb, tw, kipp=kipp, hart=True)
        return self

    def pol(self, vorn=None, hinten=None):
        """Pol-Punkte als (t, ca, cb) im Ringraum."""
        if vorn is not None:
            self.pol_e = self.a0 + self.E * vorn[0] + self.A * vorn[1] + self.B * vorn[2]
        if hinten is not None:
            self.pol_a = self.a0 + self.E * hinten[0] + self.A * hinten[1] + self.B * hinten[2]
        return self

    def auto_pole(self, ueberstand=0.25):
        """Fehlende Pole setzen: Mitte des Endrings, etwas ueber das Ende hinaus."""
        if self.pol_a is None:
            r0, r1 = self.ringe[0], self.ringe[1 if len(self.ringe) > 1 else 0]
            m0, m1 = r0.mean(0), r1.mean(0)
            d = np.linalg.norm(r0 - m0, axis=1).mean()
            self.pol_a = m0 + _norm(m0 - m1) * d * ueberstand
        if self.pol_e is None:
            r0, r1 = self.ringe[-1], self.ringe[-2]
            m0, m1 = r0.mean(0), r1.mean(0)
            d = np.linalg.norm(r0 - m0, axis=1).mean()
            self.pol_e = m0 + _norm(m0 - m1) * d * ueberstand
        return self

    def verschieben(self, v):
        v = np.asarray(v, float)
        self.ringe = [r + v for r in self.ringe]
        if self.pol_a is not None:
            self.pol_a = self.pol_a + v
        if self.pol_e is not None:
            self.pol_e = self.pol_e + v
        return self

    def flaechenzahl(self):
        return (len(self.ringe) - 1) * self.n + (2 - self.offen_a - self.offen_e) * self.n

    def tag_flaeche(self, g):
        """Zone einer Flaeche: Tag des Rings am Flaechenanfang, Spaltenfilter optional (tag_spalten = {Ring: Menge}), ganzes Teil: self.zone_alle."""
        i, k = g
        z = None
        if 0 <= i < len(self.tag):
            z = self.tag[i]
            sp = getattr(self, 'tag_spalten', {}).get(i)
            if sp is not None and k not in sp:
                z = None
        return z or getattr(self, 'zone_alle', None)

    # ─────────────────────────────── Netz
    def netz(self):
        """-> V (m x 3), F Liste Ringe von Indizes (3 oder 4), G je Flaeche (i Ringlage, k Spalte), Pole am Ende von V."""
        self.auto_pole()
        n, m = self.n, len(self.ringe)
        V = np.vstack(self.ringe + [self.pol_a[None], self.pol_e[None]])
        pa, pe = m * n, m * n + 1
        F, G = [], []
        for i in range(m - 1):
            for k in range(n):
                k2 = (k + 1) % n
                F.append((i * n + k, i * n + k2, (i + 1) * n + k2, (i + 1) * n + k))
                G.append((i, k))
        for k in range(n):
            k2 = (k + 1) % n
            if not self.offen_a:
                F.append((pa, k2, k))
                G.append((-1, k))
            if not self.offen_e:
                F.append((pe, (m - 1) * n + k, (m - 1) * n + k2))
                G.append((m - 1, k))
        return V, F, G


def flaechen_normalen(V, F):
    N = []
    for f in F:
        p = V[list(f)]
        if len(f) == 3:
            c = np.cross(p[1] - p[0], p[2] - p[0])
        else:
            c = np.cross(p[2] - p[0], p[3] - p[1])
        N.append(c)
    return np.array(N)


def aussen_drehen(V, F, G, t):
    """Flaechen so drehen, dass die Normale von der Ringmitte weg zeigt. Rueckgabe F neu."""
    n = t.n
    m = len(t.ringe)
    mitte = [r.mean(0) for r in t.ringe]
    Fn = []
    FN = flaechen_normalen(V, F)
    d = 0.0
    for f, g, c in zip(F, G, FN):
        i = g[0]
        q = mitte[min(max(i, 0), m - 1)]
        if i >= 0 and i + 1 < m:
            q = 0.5 * (mitte[i] + mitte[i + 1])
        d += float(c @ (V[list(f)].mean(0) - q))
    if d < 0:
        Fn = [tuple(reversed(f)) for f in F]
    else:
        Fn = list(F)
    return Fn


def ecken_normalen(V, F, knick=46.0):
    """Normale je Flaechenecke mit Knickwinkel: Flaechen, die weniger als `knick` Grad abweichen, werden an der Ecke gemittelt. Harte Kanten bleiben hart.
    Rueckgabe: Liste je Flaeche von Normalen (len(f) x 3)."""
    FN = flaechen_normalen(V, F)
    lg = np.linalg.norm(FN, axis=1, keepdims=True)
    FE = FN / np.maximum(lg, 1e-9)
    an = {}
    for i, f in enumerate(F):
        for v in f:
            an.setdefault(v, []).append(i)
    c = math.cos(math.radians(knick))
    aus = []
    for i, f in enumerate(F):
        z = []
        for v in f:
            s = np.zeros(3)
            for j in an[v]:
                if FE[j] @ FE[i] >= c:
                    s += FN[j]
            if not np.any(s):
                s = FN[i]
            z.append(_norm(s))
        aus.append(np.array(z))
    return aus
