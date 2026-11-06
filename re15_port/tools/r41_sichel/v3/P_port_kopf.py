# Pruefer V3: Port-Kopf (Auge, Schulterauge, Maul) an der Nahansicht "Kopf vorn nah" (EA.render, neutrales Licht, 960x960,
# 0,9 px je Modelleinheit, Mitte T[9] + (150,-150,0), Blick +X; wie leisten_v2.leiste_rundum / A_port_v1.py).
# H in Bildpunkten = 0,9 x 3865 E. (Formblatt V2: Port-H 3830-3900 E.) = 3479 px.
# Verfahren wie P_kopf.py (Auge: Halbwert-Schwelle + konvexe Huelle; Maul: Huelle der dunklen Maulflaeche + Zaehne).
# Aufruf: python P_port_kopf.py [EMD] -> rendert selbst (b2.modell, BB.kf_von_clip(mo,1,0)), misst, schreibt P_port_kopf.json.
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA
from P_farbe import lab, r1
from P_kopf import auge

OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/'
SFV2 = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv2'
H_PX = 0.9 * 3865


def nah(emd):
    mo = b2.modell(emd)
    kf = BB.kf_von_clip(mo, 1, 0)
    V, N, F = mo.geometrie(kf); R, T = mo.pose(kf)
    mitte = T[9].astype(float) + np.array([150.0, -150, 0])
    col, alp = EA.render(mo, V - mitte, N, F, np.array([0, 0, 1.0]), np.array([0.0, 1.0, 0.0]), np.array([1.0, 0, 0]), 960, 960, 0.9, 480, 480)
    b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8)
    return b, alp


def maul_port(rgb, alpha, fenster, H):
    """Maul = Komponente L* < 0,5 x Kopfhaut-Median mit dem dunkelsten Punkt des Fensters, Loecher gefuellt, plus
    angrenzende helle Zahn-Bildpunkte (L* > Haut+15, 4 px Umkreis); Breite/Hoehe = Bbox."""
    x0, y0, x1, y1 = fenster
    A = lab(rgb); L = A[..., 0]
    sub = L[y0:y1, x0:x1]
    Lh = float(np.median(sub))
    iy, ix = np.unravel_index(np.argmin(nd.gaussian_filter(sub, 3)), sub.shape)
    lb, _ = nd.label(sub < 0.5 * Lh); m = nd.binary_fill_holes(lb == lb[iy, ix])
    z = (sub > Lh + 15) & nd.binary_dilation(m, iterations=4) & ~m
    lb2, _ = nd.label(z)
    mm = m | z
    ys, xs = np.nonzero(mm); w, h = xs.ptp() + 1, ys.ptp() + 1
    return dict(bbox=[int(xs.min() + x0), int(ys.min() + y0), int(xs.max() + x0), int(ys.max() + y0)], b_H=round(w / H, 4), h_H=round(h / H, 4),
                bh=round(w / h, 2), lab_innen=r1(A[y0:y1, x0:x1][m & (sub < 0.5 * Lh)].mean(0)),
                lab_zahn=(r1(A[y0:y1, x0:x1][z].mean(0)) if z.any() else None), zahn_teile=int(lb2.max()), L_haut=round(Lh, 1))


if __name__ == '__main__':
    emd = sys.argv[1] if len(sys.argv) > 1 else os.path.join(SFV2, 'emd', 'EM30_V1_SPITZ.EMD')
    b, alp = nah(emd)
    cv2.imwrite(OUT + 'P_port_kopfnah_%s.png' % os.path.basename(emd)[:-4], cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
    rgb = b.astype(float); al = (alp > 0).astype(np.uint8) * 255
    erg = {}
    # Port V1: Ringauge = dunkler Aussenrand + heller Ring + dunkle Pupille. Gesamtauge = Komponente |L - Haut| > 12 um die Mitte.
    A = lab(rgb); L = A[..., 0]
    for nm, (cx, cy) in (('kopfauge', (430, 435)), ('schulterauge', (88, 485))):
        yy, xx = np.mgrid[:960, :960]; r = np.hypot(xx - cx, yy - cy)
        ring = (r > 60) & (r < 80); Lh = float(np.median(L[ring]))
        lb, _ = nd.label((np.abs(L - Lh) > 12) & (r < 60)); k = lb[cy, cx]
        if k == 0:
            vals = lb[(r < 6)]; k = np.bincount(vals[vals > 0]).argmax()
        m = nd.binary_fill_holes(lb == k)
        ys, xs = np.nonzero(m); w, h = xs.ptp() + 1, ys.ptp() + 1
        haut = (r > 60) & (r < 80)
        erg[nm] = dict(b_H=round(w / H_PX, 4), h_H=round(h / H_PX, 4), bh=round(w / h, 2), L_auge_haut=round(float(L[m].mean() / L[haut].mean()), 3),
                       anteil_L80=round(float((L[m] > 80).mean()), 3), anteil_heller_haut=round(float((L[m] > L[haut].mean()).mean()), 3),
                       anteil_haut_plus15=round(float((L[m] > L[haut].mean() + 15).mean()), 3),
                       lab_auge=r1(A[m].mean(0)), lab_haut=r1(A[haut].mean(0)), lab_hell=r1(A[m & (L > L[haut].mean() + 15)].mean(0)) if (m & (L > L[haut].mean() + 15)).any() else None)
    erg['maul'] = maul_port(rgb, al, (360, 520, 560, 800), H_PX)
    erg['kopfauge']['ueber_maul_H'] = round(((erg['maul']['bbox'][1] + erg['maul']['bbox'][3]) / 2 - 435) / H_PX, 4)
    print(json.dumps(erg, indent=1))
    json.dump(erg, open(OUT + 'P_port_kopf.json', 'w'), indent=1)
