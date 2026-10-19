"""B1 NEU (Runde 39 Spur G): Schema-Zeichnung des Blattes 0 aus Kollision + Tueren.

Nutzer 2026-10-17: "Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da passt
nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu der Karte."

Verfahren (Nutzer-Modell 2026-09-02, Memory reai-v2-zu-kompliziert: "Kollisionsboxen pro Raum, alle Tueren und
wo sie hinfuehren, alle Treppen ... der naechste Raum schliesst EXAKT an die Wand an"):
  1. Begehbare Flaeche je Ort = ECHTER Kollisionsloeser (probe_r39_karten flut, r 0, Schritt 100) -> PGM.
     Zellen je Ort = wenige achsparallele Rechtecke ueber dieser Flaeche (Wandflaechen = Rand +- 50).
  2. Lage im Rahmen von ROOM1180 je Tuerpaar (b1_layout.py): die Wandflaeche des Nachbarn an der Tuer liegt AUF der
     Wandflaeche des Vorgaengers, die Tuermitten (Trigger) fallen laengs der Wand zusammen. Alle Paare gegenlaeufig
     (keine Drehung). ROOM11C0 hat KEINE Tuer zum Gang: seine Westwand wird auf die Westwand von ROOM11B0 gelegt
     (= Ostwand des Gangs) - so stehen beide auch im Original (Seiten-Tabelle Blatt 0 @0x800762a0: r3 (168,76,72,32)
     und r4 (168,100,72,96), beide x 168 = Ostkante des Gang-Rechtecks r0 (120,60,56,72)).
  3. EIN Massstab fuer das ganze Blatt: RE2 Retail FUN_8006e120 x = (x+28000)/450 @0x8006e1dc-0x8006e218 (Beta ->
     Retail, Runde 35 Spur G), als Affine 65536/450 = 145,6 -> A = 146; z nach oben (E = -146) wie FUN_800473f8
     (`subu v1,zero,v1` @0x800474b0). Versatz: Mitte der Zeichnung auf die Mitte der Original-Rechtecke von Blatt 0
     (Huelle der sieben Rechtecke @0x800762a0: x 76..240, y 60..196 -> (158,128)).
  4. Pixelzellen je Ort mit SEINER Affine (genau die Formel des Markers, re15_map_zone_marker). Wandlinien
     verschiedener Orte, die sich auf <= 3 px gegenueberstehen, werden auf EINE Linie gelegt (Nutzer 2026-10-17:
     "Keine Doppelwaende") - beide Zellen wachsen/schrumpfen um hoechstens 2 px, die Abbildung bleibt.
  5. Tuermarken auf der gemeinsamen Linie (laengs = Mittel der beiden Trigger-Mitten), Treppen im Treppenhaus.
Ausgabe: Vorschau-PNG (320x240, Klassenfarben) und die C-Zeilen fuer re15_map_zones.h.
Aufruf: b1_plan.py <pgm-ordner> [vorschau.png]"""
import sys, os, re
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, HIER)
import gen_map_zones as GZ
from sca_liste import zellen

D = sys.argv[1]
A = 146                         # 65536/450 (RE2 FUN_8006e120 @0x8006e1dc-0x8006e218)
MITTE = (158, 128)              # Huelle der Rechtecke von Blatt 0 @0x800762a0
SNAP = 3                        # gegenueberliegende Wandlinien bis 3 px Abstand -> eine Linie

def lade(n):
    d = open(os.path.join(D, n + '.pgm'), 'rb').read()
    m = re.match(rb'P5\n# x0 (-?\d+) z0 (-?\d+) schritt (\d+)\n(\d+) (\d+)\n255\n', d)
    x0, z0, st, w, h = map(int, m.groups())
    return dict(x0=x0, z0=z0, st=st, a=np.frombuffer(d[m.end():], np.uint8).reshape(h, w) > 0)

def bbox(g):
    k, i = np.nonzero(g['a'])
    return (g['x0'] + i.min() * g['st'] - 50, g['x0'] + i.max() * g['st'] + 50,
            g['z0'] + k.min() * g['st'] - 50, g['z0'] + k.max() * g['st'] + 50)

def tuer(rid, slot, ziel):
    for d in GZ.read_rdt(rid)[1]:
        if d['slot'] == slot and d['dest'] == ziel: return d
    raise KeyError((hex(rid), slot, hex(ziel)))

G = {n: lade(n) for n in ('1180', '1190H', '1190O', '11B0', '11B0E', '11C0', '11D0', '11D0K', '10A0')}

