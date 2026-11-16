# Wo malt 23 ueber 22 (rot) und 22 ueber 23 (gelb)? python kj.py <aus.png> <EMD> zoom name:clip:bild ...
import sys
import numpy as np, cv2
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
G = MM.G; FB = G.FB
aus, emd, zoom = sys.argv[1], sys.argv[2], int(sys.argv[3])
E = open(emd, 'rb').read(); tim = open(G.K5 + '/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM', 'rb').read()
w = MM.modell(emd); zeilen = []
for s in sys.argv[4:]:
    name, c, b = s.split(':'); c = int(c); b = int(b)
    kam, lage, gier, H, W, ab = w['an'][name]
    i = MM.bild_index(w['cb'], c, b)
    tris, (mt, pt, mi, pi_) = MM.raster(emd, name, i, w['alle'])
    img = np.asarray(FB.buehne(E, tim, c, b, lage, gier, kam, licht=None)[0])[:, :, :3].copy()
    a = (mt == 23) & (pt == 22); d = (mt == 22) & (pt == 23)
    print(s, 'Bild', img.shape, 'Raster', mt.shape, '23 ueber 22 (roh, ohne 2-px-Toleranz):', int(a.sum()), '22 ueber 23:', int(d.sum()))
    ys, xs = np.where((mt == 23) | (mt == 22)); x0, x1, y0, y1 = max(xs.min() - 8, 0), xs.max() + 9, max(ys.min() - 8, 0), ys.max() + 9
    v = img.copy(); v[a] = (255, 0, 0); v[d] = (255, 255, 0)
    k = np.concatenate([img[y0:y1, x0:x1], np.full((y1 - y0, 3, 3), 255, np.uint8), v[y0:y1, x0:x1]], 1)
    k = cv2.resize(k, (k.shape[1] * zoom, k.shape[0] * zoom), interpolation=cv2.INTER_NEAREST)
    zeilen.append(k)
wmax = max(z.shape[1] for z in zeilen)
zeilen = [np.pad(z, ((0, 4), (0, wmax - z.shape[1]), (0, 0)), constant_values=40) for z in zeilen]
g = np.concatenate(zeilen, 0); g = (255.0 * (g / 255.0) ** 0.6).astype(np.uint8)
cv2.imwrite(aus, cv2.cvtColor(g, cv2.COLOR_RGB2BGR)); print(aus, g.shape)
