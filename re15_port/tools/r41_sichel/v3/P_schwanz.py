# Pruefer V3: Schwanzmessung (Form, Glaette, Farbe) - ein Verfahren fuer KI-Freisteller UND Port-Render.
# Eigene Rechnung, keine Leser-Skripte. Aufruf: python P_schwanz.py  -> sfv3/P/P_schwanz.json + Belegbilder.
#
# Verfahren (Riegel-Messmethode, gilt fuer KI und Port gleich):
#  Wurzel-Quellmenge Q: KI = Bildpunkte der Strecke Kerbe -> Naht (Kerbe = hoechster Hintergrundpunkt der Luecke unter
#    dem Bogen; Naht = Uebergang dunkle Schwanzhaut -> helle Rueckenhaut an der Oberkontur, siehe kerbe_schnitt); Port = Schwanz-Bildpunkte (Knochen 16-22), die an einen
#    Koerper-Bildpunkt (andere Knochen) grenzen (= Austritt aus dem Umriss).
#  Mittellinie: geodaetischer Abstand d von Q in der Schwanzmaske (8er-Nachbarschaft); Schwerpunkt je 2-px-Band von d;
#    Gauss-Glaettung sigma = 0,01 H; Neuabtastung je 1 px Bogenlaenge. Spitze = groesstes d.
#  Dicke(s): Laenge des zusammenhaengenden Maskenstuecks auf der Normalen durch den Mittellinienpunkt bei s (Anteil der
#    Mittellinienlaenge), in H.
#  Tangente: Sehne +-0,01 H; Winkelaenderung je 0,05 H = |theta(s+0,025H) - theta(s-0,025H)|; Knick = zusammenhaengender
#    Abschnitt mit Aenderung > 20 Grad (Abschnitte < 0,03 H Abstand verschmolzen), Wurzel-/Spitzenrand 0,025 H ausgenommen.
#  Farbe: Maske 2 px erodiert; Haut = Figur ohne Schwanz im Band Oberkante .. +0,45 H, 2 px erodiert, ohne Jeans (b* < -3)
#    und ohne Bildpunkte L* < 5 (Spalten/Schwarz). Zusaetzlich (nur Vergleich) Haut "Ruecken" = Band OK..+0,15 H.
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from P_farbe import lab, de00, de76, ch, r1

OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/P/'
V = 'C:/workspace/git/reAi_v2/Version_80%/'


def linie(p, q):
    n = int(max(abs(q[0] - p[0]), abs(q[1] - p[1]))) + 1
    return np.stack([np.round(np.linspace(p[0], q[0], n)), np.round(np.linspace(p[1], q[1], n))], 1).astype(int)


def kerbe_schnitt(fg, such, L=None):
    """such = (x0, y0, x1, y1): Bereich der Kerbe. Kerbe = hoechster Hintergrundpunkt mit Figur darueber, links und rechts.
    Naht (oberer Schnittpunkt) = von der Buckelmitte aus nach hinten (Bild-links) die erste Spalte der Oberkontur, in der das
    mittlere L* 4-14 px unter der Kante (3 Spalten gemittelt) nach einem hellen Stueck (>= 40) unter 35 faellt (dunkle Schwanzhaut gegen helle Rueckenhaut).
    Die kuerzeste Strecke Kerbe -> Oberkontur laeuft quer durch den Schwanz selbst (geprueft) und taugt nicht."""
    x0, y0, x1, y1 = such
    best = None
    for x in range(x0, x1):
        col = fg[:, x]
        for y in range(y0, y1):
            if not col[y] and col[:y].any() and fg[y, :x].any() and fg[y, x + 1:].any():
                if best is None or y < best[1]:
                    best = (x, y)
                break
    kx, ky = best
    OK = int(np.nonzero(fg.any(1))[0][0])
    xb = int(round(np.nonzero(fg[OK:OK + 3].any(0))[0].mean()))
    band = []
    for x in range(fg.shape[1]):
        if fg[:, x].any():
            yt = int(np.argmax(fg[:, x])); band.append((x, yt, float(L[yt + 4:yt + 15, x].mean())))
        else:
            band.append((x, -1, np.nan))
    naht = None; hell = False
    for x in range(xb, kx - 40, -1):
        v = np.nanmean([band[x + d][2] for d in (-1, 0, 1)])
        if v >= 40:
            hell = True
        elif hell and v < 35:
            naht = (x, band[x][1]); break
    o = np.array(naht)
    return np.array([kx, ky - 1]), o, float(np.hypot(o[0] - kx, o[1] - ky))


