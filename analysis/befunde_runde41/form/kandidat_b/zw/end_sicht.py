"""Entscheidung Sicht Ende / Sicht Flanke am fertigen Netz. Figur starr wie Pose 1, Bein FL angehoben wie Clip 1 (nur Lage und Gier frei).
Je Sicht: Umrissmasse, Haken, Naht sichtbar?, Linien Q in Bild 1 sichtbar?"""
import sys, json, math, pickle
sys.path.insert(0, '.')
from b_geom import *
import numpy as np, cv2, torch
import b_teile as T, b_bau as BB, b_snap as SN, b_lage2 as LG, b_mesh as BM, b_alles as AL
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P

fit = torch.load('zw/g10/fit.pt', map_location='cpu', weights_only=False)
V, w2, l2, g2 = pickle.load(open('zw/g10/snap_fein.pkl', 'rb'))
te = SN.stuecke('zw/g10/quad')
for st in te:
    st['V'] = np.array(V[st['name']], float)
J = np.asarray(fit['J'], float)
w = np.zeros_like(w2)
for k in ('fl_ober', 'fl_unter', 'fl_fuss', 'fl_zeh'):
    w[T.IDX[k]] = w2[T.IDX[k]]
R, Tt = BB.fk(w, J, T.ELTERN, 0.0)
u = max(((R[st['bone']] @ (st['V'] - J[st['bone']]).T).T + Tt[st['bone']])[:, 1].max() for st in te)
Tt = [t + [0, -u, 0] for t in Tt]
c = np.array(T.KUPPEL_MITTE, float)
N = fit['naht']
F = BM.Feld(fit, {'rumpf_m'}, 30.0)
Hk = P.enge(P.poly_maske(P.teil_b2('H')['polygon']), 2)
Bb = cv2.GaussianBlur((P.original(2).astype(np.float32) @ P.YK), (0, 0), 1.2)
Y1 = (P.original(1).astype(np.float32) @ P.YK)
au1 = lokal(K1['auge'])
aus = []
for name, g0, lg in (('Flanke', 1081, (2625, 0, -23625)), ('Flanke+', 1300, (2625, 0, -23625)), ('Ende', 2000, (2342, 0, -23862)), ('Ende+', 2300, (2600, 0, -23500)), ('Gegenflanke', 2950, (2300, 0, -23350))):
    lage, g = LG.suche(te, R, Tt, J, lg, g0, schritte=(200, 100, 50, 25), g_schritte=(40, 20, 10, 5))
    s = SN.Sicht(2, K2, lage, g, R, Tt, J)
    m = s.maske(te)
    au = (Y(g).T @ (np.asarray(K2['auge'], float) - lage).T).T
    # Naht in Bild 2
    sicht = [(N['n'][i] @ (au - N['P'][i])) > 0 for i in range(len(N['P']))]
    pz = FK.projiziere(K2, (Y(g) @ (R[9] @ (N['P'] - J[9]).T).T.T).T + Tt[9] @ Y(g).T + lage) if False else None
    w_ = s.proj(((R[9] @ (N['P'] - J[9]).T).T + Tt[9]) @ s.Yg.T + lage)
    yv = [round(float(Bb[int(np.clip(q[1], 0, 479)), int(np.clip(q[0], 0, 639))])) for q in w_]
    # Linien Q nach Bild 1
    qs = 0
    qa = 0
    yq = []
    for st_ in P.stellen():
        if st_['id'] not in ('Q1', 'Q3', 'Q5', 'P'):
            continue
        k = np.zeros((480, 640), np.uint8)
        for q in st_['polygone']:
            cv2.fillPoly(k, [np.round(np.array(q, float)).astype(np.int32)], 1)
        ys, xs = np.nonzero(k)
        for x, y in zip(xs[::9], ys[::9]):
            e, d = FK.strahl(K2, np.array([x, y], float))
            t = np.linspace(2500, 9000, 500)
            pw = e + t[:, None] * d
            pm = ((pw - lage) @ Y(g) - Tt[9]) @ R[9] + J[9]
            sd = F(pm.astype(np.float32))
            i = int(np.argmax(sd < 0))
            if sd[i] >= 0:
                continue
            qa += 1
            p = pm[i]
            n = (p - c) / np.linalg.norm(p - c)
            if n @ (au1 - p) > 0:
                qs += 1
                b = bild(p)
                yq.append(float(Y1[int(np.clip(b[1], 0, 479)), int(np.clip(b[0], 0, 639))]))
    z = LG.bewerte(m, cv2.imread(FORM + '/ziel_bild2.png', 0), [], [])[1]
    aus.append(dict(sicht=name, gier=round(g), lage=[round(lage[0]), round(lage[2])], weiss_fehlt=z['weiss_fehlt'], schwarz=z['schwarz'], haken=round(float((m & Hk).sum() / Hk.sum()), 2),
                    naht_sichtbar='%d/%d' % (sum(sicht), len(sicht)), Y_video_naht=[y for y, f in zip(yv, sicht) if f], Q_in_Bild1='%d/%d' % (qs, qa), Y_Bild1=round(float(np.mean(yq))) if yq else None))
    print(aus[-1], flush=True)
json.dump(aus, open('zw/end_sicht.json', 'w'), indent=1)
