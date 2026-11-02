#!/usr/bin/env python3
"""Rohbau C, Bauplan: je Knochen Ringstationen, Startradien aus gerippe.json, Ecken je Ring, Reliefs. Alle Stationen sind Vorgaben des Modellbaus, gemessen wird erst in der Passung."""
import numpy as np

from rc_kern import lade_gerippe, namen, Teil, ringecken


def achsen(k, spiegel=False):
    """Knochen -> o, d, ea, eb. eb = d x ea wie gerippe. Fuer spiegel laeuft eb andersherum, damit die Eckenfolge der rechten Seite auf die linke passt."""
    o = np.array(k['achse_von'], float)
    d = np.array(k['achse_bis'], float) - o
    d /= np.linalg.norm(d)
    ea = np.array([0, 1.0, 0]) if k['querachse_a'] == 'Y' else np.array([1.0, 0, 0])
    ea = ea - (ea @ d) * d
    ea /= np.linalg.norm(ea)
    eb = np.cross(d, ea)
    if spiegel:
        eb = -eb
    return o, d, ea, eb


def oval(a, b, n):
    cs, sn = ringecken(n)
    return a * b / np.sqrt((b * cs) ** 2 + (a * sn) ** 2)


def station(k, n, st, spiegel=False, spitze0=0.0, spitze1=0.0, name=None, o=None, relief=None):
    """st = Liste (t, a, b[, ca, cb]). t = Einheiten ab achse_von."""
    o2, d, ea, eb = achsen(k, spiegel)
    t = [s[0] for s in st]
    r = np.array([oval(s[1], s[2], n) for s in st])
    c = np.array([[s[3] if len(s) > 3 else 0.0, s[4] if len(s) > 4 else 0.0] for s in st])
    return Teil(name or k['name'], k['index'], o2 if o is None else o, d, ea, eb, t, c, r, relief=relief, spitze0=spitze0, spitze1=spitze1)


def finger(k, spiegel, wurzel=165.0, n=6, lang=None, name=None):
    """Finger: dicke Wurzel, Knoechel, Nagelbett, dann die Klaue als spitzer Kegel. lang = sichtbare Laenge bis zur Spitze."""
    L = lang or 800.0
    st = [(-100, wurzel * .85, wurzel * .95), (L * .12, wurzel, wurzel * 1.05), (L * .32, wurzel * .86, wurzel * .9), (L * .48, wurzel * .76, wurzel * .78), (L * .6, wurzel * .6, wurzel * .62), (L * .8, wurzel * .3, wurzel * .32)]
    return station(k, n, st, spiegel, spitze0=40, spitze1=L * .2, name=name)