def mittellinie(m, Q, H):
    """m Schwanzmaske, Q bool-Maske der Quellpunkte (in m)."""
    INF = 1e9
    d = np.full(m.shape, INF); d[Q & m] = 0
    # Dijkstra-light: wiederholte Chamfer-Relaxation (8er), reicht fuer schlanke Formen
    from collections import deque
    import heapq
    h = [(0.0, int(y), int(x)) for y, x in np.argwhere(Q & m)]
    heapq.heapify(h)
    st = [(-1, -1, 1.4142), (-1, 0, 1), (-1, 1, 1.4142), (0, -1, 1), (0, 1, 1), (1, -1, 1.4142), (1, 0, 1), (1, 1, 1.4142)]
    while h:
        dd, y, x = heapq.heappop(h)
        if dd > d[y, x]:
            continue
        for dy, dx, w in st:
            yy, xx = y + dy, x + dx
            if 0 <= yy < m.shape[0] and 0 <= xx < m.shape[1] and m[yy, xx] and dd + w < d[yy, xx]:
                d[yy, xx] = dd + w; heapq.heappush(h, (dd + w, yy, xx))
    ok = m & (d < INF)
    dmax = d[ok].max()
    pts = []
    for b in np.arange(0, dmax, 2.0):
        sel = ok & (d >= b) & (d < b + 2)
        if sel.sum():
            ys, xs = np.nonzero(sel); pts.append((xs.mean(), ys.mean()))
    P = np.array(pts)
    sig = 0.01 * H
    # Glaettung entlang der Bogenlaenge
    seg = np.r_[0, np.cumsum(np.hypot(*np.diff(P, axis=0).T))]
    s = np.arange(0, seg[-1], 1.0)
    X = np.interp(s, seg, P[:, 0]); Y = np.interp(s, seg, P[:, 1])
    X = nd.gaussian_filter1d(X, sig, mode='nearest'); Y = nd.gaussian_filter1d(Y, sig, mode='nearest')
    # Endpunkte festhalten (Glaettung zieht die Enden nach innen)
    X[0], Y[0], X[-1], Y[-1] = P[0, 0], P[0, 1], P[-1, 0], P[-1, 1]
    seg = np.r_[0, np.cumsum(np.hypot(np.diff(X), np.diff(Y)))]
    s = np.arange(0, seg[-1], 1.0)
    return np.stack([np.interp(s, seg, X), np.interp(s, seg, Y)], 1), d


def dicke(m, C, i):
    n = len(C); a, b = max(i - 3, 0), min(i + 3, n - 1)
    t = C[b] - C[a]; t = t / (np.linalg.norm(t) + 1e-9); nrm = np.array([-t[1], t[0]])
    p = C[i]; L = 0
    for sgn in (1, -1):
        k = 0.0
        while True:
            q = p + sgn * (k + 0.5) * nrm
            x, y = int(round(q[0])), int(round(q[1]))
            if not (0 <= y < m.shape[0] and 0 <= x < m.shape[1]) or not m[y, x]:
                break
            k += 0.5
        L += k
    return L, nrm


