"""Messblatt-Nachtrag V2 "Binnenzeichnung": Masse (a)-(f) an Atlas und Render, gleiche Definition fuer Familie und Varianten.

Definitionen (alle Zahlen hier, keine anderswo):
- Lab: CIE-Lab D65, OpenCV float (RGB/255), wie Messblatt V1.
- Atlas: p_lade.sammeln/p_messen.atlas (jeder Texel einmal, CLUT-Zeile der ersten Flaeche). Unbenutzte Texel werden fuer die
  Gradienten mit dem naechsten benutzten Texel gefuellt (scipy distance_transform_edt); gezaehlt wird nur, wo die 3x3-Nachbarschaft
  ganz benutzt ist und zum selben Mesh gehoert (keine Inselkanten, keine Meshnaehte).
- (a) Canny auf L* (8 bit = L* x 2,55), Sobel 3, L2-Gradient. Schwellen: niedrig 61 / hoch 122 = Stufe 6 / 12 L* (Sobel-3-Antwort auf
  eine Stufe d = 4 d). 6 und 12 L* = Schrittklassen des Sicht-Pruefers r3 2.1. Gegenprobe Schwellen 4/8 L* (41/82).
  Liniendichte = Kantentexel je 100 gueltige Zonentexel; lang = Anteil Kantentexel in 8-Komponenten mit >= 6 Texeln.
- (b) Kohaerenz je Feld (Zusammenhangskomponente eines Meshs im Atlas, >= 200 Texel): R = |Sum m e^(2i theta)| / Sum m ueber die
  Kantentexel (theta Gradientrichtung, m Betrag). Lokal: Strukturtensor, Gauss sigma 1,5 Texel, (l1-l2)/(l1+l2) an den Kantentexeln.
  Hauptrichtung der Kanten (= theta + 90 Grad) gegen die Laengsachse des Felds (PCA der Feldtexel); nur Felder mit Achsverhaeltnis >= 1,5.
- Rotklasse = Riegel A3 (a* > 15, C* > 20, Farbton -40..70 Grad). Rotflaeche = 8-Komponente der Rotklasse in der Zone mit >= 20 Texeln.
- (c) Aussenrand = Dilatation 5x5 (2 Texel) minus Flaeche, nur benutzte Zonentexel. dL_rand = L*(Flaeche) - L*(Aussenrand).
  "dunkler Rand": dL_rand >= 6 (Stufe 6 L* wie (a)).
- (d) Rotstufen: k-Mittel k = 6 (Lab, Saat 3) in der Flaeche, Zentren mit dE76 < 10 verkettet (Leser-B-Kette, Messblatt V1 8),
  gezaehlt Gruppen mit > 10 % der Flaeche.
- (e) L*- und a*-Streuung in der Rotflaeche: gepoolt innerhalb der Flaechen (Abzug Flaechenmittel), flaechengewichtet.
- (f) Hell-Kern: Kern = Erosion 5x5 (2 Texel) der Flaeche, Innenrand = Flaeche minus Kern; Hell-Kern, wenn L*(Kern) - L*(Innenrand) >= 6.
  Anteil an den Rotflaechen mit nicht leerem Kern.
Render: B_skripte/render.blick (Licht 0,55 + 0,45 max(0, N.L), weiss, Umkugel 0,46, 380 px, 3 Winkel), dieselben Masse auf den Bildpunkten;
gueltig nur Bildpunkte mit ganz innerer 3x3-Nachbarschaft (Erosion 3x3 der Figurmaske); Rotflaechen >= 20 px.

    python n2_mass.py [aus.json]        Familie + r3 + K4, Belegbilder N2_*.png nach farbe_lesung/
    python n2_mass.py --variante <ordner> [aus.json]   nur eine Variante (Atlas Koerper x < 384 + Render Clip 2)
"""
import os, sys, json, math
import numpy as np, cv2
from scipy import ndimage as ND

HIER = os.path.dirname(os.path.abspath(__file__))
FL = os.path.normpath(os.path.join(HIER, '..'))
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
sys.path.insert(0, os.path.join(FL, 'B_skripte'))
sys.path.insert(0, os.path.normpath(os.path.join(FL, '..', 'generator')))
import p_lade as PL
from p_messen import atlas as p_atlas, rect

K5 = PL.K5
SCHW = {'6/12': (61, 122), '4/8': (41, 82)}
ROT_MIN = 20


