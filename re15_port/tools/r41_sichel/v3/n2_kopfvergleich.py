# SF V3 Runde 2: Kopf vorn in der Spielkamera, KI und Port gleicher Massstab (1280er Bild; Port 640 x2 NEAREST), Ausschnitt
# +-0,13 H um die Augenmitte, x3 vergroessert, nebeneinander. Aufruf: python n2_kopfvergleich.py aus.png EMD ...
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
import n2_maul_spiel as MS
SC = MS.SC
aus = sys.argv[1]
zeilen = []
for h in (118, 125):
    n, ox, oy = b2.KI_MAP[int(b2.KI_VON_H[h])]
    ok, so = MS.KI_OK[h]; H = so - ok
    ax, ay = MS.KI_AUGE[h][0] + ox, MS.KI_AUGE[h][1] + oy
    def crop(im, ax, ay, H):
        r = int(0.13 * H)
        c = im[int(ay - 0.06 * H):int(ay - 0.06 * H) + 2 * r, int(ax - r):int(ax + r)]
        c = cv2.resize(np.ascontiguousarray(c), (3 * 2 * r, 3 * 2 * r), interpolation=cv2.INTER_NEAREST)
        return c
    reihe = [crop(b2.ki(h, hinter=(60, 60, 60))[0], ax, ay, H)]
    for spec in sys.argv[2:]:
        p = spec
        mo = b2.modell(p)
        im, ids = b2.zeichne(mo, MS.PV[str(h)], None)
        z = MS.port(p, h)
        im2 = cv2.resize(im, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        reihe.append(crop(im2, z['auge_px'][0], z['auge_px'][1], z['H']))
    s = min(x.shape[0] for x in reihe)
    reihe = [cv2.resize(x, (s, s), interpolation=cv2.INTER_NEAREST) for x in reihe]
    zeilen.append(np.hstack(reihe))
w = min(z.shape[1] for z in zeilen)
cv2.imwrite(aus, cv2.cvtColor(np.vstack([z[:, :w] for z in zeilen]), cv2.COLOR_RGB2BGR))
