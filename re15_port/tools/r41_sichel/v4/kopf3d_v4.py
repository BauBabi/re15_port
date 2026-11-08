"""SF V4 (Bauer): Bildpunkt einer Vorlage-Kamera -> Punkt auf Mesh 9 (Kopfraum), Strahl gegen die Dreiecke der Pose."""
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G
import birkin_formen as BF
import birkin_buehne as BB


def strahl(pose, xy):
    """Kameraort und Richtung (Modellraum der Pose) durch den Videopunkt xy."""
    c = BF.v4_kamerapunkt(pose)
    p = BF.v4_rueck(pose, xy, -3000.0)
    d = p - c
    return c, d / np.linalg.norm(d)


def treffer(mo, pose, xy, knochen=9):
    """Erster Schnitt des Strahls mit Mesh `knochen` (Pose) -> (Punkt Modellraum, Punkt Knochenraum) oder None."""
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    V, N, F = mo.geometrie(kf)
    R, T = mo.pose(kf)
    o, d = strahl(pose, xy)
    best = None
    for f in F:
        if f['bone'] != knochen:
            continue
        a, b, c = (V[i] for i in f['v'])
        e1, e2 = b - a, c - a
        h = np.cross(d, e2); det = e1 @ h
        if abs(det) < 1e-9:
            continue
        s = o - a; u = (s @ h) / det
        q = np.cross(s, e1); v = (d @ q) / det; t = (e2 @ q) / det
        if u >= 0 and v >= 0 and u + v <= 1 and t > 0 and (best is None or t < best[0]):
            best = (t, o + t * d)
    if best is None:
        return None
    p = best[1]
    Rk, Tk = R[knochen].astype(float) / 4096.0, T[knochen].astype(float)
    return p, Rk.T @ (p - Tk)
