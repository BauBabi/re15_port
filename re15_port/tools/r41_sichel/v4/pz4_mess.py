"""Zahlen-Pruefer V4 r4 (Kopie von pz2_mess.py, Ausgabe sfv4/pz4): eigene Messung aller Riegel aus SF_MESSBLATT_V4.md Abschnitt 7 (kein riegel_v4/v4_gemein-Messcode).
Render: pz_render.py (b2.zeichne). Messung: eigene Implementierung (L* statt Luminanz, eigene Mittellinie per geodaetischer Distanz)."""
import os, sys, json
import numpy as np, cv2
from collections import deque
from PIL import Image
from skimage.color import rgb2lab, deltaE_ciede2000
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/pz4/'
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
EA_ = np.array([9.42, 11.96, 10.94]); EB_ = np.array([0.9979, 0.9901, 0.9964])   # Messblatt Abschn. 5
K1 = np.array([0.766, 0.764, 0.768]); SW = 17.0                                  # Abschn. 1


def lab(rgb):
    a = np.asarray(rgb, float)
    return rgb2lab(np.clip(a.reshape(-1, 1, 3) / 255, 0, 1)).reshape(a.shape)


def de(a, b):
    return float(deltaE_ciede2000(np.asarray(a, float).reshape(1, 1, 3), np.asarray(b, float).reshape(1, 1, 3))[0, 0])


def vraum(img):
    return np.clip(EA_ + EB_ * cv2.GaussianBlur(img.astype(np.float32), (0, 0), 1.5), 0, 255)


def orig(b):
    a = np.asarray(Image.open(V80 + 'Beschreibung%d_Vorlage.png' % b).convert('RGB')).astype(float)
    return np.clip(SW + (a - SW) / K1, 0, 255) if b == 1 else a


def fen(box, an):
    x0, y0, x1, y1 = box
    w, h = x1 - x0, y1 - y0
    return (int(round(x0 + an[0] * w)), int(round(y0 + an[1] * h)), int(round(x0 + an[2] * w)), int(round(y0 + an[3] * h)))


def mlab(V, bx, m=None):
    x0, y0, x1, y1 = bx
    s = V[y0:y1 + 1, x0:x1 + 1].reshape(-1, 3)
    if m is not None:
        s = s[m[y0:y1 + 1, x0:x1 + 1].ravel()]
    return (lab(s.mean(0)), len(s)) if len(s) else (np.full(3, np.nan), 0)


def r(x, n=1):
    return [round(float(v), n) for v in np.ravel(x)]


def blob(V, L, box, kopfm, art='hell'):
    """Extrem = hellstes 3x3-Mittel (L*) im Kopf innerhalb der Box; Schwelle = (Extrem + Median 21x21 L*)/2; 4er-Komponente."""
    x0, y0, x1, y1 = box
    Lb = cv2.blur(L.astype(np.float32), (3, 3))
    ker = cv2.erode(kopfm.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    w = np.where(ker[y0:y1 + 1, x0:x1 + 1], Lb[y0:y1 + 1, x0:x1 + 1], -1e9 if art == 'hell' else 1e9)
    iy, ix = np.unravel_index(np.argmax(w) if art == 'hell' else np.argmin(w), w.shape)
    px, py = ix + x0, iy + y0
    umg = float(np.median(L[py - 10:py + 11, px - 10:px + 11]))
    ext = float(Lb[py, px])
    thr = (ext + umg) / 2
    m = ((L > thr) if art == 'hell' else (L < thr)) & ker
    sub = np.zeros_like(m)
    sub[y0:y1 + 1, x0:x1 + 1] = m[y0:y1 + 1, x0:x1 + 1]
    n, lb, st, _ = cv2.connectedComponentsWithStats(sub.astype(np.uint8), connectivity=4)
    k = lb[py, px]
    mk = lb == k
    ys, xs = np.nonzero(mk)
    komps = [int(st[i, 4]) for i in range(1, n)]
    return dict(ext=ext, umg=umg, thr=thr, extrem=(int(px), int(py)), px=int(mk.sum()),
                bbox=(int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())),
                wh=(int(xs.max() - xs.min() + 1), int(ys.max() - ys.min() + 1)), sp=(float(xs.mean()), float(ys.mean())),
                lab=lab(V[mk].mean(0)), lmax=float(L[mk].max()), kontrast=float(L[mk].max() - umg),
                komps=sorted(komps, reverse=True), maske=mk)


