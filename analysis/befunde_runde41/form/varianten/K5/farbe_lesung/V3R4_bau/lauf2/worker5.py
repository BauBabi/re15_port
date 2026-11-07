"""Bauer V3 R4 Lauf 2: Koordinatensuche je Gruppe gegen RV1-RV4 (Flaechen-ID-Raster) + Ader/Ring-Bedingung im Texelraum.
Alle Gruppen werden je Schritt gemeinsam verschoben (eine Bewertung fuer alle; Gruppen liegen auf getrennten Pixeln), danach waehlt jede
Gruppe den besten ihrer Kandidaten nach ihrem eigenen Ziel (Zeilen laut ZIEL).
    python such5.py start.json aus.json [sweeps]"""
import sys, json, copy, math, time
B = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5'
sys.path.insert(0, B)
import such4 as Q
import numpy as np
Q.setze_f1(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/r4b/bau5/f1')
ZIEL = {'Schwanz': [(1, 'Schwanz')], 'Hinterbeine': [(1, 'Hinterbeine')], 'Rumpf Ruecken': [(1, 'Ruecken')], 'Rumpf Flanke': [(1, 'Flanke')],
        'Vorderbein r B1': [(1, 'Vorderbeine')], 'Brust': [(2, 'Ruecken')], 'Hals Bauch': [(2, 'Hals'), (2, 'Brust/Bauch')], 'Kopf': [(2, 'Kopf')],
        'Vorderbeine': [(2, 'Vorderbeine')]}
BILD1 = ('Schwanz', 'Hinterbeine', 'Rumpf Ruecken', 'Rumpf Flanke', 'Vorderbein r B1')
GRENZEN = dict(h=(-60.0, 60.0), C=(0.5, 80.0), beta=(-1.0, 3.0), q=(0.4, 1.0), kappa=(0.0, 1.0), a=(1.0, 2.0), g=(0.0, 1.0))
SCHRITT = dict(h=4.0, C=0.15, beta=0.3, q=0.1, kappa=0.2, a=0.1, g=0.15)    # C multiplikativ (log)


def grenz(g, k, x):
    lo, hi = GRENZEN[k]
    if k == 'a' and g in BILD1:
        return 1.0
    return float(min(hi, max(lo, x)))


def ziel_g(r, g):
    """Verletzung (normiert auf die Fensterbreite) + 0,05 x Abstand zur Mitte, ueber die bindenden Zeilen der Gruppe."""
    f = 0.0
    for b, rn in ZIEL[g]:
        for mk in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p'):
            v = r.get('B%d %s %s' % (b, rn, mk))
            if not v or not v['b']:
                continue
            lo, hi = v['lo'], v['hi']
            br = max(hi - lo, 1e-6)
            if mk == 'gross_eng_p':
                f += max(0.0, v['w'] - 0.9 * hi) / br
                continue
            rand = 0.12 * br                       # Reserve 12 % der Fensterbreite je Seite
            f += max(0.0, (lo + rand) - v['w']) / br + max(0.0, v['w'] - (hi - rand)) / br
            f += 0.02 * abs(v['w'] - 0.5 * (lo + hi)) / br
    return f


def ader_ok(par):
    """Texel: Ader <= Ring (L*, a*) je Gruppe (Nutzer: nie heller, nie roter)."""
    import cv2
    cl = Q.cluts(par)
    ker = np.ones((2 * Q.RING + 1, 2 * Q.RING + 1), np.uint8)
    px = Q.T3['pixel']
    o = {}
    for (g, s_), zz in Q.ZEILEN.items():
        _Lg, _C, _h, w = Q.K.eintrag_lab(Q.G4[s_])
        Ln = Q.K.KG.lab(Q.K.KG.wort_rgb(cl[zz]))
        gen = Q.GENUTZT[(g, s_)]
        ad = gen & (w[px] > 0)
        ha = gen & (w[px] == 0) & (np.array(Q.G4[s_])[px] != 0)
        ring = ha & cv2.dilate(ad.astype(np.uint8), ker).astype(bool)
        if ad.sum() < 20 or ring.sum() < 20:
            continue
        LL = Ln[px]
        o.setdefault(g, {})['texel s%d' % s_] = dict(L=(float(LL[ad, 0].mean()), float(LL[ring, 0].mean())), a=(float(LL[ad, 1].mean()), float(LL[ring, 1].mean())))
    bad = {}
    for g, d in o.items():
        for k, v in d.items():
            if k.startswith('texel') and (v['L'][0] > v['L'][1] or v['a'][0] > v['a'][1]):
                bad[g] = bad.get(g, 0) + max(v['L'][0] - v['L'][1], 0) + max(v['a'][0] - v['a'][1], 0)
    return bad


def bewerte(par):
    r, g, n = Q.bewerten(par)
    bad = ader_ok(par)
    return r, g, n, {gr: ziel_g(r, gr) + 0.5 * bad.get(gr, 0.0) for gr in ZIEL}