# ---------------------------------------------------------------------------------------------------------
# ORTE: Zellen in Weltkoordinaten des EIGENEN Raums (Wandflaechen; x0,x1,z0,z1). Herkunft je Zeile.
bb = {n: bbox(g) for n, g in G.items()}
ORTE = {
    # Gang ROOM1180 (= ROOM1230): Laeufe aus der Flut (Dossier B1-M2): Nord+Laengsgang x 2208..5508 z -4277..31023,
    # Querarm x -9492..5508 z 12223..15223, Westbein x -9492..-6392 z 9523..12223, Versatz x -5592..5508
    # z -7277..-4277, Suedbein x -5592..-2692 z -16377..-7377.
    'gang': dict(raum=0x1180, zellen=[(2158, 5558, -4327, 31073), (-9542, 5558, 12173, 15273),
                                       (-9542, -6342, 9473, 12273), (-5642, 5558, -7327, -4227),
                                       (-5642, -2642, -16427, -7277)]),
    'halle': dict(raum=0x1190, zellen=[bb['1190H']]),            # ROOM1190 Schiessstand (Huelle der Flut)
    'ost': dict(raum=0x1190, zellen=[bb['1190O']]),              # ROOM1190 Ostkammer
    # ROOM11B0: West-Streifen (Tueren 1180/11C0) UND Werkstatt (hinter der Trennwand @0x00E8C/@0x00EC8, die nach
    # "Reservestrom an" (Bank 4 Bit 243 -> 4/6, sub00 Sca_id_set 37 03 0c 00 @0x0118C) aufgeht) = EIN Raum.
    '11b0': dict(raum=0x11B0, zellen=[(min(bb['11B0'][0], bb['11B0E'][0]), max(bb['11B0'][1], bb['11B0E'][1]),
                                       min(bb['11B0'][2], bb['11B0E'][2]), max(bb['11B0'][3], bb['11B0E'][3]))]),
    # ROOM11C0 Parkdeck: Ring um die Insel. Loch = groesstes Rechteck OHNE begehbaren Rasterpunkt in der Insel
    # (insel_11c0() unten: x -11900..4700 z -7200..8300), Aussenkanten aus der Flut; die Suedwest-Ecke unter dem
    # Einfahrtsbereich (westlich der Westsaeule) ist kein Raum.
    '11c0': dict(raum=0x11C0, zellen=None),
    # ROOM11D0 Zwinger (= ROOM1160): West-Teil (Kaefige + Westgang), Sued-Gang (bis zur Ostwand @0x008C0 x 1050) und
    # Ost-Teil an der Gangtuer (bis z -16500 = Tuerwand @0x008CC); dazwischen ragt das Suedbein des Gangs herein.
    # Die Kammer x 1550..5750 z -23050..-19350 hinter @0x008C0/@0x008B4 ist nicht begehbar (Tuer -> ROOM1260).
    '11d0': dict(raum=0x11D0, zellen=[(-11500, -4900, -23100, -11500), (-8500, 1050, -23100, -20000),
                                       (-1700, 5000, -20000, -16500)]),
    # ROOM11D0 Kammer der Kanal-Leiter (Zone 1): nur aus ROOM1260 erreichbar (1260 slot 3 -> Spawn (3900,-22000)),
    # gegen den Zwinger dicht (Waende @0x008C0 x 1050..1550, @0x008B4 z -19350..-18850); der "Tuer"-Trigger
    # -> 1260 (slot 1 @0x01228) liegt auf dem Kreis @0x00A10 (1800..3300, -22750..-21250) = Bodenloch, keine Wandtuer.
    'kammer': dict(raum=0x11D0, zellen=[bb['11D0K']]),
    # ROOM10A0 B1-Absatz (Band 4) samt beiden Treppenlaeufen: Aot_set sce 13 Band 4 @0x00E7C (24000,20100,2700,4500)
    # und @0x00E90 (20100,19500,2700,2300).
    '10a0': dict(raum=0x10A0, zellen=[(bb['10A0'][0], bb['10A0'][1], 19500, bb['10A0'][3])]),
}

