#!/usr/bin/env python3
"""Variante K, Schritt 1b: Wo liegt der Kopf von J in Bild 2 und Bild 1, was liegt unter Ober- und Unterkiefer der Nutzer-Teile."""
import json
import os

import cv2
import numpy as np

import k_kern as KK

form, mo, fj = KK.lade()
erg = {}
ok, uk = KK.maske('oberkiefer'), KK.maske('unterkiefer')
for n in (1, 2):
    s = fj['bild%d' % n]
    kam = KK.kamera(n)
    kf = KK.kf_von(mo, s['clip'], s['bild'])
    R, T = mo.pose(kf)
    Rw, Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
    tris, ids = KK.tris_welt(form, Rw, Tw)
    tief, idb = KK.raster(kam, tris, ids)
    np.save(os.path.join(KK.K_DIR, '_lauf', '_tiefe_J_bild%d.npy' % n), tief)
    np.save(os.path.join(KK.K_DIR, '_lauf', '_id_J_bild%d.npy' % n), idb)
    pk = {}
    for b in (10, 16, 17, 18, 21, 22, 23):
        p = KK.FK.projiziere(kam, Tw[b])
        pk[b] = dict(welt=[round(float(x)) for x in Tw[b]], bild=[round(float(x), 1) for x in p], tiefe=round(KK.FK.tiefe(kam, Tw[b])))
    e = dict(knochen=pk)
    if n == 2:
        for nm, m in (('oberkiefer', ok), ('unterkiefer', uk)):
            u, c = np.unique(idb[m], return_counts=True)
            e[nm] = dict(knochen={int(a): int(b) for a, b in zip(u, c)}, tiefe_median=round(float(np.median(tief[m][np.isfinite(tief[m])]))))
        # Kopf und Kiefer von J allein: wo liegen sie
        for nm, kn in (('kopf_J', (22,)), ('kiefer_J', (23,))):
            mm = np.isin(idb, kn)
            ys, xs = np.nonzero(mm)
            e[nm] = dict(px=int(mm.sum()), box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                         iou_ok=round(KK.iou(mm, ok), 3), iou_uk=round(KK.iou(mm, uk), 3))
    else:
        for nm, kn in (('kopf_J', (21, 22, 23)),):
            mm = np.isin(idb, kn)
            e[nm] = dict(sichtbar_px=int(mm.sum()))
    erg['bild%d' % n] = e
print(json.dumps(erg, indent=1))
json.dump(erg, open(os.path.join(KK.K_DIR, 'lage_J.json'), 'w'), indent=1)
