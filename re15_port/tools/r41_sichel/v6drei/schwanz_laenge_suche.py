"""SF V6 drei Punkte: Gliedlaengen 19-22 so, dass der Schwanz in P2 (Foto 2) an den Nutzerpunkten liegt (Bauer, 2026-11-08).

    python schwanz_laenge_suche.py [--emd EM30_V5f.EMD] [--aus schwanz_laengen_v6.json]

Keyframe-Winkel bleiben (Auftrag); nur Laengen. Die Gelenke liegen linear in den Laengen: T_k = T_16 + Summe R_i[:, 1] L_i (Pose P2 =
Clip 12 Bild 48), Spitze = T_22 + R_22[:, 1] (L_22 + 0,8 r_7) (Apex wie schwanz_form.DORN). Ziel (Messblatt 5 / f_schwanz.NUTZER):
Spitze an Ende (295,343); Beginn (357,243), Biegung (295,252) und Knick (290,308) innerhalb des Schwanzumrisses (Abstand zur Mittellinie
minus Radius in px); dazu moeglichst nahe an der Nutzerlinie (mittlerer Abstand der 7 Achspunkte) und kleine Aenderung (3 x Summe ln(s)^2).
Glieder 16-18 bleiben (Beginn, Wurzel, Uebergang Ruecken wie V5f). Nelder-Mead, Strafe 50 x (Abstand - Grenze)^2 ueber der Grenze, drei
Startwerte. Befund (gemessen): Die Glieder 20-22 zeigen in P2 nach Bild-links unten, nur Glied 19 nach rechts unten; die Foto-Spitze liegt
rechts unter dem Knick. Mit Laengen allein liegt die beste Loesung bei Spitze 5,8 px / Knick 5,8 px / Beginn 5,3 px (Mittellinien-Mass),
Glied 19 x 2,11, 20 x 0,67, 21 x 0,85, 22 x 2,51. Zum Vergleich mit freien Gliedern 16-18 (Lauf 2): 4,0 / 4,0 / 4,0 px, aber Glied 16 x 1,67
und 19 x 3,29 - verworfen (Wurzel/Ruecken wuerde sich aendern).
"""
import os, sys, json, argparse
import numpy as np
from scipy.optimize import minimize
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_gemein as DG          # noqa: E402
import kopf_orig_bau as KB        # noqa: E402
import schwanz_form as SF         # noqa: E402

ACHSE = [(357, 243), (325, 240), (295, 252), (291, 282), (290, 308), (289, 328), (295, 343)]   # Nutzerlinie (Messblatt 5, f 0..1)
GRENZE, STRAFE, REG = 4.0, 50.0, 3.0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', default=os.path.join(KB.AUS, 'EM30_V5f.EMD')); ap.add_argument('--aus')
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    mo = DG.V.modell(blob)
    _d, _, ms, _t = KB.teile(blob)
    rad = SF.radien_lesen(ms)
    pose = DG.V.posen()['P2']
    R, T = mo.pose(DG.BB.kf_von_clip(mo, pose['clip'], pose['bild']))
    Ys = [R[b].astype(float)[:, 1] / 4096.0 for b in DG.SCHWANZ]
    L0 = [abs(mo.skel.rel[b][1]) for b in range(17, 23)] + [DG.laenge_22(mo)]
    T16 = T[16].astype(float)

    def gelenke(L):
        P = [T16]
        for i in range(7):
            P.append(P[-1] + Ys[i] * L[i])
        P.append(P[7] + Ys[6] * SF.DORN * rad[7])
        return np.array(P)

    def rpx(p, r):
        q = DG.b2.projiziere(pose, np.vstack([p] + [p + r * e for e in np.eye(3)]))
        return max(np.linalg.norm(q[k] - q[0]) for k in (1, 2, 3))
    Rp = np.array([rpx(gelenke(L0)[j], rad[j]) for j in range(8)] + [0.0])

    def ev(S7):
        q = DG.b2.projiziere(pose, gelenke(np.array(L0) * S7))

        def dist(p, rr=True):
            p = np.asarray(p, float); best = 1e9
            for k in range(8):
                u, v = q[k], q[k + 1]; dd = v - u
                t = np.clip(np.dot(p - u, dd) / max(np.dot(dd, dd), 1e-9), 0, 1)
                best = min(best, np.linalg.norm(p - (u + t * dd)) - (((1 - t) * Rp[k] + t * Rp[k + 1]) if rr else 0))
            return best
        return dict(spitze=float(np.linalg.norm(q[8] - DG.NUTZER2['Ende'])), knick=dist(DG.NUTZER2['Knick']), beginn=dist(DG.NUTZER2['Beginn']),
                    biegung=dist(DG.NUTZER2['Biegung']), achse=float(np.mean([dist(p, False) for p in ACHSE])), q=q)

    def kosten(x):
        S7 = np.r_[np.ones(3), x]
        if np.any(S7 < 0.3):
            return 1e6
        e = ev(S7)
        return e['achse'] + REG * (np.log(S7) ** 2).sum() + STRAFE * sum(max(e[k] - GRENZE, 0) ** 2 for k in ('spitze', 'knick', 'beginn', 'biegung'))
    best = None
    for x0 in ([2.26, .9, .8, 1.9], [3.1, .5, 1, .64], [2.0, 1.0, 1.0, 1.5]):
        r = minimize(kosten, np.array(x0), method='Nelder-Mead', options=dict(maxiter=4000, xatol=1e-4, fatol=1e-4))
        if best is None or r.fun < best.fun:
            best = r
    S7 = np.r_[np.ones(3), best.x]
    e = ev(S7)
    L = {str(16 + i): int(round(L0[i] * S7[i])) for i in range(3, 7)}
    print('Faktoren', np.round(S7, 3).tolist(), 'Laengen', L)
    print('Spitze %.1f, Knick %.1f, Beginn %.1f, Biegung %.1f px, Achse %.1f px' % (e['spitze'], e['knick'], e['beginn'], e['biegung'], e['achse']))
    print('Gelenke P2', np.round(e['q']).astype(int).tolist())
    if a.aus:
        json.dump(L, open(a.aus, 'w'))


if __name__ == '__main__':
    main()
