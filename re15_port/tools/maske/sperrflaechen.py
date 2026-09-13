# -*- coding: utf-8 -*-
"""Grundflaeche einer Vordergrund-Maske in WELTkoordinaten.

Fuer jede Bildspalte der Maske den untersten Punkt nehmen und ihn durch die Kamera auf
die Bodenebene zurueckrechnen (geom.welt_am_boden). Das ergibt die Standlinie des
Gegenstands - die Flaeche, die blockieren muss, damit der Spieler nicht dahinter laeuft.
"""
import sys, os, struct, json
sys.path.insert(0, 're15_port/tools/maske')
import numpy as np
import geom, maske_aus_png
from geom import load_bg

ROOM = sys.argv[1] if len(sys.argv) > 1 else 'ROOM11F0'
CUT = int(sys.argv[2]) if len(sys.argv) > 2 else 6
rid = int(ROOM[4:], 16)
rdt, st = geom.load_rdt('re15_port/shared_assets/PSX', ROOM)
cam = struct.unpack_from('<I', rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)
bg = load_bg('build/bg_ppm', rid, CUT)

aus = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json', encoding='utf-8'))
objs = aus.get(ROOM, {}).get(str(CUT), {}).get('objekte', [])
print('%s Cut %d: %d Objekte' % (ROOM, CUT, len(objs)))
for o in objs:
    if 'png' in o:
        r = maske_aus_png.setze(o['png'], o['x'], o['y'], o.get('massstab', 1))
    else:
        print('  %s: kein PNG - uebersprungen' % o.get('name')); continue
    if r is None:
        print('  %s: keine Region' % o.get('name')); continue
    pts = []
    for sx in range(320):
        sp = np.nonzero(r[:, sx])[0]
        if not len(sp): continue
        sy = int(sp.max())
        w = geom.welt_am_boden(R, t, H, sx, sy, 0)
        if w is None: continue
        wx, wz = int(w[0]), int(w[1])
        if abs(wx) > 60000 or abs(wz) > 60000: continue
        pts.append((wx, wz))
    if not pts:
        print('  %s: keine Bodenpunkte' % o.get('name')); continue
    xs = [p[0] for p in pts]; zs = [p[1] for p in pts]
    print('  %-28s %4d Punkte  x %7d..%7d   z %7d..%7d'
          % (o.get('name', '?')[:28], len(pts), min(xs), max(xs), min(zs), max(zs)))
    # Median-Streifen: die Punkte sind eine LINIE, kein Block - je 20 Spalten ein Segment
    seg = {}
    for (wx, wz) in pts:
        k = wx // 1000
        seg.setdefault(k, []).append(wz)
    print('       Standlinie (x-Kachel 1000 -> z-Bereich):')
    for k in sorted(seg):
        v = seg[k]
        print('         x %6d..%6d   z %7d..%7d  (%d Punkte)'
              % (k*1000, k*1000+999, min(v), max(v), len(v)))
