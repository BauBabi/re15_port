"""Belegbilder N2_linien_<Modell>.png (x3): links Atlas roh, rechts Atlas x 0,5 mit den gezaehlten dunklen Linien (n2_mass.linien_stat,
Riegel B10/B11/B12) in Cyan und den Rotflaechen-Umrissen in Gelb; nur die Messzone (Koerper bzw. Haut), Rest schwarz."""
import os, sys
import numpy as np, cv2
from scipy import ndimage as ND
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import n2_mass as N
from p_messen import atlas as p_atlas


def bild(mo, zf, pfad, kopf=None):
    per, _tri = N.PL.sammeln(mo)
    rgb, mid = p_atlas(mo, per)
    u = mid >= 0
    L = N.lab_map(rgb, u)
    mmin = ND.minimum_filter(np.where(u, mid, -1), 3, mode='constant', cval=-1)
    mmax = ND.maximum_filter(np.where(u, mid, 10 ** 6), 3, mode='constant', cval=10 ** 6)
    valid = u & (mmin == mid) & (mmax == mid)
    L0f = N.fuellen(np.nan_to_num(L[..., 0]), u)
    zone = zf(u, mid)
    if kopf is not None:
        zone = zone | kopf(u, mid)
    _s, lin = N.linien_stat(L0f, valid, zone)
    rot = N.rotklasse(np.nan_to_num(L)) & zone
    rr = rot & ~cv2.erode(rot.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    a = rgb.copy()
    a[~zone] = 0
    b = (rgb.astype(float) * 0.5).astype(np.uint8)
    b[~zone] = 0
    b[rr] = (255, 220, 0)
    b[lin] = (0, 255, 255)
    z = np.zeros((rgb.shape[0], 6, 3), np.uint8) + 128
    out = cv2.resize(np.concatenate([a, z, b], 1), None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(pfad, out[:, :, ::-1])


if __name__ == '__main__':
    import fl
    import kf_tafeln as KT
    Z = N.zonen()
    for nm, mo, zf, zk in (('EM30', fl.lade_em30(), Z['em30'], Z['em30kopf']), ('G4', KT.re2_mo(0x34), Z['g4'], Z['g4kopf']),
                           ('G5', KT.re2_mo(0x36), Z['g5'], None), ('r3', fl.lade_var(N.K5 + '/farbe/K5c_farbe'), Z['k5'], Z['k5kopf']),
                           ('K4', fl.lade_var(N.K5 + '/K4_heute'), Z['k5'], Z['k5kopf'])):
        bild(mo, zf, os.path.join(N.FL, 'N2_linien_%s.png' % nm), zk)
        print(nm)
