# -*- coding: utf-8 -*-
"""R31: Punkte der Kuppelhaelften (Prop 1/2) und alle Punkte von Prop 0 ueber dem Fachboden
(y < -1036) im Fach-Grundriss — ROOM1150.RDT (nur LESEN)."""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, props, md1_lesen  # noqa
rdt = rdt_laden(sys.argv[1] if len(sys.argv) > 1 else 'ROOM1150.RDT')
pr = props(rdt)
for k in (1, 2):
    m = md1_lesen(pr[k]['md1'])['meshes']
    for mi, mm in enumerate(m):
        print('Prop %d mesh %d: %d Punkte, %d Dreiecke, %d Vierecke' % (k, mi, len(mm['tv']), len(mm['tris']), len(mm['quads'])))
        for i, p in enumerate(mm['tv']):
            print('   %2d %s' % (i, p))
        print('   tris', mm['tris']); print('   quads', mm['quads'])
m0 = md1_lesen(pr[0]['md1'])['meshes']
print('Prop 0: %d Meshes' % len(m0))
for mi, mm in enumerate(m0):
    hoch = [(i, p) for i, p in enumerate(mm['tv']) if p[1] < -1036]
    print(' mesh %d: %d Punkte, davon %d ueber dem Fachboden' % (mi, len(mm['tv']), len(hoch)))
    for i, p in hoch:
        print('   %3d %s' % (i, p))
