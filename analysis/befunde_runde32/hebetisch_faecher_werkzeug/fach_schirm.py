# -*- coding: utf-8 -*-
"""R32 PLANUNG: Ecken der beiden unteren Faecher von Prop 0 auf dem Schirm (Cut 4) je Plattform-y.
Massgeblich ist die Framedump-Messung; das hier dient nur der Planung."""
import os, sys
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, cuts, view_bauen, prop_rot, view_x_welt, projiziere  # noqa
rdt = rdt_laden(sys.argv[1] if len(sys.argv) > 1 else 'ROOM1150.RDT')
c4 = cuts(rdt)[4]
print('Cut 4 @0x%X pos %s tgt %s fov %d' % (c4['off'], c4['pos'], c4['tgt'], c4['fov']))
v = view_bauen(c4)
FA = dict(A=(96, 861, -90), B=(950, 1715, -91))
for py in (-305, -505, -705, -905, -1105, -1205, -1215):
    cr, ct = view_x_welt(v, prop_rot(0, 2048, 0), (-20700, py, -17460))
    s = 'y=%5d ' % py
    for k, (z0, z1, fy) in FA.items():
        pts = {}
        for nm, p in (('vu0', (2, fy, z0)), ('vu1', (2, fy, z1)), ('ho0', (-1258, fy, z0)), ('ho1', (-1258, fy, z1)),
                      ('vo0', (2, fy - 720, z0)), ('vo1', (2, fy - 720, z1))):
            pts[nm] = projiziere(v, cr, ct, p)
        s += ' %s: ' % k + ' '.join('%s=(%d,%d)' % (n, q[0], q[1]) for n, q in pts.items() if q)
    # Tischkante vorne oben y=-901 x=2
    t = projiziere(v, cr, ct, (2, -901, 905))
    s += ' | Platte vorn (%d,%d)' % (t[0], t[1])
    print(s)
