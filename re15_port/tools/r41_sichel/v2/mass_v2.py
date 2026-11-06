"""Spur SF Schwanz V2: Masse des Schwanzes im Bild der Spielkamera, in H (Formblatt V2 S2-S10).

H = Umriss-Oberkante (Koerper ohne Schwanz) bis Sohle im selben Bild. Wurzel = Gelenk 16 projiziert. Mittellinie = Gelenke
16-22 + Spitze projiziert. Dicke = Breite der Schwanzmaske (nur Knochen 16-22 gezeichnet, ohne Verdeckung) senkrecht zur
Mittellinie bei 0,2/0,5/0,9 der Bildlaenge. Spitze = projizierter Endpunkt; ueber der Sohle in H.
Alle Videokoordinaten 640x480.
"""
import os, sys
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import birkin_buehne as BB


def schwanz_mass(mo, pose, R_T=None, versenkt=0.0):
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    koerper = b2.umriss_schnell(mo, pose, knochen=range(16), R_T=R_T)
    ys, xs = np.nonzero(koerper)
    ok, sohle = ys.min(), ys.max()
    H = float(sohle - ok)
    x_top = xs[ys <= ok + 2].mean()
    if mo.skel.bone_count < 23:
        return dict(H=H, ok=ok, sohle=sohle)
    P3 = b2.kette_punkte(mo, kf, R_T)
    if versenkt > 0:
        # Schwanz V2: Gelenk 16 liegt `versenkt` E. im Buckel; die sichtbare Wurzelmitte ist der Punkt in dieser Bogenlaenge
        L3 = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P3, axis=0), axis=1))]
        i = max(0, min(np.searchsorted(L3, versenkt) - 1, len(P3) - 2))
        a = (versenkt - L3[i]) / max(L3[i + 1] - L3[i], 1e-9)
        P3 = np.vstack([P3[i] + a * (P3[i + 1] - P3[i]), P3[i + 1:]])
    P = b2.projiziere(pose, P3)
    seg = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    Lb = seg[-1]
    _im, ids = b2.zeichne(mo, pose, None, R_T=R_T, knochen=range(16, 23), licht=False)
    sm = ids >= 0
    dicke = {}
    for f in (0.2, 0.5, 0.9):
        t = f * Lb
        i = min(np.searchsorted(seg, t) - 1, len(P) - 2)
        i = max(i, 0)
        a = (t - seg[i]) / max(seg[i + 1] - seg[i], 1e-9)
        p = P[i] + a * (P[i + 1] - P[i])
        d = P[i + 1] - P[i]; d /= max(np.linalg.norm(d), 1e-9)
        n = np.array([-d[1], d[0]])
        w = 0
        for sgn in (1, -1):
            for k in range(1, 80):
                q = np.round(p + sgn * n * k).astype(int)
                if not (0 <= q[0] < 640 and 0 <= q[1] < 480) or not sm[q[1], q[0]]:
                    w += k - 1
                    break
        dicke[f] = (w + 1) / H
    sicht = (ids >= 0)
    _im2, ids_alle = b2.zeichne(mo, pose, None, R_T=R_T, licht=False)
    sichtbar = int(((ids_alle >= 16) & (ids_alle <= 22)).sum())
    return dict(H=H, ok=int(ok), sohle=int(sohle), x_top=float(x_top),
                wurzel_unter_ok=(P[0][1] - ok) / H, wurzel_hinter=(x_top - P[0][0]) / H,
                spitze_ueber_sohle=(sohle - P[-1][1]) / H, laenge_bild=Lb / H, dicke=dicke,
                mittellinie=P, sichtbar_px=sichtbar, schwanz_px=int(sicht.sum()))


def text(m):
    if 'dicke' not in m:
        return ['H %.0f px (kein Schwanz)' % m['H']]
    return ['Wurzel %.3f H unter OK, %.3f H hinter Buckel' % (m['wurzel_unter_ok'], m['wurzel_hinter']),
            'Dicke 0,2/0,5/0,9: %.3f / %.3f / %.3f H' % (m['dicke'][0.2], m['dicke'][0.5], m['dicke'][0.9]),
            'Spitze %.3f H ueber Sohle, Bildlaenge %.2f H' % (m['spitze_ueber_sohle'], m['laenge_bild'])]


if __name__ == '__main__':
    import json
    mo = b2.modell(sys.argv[1])
    P = json.load(open(sys.argv[2]))
    for h, pose in P.items():
        m = schwanz_mass(mo, pose)
        print('h%s H %.0f' % (h, m['H']), ' | '.join(text(m)))
