"""Nachtrag Rot (Messer): Rot je Region in Original, Rundlauf, Variante V3 r3 (Kette) und neutral (G4, V3).
Ausgabe mess.json, Zwischenbilder *.npz fuer belege.py."""
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


ERG = dict(regionen=REGIONEN, saaten=SAATEN, block=BLOCK, nboot=NBOOT, min_px=MIN_PX, bilder={})
k = P.Kandidat(V3DIR + '/form.json')
for n in (1, 2):
    reg, kn, fa = regionen_szene(n, k.mo)
    zei = np.load('zeile%d.npy' % n)
    V = P.original(n)
    To, _c = PZ.rueck(V, n)
    clip = V.min(2) >= 200
    figu = np.zeros((480, 640), bool)
    for a, m in PZ.regionen(n):
        figu |= m
    sauber, alb, m, kk = k.rendern(n)
    figv = m & (alb.max(2) > 0)
    vids = {s: np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8) for s in SAATEN}
    # Leuchtstellen (bleiben wie Runde 3): Bild 1 F1 (Maske des Messblatt-Pruefers + dieselbe Regel 3.1 an der Variante + helle F1-Texel),
    # Bild 2 Band + Zaehne (Masken des Messblatt-Pruefers + CLUT-Zeilen 7/8 der Variante)
    if n == 1:
        leucht = maske('F1') | f1_regel(vids[15]) | ((zei == 6) & (lab(alb.astype(np.float32))[..., 0] > 28.66))
    else:
        leucht = maske('band') | maske('zahn') | np.isin(zei, (7, 8))
    leucht = cv2.dilate(leucht.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)   # 2 px Saum = Optik-sigma 2 px (farbe.md 3)
    basis = figu & (kn >= 0) & figv & ~clip & ~leucht
    # Rundlauf und Neutral-Probe (Schwelle): Figur unbunt mit gleicher Helligkeit, sonst gleiches Bild
    LTo = lab(To.astype(np.float32))
    Ln = LTo.copy(); Ln[figu, 1:] = 0
    Tneu = rgb_von_lab(Ln)
    eR = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
    eN = np.clip(Tneu * PZ.LICHT[n], 0, 255).astype(np.uint8)
    Q = {'O_T': LTo, 'O_V': lab(V.astype(np.float32)), 'V3_alb': lab(alb.astype(np.float32))}
    neutral_T, neutral_V = [], []
    for s in SAATEN:
        vr = np.asarray(PZ.FV.video_filter(eR, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8)
        Q['R_T%d' % s] = lab(PZ.rueck(vr, n)[0].astype(np.float32)); Q['R_V%d' % s] = lab(vr.astype(np.float32))
        Q['V3_T%d' % s] = lab(PZ.rueck(vids[s], n)[0].astype(np.float32)); Q['V3_V%d' % s] = lab(vids[s].astype(np.float32))
        vn = np.asarray(PZ.FV.video_filter(eN, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8)
        neutral_T.append(lab(PZ.rueck(vn, n)[0].astype(np.float32))[basis][:, 1]); neutral_V.append(lab(vn.astype(np.float32))[basis][:, 1])
    nT, nV = np.concatenate(neutral_T), np.concatenate(neutral_V)
    A0 = dict(T=float(np.ceil(np.percentile(nT, 99))), V=float(np.ceil(np.percentile(nV, 99))))
    neutral = dict(T_p50_p95_p99=[round(float(np.percentile(nT, q)), 1) for q in (50, 95, 99)],
                   V_p50_p95_p99=[round(float(np.percentile(nV, q)), 1) for q in (50, 95, 99)])
    B = dict(px_basis=int(basis.sum()), px_figur_nutzer=int(figu.sum()), px_k5c_in_figur=int((figu & (kn >= 0)).sum()),
             px_leucht=int((leucht & figu).sum()), px_clip=int((clip & figu).sum()), A0=A0, neutral=neutral, zonen={})
    for rn in REGIONEN + ['Figur']:
        rm = basis & ((reg == RID[rn]) if rn != 'Figur' else (reg >= 0))
        if rm.sum() < MIN_PX:
            B['zonen'][rn] = dict(px=int(rm.sum()))
            continue
        z = {}
        for q, Lf in Q.items():
            z[q] = masse(Lf, rm, A0['V' if '_V' in q else 'T'])
        z['O_T_boot'], z['bloecke'] = boot(Q['O_T'], rm, A0['T'])
        z['O_V_boot'], _ = boot(Q['O_V'], rm, A0['V'])
        B['zonen'][rn] = z
    lm = (maske('F1') if n == 1 else maske('band')) & ~clip
    B['leuchtstelle'] = {q: masse(Lf, lm, A0['V' if '_V' in q else 'T']) for q, Lf in Q.items() if q in ('O_T', 'O_V', 'V3_T15', 'V3_V15')}
    ERG['bilder'][str(n)] = B
    np.savez_compressed('szene%d.npz' % n, basis=basis, reg=reg, leucht=leucht, figu=figu, V=V, To=To.astype(np.float32), sauber=sauber, alb=alb,
                        v3vid=vids[15], v3T=PZ.rueck(vids[15], n)[0].astype(np.float32), A0T=A0['T'], A0V=A0['V'])
    print('Bild', n, 'basis', B['px_basis'], 'A0', A0, neutral, flush=True)

# neutral: G4 (Clip 0) und V3 (Clip 2, Ruhe) wie Tafel 03, drei Winkel zusammen; Texel (Licht herausgerechnet) und Licht 0,55 + 0,45 N.L
A0n = ERG['bilder']['1']['A0']['T']
ERG['neutral'] = dict(A0=A0n)
g4 = KT.re2_mo(0x34); v3 = fl.lade_var(V3DIR)
for nm, mo, c, fn in (('G4', g4, 0, region_g4), ('V3', v3, 2, region_k5c)):
    lits, albs, regs = [], [], []
    for wt, az, el in R.WINKEL:
        img, ia, msk, rg = blick_regionen(mo, R.kf(mo, c), az, el, fn)
        lits.append(img); albs.append(ia); regs.append(rg)
        np.savez_compressed('neutral_%s_%d.npz' % (nm, az), img=img, ia=ia, msk=msk, reg=rg)
    LL = np.concatenate([lab(x.astype(np.float32)) for x in lits], 1); LA = np.concatenate([lab(x.astype(np.float32)) for x in albs], 1)
    RG = np.concatenate(regs, 1)
    E = {}
    for rn in REGIONEN + ['Figur']:
        rm = (RG == RID[rn]) if rn != 'Figur' else (RG >= 0)
        if rm.sum() < MIN_PX:
            E[rn] = dict(px=int(rm.sum())); continue
        E[rn] = dict(texel=masse(LA, rm, A0n), licht=masse(LL, rm, A0n))
    ERG['neutral'][nm] = E
    print(nm, {r: (E[r].get('texel') or {}).get('rot') for r in E}, flush=True)
json.dump(ERG, open('mess.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
