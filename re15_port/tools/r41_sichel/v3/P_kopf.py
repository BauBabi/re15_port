# Pruefer V3: Kopfauge, Schulterauge, Maul an KI-Freistellern (eigene Rechnung). Verfahren auch fuer Port-Renders nutzbar.
# Auge: Minimum (Gauss 1 px) im Suchfenster; Haut = Median L* im Ring 6-9 px um das Minimum; Auge = Zusammenhangskomponente
# L* < (Lmin + LHaut)/2 (Halbwert) mit dem Minimum, davon die konvexe Huelle (heller Kern zaehlt zum Auge). Lid = 2-px-Ring um das Auge,
# Haut = Ring 5-7 px um das Auge.
# Maul: groesste Komponente L* < 0,5 x Median-L* des Maulfensters, konvexe Huelle = Maul (Zahnflecken eingeschlossen).
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from P_farbe import lab, de00, ch, r1

OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/'


def lstern(rgb):
    return lab(rgb)[..., 0]


def auge(rgb, alpha, fenster, H, rhaut=(6, 9), schwelle='halb'):
    x0, y0, x1, y1 = fenster
    L = lstern(rgb); Ls = nd.gaussian_filter(L, 1.0)
    sub = np.where(alpha[y0:y1, x0:x1] > 250, Ls[y0:y1, x0:x1], 999)
    iy, ix = np.unravel_index(np.argmin(sub), sub.shape); cy, cx = iy + y0, ix + x0
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]; r = np.hypot(yy - cy, xx - cx)
    ring = (r >= rhaut[0]) & (r <= rhaut[1]) & (alpha > 250)
    Lh = float(np.median(L[ring])); Lmin = float(L[cy, cx])
    thr = (Lmin + Lh) / 2 if schwelle == 'halb' else schwelle * Lh
    lab_, n = nd.label((L < thr) & (r < 14))
    dunkel = lab_ == lab_[cy, cx]
    pts = np.argwhere(dunkel)[:, ::-1].astype(np.int32)
    hu = np.zeros(L.shape, np.uint8); cv2.fillConvexPoly(hu, cv2.convexHull(pts), 1)
    m = hu.astype(bool) | dunkel   # Auge = konvexe Huelle des dunklen Rings (der helle Kern liegt darin)
    ys, xs = np.nonzero(m); w, h = xs.max() - xs.min() + 1, ys.max() - ys.min() + 1
    A = lab(rgb)
    lid = nd.binary_dilation(m, iterations=2) & ~m
    haut = nd.binary_dilation(m, iterations=7) & ~nd.binary_dilation(m, iterations=4) & (alpha > 250)
    Lm = L[m]; pup = m & (L <= np.percentile(Lm, 25))
    Lhm = float(L[haut].mean())
    glanz = m & (L > Lhm)
    d = dict(mitte=r1([xs.mean(), ys.mean()]), min=[int(cx), int(cy)], b_px=int(w), h_px=int(h), flaeche=int(m.sum()),
             aequiv_px=round(float(np.sqrt(4 * m.sum() / np.pi)), 1), b_H=round(w / H, 4), h_H=round(h / H, 4),
             aequiv_H=round(float(np.sqrt(4 * m.sum() / np.pi)) / H, 4), bh=round(w / h, 2),
             lab_auge=r1(A[m].mean(0)), rgb_auge=r1(rgb[m].mean(0), 0), lab_pupille=r1(A[pup].mean(0)), rgb_pupille=r1(rgb[pup].mean(0), 0),
             lab_lid=r1(A[lid].mean(0)), rgb_lid=r1(rgb[lid].mean(0), 0), lab_haut=r1(A[haut].mean(0)), rgb_haut=r1(rgb[haut].mean(0), 0),
             L_auge_haut=round(float(Lm.mean() / Lhm), 3), L_lid_haut=round(float(L[lid].mean() / Lhm), 3),
             L_pup_haut=round(float(L[pup].mean() / Lhm), 3),
             anteil_L80=round(float((Lm > 80).mean()), 3), anteil_heller_haut=round(float((Lm > Lhm).mean()), 3),
             anteil_haut_plus15=round(float((Lm > Lhm + 15).mean()), 3), anteil_dunkel=round(float(dunkel.sum() / m.sum()), 3),
             lab_dunkel=r1(A[dunkel].mean(0)), L_dunkel_haut=round(float(L[dunkel].mean() / Lhm), 3),
             glanz_px=int(glanz.sum()), glanz_Lmax=round(float(Lm.max()), 1),
             schwelle=round(thr, 1), Lmin=round(Lmin, 1), Lhaut_ring=round(Lh, 1),
             lid_hell=round(float((L[lid] > Lhm + 15).mean()), 3))
    if glanz.any():
        gy_, gx_ = np.nonzero(glanz)
        d['glanz_mitte'] = r1([gx_.mean(), gy_.mean()])
        d['lab_glanz'] = r1(A[glanz].mean(0)); d['rgb_glanz'] = r1(rgb[glanz].mean(0), 0)
        d['glanz_rel_box'] = r1([(gx_.mean() - xs.min() + 0.5) / w, (gy_.mean() - ys.min() + 0.5) / h], 2)
    return d, m


