import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np, cv2
import v4_gemein as G, b2
from PIL import Image
emds = sys.argv[1:-1]; aus = sys.argv[-1]
P = G.posen(); rows = []
for h, box in (('135', (300, 135, 410, 245)), ('85', (420, 165, 510, 255))):
    t = []
    x0, y0, x1, y1 = box
    v = G.vorlage(1 if h == '135' else 2).astype(np.uint8)
    t.append(cv2.resize(v[y0:y1, x0:x1] if h == '85' else v[125:235, 340:450], None, fx=5, fy=5, interpolation=cv2.INTER_CUBIC))
    for e in emds:
        img, ids = G.zeichne(b2.modell(e), P[h])
        t.append(cv2.resize(img[y0:y1, x0:x1], None, fx=5, fy=5, interpolation=cv2.INTER_NEAREST))
    hh = max(a.shape[0] for a in t)
    rows.append(np.concatenate([np.pad(a, ((0, hh - a.shape[0]), (0, 6), (0, 0))) for a in t], 1))
w = max(r.shape[1] for r in rows)
Image.fromarray(np.concatenate([np.pad(r, ((0, 6), (0, w - r.shape[1]), (0, 0))) for r in rows], 0)).save(aus)
