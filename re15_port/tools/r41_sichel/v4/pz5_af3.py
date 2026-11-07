"""Zahlen-Pruefer V4 r5: AF Kopf (Id 9) gegen die Kopfbox des Messblatts (Abschnitt 2/3) mit/ohne Umkreis, ohne Gelb (b* <= 9,9)."""
import os, sys
import numpy as np
from scipy import ndimage as nd
from scipy.spatial import cKDTree
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
from pz5_mess import lab, orig, EA_, EB_, SP
S = SP.rsplit('pz5', 1)[0]
P = G.posen()
KB = {1: (368, 139, 416, 191), 2: (412, 190, 467, 263)}
for nm, pf in (('V4', SP + 'emd/EM30_V4.EMD'), ('V4R4', S + 'emd/EM30_V4r4.EMD'), ('V3', G.EMD_V3)):
    mo = b2.modell(pf)
    for ans, b in (('135', 1), ('85', 2)):
        img, ids = G.zeichne(mo, P[ans])
        Pl = lab(np.clip(EA_ + EB_ * img.astype(float), 0, 255)); Lo = lab(orig(b))
        x0, y0, x1, y1 = KB[b]
        kb = Lo[y0:y1 + 1, x0:x1 + 1].reshape(-1, 3); kb = kb[kb[:, 2] <= 9.9]
        m = ids == 9
        um = nd.binary_dilation(m, iterations=15) & (Lo[..., 2] <= 9.9)
        dq, _ = cKDTree(kb).query(Pl[m]); dr, _ = cKDTree(np.vstack([kb, Lo[um]])).query(Pl[m])
        print(nm, ans, 'Kopfbox allein %.3f (p99 %.1f), Kopfbox+Umkreis %.3f' % ((dq > 10).mean(), np.percentile(dq, 99), (dr > 10).mean()))
