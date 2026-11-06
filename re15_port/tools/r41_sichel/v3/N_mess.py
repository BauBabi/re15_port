# SF Messblatt V3, Abschnitt 10 (Nachtrag Nutzer-Befund 2026-11-06 20:35-20:50) - eigene Messung des Pruefers.
# Gleiche Funktionen fuer KI (Freisteller im 1280er Videobild, b2.ki) und Port (N_render.py: Spielkamera 50A0/12,
# 640er x2 NEAREST = 1280er Massstab). H = Oberkante Buckel bis Sohle im selben Bild (KI h118 569 px, h125 599 px;
# Port 2 x (Sohle - OK) ohne Schwanz). (B) = Betrachter, (W) = Wesen; von vorn B-links = W-rechts (Sichelseite).
#
# Auge: Startpunkt KI = Messblatt K1 (Freisteller + b2.KI_MAP-Versatz), Port = dunkelster Punkt (Gauss 1) in den oberen
#   45 % von Mesh 9. Haut/Kuppe Lh = Median L* im Ring 0,012-0,02 H um den Startpunkt. Augenmaske = 8er-Komponente
#   L* < (Lmin + Lh)/2 (Gauss 1) um den dunkelsten Punkt im Fenster +-0,015 H, Loecher gefuellt; Mitte = Schwerpunkt.
#   Kern = Bildpunkte der gefuellten Augenmaske mit L* > Lh (heller als die Kuppe); Fenster-Hell = L* > 55 im Fenster
#   +-0,012 H (Sicht-Pruefer r2 A2, dort im x3-Feld).
# Kopf auf Augenhoehe (Band +-0,01 H, Spaltenmittel L*, Gauss 1,5): KI links = Minimum in [-0,06 H, -0,01 H] (Rille zur
#   Schulter), rechts = erster Punkt ab +0,02 H mit L* >= 0,5 x Lh (Ende der dunklen Straehnenzone, Beginn Schulter);
#   Port = Ausdehnung von Mesh 9 im selben Band. lage_kopf = (Auge - links) / (rechts - links).
# Haare (Zone Z): x von Auge + 0,012 H bis Kopf rechts, y von Auge - 0,05 H bis Auge + 0,025 H, nur Figur (KI Alpha,
#   Port Mesh 9). Je Zeile L* (Gauss 0,8 in x), lokale Minima mit Prominenz >= 5 L* = Straehnenpunkte; Straehne = 8er-
#   Komponente der Straehnenpunkte (1 px waagerecht verbreitert) mit senkrechter Ausdehnung >= 0,015 H.
# Maul: Fenster x Auge - 0,045 H .. + 0,06 H, y Auge + 0,02 H .. + 0,09 H.
#   schwarz = L* < 0,25 x Lh (Leiste 11, n2_maul_spiel) ohne senkrechte Laeufe >= 0,045 H (Rillen; Luecken <= 4 px ueberbrueckt); Flaeche in 1e-4 H^2, Spaltenhoehe = Median der schwarzen Bildpunkte
#   je Spalte (Spalten mit >= 1). Groesste schwarze Komponente: Ellipsen-IoU (Ellipse gleicher Flaeche und gleicher
#   zweiter Momente), Solidity (Flaeche / konvexe Huelle). Zaehne = Top-Hat (L* - Oeffnung 5x5) > 12 und L* > Lh, 1-12 px,
#   mit L* < 0,5 Lh im 3-px-Umkreis; Zahnreihen = oben/unten der Zahn-Mittellinie (Median y);
#   Neigung der oberen Reihe (Gerade, Grad, + = (B)-rechts tiefer), Rest-Std in H, Abstands-Variationskoeffizient.
#   Schwarz-Masse nur in den Maulspalten (KI: Messblatt Abschnitt 6; Port: groesste schwarze Komponente +- 2 px).
#   Glanz = max(RGB) > 170 im Fenster ohne Zaehne; glanz_maul_px = in den Maulspalten (mit Zaehnen).
#   Lippe = Maulspalten, nicht schwarz, kein Zahn, L* < 0,6 Lh, a* >= 10. Kinn = Zeilen Auge + 0,09 .. + 0,12 H, x Zahnmitte +-0,02 H.
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
from scipy.signal import find_peaks
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import b2
from P_farbe import lab, r1
N = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/N/'
KI_H = {118: 569, 125: 599}
KI_AUGE = {118: (185.9, 40.9), 125: (205.3, 41.0)}     # Messblatt V3 K1
KI_MAUL_X = {118: (165, 212), 125: (185, 239)}           # Messblatt V3 Abschnitt 6 (Breite, Zahnhuelle)


