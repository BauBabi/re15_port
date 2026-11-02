#!/usr/bin/env python3
"""Rohbau final, Biegeprobe: Knie, Ellbogen, Schultern, Hueften, Hals und Schwanz um +-35 Grad aus der Ruhepose, Blatt und Kennzahl.
Lochprobe: Abstand der Gelenkringe. Jedes Teil ist geschlossen, ein Loch kann nur entstehen, wenn zwei Nachbarteile sich trennen.
python rf_biege.py form.json aus.png"""
import copy
import json
import os
import sys

import cv2
import numpy as np

from rf_kern import HIER, namen
import form_emd as FE
import form_pruefung as P
import form_buehne as FB
import rf_plan


def probe(j, aus):
    k = P.Kandidat(j)
    d = os.path.dirname(os.path.abspath(j))
    form = FE.lese_emd(open(os.path.join(d, 'FORM.EMD'), 'rb').read())
    g, _ = rf_plan.plan()
    ix = namen(g)
    pool = form['pool']
    ruhe = pool[form['clips'][2]['bilder'][0]['kf']]
    Z = {}
    GEL = dict(oberschenkel=2, unterschenkel=2, oberarm=2, unterarm=2)

    def gebogen(vz):
        w = copy.deepcopy(ruhe['winkel'])
        for n, ax in (('oberschenkel_r', 2), ('oberschenkel_l', 2), ('unterschenkel_r', 2), ('unterschenkel_l', 2), ('oberarm_r', 2), ('oberarm_l', 2), ('unterarm_r', 2), ('unterarm_l', 2),
                      ('hals', 2), ('kopf', 2), ('schwanz2', 1), ('schwanz3', 1), ('schwanz4', 1), ('schwanz5', 1)):
            w[ix[n]][ax] = int(w[ix[n]][ax] + vz * 400)
        w[ix['wirbel']][2] += int(vz * 200)
        return w
    bilder = []
    for vz in (1, -1):
        f2 = copy.deepcopy(form)
        kf = dict(wurzel=ruhe['wurzel'], spur=ruhe['spur'], winkel=gebogen(vz))
        f2['pool'] = list(f2['pool']) + [kf]
        kfi = len(f2['pool']) - 1
        f2['clips'] = [dict(name='b', bilder=[dict(kf=kfi, wort=0)])]
        emd = FE.schreibe_emd(f2, pruefen=False)
        mo = FE.modell(emd, k.tim)
        weiss = FB.Textur(k.tim)
        weiss.lut = weiss.lut.copy()
        weiss.lut[..., :3] = 200
        for t, az, el in (('rechts', 0, 6), ('3/4', 40, 20), ('hinten', -120, 15)):
            a = np.full((640, 640, 3), 60, np.uint8)
            FB.rastern(P.rundum_dreiecke(mo, kfi, az, el, 640), weiss, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
            cv2.putText(a, '%s %+d Grad' % (t, vz * 35), (8, 20), 0, .55, (255, 255, 255), 1, cv2.LINE_AA)
            bilder.append(a)
    cv2.imwrite(aus, np.vstack([np.hstack(bilder[:3]), np.hstack(bilder[3:])])[:, :, ::-1])


if __name__ == '__main__':
    probe(sys.argv[1], sys.argv[2])
