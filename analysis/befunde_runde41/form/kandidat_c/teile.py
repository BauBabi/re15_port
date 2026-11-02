# -*- coding: utf-8 -*-
"""Kandidat C: alle Teile im Aktorraum von Pose 1. Jedes Teil kommt aus seinem Umriss im Bild, Tiefe aus Kamera und Massstab."""
from modell import *
from schau import blatt as schau_blatt

ERLAUBT2 = ZIEL[2] > 0
_fuell = np.zeros((480, 640), np.uint8)
# Zwei Luecken im Soll-Umriss sind nur Polygonreste: Taille C1/C3 (FORMBLATT 6E, durchgehendes Bein) und Knick-Band Z7 zwischen Kugel und Innenglied (2.2, liegt auf der Figur)
cv2.fillPoly(_fuell, [np.array([(309, 342), (345, 342), (345, 349), (341, 355), (342, 362), (304, 362), (321, 353), (316, 350), (309, 348)], np.int32)], 1)
cv2.fillPoly(_fuell, [np.array([(232, 366), (252, 366), (254, 398), (232, 402)], np.int32)], 1)
cv2.fillPoly(_fuell, [np.array([(280, 326), (288, 326), (288, 346), (280, 346)], np.int32)], 1)        # Bein NL Kante rechts x=287 (Abschnitt 7), Polygon liegt 3 px innen
FIG1 = FIGUR[1] | _fuell.astype(bool)
_hand = np.zeros((480, 640), np.uint8)
cv2.fillPoly(_hand, [np.array([(296, 402), (306, 405), (313, 414), (318, 422), (300, 424)], np.int32)], 1)
cv2.fillPoly(_hand, [np.array([(345, 320), (384, 320), (384, 327), (345, 327)], np.int32)], 1)        # Unterkante L1 liegt bei y=322
cv2.fillPoly(_hand, [np.array([(212, 330), (226, 330), (226, 345), (212, 351)], np.int32)], 1)        # Oberkante Kugel unter der Saeule
FIG1 = FIG1 & ~_hand.astype(bool)
_frei = np.zeros((480, 640), np.uint8)
for _n in ('L1', 'L2'):
    cv2.fillPoly(_frei, [np.array(next(q for q in FB['nicht_figur_bild1'] if q['id'] == _n)['polygon'], np.int32)], 1)
FIG1 = FIG1 & ~cv2.dilate((_frei.astype(bool) & (ZIEL[1] == 0)).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)


HAKEN_DREH = 35.0                       # Pose 2: Haken um Y gedreht, die rosa Seite schaut weg (frei gesetzt, Bild 2 zeigt dort kein Rosa)
HAKEN_J = np.array([1000.0, -2080.0, 330.0])


def yrot_grad(g):
    a = math.radians(g)
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def tm(*ids, r=3, zus=None):
    """Teilmaske Bild 1: Polygone des Formblatts, geweitet, innerhalb des Soll-Umrisses."""
    m = np.zeros((480, 640), bool)
    for i in ids:
        m |= poly(FB1[i]['polygon'])
    m = weit(m, r)
    if zus is not None:
        m |= poly(zus)
    return m & FIG1


T1P = [(279, 192), (262, 204), (252, 216), (245, 226), (245, 240), (245, 252), (243, 264), (241, 276), (240, 288), (240, 298), (262, 316), (286, 320), (293, 313), (299, 318), (345, 318), (362, 318),
       (381, 318), (388, 300), (388, 270), (381, 250), (378, 233), (375, 230), (371, 222), (366, 214), (349, 206), (337, 198), (326, 190), (322, 187), (300, 187)]
T2P = [(354, 161), (362, 165), (370, 171), (378, 175), (390, 181), (394, 187), (416, 199), (431, 211), (437, 232), (431, 250), (420, 260), (413, 262), (384, 266), (382, 284), (378, 300), (374, 314), (356, 317), (338, 322), (300, 340),
       (262, 352), (232, 350), (222, 310), (222, 285), (226, 264), (250, 264), (254, 250), (250, 232), (250, 216), (255, 200), (263, 184), (273, 170), (287, 160), (304, 156), (320, 155), (345, 160)]