def lade(wer, h):
    if wer == 'KI':
        im, m = b2.ki(h, hinter=(0, 0, 0))
        n, ox, oy = b2.KI_MAP[b2.KI_VON_H[h]]
        return dict(im=im, fig=m, kopf=None, ax=KI_AUGE[h][0] + ox, ay=KI_AUGE[h][1] + oy, H=KI_H[h], ox=ox, oy=oy,
                    maul_x=(KI_MAUL_X[h][0] + ox, KI_MAUL_X[h][1] + ox))
    d = np.load(N + '%s_h%d.npz' % (wer, h))
    im, ids = d['im'], d['ids']; H = int(d['H'])
    L = nd.gaussian_filter(lab(im.astype(float))[..., 0], 1)
    k9 = ids == 9
    ys = np.nonzero(k9.any(1))[0]
    ob = k9.copy(); ob[int(ys.min() + 0.45 * (ys.max() - ys.min())):] = False
    ay, ax = np.unravel_index(np.argmin(np.where(ob, L, 999)), L.shape)
    return dict(im=im, fig=ids >= 0, kopf=k9, ax=float(ax), ay=float(ay), H=H, ox=0, oy=0)


def auge(d):
    im, H = d['im'], d['H']; A = lab(im.astype(float)); L = A[..., 0]; Lg = nd.gaussian_filter(L, 1)
    ax, ay = d['ax'], d['ay']
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay)
    Lh = float(np.median(L[(rr >= 0.012 * H) & (rr <= 0.02 * H)]))
    w = int(round(0.015 * H))
    y0, x0 = int(ay) - w, int(ax) - w
    sub = Lg[y0:y0 + 2 * w + 1, x0:x0 + 2 * w + 1]
    my, mx = np.unravel_index(np.argmin(sub), sub.shape)
    Lmin = float(sub[my, mx])
    dm = sub < (Lmin + Lh) / 2
    lb, _ = nd.label(dm, structure=np.ones((3, 3)))
    m0 = lb == lb[my, mx]
    hull = cv2.convexHull(np.argwhere(m0)[:, ::-1].astype(np.int32))
    m = np.zeros(m0.shape, np.uint8); cv2.fillConvexPoly(m, hull, 1); m = (m > 0) | m0
    ys, xs = np.nonzero(m)
    cx, cy = x0 + xs.mean(), y0 + ys.mean()
    Ls = L[y0:y0 + 2 * w + 1, x0:x0 + 2 * w + 1]
    kern = m & (Ls > Lh)
    w2 = int(round(0.012 * H))
    fen = L[int(cy) - w2:int(cy) + w2 + 1, int(cx) - w2:int(cx) + w2 + 1]
    return dict(Lh=round(Lh, 1), mitte=[round(cx, 1), round(cy, 1)], b_H=round((xs.ptp() + 1) / H, 4), h_H=round((ys.ptp() + 1) / H, 4),
                L_auge_haut=round(float(Ls[m].mean()) / Lh, 3), kern_px=int(kern.sum()),
                kern_Lmax=round(float(Ls[m].max()), 1), kern_Lmax_haut=round(float(Ls[m].max()) / Lh, 2),
                lab_kern=r1(A[y0:y0 + 2 * w + 1, x0:x0 + 2 * w + 1][kern].mean(0)) if kern.any() else None,
                fenster_L55=int((fen > 55).sum()), fenster_heller_1_4Lh=int((fen > 1.4 * Lh).sum()),
                fenster_px=int(fen.size), maske=(x0, y0, m))