def insel_11c0(g):
    from scipy import ndimage
    a = g['a']; H = ndimage.binary_fill_holes(a) & ~a
    lab, n = ndimage.label(H); groesse = ndimage.sum(H, lab, range(1, n + 1)); H = lab == (np.argmax(groesse) + 1)
    best = (0, None); hgt = np.zeros(a.shape[1], int)
    for r in range(a.shape[0]):
        hgt = np.where(H[r], hgt + 1, 0); stack = []
        for c in range(a.shape[1] + 1):
            cur = hgt[c] if c < a.shape[1] else 0; start = c
            while stack and stack[-1][1] >= cur:
                s0, hh = stack.pop()
                if hh * (c - s0) > best[0]: best = (hh * (c - s0), (s0, c - 1, r - hh + 1, r))
                start = s0
            stack.append((start, cur))
    s0, e0, r0, r1 = best[1]; st = g['st']
    return (g['x0'] + s0 * st - 50, g['x0'] + e0 * st + 50, g['z0'] + r0 * st - 50, g['z0'] + r1 * st + 50)
hx0, hx1, hz0, hz1 = insel_11c0(G['11C0'])
X0, X1, Z0, Z1 = bb['11C0']
_k, _i = np.nonzero(G['11C0']['a']); _z = G['11C0']['z0'] + _k * 100; _x = G['11C0']['x0'] + _i * 100
XW = int(_x[_z < hz1].min()) - 50          # Aussenkante der Westsaeule (unterhalb des Querbands)
ORTE['11c0']['zellen'] = [(X0, X1, hz1, Z1), (XW, hx0, hz0, hz1), (hx1, X1, hz0, hz1), (XW, X1, Z0, hz0)]
print('11C0 Insel (leer) x %d..%d z %d..%d, Westsaeule ab x %d, Huelle %s' % (hx0, hx1, hz0, hz1, XW, (X0, X1, Z0, Z1)))

# ---------------------------------------------------------------------------------------------------------
# LAGE im Rahmen von ROOM1180 (b1_layout.py, Tuerpaare) - hier mit den Zellen-Wandflaechen nachgerechnet.
def wand_an_tuer(ort, trig, richtung):
    """Wandflaeche der Zelle, in deren Spanne die Trigger-Mitte liegt, auf der Tuer-Seite."""
    tx, tz = trig
    best = None
    for (x0, x1, z0, z1) in ORTE[ort]['zellen']:
        if richtung in ('S', 'N') and not (x0 <= tx <= x1): continue
        if richtung in ('W', 'E') and not (z0 <= tz <= z1): continue
        f = {'S': z0, 'N': z1, 'W': x0, 'E': x1}[richtung]
        d = abs((tz if richtung in 'SN' else tx) - f)
        if best is None or d < best[0]: best = (d, f)
    return best[1]

PAARE = [  # (Ort A, Raum A, Slot A, Seite der Tuer in A, Ort B, Raum B, Slot B)
    ('gang', 0x1180, 2, 'S', 'halle', 0x1190, 4),
    ('gang', 0x1230, 6, 'S', 'ost', 0x1190, 6),
    ('gang', 0x1180, 3, 'E', '11b0', 0x11B0, 0),
    ('gang', 0x1180, 1, 'S', '10a0', 0x10A0, 1),
    ('gang', 0x1180, 0, 'S', '11d0', 0x11D0, 0),
    ('11b0', 0x11B0, 1, 'S', '11c0', 0x11C0, 0),
]
GEGEN = {'S': 'N', 'N': 'S', 'W': 'E', 'E': 'W'}
lage = {'gang': (0, 0)}
tueren = []
for (a, ra, sa, sei, b, rb, sb) in PAARE:
    tab = tuer(ra, sa, rb if rb != 0x1190 or b != 'ost' else 0x1190)
    tba = tuer(rb, sb, 0x1230 if (a, sa) == ('gang', 6) else ra)
    TA = (tab['lx'], tab['lz']); TB = (tba['lx'], tba['lz'])
    fa = wand_an_tuer(a, TA, sei); fb = wand_an_tuer(b, TB, GEGEN[sei])
    la = lage[a]
    if sei in 'SN': t = (la[0] + TA[0] - TB[0], la[1] + fa - fb)
    else:           t = (la[0] + fa - fb, la[1] + TA[1] - TB[1])
    if b == '11c0':
        # keine Tuer zum Gang: Westwand auf die Westwand von 11B0 (Ostwand des Gangs), s. Kopf Punkt 2
        w11b0 = ORTE['11b0']['zellen'][0][0] + lage['11b0'][0]
        t = (w11b0 - ORTE['11c0']['zellen'][0][0], t[1])
    lage.setdefault(b, t)
    tueren.append(dict(a=a, b=b, ra=ra, rb=rb, sa=sa, sb=sb, seite=sei, TA=TA, TB=TB, tab=tab, tba=tba))
