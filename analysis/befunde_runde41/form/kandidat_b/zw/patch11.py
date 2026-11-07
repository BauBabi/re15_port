s = open('b_lage2.py', encoding='utf-8').read()
a = s.index('def suche(teile, R, Tt, J, lage0, g0')
s = s[:a] + '''def suche(teile, R, Tt, J, lage0, g0, schritte=(120, 60, 30, 15), g_schritte=(24, 12, 6, 3), w2=None, dy=0.0, gelenke=()):
    """Koordinatenabstieg ueber Lage, Gier und optional Winkel einzelner Knochen (Grad-Schritte 2, 1). Rueckgabe lage, gier[, w2]."""
    G = json.load(open(FORM + '/pruefung/grenzen.json', encoding='utf-8'))
    Z = cv2.imread(FORM + '/ziel_bild2.png', 0)
    kante = [((p[0], p[1]), G['b2.kante_%d' % (i + 1)]['grenze']) for i, p in enumerate(P.teil_b2('R')['kante_belegt'])]
    linien = [((222 + 4, 292), (298 - 4, 292)), ((232 + 4, 347), (285 - 4, 347)), ((340, 338), (364, 338)), ((408, 230), (408, 258)), ((413, 228), (413, 258))]
    lage = np.array(lage0, float)
    g = float(g0)
    w = None if w2 is None else np.array(w2, float)

    def wert(l, gg, ww=None):
        if ww is None:
            r, t = R, Tt
        else:
            r, t = BB.fk(ww, J, T.ELTERN, dy)
            u = max(((r[st['bone']] @ (st['V'] - J[st['bone']]).T).T + t[st['bone']])[:, 1].max() for st in teile)
            t = [q + [0, -u, 0] for q in t]
        s = SN.Sicht(2, K2, l, gg, r, t, J)
        return bewerte(s.maske(teile), Z, kante, linien)
    best, info = wert(lage, g, w)
    print('Start', round(best), info)
    for dl, dg in zip(schritte, g_schritte):
        besser = True
        while besser:
            besser = False
            for d in ((dl, 0), (-dl, 0), (0, dl), (0, -dl)):
                l = lage + [d[0], 0, d[1]]
                v, i = wert(l, g, w)
                if v < best:
                    best, info, lage, besser = v, i, l, True
            for d in (dg, -dg):
                v, i = wert(lage, g + d, w)
                if v < best:
                    best, info, g, besser = v, i, g + d, True
    if w is not None:
        for sch in (math.radians(3), math.radians(1.5)):
            besser = True
            while besser:
                besser = False
                for b in gelenke:
                    for ax in range(3):
                        for sg in (1, -1):
                            ww = w.copy()
                            ww[b, ax] += sg * sch
                            v, i = wert(lage, g, ww)
                            if v < best - 1e-6:
                                best, info, w, besser = v, i, ww, True
    print('Ende', round(best), info, 'Lage', np.round(lage).tolist(), 'Gier', g)
    return (lage, g) if w2 is None else (lage, g, w)
'''
open('b_lage2.py', 'w', encoding='utf-8').write(s)

s = open('b_alles.py', encoding='utf-8').read()
a = s.index("    s1, s2, T2b = sichten()\n    lage2, gier2 = LG.suche(teile, R2, T2b, J, lage2, gier2, schritte=(30, 15), g_schritte=(6, 3))\n")
b = s.index("def durchgang(")
s = s[:a] + """    for it in range(8):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.5, sigma=70.0)
    print('Snap B1 fein: %d Ecken, Mittel %.1f, max %.1f px' % b[1], flush=True)
    s1, s2, T2b = sichten()
    gl = [T.IDX[k] for k in ('fr_ober', 'fr_unter', 'nr_ober', 'nr_unter', 'nl_ober', 'fl_ober', 'fl_unter')]
    lage2, gier2, w2 = LG.suche(teile, R2, T2b, J, lage2, gier2, schritte=(30, 15), g_schritte=(6, 3), w2=w2, dy=fit['dy2'], gelenke=gl)
    return {st['name']: st['V'] for st in teile}, w2, lage2, gier2


""" + s[b:]
open('b_alles.py', 'w', encoding='utf-8').write(s)

s = open('b_mal2.py', encoding='utf-8').read()
s = s.replace("    f = M.rauschen3(q + warp, 135.0, 21) + 0.45 * M.rauschen3(q + warp, 62.0, 22)\n", "    f = M.rauschen3(q + warp, 150.0, 21) + 0.22 * M.rauschen3(q + warp, 80.0, 22)\n")
s = s.replace("    furche = np.abs(f) < 0.055\n    grat = (f > 0.055) & (f < 0.135)\n", "    furche = np.abs(f) < 0.062\n    grat = (f > 0.062) & (f < 0.150)\n")
s = s.replace("    stufe = np.where(np.abs(f) < 0.022, 0, stufe)\n", "    stufe = np.where(np.abs(f) < 0.028, 0, stufe)\n")
s = s.replace("    kn = ins > 0.36\n", "    kn = ins > 0.46\n")
s = s.replace("    h1, h2, hid, hv = worley(p + warp * 0.6, 92.0, 5)\n", "    h1, h2, hid, hv = worley(p + warp * 0.6, 110.0, 5)\n")
s = s.replace("    stufe = np.where(kn & kern & ((-hv @ oben) / 92.0 > 0.07), 5, stufe)\n", "    stufe = np.where(kn & kern & ((-hv @ oben) / 110.0 > 0.07), 5, stufe)\n")
s = s.replace("    ader = (a_ < 0.020) & masse\n", "    ader = (a_ < 0.026) & masse\n")
s = s.replace("    stufe = np.where((a_ >= 0.020) & (a_ < 0.034) & masse, np.minimum(stufe, 1), stufe)\n", "    stufe = np.where((a_ >= 0.026) & (a_ < 0.042) & masse, np.minimum(stufe, 1), stufe)\n")
open('b_mal2.py', 'w', encoding='utf-8').write(s)