def kopf(d, au):
    im, H = d['im'], d['H']; L = lab(im.astype(float))[..., 0]
    cx, cy = au['mitte']; Lh = au['Lh']
    b0, b1 = int(cy - 0.01 * H), int(cy + 0.01 * H) + 1
    prof = nd.gaussian_filter1d(L[b0:b1].mean(0), 1.5)
    if d['kopf'] is None:
        a, b = int(cx - 0.06 * H), int(cx - 0.01 * H)
        links = a + int(np.argmin(prof[a:b]))
        rechts, dunkel = None, False
        for x in range(int(cx + 0.02 * H), int(cx + 0.15 * H)):
            if prof[x] < 0.5 * Lh:
                dunkel = True
            elif dunkel:
                rechts = x; break
        art = 'Rille/Schulter (L* Profil)'
    else:
        xs = np.nonzero(d['kopf'][b0:b1].any(0))[0]
        links, rechts = int(xs.min()), int(xs.max())
        art = 'Mesh 9'
    return dict(art=art, links=links, rechts=rechts, breite_H=round((rechts - links) / H, 4),
                lage_kopf=round((cx - links) / (rechts - links), 3),
                auge_zu_kopfmitte_H=round((cx - (links + rechts) / 2) / H, 4), profil=prof)


def haare(d, au, ko):
    im, H = d['im'], d['H']; A = lab(im.astype(float)); L = A[..., 0]
    cx, cy = au['mitte']
    xa, xb = int(cx + 0.012 * H), int(ko['rechts'])
    ya, yb = int(cy - 0.05 * H), int(cy + 0.025 * H)
    fig = d['fig'] if d['kopf'] is None else d['kopf']
    Ls = nd.gaussian_filter1d(L[ya:yb, xa:xb], 0.8, axis=1)
    pk = np.zeros(Ls.shape, bool); breiten = []
    for i in range(Ls.shape[0]):
        p, pr = find_peaks(-Ls[i], prominence=5, width=0.5)
        for j, wd in zip(p, pr['widths']):
            if fig[ya + i, xa + j]:
                pk[i, j] = True; breiten.append(wd)
    pkd = nd.binary_dilation(pk, structure=np.array([[1, 1, 1]], bool))
    lb, n = nd.label(pkd, structure=np.ones((3, 3)))
    str_ = []
    strm = np.zeros_like(pk)
    for k in range(1, n + 1):
        ys, xs = np.nonzero((lb == k) & pk)
        if len(ys) == 0 or ys.ptp() + 1 < 0.015 * H:
            continue
        strm |= (lb == k) & pk
        sl = np.polyfit(ys, xs, 1)[0] if ys.ptp() > 0 else 0.0
        str_.append(dict(oben=[int(xa + xs[np.argmin(ys)]), int(ya + ys.min())], unten=[int(xa + xs[np.argmax(ys)]), int(ya + ys.max())],
                         laenge_H=round((ys.ptp() + 1) / H, 4), neigung_grad=round(float(np.degrees(np.arctan(sl))), 1),
                         lab=r1(A[ya:yb, xa:xb][(lb == k) & pk].mean(0))))
    zm = fig[ya:yb, xa:xb]
    x0, y0, m = au['maske']
    am = np.zeros(L.shape, bool); am[y0:y0 + m.shape[0], x0:x0 + m.shape[1]] = m
    dist = nd.distance_transform_edt(~am)
    sm = np.zeros(L.shape, bool); sm[ya:yb, xa:xb] = strm
    zwischen = zm & ~nd.binary_dilation(strm, iterations=1)
    return dict(zone=[xa, ya, xb, yb], zone_H=[round((xb - xa) / H, 4), round((yb - ya) / H, 4)],
                straehnen=len(str_), laengen_H=sorted([s['laenge_H'] for s in str_], reverse=True),
                neigungen=[s['neigung_grad'] for s in str_],
                breite_px_median=round(float(np.median(breiten)), 1) if breiten else 0,
                breite_H_median=round(float(np.median(breiten)) / H, 4) if breiten else 0,
                minima_je_zeile=round(float(pk.sum(1).mean()), 2),
                lab_straehne=r1(A[ya:yb, xa:xb][strm & zm].mean(0)) if (strm & zm).any() else None,
                lab_zwischen=r1(A[ya:yb, xa:xb][zwischen].mean(0)) if zwischen.any() else None,
                L_zone_kuppe=round(float(L[ya:yb, xa:xb][zm].mean()) / au['Lh'], 3),
                abstand_auge_px=round(float(dist[sm].min()), 1) if sm.any() else None,
                abstand_auge_H=round(float(dist[sm].min()) / H, 4) if sm.any() else None,
                ueber_auge=int((sm & nd.binary_dilation(am, iterations=1)).sum()),
                liste=str_, maske=(xa, ya, strm))


