"""Runde 5: Ziel "Bild 2 Vorderbeine ohne Krallen" nach demselben Verfahren wie ziele.py / mess.py (Nachtrag Rot 1.4):
O = Vorlage (rueckgerechnet), Toleranz 2 sigma Block-Bootstrap (12 px, 300 Ziehungen, rng 15) + |Rundlauf-Mittel - O|, groesste Rotflaeche
[0,6 O, max(1,5 O, O + 3)] +- (|R - O| + Spannweite). Krallenmaske = Kennungs-Render der Variante (Geometrie in r3/r4/r5 gleich, AE1/G0),
2 px Saum (5 x 5, wie der Leuchtstellen-Saum in mess.py: Optik-sigma 2 px). Dieselbe Pixelmenge fuer Vorlage und Varianten.
    python ziel_ohne_krallen.py <aus_dir>
"""
import json, math, os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kralle_rv as KR  # noqa
R, PZ = KR.R, KR.PZ
K5 = KR.K5
BLOCK, NBOOT = 12, 300           # wie mess.py
H_ROT = R.H_ROT


def boot(Lf, reg, A0, rng):
    ys, xs = np.nonzero(reg)
    bid = (ys // BLOCK) * 1000 + (xs // BLOCK)
    ub, inv = np.unique(bid, return_inverse=True)
    L = Lf[ys, xs]
    out = {k: [] for k in ('rot_eng', 'h_rot', 'dL')}
    gruppen = [np.nonzero(inv == i)[0] for i in range(len(ub))]
    for _ in range(NBOOT):
        w = rng.integers(0, len(ub), len(ub))
        idx = np.concatenate([gruppen[i] for i in w])
        l = L[idx]; r = l[:, 1] >= A0
        hh = np.degrees(np.arctan2(l[:, 2], l[:, 1]))
        out['rot_eng'].append(100 * (r & (hh >= H_ROT[0]) & (hh < H_ROT[1])).mean())
        if r.sum() >= 5 and (~r).sum():
            out['h_rot'].append(math.degrees(math.atan2(l[r, 2].mean(), l[r, 1].mean())))
            out['dL'].append(l[r, 0].mean() - l[~r, 0].mean())
    return {k: float(np.std(v)) for k, v in out.items()}, int(len(ub))


def main():
    aus = sys.argv[1]
    os.makedirs(aus, exist_ok=True)
    M = np.load(os.path.join(KR.RR, 'rot_masken.npz'))
    E = json.load(open(os.path.join(KR.RR, 'mess.json'), encoding='utf-8'))
    Z = json.load(open(os.path.join(KR.RR, 'ziele.json'), encoding='utf-8'))
    n = 2
    kr = KR.kennung(os.path.join(K5, 'farbe', 'K5c_farbe_v3'), os.path.join(HIER, 'z_id'))[n]
    kr_saum = cv2.dilate(kr.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
    basis0 = np.unpackbits(M['basis2'])[:480 * 640].reshape(480, 640).astype(bool)
    reg = M['reg2'].astype(int)
    A0 = E['bilder']['2']['A0']['T']
    vb = basis0 & (reg == R.REGIONEN.index('Vorderbeine'))
    rm = vb & ~kr_saum
    np.savez_compressed(os.path.join(aus, 'krallen_bild2.npz'), kralle=np.packbits(kr_saum.ravel()), form=np.array([480, 640]))
    V = PZ.P.original(n)
    To, _ = PZ.rueck(V, n)
    LO = R.lab(To.astype(np.float32))
    O = R.masse(LO, rm, A0)
    sig, nb = boot(LO, rm, A0, np.random.default_rng(15))
    eR = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
    LR = [R.lab(PZ.rueck(np.asarray(PZ.FV.video_filter(eR, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0]) for s in R.SAATEN]
    Rm = [R.masse(L, rm, A0) for L in LR]
    ziel = {}
    for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p'):
        v = [q[m] for q in Rm if q[m] is not None]
        rmit, rsp = float(np.mean(v)), float(max(v) - min(v))
        o = O[m]
        if m == 'gross_eng_p':
            w = abs(rmit - o) + rsp
            lo, hi = max(0.6 * o - w, 0.0), max(1.5 * o, o + 3.0) + w
        else:
            tol = 2 * sig[m] + abs(rmit - o)
            lo, hi = o - tol, o + tol
            if m == 'rot_eng':
                lo = max(lo, 0.0)
        ziel[m] = dict(O=round(o, 2), R=round(rmit, 2), R_spann=round(rsp, 2), sigma=None if m == 'gross_eng_p' else round(sig[m], 2),
                       ziel=[round(lo, 2), round(hi, 2)], bindend=True)
    erg = dict(px_vorderbeine=int(vb.sum()), px_kralle_mit_saum=int((vb & kr_saum).sum()), px_ohne=int(rm.sum()), bloecke=nb, A0=A0,
               ziel=ziel, alt=Z['bild2|Vorderbeine|T'])
    staende = [('Runde 3', 'K5c_farbe_v3r3'), ('Runde 4', 'K5c_farbe_v3r4'), ('Runde 5', 'K5c_farbe_v3')]
    erg['staende'] = {}
    for nm, d in staende:
        k = PZ.P.Kandidat(os.path.join(K5, 'farbe', d, 'form.json'))
        sauber, alb, mm, _k = k.rendern(n)
        b = rm & mm & (alb.max(2) > 0)
        Ls = [R.lab(PZ.rueck(np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0])
              for s in R.SAATEN]
        e = {}
        for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p'):
            v = [R.masse(L, b, A0)[m] for L in Ls]
            w = float(np.mean([x for x in v if x is not None]))
            lo, hi = ziel[m]['ziel']
            e[m] = dict(wert=round(w, 2), ok=bool(w <= hi) if m == 'gross_eng_p' else bool(lo <= w <= hi))
        erg['staende'][nm] = e
    json.dump(erg, open(os.path.join(aus, 'ziel_ohne_krallen.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print(json.dumps(erg, indent=1, ensure_ascii=False))


if __name__ == '__main__':
    main()