def lab_map(rgb, u):
    L = np.full(rgb.shape, np.nan, np.float32)
    L[u] = PL.lab(rgb[u].astype(np.float32))
    return L


def rotklasse(L):
    C = np.hypot(L[..., 1], L[..., 2])
    h = np.degrees(np.arctan2(L[..., 2], L[..., 1]))
    return (L[..., 1] > 15) & (C > 20) & (h > -40) & (h < 70)


def fuellen(Lk, u):
    idx = ND.distance_transform_edt(~u, return_distances=False, return_indices=True)
    return Lk[idx[0], idx[1]]


def kanten(L0f, valid, schw):
    img = np.clip(L0f * 2.55, 0, 255).astype(np.uint8)
    e = cv2.Canny(img, schw[0], schw[1], apertureSize=3, L2gradient=True) > 0
    return e & valid


def lange(e):
    n, lb, st, _ = cv2.connectedComponentsWithStats(e.astype(np.uint8), connectivity=8)
    if n <= 1:
        return 0.0
    gr = st[1:, cv2.CC_STAT_AREA]
    return float(gr[gr >= 6].sum() / max(gr.sum(), 1))


def tensor(L0f):
    gx = cv2.Sobel(L0f.astype(np.float32), cv2.CV_32F, 1, 0, ksize=3)
    gy = cv2.Sobel(L0f.astype(np.float32), cv2.CV_32F, 0, 1, ksize=3)
    J11 = cv2.GaussianBlur(gx * gx, (0, 0), 1.5)
    J22 = cv2.GaussianBlur(gy * gy, (0, 0), 1.5)
    J12 = cv2.GaussianBlur(gx * gy, (0, 0), 1.5)
    tr = J11 + J22
    d = np.sqrt((J11 - J22) ** 2 + 4 * J12 ** 2)
    coh = np.where(tr > 1e-6, d / np.maximum(tr, 1e-6), 0)
    return gx, gy, coh


def stufen(P):
    cv2.setRNGSeed(3)
    k = min(6, len(P))
    _, lbk, cen = cv2.kmeans(P.astype(np.float32), k, None, (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.05), 4,
                             cv2.KMEANS_PP_CENTERS)
    lbk = lbk.ravel()
    ant = np.bincount(lbk, minlength=k) / len(lbk)
    grp = list(range(k))
    D = np.linalg.norm(cen[:, None] - cen[None], axis=2)
    ch = True
    while ch:
        ch = False
        for a_ in range(k):
            for b_ in range(k):
                if D[a_, b_] < 10 and grp[a_] != grp[b_]:
                    alt, neu = grp[b_], grp[a_]
                    grp = [neu if g == alt else g for g in grp]
                    ch = True
    ga = {}
    for j in range(k):
        ga[grp[j]] = ga.get(grp[j], 0) + ant[j]
    return int(sum(1 for v in ga.values() if v > 0.10))


def linien_stat(L0f, valid, zone, thr=6):
    """Dunkle Linien (Adern/Falten): Black-Top-Hat 5x5 auf L* >= thr (6 L* wie (a)); 8-Komponenten >= 6 Texel, Laenge 4 sqrt(l1) >= 6 Texel,
    Elongation sqrt(l1/l2) >= 3. Rueckgabe: Anteil Linientexel je gueltige Zonentexel %, Linien je 1000 Texel, Breite (Flaeche/Laenge),
    Laenge Median, Kontrast (Top-Hat-Mittel), Liste (Laenge, Breite, Winkel gegen u)."""
    k = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
    bh = cv2.morphologyEx(L0f.astype(np.float32), cv2.MORPH_BLACKHAT, k)
    m = (bh >= thr) & valid & zone
    n, lb, st, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    lin = np.zeros_like(m)
    L = []
    for i in range(1, n):
        if st[i, 4] < 6:
            continue
        x, y, w, h = st[i, :4]
        sub = lb[y:y + h, x:x + w] == i
        ys, xs = np.nonzero(sub)
        ev, evec = np.linalg.eigh(np.cov(np.stack([xs, ys]).astype(float)))
        lang = 4 * math.sqrt(max(ev[1], 0))
        el = math.sqrt(ev[1] / max(ev[0], 0.05))
        if lang >= 6 and el >= 3:
            lin[y:y + h, x:x + w] |= sub
            ang = math.degrees(math.atan2(evec[1, 1], evec[0, 1])) % 180
            L.append((lang, st[i, 4] / lang, ang, float(bh[y:y + h, x:x + w][sub].mean())))
    zv = max(int((zone & valid).sum()), 1)
    A = np.array(L) if L else np.zeros((0, 4))
    return dict(anteil_p=round(100 * float(lin.sum()) / zv, 2), je1000=round(1000 * len(L) / zv, 2),
                breite=round(float(np.median(A[:, 1])), 2) if len(A) else None, laenge_med=round(float(np.median(A[:, 0])), 1) if len(A) else None,
                laenge_p90=round(float(np.percentile(A[:, 0], 90)), 1) if len(A) else None,
                kontrast=round(float(np.average(A[:, 3], weights=A[:, 0])), 1) if len(A) else None), lin


