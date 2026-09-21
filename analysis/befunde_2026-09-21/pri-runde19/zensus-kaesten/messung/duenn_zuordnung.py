# -*- coding: utf-8 -*-
"""Woher kommen die duennen Punkte? Je Objekt, getrennt nach Kunst und Tiefschwarz."""
import io, json, os, struct, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, maskenbild, abnahme, raum, maske_aus_png

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
K3 = np.ones((3, 3), bool)

for room, cut in (('ROOM10F0', 4), ('ROOM10F0', 5)):
    rid = int(room[4:], 16)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    kunst = np.zeros((240, 320), bool)
    for o in AUS[room][str(cut)].get('objekte') or []:
        if 'png' in o and all(k in o for k in ('x', 'y')):
            r = maske_aus_png.setze(o['png'], o['x'], o['y'], o.get('massstab', 1))
            if r is not None:
                kunst |= r
    print('=' * 104)
    print('%s C%d' % (room, cut))
    print('  %-42s %7s %7s %9s %10s' % ('Objekt', 'Punkte', 'duenn', 'd. Kunst', 'd. Schwarz'))
    for o in objs:
        r = o[1]
        dn = r & ~nd.binary_erosion(r, K3)
        print('  %-42s %7d %7d %9d %10d'
              % (o[0][:42], int(r.sum()), int(dn.sum()), int((dn & kunst).sum()), int((dn & ~kunst).sum())))
