"""Zahlen-Pruefer V4 r5: AF fuer die Bein-Meshes 2/3/5/6 mit der Quellregion des Messblatts/der Sicht (Unterschenkel-Box 356-392 x 352-430,
Vorlage 1 entblendet; Bauer: dieselbe Box fuer beide Ansichten): (Q) nur die Box; (QR) Box + Umkreis 15 px ohne Gelb (Lesart des Riegels)."""
import os, sys, json
import numpy as np
from scipy import ndimage as nd
from scipy.spatial import cKDTree
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
from pz5_mess import lab, orig, EA_, EB_, SP
S = SP.rsplit('pz5', 1)[0]
P = G.posen()
L1 = lab(orig(1))
box = L1[352:431, 356:393].reshape(-1, 3)
for nm, pf in (('V4', SP + 'emd/EM30_V4.EMD'), ('V4R4', S + 'emd/EM30_V4r4.EMD'), ('V3', G.EMD_V3)):
    mo = b2.modell(pf)
    for ans, b in (('135', 1), ('85', 2)):
        img, ids = G.zeichne(mo, P[ans])
        Pl = lab(np.clip(EA_ + EB_ * img.astype(float), 0, 255))
        Lo = lab(orig(b))
        res = {}
        for i in (2, 3, 5, 6):
            m = ids == i
            if m.sum() < 30:
                continue
            um = nd.binary_dilation(m, iterations=15) & (Lo[..., 2] <= 9.9)
            dq, _ = cKDTree(box).query(Pl[m])
            dr, _ = cKDTree(np.vstack([box, Lo[um]])).query(Pl[m])
            res[i] = dict(n=int(m.sum()), Q=round(float((dq > 10).mean()), 3), QR=round(float((dr > 10).mean()), 3), p99_Q=round(float(np.percentile(dq, 99)), 1))
        print(nm, ans, res)
