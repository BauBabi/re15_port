# Streifen-Stellen Kopf-Gruppe {22,23} gegen {10,21,16}: python ks.py <aus.png> zoom A.EMD B.EMD name:clip:bild ...
# je Zeile: A weiss beleuchtet | A mit Marken | B weiss beleuchtet | B mit Marken. rot = Kopf gemalt, Rumpf liegt vorn; gelb = Rumpf gemalt, Kopf liegt vorn
import sys
import numpy as np, cv2
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
G = MM.G; FB = G.FB
aus, zoom, emds = sys.argv[1], int(sys.argv[2]), sys.argv[3:5]
tim = open(G.K5 + '/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM', 'rb').read()
zeilen = []
for s in sys.argv[5:]:
    p = s.split(':'); name, c, b = p[0], int(p[1]), int(p[2])
    teile = []; box = None
    for emd in emds:
        w = MM.modell(emd); E = open(emd, 'rb').read()
        kam, lage, gier, H, W, ab = w['an'][name]
        i = MM.bild_index(w['cb'], c, b)
        tris, (mt, pt, mi, pi_) = MM.raster(emd, name, i, (10, 21, 16, 22, 23))
        img = np.asarray(FB.buehne(E, tim, c, b, lage, gier, kam, licht=None)[0])[:, :, :3].copy()
        K = np.isin(mt, (22, 23)); Kt = np.isin(pt, (22, 23)); Rm = np.isin(mt, (10, 21, 16)); Rt = np.isin(pt, (10, 21, 16))
        a = K & Rt; d = Rm & Kt
        v = img.copy(); v[a] = (255, 0, 0); v[d] = (255, 255, 0)
        print(s, emd.split('/')[-1], 'roh (ohne 2-px-Toleranz): Kopf ueber Rumpf', int(a.sum()), 'Rumpf ueber Kopf', int(d.sum()), flush=True)
        if box is None:
            ys, xs = np.where(np.isin(mt, (10, 21, 16, 22, 23)))
            box = (max(xs.min() - 6, 0), min(xs.max() + 7, img.shape[1]), max(ys.min() - 6, 0), min(ys.max() + 7, img.shape[0]))
        teile += [img, v]
    x0, x1, y0, y1 = box
    ws = np.full((y1 - y0, 3, 3), 255, np.uint8)
    k = np.concatenate(sum([[t[y0:y1, x0:x1], ws] for t in teile], [])[:-1], 1)
    zeilen.append(cv2.resize(k, (k.shape[1] * zoom, k.shape[0] * zoom), interpolation=cv2.INTER_NEAREST))
wmax = max(z.shape[1] for z in zeilen)
zeilen = [np.pad(z, ((0, 4), (0, wmax - z.shape[1]), (0, 0)), constant_values=40) for z in zeilen]
g = np.concatenate(zeilen, 0); g = (255.0 * (g / 255.0) ** 0.6).astype(np.uint8)
cv2.imwrite(aus, cv2.cvtColor(g, cv2.COLOR_RGB2BGR)); print(aus, g.shape)
