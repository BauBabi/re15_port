# -*- coding: utf-8 -*-
"""Runde 31 / H — Geometrie des Kuppelfachs im Hebetisch (ROOM1150/1151) fuer die Anordnung
Granate LINKS / Sicherung RECHTS. NUR PLANUNG — massgeblich ist die Framedump-Messung.

Quellen (alle NUR gelesen):
  Kuppelhaelften   ROOM1150.RDT Prop 1 MD1 @0x138D4 / Prop 2 @0x13B88 (je 12 Punkte, 4+6 Flaechen)
                   ROOM1151.RDT Prop 1 @0x1594C / Prop 2 @0x15C00
  Deckelweg        For @0x0FC0 (15 Durchlaeufe) x Speed_set @0x0FCA +10 (Prop 1) / @0x0FD4 -10 (Prop 2)
                   -> offen Prop 1 +150 in z, Prop 2 -150 in z (Plattform-Koordinaten)
  Fachboden        Prop 0 Punkte @0x121AC..@0x1221C, y = -1036, Achteck
  Sicherung        gen/sicherung_prop.inc (MD1 wie gezeichnet)
  Granate          gen/granate_prop.inc   (MD1 wie gezeichnet)
  Kamera           Cut 4 @Datei 0xE0 (r30_lib.cuts), Plattform rot_y 2048 bei (-20700, y, -17460)
Plattform-Koordinaten, +Y nach unten; "Hoehe" = -1036 - y.
"""
import math, os, re, sys
H = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde30', 'sicherung_werkzeug'))
sys.path.insert(0, os.path.join(H, '..', '..', 'befunde_runde30', 'nachtrag-granate_werkzeug'))
from r30_lib import REPO, rdt_laden, props, md1_lesen, cuts, view_bauen, prop_rot, view_x_welt, projiziere  # noqa
import sitz as S  # noqa  (kuppel_dreiecke, kuppel_hoehe, im_achteck)

BODEN = -1036
OFFEN_LO, OFFEN_HI = 1110, 1410        # Naht 1260 -+ 150


def inc_md1(datei, name):
    t = open(os.path.join(REPO, 're15_port', 'engine', 'src', 'gen', datei), encoding='utf-8').read()
    m = re.search(r'%s\[(\d+)\]\s*=\s*\{(.*?)\};' % re.escape(name), t, re.S)
    b = bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})', m.group(2)))
    return md1_lesen(b)['meshes'][0]


def proben(m, teile=8):
    """Punkte + Kantenproben (jede Kante in <teile> Stuecke) + Flaechenmitten."""
    v = m['tv']
    aus = set(v)
    faces = [list(t) for t in m['tris']] + [[q[0], q[1], q[3], q[2]] for q in m['quads']]  # Z-Ordnung -> Umlauf
    for f in faces:
        e = [v[i] for i in f]
        for i in range(len(e)):
            a, b = e[i], e[(i + 1) % len(e)]
            for k in range(1, teile):
                t = k / teile
                aus.add(tuple(a[j] + (b[j] - a[j]) * t for j in range(3)))
        aus.add(tuple(sum(p[j] for p in e) / len(e) for j in range(3)))
    return sorted(aus)


def roty(p, ry):
    """Prop-Drehung nur um Y (Ry*Rx*Rz mit rx=rz=0): x' = x cos + z sin, z' = -x sin + z cos."""
    w = ry * 2 * math.pi / 4096
    c, s = math.cos(w), math.sin(w)
    return (p[0] * c + p[2] * s, p[1], -p[0] * s + p[2] * c)


def platzieren(pts, pos, ry):
    return [tuple(roty(p, ry)[j] + pos[j] for j in range(3)) for p in pts]