lage['kammer'] = lage['11d0']            # ROOM11D0 Zone 1: derselbe Raumrahmen wie Zone 0
print('LAGE (Rahmen ROOM1180):', lage)

# ---------------------------------------------------------------------------------------------------------
# ABBILDUNG je Ort: mx = ((A*x) >> 16) + c, my = ((-A*z) >> 16) + f  (Affine von re15_map_zone_marker)
alle = []
for n, o in ORTE.items():
    for (x0, x1, z0, z1) in o['zellen']:
        tx, tz = lage[n]
        alle += [(x0 + tx, z0 + tz), (x1 + tx, z1 + tz)]
gx0 = min(p[0] for p in alle); gx1 = max(p[0] for p in alle)
gz0 = min(p[1] for p in alle); gz1 = max(p[1] for p in alle)
cx = (gx0 + gx1) // 2; cz = (gz0 + gz1) // 2
C0 = MITTE[0] - ((A * cx) >> 16); F0 = MITTE[1] - ((-A * cz) >> 16)
for n, o in ORTE.items():
    tx, tz = lage[n]
    o['c'] = C0 + round(A * tx / 65536); o['f'] = F0 + round(-A * tz / 65536)
def px(n, x): return ((A * x) >> 16) + ORTE[n]['c']
def py(n, z): return ((-A * z) >> 16) + ORTE[n]['f']

# Pixelzellen [x0,x1]x[y0,y1] inklusive (Raender = Wandlinien)
for n, o in ORTE.items():
    o['pz'] = [[px(n, x0), px(n, x1), py(n, z1), py(n, z0)] for (x0, x1, z0, z1) in o['zellen']]

# ---------------------------------------------------------------------------------------------------------
# EINE LINIE STATT ZWEI: gegenueberliegende Raender verschiedener Orte (<= SNAP px) auf die Mitte legen.
def ueberlapp(a0, a1, b0, b1): return min(a1, b1) - max(a0, b0)
snaps = []
for _ in range(3):
    for n1, o1 in ORTE.items():
        for n2, o2 in ORTE.items():
            if n1 >= n2: continue
            for c1 in o1['pz']:
                for c2 in o2['pz']:
                    for (i1, i2, j0, j1) in ((1, 0, 2, 3), (0, 1, 2, 3)):     # senkrechte Raender
                        a, b = c1[i1], c2[i2]
                        if (i1 == 1 and -1 <= b - a <= SNAP) or (i1 == 0 and -1 <= a - b <= SNAP):
                            if a != b and ueberlapp(c1[2], c1[3], c2[2], c2[3]) >= 2:
                                m = (a + b) // 2 if i1 == 1 else (a + b + 1) // 2
                                snaps.append((n1, n2, 'x', a, b, m)); c1[i1] = m; c2[i2] = m
                    for (i1, i2) in ((3, 2), (2, 3)):                         # waagerechte Raender
                        a, b = c1[i1], c2[i2]
                        if (i1 == 3 and -1 <= b - a <= SNAP) or (i1 == 2 and -1 <= a - b <= SNAP):
                            if a != b and ueberlapp(c1[0], c1[1], c2[0], c2[1]) >= 2:
                                m = (a + b) // 2 if i1 == 3 else (a + b + 1) // 2
                                snaps.append((n1, n2, 'y', a, b, m)); c1[i1] = m; c2[i2] = m
for s in snaps: print('EINE LINIE: %s/%s %s %d|%d -> %d' % s)

# ---------------------------------------------------------------------------------------------------------
# DISJUNKTE ZELLEN: der Zeichner fuellt jede Zelle mit abe = 1 (halbtransparent, re15_inv_screen.c Schema-Fuellung);
# ueberlappende Zellen DERSELBEN Zeichnung mischten doppelt (gemessen am Sweep: hellere Flecken in den Ueberdeckungen).
# Die Vereinigung je Ort wird deshalb in nicht-ueberlappende Rechtecke zerlegt (zeilenweise Laeufe, gleiche Laeufe
# senkrecht zusammengefasst); die Umrisslinie bleibt, weil der Zeichner nur Raender ohne Nachbarzelle zieht.
def zerlege(pz):
    pts = set()
    for (x0, x1, y0, y1) in pz:
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1): pts.add((x, y))
    ys = sorted(set(y for _, y in pts)); laeufe = {}
    for y in ys:
        xs = sorted(x for x, yy in pts if yy == y); r = []; a = xs[0]; b = xs[0]
        for x in xs[1:] + [None]:
            if x is not None and x == b + 1: b = x; continue
            r.append((a, b))
            if x is not None: a = b = x
        laeufe[y] = r
    offen = {}; aus = []
    for y in ys + [ys[-1] + 1]:
        jetzt = set(laeufe.get(y, []))
        for k in list(offen):
            if k not in jetzt: aus.append([k[0], k[1], offen.pop(k), y - 1])
        for k in jetzt:
            if k not in offen: offen[k] = y
    return sorted(aus, key=lambda c: (c[2], c[0]))
