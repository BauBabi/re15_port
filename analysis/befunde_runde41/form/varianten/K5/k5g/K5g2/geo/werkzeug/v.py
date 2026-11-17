# Bildvergleich weisses Licht: python v.py <aus-Praefix> <A.EMD> <B.EMD> <zoom> name:clip:bild[:x0,y0,x1,y1] ...
import sys
import numpy as np, cv2
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_geo as G
FB = G.FB
tim = open(G.K5 + '/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM', 'rb').read()
aus, a, b, zoom = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
A, B = open(a, 'rb').read(), open(b, 'rb').read()
an = G.ansichten()
for s in sys.argv[5:]:
    p = s.split(':'); name, c, bi = p[0], int(p[1]), int(p[2])
    kam, lage, gier, H, W, ab = an[name]
    ia, ma = FB.buehne(A, tim, c, bi, lage, gier, kam, licht=None)
    ib, mb = FB.buehne(B, tim, c, bi, lage, gier, kam, licht=None)
    ia = np.asarray(ia)[:, :, :3].copy(); ib = np.asarray(ib)[:, :, :3].copy()
    d = (np.abs(ia.astype(int) - ib.astype(int)).sum(2) > 24)
    v = ib.copy(); v[d] = (255, 0, 0)
    m = np.asarray(ma); m = (m > 0) if m.ndim == 2 else (m > 0).any(2)
    ys, xs = np.where(m); x0, x1, y0, y1 = max(xs.min() - 6, 0), min(xs.max() + 7, W), max(ys.min() - 6, 0), min(ys.max() + 7, H)
    if len(p) > 3:
        a0, b0, a1, b1 = [int(x) for x in p[3].split(',')]
        x0, y0, x1, y1 = x0 + a0, y0 + b0, x0 + a1, y0 + b1
    w = np.full((y1 - y0, 3, 3), 255, np.uint8)
    bl = np.concatenate([ia[y0:y1, x0:x1], w, ib[y0:y1, x0:x1], w, v[y0:y1, x0:x1]], 1)
    bl = cv2.resize(bl, (bl.shape[1] * zoom, bl.shape[0] * zoom), interpolation=cv2.INTER_NEAREST)
    g = (255.0 * (bl / 255.0) ** 0.6).astype(np.uint8)
    ziel = '%s_%s_%d_%d.png' % (aus, name, c, bi)
    cv2.imwrite(ziel, cv2.cvtColor(g, cv2.COLOR_RGB2BGR))
    print(name, c, bi, 'Pixel anders', int(d.sum()), 'im Ausschnitt', int(d[y0:y1, x0:x1].sum()), '->', ziel, g.shape, flush=True)