def plan(g=None):
    g = g or lade_gerippe()
    ix = namen(g)
    K = g['knochen']
    T = []
    idx = {}

    def k(n):
        return K[ix[n]]

    def dazu(t):
        idx[t.name] = len(T)
        T.append(t)

    # Rumpf: drei Knochen, 10 Ecken. j=0 Bauch, j=5 Ruecken. Stationen ueberlappen an den Gelenken.
    dazu(station(k('huefte'), 10, [(-710, 330, 470), (-510, 560, 740), (-200, 690, 900), (150, 730, 930), (480, 850, 916), (610, 860, 900)], spitze0=110, spitze1=70))
    dazu(station(k('wirbel'), 10, [(-180, 840, 900), (80, 880, 900), (440, 930, 830), (800, 980, 730), (1000, 1000, 710)], spitze0=60, spitze1=60))
    dazu(station(k('brust'), 10, [(-180, 985, 720), (100, 1000, 720), (450, 860, 700), (760, 680, 690), (960, 520, 640)], spitze0=60, spitze1=110))
    # Schwanz: sechs Knochen, 8 Ecken, j=0 unten, j=4 oben
    # Der Schwanz waechst aus dem Becken: vorderes Ende taucht in den Ruecken ein (Mitte nach unten), kein Balken auf dem Ruecken.
    sch = [('schwanz1', [(-470, 210, 250, 340), (-260, 280, 310, 220), (-60, 315, 325, 90), (220, 325, 325), (470, 335, 335), (650, 335, 335)]),
           ('schwanz2', [(-60, 338, 338), (200, 342, 342), (420, 340, 340), (650, 335, 335)]),
           ('schwanz3', [(-60, 335, 335), (210, 285, 285), (430, 245, 245), (680, 222, 222)]),
           ('schwanz4', [(-60, 222, 222), (220, 192, 192), (450, 174, 174), (710, 168, 168)]),
           ('schwanz5', [(-60, 170, 170), (230, 176, 176), (460, 182, 182), (730, 196, 196)]),
           ('schwanz6', [(-60, 200, 200), (220, 216, 216), (440, 225, 225), (660, 190, 190)])]
    for i, (n, st) in enumerate(sch):
        dazu(station(k(n), 8, st, spitze0=30, spitze1=(420 if i == 5 else 40)))
    # Beine: j=0 vorn, j=2 aussen (rechts), j=4 hinten
    for s, sp in (('r', False), ('l', True)):
        dazu(station(k('oberschenkel_' + s), 8, [(-250, 130, 120), (-120, 215, 200), (90, 310, 262), (380, 300, 250), (690, 255, 215), (930, 215, 200), (1060, 170, 160)], sp, spitze0=50, spitze1=50))
        dazu(station(k('unterschenkel_' + s), 8, [(-110, 170, 170), (70, 250, 214), (300, 245, 205), (590, 190, 165), (810, 150, 142), (890, 135, 130)], sp, spitze0=50, spitze1=40))
        dazu(station(k('fuss_' + s), 8, [(-90, 130, 140), (130, 165, 178), (440, 213, 192), (740, 280, 205), (900, 300, 200)], sp, spitze0=50, spitze1=120))
    # Arme
    for s, sp in (('r', False), ('l', True)):
        dazu(station(k('oberarm_' + s), 8, [(-270, 140, 140), (-130, 250, 250), (60, 335, 335), (300, 335, 335), (550, 300, 300), (790, 280, 280), (900, 230, 230)], sp, spitze0=60, spitze1=50))
        dazu(station(k('unterarm_' + s), 8, [(-130, 225, 225), (60, 305, 305), (320, 300, 300), (620, 268, 268), (900, 255, 255), (1100, 255, 255), (1190, 200, 200)], sp, spitze0=50, spitze1=30))
        dazu(station(k('hand_' + s), 8, [(-110, 210, 215), (110, 270, 300), (360, 215, 400), (600, 150, 440), (720, 100, 380)], sp, spitze0=30, spitze1=30))
        LANG = {'1': 760, '2': 800, '3': 960, '4': 700}
        for f in '1234':
            dazu(finger(k('klaue_%s%s' % (s, f)), sp, lang=LANG[f]))
    # Hals, Kopf, Kiefer: j=0 unten, j=5 oben
    dazu(station(k('hals'), 8, [(-240, 440, 450), (40, 455, 460), (280, 480, 480), (520, 510, 510), (640, 480, 480)], spitze0=50, spitze1=50))
    dazu(kopf_teil(k('kopf')))
    dazu(kiefer_teil(k('kiefer')))
    for t in T:
        t.relief = relief_von(t)
    g['_plan'] = dict(index=idx)
    return g, T


def symm(h):
    """Halbprofil j = 0 (unten) .. n/2 (oben) -> Ring, links gleich rechts."""
    h = list(h)
    return h + h[-2:0:-1]