class Fach:
    def __init__(self, raum='ROOM1150.RDT'):
        rdt = rdt_laden(raum)
        self.zu = S.kuppel_dreiecke(rdt)
        pr = props(rdt)
        self.offen = []
        for k, dz in ((1, +150), (2, -150)):
            m = md1_lesen(pr[k]['md1'])['meshes'][0]
            v = [(p[0], p[1], p[2] + dz) for p in m['tv']]
            for t in m['tris']:
                self.offen.append([v[i] for i in t])
            for q in m['quads']:
                a, b, c, d = (v[i] for i in q)
                self.offen.append([a, b, c]); self.offen.append([b, d, c])
        self.cut4 = cuts(rdt)[4]
        self.view = view_bauen(self.cut4)

    def luft(self, welt):
        """(Luft unter der GESCHLOSSENEN Kuppel, Luft unter den OFFENEN Deckeln, ausserhalb Achteck)"""
        lz = lo = 1e9
        raus = 0
        for p in welt:
            h = BODEN - p[1]
            if not S.im_achteck(p[0], p[2]):
                raus += 1
            k = S.kuppel_hoehe(self.zu, p[0], p[2])
            lz = min(lz, (k if k is not None else -1e9) - h)
            if not (OFFEN_LO < p[2] < OFFEN_HI):
                k = S.kuppel_hoehe(self.offen, p[0], p[2])
                if k is not None:
                    lo = min(lo, k - h)
        return lz, lo, raus

    def schirm(self, welt, py):
        cr, ct = view_x_welt(self.view, prop_rot(0, 2048, 0), (-20700, py, -17460))
        out = [projiziere(self.view, cr, ct, tuple(int(round(c)) for c in p)) for p in welt]
        return [o for o in out if o]


def abstand_stab(welt, pos, ry, halb=203, r=26):
    """kleinster Abstand der Punkte zur Mantelflaeche des Sicherungs-Zylinders (negativ = drin)."""
    ax = roty((1, 0, 0), ry)
    best = 1e9
    for p in welt:
        d = [p[j] - pos[j] for j in range(3)]
        t = sum(d[j] * ax[j] for j in range(3))
        radial = math.sqrt(max(0.0, sum(d[j] * d[j] for j in range(3)) - t * t))
        if abs(t) <= halb:
            best = min(best, max(radial - r, abs(t) - halb) if radial < r else radial - r)
        else:
            best = min(best, math.hypot(max(0.0, radial - r), abs(t) - halb))
    return best


SICH = None
GRAN = None


def modelle():
    global SICH, GRAN
    if SICH is None:
        SICH = proben(inc_md1('sicherung_prop.inc', 're15_sicherung_md1'))
        GRAN = proben(inc_md1('granate_prop.inc', 're15_granate_md1'))
    return SICH, GRAN


def bewerte(fach, s_pos, s_ry, g_pos, g_ry):
    sich, gran = modelle()
    ws = platzieren(sich, s_pos, s_ry)
    wg = platzieren(gran, g_pos, g_ry)
    ls = fach.luft(ws)
    lg = fach.luft(wg)
    ab = abstand_stab(wg, s_pos, s_ry)
    ofs = sum(1 for p in ws if OFFEN_LO < p[2] < OFFEN_HI) / len(ws)
    ofg = sum(1 for p in wg if OFFEN_LO < p[2] < OFFEN_HI) / len(wg)
    return dict(ws=ws, wg=wg, luft_s=ls, luft_g=lg, abstand=ab, offen_s=ofs, offen_g=ofg)


if __name__ == '__main__':
    f = Fach()
    # Bestand Runde 30
    b = bewerte(f, (-280, -1062, 1260), 1024, (-362, -1088, 1260), 1024)
    print('BESTAND: Sicherung Luft zu/offen/raus %s  Granate %s  Abstand %.2f  in Oeffnung S %.2f G %.2f'
          % (tuple(round(x, 2) for x in b['luft_s']), tuple(round(x, 2) for x in b['luft_g']), b['abstand'], b['offen_s'], b['offen_g']))
    for py in (-305, -1205):
        ss = f.schirm(b['ws'], py); sg = f.schirm(b['wg'], py)
        print('  Plattform y=%d: Schirm-x Sicherung %.1f (%d..%d)  Granate %.1f (%d..%d)' % (
            py, sum(p[0] for p in ss) / len(ss), min(p[0] for p in ss), max(p[0] for p in ss),
            sum(p[0] for p in sg) / len(sg), min(p[0] for p in sg), max(p[0] for p in sg)))
