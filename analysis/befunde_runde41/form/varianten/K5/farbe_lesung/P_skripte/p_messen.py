"""Pruefer: Stilmasse an den Texeln, eigene Umsetzung. Ausgabe p_messung.json."""
import json, math
import numpy as np, cv2
from p_lade import *


def atlas(mo, per):
    h, w = mo.tex.shape
    rgb = np.zeros((h, w, 3), np.uint8); mid = np.full((h, w), -1, int)
    for b in sorted(per):
        d = per[b]; x, y = d['xy'][:, 0], d['xy'][:, 1]
        frei = mid[y, x] < 0
        rgb[y[frei], x[frei]] = d['rgb'][frei]; mid[y[frei], x[frei]] = b
    return rgb, mid


def rect(h, w, r):
    m = np.zeros((h, w), bool); x, y, ww, hh = r; m[y:y + hh, x:x + ww] = True; return m


def km(L, k, seed=3):
    cv2.setRNGSeed(seed)
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.05)
    _, lb, c = cv2.kmeans(L.astype(np.float32), min(k, len(L)), None, crit, 6, cv2.KMEANS_PP_CENTERS)
    lb = lb.ravel(); a = np.bincount(lb, minlength=len(c)) / len(lb)
    return c.astype(float), a, lb


def verschmelze(c, a, d=10.0, art='single'):
    """Cluster-Gruppen: single = Kette (wie Leser B), complete = jedes Paar in der Gruppe < d."""
    grp = [[i] for i in range(len(c))]
    D = np.linalg.norm(c[:, None] - c[None], axis=2)
    while True:
        best = None
        for i in range(len(grp)):
            for j in range(i + 1, len(grp)):
                sub = D[np.ix_(grp[i], grp[j])]
                v = sub.min() if art == 'single' else sub.max()
                if v < d and (best is None or v < best[0]):
                    best = (v, i, j)
        if best is None:
            break
        _, i, j = best; grp[i] += grp[j]; del grp[j]
    return sum(1 for g in grp if a[g].sum() >= 0.05)


def farben_je_teil(rgb, mid, zone, minn=200):
    r = dict(roh=[], single=[], complete=[], streu_ab=[], streu_lab=[])
    for b in np.unique(mid[zone]):
        s = zone & (mid == b)
        if s.sum() < minn:
            continue
        L = lab(rgb[s]); c, a, _ = km(L, 8)
        r['roh'].append(int((a >= 0.05).sum()))
        r['single'].append(verschmelze(c, a, 10, 'single'))
        r['complete'].append(verschmelze(c, a, 10, 'complete'))
        r['streu_ab'].append(float(np.sqrt(L[:, 1].var() + L[:, 2].var())))
        r['streu_lab'].append(float(np.sqrt(L.var(0).sum())))
    o = {k: round(float(np.mean(v)), 2) for k, v in r.items()}
    o['teile'] = len(r['roh'])
    return o


def hf_fft(Lmap, mid, zone, minn=200, f0=0.25):
    acc = wt = 0.0
    for b in np.unique(mid[zone]):
        n, cc, st, _ = cv2.connectedComponentsWithStats((zone & (mid == b)).astype(np.uint8), connectivity=8)
        for k in range(1, n):
            if st[k, 4] < minn:
                continue
            x, y, w, h = st[k, :4]
            c = cc[y:y + h, x:x + w] == k
            P = Lmap[y:y + h, x:x + w].astype(float); mu = P[c].mean()
            P = (np.where(c, P, mu) - mu) * np.outer(np.hanning(h), np.hanning(w))
            F = np.abs(np.fft.fft2(P)) ** 2
            fr = np.hypot(np.fft.fftfreq(h)[:, None], np.fft.fftfreq(w)[None, :])
            e = F[fr > 0].sum()
            if e <= 0:
                continue
            acc += st[k, 4] * F[fr > f0].sum() / e; wt += st[k, 4]
    return acc / wt if wt else None


