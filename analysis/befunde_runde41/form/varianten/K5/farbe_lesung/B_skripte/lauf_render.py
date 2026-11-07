import json
import numpy as np, cv2
import fl, render as R
import k5blatt_hilfe as H

em = fl.lade_em30()
k5 = fl.lade_var(fl.K5 + '/K5c')
modelle = (('EM30_HANGAR (Clip 0 Bild 0)', em, R.kf(em, 0)), ('K5c (Clip 2 Ruhe Bild 0)', k5, R.kf(k5, 2)))
zeilen = []
mass = {}
for gamma, gt in ((None, 'roh, ohne Gamma'), (0.5, 'Gamma 0,5 wie alte Tafeln')):
    for nm, mo, k in modelle:
        ks = []
        for wt, az, el in R.WINKEL:
            img, msk = R.blick(mo, k, az, el, 380, gamma)
            mm = R.masse(img, msk)
            mass['%s | %s | %s' % (nm, gt, wt)] = mm
            bgr = img[:, :, ::-1].copy()
            H.schrift(bgr, '%s, %s' % (nm.split(' ')[0], wt), 6, 18, 0.45)
            H.schrift(bgr, gt, 6, 36, 0.42, (200, 230, 255))
            H.schrift(bgr, 'L* %.1f  C* %.1f  L*>70 %.1f%%  L*>85 %.2f%%' % (mm['L'], mm['C'], mm['hell70'], mm['hell85']), 6, 372, 0.4, (230, 230, 120))
            ks.append(bgr)
        zeilen.append(H.reihe(ks))
r = H.stapel(zeilen)
kopf = H.kopfzeile('B_6 Render-Gegenprobe: EM30_HANGAR (gelobter Stand) und K5c, gleicher Renderer, gleiches neutrales Licht', r.shape[1], g=0.5)
kopf2 = H.kopfzeile('(0,55 + 0,45 N*L, weiss), gleiche Groesse (Umkugel). Zeile 1-2 roh, Zeile 3-4 Gamma 0,5 fuer BEIDE. Masse nur Figurpixel.', r.shape[1], g=0.5)
cv2.imwrite(fl.OUT + '/B_6_render_gegenprobe_em30_k5c.png', H.stapel([kopf, kopf2, r]))
json.dump(mass, open(fl.S + '/render_mass.json', 'w'), indent=1, ensure_ascii=False)
for k, v in mass.items():
    print(k, v)
