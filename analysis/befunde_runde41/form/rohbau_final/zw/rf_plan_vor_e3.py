#!/usr/bin/env python3
"""Rohbau final, Bauplan: je Knochen Ringstationen in Spieleinheiten. Grosse Form zuerst. Mitte spiegelgleich, links ist der Spiegel von rechts.
Herkunft: Rumpf mit Kuppel-Enden und Teleskop-Rand (A, B), Schwanz, Bein, Unterarm aus der Passung von C, Hand und Krallen nach A, Kopf mit Hufeisen-Kamm nach A und B."""
import numpy as np

from rf_kern import lade_gerippe, namen, Teil, ringecken, FINGER, HAND_L, KRALLE


def achsen(k, spiegel=False):
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


def halb(h):
    h = list(h)
    return h + h[-2:0:-1]


def teil(k, st, n, spiegel=False, name=None, **kw):
    """st: Liste (t, ring, ca[, cb]). ring = (a, b) oval, Halbprofil (n/2+1 Werte) oder voller Ring (n Werte)."""
    o, d, ea, eb = achsen(k, spiegel)
    t, r, c = [], [], []
    for s in st:
        q = list(s[1])
        if len(q) == 2:
            rr = oval(q[0], q[1], n)
        elif len(q) == n // 2 + 1:
            rr = np.array(halb(q), float)
        else:
            rr = np.array(q, float)
        t.append(s[0])
        r.append(rr)
        c.append([s[2] if len(s) > 2 else 0.0, s[3] if len(s) > 3 else 0.0])
    return Teil(name or k['name'], k['index'], o, d, ea, eb, t, c, np.array(r), **kw)


def _basis(d):
    d = np.asarray(d, float)
    d = d / np.linalg.norm(d)
    h = np.array([0, 1.0, 0]) if abs(d[1]) < .8 else np.array([1.0, 0, 0])
    a = h - (h @ d) * d
    a /= np.linalg.norm(a)
    return d, a, np.cross(d, a)


def kegel(name, kn, p, d, r, lang, n=4, knick=None, feld=None, wurzel=None, spiegel=False):
    """Spitzer kantiger Kegel mit Knickring. wurzel = (t, Faktor): zweiter Ring. p, d im Knochenraum."""
    p = np.asarray(p, float)
    dd, a, b = _basis(d)
    if spiegel:
        p = p * np.array([1, 1, -1.0])
        dd, a, b = _basis(np.asarray(d, float) * np.array([1, 1, -1.0]))
        b = -b
    tt = [0.0]
    rr = [[r] * n]
    cc = [[0.0, 0.0]]
    if wurzel:
        tt.append(wurzel[0])
        rr.append([r * wurzel[1]] * n)
        cc.append([0.0, 0.0])
    ap = (0.0, 0.0) if knick is None else knick
    return Teil(name, kn, p, dd, a, b, tt, cc, np.array(rr, float), spitze0=40.0, spitze1=lang, apex1=ap, feld=feld)


# Rumpf: drei geschlossene Stuecke. Teleskop: vorderes Stueck legt den Rand ueber das hintere, dessen Kuppel im Inneren laeuft. n = 10, j=0 Bauch, 5 Ruecken.
HUEFTE = [(-800, [150, 170, 210, 230, 260, 280], 30), (-690, [300, 330, 400, 420, 480, 540], 20), (-510, [460, 500, 580, 590, 620, 670], 20), (-260, [590, 640, 740, 750, 680, 680], 20),
          (-40, [680, 730, 810, 800, 680, 650], 20), (120, [720, 760, 820, 800, 680, 650], 20), (240, [690, 720, 760, 730, 630, 600], 20), (460, [710, 720, 730, 690, 630, 610], 20),
          (640, [610, 620, 630, 600, 550, 530], 20), (770, [360, 370, 380, 360, 330, 320], 20)]
WIRBEL = [(-360, [690, 710, 730, 710, 640, 610], 0), (-320, [790, 800, 800, 770, 700, 680], 0), (-130, [800, 810, 800, 770, 700, 680], 0), (120, [810, 800, 770, 740, 720, 720], 0),
          (360, [800, 770, 720, 720, 760, 800], 0), (560, [800, 750, 690, 700, 780, 860], 0), (760, [740, 700, 640, 650, 730, 800], 0), (960, [600, 580, 530, 540, 610, 670], 0), (1090, [320, 310, 290, 290, 320, 350], 0)]