def kopf_teil(k):
    """Schaedel [f] ausser dem Rand: Scheitelkamm auf Zug Q (Kreis Radius rund 700), Brauengrat, Augenhoehle, Jochbein, Nasenruecken. n = 10, j = 0 unten, 5 oben."""
    o, d, ea, eb = achsen(k)
    st = [(-200, [430, 440, 450, 460, 450, 440], 0), (60, [480, 500, 530, 550, 550, 540], 0), (300, [500, 560, 620, 680, 720, 740], 0), (430, [490, 540, 600, 650, 670, 670], 10),
          (600, [460, 510, 580, 660, 730, 640], 30), (720, [430, 480, 560, 520, 520, 520], 70), (860, [380, 460, 560, 470, 400, 430], 110), (1000, [320, 400, 440, 400, 340, 360], 135),
          (1140, [270, 340, 360, 340, 300, 330], 150), (1260, [190, 250, 260, 260, 250, 280], 165)]
    r = np.array([symm(s[1]) for s in st], float)
    c = np.array([[s[2], 0.0] for s in st], float)
    return Teil('kopf', k['index'], o, d, ea, eb, [s[0] for s in st], c, r, spitze0=60, spitze1=70)


def kiefer_teil(k):
    """Unterkiefer [f]: breit, flach, Kinn nach vorn. n = 8, j = 0 unten, 4 oben."""
    o, d, ea, eb = achsen(k)
    st = [(-120, [200, 240, 380, 300, 170], 60), (180, [230, 280, 440, 380, 180], 120), (480, [240, 280, 430, 360, 140], 190), (720, [230, 260, 340, 280, 120], 240), (880, [170, 190, 230, 190, 90], 260)]
    r = np.array([symm(s[1]) for s in st], float)
    c = np.array([[s[2], 0.0] for s in st], float)
    return Teil('kiefer', k['index'], o, d, ea, eb, [s[0] for s in st], c, r, spitze0=30, spitze1=60)


def glocke(x, m, b):
    return np.exp(-((x - m) / b) ** 2)


