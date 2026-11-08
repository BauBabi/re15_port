"""Ueberblendung: Foto (entblendet) mit dem Umriss des heutigen Modells (V5r4) in der alten Pose posen_v4 85/135."""
import os, sys, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G, b2
S = os.path.join(G.SFV4, 'foto')
mo = b2.modell(G.EMD_V4); P = G.posen()
for b, k in ((1, '135'), (2, '85')):
    img, ids = G.zeichne(mo, P[k])
    v = np.clip(G.vorlage(b), 0, 255).astype(np.uint8)
    mix = (0.5 * v + 0.5 * img).astype(np.uint8)
    out = cv2.cvtColor(mix, cv2.COLOR_RGB2BGR)
    for bb, col in ((range(0, 16), (0, 255, 0)), (SCHW := range(16, 23), (255, 0, 255))):
        m = np.isin(ids, list(bb)).astype(np.uint8)
        cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(out, cs, -1, col, 1)
    out = cv2.resize(out[100:460, 240:560], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(os.path.join(S, 'alt_%d.png' % b), out)
    for bn in range(23):
        ys, xs = np.nonzero(ids == bn)
        if len(xs): print(b, bn, len(xs), xs.min(), xs.max(), ys.min(), ys.max())
