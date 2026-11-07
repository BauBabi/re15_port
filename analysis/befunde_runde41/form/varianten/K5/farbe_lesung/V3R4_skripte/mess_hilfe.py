"""Funktionen aus mess.py (ohne Lauf)."""
from basis import *
import fl, render as R, kf_tafeln as KT

SAATEN = (15, 12345, 7)          # wie MESSBLATT_FARBE_V3 5 (Rundlauf mit Saaten 15 / 12345 / 7)
BLOCK = 12                       # Block-Bootstrap: 12 px > 3 x Optik-sigma 2 px und > Chroma-sigma 6 x 3 px (farbe.md 3), Bloecke damit naeherungsweise unabhaengig
NBOOT = 300
H_ROT = (-30.0, 70.0)            # Farbton-Fenster "rot" wie MESSBLATT_FARBE_V3 4 (Leser B 4: rot = a* >= 15, L* >= 20, Farbton -30..70 Grad); darunter mauve/violett
MIN_PX = 150                     # Region mit < 150 px gilt im Bild als nicht sichtbar (Kopf Bild 1: 125 px Kieferrand, Messblatt V3 0.3)
rng = np.random.default_rng(15)
MASK = np.load(K5 + '/generator/v3_daten/masken.npz')


def maske(nm):
    return np.unpackbits(MASK[nm])[:480 * 640].reshape(480, 640).astype(bool)


def f1_regel(V):
    """MESSBLATT_FARBE_V3 3.1: Video-Lab L* >= 30 und a* >= 20, 8-zusammenhaengend mit dem a*-Maximum der Box x 300-345, y 225-265."""
    L = lab(V.astype(np.float32))
    k = (L[..., 0] >= 30) & (L[..., 1] >= 20)
    n, cc = cv2.connectedComponents(k.astype(np.uint8), connectivity=8)
    box = np.zeros_like(k); box[225:266, 300:346] = True
    a = np.where(box & k, L[..., 1], -1e9)
    if a.max() < -1e8:
        return np.zeros_like(k)
    y, x = np.unravel_index(np.argmax(a), a.shape)
    return cc == cc[y, x]


def gross_cc(m):
    n, _l, st, _c = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    return int(st[1:, cv2.CC_STAT_AREA].max()) if n > 1 else 0


def masse(Lf, reg, A0):
    """Lf: Lab-Bild (H x W x 3), reg: Regionsmaske. Rotklasse = a* >= A0 (Begruendung: Neutral-Probe)."""
    L = Lf[reg]
    if len(L) == 0:
        return None
    r = L[:, 1] >= A0
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    eng = r & (h >= H_ROT[0]) & (h < H_ROT[1])
    C = np.hypot(L[:, 1], L[:, 2])
    k12 = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20)
    d = dict(px=int(len(L)), rot=round(100 * float(r.mean()), 2), rot_eng=round(100 * float(eng.mean()), 2), mauve=round(100 * float((r & (h < H_ROT[0])).mean()), 2),
             k12=round(100 * float(k12.mean()), 2), lab=[round(float(x), 1) for x in L.mean(0)])
    if eng.sum() >= 5:
        d['h_eng'] = round(math.degrees(math.atan2(float(L[eng, 2].mean()), float(L[eng, 1].mean()))), 1)
        d['C_eng'] = round(float(C[eng].mean()), 1)
    else:
        d['h_eng'] = d['C_eng'] = None
    em = np.zeros(reg.shape, bool); em[reg] = eng
    d['gross_eng_p'] = round(100 * gross_cc(em) / len(L), 2)
    if r.sum() >= 5:
        am, bm = float(L[r, 1].mean()), float(L[r, 2].mean())
        d.update(C_rot=round(float(np.hypot(L[r, 1], L[r, 2]).mean()), 1), h_rot=round(math.degrees(math.atan2(bm, am)), 1),
                 L_rot=round(float(L[r, 0].mean()), 1))
    else:
        d.update(C_rot=None, h_rot=None, L_rot=None)
    d['L_haut'] = round(float(L[~r, 0].mean()), 1) if (~r).sum() else None
    d['dL'] = round(d['L_rot'] - d['L_haut'], 1) if d['L_rot'] is not None and d['L_haut'] is not None else None
    rm = np.zeros(reg.shape, bool); rm[reg] = r
    g = gross_cc(rm)
    d.update(gross_px=g, gross_p=round(100 * g / len(L), 2))
    return d


def boot(Lf, reg, A0):
    """Block-Bootstrap (BLOCK x BLOCK px) der Vorlage: Standardabweichung von rot, C_rot, h_rot, dL, gross_p."""
    ys, xs = np.nonzero(reg)
    bid = (ys // BLOCK) * 1000 + (xs // BLOCK)
    ub, inv = np.unique(bid, return_inverse=True)
    L = Lf[ys, xs]
    out = {k: [] for k in ('rot', 'rot_eng', 'C_rot', 'h_rot', 'dL', 'gross_p')}
    gruppen = [np.nonzero(inv == i)[0] for i in range(len(ub))]
    for _ in range(NBOOT):
        w = rng.integers(0, len(ub), len(ub))
        idx = np.concatenate([gruppen[i] for i in w])
        l = L[idx]; r = l[:, 1] >= A0
        out['rot'].append(100 * r.mean())
        hh = np.degrees(np.arctan2(l[:, 2], l[:, 1]))
        out['rot_eng'].append(100 * (r & (hh >= H_ROT[0]) & (hh < H_ROT[1])).mean())
        if r.sum() >= 5 and (~r).sum():
            out['C_rot'].append(np.hypot(l[r, 1], l[r, 2]).mean())
            out['h_rot'].append(math.degrees(math.atan2(l[r, 2].mean(), l[r, 1].mean())))
            out['dL'].append(l[r, 0].mean() - l[~r, 0].mean())
    return {k: (round(float(np.std(v)), 2) if len(v) > 0.8 * NBOOT else None) for k, v in out.items()}, int(len(ub))


