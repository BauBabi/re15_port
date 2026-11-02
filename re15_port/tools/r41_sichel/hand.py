"""Runde 41 Spur SF N1.3: Bildlage der Sichelhand (Knochen 15) und der Schulter (13) in Clip 0x0B je Bild, Hangar-Szene.

    python re15_port/tools/r41_sichel/hand.py [gier_relativ=-1270]

Lage/Gier wie birkin_formen.c (BF_BIRKIN_*, Gier = Blick auf BF_SM_* + gier_relativ), Kamera ROOM50A0 Schnitt 12,
Projektion wie der Zeichner (tools/birkin_buehne.py Kamera/_gte). Ausgabe PSX-Bildpunkte.
"""
import math, os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..'))
import birkin_buehne as BB, emd_ansichtsblatt as EA      # noqa: E402
BX, BZ, SX, SZ = -8488, -21651, -12470, -17540
mo = BB.modell(open(os.path.join(BB.PSX, 'EMD', 'EM30_HANGAR.EMD'), 'rb').read())
r = BB.Raum('50A0'); s = r.schnitt(12); kam = BB.Kamera(s['pos'], s['ziel'], s['fov'] >> 7)
g0 = int(round(math.atan2(-(SZ - BZ), SX - BX) * 4096 / (2 * math.pi))) & 0xfff
g = (g0 + (int(sys.argv[1]) if len(sys.argv) > 1 else -1270)) & 0xfff
Y = np.array([[EA.cos_q12(g), 0, EA.sin_q12(g)], [0, 4096, 0], [-EA.sin_q12(g), 0, EA.cos_q12(g)]], dtype=np.int64)
lage = np.array([BX, 0, BZ], dtype=np.int64)
def proj(p):
    v = ((kam.V @ np.array(p, dtype=np.int64)) >> 12) + kam.t
    x, y = BB._gte(np.array([v[0]]), np.array([v[1]]), np.array([v[2]]), kam.h)
    return int(x[0]), int(y[0])
print('Gier %d (Bezug %d)' % (g, g0))
for b in range(0, mo.clips[0x0B][1], 2):
    R, T = mo.pose(BB.kf_von_clip(mo, 0x0B, b))
    h15, h13 = ((Y @ T[15]) >> 12) + lage, ((Y @ T[13]) >> 12) + lage
    print('B%2d Hand %s Schulter %s Hand-Welt %s' % (b, proj(h15), proj(h13), h15.tolist()))
