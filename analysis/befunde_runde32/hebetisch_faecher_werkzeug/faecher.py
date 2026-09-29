# -*- coding: utf-8 -*-
"""R32 — die beiden UNTEREN Faecher von Prop 0 (Hebetisch) aus den ausgelieferten MD1-Bytes (nur LESEN).

Findet in Prop 0 die achsparallelen Vierecke, die je Fach Boden, Decke und Seitenwaende bilden
(Face-Record-Offset + Punkt-Offsets), die Rueckwand (x = -1258) und die Vorderfront mit den
Oeffnungen (x = 2), und prueft, dass KEINE andere Flaeche von Prop 0 in das Innere eines Fachs
ragt (Durchstoss-Freiheit des leeren Fachs).

Plattform-Koordinaten, +Y nach unten. Aufruf: faecher.py [ROOM1150.RDT|ROOM1151.RDT]
"""
import os, sys
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
from r30_lib import rdt_laden, props, md1_lesen  # noqa


def lade(raum='ROOM1150.RDT'):
    rdt = rdt_laden(raum)
    p0 = props(rdt)[0]
    base = p0['md1_off']
    md = md1_lesen(p0['md1'])
    m = md['meshes'][0]
    k = m['kopf']
    voff = lambda i: base + 12 + k[0] + i * 8
    qoff = lambda i: base + 12 + k[11] + i * 16  # kopf[11] = qf (Viereck-Face-Offset)
    toff = lambda i: base + 12 + k[4] + i * 12
    return rdt, base, m, voff, qoff, toff


def flaechen(m):
    """alle Flaechen als Punktlisten im Umlauf (Vierecke in Z-Ordnung -> [0,1,3,2])."""
    v = m['tv']
    out = []
    for i, t in enumerate(m['tris']):
        out.append(('T', i, [v[j] for j in t], list(t)))
    for i, q in enumerate(m['quads']):
        out.append(('Q', i, [v[q[0]], v[q[1]], v[q[3]], v[q[2]]], list(q)))
    return out


def faecher(raum='ROOM1150.RDT'):
    """-> dict name -> Fach mit Grenzen und den Belegen (Face-Index, Face-Record, Punkte+Offsets)."""
    rdt, base, m, voff, qoff, toff = lade(raum)
    v = m['tv']
    F = flaechen(m)
    # Kandidaten: waagrechte Vierecke, die von x=2 bis x=-1258 durchgehen (Boden/Decke je Fach)
    waag = []
    for art, i, pts, idx in F:
        if art != 'Q':
            continue
        ys = {p[1] for p in pts}; xs = {p[0] for p in pts}; zs = sorted({p[2] for p in pts})
        if len(ys) == 1 and xs == {2, -1258} and len(zs) == 2:
            waag.append((pts[0][1], zs[0], zs[1], i, idx))
    senk = []
    for art, i, pts, idx in F:
        if art != 'Q':
            continue
        zs = {p[2] for p in pts}; xs = {p[0] for p in pts}; ys = sorted({p[1] for p in pts})
        if len(zs) == 1 and xs == {2, -1258} and len(ys) == 2:
            senk.append((pts[0][2], ys[0], ys[1], i, idx))
    aus = {}
    # je (z0,z1)-Spanne: Boden = groesseres y, Decke = kleineres y
    spannen = {}
    for y, z0, z1, i, idx in waag:
        spannen.setdefault((z0, z1), []).append((y, i, idx))
    for (z0, z1), lst in sorted(spannen.items()):
        if len(lst) != 2:
            continue
        lst.sort()
        decke, boden = lst[0], lst[1]
        w0 = [s for s in senk if s[0] == z0 and s[1] == decke[0] and s[2] == boden[0]]
        w1 = [s for s in senk if s[0] == z1 and s[1] == decke[0] and s[2] == boden[0]]
        if not (w0 and w1):
            continue
        aus[(z0, z1)] = dict(z0=z0, z1=z1, boden=boden[0], decke=decke[0], x_vorn=2, x_hinten=-1258,
                             boden_q=boden[1], boden_rec=qoff(boden[1]), boden_pkt=[(j, voff(j), v[j]) for j in boden[2]],
                             decke_q=decke[1], decke_rec=qoff(decke[1]),
                             wand0_q=w0[0][3], wand0_rec=qoff(w0[0][3]), wand1_q=w1[0][3], wand1_rec=qoff(w1[0][3]))
    # Rueckwand: volle Tafeln bei x=-1258, die die Fach-Spanne ganz abdecken
    for f in aus.values():
        rw = []
        for art, i, pts, idx in F:
            xs = {p[0] for p in pts}
            if xs != {-1258}:
                continue
            ys = [p[1] for p in pts]; zs = [p[2] for p in pts]
            if min(zs) <= f['z0'] and max(zs) >= f['z1'] and min(ys) <= f['decke'] and max(ys) >= f['boden']:
                rw.append((art, i, qoff(i) if art == 'Q' else toff(i)))
        f['rueckwand'] = rw
    return aus, (rdt, base, m, voff, qoff, toff)


def ragt_hinein(pts, f, eps=0.5):
    """ragt das Polygon ins OFFENE Innere des Fachs (um eps geschrumpft)? Sutherland-Hodgman-Clip."""
    ebenen = [(0, f['x_hinten'] + eps, +1), (0, f['x_vorn'] - eps, -1),
              (1, f['decke'] + eps, +1), (1, f['boden'] - eps, -1),
              (2, f['z0'] + eps, +1), (2, f['z1'] - eps, -1)]
    poly = [tuple(float(c) for c in p) for p in pts]
    for ax, w, sg in ebenen:
        neu = []
        for i in range(len(poly)):
            a, b = poly[i], poly[(i + 1) % len(poly)]
            ina, inb = sg * (a[ax] - w) > 0, sg * (b[ax] - w) > 0
            if ina:
                neu.append(a)
            if ina != inb:
                t = (w - a[ax]) / (b[ax] - a[ax])
                neu.append(tuple(a[j] + (b[j] - a[j]) * t for j in range(3)))
        poly = neu
        if not poly:
            return False
    return len(poly) >= 3


if __name__ == '__main__':
    raum = sys.argv[1] if len(sys.argv) > 1 else 'ROOM1150.RDT'
    aus, (rdt, base, m, voff, qoff, toff) = faecher(raum)
    print('%s Prop 0 MD1 @0x%X' % (raum, base))
    for (z0, z1), f in sorted(aus.items()):
        print('Fach z %d..%d: Boden y=%d (Viereck %d, Face-Record @0x%X), Decke y=%d (Viereck %d @0x%X), '
              'Wand z=%d (Viereck %d @0x%X), Wand z=%d (Viereck %d @0x%X), x %d (vorn, offen) .. %d (hinten)'
              % (z0, z1, f['boden'], f['boden_q'], f['boden_rec'], f['decke'], f['decke_q'], f['decke_rec'],
                 z0, f['wand0_q'], f['wand0_rec'], z1, f['wand1_q'], f['wand1_rec'], f['x_vorn'], f['x_hinten']))
        for j, o, p in f['boden_pkt']:
            print('    Bodenpunkt %3d @0x%05X %s' % (j, o, p))
        print('    Rueckwand bei x=-1258: %s' % ', '.join('%s%d @0x%X' % r for r in f['rueckwand']))
        hin = [(a, i) for a, i, pts, idx in flaechen(m) if ragt_hinein(pts, f)]
        print('    Flaechen von Prop 0, die ins Innere ragen: %d %s' % (len(hin), hin))
