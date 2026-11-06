# Leser A V3: Schwanz-Masse S1-S6 an den KI-Freistellern (Masken, Mittellinie, Dicke, Glaette, Muster, Farbe).
# Schwanzmaske KI: Alpha, abgetrennt durch Schnittregeln je Bild, groesste Komponente.
#   Seitenansicht (birkin11/12/13): Wurzelschnitt = Kerbe (oberster Hintergrundpunkt der Luecke zwischen Bogen-Unterseite
#   und Ruecken) -> naechster Punkt der Oberkontur (Formblatt V2 Punkt 5); Schwanz = Seite links (B) des Schnitts, ohne
#   Beine/Sichel (x > xbein und y > ybein).
#   Halteszene (birkin05/06/09): sichtbares Stueck rechts (B) des Standbeins (x >= xmin, y >= ymin); Wurzel verdeckt.
# Farben: Masken 2 px erodiert; Lab D65 (OpenCV, sRGB). Rot = a* > 15 (zusaetzlich r-g > 20 wie Formblatt Punkt 11/18).
import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from scipy import ndimage as ndi
from skimage.morphology import skeletonize

SEITE = {11: dict(xbein=95, ybein=150), 12: dict(xbein=95, ybein=150), 13: dict(xbein=80, ybein=165)}
HALTEN = {5: dict(xmin=330, ymin=380), 6: dict(xmin=380, ymin=290), 9: dict(xmin=318, ymin=295)}


def kerbe(al):
    """oberster Hintergrundpunkt im Fenster x 130-200, y 60-200, der mit dem Hintergrund unten (Luecke) verbunden ist"""
    bg = ~al
    lbl, n = ndi.label(bg)
    best = None
    for x in range(130, 201):
        ys = np.nonzero(bg[60:200, x])[0]
        if len(ys):
            y = ys.min() + 60
            aussen = set(np.unique(np.r_[lbl[0], lbl[-1], lbl[:, 0], lbl[:, -1]])) - {0}
            if lbl[y, x] in aussen and (best is None or y < best[1]):
                best = (x, y)
    return best


def oberkontur_naechst(al, p):
    cand = []
    for x in range(max(p[0] - 80, 0), min(p[0] + 80, al.shape[1])):
        ys = np.nonzero(al[:, x])[0]
        if len(ys):
            cand.append((x, ys.min()))
    cand = np.array(cand)
    d = np.hypot(cand[:, 0] - p[0], cand[:, 1] - p[1])
    return tuple(int(v) for v in cand[np.argmin(d)])


def naht(al, rgb):
    """Naht Schwanz/Rumpf an der Oberkontur: erstes x in [150, 260], ab dem das mittlere L* 4-14 px unter der Oberkante
    ueber 6 Spalten > 40 liegt (Schwanz dunkelgrau L* 20-35, Rumpf hell L* 40-56; Profil siehe Bericht)."""
    L = lab(rgb)[..., 0]
    ale = erode(al, 3)
    for x in range(150, 261):
        v = []
        for xx in range(x, x + 6):
            t = np.nonzero(al[:, xx])[0].min()
            col = L[t + 4:t + 14, xx][ale[t + 4:t + 14, xx]]
            v.extend(col.tolist())
        if v and np.mean(v) > 40:
            return (x, int(np.nonzero(al[:, x])[0].min()))
    return None


def groesste(m):
    lbl, n = ndi.label(m)
    if n == 0:
        return m
    sz = ndi.sum(np.ones_like(lbl), lbl, range(1, n + 1))
    return lbl == (int(np.argmax(sz)) + 1)


def schwanzmaske_ki(nn):
    rgb, al = load(nn)
    H = hbox(al)[4]
    if nn in SEITE:
        k = kerbe(al)
        o_nah = oberkontur_naechst(al, k)
        o = naht(al, rgb)
        yy, xx = np.mgrid[:al.shape[0], :al.shape[1]]
        cr = (o[0] - k[0]) * (yy - k[1]) - (o[1] - k[1]) * (xx - k[0])
        ref = -(o[1] - k[1]) * (0 - k[0])          # Punkt (0, k.y) liegt sicher auf der Schwanzseite
        side = np.sign(cr) == np.sign(ref)
        m = al & side & ~((xx > SEITE[nn]['xbein']) & (yy > SEITE[nn]['ybein']))
        info = dict(kerbe=list(k), naht=list(o), oberkontur_naechst=list(o_nah), schnitt_nah_px=round(float(np.hypot(o_nah[0] - k[0], o_nah[1] - k[1])), 1), schnitt_laenge_px=round(float(np.hypot(o[0] - k[0], o[1] - k[1])), 1))
    else:
        r = HALTEN[nn]
        m = al.copy()
        m[:r['ymin']] = False
        m[:, :r['xmin']] = False
        info = dict(regel=r)
    return rgb, al, groesste(m), H, info


