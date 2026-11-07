# -*- coding: utf-8 -*-
"""Rohbau B: Pose nachstellen. Bildziele + Boden, kleinste Quadrate ueber die Winkel gewaehlter Knochen. Ergebnis landet in posen.json (dokumentiert im Dossier)."""
import json, math, os, sys
import numpy as np
from scipy.optimize import least_squares
import figur, schau
from schau import FK

HIER = os.path.dirname(os.path.abspath(__file__))
E = schau.E


class Aufgabe:
    def __init__(self, f, fig, pose_name, kam_nr):
        self.f, self.fig, self.pn = f, fig, pose_name
        self.p = fig['posen'][pose_name]
        self.kam = FK.lade_kamera(kam_nr)
        self.winkel = [list(w) for w in self.p['winkel']]
        self.lage = np.array(self.p['lage'], float)
        self.Y = schau.ry(self.p['gier'] * E)
        self.wurzel = list(self.p['wurzel'])

    def welt(self, winkel, b, lok):
        R, T = schau.fk(self.f.eltern, self.f.versatz, self.wurzel, winkel)
        return self.Y @ (R[b] @ np.asarray(lok, float) + T[b]) + self.lage

    def loese(self, frei, ziele, boden=(), hoehe=(), reg=.02, achsen=None, grenzen=None):
        """frei: Knochennamen. ziele: (Knochen, lokaler Punkt, (x, y), Gewicht). boden: (Knochen, lokaler Punkt, Mindesthoehe). hoehe: (Knochen, Punkt, Hoehe, Gewicht).
        achsen: {Knochen: (1,1,1)} schaltet Achsen frei. reg: Gewicht je Grad Abweichung."""
        ids = [self.f.ix[n] for n in frei]
        ach = []
        for n, i in zip(frei, ids):
            a = (achsen or {}).get(n, (1, 1, 1))
            for j in range(3):
                if a[j]:
                    ach.append((i, j))
        x0 = np.array([self.winkel[i][j] * E for i, j in ach])

        def setz(x):
            w = [list(v) for v in self.winkel]
            for (i, j), v in zip(ach, x):
                w[i][j] = v / E
            return w

        def res(x):
            w = setz(x)
            R, T = schau.fk(self.f.eltern, self.f.versatz, self.wurzel, w)
            r = []
            for kn, lok, ziel, gew in ziele:
                b = self.f.ix[kn]
                q = self.Y @ (R[b] @ np.asarray(lok, float) + T[b]) + self.lage
                u = FK.projiziere(self.kam, q)
                r += [(u[0] - ziel[0]) * gew, (u[1] - ziel[1]) * gew]
            for kn, lok, h in boden:
                b = self.f.ix[kn]
                y = (R[b] @ np.asarray(lok, float) + T[b])[1]
                r.append(max(0.0, y + h) * .2)      # y unten: Punkt muss mindestens h ueber dem Boden liegen
            for kn, lok, h, gew in hoehe:
                b = self.f.ix[kn]
                y = (R[b] @ np.asarray(lok, float) + T[b])[1]
                r.append((y + h) * .05 * gew)
            r += list((x - x0) / math.radians(1.0) * reg)
            return np.array(r)
        lo = np.full(len(x0), -np.inf)
        hi = np.full(len(x0), np.inf)
        if grenzen:
            nam = {i: n for n, i in zip(frei, ids)}
            for j, (i, a) in enumerate(ach):
                g = grenzen.get(nam[i])
                if g is not None:
                    lo[j], hi[j] = math.radians(g[a][0]), math.radians(g[a][1])
                    x0[j] = min(max(x0[j], lo[j] + 1e-3), hi[j] - 1e-3)
        s = least_squares(res, x0, method='trf', x_scale=.2, diff_step=1e-3, max_nfev=300, bounds=(lo, hi))
        self.winkel = [[int(round(c)) for c in v] for v in setz(s.x)]
        rest = []
        R, T = schau.fk(self.f.eltern, self.f.versatz, self.wurzel, self.winkel)
        for kn, lok, ziel, gew in ziele:
            b = self.f.ix[kn]
            q = self.Y @ (R[b] @ np.asarray(lok, float) + T[b]) + self.lage
            u = FK.projiziere(self.kam, q)
            rest.append((kn, tuple(np.round(u).astype(int)), ziel, int(-q[1])))
        return rest


def speichern(aufg, namen):
    pf = os.path.join(HIER, 'posen.json')
    d = json.load(open(pf, encoding='utf-8')) if os.path.exists(pf) else {}
    e = d.setdefault(aufg.pn, {}).setdefault('winkel', {})
    for n in namen:
        i = aufg.f.ix[n]
        e[n] = [int(c) for c in aufg.winkel[i]]
    json.dump(d, open(pf, 'w', encoding='utf-8'), indent=0)