BRUST = [(-360, [720, 690, 630, 630, 730, 830], 0), (-320, [840, 800, 700, 710, 830, 950], 0), (-100, [900, 830, 700, 720, 860, 980], 0), (140, [920, 830, 700, 720, 860, 990], 0), (420, [800, 750, 680, 700, 800, 920], 0),
         (640, [690, 680, 670, 690, 740, 810], 0), (820, [600, 610, 640, 660, 680, 720], 0), (960, [470, 500, 570, 610, 580, 600], 0), (1060, [300, 330, 360, 380, 370, 370], 0)]

BRUST_KAPPE = ((0, -330.0), (1, -330.0), (2, -220.0), (3, -80.0))

HALS_KEGEL = 380.0      # M5 Abnahme 2: Faecher taucht als Kegel in die Brust
KOPF_SPITZE, KIEFER_SPITZE = 230.0, 200.0      # M5 Abnahme 2: Stirn und Kinn sind kein Deckel

# Schwanz, n = 8, j=0 unten, 4 oben. Wurzel als Keil aus dem Becken, durchgehende Verjuengung, Ende breit.
SCHWANZ = [('schwanz1', [(-470, [190, 205, 235, 230, 240], 178), (-260, [280, 293, 298, 292, 312], 99), (-60, [432, 391, 328, 308, 343], 10), (220, [588, 471, 339, 300, 338], -42), (470, [599, 478, 341, 299, 335], -13), (650, [512, 431, 326, 291, 326], 13)]),
           ('schwanz2', [(-110, [392, 343, 275, 255, 287], -20), (200, [388, 363, 315, 303, 338], -30), (420, [341, 337, 316, 311, 346], -25), (650, [316, 326, 317, 315, 351], -6)]),
           ('schwanz3', [(-110, [340, 345, 335, 330, 365], 30), (210, [276, 298, 302, 309, 343], 25), (430, [210, 228, 232, 240, 267], 15), (680, [166, 178, 186, 194, 217], 0)]),
           ('schwanz4', [(-110, [200, 215, 225, 230, 255], -5), (220, [207, 222, 229, 235, 262], -10), (450, [162, 174, 179, 184, 205], -10), (710, [133, 143, 147, 151, 169], -6)]),
           ('schwanz5', [(-110, [140, 150, 155, 160, 182], -10), (230, [162, 173, 177, 182, 204], 0), (730, [167, 178, 180, 182, 201], 15)]),
           ('schwanz6', [(-110, [150, 160, 165, 165, 182], 25), (220, [163, 176, 178, 182, 202], 20), (440, [191, 205, 212, 218, 244], 0), (660, [181, 195, 205, 214, 240], -20)])]

SCHWANZ_KEGEL = (770.0, 346.0)     # Faechermitte hinter Ring 0, laengs und nach unten (Knochenraum), Spitze liegt in allen Posen im Rumpf

# Glieder. Ecken: j=0 vorn, 2 aussen, 4 hinten, 6 innen (Y-Knochen).
OBERSCHENKEL = [(-50, (150, 150)), (20, [219, 210, 198, 205, 216, 211, 203, 211]), (90, [331, 298, 258, 288, 326, 297, 263, 296]), (380, [334, 291, 247, 287, 329, 288, 246, 293]),
                (690, [266, 233, 224, 248, 269, 231, 219, 244]), (930, [230, 216, 215, 227, 234, 216, 210, 221]), (1000, [184, 178, 175, 183, 188, 178, 171, 177])]
UNTERSCHENKEL = [(-20, [143, 135, 133, 131, 128, 128, 128, 131]), (70, [276, 237, 215, 236, 260, 237, 212, 231]), (300, [274, 242, 223, 268, 329, 282, 226, 237]), (590, [236, 210, 197, 215, 239, 224, 203, 210]),
                 (810, [201, 185, 181, 189, 199, 194, 187, 188]), (890, [185, 172, 170, 176, 182, 179, 175, 175])]
