# -*- coding: utf-8 -*-
"""Nachtrag K: Sitz der Granate im Kuppelfach — freier Platz aus der ausgelieferten Geometrie.

Plattform-Koordinaten (+Y nach unten). Quellen:
  Fachboden   ROOM1150.RDT Prop 0 MD1 @0x11E40, Vierecke 79-81, alle Punkte y=-1036
  Kuppel      Prop 1 MD1 @0x138D4 (z 1260..1645), Prop 2 MD1 @0x13B88 (z 875..1260), GESCHLOSSEN
              (Deckelweg +-150 in z erst in der Fahrt, For @0x0FC0 / Speed_set @0x0FCA/@0x0FD4)
  Sicherung   gen/sicherung_prop.inc, Sitz (-280,-1062,1260) rot_y 1024 (re15_sicherung.h)
  Granate     die 24 Waffen-Punkte aus PL00W09.PLW dir[2] (plw_baender.py), gedreht
              (x, y, z) -> (x, -(z-32), y-190) wie im Export (granate_export.py)
Prueft je Kandidat: (a) Abstand zur Sicherung (Huellen duerfen sich nicht schneiden),
(b) jeder Granatenpunkt liegt UNTER der geschlossenen Kuppel (Hoehe ueber dem Boden kleiner
als die Kuppel an derselben (x,z)), (c) auf dem Fachboden (Achteck).
"""
import os, sys, struct, math
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import props, rdt_laden, md1_lesen, inc_bytes, PSX  # noqa

BODEN = -1036


def granate_punkte():
    d = open(os.path.join(PSX, 'PLD', 'PL00W09.PLW'), 'rb').read()
    off, n = struct.unpack_from('<II', d, 0)
    dr = struct.unpack_from('<4I', d, off)
    m = md1_lesen(d[dr[2]:dr[3]])['meshes'][0]
    w = set()
    for art, fl, uvl in (('T', m['tris'], m['tuv']), ('Q', m['quads'], m['quv'])):
        for fc, uv in zip(fl, uvl):
            vs = (uv[1], uv[4], uv[7]) + ((uv[10],) if art == 'Q' else ())
            if min(vs) >= 200:
                w.update(fc)
    return [(m['tv'][i][0], -(m['tv'][i][2] - 32), m['tv'][i][1] - 190) for i in sorted(w)]


def kuppel_dreiecke(rdt):
    pr = props(rdt)
    tri = []
    for k in (1, 2):
        m = md1_lesen(pr[k]['md1'])['meshes'][0]
        v = m['tv']
        for t in m['tris']:
            tri.append([v[i] for i in t])
        for q in m['quads']:
            a, b, c, dd = (v[i] for i in q)
            tri.append([a, b, c]); tri.append([b, dd, c])
    return tri


def kuppel_hoehe(tri, x, z):
    """kleinste Hoehe ueber dem Boden (BODEN - y) der Kuppel ueber (x,z); None = keine Flaeche."""
    best = None
    for (a, b, c) in tri:
        den = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
        if den == 0:
            continue
        l1 = ((b[2] - c[2]) * (x - c[0]) + (c[0] - b[0]) * (z - c[2])) / den
        l2 = ((c[2] - a[2]) * (x - c[0]) + (a[0] - c[0]) * (z - c[2])) / den
        l3 = 1 - l1 - l2
        if min(l1, l2, l3) < -1e-9:
            continue
        y = l1 * a[1] + l2 * b[1] + l3 * c[1]
        h = BODEN - y
        if h > 0.5 and (best is None or h < best):
            best = h
    return best


def im_achteck(x, z):
    pts = [(-485, 1260), (-432, 958), (-280, 875), (-128, 958), (-74, 1260), (-128, 1562), (-280, 1645), (-432, 1562)]
    s = None
    for i in range(8):
        (x1, z1), (x2, z2) = pts[i], pts[(i + 1) % 8]
        c = (x2 - x1) * (z - z1) - (z2 - z1) * (x - x1)
        if c != 0:
            if s is None: s = c > 0
            elif (c > 0) != s: return False
    return True


def main():
    g = granate_punkte()
    gx = [p[0] for p in g]; gy = [p[1] for p in g]; gz = [p[2] for p in g]
    print('Granate (gedreht, Modellraum): %d Punkte x%d..%d y%d..%d z%d..%d' % (len(g), min(gx), max(gx), min(gy), max(gy), min(gz), max(gz)))
    halb_y = max(gy)
    for raum in ('ROOM1150.RDT', 'ROOM1151.RDT'):
        tri = kuppel_dreiecke(rdt_laden(raum))
        print(raum)
        for rot in (1024, 0):
            for xc in range(-400, -339, 5):
                zc = 1260
                yc = BODEN - halb_y
                # rot_y 1024: Modell-X -> Welt-Z (wie die Sicherung, re15_sicherung.h ROT_Y)
                welt = []
                for (x, y, z) in g:
                    if rot == 1024:
                        wx, wz = xc + z, zc - x   # Ry(90): x' = z, z' = -x (Laengsachse entlang z)
                    else:
                        wx, wz = xc + x, zc + z
                    welt.append((wx, yc + y, wz))
                # (a) Sicherung: Zylinder r=26 um x=-280, z 1057..1463
                sx_min = min(abs(p[0] - (-280)) for p in welt)
                abstand = min(p[0] for p in welt) , max(p[0] for p in welt)
                spalt = -280 - 26 - max(p[0] for p in welt)
                # (b) Kuppel
                luft = min((kuppel_hoehe(tri, p[0], p[2]) or 1e9) - (BODEN - p[1]) for p in welt)
                # (c) Boden
                boden_ok = all(im_achteck(p[0], p[2]) for p in welt)
                print('  rot_y %4d xc %d  yc %d: x %d..%d z %d..%d  Spalt zur Sicherung %d  Luft unter der Kuppel %.1f  auf dem Achteck %s'
                      % (rot, xc, yc, abstand[0], abstand[1], min(p[2] for p in welt), max(p[2] for p in welt), spalt, luft, boden_ok))


if __name__ == '__main__':
    main()