HAKEN2 = [(205, 224), (214, 218), (243, 216), (248, 228), (246, 240), (238, 252), (226, 263), (214, 262), (206, 248)]


def weich(p, s=2.0):
    return sdf(cv2.GaussianBlur(poly(p).astype(np.float32), (0, 0), s) > 0.5)


def tri(xy1, xy2):
    """Schnitt der Sehstrahlen aus beiden Bildern im Aktorraum, gilt nur fuer Teile, die in beiden Posen gleich stehen."""
    a1, r1 = FK.strahl(S1.k, np.array(xy1, float))
    a2, r2 = FK.strahl(S2.k, np.array(xy2, float))
    A1, D1 = S1.aktor(a1), r1 @ S1.R
    A2, D2 = S2.aktor(a2), r2 @ S2.R
    q = np.linalg.lstsq(np.array([D1, -D2]).T, A2 - A1, rcond=None)[0]
    return 0.5 * (A1 + D1 * q[0] + A2 + D2 * q[1])


# Eine Marke, zwei Bilder: A36/B01 Rest 53, A35/B02 Rest 137 (beide Rotbraun, FORMBLATT 2.1). Die Huelle allein laesst die Tiefe dort offen.
PUNKTE = ((337, 202, 286, 182), (336, 226, 275, 207))


def punktziele(V, s=430.0, runden=4):
    """Flaeche weich durch die triangulierten Punkte ziehen."""
    V = V.copy()
    Pz = [tri(p[:2], p[2:]) for p in PUNKTE]
    for _ in range(runden):
        for P in Pz:
            i = int(np.argmin(np.linalg.norm(V - P, axis=1)))
            dv = P - V[i]
            w = np.exp(-(np.linalg.norm(V - V[i], axis=1) / s) ** 2)
            V += w[:, None] * dv
    return V


def rumpf():
    """Rumpf: Oval-Netz an beide Umrisse gezogen. Jeder Ring bleibt nahe am Oval (Fourier bis Glied 2), so entstehen keine Huellen-Grate; zuletzt Laengsglaettung."""
    NL, NB = 32, 16
    c0 = np.array([100.0, -1900.0, 60.0])
    V, F, idx = kugelnetz(c0, (1000, 1000, 1000), NL, NB, phi0=math.pi / NL)
    d = V - c0
    d /= np.linalg.norm(d, axis=1, keepdims=True)
    r1 = strahl_ellipsoid(c0, d, (-120, -1980, -40), (1050, 990, 1020))
    r2 = strahl_ellipsoid(c0, d, (640, -1560, 280), (560, 540, 520))
    r = laplace(np.maximum(r1, r2)[:, None], nachbarn(F, len(V)), 0.5, 4)[:, 0]
    V = c0 + d * r[:, None]
    ringe = [[idx[j, i] for i in range(NL)] for j in range(1, NB)]
    z1, z2 = weich(T1P), weich(T2P)
    Z = [(S1, z1, 1.0, None), (S2, z2, 1.0, None)]
    V = silhouette_anpassen(V, F, Z, iter_n=200, rate=0.5, glatt=3, glatt_formerhalt=1.0, ringe=ringe, k_max=1)
    for _ in range(8):
        V = silhouette_anpassen(V, F, Z, iter_n=14, rate=0.45, glatt=3, glatt_formerhalt=0.4, rand=0.42)
        V = ringe_glaetten(V, ringe, 1, 0.55)
    V = laengs_glaetten(V, ringe, 0.3, 2)
    V = punktziele(V)
    return dict(name='rumpf', V=V, F=F, ringe=ringe, pole=(0, len(V) - 1), wrap=True)