FERSE = 170.0      # M2 Abnahme 2: Faechermitte herausgezogen, kein Deckel am Unterschenkel
KNIE = 45.0      # Abnahme 2: Oberschenkel endet knapp hinter dem Kniegelenk (Knochen 953), sonst steht er gebeugt als Stumpf oder Haken aus dem Knie
FERSE_FUSS = 110.0      # M2 Abnahme 2: Die Ferse gehoert dem Fuss, der Unterschenkel verjuengt sich zum Knoechel
# Fuss: Pranke, flach, vorn breit. j=0 Sohle.
FUSS = [(-90, (135, 145)), (130, (170, 180)), (440, (195, 215)), (740, (175, 245)), (900, (120, 235))]
OBERARM = [(-270, (150, 150)), (-130, [262, 268, 270, 258, 250, 252, 254, 258]), (60, [340, 372, 385, 358, 325, 325, 328, 335]), (300, [345, 328, 313, 316, 324, 314, 309, 326]),
           (550, [278, 260, 250, 257, 270, 263, 260, 270]), (790, [216, 212, 216, 219, 221, 224, 227, 224]), (900, [170, 168, 170, 174, 176, 178, 180, 176])]
# Unterarm: flach (breit quer zum Blick von Bild 1), Ringe eng gesetzt, damit die Rippen des Streifens G auf Ringen sitzen. Elle hinten als Kante, Kappe Z5 am Ellbogen.
UNTERARM = [(-130, [150, 150, 170, 170, 170, 150, 170, 150]), (60, [245, 240, 280, 295, 325, 265, 275, 245]), (240, [255, 262, 300, 295, 275, 262, 295, 262]), (400, [250, 262, 305, 285, 255, 250, 300, 258]),
            (560, [246, 258, 300, 278, 246, 246, 298, 252]), (720, [236, 248, 285, 265, 236, 238, 288, 244]), (880, [225, 238, 270, 250, 225, 228, 275, 234]), (1040, [222, 232, 265, 245, 222, 225, 270, 232]), (1190, [175, 185, 212, 195, 175, 178, 212, 188])]
# Hand: Pranke (A). n = 8, X-Knochen: j=0 Handflaeche, 4 Handruecken, 2 innen (rechte Hand +Z), 6 aussen.
# M4 Abnahme 1: Pranke etwas laenger (Stationen 1,12fach), die Handmaske von Bild 2 liegt in Hand und Fingern, nicht in einem Aussenfinger als Blatt. Laenger geht nicht: Bild 1 sieht die Hand von hinten.
HAND = [(-130, (200, 225)), (67, (245, 330)), (360, (232, 465)), (650, (195, 520)), (900, (165, 535)), (1060, (110, 455))]


def finger(k, spiegel, nr):
    (_, w, L) = FINGER[nr]
    b = {'1': 150.0, '2': 185.0, '3': 135.0, '4': 165.0}[nr]      # Mittelfinger am staerksten, Aussenfinger schlank
    # Mitte auf der gekruemmten Linie: Krallen biegen zum Boden (+a). Querschnitt schmal.
    if nr == '3':
        # Aussenfinger: Saebel, kruemmt nach aussen. In Bild 2 laeuft er so in der Maske, nicht ueber das Feuer.
        st = [(-90, (b * .82, b * .95), -10, 0), (L * .25, (b * 1.0, b * 1.08), 0, L * .01), (L * .52, (b * .76, b * .78), L * .01, -L * .015), (L * .72, (b * .42, b * .42), L * .02, -L * .075)]
        return teil(k, st, 6, spiegel, spitze0=40, spitze1=L * .28, apex1=(L * .035, -L * .15))
    st = [(-90, (b * .82, b * .95), -10), (L * .25, (b * 1.0, b * 1.08), -10), (L * .52, (b * .76, b * .78), L * .02), (L * .72, (b * .42, b * .42), L * .075)]
    return teil(k, st, 6, spiegel, spitze0=40, spitze1=L * .28, apex1=(L * .11, 0))


def kralle(hand, spiegel, name):
    """Fuenfte Kralle, starr an der Hand. Gleiche Bauart wie die Finger, ein Ring weniger."""
    (x, y, z), w, L = KRALLE
    vz = -1.0 if spiegel else 1.0
    a = np.radians(w) * vz
    o = np.array([x, y, z * vz], float)
    b = 165.0
    k = dict(achse_von=o, achse_bis=o + np.array([np.cos(a), 0, np.sin(a)]) * L, querachse_a='Y', name=name, index=hand['index'])
    st = [(-90, (b * .84, b * .95), -10), (L * .32, (b * 1.0, b * 1.05), -10), (L * .68, (b * .58, b * .58), L * .05)]
    return teil(k, st, 6, spiegel, name=name, spitze0=40, spitze1=L * .32, apex1=(L * .11, 0))