def form(m, C, H):
    n = len(C); Lc = n - 1
    prof = {}
    for f in (0.1, 0.2, 0.3, 0.5, 0.7, 0.8, 0.9):
        i = int(round(f * Lc)); prof[f] = round(dicke(m, C, i)[0] / H, 3)
    prof_spitze = {}
    for dd in (0.05, 0.1, 0.2, 0.3, 0.4):
        i = int(round(Lc - dd * H))
        if i > 0:
            prof_spitze[dd] = round(dicke(m, C, i)[0] / H, 3)
    breiten = [round(dicke(m, C, int(round(f * Lc)))[0] / H, 3) for f in np.linspace(0.05, 0.95, 19)]
    w = max(1, int(round(0.01 * H)))
    th = np.full(n, np.nan)
    for i in range(n):
        a, b = max(i - w, 0), min(i + w, n - 1)
        th[i] = np.degrees(np.arctan2(C[b, 1] - C[a, 1], C[b, 0] - C[a, 0]))
    th = np.degrees(np.unwrap(np.radians(th)))
    h = int(round(0.025 * H))
    dth = np.full(n, 0.0)
    for i in range(h, n - h):
        dth[i] = abs(th[i + h] - th[i - h])
    rand = h
    hi = dth > 20
    hi[:rand] = False; hi[n - rand:] = False
    lb, k = nd.label(hi)
    knicke = []
    for j in range(1, k + 1):
        idx = np.nonzero(lb == j)[0]
        knicke.append([int(idx[0]), int(idx[-1]), int(idx[np.argmax(dth[idx])])])
    merged = []
    for kn in knicke:
        if merged and kn[0] - merged[-1][1] < 0.03 * H:
            a = merged[-1]; a[1] = kn[1]; a[2] = a[2] if dth[a[2]] >= dth[kn[2]] else kn[2]
        else:
            merged.append(kn)
    ii = np.arange(rand, n - rand)
    imax = int(ii[np.argmax(dth[ii])])
    return dict(laenge_H=round(Lc / H, 3), dicke_H=prof, dicke_ab_spitze_H=prof_spitze, breiten_005_095=breiten, max_dwinkel_005H=round(float(dth[imax]), 1), max_bei=round(imax / Lc, 2),
                knicke=len(merged), knick_bei=[round(kn[2] / Lc, 2) for kn in merged], knick_winkel=[round(float(dth[kn[2]]), 1) for kn in merged],
                gesamtdrehung=round(float(abs(th[-1 - w] - th[w])), 1),
                winkel_profil=[round(float(th[int(f * Lc)]), 0) for f in np.linspace(0.02, 0.98, 13)],
                austritt_grad=round(float(np.degrees(np.arctan2(C[int(0.05 * H), 1] - C[0, 1], C[int(0.05 * H), 0] - C[0, 0]))), 1)), th, dth


def farbe(rgb, m_schwanz, fg, OK, H, ausschluss=None):
    A = lab(rgb)
    ms = nd.binary_erosion(m_schwanz, iterations=2)
    band = np.zeros(fg.shape, bool); band[OK:int(OK + 0.45 * H)] = True
    koerper = fg & ~nd.binary_dilation(m_schwanz, iterations=2)
    if ausschluss is not None:
        koerper &= ~ausschluss
    haut = nd.binary_erosion(koerper, iterations=2) & band & (A[..., 2] >= -3) & (A[..., 0] >= 5)
    rue = np.zeros(fg.shape, bool); rue[OK:int(OK + 0.15 * H)] = True
    ruecken = haut & rue
    ls, lh, lr = A[ms].mean(0), A[haut].mean(0), A[ruecken].mean(0)
    cs, hs = ch(ls); chh, hh = ch(lh)
    d = dict(lab_schwanz=r1(ls), rgb_schwanz=r1(rgb[ms].mean(0), 0), lab_haut=r1(lh), rgb_haut=r1(rgb[haut].mean(0), 0),
             lab_ruecken=r1(lr), n_schwanz=int(ms.sum()), n_haut=int(haut.sum()),
             de00_haut=round(de00(ls, lh), 1), de76_haut=round(de76(ls, lh), 1),
             de00_ruecken=round(de00(ls, lr), 1), de76_ruecken=round(de76(ls, lr), 1),
             L_verh=round(float(ls[0] / lh[0]), 3), C_schwanz=round(cs, 1), C_haut=round(chh, 1), C_verh=round(cs / chh, 2),
             h_schwanz=round(hs, 1), h_haut=round(hh, 1), dh=round(hs - hh, 1),
             rot_anteil_schwanz=round(float((A[ms][:, 1] > 15).mean()), 3), rot_anteil_haut=round(float((A[haut][:, 1] > 15).mean()), 3))
    return d, ms, haut


