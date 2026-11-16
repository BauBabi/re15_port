# Umriss-/Teilwechsel je Ansicht: python um.py <aus.png> zoom A.EMD B.EMD name:clip:bild ...
# Marken auf Bild B: rot = in A Figur, in B Hintergrund (Umriss verloren); gruen = in A Hintergrund, in B Figur; blau = Teil 10 -> Teil 16
import sys, collections
import numpy as np, cv2
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
G = MM.G; FB = G.FB
aus, zoom, a, b = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]
tim = open(G.K5 + '/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM', 'rb').read()
zeilen = []
for s in sys.argv[5:]:
    name, c, bi = s.split(':'); c = int(c); bi = int(bi)
    r = []
    for emd in (a, b):
        w = MM.modell(emd); i = MM.bild_index(w['cb'], c, bi)
        kam, lage, gier, H, W, ab = w['an'][name]
        tris, (mt, pt, mi, pi_) = MM.raster(emd, name, i, w['alle'])
        img = np.asarray(FB.buehne(open(emd, 'rb').read(), tim, c, bi, lage, gier, kam, licht=None)[0])[:, :, :3].copy()
        r.append((mt, img))
    (ma, ia), (mb, ib) = r
    weg = (ma >= 0) & (mb < 0); neu = (ma < 0) & (mb >= 0)
    cnt = collections.Counter(zip(ma[ma != mb].tolist(), mb[ma != mb].tolist()))
    print(s, 'Umriss verloren', int(weg.sum()), 'dazu', int(neu.sum()), '| groesste Wechsel (Teil A, Teil B, Bildpunkte):', cnt.most_common(8), flush=True)
    v = ib.copy(); v[(ma == 10) & (mb == 16)] = (0, 0, 255); v[weg] = (255, 0, 0); v[neu] = (0, 255, 0)
    ys, xs = np.where((ma >= 0) | (mb >= 0)); x0, x1, y0, y1 = max(xs.min() - 6, 0), min(xs.max() + 7, ia.shape[1]), max(ys.min() - 6, 0), min(ys.max() + 7, ia.shape[0])
    ws = np.full((y1 - y0, 3, 3), 255, np.uint8)
    k = np.concatenate([ia[y0:y1, x0:x1], ws, ib[y0:y1, x0:x1], ws, v[y0:y1, x0:x1]], 1)
    zeilen.append(cv2.resize(k, (k.shape[1] * zoom, k.shape[0] * zoom), interpolation=cv2.INTER_NEAREST))
wmax = max(z.shape[1] for z in zeilen)
zeilen = [np.pad(z, ((0, 4), (0, wmax - z.shape[1]), (0, 0)), constant_values=40) for z in zeilen]
g = np.concatenate(zeilen, 0); g = (255.0 * (g / 255.0) ** 0.6).astype(np.uint8)
cv2.imwrite(aus, cv2.cvtColor(g, cv2.COLOR_RGB2BGR)); print(aus, g.shape)