def kopf(k):
    """Kopf: Hufeisen-Kamm um das Gesicht, unten offen, Rand nach hinten gekippt (Zug Q laeuft ueber ihn), davor zurueckgesetzt Gesicht und schmale schwere Schnauze. n = 10, j=0 unten, 5 oben."""
    st = [(-230, [340, 350, 360, 370, 370, 360], 20), (60, [390, 410, 430, 450, 450, 440], 10), (300, [430, 510, 590, 640, 650, 650], 0), (470, [450, 570, 695, 735, 735, 720], 0),
          (530, [425, 500, 575, 605, 590, 575], 10), (650, [405, 455, 490, 500, 470, 470], 30), (800, [370, 410, 430, 400, 370, 400], 50), (960, [320, 360, 360, 320, 290, 320], 70),
          (1120, [260, 300, 290, 260, 235, 265], 80), (1240, [170, 215, 205, 185, 165, 185], 90)]
    t = teil(k, st, 10, spitze0=70, spitze1=KOPF_SPITZE, apex1=(10, 0))
    kipp = np.array(halb([130, 120, 50, -20, -80, -110]), float)
    for r, f in ((2, .45), (3, 1.0), (4, 1.0), (5, .5)):
        t.dt[r] = kipp * f
    # Brauengrat: Ecken 3 und 7 am Ring 5 vor, Augenhoehle dahinter
    t.dt[6, [3, 7]] += 26
    return t


def kiefer(k):
    """Unterkiefer: breit, flach, Kinn nach vorn. n = 8, j=0 unten, 4 oben."""
    st = [(-120, [200, 245, 340, 300, 175], 20), (180, [228, 280, 400, 370, 180], 50), (480, [230, 270, 390, 345, 140], 95), (720, [216, 242, 310, 265, 118], 130), (860, [150, 170, 205, 170, 85], 150)]
    return teil(k, st, 8, spitze0=30, spitze1=KIEFER_SPITZE)


def lappen(T, idx):
    """Muskellappen als Ringfolge (Bild 1: Lappen und Baender an den Beinen, Stufen im Umriss nach Rohbau B). Nur Rueck- und Aussenseite, relief bleibt bei der Passung stehen."""
    HI = (2, 3, 4, 5, 6)
    W = (.5, .9, 1.0, .9, .5)
    for nm, ringe in (('oberschenkel', {3: .07, 4: -.06, 5: .05}), ('unterschenkel', {2: .06, 3: -.07, 4: .05})):
        for sd in '_r', '_l':
            p = T[idx[nm + sd]]
            for r, a in ringe.items():
                for j, w in zip(HI, W):
                    p.relief[r, j] += a * w


def kleiner(p, k_):
    """Teil als Ganzes um den Knochenursprung skalieren. Radien und Mitten folgen ueber p.skala in der Bindung."""
    p.skala = k_
    p.t = p.t * k_
    p.o = p.o * k_
    p.spitze0 *= k_
    p.spitze1 *= k_
    p.apex0 = p.apex0 * k_
    p.apex1 = p.apex1 * k_


