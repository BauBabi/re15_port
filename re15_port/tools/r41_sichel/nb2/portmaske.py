"""Runde 41 SF NB2: Umriss des Port-Birkin in Videokoordinaten (640x480) je Szenenbild h, offline an der Buehne.

Pose je h aus der Messzeile "[hangar-takt] hN F.. B clip C bild B gier G (x,z)" eines Laufs (szene_lauf.sh, RE15_HANGAR_LOG=1);
Zeichenweg tools/birkin_buehne.py (RID-Kamera ROOM50A0 Schnitt 12, Abbildung SF_belege/kamera_50A0_12.json).
Die Ueberblendung des Ports (anim_frac) zeichnet die Buehne nicht; sie zeigt das Keyframe des Clip-Bilds.
"""
import os, re, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', '..'))
import birkin_buehne as BB      # noqa: E402

_RAUM = {}


def posen(log):
    t = open(log, encoding='latin-1').read()
    out = {}
    for m in re.finditer(r'\[hangar-takt\] h(-?\d+) F\d+ B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)', t):
        h, c, b, g, x, z = map(int, m.groups())
        out[h] = (c, b, g, x, z)
    return out


def kamera(raum='50A0', schnitt=12):
    k = (raum, schnitt)
    if k not in _RAUM:
        r = BB.Raum(raum); s = r.schnitt(schnitt)
        _RAUM[k] = (r, BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7), BB.abbildung_laden(raum, schnitt), r.licht(schnitt))
    return _RAUM[k]


def maske(mo, clip, bild, gier, x, z, y=0, raum='50A0', schnitt=12, hintergrund=None, licht=True, nur=None):
    """mo = BB.modell(blob). nur = Menge der zu zeichnenden Knochen (None = alle). Rueckgabe (Bild, Maske)."""
    r, kam, M, li = kamera(raum, schnitt)
    if nur is not None:
        import copy
        mo = copy.copy(mo); mo.meshes = [m if b in nur else _leer(m) for b, m in enumerate(mo.meshes)]
    ziel = np.array(hintergrund, copy=True) if hintergrund is not None else np.zeros((480, 640, 3), np.uint8)
    tris = BB.dreiecke(mo, BB.kf_von_clip(mo, clip, bild), (x, y, z), gier, kam, li if licht else None)
    return ziel, BB.rastern(tris, mo, ziel, M)


def _leer(m):
    import copy
    m2 = copy.copy(m); m2.tf, m2.qf = [], []
    return m2


def gelenke(mo, clip, bild, gier, x, z, y=0, raum='50A0', schnitt=12):
    """Ursprung jedes Knochens in Videokoordinaten (640x480) und Welt: {b: (vx, vy, welt)}."""
    r, kam, M, _li = kamera(raum, schnitt)
    s, c = BB.EA.sin_q12(gier), BB.EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], dtype=np.int64)
    R, T = mo.pose(BB.kf_von_clip(mo, clip, bild))
    out = {}
    for b in range(len(T)):
        w = ((Y @ np.array(T[b], dtype=np.int64)) >> 12) + np.array([x, y, z], dtype=np.int64)
        v = ((kam.V @ w) >> 12) + kam.t
        sx, sy = BB._gte(np.array([v[0]]), np.array([v[1]]), np.array([v[2]]), kam.h)
        vx, vy = BB.abbilden(float(sx[0]) + BB.PHASE, float(sy[0]) + BB.PHASE, M)
        out[b] = (round(vx, 1), round(vy, 1), w.tolist())
    return out