for n, o in ORTE.items():
    o['pz_roh'] = [list(c) for c in o['pz']]
    o['pz'] = zerlege(o['pz'])

# ---------------------------------------------------------------------------------------------------------
# TUERMARKEN auf der gemeinsamen Linie; Treppen im Treppenhaus.
marken = []
for t in tueren:
    a, b, sei = t['a'], t['b'], t['seite']
    # gemeinsame Linie: Rand der A-Zelle an der Tuer
    TA, TB = t['TA'], t['TB']
    lx = (px(a, TA[0]) + px(b, TB[0])) / 2; ly = (py(a, TA[1]) + py(b, TB[1])) / 2
    best = None
    for c in ORTE[a]['pz']:
        if sei in 'SN':
            if not (c[0] <= lx <= c[1]): continue
            y = c[3] if sei == 'S' else c[2]
            d = abs(y - ly)
            if best is None or d < best[0]: best = (d, int(round(lx)), y)
        else:
            if not (c[2] <= ly <= c[3]): continue
            x = c[1] if sei == 'E' else c[0]
            d = abs(x - lx)
            if best is None or d < best[0]: best = (d, x, int(round(ly)))
    kind = {'N': 0, 'E': 1, 'S': 2, 'W': 3}[sei]
    mx, my = best[1], best[2]
    # laengs in die Ueberdeckung der beiden Zellen klemmen, 3 px Abstand zu jeder Ecke (Balken +-2 px)
    for ca in ORTE[a]['pz']:
        for cb in ORTE[b]['pz']:
            if sei in 'SN' and (ca[3] if sei == 'S' else ca[2]) == my and (cb[2] if sei == 'S' else cb[3]) == my:
                lo, hi = max(ca[0], cb[0]) + 3, min(ca[1], cb[1]) - 3
                if lo <= hi and lo - 3 <= mx <= hi + 3: mx = min(max(mx, lo), hi)
            if sei in 'EW' and (ca[1] if sei == 'E' else ca[0]) == mx and (cb[0] if sei == 'E' else cb[1]) == mx:
                lo, hi = max(ca[2], cb[2]) + 3, min(ca[3], cb[3]) - 3
                if lo <= hi and lo - 3 <= my <= hi + 3: my = min(max(my, lo), hi)
    marken.append(dict(von=a, nach=b, x=mx, y=my, kind=kind, raum=t['ra'], slot=t['sa']))
# ROOM11D0 -> ROOM1260 und ROOM11B0 -> ROOM1260 sind Bodenloecher (Kreiszellen @0x00A10 bzw. @0x00EA4 unter dem
# Trigger) - keine Wandtuer, keine Marke.
# Treppen ROOM10A0 Band 4 (sce 13 = Achse Z -> waagerechte Sprossen, kind 4): Mitten, in den Innenraum geklemmt
for (sx, sz, quelle) in ((25350, 22350, '@0x00E7C'), (21450, 20650, '@0x00E90')):
    c = ORTE['10a0']['pz'][0]
    x = min(max(px('10a0', sx), c[0] + 3), c[1] - 3); y = min(max(py('10a0', sz), c[2] + 3), c[3] - 3)
    marken.append(dict(von='10a0', nach=None, x=x, y=y, kind=4, raum=0x10A0, slot=quelle))
for m in marken: print('MARKE', m)

