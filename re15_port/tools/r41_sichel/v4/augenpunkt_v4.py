"""SF V4 (Bauer): ein gemeinsamer Augenpunkt auf Mesh 9 fuer beide Vorlage-Kameras.

Die Riegel K4 (vorn: 0,035-0,065 H (B)-links der Kopfmitte, 0,07-0,12 H unter der Oberkante) und S2 (seitlich: 0,03-0,07 H hinter der
Vorderkante, 0,76-0,82 H ueber der Sohle) beschreiben DASSELBE Auge (Messblatt V4 Abschnitt 3: das sichtbare Auge ist das (W)-rechte).
Gesucht: der Punkt der Kopfoberflaeche (dichte Abtastung der Dreiecke/Vierecke von Mesh 9, Kopfraum), der in beiden Kameras sichtbar ist
(erster Treffer des Sehstrahls, kopf3d_v4) und beide Riegel am besten erfuellt (Abstand zur Riegelmitte in Toleranzbreiten).
python augenpunkt_v4.py <EMD>  -> druckt die Kandidaten und den gewaehlten Punkt (Kopfraum) - Eingabe fuer eichen_v4 AUGE_3D."""
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2, birkin_formen as BF, birkin_buehne as BB, kopf3d_v4 as K


def flaechenpunkte(m, n=6):
    pts = []
    for f in m.tf:
        A = np.array([m.tv[f[1]][:3], m.tv[f[3]][:3], m.tv[f[5]][:3]], float)
        for u in np.linspace(0, 1, n):
            for v in np.linspace(0, 1 - u, max(2, int(n * (1 - u)))):
                pts.append(A[0] * (1 - u - v) + A[1] * u + A[2] * v)
    for f in m.qf:
        Q = np.array([m.qv[f[2 * k + 1]][:3] for k in range(4)], float)
        for u in np.linspace(0, 1, n):
            for v in np.linspace(0, 1, n):
                pts.append((Q[0] * (1 - u) + Q[1] * u) * (1 - v) + (Q[2] * (1 - u) + Q[3] * u) * v)
    return np.array(pts)


def proj(mo, pose, pl):
    R, T = mo.pose(BB.kf_von_clip(mo, pose['clip'], pose['bild']))
    return BF.v4_proj(pose, pl @ (R[9].astype(float) / 4096).T + T[9]), pl @ (R[9].astype(float) / 4096).T + T[9]


def suche(emd):
    P = G.posen(); mo = b2.modell(emd)
    def info(pose):
        img, ids, iv = G.zeichne(mo, pose, voll=True); ok, so, H = G.h_port(iv); return ids, G.kopfbox(ids), so, H
    idv, kbv, sov, Hv = info(P['135']); ids_s, kbs, sos, Hs = info(P['85'])
    pts = flaechenpunkte(mo.meshes[9])
    xv, wv = proj(mo, P['135'], pts); xs, ws = proj(mo, P['85'], pts)
    xm = 0.5 * (kbv[0] + kbv[2])
    k4 = np.stack([(xm - xv[:, 0]) / Hv, (xv[:, 1] - kbv[1]) / Hv], 1)
    s2 = np.stack([(kbs[2] - xs[:, 0]) / Hs, (sos - xs[:, 1]) / Hs], 1)
    ok = (k4[:, 0] >= 0.035) & (k4[:, 0] <= 0.065) & (k4[:, 1] >= 0.07) & (k4[:, 1] <= 0.12) & \
         (s2[:, 0] >= 0.03) & (s2[:, 0] <= 0.07) & (s2[:, 1] >= 0.76) & (s2[:, 1] <= 0.82)
    sc = np.abs(k4[:, 0] - 0.05) / 0.015 + np.abs(k4[:, 1] - 0.093) / 0.025 + np.abs(s2[:, 0] - 0.049) / 0.02 + np.abs(s2[:, 1] - 0.79) / 0.03
    erg = []
    for j in np.argsort(sc):
        if not ok[j]:
            continue
        sicht = True
        for pose, xy, w in ((P['135'], xv[j], wv[j]), (P['85'], xs[j], ws[j])):
            t = K.treffer(mo, pose, xy)
            if t is None or np.linalg.norm(t[0] - w) > 25.0:
                sicht = False
        erg.append((float(sc[j]), pts[j], k4[j], s2[j], sicht))
        if len(erg) >= 12:
            break
    return erg


if __name__ == '__main__':
    for sc, p, k4, s2, sicht in suche(sys.argv[1] if len(sys.argv) > 1 else G.EMD_V4):
        print('Kopfraum (%.0f, %.0f, %.0f)  K4 %.3f / %.3f  S2 %.3f / %.3f  Abstand %.2f  sichtbar %s' % (*p, *k4, *s2, sc, sicht))
