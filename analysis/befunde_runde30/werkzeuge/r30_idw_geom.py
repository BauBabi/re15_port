# -*- coding: utf-8 -*-
"""Runde 30 / Thema irons-diary-welt: Kamera-Geometrie fuer ROOM1150.

Die Sichtmatrizen werden NICHT nachgebaut, sondern aus der Ausgabe der Engine-Sonde
(probe_r30_irons-diary-welt kamera -> build/r30_irons-diary-welt/kamera_1150.txt)
gelesen. Damit rechnet dieses Werkzeug mit denselben Q12-Werten wie der Port
(re15_camera_build_view = FUN_80053ca4 @0x80053ca4, H = fov >> 7).
"""
import re, os
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, '..', '..', '..'))
# R30_IDW_AUS: Messordner eines anderen Baums (die Messdateien liegen unversioniert im
# Hauptbaum; aus einem Arbeitsbaum heraus dorthin zeigen).
AUS  = os.environ.get('R30_IDW_AUS') or os.path.join(REPO, 'build', 'r30_irons-diary-welt')

def lade_kameras(pfad=None):
    pfad = pfad or os.path.join(AUS, 'kamera_1150.txt')
    cams = {}
    for z in open(pfad):
        m = re.match(r'VIEW (\d+) R= (.*?) T= (.*?) H= (\d+)', z)
        if m:
            c = int(m.group(1))
            R = np.array([int(v) for v in m.group(2).split()], float).reshape(3, 3) / 4096.0
            T = np.array([int(v) for v in m.group(3).split()], float)
            cams.setdefault(c, {}).update(dict(R=R, T=T, H=float(m.group(4)), Ri=np.array([int(v) for v in m.group(2).split()]).reshape(3,3)))
        m = re.match(r'CUT (\d+) .*pos=(-?\d+) (-?\d+) (-?\d+) tgt=(-?\d+) (-?\d+) (-?\d+)', z)
        if m:
            c = int(m.group(1))
            cams.setdefault(c, {})
            cams[c]['pos'] = np.array([int(m.group(i)) for i in (2, 3, 4)], float)
            cams[c]['tgt'] = np.array([int(m.group(i)) for i in (5, 6, 7)], float)
    return cams

def projiziere(cam, p):
    """Weltpunkt -> (sx, sy, vz). Ganzzahlig wie die Sonde (>>12 je Zeile)."""
    p = np.asarray(p, float)
    Ri = cam['Ri']
    v = np.floor((Ri @ p) / 4096.0) + cam['T']
    if v[2] <= 0:
        return None
    return 160.0 + cam['H'] * v[0] / v[2], 120.0 + cam['H'] * v[1] / v[2], v[2]

def strahl(cam, sx, sy):
    """Bildpunkt -> (Ursprung, Richtung) in Weltkoordinaten. Richtung mit vz=1."""
    # ACHTUNG: die Engine-Matrix ist wegen SquareRoot0 (BIOS-Tabelle) NICHT exakt
    # orthonormal (z.B. Cut 0 r0=4098, Cut 4 r2=-4104). R^T ist deshalb NICHT die
    # Inverse - der Ursprung laege bis ~190 Einheiten neben der Kameraposition.
    # Richtig ist die echte Inverse der Vorwaertsabbildung v = R p + T.
    R, T, H = cam['R'], cam['T'], cam['H']
    Rinv = np.linalg.inv(R)
    d = np.array([(sx - 160.0) / H, (sy - 120.0) / H, 1.0])
    wd = Rinv @ d
    wo = Rinv @ (-T)
    return wo, wd

def auf_ebene(cam, sx, sy, y):
    wo, wd = strahl(cam, sx, sy)
    s = (y - wo[1]) / wd[1]
    return wo + s * wd, s

def trianguliere(strahlen):
    """Kleinste-Quadrate-Schnittpunkt mehrerer Strahlen (o, d). Liefert Punkt + Restabstaende."""
    A = np.zeros((3, 3)); b = np.zeros(3)
    for o, d in strahlen:
        d = d / np.linalg.norm(d)
        P = np.eye(3) - np.outer(d, d)
        A += P; b += P @ o
    x = np.linalg.solve(A, b)
    rest = []
    for o, d in strahlen:
        d = d / np.linalg.norm(d)
        rest.append(np.linalg.norm((np.eye(3) - np.outer(d, d)) @ (x - o)))
    return x, rest
