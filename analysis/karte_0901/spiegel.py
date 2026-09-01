# -*- coding: utf-8 -*-
"""'Auf beiden Seiten eingezeichnet' woertlich nehmen: liegt zu einer Port-Marke ein
GEMALTES Symbol auf der GEGENUEBERLIEGENDEN Seite derselben Wandlinie?"""
import sys, os, math, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S
OUT = []
def P(*a):
    s = ' '.join(str(x) for x in a); print(s); OUT.append(s)

WAND = {0: 'N', 1: 'E', 2: 'S', 3: 'W'}
GEG = {'N': 'S', 'S': 'N', 'E': 'W', 'W': 'E'}
tu = [m for m in MARKS if m[4] <= 3]
P("Tuermarken: %d   gemalte Symbole: %d" % (len(tu), len(SYM)))
P("")
P("Fuer jede Marke das naechste gemalte Symbol auf derselben Seite (Mittelpunktabstand):")
kl = collections.Counter(); zeilen = []
for m in tu:
    pg, r, mx, my, kind = m[0], m[1], m[2], m[3], m[4]
    best = None
    for g in SYM_PAGE.get(pg, ()):
        d = math.hypot(mx - g['x'], my - g['y'])
        if best is None or d < best[0]: best = (d, g)
    if best is None: kl['keine Symbole auf dieser Seite'] += 1; continue
    d, g = best
    mw = WAND[kind]; gw = g['wand']
    if d <= 8:
        art = ('SPIEGELWAND (%s gegen %s)' % (mw, gw)) if GEG.get(mw) == gw else \
              ('GLEICHE WAND (%s == %s)' % (mw, gw)) if mw == gw else \
              ('quer (%s gegen %s)' % (mw, gw))
        kl['d<=8 px: ' + art.split(' (')[0]] += 1
        zeilen.append((d, m, g, art))
    elif d <= 16: kl['d 9..16 px'] += 1
    else: kl['d > 16 px'] += 1
for k, n in sorted(kl.items(), key=lambda t: -t[1]): P("   %-34s %3d" % (k, n))
P("")
P("   Die Faelle d<=8 px im Detail (Marke | Symbol | Abstand | Wandlage):")
for d, m, g, art in sorted(zeilen):
    P("     S%-2d Marke r%-2d(%3d,%3d) Wand %s  |  Symbol r%-2d(%3d,%3d) %dx%d Wand %-3s | %4.1f px | %s"
      % (m[0], m[1], m[2], m[3], WAND[m[4]], g['r'], g['x'], g['y'], g['w'], g['h'], g['wand'], d, art))
open(r'analysis\karte_0901\_spiegel_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
