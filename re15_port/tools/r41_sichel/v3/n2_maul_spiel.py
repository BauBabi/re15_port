# SF V3 Runde 2: Maul in der SPIELKAMERA (vorn h118/h125), gleicher Massstab fuer KI und Port wie Leiste 05
# (KI-Freisteller im 1280er Videobild b2.ki(h), Port 640er b2.zeichne x2 NEAREST). H = OK bis Sohle im selben Bild.
# Augenmitte: KI Messblatt K1 (birkin19 (185,9 40,9), birkin20 (205,3 41,0) + Versatz b2.KI_MAP), Port = dunkelster Punkt
# (Gauss 1) in der oberen Haelfte von Mesh 9. Haut = Median L* im Ring 0,012-0,02 H um das Auge.
# Dunkle Zone = groesste Komponente L* < 0,5 x Haut im Fenster Auge + (0,015 .. 0,12) H, x +-0,06 H; Hoehe/Breite = Bbox.
# Glanz = Bildpunkte mit max(RGB) > 170 in der Bbox der dunklen Zone (Sicht-Pruefer r1, B1).
# Aufruf: python n2_maul_spiel.py EMD [EMD ...]
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
from P_farbe import lab, r1
SC = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
PV = json.load(open(SC + '/sfv2/posen_vorn.json'))
KI_OK = {118: (315, 884), 125: (293, 894)}       # leisten_v2.vorn_original (1280er Bild)
KI_AUGE = {118: (185.9, 40.9), 125: (205.3, 41.0)}   # Messblatt V3 K1 (Freisteller-Pixel)


def zone(rgb, ax, ay, H, aus=None, f=0.5):
    A = lab(rgb.astype(float)); L = A[..., 0]
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay)
    Lh = float(np.median(L[(rr >= 0.012 * H) & (rr <= 0.02 * H)]))
    x0, x1 = int(ax - 0.06 * H), int(ax + 0.06 * H); y0, y1 = int(ay + 0.015 * H), int(ay + 0.12 * H)
    sub = L[y0:y1, x0:x1]
    d = nd.binary_opening(sub < f * Lh)
    lb, n = nd.label(d)
    if n == 0:
        return dict(L_haut=round(Lh, 1), dunkel_h_H=0)
    k = 1 + int(np.argmax(nd.sum(d, lb, range(1, n + 1))))
    m = lb == k
    ys, xs = np.nonzero(m)
    box = rgb[y0 + ys.min():y0 + ys.max() + 1, x0 + xs.min():x0 + xs.max() + 1]
    glanz = int((box.max(-1) > 170).sum())
    return dict(L_haut=round(Lh, 1), dunkel_b_H=round((xs.ptp() + 1) / H, 4), dunkel_h_H=round((ys.ptp() + 1) / H, 4),
                dunkel_flaeche_H2=round(m.sum() / H / H * 1e4, 2), glanz_px=glanz, glanz_je_H2=round(glanz / H / H * 1e4, 2),
                lab_dunkel=r1(A[y0:y1, x0:x1][m].mean(0)), bbox=[int(x0 + xs.min()), int(y0 + ys.min()), int(x0 + xs.max()), int(y0 + ys.max())])


def ki(h):
    im = b2.ki(h)[0]
    n, ox, oy = b2.KI_MAP[int(b2.KI_VON_H[h])]
    ok, so = KI_OK[h]
    ax, ay = KI_AUGE[h][0] + ox, KI_AUGE[h][1] + oy
    z = zone(im, ax, ay, so - ok)
    for f in (0.25, 0.35):
        zz = zone(im, ax, ay, so - ok, f=f); z['schwarz_%.2f' % f] = (zz.get('dunkel_h_H', 0), zz.get('dunkel_flaeche_H2', 0))
    return z


def port(emd, h):
    mo = b2.modell(emd)
    im, ids = b2.zeichne(mo, PV[str(h)], None)
    ok, so = b2.ok_sohle(ids, ausser=tuple(range(16, 23)))
    im2 = cv2.resize(im, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    ids2 = cv2.resize(ids.astype(np.int16), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    H = 2 * (so - ok)
    L = nd.gaussian_filter(lab(im2.astype(float))[..., 0], 1)
    k9 = ids2 == 9
    ys = np.nonzero(k9.any(1))[0]
    ob = k9.copy(); ob[int(ys.min() + 0.45 * (ys.max() - ys.min())):] = False
    Lm = np.where(ob, L, 999)
    ay, ax = np.unravel_index(np.argmin(Lm), Lm.shape)
    z = zone(im2, ax, ay, H); z['auge_px'] = [int(ax), int(ay)]; z['H'] = int(H)
    for f in (0.25, 0.35):
        zz = zone(im2, ax, ay, H, f=f); z['schwarz_%.2f' % f] = (zz.get('dunkel_h_H', 0), zz.get('dunkel_flaeche_H2', 0))
    # Gesichtsfeld gegen Nachbarhaut (wie n2_ki_kopf.vorn): Feld x +-0,035 H, y -0,035..+0,02 H; Nachbarn x +-(0,09..0,15) H
    Lf = lab(im2.astype(float))[..., 0]
    sl = slice(int(ay - 0.035 * H), int(ay + 0.02 * H) + 1)
    feld = Lf[sl, int(ax - 0.035 * H):int(ax + 0.035 * H) + 1]
    nb = []
    for s in (-1, 1):
        xa, xb = sorted((int(ax + s * 0.09 * H), int(ax + s * 0.15 * H)))
        nb.append(Lf[sl, xa:xb + 1][ids2[sl, xa:xb + 1] >= 0])
    nb = np.concatenate(nb)
    z['feld_L'] = round(float(feld.mean()), 1); z['nachbar_L'] = round(float(nb.mean()), 1)
    z['feld_nachbar'] = round(float(feld.mean() / nb.mean()), 3)
    return z


if __name__ == '__main__':
    erg = {}
    for h in (118, 125):
        try:
            erg['KI h%d' % h] = ki(h)
        except Exception as e:
            erg['KI h%d' % h] = repr(e)
        for p in sys.argv[1:]:
            erg['%s h%d' % (os.path.basename(p)[:-4], h)] = port(p, h)
    for k, v in erg.items():
        print(k, v)
