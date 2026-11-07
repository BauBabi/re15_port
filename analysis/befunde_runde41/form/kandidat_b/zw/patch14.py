s = open('b_alles.py', encoding='utf-8').read()
a = s.index("def durchgang(")
b = s.index("if __name__ == '__main__':")
s = s[:a] + '''def alle_sichten(fit, teile, w2):
    J = np.asarray(fit['J'], float)
    n = len(T.KNOCHEN)
    R1, T1 = BB.fk(np.zeros((n, 3)), J, T.ELTERN, 0.0)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    d1 = boden_dy(teile, R1, T1, J)
    d2 = boden_dy(teile, R2, T2, J)
    return {1: SN.Sicht(1, K1, O1, G1, R1, [t + [0, d1, 0] for t in T1], J), 2: SN.Sicht(2, K2, fit['lage2'], float(fit['gier2']), R2, [t + [0, d2, 0] for t in T2], J)}


def durchgang(fit_pfad, tag, ab='mesh', schritte=14, malen=None, runden=6):
    import b_fein as FN
    roh = BM.lade(fit_pfad)
    fit, rinne = BN.ergaenze(roh)
    z = 'zw/g%s' % tag
    os.makedirs(z, exist_ok=True)
    if ab == 'mesh':
        BM.baue(fit, z + '/mesh', kuppel_extra=rinne)
        ab = 'blender'
    if ab == 'blender':
        json.dump(BUDGET, open(z + '/budget.json', 'w'))
        r = subprocess.run([BLENDER, '-b', '-P', 'b_blender.py', '--', z + '/mesh', z + '/quad', z + '/budget.json'], capture_output=True, text=True, encoding='utf-8', errors='replace')
        for l in r.stdout.splitlines():
            if l.startswith(('Rueckfall', 'Traceback')) or 'Error' in l:
                print(l[:120])
        ab = 'snap'
    if ab == 'snap':
        pickle.dump(snap_lauf(fit, z + '/quad', schritte), open(z + '/snap.pkl', 'wb'))
    V, w2, fit['lage2'], fit['gier2'] = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
    torch.save(fit, z + '/fit.pt')
    teile = SN.stuecke(z + '/quad')
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
    for r_ in range(runden):
        V = {st['name']: st['V'].copy() for st in teile}
        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen if r_ == runden - 1 or True else None, snap=V, leise=r_ < runden - 1)
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0:
            break
    pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
    return form


''' + s[b:]
open('b_alles.py', 'w', encoding='utf-8').write(s)
s = open('b_lauf.py', encoding='utf-8').read()
s = s.replace("ruhe_w=None, theta=None, lage1=None, gier1=None, snap=None):", "ruhe_w=None, theta=None, lage1=None, gier1=None, snap=None, leise=False):")
s = s.replace("""    if pruefen:
        BB.pruefe(pfad, aus)
        zeile(aus)
""", """    if pruefen:
        BB.pruefe(pfad, aus)
        zeile(aus)
""")
open('b_lauf.py', 'w', encoding='utf-8').write(s)
