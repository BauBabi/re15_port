# -*- coding: utf-8 -*-
"""Biegeprobe: Ruhepose, je Gelenk eine kraeftige Beugung, Lehm von der Seite und von oben. Starre Teile muessen ueberlappen, durch den Rumpf darf man nicht hindurchsehen."""
import copy, math, os
import numpy as np
import cv2
import figur, schau

g = lambda d: int(round(d * 4096 / 360.))
f = figur.bauen()
fig = figur.fertig(f)
ruhe = fig['posen']['ruhe']
PROBEN = [('Ruhe', {}), ('Wirbel +20', {'wirbel': (0, 0, g(20))}), ('Wirbel -20', {'wirbel': (0, 0, g(-20))}), ('Brust +25', {'brust': (0, 0, g(25))}), ('Brust -30', {'brust': (0, 0, g(-30))}),
          ('Brust seitlich 25', {'brust': (0, g(25), 0)}), ('Huefte/Wirbel seitlich 20', {'wirbel': (0, g(20), 0)}), ('Kopf hoch 35', {'hals': (0, 0, g(-20)), 'kopf': (0, 0, g(-15))}), ('Maul auf', {'kiefer': (0, 0, g(40))}),
          ('Arm vor 70', {'oberarm_r': (0, 0, g(-70))}), ('Arm zurueck 40', {'oberarm_r': (0, 0, g(40))}), ('Ellbogen 90', {'unterarm_r': (0, 0, g(-90))}), ('Knie 90', {'unterschenkel_r': (0, 0, g(90))}), ('Bein vor 50', {'oberschenkel_r': (0, 0, g(-50))}), ('Schwanz seitlich', {'schwanz1': (0, g(35), 0), 'schwanz2': (0, g(35), 0), 'schwanz3': (0, g(30), 0)})]
zeilen, reihe = [], []
for nm, d in PROBEN:
    p = copy.deepcopy(ruhe)
    for k, (ax, ay, az) in d.items():
        w = p['winkel'][f.ix[k]]
        p['winkel'][f.ix[k]] = [w[0] + ax, w[1] + ay, w[2] + az]
    a = schau.ansicht(fig, p, 0, 0, W=420, H=250, sk=.042)
    b = schau.ansicht(fig, p, 0, 89, W=420, H=250, sk=.042)
    c = schau.ansicht(fig, p, 0, -89, W=420, H=250, sk=.042)
    z = np.hstack([a, b, c])
    cv2.putText(z, nm, (6, 16), 0, .45, (255, 255, 255), 1, cv2.LINE_AA)
    reihe.append(z)
    if len(reihe) == 5:
        zeilen.append(np.vstack(reihe))
        reihe = []
if reihe:
    zeilen.append(np.vstack(reihe))
h = max(z.shape[0] for z in zeilen)
zeilen = [np.vstack([z, np.zeros((h - z.shape[0], z.shape[1], 3), np.uint8)]) for z in zeilen]
cv2.imwrite(os.path.join(schau.HIER, 'lauf', 'biege.png'), np.hstack(zeilen))
