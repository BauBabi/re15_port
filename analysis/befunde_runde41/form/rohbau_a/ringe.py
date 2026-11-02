#!/usr/bin/env python3
"""Ring-Bauweise: ein Teil ist eine Folge von Ringen entlang einer Achse. Je Ring Mitte, Ebene, n Ecken mit eigenem Radius. Vierecke zwischen den Ringen.
Kein Skinning: ein Teil gehoert einem Knochen. Enden: 'flach' (Quads ueber den Ring, nur wo verdeckt) oder 'spitz' (Faecher auf eine Spitze)."""
import math
import numpy as np

RAHMEN = {'x': ((1, 0, 0), (0, -1, 0), (0, 0, -1)),      # Achse +X, 0 Grad oben (-Y), 90 Grad rechts (-Z)
          'mx': ((-1, 0, 0), (0, -1, 0), (0, 0, -1)),    # Achse -X (Schwanz), 0 Grad oben, 90 Grad rechts
          'y': ((0, 1, 0), (1, 0, 0), (0, 0, -1))}       # Achse +Y (Glieder), 0 Grad vorn (+X), 90 Grad aussen (-Z am rechten Glied)


def wd(a, b):
    """Kleinster Winkelabstand in Grad, Vorzeichen."""
    return (a - b + 180.0) % 360.0 - 180.0


def super_r(th, o, r, u, l, p):
    t = math.radians(th)
    s, c = math.sin(t), math.cos(t)
    a = o if c >= 0 else u
    b = r if s >= 0 else l
    return 1.0 / ((abs(c) / a) ** p + (abs(s) / b) ** p) ** (1.0 / p)


class Teil:
    """st: Stationen. Tupel (t, o, r, u, l) mit optional p, cu, cv, sonst dict. Ein Eintrag 'dreh' dreht die Ecken des Rings. 'k' = {Ecke: Faktor}.
    cu, cv: Versatz der Mitte in Richtung 0 Grad und 90 Grad. ko, kr: Kippung des Rings (Grad) um die Achsen quer dazu.
    relief: Liste (t, th, st, sth, amp): Gauss-Beule, Faktor auf den Radius. grat: (th, breite_grad, amp, t0, t1): Laengsgrat."""

    def __init__(self, name, knochen, rahmen, n, st, relief=(), grat=(), spiegel=False, dreh=0.0, anf='flach', ende='flach', spitze=None, anf_spitze=None,
                 ursprung=(0, 0, 0), achse=None, oben=None, mat='haut', naht=180.0, dichte=1.0, kante=None):
        self.name, self.knochen, self.n = name, knochen, n
        self.spiegel, self.anf, self.ende = spiegel, anf, ende
        self.mat, self.dichte = mat, dichte
        self.naht = naht
        self.kante = kante          # Winkelgrenze fuer harte Normalen, None = Vorgabe
        if achse is not None:
            a = np.array(achse, float)
            a /= np.linalg.norm(a)
            u = np.array(oben if oben is not None else (0, -1, 0), float)
            u = u - a * (u @ a)
            u /= np.linalg.norm(u)
            w = np.cross(a, u)      # rechts: fuer Achse +X, oben -Y ergibt das -Z wie im Rahmen 'x'
            self.ea, self.eo, self.er = a, u, w
        else:
            e = RAHMEN[rahmen]
            self.ea, self.eo, self.er = [np.array(v, float) for v in e]
        self.u0 = np.array(ursprung, float)
        ringe = []
        for s in st:
            if not isinstance(s, dict):
                s = dict(zip(('t', 'o', 'r', 'u', 'l', 'p', 'cu', 'cv'), s))
            ringe.append(s)
        self.st = ringe
        self.relief, self.grat = list(relief), list(grat)
        self.dreh = dreh
        self.spitze = spitze
        self.anf_spitze = anf_spitze
        self.baue()

    def baue(self):
        n = self.n
        nr = len(self.st)
        self.M = np.zeros((nr, 3))              # Mitte
        self.D = np.zeros((nr, n, 3))           # Richtung je Ecke (nicht Einheit, wenn gekippt)
        self.R = np.zeros((nr, n))              # Radius
        self.TH = np.zeros((nr, n))             # Winkel je Ecke
        self.T = np.zeros(nr)
        for i, s in enumerate(self.st):
            t = s['t']
            dreh = self.dreh + s.get('dreh', 0.0)
            c = self.u0 + self.ea * t + self.eo * s.get('cu', 0.0) + self.er * s.get('cv', 0.0)
            self.M[i] = c
            self.T[i] = t
            for j in range(n):
                th = dreh + 360.0 * j / n
                th0 = (th) % 360.0
                r = super_r(th0, s['o'], s['r'], s['u'], s['l'], s.get('p', 2.0))
                f = 1.0
                for (t0, a0, st, sa, amp) in self.relief:
                    f += amp * math.exp(-((t - t0) / st) ** 2 - (wd(th0, a0) / sa) ** 2)
                for (a0, br, amp, t0, t1) in self.grat:
                    if t0 <= t <= t1:
                        f += amp * math.exp(-(wd(th0, a0) / br) ** 2)
                f *= s.get('k', {}).get(j, 1.0) * s.get('f', 1.0)
                b = math.radians(th0)
                d = self.eo * math.cos(b) + self.er * math.sin(b)
                d = d + self.ea * (math.tan(math.radians(s.get('ko', 0.0))) * math.cos(b) + math.tan(math.radians(s.get('kr', 0.0))) * math.sin(b))
                self.D[i, j] = d
                self.R[i, j] = r * f
                self.TH[i, j] = th0

    def punkte(self, q=None):
        """Ecken [nr, n, 3]. q: log-Korrektur des Radius je Ecke."""
        r = self.R if q is None else self.R * np.exp(q)
        p = self.M[:, None, :] + self.D * r[:, :, None]
        return p

    def tip(self):
        """Spitzen als Punkte (lokal, gespiegelt erst in spiegeln())."""
        a = None if self.anf != 'spitz' else (self.u0 + self.ea * self.anf_spitze[0] + self.eo * self.anf_spitze[1] + self.er * self.anf_spitze[2])
        e = None if self.ende != 'spitz' else (self.u0 + self.ea * self.spitze[0] + self.eo * self.spitze[1] + self.er * self.spitze[2])
        return a, e


def hohl_quads(n, ring, mitte, umkehr=False):
    """Ring schliessen ueber eine Mitte: n/2 Vierecke (a, a+1, a+2, Mitte), bei ungeradem n ein Dreieck dazu."""
    aus = []
    j = 0
    while j + 2 <= n:
        f = [ring[j], ring[j + 1], ring[(j + 2) % n], mitte]
        aus.append(f[::-1] if umkehr else f)
        j += 2
    if j < n:
        f = [ring[j], ring[0], mitte]
        aus.append(f[::-1] if umkehr else f)
    return aus
