#!/usr/bin/env python
"""Runde 30 karten-marken (Fortsetzung): SOLL-Werte der Abnahme, gerechnet aus den
Abzuegen des Nutzers selbst.
 - Panel unter dem ROOM1000-Kasten und unter der schwebenden 2F-Marke: aus dem ROOF-Abzug
   (dort ist an diesen Stellen nichts gezeichnet, das Panel ist auf allen Blaettern dasselbe -
   geprueft an den Punkten, die auf beiden Blaettern Panel sind).
 - Schema-Fuellung: FILL abe=1 = (d5 + f5) >> 1 je Kanal (inv_render_pc.c:948-953) mit
   f = RE15_KARTE_BESUCHT 0x81A4 (TEX.TIM @0x0556).
 - Treppenmarke (156,97) auf ROOF: Punktmenge, die verschwindet, wenn das Gatter greift.
Aufruf: python r30_soll_nach_korrektur.py"""
import numpy as np
from collections import Counter
from PIL import Image
N = 'analysis/befunde_runde30/nutzer_marken/'
def lade(n):
    a = np.array(Image.open(N + n).convert('RGB'))
    return a[::3, ::3]
roof, f2, f1 = lade('befund_1070_F233_marke1.png'), lade('befund_1070_F259_marke1.png'), lade('befund_1070_F310_marke2.png')
PANEL = {(0, 16, 88), (0, 16, 120)}
# Panel-Gleichheit der Blaetter: alle Punkte, die in ZWEI Abzuegen Panel sind, sind gleich?
for a, b, n in ((roof, f2, 'ROOF/2F'), (roof, f1, 'ROOF/1F'), (f2, f1, '2F/1F')):
    pa = np.array([[tuple(a[y, x]) in PANEL for x in range(320)] for y in range(240)])
    pb = np.array([[tuple(b[y, x]) in PANEL for x in range(320)] for y in range(240)])
    beide = pa & pb
    ungleich = int((np.any(a != b, axis=2) & beide).sum())
    print("Panel %s: %d Punkte auf beiden Blaettern Panel, davon ungleich %d" % (n, int(beide.sum()), ungleich))
w = 0x81A4
f5 = (w & 31, (w >> 5) & 31, (w >> 10) & 31)
c = Counter(); fremd = 0
for y in range(90, 121):
    for x in range(208, 221):
        p = tuple(int(v) for v in roof[y, x])
        if p not in PANEL: fremd += 1; continue
        d5 = tuple(v >> 3 for v in p)
        c[tuple(((d5[i] + f5[i]) >> 1) << 3 for i in range(3))] += 1
print("1F Kasten x 208..220 y 90..120 (403 Punkte), Panel aus dem ROOF-Abzug, nicht-Panel %d:" % fremd)
for k, v in sorted(c.items()): print("   SOLL %s: %d Punkte" % (k, v))
print("2F (188,178..182) SOLL = Panel:", [tuple(int(v) for v in roof[y, 188]) for y in range(178, 183)])
print("ROOF y=155 x=148..182 SOLL = (176,176,176), 35 Punkte; IST:", Counter(tuple(int(v) for v in roof[155, x]) for x in range(148, 183)))
# Treppenmarke auf ROOF
c2 = Counter()
for y in range(92, 101):
    for x in range(150, 163):
        p = tuple(int(v) for v in roof[y, x])
        if p not in PANEL: c2[p] += 1
print("ROOF Umfeld der Treppenmarke x 150..162 y 92..100, Nicht-Panel-Farben:", dict(c2))
pts = [(x, y) for y in range(92, 101) for x in range(150, 163) if tuple(int(v) for v in roof[y, x]) not in PANEL]
print("   Punkte:", pts)
