"""Zahlen-Pruefer V4 r5: Vergleich V4 r3 (sfv4/emd/EM30_V4r3.EMD) fuer O11 (T3/T5 nicht schlechter als Runde 3), eigener Code pz5_mess."""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2, hashlib
import pz5_mess as PM
SP = PM.SP
P = json.load(open(G.POSEN))
pf = os.path.join(G.SFV4, 'emd', 'EM30_V4r3.EMD')
print('V4r3 sha', hashlib.sha256(open(pf, 'rb').read()).hexdigest()[:8])
mo = b2.modell(pf)
img, ids, idv = G.zeichne(mo, P['85'], voll=True)
np.savez_compressed(SP + 'r_V4R3_85.npz', img=img, ids=ids, idv=idv)
s = PM.seite(img, ids, idv, 'V4R3')
T = s['T']; H = s['H']
kr = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]], np.uint8)
m = (ids == 16) & (cv2.dilate(np.isin(ids, (7, 8)).astype(np.uint8), kr) > 0)
ys, xs = np.nonzero(m); i = int(np.argmax(ys + 1e-3 * xs)); b = (int(xs[i]), int(ys[i]))
Pp, W, sl, jk = np.array(T['_P']), np.array(T['_W']), np.array(T['_s']), T['_jk']
j0 = int(np.argmin(np.linalg.norm(Pp[:jk] - np.array(b), axis=1)))
L = sl[jk] - sl[j0]
wf = lambda f: W[int(np.argmin(np.abs(sl - (sl[jk] - f * L))))] / H
kn = Pp[jk]
print('T1', b, 'T3rel', round((kn[0] - b[0]) / H, 3), round((kn[1] - b[1]) / H, 3), 'T5 ab T1', [round(wf(f), 3) for f in (0.1, 0.3, 0.5, 0.7)])
