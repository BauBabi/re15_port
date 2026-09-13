# -*- coding: utf-8 -*-
"""Welche Kamerawinkel haben noch KEINE Vordergrund-Maske?

Der Nutzer stellt die Vordergrund-Objekte von Hand frei (auswahl.json-Hinweis vom
2026-09-04), der Port baut sie ein. Diese Liste sagt, wo noch etwas fehlt - und ob
das ORIGINAL fuer den Cut selbst Masken traegt (dann gaebe es eine Vorlage).
"""
import os
import sys
import glob
import struct

sys.path.insert(0, 're15_port/tools/maske')
import geom

raeume = {}
for p in glob.glob('pri/STAGE*/*/'):
    rid = os.path.basename(os.path.normpath(p))
    cuts = set()
    for f in glob.glob(os.path.join(p, '*.png')):
        basis = os.path.basename(f).split('.')[0]
        cuts.add(int(basis.split('_')[0], 10))
    raeume[rid] = cuts

print('%-9s %5s  %-26s %-22s %s'
      % ('Raum', 'Cuts', 'hat Freistellung', 'FEHLT', 'Original'))
print('-' * 88)
for rid in sorted(raeume):
    room = 'ROOM' + rid
    rdt, st = geom.load_rdt('re15_port/shared_assets/PSX', room)
    if rdt is None:
        print('%-9s  RDT fehlt' % room)
        continue
    n = rdt[1]
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    orig = [c for c in range(n) if geom.original_has_masks(rdt, cam, c)]
    da = sorted(raeume[rid])
    fehlt = [c for c in range(n) if c not in raeume[rid]]
    print('%-9s %5d  %-26s %-22s %s'
          % (room, n,
             ','.join(str(c) for c in da),
             ','.join(str(c) for c in fehlt) if fehlt else '-',
             ('traegt Masken in ' + ','.join(str(c) for c in orig)) if orig else 'keine'))