def hf_rest(Lmap, zone, sig=1.0):
    """Gegenmass ohne FFT: normierte Faltung im Feld, Rest = L - Glatt(sigma 1 Texel); Restvarianz / Gesamtvarianz, nur Innentexel."""
    m = zone.astype(np.float32); L = Lmap.astype(np.float32) * m
    g = cv2.GaussianBlur(L, (0, 0), sig) / np.maximum(cv2.GaussianBlur(m, (0, 0), sig), 1e-6)
    inn = cv2.erode(zone.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
    r = (Lmap - g)[inn]; v = Lmap[inn]
    return float(r.var() / v.var())


def dichte(tri, meshes):
    sel = [(math.sqrt(u / A), A) for b, u, A in tri if b in meshes and A > 1 and u > 0.25]
    d = np.array([s[0] for s in sel]); wA = np.array([s[1] for s in sel])
    return round(float(d.mean()), 4), round(float(np.sum(d * wA) / wA.sum()), 4)


def stil(name, rgb, mid, zone, tri=None, meshes=None):
    L = lab(rgb[zone]); C = np.hypot(L[:, 1], L[:, 2]); hue = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    Lmap = np.zeros(mid.shape); Lmap[zone] = L[:, 0]
    c, a, _ = km(L, 8)
    o = np.argsort(-a)
    rotA = (L[:, 1] > 15) & (C > 20) & (hue > -40) & (hue < 70)
    rotB = (L[:, 1] >= 15) & (L[:, 0] >= 20) & (hue > -30) & (hue < 70)
    hf = hf_fft(Lmap, mid, zone)
    r = dict(name=name, texel=int(zone.sum()),
             lab_mittel=[round(float(x), 1) for x in L.mean(0)], rgb_mittel=[int(round(x)) for x in rgb[zone].mean(0)],
             chroma=round(float(C.mean()), 1), chroma_streu=round(float(C.std()), 1),
             L_streu=round(float(L[:, 0].std()), 1), L_p5_p95=[round(float(np.percentile(L[:, 0], q)), 1) for q in (5, 95)],
             glanz_L85=round(100 * float((L[:, 0] > 85).mean()), 2), glanz_neutral=round(100 * float(((L[:, 0] > 85) & (C < 10)).mean()), 2),
             rot_A=round(100 * float(rotA.mean()), 1), rot_B=round(100 * float(rotB.mean()), 1),
             rot_lab=[round(float(x), 1) for x in L[rotA].mean(0)] if rotA.sum() > 20 else None,
             hf_fft=round(hf, 3) if hf is not None else None, hf_rest=round(hf_rest(Lmap, zone), 3),
             palette=[dict(lab=[round(float(x), 1) for x in c[i]], anteil=round(float(a[i]), 3)) for i in o])
    r['farben_je_teil'] = farben_je_teil(rgb, mid, zone)
    if tri is not None:
        r['dichte_mittel'], r['dichte_flaeche'] = dichte(tri, meshes)
    return r, c[o], a[o]


def pal_abstand(cA, aA, cRef):
    D = np.linalg.norm(cA[:, None] - cRef[None], axis=2).min(1)
    return round(float((D * aA).sum() / aA.sum()), 1), [round(float(x), 1) for x in D]


def lade_alle():
    M = {}

    def reg(nm, mo):
        per, tri = sammeln(mo); rgb, mid = atlas(mo, per); M[nm] = (mo, per, tri, rgb, mid)
    reg('EM30', em30()); reg('G4', re2(0x34)); reg('G5', re2(0x36)); reg('Zombie', re15typ(0x10)); reg('Hund', re15typ(0x20))
    reg('K5c', var(K5 + '/K5c')); reg('K4', var(K5 + '/K4_heute')); reg('J', var(VAR + '/J_dicker'))
    for vn in ('C_mitte', 'D_fleisch'):
        try:
            reg(vn, var(VAR + '/' + vn))
        except Exception as e:
            print('fehlt', vn, e)
    return M


def zonen_von(M):
    zonen = {}
    mo, per, tri, rgb, mid = M['EM30']; u = mid >= 0
    zonen['EM30_koerper'] = ('EM30', u & (mid != 9), [b for b in per if b != 9])
    zonen['EM30_haut'] = ('EM30', u & ~np.isin(mid, [0, 1, 3, 4, 6, 9]), [b for b in per if b not in (0, 1, 3, 4, 6, 9)])
    zonen['EM30_kopf'] = ('EM30', u & (mid == 9), [9])
    mo, per, tri, rgb, mid = M['G4']; u = mid >= 0; h, w = mid.shape
    kr = [26, 4, 8, 14, 19, 22, 25]
    zonen['G4_koerper'] = ('G4', u & ~np.isin(mid, kr) & ~rect(h, w, (270, 160, 52, 86)), [b for b in per if b not in kr])
    zonen['G4_kopf'] = ('G4', u & (mid == 26), [26])
    mo, per, tri, rgb, mid = M['G5']; u = mid >= 0; h, w = mid.shape
    maul = rect(h, w, (0, 143, 228, 113)); aug = rect(h, w, (450, 196, 48, 46)) | rect(h, w, (168, 95, 22, 22))
    zonen['G5_koerper'] = ('G5', u & ~maul & ~aug, list(per))
    zonen['G5_maul'] = ('G5', u & maul, list(per))
    for nm, kopf in (('Zombie', [14]), ('Hund', [3, 4])):
        mo, per, tri, rgb, mid = M[nm]; u = mid >= 0
        zonen[nm + '_koerper'] = (nm, u & ~np.isin(mid, kopf), [b for b in per if b not in kopf])
        zonen[nm + '_kopf'] = (nm, u & np.isin(mid, kopf), kopf)
    for nm in ('K5c', 'K4', 'J', 'C_mitte', 'D_fleisch'):
        if nm not in M:
            continue
        mo, per, tri, rgb, mid = M[nm]; u = mid >= 0; h, w = mid.shape
        xs = np.arange(w)[None, :]
        zonen[nm + '_koerper'] = (nm, u & (xs < 384), [b for b in per if b not in (22, 23)])
        if nm in ('K5c', 'K4'):
            zonen[nm + '_kopf'] = (nm, u & rect(h, w, (384, 0, 128, 104)), [22])
            zonen[nm + '_kiefer'] = (nm, u & rect(h, w, (384, 106, 128, 72)), [23])
            zonen[nm + '_auge'] = (nm, u & rect(h, w, (384, 196, 24, 10)), [22])
            zonen[nm + '_zahn'] = (nm, u & rect(h, w, (384, 180, 24, 14)), [22, 23])
            zonen[nm + '_kralle'] = (nm, u & rect(h, w, (410, 180, 100, 40)), [13])
            zonen[nm + '_hals'] = (nm, u & (mid == 21) & (xs < 384), [21])
            zonen[nm + '_schwanz'] = (nm, u & np.isin(mid, [1, 2, 3, 4, 5, 6]) & (xs < 384), [1, 2, 3, 4, 5, 6])
            zonen[nm + '_gesamt_ohne_zahn_auge'] = (nm, u & ~rect(h, w, (384, 175, 128, 81)), list(per))
    return zonen


if __name__ == '__main__':
    out = {}
    M = lade_alle()
    zonen = zonen_von(M)
    zen = {}
    for zn, (nm, z, ms) in zonen.items():
        mo, per, tri, rgb, mid = M[nm]
        r, c, a = stil(zn, rgb, mid, z, tri, ms)
        out[zn] = r; zen[zn] = (c, a)
        print(zn, {k: v for k, v in r.items() if k not in ('palette',)})
    ref = zen['EM30_haut'][0]
    for zn in zen:
        out[zn]['pal_abstand_EM30haut'] = pal_abstand(zen[zn][0], zen[zn][1], ref)
        print('PAL', zn, out[zn]['pal_abstand_EM30haut'])
    for zn in ('EM30_kopf', 'G4_kopf', 'G5_maul', 'Hund_kopf', 'K5c_kiefer', 'K5c_kopf', 'K4_kiefer'):
        nm, z, _ = zonen[zn]; rgb = M[nm][3]
        L = lab(rgb[z]); C = np.hypot(L[:, 1], L[:, 2])
        zahn = (L[:, 0] > 60) & (C < 35); rot = L[:, 1] > 20; dunkel = L[:, 0] < 20
        out[zn]['maul'] = dict(zahn=round(100 * float(zahn.mean()), 1), rot=round(100 * float(rot.mean()), 1), dunkel=round(100 * float(dunkel.mean()), 1),
                               rot_lab=[round(float(x), 1) for x in L[rot].mean(0)] if rot.sum() > 10 else None,
                               zahn_lab=[round(float(x), 1) for x in L[zahn].mean(0)] if zahn.sum() > 10 else None)
        print('MAUL', zn, out[zn]['maul'])
    json.dump(out, open('p_messung.json', 'w'), indent=1)