def plan(g=None):
    g = g or lade_gerippe()
    ix = namen(g)
    K = g['knochen']
    T = []
    idx = {}

    def k(n):
        return K[ix[n]]

    def dazu(t, bind=None):
        idx[t.name] = len(T)
        t.bind = bind
        T.append(t)
        return len(T) - 1

    dazu(teil(k('huefte'), HUEFTE, 10, spitze0=70, spitze1=70), 'sym')
    dazu(teil(k('wirbel'), WIRBEL, 10, spitze0=40, spitze1=70), 'sym')
    dazu(teil(k('brust'), BRUST, 10, spitze0=40, spitze1=100), 'sym')
    # M3 Abnahme 1: Knick Wirbel/Brust geht von -32 (Pose 1) bis +47 Grad (Pose 2), das Gelenk ist ein Kugelgelenk mitten im Rumpf. Darum laeuft die Rueckenseite der Brust als Kappe um den Gelenkpunkt nach hinten,
    # Bauch und Flanke bleiben: Hinterkante steht schraeg, in Pose 2 deckt die Kappe das Tal zwischen Brustdach und Wirbelbuckel, in Ruhe und Pose 1 liegt sie im Wirbel.
    kb = np.array(halb([0, 0, .1, .45, .85, 1.0]), float)
    for r, f in BRUST_KAPPE:
        T[idx['brust']].dt[r] = kb * f
    for i, (n, st) in enumerate(SCHWANZ):
        # M1 Abnahme 2: Die Wurzel ist kein Deckel, sie laeuft als langer Kegel in den Ruecken. Nur die Faechermitte, die Ringe tragen die Oberkante von Bild 1.
        kw = dict(spitze0=SCHWANZ_KEGEL[0], apex0=(SCHWANZ_KEGEL[1], 0)) if i == 0 else dict(spitze0=30)
        dazu(teil(k(n), st, 8, spitze1=(420 if i == 5 else 40), **kw), 'sym')
    # Beine
    for s, sp in (('r', False), ('l', True)):
        for n, st, kw in (('oberschenkel', OBERSCHENKEL, dict(spitze0=130, apex0=(0, -110), spitze1=KNIE)), ('unterschenkel', UNTERSCHENKEL, dict(spitze0=80, spitze1=FERSE)), ('fuss', FUSS, dict(spitze0=FERSE_FUSS, spitze1=90))):
            t = teil(k(n + '_' + s), st, 8, sp, **kw)
            dazu(t, None if s == 'r' else idx[n + '_r'])
        # M2 Abnahme 2: Zehen mit Wurzelring und kraeftigem Ansatz, Klaue statt Dorn
        for i, (z, dz, l) in enumerate(((-200, -.32, 330), (10, 0.0, 390), (210, .32, 330))):
            nm = 'zeh%d_%s' % (i, s)
            t = kegel(nm, ix['fuss_' + s], (820, 80, z), (1, .12, dz), 165, l, n=4, knick=(l * .2, 0), feld='zeh', spiegel=sp, wurzel=(200, .8))
            dazu(t, None if s == 'r' else idx['zeh%d_r' % i])
    # Arme
    for s, sp in (('r', False), ('l', True)):
        for n, st, kw in (('oberarm', OBERARM, dict(spitze0=60, spitze1=50)), ('unterarm', UNTERARM, dict(spitze0=50, spitze1=30)), ('hand', HAND, dict(spitze0=30, spitze1=30))):
            t = teil(k(n + '_' + s), st, 8, sp, **kw)
            dazu(t, None if s == 'r' else idx[n + '_r'])
        for f in '1234':
            nm = 'klaue_%s%s' % (s, f)
            dazu(finger(k(nm), sp, f), None if s == 'r' else idx['klaue_r' + f])
        dazu(kralle(k('hand_' + s), sp, 'kralle_' + s), None if s == 'r' else idx['kralle_r'])
        if s == 'l':
            for nm in ['hand_l', 'kralle_l'] + ['klaue_l' + f for f in '1234']:
                kleiner(T[idx[nm]], HAND_L)
    lappen(T, idx)
    # Hals, Kopf, Kiefer
    dazu(teil(k('hals'), [(-260, (520, 540), 50), (520, (470, 480), 0), (640, (420, 430), 0)], 8, spitze0=HALS_KEGEL, spitze1=50), 'sym')
    dazu(kopf(k('kopf')), 'sym')
    dazu(kiefer(k('kiefer')), 'sym')
    # Fangzaehne: zwei oben, zwei unten, gross und kantig. Die Reihe der kleinen Zaehne liegt in der Textur (RE2: Kopf 97 Flaechen, Zaehne gemalt).
    for s, vz in (('r', -1), ('l', 1)):
        dazu(kegel('fang_o_' + s, ix['kopf'], (1040, 250, vz * 230), (.15, 1, vz * .08), 92, 300, n=4, feld='zahn'))
        dazu(kegel('fang_u_' + s, ix['kiefer'], (640, 40, vz * 215), (.1, -1, vz * .06), 80, 230, n=4, feld='zahn'))
        # Wangenspitze: Ende des Kamms, Bild 2 Stelle P (366,304)
        dazu(kegel('wange_' + s, ix['kopf'], (560, 300, vz * 465), (-.15, 1, vz * .05), 112, 370, n=4, wurzel=(150, .78)))
    g['_plan'] = dict(index=idx)
    return g, T


# Teleskop: (hinteres Teil, Ring, vorderes Teil, Ring, log-Abstand, Ecken): Ring des hinteren Stuecks unter dem Rand bleibt um diesen Faktor kleiner. Brust: nur Bauch und Flanke, am Ruecken laeuft die Wirbelseite ohne Rand in die Brust (M3).
TELESKOP = [('huefte', 6, 'wirbel', 1, -0.10, list(range(10))), ('wirbel', 6, 'brust', 1, -0.10, [0, 1, 2, 8, 9])]
