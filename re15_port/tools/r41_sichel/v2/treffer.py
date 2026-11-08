"""Spur SF Schwanz V2: Bildpunkt -> Oberflaechenpunkt eines Meshes (Mesh-lokal) bei gegebener Pose (Strahl gegen die Dreiecke).

Dient zur Lage der KI-Details (Nutzer-Entscheid F4 2026-11-06): das Schulterauge der KI (birkin19 h118, 1280er (705-715,
360-372)) wird auf die Schulter des Modells in derselben Pose (anpassen_v2.py, h118) zurueckgerechnet.
python treffer.py <emd> <posen.json> <h> <mesh> <x1280> <y1280>
"""
import sys, os, json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA


def treffer(mo, pose, mesh, px, py):
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    R, T = mo.pose(kf)
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    Rb, Tb = R[mesh].astype(float) / 4096.0, T[mesh].astype(float)
    m = mo.meshes[mesh]
    best = None
    for verts, faces in ((m.tv, m.tf), (m.qv, m.qf)):
        if not faces:
            continue
        L = np.array([v[:3] for v in verts], float)
        Wm = (L @ Rb.T + Tb) @ Y.T + np.array([pose['x'], pose.get('y', 0), pose['z']], float)
        Vc = Wm @ (kam.V.astype(float) / 4096.0).T + kam.t.astype(float)
        sx = BB.OFX + Vc[:, 0] * kam.h / Vc[:, 2]
        sy = BB.OFY + Vc[:, 1] * kam.h / Vc[:, 2]
        X, Yv = BB.abbilden(sx + BB.PHASE, sy + BB.PHASE, M)
        for f in faces:
            ie = list(f[1::2])
            tris = [ie] if len(ie) == 3 else [[ie[0], ie[1], ie[3]], [ie[0], ie[3], ie[2]]]
            for t in tris:
                x = X[t]; y = Yv[t]
                d = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
                if abs(d) < 1e-9:
                    continue
                w1 = ((px - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (py - y[0])) / d
                w2 = ((x[1] - x[0]) * (py - y[0]) - (px - x[0]) * (y[1] - y[0])) / d
                w0 = 1 - w1 - w2
                if min(w0, w1, w2) < 0:
                    continue
                z = w0 * Vc[t[0], 2] + w1 * Vc[t[1], 2] + w2 * Vc[t[2], 2]
                if best is None or z < best[0]:
                    p = w0 * L[t[0]] + w1 * L[t[1]] + w2 * L[t[2]]
                    n = np.cross(L[t[1]] - L[t[0]], L[t[2]] - L[t[0]])
                    n = n / max(np.linalg.norm(n), 1e-9)
                    # Normale nach aussen: weg vom Mesh-Schwerpunkt
                    if n @ (p - L.mean(0)) < 0:
                        n = -n
                    best = (z, p, n)
    return best


if __name__ == '__main__':
    emd, posen, h, mesh, x, y = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), float(sys.argv[5]), float(sys.argv[6])
    mo = b2.modell(emd)
    pose = json.load(open(posen))[h]
    t = treffer(mo, pose, mesh, x / 2.0, y / 2.0)
    if t is None:
        print('kein Treffer')
    else:
        print('Mesh %d lokal (%.0f, %.0f, %.0f) Normale (%.2f, %.2f, %.2f) Tiefe %.0f' % ((mesh,) + tuple(t[1]) + tuple(t[2]) + (t[0],)))