def teil(name, st, n_ring, ziele=None, iter_n=120, phi0=0.0, kappe=(0.6, 0.6), glatt=2):
    V, F, ringe, pole = schlauch(st, n_ring, phi0=phi0, kappe=kappe)
    if ziele:
        z = []
        for q in ziele:
            s, m, g = q[:3]
            z.append((s, sdf(m), g, None) + tuple(q[3:]))
        V = silhouette_anpassen(V, F, z, iter_n=iter_n, rate=0.4, glatt=glatt, glatt_formerhalt=0.6)
    return dict(name=name, V=V, F=F, ringe=ringe, pole=pole, wrap=True)


def p3(x, y, t):
    return S1.auf_tiefe(np.array([x, y], float), t)


def st3(pts, ra, rb=None, e1=(0, 0, 1), ex=2.0):
    """Stationen direkt im Aktorraum."""
    rb = rb if rb is not None else ra
    return [dict(c=np.array(p, float), a=ra[i], b=rb[i], e1=e1, ex=ex) for i, p in enumerate(pts)]


def spiegeln(t, name, zm, dx, mass=1.0):
    """Teil an der Ebene Z = zm spiegeln und um dx in X schieben, Massstab um die Mitte."""
    V = t['V'].copy()
    m = V.mean(0)
    V = m + (V - m) * mass
    V[:, 2] = 2 * zm - V[:, 2]
    V[:, 0] += dx
    return dict(name=name, V=V, F=t['F'], ringe=t['ringe'], pole=t['pole'], wrap=True)