def H_von(idv):
    m = (idv >= 0) & ~((idv >= 16) & (idv <= 22))
    ys = np.nonzero(m.any(1))[0]
    ok, so = int(ys.min()), int(ys.max())
    top = np.nonzero(m[ok])[0]
    band = np.nonzero(m[ok:ok + 3].any(0))[0]
    return ok, so, so - ok, float(top.mean()), float(band.mean())


def kopf_vorn(img, ids, idv, nm):
    V = vraum(img)
    LAB = lab(V)
    L = LAB[..., 0]
    ok, so, H, sch, _ = H_von(idv)
    ys, xs = np.nonzero(ids == 9)
    kb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
    km = ids == 9
    b = blob(V, L, kb, km)
    kx = (kb[0] + kb[2]) / 2
    o = dict(H=H, ok=ok, sohle=so, kopfbox=kb)
    o['K1'] = dict(komps=b['komps'][:6], komps_ge4=sum(1 for k in b['komps'] if k >= 4), kontrast=round(b['kontrast'], 1), extrem=b['extrem'])
    kv5 = mlab(V, fen(kb, (0.05, 0.62, 0.35, 0.85)))[0]
    o['K2'] = dict(lab=r(b['lab']), dE=round(de(b['lab'], [46.9, 7.2, -1.1]), 2), KV5_L=round(float(kv5[0]), 1))
    o['K3'] = dict(w=round(b['wh'][0] / H, 3), h=round(b['wh'][1] / H, 3), px=b['wh'])
    o['K4'] = dict(links=round((kx - b['sp'][0]) / H, 3), unter_ok=round((b['sp'][1] - kb[1]) / H, 3), sp=r(b['sp']))
    k5 = mlab(V, fen(kb, (0.52, 0.25, 0.85, 0.75)))[0]
    o['K5'] = dict(lab=r(k5), dE=round(de(k5, [13.3, 7.4, 1.5]), 2), L_le=round(float(b['lab'][0]) - 25, 1))
    x0, y0, x1, y1 = b['bbox']
    sp = tuple(int(round(v)) for v in (2 * kx - x1, y0, 2 * kx - x0, y1))
    k6 = mlab(V, sp)[0]
    o['K6'] = dict(L=round(float(k6[0]), 1), box=sp)
    k7 = mlab(V, fen(kb, (0.44, 0.66, 0.62, 0.81)))[0]
    o['K7'] = dict(lab=r(k7), dE=round(de(k7, [18.2, 7.1, 1.3]), 2))
    f8 = fen(kb, (0.21, 0.88, 0.62, 1.05))
    k8 = mlab(V, f8)[0]
    sub = L[f8[1]:f8[3] + 1, f8[0]:f8[2] + 1]
    o['K8'] = dict(lab=r(k8), dE=round(de(k8, [9.9, 9.9, 3.8]), 2), anteil35=round(float((sub > 35).mean()), 3),
                   Lmax=round(float(sub.max()), 1), fenster=f8)
    k9 = mlab(V, fen(kb, (0.40, 0.02, 0.80, 0.32)))[0]
    o['K9'] = dict(lab=r(k9), dE=round(de(k9, [22.6, 15.4, 5.2]), 2))
    o['K10'] = dict(w=round((kb[2] - kb[0]) / H, 3), h=round((kb[3] - kb[1]) / H, 3),
                    w1=round((kb[2] - kb[0] + 1) / H, 3), h1=round((kb[3] - kb[1] + 1) / H, 3))
    RG = V[..., 0] - V[..., 1]
    ALL = (0, 0, 639, 479)
    F = {}
    F['F6'] = mlab(V, ALL, (ids == 8) & (RG >= 15)), [16.6, 17.3, 6.2]
    F['F7'] = mlab(V, ALL, ids == 13), [33.3, 5.5, 2.7]
    F['F8'] = mlab(V, ALL, ids == 10), [16.4, 8.3, 1.5]
    F['F9'] = mlab(V, ALL, np.isin(ids, [0, 1, 4]) & (LAB[..., 2] < 0)), [12.7, -0.9, -1.5]
    F['F10_id12'] = mlab(V, ALL, (ids == 12) & (RG >= 15)), [11.2, 14.0, 2.5]
    F['F10_id15'] = mlab(V, ALL, (ids == 15) & (RG >= 15)), [11.2, 14.0, 2.5]
    o['F'] = {k: dict(lab=r(v[0][0]), n=v[0][1], dE=round(de(v[0][0], v[1]), 2), dL=round(float(v[0][0][0] - v[1][0]), 1))
              for k, v in F.items() if v[0][1]}
    w = mlab(V, (40, 150, 119, 299))[0]
    o['F0'] = dict(lab=r(w), dE=round(de(w, [64.7, 0.3, -2.1]), 2))
    m10 = ids == 10
    o['F8_L_quantile'] = r(np.percentile(L[m10], [10, 25, 50, 75, 90])) if m10.any() else None
    m13 = ids == 13
    o['F7_L_quantile'] = r(np.percentile(L[m13], [10, 25, 50, 75, 90])) if m13.any() else None
    o['_blob'] = b
    return o