def maul(rgb, alpha, fenster, H, tophat=15.0, dunkel=20.0):
    """Zahnflecken = Top-Hat (L* - Oeffnung 5x5) > tophat und L* > 40, Flecken 1-12 px, deren 3-px-Umgebung einen Bildpunkt
    L* < dunkel enthaelt (= Zahn am dunklen Maulinneren, nicht Hautsprenkel). Maul = konvexe Huelle der Zahnflecken.
    Fenster: Kopfbreite +-4 px, Augenmitte + 0,028 H bis + 0,08 H (darueber liegen Hautsprenkel zwischen Auge und Maul)."""
    x0, y0, x1, y1 = fenster
    L = lstern(rgb); A = lab(rgb)
    sub = L[y0:y1, x0:x1]; al = alpha[y0:y1, x0:x1] > 250
    th = sub - nd.grey_opening(sub, size=(5, 5))
    z0 = (th > tophat) & (sub > 40) & al
    zl0, zn0 = nd.label(z0); keep = []
    for i in range(1, zn0 + 1):
        c = zl0 == i
        if c.sum() <= 12 and sub[nd.binary_dilation(c, iterations=3)].min() < dunkel:
            keep.append(i)
    zahn = np.isin(zl0, keep)
    pts = np.argwhere(zahn)[:, ::-1].astype(np.int32)
    hull = np.zeros(sub.shape, np.uint8); cv2.fillConvexPoly(hull, cv2.convexHull(pts), 1); hull = hull.astype(bool) | zahn
    zl, zn = nd.label(zahn)
    ys, xs = np.nonzero(hull); w, h = xs.max() - xs.min() + 1, ys.max() - ys.min() + 1
    inn = hull & ~nd.binary_dilation(zahn, iterations=1) & (sub < 30)
    zc = [r1([c[1] + x0, c[0] + y0], 0) for c in nd.center_of_mass(zahn, zl, range(1, zn + 1))] if zn else []
    my = (ys.min() + ys.max()) / 2 + y0
    oben = sum(1 for c in zc if c[1] < my); unten = zn - oben
    d = dict(bbox=[int(xs.min() + x0), int(ys.min() + y0), int(xs.max() + x0), int(ys.max() + y0)], b_px=int(w), h_px=int(h),
             b_H=round(w / H, 4), h_H=round(h / H, 4), bh=round(w / h, 2),
             lab_innen_L30=r1(A[y0:y1, x0:x1][inn].mean(0)), rgb_innen_L30=r1(rgb[y0:y1, x0:x1][inn].mean(0), 0),
             zahnflecken=int(zn), oben=oben, unten=unten, zahn_px_mittel=round(float(zahn.sum()) / max(zn, 1), 1),
             lab_zahn=(r1(A[y0:y1, x0:x1][zahn].mean(0)) if zn else None), mitte=r1([(xs.min() + xs.max()) / 2 + x0, my]))
    full = np.zeros(L.shape, bool); full[y0:y1, x0:x1] = hull
    return d, full


