# -*- coding: utf-8 -*-
"""Nachtrag K: gibt es eine Lage (x-Mitte, z-Mitte, Gierwinkel), in der die Granate WEDER die
Sicherung durchdringt NOCH durch die geschlossene Kuppel ragt?  Punkt-genau:
  Sicherung  = Zylinder um die Achse (y=-1062, x=-280, z 1057..1463), Radius 26 — Abstand je
               Granaten-Punkt UND je Punkt auf den Granaten-Kanten (Kanten in 8 Stuecke geteilt)
  Kuppel     = Unterseite der Dreiecke von Prop 1/2 (sitz.py kuppel_hoehe)
Ausgabe: je Kandidat der kleinste Abstand zur Sicherung und die kleinste Luft zur Kuppel."""
import os, sys, math, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sitz as S
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, md1_lesen, PSX  # noqa


def granate_kanten():
    d = open(os.path.join(PSX, 'PLD', 'PL00W09.PLW'), 'rb').read()
    off, n = struct.unpack_from('<II', d, 0)
    dr = struct.unpack_from('<4I', d, off)
    m = md1_lesen(d[dr[2]:dr[3]])['meshes'][0]
    rot = lambda p: (p[0], -(p[2] - 32), p[1] - 190)
    pts = []
    for art, fl, uvl in (('T', m['tris'], m['tuv']), ('Q', m['quads'], m['quv'])):
        for fc, uv in zip(fl, uvl):
            vs = (uv[1], uv[4], uv[7]) + ((uv[10],) if art == 'Q' else ())
            if min(vs) < 200:
                continue
            ecken = [rot(m['tv'][i]) for i in fc]
            if art == 'Q':      # Z-Ordnung 0-1-3-2 = Umlauf
                ecken = [ecken[0], ecken[1], ecken[3], ecken[2]]
            for i in range(len(ecken)):
                a, b = ecken[i], ecken[(i + 1) % len(ecken)]
                for k in range(9):
                    t = k / 8.0
                    pts.append(tuple(a[j] + (b[j] - a[j]) * t for j in range(3)))
    return pts


def main():
    g = granate_kanten()
    tri = S.kuppel_dreiecke(rdt_laden('ROOM1150.RDT'))
    best = []
    for xc in range(-368, -352):
        for zc in (1250, 1255, 1260, 1265, 1270):
            for gier in (-48, -32, -16, 0, 16, 32, 48):
                w = (1024 + gier) * 2 * math.pi / 4096
                c, s = math.cos(w), math.sin(w)
                welt = []
                for (x, y, z) in g:
                    # Ry wie sitz.py (x' = x cos + z sin, z' = -x sin + z cos), Welt = Mitte + ...
                    wx = xc + (x * c + z * s)
                    wz = zc + (-x * s + z * c)
                    welt.append((wx, -1091 + y, wz))
                abst = min(math.hypot(p[0] + 280, p[1] + 1062) - 26 if 1057 <= p[2] <= 1463 else 99
                           for p in welt)
                luft = min((S.kuppel_hoehe(tri, p[0], p[2]) or 1e9) - (-1036 - p[1]) for p in welt)
                best.append((min(abst, luft), xc, zc, gier, abst, luft))
    best.sort(reverse=True)
    print('beste 12 (Wert = min(Abstand Sicherung, Luft Kuppel)):')
    for b in best[:12]:
        print('  xc %d zc %d gier %+d: Abstand zur Sicherung %.2f  Luft unter der Kuppel %.2f' % b[1:])
    for b in best:
        if b[1] in (-365, -361) and b[2] == 1260 and b[3] == 0:
            print('  Referenz xc %d zc %d gier %+d: Abstand %.2f Luft %.2f' % b[1:])


if __name__ == '__main__' and '--rollen' not in sys.argv and '--sitz' not in sys.argv:
    main()


def rollen():
    """Variante: die Granate um ihre Laengsachse gerollt (Prop rot_x, Reihenfolge Ry*Rx*Rz =
    main.c pc_prop_rot_q12), dann so tief gelegt, dass ihr tiefster Punkt auf dem Fachboden
    liegt (y_max = -1036). Gesucht: Abstand zur Sicherung >= 0 UND Luft unter der Kuppel >= 0."""
    g = granate_kanten()
    tri = S.kuppel_dreiecke(rdt_laden('ROOM1150.RDT'))
    erg = []
    for roll in range(-160, 161, 16):          # 4096 = 360 Grad
        a = roll * 2 * math.pi / 4096
        ca, sa = math.cos(a), math.sin(a)
        # Rx(a): y' = y cos - z sin, z' = y sin + z cos  (Modell-X = Laengsachse bleibt)
        gr = [(x, y * ca - z * sa, y * sa + z * ca) for (x, y, z) in g]
        tief = max(p[1] for p in gr)            # +Y nach unten
        yc = -1036 - tief
        for xc in range(-372, -350):
            welt = [(xc + z, yc + y, 1260 - x) for (x, y, z) in gr]   # Ry(1024): x' = z, z' = -x
            abst = min(math.hypot(p[0] + 280, p[1] + 1062) - 26 if 1057 <= p[2] <= 1463 else 99
                       for p in welt)
            luft = min((S.kuppel_hoehe(tri, p[0], p[2]) or 1e9) - (-1036 - p[1]) for p in welt)
            erg.append((min(abst, luft), roll, xc, yc, abst, luft))
    erg.sort(reverse=True)
    print('gerollt, beste 10:')
    for e in erg[:10]:
        print('  rot_x %+d  xc %d  yc %.1f: Abstand zur Sicherung %.2f  Luft unter der Kuppel %.2f' % e[1:])


if __name__ == '__main__' and '--rollen' in sys.argv:
    rollen()


def pruefe_sitz(xc, yc, zc=1260):
    """Ein Sitz (rot_y 1024, rot_x 0): Abstand zur Sicherung, Luft unter der Kuppel, Tiefe im
    Fachboden — in ROOM1150 UND ROOM1151."""
    g = granate_kanten()
    for raum in ('ROOM1150.RDT', 'ROOM1151.RDT'):
        tri = S.kuppel_dreiecke(rdt_laden(raum))
        welt = [(xc + z, yc + y, zc - x) for (x, y, z) in g]
        abst = min(math.hypot(p[0] + 280, p[1] + 1062) - 26 if 1057 <= p[2] <= 1463 else 99
                   for p in welt)
        luft = min((S.kuppel_hoehe(tri, p[0], p[2]) or 1e9) - (-1036 - p[1]) for p in welt)
        tief = max(p[1] for p in welt) - (-1036)
        print('%s Sitz (%d,%d,%d): Abstand zur Sicherung %.2f  Luft unter der Kuppel %.2f  '
              'unter dem Fachboden %d (tiefster Punkt y=%d)'
              % (raum, xc, yc, zc, abst, luft, tief, max(p[1] for p in welt)))


if __name__ == '__main__' and '--sitz' in sys.argv:
    i = sys.argv.index('--sitz')
    pruefe_sitz(int(sys.argv[i + 1]), int(sys.argv[i + 2]))
