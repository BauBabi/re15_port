s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""    V, w2 = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
""", """    V, w2 = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
    # Lage Bild 2 am fertigen Netz
    import b_lage2 as LG
    teile = SN.stuecke(z + '/quad')
    for st in teile:
        st['V'] = V[st['name']]
    J = np.asarray(fit['J'], float)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    d2 = boden_dy(teile, R2, T2, J)
    T2b = [t + [0, d2, 0] for t in T2]
    fit['lage2'], fit['gier2'] = LG.suche(teile, R2, T2b, J, fit['lage2'], round(fit['gier2']))
""")
open('b_alles.py', 'w', encoding='utf-8').write(s)
s = open('b_lauf.py', encoding='utf-8').read()
s = s.replace("    print('Flaechen', F, 'Knochen', len(form['knochen']), 'Lage2', kj['bild2'])\n", "    print('Flaechen', F, 'Knochen', len(form['knochen']), 'Lage2', kj['bild2'])\n    print(' '.join('%d:%d' % (i, len(m['dreiecke']) + len(m['vierecke'])) for i, m in enumerate(form['meshes'])))\n")
open('b_lauf.py', 'w', encoding='utf-8').write(s)