def alle_teile():
    T = []
    T.append(rumpf())
    # ---- Bein FR (fern rechts in Bild 1): Huefte S laengs, Pfeiler A1, Fuss A2
    m_s = poly([(366, 250), (392, 240), (405, 238), (418, 246), (426, 258), (429, 272), (426, 286), (420, 298), (417, 308), (382, 308), (372, 280)]) & (FIG1 | weit(FIG1, 1))
    sta = stationen_bild([(402, 234), (403, 240), (404, 250), (403, 264), (402, 280), (401, 296), (400, 308)], [6720, 6720, 6740, 6760, 6780, 6800, 6820], maske=m_s, k=0.8, weite_max=60)
    fr_h = teil('fr_huefte', sta, 12, [(S1, m_s, 1.0)], iter_n=40, kappe=(0.25, 0.4), glatt=3)
    T.append(fr_h)
    m_a = tm('A1', r=2, zus=[(382, 296), (416, 296), (418, 330), (416, 360), (382, 360)])
    sta = stationen_bild([(400, 288), (399, 302), (398, 316), (398, 330), (399, 345), (399, 360)], [6780, 6810, 6840, 6880, 6920, 6960], maske=m_a, k=1.0)
    fr_b = teil('fr_bein', sta, 8, [(S1, m_a, 1.0)])
    T.append(fr_b)
    m_f = tm('A2', r=1, zus=[(382, 350), (416, 350), (418, 360), (382, 360)])
    sta = stationen_bild([(400, 350), (400, 357), (401, 362), (401, 368), (401, 373), (401, 375)], [6980, 6990, 7000, 7000, 7000, 7000], maske=m_f, k=1.15, weite_max=40)
    fr_f = teil('fr_fuss', sta, 8, [(S1, m_f, 1.0)], kappe=(0.2, 0.05))
    T.append(fr_f)
    # ---- Bein FL: gespiegelt von FR, Lage aus K6, in Bild 1 hinter C1
    zm = 60.0
    T.append(spiegeln(fr_h, 'fl_huefte', zm, 420.0))
    T.append(spiegeln(fr_b, 'fl_bein', zm, 420.0, 0.9))
    T.append(spiegeln(fr_f, 'fl_fuss', zm, 420.0, 0.9))
    # ---- Bein NR (Mitte): Oberglied C1 bis Taille, Unterglied C3, Hand C4
    m_c1 = tm('C1', r=2, zus=[(303, 300), (347, 300), (347, 330), (344, 352), (308, 350), (300, 322)])
    m_c1 = m_c1 | (poly([(309, 342), (345, 342), (345, 349), (341, 356), (326, 358), (321, 353), (316, 350), (309, 348)]) & FIG1)
    sta = stationen_bild([(326, 296), (325, 308), (324, 320), (325, 332), (327, 344), (331, 352), (334, 357)], [5880, 5800, 5700, 5560, 5420, 5350, 5320], maske=m_c1, k=[2.0, 2.0, 1.9, 1.7, 1.5, 1.3, 1.1])
    T.append(teil('nr_ober', sta, 10, [(S1, m_c1, 1.0)], kappe=(0.6, 0.25)))
    m_c3 = tm('C3', r=3) | (poly([(286, 370), (325, 346), (346, 352), (344, 400), (300, 400), (286, 392)]) & weit(FIG1, 2))
    sta = stationen_bild([(330, 344), (326, 354), (319, 366), (316, 380), (319, 392), (322, 404)], [5340, 5300, 5280, 5260, 5240, 5220], maske=m_c3, k=0.62)
    T.append(teil('nr_unter', sta, 10, [(S1, m_c3, 1.0)]))
    m_c4 = tm('C4', r=2)
    T.append(teil('nr_hand', st3([(500, -190, 140), (560, -215, 240), (660, -250, 410), (860, -280, 520), (1060, -290, 640), (1240, -250, 750), (1360, -190, 820)],
                                 [150, 230, 310, 380, 390, 310, 140], [150, 200, 240, 280, 270, 200, 100], e1=(-0.5, 0, 0.86)), 10, [(S1, m_c4, 1.0)], iter_n=60))
    # ---- Bein NL (links): Saeule B1, Innenglied B4, Kugel B2 + B3
    m_b1 = tm('B1', r=2, zus=[(236, 296), (290, 296), (288, 346), (226, 346), (226, 318)])
    m_b1 = m_b1 | poly([(226, 340), (285, 340), (284, 362), (232, 362)])
    sta = stationen_bild([(258, 290), (257, 302), (256, 316), (256, 330), (256, 344), (257, 356), (258, 364)], [5040, 5020, 5000, 5000, 5000, 5000, 5000], maske=m_b1, k=1.2, weite_max=50)
    T.append(teil('nl_ober', sta, 10, [(S1, m_b1, 1.0)], kappe=(0.6, 0.35)))
    m_b4 = tm('B4', r=2)
    sta = stationen_bild([(268, 342), (268, 354), (267, 368), (267, 382), (267, 396), (266, 410), (266, 419)], [5000, 5000, 5020, 5040, 5050, 5060, 5060], maske=m_b4, k=1.4, weite_max=30)
    T.append(teil('nl_unter', sta, 8, [(S1, m_b4, 1.0)]))
    m_b2 = tm('B2', r=2)
    m_b2 = m_b2 | (poly([(232, 366), (254, 366), (254, 392), (232, 392)]) & FIG1)
    sta = stationen_bild([(231, 346), (229, 353), (228, 364), (228, 376), (228, 388)], [4760, 4740, 4720, 4700, 4700], maske=m_b2, k=0.95, weite_max=40)
    T.append(teil('nl_kugel_o', sta, 10, [(S1, m_b2, 1.0)]))
    m_b3 = tm('B3', r=2)
    sta = stationen_bild([(226, 376), (226, 388), (226, 400), (228, 412), (232, 424)], [4700, 4690, 4680, 4660, 4640], maske=m_b3, k=0.95, weite_max=40)
    T.append(teil('nl_kugel_u', sta, 10, [(S1, m_b3, 1.0)]))
    # ---- Vorsprung: rosa Spindel Z1 in Bild 1 = Haken H in Bild 2 (Triangulation, DOSSIER)
    a = np.array([1098.0, -1984.0, 278.0]); b = np.array([850.0, -2409.0, 541.0])
    d = (b - a) / np.linalg.norm(b - a)
    pkte = [a + d * -10 + (30, 0, 0), a + d * 60, a + d * 170, a + d * 300, a + d * 430, a + d * 530]
    m_h1 = poly(next(m for m in json.load(open(os.path.join(FORM, 'lesung_zeichnung.json'), encoding='utf-8'))['merkmale'] if m['id'] == 'A01')['polygon'])
    m_h1 = weit(m_h1, 3)
    S2h = DrehStand(S2, yrot_grad(HAKEN_DREH), HAKEN_J)
    T.append(teil('vorsprung', st3(pkte, [120, 190, 225, 225, 180, 80], [90, 140, 165, 165, 130, 60], e1=(1, 0, 0)), 10, [(S1, m_h1, 0.6), (S2h, poly(HAKEN2), 1.0)], iter_n=80))
    # Hals: Band y 258-272 in Bild 2 verbindet den Haken mit der Masse (FORMBLATT 3.2), Z2 liegt darauf
    T.append(teil('hals', st3([(800, -1640, 230), (860, -1730, 250), (940, -1840, 270), (1010, -1950, 285), (1060, -2040, 295), (1085, -2100, 300)],
                              [120, 300, 300, 260, 210, 110], [120, 280, 270, 220, 170, 90], e1=(0, 0, 1)), 10, None))
    # Kiefer: traegt Z3 und Z4. In Bild 2 fehlen sie (FORMBLATT 2.3), die Flaeche ist dort also weggedreht. Eigenes Glied, frei gesetzt.
    pt = S1.bild(T[0]['V'])
    nah = np.linalg.norm(pt - np.array([325.0, 295.0]), axis=1) < 26
    d0 = float(S1.tiefe(T[0]['V'][nah]).min())
    sta = stationen_bild([(327, 272), (326, 282), (325, 293), (323, 303), (321, 310), (320, 315)], [d0 - 40, d0 - 110, d0 - 140, d0 - 140, d0 - 100, d0 - 60],
                         breiten=[11, 19, 23, 21, 14, 7], k=0.62)
    T.append(teil('kiefer', sta, 8, None, kappe=(0.5, 0.5)))
    # Zehen: Spitzen des Fusses A2 links und rechts, Formblatt 3.1
    fuss = next(t for t in T if t['name'] == 'fr_fuss')
    a = p3(417, 362, 7000); b = p3(428, 363, 7000)
    T.append(teil('fr_zeh', st3([a - (b - a) * 0.5, a, a + (b - a) * 0.5, b], [70, 95, 60, 18], [70, 90, 55, 18], e1=(0, 1, 0)), 6, None, kappe=(0.4, 0.3)))
    ziel1 = FIG1
    NICHT = ('vorsprung', 'hals', 'kiefer', 'fl_huefte', 'fl_bein', 'fl_fuss', 'fr_zeh', 'rumpf')
    union_anpassen(T, S1, ziel1, iter_n=40, glatt=2, nicht=NICHT)
    z2p = poly(T2P)
    for _ in range(3):
        for t in T:
            if t['name'] not in NICHT:                 # Glieder sauber halten: Ringe nahe am Oval, Laengslinien glatt
                t['V'] = ringe_glaetten(t['V'], t['ringe'], 2, 0.35)
                t['V'] = laengs_glaetten(t['V'], t['ringe'], 0.25, 1)
        union_anpassen(T, S1, ziel1, iter_n=8, glatt=2, nicht=NICHT)
    hart_einziehen(T, S1, ziel1, nicht=NICHT)
    fzeh = next(t for t in T if t['name'] == 'fr_zeh')
    T.append(spiegeln(fzeh, 'fl_zeh', 60.0, 420.0, 0.9))
    # FL steht im Bild hinter C1: gleicher Fehler wie FR ist egal, aber die Spiegelteile muessen die angepassten Formen tragen
    d = {t['name']: t for t in T}
    for a_, b_, m_ in (('fr_huefte', 'fl_huefte', 1.0), ('fr_bein', 'fl_bein', 0.9), ('fr_fuss', 'fl_fuss', 0.9)):
        n = spiegeln(d[a_], b_, 60.0, 420.0, m_)
        d[b_]['V'] = n['V']
    # FL steht in Bild 1 hinter C1: darf nirgends vorstehen
    fl = [t for t in T if t['name'].startswith('fl_')]
    hart_einziehen(fl, S1, cv2.erode(ziel1.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool), rand=1.0)
    # Taille: Pixel, die auf dem freien Boden L1, L2 landen, schieben die nahen Ecken weg
    kern = np.zeros((480, 640), bool)
    for n_ in ('L1', 'L2', 'SCH'):
        kern |= poly(next(q for q in FB['nicht_figur_bild1'] if q['id'] == n_)['polygon']) & (ZIEL[1] == 0)
    print('Taille Runden', abstossen(T, S1, kern))
    for t in T:
        t['V'][:, 1] = np.minimum(t['V'][:, 1], 0.0)
    return T


