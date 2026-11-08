"""Zahlen-Pruefer: Gelenkwinkel der Schwanzkette in den KEYFRAMES (V4/V3/Ref2) je Clip; Vergleich mit dem Lauf."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
def winkel(P):
    return [round(float(np.degrees(np.arccos(np.clip((P[i]-P[i-1]) @ (P[i+1]-P[i]) / np.linalg.norm(P[i]-P[i-1]) / np.linalg.norm(P[i+1]-P[i]), -1, 1)))), 1) for i in range(1, len(P)-1)]
for nm, pf in (('V4', G.EMD_V4), ('V3', G.EMD_V3), ('REF2', G.REF2)):
    mo = b2.modell(pf)
    gmax = np.zeros(6); wo = [None]*6; n30 = 0; ntot = 0
    for c, (f0, nf) in enumerate(mo.clips):
        for b in range(nf):
            kf = BB.kf_von_clip(mo, c, b)
            w = winkel(b2.kette_punkte(mo, kf)); ntot += 1
            n30 += any(x > 30 for x in w)
            for i, x in enumerate(w):
                if x > gmax[i]: gmax[i] = x; wo[i] = (c, b)
    print(nm, 'max Keyframe-Gelenkwinkel je Gelenk 1-6:', gmax.round(1).tolist(), 'bei (clip,bild)', wo, '; Keyframes mit Winkel >30 Grad:', n30, 'von', ntot)
    for c, b in ((11, 13), (11, 23), (11, 59), (11, 64), (1, 0), (1, 10), (1, 26), (1, 45)):
        if c < len(mo.clips):
            print('   clip %d bild %d: %s' % (c, b, winkel(b2.kette_punkte(mo, BB.kf_von_clip(mo, c, b)))))
