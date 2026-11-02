"""Runde 41 SF NB2: schnelle Umriss-Maske (ohne Textur/Licht) fuer die Haltungs-Anpassung: alle Dreiecke der Knochen
0..15 (ohne Schwanz) bzw. aller Knochen, Projektion wie birkin_buehne (RID-Kamera ROOM50A0 Schnitt 12, GTE H/z, Abbildung
SF_belege/kamera_50A0_12.json), gefuellt mit cv2.fillPoly. Nur Werkzeug der Nachbildung."""
import os, sys
import numpy as np
import cv2
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import portmaske as PM                 # noqa: E402
import birkin_buehne as BB             # noqa: E402


class Schablone:
    def __init__(self, blob, knochen=None):
        self.mo = BB.modell(blob)
        nb = min(self.mo.skel.bone_count, len(self.mo.meshes))
        self.kn = [b for b in range(nb) if knochen is None or b in knochen]
        self.teile = []
        for b in self.kn:
            m = self.mo.meshes[b]
            tris = []
            for verts, faces, n in ((m.tv, m.tf, 3), (m.qv, m.qf, 4)):
                if not faces:
                    continue
                V = np.array([v[:3] for v in verts], float)
                for f in faces:
                    ie = f[1::2]
                    if n == 3:
                        tris.append(V[list(ie)])
                    else:
                        tris.append(V[[ie[0], ie[1], ie[3]]]); tris.append(V[[ie[0], ie[3], ie[2]]])
            if tris:
                self.teile.append((b, np.array(tris)))

    def maske(self, clip, bild, gier, x, z, y=0, raum='50A0', schnitt=12, kf=None):
        _r, kam, M, _li = PM.kamera(raum, schnitt)
        R, T = self.mo.pose(BB.kf_von_clip(self.mo, clip, bild) if kf is None else kf)
        s, c = np.sin(gier * 2 * np.pi / 4096), np.cos(gier * 2 * np.pi / 4096)
        Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
        V = kam.V.astype(float) / 4096.0
        t = kam.t.astype(float)
        out = np.zeros((480, 640), np.uint8)
        polys = []
        for b, tris in self.teile:
            W = Y @ (R[b].astype(float) / 4096.0)
            p = tris @ W.T + (Y @ T[b].astype(float)) + np.array([x, y, z], float)
            q = p @ V.T + t
            ok = (q[..., 2] > 64).all(1)
            q = q[ok]
            sx = 160 + q[..., 0] * kam.h / q[..., 2]; sy = 120 + q[..., 1] * kam.h / q[..., 2]
            X, Yv = sx + BB.PHASE, sy + BB.PHASE
            w = M[2, 0] * X + M[2, 1] * Yv + M[2, 2]
            vx = (M[0, 0] * X + M[0, 1] * Yv + M[0, 2]) / w; vy = (M[1, 0] * X + M[1, 1] * Yv + M[1, 2]) / w
            polys.extend(np.round(np.stack([vx, vy], -1) * 4).astype(np.int32))
        for q in polys:                       # einzeln: fillPoly mit mehreren Konturen hebt Ueberlappungen auf
            cv2.fillConvexPoly(out, q, 1, lineType=cv2.LINE_8, shift=2)
        return out.astype(bool)