def rot_flaechen(L, zone, valid_ring):
    """Masse (c)-(f) an den Rotflaechen einer Zone (Bool-Masken gleicher Form). valid_ring = wo Randtexel zaehlen."""
    rot = rotklasse(np.nan_to_num(L)) & zone
    n, lb, st, _ = cv2.connectedComponentsWithStats(rot.astype(np.uint8), connectivity=8)
    k5 = np.ones((5, 5), np.uint8)
    out = []
    for i in range(1, n):
        if st[i, cv2.CC_STAT_AREA] < ROT_MIN:
            continue
        x, y, w, h = st[i, :4]
        x0, y0, x1, y1 = max(x - 3, 0), max(y - 3, 0), min(x + w + 3, L.shape[1]), min(y + h + 3, L.shape[0])
        c = (lb[y0:y1, x0:x1] == i)
        Ls = L[y0:y1, x0:x1]
        ring = cv2.dilate(c.astype(np.uint8), k5).astype(bool) & ~c & valid_ring[y0:y1, x0:x1]
        kern = cv2.erode(c.astype(np.uint8), k5, borderType=cv2.BORDER_CONSTANT, borderValue=0).astype(bool)
        innen = c & ~kern
        P = Ls[c]
        r = dict(n=int(c.sum()), L=float(P[:, 0].mean()), a=float(P[:, 1].mean()),
                 sdL=float(P[:, 0].std()), sda=float(P[:, 1].std()),
                 dL_rand=(float(P[:, 0].mean() - Ls[ring][:, 0].mean()) if ring.sum() >= 3 else None),
                 dL_kern=(float(Ls[kern][:, 0].mean() - Ls[innen][:, 0].mean()) if kern.sum() >= 3 and innen.sum() >= 3 else None),
                 stufen=stufen(P), x=int(x), y=int(y))
        out.append(r)
    return out, rot


def zus_rot(fl_):
    if not fl_:
        return dict(n_flaechen=0)
    n = np.array([f['n'] for f in fl_], float)
    sdL = math.sqrt(float((n * np.array([f['sdL'] for f in fl_]) ** 2).sum() / n.sum()))
    sda = math.sqrt(float((n * np.array([f['sda'] for f in fl_]) ** 2).sum() / n.sum()))
    rr = [f for f in fl_ if f['dL_rand'] is not None]
    kk = [f for f in fl_ if f['dL_kern'] is not None]
    st = np.array([f['stufen'] for f in fl_])
    return dict(n_flaechen=len(fl_), texel=int(n.sum()), groesste=int(n.max()),
                dL_rand_med=round(float(np.median([f['dL_rand'] for f in rr])), 1) if rr else None,
                dL_rand_gew=round(float(np.average([f['dL_rand'] for f in rr], weights=[f['n'] for f in rr])), 1) if rr else None,
                dunkler_rand_p=round(100 * float(np.mean([f['dL_rand'] >= 6 for f in rr])), 1) if rr else None,
                dunkler_rand_gew_p=round(100 * float(np.average([f['dL_rand'] >= 6 for f in rr], weights=[f['n'] for f in rr])), 1) if rr else None,
                stufen_med=float(np.median(st)), stufen_gew=round(float(np.average(st, weights=n)), 2),
                stufen_ge2_gew_p=round(100 * float(np.average(st >= 2, weights=n)), 1),
                sdL_pool=round(sdL, 1), sda_pool=round(sda, 1),
                kern_n=len(kk), hellkern_p=round(100 * float(np.mean([f['dL_kern'] >= 6 for f in kk])), 1) if kk else None,
                hellkern_gew_p=round(100 * float(np.average([f['dL_kern'] >= 6 for f in kk], weights=[f['n'] for f in kk])), 1) if kk else None,
                dL_kern_med=round(float(np.median([f['dL_kern'] for f in kk])), 1) if kk else None)


