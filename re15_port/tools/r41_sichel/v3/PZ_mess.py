# Zahlen-Pruefer V3 r1: eigene Messung (skimage-Lab/CIEDE2000, eigene Masken/Mittellinie) an den Renders aus PZ_render.py.
# python PZ_mess.py NAME  -> pz/m_NAME.json
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
from skimage.color import rgb2lab, deltaE_ciede2000
from skimage.graph import MCP_Geometric
from skimage.morphology import convex_hull_image
OUT = os.environ.get('PZ_OUT', 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/pz/')
HP = 0.9 * 3865          # Messblatt 8: H der Nahansicht in px
H5 = 569                 # Messblatt 8: Verkleinerung auf H 569
SW = list(range(16, 23))


def LAB(rgb):
    return rgb2lab(np.asarray(rgb, float) / 255.0)


def rd(x, n=4):
    return None if x is None else round(float(x), n)


# ---------------------------------------------------------------- Auge (Messblatt 1 / R3-R9), eigene Umsetzung
def auge(rgb, alp, mitte, H, fenster=12):
    A = LAB(rgb); L = A[..., 0]
    cx, cy = int(round(mitte[0])), int(round(mitte[1]))
    y0, y1, x0, x1 = cy - fenster, cy + fenster + 1, cx - fenster, cx + fenster + 1
    Lg = nd.gaussian_filter(L, 1.0)
    sub = Lg[y0:y1, x0:x1].copy(); sub[~alp[y0:y1, x0:x1]] = 1e9
    iy, ix = np.unravel_index(np.argmin(sub), sub.shape); py, px = iy + y0, ix + x0
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - px, yy - py)
    ring = (rr >= 6) & (rr <= 9) & alp
    Lh0 = float(np.median(L[ring]))
    Lmin = float(L[py, px])                       # Messblatt 1: Minimum per Gauss 1 px suchen, Schwelle am ungeglaetteten L*
    dunkel = (L < (Lmin + Lh0) / 2) & (rr <= fenster)
    lb, _ = nd.label(dunkel)
    k = lb[py, px]
    m = convex_hull_image(lb == k)
    ys, xs = np.nonzero(m)
    b, h = xs.max() - xs.min() + 1, ys.max() - ys.min() + 1
    lid = nd.binary_dilation(m, iterations=2) & ~m & alp
    haut = nd.binary_dilation(m, iterations=7) & ~nd.binary_dilation(m, iterations=4) & alp   # Ring 5-7 px
    Lh = float(L[haut].mean())
    La = L[m]
    q = np.sort(La); pup = q[:max(1, len(q) // 4)].mean()
    hell = m & (L > Lh)
    g = None
    if hell.any():                                # Kern = Schwerpunkt der Augenpunkte heller als die Haut
        gy_, gx_ = np.nonzero(hell)
        g = [round((gx_.mean() - xs.min() + 0.5) / b, 2), round((gy_.mean() - ys.min() + 0.5) / h, 2)]
        gy, gx = np.unravel_index(np.argmax(np.where(m, L, -1)), L.shape)
        g += [round((gx - xs.min() + 0.5) / b, 2), round((gy - ys.min() + 0.5) / h, 2)]
    return dict(mitte=[float(xs.mean()), float(ys.mean())], b_H=rd(b / H), h_H=rd(h / H), bh=rd(b / h, 2),
                aequiv_H=rd(np.sqrt(4 * m.sum() / np.pi) / H), L_auge_haut=rd(La.mean() / Lh, 3), L_pup_haut=rd(pup / Lh, 3),
                lid_hell=rd(float((L[lid] > Lh + 15).mean()), 3), L_lid_haut=rd(float(L[lid].mean()) / Lh, 3),
                anteil_L80=rd(float((La > 80).mean()), 3), anteil_heller_haut=rd(float(hell.sum() / m.sum()), 3), glanz_box=g,
                L_haut=rd(Lh, 1), L_min=rd(Lmin, 1), n_px=int(m.sum()))


def flecken(L, sel, alp, H):
    """Dunkle kompakte Flecken in sel: L* < 0,6 x lokaler Median (Fenster 0,03 H), Flaeche >= (0,004 H)^2."""
    k = sel & (alp > 0)
    med0 = float(np.median(L[k]))
    q = np.clip(np.where(k, L, med0) * 2.5, 0, 255).astype(np.uint8)
    med = cv2.medianBlur(q, min(255, int(0.03 * H) | 1)).astype(float) / 2.5
    d = k & (L < 0.6 * med)
    d = nd.binary_opening(d, iterations=2)
    lb, n = nd.label(d)
    out = []
    for i in range(1, n + 1):
        f = lb == i
        if f.sum() < (0.004 * H) ** 2:
            continue
        ys, xs = np.nonzero(f)
        out.append(dict(n=int(f.sum()), mitte=[float(xs.mean()), float(ys.mean())], b_H=rd((np.ptp(xs) + 1) / H),
                        h_H=rd((np.ptp(ys) + 1) / H), L=rd(L[f].mean(), 1), L_med=rd(med[f].mean(), 1)))
    return out


def maul(rgb, kn, alp, auge_px):
    A = LAB(rgb); L = A[..., 0]
    k9 = (kn == 9) & (alp > 0)
    ys9, xs9 = np.nonzero(k9)
    ax, ay = auge_px
    x0, x1 = int(xs9.min()), int(xs9.max()) + 1
    y0, y1 = int(ay + 0.025 * HP), int(ay + 0.10 * HP)
    sub = L[y0:y1, x0:x1]; As = A[y0:y1, x0:x1]
    Lm = float(np.median(sub))
    iy, ix = np.unravel_index(np.argmin(nd.gaussian_filter(sub, 3)), sub.shape)
    lb, _ = nd.label(sub < 0.5 * Lm)
    m = nd.binary_fill_holes(lb == lb[iy, ix])
    z = (sub > Lm + 15) & nd.binary_dilation(m, iterations=4) & ~m
    zl, zn = nd.label(z, structure=np.ones((3, 3)))
    u = m | z
    ys, xs = np.nonzero(u)
    my = (ys.min() + ys.max()) / 2
    breiten = [int(np.ptp(np.nonzero(zl == i)[1]) + 1) for i in range(1, zn + 1)]
    oben = sum(1 for i in range(1, zn + 1) if np.nonzero(zl == i)[0].mean() < my)
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay)
    ring = (rr >= 0.0105 * HP) & (rr <= 0.0158 * HP) & k9
    lh = A[ring].mean(0)
    li = As[m & (sub < 0.5 * Lm)].mean(0)
    lz = As[z].mean(0) if z.any() else None
    b, h = np.ptp(xs) + 1, np.ptp(ys) + 1
    # Kopfbreite in der Maulmitte-Zeile
    zeile = np.nonzero(k9[int(my + y0)] | ((kn[int(my + y0)] == 9)))[0]
    zeile = zeile if len(zeile) else np.array([0, 0])
    return dict(b_H=rd(b / HP), h_H=rd(h / HP), bh=rd(b / h, 2), unter_auge_H=rd(((ys.min() + ys.max()) / 2 + y0 - ay) / HP),
                versatz_H=rd(abs((xs.min() + xs.max()) / 2 + x0 - ax) / HP), lab_innen=[rd(v, 1) for v in li],
                lab_haut_auge=[rd(v, 1) for v in lh], L_innen_haut=rd(li[0] / lh[0], 3), da=rd(li[1] - lh[1], 1), zahn_teile=int(zn),
                oben=int(oben), unten=int(zn - oben), zahn_b_max_H=rd(max(breiten, default=0) / HP),
                lab_zahn=None if lz is None else [rd(v, 1) for v in lz],
                kopfbreite_zeile_H=rd((np.ptp(zeile) + 1) / HP), maul_kopfbreite=rd(b / (np.ptp(zeile) + 1), 2),
                L_fenster_median=rd(Lm, 1), dunkel_px=int(m.sum()))


# ---------------------------------------------------------------- Schwanz (Messblatt 2-5 / R15-R27)
def mittellinie(mf, Q, H):
    cost = np.where(mf, 1.0, np.inf)
    mcp = MCP_Geometric(cost)
    starts = list(zip(*np.nonzero(Q)))
    dist, _ = mcp.find_costs(starts)
    dist[~mf] = np.inf
    dmax = np.max(dist[np.isfinite(dist)])
    P = []
    for b0 in np.arange(0, dmax, 2.0):
        sel = (dist >= b0) & (dist < b0 + 2.0)
        if sel.sum() == 0:
            continue
        ys, xs = np.nonzero(sel); P.append([xs.mean(), ys.mean()])
    P = np.array(P, float)
    s = max(0.01 * H / 2.0, 0.5)             # Gauss 0,01 H in Bandschritten (2 px)
    P = np.stack([nd.gaussian_filter1d(P[:, 0], s, mode='nearest'), nd.gaussian_filter1d(P[:, 1], s, mode='nearest')], 1)
    d = np.r_[0, np.cumsum(np.hypot(*np.diff(P, axis=0).T))]
    t = np.arange(0, d[-1], 1.0)
    return np.stack([np.interp(t, d, P[:, 0]), np.interp(t, d, P[:, 1])], 1)


def sehne(C, i, w):
    a, b = C[max(0, i - w)], C[min(len(C) - 1, i + w)]
    v = b - a; n = np.linalg.norm(v)
    return v / n if n > 0 else np.array([1.0, 0])


def dicke(mf, C, i, H):
    t = sehne(C, i, max(1, int(round(0.01 * H))))
    nrm = np.array([-t[1], t[0]])

    def lauf(sg):
        r = 0.0
        while True:
            p = C[i] + sg * nrm * (r + 0.25)
            x, y = int(round(p[0])), int(round(p[1]))
            if not (0 <= y < mf.shape[0] and 0 <= x < mf.shape[1]) or not mf[y, x]:
                return r
            r += 0.25
    return lauf(1) + lauf(-1)


def form(mf, C, H):
    n = len(C) - 1
    w = max(1, int(round(0.01 * H)))
    th = np.unwrap(np.array([np.arctan2(*sehne(C, i, w)[::-1]) for i in range(len(C))]))
    h = int(round(0.025 * H))
    dth = np.full(len(C), np.nan)
    for i in range(h, len(C) - h):
        dth[i] = abs(np.degrees(th[i + h] - th[i - h]))
    rand = int(round(0.025 * H))
    gueltig = np.zeros(len(C), bool); gueltig[rand:len(C) - rand] = True
    ueber = gueltig & (np.nan_to_num(dth) > 20)
    lb, k = nd.label(ueber)
    seg = [np.nonzero(lb == j)[0] for j in range(1, k + 1)]
    merged = []
    for s_ in seg:
        if merged and s_[0] - merged[-1][-1] < 0.03 * H:
            merged[-1] = np.r_[merged[-1], s_]
        else:
            merged.append(s_)
    dk = {str(f): rd(dicke(mf, C, int(round(f * n)), H) / H, 3) for f in (0.1, 0.2, 0.3, 0.5, 0.7, 0.8, 0.9)}
    br = [rd(dicke(mf, C, int(round(f * n)), H) / H, 3) for f in np.arange(0.05, 0.951, 0.05)]
    teil = br[3:14]
    anstieg = max(0.0, max(teil[i + 1] - teil[i] for i in range(len(teil) - 1)))
    gesamt = abs(np.degrees(th[-1] - th[0]))
    return dict(dicke_H=dk, breiten=br, anstieg_02_07=rd(anstieg, 3), max_dwinkel=rd(np.nanmax(dth[gueltig]), 1),
                knicke=len(merged), knick_s=[rd(np.mean(m_) / n, 2) for m_ in merged], laenge_H=rd(n / H, 3), drehung=rd(gesamt, 0))


def farbe(im, ids, H, OK):
    A = LAB(im)
    tail = nd.binary_erosion(np.isin(ids, SW), iterations=2)
    body = (ids >= 0) & ~np.isin(ids, SW)
    band = np.zeros_like(body); band[OK:int(OK + 0.45 * H) + 1] = True
    haut = nd.binary_erosion(body & band, iterations=2) & (A[..., 2] >= -3) & (A[..., 0] >= 5)
    ls, lh = A[tail].mean(0), A[haut].mean(0)
    cs, chh = np.hypot(ls[1], ls[2]), np.hypot(lh[1], lh[2])
    hs, hh = np.degrees(np.arctan2(ls[2], ls[1])), np.degrees(np.arctan2(lh[2], lh[1]))
    return dict(lab_schwanz=[rd(v, 1) for v in ls], lab_haut=[rd(v, 1) for v in lh], L_verh=rd(ls[0] / lh[0], 3), C_verh=rd(cs / chh, 3),
                dh=rd(hs - hh, 1), rot_s=rd((A[tail][:, 1] > 15).mean(), 3), rot_h=rd((A[haut][:, 1] > 15).mean(), 3),
                de00=rd(deltaE_ciede2000(ls[None], lh[None])[0], 2), n_tail=int(tail.sum()), n_haut=int(haut.sum()),
                rgb_schwanz=[rd(v, 0) for v in im[tail].mean(0)], rgb_haut=[rd(v, 0) for v in im[haut].mean(0)])


def muster(imf, mf, C, H):
    A = LAB(imf); a = A[..., 1]
    m2 = nd.binary_erosion(mf, iterations=1)
    prof = []
    w = max(1, int(round(0.01 * H)))
    for i in range(len(C)):
        t = sehne(C, i, w); nrm = np.array([-t[1], t[0]])
        v = []
        for r in np.arange(-0.2 * H, 0.2 * H, 0.5):
            p = C[i] + nrm * r; x, y = int(round(p[0])), int(round(p[1]))
            if 0 <= y < a.shape[0] and 0 <= x < a.shape[1] and m2[y, x]:
                v.append(a[y, x])
        prof.append(np.mean(v) if v else np.nan)
    p = np.array(prof); ok = np.isfinite(p); p = np.interp(np.arange(len(p)), np.nonzero(ok)[0], p[ok])
    tr = nd.uniform_filter1d(p, max(3, int(round(0.05 * H))), mode='nearest')
    r = p - tr
    rs = r - r.mean()
    ak = np.correlate(rs, rs, 'full')[len(rs) - 1:]; ak = ak / ak[0]
    lo, hi = int(round(0.03 * H)), int(round(0.15 * H))
    best = None
    for k in range(max(lo, 1), min(hi, len(ak) - 1)):
        if ak[k] > ak[k - 1] and ak[k] >= ak[k + 1]:
            if best is None or ak[k] > best[1]:
                best = (k, ak[k])
    return dict(rest_std_a=rd(rs.std(), 2), ak_max=None if best is None else rd(best[1], 2),
                ak_lag_H=None if best is None else rd(best[0] / H, 3))


def schwanz(d, h):
    im, ids, imf, idf, j = d['s%s' % h], d['s%s_ids' % h], d['s%s_f' % h], d['s%s_fids' % h], d['s%s_j' % h]
    body = (ids >= 0) & ~np.isin(ids, SW)
    ys = np.nonzero(body.any(1))[0]; OK, so = int(ys.min()), int(ys.max()); H = so - OK + 1
    mf = idf >= 0
    lb, k = nd.label(mf); mf = lb == 1 + int(np.argmax(nd.sum(mf, lb, range(1, k + 1))))
    yy, xx = np.mgrid[:mf.shape[0], :mf.shape[1]]
    Q = mf & (np.hypot(xx - j[0, 0], yy - j[0, 1]) <= max(2.0, 0.02 * H))
    if not Q.any():
        dd = np.hypot(xx - j[0, 0], yy - j[0, 1]); dd[~mf] = 1e9; Q = np.zeros_like(mf); Q[np.unravel_index(np.argmin(dd), dd.shape)] = True
    C = mittellinie(mf, Q, H)
    vis = np.isin(ids, SW)
    s0 = 0
    for i in range(len(C) - 3):
        if all(vis[int(round(C[q, 1])), int(round(C[q, 0]))] for q in range(i, i + 3)):
            s0 = i; break
    Cs = C[s0:]
    out = dict(H=H, OK=OK, s0=s0, austritt_H=rd((Cs[0][1] - OK) / H, 3), form=form(mf, Cs, H), farbe=farbe(im, ids, H, OK),
               muster=muster(imf, mf, Cs, H))
    n = len(Cs) - 1; i = max(0, n - int(round(0.05 * H)))
    out['dicke_005_vor_spitze_H'] = rd(dicke(mf, Cs, i, H) / H, 3)
    out['_C'] = Cs.tolist()
    return out


if __name__ == '__main__':
    name = sys.argv[1]
    d = dict(np.load(OUT + 'r_%s.npz' % name))
    erg = {}
    b, alp, kn = d['nah'], d['nah_alp'] > 0, d['nah_kn']
    A = LAB(b); L = A[..., 0]
    fl = flecken(L, kn == 9, alp, HP)
    erg['kopf_flecken_mesh9'] = fl
    erg['flecken_mesh8'] = flecken(L, kn == 8, alp, HP)
    fl_auge = [f for f in fl if f['b_H'] <= 0.04 and f['h_H'] <= 0.03]
    if fl_auge:
        fa = min(fl_auge, key=lambda f: f['mitte'][1])      # oberster kompakter Fleck = Kopfauge (Maulteile liegen tiefer)
        erg['augen_ueber_maul'] = [f for f in fl_auge if f['mitte'][1] < fa['mitte'][1] + 0.025 * HP]
        s = H5 / HP
        W5, H5b = int(round(b.shape[1] * s)), int(round(b.shape[0] * s))
        b5 = cv2.resize(b, (W5, H5b), interpolation=cv2.INTER_AREA)
        a5 = cv2.resize(alp.astype(np.float32), (W5, H5b), interpolation=cv2.INTER_AREA) > 0.5
        ka = auge(b5, a5, (fa['mitte'][0] * s, fa['mitte'][1] * s), H5)
        erg['kopfauge'] = ka
        k8 = (cv2.resize((kn == 8).astype(np.uint8), (W5, H5b), interpolation=cv2.INTER_NEAREST) > 0) & a5
        L5 = LAB(b5)[..., 0]
        ex, ey = ka['mitte']
        reg = np.zeros_like(k8); reg[int(ey):int(ey + 0.10 * H5), int(ex - 0.16 * H5):int(ex - 0.07 * H5)] = True
        cand = np.where(reg & k8, nd.gaussian_filter(L5, 1.5), 1e9)
        sy_, sx_ = np.unravel_index(np.argmin(cand), cand.shape)
        erg['schulter_dunkelster_fleck_suchbereich'] = auge(b5, a5, (sx_, sy_), H5)
        # Schulterauge: dunkelster Punkt im Fenster +-12 px um Kopfauge + (-0,118, +0,0545) H (Messblatt K12 Mitte)
        sa = auge(b5, a5, (ex - 0.118 * H5, ey + 0.0545 * H5), H5)
        erg['schulterauge'] = sa
        erg['R2'] = dict(dx_H=rd((sa['mitte'][0] - ka['mitte'][0]) / H5), dy_H=rd((sa['mitte'][1] - ka['mitte'][1]) / H5),
                         aequiv_verh=rd(sa['aequiv_H'] / ka['aequiv_H'], 2))
        erg['maul'] = maul(b.astype(float), kn, alp, fa['mitte'])
        erg['auge_px_voll'] = fa['mitte']
    for h in ('85', '92', '83'):
        erg['schwanz_h%s' % h] = schwanz(d, h)
    for h in ('118', '125'):
        erg['vorn_h%s_schwanz_px' % h] = int(np.isin(d['v%s_ids' % h], SW).sum())
    json.dump(erg, open(OUT + 'm_%s.json' % name, 'w'), indent=1)
    for k, v in erg.items():
        if k.startswith('schwanz'):
            v = {kk: vv for kk, vv in v.items() if kk != '_C'}
        print(k, json.dumps(v))
