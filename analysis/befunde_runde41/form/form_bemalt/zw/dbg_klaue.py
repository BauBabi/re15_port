import os, sys, json, math, numpy as np
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import rf_kern, rf_plan, rf_bau, fb_form
import form_pruefung as P, form_kamera as FK
g, T = rf_plan.plan()
st = json.load(open('stand_final.json'))
L, C = rf_bau.wirksam(T, st['lr'], st['c'])
k = P.Kandidat('form.json')
lage, gier, clip, bild = k.stand(2); kf = k.kf(clip, bild)
R, Tt = k.mo.pose(kf)
a = gier * 2 * math.pi / 4096
Y = np.array([[math.cos(a), 0, math.sin(a)], [0, 1, 0], [-math.sin(a), 0, math.cos(a)]])
kam = FK.lade_kamera(2)
ziel = {'kralle_l': (439, 418), 'klaue_l2': (468, 417), 'klaue_l1': (450, 416), 'klaue_l3': (461, 392), 'klaue_l4': (414, 394)}
for i, p in enumerate(T):
    if p.name not in ziel:
        continue
    ring, k0, k1 = rf_bau.teil_ecken(p, L[i], C[i])
    c = ring[-1].mean(0)
    d = (k1 - c) / np.linalg.norm(k1 - c)
    b = p.knochen
    Rw = Y @ (R[b].astype(float) / 4096); Tw = Y @ Tt[b].astype(float) + np.asarray(lage, float)
    best = None
    for ext in range(0, 400, 10):
        q = Rw @ (k1 + d * ext) + Tw
        xy = FK.projiziere(kam, q[None])[0]
        dist = math.hypot(xy[0] - ziel[p.name][0], xy[1] - ziel[p.name][1])
        if best is None or dist < best[0]:
            best = (dist, ext, xy)
    q0 = FK.projiziere(kam, (Rw @ k1 + Tw)[None])[0]
    print(p.name, 'Spitze jetzt', np.round(q0, 1), 'Ziel', ziel[p.name], 'Abstand %.1f' % math.hypot(q0[0] - ziel[p.name][0], q0[1] - ziel[p.name][1]), '-> beste Verlaengerung', best[1], 'Abstand %.1f' % best[0], np.round(best[2], 1), 'Laenge Kegel', round(float(np.linalg.norm(k1 - c))))
print('--- neue Spitzen (Knochenraum), Ziel + 2 px in Klauenrichtung')
from scipy.optimize import least_squares
aus = {}
for i, p in enumerate(T):
    if p.name not in ('kralle_l', 'klaue_l2'):
        continue
    ring, k0, k1 = rf_bau.teil_ecken(p, L[i], C[i])
    c = ring[-1].mean(0)
    b = p.knochen
    Rw = Y @ (R[b].astype(float) / 4096); Tw = Y @ Tt[b].astype(float) + np.asarray(lage, float)
    pc = FK.projiziere(kam, (Rw @ c + Tw)[None])[0]
    zt = np.array(ziel[p.name], float)
    r = (zt - pc) / np.linalg.norm(zt - pc)
    zt2 = zt + 2 * r
    # Boden in Pose 1 und Ruhe: Spitze nicht unter den tiefsten anderen Punkt (sonst hebt form_emd.boden_nachfuehren die ganze Figur, g16: Bild 1 um 4 px)
    bod = []
    for cl in (0, 2):
        Rc, Tc = k.mo.pose(k.kf(cl, 0))
        ymax = -1e9
        for bb, mm in enumerate(k.mo.meshes):
            if bb in (13, 14, 15, 28, 30):
                continue
            for vs in (mm.tv, mm.qv):
                if vs:
                    V = np.array([x[:3] for x in vs], float) @ (Rc[bb].astype(float) / 4096).T + Tc[bb].astype(float)
                    ymax = max(ymax, V[:, 1].max())
        bod.append((Rc[b].astype(float) / 4096, Tc[b].astype(float), ymax))
    def f(x):
        q = Rw @ (k1 + x) + Tw
        r_ = [FK.projiziere(kam, q[None])[0] - zt2, 0.002 * x]
        for Rc_, Tc_, ym in bod:
            yy = (Rc_ @ (k1 + x) + Tc_)[1]
            r_.append([0.2 * max(0.0, yy - (ym - 25))])
        return np.concatenate(r_)
    s = least_squares(f, np.zeros(3))
    dk = s.x
    q = FK.projiziere(kam, (Rw @ (k1 + dk) + Tw)[None])[0]
    print('   Hoehe ueber tiefstem Punkt Pose1/Ruhe', [round(ym - (Rc_ @ (k1 + dk) + Tc_)[1]) for Rc_, Tc_, ym in bod])
    print(p.name, 'dk1', np.round(dk).astype(int).tolist(), '|dk1| %.0f' % np.linalg.norm(dk), 'Projektion', np.round(q, 1), 'Ziel', zt2.round(1))
    aus[p.name] = np.round(dk).astype(int).tolist()
json.dump(aus, open('zw/klaue_links.json', 'w'))
