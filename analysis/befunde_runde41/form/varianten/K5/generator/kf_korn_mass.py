"""Feinkorn der Haut (Quelle KORN_SD in kf_mal4.py, Versuch 2 Runde 2): L*-Rest gegen eine Gauss-Glaettung sigma s (1,0 und 2,5 Texel, im selben
Mesh normiert) an den gueltigen Haut-Texeln (Zone wie n2_mass: ohne Rotflaechen + 2 Texel, 3x3 im selben Mesh), Streuung und p90 |Rest|.
Familie EM30-Haut / RE2-G4 / RE2-G5 (Zonen n2_mass.zonen), dazu beliebige Varianten (Koerper und Kopffeld).

    python kf_korn_mass.py <aus.json> [<Variantenordner> ...]"""
import json
import os
import sys
import numpy as np, cv2
N2 = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'farbe_lesung', 'N2_skripte'))
sys.path.insert(0, N2)
import n2_mass as N
from scipy import ndimage as ND
import fl
sys.path.insert(0, os.path.join(N.FL, 'B_skripte'))
import kf_tafeln as KT

def mass(mo, zf, s_list=(1.0, 2.5)):
    per, tri = N.PL.sammeln(mo)
    rgb, mid = N.p_atlas(mo, per)
    u = mid >= 0
    L = N.lab_map(rgb, u)
    zone = zf(u, mid)
    mmin = ND.minimum_filter(np.where(u, mid, -1), 3, mode='constant', cval=-1)
    mmax = ND.maximum_filter(np.where(u, mid, 10 ** 6), 3, mode='constant', cval=10 ** 6)
    valid = u & (mmin == mid) & (mmax == mid)
    _f, rot = N.rot_flaechen(L, zone, u & zone)
    fleisch = cv2.dilate(rot.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & zone
    haut = zone & ~fleisch & valid
    L0 = np.nan_to_num(L[..., 0])
    out = {}
    for s in s_list:
        R = np.zeros_like(L0)
        for m in np.unique(mid[u]):
            mm = (mid == m).astype(np.float32)
            W = cv2.GaussianBlur(mm, (0, 0), s)
            B = cv2.GaussianBlur((L0 * mm).astype(np.float32), (0, 0), s) / np.maximum(W, 1e-6)
            R = np.where(mid == m, L0 - B, R)
        out['sd_rest_s%.1f' % s] = round(float(R[haut].std()), 2)
        out['p90abs_s%.1f' % s] = round(float(np.percentile(np.abs(R[haut]), 90)), 2)
    out['haut_texel'] = int(haut.sum())
    return out

Z = N.zonen()
res = {}
for nm, mo, zf in (('EM30', fl.lade_em30(), Z['em30']), ('G4', KT.re2_mo(0x34), Z['g4']), ('G5', KT.re2_mo(0x36), Z['g5'])):
    res[nm] = mass(mo, zf)
for p in sys.argv[2:]:
    res[os.path.basename(p)] = mass(fl.lade_var(p), Z['k5'])
    res[os.path.basename(p) + '_kopf'] = mass(fl.lade_var(p), Z['k5kopf'])
print(json.dumps(res, indent=1))
json.dump(res, open(sys.argv[1], 'w', encoding='utf-8'), indent=1)