def union_anpassen(T, s, ziel, iter_n=40, rate=0.45, glatt=2, nicht=()):
    """Alle Teile gemeinsam: nur Ecken am Gesamtumriss wandern zur Zielkante."""
    d = sdf(ziel)
    f = 0.5 * (s.k['brennweite_bild_px']['fx'] + s.k['brennweite_bild_px']['fy'])
    auge = s.auge_aktor()
    for t in T:
        t['_T'] = flaechen_dreiecke(t['F'])
        t['_NB'] = nachbarn(t['F'], len(t['V']))
    for it in range(iter_n):
        U = np.zeros((480, 640), np.uint8)
        for t in T:
            raster(t['V'], t['F'], s, U)
        dc = cv2.distanceTransform(U, cv2.DIST_L2, 3)
        for t in T:
            if t['name'] in nicht:
                continue
            V = t['V']
            N = eckennormalen(V, t['_T'])
            p = s.bild(V)
            r = V - auge
            tief = np.linalg.norm(r, axis=1)
            r /= tief[:, None]
            c = np.abs((N * r).sum(1))
            dd = lese_bl(d, p)
            am = lese_bl(dc, p) < 3.0
            w = np.where(dd > 0, np.maximum(np.exp(-(c / 0.4) ** 2), 0.4), np.exp(-(c / 0.4) ** 2) * am)
            D = (w * (-dd / (f / tief)))[:, None] * N
            D = laplace(D, t['_NB'], 0.6, glatt)
            lg = np.linalg.norm(D, axis=1, keepdims=True)
            t['V'] = V + rate * D * np.minimum(1.0, 50.0 / (lg + 1e-6))
    return T


