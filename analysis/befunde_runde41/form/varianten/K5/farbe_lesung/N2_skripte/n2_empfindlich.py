"""Empfindlichkeit der Riegel B10-B16 gegen die Messkonstanten: Linien-Top-Hat-Schwelle 4/6/8 L*, Mindestgroesse Rotflaeche 10/20/40."""
import sys, os, json
import numpy as np, cv2
from scipy import ndimage as ND
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import n2_mass as N
from p_messen import atlas as p_atlas
import fl, kf_tafeln as KT
Z = N.zonen()
out = {}
for nm, mo, zf, zk, clip in (('EM30', fl.lade_em30(), Z['em30'], Z['em30kopf'], 0), ('G4', KT.re2_mo(0x34), Z['g4'], Z['g4kopf'], 0),
                             ('G5', KT.re2_mo(0x36), Z['g5'], None, 0), ('r3', fl.lade_var(N.K5 + '/farbe/K5c_farbe'), Z['k5'], Z['k5kopf'], 2)):
    per, _ = N.PL.sammeln(mo); rgb, mid = p_atlas(mo, per); u = mid >= 0
    L = N.lab_map(rgb, u)
    mmin = ND.minimum_filter(np.where(u, mid, -1), 3, mode='constant', cval=-1)
    mmax = ND.maximum_filter(np.where(u, mid, 10 ** 6), 3, mode='constant', cval=10 ** 6)
    valid = u & (mmin == mid) & (mmax == mid)
    L0f = N.fuellen(np.nan_to_num(L[..., 0]), u)
    zone = zf(u, mid)
    rot = N.rotklasse(np.nan_to_num(L)) & zone
    fleisch = cv2.dilate(rot.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & zone
    haut = zone & ~fleisch
    r = {}
    for thr in (4, 6, 8):
        r['lin%d' % thr] = (N.linien_stat(L0f, valid, fleisch, thr)[0]['anteil_p'], N.linien_stat(L0f, valid, haut, thr)[0]['anteil_p'],
                            N.linien_stat(L0f, valid, zk(u, mid), thr)[0]['anteil_p'] if zk else None)
    for rm in (10, 20, 40):
        N.ROT_MIN = rm
        rr = N.render_mass(mo, clip, nm)['rot_alle']
        r['rot%d' % rm] = (rr['sdL_pool'], rr['sda_pool'], rr['stufen_ge2_gew_p'])
    N.ROT_MIN = 20
    out[nm] = r
    print(nm, r)
    sys.stdout.flush()
json.dump(out, open(os.path.join(N.FL, 'N2_empfindlich.json'), 'w'), indent=1)