def ellipse_iou(m):
    ys, xs = np.nonzero(m)
    if len(ys) < 5:
        return None
    c = np.cov(np.vstack([xs, ys]).astype(float)) + np.eye(2) / 12.0
    mx, my = xs.mean(), ys.mean()
    ev, evec = np.linalg.eigh(c)
    a, b = 2 * np.sqrt(ev)
    yy, xx = np.mgrid[:m.shape[0], :m.shape[1]]
    p = np.stack([xx - mx, yy - my], -1) @ evec
    e = (p[..., 0] / a) ** 2 + (p[..., 1] / b) ** 2 <= 1
    return round(float((m & e).sum() / (m | e).sum()), 3)


def maul(d, au):
    im, H = d['im'], d['H']; A = lab(im.astype(float)); L = A[..., 0]
    cx, cy = au['mitte']; Lh = au['Lh']
    x0, x1 = int(cx - 0.045 * H), int(cx + 0.06 * H); y0, y1 = int(cy + 0.02 * H), int(cy + 0.09 * H)
    s = L[y0:y1, x0:x1]; rgb = im[y0:y1, x0:x1].astype(int)
    sw0 = s < 0.25 * Lh
    # Rillen/Schatten: senkrechte schwarze Laeufe >= 0,045 H je Spalte gehoeren nicht zum Maul (KI-Rillen Kopf/Schulter
    # und Kinn/Brust laufen ueber das ganze Fenster; die Mauloeffnung ist hoechstens 0,04 H hoch, Riegel N8 spalt_max)
    sw = sw0.copy()
    swv = nd.binary_closing(sw0, structure=np.ones((5, 1), bool))    # Luecken <= 4 px im senkrechten Lauf ueberbruecken
    for c in range(sw.shape[1]):
        lbc, nc = nd.label(swv[:, c])
        for k in range(1, nc + 1):
            if (lbc == k).sum() >= 0.045 * H:
                sw[lbc == k, c] = False
    th = s - nd.grey_opening(s, size=(5, 5))
    zm = (th > 12) & (s > Lh)
    nah = nd.binary_dilation(s < 0.5 * Lh, iterations=3)
    lbz, nz = nd.label(zm, structure=np.ones((3, 3)))
    z = []
    for k in range(1, nz + 1):
        ys, xs = np.nonzero(lbz == k)
        if 1 <= len(ys) <= 12 and nah[lbz == k].any():
            z.append((xs.mean(), ys.mean(), len(ys)))
    z = np.array(z) if z else np.zeros((0, 3))
    # Maulspalten: KI = Maulbreite Messblatt V3 Abschnitt 6 (Zahnhuelle, Freisteller x 165-212 h118 / 185-239 h125);
    # Port = Spannweite der groessten schwarzen Komponente im Fenster +- 2 px (am Port liegen keine Rillen am Maul).
    if d.get('maul_x') is not None:
        ca, cb = int(d['maul_x'][0] - x0), int(d['maul_x'][1] - x0) + 1
    else:
        lbq, nq = nd.label(sw, structure=np.ones((3, 3)))
        if nq:
            q_ = lbq == 1 + int(np.argmax(nd.sum(sw, lbq, range(1, nq + 1))))
            xq = np.nonzero(q_.any(0))[0]; ca, cb = max(0, xq.min() - 2), min(sw.shape[1], xq.max() + 3)
        else:
            ca, cb = 0, sw.shape[1]
    sw[:, :ca] = False; sw[:, cb:] = False
    spalten = (ca, cb)
    lb, n = nd.label(sw, structure=np.ones((3, 3)))
    gr = nd.sum(sw, lb, range(1, n + 1)) if n else []
    gm = (lb == 1 + int(np.argmax(gr))) if n else np.zeros_like(sw)
    hull = cv2.convexHull(np.argwhere(gm)[:, ::-1].astype(np.int32)) if gm.sum() >= 3 else None
    solid = round(float(gm.sum() / max(cv2.contourArea(hull), 1)), 3) if hull is not None else None
    sp = sw.sum(0); sp = sp[sp > 0]
    zu = nd.binary_closing(sw, structure=np.ones((3, 3)), iterations=2)
    lbu, nu = nd.label(zu, structure=np.ones((3, 3)))
    um = (lbu == 1 + int(np.argmax(nd.sum(zu, lbu, range(1, nu + 1))))) if nu else zu
    ecken = None
    if um.sum() >= 10:
        ys, xs = np.nonzero(um)
        b = xs.ptp() + 1; q = max(2, int(round(0.15 * b)))
        yl = ys[xs < xs.min() + q].mean(); yr = ys[xs > xs.max() - q].mean()
        ecken = dict(links_y_H=round(float(y0 + yl - cy) / H, 4), rechts_y_H=round(float(y0 + yr - cy) / H, 4),
                     dy_rechts_minus_links_H=round(float(yr - yl) / H, 4))
        # Oberkante je Spalte: Rest-Std gegen Gerade (Unregelmaessigkeit der Oberlippenlinie)
        cols = np.arange(xs.min(), xs.max() + 1)
        top = np.array([ys[xs == c].min() for c in cols])
        k_, c_ = np.polyfit(cols, top, 1)
        ecken['oberlinie_rest_std_H'] = round(float((top - (k_ * cols + c_)).std()) / H, 4)
    erg0 = dict(schwarz_rand_e4H2=round(float((sw0 & ~sw).sum()) / H / H * 1e4, 1), schwarz15_e4H2=round(float((s < 15).sum()) / H / H * 1e4, 1), schwarz12_e4H2=round(float((s < 12).sum()) / H / H * 1e4, 1),
                umriss_iou=ellipse_iou(um), umriss_e4H2=round(float(um.sum()) / H / H * 1e4, 1), ecken=ecken)
    erg = dict(fenster=[x0, y0, x1, y1], schwarz_flaeche_e4H2=round(float(sw.sum()) / H / H * 1e4, 1),
               schwarz_teile=int(n), groesste_e4H2=round(float(gm.sum()) / H / H * 1e4, 1),
               spalt_median_px=float(np.median(sp)) if len(sp) else 0, spalt_median_H=round(float(np.median(sp)) / H, 4) if len(sp) else 0,
               spalt_max_H=round(float(sp.max()) / H, 4) if len(sp) else 0,
               ellipse_iou=ellipse_iou(gm), solidity=solid,
               glanz_px=int(((rgb.max(-1) > 170) & ~zm).sum()), zaehne=len(z))
    if gm.any():
        ys, xs = np.nonzero(gm)
        erg['groesste_bbox_H'] = [round((xs.ptp() + 1) / H, 4), round((ys.ptp() + 1) / H, 4)]
    if len(z) >= 4:
        yy_ = np.sort(z[:, 1]); i_ = int(np.argmax(np.diff(yy_)))
        grenze = (yy_[i_] + yy_[i_ + 1]) / 2
        erg['reihen_abstand_H'] = round(float(np.median(z[z[:, 1] > grenze, 1]) - np.median(z[z[:, 1] <= grenze, 1])) / H, 4)
        med = np.median(z[:, 1])
        ob = z[z[:, 1] <= med]; ob = ob[np.argsort(ob[:, 0])]
        if len(ob) >= 3:
            k, c0 = np.polyfit(ob[:, 0], ob[:, 1], 1)
            res = ob[:, 1] - (k * ob[:, 0] + c0)
            gaps = np.diff(ob[:, 0])
            erg.update(oben_n=len(ob), unten_n=int(len(z) - len(ob)), oben_neigung_grad=round(float(np.degrees(np.arctan(k))), 1),
                       oben_rest_std_H=round(float(res.std()) / H, 4),
                       oben_abstand_cv=round(float(gaps.std() / gaps.mean()), 2),
                       winkel_dy_H=round(float(k * (ob[-1, 0] - ob[0, 0])) / H, 4))
        erg['zahn_x_H'] = [round(float((x0 + z[:, 0].min() - cx) / H), 4), round(float((x0 + z[:, 0].max() - cx) / H), 4)]
        erg['zahn_mitte_x_H'] = round(float((x0 + (z[:, 0].min() + z[:, 0].max()) / 2 - cx) / H), 4)
    kx = cx + (erg.get('zahn_mitte_x_H', 0.0)) * H
    ky0, ky1 = int(cy + 0.09 * H), int(cy + 0.12 * H)
    kinn = A[ky0:ky1, int(kx - 0.02 * H):int(kx + 0.02 * H)].reshape(-1, 3)
    erg['lab_kinn'] = r1(kinn.mean(0))
    erg['kinn_anteil_a12'] = round(float((kinn[:, 1] >= 12).mean()), 2)
    erg['zahn_px'] = [(float(x0 + a), float(y0 + b)) for a, b, _ in z]
    erg.update(erg0)
    erg['spalten_x_H'] = [round(float(x0 + spalten[0] - cx) / H, 4), round(float(x0 + spalten[1] - 1 - cx) / H, 4)]
    erg['anteil_groesste'] = round(float(gm.sum()) / max(1, float(sw.sum())), 2)
    # Lippen/Maulrot: Maulspalten, Zeilen Auge + 0,02 .. + 0,09 H, nicht schwarz, kein Zahn, L* < 0,6 Lh, a* >= 10
    As = A[y0:y1, x0:x1]
    sp_m = np.zeros(s.shape, bool); sp_m[:, spalten[0]:spalten[1]] = True
    lip = sp_m & (s >= 0.25 * Lh) & ~zm & (s < 0.6 * Lh) & (As[..., 1] >= 10)
    erg['lippe_anteil'] = round(float(lip.sum()) / max(1, float(sp_m.sum())), 2)
    erg['lab_lippe'] = r1(As[lip].mean(0)) if lip.any() else None
    erg['L_lippe_kuppe'] = round(float(As[lip][:, 0].mean()) / Lh, 2) if lip.any() else None
    erg['glanz_maul_px'] = int(((rgb.max(-1) > 170) & sp_m).sum())
    erg['_gm'] = (x0, y0, gm, sw, um)
    return erg


