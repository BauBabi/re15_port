"""SF V4 (Bauer): Kopf nah (Original | V4) x6 mit den hellen Komponenten (K1/S1) und den Fenstern KV/KS (Pruefbild, sfv4/bau)."""
import sys, os
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2, riegel_v4 as RV
name = sys.argv[1] if len(sys.argv) > 1 else 'kopf'
emd = sys.argv[2] if len(sys.argv) > 2 else G.EMD_V4
P = G.posen(); mo = b2.modell(emd)
for h, b in (('135', 1), ('85', 2)):
    img, ids = G.zeichne(mo, P[h]); kb = G.kopfbox(ids); x0, y0, x1, y1 = kb
    V = G.videoraum(img); Lab = G.lab(V); bl = RV.kopf_blob(V, Lab, ids == 9, kb)
    hell = (Lab[..., 0] > bl['thr']) & (ids == 9); lb, k = nd.label(hell)
    for j in range(1, k + 1):
        ys, xs = np.nonzero(lb == j)
        if len(xs) >= 4:
            print(h, 'Komponente', len(xs), (xs.min(), ys.min(), xs.max(), ys.max()))
    m = 6
    c = img[y0 - m:y1 + m, x0 - m:x1 + m].copy()
    fen = RV.FENSTER
    for kname in (('KV1', 'KV2', 'KV3', 'KV4', 'KV5') if h == '135' else ('KS1', 'KS2')):
        bx = G.fenster(kb, fen[kname])
        cv2.rectangle(c, (bx[0] - x0 + m, bx[1] - y0 + m), (bx[2] - x0 + m, bx[3] - y0 + m), (255, 255, 0), 1)
    ob = G.KB_ORIG[b]; o = G.vorlage(b).astype(np.uint8)[ob[1] - m:ob[3] + m, ob[0] - m:ob[2] + m].copy()
    for kname in (('KV1', 'KV2', 'KV3', 'KV4', 'KV5') if h == '135' else ('KS1', 'KS2')):
        bx = G.fenster(ob, fen[kname])
        cv2.rectangle(o, (bx[0] - ob[0] + m, bx[1] - ob[1] + m), (bx[2] - ob[0] + m, bx[3] - ob[1] + m), (255, 255, 0), 1)
    f = 6
    o = cv2.resize(o, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST); c = cv2.resize(c, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    hh = max(o.shape[0], c.shape[0]); pad = lambda a: np.vstack([a, np.zeros((hh - a.shape[0], a.shape[1], 3), np.uint8)])
    cv2.imwrite(os.path.join(G.SFV4, 'bau', '%s_%s.png' % (name, h)), cv2.cvtColor(np.hstack([pad(o), pad(c)]), cv2.COLOR_RGB2BGR))
