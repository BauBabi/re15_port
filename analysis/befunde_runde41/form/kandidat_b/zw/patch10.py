s = open('b_mal2.py', encoding='utf-8').read()
a = s.index('def grundhaut(pos, nrm, bone, belegt, fit, J):')
b = s.index('def lichtfaktor(n, s, pos, nrm, bone')
neu = '''def grundhaut(pos, nrm, bone, belegt, fit, J):
    """Dunkle Haut in harten Stufen: Muskelstraenge laengs der Glieder, Falten quer an den Gelenken, Knoten in Inseln, Adern. Keine Ringelung (FORMBLATT 2.3)."""
    m = belegt
    p = pos[m].astype(np.float64)
    n = nrm[m].astype(np.float64)
    b = bone[m]
    N_ = len(p)
    ach = glied_koordinaten(fit, J)
    # Anisotrope Lage: q streckt die Muster laengs der Gliedachse, r quer dazu; Kuppel: Achse senkrecht
    q = p.copy()
    rq = p.copy()
    gelenk_nah = np.zeros(N_)
    for bi in np.unique(b):
        s_ = b == bi
        nm = T.KNOCHEN[bi][0]
        if bi in ach and not nm.startswith(('rumpf', 'maul')):
            j0, ax, lg = ach[bi]
            d = p[s_] - j0
            t = d @ ax
            w = d - t[:, None] * ax
            q[s_] = w + t[:, None] * ax * 0.24 + bi * 700.0
            rq[s_] = w * 0.24 + t[:, None] * ax + bi * 700.0
            gelenk_nah[s_] = np.maximum(1 - np.minimum(np.abs(t), np.abs(t - lg)) / 300.0, 0)
        else:
            q[s_] = p[s_] * np.array([1.0, 0.27, 1.0])
            rq[s_] = p[s_] * np.array([0.3, 1.0, 0.3])
            gelenk_nah[s_] = np.maximum(1 - np.abs(n[s_, 1] - 0.75) / 0.25, 0) * 0.6      # Bauchfalten
    warp = np.stack([M.rauschen3(p, 260.0, 11), M.rauschen3(p, 260.0, 12), M.rauschen3(p, 260.0, 13)], 1) * 42.0
    f = M.rauschen3(q + warp, 135.0, 21) + 0.45 * M.rauschen3(q + warp, 62.0, 22)
    # Grundton: grosse Flecken
    g = M.rauschen3(p, 430.0, 14) + 0.5 * M.rauschen3(p, 170.0, 15)
    stufe = np.full(N_, 2.0) + (g > 0.22) * 1.0 - (g < -0.3) * 1.0
    # Furche, darueber Grat (einseitig, das liest sich als Wulst)
    furche = np.abs(f) < 0.055
    grat = (f > 0.055) & (f < 0.135)
    stufe = np.where(grat, stufe + 1, stufe)
    stufe = np.where(furche, np.minimum(stufe, 1), stufe)
    stufe = np.where(np.abs(f) < 0.022, 0, stufe)
    # Querfalten nah am Gelenk, unregelmaessig, abgebrochen
    h = M.rauschen3(rq + warp, 95.0, 23)
    quer = (np.abs(h) < 0.05) & (M.rauschen3(p, 210.0, 24) + gelenk_nah * 1.3 > 0.72)
    stufe = np.where(quer, 0, stufe)
    stufe = np.where((h > 0.05) & (h < 0.11) & (M.rauschen3(p, 210.0, 24) + gelenk_nah * 1.3 > 0.72), np.maximum(stufe, 3), stufe)
    # Knoten
    ins = M.rauschen3(p, 560.0, 16) + 0.3 * M.rauschen3(p, 190.0, 17)
    kn = ins > 0.36
    h1, h2, hid, hv = worley(p + warp * 0.6, 92.0, 5)
    oben = np.array([0.0, -1.0, 0.0])
    kern = h1 < 0.34
    stufe = np.where(kn & (h1 >= 0.34) & (h1 < 0.46), 0, stufe)
    stufe = np.where(kn & kern, 3, stufe)
    stufe = np.where(kn & kern & ((-hv @ oben) / 92.0 > 0.07), 5, stufe)
    # Adern, duenn, einseitig dunkel begleitet
    a_ = np.abs(M.rauschen3(p + warp, 380.0, 18) + 0.3 * M.rauschen3(p, 130.0, 19))
    masse = M.rauschen3(p, 900.0, 25) > -0.15
    ader = (a_ < 0.020) & masse
    stufe = np.where(ader, 4, stufe)
    stufe = np.where((a_ >= 0.020) & (a_ < 0.034) & masse, np.minimum(stufe, 1), stufe)
    # Unterseite nie ueber Stufe 3
    stufe = np.where(n[:, 1] > 0.55, np.minimum(stufe, 3), stufe)
    stufe = np.clip(stufe, 0, 5).astype(int)
    aus = np.zeros(belegt.shape + (3,), np.float32)
    aus[m] = S[stufe]
    return aus


'''
s = s[:a] + neu + s[b:]
s = s.replace("WEISS = np.array([[176, 190, 198], [206, 216, 222]], float)", "WEISS = np.array([[150, 166, 176], [176, 190, 198]], float)")
# Striche nur aus erlaubten Zonen
a = s.index('def innen_maske(erode=4):')
neu = '''ZUSATZ = {1: dict(A05='linie', A06='linie', A10='linie', A11='band0', A13='linie', A21='linie', A25='linie0', A33='linie0', A39='linie0', A07='linie', A08='linie', A09='linie'),
          2: dict(B08='band', B09='band', B14='linie', B03='rot', B11='linie0', B10='linie0', B12='linie0')}
ZUG_Q = [(300, 192), (314, 189), (345, 189), (353, 194), (372, 201), (384, 218), (388, 236), (383, 260), (374, 278), (363, 290), (365, 302)]


def zusatz(rgb, sichten):
    """Weitere gemessene Stellen aus lesung_zeichnung.json, die auf der Figur liegen, ohne Pruefrolle. Zug Q als durchgehende Linie (FORMBLATT 2.3)."""
    Mk = P.merkmale()
    TON = dict(linie=LINIE[1], linie0=LINIE[0], band=BAND[1], band0=BAND[0], rot=ROT[1])
    for n, d in ZUSATZ.items():
        v = sichten[n]
        xy, si = v['xy'], v['sicht']
        ok = si & np.isfinite(xy[..., 0])
        x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
        y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
        for i, t in d.items():
            k = np.zeros((480, 640), np.uint8)
            cv2.fillPoly(k, [np.round(np.array(Mk[i]['polygon'], float)).astype(np.int32)], 1)
            rgb[ok & (k[y, x] > 0)] = TON[t]
    v = sichten[2]
    xy, si = v['xy'], v['sicht']
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    k = np.zeros((480, 640), np.uint8)
    cv2.polylines(k, [np.array(ZUG_Q, np.int32)], False, 1, 4)
    rgb[ok & (k[y, x] > 0)] = LINIE[0]
    return rgb


def spiegel_fl(rgb, belegt, pos, bone, J, sicht2):
    """Bein FL traegt die Zeichnung von Bein FR gespiegelt (frei gesetzt, Seite L ist nicht sichtbar). Was Bild 2 sieht, bleibt Grundhaut."""
    from scipy.spatial import cKDTree
    paare = [('fr_ober', 'fl_ober'), ('fr_unter', 'fl_unter'), ('fr_fuss', 'fl_fuss'), ('fr_zeh', 'fl_zeh')]
    sz = sicht2['sicht']
    for a, b in paare:
        ia, ib = T.IDX[a], T.IDX[b]
        ma, mb = bone == ia, bone == ib
        if not ma.any() or not mb.any():
            continue
        pa = pos[ma] - J[ia]
        pb = (pos[mb] - J[ib]) * np.array([1, 1, -1.0])
        _, n = cKDTree(pa).query(pb)
        q = rgb[ma][n]
        z = rgb[mb].copy()
        hell = (q @ np.array([.299, .587, .114])) > 70
        hell &= ~sz[mb]
        z[hell] = q[hell]
        rgb[mb] = z
    return rgb


def flicken(rgb, belegt):
    """Einzelne dunkle Texel in hellen Flaechen schliessen."""
    Y_ = rgb @ np.array([.299, .587, .114], np.float32)
    h = (Y_ > 85).astype(np.uint8)
    n = cv2.filter2D(h, -1, np.ones((3, 3), np.uint8), borderType=cv2.BORDER_CONSTANT)
    loch = belegt & (h == 0) & (n >= 6)
    ys, xs = np.nonzero(loch)
    for y, x in zip(ys, xs):
        f = rgb[max(y - 1, 0):y + 2, max(x - 1, 0):x + 2].reshape(-1, 3)
        q = f[(f @ np.array([.299, .587, .114], np.float32)) > 85]
        rgb[y, x] = q[len(q) // 2]
    return rgb


def innen_maske(erode=5):
    z = cv2.imread(FORM + '/pruefung/figur_bild1.png', 0) > 0
    m = cv2.erode(z.astype(np.uint8), cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * erode + 1, 2 * erode + 1))).astype(bool)
    m[395:, :] = False          # unten offen: Boden und Schatten nicht als Zeichnung lesen (A28, A34, C5)
    return m


def malen(pos, nrm, bone, belegt, sichten, netze, ctx):
    fit, J, s1 = ctx['fit'], ctx['J'], ctx['s1']
    rgb = grundhaut(pos, nrm, bone, belegt, fit, J)
    F1 = lichtfaktor(1, s1, pos, nrm, bone)
    rgb = strich_aus_bild(rgb, sichten[1], F1, innen_maske(), (125.0, 175.0, 245.0))
    rgb = stellen(rgb, {1: sichten[1], 2: dict(xy=sichten[2]['xy'], sicht=np.zeros_like(sichten[2]['sicht']))}, F1)
    rgb = spiegel_fl(rgb, belegt, pos, bone, J, sichten[2])
    rgb = stellen(rgb, {1: dict(xy=sichten[1]['xy'], sicht=np.zeros_like(sichten[1]['sicht'])), 2: sichten[2]}, F1)
    rgb = zusatz(rgb, sichten)
    rgb = stellen(rgb, {1: sichten[1], 2: sichten[2]}, F1)
    rgb = flicken(rgb, belegt)
    return rgb
'''
s = s[:a] + neu
open('b_mal2.py', 'w', encoding='utf-8').write(s)
