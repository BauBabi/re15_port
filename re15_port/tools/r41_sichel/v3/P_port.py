# Pruefer V3: Port-Render (Spielkamera 50A0/12, RDT-Licht, Posen sfv2/posen_seite.json) mit denselben Messfunktionen wie
# die KI (P_schwanz.py) - das ist die Riegel-Messmethode fuer Bauer und Pruefer.
# Aufruf: python P_port.py [EMD ...]  (Standard: Ref2 = shared_assets EM30_HANGAR.EMD, V1_SPITZ, V1_KRALLE aus sfv2/emd)
#  Schwanz frei = nur Knochen 16-22 gezeichnet; Wurzel-Quellmenge = frei-Bildpunkte im Radius 0,02 H um das projizierte
#  Gelenk 16; Mittellinie wie KI; Austritt s0 = erster Mittellinienpunkt, an dem im vollen Render ein Schwanzknochen
#  sichtbar ist (3 px in Folge) = Gegenstueck zur KI-Naht; Profil/Glaette ab s0 bis zur Spitze.
#  Farbe = sichtbare Schwanzbildpunkte (voller Render) gegen Haut (Figur ohne Schwanz, OK..+0,45 H, b* >= -3, L* >= 5), je 2 px erodiert.
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
from P_farbe import lab, r1
from P_schwanz import mittellinie, form, farbe, muster, dicke

OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/'
SFV2 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2'
BAUM = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(HIER))))
SCHW = tuple(range(16, 23))


def messen(emd, h, pose):
    mo = b2.modell(emd)
    im, ids = b2.zeichne(mo, pose, None)
    imf, idf = b2.zeichne(mo, pose, None, knochen=SCHW)
    body = (ids >= 0) & ~np.isin(ids, SCHW)
    ys = np.nonzero(body.any(1))[0]; OK, sohle = int(ys.min()), int(ys.max()); H = sohle - OK + 1
    mf = idf >= 0
    lb, k = nd.label(mf); s = nd.sum(mf, lb, range(1, k + 1)); mf = lb == 1 + int(np.argmax(s))
    import birkin_buehne as BB
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    j16 = b2.projiziere(pose, b2.kette_punkte(mo, kf)[0:1])[0]
    yy, xx = np.mgrid[:mf.shape[0], :mf.shape[1]]
    Q = mf & (np.hypot(xx - j16[0], yy - j16[1]) <= max(2.0, 0.02 * H))
    if not Q.any():
        d = np.hypot(xx - j16[0], yy - j16[1]); d[~mf] = 1e9; iy, ix = np.unravel_index(np.argmin(d), d.shape); Q[iy, ix] = True
    C, _ = mittellinie(mf, Q, H)
    vis_t = np.isin(ids, SCHW)
    s0 = 0
    for i in range(len(C) - 3):
        if all(vis_t[int(round(C[j, 1])), int(round(C[j, 0]))] for j in range(i, i + 3)):
            s0 = i; break
    Cs = C[s0:]
    fo, th, dth = form(mf, Cs, H)
    tsicht = vis_t.copy(); lb, k = nd.label(tsicht)
    fa, ms, haut = farbe(im.astype(float), tsicht, ids >= 0, OK, H)
    mu = muster(imf.astype(float), mf, Cs, H)
    vis = im.copy()
    c, _ = cv2.findContours(mf.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(vis, c, -1, (255, 0, 255), 1)
    cv2.polylines(vis, [Cs.astype(np.int32)], False, (255, 255, 0), 1)
    cv2.circle(vis, tuple(map(int, Cs[0])), 3, (0, 255, 255), 1)
    Lc = len(Cs) - 1
    for f in (0.2, 0.5, 0.9):
        i = int(round(f * Lc)); L_, nrm = dicke(mf, Cs, i); p = Cs[i]
        cv2.line(vis, tuple(map(int, p - nrm * L_ / 2)), tuple(map(int, p + nrm * L_ / 2)), (0, 255, 0), 1)
    return dict(H=H, OK=OK, s0_px=s0, versteckt_H=round(s0 / H, 3), form=fo, farbe=fa, muster=mu,
                austritt_unter_OK_H=round((Cs[0][1] - OK) / H, 3)), vis


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    P = json.load(open(os.path.join(SFV2, 'posen_seite.json')))
    emds = sys.argv[1:] or [os.path.join(BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD'),
                            os.path.join(SFV2, 'emd', 'EM30_V1_SPITZ.EMD'), os.path.join(SFV2, 'emd', 'EM30_V1_KRALLE.EMD')]
    erg = {}
    for e in emds:
        nm = os.path.basename(e)
        for h in ('85', '92'):
            d, vis = messen(e, int(h), P[h])
            erg['%s_h%s' % (nm, h)] = d
            cv2.imwrite(OUT + 'P_port_%s_h%s.png' % (nm[:-4], h), cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
            print(nm, h, json.dumps(d))
    json.dump(erg, open(OUT + 'P_port.json', 'w'), indent=1)