def kontur(vis, msk, col):
    c, _ = cv2.findContours(msk.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(vis, c, -1, col, 1)


if __name__ == '__main__':
    V = 'C:/workspace/git/reAi_v2/Version_80%/'
    os.makedirs(OUT, exist_ok=True)
    # Fenster in Freisteller-Koordinaten (Lupen k19/k20 x6). H: birkin19 569 (Auftrag), birkin20 Alpha-Bbox 601 - 2 (wie 19: 571 -> 569).
    F = {'birkin19': dict(H=569, auge=(172, 28, 202, 56), schulter=(105, 55, 140, 85), maul=(162, 48, 226, 88), kopf=(166, 2, 226, 114)),
         'birkin20': dict(H=599, auge=(192, 28, 222, 56), schulter=(95, 50, 140, 85), maul=(182, 50, 246, 92), kopf=(184, 1, 242, 127))}
    erg = {}
    for n, f in F.items():
        im = cv2.imread(V + n + '.png', cv2.IMREAD_UNCHANGED); rgb = im[:, :, 2::-1].astype(float); al = im[:, :, 3]
        a, ma = auge(rgb, al, f['auge'], f['H']); a6, _ = auge(rgb, al, f['auge'], f['H'], schwelle=0.6)
        s, ms = auge(rgb, al, f['schulter'], f['H'])
        kb = f['kopf']
        mu, mm = maul(rgb, al, (kb[0] - 4, int(a['mitte'][1] + 0.028 * f['H']), kb[2] + 4, int(a['mitte'][1] + 0.08 * f['H'])), f['H'])
        kb = f['kopf']
        a['lage_kopfbox'] = r1([(a['mitte'][0] - kb[0]) / (kb[2] - kb[0]), (a['mitte'][1] - kb[1]) / (kb[3] - kb[1])], 3)
        a['ueber_maulmitte_H'] = round((mu['mitte'][1] - a['mitte'][1]) / f['H'], 4)
        a['b_kopfbreite'] = round(a['b_px'] / (kb[2] - kb[0]), 3)
        mu['b_kopfbreite'] = round(mu['b_px'] / (kb[2] - kb[0]), 3)
        s['relativ_kopfauge_H'] = r1([(s['mitte'][0] - a['mitte'][0]) / f['H'], (s['mitte'][1] - a['mitte'][1]) / f['H']], 3)
        erg[n] = dict(kopfauge=a, kopfauge_schwelle06=dict((k, a6[k]) for k in ('b_px', 'h_px', 'aequiv_H', 'L_auge_haut', 'anteil_haut_plus15')),
                      schulterauge=s, maul=mu)
        vis = np.ascontiguousarray(im[:, :, :3].copy())
        kontur(vis, ma, (0, 255, 0)); kontur(vis, ms, (255, 200, 0)); kontur(vis, mm, (255, 0, 255))
        cr = vis[0:130, 90:260]
        cv2.imwrite(OUT + 'P_kopf_%s.png' % n, cv2.resize(cr, None, fx=5, fy=5, interpolation=cv2.INTER_NEAREST))
    json.dump(erg, open(OUT + 'P_kopf.json', 'w'), indent=1)
    print(json.dumps(erg, indent=1))




def auge_box(rgb, mitte, H):
    """RIEGEL-Mass Auge, massstabsfrei (KI-Freisteller und Port-Nahansicht gleich): Haut = Median L* im Ring 0,0105-0,0158 H
    um die angegebene Augenmitte; Lmin = Minimum (Gauss 0,0018 H) im Umkreis 0,01 H; dunkel = L* < (Lmin + Haut)/2;
    Auge = konvexe Huelle ALLER dunklen Bildpunkte im Umkreis 0,02 H (ein heller Ring zwischen dunklem Rand und Pupille
    zaehlt so zum Auge). Liefert Bbox (H), B/H, L*-Mittel/Haut, Anteile L* > Haut+15 und L* > 80, Lab."""
    A = lab(rgb); L = A[..., 0]; cx, cy = mitte
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]; r = np.hypot(xx - cx, yy - cy)
    ring = (r >= 0.0105 * H) & (r <= 0.0158 * H)
    Lh = float(np.median(L[ring]))
    Ls = nd.gaussian_filter(L, max(0.5, 0.00176 * H))
    Lmin = float(Ls[r <= 0.01 * H].min())
    dunkel = (L < (Lmin + Lh) / 2) & (r <= 0.02 * H)
    lb, n = nd.label(dunkel)
    if n == 0:
        return None
    gr = nd.sum(dunkel, lb, range(1, n + 1))
    dunkel = np.isin(lb, [i + 1 for i, g in enumerate(gr) if g >= max(2, (0.002 * H) ** 2)])
    pts = np.argwhere(dunkel)[:, ::-1].astype(np.int32)
    hu = np.zeros(L.shape, np.uint8); cv2.fillConvexPoly(hu, cv2.convexHull(pts), 1); m = hu.astype(bool) | dunkel
    ys, xs = np.nonzero(m); w, h = xs.ptp() + 1, ys.ptp() + 1
    hell = m & (L > Lh + 15)
    return dict(b_H=round(w / H, 4), h_H=round(h / H, 4), bh=round(w / h, 2), aequiv_H=round(float(np.sqrt(4 * m.sum() / np.pi)) / H, 4),
                L_auge_haut=round(float(L[m].mean() / Lh), 3),
                anteil_haut15=round(float(hell.sum() / m.sum()), 3), anteil_L80=round(float((L[m] > 80).mean()), 3),
                anteil_dunkel=round(float(dunkel.sum() / m.sum()), 3), L_dunkel_haut=round(float(L[dunkel].mean() / Lh), 3),
                lab_auge=r1(A[m].mean(0)), lab_haut_ring=r1(A[ring].mean(0)), lab_hell=(r1(A[hell].mean(0)) if hell.any() else None),
                lab_dunkel=r1(A[dunkel].mean(0)), mitte_box=r1([(xs.min() + xs.max()) / 2, (ys.min() + ys.max()) / 2]))