def muster(rgb, m, C, H):
    """Querbaender: a*/L* entlang der Mittellinie (Mittel ueber +-1/3 der Dicke), Trend (Gauss 0,05 H) abgezogen,
    Autokorrelation: groesster Wert bei Lag 0,02-0,15 H; periode_* = hoechstes ECHTES lokales Maximum der Autokorrelation
    bei Lag 0,03-0,15 H (Querbaender mit Periode -> Wert hoch); Rest-Std a* und L*."""
    A = lab(rgb); n = len(C)
    va, vl = [], []
    for i in range(0, n):
        L_, nrm = dicke(m, C, i)
        r = max(1.0, L_ / 3)
        ks = np.arange(-r, r + 0.01, 1.0)
        q = C[i][None, :] + ks[:, None] * nrm[None, :]
        xs, ys = np.clip(np.round(q[:, 0]).astype(int), 0, m.shape[1] - 1), np.clip(np.round(q[:, 1]).astype(int), 0, m.shape[0] - 1)
        ok = m[ys, xs]
        va.append(A[ys[ok], xs[ok], 1].mean() if ok.any() else np.nan); vl.append(A[ys[ok], xs[ok], 0].mean() if ok.any() else np.nan)
    out = {}
    for nm, v in (('a', np.array(va)), ('L', np.array(vl))):
        v = v[int(0.05 * n):int(0.95 * n)]
        v = v[~np.isnan(v)]
        rest = v - nd.gaussian_filter1d(v, 0.05 * H, mode='nearest')
        rest = rest - rest.mean()
        ac = np.correlate(rest, rest, 'full')[len(rest) - 1:]; ac = ac / (ac[0] + 1e-9)
        lo, hi = max(2, int(0.02 * H)), min(len(ac) - 1, int(0.15 * H))
        j = lo + int(np.argmax(ac[lo:hi]))
        out['ak_%s_max' % nm] = round(float(ac[j]), 2); out['ak_%s_lag_H' % nm] = round(j / H, 3); out['rest_std_%s' % nm] = round(float(rest.std()), 2)
        lo3 = max(2, int(round(0.03 * H)))
        lm = [(float(ac[q]), q) for q in range(lo3, hi) if ac[q] > ac[q - 1] and ac[q] >= ac[q + 1]]
        best = max(lm) if lm else (0.0, 0)
        out['periode_%s_ak' % nm] = round(best[0], 2); out['periode_%s_H' % nm] = round(best[1] / H, 3)
    return out


