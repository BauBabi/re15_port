"""Zahlen-Pruefer V4 r4: Renders (b2.zeichne = vorgeschriebener Messrender, Messblatt V4 Abschn. 7) fuer V4 (eigener Neubau
sfv4/pz5/emd, sha256 dfbc30bc), V3 r2 (sfv4/emd/EM30_V3.EMD, 1952713c) und Ref2 (Spiel-Asset) in den Posen "85" und "135"."""
import os, sys, json, numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
OUT = os.path.join(G.SFV4, 'pz5')
P = json.load(open(G.POSEN))
for nm, pf in (('V4', os.path.join(OUT, 'emd', 'EM30_V4.EMD')), ('V4R4', os.path.join(G.SFV4, 'emd', 'EM30_V4r4.EMD')), ('V3', G.EMD_V3), ('REF2', G.REF2)):
    mo = b2.modell(pf)
    for h in ('85', '135'):
        img, ids, idv = G.zeichne(mo, P[h], voll=True)
        np.savez_compressed(os.path.join(OUT, 'r_%s_%s.npz' % (nm, h)), img=img, ids=ids, idv=idv)
        Image.fromarray(img).save(os.path.join(OUT, 'r_%s_%s.png' % (nm, h)))
        print(nm, h, 'ok', img.shape, idv.shape)
