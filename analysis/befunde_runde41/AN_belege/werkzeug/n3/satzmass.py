"""Bildanteil je Spielbild aus dem state.log eines Fensterlaufs: Lage/Gier/Clip/Bild der Form (t=30) und Annettes (t=4d) aus dem
Log, Schnitt aus cam=, dann ID-Puffer wie szmass.py (Kinobalken abgezogen). satzmass.py <state.log> F [F ...]"""
import re, sys
from szmass import *
def zeile(log, f):
    for l in open(log, errors='replace'):
        if l.startswith('F%d ' % f):
            last = l
    return last
for f in map(int, sys.argv[2:]):
    l = zeile(sys.argv[1], f)
    cam = int(re.search(r'cam=(\d+)', l).group(1))
    objs = []
    for typ, mo, oid in (('30', FORM, 1), ('4d', FRAU, 2)):
        m = re.search(r't=%s st=\d+ ss1=\d+ ss2=\d+ ss3=\d+ g=\d+ mo=(\d+) af=(\d+)[^@]*@\((-?\d+),(-?\d+),r(\d+)' % typ, l)
        if m:
            clip, bild, x, z, r = map(int, m.groups())
            if oid == 2 and clip == 2:
                clip, bild = 1, 0   # mo=2 im state.log ist ihr Steh-Leerlauf (Fensterbild: steht); gemessen mit Steh-Clip 1 Bild 0
            objs.append((mo, clip, bild, (x, 0, z), r, oid))
    buf, al = idpuffer(cam, objs)
    m = mass(buf, al)
    print('F%d Cut %d: Form %.1f %% des Bildes, Annette %.0f %% frei, ganz im Band %s, bbox %s' % (
        f, cam, 100 * m['form'], 100 * m.get('frau_sichtbar', 0), m.get('frau_ganz'), m.get('frau_bbox')))
