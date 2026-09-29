# -*- coding: utf-8 -*-
"""R32: vollstaendige Punkt-/Flaechenliste von Prop 0 (Hebetisch) mit Datei-Offsets (nur LESEN)."""
import os, sys, struct
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, props, md1_lesen  # noqa
raum = sys.argv[1] if len(sys.argv) > 1 else 'ROOM1150.RDT'
rdt = rdt_laden(raum)
pr = props(rdt)
p0 = pr[0]
base = p0['md1_off']
md = md1_lesen(p0['md1'])
print('%s Prop 0 MD1 @0x%X, %d Bytes, nobj %d' % (raum, base, len(p0['md1']), md['nobj']))
for mi, m in enumerate(md['meshes']):
    k = m['kopf']
    print('mesh %d: tv@0x%X (%d)  tris %d  qv@0x%X (%d)  quads %d' % (mi, base + 12 + k[0], k[1], k[5], base + 12 + k[7], k[8], k[12]))
    for i, p in enumerate(m['tv']):
        print('  tv %3d @0x%05X %s' % (i, base + 12 + k[0] + i * 8, p))
    for i, p in enumerate(m['qv']):
        print('  qv %3d @0x%05X %s' % (i, base + 12 + k[7] + i * 8, p))
    for i, t in enumerate(m['tris']):
        print('  T %3d @0x%05X %s  %s' % (i, base + 12 + k[4] + i * 12, t, [m['tv'][j] for j in t]))
    for i, q in enumerate(m['quads']):
        print('  Q %3d @0x%05X %s  %s' % (i, base + 12 + k[11] + i * 16, q, [m['qv'][j] for j in q]))
