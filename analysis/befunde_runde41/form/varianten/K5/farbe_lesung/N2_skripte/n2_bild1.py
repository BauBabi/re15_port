"""Messblatt-Nachtrag V2, Abschnitt 3: Ursache Rotklasse Bild 1 Ruecken/Flanke (Original 11,9 %, r3 2,3 %).
Kette exakt wie p_riegel.kette_riegel (p_zonen.rueck, Zonen p_zonen.regionen(1), Figur = Maske & Albedo > 0).
Versuche:
 V1 Schwellen-Empfindlichkeit der Pruefer-Rotklasse (a* >= 12, C* >= 15, L* >= 20) am Original und an r3.
 V2 Zerlegung der Rot-Fehlmenge: welche Bedingung scheitert bei r3 (farbig aber L* < 20 / hell aber unbunt).
 V3 Rundlauf: Original-T x Licht -> video_filter -> rueck; wie viel Rot verliert die Kette selbst.
 V4 float- gegen uint8-Lab (Quantisierung, B2-Befund des Zahlen-Pruefers r3).
 V5 Albedo gegen Kette fuer r3 (Ruecken).
 V6-V8 (Ersatzversuche mit exakter Neubeleuchtung): n2_bild1_v7.py, n2_bild1_v8.py.

    python n2_bild1.py [<Variantenordner>] [<aus.json>]
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
FL = os.path.normpath(os.path.join(HIER, '..'))
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
import p_zonen as PZ
from p_lade import lab, K5

RF = 'Ruecken/Flanke'


def klassen(L, a=12, c=15, l=20):
    C = np.hypot(L[:, 1], L[:, 2])
    return (L[:, 1] >= a) & (C >= c) & (L[:, 0] >= l)


def lab_u8(rgb):
    x = np.clip(np.round(rgb), 0, 255).astype(np.uint8).reshape(1, -1, 3)
    o = cv2.cvtColor(x, cv2.COLOR_RGB2LAB).reshape(-1, 3).astype(float)
    return np.stack([o[:, 0] * 100 / 255, o[:, 1] - 128, o[:, 2] - 128], 1)


def zonen1(fig=None):
    return [(a, (m & fig) if fig is not None else m) for a, m in PZ.regionen(1)]


def figur(zz):
    Z = np.zeros((480, 640), bool)
    for _a, mm in zz:
        Z |= mm
    return Z


def r1(x):
    return [round(float(v), 1) for v in x]


def stat(T, zz):
    s = {}
    Z = dict(zz)
    L = lab(np.clip(T[Z[RF]], 0, 255))
    C = np.hypot(L[:, 1], L[:, 2])
    for (a, c, l) in ((12, 15, 20), (10, 15, 20), (14, 15, 20), (12, 13, 20), (12, 17, 20), (12, 15, 18), (12, 15, 22), (12, 15, 15), (12, 15, 0)):
        s['a%d_C%d_L%d' % (a, c, l)] = round(100 * float(klassen(L, a, c, l).mean()), 1)
    s['px'] = int(Z[RF].sum())
    s['lab_mittel'] = r1(L.mean(0))
    s['L_p10_50_90'] = r1(np.percentile(L[:, 0], (10, 50, 90)))
    s['a_p10_50_90'] = r1(np.percentile(L[:, 1], (10, 50, 90)))
    s['C_p10_50_90'] = r1(np.percentile(C, (10, 50, 90)))
    R = klassen(L)
    if R.sum():
        s['rot_lab'] = r1(L[R].mean(0))
        s['rot_L_p10_50_90'] = r1(np.percentile(L[R, 0], (10, 50, 90)))
    s['farbig_aber_dunkel_p'] = round(100 * float(((L[:, 1] >= 12) & (C >= 15) & (L[:, 0] < 20)).mean()), 1)
    s['hell_aber_unbunt_p'] = round(100 * float(((L[:, 0] >= 20) & ~((L[:, 1] >= 12) & (C >= 15))).mean()), 1)
    s['rot_uint8_p'] = round(100 * float(klassen(lab_u8(np.clip(T[Z[RF]], 0, 255))).mean()), 1)
    Lf = lab(np.clip(T[figur(zz)], 0, 255))
    s['figur_rot_p'] = round(100 * float(klassen(Lf).mean()), 2)
    s['figur_rot_uint8_p'] = round(100 * float(klassen(lab_u8(np.clip(T[figur(zz)], 0, 255))).mean()), 2)
    s['figur_C'] = round(float(np.hypot(Lf[:, 1], Lf[:, 2]).mean()), 2)
    s['figur_ab_streu'] = round(float(np.sqrt(Lf[:, 1].var() + Lf[:, 2].var())), 2)
    s['zonen'] = {}
    for a_, mm in zz:
        Lz = lab(np.clip(T[mm], 0, 255))
        Cz = np.hypot(Lz[:, 1], Lz[:, 2])
        s['zonen'][a_] = dict(px=int(mm.sum()), rot_p=round(100 * float(klassen(Lz).mean()), 1), C=round(float(Cz.mean()), 1),
                              ab=round(float(np.sqrt(Lz[:, 1].var() + Lz[:, 2].var())), 1), lab=r1(Lz.mean(0)))
    return s


def main(vdir, aus):
    out = {}
    v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild1.png'), cv2.COLOR_BGR2RGB)
    To, _clipo = PZ.rueck(v, 1)
    k = PZ.P.Kandidat(vdir + '/form.json')
    sauber, alb, m, kk = k.rendern(1)
    fig = m & (alb.max(2) > 0)
    vid = PZ.FV.video_filter(sauber, bild=1, entzerren=False, rauschen=True)
    Tv, _clipv = PZ.rueck(vid, 1)
    out['orig'] = stat(To, zonen1())
    out['var'] = stat(Tv, zonen1(fig))
    # V3 Rundlauf
    e = np.clip(To * PZ.LICHT[1], 0, 255).astype(np.uint8)
    vr = PZ.FV.video_filter(e, bild=1, entzerren=False, rauschen=True)
    Tr, _c = PZ.rueck(vr, 1)
    out['rundlauf_orig'] = stat(Tr, zonen1())
    # V5 Albedo gegen Kette (Variante, Ruecken)
    zr = dict(zonen1(fig))[RF]
    La = lab(alb[zr].astype(float))
    Lt = lab(np.clip(Tv[zr], 0, 255))
    out['albedo_kette'] = dict(albedo_lab=r1(La.mean(0)), kette_lab=r1(Lt.mean(0)), sauber_lab=r1(lab(sauber[zr].astype(float)).mean(0)),
                               albedo_rot_p=round(100 * float(klassen(La).mean()), 1),
                               albedo_rotA3_p=round(100 * float(((La[:, 1] > 15) & (np.hypot(La[:, 1], La[:, 2]) > 20)).mean()), 1),
                               albedo_L_p10_50_90=r1(np.percentile(La[:, 0], (10, 50, 90))),
                               kette_rot_p=round(100 * float(klassen(Lt).mean()), 1), licht_k=[float(x) for x in kk] if kk is not None else None)
    json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print(json.dumps(out, ensure_ascii=False, indent=1))


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else K5 + '/farbe/K5c_farbe', sys.argv[2] if len(sys.argv) > 2 else os.path.join(FL, 'N2_bild1.json'))
