"""Zahlen-Pruefer V4 r5: AF (O9) mit eigenem Code in zwei Lesarten.
 R  = Riegel-nah: Farbmenge je Mesh = Original-Bildpunkte im Umkreis 15 px um die Mesh-Bildpunkte, ohne Gelb (b* <= 9,9, Dossier V4 r5);
      enthaelt Hintergrund (Wand, Tuer, Boden), soweit er im Umkreis liegt.
 F  = nur Figur: dieselbe Menge, aber nur innerhalb der Port-Koerpersilhouette (Ids 0-15) um 5 px erodiert (dort liegt das Original-Wesen
      ueberwiegend; Pose-IoU 0,62) - schliesst den Hintergrund ausserhalb der Figur aus.
 Port-Bildpunkte V = a + b P (ohne Gauss), Abstand dE76 (Lab skimage) zum naechsten Mengenpunkt, Anteil > 10 je Mesh."""
import os, sys, json
import numpy as np
from scipy import ndimage as nd
from scipy.spatial import cKDTree
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
from pz5_mess import lab, orig, EA_, EB_, SP
S = SP.rsplit('pz5', 1)[0]
P = G.posen()
out = {}
for nm, pf in (('V4', SP + 'emd/EM30_V4.EMD'), ('V4R4', S + 'emd/EM30_V4r4.EMD'), ('V3', G.EMD_V3)):
    mo = b2.modell(pf)
    out[nm] = {}
    for ans, b in (('135', 1), ('85', 2)):
        img, ids = G.zeichne(mo, P[ans])
        Pl = lab(np.clip(EA_ + EB_ * img.astype(float), 0, 255))
        Lo = lab(orig(b))
        ng = Lo[..., 2] <= 9.9
        sil = nd.binary_erosion((ids >= 0) & (ids <= 15), iterations=5)
        r = {}
        for i in range(15):
            m = ids == i
            if m.sum() < 30:
                continue
            um = nd.binary_dilation(m, iterations=15) & ng
            e = dict(n=int(m.sum()))
            for art, q in (('R', um), ('F', um & sil)):
                if q.sum() < 30:
                    e[art] = None; continue
                d, _ = cKDTree(Lo[q]).query(Pl[m])
                e[art] = round(float((d > 10).mean()), 3)
                e[art + '_p99'] = round(float(np.percentile(d, 99)), 1)
            r[i] = e
        out[nm][ans] = r
        Rm = max((v['R'] or 0, k) for k, v in r.items()); Fm = max((v['F'] or 0, k) for k, v in r.items())
        print(nm, ans, 'R max %.3f (Id %d)' % Rm, 'ueber 0,01:', {k: v['R'] for k, v in r.items() if (v['R'] or 0) > 0.01},
              '| F max %.3f (Id %d)' % Fm, 'ueber 0,01:', {k: v['F'] for k, v in r.items() if (v['F'] or 0) > 0.01})
json.dump(out, open(SP + 'af.json', 'w'), indent=1)