def hart_einziehen(T, s, ziel, rand=0.8, runden=3, nicht=()):
    """Letzter Schritt: Ecken ausserhalb des Ziels ohne Glaettung auf die Zielseite der Kante schieben, quer zum Blick."""
    d = sdf(ziel).astype(np.float32)
    gx = cv2.Sobel(d, cv2.CV_32F, 1, 0, ksize=3) / 8.0
    gy = cv2.Sobel(d, cv2.CV_32F, 0, 1, ksize=3) / 8.0
    Rk = s.k['R']
    e_r = Rk[0] @ s.R
    e_d = Rk[1] @ s.R
    f = 0.5 * (s.k['brennweite_bild_px']['fx'] + s.k['brennweite_bild_px']['fy'])
    auge = s.auge_aktor()
    for _ in range(runden):
        for t in T:
            if t['name'] in nicht:
                continue
            V = t['V']
            p = s.bild(V)
            dd = lese_bl(d, p)
            m = dd > -rand
            if not m.any():
                continue
            tief = np.linalg.norm(V - auge, axis=1)
            sch = (dd + rand) * tief / f
            g = np.stack([lese_bl(gx, p), lese_bl(gy, p)], 1)
            g /= np.linalg.norm(g, axis=1, keepdims=True) + 1e-6
            D = g[:, 0:1] * e_r + g[:, 1:2] * e_d
            V[m] = V[m] - D[m] * sch[m, None]
    return T


