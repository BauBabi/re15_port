import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np, cv2
import v4_gemein as G, b2
P = G.posen()
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
v = G.vorlage(2).astype(np.uint8)
x0, y0, x1, y1 = 380, 160, 500, 280
crop = v[y0:y1, x0:x1].astype(float)
# contrast stretch
lo, hi = np.percentile(crop, 2), np.percentile(crop, 98)
st = np.clip((crop - lo) / (hi - lo) * 255, 0, 255).astype(np.uint8)
tiles = [cv2.resize(v[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST),
         cv2.resize(st, None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)]
for e in (G.REF2, SP + 'EM30_V5r3.EMD'):
    mo = b2.modell(e)
    img, ids = G.zeichne(mo, P['85'])
    o = v.copy()
    k = ids == 9
    cnt, _ = cv2.findContours(k.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    ob = cv2.resize(o[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)
    for c in cnt:
        c2 = ((c - [x0, y0]) * 6 + 3).astype(np.int32)
        cv2.polylines(ob, [c2], True, (0, 255, 255), 1)
    tiles.append(ob)
    tiles.append(cv2.resize(img[y0:y1, x0:x1], None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST))
# grid lines every 10 px
out = []
for t in tiles:
    t = t.copy()
    for gx in range(0, x1 - x0, 10):
        cv2.line(t, (gx * 6, 0), (gx * 6, 4), (255, 255, 0), 1)
        cv2.putText(t, str(x0 + gx), (gx * 6 + 1, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 0), 1)
    for gy in range(0, y1 - y0, 10):
        cv2.line(t, (0, gy * 6), (4, gy * 6), (255, 255, 0), 1)
        cv2.putText(t, str(y0 + gy), (5, gy * 6 + 4), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (255, 255, 0), 1)
    out.append(t)
a = np.vstack([np.hstack(out[0:3]), np.hstack(out[3:6])])
cv2.imwrite('p2.png', cv2.cvtColor(a, cv2.COLOR_RGB2BGR))
