s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("WAHL = dict(fl_t1=(16, 0, 1.0), fl_f=(6, 0, 0.9), nr_i1=(7, 4, 0.92))", "WAHL = dict(fl_t1=(16, 0, 1.0), fl_f=(8, 0, 0.9), fl_a=(9, 0, 0.92), fl_k=(5, 0, 1.0), nr_i1=(7, 4, 0.92))")
s = s.replace("""        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
""", """        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        w_ = wertung(z + '/v')
        if beste is None or w_ > beste[0]:
            beste = (w_, {k: v.copy() for k, v in V.items()})
            print('neue Bestmarke', w_, flush=True)
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
""")
s = s.replace("""    form = None
    import b_ruhe""", """    form = None
    beste = None
    import b_ruhe""")
s = s.replace("""    return form


if __name__""", """    if beste is not None:
        V = beste[1]
        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen, snap=V, ruhe_w=ruhe_w)
    return form


if __name__""")
s = s.replace("""def knoten_schieben(fit, wahl):""", """def wertung(aus):
    R = json.load(open(aus + '/bericht.json', encoding='utf-8'))
    z = 0
    for n in (1, 2):
        B = R['bild%d' % n]
        for k in ('punkte', 'flaechen', 'zeichnung', 'verbote'):
            z += sum(1 for x in B[k] if x['zaehlt'] and x['ok'])
    return (z, round(R['bild1']['ncc_weiss'] + R['bild2']['ncc_weiss'], 3))


def knoten_schieben(fit, wahl):""")
s = s.replace("if n == 0 and r_ >= 1:\n            break", "if n == 0 and r_ >= 2:\n            break")
open('b_alles.py', 'w', encoding='utf-8').write(s)
s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""                    ziehe(teile, s, (311.0, 349.0), (4.0, 0.0), sigma=80.0, radius=26.0, nur=['k02', 'k01'])
""", "")
open('b_fein.py', 'w', encoding='utf-8').write(s)
