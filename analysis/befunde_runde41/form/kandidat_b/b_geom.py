#!/usr/bin/env python3
"""Kandidat B: gemeinsame Geometrie. Lokalraum = Modellraum der Pose 1 (Vorderseite +X = Ende N, +Z = Seite R, Y nach unten)."""
import json, math, os, sys
import numpy as np
BAUM = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
FORM = BAUM + '/analysis/befunde_runde41/form'
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_kamera as FK

K1, K2 = FK.lade_kamera(1), FK.lade_kamera(2)
O1 = np.array([-18514.0, 0.0, -18726.0])
G1 = 2145

def Y(g):
    a = g * 2 * math.pi / 4096
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])

def welt(p, lage=O1, g=G1):
    return (Y(g) @ np.asarray(p, float).T).T + lage

def lokal(w, lage=O1, g=G1):
    return (Y(g).T @ (np.asarray(w, float) - lage).T).T

def bild(p, kam=K1, lage=O1, g=G1):
    return FK.projiziere(kam, welt(p, lage, g))

def ray_lokal(x, y, kam=K1, lage=O1, g=G1):
    e, d = FK.strahl(kam, np.array([x, y], float))
    return lokal(e, lage, g), Y(g).T @ d

def bei_tiefe(x, y, t, kam=K1, lage=O1, g=G1):
    e, d = FK.strahl(kam, np.array([x, y], float))
    s = t / (d @ kam['R'][2])
    return lokal(e + s * d, lage, g)

def in_ebene_z(x, y, z, kam=K1, lage=O1, g=G1):
    e, d = ray_lokal(x, y, kam, lage, g)
    return e + d * (z - e[2]) / d[2]

def in_ebene_x(x, y, xx, kam=K1, lage=O1, g=G1):
    e, d = ray_lokal(x, y, kam, lage, g)
    return e + d * (xx - e[0]) / d[0]

def px_je_1000(t, kam=K1):
    return kam['brennweite_bild_px']['fx'] / t * 1000
