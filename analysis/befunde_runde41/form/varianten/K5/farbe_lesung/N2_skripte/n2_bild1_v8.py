"""Messblatt-Nachtrag V2, Abschnitt 3, Versuch V8: sind K1/K2 (Bild-1-Figur C* >= 7,87, ab-Streuung >= 8,36) unter den Nebenbedingungen
K4 (Figur-Rotklasse <= 7,87 %) und B3 (keine Blau-Klasse b* < -6 und C* > 8) erreichbar?
Wie V7 (Neubeleuchtung n2_bild1_v7.lichtkarte, Ruecken 15 % Zielrot L* 45), Rest der Figur a*b* mal g, danach gekappt:
a* <= 11 (bleibt unter der Rotklasse a* >= 12) und Blau-Klasse auf C* = 8 zurueckgenommen.
"""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import n2_bild1 as B
import n2_bild1_v7 as V
PZ = B.PZ


def main(vdir, aus):
    k = PZ.P.Kandidat(vdir + '/form.json')
    sauber, alb, m, kk = k.rendern(1)
    farb = V.lichtkarte(k, 1)
    fig = m & (alb.max(2) > 0)
    zz = B.zonen1(fig)
    Zf = B.figur(zz)
    allefig = alb.max(2) > 0
    zr = dict(zz)[B.RF]
    La = B.lab(alb[zr].astype(float))
    ordnung = np.argsort(-La[:, 1])
    ys, xs = np.nonzero(zr)
    rotsel = np.zeros((480, 640), bool)
    n = int(0.15 * zr.sum())
    rotsel[ys[ordnung[:n]], xs[ordnung[:n]]] = True
    out = []
    for g in (1.0, 1.5, 2.0, 3.0, 4.0):
        alb2 = alb.astype(np.float32).copy()
        sel = Zf & ~rotsel
        Lq = B.lab(alb[sel].astype(float))
        Lq[:, 1:] *= g
        Lq[:, 1] = np.minimum(Lq[:, 1], 11.0)
        C = np.hypot(Lq[:, 1], Lq[:, 2])
        bl = (Lq[:, 2] < -6) & (C > 8)
        Lq[bl, 1:] *= (8.0 / C[bl])[:, None]
        alb2[sel] = V.lab2rgb(Lq)
        alb2[rotsel] = V.lab2rgb(np.array([[45, 26.5, 18.0]], float))[0]
        s2 = V.neu(sauber, alb, farb, allefig, alb2)
        vv = PZ.FV.video_filter(s2, bild=1, entzerren=False, rauschen=True)
        T2, _c = PZ.rueck(vv, 1)
        st = B.stat(T2, zz)
        Lalb = B.lab(alb2[Zf])
        r = dict(g=g, figur_C=st['figur_C'], figur_ab=st['figur_ab_streu'], figur_rot=st['figur_rot_p'], ruecken_rot=st['a12_C15_L20'],
                 albedo_C=round(float(np.hypot(Lalb[:, 1], Lalb[:, 2]).mean()), 1),
                 albedo_blau_p=round(100 * float(((Lalb[:, 2] < -6) & (np.hypot(Lalb[:, 1], Lalb[:, 2]) > 8)).mean()), 2),
                 zonen={a: (v['C'], v['ab'], v['rot_p']) for a, v in st['zonen'].items()})
        out.append(r)
        print('V8', r)
        sys.stdout.flush()
    json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else B.K5 + '/farbe/K5c_farbe', sys.argv[2] if len(sys.argv) > 2 else os.path.join(B.FL, 'N2_bild1_v8.json'))