# ---------------------------------------------------------------------------------------------------------
# VORSCHAU (Klassenfarben wie komp.py: Panel blau, Koerper gruen, Wand grau, Tuer gelb, Treppe hell)
if len(sys.argv) > 2:
    from PIL import Image
    img = np.zeros((240, 320, 3), np.uint8); img[:] = (0, 16, 88)
    rand = np.zeros((240, 320), bool); koerper = np.zeros((240, 320), bool)
    for n, o in ORTE.items():
        eig = np.zeros((240, 320), bool)
        for (x0, x1, y0, y1) in o['pz']: eig[y0:y1 + 1, x0:x1 + 1] = True
        koerper |= eig
        for (x0, x1, y0, y1) in o['pz']:
            for y in range(y0, y1 + 1):
                for x in range(x0, x1 + 1):
                    if x in (x0, x1) or y in (y0, y1):
                        # Rand nur, wo keine Nachbarzelle DESSELBEN Orts anschliesst (wie der Zeichner)
                        nb = [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)]
                        if any(not (0 <= yy < 240 and 0 <= xx < 320 and eig[yy, xx]) for xx, yy in nb): rand[y, x] = True
    img[koerper] = (16, 56, 40); img[rand] = (176, 176, 176)
    for m in marken:
        if m['kind'] <= 3:
            if m['kind'] in (0, 2): img[m['y'], m['x'] - 2:m['x'] + 3] = (224, 168, 40)
            else: img[m['y'] - 2:m['y'] + 3, m['x']] = (224, 168, 40)
        else:
            for r in (-2, 0, 2): img[m['y'] + r, m['x'] - 2:m['x'] + 3] = (240, 240, 216)
    Image.fromarray(img).resize((960, 720), Image.NEAREST).save(sys.argv[2]); print('Vorschau', sys.argv[2])

# ---------------------------------------------------------------------------------------------------------
for n, o in ORTE.items():
    print('ORT %-6s Raum %04X Lage %s c %d f %d Zellen %s' % (n, o['raum'], lage[n], o['c'], o['f'], o['pz']))

# ---------------------------------------------------------------------------------------------------------
# SELBSTPROBE: jeder begehbare Rasterpunkt (Flut r 0) eines Orts faellt mit SEINER Affine in seine Zellen?
QUELLE = {'gang': ['1180'], 'halle': ['1190H'], 'ost': ['1190O'], '11b0': ['11B0', '11B0E'], '11c0': ['11C0'],
          '11d0': ['11D0'], 'kammer': ['11D0K'], '10a0': ['10A0']}
for n, qs in QUELLE.items():
    aus = 0; ges = 0
    for q in qs:
        g = G[q]; k, i = np.nonzero(g['a'])
        for kk, ii in zip(k[::7], i[::7]):
            x = g['x0'] + ii * g['st']; z = g['z0'] + kk * g['st']
            mx, my = px(n, x), py(n, z); ges += 1
            if not any(c[0] <= mx <= c[1] and c[2] <= my <= c[3] for c in ORTE[n]['pz']): aus += 1
    print('SELBSTPROBE %-6s %d Punkte, ausserhalb der Zellen %d' % (n, ges, aus))

# ---------------------------------------------------------------------------------------------------------
# C-ZEILEN (re15_map_zones.h): s_map_synth ab Index 3 (synth = Index+1), s_map_synth_cells ab Index 3
REIHE = ['gang', 'halle', 'ost', '11b0', '11c0', '11d0', '10a0', 'kammer']
erste = 3
print('/* s_map_synth */')
for idx, n in enumerate(REIHE):
    o = ORTE[n]; pz = o['pz']
    x0 = min(c[0] for c in pz); x1 = max(c[1] for c in pz); y0 = min(c[2] for c in pz); y1 = max(c[3] for c in pz)
    o['synth'] = 3 + idx + 1
    print('    { %3d, %3d, %3d, %3d, %2d, %d, %d, 0, %d, 0, %d, %d },  /* B1 %s (Raum %04X) */'
          % (x0, y0, x1 - x0 + 1, y1 - y0 + 1, erste, len(pz), A, o['c'], -A, o['f'], n, o['raum']))
    erste += len(pz)
print('/* s_map_synth_cells */')
for n in REIHE:
    print('    ' + ' '.join('{ %3d, %3d, %3d, %3d },' % (c[0], c[2], c[1] - c[0] + 1, c[3] - c[2] + 1) for c in ORTE[n]['pz'])
          + '   /* %s */' % n)

ZID = {'gang': 25, 'halle': 26, 'ost': 110, '11b0': 28, '11c0': 29, '11d0': 30, '10a0': 10, 'kammer': 111}
print('/* s_map_marks Blatt 0 */')
for m in marken:
    z2 = ZID[m['nach']] if m['nach'] else 255
    print('    {  0, 255, %4d, %4d, %d, %3d, %3d, 0 },   /* %s -> %s (ROOM%04X %s) */'
          % (m['x'], m['y'], m['kind'], ZID[m['von']], z2, m['von'], m['nach'] or '-', m['raum'], m['slot']))