def ki_bild(name, H, such, seed):
    im = cv2.imread(V + name + '.png', cv2.IMREAD_UNCHANGED)
    rgb = im[:, :, 2::-1].astype(float); fg = im[:, :, 3] > 128
    OK = int(np.nonzero(fg.any(1))[0][0])
    k, o, dl = kerbe_schnitt(fg, such, lab(rgb)[..., 0])
    cut = linie(k, o)
    fg2 = fg.copy()
    for dx, dy in ((0, 0), (1, 0), (0, 1)):
        fg2[np.clip(cut[:, 1] + dy, 0, fg.shape[0] - 1), np.clip(cut[:, 0] + dx, 0, fg.shape[1] - 1)] = False
    lb, _ = nd.label(fg2)
    m = lb == lb[seed[1], seed[0]]
    Q = np.zeros(fg.shape, bool)
    for dx, dy in ((-1, 0), (0, -1), (-1, -1), (-2, 0), (0, -2)):
        Q[np.clip(cut[:, 1] + dy, 0, fg.shape[0] - 1), np.clip(cut[:, 0] + dx, 0, fg.shape[1] - 1)] = True
    Q &= m
    C, d = mittellinie(m, Q, H)
    fo, th, dth = form(m, C, H)
    fa, ms, haut = farbe(rgb, m, fg, OK, H)
    mu = muster(rgb, m, C, H)
    buckel_x = float(np.nonzero(fg[OK])[0].mean())
    mitte = (k + o) / 2.0
    wz = dict(kerbe=k.tolist(), oben=o.tolist(), schnitt_H=round(dl / H, 3), mitte_unter_OK_H=round((mitte[1] - OK) / H, 3),
              hinter_buckel_H=round((buckel_x - mitte[0]) / H, 3), kerbe_unter_OK_H=round((k[1] - OK) / H, 3),
              oben_unter_OK_H=round((o[1] - OK) / H, 3), buckel_x=round(buckel_x, 1))
    # Oberarm-Vergleich wird nicht automatisch gemessen
    vis = np.ascontiguousarray(im[:, :, 2::-1].copy())
    vis[~fg] = (60, 110, 60)
    c, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(vis, c, -1, (255, 0, 255), 1)
    c, _ = cv2.findContours(haut.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(vis, c, -1, (0, 200, 255), 1)
    cv2.line(vis, tuple(map(int, k)), tuple(map(int, o)), (0, 255, 255), 2)
    cv2.polylines(vis, [C.astype(np.int32)], False, (255, 255, 0), 1)
    Lc = len(C) - 1
    for f in (0.1, 0.2, 0.3, 0.5, 0.7, 0.9):
        i = int(round(f * Lc)); L_, nrm = dicke(m, C, i); p = C[i]
        a = p - nrm * L_ / 2; b = p + nrm * L_ / 2
        cv2.line(vis, tuple(map(int, a)), tuple(map(int, b)), (0, 255, 0), 1)
        cv2.putText(vis, '%.1f' % f, tuple(map(int, b)), 0, 0.35, (0, 255, 0), 1)
    for kb in fo['knick_bei']:
        cv2.circle(vis, tuple(map(int, C[int(kb * Lc)])), 6, (255, 128, 0), 2)
    cv2.putText(vis, '%s H%d L%.2fH Knicke %d max %.0f' % (name, H, fo['laenge_H'], fo['knicke'], fo['max_dwinkel_005H']), (4, 14), 0, 0.42, (255, 255, 255), 1)
    cv2.imwrite(OUT + 'P_schwanz_%s.png' % name, cv2.cvtColor(vis, cv2.COLOR_RGB2BGR))
    np.save(OUT + 'P_maske_%s.npy' % name, m); np.save(OUT + 'P_haut_%s.npy' % name, haut)
    return dict(H=H, OK=OK, form=fo, farbe=fa, muster=mu, wurzel=wz)


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    erg = {}
    # H: birkin12 504 (Auftrag), birkin11/13: Alpha-Bbox - 2 wie birkin12 (506 -> 504)
    for name, such, seed in (('birkin12', (100, 60, 230, 200), (20, 250)), ('birkin11', (100, 60, 230, 200), (20, 250)),
                             ('birkin13', (100, 60, 230, 200), (20, 230))):
        im = cv2.imread(V + name + '.png', cv2.IMREAD_UNCHANGED)
        H = 504 if name == 'birkin12' else int(np.nonzero((im[:, :, 3] > 128).any(1))[0].ptp()) + 1 - 2
        erg[name] = ki_bild(name, H, such, seed)
        # Muster zusaetzlich in Port-Aufloesung (H = 265 px wie der Port-Render 640x480): Bild und Maske verkleinert
        f = 265.0 / H
        rgb = cv2.resize(im[:, :, 2::-1], None, fx=f, fy=f, interpolation=cv2.INTER_AREA).astype(float)
        m = np.load(OUT + 'P_maske_%s.npy' % name)
        mk = cv2.resize(m.astype(np.uint8), (rgb.shape[1], rgb.shape[0]), interpolation=cv2.INTER_AREA) > 0
        mk = nd.binary_erosion(mk, iterations=1)
        k, o = np.array(erg[name]['wurzel']['kerbe']) * f, np.array(erg[name]['wurzel']['oben']) * f
        Q = np.zeros(mk.shape, bool); cut = linie(k, o)
        for dx, dy in ((-1, 0), (0, -1), (-1, -1), (-2, 0), (0, -2), (0, 0)):
            Q[np.clip(cut[:, 1] + dy, 0, mk.shape[0] - 1), np.clip(cut[:, 0] + dx, 0, mk.shape[1] - 1)] = True
        Q &= mk
        C2, _ = mittellinie(mk, Q, 265)
        erg[name]['muster_h265'] = muster(rgb, mk, C2, 265)
        erg[name]['form_h265'] = form(mk, C2, 265)[0]
        print(name, json.dumps(erg[name]))
    json.dump(erg, open(OUT + 'P_schwanz_ki.json', 'w'), indent=1)
