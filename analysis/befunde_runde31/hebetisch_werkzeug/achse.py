# -*- coding: utf-8 -*-
"""R31 Hebetisch: welche Plattform-Achse ist in Cut 4 links/rechts? (Rechnung, NUR zur Planung;
massgeblich ist die Framedump-Messung). Plattform rot_y 2048, Lage (-20700, y, -17460)."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, cuts, view_bauen, prop_rot, view_x_welt, projiziere  # noqa
rdt = rdt_laden('ROOM1150.RDT')
c4 = cuts(rdt)[4]
print('Cut 4 @0x%X pos %s tgt %s fov %d' % (c4['off'], c4['pos'], c4['tgt'], c4['fov']))
v = view_bauen(c4)
for py in (-305, -1205):
    cr, ct = view_x_welt(v, prop_rot(0, 2048, 0), (-20700, py, -17460))
    for p in ((-280, -1062, 1110), (-280, -1062, 1260), (-280, -1062, 1410), (-74, -1062, 1260), (-485, -1062, 1260)):
        print('Plattform y=%d  lokal %s -> Schirm %s' % (py, p, projiziere(v, cr, ct, p)))