def mittellinie(m, wurzel_hint):
    """Skelett -> laengster Pfad -> Enden entlang der Tangente bis zum Maskenrand verlaengert; Ordnung Wurzel -> Spitze."""
    ms = ndi.binary_opening(ndi.binary_closing(m, iterations=2), iterations=2)
    sk = skeletonize(groesste(ms))
    pts = np.argwhere(sk)
    idx = {tuple(p): i for i, p in enumerate(pts)}
    nb = [[] for _ in pts]
    for i, (y, x) in enumerate(pts):
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if (dy or dx) and (y + dy, x + dx) in idx:
                    nb[i].append(idx[(y + dy, x + dx)])

    def bfs(s):
        dist = {s: 0}
        prev = {s: None}
        q = [s]
        for u in q:
            for v in nb[u]:
                if v not in dist:
                    dist[v] = dist[u] + 1
                    prev[v] = u
                    q.append(v)
        return max(dist, key=dist.get), prev

    a, _ = bfs(0)
    b, prev = bfs(a)
    path = []
    u = b
    while u is not None:
        path.append(pts[u][::-1].astype(float))
        u = prev[u]
    P = np.array(path)
    if np.hypot(*(P[0] - wurzel_hint)) > np.hypot(*(P[-1] - wurzel_hint)):
        P = P[::-1]
    k = 9
    Ps = np.vstack([np.convolve(np.pad(P[:, i], k // 2, mode='edge'), np.ones(k) / k, 'valid') for i in range(2)]).T

    def ext(Pp, end):
        d = Pp[-1] - Pp[-12] if end else Pp[0] - Pp[11]
        d = d / np.linalg.norm(d)
        q = (Pp[-1] if end else Pp[0]).copy()
        add = []
        for _ in range(200):
            q = q + d
            yi, xi = int(round(q[1])), int(round(q[0]))
            if not (0 <= yi < m.shape[0] and 0 <= xi < m.shape[1]) or not m[yi, xi]:
                break
            add.append(q.copy())
        return add

    a0 = ext(Ps, False)[::-1]
    a1 = ext(Ps, True)
    Pe = np.vstack([np.array(a0).reshape(-1, 2), Ps, np.array(a1).reshape(-1, 2)])
    seg = np.r_[0, np.cumsum(np.hypot(*np.diff(Pe, axis=0).T))]
    t = np.arange(0, seg[-1], 1.0)
    return np.vstack([np.interp(t, seg, Pe[:, 0]), np.interp(t, seg, Pe[:, 1])]).T


def breite(m, p, d):
    n = np.array([-d[1], d[0]])
    w = 0
    for sg in (1, -1):
        for k in np.arange(0.5, 200, 0.5):
            q = p + sg * n * k
            yi, xi = int(round(q[1])), int(round(q[0]))
            if not (0 <= yi < m.shape[0] and 0 <= xi < m.shape[1]) or not m[yi, xi]:
                w += k - 0.5
                break
    return w + 1


def form(m, P, H):
    L = len(P)
    h = max(int(round(0.01 * H)), 2)
    ang = []
    for i in range(L):
        a, b = P[max(i - h, 0)], P[min(i + h, L - 1)]
        ang.append(np.degrees(np.arctan2(b[1] - a[1], b[0] - a[0])))
    ang = np.degrees(np.unwrap(np.radians(ang)))
    w05 = max(int(round(0.05 * H)), 2)
    dd = np.abs(ang[w05:] - ang[:-w05]) if L > w05 else np.array([0.0])
    dmax = float(dd.max())
    imax = int(dd.argmax())
    lbl, nk = ndi.label(dd > 20)
    pos = [round(float((np.nonzero(lbl == i)[0].mean() + w05 / 2) / L), 2) for i in range(1, nk + 1)]
    tang = np.gradient(P, axis=0)
    W = np.array([breite(m, P[i], tang[i] / max(np.linalg.norm(tang[i]), 1e-9)) for i in range(L)])
    dicken = {}
    for f in (0.1, 0.2, 0.3, 0.5, 0.7, 0.9):
        i0, i1 = int((f - 0.01) * L), int((f + 0.01) * L) + 1
        dicken[f] = round(float(np.median(W[i0:i1])) / H, 4)
    w5 = np.median(W[int(0.49 * L):int(0.51 * L) + 1])
    start = None
    for i in np.nonzero(W < 0.5 * w5)[0]:
        if (W[i:] < 0.6 * w5).all():
            start = int(i)
            break
    return dict(laenge_px=L, laenge_H=round(L / H, 3), dicke_H=dicken, dicke_px={f: round(v * H, 1) for f, v in dicken.items()},
                winkel_ges=round(float(ang[-1] - ang[0]), 1), max_dwinkel_005H=round(dmax, 1), max_bei=round((imax + w05 / 2) / L, 2),
                knicke_gt20=int(nk), knick_pos=pos, verjuengung_ab=(round(start / L, 2) if start is not None else None),
                verjuengt_laenge_H=(round((L - start) / H, 3) if start is not None else None),
                breiten_profil=[round(float(W[int(f * (L - 1))]), 1) for f in np.linspace(0, 1, 21)],
                winkel_profil=[round(float(ang[int(f * (L - 1))]), 1) for f in np.linspace(0, 1, 21)])


def muster(rgb, m, P, H):
    """a* und L* entlang der Mittellinie (Mittel ueber die erodierte Breite) -> Autokorrelation nach Abzug des Trends
       (gleitendes Mittel 31 px) = Querbaender; quer: Haelften links/rechts der Mittellinie -> Laengszonen."""
    L3 = lab(rgb)
    me = erode(m, 2)
    tang = np.gradient(P, axis=0)
    pa, pl, hA, hB = [], [], [], []
    for i in range(len(P)):
        d = tang[i] / max(np.linalg.norm(tang[i]), 1e-9)
        n = np.array([-d[1], d[0]])
        va, vl, s1, s2 = [], [], [], []
        for k in np.arange(-80, 80.5, 1.0):
            q = P[i] + n * k
            yi, xi = int(round(q[1])), int(round(q[0]))
            if 0 <= yi < m.shape[0] and 0 <= xi < m.shape[1] and me[yi, xi]:
                va.append(L3[yi, xi, 1])
                vl.append(L3[yi, xi, 0])
                (s1 if k < 0 else s2).append(L3[yi, xi, 1])
        if va:
            pa.append(np.mean(va)); pl.append(np.mean(vl))
            hA.append(np.mean(s1) if s1 else np.nan); hB.append(np.mean(s2) if s2 else np.nan)
    pa, pl = np.array(pa), np.array(pl)

    def ak(x):
        x = x - np.convolve(np.pad(x, 15, mode='edge'), np.ones(31) / 31, 'valid')
        x = x - x.mean()
        c = np.correlate(x, x, 'full')[len(x) - 1:]
        c = c / max(c[0], 1e-9)
        lo = max(int(0.02 * H), 3)
        seg = c[lo:len(x) // 2]
        if len(seg) == 0:
            return None, None, round(float(np.std(x)), 2)
        j = int(np.argmax(seg)) + lo
        return round(float(c[j]), 2), j, round(float(np.std(x)), 2)

    ra, la, sa = ak(pa)
    rl, ll, sl_ = ak(pl)
    return dict(ak_a_max=ra, ak_a_lag_px=la, ak_a_lag_H=(round(la / H, 3) if la else None), a_rest_std=sa,
                ak_L_max=rl, ak_L_lag_px=ll, L_rest_std=sl_,
                a_haelfte_neg=round(float(np.nanmean(hA)), 1), a_haelfte_pos=round(float(np.nanmean(hB)), 1),
                a_profil_21=[round(float(pa[int(f * (len(pa) - 1))]), 1) for f in np.linspace(0, 1, 21)],
                L_profil_21=[round(float(pl[int(f * (len(pl) - 1))]), 1) for f in np.linspace(0, 1, 21)])


def farben(rgb, m, extra):
    L3 = lab(rgb)
    rg = rgb[..., 0].astype(int) - rgb[..., 1].astype(int)
    r = {}
    for k, mm in dict(schwanz=m, **extra).items():
        mme = erode(mm, 2)
        r[k] = stats(rgb, L3, mme)
        if r[k]:
            r[k + '_rot_a15'] = round(float((mme & (L3[..., 1] > 15)).sum() / mme.sum()), 3)
            r[k + '_rot_rg20'] = round(float((mme & (rg > 20)).sum() / mme.sum()), 3)
            r[k + '_hue'] = round(float(np.degrees(np.arctan2(L3[..., 2][mme].mean(), L3[..., 1][mme].mean()))), 1)
            r[k + '_L_p10_p90'] = [round(float(np.percentile(L3[..., 0][mme], 10)), 1), round(float(np.percentile(L3[..., 0][mme], 90)), 1)]
    return r


def koerpermasken(rgb, al, m, H):
    """Haut = Figur ohne Schwanz (4 px Abstand), obere 0,45 H, ohne Jeans (b* < -6, a* < 6)."""
    L3 = lab(rgb)
    top = np.nonzero(al.any(1))[0].min()
    body = al & ~ndi.binary_dilation(m, iterations=4)
    jeans = ndi.binary_opening(body & (L3[..., 2] < -6) & (L3[..., 1] < 6), iterations=1)
    upper = np.zeros_like(al)
    upper[top:top + int(0.45 * H)] = True
    haut = body & upper & ~ndi.binary_dilation(jeans, iterations=3)
    return haut, jeans


def wurzelhinweis(nn, m, info):
    if 'kerbe' in info:
        return np.array(info['kerbe'], float)
    ys, xs = np.nonzero(m)
    i = np.argmin(xs + ys * (1 if nn == 5 else 0.2))
    return np.array([xs[i], ys[i]], float)


if __name__ == '__main__':
    out = {}
    for nn in (11, 12, 13, 5, 6, 9):
        rgb, al, m, H, info = schwanzmaske_ki(nn)
        P = mittellinie(m, wurzelhinweis(nn, m, info))
        haut, jeans = koerpermasken(rgb, al, m, H)
        r = dict(bild='birkin%02d' % nn, n=MAP[nn][0], h=MAP[nn][1], H=int(H), info=info,
                 form=form(m, P, H), muster=muster(rgb, m, P, H), farbe=farben(rgb, m, dict(haut=haut, jeans=jeans)))
        r['wurzel'] = [round(float(v), 1) for v in P[0]]
        r['spitze'] = [round(float(v), 1) for v in P[-1]]
        top = int(np.nonzero(al.any(1))[0].min())
        r['wurzel_unter_OK_H'] = round((P[0][1] - top) / H, 3)
        w = int(0.05 * H)
        d = P[w] - P[0]
        r['austritt_winkel_bild_grad'] = round(float(np.degrees(np.arctan2(d[1], d[0]))), 1)
        f = r['farbe']
        r['dE76_schwanz_haut'] = round(de76(f['schwanz']['lab'], f['haut']['lab']), 1)
        r['dE00_schwanz_haut'] = round(de2000(f['schwanz']['lab'], f['haut']['lab']), 1)
        if f.get('jeans'):
            r['dE00_schwanz_jeans'] = round(de2000(f['schwanz']['lab'], f['jeans']['lab']), 1)
        np.save(SCR + 'A_schwanz_m%02d.npy' % nn, m)
        np.save(SCR + 'A_schwanz_P%02d.npy' % nn, P)
        np.save(SCR + 'A_haut_m%02d.npy' % nn, haut)
        out['ki%02d' % nn] = r
        fo, mu = r['form'], r['muster']
        print('birkin%02d h%d H%d Laenge %.2fH Dicke %s Knicke %d max %.0f Grad/0,05H bei %.2f (%s)' % (
            nn, r['h'], H, fo['laenge_H'], fo['dicke_H'], fo['knicke_gt20'], fo['max_dwinkel_005H'], fo['max_bei'], fo['knick_pos']))
        print('    Lab S %s Haut %s Jeans %s | rot a15 S %.2f Haut %.2f | L* S/Haut %.2f | dE00 %.1f dE76 %.1f' % (
            f['schwanz']['lab'], f['haut']['lab'], f['jeans']['lab'] if f.get('jeans') else None, f['schwanz_rot_a15'], f['haut_rot_a15'],
            f['schwanz']['lab'][0] / f['haut']['lab'][0], r['dE00_schwanz_haut'], r['dE76_schwanz_haut']))
        print('    Muster AK a %s lag %s H std %s | AK L %s | a Haelften %s/%s | Info %s Wurzel %s Spitze %s unterOK %.3f Austritt %.0f Grad | Verj ab %s (%s H)' % (
            mu['ak_a_max'], mu['ak_a_lag_H'], mu['a_rest_std'], mu['ak_L_max'], mu['a_haelfte_neg'], mu['a_haelfte_pos'], info, r['wurzel'], r['spitze'],
            r['wurzel_unter_OK_H'], r['austritt_winkel_bild_grad'], fo['verjuengung_ab'], fo['verjuengt_laenge_H']))
        print('    Breiten', fo['breiten_profil'])
        print('    Winkel ', fo['winkel_profil'])
        print('    a*prof ', mu['a_profil_21'])
    json.dump(out, open(SCR + 'A_schwanz_ki.json', 'w'), indent=1, default=lambda o: o.item() if hasattr(o, 'item') else str(o))
