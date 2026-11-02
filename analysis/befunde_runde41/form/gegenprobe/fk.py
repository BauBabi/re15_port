#!/usr/bin/env python3
"""Unabhaengige Vorwaertsrechnung: liest nur gerippe.json und form_kamera, kein rig.py, kein fit.py."""
import json, math, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
F = os.path.normpath(os.path.join(HIER, '..'))
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_kamera as FK

def lade(pfad=None):
    return json.load(open(pfad or os.path.join(F, 'gerippe.json'), encoding='utf-8'))

def rx(a):
    s, c = math.sin(a), math.cos(a); return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])
def ry(a):
    s, c = math.sin(a), math.cos(a); return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
def rz(a):
    s, c = math.sin(a), math.cos(a); return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])
E = 2 * math.pi / 4096

def fk(g, pose):
    """R, T je Knochen in Welt. M = Rx Ry Rz, R = R_eltern lokal, T = R_eltern versatz + T_eltern, welt = Y(gier)(R v + T) + Lage."""
    p = g['posen'][pose]
    Y = ry(p['gier'] * E)
    lage = np.array(p['lage'], float)
    n = len(g['knochen'])
    Rl, Tl = [None] * n, [None] * n
    for k in g['knochen']:
        i = k['index']
        a = p['winkel'][i]
        M = rx(a[0] * E) @ ry(a[1] * E) @ rz(a[2] * E)
        if k['eltern'] < 0:
            Rl[i] = M
            Tl[i] = np.array(p['wurzel'], float)
        else:
            e = k['eltern']
            Rl[i] = Rl[e] @ M
            Tl[i] = Rl[e] @ np.array(k['versatz'], float) + Tl[e]
    R = np.array([Y @ r for r in Rl])
    T = np.array([Y @ t + lage for t in Tl])
    return R, T

def ende(g, R, T, i):
    k = g['knochen'][i]
    return T[i] + R[i] @ np.array(k['achse_bis'], float)
def anfang(g, R, T, i):
    k = g['knochen'][i]
    return T[i] + R[i] @ np.array(k['achse_von'], float)

if __name__ == '__main__':
    g = lade()
    for nr in (1, 2):
        kam = FK.lade_kamera(nr)
        R, T = fk(g, 'bild%d' % nr)
        print('=== Bild', nr, 'det(R) alle', set(np.round(np.linalg.det(R), 3)))
        for k in g['knochen']:
            i = k['index']
            a, b = anfang(g, R, T, i), ende(g, R, T, i)
            qa, qb = FK.projiziere(kam, a), FK.projiziere(kam, b)
            print('%-16s Anf Bild (%4.0f,%4.0f) Hoehe %5.0f Tiefe %5.0f | Ende (%4.0f,%4.0f) Hoehe %5.0f Tiefe %5.0f' % (k['name'], qa[0], qa[1], -a[1], FK.tiefe(kam, a), qb[0], qb[1], -b[1], FK.tiefe(kam, b)))
