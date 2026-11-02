s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""def alle_sichten(fit, teile, w2):""", """def knoten_schieben(fit, wahl):
    \"\"\"Einzelne Knoten in der Bildebene von Bild 1 nachstellen (dx, dy Pixel, Radiusfaktor). Handarbeit an Stellen, die der Fit nicht trifft.\"\"\"
    p = np.array(fit['pos'], float)
    r = np.array(fit['rad'], float)
    ni = {n: i for i, n in enumerate(fit['namen'])}
    f = K1['brennweite_bild_px']
    for k, (dx, dy, fr) in wahl.items():
        i = ni[k]
        w = welt(p[i])
        t = FK.tiefe(K1, w)
        w2 = w + K1['R'][0] * dx * t / f['fx'] + K1['R'][1] * dy * t / f['fy']
        p[i] = lokal(w2)
        r[i] *= fr
    fit = dict(fit)
    fit['pos'], fit['rad'] = p, r
    return fit


def alle_sichten(fit, teile, w2):""")
s = s.replace("""    roh = BM.lade(fit_pfad)
    fit, rinne = BN.ergaenze(roh)
    z = 'zw/g%s' % tag
    os.makedirs(z, exist_ok=True)
    if ab == 'mesh':
        BM.baue(fit, z + '/mesh', kuppel_extra=rinne)""", """    roh = BM.lade(fit_pfad)
    if wahl:
        roh = knoten_schieben(roh, wahl)
    fit, rinne = BN.ergaenze(roh)
    z = 'zw/g%s' % tag
    os.makedirs(z, exist_ok=True)
    if ab == 'mesh':
        BM.baue(fit, z + '/mesh', kuppel_extra=rinne)""")
s = s.replace("def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=6):", "def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=6, wahl=None):")
s = s.replace("    durchgang(a.fit, a.tag, a.ab, a.snap, malen=M2.malen)\n", "    durchgang(a.fit, a.tag, a.ab, a.snap, malen=M2.malen, wahl=WAHL)\n")
s = s.replace("BLENDER = 'C:/Program Files", "WAHL = dict(fl_t1=(16, 0, 1.0), fl_f=(6, 0, 0.9), nr_i1=(7, 4, 0.92))\nBLENDER = 'C:/Program Files")
open('b_alles.py', 'w', encoding='utf-8').write(s)