def geod(mask, seed):
    D = np.full(mask.shape, -1, int)
    q = deque([seed])
    D[seed[1], seed[0]] = 0
    while q:
        x, y = q.popleft()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1)):
            X, Y = x + dx, y + dy
            if 0 <= X < mask.shape[1] and 0 <= Y < mask.shape[0] and mask[Y, X] and D[Y, X] < 0:
                D[Y, X] = D[y, x] + 1
                q.append((X, Y))
    return D


def schwanz(ids, idv, H, ok, so, sch):
    t = (ids >= 16) & (ids <= 22)
    k = (ids == 7) | (ids == 8)
    kd = cv2.dilate(k.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    grenz = t & kd
    ys, xs = np.nonzero(grenz)
    i = np.argmax(ys)
    beg = (int(xs[i]), int(ys[i]))
    n, lb, st, _ = cv2.connectedComponentsWithStats(t.astype(np.uint8), connectivity=8)
    big = lb == (1 + int(np.argmax(st[1:, 4])))
    gy, gx = np.nonzero(big & kd)
    seed = (int(gx[np.argmax(gx)]), int(gy[np.argmax(gx)]))
    D = geod(big, seed)
    pts = []
    for d0 in range(0, D.max() + 1, 3):
        m = (D >= d0) & (D < d0 + 3)
        if m.sum() < 1:
            continue
        yy, xx = np.nonzero(m)
        pts.append((xx.mean(), yy.mean()))
    P = np.array(pts)
    Ps = np.array([P[max(0, j - 1):j + 2].mean(0) for j in range(len(P))])
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(Ps, axis=0), axis=1))]

    def breite(j):
        a = Ps[min(j + 2, len(Ps) - 1)] - Ps[max(j - 2, 0)]
        a /= np.linalg.norm(a) + 1e-9
        nrm = np.array([-a[1], a[0]])
        c = Ps[j]
        w = 0
        for sgn in (1, -1):
            for st_ in np.arange(0.5, 80, 0.5):
                x, y = c + sgn * st_ * nrm
                xi, yi = int(round(x)), int(round(y))
                if not (0 <= xi < 640 and 0 <= yi < 480) or not t[yi, xi]:
                    break
                w += 0.5
        return w
    W = np.array([breite(j) for j in range(len(Ps))])
    ang = []
    for j in range(len(Ps)):
        j2 = np.searchsorted(s, s[j] + 0.05 * H)
        if j2 >= len(Ps):
            break
        d = Ps[j2] - Ps[j]
        ang.append(np.degrees(np.arctan2(d[1], -d[0])))
    ang = np.array(ang)
    # Richtung lokal (zentriert, +-0,025 H) je Mittellinienpunkt
    def richt(j):
        a = np.searchsorted(s, s[j] - 0.025 * H); b_ = min(len(Ps) - 1, np.searchsorted(s, s[j] + 0.025 * H))
        d = Ps[b_] - Ps[max(0, a)]
        return float(np.degrees(np.arctan2(d[1], -d[0])))
    R_ = np.array([richt(j) for j in range(len(Ps))])
    hinten = [j for j in range(len(Ps)) if Ps[j][0] < beg[0] - 0.10 * H]
    j45 = next(j for j in hinten if R_[j] >= 45)
    jb = j45
    ja = int(np.searchsorted(s, s[jb] - 0.05 * H)); je = min(len(Ps) - 1, int(np.searchsorted(s, s[jb] + 0.05 * H)))
    best = (R_[je] - R_[ja], ja)
    jend = int(np.searchsorted(s, s[-1] - 0.04 * H))
    jk = jb + int(np.argmin(W[jb:jend]))
    bx, by = beg

    def rel(p):
        return (round((p[0] - bx) / H, 3), round((p[1] - by) / H, 3))
    o = dict(beginn=beg, saat=seed, T1=dict(hinter=round((sch - bx) / H, 3), ueber=round((so - by) / H, 3)),
             T2=dict(p=r(Ps[jb]), rel=rel(Ps[jb]), dreh=round(float(best[0]), 1)),
             T3=dict(p=r(Ps[jk]), rel=rel(Ps[jk]), breite=round(W[jk] / H, 3)))
    ye = np.nonzero(t.any(1))[0].max()
    xe = np.nonzero(t[ye])[0].mean()
    o['T4'] = dict(p=r(Ps[-1]), rel=rel(Ps[-1]), unterster_px=(round(float(xe), 1), int(ye)), rel_px=rel((xe, ye)),
                   ueber=round((so - Ps[-1][1]) / H, 3))
    o['abs'] = {nm: (round((sch - p[0]) / H, 3), round((so - p[1]) / H, 3))
                for nm, p in (('Beginn', beg), ('Biegung', Ps[jb]), ('Knick', Ps[jk]), ('Ende', Ps[-1]))}
    Lk = s[jk]
    o['T5'] = {}
    for f in (0.1, 0.3, 0.5, 0.7, 0.9):
        j = int(np.argmin(np.abs(s - (Lk - f * Lk))))
        o['T5'][f] = round(W[j] / H, 3)
    seq = [W[int(np.argmin(np.abs(s - (Lk - f * Lk))))] / H for f in np.linspace(0.05, 0.75, 15)]
    o['T5']['max_abfall'] = round(float(max(max(seq[:i + 1]) - seq[i] for i in range(len(seq)))), 3)
    end = W[jk + 1:]
    d = Ps[-1] - Ps[jk]
    o['T6'] = dict(breite_median=round(float(np.median(end)) / H, 3), breite_min=round(float(end.min()) / H, 3),
                   laenge=round(float(np.linalg.norm(d)) / H, 3),
                   richtung=round(float(np.degrees(np.arctan2(d[1], abs(d[0])))), 1), nach_vorn=bool(d[0] > 0),
                   taille=round(W[jk] / H, 3),
                   schmalste_davor_ab_biegung=round(float(W[jb:jk].min()) / H, 3) if jk > jb else None)
    o['richtungen_je_0.05H'] = [(round(float(s[j] / H), 2), round(float(R_[j]), 0)) for j in range(0, len(Ps), max(1, len(Ps) // 25))]
    o['_P'] = Ps.tolist()
    o['_W'] = W.tolist()
    o['_s'] = s.tolist()
    o['_jb'] = jb
    o['_jk'] = jk
    return o


def seite(img, ids, idv, nm):
    V = vraum(img)
    LAB = lab(V)
    L = LAB[..., 0]
    ok, so, H, sch, sch3 = H_von(idv)
    ys, xs = np.nonzero(ids == 9)
    kb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
    km = ids == 9
    b = blob(V, L, kb, km)
    o = dict(H=H, ok=ok, sohle=so, scheitel=round(sch, 1), scheitel_3zeilen=round(sch3, 1), kopfbox=kb)
    o['S1'] = dict(lab=r(b['lab']), dE=round(de(b['lab'], [48.4, 4.0, 0.5]), 2), w=round(b['wh'][0] / H, 3), h=round(b['wh'][1] / H, 3),
                   px=b['wh'], komps=b['komps'][:8], komps_ge4=sum(1 for k in b['komps'] if k >= 4),
                   kontrast=round(b['kontrast'], 1), extrem=b['extrem'], sp=r(b['sp']))
    o['S2'] = dict(hinter=round((kb[2] - b['sp'][0]) / H, 3), ueber=round((so - b['sp'][1]) / H, 3))
    f3 = fen(kb, (0.84, 0.52, 0.96, 0.80))
    s3 = mlab(V, f3)[0]
    fy, fx = np.nonzero(b['maske'])
    dx = np.maximum(0, np.maximum(f3[0] - fx, fx - f3[2]))
    dy = np.maximum(0, np.maximum(f3[1] - fy, fy - f3[3]))
    o['S3'] = dict(lab=r(s3), dE=round(de(s3, [19.1, 1.2, 1.1]), 2), abstand=round(float(np.hypot(dx, dy).min()) / H, 3), fenster=f3)
    s4 = mlab(V, fen(kb, (0.11, 0.04, 0.53, 0.29)))[0]
    o['S4'] = dict(lab=r(s4), dE=round(de(s4, [27.6, 15.3, 3.3]), 2))
    o['T'] = schwanz(ids, idv, H, ok, so, sch3)
    bx, by = o['T']['beginn']
    P = np.array(o['T']['_P'])
    jb, jk = o['T']['_jb'], o['T']['_jk']
    t = (ids >= 16) & (ids <= 22)
    C = np.hypot(LAB[..., 1], LAB[..., 2])
    yy, xx = np.mgrid[0:480, 0:640]

    def kreis(c, rr):
        return (xx - c[0]) ** 2 + (yy - c[1]) ** 2 <= rr * rr
    ALL = (0, 0, 639, 479)
    T8 = {}
    T8['F15'] = mlab(V, ALL, t & kreis((bx - 12, by - 5), 12) & (C >= 6)), [9.3, 11.4, 1.5]
    T8['F16'] = mlab(V, ALL, t & kreis((P[jb][0] + 10, P[jb][1]), 10) & (C >= 6)), [8.7, 8.4, 2.2]
    T8['F17'] = mlab(V, ALL, t & (C < 6) & (yy <= P[jb][1])), [26.8, -0.1, 2.4]
    T8['F18'] = mlab(V, ALL, t & (yy > P[jk][1]) & (yy <= P[-1][1] + 3)), [15.2, 3.2, 1.6]
    T8['F17_alle_oberhalb_T2'] = mlab(V, ALL, t & (yy <= P[jb][1])), [26.8, -0.1, 2.4]
    T8['gesamt_bis_Knick_vs_Bogen_gesamt'] = mlab(V, ALL, t & (yy <= P[jk][1])), [22.4, 3.1, 2.2]
    o['T8'] = {k: dict(lab=r(v[0][0]), n=v[0][1], dE=round(de(v[0][0], v[1]), 2)) for k, v in T8.items() if v[0][1]}
    o['T8_anteil_chroma_lt6_oberhalb_T2'] = round(float((t & (C < 6) & (yy <= P[jb][1])).sum() / max(1, (t & (yy <= P[jb][1])).sum())), 3)
    m8 = ids == 8
    p70 = np.percentile(L[m8], 70)
    F = {'F14': (mlab(V, ALL, m8 & (L >= p70)), [60.0, 11.0, 1.2]),
         'F19': (mlab(V, ALL, np.isin(ids, [0, 1, 4]) & (LAB[..., 2] < 0)), [18.4, 0.5, -3.5])}
    o['F'] = {k: dict(lab=r(v[0][0]), n=v[0][1], dE=round(de(v[0][0], v[1]), 2), dL=round(float(v[0][0][0] - v[1][0]), 1))
              for k, v in F.items() if v[0][1]}
    w = mlab(V, (40, 150, 119, 299))[0]
    o['F0'] = dict(lab=r(w), dE=round(de(w, [64.7, 0.3, -2.1]), 2))
    return o


def orig_check():
    """Selbstpruefung der eigenen Messfunktionen am Original (die Messblatt-Ziele muessen herauskommen)."""
    out = {}
    V = orig(1)
    L = lab(V)[..., 0]
    kb = (368, 139, 416, 191)
    km = np.zeros((480, 640), bool)
    km[kb[1]:kb[3] + 1, kb[0]:kb[2] + 1] = True
    b = blob(V, L, (370, 155, 392, 178), km)          # Suchfenster um das Auge-Linienende (382,164)
    out['vorn_auge'] = dict(lab=r(b['lab']), wh=b['wh'], sp=r(b['sp']), w=round(b['wh'][0] / 302, 3), h=round(b['wh'][1] / 302, 3),
                            kontrast=round(b['kontrast'], 1), komps=b['komps'][:5],
                            links_von_mitte=round((392 - b['sp'][0]) / 302, 3), unter_ok=round((b['sp'][1] - 139) / 302, 3))
    for nm, an, z in (('KV1', (0.40, 0.02, 0.80, 0.32), [22.6, 15.4, 5.2]), ('KV2', (0.52, 0.25, 0.85, 0.75), [13.3, 7.4, 1.5]),
                      ('KV3', (0.44, 0.66, 0.62, 0.81), [18.2, 7.1, 1.3]), ('KV4', (0.21, 0.88, 0.62, 1.05), [9.9, 9.9, 3.8]),
                      ('KV5', (0.05, 0.62, 0.35, 0.85), [18.9, 7.1, 0.6])):
        f = fen(kb, an)
        l = mlab(V, f)[0]
        out[nm] = dict(fenster=f, lab=r(l), dE_ziel=round(de(l, z), 2))
    V2 = orig(2)
    L2 = lab(V2)[..., 0]
    kb2 = (412, 190, 467, 263)
    km2 = np.zeros((480, 640), bool)
    km2[kb2[1]:kb2[3] + 1, kb2[0]:kb2[2] + 1] = True
    b2_ = blob(V2, L2, (440, 225, 462, 248), km2)    # Suchfenster um das Auge-Linienende (460,234)
    out['seite_auge'] = dict(lab=r(b2_['lab']), wh=b2_['wh'], sp=r(b2_['sp']), w=round(b2_['wh'][0] / 253, 3), h=round(b2_['wh'][1] / 253, 3),
                             komps=b2_['komps'][:5], hinter_vk465=round((465 - b2_['sp'][0]) / 253, 3), ueber_sohle436=round((436 - b2_['sp'][1]) / 253, 3))
    for nm, an, z in (('KS1', (0.11, 0.04, 0.53, 0.29), [27.6, 15.3, 3.3]), ('KS2', (0.84, 0.52, 0.96, 0.80), [19.1, 1.2, 1.1])):
        f = fen(kb2, an)
        l = mlab(V2, f)[0]
        out[nm] = dict(fenster=f, lab=r(l), dE_ziel=round(de(l, z), 2))
    out['wand1'] = r(mlab(V, (40, 150, 119, 299))[0])
    out['wand2'] = r(mlab(V2, (40, 150, 119, 299))[0])
    return out


def dump(o, ind=''):
    for k, v in o.items():
        if str(k).startswith('_'):
            continue
        if isinstance(v, dict):
            print(ind + str(k) + ':')
            dump(v, ind + '  ')
        else:
            print(ind + '%s: %s' % (k, v))


if __name__ == '__main__':
    res = {'orig': orig_check()}
    for nm in sys.argv[1:] or ['V4', 'V3', 'REF2']:
        d = np.load(SP + 'r_%s_135.npz' % nm)
        v = kopf_vorn(d['img'], d['ids'], d['idv'], nm)
        v.pop('_blob')
        d = np.load(SP + 'r_%s_85.npz' % nm)
        s = seite(d['img'], d['ids'], d['idv'], nm)
        res[nm] = dict(vorn=v, seite=s)
    json.dump(res, open(SP + 'mess_%s.json' % '_'.join(sys.argv[1:] or ['alle']), 'w'), indent=1,
              default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    dump(res)