def abstossen(T, s, verbot, runden=40, px=1.2, radius=13.0):
    """Solange Render auf verbotene Pixel faellt: Ecken nahe dem Treffer im Bild davon weg schieben. Wirkt nur auf dem Treffer, die Glaettung bleibt."""
    Rk = s.k['R']
    e_r, e_d = Rk[0] @ s.R, Rk[1] @ s.R
    f = 0.5 * (s.k['brennweite_bild_px']['fx'] + s.k['brennweite_bild_px']['fy'])
    auge = s.auge_aktor()
    for r in range(runden):
        U = np.zeros((480, 640), np.uint8)
        for t in T:
            raster(t['V'], t['F'], s, U)
        B = U.astype(bool) & verbot
        if not B.any():
            return r
        k, lab = cv2.connectedComponents(B.astype(np.uint8))
        for j in range(1, k):
            ys, xs = np.nonzero(lab == j)
            c = np.array([xs.mean() + 0.5, ys.mean() + 0.5])
            d_ = cv2.distanceTransform((~verbot).astype(np.uint8), cv2.DIST_L2, 3)
            for t in T:
                if t['name'] == 'rumpf':
                    continue
                p = s.bild(t['V'])
                d = np.linalg.norm(p - c, axis=1)
                m = d < radius
                if not m.any():
                    continue
                gx = cv2.Sobel(d_.astype(np.float32), cv2.CV_32F, 1, 0, ksize=5)
                gy = cv2.Sobel(d_.astype(np.float32), cv2.CV_32F, 0, 1, ksize=5)
                g = np.array([gx[int(c[1]), int(c[0])], gy[int(c[1]), int(c[0])]])
                if np.linalg.norm(g) < 1e-3:
                    g = np.mean(p[m] - c, axis=0)
                g = g / (np.linalg.norm(g) + 1e-6)
                tief = np.linalg.norm(t['V'][m] - auge, axis=1, keepdims=True)
                w = (1 - d[m] / radius)[:, None]
                t['V'][m] += (g[0] * e_r + g[1] * e_d) * px * w * tief / f
    return runden


def nachsehen(T, tag=''):
    FARBEN = [(180, 120, 120), (120, 180, 120), (120, 120, 180), (180, 180, 100), (100, 180, 180), (180, 100, 180), (200, 160, 100), (100, 200, 160), (160, 100, 200), (200, 200, 200), (150, 150, 90), (90, 150, 150)]
    tt = [(t['V'], t['F'], FARBEN[i % len(FARBEN)]) for i, t in enumerate(T)]
    schau_blatt(tt, 'schau_teile%s.png' % tag, 360)
    for n, s, zi in ((1, S1, FIG1), (2, S2, ZIEL[2] == 255)):
        r = np.zeros((480, 640), np.uint8)
        for t in T:
            raster(t['V'], t['F'], s, r)
        r = r.astype(bool)
        o = BILD[n].copy()
        c, _ = cv2.findContours(zi.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(o, c, -1, (0, 255, 0), 1)
        for t in T:
            r2 = raster(t['V'], t['F'], s)
            c, _ = cv2.findContours(r2, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
            cv2.drawContours(o, c, -1, (255, 80, 80), 1)
        c, _ = cv2.findContours(r.astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(o, c, -1, (255, 255, 0), 1)
        cv2.imwrite('teile%s_%d.png' % (tag, n), cv2.resize(o[140:470, 170:480], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)[:, :, ::-1])
        print(n, 'Ueberdeckung', round((r & zi).sum() / (r | zi).sum(), 3), 'weiss gedeckt', round((r & zi).sum() / zi.sum(), 3))
    print('Flaechen', sum(len(t['F']) for t in T), {t['name']: len(t['F']) for t in T})


if __name__ == '__main__':
    import time
    t0 = time.time()
    T = alle_teile()
    print('zeit', time.time() - t0)
    nachsehen(T)