def atlas_mass(mo, zone_fn, name, bild=None):
    per, tri = PL.sammeln(mo)
    rgb, mid = p_atlas(mo, per)
    u = mid >= 0
    L = lab_map(rgb, u)
    zone = zone_fn(u, mid)
    mmin = ND.minimum_filter(np.where(u, mid, -1), 3, mode='constant', cval=-1)
    mmax = ND.maximum_filter(np.where(u, mid, 10 ** 6), 3, mode='constant', cval=10 ** 6)
    valid = u & (mmin == mid) & (mmax == mid)
    L0f = fuellen(np.nan_to_num(L[..., 0]), u)
    gx, gy, coh = tensor(L0f)
    res = dict(name=name, zone_texel=int(zone.sum()), gueltig_texel=int((zone & valid).sum()))
    rot_fl, rot = rot_flaechen(L, zone, u & zone)
    fleisch = cv2.dilate(rot.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & zone
    haut = zone & ~fleisch
    res['rot_anteil_p'] = round(100 * float(rot.sum() / max(zone.sum(), 1)), 1)
    res['fleisch_texel'] = int(fleisch.sum())
    res['haut_texel'] = int(haut.sum())
    E = {}
    for sk, s in SCHW.items():
        e = kanten(L0f, valid, s)
        d = {}
        for zn, z in (('zone', zone), ('fleisch', fleisch), ('haut', haut)):
            ez = e & z
            zv = z & valid
            d[zn] = dict(dichte=round(100 * float(ez.sum() / max(zv.sum(), 1)), 2), lang_p=round(100 * lange(ez), 1))
        E[sk] = (e & zone)
        res['kanten_' + sk] = d
    for zn, z in (('zone', zone), ('fleisch', fleisch), ('haut', haut)):
        res['linien_' + zn] = linien_stat(L0f, valid, z)[0]
    e = E['6/12']
    felder = []
    for b in np.unique(mid[zone]):
        mb = zone & (mid == b)
        n, lb, st, _ = cv2.connectedComponentsWithStats(mb.astype(np.uint8), connectivity=8)
        for i in range(1, n):
            if st[i, cv2.CC_STAT_AREA] < 200:
                continue
            f = lb == i
            ef = e & f
            if ef.sum() < 10:
                felder.append(dict(n=int(f.sum()), ne=int(ef.sum()), R=None))
                continue
            th = np.arctan2(gy[ef], gx[ef])
            m = np.hypot(gx[ef], gy[ef])
            z2 = (m * np.exp(2j * th)).sum() / m.sum()
            R = float(abs(z2))
            kant_dir = (0.5 * math.degrees(math.atan2(z2.imag, z2.real)) + 90) % 180
            ys, xs = np.nonzero(f)
            ev, evec = np.linalg.eigh(np.cov(np.stack([xs, ys]).astype(float)))
            ach = math.degrees(math.atan2(evec[1, 1], evec[0, 1])) % 180
            asp = math.sqrt(ev[1] / max(ev[0], 1e-6))
            dw = abs(kant_dir - ach) % 180
            dw = min(dw, 180 - dw)
            felder.append(dict(n=int(f.sum()), ne=int(ef.sum()), R=R, lok=float(coh[ef].mean()), kdir=kant_dir, ach=ach, asp=asp, dw=dw))
    fz = [f for f in felder if f['R'] is not None]
    w = np.array([f['ne'] for f in fz], float)
    res['felder'] = len(felder)
    res['felder_mit_kanten'] = len(fz)
    if fz:
        res['koh_feld_gew'] = round(float(np.average([f['R'] for f in fz], weights=w)), 3)
        res['koh_feld_med'] = round(float(np.median([f['R'] for f in fz])), 3)
        res['koh_lokal_gew'] = round(float(np.average([f['lok'] for f in fz], weights=w)), 3)
        lg = [f for f in fz if f['asp'] >= 1.5]
        if lg:
            wl = np.array([f['ne'] for f in lg], float)
            res['laengs_felder'] = len(lg)
            res['dw_achse_med'] = round(float(np.median([f['dw'] for f in lg])), 1)
            res['dw_achse_lt30_gew_p'] = round(100 * float(np.average([f['dw'] < 30 for f in lg], weights=wl)), 1)
            res['dw_achse_gt60_gew_p'] = round(100 * float(np.average([f['dw'] > 60 for f in lg], weights=wl)), 1)
        res['kdir_u_med'] = round(float(np.median([min(f['kdir'], 180 - f['kdir']) for f in fz])), 1)
    res['rot'] = zus_rot(rot_fl)
    Rz = L[rot]
    res['rot_sdL_ges'] = round(float(Rz[:, 0].std()), 1) if len(Rz) else None
    res['rot_sda_ges'] = round(float(Rz[:, 1].std()), 1) if len(Rz) else None
    if bild is not None:
        bild_atlas(bild, rgb, u, E['6/12'], gx, gy, coh, zone, rot)
    return res


def bild_atlas(pfad, rgb, u, e, gx, gy, coh, zone, rot):
    """Belegbild x3: links Atlas roh (unbenutzt schwarz), Mitte Kanten (weiss) ueber abgedunkeltem Atlas (x 0,35), Rotflaechen-Umriss gelb,
    rechts Richtungsfeld: Kantentexel eingefaerbt nach Kantenrichtung (HSV-Farbton = Winkel 0..180 Grad gegen u), Helligkeit = lokale Kohaerenz;
    Zone ohne Kante dunkelgrau."""
    h, w = u.shape
    a = rgb.copy()
    a[~u] = 0
    b = (rgb.astype(float) * 0.35).astype(np.uint8)
    b[~u] = 0
    b[e] = (255, 255, 255)
    rr = rot & ~cv2.erode(rot.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    b[rr & ~e] = (255, 220, 0)
    th = (np.degrees(np.arctan2(gy, gx)) + 90) % 180
    hsv = np.zeros((h, w, 3), np.uint8)
    hsv[..., 0] = (th / 180 * 179).astype(np.uint8)
    hsv[..., 1] = 255
    hsv[..., 2] = (np.clip(coh, 0, 1) * 255).astype(np.uint8)
    c = cv2.cvtColor(hsv, cv2.COLOR_HSV2RGB)
    grau = np.zeros((h, w, 3), np.uint8)
    grau[u & zone] = 40
    c[~e] = grau[~e]
    z = np.zeros((h, 6, 3), np.uint8) + 128
    out = np.concatenate([a, z, b, z, c], 1)
    out = cv2.resize(out, None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST)
    cv2.imwrite(pfad, out[:, :, ::-1])


def render_mass(mo, clip, name, bild=None):
    import render as R
    res = dict(name=name, winkel=[])
    alle_fl = []
    for wt, az, el in R.WINKEL:
        img, msk = R.blick(mo, R.kf(mo, clip), az, el, 380, None)
        valid = cv2.erode(msk.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
        L = np.full(img.shape, np.nan, np.float32)
        L[msk] = PL.lab(img[msk].astype(np.float32))
        L0 = np.nan_to_num(L[..., 0])
        e = kanten(L0, valid, SCHW['6/12'])
        rot_fl, rot = rot_flaechen(L, msk, msk)
        alle_fl += rot_fl
        fleisch = cv2.dilate(rot.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & msk
        haut = msk & ~fleisch
        gx, gy, coh = tensor(L0)
        d = dict(winkel=wt, px=int(msk.sum()), rot_p=round(100 * float(rot.sum() / msk.sum()), 1),
                 dichte=round(100 * float(e.sum() / valid.sum()), 2), lang_p=round(100 * lange(e), 1),
                 dichte_fleisch=round(100 * float((e & fleisch).sum() / max((valid & fleisch).sum(), 1)), 2),
                 dichte_haut=round(100 * float((e & haut).sum() / max((valid & haut).sum(), 1)), 2),
                 koh_lokal=round(float(coh[e].mean()), 3) if e.sum() else None,
                 rot=zus_rot(rot_fl))
        Rz = L[rot]
        d['rot_sdL_ges'] = round(float(Rz[:, 0].std()), 1) if len(Rz) else None
        d['rot_sda_ges'] = round(float(Rz[:, 1].std()), 1) if len(Rz) else None
        d['rot_Lmed'] = round(float(np.median(Rz[:, 0])), 1) if len(Rz) else None
        res['winkel'].append(d)
        if bild is not None and wt == 'Seite':
            a = img.copy()
            a[~msk] = 0
            b = (img.astype(float) * 0.35).astype(np.uint8)
            b[~msk] = 0
            b[e] = (255, 255, 255)
            rr = rot & ~cv2.erode(rot.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
            b[rr & ~e] = (255, 220, 0)
            z = np.zeros((380, 6, 3), np.uint8) + 128
            out = cv2.resize(np.concatenate([a, z, b], 1), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
            cv2.imwrite(bild, out[:, :, ::-1])
    res['rot_alle'] = zus_rot(alle_fl)
    return res


def zonen():
    def em30(u, mid):
        return u & ~np.isin(mid, [0, 1, 3, 4, 6, 9])

    def g4(u, mid):
        h, w = mid.shape
        return u & ~np.isin(mid, [26, 4, 8, 14, 19, 22, 25]) & ~rect(h, w, (270, 160, 52, 86))

    def g5(u, mid):
        h, w = mid.shape
        return u & ~rect(h, w, (0, 143, 228, 113)) & ~rect(h, w, (450, 196, 48, 46)) & ~rect(h, w, (168, 95, 22, 22))

    def k5(u, mid):
        return u & (np.arange(mid.shape[1])[None, :] < 384)

    def k5kopf(u, mid):
        h, w = mid.shape
        return u & rect(h, w, (384, 0, 128, 104))

    def g4kopf(u, mid):
        return u & (mid == 26)

    def em30kopf(u, mid):
        return u & (mid == 9)
    return dict(em30=em30, g4=g4, g5=g5, k5=k5, k5kopf=k5kopf, g4kopf=g4kopf, em30kopf=em30kopf)


def variante(vdir, name):
    import fl
    Z = zonen()
    mo = fl.lade_var(vdir)
    return dict(atlas=atlas_mass(mo, Z['k5'], name), render=render_mass(mo, 2, name), kopf=atlas_mass(mo, Z['k5kopf'], name + '_kopf'))


if __name__ == '__main__':
    if '--variante' in sys.argv:
        i = sys.argv.index('--variante')
        vd = sys.argv[i + 1]
        r = variante(vd, os.path.basename(os.path.normpath(vd)))
        aus = sys.argv[i + 2] if len(sys.argv) > i + 2 else None
        print(json.dumps(r, ensure_ascii=False, indent=1))
        if aus:
            json.dump(r, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        sys.exit(0)
    import fl
    import kf_tafeln as KT
    aus = sys.argv[1] if len(sys.argv) > 1 else os.path.join(FL, 'N2_messung.json')
    bild = os.path.join(FL, 'N2_%s.png')
    em = fl.lade_em30()
    g4 = KT.re2_mo(0x34)
    g5 = KT.re2_mo(0x36)
    r3 = fl.lade_var(K5 + '/farbe/K5c_farbe_r3')
    k4 = fl.lade_var(K5 + '/K4_heute')
    Z = zonen()
    out = {}
    for nm, mo, zf, clip in (('EM30_haut', em, Z['em30'], 0), ('G4_koerper', g4, Z['g4'], 0), ('G5_koerper', g5, Z['g5'], 0),
                             ('r3_koerper', r3, Z['k5'], 2), ('K4_koerper', k4, Z['k5'], 2)):
        kurz = nm.split('_')[0]
        out[nm] = dict(atlas=atlas_mass(mo, zf, nm, bild % ('atlas_' + kurz)),
                       render=render_mass(mo, clip, nm, bild % ('render_seite_' + kurz)))
        print(nm, json.dumps(out[nm]['atlas'], ensure_ascii=False))
        print('  render', json.dumps(out[nm]['render']['rot_alle'], ensure_ascii=False),
              [(w['winkel'], w['dichte'], w['lang_p'], w['rot_p'], w['rot_sdL_ges'], w['rot_sda_ges'], w['koh_lokal']) for w in out[nm]['render']['winkel']])
        sys.stdout.flush()
    for nm, mo, zf in (('EM30_kopf', em, Z['em30kopf']), ('G4_kopf', g4, Z['g4kopf']), ('r3_kopf', r3, Z['k5kopf']), ('K4_kopf', k4, Z['k5kopf'])):
        out[nm] = dict(atlas=atlas_mass(mo, zf, nm))
        print(nm, json.dumps(out[nm]['atlas'], ensure_ascii=False))
    json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