def alles(wer, h):
    d = lade(wer, h)
    au = auge(d); ko = kopf(d, au); ha = haare(d, au, ko); ma = maul(d, au)
    return d, au, ko, ha, ma


def sauber(x):
    if isinstance(x, dict):
        return {k: sauber(v) for k, v in x.items() if k not in ('maske', 'profil', '_gm', 'zahn_px')}
    if isinstance(x, (list, tuple)):
        return [sauber(v) for v in x]
    if isinstance(x, (np.integer,)):
        return int(x)
    if isinstance(x, (np.floating,)):
        return float(x)
    return x


if __name__ == '__main__':
    wer = sys.argv[1:] or ['KI', 'r2', 'r1', 'v1']
    erg = {}
    for w in wer:
        for h in (118, 125):
            d, au, ko, ha, ma = alles(w, h)
            erg['%s h%d' % (w, h)] = sauber(dict(H=d['H'], auge=au, kopf=ko, haare=ha, maul=ma))
    json.dump(erg, open(N + 'N_mess.json', 'w'), indent=1)
    for k, v in erg.items():
        a, ko, ha, ma = v['auge'], v['kopf'], v['haare'], v['maul']
        print('==', k, 'H', v['H'])
        print(' auge', {x: a[x] for x in ('mitte', 'Lh', 'b_H', 'h_H', 'L_auge_haut', 'kern_px', 'kern_Lmax', 'kern_Lmax_haut', 'lab_kern', 'fenster_L55', 'fenster_heller_1_4Lh', 'fenster_px')})
        print(' kopf', {x: ko[x] for x in ('art', 'links', 'rechts', 'breite_H', 'lage_kopf', 'auge_zu_kopfmitte_H')})
        print(' haare', {x: ha[x] for x in ha if x != 'liste'})
        print(' maul', ma)
