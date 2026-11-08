import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np, cv2
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB
from PIL import Image
emd = sys.argv[1]; P = G.posen(); mo = b2.modell(emd)
tiles = []
for h in ('135', '85'):
    img, ids = G.zeichne(mo, P[h]); L = G.lab(img.astype(float))[..., 0]; k = ids == 9
    il, _ = G.zeichne(mo, P[h], licht=False); Ll = G.lab(il.astype(float))[..., 0]
    med = np.median(L[k]); hell = k & (L > med + 8)
    kb = G.kopfbox(ids); x0, y0, x1, y1 = kb; m = 6
    a = img[y0-m:y1+m, x0-m:x1+m].copy(); b = a.copy(); b[hell[y0-m:y1+m, x0-m:x1+m]] = (0, 255, 255)
    c = il[y0-m:y1+m, x0-m:x1+m]
    tiles.append(np.concatenate([cv2.resize(t, None, fx=8, fy=8, interpolation=cv2.INTER_NEAREST) for t in (a, b, c)], 1))
    print(h, 'median', med, 'hell', hell.sum(), k.sum(), 'Textur ohne Licht: L P50/P90/P99', np.percentile(Ll[k], [50, 90, 99]).round(1))
w = max(t.shape[1] for t in tiles)
Image.fromarray(np.concatenate([np.pad(t, ((0, 4), (0, w - t.shape[1]), (0, 0))) for t in tiles], 0)).save(sys.argv[2])
