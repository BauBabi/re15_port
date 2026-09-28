# -*- coding: utf-8 -*-
"""Nachtrag K: Platz im Kuppelfach neben der Sicherung (Plattform-Koordinaten).
Liest ROOM1150.RDT/ROOM1151.RDT Prop 0/1/2 (MD1) und die eingebackene Sicherung
(gen/sicherung_prop.inc), gibt Fachboden, Sicherungs-Huelle und die Kuppel-Innenhoehe
ueber zwei Kandidaten-Streifen aus.
"""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import props, rdt_laden, md1_lesen, inc_bytes  # noqa

for raum in ('ROOM1150.RDT', 'ROOM1151.RDT'):
    rdt = rdt_laden(raum)
    pr = props(rdt)
    print(raum, 'nOmodel', rdt[2])
    for k in range(3):
        m = md1_lesen(pr[k]['md1'])['meshes'][0]
        pts = m['tv']
        xs = [p[0] for p in pts]; ys = [p[1] for p in pts]; zs = [p[2] for p in pts]
        print('  Prop %d MD1 @0x%X: %d Punkte  x%d..%d y%d..%d z%d..%d' % (k, pr[k]['md1_off'], len(pts), min(xs), max(xs), min(ys), max(ys), min(zs), max(zs)))
    # Fachboden: alle Punkte mit y == -1036
    m0 = md1_lesen(pr[0]['md1'])['meshes'][0]
    boden = sorted({p for p in m0['tv'] if p[1] == -1036})
    print('  Fachboden y=-1036:', boden)
    # Kuppel (Prop 1/2): tiefste Innenpunkte = kleinste |y| ueber dem Fach? -> y-Werte in den Streifen
    for k in (1, 2):
        m = md1_lesen(pr[k]['md1'])['meshes'][0]
        for (xa, xb) in ((-440, -300), (-260, -120)):
            ys = [p[1] for p in m['tv'] if xa <= p[0] <= xb]
            if ys:
                print('    Prop %d Punkte mit x in [%d,%d]: y %d..%d (%d Punkte)' % (k, xa, xb, min(ys), max(ys), len(ys)))
s = md1_lesen(inc_bytes('re15_sicherung_md1'))['meshes'][0]['tv']
print('Sicherung (Modellraum): x%d..%d y%d..%d z%d..%d' % (min(p[0] for p in s), max(p[0] for p in s), min(p[1] for p in s), max(p[1] for p in s), min(p[2] for p in s), max(p[2] for p in s)))