def relief_von(t):
    """Feste Wuelste und Grate als log-Faktoren je Ecke. Die Passung sieht sie mit, ihre Regeln gelten nur der Grundform. Alles [f], grosse Form zuerst."""
    n, T_ = t.n, t.T
    R = np.zeros((T_, n))
    nm = t.name
    s = t.t

    def eck(*a):
        v = np.zeros(n)
        for q, w in a:
            v[q % n] = w
        return v
    if nm in ('huefte', 'wirbel', 'brust'):
        R += eck((5, .05), (4, -.01), (6, -.01))[None, :]            # Wirbelgrat
        if nm == 'brust':
            R += glocke(s, 100, 300)[:, None] * eck((3, .05), (7, .05), (4, .06), (6, .06), (2, .04), (8, .04), (0, .06), (1, .06), (9, .06))[None, :]   # Schulterblaetter, Brustkasten
        if nm == 'wirbel':
            R += glocke(s, 440, 280)[:, None] * eck((0, -.10), (1, -.13), (9, -.13), (2, -.07), (8, -.07), (3, -.03), (7, -.03))[None, :]   # Taille
        if nm == 'huefte':
            R += glocke(s, 0, 300)[:, None] * eck((3, .05), (7, .05))[None, :]                                              # Beckenkamm
            # Beckenbuckel: Oberseite hinten steigt in die Schwanzwurzel, der Schwanz waechst aus dem Koerper
            R[0, [4, 5, 6]] += (.40, .55, .40)
            R[1, [4, 5, 6]] += (.25, .35, .25)
            R[2, [4, 5, 6]] += (.10, .14, .10)
    if nm.startswith('schwanz'):
        R += eck((4, .14), (3, .03), (5, .03), (0, -.10), (1, -.03), (7, -.03))[None, :]
        # Schwanzwurzel tief: Unterseite haengt als Keil bis an das Becken, der Schwanz waechst aus dem Rumpf, keine Bruecke darueber. Von hinten (Bild 1) liegt sie hinter dem Schwanz.
        F = {'schwanz1': (0, .2, .55, .85, .85, .7), 'schwanz2': (.6, .4, .22, .1), 'schwanz3': (.06, 0, 0, 0)}.get(nm)
        if F:
            R += np.array(F)[:, None] * eck((0, 1.0), (1, .65), (7, .65), (2, .2), (6, .2))[None, :]
    if nm.startswith('oberschenkel'):
        R += glocke(s, 300, 260)[:, None] * eck((0, .10), (1, .06), (7, .06), (4, .07), (3, .03), (5, .03), (2, -.04), (6, -.04))[None, :]
    if nm.startswith('unterschenkel'):
        R += glocke(s, 0, 110)[:, None] * eck((0, .12), (1, .05), (7, .05))[None, :]                                        # Knie
        R += glocke(s, 260, 170)[:, None] * eck((4, .18), (3, .10), (5, .10))[None, :]                                      # Wade
        R += eck((0, .05))[None, :]                                                                                         # Schienbeingrat
    if nm.startswith('oberarm'):
        R += glocke(s, 60, 200)[:, None] * eck((1, .10), (2, .16), (3, .10))[None, :]                                       # Deltamuskel
        R += glocke(s, 420, 220)[:, None] * eck((0, .12), (1, .05), (7, .05), (4, .10), (3, .04), (5, .04))[None, :]        # Beuger, Strecker
    if nm.startswith('unterarm'):
        R += glocke(s, 40, 110)[:, None] * eck((4, .24), (3, .08), (5, .08))[None, :]                                       # Ellbogenkappe, Stelle Z5
        R += glocke(s, 320, 200)[:, None] * eck((2, .14), (1, .08), (3, .08), (6, .05))[None, :]                            # Strecker
        R += glocke(s, 1090, 90)[:, None] * eck((2, .08), (6, .08))[None, :]                                                # Handgelenk
    if nm.startswith('hand'):
        R += glocke(s, 620, 160)[:, None] * eck((3, .08), (5, .08))[None, :]                                                # Knoechel
    # Eingezogener Anfang: Der Kind-Teil taucht unter dem Eltern-Teil hervor, statt mit ihm auf gleicher Hoehe zu flimmern (Fenster an den Gelenken).
    if nm in ('wirbel', 'brust'):
        R[0] -= .18
        R[1] -= .05
    if nm == 'unterarm_r' or nm.startswith('unterschenkel'):
        R[0] -= .18
    if nm == 'unterarm_l':
        R[0] += .10          # Bild 2: Der Nutzer-Umriss des linken Arms beginnt an der Rumpfkante, der Unterarm steht am Ellbogen vor dem Oberarm
    if nm.startswith('schwanz') and nm != 'schwanz1':
        R[0] -= .12
    return R


def naehte(idx):
    """(Teil A, Ring A, Teil B, Ring B): Querschnitte, die am Gelenk zusammenpassen sollen."""
    N = [('huefte', 4, 'wirbel', 1), ('wirbel', 3, 'brust', 1)]
    for i in range(1, 6):
        a, b = 'schwanz%d' % i, 'schwanz%d' % (i + 1)
        N.append((a, 4 if i == 1 else 2, b, 1))
    for s in 'rl':
        N.append(('oberarm_' + s, 5, 'unterarm_' + s, 1))
        N.append(('oberschenkel_' + s, 5, 'unterschenkel_' + s, 1))
    return [(idx[a], ra, idx[b], rb) for a, ra, b, rb in N]


def spiegelpaare(idx):
    # Haende bleiben frei: Die linke ist im Bild flacher und kleiner als die rechte (FORMBLATT2 Abschnitt 7).
    return [(idx[n + 'r'], idx[n + 'l']) for n in ('oberarm_', 'oberschenkel_', 'unterschenkel_', 'fuss_')]


SYMM = ('huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer')
ENTWURF = dict(kiefer=30.0, kopf=20.0, hals=5.0)
