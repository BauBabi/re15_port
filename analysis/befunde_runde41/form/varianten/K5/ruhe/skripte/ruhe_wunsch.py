#!/usr/bin/env python3
"""Wunsch- und Herkunfts-Masse fuer den Riegel-Entwurf K5e (gleich fuer K5c, K5d und die Plan-Vorschau).
Albedo je Flaeche = Lab der Texel im UV-Polygon (fillPoly + polylines, wie riegel_k5d.flaechen_alb) ueber die CLUT-Zeile der Flaeche.
Weisses Licht = rl.weiss_id (Tafel 07). Flaechen-Schluessel (mesh, viereck, fi) sind fuer K5c, K5d und K5e gleich (Eckindizes gleich)."""
import json
import os

import cv2
import numpy as np

import rl
import ruhe_mass as M
import ruhe_material as RM

ROT = dict(a=15.0, C=15.0)      # Albedo-Rotklasse Lippe (Messblatt V4F 2.4: a* > 15, C* > 15)


def mat():
    mj = json.load(open(os.path.join(rl.OUT, 'material.json'), encoding='utf-8'))
    return {nm: [tuple(int(x) for x in s.split('|')) for s in v['liste']] for nm, v in mj['material'].items()}


def albedo(F, zeile_je, worte):
    c = np.array(worte, np.int64)
    rgb = np.stack([c & 31, (c >> 5) & 31, (c >> 10) & 31], -1).astype(float) * 255.0 / 31.0
    labz = RM.lab(rgb.reshape(-1, 3)).reshape(rgb.shape)
    return {k: labz[zeile_je[k]][d['idx']] for k, d in F.items()}


def stand_albedo(ordner):
    F, _form, tim = RM.flaechen(ordner)
    return albedo(F, {k: d['zeile'] for k, d in F.items()}, tim['cluts']), F


def pool(A, keys):
    x = [A[k] for k in keys if k in A and len(A[k])]
    return np.vstack(x) if x else np.zeros((0, 3))


def weiss_gelenk(mo, clip, gelenk, glieder):
    """dL* Gelenkkoerper gegen angrenzendes Glied (Ring 6 px um die Gelenkpixel, nur Pixel der Glied-Flaechen derselben Meshes), 4 Winkel."""
    gs = set(gelenk)
    gm = set(k[0] for k in gelenk)
    ls = set(k for k in glieder if k[0] in gm)
    ker = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (13, 13))
    out = []
    for wn, az, el in rl.WINKEL4:
        img, K = rl.weiss_id(mo, clip, az, el)
        L = M.lab(img)[..., 0]
        fk = K['flaeche']
        me = K['mesh']
        istg = np.zeros(me.shape, bool)
        istl = np.zeros(me.shape, bool)
        ys, xs = np.nonzero(K['maske'])
        for y, x in zip(ys, xs):
            k = (int(me[y, x]), int(fk[y, x] // 100000), int(fk[y, x] % 100000))
            if k in gs:
                istg[y, x] = True
            elif k in ls:
                istl[y, x] = True
        ring = (cv2.dilate(istg.astype(np.uint8), ker) > 0) & istl
        if istg.sum() >= 20 and ring.sum() >= 20:
            out.append(dict(winkel=wn, dL=round(float(L[istg].mean() - L[ring].mean()), 2), px_gelenk=int(istg.sum()), px_ring=int(ring.sum())))
    return out


def wuensche(A, mo, clip, mt):
    """A = Albedo je Flaeche; mo = Modell fuer weisses Licht."""
    e = {}
    g = pool(A, mt['Gelenk'])
    haut_glieder = [k for k in mt['Haut'] if k[0] in set(k2[0] for k2 in mt['Gelenk'])]
    nb = pool(A, haut_glieder)
    e['gelenk_albedo'] = dict(gelenk=np.round(np.median(g, 0), 1).tolist(), nachbar=np.round(np.median(nb, 0), 1).tolist(),
                              dL_median=round(float(np.median(g[:, 0]) - np.median(nb[:, 0])), 2),
                              C_gelenk=round(float(np.median(np.hypot(g[:, 1], g[:, 2]))), 2))
    wg = weiss_gelenk(mo, clip, mt['Gelenk'], haut_glieder)
    e['gelenk_weiss'] = dict(je_winkel=wg, dL_mittel=round(float(np.mean([x['dL'] for x in wg])), 2) if wg else None,
                             dL_min=round(float(min(x['dL'] for x in wg)), 2) if wg else None)
    sp = pool(A, mt['Fleisch_Schwanzspitze'])
    sw = pool(A, [k for k in mt['Haut'] if k[0] in (1, 2, 3, 4, 5)])
    rot = lambda x: float(100 * ((x[:, 1] >= 12) & (np.hypot(x[:, 1], x[:, 2]) >= 15)).mean()) if len(x) else 0.0
    e['schwanzspitze'] = dict(albedo_median=np.round(np.median(sp, 0), 1).tolist(), fleisch_anteil=round(rot(sp), 1),
                              schwanz_albedo_median=np.round(np.median(sw, 0), 1).tolist(), dL=round(float(np.median(sp[:, 0]) - np.median(sw[:, 0])), 2))
    bu = pool(A, mt['Fleisch_Buckel'])
    b10 = pool(A, [k for k in mt['Haut'] if k[0] == 10])
    bm = np.median(bu, 0)
    e['buckel'] = dict(albedo_median=np.round(bm, 1).tolist(), fleisch_anteil=round(rot(bu), 1), flaechen=len(mt['Fleisch_Buckel']),
                       mesh10_haut_median=np.round(np.median(b10, 0), 1).tolist(), dL=round(float(bm[0] - np.median(b10[:, 0])), 2),
                       ton=round(float(np.degrees(np.arctan2(bm[2], bm[1]))), 1))
    li = pool(A, mt['Lippensaum'] + [k for k in mt['Maul_Band'] if k not in mt['Lippensaum']])
    hp = pool(A, mt['Haut'])
    e['lippe_unterkiefer'] = dict(rot_pct=round(float(100 * ((li[:, 1] > ROT['a']) & (np.hypot(li[:, 1], li[:, 2]) > ROT['C'])).mean()), 2),
                                  L_p95=round(float(np.percentile(li[:, 0], 95)), 1), haut_L_p95=round(float(np.percentile(hp[:, 0], 95)), 1),
                                  median=np.round(np.median(li, 0), 1).tolist())
    kr = pool(A, [k for k in mt['Kralle']])
    hand = pool(A, [k for k in mt['Haut'] if k[0] in (13, 18)])
    km, hm = np.median(kr, 0), np.median(hand, 0)
    e['krallen'] = dict(kralle=np.round(km, 1).tolist(), hand=np.round(hm, 1).tolist(), dE=round(float(np.linalg.norm(km - hm)), 2),
                        a_gt10_pct=round(float(100 * (kr[:, 1] > 10).mean()), 2),
                        rotklasse_pct=round(float(100 * ((kr[:, 1] > ROT['a']) & (np.hypot(kr[:, 1], kr[:, 2]) > ROT['C'])).mean()), 2))
    return e
